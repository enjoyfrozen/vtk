#!/bin/sh

set -e

readonly rkcommon_version="1.8.0"
readonly rkcommon_tarball="v$rkcommon_version.tar.gz"
readonly rkcommon_sha256sum="f037c15f7049610ef8bca37500b2ab00775af60ebbb9d491ba5fc2e5c04a7794"

readonly embree_version="3.13.2"
readonly embree_tarball="v$embree_version.tar.gz"
readonly embree_sha256sum="dcda827e5b7a606c29d00c1339f1ef00f7fa6867346bc46a2318e8f0a601c6f9"

readonly openvkl_version="1.1.0"
readonly openvkl_tarball="v$openvkl_version.tar.gz"
readonly openvkl_sha256sum="d193c75a2c57acd764649215b244c432694a0169da374a9d769a81b02a9132e9"

readonly ospray_version="2.8.0"
readonly ospray_tarball="v$ospray_version.tar.gz"
readonly ospray_sha256sum="2dabc75446a0e2e970952d325f930853a51a9b4d1868c8135f05552a4ae04d39"

readonly ospray_root="$HOME/ospray"

readonly rkcommon_src="$ospray_root/rkcommon/src"
readonly rkcommon_build="$ospray_root/rkcommon/build"
readonly embree_src="$ospray_root/embree/src"
readonly embree_build="$ospray_root/embree/build"
readonly openvkl_src="$ospray_root/openvkl/src"
readonly openvkl_build="$ospray_root/openvkl/build"
readonly ospray_src="$ospray_root/src"
readonly ospray_build="$ospray_root/build"

dnf install -y --setopt=install_weak_deps=False \
    ispc
dnf clean all

mkdir -p "$ospray_root" \
    "$rkcommon_src" "$rkcommon_build" \
    "$embree_src" "$embree_build" \
    "$openvkl_src" "$openvkl_build" \
    "$ospray_src" "$ospray_build"
cd "$ospray_root"

(
    echo "$rkcommon_sha256sum  $rkcommon_tarball"
    echo "$embree_sha256sum  $embree_tarball"
    echo "$openvkl_sha256sum  $openvkl_tarball"
    echo "$ospray_sha256sum  $ospray_tarball"
) > ospray.sha256sum
curl -OL "https://github.com/ospray/rkcommon/archive/refs/tags/$rkcommon_tarball"
curl -OL "https://github.com/embree/embree/archive/$embree_tarball"
curl -OL "https://github.com/openvkl/openvkl/archive/$openvkl_tarball"
curl -OL "https://github.com/ospray/ospray/archive/refs/tags/$ospray_tarball"
sha256sum --check ospray.sha256sum

tar -C "$rkcommon_src" --strip-components=1 -xf "$rkcommon_tarball"
tar -C "$embree_src" --strip-components=1 -xf "$embree_tarball"
tar -C "$openvkl_src" --strip-components=1 -xf "$openvkl_tarball"
tar -C "$ospray_src" --strip-components=1 -xf "$ospray_tarball"

cd "$rkcommon_build"

cmake -GNinja "$rkcommon_src" \
    -DBUILD_TESTING=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local
ninja
ninja install

cd "$embree_build"

cmake -GNinja "$embree_src" \
    -DBUILD_TESTING=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DEMBREE_TUTORIALS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local
ninja
ninja install

cd "$openvkl_build"

cmake -GNinja "$openvkl_src" \
    -DBUILD_TESTING=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local
ninja
ninja install

cd "$ospray_build"

cmake -GNinja "$ospray_src" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local
ninja
ninja install

cd

rm -rf "$ospray_root"
