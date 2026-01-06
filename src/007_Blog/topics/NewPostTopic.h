#pragma once

#include "003_Navigation/NavigationTopic.h"
#include <memory>

class Session;
class BlogSession;

class NewPostTopic : public NavigationTopic
{
public:
  explicit NewPostTopic(std::shared_ptr<Session> session);
  std::unique_ptr<Wt::WWidget> createNewPostPage();

private:
  std::unique_ptr<Wt::WWidget> newPostPage();
  std::shared_ptr<Session> session_;
  std::shared_ptr<BlogSession> blogSession_;
};
