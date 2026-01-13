#include "008_Stylus/TemplatesManager/PreviewWidgets/XMLElemNode.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/XmlBrain.h"

#include <Wt/WText.h>
#include <tinyxml2.h>

namespace Stylus
{

    XMLElemNode::XMLElemNode(std::shared_ptr<XmlBrain> xmlBrain, tinyxml2::XMLNode *node)
        : xmlBrain_(xmlBrain), node_(node)
    {
        setStyleClass("ring ring-transparent");
        mouseWentOver().connect(this, [=]() {
            toggleStyleClass("!ring-green-700", true);
        });
        mouseWentOut().connect(this, [=]() {
            toggleStyleClass("!ring-green-700", false);
        });
        mouseWentOver().preventPropagation();
        mouseWentOut().preventPropagation();
        
        clicked().connect(this, [=]() {
            xmlBrain_->selectedNode_ = node_;
            xmlBrain_->stateChanged().emit();
        });

        if(xmlBrain_->selectedNode_ && node_ && xmlBrain_->selectedNode_ == node_)
        {
            addStyleClass("!ring-green-500");
            setAttributeValue("style", "zoom: 1.1;");
        }
        if(!node_) return;

        if(node_->ToElement()){
            setHtmlTagName(node_->ToElement()->Name());
            if(node_->ToElement()->Attribute("class")){
                addStyleClass(node_->ToElement()->Attribute("class"));
            }
            auto first_child = node_->FirstChild();
            while(first_child)
            {
                // if(first_child->ToElement())
                // {
                    auto elem_node = addNew<XMLElemNode>(xmlBrain_, first_child);                    
                // }else if(first_child->ToText())
                // {
                    // addNew<Wt::WText>(first_child->ToText()->Value());
                // }
                first_child = first_child->NextSibling();
            }
        }else if(node_->ToText()){
            std::cout << "\n\nAdding text node with content: " << node_->ToText()->Value() << std::endl;
            auto text_content = node_->ToText()->Value();
            auto text_widget = addNew<Wt::WText>(text_content);
        }

    }

} // namespace Stylus
