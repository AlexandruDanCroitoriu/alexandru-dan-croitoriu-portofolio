#include "App.h"

#include <Wt/WPushButton.h>
#include "003_WidgetDisplay/WidgetDisplay.h"
#include "004_TailwindTheme/TailwindTheme.h"
#include "005_PersonalGalery/PersonalGalery.h"

#include <Wt/WRandom.h>

App::App(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
{
    Wt::log("debug") << "App::App() - application starting";
    // Title
    setTitle("Alexandru Dan Croitoriu Portfolio");
    
    // Load Monaco Editor loader from local static files
    require("/static/monaco/vs/loader.js");
    
    // Load Tailwind Plus Elements
    // require("/static/theme/tailwindcss/tailwindplus/index.js");
    
    // Load Tailwind CSS style sheet

    // setCssTheme("polished");
    // setCssTheme("");

    setTheme(std::make_shared<TailwindTheme>());

    // #ifdef DEBUG
    //     useStyleSheet("static/css/tailwind.css?v=" + Wt::WRandom::generateId());
    // #else
    //     useStyleSheet("static/css/tailwind.minify.css");
    // #endif

    createApp();
}


void App::createApp()
{
    Wt::log("debug") << "App::createApp() - creating application UI";

    // root()->addNew<Wt::WPushButton>("Hello World");

    // root()->addNew<WidgetDisplay>();
    root()->addNew<PersonalGalery>();
}
