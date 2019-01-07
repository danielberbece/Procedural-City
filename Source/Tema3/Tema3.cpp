#include "Tema3.h"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <Core/Engine.h>

using namespace std;

Tema3::Tema3()
{
}

Tema3::~Tema3()
{
}

void Tema3::Init()
{
	/*const string textureLoc = "Source/Tema3/Textures/";

	// Load textures
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "img5.jpg").c_str(), GL_REPEAT);
		mapTextures["walls1"] = texture;
	}
	
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "roof.jpg").c_str(), GL_REPEAT);
		mapTextures["roof1"] = texture;
	}
	
	std::srand(std::time(NULL));
	int rootLine = std::rand() / (RAND_MAX / MODEL_WIDTH);
	int rootCol = std::rand() / (RAND_MAX / MODEL_WIDTH);
	Building * b;
	

	for (int i = 0; i < MODEL_WIDTH; i++) {
		cityMap[i][randCol] = 1;
		b = new Building(TOWER, 3, meterUnitsScale, meterUnitsScale, i * meterUnitsScale, randCol * meterUnitsScale, mapTextures["walls1"], mapTextures["roof1"]);
		buildings.push_back(b);
		//for (int j = 0; j < MODEL_WIDTH; j++) {
		//}
	}*/

	// Create a simple building
	
	//b = new Building(TOWER, 5, 2, 1, 0, 0, mapTextures["walls1"], mapTextures["roof1"]);
	//buildings.push_back(b);
	/*for (int i = 0; i < 60; i += 2) {
		for (int j = 0; j < 60; j += 2) {
			float height = 0;
			while (height < 0.2) {
				height = ((float)std::rand()) / (RAND_MAX / 4);
			}
			float typeRand = ((float)std::rand()) / ((float)RAND_MAX / 3);
			if (typeRand < 1) {
				b = new Building(BLOCKY, height, 1, 1, i, j, mapTextures["walls1"], mapTextures["roof1"]);
				buildings.push_back(b);
			} else if(typeRand < 2) {
				b = new Building(TOWER, height, 1, 1, i, j, mapTextures["walls1"], mapTextures["roof1"]);
				buildings.push_back(b);
			} else {
				b = new Building(SIMPLE, height, 1, 1, i, j, mapTextures["walls1"], mapTextures["roof1"]);
				buildings.push_back(b);
			}
		}
	}*/

	city = new City();

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderCity");
		shader->AddShader("Source/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema3/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Tema3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema3::Update(float deltaTimeSeconds) {
	city->renderCity(shaders["ShaderCity"], GetSceneCamera());
}

void Tema3::FrameEnd()
{
	//DrawCoordinatSystem();
}

void Tema3::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		//TODO : activate texture location 0
		//TODO : Bind the texture1 ID
		//TODO : Send texture uniform value
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_1"), 0);
	}

	if (texture2)
	{
		//TODO : activate texture location 1
		//TODO : Bind the texture2 ID
		//TODO : Send texture uniform value
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_2"), 1);
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Tema3::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	}
}

void Tema3::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Tema3::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema3::OnWindowResize(int width, int height)
{
}