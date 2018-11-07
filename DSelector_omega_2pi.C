#include "DSelector_omega_2pi.h"

void DSelector_omega_2pi::Init(TTree *locTree)
{
	// USERS: IN THIS FUNCTION, ONLY MODIFY SECTIONS WITH A "USER" OR "EXAMPLE" LABEL. LEAVE THE REST ALONE.

	// The Init() function is called when the selector needs to initialize a new tree or chain.
	// Typically here the branch addresses and branch pointers of the tree will be set.
	// Init() will be called many times when running on PROOF (once per file to be processed).

	//USERS: SET OUTPUT FILE NAME //can be overriden by user in PROOF
	dOutputFileName = "omega_2pi.root"; //"" for none
	dOutputTreeFileName = ""; //"" for none
	dFlatTreeFileName = ""; //output flat tree (one combo per tree entry), "" for none
	dFlatTreeName = ""; //if blank, default name will be chosen

	//Because this function gets called for each TTree in the TChain, we must be careful:
		//We need to re-initialize the tree interface & branch wrappers, but don't want to recreate histograms
	bool locInitializedPriorFlag = dInitializedFlag; //save whether have been initialized previously
	DSelector::Init(locTree); //This must be called to initialize wrappers for each new TTree
	//gDirectory now points to the output file with name dOutputFileName (if any)
	if(locInitializedPriorFlag)
		return; //have already created histograms, etc. below: exit

	Get_ComboWrappers();
	dPreviousRunNumber = 0;

	/*********************************** EXAMPLE USER INITIALIZATION: ANALYSIS ACTIONS **********************************/

	// EXAMPLE: Create deque for histogramming particle masses:
	// // For histogramming the phi mass in phi -> K+ K-
	// // Be sure to change this and dAnalyzeCutActions to match reaction
	std::deque<Particle_t> MyOmegaPrime;
	MyOmegaPrime.push_back(PiPlus); MyOmegaPrime.push_back(PiMinus); MyOmegaPrime.push_back(PiPlus); MyOmegaPrime.push_back(PiMinus); MyOmegaPrime.push_back(Pi0);

        std::deque<Particle_t> MyOmega;
        MyOmega.push_back(PiPlus); MyOmega.push_back(PiMinus); MyOmega.push_back(Pi0);

	//ANALYSIS ACTIONS: //Executed in order if added to dAnalysisActions
	//false/true below: use measured/kinfit data

	//PID
	dAnalysisActions.push_back(new DHistogramAction_ParticleID(dComboWrapper, false));
	//below: value: +/- N ns, Unknown: All PIDs, SYS_NULL: all timing systems
	dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, PiPlus, SYS_TOF));
	dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, PiMinus, SYS_TOF));

	//MASSES
	dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, Pi0, 1100, 0.08, 0.19, "Pi0"));
	dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, 0, MyOmegaPrime, 4000, 0.5, 4.5, "OmegaPrime"));
        dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, 0, MyOmega, 1000, 0.5, 1.5, "Omega"));
	dAnalysisActions.push_back(new DHistogramAction_MissingMassSquared(dComboWrapper, false, 1000, -0.1, 0.1));

	//KINFIT RESULTS
	dAnalysisActions.push_back(new DHistogramAction_KinFitResults(dComboWrapper));
	dAnalysisActions.push_back(new DCutAction_KinFitFOM(dComboWrapper, 1e-5));

	//MASSES
	dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, Pi0, 1100, 0.08, 0.19, "Pi0_KinFit"));
	dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, true, 0, MyOmegaPrime, 4000, 0.5, 4.5, "OmegaPrime_KinFit"));
        dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, true, 0, MyOmega, 1000, 0.5, 1.5, "Omega_KinFit"));


        //CUT OMEGA
        //dAnalysisActions.push_back(new DCutAction_InvariantMass(dComboWrapper, false, omega, 0.68, 0.88));

        //MASSES
        //dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, true, 0, MyOmegaPrime, 4000, 0.5, 4.5, "OmegaPrime_OmegaCut"));


	//CUT MISSING MASS
	//dAnalysisActions.push_back(new DCutAction_MissingMassSquared(dComboWrapper, false, -0.03, 0.02));

	//BEAM ENERGY
	dAnalysisActions.push_back(new DHistogramAction_BeamEnergy(dComboWrapper, false));
	//dAnalysisActions.push_back(new DCutAction_BeamEnergy(dComboWrapper, false, 8.4, 9.05));

	//KINEMATICS
	dAnalysisActions.push_back(new DHistogramAction_ParticleComboKinematics(dComboWrapper, false));

	// ANALYZE CUT ACTIONS
	// // Change MyPhi to match reaction
	dAnalyzeCutActions = new DHistogramAction_AnalyzeCutActions( dAnalysisActions, dComboWrapper, false, 0, MyOmegaPrime, 4000, 0.5, 4.5, "CutActionEffect" );

	//INITIALIZE ACTIONS
	//If you create any actions that you want to run manually (i.e. don't add to dAnalysisActions), be sure to initialize them here as well
	Initialize_Actions();
	dAnalyzeCutActions->Initialize(); // manual action, must call Initialize()

	/******************************** EXAMPLE USER INITIALIZATION: STAND-ALONE HISTOGRAMS *******************************/

	//EXAMPLE MANUAL HISTOGRAMS:
	dHist_MissingMassSquared = new TH1I("MissingMassSquared", ";Missing Mass Squared (GeV/c^{2})^{2}", 600, -0.06, 0.06);
	dHist_BeamEnergy = new TH1I("BeamEnergy", ";Beam Energy (GeV)", 600, 0.0, 12.0);

        dHist_RFTime = new TH1F("RFTime", "; RF Time (ns)", 100, -10.0, 10.0);
        dHist_RFTimeMeasured = new TH1F("RFTime_Measured", "; RF Time (ns)", 100, -10.0, 10.0);

        dHist_KinFitCL = new TH1F("KinFitCL", ";KinFit CL (%)", 200, 0.0, 1.0);
        dHist_KinFitChiSq = new TH1F("KinFitChiSq", ";KinFit #chi^{2}", 200, 0.0, 100.0);

        dHist_MPiPiPi = new TH1F("MPiPiPi", ";M(#pi^{+}#pi^{-}#pi^{0}) (GeV)", 100, 0.3, 1.3 );
        dHist_MPiPi = new TH1F("MPiPi", ";M(#pi^{+}#pi^{-}) (GeV)", 120, 0.3, 1.5 );

        dHist_M5Pi = new TH1F("M5Pi", ";M(#pi^{+}#pi^{-}#pi^{+}#pi^{-}#pi^{0}) (GeV)", 500, 0.0, 5);

        dHist_MOmegaPip = new TH1F("MOmegaPip", ";M(#omega#pi^{+}) (GeV)", 500, 0.0, 5);
        dHist_MOmegaPim = new TH1F("MOmegaPim", ";M(#omega#pi^{-}) (GeV)", 500, 0.0, 5);

        dHist_MOmegaPiPi = new TH1F("MOmegaPiPi", ";M(#omega#pi^{+}#pi^{-}) (GeV)", 500, 0.0, 5);
        dHist_MOmegaPiPi_acc = new TH1F("MOmegaPiPi_acc", ";M(#omega#pi^{+}#pi^{-}) (GeV)", 500, 0.0, 5);
	dHist_Pi0Mass = new TH1F("Pi0Mass", ";#gamma#gamma Invariant Mass", 680, 0.05, 0.22);

	dHist_MOmegaPiPi_vs_MPiPiPi = new TH2I("MOmegaPiPi_vs_MPiPiPi", " ;M(#omega#pi^{+}#pi^{-}) (GeV);M(#pi^{+}#pi^{-}#pi^{0}) (GeV)", 500, 0.0, 5.0, 500, 0.0, 5.0);
        dHist_MOmegaPiPi_vs_MPiPi = new TH2I("MOmegaPiPi_vs_MPiPi", " ;M(#omega#pi^{+}#pi^{-}) (GeV);M(#pi^{+}#pi^{-}) (GeV) (after #omega cut)", 500, 0.0, 5.0, 500, 0.0, 5.0);



	/************************** EXAMPLE USER INITIALIZATION: CUSTOM OUTPUT BRANCHES - MAIN TREE *************************/

	//EXAMPLE MAIN TREE CUSTOM BRANCHES (OUTPUT ROOT FILE NAME MUST FIRST BE GIVEN!!!! (ABOVE: TOP)):
	//The type for the branch must be included in the brackets
	//1st function argument is the name of the branch
	//2nd function argument is the name of the branch that contains the size of the array (for fundamentals only)
	/*
	dTreeInterface->Create_Branch_Fundamental<Int_t>("my_int"); //fundamental = char, int, float, double, etc.
	dTreeInterface->Create_Branch_FundamentalArray<Int_t>("my_int_array", "my_int");
	dTreeInterface->Create_Branch_FundamentalArray<Float_t>("my_combo_array", "NumCombos");
	dTreeInterface->Create_Branch_NoSplitTObject<TLorentzVector>("my_p4");
	dTreeInterface->Create_Branch_ClonesArray<TLorentzVector>("my_p4_array");
	*/

	/************************** EXAMPLE USER INITIALIZATION: CUSTOM OUTPUT BRANCHES - FLAT TREE *************************/

	//EXAMPLE FLAT TREE CUSTOM BRANCHES (OUTPUT ROOT FILE NAME MUST FIRST BE GIVEN!!!! (ABOVE: TOP)):
	//The type for the branch must be included in the brackets
	//1st function argument is the name of the branch
	//2nd function argument is the name of the branch that contains the size of the array (for fundamentals only)
	/*
	dFlatTreeInterface->Create_Branch_Fundamental<Int_t>("flat_my_int"); //fundamental = char, int, float, double, etc.
	dFlatTreeInterface->Create_Branch_FundamentalArray<Int_t>("flat_my_int_array", "flat_my_int");
	dFlatTreeInterface->Create_Branch_NoSplitTObject<TLorentzVector>("flat_my_p4");
	dFlatTreeInterface->Create_Branch_ClonesArray<TLorentzVector>("flat_my_p4_array");
	*/

	/************************************* ADVANCED EXAMPLE: CHOOSE BRANCHES TO READ ************************************/

	//TO SAVE PROCESSING TIME
		//If you know you don't need all of the branches/data, but just a subset of it, you can speed things up
		//By default, for each event, the data is retrieved for all branches
		//If you know you only need data for some branches, you can skip grabbing data from the branches you don't need
		//Do this by doing something similar to the commented code below

	//dTreeInterface->Clear_GetEntryBranches(); //now get none
	//dTreeInterface->Register_GetEntryBranch("Proton__P4"); //manually set the branches you want
}

Bool_t DSelector_omega_2pi::Process(Long64_t locEntry)
{
	// The Process() function is called for each entry in the tree. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	//
	// This function should contain the "body" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	// Use fStatus to set the return value of TTree::Process().
	// The return value is currently not used.

	//CALL THIS FIRST
	DSelector::Process(locEntry); //Gets the data from the tree for the entry
	//cout << "RUN " << Get_RunNumber() << ", EVENT " << Get_EventNumber() << endl;
	//TLorentzVector locProductionX4 = Get_X4_Production();

	/******************************************** GET POLARIZATION ORIENTATION ******************************************/

	//Only if the run number changes
	//RCDB environment must be setup in order for this to work! (Will return false otherwise)
	UInt_t locRunNumber = Get_RunNumber();
	if(locRunNumber != dPreviousRunNumber)
	{
		dIsPolarizedFlag = dAnalysisUtilities.Get_IsPolarizedBeam(locRunNumber, dIsPARAFlag);
		dPreviousRunNumber = locRunNumber;
	}

	/********************************************* SETUP UNIQUENESS TRACKING ********************************************/

	//ANALYSIS ACTIONS: Reset uniqueness tracking for each action
	//For any actions that you are executing manually, be sure to call Reset_NewEvent() on them here
	Reset_Actions_NewEvent();
	//dAnalyzeCutActions->Reset_NewEvent(); // manual action, must call Reset_NewEvent()

	//PREVENT-DOUBLE COUNTING WHEN HISTOGRAMMING
		//Sometimes, some content is the exact same between one combo and the next
			//e.g. maybe two combos have different beam particles, but the same data for the final-state
		//When histogramming, you don't want to double-count when this happens: artificially inflates your signal (or background)
		//So, for each quantity you histogram, keep track of what particles you used (for a given combo)
		//Then for each combo, just compare to what you used before, and make sure it's unique

	//EXAMPLE 1: Particle-specific info:
	set<Int_t> locUsedSoFar_BeamEnergy; //Int_t: Unique ID for beam particles. set: easy to use, fast to search

	//EXAMPLE 2: Combo-specific info:
		//In general: Could have multiple particles with the same PID: Use a set of Int_t's
		//In general: Multiple PIDs, so multiple sets: Contain within a map
		//Multiple combos: Contain maps within a set (easier, faster to search)
	set<map<Particle_t, set<Int_t> > > locUsedSoFar_MissingMass;
        set<map<Particle_t, set<Int_t> > > locUsedSoFar_OmegaPiPiMass;
	set<set<Int_t> > locUsedSoFar_Pi0Mass;

	//INSERT USER ANALYSIS UNIQUENESS TRACKING HERE

	/**************************************** EXAMPLE: FILL CUSTOM OUTPUT BRANCHES **************************************/

	/*
	Int_t locMyInt = 7;
	dTreeInterface->Fill_Fundamental<Int_t>("my_int", locMyInt);

	TLorentzVector locMyP4(4.0, 3.0, 2.0, 1.0);
	dTreeInterface->Fill_TObject<TLorentzVector>("my_p4", locMyP4);

	for(int loc_i = 0; loc_i < locMyInt; ++loc_i)
		dTreeInterface->Fill_Fundamental<Int_t>("my_int_array", 3*loc_i, loc_i); //2nd argument = value, 3rd = array index
	*/

	/************************************************* LOOP OVER COMBOS *************************************************/

	//Loop over combos
	for(UInt_t loc_i = 0; loc_i < Get_NumCombos(); ++loc_i)
	{
		//Set branch array indices for combo and all combo particles
		dComboWrapper->Set_ComboIndex(loc_i);

		// Is used to indicate when combos have been cut
		if(dComboWrapper->Get_IsComboCut()) // Is false when tree originally created
			continue; // Combo has been cut previously

		/********************************************** GET PARTICLE INDICES *********************************************/

		//Used for tracking uniqueness when filling histograms, and for determining unused particles

		//Step 0
		Int_t locBeamID = dComboBeamWrapper->Get_BeamID();
		Int_t locPiPlus1TrackID = dPiPlus1Wrapper->Get_TrackID();
		Int_t locPiPlus2TrackID = dPiPlus2Wrapper->Get_TrackID();
		Int_t locPiMinus1TrackID = dPiMinus1Wrapper->Get_TrackID();
		Int_t locPiMinus2TrackID = dPiMinus2Wrapper->Get_TrackID();
		Int_t locProtonTrackID = dProtonWrapper->Get_TrackID();

		//Step 1
		Int_t locPhoton1NeutralID = dPhoton1Wrapper->Get_NeutralID();
		Int_t locPhoton2NeutralID = dPhoton2Wrapper->Get_NeutralID();

		/*********************************************** GET FOUR-MOMENTUM **********************************************/

		// Get P4's: //is kinfit if kinfit performed, else is measured
		//dTargetP4 is target p4

		//Step 0
                TLorentzVector locBeamX4 = dComboBeamWrapper->Get_X4();
		TLorentzVector locBeamP4 = dComboBeamWrapper->Get_P4();
		TLorentzVector locPiPlus1P4 = dPiPlus1Wrapper->Get_P4();
		TLorentzVector locPiPlus2P4 = dPiPlus2Wrapper->Get_P4();
		TLorentzVector locPiMinus1P4 = dPiMinus1Wrapper->Get_P4();
		TLorentzVector locPiMinus2P4 = dPiMinus2Wrapper->Get_P4();
		TLorentzVector locProtonP4 = dProtonWrapper->Get_P4();
		//Step 1
		TLorentzVector locDecayingPi0P4 = dDecayingPi0Wrapper->Get_P4();
		TLorentzVector locPhoton1P4 = dPhoton1Wrapper->Get_P4();
		TLorentzVector locPhoton2P4 = dPhoton2Wrapper->Get_P4();

		// Get Measured P4's:
		//Step 0
                TLorentzVector locBeamX4_Measured = dComboBeamWrapper->Get_X4_Measured();
		TLorentzVector locBeamP4_Measured = dComboBeamWrapper->Get_P4_Measured();
		TLorentzVector locPiPlus1P4_Measured = dPiPlus1Wrapper->Get_P4_Measured();
		TLorentzVector locPiPlus2P4_Measured = dPiPlus2Wrapper->Get_P4_Measured();
		TLorentzVector locPiMinus1P4_Measured = dPiMinus1Wrapper->Get_P4_Measured();
		TLorentzVector locPiMinus2P4_Measured = dPiMinus2Wrapper->Get_P4_Measured();
		TLorentzVector locProtonP4_Measured = dProtonWrapper->Get_P4_Measured();
		//Step 1
		TLorentzVector locPhoton1P4_Measured = dPhoton1Wrapper->Get_P4_Measured();
		TLorentzVector locPhoton2P4_Measured = dPhoton2Wrapper->Get_P4_Measured();

		/********************************************* COMBINE FOUR-MOMENTUM ********************************************/

		// DO YOUR STUFF HERE

		// Combine 4-vectors
		TLorentzVector locMissingP4_Measured = locBeamP4_Measured + dTargetP4;
		locMissingP4_Measured -= locPiPlus1P4_Measured + locPiPlus2P4_Measured + locPiMinus1P4_Measured + locPiMinus2P4_Measured + locProtonP4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;

                TLorentzVector locPi0P4 = locPhoton1P4 + locPhoton2P4;
                TLorentzVector locPiPiPiP4_combo11 = locPiPlus1P4 + locPiMinus1P4 + locPhoton1P4 + locPhoton2P4;
                TLorentzVector locPiPiPiP4_combo12 = locPiPlus1P4 + locPiMinus2P4 + locPhoton1P4 + locPhoton2P4;
                TLorentzVector locPiPiPiP4_combo21 = locPiPlus2P4 + locPiMinus1P4 + locPhoton1P4 + locPhoton2P4;
                TLorentzVector locPiPiPiP4_combo22 = locPiPlus2P4 + locPiMinus2P4 + locPhoton1P4 + locPhoton2P4;

                TLorentzVector locOmegaPiPiP4 = locPiPlus1P4 + locPiMinus1P4 + locPiPlus2P4 + locPiMinus1P4 + locPhoton1P4 + locPhoton2P4;

		TLorentzVector locPi0P4_Measured = locPhoton1P4_Measured + locPhoton2P4_Measured;
                TLorentzVector locPiPiPiP4_combo11_Measured = locPiPlus1P4_Measured + locPiMinus1P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;
                TLorentzVector locPiPiPiP4_combo12_Measured = locPiPlus1P4_Measured + locPiMinus2P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;
                TLorentzVector locPiPiPiP4_combo21_Measured = locPiPlus2P4_Measured + locPiMinus1P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;
                TLorentzVector locPiPiPiP4_combo22_Measured = locPiPlus2P4_Measured + locPiMinus2P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;

                TLorentzVector locOmegaPiPiP4_Measured = locPiPlus1P4_Measured + locPiMinus1P4_Measured + locPiPlus2P4_Measured + locPiMinus1P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;


		/******************************************** EXECUTE ANALYSIS ACTIONS *******************************************/

		// Loop through the analysis actions, executing them in order for the active particle combo
		dAnalyzeCutActions->Perform_Action(); // Must be executed before Execute_Actions()
		if(!Execute_Actions()) //if the active combo fails a cut, IsComboCutFlag automatically set
			continue;

		//if you manually execute any actions, and it fails a cut, be sure to call:
			//dComboWrapper->Set_IsComboCut(true);

		/**************************************** EXAMPLE: FILL CUSTOM OUTPUT BRANCHES **************************************/

		/*
		TLorentzVector locMyComboP4(8.0, 7.0, 6.0, 5.0);
		//for arrays below: 2nd argument is value, 3rd is array index
		//NOTE: By filling here, AFTER the cuts above, some indices won't be updated (and will be whatever they were from the last event)
			//So, when you draw the branch, be sure to cut on "IsComboCut" to avoid these.
		dTreeInterface->Fill_Fundamental<Float_t>("my_combo_array", -2*loc_i, loc_i);
		dTreeInterface->Fill_TObject<TLorentzVector>("my_p4_array", locMyComboP4, loc_i);
		*/


		/******************************************** ACCIDENTAL SUBRACTION INFO *******************************************/


		// time difference between tagger and RF (corrected for production vertex position relative to target center)
              	double locDeltaTRF_Measured = locBeamX4_Measured.T() - (dComboWrapper->Get_RFTime_Measured()
                                                  + (locBeamX4_Measured.Z() - dComboWrapper->Get_TargetCenter().Z() )/29.9792458);
		dHist_RFTimeMeasured->Fill(locDeltaTRF_Measured);

                double locDeltaTRF = locBeamX4.T() - (dComboWrapper->Get_RFTime()
                                                  + (locBeamX4.Z() - dComboWrapper->Get_TargetCenter().Z() )/29.9792458);
                dHist_RFTime->Fill(locDeltaTRF);

		// calculate accidental subtraction weight based on time difference

		double locWeight = 0.; // weight to accidentally subtracted histgorams
		bool locAccid = false; // flag to fill separate prompt and accidental histograms for later subtraction

		if(fabs(locDeltaTRF_Measured) < 0.5*4.008) { // prompt signal recieves a weight of 1
			locWeight = 1.;
			locAccid = false;
		}
                else { // accidentals recieve a weight of 1/# RF bunches included in TTree (8 in this case)
			locWeight = -1./8.;
			locAccid = true;
		}


                if(locAccid) {
                        dComboWrapper->Set_IsComboCut(true);
                        continue;
                }

		/**************************************** FILL 5PI MASS BEFORE PID CUTS *************************************/
		dHist_M5Pi->Fill(locOmegaPiPiP4_Measured.M());

                // make pions from omega as 1, the other pions as 2

		Double_t omega_PDG = 0.78265;
		Double_t combo11 = fabs(locPiPiPiP4_combo11_Measured.M() - omega_PDG);
                Double_t combo12 = fabs(locPiPiPiP4_combo12_Measured.M() - omega_PDG);
                Double_t combo21 = fabs(locPiPiPiP4_combo21_Measured.M() - omega_PDG);
                Double_t combo22 = fabs(locPiPiPiP4_combo22_Measured.M() - omega_PDG);
		Double_t omega_min = combo11;
		TLorentzVector temp_pi;

		if((combo12 < combo11) && (combo12 < combo21) && (combo12 < combo22)) {
			omega_min = combo12;
			temp_pi = locPiMinus2P4_Measured;
			locPiMinus2P4_Measured = locPiMinus1P4_Measured;
			locPiMinus1P4_Measured = temp_pi;
                        temp_pi = locPiMinus2P4;
                        locPiMinus2P4 = locPiMinus1P4;
                        locPiMinus1P4 = temp_pi;
		}

                else if((combo21 < combo11) && (combo21 < combo12) && (combo21 < combo22)) {
			omega_min = combo21;
                        temp_pi = locPiPlus2P4_Measured;
                        locPiPlus2P4_Measured = locPiPlus1P4_Measured;
                        locPiPlus1P4_Measured = temp_pi;
                        temp_pi = locPiPlus2P4;
                        locPiPlus2P4 = locPiPlus1P4;
                        locPiPlus1P4 = temp_pi;
                }
                else if((combo22 < combo11) && (combo22 < combo12) && (combo22 < combo21)) {
                        omega_min = combo22;
                        temp_pi = locPiPlus2P4_Measured;
                        locPiPlus2P4_Measured = locPiPlus1P4_Measured;
                        locPiPlus1P4_Measured = temp_pi;
                        temp_pi = locPiPlus2P4;
                        locPiPlus2P4 = locPiPlus1P4;
                        locPiPlus1P4 = temp_pi;
                        temp_pi = locPiMinus2P4_Measured;
                        locPiMinus2P4_Measured = locPiMinus1P4_Measured;
                        locPiMinus1P4_Measured = temp_pi;
                        temp_pi = locPiMinus2P4;
                        locPiMinus2P4 = locPiMinus1P4;
                        locPiMinus1P4 = temp_pi;
                }

                // 3 pions from omega
                TLorentzVector locPiPiPiP4 = locPiPlus1P4 + locPiMinus1P4 + locPhoton1P4 + locPhoton2P4;
		TLorentzVector locPiPiPiP4_Measured = locPiPlus1P4_Measured + locPiMinus1P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured;

		// 2 pions not from omega
                TLorentzVector locPiPiP4 = locPiPlus2P4 + locPiMinus2P4;
                TLorentzVector locPiPiP4_Measured = locPiPlus2P4_Measured + locPiMinus2P4_Measured;

		// omega + pi
                TLorentzVector locPiPiPiPipP4 = locPiPlus1P4 + locPiMinus1P4 + locPhoton1P4 + locPhoton2P4 + locPiPlus2P4;
                TLorentzVector locPiPiPiPipP4_Measured = locPiPlus1P4_Measured + locPiMinus1P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured + locPiPlus2P4_Measured;
                TLorentzVector locPiPiPiPimP4 = locPiPlus1P4 + locPiMinus1P4 + locPhoton1P4 + locPhoton2P4 + locPiMinus2P4;
                TLorentzVector locPiPiPiPimP4_Measured = locPiPlus1P4_Measured + locPiMinus1P4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured + locPiMinus2P4_Measured;


		/**************************************** EXAMPLE: HISTOGRAM BEAM ENERGY *****************************************/


		//Histogram beam energy (if haven't already)
		if(locUsedSoFar_BeamEnergy.find(locBeamID) == locUsedSoFar_BeamEnergy.end())
		{
			dHist_BeamEnergy->Fill(locBeamP4.E());
			locUsedSoFar_BeamEnergy.insert(locBeamID);
		}

		/************************************ EXAMPLE: HISTOGRAM MISSING MASS SQUARED ************************************/
/*
                // let's cut on the kinematic fit first
                dHist_KinFitCL->Fill(dComboWrapper->Get_ConfidenceLevel_KinFit());
                dHist_KinFitChiSq->Fill(dComboWrapper->Get_ChiSq_KinFit());

                //if(dComboWrapper->Get_ConfidenceLevel_KinFit() <= 0.) {
                //if(dComboWrapper->Get_ConfidenceLevel_KinFit() < 1.E-20) { 
                //if(dComboWrapper->Get_ConfidenceLevel_KinFit() < 5.73303E-7) {  // 5 sigma
                if(dComboWrapper->Get_ConfidenceLevel_KinFit() < 0.01) { 
                //if(dComboWrapper->Get_ConfidenceLevel_KinFit() < 0.1) { 
                        dComboWrapper->Set_IsComboCut(true);
                        continue;
                }
*/
		//Missing Mass Squared
		double locMissingMassSquared = locMissingP4_Measured.M2();

		//Uniqueness tracking: Build the map of particles used for the missing mass
			//For beam: Don't want to group with final-state photons. Instead use "Unknown" PID (not ideal, but it's easy).
		map<Particle_t, set<Int_t> > locUsedThisCombo_MissingMass;
		locUsedThisCombo_MissingMass[Unknown].insert(locBeamID); //beam
		locUsedThisCombo_MissingMass[PiPlus].insert(locPiPlus1TrackID);
		locUsedThisCombo_MissingMass[PiPlus].insert(locPiPlus2TrackID);
		locUsedThisCombo_MissingMass[PiMinus].insert(locPiMinus1TrackID);
		locUsedThisCombo_MissingMass[PiMinus].insert(locPiMinus2TrackID);
		locUsedThisCombo_MissingMass[Proton].insert(locProtonTrackID);
		locUsedThisCombo_MissingMass[Gamma].insert(locPhoton1NeutralID);
		locUsedThisCombo_MissingMass[Gamma].insert(locPhoton2NeutralID);

		//compare to what's been used so far
		if(locUsedSoFar_MissingMass.find(locUsedThisCombo_MissingMass) == locUsedSoFar_MissingMass.end())
		{
			//unique missing mass combo: histogram it, and register this combo of particles
			dHist_MissingMassSquared->Fill(locMissingMassSquared);
			locUsedSoFar_MissingMass.insert(locUsedThisCombo_MissingMass);
		}

		//E.g. Cut
		if((locMissingMassSquared < -0.04) || (locMissingMassSquared > 0.04))
		{
			dComboWrapper->Set_IsComboCut(true);
			continue;
		}



		/***************************************** HISTOGRAM Pi0 INVARIANT MASS ******************************************/

		double locPi0Mass = locPi0P4_Measured.M();

		//Uniqueness tracking:
		set<Int_t> locUsedThisCombo_Pi0Mass;
		locUsedThisCombo_Pi0Mass.insert(locPhoton1NeutralID);
		locUsedThisCombo_Pi0Mass.insert(locPhoton2NeutralID);

		//compare to what's been used so far
		if(locUsedSoFar_Pi0Mass.find(locUsedThisCombo_Pi0Mass) == locUsedSoFar_Pi0Mass.end())
		{
			//unique missing mass combo: histogram it, and register this combo of particles
			dHist_Pi0Mass->Fill(locPi0Mass);
			locUsedSoFar_Pi0Mass.insert(locUsedThisCombo_Pi0Mass);
		}


                /***************************************** HISTOGRAM OMEGA PIPI INVARIANT MASS, ETC ******************************************/

                map<Particle_t, set<Int_t> > locUsedThisCombo_OmegaPiPiMass;
                locUsedThisCombo_OmegaPiPiMass[PiPlus].insert(locPiPlus1TrackID);
                locUsedThisCombo_OmegaPiPiMass[PiPlus].insert(locPiPlus2TrackID);
                locUsedThisCombo_OmegaPiPiMass[PiMinus].insert(locPiMinus1TrackID);
                locUsedThisCombo_OmegaPiPiMass[PiMinus].insert(locPiMinus2TrackID);
                locUsedThisCombo_OmegaPiPiMass[Gamma].insert(locPhoton1NeutralID);
                locUsedThisCombo_OmegaPiPiMass[Gamma].insert(locPhoton2NeutralID);

                //compare to what's been used so far
                if(locUsedSoFar_OmegaPiPiMass.find(locUsedThisCombo_OmegaPiPiMass) == locUsedSoFar_OmegaPiPiMass.end())
                {
                  // let's plot some masses

                        dHist_MPiPi->Fill(locPiPiP4_Measured.M());
			dHist_MPiPiPi->Fill(locPiPiPiP4_Measured.M());
			dHist_MOmegaPiPi_vs_MPiPiPi->Fill(locOmegaPiPiP4_Measured.M(),locPiPiPiP4_Measured.M());

	                if(omega_min > 0.05) {
        	                dComboWrapper->Set_IsComboCut(true);
                	        continue;
                	}

			dHist_MOmegaPiPi->Fill(locOmegaPiPiP4_Measured.M());
                        dHist_MOmegaPiPi_vs_MPiPi->Fill(locOmegaPiPiP4_Measured.M(),locPiPiP4_Measured.M());

                        dHist_MOmegaPip->Fill(locPiPiPiPipP4_Measured.M());
                        dHist_MOmegaPim->Fill(locPiPiPiPimP4_Measured.M());

                	locUsedSoFar_OmegaPiPiMass.insert(locUsedThisCombo_OmegaPiPiMass);

                }




		/****************************************** FILL FLAT TREE (IF DESIRED) ******************************************/

		/*
		//FILL ANY CUSTOM BRANCHES FIRST!!
		Int_t locMyInt_Flat = 7;
		dFlatTreeInterface->Fill_Fundamental<Int_t>("flat_my_int", locMyInt_Flat);

		TLorentzVector locMyP4_Flat(4.0, 3.0, 2.0, 1.0);
		dFlatTreeInterface->Fill_TObject<TLorentzVector>("flat_my_p4", locMyP4_Flat);

		for(int loc_j = 0; loc_j < locMyInt_Flat; ++loc_j)
		{
			dFlatTreeInterface->Fill_Fundamental<Int_t>("flat_my_int_array", 3*loc_j, loc_j); //2nd argument = value, 3rd = array index
			TLorentzVector locMyComboP4_Flat(8.0, 7.0, 6.0, 5.0);
			dFlatTreeInterface->Fill_TObject<TLorentzVector>("flat_my_p4_array", locMyComboP4_Flat, loc_j);
		}
		*/

		//FILL FLAT TREE
		//Fill_FlatTree(); //for the active combo
	} // end of combo loop

	//FILL HISTOGRAMS: Num combos / events surviving actions
	Fill_NumCombosSurvivedHists();

	/******************************************* LOOP OVER THROWN DATA (OPTIONAL) ***************************************/
/*
	//Thrown beam: just use directly
	if(dThrownBeam != NULL)
		double locEnergy = dThrownBeam->Get_P4().E();

	//Loop over throwns
	for(UInt_t loc_i = 0; loc_i < Get_NumThrown(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dThrownWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}
*/
	/****************************************** LOOP OVER OTHER ARRAYS (OPTIONAL) ***************************************/
/*
	//Loop over beam particles (note, only those appearing in combos are present)
	for(UInt_t loc_i = 0; loc_i < Get_NumBeam(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dBeamWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}

	//Loop over charged track hypotheses
	for(UInt_t loc_i = 0; loc_i < Get_NumChargedHypos(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dChargedHypoWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}

	//Loop over neutral particle hypotheses
	for(UInt_t loc_i = 0; loc_i < Get_NumNeutralHypos(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dNeutralHypoWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}
*/

	/************************************ EXAMPLE: FILL CLONE OF TTREE HERE WITH CUTS APPLIED ************************************/
/*
	Bool_t locIsEventCut = true;
	for(UInt_t loc_i = 0; loc_i < Get_NumCombos(); ++loc_i) {
		//Set branch array indices for combo and all combo particles
		dComboWrapper->Set_ComboIndex(loc_i);
		// Is used to indicate when combos have been cut
		if(dComboWrapper->Get_IsComboCut())
			continue;
		locIsEventCut = false; // At least one combo succeeded
		break;
	}
	if(!locIsEventCut && dOutputTreeFileName != "")
		Fill_OutputTree();
*/

	return kTRUE;
}

void DSelector_omega_2pi::Finalize(void)
{
	//Save anything to output here that you do not want to be in the default DSelector output ROOT file.

	//Otherwise, don't do anything else (especially if you are using PROOF).
		//If you are using PROOF, this function is called on each thread,
		//so anything you do will not have the combined information from the various threads.
		//Besides, it is best-practice to do post-processing (e.g. fitting) separately, in case there is a problem.

	//DO YOUR STUFF HERE

	//CALL THIS LAST
	DSelector::Finalize(); //Saves results to the output file
}
