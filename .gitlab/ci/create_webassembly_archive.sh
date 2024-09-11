#!/bin/sh

set -e
set -x

# Select the wasm architecture for the package.
readonly architecture="${1}"
case "$architecture" in
    wasm64-emscripten)
        node_args="--experimental-wasm-memory64"
        ;;
    *)
        node_args=""
        ;;
esac
readonly node_args
version=$( node $node_args --eval "import('./build/install/bin/vtkWasmSceneManager.mjs').then(m => m.default().then(i => console.log(i.getVTKVersion())))" )
readonly version

cd build/install/bin
tar -cvzf "vtk-$version-$architecture.tar.gz" ./vtkWasmSceneManager.*
cd ../../
