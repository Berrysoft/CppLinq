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

function(add_exe_test EXE_NAME)
  add_executable(${EXE_NAME} test/${EXE_NAME}.cpp)
  add_test(${EXE_NAME}_test ${EXE_NAME})
  set_tests_properties(${EXE_NAME}_test PROPERTIES PASS_REGULAR_EXPRESSION "Success.\n")
endfunction()

add_exe_test(enumerable)
add_exe_test(range)
add_exe_test(repeat)
add_exe_test(pend)

add_exe_test(where)
add_exe_test(select)
add_exe_test(select_many)
add_exe_test(skip)
add_exe_test(take)
add_exe_test(reverse)
add_exe_test(concat)
add_exe_test(zip)
add_exe_test(cast)

add_exe_test(count)
add_exe_test(anyall)
add_exe_test(aggregate)
add_exe_test(sort)
add_exe_test(minmax)
add_exe_test(get_at)
add_exe_test(index_of)
add_exe_test(set)
add_exe_test(group)

add_exe_test(to_container)

add_exe_test(split)
add_exe_test(joinstr)
add_exe_test(instr)
add_exe_test(replace)
add_exe_test(starts_ends_with)

add_exe_test(chain)

add_exe_test(extension)
