#pragma once

#include "../NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class ComponentsTopic : public NavigationTopic
{
public:
    ComponentsTopic();
    void populateSubMenu(Wt::WMenu* menu) override;

private:
    std::unique_ptr<Wt::WWidget> monacoEditorDemo();
};
