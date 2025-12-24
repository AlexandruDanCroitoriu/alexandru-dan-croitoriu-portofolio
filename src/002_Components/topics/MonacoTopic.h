#pragma once

#include "003_Navigation/NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class MonacoTopic : public NavigationTopic
{
public:
    MonacoTopic();
    std::unique_ptr<Wt::WWidget> createMonacoEditorDemo();

private:
    std::unique_ptr<Wt::WWidget> monacoEditorDemo();
};
