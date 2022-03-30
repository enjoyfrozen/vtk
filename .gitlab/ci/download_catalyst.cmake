cmake_minimum_required(VERSION 3.12)

set(data_host "https://data.kitware.com")

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "wheel")
  return ()
endif ()

# Determine the tarball item to download. ci_catalyst_{os}_{date}.tar.gz
# 2022-03-23: introduce catalyst with most recent commit, 3f7871c0a2e737cb9ed35fc1c2208456fcc00a0e
if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "vs2019")
  set(file_item "62441ad64acac99f42392431")
  set(file_hash "dc42e1e501b7bb5d71c0b3dbc526a003f8c6cad665a7ae1335d307f80406109d85ff696f8b1a72c2120c41cf887a2214ae141abe8a46aaf9c2b5738ebd9b218c")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_arm64")
  set(file_item "6242bc724acac99f421c66ee")
  set(file_hash "3a5ac525ff86df36d461b1f4d6c9e110008eb126e46f0ddd5052fa792afe31cfc2dce6030abb42deca9ebcec53202766ddb65b5dbc65a1a4d3a62711898cbce1")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_x86_64")
  set(file_item "62441b714acac99f423925a7")
  set(file_hash "0b70a47fe6ccf70e718c759e449be07145becea3a4cc1ed898aa7170ecd6168992b20451dd5c0f41cf9278a928cefa93bac627408872bcc9430e3dd86db859bf")
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
