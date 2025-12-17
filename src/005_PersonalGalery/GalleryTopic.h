#pragma once

#include <Wt/WObject.h>
#include <Wt/WString.h>

namespace Wt {
    class WMenu;
}

class GalleryTopic : public Wt::WObject
{
public:
    GalleryTopic();
    virtual ~GalleryTopic() = default;

    virtual void populateSubMenu(Wt::WMenu* menu) = 0;
};
