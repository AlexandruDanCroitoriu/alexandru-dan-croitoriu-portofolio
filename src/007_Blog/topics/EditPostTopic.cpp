#include "007_Blog/topics/EditPostTopic.h"
#include "007_Blog/BlogUtils.h"
#include "003_Navigation/DeferredWidget.h"
#include "002_Components/MonacoEditor.h"

#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Tag.h"
#include "005_Dbo/Tables/Permission.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>

#include <cctype>
#include <algorithm>

namespace dbo = Wt::Dbo;

static std::string slugify(const std::string &value)
{
  wApp->log("debug") << "slugify(const std::string& value)";
  std::string slug = value;
  for (char &c : slug)
  {
    const unsigned char uc = static_cast<unsigned char>(c);
    if (std::isspace(uc))
      c = '-';
    else
      c = static_cast<char>(std::tolower(uc));
  }
  return slug;
}

EditPostTopic::EditPostTopic(std::shared_ptr<Session> session, const std::string &slug)
    : session_(std::move(session)), slug_(slug)
{
  wApp->log("debug") << "EditPostTopic::EditPostTopic(std::shared_ptr<Session> session, const std::string& slug)";
}

std::unique_ptr<Wt::WWidget> EditPostTopic::createEditPostPage()
{
  wApp->log("debug") << "EditPostTopic::createEditPostPage()";
  // Capture session and slug by value to avoid dangling references in deferred execution
  auto session = session_;
  auto slug = slug_;
  return deferCreateTopicPage([session, slug]()
                     {
    EditPostTopic topic(session, slug);
    return topic.editPage(); });
}

std::unique_ptr<Wt::WWidget> EditPostTopic::editPage()
{
  wApp->log("debug") << "EditPostTopic::editPage()";
  auto container = std::make_unique<Wt::WContainerWidget>();
  container->addStyleClass("w-full max-w-4xl mx-auto space-y-4 p-6");

  // Access guard
  if (!isBlogAdmin(*session_))
  {
    auto title = container->addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-2xl font-bold text-red-700'>Not Authorized</h2>"));
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    container->addNew<Wt::WText>("Only the admin can edit posts.");
    return container;
  }

  // Load post data and copy values we need (transaction scope is critical)
  std::string postTitle;
  std::string postBrief;
  std::string postBody;
  int stateIdx = 0;
  std::vector<std::string> prechecked;

  {
    dbo::Transaction t(*session_);
    auto posts = session_->find<Post>("where slug = ?").bind(slug_).resultList();
    if (posts.empty())
    {
      auto title = container->addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-2xl font-bold text-red-700'>Post not found</h2>"));
      title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
      return container;
    }
    auto post = posts.front();

    // Copy all data we need while transaction is active
    postTitle = post->title_;
    postBrief = post->briefXml_;
    postBody = post->bodyXml_;

    if (post->state_ == Post::State::Published)
      stateIdx = 1;
    else if (post->state_ == Post::State::Archived)
      stateIdx = 2;

    // Collect current post tags for pre-check
    for (const auto &tag : post->tags_)
    {
      prechecked.push_back(tag->slug_);
    }
  } // Transaction ends here, post pointer is no longer valid

  auto titleWrapper = container->addNew<Wt::WContainerWidget>();
  titleWrapper->setStyleClass("flex items-center justify-between flex-wrap");

  auto title = titleWrapper->addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-3xl font-bold text-gray-800'>Edit Post</h2>"));
  title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

  auto saveBtn = titleWrapper->addNew<Wt::WPushButton>("Save");

  auto titleEdit = container->addNew<Wt::WLineEdit>(postTitle);
  titleEdit->addStyleClass("w-full rounded-md border border-gray-300 p-2");

  
  // State selection
  auto stateGroup = std::make_shared<Wt::WButtonGroup>();
  auto stateContainer = container->addNew<Wt::WContainerWidget>();
  stateContainer->addStyleClass("space-x-2 mt-2 flex items-center");

  auto stateLabel = stateContainer->addNew<Wt::WText>("State:");
  stateLabel->addStyleClass("text-sm font-semibold text-gray-700 mt-2");

  auto draftRadio = stateContainer->addNew<Wt::WRadioButton>("Draft");
  stateGroup->addButton(draftRadio, 0);
  auto publishedRadio = stateContainer->addNew<Wt::WRadioButton>("Published");
  stateGroup->addButton(publishedRadio, 1);
  auto archivedRadio = stateContainer->addNew<Wt::WRadioButton>("Archived");
  stateGroup->addButton(archivedRadio, 2);

  std::string radioStyles = "";
  radioStyles += "[&>input]:hidden text-white text-sm lg:text-md inline-block cursor-pointer ";
  radioStyles += "[&>input]:[&~span]:p-1 ";
  radioStyles += "[&>input]:[&~span]:rounded-md ";
  radioStyles += "[&>input]:[&~span]:bg-gray-400 ";
  radioStyles += "[&>input]:checked:[&~span]:bg-gray-700 ";

  draftRadio->addStyleClass(radioStyles);
  publishedRadio->addStyleClass(radioStyles);
  archivedRadio->addStyleClass(radioStyles);

  stateGroup->setCheckedButton(stateGroup->button(stateIdx));

  // Tag selection (existing tags)
  auto tagContainerWrapper = container->addNew<Wt::WContainerWidget>();

  auto tagContainer = container->addNew<Wt::WContainerWidget>();
  tagContainer->addStyleClass("space-x-1 px-2 rounded-md");
  
  auto tagCheckboxes = std::make_shared<std::vector<Wt::WCheckBox *>>();
  auto tagSlugs = std::make_shared<std::vector<std::string>>();

  auto refreshTags = [this, radioStyles, tagContainer, tagCheckboxes, tagSlugs](const std::vector<std::string> &pre)
  {
    tagContainer->clear();
    tagCheckboxes->clear();
    tagSlugs->clear();

    dbo::Transaction t(*session_);
    auto tags = session_->find<Tag>("order by name asc").resultList();
    for (const dbo::ptr<Tag> &tag : tags)
    {
      auto cb = tagContainer->addNew<Wt::WCheckBox>(tag->name_);
      cb->addStyleClass(radioStyles);
      const bool checked = std::find(pre.begin(), pre.end(), tag->slug_) != pre.end();
      cb->setChecked(checked);
      tagCheckboxes->push_back(cb);
      tagSlugs->push_back(tag->slug_);
    }
  };

  refreshTags(prechecked);

  // New tag input
  auto newTagLabel = tagContainerWrapper->addNew<Wt::WText>("Add new tag:");
  newTagLabel->addStyleClass("text-sm font-semibold text-gray-700 mt-4");
  auto newTagEdit = tagContainerWrapper->addNew<Wt::WLineEdit>();
  newTagEdit->setPlaceholderText("Type tag name and press Enter");
  newTagEdit->addStyleClass("w-full rounded-md border border-gray-300 p-2");
  newTagEdit->enterPressed().connect([this, newTagEdit, refreshTags, tagCheckboxes, tagSlugs]()
                                     {
    std::string tagName = newTagEdit->text().toUTF8();
    tagName.erase(0, tagName.find_first_not_of(" \t\n\r"));
    if (!tagName.empty())
      tagName.erase(tagName.find_last_not_of(" \t\n\r") + 1);

    if (tagName.empty()) return;

    std::string tagSlug = slugify(tagName);

    // Capture selected tags
    std::vector<std::string> selected;
    for (size_t i = 0; i < tagCheckboxes->size() && i < tagSlugs->size(); ++i) {
      if ((*tagCheckboxes)[i]->isChecked()) {
        selected.push_back((*tagSlugs)[i]);
      }
    }
    selected.push_back(tagSlug);

    {
      dbo::Transaction t(*session_);
      auto existingTags = session_->find<Tag>("where slug = ?").bind(tagSlug).resultList();
      if (existingTags.empty()) {
        auto newTag = session_->add(std::make_unique<Tag>());
        auto tm = newTag.modify();
        tm->name_ = tagName;
        tm->slug_ = tagSlug;
      }
      t.commit();
    }

    refreshTags(selected);
    newTagEdit->setText(""); });

  saveBtn->addStyleClass("bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded");

  auto status = container->addNew<Wt::WText>("");
  status->addStyleClass("mt-2");

  auto briefEdit = container->addNew<MonacoEditor>("xml");
  briefEdit->addStyleClass("w-full rounded-md border border-gray-300");
  briefEdit->setHeight(Wt::WLength(200, Wt::LengthUnit::Pixel));
  briefEdit->setContent(postBrief);

  auto bodyEdit = container->addNew<MonacoEditor>("xml");
  bodyEdit->addStyleClass("w-full rounded-md border border-gray-300");
  bodyEdit->setHeight(Wt::WLength(500, Wt::LengthUnit::Pixel));
  bodyEdit->setContent(postBody);

  // Capture session as shared_ptr and slug as value to avoid dangling references
  auto sessionPtr = session_;
  auto postSlug = slug_;

  saveBtn->clicked().connect([sessionPtr, postSlug, titleEdit, briefEdit, bodyEdit, stateGroup, tagCheckboxes, tagSlugs, status]()
                             {
    if (!sessionPtr || !titleEdit || !briefEdit || !bodyEdit || !stateGroup || !tagCheckboxes || !tagSlugs || !status) {
      if (status) status->setText("Widget error");
      return;
    }
    
    if (!isBlogAdmin(*sessionPtr)) {
      status->setText("Not authorized.");
      return;
    }

    std::string postTitle = titleEdit->text().toUTF8();
    if (postTitle.empty()) {
      status->setText("Title is required.");
      return;
    }

    try {
      std::string redirectSlug;
      {
        dbo::Transaction t(*sessionPtr);
        
        auto posts = sessionPtr->find<Post>("where slug = ?").bind(postSlug).resultList();
        if (posts.empty()) {
          status->setText("Post not found.");
          return;
        }

        // Nested scope forces dbo::ptr to destruct before commit
        {
          auto p = posts.front().modify();
          p->title_ = postTitle;
          p->briefXml_ = briefEdit->getUnsavedText();
          p->bodyXml_ = bodyEdit->getUnsavedText();

          // Recompute slug from title to keep URL in sync
          redirectSlug = slugify(postTitle);
          p->slug_ = redirectSlug;

          int stateIdx = stateGroup->checkedId();
          if (stateIdx == 1) {
            p->state_ = Post::State::Published;
            p->publishedAt_ = Wt::WDateTime::currentDateTime();
          } else if (stateIdx == 2) {
            p->state_ = Post::State::Archived;
          } else {
            p->state_ = Post::State::Draft;
          }

          p->updatedAt_ = Wt::WDateTime::currentDateTime();

          // Update tags: clear then re-add selected
          p->tags_.clear();
          
          // Build list of selected tag slugs first
          std::vector<std::string> selectedTagSlugs;
          for (size_t i = 0; i < tagCheckboxes->size() && i < tagSlugs->size(); ++i) {
            Wt::WCheckBox* cb = (*tagCheckboxes)[i];
            if (cb && cb->isChecked()) {
              selectedTagSlugs.push_back((*tagSlugs)[i]);
            }
          }
          
          // Add tags from the collected slugs
          for (const auto& slug : selectedTagSlugs) {
            auto tags = sessionPtr->find<Tag>("where slug = ?").bind(slug).resultList();
            if (!tags.empty()) {
              p->tags_.insert(tags.front());
            }
          }
        }  // dbo::ptr<Post> p destroyed here

        t.commit();
      }

      // Redirect back to post detail
      if (!redirectSlug.empty()) {
        Wt::WApplication::instance()->setInternalPath("/blog/post/" + redirectSlug, true);
      }
    } catch (const std::exception& ex) {
      status->setText(std::string("Error: ") + ex.what());
    } catch (...) {
      status->setText("Unknown error saving post");
    } });

  return container;
}
