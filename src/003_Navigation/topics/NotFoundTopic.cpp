#include "003_Navigation/topics/NotFoundTopic.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>
#include <Wt/WApplication.h>

NotFoundTopic::NotFoundTopic()
{
    wApp->log("debug") << "NotFoundTopic::NotFoundTopic()";
}

std::unique_ptr<Wt::WWidget> NotFoundTopic::createNotFoundPage(const std::string& attemptedPath)
{
    wApp->log("debug") << "NotFoundTopic::createNotFoundPage(const std::string& attemptedPath)";
    return notFoundPage(attemptedPath);
}

std::unique_ptr<Wt::WWidget> NotFoundTopic::notFoundPage(const std::string& attemptedPath)
{
    wApp->log("debug") << "NotFoundTopic::notFoundPage(const std::string& attemptedPath)";
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("w-full max-w-3xl mx-auto p-8");

    auto card = container->addNew<Wt::WContainerWidget>();
    card->addStyleClass("bg-white rounded-lg shadow-md p-8 text-center");

    auto title = card->addNew<Wt::WText>("<h1 class='text-3xl font-bold text-gray-900 mb-2'>Page Not Found</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    auto desc = card->addNew<Wt::WText>(
        "<p class='text-gray-600 mb-6'>The page you’re looking for doesn’t exist or may have moved.</p>");
    desc->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Show attempted path (escaped by default)
    auto pathInfo = card->addNew<Wt::WText>(Wt::WString("Requested path: {1}").arg(attemptedPath));
    pathInfo->addStyleClass("text-gray-500 mb-6");

    auto back = card->addNew<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, "/"), "← Go back home");
    back->addStyleClass("inline-block px-4 py-2 bg-slate-700 hover:bg-slate-800 text-white rounded-md");

    return container;
}
