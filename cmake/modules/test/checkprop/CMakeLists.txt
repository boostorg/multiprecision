cmake_minimum_required(VERSION 3.5)
project(checkprop)

find_package(CM)

include(CMProperties)

get_property(CURRENT_PROP_VAL GLOBAL PROPERTY ${PROP_NAME})

if(NOT "${CURRENT_PROP_VAL}" STREQUAL "${PROP_VALUE}")
    message(FATAL_ERROR "Property ${PROP_NAME} set to ${CURRENT_PROP_VAL}, expected ${PROP_VALUE}")
endif()
