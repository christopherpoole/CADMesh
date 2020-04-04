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

// CADMesh //
#include "OBJReader.hh"
#include "Exceptions.hh"


namespace CADMesh
{

namespace File
{

// Lexer states.
State* OBJReader::CADMeshLexerState(StartSolid)
{
    StartOfA(Solid);

    TryState(Vertex);
    TryState(Ignore);
 
    Error("Invalid element tag.");
}


State* OBJReader::CADMeshLexerState(EndSolid)
{
    if (Next() != "")
        lexer->LastError(); 

    EndOfA(Solid);
    FinalState();
}


State* OBJReader::CADMeshLexerState(Ignore)
{
    if (DidNotSkipLine())
        NextState(EndSolid);
    
    TryState(Vertex);
    TryState(Facet);
    TryState(Ignore);
  
    NextState(EndSolid);
}


State* OBJReader::CADMeshLexerState(Vertex)
{
    SkipLineBreaks();

    if (DoesNotMatchExactly("v "))
        Error("A vertex is indicated by the tag 'v'.");
    
    SkipWhiteSpace();
    StartOfA(Vertex);

    //  Expect three numbers separated by white space.
    if (NotANumber())
        Error("First number in three vector not found.");

    ThisIsA(Number);
    SkipWhiteSpace();

    if (NotANumber())
        Error("Second number in three vector not found.");
    
    ThisIsA(Number);
    SkipWhiteSpace();

    if (NotANumber())
        Error("Third number in three vector not found.");

    ThisIsA(Number);

    EndOfA(Vertex);

    SkipLine();   

    TryState(Vertex);
    TryState(Facet);
    TryState(Ignore);
 
    NextState(EndSolid);
}


State* OBJReader::CADMeshLexerState(Facet)
{
    SkipLineBreaks();

    if (DoesNotMatchExactly("f "))
        Error("A facet is indicated by the tag 'f'.");

    SkipWhiteSpace();
    StartOfA(Facet);

    //  Expect three numbers separated by white space, we are ignoring the
    //  other parameters specified after the /'s.
    if (NotANumber())
        Error("First number in three vector not found.");

    ThisIsA(Number);
   
    OneOf("/");
    Number();
    OneOf("/");
    Number();
    SkipWhiteSpace();

    if (NotANumber())
        Error("Second number in three vector not found.");
    
    ThisIsA(Number);

    OneOf("/");
    Number();
    OneOf("/");
    Number();
    SkipWhiteSpace();

    if (NotANumber())
        Error("Third number in three vector not found.");

    ThisIsA(Number);

    EndOfA(Facet);
    
    SkipLine();
 
    TryState(Facet);
    TryState(Ignore);
   
    NextState(EndSolid);
}


// Parser.
G4bool OBJReader::Read(G4String filepath)
{
    // Run the lexer.
    auto items = RunLexer(filepath, StartSolid);

    if (items.size() == 0)
    {
        Exceptions::ParserError("OBJReader::Read", "The OBJ file appears to be empty.");
    }

    // Start parsing tokens.
    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The mesh appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("OBJReader::Read", error.str());
        }

        auto mesh = ParseMesh(item.children);

        // Rename and add the mesh.
        auto name = G4String(item.value);
        AddMesh(Mesh::New(mesh, name));
    }

    return true;
}


G4bool OBJReader::CanRead(Type file_type)
{
    return (file_type == OBJ);
}


std::shared_ptr<Mesh> OBJReader::ParseMesh(Items items)
{
    Points vertices;
    Triangles facets;

    // Parse vertices first.
    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The vertex appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("OBJReader::Mesh", error.str());
        }

        if (item.token == VertexToken)
        {
            vertices.push_back(ParseVertex(item.children));
        }
    }

    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The facet appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("OBJReader::Mesh", error.str());
        }

        if (item.token == FacetToken)
        {
            facets.push_back(ParseFacet(item.children, vertices));
        }
    }

    return Mesh::New(facets);
}   


G4ThreeVector OBJReader::ParseVertex(Items items)
{
    std::vector<double> numbers;

    for (auto item : items)
    {
        numbers.push_back((double) atof(item.value.c_str()));
    }

    if (numbers.size() != 3)
    {
        std::stringstream error;
        error << "Three vectors in OBJ files require exactly 3 numbers";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("OBJReader::ParseThreeVector", error.str());
    }

    return G4ThreeVector(numbers[0], numbers[1], numbers[2]);
}


G4TriangularFacet* OBJReader::ParseFacet(Items items, Points vertices)
{
    std::vector<int> indices;

    for (auto item : items)
    {
        indices.push_back((int) atoi(item.value.c_str()));
    }

    if (indices.size() != 3)
    {
        std::stringstream error;
        error << "Facets in OBJ files require exactly 3 indicies";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("OBJReader::ParseFacet", error.str());
    }

    return new G4TriangularFacet( vertices[indices[0] - 1]
                                , vertices[indices[1] - 1]
                                , vertices[indices[2] - 1]
                                , ABSOLUTE);
}


} // File namespace

} // CADMesh namespace

