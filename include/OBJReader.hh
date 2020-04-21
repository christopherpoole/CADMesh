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

// CADMesh//
#include "Reader.hh"
#include "Lexer.hh"
#include "LexerMacros.hh"

// GEANT4 //
#include "globals.hh"


namespace CADMesh
{

namespace File
{

class OBJReader : public Reader
{
  public:
    OBJReader() : Reader("OBJReader") { };
  
    G4bool Read(G4String filepath);
    G4bool CanRead(Type file_type);

  protected:
    //Lexer.
    CADMeshLexerStateDefinition(StartSolid);
    CADMeshLexerStateDefinition(EndSolid);

    CADMeshLexerStateDefinition(Ignore);
    CADMeshLexerStateDefinition(Vertex);
    CADMeshLexerStateDefinition(Facet);

    // Parser. 
    std::shared_ptr<Mesh> ParseMesh(Items items);
    G4ThreeVector ParseVertex(Items items);
    G4TriangularFacet* ParseFacet(Items items, Points vertices, G4bool quad);
};

} // File namespace

} // CADMesh namespace

