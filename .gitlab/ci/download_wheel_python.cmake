cmake_minimum_required(VERSION 3.12)

# Where Python for wheels are stored.
set(python_url_root "https://www.paraview.org/files/dependencies/python-for-wheels")

# Python version specifics.
set(python36_version "3.6.8") # Source-only releases after this.
set(python37_version "3.7.9") # Source-only releases after this.
set(python38_version "3.8.10") # Source-only releases after this.
set(python39_version "3.9.9")

# Hashes for various deployments.
set(python36_windows_x86_64_hash "3f70798c4885d11842869c9fb2842c8f8bafed1ebfac04e3f4ae1af6498527ce")
set(python37_windows_x86_64_hash "edd1b8c491635939f7b7e538650607db8307d6dfd3fef043ec5bc21ce4035700")
set(python38_windows_x86_64_hash "ea898990a11ecd89177ba64c2b63132431214a877e9c2d80b3435e26086cc9d1")
set(python39_windows_x86_64_hash "2a0756e62ca30978baf530c55b5ca2ff6c8a8679d2eec9c3729f81a089e8efb4")

set(python36_macos_x86_64_hash "bd3b68dfc9787c39312c8bd554853fe750abd999e100690c3ed81c29447b02d3")
set(python37_macos_x86_64_hash "1d31a228ac921c13787f74e0b9e7a04ae5806e70c5226e23711840bf0f0c9e90")
set(python38_macos_x86_64_hash "8c49fa50d34529e58769d3901e9e079554424d59bc1aa7dceb82c8c63f09cbc1")
set(python39_macos_arm64_hash "082334689bd31fdeff84e15e83e745418838bf4f918cbda96c7d123dba574453")
set(python39_macos_x86_64_hash "d1fa6db1adab392657b4bbb00b26be23f2a40b93ce8ca3dde53eec0f6f5400e8")

# Extracting information from the build configuration.
if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  set(python_platform "windows")
  set(python_ext "zip")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos")
  set(python_platform "macos")
  set(python_ext "tar.xz")
else ()
  message(FATAL_ERROR
    "Unknown platform for Python")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "36_")
  set(python_version 36)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "37_")
  set(python_version 37)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "38_")
  set(python_version 38)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "39_")
  set(python_version 39)
else ()
  message(FATAL_ERROR
    "Unknown version for Python")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "x86_64")
  set(python_arch "x86_64")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "arm64")
  set(python_arch "arm64")
else ()
  message(FATAL_ERROR
    "Unknown architecture for Python")
endif ()

# Figure out what file we're supposed to download.
set(python_subdir "python-${python${python_version}_version}-${python_platform}-${python_arch}")
set(filename "${python_subdir}.${python_ext}")
set(sha256sum "${python${python_version}_${python_platform}_${python_arch}_hash}")

# Verify that we have a hash to validate.
if (NOT sha256sum)
  message(FATAL_ERROR
    "Unsupported configuration ${python_platform}/${python_arch} ${python${python_version}_version}")
endif ()

# Download the file.
file(DOWNLOAD
  "${python_url_root}/${filename}"
  ".gitlab/${filename}"
  STATUS download_status
  EXPECTED_HASH "SHA256=${sha256sum}")

# Check the download status.
list(GET download_status 0 res)
if (res)
  list(GET download_status 1 err)
  message(FATAL_ERROR
    "Failed to download ${filename}: ${err}")
endif ()

# Extract the file.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    -E tar
    xf "${filename}"
  WORKING_DIRECTORY ".gitlab"
  RESULT_VARIABLE res
  ERROR_VARIABLE err
  ERROR_STRIP_TRAILING_WHITESPACE)
if (res)
  message(FATAL_ERROR
    "Failed to extract ${filename}: ${err}")
endif ()

# Move to a predictable prefix.
file(RENAME
  ".gitlab/${python_subdir}"
  ".gitlab/python")
