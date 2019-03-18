project(cppevent-tests CXX)
#set_property(TARGET cppevent-tests PROPERTY CXX_STANDARD 17)
add_executable (cppevent-tests main.cpp tests.cpp catch.hpp)
target_include_directories(cppevent-tests PRIVATE ../include)