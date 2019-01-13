#include "Building.h"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>

#include <Core/Engine.h>

using namespace std;

Building::Building(int t, float height, float width,  float depth, float posX, float posZ, Texture2D *walls1, Texture2D *walls2, Texture2D *roof) {
	// Set the building params
	mHeight = height;
	mWidth = width;
	mDepth = depth;
	mPosX = posX;
	mPosZ = posZ;

	// Set texture for walls
	textureWalls = walls1;
	textureRoof = roof;
	type = t;

	// Randomly pick the building shape
	float typeRand = ((float)std::rand()) / ((float)RAND_MAX / 2);
	if (typeRand < 1) {
		shape = Circle;
	} else {
		shape = Square;
	}

	// Specify the building lighting coefficients
	materialShininess = 20;
	materialKd = 0.5;
	materialKs = 0.4;

	// Create building, depending on type:
	switch (type) {
		case SIMPLE:
			if (shape == Square) {
				textureWalls = walls2;
			}
			createSimpleBuilding(shape, height, width, depth, posX, posZ);
			break;
		case BLOCKY:
			createBlockyBuilding(shape);
			break;
		case TOWER:
			createTowerBuilding(height, width, depth, posX, posZ);
			break;
		default:
			// ERROR, no such building
			break;
	}
	
}

Building::~Building() {
}

void Building::createSimpleBuilding(SimpleShape shape, float height, float width, float depth, float posX, float posZ) {
	float radius;
	switch (shape) {
		case Square:
			// Create the walls:
			createBuildingSide(height, width, posX, posZ, 0);
			createBuildingSide(height, depth, posX + width, posZ, 1);
			createBuildingSide(height, width, posX + width, posZ + depth, 2);
			createBuildingSide(height, depth, posX, posZ + depth, 3);

			// Create roof:
			createRoof(height, width, depth, posX, posZ);
			break;
		case Circle:
			// The radius of the building will be the smallest side
			if (depth < width) {
				radius = depth;
			} else {
				radius = width;
			}
			createCylinder(0, height, radius / 2, posX, posZ);
			break;
		case Hexagon:
			// TODO: createHexBuilding();
			break;
		default:
			// ERROR
			break;
	}
}

void Building::createBlockyBuilding(SimpleShape shape) {
	std::rand();
	int numberBlocks = ((std::rand()) / (RAND_MAX / 4)) + 1;
	float totalBlocks = numberBlocks;
	float lastScale = 1.0f;
	float scale;
	float heightScale;
	float lastHeightScale = 0.1f;
	int first = 1;
	for (int i = 1; i <= numberBlocks; i++) {
		scale = 1.0f;
		while (first != 1 && scale >= lastScale || scale < lastScale / 4 ) {
			scale = ((float) std::rand()) / RAND_MAX;
		}
		first = 0;
		heightScale = 0.0f;
		while (heightScale <= lastHeightScale || heightScale > ((float) i / numberBlocks)) {
			heightScale = ((float)std::rand()) / RAND_MAX;
		}
		switch (shape) {
			case Square:
				createSimpleBuilding(shape, mHeight * heightScale, mWidth * scale, mDepth * scale, mPosX + mWidth * ((1 - scale) / 2), mPosZ + mDepth * ((1 - scale) / 2));
				break;
			case Circle:
				if (mWidth * scale < mDepth * scale) {
					createSimpleBuilding(shape, mHeight * heightScale, mWidth * scale, mWidth * scale, mPosX + mWidth * ((1 - scale) / 2), mPosZ + mWidth * ((1 - scale) / 2));
					break;
				} else {
					createSimpleBuilding(shape, mHeight * heightScale, mDepth * scale, mDepth * scale, mPosX + mDepth * ((1 - scale) / 2), mPosZ + mDepth * ((1 - scale) / 2));
					break;
				}
				break;
			case Hexagon:
				// TODO
				break;
			default:
				break;
		}
		lastScale = scale;
		lastHeightScale = heightScale;
	}
}

void Building::createTowerBuilding(float height, float width, float depth, float posX, float posZ) {
	float radius;
	if (depth < width) {
		radius = depth;
	} else {
		radius = width;
	}
	createCylinder(std::rand() / (RAND_MAX / 3), height, radius / 2, posX, posZ);
}

void Building::createCylinder(int nFlatWalls, float height, float radius, float posX, float posZ) {
	glm::vec3 center = glm::vec3(posX + radius, 0.0f, posZ + radius);
	int slices = 40;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> normals;
	glm::vec3 beforePoint = center + glm::vec3(radius, 0, 0);
	vector<glm::vec2> textureCoords;
	glm::vec2 beforeTextCoord = glm::vec2(0.0f, 0.0f);
	glm::vec3 beforeNorm = glm::vec3(1.0f, 0.0f, 0.0f);

	// Add some flat walls to the "cylinder" building
	vector<float> anglesToSkip;
	int anglesIndex = 0;
	int angles = nFlatWalls;
	if (angles == 4) { angles--; }

	// Set the angles at which the flat walls will be
	for (int i = 0; i < angles; i++) {
		float angleToSkip = (((float)std::rand()) / (float)RAND_MAX) * AI_MATH_HALF_PI;
		angleToSkip += AI_MATH_HALF_PI * i * 0.9f;
		anglesToSkip.push_back(angleToSkip);
	}

	// Add spotlight to light the buildings at night
	float angleForLight = rand() / (RAND_MAX / AI_MATH_HALF_PI);

	// Add the vertices, normals,... for the cylinder mesh
	for (float i = AI_MATH_TWO_PI / ((float) slices), j = 0; i <= AI_MATH_TWO_PI + 0.01f; i += AI_MATH_TWO_PI / ((float) slices), j += 4) {
		if (anglesIndex < anglesToSkip.size()) {
			if ( i > anglesToSkip[anglesIndex] && i < anglesToSkip[anglesIndex] + AI_MATH_HALF_PI) {
				i += AI_MATH_HALF_PI;
				anglesIndex += 1;
			}
		}
		if (angleForLight < i) {
			angleForLight += AI_MATH_HALF_PI;
			lightPoints.push_back(center + glm::vec3(cos(i) * (radius + lightWallDist), 0, sin(i) * (radius + lightWallDist)));
		}
		vertices.push_back(center + glm::vec3(glm::cos(i) * radius, height, glm::sin(i) * radius));
		vertices.push_back(center + glm::vec3(glm::cos(i) * radius, 0, glm::sin(i) * radius));
		vertices.push_back(beforePoint);
		vertices.push_back(beforePoint + glm::vec3(0, height, 0));
		
		normals.push_back(glm::vec3(glm::cos(i), 0, glm::sin(i)));
		normals.push_back(glm::vec3(glm::cos(i), 0, glm::sin(i)));
		normals.push_back(beforeNorm);
		normals.push_back(beforeNorm);
	
		int aux[] = { j, j + 1, j + 3, j + 1, j + 2, j + 3 };
		indices.insert(indices.end(), aux, aux + 6);

		float scale = 0.1f * slices / AI_MATH_TWO_PI;
		textureCoords.push_back(glm::vec2(i * scale , 0.0f));
		textureCoords.push_back(glm::vec2(i * scale , height * 5));
		textureCoords.push_back(beforeTextCoord + glm::vec2(0.0f, height * 5));
		textureCoords.push_back(beforeTextCoord);

		beforePoint = center + glm::vec3(glm::cos(i) * radius, 0, glm::sin(i) * radius);
		beforeTextCoord = glm::vec2(i * scale, 0.0f);
		beforeNorm = glm::vec3(glm::cos(i), 0, glm::sin(i));
	}

	Mesh* mesh = new Mesh("towerBuilding");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	meshes.push_back(mesh);
	mapTextures.push_back(textureWalls);
	
	std::vector<glm::vec3> vertices1;
	std::vector<unsigned short> indices1;
	std::vector<glm::vec3> normals1;
	beforePoint = center + glm::vec3(radius, height, 0);
	vector<glm::vec2> textureCoords1;
	beforeTextCoord = glm::vec2(1.0f, 0.0f);
	anglesIndex = 0;

	// Add the roof mesh
	for (float i = AI_MATH_TWO_PI / (float)slices, j = 0; i <= AI_MATH_TWO_PI + 0.01f; i += AI_MATH_TWO_PI / (float)slices, j += 3) {
		if (anglesIndex < anglesToSkip.size()) {
			if (i > anglesToSkip[anglesIndex] && i < anglesToSkip[anglesIndex] + AI_MATH_HALF_PI) {
				i += AI_MATH_HALF_PI;
				anglesIndex += 1;
			}
		}
		vertices1.push_back(center + glm::vec3(0, height, 0));
		vertices1.push_back(beforePoint);
		vertices1.push_back(center + glm::vec3(glm::cos(i) * radius, height, glm::sin(i) * radius));

		normals1.push_back(glm::vec3(0, 1, 0));
		normals1.push_back(glm::vec3(0, 1, 0));
		normals1.push_back(glm::vec3(0, 1, 0));

		int aux[] = { j, j + 1, j + 2};
		indices1.insert(indices1.end(), aux, aux + 3);

		float scaleX = (glm::cos(i) + 1) / 2.0f;
		float scaleY = (glm::sin(i) + 1) / 2.0f;
		textureCoords1.push_back(glm::vec2(0.5, 0.5));
		textureCoords1.push_back(beforeTextCoord);
		textureCoords1.push_back(glm::vec2(scaleX, scaleY));

		beforePoint = center + glm::vec3(glm::cos(i) * radius, height, glm::sin(i) * radius);
		beforeTextCoord = glm::vec2(scaleX, scaleY);
	}

	mesh = new Mesh("towerBuildingTop");
	mesh->InitFromData(vertices1, normals1, textureCoords1, indices1);
	meshes.push_back(mesh);
	mapTextures.push_back(textureRoof);
}

// Create Square roof
void Building::createRoof(float height, float width, float depth, float x, float z) {
	vector<glm::vec3> vertices = {
		glm::vec3(x + width, height, z + depth),	// Top Right
		glm::vec3(x + width, height, z),	// Bottom Right
		glm::vec3(x, height, z),	// Bottom Left
		glm::vec3(x, height, z + depth),	// Top Left
	};

	vector<glm::vec3> normals
	{
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};

	vector<glm::vec2> textureCoords
	{
		glm::vec2(width * 4, 0.0f),
		glm::vec2(width * 4, depth * 4),
		glm::vec2(0.0f, depth * 4),
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

	mapTextures.push_back(textureRoof);
}

// direction: 0: Ox    1: -Oz    2: -Ox    3: Oz  
void Building::createBuildingSide(float height, float width, float x, float z, int direction) {
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	switch (direction) {
		case 0:
			vertices = {
			glm::vec3(x + width, height, z),	// Top Right
			glm::vec3(x + width, 0.0f, z),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			normals = {
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1)
			};
			lightPoints.push_back(glm::vec3(x + width / 2, 0, z - lightWallDist));
			break;
		case 1:
			vertices = {
			glm::vec3(x, height, z + width),	// Top Right
			glm::vec3(x, 0.0f, z + width),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			normals = {
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0)
			};
			lightPoints.push_back(glm::vec3(x + lightWallDist, 0, z + width / 2));

			break;
		case 2:
			vertices = {
			glm::vec3(x - width, height, z),	// Top Right
			glm::vec3(x - width, 0.0f, z),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			normals = {
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1)
			};
			lightPoints.push_back(glm::vec3(x - width / 2, 0, z + lightWallDist));

			break;
		case 3:
			vertices = {
			glm::vec3(x, height, z - width),	// Top Right
			glm::vec3(x, 0.0f, z - width),	// Bottom Right
			glm::vec3(x, 0.0f, z),	// Bottom Left
			glm::vec3(x, height, z),	// Top Left
			};
			normals = {
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0)
			};
			lightPoints.push_back(glm::vec3(x - lightWallDist, 0, z - width / 2));

			break;
		default:
			// ERROR
			break;
	}
	
	vector<glm::vec2> textureCoords
	{
		glm::vec2(width * 4, 0.0f),
		glm::vec2(width * 4, height * 5),
		glm::vec2(0.0f, height * 5),
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
	mapTextures.push_back(textureWalls);
}

void Building::render(Shader *shader, EngineComponents::Camera *camera, glm::vec3 lightPosition, int typeOfLight) {
	glm::mat4 modelMatrix = glm::mat4(1);

	// If light is set to night light, the increase the diffuse parameter
	if (typeOfLight == 1) {
		materialKd = 2;
	}
	for (int i = 0; i < meshes.size(); i++) {	
		renderMesh(meshes[i], shader, modelMatrix, mapTextures[i], camera, lightPosition, glm::vec3(0, 1, 0), typeOfLight);
	}

	// Set back the diffuse param
	if (typeOfLight == 1) {
		materialKd = 0.5;
	}
}

void Building::renderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, EngineComponents::Camera *camera, glm::vec3 lightPosition, glm::vec3 lightDirection, int typeOfLight)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

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

	if (typeOfLight == 1) {
		char buffer[64];
		int loc;
		for (int i = 0; i < 4; i++) {
			sprintf(buffer, "spotLightsPos[%i]", i);
			loc = glGetUniformLocation(shader->program, buffer);
			glUniform3f(loc, lightPoints[i].x, lightPoints[i].y, lightPoints[i].z);
		}
		int cut_off_angle = glGetUniformLocation(shader->program, "cut_off_angle");
		glUniform1f(cut_off_angle, cutoff_angle);
	}
	
	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}