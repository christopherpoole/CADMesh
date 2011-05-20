/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE - basic example
 *
 * File:      cadmesh_example.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      20th March, 2011
 **************************************************/

// USER //
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"

// GEANT4 //
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisExecutive.hh"

int main(int argc,char** argv)
{
    G4RunManager* run_manager = new G4RunManager;

    G4VUserDetectorConstruction* detector_construction = new DetectorConstruction;
    run_manager->SetUserInitialization(detector_construction);

    G4VUserPhysicsList* physics_list = new PhysicsList;
    run_manager->SetUserInitialization(physics_list);

    G4VUserPrimaryGeneratorAction* primary_generator = new PrimaryGeneratorAction;
    run_manager->SetUserAction(primary_generator);

    run_manager->Initialize();
    
    G4UImanager * ui_manager = G4UImanager::GetUIpointer();
    G4VisManager* vis_manager = NULL;
    
    if (argc==1)
    {
        vis_manager = new G4VisExecutive;
        vis_manager->Initialize();
        
        G4UIsession * session = 0;
        session = new G4UIterminal(new G4UItcsh);
        ui_manager->ApplyCommand("/control/execute macros/vis.mac");
        session->SessionStart();
        
        delete session;
    }
    else
    {
        G4String command = "/control/execute ";
        G4String macro_file = argv[1];
        ui_manager->ApplyCommand(command + macro_file);
    }
    
    delete vis_manager;
    delete run_manager;

    return 0;
}


