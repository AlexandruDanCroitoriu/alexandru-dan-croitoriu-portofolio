#include "004_TailwindTheme/TailwindTheme.h"

#include <Wt/WLinkedCssStyleSheet.h>
#include <Wt/WLogger.h>

#include <Wt/WApplication.h>

#include <Wt/WIconPair.h>
#include <Wt/WAbstractItemView.h>
#include <Wt/WFormWidget.h>
#include <Wt/WText.h>
#include <Wt/WLabel.h>
#include <Wt/DomElement.h>
#include <Wt/WPopupWidget.h>
#include <Wt/WDialog.h>
#include <Wt/WWebWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WMenuItem.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPanel.h>
#include <Wt/WProgressBar.h>
#include <Wt/WNavigationBar.h>
#include <Wt/WGoogleMap.h>
#include <Wt/WRadioButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WFileUpload.h>
#include <Wt/WSlider.h>
#include <Wt/WInPlaceEdit.h>
#include <Wt/WAbstractSpinBox.h>
#include <Wt/WDateEdit.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WSuggestionPopup.h>
#include <Wt/WTabWidget.h>
#include <Wt/WDatePicker.h>

#include <Wt/WRandom.h>
#include <boost/algorithm/string.hpp>

namespace {
  const std::string btnClasses[] = {
    "navbar-toggler",
    "accordion-button"
  };
}

TailwindTheme::TailwindTheme()
{
    // wApp->log("debug") << "TailwindTheme::TailwindTheme()";
}

std::string TailwindTheme::name() const
{
    // wApp->log("debug") << "TailwindTheme::name()";
    return "tailwindcss";
}

std::string TailwindTheme::resourcesUrl() const
{
    // wApp->log("debug") << "TailwindTheme::resourcesUrl()";
    return "static/theme/" + name() + "/";
}

std::vector<Wt::WLinkedCssStyleSheet> TailwindTheme::styleSheets() const
{
    // wApp->log("debug") << "TailwindTheme::styleSheets()";
    std::vector<Wt::WLinkedCssStyleSheet> result;
    const std::string themeDir = resourcesUrl();

#ifdef DEBUG
    result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink(themeDir + "tailwind.css?v=" + Wt::WRandom::generateId())));
#else
    result.push_back(Wt::WLinkedCssStyleSheet(Wt::WLink(themeDir + "tailwind.minify.css")));
#endif

    return result;
}

void TailwindTheme::init(Wt::WApplication *app) const
{
    // wApp->log("debug") << "TailwindTheme::init(Wt::WApplication *app) ";
    //   app->builtinLocalizedStrings().useBuiltin(Wt::skeletons::BootstrapTheme_xml);
    //   app->builtinLocalizedStrings().useBuiltin(Wt::skeletons::Bootstrap5Theme_xml);
    //   app->require(resourcesUrl() + "bootstrap.bundle.min.js");
    //   LOAD_JAVASCRIPT(app, "js/Bootstrap5Theme.js", "theme", wtjs3);

    //! This is not working (check terminal)
    // Wt::WString v = app->metaHeader(Wt::MetaHeaderType::Meta, "viewport");
    // if (v.empty())
    // {
    //     app->addMetaHeader("viewport", "width=device-width, initial-scale=1");
    // }

    // Define setValidationState function inline for form validation
    std::string validationJs = WT_CLASS ".setValidationState=function(e,t,n,i,o,s){"
        "var a=0!==(1&i),l=0!==(2&i);if(t){e.classList.remove(s);a&&e.classList.add(o)}"
        "else{e.classList.remove(o);l&&e.classList.add(s)}"
        "if(n){if(0!==(4&i)){e.setAttribute('title',n)}}"
        "else{if(0!==(8&i)){e.removeAttribute('title')}}"
    "};";
    app->doJavaScript(validationJs);

    // Load Tailwind Plus Elements JavaScript as an ES module
    // WApplication::require() injects classic scripts only; Elements expects type="module".
    {
        const std::string src = resourcesUrl() + "tailwindplus/index.js";
        std::string js = "(function(){var s=document.createElement('script');";
        js += "s.type='module';";
        js += "s.src='"; js += src; js += "';";
        js += "document.head.appendChild(s);})();";
        app->doJavaScript(js);
    }
}

void TailwindTheme::apply(Wt::WWidget *widget, Wt::WWidget *child, int widgetRole) const
{
    // wApp->log("debug") << "TailwindTheme::apply(Wt::WWidget *widget, Wt::WWidget *child, int widgetRole)";
    if (!widget->isThemeStyleEnabled())
        return;

    switch (widgetRole)
    {
    case Wt::WidgetThemeRole::MenuItemIcon:
        child->addStyleClass("Wt-icon");
        break;

    case Wt::WidgetThemeRole::MenuItemCheckBox:
        child->addStyleClass("Wt-chkbox");
        ((Wt::WFormWidget *)child)->label()->addStyleClass("form-checkbox");
        break;

    case Wt::WidgetThemeRole::MenuItemClose:
        child->addStyleClass("Wt-close-icon");
        ((Wt::WText *)child)->setText("&times;");
        break;

    case Wt::WidgetThemeRole::DialogContent:
        child->addStyleClass("modal-content");
        break;

    case Wt::WidgetThemeRole::DialogCoverWidget:
        child->addStyleClass("modal-backdrop in");
        child->setAttributeValue("style", "opacity:0.5");
        break;

    case Wt::WidgetThemeRole::DialogTitleBar:
        child->addStyleClass("modal-header");
        break;

    case Wt::WidgetThemeRole::DialogBody:
        child->addStyleClass("modal-body");
        break;

    case Wt::WidgetThemeRole::DialogFooter:
        child->addStyleClass("modal-footer");
        break;

    case Wt::WidgetThemeRole::DialogCloseIcon:
        child->addStyleClass("btn-close");
        break;

    case Wt::WidgetThemeRole::TableViewRowContainer:
    {
        auto view = dynamic_cast<Wt::WAbstractItemView *>(widget);
        child->toggleStyleClass("Wt-striped", view->alternatingRowColors());
        break;
    }

    case Wt::WidgetThemeRole::DatePickerPopup:
        child->addStyleClass("Wt-datepicker");
        break;

    case Wt::WidgetThemeRole::DatePickerIcon:
    {
        auto icon = dynamic_cast<Wt::WImage *>(child);
        icon->setImageLink(resourcesUrl() + "calendar-date.svg");
        icon->setVerticalAlignment(Wt::AlignmentFlag::Middle);
        icon->resize(20, 20);
        icon->setMargin(5, Wt::Side::Left);
        icon->addStyleClass("Wt-datepicker-icon");
        break;
    }

    case Wt::WidgetThemeRole::TimePickerPopup:
        child->addStyleClass("Wt-timepicker");
        break;

    case Wt::WidgetThemeRole::PanelTitleBar:
        child->addStyleClass("card-header");
        break;

    case Wt::WidgetThemeRole::PanelBody:
        child->addStyleClass("card-body");
        break;

    case Wt::WidgetThemeRole::PanelCollapseButton:
    {
        auto app = wApp;
        auto iconPair = dynamic_cast<Wt::WIconPair *>(child);
        // this sets display: block, which makes sure the icons are aligned properly
        iconPair->icon1()->setInline(false);
        iconPair->icon1()->setImageLink(app->onePixelGifUrl());
        iconPair->icon2()->setInline(false);
        iconPair->icon2()->setImageLink(app->onePixelGifUrl());
        iconPair->addStyleClass("Wt-collapse-button");
        break;
    }

    case Wt::WidgetThemeRole::InPlaceEditing:
        child->addStyleClass("input-group");
        break;

    case Wt::WidgetThemeRole::InPlaceEditingButton:
        child->addStyleClass("btn-outline-secondary");
        break;

    case Wt::WidgetThemeRole::NavCollapse:
        child->addStyleClass("navbar-collapse collapse");
        break;

    case Wt::WidgetThemeRole::NavBrand:
        child->addStyleClass("navbar-brand");
        break;

    case Wt::WidgetThemeRole::NavbarSearchForm:
        child->addStyleClass("d-flex");
        break;

    case Wt::WidgetThemeRole::NavbarMenu:
        child->addStyleClass("navbar-nav");
        break;

    case Wt::WidgetThemeRole::NavbarBtn:
        child->addStyleClass("navbar-toggler");
        break;

    case Wt::WidgetThemeRole::TimePickerPopupContent:
        child->addStyleClass("d-flex");
        break;

    // Test to remove old BS formatting
    default:
        if (child->hasStyleClass("form-inline"))
        {
            child->removeStyleClass("form-inline");
            child->addStyleClass("row");
        }
        break;
    }
}

void TailwindTheme::apply(Wt::WWidget *widget, Wt::DomElement &element, int elementRole) const
{
    // wApp->log("debug") << "TailwindTheme::apply(Wt::WWidget *widget, Wt::DomElement &element, int elementRole)";
    bool creating = element.mode() == Wt::DomElement::Mode::Create;

    if (!widget->isThemeStyleEnabled())
        return;

    {
        auto popup = dynamic_cast<Wt::WPopupWidget *>(widget);
        if (popup)
        {
            auto dialog = dynamic_cast<Wt::WDialog *>(widget);
            if (!dialog)
            {
                element.addPropertyWord(Wt::Property::Class, "dropdown-menu");
            }
        }
    }

    switch (element.type())
    {

    case Wt::DomElementType::A:
    {
        if (creating && dynamic_cast<Wt::WPushButton *>(widget))
            element.addPropertyWord(Wt::Property::Class, classBtn(widget));

        auto btn = dynamic_cast<Wt::WPushButton *>(widget);
        if (creating && btn && btn->isDefault())
            element.addPropertyWord(Wt::Property::Class, "btn-primary");

        auto item = dynamic_cast<Wt::WMenuItem *>(widget->parent());
        if (item)
        {
            auto popupMenu = dynamic_cast<Wt::WPopupMenu *>(item->parentMenu());
            if (popupMenu)
            {
                element.addPropertyWord(Wt::Property::Class, "dropdown-item");
            }
            else
            {
                element.addPropertyWord(Wt::Property::Class, "nav-link");
            }
        }

        if (element.getAttribute("href").empty() &&
            element.getProperty(Wt::Property::Class).empty())
            element.addPropertyWord(Wt::Property::Class, "dropdown-item");

        break;
    }

    case Wt::DomElementType::BUTTON:
    {
        auto button = dynamic_cast<Wt::WPushButton *>(widget);

        if (button)
        {
            if (creating && button->isDefault())
                element.addPropertyWord(Wt::Property::Class, "btn btn-primary");

            else if (creating)
                element.addPropertyWord(Wt::Property::Class, classBtn(widget));

            if (!button->link().isNull())
                wApp->log("INFO-theme") << "Cannot use WPushButton::setLink() after the button has been rendered with WBootstrapTheme";
                // LOG_ERROR("Cannot use WPushButton::setLink() after the button has "
                        //   "been rendered with WBootstrapTheme");
        }
        break;
    }

    case Wt::DomElementType::DIV:
    {
        auto dialog = dynamic_cast<Wt::WDialog *>(widget);
        if (dialog)
        {
            element.addPropertyWord(Wt::Property::Class, "modal Wt-dialog");
            return;
        }

        auto panel = dynamic_cast<Wt::WPanel *>(widget);
        if (panel)
        {
            element.addPropertyWord(Wt::Property::Class, "card Wt-panel");
            return;
        }

        auto bar = dynamic_cast<Wt::WProgressBar *>(widget);
        if (bar)
        {
            switch (elementRole)
            {
            case Wt::MainElement:
                element.addPropertyWord(Wt::Property::Class, "progress");
                break;
            case Wt::ProgressBarBar:
                element.addPropertyWord(Wt::Property::Class, "progress-bar");
                break;
            }
            return;
        }

        auto map = dynamic_cast<Wt::WGoogleMap *>(widget);
        if (map)
        {
            element.addPropertyWord(Wt::Property::Class, "Wt-googlemap");
            return;
        }

        auto navBar = dynamic_cast<const Wt::WNavigationBar *>(widget);
        if (navBar)
        {
            element.addPropertyWord(Wt::Property::Class, "navbar");
            if (!hasNavbarExpandClass(navBar))
            {
                element.addPropertyWord(Wt::Property::Class, "navbar-expand-lg");
            }
            return;
        }
    }
    break;

    case Wt::DomElementType::LABEL:
    {
        if (elementRole == Wt::ToggleButtonRole)
        {
            auto cb = dynamic_cast<Wt::WCheckBox *>(widget);
            Wt::WRadioButton *rb = nullptr;

            if (cb)
                element.addPropertyWord(Wt::Property::Class, "form-check");
            else
            {
                rb = dynamic_cast<Wt::WRadioButton *>(widget);
                if (rb)
                    element.addPropertyWord(Wt::Property::Class, "form-check");
            }

            if ((cb || rb) && !widget->isInline())
                element.setType(Wt::DomElementType::DIV);
            else
                element.addPropertyWord(Wt::Property::Class, "form-check-inline");
        }
        else if (elementRole == Wt::FormLabel)
        {
            element.addPropertyWord(Wt::Property::Class, "form-file-label");
        }
    }
    break;

    case Wt::DomElementType::LI:
    {
        auto item = dynamic_cast<Wt::WMenuItem *>(widget);
        if (item)
        {
            const bool separator = item->isSeparator();
            const bool sectionHeader = item->isSectionHeader();
            if (separator)
                element.addPropertyWord(Wt::Property::Class, "dropdown-divider");
            if (!separator && !sectionHeader)
            {
                auto popupMenu = dynamic_cast<Wt::WPopupMenu *>(item->parentMenu());
                if (!popupMenu)
                {
                    element.addPropertyWord(Wt::Property::Class, "nav-item");
                }
            }
            if (item->menu())
            {
                if (dynamic_cast<Wt::WPopupMenu *>(item->parentMenu()))
                    element.addPropertyWord(Wt::Property::Class, "dropdown");
            }
        }
    }
    break;

    case Wt::DomElementType::INPUT:
    {
        if (elementRole == Wt::ToggleButtonInput)
        {
            element.addPropertyWord(Wt::Property::Class, "form-check-input");
            element.addPropertyWord(Wt::Property::Class, "Wt-chkbox");
            break;
        }
        else if (elementRole == Wt::FileUploadInput)
        {
            element.addPropertyWord(Wt::Property::Class, "form-control");
            break;
        }

        auto tb = dynamic_cast<Wt::WAbstractToggleButton *>(widget);
        auto sl = dynamic_cast<Wt::WSlider *>(widget);
        auto fu = dynamic_cast<Wt::WFileUpload *>(widget);
        if (!(tb || sl || fu))
            element.addPropertyWord(Wt::Property::Class, "block w-full rounded-md bg-white px-3 py-1.5 text-base text-gray-900 outline outline-1 -outline-offset-1 outline-gray-300 placeholder:text-gray-400 focus:outline focus:outline-2 focus:-outline-offset-2 focus:outline-indigo-600 sm:text-sm/6 dark:bg-white/5 dark:text-white dark:outline-white/10 dark:placeholder:text-gray-500 dark:focus:outline-indigo-500");
        else if (sl && !sl->nativeControl())
        {
            element.addPropertyWord(Wt::Property::Class, "form-range");

            if (sl->orientation() == Wt::Orientation::Vertical)
            {
                element.addPropertyWord(Wt::Property::Class, "Wt-native-vertical-slider");
            }
        }

        auto spinBox = dynamic_cast<Wt::WAbstractSpinBox *>(widget);
        if (spinBox)
        {
            element.addPropertyWord(Wt::Property::Class, "Wt-spinbox");
            return;
        }

        auto dateEdit = dynamic_cast<Wt::WDateEdit *>(widget);
        if (dateEdit)
        {
            element.addPropertyWord(Wt::Property::Class, "Wt-dateedit");
            return;
        }

        auto timeEdit = dynamic_cast<Wt::WTimeEdit *>(widget);
        if (timeEdit)
        {
            element.addPropertyWord(Wt::Property::Class, "Wt-timeedit");
            return;
        }
    }
    break;

    case Wt::DomElementType::SELECT:
        element.addPropertyWord(Wt::Property::Class, "form-select");
        break;

    case Wt::DomElementType::TEXTAREA:
        element.addPropertyWord(Wt::Property::Class, "form-control");
        break;

    case Wt::DomElementType::UL:
    {
        auto popupMenu = dynamic_cast<Wt::WPopupMenu *>(widget);
        if (popupMenu)
        {
            element.addPropertyWord(Wt::Property::Class, "dropdown-menu");

            if (popupMenu->parentItem() &&
                dynamic_cast<Wt::WPopupMenu *>(popupMenu->parentItem()->parentMenu()))
                element.addPropertyWord(Wt::Property::Class, "submenu");
        }
        else
        {
            auto menu = dynamic_cast<Wt::WMenu *>(widget);
            if (menu)
            {
                if (element.getProperty(Wt::Property::Class).find("navbar-nav") == std::string::npos)
                    element.addPropertyWord(Wt::Property::Class, "nav");

                auto tabs = dynamic_cast<Wt::WTabWidget *>(menu->parent()->parent());

                if (tabs)
                    element.addPropertyWord(Wt::Property::Class, "nav-tabs");
            }
            else
            {
                auto suggestions = dynamic_cast<Wt::WSuggestionPopup *>(widget);

                if (suggestions)
                    element.addPropertyWord(Wt::Property::Class, "typeahead");
            }
        }
        break;
    }

    case Wt::DomElementType::SPAN:
    {
        if (elementRole == Wt::ToggleButtonSpan)
            element.addPropertyWord(Wt::Property::Class, "form-check-label");
        else if (elementRole == Wt::FormText)
            element.addPropertyWord(Wt::Property::Class, "form-file-text");
        else if (elementRole == Wt::FormButton)
            element.addPropertyWord(Wt::Property::Class, "form-file-button");

        auto inPlaceEdit = dynamic_cast<Wt::WInPlaceEdit *>(widget);
        if (inPlaceEdit)
            element.addPropertyWord(Wt::Property::Class, "Wt-in-place-edit");
        else
        {
            auto picker = dynamic_cast<Wt::WDatePicker *>(widget);
            if (picker)
                element.addPropertyWord(Wt::Property::Class, "Wt-datepicker");
        }
    }
    break;

    case Wt::DomElementType::FORM:
        if (elementRole == Wt::FileUploadForm)
        {
            element.addPropertyWord(Wt::Property::Class, "input-group mb-2");

            // WWebWidget will grab the style class from the DOM element and apply it to the widget.
            // If we're using progressive bootstrap that means the form-control class previously applied to
            // the input gets applied to the form when enableAjax() is called. To counteract what I think
            // is mostly a hack in WWebWidget, we sadly have to add this hack.
            widget->removeStyleClass("form-control");
        }
        break;

    default:
        break;
    }
}


std::string TailwindTheme::disabledClass() const
{
    // wApp->log("debug") << "TailwindTheme::disabledClass()";
    return "disabled";
}

std::string TailwindTheme::activeClass() const
{
    // wApp->log("debug") << "TailwindTheme::activeClass()";
    return "active";
}

std::string TailwindTheme::utilityCssClass(int utilityCssClassRole) const
{
    // wApp->log("debug") << "TailwindTheme::utilityCssClass(int utilityCssClassRole)";
    switch (utilityCssClassRole) {
    case Wt::ToolTipInner:
        return "tooltip-inner";
    case Wt::ToolTipOuter:
        return "tooltip fade top in position-absolute";
    default:
        return "";
    }
}

bool TailwindTheme::canStyleAnchorAsButton() const
{
    // wApp->log("debug") << "TailwindTheme::canStyleAnchorAsButton()";
  return true;
}

void TailwindTheme::applyValidationStyle(Wt::WWidget *widget,
                                            const Wt::WValidator::Result& validation,
                                            Wt::WFlags<Wt::ValidationStyleFlag> styles) const
{
    // wApp->log("debug") << "TailwindTheme::applyValidationStyle(Wt::WWidget *widget, const Wt::WValidator::Result& validation, Wt::WFlags<Wt::ValidationStyleFlag> styles)";
  Wt::WApplication *app = Wt::WApplication::instance();

  if (app->environment().ajax()) {
    Wt::WStringStream js;
    js << WT_CLASS ".setValidationState(" << widget->jsRef() << ","
       << (validation.state() == Wt::ValidationState::Valid) << ","
       << validation.message().jsStringLiteral() << ","
       << styles.value() << ","
       << "'is-valid', 'is-invalid');";

    widget->doJavaScript(js.str());
  } else {
    bool validStyle
      = (validation.state() == Wt::ValidationState::Valid) &&
      styles.test(Wt::ValidationStyleFlag::ValidStyle);
    bool invalidStyle
      = (validation.state() != Wt::ValidationState::Valid) &&
      styles.test(Wt::ValidationStyleFlag::InvalidStyle);

    widget->toggleStyleClass("is-valid", validStyle);
    widget->toggleStyleClass("is-invalid", invalidStyle);
  }
}

bool TailwindTheme::canBorderBoxElement(const Wt::DomElement& element) const
{
  // Irrelevant, is used for old IE versions
    // wApp->log("debug") << "TailwindTheme::canBorderBoxElement(const Wt::DomElement& element)";
  return true;
}

Wt::Side TailwindTheme::panelCollapseIconSide() const
{
    // wApp->log("debug") << "TailwindTheme::panelCollapseIconSide()";
  return Wt::Side::Right;
}




// PRIVATE
std::string TailwindTheme::classBtn(const Wt::WWidget *widget)
{
    // wApp->log("debug") << "TailwindTheme::classBtn(const Wt::WWidget *widget)";
  auto button = dynamic_cast<const Wt::WPushButton *>(widget);
  return (hasButtonStyleClass(widget)
          || (button && button->isDefault())) ? "btn" : "rounded-md px-2.5 py-1.5 text-sm font-semibold text-white ring-1 ring-inset cursor-pointer";
}


bool TailwindTheme::hasButtonStyleClass(const Wt::WWidget *widget)
{
    // wApp->log("debug") << "TailwindTheme::hasButtonStyleClass(const Wt::WWidget *widget)";
#ifndef WT_TARGET_JAVA
  int size = sizeof(btnClasses)/sizeof(std::string);
#else
  int size = Utils::sizeofFunction(btnClasses);
#endif
  for (int i = 0; i < size; ++i) {
    if (widget->hasStyleClass(btnClasses[i])) {
      return true;
    }
  }
  const auto classesStr = widget->styleClass().toUTF8();
  std::vector<std::string> classes;
  boost::split(classes, classesStr, boost::is_any_of(" "));
  for (const auto &c : classes) {
    if (boost::starts_with(c, "btn-")) {
      return true;
    }
  }
  return false;
}

bool TailwindTheme::hasNavbarExpandClass(const Wt::WNavigationBar *navigationBar)
{
    // wApp->log("debug") << "TailwindTheme::hasNavbarExpandClass(const Wt::WNavigationBar *navigationBar)";
  const auto classesStr = navigationBar->styleClass().toUTF8();
  std::vector<std::string> classes;
  boost::split(classes, classesStr, boost::is_any_of(" "));
  for (const auto &c : classes) {
    if (boost::starts_with(c, "navbar-expand-")) {
      return true;
    }
  }
  return false;
}
