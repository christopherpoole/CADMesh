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
//#include "G4UIterminal.hh"
//#include "G4UItcsh.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc, char** argv)
{
    G4RunManager* run_manager = new G4RunManager;

    DetectorConstruction* detector_construction = new DetectorConstruction;

    if (argc == 3) {
        detector_construction->SetCADFilename(argv[2]);
    } else if (argc == 4) {
        // We must specify the file type for tetrahedral meshes.
        detector_construction->SetCADFilename(argv[2]);
        detector_construction->SetCADFiletype(argv[3]);
    } else {
        G4cout << "Usage:" << G4endl;
        G4cout << "    cadmesh_example <macro> <cad file name> <cad file type, optional>" << G4endl;
        return 0;
    }

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


