find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(linq_add_test name)
  add_executable(${name} "${name}.cpp")
  target_link_libraries(${name} PRIVATE linq Boost::unit_test_framework Boost::dynamic_linking)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

linq_add_test(core_test)
linq_add_test(query_test)
linq_add_test(aggregate_test)
linq_add_test(string_test)
linq_add_test(extension_test)
