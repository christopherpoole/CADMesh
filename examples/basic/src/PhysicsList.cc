/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE - basic example
 *
 * File:      PhysicsList.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      20th March, 2011
 **************************************************/

// USER//
#include "PhysicsList.hh"

// GEANT4 //
#include "G4ParticleTypes.hh"


PhysicsList::PhysicsList()
{;}

PhysicsList::~PhysicsList()
{;}

void PhysicsList::ConstructParticle()
{
    G4Geantino::GeantinoDefinition();
}

void PhysicsList::ConstructProcess()
{
    AddTransportation();
}

void PhysicsList::SetCuts()
{
 
  G4int temp = GetVerboseLevel();                                                
  SetVerboseLevel(0);                                                           
  SetCutsWithDefault();   
  SetVerboseLevel(temp);  
}

