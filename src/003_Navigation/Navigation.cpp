#include "003_Navigation/Navigation.h"
#include "003_Navigation/NavigationTopic.h"
#include "003_Navigation/topics/ComponentsTopic.h"
#include "003_Navigation/topics/CvPortofolioTopic.h"
#include "003_Navigation/topics/UserSettingsTopic.h"
#include "003_Navigation/topics/NotFoundTopic.h"
#include "003_Navigation/topics/NotAuthorizedTopic.h"
#include "005_Dbo/Session.h"

#include <Wt/WText.h>
#include <Wt/WAnimation.h>
#include <Wt/WTemplate.h>
#include <Wt/WApplication.h>
#include <Wt/Auth/Login.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>

Navigation::Navigation(std::shared_ptr<Session> session)
    : session_(session), menuOpen_(false), currentActiveAnchor_(nullptr)
{
    lastUnknownPath_ = std::make_shared<std::string>("");
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

    // Create main content area container
    contentsArea_ = addNew<Wt::WContainerWidget>();
    // Add left margin on medium+ screens to account for fixed 16rem sidebar
    contentsArea_->addStyleClass("flex-1 flex flex-col w-full md:ml-64");

    authDialog_ = wApp->root()->addNew<Wt::WDialog>("");
    authDialog_->keyWentDown().connect([=](Wt::WKeyEvent e) {
        wApp->globalKeyWentDown().emit(e); // Emit the global key event
    });
    authDialog_->setTitleBarEnabled(false);
    authDialog_->setClosable(false);
    authDialog_->setModal(true);
    authDialog_->escapePressed().connect([this]() {
        if (authDialog_ != nullptr) {
            authDialog_->hide();
        }
    });
    authDialog_->setStyleClass("absolute bg-gray-900 rounded-md [&>div]:h-full");
    authDialog_->addStyleClass("top-0 left-0 right-0 bottom-0");
    // authDialog_->addStyleClass("sm:top-5 sm:left-5 sm:right-5 sm:bottom-5"); // Centered on small+ screens
    authDialog_->contents()->setStyleClass("p-4 h-full flex justify-center");

    authWidget_ = authDialog_->contents()->addWidget(std::make_unique<AuthWidget>(session_));
    authWidget_->addStyleClass("bg-gray-700 text-white p-4 rounded-md shadow-lg w-full max-w-sm mt-20 h-fit");
    authWidget_->keyWentDown().connect([=](Wt::WKeyEvent e) {
        wApp->globalKeyWentDown().emit(e); // Emit the global key event
    });

    // Create overlay for mobile
    contentsCover_ = addNew<Wt::WContainerWidget>();
    contentsCover_->addStyleClass("fixed inset-0 bg-black/40 hidden z-40");
    contentsCover_->clicked().connect(this, &Navigation::closeMenu);
    
    session_->login().changed().connect(this, &Navigation::authChanged);
    authWidget_->processEnvironment();
    session_->login().changed().emit();
    // authDialog_->show();
    
}

void Navigation::setUI()
{
    currentActiveAnchor_ = nullptr;
    anchorsByPath_.clear();
    routes_.clear();

    sidebar_->clear();
    contentsArea_->clear();

    // Create mobile menu toggle button
    menuToggleButton_ = contentsArea_->addNew<Wt::WPushButton>();
    menuToggleButton_->addStyleClass("md:hidden fixed top-4 right-4 z-50 bg-gray-800 text-white px-4 py-2 rounded-lg shadow-lg hover:bg-gray-700 transition-colors");
    menuToggleButton_->setText("☰");
    menuToggleButton_->clicked().connect(this, &Navigation::toggleMenu);

    // Create logo/title area using WTemplate
    auto logoArea = sidebar_->addNew<Wt::WTemplate>();
    logoArea->addStyleClass("relative h-fit flex items-center pb-4 shrink-0 border-b border-gray-700");
    logoArea->setTemplateText(
        "<div class=\"w-12 font-white text-white\">${tr:favicon.svg class=\"\"}</div>"
        "<div class=\"mx-auto\">"
        "  <div class=\"font-bold tracking-wide text-xl\">Alexandru Dan</div>"
        "  <div class=\"font-bold tracking-wide text-xl\">Croitoriu</div>"
        "</div>"
    );
    logoArea->addFunction("tr", &Wt::WTemplate::Functions::tr);

    // Create stacked widget for content
    contentsStack_ = contentsArea_->addNew<Wt::WStackedWidget>();
    contentsStack_->addStyleClass("flex-1 overflow-auto");
    contentsStack_->setHeight(Wt::WLength(100, Wt::LengthUnit::Percentage));

    // Nav list container below logo
    navList_ = sidebar_->addNew<Wt::WContainerWidget>();
    navList_->addStyleClass("flex flex-col space-y-1 flex-1 h-full overflow-y-auto mt-4");

    // Create auth wrapper at bottom of sidebar
    authWrapper_ = sidebar_->addNew<Wt::WContainerWidget>();
    authWrapper_->addStyleClass("mt-auto m-2 border-t pt-2 border-gray-700");

    // Define routes and build sidebar anchors
    setupRoutes();
    buildSidebar();
    connectRouting();

    // Show initial route
    auto path = wApp->internalPath();
    if (path.empty()) {
        path = "/";
    }
    navigateTo(path);
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

void Navigation::setupRoutes()
{
    // Topics instances captured by value in route lambdas
    auto cvTopic = std::make_shared<CvPortofolioTopic>();
    auto components = std::make_shared<ComponentsTopic>();
    auto userSettings = std::make_shared<UserSettingsTopic>(session_);
    auto notFound = std::make_shared<NotFoundTopic>();
    auto notAuthorized = std::make_shared<NotAuthorizedTopic>();

    routes_["/"] = [cvTopic]() {
        return cvTopic->createCvPage();
    };
    routes_["/components/monaco"] = [components]() {
        return components->createMonacoEditorDemo();
    };
    routes_["/account/settings"] = [userSettings]() {
        return userSettings->createSettingsPage();
    };
    authRequiredRoutes_["/account/settings"] = true;
    
    // Not Found and Not Authorized (special, always available)
    routes_["/not-found"] = [notFound, this]() {
        return notFound->createNotFoundPage(*lastUnknownPath_);
    };
    routes_["/not-authorized"] = [notAuthorized]() {
        return notAuthorized->createNotAuthorizedPage();
    };
}

void Navigation::buildSidebar()
{
    auto makeAnchor = [this](Wt::WContainerWidget* parent,
                             const std::string& label,
                             const std::string& path) {
        auto a = parent->addNew<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, path), Wt::WString::fromUTF8(label));
        a->addStyleClass("px-3 py-1 rounded-md font-medium transition-all duration-200 block w-full");
        a->addStyleClass("hover:bg-gray-600 cursor-pointer text-gray-300 hover:text-white");
        a->addStyleClass("border-l-2 border-transparent hover:border-blue-400");
        a->clicked().connect([this]() { closeMenu(); });
        anchorsByPath_[path] = a;
        return a;
    };

    // Top-level links
    makeAnchor(navList_, "Personal CV/Portfolio", "/");

    // Section: Components
    auto sectionHeader = navList_->addNew<Wt::WText>("<div class='px-3 pt-3 text-gray-400 text-xs uppercase tracking-wide'>Components</div>");
    sectionHeader->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    auto section = navList_->addNew<Wt::WContainerWidget>();
    section->addStyleClass("flex flex-col space-y-1 pl-4 mt-1");
    makeAnchor(section, "Monaco Editor", "/components/monaco");
}

void Navigation::connectRouting()
{
    // React to internal path changes
    wApp->internalPathChanged().connect([this]() {
        navigateTo(wApp->internalPath());
    });
}

void Navigation::markActive(const std::string& path)
{
    if (currentActiveAnchor_) {
        currentActiveAnchor_->removeStyleClass("bg-gray-600", true);
        currentActiveAnchor_->removeStyleClass("text-white", true);
        currentActiveAnchor_->removeStyleClass("border-blue-400", true);
    }
    auto it = anchorsByPath_.find(path);
    if (it != anchorsByPath_.end()) {
        currentActiveAnchor_ = it->second;
        currentActiveAnchor_->addStyleClass("bg-gray-600");
        currentActiveAnchor_->addStyleClass("text-white");
        currentActiveAnchor_->addStyleClass("border-blue-400");
    } else {
        currentActiveAnchor_ = nullptr;
    }
}

void Navigation::navigateTo(const std::string& rawPath)
{
    std::string path = rawPath;
    if (path.empty()) path = "/";
    
    auto it = routes_.find(path);
    
    // Check if path is known
    if (it == routes_.end()) {
        // Path not found: render not-found page WITHOUT changing the URL
        *lastUnknownPath_ = path;
        auto notFoundIt = routes_.find("/not-found");
        if (notFoundIt != routes_.end()) {
            contentsStack_->clear();
            contentsStack_->addWidget(notFoundIt->second());
            markActive("");  // Don't highlight any anchor for 404
        }
        return;
    }
    
    // Check if path requires auth
    bool requiresAuth = authRequiredRoutes_.count(path) && authRequiredRoutes_[path];
    if (requiresAuth && !session_->login().loggedIn()) {
        // Render not-authorized page WITHOUT changing the URL
        auto notAuthIt = routes_.find("/not-authorized");
        if (notAuthIt != routes_.end()) {
            contentsStack_->clear();
            contentsStack_->addWidget(notAuthIt->second());
            markActive("");  // Don't highlight any anchor for 403
        }
        return;
    }
    
    // Route found and authorized: render it
    contentsStack_->clear();
    contentsStack_->addWidget(it->second());
    markActive(path);
}

void Navigation::authChanged()
{
    std::cout << "\n\n Auth changed, logged in: " << session_->login().loggedIn() << "\n\n";
    // Rebuild UI without changing internal path; let the current path be handled by navigateTo()
    setUI();

    if (!session_->login().loggedIn()) {
        if(!authDialog_->isHidden()) {
            authDialog_->hide();
        }
        // Show login button
        auto loginButton = authWrapper_->addNew<Wt::WPushButton>("Login");
        loginButton->addStyleClass("w-full bg-blue-600 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded");
        loginButton->clicked().connect([this]() {
            if (authDialog_ != nullptr) {
                authDialog_->show();
            }
        });
    } else {
        if(!authDialog_->isHidden()) authDialog_->hide();

        // User is logged in
        // Get user name within transaction
        std::string userName;
        {
            dbo::Transaction transaction(*session_);
            userName = session_->user()->name_;
        }
        
        // Show user button with popup menu including Settings and Logout
        auto userMenu = std::make_unique<Wt::WPopupMenu>(contentsStack_);
        userMenu->addStyleClass("bg-slate-700 ring-gray-50/10 rounded-md shadow-lg");
        
        auto settingsItem = userMenu->addItem("Settings");
        settingsItem->addStyleClass("px-4 py-2 hover:bg-slate-800 block w-full text-gray-200 hover:text-white transition-colors");
        settingsItem->clicked().connect([this]() {
            // Route to settings via internal path so anchors/router handle it
            wApp->setInternalPath("/account/settings", true);
        });
        
        auto logoutItem = userMenu->addItem("Logout");
        logoutItem->addStyleClass("px-4 py-2 hover:bg-slate-800 block w-full border-t border-gray-700 mt-2 pt-2 text-gray-200 hover:text-white transition-colors");
        logoutItem->clicked().connect([this]() {
            session_->login().logout();
        });
        
        // Create user button with template content
        auto userContainer = authWrapper_->addNew<Wt::WContainerWidget>();
        userContainer->addStyleClass("bg-gray-50/10 hover:bg-gray-50/20 ring-gray-50/5 w-full rounded-md p-2 cursor-pointer");
        
        auto userTemplate = userContainer->addNew<Wt::WTemplate>(
            "<div class='flex items-center gap-2'>"
            "  <div class='inline-block size-8 overflow-hidden rounded-full bg-gray-100 outline outline-1 -outline-offset-1 outline-black/5 dark:bg-gray-800 dark:outline-white/10'>"
            "    ${icon}"
            "  </div>"
            "  <div class='flex flex-col'>"
            "    <div class='capitalize text-sm'>${login-name}</div>"
            "    <div class='capitalize text-xs text-gray-400'>${user-name}</div>"
            "  </div>"
            "</div>"
        );
        userTemplate->bindWidget("icon", std::make_unique<Wt::WTemplate>(Wt::WString::tr("user.empty.svg")));
        userTemplate->bindString("login-name", session_->login().user().identity(Wt::Auth::Identity::LoginName));
        userTemplate->bindString("user-name", userName);
        
        // Store menu pointer for use in closures
        auto menuPtr = userMenu.get();
        
        userContainer->clicked().connect([this, menuPtr, userContainer]() {
            if (menuPtr->isHidden()) {
                menuPtr->popup(userContainer, Wt::Orientation::Horizontal);
            } else {
                menuPtr->hide();
            }
        });
        
        // Close menu when clicking outside (on the content cover)
        contentsCover_->clicked().connect([menuPtr]() {
            if (!menuPtr->isHidden()) {
                menuPtr->hide();
            }
        });
        
        // Keep the menu alive by making it a child of root
        wApp->root()->addChild(std::move(userMenu));
    }
}

