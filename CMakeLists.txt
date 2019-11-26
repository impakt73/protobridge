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

find_package(verilator REQUIRED)

if(NOT DEFINED PROTOBRIDGE_HW_PATH)
    message(FATAL_ERROR "PROTOBRIDGE_HW_PATH must be defined by the parent project!")
endif()

verilate(protobridge $<DEFINED PROTOBRIDGE_TRACE:TRACE>
    INCLUDE_DIRS "${PROTOBRIDGE_HW_PATH}"
    SOURCES      "${PROTOBRIDGE_HW_PATH}/top.v"
)
