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
#include "PLYReader.hh"
#include "Exceptions.hh"


namespace CADMesh
{

namespace File
{

// Lexer states.
State* PLYReader::CADMeshLexerState(StartHeader)
{
    if (DoesNotMatchExactly("ply"))
        Error("PLY files start with 'ply'.");

    StartOfA(Header);

    SkipLine();

    TryState(Element);
    TryState(Ignore);
 
    Error("Invalid header tag.");
}


State* PLYReader::CADMeshLexerState(EndHeader)
{
    if (DoesNotMatchExactly("end_header"))
        Error("PLY file headers end with 'end_header'.");
    
    MaybeEndOfA(Element);

    EndOfA(Header);
    FinalState();
}


State* PLYReader::CADMeshLexerState(Element)
{
    if (DoesNotMatchExactly("element "))
        Error("An element is indicated by the tag 'element'.");

    MaybeEndOfA(Element);
    
    SkipWhiteSpace();
    StartOfA(Element);

    //  Expecting a label and a number/count.
    if (NotManyCharacters())
        Error("Element type not found.");

    ThisIsA(Word);
    SkipWhiteSpace();

    if (NotANumber())
        Error("Element count not found.");

    ThisIsA(Number);
    SkipLine();

    TryState(Property);
    TryState(Ignore);
  
    NextState(EndHeader);
}


State* PLYReader::CADMeshLexerState(Property)
{
   if (DoesNotMatchExactly("property "))
        Error("An property is indicated by the tag 'property'.");

    SkipWhiteSpace();
    StartOfA(Property);

    //  Expecting a label and a number/count.
    if (NotManyCharacters())
        Error("Element type not found.");

    ThisIsA(Word);
    SkipWhiteSpace();

    RestOfLine();
    ThisIsA(Word);

    EndOfA(Property);

    SkipLineBreak();

    TryState(Property);
    TryState(Element);
    TryState(EndHeader);
    TryState(Ignore);
  
    NextState(EndHeader);
}


State* PLYReader::CADMeshLexerState(Ignore)
{
    if (DidNotSkipLine())
        NextState(EndHeader);
    
    TryState(Element);
    TryState(Property);
    TryState(EndHeader);
    
    NextState(Ignore);
}


State* PLYReader::CADMeshLexerState(Vertex)
{
    SkipLineBreaks();
    SkipWhiteSpace();
    SkipLineBreaks();
    
    StartOfA(Vertex);

    size_t i = 0;

    // Try upto 32 properties per vertex/line.
    while (i < 32)
    {
        if (AtEndOfLine()) break;

        SkipWhiteSpace();

        if (NotANumber())
            Error("Expecting only numbers in the vertex specification.");

        ThisIsA(Number);
        SkipWhiteSpace();
    
        i++;
    }

    EndOfA(Vertex);

    SkipLine();   

    TryState(Vertex);

    FinalState();
}


State* PLYReader::CADMeshLexerState(Facet)
{
    SkipLineBreaks();
    SkipWhiteSpace();
    SkipLineBreaks();
    
    StartOfA(Facet);

    size_t i = 0;

    // Try upto 32 properties per facet/line.
    while (i < 32)
    {
        if (AtEndOfLine()) break;

        SkipWhiteSpace();

        if (NotANumber())
            Error("Expecting only numbers in the facet specification.");

        ThisIsA(Number);
        SkipWhiteSpace();
    
        i++;
    }

    EndOfA(Facet);
    
    SkipLine();

    TryState(Facet);

    FinalState();
}


// Parser.
G4bool PLYReader::Read(G4String filepath)
{
    // Run the lexer on the header.
    auto lexer = Lexer(filepath, new StartHeaderState);
    auto items = lexer.GetItems();

    // Start parsing tokens.
    if (items.size() == 0)
    {
        std::stringstream error;
        error << "The header appears to be empty.";

        Exceptions::ParserError("PLYReader::Read", error.str());
    }

    ParseHeader(items);
   
    // Run the lexer on the vertices.
    lexer.Run(new VertexState, vertex_count_);
    auto vertex_items = lexer.GetItems();

    if (vertex_items.size() == 0)
    {
        Exceptions::ParserError("PLYReader::Read", "The PLY file appears to have no vertices.");
    }

    if (vertex_items.size() != vertex_count_)
    {
        Exceptions::ParserError("PLYReader::Read", "The PLY file appears to be missing vertices.");
    }


    // Run the lexer on the facets.
    lexer.Run(new FacetState, facet_count_);
    auto face_items = lexer.GetItems();

    if (face_items.size() == 0)
    {
        Exceptions::ParserError("PLYReader::Read", "The PLY file appears to have no facets.");
    }

    if (face_items.size() != facet_count_)
    {
        Exceptions::ParserError("PLYReader::Read", "The PLY file appears to be missing facets");
    }

    auto mesh = ParseMesh(vertex_items, face_items);
    AddMesh(Mesh::New(mesh));
    
    return true;
}


G4bool PLYReader::CanRead(Type file_type)
{
    return (file_type == PLY);
}


void PLYReader::ParseHeader(Items items)
{
    if (items.size() != 1)
    {
        std::stringstream error;
        error << "The header appears to be invalid or missing."
                << "Error around line 1.";

        Exceptions::ParserError("PLYReader::ParseHeader", error.str());
    }

    for (auto item : items[0].children)
    { 
        if (item.token == ElementToken)
        {
            if (item.children.size() < 2)
            {
                std::stringstream error;
                error << "Invalid element information in header. Expecting 'vertex' or 'face' and a number."
                        << "Error around line "<< item.line  << ".";

                Exceptions::ParserError("PLYReader::ParseHeader", error.str());
            }
            
            if (item.children[0].token == WordToken && item.children[1].token == NumberToken)
            {
                if (item.children[0].value == "vertex")
                {
                    vertex_count_ = atoi(item.children[1].value.c_str());

                    // Find the x, y, z indices.
                    for (size_t i = 2; i < item.children.size(); i++)
                    {
                        auto property = item.children[i];

                        if (property.children.size() > 1)
                        {
                            if (property.children[1].token == WordToken)
                            {
                                if (property.children[1].value == "x")
                                {
                                    x_index_ = i - 2;
                                }

                                if (property.children[1].value == "y")
                                {
                                    y_index_ = i - 2;
                                }

                                if (property.children[1].value == "z")
                                {
                                    z_index_ = i - 2;
                                }
                            }
                        }
                    }
                } 

                else if (item.children[0].value == "face")
                {
                    facet_count_ = atoi(item.children[1].value.c_str());
 
                    // Find the facet indices.
                    for (size_t i = 2; i < item.children.size(); i++)
                    {
                        auto property = item.children[i];

                        if (property.children.size() > 1)
                        {
                            if (property.children[1].token == WordToken)
                            {
                                if (property.children[1].value == "uchar int vertex_indices")
                                {
                                    facet_index_ = i - 2;
                                }
                            }
                        }
                    }
 
                } 
            }
        }
    }

    if (vertex_count_ == 0)
    {
        std::stringstream error;
        error << "The number of vertices was not found in the header.";

        Exceptions::ParserError("PLYReader::ParseHeader", error.str());
    }

    if (facet_count_ == 0)
    {
        std::stringstream error;
        error << "The number of faces was not found in the header.";

        Exceptions::ParserError("PLYReader::ParseHeader", error.str());
    }

    if ((x_index_ == y_index_) || (y_index_ == z_index_) || (x_index_ == z_index_))
    {
        std::stringstream error;
        error << "The vertex x, y, z indices were not found in the header.";

        Exceptions::ParserError("PLYReader::ParseHeader", error.str());
    }
}   


std::shared_ptr<Mesh> PLYReader::ParseMesh(Items vertex_items, Items face_items)
{
    Points vertices;
    Triangles facets;

    // Parse vertices first.
    for (auto item : vertex_items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The vertex appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("PLYReader::ParseMesh", error.str());
        }

        if (item.token == VertexToken)
        {
            vertices.push_back(ParseVertex(item.children));
        }
    }

    for (auto item : face_items)
    {
        if (item.children.size() == 0)
        {
            std::stringstream error;
            error << "The facet appears to be empty."
                    << "Error around line " << item.line << ".";

            Exceptions::ParserError("PLYReader::Mesh", error.str());
        }

        if (item.token == FacetToken)
        {
            facets.push_back(ParseFacet(item.children, vertices));
        }
    }

    return Mesh::New(facets);
}   


G4ThreeVector PLYReader::ParseVertex(Items items)
{
    std::vector<double> numbers;

    for (auto item : items)
    {
        numbers.push_back((double) atof(item.value.c_str()));
    }

    if (numbers.size() < 3)
    {
        std::stringstream error;
        error << "Vertices in PLY files require atleast 3 numbers. ";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("PLYReader::ParseVertex", error.str());
    }

    return G4ThreeVector(numbers[x_index_], numbers[y_index_], numbers[z_index_]);
}


G4TriangularFacet* PLYReader::ParseFacet(Items items, Points vertices)
{
    std::vector<int> indices;

    for (auto item : items)
    {
        indices.push_back((int) atoi(item.value.c_str()));
    }

    if (indices.size() < 4) // "3" and 3 numbers
    {
        std::stringstream error;
        error << "Facets in PLY files require 3 indicies";

        if (items.size() != 0)
        {
            error << "Error around line " << items[0].line << ".";
        }

        Exceptions::ParserError("PLYReader::ParseFacet", error.str());
    }

    return new G4TriangularFacet( vertices[indices[1 + facet_index_]]
                                , vertices[indices[2 + facet_index_]]
                                , vertices[indices[3 + facet_index_]]
                                , ABSOLUTE);
}


} // File namespace

} // CADMesh namespace

