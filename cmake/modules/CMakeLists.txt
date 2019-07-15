cmake_minimum_required (VERSION 3.5)

install(DIRECTORY share/modules/cmake DESTINATION cm)

enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR})
add_subdirectory(test)
