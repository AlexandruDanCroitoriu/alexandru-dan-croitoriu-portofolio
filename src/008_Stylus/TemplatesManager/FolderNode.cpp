#include "008_Stylus/TemplatesManager/FolderNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WIconPair.h>
#include <Wt/WPoint.h>

namespace Stylus
{

FolderNode::FolderNode(const std::string& name, Wt::Dbo::ptr<TemplateFolder> folder)
    : Wt::WTreeNode(name),
      folder_(folder)
{
    wApp->log("debug") << "FolderNode::FolderNode(" << name << ")";
    label_wrapper_ = labelArea();
    label_wrapper_->addStyleClass("flex items-center cursor-pointer mr-[3px]");
    setSelectable(false);

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            showPopup(event);
        }
    });

    setLabelIcon(std::make_unique<Wt::WIconPair>("./static/stylus/yellow-folder-closed.png",
                                                 "./static/stylus/yellow-folder-open.png"));
}

void FolderNode::createNewFolderDialog()
{
    wApp->log("debug") << "FolderNode::createNewFolderDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create folder"));
    dialog->setModal(true);
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("Folder creation is not implemented yet."));
    dialog->show();
}

void FolderNode::createRenameFolderDialog()
{
    wApp->log("debug") << "FolderNode::createRenameFolderDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename folder"));
    dialog->setModal(true);
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("Folder rename is not implemented yet."));
    dialog->show();
}

void FolderNode::createRemoveFolderMessageBox()
{
    wApp->log("debug") << "FolderNode::createRemoveFolderMessageBox";
    Wt::WMessageBox::show("Delete folder", "Folder deletion is not implemented yet.", Wt::StandardButton::Ok);
}

void FolderNode::createNewFileDialog()
{
    wApp->log("debug") << "FolderNode::createNewFileDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create file"));
    dialog->setModal(true);
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("File creation is not implemented yet."));
    dialog->show();
}

void FolderNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "FolderNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->addItem("New folder")->triggered().connect(this, &FolderNode::createNewFolderDialog);
        popup_->addItem("Rename folder")->triggered().connect(this, &FolderNode::createRenameFolderDialog);
        popup_->addItem("Delete folder")->triggered().connect(this, &FolderNode::createRemoveFolderMessageBox);
        popup_->addSeparator();
        popup_->addItem("New file")->triggered().connect(this, &FolderNode::createNewFileDialog);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

}
