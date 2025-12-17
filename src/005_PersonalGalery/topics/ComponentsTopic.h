#pragma once

#include "../GalleryTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class ComponentsTopic : public GalleryTopic
{
public:
    ComponentsTopic();
    void populateSubMenu(Wt::WMenu* menu) override;

private:
    std::unique_ptr<Wt::WWidget> monacoEditorDemo();
};
