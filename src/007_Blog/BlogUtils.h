#pragma once

#include "005_Dbo/Session.h"
#include "005_Dbo/Tables/Permission.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WApplication.h>

namespace dbo = Wt::Dbo;

inline bool isBlogAdmin(Session& session)
{
  wApp->log("debug") << "isBlogAdmin(Session& session)";
  if (!session.login().loggedIn()) return false;
  dbo::Transaction t(session);
  auto user = session.user();
  if (!user) return false;
  auto perms = session.find<Permission>().where("name = ?").bind("BLOG_ADMIN").resultList();
  if (perms.empty()) return false;
  return user->hasPermission(perms.front());
}
