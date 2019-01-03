cmake_minimum_required(VERSION 3.1)
project(CppLinq)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(include)

function(add_headers VAR)
  set(headers ${${VAR}})
  foreach (header ${ARGN})
    set(headers ${headers} include/linq/${header})
  endforeach()
  set(${VAR} ${headers} PARENT_SCOPE)
endfunction()

add_headers(SF_HEADERS core.hpp query.hpp aggregate.hpp to_container.hpp)

install(FILES ${SF_HEADERS} DESTINATION include/linq)

enable_testing()

add_executable(where test/where.cpp)
add_test(where_test where)
set_tests_properties(where_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(select test/select.cpp)
add_test(select_test select)
set_tests_properties(select_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(range test/range.cpp)
add_test(range_test range)
set_tests_properties(range_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(skip test/skip.cpp)
add_test(skip_test skip)
set_tests_properties(skip_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(take test/take.cpp)
add_test(take_test take)
set_tests_properties(take_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(count test/count.cpp)
add_test(count_test count)
set_tests_properties(count_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(to_container test/to_container.cpp)
add_test(to_container_test to_container)
set_tests_properties(to_container_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(chain test/chain.cpp)
add_test(chain_test chain)
set_tests_properties(chain_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(extension test/extension.cpp)
add_test(extension_test extension)
set_tests_properties(extension_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")
