#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/TemplateNode.h"
#include "008_Stylus/Tables/MessageTemplate.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/StylusSession.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>
#include <Wt/WTree.h>
#include <Wt/WComboBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WIcon.h>

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

    auto arrowUp = label_wrapper_->addNew<Wt::WIcon>("arrow-up");
    auto arrowDown = label_wrapper_->addNew<Wt::WIcon>("arrow-down");
    arrowDown->hide();
    arrowUp->hide();
    std::string arrowStyles = "border border-gray-600 rounded-md p-0.5 cursor-pointer hover:bg-gray-900 hover:text-white";
    arrowUp->setStyleClass(arrowStyles + " mr-1 ml-auto");
    arrowDown->setStyleClass(arrowStyles);

    arrowUp->clicked().connect(this, &TemplateNode::moveTemplateUp);
    arrowDown->clicked().connect(this, &TemplateNode::moveTemplateDown);

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

    selected().connect(this, [=](bool selected)
    {
        if (selected)
        {
            arrowUp->show();
            arrowDown->show();
        }
        else
        {
            arrowUp->hide();
            arrowDown->hide();
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

void TemplateNode::createChangeFileDialog()
{
    wApp->log("debug") << "TemplateNode::createChangeFileDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Change File"));
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

    // Folder selection
    auto folderLabel = dialog->contents()->addWidget(std::make_unique<Wt::WText>("Select destination folder:"));
    folderLabel->setStyleClass("w-full block");
    auto folderCombo = dialog->contents()->addWidget(std::make_unique<Wt::WComboBox>());
    folderCombo->addStyleClass("bg-gray-800");

    // File selection
    auto fileLabel = dialog->contents()->addWidget(std::make_unique<Wt::WText>("Select destination file:"));
    fileLabel->setStyleClass("w-full block mt-2");
    auto fileCombo = dialog->contents()->addWidget(std::make_unique<Wt::WComboBox>());
    fileCombo->addStyleClass("bg-gray-800");

    // Store folder IDs and file IDs
    std::shared_ptr<std::vector<long long>> folderIds = std::make_shared<std::vector<long long>>();
    std::shared_ptr<std::vector<long long>> fileIds = std::make_shared<std::vector<long long>>();

    // Load folders
    {
        Wt::Dbo::Transaction t(session_);
        auto folders = session_.find<TemplateFolder>().orderBy("order_index").resultList();
        long long currentFolderId = -1;
        if (messageTemplate_->file_ && messageTemplate_->file_->folder_)
        {
            currentFolderId = messageTemplate_->file_->folder_.id();
        }

        for (auto f : folders)
        {
            folderCombo->addItem(f->folderName_);
            folderIds->push_back(f.id());
            if (f.id() == currentFolderId)
            {
                folderCombo->setCurrentIndex(static_cast<int>(folderIds->size()-1));
            }
        }
        t.commit();
    }

    // Lambda to populate file combo based on selected folder
    auto populateFiles = [=]() {
        fileCombo->clear();
        fileIds->clear();

        int folderIdx = folderCombo->currentIndex();
        if (folderIdx < 0 || folderIdx >= static_cast<int>(folderIds->size()))
            return;

        long long selectedFolderId = (*folderIds)[folderIdx];
        long long currentFileId = messageTemplate_->file_ ? messageTemplate_->file_.id() : -1;

        Wt::Dbo::Transaction t(session_);
        auto files = session_.find<TemplateFile>()
            .where("folder_id = ?")
            .bind(selectedFolderId)
            .orderBy("order_index")
            .resultList();

        for (auto f : files)
        {
            fileCombo->addItem(f->fileName_);
            fileIds->push_back(f.id());
            if (f.id() == currentFileId)
            {
                fileCombo->setCurrentIndex(static_cast<int>(fileIds->size()-1));
            }
        }
        t.commit();
    };

    // Populate files for initially selected folder
    populateFiles();

    // Update files when folder changes
    folderCombo->changed().connect([=]() {
        populateFiles();
    });

    auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
    errorText->setStyleClass("w-full block text-red-500 mt-2");
    dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
    auto moveBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Move Template"));
    moveBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");

    moveBtn->clicked().connect([=]() {
        int fileIdx = fileCombo->currentIndex();
        if (fileIdx < 0 || fileIdx >= static_cast<int>(fileIds->size()))
        {
            errorText->setText("Please select a destination file.");
            return;
        }

        long long destFileId = (*fileIds)[fileIdx];
        long long srcFileId = messageTemplate_->file_.id();
        if (destFileId == srcFileId)
        {
            errorText->setText("This template is already in the selected file.");
            return;
        }

        Wt::Dbo::Transaction t(session_);

        // Shift destination file templates by +1 to make room at order 1
        auto destTemplatesDesc = session_.find<MessageTemplate>()
            .where("file_id = ?")
            .bind(destFileId)
            .orderBy("order_index DESC")
            .resultList();
        for (auto dt : destTemplatesDesc)
        {
            dt.modify()->order = dt->order + 1;
        }
        
        // Move current template to destination file at order 1
        auto destFilePtr = session_.load<TemplateFile>(destFileId);
        messageTemplate_.modify()->file_ = destFilePtr;
        messageTemplate_.modify()->order = 1;
        // Ensure destination parents are expanded to reveal moved template
        destFilePtr.modify()->expanded_ = true;
        if (destFilePtr->folder_)
        {
            destFilePtr->folder_.modify()->expanded_ = true;
        }

        // Renormalize source file orders after removing this template
        auto srcTemplates = session_.find<MessageTemplate>()
            .where("file_id = ?")
            .bind(srcFileId)
            .orderBy("order_index")
            .resultList();
        int orderIndex = 1;
        for (auto st : srcTemplates)
        {
            st.modify()->order = orderIndex++;
        }

        t.commit();
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
        auto file = messageTemplate_->file_;
        auto fileId = file ? file.id() : -1;
        messageTemplate_.remove();

        if (fileId != -1)
        {
            auto remainingTemplates = session_.find<MessageTemplate>()
                .where("file_id = ?")
                .bind(fileId)
                .orderBy("order_index")
                .resultList();

            int orderIndex = 1;
            for (auto remaining : remainingTemplates)
            {
                remaining.modify()->order = orderIndex++;
            }
        }

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
        popup_->setStyleClass("rounded-md");
        popup_->addItem("Rename template")->triggered().connect(this, &TemplateNode::createRenameTemplateDialog);
        popup_->addItem("Change file")->triggered().connect(this, &TemplateNode::createChangeFileDialog);
        popup_->addSeparator();
        popup_->addItem("Delete template")->triggered().connect(this, &TemplateNode::createRemoveTemplateDialog);
        popup_->setHideOnSelect(true);
        popup_->setAutoHide(true, 400);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

void TemplateNode::moveTemplateUp()
{
    if (!messageTemplate_ || !messageTemplate_->file_)
        return;

    Wt::Dbo::Transaction t(session_);

    int currentOrder = messageTemplate_->order;
    if (currentOrder <= 1)
    {
        t.commit();
        return;
    }

    auto neighbor = session_.find<MessageTemplate>()
        .where("file_id = ? AND order_index = ?")
        .bind(messageTemplate_->file_.id())
        .bind(static_cast<long long>(currentOrder - 1))
        .resultValue();

    if (neighbor)
    {
        int tempOrder = messageTemplate_.modify()->order;
        messageTemplate_.modify()->order = neighbor.modify()->order;
        neighbor.modify()->order = tempOrder;
    }

    t.commit();
    changed_.emit();
}

void TemplateNode::moveTemplateDown()
{
    if (!messageTemplate_ || !messageTemplate_->file_)
        return;

    Wt::Dbo::Transaction t(session_);

    int currentOrder = messageTemplate_->order;

    auto neighbor = session_.find<MessageTemplate>()
        .where("file_id = ? AND order_index = ?")
        .bind(messageTemplate_->file_.id())
        .bind(static_cast<long long>(currentOrder + 1))
        .resultValue();

    if (neighbor)
    {
        int tempOrder = messageTemplate_.modify()->order;
        messageTemplate_.modify()->order = neighbor.modify()->order;
        neighbor.modify()->order = tempOrder;
    }

    t.commit();
    changed_.emit();
}

}
