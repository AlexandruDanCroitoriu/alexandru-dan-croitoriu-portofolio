# Project Overview

Personal portfolio with:
- Main page: personal info and CV at `/`
- Component gallery at `/components/<name>`
- Wt framework wt-examples at `/wt-examples/<name>`
- Blog at `/blog` and articles at `/blog/<name>`

## Structure

- `/src`: C++ source code (folders prefixed: `001_`, `002_`, etc.)
- `/resources`: Wt library files (**⚠️ DO NOT EDIT**)
- `/static`: CSS, JavaScript, images
- `/build/debug` & `/build/release`: Build artifacts
- `CMakeLists.txt`: Build configuration

## Tech Stack

- **Wt**: C++ web framework
- **CMake**: Build system
- **Boost**: C++ libraries
- **Tailwind CSS**: Styling
- **SQLite/PostgreSQL**: Database
- **Docker**: Deployment

## Build Commands

**Debug Build:**
```bash
mkdir -p build/debug && cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -j$(nproc)
make run  # Starts server on http://0.0.0.0:9020
```

**Release Build:**
```bash
mkdir -p build/release && cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j$(nproc)
make run
```

**Tailwind CSS:**
```bash
cd static/stylus/tailwind
npm run watch   # Development (auto-recompile)
npm run minify  # Production (minified output)
```

## Code Style

- **Language**: C++17 standard
- **Headers**: Use `#pragma once` (not include guards)
- **Includes**: 
  - Project headers: quotes (`"003_Navigation/Navigation.h"`)
  - System/Wt headers: angles (`<Wt/WApplication.h>`)
- **Naming Conventions**:
  - Classes: PascalCase (`Navigation`, `AuthWidget`)
  - Methods: camelCase (`createApp()`, `toggleMenu()`)
  - Private members: camelCase with trailing underscore (`session_`, `menuOpen_`)
- **Folder Organization**: Source folders numbered (`000_Server/`, `001_App/`, `002_Components/`, etc.)
- **Smart Pointers**: Use `std::shared_ptr` and `std::unique_ptr` appropriately
- **Wt Patterns**:
  - Create child widgets: `addNew<T>()`
  - Apply styles: `addStyleClass("tailwind-classes")`
  - Templates: Use `Wt::WTemplate` with XML resources
- **Database**: SQLite for debug builds, PostgreSQL for release builds

## UI Guidelines

- **Colors**: Dark sidebar (`bg-gray-800`), light content (`bg-gray-100`), dark code areas (`bg-slate-800`)
- **Components**: Rounded corners, subtle shadows, consistent spacing (`p-2`, `p-4`, `gap-4`)
- **Interactive**: Hover states, clear visual hierarchy, good contrast
- **Responsive**: Mobile-first with Tailwind breakpoints (`md:`, `lg:`)
- **WPushButton**: Use Tailwind classes for light and dark themed buttons as
 - **Light** bg-gray-50/10 hover:bg-gray-50/20 ring-gray-50/5
 - **Dark** bg-slate-700 hover:bg-slate-800 ring-gray-50/10

