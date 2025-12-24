#include "006_Auth/AuthWidget.h"
#include "006_Auth/RegistrationView.h"
#include "005_Dbo/Session.h"
#include "006_Auth/UserDetailsModel.h"
#include "005_Dbo/Tables/User.h"
#include "005_Dbo/Tables/Permission.h"

#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/AuthModel.h>
#include <Wt/WApplication.h>
#include <Wt/WAnchor.h>
#include <Wt/WPushButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WDialog.h>
#include <Wt/WRadioButton.h>
#include <Wt/WCheckBox.h>

AuthWidget::AuthWidget(std::shared_ptr<Session> session)
  : Wt::Auth::AuthWidget(Session::auth(), session->users(), session->login()),
    session_(session), loginTemplateId_("Wt.Auth.template.login-v1")
{ 
  // Disable internal path routing; Navigation drives dialog via its own paths
  setInternalBasePath("");

  model()->addPasswordAuth(&Session::passwordAuth());
  model()->addOAuth(Session::oAuth());
  setRegistrationEnabled(true);

  keyWentDown().connect([=](Wt::WKeyEvent e)
  { 
      wApp->globalKeyWentDown().emit(e); // Emit the global key event
  });

  processEnvironment();
}

std::unique_ptr<Wt::WWidget> AuthWidget::createRegistrationView(const Wt::Auth::Identity& id)
{
  auto registrationView = std::make_unique<RegistrationView>(session_, this);
  std::unique_ptr<Wt::Auth::RegistrationModel> registrationModel = createRegistrationModel();

  if (id.isValid())
    registrationModel->registerIdentified(id);

  registrationView->setModel(std::move(registrationModel));
  return registrationView;
}

void AuthWidget::createLoginView()
{
  setTemplateText(tr(loginTemplateId_)); // default wt template

  // setTemplateText(tr("Wt.Auth.template.login-v0")); // v0 nothing but the data and some basic structure
  // setTemplateText(tr("Wt.Auth.template.login-v1")); // custom implementation v1


  // auto container = bindWidget("template-changer-widget", std::make_unique<Wt::WContainerWidget>());
  // container->setStyleClass("flex items-center justify-start space-x-2");
  // auto group = std::make_shared<Wt::WButtonGroup>();

  // auto default_tmp_btn = container->addNew<Wt::WRadioButton>("default");
  // group->addButton(default_tmp_btn);

  // auto v0_tmp_btn = container->addNew<Wt::WRadioButton>("v0");
  // group->addButton(v0_tmp_btn);

  // auto v1_tmp_btn = container->addNew<Wt::WRadioButton>("v1");
  // group->addButton(v1_tmp_btn);

  // if(loginTemplateId_.compare("Wt.Auth.template.login") == 0)
  // {
  //   group->setSelectedButtonIndex(0); // Select the first button by default.
  // }else if(loginTemplateId_.compare("Wt.Auth.template.login-v0") == 0)
  // {
  //   group->setSelectedButtonIndex(1); // Select the second button by default.
  // }else if(loginTemplateId_.compare("Wt.Auth.template.login-v1") == 0)
  // {
  //   group->setSelectedButtonIndex(2); // Select the third button by default.
  // }

  // group->checkedChanged().connect(this, [=](Wt::WRadioButton *button) {
  //   if(button == default_tmp_btn) {
  //     loginTemplateId_ = "Wt.Auth.template.login";
  //   } else if(button == v0_tmp_btn) {
  //     loginTemplateId_ = "Wt.Auth.template.login-v0";
  //   } else if(button == v1_tmp_btn) {
  //     loginTemplateId_ = "Wt.Auth.template.login-v1";
  //   }
  //   // setTemplateText(tr(loginTemplateId_));
  //   model()->reset();
  //   createLoginView(); // Recreate the login view with the new template
  // });

  createPasswordLoginView();
  createOAuthLoginView();
#ifdef WT_HAS_SAML
  createSamlLoginView();
#endif // WT_HAS_SAML_
  
  // Set remember me checkbox to be checked by default
  if (auto rememberCheckbox = resolve<Wt::WCheckBox*>(Wt::Auth::AuthModel::RememberMeField)) {
    rememberCheckbox->setChecked(true);
  }

  // Keep registration in the same dialog without changing internal paths
  if (auto regAnchor = resolve<Wt::WAnchor*>("register")) {
    // Prevent navigation: keep anchor but disable default link behavior
    regAnchor->setLink(Wt::WLink("javascript:void(0)"));
    regAnchor->clicked().connect([this]() { showRegistrationView(); });
  } else if (auto regBtn = resolve<Wt::WPushButton*>("register")) {
    regBtn->clicked().connect([this]() { showRegistrationView(); });
  }

}

Wt::WDialog *AuthWidget::showDialog(const Wt::WString& title, std::unique_ptr<Wt::WWidget> contents) 
{
  if (contents) {
    dialog_ = std::make_unique<Wt::WDialog>(title);
    dialog_->contents()->addWidget(std::move(contents));
    // dialog_->setMinimumSize(Wt::WLength(100, Wt::LengthUnit::ViewportWidth), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    // dialog_->setMaximumSize(Wt::WLength(100, Wt::LengthUnit::ViewportWidth), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    // dialog_->setStyleClass("absolute top-0 left-0 right-0 bottom-0 w-screen h-screen");
    dialog_->setTitleBarEnabled(false);
    dialog_->escapePressed().connect([this]() { dialog_.reset(); });
    // dialog_->contents()->setStyleClass("min-h-screen min-w-screen m-1 p-1 flex items-center justify-center bg-white dark:bg-gray-900 text-gray-900 dark:text-white");
    dialog_->contents()->childrenChanged().connect(this, [this]() { dialog_.reset(); });

    dialog_->setStyleClass("absolute bg-gray-900 rounded-md [&>div]:h-full");
    dialog_->addStyleClass("top-0 left-0 right-0 bottom-0");
    dialog_->contents()->setStyleClass("p-4 h-full flex justify-center");


    dialog_->footer()->hide();

  if (!Wt::WApplication::instance()->environment().ajax()) {
      /*
       * try to center it better, we need to set the half width and
       * height as negative margins.
       */
      // dialog_->setMargin(Wt::WLength("-21em"), Wt::Side::Left); // .Wt-form width
      // dialog_->setMargin(Wt::WLength("-200px"), Wt::Side::Top); // ???
    }

    dialog_->show();
  }

  return dialog_.get();
}

void AuthWidget::showRegistrationView()
{
  // Switch to registration flow; base provides registerNewUser()
    currentView_ = ViewState::Registration;
  this->registerNewUser();
}

void AuthWidget::showLoginView()
{
  // Switch back to login view by resetting the model
    currentView_ = ViewState::Login;
  model()->reset();
}

void AuthWidget::hideInternalDialog()
{
  if (dialog_) {
    dialog_->hide();
    dialog_.reset();
  }
}
