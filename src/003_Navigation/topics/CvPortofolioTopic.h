#pragma once

#include "../NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>

class CvPortofolioTopic : public NavigationTopic
{
public:
    CvPortofolioTopic();
    
    void populateSubMenu(Wt::WMenu* menu) override;
    std::unique_ptr<Wt::WWidget> createCvPage();

private:
    std::unique_ptr<Wt::WWidget> cvPage();
};
