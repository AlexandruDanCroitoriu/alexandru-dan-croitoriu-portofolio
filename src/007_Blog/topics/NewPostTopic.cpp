#include "007_Blog/topics/NewPostTopic.h"
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

#include <algorithm>

namespace dbo = Wt::Dbo;

NewPostTopic::NewPostTopic(std::shared_ptr<Session> session)
  : session_(std::move(session))
{
  wApp->log("debug") << "NewPostTopic::NewPostTopic(std::shared_ptr<Session> session)";
}

std::unique_ptr<Wt::WWidget> NewPostTopic::createNewPostPage()
{
  wApp->log("debug") << "NewPostTopic::createNewPostPage()";
  return deferCreate([this]() { return newPostPage(); });
}

std::unique_ptr<Wt::WWidget> NewPostTopic::newPostPage()
{
  wApp->log("debug") << "NewPostTopic::newPostPage()";
  auto container = std::make_unique<Wt::WContainerWidget>();
  container->addStyleClass("w-full max-w-4xl mx-auto space-y-4 p-6");

  // Access guard
  if (!isBlogAdmin(*session_)) {
    auto title = container->addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-2xl font-bold text-red-700'>Not Authorized</h2>"));
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    container->addNew<Wt::WText>("Only the admin can create new posts.");
    return container;
  }
  auto titleWrapper = container->addNew<Wt::WContainerWidget>();
  titleWrapper->setStyleClass("flex items-center justify-between flex-wrap");

  auto title = titleWrapper->addNew<Wt::WText>(Wt::WString::fromUTF8("<h2 class='text-3xl font-bold text-gray-800'>Create New Post</h2>"));
  title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

  auto createBtn = titleWrapper->addNew<Wt::WPushButton>("Create");


  auto titleEdit = container->addNew<Wt::WLineEdit>();
  titleEdit->setPlaceholderText("Post title");
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

  stateGroup->setCheckedButton(stateGroup->button(0)); // Default to Draft

  // Tag selection (existing tags)
  auto tagContainerWrapper = container->addNew<Wt::WContainerWidget>();

  auto tagContainer = container->addNew<Wt::WContainerWidget>();
  tagContainer->addStyleClass("space-x-1 px-2 rounded-md");

  auto tagCheckboxes = std::make_shared<std::vector<Wt::WCheckBox*>>();
  auto tagSlugs = std::make_shared<std::vector<std::string>>();

  auto refreshTags = [this, radioStyles, tagContainer, tagCheckboxes, tagSlugs](const std::vector<std::string>& prechecked) {
    tagContainer->clear();
    tagCheckboxes->clear();
    tagSlugs->clear();

    dbo::Transaction t(*session_);
    auto tags = session_->find<Tag>("order by name asc").resultList();
    for (const dbo::ptr<Tag>& tag : tags) {
      auto cb = tagContainer->addNew<Wt::WCheckBox>(tag->name_);
      cb->addStyleClass(radioStyles);
      const bool checked = std::find(prechecked.begin(), prechecked.end(), tag->slug_) != prechecked.end();
      cb->setChecked(checked);
      tagCheckboxes->push_back(cb);
      tagSlugs->push_back(tag->slug_);
    }
  };

  // Initial render with no prechecked items
  refreshTags({});

  // New tag input: enter to add and re-render
  auto newTagLabel = tagContainerWrapper->addNew<Wt::WText>("Add new tag:");
  newTagLabel->addStyleClass("text-sm font-semibold text-gray-700 mt-4");
  auto newTagEdit = tagContainerWrapper->addNew<Wt::WLineEdit>();
  newTagEdit->setPlaceholderText("Type tag name and press Enter");
  newTagEdit->addStyleClass("w-full rounded-md border border-gray-300 p-2");
  newTagEdit->enterPressed().connect([this, newTagEdit, refreshTags, tagCheckboxes, tagSlugs]() {
    std::string tagName = newTagEdit->text().toUTF8();
    // Trim whitespace
    tagName.erase(0, tagName.find_first_not_of(" \t\n\r"));
    if (!tagName.empty())
      tagName.erase(tagName.find_last_not_of(" \t\n\r") + 1);

    if (tagName.empty()) return;

    std::string tagSlug = tagName;
    for (char& c : tagSlug) {
      if (c == ' ') c = '-';
      else c = std::tolower(c);
    }

    // Capture currently selected tags before refresh
    std::vector<std::string> selected;
    for (size_t i = 0; i < tagCheckboxes->size() && i < tagSlugs->size(); ++i) {
      if ((*tagCheckboxes)[i]->isChecked()) {
        selected.push_back((*tagSlugs)[i]);
      }
    }
    // Ensure new tag slug is included
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

    // Re-render checkboxes with new selection
    refreshTags(selected);
    newTagEdit->setText("");

    
  });

  auto briefEdit = container->addNew<MonacoEditor>("html");
  briefEdit->addStyleClass("w-full rounded-md border border-gray-300");
  briefEdit->setHeight(Wt::WLength(200, Wt::LengthUnit::Pixel));

  auto bodyEdit = container->addNew<MonacoEditor>("html");
  bodyEdit->addStyleClass("w-full rounded-md border border-gray-300");
  bodyEdit->setHeight(Wt::WLength(500, Wt::LengthUnit::Pixel));


  createBtn->addStyleClass("bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded");

  auto status = container->addNew<Wt::WText>("");
  status->addStyleClass("mt-2");

  createBtn->clicked().connect([this, titleEdit, briefEdit, bodyEdit, stateGroup, 
                                 tagCheckboxes, tagSlugs, status]() {
    if (!isBlogAdmin(*session_)) {
      status->setText("Not authorized.");
      return;
    }

    std::string postTitle = titleEdit->text().toUTF8();
    std::string brief = briefEdit->getUnsavedText();
    std::string body  = bodyEdit->getUnsavedText();
    if (postTitle.empty()) {
      status->setText("Title is required.");
      return;
    }

    dbo::Transaction t(*session_);
    
    // Create the post
    auto post = session_->add(std::make_unique<Post>());
    auto p = post.modify();

    p->title_ = postTitle;
    std::string slug = postTitle;
    for (char& c : slug) { if (c == ' ') c = '-'; else c = std::tolower(c); }
    p->slug_ = slug;
    p->briefXml_ = brief;
    p->bodyXml_ = body;
    p->author_ = session_->user();
    
    // Set state based on radio button selection
    int stateIdx = stateGroup->checkedId();
    if (stateIdx == 1) {
      p->state_ = Post::State::Published;
      p->publishedAt_ = Wt::WDateTime::currentDateTime();
    } else if (stateIdx == 2) {
      p->state_ = Post::State::Archived;
    } else {
      p->state_ = Post::State::Draft;
    }
    
    auto now = Wt::WDateTime::currentDateTime();
    p->createdAt_ = now;
    p->updatedAt_ = now;
    p->viewCount_ = 0;
    
    // Associate selected tags (includes newly added via enter)
    for (size_t i = 0; i < tagCheckboxes->size() && i < tagSlugs->size(); ++i) {
      if ((*tagCheckboxes)[i]->isChecked()) {
        auto tagQuery = session_->find<Tag>("where slug = ?").bind((*tagSlugs)[i]);
        auto tags = tagQuery.resultList();
        if (!tags.empty()) {
          post.modify()->tags_.insert(tags.front());
        }
      }
    }
    
    t.commit();

    // Redirect to the new post page
        Wt::WApplication::instance()->setInternalPath("/blog/post/" + slug, true);
  });

  return container;
}
