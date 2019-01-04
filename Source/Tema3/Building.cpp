#include "Building.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Building::Building(int type, float height, float width,  float depth, float posX, float posZ) {
	mHeight = height;
	mWidth = width;
	mDepth = depth;
	mPosX = posX;
	mPosZ = posZ;

	switch (type) {
		case SIMPLE:
			createSimpleBuilding(height, width, depth, posX, posZ);
			break;
		case BLOCKY:
			createBlockyBuilding();
			break;
		default:
			break;
	}

}

Building::~Building() {
	
}

void Building::createSimpleBuilding(float height, float width, float depth, float posX, float posZ) {
	const string textureLoc = "Source/Tema3/Textures/";
	
	Texture2D* texture = new Texture2D();
	texture->Load2D((textureLoc + "img5.jpg").c_str(), GL_REPEAT);

	createBuildingSide(height, width, posX, posZ, 0);
	mapTextures.push_back(texture);
	createBuildingSide(height, depth, posX + width, posZ, 1);
	mapTextures.push_back(texture);
	createBuildingSide(height, width, posX + width, posZ + depth, 2);
	mapTextures.push_back(texture);
	createBuildingSide(height, depth, posX, posZ + depth, 3);
	mapTextures.push_back(texture);

	// Create roof:
	createRoof(height, width, depth, posX, posZ);
	texture = new Texture2D();
	texture->Load2D((textureLoc + "roof.jpg").c_str(), GL_REPEAT);
	mapTextures.push_back(texture);
}

void Building::createBlockyBuilding() {
	createSimpleBuilding(mHeight, mWidth * 0.5, mDepth * 0.5, mPosX + mWidth * 0.25f, mPosZ + mDepth * 0.25f);
	createSimpleBuilding(mHeight * 0.7, mWidth * 0.8, mDepth * 0.8, mPosX + mWidth * 0.1f, mPosZ + mDepth * 0.1f);
	createSimpleBuilding(mHeight * 0.4, mWidth, mDepth, mPosX, mPosZ);
}

void Building::createRoof(float height, float width, float depth, float x, float z) {
	vector<glm::vec3> vertices = {
			glm::vec3(x + width, height, z + depth),	// Top Right
			glm::vec3(x + width, height, z),	// Bottom Right
			glm::vec3(x, height, z),	// Bottom Left
			glm::vec3(x, height, z + depth),	// Top Left
	};

	vector<glm::vec3> normals
	{
		glm::vec3(0, 1, 1),
		glm::vec3(1, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0)
	};

	vector<glm::vec2> textureCoords
	{
		glm::vec2(width, 0.0f),
		glm::vec2(width, depth),
		glm::vec2(0.0f, depth),
		glm::vec2(0.0f, 0.0f)
	};

	vector<unsigned short> indices =
	{
		0, 1, 3,
		1, 2, 3
	};

	Mesh* mesh = new Mesh("buildingRoof");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);
}
// direction: 0: Ox    1: -Oz    2: -Ox    3: Oz  
void Building::createBuildingSide(float height, float width, float x, float z, int direction) {
	vector<glm::vec3> vertices;
	switch (direction) {
		case 0:
			vertices = {
			glm::vec3(x + width, height, z),	// Top Right
			glm::vec3(x + width, 0.0f, z),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			break;
		case 1:
			vertices = {
			glm::vec3(x, height, z + width),	// Top Right
			glm::vec3(x, 0.0f, z + width),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			break;
		case 2:
			vertices = {
			glm::vec3(x - width, height, z),	// Top Right
			glm::vec3(x - width, 0.0f, z),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			break;
		case 3:
			vertices = {
			glm::vec3(x, height, z - width),	// Top Right
			glm::vec3(x, 0.0f, z - width),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			break;
		default:
			// ERROR
			break;
	}
		
	vector<glm::vec3> normals
	{
		glm::vec3(0, 1, 1),
		glm::vec3(1, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0)
	};

	vector<glm::vec2> textureCoords
	{
		glm::vec2(width, 0.0f),
		glm::vec2(width, height),
		glm::vec2(0.0f, height),
		glm::vec2(0.0f, 0.0f)
	};

	vector<unsigned short> indices =
	{
		0, 1, 3,
		1, 2, 3
	};

	Mesh* mesh = new Mesh("buildingSide");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);
}

void Building::render(Shader *shader, EngineComponents::Camera *camera) {
	for (int i = 0; i < meshes.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		renderMesh(meshes[i], shader, modelMatrix, mapTextures[i], camera);
	}
}

void Building::renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_1"), 0);
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}