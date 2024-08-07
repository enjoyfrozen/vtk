cmake_minimum_required(VERSION 3.12)

# Input variables.
set(adios2_version "2.10.1")
set(adios2_build_date "20240725.0")

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "mpi")
    set(adios2_prefix "adios2-mpi")
    set(adios2_platform "windows-x86_64")
    set(adios2_ext "zip")
    set(adios2_hash "2184a16bcaad938f793779cc2156a55060713dbd08a13b497208e36242828423")
  else()
    set(adios2_prefix "adios2-nompi")
    set(adios2_platform "windows-x86_64")
    set(adios2_ext "zip")
    set(adios2_hash "f016351a1d748fb71cfe1f0b200f4385b4645cc223ce758fbbc787d8beccf356")
  endif()
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_arm64")
  set(adios2_prefix "adios2")
  set(adios2_platform "macos-arm64")
  set(adios2_ext "tar.gz")
  set(adios2_hash "144dcdfaa35c69b5618ef9f2a97dfe8f1bb48367b7a5418d34b9628751f51292")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_x86_64")
  set(adios2_prefix "adios2")
  set(adios2_platform "macos-x86_64")
  set(adios2_ext "tar.gz")
  set(adios2_hash "ca63395dc37f115544602cf3ed73d70de8015dfd502248ecd8d98667ac9cd4e4")
else ()
  message(FATAL_ERROR
    "Unknown platform for ADIOS2")
endif ()

set(adios2_url "https://gitlab.kitware.com/api/v4/projects/6955/packages/generic/adios2/v${adios2_version}-${adios2_build_date}")
set(adios2_file "${adios2_prefix}-v${adios2_version}-${adios2_platform}.${adios2_ext}")

# Download the file.
file(DOWNLOAD
  "${adios2_url}/${adios2_file}"
  ".gitlab/${adios2_file}"
  STATUS download_status
  EXPECTED_HASH "SHA256=${adios2_hash}")

# Check the download status.
list(GET download_status 0 res)
if (res)
  list(GET download_status 1 err)
  message(FATAL_ERROR
    "Failed to download ${adios2_file}: ${err}")
endif ()

# Extract the file.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    -E tar
    xf "${adios2_file}"
  WORKING_DIRECTORY ".gitlab"
  RESULT_VARIABLE res
  ERROR_VARIABLE err
  ERROR_STRIP_TRAILING_WHITESPACE)
if (res)
  message(FATAL_ERROR
    "Failed to extract ${adios2_file}: ${err}")
endif ()
