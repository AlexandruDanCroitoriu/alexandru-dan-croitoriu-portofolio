#include "003_Navigation/Navigation.h"
#include "003_Navigation/NavigationTopic.h"
#include "002_Components/topics/MonacoTopic.h"
#include "003_Navigation/topics/CvPortofolioTopic.h"
#include "007_Blog/topics/BlogTopic.h"
#include "007_Blog/topics/NewPostTopic.h"
#include "007_Blog/topics/EditPostTopic.h"
#include "007_Blog/topics/PostDetailTopic.h"
#include "006_Auth/topics/UserSettingsTopic.h"
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
    widgetCache_.clear();

    sidebar_->clear();
    contentsArea_->clear();

    // Create mobile menu toggle button
    menuToggleButton_ = contentsArea_->addNew<Wt::WPushButton>();
    menuToggleButton_->addStyleClass("md:hidden fixed top-4 right-4 z-50 bg-gray-800 text-white px-4 py-2 rounded-lg shadow-lg hover:bg-gray-700 transition-colors");
    menuToggleButton_->setText("☰");
    menuToggleButton_->clicked().connect(this, &Navigation::toggleMenu);

    // Create logo/title area using direct text (avoid SVG template which causes Magick crash)
    auto logoArea = sidebar_->addNew<Wt::WContainerWidget>();
    logoArea->addStyleClass("relative h-fit flex items-center pb-4 shrink-0 border-b border-gray-700");
    
    auto logoDirDiv = logoArea->addNew<Wt::WContainerWidget>();
    logoDirDiv->addStyleClass("mx-auto");
    logoDirDiv->addNew<Wt::WText>("Alexandru Dan")->addStyleClass("font-bold tracking-wide text-xl");
    logoDirDiv->addNew<Wt::WBreak>();
    logoDirDiv->addNew<Wt::WText>("Croitoriu")->addStyleClass("font-bold tracking-wide text-xl");

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
    auto monacoTopic = std::make_shared<MonacoTopic>();
    auto blogTopic = std::make_shared<BlogTopic>(session_);
    auto userSettings = std::make_shared<UserSettingsTopic>(session_);
    auto notFound = std::make_shared<NotFoundTopic>();
    auto notAuthorized = std::make_shared<NotAuthorizedTopic>();
    auto newPostTopic = std::make_shared<NewPostTopic>(session_);

    routes_["/"] = [cvTopic]() {
        return cvTopic->createCvPage();
    };
    routes_["/blog"] = [blogTopic]() {
        return blogTopic->createBlogPage();
    };
    routes_["/blog/new"] = [this, newPostTopic, notAuthorized]() {
        // Admin-only route
        // Check BLOG_ADMIN permission
        dbo::Transaction t(*session_);
        auto user = session_->user();
        auto perms = session_->find<Permission>().where("name = ?").bind("BLOG_ADMIN").resultList();
        bool isAdmin = user && !perms.empty() && user->hasPermission(perms.front());
        if (!isAdmin) {
            return notAuthorized->createNotAuthorizedPage();
        }
        return newPostTopic->createNewPostPage();
    };
    pathPatterns_.push_back({
        std::regex("^/blog/post/([^/]+)$"),
        [this](const std::smatch& match) -> std::unique_ptr<Wt::WWidget> {
            std::string slug = match[1].str();
            auto postDetail = std::make_shared<PostDetailTopic>(session_, slug);
            return postDetail->createPostDetailPage();
        }
    });
    pathPatterns_.push_back({
        std::regex("^/blog/post/([^/]+)/edit$"),
        [this, notAuthorized](const std::smatch& match) -> std::unique_ptr<Wt::WWidget> {
            bool isAdmin = false;
            {
                dbo::Transaction t(*session_);
                if (session_->login().loggedIn()) {
                    auto user = session_->user();
                    if (user) {
                        auto perms = session_->find<Permission>().where("name = ?").bind("BLOG_ADMIN").resultList();
                        if (!perms.empty()) {
                            isAdmin = user->hasPermission(perms.front());
                        }
                    }
                }
            }
            
            if (!isAdmin) {
                return notAuthorized->createNotAuthorizedPage();
            }
            std::string slug = match[1].str();
            auto editTopic = std::make_shared<EditPostTopic>(session_, slug);
            return editTopic->createEditPostPage();
        }
    });
    routes_["/components/monaco"] = [monacoTopic]() {
        return monacoTopic->createMonacoEditorDemo();
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
    // Nested under CV/Portfolio
    auto portfolioSection = navList_->addNew<Wt::WContainerWidget>();
    portfolioSection->addStyleClass("flex flex-col space-y-1 pl-4 mt-1");
    makeAnchor(portfolioSection, "Blog", "/blog");

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
    if (internalPathConnection_.isConnected()) {
        internalPathConnection_.disconnect();
    }
    internalPathConnection_ = wApp->internalPathChanged().connect([this]() {
        navigateTo(wApp->internalPath());
    });
}

void Navigation::markActive(const std::string& path)
{
    std::cout << "\nMarking active path: " << path << "\n";
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
    
    // Strip query parameters for route matching (but keep them in URL for widgets to read)
    std::string pathWithoutQuery = path;
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        pathWithoutQuery = path.substr(0, queryPos);
    }
    
    auto it = routes_.find(pathWithoutQuery);
    
    // Check exact routes first
    if (it != routes_.end()) {
        // Check if path requires auth
        bool requiresAuth = authRequiredRoutes_.count(pathWithoutQuery) && authRequiredRoutes_[pathWithoutQuery];
        if (requiresAuth && !session_->login().loggedIn()) {
            // Render not-authorized page WITHOUT changing the URL
            auto notAuthIt = routes_.find("/not-authorized");
            if (notAuthIt != routes_.end()) {
                // Check cache first
                auto cacheIt = widgetCache_.find("/not-authorized");
                Wt::WWidget* widget = nullptr;
                if (cacheIt != widgetCache_.end()) {
                    widget = cacheIt->second;
                } else {
                    auto newWidget = notAuthIt->second();
                    widget = newWidget.get();
                    widgetCache_["/not-authorized"] = widget;
                    contentsStack_->addWidget(std::move(newWidget));
                }
                contentsStack_->setCurrentWidget(widget);
                markActive("");  // Don't highlight any anchor for 403
            }
            return;
        }
        
        // Route found and authorized: check cache or create new widget
        auto cacheIt = widgetCache_.find(pathWithoutQuery);
        Wt::WWidget* widget = nullptr;
        if (cacheIt != widgetCache_.end()) {
            // Widget already exists in cache, just show it
            widget = cacheIt->second;
        } else {
            // Create new widget and add to cache
            auto newWidget = it->second();
            widget = newWidget.get();
            widgetCache_[pathWithoutQuery] = widget;
            contentsStack_->addWidget(std::move(newWidget));
        }
        contentsStack_->setCurrentWidget(widget);
        markActive(pathWithoutQuery);
        return;
    }
    
    // Try pattern-based routes
    for (const auto& pattern : pathPatterns_) {
        std::smatch match;
        if (std::regex_match(pathWithoutQuery, match, pattern.pattern)) {
            // For pattern-based routes, use full path as cache key
            auto cacheIt = widgetCache_.find(pathWithoutQuery);
            Wt::WWidget* widget = nullptr;
            if (cacheIt != widgetCache_.end()) {
                // Widget already exists in cache, just show it
                widget = cacheIt->second;
            } else {
                // Create new widget and add to cache
                auto newWidget = pattern.factory(match);
                widget = newWidget.get();
                widgetCache_[pathWithoutQuery] = widget;
                contentsStack_->addWidget(std::move(newWidget));
            }
            contentsStack_->setCurrentWidget(widget);
            markActive("");  // Don't highlight sidebar for dynamic paths
            return;
        }
    }
    
    // Path not found: render not-found page WITHOUT changing the URL
    *lastUnknownPath_ = path;
    auto notFoundIt = routes_.find("/not-found");
    if (notFoundIt != routes_.end()) {
        // Check cache first
        auto cacheIt = widgetCache_.find("/not-found");
        Wt::WWidget* widget = nullptr;
        if (cacheIt != widgetCache_.end()) {
            widget = cacheIt->second;
        } else {
            auto newWidget = notFoundIt->second();
            widget = newWidget.get();
            widgetCache_["/not-found"] = widget;
            contentsStack_->addWidget(std::move(newWidget));
        }
        contentsStack_->setCurrentWidget(widget);
        markActive("");  // Don't highlight any anchor for 404
    }
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

