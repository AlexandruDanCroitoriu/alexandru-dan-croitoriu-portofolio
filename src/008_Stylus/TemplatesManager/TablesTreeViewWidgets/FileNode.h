#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WSignal.h>

class TemplateFile;
class StylusSession;

namespace Stylus
{

class FileNode : public Wt::WTreeNode
{
public:
    FileNode(StylusSession& session, Wt::Dbo::ptr<TemplateFile> file);

    Wt::Signal<std::string> file_changed_;
    Wt::WContainerWidget* label_wrapper_;
    Wt::Dbo::ptr<TemplateFile> file_;

    Wt::Signal<> &changed() { return changed_; }
private:
    StylusSession& session_;
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createNewTemplateDialog();
    void createRenameFileDialog();
    void createChangeFolderDialog();
    void createDeleteFileDialog();
    void showPopup(const Wt::WMouseEvent& event);
    void moveFileUp();
    void moveFileDown();
    Wt::Signal<> changed_;
};

}
