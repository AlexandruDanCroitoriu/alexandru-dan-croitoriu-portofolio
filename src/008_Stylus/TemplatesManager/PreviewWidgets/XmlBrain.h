#pragma once

#include <map>
#include <memory>
#include <string>

#include <tinyxml2.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WSignal.h>

class StylusSession;
class MessageTemplate;

namespace Stylus
{

    class XmlBrain
    {
        public:
            XmlBrain(StylusSession& session, Wt::Dbo::ptr<MessageTemplate> messageTemplate = Wt::Dbo::ptr<MessageTemplate>());
            
            std::shared_ptr<tinyxml2::XMLDocument> doc_;
            tinyxml2::XMLNode* selectedNode_;

            
            
            StylusSession& session_;
            Wt::Signal<>& stateChanged() { return stateChanged_; }
            void saveCurrentVersion();
            Wt::Dbo::ptr<MessageTemplate> messageTemplate_;
        private:
            Wt::Signal<> stateChanged_;
    };
}