#include "007_Blog/BlogView.h"
#include "007_Blog/PostView.h"

#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Permission.h"
#include "005_Dbo/Tables/Tag.h"

#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WCheckBox.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>
#include <Wt/Dbo/Query.h>

#include <Wt/WApplication.h>

namespace dbo = Wt::Dbo;

BlogView::BlogView(std::shared_ptr<Session> session)
  : session_(std::move(session))
{  
  wApp->log("debug") << "BlogView::BlogView(std::shared_ptr<Session> session)";  
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
  wApp->log("debug") << "BlogView::renderHeader()";
  auto title = addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-3xl font-bold text-gray-800'>Blog</h2>"));
  title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

  if (currentUserIsBlogAdmin()) {
    auto newPostBtn = addNew<Wt::WPushButton>("New Post");
    newPostBtn->addStyleClass("bg-slate-700 hover:bg-slate-800 text-white rounded-md px-3 py-1");
    newPostBtn->clicked().connect([](){
      Wt::WApplication::instance()->setInternalPath("/blog/new", true);
    });
  }

  renderFilter();
}

void BlogView::renderPostsList()
{
  wApp->log("debug") << "BlogView::renderPostsList()";
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
  
  if (selectedTagSlugs_.empty()) {
    // No tag filter
    if (effectiveShowAll) {
      postsQuery = session_->find<Post>("order by " + orderColumn + " desc");
    } else {
      postsQuery = session_->find<Post>(
                      "where state = ? order by " + orderColumn + " desc")
                    .bind(effectiveFilter);
    }
  } else {
    // Filter by multiple tags (ANY tag match)
    std::string tagCondition;
    for (size_t i = 0; i < selectedTagSlugs_.size(); ++i) {
      if (i > 0) tagCondition += " or ";
      tagCondition += "t.slug = ?";
    }

    std::string sql =
      "select distinct p from post p "
      "join posts_tags pt on pt.post_id = p.id "
      "join tag t on t.id = pt.tag_id "
      "where (" + tagCondition + ")";
    
    if (!effectiveShowAll) {
      sql += " and p.state = ?";
    }
    
    sql += " order by p." + orderColumn + " desc";

    auto query = session_->query<dbo::ptr<Post>>(sql);
    
    // Bind all tag slugs
    for (const auto& slug : selectedTagSlugs_) {
      query.bind(slug);
    }
    
    // Bind state if filtering by state
    if (!effectiveShowAll) {
      query.bind(effectiveFilter);
    }
    
    postsQuery = query;
  }

  auto posts = postsQuery.resultList();
  for (const dbo::ptr<Post>& post : posts) {
    postsContainer_->addNew<PostView>(*session_, post, PostView::RenderType::Brief);
  }
}

bool BlogView::currentUserIsBlogAdmin()
{
  wApp->log("debug") << "BlogView::currentUserIsBlogAdmin()";
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
  filterBar->addStyleClass("flex flex-col gap-4");

  const bool isAdmin = currentUserIsBlogAdmin();

  // State filter: only visible to admin
  if (isAdmin) {
    auto stateContainer = filterBar->addNew<Wt::WContainerWidget>();
    stateContainer->addStyleClass("flex items-center gap-3");

    auto stateLabel = stateContainer->addNew<Wt::WText>("State:");
    stateLabel->addStyleClass("text-sm text-gray-600");

    filterCombo_ = stateContainer->addNew<Wt::WComboBox>();
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
  auto tagSection = filterBar->addNew<Wt::WContainerWidget>();
  tagSection->addStyleClass("space-y-2");

  auto tagLabel = tagSection->addNew<Wt::WText>("Tags:");
  tagLabel->addStyleClass("text-sm text-gray-600 block");

  tagFilterContainer_ = tagSection->addNew<Wt::WContainerWidget>();
  tagFilterContainer_->addStyleClass("flex flex-wrap gap-2");

  allTagSlugs_.clear();
  tagCheckboxes_.clear();

  {
    dbo::Transaction t(*session_);
    auto tags = session_->find<Tag>("order by name asc").resultList();
    for (const dbo::ptr<Tag>& tag : tags) {
      auto checkbox = tagFilterContainer_->addNew<Wt::WCheckBox>(tag->name_);
      checkbox->addStyleClass("text-sm text-gray-700");
      
      allTagSlugs_.push_back(tag->slug_);
      tagCheckboxes_.push_back(checkbox);
      
      // Check if this tag is already selected
      auto it = std::find(selectedTagSlugs_.begin(), selectedTagSlugs_.end(), tag->slug_);
      if (it != selectedTagSlugs_.end()) {
        checkbox->setChecked(true);
      }
      
      checkbox->changed().connect([this, slug = tag->slug_]() {
        // Update selectedTagSlugs_ based on checkbox states
        selectedTagSlugs_.clear();
        for (size_t i = 0; i < tagCheckboxes_.size(); ++i) {
          if (tagCheckboxes_[i]->isChecked()) {
            selectedTagSlugs_.push_back(allTagSlugs_[i]);
          }
        }
        updateUrlWithFilters();
        renderPostsList();
      });
    }
  }
}

void BlogView::initializeFiltersFromUrl()
{
  wApp->log("debug") << "BlogView::initializeFiltersFromUrl()";
  auto app = Wt::WApplication::instance();
  std::string internalPath = app->internalPath();
  
  // Parse query parameters from internal path
  std::string stateValue;
  std::vector<std::string> tagValues;
  
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
          tagValues.push_back(value);
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
  selectedTagSlugs_ = tagValues; // Empty if no tags
}

void BlogView::updateUrlWithFilters()
{
  wApp->log("debug") << "BlogView::updateUrlWithFilters()";
  auto app = Wt::WApplication::instance();
  std::string path = "/blog";
  
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
  
  // Add tag parameters (support multiple tags)
  for (const auto& tagSlug : selectedTagSlugs_) {
    params.push_back("tag=" + tagSlug);
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
