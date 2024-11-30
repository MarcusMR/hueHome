cmake_minimum_required(VERSION 3.10)

project(hueHome)

set(SOURCES
    src/hueHome.cpp
    include/control.cpp
)

set(INCLUDES
    include/control.h
)

find_package(CURL REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})

target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

target_include_directories(${PROJECT_NAME} PRIVATE include)
