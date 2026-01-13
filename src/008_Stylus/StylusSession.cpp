#include "008_Stylus/StylusSession.h"
#include "008_Stylus/Tables/TemplateFile.h"
#include "008_Stylus/Tables/TemplateFolder.h"
#include "008_Stylus/Tables/MessageTemplate.h"

#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/WLogger.h>
#include <Wt/WApplication.h>

#include <memory>
#include <exception>

StylusSession::StylusSession(const std::string &sqliteDb, bool dev)
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

  if(dev)
    createInitialDataDev();
  else 
    createInitialDataProd();
}

void StylusSession::createInitialDataDev()
{
  Wt::log("debug") << "StylusSession::createInitialData()";
  
  Wt::Dbo::Transaction t(*this);

  // Check if any folders already exist
  auto existingFolders = find<TemplateFolder>().resultList();

  if (existingFolders.empty()) {
    // Create 3 default folders with files and templates
    for (int i = 1; i <= 5; ++i) {
      auto folder = add(std::make_unique<TemplateFolder>());
      auto f = folder.modify();
      f->folderName_ = "Templates Folder " + std::to_string(i);
      f->expanded_ = true;
      f->order = getNextFolderOrder();  // Use helper method for consistent ordering

      // Create 3 template files per folder
      for (int j = 1; j <= 3; ++j) {
        auto tempFile = add(std::make_unique<TemplateFile>());
        auto tf = tempFile.modify();
        tf->fileName_ = "file-" + std::to_string(i) + "-" + std::to_string(j);
        tf->folder_ = folder;
        tf->expanded_ = true;
        tf->order = j;  // Keep initial files ordered 1..n inside each folder

        // Create 3 message templates per file
        for (int k = 1; k <= 3; ++k) {
          auto messageTemp = add(std::make_unique<MessageTemplate>());
          auto mt = messageTemp.modify();
          mt->viewMode_ = ViewMode::Template;
          mt->messageId_ = "temp-" + std::to_string(i) + "-" + std::to_string(j) + "-" + std::to_string(k);
            mt->templateXml_ = 
R"(<div class="divide-y divide-white/10 overflow-hidden rounded-lg bg-gray-800/50 outline outline-1 -outline-offset-1 outline-white/10">
  <div class="px-4 py-5 sm:px-6">
    <div>Card Header</div>
  </div>
  <div class="px-4 py-5 sm:p-6">
    <div>Card Content</div>
  </div>
  <div class="px-4 py-4 sm:px-6">
    <div>Card Footer</div>
  </div>
</div>)";
          mt->file_ = tempFile;
          mt->order = k;
        }
      }
    }

    Wt::log("info") << "Created initial Stylus templates and folders.";
    t.commit();
  }

}

void StylusSession::createInitialDataProd()
{
  Wt::log("debug") << "StylusSession::createInitialDataProd()";
  
  Wt::Dbo::Transaction t(*this);

  // Check if any folders already exist
  auto existingFolders = find<TemplateFolder>().resultList();

  if (existingFolders.empty()) {
    auto folder = add(std::make_unique<TemplateFolder>());
    auto f = folder.modify();
    f->folderName_ = "Tailwind Plus Templates";
    f->expanded_ = true;
    f->order = getNextFolderOrder();
    auto tempFile = add(std::make_unique<TemplateFile>());
    auto tf = tempFile.modify();
    tf->fileName_ = "Cards";
    tf->folder_ = folder;
    tf->expanded_ = true;
    tf->order = 1;
    auto messageTemp = add(std::make_unique<MessageTemplate>());
    auto mt = messageTemp.modify();
    mt->viewMode_ = ViewMode::Template;
    mt->messageId_ = "basic-card";
    mt->templateXml_ = 
R"(<div class="overflow-hidden rounded-lg bg-gray-800/50 outline outline-1 -outline-offset-1 outline-white/10">
  <div class="px-4 py-5 sm:p-6">
  </div>
</div>)";
    mt->file_ = tempFile;
    mt->order = 1;
  }
  t.commit();
}


int StylusSession::getNextFolderOrder()
{
  Wt::log("debug") << "StylusSession::getNextFolderOrder()";
  
  // Get the maximum order value from all existing folders
  // If no folders exist, start with 1
  auto allFolders = find<TemplateFolder>().resultList();
  
  int maxOrder = 0;
  for (const auto& folder : allFolders) {
    if (folder->order > maxOrder) {
      maxOrder = folder->order;
    }
  }
  
  return maxOrder + 1;
}
