=Introduction=
Importing predefined CAD models into [http://www.geant4.org/geant4/ GEANT4] is not always possible or requires intermediate file format conversion to Geometry Description Markup Language (GDML) using commercial or third party software. CADMesh is a direct CAD model import interface for GEANT4 leveraging [http://vcg.sourceforge.net/index.php/Main_Page VCGLIB]. Currently it supports the import of triangular facet surface meshes defined in formats such as STL and PLY. A G4TessellatedSolid is returned and can be included in a standard user detector constructor.

Additional functionality is included for the fast navigation of tessellated solids by automatically creating equivalent tetrahedral meshes thereby making smart voxelisation available for the solid.

===Citing CADMesh===
CADMesh is described in a technical paper, available [http://eprints.qut.edu.au/53299/ here]. If you would like the cite the article, you may wish use the following bibtex entry:
{{{
@article{poole2012acad,
    author = {Poole, C. M. and Cornelius, I. and Trapp, J. V. and Langton, C. M.},
    title = {{A CAD Interface for GEANT4}},
    journal={Australasian Physical & Engineering Science in Medicine},
    year = 2012,
    month = September,
    doi = {10.1007/s13246-012-0159-8},
    url = {http://www.springerlink.com/content/u563877422284578}
}
}}}
The fast tessellated solid navigation technique included in CADMesh is described [http://eprints.qut.edu.au/52696/ here]:
{{{
@article{poole2012fast,
    title={Fast Tessellated Solid Navigation in GEANT4},
    author={Poole, C. and Cornelius, I. and Trapp, J. and Langton, C.M.},
    journal={IEEE Transactions on Nuclear Science},
    volume={99},
    pages={1--7},
    year={2012},
    publisher={Institute of Electrical and Electronics Engineers}
}
}}}

=Example Usage=
The following shows basic usage for cadmesh in a UserDetectorConstruction, a complete example can be found [http://code.google.com/p/cadmesh/source/browse/#hg%2Fexamples%2Fbasic here]
{{{
// User Detector Constructor
#include "CADMesh.hh"
...
G4ThreeVector offset;
G4VSolid * cad_solid;
G4LogicalVolume * cad_logical;
G4VPhysicalVolume * cad_physical;
...
offset = G4ThreeVector(-30*cm, 0, 0);
CADMesh * mesh = new CADMesh("../../models/cone.ply", "PLY", mm, offset, false);

cad_solid = mesh->TessellatedMesh();
cad_logical = new G4LogicalVolume(cad_solid, water, "cad_logical", 0, 0, 0);
cad_physical = new G4PVPlacement(0, G4ThreeVector(), cad_logical,
                                     "cad_physical", world_logical, false, 0);
...

# GNUMakefile
name := example
G4TARGET := $(name)
G4EXLIB := true

include cadmesh.gmk

.PHONY: all 
all: lib bin 

include $(G4INSTALL)/config/binmake.gmk

}}}

=Dependencies=
A working installation of the folowing libraries or packages is required:
 * GEANT4
 * VCGLIB
 * TETGEN
 * QT & OpenGL (for COLLADA support)

=Installation (v0.7 and above)=
For installation of v0.5 and below see the wikipage [OldInstallation]. For the old gmake installation method see [GMakeInstallation]. Now we use cmake!

Download the latest release of CADMesh from the [http://code.google.com/p/cadmesh/downloads/list Downloads] section, and extract the archive, or checkout the development branch from the repository. Additionally, download VCGLIB (you can find more information about acquiring VCGLIB [http://vcg.sourceforge.net/index.php/Tutorial#Getting_VCG_Lib here]) and download tetgen from [http://tetgen.berlios.de/ here] (on ubuntu you can `sudo apt-get install libtet1.4.2-dev`).

{{{
# Checkout VCGLIB
svn co https://vcg.svn.sourceforge.net/svnroot/vcg/trunk/vcglib vcglib -r 4041

# CADMesh latest dev branch
hg clone https://code.google.com/p/cadmesh/ 
# OR with source from downloads section
tar xvjf cadmesh-VERSION.tar.bz2

cd cadmesh/
mkdir build
cd build

# Default
cmake .. -DVCGLIB_DIR="/path/to/vcglib"

# For CAD/VCGLIB only 
cmake .. -DVCGLIB_DIR="/path/to/vcglib" -WITH_TETGEN="OFF"

# FOR TET only
cmake .. -DWITH_VCGLIB="OFF"
# OR
cmake .. -DWITH_VCGLIB="OFF" -DTETGEN_DIR="/path/to/tetgen"

make
sudo make install

}}}

Here is a basic example usage. The CMakeLists.txt file illustrates how to link against CADMesh. The VCGLIB_DIR variable can be hard - coded if you wish.
{{{

# Run example (Linux with g++)
cd examples/basic
mkdir build
cd build

cmake .. -DVCGLIB_DIR="/path/to/vcglib"
make
cd ..

./build/cadmesh_example
Idle> exit

