#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/Dbo/ptr.h>
#include <memory>

namespace Stylus
{

class StylusSession;
class MessageTemplate;
enum class ViewMode;
class XmlBrain;

class TempView : public Wt::WContainerWidget
{
public:
        TempView(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<MessageTemplate> messageTemplate);
        ~TempView();
        void setViewMode(ViewMode mode);
                
                void keyWentDown(Wt::WKeyEvent e);
        std::shared_ptr<XmlBrain> xmlBrain_;
private:
        std::shared_ptr<StylusSession> session_;
        Wt::Dbo::ptr<MessageTemplate> messageTemplate_;

        Wt::WContainerWidget *leftArea_;
        Wt::WContainerWidget *mainArea_;
        // Wt::WContainerWidget *rightArea_;
        Wt::Signals::connection globalKeyConnection_;
        void refreshView();
};

} // namespace Stylus