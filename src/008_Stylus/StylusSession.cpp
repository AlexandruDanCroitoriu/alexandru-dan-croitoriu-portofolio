#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/Tables/MessageTemplate.h"

#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/WLogger.h>
#include <Wt/WApplication.h>

#include <memory>

StylusSession::StylusSession(const std::string &sqliteDb)
{
  Wt::log("debug") << "StylusSession::StylusSession()";
  
  // Stylus always uses SQLite
  auto sqliteConnection = std::make_unique<Wt::Dbo::backend::Sqlite3>(sqliteDb);
  #ifdef DEBUG
  sqliteConnection->setProperty("show-queries", "true");
  #endif
  Wt::log("info") << "Using SQLite database for Stylus: " << sqliteDb;

  setConnection(std::move(sqliteConnection));

  mapClass<TemplateFile>("template_file");
  mapClass<TemplateFolder>("template_folder");
  mapClass<MessageTemplate>("message_template");

  try {
    if (!created_) {
      createTables();
      created_ = true;
      Wt::log("info") << "Created Stylus database tables.";
    } else {
      Wt::log("info") << "Using existing Stylus database tables";
    }
  } catch (Wt::Dbo::Exception& e) {
    Wt::log("info") << "Using existing Stylus database tables";
  }
  createInitialData();
}

void StylusSession::createInitialData()
{
  Wt::log("debug") << "StylusSession::createInitialData()";
  
  Wt::Dbo::Transaction t(*this);

  // Check if any folders already exist
  auto existingFolders = find<TemplateFolder>().resultList();

  if (existingFolders.empty()) {
    // Create default templates folder
    auto defaultFolder = add(std::make_unique<TemplateFolder>());
    auto f = defaultFolder.modify();
    f->folderName_ = "Default Templates";
    f->expanded_ = true;

    // Create sample template files
    auto emailFile = add(std::make_unique<TemplateFile>());
    auto ef = emailFile.modify();
    ef->fileName_ = "email_template.xml";
    ef->folder_ = defaultFolder;
    ef->expanded_ = true;

    auto htmlFile = add(std::make_unique<TemplateFile>());
    auto hf = htmlFile.modify();
    hf->fileName_ = "html_template.xml";
    hf->folder_ = defaultFolder;
    hf->expanded_ = true;

    // Create sample message templates
    auto welcomeMsg = add(std::make_unique<MessageTemplate>());
    auto wm = welcomeMsg.modify();
    wm->messageId_ = "welcome";
    wm->templateXml_ = R"(<?xml version="1.0" encoding="UTF-8"?>
<message>
  <subject>Welcome to the System</subject>
  <body>Hello {name},</body>
</message>)";
    wm->file_ = emailFile;

    auto notificationMsg = add(std::make_unique<MessageTemplate>());
    auto nm = notificationMsg.modify();
    nm->messageId_ = "notification";
    nm->templateXml_ = R"(<?xml version="1.0" encoding="UTF-8"?>
<message>
  <subject>System Notification</subject>
  <body>{content}</body>
</message>)";
    nm->file_ = emailFile;

    Wt::log("info") << "Created initial Stylus templates and folders.";
  }

  t.commit();
}
