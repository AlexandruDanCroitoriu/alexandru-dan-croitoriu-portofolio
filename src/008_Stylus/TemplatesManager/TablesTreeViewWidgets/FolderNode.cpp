#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/FolderNode.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/Tables/TemplateFile.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WIconPair.h>
#include <Wt/WPoint.h>
#include <Wt/WTemplate.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTree.h>
#include <Wt/WIcon.h>

namespace Stylus
{

FolderNode::FolderNode(StylusSession& session, Wt::Dbo::ptr<TemplateFolder> folder)
    : Wt::WTreeNode(folder->folderName_),
      folder_(folder),
    session_(session)
{
    wApp->log("debug") << "FolderNode::FolderNode(" << folder->folderName_ << ")";
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");
    addStyleClass("[&>.Wt-item]:hover:bg-gray-700");
    label_wrapper_ = labelArea();
    label_wrapper_->addStyleClass("flex items-center cursor-pointer ");
    setSelectable(true);
    
    auto arrowUp = label_wrapper_->addNew<Wt::WIcon>("arrow-up");
    auto arrowDown = label_wrapper_->addNew<Wt::WIcon>("arrow-down");
    arrowDown->hide();
    arrowUp->hide();
    std::string arrowStyles = "border border-gray-600 rounded-md p-0.5 cursor-pointer hover:bg-gray-900 hover:text-white";
    arrowUp->setStyleClass(arrowStyles + " mr-1 ml-auto");
    arrowDown->setStyleClass(arrowStyles);
    
    arrowUp->clicked().connect(this, &FolderNode::moveFolderUp);
    arrowDown->clicked().connect(this, &FolderNode::moveFolderDown);
    

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            tree()->select(this);
            showPopup(event);
        }
    });

    label_wrapper_->doubleClicked().connect(this, [=]() 
    {
        if(isExpanded()){
            collapsed().emit(Wt::WMouseEvent());
        }else{
            expanded().emit(Wt::WMouseEvent());
        }
    });

    selected().connect(this, [=](bool selected)
    {
        if (selected)
        {
            wApp->log("debug") << "FolderNode selected: " << folder_->folderName_;
            arrowUp->show();
            arrowDown->show();
        }else 
        {
            arrowUp->hide();
            arrowDown->hide();
        }
    });

    expanded().connect(this, [=]()
    {
        if (folder_)
        {
            Wt::Dbo::Transaction t(session_);
            folder_.modify()->expanded_ = true;
            t.commit();
        }
    });

    collapsed().connect(this, [=]()
    {
        if (folder_)
        {
            Wt::Dbo::Transaction t(session_);
            folder_.modify()->expanded_ = false;
            t.commit();
        }
    });

    if(folder_ && folder_->expanded_)
        expand();

    setLabelIcon(std::make_unique<Wt::WIconPair>("./static/icons/folder-closed.png",
                                                 "./static/icons/folder-open.png"));
}

void FolderNode::createRenameFolderDialog()
{
    wApp->log("debug") << "FolderNode::createRenameFolderDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename folder"));
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
    auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("New Folder name:"));
    label->setStyleClass("w-full block");
    auto input = dialog->contents()->addWidget(std::make_unique<Wt::WLineEdit>(folder_->folderName_));
    input->setStyleClass("w-full");

    auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
    errorText->setStyleClass("w-full block text-red-500 mt-2");
    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto renameBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Rename Folder"));
    renameBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");
    
    input->enterPressed().connect([=]() { 
        renameBtn->clicked().emit(Wt::WMouseEvent());
    });
    renameBtn->clicked().connect([=]() {
        // Perform folder rename logic here
        std::string folderName = input->text().toUTF8();
        if (folderName.empty())
        {
            errorText->setText("Folder name cannot be empty.");
            input->setFocus();
            return;
        }
        {
            Wt::Dbo::Transaction t(session_);
            auto existingFolder = session_.find<TemplateFolder>().where("folder_name = ?").bind(folderName).resultList();
            if (!existingFolder.empty())
            {
                errorText->setText("A folder with this name already exists.");
                input->setFocus();
                return;
            }
            folder_.modify()->folderName_ = folderName;
            t.commit();
        }
        dialog->accept();
        changed_.emit();
    });
    dialog->show();
}

void FolderNode::createRemoveFolderDialog()
{
    wApp->log("debug") << "FolderNode::createRemoveFolderDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Delete File"));
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
    
    auto errorTemplate = dialog->contents()->addWidget(std::make_unique<Wt::WTemplate>());

    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto deleteBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Delete File"));
    deleteBtn->addStyleClass("bg-red-600 hover:bg-red-700 text-white px-4 py-2 rounded-md self-end");
    deleteBtn->addStyleClass("disabled:opacity-50 disabled:cursor-not-allowed");

    Wt::WString templateText;
    if(folder_)
    {
        Wt::Dbo::Transaction t(session_);
        auto files = folder_->files_;
        if(!files.empty())
        {
            templateText = "<div>"
                         "<div>Cannot delete folder '<strong>"
                         + folder_->folderName_
                         + "</strong>' because it contains files.</div>"
                         "<div>Please remove all files before deleting the folder.</div>"
                         "</div>";
            for (const auto& filePtr : files)
            {
                templateText += "<div class='mt-2'>- " + filePtr->fileName_ + "</div>";
            }
            errorTemplate->setTemplateText(templateText, Wt::TextFormat::UnsafeXHTML);
            deleteBtn->setDisabled(true);
            errorTemplate->setStyleClass("w-full block text-red-500 mt-2");
        }else {
            templateText = "<div>Are you sure you want to delete the folder '<strong>" + folder_->folderName_ + "</strong>'?</div>";
            errorTemplate->setTemplateText(templateText, Wt::TextFormat::UnsafeXHTML);
            errorTemplate->setStyleClass("w-full block mt-2");
        }
        t.commit();
    }

    deleteBtn->clicked().connect([=]() {
        // Perform folder deletion logic here
        Wt::Dbo::Transaction t(session_);
        folder_.remove();
        t.commit();
        dialog->accept();
        changed_.emit();
    });

    dialog->show();

}

void FolderNode::createNewFileDialog()
{
    wApp->log("debug") << "FolderNode::createNewFileDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new File"));
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
    auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("File name:"));
    label->setStyleClass("w-full block");
    auto input = dialog->contents()->addWidget(std::make_unique<Wt::WLineEdit>());
    input->setStyleClass("w-full");

    auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
    errorText->setStyleClass("w-full block text-red-500 mt-2");
    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto createBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Create File"));
    createBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");
    
    input->enterPressed().connect([=]() { 
        createBtn->clicked().emit(Wt::WMouseEvent());
    });
    createBtn->clicked().connect([=]() {
        std::string fileName = input->text().toUTF8();
        if (fileName.empty())
        {
            errorText->setText("File name cannot be empty.");
            return;
        }

        // Check for duplicate file names within the folder
        {
            Wt::Dbo::Transaction t(session_);
            auto existingFile = session_.find<TemplateFile>()
                .where("file_name = ? AND folder_id = ?")
                .bind(fileName)
                .bind(folder_.id())
                .resultList();
            if (!existingFile.empty())
            {
                errorText->setText("A file with this name already exists in the folder.");
                return;
            }

            // Determine next order index within the folder (1..n)
            auto currentFiles = session_.find<TemplateFile>()
                .where("folder_id = ?")
                .bind(folder_.id())
                .resultList();
            int nextOrder = static_cast<int>(currentFiles.size()) + 1;

            // Create and persist the new file
            auto newFile = session_.addNew<TemplateFile>();
            newFile.modify()->fileName_ = fileName;
            newFile.modify()->expanded_ = false;
            newFile.modify()->folder_ = folder_;
            newFile.modify()->order = nextOrder;
            folder_.modify()->expanded_ = true; // Ensure folder is expanded to show new file
            t.commit();
        }

        changed_.emit();
        dialog->accept();
    });
    dialog->show();
}

void FolderNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "FolderNode::showPopup";

    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->setStyleClass("rounded-md");
        popup_->addItem("New file")->triggered().connect(this, &FolderNode::createNewFileDialog);
        popup_->addSeparator();
        popup_->addItem("Rename folder")->triggered().connect(this, &FolderNode::createRenameFolderDialog);
        popup_->addSeparator();
        popup_->addItem("Delete folder")->triggered().connect(this, &FolderNode::createRemoveFolderDialog);
        popup_->setHideOnSelect(true);
        popup_->setAutoHide(true, 400);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

void FolderNode::moveFolderUp()
{
    if (!folder_)
        return;

    Wt::Dbo::Transaction t(session_);
    
    int currentOrder = folder_->order;
    
    // If folder is already at order 1, cannot move up
    if (currentOrder <= 1)
    {
        t.commit();
        return;
    }
    
    // Find the folder with order_index = currentOrder - 1
    auto folderToSwap = session_.find<TemplateFolder>()
        .where("order_index = ?")
        .bind(static_cast<long long>(currentOrder - 1))
        .resultValue();
    
    if (folderToSwap)
    {
        // Swap the order indices
        int tempOrder = folder_.modify()->order;
        folder_.modify()->order = folderToSwap.modify()->order;
        folderToSwap.modify()->order = tempOrder;
    }
    
    t.commit();
    changed_.emit();
}

void FolderNode::moveFolderDown()
{
    if (!folder_)
        return;

    Wt::Dbo::Transaction t(session_);
    
    int currentOrder = folder_->order;
    
    // Find the folder with order_index = currentOrder + 1
    auto folderToSwap = session_.find<TemplateFolder>()
        .where("order_index = ?")
        .bind(static_cast<long long>(currentOrder + 1))
        .resultValue();
    
    if (folderToSwap)
    {
        // Swap the order indices
        int tempOrder = folder_.modify()->order;
        folder_.modify()->order = folderToSwap.modify()->order;
        folderToSwap.modify()->order = tempOrder;
    }
    
    t.commit();
    changed_.emit();
}

}
