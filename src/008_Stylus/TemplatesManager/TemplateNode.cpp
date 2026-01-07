#include "008_Stylus/TemplatesManager/TemplateNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>

namespace Stylus
{

TemplateNode::TemplateNode(const std::string& name, Wt::Dbo::ptr<MessageTemplate> messageTemplate)
    : Wt::WTreeNode(name),
      messageTemplate_(messageTemplate)
{
    wApp->log("debug") << "TemplateNode::TemplateNode(" << name << ")";
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");
    label_wrapper_ = labelArea();
    label_wrapper_->addStyleClass("flex items-center cursor-pointer");
    setSelectable(true);

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            showPopup(event);
        }
    });
}

void TemplateNode::createRenameTemplateDialog()
{
    wApp->log("debug") << "TemplateNode::createRenameTemplateDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename template"));
    dialog->setModal(true);
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("Template rename is not implemented yet."));
    dialog->show();
}

void TemplateNode::deleteTemplateMessageBox()
{
    wApp->log("debug") << "TemplateNode::deleteTemplateMessageBox";
    Wt::WMessageBox::show("Delete template", "Template deletion is not implemented yet.", Wt::StandardButton::Ok);
}

void TemplateNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "TemplateNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->addItem("Rename template")->triggered().connect(this, &TemplateNode::createRenameTemplateDialog);
        popup_->addItem("Delete template")->triggered().connect(this, &TemplateNode::deleteTemplateMessageBox);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

}
