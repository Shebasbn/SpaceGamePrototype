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

struct Camera {
	glm::vec2 cameraPos = { 0,0 };
}camera;

struct GameData
{
	glm::vec2 cameraPos = { 0,0 };

}gameData;

struct Body
{
	size_t ID;
	glm::vec2 position;
	float size;
	gl2d::Texture texture;
	std::string name;
	std::string descritpion;
};

glm::vec2 setBodyCoordinates(int x, int y) {
	return gameData.cameraPos + glm::vec2((float)x, (float)y);
}

gl2d::Renderer2D renderer;

constexpr int BACKGROUND_COUNT = 1;
constexpr int BODY_COUNT = 2;

//gl2d::Texture bodyTextures[BODY_COUNT];
Body bodies[BODY_COUNT];

//gl2d::Texture spaceShipTexture;
//glm::vec2 shipPos{};

//enum BODIES
//{
//	NO_BODY=-1, 
//};
BOOL openBodyMenu;
int bodyMenuID = -1;

gl2d::Texture backgroundTexture[BACKGROUND_COUNT];
TileRenderer tileRenderer[BACKGROUND_COUNT];

//gl2d::Texture terranPlanetTexture;
//glm::vec2 terranPos;
//constexpr float terranSize = 48 * 2;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	//loading the saved data. Loading an entire structure like this makes savind game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

#pragma region init bodies
	
	for (size_t i = 0; i < BODY_COUNT; i++) {
		bodies[i].ID = i;
	}

	bodies[0] = { 0, setBodyCoordinates(800, 400), 48 * 2};
	bodies[1] = { 1, setBodyCoordinates(-400, 200), 48 * 2 };

#pragma region

#pragma region load textures
	
	
	// Background tiles
	backgroundTexture[0].loadFromFile(RESOURCES_PATH "/Backgrounds/SpaceBackground.png", true);
	tileRenderer[0].texture = backgroundTexture[0];

	// System Bodies
	bodies[0].texture.loadFromFile(RESOURCES_PATH "/Planets/Terran.png", true);
	bodies[1].texture.loadFromFile(RESOURCES_PATH "/Planets/Lava.png", true);

#pragma endregion

	/*Body ship{}*/


	/*backgroundTexture[0].loadFromFile(RESOURCES_PATH "/background1.png", true);
	backgroundTexture[1].loadFromFile(RESOURCES_PATH "/background2.png", true);
	backgroundTexture[2].loadFromFile(RESOURCES_PATH "/background3.png", true);*/
	//backgroundTexture[3].loadFromFile(RESOURCES_PATH "/background4.png", true);

	/*tileRenderer[0].texture = backgroundTexture[0];
	tileRenderer[1].texture = backgroundTexture[1];
	tileRenderer[2].texture = backgroundTexture[2];*/
	//tileRenderer[3].texture = backgroundTexture[3];

	/*tileRenderer[0].paralaxStrength = 0;
	tileRenderer[1].paralaxStrength = 0.5f;
	tileRenderer[2].paralaxStrength = 0.75f;*/

	renderer.currentCamera.zoom = 2.0f;

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

	//gameData.cameraPos++;

#pragma endregion

#pragma region mouse position

	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.0f, h / 2.0f);
	glm::vec2 mouseDirection = mousePos - screenCenter;

	glm::vec2 normalMouseDirection = mouseDirection / screenCenter;

#pragma endregion

#pragma region camera movement

	float cameraSpeed = deltaTime * 200;
	BOOL mouseMovement = false;

	glm::vec2 move = {};

	if (normalMouseDirection.x >= 0.65f || normalMouseDirection.x <= -0.65f) {
		move.x = 1;
		mouseMovement = true;
	}

	if (normalMouseDirection.y >= 0.65f || normalMouseDirection.y <= -0.65f) {
		move.y = 1;
		mouseMovement = true;
	}

	if (platform::isButtonHeld(platform::Button::W) ||
		platform::isButtonHeld(platform::Button::Up))
	{
		move.y = -1;
		mouseMovement = false;
	}
	if (platform::isButtonHeld(platform::Button::S) ||
		platform::isButtonHeld(platform::Button::Down))
	{
		move.y = 1;
		mouseMovement = false;
	}
	if (platform::isButtonHeld(platform::Button::A) ||
		platform::isButtonHeld(platform::Button::Left))
	{
		move.x = -1;
		mouseMovement = false;
	}
	if (platform::isButtonHeld(platform::Button::D) ||
		platform::isButtonHeld(platform::Button::Right))
	{
		move.x = 1;
		mouseMovement = false;
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
		if (mouseMovement)
		{
			move *= normalMouseDirection;
		}
		gameData.cameraPos += move;
	}

	renderer.currentCamera.follow(gameData.cameraPos, cameraSpeed, 0, 0, w, h);
#pragma endregion

#pragma region render background

	//renderer.currentCamera.zoom = 2.0f;

	for (int i = 0; i < BACKGROUND_COUNT; i++)
	{
		tileRenderer[i].render(renderer);
	}

#pragma endregion
	
#pragma region render bodies

	for (int i = 0; i < BODY_COUNT; i++)
	{
		renderer.renderRectangle({ bodies[i].position, bodies[i].size, bodies[i].size }, bodies[i].texture);
	}
	
#pragma endregion

#pragma region interact with bodies
	glm::vec2 realMousePos;
	if (platform::isLMouseReleased())
	{
		// TODO: mouse position is not correct
		//glm::vec2 cameraTopLeft = gameData.cameraPos - screenCenter;
		//glm::vec2 cameraBottomeRight = gameData.cameraPos + screenCenter;
		/*float cameraWidth = (gameData.cameraPos.x - cameraTopLeft.x) * 2;
		float cameraHeight = (gameData.cameraPos.x - cameraTopLeft.x) * 2;*/
		realMousePos = gameData.cameraPos + (normalMouseDirection * screenCenter) / renderer.currentCamera.zoom;

		for (int b = 0; b < BODY_COUNT; b++)
		{
			if (realMousePos.x >= bodies[b].position.x && 
				realMousePos.x <= bodies[b].position.x + bodies[b].size &&
				realMousePos.y >= bodies[b].position.y && 
				realMousePos.y <= bodies[b].position.y + bodies[b].size)
			{
				openBodyMenu = true;
				bodyMenuID = b;
				break;
			}
			else
			{
				openBodyMenu = false;
				bodyMenuID = -1;
			}
		}
		

		/*if (realMousePos.x >= shipPos.x && realMousePos.x <= shipPos.x + shipSize &&
			realMousePos.y >= shipPos.y && realMousePos.y <= shipPos.y + shipSize)
		{
			openBodyMenu = true;
			bodyID = 0;
		}
		else if (realMousePos.x >= terranPos.x && terranPos.x <= terranPos.x + terranSize &&
			terranPos.y >= terranPos.y && realMousePos.y <= terranPos.y + terranSize)
		{
			openBodyMenu = true;
			bodyID = 1;
		}*/
		

		
	}

	/*void (glm::vec2 cameraPosition, ) {
		
	}*/

#pragma endregion
	
	//renderer.renderRectangle({gameData.playerPos, 100, 100 }, spaceShipTexture);



	renderer.flush();

	//gameData.rectPos = glm::clamp(gameData.rectPos, glm::vec2{0,0}, glm::vec2{w - 100,h - 100});
	//renderer.renderRectangle({gameData.rectPos, 100, 100}, Colors_Blue);

	//renderer.flush();

	//ImGui::ShowDemoWindow();

#pragma UI Menus
	if (openBodyMenu)
	{
		std::string menuTitle;
		std::string bodyName;
		std::string bodyType;
		switch (bodyMenuID)
		{
		case 0:
			menuTitle = "Planet Menu";
			bodyName = "Earth";
			bodyType = "Terran Planet";
			break;
		case 1:
			menuTitle = "Planet Menu";
			bodyName = "Tardul";
			bodyType = "Lava Planet";
			break;
		default:
			break;
		}
		ImGui::Begin(menuTitle.c_str());
		ImGui::Text(bodyName.c_str());
		ImGui::Text(bodyType.c_str());
		ImGui::End();
	}

	/*ImGui::Begin("Test Imgui");
	ImGui::DragFloat2("Zoom Level", &renderer.currentCamera.zoom);
	ImGui::DragFloat2("Renderer Camera Position", &renderer.currentCamera.position.x);
	ImGui::DragFloat2("Camera Position", &gameData.cameraPos.x);
	ImGui::DragFloat2("Normal Mouse Direction", &(float)normalMouseDirection.x);
	ImGui::DragFloat2("Mouse Position", &(float)mouseDirection.x);
	ImGui::DragFloat2("Real Mouse Position", &(float)realMousePos.x);
	ImGui::DragFloat4("Ship Position", &(float)shipPosition.x);
	ImGui::DragFloat2("Ship-Camera Position", &(float)glm::vec2(shipPos - gameData.cameraPos).x);
	ImGui::End();*/
	

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{

	//saved the data.
	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
