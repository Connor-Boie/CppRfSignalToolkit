cmake_minimum_required(VERSION 3.22)

project(
    CppRfSignalToolkit
    VERSION 0.1.0
    DESCRIPTION "Tools for processing and analyzing recorded RF I/Q samples"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
