#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>

#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Tag.h"

namespace Wt { namespace Dbo { template <class C> class ptr; } }

class Session;
class Post;
class Permission;

class BlogView : public Wt::WContainerWidget
{
public:
  explicit BlogView(std::shared_ptr<Session> session);

private:
  std::shared_ptr<Session> session_;
  Wt::WContainerWidget* postsContainer_ {nullptr};
  // State filter (admin only)
  Wt::WComboBox* filterCombo_ {nullptr};
  Post::State currentFilter_ = Post::State::Published; // Active = Published
  bool showAllStates_ = false;
  // Tag filter (visible to all)
  Wt::WComboBox* tagCombo_ {nullptr};
  std::vector<std::string> tagSlugs_;
  std::string currentTagSlug_;

  void renderHeader();
  void renderFilter();
  void renderPostsList();
  bool currentUserIsBlogAdmin();
  void initializeFiltersFromUrl();
  void updateUrlWithFilters();
};
