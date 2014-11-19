# Introduction
Importing predefined CAD models into [GEANT4](http://www.geant4.org/geant4/) is not always possible or requires intermediate file format conversion to Geometry Description Markup Language (GDML) using commercial or third party software.
CADMesh is a direct CAD model import interface for GEANT4 leveraging [ASSIMP](http://assimp.sourceforge.net/) for reading the CAD files.
Currently it supports the import of triangular facet surface meshes defined in formats such as STL and PLY. A G4TessellatedSolid is returned and can be included in a standard user detector constructor.
You can find the project page and a git repository [here](https://github.com/christopherpoole/CADMesh).
The old [Google Code page for CADMesh](http://code.google.com/p/cadmesh/) is still kept up-to-date, however future developments occur here on GitHub.

![Screen Shot](https://raw.github.com/christopherpoole/CADMesh/master/screenshot.png)

Additional functionality is included for the fast navigation of tessellated solids by automatically creating equivalent tetrahedral meshes thereby making smart voxelisation available for the solid.

# Citing CADMesh
CADMesh is described in a technical paper, available [here](http://christopherpoole.github.com/A-CAD-interface-for-GEANT4/). If you would like the cite the article, you may wish use the following bibtex entry:

    @article{poole2012acad,
        author = {Poole, C. M. and Cornelius, I. and Trapp, J. V. and Langton, C. M.},
        title = {{A CAD Interface for GEANT4}},
        journal={Australasian Physical & Engineering Science in Medicine},
        year = 2012,
        month = September,
        doi = {10.1007/s13246-012-0159-8},
        url = {http://www.springerlink.com/content/u563877422284578}
    }

The fast tessellated solid navigation technique included in CADMesh is described [here](http://christopherpoole.github.com/Fast-tessellated-solid-navigation-in-GEANT4/):

    @article{poole2012fast,
        title={Fast Tessellated Solid Navigation in GEANT4},
        author={Poole, C. and Cornelius, I. and Trapp, J. and Langton, C.M.},
        journal={IEEE Transactions on Nuclear Science},
        volume={99},
        pages={1--7},
        year={2012},
        publisher={Institute of Electrical and Electronics Engineers}
    }

#Example Usage
The following shows basic default usage for cadmesh in a UserDetectorConstruction:

    // User Detector Constructor
    #include "CADMesh.hh"
    ...
    G4ThreeVector offset;
    G4VSolid * cad_solid;
    G4LogicalVolume * cad_logical;
    G4VPhysicalVolume * cad_physical;
    ...
    offset = G4ThreeVector(-30*cm, 0, 0);
    CADMesh * mesh = new CADMesh("../../models/cone.ply", mm, offset, false);

    cad_solid = mesh->TessellatedMesh();
    cad_logical = new G4LogicalVolume(cad_solid, water, "cad_logical", 0, 0, 0);
    cad_physical = new G4PVPlacement(0, G4ThreeVector(), cad_logical,
                                         "cad_physical", world_logical, false, 0);
    ...

    # CMakeLists.txt
    cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
    project(cadmesh_example)

    # GEANT4 core
    find_package(Geant4 REQUIRED ui_all vis_all)
    include(${Geant4_USE_FILE})
    include_directories(${PROJECT_SOURCE_DIR}/include)

    # CADMesh
    find_package(cadmesh)

    # User code
    file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
    file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

    add_executable(cadmesh_example cadmesh_example.cc ${sources} ${headers})
    target_link_libraries(cadmesh_example ${Geant4_LIBRARIES})
    target_link_libraries(cadmesh_example ${cadmesh_LIBRARIES})

# Dependencies
A working installation of the following libraries or packages is required:
 * GEANT4
 * [ASSIMP](http://assimp.sourceforge.net/)
 * [TETGEN](http://tetgen.org/)

They are included in the repository as external projects/submodules.

# Installation
Download the latest release of CADMesh from the [Downloads](https://github.com/christopherpoole/CADMesh/releases) section, and extract the archive, or checkout the development branch from the repository.

    # CADMesh latest dev branch
    git clone https://github.com/christopherpoole/CADMesh.git
    
    cd cadmesh/
    
    # Fetch the external dependancies
    git submodule update --init --recursive

    mkdir build
    cd build

    sudo make install

Here is a basic example usage. The CMakeLists.txt file illustrates how to link against CADMesh.

    # Run example (Linux with g++)
    cd examples/basic
    mkdir build
    cd build

    cmake .. 
    make
    cd ..

    ./build/cadmesh_example
    Idle> exit

