# CADMesh

Load triangular mesh based CAD files into Geant4 geometry quickly and easily.

```cpp
    #include "CADMesh.hh"
    ....
    auto mesh = CADMesh::TessellatedMesh::FromPLY("mesh.ply");
    G4VSolid* solid = mesh->GetSolid();
```

CADMesh is distributed as a single header. No need to compile anything. Copy it to your project include directory, and start using it with the built in readers for PLY, STL, and OBJ files.

ASSIMP and TETGEN can be turned on and off using `#defines`, ASSIMP can even be set tot he default reader if you want that.
Please see the examples directory for inspiration on the different ways you can use CADMesh.
 * [Using the ASSIMP reader](https://github.com/christopherpoole/CADMesh/tree/master/examples/external)
 * [Using tetgen with CADMesh](https://github.com/christopherpoole/CADMesh/tree/master/examples/tetrahedra)

![cadmesh screenshot](https://raw.github.com/christopherpoole/CADMesh/master/screenshot.png)

---

## Dependencies
If you need to read PLY, STL or OBJ files, there are no dependencies (other than Geant4 obviously).

Optional dependencies:
 * [ASSIMP](https://github.com/assimp/assimp)
 * [TETGEN](http://tetgen.org/)

If you really want to install them, on a Debian like distribution you can run:
```
    apt-get install libtet1.5-dev libassimp-dev
```

## Installation
Download the latest release of CADMesh (the CADMesh.hh file) from the [Downloads](https://github.com/christopherpoole/CADMesh/releases) section.
Copy `CADMesh.hh` to the `includes` directory of your project.
That's it.

## Further Reading
Importing predefined CAD models into [GEANT4](http://www.geant4.org/geant4/) is not always possible or requires intermediate file format conversion to Geometry Description Markup Language (GDML) using commercial or third party software.
CADMesh is a direct CAD model import interface for GEANT4 leveraging [ASSIMP](https://github.com/assimp/assimp) for reading the CAD files.
Currently it supports the import of triangular facet surface meshes defined in formats such as STL and PLY. A G4TessellatedSolid is returned and can be included in a standard user detector constructor.
You can find the project page and a git repository [here](https://github.com/christopherpoole/CADMesh).
The old [Google Code page for CADMesh](http://code.google.com/p/cadmesh/) redirects to this page.

Additional functionality is included for the fast navigation of tessellated solids by automatically creating equivalent tetrahedral meshes thereby making smart voxelisation available for the solid.

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

