#pragma once

#include <string>

#include <Wt/Dbo/Types.h>
#include "005_Dbo/Tables/Post.h"

class Post;

class Tag : public Wt::Dbo::Dbo<Tag>
{
public:
  std::string name_;
  std::string slug_;

  Wt::Dbo::collection< Wt::Dbo::ptr<Post> > posts_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, name_, "name");
    Wt::Dbo::field(a, slug_, "slug");

    Wt::Dbo::hasMany(a, posts_, Wt::Dbo::ManyToMany, "posts_tags");
  }
};


DBO_EXTERN_TEMPLATES(Tag)
