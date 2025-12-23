#pragma once

#include "003_Navigation/NavigationTopic.h"
#include <memory>

class Session;

class UserSettingsTopic : public NavigationTopic
{
public:
    UserSettingsTopic(std::shared_ptr<Session> session);
    std::unique_ptr<Wt::WWidget> createSettingsPage();

private:
    std::shared_ptr<Session> session_;
};
