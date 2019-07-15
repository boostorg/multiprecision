install_dir(${TEST_DIR}/libsimple TARGETS check CMAKE_ARGS -DBUILD_SHARED_LIBS=On)
install_dir(${TEST_DIR}/basicapp TARGETS check CMAKE_ARGS -DBUILD_SHARED_LIBS=On)
test_check_package(NAME simple HEADER simple.h TARGET simple)
test_check_pkgconfig(NAME simple HEADER simple.h)
