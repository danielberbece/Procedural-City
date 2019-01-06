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
	void render(Shader *shader, EngineComponents::Camera *camera);
private:
	float mHeight, mWidth, mDepth;
	float mPosX, mPosZ;
	enum SimpleShape {Square, Circle, Hexagon};
	void createSimpleBuilding(SimpleShape shape, float height, float width, float depth, float posX, float posZ);
	void createBlockyBuilding(SimpleShape shape);
	void createTowerBuilding(float height, float width, float depth, float posX, float posZ);
	void createRoof(float height, float width, float depth, float posX, float posZ);
	void createBuildingSide(float height, float width, float x, float y, int direction);
	void createCylinder(int nFlatWalls, float height, float radius, float posX, float posZ);
	void renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera);
	std::vector<Mesh*> meshes;
	std::vector<Texture2D*> mapTextures;
	Texture2D* renderTexture;
	Texture2D* textureWalls;
	Texture2D* textureRoof;

};