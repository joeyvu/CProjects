////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here

#define TRAIL_SIZE 6

struct draculaView {
    GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	DraculaView new = malloc(sizeof(*new));
	assert(new != NULL);
	new->gv = GvNew(pastPlays, messages);

	return new;
}

void DvFree(DraculaView dv)
{
	GvFree(dv->gv);
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
    return GvGetRound(dv -> gv);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv -> gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv -> gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv -> gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv -> gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	*numTraps = 0;
	PlaceId *ids = GvGetTrapLocations(dv->gv, numTraps);

	return ids;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	Round round = GvGetRound(dv->gv);

	*numReturnedMoves = 0;
	if (round == 0) return NULL;

	PlaceId location = DvGetPlayerLocation(dv, PLAYER_DRACULA);


	PlaceId *reachable = GvGetReachableByType(dv->gv, PLAYER_DRACULA, round, 
						 location, true, false, true, numReturnedMoves);

	

	int trail_size = 0;
	bool can_free = false;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, TRAIL_SIZE, &trail_size, &can_free);


	bool doubled_back = false;
	bool hid = false;

	// Check if Dracula has doubled back or hid in the last five rounds
	for (int i = 0; i < trail_size; i++)
	{
		if (trail[i] == DOUBLE_BACK_1
			|| trail[i] == DOUBLE_BACK_2
			|| trail[i] == DOUBLE_BACK_3
			|| trail[i] == DOUBLE_BACK_4
			|| trail[i] == DOUBLE_BACK_5
			) {
			doubled_back = true;
		} else if (trail[i] == HIDE) {
			hid = true;
		}
	}
	if (can_free) free(trail);
	// Allocate enough memory for worst case scenario
	PlaceId *result = malloc((*numReturnedMoves * sizeof(PlaceId)) + (6 * sizeof(PlaceId)));
	// Result array index
	
	int j = 0;

	if (!hid) {
		// Add hide to start
		result[0] = HIDE;
		j++;
	}
	PlaceId add;
	for (int i = 0; i < *numReturnedMoves; i++)
	{
		add = NOWHERE;
		// Check if its in the trail and return the right double_back constant

		if (in_trail(dv, reachable[i]) == NOWHERE) {
			// Add location
			add = reachable[i];
		} else if (!doubled_back) {
			// Add corresponding double backed 
			add = in_trail(dv, reachable[i]);	
		}

		if (add != NOWHERE) {
			// Add to results
			result[j] = add;
			j++;
		}
	}
	*numReturnedMoves = j;
	return realloc(result, j * sizeof(PlaceId));
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	return DvWhereCanIGoByType(dv, true, true, numReturnedLocs);
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	
	int numReturned = 0;
	PlaceId *valid = DvGetValidMoves(dv, &numReturned);

	int trail_size = 0;
	bool can_free = false;
	PlaceId *trail = GvGetLastLocations(dv->gv, PLAYER_DRACULA, TRAIL_SIZE, &trail_size, &can_free);
	for (int i = 0; i < numReturned; i++)
	{
		if ((valid[i] == DOUBLE_BACK_1 || valid[i] == HIDE)) {
			valid[i] = trail[trail_size - 1];
		} else if (valid[i] == DOUBLE_BACK_2) {
			valid[i] = trail[trail_size - 2];
		} else if (valid[i] == DOUBLE_BACK_3) {
			valid[i] = trail[trail_size - 3];
		} else if (valid[i] == DOUBLE_BACK_4) {
			valid[i] = trail[trail_size - 4];
		} else if (valid[i] == DOUBLE_BACK_5) {
			valid[i] = trail[trail_size - 5];
		} 
		
	}
	if (can_free) free(trail);

	
	// Scan array for duplicates that occur in edge cases
	int count;
	for (int i = 0; i < numReturned; i++)
	{
		count = 0;
		for (int j = 0; j < numReturned; j++)
		{
			if (valid[i] == valid[j]) {
				count++;
				if (count > 1) {
					valid[j] = valid[numReturned - 1];
					numReturned--;
					
				}
			}
		}
	}
	*numReturnedLocs = numReturned;
	return valid;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	
	*numReturnedLocs = 0;
	return DvWhereCanTheyGoByType(dv, player, true, true, 
							true, numReturnedLocs);
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{

	*numReturnedLocs = 0;

	Round round = GvGetRound(dv->gv);
	if (player == PLAYER_DRACULA) {
		if (round == 0) {
			return NULL;
		}
		return DvWhereCanIGoByType(dv, road, boat, numReturnedLocs);
	}
	// Check if player has taken a turn
	if (round == 0) { 
		int returned_moves;
		bool can_free;
		PlaceId *temp = GvGetMoveHistory(dv->gv, player, &returned_moves, &can_free);
		if (can_free) free(temp);
		if (returned_moves == 0) {
			return NULL;
		}
	}

	return GvGetReachableByType(dv->gv, player, round, 
						GvGetPlayerLocation(dv->gv, player), 
						road, rail, boat, numReturnedLocs);

	
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// Checks if a target move or location is in the trail 
// returns its corresponding DOUBLE_BACK value or NULL if not found
PlaceId in_trail(DraculaView dv, PlaceId target) {
	int trail_size = 0;
	bool can_free = false;;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, TRAIL_SIZE, &trail_size, &can_free);
	int pos = -1;
	for (int i = trail_size; i >= 0; i--)
	{
		if (trail[i] == target) {
			pos = i;
			break;
		}
	}
	if (can_free) free(trail);
	PlaceId ret = NOWHERE;
	if (pos == -1) {
		return ret;
	}
	pos = trail_size - pos;
	if (pos == 1) {
		ret = DOUBLE_BACK_1;
	} else if (pos == 2) {
		ret = DOUBLE_BACK_2;
	} else if (pos == 3) {
		ret = DOUBLE_BACK_3;
	} else if (pos == 4) {
		ret = DOUBLE_BACK_4;
	} else if (pos ==  5) {
		ret = DOUBLE_BACK_5;
	}
	return ret;

}

void debugPrintDv(DraculaView dv) {
    debugPrintGv(dv->gv);
    }


