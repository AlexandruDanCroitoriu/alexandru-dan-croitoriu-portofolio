# PersonalGalery Implementation Plan

## 🎯 Implementation Progress

### ✅ Completed Tasks (Core + Components)

**Core Structure** - ✅ COMPLETE
- ✅ Created directory structure: `src/005_PersonalGalery/` and `src/005_PersonalGalery/topics/`
- ✅ Implemented `GalleryTopic.h` and `GalleryTopic.cpp` (base class)
- ✅ Implemented `PersonalGalery.h` and `PersonalGalery.cpp` (main widget with menu structure)
- ✅ Implemented `DeferredWidget.h` (template helper for lazy loading)

**Components Navigation** - ✅ COMPLETE
- ✅ Replaced prior topics with a single `ComponentsTopic`
- ✅ Menu item: **Components → Monaco Editor** (live Monaco editor demo)
- ✅ Wired PersonalGalery to use only ComponentsTopic
- ✅ Updated CMake to include ComponentsTopic

### 📋 Remaining Tasks

**Integration & Testing** - 🔄 NEXT
- ⬜ Build and test the application
- ⬜ Verify navigation works (Components → Monaco Editor)
- ⬜ Test responsive behavior (desktop and mobile)
- ⬜ Verify TailwindCSS styling is applied correctly

### 📁 Files Created / Updated

```
src/005_PersonalGalery/
├── PersonalGalery.h              ✅
├── PersonalGalery.cpp            ✅
├── GalleryTopic.h                ✅
├── GalleryTopic.cpp              ✅
├── DeferredWidget.h              ✅
└── topics/
    ├── ComponentsTopic.h         ✅
    └── ComponentsTopic.cpp       ✅
```

---

## Overview
This document provides a detailed implementation plan for the **PersonalGalery** widget, based on the navigation architecture from the `0-example-widgetgallery` application. The PersonalGalery will use TailwindCSS for styling and will implement a hierarchical menu structure with deferred content loading.

## Architecture Analysis

### Key Components from Example App

1. **WidgetGallery (Main Container)**
   - Extends `BaseTemplate` which extends `Wt::WTemplate`
   - Uses a WTemplate with XML template definition
   - Contains:
     - `WStackedWidget` for content area
     - Top-level `WMenu` with submenu items
     - Mobile-responsive menu toggle button
     - Menu overlay/cover for mobile

2. **Topic (Base Class for Content Categories)**
   - Base class for all content categories
   - Has a `populateSubMenu()` method to add submenu items
   - Each topic can have multiple sub-pages

3. **Navigation Structure**
   - Two-level menu hierarchy:
     - **Top-level menu**: Main categories (Layout, Forms, Navigation, etc.)
     - **Sub-menu**: Specific examples within each category
   - Internal path support for deep linking
   - Deferred widget loading (content created only when selected)

4. **DeferredWidget Pattern**
   - Widgets are created only when first accessed
   - Improves initial load time
   - Uses lambda functions to defer creation

5. **TopicTemplate**
   - Template-based content rendering
   - Binds example widgets to template placeholders
   - Includes XML templates for content structure

### Navigation Flow

```
WidgetGallery (Main)
├── WStackedWidget (content area)
├── WMenu (top-level)
│   ├── MenuItem "Layout" → SubMenu
│   │   ├── "HBoxLayout"
│   │   ├── "VBoxLayout"
│   │   └── ...
│   ├── MenuItem "Forms" → SubMenu
│   ├── MenuItem "Navigation" → SubMenu
│   │   ├── "Internal paths"
│   │   ├── "Anchor"
│   │   ├── "Stacked widget"
│   │   ├── "Menu"
│   │   └── ...
│   └── ...
└── Toggle Button (mobile menu)
```

## PersonalGalery Design

### Proposed Structure

```
PersonalGalery (Main Widget)
├── WContainerWidget (sidebar/menu area)
│   ├── Title/Logo area
│   ├── WMenu (main navigation)
│   │   └── "Components" → SubMenu
│   │       └── "Monaco Editor" (live demo page)
│   └── Menu Toggle Button
├── WStackedWidget (content display area)
└── WContainerWidget (overlay for mobile)
```

### TailwindCSS Styling Approach

Since the example uses custom CSS classes, we'll replace them with TailwindCSS utilities:

- **Sidebar**: `flex flex-col w-64 bg-gray-800 text-white min-h-screen`
- **Menu Items**: `px-4 py-2 hover:bg-gray-700 cursor-pointer transition-colors`
- **Content Area**: `flex-1 p-6 bg-gray-50`
- **Mobile Menu Toggle**: `md:hidden fixed top-4 right-4 z-50`
- **Responsive**: Use Tailwind's responsive prefixes (`sm:`, `md:`, `lg:`)

## Implementation Tasks

### Phase 1: Core Structure Setup

#### Task 1.1: Create Base Files
**Priority**: High  
**Estimated Time**: 30 minutes

**Files to create:**
- `src/005_PersonalGalery/PersonalGalery.h`
- `src/005_PersonalGalery/PersonalGalery.cpp`
- `src/005_PersonalGalery/GalleryTopic.h`
- `src/005_PersonalGalery/GalleryTopic.cpp`

**PersonalGalery.h structure:**
```cpp
#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include <Wt/WPushButton.h>

class GalleryTopic;

class PersonalGalery : public Wt::WContainerWidget
{
public:
    PersonalGalery();

private:
    Wt::WStackedWidget* contentsStack_;
    Wt::WMenu* mainMenu_;
    Wt::WPushButton* menuToggleButton_;
    Wt::WContainerWidget* sidebar_;
    Wt::WContainerWidget* contentsCover_;
    bool menuOpen_;

    Wt::WMenuItem* addToMenu(Wt::WMenu* menu,
                             const Wt::WString& name,
                             std::unique_ptr<GalleryTopic> topic);
    void toggleMenu();
    void openMenu();
    void closeMenu();
    void setupLayout();
    void setupStyling();
};
```

**GalleryTopic.h structure:**
```cpp
#pragma once

#include <Wt/WObject.h>
#include <Wt/WString.h>

namespace Wt {
    class WMenu;
}

class GalleryTopic : public Wt::WObject
{
public:
    GalleryTopic();
    virtual ~GalleryTopic() = default;

    virtual void populateSubMenu(Wt::WMenu* menu) = 0;
};
```

#### Task 1.2: Implement PersonalGalery Constructor
**Priority**: High  
**Estimated Time**: 1 hour

**Implementation details:**
```cpp
PersonalGalery::PersonalGalery()
    : menuOpen_(false)
{
    // Apply main container styling
    addStyleClass("flex flex-row min-h-screen bg-gray-100");

    // Create sidebar
    sidebar_ = addNew<Wt::WContainerWidget>();
    sidebar_->addStyleClass("w-64 bg-gray-800 text-white flex flex-col transition-all duration-300");
    sidebar_->addStyleClass("md:block hidden"); // Hidden on mobile by default

    // Create logo/title area
    auto logoArea = sidebar_->addNew<Wt::WContainerWidget>();
    logoArea->addStyleClass("p-6 border-b border-gray-700");
    auto title = logoArea->addNew<Wt::WText>("<h1 class='text-2xl font-bold'>Personal Gallery</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Create main content area container
    auto contentArea = addNew<Wt::WContainerWidget>();
    contentArea->addStyleClass("flex-1 flex flex-col");

    // Create mobile menu toggle button
    menuToggleButton_ = contentArea->addNew<Wt::WPushButton>();
    menuToggleButton_->addStyleClass("md:hidden fixed top-4 right-4 z-50 bg-gray-800 text-white px-4 py-2 rounded-lg");
    menuToggleButton_->setText("☰ Menu");
    menuToggleButton_->clicked().connect(this, &PersonalGalery::toggleMenu);

    // Create stacked widget for content
    contentsStack_ = contentArea->addNew<Wt::WStackedWidget>();
    contentsStack_->addStyleClass("flex-1 p-6");

    // Create menu
    mainMenu_ = sidebar_->addNew<Wt::WMenu>(contentsStack_);
    mainMenu_->addStyleClass("flex flex-col space-y-1 p-2");
    mainMenu_->setInternalPathEnabled();
    mainMenu_->setInternalBasePath("/gallery");

    // Create overlay for mobile
    contentsCover_ = addNew<Wt::WContainerWidget>();
    contentsCover_->addStyleClass("fixed inset-0 bg-black bg-opacity-50 hidden z-40");
    contentsCover_->clicked().connect(this, &PersonalGalery::closeMenu);

    // Add transition animation
    Wt::WAnimation animation(Wt::AnimationEffect::Fade,
                            Wt::TimingFunction::Linear,
                            200);
    contentsStack_->setTransitionAnimation(animation, true);

    // Populate menu with topics
    // (Will be implemented in Phase 2)
}
```

#### Task 1.3: Implement Menu Toggle Logic
**Priority**: High  
**Estimated Time**: 30 minutes

**Functions to implement:**
```cpp
void PersonalGalery::toggleMenu()
{
    if (menuOpen_) {
        closeMenu();
    } else {
        openMenu();
    }
}

void PersonalGalery::openMenu()
{
    if (menuOpen_) return;
    
    sidebar_->removeStyleClass("hidden");
    sidebar_->addStyleClass("block");
    contentsCover_->removeStyleClass("hidden");
    menuToggleButton_->setText("✕ Close");
    
    menuOpen_ = true;
}

void PersonalGalery::closeMenu()
{
    if (!menuOpen_) return;
    
    sidebar_->addStyleClass("md:block");
    sidebar_->addStyleClass("hidden");
    sidebar_->removeStyleClass("block");
    contentsCover_->addStyleClass("hidden");
    menuToggleButton_->setText("☰ Menu");
    
    menuOpen_ = false;
}

Wt::WMenuItem* PersonalGalery::addToMenu(Wt::WMenu* menu,
                                         const Wt::WString& name,
                                         std::unique_ptr<GalleryTopic> topic)
{
    auto topicPtr = addChild(std::move(topic));
    
    // Create submenu
    auto subMenuPtr = std::make_unique<Wt::WMenu>(contentsStack_);
    auto subMenu = subMenuPtr.get();
    
    // Create menu item with submenu
    auto itemPtr = std::make_unique<Wt::WMenuItem>(name);
    itemPtr->setMenu(std::move(subMenuPtr));
    
    auto item = menu->addItem(std::move(itemPtr));
    
    // Style submenu
    subMenu->addStyleClass("flex flex-col space-y-1 pl-4");
    
    // Close mobile menu when submenu item selected
    subMenu->itemSelected().connect(this, &PersonalGalery::closeMenu);
    
    // Enable internal paths
    subMenu->setInternalPathEnabled("/" + item->pathComponent());
    
    // Populate submenu
    topicPtr->populateSubMenu(subMenu);
    
    return item;
}
```

### Phase 2: Components Topic

#### Task 2.1: Create ComponentsTopic
**Priority**: High  
**Estimated Time**: 45 minutes

**Files:**
- `src/005_PersonalGalery/topics/ComponentsTopic.h`
- `src/005_PersonalGalery/topics/ComponentsTopic.cpp`

**Structure:**
```cpp
// ComponentsTopic.h
#pragma once
#include "../GalleryTopic.h"

class ComponentsTopic : public GalleryTopic
{
public:
    ComponentsTopic();
    void populateSubMenu(Wt::WMenu* menu) override;

private:
    std::unique_ptr<Wt::WWidget> monacoEditorDemo();
};

// ComponentsTopic.cpp
#include "ComponentsTopic.h"
#include "../DeferredWidget.h"
#include "002_Components/MonacoEditor.h"

ComponentsTopic::ComponentsTopic() {}

void ComponentsTopic::populateSubMenu(Wt::WMenu* menu)
{
    menu->addItem("Monaco Editor",
                  deferCreate([this]{ return monacoEditorDemo(); }));
}

std::unique_ptr<Wt::WWidget> ComponentsTopic::monacoEditorDemo()
{
    auto container = std::make_unique<Wt::WContainerWidget>();
    // add Tailwind utility classes and embed MonacoEditor
    return container;
}
```

### Phase 3: Menu Styling with TailwindCSS

#### Task 3.1: Style Main Menu Items
**Priority**: High  
**Estimated Time**: 30 minutes

**CSS Classes to apply:**
```cpp
// In PersonalGalery constructor, after creating menu items:
void PersonalGalery::setupMenuStyling()
{
    // Main menu item styling
    const std::string menuItemClass = 
        "px-4 py-3 rounded-lg transition-all duration-200 "
        "hover:bg-gray-700 cursor-pointer text-gray-200 "
        "hover:text-white font-medium";
    
    // Active menu item
    const std::string activeItemClass = 
        "bg-gray-700 text-white font-semibold";
    
    // Apply to all menu items programmatically or via CSS injection
}
```

#### Task 3.2: Style Submenu Items
**Priority**: High  
**Estimated Time**: 30 minutes

**CSS Classes:**
```cpp
const std::string subMenuItemClass = 
    "px-3 py-2 rounded-md text-sm transition-all duration-200 "
    "hover:bg-gray-600 cursor-pointer text-gray-300 "
    "hover:text-white border-l-2 border-transparent "
    "hover:border-blue-400";

const std::string activeSubItemClass = 
    "bg-gray-600 text-white border-blue-400";
```

#### Task 3.3: Responsive Design
**Priority**: High  
**Estimated Time**: 1 hour

**Breakpoints:**
- Mobile (< 768px): Hidden sidebar, toggle button visible
- Tablet/Desktop (≥ 768px): Visible sidebar, toggle button hidden

**Implementation:**
```cpp
// Sidebar classes
sidebar_->addStyleClass(
    "fixed md:static inset-y-0 left-0 z-50 "
    "w-64 md:w-64 transform md:transform-none "
    "transition-transform duration-300 ease-in-out "
    "-translate-x-full md:translate-x-0"
);

// When menu opens on mobile
sidebar_->removeStyleClass("-translate-x-full");

// When menu closes on mobile
sidebar_->addStyleClass("-translate-x-full");
```

### Phase 4: Deferred Loading Implementation

#### Task 4.1: Create DeferredWidget Helper
**Priority**: Medium  
**Estimated Time**: 30 minutes

**File:** `src/005_PersonalGalery/DeferredWidget.h`

```cpp
#pragma once

#include <Wt/WContainerWidget.h>
#include <functional>

template <typename Function>
class DeferredWidget : public Wt::WContainerWidget
{
public:
    DeferredWidget(Function f)
        : f_(f) 
    {
        addStyleClass("deferred-widget");
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
    bool loaded_ = false;
};

template <typename Function>
std::unique_ptr<DeferredWidget<Function>> deferCreate(Function f)
{
    return std::make_unique<DeferredWidget<Function>>(f);
}
```

#### Task 4.2: Apply Deferred Loading to Topics
**Priority**: Low  
**Estimated Time**: 1 hour

**Modify populateSubMenu methods:**
```cpp
void ProjectsTopic::populateSubMenu(Wt::WMenu* menu)
{
    menu->addItem("Web Applications",
                  deferCreate([this]{ return webApplications(); }));
    menu->addItem("Data Visualization",
                  deferCreate([this]{ return dataVisualization(); }));
    menu->addItem("Open Source",
                  deferCreate([this]{ return openSource(); }));
}
```

### Phase 5: Integration & Testing

#### Task 5.1: Update CMakeLists.txt
**Priority**: High  
**Estimated Time**: 15 minutes

Add PersonalGalery sources to build:
```cmake
# In appropriate CMakeLists.txt
add_library(PersonalGalery
    src/005_PersonalGalery/PersonalGalery.cpp
    src/005_PersonalGalery/GalleryTopic.cpp
    src/005_PersonalGalery/topics/ProjectsTopic.cpp
    src/005_PersonalGalery/topics/SkillsTopic.cpp
    src/005_PersonalGalery/topics/ExperienceTopic.cpp
    src/005_PersonalGalery/topics/AboutTopic.cpp
)
```

#### Task 5.2: Integrate into Main Application
**Priority**: High  
**Estimated Time**: 30 minutes

**Add to WidgetDisplay or main app:**
```cpp
#include "005_PersonalGalery/PersonalGalery.h"

// In WidgetDisplay or appropriate place:
auto gallery = addNew<PersonalGalery>();
```

#### Task 5.3: Test Navigation Flow
**Priority**: High  
**Estimated Time**: 1 hour

**Test cases:**
- ✓ Menu items clickable
- ✓ Submenu items display correct content
- ✓ Internal paths work correctly
- ✓ Mobile menu toggle works
- ✓ Overlay closes menu
- ✓ Transitions smooth
- ✓ Responsive behavior correct

#### Task 5.4: Test Styling
**Priority**: High  
**Estimated Time**: 45 minutes

**Test cases:**
- ✓ TailwindCSS classes applied
- ✓ Hover states work
- ✓ Active states highlighted
- ✓ Mobile responsive
- ✓ Color scheme consistent

## Detailed File Structure

```
src/005_PersonalGalery/
├── PersonalGalery.h              # Main gallery widget header
├── PersonalGalery.cpp            # Main gallery widget implementation
├── GalleryTopic.h                # Base topic class header
├── GalleryTopic.cpp              # Base topic class implementation
├── DeferredWidget.h              # Deferred loading helper
├── topics/
│   ├── ProjectsTopic.h           # Projects category header
│   ├── ProjectsTopic.cpp         # Projects category implementation
│   ├── SkillsTopic.h            # Skills category header
│   ├── SkillsTopic.cpp          # Skills category implementation
│   ├── ExperienceTopic.h        # Experience category header
│   ├── ExperienceTopic.cpp      # Experience category implementation
│   ├── AboutTopic.h             # About category header
│   └── AboutTopic.cpp           # About category implementation
└── README.md                     # Documentation
```

## TailwindCSS Class Reference

### Layout Classes
- **Container**: `flex`, `flex-col`, `flex-row`, `min-h-screen`
- **Sidebar**: `w-64`, `bg-gray-800`, `text-white`, `fixed`, `md:static`
- **Content**: `flex-1`, `p-6`, `max-w-4xl`, `mx-auto`

### Typography Classes
- **Headings**: `text-3xl`, `text-2xl`, `font-bold`, `font-semibold`
- **Body**: `text-gray-700`, `leading-relaxed`, `text-sm`

### Interactive Classes
- **Buttons**: `px-4`, `py-2`, `rounded-lg`, `cursor-pointer`
- **Hover**: `hover:bg-gray-700`, `hover:text-white`
- **Transitions**: `transition-all`, `duration-200`, `duration-300`

### Spacing Classes
- **Padding**: `p-2`, `p-4`, `p-6`, `px-3`, `py-2`
- **Margin**: `mb-4`, `mt-4`, `mx-auto`
- **Gap**: `space-y-1`, `space-y-4`

### Responsive Classes
- **Breakpoints**: `md:block`, `md:hidden`, `md:w-64`, `md:static`
- **Mobile**: `fixed`, `inset-0`, `z-50`, `transform`

### Color Scheme
- **Background**: `bg-gray-50`, `bg-gray-100`, `bg-gray-600`, `bg-gray-700`, `bg-gray-800`
- **Text**: `text-white`, `text-gray-200`, `text-gray-300`, `text-gray-700`
- **Accent**: `border-blue-400`, `bg-blue-500`

## Implementation Checklist

### Core Structure
- [ ] Create PersonalGalery.h and PersonalGalery.cpp
- [ ] Create GalleryTopic.h and GalleryTopic.cpp
- [ ] Implement menu toggle functionality
- [ ] Setup WStackedWidget for content
- [ ] Configure internal paths

### Components Topic
- [ ] Create ComponentsTopic with Monaco Editor demo page
- [ ] Add lorem ipsum / descriptive text for demo

### Styling
- [ ] Apply TailwindCSS to sidebar
- [ ] Apply TailwindCSS to menu items
- [ ] Apply TailwindCSS to content area
- [ ] Implement responsive design
- [ ] Add hover and active states
- [ ] Add transitions and animations

### Advanced Features
- [ ] Implement DeferredWidget
- [ ] Apply deferred loading to all topics
- [ ] Configure animations for content transitions

### Integration
- [ ] Update CMakeLists.txt
- [ ] Integrate into main application
- [ ] Test all navigation paths
- [ ] Test responsive behavior
- [ ] Verify styling consistency

## Estimated Total Time
- **Phase 1 (Core Structure)**: 2 hours
- **Phase 2 (Topics)**: 3 hours
- **Phase 3 (Styling)**: 2 hours
- **Phase 4 (Deferred Loading)**: 1.5 hours
- **Phase 5 (Integration & Testing)**: 2.5 hours

**Total**: ~11 hours

## Dependencies
- Wt library (WContainerWidget, WMenu, WStackedWidget, WText, WPushButton)
- TailwindCSS (already configured in project)
- C++14 or later (for std::unique_ptr, lambdas)

## Notes
- The example app uses XML templates for content, but we'll use direct widget creation for simplicity
- Mobile-first responsive design with hamburger menu
- Internal path support enables deep linking and browser history
- Deferred loading improves initial page load performance
- All styling done via TailwindCSS utility classes
- Lorem ipsum text as placeholder content for all pages

## Future Enhancements
- Add search functionality
- Implement breadcrumb navigation
- Add keyboard navigation support
- Add animation effects for page transitions
- Create custom icons for menu items
- Add dark/light theme toggle
- Implement user preferences storage
