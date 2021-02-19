////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

// add your own #includes here

enum encounter {
	NO_ENCOUNTER,
	TRAP,
	IMMATURE_VAMP
};

struct drac_move {
	PlaceId loc;
	enum encounter enc;
};

typedef struct drac_move drac_move_t;

struct gameView {
	Map map;
	int score;
	Round round;
	Player player;
	int health[NUM_PLAYERS];
	PlaceId location[NUM_PLAYERS];
	drac_move_t trail[TRAIL_SIZE];
	bool heal_hunter_next[NUM_PLAYERS];

	PlaceId* past_moves[NUM_PLAYERS];
	PlaceId* past_locs[NUM_PLAYERS];
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
// Eric

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView new = malloc(sizeof(*new));
	if (new == NULL)
	{
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	
	//Initialising map, score, etc. 
	new->map = MapNew();
	new->score = GAME_START_SCORE;
	new->round = 0;
	new->player = PLAYER_LORD_GODALMING;
    
    //Used to allocate memory by number of rounds
	int num_rounds = (strlen(pastPlays))/40 + 1;
    
    //Initialise player health etc.
	for (Player p = 0; p < (int)NUM_PLAYERS; p++)
	{
		new->health[p] = GAME_START_HUNTER_LIFE_POINTS;
		new->location[p] = NOWHERE;
		new->past_locs[p] = malloc(num_rounds * sizeof(PlaceId));
		new->past_moves[p] = malloc(num_rounds * sizeof(PlaceId));
		new->heal_hunter_next[p] = false;
	}
	
	//Override Dracula Health
	new->health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
    
    //Initialise Dracula trail
	for (int i = 0; i < TRAIL_SIZE; i++) {
		new->trail[i] = (drac_move_t){.loc = NOWHERE, .enc = NO_ENCOUNTER};
	}
    
    //Buffer turn characters
	char turn[8] = {0};
	
	//Simulate every turn (basically game engine)
	//Changes gameview appropriately
	for (int offset = 0; offset < strlen(pastPlays); offset += 8)
	{
	    //Copy current move into turn buffer
		strncpy(turn, pastPlays + offset, 7);
		//Player's move
		Player current_player = PlayerCharToEnum(turn[0]);
		//Current location abbrev
		char loc_chars[3] = {0};
		//Fills the location buffer
		strncpy(loc_chars, turn + 1, 2);
		//Player's new location
		PlaceId new_location = placeAbbrevToId(loc_chars);
		//Add the new location to past moves
		new->past_moves[current_player][new->round] = new_location;
		//If hunter move
		if (current_player != PLAYER_DRACULA)
		{
		    if (new->heal_hunter_next[current_player] == true) {
		        new->health[current_player] = GAME_START_HUNTER_LIFE_POINTS;
		        new->heal_hunter_next[current_player] = false;
		    }
		    //If they didn't move
			bool no_move = (new_location == new->location[current_player]);
			//Checking for encounters
			for (int i = 3; i < 7; i++)
			{
				if (turn[i] == 'T')
				{
					new->health[current_player] -= LIFE_LOSS_TRAP_ENCOUNTER;
					for (int i = TRAIL_SIZE - 1; i >=0; i--) {
						if (new->trail[i].loc == new_location && new->trail[i].enc == TRAP) {
							new->trail[i].enc = NO_ENCOUNTER;
							break;
						}
					}
				}
				else if (turn[i] == 'D')
				{
					new->health[current_player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
					new->health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
				}
				else if (turn[i] == 'V')
				{
					for (int i = TRAIL_SIZE - 1; i >=0; i--) {
						if (new->trail[i].loc == new_location && new->trail[i].enc == IMMATURE_VAMP) {
							new->trail[i].enc = NO_ENCOUNTER;
							break;
						}
					}
				}
				else
				{
					// A '.', we are done.
					break;
				}
                //Checking if hunter lost all health
				if (new->health[current_player] <= 0)
				{
					new_location = HOSPITAL_PLACE;
					new->heal_hunter_next[current_player] = true;
					new->health[current_player] = 0;
					new->score -= SCORE_LOSS_HUNTER_HOSPITAL;
					break;
				}
			}
			//Resting
			if (no_move && (new_location == new->location[current_player]))
				new->health[current_player] += LIFE_GAIN_REST;

			if (new->health[current_player] > 9)
				new->health[current_player] = 9;

			new->location[current_player] = new_location;
			new->past_locs[current_player][new->round] = new_location;
		}
		else // Dracula
		{
		    //Vampire matures
			if (new->trail[5].enc == IMMATURE_VAMP) {
				new->score -= SCORE_LOSS_VAMPIRE_MATURES;
			}
            //Shifts trail down
			memmove(new->trail + 1, new->trail, sizeof(drac_move_t) * 5);
			//Fills the turn buffer
			strncpy(turn, pastPlays + offset, 7);
			//Location buffer filled
			char loc_chars[3] = {0};
			strncpy(loc_chars, turn + 1, 2);
			//Converts to PlaceId
			PlaceId new_location = placeAbbrevToId(loc_chars);
			
			
            //Process double back
			if (DOUBLE_BACK_1 <= new_location && new_location <= DOUBLE_BACK_5) {
				new_location = new->trail[new_location - DOUBLE_BACK_1 + 1].loc;
			}
			//Process hide
			else if (new_location == HIDE) {
				new_location = new->trail[1].loc;
			}
			else if (new_location == TELEPORT) {
			    new_location = CASTLE_DRACULA;
			}
            //Override location
			new->location[PLAYER_DRACULA] = new_location;
            //Losing health at sea
			if (placeIsSea(new_location)) {
				new->health[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
			}
			//Recovering at castle
			else if (new_location == CASTLE_DRACULA) {
				new->health[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
			}
			//Next Dracula encounter
			drac_move_t curr_move;
			curr_move.loc = new_location;
			//Deciding which encounter
			if (turn[4] == 'V') {
				curr_move.enc = IMMATURE_VAMP;
			}
			else if (turn[3] == 'T') {
				curr_move.enc = TRAP;
			}
			else {
			    curr_move.enc = NO_ENCOUNTER;
			}
			//Setting at beginning of trail
			new->trail[0] = curr_move;

            //Losing a score for Dracula surviving
			new->score -= SCORE_LOSS_DRACULA_TURN;
			//Adding into array
			new->past_locs[current_player][new->round] = new_location;
			new->round++;
		}
        //Increment player
		new->player++;
		new->player%=NUM_PLAYERS;
	}

	return new;
}

void GvFree(GameView gv)
{
	assert(gv != NULL);
	for (int i = 0 ; i < NUM_PLAYERS ; i++) {
        free(gv->past_moves[i]);
        free(gv->past_locs[i]);
    }
	MapFree(gv->map);
	free(gv);
	return;
}

////////////////////////////////////////////////////////////////////////
// Game State Information
//Joey
Round GvGetRound(GameView gv)
{
    assert(gv != NULL);
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
    assert(gv != NULL);
	return (gv->player);
}

int GvGetScore(GameView gv)
{
    assert(gv != NULL);
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
    assert(gv != NULL);
	return gv->health[player];
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// player's health is 0, and they're a hunter, they're at the hospital
	if (player == PLAYER_DRACULA && gv->health[player] == 0) {
		return HOSPITAL_PLACE;
	}
	return gv->location[player];
}

PlaceId GvGetVampireLocation(GameView gv)
{
    assert(gv != NULL);
	for (int i = 0; i < TRAIL_SIZE; i++) {
		if (gv->trail[i].enc == IMMATURE_VAMP) {
			return gv->trail[i].loc;
		}
	}

	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
    assert(gv != NULL);
	PlaceId* ids = malloc(TRAIL_SIZE * sizeof(PlaceId));
	int j = 0;
	for (int i = 0; i < TRAIL_SIZE; i++) {
		if (gv->trail[i].enc == TRAP) {
		ids[j++] = gv->trail[i].loc;
		}
	}
	*numTraps = j;
	return ids;
}

////////////////////////////////////////////////////////////////////////
// Game History
// Sean
PlaceId *GvGetMoveHistory(GameView gv, Player player,
						  int *numReturnedMoves, bool *canFree)
{
	*numReturnedMoves = gv->round + (gv->player > player);
	// printf("%d\n", *numReturnedMoves);
	*canFree = false;
	return gv->past_moves[player];
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
						int *numReturnedMoves, bool *canFree)
{
	*canFree = false;
	// Move history array size
	int size = gv->round + (gv->player > player);

	if (numMoves > size) {
		*numReturnedMoves = size;
	} else {
		*numReturnedMoves = numMoves;
	}
									
	int start = size - numMoves;	// V - Acts as else if for pointer arithmetic 
	return gv->past_moves[player] + ((start > 0) * start);

}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
							  int *numReturnedLocs, bool *canFree)
{
	*numReturnedLocs = gv->round + (gv->player > player);
	*canFree = false;
	return gv->past_locs[player];
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
							int *numReturnedLocs, bool *canFree)
{
	*canFree = false;
	*numReturnedLocs = numLocs;
	// Move history array size
	int size = gv->round + (gv->player > player);

	if (numLocs > size) {
		*numReturnedLocs = size;
	}								
	int start = size - numLocs;	// V - Acts as else if for pointer arithmetic 
	return gv->past_locs[player] + ((start > 0) * start);
}

////////////////////////////////////////////////////////////////////////
// Making a Move
// Liam
PlaceId *GvGetReachable(GameView gv, Player player, Round round,
						PlaceId from, int *numReturnedLocs) {
    // TODO
	/*PlaceId* ids = malloc(MapNumPlaces(gv->map) * sizeof(PlaceId));
	ConnList conns = MapGetConnections(gv->map, from);
	int j = 0;
	for (int i = 0; i < MapNumPlaces(gv->map); i++) {
		ids[j++] = 0;
	}
	*numReturnedLocs = 0;*/
	
	assert(gv != NULL);
	return GvGetReachableByType(gv, player, round, from, true, true, true, numReturnedLocs);
	
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
							  PlaceId from, bool road, bool rail,
							  bool boat, int *numReturnedLocs) {
							  
	*numReturnedLocs = 0;
	assert(gv != NULL);
	assert(from >= 0);
	
	// Map init and variables
    Map m = getMap(gv);
	int NUM_LOC = MapNumPlaces(m); // Number of locations in map
	// Array for viable locations
    PlaceId *arrayLoc = calloc(NUM_LOC, sizeof(PlaceId)); 
    arrayLoc[0] = from;
    // Array for visited locations
	char *visited = calloc(NUM_LOC, sizeof(char));

	// Variables 
	int railDist = (round + player) % 4; // Moves hunter can make by rail
	ConnList curr = MapGetConnections(m, from); // 
	int flag = 0; // Flag to skip loops
    
    while(curr != NULL) {
        flag = 0;
        // Player is dracula and at the church
        if(curr->p == ST_JOSEPH_AND_ST_MARY && player == PLAYER_DRACULA) flag = 1;
        // Not dracula and not at church
        if(flag == 0) {
            // Dracula cant take the RAIL, Location is of RAIL type 
            if(player != PLAYER_DRACULA && rail && curr->type == RAIL && (railDist >= 1)) {
                visited[curr->p] = 1; // Viable connection for hunters
            // Location is of ROAD type, Both dracula and hunters can travel   
            } else if(curr->type == ROAD && road && visited[curr->p] != 1) {
                visited[curr->p] = 'R'; // Viable connection for all types of players
            // Location is of BOAT type, Both dracula and hunters can travel
            } else if(curr->type == BOAT && boat && visited[curr->p] != 1) {
                visited[curr->p] = 'B'; // Viable connection for all types of players
            }
        }
        curr = curr->next; // Move to next node 
    }
    // For hunters to see there rail options
    if(railDist >= 2) {
        for(int i = 0; i < NUM_LOC; i++) {
            if(visited[i] == 1) { // RAIL TYPE
                curr = MapGetConnections(m, i);
                while(curr != NULL) {
                    if(curr->type == RAIL) {
                        visited[curr->p] = 2; // Max rail distance is 2
                    }
                    curr = curr->next; // Move to next node 
                }
            }
        }
    }
    // Same as above but more travel options due to increased moves
    if(railDist >= 3) {
        for(int i = 0; i < NUM_LOC; i++) {
            if(visited[i] == 2) {
                curr = MapGetConnections(m, i);
                while(curr != NULL) {
                    if(curr->type == RAIL) {
                        visited[curr->p] = 3; // Max rail distance is 3
                    }
                    curr = curr->next;
                }
            }
        }
    }
    
    int count = 0;
    // Loop through visited array to find viable connections
    for(int i = 0; i < NUM_LOC; i++) {
        // Is not a viable, continue to next iteration
        if((visited[i] != 1 && visited[i] != 2 && visited[i] != 3 &&
            visited[i] != 'R' && visited[i] != 'B') && i != from) {
        //if (visited[i] == 0 && i != from) {
            continue;
        }
        // Viable locations from current location, add to array of locations
        arrayLoc[count] = i;
        count++;
    }

	*numReturnedLocs = count;
	free(visited);
	//printf("%d\n", *numReturnedLocs);
	return arrayLoc;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

Player PlayerCharToEnum(char c)
{
	if (c == 'G')
	{
		return PLAYER_LORD_GODALMING;
	}
	else if (c == 'S')
	{
		return PLAYER_DR_SEWARD;
	}
	else if (c == 'H')
	{
		return PLAYER_VAN_HELSING;
	}
	else if (c == 'M')
	{
		return PLAYER_MINA_HARKER;
	}
	else if (c == 'D')
	{
		return PLAYER_DRACULA;
	}
	else
	{
		return -1;
	}
}

Map getMap(GameView gv) {
	return gv -> map;
}

void debugPrintGv(GameView gv) {
    for (int i = 0; i < TRAIL_SIZE; i++) {
        printf("Loc: %s, Enc: %d\n", placeIdToName(gv->trail[i].loc), gv->trail[i].enc);
    }
}

// have a function to mallocing and copying arrays
