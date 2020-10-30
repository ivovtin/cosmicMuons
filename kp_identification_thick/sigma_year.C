#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <vector>
#include <algorithm>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <Riostream.h>
#include <sstream>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>
#include <TTree.h>
#include <TF1.h>
#include <TMath.h>
#include "TFile.h"
#include "TVirtualPad.h"
#include <TSystem.h>
#include <iomanip>
#pragma hdrstop
#include<stdio.h>
#include<stdlib.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TStyle.h>
#include <TLine.h>
#include <TEventList.h>
#include <TProfile.h>
#include <TLegend.h>
#include <vector>
#include <TChain.h>
#include "TMinuit.h"
#include "TRandom3.h"
#include <math.h>
#include "TVirtualFitter.h"
#include <algorithm>
#include <assert.h>

using namespace std;

int main(int argc, char* argv[])
{
    int key;
    float Npethr;

    float npetrh;
    float eff_pion, eff_kaon, not_eff_pion, not_eff_kaon, Ksigma;

    TString dir_in="/spool/users/ovtin/cosmruns/results";

    TString result= dir_in + "/" + "thick_results_allyears";
    gSystem->Exec("mkdir "+ result);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ result);
    gSystem->Exec("ln -s "+ result + " /home/ovtin/public_html/atc_cosmic/thick_cnt/thick_results_allyears");

    for (int i=1; i<=12; i++)
    {
	if(i==1) Npethr=0.1;
	if(i==2) Npethr=0.3;
	if(i==3) Npethr=0.5;
	if(i==4) Npethr=0.7;
	if(i==5) Npethr=1.0;
	if(i==6) Npethr=1.5;
	if(i==7) Npethr=2.0;
	if(i==8) Npethr=2.5;
	if(i==9) Npethr=3.0;
	if(i==10) Npethr=3.5;
	if(i==11) Npethr=4.0;
	if(i==12) Npethr=4.5;

	TFile *fout=0;
	TString fname = result + "/" + TString::Format("cnt_thick_sigma_years_%f.root",Npethr).Data();
	fout=new TFile(fname,"RECREATE");
	cout<<fname<<endl;

	TProfile* pr1[80];
	char name1[80];
	for (int j=0; j<80; j++)
	{
	    sprintf(name1,"Cnt%d",j);
	    pr1[j] = new TProfile(name1,"#sigma",100,2013,2021,0,5);
	}

	for (int cnt=0; cnt<80; cnt++)
	{
	    for (int key=2014; key<=2020; key++)
	    {

		TString KEDR = dir_in + "/" + TString::Format("kp_identification_thick_%d_%f",key,Npethr).Data();

		for (int ii=9; ii<=14; ii++)
		{
		    TString fin = KEDR + "/" + TString::Format("cnt_thick_%d_npetrh%f_eff_ineff_p%d.dat",cnt,Npethr,ii).Data();
		    //cout<<fin<<endl;
		    string line;
		    ifstream in(fin);
		    if (in.is_open())
		    {
			while (getline(in, line))
			{
			    if( line.size()>=1 && line[0]!='#' )
			    {
				std::istringstream i_str(line);
				i_str >> npetrh >> eff_pion >> eff_kaon >> not_eff_pion >> not_eff_kaon >> Ksigma;
				pr1[cnt]->Fill(key,Ksigma);
			    }
			}
		    }
		    else {
			cout << "Can't open input text file !"<<endl;
		    }
		    in.close();
		}

	    }
	}
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0000);

	TCanvas c("c","c",600,600);

	for( int i=0; i<80; i++ )
	{
	    gPad->SetGrid();
	    pr1[i]->SetMarkerStyle(20);
	    pr1[i]->SetMarkerColor(4);
	    pr1[i]->SetLineWidth(2);
	    pr1[i]->SetLineColor(4);
	    pr1[i]->SetTitle("; Year; #sigma");
	    pr1[i]->GetXaxis()->SetTitleOffset(1.2);
	    pr1[i]->GetYaxis()->SetTitleOffset(1.2);
	    pr1[i]->Draw("prof");

	    c.Update();
	    c.Print(result + "/" + TString::Format("npethr%f_cnt%d.png",Npethr,i).Data());
	}
	fout->Write();
	fout->Close();
    }
}
