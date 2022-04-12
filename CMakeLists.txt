cmake_minimum_required(VERSION 3.15.1)
project(laplacian-pyramid-library)

set(CMAKE_CXX_STANDARD 17)

OPTION(BUILD_TEST "Build tests" ON)
OPTION(BUILD_DOCUMENTATION "Build documentation" ON)
OPTION(BUILD_CODE_DOCUMENTATION "Build code documentation" ON)
OPTION(BUILD_EXT_LIBS "Build external libraries" ON)

if (BUILD_EXT_LIBS)
    add_subdirectory(ext)
    set(BUILD_SHARED_LIBS ON CACHE INTERNAL "")
    find_package(OpenCV REQUIRED CONFIG
            PATHS ${CMAKE_BINARY_DIR}/ext/opencv/opencv)
    unset(BUILD_SHARED_LIBS)
endif()

add_subdirectory(lib)

if (BUILD_TEST)
    add_subdirectory(test)
endif()
if (BUILD_DOCUMENTATION)
    add_subdirectory(doc_source)
endif()
if (BUILD_CODE_DOCUMENTATION)
    add_subdirectory(doxygen)
endif ()