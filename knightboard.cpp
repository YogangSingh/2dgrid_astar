// Theodore Nowak 08/19/2015
// A* Search Algorithm
// Knight Board

// Compiling: g++ -std=c++0x
// gcc version 4.6.3 (Ubuntu/Linaro 4.6.3-1ubuntu5)

/* Code satisfying all 5 levels of challenge
** Level 1: Refer to verifySequence() at line 522
** Level 2: Refer to search() at line 369
** Level 3: Refer to search() at line 369
** Level 4: Refer to everything
** Level 5: Refer to everything and note prioritizeShortest() and prioritizeLongest() lines 319, 328
**
** Description:
** To complete this challenge I implemented an A* breadth-based search algorithm in order to find the shortest path to the end goal. It does
** this by creating a priority queue of defined 'Node's and searching for the most cost efficient (or inefficient) path until it reaches the end.
** Then, upon reaching the end point, the reconstructPath() function follows a map of parent coordinates back to the start, and displays
** The completed path taken. A defined Map class also does preprocessing of the input map, and creates an array of map characteristic data
** To be used throughout the program. 
** 
** Robustness:
** I have not found a bug yet after about 5-10 tricky start locations testing teleport, to blocking, and longest and shortest pathing.
** Update: One potential bug: In order to allow teleport moves when validating, the map class must be input to the validateSequence() function.
** But, if one made up their own sequence, and then made a small map, the check would not make sure that their sequence of moves operates inside the map.
** With the final version (level 3-5 of the program) I don't think this matters/is a worthy trade off. But, limitations should always be noted.
**
** Improvements:
** There are a few ways I would improve the code if I had more time. Firstly, I would implement a heuristic to my search algorithm in order to speed
** Up completion time. Secondly, I would used a linked list of nodes pointing to the one that came before it, rather than having an array based map
** With each coordinate having a set of parent coordinates. Additionally, I would make the map input easier for the user, would make switching between
** Shortest and longest paths easier, and would generally shift more onus on my node struct.
**
*/ 

#include <string>
#include <math.h>
#include <stdlib.h>
#include <array>
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

// Maps available for input
// ******************************************************************************************************************************************************************************************************

/*
// Small Map, comment out to use bigger map (bmap)
static const int n = 8;
static char smap [n][n] = {

	// 0    1    2    3    4    5    6    7
	 {'.', '.', '.', '.', '.', '.', '.', '.'}, // 0
	 {'.', '.', '.', '.', '.', '.', '.', '.'}, // 1
	 {'.', 'S', '.', '.', '.', '.', '.', '.'}, // 2
	 {'.', '.', '.', '.', '.', '.', '.', '.'}, // 3
	 {'.', '.', '.', '.', '.', 'E', '.', '.'}, // 4
	 {'.', '.', '.', '.', '.', '.', '.', '.'}, // 5
	 {'.', '.', '.', '.', '.', '.', '.', '.'}, // 6
	 {'.', '.', '.', '.', '.', '.', '.', '.'}  // 7

};
*/



// Big Map, comment out to use smaller map (smap)
static const int n = 32;
static char bmap[n][n] = {

   // 0    1    2    3    4    5    6    7    8    9    10    11    12    13   14    15    16    17    18    19    20    21    22    23    24    25   26    27    28    29    30    31
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  'L',  'L', 'L',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 0
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  'L',  'L', 'L',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 1
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  'L',  'L', 'L',  '.',  '.',  '.',  'L',  'L',  'L',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 2
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  'L',  'L', 'L',  '.',  '.',  'L',  'L',  'L',  '.',  '.',  '.',  'R',  'R',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 3
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  'L',  'L', 'L',  'L',  'L',  'L',  'L',  'L',  '.',  '.',  '.',  'R',  'R',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 4
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  'L',  'L', 'L',  'L',  'L',  'L',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 5
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', '.', '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  'R',  'R',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 6
	{'.', '.', '.', '.', '.', '.', '.', '.', 'B', 'B', '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  'R',  'R',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 7
	{'.', '.', '.', '.', '.', '.', '.', '.', 'W', 'B', 'B',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 8
	{'.', '.', '.', 'R', 'R', '.', '.', '.', 'W', 'W', 'B',  'B',  'B',  'B', 'B',  'B',  'B',  'B',  'B',  'B',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 9
	{'.', '.', '.', 'R', 'R', '.', '.', '.', 'W', 'W', '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  'B',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 10
	{'.', '.', '.', '.', '.', '.', '.', '.', 'W', 'W', '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  'B',  '.',  '.',  '.',  '.',  '.',  '.', 'T',  '.',  '.',  '.',  '.',  '.'}, // 11
	{'.', '.', '.', 'w', 'W', 'W', 'W', 'W', 'W', 'W', '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  'B',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 12
	{'.', '.', '.', 'w', 'W', 'W', 'W', 'W', 'W', 'W', '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  'B',  '.',  '.',  'R',  'R',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 13
	{'.', '.', '.', 'W', 'W', '.', '.', '.', '.', '.', '.',  '.',  '.',  '.', '.',  'B',  'B',  'B',  'B',  'B',  '.',  '.',  'R',  'R',  '.',  'W', 'W',  'W',  'W',  'W',  'W',  'W'}, // 14
	{'.', '.', '.', 'W', 'W', '.', '.', '.', '.', '.', '.',  '.',  '.',  '.', '.',  'B',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  'W', '.',  '.',  '.',  '.',  '.',  '.'}, // 15
	{'W', 'W', 'W', 'W', '.', '.', '.', '.', '.', '.', '.',  '.',  '.',  '.', '.',  'B',  'S',  '.',  '.',  'W',  'W',  'W',  'W',  'W',  'W',  'W', '.',  '.',  '.',  '.',  '.',  '.'}, // 16
	{'.', '.', '.', 'W', 'W', 'W', 'W', 'W', 'W', 'W', '.',  '.',  '.',  '.', '.',  'B',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  'B',  'B',  'B',  'B'}, // 17
	{'.', '.', '.', 'W', 'W', 'W', 'W', 'W', 'W', 'W', '.',  '.',  '.',  '.', '.',  'B',  'B',  'B',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  'B',  '.',  '.',  '.'}, // 18
	{'.', '.', '.', 'W', 'W', 'W', 'W', 'W', 'W', 'W', '.',  '.',  '.',  '.', '.',  '.',  '.',  'B',  'W',  'W',  'W',  'W',  'W',  'W',  'B',  'B', 'B',  'B',  'B',  '.',  '.',  '.'}, // 19
	{'.', '.', '.', 'W', 'W', 'W', 'W', 'W', 'W', 'W', '.',  '.',  '.',  '.', '.',  '.',  '.',  'B',  'W',  'W',  'W',  'W',  'W',  'W',  'B',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 20
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  'B',  'B', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  'B',  'B', '.',  '.',  '.',  '.',  '.',  '.'}, // 21
	{'.', '.', '.', '.', '.', 'R', 'R', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  'B', '.',  '.',  '.',  '.',  '.',  '.'}, // 22
	{'.', '.', '.', '.', '.', 'R', 'R', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  'B', '.',  'T',  '.',  '.',  '.',  '.'}, // 23
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  'R',  'R',  '.',  '.',  '.',  '.',  '.',  '.',  'B', '.',  '.',  '.',  '.',  '.',  '.'}, // 24
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  'R',  'R',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  'E',  '.',  '.'}, // 25
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 26
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 27
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 28
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 29
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  'R',  'R',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 30
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',  'B',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  'R',  'R',  '.',  '.', '.',  '.',  '.',  '.',  '.',  '.'}, // 31
};

// *******************************************************************************************************************************************************************************************************


// Array of movement directions and magnitudes for 8 possible
// knight movements
static const int dirmap[8][2] = {
	{2,1}, {1,2}, {-1,2}, {-2,1}, 
	{-2,-1}, {-1,-2}, {1,-2}, {2,-1}
};

// Class containing map manipulation functions and map characteristic variables
class Map  {

private:
	int start[2]; // Start point on map
	int end[2]; // End point on map
	int final[3]; // Final destination of search, if success should = end

	// Variables to handle teleport locations ('T')
	bool T1;
	int t1[2];
	bool T2;
	int t2[2];

public:

	int size; // Size of map
	char map[n][n]; // Local copy of chosen map

	// Open Map: Map of all locations that have been added to the priority queue
	// Closed Map: Map of all locations that have been at the top of the priority queue and popped
	// Cost Map: Map of the cost to move to each location on the map
	// Parent Element X: The X coordinate of the location that came before the current one (used to find route back)
	// Parent Element Y: The Y coordinate of the location that came before the current one (used to find route back)
	// ********************************************************************************************
	// [0]: Open Map, [1]: Closed Map, [2]: Cost Map, [3]: Parent Element X, [4]: Parent Element Y
	// ********************************************************************************************
	int mapData[n][n][5];

	// Initialize a map class with the map and its size
	Map(int msize, char tmap[n][n]) {
		
		size = msize;

		// Initialize default (nonexistence) values for T1 and T2 teleport locations
		t1[0] = -1;
		t1[1] = -1;
		t2[0] = -1;
		t2[1] = -1;
		T1 = false;
		T2 = false;

		// Initialize final values as absurd to know if none are assigned
		final[0] = -1;
		final[1] = -1;
		final[2] = 0;


		// Create local copy of map and initialize all mapData to starting states
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				map[j][i] = tmap[j][i];
				mapData[j][i][0] = 0;
				mapData[j][i][1] = 0;
				mapData[j][i][2] = 1;
				mapData[j][i][3] = 0;
				mapData[j][i][4] = 0;
			}
		}

		// Run through map and update all mapData information
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {

				// Set start point, and make start cost 0
				if (map[j][i] == 'S') {
					start[0] = i;
					start[1] = j;
					mapData[j][i][2] = 0;
				}

				// Set end point (remain cost of 1 to move there)
				if (map[j][i] == 'E') {
					end[0] = i;
					end[1] = j;
				}

				// Set water movement cost 2
				if (map[j][i] == 'W') {
					mapData[j][i][2] = 2;
				}

				// Set lava movement cost 5
				if (map[j][i] == 'L') {
					mapData[j][i][2] = 5;
				}

				// Find and define the teleport points
				if (map[j][i] == 'T') {
					if (!T1) {
						t1[0] = i;
						t1[1] = j;
						T1 = true;
					}
					else {
						t2[0] = i;
						t2[1] = j;
						T2 = true;
					}
				}
			}
		}

	}

	// Various map get functions
	int getStartX() {return start[0];}
	int getStartY() {return start[1];}
	int getEndX() {return end[0];}
	int getEndY() {return end[1];}
	int getT1X() {return t1[0];}
	int getT1Y() {return t1[1];}
	int getT2X() {return t2[0];}
	int getT2Y() {return t2[1];}
	int getFinalX() {return final[0];}
	int getFinalY() {return final[1];}
	int getFinalCost() {return final[2];}

	char getChar(int x, int y) {return map[y][x];}

	int getOpen(int x, int y) {return mapData[y][x][0];}
	int getClosed(int x, int y) {return mapData[y][x][1];}
	int getCost(int x, int y) {return mapData[y][x][2];}
	int getParentX(int x, int y) {return mapData[y][x][3];}
	int getParentY(int x, int y) {return mapData[y][x][4];}

	// Set the final location of the route
	void setFinal(int x, int y, int cost) {
		final[0] = x;
		final[1] = y;
		final[2] = cost;
	}

	// Set functions for various map data elements
	void setChar(int x, int y, char Char) {
		map[y][x] = Char;
	}

	void addToOpen(int x, int y) {
		mapData[y][x][0] = 1;
	}

	void addToClosed(int x, int y) {
		mapData[y][x][1] = 1;
	}

	void setCost(int x, int y, int Cost) {
		mapData[y][x][2] = Cost;
	}

	void setParent(int x, int y, int px, int py) {
		mapData[y][x][3] = px;
		mapData[y][x][4] = py;
	}

	// Prints information about the map
	void printMapStats() {
		cout << "Start Point (X, Y): (" << getStartX() << ", " << getStartY() << ")\n";
		cout << "End Point (X, Y): (" << getEndX() << ", " << getEndY() << ")\n";
		cout << "Teleporter 1 (X, Y): (" << getT1X() << ", " << getT1Y() << ")\n";
		cout << "Teleporter 1 (X, Y): (" << getT2X() << ", " << getT2Y() << ")\n";
		cout << "Map Size (N, N): (" << size << ", " << size << ")\n";
	}

	// Print the entire map
	void printMap() {
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				cout << map[j][i];
			}
			cout << '\n';
		}
	}

	// Print the entire cost map
	void printCostMap() {
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				cout << mapData[j][i][2];
			}
			cout << '\n';
		}
	}

	// Print the entire map that has been searched
	void printClosedMap() {
		for (int j = 0; j < n; j++) {
			for (int i = 0; i < n; i++) {
				cout << mapData[j][i][1];
			}
			cout << '\n';
		}
	}
};

// Node structure to use in our priority queue
typedef struct Node Node;

struct Node {

	int x; // Position x
	int y; // Position y
	int cost; // Priority/Cost of moving to space

	// Default constructor
	Node() {} 

	Node(int xpos, int ypos) {
		x = xpos; // Set initialized x
		y = ypos; // Set initialized y
		cost = 0; // Set default cost of initialized node to 0: Will be overwritten 
	}

	Node(int xpos, int ypos, int Cost)  {
		x = xpos;
		y = ypos;
		cost = Cost;
	}
};

// Used in the Priority Queue to decide priority: Least cost is priority
struct prioritizeShortest {

	bool operator()(const Node & a, const Node & b) const 
	{
		return a.cost > b.cost;
	}
};

// Used in the Priority Queue to decide priority: Most cost is priority
struct prioritizeLongest {

	bool operator()(const Node & a, const Node & b) const
	{
		return a.cost < b.cost;
	}

};

// Checking if blocked by checking each tile between initial position
// and end of movement position and seeing if a 'B' tile resides there
// if so: YOU ARE BLOCKED!

// Check if blocked for 2 movement in X
bool blockCheckX(int i, Map map, Node current) {
	for (int j = 1; j <= abs(dirmap[i][0]); j++) {
		for (int k = 0; k <= abs(dirmap[i][1]); k++) {
			if (map.getChar(current.x+(j*(dirmap[i][0]/abs(dirmap[i][0]))),
				current.y+(k*(dirmap[i][1]/abs(dirmap[i][1])))) == 'B') {
				return true;	
			}
		}
	}
	return false; 
}

// Check if blocked for 2 movement in Y
bool blockCheckY(int i, Map map, Node current) {
	for (int k = 1; k <= abs(dirmap[i][1]); k++) {
		for (int j = 0; j <= abs(dirmap[i][0]); j++) {
			if (map.getChar(current.x+(j*(dirmap[i][0]/abs(dirmap[i][0]))),
				current.y+(k*(dirmap[i][1]/abs(dirmap[i][1])))) == 'B') {
				return true;	
			}
		}
	}
	return false;
}

// A* search algorithm to find end point
// Inside, to change search from shortest to longest, one must change
// prioritizeShortest to prioritizeLongest in the priority queue definition
Map search(Map map) {

	// 									Priority queue sorting lowest cost options
	// **************************************************************************************************************************
	// ****************** Change prioritizeShortest to prioritizeLongest to get the Longest Path to the End Point ***********************
	// **************************************************************************************************************************
	priority_queue<Node, vector<Node>, prioritizeShortest> openpq;

	// Helper variables for calculations in search
	int currdirx;
	int currdiry;
	int elecost;

	bool blocked = false;  // Blocked path helper variable

	// Set the parent of the starting point to itself
	map.setParent(map.getStartX(), map.getStartY(), map.getStartX(), map.getStartY());

	// Update current with starting point
	Node current(map.getStartX(), map.getStartY(), 0);
	openpq.push(current);

	// Loop through sorted search elements until we arrive at the end
	while (!openpq.empty()) {

		// Update then pop our current position off the open pqueue
		current = openpq.top();
		openpq.pop();

		// Add this point to the closed map to indicate that its neighbors have been searched
		map.addToClosed(current.x, current.y);

		// Stop if we are at the goal, set the final resting position, and return the updated map class
		if (current.x == map.getEndX() && current.y == map.getEndY()) {
			map.setFinal(current.x, current.y, current.cost);
			return map;
		}

		// Run through the possible next positions
		for (int i = 0; i < 8; i++) {
			
			blocked = false;  // Reset blocked bool

			// Define utility variables to store position plus direction movements and update cost function
			currdirx = current.x + dirmap[i][0];
			currdiry = current.y + dirmap[i][1];

			// When checking whether a path/route is blocked:
			// ASSUMPTION: MOVE IN 2 MOVEMENT DIRECTION FIRST
			// Do not add to queue a block pass through
			if (!(currdirx > (map.size-1) || currdirx < 0 || currdiry > (map.size-1) || currdiry < 0)) {

				// Update cost calculation helper variable
				elecost = current.cost + map.getCost(currdirx, currdiry);

				// Check blocked if movement is 2 or -2 in X direction
				if ((i == 0) || (i == 3) || (i == 4) || (i == 7)) {
					blocked = blockCheckX(i, map, current);				
				}
	
				// Check blocked if movement is 2 or -2 in Y direction
				else {
					blocked = blockCheckY(i, map, current);
				}

				// Conditions for teleport locations.
				// Add both to searched map (open map), set the parent of the landed on teleport
				// As the current location, set the landed on teleport as the parent of the teleported to point
				// No extra cost added for the teleport
				if (!(map.getChar(currdirx, currdiry) != 'T' || map.getClosed(currdirx, currdiry) == 1 ||  map.getOpen(currdirx, currdiry) == 1))
				{
					if (!((currdirx != map.getT1X()) || (currdiry != map.getT1Y()))) 
					{
						openpq.push(Node(map.getT2X(), map.getT2Y(), (current.cost + map.getCost(currdirx, currdiry))));
						map.addToOpen(currdirx, currdiry);
						map.setParent(map.getT2X(), map.getT2Y(), map.getT1X(), map.getT1Y());
						map.setParent(map.getT1X(), map.getT1Y(), current.x, current.y);
					}
					else 
					{
						openpq.push(Node(map.getT1X(), map.getT1Y(), (current.cost + map.getCost(currdirx, currdiry))));
						map.addToOpen(currdirx, currdiry);
						map.setParent(map.getT1X(), map.getT1Y(), map.getT2X(), map.getT2Y());
						map.setParent(map.getT2X(), map.getT2Y(), current.x, current.y);
					}
				}
	
				// If the way does not encounter a block, nor does it land on a rock,
				// Nor has it been landed on or added already
				// Procede with adding the movement to next vector
				else if (!(blocked || (map.getChar(currdirx, currdiry) == 'R') || 
						(map.getClosed(currdirx, currdiry) == 1) || (map.getOpen(currdirx, currdiry) == 1))) 
				{
					openpq.push(Node(currdirx, currdiry, (current.cost + map.getCost(currdirx, currdiry))));
					map.addToOpen(currdirx, currdiry);
					map.setParent(currdirx, currdiry, current.x, current.y);
				}
			}
		}
	}
	// If there is no route, return the start points, and 0 cost for our final rest point
	map.setFinal(map.getStartX(), map.getStartY(), 0);
	return map;
}

// Reconstructs the path back from the end point to the start
// Via the route recorded in mapData
vector<Node> reconstructPath(Map map) {
	
	vector<Node> total_path;  // Vector of the path back to the start

	// Set our rerouting loop vars as the final location of x and y
	// And set local var to the final cost of route
	int i = map.getFinalX();
	int j = map.getFinalY();
	int cost = map.getFinalCost();

	total_path.push_back(Node(i,j));

	int tempi;
	int tempj;

	// Follow the map of parent nodes back to the map start point
	// And push each successive Node into the total_path
	while ((i != map.getStartX()) || (j != map.getStartY())) {

		tempi = map.getParentX(i, j);
		tempj = map.getParentY(i, j);

		i =  tempi;
		j = tempj;

		total_path.push_back(Node(i,j));
	}

	cout << "Original Map:\n";
	map.printMap();
	cout << "\n";

	// Mark 'K' on map where we have moved to
	for (int i = 0; i < total_path.size()-1; i++) {
		map.setChar(total_path[i].x, total_path[i].y, 'K');
	}

	cout << "Map of Moves:\n";
	map.printMap();

	cout << "Cost of Movement: " << cost << "\n";

	return total_path;
}

// Function that verifies a sequence of moves
bool verifySequence(vector<Node> total_path, Map map) {

	for (int i = 0; i < total_path.size()-1; i++) {
		if (!((abs(total_path[i].x - total_path[i+1].x) == 1 &&
			   abs(total_path[i].y - total_path[i+1].y) == 2) ||
			  (abs(total_path[i].x - total_path[i+1].x) == 2 &&
			   abs(total_path[i].y - total_path[i+1].y) == 1)))
		{
			if (!((total_path[i+1].x == map.getT1X() && total_path[i+1].y == map.getT1Y()) ||
				  (total_path[i+1].x == map.getT2X() && total_path[i+1].y == map.getT2Y())))
			{
				cout << "The sequence failed to pass inspection.\n";
				return false;
			}
		}
	}
	cout << "The sequence successfully passed inspection.\n";
	return true;
}

int main() {

	// Map being loaded in
	Map map(n,bmap);

	// Create a bad sequence of moves to test verifySequence on
	vector<Node> test = {Node(1,1), Node(3,2), Node(4,4), Node(5,6), Node(7,8), Node(9,9), Node(10,11)};
	
	// Search, generate routes, plots maps, and verify for select map
	verifySequence(reconstructPath(search(map)), map);
	
	// Verify our test sequence (should return bad)
	verifySequence(test, map);

	return(0);
} 
