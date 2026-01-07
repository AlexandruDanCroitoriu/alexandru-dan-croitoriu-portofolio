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
    // Create 3 default folders with files and templates
    for (int i = 1; i <= 3; ++i) {
      auto folder = add(std::make_unique<TemplateFolder>());
      auto f = folder.modify();
      f->folderName_ = "Templates Folder " + std::to_string(i);
      f->expanded_ = true;

      // Create 3 template files per folder
      for (int j = 1; j <= 3; ++j) {
        auto tempFile = add(std::make_unique<TemplateFile>());
        auto tf = tempFile.modify();
        tf->fileName_ = "file-" + std::to_string(i) + "-" + std::to_string(j);
        tf->folder_ = folder;
        tf->expanded_ = true;

        // Create 3 message templates per file
        for (int k = 1; k <= 3; ++k) {
          auto messageTemp = add(std::make_unique<MessageTemplate>());
          auto mt = messageTemp.modify();
          mt->messageId_ = "temp-" + std::to_string(i) + "-" + std::to_string(j) + "-" + std::to_string(k);
          mt->templateXml_ = "<div>Template " + std::to_string(i) + "-" + std::to_string(j) + "-" + std::to_string(k) + "</div>";
          mt->file_ = tempFile;
        }
      }
    }

    Wt::log("info") << "Created initial Stylus templates and folders.";
    t.commit();
  }

}