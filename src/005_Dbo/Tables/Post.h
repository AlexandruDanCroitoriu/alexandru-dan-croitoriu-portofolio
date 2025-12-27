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

  std::string briefXml_;

  std::string bodyXml_;

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

    Wt::Dbo::field(a, briefXml_, "brief_xml");

    Wt::Dbo::field(a, bodyXml_, "body_xml");

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
