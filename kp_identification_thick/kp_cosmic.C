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
	cout<<"Usage: "<<progname<<"\t"<<" 2014 ( or 2015, 2016 ...)   Npethr"<<endl;
        exit(0);
}

int main(int argc, char* argv[])
{
    progname=argv[0];
    if( argc>1 )
    {
	key=atoi(argv[1]);
	Npethr=atof(argv[2]);
	if( key>2023 ){ Usage(progname); return 0;}
	if( key<2014 ){ Usage(progname); return 0;}
    }
    else
    {
	Usage(progname);
    }

    //for print DEBUG information
    bool verbose=0;

    //***************preselections*************
    float min_p=50.; //MeV
    float max_p=10000.; //MeV
    float max_chi2=100.;
    float min_nhits=30.;
    //****************************************

    TFile *fout=0;
    TString fnameout;
    TString KEDR;
    TString dir_out="/store/users/ovtin/cosmruns/results";
    KEDR = dir_out + "/" + TString::Format("kp_identification_thick_aerReg0_new_%d_%f",key,Npethr).Data();
    gSystem->Exec("mkdir "+ KEDR);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ KEDR);
    gSystem->Exec("ln -s "+ KEDR + " /home/ovtin/public_html/atc_cosmic/thick_cnt_aerReg0/"+ TString::Format("kp_identification_thick_%d_%f",key,Npethr).Data());
    fnameout=KEDR + "/" + TString::Format("kp_ident_thick_cosm_aerReg0_%d.root",key).Data();

    cout<<fnameout<<endl;
    fout = new TFile(fnameout,"RECREATE");

    //include samples
    chain();
    Int_t nentr=tt->GetEntries();
    if(verbose) cout<<"Nentries="<<nentr<<endl;

    setbranchstatus();
    setbranchaddress();

    float Pkaon,Ppion, mkaon=493.66, mpion=139.57, mmuon=105.65;

    char name1[160],name2[160],name3[160],name4[160],name5[160],name6[160],name7[160],name8[160];
    TH1F* h1[160];
    TH1F* h2[160];
    TH1F* h3[160];
    TH1F* h4[160];
    TH1F* h5[160];
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
	sprintf(name8,"h5_runs_cnt%d",j);
	if( key==2014 ) h5[j]=new TH1F(name8,"runs",1230,19635,20865);
	if( key==2015 ) h5[j]=new TH1F(name8,"runs",2091,20885,22976);
	if( key==2016 ) h5[j]=new TH1F(name8,"runs",1176,23002,24176);
	if( key==2017 ) h5[j]=new TH1F(name8,"runs",1723,24597,26320);
	if( key==2018 ) h5[j]=new TH1F(name8,"runs",1587,26337,27924);
	if( key==2019 ) h5[j]=new TH1F(name8,"runs",961,28059,29020);
	if( key==2020 ) h5[j]=new TH1F(name8,"runs",513,29091,29604);
	if( key==2021 ) h5[j]=new TH1F(name8,"runs",887,29607,30494);
	if( key==2022 ) h5[j]=new TH1F(name8,"runs",993,30546,31539);
	if( key==2023 ) h5[j]=new TH1F(name8,"runs",404,31561,31965);
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

	//skip runs
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
	if( bcosm.run>=29980 && bcosm.run<=29995 ) continue;                                                               //2020
	if( bcosm.run>=29845 && bcosm.run<=29860 ) continue;                                                               //2020
	if( bcosm.run>=29950 && bcosm.run<=29955 ) continue;                                                               //2020

        //skip runs with bad 1 ph.e. calibration
        if( bcosm.run>=28301 && bcosm.run<=28474 ) continue;
        if( bcosm.run>=28670 && bcosm.run<=28750 ) continue;
        if( bcosm.run>=29450 && bcosm.run<=29529 ) continue;
	if( bcosm.run>=30050 && bcosm.run<=30250 ) continue;

        //2021-2022
        if( bcosm.run==29607 || bcosm.run==29609 || bcosm.run==29612 || bcosm.run==29613 || bcosm.run==29614 || bcosm.run==29618 ) continue;
        if( bcosm.run==29620 || bcosm.run==29624 || bcosm.run==29625 || bcosm.run==29626 || bcosm.run==29631 || bcosm.run==29632 ) continue;
        if( bcosm.run==29633 || bcosm.run==29634 || bcosm.run==29665 || bcosm.run==29674 || bcosm.run==29675 || bcosm.run==29676 ) continue;
        if( bcosm.run==29686 || bcosm.run==29732 || bcosm.run==29733 || bcosm.run==29748 || bcosm.run==29763 || bcosm.run==29784 ) continue;
        if( bcosm.run==29785 || bcosm.run==29802 || bcosm.run==29805 || bcosm.run==29806 || bcosm.run==29808 || bcosm.run==29827 ) continue;
        if( bcosm.run==29839 || bcosm.run==29840 || bcosm.run==29867 || bcosm.run==29877 || bcosm.run==29880 || bcosm.run==29881 ) continue;
        if( bcosm.run==29885 || bcosm.run==29889 || bcosm.run==29890 || bcosm.run==29903 || bcosm.run==29905 || bcosm.run==29947 ) continue;
        if( bcosm.run==29949 || bcosm.run==29973 || bcosm.run==29989 || bcosm.run==30064 || bcosm.run==30065 || bcosm.run==30071 ) continue;
        if( bcosm.run==30072 || bcosm.run==30133 || bcosm.run==30134 || bcosm.run==30149 || bcosm.run==30163 || bcosm.run==30247 ) continue;
        if( bcosm.run==30277 || bcosm.run==30284 || bcosm.run==30338 || bcosm.run==30400 || bcosm.run==30445 || bcosm.run==30452 ) continue;
        if( bcosm.run==30453 || bcosm.run==30454 || bcosm.run==30455 || bcosm.run==30456 || bcosm.run==30462 || bcosm.run==30465 ) continue;
        if( bcosm.run==30482 || bcosm.run==30483 || bcosm.run==30488 || bcosm.run==30489 || bcosm.run==30490 || bcosm.run==30491 ) continue;
        if( bcosm.run==30492 || bcosm.run==30493 || bcosm.run==30494 || bcosm.run==30576 || bcosm.run==30577 || bcosm.run==30584 ) continue;
        if( bcosm.run==30585 || bcosm.run==30610 || bcosm.run==30639 || bcosm.run==30640 || bcosm.run==30643 || bcosm.run==30644 ) continue;
        if( bcosm.run==30693 || bcosm.run==30694 || bcosm.run==30817 || bcosm.run==30822 || bcosm.run==30823 || bcosm.run==30881 ) continue;
        if( bcosm.run==30882 || bcosm.run==30956 || bcosm.run==30964 || bcosm.run==30997 || bcosm.run==30999 || bcosm.run==31000 ) continue;
        if( bcosm.run==31004 || bcosm.run==31021 || bcosm.run==31028 || bcosm.run==31029 || bcosm.run==31030 || bcosm.run==31105 ) continue;
        if( bcosm.run==31120 || bcosm.run==31145 || bcosm.run==31149 || bcosm.run==31150 || bcosm.run==31152 || bcosm.run==31153 ) continue;
        if( bcosm.run==31155 || bcosm.run==31156 || bcosm.run==31157 || bcosm.run==31158 || bcosm.run==31164 || bcosm.run==31206 ) continue;
        if( bcosm.run==31207 || bcosm.run==31209 || bcosm.run==31210 || bcosm.run==31239 || bcosm.run==31247 || bcosm.run==31248 ) continue;
	if( bcosm.run==31317 || bcosm.run==31324 || bcosm.run==31325 || bcosm.run==31521 || bcosm.run==31522 ) continue;
	if( bcosm.run==30760 || bcosm.run==30819 ) continue;
	if( bcosm.run>=30830 && bcosm.run<=30840 ) continue;
	if( bcosm.run>=29530 && bcosm.run<=29540 ) continue;
	if( bcosm.run>=29550 && bcosm.run<=29604 ) continue;
	if( bcosm.run>=31478 && bcosm.run<=31522 ) continue;

        //2023
	if( bcosm.run==31561 || bcosm.run==31562 || bcosm.run==31578 || bcosm.run==31583 ) continue;   //jan23 skip
	if( bcosm.run==31639 || bcosm.run==31641 || bcosm.run==31642 ) continue;                       //feb23cosmruns_1
	if( bcosm.run==31646 || bcosm.run==31650 || bcosm.run==31658 ) continue;                       //feb23cosmruns_2
	if( bcosm.run==31659 || bcosm.run==31660 || bcosm.run==31680 ) continue;                       //feb23cosmruns_3
	if( bcosm.run==31681 || bcosm.run==31682 || bcosm.run==31686 ) continue;                       //feb23cosmruns_4
	if( bcosm.run==31693 || bcosm.run==31697 || bcosm.run==31698 ) continue;                       //march23cosmruns_1
	if( bcosm.run==31699 || bcosm.run==31703 || bcosm.run==31704 ) continue;                       //march23cosmruns_2
	if( bcosm.run==31732 || bcosm.run==31747 || bcosm.run==31748 ) continue;                       //march23cosmruns_4
	if( bcosm.run==31749 || bcosm.run==31750 ) continue;                                           //march23cosmruns_5
	if( bcosm.run==31793 ) continue;                                                               //march23cosmruns_6
	if( bcosm.run==31851 || bcosm.run==31853 || bcosm.run==31854 ) continue;                       //march23cosmruns_7
	if( bcosm.run==31855 ) continue;                                                               //march23cosmruns_8
	if( bcosm.run==31918 ) continue;                                                               //april23cosmruns_1

	if( bcosm.P>min_p && bcosm.P<max_p &&  bcosm.chi2<max_chi2 && bcosm.nhits>min_nhits && sqrt(pow(bcosm.Xip,2)+pow(bcosm.Yip,2)+pow(bcosm.Zip,2))<35 )
	{
	    Pkaon=bcosm.P*(mkaon/mmuon);
	    Ppion=bcosm.P*(mpion/mmuon);

	    int n=0;
	    int nn=0;
	    int j=0;
	    int jj=0;
	    int counter0;
	    int counter1;
	    int counter2;
	    int counter3;
	    int counter4;
	    int counter5;
	    int counter6;
	    float sum_npe=0;
	    float sum_npe1=0;
	    int n2=0;
	    int n3=0;
	    int n5=0;
	    int n6=0;

	    for(int i=0; i<bcosm.natc_cr; i++)
	    {
		if ( verbose ) cout<<"cnt"<<bcosm.cnt[i]<<"\t"<<"npe="<<bcosm.npe[i]<<endl;

		//if( bcosm.single_aerogel_REGION5[i]==1 && bcosm.wlshit[i]!=1 )
		if( bcosm.single_aerogel_REGION0[i]==1 && bcosm.wlshit[i]!=1 )
		{
		    //cout<<"cnt="<<bcosm.cnt[i]<<endl;

		    //crossing cylinder with top
		    if (j==0) counter1=bcosm.cnt[i];
		    j++;
		    if(counter1<80){
			counter2=counter1+80;
			counter3=counter1+79;
			if( counter1==20 || counter1==40 || counter1==0 || counter1==60 ) counter3=counter1+99;
		    }
		    else {
			counter2=counter1-80;
			counter3=counter1-79;
			if( counter1==119 || counter1==139 || counter1==99 || counter1==159 ) counter3=counter1-99;
		    }

		    if( bcosm.cnt[i]==counter1 || bcosm.cnt[i]==counter2 || bcosm.cnt[i]==counter3 )
		    {
			n++;
			sum_npe+=bcosm.npe[i];
			//cout<<"\t"<<"k="<<k<<"\t"<<"counter1="<<counter1<<"\t"<<"bcosm.cnt[i]="<<bcosm.cnt[i]<<endl;
			if(bcosm.cnt[i]==counter2) n2++;
			if(bcosm.cnt[i]==counter3) n3++;
		    }

		    //crossing cylinder with bottom
		    if( bcosm.cnt[i]!=counter1 && bcosm.cnt[i]!=counter2 && bcosm.cnt[i]!=counter3 )
		    {
			if (jj==0)
			{
			    sum_npe1=0;
			    counter4=bcosm.cnt[i];
			}
			jj++;
			if(counter4<80){
			    counter5=counter4+80;
			    counter6=counter4+79;
			    if( counter4==20 || counter4==40 || counter4==0 || counter4==60 ) counter6=counter4+99;
			}
			else{
			    counter5=counter4-80;
			    counter6=counter4-79;
			    if( counter4==119 || counter4==139 || counter4==99 || counter4==159 ) counter6=counter4-99;
			}
		    }

		    if( bcosm.cnt[i]==counter4 || bcosm.cnt[i]==counter5 || bcosm.cnt[i]==counter6 )
		    {
			nn++;
			sum_npe1+=bcosm.npe[i];
			//cout<<"\t"<<"k="<<k<<"\t"<<"counter4="<<counter4<<"\t"<<"bcosm.cnt[i]="<<bcosm.cnt[i]<<endl;
			if(bcosm.cnt[i]==counter5) n5++;
			if(bcosm.cnt[i]==counter6) n6++;
		    }

		    //if ( bcosm.cnt[i]!=counter1 && bcosm.cnt[i]!=counter2 && bcosm.cnt[i]!=counter3 && bcosm.cnt[i]!=counter4 && bcosm.cnt[i]!=counter5 && bcosm.cnt[i]!=counter6 ) cout<<"\t"<<"k="<<k<<"\t"<<"************************************bcosm.cnt[i]="<<bcosm.cnt[i]<<endl;

		    if(n>1 || nn>1)
		    {
			//cout<<"n>1 or nn>1"<<endl;

			counter0=counter1;
			if ( counter0>=80 && n2==1 ) counter0=counter2;
			if ( counter0>=80 && n3==1 ) counter0=counter3;

			if(nn>1)
			{
			    counter0=counter4;
			    if ( counter0>=80 && n5==1 ) counter0=counter5;
			    if ( counter0>=80 && n6==1 ) counter0=counter6;
			    sum_npe=sum_npe1;
			}

			h1[counter0]->Fill(bcosm.P);

			if(bcosm.P>200 && bcosm.P<300){
			    h2[counter0]->Fill(sum_npe);
			}

			if(bcosm.P>700 && bcosm.P<1100){
			    h3[counter0]->Fill(sum_npe);
			}

			h4[counter0]->Fill(bcosm.emcenergy/bcosm.P);
			h5[counter0]->Fill(bcosm.run);

			pr1[counter0]->Fill(bcosm.P,sum_npe);
			pr2[counter0]->Fill(Ppion,sum_npe);
			pr3[counter0]->Fill(Pkaon,sum_npe);

			for(int ii=0; ii<=20-1; ii++)
			{
			    p_all[counter0][ii]=100*ii+50;
			    err_p_all[counter0][ii]=50;

			    if(bcosm.P>100*ii&&bcosm.P<100+100*ii){
				if(sum_npe>Npethr)
				{
				    N_npenotzero_mu[counter0][ii]=++N_npenotzero_mu[counter0][ii];
				}
				N_npetotal_mu[counter0][ii]=++N_npetotal_mu[counter0][ii];
				eff_mu[counter0][ii]=N_npenotzero_mu[counter0][ii]/N_npetotal_mu[counter0][ii];
				err_eff_mu[counter0][ii]=sqrt(N_npenotzero_mu[counter0][ii])/N_npetotal_mu[counter0][ii];
				not_eff_mu[counter0][ii]=1-eff_mu[counter0][ii];
			    }
			    if(Ppion>100*ii&&Ppion<100+100*ii){
				if(sum_npe>Npethr)
				{
				    N_npenotzero_p[counter0][ii]=++N_npenotzero_p[counter0][ii];
				}
				N_npetotal_p[counter0][ii]=++N_npetotal_p[counter0][ii];
				eff_p[counter0][ii]=N_npenotzero_p[counter0][ii]/N_npetotal_p[counter0][ii];
				err_eff_p[counter0][ii]=sqrt(N_npenotzero_p[counter0][ii])/N_npetotal_p[counter0][ii];
				not_eff_p[counter0][ii]=1-eff_p[counter0][ii];
			    }
			    if(Pkaon>100*ii&&Pkaon<100+100*ii){
				if(sum_npe>Npethr)
				{
				    N_npenotzero_k[counter0][ii]=++N_npenotzero_k[counter0][ii];
				}
				N_npetotal_k[counter0][ii]=++N_npetotal_k[counter0][ii];
				eff_k[counter0][ii]=N_npenotzero_k[counter0][ii]/N_npetotal_k[counter0][ii];
				err_eff_k[counter0][ii]=sqrt(N_npenotzero_k[counter0][ii])/N_npetotal_k[counter0][ii];
				not_eff_k[counter0][ii]=1-eff_k[counter0][ii];
			    }

			    Ksigma[counter0][ii]=0;
			    Ksigma[counter0][ii]=abs(sqrt(2.)*(TMath::ErfInverse(1-2*eff_k[counter0][ii])+TMath::ErfInverse(1-2*(1-eff_p[counter0][ii]))));
			    if ( verbose ) cout<<"#sigma="<<Ksigma[counter0][ii]<<endl;
			}
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

    float mu_0;
    float mu_max;
    float p_thr;
    float p_cal;
    float mu_cal;
    TH1F *h6 = new TH1F("N_{ph.e.}","N_{ph.e.}",20,0,20);
    TH1F *h7 = new TH1F("Barrel N_{ph.e.}","N_{ph.e.}",20,0,20);
    TH1F *h8 = new TH1F("Endcap N_{ph.e.}","N_{ph.e.}",20,0,20);

    for(int i=0; i<80; i++)
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
	gPad->SetLogy();
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
	//gPad->Modified(); gPad->Update();
	//pr1[i]->GetYaxis()->SetLimits(0., 15.);
	//pr1[i]->SetMinimum(0.);
	//pr1[i]->SetMaximum(15.0);
	gPad->Modified(); gPad->Update();

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
	//gPad->Modified(); gPad->Update();
	//pr2[i]->GetYaxis()->SetLimits(0., 15.);
	//pr2[i]->SetMinimum(0.);
	//pr2[i]->SetMaximum(15.0);
	//gPad->Modified(); gPad->Update();
	pr3[i]->Draw("same");
	//gPad->Modified(); gPad->Update();
	//pr3[i]->GetYaxis()->SetLimits(0., 15.);
	//pr3[i]->SetMinimum(0.);
	//pr3[i]->SetMaximum(15.0);
	//gPad->Modified(); gPad->Update();

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
	gr1[i]->Write("gr1");

	gr2[i]=new TGraphErrors(20,p_all[i],eff_p[i],err_p_all[i],err_eff_p[i]);
	gr2[i]->SetMarkerStyle(20);
	gr2[i]->SetMarkerColor(4);
	gr2[i]->SetLineWidth(2);
	gr2[i]->SetLineColor(4);
	gr2[i]->SetTitle("Efficiency&Momentum");
	gr2[i]->GetXaxis()->SetTitle("P, MeV/c");
	gr2[i]->GetYaxis()->SetTitle("Efficiency");
	gr2[i]->Write("gr2");

	gr3[i]=new TGraphErrors(20,p_all[i],eff_k[i],err_p_all[i],err_eff_k[i]);
	gr3[i]->SetMarkerStyle(20);
	gr3[i]->SetMarkerColor(2);
	gr3[i]->SetLineWidth(2);
	gr3[i]->SetLineColor(2);
	gr3[i]->SetTitle("Efficiency&Momentum");
	gr3[i]->GetXaxis()->SetTitle("P, MeV/c");
	gr3[i]->GetYaxis()->SetTitle("Efficiency");
	gr3[i]->Write("gr3");

	mg1[i]->SetMaximum(1.2);
	mg1[i]->SetMinimum(0);
	mg1[i]->Add(gr1[i]);
	mg1[i]->Add(gr2[i]);
	mg1[i]->Add(gr3[i]);
	mg1[i]->SetTitle("Efficiency&Momentum; P, MeV/c; Efficiency");
	mg1[i]->Draw("ap");
	gPad->Modified(); gPad->Update();
	mg1[i]->GetYaxis()->SetLimits(0, 1.2);
	mg1[i]->SetMinimum(0.);
	mg1[i]->SetMaximum(1.2);
	mg1[i]->Write("mg1");
	gPad->Modified(); gPad->Update();

	c.cd(8);
	gPad->SetGrid();
	mg2[i] = new TMultiGraph();
	gr4[i]=new TGraphErrors(20,p_all[i],not_eff_p[i],err_p_all[i],0);
	gr4[i]->SetMarkerStyle(20);
	gr4[i]->SetMarkerColor(4);
	gr4[i]->SetLineWidth(2);
	gr4[i]->SetLineColor(4);
        gr4[i]->Write("gr4");

	gr5[i]=new TGraphErrors(20,p_all[i],not_eff_k[i],err_p_all[i],0);
	gr5[i]->SetMarkerStyle(20);
	gr5[i]->SetMarkerColor(2);
	gr5[i]->SetLineWidth(2);
	gr5[i]->SetLineColor(2);
        gr5[i]->Write("gr5");

	mg2[i]->SetMaximum(1.2);
	mg2[i]->SetMinimum(0);
	mg2[i]->Add(gr4[i]);
	mg2[i]->Add(gr5[i]);
	mg2[i]->SetTitle("K efficiency and #pi Misidentification; P, MeV/c; K efficiency and #pi Misidentification");
	mg2[i]->Draw("ap");
	gPad->Modified(); gPad->Update();
	mg2[i]->GetYaxis()->SetLimits(0., 1.2);
	mg2[i]->SetMinimum(0.);
	mg2[i]->SetMaximum(1.2);
        mg2[i]->Write("mg2");
	gPad->Modified(); gPad->Update();
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
	gr6[i]->Draw("ap");
	gPad->Modified(); gPad->Update();
	gr6[i]->GetYaxis()->SetLimits(0., 5.);
	gr6[i]->SetMinimum(0.);
	gr6[i]->SetMaximum(5.0);
	gPad->Modified(); gPad->Update();

	c.cd(10);
	h5[i]->Draw();

	c.Update();
	//c.Print(KEDR + "/" + TString::Format("cnt_%d.png",i).Data());
	c.SaveAs(KEDR + "/" + TString::Format("cnt_%d.png",i).Data());

	for(int ii=5; ii<=16; ii++)
	{
	    ofstream of(KEDR + "/" + TString::Format("cnt_thick_%d_npetrh%f_eff_ineff_p%d.dat",i,Npethr,ii).Data(),ios::out);
	    of<<"\t"<<Npethr<<"\t"<<"\t"<<eff_p[i][ii]<<"\t"<<eff_k[i][ii]<<"\t"<<not_eff_p[i][ii]<<"\t"<<not_eff_k[i][ii]<<"\t"<<Ksigma[i][ii]<<endl;
	    of.close();
	}

	mu_0 = myfit1[i]->GetParameter(0);
	mu_max = myfit1[i]->GetParameter(1);
	p_thr = myfit1[i]->GetParameter(2);
        p_cal = 1200.;
        mu_cal = mu_0+mu_max*((pow(p_cal,2)-pow(p_thr,2))/pow(p_cal,2));

	h6->Fill(mu_cal);
	if( i>=20 && i<60 )
	{
	    h7->Fill(mu_cal);
	}
	else
	{
	    h8->Fill(mu_cal);
	}
    }

    TCanvas c1("c1","c1",600,600);
    c1.cd();
    gROOT->SetStyle("Plain");
    h6->SetTitle("; N_{ph.e.}; Number of counters");
    h6->GetXaxis()->SetTitleOffset(1.2);
    h6->GetYaxis()->SetTitleOffset(1.2);
    h6->SetFillColor(kRed);
    h6->Draw();
    c1.Update();
    c1.SaveAs(KEDR + "/" + "Nphe.png");

    TCanvas c2("c2","c2",600,600);
    c2.cd();
    gROOT->SetStyle("Plain");
    h7->SetTitle("Barrel; N_{ph.e.}; Number of counters");
    h7->GetXaxis()->SetTitleOffset(1.2);
    h7->GetYaxis()->SetTitleOffset(1.2);
    h7->SetFillColor(kRed);
    h7->Draw();
    c2.Update();
    c2.SaveAs(KEDR + "/" + "Nphe_barrel.png");

    TCanvas c3("c3","c3",600,600);
    c3.cd();
    gROOT->SetStyle("Plain");
    h8->SetTitle("Endcap; N_{ph.e.}; Number of counters");
    h8->GetXaxis()->SetTitleOffset(1.2);
    h8->GetYaxis()->SetTitleOffset(1.2);
    h8->SetFillColor(kRed);
    h8->Draw();
    c3.Update();
    c3.SaveAs(KEDR + "/" + "Nphe_endcap.png");

    fout->Write();
    fout->Close();
}
