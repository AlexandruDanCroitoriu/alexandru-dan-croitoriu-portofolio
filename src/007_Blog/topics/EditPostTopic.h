#pragma once

#include <Wt/WContainerWidget.h>
#include <memory>
#include <string>

class Session;
class BlogSession;

class EditPostTopic
{
public:
  EditPostTopic(std::shared_ptr<Session> session, const std::string& slug);
  std::unique_ptr<Wt::WWidget> createEditPostPage();

private:
  std::unique_ptr<Wt::WWidget> editPage();
  std::shared_ptr<Session> session_;
  std::shared_ptr<BlogSession> blogSession_;
  std::string slug_;
};
