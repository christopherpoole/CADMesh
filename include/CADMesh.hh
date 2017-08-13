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

#pragma once

// TETGEN //
#include "tetgen.h"

// Open Asset Importer Library //
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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
#include "G4UIcommand.hh"


class CADMesh {
  public:
    CADMesh(char * file_name);
    CADMesh(char * file_name, char * file_type);
    CADMesh(const aiScene* scene);

    //static CADMesh* FromPLY(char* file);
    //static CADMesh* FromSTL(char* file);
    //static CADMesh* FromOBJ(char* file);

    ~CADMesh();

  protected:
    Init();

  public:
    // Load tessellated meshes using ASSIMP.
    G4VSolid* TessellatedMesh();
    G4VSolid* TessellatedMesh(G4int index);
    G4VSolid* TessellatedMesh(G4String name);

    // Load tetrahedral meshes using TETGEN.
    G4AssemblyVolume * TetrahedralMesh();
    
    // Getters and setters for various properties. 
    G4TessellatedSolid * GetSolid() {
        return volume_solid;
    };

    G4String GetFileName() {
        return this->file_name;
    };

    G4String GetFileType() {
        return this->file_type;
    };

    void SetVerbose(G4int verbose_) {
        this->verbose = verbose_;
    };

    G4int GetVerbose() {
        return this->verbose;
    };

    void SetScale(G4double scale_) {
        this->scale = scale_;
    };

    G4double GetScale() {
        return this->scale;
    };

    void SetOffset(G4ThreeVector offset_) {
        this->offset = offset_;
    };
    
    G4ThreeVector GetOffset() {
        return this-> offset;
    };

    void SetReverse(G4bool reverse_) {
        this->reverse = reverse_;
    };

    G4bool GetReverse() {
        return this->reverse;
    };

    void SetMaterial(G4Material* material_) {
        this->material = material_;
    };

    G4Material* GetMaterial() {
        return this->material;
    };

    void SetQuality(G4double quality_) {
        this->quality = quality_;
    };

    G4double GetQuality() {
        return this->quality;
    };

    G4AssemblyVolume * GetAssembly() {
        return assembly;
    };

    tetgenio GetTetgenInput() {
        return in;
    };

    tetgenio GetTetgenOutput() {
        return out;
    };

  private:
    // Private helper functions.
    G4ThreeVector GetTetPoint(G4int index_offset);
 
  private:
    // For tesselated meshes.
    G4TessellatedSolid * volume_solid;

    const aiScene* scene;
    aiMesh* m;

    // For tetrahedral meshes.
    tetgenio in, out;
    G4AssemblyVolume * assembly;

    // General properties. 
    G4int verbose;
    G4bool has_mesh;
    G4bool has_solid;

    G4String name;
    G4double scale;
    G4ThreeVector offset;
    G4bool reverse;

    G4double quality;
    
    G4Material * material;

    G4String file_name;
    G4String file_type;
};

