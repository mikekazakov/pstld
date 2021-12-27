set(_target "custom-linked-mixed")

add_executable(${_target} EXCLUDE_FROM_ALL test.cpp f.cpp g.mm)

target_link_libraries(${_target} PRIVATE pstld)
    
set_target_properties(${_target} PROPERTIES
    FOLDER "Tests/Custom"
    CXX_STANDARD 17
    COMPILE_FLAGS "-fobjc-arc -Wall -Wextra -Wpedantic -Werror")

add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")

add_dependencies(pstld-build-custom-tests ${_target})
