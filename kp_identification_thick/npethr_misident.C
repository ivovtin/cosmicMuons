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
string progname;

int Usage(string status)
{
	cout<<"Usage: "<<progname<<"\t"<<" 2014 ( or 2015, 2016 ...)"<<endl;
        exit(0);
}

int main(int argc, char* argv[])
{
    int key;
    float Npethr;

    progname=argv[0];
    if( argc>1 )
    {
	key=atoi(argv[1]);
	if( key>2020 ){ Usage(progname); return 0;}
	if( key<2014 ){ Usage(progname); return 0;}
    }
    else
    {
	Usage(progname);
    }


    float npetrh;
    float eff_pion, eff_kaon, not_eff_pion, not_eff_kaon, Ksigma;

    TFile *fout=0;
    TString dir_in="/spool/users/ovtin/cosmruns/results";
    TString result= dir_in + "/" + TString::Format("thick_results_%d",key).Data();
    gSystem->Exec("mkdir "+ result);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ result);
    gSystem->Exec("ln -s "+ result + " /home/ovtin/public_html/thick_cnt/"+ TString::Format("thick_results_%d",key).Data());
    TString fname = result + "/" + TString::Format("cnt_thick_misindentification_allcnt_%d.root",key).Data();
    fout=new TFile(fname,"RECREATE");
    cout<<fname<<endl;

    TProfile* pr1[80];
    TProfile* pr2[80];
    TProfile* pr3[80];
    char name1[80];
    char name2[80];
    char name3[80];
    TLegend* legend[80];
    for (int j=0; j<80; j++)
    {
	sprintf(name1,"Cnt%d_k",j);
	sprintf(name2,"Cnt%d_pi",j);
	sprintf(name3,"Cnt%d",j);
	pr1[j] = new TProfile(name1,"Misidentification",100,0,5,0,5);
	pr2[j] = new TProfile(name2,"Misidentification",100,0,5,0,5);
	pr3[j] = new TProfile(name3,"#sigma",100,0,6.5,0,5);
        legend[j] = new TLegend(0.80,0.75,0.90,0.9);
    }

    for (int cnt=0; cnt<80; cnt++)
    {
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
			    pr1[cnt]->Fill(npetrh,eff_kaon);
			    pr2[cnt]->Fill(npetrh,not_eff_pion);
			    pr3[cnt]->Fill(npetrh,Ksigma);
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

    TCanvas c("c","c",1200,600);
    c.Divide(2,1);

    for( int i=0; i<80; i++ )
    {
        c.cd(1);
        gPad->SetGrid();
	pr1[i]->SetMarkerStyle(20);
	pr1[i]->SetMarkerColor(3);
	pr1[i]->SetLineWidth(2);
	pr1[i]->SetLineColor(3);
	pr1[i]->SetTitle("; Threshold, N_{ph.e.}; Misidentification");
	pr1[i]->GetXaxis()->SetTitleOffset(1.2);
	pr1[i]->GetYaxis()->SetTitleOffset(1.6);
	pr1[i]->GetYaxis()->SetRangeUser(0,0.50);

	pr2[i]->SetMarkerStyle(20);
	pr2[i]->SetMarkerColor(4);
	pr2[i]->SetLineWidth(2);
	pr2[i]->SetLineColor(4);

	pr1[i]->Draw("prof");
	pr2[i]->Draw("same");

	sprintf(name1,"Cnt%d_k",i);
	sprintf(name2,"Cnt%d_pi",i);
	legend[i]->AddEntry(name1,"K","lep");
	legend[i]->AddEntry(name2,"#pi ","lep");
	legend[i]->Draw("same");

        c.cd(2);
        gPad->SetGrid();
    	pr3[i]->SetMarkerStyle(20);
	pr3[i]->SetMarkerColor(4);
	pr3[i]->SetLineWidth(2);
	pr3[i]->SetLineColor(4);
	pr3[i]->SetTitle("; Threshold, N_{ph.e.}; #sigma");
	pr3[i]->GetXaxis()->SetTitleOffset(1.2);
	pr3[i]->GetYaxis()->SetTitleOffset(1.2);
	pr3[i]->Draw("prof");

        c.Update();
	c.Print(result + "/" + TString::Format("cnt_%d.png",i).Data());
    }
    fout->Write();
    fout->Close();
}
