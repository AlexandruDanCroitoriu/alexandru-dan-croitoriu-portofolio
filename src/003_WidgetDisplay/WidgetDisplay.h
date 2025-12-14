#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
// namespace Wt {
//     class WStackedWidget;
//     class WMenu;
// }

class WidgetDisplay : public Wt::WContainerWidget
{
public:
    WidgetDisplay();

private:
    void createMonacoEditorExample();
    
    Wt::WStackedWidget* stackedWidget_;
    Wt::WMenu* menu_;
};