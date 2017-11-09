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
#include <array>
#include <vector>
#include <cmath>

#include <globals.hh>
#include <G4ios.hh>
#include <G4String.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4Types.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4RotationMatrix.hh>
#include <G4AssemblyVolume.hh>
#include <G4Tet.hh>

#include "cadmeshLog.hh"
#include "cadmeshHelpers.hh"
#include "cadmeshRegionAttributes.hh"

#include "cadmeshTetrahedralMesh.hh"


//****************************************************************************************
//    TetAssemblyVolume
//****************************************************************************************

cadmesh::TetAssemblyVolume::TetAssemblyVolume()
  : fTetAssembly(new G4AssemblyVolume), fRegionIDs(),
    fXmin(0.0), fXmax(0.0), fYmin(0.0), fYmax(0.0), fZmin(0.0), fZmax(0.0)
{
}


void
cadmesh::TetAssemblyVolume::AddTetrahedron(G4LogicalVolume* volume, G4int regionID)
{
  if (volume->GetSolid()->GetEntityType() != "G4Tet")
  {
    G4String errMsg = "Must not add non-tetrahedral volume to tetrahedral mesh.";
    G4Exception("CADMesh", "TYPE_ERROR", FatalException, errMsg);
    return;
  }

  // trivial placement, as we expect that information to be encoded in the solid.
  G4ThreeVector nullVector = G4ThreeVector();
  G4RotationMatrix* undefinedRotation = nullptr;
  fTetAssembly->AddPlacedVolume(volume, nullVector, undefinedRotation);

  // cache region ID of tetrahedron
  fRegionIDs.push_back(regionID);

  // update extent of tetrahedral mesh
  const G4VisExtent& tetExtent = volume->GetSolid()->GetExtent();
  if(GetNumberOfTetrahedra() > 0)
  {
    // update
    fXmin = std::min(fXmin, tetExtent.GetXmin());
    fXmax = std::max(fXmax, tetExtent.GetXmax());
    fYmin = std::min(fYmin, tetExtent.GetYmin());
    fYmax = std::max(fYmax, tetExtent.GetYmax());
    fZmin = std::min(fZmin, tetExtent.GetZmin());
    fZmax = std::max(fZmax, tetExtent.GetZmax()); 
  }
  else
  {
    // init
    fXmin = tetExtent.GetXmin();
    fXmax = tetExtent.GetXmax();
    fYmin = tetExtent.GetYmin();
    fYmax = tetExtent.GetYmax();
    fZmin = tetExtent.GetZmin();
    fZmax = tetExtent.GetZmax();
  }
}


//****************************************************************************************
//    TetMeshFileReader
//****************************************************************************************

cadmesh::TetrahedralMeshFileReader::TetrahedralMeshFileReader(G4double unitOfLength)
  : fUnitOfLength(unitOfLength)
{
}

//****************************************************************************************

std::vector<cadmesh::Tet>
cadmesh::TetrahedralMeshFileReader::ReadTetrahedralMesh(const G4String& filePath)
{
  std::vector<Tet> tetrahedra;

  const G4String& extension = PathSplitExt(filePath).second;
  if (extension == ".ele")
  {
    tetrahedra = ReadELE(filePath);
  }
  else
  {
    G4ExceptionDescription description;
    description << "File format not supported: '" << extension << "'.";
    description << "Could not load tetrahedral mesh.";
    G4Exception("CADMesh", "INVALID_READ", FatalException, description);
    return std::vector<Tet>();
  }

  return tetrahedra;
}

//****************************************************************************************

std::vector<cadmesh::Tet>
cadmesh::TetrahedralMeshFileReader::ReadELE(const G4String& filePath)
{
  // An ELE file is accompanied by a seperate NODE files that define all mesh nodes
  // E.g. for "<filePath>.ele" there should be "<filePath>.node"
  G4String nodeFilePath = PathSplitExt(filePath).first + ".node";
  
  if (GetVerbosityLevel() >= INFO)
    G4cout << "CADMesh: Reading nodes from '" << nodeFilePath << "'." << G4endl;
  
  // read mesh nodes from NODE file
  std::ifstream nodeFileStream(nodeFilePath);
  if (nodeFileStream.is_open() == false)
  {
    G4ExceptionDescription description;
    description << "Cannot open file: '" << nodeFilePath << "'.";
    G4Exception("CADMesh", "INVALID_READ", FatalException, description);
    return std::vector<Tet>();
  }
  std::vector<G4ThreeVector> nodes = ParseNodeSection(nodeFileStream, fUnitOfLength);

  if (GetVerbosityLevel() >= INFO)
    G4cout << "CADMesh: Reading tetrahedra from '" << filePath << "'." << G4endl;

  // After reading the nodes, the ELE file is opened
  std::ifstream eleFileStream(filePath);
  if (eleFileStream.is_open() == false)
  {
    G4ExceptionDescription description;
    description << "Cannot open file: '" << filePath << "'.";
    G4Exception("CADMesh", "INVALID_READ", FatalException, description);
    return std::vector<Tet>();
  }

  // The first non-comment line should be the header, containing:
  //
  //    <# of tetrahedra> <nodes per tet. (4 or 10)> <region attribute (0 or 1)>
  //
  std::size_t nTetrahedra = 0;
  std::size_t nNodesPerTet = 0;
  G4bool hasRegionAttribute = false;

  // interpret the first non-comment line as the section header
  G4String line;
  while (std::getline(eleFileStream, line))
  {
    // skip comments & emtpy lines
    if (line.front() == '#' || line.empty())
      continue;
    
    std::istringstream lineParser(line);

    // <# of tetrahedra> <nodes per tet. (4 or 10)> <region attribute (0 or 1)>
    if (lineParser >> nTetrahedra >> nNodesPerTet)
      lineParser >> hasRegionAttribute;

    if (lineParser.fail())
    {
      G4ExceptionDescription description;
      description << "Failed to parse ele section header: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<Tet>();
    }

    break;
  }

  // If generated with the '-o2' flag, TetGen's ELE file might define tetrahedra
  // from 10 points, i.e. the corners and each edge's midpoint. For now, abort
  // when encountering these files.
  if (nNodesPerTet != 4)
  {
    G4String errMsg = "Cannot read tetrahedral mesh generated with '-o2' flag.";
    G4Exception("CADMesh", "INPUT_ERROR", FatalException, errMsg);
    return std::vector<Tet>();
  }

  // strings we'll need to name the solid later on
  const G4String& fileName = PathSplit(filePath).second;
  const G4String& fileNameRoot = PathSplitExt(fileName).first;

  // After the header, each row of the ELE file defines one tetrahedron, 
  // via the indices of specific nodes: 
  //    ...
  //    <tetrahedron #> <node> <node> ... <node> [attribute]
  //    ...
  std::vector<Tet> tetrahedra;
  std::size_t counter = 0;
  while (std::getline(eleFileStream, line) && counter < nTetrahedra)
  {
    // skip comments & emtpy lines
    if (line.front() == '#' || line.empty())
      continue;

    std::istringstream lineParser(line);

    // <tetrahedron #>
    std::size_t tetNumber = 0;
    lineParser >> tetNumber;

    // <node> <node> ... <node>
    std::array<G4ThreeVector, 4> cornerNodes;
    for (auto& cornerNode : cornerNodes)
    {
      G4int index;
      lineParser >> index;
      cornerNode = nodes[index];
    }

    // [attribute] aka. regionID
    G4int regionID = DEFAULT_REGION_ID;
    if (hasRegionAttribute)
      lineParser >> regionID;

    // check if parsing any of the integers has failed
    if (lineParser.fail())
    {
      G4ExceptionDescription description;
      description << "Failed to read tetrahedron: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<Tet>();
    }

    G4String tetSolidName = fileNameRoot + "_tet" + std::to_string(counter);
    G4Tet* tetSolid = new G4Tet(tetSolidName, cornerNodes[0], cornerNodes[1],
                                              cornerNodes[2], cornerNodes[3]);

    tetrahedra.push_back(Tet{tetSolid, regionID});
    ++counter;
  }

  if (GetVerbosityLevel() >= DEBUG)
  {
    G4cout << "CADMesh: Obtained mesh containting ";
    G4cout << tetrahedra.size() << " tetrahedra." << G4endl;
  }

  return tetrahedra;
}

//****************************************************************************************

std::unique_ptr<cadmesh::TetAssemblyVolume>
cadmesh::TetrahedralMeshFileReader::ReadTetAssemblyVolume(const G4String& filePath,
                                                          const RegionAttributeMap& regionMap)
{
  std::vector<Tet> tetrahedra = ReadTetrahedralMesh(filePath);
  std::unique_ptr<TetAssemblyVolume> tetAssembly(new TetAssemblyVolume);

  if (GetVerbosityLevel() >= INFO)
    G4cout << "CADMesh: Assigning materials from region attributes..." << G4endl;

  if (GetVerbosityLevel() >= DEBUG)
    PrintTable(regionMap);
  
  for (const auto& tet : tetrahedra)
  {
    G4Material* material = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    G4Colour colour = G4Colour::White();
    
    // find region and set colour and material accordingly
    if (regionMap.find(tet.regionID) != regionMap.end())
    {
      material = regionMap.at(tet.regionID).material;
      colour = regionMap.at(tet.regionID).colour;
    }
    else
    {
      G4ExceptionDescription description;
      description << "Unknown region '" << tet.regionID << "'";
      description << ", setting material to 'G4_AIR'.";
      G4Exception("CADMesh", "INPUT_ERROR", JustWarning, description);
    }

    // create corresponding logical volume
    G4String logicalName = tet.solid->GetName() + "_logical"; 
    G4LogicalVolume* tetLogical = new G4LogicalVolume(tet.solid, material, logicalName);
    tetLogical->SetVisAttributes(colour);

    tetAssembly->AddTetrahedron(tetLogical, tet.regionID);
  }
  
  return tetAssembly;
}
