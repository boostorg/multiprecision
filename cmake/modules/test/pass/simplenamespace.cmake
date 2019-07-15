install_dir(${TEST_DIR}/libsimplenamespace TARGETS check)
test_check_package(NAME simple HEADER simple.h TARGET lib::simple)
