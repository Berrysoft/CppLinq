find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(linq_add_test name)
  add_executable(${name} "${name}.cpp")
  target_compile_definitions(${name} PRIVATE "BOOST_TEST_DYN_LINK")
  target_link_libraries(${name} PRIVATE linq Boost::unit_test_framework)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

linq_add_test(core_test)
linq_add_test(query_test)
linq_add_test(aggregate_test)
linq_add_test(string_test)

if(IS_WINDOWS_10)
  linq_add_test(winrt_test)
endif()
