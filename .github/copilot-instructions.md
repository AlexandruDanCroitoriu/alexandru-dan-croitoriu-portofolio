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

## UI Guidelines

- **Colors**: Dark sidebar (`bg-gray-700`), light content (`bg-gray-100`), dark code areas (`bg-slate-800`)
- **Components**: Rounded corners, subtle shadows, consistent spacing (`p-2`, `p-4`, `gap-4`)
- **Interactive**: Hover states, clear visual hierarchy, good contrast
