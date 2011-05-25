/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE - basic example
 *
 * File:      PhysicsList.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      20th March, 2011
 **************************************************/

#ifndef PhysicsList_h
#define PhysicsList_h 1

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

#endif

