/**
 * @file    AOloopControl_perfTest.c
 * @brief   Adaptive Optics Control loop engine testing
 * 
 * AO engine uses stream data structure
 *  
 * 
 * ## Main files
 * 
 * AOloopControl_perfTest_DM.c               Test DM speed and examine DM modes
 * AOloopControl_perfTest_status.c           Report loop metrics, loop performance monitor
 * 
 * @bug No known bugs.
 * 
 * 
 */



#define _GNU_SOURCE

// uncomment for test print statements to stdout
//#define _PRINT_TEST



/* =============================================================================================== */
/* =============================================================================================== */
/*                                        HEADER FILES                                             */
/* =============================================================================================== */
/* =============================================================================================== */
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <dirent.h> 
#include <stdio.h> 
#include <string.h> /* strrchr */


#include "CommandLineInterface/CLIcore.h"
#include "00CORE/00CORE.h"
#include "COREMOD_tools/COREMOD_tools.h"
#include "COREMOD_iofits/COREMOD_iofits.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "statistic/statistic.h"

#include "AOloopControl/AOloopControl.h"
#include "AOloopControl_perfTest/AOloopControl_perfTest.h"

/* =============================================================================================== */
/* =============================================================================================== */
/*                                      DEFINES, MACROS                                            */
/* =============================================================================================== */
/* =============================================================================================== */

# ifdef _OPENMP
# include <omp.h>
#define OMP_NELEMENT_LIMIT 1000000
# endif

#define MaxNBdatFiles 100000

/* =============================================================================================== */
/* =============================================================================================== */
/*                                  GLOBAL DATA DECLARATION                                        */
/* =============================================================================================== */
/* =============================================================================================== */

//extern long aoloopcontrol_var.aoconfID_wfsim;              // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_dmC;                // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_dmRM;               // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_DMmodes;            // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_gainb;              // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_limitb;             // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_multfb;             // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_DMmode_GAIN;        // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_LIMIT_modes;        // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_MULTF_modes;        // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_cmd_modes;          // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_meas_modes;         // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_RMS_modes;          // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_AVE_modes;          // declared in AOloopControl.c
//extern long aoloopcontrol_var.aoconfID_modeARPFgainAuto;   // declared in AOloopControl.c
     

static int wcol, wrow; // window size

// TIMING
static struct timespec tnow;
static struct timespec tdiff;


typedef struct {
		char   name[500];
		double tstart;
		double tend;
		long   cnt;
	} StreamDataFile;  



/* =============================================================================================== */
/*                                     MAIN DATA STRUCTURES                                        */
/* =============================================================================================== */

static int INITSTATUS_AOloopControl_perfTest = 0;

extern long LOOPNUMBER; // current loop index

extern AOLOOPCONTROL_CONF *AOconf; // declared in AOloopControl.c
extern AOloopControl_var aoloopcontrol_var; // declared in AOloopControl.c



// CLI commands
//
// function CLI_checkarg used to check arguments
// CLI_checkarg ( CLI argument index , type code )
//
// type codes:
// 1: float
// 2: long
// 3: string, not existing image
// 4: existing image
// 5: string
//



/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 9. STATUS / TESTING / PERF MEASUREMENT                                   */
/* =============================================================================================== */
/* =============================================================================================== */












/** @brief CLI function for AOcontrolLoop_TestDMSpeed */
int_fast8_t AOcontrolLoop_perfTest_TestDMSpeed_cli()
{
    if(CLI_checkarg(1,4)+CLI_checkarg(2,2)+CLI_checkarg(3,2)+CLI_checkarg(4,1)==0) {
        AOcontrolLoop_perfTest_TestDMSpeed( data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOcontrolLoop_TestSystemLatency */
int_fast8_t AOcontrolLoop_perfTest_TestSystemLatency_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,1)+CLI_checkarg(4,2)==0) {
        AOcontrolLoop_perfTest_TestSystemLatency(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_TestDMmodeResp */
int_fast8_t AOloopControl_perfTest_TestDMmodeResp_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,2)+CLI_checkarg(3,1)+CLI_checkarg(4,1)+CLI_checkarg(5,1)+CLI_checkarg(6,1)+CLI_checkarg(7,1)+CLI_checkarg(8,2)+CLI_checkarg(9,4)+CLI_checkarg(10,4)+CLI_checkarg(11,4)+CLI_checkarg(12,3)==0) {
        AOloopControl_perfTest_TestDMmodeResp(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numf, data.cmdargtoken[6].val.numf, data.cmdargtoken[7].val.numf, data.cmdargtoken[8].val.numl, data.cmdargtoken[9].val.string, data.cmdargtoken[10].val.string, data.cmdargtoken[11].val.string, data.cmdargtoken[12].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_TestDMmodes_Recovery */
int_fast8_t AOloopControl_perfTest_TestDMmodes_Recovery_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,1)+CLI_checkarg(3,4)+CLI_checkarg(4,4)+CLI_checkarg(5,4)+CLI_checkarg(6,4)+CLI_checkarg(7,1)+CLI_checkarg(8,2)+CLI_checkarg(9,3)+CLI_checkarg(10,3)+CLI_checkarg(11,3)+CLI_checkarg(12,3)==0) {
        AOloopControl_perfTest_TestDMmodes_Recovery(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numf, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.string, data.cmdargtoken[5].val.string, data.cmdargtoken[6].val.string, data.cmdargtoken[7].val.numf, data.cmdargtoken[8].val.numl, data.cmdargtoken[9].val.string, data.cmdargtoken[10].val.string, data.cmdargtoken[11].val.string, data.cmdargtoken[12].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_blockstats */
int_fast8_t AOloopControl_perfTest_blockstats_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,5)==0) {
        AOloopControl_perfTest_blockstats(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_InjectMode */
int_fast8_t AOloopControl_perfTest_InjectMode_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0)    {
        AOloopControl_perfTest_InjectMode(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
        return 0;
    }
    else    return 1;
}

/** @brief CLI function for AOloopControl_loopMonitor */
int_fast8_t AOloopControl_perfTest_loopMonitor_cli() {
    if(CLI_checkarg(1,1)+CLI_checkarg(2,2)==0) {
        AOloopControl_perfTest_loopMonitor(LOOPNUMBER, data.cmdargtoken[1].val.numf, data.cmdargtoken[2].val.numl);
        return 0;
    } else {
        AOloopControl_perfTest_loopMonitor(LOOPNUMBER, 1.0, 8);
        return 0;
    }
}

/** @brief CLI function for AOloopControl_statusStats */
int_fast8_t AOloopControl_perfTest_statusStats_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,2)==0) {
        AOloopControl_perfTest_statusStats(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_mkTestDynamicModeSeq */
int_fast8_t AOloopControl_perfTest_mkTestDynamicModeSeq_cli()
{
    if(CLI_checkarg(1,3)+CLI_checkarg(2,2)+CLI_checkarg(3,2)+CLI_checkarg(4,2)==0) {
        AOloopControl_perfTest_mkTestDynamicModeSeq(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl);
        return 0;
    }
    else  return 1;
}

/** @brief CLI function for AOloopControl_AnalyzeRM_sensitivity */
int_fast8_t AOloopControl_perfTest_AnalyzeRM_sensitivity_cli()
{
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,4)+CLI_checkarg(4,4)+CLI_checkarg(4,4)+CLI_checkarg(5,4)+CLI_checkarg(6,1)+CLI_checkarg(7,1)+CLI_checkarg(8,3)==0)    {
        AOloopControl_perfTest_AnalyzeRM_sensitivity(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.string, data.cmdargtoken[5].val.string, data.cmdargtoken[6].val.numf, data.cmdargtoken[7].val.numf, data.cmdargtoken[8].val.string);
        return 0;
    }
    else        return 1;
}



int_fast8_t AOloopControl_LoopTimer_Analysis_cli()
{
    if(CLI_checkarg(1,4)+CLI_checkarg(2,5)+CLI_checkarg(3,5)==0)    
    {	
		AOloopControl_LoopTimer_Analysis(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string);
		return 0;
	}
	else
		return 1;
}


int_fast8_t AOloopControl_perfTest_mkSyncStreamFiles2_cli()
{
	if(CLI_checkarg(1,5)+CLI_checkarg(2,5)+CLI_checkarg(3,5)+CLI_checkarg(4,1)+CLI_checkarg(5,1)+CLI_checkarg(6,1)+CLI_checkarg(7,1)==0)    
    {
		AOloopControl_perfTest_mkSyncStreamFiles2(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numf, data.cmdargtoken[6].val.numf, data.cmdargtoken[7].val.numf);
		return 0;
	}
	else
		return 1;
}


int_fast8_t AOloopControl_perfTest_ComputeSimilarityMatrix_cli()
{
	if(CLI_checkarg(1,4)+CLI_checkarg(2,5) == 0)
	{
		AOloopControl_perfTest_ComputeSimilarityMatrix(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string);
	}
	else
		return 1;
}



int_fast8_t AOloopControl_perfTest_StatAnalysis_2streams_cli()
{
	if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,4)+CLI_checkarg(4,4)+CLI_checkarg(5,2)+CLI_checkarg(6,2) == 0)
	{
		AOloopControl_perfTest_StatAnalysis_2streams(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.string, data.cmdargtoken[5].val.numl, data.cmdargtoken[6].val.numl);
	}
	else
		return 1;
}


int_fast8_t AOloopControl_perfTest_SelectWFSframes_from_PSFframes_cli()
{
	if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,1)+CLI_checkarg(4,2)+CLI_checkarg(5,2)+CLI_checkarg(6,2)+CLI_checkarg(7,2)+CLI_checkarg(8,2)+CLI_checkarg(9,1) == 0)
	{
		AOloopControl_perfTest_SelectWFSframes_from_PSFframes(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numl, data.cmdargtoken[5].val.numl, data.cmdargtoken[6].val.numl, data.cmdargtoken[7].val.numl, data.cmdargtoken[8].val.numl, data.cmdargtoken[9].val.numf);
	}
	else
		return 1;
}





/* =============================================================================================== */
/* =============================================================================================== */
/*                                    FUNCTIONS SOURCE CODE                                        */
/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl functions */







/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 1. INITIALIZATION, configurations                                        */
/* =============================================================================================== */
/* =============================================================================================== */

void __attribute__ ((constructor)) libinit_AOloopControl_perfTest()
{
	if ( INITSTATUS_AOloopControl_perfTest == 0 )
	{
		init_AOloopControl_perfTest();
		RegisterModule(__FILE__, "cacao", "AO loop control performance monitoring and testing");
		INITSTATUS_AOloopControl_perfTest = 1;
	}
}


int_fast8_t init_AOloopControl_perfTest()
{
    FILE *fp;

/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 9. STATUS / TESTING / PERF MEASUREMENT                                   */
/* =============================================================================================== */
/* =============================================================================================== */

    RegisterCLIcommand("aoldmtestsp", __FILE__, AOcontrolLoop_perfTest_TestDMSpeed_cli, "test DM speed by sending circular tip-tilt", "<dmname> <delay us [long]> <NB pts> <ampl>", "aoldmtestsp dmdisp2 100 20 0.1", "long AOcontrolLoop_perfTest_TestDMSpeed(char *dmname, long delayus, long NBpts, float ampl)");

    RegisterCLIcommand("aoltestlat", __FILE__, AOcontrolLoop_perfTest_TestSystemLatency_cli, "test system latency", "<dm stream> <wfs stream> <ampl [um]> <NBiter>", "aoltestlat dmC wfsim 0.1 5000", "long AOcontrolLoop_perfTest_TestSystemLatency(char *dmname, char *wfsname, float OPDamp, long NBiter)");

    RegisterCLIcommand("aoltestmresp", __FILE__, AOloopControl_perfTest_TestDMmodeResp_cli, "Measure system response for a single mode", "<DM modes [3D im]> <mode #> <ampl [um]> <fmin [Hz]> <fmax [Hz]> <fstep> <meas. time [sec]> <time step [us]> <DM mask> <DM in [2D stream]> <DM out [2D stream]>  <output [2D im]>", "aoltestmresp DMmodesC 5 0.05 10.0 100.0 1.2 1.0 1000 dmmask dmdisp3 dmC out", "long AOloopControl_perfTest_TestDMmodeResp(char *DMmodes_name, long index, float ampl, float fmin, float fmax, float fmultstep, float avetime, long dtus, char *DMmask_name, char *DMstream_in_name, char *DMstream_out_name, char *IDout_name)");

    RegisterCLIcommand("aoltestdmrec", __FILE__, AOloopControl_perfTest_TestDMmodes_Recovery_cli, "Test system DM modes recovery", "<DM modes [3D im]> <ampl [um]> <DM mask [2D im]> <DM in [2D stream]> <DM out [2D stream]> <meas out [2D stream]> <lag time [us]>  <NB averages [long]>  <out ave [2D im]> <out rms [2D im]> <out meas ave [2D im]> <out meas rms [2D im]>", "aoltestdmrec DMmodesC 0.05 DMmask dmsisp2 dmoutr 2000  20 outave outrms outmave outmrms", "long AOloopControl_perfTest_TestDMmodes_Recovery(char *DMmodes_name, float ampl, char *DMmask_name, char *DMstream_in_name, char *DMstream_out_name, char *DMstream_meas_name, long tlagus, long NBave, char *IDout_name, char *IDoutrms_name, char *IDoutmeas_name, char *IDoutmeasrms_name)");

    RegisterCLIcommand("aolresetrms", __FILE__, AOloopControl_perfTest_resetRMSperf, "reset RMS performance monitor", "no arg", "aolresetrms", "int AOloopControl_perfTest_resetRMSperf()");

    RegisterCLIcommand("aolinjectmode",__FILE__, AOloopControl_perfTest_InjectMode_cli, "inject single mode error into RM channel", "<index> <ampl>", "aolinjectmode 20 0.1", "int AOloopControl_perfTest_InjectMode()");

    RegisterCLIcommand("aolstatusstats", __FILE__, AOloopControl_perfTest_statusStats_cli, "measures distribution of status values", "<update flag [int]> <NBsample [long]>", "aolstatusstats 0 100000", "int AOloopControl_perfTest_statusStats(int updateconf, long NBsample)");

    RegisterCLIcommand("aolmon", __FILE__, AOloopControl_perfTest_loopMonitor_cli, "monitor loop", "<frequ> <Nbcols>", "aolmon 10.0 3", "int AOloopControl_perfTest_loopMonitor(long loop, double frequ)");

    RegisterCLIcommand("aolblockstats", __FILE__, AOloopControl_perfTest_blockstats_cli, "measures mode stats per block", "<loopnb> <outim>", "aolblockstats 2 outstats", "long AOloopControl_perfTest_blockstats(long loop, const char *IDout_name)");

    RegisterCLIcommand("aolmktestmseq", __FILE__, AOloopControl_perfTest_mkTestDynamicModeSeq_cli, "make modal periodic test sequence", "<outname> <number of slices> <number of modes> <firstmode>", "aolmktestmseq outmc 100 50 0", "long AOloopControl_perfTest_mkTestDynamicModeSeq(const char *IDname_out, long NBpt, long NBmodes, long StartMode)");

    RegisterCLIcommand("aolzrmsens", __FILE__, AOloopControl_perfTest_AnalyzeRM_sensitivity_cli, "Measure zonal RM sensitivity", "<DMmodes> <DMmask> <WFSref> <WFSresp> <WFSmask> <amplitude[nm]> <lambda[nm]> <outname>", "aolzrmsens DMmodes dmmask wfsref0 zrespmat wfsmask 0.1 outfile.txt", "long AOloopControl_perfTest_AnalyzeRM_sensitivity(const char *IDdmmodes_name, const char *IDdmmask_name, const char *IDwfsref_name, const char *IDwfsresp_name, const char *IDwfsmask_name, float amplimitnm, float lambdanm, const char *foutname)");

	RegisterCLIcommand("aoltimingstat", __FILE__, AOloopControl_LoopTimer_Analysis_cli, "Analysis of loop timing data", "<TimingImage> <TimingTXTfile> <outFile>", "aoltimingstat aol0_looptiming timing.txt outfile.txt", "long AOloopControl_LoopTimer_Analysis(char *IDname, char *fnametxt, char *outfname)");


	RegisterCLIcommand("aolptmksyncs2",
		__FILE__,
		AOloopControl_perfTest_mkSyncStreamFiles2_cli,
		"synchronize two streams from disk telemetry",
		"<datadir> <stream0name> <stream1name> <tstart> <tend> <dt> <dtlag>",
		"aolptmksyncs2 \"/media/data/20180701/\" aol2_wfsim aol3_wfsim 1530410732.0 1530410733.0 0.001 0.00001",
		"int AOloopControl_perfTest_mkSyncStreamFiles2(char *datadir, char *stream0, char *stream1, double tstart, double tend, double dt, double dtlag)");
		
		
	RegisterCLIcommand("aolperfcompsimM",
		__FILE__,
		AOloopControl_perfTest_ComputeSimilarityMatrix_cli,
		"compute similarity matrix from image sequence",
		"<input cube> <output matrix>",
		"aolperfcompsimM imc outM",
		"int AOloopControl_perfTest_ComputeSimilarityMatrix(char *IDname, char *IDname_out)");
		

	RegisterCLIcommand("aolperfsimpairs",
		__FILE__,
		AOloopControl_perfTest_StatAnalysis_2streams_cli,
		"Find similarity pairs and perform statistical analysis",
		"<input stream0 cube> <input stream1 cube> <input simM0> <input simM1> <min frame dt> <NBselected>",
		"aolperfsimpairs imc0 imc1 simM0 simM1 20 1000",
		"int AOloopControl_perfTest_StatAnalysis_2streams(char *IDname_stream0, char *IDname_stream1, char *IDname_simM0, char *IDname_simM1, long dtmin, long NBselected)");

	
	RegisterCLIcommand("aolperfselwfsfpsf",
		__FILE__,
		AOloopControl_perfTest_SelectWFSframes_from_PSFframes_cli,
		"select WFS frames from PSF frames (synchronized)",
		"<input WFS cube> <input PSF cube> <fraction> <x0> <x1> <y0> <y1> <EvalMode> <alpha>",
		"aolperfselwfsfpsf imwfsC impsfC 100 120 100 120 0 2.0",
		"int AOloopControl_perfTest_SelectWFSframes_from_PSFframes(char *IDnameWFS, char *IDnamePSF, float frac, long x0, long x1, long y0, long y1, int EvalMode, float alpha)");
	
}





/**
 *  ## Purpose
 *
 * Measure hardware latency between DM and WFS streams
 *
 *
 * ## Arguments
 *
 * @param[in]
 * dmname	char*
 * 			DM actuation stream to which function sends pokes
 *
 * @param[in]
 * wfsname	char*
 * -		WFS image stream
 *
 * @param[in]
 * OPDamp	FLOAT
 * 			Poke amplitude \[um\]
 *
 * @param[in]
 * NBiter	LONG
 * 			Number of poke cycles
 *
 */
int_fast8_t AOcontrolLoop_perfTest_TestSystemLatency(
    const char *dmname,
    char *wfsname,
    float OPDamp,
    long NBiter) {
    long IDdm;
    long dmxsize, dmysize;
    long IDwfs;
    long wfsxsize, wfsysize, wfssize;
    //    long twait0us = 100000;

    double tdouble_start;
    double tdouble_end;
    long wfscntstart;
    long wfscntend;

    struct timespec tstart;
    //    struct timespec tnow;
    struct timespec *tarray;
    double tdouble;
    double dtmax = 1.0;  // Max running time per iteration
    double dt, dt1;
    double *dtarray;
    double a, b;
    char command[200];
    long IDdm0, IDdm1; // DM shapes
    long ii, jj;
    float x, y;

    long IDwfsc;
    long wfs_NBframesmax = 50;
    long wfsframe;
    long twaitus = 30000; // initial wait [us]
    double dtoffset0 = 0.002; // 2 ms
    long wfsframeoffset = 10;

    long IDwfsref;

    char *ptr;
    long kk, kkmax;
    double *valarray;
    double tmp;
    double dtoffset;
    long kkoffset;

    long iter;

    float *latencyarray;
    float *latencysteparray;
    float latencyave, latencystepave;

    FILE *fp;
    float minlatency, maxlatency;
    double wfsdt;

    uint8_t datatype;
    uint32_t naxes[3];



    // ===========================
    // processinfo support
    // ===========================

    PROCESSINFO *processinfo;

    char pinfoname[200];
    sprintf(pinfoname, "mlat-%s-%s", dmname, wfsname);
    char descrstring[200];
    sprintf(descrstring, "lat %s %s", dmname, wfsname);
    char msgstring[200];
    sprintf(msgstring, "Measure Latency amp=%f %ld iter", OPDamp, NBiter);

    processinfo = processinfo_setup(
                      pinfoname,
                      descrstring,    // description
                      msgstring,  // message on startup
                      __FUNCTION__, __FILE__, __LINE__
                  );

    // OPTIONAL SETTINGS
    processinfo->MeasureTiming = 1; // Measure timing
    processinfo->RT_priority = 80;  // RT_priority, 0-99. Larger number = higher priority. If <0, ignore
	processinfo->loopcntMax = NBiter;
    int loopOK = 1;




    processinfo_WriteMessage(processinfo, "Allocating memory");

    latencyarray = (float *) malloc(sizeof(float) * NBiter);
    latencysteparray = (float *) malloc(sizeof(float) * NBiter);

    IDdm = image_ID(dmname);
    dmxsize = data.image[IDdm].md[0].size[0];
    dmysize = data.image[IDdm].md[0].size[1];

    IDdm0 = create_2Dimage_ID("_testdm0", dmxsize, dmysize);
    IDdm1 = create_2Dimage_ID("_testdm1", dmxsize, dmysize);

    float RMStot = 0.0;
    for(ii = 0; ii < dmxsize; ii++)
        for(jj = 0; jj < dmysize; jj++) {
            x = (2.0 * ii - 1.0 * dmxsize) / dmxsize;
            y = (2.0 * jj - 1.0 * dmxsize) / dmysize;
            data.image[IDdm0].array.F[jj * dmxsize + ii] = 0.0;
            data.image[IDdm1].array.F[jj * dmxsize + ii] = OPDamp * (sin(8.0 * x) * sin(8.0 * y));
            RMStot += data.image[IDdm1].array.F[jj * dmxsize + ii] * data.image[IDdm1].array.F[jj * dmxsize + ii];
        }
    RMStot = sqrt(RMStot / dmxsize / dmysize);

    for(ii = 0; ii < dmxsize; ii++)
        for(jj = 0; jj < dmysize; jj++) {
            data.image[IDdm1].array.F[jj * dmxsize + ii] *= OPDamp / RMStot;
        }


    if(system("mkdir -p tmp") != 0) {
        printERROR(__FILE__, __func__, __LINE__, "system() returns non-zero value");
    }

    save_fits("_testdm0", "!tmp/_testdm0.fits");
    save_fits("_testdm1", "!tmp/_testdm1.fits");

    IDwfs = image_ID(wfsname);
    sprintf(msgstring, "Connecting to stream %s %ld", wfsname, IDwfs);
    processinfo_WriteMessage(processinfo, msgstring);

    if(IDwfs == -1) {
        sprintf(msgstring, "Cannot connect to stream %s", wfsname);
        processinfo_error(processinfo, msgstring);
        return RETURN_FAILURE;
    }

    wfsxsize = data.image[IDwfs].md[0].size[0];
    wfsysize = data.image[IDwfs].md[0].size[1];
    wfssize = wfsxsize * wfsysize;
    datatype = data.image[IDwfs].md[0].datatype;

    naxes[0] = wfsxsize;
    naxes[1] = wfsysize;
    naxes[2] = wfs_NBframesmax;
    IDwfsc = create_image_ID("_testwfsc", 3, naxes, datatype, 0, 0);

    float FrameRateWait = 5.0;
    sprintf(msgstring, "Measuring frame rate over %.1f sec", FrameRateWait);
    processinfo_WriteMessage(processinfo, msgstring);


    // coarse estimate of frame rate
    clock_gettime(CLOCK_REALTIME, &tnow);
    tdouble_start = 1.0 * tnow.tv_sec + 1.0e-9 * tnow.tv_nsec;
    wfscntstart = data.image[IDwfs].md[0].cnt0;
    sleep(FrameRateWait);
    clock_gettime(CLOCK_REALTIME, &tnow);
    tdouble_end = 1.0 * tnow.tv_sec + 1.0e-9 * tnow.tv_nsec;
    wfscntend = data.image[IDwfs].md[0].cnt0;
    wfsdt = (tdouble_end - tdouble_start) / (wfscntend - wfscntstart);

    printf("wfs dt = %f sec\n", wfsdt);


    // update times
    dtmax = wfsdt * wfs_NBframesmax * 1.2 + 0.5;
    twaitus = 1000000.0 * wfsdt * 3.0;
    dtoffset0 = 1.5 * wfsdt;


    tarray = (struct timespec *) malloc(sizeof(struct timespec) * wfs_NBframesmax);
    dtarray = (double *) malloc(sizeof(double) * wfs_NBframesmax);

    if(system("mkdir -p timingstats") != 0) {
        printERROR(__FILE__, __func__, __LINE__, "system() returns non-zero value");
    }

    if((fp = fopen("timingstats/hardwlatency.txt", "w")) == NULL) {
        printf("ERROR: cannot create file \"timingstats/hardwlatency.txt\"\\n");
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &tnow);
    tdouble_start = 1.0 * tnow.tv_sec + 1.0e-9 * tnow.tv_nsec;
    wfscntstart = data.image[IDwfs].md[0].cnt0;
    wfsframeoffset = (long)(0.3 * wfs_NBframesmax);


    printf("WFS size : %ld %ld\n", wfsxsize, wfsysize);
    if(datatype == _DATATYPE_FLOAT) {
        printf("data type  :  _DATATYPE_FLOAT\n");
    }
    if(datatype == _DATATYPE_UINT16) {
        printf("data type  :  _DATATYPE_UINT16\n");
    }
    if(datatype == _DATATYPE_INT16) {
        printf("data type  :  _DATATYPE_INT16\n");
    }

    list_image_ID();





    // ===========================
    // Start loop
    // ===========================

    processinfo_loopstart(processinfo); // Notify processinfo that we are entering loop
    processinfo_WriteMessage(processinfo, "Starting loop");
	iter = 0;
    while(loopOK == 1) {
        //double tlastdouble;
        double tstartdouble;
        long NBwfsframe;
        unsigned long wfscnt0;
        double latencymax = 0.0;
        double latency;


        loopOK = processinfo_loopstep(processinfo);

        sprintf(msgstring, "iteration %5ld / %5ld", iter, NBiter);
        processinfo_WriteMessage(processinfo, msgstring);


        processinfo_exec_start(processinfo);

        printf(" - ITERATION %5ld / %5ld\n", iter, NBiter);
        fflush(stdout);

        for(ii = 0; ii < 10; ii++) {
            printf("  %5ld  ->  %f\n", ii, (float) data.image[IDwfs].array.SI16[ii]);
        }


        printf("write to %s\n", dmname);
        fflush(stdout);
        copy_image_ID("_testdm0", dmname, 1);

        unsigned int dmstate = 0;

        // waiting time
        usleep(twaitus);

        // and waiting frames
        wfscnt0 = data.image[IDwfs].md[0].cnt0;
        for(wfsframe = 0; wfsframe < wfs_NBframesmax; wfsframe++) {
            while(wfscnt0 == data.image[IDwfs].md[0].cnt0) {
                usleep(50);
            }
            wfscnt0 = data.image[IDwfs].md[0].cnt0;
        }

        dt = 0.0;
        clock_gettime(CLOCK_REALTIME, &tstart);
        tstartdouble = 1.0 * tstart.tv_sec + 1.0e-9 * tstart.tv_nsec;
        //    tlastdouble = tstartdouble;



        wfsframe = 0;
        wfscnt0 = data.image[IDwfs].md[0].cnt0;
        printf("\n");
        while((dt < dtmax) && (wfsframe < wfs_NBframesmax)) {
            // WAITING for image
            while(wfscnt0 == data.image[IDwfs].md[0].cnt0) {
                usleep(2);
            }

            wfscnt0 = data.image[IDwfs].md[0].cnt0;
            printf("[%8ld / %8ld]  %f  %f\n", wfsframe, wfs_NBframesmax, dt, dtmax);
            fflush(stdout);

            if(datatype == _DATATYPE_FLOAT) {
                // copy image to cube slice
                ptr = (char *) data.image[IDwfsc].array.F;
                ptr += sizeof(float) * wfsframe * wfssize;
                memcpy(ptr, data.image[IDwfs].array.F, sizeof(float)*wfssize);
            }

            if(datatype == _DATATYPE_UINT16) {
                // copy image to cube slice
                ptr = (char *) data.image[IDwfsc].array.UI16;
                ptr += sizeof(short) * wfsframe * wfssize;
                memcpy(ptr, data.image[IDwfs].array.UI16, sizeof(short)*wfssize);
            }

            if(datatype == _DATATYPE_INT16) {
                // copy image to cube slice
                ptr = (char *) data.image[IDwfsc].array.SI16;
                ptr += sizeof(short) * wfsframe * wfssize;
                memcpy(ptr, data.image[IDwfs].array.SI16, sizeof(short)*wfssize);
            }

            clock_gettime(CLOCK_REALTIME, &tarray[wfsframe]);

            tdouble = 1.0 * tarray[wfsframe].tv_sec + 1.0e-9 * tarray[wfsframe].tv_nsec;
            dt = tdouble - tstartdouble;
            //  dt1 = tdouble - tlastdouble;
            dtarray[wfsframe] = dt;
            //     tlastdouble = tdouble;

            // apply DM pattern #1
            if((dmstate == 0) && (dt > dtoffset0) && (wfsframe > wfsframeoffset)) {
                usleep((long)(ran1() * 1000000.0 * wfsdt));
                printf("\nDM STATE CHANGED ON ITERATION %ld\n\n", wfsframe);
                kkoffset = wfsframe;
                dmstate = 1;
                copy_image_ID("_testdm1", dmname, 1);

                clock_gettime(CLOCK_REALTIME, &tnow);
                tdouble = 1.0 * tnow.tv_sec + 1.0e-9 * tnow.tv_nsec;
                dt = tdouble - tstartdouble;
                dtoffset = dt; // time at which DM command is sent
            }
            wfsframe++;
        }
        printf("\n\n %ld frames recorded\n", wfsframe);
        fflush(stdout);
        copy_image_ID("_testdm0", dmname, 1);
        dmstate = 0;


        // Computing difference between consecutive images
        NBwfsframe = wfsframe;


        valarray = (double *) malloc(sizeof(double) * NBwfsframe);
        double valmax = 0.0;
        double valmaxdt = 0.0;
        for(kk = 1; kk < NBwfsframe; kk++) {
            valarray[kk] = 0.0;

            if(datatype == _DATATYPE_FLOAT)
                for(ii = 0; ii < wfssize; ii++) {
                    tmp = data.image[IDwfsc].array.F[kk * wfssize + ii] - data.image[IDwfsc].array.F[(kk - 1) * wfssize + ii];
                    valarray[kk] += tmp * tmp;
                }

            if(datatype == _DATATYPE_UINT16)
                for(ii = 0; ii < wfssize; ii++) {
                    tmp = data.image[IDwfsc].array.UI16[kk * wfssize + ii] - data.image[IDwfsc].array.UI16[(kk - 1) * wfssize + ii];
                    valarray[kk] += 1.0 * tmp * tmp;
                }

            if(datatype == _DATATYPE_INT16)
                for(ii = 0; ii < wfssize; ii++) {
                    tmp = 0.0;
                    tmp = data.image[IDwfsc].array.SI16[kk * wfssize + ii] - data.image[IDwfsc].array.SI16[(kk - 1) * wfssize + ii];
                    valarray[kk] += 1.0 * tmp * tmp;
                }
            valarray[kk] = sqrt(valarray[kk] / wfssize / 2);

            if(valarray[kk] > valmax) {
                valmax = valarray[kk];
                valmaxdt = 0.5 * (dtarray[kk - 1] + dtarray[kk]);
                kkmax = kk - kkoffset;
            }
        }


        //
        //
        //
        for(wfsframe = 1; wfsframe < NBwfsframe; wfsframe++) {
            fprintf(fp, "%ld   %10.2f     %g\n", wfsframe - kkoffset, 1.0e6 * (0.5 * (dtarray[wfsframe] + dtarray[wfsframe - 1]) - dtoffset), valarray[wfsframe]);
        }

        printf("mean interval =  %10.2f ns\n", 1.0e9 * (dt - dtoffset) / NBwfsframe);
        fflush(stdout);

        free(valarray);

        latency = valmaxdt - dtoffset;
        // latencystep = kkmax;

        printf("... Hardware latency = %f ms  = %ld frames\n", 1000.0 * latency, kkmax);
        if(latency > latencymax) {
            latencymax = latency;
            save_fl_fits("_testwfsc", "!./timingstats/maxlatencyseq.fits");
        }

        fprintf(fp, "# %5ld  %8.6f\n", iter, (valmaxdt - dtoffset));

        latencysteparray[iter] = 1.0 * kkmax;
        latencyarray[iter] = (valmaxdt - dtoffset);


        // process signals, increment loop counter
        processinfo_exec_end(processinfo);
        iter++;
    }
    fclose(fp);

    clock_gettime(CLOCK_REALTIME, &tnow);
    tdouble_end = 1.0 * tnow.tv_sec + 1.0e-9 * tnow.tv_nsec;
    wfscntend = data.image[IDwfs].md[0].cnt0;



    free(dtarray);
    free(tarray);


    sprintf(msgstring, "Processing Data (%ld iterations)", NBiter);
    processinfo_WriteMessage(processinfo, msgstring);



    latencyave = 0.0;
    latencystepave = 0.0;
    minlatency = latencyarray[0];
    maxlatency = latencyarray[0];
    for(iter = 0; iter < NBiter; iter++) {
        if(latencyarray[iter] > maxlatency) {
            maxlatency = latencyarray[iter];
        }

        if(latencyarray[iter] < minlatency) {
            minlatency = latencyarray[iter];
        }

        latencyave += latencyarray[iter];
        latencystepave += latencysteparray[iter];
    }
    latencyave /= NBiter;
    latencystepave /= NBiter;

    //save__fl_fits("_testwfsc", "!./timingstats/maxlatencyseq.fits");


    quick_sort_float(latencyarray, NBiter);

    printf("AVERAGE LATENCY = %8.3f ms   %f frames\n", latencyave * 1000.0, latencystepave);
    printf("min / max over %ld measurements: %8.3f ms / %8.3f ms\n", NBiter, minlatency * 1000.0, maxlatency * 1000.0);

    if(sprintf(command, "echo %8.6f > conf/param_hardwlatency.txt", latencyarray[NBiter / 2]) < 1) {
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    }

    if(system(command) != 0) {
        printERROR(__FILE__, __func__, __LINE__, "system() returns non-zero value");
    }

    if(sprintf(command, "echo %f %f %f %f %f > timingstats/hardwlatencyStats.txt", latencyarray[NBiter / 2], latencyave, minlatency, maxlatency, latencystepave) < 1) {
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    }

    if(system(command) != 0) {
        printERROR(__FILE__, __func__, __LINE__, "system() returns non-zero value");
    }


    dt = tdouble_end - tdouble_start;
    printf("FRAME RATE = %.3f Hz\n", 1.0 * (wfscntend - wfscntstart) / dt);

    if(sprintf(command, "echo %.3f > conf/param_mloopfrequ.txt", 1.0 * (wfscntend - wfscntstart) / dt) < 1) {
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    }

    if(system(command) != 0) {
        printERROR(__FILE__, __func__, __LINE__, "system() returns non-zero value");
    }

    free(latencyarray);
    free(latencysteparray);



    sprintf(msgstring, "Measured %8.3f ms @ %.3f Hz", latencyave * 1000.0, 1.0 * (wfscntend - wfscntstart));
    processinfo_WriteMessage(processinfo, msgstring);

    // ==================================
    // ENDING LOOP
    // ==================================
    processinfo_cleanExit(processinfo);


    return 0;
}












// waits on semaphore 3

long AOloopControl_perfTest_blockstats(long loop, const char *IDout_name)
{
    long IDout;
    uint32_t *sizeout;
    long NBmodes;
    char fname[200];
    long IDmodeval;
    long m, blk, i;
    long cnt;
    long IDblockRMS, IDblockRMS_ave;
    long NBblock;

    float *rmsarray;
    int *indexarray;

    float alpha = 0.0001;


    if(sprintf(fname, "aol%ld_modeval", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    IDmodeval = read_sharedmem_image(fname);
    NBmodes = data.image[IDmodeval].md[0].size[0];

    sizeout = (uint32_t*) malloc(sizeof(uint32_t)*2);
    sizeout[0] = NBmodes;
    sizeout[1] = 1;
    IDout = create_image_ID(IDout_name, 2, sizeout, _DATATYPE_FLOAT, 1, 0);
    COREMOD_MEMORY_image_set_createsem(IDout_name, 10);

    printf("%ld modes\n", NBmodes);


    m = 0;
    blk = 0;
    while(m<NBmodes)
    {
		long ID;
		long n;
		
        if(sprintf(fname, "aol%ld_DMmodes%02ld", loop, blk) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        ID = read_sharedmem_image(fname);
        n = data.image[ID].md[0].size[2];
        for(i=0; i<n; i++)
        {
            data.image[IDout].array.F[m] = blk;
            m++;
        }
        blk++;
    }
    NBblock = blk;

    rmsarray = (float*) malloc(sizeof(float)*NBblock);


    indexarray = (int*) malloc(sizeof(int)*NBmodes);
    for(m=0; m<NBmodes; m++)
        indexarray[m] = (int) (0.1 + data.image[IDout].array.F[m]);


    printf("NBblock = %ld\n", NBblock);
    sizeout[0] = NBblock;
    sizeout[1] = 1;

    if(sprintf(fname, "aol%ld_blockRMS", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    IDblockRMS = create_image_ID(fname, 2, sizeout, _DATATYPE_FLOAT, 1, 0);
    COREMOD_MEMORY_image_set_createsem(fname, 10);

    if(sprintf(fname, "aol%ld_blockRMS_ave", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    IDblockRMS_ave = create_image_ID(fname, 2, sizeout, _DATATYPE_FLOAT, 1, 0);
    COREMOD_MEMORY_image_set_createsem(fname, 10);


    cnt =  0;
    for(;;)
    {
        if(data.image[IDmodeval].md[0].sem==0)
        {
            while(cnt==data.image[IDmodeval].md[0].cnt0) // test if new frame exists
                usleep(5);
            cnt = data.image[IDmodeval].md[0].cnt0;
        }
        else
            sem_wait(data.image[IDmodeval].semptr[3]);

        for(blk=0; blk<NBblock; blk++)
            rmsarray[blk] = 0.0;

        for(m=0; m<NBmodes; m++)
            rmsarray[indexarray[m]] += data.image[IDmodeval].array.F[m]*data.image[IDmodeval].array.F[m];

        data.image[IDblockRMS].md[0].write = 1;
        for(blk=0; blk<NBblock; blk++)
            data.image[IDblockRMS].array.F[blk] = rmsarray[blk];
        COREMOD_MEMORY_image_set_sempost_byID(IDblockRMS, -1);
        data.image[IDblockRMS].md[0].cnt0++;
        data.image[IDblockRMS].md[0].write = 0;

        data.image[IDblockRMS_ave].md[0].write = 1;
        for(blk=0; blk<NBblock; blk++)
            data.image[IDblockRMS_ave].array.F[blk] = (1.0-alpha)* data.image[IDblockRMS_ave].array.F[blk] + alpha*rmsarray[blk];
        COREMOD_MEMORY_image_set_sempost_byID(IDblockRMS_ave, -1);
        data.image[IDblockRMS_ave].md[0].cnt0++;
        data.image[IDblockRMS_ave].md[0].write = 0;

    }

    free(sizeout);
    free(rmsarray);
    free(indexarray);


    return(IDout);
}





int_fast8_t AOloopControl_perfTest_InjectMode( long index, float ampl )
{
    if(aoloopcontrol_var.AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(1);

    if(aoloopcontrol_var.aoconfID_DMmodes==-1)
    {
		char name[200];
		
        if(sprintf(name, "aol%ld_DMmodes", LOOPNUMBER) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        aoloopcontrol_var.aoconfID_DMmodes = read_sharedmem_image(name);
    }

    if(aoloopcontrol_var.aoconfID_dmRM==-1)
        aoloopcontrol_var.aoconfID_dmRM = read_sharedmem_image(AOconf[LOOPNUMBER].DMctrl.dmRMname);


    if((index<0)||(index>AOconf[LOOPNUMBER].AOpmodecoeffs.NBDMmodes-1))
    {
        printf("Invalid mode index... must be between 0 and %ld\n", AOconf[LOOPNUMBER].AOpmodecoeffs.NBDMmodes);
    }
    else
    {
        float *arrayf;
		long i;
		
        arrayf = (float*) malloc(sizeof(float)*AOconf[LOOPNUMBER].DMctrl.sizeDM);

        for(i=0; i<AOconf[LOOPNUMBER].DMctrl.sizeDM; i++)
            arrayf[i] = ampl*data.image[aoloopcontrol_var.aoconfID_DMmodes].array.F[index*AOconf[LOOPNUMBER].DMctrl.sizeDM+i];



        data.image[aoloopcontrol_var.aoconfID_dmRM].md[0].write = 1;
        memcpy (data.image[aoloopcontrol_var.aoconfID_dmRM].array.F, arrayf, sizeof(float)*AOconf[LOOPNUMBER].DMctrl.sizeDM);
        data.image[aoloopcontrol_var.aoconfID_dmRM].md[0].cnt0++;
        data.image[aoloopcontrol_var.aoconfID_dmRM].md[0].write = 0;

        free(arrayf);
        AOconf[LOOPNUMBER].aorun.DMupdatecnt ++;
    }

    return(0);
}






//
// measure response matrix sensitivity
//
int_fast8_t AOloopControl_perfTest_AnalyzeRM_sensitivity(const char *IDdmmodes_name, const char *IDdmmask_name, const char *IDwfsref_name, const char *IDwfsresp_name, const char *IDwfsmask_name, float amplimitnm, float lambdanm, const char *foutname)
{
    FILE *fp;
    long IDdmmodes;
    long IDdmmask;
    long IDwfsref;
    long IDwfsresp;
    long IDwfsmask;
	double dmmodermscnt;
    long dmxsize, dmysize, dmxysize;
    long NBmodes;
    long wfsxsize, wfsysize, wfsxysize;
    long mode, mode1;

    long ii;


    double wfsmodermscnt;
    double tmp1;

    double wfsreftot, wfsmasktot;
    long IDoutXP, IDoutXP_WFS;
    double XPval;




    printf("amplimit = %f nm\n", amplimitnm);


    IDdmmodes = image_ID(IDdmmodes_name);
    dmxsize = data.image[IDdmmodes].md[0].size[0];
    dmysize = data.image[IDdmmodes].md[0].size[1];
    NBmodes = data.image[IDdmmodes].md[0].size[2];
    dmxysize = dmxsize * dmysize;

    IDdmmask = image_ID(IDdmmask_name);

    IDwfsref = image_ID(IDwfsref_name);
    wfsxsize = data.image[IDwfsref].md[0].size[0];
    wfsysize = data.image[IDwfsref].md[0].size[1];
    wfsxysize = wfsxsize * wfsysize;

    IDwfsresp = image_ID(IDwfsresp_name);
    IDwfsmask = image_ID(IDwfsmask_name);

    wfsreftot = 0.0;
    for(ii=0; ii<wfsxysize; ii++)
        wfsreftot += data.image[IDwfsref].array.F[ii];

    wfsmasktot = 0.0;
    for(ii=0; ii<wfsxysize; ii++)
        wfsmasktot += data.image[IDwfsmask].array.F[ii];


    list_image_ID();
    printf("NBmodes = %ld\n", NBmodes);
    printf("wfs size = %ld %ld\n", wfsxsize, wfsysize);
    printf("wfs resp ID : %ld\n", IDwfsresp);
    printf("wfs mask ID : %ld\n", IDwfsmask);

    printf("wfsmasktot = %f\n", wfsmasktot);

    fp = fopen(foutname, "w");

    fprintf(fp, "# col 1 : mode index\n");
    fprintf(fp, "# col 2 : average value (should be zero)\n");
    fprintf(fp, "# col 3 : DM mode RMS\n");
    fprintf(fp, "# col 4 : WFS mode RMS\n");
    fprintf(fp, "# col 5 : SNR for a 1um DM motion with 1 ph\n");
    fprintf(fp, "# col 6 : fraction of flux used in measurement\n");
    fprintf(fp, "# col 7 : Photon Efficiency\n");
    fprintf(fp, "\n");



    for(mode=0; mode<NBmodes; mode++)
    {
		double dmmoderms;
		double aveval;
		double SNR, SNR1; // single pixel SNR
		float frac = 0.0;
		float pcnt;
		double sigmarad;
		double eff; // efficiency
		double wfsmoderms;
		
		
        dmmoderms = 0.0;
        dmmodermscnt = 0.0;
        aveval = 0.0;
        for(ii=0; ii<dmxysize; ii++)
        {
            tmp1 = data.image[IDdmmodes].array.F[mode*dmxysize+ii]*data.image[IDdmmask].array.F[ii];
            aveval += tmp1;
            dmmoderms += tmp1*tmp1;
            dmmodermscnt += data.image[IDdmmask].array.F[ii];
        }
        dmmoderms = sqrt(dmmoderms/dmmodermscnt);
        aveval /= dmmodermscnt;

        SNR = 0.0;
        wfsmoderms = 0.0;
        wfsmodermscnt = 0.0;
        pcnt = 0.0;
        for(ii=0; ii<wfsxysize; ii++)
        {
            tmp1 = data.image[IDwfsresp].array.F[mode*wfsxysize+ii]*data.image[IDwfsmask].array.F[ii];
            wfsmoderms += tmp1*tmp1;
            wfsmodermscnt = 1.0;
            wfsmodermscnt += data.image[IDwfsmask].array.F[ii];

            if(data.image[IDwfsmask].array.F[ii]>0.1)
                if(data.image[IDwfsref].array.F[ii]>fabs(data.image[IDwfsresp].array.F[mode*wfsxysize+ii]*amplimitnm*0.001))
                {
                    SNR1 = data.image[IDwfsresp].array.F[mode*wfsxysize+ii]/sqrt(data.image[IDwfsref].array.F[ii]);
                    SNR1 /= wfsreftot;
                    SNR += SNR1*SNR1;
                    pcnt += data.image[IDwfsref].array.F[ii];
                }
        }
        frac = pcnt/wfsreftot;

        wfsmoderms = sqrt(wfsmoderms/wfsmodermscnt);
        SNR = sqrt(SNR); // SNR for 1 ph, 1um DM actuation
        // -> sigma for 1ph = 1/SNR [DMum]

        // 1umDM act = 2.0*M_PI * ( 2.0 / (lambdanm*0.001) ) rad WF
        // -> sigma for 1ph = (1/SNR) * 2.0*M_PI * ( 2.0 / (lambdanm*0.001) ) rad WF
        sigmarad = (1.0/SNR) * 2.0*M_PI * ( 2.0 / (lambdanm*0.001) );

        // SNR is in DMum per sqrt(Nph)
        // factor 2.0 for DM reflection

        eff = 1.0/(sigmarad*sigmarad);


        fprintf(fp, "%5ld   %16f   %16f   %16f    %16g      %12g        %12.10f\n", mode, aveval, dmmoderms, wfsmoderms, SNR, frac, eff);
    }

    fclose(fp);


    // computing DM space cross-product
    IDoutXP = create_2Dimage_ID("DMmodesXP", NBmodes, NBmodes);

    for(mode=0; mode<NBmodes; mode++)
        for(mode1=0; mode1<mode+1; mode1++)
        {
            XPval = 0.0;
            for(ii=0; ii<dmxysize; ii++)
                XPval += data.image[IDdmmask].array.F[ii]*data.image[IDdmmodes].array.F[mode*dmxysize+ii]*data.image[IDdmmodes].array.F[mode1*dmxysize+ii];

            data.image[IDoutXP].array.F[mode*NBmodes+mode1] = XPval/dmmodermscnt;
        }
    save_fits("DMmodesXP", "!DMmodesXP.fits");


    // computing WFS space cross-product
    IDoutXP_WFS = create_2Dimage_ID("WFSmodesXP", NBmodes, NBmodes);
    for(mode=0; mode<NBmodes; mode++)
        for(mode1=0; mode1<mode+1; mode1++)
        {
            XPval = 0.0;
            for(ii=0; ii<wfsxysize; ii++)
                XPval += data.image[IDwfsresp].array.F[mode*wfsxysize+ii]*data.image[IDwfsresp].array.F[mode1*wfsxysize+ii];

            data.image[IDoutXP_WFS].array.F[mode*NBmodes+mode1] = XPval/wfsxysize;
        }
    save_fits("WFSmodesXP", "!WFSmodesXP.fits");


    return(0);
}



//
// create dynamic test sequence
//
long AOloopControl_perfTest_mkTestDynamicModeSeq(const char *IDname_out, long NBpt, long NBmodes, long StartMode)
{
    long IDout;
    long xsize, ysize, xysize;
    long ii, kk;
    float ampl0;
    float ampl;
    float pha0;
    char name[200];
    long m, m1;
    if(aoloopcontrol_var.AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(1);

    if(aoloopcontrol_var.aoconfID_DMmodes==-1)
    {
        if(sprintf(name, "aol%ld_DMmodes", LOOPNUMBER) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        aoloopcontrol_var.aoconfID_DMmodes = read_sharedmem_image(name);
    }
    xsize = data.image[aoloopcontrol_var.aoconfID_DMmodes].md[0].size[0];
    ysize = data.image[aoloopcontrol_var.aoconfID_DMmodes].md[0].size[1];
    xysize = xsize*ysize;

    IDout = create_3Dimage_ID(IDname_out, xsize, ysize, NBpt);

    for(kk=0; kk<NBpt; kk++)
    {
        for(ii=0; ii<xysize; ii++)
            data.image[IDout].array.F[kk*xysize+ii] = 0.0;

        for(m=0; m<NBmodes; m++)
        {
			m1 = m + StartMode;
            ampl0 = 1.0;
            pha0 = M_PI*(1.0*m/NBmodes);
            ampl = ampl0 * sin(2.0*M_PI*(1.0*kk/NBpt)+pha0);
            for(ii=0; ii<xysize; ii++)
                data.image[IDout].array.F[kk*xysize+ii] += ampl * data.image[aoloopcontrol_var.aoconfID_DMmodes].array.F[m1*xysize+ii];
        }        
    }

    return(IDout);
}




//
// analysis of timing data
// Takes two args: 
//  looptiming FITS file
//  looptiming txt file
//

long AOloopControl_LoopTimer_Analysis(char *IDname, char *fnametxt, char *outfname)
{
	long ID;
	int NBtimer;
	long NBsample;
	FILE *fpout;
	FILE *fptxt;
	
	long frameNB;
	uint64_t *cnt0array;
	uint64_t *cnt1array;
	double *frameTimearray;
	long sp;
	long frNB;
	
	int timer;
	double timerval;
	
	
	// analysis
	long missedFrames;
	
	double *timer_ave;
	double *timer_min;
	double *timer_max;
	double *timer_dev;
	
	double rms;
	int ret;
	
	
	ID = image_ID(IDname);
	
	NBtimer = data.image[ID].md[0].size[0];
	NBsample = data.image[ID].md[0].size[2];
	fflush(stdout);
	
	cnt0array = (uint64_t *) malloc(sizeof(uint64_t) * NBsample);
	cnt1array = (uint64_t *) malloc(sizeof(uint64_t) * NBsample);
	frameTimearray = (double *) malloc(sizeof(double) * NBsample);
	
	timer_ave = (double*) malloc(sizeof(double) * NBtimer);
	timer_min = (double*) malloc(sizeof(double) * NBtimer);
	timer_max = (double*) malloc(sizeof(double) * NBtimer);
	timer_dev = (double*) malloc(sizeof(double) * NBtimer);
	
	double f1;
	long l1, l2;
	
	
	
	printf("%d timers\n", NBtimer);
	printf("%ld samples\n", NBsample);
	
	
	
	printf("Creating file \"%s\"\n", outfname);
	fflush(stdout);
	if( (fpout = fopen(outfname, "w")) == NULL)
	{
		printf("ERROR: cannot create file %s\n", outfname);
		exit(0);
	}
	
	printf("Opening file \"%s\"\n", fnametxt);
	fflush(stdout);
	if( (fptxt=fopen(fnametxt, "r")) == NULL)
	{
		printf("ERROR: cannot open file %s\n", fnametxt);
		exit(0);
	}
	
	fprintf(fpout, "# AOloopControl timing\n\n");
	
	list_image_ID();
	
	
	printf("Reading File %s\n\n", fnametxt);
	printf("\n");
	for(sp=0; sp< NBsample; sp++)
	{
		printf("\r     sample %ld / %ld                ", sp, NBsample);
		fflush(stdout);
		
		ret = fscanf(fptxt, "%ld %ld %ld %lf\n", &frNB, &l1, &l2, &f1); 

		cnt0array[sp] = l1;
		cnt1array[sp] = l2;
		frameTimearray[sp] = f1;
		
		fprintf(fpout, "%5ld  %10lu  %10lu  %18.9lf    ", sp, cnt0array[sp], cnt1array[sp], frameTimearray[sp]);
		
		
		if(sp==0)
		{
			for(timer=0; timer<NBtimer; timer++)
			{
				timer_min[timer] = data.image[ID].array.F[sp*NBtimer + timer];
				timer_max[timer] = data.image[ID].array.F[sp*NBtimer + timer];
			}
		}
		
		for(timer=0; timer<NBtimer; timer++)
		{	
			timerval = data.image[ID].array.F[sp*NBtimer + timer];
			fprintf(fpout, "  %12.9f", timerval);
		
			timer_ave[timer] += timerval;
			if(timerval < timer_min[timer])
				timer_min[timer] = timerval;
			if(timerval > timer_max[timer])
				timer_max[timer] = timerval;
		}
		fprintf(fpout, "\n");				
	}
	
	missedFrames = (cnt1array[NBsample-1]-cnt1array[0]) - NBsample;
	
	
	for(timer=0; timer<NBtimer; timer++)
	{
		timer_ave[timer] /= NBsample;
		
		rms = 0.0;
		for(sp=0; sp< NBsample; sp++)
		{
			timerval = data.image[ID].array.F[sp*NBtimer + timer];
			rms += (timerval - timer_ave[timer]) * (timerval - timer_ave[timer]);
		}
		timer_dev[timer] = sqrt(rms/NBsample);
	}
	printf("\n\n");
	
	
	// Print report
	printf("Missed frames   :   %5ld / %10ld  = %.6f\n", missedFrames, NBsample, 100.0*missedFrames/NBsample);
	printf("-------------------------------------------------\n");
	printf("| TIMER |   min   -   ave   -   max   | std dev |\n");
	printf("|  XXX  | xxxx.xx - xxxx.xx - xxxx.xx | xxxx.xx |\n");
	printf("-------------------------------------------------\n");
	for(timer=0; timer<NBtimer; timer++)
		printf("|  %3d  | %7.2f - %7.2f - %7.2f | %7.2f |\n", timer, timer_min[timer]*1e6, timer_ave[timer]*1e6, timer_max[timer]*1e6, timer_dev[timer]*1e6);
	printf("-------------------------------------------------\n");
	
	fclose(fpout);
	
	free(timer_ave);
	free(timer_min);
	free(timer_max);
	free(timer_dev);
	
	free(cnt0array);
	free(cnt1array);
	free(frameTimearray);
	
	return(0);
}






char *remove_ext (char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    // Error checks and allocate string.

    if (mystr == NULL)
        return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;

    // Make a copy and find the relevant characters.

    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    // If it has an extension separator.

    if (lastdot != NULL) {
        // and it's before the extenstion separator.

        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                // then remove it.

                *lastdot = '\0';
            }
        } else {
            // Has extension separator with no path separator.

            *lastdot = '\0';
        }
    }

    // Return the modified string.

    return retstr;
}














//
// WARNING: right=NBelem-1
//
void quicksort_StreamDataFile(StreamDataFile *datfile, long left, long right)
{
    register long i,j;
    StreamDataFile x, y;

    i = left;
    j = right;
    x.tstart = datfile[(left+right)/2].tstart;

    do {
        while(datfile[i].tstart < x.tstart && i<right) i++;
        while(x.tstart < datfile[j].tstart && j>left) j--;

        if(i<=j) {
			y.tstart = datfile[i].tstart;
            y.tend = datfile[i].tend;
            y.cnt = datfile[i].cnt;
            strcpy(y.name, datfile[i].name);
            
            datfile[i].tstart = datfile[j].tstart;
            datfile[i].tend = datfile[j].tend;
            datfile[i].cnt = datfile[j].cnt;
            strcpy(datfile[i].name, datfile[j].name);
            
            datfile[j].tstart = y.tstart;
            datfile[j].tend = y.tend;
            datfile[j].cnt = y.cnt;
            strcpy(datfile[j].name, y.name);
            
            i++;
            j--;
        }
    } while(i<=j);

    if(left<j) quicksort_StreamDataFile(datfile,left,j);
    if(i<right) quicksort_StreamDataFile(datfile,i,right);
}









/**
 * # Purpose
 *
 * Create timing summary file
 *
 */
int AOloopControl_perfTest_mkTimingFile(
    char *inTimingfname,
    char *outTimingfname,
    char *tmpstring
)
{
    FILE *fp;
    FILE *fpout;
    StreamDataFile datfile;
    long cnt;
    double valf1, valf2;
    long vald1, vald2, vald3, vald4;
    char line[512];
    long linecnt = 0;

    double *tarray;
    double MaxNBsample = 1000000;


    if((fp = fopen(inTimingfname, "r"))==NULL)
    {
        printf("Cannot open file \"%s\"\n", inTimingfname);
        exit(0);
    }
    else
    {
        double tlast = 0.0;
        int tOK = 1;
        int scanOK = 1;



        tarray = (double*) malloc(sizeof(double)*MaxNBsample);

        cnt = 0;

        while(scanOK == 1)
        {
            if( fgets(line, sizeof(line), fp) == NULL )
                scanOK = 0;
            else
            {
                if( line[0] != '#' )
                    scanOK = 1;
                
                if(scanOK==1)
                {
                    if((sscanf(line, "%ld %ld %lf %lf %ld %ld\n", &vald1, &vald2, &valf1, &valf2, &vald3, &vald4)==6) && (tOK==1))
                    {
						//printf("cnt %5ld read\n", cnt);//TEST
                        tarray[cnt] = valf2;

                        if(cnt == 0)
                        {
                            datfile.tstart = valf2;
                            tlast = valf2;
                        }
                        else
                        {
                            if (valf2 > tlast)
                                tOK = 1;
                            else
                                tOK = 0;
                            tlast = valf2;
                        }
                        cnt++;
                    }
                }
            }

            if(tOK==0)
                scanOK = 0;

            //printf("[%5ld] [%d] LINE: \"%s\"\n", linecnt, scanOK, line);
            linecnt++;

        }
        fclose(fp);
        datfile.tend = valf2;
        datfile.cnt = cnt;
        strcpy(datfile.name, tmpstring);

        free(tarray);

    }
    
    //printf("datfile.tstart  = %f\n", datfile.tstart);
    //printf("datfile.tend    = %f\n", datfile.tend);

    // write timing summary file

    if((fpout = fopen(outTimingfname, "w"))==NULL)
    {
        printf("Cannot write file \"%s\"\n", outTimingfname);
        exit(0);
    }
    else
    {
        fprintf(fpout, "%s   %20.9f %20.9f   %10ld  %10.3f\n", tmpstring, datfile.tstart, datfile.tend, datfile.cnt, datfile.cnt/(datfile.tend-datfile.tstart));
        fclose(fp);
    }

	

    return 0;
}





/**
 * # Purpose
 *
 * Synchronize two telemetry streams
 *
 * # Arguments
 *
 * savedir is the location of the telemetry, for example /media/data/20180202
 *
 * dtlag: positive when stream0 is earlier than stream1
 *
 */
int AOloopControl_perfTest_mkSyncStreamFiles2(
    char   *datadir,
    char   *stream0,
    char   *stream1,
    double  tstart,
    double  tend,
    double  dt,
    double  dtlag
)
{
    DIR *d0;
    struct dirent *dir;
    char datadirstream[500];
    char *ext;
    char *tmpstring;
    char line[512];


    StreamDataFile *datfile;
    long NBdatFiles;

    FILE *fp;
    char fname[500];
    long cnt;
    double valf1, valf2;
    long vald1, vald2, vald3, vald4;
    long i;

    uint32_t stream0xsize;
    uint32_t stream0ysize;
    uint32_t zsize;
    double *tstartarray;
    double *tendarray;
    double *exparray;
    double *exparray0;
    double *exparray1;
    long tstep;

    double *intarray_start;
    double *intarray_end;
    double *dtarray;

    long xysize;

    double dtoffset;

    long IDout0, IDout1;
    long xysize0, xysize1;
    long xsize0, ysize0, xsize1, ysize1;

    double dtlagarray[10]; // maximum 10 streams
    double medianexptimearray[10];



    // compute exposure start for each slice of output

    // How many frames are expected in the output ?
    zsize = (tend-tstart)/dt;
    printf("zsize = %ld\n", (long) zsize);
    fflush(stdout);


    // Should frame be kept or not ?
    int *frameOKarray;
    frameOKarray = (int*) malloc(sizeof(double)*zsize);


    // Allocate Working arrays and populate timing arrays

    tstartarray = (double*) malloc(sizeof(double)*zsize);
    tendarray   = (double*) malloc(sizeof(double)*zsize);
    exparray    = (double*) malloc(sizeof(double)*zsize);   // exposure time accumulated, in unit of input frame(s)
    exparray0   = (double*) malloc(sizeof(double)*zsize);
    exparray1   = (double*) malloc(sizeof(double)*zsize);
    for(tstep=0; tstep<zsize; tstep++)
    {
        tstartarray[tstep] = tstart + 1.0*tstep*(tend-tstart)/zsize;
        tendarray[tstep] = tstart + 1.0*(tstep+1)*(tend-tstart)/zsize;
        exparray[tstep] = 0.0;
        frameOKarray[tstep] = 1;
    }



    printf("tstart = %20.8f\n", tstart);
    printf("tend   = %20.8f\n", tend);


    dtlagarray[0] = dtlag;

    int stream;
    int NBstream = 2;
    for(stream=0; stream<NBstream; stream++)
    {
        for(tstep=0; tstep<zsize; tstep++)
            exparray[tstep] = 0.0;

        if(stream==0)
        {
            dtoffset = 0.0; // stream 0 is used as reference
            sprintf(datadirstream, "%s/%s", datadir, stream0);
        }
        else
        {
            dtoffset = +dtlag; // stream 1 is lagging behind by dtlag, so we bring it back in time
            // this is achieved by pushing/delaying the output timing window
            sprintf(datadirstream, "%s/%s", datadir, stream1);
        }

        datfile = (StreamDataFile*) malloc(sizeof(StreamDataFile)*MaxNBdatFiles);
        //
        // Identify relevant files in directory
        //

        printf("SCANNING directory (stream %d)  %s ... \n", stream, datadirstream);
        fflush(stdout);

        NBdatFiles = 0;
        d0 = opendir(datadirstream);
        if (d0) {
            while ((dir = readdir(d0)) != NULL) {
                ext = strrchr(dir->d_name, '.');
                if (!ext) {
                    // printf("no extension\n");
                } else {
                    int datfileOK = 0;
                    if(strcmp(ext+1, "dat")==0)
                        datfileOK = 1;
                    if(strcmp(ext+1, "txt")==0)
                        datfileOK = 2;

                    if(datfileOK != 0)
                    {
                        int mkTiming;
                        float tmpv;
                        int ret;

                        tmpstring = remove_ext(dir->d_name, '.', '/');



                        // TIMING FILE
                        //

                        // Does timing file exist ?
                        sprintf(fname, "%s/%s.timing", datadirstream, tmpstring);
                        if ( (fp=fopen(fname, "r")) == NULL )
                        {
                            char fnamein[256];

                            printf("File %s : No timing info found -> creating\n", fname);

                            if(datfileOK == 1)
                                sprintf(fnamein, "%s/%s.dat", datadirstream, tmpstring);
                            else
                                sprintf(fnamein, "%s/%s.txt", datadirstream, tmpstring);

                            printf("input  : %s\n", fnamein);
                            printf("output : %s\n", fname);

                            AOloopControl_perfTest_mkTimingFile(fnamein, fname, tmpstring);


                            if ( (fp=fopen(fname, "r")) == NULL )
                            {
                                printf("ERROR: can't open file %s\n", fname);
                                exit(0);
                            }

                        }





                        // read timing file

                        int scanOK = 1; // keep scanning file
                        int readOK = 0; // read successful
                        int linenb = 0;
                        while(scanOK==1)
                        {
                            //printf("Reading line %d\n", linenb); //TEST

                            if( fgets(line, sizeof(line), fp) == NULL )
                                scanOK = 0;

                            if( line[0] != '#' )
                            {
                                ret = sscanf(line, "%s   %lf %lf   %ld  %f\n",
                                             tmpstring,
                                             &datfile[NBdatFiles].tstart,
                                             &datfile[NBdatFiles].tend,
                                             &datfile[NBdatFiles].cnt,
                                             &tmpv);

                                if(ret == 5)
                                {
                                    mkTiming = 0;
                                    strcpy(datfile[NBdatFiles].name, tmpstring);
                                    //printf("File %s : timing info found\n", fname);
                                    scanOK = 0; // done reading
                                    readOK = 1;
                                }
                            }
                        }
                        fclose(fp);

                        if(readOK==0)
                        {
                            printf("File %s corrupted \n", fname);
                            exit(0);
                        }



                        if((datfile[NBdatFiles].tstart < tend)&&(datfile[NBdatFiles].tend > tstart))
                            NBdatFiles++;
                    }


                }
            }
            closedir(d0);
        }


        printf("\ndone\n");
        fflush(stdout);

        printf("NBdatFiles = %ld\n", NBdatFiles);


        for(i=0; i<NBdatFiles; i++)
        {
            printf("FILE [%ld]: %20s       %20.9f -> %20.9f   [%10ld]  %10.3f Hz\n", i,
                   datfile[i].name,
                   datfile[i].tstart,
                   datfile[i].tend,
                   datfile[i].cnt,
                   datfile[i].cnt/(datfile[i].tend-datfile[i].tstart));
        }


        printf("==========================================================\n");


        // sort files according to time
        if(NBdatFiles>1)
            quicksort_StreamDataFile(datfile, 0, NBdatFiles-1);

        for(i=0; i<NBdatFiles; i++)
        {
            printf("FILE [%ld]: %20s       %20.9f -> %20.9f   [%10ld]  %10.3f Hz\n", i,
                   datfile[i].name,
                   datfile[i].tstart,
                   datfile[i].tend,
                   datfile[i].cnt,
                   datfile[i].cnt/(datfile[i].tend-datfile[i].tstart));
        }


        printf("==========================================================\n");




        int initOutput = 0;
        long xsize, ysize;
        long IDout;

        for(i=0; i<NBdatFiles; i++)
        {
            printf("FILE: %20s       %20.9f -> %20.9f   [%10ld]  %10.3f Hz\n",
                   datfile[i].name,
                   datfile[i].tstart,
                   datfile[i].tend,
                   datfile[i].cnt,
                   datfile[i].cnt/(datfile[i].tend-datfile[i].tstart));

            // LOAD FITS FILE
            long IDc;
            sprintf(fname, "%s/%s.fits", datadirstream, datfile[i].name);
            printf("----------------------[%ld] LOADING FILE %s\n", i, fname);
            IDc = load_fits(fname, "im0C", 2);


            // CREATE OUTPUT CUBE IF FIRST FILE
            if(initOutput == 0)
            {
                xsize = data.image[IDc].md[0].size[0];
                ysize = data.image[IDc].md[0].size[1];
                xysize = xsize*ysize;
                if(stream==0)
                {
                    IDout = create_3Dimage_ID("out0", xsize, ysize, zsize);
                    IDout0 = IDout;
                    xysize0 = xysize;
                    xsize0 = xsize;
                    ysize0 = ysize;
                }
                else
                {
                    IDout = create_3Dimage_ID("out1", xsize, ysize, zsize);
                    IDout1 = IDout;
                    xysize1 = xysize;
                    xsize1 = xsize;
                    ysize1 = ysize;
                }
                initOutput = 1;
            }

            // start and end time for input exposures
            intarray_start = (double*) malloc(sizeof(double)*datfile[i].cnt);
            intarray_end   = (double*) malloc(sizeof(double)*datfile[i].cnt);
            dtarray = (double*) malloc(sizeof(double)*datfile[i].cnt);


            long j;

            sprintf(fname, "%s/%s.dat", datadirstream, datfile[i].name);
            printf("fname = %s\n", fname);
            fflush(stdout);

            if((fp = fopen(fname, "r"))==NULL)
            {
                sprintf(fname, "%s/%s.txt", datadirstream, datfile[i].name);

                if((fp = fopen(fname, "r"))==NULL) {
                    printf("Cannot open file \"%s.dat\" or \"%s.txt\"\n", datfile[i].name, datfile[i].name);
                    exit(0);
                }
            }


            int scanOK = 1;
            j = 0;
            while(scanOK == 1)
            {
                if( fgets(line, sizeof(line), fp) == NULL )
                    scanOK = 0;
                else
                {
                    if( line[0] != '#' )
                        scanOK = 1;

                    if(scanOK==1)
                    {
                        if(sscanf(line, "%ld %ld %lf %lf %ld %ld\n", &vald1, &vald2, &valf1, &valf2, &vald3, &vald4)==6)
                        {
                            intarray_end[j] = valf2;
                            j++;
                            if(j==datfile[i].cnt)
                                scanOK = 0;
                        }
                    }
                }
            }




/*
            for(j=0; j<datfile[i].cnt; j++)
            {
                if(fscanf(fp, "%ld %ld %lf %lf %ld %ld\n", &vald1, &vald2, &valf1, &valf2, &vald3, &vald4)!=6)
                {
                    printf("fscanf error, %s line %d\n", __FILE__, __LINE__);
                    exit(0);
                }
                else
                    intarray_end[j] = valf2;
            }*/
            fclose(fp);






            for(j=0; j<datfile[i].cnt-1; j++)
                dtarray[j] = intarray_end[j+1] - intarray_end[j];


            double dtmedian;
            qs_double(dtarray, 0, datfile[i].cnt-1);
            dtmedian = dtarray[(datfile[i].cnt-1)/2];
            printf("   dtmedian = %10.3f us\n", 1.0e6*dtmedian);

            // we assume here that every frame has the same exposure time, with 100% duty cycle
            for(j=0; j<datfile[i].cnt; j++)
                intarray_start[j] = intarray_end[j] - dtmedian;


            int j0 = 0;
            double expfrac;


            for(tstep=0; tstep<zsize; tstep++)
            {
                while((intarray_end[j0] < (tstartarray[tstep]+dtoffset) ) && (j0 < datfile[i].cnt))
                    j0++;
                j = j0;

                while( (intarray_start[j] < (tendarray[tstep]+dtoffset)) && (j<datfile[i].cnt) )
                {
                    expfrac = 1.0;

                    if((tstartarray[tstep]+dtoffset)>intarray_start[j])
                        expfrac -= ((tstartarray[tstep]+dtoffset)-intarray_start[j])/dtmedian;

                    if((tendarray[tstep]+dtoffset)<intarray_end[j])
                        expfrac -= (intarray_end[j]-(tendarray[tstep]+dtoffset))/dtmedian;

                    exparray[tstep] += expfrac;

                    //                    printf("  FILE %d        %5ld   %8.6f  [%20.6f] -> %5ld\n", i, j, expfrac, intarray_start[j], tstep);

                    long ii;

                    switch(data.image[IDc].md[0].datatype)
                    {
                    case _DATATYPE_UINT8 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.UI8[xysize*j+ii];
                        break;

                    case _DATATYPE_INT8 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.SI8[xysize*j+ii];
                        break;

                    case _DATATYPE_UINT16 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.UI16[xysize*j+ii];
                        break;

                    case _DATATYPE_INT16 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.SI16[xysize*j+ii];
                        break;

                    case _DATATYPE_UINT32 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.UI32[xysize*j+ii];
                        break;

                    case _DATATYPE_INT32 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.SI32[xysize*j+ii];
                        break;

                    case _DATATYPE_UINT64 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.UI64[xysize*j+ii];
                        break;

                    case _DATATYPE_INT64 :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.SI64[xysize*j+ii];
                        break;

                    case _DATATYPE_FLOAT :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.F[xysize*j+ii];
                        break;

                    case _DATATYPE_DOUBLE :
                        for (ii = 0; ii < xysize; ii++)
                            data.image[IDout].array.F[xysize*tstep+ii] += expfrac*data.image[IDc].array.D[xysize*j+ii];
                        break;

                    default :
                        list_image_ID();
                        printERROR(__FILE__,__func__,__LINE__,"datatype value not recognised");
                        printf("ID %ld  datatype = %d\n", IDc, data.image[IDc].md[0].datatype);
                        exit(0);
                        break;
                    }
                    j++;
                }
            }


            delete_image_ID("im0C");

            free(intarray_start);
            free(intarray_end);
            free(dtarray);

        }

        for(tstep=0; tstep<zsize; tstep++)
        {
            if(exparray[tstep] > 0.01)
            {
                long ii;
                for(ii=0; ii<xysize; ii++)
                    data.image[IDout].array.F[xysize*tstep+ii] /= exparray[tstep];
            }
        }


        printf("zsize = %ld\n", (long) zsize);
        fflush(stdout);

        // COMPUTE MEDIAN EXPTIME
        if(stream==0)
            memcpy(exparray0, exparray, sizeof(double)*zsize);
        else
            memcpy(exparray1, exparray, sizeof(double)*zsize);

        quick_sort_double(exparray, zsize);

        double exptmedian;
        exptmedian = exparray[zsize/2];
        medianexptimearray[stream] = exptmedian;
        printf("Median Exp Time = %6.3f\n", exptmedian);

        if(stream==0)
            memcpy(exparray, exparray0, sizeof(double)*zsize);
        else
            memcpy(exparray, exparray1, sizeof(double)*zsize);

        // SELECTION
        for(tstep=0; tstep<zsize; tstep++)
        {
            if(exparray[tstep]<0.8*exptmedian)
                frameOKarray[tstep] = 0;
            if(exparray[tstep]>1.2*exptmedian)
                frameOKarray[tstep] = 0;
        }

        free(datfile);
    }


    long NBmissingFrame = 0;
    for(tstep=0; tstep<zsize; tstep++)
        if(frameOKarray[tstep] == 0)
            NBmissingFrame++;


    sprintf(fname, "exptime.dat");
    fp = fopen(fname, "w");

    fprintf(fp, "# Exposure time per output frame, unit = input frame\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Generated by function %s in file %s\n", __FUNCTION__, __FILE__);
    fprintf(fp, "# stream0 : %s\n", stream0);
    fprintf(fp, "# stream1 : %s\n", stream1);
    fprintf(fp, "# tstart  : %f\n", tstart);
    fprintf(fp, "# tend    : %f\n", tend);
    fprintf(fp, "# dt      : %f\n", dt);
    fprintf(fp, "#\n");
    fprintf(fp, "# stream0 median exp time : %6.3f frame -> %8.3f Hz\n", medianexptimearray[0], medianexptimearray[0]/dt);
    fprintf(fp, "# stream1 median exp time : %6.3f frame -> %8.3f Hz\n", medianexptimearray[1], medianexptimearray[1]/dt);
    fprintf(fp, "# missing frames : %6ld / %ld  ( %10.6f %%)\n", NBmissingFrame, (long) zsize, 100.0*NBmissingFrame/zsize);
    fprintf(fp, "#\n");
    fprintf(fp, "# col 1 :   time step\n");
    fprintf(fp, "# col 2 :   output frame index (valid if OK flag = 1)\n");
    fprintf(fp, "# col 3 :   OK flag\n");
    fprintf(fp, "# col 4 :   time (stream0)\n");
    fprintf(fp, "# col 5 :   stream0 exposure time\n");
    fprintf(fp, "# col 6 :   stream1 exposure time\n");
    fprintf(fp, "#\n");

    long NBframeOK = 0;
    for(tstep=0; tstep<zsize; tstep++)
    {
        fprintf(fp, "%5ld %5ld %d %10.6f %10.6f %10.6f\n", tstep, NBframeOK, frameOKarray[tstep], tstartarray[tstep], exparray0[tstep], exparray1[tstep]);
        if(frameOKarray[tstep]==1)
        {
            if(tstep!=NBframeOK)
            {
                void *ptr0;
                void *ptr1;

                ptr0 = (char*) data.image[IDout0].array.F + sizeof(float)*xysize0*tstep;
                ptr1 = (char*) data.image[IDout0].array.F + sizeof(float)*xysize0*NBframeOK;
                memcpy((void*) ptr1, (void*) ptr0, sizeof(float)*xysize0);

                ptr0 = (char*) data.image[IDout1].array.F + sizeof(float)*xysize1*tstep;
                ptr1 = (char*) data.image[IDout1].array.F + sizeof(float)*xysize1*NBframeOK;
                memcpy((void*) ptr1, (void*) ptr0, sizeof(float)*xysize1);

            }
            NBframeOK++;
        }
    }
    fclose(fp);


    free(tstartarray);
    free(tendarray);
    free(exparray);
    free(exparray0);
    free(exparray1);
    free(frameOKarray);

    if(NBframeOK>0)
    {
        long IDoutc0;
        IDoutc0 = create_3Dimage_ID("outC0", xsize0, ysize0, NBframeOK);
        memcpy(data.image[IDoutc0].array.F, data.image[IDout0].array.F, sizeof(float)*xysize0*NBframeOK);

        long IDoutc1;
        IDoutc1 = create_3Dimage_ID("outC1", xsize1, ysize1, NBframeOK);
        memcpy(data.image[IDoutc1].array.F, data.image[IDout1].array.F, sizeof(float)*xysize1*NBframeOK);
    }
    delete_image_ID("out0");
    delete_image_ID("out1");


    return 0;
}



/**
 * # Purpose
 *
 * Compute similarity matrix between frames of a datacube
 *
 */

int AOloopControl_perfTest_ComputeSimilarityMatrix(
    char *IDname,
    char *IDname_out
)
{
    long ID, IDout;
    uint32_t xsize, ysize, xysize, zsize;
    long k1, k2;
    long cnt = 0;
    float *array1;
    float *array2;
    char *srcptr;

    int perccomplete;
    int perccompletelast = 0;



    ID = image_ID(IDname);
    xsize = data.image[ID].md[0].size[0];
    ysize = data.image[ID].md[0].size[1];
    xysize = xsize*ysize;
    zsize = data.image[ID].md[0].size[2];

    array1 = (float*) malloc(sizeof(float)*xysize);
    array2 = (float*) malloc(sizeof(float)*xysize);


    IDout = create_2Dimage_ID(IDname_out, zsize, zsize);
    printf("\n");
    for(k1=0; k1<zsize; k1++)
    {
        srcptr = (char*) data.image[ID].array.F + sizeof(float)*xysize*k1;

        memcpy(array1, srcptr, sizeof(float)*xysize);

        for(k2=0; k2<k1; k2++)
        {
            double val = 0.0;
            long ii;
            double v0;

            srcptr = (char*) data.image[ID].array.F + sizeof(float)*xysize*k2;
            memcpy(array2, srcptr, sizeof(float)*xysize);

            perccomplete = (int) (100.0*cnt/(zsize*(zsize-1)/2));
            if(perccompletelast<perccomplete)
            {
                printf("\r [%5.2f %%]   %5ld / %5ld    %5ld / %5ld     ", 100.0*cnt/(zsize*(zsize-1)/2), k1, (long) zsize, k2, (long) zsize);
                fflush(stdout);
                perccompletelast = perccomplete;
            }

            for(ii=0; ii<xysize; ii++)
            {
                v0 = (array1[ii] - array2[ii]);
                val += v0*v0;
            }

            data.image[IDout].array.F[k1*zsize+k2] = val;
            cnt++;
        }
    }
    printf("\n");

    free(array1);
    free(array2);

    return 0;
}





/**
 * # Purpose
 *
 * Perform statistical analysis of two streams from similarity matrices
 *
 * # Details
 *
 * Selects the NBselected most similar pairs in stream0 and stream1 separated by at least dtmin frames
 *
 * Computes the differences between the corresponding pairs in the other stream
 *
 * # Output
 *
 * sim0pairs.txt  : best NBselected stream0 pairs\n
 * sim1pairs.txt  : best NBselected stream1 pairs\n
 * sim2Ddistrib   : 2D similarity distribution image\n
 *
 * sim0diff0      : best sim pairs 0, differences stream 0 images\n
 * sim0diff1      : best sim pairs 0, differences stream 0 images\n
 * sim1diff0      : best sim pairs 0, differences stream 0 images\n
 * sim1diff1      : best sim pairs 0, differences stream 0 images\n
 *
 */

int AOloopControl_perfTest_StatAnalysis_2streams(
    char *IDname_stream0,
    char *IDname_stream1,
    char *IDname_simM0,
    char *IDname_simM1,
    long dtmin,
    long NBselected
)
{
    long IDstream0;
    long IDstream1;
    long IDsimM0;
    long IDsimM1;

    long NBpairMax;

    // similarity pairs extracted from stream0
    long *sim0pair_k1;
    long *sim0pair_k2;
    double *sim0pair_val;

    // similarity pairs extracted from stream1
    long *sim1pair_k1;
    long *sim1pair_k2;
    double *sim1pair_val;

    long xsize0, ysize0, NBframe0, xysize0;
    long xsize1, ysize1, NBframe1, xysize1;
    long k1, k2;
    long paircnt;


    // ouput
    long pair;
    FILE *fpout0;
    FILE *fpout1;

    double mediansim0, mediansim1;




    IDstream0 = image_ID(IDname_stream0);
    IDstream1 = image_ID(IDname_stream1);
    IDsimM0 = image_ID(IDname_simM0);
    IDsimM1 = image_ID(IDname_simM1);

    xsize0 = data.image[IDstream0].md[0].size[0];
    ysize0 = data.image[IDstream0].md[0].size[1];
    xysize0 = xsize0*ysize0;
    NBframe0 = data.image[IDstream0].md[0].size[2];

    xsize1 = data.image[IDstream1].md[0].size[0];
    ysize1 = data.image[IDstream1].md[0].size[1];
    xysize1 = xsize1*ysize1;
    NBframe1 = data.image[IDstream1].md[0].size[2];


    // a few checks before proceeding
    if(NBframe0!=NBframe1)
    {
        printf("[%s] [%s] [%d]  ERROR: NBframe0 (%ld) != NBframe1 (%ld)\n", __FILE__, __FUNCTION__, __LINE__, NBframe0, NBframe1);
        exit(0);
    }

    if(NBframe0!=data.image[IDsimM0].md[0].size[0])
    {
        printf("[%s] [%s] [%d]  ERROR: NBframe0 (%ld) != simM0 xsize (%ld)\n", __FILE__, __FUNCTION__, __LINE__, NBframe0, (long) data.image[IDsimM0].md[0].size[0]);
        exit(0);
    }

    if(NBframe0!=data.image[IDsimM0].md[0].size[1])
    {
        printf("[%s] [%s] [%d]  ERROR: NBframe0 (%ld) != simM0 ysize (%ld)\n", __FILE__, __FUNCTION__, __LINE__, NBframe0, (long) data.image[IDsimM0].md[0].size[1]);
        exit(0);
    }

    if(NBframe1!=data.image[IDsimM1].md[0].size[0])
    {
        printf("[%s] [%s] [%d]  ERROR: NBframe0 (%ld) != simM0 xsize (%ld)\n", __FILE__, __FUNCTION__, __LINE__, NBframe1, (long) data.image[IDsimM1].md[0].size[0]);
        exit(0);
    }

    if(NBframe1!=data.image[IDsimM1].md[0].size[1])
    {
        printf("[%s] [%s] [%d]  ERROR: NBframe0 (%ld) != simM0 ysize (%ld)\n", __FILE__, __FUNCTION__, __LINE__, NBframe1, (long) data.image[IDsimM1].md[0].size[1]);
        exit(0);
    }










    NBpairMax = data.image[IDsimM0].md[0].size[0]*(data.image[IDsimM0].md[0].size[0]-1)/2;

    sim0pair_k1 = (long*) malloc(sizeof(long)*NBpairMax);
    sim0pair_k2 = (long*) malloc(sizeof(long)*NBpairMax);
    sim0pair_val = (double*) malloc(sizeof(double)*NBpairMax);

    sim1pair_k1 = (long*) malloc(sizeof(long)*NBpairMax);
    sim1pair_k2 = (long*) malloc(sizeof(long)*NBpairMax);
    sim1pair_val = (double*) malloc(sizeof(double)*NBpairMax);


    paircnt = 0;
    for(k1=0; k1<NBframe0; k1++)
        for(k2=0; k2<k1; k2++)
        {
            if((k1-k2)>dtmin)
            {
                if(paircnt > NBpairMax-1)
                {
                    printf("[%s] [%s] [%d]  ERROR: paircnt (%ld) >= NBpairMax (%ld)\n", __FILE__, __FUNCTION__, __LINE__, paircnt, NBpairMax);
                    printf("NBframe0 = %ld\n", NBframe0);

                    exit(0);
                }

                sim0pair_k1[paircnt] = k1;
                sim0pair_k2[paircnt] = k2;
                sim0pair_val[paircnt] = data.image[IDsimM0].array.F[k1*NBframe0+k2];

                sim1pair_k1[paircnt] = k1;
                sim1pair_k2[paircnt] = k2;
                sim1pair_val[paircnt] = data.image[IDsimM1].array.F[k1*NBframe1+k2];

                paircnt++;
            }
        }



    quick_sort3ll_double(sim0pair_val, sim0pair_k1, sim0pair_k2, paircnt);
    quick_sort3ll_double(sim1pair_val, sim1pair_k1, sim1pair_k2, paircnt);
    mediansim0 = sim0pair_val[paircnt/2];
    mediansim1 = sim1pair_val[paircnt/2];

    if( (fpout0 = fopen("sim0pairs.txt", "w")) == NULL)
    {
        printf("[%s] [%s] [%d]  ERROR: cannot create file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(0);
    }
    for(pair=0; pair<NBselected; pair++)
    {
        k1 = sim0pair_k1[pair];
        k2 = sim0pair_k2[pair];
        fprintf(fpout0, "%5ld  %5ld  %5ld  %8.6f  %8.6f\n", pair, k1, k2, data.image[IDsimM0].array.F[k1*NBframe0+k2]/mediansim0, data.image[IDsimM1].array.F[k1*NBframe0+k2]/mediansim1);
    }
    fclose(fpout0);


    if( (fpout1 = fopen("sim1pairs.txt", "w")) == NULL)
    {
        printf("[%s] [%s] [%d]  ERROR: cannot create file\n", __FILE__, __FUNCTION__, __LINE__);
        exit(0);
    }
    for(pair=0; pair<NBselected; pair++)
    {
        k1 = sim1pair_k1[pair];
        k2 = sim1pair_k2[pair];
        fprintf(fpout1, "%5ld  %5ld  %5ld  %8.6f  %8.6f\n", pair, k1, k2, data.image[IDsimM0].array.F[k1*NBframe0+k2]/mediansim0, data.image[IDsimM1].array.F[k1*NBframe1+k2]/mediansim1);
    }
    fclose(fpout1);



    // Create 2D distribution of similarities

    uint32_t xsize2Ddistrib = 512;
    uint32_t ysize2Ddistrib = 512;

    long IDsim2Ddistrib = create_2Dimage_ID("sim2Ddistrib", xsize2Ddistrib, ysize2Ddistrib);

    for(k1=0; k1<NBframe0; k1++)
        for(k2=0; k2<k1; k2++)
        {
            if((k1-k2)>dtmin)
            {
                float x, y;
                long ii, jj;

                x = data.image[IDsimM0].array.F[k1*NBframe0+k2] / mediansim0;
                y = data.image[IDsimM1].array.F[k1*NBframe1+k2] / mediansim1;

                ii = (uint32_t) (0.5*x*xsize2Ddistrib);
                jj = (uint32_t) (0.5*y*ysize2Ddistrib);

                if((ii<xsize2Ddistrib)&&(jj<ysize2Ddistrib))
                    data.image[IDsim2Ddistrib].array.F[jj*xsize2Ddistrib+ii] += 1.0;
            }
        }


    long IDsim0diff0 = create_3Dimage_ID("sim0diff0", xsize0, ysize0, NBselected);
    long IDsim0diff1 = create_3Dimage_ID("sim0diff1", xsize1, ysize1, NBselected);

    for(pair=0; pair<NBselected; pair++)
    {
        long ii;

        k1 = sim0pair_k1[pair];
        k2 = sim0pair_k2[pair];

        for(ii=0; ii<xysize0; ii++)
            data.image[IDsim0diff0].array.F[pair*xysize0+ii] = data.image[IDstream0].array.F[k1*xysize0+ii] - data.image[IDstream0].array.F[k2*xysize0+ii];
        for(ii=0; ii<xysize1; ii++)
            data.image[IDsim0diff1].array.F[pair*xysize1+ii] = data.image[IDstream1].array.F[k1*xysize1+ii] - data.image[IDstream1].array.F[k2*xysize1+ii];
    }


    long IDsim1diff0 = create_3Dimage_ID("sim1diff0", xsize0, ysize0, NBselected);
    long IDsim1diff1 = create_3Dimage_ID("sim1diff1", xsize1, ysize1, NBselected);

    for(pair=0; pair<NBselected; pair++)
    {
        long ii;

        k1 = sim1pair_k1[pair];
        k2 = sim1pair_k2[pair];

        for(ii=0; ii<xysize0; ii++)
            data.image[IDsim1diff0].array.F[pair*xysize0+ii] = data.image[IDstream0].array.F[k1*xysize0+ii] - data.image[IDstream0].array.F[k2*xysize0+ii];
        for(ii=0; ii<xysize1; ii++)
            data.image[IDsim1diff1].array.F[pair*xysize1+ii] = data.image[IDstream1].array.F[k1*xysize1+ii] - data.image[IDstream1].array.F[k2*xysize1+ii];
    }



    free(sim0pair_k1);
    free(sim0pair_k2);
    free(sim0pair_val);

    free(sim1pair_k1);
    free(sim1pair_k2);
    free(sim1pair_val);

    return 0;
}





/**
 * 
 * PSF evaluation window is (x0,y0) to (x1,y1)
 * 
 * Optional input: PSFmask, to be multiplied by PSF
 * 
 * 
 * EvalMode = 0  : Maximize Energy concentration
 * EvalMode = 1  : Maximize flux
 * EvalMode = 2  : Minimize flux
 * 
 * output:
 * 
 * imwfsbest
 * imwfsall
 * 
 * impsfbest
 * impsfall
 * 
 * 
 * 
 */ 

int AOloopControl_perfTest_SelectWFSframes_from_PSFframes(char *IDnameWFS, char *IDnamePSF, float frac, long x0, long x1, long y0, long y1, int EvalMode, float alpha)
{
	long IDwfs;
	long IDpsf;
	long IDpsfmask;   // optional

	long NBframe;
	long xsizewfs, ysizewfs, xysizewfs;
	long xsizepsf, ysizepsf, xysizepsf;
	
	double *evalarray;
	long *indexarray;
	
	IDwfs = image_ID(IDnameWFS);
	IDpsf = image_ID(IDnamePSF);
	
	xsizewfs = data.image[IDwfs].md[0].size[0];
	ysizewfs = data.image[IDwfs].md[0].size[1];
	xysizewfs = xsizewfs*ysizewfs;
	
	xsizepsf = data.image[IDpsf].md[0].size[0];
	ysizepsf = data.image[IDpsf].md[0].size[1];
	xysizepsf = xsizepsf*ysizepsf;	
	
	NBframe = data.image[IDwfs].md[0].size[2];
	
	evalarray = (double*) malloc(sizeof(double)*NBframe);
	indexarray = (long*) malloc(sizeof(long)*NBframe);
	
	long x0t, y0t, x1t, y1t;
	x0t = x0;
	x1t = x1;
	y0t = y0;
	y1t = y1;
	
	if(x0<0)
		x0t = x0;
	if(x1>xsizepsf-1)
		x1t = xsizepsf-1;
	if(y0<0)
		y0t = y0;
	if(y1>ysizepsf-1)
		y1t = ysizepsf-1;	
	
	printf("WINDOW: %ld - %ld     %ld -%ld\n", x0t, x1t, y0t, y1t);
	
	long kk;
	IDpsfmask = image_ID("PSFmask");
	if(IDpsfmask != -1)
	{
		for(kk=0;kk<NBframe;kk++)
		{
			long ii, jj;
			
			for(ii=x0t;ii<x1t;ii++)
				for(jj=y0t;jj<y1t;jj++)
					data.image[IDpsf].array.F[kk*xysizepsf+jj*xsizepsf+ii] *= data.image[IDpsfmask].array.F[jj*xsizepsf+ii];
		}
	}
	
	
	
	for(kk=0;kk<NBframe;kk++)
	{
		long ii, jj;
		double sum = 0.0;
		double ssum = 0.0;
		
		indexarray[kk] = kk;

		for(ii=x0t;ii<x1t;ii++)
			for(jj=y0t;jj<y1t;jj++)
			{
				float tval;
				tval  = data.image[IDpsf].array.F[kk*xysizepsf+jj*xsizepsf+ii];
				if(tval<0.0)
					tval = 0.0;
				sum += tval;
				ssum += pow(tval,alpha);				
			}		
		
		// best frame 
		switch (EvalMode) 
		{
			case 0 :
				evalarray[kk] = -(ssum/(pow(sum,alpha)));
			break;
			
			case 1 :
				evalarray[kk] = -sum;
			break;
			
			case 2 :
				evalarray[kk] = sum;
			break;
			
			default:
				evalarray[kk] = -sum;
			break;
		}
	}
	
	quick_sort2l(evalarray, indexarray, NBframe);
	
	
	
	
	long IDwfsbest, IDwfsall;
	long IDpsfbest, IDpsfall;
	
	IDwfsbest = create_2Dimage_ID("imwfsbest", xsizewfs, ysizewfs);
	IDwfsall  = create_2Dimage_ID("imwfsall", xsizewfs, ysizewfs);
	
	IDpsfbest = create_2Dimage_ID("impsfbest", xsizepsf, ysizepsf);
	IDpsfall  = create_2Dimage_ID("impsfall", xsizepsf, ysizepsf);
	
	
	
	long kklim;
	kklim = (long) (frac*NBframe);
	
	printf("kklim = %ld     %ld %ld\n", kklim, xysizewfs, xysizepsf);
	
	FILE *fp = fopen("fptest.txt", "w");
	for(kk=0;kk<NBframe;kk++)
	{
		long ii;
		
		fprintf(fp, "%6ld  %6ld  %g\n", kk, indexarray[kk], evalarray[kk]);
		
		if(kk<kklim)
		{
			for(ii=0;ii<xysizewfs;ii++)
				data.image[IDwfsbest].array.F[ii] += data.image[IDwfs].array.F[indexarray[kk]*xysizewfs+ii];
			
			for(ii=0;ii<xysizepsf;ii++)
				data.image[IDpsfbest].array.F[ii] += data.image[IDpsf].array.F[indexarray[kk]*xysizepsf+ii];
			
		}
	
		
		for(ii=0;ii<xysizewfs;ii++)
				data.image[IDwfsall].array.F[ii] += data.image[IDwfs].array.F[kk*xysizewfs+ii];
	
		for(ii=0;ii<xysizepsf;ii++)
				data.image[IDpsfall].array.F[ii] += data.image[IDpsf].array.F[kk*xysizepsf+ii];
	}
	fclose(fp);
	
	long ii;
	
	for(ii=0;ii<xysizewfs;ii++)
		data.image[IDwfsbest].array.F[ii] /= kklim;

	for(ii=0;ii<xysizepsf;ii++)
		data.image[IDpsfbest].array.F[ii] /= kklim;
	
	
	for(ii=0;ii<xysizewfs;ii++)
		data.image[IDwfsall].array.F[ii] /= NBframe;

	for(ii=0;ii<xysizepsf;ii++)
		data.image[IDpsfall].array.F[ii] /= NBframe;	
	
	
	free(evalarray);
	free(indexarray);
	
	return 0;
}

