#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#define SIMPLE 1
#define BLOCKY 2
#define TOWER 3

class Building {
public:
	Building(int type, float height, float width, float depth, float posX, float posZ, Texture2D *walls, Texture2D *roof);
	~Building();
	void render(Shader *shader, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight);
private:
	float mHeight, mWidth, mDepth;
	float mPosX, mPosZ;
	int type;
	enum SimpleShape {Square, Circle, Hexagon};
	SimpleShape shape;
	void createSimpleBuilding(SimpleShape shape, float height, float width, float depth, float posX, float posZ);
	void createBlockyBuilding(SimpleShape shape);
	void createTowerBuilding(float height, float width, float depth, float posX, float posZ);
	void createRoof(float height, float width, float depth, float posX, float posZ);
	void createBuildingSide(float height, float width, float x, float y, int direction);
	void createCylinder(int nFlatWalls, float height, float radius, float posX, float posZ);
	void renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera, glm::vec3 lightPosition, glm::vec3 lightDirection, int typeOfLight);
	std::vector<Mesh*> meshes;
	std::vector<Texture2D*> mapTextures;
	std::vector<glm::vec3> lightPoints;
	Texture2D* renderTexture;
	Texture2D* textureWalls;
	Texture2D* textureRoof;
	float cutoff_angle = 40;

	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	float lightWallDist = 0.01f;

};