cmake_minimum_required(VERSION 3.1)
project(cpplinq)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(include)

if(WIN32)
  string(REGEX MATCH "10.0[.[0-9]+]?" MATCH_WINDOWS_10 ${CMAKE_SYSTEM_VERSION})
  string(LENGTH "${MATCH_WINDOWS_10}" MATCH_WINDOWS_10_LENGTH)
  if(${MATCH_WINDOWS_10_LENGTH})
    message("You are running on Windows 10")
    set(IS_WINDOWS_10 true)
  endif()
endif()

file(GLOB LINQ_HEADERS "include/linq/*.hpp")

if (IS_WINDOWS_10)
  add_definitions(-DLINQ_USE_WINRT)
endif()

add_library(linq INTERFACE)

install(FILES ${LINQ_HEADERS} DESTINATION include/linq)
install(TARGETS linq EXPORT cpplinq_targets)

include(CMakePackageConfigHelpers)

export(EXPORT cpplinq_targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cpplinq/cpplinq-targets.cmake"
  NAMESPACE cpplinq::
)
configure_file(cmake/cpplinq-config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/cpplinq/cpplinq-config.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cpplinq)
install(EXPORT cpplinq_targets
  FILE
    cpplinq-targets.cmake
  NAMESPACE
    cpplinq::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/cpplinq-config.cmake
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

find_package(bstest CONFIG REQUIRED)
file(GLOB TEST_SOURCE "test/*.cpp")
add_executable(test ${TEST_SOURCE})
target_link_libraries(test bstest::bstest)

if (IS_WINDOWS_10)
  target_link_libraries(test WindowsApp.lib)
endif()
