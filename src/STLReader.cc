
// CADMesh //
#include "STLReader.hh"
#include "Exceptions.hh"


namespace CADMesh
{

namespace File
{


G4bool STLReader::Read(G4String filepath)
{
    Lexer lexer(filepath);
    lexer.Run(new StartSolidState);

    auto items = lexer.GetItems();

    if (items.size() == 0)
    {
        Exceptions::ParserError("STLReader::Read", "The STL file appears to be empty.");
    }

    for (auto item : lexer.GetItems())
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

        if (item.token == VerticesToken)
        {
            triangles.push_back(ParseVertices(item.children)); 
        } 
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
        if (item.token == ThreeVectorToken)
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
        if (item.token == NumberToken)
        {
            numbers.push_back((double) atof(item.value.c_str()));
        }
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



CADMeshLexerState(StartSolid)
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


CADMeshLexerState(EndSolid)
{
    SkipWhiteSpace();
    
    if (DoesNotMatchExactly("endsolid"))
        Error("STL files end with 'endsolid'.");

    Skip();
    EndOfA(Solid);

    FinalState();
}


CADMeshLexerState(StartFacet)
{
    SkipWhiteSpace();
    
    if (DoesNotMatchExactly("facet normal"))
        Error("Facets are indicated by the tag 'facet normal'.");
   
    SkipWhiteSpace();
    
    StartOfA(Facet);
    
    // The facet normal should be next.
    NextState(ThreeVector);
}


CADMeshLexerState(EndFacet)
{
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


CADMeshLexerState(StartVertices)
{
    SkipWhiteSpace();
    
    if (DoesNotMatchExactly("outer loop"))
        Error("The start of the vertices is indicated by the tag 'outer loop'.");

    SkipWhiteSpace();
    SkipLineBreak();

    StartOfA(Vertices);

    NextState(Vertex);
}


CADMeshLexerState(EndVertices)
{
    SkipWhiteSpace();
    
    if (DoesNotMatchExactly("endloop"))
        Error("The end of the vertices is indicated by the tag 'endloop'.");

    SkipWhiteSpace();
    SkipLineBreak();

    EndOfA(Vertices);

    NextState(EndFacet);
}


CADMeshLexerState(Vertex)
{
    SkipWhiteSpace();
    
    if (DoesNotMatchExactly("vertex"))
        Error("A vertex is indicated by the tag 'vertex'.");

    SkipWhiteSpace();
 
    NextState(ThreeVector);
}


CADMeshLexerState(ThreeVector)
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


} // File namespace

} // CADMesh namespace

