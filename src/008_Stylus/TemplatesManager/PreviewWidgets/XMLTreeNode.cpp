#include "008_Stylus/TemplatesManager/PreviewWidgets/XMLTreeNode.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/XmlBrain.h"

#include <Wt/WText.h>

namespace Stylus
{

    XMLTreeNode::XMLTreeNode(std::shared_ptr<XmlBrain> xmlBrain, tinyxml2::XMLElement *node)
        : xmlBrain_(xmlBrain), node_(node)
    {
        setStyleClass("flex flex-col text-sm");

        labelWrapper_ = addWidget(std::make_unique<Wt::WContainerWidget>());
        contentWrapper_ = addWidget(std::make_unique<Wt::WContainerWidget>());

        labelWrapper_->setStyleClass("flex space-x-2 truncate rounded-md mr-[3px] cursor-pointer overflow-visible ring-1 ring-transparent");
        contentWrapper_->setStyleClass("flex flex-col ml-3 rounded-md");
        // Classes for the lines of the tree
        labelWrapper_->addStyleClass("relative before:absolute before:-left-3 before:top-1/2 before:h-px before:w-3 before:bg-gray-600");
        contentWrapper_->addStyleClass("relative pl-3 before:absolute before:left-0 before:top-0 before:bottom-0 before:w-px before:bg-gray-600");

        if(node_){
            if(node_->ToElement()){
                auto tag_name = labelWrapper_->addWidget(std::make_unique<Wt::WText>(node_->Name()));
                tag_name->setStyleClass("font-medium pl-[5px]");
            }else if (node_->ToText()){
                auto text_content = labelWrapper_->addWidget(std::make_unique<Wt::WText>(node_->Value()));
                text_content->setStyleClass("font-light italic text-gray-400 select-none pl-[5px]");
            }
        }

        labelWrapper_->mouseWentOver().connect(this, [=]() {
            labelWrapper_->toggleStyleClass("!ring-green-700", true);
        });
        labelWrapper_->mouseWentOut().connect(this, [=]() {
            labelWrapper_->toggleStyleClass("!ring-green-700", false);
        });

        labelWrapper_->clicked().connect(this, [=]() {
            xmlBrain_->selectedNode_ = node_;
            xmlBrain_->stateChanged().emit();
        });

        if(xmlBrain_->selectedNode_ == node_)
        {
            labelWrapper_->addStyleClass("!ring-green-500");
        }

        auto child_node = node_->FirstChild();
        while (child_node)
        {
                auto element_node = static_cast<tinyxml2::XMLElement*>(child_node);
                auto child_widget = contentWrapper_->addWidget(std::make_unique<XMLTreeNode>(xmlBrain_, element_node));
                // child_widget->setStyleClass("mt-[2px]");
          
            // else {
            //     addWidget(std::make_unique<Wt::WText>("This node is not text or element nodes, WHAT DID YOU DO ?"))->setStyleClass("font-bold text-[#ff0000] outline-2 outline-[#ff0000] rounded-md p-[2px] hover:bg-[#ff0000]/30 cursor-pointer");
            // }
            
            child_node = child_node->NextSibling();
        }
        
    }

} // namespace Stylus
