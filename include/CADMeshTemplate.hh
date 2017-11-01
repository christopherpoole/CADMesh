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

// CADMesh //
#include "Configuration.hh"
#include "FileTypes.hh"
#include "Reader.hh"

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

    CADMeshTemplate( G4String file_name);

    CADMeshTemplate( G4String file_name
                   , File::Type file_type);

    CADMeshTemplate( std::shared_ptr<File::Reader> reader);

    CADMeshTemplate( G4String file_name
                   , std::shared_ptr<File::Reader> reader);

    CADMeshTemplate( G4String file_name
                   , File::Type file_type
                   , std::shared_ptr<File::Reader> reader);

    static std::shared_ptr<T> FromPLY( G4String file_name);

    static std::shared_ptr<T> FromPLY( G4String file_name
                                     , std::shared_ptr<File::Reader> reader);

    ~CADMeshTemplate();

  public:
    virtual G4VSolid* GetSolid() = 0;
    virtual G4VSolid* GetSolid(G4int index) = 0;
    virtual G4VSolid* GetSolid(G4String name) = 0;

    virtual G4AssemblyVolume* GetAssembly() = 0;

    bool IsValidForNavigation();
    // TODO: Add method with same arguments as GetSolid.

  public:
    G4String GetFileName();

    File::Type GetFileType();

    void SetVerbose(G4int verbose);
    G4int GetVerbose();

    void SetScale(G4double scale);
    G4double GetScale();

    void SetOffset(G4ThreeVector offset);
    G4ThreeVector GetOffset();

  protected:
    G4String file_name_;
    File::Type file_type_;
    G4int verbose_;
    G4double scale_;
    G4ThreeVector offset_;

    G4AssemblyVolume* assembly_ = nullptr;

    std::shared_ptr<File::Reader> reader_ = nullptr;
};

}

