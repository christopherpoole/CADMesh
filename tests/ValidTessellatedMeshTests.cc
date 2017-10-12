#define CATCH_CONFIG_MAIN 
#include "catch.hpp"

// CADMesh //
#include "CADMesh.hh"

#include "Simulator.hh"

SCENARIO( "Load a PLY file as a tessellated mesh.") {

    GIVEN( "the sphere in the file 'sphere.ply'" ) {
        auto mesh = CADMesh::TessellatedMesh::FromPLY("../models/sphere.ply");

        WHEN( "constructing the solid volume" ) {
            auto solid = (G4TessellatedSolid*) mesh->GetSolid();

            THEN( "CADMesh should report that the volume is navigable" ) {
                REQUIRE( mesh->IsValidForNavigation() );
            }

            THEN( "the number of facets should equal that in the file" ) {
                REQUIRE( solid->GetNumberOfFacets() == 1280 );
            }

            THEN( "the solid is closed" ) {
                REQUIRE( solid->GetSolidClosed() );
            }

            THEN( "the point (0, 0, 0) is inside the volume" ) {
                REQUIRE( solid->Inside(G4ThreeVector(0, 0, 0)) == kInside );
            }

            THEN( "the point (1, 0, 0) is on the surface of the volume" ) {
                REQUIRE( solid->Inside(G4ThreeVector(1, 0, 0)) == kSurface );
            }

            THEN( "the point (2, 0, 0) is outside the volume" ) {
                REQUIRE( solid->Inside(G4ThreeVector(2, 0, 0)) == kOutside );
            }

            THEN( "the mimimum x extent should be at the sphere boundry" ) {
                REQUIRE( solid->GetMinXExtent() == -1.0 );
            }
 
            THEN( "the maximum x extent should be at the sphere boundry" ) {
                REQUIRE( solid->GetMaxXExtent() == 1.0 );
            }
        
            THEN( "the mimimum y extent should be at the sphere boundry" ) {
                REQUIRE( solid->GetMinYExtent() == -1.0 );
            }
 
            THEN( "the maximum y extent should be at the sphere boundry" ) {
                REQUIRE( solid->GetMaxYExtent() == 1.0 );
            }
  
            THEN( "the mimimum z extent should be at the sphere boundry" ) {
                REQUIRE( solid->GetMinZExtent() == -1.0 );
            }
 
            THEN( "the maximum z extent should be at the sphere boundry" ) {
                REQUIRE( solid->GetMaxZExtent() == 1.0 );
            }
            
            THEN( "the geometry should be navigable by the Geant4 kernel" ) {
                REQUIRE_NOTHROW( Simulator(solid) );
            }
        }
    }
}

