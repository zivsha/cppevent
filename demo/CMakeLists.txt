project(cppevent-demo CXX)

add_executable (cppevent-demo demo.cpp timer.hpp observable_queue.hpp)
target_include_directories(cppevent-demo PRIVATE ../include)