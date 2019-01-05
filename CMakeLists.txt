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

add_headers(SF_HEADERS core.hpp query.hpp aggregate.hpp to_container.hpp string.hpp)

install(FILES ${SF_HEADERS} DESTINATION include/linq)

enable_testing()

add_executable(enumerable test/enumerable.cpp)
add_test(enumerable_test enumerable)
set_tests_properties(enumerable_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(where test/where.cpp)
add_test(where_test where)
set_tests_properties(where_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(select test/select.cpp)
add_test(select_test select)
set_tests_properties(select_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(select_many test/select_many.cpp)
add_test(select_many_test select_many)
set_tests_properties(select_many_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(range test/range.cpp)
add_test(range_test range)
set_tests_properties(range_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(skip test/skip.cpp)
add_test(skip_test skip)
set_tests_properties(skip_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(take test/take.cpp)
add_test(take_test take)
set_tests_properties(take_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(reverse test/reverse.cpp)
add_test(reverse_test reverse)
set_tests_properties(reverse_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(concat test/concat.cpp)
add_test(concat_test concat)
set_tests_properties(concat_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(zip test/zip.cpp)
add_test(zip_test zip)
set_tests_properties(zip_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(cast test/cast.cpp)
add_test(cast_test cast)
set_tests_properties(cast_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(count test/count.cpp)
add_test(count_test count)
set_tests_properties(count_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(anyall test/anyall.cpp)
add_test(anyall_test anyall)
set_tests_properties(anyall_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(aggregate test/aggregate.cpp)
add_test(aggregate_test aggregate)
set_tests_properties(aggregate_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(sort test/sort.cpp)
add_test(sort_test sort)
set_tests_properties(sort_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(minmax test/minmax.cpp)
add_test(minmax_test minmax)
set_tests_properties(minmax_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(to_container test/to_container.cpp)
add_test(to_container_test to_container)
set_tests_properties(to_container_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(split test/split.cpp)
add_test(split_test split)
set_tests_properties(split_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(join test/join.cpp)
add_test(join_test join)
set_tests_properties(join_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(instr test/instr.cpp)
add_test(instr_test instr)
set_tests_properties(instr_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(chain test/chain.cpp)
add_test(chain_test chain)
set_tests_properties(chain_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")

add_executable(extension test/extension.cpp)
add_test(extension_test extension)
set_tests_properties(extension_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")
