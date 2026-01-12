#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WSignal.h>

class MessageTemplate;
class StylusSession;

namespace Stylus
{

class TemplateNode : public Wt::WTreeNode
{
public:
    TemplateNode(StylusSession& session, Wt::Dbo::ptr<MessageTemplate> messageTemplate);

    Wt::WContainerWidget* label_wrapper_;
    Wt::Dbo::ptr<MessageTemplate> messageTemplate_;

    Wt::Signal<> &changed() { return changed_; }
private:
    StylusSession& session_;
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createRenameTemplateDialog();
    void createRemoveTemplateDialog();
    void createChangeFileDialog();
    void showPopup(const Wt::WMouseEvent& event);
    void moveTemplateUp();
    void moveTemplateDown();
    Wt::Signal<> changed_;
};

}
