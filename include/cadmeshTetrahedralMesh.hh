/********************************************************************************
* CADMesh - A CAD file interface for Geant4                                     *
*                                                                               *
* Copyright (c) 2017      Simon Kirchhof <s.kirchhof@dkfz-heidelberg.de>        *
*                                                                               *
* Permission is hereby granted, free of charge, to any person obtaining a copy  *
* of this software and associated documentation files (the "Software"),         *
* to deal in the Software without restriction, including without limitation     *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,      *
* and/or sell copies of the Software, and to permit persons to whom             *
* the Software is furnished to do so, subject to the following conditions:      *
*                                                                               *
* The above copyright notice and this permission notice shall be included       *
* in all copies or substantial portions of the Software.                        *
*                                                                               *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,               *
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES               *
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,                 *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE    *
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 *
********************************************************************************/
#pragma once

#include <memory>
#include <vector>

#include <G4String.hh>
#include <G4Types.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tet.hh>
#include <G4AssemblyVolume.hh>
#include <G4VisExtent.hh>

#include "cadmeshRegionAttributes.hh"


namespace cadmesh {


struct Tet
{
  // shape 
  G4Tet* solid;

  // ELE file associate an integer attribute with each tetrahedron
  // to define to which region, i.e. "meta-shape", it belongs
  G4int regionID;
};


// a customised G4AssemblyVolume
class TetAssemblyVolume
{
  public:
    TetAssemblyVolume();

    // non-copyable
    TetAssemblyVolume(const TetAssemblyVolume&) = delete;
    TetAssemblyVolume& operator=(const TetAssemblyVolume&) = delete;

    // but movable
    TetAssemblyVolume(TetAssemblyVolume&&) = default;
    TetAssemblyVolume& operator=(TetAssemblyVolume&&) = default;

    const std::vector<G4int>& GetRegionIDs() const;
    G4VisExtent GetVisExtent() const;
    std::size_t GetNumberOfTetrahedra() const;

  public:
    // Analogue to G4AssemblyVolume::AddPlacedVolume(), accepts only
    // logical volumes made from solids of entity type "G4Tet".
    void AddTetrahedron(G4LogicalVolume* volume, G4int regionID);
    
    // Analogue to G4AssemblyVolume::MakeImprint()
    void MakeImprint(G4LogicalVolume* motherVolume,
                     const G4ThreeVector& translation = G4ThreeVector(),
                     G4RotationMatrix* rotationInMother = nullptr,
                     G4bool surfaceCheck = false);

    G4VPhysicalVolume* GetImprintedPhysicalVolume(G4int index);
    std::size_t TotalImprintedVolumes() const;

    std::size_t GetImprintsCount() const;
    std::size_t GetInstanceCount() const;
    std::size_t GetAssemblyID() const;

  private:
    // owner of the physical volumes and their rotation matrices
    std::unique_ptr<G4AssemblyVolume> fTetAssembly;

    // contains one region ID for each tetrahedron
    std::vector<G4int> fRegionIDs;

    // extent of the tetrahedral mesh
    G4double fXmin, fXmax, fYmin, fYmax, fZmin, fZmax;
};


class TetrahedralMeshFileReader
{
  public:
    explicit TetrahedralMeshFileReader(G4double unitOfLength = mm);

    // Reads a tetrahedral mesh from file. ELE (TetGen) is the only supported 
    // file type so far.
    std::vector<Tet> ReadTetrahedralMesh(const G4String& filePath);
    
    // Facilitates the assignment of material and colour, i.e. creation of
    // unique logical volumes for each tetrahedron. Logical volumes are
    // trivially placed in an assembly volume.
    std::unique_ptr<TetAssemblyVolume> ReadTetAssemblyVolume(const G4String& filePath, 
                                                             const RegionAttributeMap& ram);

    void SetUnitOfLength(G4double unitOfLength);
    G4double GetUnitOfLength();

  private:
    // implementation specifics
    std::vector<Tet> ReadELE(const G4String& filePath);

  private:
    // Geant4 internal unit, used to interpret the length scale of the meshes.
    G4double fUnitOfLength;
};


}  // namespace cadmesh


#include "cadmeshTetrahedralMesh.icc"
