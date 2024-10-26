#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <tileRenderer.h>

struct GameData
{
	//glm::vec2 playerPos = {100,100};
	glm::vec2 cameraPos = {100,100};

}gameData;

gl2d::Renderer2D renderer;

constexpr int BACKGROUND_COUNT = 3;

gl2d::Texture spaceShipTexture;
glm::vec2 shipPos{};

gl2d::Texture backgroundTexture[BACKGROUND_COUNT];
TileRenderer tileRenderer[BACKGROUND_COUNT];

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	spaceShipTexture.loadFromFile(RESOURCES_PATH "/spaceShip/ships/green.png", true);
	backgroundTexture[0].loadFromFile(RESOURCES_PATH "/background1.png", true);
	backgroundTexture[1].loadFromFile(RESOURCES_PATH "/background2.png", true);
	backgroundTexture[2].loadFromFile(RESOURCES_PATH "/background3.png", true);
	//backgroundTexture[3].loadFromFile(RESOURCES_PATH "/background4.png", true);

	tileRenderer[0].texture = backgroundTexture[0];
	tileRenderer[1].texture = backgroundTexture[1];
	tileRenderer[2].texture = backgroundTexture[2];
	//tileRenderer[3].texture = backgroundTexture[3];

	tileRenderer[0].paralaxStrength = 0;
	tileRenderer[1].paralaxStrength = 0.5f;
	tileRenderer[2].paralaxStrength = 0.75f;

	renderer.currentCamera.zoom = 2.0f;
	

	//loading the saved data. Loading an entire structure like this makes savind game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

	return true;
}


//IMPORTANT NOTICE, IF YOU WANT TO SHIP THE GAME TO ANOTHER PC READ THE README.MD IN THE GITHUB
//https://github.com/meemknight/cmakeSetup
//OR THE INSTRUCTION IN THE CMAKE FILE.
//YOU HAVE TO CHANGE A FLAG IN THE CMAKE SO THAT RESOURCES_PATH POINTS TO RELATIVE PATHS
//BECAUSE OF SOME CMAKE PROGBLMS, RESOURCES_PATH IS SET TO BE ABSOLUTE DURING PRODUCTION FOR MAKING IT EASIER.

bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion

#pragma region camera movement

	float cameraSpeed = deltaTime * 200;

	glm::vec2 move = {};

	if (platform::isButtonHeld(platform::Button::W) ||
		platform::isButtonHeld(platform::Button::Up))
	{
		move.y = -1;
	}
	if (platform::isButtonHeld(platform::Button::S) ||
		platform::isButtonHeld(platform::Button::Down))
	{
		move.y = 1;
	}
	if (platform::isButtonHeld(platform::Button::A) ||
		platform::isButtonHeld(platform::Button::Left))
	{
		move.x = -1;
	}
	if (platform::isButtonHeld(platform::Button::D) ||
		platform::isButtonHeld(platform::Button::Right))
	{
		move.x = 1;
	}
	if (platform::isButtonHeld(platform::Button::Z))
	{
		if (renderer.currentCamera.zoom <= 3.0f)
		{
			renderer.currentCamera.zoom += deltaTime * 0.5f;
		}
	}
	if (platform::isButtonHeld(platform::Button::X))
	{
		if (renderer.currentCamera.zoom >= 1.0f)
		{
			renderer.currentCamera.zoom -= deltaTime * 0.5f;
		}
	}
	if (platform::isButtonHeld(platform::Button::Q))
	{
		renderer.currentCamera.rotation -= deltaTime * 20;
	}
	if (platform::isButtonHeld(platform::Button::E))
	{
		renderer.currentCamera.rotation += deltaTime * 20;
	}
	glm::vec3 moveVector(move, 0.0f);
	float cameraAngle = glm::radians(renderer.currentCamera.rotation);
	glm::vec3 axis(0.0f, 0.0f, 1.0f);

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), cameraAngle, axis);
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(moveVector, 1.0f);

	move.x = rotatedVector.x;
	move.y = rotatedVector.y;
	

	if (move.x != 0 || move.y != 0)
	{
		move = glm::normalize(move);
		move *= cameraSpeed;
		gameData.cameraPos += move;
	}

	renderer.currentCamera.follow(gameData.cameraPos, cameraSpeed, 10, 10, w, h);
#pragma endregion

#pragma region render background

	//renderer.currentCamera.zoom = 2.0f;

	for (int i = 0; i < BACKGROUND_COUNT; i++)
	{
		tileRenderer[i].render(renderer);
	}

#pragma endregion
	
#pragma region render bodies
	glm::vec2 shipCoordinate{ 100, 100 };
	if (shipPos.x == 0 && shipPos.y == 0)
	{
		shipPos = gameData.cameraPos + shipCoordinate;
	}
	renderer.renderRectangle({ shipPos, 100, 100 }, spaceShipTexture);

#pragma endregion

#pragma region mouse position

	auto mousePos = platform::getRelMousePosition();

#pragma endregion

	
	
	//renderer.renderRectangle({gameData.playerPos, 100, 100 }, spaceShipTexture);



	renderer.flush();

	//gameData.rectPos = glm::clamp(gameData.rectPos, glm::vec2{0,0}, glm::vec2{w - 100,h - 100});
	//renderer.renderRectangle({gameData.rectPos, 100, 100}, Colors_Blue);

	//renderer.flush();

	//ImGui::ShowDemoWindow();
	ImGui::Begin("Test Imgui");

	ImGui::DragFloat2("Zoom Level", &renderer.currentCamera.zoom);

	ImGui::End();

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{

	//saved the data.
	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
