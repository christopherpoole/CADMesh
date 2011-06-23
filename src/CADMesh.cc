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
#include "G4UIcommand.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4Tet.hh"

// VCGLIB //
#include "wrap/io_trimesh/import.h"
#include "wrap/io_trimesh/import_ply.h"
#include "wrap/io_trimesh/import_stl.h"
#include "wrap/io_trimesh/import_off.h"

#include "wrap/ply/plylib.h"


using namespace vcg::tri::io;

CADMesh::CADMesh(char * file_name, char * file_type, double units, G4ThreeVector offset, G4bool reverse)
{
    units_ = units;
    offset_ = offset;
    reverse_ = reverse;

    file_name_ = file_name;
    file_type_ = file_type;
    file_type_.toUpper();

    material_ = NULL;
    quality_ = 0;

    has_mesh_ = false;
    has_solid_ = false;
    verbose_ = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type, G4Material * material, double quality)
{
    units_ = mm;
    offset_ = G4ThreeVector();
    reverse_ = false;

    file_name_ = file_name;
    file_type_ = file_type;
    file_type_.toUpper();

    material_ = material;
    quality_ = quality;

    has_mesh_ = false;
    has_solid_ = false;
    verbose_ = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type)
{
    units_ = mm;
    offset_ = G4ThreeVector();
    reverse_ = false;

    file_name_ = file_name;
    file_type_ = file_type;
    file_type_.toUpper();

    material_ = NULL;
    quality_ = 0;

    has_mesh_ = false;
    has_solid_ = false;
    verbose_ = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type, G4Material * material)
{
    units_ = mm;
    offset_ = G4ThreeVector();
    reverse_ = false;

    file_name_ = file_name;
    file_type_ = file_type;
    file_type_.toUpper();

    material_ = material;
    quality_ = 0;

    has_mesh_ = false;
    has_solid_ = false;
    verbose_ = 0;
}

CADMesh::~CADMesh()
{
}

G4VSolid* CADMesh::TessellatedMesh()
{
    if (!has_mesh_) {
        if (file_type_ == "STL") {
            ImporterSTL<CADTriMesh>::Open(m, file_name_);
        } else if (file_type_ == "PLY") {
            ImporterPLY<CADTriMesh>::Open(m, file_name_);
        } else if (file_type_ == "OFF") {
            ImporterOFF<CADTriMesh>::Open(m, file_name_);
        } else {
            G4cerr << "CADMesh/LoadSTL: "
                   << "No G4TessellatedSoild to return. Specify valid mesh type (STL, PLY, OFF), not: "
                   << file_type_
                   << G4endl;
            has_mesh_ = false;

            return 0;
        }
        has_mesh_ = true;

    } else {
        G4cerr << "CADMesh/LoadSTL: "
               << "Mesh already loaded from "
               << file_name_
               << ", not loading. Use CADMesh/GetSolid to get the currently loaded mesh as a G4TessellatedSolid"
               << G4endl;
        return 0;
    }

    if (!has_mesh_) {
        G4cerr << "CADMesh/BuildSolid: "
               << "Load a mesh of type STL, PLY or OFF first."
               << G4endl;
        return 0;
    }

    volume_solid = new G4TessellatedSolid(file_name_);

    G4ThreeVector point_1 = G4ThreeVector();
    G4ThreeVector point_2 = G4ThreeVector();
    G4ThreeVector point_3 = G4ThreeVector();

    CADTriMesh::FaceIterator face_iterator;

    for(face_iterator=m.face.begin(); face_iterator!=m.face.end(); ++face_iterator)
    {
        point_1 = G4ThreeVector(
                (*face_iterator).V(0)->P()[0] * units_ - offset_.x(),
                (*face_iterator).V(0)->P()[1] * units_ - offset_.y(),
                (*face_iterator).V(0)->P()[2] * units_ - offset_.z());
        point_2 = G4ThreeVector(
                (*face_iterator).V(1)->P()[0] * units_ - offset_.x(),
                (*face_iterator).V(1)->P()[1] * units_ - offset_.y(),
                (*face_iterator).V(1)->P()[2] * units_ - offset_.z());
        point_3 = G4ThreeVector(
                (*face_iterator).V(2)->P()[0] * units_ - offset_.x(),
                (*face_iterator).V(2)->P()[1] * units_ - offset_.y(),
                (*face_iterator).V(2)->P()[2] * units_ - offset_.z());
                
        G4TriangularFacet * facet;
        if (reverse_ == false) {
            facet = new G4TriangularFacet(point_1, point_2, point_3, ABSOLUTE);
        } else {
            facet = new G4TriangularFacet(point_2, point_1, point_3, ABSOLUTE);
        }
        volume_solid->AddFacet((G4VFacet*) facet);
    }

    volume_solid->SetSolidClosed(true);

    return volume_solid;
}

#ifndef NOTET
G4AssemblyVolume * CADMesh::TetrahedralMesh()
{
    // USAGE: assembly->MakeImprint(world_logical, assembly_transform_3d, 0); //

    G4bool do_tet = true;
    if (file_type_ == "STL") {
        G4bool state = in.load_stl(file_name_);
    } else if (file_type_ == "PLY") {
        G4bool state = in.load_ply(file_name_);
    } else if (file_type_ == "OFF") {
        G4bool state = out.load_tetmesh(file_name_);
        do_tet = false;
    }

    if (do_tet)
    {
        G4String config = G4String("Yp");
        if (quality_ > 0) config = config + G4String("q") + G4UIcommand::ConvertToString(quality_);
        tetrahedralize((char *) config.c_str(), &in, &out);
    }

    assembly = new G4AssemblyVolume();
    G4RotationMatrix * element_rotation = new G4RotationMatrix();
    G4ThreeVector element_position = G4ThreeVector();
    G4Transform3D assembly_transform = G4Translate3D();

    for (int i=0; i<out.numberoftetrahedra; i++) {
        int offset = i * 4; /* For a tetrahedron, out.numberofcorners == 4 */

        G4ThreeVector p1 = GetTetPoint(offset);
        G4ThreeVector p2 = GetTetPoint(offset + 1);
        G4ThreeVector p3 = GetTetPoint(offset + 2);
        G4ThreeVector p4 = GetTetPoint(offset + 3);

        G4VSolid * tet_solid = new G4Tet(G4String("tet_solid_") + G4UIcommand::ConvertToString(i), p1, p2, p3, p4, 0);
        G4LogicalVolume * tet_logical = new G4LogicalVolume(tet_solid, material_, G4String("tet_logical_") + G4UIcommand::ConvertToString(i), 0, 0, 0);
        assembly->AddPlacedVolume(tet_logical, element_position, element_rotation);

#ifdef DEBUG
        if (i%1000 == 0) G4cout << "Tetrahedrons added: " << i << G4endl;
#endif
    }

#ifdef DEBUG
        G4cout << "Loading of " << out.numberoftetrahedra << "tetrahedrons complete." << G4endl;
#endif

    return assembly;
}

G4ThreeVector CADMesh::GetTetPoint(G4int offset)
{
    return G4ThreeVector(out.pointlist[out.tetrahedronlist[offset]*3],
                         out.pointlist[out.tetrahedronlist[offset]*3+1],
                         out.pointlist[out.tetrahedronlist[offset]*3+2]);
}
#endif
