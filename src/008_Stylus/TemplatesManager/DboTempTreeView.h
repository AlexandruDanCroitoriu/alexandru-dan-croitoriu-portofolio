#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>

class StylusSession;
class TemplateFile;
class MessageTemplate;

namespace Stylus
{

class DboTempTreeView : public Wt::WContainerWidget
{
public:
    DboTempTreeView(StylusSession& session);

    void refresh();

    Wt::Signal<Wt::Dbo::ptr<TemplateFile>>& file_selected() { return file_selected_; }
    Wt::Signal<Wt::Dbo::ptr<MessageTemplate>>& template_selected() { return template_selected_; }

private:
    StylusSession& session_;
    Wt::WTree* tree_;

    Wt::Signal<Wt::Dbo::ptr<TemplateFile>> file_selected_;
    Wt::Signal<Wt::Dbo::ptr<MessageTemplate>> template_selected_;

    void populateTree();
};

}
