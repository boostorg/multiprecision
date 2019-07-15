install_dir(${TEST_DIR}/simpletest TARGETS check CMAKE_ARGS -DBUILD_SHARED_LIBS=On -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=On)
test_check_package(NAME simple HEADER simple.h TARGET simple CMAKE_ARGS -DBUILD_SHARED_LIBS=On -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=On)
test_check_pkgconfig(NAME simple HEADER simple.h)
