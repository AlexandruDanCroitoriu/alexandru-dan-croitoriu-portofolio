#include "App.h"

#include <Wt/WPushButton.h>
#include "004_TailwindTheme/TailwindTheme.h"
#include "003_Navigation/Navigation.h"

#include <Wt/WTemplate.h>

#include <Wt/WRandom.h>

App::App(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
{
    Wt::log("debug") << "App::App(const Wt::WEnvironment& env)";
    
    // Initialize database session
    session_ = std::make_shared<Session>("portfolio.db");
    
    // Restore login from remember-me token if available
    session_->restoreLogin();
    
    // Title
    setTitle("Alexandru Dan Croitoriu Portfolio");
    
    // Load Monaco Editor loader from local static files
    require("/static/monaco/vs/loader.js");
    // require("/static/js/animate-css-grid.js");
    // require("https://unpkg.com/animate-css-grid@latest");

    // Load utility XML resources (favicon)
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/Utils");

    // Load custom Auth system template resources
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/001_Auth/ovrwt-auth");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/001_Auth/ovrwt-auth-login");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/001_Auth/ovrwt-auth-strings");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/001_Auth/ovrwt-registration-view");

    // Load Tailwind Plus Elements
    // require("/static/theme/tailwindcss/tailwindplus/index.js");
    
    // Load Tailwind CSS style sheet

    // setCssTheme("polished");
    // setCssTheme("");
    
    setTheme(std::make_shared<TailwindTheme>());

    createApp();
}


void App::createApp()
{
    Wt::log("debug") << "App::createApp()";

    // root()->addNew<Wt::WPushButton>("Hello World");
    // root()->addNew<Wt::WTemplate>(Wt::WString::tr("favicon.svg"));

    // root()->addNew<WidgetDisplay>();
    root()->addNew<Navigation>(session());
}
