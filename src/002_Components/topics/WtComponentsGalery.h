#pragma once

#include "003_Navigation/NavigationTopic.h"
#include <Wt/WWidget.h>
#include <memory>
#include <string>

class WtComponentsGalery : public NavigationTopic
{
public:
    WtComponentsGalery();
    std::unique_ptr<Wt::WWidget> createWtComponentsGalery();

private:
    enum class ExamplePage { LineEdit = 0, TextArea = 1 };

    
    std::unique_ptr<Wt::WWidget> buildLineEditExample();
    std::unique_ptr<Wt::WWidget> buildTextAreaExample();

    ExamplePage exampleFromPath(const std::string& path) const;
    std::string pathForExample(ExamplePage example) const;

    std::string makeIncludeLine(const std::string& header, const std::string& docUrl) const;
    std::string makeMethodCall(const std::string& object, const std::string& docUrl, const std::string& method,
                               const std::string& args, const std::string& comment = "") const;
};