/********************************************************************************
* CADMesh - A CAD file interface for Geant4                                     *
*                                                                               *
* Copyright (c) 2011-2014 Christopher M. Poole <mail@christopherpoole.net>      *
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

#include <vector>

#include <G4Types.hh>
#include <G4String.hh>
#include <G4SystemOfUnits.hh>
#include <G4TessellatedSolid.hh>

#include "cadmeshRegionAttributes.hh"


namespace cadmesh {


// Groups geometrical information and meta data.
struct PolygonMesh
{
  // Geant4 solid that allows to describe shapes by an enclosed 
  // mesh of polygons (currently tri- and quadrangles).
  G4TessellatedSolid* tessellatedSolid;
  
  // SMESH files may associate an integer attribute to each mesh.
  G4int meshID;

  // Other file types, e.g. STL, name meshes.
  G4String name;
};


// a tree hierarchy of polygon mesh identifiers
struct PolygonMeshHierarchyNode
{
  PolygonMeshHierarchyNode(G4int anID, const G4String& aName = "")
    : meshID(anID), name(aName), children()
  {
  }
  
  G4int meshID;
  G4String name;

  std::vector<PolygonMeshHierarchyNode> children;
};


class PolygonMeshFileReader
{
  public:
    PolygonMeshFileReader(G4double unitOfLength = mm, G4bool permuteFacetPoints = false);

    // Reads a collection of polygon meshes from a file. Supported file
    // types are: STL, PLY, OFF, OBJ and SMESH.
    std::vector<PolygonMesh> ReadMeshCollection(const G4String& filePath);

    // Similar to ReadMeshCollection(). Returns the first mesh defined.
    PolygonMesh ReadMesh(const G4String& filePath);

    void SetUnitOfLength(G4double unitOfLength) { fUnitOfLength = unitOfLength; }
    void SetPermuteFacetPoints(G4bool permuteFacetPoints) { fPermuteFacetPoints = permuteFacetPoints; }
    G4double GetUnitOfLength() { return fUnitOfLength; }
    G4bool GetPermuteFacetPoints() { return fPermuteFacetPoints; }

  private:
    // implementation specifics
    std::vector<PolygonMesh> ReadAssimpFileType(const G4String& filePath);
    std::vector<PolygonMesh> ReadSMESH(const G4String& filePath);

  private:
    // Geant4 internal unit, used to interpret the length scale of the meshes.
    G4double fUnitOfLength;

    // If set to true, will invoke cyclic permutation of facet points upon reading, 
    // which yields opposite facet normals.
    G4bool fPermuteFacetPoints;
};


G4LogicalVolume* NestMeshes(const std::vector<PolygonMesh>& meshCollection,
                            const RegionAttributeMap& regionAttributeMap,
                            const PolygonMeshHierarchyNode& root);


}  // namespace cadmesh
