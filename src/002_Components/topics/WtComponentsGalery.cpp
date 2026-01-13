#include "002_Components/topics/WtComponentsGalery.h"
#include "003_Navigation/DeferredWidget.h"

#include <Wt/WAnchor.h>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WTextArea.h>
#include <sstream>
#include <utility>
#include <vector>

WtComponentsGalery::WtComponentsGalery()
{
    wApp->log("debug") << "WtComponentsGalery::WtComponentsGalery()";
}

std::unique_ptr<Wt::WWidget> WtComponentsGalery::buildLineEditExample()
{
    auto wrapper = std::make_unique<Wt::WContainerWidget>();
    wrapper->addStyleClass("space-y-2");

    wrapper->addNew<Wt::WTemplate>("<h3 class='font-semibold text-gray-800'>Line Edit Example</h3>");
    auto contentWrapper = wrapper->addNew<Wt::WContainerWidget>();
    contentWrapper->addStyleClass("border border-gray-300 rounded-lg bg-white mb-2 p-6 space-y-6");

    auto lineEditLabel = contentWrapper->addNew<Wt::WLabel>("Input Label");
    auto lineEdit = contentWrapper->addNew<Wt::WLineEdit>();
    lineEdit->setPlaceholderText("Line edit input...");
    lineEditLabel->setBuddy(lineEdit);
    contentWrapper->addNew<Wt::WContainerWidget>()->setStyleClass("w-full border-t border-gray-200 m-0");
    lineEdit->setDisabled(true);
    auto sourceTitle = contentWrapper->addNew<Wt::WTemplate>("<h4 class='font-semibold text-gray-800'>Code Example</h4>");
    sourceTitle->addStyleClass("mb-0");

    std::ostringstream oss;
    auto addLine = [&](const std::string &line)
    { oss << line << "\n"; };

    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeIncludeLine("Wt/WLineEdit.h", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#details"));
    addLine(makeIncludeLine("Wt/WLabel.h", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLabel.html#details"));
    addLine("");
    addLine("<span style='color: #61afef;'>auto</span> label = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WLabel</span>&gt;(<span style='color: #98c379;'>\"Input Label\"</span>);");
    addLine("<span style='color: #61afef;'>auto</span> lineEdit = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WLineEdit</span>&gt;();");
    addLine("label-&gt;<a href='https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLabel.html#af468c613eab7d184c68fd6ae20978de5' target='_blank' style='color: #61afef;  text-decoration: underline;'>setBuddy</a>(lineEdit);");
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Signals</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine("lineEdit-&gt;<a href='https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a0b1824c15b82b513176cf0a9f0adcf29' target='_blank' style='color: #61afef;  text-decoration: underline;'>textInput()</a>.<span style='color: #61afef;'>connect</span>([=]() {});");
    addLine("lineEdit-&gt;<a href='https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a5216ba1594b5dc525cc47cad593c7ba8' target='_blank' style='color: #61afef;  text-decoration: underline;'>changed()</a>.<span style='color: #61afef;'>connect</span>([=]() {}); <span style='color: #6a9955;'>// From Wt::WFormWidget</span>");
    addLine("lineEdit-&gt;<a href='https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a1f3d3f3f3e2f4f4b5e6c7d8e9f0a1b2c' target='_blank' style='color: #61afef;  text-decoration: underline;'>validated()</a>.<span style='color: #61afef;'>connect</span>([=](<span style='color: #61afef;'>Wt::ValidationState</span> state) {}); <span style='color: #6a9955;'>// From Wt::WFormWidget</span>");
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Value Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#af24caa7b5618d12c6638012d15ab5296",
                           "setValueText", "<span style='color: #98c379;'>\"Initial text\"</span>"));
    addLine("<span style='color: #61afef;'>Wt::WString</span> text = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a8dc5388e16ed625a7ddb1a68d81f8f57",
                           "text", ""));
    addLine("<span style='color: #61afef;'>Wt::WString</span> valueText = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a5d99b416b458b7fb5fbe0f73b1c5fe7e",
                           "valueText", ""));
    addLine("<span style='color: #61afef;'>Wt::WString</span> displayText = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a496146733c3d5fa0105b485bf9304e35",
                           "displayText", ""));

    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Max Length Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#aaeca334d8a47be029b6a44912b01ed5d",
                           "setMaxLength", "128"));
    addLine("<span style='color: #61afef;'>int</span> maxLength = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a14d707a8577685eb33588539305f3b78",
                           "maxLength", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Placeholder Text Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#aabebd75188d1363c022925845f6b3269",
                           "setPlaceholderText", "<span style='color: #98c379;'>\"Line edit input...\"</span>", "Wt::WFormWidget"));
    addLine("<span style='color: #61afef;'>Wt::WString</span> placeholderText = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a75425fe7a19d61dbdf17492d3c7d4628",
                           "placeholderText", "", "Wt::WFormWidget"));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Input Mask Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a0639ec7e420ba32ffb148303a79fb969",
                           "setInputMask", "<span style='color: #98c379;'>\"AAAA-9999\"</span>, <span style='color: #569cd6;'>Wt::InputMaskFlag::KeepMaskWhileBlurred</span>"));
    addLine("<span style='color: #569cd6;'>Wt::WString</span> inputMask = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a15a01f7030b64f7ac8dc2e363d5ce9e9",
                           "inputMask", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Input Mode Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a5d772a843fb12447da0e295e6fae805b",
                           "setInputMode", "<span style='color: #569cd6;'>Wt::InputMode::Numeric</span>"));
    addLine("<span style='color: #569cd6;'>Wt::InputMode</span> inputMode = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#afea0d586e9bcc578d0f848f65bbf4c28",
                           "inputMode", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Echo Mode Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a27c9bc232bea0ab76ee266be3325a10e",
                           "setEchoMode", "<span style='color: #569cd6;'>Wt::EchoMode::Password</span>"));
    addLine("<span style='color: #569cd6;'>Wt::EchoMode</span> echoMode = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a906ed01bcd06a2f5b5dcbc102b29fa66",
                           "echoMode", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Selection Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a5998d0a1c9f842fdf4561ebb71292378",
                           "setSelection", "0, 4"));
    addLine("<span style='color: #569cd6;'>bool</span> hasSelectedText = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a58f3ab53d913b5859059d2784e69a1f8",
                           "hasSelectedText", ""));
    addLine("<span style='color: #569cd6;'>int</span> selectionStartPosition = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#abd05e3f58ac768ddc41cfe77e5a8a45f",
                           "selectionStart", ""));
    addLine("<span style='color: #569cd6;'>Wt::WString</span> selectedText = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#aa571d1f8c525822d820b188fc167160a",
                           "selectedText", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Text Size Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a4c6c5a52deefa5a246e0638f1572d0d8",
                           "setTextSize", "4"));
    addLine("<span style='color: #61afef;'>int</span> lineEditSize = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a55bdc89327923f865d024f59541c20d0",
                           "textSize", ""));

    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Autocomplete Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a67ea9e169101e96837e53cc9cc600a67",
                           "setAutoComplete", "false"));
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a9efd6b54f094aad70b2b525f27a4dca8",
                           "setAutoComplete", "<span style='color: #569cd6;'>Wt::AutoCompleteMode::Off</span>"));
    addLine("<span style='color: #569cd6;'>bool</span> autoCompleteOn = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#aa8fc3164b23ebce987cc1f21641c549e",
                           "autoComplete", ""));
    addLine("<span style='color: #569cd6;'>Wt::AutoCompleteMode</span> autoCompleteMode = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a45e523b854d98ab7d59ea7f769807364",
                           "autoCompleteToken", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Validation Setter and Getter</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a8b5fa8567e930691b95f1d0fa69359f1",
                           "setValidator", "<span style='color: #61afef;'>std::shared_ptr</span>&lt;<span style='color: #e06c75;'>Wt::WValidator</span>&gt; validator", "Wt::WFormWidget"));
    addLine("<span style='color: #61afef;'>std::shared_ptr</span>&lt;<span style='color: #e06c75;'>Wt::WValidator</span>&gt; validator = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a0d362bbeea8f4da4617b24757772a996",
                           "validator", "", "Wt::WFormWidget"));
    addLine("<span style='color: #61afef;'>Wt::ValidationState</span> validationState = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a500cfde61dad0df1bb4dd0f3b7115f8a",
                           "validate", ""));
    oss << "</pre>";
    oss << "</div>";
    oss << "<div class='font-semibold text-gray-800 mt-2'>Other Methods</div>";
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine("<span style='color: #569cd6;'>Wt::WLabel*</span> labelBuddy = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a71e7a278888ebe7857c89de55e16ee0e",
                           "label", "", "Wt::WFormWidget"));
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a100f119ac4ec3db49fbdd53ab50fff6b",
                           "setHidden", "true, Wt::WAnimation()", "Wt::WFormWidget"));
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#ad26d15f452d47832a4467608977f03f5",
                           "setEnabled", "false", "Wt::WFormWidget"));
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a353ae79a10748c870f1fa2ce1f174eaa",
                           "setReadOnly", "true", "Wt::WFormWidget"));
    addLine("<span style='color: #569cd6;'>bool</span> isReadOnly = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#a308ae2b05ee007bea4637b1539ce251e",
                           "isReadOnly", "", "Wt::WFormWidget"));
    addLine(makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WFormWidget.html#add294e4a3073e58ea1362dda7e811955",
                           "setToolTip", "<span style='color: #98c379;'>\"&lt;span class='bg-green-200'&gt;This is a WLineEdit component&lt;/span&gt;\"</span>", "Wt::WFormWidget"));
    addLine("<span style='color: #569cd6;'>int</span> cursorPos = " +
            makeMethodCall("lineEdit", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#a6e9fe523757e4e2cd0531cd4b48c1560",
                           "cursorPosition", ""));
    oss << "</pre>";
    oss << "</div>";
    contentWrapper->addNew<Wt::WTemplate>(oss.str());
    return wrapper;
}

std::unique_ptr<Wt::WWidget> WtComponentsGalery::buildTextAreaExample()
{
    auto wrapper = std::make_unique<Wt::WContainerWidget>();
    wrapper->addStyleClass("space-y-2");

    wrapper->addNew<Wt::WTemplate>("<h3 class='font-semibold text-gray-800'>Text Area Example</h3>");
    auto contentWrapper = wrapper->addNew<Wt::WContainerWidget>();
    contentWrapper->addStyleClass("border border-gray-300 rounded-lg bg-white mb-2 p-6 space-y-6");

    auto textAreaLabel = contentWrapper->addNew<Wt::WLabel>("Text Area Label");
    auto textArea = contentWrapper->addNew<Wt::WTextArea>();
    textArea->addStyleClass("field-sizing-content"); // Adjust size based on content
    textArea->setPlaceholderText("Text area input...");
    textAreaLabel->setBuddy(textArea);

    auto sourceTitle = contentWrapper->addNew<Wt::WTemplate>("<h4 class='font-semibold text-gray-800'>Code Example</h4>");
    sourceTitle->addStyleClass("mb-0");
    std::ostringstream oss;
    auto addLine = [&](const std::string &line)
    { oss << line << "\n"; };
    oss << "<div class='bg-gray-800 border border-gray-700 rounded-lg'>";
    oss << "<pre class='p-4 overflow-x-auto text-sm text-gray-100 text-wrap wrap-anywhere'>";
    addLine(makeIncludeLine("Wt/WTextArea.h", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WTextArea.html#details"));
    addLine(makeIncludeLine("Wt/WLabel.h", "https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLabel.html#details"));
    addLine("");
    addLine("<span style='color: #61afef;'>auto</span> label = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WLabel</span>&gt;(<span style='color: #98c379;'>\"Text Area Label\"</span>);"); 
    addLine("<span style='color: #61afef;'>auto</span> textArea = <span style='color: #61afef;'>addNew</span>&lt;<span style='color: #e06c75;'>Wt::WTextArea</span>&gt;();");
    addLine("textArea-&gt;<a href='https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WWidget.html#abb5949e98f47edb7db98d7b6df07a31c' target='_blank' style='color: #61afef;  text-decoration: underline;'>addStyleClass</a>(<span style='color: #98c379;'>\"field-sizing-content\"</span>);  <span style='color: #6a9955;'>// Adjust size based on content</span>");
    addLine("label-&gt;<a href='https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLabel.html#af468c613eab7d184c68fd6ae20978de5' target='_blank' style='color: #61afef;  text-decoration: underline;'>setBuddy</a>(textArea);");
    oss << "</pre>";
    oss << "</div>";
    contentWrapper->addNew<Wt::WTemplate>(oss.str());

    return wrapper;
}

std::string WtComponentsGalery::makeIncludeLine(const std::string &header, const std::string &docUrl) const
{
    std::ostringstream oss;
    oss << "<span style='color: #d19a66;'>#include</span> &lt;<a href='" << docUrl
        << "' target='_blank' style='color: #56b6c2; text-decoration: underline;'>"
        << header << "</a>&gt;";
    return oss.str();
}

std::string WtComponentsGalery::makeMethodCall(const std::string &object, const std::string &docUrl, const std::string &method,
                                               const std::string &args, const std::string &inherited) const
{
    std::ostringstream oss;
    oss << object << "-&gt;<a href='" << docUrl << "' target='_blank' style='color: #61afef; text-decoration: underline;'>"
        << method << "</a>(" << args << ");";

    if (inherited.size() > 0)
    {
        oss << " <span style='color: #6a9955;'>// From " << inherited << "</span>";
    }
    return oss.str();
}

WtComponentsGalery::ExamplePage WtComponentsGalery::exampleFromPath(const std::string &path) const
{
    if (path.rfind("/components/wt-components/textarea", 0) == 0)
    {
        return ExamplePage::TextArea;
    }
    return ExamplePage::LineEdit;
}

std::string WtComponentsGalery::pathForExample(ExamplePage example) const
{
    if (example == ExamplePage::TextArea)
    {
        return "/components/wt-components/textarea";
    }
    return "/components/wt-components/line-edit";
}

std::unique_ptr<Wt::WWidget> WtComponentsGalery::createWtComponentsGalery()
{
    wApp->log("debug") << "WtComponentsGalery::createWtComponentsGalery()";
    return deferCreateTopicPage([this]()
                                {
                           auto container = std::make_unique<Wt::WContainerWidget>();
                           container->addStyleClass("w-full max-w-5xl mx-auto space-y-2 p-6");

                           container->addNew<Wt::WTemplate>("<h2 class='text-3xl font-bold text-gray-800 mb-2'>Wt Library Components / Widgets</h2>");

                           container->addNew<Wt::WTemplate>(
                               "<p class='text-gray-700 leading-relaxed mb-2'>"
                               "A comprehensive showcase of various Wt library components and widgets, demonstrating their usage, features, "
                               "and customization options within a web application context."
                               "</p>");

                           auto nav = container->addNew<Wt::WContainerWidget>();
                           nav->addStyleClass("flex flex-wrap items-center gap-2 border-y border-gray-200 py-3");

                           std::vector<std::pair<ExamplePage, Wt::WAnchor *>> navItems;
                           auto addNavLink = [&](ExamplePage example, const std::string &label)
                           {
                               auto anchor = nav->addNew<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, pathForExample(example)), Wt::WString::fromUTF8(label));
                               anchor->addStyleClass("px-3 py-2 rounded-md text-sm font-semibold border shadow-sm transition-colors duration-200");
                               anchor->addStyleClass("bg-white text-gray-800 border-gray-200 hover:bg-gray-50");
                               navItems.push_back({example, anchor});
                           };

                           addNavLink(ExamplePage::LineEdit, "Line edit");
                           addNavLink(ExamplePage::TextArea, "Text area");

                           auto stack = container->addNew<Wt::WStackedWidget>();
                           stack->addStyleClass("mt-4");

                           // Store factory functions for lazy creation
                           std::vector<std::function<std::unique_ptr<Wt::WWidget>()>> factories = {
                               [this]() { return buildLineEditExample(); },
                               [this]() { return buildTextAreaExample(); }
                           };
                           
                           // Add placeholder widgets that will be replaced on demand
                           std::vector<Wt::WContainerWidget*> placeholders;
                           for (size_t i = 0; i < factories.size(); ++i) {
                               auto placeholder = stack->addNew<Wt::WContainerWidget>();
                               placeholders.push_back(placeholder);
                           }

                           auto activeExample = exampleFromPath(wApp->internalPath());
                           int initialIndex = static_cast<int>(activeExample);
                           stack->setCurrentIndex(initialIndex);

                           auto setActiveNav = [&](ExamplePage example)
                           {
                               for (auto &item : navItems)
                               {
                                   bool isActive = item.first == example;
                                   item.second->toggleStyleClass("border-slate-800", isActive, true);
                                   item.second->toggleStyleClass("bg-white text-gray-800 border-gray-200 hover:bg-gray-50", !isActive, true);
                               }
                           };
                           setActiveNav(activeExample);

                           // Lambda to load content for a specific index
                           auto loadContent = [&factories, &placeholders](int index) {
                               if (index >= 0 && index < static_cast<int>(placeholders.size())) {
                                   auto placeholder = placeholders[index];
                                   // Only create if not already created
                                   if (placeholder->children().empty()) {
                                       auto widget = factories[index]();
                                       placeholder->addWidget(std::move(widget));
                                   }
                               }
                           };

                           // Create the initial page on demand
                           loadContent(initialIndex);

                           return container; });
}