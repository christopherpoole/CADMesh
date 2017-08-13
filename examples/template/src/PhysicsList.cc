/* ************************************************
 * GEANT4 CAD INTERFACE - template
 *
 * File:      PhysicsList.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      13th August, 2017
 **************************************************/

// USER//
#include "PhysicsList.hh"

// GEANT4 //
#include "G4EmStandardPhysics.hh"


PhysicsList::PhysicsList()
{
    RegisterPhysics(new G4EmStandardPhysics());
}

PhysicsList::~PhysicsList()
{
}

void PhysicsList::ConstructParticle()
{
    G4VModularPhysicsList::ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
    G4VModularPhysicsList::ConstructProcess();
}


void PhysicsList::SetCuts()
{
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
}

