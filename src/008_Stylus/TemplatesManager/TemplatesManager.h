#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WIconPair.h>

namespace Stylus
{

class StylusSession;
class TemplateFolder;
class TemplateFile;
class MessageTemplate;
class TempView;
class StylusState;

class TemplatesManager : public Wt::WContainerWidget
{
public:
    TemplatesManager(std::shared_ptr<StylusSession> sessionDev, std::shared_ptr<StylusSession> sessionProd, std::shared_ptr<StylusState> stylusState);

    void keyWentDown(Wt::WKeyEvent e);
    Wt::WIconPair* toggleStylusMainNav_;
private:
    std::shared_ptr<StylusState> stylusState_;
    
    std::shared_ptr<StylusSession> sessionDev_;
    std::shared_ptr<StylusSession> sessionProd_;
    
    Wt::WTree* tree_;
    Wt::WContainerWidget* contentWrapper_;
    bool isRebuilding_ = false;  // Flag to suppress selection signals during tree rebuild

    void populateTree();
    void renderTemplates(std::vector<Wt::Dbo::ptr<MessageTemplate>> templates);
    void renderSelection(Wt::Dbo::ptr<MessageTemplate> messageTemplate);
    std::shared_ptr<StylusSession> getSelectedSession();

};

}
