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

#include "008_Stylus/StylusState.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>
#include <Wt/WPoint.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WText.h>
#include <Wt/WDialog.h>
#include <Wt/WRadioButton.h>
#include <Wt/WButtonGroup.h>

namespace Stylus
{

    TemplatesManager::TemplatesManager(std::shared_ptr<StylusSession> sessionDev, std::shared_ptr<StylusSession> sessionProd, std::shared_ptr<StylusState> stylusState)
        : sessionDev_(sessionDev),
          sessionProd_(sessionProd),
          stylusState_(stylusState)
    {
        wApp->log("debug") << "TemplatesManager::TemplatesManager";
        setStyleClass("max-h-[100svh] flex overflow-hidden");

        auto treeWrapper = addNew<Wt::WContainerWidget>();
        treeWrapper->setStyleClass("h-[100svh] border-r border-gray-600 overflow-y-auto overflow-x-hidden max-w-sm min-w-[240px]");

        auto settingsArea = treeWrapper->addNew<Wt::WContainerWidget>();
        settingsArea->setStyleClass("flex items-center border-b border-gray-600 p-1");

        auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
        auto devRadio = settingsArea->addNew<Wt::WRadioButton>("Dev DBO");
        auto prodRadio = settingsArea->addNew<Wt::WRadioButton>("Prod DBO");
        buttonGroup->addButton(devRadio);
        buttonGroup->addButton(prodRadio);

        tree_ = treeWrapper->addWidget(std::make_unique<Wt::WTree>());
        tree_->addStyleClass("relative -left-[18px] w-[calc(100%+18px)] mb-12");
        tree_->setSelectionMode(Wt::SelectionMode::Single);

        contentWrapper_ = addNew<Wt::WContainerWidget>();
        contentWrapper_->setStyleClass("h-[100svh] overflow-auto grow flex flex-col items-stretch space-y-4");

        buttonGroup->checkedChanged().connect(this, [=](Wt::WRadioButton *button)
                                              {
            if (button == devRadio) {
                wApp->log("debug") << "Switched to Dev DBO";
                stylusState_->xmlNode_->SetAttribute("dbo-to-use", "dev");
            } else if (button == prodRadio) {
                wApp->log("debug") << "Switched to Prod DBO";
                stylusState_->xmlNode_->SetAttribute("dbo-to-use", "prod");
            }
            stylusState_->doc_->SaveFile(stylusState_->stateFilePath_.c_str());
            populateTree(); });

        if (stylusState_ && stylusState_->xmlNode_->Attribute("dbo-to-use"))
        {
            std::string dboToUse = stylusState_->xmlNode_->Attribute("dbo-to-use");
            if (dboToUse == "prod")
            {
                prodRadio->setChecked(true);
            }
            else
            {
                devRadio->setChecked(true);
            }
        }
        else
        {
            devRadio->setChecked(true);
        }
        buttonGroup->checkedChanged().emit(buttonGroup->checkedButton());
    }

    void TemplatesManager::populateTree()
    {
        wApp->log("debug") << "TemplatesManager::populateTree";
        std::shared_ptr<StylusSession> session;

        if (stylusState_ && stylusState_->xmlNode_->Attribute("dbo-to-use"))
        {
            std::string dboToUse = stylusState_->xmlNode_->Attribute("dbo-to-use");
            if (dboToUse.compare("prod") == 0)
            {
                session = sessionProd_;
            }
            else
            {
                session = sessionDev_;
            }
        }
        else
        {
            wApp->log("debug") << "Defaulting to Dev DBO";
            session = sessionDev_;
        }
        // session = sessionProd_;

        isRebuilding_ = true;  // Set flag to ignore selection signals during rebuild
        
        auto root_node = std::make_unique<RootNode>(session);
        auto root_ptr = root_node.get();
        root_ptr->setLoadPolicy(Wt::ContentLoading::Eager);
        root_ptr->changed().connect(this, &TemplatesManager::populateTree);
        tree_->setTreeRoot(std::move(root_node));

        Wt::Dbo::Transaction t(*session);
        auto folders = session->find<TemplateFolder>().orderBy("order_index").resultList();

        bool selectedRestored = false;

        for (auto folder : folders)
        {
            auto folder_node = std::make_unique<FolderNode>(session, folder);
            auto folder_ptr = folder_node.get();
            folder_node->changed().connect(this, &TemplatesManager::populateTree);
            root_ptr->addChildNode(std::move(folder_node));

            // Track selection changes
            folder_ptr->selected().connect(this, [=](bool selected)
                                           {
            
                // Skip selection signal if we're rebuilding the tree
                if (isRebuilding_ && !selected)
                    return;
                
                Wt::Dbo::Transaction t(*session);
                if (selected)
                {
                    std::vector<Wt::Dbo::ptr<MessageTemplate>> messageTemplates;
                    for (auto file : folder->files_)
                    {
                        for (auto tmpl : file->templates_)
                        {
                            messageTemplates.push_back(tmpl);
                        }
                    }
                    renderSelection(messageTemplates);
                }
                auto f = folder.modify();
                f->selected_ = selected;
                t.commit(); });
            if (folder->selected_ && !selectedRestored)
            {
                tree_->select(folder_ptr);
                selectedRestored = true;
            }
            // When a folder is re-expanded, re-open any child files that were previously expanded.
            folder_ptr->expanded().connect([folder_ptr]()
                                           {
                for (auto child : folder_ptr->childNodes())
                {
                    if (auto file_child = dynamic_cast<FileNode*>(child))
                    {
                        if (file_child->file_ && file_child->file_->expanded_)
                        {
                            file_child->expand();
                        }
                    }
                } });

            auto orderedFiles = session->find<TemplateFile>()
                                    .where("folder_id = ?")
                                    .bind(folder.id())
                                    .orderBy("order_index")
                                    .resultList();

            for (auto file : orderedFiles)
            {
                auto file_node = std::make_unique<FileNode>(session, file);
                auto file_ptr = file_node.get();
                file_ptr->changed().connect(this, &TemplatesManager::populateTree);
                folder_ptr->addChildNode(std::move(file_node));

                file_ptr->selected().connect(this, [=](bool selected)
                                             {
                    // Skip selection signal if we're rebuilding the tree
                    if (isRebuilding_ && !selected)
                        return;
                    
                    Wt::Dbo::Transaction t(*session);
                    if (selected)
                    {
                        std::vector<Wt::Dbo::ptr<MessageTemplate>> messageTemplates;
                        auto templates = session->find<MessageTemplate>()
                                            .where("file_id = ?")
                                            .bind(file.id())
                                            .orderBy("order_index")
                                            .resultList();
                        for (auto tmpl : templates)
                        {
                            messageTemplates.push_back(tmpl);
                        }
                        renderSelection(messageTemplates);
                    } 
                    file.modify()->selected_ = selected;
                    t.commit(); });
                if (file->selected_ && !selectedRestored)
                {
                    tree_->select(file_ptr);
                    selectedRestored = true;
                }

                // Add templates under the file (ordered)
                auto orderedTemplates = session->find<MessageTemplate>()
                                            .where("file_id = ?")
                                            .bind(file.id())
                                            .orderBy("order_index")
                                            .resultList();

                for (auto tmpl : orderedTemplates)
                {
                    auto template_node = std::make_unique<TemplateNode>(session, tmpl);
                    auto template_ptr = template_node.get();
                    file_ptr->addChildNode(std::move(template_node));
                    template_ptr->changed().connect(this, &TemplatesManager::populateTree);
                    template_ptr->selected().connect(this, [=](bool selected)
                                                     {
                        // Skip selection signal if we're rebuilding the tree
                        if (isRebuilding_ && !selected)
                            return;
                        
                        if (selected)
                        {
                            std::vector<Wt::Dbo::ptr<MessageTemplate>> messageTemplates;
                            messageTemplates.push_back(tmpl);
                            renderSelection(messageTemplates);
                        }
                        Wt::Dbo::Transaction t(*session);
                        tmpl.modify()->selected_ = selected;
                        t.commit(); });
                    if (tmpl->selected_ && !selectedRestored)
                    {
                        tree_->select(template_ptr);
                        selectedRestored = true;
                    }
                }
            }
        }

        t.commit();
        isRebuilding_ = false;  // Clear flag after tree is rebuilt
        std::cout << "\n\n selectedRestored: " << selectedRestored << "\n\n";
        // tree_->select(tree_->selectedNodes(), true); // Refresh selection to trigger rendering
        // renderSelection();
    }

    void TemplatesManager::renderSelection(std::vector<Wt::Dbo::ptr<MessageTemplate>> messageTemplate)
    {
        if (!contentWrapper_)
            return;

        // Clear previous content
        contentWrapper_->clear();


        for (auto tmpl : messageTemplate)
        {
            auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(sessionDev_, tmpl));
            // tempView->setViewMode(ViewMode::Editor);
            // tempViews_.push_back(tempView);
        }

        // std::shared_ptr<StylusSession> session;

        // if (stylusState_ && stylusState_->xmlNode_->Attribute("dbo-to-use"))
        // {
        //     std::string dboToUse = stylusState_->xmlNode_->Attribute("dbo-to-use");
        //     if (dboToUse.compare("prod") == 0)
        //     {
        //         session = sessionProd_;
        //     }
        //     else
        //     {
        //         session = sessionDev_;
        //     }
        // }
        // else
        // {
        //     wApp->log("debug") << "Defaulting to Dev DBO";
        //     session = sessionDev_;
        // }

        // Wt::Dbo::Transaction t(*session);

        // switch (selectedKind_)
        // {
        // case SelectedKind::Template:
        // {
        //     if (selectedTemplateId_ < 0)
        //         break;
        //     auto tmplList = session->find<MessageTemplate>()
        //                         .where("id = ?")
        //                         .bind(selectedTemplateId_)
        //                         .limit(1)
        //                         .resultList();
        //     for (auto tmpl : tmplList)
        //     {
        //         auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(session, tmpl));
        //         tempView->addStyleClass("h-full");
        //         // tempView->setViewMode(ViewMode::Editor);
        //         // tempViews_.push_back(tempView);
        //     }
        //     break;
        // }
        // case SelectedKind::File:
        // {
        //     if (selectedFileId_ < 0)
        //         break;
        //     auto orderedTemplates = session->find<MessageTemplate>()
        //                                 .where("file_id = ?")
        //                                 .bind(selectedFileId_)
        //                                 .orderBy("order_index")
        //                                 .resultList();
        //     for (auto tmpl : orderedTemplates)
        //     {
        //         auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(session, tmpl));
        //         // tempView->setViewMode(ViewMode::Editor);
        //         // tempViews_.push_back(tempView);
        //     }
        //     break;
        // }
        // case SelectedKind::Folder:
        // {
        //     if (selectedFolderId_ < 0)
        //         break;
        //     auto orderedFiles = session->find<TemplateFile>()
        //                             .where("folder_id = ?")
        //                             .bind(selectedFolderId_)
        //                             .orderBy("order_index")
        //                             .resultList();
        //     for (auto file : orderedFiles)
        //     {
        //         auto orderedTemplates = session->find<MessageTemplate>()
        //                                     .where("file_id = ?")
        //                                     .bind(file.id())
        //                                     .orderBy("order_index")
        //                                     .resultList();
        //         for (auto tmpl : orderedTemplates)
        //         {
        //             auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(session, tmpl));
        //             // tempView->setViewMode(ViewMode::Editor);
        //             // tempViews_.push_back(tempView);
        //         }
        //     }
        //     break;
        // }
        // case SelectedKind::None:
        // default:
        //     break;
        // }

        // t.commit();
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
