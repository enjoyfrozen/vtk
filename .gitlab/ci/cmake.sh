#!/bin/sh

set -e

readonly mindeps_version="3.12.4"
readonly mindeps_prefix="cmake-mindeps"
readonly latest_version="3.30.2"
readonly latest_prefix="cmake"

case "$( uname -s )" in
    Linux)
        shatool="sha256sum"
        mindeps_sha256sum="486edd6710b5250946b4b199406ccbf8f567ef0e23cfe38f7938b8c78a2ffa5f"
        mindeps_platform="Linux-x86_64"
        latest_sha256sum="cdd7fb352605cee3ae53b0e18b5929b642900e33d6b0173e19f6d4f2067ebf16"
        latest_platform="linux-x86_64"
        ;;
    Darwin)
        shatool="shasum -a 256"
        mindeps_sha256sum="95d76c00ccb9ecb5cb51de137de00965c5e8d34b2cf71556cf8ba40577d1cff3"
        mindeps_platform="Darwin-x86_64"
        latest_sha256sum="c6fdda745f9ce69bca048e91955c7d043ba905d6388a62e0ff52b681ac17183c"
        latest_platform="macos-universal"
        ;;
    *)
        echo "Unrecognized platform $( uname -s )"
        exit 1
        ;;
esac
readonly shatool
readonly mindeps_sha256sum
readonly mindeps_platform
readonly latest_sha256sum
readonly latest_platform


# Select the CMake version to install
readonly cmake_version="${1:-latest}"

case "$cmake_version" in
    latest)
        version="$latest_version"
        sha256sum="$latest_sha256sum"
        platform="$latest_platform"
        prefix="$latest_prefix"
        ;;
    mindeps)
        version="$mindeps_version"
        sha256sum="$mindeps_sha256sum"
        platform="$mindeps_platform"
        prefix="$mindeps_prefix"

        # Skip if we're not in a `mindeps` job.
        if ! echo "$CMAKE_CONFIGURATION" | grep -q -e 'mindeps'; then
            exit 0
        fi
        ;;
    *)
        echo "Unknown CMake version: $cmake_version"
        exit 1
esac
readonly version
readonly sha256sum
readonly platform
readonly prefix

readonly filename="cmake-$version-$platform"
readonly tarball="$filename.tar.gz"

cd .gitlab

echo "$sha256sum  $tarball" > cmake.sha256sum
curl -OL "https://github.com/Kitware/CMake/releases/download/v$version/$tarball"
$shatool --check cmake.sha256sum
tar xf "$tarball"
mv "$filename" "$prefix"

if [ "$( uname -s )" = "Darwin" ]; then
    ln -s CMake.app/Contents/bin "$prefix/bin"
fi
