#include "008_Stylus/TemplatesManager/TemplatesManager.h"
#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/RootNode.h"
#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/FolderNode.h"
#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/FileNode.h"
#include "008_Stylus/TemplatesManager/TablesTreeViewWidgets/TemplateNode.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/MessageTemplate.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/TempView.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/XmlBrain.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>
#include <Wt/WPoint.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WText.h>
#include <Wt/WDialog.h>

namespace Stylus
{

TemplatesManager::TemplatesManager(StylusSession& session)
    : session_(session)
{
    wApp->log("debug") << "TemplatesManager::TemplatesManager";
    setStyleClass("max-h-[100svh] flex overflow-hidden");
    
    auto treeWrapper = addNew<Wt::WContainerWidget>();
    treeWrapper->setStyleClass("h-[100svh] border-r border-gray-600 overflow-y-auto overflow-x-hidden max-w-sm min-w-[240px]");
    
    tree_ = treeWrapper->addWidget(std::make_unique<Wt::WTree>());
    tree_->addStyleClass("relative -left-[18px] w-[calc(100%+18px)]");
    tree_->setSelectionMode(Wt::SelectionMode::Single);

    auto contentWrapper_ = addNew<Wt::WContainerWidget>();
    contentWrapper_->setStyleClass("h-[100svh] overflow-auto p-4 grow flex flex-col items-stretch space-y-4");
    // Assign to member for later rendering
    this->contentWrapper_ = contentWrapper_;

    populateTree();
}


void TemplatesManager::populateTree()
{
    wApp->log("debug") << "TemplatesManager::populateTree";
    auto root_node = std::make_unique<RootNode>(session_);
    auto root_ptr = root_node.get();
    root_ptr->setLoadPolicy(Wt::ContentLoading::Eager);
    root_ptr->changed().connect(this, &TemplatesManager::populateTree);
    tree_->setTreeRoot(std::move(root_node));

    Wt::Dbo::Transaction t(session_);
    auto folders = session_.find<TemplateFolder>().orderBy("order_index").resultList();

    for (auto folder : folders)
    {
        auto folder_node = std::make_unique<FolderNode>(session_, folder);
        auto folder_ptr = folder_node.get();
        folder_node->changed().connect(this, &TemplatesManager::populateTree);
        root_ptr->addChildNode(std::move(folder_node));

        // Restore selection if this is the previously selected folder
        if (selectedKind_ == SelectedKind::Folder && selectedFolderId_ == folder.id())
        {
            tree_->select(folder_ptr);
        }

        // Track selection changes
        folder_ptr->selected().connect(this, [=](bool selected)
        {
            if (selected)
            {
                selectedKind_ = SelectedKind::Folder;
                selectedFolderId_ = folder.id();
                selectedFileId_ = -1;
                selectedTemplateId_ = -1;
                renderSelection();
            }
        });

        // When a folder is re-expanded, re-open any child files that were previously expanded.
        folder_ptr->expanded().connect([folder_ptr]() {
            for (auto child : folder_ptr->childNodes())
            {
                if (auto file_child = dynamic_cast<FileNode*>(child))
                {
                    if (file_child->file_ && file_child->file_->expanded_)
                    {
                        file_child->expand();
                    }
                }
            }
        });

        auto orderedFiles = session_.find<TemplateFile>()
            .where("folder_id = ?")
            .bind(folder.id())
            .orderBy("order_index")
            .resultList();

        for (auto file : orderedFiles)
        {
            auto file_node = std::make_unique<FileNode>(session_, file);
            auto file_ptr = file_node.get();
            file_ptr->changed().connect(this, &TemplatesManager::populateTree);
            folder_ptr->addChildNode(std::move(file_node));

            // Restore selection if this is the previously selected file
            if (selectedKind_ == SelectedKind::File && selectedFileId_ == file.id())
            {
                tree_->select(file_ptr);
            }

            // Add templates under the file (ordered)
            auto orderedTemplates = session_.find<MessageTemplate>()
                .where("file_id = ?")
                .bind(file.id())
                .orderBy("order_index")
                .resultList();

            for (auto tmpl : orderedTemplates)
            {
                auto template_node = std::make_unique<TemplateNode>(session_, tmpl);
                auto template_ptr = template_node.get();
                file_ptr->addChildNode(std::move(template_node));
                template_ptr->changed().connect(this, &TemplatesManager::populateTree);
                template_ptr->selected().connect(this, [=](bool selected)
                {
                    if (selected)
                    {
                        selectedKind_ = SelectedKind::Template;
                        selectedTemplateId_ = tmpl.id();
                        selectedFileId_ = file.id();
                        selectedFolderId_ = folder.id();
                        template_selected_.emit(tmpl);
                        renderSelection();
                    }
                });

                // Restore selection if this is the previously selected template
                if (selectedKind_ == SelectedKind::Template && selectedTemplateId_ == tmpl.id())
                {
                    tree_->select(template_ptr);
                }
            }

            file_ptr->selected().connect(this, [=](bool selected)
            {
                if (selected)
                {
                    selectedKind_ = SelectedKind::File;
                    selectedFileId_ = file.id();
                    selectedFolderId_ = folder.id();
                    selectedTemplateId_ = -1;
                    file_selected_.emit(file);
                    renderSelection();
                }
            });
        }
    }

    t.commit();
    renderSelection();
}


void TemplatesManager::renderSelection()
{
    if (!contentWrapper_)
        return;

    // Clear previous content
    contentWrapper_->clear();
    tempViews_.clear();
    
    Wt::Dbo::Transaction t(session_);

    switch (selectedKind_)
    {
    case SelectedKind::Template:
    {
        if (selectedTemplateId_ < 0) break;
        auto tmplList = session_.find<MessageTemplate>()
            .where("id = ?")
            .bind(selectedTemplateId_)
            .limit(1)
            .resultList();
        for (auto tmpl : tmplList)
        {
            auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(session_, tmpl));
            tempView->setViewMode(TemplateViewMode::Editor);
            tempViews_.push_back(tempView);
        }
        break;
    }
    case SelectedKind::File:
    {
        if (selectedFileId_ < 0) break;
        auto orderedTemplates = session_.find<MessageTemplate>()
            .where("file_id = ?")
            .bind(selectedFileId_)
            .orderBy("order_index")
            .resultList();
        for (auto tmpl : orderedTemplates)
        {
            auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(session_, tmpl));
            tempView->setViewMode(TemplateViewMode::Editor);
            tempViews_.push_back(tempView);
        }
        break;
    }
    case SelectedKind::Folder:
    {
        if (selectedFolderId_ < 0) break;
        auto orderedFiles = session_.find<TemplateFile>()
            .where("folder_id = ?")
            .bind(selectedFolderId_)
            .orderBy("order_index")
            .resultList();
        for (auto file : orderedFiles)
        {
            auto orderedTemplates = session_.find<MessageTemplate>()
                .where("file_id = ?")
                .bind(file.id())
                .orderBy("order_index")
                .resultList();
            for (auto tmpl : orderedTemplates)
            {
                auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(session_, tmpl));
                tempView->setViewMode(TemplateViewMode::Editor);
                tempViews_.push_back(tempView);
            }
        }
        break;
    }
    case SelectedKind::None:
    default:
        break;
    }

    t.commit();
}

void TemplatesManager::keyWentDown(Wt::WKeyEvent e)
{
    // for(auto tempView : tempViews_)
    // {
    //     if(tempView->xmlBrain_->selectedNode_)
    //         tempView->keyWentDown(e);
    // }
}


}
