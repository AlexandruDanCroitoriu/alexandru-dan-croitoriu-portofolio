#include "003_Navigation/Navigation.h"
#include "003_Navigation/NavigationTopic.h"
#include "003_Navigation/topics/ComponentsTopic.h"
#include "003_Navigation/topics/CvPortofolioTopic.h"

#include <Wt/WText.h>
#include <Wt/WAnimation.h>
#include <Wt/WTemplate.h>
#include <Wt/WApplication.h>

Navigation::Navigation()
    : menuOpen_(false), currentSelectedItem_(nullptr)
{
    // Apply main container styling
    addStyleClass("flex flex-row h-screen bg-gray-100");

    // Create sidebar
    sidebar_ = addNew<Wt::WContainerWidget>();
    sidebar_->addStyleClass("w-64 p-2 bg-gray-800 text-white flex flex-col transition-all duration-300");
    // Keep sidebar fixed on all breakpoints so it doesn't scroll with page
    sidebar_->addStyleClass("fixed inset-y-0 left-0 z-50");
    // Ensure the sidebar keeps viewport height and scrolls independently
    sidebar_->addStyleClass("h-screen overflow-y-auto");
    sidebar_->addStyleClass("transform md:transform-none transition-transform duration-300 ease-in-out");
    sidebar_->addStyleClass("-translate-x-full md:translate-x-0"); // Hidden on mobile by default

    // Create logo/title area using WTemplate
    auto logoArea = sidebar_->addNew<Wt::WTemplate>();
    logoArea->addStyleClass("relative h-fit flex items-center pb-4 shrink-0 border-b border-gray-700");
    logoArea->setTemplateText(
        "<div class=\"\">${tr:favicon.svg class=\"\"}</div>"
        "<div class=\"mx-auto\">"
        "  <div class=\"font-bold tracking-wide text-xl\">Alexandru Dan</div>"
        "  <div class=\"font-bold tracking-wide text-xl\">Croitoriu</div>"
        "</div>"
    );
    logoArea->addFunction("tr", &Wt::WTemplate::Functions::tr);

    // Create main content area container
    auto contentArea = addNew<Wt::WContainerWidget>();
    // Add left margin on medium+ screens to account for fixed 16rem sidebar
    contentArea->addStyleClass("flex-1 flex flex-col w-full md:ml-64");

    // Create mobile menu toggle button
    menuToggleButton_ = contentArea->addNew<Wt::WPushButton>();
    menuToggleButton_->addStyleClass("md:hidden fixed top-4 right-4 z-50 bg-gray-800 text-white px-4 py-2 rounded-lg shadow-lg hover:bg-gray-700 transition-colors");
    menuToggleButton_->setText("☰");
    menuToggleButton_->clicked().connect(this, &Navigation::toggleMenu);

    // Create stacked widget for content
    contentsStack_ = contentArea->addNew<Wt::WStackedWidget>();
    contentsStack_->addStyleClass("flex-1 overflow-auto");
    contentsStack_->setHeight(Wt::WLength(100, Wt::LengthUnit::Percentage));

    // Create menu
    mainMenu_ = sidebar_->addNew<Wt::WMenu>(contentsStack_);
    // Fill available sidebar height and scroll independently
    mainMenu_->addStyleClass("flex flex-col space-y-1 flex-1 h-full overflow-y-auto mt-4");
    // Don't enable internal paths for main menu - only submenus have paths
    
    // Auto-select first submenu item when main menu item is selected
    mainMenu_->itemSelected().connect([this](Wt::WMenuItem* item) {
        if (item && item->menu() && item->menu()->count() > 0) {
            // Select first submenu item, which will update the URL path and active state
            item->menu()->select(0);
            return;
        }

        setActiveItem(item);
        closeMenu();
    });

    // Create overlay for mobile
    contentsCover_ = addNew<Wt::WContainerWidget>();
    contentsCover_->addStyleClass("fixed inset-0 bg-black/40 hidden z-40");
    contentsCover_->clicked().connect(this, &Navigation::closeMenu);

    // Add transition animation
    // Wt::WAnimation animation(Wt::AnimationEffect::Fade,
    //                         Wt::TimingFunction::Linear,
    //                         200);
    // contentsStack_->setTransitionAnimation(animation, true);

    // Populate menu with topics
    auto cvTopic = std::make_unique<CvPortofolioTopic>();
    auto cvWidget = cvTopic->createCvPage();

    addDirectItemToMenu(mainMenu_, "Personal CV/Portfolio", std::move(cvWidget), "");
    addToMenu(mainMenu_, "Components", std::make_unique<ComponentsTopic>());
}

void Navigation::toggleMenu()
{
    if (menuOpen_) {
        closeMenu();
    } else {
        openMenu();
    }
}

void Navigation::openMenu()
{
    if (menuOpen_) return;
    
    sidebar_->removeStyleClass("-translate-x-full");
    sidebar_->addStyleClass("translate-x-0");
    contentsCover_->removeStyleClass("hidden");
    menuToggleButton_->setText("✕");
    
    menuOpen_ = true;
}

void Navigation::closeMenu()
{
    if (!menuOpen_) return;
    
    sidebar_->addStyleClass("-translate-x-full");
    sidebar_->removeStyleClass("translate-x-0");
    contentsCover_->addStyleClass("hidden");
    menuToggleButton_->setText("☰");
    
    menuOpen_ = false;
}

void Navigation::setActiveItem(Wt::WMenuItem* item)
{
    if (!item || !mainMenu_) return;

    if (currentSelectedItem_ && currentSelectedItem_ != item) {
        currentSelectedItem_->removeStyleClass("active", true);
    }

    // Clear lingering active classes across all menu levels except the target item
    for (int i = 0; i < mainMenu_->count(); ++i) {
        auto mainItem = mainMenu_->itemAt(i);
        if (!mainItem) continue;

        if (mainItem != item) {
            mainItem->removeStyleClass("active", true);
        }

        if (!mainItem->menu()) continue;

        auto subMenu = mainItem->menu();
        for (int j = 0; j < subMenu->count(); ++j) {
            auto subItem = subMenu->itemAt(j);
            if (!subItem || subItem == item) continue;
            subItem->removeStyleClass("active", true);
        }
    }

    currentSelectedItem_ = item;
}

Wt::WMenuItem* Navigation::addToMenu(Wt::WMenu* menu,
                                    const Wt::WString& name,
                                    std::unique_ptr<NavigationTopic> topic)
{
    auto topicPtr = addChild(std::move(topic));
    
    // Create submenu
    auto subMenuPtr = std::make_unique<Wt::WMenu>(contentsStack_);
    auto subMenu = subMenuPtr.get();
    
    // Create menu item with submenu
    auto itemPtr = std::make_unique<Wt::WMenuItem>(name);
    itemPtr->setMenu(std::move(subMenuPtr));
    
    auto item = menu->addItem(std::move(itemPtr));
    item->setPathComponent(name.toUTF8());
    item->setInternalPathEnabled(false);
    // item->disable(); // Disable direct selection of main menu item

    // Style main menu item
    item->anchor()->addStyleClass("px-3 py-1 rounded-lg text-gray-200 w-full block font-medium");
    item->anchor()->addStyleClass("hover:bg-gray-600 hover:text-white transition-all duration-200");
    
    
    // Style submenu
    subMenu->addStyleClass("flex flex-col space-y-1 pl-4 mt-1");
    
    // Style submenu items
    auto styleSubMenuItem = [](Wt::WMenuItem* subItem) {
        subItem->anchor()->addStyleClass("px-3 py-1 rounded-md text-sm transition-all duration-200 block w-full");
        subItem->anchor()->addStyleClass("hover:bg-gray-600 cursor-pointer text-gray-300");
        subItem->anchor()->addStyleClass("hover:text-white border-l-2 border-transparent");
        subItem->anchor()->addStyleClass("hover:border-blue-400");
        subItem->addStyleClass("[&.active>a]:border-blue-400 [&.active>a]:text-white [&.active>a]:bg-gray-600");
    };
    
    // Close mobile menu when submenu item selected and track active state
    subMenu->itemSelected().connect([this](Wt::WMenuItem* subItem) {
        setActiveItem(subItem);
        closeMenu();
    });
    
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

Wt::WMenuItem* Navigation::addDirectItemToMenu(Wt::WMenu* menu,
                                               const Wt::WString& name,
                                               std::unique_ptr<Wt::WWidget> content,
                                               const Wt::WString& path)
{
    // Create direct menu item (no submenu)
    auto item = menu->addItem(name, std::move(content));
    
    // Style main menu item
    item->anchor()->addStyleClass("px-3 py-1 rounded-md font-medium transition-all duration-200 block w-full");
    item->anchor()->addStyleClass("hover:bg-gray-600 cursor-pointer text-gray-300");
    item->anchor()->addStyleClass("hover:text-white border-l-2 border-transparent");
    item->anchor()->addStyleClass("hover:border-blue-400");

    item->addStyleClass("[&.active>a]:border-blue-400 [&.active>a]:text-white [&.active>a]:bg-gray-600");

    // Enable internal path
    std::string pathStr = path.toUTF8();
    item->setPathComponent(pathStr);
    
    // Only enable internal paths if not already enabled
    if (!menu->internalPathEnabled()) {
        // Use "/" as base path if path is empty
        menu->setInternalPathEnabled(pathStr.empty() ? "/" : pathStr);
    }
    
    return item;
}
