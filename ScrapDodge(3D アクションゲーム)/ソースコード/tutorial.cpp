/*============================================================
	[tutorial.cpp]
	▼題名 : チュートリアルシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "manager.h"
#include "title.h"
#include "game.h"
#include "tutorial.h"
#include "result.h"
#include "audio.h"
#include "fade.h"

#include "camera.h"
#include "sky.h"
#include "player.h"
#include "polygon.h"
#include "keyboard.h"
#include "playerHp.h"
#include "slowGauge.h"
#include "enemyHp.h"
#include "meshField.h"
#include "wall.h"
#include "damageScreen.h"

#include "laserInstance.h"
#include "speedManager.h"
#include "pauseManager.h"
#include "tutorialManager.h"
#include "loading.h"
#include "guide.h"

#include <time.h>

void Tutorial::Init()
{
	m_IsFade = false;
	Fade::SetFadeOut(30);

	SpeedManager::Init();
	PauseManager::Init();
	TutorialManager::Init();

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

	AddGameObject<LaserInstance>(laser);

	AddGameObject<Player>(object);

	AddGameObject<PlayerHp>(ui);
	AddGameObject<EnemyHp>(ui);
	AddGameObject<SlowGauge>(ui);
	AddGameObject<Guide>(ui);

	AddGameObject<DamageScreen>(screen);

	m_BGM = new Audio();
	m_BGM->Load("asset\\audio\\game.wav");
	m_BGM->Play(true, BGM_VOLUME);
}

void Tutorial::Uninit()
{
	m_BGM->Uninit();
	delete m_BGM;

	Scene::Uninit();
}

void Tutorial::Update()
{
	SpeedManager::Update();
	PauseManager::Update();
	TutorialManager::Update();
	Scene::Update();

	if (m_IsFade == true && Fade::GetActive() == false)
	{
		Loading::SetLoadNextScene(Load_Game);
		Manager::SetScene<Loading>();
	}
}

void Tutorial::Draw()
{
	Scene::Draw();
}
