#define CATCH_CONFIG_MAIN 
#include "catch.hpp"

using namespace Catch;

// CADMesh //
#include "CADMesh.hh"

#include "Simulator.hh"


SCENARIO( "Load a PLY file as a tessellated mesh.") {

    GIVEN( "the sphere with a missing face in 'sphereWithMissingFace.ply'" ) {
        auto mesh = CADMesh::TessellatedMesh::FromPLY(
                                        "../models/sphereWithMissingFace.ply");

        WHEN( "constructing the solid volume" ) {
            auto solid = (G4TessellatedSolid*) mesh->GetSolid();

            THEN( "the number of facets should equal that in the file" ) {
                REQUIRE( solid->GetNumberOfFacets() == 994 );
            }

            THEN( "the inside point (0, 0, 0) reports as not inside" ) {
                REQUIRE_FALSE( solid->Inside(G4ThreeVector(0, 0, 0)) == kInside );
            }

            THEN( "the geometry should not be navigable by the Geant4 kernel" ) {
                REQUIRE_THROWS_WITH( Simulator(solid)
                                   , Contains("Track stuck or not moving") );
            }
        }
    }
}
