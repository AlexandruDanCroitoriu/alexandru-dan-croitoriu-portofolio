#include "008_Stylus/TemplatesManager/TempView.h"
#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/MessageTemplate.h"

#include <Wt/WApplication.h>
#include <Wt/WTemplate.h>

namespace Stylus
{

    TempView::TempView(StylusSession& session, Wt::Dbo::ptr<MessageTemplate> file)
        : session_(session),
            file_(file)
    {
        wApp->log("debug") << "TempView::TempView(StylusSession& session)";
        // WTemplate does not accept TextFormat in constructor; set it separately
            auto temp = addNew<Wt::WTemplate>(Wt::WString::fromUTF8(file_->templateXml_));
    }

} // namespace Stylus