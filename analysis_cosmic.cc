#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "ReadNat/rr_def.h"
#include "ReadNat/re_def.h"
#include "ReadNat/ss_def.h"
#include "VDDCRec/ktracks.h"
#include "VDDCRec/mtofhits.h"
#include "VDDCRec/ToFTrack.hh"
#include "KrToF/tof_system.h"
#include "KrAtc/atcrec.h"
#include "KrAtc/atc_to_track.h"
#include "KrVDDCMu/dcmu.h"
#include "KrMu/mu_system.h"
#include "KrMu/mu_event.h"

#include "KrAtc/atc_geometry.h"
#include "KrAtc/atc_regions.h"
#include "KrAtc/AtcHit.hh"
#include "KrAtc/AtcPar.hh"
#include "KrAtc/AtcRec.hh"

#include "KEmcRec/emc_struct.h"
#include "KEmcRec/emc_system.h"
#include "KEmcRec/emc_struct.h"

#include "VDDCRec/kdcvd.h"
#include "VDDCRec/kvd.h"
#include "VDDCRec/kdcpar.h"
#include "VDDCRec/ktracks.h"
#include "VDDCRec/khits.h"
#include "VDDCRec/tfit.h"
#include "VDDCRec/ktrkhits.h"
#include "VDDCRec/ktrkfpar.h"
#include "VDDCRec/kglobparam.h"
#include "ReadNat/mcdc_def.h"
#include "ReadNat/dcrawhitspar.h"

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


using namespace std;

static const char* progname;

struct ProgramParameters {
	bool read_reco;
	int min_track_number;
	int max_track_number;
	int min_beam_track_number;
	int min_ip_track_number;
        float min_momentum;
        float max_momentum;
	float min_cluster_energy;
	float min_total_energy;
	int min_cluster_number;
	int min_lkrcl_number;
	int min_csicl_number;
	const char* rootfile;
	int MCCalibRunNumber;
	int NEvents;
	int kdisp_ev_cut;
	bool process_only;
};

static const struct ProgramParameters def_progpar={false,0,1,0,0,0,100000,50,100,2,0,0,"/store/users/ovtin/out.root",19862,0,0,false};

static struct ProgramParameters progpar(def_progpar);

enum {
	CutLongDcRecord=1,
	CutLongVdRecord,
        AtcEventDamageCut,
        AtcIllegalTrackCut,
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

static TTree *eventTree;

typedef struct {
    Int_t vrtntrk,vrtnip,vrtnbeam,nhits,nhitsvd,nhitsxy,nhitsz,nvec,nvecxy,nvecz,ncls,nlkr,ncsi,munhits,run,rEv;
    Float_t P,Pt,chi2,theta,phi,emcenergy,lkrenergy,csienergy;
    Int_t natc_cr, cnt[10],aerogel_REGION[10],aerogel_REGION0[10],aerogel_REGION5[10],aerogel_REGION20[10],
    active_REGION[10],active_REGION0[10],active_REGION5[10],active_REGION20[10],testreg[10],
    single_aerogel_REGION[10],single_aerogel_REGION0[10],single_aerogel_REGION5[10],single_aerogel_REGION20[10],
    single_active_REGION[10],single_active_REGION0[10],single_active_REGION5[10],single_active_REGION20[10],
    single_testreg[10];
    Float_t wlshit[10],nearwls[10],tlen[10],pathwls[10],npe[10],npen[10];
    Float_t rin[10], phiin[10], zin[10];	//local cylindric coordinates of track in-point
    Float_t rout[10], phiout[10], zout[10]; //local cylindric coordinates of track out-point
    Float_t rwls[10], phiwls[10], zwls[10];	//position of WLS crossing (middle plane of the counter)
    Float_t neighnpe[10];           //maximum amplitude of neighbouring counters
    Float_t Rin_gl[10], Phiin_gl[10], Zin_gl[10];	//global cylindric coordinates of track in-point
    Float_t Rout_gl[10], Phiout_gl[10], Zout_gl[10];     //global cylindric coordinates of track out-point
    Float_t Xip,Yip,Zip;
} BCOSM;

static BCOSM bcosm;
int natccross;

static struct ATCCounterBranch bcnt[9];
static struct ATCBranch batc;

//Rejection prior to reconstruction helps to save CPU time
extern "C" void kemc_energy_(float Ecsi[2],float *Elkr);

int pre_event_rejection()
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
    if( tP(0)>progpar.max_momentum )       return MaxMomentumCut;
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

int atc_rejection()
{
    //ATC raw record damaged in any way (including when DeltaT is absent or out of range)
    if( atc_rec.eventdamage ) return AtcEventDamageCut;

    //track determined as illegal by atcrec
    if( atc_track.illtrack[0] ) return AtcIllegalTrackCut;

    //no counters on tracks, very strange if occurs
    //if( atc_track.ncnt_on_track[0]==0 ) return NoAtcOnTrackCut;

    return 0;
}

int mu_event_rejection()
{
    //if( mu_next_event()<=0 ) return MUCut;

    return 0;
}

int analyse_event()
{
    if(eTracksAll==1)
    {
	int t=0;
	unsigned short MUnhits=mu_next_event();

	if( sqrt(pow(tX0IP(t),2)+pow(tY0IP(t),2)+pow(tZ0IP(t),2))<40 )
	{
	    bcosm.vrtntrk = eTracksAll;
	    bcosm.vrtnip = eTracksIP;
	    bcosm.vrtnbeam = eTracksBeam;

            bcosm.Xip = tX0IP(t);
            bcosm.Yip = tY0IP(t);
            bcosm.Zip = tZ0IP(t);

	    bcosm.nhits = tHits(t);
	    bcosm.nhitsvd = tHitsVD(t);

	    bcosm.nhitsxy = tHitsXY(t);
	    bcosm.nhitsz = tHits(t)-tHitsXY(t);

	    bcosm.nvec = tVectors(t);
	    bcosm.nvecxy = tVectorsXY(t);
	    bcosm.nvecz = tVectorsZ(t);

	    bcosm.theta = tTeta(t);
	    bcosm.phi = ktrrec_.FITRAK[t]+(ktrrec_.FITRAK[t]<0?360:0);

	    bcosm.P = tP(t);
	    bcosm.Pt = tPt(t);
	    bcosm.chi2 = tCh2(t);

	    bcosm.ncls=semc.emc_ncls;
	    bcosm.nlkr=emcRec->lkrClusters.size();
	    bcosm.ncsi=emcRec->csiClusters.size();

	    bcosm.emcenergy=0;
	    bcosm.lkrenergy=0;
	    bcosm.csienergy=0;

	    for(int c=0; c<semc.emc_ncls; c++)
	    {
		bcosm.emcenergy+=semc.emc_energy[c];
		if( semc.emc_type[c]==1 )
		    bcosm.lkrenergy+=semc.emc_energy[c];
		else if( semc.emc_type[c]==2 )
		    bcosm.csienergy+=semc.emc_energy[c];
	    }

	    bcosm.rEv = kedrraw_.Header.Number;
	    bcosm.run = kedrraw_.Header.RunNumber;
	    bcosm.munhits = MUnhits;

	    natccross = 0;
	    for(int i=0; i<NATC; i++) if( atc_track.cnt_ntrk[i] ) natccross++;
	    bcosm.natc_cr = natccross;

	    for(int i=0; i<natccross; i++)
	    {
		int icnt = atc_track.cnt_cross[t][i]-1;
		bcosm.cnt[i] = icnt;
		bcosm.wlshit[i] = atc_track.wls_hit[t][i];
		bcosm.nearwls[i] = atc_track.near_wls[t][i];
		bcosm.tlen[i] = atc_track.tlen_in_aer[t][i]*0.1;    //tlen in cm
		bcosm.pathwls[i] = atc_track.tlen_in_wls[t][i]*0.1;

		// set 1pe amplitude to default (30 cnts) if not available but the counter working
		if( !cntPar[icnt+1].isOffline() && cntPar[icnt+1].getA1pe()==0 && kedrrun_cb_.Header.RunType != 64)
		    bcosm.npe[i] = atc_rec.amp[icnt]/30.;
		else
		    bcosm.npe[i] = atc_rec.npe[icnt];

		if( bcosm.tlen[i]!=0 ) {
		    if( atc_is_endcap(icnt+1) )
			bcosm.npen[i] = ThicknessEndcap*0.1*bcosm.npe[i]/bcosm.tlen[i];
		    else if( atc_is_barrel_1layer(icnt+1) )
			bcosm.npen[i] = ThicknessBarrel1*0.1*bcosm.npe[i]/bcosm.tlen[i];
		    else if( atc_is_barrel_2layer(icnt+1) )
			bcosm.npen[i] = ThicknessBarrel2*0.1*bcosm.npe[i]/bcosm.tlen[i];
		}

		bcosm.rin[i]=atc_track.rin[t][i]*0.1;
		bcosm.rout[i]=atc_track.rout[t][i]*0.1;
		bcosm.phiin[i]=atc_track.phiin[t][i];
		bcosm.phiout[i]=atc_track.phiout[t][i];
		bcosm.zin[i]=atc_track.zin[t][i]*0.1;
		bcosm.zout[i]=atc_track.zout[t][i]*0.1;
		bcosm.rwls[i]=atc_track.rwls[t][i]*0.1;
		bcosm.phiwls[i]=atc_track.phiwls[t][i];
		bcosm.zwls[i]=atc_track.zwls[t][i]*0.1;

		double rin_gl,phiin_gl,zin_gl,rout_gl,phiout_gl,zout_gl;
		atc_get_global(icnt+1,atc_track.rin[t][i],atc_track.phiin[t][i],atc_track.zin[t][i],rin_gl, phiin_gl,zin_gl);
		atc_get_global(icnt+1,atc_track.rout[t][i],atc_track.phiout[t][i],atc_track.zout[t][i],rout_gl, phiout_gl,zout_gl);
		bcosm.Rin_gl[i]=rin_gl*0.1;
		bcosm.Rout_gl[i]=rout_gl*0.1;
		bcosm.Phiin_gl[i]=phiin_gl;
		bcosm.Phiout_gl[i]=phiout_gl;
		bcosm.Zin_gl[i]=zin_gl*0.1;
		bcosm.Zout_gl[i]=zout_gl*0.1;

		//===================================================ATC_DOUBLE_CROSS===================================
		//если есть пересечение области аэрогеля с разным отступом от стенок и от шифтера
		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION,ATC_DOUBLE_CROSS) )  bcosm.aerogel_REGION[i]=1;
		else  bcosm.aerogel_REGION[i]=0;

		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION0,ATC_DOUBLE_CROSS) )  bcosm.aerogel_REGION0[i]=1;
		else  bcosm.aerogel_REGION0[i]=0;

		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION5,ATC_DOUBLE_CROSS) )  bcosm.aerogel_REGION5[i]=1;
		else  bcosm.aerogel_REGION5[i]=0;

		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION20,ATC_DOUBLE_CROSS) )  bcosm.aerogel_REGION20[i]=1;
		else  bcosm.aerogel_REGION20[i]=0;

		//если есть пересечение области аэрогеля и шифтера с разным отступом от стенок
		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION,ATC_DOUBLE_CROSS) )  bcosm.active_REGION[i]=1;
		else  bcosm.active_REGION[i]=0;

		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION0,ATC_DOUBLE_CROSS) )  bcosm.active_REGION0[i]=1;
		else  bcosm.active_REGION0[i]=0;

		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION5,ATC_DOUBLE_CROSS) )  bcosm.active_REGION5[i]=1;
		else  bcosm.active_REGION5[i]=0;

		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION20,ATC_DOUBLE_CROSS) )  bcosm.active_REGION20[i]=1;
		else  bcosm.active_REGION20[i]=0;

		if( atc_track_cross_region(t,i,ATC_CRT_REGION,ATC_DOUBLE_CROSS) ) bcosm.testreg[i]=1;
		else  bcosm.testreg[i]=0;

		//===================================================ATC_SINGLE_CROSS===================================
		//если есть пересечение области аэрогеля с разным отступом от стенок и от шифтера
		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION,ATC_SINGLE_CROSS) )  bcosm.single_aerogel_REGION[i]=1;
		else  bcosm.single_aerogel_REGION[i]=0;

		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION0,ATC_SINGLE_CROSS) )  bcosm.single_aerogel_REGION0[i]=1;
		else  bcosm.single_aerogel_REGION0[i]=0;

		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION5,ATC_SINGLE_CROSS) )  bcosm.single_aerogel_REGION5[i]=1;
		else  bcosm.single_aerogel_REGION5[i]=0;

		if( atc_track_cross_region(t,i,ATC_AEROGEL_REGION20,ATC_SINGLE_CROSS) )  bcosm.single_aerogel_REGION20[i]=1;
		else  bcosm.single_aerogel_REGION20[i]=0;

		//если есть пересечение области аэрогеля и шифтера с разным отступом от стенок
		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION,ATC_SINGLE_CROSS) )  bcosm.single_active_REGION[i]=1;
		else  bcosm.single_active_REGION[i]=0;

		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION0,ATC_SINGLE_CROSS) )  bcosm.single_active_REGION0[i]=1;
		else  bcosm.single_active_REGION0[i]=0;

		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION5,ATC_SINGLE_CROSS) )  bcosm.single_active_REGION5[i]=1;
		else  bcosm.single_active_REGION5[i]=0;

		if( atc_track_cross_region(t,i,ATC_ACTIVE_REGION20,ATC_SINGLE_CROSS) )  bcosm.single_active_REGION20[i]=1;
		else  bcosm.single_active_REGION20[i]=0;

		if( atc_track_cross_region(t,i,ATC_CRT_REGION,ATC_SINGLE_CROSS) ) bcosm.single_testreg[i]=1;
		else  bcosm.single_testreg[i]=0;
	    }
	    eventTree->Fill();
	}
	if( progpar.process_only ) return 1;
    }
    return 0;
}

static const char* optstring="ra:h:b:p:s:w:t:e:c:k:i:o:v:n:l:x";

void Usage(int status)
{
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
		fout = new TFile(progpar.rootfile,"RECREATE");

	eventTree = new TTree("et","Event tree");
	eventTree->SetAutoSave(500000000); // autosave when 0.5 Gbyte written
	eventTree->Branch("bcosm",&bcosm,"vrtntrk/I:vrtnip:vrtnbeam:nhitst:nhitsvd:nhitsxy:nhitsz:nvec:nvecxy:nvecz:ncls:nlkr:ncsi:munhits:run:rEv"
			  ":P/F:Pt:chi2:theta:phi:emcenergy:lkrenergy:csienergy"
			  ":natc_cr/I:cnt[10]:aerogel_REGION[10]:aerogel_REGION0[10]:aerogel_REGION5[10]:aerogel_REGION20[10]:active_REGION[10]:active_REGION0[10]"
			  ":active_REGION5[10]:active_REGION20[10]:testreg[10]:single_aerogel_REGION[10]:single_aerogel_REGION0[10]:single_aerogel_REGION5[10]"
			  ":single_aerogel_REGION20[10]:single_active_REGION[10]:single_active_REGION0[10]:single_active_REGION5[10]:single_active_REGION20[10]:single_testreg[10]"
			  ":wlshit[10]/F:nearwls[10]:tlen[10]:pathwls[10]:npe[10]:npen[10]"
			  ":rin[10]:phiin[10]:zin[10]:rout[10]:phiout[10]:zout[10]:rwls[10]:phiwls[10]:zwls[10]:neighnpe[10]:Rin_gl[10]:Phiin_gl[10]:Zin_gl[10]"
			  ":Rout_gl[10]:Phiout_gl[10]:Zout_gl[10]:Xip:Yip:Zip");

	//Set kframework signal handling
	kf_install_signal_handler(1);

	//Set subsystems to be used
        kf_use(KF_VDDC_SYSTEM|KF_TOF_SYSTEM|KF_ATC_SYSTEM|KF_EMC_SYSTEM|KF_MU_SYSTEM);

	//Register to kframework used cuts
	char buf[100];
	kf_add_cut(KF_PRE_SEL,CutLongDcRecord,"DC event length >1940 words");
	kf_add_cut(KF_PRE_SEL,CutLongVdRecord,"VD event length >156 words");

	sprintf(buf,"momentum <= %5.fMeV/c",progpar.min_momentum);
	kf_add_cut(KF_VDDC_SEL,MinMomentumCut,buf);
	sprintf(buf,"momentum > %5.fMeV/c",progpar.max_momentum);
	kf_add_cut(KF_VDDC_SEL,MaxMomentumCut,buf);
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

	kf_MCCalibRunNumber(1,progpar.MCCalibRunNumber,progpar.MCCalibRunNumber,50,1.,1.,1.);
	//kf_MCCalibRunNumber(progpar.simOn,progpar.MCCalibRunNumber,progpar.MCCalibRunNumberL,progpar.NsimRate,progpar.Scale,progpar.Ascale,progpar.Zscale);

	kf_kdisplay_cut(progpar.kdisp_ev_cut);
	//Set automatic cosmic run determination
//	kf_cosmic(-1);  //auto
	kf_cosmic(1);  //cosmic  >0

	//Register an analysis routine
	kf_register_analysis(analyse_event);
        cout<<"analyse_event="<<analyse_event<<endl;

	//Do not reconstruct, read reconstruction records from file
	kf_reco_from_file(progpar.read_reco);

	//Set exclusive event processing
	kf_process_only(progpar.process_only);

	TBenchmark *benchmark=new TBenchmark;
	benchmark->Start("test");

	//Call analysis job
	cout<<"NEvents="<<progpar.NEvents<<"\t"<<"argv[optind]="<<argv[optind]<<"\t"<<"&argv[optind]="<<&argv[optind]<<"\t"<<"argc-optind="<<argc-optind<<endl;               //argv[optind]:/space/runs/daq021949.nat.bz2
	kf_process(argc-optind,&argv[optind],progpar.NEvents);

	benchmark->Show("test");

	if( fout ) {
		fout->Write();
		fout->Close();
	}

	return 0;
}
