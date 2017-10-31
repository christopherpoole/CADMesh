
#pragma once

// GEANT4 //
#include "G4ThreeVector.hh"
#include "G4TriangularFacet.hh"

// STL //
#include <vector>
#include <memory>


namespace CADMesh
{

typedef std::vector<G4ThreeVector> Points;
typedef std::vector<G4TriangularFacet*> Triangles;


class Mesh
{
  public:
    Mesh(Points points, Triangles triangles, G4String name = "");

    static std::shared_ptr<Mesh> New( Points points
                                    , Triangles triangles
                                    , G4String name = "");

    static std::shared_ptr<Mesh> New( Triangles triangles
                                    , G4String name = "");

  public:
    G4String GetName();
    Points GetPoints();
    Triangles GetTriangles();

    G4bool IsValidForNavigation();

  private:
    G4String name_ = "";

    Points points_;
    Triangles triangles_;
};

typedef std::vector<std::shared_ptr<Mesh> > Meshes;

} // CADMesh namespace

