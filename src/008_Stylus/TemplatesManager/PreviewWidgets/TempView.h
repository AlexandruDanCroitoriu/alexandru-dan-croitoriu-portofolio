#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/Dbo/ptr.h>
#include <memory>

class MonacoEditor;

namespace Stylus
{

class StylusSession;
class MessageTemplate;
enum class ViewMode;
class XmlBrain;
class StylusState;

class TempView : public Wt::WContainerWidget
{
public:
        TempView(std::shared_ptr<StylusSession> session, Wt::Dbo::ptr<MessageTemplate> messageTemplate, std::shared_ptr<StylusState> stylusState = nullptr);
        ~TempView();
                
        void keyWentDown(Wt::WKeyEvent e);
        std::shared_ptr<XmlBrain> xmlBrain_;
private:
        std::shared_ptr<StylusSession> session_;
        std::shared_ptr<StylusState> stylusState_;
        Wt::Dbo::ptr<MessageTemplate> messageTemplate_;


        Wt::WContainerWidget *centralArea_;
        Wt::WContainerWidget *leftArea_;
        Wt::WContainerWidget *centralMainArea_;
        MonacoEditor* monaco_;
        Wt::WContainerWidget *rightArea_;
        Wt::Signals::connection globalKeyConnection_;
        void refreshView();
};

} // namespace Stylus