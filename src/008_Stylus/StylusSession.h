#pragma once

#include <memory>
#include <string>

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>

namespace dbo = Wt::Dbo;

class TemplateFolder;
class TemplateFile;
class MessageTemplate;

class StylusSession : public dbo::Session
{
public:
  explicit StylusSession(const std::string& sqliteDb);

private:
  void createInitialData();
  bool created_ = false;
};
