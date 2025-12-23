#pragma once

#include <Wt/WContainerWidget.h>

#include <memory>

class Session;

class PostDetailTopic
{
public:
  explicit PostDetailTopic(std::shared_ptr<Session> session, const std::string& slug);
  std::unique_ptr<Wt::WWidget> createPostDetailPage();

private:
  std::shared_ptr<Session> session_;
  std::string slug_;
};
