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

    TryState(Object);
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
    
    TryState(Object);
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
    TryState(Object);
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

    //  Expect three or four numbers separated by white space, we are ignoring
    //  the other parameters specified after the /'s.
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

    OneOf("/");
    Number();
    OneOf("/");
    Number();
    SkipWhiteSpace();

    // Might be a quad rather than a tri.
    if (Number())
        ThisIsA(Number);

    EndOfA(Facet);
    
    SkipLine();
 
    TryState(Facet);
    TryState(Vertex);
    TryState(Object);
    TryState(Ignore);
   
    NextState(EndSolid);
}


State* OBJReader::CADMeshLexerState(Object)
{
    SkipLineBreaks();

    if (DoesNotMatchExactly("o "))
        Error("An object is indicated by the tag 'o'.");

    EndOfA(Solid);
    
    SkipWhiteSpace();
    StartOfA(Solid);

    ManyCharacters();
    ThisIsA(Word); // The object name.

    SkipWhiteSpace();

    TryState(Vertex);
    TryState(Facet);
    TryState(Object);
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
            continue;
        }

        auto mesh = ParseMesh(item.children);

        // Only add meshes with faces.
        if (mesh->GetTriangles().size() == 0)
        {
            continue;
        }

        // We are expecting the first token to be the mesh/object name.
        G4String name;
        
        if (item.children[0].token == WordToken)
        {
            name = item.children[0].value;
        }

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
    Triangles facets;

    // Parse vertices first.
    for (auto item : items)
    {
        if (item.token != VertexToken)
        {
            continue;
        }

        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The vertex appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("OBJReader::Mesh", error.str());
        }

        vertices_.push_back(ParseVertex(item.children));
    }

    for (auto item : items)
    {
        if (item.token != FacetToken)
        {
            continue;
        }

        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The facet appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("OBJReader::Mesh", error.str());
        }

        facets.push_back(ParseFacet(item.children, false));

        // Add the upper triangle of the quad.
        if (item.children.size() == 4)
        {
            facets.push_back(ParseFacet(item.children, true));
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


G4TriangularFacet* OBJReader::ParseFacet(Items items, G4bool quad)
{
    std::vector<int> indices;

    for (auto item : items)
    {
        indices.push_back((int) atoi(item.value.c_str()));
    }

    if (indices.size() < 3)
    {
        std::stringstream error;
        error << "Facets in OBJ files require at least 3 indicies";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("OBJReader::ParseFacet", error.str());
    }

    if (quad && indices.size() != 4)
    {
        std::stringstream error;
        error << "Trying to triangle-ify a facet that isn't a quad";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("OBJReader::ParseFacet", error.str());
    }

    if (quad)
    {
        return new G4TriangularFacet( vertices_[indices[0] - 1]
                                    , vertices_[indices[2] - 1]
                                    , vertices_[indices[3] - 1]
                                    , ABSOLUTE);
    }

    else
    {
        return new G4TriangularFacet( vertices_[indices[0] - 1]
                                    , vertices_[indices[1] - 1]
                                    , vertices_[indices[2] - 1]
                                    , ABSOLUTE);
    }
}


} // File namespace

} // CADMesh namespace

