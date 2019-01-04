#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include <vector>

#define SIMPLE 1
#define BLOCKY 2

class Building {
public:
	Building(int type, float height, float width, float depth, float posX, float posZ);
	~Building();
	void render(Shader *shader, EngineComponents::Camera *camera);
private:
	float mHeight, mWidth, mDepth;
	float mPosX, mPosZ;
	void createSimpleBuilding(float height, float width, float depth, float posX, float posZ);
	void createBlockyBuilding();
	void createRoof(float height, float width, float depth, float posX, float posZ);
	void createBuildingSide(float height, float width, float x, float y, int direction);
	void renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera);
	std::vector<Mesh*> meshes;
	std::vector<Texture2D*> mapTextures;
};