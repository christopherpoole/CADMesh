#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* event);

  private:
    G4GeneralParticleSource* fParticleGun;
};

#endif  // PRIMARY_GENERATOR_ACTION_HH
