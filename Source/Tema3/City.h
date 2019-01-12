#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "Building.h"
#include "Street.h"
#include "Decoration.h"
#include <vector>

// In meters:
#define MODEL_WIDTH 128
//128
//#define STREET_WIDTH 8
//#define CITY_WIDTH MODEL_WIDTH * STREET_WIDTH

enum Direction { North, South, East, West };

class City {
public:
	City();
	~City();
	void renderCity(Shader *shader, EngineComponents::Camera *camera);
	void setMode(int mode);
private:
	void getBuildingAreas();
	void boardStreetMap();
	void loadTextures();
	std::vector<int> fillArea(int line, int col);
	int isNextOccupied(int line, int col, Direction dir);
	Direction oppositeDirection(Direction dir);
	void generateStreets(int line, int col, Direction fromDir);
	void makeStreet(int startLine, int startCol, Direction dir);
	void addStreetTile(int line, int col, Direction dir);
	int inMapArea(int line, int col);
	std::unordered_map<std::string, Texture2D*> mapTextures;
	std::vector<Building*> buildings;
	std::vector<Decoration*> decorations;
	int streetMap[MODEL_WIDTH][MODEL_WIDTH] = { 0 };
	float meterUnitsScale = 1.0f / 16;
	float unitsMeterScale = 16;
	Streets *streets;
	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	int typeOfLight = 0;

	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	
};