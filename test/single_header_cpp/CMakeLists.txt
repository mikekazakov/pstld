set(_target "custom-single-header-cpp")

add_executable(${_target} EXCLUDE_FROM_ALL test.cpp f.h f.cpp)

target_include_directories( ${_target} PRIVATE $<TARGET_PROPERTY:pstld,INCLUDE_DIRECTORIES>)
    
set_target_properties(${_target} PROPERTIES
    FOLDER "Tests/Custom"
    CXX_STANDARD 17
    COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Werror")

add_test(${_target} "${CMAKE_CURRENT_BINARY_DIR}/${_target}")

add_dependencies(pstld-build-custom-tests ${_target})
