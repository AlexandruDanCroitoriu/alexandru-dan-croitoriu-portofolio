#include "003_WidgetDisplay/WidgetDisplay.h"

#include "002_Components/MonacoEditor.h"

#include <Wt/WText.h>

WidgetDisplay::WidgetDisplay()
{

    auto nevigaiton = addNew<Wt::WContainerWidget>();
    
    // Create the stacked widget first
    stackedWidget_ = addNew<Wt::WStackedWidget>();
    
    // Create the menu and pass the stacked widget to it
    menu_ = nevigaiton->addNew<Wt::WMenu>(stackedWidget_);

    createMonacoEditorExample();
}

void WidgetDisplay::createMonacoEditorExample()
{
    
    auto content = std::make_unique<Wt::WContainerWidget>();
    auto content_ptr = content.get();
    menu_->addItem("texteditor", std::move(content));
    
    content_ptr->addNew<Wt::WText>("<h3>Monaco Editor Example (CSS)</h3>");
    auto monaco = content_ptr->addNew<MonacoEditor>("css");
    monaco->setHeight(Wt::WLength(500, Wt::LengthUnit::Pixel));

}