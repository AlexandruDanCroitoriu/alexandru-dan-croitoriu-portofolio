#include "008_Stylus/TemplatesManager/FileNode.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/MessageTemplate.h"
#include "008_Stylus/Tables/TemplateFolder.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>
#include <Wt/WTree.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WIcon.h>
#include <Wt/WComboBox.h>

namespace Stylus
{

    FileNode::FileNode(StylusSession &session, Wt::Dbo::ptr<TemplateFile> file)
        : Wt::WTreeNode(file->fileName_),
          file_(file),
          session_(session)
    {
        wApp->log("debug") << "FileNode::FileNode(" << file_->fileName_ << ")";
        label_wrapper_ = labelArea();
        addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");
        addStyleClass("[&>.Wt-item]:hover:bg-gray-700");

        label_wrapper_->addStyleClass("flex items-center cursor-pointer ");
        setSelectable(true);

        auto arrowUp = label_wrapper_->addNew<Wt::WIcon>("arrow-up");
        auto arrowDown = label_wrapper_->addNew<Wt::WIcon>("arrow-down");
        arrowDown->hide();
        arrowUp->hide();
        std::string arrowStyles = "border border-gray-600 rounded-md p-0.5 cursor-pointer hover:bg-gray-900 hover:text-white";
        arrowUp->setStyleClass(arrowStyles + " mr-1 ml-auto");
        arrowDown->setStyleClass(arrowStyles);

        arrowUp->clicked().connect(this, &FileNode::moveFileUp);
        arrowDown->clicked().connect(this, &FileNode::moveFileDown);

        label_wrapper_->doubleClicked().connect(this, [=]()
                                                {
        if(isExpanded())
            collapsed().emit(Wt::WMouseEvent());
        else
            expanded().emit(Wt::WMouseEvent()); });

        label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent &event)
                                              {
        // if (event.button() == Wt::MouseButton::Right && tree()->isSelected(this))
        // {
        //     showPopup(event);
        // }
        if (event.button() == Wt::MouseButton::Right)
        {
            tree()->select(this);
            showPopup(event);
        } });

        setLabelIcon(std::make_unique<Wt::WIconPair>("./static/icons/file-closed.svg", "./static/icons/file-open.svg"));

        if (file_ && file_->expanded_)
            expand();

        selected().connect(this, [=](bool selected)
                           {
        if (selected)
        {
            arrowUp->show();
            arrowDown->show();
            wApp->log("debug") << "FileNode selected: " << file_->fileName_;
        }
        else
        {
            arrowUp->hide();
            arrowDown->hide();
        } });

        expanded().connect(this, [=]()
                           {
        if (file_)
        {
            Wt::Dbo::Transaction t(session_);
            file_.modify()->expanded_ = true;
            t.commit();
        } });

        collapsed().connect(this, [=]()
                            {
        if (file_)
        {
            Wt::Dbo::Transaction t(session_);
            file_.modify()->expanded_ = false;
            t.commit();
        }
        std::cout << "\n\nFileNode::collapsed()\n\n"; });
    }

    void FileNode::createNewTemplateDialog()
    {
        wApp->log("debug") << "RootNode::createNewFolderDialog()";
        auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create Template"));
        dialog->addStyleClass("text-white border border-gray-600 rounded-md !bg-gray-800");
        dialog->addStyleClass("[&>.dialog-layout]:bg-gray-800 [&>.dialog-layout]:rounded-md");
        dialog->titleBar()->setStyleClass("px-2 py-0 flex items-center justify-center overflow-x-visible h-[40px] bg-gray-900 text-white font-bold");
        dialog->titleBar()->addStyleClass("bg-gray-800 hover:bg-gray-700 border border-gray-600 rounded-t-md cursor-pointer");
        dialog->contents()->setStyleClass("bg-gray-800 text-white p-4 rounded-b-md flex flex-col items-end min-w-sm");
        auto closeBtn = dialog->titleBar()->addWidget(std::make_unique<Wt::WPushButton>("X"));
        closeBtn->clicked().connect(dialog, [=]()
                                    { dialog->reject(); });
        closeBtn->addStyleClass("ml-auto !border-gray-700 text-gray-600 hover:bg-gray-600 hover:text-white rounded-md ");
        dialog->setResizable(false);
        dialog->setMovable(true);
        dialog->setModal(true);
        dialog->rejectWhenEscapePressed();
        auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("Template ID:"));
        label->setStyleClass("w-full block");
        auto input = dialog->contents()->addWidget(std::make_unique<Wt::WLineEdit>());
        input->setStyleClass("w-full");

        auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
        errorText->setStyleClass("w-full block text-red-500 mt-2");
        dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
        auto createBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Create Template"));
        createBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");

        input->enterPressed().connect([=]()
                                      { createBtn->clicked().emit(Wt::WMouseEvent()); });

        createBtn->clicked().connect([=]() 
                                     {
        wApp->log("debug") << "FileNode::createNewTemplateDialog() - Create button clicked";
        std::string templateName = input->text().toUTF8();
        if (templateName.empty())
        {
            errorText->setText("Template name cannot be empty.");
            input->setFocus();  
            return;
        }

        // Create and persist the new template with next order index
        {
            Wt::Dbo::Transaction t(session_);

            int nextOrder = 1;
            auto maxTemplate = session_.find<MessageTemplate>()
                .where("file_id = ?")
                .bind(file_.id())
                .orderBy("order_index DESC")
                .limit(1)
                .resultValue();
            if (maxTemplate)
            {
                nextOrder = maxTemplate->order + 1;
            }

            auto newTemplate = session_.addNew<MessageTemplate>();
            newTemplate.modify()->messageId_ = templateName;
            newTemplate.modify()->file_ = file_;
            newTemplate.modify()->order = nextOrder;
            file_.modify()->expanded_ = true; // Ensure file is expanded to show new template
            t.commit();
        }
        
        dialog->accept();
        changed_.emit(); });
        dialog->show();
    }

    void FileNode::createRenameFileDialog()
    {
        wApp->log("debug") << "FileNode::createRenameFileDialog()";
        auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename File"));
        dialog->addStyleClass("text-white border border-gray-600 rounded-md !bg-gray-800");
        dialog->addStyleClass("[&>.dialog-layout]:bg-gray-800 [&>.dialog-layout]:rounded-md ");
        dialog->titleBar()->setStyleClass("px-2 py-0 flex items-center justify-center overflow-x-visible h-[40px] bg-gray-900 text-white font-bold");
        dialog->titleBar()->addStyleClass("bg-gray-800 hover:bg-gray-700 border border-gray-600 rounded-t-md cursor-pointer");
        dialog->contents()->setStyleClass("bg-gray-800 text-white p-4 rounded-b-md flex flex-col items-end min-w-sm min-h-fit");
        auto closeBtn = dialog->titleBar()->addWidget(std::make_unique<Wt::WPushButton>("X"));
        closeBtn->clicked().connect(dialog, [=]()
                                    { dialog->reject(); });
        closeBtn->addStyleClass("ml-auto !border-gray-700 text-gray-600 hover:bg-gray-600 hover:text-white rounded-md ");
        dialog->setResizable(false);
        dialog->setMovable(true);
        dialog->setModal(true);
        dialog->rejectWhenEscapePressed();
        auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("New File name:"));
        label->setStyleClass("w-full block");
        auto input = dialog->contents()->addWidget(std::make_unique<Wt::WLineEdit>(file_->fileName_));
        input->setStyleClass("w-full");

        auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
        errorText->setStyleClass("w-full block text-red-500 mt-2");
        dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
        auto renameBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Rename File"));
        renameBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");

        input->enterPressed().connect([=]()
                                      { renameBtn->clicked().emit(Wt::WMouseEvent()); });
        renameBtn->clicked().connect([=]()
                                     {
        // Perform file rename logic here
        std::string fileName = input->text().toUTF8();
        if (fileName.empty())
        {
            errorText->setText("File name cannot be empty.");
            input->setFocus();
            return;
        }
        {
            Wt::Dbo::Transaction t(session_);
            auto existingFile = session_.find<TemplateFile>().where("file_name = ? AND folder_id = ?").bind(fileName).bind(file_->folder_.id()).resultList();
            if (!existingFile.empty())
            {
                errorText->setText("A file with this name already exists.");
                input->setFocus();
                return;
            }
            file_.modify()->fileName_ = fileName;
            t.commit();
        }
        
        dialog->accept();
        changed_.emit(); });
        dialog->show();
    }

    void FileNode::createDeleteFileDialog()
    {
    wApp->log("debug") << "FileNode::createDeleteFileDialog";
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
    if(file_)
    {
        Wt::Dbo::Transaction t(session_);
        auto dboTemplates = file_->templates_;
        if(!dboTemplates.empty())
        {
            templateText = "<div>"
                         "<div>Cannot delete file '<strong>"
                         + file_->fileName_
                         + "</strong>' because it contains templates.</div>"
                         "<div>Please remove all templates before deleting the file.</div>"
                         "</div>";
            for (const auto& dboTemp : dboTemplates)
            {
                templateText += "<div class='mt-2'>- " + dboTemp->messageId_ + "</div>";
            }
            errorTemplate->setTemplateText(templateText, Wt::TextFormat::UnsafeXHTML);
            deleteBtn->setDisabled(true);
            errorTemplate->setStyleClass("w-full block text-red-500 mt-2");
        }else {
            templateText = "<div>Are you sure you want to delete the folder '<strong>" + file_->fileName_ + "</strong>'?</div>";
            errorTemplate->setTemplateText(templateText, Wt::TextFormat::UnsafeXHTML);
            errorTemplate->setStyleClass("w-full block mt-2");
        }
        t.commit();
    }

    deleteBtn->clicked().connect([=]() {
        // Perform file deletion logic here
        Wt::Dbo::Transaction t(session_);
        auto folder = file_->folder_;
        auto folderId = folder ? folder.id() : -1;
        file_.remove();

        // Renormalize remaining files in the folder to keep order indexes contiguous
        if (folderId != -1)
        {
            auto remainingFiles = session_.find<TemplateFile>()
                .where("folder_id = ?")
                .bind(folderId)
                .orderBy("order_index")
                .resultList();

            int orderIndex = 1;
            for (auto remaining : remainingFiles)
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

    void FileNode::showPopup(const Wt::WMouseEvent &event)
    {
        wApp->log("debug") << "FileNode::showPopup(const Wt::WMouseEvent& event)";
        if (!popup_)
        {
            popup_ = std::make_unique<Wt::WPopupMenu>();
            popup_->setStyleClass("rounded-md");
            popup_->addItem("New template")->triggered().connect(this, &FileNode::createNewTemplateDialog);
            popup_->addSeparator();
            popup_->addItem("Rename file")->triggered().connect(this, &FileNode::createRenameFileDialog);
            popup_->addItem("Change folder")->triggered().connect(this, &FileNode::createChangeFolderDialog);
            popup_->addSeparator();
            popup_->addItem("Delete file")->triggered().connect(this, &FileNode::createDeleteFileDialog);
            popup_->setHideOnSelect(true);
            popup_->setAutoHide(true, 400);
        }

        popup_->popup(Wt::WPoint(event.document().x, event.document().y));
    }

    void FileNode::createChangeFolderDialog()
    {
        wApp->log("debug") << "FileNode::createChangeFolderDialog()";
        if (!file_ || !file_->folder_)
            return;

        auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Change Folder"));
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

        auto label = dialog->contents()->addWidget(std::make_unique<Wt::WText>("Select destination folder:"));
        label->setStyleClass("w-full block");
        auto combo = dialog->contents()->addWidget(std::make_unique<Wt::WComboBox>());
        // combo->setThemeStyleEnabled(true);
        combo->addStyleClass("bg-gray-800");

        // Load folders ordered by order_index
        std::vector<long long> folderIds;
        {
            Wt::Dbo::Transaction t(session_);
            auto folders = session_.find<TemplateFolder>().orderBy("order_index").resultList();
            for (auto f : folders)
            {
                combo->addItem(f->folderName_);
                folderIds.push_back(f.id());
                if (file_->folder_ && f.id() == file_->folder_.id())
                {
                    combo->setCurrentIndex(static_cast<int>(folderIds.size()-1));
                }
            }
            t.commit();
        }

        auto errorText = dialog->contents()->addWidget(std::make_unique<Wt::WText>(""));
        errorText->setStyleClass("w-full block text-red-500 mt-2");
        dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>())->setStyleClass("border w-full border-gray-600 my-2");
        auto moveBtn = dialog->contents()->addWidget(std::make_unique<Wt::WPushButton>("Move File"));
        moveBtn->addStyleClass("bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md self-end");

        moveBtn->clicked().connect([=]() {
            int idx = combo->currentIndex();
            if (idx < 0 || idx >= static_cast<int>(folderIds.size()))
            {
                errorText->setText("Please select a destination folder.");
                return;
            }

            long long destFolderId = folderIds[idx];
            long long srcFolderId = file_->folder_.id();
            if (destFolderId == srcFolderId)
            {
                errorText->setText("This file is already in the selected folder.");
                return;
            }

            Wt::Dbo::Transaction t(session_);

            // Check if destination folder already has a file with the same name
            auto existingFile = session_.find<TemplateFile>()
                .where("folder_id = ? AND file_name = ?")
                .bind(destFolderId)
                .bind(file_->fileName_)
                .resultValue();
            
            if (existingFile)
            {
                errorText->setText("A file with the name '" + file_->fileName_ + "' already exists in the selected folder.");
                t.commit();
                return;
            }

            // Shift destination folder files by +1 to make room at order 1
            auto destFilesDesc = session_.find<TemplateFile>()
                .where("folder_id = ?")
                .bind(destFolderId)
                .orderBy("order_index DESC")
                .resultList();
            for (auto df : destFilesDesc)
            {
                df.modify()->order = df->order + 1;
            }
            
            // Move current file to destination folder at order 1
            auto destFolderPtr = session_.load<TemplateFolder>(destFolderId);
            file_.modify()->folder_ = destFolderPtr;
            file_.modify()->order = 1;
            destFolderPtr.modify()->expanded_ = true; // Ensure destination folder is expanded to show moved file

            // Renormalize source folder orders after removing this file
            auto srcFiles = session_.find<TemplateFile>()
                .where("folder_id = ?")
                .bind(srcFolderId)
                .orderBy("order_index")
                .resultList();
            int orderIndex = 1;
            for (auto sf : srcFiles)
            {
                sf.modify()->order = orderIndex++;
            }

            t.commit();
            dialog->accept();
            changed_.emit();
        });

        dialog->show();
    }

    void FileNode::moveFileUp()
    {
        if (!file_ || !file_->folder_)
            return;

        Wt::Dbo::Transaction t(session_);

        int currentOrder = file_->order;
        if (currentOrder <= 1)
        {
            t.commit();
            return;
        }

        auto neighbor = session_.find<TemplateFile>()
            .where("folder_id = ? AND order_index = ?")
            .bind(file_->folder_.id())
            .bind(static_cast<long long>(currentOrder - 1))
            .resultValue();

        if (neighbor)
        {
            int tempOrder = file_.modify()->order;
            file_.modify()->order = neighbor.modify()->order;
            neighbor.modify()->order = tempOrder;
        }

        t.commit();
        changed_.emit();
    }

    void FileNode::moveFileDown()
    {
        if (!file_ || !file_->folder_)
            return;

        Wt::Dbo::Transaction t(session_);

        int currentOrder = file_->order;

        auto neighbor = session_.find<TemplateFile>()
            .where("folder_id = ? AND order_index = ?")
            .bind(file_->folder_.id())
            .bind(static_cast<long long>(currentOrder + 1))
            .resultValue();

        if (neighbor)
        {
            int tempOrder = file_.modify()->order;
            file_.modify()->order = neighbor.modify()->order;
            neighbor.modify()->order = tempOrder;
        }

        t.commit();
        changed_.emit();
    }

}
