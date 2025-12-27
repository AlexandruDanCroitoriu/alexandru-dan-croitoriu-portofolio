#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>

class MessageTemplate;

namespace Stylus
{

class TemplateNode : public Wt::WTreeNode
{
public:
    TemplateNode(const std::string& name, Wt::Dbo::ptr<MessageTemplate> messageTemplate = Wt::Dbo::ptr<MessageTemplate>());

    Wt::Signal<std::string> template_changed_;
    Wt::WContainerWidget* label_wrapper_;
    Wt::Dbo::ptr<MessageTemplate> messageTemplate_;

private:
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createRenameTemplateDialog();
    void deleteTemplateMessageBox();
    void showPopup(const Wt::WMouseEvent& event);
};

}
