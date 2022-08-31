#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <vector>
#include <algorithm>
#include <TMultiGraph.h>
#include <TGraphErrors.h>

#include "KDB/kdb.h"

#include "light_coefficient_cnt_cor.h"

using namespace std;
string progname;

int Usage(string status)
{
   cout<<"Usage: "<<progname<<"\t"<<"Verbose->(0/1)  Range with data->(1,2...)  Cnt type->(AY=1,AS=2,AR=3,AQ=4,EX=5)"<<endl;
   cout<<"For example: 1) ./light_coefficient 0 3 2 "<<endl;
   cout<<"             2) ./light_coefficient 1 3 2 "<<endl;
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
    if( argc>2 ) time_region=atoi(argv[2]);
    if( argc>3 ) cnt_type=atoi(argv[3]);
    if( argc>4 || cnt_type>5 || time_region>18 ){ Usage(progname); return 0;}

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

    atc_coef_forSim = KEDR + "/" + "tmp";
    gSystem->Exec("mkdir " + atc_coef_forSim);
    gSystem->Exec("ln -s "+ atc_coef_forSim + " tmp");

    //we have 5 type of counters
    if ( cnt_type==1 ){ cnt_begin=20; cnt_end=40; size_cnt_type=58; NAME_str=TString::Format("data_AY_%02drange",time_region).Data();}    //20-40cnt - AY
    if ( cnt_type==2 ){ cnt_begin=40; cnt_end=60; size_cnt_type=68; NAME_str=TString::Format("data_AS_%02drange",time_region).Data();}    //40-60cnt - AS
    if ( cnt_type==3 ){ cnt_begin=100; cnt_end=120; size_cnt_type=68; NAME_str=TString::Format("data_AR_%02drange",time_region).Data();}  //100-120cnt - AR
    if ( cnt_type==4 ){ cnt_begin=120; cnt_end=140; size_cnt_type=58; NAME_str=TString::Format("data_AQ_%02drange",time_region).Data();}  //120-140cnt - AQ
    if ( cnt_type==5 ){ cnt_begin=0; cnt_end=160; size_cnt_type=28; NAME_str=TString::Format("data_EX_%02drange",time_region).Data();}    //EX

    if(verbose) cout<<"Time range is "<<time_region<<";"<<"\t"<<"Cnt type is "<<cnt_type<<";"<<"\t"<<"Size Cnt type is "<<size_cnt_type<<endl;

    //setup geometry sizes for areas of cnt type
    set_geometry_atc_cnts();

    for( int k=0; k<size_cnt_type; k++ )
    {
	for( int cntATC=0; cntATC<160; cntATC++ )
	{
	    if( cntATC>=cnt_begin && cntATC<cnt_end )
	    {
		gSystem->Exec("rm -r " + atc_coef_forSim + "/" + TString::Format("data_cnt%d_Klight_area_all_%dtime_region_%dcnt_type_%darea.dat",cntATC,time_region,cnt_type,k).Data());
	    }
	}
    }

    //include samples
    chain();
    setbranchstatus();
    setbranchaddress();

    Int_t nentr=tt->GetEntries();
    if(verbose) cout<<"Total number of events in trees is "<<nentr<<endl;

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
			    nevents[j][k]++;
			    ofstream of(atc_coef_forSim + "/" + TString::Format("data_cnt%d_Klight_area_all_%dtime_region_%dcnt_type_%darea.dat",j,time_region,cnt_type,k).Data(),ios::app);
			    of<<LC<<endl;
			    of.close();
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

    char name2[size_cnt_type];
    TCanvas *cc1 = new TCanvas();
    cc1->cd();

    TString outPic[160];
    for( int cntATC=0; cntATC<160; cntATC++ )
    {
	if( cnt_type==5 && ( cntATC>=0 && cntATC<=19 ) ) { cnt_begin=0;  cnt_end=20; }
	if( cnt_type==5 && ( cntATC>=60 && cntATC<=99 ) ) { cnt_begin=60;  cnt_end=100; }
	if( cnt_type==5 && ( cntATC>=140 ) ) { cnt_begin=140;  cnt_end=160; }

	if( cntATC>=cnt_begin && cntATC<cnt_end )
	{
	    outPic[cntATC] = KEDR + "/" + NAME_str + "/" + TString::Format("cntATC%d",cntATC).Data();
	    gSystem->Exec("mkdir " + outPic[cntATC]);
	    gSystem->Exec("cp " + indexFile + " " + outPic[cntATC]);

	    gSystem->Exec("rm -r " + atc_coef_forSim + "/" + TString::Format("data_Kcnt_cnt%d_%dtime_region_%dcnt_type.dat",cntATC,time_region,cnt_type).Data() );

	    TString fordel = TString::Format("data_Kcnt_%d-%d_%dtime_range.dat",cnt_begin,cnt_end,time_region).Data();
            gSystem->Exec("rm " + fordel);
	}
    }

    for( int k=0; k<size_cnt_type; k++ )
    {
	for( int cntATC=0; cntATC<160; cntATC++ )
	{
	    if( cnt_type==5 && ( cntATC>=0 && cntATC<=19 ) ) { cnt_begin=0;  cnt_end=20; }
	    if( cnt_type==5 && ( cntATC>=60 && cntATC<=99 ) ) { cnt_begin=60;  cnt_end=100; }
	    if( cnt_type==5 && ( cntATC>=140 ) ) { cnt_begin=140;  cnt_end=160; }

	    if( cntATC>=cnt_begin && cntATC<cnt_end )
	    {
		sprintf(name2,"CntType%d_area%d",cnt_type,k);
		numbin = 400;
		double hbl = 0;
		double hbr = 50;
		TH1F *h1 = new TH1F(name2,name2,numbin,hbl,hbr);

		if(verbose) cout<<"numbin="<<numbin<<endl;

		TString DIR_REF = atc_coef_forSim + "/" + TString::Format("data_cnt%d_Klight_area_all_%dtime_region_%dcnt_type_%darea.dat",cntATC,time_region,cnt_type,k).Data();

		float x0;
		string nf_param(DIR_REF.Data());
		std::ifstream f_in(nf_param.c_str());
		std::string o;
		if (!f_in.is_open())
		    cout << "File not open!!!\n";
		else
		{
		    while( getline(f_in, o) ){
			if( o.size()>=1 && o[0]!='#' ){
			    std::istringstream i_str(o);
			    i_str >> x0;
			    h1->Fill(x0);
			    if(verbose) cout<<x0<<endl;
			}
		    }
		    f_in.close();
		}

		h1->Draw("hist");

		TF1 *myfit1 = new TF1("myfit1","gaus");

		//TF1 *myfit1 = new TF1("myfit1",poissonf,0,10,2);

		myfit1->SetParName(0,"Const");
		myfit1->SetParName(1,"#mu");
		myfit1->SetParName(2,"p2");

		//myfit1->SetParameter(0,0.01);
		//myfit1->SetParameter(1,0.1);
		myfit1->SetParameter(0,10.0);
		myfit1->SetParameter(1,5.0);
		myfit1->SetParameter(2,1.0);

		//TF1 *myfit1 = new TF1("myfit1","[0] * TMath::Exp(-[1]) * TMath::Power([1],x) / TMath::Gamma(x+1.0)", 0.0, 50.0);
		//myfit1->SetParameter(0,10.0);
		//myfit1->SetParameter(1,5.0);

		h1->SetAxisRange(0., 4., "X");
		double minb = h1->GetMinimumBin();
		double boardL = minb*(hbr-hbl)/numbin;
		if(verbose) cout<<"minb="<<minb<<"\t"<<"boardL="<<boardL<<endl;
		h1->GetXaxis()->UnZoom();
		h1->Fit("myfit1","","",boardL-0.2,25.);
		double chi2 =  myfit1->GetChisquare();
		if(verbose) cout<<"chi2="<<chi2<<endl;

		average_LC=myfit1->GetParameter(1);
		err_LC=myfit1->GetParError(1);
		if(verbose) cout<<"Mean="<<myfit1->GetParameter(1)<<"\t"<<"average_LC="<<average_LC<<"\t"<<"err_LC="<<myfit1->GetParError(1)<<endl;

		for( int nrebin=2; nrebin<=3; nrebin++ ) if ( average_LC<0 )
		{
		    h1->Rebin(nrebin);
		    h1->SetAxisRange(0., 4., "X");
		    minb = h1->GetMinimumBin();
		    numbin = h1->GetNbinsX();
		    boardL = minb*(hbr-hbl)/numbin;
		    if(verbose) cout<<"minb="<<minb<<"\t"<<"boardL="<<boardL<<endl;
		    h1->GetXaxis()->UnZoom();
		    h1->Fit("myfit1","","",boardL-0.2,25.);
		    double chi2 =  myfit1->GetChisquare();
		    if(verbose) cout<<"chi2="<<chi2<<endl;
		    average_LC=myfit1->GetParameter(1);
		    if( average_LC>0 ) continue;
		}

		cc1->Update();
		cc1->Print(outPic[cntATC] + "/" + TString::Format("hist_cnt%d_type%d_area%d.png",cntATC,cnt_type,k).Data());

		average_LC=myfit1->GetParameter(1);
		err_LC=myfit1->GetParError(1);
		if(verbose) cout<<"Mean="<<myfit1->GetParameter(1)<<"\t"<<"average_LC="<<average_LC<<"\t"<<"err_LC="<<myfit1->GetParError(1)<<endl;
		if( average_LC<0. || average_LC>50. ) average_LC = h1->GetMean();
		h1->Delete();
                myfit1->Delete();

		//====================================================================================
                //open file with LC for several counters of one type
		int a;
		float b, c;

		string nf_param2  = NAME_str + ".dat";
		std::ifstream f_in2(nf_param2.c_str());
		std::string o2;
		if (!f_in2.is_open())
		    cout << "File not open!!!\n";
		else
		{
		    while( getline(f_in2, o2) ){
			if( o2.size()>=1 && o2[0]!='#' ){
			    std::istringstream i_str(o2);
			    i_str >> a >> b >> c ;
			    if(verbose) cout<<a<<"\t"<<b<<"\t"<<c<<endl;
			    if( a==(k+1) ) break;
			}
		    }
		    f_in.close();
		}

		//====================================================================================
		//determine correction for each cntATC for type of counter
		float Kcnt;
		if( nevents[cntATC][k]<15 ){ Kcnt=0; }
		else{
		    Kcnt=average_LC/b;
		}
		if(verbose) cout<<a<<"\t"<<"Kcnt="<<Kcnt<<endl;

		ofstream of1(atc_coef_forSim + "/" + TString::Format("data_Kcnt_cnt%d_%dtime_region_%dcnt_type.dat",cntATC,time_region,cnt_type).Data(),ios::app);
		of1<<a<<"\t"<<Kcnt<<endl;
		of1.close();
	    }
       	}
    }
    //====================================================================================

    TCanvas *cc2 = new TCanvas();
    cc2->cd();

    for( int cntATC=0; cntATC<160; cntATC++ )
    {
	if( cnt_type==5 && ( cntATC>=0 && cntATC<=19 ) ) { cnt_begin=0;  cnt_end=20; }
	if( cnt_type==5 && ( cntATC>=60 && cntATC<=99 ) ) { cnt_begin=60;  cnt_end=100; }
	if( cnt_type==5 && ( cntATC>=140 ) ) { cnt_begin=140;  cnt_end=160; }

	if( cntATC>=cnt_begin && cntATC<cnt_end )
	{
	    float x1,y1;
	    TH1F *h2=new TH1F("h2","The distribution Kcnt",20,0,2.5);

	    TString DIR_REF = atc_coef_forSim + "/" + TString::Format("data_Kcnt_cnt%d_%dtime_region_%dcnt_type.dat",cntATC,time_region,cnt_type).Data();

	    string nf_param(DIR_REF.Data());
	    std::ifstream f_in(nf_param.c_str());
	    std::string o;
	    if (!f_in.is_open())
		cout << "File not open!!!\n";
	    else
	    {
		while( getline(f_in, o) ){
		    if( o.size()>=1 && o[0]!='#' ){
			std::istringstream i_str(o);
			i_str >> x1 >> y1 ;
			h2->Fill(y1);
		    }
		}
		f_in.close();
	    }

	    h2->Draw("hist");
	    TF1* myfit2=new TF1("myfit2","gaus");
	    myfit2->SetParameter(0,1.0);
	    myfit2->SetParameter(1,1.0);
	    myfit2->SetParameter(2,0.2);
	    h2->Fit("myfit2","","",0.2,2.0);

	    cc2->Update();
	    cc2->Print(outPic[cntATC] + "/" + TString::Format("hist_cnt%d_type%d_all.png",cntATC,cnt_type).Data());

	    if(verbose) cout<<"Mean="<<myfit2->GetParameter(1)<<endl;
	    h2->Delete();

	    ofstream of2(TString::Format("data_Kcnt_%d-%d_%dtime_range.dat",cnt_begin,cnt_end,time_region).Data(),ios::app);
	    of2<<cntATC<<"\t"<<myfit2->GetParameter(1)<<endl;
	    of2.close();
	}
    }

}
