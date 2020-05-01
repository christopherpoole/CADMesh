// The MIT License (MIT)
//
// Copyright (c) 2011-2020 Christopher M. Poole <mail@christopherpoole.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
//#include "G4SystemOfUnits.hh"
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

    static std::shared_ptr<T> From( G4String file_name);

    static std::shared_ptr<T> From( G4String file_name
                                  , std::shared_ptr<File::Reader> reader);

    static std::shared_ptr<T> FromPLY( G4String file_name);

    static std::shared_ptr<T> FromPLY( G4String file_name
                                     , std::shared_ptr<File::Reader> reader);

    static std::shared_ptr<T> FromSTL( G4String file_name);

    static std::shared_ptr<T> FromSTL( G4String file_name
                                     , std::shared_ptr<File::Reader> reader);

    static std::shared_ptr<T> FromOBJ( G4String file_name);

    static std::shared_ptr<T> FromOBJ( G4String file_name
                                     , std::shared_ptr<File::Reader> reader);

    ~CADMeshTemplate();

  public:
    virtual G4VSolid* GetSolid() = 0;
    virtual G4VSolid* GetSolid(G4int index) = 0;
    virtual G4VSolid* GetSolid(G4String name, G4bool exact = true) = 0;

    virtual std::vector<G4VSolid*> GetSolids() = 0;

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

    void SetOffset(G4double x, G4double y, G4double z);
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

