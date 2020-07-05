enable_testing()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(linq_add_test name)
  add_executable(${name} "${name}.cpp")
  target_link_libraries(${name} PRIVATE linq Boost::unit_test_framework)
  add_test(${name} ${name})
endfunction()

linq_add_test(core_test)
linq_add_test(query_test)
