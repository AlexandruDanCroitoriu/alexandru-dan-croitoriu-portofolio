#pragma once
#include <Wt/WContainerWidget.h>

#include <tinyxml2.h>

namespace Stylus 
{

    class XmlBrain;
    
    class XMLElemNode : public Wt::WContainerWidget
{
    public:
        XMLElemNode(std::shared_ptr<XmlBrain> xmlBrain, tinyxml2::XMLNode* node);

    private:
        std::shared_ptr<XmlBrain> xmlBrain_;
        tinyxml2::XMLNode* node_;


};
} // namespace Stylus