add_custom_target(pstld-build-llvm-tests
    COMMENT "Build all the pstl tests.")

add_custom_target(check-pstld-llvm
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    USES_TERMINAL
    DEPENDS pstld-build-llvm-tests
    COMMENT "Build and run all the unit tests.")

set(UNIT_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/all_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/any_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/count.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find.pass.cpp    
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find_first_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find_if.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/for_each.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/none_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/numerics/numeric.ops/reduce.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/numerics/numeric.ops/transform_reduce.pass.cpp
)

foreach(_file IN LISTS UNIT_TESTS)
    file(RELATIVE_PATH _target "${CMAKE_CURRENT_SOURCE_DIR}" "${_file}")
    string(REPLACE ".cpp" "" _target "${_target}")
    string(REPLACE "/" "-" _target "${_target}")
    set(_target "pstl-${_target}")

    add_executable(${_target} EXCLUDE_FROM_ALL "${_file}")
    target_include_directories(${_target} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test")
    target_link_libraries(${_target} PRIVATE pstld)
    set_target_properties(${_target} PROPERTIES CXX_EXTENSIONS NO
                                                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(${_target} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${_target} PROPERTIES COMPILE_FLAGS "-include${CMAKE_CURRENT_LIST_DIR}/../include/pstld/hack_into_std.h")
    add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
    add_dependencies(pstld-build-llvm-tests ${_target})
endforeach()
