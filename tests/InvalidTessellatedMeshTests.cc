#define CATCH_CONFIG_MAIN 
#include "catch.hpp"

using namespace Catch;

// CADMesh //
#include "CADMesh.hh"

SCENARIO( "Load an invalid PLY file as a tessellated mesh.") {

    GIVEN( "the sphere with a missing face in 'sphereWithMissingFace.ply'" ) {
        WHEN( "reading the file" ) {
            THEN( "the file should not open" ) {
                REQUIRE_THROWS( 
                    CADMesh::TessellatedMesh::FromPLY("../meshes/sphereWithMissingFace.ply")
                  );
            }
        }
    }
}
