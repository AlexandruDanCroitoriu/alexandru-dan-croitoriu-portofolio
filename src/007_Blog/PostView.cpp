#include "007_Blog/PostView.h"
#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Tag.h"

#include <Wt/WText.h>
#include <Wt/WAnchor.h>
#include <Wt/WApplication.h>
#include <Wt/Dbo/Transaction.h>

PostView::PostView(Session& session, const Wt::Dbo::ptr<Post>& post, RenderType type)
{
  addStyleClass("bg-white rounded-lg shadow-md p-4 mb-4");

  // Header container with title on left and state/tags on right
  auto headerContainer = addNew<Wt::WContainerWidget>();
  headerContainer->addStyleClass("flex items-start justify-between gap-4 mb-3");

  // Title container (left side)
  auto titleContainer = headerContainer->addNew<Wt::WContainerWidget>();
  titleContainer->addStyleClass("flex-1 min-w-0");

  if (type == RenderType::Brief) {
    // Clickable title for brief view
    auto titleAnchor = titleContainer->addNew<Wt::WAnchor>();
    titleAnchor->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    titleAnchor->setText(Wt::WString::fromUTF8("<h3 class='text-xl font-semibold text-blue-600 hover:text-blue-800 cursor-pointer'>" + post->title_ + "</h3>"));
    titleAnchor->clicked().connect([slug = post->slug_]() {
      Wt::WApplication::instance()->setInternalPath("/blog/post/" + slug, true);
    });
  } else {
    // Non-clickable title for detail view
    auto title = titleContainer->addNew<Wt::WText>(Wt::WString::fromUTF8("<h3 class='text-3xl font-bold text-gray-800'>" + post->title_ + "</h3>"));
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
  }

  // Meta information: state and tags (right side)
  auto metaContainer = headerContainer->addNew<Wt::WContainerWidget>();
  metaContainer->addStyleClass("flex flex-wrap items-center gap-2 justify-end");

  // State badge
  std::string stateText, stateBgColor;
  if (post->state_ == Post::State::Published) {
    stateText = "Published";
    stateBgColor = "bg-green-100 text-green-800";
  } else if (post->state_ == Post::State::Draft) {
    stateText = "Draft";
    stateBgColor = "bg-yellow-100 text-yellow-800";
  } else if (post->state_ == Post::State::Archived) {
    stateText = "Archived";
    stateBgColor = "bg-gray-100 text-gray-800";
  }
  
  auto stateBadge = metaContainer->addNew<Wt::WText>(stateText);
  stateBadge->addStyleClass("px-2 py-1 rounded-full text-xs font-medium " + stateBgColor);

  // Tags
  Wt::Dbo::Transaction t(session);
  auto tags = post->tags_;
  for (const auto& tag : tags) {
    auto tagBadge = metaContainer->addNew<Wt::WText>(tag->name_);
    tagBadge->addStyleClass("px-2 py-1 rounded-full text-xs font-medium bg-blue-100 text-blue-800");
  }

  // Body content
  const std::string& html = (type == RenderType::Brief) ? post->briefHtml_ : post->bodyHtml_;
  auto body = addNew<Wt::WText>(Wt::WString::fromUTF8("<div class='prose max-w-none'>" + html + "</div>"));
  body->setTextFormat(Wt::TextFormat::UnsafeXHTML);
}
