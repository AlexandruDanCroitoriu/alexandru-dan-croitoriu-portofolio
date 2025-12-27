#include "008_Stylus/StylusState.h"
#include <Wt/WApplication.h>

StylusState::StylusState()
    : doc_(std::make_shared<tinyxml2::XMLDocument>())
{
    wApp->log("debug") << "StylusState::StylusState()";
    state_file_path_ = "../../static/stylus/stylus-state.xml";

}