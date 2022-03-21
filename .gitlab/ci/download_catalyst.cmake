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
  set(file_item "623848324acac99f429d4ac9")
  set(file_hash "19924a4076703f4c0ef44d40ebbf582c8b9834b0d88e253fc2355a28d403665b01059a58fba68bfc2ec3a20974ebd069637387c4aa7c37c60538e09b3cf2ad16")
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
    xf ".gitlab/catalyst.tar.gz"
  RESULT_VARIABLE res
  ERROR_VARIABLE err
  ERROR_STRIP_TRAILING_WHITESPACE)
if (res)
  message(FATAL_ERROR
    "Failed to extract catalyst.tar.gz: ${err}")
endif ()
