#pragma once

#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WSignal.h>

class StylusSession;

namespace Stylus
{

class RootNode : public Wt::WTreeNode
{
public:
    RootNode(StylusSession& session);

    Wt::Signal<std::string> file_changed_;
    Wt::WContainerWidget* label_wrapper_;
    Wt::Signal<> &changed(){ return changed_; }
private:
    StylusSession& session_;
    std::unique_ptr<Wt::WPopupMenu> popup_;

    void createNewFolderDialog();
    void showPopup(const Wt::WMouseEvent& event);
    Wt::Signal<> changed_;
};

}
