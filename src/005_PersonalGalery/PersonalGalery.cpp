#include "005_PersonalGalery/PersonalGalery.h"
#include "005_PersonalGalery/GalleryTopic.h"
#include "005_PersonalGalery/topics/ComponentsTopic.h"

#include <Wt/WText.h>
#include <Wt/WAnimation.h>

PersonalGalery::PersonalGalery()
    : menuOpen_(false)
{
    // Apply main container styling
    addStyleClass("flex flex-row min-h-screen bg-gray-100");

    // Create sidebar
    sidebar_ = addNew<Wt::WContainerWidget>();
    sidebar_->addStyleClass("w-64 bg-gray-800 text-white flex flex-col transition-all duration-300");
    // Keep sidebar fixed on all breakpoints so it doesn't scroll with page
    sidebar_->addStyleClass("fixed inset-y-0 left-0 z-50");
    // Ensure the sidebar keeps viewport height and scrolls independently
    sidebar_->addStyleClass("h-screen overflow-y-auto");
    sidebar_->addStyleClass("transform md:transform-none transition-transform duration-300 ease-in-out");
    sidebar_->addStyleClass("-translate-x-full md:translate-x-0"); // Hidden on mobile by default

    // Create logo/title area
    auto logoArea = sidebar_->addNew<Wt::WContainerWidget>();
    logoArea->addStyleClass("p-6 border-b border-gray-700 flex justify-center pb-2 pt-2");
    auto title = logoArea->addNew<Wt::WText>("<h1 class='text-2xl font-bold'>Personal WT Gallery</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Create main content area container
    auto contentArea = addNew<Wt::WContainerWidget>();
    // Add left margin on medium+ screens to account for fixed 16rem sidebar
    contentArea->addStyleClass("flex-1 flex flex-col relative md:ml-64");

    // Create mobile menu toggle button
    menuToggleButton_ = contentArea->addNew<Wt::WPushButton>();
    menuToggleButton_->addStyleClass("md:hidden fixed bottom-4 right-4 z-50 bg-gray-800 text-white px-4 py-2 rounded-lg shadow-lg hover:bg-gray-700 transition-colors");
    menuToggleButton_->setText("☰");
    menuToggleButton_->clicked().connect(this, &PersonalGalery::toggleMenu);

    // Create stacked widget for content
    contentsStack_ = contentArea->addNew<Wt::WStackedWidget>();
    contentsStack_->addStyleClass("flex-1 p-6 overflow-auto");

    // Create menu
    mainMenu_ = sidebar_->addNew<Wt::WMenu>(contentsStack_);
    // Fill available sidebar height and scroll independently
    mainMenu_->addStyleClass("flex flex-col space-y-1 p-2 flex-1 h-full overflow-y-auto");
    // Don't enable internal paths for main menu - only submenus have paths
    
    // Auto-select first submenu item when main menu item is selected
    mainMenu_->itemSelected().connect([this](Wt::WMenuItem* item) {
        if (item && item->menu() && item->menu()->count() > 0) {
            // Select first submenu item, which will update the URL path
            item->menu()->select(0);
        }
    });

    // Create overlay for mobile
    contentsCover_ = addNew<Wt::WContainerWidget>();
    contentsCover_->addStyleClass("fixed inset-0 bg-black/40 hidden z-40");
    contentsCover_->clicked().connect(this, &PersonalGalery::closeMenu);

    // Add transition animation
    Wt::WAnimation animation(Wt::AnimationEffect::Fade,
                            Wt::TimingFunction::Linear,
                            200);
    contentsStack_->setTransitionAnimation(animation, true);

    // Populate menu with topics
    addToMenu(mainMenu_, "Components", std::make_unique<ComponentsTopic>());

    // Select first menu item to trigger auto-select of first submenu
    mainMenu_->select(0);
    if (mainMenu_->itemAt(0)->menu()) {
        mainMenu_->itemAt(0)->menu()->select(0);
    }
}

void PersonalGalery::toggleMenu()
{
    if (menuOpen_) {
        closeMenu();
    } else {
        openMenu();
    }
}

void PersonalGalery::openMenu()
{
    if (menuOpen_) return;
    
    sidebar_->removeStyleClass("-translate-x-full");
    sidebar_->addStyleClass("translate-x-0");
    contentsCover_->removeStyleClass("hidden");
    menuToggleButton_->setText("✕");
    
    menuOpen_ = true;
}

void PersonalGalery::closeMenu()
{
    if (!menuOpen_) return;
    
    sidebar_->addStyleClass("-translate-x-full");
    sidebar_->removeStyleClass("translate-x-0");
    contentsCover_->addStyleClass("hidden");
    menuToggleButton_->setText("☰");
    
    menuOpen_ = false;
}

Wt::WMenuItem* PersonalGalery::addToMenu(Wt::WMenu* menu,
                                         const Wt::WString& name,
                                         std::unique_ptr<GalleryTopic> topic)
{
    auto topicPtr = addChild(std::move(topic));
    
    // Create submenu
    auto subMenuPtr = std::make_unique<Wt::WMenu>(contentsStack_);
    auto subMenu = subMenuPtr.get();
    
    // Create menu item with submenu
    auto itemPtr = std::make_unique<Wt::WMenuItem>(name);
    itemPtr->setMenu(std::move(subMenuPtr));
    
    auto item = menu->addItem(std::move(itemPtr));
    
    // Style main menu item
    item->addStyleClass("px-4 py-3 rounded-lg text-gray-200 font-medium");
    
    // Style submenu
    subMenu->addStyleClass("flex flex-col space-y-1 pl-4 mt-2");
    
    // Style submenu items
    auto styleSubMenuItem = [](Wt::WMenuItem* subItem) {
        subItem->addStyleClass("px-3 py-2 rounded-md text-sm transition-all duration-200");
        subItem->addStyleClass("hover:bg-gray-600 cursor-pointer text-gray-300");
        subItem->addStyleClass("hover:text-white border-l-2 border-transparent");
        subItem->addStyleClass("hover:border-blue-400");
    };
    
    // Close mobile menu when submenu item selected
    subMenu->itemSelected().connect(this, &PersonalGalery::closeMenu);
    
    // Enable internal paths for submenu only (not main menu)
    subMenu->setInternalPathEnabled(item->pathComponent() + "/");
    
    // Populate submenu
    topicPtr->populateSubMenu(subMenu);
    
    // Apply styling to all submenu items
    for (int i = 0; i < subMenu->count(); ++i) {
        styleSubMenuItem(subMenu->itemAt(i));
    }
    
    return item;
}
