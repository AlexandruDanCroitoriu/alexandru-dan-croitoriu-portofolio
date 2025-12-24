#pragma once
#include <memory>
#include <string>

#include <Wt/Auth/AuthWidget.h>

class Session;

class AuthWidget : public Wt::Auth::AuthWidget
{
public:
  explicit AuthWidget(std::shared_ptr<Session> session);

  /* We will use a custom registration view */
  std::unique_ptr<Wt::WWidget> createRegistrationView(const Wt::Auth::Identity& id) override;
  void createLoginView() override;

  // Programmatically switch to the registration view
  void showRegistrationView();
  
  // Programmatically switch to the login view
  void showLoginView();

  // Hide internal dialog (registration/login overlays opened by AuthWidget)
  void hideInternalDialog();

  enum class ViewState { Login, Registration };
  ViewState getCurrentView() const { return currentView_; }

protected:
  Wt::WDialog *showDialog(const Wt::WString& title, std::unique_ptr<Wt::WWidget> contents) override;

private:
  std::shared_ptr<Session> session_;
  std::string loginTemplateId_; // default template id
  std::unique_ptr<Wt::WDialog> dialog_;
  ViewState currentView_ = ViewState::Login;
};
