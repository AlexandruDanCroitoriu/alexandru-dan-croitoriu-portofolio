#include "007_Blog/BlogView.h"
#include "007_Blog/PostView.h"

#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Permission.h"
#include "005_Dbo/Tables/Tag.h"

#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>
#include <Wt/Dbo/Query.h>

#include <Wt/WApplication.h>

namespace dbo = Wt::Dbo;

BlogView::BlogView(std::shared_ptr<Session> session)
  : session_(std::move(session))
{
  addStyleClass("w-full max-w-4xl mx-auto space-y-4 p-6");
  
  // Initialize filters from URL query parameters first
  initializeFiltersFromUrl();
  
  renderHeader();
  postsContainer_ = addNew<Wt::WContainerWidget>();
  postsContainer_->addStyleClass("space-y-4");
  renderPostsList();
}

void BlogView::renderHeader()
{
  auto title = addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-3xl font-bold text-gray-800'>Blog</h2>"));
  title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

  if (currentUserIsBlogAdmin()) {
    auto newPostBtn = addNew<Wt::WPushButton>("New Post");
    newPostBtn->addStyleClass("bg-slate-700 hover:bg-slate-800 text-white rounded-md px-3 py-1");
    newPostBtn->clicked().connect([](){
      Wt::WApplication::instance()->setInternalPath("/portfolio/blog/new", true);
    });
  }

  renderFilter();
}

void BlogView::renderPostsList()
{
  dbo::Transaction t(*session_);
  postsContainer_->clear();

  // Security: Non-admin users can only see Published posts
  const bool isAdmin = currentUserIsBlogAdmin();
  Post::State effectiveFilter = currentFilter_;
  bool effectiveShowAll = showAllStates_;
  
  if (!isAdmin) {
    effectiveFilter = Post::State::Published;
    effectiveShowAll = false;
  }

  // Determine ordering column based on state
  std::string orderColumn = (effectiveFilter == Post::State::Published) ? "published_at" : "created_at";

  Wt::Dbo::Query<dbo::ptr<Post>> postsQuery;
  if (effectiveShowAll) {
    // Show all states
    if (currentTagSlug_.empty()) {
      postsQuery = session_->find<Post>("order by " + orderColumn + " desc");
    } else {
      std::string sql =
        "select p from post p "
        "join posts_tags pt on pt.post_id = p.id "
        "join tag t on t.id = pt.tag_id "
        "where t.slug = ? "
        "order by p." + orderColumn + " desc";
      postsQuery = session_->query<dbo::ptr<Post>>(sql)
                      .bind(currentTagSlug_);
    }
  } else {
    // Filter by specific state
    if (currentTagSlug_.empty()) {
      postsQuery = session_->find<Post>(
                      "where state = ? order by " + orderColumn + " desc")
                    .bind(effectiveFilter);
    } else {
      std::string sql =
        "select p from post p "
        "join posts_tags pt on pt.post_id = p.id "
        "join tag t on t.id = pt.tag_id "
        "where p.state = ? and t.slug = ? "
        "order by p." + orderColumn + " desc";
      postsQuery = session_->query<dbo::ptr<Post>>(sql)
                      .bind(effectiveFilter)
                      .bind(currentTagSlug_);
    }
  }

  auto posts = postsQuery.resultList();
  for (const dbo::ptr<Post>& post : posts) {
    postsContainer_->addNew<PostView>(*session_, post, PostView::RenderType::Brief);
  }
}

bool BlogView::currentUserIsBlogAdmin()
{
  if (!session_->login().loggedIn()) return false;
  dbo::Transaction t(*session_);
  auto user = session_->user();
  if (!user) return false;
  auto blogAdminQuery = session_->find<Permission>().where("name = ?").bind("BLOG_ADMIN");
  auto perms = blogAdminQuery.resultList();
  if (perms.empty()) return false;
  auto blogAdmin = perms.front();
  return user->hasPermission(blogAdmin);
}

void BlogView::renderFilter()
{
  // Filter bar
  auto filterBar = addNew<Wt::WContainerWidget>();
  filterBar->addStyleClass("flex items-center gap-3");

  const bool isAdmin = currentUserIsBlogAdmin();

  // State filter: only visible to admin
  if (isAdmin) {
    auto stateLabel = filterBar->addNew<Wt::WText>("State:");
    stateLabel->addStyleClass("text-sm text-gray-600");

    filterCombo_ = filterBar->addNew<Wt::WComboBox>();
    filterCombo_->addStyleClass("bg-gray-50/10 ring-1 ring-gray-50/5 rounded px-2 py-1 text-sm");

    filterCombo_->addItem("All States");
    filterCombo_->addItem("Active");
    filterCombo_->addItem("Draft");
    filterCombo_->addItem("Archived");
    
    // Set current index based on initialized filter state
    int stateIdx = 1;  // Default to Active
    if (showAllStates_) stateIdx = 0;
    else if (currentFilter_ == Post::State::Draft) stateIdx = 2;
    else if (currentFilter_ == Post::State::Archived) stateIdx = 3;
    filterCombo_->setCurrentIndex(stateIdx);

    filterCombo_->changed().connect([this]() {
      const auto idx = filterCombo_->currentIndex();
      if (idx == 0) {
        showAllStates_ = true;
        currentFilter_ = Post::State::Published; // Default, but ignored when showAllStates_ is true
      } else if (idx == 1) {
        showAllStates_ = false;
        currentFilter_ = Post::State::Published; // Active
      } else if (idx == 2) {
        showAllStates_ = false;
        currentFilter_ = Post::State::Draft;
      } else if (idx == 3) {
        showAllStates_ = false;
        currentFilter_ = Post::State::Archived;
      } else {
        showAllStates_ = false;
        currentFilter_ = Post::State::Published; // Fallback
      }
      updateUrlWithFilters();
      renderPostsList();
    });
  } else {
    // Non-admins always see Active
    currentFilter_ = Post::State::Published;
  }

  // Tag filter: visible to all
  auto tagLabel = filterBar->addNew<Wt::WText>("Tag:");
  tagLabel->addStyleClass("text-sm text-gray-600");

  tagCombo_ = filterBar->addNew<Wt::WComboBox>();
  tagCombo_->addStyleClass("bg-gray-50/10 ring-1 ring-gray-50/5 rounded px-2 py-1 text-sm");

  tagCombo_->addItem("All Tags");
  tagSlugs_.clear();

  {
    dbo::Transaction t(*session_);
    auto tags = session_->find<Tag>("order by name asc").resultList();
    for (const dbo::ptr<Tag>& tag : tags) {
      tagCombo_->addItem(tag->name_);
      tagSlugs_.push_back(tag->slug_);
    }
  }

  // Set current index based on initialized tag filter
  int tagIdx = 0;
  if (!currentTagSlug_.empty()) {
    auto it = std::find(tagSlugs_.begin(), tagSlugs_.end(), currentTagSlug_);
    if (it != tagSlugs_.end()) {
      tagIdx = std::distance(tagSlugs_.begin(), it) + 1; // +1 for "All Tags"
    }
  }
  tagCombo_->setCurrentIndex(tagIdx);

  tagCombo_->changed().connect([this]() {
    const int idx = tagCombo_->currentIndex();
    if (idx <= 0) {
      currentTagSlug_.clear();
    } else {
      // idx - 1 into tagSlugs_
      if (static_cast<size_t>(idx - 1) < tagSlugs_.size())
        currentTagSlug_ = tagSlugs_[idx - 1];
    }
    updateUrlWithFilters();
    renderPostsList();
  });
}

void BlogView::initializeFiltersFromUrl()
{
  auto app = Wt::WApplication::instance();
  std::string internalPath = app->internalPath();
  
  // Parse query parameters from internal path
  std::string stateValue, tagValue;
  size_t queryPos = internalPath.find('?');
  if (queryPos != std::string::npos) {
    std::string queryString = internalPath.substr(queryPos + 1);
    
    // Parse key=value pairs separated by &
    size_t pos = 0;
    while (pos < queryString.length()) {
      size_t ampPos = queryString.find('&', pos);
      std::string param = (ampPos != std::string::npos) 
                          ? queryString.substr(pos, ampPos - pos)
                          : queryString.substr(pos);
      
      size_t eqPos = param.find('=');
      if (eqPos != std::string::npos) {
        std::string key = param.substr(0, eqPos);
        std::string value = param.substr(eqPos + 1);
        
        if (key == "state") {
          stateValue = value;
        } else if (key == "tag") {
          tagValue = value;
        }
      }
      
      if (ampPos == std::string::npos) break;
      pos = ampPos + 1;
    }
  }
  
  // Apply state filter (only for admin users)
  const bool isAdmin = currentUserIsBlogAdmin();
  
  if (!stateValue.empty() && isAdmin) {
    if (stateValue == "all") {
      showAllStates_ = true;
      currentFilter_ = Post::State::Published; // Default, but ignored
    } else if (stateValue == "draft") {
      showAllStates_ = false;
      currentFilter_ = Post::State::Draft;
    } else if (stateValue == "archived") {
      showAllStates_ = false;
      currentFilter_ = Post::State::Archived;
    } else {
      showAllStates_ = false;
      currentFilter_ = Post::State::Published; // "active" or default
    }
  } else {
    // Non-admins or no state param: always show only Published
    showAllStates_ = false;
    currentFilter_ = Post::State::Published;
  }
  
  // Apply tag filter
  currentTagSlug_ = tagValue; // Empty string if not found
}

void BlogView::updateUrlWithFilters()
{
  auto app = Wt::WApplication::instance();
  std::string path = "/portfolio/blog";
  
  // Build query string
  std::vector<std::string> params;
  
  // Add state parameter (only if admin and not default)
  if (currentUserIsBlogAdmin()) {
    if (showAllStates_) {
      params.push_back("state=all");
    } else if (currentFilter_ == Post::State::Draft) {
      params.push_back("state=draft");
    } else if (currentFilter_ == Post::State::Archived) {
      params.push_back("state=archived");
    } else if (currentFilter_ == Post::State::Published) {
      params.push_back("state=active");
    }
  }
  
  // Add tag parameter
  if (!currentTagSlug_.empty()) {
    params.push_back("tag=" + currentTagSlug_);
  }
  
  // Construct final path with query string
  if (!params.empty()) {
    path += "?";
    for (size_t i = 0; i < params.size(); ++i) {
      if (i > 0) path += "&";
      path += params[i];
    }
  }
  
  app->setInternalPath(path, false); // false = don't emit pathChanged signal
}
