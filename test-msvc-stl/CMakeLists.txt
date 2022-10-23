add_custom_target(pstld-build-msvc-tests
    COMMENT "Build all the msvc-stl tests.")
set_target_properties(pstld-build-msvc-tests PROPERTIES FOLDER "Tests/MSVC")

add_custom_target(check-pstld-msvc
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    USES_TERMINAL
    DEPENDS pstld-build-msvc-tests
    COMMENT "Build and run all the unit tests.")
set_target_properties(check-pstld-msvc PROPERTIES FOLDER "Tests/MSVC")

set(UNIT_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_adjacent_difference/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_adjacent_find/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_all_of/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_count/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_equal/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_exclusive_scan/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_find/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_find_end/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_find_first_of/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_for_each/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_inclusive_scan/test.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_is_heap/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_is_partitioned/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_is_sorted/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_mismatch/test.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_partition/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_reduce/test.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_remove/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_replace/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_search/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_search_n/test.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_set_difference/test.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_set_intersection/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_sort/test.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_stable_sort/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_transform/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_transform_exclusive_scan/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_transform_inclusive_scan/test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_transform_reduce/test.cpp
)

foreach(_file IN LISTS UNIT_TESTS)
    file(RELATIVE_PATH _target "${CMAKE_CURRENT_LIST_DIR}" "${_file}")
    string(REPLACE "STL/tests/std/tests/" "" _target "${_target}")
    string(REPLACE "/test.cpp" "" _target "${_target}")
    string(REPLACE "/" "-" _target "${_target}")
    set(_target "msvc-${_target}")

    add_executable(${_target} EXCLUDE_FROM_ALL "${_file}")
    target_include_directories(${_target} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/include")
    target_link_libraries(${_target} PRIVATE pstld)
    set_target_properties(${_target} PROPERTIES
        FOLDER "Tests/MSVC"
        COMPILE_FLAGS "-includepstld/pstld.h -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -UNDEBUG"
        CXX_STANDARD 17
        CXX_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMPILE_DEFINITIONS PSTLD_HACK_INTO_STD )
    add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
    add_dependencies(pstld-build-msvc-tests ${_target})
endforeach()
