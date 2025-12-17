#pragma once
#include <Wt/WFormModel.h>
#include <memory>


class Session;

class UserDetailsModel : public Wt::WFormModel
{
public:
  static const Field UI_DARK_MODE_FIELD;
  static const Field UI_PENGUIN_THEME_NAME_FIELD;

  UserDetailsModel(std::shared_ptr<Session> session);

  void save(const Wt::Auth::User& user);

private:
  std::shared_ptr<Session> session_;
};
