/*============================================================
	[title.cpp]
	▼題名 : タイトルシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "title.h"
#include "loading.h"
#include "fade.h"
#include "audio.h"
#include "manager.h"
#include "titleCamera.h"
#include "polygon.h"
#include "keyboard.h"
#include "flag.h"
#include "robot.h"
#include "titleDorm.h"
#include "titleGround.h"
#include "titleLaser.h"
#include "titleStone.h"
#include "titleButton.h"

void Title::Init()
{
	m_IsFade = false;

	Fade::SetFadeOut(30);

	pCamera = AddGameObject<TitleCamera>(camera);
	AddGameObject<Flag>(object);
	AddGameObject<Robot>(object);
	AddGameObject<TitleDorm>(object);
	AddGameObject<TitleGround>(field);

	InitStone();

	m_BGM = new Audio();
	m_BGM->Load("asset\\audio\\title.wav");
	m_BGM->Play(true, BGM_VOLUME);
}

void Title::Update()
{
	m_Frame++;

	if (Keyboard_IsAnyKeyDown() && m_Frame < THIRD_CUT)
	{
		m_Frame = THIRD_CUT;
		Keyboard_Reset();
	}

	if (m_Frame == FIRST_CUT)
	{
		for (TitleStone* stone : pStones)
		{
			stone->SetDestroy();
		}
	}

	if (m_Frame == SECOND_CUT)
	{
		pCamera->SetMovie(1);
		
		SetMovieStone();
	}

	if (m_Frame == THIRD_CUT)
	{
		TitleButton* tb = AddGameObject<TitleButton>(ui);
		tb->SetText();
	}

	Scene::Update();

	

	if (m_IsFade == true && Fade::GetActive() == false)
	{
		Loading::SetLoadNextScene(Load_Tutorial);
		Manager::SetScene<Loading>();
	}
}

void Title::Uninit()
{
	m_BGM->Uninit();
	delete m_BGM;

	Scene::Uninit();
}

void Title::InitStone()
{
	TitleStone* stone;
	stone = AddGameObject<TitleStone>(object);
	stone->SetDefPos(STONES[0].pos);
	stone->SetScale(STONES[0].scale);
	stone->SetFrame(STONES[0].frame);
	stone->SetDelayFrame(STONES[0].delayFrame);
	pStones.push_back(stone);

	stone = AddGameObject<TitleStone>(object);
	stone->SetDefPos(STONES[1].pos);
	stone->SetScale(STONES[1].scale);
	stone->SetFrame(STONES[1].frame);
	stone->SetDelayFrame(STONES[1].delayFrame);
	pStones.push_back(stone);
}

void Title::SetMovieStone()
{
	{
		TitleLaser* laser = AddGameObject<TitleLaser>(object);
		laser->SetDefaultPos(FIRST_LASER_POS);
		laser->SetVelocity(FIRST_LASER_VEL);
		laser->SetMaxFrame(FIRST_LASER_FRAME);

		laser = AddGameObject<TitleLaser>(object);
		laser->SetDefaultPos(SECOND_LASER_POS);
		laser->SetVelocity(SECOND_LASER_VEL);
		laser->SetMaxFrame(SECOND_LASER_FRAME);

		laser = AddGameObject<TitleLaser>(object);
		laser->SetDefaultPos(THIRD_LASER_POS);
		laser->SetVelocity(THIRD_LASER_VEL);
		laser->SetMaxFrame(THIRD_LASER_FRAME);
	}
	{
		TitleStone* stone;

		for (int i = 0; i < M_STONE_NUM; i++)
		{
			stone = AddGameObject<TitleStone>(object);
			stone->SetDefPos(MOVIE_STONES[0].pos);
			stone->SetScale(MOVIE_STONES[0].scale);
			stone->SetDelayFrame((MOVIE_STONES[0].delayFrame * i) + MOVIE_STONES[0].frame);

			stone = AddGameObject<TitleStone>(object);
			stone->SetDefPos(MOVIE_STONES[1].pos);
			stone->SetScale(MOVIE_STONES[1].scale);
			stone->SetDelayFrame((MOVIE_STONES[1].delayFrame * i) + MOVIE_STONES[1].frame);

			stone = AddGameObject<TitleStone>(object);
			stone->SetDefPos(MOVIE_STONES[2].pos);
			stone->SetScale(MOVIE_STONES[2].scale);
			stone->SetDelayFrame((MOVIE_STONES[2].delayFrame * i) + MOVIE_STONES[2].frame);
		}
	}
}
