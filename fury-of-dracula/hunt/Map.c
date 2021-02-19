////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Map.c: an implementation of a Map type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "Map.h"
#include "Places.h"
#include "Queue.h"

struct map {
	int nV; // number of vertices
	int nE; // number of edges
	ConnList connections[NUM_REAL_PLACES];
};

static void addConnections(Map m);
static void addConnection(Map m, PlaceId v, PlaceId w, TransportType type);
static inline bool isSentinelEdge(Connection c);

static ConnList connListInsert(ConnList l, PlaceId v, TransportType type);
static bool connListContains(ConnList l, PlaceId v, TransportType type);

////////////////////////////////////////////////////////////////////////

/** Creates a new map. */
// #vertices always same as NUM_REAL_PLACES
Map MapNew(void)
{
	Map m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Couldn't allocate Map!\n");
		exit(EXIT_FAILURE);
	}

	m->nV = NUM_REAL_PLACES;
	m->nE = 0;
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		m->connections[i] = NULL;
	}

	addConnections(m);
	return m;
}

/** Frees all memory allocated for the given map. */
void MapFree(Map m)
{
	assert (m != NULL);

	for (int i = 0; i < m->nV; i++) {
		ConnList curr = m->connections[i];
		while (curr != NULL) {
			ConnList next = curr->next;
			free(curr);
			curr = next;
		}
	}
	free(m);
}

////////////////////////////////////////////////////////////////////////

/** Prints a map to `stdout`. */
void MapShow(Map m)
{
	assert(m != NULL);

	printf("V = %d, E = %d\n", m->nV, m->nE);
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			printf("%s connects to %s by %s\n",
			       placeIdToName((PlaceId) i),
			       placeIdToName(curr->p),
			       transportTypeToString(curr->type)
			);
		}
	}
}

////////////////////////////////////////////////////////////////////////

/** Gets the number of places in the map. */
int MapNumPlaces(Map m)
{
	assert(m != NULL);
	return m->nV;
}

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type)
{
	assert(m != NULL);
	assert(transportTypeIsValid(type) || type == ANY);

	int nE = 0;
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			if (curr->type == type || type == ANY) {
				nE++;
			}
		}
	}

	return nE;
}

////////////////////////////////////////////////////////////////////////

/// Add edges to Graph representing map of Europe
static void addConnections(Map m)
{
	assert(m != NULL);

	for (int i = 0; !isSentinelEdge(CONNECTIONS[i]); i++) {
		addConnection(m, CONNECTIONS[i].v, CONNECTIONS[i].w, CONNECTIONS[i].t);
	}
}

/// Add a new edge to the Map/Graph
static void addConnection(Map m, PlaceId start, PlaceId end, TransportType type)
{
	assert(m != NULL);
	assert(start != end);
	assert(placeIsReal(start));
	assert(placeIsReal(end));
	assert(transportTypeIsValid(type));

	// don't add edges twice
	if (connListContains(m->connections[start], end, type)) return;

	m->connections[start] = connListInsert(m->connections[start], end, type);
	m->connections[end]   = connListInsert(m->connections[end], start, type);
	m->nE++;
}

/// Is this the magic 'sentinel' edge?
static inline bool isSentinelEdge(Connection c)
{
	return c.v == -1 && c.w == -1 && c.t == ANY;
}

/// Insert a node into an adjacency list.
static ConnList connListInsert(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	ConnList new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate ConnNode");
		exit(EXIT_FAILURE);
	}
	
	new->p = p;
	new->type = type;
	new->next = l;
	return new;
}

/// Does this adjacency list contain a particular value?
static bool connListContains(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	for (ConnList curr = l; curr != NULL; curr = curr->next) {
		if (curr->p == p && curr->type == type) {
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////

ConnList MapGetConnections(Map m, PlaceId p)
{
	assert(placeIsReal(p));
	return m->connections[p];
}

////////////////////////////////////////////////////////////////////////

// TODO: confirm similar code in gameview
bool isConnected(struct connNode node, bool road, bool rail, bool boat) {
	return ((node.type == ROAD && road)
		|| (node.type == RAIL && rail)
		|| (node.type == BOAT && boat));
}


bool is_sea_loc(PlaceId loc) {
	for (int i = MIN_REAL_PLACE; i < NUM_REAL_PLACES; i++)
	{
		if (PLACES[i].id == loc) return (PLACES[i].type == SEA);
	}
	return false;
}
// Return array of shortest path from src to dest. NULL if dest not connected
// Note: shortest path does not include src
PlaceId *shortestPath(GameView gv, PlaceId src, PlaceId dest, int *size, Round round, Player player)
{
	Map m = getMap(gv);
	bool seen[m -> nV];
	int pred[m -> nV];
	for (int i = 0; i < m -> nV; i++) {
		seen[i] = false;
		pred[i] = -1;
	}
	Queue q = newQueue();
	QueueJoin(q, src);
	seen[src] = true;

	int curRound = round;
	int placesLeftInRound = QueueSize(q);
	
	bool foundDest = false;
	while (!QueueIsEmpty(q) && !foundDest) {
		int cur = QueueLeave(q);
		int nNeighbours = 0;
		PlaceId *neighbours = GvGetReachable(gv, player, curRound, cur, &nNeighbours);
		for (int i = 0; i < nNeighbours; i++) {
			
			PlaceId place = neighbours[i];
			if (!seen[place]) {
				QueueJoin(q, place);
				seen[place] = true;
				pred[place] = cur;
			}
			if (place == dest) {
				foundDest = true;
				break;
			}
		}
		free(neighbours);
		placesLeftInRound--;
		if (placesLeftInRound == 0) {
			placesLeftInRound = QueueSize(q);
			curRound++;
		}
	}

	dropQueue(q);

	*size = 0;
	if (!foundDest)
		return NULL;

	for (int i = dest; pred[i] != -1; i = pred[i]) {
		*size += 1;
	}
	PlaceId *path = malloc(*size * sizeof(PlaceId));
	assert(path != NULL);
	for (int i = dest, j = *size - 1; pred[i] != -1; i = pred[i], j--) {
		path[j] = i;
	}
	return path;
}

// is able to return all the possible location of a character, given where they are and what round it is
PlaceId *possibleLocs(GameView gv, PlaceId src, int *size, Round startRound, Round endRound, Player player)
{
	Map m = getMap(gv);
	bool seen[m -> nV];
	for (int i = 0; i < m -> nV; i++) {
		seen[i] = false;
	}
	Queue q = newQueue();
	QueueJoin(q, src);
	seen[src] = true;

	Queue visited = newQueue();

	int curRound = startRound;
	int placesLeftInRound = QueueSize(q);
	
	while (!QueueIsEmpty(q)) {
		int cur = QueueLeave(q);
		QueueJoin(visited, cur);
		int nNeighbours = 0;
		PlaceId *neighbours = GvGetReachable(gv, player, curRound, cur, &nNeighbours);
		// Remove trail from Dracula's neighbours
		/*if (player == PLAYER_DRACULA) {
			int numMoves = TRAIL_SIZE - (endRound - startRound);
			//printf("numMoves %d, endround %d, startround %d\n\n\n", numMoves, endRound, startRound);
			int numReturnedMoves = 0;
			bool canFree = true;
			PlaceId *trail = GvGetLastMoves(gv, player, numMoves,
							&numReturnedMoves, &canFree);
			for (int i = 0; i < numReturnedMoves; i++) {
				if (trail[i] != NOWHERE && trail[i] != 100 && neighbours[trail[i]] != 0) {
					neighbours[trail[i]] = 0;
					printf("%s was set to unreachable for dracula\n\n\n", placeIdToAbbrev(trail[i]));
					fflush(stdout);
					//assert(0);
					
				}
			}
			printf("reached end of for loop\n\n\n\n");
			fflush(stdout);
		}*/








		for (int i = 0; i < nNeighbours; i++) {
			PlaceId place = neighbours[i];
			if (!seen[place]) {
				QueueJoin(q, place);
				seen[place] = true;
			}
		}
		free(neighbours);
		placesLeftInRound--;
		if (placesLeftInRound == 0) {
			placesLeftInRound = QueueSize(q);
			curRound++;
		}

		// Visted all possible locations by endRound
		if (curRound == endRound)
			break;
	}

	dropQueue(q);
	
	*size = QueueSize(visited);
	PlaceId *result = malloc(*size * sizeof(PlaceId));
	assert(result != NULL);
	printf("result: ");
	for (int i = 0; i < *size; i++) {
		result[i] = QueueLeave(visited);
		printf("%d, ", result[i]);
	}
	printf("\n\n\n");
	fflush(stdout);
	dropQueue(visited);
	
	return result;
}

int degree(Map m, PlaceId place, bool road, bool rail, bool boat) {
	int degree = 0;
	ConnList curr = MapGetConnections(m, place);
	for (; curr != NULL; curr = curr->next) {
		if (isConnected(*curr, road, rail, boat) && curr -> p != place) {
			degree++;
		}
	}
	return degree;

}