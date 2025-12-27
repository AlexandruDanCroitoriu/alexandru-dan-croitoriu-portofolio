#pragma once

#include <string>

#include <Wt/Dbo/Types.h>

class TemplateFile;

class MessageTemplate : public Wt::Dbo::Dbo<MessageTemplate>
{
public:
  std::string messageId_;
  std::string templateXml_;

  Wt::Dbo::ptr<TemplateFile> file_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, messageId_, "message_id");
    Wt::Dbo::field(a, templateXml_, "template_xml");
    Wt::Dbo::belongsTo(a, file_, "file");
  }
};


DBO_EXTERN_TEMPLATES(MessageTemplate)
