#include "App.h"

#include <Wt/WPushButton.h>
#include "003_WidgetDisplay/WidgetDisplay.h"

App::App(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
{
    Wt::log("debug") << "App::App() - application starting";
    // Title
    setTitle("Alexandru Dan Croitoriu Portfolio");
    
    // Load Monaco Editor loader from local static files
    require(docRoot() + "/static/monaco/vs/loader.js");
    
    createApp();
}


void App::createApp()
{
    Wt::log("debug") << "App::createApp() - creating application UI";

    root()->addNew<Wt::WPushButton>("Hello World");

    root()->addNew<WidgetDisplay>();


}
