#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <vector>
#include <algorithm>

#include "KDB/kdb.h"

#include "cosmic.h"

using namespace std;

inline char* timestamp(time_t t)
{
    static const char* time_fmt="%m %d %Y %H %M %S";
    static char strtime[50];

    struct tm* brtm=localtime(&t);
    strftime(strtime,50,time_fmt,brtm);

    return strtime;
}

int main(int argc, char* argv[])
{
    //for print DEBUG information
    bool verbose=0;

    //***************preselections*************
    int min_munhits=2;
    float min_p=1000.; //MeV
    float max_p=10000.; //MeV
    float max_chi2=100.;
    float min_nhits=20.;
    //****************************************

    TFile *fout=0;
    TString fnameout;
    TString KEDR;
    TString dir_out="/spool/users/ovtin/cosmruns/results";
    TString fout_result=dir_out + "/" + "fout_result.dat";
    fnameout=dir_out + "/" + "exp_cosm.root";
    KEDR = "/home/ovtin/public_html/cosmic/";
    fout_result=dir_out + "/" + "out_cosm.dat";

    cout<<fnameout<<endl;
    fout = new TFile(fnameout,"RECREATE");

    //include samples
    chain();
    Int_t nentr=tt->GetEntries();
    if(verbose) cout<<"Nentries="<<nentr<<endl;

    setbranchstatus();
    setbranchaddress();

    time_t rawtime;
    time ( &rawtime );
    if( verbose ) cout << "time="<<  rawtime <<"\n"<<endl;

    time_t runTime_begin; //start time of the current run
    time_t runTime_end; //end time of the current run

    char name0[161];
    TProfile* pr[160];
    for (int j=0; j<160; j++)
    {
	sprintf(name0,"Cnt%d",j);
	pr[j]=new TProfile(name0,name0,200,1400778000,1601554829,0,200);
    }

    int runprev=0;

    //event loop
    for(int k=0; k<nentr; k++)
    {
	if(k>0) { tt->GetEntry(k-1); runprev=bcosm.run; }

	tt->GetEntry(k);
	if( (k %100000)==0 )cout<<k<<endl;

	if( k==0 || runprev!=bcosm.run ){
	    KDBconn* connection=kdb_open();
	    if( !connection ) {
		cerr<<"Can not establish connection to database"<<endl;
		return 1;
	    }

	    runTime_begin=kdb_run_get_begin_time(connection, bcosm.run);
	    runTime_end=kdb_run_get_end_time(connection, bcosm.run);
	    cout<<"\n========================================================================="<<endl;
	    cout<<"Check database...."<<endl;
	    cout<<"Begin time of Run"<< bcosm.run<<": "<<runTime_begin<<endl;
	    cout<<"Begin time of Run"<< bcosm.run<<": "<<timestamp(runTime_begin)<<endl;
	    cout<<"End time of Run"<< bcosm.run<<": "<<timestamp(runTime_end)<<endl;
	    kdb_close(connection);
	}

        //if(k>10) continue;

	if ( verbose ) cout<<"P="<<bcosm.P<<"\t"<<"Natc_cross="<<bcosm.natc_cr<<endl;

	if( bcosm.P>min_p && bcosm.P<max_p && bcosm.chi2<max_chi2 && bcosm.nhits>min_nhits )
	{
	    for(int i=0; i<bcosm.natc_cr; i++)
	    {
		if ( verbose ) cout<<"cnt"<<bcosm.cnt[i]<<"\t"<<"npe="<<bcosm.npe[i]<<endl;
		for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.aerogel_REGION0[i]==1 ) pr[j]->Fill(runTime_begin,bcosm.npe[i]);
	    }
	}

    }

    gStyle->SetPalette(1);
    gStyle->SetPadGridX(kTRUE);
    gStyle->SetPadGridY(kTRUE);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(1011);

    TCanvas c("c","c",1400,800);
    c.cd();
    char namefit[161];
    TF1* myfit[161];
    for(int i=0; i<160; i++)
    {
       	sprintf(namefit,"myfit",i);
        myfit[i] = new TF1(namefit,"[0]*exp(-(x-1400778000)/[1])",1400778000,1601554829);
	myfit[i]->SetParLimits(0,0,10000);
	myfit[i]->SetParLimits(1,500000,1e12);
	myfit[i]->SetLineColor(kBlue);
	pr[i]->Fit(namefit,"","",1400778000,1601554829);
	pr[i]->SetMarkerStyle(20);
	pr[i]->SetMarkerSize(0.5);
	pr[i]->SetMarkerColor(2);
	pr[i]->SetLineWidth(2);
	pr[i]->SetLineColor(2);
	pr[i]->GetXaxis()->SetTimeDisplay(1);
	pr[i]->GetXaxis()->SetTimeFormat("%d/%m/%y%F1970-01-01 00:00:00");
	pr[i]->GetXaxis()->SetTitleSize(0.05);
	pr[i]->GetXaxis()->SetTitleOffset(1.0);
	pr[i]->GetXaxis()->SetLabelSize(0.05);
	//pr[i]->GetXaxis()->SetNdivisions(205);
	pr[i]->GetXaxis()->SetNdivisions(510);
	pr[i]->GetYaxis()->SetTitleSize(0.05);
	pr[i]->GetYaxis()->SetTitleOffset(1.00);
	pr[i]->GetYaxis()->SetLabelSize(0.05);
	pr[i]->GetYaxis()->SetNdivisions(205);
	pr[i]->GetYaxis()->SetDecimals();
	pr[i]->GetXaxis()->SetTitle("Time(d/m/y)");
	pr[i]->GetYaxis()->SetTitle("N_{ph.e.}");
	pr[i]->SetTitle("");
	pr[i]->Draw();
	c.Update();
	c.Print(KEDR+TString::Format("cnt_%d.png",i).Data());
    }

    fout->Write();
    fout->Close();

}
