#pragma once

#include "002_Components/MonacoEditor.h"

#include <Wt/WSignal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/ptr.h>

class StylusSession;
class TemplateFile;

namespace Stylus
{

    class FilesManagerSidebar : public Wt::WContainerWidget
    {
    public:
        FilesManagerSidebar();
        Wt::WContainerWidget* contents_;
        Wt::Signal<Wt::WString>& width_changed() { return width_changed_; }

    protected:
        void layoutSizeChanged(int width, int height) override;
        
    private:
        Wt::Signal<Wt::WString> width_changed_;
    };

    class FilesManager : public Wt::WContainerWidget
    {
    public:
        FilesManager(StylusSession& session, int sidebar_width = 240);

        Wt::WGridLayout* grid_layout_;
        MonacoEditor* editor_;

        void setTreeFolderWidgets();
        void reuploadFile();
        
        Wt::Signal<Wt::WString>& file_saved() { return file_saved_; }
        Wt::Signal<>& file_selected() { return file_selected_; }

        Wt::Signal<std::string> folders_changed_;
        std::string selected_file_path_;
        
        FilesManagerSidebar* sidebar_;

    private:
        StylusSession& session_;
        Wt::WTree* tree_;
        Wt::Dbo::ptr<TemplateFile> selected_file_;

        Wt::Signal<Wt::WString> file_saved_;
        Wt::Signal<> file_selected_;
    };
    
}