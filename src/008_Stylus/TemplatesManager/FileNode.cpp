#include "008_Stylus/TemplatesManager/FileNode.h"

#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <Wt/WText.h>
#include <Wt/WPoint.h>

#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFile.h"

namespace Stylus
{

FileNode::FileNode(StylusSession& session, Wt::Dbo::ptr<TemplateFile> file)
    : Wt::WTreeNode(file ? "New File" : file->fileName_),
      file_(file),
    session_(session)
{
    wApp->log("debug") << "FileNode::FileNode(" << file_->fileName_ << ")";
    label_wrapper_ = labelArea();
    addStyleClass("[&>.Wt-selected]:!bg-gray-200 [&>.Wt-selected]:text-black [&>.Wt-selected]:rounded-md");

    label_wrapper_->addStyleClass("flex items-center cursor-pointer ");
    setSelectable(true);

    label_wrapper_->doubleClicked().connect(this, [=]() 
    {
        if(isExpanded())
            collapsed().emit(Wt::WMouseEvent());
        else
            expanded().emit(Wt::WMouseEvent());
    });

    label_wrapper_->mouseWentUp().connect(this, [=](const Wt::WMouseEvent& event)
    {
        if (event.button() == Wt::MouseButton::Right)
        {
            showPopup(event);
        }
    });
    
    setLabelIcon(std::make_unique<Wt::WIconPair>("./static/icons/file-closed.svg", "./static/icons/file-open.svg"));

    if (file_ && file_->expanded_)
        expand();

    selected().connect(this, [=](bool selected)
    {
        if (selected)
        {
            wApp->log("debug") << "FolderNode selected: " << file_->fileName_;
        }
    });

    expanded().connect(this, [=]()
    {
        if (file_)
        {
            Wt::Dbo::Transaction t(session_);
            file_.modify()->expanded_ = true;
            t.commit();
        }
    });

    collapsed().connect(this, [=]()
    {
        if (file_)
        {
            Wt::Dbo::Transaction t(session_);
            file_.modify()->expanded_ = false;
            t.commit();
        }
        std::cout << "\n\nFileNode::collapsed()\n\n";
    });
}

void FileNode::createNewTemplateDialog()
{
    wApp->log("debug") << "FileNode::createNewTemplateDialog";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create template"));
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    dialog->contents()->addWidget(std::make_unique<Wt::WText>("Template creation is not implemented yet."));
    dialog->show();
}

void FileNode::createRenameFileDialog()
{
    wApp->log("debug") << "FileNode::createRenameFileDialog()";
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Rename file"));
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
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
    wApp->log("debug") << "FileNode::showPopup(const Wt::WMouseEvent& event)";
    if (!popup_)
    {
        popup_ = std::make_unique<Wt::WPopupMenu>();
        // popup_->setStyleClass("rounded-md");
        popup_->addItem("New template")->triggered().connect(this, &FileNode::createNewTemplateDialog);
        popup_->addSeparator();
        popup_->addItem("Rename file")->triggered().connect(this, &FileNode::createRenameFileDialog);
        popup_->addItem("Delete file")->triggered().connect(this, &FileNode::deleteFileMessageBox);
    }

    popup_->popup(Wt::WPoint(event.document().x, event.document().y));
}

}
