cmake_minimum_required(VERSION 3.18)

option(GULAMAN_LOGGING "Enable logging to stdout" ON)

include("${CMAKE_SOURCE_DIR}/cmake/rut.cmake")
RUTLoadCPM()
CPMAddPackage("gh:u16rogue/metapp@0.6.0")
CPMAddPackage("gh:fmtlib/fmt#10.2.1")
CPMAddPackage("gh:u16rogue/kitakit@0.2.0")

add_subdirectory("sdk/")

set(_NAME "gulaman")
project(${_NAME})

add_library(${_NAME} SHARED
  "src/logging.hh"
  "src/logging.cc"
  "src/global.hh"
  "src/main.cc"
  "src/modules.hh"
  "src/modules.cc"
  "src/plugin.hh"
  "src/plugin.cc"
  "src/core_plugin.cc"
  "src/api_impl.cc"
)

set_target_properties(${_NAME} PROPERTIES
  CXX_STANDARD 20
)

target_link_libraries(${_NAME} PRIVATE
  kitakit metapp fmt gulaman-sdk
)

target_compile_definitions(${_NAME} PRIVATE
  GULAMAN_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  $<$<BOOL:GULAMAN_LOGGING>:GULAMAN_LOGGING=1>
)

RUTTargetPedantic(${_NAME})
RUTTargetBuildDir(${_NAME} "${CMAKE_BINARY_DIR}")
RUTTargetBuildName(${_NAME} "gulaman" ".bin")
RUTTargetNoExcept(${_NAME})
