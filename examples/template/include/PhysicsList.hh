/* ************************************************
 * GEANT4 CAD INTERFACE - template
 *
 * File:      PhysicsList.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      13th August, 2017
 **************************************************/

#pragma once

// GEANT4 //
#include "globals.hh"
#include "G4VModularPhysicsList.hh"


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

