////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <stdbool.h>


struct placeStats {
	PlaceId p; //place
	double roundsToCD; //numbers of rounds to get to castle dracula (no rail)
	double degree; // not including rail
	double *roundsFrom; // array of number of rounds to get to p from hunter, hunterId is index
}; 
typedef struct placeStats *PlaceStatsNode;

///////////////////////////////////////////////////////////////////////////
// LOCAL PROTOTYPES

PlaceId decideStartPlace(HunterView hv);
int placeStatsCmp(void *a, void *b);
void *getMax(void **array, int size, int elemSize, int (*cmp)(void *, void *)); 

PlaceStatsNode *getDraculaLocsStats(HunterView hv, PlaceId *possibleLocs, int nPossibleLocs);
double calcPlacePriority(PlaceStatsNode n);
PlaceId huntDracula(HunterView hv, PlaceId dracLoc, Round dracLocRound, Player hunter);

bool atAdjLoc(HunterView hv, PlaceId lastSeenDracLoc);
PlaceId assignedStartPlace(HunterView hv); 
PlaceId firstSixRounds(HunterView hv);
bool isThisValidMove(HunterView hv, PlaceId move);
// Player whoIsClosestToCD(HunterView hv)


///////////////////////////////////////////////////////////////////////////
// LOGIC FUNCTIONS

void decideHunterMove(HunterView hv)
{	
	int round = HvGetRound(hv);
	int lastSeenDracRound = 0;
	PlaceId lastSeenDracLoc = HvGetLastKnownDraculaLocation(hv, &lastSeenDracRound);
	int lagRounds = round - lastSeenDracRound;

	Player hunter = HvGetPlayer(hv);
	PlaceId curLoc = HvGetPlayerLocation(hv, hunter);
	PlaceId move;
      
	// We dont know where Dracula is
	if(lastSeenDracLoc == NOWHERE) { 
	    // Round = 0
	    if(round == 0) {
	        move = assignedStartPlace(hv); // Potentially make this return
	    // Round is before round, stick with default strat
	    } else if(round <= 6 && round != 0) {
	        move = firstSixRounds(hv);
	    // Time to research, we couldn't find dracula's trail
	    } else if(round >= 7) {
	        move = curLoc;
	    }
	} else if(lastSeenDracLoc != NOWHERE) { // We have an idea where Dracula could be
	    // Dracula's last seen location still in his trail and player is not at it
        if(lagRounds < 6 && atAdjLoc(hv, lastSeenDracLoc) == false) {
            // Move towards location
            int pathSize = 0;
            PlaceId *moveTowardTrail = HvGetShortestPathTo(hv, hunter, lastSeenDracLoc, &pathSize);
            if (pathSize != 0) {
				move = moveTowardTrail[0]; // This assumes HvGetShortestPath gets the truest shortest path
			} 
			free(moveTowardTrail);
		} else if(lagRounds < 6 && atAdjLoc(hv, lastSeenDracLoc) == true) { // We are on Dracula's trail
            // Guess where dracula would go and move there
            // This is where viability should come into play
            move = huntDracula(hv, lastSeenDracLoc, lastSeenDracRound, hunter); 
        } else if(lagRounds > 5) { // Dracula's trail has gone cold, so we research
            move = curLoc;
        }
	} 
	
	//  TIME TO REST Takes priority over other moves
    int hunterHealth = HvGetHealth(hv, hunter);
    // Only heal when less than 8(riskier), hunter can be killed by two traps and drac encounter 
    if(hunterHealth < 8) move = curLoc;
    // Safer as 2 traps and drac encounter still leaves hunter alive
    // if(hunterHealth < 9) move = curLoc;
    // Even riskier, hunter dies by 1 trap and Drac encounter
    // if(hunterHealth < 7) move = curLoc;
	
	char *moveAbbrev;
	char *msg;
		
	if(round == 0) {
	    moveAbbrev = strdup(placeIdToAbbrev(move));
	    msg = "First move must be valid!";
	} else if(isThisValidMove(hv, move) == true) {
	    moveAbbrev = strdup(placeIdToAbbrev(move));
	    msg = "Valid Move!";
	} else if(isThisValidMove(hv, move) == false) {
	    int nValidMoves = 0;
	    PlaceId *validMoves = HvWhereCanIGo(hv, &nValidMoves);
	    moveAbbrev = strdup(placeIdToAbbrev(validMoves[0]));
	    msg = "Current move is not valid. Selecting random move!";
	}
    
	registerBestPlay(moveAbbrev, msg);
	free(moveAbbrev);


}

PlaceId assignedStartPlace(HunterView hv) {
    Player hunter = HvGetPlayer(hv);
    PlaceId move;    

    if(hunter == PLAYER_MINA_HARKER) {
        move = CASTLE_DRACULA;
    } else if(hunter == PLAYER_LORD_GODALMING) {
        move = VALONA;
    } else if(hunter == PLAYER_DR_SEWARD) {
        move = LISBON;
    } else if(hunter == PLAYER_VAN_HELSING) {
        move = SWANSEA;
    }
    return move;
}
// Strat for first five rounds, hunters will follow a specifc path until they converge to LORD GOLDAMING and can then research
// Unless we run into dracs trail first 
// this could bait dracula players to start at the middle where we can then trap him and converge on him
PlaceId firstSixRounds(HunterView hv) {
    Player hunter = HvGetPlayer(hv);
    PlaceId move;
    int round = HvGetRound(hv);
    
    if(round == 1) {
        if(hunter == PLAYER_MINA_HARKER) {
            move = KLAUSENBURG;
        } else if(hunter == PLAYER_LORD_GODALMING) {
            move = SARAJEVO;
        } else if(hunter == PLAYER_DR_SEWARD) {
            move = MADRID;
        } else if(hunter == PLAYER_VAN_HELSING) {
            move = LIVERPOOL;
        }
    } else if(round == 2) {
        if(hunter == PLAYER_MINA_HARKER) {
            move = BELGRADE;
        } else if(hunter == PLAYER_LORD_GODALMING) {
            move = ZAGREB;
        } else if(hunter == PLAYER_DR_SEWARD) {
            move = SARAGOSSA;
        } else if(hunter == PLAYER_VAN_HELSING) {
            move = MANCHESTER;
        }
    } else if(round == 3) {
        if(hunter == PLAYER_MINA_HARKER) {
            move = SZEGED;
        } else if(hunter == PLAYER_LORD_GODALMING) {
            move = MUNICH;
        } else if(hunter == PLAYER_DR_SEWARD) {
            move = TOULOUSE;
        } else if(hunter == PLAYER_VAN_HELSING) {
            move = LONDON;
        }
    } else if(round == 4) {
        if(hunter == PLAYER_MINA_HARKER) {
            move = BUDAPEST;
        } else if(hunter == PLAYER_LORD_GODALMING) {
            move = STRASBOURG;
        } else if(hunter == PLAYER_DR_SEWARD) {
            move = CLERMONT_FERRAND;
        } else if(hunter == PLAYER_VAN_HELSING) {
            move = ENGLISH_CHANNEL;
        } 
    } else if(round == 5) {
        if(hunter == PLAYER_MINA_HARKER) {
            move = VIENNA;
        } else if(hunter == PLAYER_LORD_GODALMING) {
            move = FRANKFURT;
        } else if(hunter == PLAYER_DR_SEWARD) {
            move = GENEVA;
        } else if(hunter == PLAYER_VAN_HELSING) {
            move = LE_HAVRE;
        }
    } else if(round == 6) {
        if(hunter == PLAYER_MINA_HARKER) {
            move = MUNICH;
        } else if(hunter == PLAYER_LORD_GODALMING) {
            move = LEIPZIG;
        } else if(hunter == PLAYER_DR_SEWARD) {
            move = ZURICH;
        } else if(hunter == PLAYER_VAN_HELSING) {
            move = PARIS;
        }
    }
    return move;
}


// Function to check if hunter is at location adj to lastSeenDracLoc
bool atAdjLoc(HunterView hv, PlaceId lastSeenDracLoc) {
    Round curRound = HvGetRound(hv);
    Player curHunter = HvGetPlayer(hv);
    PlaceId curLoc = HvGetPlayerLocation(hv, curHunter);
	int numReturnedLocs = 0;
    PlaceId *adjLastSeenDracLoc = HvGetReachableByType(hv, PLAYER_DRACULA, curRound, lastSeenDracLoc, true, false, true, &numReturnedLocs);
    
    for(int i = 0; i < numReturnedLocs; i++) {
        if(curLoc == adjLastSeenDracLoc[i]) {
			free(adjLastSeenDracLoc);
            return true;
        }
    }
	free(adjLastSeenDracLoc);
    return false;   
}
   
PlaceId huntDracula(HunterView hv, PlaceId dracLoc, Round dracLocRound, Player hunter) {
	int nPossibleLocs = 0;

	Round curRound = HvGetRound(hv);
	PlaceId *possibleLocs = HvPossibleLocs(hv, dracLoc, &nPossibleLocs, dracLocRound, curRound, PLAYER_DRACULA);

	PlaceStatsNode *locsStats = getDraculaLocsStats(hv, possibleLocs, nPossibleLocs);
	PlaceStatsNode bestLocStats = (PlaceStatsNode) getMax((void **) locsStats, nPossibleLocs, sizeof(PlaceStatsNode), placeStatsCmp);
	PlaceId bestLoc = bestLocStats->p;


	
	int pathSize = 0;
	PlaceId *path= HvGetShortestPathTo(hv, HvGetPlayer(hv), bestLoc, &pathSize);
	if (pathSize == 0) {
		int splitUp = 0;
			fflush(stdout);
			for (int otherPlayer = 0; otherPlayer < hunter; otherPlayer++) {

				if (HvGetPlayerLocation(hv, otherPlayer) == HvGetPlayerLocation(hv, hunter)) {
					splitUp++;
				}
				
			}
			int numReturnedLocs = 0;
			PlaceId *possibleEscapes = HvGetReachableByType(hv, PLAYER_DRACULA, dracLocRound, dracLoc, true, false, true, &numReturnedLocs);
			if (splitUp >= numReturnedLocs) {
				bestLoc = possibleEscapes[numReturnedLocs - 1];
			} else {
				bestLoc = possibleEscapes[splitUp];
			}
			fflush(stdout);
			path = HvGetShortestPathTo(hv, HvGetPlayer(hv), bestLoc, &pathSize);

			fflush(stdout);
	}

	return path[0];
}

double calcPlacePriority(PlaceStatsNode n) {

	
	double priorty = 0;
	priorty += 1 - n->roundsToCD;
	priorty += n->degree * 0.2;
	
	return priorty;
}


///////////////////////////////////////////////////////////////////////////
// UTLITY FUNCTIONS

void setPlaceStatsMinMax(PlaceStatsNode minVals, PlaceStatsNode maxVals, PlaceStatsNode n) {
	minVals->roundsToCD = doubleMin(minVals->roundsToCD, n->roundsToCD);
	minVals->degree = doubleMin(minVals->degree, n->degree);
	//minVals->roundsFrom = doubleMin(minVals->roundsFrom, n->roundsFrom);

	maxVals->roundsToCD = doubleMax(maxVals->roundsToCD, n->roundsToCD);
	maxVals->degree = doubleMax(maxVals->degree, n->degree);
	//maxVals->roundsFrom = doubleMax(maxVals->roundsFrom, n->roundsFrom);
}


PlaceStatsNode createPlaceStatsNode() {
	PlaceStatsNode node = calloc(1, sizeof(struct placeStats));
	assert(node != NULL);
	node -> roundsFrom = calloc((NUM_PLAYERS - 1), sizeof(double)); 
	assert(node -> roundsFrom != NULL);
	return node;
}

// Normalise x
void normalise(double *x, double xMin, double xMax) {
	*x = (*x - xMin) / (xMax - xMin);
}

void normalisePlaceStats(PlaceStatsNode minVals, PlaceStatsNode maxVals, PlaceStatsNode *stats, int size) {
	for (int i = 0; i < size; i++) {
		PlaceStatsNode n = stats[i];
		normalise(&(n->roundsToCD), minVals->roundsToCD, maxVals->roundsToCD);
		normalise(&(n->degree), minVals->degree, maxVals->degree);
		for (int j = 0; j < NUM_PLAYERS - 1; j++) {
			normalise(&(n->roundsFrom[j]), minVals->roundsFrom[j], maxVals->roundsFrom[j]);
		}
	}
}

PlaceStatsNode *getDraculaLocsStats(HunterView hv, PlaceId *possibleLocs, int nPossibleLocs) {
	Round round = HvGetRound(hv);

	PlaceStatsNode minVals = createPlaceStatsNode();
	minVals->p = UNKNOWN;
	minVals->roundsToCD = DBL_MAX;
	minVals->degree = DBL_MAX;

	PlaceStatsNode maxVals = createPlaceStatsNode();
	maxVals->p = UNKNOWN;
	maxVals->roundsToCD = -DBL_MAX;
	maxVals->degree = -DBL_MAX;

	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		minVals->roundsFrom[i] = DBL_MAX;
		maxVals->roundsFrom[i] = -DBL_MAX;
	}

	PlaceStatsNode *statsArray = malloc(nPossibleLocs * sizeof(PlaceStatsNode));
	assert(statsArray != NULL);

	for (int i = 0; i < nPossibleLocs; i++) {
		PlaceStatsNode node = createPlaceStatsNode();

		PlaceId place = possibleLocs[i];
		node -> p = place;
		node -> degree = degree(HvGetMap(hv), place, true, false, true);
	
		for (int j = 0; j < NUM_PLAYERS - 1; j++) {
			int nRoundsFrom = 0;
			PlaceId *path = HvShortestPath(hv, place, HvGetPlayerLocation(hv, j), &nRoundsFrom, round, j);
			node -> roundsFrom[j] = nRoundsFrom;
			free(path);
		}

		int roundsToCD = 0;
		PlaceId *path = HvShortestPath(hv, place, CASTLE_DRACULA, &roundsToCD, round, PLAYER_DRACULA);
		node -> roundsToCD = roundsToCD;
		free(path);

		statsArray[i] = node;
		setPlaceStatsMinMax(minVals, maxVals, node);
	}

	normalisePlaceStats(minVals, maxVals, statsArray, nPossibleLocs);
	return statsArray;
}

void *getMax(void **array, int size, int elemSize, int (*cmp)(void *, void *)) {
	if (size == 0)
		return NULL;

	void *max = *array;
	for (int i = 0; i < size; i++) {
		void *cur = *(array + i);
		if (cmp(max, cur) >= 0)
			max = cur;
	}
	return max;
}

// Returns -1 if a > b, 0 if a == b else 1
int placeStatsCmp(void *a, void *b) {
	double aPriority = calcPlacePriority((PlaceStatsNode) a);
	double bPriority = calcPlacePriority((PlaceStatsNode) b);
	
	return aPriority > bPriority ? -1 : aPriority < bPriority;
}

// Function to check if current move is valid
bool isThisValidMove(HunterView hv, PlaceId move) {
    int nValidMoves = 0;
	PlaceId *validMoves = HvWhereCanIGo(hv, &nValidMoves);
	
	for(int i = 0; i < nValidMoves; i++) {
	    if(move == validMoves[i]) {
			free(validMoves);
	        return true;
	    }
	}
	free(validMoves);
	return false;
}

// Returns max of a and b
double doubleMax(double a, double b) {
	return a < b ? b : a;
}

double doubleMin(double a, double b) {
	return a > b ? b : a;
}

// Function to check who is closest to count_dracula
/*
Player whoIsClosestToCD(HunterView hv) {
    int pathLengthG; // Godalming
    int pathLengthM; // Mina
    int pathLengthH; // Helsing
    int pathLengthS; // Seward
    PlaceId *pathGodalming = HvGetShortestPathTo(hv, LORD_GODALMING, CASTLE_DRACULA, &pathLengthG);
    PlaceId *pathMina = HvGetShortestPathTo(hv, MINA_HARKER, CASTLE_DRACULA, &pathLengthM);
    PlaceId *pathHelsing = HvGetShortestPathTo(hv, VAN_HELSING, CASTLE_DRACULA, &pathLengthH);
    PlaceId *pathSeward = HvGetShortestPathTo(hv, DR_SEWARD, CASTLE_DRACULA, &pathLengthS);
    
    if(pathLengthG >= pathLengthM && pathLengthG >= pathLengthH && pathLengthG >= pathLengthS) {
        return LORD_GODALMING;  
    }
    if(pathLengthH >= pathLengthG && pathLengthH >= pathLengthM && pathLengthH >= pathLengthS) {
        return VAN_HELSING;  
    }
    if(pathLengthS >= pathLengthG && pathLengthS >= pathLengthH && pathLengthS >= pathLengthM) {
        return DR_SEWARD;  
    }
    return MINA_HARKER; // Default go to CD
    
}*/

