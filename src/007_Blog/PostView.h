#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WString.h>
#include <Wt/Dbo/ptr.h>

#include <string>

class Session;
class Post;

class PostView : public Wt::WContainerWidget
{
public:
  enum class RenderType { Brief, Detail };

  PostView(Session& session, const Wt::Dbo::ptr<Post>& post, RenderType type);
};
