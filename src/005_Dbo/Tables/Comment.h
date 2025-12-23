#pragma once

#include <string>

#include <Wt/Dbo/Types.h>
#include <Wt/WDateTime.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/User.h"

class Post;
class User;

class Comment : public Wt::Dbo::Dbo<Comment>
{
public:
  Wt::Dbo::ptr<Post> post_;
  Wt::Dbo::ptr<User> author_;

  Wt::Dbo::ptr<Comment> parent_;
  Wt::Dbo::collection< Wt::Dbo::ptr<Comment> > replies_;

  std::string content_;
  Wt::WDateTime createdAt_;
  Wt::WDateTime updatedAt_;
  bool isApproved_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::belongsTo(a, post_, "post");
    Wt::Dbo::belongsTo(a, author_, "author");

    Wt::Dbo::belongsTo(a, parent_, "parent");
    Wt::Dbo::hasMany(a, replies_, Wt::Dbo::ManyToOne, "parent");

    Wt::Dbo::field(a, content_, "content");
    Wt::Dbo::field(a, createdAt_, "created_at");
    Wt::Dbo::field(a, updatedAt_, "updated_at");
    Wt::Dbo::field(a, isApproved_, "is_approved");
  }
};


DBO_EXTERN_TEMPLATES(Comment)
