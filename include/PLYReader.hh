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

CADMeshLexerToken(Header);
CADMeshLexerToken(Element);
CADMeshLexerToken(Property);

class PLYReader : public Reader
{
  public:
    PLYReader() : Reader("PLYReader") { };
  
    G4bool Read(G4String filepath);
    G4bool CanRead(Type file_type);

  protected:
    //Lexer.
    CADMeshLexerStateDefinition(StartHeader);
    CADMeshLexerStateDefinition(EndHeader);

    CADMeshLexerStateDefinition(Element);
    CADMeshLexerStateDefinition(Property);
    CADMeshLexerStateDefinition(Ignore);
 
    CADMeshLexerStateDefinition(Vertex);
    CADMeshLexerStateDefinition(Facet);

    // Parser. 
    void ParseHeader(Items items);

    std::shared_ptr<Mesh> ParseMesh(Items vertex_items, Items face_items);
    G4ThreeVector ParseVertex(Items items);
    G4TriangularFacet* ParseFacet(Items items, Points vertices);

    size_t vertex_count_ = 0;
    size_t facet_count_ = 0;

    size_t x_index_ = 0;
    size_t y_index_ = 0;
    size_t z_index_ = 0;

    size_t max_index_ = 0;    
};

} // File namespace

} // CADMesh namespace

