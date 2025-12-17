#pragma once

#include <Wt/WContainerWidget.h>
#include <functional>
#include <memory>

template <typename Function>
class DeferredWidget : public Wt::WContainerWidget
{
public:
    DeferredWidget(Function f)
        : f_(f), loaded_(false)
    {
        addStyleClass("deferred-widget"); // used for debugging when checking the doom in the browser
    }

protected:
    void load() override
    {
        if (!loaded_) {
            auto widget = f_();
            addWidget(std::move(widget));
            loaded_ = true;
        }
        Wt::WContainerWidget::load();
    }

private:
    Function f_;
    bool loaded_;
};

template <typename Function>
std::unique_ptr<DeferredWidget<Function>> deferCreate(Function f)
{
    return std::make_unique<DeferredWidget<Function>>(f);
}
