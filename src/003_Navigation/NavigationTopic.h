#pragma once

#include <Wt/WObject.h>

// Base class for content topics (minimal interface, no submenu needed)
class NavigationTopic : public Wt::WObject
{
public:
    NavigationTopic() = default;
    virtual ~NavigationTopic() = default;
};
