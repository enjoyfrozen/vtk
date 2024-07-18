##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  set(jdk_sha256_sum d37df800a005b91bed511e214821abf21ffe78d2c4d84f545268b1bd677e7401)
  set(jdk_url https://github.com/adoptium/temurin8-binaries/releases/download/jdk8u412-b08/OpenJDK8U-jdk_x64_windows_hotspot_8u412b08.zip)
  set(jdk_ext zip)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos")
  if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "x86_64")
    set(jdk_sha256_sum fd62491f7634c1cbed7557d6b21db7ef4818fbc0e63e678110d9d92cbea4ad8c)
    set(jdk_url https://github.com/adoptium/temurin8-binaries/releases/download/jdk8u412-b08/OpenJDK8U-jdk_x64_mac_hotspot_8u412b08.tar.gz) 
  elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "arm64")
    set(jdk_sha256_sum ac3ff5a57b9d00606e9b319bde7309a4ecb9f2c4ddc0f48d001f234e93b9da86)
    set(jdk_url https://download.oracle.com/java/17/archive/jdk-17.0.11_macos-aarch64_bin.tar.gz)
  else()
    message(FATAL_ERROR "Incompatible APPLE platform: ${ARCHITECTURE}")
  endif()
  set(jdk_ext tar.gz)
else()
  message(FATAL_ERROR "Incompatible platform: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

message("Downloading ${jdk_url}")
set(outdir "${CMAKE_SOURCE_DIR}/.gitlab/")
set(archive_path "${outdir}/jdk.${jdk_ext}")

# Download and extract
file(DOWNLOAD ${jdk_url} "${archive_path}" EXPECTED_HASH SHA256=${jdk_sha256_sum})
file(ARCHIVE_EXTRACT INPUT "${archive_path}" DESTINATION "${outdir}" VERBOSE)

# Rename subdir to standard naming
file(GLOB jdk_path "${outdir}/jdk*-*/")
file(RENAME ${jdk_path} "${outdir}/jdk/")
