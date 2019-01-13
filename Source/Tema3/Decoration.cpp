#include "Decoration.h"
#include <iostream>
#include <Core/Engine.h>

using namespace std;

vector<Mesh*> Decoration::meshesTree;
vector<Texture2D*> Decoration::texturesTree;
Decoration::Decoration(int type, Texture2D *tex1, float depth, float width, float height, float x, float z) {
	// Setting parameters for the decoration object
	materialShininess = 60;
	materialKd = 0.4;
	materialKs = 0.3;
	cutoff_angle = 30;
	texture = tex1;

	// Creating mesh
	vector<glm::vec3> vertices {
		glm::vec3(x + width, depth, z + height),	// Top Right
		glm::vec3(x, depth, z + height),	// Bottom Right
		glm::vec3(x, depth, z),	// Bottom Left
		glm::vec3(x + width, depth, z),	// Top Left
	};

	vector<glm::vec3> normals {
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};

	vector<glm::vec2> textureCoords {
			glm::vec2(0.0f, height / 0.0625f),
			glm::vec2(width / 0.0625f, height / 0.0625f),
			glm::vec2(width / 0.0625f, 0.0f),
			glm::vec2(0.0f, 0.0f)
	};	

	vector<unsigned short> indices = {
		0, 1, 3,
		1, 2, 3
	};

	Mesh* mesh = new Mesh("ground");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);

	// If decoration is grass, then also add some trees
	if(type == GRASS) {
		// If the tree mesh is not added to the static member, add it
		if (Decoration::texturesTree.size() == 0) {
			Texture2D *treeStem = new Texture2D();
			treeStem->Load2D((RESOURCE_PATH::MODELS + "Tree/maple_bark.png").c_str(), GL_REPEAT);
			Decoration::texturesTree.push_back(treeStem);
			Texture2D *treeLeaf = new Texture2D();
			treeLeaf->Load2D((RESOURCE_PATH::MODELS + "Tree/maple_leaf.png").c_str(), GL_REPEAT);
			Decoration::texturesTree.push_back(treeLeaf);
		}

		// If the tree texture is not added to the static member, add it
		if (Decoration::meshesTree.size() == 0) {
			Mesh* treeStem = new Mesh("box");
			treeStem->LoadMesh(RESOURCE_PATH::MODELS + "Tree", "MapleTreeStem.obj");
			Decoration::meshesTree.push_back(treeStem);
			Mesh* treeLeaf = new Mesh("box");
			treeLeaf->LoadMesh(RESOURCE_PATH::MODELS + "Tree", "MapleTreeLeaves.obj");
			Decoration::meshesTree.push_back(treeLeaf);
		}

		// Depending on the area, add trees with some randomness to the position:
		for (float i = x + 0.03; i < x + width - 0.03f; i += 0.08f) {
			for (float j = z + 0.03; j < z + height - 0.03f; j += 0.08f) {
				float diffi = (float)rand() / ((float)RAND_MAX);
				diffi = diffi / 16.6f - 0.02;
				float diffj = (float)rand() / ((float)RAND_MAX);
				diffj = diffj / 16.6f - 0.02;

				float hight = (float)rand() / ((float)RAND_MAX);
				hight = hight / 1000 + 0.001;
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(i + diffi, 0.0f, j + diffj));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(hight));
				modelMatrixes.push_back(modelMatrix);
			}
		}
	}
}

Decoration::~Decoration() {
}

void Decoration::render(Shader *shader, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight) {
	// Render the mesh for base decoration
	glm::mat4 modelMatrix = glm::mat4(1);
	for (int i = 0; i < meshes.size(); i++) {
		renderMesh(meshes[i], shader, modelMatrix, texture, camera, lightPosition, typeOfLight);
	}

	// If we have trees, render them too
	for (auto modelM : modelMatrixes) {
		renderMesh(Decoration::meshesTree[0], shader, modelM, Decoration::texturesTree[0], camera, lightPosition, typeOfLight);
		renderMesh(Decoration::meshesTree[1], shader, modelM, Decoration::texturesTree[1], camera, lightPosition, typeOfLight);
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