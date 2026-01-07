#pragma once

#include <string>

#include <Wt/Dbo/Types.h>

class TemplateFolder;
class MessageTemplate;

class TemplateFile : public Wt::Dbo::Dbo<TemplateFile>
{
public:
  std::string fileName_;
  int order;
  bool expanded_ = true;
  Wt::Dbo::ptr<TemplateFolder> folder_;
  Wt::Dbo::collection< Wt::Dbo::ptr<MessageTemplate> > templates_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, fileName_, "file_name");
    Wt::Dbo::field(a, expanded_, "expanded");
    Wt::Dbo::belongsTo(a, folder_, "folder");
    Wt::Dbo::hasMany(a, templates_, Wt::Dbo::ManyToOne, "file");
  }
};


DBO_EXTERN_TEMPLATES(TemplateFile)
