#include "008_Stylus/Utils/XmlBrain.h"
#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/MessageTemplate.h"
#include "005_Dbo/Tables/TemplateFile.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>

namespace Stylus
{
    XmlBrain::XmlBrain(Session& session, Wt::Dbo::ptr<MessageTemplate> messageTemplate)
        : session_(session),
          doc_(std::make_shared<tinyxml2::XMLDocument>()),
          selectedNode_(nullptr),
          filePath_("")
    {
        if (!messageTemplate) {
            return;
        }

        std::string xmlContent;
        {
            Wt::Dbo::Transaction t(session_);
            xmlContent = messageTemplate->templateXml_;
            if (messageTemplate->file_) {
                filePath_ = messageTemplate->file_->fileName_;
            }
            t.commit();
        }

        tinyxml2::XMLError eResult = doc_->Parse(xmlContent.c_str());
        if (eResult != tinyxml2::XML_SUCCESS) {
            wApp->log("error") << "\nError parsing XML template: " << doc_->ErrorIDToName(eResult) << "\n";
            return;
        }

        selectedNode_ = doc_->RootElement();
        if (selectedNode_) {
            xmlNodeSelected_.emit(selectedNode_, false);
        }
    }


}
