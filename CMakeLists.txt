cmake_minimum_required(VERSION 3.1)
project(cpplinq CXX)
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

file(GLOB LINQ_HEADERS "include/linq/*.hpp")
add_library(linq INTERFACE)
target_include_directories(linq
  INTERFACE
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(linq INTERFACE "-fcoroutines")
endif()

option(LINQ_INSTALL "Install CppLinq" OFF)
option(LINQ_BUILD_TESTS "Build tests." OFF)

if(${LINQ_INSTALL})
  install(FILES ${LINQ_HEADERS} DESTINATION include/linq)
  install(TARGETS linq EXPORT cpplinq-targets)
  install(EXPORT cpplinq-targets DESTINATION lib/cmake/linq)
  install(FILES cmake/cpplinq-config.cmake DESTINATION lib/cmake/linq)
endif()

if(${LINQ_BUILD_TESTS})
  enable_testing()
  add_subdirectory(test)
endif()
