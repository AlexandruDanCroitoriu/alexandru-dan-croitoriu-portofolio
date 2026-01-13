#include "008_Stylus/TemplatesManager/PreviewWidgets/TempView.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/XMLTreeNode.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/XMLElemNode.h"
#include "008_Stylus/TemplatesManager/PreviewWidgets/XmlBrain.h"

#include "002_Components/MonacoEditor.h"

#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/MessageTemplate.h"

#include <Wt/WApplication.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WIconPair.h>
#include <Wt/WPushButton.h>

namespace Stylus
{

    TempView::TempView(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<MessageTemplate> messageTemplate)
        : session_(std::move(session)),
            messageTemplate_(messageTemplate)
    {
        wApp->log("debug") << "TempView::TempView(StylusSession& session)";
        // WTemplate does not accept TextFormat in constructor; set it separately
        setStyleClass("border border-gray-600 rounded-md flex min-w-fit");
        auto settingsArea = addNew<Wt::WContainerWidget>();
        settingsArea->setStyleClass("flex flex-col p-1 border-r border-gray-600");
        leftArea_ = addNew<Wt::WContainerWidget>();
        leftArea_->setStyleClass("pr-2 border-r border-gray-600 min-w-[240px]");
        mainArea_ = addNew<Wt::WContainerWidget>();
        mainArea_->setStyleClass("px-4 py-2");
        // rightArea_ = addNew<Wt::WContainerWidget>();
        // rightArea_->setStyleClass("pl-2 border-l border-gray-600");

        xmlBrain_ = std::make_shared<XmlBrain>(session_, messageTemplate_);
        xmlBrain_->stateChanged().connect(this, [=]() {
            refreshView();
        });

        mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
        {
            if (event.button() == Wt::MouseButton::Right)
            {
                xmlBrain_->selectedNode_ = nullptr;
                xmlBrain_->stateChanged().emit();
            }
        });
        globalKeyConnection_ = wApp->globalKeyWentDown().connect([this](Wt::WKeyEvent e) {
            if(messageTemplate_->viewMode_ == ViewMode::Editor || xmlBrain_->selectedNode_)
                keyWentDown(e);
        });
        
        auto switchViewMoode = settingsArea->addNew<Wt::WIconPair>(
            "static/icons/view-template.svg",
            "static/icons/view-editor.svg"
        );
        switchViewMoode->setStyleClass("cursor-pointer mb-2 w-6 border border-gray-600 rounded hover:bg-gray-700/50 p-1");
        
        // Connect both icon states to toggle view mode
        switchViewMoode->icon1Clicked().connect(this, [=]() {
            setViewMode(ViewMode::Editor);
            switchViewMoode->setState(1);  // Show icon2
        });
        switchViewMoode->icon2Clicked().connect(this, [=]() {
            setViewMode(ViewMode::Template);
            switchViewMoode->setState(0);  // Show icon1
        });
        
        // Set initial icon state based on current view mode
        if(messageTemplate_->viewMode_ == ViewMode::Template) {
            switchViewMoode->setState(0);  // Show icon1 (view-template.svg)
        } else {
            switchViewMoode->setState(1);  // Show icon2 (view-editor.svg)
        }
        
        refreshView();
    }

    TempView::~TempView()
    {
        globalKeyConnection_.disconnect();
        wApp->log("debug") << "TempView::~TempView() for temp with id " << xmlBrain_->messageTemplate_->id() << "\n";
    }

    void TempView::setViewMode(ViewMode mode)
    {
        Wt::Dbo::Transaction t(*session_);
        messageTemplate_.modify()->viewMode_ = mode;
        t.commit();
        refreshView();

    }

    void TempView::refreshView()
    {
        leftArea_->clear();
        mainArea_->clear();
        // rightArea_->clear();
        auto mainWrapper = mainArea_->addNew<Wt::WContainerWidget>();

        
        
        if(messageTemplate_->viewMode_ == ViewMode::Template)
        {
            auto monaco = leftArea_->addNew<MonacoEditor>("xml");
            monaco->setStyleClass("h-full w-full");
            monaco->setContent(messageTemplate_->templateXml_);
            monaco->setLineNumber(false);
            monaco->setLineWrap(false);
            auto temp = mainWrapper->addNew<Wt::WTemplate>(Wt::WString::fromUTF8(messageTemplate_->templateXml_));
        }else {
            leftArea_->addNew<XMLTreeNode>(xmlBrain_, xmlBrain_->doc_->RootElement());
            auto elem_node = mainWrapper->addNew<XMLElemNode>(xmlBrain_, xmlBrain_->doc_->RootElement());
        }
    }


    void TempView::keyWentDown(Wt::WKeyEvent e)
    {
        wApp->log("debug") << "\nTempView::keyWentDown for temp with id " << xmlBrain_->messageTemplate_->id() << "\n";
        if (e.modifiers().test(Wt::KeyboardModifier::Alt)){

            if(e.key() == Wt::Key::Up){
                if(xmlBrain_->selectedNode_){
                    if(xmlBrain_->selectedNode_->PreviousSibling()){
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->PreviousSibling();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }if(e.key() == Wt::Key::Down){
                if(xmlBrain_->selectedNode_){
                    if(xmlBrain_->selectedNode_->NextSibling()){
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->NextSibling();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }else if(e.key() == Wt::Key::Left){
                if(xmlBrain_->selectedNode_){
                    if(xmlBrain_->selectedNode_->Parent()){
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->Parent();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }else if(e.key() == Wt::Key::Right){
                if(xmlBrain_->selectedNode_){
                    if(xmlBrain_->selectedNode_->FirstChild()){
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->FirstChild();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
        }else if (e.modifiers().test(Wt::KeyboardModifier::Shift)) {
            if (e.key() == Wt::Key::Up) {
                if(xmlBrain_->selectedNode_) {
                    if(xmlBrain_->selectedNode_->PreviousSibling()) {
                        auto parent = xmlBrain_->selectedNode_->Parent();
                        parent->InsertAfterChild(xmlBrain_->selectedNode_, xmlBrain_->selectedNode_->PreviousSibling());
                        xmlBrain_->saveCurrentVersion();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            } else if (e.key() == Wt::Key::Down) {
                if(xmlBrain_->selectedNode_) {
                    if(xmlBrain_->selectedNode_->NextSibling()) {
                        auto parent = xmlBrain_->selectedNode_->Parent();
                        parent->InsertAfterChild(xmlBrain_->selectedNode_->NextSibling(), xmlBrain_->selectedNode_);
                        xmlBrain_->saveCurrentVersion();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            } else if (e.key() == Wt::Key::Left) {
                // Decrease indent (move node up a level)
                if(xmlBrain_->selectedNode_) {
                    auto parent = xmlBrain_->selectedNode_->Parent();
                    if(parent && parent == xmlBrain_->doc_->RootElement()) return; // Can't move out of root
                    if(parent && parent->ToElement() && parent->Parent()) {
                        if(parent->Parent()->FirstChild() == parent){
                            parent->Parent()->InsertFirstChild(xmlBrain_->selectedNode_);
                        }else {
                            parent->Parent()->InsertAfterChild(parent->PreviousSibling(), xmlBrain_->selectedNode_);
                        }
                        xmlBrain_->saveCurrentVersion();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            } else if (e.key() == Wt::Key::Right) {
                // Increase indent (move node down a level)
                if(xmlBrain_->selectedNode_) {
                    auto nextSibling = xmlBrain_->selectedNode_->NextSiblingElement();
                    if(nextSibling) {
                        nextSibling->InsertFirstChild(xmlBrain_->selectedNode_);
                        xmlBrain_->saveCurrentVersion();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
        }
        
    }

} // namespace Stylus