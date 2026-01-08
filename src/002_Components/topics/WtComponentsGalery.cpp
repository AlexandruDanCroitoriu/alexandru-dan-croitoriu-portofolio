#include "002_Components/topics/WtComponentsGalery.h"
#include "003_Navigation/DeferredWidget.h"

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>
#include <Wt/WRadioButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WDoubleSpinBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WColorPicker.h>
#include <Wt/WTemplate.h>

WtComponentsGalery::WtComponentsGalery()
{
    wApp->log("debug") << "WtComponentsGalery::WtComponentsGalery()";
}

std::unique_ptr<Wt::WWidget> WtComponentsGalery::formInputsDemo()
{
    wApp->log("debug") << "WtComponentsGalery::formInputsDemo()";
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("w-full max-w-5xl mx-auto space-y-2 p-6");

    // Title
    auto title = container->addNew<Wt::WTemplate>("<h2 class='text-3xl font-bold text-gray-800 mb-2'>Wt Library Components / Widgets</h2>");
    // title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Description
    auto description = container->addNew<Wt::WTemplate>(
        "<p class='text-gray-700 leading-relaxed mb-2'>"
        "A comprehensive showcase of various Wt library components and widgets, demonstrating their usage, features, "
        "and customization options within a web application context."
        "</p>");

    // Example container with border
    auto exampleContainer = container->addNew<Wt::WContainerWidget>();

    createLineEditExample(exampleContainer);
    createTextAreaExample(exampleContainer);

    return container;
}

void WtComponentsGalery::createLineEditExample(Wt::WContainerWidget* wrapper)
{
    // Example section title
    auto exampleTitle = wrapper->addNew<Wt::WTemplate>("<h3 class='font-semibold text-gray-800'>Basic Example</h3>");
    auto contentWrapper = wrapper->addNew<Wt::WContainerWidget>();
    contentWrapper->addStyleClass("border border-gray-300 rounded-lg bg-white mb-6 p-6 space-y-6");

    auto lineEditLabel = contentWrapper->addNew<Wt::WLabel>("Input Label");
    auto textLineEdit = contentWrapper->addNew<Wt::WLineEdit>();
    textLineEdit->setPlaceholderText("Line edit input...");
    lineEditLabel->setBuddy(textLineEdit);

    // Source code section
    auto sourceTitle = contentWrapper->addNew<Wt::WTemplate>("<h4 class='font-semibold text-gray-800'>Code Example</h4>");
    sourceTitle->addStyleClass("mb-0");
    auto sourceCode = contentWrapper->addNew<Wt::WTemplate>(
        "<div class='bg-gray-800 border border-gray-700 rounded-lg'>"
        "<pre class='p-4 overflow-x-auto text-sm text-gray-100'>"
        "<span style='color: #d19a66;'>#include</span> &lt;<span style='color: #56b6c2;'>Wt/WLineEdit.h</span>&gt;\n"
        "<span style='color: #d19a66;'>#include</span> &lt;<span style='color: #56b6c2;'>Wt/WLabel.h</span>&gt;\n\n"
        "<span style='color: #61afef;'>auto</span> label = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WLabel</span>&gt;(<span style='color: #98c379;'>\"Input Label\"</span>);\n"
        "<span style='color: #61afef;'>auto</span> lineEdit = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WLineEdit</span>&gt;();\n"
        "lineEdit-&gt;<span style='color: #61afef;'>setPlaceholderText</span>(<span style='color: #98c379;'>\"Line edit input...\"</span>);\n\n"
        "label-&gt;<span style='color: #61afef;'>setBuddy</span>(lineEdit);"
        "</pre>"
        "</div>"
    );
}

void WtComponentsGalery::createTextAreaExample(Wt::WContainerWidget* wrapper)
{
    // Example section title
    auto exampleTitle = wrapper->addNew<Wt::WTemplate>("<h3 class='font-semibold text-gray-800'>Text Area Example</h3>");
    auto contentWrapper = wrapper->addNew<Wt::WContainerWidget>();
    contentWrapper->addStyleClass("border border-gray-300 rounded-lg bg-white mb-6 p-6 space-y-6");
    
    auto textAreaLabel = contentWrapper->addNew<Wt::WLabel>("Text Area Label");
    auto textArea = contentWrapper->addNew<Wt::WTextArea>();
    textArea->setPlaceholderText("Text area input...");
    textAreaLabel->setBuddy(textArea);

    // Source code section
    auto sourceTitle = contentWrapper->addNew<Wt::WTemplate>("<h4 class='font-semibold text-gray-800'>Code Example</h4>");
    sourceTitle->addStyleClass("mb-0");
    auto sourceCode = contentWrapper->addNew<Wt::WTemplate>(
        "<div class='bg-gray-800 border border-gray-700 rounded-lg'>"
        "<pre class='p-4 overflow-x-auto text-sm text-gray-100'>"
        "<span style='color: #d19a66;'>#include</span> &lt;<span style='color: #56b6c2;'>Wt/WTextArea.h</span>&gt;\n"
        "<span style='color: #d19a66;'>#include</span> &lt;<span style='color: #56b6c2;'>Wt/WLabel.h</span>&gt;\n\n"
        "<span style='color: #61afef;'>auto</span> label = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WLabel</span>&gt;(<span style='color: #98c379;'>\"Text Area Label\"</span>);\n"
        "<span style='color: #61afef;'>auto</span> textArea = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WTextArea</span>&gt;();\n"
        "textArea-&gt;<span style='color: #61afef;'>setPlaceholderText</span>(<span style='color: #98c379;'>\"Text area input...\"</span>);\n\n"
        "label-&gt;<span style='color: #61afef;'>setBuddy</span>(textArea);"
        "</pre>"
        "</div>"
    );

}

std::unique_ptr<Wt::WWidget> WtComponentsGalery::createFormInputsDemo()
{
    wApp->log("debug") << "WtComponentsGalery::createFormInputsDemo()";
    return deferCreate([this]() { return formInputsDemo(); });
}