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

namespace Stylus
{

    TempView::TempView(StylusSession& session, Wt::Dbo::ptr<MessageTemplate> file)
        : session_(session),
            file_(file)
    {
        wApp->log("debug") << "TempView::TempView(StylusSession& session)";
        // WTemplate does not accept TextFormat in constructor; set it separately
        setStyleClass("border border-gray-600 rounded-md p-4 flex min-w-fit");
        leftArea_ = addNew<Wt::WContainerWidget>();
        leftArea_->setStyleClass("pr-2 border-r border-gray-600");
        mainArea_ = addNew<Wt::WContainerWidget>();
        mainArea_->setStyleClass("grow px-4 py-2");
        rightArea_ = addNew<Wt::WContainerWidget>();
        rightArea_->setStyleClass("pl-2 border-l border-gray-600");


        xmlBrain_ = std::make_shared<XmlBrain>(session_, file_);
        xmlBrain_->stateChanged().connect(this, [=]() {
            setViewMode();
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
            keyWentDown(e);
        });
    }

    TempView::~TempView()
    {
        globalKeyConnection_.disconnect();
        wApp->log("debug") << "TempView::~TempView() for temp with id " << xmlBrain_->messageTemplate_->id() << "\n";
    }

    void TempView::setViewMode(TemplateViewMode mode)
    {

        leftArea_->clear();
        mainArea_->clear();
        rightArea_->clear();

        
        
        if(mode == TemplateViewMode::Template)
        {
            auto monaco = leftArea_->addNew<MonacoEditor>("xml");
            auto temp = mainArea_->addNew<Wt::WTemplate>(Wt::WString::fromUTF8(file_->templateXml_));
        }else {
            leftArea_->addNew<XMLTreeNode>(xmlBrain_, xmlBrain_->doc_->RootElement());
            auto elem_node = mainArea_->addNew<XMLElemNode>(xmlBrain_, xmlBrain_->doc_->RootElement());
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