#include "008_Stylus/TemplatesManager/PreviewWidgets/XmlBrain.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/MessageTemplate.h"
#include "008_Stylus/Tables/TemplateFile.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>

namespace Stylus
{
    XmlBrain::XmlBrain(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<MessageTemplate> messageTemplate)
        : session_(std::move(session)),
          doc_(std::make_shared<tinyxml2::XMLDocument>()),
          selectedNode_(nullptr),
        messageTemplate_(messageTemplate)
    {
        if (!messageTemplate) {
            wApp->log("error") << "XmlBrain::XmlBrain: Invalid MessageTemplate pointer.";
            return;
        }

        std::string xmlContent;
        Wt::Dbo::Transaction t(*session_);
        xmlContent = messageTemplate->templateXml_;
        t.commit();

        tinyxml2::XMLError eResult = doc_->Parse(xmlContent.c_str());
        if (eResult != tinyxml2::XML_SUCCESS) {
            wApp->log("error") << "\nError parsing XML template: " << doc_->ErrorIDToName(eResult) << "\n";
            return;
        }
    }

void XmlBrain::saveCurrentXmlModifications()
{
    if (!doc_) {
        wApp->log("error") << "XmlBrain::saveCurrentXmlModifications: XML document is null.";
        return;
    }

    tinyxml2::XMLPrinter printer;
    doc_->Print(&printer);
    std::string updatedXml = printer.CStr();

    {
        Wt::Dbo::Transaction t(*session_);
        // auto messageTemplate = session_.find<MessageTemplate>().where("template_xml = ?").bind(updatedXml).resultValue();
        auto messageTemplate = session_->find<MessageTemplate>().where("id = ?").bind(messageTemplate_->id()).resultValue();
        if (messageTemplate) {
            messageTemplate.modify()->templateXml_ = updatedXml;
        } else {
            wApp->log("error") << "XmlBrain::saveCurrentXmlModifications: Could not find MessageTemplate to update.";
        }
        t.commit();
    }
}

// void XmlBrain::reloadTemplateFromDb()
// {
//     {
//         Wt::Dbo::Transaction t(*session_);
//         auto messageTemplate = session_->find<MessageTemplate>().where("id = ?").bind(messageTemplate_->id()).resultValue();
//         if (messageTemplate) {
//             std::string xmlContent = messageTemplate->templateXml_;
//             tinyxml2::XMLError eResult = doc_->Parse(xmlContent.c_str());
//             if (eResult != tinyxml2::XML_SUCCESS) {
//                 wApp->log("error") << "\nError parsing XML template during reload: " << doc_->ErrorIDToName(eResult) << "\n";
//                 // return;
//             }
//         } else {
//             wApp->log("error") << "XmlBrain::reloadTemplateFromDb: Could not find MessageTemplate to reload.";
//         }
//         t.commit();
//         // stateChanged_.emit();
//         tempChanged_.emit();
//     }
// }

    
}
