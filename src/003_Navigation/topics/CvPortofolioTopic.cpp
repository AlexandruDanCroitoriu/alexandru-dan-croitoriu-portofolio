#include "003_Navigation/topics/CvPortofolioTopic.h"
#include "003_Navigation/DeferredWidget.h"

#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WRadioButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WApplication.h>

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

    // Chose tipe of CV/Portfolio to display (Coding and IT, All)
    auto typeSelector = container->addNew<Wt::WContainerWidget>();
    typeSelector->addStyleClass("mb-6 flex items-center space-x-4");
    auto buttonGroup = std::make_shared<Wt::WButtonGroup>();
    auto codingCvRadio = typeSelector->addNew<Wt::WRadioButton>("Coding and IT");
    
    auto allCvRadio = typeSelector->addNew<Wt::WRadioButton>("Full");
    buttonGroup->addButton(allCvRadio, 0);
    allCvRadio->addStyleClass("[&>input]:hidde text-white text-sm lg:text-md inline-block cursor-pointer");
    allCvRadio->addStyleClass("[&>input]:[&~span]:p-1");
    allCvRadio->addStyleClass("[&>input]:[&~span]:rounded-md");
    allCvRadio->addStyleClass("[&>input]:[&~span]:bg-gray-400");
    allCvRadio->addStyleClass("[&>input]:checked:[&~span]:bg-gray-700");

    codingCvRadio->setChecked(true);
    buttonGroup->addButton(codingCvRadio, 1);

    codingCvRadio->addStyleClass("[&>input]:hidde text-white text-sm lg:text-md inline-block cursor-pointer");
    codingCvRadio->addStyleClass("[&>input]:[&~span]:p-1");
    codingCvRadio->addStyleClass("[&>input]:[&~span]:rounded-md");
    codingCvRadio->addStyleClass("[&>input]:[&~span]:bg-gray-400");
    codingCvRadio->addStyleClass("[&>input]:checked:[&~span]:bg-gray-700");


    auto gridContainer = container->addNew<Wt::WContainerWidget>();

    std::string gridCellStyle = "bg-green-300 rounded-md shadow-md w-60 h-60";

    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    gridContainer->addNew<Wt::WContainerWidget>()->addStyleClass(gridCellStyle);
    
    buttonGroup->checkedChanged().connect([gridContainer, codingCvRadio, allCvRadio](Wt::WRadioButton* selectedRadioButton) {
        std::cout << "CvPortofolioTopic::cvPage() - Selected radio button changed" << std::endl;
        if (codingCvRadio == selectedRadioButton) {
            gridContainer->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit,minmax(200px,1fr))]");
        } else if (allCvRadio == selectedRadioButton) {
            gridContainer->setStyleClass("grid gap-4 grid-cols-[repeat(auto-fit,minmax(400px,1fr))]");
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
