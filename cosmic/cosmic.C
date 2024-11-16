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
    TString dir_out="/store/users/ovtin/cosmruns/results";
    KEDR = dir_out + "/" + "cosm_stability";
    gSystem->Exec("mkdir "+ KEDR);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ KEDR);
    gSystem->Exec("ln -s "+ KEDR + " /home/ovtin/public_html/atc_cosmic");
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

    char name0[161], name1[161], name2[161], name3[161];
    TProfile* pr[160];
    TProfile* pr1[160];
    TProfile* pr2[160];
    TProfile* pr3[160];
    for (int j=0; j<160; j++)
    {
	sprintf(name0,"Cnt%d",j);
	sprintf(name1,"Run_Cnt%d",j);
	sprintf(name2,"Cnt%d_shifter",j);
	sprintf(name3,"Run_Cnt%d_shifter",j);
	//pr[j]=new TProfile(name0,name0,1000,1400778000,1603714452,0,200);
	//pr1[j]=new TProfile(name1,name1,9969,19635,29604,0,200);
	pr[j]=new TProfile(name0,name0,1000,1400778000,1680870067,0,200);   //2023
	pr1[j]=new TProfile(name1,name1,12330,19635,31965,0,200);           //2023
	//pr[j]=new TProfile(name0,name0,1000,1400778000,1672018085,0,200);   //2022
	//pr1[j]=new TProfile(name1,name1,11904,19635,31539,0,200);           //2022
	//pr2[j]=new TProfile(name2,name2,1000,1400778000,1603714452,0,200);
	//pr3[j]=new TProfile(name3,name3,9969,19635,29604,0,200);
	pr2[j]=new TProfile(name2,name2,1000,1400778000,1680870067,0,200);   //2023
	pr3[j]=new TProfile(name3,name3,12330,19635,31965,0,200);            //2023
	//pr2[j]=new TProfile(name2,name2,1000,1400778000,1672018085,0,200);   //2022
	//pr3[j]=new TProfile(name3,name3,11904,19635,31539,0,200);            //2022
    }

    int runprev=0;
    int NumRun=0;

    //event loop
    for(int k=0; k<nentr; k++)
    {
	if(k>0) { tt->GetEntry(k-1); runprev=bcosm.run; }

	tt->GetEntry(k);
	if( (k %100000)==0 )cout<<k<<endl;

        //skip bad runs from next list
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
	if( bcosm.run==27642 || bcosm.run==27643 || bcosm.run==27644 ) continue;                       //2018
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
	if( bcosm.run==31317 || bcosm.run==31324 || bcosm.run==31325 ) continue;
	if( bcosm.run==30760 || bcosm.run==30819 ) continue;
	if( bcosm.run>=30830 && bcosm.run<=30840 ) continue;
	if( bcosm.run>=29530 && bcosm.run<=29540 ) continue;
	if( bcosm.run>=29550 && bcosm.run<=29604 ) continue;
	if( bcosm.run>=31478 && bcosm.run<=31522 ) continue;

        //2023
	//if( bcosm.run>=31709 && bcosm.run!=31710 && bcosm.run!=31712 && bcosm.run!=31714 ) continue;    //test

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
		//for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.single_aerogel_REGION5[i]==1 && bcosm.wlshit[i]!=1 )
		for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.aerogel_REGION[i]==1 && bcosm.wlshit[i]!=1 ) //aerogel
		{
		    pr[j]->Fill(runTime_begin,bcosm.npe[i]);
		    pr1[j]->Fill(bcosm.run,bcosm.npe[i]);
		}
		for( int j=0; j<160; j++) if( j==bcosm.cnt[i] && bcosm.wlshit[i]==1 )  //shifter
		{
		    pr2[j]->Fill(runTime_begin,bcosm.npe[i]);
		    pr3[j]->Fill(bcosm.run,bcosm.npe[i]);
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
        //myfit[i] = new TF1(namefit,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
        myfit[i] = new TF1(namefit,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067); //2023
        //myfit[i] = new TF1(namefit,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1672018085);   //2022
	myfit[i]->SetParLimits(0,0,10000);
	myfit[i]->SetParLimits(1,500000,1e12);
	myfit[i]->SetParLimits(2,0,10000);
	myfit[i]->SetParNames("p0","#tau","Const. level");
	myfit[i]->SetLineColor(kBlue);
	gStyle->SetOptFit(kTRUE);
	//pr[i]->Fit(namefit,"","",1400778000,1603714452);
	pr[i]->Fit(namefit,"","",1400778000,1680870067); //2023
	//pr[i]->Fit(namefit,"","",1400778000,1672018085);   //2022
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
	pr[i]->GetXaxis()->SetNdivisions(205);
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
	pr1[i]->GetXaxis()->SetNdivisions(205);
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

    TCanvas c2("c2","c2",1400,800);
    c2.cd();
    char namefit2[161];
    TF1* myfit2[161];
    for(int i=0; i<160; i++)
    {
       	sprintf(namefit2,"myfit2",i);
        //myfit2[i] = new TF1(namefit2,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
        myfit2[i] = new TF1(namefit2,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067); //2023
        //myfit2[i] = new TF1(namefit2,"[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1672018085);   //2022
	myfit2[i]->SetParLimits(0,0,10000);
	myfit2[i]->SetParLimits(1,500000,1e12);
	myfit2[i]->SetParLimits(2,0,10000);
	myfit2[i]->SetParNames("p0","#tau","Const. level");
	myfit2[i]->SetLineColor(kBlue);
	gStyle->SetOptFit(kTRUE);
	//pr2[i]->Fit(namefit2,"","",1400778000,1603714452);
	pr2[i]->Fit(namefit2,"","",1400778000,1680870067); //2023
	//pr2[i]->Fit(namefit2,"","",1400778000,1672018085);   //2022
	pr2[i]->SetMarkerStyle(20);
	pr2[i]->SetMarkerSize(0.5);
	pr2[i]->SetMarkerColor(1);
	pr2[i]->SetLineWidth(2);
	pr2[i]->SetLineColor(1);
	pr2[i]->GetXaxis()->SetTimeDisplay(1);
	pr2[i]->GetXaxis()->SetTimeFormat("%d/%m/%y%F1970-01-01 00:00:00");
	pr2[i]->GetXaxis()->SetTitleSize(0.05);
	pr2[i]->GetXaxis()->SetTitleOffset(1.0);
	pr2[i]->GetXaxis()->SetLabelSize(0.05);
	pr2[i]->GetXaxis()->SetNdivisions(205);
	pr2[i]->GetYaxis()->SetTitleSize(0.05);
	pr2[i]->GetYaxis()->SetTitleOffset(1.00);
	pr2[i]->GetYaxis()->SetLabelSize(0.05);
	pr2[i]->GetYaxis()->SetNdivisions(205);
	pr2[i]->GetYaxis()->SetDecimals();
	pr2[i]->GetXaxis()->SetTitle("Time(d/m/y)");
	pr2[i]->GetYaxis()->SetTitle("N_{ph.e.}");
	pr2[i]->SetTitle("");
	pr2[i]->Draw();
	c2.Update();
	c2.Print(KEDR + "/" + TString::Format("cnt_%d_shifter.png",i).Data());
    }

    TCanvas c3("c3","c3",1400,800);
    c3.cd();
    for(int i=0; i<160; i++)
    {
	pr3[i]->SetMarkerStyle(20);
	pr3[i]->SetMarkerSize(0.5);
	pr3[i]->SetMarkerColor(1);
	pr3[i]->SetLineWidth(2);
	pr3[i]->SetLineColor(1);
	pr3[i]->GetXaxis()->SetTitleSize(0.05);
	pr3[i]->GetXaxis()->SetTitleOffset(1.0);
	pr3[i]->GetXaxis()->SetLabelSize(0.05);
	pr3[i]->GetXaxis()->SetNdivisions(205);
	pr3[i]->GetYaxis()->SetTitleSize(0.05);
	pr3[i]->GetYaxis()->SetTitleOffset(1.00);
	pr3[i]->GetYaxis()->SetLabelSize(0.05);
	pr3[i]->GetYaxis()->SetNdivisions(205);
	pr3[i]->GetYaxis()->SetDecimals();
	pr3[i]->GetXaxis()->SetTitle("Run");
	pr3[i]->GetYaxis()->SetTitle("N_{ph.e.}");
	pr3[i]->SetTitle("");
	pr3[i]->Draw();
	c3.Update();
	c3.Print(KEDR + "/" + TString::Format("runs_cnt_%d_shifter.png",i).Data());
    }

    fout->Write();
    fout->Close();

    cout<<"NumRun="<<NumRun<<endl;
}
