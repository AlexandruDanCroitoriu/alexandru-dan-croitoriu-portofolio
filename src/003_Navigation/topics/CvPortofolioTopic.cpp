#include "003_Navigation/topics/CvPortofolioTopic.h"
#include "003_Navigation/DeferredWidget.h"

#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WPushButton.h>
#include <Wt/WRadioButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WImage.h>
#include <Wt/WApplication.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>
#include <Wt/WPanel.h>

#include <fstream>
#include <sstream>

CvPortofolioTopic::CvPortofolioTopic()
{
    wApp->log("debug") << "CvPortofolioTopic::CvPortofolioTopic()";
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/002_Cv/AboutProfile");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/002_Cv/FreelanceShopify");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/002_Cv/Groudswell");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/002_Cv/GcpApigee");
    wApp->messageResourceBundle().use(wApp->docRoot() + "/static/xml/002_Cv/Education");

}

std::unique_ptr<Wt::WWidget> CvPortofolioTopic::createCvPage()
{
    return deferCreateTopicPage([this]() { return cvPage(); });
}

std::unique_ptr<Wt::WWidget> CvPortofolioTopic::cvPage()
{
    wApp->log("debug") << "CvPortofolioTopic::cvPage()";
    auto container = std::make_unique<Wt::WContainerWidget>();
    container->addStyleClass("w-full space-y-2 p-2 pb-22 sm:p-6 h-screen overflow-y-auto");

    auto createReadmeBtn = container->addNew<Wt::WPushButton>("generate readme");
    createReadmeBtn->clicked().connect([this](){
        std::cout << "CvPortofolioTopic::createReadmeBtn clicked" << std::endl;

        Wt::WString content =
        "# Alexandru Dan Croitoriu\n\n"
        "## About Me\n"
        + Wt::WString::tr("Cv.About.summary") + "\n\n"
        "- " + Wt::WString::tr("Cv.About.skill-cloud") + "\n"
        "- " + Wt::WString::tr("Cv.About.skill-backend") + "\n"
        "- " + Wt::WString::tr("Cv.About.skill-frontend") + "\n"
        "- " + Wt::WString::tr("Cv.About.skill-cicd") + "\n"
        "- " + Wt::WString::tr("Cv.About.skill-dwh") + "\n"
        "- " + Wt::WString::tr("Cv.About.skill-communication") + "\n\n"
        "## links:\n"
        "[Linkedin](" + Wt::WString::tr("Cv.About.link-linkedin-url") + ")<br>\n"
        "[GitHub](" + Wt::WString::tr("Cv.About.link-github-url") + ")\n\n"
        "## **Languages:**\n"
        " - " + Wt::WString::tr("Cv.About.language-1") + "\n"
        " - " + Wt::WString::tr("Cv.About.language-2") + "\n\n"
        "## Work Experience\n\n"
        "### " + Wt::WString::tr("Cv.Shopify.title") + " - " + Wt::WString::tr("Cv.Shopify.role") + "\n"
        + Wt::WString::tr("Cv.Shopify.period-start") + " - " + Wt::WString::tr("Cv.Shopify.period-end") + "\n\n"
        + Wt::WString::tr("Cv.Shopify.summary") + "\n\n"
        + Wt::WString::tr("Cv.Shopify.responsibilities") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-1") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-2") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-3") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-4") + "\n\n"
        + Wt::WString::tr("Cv.Shopify.gained-exp") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.exp-summary") + "\n";

        content += "\n\n### " + Wt::WString::tr("Cv.GcpApigee.title") + " - " + Wt::WString::tr("Cv.GcpApigee.role") + "\n"
        + Wt::WString::tr("Cv.GcpApigee.period-start") + " - " + Wt::WString::tr("Cv.GcpApigee.period-end") + "\n\n"
        + Wt::WString::tr("Cv.GcpApigee.summary") + "\n\n"
        + Wt::WString::tr("Cv.GcpApigee.responsibilities") + "\n"
        "- " + Wt::WString::tr("Cv.GcpApigee.responsibility-1") + "\n"
        "- " + Wt::WString::tr("Cv.GcpApigee.responsibility-2") + "\n"
        "- " + Wt::WString::tr("Cv.GcpApigee.responsibility-3") + "\n"
        "- " + Wt::WString::tr("Cv.GcpApigee.responsibility-4") + "\n"
        "- " + Wt::WString::tr("Cv.GcpApigee.responsibility-5") + "\n\n"
        + Wt::WString::tr("Cv.GcpApigee.gained-exp") + "\n"
        "- " + Wt::WString::tr("Cv.GcpApigee.exp-summary") + "\n";

        content += "\n\n### " + Wt::WString::tr("Cv.Shopify.title") + " - " + Wt::WString::tr("Cv.Shopify.role") + "\n"
        + Wt::WString::tr("Cv.ShopifyPrevious.period-start") + " - " + Wt::WString::tr("Cv.ShopifyPrevious.period-end") + "\n\n"
        + Wt::WString::tr("Cv.Shopify.summary") + "\n\n"
        + Wt::WString::tr("Cv.Shopify.responsibilities") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-1") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-2") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-3") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.responsibility-4") + "\n\n"
        + Wt::WString::tr("Cv.Shopify.gained-exp") + "\n"
        "- " + Wt::WString::tr("Cv.Shopify.exp-summary") + "\n";

        content += "\n\n### " + Wt::WString::tr("Cv.Groudswell.title") + " - " + Wt::WString::tr("Cv.Groudswell.role") + "\n"
        + Wt::WString::tr("Cv.Groudswell.period-start") + " - " + Wt::WString::tr("Cv.Groudswell.period-end") + "\n\n"
        + Wt::WString::tr("Cv.Groudswell.summary") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.core-stack-wt") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.core-stack-ice") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.core-stack-dbo") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.core-stack-linux") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.core-stack-jenkins") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.core-stack-ci") + "\n\n"
        + Wt::WString::tr("Cv.Groudswell.gained-exp") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.exp-1") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.exp-2") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.exp-3") + "\n"
        "- " + Wt::WString::tr("Cv.Groudswell.exp-4") + "\n";

        content += "\n\n## Education\n\n"
        "### " + Wt::WString::tr("Cv.Education.wellcode.title") + "\n"
        + Wt::WString::tr("Cv.Education.wellcode.period") + "\n"
        "- " + Wt::WString::tr("Cv.Education.wellcode.detail-1") + "\n"
        "- " + Wt::WString::tr("Cv.Education.wellcode.detail-2") + "\n"
        "- " + Wt::WString::tr("Cv.Education.wellcode.detail-3") + "\n\n"
        "### " + Wt::WString::tr("Cv.Education.edmond.title") + "\n"
        + Wt::WString::tr("Cv.Education.edmond.period") + "\n";
        

        // write the readme file to disk
        std::ofstream readmeFile("../../generated_README.md");
        if (readmeFile.is_open()) {
            readmeFile << content.toUTF8();
            readmeFile.close();
            std::cout << "README file generated successfully.\n";
        } else {
            std::cerr << "Unable to open file for writing.\n";
        }
    });
    
    
    auto animation = Wt::WAnimation(Wt::AnimationEffect::SlideInFromBottom, Wt::TimingFunction::EaseInOut, 500);

    auto aboutSection = container->addNew<Wt::WContainerWidget>();
    aboutSection->addStyleClass("mx-auto max-w-5xl mt-2");
    aboutSection->addNew<Wt::WTemplate>(Wt::WString::tr("Cv.About.job-content"))->addFunction("tr", &Wt::WTemplate::Functions::tr);

    auto workExperienceTitle = container->addNew<Wt::WText>("<h2 class='mx-auto mt-6 mb-2 max-w-5xl text-2xl font-semibold tracking-tight text-slate-900'>Work Experiance</h2>");
    workExperienceTitle->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    auto timeline = container->addNew<Wt::WContainerWidget>();
    timeline->setHtmlTagName("ol");
    timeline->addStyleClass("mx-auto max-w-5xl relative space-y-6 before:absolute before:-ms-px before:h-full before:w-0.5 before:rounded-full before:bg-gray-200");

    auto addTimelinePanel = [&](std::unique_ptr<Wt::WPanel> panel, const Wt::WString& period, bool isLast = false) {
        auto li = timeline->addNew<Wt::WContainerWidget>();
        li->setHtmlTagName("li");
        li->addStyleClass("relative -ms-1.5 flex items-start gap-4" + std::string(isLast ? " mb-0" : ""));
        auto dot = li->addNew<Wt::WContainerWidget>();
        dot->setHtmlTagName("span");
        dot->addStyleClass("mt-3 size-3 shrink-0 rounded-full bg-blue-600");
        auto wrapper = li->addNew<Wt::WContainerWidget>();
        wrapper->addStyleClass("w-full");
        auto timeEl = wrapper->addNew<Wt::WText>("<time class=\"text-xs font-medium text-gray-500\">" + period.toUTF8() + "</time>");
        timeEl->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        wrapper->addWidget(std::move(panel));
    };

    auto addTimelinePoint = [&](const Wt::WString& period) {
        auto li = timeline->addNew<Wt::WContainerWidget>();
        li->setHtmlTagName("li");
        li->addStyleClass("relative -ms-1.5 flex items-start gap-4");
        auto dot = li->addNew<Wt::WContainerWidget>();
        dot->setHtmlTagName("span");
        dot->addStyleClass("mt-3 size-3 shrink-0 rounded-full bg-blue-600");
        auto wrapper = li->addNew<Wt::WContainerWidget>();
        wrapper->addStyleClass("w-full pb-1");
        auto timeEl = wrapper->addNew<Wt::WText>("<time class=\"text-xs font-medium text-gray-500\">" + period.toUTF8() + "</time>");
        timeEl->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    };


    // shopify
    {
        auto panel = std::make_unique<Wt::WPanel>();
        panel->setThemeStyleEnabled(false);
        panel->addStyleClass("rounded-3xl border border-slate-200 bg-white shadow-sm ml-[-15px] mr-[15px]");
        panel->setAnimation(animation);
        panel->setTitleBar(true);
        panel->setCollapsible(true);
        panel->setCollapsed(true);
        panel->setCentralWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("Cv.Shopify.job-content")))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addNew<Wt::WTemplate>(Wt::WString::tr("Cv.Shopify.job-header"))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addStyleClass("border-b border-slate-200 bg-slate-50 px-5 py-4 sm:px-6 rounded-3xl cursor-pointer");
        addTimelinePanel(std::move(panel), Wt::WString::tr("Cv.Shopify.period-end"));
    }

    // gcp apigee
    {
        auto panel = std::make_unique<Wt::WPanel>();
        panel->setThemeStyleEnabled(false);
        panel->addStyleClass("rounded-3xl border border-slate-200 bg-white shadow-sm ml-[-15px] mr-[15px]");
        panel->setAnimation(animation);
        panel->setTitleBar(true);
        panel->setCollapsible(true);
        panel->setCollapsed(true);
        panel->setCentralWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("Cv.GcpApigee.job-content")))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addNew<Wt::WTemplate>(Wt::WString::tr("Cv.GcpApigee.job-header"))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addStyleClass("border-b border-slate-200 bg-slate-50 px-5 py-4 sm:px-6 rounded-3xl cursor-pointer");
        addTimelinePanel(std::move(panel), Wt::WString::tr("Cv.GcpApigee.period-end"));
    }

    // shopify previous
    {
        auto panel = std::make_unique<Wt::WPanel>();
        panel->setThemeStyleEnabled(false);
        panel->addStyleClass("rounded-3xl border border-slate-200 bg-white shadow-sm ml-[-15px] mr-[15px]");
        panel->setAnimation(animation);
        panel->setTitleBar(true);
        panel->setCollapsible(true);
        panel->setCollapsed(true);
        panel->setCentralWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("Cv.Shopify.job-content")))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addNew<Wt::WTemplate>(Wt::WString::tr("Cv.ShopifyPrevious.job-header"))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addStyleClass("border-b border-slate-200 bg-slate-50 px-5 py-4 sm:px-6 rounded-3xl cursor-pointer");
        addTimelinePanel(std::move(panel), Wt::WString::tr("Cv.ShopifyPrevious.period-end"));
    }

    // groudswell
    {
        auto panel = std::make_unique<Wt::WPanel>();
        panel->setThemeStyleEnabled(false);
        panel->addStyleClass("rounded-3xl border border-slate-200 bg-white shadow-sm ml-[-15px] mr-[15px]");
        panel->setAnimation(animation);
        panel->setTitleBar(true);
        panel->setCollapsible(true);
        panel->setCollapsed(true);
        panel->setCentralWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("Cv.Groudswell.job-content")))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addNew<Wt::WTemplate>(Wt::WString::tr("Cv.Groudswell.job-header"))->addFunction("tr", &Wt::WTemplate::Functions::tr);
        panel->titleBarWidget()->addStyleClass("border-b border-slate-200 bg-slate-50 px-5 py-4 sm:px-6 rounded-3xl cursor-pointer");
        addTimelinePanel(std::move(panel), Wt::WString::tr("Cv.Groudswell.period-end"), true);
    }

    addTimelinePoint(Wt::WString::tr("Cv.Groudswell.period-start"));

    auto educationTitle = container->addNew<Wt::WText>("<h2 class='mx-auto mt-6 mb-2 max-w-5xl text-2xl font-semibold tracking-tight text-slate-900'>Education</h2>");
    educationTitle->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    auto educationSection = container->addNew<Wt::WContainerWidget>();
    educationSection->addStyleClass("mx-auto max-w-5xl");
    educationSection->addNew<Wt::WTemplate>(Wt::WString::tr("Cv.Education.job-content"))->addFunction("tr", &Wt::WTemplate::Functions::tr);



    // // Chose tipe of CV/Portfolio to display (Coding and IT, All)
    // auto typeSelector = container->addNew<Wt::WContainerWidget>();
    // typeSelector->addStyleClass("mb-6 flex items-center space-x-4");
    // auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
    // auto codingCvRadio = typeSelector->addNew<Wt::WRadioButton>("Coding and IT");
    
    // auto allCvRadio = typeSelector->addNew<Wt::WRadioButton>("Full");
    // buttonGroup->addButton(allCvRadio, 0);
    // allCvRadio->addStyleClass("[&>input]:hidden text-white text-sm lg:text-md inline-block cursor-pointer");
    // allCvRadio->addStyleClass("[&>input]:[&~span]:p-1");
    // allCvRadio->addStyleClass("[&>input]:[&~span]:rounded-md");
    // allCvRadio->addStyleClass("[&>input]:[&~span]:bg-gray-400");
    // allCvRadio->addStyleClass("[&>input]:checked:[&~span]:bg-gray-700");

    // codingCvRadio->setChecked(true);
    // buttonGroup->addButton(codingCvRadio, 1);

    // codingCvRadio->addStyleClass("[&>input]:hidden text-white text-sm lg:text-md inline-block cursor-pointer");
    // codingCvRadio->addStyleClass("[&>input]:[&~span]:p-1");
    // codingCvRadio->addStyleClass("[&>input]:[&~span]:rounded-md");
    // codingCvRadio->addStyleClass("[&>input]:[&~span]:bg-gray-400");
    // codingCvRadio->addStyleClass("[&>input]:checked:[&~span]:bg-gray-700");


    // auto sectionsWrapper = container->addNew<Wt::WContainerWidget>();

    // std::string gridCellStyle = "transition-opacity duration-700 ease-in-out ";
    // gridCellStyle += "bg-green-300 rounded-md shadow-md w-full h-60 ";
    // Wt::WString jsFunc =
    // R"(
    // const observer = new IntersectionObserver((entries)=>{
    //     entries.forEach((entry)=>{
    //         const viewportHeight = window.innerHeight || document.documentElement.clientHeight;
    //         const elementTop = entry.boundingClientRect.top;
            
    //         // Element is at bottom of screen if its top is in lower half of viewport
    //         const isAtBottom = elementTop >= viewportHeight * 0.5;
            
    //         if(entry.isIntersecting){
    //             // If element is visible and NOT at bottom (i.e., at top), add class immediately
    //             // If element is visible and at bottom (entering from scroll), add class
    //             if(!isAtBottom || isAtBottom){
    //                 entry.target.classList.add("!opacity-100");
    //             }
    //         } else if(isAtBottom) {
    //             // Only remove class when leaving from bottom area
    //             entry.target.classList.remove("!opacity-100");
    //         }
    //     });
    // }, { 
    //     threshold: 0.1
    // });
    // )";

    // // Growndswell Details Block
    // {
    // auto groundswellDetails = sectionsWrapper->addNew<Wt::WContainerWidget>();
    // groundswellDetails->addStyleClass("rounded-md shadow-md w-full p-6 bg-white relative");
    // auto groundsugroundswellellSkillRoleWrapper = groundswellDetails->addNew<Wt::WContainerWidget>();
    // groundsugroundswellellSkillRoleWrapper->addStyleClass("flex items-center justify-between flex-wrap");
    // auto growndswellRolesWrapper = groundsugroundswellellSkillRoleWrapper->addNew<Wt::WContainerWidget>();
    // growndswellRolesWrapper->addStyleClass("flex items-start flex-wrap space-x-6 space-y-2");
    // auto growndswellSkillsWrapper = groundsugroundswellellSkillRoleWrapper->addNew<Wt::WContainerWidget>();
    // growndswellSkillsWrapper->addStyleClass("flex items-start flex-wrap space-x-2 space-y-2");
    // auto etlDeveloperRole = growndswellRolesWrapper->addNew<Wt::WText>(
    //     "<div class='text-2xl font-bold text-gray-800'>ETL Developer</div>"
    //     "<div class='text-sm font-semibold text-gray-600'>Informatica PowerCenter</div>"
    // );
    // etlDeveloperRole->setTextFormat(Wt::TextFormat::XHTML);
    // auto softwareDeveloperRole = growndswellRolesWrapper->addNew<Wt::WText>(
    //     "<div class='text-2xl font-bold text-gray-800'>Software Developer</div>"
    //     "<div class='text-sm font-semibold text-gray-600'>C++17 / Wt / Wt::Dbo / Ice </div>"
    // );
    // auto informaticaLogo = growndswellSkillsWrapper->addNew<Wt::WImage>("static/icons/informatica-logo.png");
    // informaticaLogo->setStyleClass("h-10 border border-gray-300 rounded-md hover:shadow-lg hover:border-gray-400 transition-all hover:bg-gray-100 p-1");
    // auto cppLogo = growndswellSkillsWrapper->addNew<Wt::WImage>("static/icons/cpp-logo.png");
    // cppLogo->setStyleClass("h-10 border border-gray-300 rounded-md hover:shadow-lg hover:border-gray-400 transition-all hover:bg-gray-300 p-1");
    // }


    // for ( int i = 0; i < 10; ++i )
    // {
    //     auto block = sectionsWrapper->addNew<Wt::WContainerWidget>();
    //     block->addStyleClass(gridCellStyle);
    //     // jsFunc += "observer.observe(document.getElementById('" + block->id() + "'));";
    //     jsFunc += block->jsRef() + ".classList.add('opacity-0');"; // Initial state
    //     jsFunc += "observer.observe(" + block->jsRef() + ");";
    //     // jsFunc += "console.log(" + block->jsRef() + ");";
    // }

    // sectionsWrapper->doJavaScript(jsFunc.toUTF8());
    // sectionsWrapper->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit, 1fr)]");

    // buttonGroup->checkedChanged().connect([sectionsWrapper, codingCvRadio, allCvRadio](Wt::WRadioButton* selectedRadioButton) {
    //     if (codingCvRadio == selectedRadioButton) {
    //         sectionsWrapper->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit, 1fr)]");
    //     } else if (allCvRadio == selectedRadioButton) {
    //         sectionsWrapper->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit, 2fr)]");
    //     }else {
    //         wApp->log("error") << "CvPortofolioTopic::cvPage() - Unknown radio button selected";
    //     }
    // });

    // buttonGroup->checkedChanged().emit(buttonGroup->checkedButton());


    return container;
}
