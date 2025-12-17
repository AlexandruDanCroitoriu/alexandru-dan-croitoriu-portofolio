#pragma once

#include "006_Auth/AuthWidget.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>

#include <memory>

class NavigationTopic;
class Session;

class Navigation : public Wt::WContainerWidget
{
public:
    Navigation(std::shared_ptr<Session> session);

private:
    Wt::WStackedWidget* contentsStack_;
    Wt::WMenu* mainMenu_;
    Wt::WPushButton* menuToggleButton_;
    Wt::WContainerWidget* sidebar_;
    Wt::WContainerWidget* contentsCover_;
    bool menuOpen_;
    Wt::WMenuItem* currentSelectedItem_{};

    Wt::WMenuItem* addToMenu(Wt::WMenu* menu,
                             const Wt::WString& name,
                             std::unique_ptr<NavigationTopic> topic);
                             
    Wt::WMenuItem* addDirectItemToMenu(Wt::WMenu* menu,
                                       const Wt::WString& name,
                                       std::unique_ptr<Wt::WWidget> content,
                                       const Wt::WString& path = "");
    void toggleMenu();
    void openMenu();
    void closeMenu();
    void setActiveItem(Wt::WMenuItem* item);

    std::shared_ptr<Session> session_;
    Wt::WDialog* authDialog_;
    AuthWidget* authWidget_;
    Wt::WContainerWidget* authWrapper_;
    void authChanged();
};
