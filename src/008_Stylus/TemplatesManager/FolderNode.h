#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>

class TemplateFolder;
class StylusSession;

namespace Stylus
{

class FolderNode : public Wt::WTreeNode
{
public:
    FolderNode(StylusSession& session, Wt::Dbo::ptr<TemplateFolder> folder = Wt::Dbo::ptr<TemplateFolder>());

    Wt::Signal<std::string> folders_changed_;
    Wt::WContainerWidget* label_wrapper_;
    Wt::Dbo::ptr<TemplateFolder> folder_;

private:
    StylusSession& session_;
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createNewFolderDialog();
    void createRenameFolderDialog();
    void createRemoveFolderMessageBox();
    void createNewFileDialog();
    void showPopup(const Wt::WMouseEvent& event);
};

}
