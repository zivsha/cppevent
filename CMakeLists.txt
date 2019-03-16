cmake_minimum_required(VERSION 3.1)
project(cppevent VERSION 1.0.0 LANGUAGES CXX)

option(CPPEVENT_BUILD_DEMO "Build demo" ON)
option(CPPEVENT_BUILD_TESTS "Build tests" ON)

if(CPPEVENT_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if(CPPEVENT_BUILD_TESTS)
    add_subdirectory(tests)
endif()