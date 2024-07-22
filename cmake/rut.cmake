file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/_rut/rut-bin")
if (NOT TARGET "rut-bin")
  project("rut-bin")
  add_library("rut-bin" INTERFACE)
  target_include_directories("rut-bin" INTERFACE "${CMAKE_BINARY_DIR}/_rut/")
endif()

message("-- rut > Loaded!")

function (RUTLoadCPM)
  # SPDX-License-Identifier: MIT
  #
  # SPDX-FileCopyrightText: Copyright (c) 2019-2023 Lars Melchior and contributors

  set(CPM_DOWNLOAD_VERSION 0.39.0)
  set(CPM_HASH_SUM "66639bcac9dd2907b2918de466783554c1334446b9874e90d38e3778d404c2ef")

  if(CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
  elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
  else()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
  endif()

  # Expand relative path. This is important if the provided path contains a tilde (~)
  get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)

  file(DOWNLOAD
       https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION} EXPECTED_HASH SHA256=${CPM_HASH_SUM}
  )

  include(${CPM_DOWNLOAD_LOCATION})
endfunction()

function (RUTTargetPedantic target)
  target_compile_options(${target}
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
      $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
  )
endfunction()

function (RUTTargetNoExcept target)
  target_compile_options(${target}
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -fno-exceptions>
      $<$<CXX_COMPILER_ID:MSVC>:>
  )
endfunction()

function (RUTTargetBuildName target fname fext)
  set_target_properties(${target} PROPERTIES
    PREFIX              ""
    SUFFIX              "${fext}"
    OUTPUT_NAME         "${fname}"
    LIBRARY_OUTPUT_NAME "${fname}"
  )
endfunction()

function (RUTTargetBuildDir target dir)
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  "${dir}"
    LIBRARY_OUTPUT_DIRECTORY  "${dir}"
  )
endfunction()

function (RUTGenBin2HeaderCpp name file)
  message("-- rut > Generating binary header file for ${file} as ${name}...")
  file(READ ${file} file_hex HEX)
  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," file_hex ${file_hex})
  set(file_hex "namespace rut::bin{constexpr unsigned char ${name}[]={${file_hex}}\;}")
  file(WRITE "${CMAKE_BINARY_DIR}/_rut/rut-bin/${name}.hh" ${file_hex})
endfunction()

function (RUTGenBin2HeaderC name file)
  message("-- rut > Generating binary header file for ${file} as ${name}...")
  file(READ ${file} file_hex HEX)
  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," file_hex ${file_hex})
  set(file_hex "const unsigned char ${name}[]={${file_hex}}\;")
  file(WRITE "${CMAKE_BINARY_DIR}/_rut/rut-bin/${name}.h" ${file_hex})
endfunction()
