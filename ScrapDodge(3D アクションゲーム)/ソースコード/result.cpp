/*============================================================
	[result.cpp]
	▼題名 : リザルトシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "result.h"
#include "title.h"
#include "game.h"
#include "fade.h"

#include "manager.h"
#include "camera.h"
#include "polygon.h"
#include "keyboard.h"
#include "resultButton.h"

void Result::Init()
{
	m_IsFade = false;
	Fade::SetFadeOut(120);

	AddGameObject<Polygon2D>(ui)->SetInfo({ 0.0f ,0.0f ,0.0f }, { SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f }, { 0.0f,0.0f,0.0f }, "asset\\texture\\result.png");
	ResultButton* rButton = AddGameObject<ResultButton>(ui);
	rButton->SetText();
}

void Result::Update()
{
	Scene::Update();
}
