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
#include <Wt/WTemplate.h>

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
        treeWrapper->setStyleClass("h-[100svh] border-r border-gray-600 overflow-y-auto overflow-x-hidden max-w-sm min-w-[240px] relative");

        // auto treeWrapperToggle = treeWrapper->addNew<Wt::WIconPair>(
        //     "static/icons/hamburger-open-white.svg",
        //     "static/icons/hamburger-close-white.svg"
        // );
        // treeWrapperToggle->setStyleClass("cursor-pointer h-6 w-6 border border-gray-600 rounded hover:bg-gray-700/50 p-1 flex items-center justify-center absolute top-2 -right-2");

        auto settingsArea = treeWrapper->addNew<Wt::WContainerWidget>();
        settingsArea->setStyleClass("flex items-center border-b border-gray-600 p-1 space-x-2");

        toggleStylusMainNav_ = settingsArea->addNew<Wt::WIconPair>(
            "static/icons/hamburger-open-white.svg",
            "static/icons/hamburger-close-white.svg"
        );
        toggleStylusMainNav_->setStyleClass("cursor-pointer h-6 w-6 border border-gray-600 rounded hover:bg-gray-700/50 p-1 flex items-center justify-center");
        if(stylusState_->stylusNode_->Attribute("main-menu-open") && 
           std::string(stylusState_->stylusNode_->Attribute("main-menu-open")).compare("false") == 0) {
            toggleStylusMainNav_->showIcon1();
        } else {
            toggleStylusMainNav_->showIcon2();
        }

        auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
        auto devRadio = settingsArea->addNew<Wt::WRadioButton>("Dev DBO");
        devRadio->setStyleClass("[&>input]:hidden [&>span]:px-1 [&>span]:border [&>span]:border-gray-600 [&>span]:rounded-md [&>input:checked]:[&~span]:bg-gray-700");
        auto prodRadio = settingsArea->addNew<Wt::WRadioButton>("Prod DBO");
        prodRadio->setStyleClass("[&>input]:hidden [&>span]:px-1 [&>span]:border [&>span]:border-gray-600 [&>span]:rounded-md [&>input:checked]:[&~span]:bg-gray-700");
        buttonGroup->addButton(devRadio);
        buttonGroup->addButton(prodRadio);

        tree_ = treeWrapper->addWidget(std::make_unique<Wt::WTree>());
        tree_->addStyleClass("relative -left-[18px] w-[calc(100%+18px)] mb-12");
        tree_->setSelectionMode(Wt::SelectionMode::Single);

        contentWrapper_ = addNew<Wt::WContainerWidget>();

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

    std::shared_ptr<StylusSession> TemplatesManager::getSelectedSession()
    {
        if (stylusState_ && stylusState_->xmlNode_->Attribute("dbo-to-use"))
        {
            std::string dboToUse = stylusState_->xmlNode_->Attribute("dbo-to-use");
            if (dboToUse.compare("prod") == 0)
            {
                return sessionProd_;
            }
            else
            {
                return sessionDev_;
            }
        }
        else
        {
            wApp->log("debug") << "Defaulting to Dev DBO";
            return sessionDev_;
        }
    }

    void TemplatesManager::populateTree()
    {
        wApp->log("debug") << "TemplatesManager::populateTree";
        std::shared_ptr<StylusSession> session = getSelectedSession();
        // session = sessionProd_;

        isRebuilding_ = true;  // Set flag to ignore selection signals during rebuild
    
        auto root_node = std::make_unique<RootNode>(session);
        auto root_ptr = root_node.get();
        root_ptr->setLoadPolicy(Wt::ContentLoading::Eager);
        root_ptr->changed().connect(this, &TemplatesManager::populateTree);
        tree_->setTreeRoot(std::move(root_node));
        root_ptr->selected().connect(this, [=](bool selected)
        {
            // Skip selection signal if we're rebuilding the tree
            if (isRebuilding_ && !selected)
            return;
            
            std::vector<Wt::Dbo::ptr<MessageTemplate>> templates;
            Wt::Dbo::Transaction t(*session);
            auto folders = session->find<TemplateFolder>().orderBy("order_index").resultList();
            for (auto folder : folders)
            {
                folder.modify()->selected_ = false;
                auto files = session->find<TemplateFile>()
                .where("folder_id = ?")
                .bind(folder.id())
                .resultList();
                for (auto file : files)
                {
                    file.modify()->selected_ = false;
                    auto fileTemplates = session->find<MessageTemplate>()
                                            .where("file_id = ?")
                                            .bind(file.id())
                                            .resultList();
                    templates.insert(templates.end(), fileTemplates.begin(), fileTemplates.end());  
                }
            }
            t.commit();
            renderTemplates(templates);
            });
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
                auto f = folder.modify();
                f->selected_ = selected;
                if (selected)
                {
                    std::vector<Wt::Dbo::ptr<MessageTemplate>> templates;
                    auto files = session->find<TemplateFile>()
                                        .where("folder_id = ?")
                                        .bind(folder.id())
                                        .resultList();
                    for (auto file : files)
                    {
                        auto fileTemplates = session->find<MessageTemplate>()
                                                .where("file_id = ?")
                                                .bind(file.id())
                                                .resultList();
                        templates.insert(templates.end(), fileTemplates.begin(), fileTemplates.end());  
                    }
                    renderTemplates(templates);
                }
                t.commit(); 
            });
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
                    file.modify()->selected_ = selected;
                    if (selected)
                    {
                        std::vector<Wt::Dbo::ptr<MessageTemplate>> templates;
                        auto fileTemplates = session->find<MessageTemplate>()
                        .where("file_id = ?")
                        .bind(file.id())
                        .resultList();
                        templates.insert(templates.end(), fileTemplates.begin(), fileTemplates.end());  
                        renderTemplates(templates);
                    } 
                    t.commit(); 
                });
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
                        
                        Wt::Dbo::Transaction t(*session);
                        tmpl.modify()->selected_ = selected;
                        t.commit(); 
                        if (selected)
                        {
                            renderSelection(tmpl);
                        }
                    });
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
    
    void TemplatesManager::renderTemplates(std::vector<Wt::Dbo::ptr<MessageTemplate>> templates)
    {
        if (!contentWrapper_)
            return;

        // Clear previous content
        contentWrapper_->clear();
        if(!contentWrapper_->hasStyleClass("!flex-row")){
            contentWrapper_->toggleStyleClass("!flex-row", true);
        }
        contentWrapper_->setStyleClass("h-[100svh] overflow-auto grow grid grid-cols-2 lg:grid-cols-3 gap-4 p-4 auto-rows-max");

        std::shared_ptr<StylusSession> session = getSelectedSession();

        for (auto tmpl : templates)
        {
            auto temp = contentWrapper_->addWidget(std::make_unique<Wt::WTemplate>(tmpl->templateXml_));
            temp->addStyleClass("hover:border hover:border-green-500 cursor-pointer ");

            temp->clicked().connect(this, [=]()
            {
                tree_->select(tree_->treeRoot()); // Deselect all
                Wt::Dbo::Transaction t(*session);
                tmpl.modify()->selected_ = true;
                t.commit();
                populateTree();
            });
        }
    }

    void TemplatesManager::renderSelection(Wt::Dbo::ptr<MessageTemplate> messageTemplate)
    {
        if (!contentWrapper_)
            return;

        // Clear previous content
        contentWrapper_->clear();
        if(contentWrapper_->hasStyleClass("!flex-row")){
            contentWrapper_->toggleStyleClass("!flex-row", false);
        }
        contentWrapper_->setStyleClass("h-[100svh] overflow-y-auto overflow-x-hidden grow flex flex-col flex-wrap items-stretch space-y-4 space-x-4");

        auto tempView = contentWrapper_->addWidget(std::make_unique<TempView>(sessionDev_, messageTemplate, stylusState_));
        // tempView->addStyleClass("h-full overflow-auto");
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
