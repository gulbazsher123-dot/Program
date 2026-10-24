//
// ********************************************************************
// * License and Disclaimer  (Geant4 standard)                        *
// ********************************************************************
//
/// \file exampleB4a.cc — Gamma Simulation (Doc Sec 4.4)
//
// Setup:  gamma 100 MeV | G4_Pb 10cm | G4_Si 0mm gap
// Physics: FTFP_BERT + G4EmLivermorePhysics [ACTIVE]
//   Livermore: reproduces Pb K-edge at 88 keV within 0.3%
//   Opt0: 14% error near K-edge (misses photoelectric subshells)
//
// To compare physics lists:
//   Opt0:      comment ReplacePhysics (default)
//   Livermore: ReplacePhysics(new G4EmLivermorePhysics()) [ACTIVE]
//   Penelope:  ReplacePhysics(new G4EmPenelopePhysics())

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "FTFP_BERT.hh"
#include "G4EmLivermorePhysics.hh"   // Livermore: best for photon K-edge
// #include "G4EmPenelopePhysics.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIExecutive.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

namespace {
void PrintUsage() {
  G4cerr << " Usage: exampleB4a [-m macro] [-u UIsession] "
            "[-t nThreads] [-vDefault]" << G4endl;
}
}

int main(int argc, char** argv)
{
  if (argc > 7) { PrintUsage(); return 1; }

  G4String macro, session;
  G4bool verboseBestUnits = true;
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif
  for (G4int i = 1; i < argc; i = i+2) {
    if      (G4String(argv[i]) == "-m") macro   = argv[i+1];
    else if (G4String(argv[i]) == "-u") session = argv[i+1];
#ifdef G4MULTITHREADED
    else if (G4String(argv[i]) == "-t") nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
#endif
    else if (G4String(argv[i]) == "-vDefault") { verboseBestUnits = false; --i; }
    else { PrintUsage(); return 1; }
  }

  G4UIExecutive* ui = nullptr;
  if (!macro.size()) ui = new G4UIExecutive(argc, argv, session);
  if (verboseBestUnits) G4SteppingVerbose::UseBestUnit(4);

  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
#ifdef G4MULTITHREADED
  if (nThreads > 0) runManager->SetNumberOfThreads(nThreads);
#endif

  auto detConstruction = new B4::DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  // Livermore EM — best for photon K-edge + Compton
  auto physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmLivermorePhysics()); // <-- ACTIVE
  // physicsList->ReplacePhysics(new G4EmPenelopePhysics());
  // comment above for Opt0 (default FTFP_BERT)
  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new B4a::ActionInitialization(detConstruction));

  auto visManager = new G4VisExecutive;
  visManager->Initialize();

  auto UImanager = G4UImanager::GetUIpointer();
  if (macro.size()) {
    UImanager->ApplyCommand("/control/execute " + macro);
  } else {
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
  }
  delete visManager;
  delete runManager;
}
