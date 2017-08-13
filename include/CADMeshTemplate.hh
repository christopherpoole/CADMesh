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

// STL //
#include <memory>

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


namespace CADMesh
{

template <typename T>
class CADMeshTemplate
{
  public:
    CADMeshTemplate();
    CADMeshTemplate(char * file_name);
    CADMeshTemplate(char * file_name, char * file_type);

    static std::shared_ptr<T> FromPLY(char* file_name);

    ~CADMeshTemplate();

  protected:
    void Init();

  public:
    G4String GetFileName() {
        return this->file_name_;
    };

    G4String GetFileType() {
        return this->file_type_;
    };

    void SetVerbose(G4int verbose) {
        this->verbose_ = verbose;
    };

    G4int GetVerbose() {
        return this->verbose_;
    };

    void SetScale(G4double scale) {
        this->scale_ = scale;
    };

    G4double GetScale() {
        return this->scale_;
    };

    void SetOffset(G4ThreeVector offset) {
        this->offset_ = offset;
    };
    
    G4ThreeVector GetOffset() {
        return this->offset_;
    };

  protected:
    G4String file_name_;
    G4String file_type_;

    G4int verbose_;
    
    G4double scale_;
   
    G4ThreeVector offset_;
};

}

