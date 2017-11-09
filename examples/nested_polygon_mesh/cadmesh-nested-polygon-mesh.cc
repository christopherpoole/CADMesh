#include <G4RunManager.hh>
#include <G4ios.hh>
#include <G4VisExecutive.hh>
#include <G4UImanager.hh>
#include <G4UIExecutive.hh>

#include <cstdlib>

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"


int main(int argc, char** argv)
{
  G4RunManager* runManager = new G4RunManager;
  DetectorConstruction* detectorConstruction = new DetectorConstruction;

  if (argc != 2)
  {
      G4cout << "Usage:" << G4endl;
      G4cout << "    cadmesh-polygon-mesh <polygon-mesh-file>" << G4endl;
      return EXIT_FAILURE;
  }
  else
  {
    detectorConstruction->SetCADFilename(argv[1]);
  }

  runManager->SetUserInitialization(detectorConstruction);

  PhysicsList* physicsList = new PhysicsList;
  runManager->SetUserInitialization(physicsList);

  PrimaryGeneratorAction* primaryGenerator = new PrimaryGeneratorAction;
  runManager->SetUserAction(primaryGenerator);

  runManager->Initialize();

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);

  uiManager->ApplyCommand("/control/execute visual_run.mac");
  ui->SessionStart();
  delete ui;

  delete visManager;
  delete runManager;
  return EXIT_SUCCESS;
}
