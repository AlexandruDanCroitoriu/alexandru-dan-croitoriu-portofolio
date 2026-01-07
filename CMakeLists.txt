cmake_minimum_required(VERSION 3.13...3.22)
project(alexandru_dan_croitoriu_portofolio)

# Set number of parallel build jobs to number of available processors
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
  set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Define DEBUG macro for Debug build type and RELEASE for Release builds.
if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_compile_definitions(DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  set(WT_CONFIG_FILE "../../wt_config_debug.xml")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  add_compile_definitions(RELEASE)
  set(WT_CONFIG_FILE "../../wt_config_release.xml")
else()
  set(WT_CONFIG_FILE "../../wt_config.xml")
endif()

set(RLIB --docroot ../../ -c ${WT_CONFIG_FILE} --http-address 0.0.0.0 --http-port 9020)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/)


include(FetchContent)

# Try to use a system tinyxml2; if unavailable, fetch it from source.
find_package(tinyxml2 CONFIG QUIET)
if(tinyxml2_FOUND)
  set(TINYXML2_TARGET tinyxml2::tinyxml2)
else()
  message(STATUS "tinyxml2 not found locally; fetching via FetchContent")
  FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 10.0.0
  )
  FetchContent_MakeAvailable(tinyxml2)
  set(TINYXML2_TARGET tinyxml2)
endif()



set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/000_Server/Server.cpp
    ${SOURCE_DIR}/001_App/App.cpp
    
    ${SOURCE_DIR}/002_Components/MonacoEditor.cpp
    ${SOURCE_DIR}/002_Components/topics/MonacoTopic.cpp

    # Navigation sources
    ${SOURCE_DIR}/003_Navigation/Navigation.cpp
    ${SOURCE_DIR}/003_Navigation/NavigationTopic.cpp
    ${SOURCE_DIR}/003_Navigation/topics/CvPortofolioTopic.cpp
    ${SOURCE_DIR}/003_Navigation/topics/NotFoundTopic.cpp
    ${SOURCE_DIR}/003_Navigation/topics/NotAuthorizedTopic.cpp

    ${SOURCE_DIR}/004_TailwindTheme/TailwindTheme.cpp
    ${SOURCE_DIR}/004_TailwindTheme/TailwindThemeBase.cpp
      
    ${SOURCE_DIR}/005_Dbo/Session.cpp
    ${SOURCE_DIR}/005_Dbo/Tables/User.cpp
    ${SOURCE_DIR}/005_Dbo/Tables/Permission.cpp
    ${SOURCE_DIR}/005_Dbo/Tables/Post.cpp
    ${SOURCE_DIR}/005_Dbo/Tables/Comment.cpp
    ${SOURCE_DIR}/005_Dbo/Tables/Tag.cpp

    
    ${SOURCE_DIR}/006_Auth/AuthWidget.cpp
    ${SOURCE_DIR}/006_Auth/RegistrationView.cpp
    ${SOURCE_DIR}/006_Auth/UserDetailsModel.cpp
    ${SOURCE_DIR}/006_Auth/topics/UserSettingsTopic.cpp

    # Blog views
    ${SOURCE_DIR}/007_Blog/BlogView.cpp
    ${SOURCE_DIR}/007_Blog/PostView.cpp

    # Blog topics
    ${SOURCE_DIR}/007_Blog/topics/BlogTopic.cpp
    ${SOURCE_DIR}/007_Blog/topics/NewPostTopic.cpp
    ${SOURCE_DIR}/007_Blog/topics/EditPostTopic.cpp
    ${SOURCE_DIR}/007_Blog/topics/PostDetailTopic.cpp

    # Stylus
    ${SOURCE_DIR}/008_Stylus/Stylus.cpp
    ${SOURCE_DIR}/008_Stylus/StylusState.cpp
    ${SOURCE_DIR}/008_Stylus/StylusSession.cpp
    ${SOURCE_DIR}/008_Stylus/Tables/MessageTemplate.cpp
    ${SOURCE_DIR}/008_Stylus/Tables/TemplateFile.cpp
    ${SOURCE_DIR}/008_Stylus/Tables/TemplateFolder.cpp
    ${SOURCE_DIR}/008_Stylus/Utils/XmlBrain.cpp
    ${SOURCE_DIR}/008_Stylus/Utils/FilesManager.cpp
    ${SOURCE_DIR}/008_Stylus/TemplatesManager/RootNode.cpp
    ${SOURCE_DIR}/008_Stylus/TemplatesManager/FolderNode.cpp
    ${SOURCE_DIR}/008_Stylus/TemplatesManager/FileNode.cpp
    ${SOURCE_DIR}/008_Stylus/TemplatesManager/TemplateNode.cpp
    ${SOURCE_DIR}/008_Stylus/TemplatesManager/DboTempTreeView.cpp

)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
    wthttp
    wt
    wtdbo
    wtdbosqlite3
  ${TINYXML2_TARGET}
    # boost_regex
    # cpr::cpr
    # nlohmann_json::nlohmann_json
    # whisper
)

# Additionally, for Release builds, also link Postgres backend
if(CMAKE_BUILD_TYPE MATCHES Release)
  add_compile_definitions(WT_DBO_POSTGRES)
  # Find Postgres Dbo backend library (name differs across distros)
  find_library(WTDBO_POSTGRES_LIB
    NAMES wtdbopostgres wtdbo_postgres
    PATHS /usr/local/lib /usr/lib
  )
  if(WTDBO_POSTGRES_LIB)
    target_link_libraries(${PROJECT_NAME} ${WTDBO_POSTGRES_LIB})
  else()
    message(FATAL_ERROR "Wt Dbo Postgres backend library not found (searched for wtdbopostgres/wtdbo_postgres)")
  endif()
  # Link libpq if available (some builds require explicit linkage)
  find_library(POSTGRESQL_LIB NAMES pq PATHS /usr/local/lib /usr/lib)
  if(POSTGRESQL_LIB)
    target_link_libraries(${PROJECT_NAME} ${POSTGRESQL_LIB})
  endif()
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${RLIB}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)