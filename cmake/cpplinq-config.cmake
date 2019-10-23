if(WIN32)
  string(REGEX MATCH "10.0[.[0-9]+]?" MATCH_WINDOWS_10 ${CMAKE_SYSTEM_VERSION})
  string(LENGTH "${MATCH_WINDOWS_10}" MATCH_WINDOWS_10_LENGTH)
  if(${MATCH_WINDOWS_10_LENGTH})
    message("You are running on Windows 10")
    set(IS_WINDOWS_10 true)
  endif()
endif()

if (IS_WINDOWS_10)
  add_definitions(-DLINQ_USE_WINRT)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cpplinq-targets.cmake")
