#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>

class TemplateFile;

namespace Stylus
{

class FileNode : public Wt::WTreeNode
{
public:
    FileNode(const std::string& name, Wt::Dbo::ptr<TemplateFile> file = Wt::Dbo::ptr<TemplateFile>());

    Wt::Signal<std::string> file_changed_;
    Wt::WContainerWidget* label_wrapper_;
    Wt::Dbo::ptr<TemplateFile> file_;

private:
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createNewTemplateDialog();
    void createRenameFileDialog();
    void deleteFileMessageBox();
    void showPopup(const Wt::WMouseEvent& event);
};

}
