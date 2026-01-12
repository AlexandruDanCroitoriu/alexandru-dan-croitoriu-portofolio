#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/Dbo/ptr.h>

class StylusSession;
class MessageTemplate;

namespace Stylus
{
        class XmlBrain;

        enum TemplateViewMode
        {
                Template,
                Editor
        };

        class TempView : public Wt::WContainerWidget
        {
        public:
                TempView(StylusSession &session, Wt::Dbo::ptr<MessageTemplate> file);
                ~TempView();
                void setViewMode(TemplateViewMode mode = TemplateViewMode::Editor);
                
                void keyWentDown(Wt::WKeyEvent e);
                std::shared_ptr<XmlBrain> xmlBrain_;
        private:
                StylusSession &session_;
                Wt::Dbo::ptr<MessageTemplate> file_;

                Wt::WContainerWidget *leftArea_;
                Wt::WContainerWidget *mainArea_;
                Wt::WContainerWidget *rightArea_;
                Wt::Signals::connection globalKeyConnection_;

        };

} // namespace Stylus