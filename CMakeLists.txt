cmake_minimum_required(VERSION 3.13)
project(tbpsim)
set(CMAKE_CXX_STANDARD 20)

include_directories(cfw)

find_package(X11 REQUIRED)

set(SOURCE_FILES main.cpp)

add_executable(tbpsim ${SOURCE_FILES})
target_link_libraries(tbpsim
        INTERFACE # pick one
        cfw PRIVATE ${X11_LIBRARIES})

