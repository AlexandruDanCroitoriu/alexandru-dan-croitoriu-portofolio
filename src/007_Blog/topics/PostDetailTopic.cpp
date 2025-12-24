#include "007_Blog/topics/PostDetailTopic.h"
#include "007_Blog/topics/EditPostTopic.h"

#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Permission.h"
#include "007_Blog/PostView.h"

#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>

namespace dbo = Wt::Dbo;

static bool isBlogAdmin(Session& session)
{
  if (!session.login().loggedIn()) return false;
  dbo::Transaction t(session);
  auto user = session.user();
  if (!user) return false;
  auto perms = session.find<Permission>().where("name = ?").bind("BLOG_ADMIN").resultList();
  if (perms.empty()) return false;
  return user->hasPermission(perms.front());
}

PostDetailTopic::PostDetailTopic(std::shared_ptr<Session> session, const std::string& slug)
  : session_(std::move(session)), slug_(slug)
{
}

std::unique_ptr<Wt::WWidget> PostDetailTopic::createPostDetailPage()
{
  auto container = std::make_unique<Wt::WContainerWidget>();
  container->addStyleClass("w-full max-w-4xl mx-auto p-6");

  dbo::Transaction t(*session_);
  auto postQuery = session_->find<Post>("where slug = ?").bind(slug_);
  auto posts = postQuery.resultList();

  if (posts.empty()) {
    auto notFound = container->addNew<Wt::WText>("<h2 class='text-2xl font-bold text-gray-800'>Post not found</h2>");
    notFound->setTextFormat(Wt::TextFormat::UnsafeXHTML);
  } else {
    auto post = posts.front();
    // Admin edit button
    if (isBlogAdmin(*session_)) {
      auto editBtn = container->addNew<Wt::WPushButton>("Edit Post");
      editBtn->addStyleClass("bg-slate-700 hover:bg-slate-800 text-white rounded-md px-3 py-1 mb-2");
      editBtn->clicked().connect([slug = post->slug_]() {
        Wt::WApplication::instance()->setInternalPath("/portfolio/blog/post/" + slug + "/edit", true);
      });
    }
    container->addNew<PostView>(*session_, post, PostView::RenderType::Detail);
  }

  return container;
}
