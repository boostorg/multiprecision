cm_test_link_libraries(simple)

if(NOT BUILD_TESTING)
    cm_test_link_libraries(foo::non_existent)
endif()

cm_test(NAME simpletest SOURCES test.cpp)
