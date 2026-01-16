#include "002_Components/MonacoEditor.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>
#include <Wt/WLogger.h>
#include <Wt/WResource.h>
#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Wt/WServer.h>

/**
 * @brief Custom WResource for serving string content
 * 
 * This resource serves dynamic string content (e.g., editor text) via HTTP.
 * It handles requests by returning the stored content with appropriate MIME type.
 */
class StringContentResource : public Wt::WResource {
public:
    /**
     * @brief Constructor
     * @param content The string content to serve
     * @param mimeType MIME type for the content (default: text/plain)
     */
    StringContentResource(const std::string& content, const std::string& mimeType = "text/plain; charset=utf-8")
        : content_(content), mimeType_(mimeType) {
    }
    
    /**
     * @brief Handles HTTP requests for this resource
     * @param request The HTTP request
     * @param response The HTTP response to populate
     */
    void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override {
        response.setMimeType(mimeType_);
        response.out() << content_;
    }

private:
    std::string content_;   ///< The content to serve
    std::string mimeType_;  ///< The MIME type for the content
};
#include <filesystem>

MonacoEditor::MonacoEditor(std::string language)
    : js_signal_text_changed_(this, "editorTextChanged"),
      current_text_(""),
      unsaved_text_("")
{
    wApp->log("debug") << "MonacoEditor::MonacoEditor(std::string language):" << language;
    setLayoutSizeAware(true);
    setCanReceiveFocus(true);
    setMinimumSize(Wt::WLength(1, Wt::LengthUnit::Pixel), Wt::WLength(1, Wt::LengthUnit::Pixel));

    // setMaximumSize(Wt::WLength::Auto, Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    // setStyleClass("h-fill");

    js_signal_text_changed_.connect(this, &MonacoEditor::editorTextChanged);
    doJavaScript(R"(require.config({ paths: { 'vs': 'static/monaco/vs' } });)");
    editor_js_var_name_ = language + Wt::WRandom::generateId() + "_editor";
    
    resize(Wt::WLength::Auto, Wt::WLength::Auto);
    
    std::string initializer =
        R"(
        require(['vs/editor/editor.main'], function () {
            window.)" + editor_js_var_name_ + R"(_current_text = `)" + current_text_ + R"(`;
            window.)" + editor_js_var_name_ + R"( = monaco.editor.create(document.getElementById(')" + id() + R"('), {
                language: ')" + language + R"(',
                theme: 'vs-dark',
                // wordWrap: 'on',
                // lineNumbers: 'on',
                tabSize: 2,
                insertSpaces: false,
                detectIndentation: false,
                trimAutoWhitespace: false,
                lineEnding: '\n',
                minimap: { enabled: false },
                automaticLayout: true,
                scrollbar: {
                    vertical: 'auto',    // Show vertical scrollbar only if needed
                    horizontal: 'auto',  // Show horizontal scrollbar only if needed
                    handleMouseWheel: true
                },
                scrollBeyondLastLine: false
            });

            window.)" + editor_js_var_name_ + R"(.onDidChangeModelContent(function (event) {
                if (window.)" + editor_js_var_name_ + R"(_current_text !== window.)" + editor_js_var_name_ + R"(.getValue()) {
                    window.)" + editor_js_var_name_ + R"(_current_text = window.)" + editor_js_var_name_ + R"(.getValue();
                    Wt.emit(')" + id() + R"(', 'editorTextChanged', window.)" + editor_js_var_name_ + R"(.getValue());
                }
            });
            window.addEventListener('resize', function() {
                var ed = window.)" + editor_js_var_name_ + R"(;
                if(ed) {
                    ed.layout();
                }
            });
            window.)" + editor_js_var_name_ + R"(.getDomNode().addEventListener('keydown', function(e) {
                if ((e.ctrlKey || e.metaKey)) {
                    if (e.key === 's') {
                        e.preventDefault();
                    }
                }
                if (e.altKey && e.key === 'x') {
                    const currentMinimap = window.)" + editor_js_var_name_ + R"(.getOptions().get(monaco.editor.EditorOption.minimap).enabled;
                    window.)" + editor_js_var_name_ + R"(.updateOptions({ minimap: { enabled: !currentMinimap } });
                }
                if (e.altKey && e.key === 'z') {
                    e.preventDefault();
                    const currentWordWrap = window.)" + editor_js_var_name_ + R"(.getOptions().get(monaco.editor.EditorOption.wordWrap);
                    const newWordWrap = currentWordWrap === 'off' ? 'on' : 'off';
                    window.)" + editor_js_var_name_ + R"(.updateOptions({ wordWrap: newWordWrap });
                }
            });
        });
    )";

    setJavaScriptMember("something", initializer);

    keyWentDown().connect([=](Wt::WKeyEvent e){ 
        Wt::WApplication::instance()->globalKeyWentDown().emit(e); // Emit the global key event
        if (e.modifiers().test(Wt::KeyboardModifier::Control))
        {
            if (e.key() == Wt::Key::S)
            {
                if(unsavedChanges()){
                    save_file_signal_.emit(unsaved_text_);
                }
            }
        } 
    });
}

void MonacoEditor::layoutSizeChanged(int width, int height)
{
    wApp->log("debug") << "MonacoEditor::layoutSizeChanged(int width, int height): " << width << "x" << height;
    resetLayout();
    if(width > 1){
        width_changed_.emit(Wt::WString(std::to_string(width)));
    }
}


void MonacoEditor::editorTextChanged(std::string text)
{
    wApp->log("debug") << "MonacoEditor::editorTextChanged(std::string text): text---\n" << text << "\n---";
    unsaved_text_ = text;
    available_save_.emit();
}

void MonacoEditor::textSaved()
{
    wApp->log("debug") << "MonacoEditor::textSaved()";
    current_text_ = unsaved_text_;
    available_save_.emit();
}

void MonacoEditor::setReadOnly(bool readOnly) {
    wApp->log("debug") << "MonacoEditor::setReadOnly(bool readOnly): " << (readOnly ? "true" : "false");
    // Try repeatedly until the editor instance is ready to ensure the flag flips.
    doJavaScript(R"(
        (function(){
            var attempts = 0;
            var setRO = function(){
                var ed = window.)" + editor_js_var_name_ + R"(;
                if(ed){
                    ed.updateOptions({ readOnly: )" + std::string(readOnly ? "true" : "false") + R"( });
                } else if(attempts < 10) {
                    attempts++;
                    setTimeout(setRO, attempts === 1 ? 0 : 100);
                }
            };
            setRO();
        })();
    )");
}

bool MonacoEditor::unsavedChanges()
{
    wApp->log("debug") << "MonacoEditor::unsavedChanges()";
    if (current_text_.compare(unsaved_text_) == 0)
    {
        return false;
    }
    return true;
}

void MonacoEditor::resetLayout()
{
    wApp->log("debug") << "MonacoEditor::resetLayout()";
    doJavaScript(
        R"(
            (function(){
                var attempts = 0;
                var maxAttempts = 20;
                var doLayout = function(){
                    var ed = window.)" + editor_js_var_name_ + R"(;
                    if(ed){
                        ed.layout();
                    } else if(attempts < maxAttempts) {
                        attempts++;
                        setTimeout(doLayout, attempts === 1 ? 0 : 200);
                    }
                };
                doLayout();
            })();
        )");
}

void MonacoEditor::setContent(const std::string& content)
{
    wApp->log("debug") << "MonacoEditor::setContent(const std::string& content): " << content;
    
    // Create a WResource for serving the content
    auto resource = std::make_shared<StringContentResource>(content);
    
    // Generate a unique path for this resource
    std::string resourcePath = "/monaco_content/" + Wt::WRandom::generateId();
    
    // Register the resource with the server
    Wt::WServer::instance()->addResource(resource, resourcePath);
    
    doJavaScript(
        R"(
            (function(){
                var attempts = 0;
                var maxAttempts = 20;
                var setContent = function(){
                    var ed = window.)" + editor_js_var_name_ + R"(;
                    if(ed){
                        var xhr = new XMLHttpRequest();
                        xhr.onreadystatechange = function() {
                            if (xhr.readyState === 4 && xhr.status === 200) {
                                window.)" + editor_js_var_name_ + R"(_current_text = xhr.responseText;
                                window.)" + editor_js_var_name_ + R"(.setValue(xhr.responseText);
                            } else if (xhr.readyState === 4) {
                                console.error('Failed to load content: HTTP ' + xhr.status);
                            }
                        };
                        xhr.open('GET', ')" + resourcePath + R"(', true);
                        xhr.overrideMimeType('text/plain; charset=utf-8');
                        xhr.send();
                    } else if(attempts < maxAttempts) {
                        attempts++;
                        setTimeout(setContent, attempts === 1 ? 0 : 200);
                    } else {
                        console.error("Editor instance )" + editor_js_var_name_ + R"( is not initialized after " + maxAttempts + " attempts");
                    }
                };
                setContent();
            })();
        )");
    
    current_text_ = content;
    unsaved_text_ = content;
    resetLayout();
}

void MonacoEditor::setContentPreserveCursor(const std::string& content)
{
    wApp->log("debug") << "MonacoEditor::setContentPreserveCursor(const std::string& content): " << content;
    
    // Create a WResource for serving the content
    auto resource = std::make_shared<StringContentResource>(content);
    
    // Generate a unique path for this resource
    std::string resourcePath = "/monaco_content/" + Wt::WRandom::generateId();
    
    // Register the resource with the server
    Wt::WServer::instance()->addResource(resource, resourcePath);
    
    doJavaScript(
        R"(
            (function(){
                var attempts = 0;
                var maxAttempts = 20;
                var setContent = function(){
                    var ed = window.)" + editor_js_var_name_ + R"(;
                    if(ed){
                        // Save current cursor position
                        var cursorPos = ed.getPosition();
                        
                        var xhr = new XMLHttpRequest();
                        xhr.onreadystatechange = function() {
                            if (xhr.readyState === 4 && xhr.status === 200) {
                                window.)" + editor_js_var_name_ + R"(_current_text = xhr.responseText;
                                window.)" + editor_js_var_name_ + R"(.setValue(xhr.responseText);
                                
                                // Restore cursor position if it's valid for the new content
                                if (cursorPos && cursorPos.lineNumber <= ed.getModel().getLineCount()) {
                                    ed.setPosition(cursorPos);
                                    ed.revealPositionInCenter(cursorPos);
                                }
                            } else if (xhr.readyState === 4) {
                                console.error('Failed to load content: HTTP ' + xhr.status);
                            }
                        };
                        xhr.open('GET', ')" + resourcePath + R"(', true);
                        xhr.overrideMimeType('text/plain; charset=utf-8');
                        xhr.send();
                    } else if(attempts < maxAttempts) {
                        attempts++;
                        setTimeout(setContent, attempts === 1 ? 0 : 200);
                    } else {
                        console.error("Editor instance )" + editor_js_var_name_ + R"( is not initialized after " + maxAttempts + " attempts");
                    }
                };
                setContent();
            })();
        )");
    
    current_text_ = content;
    unsaved_text_ = content;
    resetLayout();
}
void MonacoEditor::setDarkTheme(bool dark)
{
    wApp->log("debug") << "MonacoEditor::setDarkTheme(bool dark): " << (dark ? "true" : "false");
    wApp->doJavaScript(R"(
        (function() {
            var interval = setInterval(function() {
                if (window.monaco) {
                    clearInterval(interval);
                    monaco.editor.setTheme(')" + std::string(dark ? "vs-dark" : "vs-light") + R"(');
                }
            }, 100);
        })();
    )");
}


std::string MonacoEditor::getFileText(std::string filePath)
{
    wApp->log("debug") << "MonacoEditor::getFileText(std::string filePath): " << filePath;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        Wt::log("error") << "Failed to read file: " << filePath;
        return "!Failed to read file!";
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    Wt::WString file_content_wt = Wt::WString::fromUTF8(file_content);
    return file_content;
}

void MonacoEditor::saveFile()
{
    wApp->log("debug") << "MonacoEditor::saveFile()";
    // Save the unsaved text to the file system
    if (unsaved_text_.empty())
    {
        Wt::log("info") << "No unsaved text to save.";
        return;
    }
    std::ofstream file(selected_file_path_);
    if (!file.is_open())
    {
        Wt::log("error") << "Failed to open file for writing: " << selected_file_path_;
        return;
    }
    file << unsaved_text_;
    file.close();
    Wt::log("info") << "File path: " << selected_file_path_ << " saved successfully.";
}

void MonacoEditor::setLineWrap(bool wrap)
{
    wApp->log("debug") << "MonacoEditor::setLineWrap(bool wrap): " << (wrap ? "true" : "false");
    doJavaScript(
        R"(
            (function(){
                var attempts = 0;
                var maxAttempts = 10;
                var setWrap = function(){
                    var ed = window.)" + editor_js_var_name_ + R"(;
                    if(ed){
                        ed.updateOptions({ wordWrap: ')" + std::string(wrap ? "on" : "off") + R"(' });
                    } else if(attempts < maxAttempts) {
                        attempts++;
                        setTimeout(setWrap, attempts === 1 ? 0 : 100);
                    }
                };
                setWrap();
            })();
        )");
}

void MonacoEditor::toggleMinimap()
{
    wApp->log("debug") << "MonacoEditor::toggleMinimap()";
    doJavaScript(
        R"(
            (function(){
                var attempts = 0;
                var maxAttempts = 10;
                var toggleMap = function(){
                    var ed = window.)" + editor_js_var_name_ + R"(;
                    if(ed){
                        const currentMinimap = ed.getOptions().get(monaco.editor.EditorOption.minimap).enabled;
                        ed.updateOptions({ minimap: { enabled: !currentMinimap } });
                    } else if(attempts < maxAttempts) {
                        attempts++;
                        setTimeout(toggleMap, attempts === 1 ? 0 : 100);
                    }
                };
                toggleMap();
            })();
        )");
}

void MonacoEditor::setLineNumber(bool show)
{
    wApp->log("debug") << "MonacoEditor::setLineNumber(bool show): " << (show ? "true" : "false");
    doJavaScript(
        R"(
            (function(){
                var attempts = 0;
                var maxAttempts = 10;
                var setLineNum = function(){
                    var ed = window.)" + editor_js_var_name_ + R"(;
                    if(ed){
                        ed.updateOptions({ lineNumbers: ')" + std::string(show ? "on" : "off") + R"(' });
                    } else if(attempts < maxAttempts) {
                        attempts++;
                        setTimeout(setLineNum, attempts === 1 ? 0 : 100);
                    }
                };
                setLineNum();
            })();
        )");
}
