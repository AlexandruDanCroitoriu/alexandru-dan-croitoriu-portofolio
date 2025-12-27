#include "008_Stylus/TemplatesManager/FileNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>

namespace Stylus
{

FileNode::FileNode(const std::string& name, Wt::Dbo::ptr<TemplateFile> file)
    : Wt::WTreeNode(name),
      file_(file)
{
    wApp->log("debug") << "FileNode::FileNode(" << name << ")";
    label_wrapper_ = labelArea();
    label_wrapper_->addStyleClass("flex items-center cursor-pointer mr-[3px]");
    setSelectable(true);

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            showPopup(event);
        }
    });
}

void FileNode::createNewTemplateDialog()
{
    wApp->log("debug") << "FileNode::createNewTemplateDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create template"));
    dialog->setModal(true);
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("Template creation is not implemented yet."));
    dialog->show();
}

void FileNode::createRenameFileDialog()
{
    wApp->log("debug") << "FileNode::createRenameFileDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename file"));
    dialog->setModal(true);
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("File rename is not implemented yet."));
    dialog->show();
}

void FileNode::deleteFileMessageBox()
{
    wApp->log("debug") << "FileNode::deleteFileMessageBox";
    Wt::WMessageBox::show("Delete file", "File deletion is not implemented yet.", Wt::StandardButton::Ok);
}

void FileNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "FileNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->addItem("New template")->triggered().connect(this, &FileNode::createNewTemplateDialog);
        popup_->addSeparator();
        popup_->addItem("Rename file")->triggered().connect(this, &FileNode::createRenameFileDialog);
        popup_->addItem("Delete file")->triggered().connect(this, &FileNode::deleteFileMessageBox);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

}
