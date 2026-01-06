#pragma once

#include "003_Navigation/NavigationTopic.h"
#include <memory>

class Session;
class BlogSession;

class BlogTopic : public NavigationTopic
{
public:
  explicit BlogTopic(std::shared_ptr<Session> session);
  std::unique_ptr<Wt::WWidget> createBlogPage();

private:
  std::unique_ptr<Wt::WWidget> blogPage();
  std::shared_ptr<Session> session_;
  std::shared_ptr<BlogSession> blogSession_;
};
