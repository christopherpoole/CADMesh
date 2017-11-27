#ifndef PHYSICS_LIST_HH
#define PHYSICS_LIST_HH

#include <G4VModularPhysicsList.hh>


class PhysicsList: public G4VModularPhysicsList
{
  public:
    PhysicsList();
    ~PhysicsList();

  protected:
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();
};

#endif  // PHYSICS_LIST_HH
