#include "008_Stylus/TemplatesManager/TemplateNode.h"
#include "008_Stylus/Tables/MessageTemplate.h"
#include "008_Stylus/StylusSession.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>
#include <Wt/WTree.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>

namespace Stylus
{

TemplateNode::TemplateNode(StylusSession& session, Wt::Dbo::ptr<MessageTemplate> messageTemplate)
    : Wt::WTreeNode(messageTemplate->messageId_),
        messageTemplate_(messageTemplate),
        session_(session)
{
    wApp->log("debug") << "TemplateNode::TemplateNode(" << messageTemplate->messageId_ << ")";
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");
    addStyleClass("[&>.Wt-item]:hover:bg-gray-700");
    label_wrapper_ = labelArea();
    label_wrapper_->addStyleClass("flex items-center cursor-pointer");
    setSelectable(true);

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        // if (event.button() == Wt::MouseButton::Right && tree()->isSelected(this))
        // {
        //     showPopup(event);
        // }
        if (event.button() == Wt::MouseButton::Right)
        {
            tree()->select(this);
            showPopup(event);
        }
    });
}

void TemplateNode::createRenameTemplateDialog()
{
    wApp->log("debug") << "TemplateNode::createRenameTemplateDialog";
   auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename Template"));
    dialog->addStyleClass("text-white border border-gray-600 rounded-md !bg-gray-800");
    dialog->addStyleClass("[&>.dialog-layout]:bg-gray-800 [&>.dialog-layout]:rounded-md ");
    dialog->titleBar()->setStyleClass("px-2 py-0 flex items-center justify-center overflow-x-visible h-[40px] bg-gray-900 text-white font-bold");
    dialog->titleBar()->addStyleClass("bg-gray-800 hover:bg-gray-700 border border-gray-600 rounded-t-md cursor-pointer");
    dialog->contents()->setStyleClass("bg-gray-800 text-white p-4 rounded-b-md flex flex-col items-end min-w-sm min-h-fit");
    auto closeBtn = dialog->titleBar()->addWidget(std::make_unique<Wt::WPushButton>("X"));
    closeBtn->clicked().connect(dialog, [=]() { dialog->reject(); });
    closeBtn->addStyleClass("ml-auto !border-gray-700 text-gray-600 hover:bg-gray-600 hover:text-white rounded-md ");
    dialog->setResizable(false);
    dialog->setMovable(true);
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("New Template ID:"));
    label->setStyleClass("w-full block");
    auto input = dialog->contents()->addWidget(std::make_unique<Wt::WLineEdit>(messageTemplate_->messageId_));
    input->setStyleClass("w-full");

    auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
    errorText->setStyleClass("w-full block text-red-500 mt-2");
    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto renameBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Rename Template"));
    renameBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");
    
    input->enterPressed().connect([=]() { 
        renameBtn->clicked().emit(Wt::WMouseEvent());
    });
    renameBtn->clicked().connect([=]() {
        // Perform template rename logic here
        std::string templateName = input->text().toUTF8();
        if (templateName.empty())
        {
            errorText->setText("Template name cannot be empty.");
            return;
        }
        {
            Wt::Dbo::Transaction t(session_);
            messageTemplate_.modify()->messageId_ = templateName;
            t.commit();
        }
 
        dialog->accept();
        changed_.emit();
    });
    dialog->show();
}

void TemplateNode::createRemoveTemplateDialog()
{
    wApp->log("debug") << "TemplateNode::createRemoveTemplateDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Delete Template"));
    dialog->addStyleClass("text-white border border-gray-600 rounded-md !bg-gray-800");
    dialog->addStyleClass("[&>.dialog-layout]:bg-gray-800 [&>.dialog-layout]:rounded-md ");
    dialog->titleBar()->setStyleClass("px-2 py-0 flex items-center justify-center overflow-x-visible h-[40px] bg-gray-900 text-white font-bold");
    dialog->titleBar()->addStyleClass("bg-gray-800 hover:bg-gray-700 border border-gray-600 rounded-t-md cursor-pointer");
    dialog->contents()->setStyleClass("bg-gray-800 text-white p-4 rounded-b-md flex flex-col items-end min-w-sm min-h-fit");
    auto closeBtn = dialog->titleBar()->addWidget(std::make_unique<Wt::WPushButton>("X"));
    closeBtn->clicked().connect(dialog, [=]() { dialog->reject(); });
    closeBtn->addStyleClass("ml-auto !border-gray-700 text-gray-600 hover:bg-gray-600 hover:text-white rounded-md ");
    dialog->setResizable(false);
    dialog->setMovable(true);
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    
    auto errorTemplate = dialog->contents()->addWidget(std::make_unique<Wt::WText>("Are you sure you want to delete the template '<strong>" + messageTemplate_->messageId_ + "</strong>'?"));
    errorTemplate->setStyleClass("w-full block mt-2");

    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto deleteBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Delete Template"));
    deleteBtn->addStyleClass("bg-red-600 hover:bg-red-700 text-white px-4 py-2 rounded-md self-end");
    deleteBtn->addStyleClass("disabled:opacity-50 disabled:cursor-not-allowed");

    deleteBtn->clicked().connect([=]() {
        // Perform folder deletion logic here
        Wt::Dbo::Transaction t(session_);
        messageTemplate_.remove();
        t.commit();
        dialog->accept();
        changed_.emit();
    });

    dialog->show();
}

void TemplateNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "TemplateNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->addItem("Rename template")->triggered().connect(this, &TemplateNode::createRenameTemplateDialog);
        popup_->addSeparator();
        popup_->addItem("Delete template")->triggered().connect(this, &TemplateNode::createRemoveTemplateDialog);
        popup_->setHideOnSelect(true);
        popup_->setAutoHide(true, 400);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

}
