/*
 * Copyright (C) 2012 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include "004_TailwindTheme/TailwindThemeBase.h"
#include <vector>
#include <boost/algorithm/string.hpp>

#include <Wt/WAbstractItemView.h>
#include <Wt/WAbstractSpinBox.h>
#include <Wt/WApplication.h>
#include <Wt/WCalendar.h>
#include <Wt/WDateEdit.h>
#include <Wt/WDialog.h>
#include <Wt/WEnvironment.h>
#include <Wt/WLinkedCssStyleSheet.h>
#include <Wt/WMessageResourceBundle.h>
#include <Wt/WPanel.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WProgressBar.h>
#include <Wt/WPushButton.h>
#include <Wt/WSuggestionPopup.h>
#include <Wt/WTabWidget.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WLineEdit.h>

#include "Wt/DomElement.h"
#include <Wt/WRandom.h>

#ifndef WT_DEBUG_JS
#include "js/CssThemeValidate.min.js"
#endif

TailwindThemeBase::TailwindThemeBase()
  : WTheme(),
    name_("default")
{ }

TailwindThemeBase::~TailwindThemeBase()
{ }

std::vector<Wt::WLinkedCssStyleSheet> TailwindThemeBase::styleSheets() const
{
  std::vector<Wt::WLinkedCssStyleSheet> result;

  if (!name_.empty()) {
    std::string themeDir = resourcesUrl();

    Wt::WApplication *app = Wt::WApplication::instance();

    result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink(themeDir + "wt.css")));

    if (app->environment().agentIsIElt(9))
      result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink(themeDir + "wt_ie.css")));

    if (app->environment().agent() == Wt::UserAgent::IE6)
      result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink(themeDir + "wt_ie6.css")));
  }

#ifdef DEBUG
  result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink("static/theme/tailwindcss/tailwind.css?v=" + Wt::WRandom::generateId())));
#else
  result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink("static/theme/tailwindcss/tailwind.minify.css?v=" + Wt::WRandom::generateId())));
#endif

  return result;
}

void TailwindThemeBase::apply(Wt::WWidget *widget, Wt::WWidget *child, int widgetRole) const
{
  if (!widget->isThemeStyleEnabled())
    return;

  switch (widgetRole) {
  case Wt::MenuItemIcon:
    child->addStyleClass("Wt-icon");
    break;
  case Wt::MenuItemCheckBox:
    child->addStyleClass("Wt-chkbox");
    break;
  case Wt::MenuItemClose:
    widget->addStyleClass("Wt-closable");
    child->addStyleClass("closeicon");
    break;

  case Wt::DialogCoverWidget:
    child->setStyleClass("Wt-dialogcover in");
    break;
  case Wt::DialogTitleBar:
    child->addStyleClass("titlebar");
    break;
  case Wt::DialogBody:
    child->addStyleClass("body");
    break;
  case Wt::DialogFooter:
    child->addStyleClass("footer");
    break;
  case Wt::DialogCloseIcon:
    child->addStyleClass("closeicon");
    break;

  case Wt::TableViewRowContainer:
    {
      Wt::WAbstractItemView *view = dynamic_cast<Wt::WAbstractItemView *>(widget);

      std::string backgroundImage;

      if (view->alternatingRowColors())
        backgroundImage = "stripes/stripe-";
      else
        backgroundImage = "no-stripes/no-stripe-";

      backgroundImage = resourcesUrl() + backgroundImage
        + std::to_string(static_cast<int>(view->rowHeight().toPixels()))
        + "px.gif";

      child->decorationStyle().setBackgroundImage(Wt::WLink(backgroundImage));

      break;
    }

  case Wt::DatePickerPopup:
    child->addStyleClass("Wt-datepicker");
    break;
  case Wt::DatePickerIcon:
    {
      auto icon = dynamic_cast<Wt::WImage*>(child);
      icon->setImageLink(Wt::WApplication::relativeResourcesUrl() + "date.gif");
      icon->setVerticalAlignment(Wt::AlignmentFlag::Middle);
      icon->resize(16, 16);
      break;
    }

  case Wt::PanelTitleBar:
    child->addStyleClass("titlebar");
    break;
  case Wt::PanelBody:
    child->addStyleClass("body");
    break;
  case Wt::PanelCollapseButton:
    child->setFloatSide(Wt::Side::Left);
    break;

  case Wt::AuthWidgets:
    Wt::WApplication *app = Wt::WApplication::instance();
    app->useStyleSheet(Wt::WApplication::relativeResourcesUrl() + "form.css");
    break;
  }
}

void TailwindThemeBase::apply(Wt::WWidget *widget, Wt::DomElement& element, int elementRole)
  const
{
  bool creating = element.mode() == Wt::DomElement::Mode::Create;

  if (!widget->isThemeStyleEnabled())
    return;

  {
    Wt::WPopupWidget *popup = dynamic_cast<Wt::WPopupWidget *>(widget);
    if (popup)
      element.addPropertyWord(Wt::Property::Class, "Wt-outset");
  }

  switch (element.type()) {
  case Wt::DomElementType::BUTTON:
    if (creating) {
      element.addPropertyWord(Wt::Property::Class, "rounded-md px-2.5 py-1.5 text-sm font-semibold ring-1 ring-inset cursor-pointer");
      Wt::WPushButton *b = dynamic_cast<Wt::WPushButton *>(widget);
      if (b) {
        if (b->isDefault())
          element.addPropertyWord(Wt::Property::Class, "Wt-btn-default");

        if (!b->text().empty())
          element.addPropertyWord(Wt::Property::Class, "with-label");
      }
    }
    break;

  case Wt::DomElementType::UL:
    if (dynamic_cast<Wt::WPopupMenu *>(widget))
      element.addPropertyWord(Wt::Property::Class, "Wt-popupmenu Wt-outset");
    else {
      Wt::WTabWidget *tabs
        = dynamic_cast<Wt::WTabWidget *>(widget->parent()->parent());

      if (tabs)
        element.addPropertyWord(Wt::Property::Class, "Wt-tabs");
      else {
        Wt::WSuggestionPopup *suggestions
          = dynamic_cast<Wt::WSuggestionPopup *>(widget);

        if (suggestions)
          element.addPropertyWord(Wt::Property::Class, "Wt-suggest");
      }
    }
    break;

  case Wt::DomElementType::LI:
    {
      Wt::WMenuItem *item = dynamic_cast<Wt::WMenuItem *>(widget);
      if (item) {
        if (item->isSeparator())
          element.addPropertyWord(Wt::Property::Class, "Wt-separator");
           if (item->isSectionHeader())
          element.addPropertyWord(Wt::Property::Class, "Wt-sectheader");
        if (item->menu())
          element.addPropertyWord(Wt::Property::Class, "submenu");
      }
    }
    break;

  case Wt::DomElementType::DIV:
    {
      Wt::WDialog *dialog = dynamic_cast<Wt::WDialog *>(widget);
      if (dialog) {
        element.addPropertyWord(Wt::Property::Class, "Wt-dialog");
        return;
      }

      Wt::WPanel *panel = dynamic_cast<Wt::WPanel *>(widget);
      if (panel) {
        element.addPropertyWord(Wt::Property::Class, "Wt-panel Wt-outset");
        return;
      }

      Wt::WProgressBar *bar = dynamic_cast<Wt::WProgressBar *>(widget);
      if (bar) {
        switch (elementRole) {
        case Wt::MainElement:
          element.addPropertyWord(Wt::Property::Class, "Wt-progressbar");
          break;
        case Wt::ProgressBarBar:
          element.addPropertyWord(Wt::Property::Class, "Wt-pgb-bar");
          break;
        case Wt::ProgressBarLabel:
          element.addPropertyWord(Wt::Property::Class, "Wt-pgb-label");
        }
        return;
      }
    }

    break;

  case Wt::DomElementType::INPUT:
    {
      Wt::WLineEdit *lineEdit = dynamic_cast<Wt::WLineEdit *>(widget);
      if (lineEdit) {
        element.addPropertyWord(Wt::Property::Class, "shadow appearance-none border rounded w-full py-2 px-3 leading-tight focus:outline-none focus:shadow-outline");
        return;
      }

      Wt::WAbstractSpinBox *spinBox = dynamic_cast<Wt::WAbstractSpinBox *>(widget);
      if (spinBox) {
        element.addPropertyWord(Wt::Property::Class, "Wt-spinbox");
        return;
      }

      Wt::WDateEdit *dateEdit = dynamic_cast<Wt::WDateEdit *>(widget);
      if (dateEdit) {
        element.addPropertyWord(Wt::Property::Class, "Wt-dateedit");
        return;
      }

      Wt::WTimeEdit *timeEdit = dynamic_cast<Wt::WTimeEdit *>(widget);
      if (timeEdit) {
        element.addPropertyWord(Wt::Property::Class, "Wt-timeedit");
        return;
      }
    }
    break;

  default:
    break;
  }
}

std::string TailwindThemeBase::disabledClass() const
{
  return "Wt-disabled";
}

std::string TailwindThemeBase::activeClass() const
{
  return "Wt-selected";
}

std::string TailwindThemeBase::utilityCssClass(int utilityCssClassRole) const
{
  switch (utilityCssClassRole) {
  case Wt::ToolTipOuter:
    return "Wt-tooltip";
  default:
    return "";
  }
}

std::string TailwindThemeBase::name() const
{
  return name_;
}

bool TailwindThemeBase::canStyleAnchorAsButton() const
{
  return false;
}

void TailwindThemeBase::applyValidationStyle(Wt::WWidget *widget,
                                     const Wt::WValidator::Result& validation,
                                     Wt::WFlags<Wt::ValidationStyleFlag> styles) const
{
  Wt::WApplication *app = Wt::WApplication::instance();

  LOAD_JAVASCRIPT(app, "js/CssThemeValidate.js", "validate", wtjs1);
  LOAD_JAVASCRIPT(app, "js/CssThemeValidate.js", "setValidationState", wtjs2);

  if (app->environment().ajax()) {
    Wt::WStringStream js;
    js << WT_CLASS ".setValidationState(" << widget->jsRef() << ","
       << (validation.state() == Wt::ValidationState::Valid) << ","
       << validation.message().jsStringLiteral() << ","
       << styles.value() << ");";

    widget->doJavaScript(js.str());
  } else {
    bool validStyle
      = (validation.state() == Wt::ValidationState::Valid) &&
      styles.test(Wt::ValidationStyleFlag::ValidStyle);
    bool invalidStyle
      = (validation.state() != Wt::ValidationState::Valid) &&
      styles.test(Wt::ValidationStyleFlag::InvalidStyle);

    widget->toggleStyleClass("Wt-valid", validStyle);
    widget->toggleStyleClass("Wt-invalid", invalidStyle);
  }
}

bool TailwindThemeBase::canBorderBoxElement(const Wt::DomElement& element) const
{
  return true;
}
