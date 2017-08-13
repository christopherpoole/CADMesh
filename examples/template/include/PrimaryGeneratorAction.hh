/* ************************************************
 * GEANT4 CAD INTERFACE - template
 *
 * File:      PrimaryGeneratorAction.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      13th August, 2017
 **************************************************/

#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

class G4GeneralParticleSource;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        PrimaryGeneratorAction();
        ~PrimaryGeneratorAction();

    public:
        void GeneratePrimaries(G4Event* event);

    private:
        G4GeneralParticleSource* particle_gun;
};

