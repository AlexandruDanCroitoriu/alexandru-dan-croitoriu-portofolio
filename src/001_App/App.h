#pragma once

#include <Wt/WApplication.h>
#include "005_Dbo/Session.h"
#include <memory>

class App : public Wt::WApplication
{
public:
    App(const Wt::WEnvironment& env);
    std::shared_ptr<Session> session() { return session_; }

private:
    std::shared_ptr<Session> session_;
    void createApp();
};