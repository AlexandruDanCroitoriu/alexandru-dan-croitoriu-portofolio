#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/Dbo/ptr.h>

class StylusSession;
class MessageTemplate;

namespace Stylus
{

    class TempView : public Wt::WContainerWidget
    {
    public:
            TempView(StylusSession& session, Wt::Dbo::ptr<MessageTemplate> file);
    private:
            StylusSession& session_;
            Wt::Dbo::ptr<MessageTemplate> file_;
    };

}