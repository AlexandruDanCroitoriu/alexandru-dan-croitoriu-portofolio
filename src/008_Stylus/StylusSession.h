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
  explicit StylusSession(const std::string& sqliteDb, bool dev);

  /// Get the next available order number for a new TemplateFolder
  int getNextFolderOrder();

private:
  void createInitialDataDev();
  void createInitialDataProd();
  bool created_ = false;
};
