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
#include <Wt/WTextArea.h>

namespace Stylus
{

    TempView::TempView(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<MessageTemplate> messageTemplate, std::shared_ptr<StylusState> stylusState)
        : session_(std::move(session)),
          messageTemplate_(messageTemplate),
            stylusState_(stylusState)
    {
        wApp->log("debug") << "TempView::TempView(StylusSession& session)";
        // WTemplate does not accept TextFormat in constructor; set it separately
        setStyleClass("border border-gray-600 rounded-md flex w-full h-full");
        leftArea_ = addNew<Wt::WContainerWidget>();
        centralArea_ = addNew<Wt::WContainerWidget>();
        centralMainArea_ = centralArea_->addNew<Wt::WContainerWidget>();
        monaco_ = centralArea_->addNew<MonacoEditor>("xml");
        // monaco_->setLineNumber(false);
        monaco_->setLineWrap(false);
        monaco_->setStyleClass("h-1/2 w-[calc(100%-2px)]");
        monaco_->availableSave().connect(this, [=]()
                                         {
                                             Wt::Dbo::Transaction transaction(*session_);
                                             messageTemplate_.modify()->templateXml_ = monaco_->getUnsavedText();
                                             transaction.commit();
                                            // xmlBrain_->saveCurrentXmlModifications();
                                            xmlBrain_->doc_->Clear();
                                            auto error = xmlBrain_->doc_->Parse(messageTemplate_->templateXml_.c_str());
                                            if (error != tinyxml2::XML_SUCCESS) {
                                                wApp->log("error") << "\nError parsing XML template after Monaco edit: " << xmlBrain_->doc_->ErrorIDToName(error) << "\n";
                                                // return;
                                            }
                                            xmlBrain_->selectedNode_ = nullptr;
                                            xmlBrain_->stateChanged().emit();
                                            });
        monaco_->setContent(messageTemplate_->templateXml_);
        rightArea_ = addNew<Wt::WContainerWidget>();

        centralArea_->setStyleClass("flex grow flex-wrap");
        leftArea_->setStyleClass("pr-2 border-r border-gray-600 min-w-[180px] overflow-x-hidden");
        centralMainArea_->setStyleClass("px-4 py-2 h-1/2");
        rightArea_->setStyleClass("p-2 border-l border-gray-600 min-w-[240px] max-w-[240px] w-[240px]");

        xmlBrain_ = std::make_shared<XmlBrain>(session_, messageTemplate_);
        xmlBrain_->stateChanged().connect(this, [=]()
                                          { refreshView(); });
        mouseWentUp().connect(this, [=](const Wt::WMouseEvent &event)
                              {
            if (event.button() == Wt::MouseButton::Right)
            {
                xmlBrain_->selectedNode_ = nullptr;
                xmlBrain_->stateChanged().emit();
            } });
        globalKeyConnection_ = wApp->globalKeyWentDown().connect([this](Wt::WKeyEvent e)
                                                                 {
                keyWentDown(e); 
            });

        refreshView();
    }

    TempView::~TempView()
    {
        globalKeyConnection_.disconnect();
        wApp->log("debug") << "TempView::~TempView() for temp with id " << xmlBrain_->messageTemplate_->id() << "\n";
    }

    void TempView::refreshView()
    {
        leftArea_->clear();
        centralMainArea_->clear();
        rightArea_->clear();

        // check if document is valid
        if (!xmlBrain_->doc_ || !xmlBrain_->doc_->RootElement())
        {
            auto error = xmlBrain_->doc_->Parse(xmlBrain_->messageTemplate_->templateXml_.c_str());
            auto errorText = centralMainArea_->addNew<Wt::WText>(error != tinyxml2::XML_SUCCESS ? "\nError parsing XML template: " + std::string(xmlBrain_->doc_->ErrorIDToName(error)) + "\n" : "\nError: XML document is empty or invalid.\n");
            errorText->setStyleClass("text-red-600 font-bold");
            return;
        }
        leftArea_->addNew<XMLTreeNode>(xmlBrain_, xmlBrain_->doc_->RootElement());
        auto elem_node = centralMainArea_->addNew<XMLElemNode>(xmlBrain_, xmlBrain_->doc_->RootElement());

        if (!xmlBrain_->selectedNode_)
            return;

        if (xmlBrain_->selectedNode_->ToElement())
        {
            if (!xmlBrain_->selectedNode_->FirstChild())
            {
                auto textArea = rightArea_->addNew<Wt::WTextArea>();
                textArea->addStyleClass("field-sizing-content");
                // if(!monaco_->hasFocus()) textArea->setFocus(true);
                textArea->setText("");
                textArea->keyWentDown().connect([=](Wt::WKeyEvent e)
                                        {
                    this->keyWentDown(e);
                    if(e.modifiers().test(Wt::KeyboardModifier::Control) && e.key() == Wt::Key::S)
                    {
                        tinyxml2::XMLText* newText = xmlBrain_->doc_->NewText(textArea->text().toUTF8().c_str());
                        xmlBrain_->selectedNode_->ToElement()->InsertEndChild(newText);
                        xmlBrain_->saveCurrentXmlModifications();
                        xmlBrain_->stateChanged().emit();
                        monaco_->setContentPreserveCursor(xmlBrain_->messageTemplate_->templateXml_);
                    }
                });

            }
            else if (xmlBrain_->selectedNode_->FirstChild() && xmlBrain_->selectedNode_->FirstChild() == xmlBrain_->selectedNode_->LastChild() && xmlBrain_->selectedNode_->FirstChild()->ToText())
            {
                auto textArea = rightArea_->addNew<Wt::WTextArea>();
                textArea->addStyleClass("field-sizing-content");
                // if(!monaco_->hasFocus()) textArea->setFocus(true);
                // stryp before and after whitespace/newlines
                std::string value = xmlBrain_->selectedNode_->FirstChild()->ToText()->Value();
                // size_t first = value.find_first_not_of(" \n\r\t");
                // size_t last = value.find_last_not_of(" \n\r\t");
                // if (first != std::string::npos && last != std::string::npos)
                    // value = value.substr(first, (last - first + 1));
                textArea->setText(value);
                textArea->keyWentDown().connect([=](Wt::WKeyEvent e)
                                        {
                    this->keyWentDown(e);
                    if(e.modifiers().test(Wt::KeyboardModifier::Control) && e.key() == Wt::Key::S)
                    {
                        xmlBrain_->selectedNode_->FirstChild()->ToText()->SetValue(textArea->text().toUTF8().c_str());
                        xmlBrain_->saveCurrentXmlModifications();
                        xmlBrain_->stateChanged().emit();
                        monaco_->setContentPreserveCursor(xmlBrain_->messageTemplate_->templateXml_);
                    }
                });
            }
        }
        else if (xmlBrain_->selectedNode_->ToText())
        {
            auto textArea = rightArea_->addNew<Wt::WTextArea>();
            textArea->addStyleClass("field-sizing-content");
            // if(!monaco_->hasFocus()) textArea->setFocus(true);
            // stryp before and after whitespace/newlines
            std::string value = xmlBrain_->selectedNode_->ToText()->Value();
            // size_t first = value.find_first_not_of(" \n\r\t");
            // size_t last = value.find_last_not_of(" \n\r\t");
            // if (first != std::string::npos && last != std::string::npos)
            // value = value.substr(first, (last - first + 1));
            textArea->setText(value);
            textArea->keyWentDown().connect([=](Wt::WKeyEvent e)
                                            {
                this->keyWentDown(e);
                if(e.modifiers().test(Wt::KeyboardModifier::Control) && e.key() == Wt::Key::S)
                {
                    xmlBrain_->selectedNode_->ToText()->SetValue(textArea->text().toUTF8().c_str());
                    xmlBrain_->saveCurrentXmlModifications();
                    xmlBrain_->stateChanged().emit();
                    monaco_->setContentPreserveCursor(xmlBrain_->messageTemplate_->templateXml_);
                }
            });
        }
    }

    void TempView::keyWentDown(Wt::WKeyEvent e)
    {
        wApp->log("debug") << "\nTempView::keyWentDown for temp with id " << xmlBrain_->messageTemplate_->id() << "\n";
        if (e.modifiers().test(Wt::KeyboardModifier::Alt))
        {

            if (e.key() == Wt::Key::Up)
            {
                if (xmlBrain_->selectedNode_)
                {
                    if (xmlBrain_->selectedNode_->PreviousSibling())
                    {
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->PreviousSibling();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
            if (e.key() == Wt::Key::Down)
            {
                if (xmlBrain_->selectedNode_)
                {
                    if (xmlBrain_->selectedNode_->NextSibling())
                    {
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->NextSibling();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
            else if (e.key() == Wt::Key::Left)
            {
                if (xmlBrain_->selectedNode_)
                {
                    if (xmlBrain_->selectedNode_->Parent())
                    {
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->Parent();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
            else if (e.key() == Wt::Key::Right)
            {
                if (xmlBrain_->selectedNode_)
                {
                    if (xmlBrain_->selectedNode_->FirstChild())
                    {
                        xmlBrain_->selectedNode_ = xmlBrain_->selectedNode_->FirstChild();
                        xmlBrain_->stateChanged().emit();
                    }
                }else if(xmlBrain_->doc_->RootElement()){
                    xmlBrain_->selectedNode_ = xmlBrain_->doc_->RootElement();
                    xmlBrain_->stateChanged().emit();
                }
            }
        }
        else if (e.modifiers().test(Wt::KeyboardModifier::Shift))
        {
            if (e.key() == Wt::Key::Up)
            {
                if (xmlBrain_->selectedNode_)
                {
                    if (xmlBrain_->selectedNode_->PreviousSibling())
                    {
                        auto parent = xmlBrain_->selectedNode_->Parent();
                        parent->InsertAfterChild(xmlBrain_->selectedNode_, xmlBrain_->selectedNode_->PreviousSibling());
                        xmlBrain_->saveCurrentXmlModifications();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
            else if (e.key() == Wt::Key::Down)
            {
                if (xmlBrain_->selectedNode_)
                {
                    if (xmlBrain_->selectedNode_->NextSibling())
                    {
                        auto parent = xmlBrain_->selectedNode_->Parent();
                        parent->InsertAfterChild(xmlBrain_->selectedNode_->NextSibling(), xmlBrain_->selectedNode_);
                        xmlBrain_->saveCurrentXmlModifications();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
            else if (e.key() == Wt::Key::Left)
            {
                // Decrease indent (move node up a level)
                if (xmlBrain_->selectedNode_)
                {
                    auto parent = xmlBrain_->selectedNode_->Parent();
                    if (parent && parent == xmlBrain_->doc_->RootElement())
                        return; // Can't move out of root
                    if (parent && parent->ToElement() && parent->Parent())
                    {
                        if (parent->Parent()->FirstChild() == parent)
                        {
                            parent->Parent()->InsertFirstChild(xmlBrain_->selectedNode_);
                        }
                        else
                        {
                            parent->Parent()->InsertAfterChild(parent->PreviousSibling(), xmlBrain_->selectedNode_);
                        }
                        xmlBrain_->saveCurrentXmlModifications();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
            else if (e.key() == Wt::Key::Right)
            {
                // Increase indent (move node down a level)
                if (xmlBrain_->selectedNode_)
                {
                    auto nextSibling = xmlBrain_->selectedNode_->NextSiblingElement();
                    if (nextSibling)
                    {
                        nextSibling->InsertFirstChild(xmlBrain_->selectedNode_);
                        xmlBrain_->saveCurrentXmlModifications();
                        xmlBrain_->stateChanged().emit();
                    }
                }
            }
        }
    }

} // namespace Stylus