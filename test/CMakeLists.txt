find_package(GTest)

add_executable(test_screen_projection test_screen_projection.cpp)
target_link_libraries(test_screen_projection wkttool_lib ${CONAN_LIBS})
add_test(NAME test_screen_projection COMMAND test_screen_projection
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_variant_signal test_variant_signal.cpp)
target_link_libraries(test_variant_signal wkttool_lib ${CONAN_LIBS})
add_test(NAME test_variant_signal COMMAND test_variant_signal
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_sfml_conversion test_sfml_conversion.cpp)
target_link_libraries(test_sfml_conversion wkttool_lib ${CONAN_LIBS})
add_test(NAME test_sfml_conversion COMMAND test_sfml_conversion
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_make_grid test_make_grid.cpp)
target_link_libraries(test_make_grid wkttool_lib ${CONAN_LIBS})
add_test(NAME test_make_grid COMMAND test_make_grid
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_subsample test_subsample.cpp)
target_link_libraries(test_subsample wkttool_lib ${CONAN_LIBS})
add_test(NAME test_subsample COMMAND test_subsample
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_drawables test_drawables.cpp)
target_link_libraries(test_drawables wkttool_lib ${CONAN_LIBS})
add_test(NAME test_drawables COMMAND test_drawables
          WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
