cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "/usr/bin/clang" CACHE FILEPATH "c compiler" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE FILEPATH "c++ compiler" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(coral VERSION 0.1.0 LANGUAGES C CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(CORAL_BUILD_EXAMPLES "build examples" ON)
option(CORAL_BUILD_TESTS "build tests" ON)
option(CORAL_BUILD_TOOLS "build tools" ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  Debug Release RelWithDebInfo MiniSizeRel)
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
message(STATUS "CPU cores on current server is: ${CPU_CORES}")
math(EXPR JOBS "${CPU_CORES} + ${CPU_CORES} / 2")
set(CMAKE_BUILD_PARALLEL_LEVEL ${JOBS} CACHE STRING "build parallel level.")
message(STATUS "Compiler default work jobs: ${CMAKE_BUILD_PARALLEL_LEVEL}")

include(cmake/coral_options.cmake)

add_subdirectory(coral)

if(CORAL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(CORAL_BUILD_TOOLS)
  add_subdirectory(tools)
endif()
