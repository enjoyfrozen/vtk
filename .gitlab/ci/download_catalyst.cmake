cmake_minimum_required(VERSION 3.12)

set(data_host "https://data.kitware.com")

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "wheel")
  return ()
endif ()

# Determine the tarball name to download
if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "vs2019")
  set(file_item "")
  set(file_hash "")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_arm64")
  set(file_item "623857534acac99f429db268")
  set(file_hash "1a7447c89580ab5eb3bbfd3b70b61001e4bf5b4ca0fbf25fc755761323cbc3b38dbc6c375e20e820235c90d40fc1aaad952fe88d1f4bccdee75f6785ae5dcec9")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_x86_64")
  set(file_item "")
  set(file_hash "")
else ()
  message(FATAL_ERROR
    "Unknown ABI to use for Qt")
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
