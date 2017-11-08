
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

class STLReader : public Reader
{
  public:
    STLReader() : Reader("STLReader") { };
  
    G4bool Read(G4String filepath);
    G4bool CanRead(Type file_type);

  protected:
    std::shared_ptr<Mesh> ParseMesh(Items items);
    G4TriangularFacet* ParseFacet(Items items);
    G4TriangularFacet* ParseVertices(Items items);
    G4ThreeVector ParseThreeVector(Items items);
};

// Lexer tokens.
CADMeshLexerToken(Solid);
CADMeshLexerToken(Facet);
CADMeshLexerToken(Vertices);
CADMeshLexerToken(ThreeVector);
CADMeshLexerToken(Number);

// Lexer state transitions.
CADMeshLexerStateDefinition(StartSolid);
CADMeshLexerStateDefinition(EndSolid);
CADMeshLexerStateDefinition(StartFacet);
CADMeshLexerStateDefinition(EndFacet);
CADMeshLexerStateDefinition(StartVertices);
CADMeshLexerStateDefinition(EndVertices);
CADMeshLexerStateDefinition(Vertex);
CADMeshLexerStateDefinition(ThreeVector);


} // File namespace

} // CADMesh namespace

