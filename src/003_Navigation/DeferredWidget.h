#pragma once

#include <Wt/WContainerWidget.h>
#include <functional>
#include <memory>
#include <Wt/WApplication.h>

template <typename Function>
class DeferredWidget : public Wt::WContainerWidget
{
public:
    DeferredWidget(Function f)
        : f_(f), loaded_(false)
    {
        wApp->log("debug") << "DeferredWidget::DeferredWidget(Function f)";
        addStyleClass("deferred-widget h-screen overflow-y-auto"); // used for debugging when checking the doom in the browser
        addStyleClass("[&::-webkit-scrollbar]:w-2.5 [&::-webkit-scrollbar-track]:bg-gray-500 [&::-webkit-scrollbar-thumb]:bg-gray-800 [&::-webkit-scrollbar-thumb]:rounded-full"); // custom scrollbar styles
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
    wApp->log("debug") << "deferCreate(Function f)";
    return std::make_unique<DeferredWidget<Function>>(f);
}
