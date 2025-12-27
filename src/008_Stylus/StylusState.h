#pragma once

#include <tinyxml2.h>
#include <memory>
#include <string>



struct StylusState {
    StylusState();
    
    std::string stateFilePath_;

    std::shared_ptr<tinyxml2::XMLDocument> doc_;
    tinyxml2::XMLElement* stylusNode_ = nullptr;
    tinyxml2::XMLElement* xmlNode_ = nullptr;
    tinyxml2::XMLElement* cssNode_ = nullptr;
    tinyxml2::XMLElement* jsNode_ = nullptr;
    tinyxml2::XMLElement* tailwindConfigNode_ = nullptr;
    tinyxml2::XMLElement* settingsNode_ = nullptr;
    tinyxml2::XMLElement* imagesManagerNode_ = nullptr;
    tinyxml2::XMLElement* copyNode_ = nullptr;

};