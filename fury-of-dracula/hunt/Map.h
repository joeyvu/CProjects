////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... The Fury of Dracula
// Map.h: an interface to a Map data type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

#include <stdbool.h>
#include <stdlib.h>

#include "Places.h"
#include "Queue.h"
#include "Game.h"
#include "GameView.h"

typedef struct gameView *GameView;

typedef struct connNode *ConnList;
struct connNode {
	PlaceId p; // ALICANTE, etc.
	TransportType type; // ROAD, RAIL, BOAT
	ConnList next; // link to next node
};

typedef struct pathNode *PathNode;

// Map representation is hidden
typedef struct map *Map;

/** Creates a new map. */
Map MapNew(void);

/** Frees all memory allocated for the given map. */
void MapFree(Map m);

/** Prints a map to `stdout`. */
void MapShow(Map m);

/** Gets the number of places in the map. */
int MapNumPlaces(Map m);

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type);

/**
 *  Gets a list of connections from the given place.
 *  The returned list should NOT be modified or freed.
 */
ConnList MapGetConnections(Map m, PlaceId p);

int degree(Map m, PlaceId place, bool road, bool rail, bool boat);

bool is_sea_loc(PlaceId loc);

bool isConnected(struct connNode node, bool road, bool rail, bool boat);

PlaceId *shortestPath(GameView gv, PlaceId src, PlaceId dest, int *size, Round round, Player player);

PlaceId *possibleLocs(GameView gv, PlaceId src, int *size, Round startRound, Round endRound, Player player);
#endif // !defined(FOD__MAP_H_)
