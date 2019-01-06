#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "Building.h"

// In meters:
#define CITY_WIDTH MODEL_WIDTH * STREET_WIDTH
#define MODEL_WIDTH 128
#define STREET_WIDTH 2
enum Direction { North, South, East, West };

class City {
public:
	City();
	~City();
	void renderCity(Shader *shader, EngineComponents::Camera *camera);
private:
	void generateStreets(int line, int col, Direction fromDir);
	void makeStreet(int startLine, int startCol, Direction dir);
	void addStreetTile(int line, int col);
	void addStreetsToMap();
	int inMapArea(int line, int col);
	std::unordered_map<std::string, Texture2D*> mapTextures;
	std::vector<Building *> buildings;
	int streetMap[MODEL_WIDTH][MODEL_WIDTH] = { 0 };
	int cityMap[CITY_WIDTH][CITY_WIDTH] = { 0 };
	float meterUnitsScale = 1.0f / 16;
	float unitsMeterScale = 16;
};