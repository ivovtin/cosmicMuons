#ifndef kp_cosmic_h
# define kp_cosmic_h

#include <Riostream.h>
#include <sstream>
#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>
#include <TGraphErrors.h>
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
#include <vector>
#include <TChain.h>
#include "TMinuit.h"
#include "TRandom3.h"
#include <math.h>
#include "TVirtualFitter.h"
#include <algorithm>
#include <assert.h>

    struct data
    {
	int vrtntrk,vrtnip,vrtnbeam,nhits,nhitsvd,nhitsxy,nhitsz,nvec,nvecxy,nvecz,ncls,nlkr,ncsi,munhits,run,rEv;
	float P,Pt,chi2,theta,phi,emcenergy,lkrenergy,csienergy;
	int natc_cr, cnt[10],aerogel_REGION[10],aerogel_REGION0[10],aerogel_REGION5[10],aerogel_REGION20[10],
	    active_REGION[10],active_REGION0[10],active_REGION5[10],active_REGION20[10],testreg[10],
	single_aerogel_REGION[10],single_aerogel_REGION0[10],single_aerogel_REGION5[10],single_aerogel_REGION20[10],
	single_active_REGION[10],single_active_REGION0[10],single_active_REGION5[10],single_active_REGION20[10],
	single_testreg[10];
	float wlshit[10],nearwls[10],tlen[10],pathwls[10],npe[10],npen[10];
        float Xip,Yip,Zip;
    };
    data bcosm;

    int key;
    TChain *tt=new TChain("et");
    void chain(){
	if(key==2014){
	    for(int i=1; i<=19; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jun14_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_oct14_%d.root",i).Data());
	    for(int i=1; i<=7; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_nov14_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_dec14_%d.root",i).Data());
	}
	else if (key==2015){
	    for(int i=1; i<=2; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jan15_%d.root",i).Data());
	    for(int i=1; i<=4; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_feb15_%d.root",i).Data());
	    for(int i=1; i<=2; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_apr15_%d.root",i).Data());
	    for(int i=1; i<=2; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_may15_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jun15_%d.root",i).Data());
	    for(int i=1; i<=2; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jul15_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_oct15_%d.root",i).Data());
	    for(int i=1; i<=4; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_nov15_%d.root",i).Data());
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_dec15_%d.root",i).Data());
	}
	else if (key==2016){
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jan16_%d.root",i).Data());
	    for(int i=1; i<=7; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_feb16_%d.root",i).Data());
	    for(int i=1; i<=4; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_apr16_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_march16_%d.root",i).Data());
	    for(int i=1; i<=6; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_may16_%d.root",i).Data());
	    for(int i=1; i<=4; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jun16_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_oct16_%d.root",i).Data());
	}
	else if (key==2017){
	    for(int i=1; i<=2; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_feb17_%d.root",i).Data());
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_apr17_%d.root",i).Data());
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_march17_%d.root",i).Data());
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_may17_%d.root",i).Data());
	    for(int i=1; i<=1; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jun17_%d.root",i).Data());
	    for(int i=1; i<=9; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_oct17_%d.root",i).Data());
	    for(int i=1; i<=1; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_nov17_%d.root",i).Data());
	    for(int i=1; i<=7; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_dec17_%d.root",i).Data());
	}
	else if (key==2018){
	    for(int i=1; i<=7; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jan18_%d.root",i).Data());
	    for(int i=1; i<=7; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_feb18_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_apr18_%d.root",i).Data());
	    for(int i=1; i<=5; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_march18_%d.root",i).Data());
	    for(int i=1; i<=6; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_may18_%d.root",i).Data());
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jun18_%d.root",i).Data());
	    for(int i=1; i<=3; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_nov18_%d.root",i).Data());
	}
	else if (key==2019){
	    for(int i=1; i<=7; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_apr19_%d.root",i).Data());
	    for(int i=1; i<=13; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_may19_%d.root",i).Data());
	    for(int i=1; i<=10; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_jun19_%d.root",i).Data());
	    for(int i=1; i<=15; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_oct19_%d.root",i).Data());
	    for(int i=1; i<=13; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_nov19_%d.root",i).Data());
	    for(int i=1; i<=1; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_dec19_%d.root",i).Data());
	}
	else if (key==2020){
	    for(int i=1; i<=10; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_feb20_%d.root",i).Data());
	    for(int i=1; i<=9; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_march20_%d.root",i).Data());
	    for(int i=1; i<=15; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_sep20_%d.root",i).Data());
	    for(int i=1; i<=1; i++) tt->Add(TString::Format("/spool/users/ovtin/cosmruns/results/cosm_runs_oct20_%d.root",i).Data());
	}
    }

    void setbranchstatus(){
	tt->SetBranchStatus("*",0);
	tt->SetBranchStatus("bcosm",1);
    }

    void setbranchaddress(){
	tt->SetBranchAddress("bcosm",&bcosm);
    }

#endif
