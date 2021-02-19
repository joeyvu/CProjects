////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "PQueue.h"
#include "Map.h"
#include <math.h>
#include <stdio.h>
#include <string.h>




// Could just store floats
#define SCORE_NORMALISE 10.0f


// My idea is to use these weights to perfect the AI performance
// If we're clever we can change these values depending on performance
#define DIST_WEIGHT 0.14f
#define DEGREE_WEIGHT 1.2f
#define SEA_MOVE_WEIGHT 1.0f
#define ATTACK_HUNTER_WEIGHT -35.0f
#define CD_WEIGHT 10

// TODO Average is very basic and a smarter calculation will increase performance a lot
int calculate_dist(Queue q) {
	int average = 0;
	int size = QueueSize(q);
	while (!QueueIsEmpty(q)) {
		average +=  QueueLeave(q);
	}
	return average / (size + 1) * SCORE_NORMALISE;

}






int viability(DraculaView dv, PlaceId place) {
	// I have no enum experience idk if theres a smarter way to do this
	Player hunters[4] = {PLAYER_LORD_GODALMING, PLAYER_MINA_HARKER, PLAYER_DR_SEWARD, PLAYER_VAN_HELSING};
	// No real reason to use queue except we have it and dont know the number of results
	Queue q = newQueue();
	PlaceId *reachable;
	PlaceId *temp;
	int numLocs = 0;
	int length = 0;
	int original_dist = 0;
	// TODO should maybe take into account that closer hunters are more important than really far away ones
	for (int i = 0; i < 4; i++) {
		// Time taken could be too long, will need to write one algorithm for all locations otherwie
		// Since this will run a new BFS for each location

		// This allows us to disregard locations which dont bring hunter closer
		temp = DvshortestPath(dv, place, DvGetPlayerLocation(dv, hunters[i]), &original_dist, DvGetRound(dv), hunters[i]);
		free(temp);
		reachable = DvWhereCanTheyGo(dv, hunters[i], &numLocs);
		for (int j = 0; j < numLocs; j++) {
			// printf("Shortest path to %s\n", placeIdToName(reachable[j]));
			temp = DvshortestPath(dv, place, reachable[j], &length, DvGetRound(dv), hunters[i]);
			free(temp);
			if (length) {
				if (original_dist < 15) {
					QueueJoin(q, length * abs((15 - original_dist)));
				} else {
					QueueJoin(q, length);
				}
			} 
		}
		
	}
	int viability = 0;
	// Maybe we want to store each hunters individual distances distances in a struct for calculations

	viability += calculate_dist(q) * DIST_WEIGHT;
	// If we want to add special cases we can just up the viability
	Map map = MapNew();
	viability += degree(map, place, true, true, true) * DEGREE_WEIGHT;

	// TODO
	if (is_sea_loc(place)) {
		if (DvGetHealth(dv, PLAYER_DRACULA) >= 40 /* Maybe add if hunters are really close */) {
			viability +=  10 * SEA_MOVE_WEIGHT;
		} 
		// else if (DvGetHealth(dv, PLAYER_DRACULA) < 10) {
		//     viability -= 10 * SEA_MOVE_WEIGHT;
		// }
		else {
			viability -=  5 * SEA_MOVE_WEIGHT;
		}

	}
	
	
	if (place == CASTLE_DRACULA) viability += CD_WEIGHT;

	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (DvGetPlayerLocation(dv, hunters[i]) == place) count ++;
	}

	viability += count * ATTACK_HUNTER_WEIGHT;

	// TODO add lots of viabilities
	// Try to get the scores right

	// Distance from border could be another factor

	printf("Viabality of %s is %d\n", placeIdToName(place), viability);

	return viability;

}


void get_start_move(DraculaView dv, PQueue pq) {
	int size = 0;
	PlaceId *all = DvpossibleLocs(dv, CASTLE_DRACULA, &size, 0, 366, PLAYER_DRACULA);
	for (int i = 0; i < size; i++) {
		PQueueJoin(pq, all[i], viability(dv, all[i]));
	}
	return;
}


void decideDraculaMove(DraculaView dv)
{ 

	// Using lots of different functions to facilitate working on same file and for upgradability

	// Distance self from hunters

	// Sea move lower priority unless getaway needed

	// Place Hard to avoid traps

	// Place vampires in hard to reach locations

	// Castle Dracula gives you more health

	// Not at sea during round%13 == 0

	// High degree of edges is good unless placing a vampire

	// Favour moving towards player 4 if a tie?

	// Hiding after placing vampire to place trap too might be clever


	
	PQueue pq = newPQueue();

	// First move TO DO
	if (DvGetRound(dv) == 0) {
		get_start_move(dv, pq);
		registerBestPlay((char *)placeIdToAbbrev(PQueueLeave(pq)), "Off I pop");
		return;

	} else {

		// No sea or road priority yet
		int numLocs = 0;
		PlaceId *reachable = DvGetValidMoves(dv, &numLocs);

		


		// Dracula has to teleport
		if (reachable == NULL) {
			printf("None reachable\n");
			registerBestPlay((char *)placeIdToAbbrev(CASTLE_DRACULA), "Off I pop");
			return;
		}


		// Will also let us update our registered best move until time runs out as it gets updated
		

		// Join the viability of each location to the queue. Can be expanded later to look further into future
		for (int i = 0; i < numLocs; i++)
		{	
			printf("Checking %s\n", placeIdToName(reachable[i]));
			// Using valid moves array and checking validity on the location the move corresponds to
			PQueueJoin(pq, reachable[i], viability(dv, move_to_location(dv, reachable[i])));
		}
	}
	
	
	PlaceId best_move = PQueueLeave(pq);


	// int temp = 0;
	// PlaceId *test2 = DvWhereCanIGo(dv, &temp);
	// printf("%d\n", temp);
	// for (int i = 0; i < temp; i++)
	// {
	// 	printf("Places to go is %s\n", placeIdToName(test2[i]));
	// }

	// printf("\n\n");

	// for (int i = 0; i < TRAIL_SIZE; i++)
	// {
	// 	 printf("Trail is %s\n",placeIdToName(trail_place(dv, i)));
	// }
	

	// PlaceId *test = DvGetValidMoves(dv, &temp);
	// for (int i = 0; i < temp; i++)
	// {
	// 	printf("Valid is %s\n", placeIdToName(test[i]));
	// }

	// Might need to copy into new string instead of typecast
	registerBestPlay(strdup(placeIdToAbbrev(best_move)), "Mwahahahaha");
}
