////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testDraculaView.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for basic functions, "
			   "just before Dracula's first move\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "..."
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 0);
		free(traps);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for basic functions extensive, encountering Dracula and"
				"Dracula losing life at Sea");

		char *trail =
			"GKL.... SKL.... HGA.... MGA.... DCD.V.. " 
			"GCDVD.. SCDD... HCDD... MCDD... DGAT... "
			"GCD.... SCD.... HCD.... MCD.... DCNT... "
			"GCD.... SCD.... HCD.... MCD.... DBS.... ";

		
		Message messages[20] = {};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetScore(dv) == GAME_START_SCORE - 4 * SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 8);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == CASTLE_DRACULA);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == CASTLE_DRACULA);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == CASTLE_DRACULA);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == CASTLE_DRACULA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == BLACK_SEA);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == CONSTANTA);
		assert(traps[1] == GALATZ);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		
		Message messages[9] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahah",
			"Aha!", "", "", ""
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		assert(DvGetScore(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 5);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 30);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GENEVA);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula leaving minions 1\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST.... DLOT... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == EDINBURGH);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);
		free(traps);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula leaving minions 2\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCN.V.. "
			"GST.... SST.... HST.... MST.... DBS.... "
			"GST.... SST.... HST.... MST.... DD2T... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == CONSTANTA);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);

		assert(numTraps == 2);
		assert(traps[0] == CONSTANTA);
		assert(traps[1] == CONSTANTA);
		free(traps);
	}

	{
		printf("Dracula leaving traps and vampire\n");

		char *trail =
			"GCD.... SCD.... HCD.... MCD.... DCN.V.. " 
			"GCD.... SCD.... HCD.... MCD.... DGAT... "
			"GCD.... SCD.... HCD.... MCD.... DBCT... "
			"GCD.... SCD.... HCD.... MCD.... DHIT... ";

		
		Message messages[20] = {};
		
		DraculaView dv = DvNew(trail, messages);
		assert(DvGetVampireLocation(dv) == CONSTANTA);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == BUCHAREST);
		assert(traps[1] == BUCHAREST);
		assert(traps[2] == GALATZ);

		printf("Test passed!\n");
		DvFree(dv);
	}	

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula's valid moves 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing appropriate double back and hide result\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCDV... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... ";
		
		Message messages[15] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 2);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Tests for Dracula's no Move\n");
		
		char *trail = "GGE.... SGE.... HGE.... MGE....";
		
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 0);
		assert(moves == NULL);
		free(moves);
		printf("Passed no move!\n");
		DvFree(dv);	
	}	


	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvWhereCanIGo 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{



		printf("Test for DvWhereCanIGo 2\n");
		
		char *trail =
			"GED.... SGE.... HZU.... MCA.... DCF.V.. "
			"GMN.... SCFVD.. HGE.... MLS.... DBOT... "
			"GLO.... SMR.... HCF.... MMA.... DTOT... "
			"GPL.... SMS.... HMR.... MGR.... DBAT... "
			"GLO.... SBATD.. HMS.... MMA.... DSRT... "
			"GPL.... SSJ.... HBA.... MGR.... DALT... "
			"GPL.... SSJ.... HBA.... MGR.... DMAT... "
			"GLO.... SBE.... HMS.... MMATD..";
		
		Message messages[39] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		sortPlaces(locs, numLocs);
		for (int i = 0; i < numLocs; i++)
		assert(numLocs == 7);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ALICANTE);
		assert(locs[1] == CADIZ);
		assert(locs[2] == GRANADA);
		assert(locs[3] == LISBON);
		assert(locs[4] == MADRID);
		assert(locs[5] == SANTANDER);
		assert(locs[6] == SARAGOSSA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGo 3\n");
		// Trail limit test, can return to location if it left trail even if double backed.
		// Test that sea travel is an option
		//
		char *trail = "GSW.... SLS.... HMR.... MHA.... DCN.V.. "
					  "GLO.... SAL.... HCO.... MBR.... DD1T... "
					  "GED.... SBO.... HLI.... MPR.... DGAT... "
					  "GLV.... SNA.... HNU.... MBD.... DBCT... "
					  "GIR.... SPA.... HPR.... MKL.... DSOT... "
					  "GAO.... SST.... HSZ.... MCD.... DVRT... "
					  "GNS.... SFR.... HBD.... MKL.... ";



		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);

		assert(numLocs == 3);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BLACK_SEA);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == VARNA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test where can they go\n");

		char *trail = "GGW.... SPL.... HCA.... MCG.... DST.V.. "
					  "GDU.... SLO.... HLS.... MTS.... ";
	
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_DRACULA, &numLocs);

		assert(numLocs == 9);
		sortPlaces(locs, numLocs);

		assert(locs[0] == BRUSSELS);
		assert(locs[1] == COLOGNE);
		assert(locs[2] == FRANKFURT);
		assert(locs[3] == GENEVA);
		assert(locs[4] == MUNICH);
		assert(locs[5] == NUREMBURG);
		assert(locs[6] == PARIS);
		assert(locs[7] == STRASBOURG);
		assert(locs[8] == ZURICH);
		printf("Dracula passed\n");
		free(locs);

		locs = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocs);
		assert(numLocs == 3);
		sortPlaces(locs, numLocs);
		assert(locs[0] == DUBLIN);
		assert(locs[1] == GALWAY);
		assert(locs[2] == IRISH_SEA);
		printf("Godalming passed\n");
		free(locs);

		locs = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numLocs);
		sortPlaces(locs, numLocs);
		assert(numLocs == 7);
		assert(locs[0] == EDINBURGH);
		assert(locs[1] == ENGLISH_CHANNEL);
		assert(locs[2] == LIVERPOOL);
		assert(locs[3] == LONDON);
		assert(locs[4] == MANCHESTER);
		assert(locs[5] == PLYMOUTH);
		assert(locs[6] == SWANSEA);

		printf("Seward passed\n");
		free(locs);

	}	

	{
		printf("Very Extensive test of all functions\n");

		char *trail =
			"GSW.... SLS.... HMR.... MHA.... DSJ.V.. "
			"GLO.... SAL.... HCO.... MBR.... DBET... "
			"GED.... SBO.... HLI.... MPR.... DKLT... "
			"GLV.... SNA.... HNU.... MBD.... DCDT... "
			"GIR.... SPA.... HPR.... MKLT... DHIT... "
			"GAO.... SST.... HSZ.... MCDTTD. DGAT... "
			"GMS.... SFL.... HKL.... MSZ.... DCNT.V. "
			"GTS.... SRO.... HBC.... MCNTD.. DBS..M. "
			"GIO.... SBI.... HCN.... MCN.... DIO.... "
			"GIO.... SAS.... HBS.... MCN.... DTS.... "
			"GTS.... SAS.... HIO.... MBS.... DMS.... "
			"GMS.... SIO.... HTS.... MIO.... DAO..M. "
			"GAO.... STS.... HMS.... MTS.... DNS.... "
			"GBB.... SMS.... HAO.... MMS.... DED.V.. "
			"GNA.... SAO.... HEC.... MAO.... DMNT... "
			"GBO.... SIR.... HLE.... MEC.... DD2T... "
			"GSR.... SDU.... HBU.... MPL.... DHIT... "
			"GSN.... SIR.... HAM.... MLO.... DTPT... "
			"GAL.... SAO.... HCO.... MEC.... DCDT... "
			"GMS.... SMS.... HFR.... MLE.... DKLT.V. "
			"GTS.... STS.... HBR.... MCO.... DGAT.M. "
			"GIO.... SIO.... HBD.... MLI.... DD3T.M. "
			"GBS.... SBS.... HKLT... MBR.... DHI..M. "
			"GCN.... SCN.... HCDTTTD MVI.... ";
			

		
		Message messages[114] = { };
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 23);
		assert(DvGetScore(dv) == 305);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 58);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 9);
		assert(DvGetHealth(dv, PLAYER_DR_SEWARD) == 9);
		assert(DvGetHealth(dv, PLAYER_VAN_HELSING) == 0);
		assert(DvGetHealth(dv, PLAYER_MINA_HARKER) == 9);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == CONSTANTA);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == CONSTANTA);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ST_JOSEPH_AND_ST_MARY);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == VIENNA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == CASTLE_DRACULA);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		
		
		assert(numTraps == 1);
		assert(traps[0] == GALATZ);
		free(traps);
		DvFree(dv);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGoByType(dv, PLAYER_DRACULA, true, false, true, &numLocs);
		assert(numLocs == 0);
		free(locs);

	}

	printf("All DraculaView Tests Passed!\n");
	return EXIT_SUCCESS;
}
