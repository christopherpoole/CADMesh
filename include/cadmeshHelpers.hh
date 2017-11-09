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

#include <fstream>
#include <utility>
#include <vector>

#include <G4Types.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>


namespace cadmesh {


// Get directory name from full path.
//
// Examples:
// "/path/to/file.txt" yields  "/path/to/"
// "/path/to/file"     yields  "/path/to/"
// "/path/to/"         yields  "/path/to/"
G4String PathDirName(const G4String& path);


// Split into directory and file name.
//
// Examples:
// "/path/to/file.txt" yields  ("/path/to/", "file.txt")
// "/path/to/file"     yields  ("/path/to/", "file")
// "/path/to/""        yields  ("/path/to/", "")
std::pair<G4String, G4String> PathSplit(const G4String& path);


// Split into root and file extension.
//
// Examples:
// "file.txt"      yields  ("file", ".txt")
// ".hidden_file"  yields  (".hidden_file", "")
// "file_no_ext"   yields  ("file_no_ext", "")
// "file.one.two"  yields  ("file.one", ".two")
std::pair<G4String, G4String> PathSplitExt(const G4String& fileName);
    

// Parse node section of SMESH or NODE file.
//
// Parses through the section in TetGen's SMESH or NODE files which defines a mesh's nodes.
// See: http://wias-berlin.de/software/tetgen/1.5/doc/manual/manual006.html. The input file
// stream is assumed to point at the beginning of this section.
std::vector<G4ThreeVector> ParseNodeSection(std::ifstream& nodeFileStream,
                                            G4double unitOfLength);


}  // namespace cadmesh
