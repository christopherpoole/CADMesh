#include <G4EmStandardPhysics.hh>

#include "PhysicsList.hh"


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
