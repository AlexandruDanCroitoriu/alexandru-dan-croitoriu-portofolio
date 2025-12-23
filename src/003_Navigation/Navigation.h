#pragma once

#include "006_Auth/AuthWidget.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>

#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

class NavigationTopic;
class Session;

class Navigation : public Wt::WContainerWidget
{
public:
    Navigation(std::shared_ptr<Session> session);

private:
    void setUI();
    Wt::WStackedWidget* contentsStack_;
    Wt::WPushButton* menuToggleButton_;
    Wt::WContainerWidget* sidebar_;
    Wt::WContainerWidget* navList_;
    Wt::WContainerWidget* contentsArea_;
    Wt::WContainerWidget* contentsCover_;
    bool menuOpen_;
    Wt::WAnchor* currentActiveAnchor_;

    // Routing: internal path -> content factory
    std::unordered_map<std::string, std::function<std::unique_ptr<Wt::WWidget>()>> routes_;
    // Paths that require authentication
    std::unordered_map<std::string, bool> authRequiredRoutes_;
    // Keep anchor by path to toggle active styles
    std::unordered_map<std::string, Wt::WAnchor*> anchorsByPath_;
    // Last unknown path to show on Not Found page
    std::shared_ptr<std::string> lastUnknownPath_;

    void setupRoutes();
    void buildSidebar();
    void navigateTo(const std::string& path);
    void markActive(const std::string& path);
    void toggleMenu();
    void openMenu();
    void closeMenu();
    void connectRouting();

    std::shared_ptr<Session> session_;
    Wt::WDialog* authDialog_;
    AuthWidget* authWidget_;
    Wt::WContainerWidget* authWrapper_;
    void authChanged();
};
