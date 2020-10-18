#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <vector>
#include <algorithm>
#include <TMultiGraph.h>
#include <TGraphErrors.h>

#include "KDB/kdb.h"

#include "kp_cosmic.h"

using namespace std;
string progname;

inline char* timestamp(time_t t)
{
    static const char* time_fmt="%m %d %Y %H %M %S";
    static char strtime[50];

    struct tm* brtm=localtime(&t);
    strftime(strtime,50,time_fmt,brtm);

    return strtime;
}

int Usage(string status)
{
	cout<<"Usage: "<<progname<<"\t"<<" 0,1,... "<<endl;
        exit(0);
}

int main(int argc, char* argv[])
{
    progname=argv[0];
    if( argc>1 )
    {
	key=atoi(argv[1]);
	if( key>6 ){ Usage(progname); return 0;}
	if( key<0 ){ Usage(progname); return 0;}
    }
    else
    {
	Usage(progname);
    }

    //for print DEBUG information
    bool verbose=0;

    //***************preselections*************
    int min_munhits=2;
    float min_p=50.; //MeV
    float max_p=10000.; //MeV
    float max_chi2=100.;
    float min_nhits=20.;
    float Npethr=0.1;
    //****************************************

    TFile *fout=0;
    TString fnameout;
    TString KEDR;
    TString dir_out="/spool/users/ovtin/cosmruns/results";
    TString fout_result=dir_out + "/" + "fout_result.dat";
    fnameout=dir_out + "/" + "kp_ident_cosm.root";
    KEDR = "/home/ovtin/public_html/kp_identification/";
    fout_result=dir_out + "/" + "out_cosm.dat";

    cout<<fnameout<<endl;
    fout = new TFile(fnameout,"RECREATE");

    //include samples
    chain();
    Int_t nentr=tt->GetEntries();
    if(verbose) cout<<"Nentries="<<nentr<<endl;

    setbranchstatus();
    setbranchaddress();

    float Pkaon,Ppion, mkaon=493.66, mpion=139.57, mmuon=105.65;

    char name1[160],name2[160],name3[160],name4[160],name5[160],name6[160],name7[160];
    TH1F* h1[160];
    TH1F* h2[160];
    TH1F* h3[160];
    TH1F* h4[160];
    TProfile* pr1[160];
    TProfile* pr2[160];
    TProfile* pr3[160];
    for (int j=0; j<160; j++)
    {
	sprintf(name1,"h1_mom_cnt%d",j);
	h1[j]=new TH1F(name1,"Cosmic muon momentum",200,0,3000);
	sprintf(name2,"h2_npe_cnt%d",j);
        h2[j]=new TH1F(name2,"<<Kaons>>",500,0,50);
	sprintf(name3,"h3_npe_cnt%d",j);
	h3[j]=new TH1F(name3,"<<#pi>>",500,0,50);
	sprintf(name4,"h4_etop_cnt%d",j);
        h4[j]=new TH1F(name4,"e/p",200,0,2);
	sprintf(name5,"pr1_#mu_cnt%d",j);
	pr1[j]=new TProfile(name5,"#mu npe/Momentum",200,0,3000,0,50);
	sprintf(name6,"pr2_#pi_cnt%d",j);
	pr2[j]=new TProfile(name6,"#pi and K npe/Momentum",200,0,3000,0,50);
	sprintf(name7,"pr3_#K_cnt%d",j);
	pr3[j]=new TProfile(name7,"#K npe/Momentum",200,0,3000,0,50);
    }


    float p_all[160][20];
    float err_p_all[160][20];

    float eff_mu[160][20];
    float not_eff_mu[160][20];
    float err_eff_mu[160][20];
    double N_npenotzero_mu[160][20];
    double N_npetotal_mu[160][20];

    float eff_p[160][20];
    float not_eff_p[160][20];
    float err_eff_p[160][20];
    double N_npenotzero_p[160][20];
    double N_npetotal_p[160][20];

    float eff_k[160][20];
    float not_eff_k[160][20];
    float err_eff_k[160][20];
    double N_npenotzero_k[160][20];
    double N_npetotal_k[160][20];

    float Ksigma[160][20];

    //event loop
    for(int k=0; k<nentr; k++)
    {
	tt->GetEntry(k);
	if( (k %100000)==0 )cout<<k<<endl;

	if ( verbose ) cout<<"P="<<bcosm.P<<"\t"<<"Natc_cross="<<bcosm.natc_cr<<endl;

	if( bcosm.P>min_p && bcosm.P<max_p && bcosm.chi2<max_chi2 && bcosm.nhits>min_nhits )
	{
	    Pkaon=bcosm.P*(mkaon/mmuon);
	    Ppion=bcosm.P*(mpion/mmuon);

	    for(int i=0; i<bcosm.natc_cr; i++)
	    {
		if ( verbose ) cout<<"cnt"<<bcosm.cnt[i]<<"\t"<<"npe="<<bcosm.npe[i]<<endl;
		for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.aerogel_REGION[i]==1 )
		{
		    h1[j]->Fill(bcosm.P);

		    if(bcosm.P>200 && bcosm.P<300){
			h2[j]->Fill(bcosm.npe[i]);
		    }

		    if(bcosm.P>700 && bcosm.P<1100){
			h3[j]->Fill(bcosm.npe[i]);
		    }

		    h4[j]->Fill(bcosm.emcenergy/bcosm.P);

		    pr1[j]->Fill(bcosm.P,bcosm.npe[i]);
		    pr2[j]->Fill(Ppion,bcosm.npe[i]);
		    pr3[j]->Fill(Pkaon,bcosm.npe[i]);

		    for(int ii=0; ii<=20-1; ii++)
		    {
			p_all[j][ii]=100*ii+50;
			err_p_all[j][ii]=50;

			if(bcosm.P>100*ii&&bcosm.P<100+100*ii){
			    if(bcosm.npe[i]>Npethr)
			    {
				N_npenotzero_mu[j][ii]=++N_npenotzero_mu[j][ii];
			    }
			    N_npetotal_mu[j][ii]=++N_npetotal_mu[j][ii];
			    eff_mu[j][ii]=N_npenotzero_mu[j][ii]/N_npetotal_mu[j][ii];
			    err_eff_mu[j][ii]=sqrt(N_npenotzero_mu[j][ii])/N_npetotal_mu[j][ii];
			    not_eff_mu[j][ii]=1-eff_mu[j][ii];
			}
			if(Ppion>100*ii&&Ppion<100+100*ii){
			    if(bcosm.npe[i]>Npethr)
			    {
				N_npenotzero_p[j][ii]=++N_npenotzero_p[j][ii];
			    }
			    N_npetotal_p[j][ii]=++N_npetotal_p[j][ii];
			    eff_p[j][ii]=N_npenotzero_p[j][ii]/N_npetotal_p[j][ii];
			    err_eff_p[j][ii]=sqrt(N_npenotzero_p[j][ii])/N_npetotal_p[j][ii];
			    not_eff_p[j][ii]=1-eff_p[j][ii];
			}
			if(Pkaon>100*ii&&Pkaon<100+100*ii){
			    if(bcosm.npe[i]>Npethr)
			    {
				N_npenotzero_k[j][ii]=++N_npenotzero_k[j][ii];
			    }
			    N_npetotal_k[j][ii]=++N_npetotal_k[j][ii];
			    eff_k[j][ii]=N_npenotzero_k[j][ii]/N_npetotal_k[j][ii];
			    err_eff_k[j][ii]=sqrt(N_npenotzero_k[j][ii])/N_npetotal_k[j][ii];
			    not_eff_k[j][ii]=1-eff_k[j][ii];
			}

                        Ksigma[j][ii]=0;
			Ksigma[j][ii]=sqrt(2.)*pow(TMath::Erf(-1+2*(1-eff_k[j][ii])),-1)+pow(TMath::Erf(-1+2*eff_p[j][ii]),-1);
                        if ( Ksigma[j][ii]<0 || Ksigma[j][ii]>10 ) Ksigma[j][ii]=0;
			if ( verbose ) cout<<"#sigma="<<Ksigma[j][ii]<<endl;
		    }

		}
	    }
	}

    }

    gStyle->SetPalette(1);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(1011);


    TCanvas c("c","c",1600,1200);
    c.Divide(4,3);

    char namefit1[160],namefit2[160],namefit3[160];
    TF1* myfit1[160];
    TF1* myfit2[160];
    TF1* myfit3[160];

    TGraphErrors* gr1[160];
    TGraphErrors* gr2[160];
    TGraphErrors* gr3[160];
    TGraphErrors* gr4[160];
    TGraphErrors* gr5[160];
    TGraphErrors* gr6[160];
    TMultiGraph* mg1[160];
    TMultiGraph* mg2[160];

    for(int i=0; i<160; i++)
    {
        c.cd(1);
	h1[i]->GetXaxis()->SetTitle("N_{ph.e.}");
	h1[i]->SetTitle("Momentum");
        h1[i]->SetXTitle("P, MeV/c");
	h1[i]->Draw();
        TLine *l1=new TLine(200,0,200,4500);
	l1->SetLineColor(kRed);
	l1->Draw();
	TLine *l2=new TLine(300,0,300,4500);
	l2->SetLineColor(kRed);
	l2->Draw();
	TLine *l3=new TLine(700,0,700,4500);
	l3->SetLineColor(kBlue);
	l3->Draw();
	TLine *l4=new TLine(1100,0,1100,4500);
	l4->SetLineColor(kBlue);
	l4->Draw();

	c.cd(2);
	gPad->SetLogy();
	h2[i]->SetLineColor(kRed);
	h2[i]->Draw("hist");

        c.cd(3);
	h3[i]->Draw("hist");

	c.cd(4);
	h4[i]->Draw("hist");

	c.cd(5);
        sprintf(namefit1,"myfit1",i);
        myfit1[i]=new TF1(namefit1,"[0]+[1]*(x^2-[2]^2)/(x^2)*(TMath::Erf(x-[2])+1)/2",200.,3000.);
	myfit1[i]->SetLineColor(kBlue);
	myfit1[i]->SetParameter(0,pr1[i]->GetMinimum());
	myfit1[i]->SetParameter(1,pr1[i]->GetMaximum());
	myfit1[i]->SetParameter(2,200);
	myfit1[i]->SetParNames("#mu_{0}","#mu_{max}","P_{thr}");
	pr1[i]->Fit(namefit1,"","",200,3000);
	pr1[i]->SetXTitle("P, MeV/c");
	pr1[i]->SetYTitle("N_{ph.e.}");
	gStyle->SetOptFit(1011);
	pr1[i]->Draw("prof");

	c.cd(6);
        sprintf(namefit2,"myfit2",i);
        myfit2[i]=new TF1(namefit2,"[0]+[1]*(x^2-[2]^2)/(x^2)*(TMath::Erf(x-[2])+1)/2",200.,3000.);
	myfit2[i]->SetLineColor(kBlue);
	myfit2[i]->SetParameter(0,pr2[i]->GetMinimum());
	myfit2[i]->SetParameter(1,pr2[i]->GetMaximum());
	myfit2[i]->SetParameter(2,200);
	myfit2[i]->SetParNames("#mu_{0}","#mu_{max}","P_{thr}");
	pr2[i]->Fit(namefit2,"","",200,3000);
	pr2[i]->SetXTitle("P, MeV/c");
	pr2[i]->SetYTitle("N_{ph.e.}");
	gStyle->SetOptFit(1011);

	sprintf(namefit3,"myfit3",i);
	myfit3[i]=new TF1(namefit3,"[0]+[1]*(x^2-[2]^2)/(x^2)*(TMath::Erf(x-[2])+1)/2",520.,3000.);
	myfit3[i]->SetLineColor(kRed);
	myfit3[i]->SetParameter(0,pr3[i]->GetMinimum());
	myfit3[i]->SetParameter(1,pr3[i]->GetMaximum());
	myfit3[i]->SetParameter(2,200);
	myfit3[i]->SetParNames("#mu_{0}","#mu_{max}","P_{thr}");
	pr3[i]->Fit(namefit3,"","",520,3000);
        pr3[i]->SetLineColor(kRed);
	pr3[i]->SetXTitle("P, MeV/c");
	pr3[i]->SetYTitle("N_{ph.e.}");
	gStyle->SetOptFit(1011);
	pr2[i]->Draw("prof");
	pr3[i]->Draw("same");

	c.cd(7);
        gPad->SetGrid();
	mg1[i] = new TMultiGraph();
	gr1[i]=new TGraphErrors(20,p_all[i],eff_mu[i],err_p_all[i],err_eff_mu[i]);
	gr1[i]->SetMarkerStyle(20);
	gr1[i]->SetMarkerColor(3);
	gr1[i]->SetLineWidth(2);
	gr1[i]->SetLineColor(3);
	gr1[i]->SetTitle("Efficiency&Momentum");
	gr1[i]->GetXaxis()->SetTitle("P, MeV/c");
	gr1[i]->GetYaxis()->SetTitle("Efficiency");

	gr2[i]=new TGraphErrors(20,p_all[i],eff_p[i],err_p_all[i],err_eff_p[i]);
	gr2[i]->SetMarkerStyle(20);
	gr2[i]->SetMarkerColor(4);
	gr2[i]->SetLineWidth(2);
	gr2[i]->SetLineColor(4);
	gr2[i]->SetTitle("Efficiency&Momentum");
	gr2[i]->GetXaxis()->SetTitle("P, MeV/c");
	gr2[i]->GetYaxis()->SetTitle("Efficiency");

	gr3[i]=new TGraphErrors(20,p_all[i],eff_k[i],err_p_all[i],err_eff_k[i]);
	gr3[i]->SetMarkerStyle(20);
	gr3[i]->SetMarkerColor(2);
	gr3[i]->SetLineWidth(2);
	gr3[i]->SetLineColor(2);
	gr3[i]->SetTitle("Efficiency&Momentum");
	gr3[i]->GetXaxis()->SetTitle("P, MeV/c");
	gr3[i]->GetYaxis()->SetTitle("Efficiency");

        mg1[i]->Add(gr1[i]);
	mg1[i]->Add(gr2[i]);
	mg1[i]->Add(gr3[i]);
	mg1[i]->SetTitle("Efficiency&Momentum; P, MeV/c; Efficiency");
	mg1[i]->Draw("ap");

	c.cd(8);
        gPad->SetGrid();
	mg2[i] = new TMultiGraph();
	gr4[i]=new TGraphErrors(20,p_all[i],not_eff_p[i],err_p_all[i],0);
	gr4[i]->SetMarkerStyle(20);
	gr4[i]->SetMarkerColor(4);
	gr4[i]->SetLineWidth(2);
	gr4[i]->SetLineColor(4);

	gr5[i]=new TGraphErrors(20,p_all[i],not_eff_k[i],err_p_all[i],0);
	gr5[i]->SetMarkerStyle(20);
	gr5[i]->SetMarkerColor(2);
	gr5[i]->SetLineWidth(2);
	gr5[i]->SetLineColor(2);

	mg2[i]->Add(gr4[i]);
	mg2[i]->Add(gr5[i]);
	mg2[i]->SetTitle("K efficiency and #pi Misidentification; P, MeV/c; K efficiency and #pi Misidentification");
	mg2[i]->Draw("ap");
        TLine *l5=new TLine(650,0,650,1);
	l5->SetLineColor(kRed);
	l5->Draw();
	TLine *l6=new TLine(1500,0,1500,1);
	l6->SetLineColor(kRed);
	l6->Draw();

	c.cd(9);
	gPad->SetGrid();
        gr6[i]=new TGraphErrors(20,p_all[i],Ksigma[i],err_p_all[i],0);
	gr6[i]->SetMarkerStyle(20);
	gr6[i]->SetMarkerColor(3);
	gr6[i]->SetLineWidth(2);
	gr6[i]->SetLineColor(3);
	gr6[i]->SetTitle("#sigma");
	TAxis *axis3 = gr6[i]->GetYaxis();
	axis3->SetLimits(0.,10.);
        gr6[i]->Draw("ap");

	c.Update();
	c.Print(KEDR+TString::Format("cnt_%d.png",i).Data());
    }

    fout->Write();
    fout->Close();

}
