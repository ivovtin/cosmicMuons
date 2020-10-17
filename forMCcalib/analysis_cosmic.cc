#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "ReadNat/re_def.h"
#include "ReadNat/ss_def.h"
#include "VDDCRec/ktracks.h"
#include "VDDCRec/mtofhits.h"
#include "VDDCRec/ToFTrack.hh"
#include "KrToF/tof_system.h"
#include "KEmcRec/emc_struct.h"
#include "KrAtc/atcrec.h"
#include "KrAtc/atc_to_track.h"
#include "KrVDDCMu/dcmu.h"
#include "KrMu/mu_system.h"
#include "KrMu/mu_event.h"

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TBenchmark.h"

#include "KaFramework/kframework.h"
#include "KaFramework/eventbr.h"
#include "KaFramework/vddcbr.h"
#include "KaFramework/emcbr.h"
#include "KaFramework/atcbr.h"
#include "KaFramework/tofbr.h"
#include "KaFramework/mubr.h"

#include "KrAtc/AtcHit.hh"

using namespace std;

static const char* progname;

struct ProgramParameters {
	bool read_reco;
	int min_track_number;                 //миниммальное число треков
	int max_track_number;                 //максимальное число треков
	int min_beam_track_number;            //миниммальное число пучковых треков
	int min_ip_track_number;              //миниммальное число треков из точки взаимодействия
        float min_momentum;                   //минимальный импульс
        float max_momentum;                   //максимальный импульс
	float min_cluster_energy; //MeV       //минимальная энергия кластера
	float min_total_energy; //MeV         //минимальная общая энергия
	int min_cluster_number;               //минимальное кол-во кластеров
	int min_lkrcl_number;
	int min_csicl_number;
	const char* rootfile;
	int MCCalibRunNumber;                  //19862 - номер захода - загрузка из БД калибровок при обработке файла моделирования в заданном интервале заходов
	int NEvents;                          //число обрабатываемых событий
	int kdisp_ev_cut;
	bool process_only;       //process one event only
};

//static const struct ProgramParameters def_progpar={false,0,0,0,0,10000,0,0,1,0,0,0,"/store/users/ovtin/out.root",false};   //cosmic
//static const struct ProgramParameters def_progpar={false,0,1,0,0,0,3100,20,40,2,0,0,"/store/users/ovtin/out.root",false};   //cosmic
//static const struct ProgramParameters def_progpar={false,0,1,0,0,0,3100,10,10,0,0,0,"/store/users/ovtin/out.root",19862,false};   //25092017
//static const struct ProgramParameters def_progpar={false,0,1,0,0,0,3100,50,100,2,0,0,"/store/users/ovtin/out.root",19862,0,0,false};   //sim-test 22112016
static const struct ProgramParameters def_progpar={false,0,1,0,0,0,100000,50,100,2,0,0,"/store/users/ovtin/out.root",19862,0,0,false};   //sim-test 22112016

static struct ProgramParameters progpar(def_progpar);

enum {
	CutLongDcRecord=1,
	CutLongVdRecord,
        AtcEventDamageCut,
        AtcIllegalTrackCut,
//      NoAtcOnTrackCut,
        CutTotalEnergyFast,
	CutTrackNumber_min,
	CutTrackNumber_max,
	CutBeamTrackNumber,
	CutIPTrackNumber,
	CutTotalEnergy,
	CutClusterNumber,
	CutLkrClusterNumber,
	CutCsiClusterNumber,
    OneTrackCut,
    MinMomentumCut,
    MaxMomentumCut,
    MinXYVectorsCut,
    MinZVectorsCut,
    Chi2Cut,
    TofCut,
    MUCut,
};

static TTree *eventTree, *trackTree, *emcTowerTree, *lkrStripTree, *lkrStripTrackTree, *ATCTree;             //создаем деревья
static struct EventBranch bevent;
static struct VertexBranch bvertex;
static struct TrackBranch btrack;
static struct ToFBranch btof;
static struct MUBranch bmu;
static struct EMCBranch bemc;
static struct TowerClusterBranch bcluster[2];  //[2]
static struct StripClusterBranch bstrip;
static struct StripTrackBranch bstriptrack;

//static struct ATCCounterBranch bcnt[160];
static struct ATCCounterBranch bcnt[9];
//static struct ATCCounterBranch bcnt;                                                     // создаем АЧС структуру для счетчика
static struct ATCBranch batc;

//Rejection prior to reconstruction helps to save CPU time
extern "C" void kemc_energy_(float Ecsi[2],float *Elkr);

int pre_event_rejection()                                                                    //предварительный отброс события
{
    //maximum track number supposed to be 7
	//maximum record size for DC: Ntracks*maxNhits*HitLength + 10% (spare)
	static const int maxDClen=1940; //=1940
	//maximum record size for VD: Ntracks*maxNhits*Length + 2 (for dT) + 10% (spare)
	static const int maxVDlen=156; //=156

	if( RawLength(SS_DC)>maxDClen ) return CutLongDcRecord; //too long DC event

	if( RawLength(SS_VD)>maxVDlen ) return CutLongVdRecord; //too long VD event

	float Ecsi[2], Elkr;
	kemc_energy_(Ecsi,&Elkr);

    //Total energy deposition should be more than progpar.min_total_energy
	if( Ecsi[0]+Ecsi[1]+Elkr<progpar.min_total_energy ) return CutTotalEnergyFast;

	return 0;
}

int vddc_event_rejection()
{
	if( eTracksAll<progpar.min_track_number )       return CutTrackNumber_min;
	if( eTracksAll>progpar.max_track_number )       return CutTrackNumber_max;
	if( eTracksBeam<progpar.min_beam_track_number ) return CutBeamTrackNumber;
	if( eTracksIP<progpar.min_ip_track_number )     return CutIPTrackNumber;

    if( tP(0)<=progpar.min_momentum )       return MinMomentumCut;
    if( tP(1)<=progpar.min_momentum )       return MinMomentumCut;
    if( tP(0)>progpar.max_momentum )       return MaxMomentumCut;
    if( tP(1)>progpar.max_momentum )       return MaxMomentumCut;

    /*    if( tVectorsXY(0)<3 )                   return MinXYVectorsCut;
    if( tVectorsZ(0)<2 )                    return MinZVectorsCut;
    */
    if( tCh2(0)>150 )               return Chi2Cut;

	return 0;
}

int tof_event_rejection()
{
    //if( kschit_.time_ns[0]<-4 || kschit_.time_ns[0]>4)         return TofCut;
    //if( kschit_.time_ns[1]<-4 || kschit_.time_ns[1]>4)         return TofCut;

    return 0;
}

int emc_event_rejection()
{
	int nemc=0, nlkr=0, ncsi=0;
	float tot_energy=0;

	for(int c=0; c<semc.emc_ncls; c++) {
		tot_energy+=semc.emc_energy[c];
		if( semc.emc_energy[c]>progpar.min_cluster_energy ) {
			nemc++;
			if( semc.emc_type[c]==1 )
				nlkr++;
			else
				ncsi++;
		}
	}

	if( tot_energy<progpar.min_total_energy ) return CutTotalEnergy;
	if( nemc<progpar.min_cluster_number )     return CutClusterNumber;
	if( nlkr<progpar.min_lkrcl_number )       return CutLkrClusterNumber;
	if( ncsi<progpar.min_csicl_number )       return CutCsiClusterNumber;

	return 0;
}

int atc_rejection()           //отброс событий в АЧС
{
    //ATC raw record damaged in any way (including when DeltaT is absent or out of range)     //сырые данные повреждены
    if( atc_rec.eventdamage ) return AtcEventDamageCut;

    //track determined as illegal by atcrec                                                   //трек не определяется в atcrec
    if( atc_track.illtrack[0] ) return AtcIllegalTrackCut;

    //no counters on tracks, very strange if occurs                                           //нет счетчика на трек
//    if( atc_track.ncnt_on_track[0]==0 ) return NoAtcOnTrackCut;

    return 0;
}

int mu_event_rejection()
{
    if( mu_next_event()<=0 ) return MUCut;    //отбрасываем если нет срабатывания в мюонной системы

    return 0;
}

int analyse_event()                                                    //анализ события
{
//    if( eTracksAll==2 && tVectorsZ(0)>=1 && tVectorsZ(1)>=1 && kdcrec_.NHITS<160 && tHitsVD(0)>1 && tVertex(0)==1 && tVertex(1)==1 && CPhi2t<0 && CThe2t<0 && (tCharge(0)+tCharge(1))==0 && ktrrec_.TrackNcls[0]>0&&ktrrec_.TrackNcls[1]>0&&tP(0)<5000&&tP(1)<5000)
//    if(eTracksAll==2)  //baba-events
       if(eTracksAll==1)    //cosmic                      // eTracksAll - число реконструированных треков на событие, emc_ncls - число кластеров
       {
//	   cout<<"eTracksAll="<<eTracksAll<<endl;

	   copy(&bevent);                                //событие
	   bevent.event=kdcenum_.EvNum; //take succesive event number from VDDCRec
	   unsigned short nhits=mu_next_event();

	   for(int t=0; t<eTracksAll; t++)                            //цикл по числу треков  - заполняем однотрековыми
	   {
		if(sqrt(pow(tX0(t),2)+pow(tY0(t),2)+pow(tZ0(t),2))<40)    {          //cut area in DC       40 - радиус сферы в см
//              	cout<<"Event <30"<<endl;
//             	cout<<"tX0(t)="<<tX0(t)<<"   tY0(t)="<<tY0(t)<<"    tZ0(t)="<<tZ0(t)<<endl;   //coordinates begin track

		copy(&btrack,t);
		copy(&bvertex);
 		copy(&btof,t);
		//copy(&bemc,t);
		copy(&bemc);
		copy(&bmu,t,nhits);
		copy(&batc);
//		atc_to_track(t);
//		cout<<"Number of counters crossed by the track atctrackinfo.ncnt="<<atctrackinfo.ncnt<<"\t"<<endl;
/*                for(int k=0; k<atctrackinfo.ncnt; k++)                      //цикл по числу пересеченных счетчиков на трек
		{
//                cout<<"Track="<<t<<"\t"<<"atctrackinfo.cnt="<<atctrackinfo.cnt[k]-1<<"\n"<<endl;
//		copy(&bcnt[atctrackinfo.cnt[k]-1],atctrackinfo.cnt[k]-1,t);
		copy(&bcnt[k],atctrackinfo.cnt[k]-1,t);
		}
*/
/*
		    for(int i=0; i<NATC; i++) {                          //чтобы заполнить ветку для всех счетчиков АЧС
		    copy(&bcnt[i],i,t);
		    }
*/
/*		copy(&bcnt[0],0,t);
		copy(&bcnt[1],0,t);
		copy(&bcnt[2],0,t);
		copy(&bcnt[3],0,t);
		copy(&bcnt[4],0,t);
		copy(&bcnt[5],0,t);
		copy(&bcnt[6],0,t);
		copy(&bcnt[7],0,t);
		copy(&bcnt[8],0,t);
		copy(&bcnt[9],0,t);
*/
//		for(int i=0; i<atc_track.ncnt_on_track[t]; i++)                    //цикл по числу пересеченных счетчиков на трек
		for(int i=0; i<9; i++)                    //цикл по числу пересеченных счетчиков на трек
		{
//                    cout<<"Event="<<kdcenum_.EvNum<<"\t"<<"Track="<<t<<"\t"<<"i="<<i<<"\t"<<"atc_track.cnt_cross[t][i]="<<atc_track.cnt_cross[t][i]<<"\t"<<"atc_rec.npe="<<atc_rec.npe[atc_track.cnt_cross[t][i]-1]<<endl;
//		    copy(&bcnt[atc_track.cnt_cross[t][i]-1],(atc_track.cnt_cross[t][i]-1),t);
                    //atc_track.cnt_cross[t][i]-1-список пересеченных счетчиков
		    copy(&bcnt[i],(atc_track.cnt_cross[t][i]-1),t);
//		ATCTree->Fill();
		}

		trackTree->Fill();
		eventTree->Fill();

		for(int c=0; c<semc.dc_emc_ncls[t]; c++)
		{                                                             //dc_emc_ncls[NDCH_TRK] - число кластеров emc, соответствующих данному треку
		    copy(&bcluster[c],(semc.dc_emc_cls[t][c]-1)); //?????     //dc_emc_cls[NDCH_TRK][NEMC_CLS] - номера таких кластеров
		}
		emcTowerTree->Fill();
/*
		copy(&batc);
		for(int i=0; i<NATC; i++) {                          //чтобы заполнить ветку для всех счетчиков АЧС
		    copy(&bcnt[i],i);
		}
		ATCTree->Fill();
*/
		for(int c=0; c<semc.str_ncls; c++) {
		    copy(&bstrip,c);
		}
		lkrStripTree->Fill();

		for(int t=0; t<semc.str_ntracks; t++) {
		    copy(&bstriptrack,t);
		}
		lkrStripTrackTree->Fill();
             }

	    }
	    if( progpar.process_only ) return 1;
	}
	return 0;
}


//static const char* optstring="ra:b:p:t:e:c:k:i:o:v:x";
static const char* optstring="ra:h:b:p:s:w:t:e:c:k:i:o:v:n:l:x";

void Usage(int status)
{                   //static const struct ProgramParameters def_progpar={false,1,0,0,50,300,1,0,0,"/store/users/ovtin/out.root",false};
	cout.precision(3);
	cout<<"Usage:\n"
		<<progname<<" [OPTIONS] run|nat_file|run_list_file...\n\n"
		<<"Reconstruction of cosmic events and search of hits in ATC system.\n"
		<<"Options:\n"
		<<"  -r             Read VDDC&EMC reconstructed info from file (reconstruct by default)\n"
		<<"  -a tracks      Minimum total tracks number required (default to "<<def_progpar.min_track_number<<")\n"
		<<"  -h tracks      Maximum total tracks number required (default to "<<def_progpar.max_track_number<<")\n"
		<<"  -b tracks      Minimum beam tracks number required (default to "<<def_progpar.min_beam_track_number<<")\n"
		<<"  -p tracks      Minumum IP tracks number required (default to "<<def_progpar.min_ip_track_number<<")\n"
            	<<"  -s Momentum      Lower limit of momentum cut, MeV/c (default: "<<def_progpar.min_momentum<<")\n"
            	<<"  -w Momentum      Maximum limit of momentum cut, MeV/c (default: "<<def_progpar.max_momentum<<")\n"
 	        <<"  -t energy      EMC cluster energy threshold (default to "<<def_progpar.min_cluster_energy<<" MeV)\n"
 		<<"  -e energy      Minumum total energy in EMC (default to "<<def_progpar.min_total_energy<<" MeV)\n"
		<<"  -c clusters    Minumum number of clusters in both calorimeters (default to "<<def_progpar.min_cluster_number<<")\n"
		<<"  -k clusters    Minumum number of clusters in LKr calorimeter (default to "<<def_progpar.min_lkrcl_number<<")\n"
		<<"  -i clusters    Minumum number of clusters in CsI calorimeter (default to "<<def_progpar.min_csicl_number<<")\n"
	        <<"  -o RootFile    Output ROOT file name (default to "<<def_progpar.rootfile<<")\n"
	        <<"  -v MCCalibRunNumber    MCCalibRunNumber (default to "<<def_progpar.MCCalibRunNumber<<")\n"
                <<"  -n NEvents     Number events in process "<<def_progpar.NEvents<<"\n"
                <<"  -l KD_ev_cut   KDisplay draw event's from event "<<def_progpar.kdisp_ev_cut<<"\n"
	        <<"  -x             Process the events specified after file exclusively and print debug information"
		<<endl;
	exit(status);
}

int main(int argc, char* argv[])
{
	progname=argv[0];

	//If no arguments print usage help
	if( argc==1 ) Usage(0);

	int opt;
//----------------- Process options -----------------//
	while( (opt=getopt(argc,argv,optstring))>0 ) {
		switch( opt ) {
			case '?': Usage(1); break;
			case 'r': progpar.read_reco=true; break;
			case 'a': progpar.min_track_number=atoi(optarg); break;
			case 'h': progpar.max_track_number=atoi(optarg); break;
			case 'b': progpar.min_beam_track_number=atoi(optarg); break;
			case 'p': progpar.min_ip_track_number=atoi(optarg); break;
			case 's': progpar.min_momentum=atof(optarg); break;
			case 'w': progpar.max_momentum=atof(optarg); break;
			case 't': progpar.min_cluster_energy=atof(optarg); break;
			case 'e': progpar.min_total_energy=atof(optarg); break;
			case 'c': progpar.min_cluster_number=atoi(optarg); break;
			case 'k': progpar.min_lkrcl_number=atoi(optarg); break;
			case 'i': progpar.min_csicl_number=atoi(optarg); break;
			case 'o': progpar.rootfile=optarg; break;
		        case 'v': progpar.MCCalibRunNumber=atoi(optarg); break;
                        case 'n': progpar.NEvents=atoi(optarg); break;
                        case 'l': progpar.kdisp_ev_cut=atoi(optarg); break;
			case 'x': progpar.process_only=true; break;
			default : Usage(1);
		}
	}

	if( progpar.min_track_number<progpar.min_beam_track_number ||
		progpar.min_beam_track_number<progpar.min_ip_track_number ) {
		cerr<<"Error in parameters specification, should be MinTotalTracks>=MinBeamTracks>=MinIPTracks"<<endl;
		return 1;
	}


	char timestr[161];
	time_t curtime=time(NULL);

	strftime(timestr,161,"%F %T %z",localtime(&curtime));
	cout<<"Current time "<<timestr<<endl;
	cout<<" Writing output trees to "<<progpar.rootfile<<endl;


//----------------- Initialize ROOT file and trees -----------------//
	TFile *fout=0;

	//Create root file if exclusive event processing is not set
	if( !progpar.process_only )
		fout = new TFile(progpar.rootfile,"RECREATE");        //fout = new TFile(progpar.rootfile,"RECREATE","",7);

	eventTree = new TTree("et","Event tree");
	eventTree->Branch("ev",&bevent,eventBranchList);
        eventTree->Branch("vrt",&bvertex,vertexBranchList);
	eventTree->Branch("emc",&bemc,emcBranchList);
     	eventTree->Branch("atc",&batc,atcBranchList);                              //!!!!!!

	trackTree = new TTree("tt","Track tree");
	trackTree->Branch("ev",&bevent,eventBranchList);
	trackTree->Branch("vrt",&bvertex,vertexBranchList);
	trackTree->Branch("emc",&bemc,emcBranchList);
	trackTree->Branch("t",&btrack,trackBranchList);
	trackTree->Branch("tof",&btof,ToFBranchList);
	trackTree->Branch("mu",&bmu,MUBranchList);
	trackTree->Branch("atc",&batc,atcBranchList);
/*        for(int i=0; i<NATC; i++) {
	char branchname[161];
        sprintf(branchname,"cnt%d",i);
	trackTree->Branch(branchname,&bcnt[i],atcCounterBranchList);
	}
*/
	for(int i=0; i<9; i++) {
	char branchname[9];
//        sprintf(branchname,"crcnt%d",i);
        sprintf(branchname,"cnt%d",i);
	trackTree->Branch(branchname,&bcnt[i],atcCounterBranchList);
	}

/*
	ATCTree = new TTree("ATC","Event tree");
	ATCTree->Branch("atc",&batc,atcBranchList);                                           //!!!!!!
        for(int i=0; i<NATC; i++) {                          //чтобы заполнить ветку для всех счетчиков
//        for(int i=109; i<110; i++) {
	char branchname[161];
        sprintf(branchname,"cnt%d",i);
	ATCTree->Branch(branchname,&bcnt[i],atcCounterBranchList);
	}
*/
	emcTowerTree = new TTree("emct","EMC tower cluster tree");
	emcTowerTree->Branch("ev",&bevent,eventBranchList);
	emcTowerTree->Branch("emc",&bemc,emcBranchList);
	emcTowerTree->Branch("vrt",&bvertex,vertexBranchList);
	for (int i=0; i<2; i++)
	{
	    char clastername[3];
            sprintf(clastername,"c%d",i);
	    emcTowerTree->Branch(clastername,&bcluster[i],towerClusterBranchList);
	}


	lkrStripTree = new TTree("strclt","LKr strip cluster tree");
	lkrStripTree->Branch("strcls",&bstrip,stripClusterBranchList);

	lkrStripTrackTree = new TTree("strtrt","LKr strip cluster tree");
	lkrStripTrackTree->Branch("strtrk",&bstriptrack,stripTrackBranchList);

//----------------- Configure kframework -----------------//
//        gRun->RunNumber()=19862;        1/06/2017

	//Set kframework signal handling
	kf_install_signal_handler(1);

	//Set subsystems to be used
//	kf_use(KF_VDDC_SYSTEM|KF_EMC_SYSTEM);
        kf_use(KF_VDDC_SYSTEM|KF_TOF_SYSTEM|KF_ATC_SYSTEM|KF_EMC_SYSTEM|KF_MU_SYSTEM);                             //устанавливаем, какие будем использовать системы

	//Register to kframework used cuts
	char buf[100];
	kf_add_cut(KF_PRE_SEL,CutLongDcRecord,"DC event length >1940 words");
	kf_add_cut(KF_PRE_SEL,CutLongVdRecord,"VD event length >156 words");


	//    kf_add_cut(KF_VDDC_SEL,OneTrackCut,"1 cosmic track");
	//char buf[200];
	sprintf(buf,"momentum <= %5.fMeV/c",progpar.min_momentum);
	kf_add_cut(KF_VDDC_SEL,MinMomentumCut,buf);
	sprintf(buf,"momentum > %5.fMeV/c",progpar.max_momentum);
	kf_add_cut(KF_VDDC_SEL,MaxMomentumCut,buf);
	/* kf_add_cut(KF_VDDC_SEL,MinXYVectorsCut,"XY vectors <3");
	 kf_add_cut(KF_VDDC_SEL,MinZVectorsCut,"Z vectors <2");
        */
	 //kf_add_cut(KF_VDDC_SEL,Chi2Cut,"bad chi-square");


	sprintf(buf,"raw total EMC energy <%.3gMeV",progpar.min_total_energy);
	kf_add_cut(KF_PRE_SEL,CutTotalEnergyFast,buf);
	sprintf(buf,"total number of tracks <%d",progpar.min_track_number);
	kf_add_cut(KF_VDDC_SEL,CutTrackNumber_min,buf);
	sprintf(buf,"total number of tracks >%d",progpar.max_track_number);
	kf_add_cut(KF_VDDC_SEL,CutTrackNumber_max,buf);
	sprintf(buf,"number of beam tracks <%d",progpar.min_beam_track_number);
	kf_add_cut(KF_VDDC_SEL,CutBeamTrackNumber,buf);
	sprintf(buf,"number of IP tracks <%d",progpar.min_ip_track_number);
	kf_add_cut(KF_VDDC_SEL,CutIPTrackNumber,"IP tracks cut");

	kf_add_cut(KF_ATC_SEL,AtcEventDamageCut,"damaged record");
	kf_add_cut(KF_ATC_SEL,AtcIllegalTrackCut,"illegal track");
	//    kf_add_cut(KF_ATC_SEL,NoAtcOnTrackCut,"no counters on track");

	sprintf(buf,"total EMC energy <%.3gMeV",progpar.min_total_energy);
	kf_add_cut(KF_EMC_SEL,CutTotalEnergy,buf);
	sprintf(buf,"total number of clusters <%d",progpar.min_cluster_number);
	kf_add_cut(KF_EMC_SEL,CutClusterNumber,buf);
	sprintf(buf,"number of LKr clusters <%d",progpar.min_lkrcl_number);
	kf_add_cut(KF_EMC_SEL,CutLkrClusterNumber,buf);
	sprintf(buf,"number of CsI clusters <%d",progpar.min_csicl_number);
	kf_add_cut(KF_EMC_SEL,CutCsiClusterNumber,buf);

	kf_add_cut(KF_TOF_SEL,TofCut,"TOF system cut");
        kf_add_cut(KF_MU_SEL,MUCut,"Mu system cut");

	//Register selection routines
	kf_register_selection(KF_PRE_SEL,pre_event_rejection);
	kf_register_selection(KF_VDDC_SEL,vddc_event_rejection);
	kf_register_selection(KF_TOF_SEL,tof_event_rejection);
	kf_register_selection(KF_ATC_SEL,atc_rejection);
	kf_register_selection(KF_EMC_SEL,emc_event_rejection);
	kf_register_selection(KF_MU_SEL,mu_event_rejection);

	kf_MCCalibRunNumber(progpar.MCCalibRunNumber);
        kf_kdisplay_cut(progpar.kdisp_ev_cut);
	//Set automatic cosmic run determination
//	kf_cosmic(-1);  //auto
	kf_cosmic(1);  //cosmic  >0
//	kf_cosmic(0);  //beam

	//Register an analysis routine
	kf_register_analysis(analyse_event);
        cout<<"analyse_event="<<analyse_event<<endl;

	//Do not reconstruct, read reconstruction records from file
	kf_reco_from_file(progpar.read_reco);

	//Set exclusive event processing
	kf_process_only(progpar.process_only);
	//kf_display_all(5000);
        //kf_set_display(5000,1,1.0);

	TBenchmark *benchmark=new TBenchmark;
	//cout<<" Starting benchmark test"<<endl;
	benchmark->Start("test");

	//Call analysis job
//	cout<<"argv[optind]:"<<argv[optind]<<endl;
	cout<<"NEvents="<<progpar.NEvents<<"\t"<<"argv[optind]="<<argv[optind]<<"\t"<<"&argv[optind]="<<&argv[optind]<<"\t"<<"argc-optind="<<argc-optind<<endl;               //argv[optind]:/space/runs/daq021949.nat.bz2
//	kf_process(argc-optind,&argv[optind],0);
	kf_process(argc-optind,&argv[optind],progpar.NEvents);             //установка ограничения на число обрабатываемых событий

	benchmark->Show("test");

	if( fout ) {
		fout->Write();         //пишем данные в файл и закрываем его
		fout->Close();
	}

	return 0;
}
