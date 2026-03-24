/*============================================================
	[loading.cpp]
	▼題名 : ローディングシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include <thread>
#include "manager.h"
#include "fade.h"


#include "game.h"
#include "player.h"
#include "sky.h"
#include "wall.h"
#include "meshField.h"
#include "enemy.h"
#include "damageArea.h"
#include "laser.h"
#include "playerLaser.h"
#include "stone.h"

#include "damageScreen.h"
#include "playerHp.h"
#include "enemyHp.h"
#include "slowGauge.h"

#include "loading.h"
#include "loadCamera.h"
#include "loadText.h"

#include "Tutorial.h"
#include "guide.h"
#include "tutorialEnemy.h"

LoadScene Loading::m_Scene = Load_Game;

void Loading::Init()
{
	m_IsFade = false;

	Fade::SetFadeOut(30);

	LoadCamera* pCamera = AddGameObject<LoadCamera>(camera);
	pCamera->SetPosition({5.0f, 0.0f, -8.0f});
	pCamera->SetTarget({ 5.0f, 0.0f, -5.0f });

	LoadText* tText = AddGameObject<LoadText>(object);
	tText->SetPosition(-3.0f, 0.0f, 5.0f);

	Load();
}

void Loading::Update()
{
	Scene::Update();

	m_Frame++;

	if (m_IsFinish && m_Frame >= MIN_LOAD_FRAME && m_IsFade == false)
	{
		m_IsFade = true;
		Fade::SetFadeIn(30);
	}

	if (m_IsFade == true && Fade::GetActive() == false)
	{
		switch (m_Scene)
		{
		case Load_Game:
			Manager::SetScene<Game>();
			break;
		case Load_Tutorial:
			Manager::SetScene<Tutorial>();
			break;
		default:
			break;
		}
	}
}

void Loading::Load()
{
	switch (m_Scene)
	{
	case Load_Game:
	{
		std::thread Load([=]
			{
				Player::Load();
				Sky::Load();
				Wall::Load();
				MeshField::Load();
				Enemy::Load();
				DamageArea::Load();
				Laser::Load();
				PlayerLaser::Load();
				Stone::Load();

				PlayerHp::Load();
				EnemyHp::Load();
				SlowGauge::Load();
				DamageScreen::Load();

				m_IsFinish = true;
			});
		Load.detach();
	}
		break;
	case Load_Tutorial:
	{
		std::thread Load([=]
			{
				Player::Load();
				Sky::Load();
				Wall::Load();
				MeshField::Load();
				TutorialEnemy::Load();
				DamageArea::Load();
				Laser::Load();
				PlayerLaser::Load();

				PlayerHp::Load();
				EnemyHp::Load();
				SlowGauge::Load();
				DamageScreen::Load();
				Guide::Load();

				m_IsFinish = true;
			});
		Load.detach();
	}
		break;
	default:
		break;
	}
}

void Loading::SetLoadNextScene(LoadScene Scene)
{
	m_Scene = Scene;
}
