cmake_minimum_required(VERSION 3.1)
project(cpplinq CXX)
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(WIN32)
  string(REGEX MATCH "10.0[.[0-9]+]?" MATCH_WINDOWS_10 ${CMAKE_SYSTEM_VERSION})
  string(LENGTH "${MATCH_WINDOWS_10}" MATCH_WINDOWS_10_LENGTH)
  if(${MATCH_WINDOWS_10_LENGTH})
    message("You are running on Windows 10")
    set(IS_WINDOWS_10 true)
  endif()
endif()

file(GLOB LINQ_HEADERS "include/linq/*.hpp")
add_library(linq INTERFACE)
target_include_directories(linq
  INTERFACE
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(IS_WINDOWS_10)
  target_compile_definitions(linq INTERFACE "LINQ_USE_WINRT")
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
