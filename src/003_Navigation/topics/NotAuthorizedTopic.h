#pragma once

#include "../NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class NotAuthorizedTopic : public NavigationTopic
{
public:
    NotAuthorizedTopic();
    std::unique_ptr<Wt::WWidget> createNotAuthorizedPage();

private:
    std::unique_ptr<Wt::WWidget> notAuthorizedPage();
};
