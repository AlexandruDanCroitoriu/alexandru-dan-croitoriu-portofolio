#include "008_Stylus/TemplatesManager/RootNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>

#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFile.h"

namespace Stylus
{

RootNode::RootNode(StylusSession& session)
    : Wt::WTreeNode("DBO Root"),
    session_(session)
{
    label_wrapper_ = labelArea();
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");

    label_wrapper_->addStyleClass("flex items-center cursor-pointer ");
    setSelectable(true);
    expand();

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            showPopup(event);
        }
    });
    
    setLabelIcon(std::make_unique<Wt::WIconPair>("./static/icons/folder-closed.png", "./static/icons/folder-open.png"));


}

void RootNode::createNewFolderDialog()
{
    wApp->log("debug") << "RootNode::createNewFolderDialog()";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new folder"));
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("create new folder is not implemented yet."));
    dialog->show();
}


void RootNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "FileNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->setStyleClass("rounded-md");
        popup_->addItem("New Folder")->triggered().connect(this, &RootNode::createNewFolderDialog);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}


}
