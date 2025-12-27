#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WSignal.h>
#include <Wt/Dbo/ptr.h>

class Session;
class TemplateFile;
class MessageTemplate;

namespace Stylus
{

class TemplateFoldersTreeView : public Wt::WContainerWidget
{
public:
    TemplateFoldersTreeView(Session& session);

    void refresh();

    Wt::Signal<Wt::Dbo::ptr<TemplateFile>>& file_selected() { return file_selected_; }
    Wt::Signal<Wt::Dbo::ptr<MessageTemplate>>& template_selected() { return template_selected_; }

private:
    Session& session_;
    Wt::WTree* tree_;

    Wt::Signal<Wt::Dbo::ptr<TemplateFile>> file_selected_;
    Wt::Signal<Wt::Dbo::ptr<MessageTemplate>> template_selected_;

    void populateTree();
};

}
