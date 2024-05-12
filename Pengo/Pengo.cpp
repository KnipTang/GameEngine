// Pengo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SDL.h>
#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"


#include <iostream>
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "RotationComponent.h"
#include "InputManager.h"
#include "Subject.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "CollisionComponent.h"
#include "PengoCollisionObserver.h"
#include "SDLSoundSystem.h"
#include "SoundCommands.h"
#include "PlayerCommands.h"
#include "FpsComponent.h"
#include "MovementComponent.h"
#include "PengoState.h"
#include "PengoComponent.h"
#include "Wall.h"
#include "Enemy.h"
#include "Animation.h"

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Demo");

	//Background image
	auto backGround = std::make_unique<dae::GameObject>();
	backGround.get()->AddComponent(new dae::RenderComponent(backGround.get()));
	backGround.get()->GetComponent<dae::RenderComponent>()->SetTexture("background.tga");
	//
	//DAE logo image
	auto DAElogo = std::make_unique<dae::GameObject>();
	DAElogo.get()->AddComponent(new dae::RenderComponent(DAElogo.get()));
	DAElogo.get()->GetComponent<dae::RenderComponent>()->SetTexture("logo.tga");
	DAElogo.get()->SetGameObjectPosition(216, 180);

	//Text
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto text = std::make_unique<dae::GameObject>();
	text.get()->SetGameObjectPosition(80, 20);
	text.get()->AddComponent(new dae::TextObject(text.get(), "Programming 4 Assignment", std::move(font)));

	//FPS
	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 12);
	auto FPS = std::make_unique<dae::GameObject>();
	FPS.get()->SetGameObjectPosition(0, 0);
	FPS.get()->AddComponent(new dae::TextObject(FPS.get(), "0FPS", std::move(font)));
	FPS.get()->AddComponent(new dae::FpsComponent(FPS.get(), FPS.get()->GetComponent<dae::TextObject>()));

	
	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);
	auto P1_Explain = std::make_unique<dae::GameObject>();
	P1_Explain.get()->SetGameObjectPosition(0, 100);
	P1_Explain.get()->AddComponent(new dae::TextObject(P1_Explain.get(), "Use the D-Pad to move m_Player 1, X to inflict damage, A and B to pick up pellets", std::move(font)));

	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);
	auto P2_Explain = std::make_unique<dae::GameObject>();
	P2_Explain.get()->SetGameObjectPosition(0, 120);
	P2_Explain.get()->AddComponent(new dae::TextObject(P2_Explain.get(), "Use WASD to move m_Player 2, C to inflict damage, Z and X to pick up pellets", std::move(font)));

	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);
	auto Sound_Explain = std::make_unique<dae::GameObject>();
	Sound_Explain.get()->SetGameObjectPosition(50, 200);
	Sound_Explain.get()->AddComponent(new dae::TextObject(Sound_Explain.get(), "Press P to play sound", std::move(font)));

	int element = 16;
	int mapBorder = 8;

	//P1
	//PengoPlayer P1{ mapBorder, mapBorder };
	auto P1 = std::make_unique<dae::GameObject>();
	P1.get()->AddComponent(new PengoComponent(P1.get()));
	P1.get()->AddComponent(new dae::RenderComponent(P1.get()));
	P1.get()->GetComponent<dae::RenderComponent>()->SetTexture("CharactersSheet.png");
	P1.get()->GetComponent<dae::RenderComponent>()->SetSourceRect(16 * 0, 16 * 0, 16, 16);
	P1.get()->AddComponent(new Animation(P1.get()));
	P1.get()->SetGameObjectPosition(static_cast<float>(mapBorder), static_cast<float>(mapBorder));
	P1.get()->AddComponent(new dae::CollisionComponent(P1.get(), 16.f, 16.f));
	P1.get()->AddComponent(new dae::HealthComponent(P1.get()));
	P1.get()->AddComponent(new dae::ScoreComponent(P1.get()));
	P1.get()->AddComponent(new MovementComponent(P1.get()));
	P1.get()->GetComponent<dae::CollisionComponent>()->AddObserver(new PengoCollisionObserver(P1.get()));
	P1.get()->SetTag("Player");

	dae::InputManager::GetInstance().BindCommand(SDLK_w, dae::InputActionType::IsDown, std::make_unique<dae::Movement>(P1.get(), Controlls::UP));
	dae::InputManager::GetInstance().BindCommand(SDLK_s, dae::InputActionType::IsDown, std::make_unique<dae::Movement>(P1.get(), Controlls::DOWN));
	dae::InputManager::GetInstance().BindCommand(SDLK_a, dae::InputActionType::IsDown, std::make_unique<dae::Movement>(P1.get(), Controlls::LEFT));
	dae::InputManager::GetInstance().BindCommand(SDLK_d, dae::InputActionType::IsDown, std::make_unique<dae::Movement>(P1.get(), Controlls::RIGHT));
	dae::InputManager::GetInstance().BindCommand(SDLK_e, dae::InputActionType::IsDown, std::make_unique<dae::Attack>(P1.get()));

	auto P2 = std::make_unique<dae::GameObject>();
	P2.get()->AddComponent(new dae::RenderComponent(P2.get()));
	P2.get()->GetComponent<dae::RenderComponent>()->SetTexture("CharactersSheet.png");
	P2.get()->GetComponent<dae::RenderComponent>()->SetSourceRect(0, 16*0, 16, 16);
	P2.get()->SetGameObjectPosition(16 * 20, 16 * 15);
	P2.get()->AddComponent(new dae::CollisionComponent(P2.get(), 16, 16));
	P2.get()->AddComponent(new dae::HealthComponent(P2.get()));
	P2.get()->AddComponent(new dae::ScoreComponent(P2.get()));
	P2.get()->GetComponent<dae::CollisionComponent>()->AddObserver(new PengoCollisionObserver(P2.get()));
	P2.get()->SetTag("Player");

	Enemy enemy{ mapBorder + element * 2, mapBorder + element * 1 };

	//Wall
	std::vector<Wall> walls;
	{
		walls.emplace_back(mapBorder + element * 1,  mapBorder + element * 0);
		walls.emplace_back(mapBorder + element * 5,  mapBorder + element * 0);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 0);

		walls.emplace_back(mapBorder + element * 1,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 3,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 4,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 5,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 6,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 7,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 8,	 mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 9,  mapBorder + element * 1);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 1);

		walls.emplace_back(mapBorder + element * 3,  mapBorder + element * 2);
		walls.emplace_back(mapBorder + element * 9,  mapBorder + element * 2);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 2);

		walls.emplace_back(mapBorder + element * 1,	 mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 2,	 mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 3,	 mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 5,  mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 6,	 mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 7,	 mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 9,	 mapBorder + element * 3);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 3);

		walls.emplace_back(mapBorder + element * 5, mapBorder + element * 4);
		walls.emplace_back(mapBorder + element * 9,  mapBorder + element * 4);

		walls.emplace_back(mapBorder + element * 0,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 1,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 2,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 3,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 4,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 5,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 7,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 8,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 9,  mapBorder + element * 5);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 5);

		walls.emplace_back(mapBorder + element * 7,  mapBorder + element * 6);
		walls.emplace_back(mapBorder + element * 9,  mapBorder + element * 6);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 6);

		walls.emplace_back(mapBorder + element * 1, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 2, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 3, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 4, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 5, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 6, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 9, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 7);
		walls.emplace_back(mapBorder + element * 12, mapBorder + element * 7);

		walls.emplace_back(mapBorder + element * 1, mapBorder + element * 8);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 8);

		walls.emplace_back(mapBorder + element * 1, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 2, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 3, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 4, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 5, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 9, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 10, mapBorder + element * 9);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 9);

		walls.emplace_back(mapBorder + element * 1, mapBorder + element * 10);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 10);
		walls.emplace_back(mapBorder + element * 9, mapBorder + element * 10);

		walls.emplace_back(mapBorder + element * 1, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 3, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 4, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 5, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 6, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 9, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 10, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 11);
		walls.emplace_back(mapBorder + element * 12, mapBorder + element * 11);

		walls.emplace_back(mapBorder + element * 3, mapBorder + element * 12);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 12);

		walls.emplace_back(mapBorder + element * 0, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 1, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 3, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 5, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 7, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 8, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 9, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 10, mapBorder + element * 13);
		walls.emplace_back(mapBorder + element * 11, mapBorder + element * 13);

		walls.emplace_back(mapBorder + element * 5, mapBorder + element * 14);
	}

	//Borders
	std::vector<std::unique_ptr<dae::GameObject>> borders;
	{
		auto border = std::make_unique<dae::GameObject>();
		border.get()->SetGameObjectPosition(0, 0);
		border.get()->AddComponent(new dae::CollisionComponent(border.get(), 224.f, 8.f));
		border.get()->SetTag("Wall");
		borders.emplace_back(std::move(border));

		border = std::make_unique<dae::GameObject>();
		border.get()->SetGameObjectPosition(0, 0);
		border.get()->AddComponent(new dae::CollisionComponent(border.get(), 8.f, 256.f));
		border.get()->SetTag("Wall");
		borders.emplace_back(std::move(border));

		border = std::make_unique<dae::GameObject>();
		border.get()->SetGameObjectPosition(0, 248.f);
		border.get()->AddComponent(new dae::CollisionComponent(border.get(), 224.f, 8.f));
		border.get()->SetTag("Wall");
		borders.emplace_back(std::move(border));

		border = std::make_unique<dae::GameObject>();
		border.get()->SetGameObjectPosition(216.f, 0);
		border.get()->AddComponent(new dae::CollisionComponent(border.get(), 8.f, 256.f));
		border.get()->SetTag("Wall");
		borders.emplace_back(std::move(border));
	}
	//auto displayLives{ std::make_shared<dae::GameObject>() };
	//displayLives.get()->SetGameObjectPosition(0, 150);
	//displayLives.get()->SetParent(P1.get(), true);
	//displayLives->AddComponent(new dae::StateDisplay(displayLives.get(), "Lives", P1.get()->GetComponent<dae::HealthComponent>()->GetLives()));
	//scene.Add(displayLives);
	//
	//auto displayPoints{ std::make_shared<dae::GameObject>() };
	//displayPoints.get()->SetGameObjectPosition(0, 170);
	//displayPoints.get()->SetParent(P1.get(), true);
	//displayPoints->AddComponent(new dae::StateDisplay(displayPoints.get(), "Points", P1.get()->GetComponent<dae::ScoreComponent>()->GetPoints()));
	//scene.Add(displayPoints);
	//auto achievementObject{ std::make_shared<dae::GameObject>() };
	//achievementObject.get()->SetParent(P1.get(), true);
	////achievementObject->AddComponent(new dae::Achievement(g_SteamAchievements));
	//scene.Add(achievementObject);
	//g_achievementObserver = new dae::Achievement(g_SteamAchievements);
	//P1.get()->GetComponent<dae::HealthComponent>()->AddObserver(displayLives->GetComponent<dae::StateDisplay>());
	//P1.get()->GetComponent<dae::ScoreComponent>()->AddObserver(displayPoints->GetComponent<dae::StateDisplay>());
	//P1.get()->GetComponent<dae::ScoreComponent>()->AddObserver(g_achievementObserver);

	auto GameBackground = std::make_unique<dae::GameObject>();
	GameBackground.get()->AddComponent(new dae::RenderComponent(GameBackground.get()));
	GameBackground.get()->GetComponent<dae::RenderComponent>()->SetTexture("LevelsSheet.png");
	GameBackground.get()->GetComponent<dae::RenderComponent>()->SetSourceRect(0, 0, 224, 256);
	GameBackground.get()->SetGameObjectPosition(16 * 0, 16 * 0);
	//background.get()->AddComponent(new dae::CollisionComponent(background.get(), 16.f, 16.f));

	/*
	displayLives = std::make_shared<dae::GameObject>();
	displayLives.get()->SetGameObjectPosition(0, 190);
	displayLives.get()->SetParent(P2.get(), true);
	displayLives->AddComponent(new dae::StateDisplay(displayLives.get(), "Lives", P2.get()->GetComponent<dae::HealthComponent>()->GetLives()));
	scene.Add(displayLives);

	displayPoints = std::make_shared<dae::GameObject>();
	displayPoints.get()->SetGameObjectPosition(0, 210);
	displayPoints.get()->SetParent(P2.get(), true);
	displayPoints->AddComponent(new dae::StateDisplay(displayPoints.get(), "Points", P2.get()->GetComponent<dae::ScoreComponent>()->GetPoints()));
	scene.Add(displayPoints);

	P2.get()->GetComponent<dae::HealthComponent>()->AddObserver(displayLives->GetComponent<dae::StateDisplay>());
	P2.get()->GetComponent<dae::ScoreComponent>()->AddObserver(displayPoints->GetComponent<dae::StateDisplay>());
	P2.get()->GetComponent<dae::ScoreComponent>()->AddObserver(g_achievementObserver);

	//Trash the Cache
	//go = std::make_shared<dae::GameObject>();
	//go.get()->AddComponent(new dae::Exercise1_ImGui_Component(go.get()));
	//go.get()->AddComponent(new dae::Exercise2_ImGui_Component(go.get()));
	//scene.Add(go);
	*/

	//auto& ss = dae::SoundServiceLocator::get_sound_system();
	//ss.play("PinkPanther60.wav", 100);
	//ss.play("test.mp3", 100);

	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_DPAD_UP), dae::InputActionType::IsPressed, std::make_unique<dae::Movement>(P2.get(), glm::vec3(0, -1, 0)));
	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_DPAD_DOWN), dae::InputActionType::IsPressed, std::make_unique<dae::Movement>(P2.get(), glm::vec3(0, 1, 0)));
	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_DPAD_LEFT), dae::InputActionType::IsPressed, std::make_unique<dae::Movement>(P2.get(), glm::vec3(-1, 0, 0)));
	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_DPAD_RIGHT), dae::InputActionType::IsPressed, std::make_unique<dae::Movement>(P2.get(), glm::vec3(1, 0, 0)));

	dae::InputManager::GetInstance().BindCommand(SDLK_p, dae::InputActionType::IsUp, std::make_unique<dae::SoundCommand>());

	//scene.Add(std::move(backGround));
	//scene.Add(std::move(DAElogo));
	//scene.Add(std::move(text));
	//scene.Add(std::move(P1_Explain));
	//scene.Add(std::move(P2_Explain));
	//scene.Add(std::move(Sound_Explain));

	scene.Add(std::move(GameBackground));
	scene.Add(std::move(P1));
	scene.Add(std::move(P2));
	scene.Add(enemy.GetActor());
	for (auto& wall : walls)
	{
		scene.Add(wall.GetActor());
	}
	for (auto& bord : borders)
	{
		scene.Add(std::move(bord));
	}
	scene.Add(std::move(FPS));

	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_B), dae::InputActionType::IsDown, std::make_unique<dae::PointIncrease>(P1.get(), P1.get()->GetComponent<dae::ScoreComponent>()));
	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_A), dae::InputActionType::IsDown, std::make_unique<dae::PointIncrease>(P1.get(), P1.get()->GetComponent<dae::ScoreComponent>()));
	//dae::InputManager::GetInstance().BindCommand(WORD(XINPUT_GAMEPAD_X), dae::InputActionType::IsDown, std::make_unique<dae::Damage>(P1.get(), P1.get()->GetComponent<dae::HealthComponent>()));
	//dae::InputManager::GetInstance().BindCommand(SDLK_z, dae::InputActionType::IsDown, std::make_unique<dae::PointIncrease>(P2.get(), P2.get()->GetComponent<dae::ScoreComponent>()));
	//dae::InputManager::GetInstance().BindCommand(SDLK_x, dae::InputActionType::IsDown, std::make_unique<dae::PointIncrease>(P2.get(), P2.get()->GetComponent<dae::ScoreComponent>()));
	//dae::InputManager::GetInstance().BindCommand(SDLK_c, dae::InputActionType::IsDown, std::make_unique<dae::Damage>(P2.get(), P2.get()->GetComponent<dae::HealthComponent>()));
	//dae::InputManager::GetInstance().BindCommand(SDLK_t, dae::InputActionType::IsDown, std::make_unique<dae::Damage>(go.get(), go.get()->GetComponent<dae::HealthComponent>()));
	//dae::InputManager::GetInstance().BindCommand(SDLK_x, dae::InputActionType::IsDown, std::make_unique<dae::PointIncrease>(go.get(), go.get()->GetComponent<dae::ScoreComponent>()));

	//if (g_SteamAchievements)
	//	g_SteamAchievements->SetAchievement("ACH_WIN_ONE_GAME");
}
int main(int, char* []) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
	return 0;
}