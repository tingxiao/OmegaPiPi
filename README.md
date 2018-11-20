# OmegaPiPi Final State

## Process:
gamma p->pi^+pi^-pi^+pi^-pi^0 p

The pi^+ and pi^- from the combination of pi^+pi^-pi^0 with invariant mass closest to the omega mass are assumed from omega.

## Root file:
e.g. tree_pi0pippippimpim__B4_031057.root

## Event selections:

### At initialization:
* Time of flight of pi+ and pi- in SYS_TOF, |delta_t| < 2 ns
* kinematic fit cut: DCutAction_KinFitFOM(dComboWrapper, 1e-5)

### Looping over combos:
* Accidental subtraction 
* Beam energy > 8 GeV
* Missing mass squared < 0.04 GeV
* |M(pi+pi-pi0)-M(omega)| < 0.05 GeV 
