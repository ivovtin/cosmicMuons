#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <vector>
#include <algorithm>
#include <TMultiGraph.h>
#include <TGraphErrors.h>

#include "KDB/kdb.h"

#include "light_coefficient.h"

using namespace std;
string progname;

int Usage(string status)
{
   cout<<"Usage: "<<progname<<"\t"<<"Verbose->(0/1)  longStableATC->(0/1)  lightCoeffATC->(0/1)  Range with data->(1,2...)  Cnt type->(AY=1,AS=2,AR=3,AQ=4,EX=5)"<<endl;
   cout<<"For example: 1) ./light_coefficient 0 0 0 3 2 "<<endl;
   cout<<"             2) ./light_coefficient 1 0 1 3 2 "<<endl;
   cout<<"             3) ./light_coefficient 1 1 0 3 2 "<<endl;
   exit(0);
}

Double_t poissonf(Double_t*x,Double_t*par)
{
  return par[0]*TMath::Poisson(x[0],par[1]);
}

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
    progname=argv[0];

    if( argc>1 ) verbose=atoi(argv[1]);
    if( argc>2 ) longStableATC=atoi(argv[2]);
    if( argc>3 ) lightCoeffATC=atoi(argv[3]);
    if( argc>4 ) time_region=atoi(argv[4]);
    if( argc>5 ) cnt_type=atoi(argv[5]);
    if( argc>6 || cnt_type>5 || time_region>18 ){ Usage(progname); return 0;}

    bool fillFileForlightCoeffATC = 1;

    //***************preselections*************
    float min_p=1000.; //MeV
    float max_p=10000.; //MeV
    float max_chi2=50.;  //100.
    if( cnt_type==5 ) max_chi2=1000;
    float min_nhits=30.;
    float min_energy=0.;
    if( cnt_type==5 ) min_energy=0.;
    float min_ncls=0;
    if( cnt_type==5 ) min_ncls=0;
    float min_muhits=5;
    if( cnt_type==5 ) min_muhits=0;
    //****************************************

    gSystem->Exec("mkdir "+ KEDR);
    gSystem->Exec("cp " + indexFile + " "+ KEDR);
    gSystem->Exec("ln -s "+ KEDR + " " + publicLink);
    gSystem->Exec("ln -s "+ KEDR + " res");

    if( longStableATC )
    {
	fnameout=KEDR + "/" + "exp_cosm.root";
	fout = new TFile(fnameout,"RECREATE");
	if(verbose) cout<<fnameout<<endl;
    }

    atc_coef_forSim = KEDR + "/" + "tmp";
    //gSystem->Exec("rm -r " + atc_coef_forSim);
    gSystem->Exec("mkdir " + atc_coef_forSim);
    gSystem->Exec("ln -s "+ atc_coef_forSim + " tmp");

    //we have 5 type of counters
    if (cnt_type==1){ cnt_begin=20; cnt_end=40; size_cnt_type=58; NAME_str=TString::Format("data_AY_%02drange",time_region).Data();}    //20-40cnt - AY
    if (cnt_type==2){ cnt_begin=40; cnt_end=60; size_cnt_type=68; NAME_str=TString::Format("data_AS_%02drange",time_region).Data();}    //40-60cnt - AS
    if (cnt_type==3){ cnt_begin=100; cnt_end=120; size_cnt_type=68; NAME_str=TString::Format("data_AR_%02drange",time_region).Data();}  //100-120cnt - AR
    if (cnt_type==4){ cnt_begin=120; cnt_end=140; size_cnt_type=58; NAME_str=TString::Format("data_AQ_%02drange",time_region).Data();}  //120-140cnt - AQ
    if (cnt_type==5){ cnt_begin=0; cnt_end=160; size_cnt_type=28; NAME_str=TString::Format("data_EX_%02drange",time_region).Data();}    //EX

    if(verbose) cout<<"Time range is "<<time_region<<";"<<"\t"<<"Cnt type is "<<cnt_type<<";"<<"\t"<<"Size Cnt type is "<<size_cnt_type<<endl;

    //setup geometry sizes for areas of cnt type
    set_geometry_atc_cnts();

    for(int k=0; k<size_cnt_type; k++) if( lightCoeffATC && fillFileForlightCoeffATC )
    {
        gSystem->Exec("rm -r " + atc_coef_forSim + "/" + TString::Format("data_cnt_Klight_area_all_%dtime_region_%dcnt_type_%darea.dat",time_region,cnt_type,k).Data());
    }

    //include samples
    chain();
    setbranchstatus();
    setbranchaddress();

    Int_t nentr=tt->GetEntries();
    if(verbose) cout<<"Total number of events in trees is "<<nentr<<endl;

    if( longStableATC )
    {
	for (int j=0; j<160; j++)
	{
	    sprintf(name0,"Cnt%d",j);
	    sprintf(name1,"Run_Cnt%d",j);
	    pr[j]=new TProfile(name0,name0,1000,1400778000,1603714452,0,200);
	    pr1[j]=new TProfile(name1,name1,9969,19635,29604,0,200);
	}
    }

    //event loop
    for(int k=0; k<nentr; k++)
    {
	if(k>0) { tt->GetEntry(k-1); runprev=bcosm.run; }

	tt->GetEntry(k);
	if( (k %100000)==0  )cout<<"counter of events: "<<k<<endl;

	//skip bad runs
        if( bcosm.run==19727 || bcosm.run==19770 || bcosm.run==19771 || bcosm.run==19772 || bcosm.run==19774 ) continue;   //2014
        if( bcosm.run==19776 || bcosm.run==19779 || bcosm.run==19794 || bcosm.run==19796 || bcosm.run==19797 ) continue;   //2014
        if( bcosm.run==19654 || bcosm.run==19655 || bcosm.run==20143 || bcosm.run==20864 ) continue;                       //2014
	if( bcosm.run==21817 || bcosm.run==21818 || bcosm.run==21819 || bcosm.run==21820 || bcosm.run==21821 ) continue;   //2015
	if( bcosm.run==23716 || bcosm.run==23720 || bcosm.run==23907 ) continue;                                           //2016
	if( bcosm.run>24087 && bcosm.run<24177 ) continue;                                                                 //2016
	if( bcosm.run>25750 && bcosm.run<25850 ) continue;                                                                 //2017
	if( bcosm.run==24714 || bcosm.run==24715 || bcosm.run==24716 || bcosm.run==24717 || bcosm.run==24718 ) continue;   //2017
	if( bcosm.run==25050 || bcosm.run==25300 || bcosm.run==25451 || bcosm.run==25252 || bcosm.run==25253 ) continue;   //2017
	if( bcosm.run==25254 || bcosm.run==25255 || bcosm.run==25256 || bcosm.run==25257 || bcosm.run==25258 ) continue;   //2017
	if( bcosm.run==25259 || bcosm.run==25295 || bcosm.run==25441 || bcosm.run==25576 || bcosm.run==25577 ) continue;   //2017
	if( bcosm.run==25578 || bcosm.run==25580 || bcosm.run==25581 || bcosm.run==25868 || bcosm.run==25878 ) continue;   //2017
	if( bcosm.run==26270 || bcosm.run==26337 || bcosm.run==26338 || bcosm.run==26339 ) continue;                       //2017
	if( bcosm.run==26349 || bcosm.run==26862 || bcosm.run==27016 || bcosm.run==27293 || bcosm.run==27642 ) continue;   //2018
	if( bcosm.run==27643 || bcosm.run==27760 || bcosm.run==27761 || bcosm.run==27857 || bcosm.run==27861 ) continue;   //2018
	if( bcosm.run==27922 || bcosm.run==27923 || bcosm.run==27924 || bcosm.run==26341 ) continue;                       //2018
	if( bcosm.run==28060 || bcosm.run==28061 || bcosm.run==28090 || bcosm.run==28091 || bcosm.run==28111 ) continue;   //2019
	if( bcosm.run==28112 || bcosm.run==28191 || bcosm.run==28192 || bcosm.run==28334 || bcosm.run==28429 ) continue;   //2019
	if( bcosm.run==28438 || bcosm.run==28529 || bcosm.run==28639 || bcosm.run==28231 || bcosm.run==28430 ) continue;   //2019
	if( bcosm.run==28431 || bcosm.run==28547 || bcosm.run==28648 || bcosm.run==28674 || bcosm.run==28677 ) continue;   //2019
	if( bcosm.run==28708 || bcosm.run==28752 || bcosm.run==28778 || bcosm.run==28754 || bcosm.run==28814 ) continue;   //2019
	if( bcosm.run==28882 || bcosm.run==28893 || bcosm.run==28894 || bcosm.run==28956 || bcosm.run==29019 ) continue;   //2019
	if( bcosm.run==29020 || bcosm.run==29091 || bcosm.run==29092 || bcosm.run==29123 || bcosm.run==29127 ) continue;   //2020
        if( bcosm.run==29146 || bcosm.run==29161 || bcosm.run==29162 || bcosm.run==29165 ) continue;                       //2020
	if( bcosm.run==29336 || bcosm.run==29337 || bcosm.run==29431 || bcosm.run==29432 || bcosm.run==29433 ) continue;   //2020
	if( bcosm.run==29480 || bcosm.run==29441 || bcosm.run==29442 || bcosm.run==29443 || bcosm.run==29445 ) continue;   //2020
	if( bcosm.run==29446 || bcosm.run==29447 || bcosm.run==29448 || bcosm.run==29449 ) continue;                       //2020
	if( bcosm.run==29450 || bcosm.run==29451 || bcosm.run==29452 || bcosm.run==29455 || bcosm.run==29456 ) continue;   //2020
	if( bcosm.run==29457 || bcosm.run==29458 || bcosm.run==29459 || bcosm.run==29590 || bcosm.run==29591 ) continue;   //2020
        if( bcosm.run==29522 || bcosm.run==29525 || bcosm.run==29557 || bcosm.run==29558 || bcosm.run==29581 ) continue;   //2020
	if( bcosm.run==29595 || bcosm.run==29596 || bcosm.run==29597 || bcosm.run==29598 || bcosm.run==29599 ) continue;   //2020
	if( bcosm.run==29582 || bcosm.run==29589 || bcosm.run==29601 ) continue;                                           //2020

	if( k==0 || runprev!=bcosm.run ){
	    KDBconn* connection=kdb_open();
	    if( !connection ) {
		cerr<<"Can not establish connection to database"<<endl;
		return 1;
	    }

	    runTime_begin=kdb_run_get_begin_time(connection, bcosm.run);
	    runTime_end=kdb_run_get_end_time(connection, bcosm.run);
	    if(verbose){
		cout<<"\n========================================================================="<<endl;
		cout<<"Check database...."<<endl;
		cout<<"Begin time of Run"<< bcosm.run<<": "<<runTime_begin<<endl;
		cout<<"Begin time of Run"<< bcosm.run<<": "<<timestamp(runTime_begin)<<endl;
		cout<<"End time of Run"<< bcosm.run<<": "<<timestamp(runTime_end)<<endl;
	    }
	    kdb_close(connection);

	    NumRun++;
            if(verbose) cout<<"Number of runs processed is "<<NumRun<<endl;
	}

	//if ( verbose ) cout<<"P="<<bcosm.P<<"\t"<<"Natc_cross="<<bcosm.natc_cr<<endl;

        //for cosmic stability
	if( longStableATC )
	{
	    if( bcosm.P>min_p && bcosm.P<max_p  && bcosm.chi2<max_chi2 && bcosm.nhits>min_nhits && sqrt(pow(bcosm.Xip,2)+pow(bcosm.Yip,2)+pow(bcosm.Zip,2))<35 )
	    {
		for(int i=0; i<bcosm.natc_cr; i++)
		{
		    if ( verbose ) cout<<"cnt="<<bcosm.cnt[i]<<"\t"<<"npe="<<bcosm.npe[i]<<endl;
		    for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.single_aerogel_REGION5[i]==1 && bcosm.wlshit[i]!=1 )
		    {
			pr[j]->Fill(runTime_begin,bcosm.npe[i]);
			pr1[j]->Fill(bcosm.run,bcosm.npe[i]);
		    }
		}
	    }
	}

	//for light collection
	if( lightCoeffATC && fillFileForlightCoeffATC )
	{
	    if( bcosm.P>min_p && bcosm.P<max_p  && bcosm.chi2<max_chi2 && bcosm.nhits>min_nhits && sqrt(pow(bcosm.Xip,2)+pow(bcosm.Yip,2)+pow(bcosm.Zip,2))<35
                && bcosm.ncls>min_ncls && bcosm.emcenergy>min_energy && bcosm.munhits>min_muhits
	      )
	    {
		for(int i=0; i<bcosm.natc_cr; i++)
		{
		    if( cnt_type==5 && ( bcosm.cnt[i]>=0 && bcosm.cnt[i]<=19 ) ) { cnt_begin=0;  cnt_end=20; }
		    if( cnt_type==5 && ( bcosm.cnt[i]>=60 && bcosm.cnt[i]<=99 ) ) { cnt_begin=60;  cnt_end=100; }
		    if( cnt_type==5 && ( bcosm.cnt[i]>=140 ) ) { cnt_begin=140;  cnt_end=160; }

		    if( bcosm.cnt[i]>=cnt_begin && bcosm.cnt[i]<cnt_end )
		    {
			for(int k=0; k<size_cnt_type; k++)
			{
			    if(cnt_type==1){ phi_begin=(bcosm.phiin[i]+bcosm.phiout[i])/2; phi_end=(bcosm.phiin[i]+bcosm.phiout[i])/2; z_begin=(bcosm.zin[i]+bcosm.zout[i])/2; z_end=(bcosm.zin[i]+bcosm.zout[i])/2; phi_area1=phi1[k]-0.0015; phi_area2=phi2[k]-0.0015; z_area1=z1[k]; z_area2=z2[k];}
			    if(cnt_type==2){ phi_begin=(bcosm.phiin[i]+bcosm.phiout[i])/2; phi_end=(bcosm.phiin[i]+bcosm.phiout[i])/2; z_begin=(bcosm.zin[i]+bcosm.zout[i])/2; z_end=(bcosm.zin[i]+bcosm.zout[i])/2; phi_area1=phi1[k]+0.0055; phi_area2=phi2[k]+0.00055; z_area1=z1[k]; z_area2=z2[k];}
			    if(cnt_type==3){ phi_begin=(bcosm.phiin[i]+bcosm.phiout[i])/2; phi_end=(bcosm.phiin[i]+bcosm.phiout[i])/2; z_begin=(bcosm.zin[i]+bcosm.zout[i])/2; z_end=(bcosm.zin[i]+bcosm.zout[i])/2; phi_area1=phi1[k]-0.002785; phi_area2=phi2[k]-0.002785; z_area1=z1[k]; z_area2=z2[k];}
			    if(cnt_type==4){ phi_begin=(bcosm.phiin[i]+bcosm.phiout[i])/2; phi_end=(bcosm.phiin[i]+bcosm.phiout[i])/2; z_begin=(bcosm.zin[i]+bcosm.zout[i])/2; z_end=(bcosm.zin[i]+bcosm.zout[i])/2; phi_area1=phi1[k]+0.001874; phi_area2=phi2[k]+0.001874; z_area1=z1[k]; z_area2=z2[k];}
			    if(cnt_type==5){ phi_begin=(bcosm.rin[i]*sin(bcosm.phiin[i])+bcosm.rout[i]*sin(bcosm.phiout[i]))/2; phi_end=(bcosm.rin[i]*sin(bcosm.phiin[i])+bcosm.rout[i]*sin(bcosm.phiout[i]))/2; z_begin=(bcosm.rin[i]*cos(bcosm.phiin[i])+bcosm.rout[i]*cos(bcosm.phiout[i]))/2; z_end=(bcosm.rin[i]*cos(bcosm.phiin[i])+bcosm.rout[i]*cos(bcosm.phiout[i]))/2; phi_area1=x2[k]; phi_area2=x1[k]; z_area1=yy1[k]; z_area2=yy2[k];}

			    for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.single_aerogel_REGION0[i]==1 && bcosm.wlshit[i]!=1 && phi_begin<phi_area1 && phi_end>phi_area2 && z_begin>z_area1 && z_end<z_area2 )
			    {
				betta=sqrt(1-1/pow(((bcosm.emcenergy+105.65)/105.65),2));

				//1,2,3,5,13,15,19,23,26,37,39,40,57,60,63,64,66,93,109,113,125,129,152
				if( bcosm.cnt[i]==0 || bcosm.cnt[i]==1 ||  bcosm.cnt[i]==2 || bcosm.cnt[i]==4 || bcosm.cnt[i]==12 || bcosm.cnt[i]==14 || bcosm.cnt[i]==18 || bcosm.cnt[i]==22 || bcosm.cnt[i]==25 || bcosm.cnt[i]==36 || bcosm.cnt[i]==38 || bcosm.cnt[i]==39 || bcosm.cnt[i]==56 || bcosm.cnt[i]==59 || bcosm.cnt[i]==62 ||  bcosm.cnt[i]==63 ||  bcosm.cnt[i]==65 || bcosm.cnt[i]==92 ||  bcosm.cnt[i]==108 || bcosm.cnt[i]==112 || bcosm.cnt[i]==124 || bcosm.cnt[i]==128 ||  bcosm.cnt[i]==151) {
				    LC=(bcosm.npe[i]/bcosm.tlen[i])/(1.-1./pow(1.05576*betta,2));
				}
				else{
				    LC=(bcosm.npe[i]/bcosm.tlen[i])/(1.-1./pow(1.04752*betta,2));
				}
				nevents[k]++;
				ofstream of(atc_coef_forSim + "/" + TString::Format("data_cnt_Klight_area_all_%dtime_region_%dcnt_type_%darea.dat",time_region,cnt_type,k).Data(),ios::app);
				of<<LC<<endl;
				of.close();
			    }
			}
		    }
		}
	    }
	}

    }

    gStyle->SetPalette(1);
    gStyle->SetPadGridX(kTRUE);
    gStyle->SetPadGridY(kTRUE);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(1011);

    if( longStableATC ){
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
    }


    if( lightCoeffATC )
    {
        TString outPic = KEDR + "/" + NAME_str;
	gSystem->Exec("mkdir " + outPic);
	gSystem->Exec("cp " + indexFile + " " + outPic);

	TString fordel = NAME_str + ".dat";
	if( fillFileForlightCoeffATC ) gSystem->Exec("rm " + fordel);
	NAME_str = NAME_str + ".dat";

	TH1F* h1[size_cnt_type];
	TF1* myfit1[size_cnt_type];
	char name2[size_cnt_type];

        TCanvas *cc1 = new TCanvas();
        cc1->cd();

	for(int k=0; k<size_cnt_type; k++)
	{
	    if( fillFileForlightCoeffATC==0 ) nevents[k] = 70000;

	    sprintf(name2,"CntType%d_area%d",cnt_type,k);
	    numbin = 400;
	    double hbl = 0;
	    double hbr = 50;
	    h1[k] = new TH1F(name2,name2,numbin,hbl,hbr);

            if(verbose) cout<<"numbin="<<numbin<<endl;

	    float x0[nevents[k]];
	    ostringstream o_str;
            TString DIR_REF = atc_coef_forSim + "/" + TString::Format("data_cnt_Klight_area_all_%dtime_region_%dcnt_type_%darea.dat",time_region,cnt_type,k).Data();

	    string f_in_ref;
	    o_str << DIR_REF;
	    f_in_ref = o_str.str(); o_str.str("");

	    FILE *pF;
	    pF=fopen(f_in_ref.c_str(), "r");
	    for (int j=0; j<nevents[k]; j++)
	    {
		fscanf(pF," %f ",&x0[j]);
		h1[k]->Fill(x0[j]);
		//if(verbose) cout<<x0[j]<<endl;
	    }
	    fclose(pF);

	    h1[k]->Draw("hist");

	    myfit1[k] = new TF1("myfit1","gaus");

	    myfit1[k]->SetParName(0,"Const");
	    myfit1[k]->SetParName(1,"#mu");
	    myfit1[k]->SetParName(2,"p2");

	    myfit1[k]->SetParameter(0,10.0);
	    myfit1[k]->SetParameter(1,5.0);
	    myfit1[k]->SetParameter(2,1.0);

	    h1[k]->SetAxisRange(0., 4., "X");
	    double minb = h1[k]->GetMinimumBin();
            double boardL = minb*(hbr-hbl)/numbin;
            if(verbose) cout<<"minb="<<minb<<"\t"<<"boardL="<<boardL<<endl;
	    h1[k]->GetXaxis()->UnZoom();
	    h1[k]->Fit("myfit1","","",boardL-0.2,25.);
	    double chi2 =  myfit1[k]->GetChisquare();
            if(verbose) cout<<"chi2="<<chi2<<endl;

	    average_LC=myfit1[k]->GetParameter(1);
	    err_LC=myfit1[k]->GetParError(1);
	    if(verbose) cout<<"Mean="<<myfit1[k]->GetParameter(1)<<"\t"<<"average_LC="<<average_LC<<"\t"<<"err_LC="<<myfit1[k]->GetParError(1)<<endl;

	    for( int nrebin=2; nrebin<=3; nrebin++ ) if ( average_LC<0 )
	    {
		h1[k]->Rebin(nrebin);
		h1[k]->SetAxisRange(0., 4., "X");
		minb = h1[k]->GetMinimumBin();
                numbin = h1[k]->GetNbinsX();
		boardL = minb*(hbr-hbl)/numbin;
		if(verbose) cout<<"minb="<<minb<<"\t"<<"boardL="<<boardL<<endl;
		h1[k]->GetXaxis()->UnZoom();
		h1[k]->Fit("myfit1","","",boardL-0.2,25.);
		double chi2 =  myfit1[k]->GetChisquare();
		if(verbose) cout<<"chi2="<<chi2<<endl;
		average_LC=myfit1[k]->GetParameter(1);
                if( average_LC>0 ) continue;
	    }
	    average_LC=myfit1[k]->GetParameter(1);
	    err_LC=myfit1[k]->GetParError(1);

	    cc1->Update();
	    cc1->Print(outPic + "/" + TString::Format("hist_cnt_type%d_area%d.png",cnt_type,k).Data());

	    if(verbose) cout<<"Mean="<<myfit1[k]->GetParameter(1)<<"\t"<<"average_LC="<<average_LC<<"\t"<<"err_LC="<<myfit1[k]->GetParError(1)<<endl;

            if( average_LC<0. || average_LC>50. ) average_LC = h1[k]->GetMean();

	    //====================================================================================

	    ofstream of2(NAME_str.c_str(),ios::app);
	    of2<<k+1<<"\t"<<average_LC<<"\t"<<err_LC<<endl;
	    of2.close();
	}
    }

}
