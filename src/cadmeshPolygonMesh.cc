/********************************************************************************
* CADMesh - A CAD file interface for Geant4                                     *
*                                                                               *
* Copyright (c) 2011-2017 Christopher M. Poole <mail@christopherpoole.net>      *
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
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <globals.hh>
#include <G4ios.hh>
#include <G4Types.hh>
#include <G4String.hh>
#include <G4TessellatedSolid.hh>
#include <G4TriangularFacet.hh>
#include <G4QuadrangularFacet.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include "cadmeshLog.hh"
#include "cadmeshHelpers.hh"
#include "cadmeshRegionAttributes.hh"

#include "cadmeshPolygonMesh.hh"


//****************************************************************************************
//    Helpers
//****************************************************************************************
namespace {

G4int AddFacetsFromAiMesh(const aiMesh* inMesh, G4TessellatedSolid* solid,
                          G4double unitOfLength, G4bool permuteFacetPoints)
{
  if (inMesh == nullptr || solid == nullptr)
    return 0;

  G4int nFacets = 0;
  for(unsigned i = 0; i < inMesh->mNumFaces; ++i)
  {
    const aiFace& face = inMesh->mFaces[i];

    G4bool success = false;
    if (face.mNumIndices == 3)
    {
      const aiVector3D& v0 = inMesh->mVertices[face.mIndices[0]];
      const aiVector3D& v1 = inMesh->mVertices[face.mIndices[1]];
      const aiVector3D& v2 = inMesh->mVertices[face.mIndices[2]];
  
      G4ThreeVector p0 = G4ThreeVector(v0.x, v0.y, v0.z) * unitOfLength;
      G4ThreeVector p1 = G4ThreeVector(v1.x, v1.y, v1.z) * unitOfLength;
      G4ThreeVector p2 = G4ThreeVector(v2.x, v2.y, v2.z) * unitOfLength;
      
      if (permuteFacetPoints == true)
      {
        success = solid->AddFacet(new G4TriangularFacet(p0, p2, p1, ABSOLUTE));
      }
      else
      {
        success = solid->AddFacet(new G4TriangularFacet(p0, p1, p2, ABSOLUTE));
      }
    }
    else if (face.mNumIndices == 4)
    {
      const aiVector3D& v0 = inMesh->mVertices[face.mIndices[0]];
      const aiVector3D& v1 = inMesh->mVertices[face.mIndices[1]];
      const aiVector3D& v2 = inMesh->mVertices[face.mIndices[2]];
      const aiVector3D& v3 = inMesh->mVertices[face.mIndices[3]];

      G4ThreeVector p0 = G4ThreeVector(v0.x, v0.y, v0.z) * unitOfLength;
      G4ThreeVector p1 = G4ThreeVector(v1.x, v1.y, v1.z) * unitOfLength;
      G4ThreeVector p2 = G4ThreeVector(v2.x, v2.y, v2.z) * unitOfLength;
      G4ThreeVector p3 = G4ThreeVector(v3.x, v3.y, v3.z) * unitOfLength;

      if (permuteFacetPoints == true)
      {
        success = solid->AddFacet(new G4QuadrangularFacet(p0, p3, p2, p1, ABSOLUTE));
      }
      else
      {
        success = solid->AddFacet(new G4QuadrangularFacet(p0, p1, p2, p3, ABSOLUTE));
      }
    }
    else 
    {
      G4String errMsg = "Invalid facet, not tri- or quadrangular.";
      G4Exception("CADMesh", "INPUT_ERROR", JustWarning, errMsg);
    }
    nFacets = success ? (nFacets + 1) : nFacets;
  }

  return nFacets;
}

}  // anonymous namespace


//****************************************************************************************
//    PolygonMeshFileReader
//****************************************************************************************

cadmesh::PolygonMeshFileReader::PolygonMeshFileReader(G4double unitOfLength,
                                                      G4bool permuteFacetPoints)
  : fUnitOfLength(unitOfLength), fPermuteFacetPoints(permuteFacetPoints)
{
}

//****************************************************************************************

std::vector<cadmesh::PolygonMesh> 
cadmesh::PolygonMeshFileReader::ReadMeshCollection(const G4String& filePath)
{
  std::vector<PolygonMesh> meshCollection;

  const G4String& extension = PathSplitExt(filePath).second;
  if (extension == ".stl" || extension == ".ply" ||
      extension == ".off" || extension == ".obj")
  {
    meshCollection = ReadAssimpFileType(filePath);
  }
  else if (extension == ".smesh")
  {
    meshCollection = ReadSMESH(filePath);
  }
  else
  {
    G4ExceptionDescription description;
    description << "File Format not supported: '" << extension << "'.";
    G4Exception("CADMesh", "INVALID_READ", FatalException, description);
    return std::vector<PolygonMesh>();
  }

  return meshCollection;
}

//****************************************************************************************

cadmesh::PolygonMesh cadmesh::PolygonMeshFileReader::ReadMesh(const G4String& filePath)
{
  return ReadMeshCollection(filePath).at(0);
}

//****************************************************************************************

std::vector<cadmesh::PolygonMesh>
cadmesh::PolygonMeshFileReader::ReadAssimpFileType(const G4String& filePath)
{
  if (GetVerbosityLevel() >= INFO)
    G4cout << "CADMesh: Reading polygon mesh file '" << filePath << "'..." << G4endl;
  
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filePath, aiProcess_JoinIdenticalVertices);

  if (scene == nullptr)
  {
    G4Exception("CADMesh", "INVALID_READ", FatalException, importer.GetErrorString());
    return std::vector<cadmesh::PolygonMesh>();
  }

  std::vector<PolygonMesh> meshCollection;
  for(unsigned i = 0; i < scene->mNumMeshes; ++i)
  {
    const aiMesh* inMesh = scene->mMeshes[i];
    PolygonMesh mesh{nullptr, DEFAULT_REGION_ID, ""};

    // Note, that the file types read via Assimp do not define a surface/region ID,
    // i.e. the polygon mesh will keep the default value.

    // Adopt name, if mesh already has a name. Otherwise, construct from file name.
    if (inMesh->mName != aiString(""))
    {
      mesh.name = inMesh->mName.C_Str();
    }
    else
    {
      const G4String& fileName = cadmesh::PathSplit(filePath).second;
      const G4String& root = cadmesh::PathSplitExt(fileName).first;
      mesh.name = root + "_mesh" + std::to_string(i);
    }

    // transfer facets to Geant4's respective solid
    mesh.tessellatedSolid = new G4TessellatedSolid(mesh.name + "_solid");
    G4int nFacets = AddFacetsFromAiMesh(inMesh, mesh.tessellatedSolid,
                                        fUnitOfLength, fPermuteFacetPoints);
    mesh.tessellatedSolid->SetSolidClosed(true);

    // Add to mesh collection
    meshCollection.push_back(mesh);

    if (GetVerbosityLevel() >= DEBUG)
    {
      G4cout << "CADMesh: Obtained mesh '" << mesh.name << "' ";
      G4cout << "consisting of " << nFacets << " facets." << G4endl;
    }
  }

  if (GetVerbosityLevel() >= INFO)
    G4cout << "CADMesh: ... read " << meshCollection.size() << " meshes." << G4endl;

  return meshCollection;
}

//****************************************************************************************

std::vector<cadmesh::PolygonMesh>
cadmesh::PolygonMeshFileReader::ReadSMESH(const G4String& filePath)
{
  if (GetVerbosityLevel() >= INFO)
  {
    G4cout << "CADMesh: Trying to read enclosed polygon meshes from SMESH file:";
    G4cout << " '" << filePath << "'..." << G4endl;
  }
  
  std::ifstream smeshFileStream(filePath);
  if (smeshFileStream.is_open() == false)
  {
    G4ExceptionDescription description;
    description << "Cannot open file: '" << filePath << "'.";
    G4Exception("CADMesh", "INVALID_READ", FatalException, description);
    return std::vector<cadmesh::PolygonMesh>();
  }

  // the nodes can be defined in the first section of the SMESH file:
  std::vector<G4ThreeVector> nodes = ParseNodeSection(smeshFileStream, fUnitOfLength);

  // No nodes? In this case, the nodes might be defined in a separate NODE file.
  // E.g. for "<filePath>.smesh" there should be "<filePath>.node".
  if (nodes.size() == 0)
  {
    G4String nodeFilePath = cadmesh::PathSplitExt(filePath).first + ".node";

    if (cadmesh::GetVerbosityLevel() >= cadmesh::INFO)
      G4cout << "CADMesh: Reading nodes from '" << nodeFilePath << "'." << G4endl;

    // read mesh nodes from NODE file
    std::ifstream nodeFileStream(nodeFilePath);
    if (nodeFileStream.is_open() == false)
    {
      G4ExceptionDescription description;
      description << "Cannot open file: '" << nodeFilePath << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<cadmesh::PolygonMesh>();
    }
    nodes = ParseNodeSection(nodeFileStream, fUnitOfLength);
  }

  // Proceed with second section of SMESH file: the facets.
  // The first non-comment line should be the header, containing:
  //
  //    <# of facets> <boundary markers (0 or 1)>
  //
  std::size_t nFacets = 0;
  G4bool hasBoundaryMarker = false;

  G4String line;
  while (std::getline(smeshFileStream, line))
  {
    // skip comments & emtpy lines
    if (line.front() == '#' || line.empty())
      continue;

    std::istringstream lineParser(line);

    // read header: <# of facets> <boundary markers (0 or 1)>
    if (lineParser >> nFacets) 
      lineParser >> hasBoundaryMarker;

    if (lineParser.fail())
    {
      G4ExceptionDescription description;
      description << "Failed to parse facet section header: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<PolygonMesh>();
    }

    break;
  }

  // strings we'll need to name the solid later on
  const G4String& fileName = PathSplit(filePath).second;
  const G4String& fileNameRoot = PathSplitExt(fileName).first;

  // After the header, each line of the facet section in the SMESH file 
  // defines one facet, via the indices of specific nodes: 
  //    ...
  //    <# of corners> <corner 1> ... <corner #> [boundary marker]
  //    ...
  std::vector<PolygonMesh> meshCollection;
  std::size_t counter = 0;
  while (std::getline(smeshFileStream, line) && counter < nFacets)
  {
    // skip comments & emtpy lines
    if (line.front() == '#' || line.empty())
      continue;

    std::istringstream lineParser(line);
    ++counter;

    // read first entry: <# of corners>
    std::size_t nCorners;
    if (lineParser >> nCorners)
    {
      // abort, if polygons are used, which can't be defined in Geant4.
      if (nCorners != 3 && nCorners != 4)
      {
        G4String errMsg = "Invalid facet, not tri- or quadrangular.";
        G4Exception("CADMesh", "INPUT_ERROR", FatalException, errMsg);
        return std::vector<PolygonMesh>();
      }
    }
    else
    {
      G4ExceptionDescription description;
      description << "Failed to read facet: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<PolygonMesh>();
    }

    // read polygon corners: <corner 1> ... <corner #>
    std::vector<G4ThreeVector> corners(nCorners);
    for (auto& corner : corners)
    {
      std::size_t index;
      lineParser >> index;
      corner = nodes[index];
    }

    // get boundary marker if defined
    G4int boundaryMarker = DEFAULT_REGION_ID;
    if (hasBoundaryMarker)
      lineParser >> boundaryMarker;

    // check if parsing any of the integers has failed
    if (lineParser.fail())
    {
      G4ExceptionDescription description;
      description << "Failed to read facet: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<PolygonMesh>();
    }

    // unary predicate, returns true if a mesh's ID is equal to 
    // the parsed boundary marker
    auto FacetBelongsToMesh = [boundaryMarker](const PolygonMesh& mesh)
    {
      return mesh.meshID == boundaryMarker;
    };

    // find corresponding mesh in collection
    auto meshIter = std::find_if(meshCollection.begin(),
                                 meshCollection.end(),
                                 FacetBelongsToMesh);

    G4TessellatedSolid* currentSolid = nullptr;
    if (meshIter != meshCollection.end())
    {
      // most of the time, the respective mesh already exists
      currentSolid = meshIter->tessellatedSolid;
    }
    else
    {
      // create mesh if not yet existent
      PolygonMesh newMesh{nullptr, DEFAULT_REGION_ID, ""};

      newMesh.name = fileNameRoot + "_mesh" + std::to_string(boundaryMarker);
      newMesh.tessellatedSolid = new G4TessellatedSolid(newMesh.name + "_solid");
      newMesh.meshID = boundaryMarker;

      meshCollection.push_back(newMesh);
      currentSolid = meshCollection.back().tessellatedSolid;
    }

    // add facet to solid
    if (nCorners == 3)
    {
      if (fPermuteFacetPoints == true)
      {
        currentSolid->AddFacet(
          new G4TriangularFacet(corners[0], corners[2], corners[1],ABSOLUTE));
      }
      else
      {
        currentSolid->AddFacet(
          new G4TriangularFacet(corners[0], corners[1], corners[2], ABSOLUTE));
      }
    }
    else if (nCorners == 4)
    {
      if (fPermuteFacetPoints == true)
      {
        currentSolid->AddFacet(
          new G4QuadrangularFacet(corners[0], corners[3], corners[2], corners[1], ABSOLUTE));
      }
      else
      {
        currentSolid->AddFacet(
          new G4QuadrangularFacet(corners[0], corners[1], corners[2], corners[3], ABSOLUTE));
      }
    }
  }

  for (auto& mesh : meshCollection)
    mesh.tessellatedSolid->SetSolidClosed(true);

  if (GetVerbosityLevel() >= INFO)
    G4cout << "CADMesh: ... read " << meshCollection.size() << " meshes." << G4endl;

  return meshCollection;
}

//****************************************************************************************

G4LogicalVolume* cadmesh::NestMeshes(const std::vector<PolygonMesh>& meshCollection,
                                     const RegionAttributeMap& regionAttributeMap,
                                     const PolygonMeshHierarchyNode& root)
{
  G4LogicalVolume* polygonMeshLogical = nullptr;
  
  // unary predicate for std::find_if
  const G4int meshID = root.meshID;
  const G4String& name = root.name;
  auto MeshIsRoot = [meshID, name](const PolygonMesh& mesh)
  {
    return (mesh.meshID == meshID) || (mesh.name == name);
  };

  // find corresponding mesh in collection
  auto meshIter = std::find_if(meshCollection.begin(),
                               meshCollection.end(),
                               MeshIsRoot);
  
  // if found proceed
  if (meshIter != meshCollection.end())
  {    
    G4Material* material = G4NistManager::Instance()->FindMaterial("G4_AIR");
    G4Colour colour = G4Colour::White();
    
    // find region defined by the enclosed surface,
    // i.e. region marker <==> surface marker
    if (regionAttributeMap.find(meshID) != regionAttributeMap.end())
    {
      material = regionAttributeMap.at(meshID).material;
      colour = regionAttributeMap.at(meshID).colour;
    }
    else
    {
      G4ExceptionDescription description;
      description << "Unknown region '" << meshID << "'";
      description << ", setting interior material to 'G4_AIR'.";
      G4Exception("CADMesh", "INPUT_ERROR", JustWarning, description);
    }
    
    polygonMeshLogical = new G4LogicalVolume(meshIter->tessellatedSolid, material,
                                             meshIter->name + "_logical");
    polygonMeshLogical->SetVisAttributes(colour);

    // open node to look at children
    for (const PolygonMeshHierarchyNode& child : root.children)
    {
      // get sub-hierarchy recursively
      G4LogicalVolume* childLogical = NestMeshes(meshCollection, regionAttributeMap, child);

      // place, if child exists...
      if (childLogical)
      {
        G4String physVolName = "mesh" + std::to_string(child.meshID) + "_physical";

        // trivial placement, as that information is supposed to be encoded in the solid already
        new G4PVPlacement(nullptr, G4ThreeVector(), childLogical, physVolName,
                          polygonMeshLogical, false, 0 /* copy number */);
      }
    }
  }

  return polygonMeshLogical;
}
