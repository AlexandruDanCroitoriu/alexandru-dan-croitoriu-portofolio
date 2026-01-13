#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/RootNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTree.h>

#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFolder.h"

namespace Stylus
{

RootNode::RootNode(std::shared_ptr<StylusSession> session)
    : Wt::WTreeNode("DBO Root"),
    session_(session)
{
    label_wrapper_ = labelArea();
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");
    addStyleClass("[&>.Wt-item]:hover:bg-gray-700");

    label_wrapper_->addStyleClass("flex items-center cursor-pointer ");
    setSelectable(true);
    expand();

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
    
    setLabelIcon(std::make_unique<Wt::WIconPair>("./static/icons/folder-closed.png", "./static/icons/folder-open.png"));


}

void RootNode::createNewFolderDialog()
{
    wApp->log("debug") << "RootNode::createNewFolderDialog()";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new folder"));
    dialog->addStyleClass("text-white border border-gray-600 rounded-md !bg-gray-800");
    dialog->addStyleClass("[&>.dialog-layout]:bg-gray-800 [&>.dialog-layout]:rounded-md");
    dialog->titleBar()->setStyleClass("px-2 py-0 flex items-center justify-center overflow-x-visible h-[40px] bg-gray-900 text-white font-bold");
    dialog->titleBar()->addStyleClass("bg-gray-800 hover:bg-gray-700 border border-gray-600 rounded-t-md cursor-pointer");
    dialog->contents()->setStyleClass("bg-gray-800 text-white p-4 rounded-b-md flex flex-col items-end min-w-sm");
    auto closeBtn = dialog->titleBar()->addWidget(std::make_unique<Wt::WPushButton>("X"));
    closeBtn->clicked().connect(dialog, [=]() { dialog->reject(); });
    closeBtn->addStyleClass("ml-auto !border-gray-700 text-gray-600 hover:bg-gray-600 hover:text-white rounded-md ");
    dialog->setResizable(false);
    dialog->setMovable(true);
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("Folder name:"));
    label->setStyleClass("w-full block");
    auto input = dialog->contents()->addWidget(std::make_unique<Wt::WLineEdit>());
    input->setStyleClass("w-full");

    auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
    errorText->setStyleClass("w-full block text-red-500 mt-2");
    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto createBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Create"));
    createBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");
    
    input->enterPressed().connect([=]() { 
        createBtn->clicked().emit(Wt::WMouseEvent());
    });

    createBtn->clicked().connect([=]() {
        std::string folderName = input->text().toUTF8();
        if (folderName.empty())
        {
            errorText->setText("Folder name cannot be empty.");
            input->setFocus();  
            return;
        }

        // Check for duplicate folder names
        {
            Wt::Dbo::Transaction t(*session_);
            auto existingFolder = session_->find<TemplateFolder>().where("folder_name = ?").bind(folderName).resultList();
            if (!existingFolder.empty())
            {
                errorText->setText("A folder with this name already exists.");
                input->setFocus();
                return;
            }
            t.commit();
        }

        Wt::Dbo::Transaction t(*session_);
        auto newFolder = session_->add(std::make_unique<TemplateFolder>());
        newFolder.modify()->folderName_ = folderName;
        newFolder.modify()->expanded_ = true;
        t.commit();
        wApp->log("debug") << "Created new folder: " << folderName;
        
        changed_.emit();
        dialog->accept();
    });

    dialog->show();
}


void RootNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "FileNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->setStyleClass("rounded-md");
        popup_->addItem("New Folder")->triggered().connect(this, &RootNode::createNewFolderDialog);
        popup_->setHideOnSelect(true);
        popup_->setAutoHide(true, 400);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}


}
