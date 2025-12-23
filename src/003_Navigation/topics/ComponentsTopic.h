#pragma once

#include "../NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class ComponentsTopic : public NavigationTopic
{
public:
    ComponentsTopic();
    std::unique_ptr<Wt::WWidget> createMonacoEditorDemo();

private:
    std::unique_ptr<Wt::WWidget> monacoEditorDemo();
};
