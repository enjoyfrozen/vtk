#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="json"
readonly ownership="JSON Upstream <kwrobot@kitware.com>"
readonly subtree="ThirdParty/$name/vtk$name"
readonly repo="https://gitlab.kitware.com/danlipsa/json.git"
readonly tag="for/vtk"
readonly paths="
single_include/nlohmann/json.hpp
LICENSE.MIT
README.md
README.kitware.md
CMakeLists.vtk.txt
"

extract_source () {
    git_archive
    pushd "$extractdir/$name-reduced"
    mv -v single_include/* .
    mv -v CMakeLists.vtk.txt CMakeLists.txt
    popd
}

. "${BASH_SOURCE%/*}/../update-common.sh"
