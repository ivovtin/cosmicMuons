#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <vector>
#include <algorithm>
#include <TMultiGraph.h>
#include <TGraphErrors.h>

#include "KDB/kdb.h"

#include "cosmic.h"

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

int main(int argc, char* argv[])
{
    //for print DEBUG information
    bool verbose=0;

    //***************preselections*************
    float min_p=1000.; //MeV
    float max_p=10000.; //MeV
    float max_chi2=100.;
    float min_nhits=30.;
    //****************************************

    TFile *fout=0;
    TString KEDR;
    TString dir_out="/spool/users/ovtin/cosmruns/results";
    KEDR = dir_out + "/" + "cosm_stability";
    gSystem->Exec("mkdir "+ KEDR);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ KEDR);
    gSystem->Exec("ln -s "+ KEDR + " /home/ovtin/public_html/cosmic");
    TString fnameout;
    fnameout=KEDR + "/" + "exp_cosm.root";

    cout<<fnameout<<endl;
    fout = new TFile(fnameout,"RECREATE");

    //include samples
    chain();
    Int_t nentr=tt->GetEntries();
    if(verbose) cout<<"Nentries="<<nentr<<endl;

    setbranchstatus();
    setbranchaddress();

    time_t runTime_begin; //start time of the current run
    time_t runTime_end; //end time of the current run

    char name0[161];
    char name1[161];
    TProfile* pr[160];
    TProfile* pr1[160];
    for (int j=0; j<160; j++)
    {
	sprintf(name0,"Cnt%d",j);
	sprintf(name1,"Run_Cnt%d",j);
	pr[j]=new TProfile(name0,name0,1000,1400778000,1603714452,0,200);
	pr1[j]=new TProfile(name1,name1,9969,19635,29604,0,200);
    }

    int runprev=0;
    int NumRun=0;

    //event loop
    for(int k=0; k<nentr; k++)
    {
	if(k>0) { tt->GetEntry(k-1); runprev=bcosm.run; }

	tt->GetEntry(k);
	if( (k %100000)==0 )cout<<k<<endl;

	//skip runs
        if( bcosm.run==19770 || bcosm.run==19771 || bcosm.run==19772 || bcosm.run==19774 || bcosm.run==19776 ) continue;  //2014
        if( bcosm.run==19779 || bcosm.run==19794 || bcosm.run==19796 || bcosm.run==19797 || bcosm.run==19654 ) continue;  //2014
        if( bcosm.run==19655 || bcosm.run==20143 ) continue;  //2014
	if( bcosm.run==21817 || bcosm.run==21818 || bcosm.run==21819 || bcosm.run==21820 || bcosm.run==21821 ) continue;  //2015
	if( bcosm.run==23716 || bcosm.run==23720 || bcosm.run==23907 ) continue;                       //2016
	if( bcosm.run>24087 && bcosm.run<24177 ) continue;                                             //2016
	if( bcosm.run>25750 && bcosm.run<25850 ) continue;                                             //2017
	if( bcosm.run==25252 || bcosm.run==25253 || bcosm.run==25254 || bcosm.run==25255 ) continue;   //2017
	if( bcosm.run==25256 || bcosm.run==25257 || bcosm.run==25258 || bcosm.run==25259 ) continue;   //2017
	if( bcosm.run==25451 || bcosm.run==25441 || bcosm.run==25576 || bcosm.run==25577 || bcosm.run==25578 ) continue;   //2017
	if( bcosm.run==25580 || bcosm.run==25581 || bcosm.run==26337 || bcosm.run==26338 || bcosm.run==26349 ) continue;   //2017
	if( bcosm.run==27016 || bcosm.run==27293 || bcosm.run==27642 || bcosm.run==27643 ) continue;   //2018
	if( bcosm.run==27760 || bcosm.run==27761 || bcosm.run==27857 || bcosm.run==27861 ) continue;   //2018
	if( bcosm.run==27922 || bcosm.run==27923 || bcosm.run==27924 || bcosm.run==26341 ) continue;   //2018
	if( bcosm.run==28333 || bcosm.run==28334 || bcosm.run==28429 || bcosm.run==28430 || bcosm.run==28431 ) continue;   //2019
	if( bcosm.run==28438 || bcosm.run>28677 && bcosm.run<28745 ) continue;                                             //2019
	if( bcosm.run==28529 || bcosm.run==28754 || bcosm.run==28882 ) continue;                       //2019
	if( bcosm.run==29091 || bcosm.run==29092 || bcosm.run==29123 || bcosm.run==29127 ) continue;   //2020
        if( bcosm.run==29146 || bcosm.run==29161 || bcosm.run==29162 || bcosm.run==29165 ) continue;   //2020
	if( bcosm.run==29431 || bcosm.run==29432 || bcosm.run==29433 || bcosm.run==29480 ) continue;   //2020
	if( bcosm.run==29441 || bcosm.run==29442 || bcosm.run==29443 || bcosm.run==29445 ) continue;   //2020
	if( bcosm.run==29446 || bcosm.run==29447 || bcosm.run==29448 || bcosm.run==29449 ) continue;   //2020
	if( bcosm.run==29450 || bcosm.run==29451 || bcosm.run==29452 || bcosm.run==29455 || bcosm.run==29456 ) continue;   //2020
	if( bcosm.run==29457 || bcosm.run==29458 || bcosm.run==29459 || bcosm.run==29590 || bcosm.run==29591 ) continue;   //2020
        if( bcosm.run==29525 || bcosm.run==29595 || bcosm.run==29596 || bcosm.run==29597 ) continue;   //2020
	if( bcosm.run==29598 || bcosm.run==29599 || bcosm.run==29582 || bcosm.run==29589 || bcosm.run==29601 ) continue;   //2020

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

	    NumRun++;
            cout<<"NumRun="<<NumRun<<endl;
	}

	if ( verbose ) cout<<"P="<<bcosm.P<<"\t"<<"Natc_cross="<<bcosm.natc_cr<<endl;

	if( bcosm.P>min_p && bcosm.P<max_p  && bcosm.chi2<max_chi2 && bcosm.nhits>min_nhits && sqrt(pow(bcosm.Xip,2)+pow(bcosm.Yip,2)+pow(bcosm.Zip,2))<35 )
	{
	    for(int i=0; i<bcosm.natc_cr; i++)
	    {
		if ( verbose ) cout<<"cnt"<<bcosm.cnt[i]<<"\t"<<"npe="<<bcosm.npe[i]<<endl;
		for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.single_aerogel_REGION5[i]==1 && bcosm.wlshit[i]!=1 )
		{
		    pr[j]->Fill(runTime_begin,bcosm.npe[i]);
		    pr1[j]->Fill(bcosm.run,bcosm.npe[i]);
		}
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
        myfit[i] = new TF1(namefit,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
	myfit[i]->SetParLimits(0,0,10000);
	myfit[i]->SetParLimits(1,500000,1e12);
	myfit[i]->SetParLimits(2,0,10000);
	myfit[i]->SetParNames("p0","#tau","Const. level");
	myfit[i]->SetLineColor(kBlue);
	gStyle->SetOptFit(kTRUE);
	pr[i]->Fit(namefit,"","",1400778000,1603714452);
	pr[i]->SetMarkerStyle(20);
	pr[i]->SetMarkerSize(0.5);
	pr[i]->SetMarkerColor(1);
	pr[i]->SetLineWidth(2);
	pr[i]->SetLineColor(1);
	pr[i]->GetXaxis()->SetTimeDisplay(1);
	pr[i]->GetXaxis()->SetTimeFormat("%d/%m/%y%F1970-01-01 00:00:00");
	pr[i]->GetXaxis()->SetTitleSize(0.05);
	pr[i]->GetXaxis()->SetTitleOffset(1.0);
	pr[i]->GetXaxis()->SetLabelSize(0.05);
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
	c.Print(KEDR + "/" + TString::Format("cnt_%d.png",i).Data());
    }

    TCanvas c1("c1","c1",1400,800);
    c1.cd();
    for(int i=0; i<160; i++)
    {
	pr1[i]->SetMarkerStyle(20);
	pr1[i]->SetMarkerSize(0.5);
	pr1[i]->SetMarkerColor(1);
	pr1[i]->SetLineWidth(2);
	pr1[i]->SetLineColor(1);
	pr1[i]->GetXaxis()->SetTitleSize(0.05);
	pr1[i]->GetXaxis()->SetTitleOffset(1.0);
	pr1[i]->GetXaxis()->SetLabelSize(0.05);
	pr1[i]->GetXaxis()->SetNdivisions(510);
	pr1[i]->GetYaxis()->SetTitleSize(0.05);
	pr1[i]->GetYaxis()->SetTitleOffset(1.00);
	pr1[i]->GetYaxis()->SetLabelSize(0.05);
	pr1[i]->GetYaxis()->SetNdivisions(205);
	pr1[i]->GetYaxis()->SetDecimals();
	pr1[i]->GetXaxis()->SetTitle("Run");
	pr1[i]->GetYaxis()->SetTitle("N_{ph.e.}");
	pr1[i]->SetTitle("");
	pr1[i]->Draw();
	c1.Update();
	c1.Print(KEDR + "/" + TString::Format("runs_cnt_%d.png",i).Data());
    }

    fout->Write();
    fout->Close();

    cout<<"NumRun="<<NumRun<<endl;
}
