#include "002_Components/MonacoEditor.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>
#include <Wt/WLogger.h>
#include <fstream>
#include <iostream>

MonacoEditor::MonacoEditor(std::string language)
    : js_signal_text_changed_(this, "editorTextChanged"),
      current_text_(""),
      unsaved_text_("")
{
    wApp->log("debug") << "MonacoEditor::MonacoEditor(std::string language):" << language;
    setLayoutSizeAware(true);
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
                wordWrap: 'on',
                lineNumbers: 'on',
                tabSize: 4,
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
                    setTimeout(setRO, 100);
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

void MonacoEditor::setEditorText(std::string resourcePath)
{
    wApp->log("debug") << "MonacoEditor::setEditorText(std::string resourcePath): " << resourcePath;
    resetLayout();
    auto resourcePathUrl = resourcePath + "?v=" + Wt::WRandom::generateId();
    doJavaScript(
        R"(
            setTimeout(function() {
                if(!window.)" + editor_js_var_name_ + R"() {
                    setTimeout(function() {
                        console.log("Setting editor text to: )" + resourcePathUrl + R"(");
                        if (window.)" + editor_js_var_name_ + R"() {
                            fetch(')" + resourcePathUrl + R"(')
                            .then(response => response.text())
                            .then(css => {
                                window.)" + editor_js_var_name_ + R"(_current_text = css;
                                window.)" + editor_js_var_name_ + R"(.setValue(css);
                            });
                        } else {
                            console.error("Editor instance is stil l not initialized.");
                        }
                    }, 2000);
                    return;
                }
                console.log("Setting editor text to: )" + resourcePathUrl + R"(");
                fetch(')" + resourcePathUrl + R"(')
                    .then(response => response.text())
                    .then(css => {
                        window.)" + editor_js_var_name_ + R"(_current_text = css;
                        window.)" + editor_js_var_name_ + R"(.setValue(css);
                    });
            }, 10); // Delay to ensure the editor is ready
        )");
    current_text_ = getFileText(resourcePath);
    unsaved_text_ = current_text_;
    selected_file_path_ = resourcePath;
    resetLayout();
}

void MonacoEditor::resetLayout()
{
    wApp->log("debug") << "MonacoEditor::resetLayout()";
    doJavaScript("setTimeout(function() { window." + editor_js_var_name_ + ".layout() }, 200);");
}

void MonacoEditor::setContent(const std::string& content)
{
    wApp->log("debug") << "MonacoEditor::setContent(const std::string& content): " << content;
    // Set content directly via JavaScript without using the resource system
    // This avoids resource path conflicts when editing the same post multiple times
    std::string escapedContent = content;
    
    // Escape backslashes and quotes for JavaScript string
    size_t pos = 0;
    while ((pos = escapedContent.find('\\', pos)) != std::string::npos) {
        escapedContent.replace(pos, 1, "\\\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = escapedContent.find('"', pos)) != std::string::npos) {
        escapedContent.replace(pos, 1, "\\\"");
        pos += 2;
    }
    pos = 0;
    while ((pos = escapedContent.find('\n', pos)) != std::string::npos) {
        escapedContent.replace(pos, 1, "\\n");
        pos += 2;
    }
    pos = 0;
    while ((pos = escapedContent.find('\r', pos)) != std::string::npos) {
        escapedContent.replace(pos, 1, "\\r");
        pos += 2;
    }
    
    doJavaScript(
        R"(
            setTimeout(function() {
                if (window.)" + editor_js_var_name_ + R"() {
                    var content = ")" + escapedContent + R"(";
                    window.)" + editor_js_var_name_ + R"(_current_text = content;
                    window.)" + editor_js_var_name_ + R"(.setValue(content);
                } else {
                    console.error("Editor instance )" + editor_js_var_name_ + R"( is not yet initialized");
                }
            }, 100);
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

void MonacoEditor::toggleLineWrap()
{
    wApp->log("debug") << "MonacoEditor::toggleLineWrap()";
    doJavaScript(R"(
        setTimeout(function() {
            if (window.)" + editor_js_var_name_ + R"() {
                const currentWordWrap = window.)" + editor_js_var_name_ + R"(.getOptions().get(monaco.editor.EditorOption.wordWrap);
                const newWordWrap = currentWordWrap === 'off' ? 'on' : 'off';
                window.)" + editor_js_var_name_ + R"(.updateOptions({ wordWrap: newWordWrap });
            }
        }, 20);
    )");
}
void MonacoEditor::toggleMinimap()
{
    wApp->log("debug") << "MonacoEditor::toggleMinimap()";
    doJavaScript(R"(
        setTimeout(function() {
            if (window.)" + editor_js_var_name_ + R"() {
                const currentMinimap = window.)" + editor_js_var_name_ + R"(.getOptions().get(monaco.editor.EditorOption.minimap).enabled;
                window.)" + editor_js_var_name_ + R"(.updateOptions({ minimap: { enabled: !currentMinimap } });
            }
        }, 100);
    )");
}

void MonacoEditor::setLineNumber(bool show)
{
    wApp->log("debug") << "MonacoEditor::setLineNumber(bool show): " << (show ? "true" : "false");
    doJavaScript(R"(
        setTimeout(function() {
            if (window.)" + editor_js_var_name_ + R"() {
                window.)" + editor_js_var_name_ + R"(.updateOptions({ lineNumbers: ')" + std::string(show ? "on" : "off") + R"(' });
            }
        }, 100);
    )");
}
