#include "City.h"
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <ctime>
#include <Core/Engine.h>

#define DAY 0
#define NIGHT 1

using namespace std;

City::City() {
	// Seed at the beggining of time :)
	std::srand(std::time(NULL));
	loadTextures();
	// Set the light position for the sun:
	lightPosition = glm::vec3(0, 3, 0);
	
	// Initialize streets member with the proper textures
	streets = new Streets(mapTextures["road"], mapTextures["crossroad"]);

	// Prepare street map for generation of streets
	limitStreetMap();

	// Create the streets, procedurally
	int rootLine = rand() / (RAND_MAX / (MODEL_WIDTH - 1) ) + 1;
	int rootCol = rand() / (RAND_MAX / (MODEL_WIDTH - 1) ) + 1;
	generateStreets(rootLine, rootCol, North);

	// Get building areas and make the buildings
	getBuildingAreas();
}

City::~City() {
}

void City::setMode(int mode) {
	typeOfLight = mode;
}

void City::loadTextures() {
	// Add .jpg textures
	const string textureLoc = "Source/Tema3/Textures/";
	vector<string> textNames = {"walls1", "roof1", "road", "crossroad", "soil", "grass", "walk"};
	Texture2D* texture;
	for (int i = 0; i < textNames.size(); i++) {
		texture = new Texture2D();
		texture->Load2D((textureLoc + textNames[i] + ".jpg").c_str(), GL_REPEAT);
		mapTextures[textNames[i]] = texture;
	}

	// Add texture for parking lots, using GL_MIRRORED_REPEAT
	texture = new Texture2D();
	texture->Load2D((textureLoc + "parking.jpg").c_str(), GL_MIRRORED_REPEAT);
	mapTextures["parking"] = texture;

	// Add .png texture
	texture = new Texture2D();
	texture->Load2D((textureLoc + "walls2.png").c_str(), GL_REPEAT);
	mapTextures["walls2"] = texture;
}

void City::limitStreetMap() {
	for (int i = 0; i < MODEL_WIDTH; i++) {
		streetMap[0][i] = -1;
		streetMap[MODEL_WIDTH - 1][i] = -1;
		streetMap[i][0] = -1;
		streetMap[i][MODEL_WIDTH - 1] = -1;
	}
}

void City::getBuildingAreas() {
	vector<int> areaLimits;
	for (int i = 0; i < MODEL_WIDTH; i++) {
		for (int j = 0; j < MODEL_WIDTH; j++) {
			if (streetMap[i][j] == 0) {
				areaLimits = fillArea(i, j);
				if (areaLimits[2] - areaLimits[0] > 2 && areaLimits[3] - areaLimits[1] > 2) {
					// Make buildings
					putBuildingsInArea(areaLimits);
				} else {
					// Make green space or parking lot
					float decorationType = rand() / (RAND_MAX / 2.0f);
					Decoration* d;
					if (decorationType < 1) {	// Make green space
						d = new Decoration(GRASS, mapTextures["grass"], 0, (areaLimits[2] - areaLimits[0] + 1) * meterUnitsScale, (areaLimits[3] - areaLimits[1] + 1) * meterUnitsScale,
										   areaLimits[0] * meterUnitsScale, areaLimits[1] * meterUnitsScale);
					} else {	// Make parking lot
						d = new Decoration(PARKING, mapTextures["parking"], 0, (areaLimits[2] - areaLimits[0] + 1) * meterUnitsScale, (areaLimits[3] - areaLimits[1] + 1) * meterUnitsScale,
										   areaLimits[0] * meterUnitsScale, areaLimits[1] * meterUnitsScale);
					}
					decorations.push_back(d);
				}
			}
		}
	}
}

void City::putBuildingsInArea(std::vector<int> limits) {

	// Fill the area with multiple buildings:
	float startX = (limits[0] + 0.5f) * meterUnitsScale;
	float startY = (limits[1] + 0.5f) * meterUnitsScale;
	if (limits[2] - limits[0] > limits[3] - limits[1]) {
		// If the side on X axis is longer:
		for (; startX < limits[2] * meterUnitsScale; startX += 0.5f * meterUnitsScale) {
			if (limits[2] * meterUnitsScale - startX < 3.0f * meterUnitsScale) {
				if (limits[2] * meterUnitsScale - startX < 0.5f * meterUnitsScale) {
					// Not enough space for a real building
					continue;
				}
				// make building the whole remaining area
				int height = rand() / (RAND_MAX / 4) + 2;
				int type = rand() / (RAND_MAX / 3) + 1;
				buildings.push_back(new Building(type, height * meterUnitsScale, (limits[2] + 0.5f) * meterUnitsScale - startX, (limits[3] - limits[1]) * meterUnitsScale,
					startX, startY, mapTextures["walls1"], mapTextures["walls2"], mapTextures["roof1"]));
				startX = limits[2];
			} else {
				float width =(2 + rand() / (RAND_MAX / 1.0f)) * meterUnitsScale;
				int height = rand() / (RAND_MAX / 4) + 2;
				int type = rand() / (RAND_MAX / 3) + 1;
				buildings.push_back(new Building(type, height * meterUnitsScale, width, (limits[3] - limits[1]) * meterUnitsScale,
					startX, startY, mapTextures["walls1"], mapTextures["walls2"], mapTextures["roof1"]));
				startX += width;
			}
		}
	} else {
		// If the side on Y axis is longer:
		for (; startY < limits[3] * meterUnitsScale; startY += 0.5f * meterUnitsScale) {
			if (limits[3] * meterUnitsScale - startY < 3.0f * meterUnitsScale) {
				if (limits[3] * meterUnitsScale - startY < 0.5f * meterUnitsScale) {
					// Not enough space for a real building
					continue;
				}
				// make building the whole remaining area
				int height = rand() / (RAND_MAX / 4) + 2;
				int type = rand() / (RAND_MAX / 3) + 1;
				buildings.push_back(new Building(type, height * meterUnitsScale, (limits[2] - limits[0]) * meterUnitsScale, (limits[3] + 0.5f) * meterUnitsScale - startY,
												 startX, startY, mapTextures["walls1"], mapTextures["walls2"], mapTextures["roof1"]));
				startY = limits[3];
			} else {
				float width = (2 + rand() / (RAND_MAX / 1.0f)) * meterUnitsScale;
				int height = rand() / (RAND_MAX / 4) + 2;
				int type = rand() / (RAND_MAX / 3) + 1;
				buildings.push_back(new Building(type, height * meterUnitsScale, (limits[2] - limits[0]) * meterUnitsScale, width,
												 startX, startY, mapTextures["walls1"], mapTextures["walls2"], mapTextures["roof1"]));
				startY += width;
			}
		}
	}

	// Add some pavement so that the buildings don't float in dark space
	decorations.push_back(new Decoration(PAVEMENT, mapTextures["walk"], 0, (limits[2] - limits[0] + 1) * meterUnitsScale, 
		(limits[3] - limits[1] + 1) * meterUnitsScale,limits[0] * meterUnitsScale, limits[1] * meterUnitsScale));
}


vector<int> City::fillArea(int line, int col) {
	// A Breath-First approach for getting the building areas

	queue<pair<int, int> > Q;
	Q.push(make_pair(line, col));
	streetMap[line][col] = 2;
	// minline, mincol, maxline, maxcol
	vector<int> margins = { line, col, line, col };
	while (!Q.empty()) {
		pair<int, int> point = Q.front();
		Q.pop();
		if (point.first < margins[0]) {
			margins[0] = point.first;
		}
		if (point.second < margins[1]) {
			margins[1] = point.second;
		}
		if (point.first > margins[2]) {
			margins[2] = point.first;
		}
		if (point.second > margins[3]) {
			margins[3] = point.second;
		}
		// Go in each direction (N,S,W,E)
		if (streetMap[point.first - 1][point.second] == 0) {
			Q.push(make_pair(point.first - 1, point.second));
			streetMap[point.first - 1][point.second] = 2;
		}
		if (streetMap[point.first + 1][point.second] == 0) {
			Q.push(make_pair(point.first + 1, point.second));
			streetMap[point.first + 1][point.second] = 2;
		}
		if (streetMap[point.first][point.second - 1] == 0) {
			Q.push(make_pair(point.first, point.second - 1));
			streetMap[point.first][point.second - 1] = 2;
		}
		if (streetMap[point.first][point.second + 1] == 0) {
			Q.push(make_pair(point.first, point.second + 1));
			streetMap[point.first][point.second + 1] = 2;
		}
	}
	return margins;
}

void City::renderCity(Shader *shader, EngineComponents::Camera *camera) {
	// Render buildings
	for (auto building : buildings) {
		building->render(shader, camera, lightPosition, typeOfLight);
	}

	// Render decorations (no-building areas)
	for (auto d : decorations) {
		d->render(shader, camera, lightPosition, typeOfLight);
	}

	// Render streets
	streets->render(shader, camera, lightPosition, typeOfLight);
}

void City::generateStreets(int line, int col, Direction fromDir) {
	// Procedural generation of street map

	// Current is an intersection, treat it as so
	if (streetMap[line][col] == 0) {
		streetMap[line][col] = 1;
		// Check if ahead is a street. If it is, stop
		if (isNextOccupied(line, col, oppositeDirection(fromDir))) {
			streets->addStreet(ROAD, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)fromDir);
		} else {
			streets->addStreet(CROSS, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)fromDir);
			// Go in 3 directions, because one is already made (the one we came from)
			for (int i = North; i <= West; i++) {
				// Go in this dir if it is not towards another street, and is not the one we came from
				if (i != (int)fromDir && !isNextOccupied(line, col, (Direction)i)) {	
					makeStreet(line, col, (Direction)i);
				}
			}
		}
	}
}

int City::isNextOccupied(int line, int col, Direction dir) {
	// Check if in the next tiles there is already a street, in order to signal a stop
	switch (dir) {
	case North:
		if (streetMap[line-1][col] == 1 || streetMap[line-1][col-1] == 1 || streetMap[line-1][col+1] == 1) {
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
		//Shouldn't get here anyway
		break;
	}

	int length = rand() / (RAND_MAX / 10.0f) + 4.3f;
	// Stop when you meet another street or end of road or end of map
	for (int i = 0; i < length; i++) {
		line += dLine;
		col += dCol;
		if (streetMap[line][col] != 0) {
			// Not an empty tile. Met another street or met the end of map. Exit.
			return;
		} else {
			streetMap[line][col] = 1;
			streets->addStreet(ROAD, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)dir);
		}
	}

	line += dLine;
	col += dCol;

	// If we got here we got to the end of the street, meaning an intersection
	generateStreets(line, col, oppositeDirection(dir));
}

Direction City::oppositeDirection(Direction dir) {
	// Get the opposite direction
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

