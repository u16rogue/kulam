cmake_minimum_required(VERSION 3.18)

set(_NAME "kulam")
project(${_NAME})

add_library(${_NAME} SHARED
  "src/main.cc"
)

set_target_properties(${_NAME} PROPERTIES
  CXX_STANDARD 20
)

RUTTargetPedantic(${_NAME})
RUTTargetBuildDir(${_NAME} "${CMAKE_BINARY_DIR}")
RUTTargetBuildName(${_NAME} "kulam" ".bin")
