#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>

namespace Stylus
{

class StylusSession;
class TemplateFile;
class MessageTemplate;
class TempView;
class StylusState;

class TemplatesManager : public Wt::WContainerWidget
{
public:
    TemplatesManager(std::shared_ptr<StylusSession> sessionDev, std::shared_ptr<StylusSession> sessionProd, std::shared_ptr<StylusState> stylusState);

    // Wt::Signal<Wt::Dbo::ptr<TemplateFile>>& file_selected() { return file_selected_; }
    // Wt::Signal<Wt::Dbo::ptr<MessageTemplate>>& template_selected() { return template_selected_; }

    void keyWentDown(Wt::WKeyEvent e);
private:
    std::shared_ptr<StylusState> stylusState_;
    
    std::shared_ptr<StylusSession> sessionDev_;
    std::shared_ptr<StylusSession> sessionProd_;
    
    Wt::WTree* tree_;
    Wt::WContainerWidget* contentWrapper_;
    bool isRebuilding_ = false;  // Flag to suppress selection signals during tree rebuild

    // Wt::Signal<Wt::Dbo::ptr<TemplateFile>> file_selected_;
    // Wt::Signal<Wt::Dbo::ptr<MessageTemplate>> template_selected_;

    void populateTree();
    void renderSelection(std::vector<Wt::Dbo::ptr<MessageTemplate>> messageTemplate);
    // std::vector<TempView*> tempViews_;

};

}
