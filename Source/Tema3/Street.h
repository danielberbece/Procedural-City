#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#define CROSS 0
#define ROAD 1

class Streets {
public:
	Streets(Texture2D *tex1, Texture2D *tex2);
	~Streets();
	void render(Shader *shader, EngineComponents::Camera *camera);
	void addStreet(int type, float width, float height, float posX, float posZ, int dir);
private:
	float mWidth, mHeight;
	float mPosX, mPosZ;
	void renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera);
	std::vector<Mesh*> meshes;
	std::vector<int> texturesId;
	std::vector<Texture2D*> textures;
	Texture2D* streetTexture;
	Texture2D* crossTexture;
};