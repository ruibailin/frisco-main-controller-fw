
/*------------------------------------
 * pat.c
 * Create:  2023-02-17
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 * User Process register here
 *
 *
 *
 *------------------------------------
 */


#include "../src/eos/kernel/process/basic/pat.h"
/*================================================================*/
/*------------------------------------*/
static void null_p(void *in)
{

}
/*------------------------------------*/
extern void	sys_proc_00(void *in);
extern void	sys_timer(void *in);
extern void	sys_idler(void *in);
extern void	sys_comer(void *in);

//PMG
/*------------------------------------*/
extern void pmg_main_task(void *in);
extern void pmg_iwdg_task(void *in);
extern void pmg_appn_task(void *in);
extern void pmg_scan_task(void *in);
/*------------------------------------*/
extern void pmg_app00_task(void *in);
extern void pmg_app01_task(void *in);
extern void pmg_app02_task(void *in);
extern void pmg_app03_task(void *in);
extern void pmg_app04_task(void *in);
extern void pmg_app05_task(void *in);
extern void pmg_app06_task(void *in);
extern void pmg_app07_task(void *in);
extern void pmg_app08_task(void *in);
extern void pmg_app09_task(void *in);
extern void pmg_app10_task(void *in);
extern void pmg_app11_task(void *in);
extern void pmg_app12_task(void *in);
extern void pmg_app13_task(void *in);
extern void pmg_app14_task(void *in);
extern void pmg_app15_task(void *in);
extern void pmg_app16_task(void *in);
extern void pmg_app17_task(void *in);
extern void pmg_app18_task(void *in);
extern void pmg_app19_task(void *in);
extern void pmg_app20_task(void *in);
extern void pmg_app21_task(void *in);
extern void pmg_app22_task(void *in);
extern void pmg_app23_task(void *in);
extern void pmg_app24_task(void *in);
extern void pmg_app25_task(void *in);
extern void pmg_app26_task(void *in);
extern void pmg_app27_task(void *in);
extern void pmg_app28_task(void *in);
extern void pmg_app29_task(void *in);
extern void pmg_app30_task(void *in);
extern void pmg_app31_task(void *in);
extern void pmg_app32_task(void *in);
extern void pmg_app33_task(void *in);
extern void pmg_app34_task(void *in);
extern void pmg_app35_task(void *in);
extern void pmg_app36_task(void *in);
extern void pmg_app37_task(void *in);
extern void pmg_app38_task(void *in);
extern void pmg_app39_task(void *in);
extern void pmg_app40_task(void *in);
extern void pmg_app41_task(void *in);
extern void pmg_app42_task(void *in);
extern void pmg_app43_task(void *in);
extern void pmg_app44_task(void *in);
extern void pmg_app45_task(void *in);
extern void pmg_app46_task(void *in);
extern void pmg_app47_task(void *in);
extern void pmg_app48_task(void *in);
extern void pmg_app49_task(void *in);
extern void pmg_app50_task(void *in);
extern void pmg_app51_task(void *in);
extern void pmg_app52_task(void *in);
extern void pmg_app53_task(void *in);
extern void pmg_app54_task(void *in);
extern void pmg_app55_task(void *in);
extern void pmg_app56_task(void *in);
extern void pmg_app57_task(void *in);
extern void pmg_app58_task(void *in);
extern void pmg_app59_task(void *in);
/*------------------------------------*/
/*The task with largest pno will be scheduled at first*/
/*So pmg_main_task has the largest*/
PAT PATPool[MAX_PAT_NUM]={
/*       name      		entry  		type	attr	*/
/*000*/  {"System 00", 	&sys_proc_00,	REAL,		0},	//must be sys_proc_00
/*001*/  {"app01", 	&pmg_app01_task,	MSG,		0},
/*002*/  {"app02", 	&pmg_app02_task,	MSG,		0},
/*003*/  {"app03", 	&pmg_app03_task,	MSG,		0},
/*004*/  {"app04", 	&pmg_app04_task,	MSG,		0},
/*005*/  {"app05", 	&pmg_app05_task,	MSG,		0},
/*006*/  {"app06", 	&pmg_app06_task,	MSG,		0},
/*007*/  {"app07", 	&pmg_app07_task,	MSG,		0},
/*008*/  {"app08", 	&pmg_app08_task,	MSG,		0},
/*009*/  {"app09", 	&pmg_app09_task,	MSG,		0},

/*010*/  {"app10", 	&pmg_app10_task,	MSG,		0},
/*011*/  {"app11", 	&pmg_app11_task,	MSG,		0},
/*012*/  {"app12", 	&pmg_app12_task,	MSG,		0},
/*013*/  {"app13", 	&pmg_app13_task,	MSG,		0},
/*014*/  {"app14", 	&pmg_app14_task,	MSG,		0},
/*015*/  {"app15", 	&pmg_app15_task,	MSG,		0},
/*016*/  {"app16", 	&pmg_app16_task,	MSG,		0},
/*017*/  {"app17", 	&pmg_app17_task,	MSG,		0},
/*018*/  {"app18", 	&pmg_app18_task,	MSG,		0},
/*019*/  {"app19", 	&pmg_app19_task,	MSG,		0},

/*020*/  {"app20", 	&pmg_app20_task,	MSG,		0},
/*021*/  {"app21", 	&pmg_app21_task,	MSG,		0},
/*022*/  {"app22", 	&pmg_app22_task,	MSG,		0},
/*023*/  {"app23", 	&pmg_app23_task,	MSG,		0},
/*024*/  {"app24", 	&pmg_app24_task,	MSG,		0},
/*025*/  {"app25", 	&pmg_app25_task,	MSG,		0},
/*026*/  {"app26", 	&pmg_app26_task,	MSG,		0},
/*027*/  {"app27", 	&pmg_app27_task,	MSG,		0},
/*028*/  {"app28", 	&pmg_app28_task,	MSG,		0},
/*029*/  {"app29", 	&pmg_app29_task,	MSG,		0},

/*030*/  {"app30", 	&pmg_app30_task,	MSG,		0},
/*031*/  {"app31", 	&pmg_app31_task,	MSG,		0},
/*032*/  {"app32", 	&pmg_app32_task,	MSG,		0},
/*033*/  {"app33", 	&pmg_app33_task,	MSG,		0},
/*034*/  {"app34", 	&pmg_app34_task,	MSG,		0},
/*035*/  {"app35", 	&pmg_app35_task,	MSG,		0},
/*036*/  {"app36", 	&pmg_app36_task,	MSG,		0},
/*037*/  {"app37", 	&pmg_app37_task,	MSG,		0},
/*038*/  {"app38", 	&pmg_app38_task,	MSG,		0},
/*039*/  {"app39", 	&pmg_app39_task,	MSG,		0},

/*040*/  {"app40", 	&pmg_app40_task,	MSG,		0},
/*041*/  {"app41", 	&pmg_app41_task,	MSG,		0},
/*042*/  {"app42", 	&pmg_app42_task,	MSG,		0},
/*043*/  {"app43", 	&pmg_app43_task,	MSG,		0},
/*044*/  {"app44", 	&pmg_app44_task,	MSG,		0},
/*045*/  {"app45", 	&pmg_app45_task,	MSG,		0},
/*046*/  {"app46", 	&pmg_app46_task,	MSG,		0},
/*047*/  {"app47", 	&pmg_app47_task,	MSG,		0},
/*048*/  {"app48", 	&pmg_app48_task,	MSG,		0},
/*049*/  {"app49", 	&pmg_app49_task,	MSG,		0},

/*050*/  {"Null Pro", 	&null_p,	0,		0},
/*051*/  {"Null Pro", 	&null_p,	0,		0},
/*052*/  {"Null Pro", 	&null_p,	0,		0},
/*053*/  {"Null Pro", 	&null_p,	0,		0},
/*054*/  {"Null Pro", 	&null_p,	0,		0},
/*055*/  {"Null Pro", 	&null_p,	0,		0},

/*056*/  {"App", 	&pmg_appn_task,	MSG,		0},
/*057*/  {"Low", 	&pmg_scan_task,	REAL,		0},
/*058*/  {"WDT", 	&pmg_iwdg_task,	MSG,		0},
/*059*/  {"Main", 	&pmg_main_task,	MSG,		0},
/*060*/  {"Com outside", 	&sys_comer,	MSG,		0},
/*061*/  {"Null Pro", 	&null_p,	0,		0},
/*062*/  {"Timer Proc", 	&sys_timer,	TIME,		1},
/*063*/  {"Idler Proc", 	&sys_idler,	IDLE,		0},
};

/*================================================================*/

/* end of pat.c */
