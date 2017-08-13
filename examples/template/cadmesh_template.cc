/* ************************************************
 * GEANT4 CAD INTERFACE - template
 *
 * File:      cadmesh_example.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      13th August, 2017
 **************************************************/

// USER //
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"

// GEANT4 //
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


int main(int argc, char** argv)
{
    G4RunManager* run_manager = new G4RunManager;

    DetectorConstruction* detector_construction = new DetectorConstruction;
    run_manager->SetUserInitialization(detector_construction);

    PhysicsList* physics_list = new PhysicsList;
    run_manager->SetUserInitialization(physics_list);

    PrimaryGeneratorAction* primary_generator = new PrimaryGeneratorAction;
    run_manager->SetUserAction(primary_generator);

    run_manager->Initialize();

    G4VisManager* vis_manager = new G4VisExecutive;
    vis_manager->Initialize();

    G4UImanager * ui_manager = G4UImanager::GetUIpointer();
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    ui_manager->ApplyCommand(G4String("/control/execute ") + G4String(argv[1])); 
    ui->SessionStart();
    delete ui; 
    
    return 0;
}


