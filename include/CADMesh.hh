/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      CADMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

#ifndef CADMesh_HH
#define CADMesh_HH

// GEANT4 //
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4Tet.hh"
#include "G4AssemblyVolume.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

// TETGEN //
#include "tetgen.h"

// Open Asset Importer Library //
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


class CADMesh {
  public:
    CADMesh(char * file, double units, G4ThreeVector offset, G4bool reverse);
    CADMesh(char * file, char * type, double units, G4ThreeVector offset, G4bool reverse);
    CADMesh(char * file, char * type, G4Material * material, double quality);
    CADMesh(char * file, char * type, G4Material * material, double quality, G4ThreeVector offset);
    CADMesh(char * file, char * type);
    CADMesh(char * file, char * type, G4Material * material);
    ~CADMesh();

  public:
    G4AssemblyVolume * TetrahedralMesh();

    G4AssemblyVolume * GetAssembly() {
        return assembly;
    };

    int get_input_point_count() {
        return in.numberofpoints;
    };

    int get_output_point_count() {
        return out.numberofpoints;
    };

    int get_tetrahedron_count() {
        return out.numberoftetrahedra;
    };

  private:
    G4ThreeVector GetTetPoint(G4int index_offset);
    tetgenio in, out;
    G4AssemblyVolume * assembly;
  
  public:
    G4VSolid* TessellatedMesh(G4int index);
    
    G4VSolid* TessellatedMesh() {
        return TessellatedMesh(0);   
    };
 
    G4VSolid* TessellatedMesh(G4String name) {
        Assimp::Importer importer;
        scene = importer.ReadFile(file_name,
                aiProcess_Triangulate           |
                aiProcess_JoinIdenticalVertices |
                aiProcess_CalcTangentSpace);

        for (G4int index = 0; index < scene->mNumMeshes; index++) {
            aiMesh* mesh = scene->mMeshes[index];

            if (strcmp(mesh->mName.C_Str(), name.c_str()))
                return TessellatedMesh(index);
        }

        G4cerr << "Mesh " << name << " not found. Cannot be loaded." << G4endl;
    };

    G4TessellatedSolid * GetSolid() {
        return volume_solid;
    };

    G4String MeshName() {
        return file_name;
    };

    void SetVerbose(int verbose) {
        this->verbose = verbose;
    };

  private:
    G4TessellatedSolid * volume_solid;

    const aiScene* scene;
    aiMesh* m;

  private:
    G4int verbose;
    G4bool has_mesh;
    G4bool has_solid;

    G4String name;
    G4double units;
    G4ThreeVector offset;
    G4bool reverse;

    G4double quality;
    
    G4Material * material;

    char * file_name;
    G4String file_type;
};

#endif // CADMesh_HH

