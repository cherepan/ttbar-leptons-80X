#ifndef PROCESSINGMUONS_H
#define PROCESSINGMUONS_H

#include "UserCode/ttbar-leptons-80X/interface/recordFuncs.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "UserCode/llvv_fwk/interface/PatUtils.h"


int processMuons_ID_ISO_Kinematics(pat::MuonCollection& muons, reco::Vertex goodPV, double weight, // input
	patUtils::llvvMuonId::MuonId mu_ID, patUtils::llvvMuonId::MuonId veto_mu_ID,               // config/cuts
	patUtils::llvvMuonIso::MuonIso mu_ISO, patUtils::llvvMuonIso::MuonIso veto_mu_ISO,
	double pt_cut, double eta_cut, double veto_pt_cut, double veto_eta_cut,
	pat::MuonCollection& selMuons, LorentzVector& muDiff, unsigned int& nVetoMu,               // output
	bool record, bool debug); // more output

#endif /* PROCESSINGMUONS_H */


