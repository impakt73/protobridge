cmake_minimum_required (VERSION 3.10)

project(protobridge LANGUAGES CXX)

add_library(protobridge STATIC)

target_include_directories(protobridge PUBLIC inc)
target_include_directories(protobridge PRIVATE src)

target_sources(protobridge
    PRIVATE
        inc/protobridge.h
        src/protobridge.cpp
)

install(TARGETS protobridge)

find_package(verilator REQUIRED)

if(NOT DEFINED PROTOBRIDGE_HW_PATH)
    message(FATAL_ERROR "PROTOBRIDGE_HW_PATH must be defined by the parent project!")
endif()

if (DEFINED PROTOBRIDGE_TRACE)
    set(PROTOBRIDGE_TRACE_ARG "TRACE")
endif()

verilate(protobridge ${PROTOBRIDGE_TRACE_ARG}
    INCLUDE_DIRS "${PROTOBRIDGE_HW_PATH}"
    SOURCES      "${PROTOBRIDGE_HW_PATH}/top.sv"
)
