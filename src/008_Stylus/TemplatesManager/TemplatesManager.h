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

    class TempView;

class TemplatesManager : public Wt::WContainerWidget
{
public:
    TemplatesManager(StylusSession& session);

    Wt::Signal<Wt::Dbo::ptr<TemplateFile>>& file_selected() { return file_selected_; }
    Wt::Signal<Wt::Dbo::ptr<MessageTemplate>>& template_selected() { return template_selected_; }

    void keyWentDown(Wt::WKeyEvent e);
private:
    enum class SelectedKind { None, Folder, File, Template };

    StylusSession& session_;
    Wt::WTree* tree_;
    Wt::WContainerWidget* contentWrapper_;

    SelectedKind selectedKind_ = SelectedKind::None;
    long long selectedFolderId_ = -1;
    long long selectedFileId_ = -1;
    long long selectedTemplateId_ = -1;

    Wt::Signal<Wt::Dbo::ptr<TemplateFile>> file_selected_;
    Wt::Signal<Wt::Dbo::ptr<MessageTemplate>> template_selected_;

    void populateTree();
    void renderSelection();
    std::vector<TempView*> tempViews_;

};

}
