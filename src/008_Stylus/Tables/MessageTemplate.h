#pragma once

#include <string>

#include <Wt/Dbo/Types.h>

namespace Stylus {

class TemplateFile;

enum class ViewMode 
{
    Template,
    Editor
};

class MessageTemplate : public Wt::Dbo::Dbo<MessageTemplate>
{
public:
  std::string messageId_;
  std::string templateXml_;
  int order;
  bool selected_ = false;
  ViewMode viewMode_;

  Wt::Dbo::ptr<TemplateFile> file_;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, messageId_, "message_id");
    Wt::Dbo::field(a, templateXml_, "template_xml");
    Wt::Dbo::field(a, order, "order_index");
    Wt::Dbo::field(a, viewMode_, "view_mode");
    Wt::Dbo::field(a, selected_, "selected");
    Wt::Dbo::belongsTo(a, file_, "file");
  }
};

} // namespace Stylus

DBO_EXTERN_TEMPLATES(Stylus::MessageTemplate)
