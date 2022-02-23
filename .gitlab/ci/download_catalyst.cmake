cmake_minimum_required(VERSION 3.12)

set(data_host "https://data.kitware.com")

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "wheel")
  return ()
endif ()

# Determine the tarball item to download. ci_catalyst_{os}_{date}.tar.gz
# 2022-03-23: introduce catalyst with most recent commit, 3f7871c0a2e737cb9ed35fc1c2208456fcc00a0e
if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "vs2019")
  set(file_item "")
  set(file_hash "")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_arm64")
  set(file_item "6242bc724acac99f421c66ee")
  set(file_hash "3a5ac525ff86df36d461b1f4d6c9e110008eb126e46f0ddd5052fa792afe31cfc2dce6030abb42deca9ebcec53202766ddb65b5dbc65a1a4d3a62711898cbce1")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_x86_64")
  set(file_item "")
  set(file_hash "")
else ()
  message(FATAL_ERROR
    "Unknown ABI to use for Catalyst")
endif ()

# Ensure we have a hash to verify.
if (NOT DEFINED file_item OR NOT DEFINED file_hash)
  message(FATAL_ERROR
    "Unknown file and hash for catalyst dependency")
endif ()

# Download the file.
file(DOWNLOAD
  "${data_host}/api/v1/item/${file_item}/download"
  ".gitlab/catalyst.tar.gz"
  STATUS download_status
  EXPECTED_HASH "SHA512=${file_hash}")

# Check the download status.
list(GET download_status 0 res)
if (res)
  list(GET download_status 1 err)
  message(FATAL_ERROR
    "Failed to download catalyst.tar.gz: ${err}")
endif ()

# Extract the file.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    -E tar
    xf "catalyst.tar.gz"
  WORKING_DIRECTORY ".gitlab"
  RESULT_VARIABLE res
  ERROR_VARIABLE err
  ERROR_STRIP_TRAILING_WHITESPACE)
if (res)
  message(FATAL_ERROR
    "Failed to extract catalyst.tar.gz: ${err}")
endif ()
