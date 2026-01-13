#include "008_Stylus/StylusState.h"
#include <Wt/WApplication.h>

#include <fstream>

namespace Stylus {


StylusState::StylusState()
    : doc_(std::make_shared<tinyxml2::XMLDocument>())
{
    wApp->log("debug") << "StylusState::StylusState()";
    stateFilePath_ = "../stylus-state.xml";

    doc_->LoadFile(stateFilePath_.c_str());
    if (doc_->ErrorID() != tinyxml2::XML_SUCCESS)
    {
        wApp->log("info") << "Stylus state XML file not found or error loading. Creating new state file.";
        // crete file
        std::ofstream file(stateFilePath_);
        if (!file.is_open())
        {
            wApp->log("error") << "Failed to create stylus state file at: " << stateFilePath_;
            return;
        }
        doc_->LoadFile(stateFilePath_.c_str());
    }
    stylusNode_ = doc_->FirstChildElement("stylus");
        if (stylusNode_ == nullptr)
        {
            std::cerr << "Error finding <stylus> node in XML file." << std::endl;
            stylusNode_ = doc_->NewElement("stylus");
            stylusNode_->SetAttribute("open", "true");
            doc_->InsertFirstChild(stylusNode_);
        }
        xmlNode_ = stylusNode_->FirstChildElement("xml-manager");
        if (xmlNode_ == nullptr)
        {
            std::cerr << "Error finding <xml-manager> node in XML file." << std::endl;
            xmlNode_ = doc_->NewElement("xml-manager");
            xmlNode_->SetAttribute("sidebar-width", 300);
            xmlNode_->SetAttribute("dbo-to-use", "dev");
            stylusNode_->InsertEndChild(xmlNode_);
        }
        cssNode_ = stylusNode_->FirstChildElement("css-manager");
        if (cssNode_ == nullptr)
        {
            std::cerr << "Error finding <css-manager> node in XML file." << std::endl;
            cssNode_ = doc_->NewElement("css-manager");
            cssNode_->SetAttribute("sidebar-width", 300);
            stylusNode_->InsertEndChild(cssNode_);
        }
        jsNode_ = stylusNode_->FirstChildElement("js-manager");
        if (jsNode_ == nullptr)
        {
            std::cerr << "Error finding <js-manager> node in XML file." << std::endl;
            jsNode_ = doc_->NewElement("js-manager");
            jsNode_->SetAttribute("sidebar-width", 300);
            stylusNode_->InsertEndChild(jsNode_);
        }
        tailwindConfigNode_ = stylusNode_->FirstChildElement("tailwind-config");
        if (tailwindConfigNode_ == nullptr)
        {
            std::cerr << "Error finding <tailwind-config> node in XML file." << std::endl;
            tailwindConfigNode_ = doc_->NewElement("tailwind-config");
            tailwindConfigNode_->SetAttribute("editor-width", 500);
            stylusNode_->InsertEndChild(tailwindConfigNode_);
        }
        settingsNode_ = stylusNode_->FirstChildElement("settings");
        if (settingsNode_ == nullptr)
        {
            std::cerr << "Error finding <settings> node in XML file." << std::endl;
            settingsNode_ = doc_->NewElement("settings");
            stylusNode_->InsertEndChild(settingsNode_);
        }
        imagesManagerNode_ = stylusNode_->FirstChildElement("images-manager");
        if (imagesManagerNode_ == nullptr)
        {
            std::cerr << "Error finding <images-manager> node in XML file." << std::endl;
            imagesManagerNode_ = doc_->NewElement("images-manager");
        }
        copyNode_ = doc_->FirstChildElement("copy");
        if (copyNode_ == nullptr)
        {
            std::cerr << "Error finding <copy> node in XML file." << std::endl;
            copyNode_ = doc_->NewElement("copy");
            doc_->InsertEndChild(copyNode_);
        }
        // organizeXmlNode(copyNode_, stateFilePath_);
        doc_->SaveFile(stateFilePath_.c_str());
        if (doc_->ErrorID() != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Error saving XML file: " << doc_->ErrorID() << std::endl;
        }
        std::cout << "\n\nStylusState initialized successfully.\n\n";

}

}