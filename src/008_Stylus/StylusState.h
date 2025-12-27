#pragma once

#include <tinyxml2.h>
#include <memory>
#include <string>



struct StylusState {
    StylusState();
    
    std::string state_file_path_;

    std::shared_ptr<tinyxml2::XMLDocument> doc_;
    tinyxml2::XMLElement* stylus_node_ = nullptr;
    tinyxml2::XMLElement* xml_node_ = nullptr;
    tinyxml2::XMLElement* css_node_ = nullptr;
    tinyxml2::XMLElement* js_node_ = nullptr;
    tinyxml2::XMLElement* tailwind_config_node_ = nullptr;
    tinyxml2::XMLElement* settings_node_ = nullptr;
    tinyxml2::XMLElement* images_manager_node_ = nullptr;
    tinyxml2::XMLElement* copy_node_ = nullptr;

};