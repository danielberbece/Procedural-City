#include "Street.h"
#include <iostream>
#include <Core/Engine.h>

using namespace std;

Streets::Streets(Texture2D *tex1, Texture2D *tex2) {
	streetTexture = tex1;
	crossTexture = tex2;
	textures.push_back(crossTexture);
	textures.push_back(streetTexture);
}

Streets::~Streets() {
}

void Streets::addStreet(int type, float width, float height, float x, float z, int dir) {
	vector<glm::vec3> vertices = {
			glm::vec3(x + width, 0, z + height),	// Top Right
			glm::vec3(x, 0, z + height),	// Bottom Right
			glm::vec3(x, 0, z),	// Bottom Left
			glm::vec3(x + width, 0, z),	// Top Left
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
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 0)
	};
	if (dir == 2 || dir == 3) {
		textureCoords =	{
			glm::vec2(1, 1),
			glm::vec2(0, 1),
			glm::vec2(0, 0),
			glm::vec2(1, 0)
		};
	}

	vector<unsigned short> indices =
	{
		0, 1, 3,
		1, 2, 3
	};

	Mesh* mesh = new Mesh("street");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);
	if (type == CROSS) {
		texturesId.push_back(0);
	} else {
		texturesId.push_back(1);
	}
}

void Streets::render(Shader *shader, EngineComponents::Camera *camera) {
	glm::mat4 modelMatrix = glm::mat4(1);
	for (int i = 0; i < meshes.size(); i++) {
		renderMesh(meshes[i], shader, modelMatrix, textures[texturesId[i]], camera);
	}
}

void Streets::renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture, EngineComponents::Camera *camera) {
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