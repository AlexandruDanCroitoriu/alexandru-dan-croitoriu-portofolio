#include "005_PersonalGalery/topics/ComponentsTopic.h"
#include "005_PersonalGalery/DeferredWidget.h"
#include "002_Components/MonacoEditor.h"

#include <Wt/WApplication.h>

#include <Wt/WMenu.h>
#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>

ComponentsTopic::ComponentsTopic()
{
}

void ComponentsTopic::populateSubMenu(Wt::WMenu* menu)
{
    menu->addItem("Monaco Editor",
                  deferCreate([this]{ return monacoEditorDemo(); }));
}

std::unique_ptr<Wt::WWidget> ComponentsTopic::monacoEditorDemo()
{
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("max-w-5xl mx-auto space-y-6");

    // Title
    auto title = container->addNew<Wt::WText>("<h2 class='text-3xl font-bold text-gray-800 mb-4'>Monaco Editor</h2>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Description
    auto description = container->addNew<Wt::WText>(
        "<p class='text-gray-700 leading-relaxed mb-6'>"
        "A feature-rich code editor component integrated with Wt, providing syntax highlighting, customizable themes, "
        "line wrapping, minimap toggle, read-only mode, and file operations. The editor supports multiple programming languages and "
        "offers a professional development experience with a familiar, modern interface."
        "</p>");
    description->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Example section title
    auto exampleTitle = container->addNew<Wt::WText>("<h3 class='font-semibold text-gray-800'>Example</h3>");
    exampleTitle->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    exampleTitle->addStyleClass("mb-0");

    // Example container with border
    auto exampleContainer = container->addNew<Wt::WContainerWidget>();
    exampleContainer->addStyleClass("border border-gray-300 rounded-lg bg-white mb-6");

    // Control panel above editor
    auto controlPanel = exampleContainer->addNew<Wt::WContainerWidget>();
    controlPanel->addStyleClass("flex flex-wrap gap-3 pt-4 px-4 py-3 bg-gray-100 rounded-lg border border-gray-200");

    // Theme toggle button
    auto themeBtn = controlPanel->addNew<Wt::WPushButton>("🌙 Dark Mode");
    themeBtn->addStyleClass("px-4 py-2 bg-slate-700 hover:bg-slate-800 text-white rounded-lg text-sm font-semibold transition duration-200 shadow-md hover:shadow-lg");
    
    // Read-only toggle button
    auto readOnlyBtn = controlPanel->addNew<Wt::WPushButton>("🔒 Read-Only");
    readOnlyBtn->addStyleClass("px-4 py-2 bg-slate-700 hover:bg-slate-800 text-white rounded-lg text-sm font-semibold transition duration-200 shadow-md hover:shadow-lg");
    
    // Line wrap toggle button
    auto lineWrapBtn = controlPanel->addNew<Wt::WPushButton>("↳ Line Wrap");
    lineWrapBtn->addStyleClass("px-4 py-2 bg-slate-700 hover:bg-slate-800 text-white rounded-lg text-sm font-semibold transition duration-200 shadow-md hover:shadow-lg");
    
    // Minimap toggle button
    auto minimapBtn = controlPanel->addNew<Wt::WPushButton>("📊 Minimap");
    minimapBtn->addStyleClass("px-4 py-2 bg-slate-700 hover:bg-slate-800 text-white rounded-lg text-sm font-semibold transition duration-200 shadow-md hover:shadow-lg");

    // Save button / status indicator
    auto saveBtn = controlPanel->addNew<Wt::WPushButton>("✓ Saved");
    saveBtn->addStyleClass("px-4 py-2 bg-gray-500 text-white rounded-lg text-sm font-semibold transition duration-200 opacity-50 cursor-not-allowed");
    saveBtn->setEnabled(false); // Initially disabled since no changes

    // Live editor instance
    auto editor = exampleContainer->addNew<MonacoEditor>("css");
    editor->setHeight(Wt::WLength(400, Wt::LengthUnit::Pixel));
    editor->setReadOnly(false); // ensure unlocked by default

    // Theme toggle
    auto darkMode = std::make_shared<bool>(false);
    themeBtn->clicked().connect([themeBtn, darkMode]() {
        *darkMode = !*darkMode;
        wApp->log("debug") << "Toggling theme, now darkMode=" << *darkMode;
        MonacoEditor::setDarkTheme(*darkMode);
        themeBtn->setText(*darkMode ? "☀️ Light Mode" : "🌙 Dark Mode");
        themeBtn->removeStyleClass(*darkMode ? "bg-slate-700 hover:bg-slate-800" : "bg-amber-700 hover:bg-amber-800");
        themeBtn->addStyleClass(*darkMode ? "bg-amber-700 hover:bg-amber-800" : "bg-slate-700 hover:bg-slate-800");
    });

    // Read-only toggle
    auto isReadOnly = std::make_shared<bool>(false);
    readOnlyBtn->clicked().connect([readOnlyBtn, editor, isReadOnly]() {
        *isReadOnly = !*isReadOnly;
        editor->setReadOnly(*isReadOnly);
        if (!*isReadOnly) {
            editor->resetLayout(); // ensure unlock applies immediately
        }

        const std::string base =
            "px-4 py-2 text-white rounded-lg text-sm font-semibold transition duration-200 shadow-md hover:shadow-lg ";
        const std::string onCls = "bg-red-700 hover:bg-red-800";
        const std::string offCls = "bg-slate-700 hover:bg-slate-800";
        readOnlyBtn->setStyleClass(base + (*isReadOnly ? onCls : offCls));
        readOnlyBtn->setText(*isReadOnly ? "🔓 Edit" : "🔒 Read-Only");
    });

    // Line wrap toggle
    auto lineWrapOn = std::make_shared<bool>(true);
    lineWrapBtn->clicked().connect([lineWrapBtn, editor, lineWrapOn]() {
        *lineWrapOn = !*lineWrapOn;
        editor->toggleLineWrap();
        lineWrapBtn->removeStyleClass(*lineWrapOn ? "bg-slate-700 hover:bg-slate-800" : "bg-emerald-700 hover:bg-emerald-800");
        lineWrapBtn->addStyleClass(*lineWrapOn ? "bg-emerald-700 hover:bg-emerald-800" : "bg-slate-700 hover:bg-slate-800");
        lineWrapBtn->setText(*lineWrapOn ? "↳ Line Wrap" : "→ No Wrap");
    });

    // Minimap toggle
    auto minimapOn = std::make_shared<bool>(true);
    minimapBtn->clicked().connect([minimapBtn, editor, minimapOn]() {
        *minimapOn = !*minimapOn;
        editor->toggleMinimap();
        minimapBtn->removeStyleClass(*minimapOn ? "bg-slate-700 hover:bg-slate-800" : "bg-indigo-700 hover:bg-indigo-800");
        minimapBtn->addStyleClass(*minimapOn ? "bg-indigo-700 hover:bg-indigo-800" : "bg-slate-700 hover:bg-slate-800");
        minimapBtn->setText(*minimapOn ? "📊 Minimap" : "📋 No Minimap");
    });

    // Monitor unsaved changes and update button appearance
    editor->availableSave().connect([saveBtn, editor]() {
        std::string currentText = editor->getUnsavedText();
        std::string savedText = editor->getSavedText();
        if (currentText != savedText) {
            saveBtn->setText("💾 Save");
            saveBtn->setStyleClass("px-4 py-2 bg-orange-700 hover:bg-orange-800 text-white rounded-lg text-sm font-semibold transition duration-200 shadow-md hover:shadow-lg");
            saveBtn->setEnabled(true);
        } else {
            saveBtn->setText("✓ Saved");
            saveBtn->setStyleClass("px-4 py-2 bg-gray-500 text-white rounded-lg text-sm font-semibold transition duration-200 opacity-50 cursor-not-allowed");
            saveBtn->setEnabled(false);
        }
    });

    // Save button click handler
    saveBtn->clicked().connect([saveBtn, editor]() {
        if (editor->getUnsavedText() != editor->getSavedText()) {
            editor->textSaved();
            saveBtn->setText("✓ Saved");
            saveBtn->setStyleClass("px-4 py-2 bg-gray-500 text-white rounded-lg text-sm font-semibold transition duration-200 opacity-50 cursor-not-allowed");
            saveBtn->setEnabled(false);
        }
    });

    // Source code section
    auto sourceTitle = container->addNew<Wt::WText>("<h4 class='font-semibold text-gray-800'>Code Example</h4>");
    sourceTitle->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    sourceTitle->addStyleClass("mb-0");
    auto sourceCode = container->addNew<Wt::WText>(
        "<div class='bg-gray-800 border border-gray-700 rounded-lg'>"
        "<pre class='p-4 overflow-x-auto text-sm text-gray-100'>"
        "<span style='color: #d19a66;'>#include</span> &lt;<span style='color: #56b6c2;'>Wt/WContainerWidget.h</span>&gt;\n"
        "<span style='color: #d19a66;'>#include</span><span style='color: #56b6c2;'>\"002_Components/MonacoEditor.h\"</span>\n\n"
        "<span style='color: #61afef;'>auto</span> editor = container-&gt;<span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>MonacoEditor</span>&gt;(<span style='color: #98c379;'>\"css\"</span>);\n"
        "editor-&gt;<span style='color: #61afef;'>setHeight</span>(<span style='color: #e06c75;'>Wt::WLength</span>(<span style='color: #d19a66;'>400</span>, <span style='color: #e06c75;'>Wt::LengthUnit::Pixel</span>));\n\n"
        "<span style='color: #7d8590;'>// Features:</span>\n"
        "editor-&gt;<span style='color: #61afef;'>setReadOnly</span>(<span style='color: #d19a66;'>false</span>);\n"
        "editor-&gt;<span style='color: #61afef;'>toggleLineWrap</span>();\n"
        "editor-&gt;<span style='color: #61afef;'>toggleMinimap</span>();\n"
        "<span style='color: #e06c75;'>MonacoEditor</span>::<span style='color: #61afef;'>setDarkTheme</span>(<span style='color: #d19a66;'>true</span>);"
        "</pre>"
        "</div>"
    );
    sourceCode->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    return container;
}
