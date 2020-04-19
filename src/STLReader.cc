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
#include "STLReader.hh"
#include "Exceptions.hh"
#include "LexerMacros.hh"

namespace CADMesh
{

namespace File
{


// Lexer states.
State* STLReader::CADMeshLexerState(StartSolid)
{
    if (DoesNotMatchExactly("solid"))
        Error("STL files start with 'solid'.");

    SkipWhiteSpace();
    
    // The name of the solid.
    ManyLetters();
    
    StartOfA(Solid);
    
    SkipLineBreak();
    NextState(StartFacet);
}


State* STLReader::CADMeshLexerState(EndSolid)
{
    SkipWhiteSpace();
    SkipLineBreaks();
    SkipWhiteSpace();
   
    if (DoesNotMatchExactly("endsolid"))
        Error("STL files end with 'endsolid'.");

    Skip();
    EndOfA(Solid);

    FinalState();
}


State* STLReader::CADMeshLexerState(StartFacet)
{
    SkipWhiteSpace();
    SkipLineBreaks();
    SkipWhiteSpace();

    if (DoesNotMatchExactly("facet normal"))
        Error("Facets are indicated by the tag 'facet normal'.");
   
    SkipWhiteSpace();
    
    StartOfA(Facet);

    // Skip the normal...
    SkipLine();

    // The facet normal should be next.
    NextState(StartVertices);
}


State* STLReader::CADMeshLexerState(EndFacet)
{
    SkipWhiteSpace();
    SkipLineBreaks();
    SkipWhiteSpace();
   
    if (DoesNotMatchExactly("endfacet"))
        Error("The end of a facets is indicated by the tag 'endfacet'.");
   
    SkipWhiteSpace();
    SkipLineBreak();
    
    EndOfA(Facet);

    // Another facet, could be next.
    TryState(StartFacet);
    
    // Otherwise, we must be at the end of the solid.
    NextState(EndSolid);
}


State* STLReader::CADMeshLexerState(StartVertices)
{
    SkipWhiteSpace();
    SkipLineBreaks();
    SkipWhiteSpace();
   
    if (DoesNotMatchExactly("outer loop"))
        Error("The start of the vertices is indicated by the tag 'outer loop'.");

    SkipWhiteSpace();
    SkipLineBreak();

    StartOfA(Vertices);

    NextState(Vertex);
}


State* STLReader::CADMeshLexerState(EndVertices)
{
    SkipWhiteSpace();
    SkipLineBreaks();
    SkipWhiteSpace();
    
    if (DoesNotMatchExactly("endloop"))
        Error("The end of the vertices is indicated by the tag 'endloop'.");

    SkipWhiteSpace();
    SkipLineBreak();

    EndOfA(Vertices);

    NextState(EndFacet);
}


State* STLReader::CADMeshLexerState(Vertex)
{
    SkipWhiteSpace();
    SkipLineBreaks();
    SkipWhiteSpace();

    if (DoesNotMatchExactly("vertex"))
        Error("A vertex is indicated by the tag 'vertex'.");

    SkipWhiteSpace();
 
    NextState(ThreeVector);
}


State* STLReader::CADMeshLexerState(ThreeVector)
{
    SkipWhiteSpace();

    StartOfA(ThreeVector);
    
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
    EndOfA(ThreeVector);

    SkipWhiteSpace();

    if (DidNotSkipLineBreak())
        Error("Expecting a new line at the end of a three vector.");

    // After a three vector we can expect the start of a list of vertices.
    TryState(StartVertices);

    // Or we can expect another vertex.
    TryState(Vertex);

    // Or we must be at the end of a list of vertices.
    NextState(EndVertices);
}

// Parser.
G4bool STLReader::Read(G4String filepath)
{
    // Run the lexer.
    auto items = RunLexer(filepath, StartSolid);

    if (items.size() == 0)
    {
        Exceptions::ParserError("STLReader::Read", "The STL file appears to be empty.");
    }

    // Start parsing tokens.
    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The mesh appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("STLReader::Read", error.str());
        }

        auto mesh = ParseMesh(item.children);

        // Rename and add the mesh.
        auto name = G4String(item.value);
        AddMesh(Mesh::New(mesh, name));
    }

    return true;
}


G4bool STLReader::CanRead(Type file_type)
{
    return (file_type == STL);
}


std::shared_ptr<Mesh> STLReader::ParseMesh(Items items)
{
    Triangles triangles;

    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The facet appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("STLReader::Mesh", error.str());
        }

        triangles.push_back(ParseFacet(item.children));
    }

    return Mesh::New(triangles);
}   


G4TriangularFacet* STLReader::ParseFacet(Items items)
{
    Triangles triangles;

    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The vertex appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("STLReader::ParseFacet", error.str());
        }

        triangles.push_back(ParseVertices(item.children)); 
    }

    if (triangles.size() != 1)
    {
        std::stringstream error;
        error << "STL files expect exactly 1 triangle per facet.";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("STLReader::ParseFacet", error.str());
    }

    return triangles[0];
}


G4TriangularFacet* STLReader::ParseVertices(Items items)
{
    std::vector<G4ThreeVector> vertices; 

    for (auto item : items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The vertex appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("STLReader::ParseVertices", error.str());
        }
    
        vertices.push_back(ParseThreeVector(item.children)); 
    }

    if (vertices.size() != 3)
    {
        std::stringstream error;
        error << "STL files expect exactly 3 vertices for a triangular facet. ";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("STLReader::ParseVertices", error.str());
    }

    return new G4TriangularFacet( vertices[0]
                                , vertices[1]
                                , vertices[2]
                                , ABSOLUTE);
}


G4ThreeVector STLReader::ParseThreeVector(Items items)
{
    std::vector<double> numbers;

    for (auto item : items)
    {
        numbers.push_back((double) atof(item.value.c_str()));
    }

    if (numbers.size() != 3)
    {
        std::stringstream error;
        error << "Three vectors in STL files require exactly 3 numbers";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("STLReader::ParseThreeVector", error.str());
    }

    return G4ThreeVector(numbers[0], numbers[1], numbers[2]);
}


} // File namespace

} // CADMesh namespace

