#pragma once

#include <Wt/WObject.h>
#include <Wt/WString.h>

namespace Wt {
    class WMenu;
}

class NavigationTopic : public Wt::WObject
{
public:
    NavigationTopic();
    virtual ~NavigationTopic() = default;

    virtual void populateSubMenu(Wt::WMenu* menu) = 0;
};
