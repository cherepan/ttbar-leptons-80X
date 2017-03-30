#include <iostream>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "THStack.h"
#include "TH1F.h"                                                                                                                                       
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TEventList.h"
#include "TROOT.h"
#include "TNtuple.h"
#include <Math/VectorUtil.h>

#include <map>
#include <string>
#include <vector>

#include "dtag_xsecs.h"

#define INPUT_DTAGS_START 6

using namespace std;



//histo_project3toN_ratio_distr.cc
//int stacked_histo_distr (int argc, char *argv[])
int main (int argc, char *argv[])
{
if (argc < 5)
	{
	//std::cout << "Usage : " << argv[0] << " x|y|z proj_name distr1 distr2 rebin_factor x_axis_min_range x_axis_max_range filename" << std::endl;
	std::cout << "Usage : " << argv[0] << " x|y|z proj_name content_name distr1 rebin_factor x_axis_min_range x_axis_max_range filename" << std::endl;
	exit (0);
	}

gROOT->Reset();

TString proj(argv[1]);
if (proj != TString("x") && proj != TString("y") && proj != TString("z"))
	{
	printf("UNKNOWN PROJECTION GIVEN\n");
	printf("supported: x, y, z\n");
	return 2;
	}

TString proj_name(argv[2]);
TString cont_name(argv[3]);
TString distr1(argv[4]);
//TString distr2(argv[4]);
Int_t rebin_factor(atoi(argv[5]));
double x_axis_min_range = atof(argv[6]);
double x_axis_max_range = atof(argv[7]);
TString filename(argv[8]);

cout << distr1 << endl;
//cout << distr2 << endl;
cout << "x axis minimum limit = " << x_axis_min_range << endl; // apparently, it doesn't work histogram's X axis -- you cannot zoom in within the histogram
cout << "x axis maximum limit = " << x_axis_max_range << endl; // apparently, it doesn't work histogram's X axis -- you cannot zoom in within the histogram
cout << filename   << endl;

TFile * file = TFile::Open(filename);
TH1D * h1 = (TH1D*) ((TH3D*) file->Get(distr1))->Project3D(proj);
//TH1D * h2 = (TH1D*) ((TH3D*) file->Get(distr2))->Project3D(proj);
h1->Sumw2();
//h2->Sumw2();

TCanvas *cst = new TCanvas("cst","stacked hists",10,10,700,700);
//TLegend* leg = new TLegend(0.845, 0.5, 0.99, 0.99);

TH1D *h3 = (TH1D*)h1->Clone("h3");
h3->SetLineColor(kBlack);
//h3->SetMinimum(0.8);  // Define Y ..
//h3->SetMaximum(1.35); // .. range
h3->Sumw2();
h3->SetStats(0);      // No statistics on lower plot
//h3->Divide(h2);
h3->SetMarkerStyle(21);

// normalize h3 for bin width
for (Int_t i=0; i<=h3->GetSize(); i++)
	{
	//yAxis->GetBinLowEdge(3)
	double content = h3->GetBinContent(i);
	double width   = h3->GetXaxis()->GetBinUpEdge(i) - h3->GetXaxis()->GetBinLowEdge(i);
	h3->SetBinContent(i, content/width);
	}

h3->SetTitle(distr1);
h3->SetXTitle(proj_name);
h3->SetYTitle(cont_name);

h3->Draw("ep");       // Draw the ratio plot

//h3->GetYaxis()->SetRange(0.0001, 1); // ranges from analysis note CMS AN-2012/489
//h3->GetYaxis()->SetRangeUser(0.0001, 1); // ranges from analysis note CMS AN-2012/489
h3->GetXaxis()->SetRange(x_axis_min_range, x_axis_max_range);
h3->GetXaxis()->SetRangeUser(x_axis_min_range, x_axis_max_range);

cout << "jet distr:" << endl;
h3->Print();

TFile f( filename.ReplaceAll(".root","") + "_" + distr1 + "_" + proj + ".root", "create" );
h3->Write();
f.Write();
f.Close();


h3->Draw("ep");       // Draw the ratio plot

cst->SetLogy();
//leg->Draw();

//MC_stack_124->GetXaxis()->SetTitle("#rho");
//mcrelunc929->GetYaxis()->SetTitle("Data/#Sigma MC");

cst->Update();

//hs->GetXaxis()->SetTitle(distr);
//hs_data->SetXTitle(distr);

cst->Modified();

cst->SaveAs( filename.ReplaceAll(".root","") + "_" + distr1 + "_" + proj + ".png" );

return 0;
}
