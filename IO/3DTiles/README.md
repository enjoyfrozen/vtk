# vtkTiler - Convert large 3D geospatial datasets to the 3D Tiles format.

# Install
- Using node and npm installed on Ubuntu 20.04:
- `cd ~/external/3d-tiles-tools/;npm install 3d-tiles-tools`. Help at: <https://github.com/AnalyticalGraphicsInc/3d-tiles-tools/tree/master/tools>
- `cd ~/external/gltf-pipeline;npm install gltf-pipeline`. Help at: <https://github.com/CesiumGS/gltf-pipeline>
- Clone <https://github.com/CesiumGS/3d-tiles-samples>. and then `npm install.`


# Build

## Dependencies
* [CMake](https://cmake.org) >= 3.2
* [VTK](https://vtk.org) >= 9.0.0 (optionally with raytracing capabilities to enable OSPray rendering).
* A C++11 compiler.
* A CMake-compatible build system (Visual Studio, XCode, Ninja, Make...).

## Configuration and building
Set the following CMake options:
* `VTK_DIR`: Point to a build or install directory of VTK.

Then build the software using your build system.


# Convert data to 3D Tiles
- Create a gltf file for all Jacksonville OBJs
```
cd ~/projects/tiler/build
./bin/tiler ../../../data/CORE3D/Jacksonville/building_*building*.obj -o 3d-tiles/ `
```
- Convert gltf to glb
```
find . -name '*.gltf' -exec bash -c 'nodejs ~/external/gltf-pipeline/bin/gltf-pipeline.js -i ${0} -o ${0%.*}.glb' {} \;
find . -name '*.gltf' -exec rm {} \;
find . -name '*.bin' -exec rm {} \;
```
- Convert glb to b3dm
```
find . -name '*.glb' -exec bash -c 'nodejs ~/external/3d-tiles-tools/tools/bin/3d-tiles-tools.js glbToB3dm ${0} ${0%.*}.b3dm' {} \;
find . -name '*.glb' -exec rm {} \;

```
# View in cesium
1. Use 3d-tiles-samples
  - Link the tileset created for previous set:
  `cd ~/external/3d-tiles-samples/tilesets; ln -s ~/data/CORE3D/Jacksonville/local_z_up/Batchedbuilding_15_building_22/`
  - Start web server:
  `cd ..;npm start`
2. Load `cd ~/tasks/3d-tiles-samples/;google-chrome building-15-building_22.html` created like in the documentation.


# TODO
- add saving to glb to vtkGLTFExporter
- add a b3dm writer to VTK
- add MTL support to the OBJ reader
