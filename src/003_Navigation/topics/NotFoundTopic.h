#pragma once

#include "../NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>
#include <string>

class NotFoundTopic : public NavigationTopic
{
public:
    NotFoundTopic();
    std::unique_ptr<Wt::WWidget> createNotFoundPage(const std::string& attemptedPath);

private:
    std::unique_ptr<Wt::WWidget> notFoundPage(const std::string& attemptedPath);
};
