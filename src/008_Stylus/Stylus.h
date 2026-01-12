#pragma once

#include <Wt/WDialog.h>
#include <Wt/WMenu.h>
#include <Wt/WMenuItem.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/StylusState.h"
#include <memory>

namespace Stylus {

    class TemplatesManager;

class Stylus : public Wt::WDialog
{
public:
    Stylus();

private:
    void initializeDialog();
    void setupContent();
    void setupKeyboardShortcuts();
    void keyWentDown(Wt::WKeyEvent e);

    std::shared_ptr<StylusSession> session_;

    Wt::WContainerWidget* navbar_wrapper_;
    Wt::WMenu* menu_;
    Wt::WStackedWidget* content_stack_;

    Wt::WContainerWidget* xml_files_wrapper_;
    Wt::WContainerWidget* css_files_wrapper_;
    Wt::WContainerWidget* js_files_wrapper_;
    Wt::WContainerWidget* tailwind_files_wrapper_;
    Wt::WContainerWidget* images_files_wrapper_;
    Wt::WContainerWidget* settings_wrapper_;

    TemplatesManager* templates_manager_;

    Wt::WMenuItem* xml_menu_item_;
    Wt::WMenuItem* css_menu_item_;
    Wt::WMenuItem* js_menu_item_;
    Wt::WMenuItem* tailwind_menu_item_;
    Wt::WMenuItem* images_menu_item_;
    Wt::WMenuItem* settings_menu_item_;

    StylusState stylus_state_;
};

}