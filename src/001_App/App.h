#pragma once

#include <Wt/WApplication.h>

class App : public Wt::WApplication
{
public:
    App(const Wt::WEnvironment& env);

private:
    void createApp();
};