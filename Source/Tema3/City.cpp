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

	std::srand(std::time(NULL));
	int rootLine = 3; //rand() / (RAND_MAX / MODEL_WIDTH);
	int rootCol = 3;  //rand() / (RAND_MAX / MODEL_WIDTH);
	generateStreets(rootLine, rootCol, North);
	addStreetsToMap();

	Building *b;
	for (int i = 0; i < CITY_WIDTH; i++) {
		for (int j = 0; j < CITY_WIDTH; j++) {
			if (cityMap[i][j] == 1) {
				b = new Building(TOWER, 0.3, meterUnitsScale, meterUnitsScale, i * meterUnitsScale, j * meterUnitsScale, mapTextures["walls1"], mapTextures["roof1"]);
				buildings.push_back(b);
			}
		}
	}
}

City::~City() {
}

void City::addStreetsToMap() {
	for (int i = 0; i < MODEL_WIDTH; i++) {
		for (int j = 0; j < MODEL_WIDTH; j++) {
			if (streetMap[i][j] == 1) {
				addStreetTile(i, j);
			}
		}
	}
}

void City::renderCity(Shader *shader, EngineComponents::Camera *camera) {
	for (auto building : buildings) {
		building->render(shader, camera);
	}
}

void City::generateStreets(int line, int col, Direction fromDir) {
	if (!inMapArea(line, col)) {
		// Reached end of map. The End.
		return;
	}

	// Current at the intersection:
	if (streetMap[line][col] == 0) {
		streetMap[line][col] = 1;
		// go in 3 directions 
		for (int i = North; i <= West; i++) {
			if (i != fromDir) {	// Go in this dir
				// How much?
				makeStreet(line, col, (Direction)i);
			}
		}
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

	int length = rand() / (RAND_MAX / 20) + 10;
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
		}
	}

	line += dLine;
	col += dCol;

	// If we got here we got to the end of the street, meaning an intersection
	generateStreets(line, col, dir);
}

void City::addStreetTile(int line, int col) {
	for (int i = line * STREET_WIDTH; i < (line + 1) * STREET_WIDTH; i++) {
		for (int j = col * STREET_WIDTH; j < (col + 1) * STREET_WIDTH; j++) {
			cityMap[i][j] = 1;
		}
	}
}

int City::inMapArea(int line, int col) {
	if (line >= MODEL_WIDTH || line < 0 || col >= MODEL_WIDTH || col < 0) {
		return 0;
	}
	return 1;
}
