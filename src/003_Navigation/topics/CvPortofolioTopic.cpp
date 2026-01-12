#include "003_Navigation/topics/CvPortofolioTopic.h"
#include "003_Navigation/DeferredWidget.h"

#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WRadioButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WImage.h>
#include <Wt/WApplication.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>

CvPortofolioTopic::CvPortofolioTopic()
{
    wApp->log("debug") << "CvPortofolioTopic::CvPortofolioTopic()";
}

std::unique_ptr<Wt::WWidget> CvPortofolioTopic::createCvPage()
{
    return deferCreateTopicPage([this]() { return cvPage(); });
}

std::unique_ptr<Wt::WWidget> CvPortofolioTopic::cvPage()
{
    wApp->log("debug") << "CvPortofolioTopic::cvPage()";
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("w-full max-w-5xl mx-auto space-y-2 p-6");

    // Title
    auto title = container->addNew<Wt::WText>("<h2 class='text-3xl font-bold text-gray-800 mb-4'>CV & Portfolio</h2>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Chose tipe of CV/Portfolio to display (Coding and IT, All)
    auto typeSelector = container->addNew<Wt::WContainerWidget>();
    typeSelector->addStyleClass("mb-6 flex items-center space-x-4");
    auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
    auto codingCvRadio = typeSelector->addNew<Wt::WRadioButton>("Coding and IT");
    
    auto allCvRadio = typeSelector->addNew<Wt::WRadioButton>("Full");
    buttonGroup->addButton(allCvRadio, 0);
    allCvRadio->addStyleClass("[&>input]:hidden text-white text-sm lg:text-md inline-block cursor-pointer");
    allCvRadio->addStyleClass("[&>input]:[&~span]:p-1");
    allCvRadio->addStyleClass("[&>input]:[&~span]:rounded-md");
    allCvRadio->addStyleClass("[&>input]:[&~span]:bg-gray-400");
    allCvRadio->addStyleClass("[&>input]:checked:[&~span]:bg-gray-700");

    codingCvRadio->setChecked(true);
    buttonGroup->addButton(codingCvRadio, 1);

    codingCvRadio->addStyleClass("[&>input]:hidden text-white text-sm lg:text-md inline-block cursor-pointer");
    codingCvRadio->addStyleClass("[&>input]:[&~span]:p-1");
    codingCvRadio->addStyleClass("[&>input]:[&~span]:rounded-md");
    codingCvRadio->addStyleClass("[&>input]:[&~span]:bg-gray-400");
    codingCvRadio->addStyleClass("[&>input]:checked:[&~span]:bg-gray-700");


    auto sectionsWrapper = container->addNew<Wt::WContainerWidget>();

    std::string gridCellStyle = "transition-opacity duration-700 ease-in-out ";
    gridCellStyle += "bg-green-300 rounded-md shadow-md w-full h-60 ";
    Wt::WString jsFunc =
    R"(
    const observer = new IntersectionObserver((entries)=>{
        entries.forEach((entry)=>{
            const viewportHeight = window.innerHeight || document.documentElement.clientHeight;
            const elementTop = entry.boundingClientRect.top;
            
            // Element is at bottom of screen if its top is in lower half of viewport
            const isAtBottom = elementTop >= viewportHeight * 0.5;
            
            if(entry.isIntersecting){
                // If element is visible and NOT at bottom (i.e., at top), add class immediately
                // If element is visible and at bottom (entering from scroll), add class
                if(!isAtBottom || isAtBottom){
                    entry.target.classList.add("!opacity-100");
                }
            } else if(isAtBottom) {
                // Only remove class when leaving from bottom area
                entry.target.classList.remove("!opacity-100");
            }
        });
    }, { 
        threshold: 0.1
    });
    )";

    // Growndswell Details Block
    {
    auto groundswellDetails = sectionsWrapper->addNew<Wt::WContainerWidget>();
    groundswellDetails->addStyleClass("rounded-md shadow-md w-full p-6 bg-white relative");
    auto groundsugroundswellellSkillRoleWrapper = groundswellDetails->addNew<Wt::WContainerWidget>();
    groundsugroundswellellSkillRoleWrapper->addStyleClass("flex items-center justify-between flex-wrap");
    auto growndswellRolesWrapper = groundsugroundswellellSkillRoleWrapper->addNew<Wt::WContainerWidget>();
    growndswellRolesWrapper->addStyleClass("flex items-start flex-wrap space-x-6 space-y-2");
    auto growndswellSkillsWrapper = groundsugroundswellellSkillRoleWrapper->addNew<Wt::WContainerWidget>();
    growndswellSkillsWrapper->addStyleClass("flex items-start flex-wrap space-x-2 space-y-2");

    auto etlDeveloperRole = growndswellRolesWrapper->addNew<Wt::WText>(
        "<div class='text-2xl font-bold text-gray-800'>ETL Developer</div>"
        "<div class='text-sm font-semibold text-gray-600'>Informatica PowerCenter</div>"
    );
    etlDeveloperRole->setTextFormat(Wt::TextFormat::XHTML);

    auto softwareDeveloperRole = growndswellRolesWrapper->addNew<Wt::WText>(
        "<div class='text-2xl font-bold text-gray-800'>Software Developer</div>"
        "<div class='text-sm font-semibold text-gray-600'>C++17 / Wt / Wt::Dbo / Ice </div>"
    );


    auto informaticaLogo = growndswellSkillsWrapper->addNew<Wt::WImage>("static/icons/informatica-logo.png");
    informaticaLogo->setStyleClass("h-10 border border-gray-300 rounded-md hover:shadow-lg hover:border-gray-400 transition-all hover:bg-gray-100 p-1");

    auto cppLogo = growndswellSkillsWrapper->addNew<Wt::WImage>("static/icons/cpp-logo.png");
    cppLogo->setStyleClass("h-10 border border-gray-300 rounded-md hover:shadow-lg hover:border-gray-400 transition-all hover:bg-gray-300 p-1");
    
    }


    
    
    for ( int i = 0; i < 10; ++i )
    {
        auto block = sectionsWrapper->addNew<Wt::WContainerWidget>();
        block->addStyleClass(gridCellStyle);
        // jsFunc += "observer.observe(document.getElementById('" + block->id() + "'));";
        jsFunc += block->jsRef() + ".classList.add('opacity-0');"; // Initial state
        jsFunc += "observer.observe(" + block->jsRef() + ");";
        // jsFunc += "console.log(" + block->jsRef() + ");";
    }

    sectionsWrapper->doJavaScript(jsFunc.toUTF8());
    
    buttonGroup->checkedChanged().connect([sectionsWrapper, codingCvRadio, allCvRadio](Wt::WRadioButton* selectedRadioButton) {
        if (codingCvRadio == selectedRadioButton) {
            sectionsWrapper->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit, 1fr)]");
        } else if (allCvRadio == selectedRadioButton) {
            sectionsWrapper->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit, 2fr)]");
        }else {
            wApp->log("error") << "CvPortofolioTopic::cvPage() - Unknown radio button selected";
        }
    });

    buttonGroup->checkedChanged().emit(buttonGroup->checkedButton());

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


    return container;
}
