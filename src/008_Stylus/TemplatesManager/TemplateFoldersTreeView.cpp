#include "008_Stylus/TemplatesManager/TemplateFoldersTreeView.h"
#include "008_Stylus/TemplatesManager/FolderNode.h"
#include "008_Stylus/TemplatesManager/FileNode.h"
#include "008_Stylus/TemplatesManager/TemplateNode.h"
#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/TemplateFolder.h"
#include "005_Dbo/Tables/TemplateFile.h"
#include "005_Dbo/Tables/MessageTemplate.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>

namespace Stylus
{

TemplateFoldersTreeView::TemplateFoldersTreeView(Session& session)
    : session_(session)
{
    wApp->log("debug") << "TemplateFoldersTreeView::TemplateFoldersTreeView";
    setStyleClass("w-full ");

    tree_ = addWidget(std::make_unique<Wt::WTree>());
    tree_->setSelectionMode(Wt::SelectionMode::Single);

    populateTree();
}

void TemplateFoldersTreeView::refresh()
{
    wApp->log("debug") << "TemplateFoldersTreeView::refresh";
    populateTree();
}

void TemplateFoldersTreeView::populateTree()
{
    wApp->log("debug") << "TemplateFoldersTreeView::populateTree";
    auto root_node = std::make_unique<FolderNode>("Templates");
    auto root_ptr = root_node.get();
    tree_->setTreeRoot(std::move(root_node));
    tree_->treeRoot()->setSelectable(false);
    tree_->treeRoot()->expand();

    Wt::Dbo::Transaction t(session_);
    auto folders = session_.find<TemplateFolder>().resultList();

    for (auto folder : folders)
    {
        auto folder_node = std::make_unique<FolderNode>(folder->folderName_, folder);
        auto folder_ptr = folder_node.get();
        root_ptr->addChildNode(std::move(folder_node));
        folder_ptr->expand();

        for (auto file : folder->files_)
        {
            auto file_node = std::make_unique<FileNode>(file->fileName_, file);
            auto file_ptr = file_node.get();
            folder_ptr->addChildNode(std::move(file_node));
            file_ptr->expand();
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
