#include "008_Stylus/TemplatesManager/DboTempTreeView.h"
#include "008_Stylus/TemplatesManager/RootNode.h"
#include "008_Stylus/TemplatesManager/FolderNode.h"
#include "008_Stylus/TemplatesManager/FileNode.h"
#include "008_Stylus/TemplatesManager/TemplateNode.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/MessageTemplate.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>
#include <Wt/WPoint.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WText.h>
#include <Wt/WDialog.h>

namespace Stylus
{

DboTempTreeView::DboTempTreeView(StylusSession& session)
    : session_(session)
{
    wApp->log("debug") << "DboTempTreeView::DboTempTreeView";
    setStyleClass("w-full ");

    tree_ = addWidget(std::make_unique<Wt::WTree>());
    tree_->addStyleClass("relative -left-[18px]");
    tree_->setSelectionMode(Wt::SelectionMode::Single);
    populateTree();
}

void DboTempTreeView::refresh()
{
    wApp->log("debug") << "DboTempTreeView::refresh";
    populateTree();
}

void DboTempTreeView::populateTree()
{
    wApp->log("debug") << "DboTempTreeView::populateTree";
    auto root_node = std::make_unique<RootNode>(session_);
    auto root_ptr = root_node.get();
    root_ptr->setLoadPolicy(Wt::ContentLoading::Eager);

    tree_->setTreeRoot(std::move(root_node));

    Wt::Dbo::Transaction t(session_);
    auto folders = session_.find<TemplateFolder>().resultList();

    for (auto folder : folders)
    {
        auto folder_node = std::make_unique<FolderNode>(session_, folder);
        auto folder_ptr = folder_node.get();
        root_ptr->addChildNode(std::move(folder_node));

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

        for (auto file : folder->files_)
        {
            auto file_node = std::make_unique<FileNode>(session_, file);
            auto file_ptr = file_node.get();
            folder_ptr->addChildNode(std::move(file_node));

            // Add templates under the file
            for (auto tmpl : file->templates_)
            {
                auto template_node = std::make_unique<TemplateNode>(tmpl->messageId_, tmpl);
                auto template_ptr = template_node.get();
                file_ptr->addChildNode(std::move(template_node));

                template_ptr->selected().connect(this, [=](bool selected)
                {
                    if (selected)
                    {
                        template_selected_.emit(tmpl);
                    }
                });
            }

            file_ptr->selected().connect(this, [=](bool selected)
            {
                if (selected)
                {
                    file_selected_.emit(file);
                }
            });
        }
    }

    t.commit();
}


}
