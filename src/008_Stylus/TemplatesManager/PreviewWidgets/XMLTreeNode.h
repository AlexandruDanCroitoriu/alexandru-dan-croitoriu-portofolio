#pragma once
#include <Wt/WContainerWidget.h>

#include <tinyxml2.h>

namespace Stylus 
{

    class XmlBrain;
    
    class XMLTreeNode : public Wt::WContainerWidget
{
    public:
        XMLTreeNode(std::shared_ptr<XmlBrain> xmlBrain, tinyxml2::XMLElement* node);

    private:
        std::shared_ptr<XmlBrain> xmlBrain_;
        tinyxml2::XMLElement* node_;

        Wt::WContainerWidget* labelWrapper_;
        Wt::WContainerWidget* contentWrapper_;

};
} // namespace Stylus