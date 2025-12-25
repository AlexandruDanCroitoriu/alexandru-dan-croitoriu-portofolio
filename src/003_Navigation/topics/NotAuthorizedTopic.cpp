#include "003_Navigation/topics/NotAuthorizedTopic.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>
#include <Wt/WApplication.h>

NotAuthorizedTopic::NotAuthorizedTopic()
{
    wApp->log("debug") << "NotAuthorizedTopic::NotAuthorizedTopic()";
}

std::unique_ptr<Wt::WWidget> NotAuthorizedTopic::createNotAuthorizedPage()
{
    wApp->log("debug") << "NotAuthorizedTopic::createNotAuthorizedPage()";
    return notAuthorizedPage();
}

std::unique_ptr<Wt::WWidget> NotAuthorizedTopic::notAuthorizedPage()
{
    wApp->log("debug") << "NotAuthorizedTopic::notAuthorizedPage()";
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("w-full max-w-3xl mx-auto p-8");

    auto card = container->addNew<Wt::WContainerWidget>();
    card->addStyleClass("bg-white rounded-lg shadow-md p-8 text-center");

    auto title = card->addNew<Wt::WText>("<h1 class='text-3xl font-bold text-gray-900 mb-2'>Not Authorized</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    auto desc = card->addNew<Wt::WText>(
        "<p class='text-gray-600 mb-6'>You don't have permission to access this page. Please log in to continue.</p>");
    desc->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    auto back = card->addNew<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, "/"), "← Go back home");
    back->addStyleClass("inline-block px-4 py-2 bg-slate-700 hover:bg-slate-800 text-white rounded-md");

    return container;
}
