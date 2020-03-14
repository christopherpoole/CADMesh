// GEANT4 //
#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4TessellatedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

#include "G4VModularPhysicsList.hh"
#include "G4Gamma.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4SystemOfUnits.hh"

// CADMESH //
#include "CADMesh.hh"


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    G4VPhysicalVolume* Construct()
    {
        // Materials //
        G4NistManager * nist_manager = G4NistManager::Instance();
        G4Material * air = nist_manager->FindOrBuildMaterial("G4_AIR");
        G4Material * water = nist_manager->FindOrBuildMaterial("G4_WATER");

        // World //
        auto world_solid = new G4Box("world_solid", 200*mm, 200*mm, 200*mm);

        auto world_logical = new G4LogicalVolume( world_solid
                                                , air
                                                , "world_logical"
                                                , 0, 0, 0
        );

        auto world_physical = new G4PVPlacement( 0
                                               , G4ThreeVector()
                                               , world_logical
                                               , "world_physical"
                                               , 0, false, 0
        );

        /////////////
        // CADMesh //
        /////////////

        // Read your file. STL in this example. PLY and OBJ can also be loaded 
        // using the built-in reader (no external software dependencies). Look
        // at the other examples for using external readers.
        auto mesh = CADMesh::TessellatedMesh::FromSTL("./cone.stl");

        // Optionally set the mesh scale and offset. These values are applied
        // directly to the mesh vertices before generating the solid. The scale
        // is applied before the offset.
        mesh->SetScale(mm);
        mesh->SetOffset(G4ThreeVector(50, -100, 100));

        // Get the G4VSolid*. Use this like you would any other solid in Geant4.
        auto solid = mesh->GetSolid();

        auto logical = new G4LogicalVolume( solid
                                          , water
                                          , "logical"
                                          , 0, 0, 0
        );

        auto physical = new G4PVPlacement( 0
                                         , G4ThreeVector()
                                         , logical
                                         , "physical"
                                         , world_logical
                                         , false, 0
        );
 
        return world_physical;
    };
};


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  private:
    G4GeneralParticleSource* particle_gun;

  public:
    PrimaryGeneratorAction()
    {
        particle_gun = new G4GeneralParticleSource();
        particle_gun->SetParticleDefinition(G4Gamma::Definition());
    };

    ~PrimaryGeneratorAction()
    {
        delete particle_gun;
    };

    void GeneratePrimaries(G4Event* event)
    {
        particle_gun->GeneratePrimaryVertex(event);
    };
};


int main(int argc, char** argv)
{
    auto run_manager = new G4RunManager();

    auto detector_construction = new DetectorConstruction();
    run_manager->SetUserInitialization(detector_construction);

    auto physics_list = new G4VModularPhysicsList();
    run_manager->SetUserInitialization(physics_list);

    auto primary_generator_action = new G4VPrimaryGeneratorAction();
    run_manager->SetUserAction(primary_generator_action);

    run_manager->Initialize();

    auto vis_manager = new G4VisExecutive();
    vis_manager->Initialize();

    auto session = G4UImanager::GetUIpointer();

    // Source //
    session->ApplyCommand("/gps/pos/type Volume");
    session->ApplyCommand("/gps/pos/shape Para");
    session->ApplyCommand("/gps/pos/confine world_physical");
    session->ApplyCommand("/gps/ang/type iso");
    session->ApplyCommand("/gps/pos/halfx 100 mm");
    session->ApplyCommand("/gps/pos/halfy 100 mm");
    session->ApplyCommand("/gps/pos/halfz 100 mm");

    // Visualisation //
    session->ApplyCommand("/vis/open OGL 500x500-500-50");
    session->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 60 30");
    session->ApplyCommand("/vis/drawVolume");
    session->ApplyCommand("/vis/scene/add/trajectories");
    session->ApplyCommand("/vis/scene/endOfEventAction accumulate");

    // Run //
    auto ui = new G4UIExecutive(argc, argv);
    run_manager->BeamOn(1e2);
    ui->SessionStart();

    delete session;
    delete ui;
}


