#ifndef DSelector_omega_2pi_h
#define DSelector_omega_2pi_h

#include <iostream>

#include "DSelector/DSelector.h"
#include "DSelector/DHistogramActions.h"
#include "DSelector/DCutActions.h"

#include "TH1I.h"
#include "TH2I.h"

class DSelector_omega_2pi : public DSelector
{
	public:

		DSelector_omega_2pi(TTree* locTree = NULL) : DSelector(locTree){}
		virtual ~DSelector_omega_2pi(){}

		void Init(TTree *tree);
		Bool_t Process(Long64_t entry);

	private:

		void Get_ComboWrappers(void);
		void Finalize(void);

		// BEAM POLARIZATION INFORMATION
		UInt_t dPreviousRunNumber;
		bool dIsPolarizedFlag; //else is AMO
		bool dIsPARAFlag; //else is PERP or AMO

		// ANALYZE CUT ACTIONS
		// // Automatically makes mass histograms where one cut is missing
		DHistogramAction_AnalyzeCutActions* dAnalyzeCutActions;

		//CREATE REACTION-SPECIFIC PARTICLE ARRAYS

		//Step 0
		DParticleComboStep* dStep0Wrapper;
		DBeamParticle* dComboBeamWrapper;
		DChargedTrackHypothesis* dPiPlus1Wrapper;
		DChargedTrackHypothesis* dPiPlus2Wrapper;
		DChargedTrackHypothesis* dPiMinus1Wrapper;
		DChargedTrackHypothesis* dPiMinus2Wrapper;
		DChargedTrackHypothesis* dProtonWrapper;

		//Step 1
		DParticleComboStep* dStep1Wrapper;
		DKinematicData* dDecayingPi0Wrapper;
		DNeutralParticleHypothesis* dPhoton1Wrapper;
		DNeutralParticleHypothesis* dPhoton2Wrapper;

		// DEFINE YOUR HISTOGRAMS HERE
		// EXAMPLES:
		TH1I* dHist_MissingMassSquared;
                TH1I* dHist_MissingMassSquared_prompt;
                TH1I* dHist_MissingMassSquared_acc;

		TH1I* dHist_BeamEnergy;

                TH1F* dHist_RFTime;
                TH1F* dHist_RFTimeMeasured;

                TH1F* dHist_KinFitCL;
                TH1F* dHist_KinFitChiSq;

                TH1F* dHist_MPiPi;
                TH1F* dHist_MPiPiPi;
                TH1F* dHist_MPiPiPi_acc;
                TH1F* dHist_MPiPiPi_prompt;

                TH1F* dHist_M5Pi;
                TH1F* dHist_MOmegaPiPi;
                TH1F* dHist_MOmegaPiPi_acc;
                TH1F* dHist_MOmegaPiPi_prompt;
                TH1F* dHist_Pi0Mass;
                TH1F* dHist_Pi0Mass_acc;
                TH1F* dHist_Pi0Mass_prompt;

                TH1F* dHist_MOmegaPip;
                TH1F* dHist_MOmegaPim;
                TH2I* dHist_MOmegaPiPi_vs_MPiPiPi;
                TH2I* dHist_MOmegaPiPi_vs_MPiPi;



	ClassDef(DSelector_omega_2pi, 0);
};

void DSelector_omega_2pi::Get_ComboWrappers(void)
{
	//Step 0
	dStep0Wrapper = dComboWrapper->Get_ParticleComboStep(0);
	dComboBeamWrapper = static_cast<DBeamParticle*>(dStep0Wrapper->Get_InitialParticle());
	dPiPlus1Wrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(1));
	dPiPlus2Wrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(2));
	dPiMinus1Wrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(3));
	dPiMinus2Wrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(4));
	dProtonWrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(5));

	//Step 1
	dStep1Wrapper = dComboWrapper->Get_ParticleComboStep(1);
	dDecayingPi0Wrapper = dStep1Wrapper->Get_InitialParticle();
	dPhoton1Wrapper = static_cast<DNeutralParticleHypothesis*>(dStep1Wrapper->Get_FinalParticle(0));
	dPhoton2Wrapper = static_cast<DNeutralParticleHypothesis*>(dStep1Wrapper->Get_FinalParticle(1));
}

#endif // DSelector_omega_2pi_h
