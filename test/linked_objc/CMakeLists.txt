get_filename_component(_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE "_" "-" _target "${_target}")
set(_target "custom-${_target}")

add_executable(${_target} EXCLUDE_FROM_ALL test.mm)

target_link_libraries(${_target} PRIVATE pstld)
    
set_target_properties(${_target} PROPERTIES
    FOLDER "Tests/Custom"
    CXX_STANDARD 17
    COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Werror")

add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")

add_dependencies(pstld-build-custom-tests ${_target})
