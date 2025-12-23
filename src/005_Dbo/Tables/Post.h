#pragma once

#include <string>

#include <Wt/Dbo/Types.h>
#include <Wt/WDateTime.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include "005_Dbo/Tables/User.h"

class User;
class Comment;
class Tag;

class Post : public Wt::Dbo::Dbo<Post>
{
public:
  enum class State { Draft = 0, Published = 1, Archived = 2 };

  std::string title_;
  std::string slug_;

  std::string briefSrc_;
  std::string briefHtml_;

  std::string bodySrc_;
  std::string bodyHtml_;

  Wt::Dbo::ptr<User> author_;
  State state_;

  Wt::WDateTime publishedAt_;
  Wt::WDateTime createdAt_;
  Wt::WDateTime updatedAt_;
  int viewCount_;

  Wt::Dbo::collection< Wt::Dbo::ptr<Comment> > comments_;
  Wt::Dbo::collection< Wt::Dbo::ptr<Tag> > tags_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, title_, "title");
    Wt::Dbo::field(a, slug_, "slug");

    Wt::Dbo::field(a, briefSrc_, "brief_src");
    Wt::Dbo::field(a, briefHtml_, "brief_html");

    Wt::Dbo::field(a, bodySrc_, "body_src");
    Wt::Dbo::field(a, bodyHtml_, "body_html");

    Wt::Dbo::belongsTo(a, author_, "author");
    Wt::Dbo::field(a, state_, "state");

    Wt::Dbo::field(a, publishedAt_, "published_at");
    Wt::Dbo::field(a, createdAt_, "created_at");
    Wt::Dbo::field(a, updatedAt_, "updated_at");
    Wt::Dbo::field(a, viewCount_, "view_count");

    Wt::Dbo::hasMany(a, comments_, Wt::Dbo::ManyToOne, "post");
    Wt::Dbo::hasMany(a, tags_, Wt::Dbo::ManyToMany, "posts_tags");
  }
};


DBO_EXTERN_TEMPLATES(Post)
