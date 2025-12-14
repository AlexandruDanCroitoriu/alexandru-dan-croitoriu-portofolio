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


# FetchContent_Declare(
#     tinyxml2
#     GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
#     GIT_TAG 10.0.0  # Use latest stable version
# )
# FetchContent_MakeAvailable(tinyxml2)



set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/000_Server/Server.cpp
    ${SOURCE_DIR}/001_App/App.cpp
    
    ${SOURCE_DIR}/002_Components/MonacoEditor.cpp

    ${SOURCE_DIR}/003_WidgetDisplay/WidgetDisplay.cpp

)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
    wthttp
    wt
    wtdbo
    # tinyxml2::tinyxml2
    # boost_regex
    # cpr::cpr
    # nlohmann_json::nlohmann_json
    # whisper
)

if(CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(${PROJECT_NAME} wtdbopostgres)
else()
    target_link_libraries(${PROJECT_NAME} wtdbosqlite3)
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${RLIB}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)