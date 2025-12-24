#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Permission.h"
#include "005_Dbo/Tables/Post.h"
#include "005_Dbo/Tables/Comment.h"
#include "005_Dbo/Tables/Tag.h"
#include "000_Server/Server.h"

#include <Wt/Dbo/SqlConnection.h>
#if defined(DEBUG)
#include <Wt/Dbo/backend/Sqlite3.h>
#else
#include <Wt/Dbo/backend/Postgres.h>
#endif
#include <Wt/Auth/Identity.h>
#include <Wt/Auth/PasswordService.h>


#include <cstdlib>
#include <memory>
#include <stdexcept>


Session::Session(const std::string &sqliteDb)
{
  std::unique_ptr<Wt::Dbo::SqlConnection> connection;

  #ifdef DEBUG
  // Debug mode - use SQLite
  auto sqliteConnection = std::make_unique<Wt::Dbo::backend::Sqlite3>(sqliteDb);
  sqliteConnection->setProperty("show-queries", "true");
  Wt::log("info") << "Using SQLite database in debug mode";
  connection = std::move(sqliteConnection);
  #else
  // Production mode - check if PostgreSQL is available
  #ifdef WT_DBO_POSTGRES
  // PostgreSQL is enabled - use it for production
  const char *postgresHost = std::getenv("POSTGRES_HOST");
  if (!postgresHost) {
    throw std::runtime_error("POSTGRES_HOST environment variable is not set");
  }

  const char *postgresPort = std::getenv("POSTGRES_PORT");
  if (!postgresPort) {
    throw std::runtime_error("POSTGRES_PORT environment variable is not set");
  }

  const char *postgresDatabase = std::getenv("POSTGRES_DBNAME");
  if (!postgresDatabase) {
    throw std::runtime_error("POSTGRES_DBNAME environment variable is not set");
  }

  const char *postgresUser = std::getenv("POSTGRES_USER");
  if (!postgresUser) {
    throw std::runtime_error("POSTGRES_USER environment variable is not set");
  }

  const char *postgresPassword = std::getenv("POSTGRES_PASSWORD");
  if (!postgresPassword) {
    throw std::runtime_error("POSTGRES_PASSWORD environment variable is not set");
  }

  std::string postgresConnectionString = "host=" + std::string(postgresHost) +
                  " port=" + std::string(postgresPort) +
                  " dbname=" + std::string(postgresDatabase) +
                  " user=" + std::string(postgresUser) +
                  " password=" + std::string(postgresPassword);

  auto postgresConnection = std::make_unique<Wt::Dbo::backend::Postgres>(postgresConnectionString.c_str());
  Wt::log("info") << "Using PostgreSQL database in production mode";
  connection = std::move(postgresConnection);
  #else
  // PostgreSQL not available - use SQLite for production
  auto sqliteConnection = std::make_unique<Wt::Dbo::backend::Sqlite3>(sqliteDb);
  Wt::log("info") << "Using SQLite database in production mode (PostgreSQL not available)";
  connection = std::move(sqliteConnection);
  #endif
  #endif

  if (!connection) {
    throw std::runtime_error("Database connection was not initialised");
  }

  setConnection(std::move(connection));

  mapClass<User>("user");
  mapClass<Permission>("permission");
  mapClass<Post>("post");
  mapClass<Comment>("comment");
  mapClass<Tag>("tag");
  mapClass<AuthInfo>("auth_info");
  mapClass<AuthInfo::AuthIdentityType>("auth_identity");
  mapClass<AuthInfo::AuthTokenType>("auth_token");

  try {
    if (!created_) {
      createTables();
      created_ = true;
      Wt::log("info") << "Created database.";
    } else {
      Wt::log("info") << "Using existing database";
    }
  } catch (Wt::Dbo::Exception& e) {
    Wt::log("info") << "Using existing database";
  }
  users_ = std::make_unique<UserDatabase>(*this);
  createInitialData();

}


Wt::Auth::AbstractUserDatabase& Session::users()
{
  return *users_;
}

dbo::ptr<User> Session::user() const
{
  if (login_.loggedIn()) {
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    return authInfo->user();
  } else
    return dbo::ptr<User>();
}

dbo::ptr<User> Session::user(const Wt::Auth::User& authUser)
{
  dbo::ptr<AuthInfo> authInfo = users_->find(authUser);

  dbo::ptr<User> user = authInfo->user();

  if (!user) {
    user = add(std::make_unique<User>());
    authInfo.modify()->setUser(user);
  }

  return user;
}

const Wt::Auth::AuthService& Session::auth()
{
  return Server::authService;
}

const Wt::Auth::PasswordService& Session::passwordAuth()
{
  return Server::passwordService;
}

std::vector<const Wt::Auth::OAuthService *> Session::oAuth()
{
  std::vector<const Wt::Auth::OAuthService *> result;
  result.reserve(Server::oAuthServices.size());
  for (const auto& auth : Server::oAuthServices) {
    result.push_back(auth.get());
  }
  return result;
}

Wt::Dbo::ptr<User> addUser(Wt::Dbo::Session& session, UserDatabase& users, const std::string& loginName,
             const std::string& email, const std::string& password)
{
  Wt::Dbo::Transaction t(session);
  auto user = session.addNew<User>(loginName);
  auto authUser = users.registerNew();
  authUser.addIdentity(Wt::Auth::Identity::LoginName, loginName);
  authUser.setEmail(email);
  Server::passwordService.updatePassword(authUser, password);

  // Link User and auth user
  Wt::Dbo::ptr<AuthInfo> authInfo = session.find<AuthInfo>("where id = ?").bind(authUser.id());
  authInfo.modify()->setUser(user);

  t.commit();
  return user;
}

void Session::createInitialData()
{
  // Ensure STYLUS and BLOG_ADMIN permissions exist
  {
    Wt::Dbo::Transaction t(*this);

    Wt::Dbo::ptr<Permission> stylusPermission = find<Permission>()
      .where("name = ?")
      .bind("STYLUS");
    if (!stylusPermission) {
      stylusPermission = add(std::make_unique<Permission>("STYLUS"));
      Wt::log("info") << "Created STYLUS permission.";
    }

    Wt::Dbo::ptr<Permission> blogAdminPermission = find<Permission>()
      .where("name = ?")
      .bind("BLOG_ADMIN");
    if (!blogAdminPermission) {
      blogAdminPermission = add(std::make_unique<Permission>("BLOG_ADMIN"));
      Wt::log("info") << "Created BLOG_ADMIN permission.";
    }

    t.commit();
  }

  // Lookup or create admin user
  Wt::Dbo::ptr<User> adminUser;
  {
    Wt::Dbo::Transaction t(*this);
    Wt::Dbo::ptr<AuthInfo::AuthIdentityType> existingIdentity =
      find<AuthInfo::AuthIdentityType>()
        .where("provider = ? AND identity = ?")
        .bind(Wt::Auth::Identity::LoginName)
        .bind("maxuli");

    if (existingIdentity) {
      auto authUser = users_->findWithIdentity(Wt::Auth::Identity::LoginName, std::string("maxuli"));
      adminUser = user(authUser);
      Wt::log("info") << "Admin user 'maxuli' already exists.";
    } else {
      adminUser = addUser(*this, *users_, "maxuli", "maxuli@example.com", "asdfghj1");
      Wt::log("info") << "Created admin user 'maxuli'.";
    }
    t.commit();
  }

  // Ensure admin has STYLUS and BLOG_ADMIN permissions
  {
    Wt::Dbo::Transaction t(*this);
    Wt::Dbo::ptr<Permission> stylusPermission = find<Permission>()
      .where("name = ?")
      .bind("STYLUS");
    Wt::Dbo::ptr<Permission> blogAdminPermission = find<Permission>()
      .where("name = ?")
      .bind("BLOG_ADMIN");

    if (stylusPermission && !adminUser->hasPermission(stylusPermission)) {
      adminUser.modify()->permissions_.insert(stylusPermission);
    }
    if (blogAdminPermission && !adminUser->hasPermission(blogAdminPermission)) {
      adminUser.modify()->permissions_.insert(blogAdminPermission);
    }
    t.commit();
  }

  // Seed initial blog tags, posts, and comments if none exist
  {
    Wt::Dbo::Transaction t(*this);

    auto existingPosts = find<Post>().resultList();
    if (existingPosts.empty()) {
      // Ensure a couple of regular users for comments
      auto ensureUser = [this](const std::string& loginName,
                               const std::string& email,
                               const std::string& password) -> Wt::Dbo::ptr<User>
      {
        Wt::Dbo::ptr<AuthInfo::AuthIdentityType> identity =
          find<AuthInfo::AuthIdentityType>()
            .where("provider = ? AND identity = ?")
            .bind(Wt::Auth::Identity::LoginName)
            .bind(loginName);
        if (identity)
        {
          auto authUser = users_->findWithIdentity(Wt::Auth::Identity::LoginName, loginName);
          return user(authUser);
        }
        return addUser(*this, *users_, loginName, email, password);
      };

      auto alice = ensureUser("alice", "alice@example.com", "alice123A!");
      auto bob   = ensureUser("bob",   "bob@example.com",   "bob123A!");

      // Create tags if missing
      auto getOrCreateTag = [this](const std::string& name, const std::string& slug) -> Wt::Dbo::ptr<Tag>
      {
        auto q = find<Tag>().where("slug = ?").bind(slug).resultList();
        if (!q.empty()) return q.front();
        auto tag = add(std::make_unique<Tag>());
        tag.modify()->name_ = name;
        tag.modify()->slug_ = slug;
        return tag;
      };

      auto tagCpp = getOrCreateTag("C++", "cpp");
      auto tagWt  = getOrCreateTag("Wt",  "wt");
      auto tagPf  = getOrCreateTag("Portfolio", "portfolio");

      auto now = Wt::WDateTime::currentDateTime();

      auto createPost = [this, &now, &adminUser](const std::string& title,
                                                 const std::string& briefHtml,
                                                 const std::string& bodyHtml) -> Wt::Dbo::ptr<Post>
      {
        auto post = add(std::make_unique<Post>());
        auto p = post.modify();
        p->title_ = title;
        // simple slug: lowercase with dashes
        std::string slug = title;
        for (char& c : slug) { if (c == ' ') c = '-'; else c = std::tolower(c); }
        p->slug_ = slug;
        p->briefSrc_ = briefHtml; // storing same content for now
        p->briefHtml_ = briefHtml;
        p->bodySrc_ = bodyHtml;
        p->bodyHtml_ = bodyHtml;
        p->author_ = adminUser;
        p->state_ = Post::State::Published;
        p->publishedAt_ = now;
        p->createdAt_ = now;
        p->updatedAt_ = now;
        p->viewCount_ = 0;
        return post;
      };

      auto p1 = createPost(
        "Welcome to the Portfolio Blog",
        "<p>Short intro to the portfolio and blog.</p>",
        "<p>Welcome! This blog shares updates on C++ projects, Wt components, and portfolio highlights.</p>"
      );
      auto p2 = createPost(
        "Wt + Tailwind UI Notes",
        "<p>Combining Wt with Tailwind CSS.</p>",
        "<p>We use Wt for robust server-side C++ web apps and Tailwind for modern UI styling.</p>"
      );
      auto p3 = createPost(
        "C++ Tips for Web Apps",
        "<p>Practical C++ techniques in web contexts.</p>",
        "<p>RAII, smart pointers, and Dbo mappings help keep web app code clean and safe.</p>"
      );

      // Attach tags
      p1.modify()->tags_.insert(tagPf);
      p1.modify()->tags_.insert(tagWt);
      p2.modify()->tags_.insert(tagWt);
      p2.modify()->tags_.insert(tagCpp);
      p3.modify()->tags_.insert(tagCpp);

      // Comments on each post
      auto addComment = [this, &now](const Wt::Dbo::ptr<Post>& post,
                                     const Wt::Dbo::ptr<User>& author,
                                     const std::string& content,
                                     const Wt::Dbo::ptr<Comment>& parent = Wt::Dbo::ptr<Comment>()) -> Wt::Dbo::ptr<Comment>
      {
        auto c = add(std::make_unique<Comment>());
        auto cm = c.modify();
        cm->post_ = post;
        cm->author_ = author;
        cm->parent_ = parent;
        cm->content_ = content;
        cm->createdAt_ = now;
        cm->updatedAt_ = now;
        cm->isApproved_ = true;
        return c;
      };

      auto c11 = addComment(p1, alice, "Great start! Looking forward to more posts.");
      addComment(p1, bob,   "+1, excited to see Wt content.", c11);

      auto c21 = addComment(p2, bob,   "Tailwind + Wt is a nice combo.");
      addComment(p2, alice, "Would love code snippets.", c21);

      auto c31 = addComment(p3, alice, "RAII saves the day.");
      addComment(p3, bob,   "Smart pointers FTW!", c31);

      Wt::log("info") << "Seeded initial blog posts, tags, and comments.";
    }

    t.commit();
  }
}

void Session::restoreLogin()
{
  // Try to restore login from remember-me token if present
  // This is called when a new session starts, to restore login from auth token cookie
  if (!login_.loggedIn()) {
    const std::string* tokenCookiePtr = Wt::WApplication::instance()->environment().getCookie("logincookie");
    if (tokenCookiePtr) {
      std::string tokenCookie = *tokenCookiePtr;
      // Query the auth_token table to find and verify the token
      Wt::Dbo::Transaction t(*this);
      auto tokenResult = find<AuthInfo::AuthTokenType>("where value = ?")
        .bind(tokenCookie);
      
      auto token = tokenResult.resultValue();
      if (token && !token->expires().isNull() && token->expires() > Wt::WDateTime::currentDateTime()) {
        // Token is valid, get the associated AuthInfo
        Wt::Dbo::ptr<AuthInfo> authInfo = token->authInfo();
        
        if (authInfo) {
          login_.login(Wt::Auth::User(std::to_string(authInfo->id()), *users_));
          Wt::log("info") << "Restored login from remember-me token";
        }
      }
      t.commit();
    }
  }
}



