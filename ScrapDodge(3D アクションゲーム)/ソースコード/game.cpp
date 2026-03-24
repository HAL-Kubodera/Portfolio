/*============================================================
	[game.cpp]
	Å•ëËñº : ÉQÅ[ÉÄÉVÅ[Éì
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#include "main.h"
#include "manager.h"
#include "title.h"
#include "game.h"
#include "result.h"
#include "audio.h"
#include "fade.h"

#include "camera.h"
#include "sky.h"
#include "player.h"
#include "enemy.h"
#include "polygon.h"
#include "keyboard.h"
#include "playerHp.h"
#include "slowGauge.h"
#include "enemyHp.h"
#include "meshField.h"
#include "wall.h"
#include "damageScreen.h"

#include "laserInstance.h"
#include "cylinderInstance.h"
#include "speedManager.h"
#include "pauseManager.h"

#include <time.h>

void Game::Init()
{
	srand((unsigned int)time(NULL));

	m_IsFade = false;
	Fade::SetFadeOut(30);

	SpeedManager::Init();
	PauseManager::Init();

	GameObject* gameObject;

	AddGameObject<Camera>(camera);

	MeshField* mField = AddGameObject<MeshField>(field);

	AddGameObject<Sky>(field);

	gameObject = AddGameObject<Wall>(object);
	gameObject->SetPosition(16.0f, 0.75f, 0.0f);
	gameObject->SetScale(0.5f, 1.5f, 41.0f);
	gameObject->SetRotation(0.0f, -0.2f, 0.0f);

	gameObject = AddGameObject<Wall>(object);
	gameObject->SetPosition(-16.0f, 0.75f, 0.0f);
	gameObject->SetScale(0.5f, 1.5f, 41.0f);
	gameObject->SetRotation(0.0f, 0.2f, 0.0f);

	gameObject = AddGameObject<Wall>(object);
	gameObject->SetPosition(0.0f, 0.75f, 20.0f);
	gameObject->SetScale(0.5f, 1.5f, 24.0f);
	gameObject->SetRotation(0.0f, XM_PIDIV2, 0.0f);

	gameObject = AddGameObject<Wall>(object);
	gameObject->SetPosition(0.0f, 0.75f, -20.0f);
	gameObject->SetScale(0.5f, 1.5f, 40.0f);
	gameObject->SetRotation(0.0f, XM_PIDIV2, 0.0f);

	Enemy* pEnemy;
	pEnemy = AddGameObject<Enemy>(object);
	pEnemy->SetPosition({ 0.0f,0.0f,23.0f });
	pEnemy->SetScale({ 1.5f,1.5f,1.5f });

	AddGameObject<LaserInstance>(laser);
	AddGameObject<CylinderInstance>(object);

	AddGameObject<Player>(object);

	AddGameObject<PlayerHp>(ui);
	AddGameObject<EnemyHp>(ui);
	AddGameObject<SlowGauge>(ui);

	AddGameObject<DamageScreen>(screen);

	m_BGM = new Audio();
	m_BGM->Load("asset\\audio\\game.wav");
	m_BGM->Play(true, BGM_VOLUME);
}

void Game::Uninit()
{
	m_BGM->Uninit();
	delete m_BGM;

	Scene::Uninit();
}

void Game::Update()
{
	SpeedManager::Update();
	PauseManager::Update();
	Scene::Update();

	if (m_IsFade == true && PauseManager::GetPause() == false && Fade::GetActive() == false)
		Manager::SetScene<Result>();
}

void Game::Draw()
{
	Scene::Draw();
}
