#include "003_Navigation/topics/CvPortofolioTopic.h"
#include "003_Navigation/DeferredWidget.h"

#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WMenu.h>

CvPortofolioTopic::CvPortofolioTopic()
{
}

std::unique_ptr<Wt::WWidget> CvPortofolioTopic::createCvPage()
{
    return deferCreate([this]() { return cvPage(); });
}

std::unique_ptr<Wt::WWidget> CvPortofolioTopic::cvPage()
{
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("w-full max-w-5xl mx-auto space-y-2 p-6");

    // Title
    auto title = container->addNew<Wt::WText>("<h2 class='text-3xl font-bold text-gray-800 mb-4'>CV & Portfolio</h2>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Personal Info Section
    auto infoSection = container->addNew<Wt::WContainerWidget>();
    infoSection->addStyleClass("bg-white rounded-lg shadow-md p-6 mb-4");
    
    auto infoContent = infoSection->addNew<Wt::WText>(
        "<h3 class='text-2xl font-semibold text-gray-800 mb-3'>Alexandru Dan Croitoriu</h3>"
        "<p class='text-gray-700 mb-2'><strong>Email:</strong> your.email@example.com</p>"
        "<p class='text-gray-700 mb-2'><strong>Location:</strong> Your Location</p>"
        "<p class='text-gray-700 mb-4'><strong>LinkedIn:</strong> <a href='#' class='text-blue-600 hover:underline'>linkedin.com/in/yourprofile</a></p>"
    );
    infoContent->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // About Section
    auto aboutSection = container->addNew<Wt::WContainerWidget>();
    aboutSection->addStyleClass("bg-white rounded-lg shadow-md p-6 mb-4");
    
    auto aboutContent = aboutSection->addNew<Wt::WText>(
        "<h3 class='text-2xl font-semibold text-gray-800 mb-3'>About Me</h3>"
        "<p class='text-gray-700 leading-relaxed'>"
        "Experienced C++ developer with expertise in web frameworks, particularly Wt. "
        "Passionate about building modern, responsive web applications with clean architecture."
        "</p>"
    );
    aboutContent->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Experience Section
    auto expSection = container->addNew<Wt::WContainerWidget>();
    expSection->addStyleClass("bg-white rounded-lg shadow-md p-6 mb-4");
    
    auto expContent = expSection->addNew<Wt::WText>(
        "<h3 class='text-2xl font-semibold text-gray-800 mb-3'>Experience</h3>"
        "<div class='mb-4'>"
        "  <h4 class='text-xl font-medium text-gray-800'>Your Position</h4>"
        "  <p class='text-gray-600 italic mb-2'>Company Name • Year - Present</p>"
        "  <ul class='list-disc list-inside text-gray-700 space-y-1'>"
        "    <li>Developed web applications using C++ and Wt framework</li>"
        "    <li>Implemented responsive UI components with Tailwind CSS</li>"
        "    <li>Built and maintained CI/CD pipelines</li>"
        "  </ul>"
        "</div>"
    );
    expContent->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Skills Section
    auto skillsSection = container->addNew<Wt::WContainerWidget>();
    skillsSection->addStyleClass("bg-white rounded-lg shadow-md p-6 mb-4");
    
    auto skillsContent = skillsSection->addNew<Wt::WText>(
        "<h3 class='text-2xl font-semibold text-gray-800 mb-3'>Skills</h3>"
        "<div class='grid grid-cols-2 gap-4'>"
        "  <div>"
        "    <h4 class='font-medium text-gray-800 mb-2'>Programming Languages</h4>"
        "    <ul class='list-disc list-inside text-gray-700'>"
        "      <li>C++</li>"
        "      <li>JavaScript/TypeScript</li>"
        "      <li>Python</li>"
        "    </ul>"
        "  </div>"
        "  <div>"
        "    <h4 class='font-medium text-gray-800 mb-2'>Technologies</h4>"
        "    <ul class='list-disc list-inside text-gray-700'>"
        "      <li>Wt Framework</li>"
        "      <li>CMake</li>"
        "      <li>Tailwind CSS</li>"
        "      <li>Docker</li>"
        "    </ul>"
        "  </div>"
        "</div>"
    );
    skillsContent->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    return container;
}
