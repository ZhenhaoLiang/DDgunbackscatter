#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include <sys/time.h>
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }
  struct timeval hTimeValue;
  gettimeofday(&hTimeValue, NULL);
  G4int m_hRanSeed = hTimeValue.tv_usec;
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  G4cout << "Initialize random numbers with seed = "
         << m_hRanSeed << G4endl;
  CLHEP::HepRandom::setTheSeed(m_hRanSeed);
  auto actioninitial = new ActionInitialization();
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if ( (! ui) &&  (argc > 2) )
  {
    //argv[2] is out file name
    std::string outfile = argv[2];
    outfile = outfile + ".root";
    actioninitial->SetDataFilenamemy(outfile);
  }
  //actioninitial->SetDataFilenamemy("out.root");
  G4RunManager* runManager = new G4RunManager;
  
  // Detector construction
  runManager->SetUserInitialization(new DetectorConstruction());
  // Physics list
  G4VModularPhysicsList* physicsList = new QBBC;
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);
  // User action initialization
  runManager->SetUserInitialization(actioninitial);
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
 

  // Process macro or start UI session
  //
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
