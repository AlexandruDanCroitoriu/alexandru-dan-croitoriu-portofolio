#include "006_Auth/topics/UserSettingsTopic.h"
#include "003_Navigation/DeferredWidget.h"
#include "005_Dbo/Session.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WMenu.h>
#include <Wt/WApplication.h>

UserSettingsTopic::UserSettingsTopic(std::shared_ptr<Session> session)
    : session_(session)
{
    wApp->log("debug") << "UserSettingsTopic::UserSettingsTopic()";
}

std::unique_ptr<Wt::WWidget> UserSettingsTopic::createSettingsPage()
{
    wApp->log("debug") << "UserSettingsTopic::createSettingsPage()";
    return deferCreate([this]() { return settingsPage(); });
}

std::unique_ptr<Wt::WWidget> UserSettingsTopic::settingsPage()
{
    wApp->log("debug") << "UserSettingsTopic::settingsPage()";
    auto container = std::make_unique<Wt::WContainerWidget>();
    
    auto title = container->addNew<Wt::WText>("<h1 class='text-3xl font-bold mb-6 text-gray-900'>User Settings</h1>");
    title->setTextFormat(Wt::TextFormat::XHTML);
    
    auto settingsCard = container->addNew<Wt::WContainerWidget>();
    settingsCard->addStyleClass("bg-white rounded-lg shadow-md p-6 max-w-2xl");
    
    auto settingsText = settingsCard->addNew<Wt::WText>(
        "<div class='space-y-4'>"
        "<div>"
        "  <h2 class='text-xl font-semibold text-gray-800 mb-2'>Account</h2>"
        "  <p class='text-gray-600'>Manage your account settings and preferences</p>"
        "</div>"
        "<div class='border-t pt-4 mt-4'>"
        "  <h2 class='text-xl font-semibold text-gray-800 mb-2'>Notifications</h2>"
        "  <p class='text-gray-600'>Configure your notification preferences</p>"
        "</div>"
        "<div class='border-t pt-4 mt-4'>"
        "  <h2 class='text-xl font-semibold text-gray-800 mb-2'>Privacy</h2>"
        "  <p class='text-gray-600'>Manage your privacy and data settings</p>"
        "</div>"
        "</div>"
    );
    settingsText->setTextFormat(Wt::TextFormat::XHTML);
    
    return container;
}
