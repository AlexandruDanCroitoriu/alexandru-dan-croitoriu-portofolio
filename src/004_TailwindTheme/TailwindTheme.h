#pragma once

#include <Wt/WTheme.h>
#include "004_TailwindTheme/TailwindThemeBase.h"

// class TailwindTheme : public Wt::WTheme
class TailwindTheme : public TailwindThemeBase
{
public:
    /*! \brief Constructor.
    */
    TailwindTheme();

    /*! \brief Returns a theme name.
    *
    * Returns a unique name for the theme. This name is used by the
    * implementation of resourcesUrl() to compute a location for the theme's
    * resources.
    */
    std::string name() const override;

    /*! \brief Returns the URL where theme-related resources are stored.
     *
     * The implementation considers a folder within the
     * static/theme directory plus the theme name().
     */
    std::string resourcesUrl() const override;

    /*! \brief Returns a vector with stylesheets for the theme.
     *
     * This should return a vector with stylesheets that implement the
     * theme. This list may be tailored to the current user agent, which
     * is read from the application environment.
     */
    std::vector<Wt::WLinkedCssStyleSheet> styleSheets() const override;

    /*! \brief Called when the theme is assigned to a WApplication
     *
     * This allows the theme to do things like load resource bundles when it
     * is added to an application using WApplication::setTheme().
     */
    void init(Wt::WApplication *app) const override;

    /*! \brief Applies the theme to a child of a composite widget.
     *
     * The \p widgetRole indicates the role that \p child has within the
     * implementation of the \p widget.
     */
    void apply(Wt::WWidget *widget, Wt::WWidget *child, int widgetRole) const override;

    /*! \brief Applies the theme to a DOM element that renders a widget.
     *
     * The \p element is a rendered representation of the \p widget, and
     * may be further customized to reflect the theme.
     */
    void apply(Wt::WWidget *widget, Wt::DomElement &element, int elementRole) const override;

    /*! \brief Returns a generic CSS class name for a disabled element.
     */
    std::string disabledClass() const override;

    /*! \brief Returns a generic CSS class name for an active element.
     */
    std::string activeClass() const override;

    /*! \brief Returns a generic CSS class name for the chosen role.
     *
     * \sa WTheme::utilityCssClassRole
     */
    std::string utilityCssClass(int utilityCssClassRole) const override;

    /*! \brief Returns whether the theme allows for an anchor to be styled
     *         as a button.
     */
    bool canStyleAnchorAsButton() const override;

    /*! \brief Applies a style that indicates the result of validation.
     */
    void applyValidationStyle(Wt::WWidget *widget,
                              const Wt::WValidator::Result &validation,
                              Wt::WFlags<Wt::ValidationStyleFlag> flags) const override;

    /*! \brief Irrelevant, is used for old IE versions
     */
    bool canBorderBoxElement(const Wt::DomElement &element) const override;

    /*! \brief Returns which side the WPanel collapse icon should be added on
     *
     * Side is assumed to be Side::Left or Side::Right, other sides are not supported.
     */
    Wt::Side panelCollapseIconSide() const override;

private:
    static std::string classBtn(const Wt::WWidget *widget);
    static bool hasButtonStyleClass(const Wt::WWidget *widget);
    static bool hasNavbarExpandClass(const Wt::WNavigationBar *widget);
};