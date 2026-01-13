#pragma once

#include <string>

#include <Wt/Dbo/Types.h>

namespace Stylus {

class TemplateFile;

class TemplateFolder : public Wt::Dbo::Dbo<TemplateFolder>
{
public:
  std::string folderName_;
  bool expanded_ = true;
  int order;
  bool selected_ = false;
  Wt::Dbo::collection< Wt::Dbo::ptr<TemplateFile> > files_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, folderName_, "folder_name");
    Wt::Dbo::field(a, expanded_, "expanded");
    Wt::Dbo::field(a, selected_, "selected");
    Wt::Dbo::field(a, order, "order_index");
    Wt::Dbo::hasMany(a, files_, Wt::Dbo::ManyToOne, "folder");
  }
};

} // namespace Stylus

DBO_EXTERN_TEMPLATES(Stylus::TemplateFolder)
