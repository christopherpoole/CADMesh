/********************************************************************************
* CADMesh - A CAD file interface for Geant4                                     *
*                                                                               *
* Copyright (c) 2017 Simon Kirchhof <s.kirchhof@dkfz-heidelberg.de>             *
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
#include <utility>
#include <vector>

#include <G4Types.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>

#include "cadmeshHelpers.hh"


G4String cadmesh::PathDirName(const G4String& path)
{ 
  G4String dir = "";

  std::size_t found = path.find_last_of("/\\");
  if (found < std::string::npos)
  {
    dir = path.substr(0, found + 1);
  }

  return dir;
}


std::pair<G4String, G4String> cadmesh::PathSplit(const G4String& path)
{
  G4String dir = "";
  G4String fileName = "";

  std::size_t found = path.find_last_of("/\\");
  if (found < std::string::npos)
  {
    dir = path.substr(0, found + 1);
    fileName = path.substr(found + 1, std::string::npos);
  } 
  else if (found == std::string::npos)
  {
    fileName = path;
  }

  return std::pair<G4String, G4String>(dir, fileName);
}


std::pair<G4String, G4String> cadmesh::PathSplitExt(const G4String& path)
{
  G4String root = "";
  G4String ext = "";

  std::pair<G4String, G4String> dirAndFileName = PathSplit(path);
  const G4String& dir = dirAndFileName.first;
  const G4String& fileName = dirAndFileName.second;

  std::size_t found = fileName.find_last_of(".");
  if (found > 0 && found < std::string::npos)
  {
    root = dir + fileName.substr(0, found);
    ext = fileName.substr(found, std::string::npos);
  }
  else
  {
    root = path;
  }

  return std::pair<G4String, G4String>(root, ext);
}


std::vector<G4ThreeVector>
cadmesh::ParseNodeSection(std::ifstream& nodeFileStream, G4double unitOfLength)
{
  // header of node section:
  //    <# of points> <dimension (3)> <# of attributes> <boundary markers (0 or 1)>
  std::size_t nNodes = 0;
  std::size_t dimension = 0;

  G4String line;
  while (std::getline(nodeFileStream, line))
  {
    // skip comments & emtpy lines
    if (line.front() == '#' || line.empty())
      continue;

    std::istringstream lineParser(line);

    // read first entry: <# of points>
    lineParser >> nNodes;
    if (lineParser.fail())
    {
      G4ExceptionDescription description;
      description << "Failed to parse node section header: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<G4ThreeVector>();
    }

    if (nNodes > 0)
    {
      // read second entry: <dimension (3)>
      lineParser >> dimension;
      if (lineParser.fail())
      {
        G4ExceptionDescription description;
        description << "Failed to parse node section header: '" << line << "'.";
        G4Exception("CADMesh", "INVALID_READ", FatalException, description);
        return std::vector<G4ThreeVector>();
      }

      // possible logic error
      if (dimension != 3) 
      {
        G4String errMsg = "Error: Node coordinates are not 3-dimensional.";
        G4Exception("CADMesh", "LOGIC_ERROR", FatalException, errMsg);
        return std::vector<G4ThreeVector>();
      }
    }
    else
    {
      // no nodes, return empty vector
      return std::vector<G4ThreeVector>();
    }
    
    // ignore the rest, i.e. <# of attributes> <boundary markers (0 or 1)>
    break;
  }

  //  Remaining lines list points: 
  //    <point #> <x> <y> <z> [attributes] [boundary marker]
  std::vector<G4ThreeVector> nodes;
  while (std::getline(nodeFileStream, line) && nodes.size() < nNodes)
  {
    // skip comments & emtpy lines
    if (line.front() == '#' || line.empty())
      continue;

    std::istringstream lineParser(line);

    std::size_t nodeCount;
    G4double x, y, z;

    // read node: <point #> <x> <y> <z>
    if (lineParser >> nodeCount)
      lineParser >> x >> y >> z;
    
    if (lineParser.fail())
    {
      G4ExceptionDescription description;
      description << "Failed to read node: '" << line << "'.";
      G4Exception("CADMesh", "INVALID_READ", FatalException, description);
      return std::vector<G4ThreeVector>();
    }

    nodes.push_back(G4ThreeVector(x, y, z) * unitOfLength);
  }

  return nodes;
}
