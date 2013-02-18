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
#include "G4ThreeVector.hh"
#include "G4Material.hh"

#include "GL/gl.h"
#include "GL/glu.h"

#ifndef NOVCGLIB
// VCGLIB //
#include "wrap/io_trimesh/import.h"
#include "wrap/io_trimesh/import_dae.h"
#include "wrap/io_trimesh/import_ply.h"
#include "wrap/io_trimesh/import_stl.h"
#include "wrap/io_trimesh/import_off.h"
#include "wrap/ply/plylib.h"

using namespace vcg::tri::io;
#endif

#ifndef NOASSIMP
CADMesh::CADMesh(char * file_name, double units,
        G4ThreeVector offset, G4bool reverse)
{
    this->units = units;
    this->offset = offset;
    this->reverse = reverse;

    this->file_name = file_name;
    this->file_type = "ASSIMP";
    this->file_type.toUpper();

    this->material = NULL;
    this->quality = 0;

    this->has_mesh = false;
    this->has_solid = false;
    this->verbose = 0;
}
#endif

CADMesh::CADMesh(char * file_name, char * file_type, double units,
        G4ThreeVector offset, G4bool reverse)
{
    this->units = units;
    this->offset = offset;
    this->reverse = reverse;

    this->file_name = file_name;
    this->file_type = file_type;
    this->file_type.toUpper();

    this->material = NULL;
    this->quality = 0;

    this->has_mesh = false;
    this->has_solid = false;
    this->verbose = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type,
        G4Material * material, double quality)
{
    this->units = mm;
    this->offset = G4ThreeVector();
    this->reverse = false;

    this->file_name = file_name;
    this->file_type = file_type;
    this->file_type.toUpper();

    this->material = material;
    this->quality = quality;

    this->has_mesh = false;
    this->has_solid = false;
    this->verbose = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type)
{
    this->units = mm;
    this->offset = G4ThreeVector();
    this->reverse = false;

    this->file_name = file_name;
    this->file_type = file_type;
    this->file_type.toUpper();

    this->material = NULL;
    this->quality = 0;

    this->has_mesh = false;
    this->has_solid = false;
    this->verbose = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type, G4Material * material)
{
    this->units = mm;
    this->offset = G4ThreeVector();
    this->reverse = false;

    this->file_name = file_name;
    this->file_type = file_type;
    this->file_type.toUpper();

    this->material = material;
    this->quality = 0;

    this->has_mesh = false;
    this->has_solid = false;
    this->verbose = 0;
}

CADMesh::CADMesh(char * file_name, char * file_type,
        G4Material * material, double quality, G4ThreeVector offset)
{
    this->units = mm;
    this->offset = offset;
    this->reverse = false;

    this->file_name = file_name;
    this->file_type = file_type;
    this->file_type.toUpper();

    this->material = material;
    this->quality = quality;

    this->has_mesh = false;
    this->has_solid = false;
    this->verbose = 0;
}

CADMesh::~CADMesh()
{
}

#ifndef NOASSIMP
G4VSolid* CADMesh::TessellatedMesh()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file_name,
            aiProcess_Triangulate           |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace);

    m = scene->mMeshes[0];

    volume_solid = new G4TessellatedSolid(file_name);

    G4ThreeVector point_1;
    G4ThreeVector point_2;
    G4ThreeVector point_3;

    for(unsigned int i=0; i < m->mNumFaces; i++)
    {
        const aiFace& face = m->mFaces[i];

        point_1.setX(m->mVertices[face.mIndices[0]].x);
        point_1.setY(m->mVertices[face.mIndices[0]].y);
        point_1.setZ(m->mVertices[face.mIndices[0]].z);

        point_2.setX(m->mVertices[face.mIndices[1]].x);
        point_2.setY(m->mVertices[face.mIndices[1]].y);
        point_2.setZ(m->mVertices[face.mIndices[1]].z);

        point_3.setX(m->mVertices[face.mIndices[2]].x);
        point_3.setY(m->mVertices[face.mIndices[2]].y);
        point_3.setZ(m->mVertices[face.mIndices[2]].z);
        
        G4TriangularFacet * facet;
        if (reverse == false) {
            facet = new G4TriangularFacet(point_1, point_2, point_3, ABSOLUTE);
        } else {
            facet = new G4TriangularFacet(point_2, point_1, point_3, ABSOLUTE);
        }
        volume_solid->AddFacet((G4VFacet*) facet);
    }

    volume_solid->SetSolidClosed(true);

    if (volume_solid->GetNumberOfFacets() == 0) {
        G4cerr << "CADMesh/TessellatedMesh: "
               << "Load a mesh has 0 faces, " << file_name << " may not exist."
               << G4endl;
        return 0;
    }

    return volume_solid;
}
#endif

#ifndef NOVCGLIB
G4VSolid* CADMesh::TessellatedMesh()
{
    if (!has_mesh) {
        if (file_type == "STL") {
            ImporterSTL<CADTriMesh>::Open(m, file_name);
        } else if (file_type == "PLY") {
            ImporterPLY<CADTriMesh>::Open(m, file_name);
        } else if (file_type == "OFF") {
           ImporterOFF<CADTriMesh>::Open(m, file_name);
        } else if (file_type == "COLLADA") {
            InfoDAE info = InfoDAE();
            ImporterDAE<CADTriMesh>::Open(m, file_name, info, 0);
        } else {
            G4cerr << "CADMesh/TessellatedMesh: "
                   << "No G4TessellatedSoild to return. Specify valid mesh type (STL, PLY, OFF, COLLADA), not: "
                   << file_type
                   << G4endl;
            has_mesh = false;

            return 0;
        }
        has_mesh = true;

    } else {
        G4cerr << "CADMesh/TessellatedMesh: "
               << "Mesh already loaded from "
               << file_name
               << ", not loading. Use CADMesh/GetSolid to get the currently loaded mesh as a G4TessellatedSolid"
               << G4endl;
        return 0;
    }

    if (!has_mesh) {
        G4cerr << "CADMesh/TessellatedMesh: "
               << "Load a mesh of type STL, PLY or OFF first."
               << G4endl;
        return 0;
    }

    volume_solid = new G4TessellatedSolid(file_name);

    G4ThreeVector point_1 = G4ThreeVector();
    G4ThreeVector point_2 = G4ThreeVector();
    G4ThreeVector point_3 = G4ThreeVector();

    CADTriMesh::FaceIterator face_iterator;

    for(face_iterator=m.face.begin(); face_iterator!=m.face.end(); ++face_iterator)
    {
        point_1 = G4ThreeVector(
                (*face_iterator).V(0)->P()[0] * units + offset.x(),
                (*face_iterator).V(0)->P()[1] * units + offset.y(),
                (*face_iterator).V(0)->P()[2] * units + offset.z());
        point_2 = G4ThreeVector(
                (*face_iterator).V(1)->P()[0] * units + offset.x(),
                (*face_iterator).V(1)->P()[1] * units + offset.y(),
                (*face_iterator).V(1)->P()[2] * units + offset.z());
        point_3 = G4ThreeVector(
                (*face_iterator).V(2)->P()[0] * units + offset.x(),
                (*face_iterator).V(2)->P()[1] * units + offset.y(),
                (*face_iterator).V(2)->P()[2] * units + offset.z());
                
        G4TriangularFacet * facet;
        if (reverse == false) {
            facet = new G4TriangularFacet(point_1, point_2, point_3, ABSOLUTE);
        } else {
            facet = new G4TriangularFacet(point_2, point_1, point_3, ABSOLUTE);
        }
        volume_solid->AddFacet((G4VFacet*) facet);
    }

    volume_solid->SetSolidClosed(true);

    if (volume_solid->GetNumberOfFacets() == 0) {
    G4cerr << "CADMesh/TessellatedMesh: "
               << "Load a mesh has 0 faces, " << file_name << " may not exist."
               << G4endl;
        return 0;
    }

    return volume_solid;
}
#endif

#ifndef NOTET
G4AssemblyVolume * CADMesh::TetrahedralMesh()
{
    // USAGE: assembly->MakeImprint(world_logical, assembly_transform_3d, 0); //

    G4bool do_tet = true;
    if (file_type == "STL") {
        G4bool state = in.load_stl(file_name);
    } else if (file_type == "PLY") {
        G4bool state = in.load_ply(file_name);
    } else if (file_type == "TET") {
        G4bool state = out.load_tetmesh(file_name);
        do_tet = false;
    } else if (file_type == "OFF") {
        G4bool state = out.load_off(file_name);
        do_tet = false;
    }

    if (do_tet)
    {
        G4String config = G4String("Yp");
        if (quality > 0) config = config + G4String("q") + G4UIcommand::ConvertToString(quality);
#ifdef DEBUG
        G4cout << "Tetrahedralisation configuration: " << config << G4endl;
#endif
        tetrahedralize((char *) config.c_str(), &in, &out);
    }

#ifdef DEBUG
    G4cout << "Tetrahedra available: " << out.numberoftetrahedra << G4endl;
#endif

    assembly = new G4AssemblyVolume();
    G4RotationMatrix * element_rotation = new G4RotationMatrix();
    G4ThreeVector element_position = G4ThreeVector();
    G4Transform3D assembly_transform = G4Translate3D();

    for (int i=0; i<out.numberoftetrahedra; i++) {
        int index_offset = i * 4; /* For a tetrahedron, out.numberofcorners == 4 */

        G4ThreeVector p1 = GetTetPoint(index_offset);
        G4ThreeVector p2 = GetTetPoint(index_offset + 1);
        G4ThreeVector p3 = GetTetPoint(index_offset + 2);
        G4ThreeVector p4 = GetTetPoint(index_offset + 3);

        G4String tet_name = file_name + G4String("_tet_") + G4UIcommand::ConvertToString(i);

        G4VSolid * tet_solid = new G4Tet(tet_name + G4String("_solid"), p1, p2, p3, p4, 0);
        G4LogicalVolume * tet_logical = new G4LogicalVolume(tet_solid, material, tet_name + G4String("_logical"), 0, 0, 0);
        assembly->AddPlacedVolume(tet_logical, element_position, element_rotation);

#ifdef DEBUG
        if (i%1000 == 0) G4cout << "Tetrahedrons added: " << i << G4endl;
#endif
    }

#ifdef DEBUG
    G4cout << "Loading of " << out.numberoftetrahedra << " tetrahedra complete with material: " << material->GetName() << G4endl;
#endif

    return assembly;
}

G4ThreeVector CADMesh::GetTetPoint(G4int index_offset)
{
    return G4ThreeVector(out.pointlist[out.tetrahedronlist[index_offset]*3] - offset.x(),
            out.pointlist[out.tetrahedronlist[index_offset]*3+1] - offset.y(),
            out.pointlist[out.tetrahedronlist[index_offset]*3+2] - offset.z());
}
#endif
