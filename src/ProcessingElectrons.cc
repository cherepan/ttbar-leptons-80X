#include "UserCode/ttbar-leptons-80X/interface/ProcessingElectrons.h"
#include "UserCode/ttbar-leptons-80X/interface/recordFuncs.h"



int processElectrons_ID_ISO_Kinematics(pat::ElectronCollection& electrons, reco::Vertex goodPV, double rho, double weight, // input
	patUtils::llvvElecId::ElecId el_ID, patUtils::llvvElecId::ElecId veto_el_ID,                           // config/cuts
	patUtils::llvvElecIso::ElecIso el_ISO, patUtils::llvvElecIso::ElecIso veto_el_ISO,
	double pt_cut, double eta_cut, double veto_pt_cut, double veto_eta_cut,
	pat::ElectronCollection& selElectrons, LorentzVector& elDiff, unsigned int& nVetoE,                    // output
	bool record, bool debug) // more output
{

for(unsigned int count_idiso_electrons = 0, n=0; n<electrons.size (); ++n)
	{
	pat::Electron& electron = electrons[n];

	if (record)
		{
		fill_2d(string("control_el_slimmedelectrons_pt_eta"), 250, 0., 500., 200, -3., 3., electron.pt(), electron.eta(), weight);
		fill_1d(string("control_el_slimmedelectrons_phi"), 128, -3.2, 3.2, electron.phi(), weight);
		//fill_1d(string("control_tau_slimmedtaus_phi"), 128, -3.2, 3.2, tau.phi(), weight);
		}

	bool 
		passKin(true),     passId(true),     passIso(true),
		passVetoKin(true), passVetoId(true), passVetoIso(true);
	bool passSigma(false), passSigmaVeto(false);
	bool passImpactParameter(false), passImpactParameterVeto(true);
	// from passId( pat::Photon .. ) of PatUtils:
	//bool elevto = photon.hasPixelSeed();  //LQ  REACTIVATED FOR TIGHT ID, OTHERWISE MANY ELECtRONS pass the photon Id
	// and then, in Tight ID, they include:
	// && !elevto 
	//
	// So, it would be nice to add it to Tight Electron ID:
	//bool hasPixelSeed = electron.hasPixelSeed();
	// but electrons don't have this method
	// will have to cross-clean with photons or etc

	/* did it for excess of 1 highly weighted QCD in e-tau, it didn't change anything
	// removing all electrons close to tight Photons
	// actually, people do it the other way around, testing v9.5
	double minDRlg(9999.);
	for(size_t i=0; i<selPhotons.size(); i++)
		{
		minDRlg = TMath::Min(minDRlg, deltaR(electron.p4(), selPhotons[i].p4()));
		}
	if(minDRlg<0.1) continue;
	*/

	int lid(electron.pdgId()); // should always be 11

	//apply electron corrections
	/* no lepcorrs in 13.6
	if(abs(lid)==11)
		{
		elDiff -= electron.p4();
		ElectronEnCorrector.calibrate(electron, ev.eventAuxiliary().run(), edm::StreamID::invalidStreamID()); 
		//electron = patUtils::GenericLepton(electron.el); //recreate the generic electron to be sure that the p4 is ok
		elDiff += electron.p4();
		}
	*/

	// TODO: probably, should make separate collections for each step, for corrected particles, then -- passed ID etc
	// fill_pt_e( string("all_electrons_corrected_pt"), electron.pt(), weight);
	// if (n < 2)
	// 	{
	// 	fill_pt_e( string("top2pt_electrons_corrected_pt"), electron.pt(), weight);
	// 	}

	//no need for charge info any longer
	//lid = abs(lid);
	//TString lepStr(lid == 13 ? "mu" : "e");
	// should always be 11!
			
	// no need to mess with photon ID // //veto nearby photon (loose electrons are many times photons...)
	// no need to mess with photon ID // double minDRlg(9999.);
	// no need to mess with photon ID // for(size_t ipho=0; ipho<selPhotons.size(); ipho++)
	// no need to mess with photon ID //   minDRlg=TMath::Min(minDRlg,deltaR(leptons[n].p4(),selPhotons[ipho].p4()));
	// no need to mess with photon ID // if(minDRlg<0.1) continue;


	// ------------------------- electron IDs
	//Cut based identification
	//https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Working_points_for_2016_data_for
	// full5x5_sigmaIetaIeta is forgotten in our passId for electrons
	// getting high MC/Data in electrons now -- maybe due to photons,
	// checking sigmaIetaIeta, then photon rejection (hasPixelSeed()) and cross-cleaning with photons
	float eta = std::abs(electron.superCluster()->position().eta());
	float sigmaIetaIeta = electron.full5x5_sigmaIetaIeta();
	if (eta <= 1.479) // barrel, newer selection is precise?
		{
		passSigma =     sigmaIetaIeta < 0.00998; // Tight WP
		passSigmaVeto = sigmaIetaIeta < 0.0115;  // Veto WP
		}
	else if (eta > 1.479) // endcap
		{
		passSigma =     sigmaIetaIeta < 0.0292; // Tight WP
		passSigmaVeto = sigmaIetaIeta < 0.037;  // Veto WP
		}
	passImpactParameter = electron.dB() < 0.02;
	// what units is this? in the PAT example on top they say "we use < 0.02cm",
	// in recommendations for muons it is < 0.2 
	// and say "The 2 mm cut preserves efficiency for muons from decays of b and c hadrons"
	//https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookPATExampleTopQuarks
	//https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId
	//
	// the electron ID from parUtils has the impact parameter commented out:
	//                  //dxy             < 0.05     &&
	//                  //dz              < 0.10     &&
	// -- dxy should be the same as dB
	// ..will have to test 0.05 and 0.02 if 0.2 doesn't work well..


	passId     = patUtils::passId(electron, goodPV, el_ID,      patUtils::CutVersion::Moriond17Cut) && passSigma && passImpactParameter;
	passVetoId = patUtils::passId(electron, goodPV, veto_el_ID, patUtils::CutVersion::Moriond17Cut) && passSigmaVeto && passImpactParameterVeto;

	// ------------------------- electron isolation

	passIso     = patUtils::passIso(electron, el_ISO,      patUtils::CutVersion::Moriond17Cut, rho);
	passVetoIso = patUtils::passIso(electron, veto_el_ISO, patUtils::CutVersion::Moriond17Cut, rho);


	// ---------------------------- Electron Kinematics
	//double leta(fabs(lid==11 ? lepton.el.superCluster()->eta() : lepton.eta()));
	double leta( electron.superCluster()->eta() );

	// ---------------------- Main lepton kin
	if(electron.pt() < pt_cut)                 passKin = false;
	if(leta > eta_cut)                         passKin = false;
	// also barrel-endcap window (these guys do it: https://gitlab.cern.ch/ttH/reference/blob/master/definitions/Moriond17.md#22-electron)
	if(leta > 1.4442 && leta < 1.5660)     passKin = false; // Crack veto

	// ---------------------- Veto lepton kin
	if (electron.pt () < veto_pt_cut)            passVetoKin = false;
	if (leta > veto_eta_cut)                     passVetoKin = false;
	if (leta > 1.4442 && leta < 1.5660) passVetoKin = false; // Crack veto


	if (passKin     && passId     && passIso)
		{
		selElectrons.push_back(electron);
		if (record)
			{
			fill_2d(string("control_el_selElectrons_pt_eta"), 250, 0., 500., 200, -3., 3., electron.pt(), electron.eta(), weight);
			fill_1d(string("control_el_selElectrons_phi"), 128, -3.2, 3.2, electron.phi(), weight);
			}
		}
	else if(passVetoKin && passVetoId && passVetoIso) nVetoE++;

	}

// TODO: there should be no need to sort selected electrons here again -- they are in order of Pt
std::sort(selElectrons.begin(),   selElectrons.end(),   utils::sort_CandidatesByPt);
// std::sort(selLeptons.begin(),   selLeptons.end(),   utils::sort_CandidatesByPt);
// std::sort(selLeptons_nocor.begin(),   selLeptons_nocor.end(),   utils::sort_CandidatesByPt);




return 0;
}


