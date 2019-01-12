#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

class Decoration {
public:
	Decoration(Texture2D *tex1, float depth, float width, float height, float x, float z, float textScale);
	~Decoration();
	void render(Shader *shader, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight);
	void addDecoration(int type, float width, float height, float posX, float posZ, int dir);
private:
	float mWidth, mHeight;
	float mPosX, mPosZ;
	void renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight);
	std::vector<Mesh*> meshes;
	Texture2D* texture;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	float cutoff_angle;
};