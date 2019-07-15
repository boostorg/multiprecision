macro(check_prop PROP_NAME VAR_NAME PROP_VALUE)
    build_dir(${TEST_DIR}/checkprop CMAKE_ARGS
              -D${VAR_NAME}=${PROP_VALUE}
              -DPROP_NAME=${PROP_NAME}
              -DPROP_VALUE=${PROP_VALUE})
endmacro()

foreach(VALUE On Off 1 0)
    check_prop(CXX_EXCEPTIONS CMAKE_CXX_EXCEPTIONS ${VALUE})
    check_prop(CXX_RTTI CMAKE_CXX_RTTI ${VALUE})
    check_prop(CXX_STATIC_RUNTIME CMAKE_CXX_STATIC_RUNTIME ${VALUE})
    check_prop(CXX_WARNINGS CMAKE_CXX_WARNINGS ${VALUE})
    check_prop(CXX_WARNINGS_AS_ERRORS CMAKE_CXX_WARNINGS_AS_ERRORS ${VALUE})
endforeach()
check_prop(CXX_WARNINGS CMAKE_CXX_WARNINGS ALL)