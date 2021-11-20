add_custom_target(pstld-build-msvc-tests
    COMMENT "Build all the msvc-stl tests.")

add_custom_target(check-pstld-msvc
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    USES_TERMINAL
    DEPENDS pstld-build-msvc-tests
    COMMENT "Build and run all the unit tests.")

set(UNIT_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/STL/tests/std/tests/P0024R2_parallel_algorithms_all_of/test.cpp
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
    set_target_properties(${_target} PROPERTIES CXX_EXTENSIONS NO
                                                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(${_target} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${_target} PROPERTIES COMPILE_FLAGS "-include${CMAKE_CURRENT_LIST_DIR}/../include/pstld/hack_into_std.h")
    add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
    add_dependencies(pstld-build-msvc-tests ${_target})
endforeach()
