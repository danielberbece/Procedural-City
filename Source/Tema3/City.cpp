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
	std::srand(std::time(NULL));
	loadTextures();
	lightPosition = glm::vec3(0, 3, 0);
	
	streets = new Streets(mapTextures["road"], mapTextures["crossroad"]);


	int rootLine = rand() / (RAND_MAX / (MODEL_WIDTH - 1) ) + 1;
	int rootCol = rand() / (RAND_MAX / (MODEL_WIDTH - 1) ) + 1;
	boardStreetMap();
	generateStreets(rootLine, rootCol, North);
	getBuildingAreas();
}

City::~City() {
}

void City::setMode(int mode) {
	typeOfLight = mode;
}

void City::loadTextures() {
	const string textureLoc = "Source/Tema3/Textures/";
	vector<string> textNames = {"walls1", "roof1", "road", "crossroad", "soil", "grass", "walk" };
	Texture2D* texture;
	for (int i = 0; i < textNames.size(); i++) {
		texture = new Texture2D();
		texture->Load2D((textureLoc + textNames[i] + ".jpg").c_str(), GL_REPEAT);
		mapTextures[textNames[i]] = texture;
	}

	textNames = {};
	for (int i = 0; i < textNames.size(); i++) {
		texture = new Texture2D();
		texture->Load2D((textureLoc + textNames[i] + ".png").c_str(), GL_REPEAT);
		mapTextures[textNames[i]] = texture;
	}
}

void City::boardStreetMap() {
	for (int i = 0; i < MODEL_WIDTH; i++) {
		streetMap[0][i] = -1;
		streetMap[MODEL_WIDTH - 1][i] = -1;
		streetMap[i][0] = -1;
		streetMap[i][MODEL_WIDTH - 1] = -1;
	}
}

void City::getBuildingAreas() {
	int totalAreas = 0;
	vector<int> areaLimits;
	for (int i = 0; i < MODEL_WIDTH; i++) {
		for (int j = 0; j < MODEL_WIDTH; j++) {
			if (streetMap[i][j] == 0) {
				areaLimits = fillArea(i, j);
				if (areaLimits[2] - areaLimits[0] > 2 && areaLimits[3] - areaLimits[1] > 2) {
					int height = rand() / (RAND_MAX / 3) + 2;
					int type = rand() / (RAND_MAX / 3) + 1;
					buildings.push_back(new Building(type, height * meterUnitsScale, (areaLimits[2] - areaLimits[0]) * meterUnitsScale, (areaLimits[3] - areaLimits[1]) * meterUnitsScale,
													 (areaLimits[0] + 0.5) * meterUnitsScale, (areaLimits[1] + 0.5) * meterUnitsScale, mapTextures["walls1"], mapTextures["roof1"]));
					Decoration* d = new Decoration(mapTextures["walk"], 0, (areaLimits[2] - areaLimits[0] + 1) * meterUnitsScale, (areaLimits[3] - areaLimits[1] + 1) * meterUnitsScale,
												   areaLimits[0] * meterUnitsScale, areaLimits[1] * meterUnitsScale, 0.6f);
					decorations.push_back(d);
					totalAreas += 1;
				} else {
					// TODO: Make green space or walk area
					Decoration* d = new Decoration(mapTextures["grass"], -0.0001f, (areaLimits[2] - areaLimits[0] + 1) * meterUnitsScale, (areaLimits[3] - areaLimits[1] + 1) * meterUnitsScale,
												   areaLimits[0] * meterUnitsScale, areaLimits[1] * meterUnitsScale, 0.1f);
					decorations.push_back(d);
				}
			}
		}
	}
}

vector<int> City::fillArea(int line, int col) {
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
		// Go in each direction
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
	for (auto building : buildings) {
		building->render(shader, camera, lightPosition, typeOfLight);
	}

	for (auto d : decorations) {
		d->render(shader, camera, lightPosition, typeOfLight);
	}

	streets->render(shader, camera, lightPosition, typeOfLight);
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
			streets->addStreet(ROAD, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)fromDir);
		} else {
			streets->addStreet(CROSS, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)fromDir);
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
		if (!inMapArea(line, col)) {
			// Reached end of map. The End.
			return;
		}
		// TODO: Check on sides. If there is a street, put a cross and stop
		//
		if (streetMap[line][col] != 0) {
			// Not an empty tile. The End.
			//streets->addStreet(CROSS, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)dir);
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

void City::addStreetTile(int line, int col, Direction dir) {
	streets->addStreet(CROSS, meterUnitsScale, meterUnitsScale, line * meterUnitsScale, col * meterUnitsScale, (int)dir);
}

int City::inMapArea(int line, int col) {
	if (line >= MODEL_WIDTH || line < 0 || col >= MODEL_WIDTH || col < 0) {
		cout << "ERROR: Checked and not in map area" << endl;
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

