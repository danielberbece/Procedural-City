#include "City.h"
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <Core/Engine.h>

using namespace std;

City::City() {
	const string textureLoc = "Source/Tema3/Textures/";

	// Load textures
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "img5.jpg").c_str(), GL_REPEAT);
		mapTextures["walls1"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "roof.jpg").c_str(), GL_REPEAT);
		mapTextures["roof1"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "road-nostripe.jpg").c_str(), GL_REPEAT);
		mapTextures["road"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "cross-road.jpg").c_str(), GL_REPEAT);
		mapTextures["crossroad"] = texture;
	}

	streets = new Streets(mapTextures["road"], mapTextures["crossroad"]);


	std::srand(std::time(NULL));
	int rootLine = rand() / (RAND_MAX / MODEL_WIDTH);
	int rootCol = rand() / (RAND_MAX / MODEL_WIDTH);
	generateStreets(rootLine, rootCol, North);
}

City::~City() {
}

void City::addStreetsToMap() {
	
}

void City::renderCity(Shader *shader, EngineComponents::Camera *camera) {
	for (auto building : buildings) {
		building->render(shader, camera);
	}

	streets->render(shader, camera);
}

void City::generateStreets(int line, int col, Direction fromDir) {
	if (!inMapArea(line, col)) {
		// Reached end of map. The End.
		return;
	}

	// Current at the intersection:
	if (streetMap[line][col] == 0) {
		streetMap[line][col] = 1;
		// Check if ahead is a street. If it is, stop
		if (isNextOccupied(line, col, oppositeDirection(fromDir))) {
			streets->addStreet(ROAD, meterUnitsScale, meterUnitsScale, col * meterUnitsScale, line * meterUnitsScale, (int)fromDir);
		} else {
			streets->addStreet(CROSS, meterUnitsScale, meterUnitsScale, col * meterUnitsScale, line * meterUnitsScale, (int)fromDir);
			// go in 3 directions 
			for (int i = North; i <= West; i++) {
				if (i != (int)fromDir && !isNextOccupied(line, col, (Direction)i)) {	// Go in this dir
					// How much?
					makeStreet(line, col, (Direction)i);
				}
			}
		}
	}
}

int City::isNextOccupied(int line, int col, Direction dir) {
	switch (dir) {
	case North:
		if (streetMap[line - 1][col] == 1 || streetMap[line - 1][col - 1] == 1 || streetMap[line - 1][col + 1] == 1) {
			return 1;
		} else {
			return 0;
		}
	case South:
		if (streetMap[line + 1][col] == 1 || streetMap[line + 1][col - 1] == 1 || streetMap[line + 1][col + 1] == 1) {
			return 1;
		} else {
			return 0;
		}
	case East:
		if (streetMap[line][col + 1] == 1 || streetMap[line - 1][col + 1] == 1 || streetMap[line + 1][col + 1] == 1) {
			return 1;
		} else {
			return 0;
		}
		break;
	case West:
		if (streetMap[line][col - 1] == 1 || streetMap[line - 1][col - 1] == 1 || streetMap[line + 1][col - 1] == 1) {
			return 1;
		} else {
			return 0;
		}
	default:
		//Shouldn't get here anyway
		return 0;
	}
}

void City::makeStreet(int line, int col, Direction dir) {
	int dLine, dCol;
	switch (dir) {
	case North:
		dLine = -1;
		dCol = 0;
		break;
	case South:
		dLine = 1;
		dCol = 0;
		break;
	case East:
		dLine = 0;
		dCol = 1;
		break;
	case West:
		dLine = 0;
		dCol = -1;
		break;
	default:
		//ERROR
		break;
	}

	int length = rand() / (RAND_MAX / 16) + 10;
	// Stop when you meet another street or end of road or end of map
	for (int i = 0; i < length; i++) {
		line += dLine;
		col += dCol;
		if (!inMapArea(line, col)) {
			// Reached end of map. The End.
			return;
		}
		if (streetMap[line][col] != 0) {
			// Not an empty tile. The End.
			return;
		} else {
			streetMap[line][col] = 1;
			streets->addStreet(ROAD, meterUnitsScale, meterUnitsScale, col * meterUnitsScale, line * meterUnitsScale, (int)dir);
		}
	}

	line += dLine;
	col += dCol;

	// If we got here we got to the end of the street, meaning an intersection
	generateStreets(line, col, oppositeDirection(dir));
}

void City::addStreetTile(int line, int col, Direction dir) {
	streets->addStreet(CROSS, meterUnitsScale, meterUnitsScale, col * meterUnitsScale, line * meterUnitsScale, (int)dir);
}

int City::inMapArea(int line, int col) {
	if (line >= MODEL_WIDTH || line < 0 || col >= MODEL_WIDTH || col < 0) {
		return 0;
	}
	return 1;
}

Direction City::oppositeDirection(Direction dir) {
	switch (dir) {
	case South:
		return North;
	case North:
		return South;
	case West:
		return East;
	case East:
		return West;
	default:
		//Shouldn't get here
		return North;
	}
}

