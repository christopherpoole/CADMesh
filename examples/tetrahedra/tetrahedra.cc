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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
// !!! SET THIS BEFORE INCLUDING CADMESH.HH TO USE TETGEN !!! //
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
#define USE_CADMESH_TETGEN

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
        auto world_solid = new G4Box("world_solid", 1*m, 1*m, 1*m);

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

        ////////////
        // TETGEN //
        ////////////

        auto sphere_tets = CADMesh::TetrahedralMesh::FromPLY("./sphere.ply");
        sphere_tets->SetScale(500);
        sphere_tets->SetMaterial(water);

        auto sphere_assembly = sphere_tets->GetAssembly();

        auto position = G4ThreeVector();
        auto rotation = new G4RotationMatrix();

        sphere_assembly->MakeImprint(world_logical, position, rotation);

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

    auto primary_generator_action = new PrimaryGeneratorAction();
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
    session->ApplyCommand("/gps/pos/halfx 1 m");
    session->ApplyCommand("/gps/pos/halfy 1 m");
    session->ApplyCommand("/gps/pos/halfz 1 m");

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

