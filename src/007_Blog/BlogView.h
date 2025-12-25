#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>

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
  // State filter (admin only) - now using checkboxes
  Wt::WContainerWidget* stateFilterContainer_ {nullptr};
  std::vector<Post::State> selectedStates_;
  std::vector<Wt::WCheckBox*> stateCheckboxes_;
  // Tag filter (visible to all)
  Wt::WContainerWidget* tagFilterContainer_ {nullptr};
  std::vector<std::string> allTagSlugs_;
  std::vector<std::string> selectedTagSlugs_;
  std::vector<Wt::WCheckBox*> tagCheckboxes_;

  void renderHeader();
  void renderFilter();
  void renderPostsList();
  bool currentUserIsBlogAdmin();
  void initializeFiltersFromUrl();
  void updateUrlWithFilters();
};
