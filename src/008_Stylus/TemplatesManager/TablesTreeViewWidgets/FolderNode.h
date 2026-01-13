#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WSignal.h>
#include <memory>

class TemplateFolder;
class StylusSession;

namespace Stylus
{

class FolderNode : public Wt::WTreeNode
{
public:
    FolderNode(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<TemplateFolder> folder);

    Wt::Signal<std::string> folders_changed_;
    Wt::WContainerWidget* label_wrapper_;
    Wt::Dbo::ptr<TemplateFolder> folder_;   

    Wt::Signal<> &changed() { return changed_; }
private:
    std::shared_ptr<StylusSession> session_;
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createRenameFolderDialog();
    void createRemoveFolderDialog();
    void createNewFileDialog();
    void showPopup(const Wt::WMouseEvent& event);
    void moveFolderUp();
    void moveFolderDown();
    Wt::Signal<> changed_;
};

}
