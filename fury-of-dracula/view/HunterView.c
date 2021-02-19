////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	HunterView new = malloc(sizeof(*new));
	assert(new != NULL);
	new -> gv = GvNew(pastPlays, messages);

	return new;
}

void HvFree(HunterView hv)
{
	GvFree(hv -> gv);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv -> gv);
}

Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv -> gv);
}

int HvGetScore(HunterView hv)
{
	return GvGetScore(hv -> gv);
}

int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv -> gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv -> gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv -> gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{	
	// finds last location
	int size = 0;
	bool canFree;
	PlaceId *draculaHistory = GvGetLocationHistory(hv -> gv, PLAYER_DRACULA, &size, &canFree);
	int pos = -1;
	// iterates through to find the last location
	for (int i = size - 1; i >= 0; i--) {
		if (draculaHistory[i] != CITY_UNKNOWN && draculaHistory[i] != SEA_UNKNOWN) {
			pos = i;
			break;
		}

	}
	// set place if found
	PlaceId place = NOWHERE;
	if (pos >= 0) {
		place = draculaHistory[pos];
		*round = pos;
	}
    //Process double back
    if (DOUBLE_BACK_1 <= place && place <= DOUBLE_BACK_5) {
		place = draculaHistory[pos - (place - DOUBLE_BACK_1 + 1)];
	}
	//Process hide
	else if (place == HIDE) {
		place = draculaHistory[pos - 1];
	}
	// freeing copy of array if array was copy
	if (canFree) {
		free(draculaHistory);
	}
	return place;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{	
	// TODO see note on storing locations in hunter struct
	// TODO consider finding all shortest paths to each place from src and saving result
	PlaceId src = GvGetPlayerLocation(hv -> gv, hunter);
	Round round = HvGetRound(hv);

	return shortestPath(hv -> gv, src, dest, pathLength, round, hunter);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{	
	return HvWhereCanIGoByType(hv, true, true, true, numReturnedLocs);
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player hunter = GvGetPlayer(hv -> gv);
	return HvWhereCanTheyGoByType(hv, hunter, road, rail, boat, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	return HvWhereCanTheyGoByType(hv, player, true, true, true, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{

	bool notHadTurn = player >= HvGetPlayer(hv);
	int round = HvGetRound(hv);

	// player isn't on map yet
	if (notHadTurn && round == 0) {
		*numReturnedLocs = 0;
		return NULL;
	}

	// next Round is the next round the player can move in
	int nextRound = round + !notHadTurn;
	PlaceId playerLocation = HvGetPlayerLocation(hv, player);
	return GvGetReachableByType(hv -> gv, player, nextRound, playerLocation,
	                                     road, rail, boat, numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
