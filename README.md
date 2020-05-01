# CADMesh

Load triangular mesh based CAD files into Geant4 quickly and easily.

```cpp
    #include "CADMesh.hh"
    ....
    auto mesh = CADMesh::TessellatedMesh::FromPLY("mesh.ply");
    G4VSolid* solid = mesh->GetSolid();
```

CADMesh is distributed as a single header. No need to compile anything.
Copy `CADMesh.hh` to your project `include` directory, and start using it with the built in readers for PLY, STL, and OBJ files.
See the [basic example](https://github.com/christopherpoole/CADMesh/tree/master/examples/basic) for more details.

<p align="center">
<img src="https://raw.github.com/christopherpoole/CADMesh/master/screenshot.png"/>
</p>

# Hire Me
I'm self employed, and depend on people like you needing to simulate ionising radiation.
If you don't have time to write your own simulations, or if you want to get better radiation simulation results faster, please get in touch with me here [chris@radan.io](https://radan.io/about.html), or visit my website [radan.io](https://radan.io).
I'm widely experienced in building radiotherapy dose calculation software, radiation protection simulation technology, and I'm always interested in contributing to your academic papers.

<p align="center">
<div style="max-width: 400px">
<a href="https://radan.io"><img style="max-width: 400px" src="https://radan.io/assets/img/logo.png"/></a>
</div>
</p>

---

## Why?
Why is CADMesh 2 now header only, and defaults to no dependencies?
Well, one of the biggest problems reported with the original CADMesh was difficulty in installing it in HPC environments that may not have internet access.
Also, the dependencies themselves sometimes became troublesome to compile.
Now, for the basic and most widely reported use case of loading in STL, PLY or OBJ files into Geant4 is really easy.
`CADMesh.hh` stays with your code, and you don't have to worry about installing it everywhere you want your simulation to run.

## Installation
Download the latest release of CADMesh (the CADMesh.hh file) from the [Downloads](https://github.com/christopherpoole/CADMesh/releases) section.
Copy `CADMesh.hh` to the `includes` directory of your project.
That's it.

Read on if you would like the use the optional ASSIMP or TETGEN file readers.

## Usage

To use CADMesh, first include the header `#include "CADMesh.hh"`.

### Loading Meshes

Using the built-in readers, load meshes in the following way.
The built-in readers can only read ASCII files.

#### PLY 
```
auto mesh = CADMesh::TessellatedMesh::FromPLY("mesh.ply");
```

#### STL
```
auto mesh = CADMesh::TessellatedMesh::FromSTL("mesh.stl");
```

#### OBJ 
```
auto mesh = CADMesh::TessellatedMesh::FromOBJ("mesh.obj");
```

### Scale and Offset
Scale and offset can be set to the meshes directly, before creating a `G4TesselatedSolid`. This is useful if you need to convert units, or adjust the mesh origin.
The scale is applied before the offset internally, regardless of which order you specify them in your code.

#### Scale
By default the mesh scale is one (mm in Geant4).
```
mesh->SetScale(100.0);
```

#### Offset
Offset can be specified as three numbers or as a three vector.
```
double x = 0.0;
double y = 0.0;
double z = 0.0;

mesh->SetOffset(x, y, z);
mesh->SetOffset(G4ThreeVector(x, y, z);
```

#### Get the Solid
To get your mesh as a `G4TessellatedSolid`, do this:
```
auto solid = mesh->GetSolid();
```
Once you have the solid, you can use it like you would any other `G4VSolid` in Geant4. 
#### Multiple Meshes
Some file types, such as OBJ, can contain multiple meshes.
At the moment we support accessing meshes by name and index in OBJ files using the built-in reader.
Have a look at the `multiple` example for more information.
```
std::vector<G4VSolid*> solids = mesh->GetSolids();
auto solid = mesh->GetSolid(2);      // Get the third mesh as a solid.
auto solid = mesh->GetSolid("cube"); // Get the mesh named "cube" as a solid.
auto solid = mesh->GetSolid("cub", false); // Inexact name matching.
```
You can set the scale and offset for each mesh before getting the solid, to add the same mesh multiple times to your geometry, but at difference scales - if you want to do that.

### Filling Meshes With Tetrahedra
As described [here](https://github.com/christopherpoole/CADMesh/blob/master/Poole%20et%20al.%20-%20Fast%20tessellated%20solid%20navigation%20in%20GEANT4.pdf), tessellated solid navigation can be sped up by filling meshes with tetrahedra, and navigating that equivalent geometry instead.
To do this you need to add `tetgen` as a dependency to your project - read more about this further on in the readme.

#### PLY 
```
auto tets = CADMesh::TetrahedralMesh::FromPLY("mesh.ply");
```

#### STL
```
auto tets = CADMesh::TetrahedralMesh::FromSTL("mesh.stl");
```

#### OBJ 
```
auto tets = CADMesh::TetrahedralMesh::FromOBJ("mesh.obj");
```

Scale and offset can be applied as described above.

#### Tetrahedra Material
We need to specify the material for all of these tetrahedra that have been generated.
```
tets->SetMaterial(g4_material);
```

#### Get the Assembly
Rather than a `G4Solid`, in the case of the tessellated meshes, here we place a `G4Assembly`. Don't worry if you haven't used this type before - there is nothing to it - it is just a group of solids all with the same material.

```
auto assembly = tets->GetAssembly();

auto position = G4ThreeVector();
auto rotation = new G4RotationMatrix();

assembly->MakeImprint(world_logical, position, rotation);
``` 

## Developers
These instructions are for people seeking to modify CADMesh in some way.

### Header Generation
To generate the header file for distribution (or testing), run the `cmake/generateSingleHeade.py` script while in the `build` directory.

```
mkdir build
cd build
python3 ../cmake/generateSingleHeader.py
```

Now you have `CADMesh.hh` in the `build` directory.

### Running Tests
CADMesh is distributed with some tests.
They are a work in progress, and we use them to make sure CADMesh works as expected in the general sense, as well as to test specific scenarios (for example when certain files can't be loaded for some reason).

To run all the tests:

```
mkdir build
cd build
cmake -D WITH_TESTS=ON ..
make
make test
```

To run a specific test, for example:
```
./ValidTessellatedMeshTests
```

## Optional Dependencies
If you need to read PLY, STL or OBJ files, there are no dependencies (other than Geant4 obviously).

Optional dependencies:
 * [ASSIMP](https://github.com/assimp/assimp)
 * [TETGEN](http://tetgen.org/)

If you really want to install them, on a Debian like distribution you can run:
```
    apt-get install libtet1.5-dev libassimp-dev
```

ASSIMP and TETGEN can be turned on and off using `#defines`, ASSIMP can even be used as the default reader if you want that.
Please see the examples directory for inspiration on the different ways you can use CADMesh:
 * [Using the ASSIMP reader](https://github.com/christopherpoole/CADMesh/tree/master/examples/external)
 * [Using tetgen with CADMesh](https://github.com/christopherpoole/CADMesh/tree/master/examples/tetrahedra)

## Further Reading
Importing predefined CAD models into [GEANT4](http://www.geant4.org/geant4/) is not always possible or requires intermediate file format conversion to Geometry Description Markup Language (GDML) using commercial or third party software.
CADMesh is a direct CAD model import interface for GEANT4 leveraging built-in readers as well as [ASSIMP](https://github.com/assimp/assimp) for reading the CAD files.
Currently it supports the import of triangular facet surface meshes defined in formats such as STL, PLY, and OBJ.
A G4TessellatedSolid is returned and can be included in a standard user detector constructor.
You can find the project page and a git repository [here](https://github.com/christopherpoole/CADMesh).
The old [Google Code page for CADMesh](http://code.google.com/p/cadmesh/) redirects to this page.

Additional functionality is included for the fast navigation of tessellated solids by automatically creating equivalent tetragon filled assemblies thereby making smart voxelisation available for the solid.

### Citing CADMesh
CADMesh is described in a technical paper, available [here](https://github.com/christopherpoole/CADMesh/blob/master/Poole%20et%20al.%20-%20A%20CAD%20interface%20for%20GEANT4.pdf), and in this repository.
If you would like the cite the article, you may wish use the following bibtex entry:

    @article{poole2012acad,
        author = {Poole, C. M. and Cornelius, I. and Trapp, J. V. and Langton, C. M.},
        title = {{A CAD Interface for GEANT4}},
        journal={Australasian Physical & Engineering Science in Medicine},
        year = 2012,
        month = September,
        doi = {10.1007/s13246-012-0159-8},
        url = {http://www.springerlink.com/content/u563877422284578}
    }

The fast tessellated solid navigation technique included in CADMesh is described [here](https://github.com/christopherpoole/CADMesh/blob/master/Poole%20et%20al.%20-%20Fast%20tessellated%20solid%20navigation%20in%20GEANT4.pdf), the paper is included in this repository as well:

    @article{poole2012fast,
        title={Fast Tessellated Solid Navigation in GEANT4},
        author={Poole, C. and Cornelius, I. and Trapp, J. and Langton, C.M.},
        journal={IEEE Transactions on Nuclear Science},
        volume={99},
        pages={1--7},
        year={2012},
        publisher={Institute of Electrical and Electronics Engineers}
    }

