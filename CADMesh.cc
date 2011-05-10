/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE
 *
 * File:      CADMesh.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

// USER //
#include "CADMesh.hh"

// GEANT4 //
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"

// VCGLIB //
#include "wrap/io_trimesh/import.h"
#include "wrap/io_trimesh/import_ply.h"
#include "wrap/io_trimesh/import_stl.h"
#include "wrap/io_trimesh/import_off.h"

#include "wrap/ply/plylib.h"


using namespace vcg::tri::io;

CADMesh::CADMesh()
{
    has_mesh = false;
    has_solid = false;

    verbose = 0;
}
CADMesh::~CADMesh()
{
}

G4VSolid* CADMesh::LoadMesh(char * file, char * type, G4ThreeVector iso) {
    if (!has_mesh) {
        file_name = file;

        G4String file_type = type;
        file_type.toUpper();

        if (file_type == "STL") {
            ImporterSTL<G4CADTriMesh>::Open(m, file_name);
        } else if (file_type == "PLY") {
            ImporterPLY<G4CADTriMesh>::Open(m, file_name);
        } else if (file_type == "OFF") {
            ImporterOFF<G4CADTriMesh>::Open(m, file_name);
        } else {
            G4cerr << "CADMesh/LoadSTL: "
                   << "No G4TessellatedSoild to return. Specify valid mesh type (STL, PLY, OFF), not: "
                   << file_type
                   << G4endl;
            has_mesh = false;

            return 0;
        }
        has_mesh = true;

    } else {
        G4cerr << "CADMesh/LoadSTL: "
               << "Mesh already loaded from "
               << file_name
               << ", not loading. Use CADMesh/GetSolid to get the currently loaded mesh as a G4TessellatedSolid"
               << G4endl;
        return 0;
    }

    name = file_name;
    return BuildSolid();
}


G4VSolid* CADMesh::BuildSolid() {
    if (!has_mesh) {
        G4cerr << "CADMesh/BuildSolid: "
               << "Load a mesh of type STL, PLY or OFF first."
               << G4endl;
        return 0;
    }

    volume_solid = new G4TessellatedSolid(name);

    G4ThreeVector point_1 = G4ThreeVector();
    G4ThreeVector point_2 = G4ThreeVector();
    G4ThreeVector point_3 = G4ThreeVector();

    CADMesh::FaceIterator face_iterator;

    for(face_iterator=m.face.begin(); face_iterator!=m.face.end(); ++face_iterator)
    {
        point_1 = G4ThreeVector(
                (*face_iterator).V(0)->P()[0] * mm,
                (*face_iterator).V(0)->P()[1] * mm,
                (*face_iterator).V(0)->P()[2] * mm);
        point_2 = G4ThreeVector(
                (*face_iterator).V(1)->P()[0] * mm,
                (*face_iterator).V(1)->P()[1] * mm,
                (*face_iterator).V(1)->P()[2] * mm);
        point_3 = G4ThreeVector(
                (*face_iterator).V(2)->P()[0] * mm,
                (*face_iterator).V(2)->P()[1] * mm,
                (*face_iterator).V(2)->P()[2] * mm);

        G4TriangularFacet *facet = new G4TriangularFacet(point_1, point_2, point_3, ABSOLUTE);
        volume_solid->AddFacet((G4VFacet*) facet);
    }

    volume_solid->SetSolidClosed(true);

    return volume_solid;
}

