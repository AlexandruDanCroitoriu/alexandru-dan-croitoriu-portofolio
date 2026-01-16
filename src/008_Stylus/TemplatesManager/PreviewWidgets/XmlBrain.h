#pragma once

#include <map>
#include <memory>
#include <string>

#include <tinyxml2.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WSignal.h>

namespace Stylus
{

class StylusSession;
class MessageTemplate;

class XmlBrain
{
public:
        XmlBrain(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<MessageTemplate> messageTemplate = Wt::Dbo::ptr<MessageTemplate>());
        
        std::shared_ptr<tinyxml2::XMLDocument> doc_;
        tinyxml2::XMLNode* selectedNode_;

        std::shared_ptr<StylusSession> session_;
        Wt::Signal<> &stateChanged() { return stateChanged_; }
        void saveCurrentXmlModifications();
        Wt::Dbo::ptr<MessageTemplate> messageTemplate_;
private:
        Wt::Signal<> stateChanged_;
};

} // namespace Stylus