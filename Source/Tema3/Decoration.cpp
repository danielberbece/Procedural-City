#include "Decoration.h"
#include <iostream>
#include <Core/Engine.h>

using namespace std;

Decoration::Decoration(Texture2D *tex1, float depth, float width, float height, float x, float z, float textScale) {
	materialShininess = 60;
	materialKd = 0.4;
	materialKs = 0.3;
	cutoff_angle = 30;
	texture = tex1;
	vector<glm::vec3> vertices {
		glm::vec3(x + width, depth, z + height),	// Top Right
		glm::vec3(x, depth, z + height),	// Bottom Right
		glm::vec3(x, depth, z),	// Bottom Left
		glm::vec3(x + width, depth, z),	// Top Left
	};

	vector<glm::vec3> normals
	{
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};

	vector<glm::vec2> textureCoords;
	if (width < height) {
		textureCoords = {
			glm::vec2(2, 0.0f),
			glm::vec2(2, (height / width) * textScale),
			glm::vec2(0.0f, (height / width) * textScale),
			glm::vec2(0.0f, 0.0f)
		};
	} else {
		textureCoords = {
			glm::vec2((width / height) * textScale, 0.0f),
			glm::vec2((width / height) * textScale, 2),
			glm::vec2(0.0f, 2),
			glm::vec2(0.0f, 0.0f)
		};
	}

	textureCoords = {
			glm::vec2((width / textScale), 0.0f),
			glm::vec2((width / textScale), height / textScale),
			glm::vec2(0.0f, height / textScale),
			glm::vec2(0.0f, 0.0f)
	};
	
	cout << width << " " << height << endl;
	

	vector<unsigned short> indices =
	{
		0, 1, 3,
		1, 2, 3
	};

	Mesh* mesh = new Mesh("ground");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);
}

Decoration::~Decoration() {
}

void Decoration::addDecoration(int type, float width, float height, float x, float z, int dir) {
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


	vector<unsigned short> indices =
	{
		0, 1, 3,
		1, 2, 3
	};

	Mesh* mesh = new Mesh("street");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);
}

void Decoration::render(Shader *shader, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight) {
	glm::mat4 modelMatrix = glm::mat4(1);
	for (int i = 0; i < meshes.size(); i++) {
		renderMesh(meshes[i], shader, modelMatrix, texture, camera, lightPosition, typeOfLight);
	}
}

void Decoration::renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	glm::vec3 eyePosition = camera->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

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

	if (texture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_1"), 0);
	}

	int type = glGetUniformLocation(shader->program, "t");
	glUniform1i(type, typeOfLight);

	int cut_off_angle = glGetUniformLocation(shader->program, "cut_off_angle");
	glUniform1f(cut_off_angle, cutoff_angle);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}