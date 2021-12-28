add_custom_target(pstld-build-llvm-tests
    COMMENT "Build all the pstl tests.")
set_target_properties(pstld-build-llvm-tests PROPERTIES FOLDER "Tests/LLVM")

add_custom_target(check-pstld-llvm
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    USES_TERMINAL
    DEPENDS pstld-build-llvm-tests
    COMMENT "Build and run all the unit tests.")
set_target_properties(check-pstld-llvm PROPERTIES FOLDER "Tests/LLVM")

set(UNIT_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/copy_move.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/fill.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/generate.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/replace.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/replace_copy.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/swap_ranges.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/transform_binary.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/transform_unary.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.modifying.operations/alg.reverse/reverse.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/adjacent_find.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/all_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/any_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/count.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/equal.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find_end.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find_first_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/find_if.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/for_each.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/mismatch.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/none_of.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.nonmodifying/search_n.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.sorting/is_sorted.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.sorting/sort.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/algorithms/alg.sorting/alg.min.max/minmax_element.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/numerics/numeric.ops/adjacent_difference.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/numerics/numeric.ops/reduce.pass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test/std/numerics/numeric.ops/transform_reduce.pass.cpp
)

foreach(_file IN LISTS UNIT_TESTS)
    file(RELATIVE_PATH _target "${CMAKE_CURRENT_LIST_DIR}" "${_file}")
    string(REPLACE "llvm-project/pstl/test/std/" "" _target "${_target}")
    string(REPLACE ".pass.cpp" "" _target "${_target}")
    string(REPLACE "/" "-" _target "${_target}")
    set(_target "llvm-${_target}")

    add_executable(${_target} EXCLUDE_FROM_ALL "${_file}")
    target_include_directories(${_target} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/llvm-project/pstl/test")
    target_link_libraries(${_target} PRIVATE pstld)
    set_target_properties(${_target} PROPERTIES
        COMPILE_FLAGS "-includepstld/pstld.h -includetest-llvm-pstl/helpers.h -Wall -Wextra -Wpedantic -Werror"
        CXX_STANDARD 17
        CXX_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMPILE_DEFINITIONS PSTLD_HACK_INTO_STD )
    set_target_properties(${_target} PROPERTIES FOLDER "Tests/LLVM")
    add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
    add_dependencies(pstld-build-llvm-tests ${_target})
endforeach()
