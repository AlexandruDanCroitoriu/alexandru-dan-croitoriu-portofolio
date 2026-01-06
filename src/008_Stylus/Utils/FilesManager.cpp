#include "008_Stylus/Utils/FilesManager.h"
#include "008_Stylus/TemplatesManager/FolderNode.h"
#include "008_Stylus/TemplatesManager/FileNode.h"
#include "008_Stylus/TemplatesManager/TemplateNode.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/MessageTemplate.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WGridLayout.h>

namespace Stylus
{

FilesManagerSidebar::FilesManagerSidebar()
{
    setStyleClass("flex flex-col max-h-screen select-none");
    setLayoutSizeAware(true);
    setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength::Auto);
    setMaximumSize(Wt::WLength(1000, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));

    contents_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    contents_->setStyleClass("w-full flex-[1] overflow-y-auto overflow-x-hidden flex flex-col stylus-scrollbar");
}

void FilesManagerSidebar::layoutSizeChanged(int width, int height)
{
    if (width >= 240 && width <= 1000)
    {
        width_changed_.emit(Wt::WString(std::to_string(width)));
    }
}

FilesManager::FilesManager(StylusSession &session, int sidebar_width)
    : session_(session)
{
    auto grid_layout = std::make_unique<Wt::WGridLayout>();

    sidebar_ = grid_layout->addWidget(std::make_unique<FilesManagerSidebar>(), 0, 0);
    sidebar_->addStyleClass("z-[100]");

    editor_ = grid_layout->addWidget(std::make_unique<MonacoEditor>("xml"), 0, 1);
    editor_->setStyleClass("z-[99] h-screen");

    grid_layout->setColumnResizable(0, true, Wt::WLength(sidebar_width, Wt::LengthUnit::Pixel));
    grid_layout->setContentsMargins(0, 0, 0, 0);

    grid_layout_ = grid_layout.get();
    setLayout(std::move(grid_layout));

    tree_ = sidebar_->contents_->addWidget(std::make_unique<Wt::WTree>());
    tree_->setSelectionMode(Wt::SelectionMode::Single);

    sidebar_->width_changed().connect(this, [=](Wt::WString /*width*/)
                                      {
                                          // Width change handling can be added when resizing logic is defined.
                                      });

    editor_->saveFileSignal().connect(this, [=](std::string /*text*/)
                                      {
                                          file_saved_.emit(Wt::WString(selected_file_path_));
                                      });

    setTreeFolderWidgets();
}

void FilesManager::reuploadFile()
{
    if (!selected_file_)
    {
        editor_->setContent("// Select a file to view templates\n");
        return;
    }

    Wt::Dbo::Transaction t(session_);
    auto templates = selected_file_->templates_;

    if (templates.empty())
    {
        editor_->setContent("// No templates in this file yet\n");
        return;
    }

    auto firstTemplate = *templates.begin();
    editor_->setContent(firstTemplate->templateXml_);
}

void FilesManager::setTreeFolderWidgets()
{
    auto root_node = std::make_unique<FolderNode>(session_);
    auto root_ptr = root_node.get();
    tree_->setTreeRoot(std::move(root_node));
    tree_->treeRoot()->setSelectable(false);
    tree_->treeRoot()->expand();

    Wt::Dbo::Transaction t(session_);
    auto folders = session_.find<TemplateFolder>().resultList();

    for (auto folder : folders)
    {
        auto folder_node = std::make_unique<FolderNode>(session_, folder);
        auto folder_ptr = folder_node.get();
        root_ptr->addChildNode(std::move(folder_node));
        folder_ptr->expand();

        for (auto file : folder->files_)
        {
            auto file_node = std::make_unique<FileNode>(session_, file);
            auto file_ptr = file_node.get();
            folder_ptr->addChildNode(std::move(file_node));

            file_ptr->selected().connect(this, [=](bool selected)
                                           {
                                               if (selected)
                                               {
                                                   selected_file_path_ = file->fileName_;
                                                   selected_file_ = file;
                                                   file_selected_.emit();
                                                   reuploadFile();
                                               }
                                           });
        }

        folder_ptr->folders_changed_.connect(this, [=](std::string path)
                                              {
                                                  selected_file_path_ = path;
                                                  folders_changed_.emit(path);
                                              });
    }

    t.commit();

    root_ptr->folders_changed_.connect(this, [=](std::string path)
                                       {
                                           selected_file_path_ = path;
                                           folders_changed_.emit(path);
                                       });
}

}