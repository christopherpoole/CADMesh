# CADMesh Examples

This directory contains a few examples showing different ways of using CADMesh in your Geant4 simulations.

To run a example, change to its directory, and do the following:

```
cd basic/ 
mkdir build/
cd build/
cmake ..
make
./basic # or ./external or ./multiple or ./tetrahedral
```

Make sure to run the binary in the build directory, as the required mesh files are copied there automatically.

# Basic
Load PLY, STL, and OBJ files into your detector construction using the CADMesh built in readers. There are no dependencies to install. All you need is `CADMesh.hh`.

# External 
This example demonstrates loading meshes with the readers/loaders available in the ASSIMP geometry library. You need to add the ASSIMP library as a dependency to your own project.

# Multiple
Access multiple meshes contained within a single file and place them how you want. This is useful if you have assemblies exported from Blender for example.

# TETGEN
Learn how to load meshes into Geant4, and fill them with tetrahedra. This requires adding `tetgen` to your project as a dependency.

