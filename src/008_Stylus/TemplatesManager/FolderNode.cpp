#include "008_Stylus/TemplatesManager/FolderNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WIconPair.h>
#include <Wt/WPoint.h>

#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFolder.h"

namespace Stylus
{

FolderNode::FolderNode(StylusSession& session, Wt::Dbo::ptr<TemplateFolder> folder)
    : Wt::WTreeNode(folder_ ? "error" : folder->folderName_),
      folder_(folder),
    session_(session)
{
    wApp->log("debug") << "FolderNode::FolderNode(" << folder->folderName_ << ")";
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");
    label_wrapper_ = labelArea();
    label_wrapper_->addStyleClass("flex items-center cursor-pointer ");
    setSelectable(true);

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            showPopup(event);
        }
    });

    label_wrapper_->doubleClicked().connect(this, [=]() 
    {
        if(isExpanded()){
            collapsed().emit(Wt::WMouseEvent());
        }else{
            expanded().emit(Wt::WMouseEvent());
        }
    });

    selected().connect(this, [=](bool selected)
    {
        if (selected)
        {
            wApp->log("debug") << "FolderNode selected: " << folder->folderName_;
        }
    });

    expanded().connect(this, [=]()
    {
        if (folder_)
        {
            Wt::Dbo::Transaction t(session_);
            folder_.modify()->expanded_ = true;
            t.commit();
        }
    });

    collapsed().connect(this, [=]()
    {
        if (folder_)
        {
            Wt::Dbo::Transaction t(session_);
            folder_.modify()->expanded_ = false;
            t.commit();
        }
    });

    if(folder_ && folder_->expanded_)
        expand();

    setLabelIcon(std::make_unique<Wt::WIconPair>("./static/icons/folder-closed.png",
                                                 "./static/icons/folder-open.png"));
}

void FolderNode::createNewFolderDialog()
{
    wApp->log("debug") << "FolderNode::createNewFolderDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create folder"));
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("Folder creation is not implemented yet."));
    dialog->show();
}

void FolderNode::createRenameFolderDialog()
{
    wApp->log("debug") << "FolderNode::createRenameFolderDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename folder"));
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
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
    dialog->rejectWhenEscapePressed();
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("File creation is not implemented yet."));
    dialog->show();
}

void FolderNode::showPopup(const Wt::WMouseEvent& event)
{
    wApp->log("debug") << "FolderNode::showPopup";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        popup_->setStyleClass("rounded-md");
        popup_->addItem("New folder")->triggered().connect(this, &FolderNode::createNewFolderDialog);
        popup_->addItem("Rename folder")->triggered().connect(this, &FolderNode::createRenameFolderDialog);
        popup_->addItem("Delete folder")->triggered().connect(this, &FolderNode::createRemoveFolderMessageBox);
        popup_->addSeparator();
        popup_->addItem("New file")->triggered().connect(this, &FolderNode::createNewFileDialog);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

}
