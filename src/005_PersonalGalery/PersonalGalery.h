#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include <Wt/WPushButton.h>

class GalleryTopic;

class PersonalGalery : public Wt::WContainerWidget
{
public:
    PersonalGalery();

private:
    Wt::WStackedWidget* contentsStack_;
    Wt::WMenu* mainMenu_;
    Wt::WPushButton* menuToggleButton_;
    Wt::WContainerWidget* sidebar_;
    Wt::WContainerWidget* contentsCover_;
    bool menuOpen_;

    Wt::WMenuItem* addToMenu(Wt::WMenu* menu,
                             const Wt::WString& name,
                             std::unique_ptr<GalleryTopic> topic);
    void toggleMenu();
    void openMenu();
    void closeMenu();
};
