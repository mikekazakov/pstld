add_custom_target(pstld-build-custom-tests
    COMMENT "Build all the custom tests.")
set_target_properties(pstld-build-custom-tests PROPERTIES FOLDER "Tests/Custom")

add_custom_target(check-pstld-custom
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    USES_TERMINAL
    DEPENDS pstld-build-custom-tests
    COMMENT "Build and run all the unit tests.")
set_target_properties(check-pstld-custom PROPERTIES FOLDER "Tests/Custom")

add_subdirectory(linked_objcpp_arc)
add_subdirectory(linked_objcpp_noarc)
add_subdirectory(linked_mixed)
add_subdirectory(single_header_cpp)
add_subdirectory(single_header_mixed)
add_subdirectory(single_header_objcpp_arc)
add_subdirectory(single_header_objcpp_noarc)
