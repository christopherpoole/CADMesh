
// GEANT4 //
#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>

#include <G4VUserDetectorConstruction.hh>
#include <G4NistManager.hh>
#include <G4TessellatedSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>

#include <G4VModularPhysicsList.hh>
#include <G4Geantino.hh>

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4GeneralParticleSource.hh>

#include <G4ExceptionHandler.hh>


class DetectorConstruction : public G4VUserDetectorConstruction
{
  private:
      G4TessellatedSolid* solid_;

  public:
    DetectorConstruction(G4TessellatedSolid* solid) : solid_(solid)
    {
    };

    G4VPhysicalVolume* Construct()
    {
        G4NistManager * nist_manager = G4NistManager::Instance();
        G4Material * air = nist_manager->FindOrBuildMaterial("G4_AIR");
        G4Material * water = nist_manager->FindOrBuildMaterial("G4_WATER");

        double x = solid_->GetMaxXExtent() - solid_->GetMinXExtent();
        double y = solid_->GetMaxYExtent() - solid_->GetMinYExtent();
        double z = solid_->GetMaxZExtent() - solid_->GetMinZExtent();

        // Set world half lengths to the extent of the test solid.
        auto world_solid = new G4Box("world_solid", x, y, z);

        auto world_logical = new G4LogicalVolume( world_solid
                                                , air
                                                , "world_logical"
                                                , 0, 0, 0
        );

        auto world_physical = new G4PVPlacement( 0
                                               , G4ThreeVector()
                                               , world_logical
                                               , "world_physical"
                                               , 0, false, 0
        );

        auto logical = new G4LogicalVolume( solid_
                                          , water
                                          , "logical"
                                          , 0, 0, 0
        );

        auto physical = new G4PVPlacement( 0
                                         , G4ThreeVector()
                                         , logical
                                         , "physical"
                                         , world_logical
                                         , false, 0
        );
 
        return world_physical;
    };
};


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  private:
    G4GeneralParticleSource* particle_gun_;

  public:
    PrimaryGeneratorAction()
    {
        particle_gun_ = new G4GeneralParticleSource();
        particle_gun_->SetParticleDefinition(G4Geantino::Definition());
    };

    ~PrimaryGeneratorAction()
    {
        delete particle_gun_;
    };

    void GeneratePrimaries(G4Event* event)
    {
        particle_gun_->GeneratePrimaryVertex(event);
    };
};


class ExceptionHandler : public G4VExceptionHandler
{
  public:
    ExceptionHandler() : G4VExceptionHandler()
    {
    };
  
    G4bool Notify( const char* /*originOfException*/
                 , const char* /*exceptionCode*/
                 , G4ExceptionSeverity /*severity*/
                 , const char* description)
    {
        // We are going to throw here so we can catch in the test.
        throw std::runtime_error(description);

        // Abort on any exception. Never reached.
        return true;
    };
};



class Simulator
{
  public:
    Simulator(G4TessellatedSolid* solid, bool vis = false, bool cli = false)
    {
        auto run_manager = new G4RunManager();

        auto exception_hanlder = new ExceptionHandler();

        auto detector_construction = new DetectorConstruction(solid);
        run_manager->SetUserInitialization(detector_construction);

        auto physics_list = new G4VModularPhysicsList();
        run_manager->SetUserInitialization(physics_list);

        auto primary_generator_action = new PrimaryGeneratorAction();
        run_manager->SetUserAction(primary_generator_action);

        run_manager->Initialize();

        if (vis)
        {
            auto vis_manager = new G4VisExecutive();
            vis_manager->Initialize();
        }

        auto session = G4UImanager::GetUIpointer();

        // The source must be outside of the test volume to guarantee tracking
        // and navigation errors occour.
        session->ApplyCommand("/gps/pos/type Volume");
        session->ApplyCommand("/gps/pos/shape Para");
        session->ApplyCommand("/gps/pos/confine world_physical");
        session->ApplyCommand("/gps/ang/type iso");
 
        double x = solid->GetMaxXExtent() - solid->GetMinXExtent();
        std::stringstream command_x;
        command_x << "/gps/pos/halfx " << x << " mm"; 
        session->ApplyCommand(command_x.str());
 
        double y = solid->GetMaxYExtent() - solid->GetMinYExtent();
        std::stringstream command_y;
        command_y << "/gps/pos/halfy " << y << " mm"; 
        session->ApplyCommand(command_y.str());

        double z = solid->GetMaxZExtent() - solid->GetMinZExtent();
        std::stringstream command_z;
        command_z << "/gps/pos/halfz " << z << " mm"; 
        session->ApplyCommand(command_z.str());

        if (vis)
        { 
            session->ApplyCommand("/vis/open OGL 500x500-500-50");
            session->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 60 30");
            session->ApplyCommand("/vis/drawVolume");
            session->ApplyCommand("/vis/scene/add/trajectories");
            session->ApplyCommand("/vis/scene/endOfEventAction accumulate");
        }

        G4UIExecutive* ui = nullptr;

        if (cli)
        {
            int argc = 1;
            char* argv[1] = {(char*) ""};
            ui = new G4UIExecutive(argc, argv);
            ui->SessionStart();
        }

        run_manager->BeamOn(1e5);

        delete session;
        delete ui;
    };
};
