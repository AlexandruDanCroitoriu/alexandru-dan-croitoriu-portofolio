#include "003_Navigation/topics/BlogTopic.h"
#include "003_Navigation/DeferredWidget.h"

#include "007_Blog/BlogView.h"
#include "005_Dbo/Session.h"

#include <Wt/WContainerWidget.h>

BlogTopic::BlogTopic(std::shared_ptr<Session> session)
  : session_(std::move(session))
{
}

std::unique_ptr<Wt::WWidget> BlogTopic::createBlogPage()
{
  return deferCreate([this]() { return blogPage(); });
}

std::unique_ptr<Wt::WWidget> BlogTopic::blogPage()
{
  auto container = std::make_unique<Wt::WContainerWidget>();
  container->addStyleClass("w-full");
  container->addNew<BlogView>(session_);
  return container;
}
