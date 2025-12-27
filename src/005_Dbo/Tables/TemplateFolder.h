#pragma once

#include <string>

#include <Wt/Dbo/Types.h>

class TemplateFile;

class TemplateFolder : public Wt::Dbo::Dbo<TemplateFolder>
{
public:
  std::string folderName_;

  Wt::Dbo::collection< Wt::Dbo::ptr<TemplateFile> > files_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, folderName_, "folder_name");
    Wt::Dbo::hasMany(a, files_, Wt::Dbo::ManyToOne, "folder");
  }
};


DBO_EXTERN_TEMPLATES(TemplateFolder)
