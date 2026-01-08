#pragma once

#include "003_Navigation/NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class WtComponentsGalery : public NavigationTopic
{
public:
    WtComponentsGalery();
    std::unique_ptr<Wt::WWidget> createFormInputsDemo();

private:
    std::unique_ptr<Wt::WWidget> formInputsDemo();

    void createLineEditExample(Wt::WContainerWidget* wrapper);
    void createTextAreaExample(Wt::WContainerWidget* wrapper);
};