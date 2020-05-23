enable_testing()
find_package(GTest)

add_executable(test_screen_projection test_screen_projection.cpp)
target_link_libraries(test_screen_projection wkttool_lib ${CONAN_LIBS})
add_test(screen_projection test_screen_projection)

add_executable(test_variant_signal test_variant_signal.cpp)
target_link_libraries(test_variant_signal wkttool_lib ${CONAN_LIBS})
add_test(variant_signal test_variant_signal)
