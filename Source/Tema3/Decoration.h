#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#define GRASS 0
#define PAVEMENT 1
#define PARKING 2

class Decoration {
public:
	Decoration(int type, Texture2D *tex1, float depth, float width, float height, float x, float z);
	~Decoration();
	void render(Shader *shader, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight);
private:
	float mWidth, mHeight;
	float mPosX, mPosZ;
	void renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight);
	std::vector<Mesh*> meshes;
	static std::vector<Mesh*> meshesTree;
	static std::vector<Texture2D*> texturesTree;
	std::vector<int> textureTreeIndexs;
	std::vector<int> meshesTreeIndexs;
	std::vector<glm::mat4> modelMatrixes;

	Texture2D* texture;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	float cutoff_angle;
};