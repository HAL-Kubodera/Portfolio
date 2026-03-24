/*
ファイル : pauseManager.cpp
  名 前  : Shunsuke Kubodera
  日 付  : 2026/1/15
*/
#include "main.h"
#include "renderer.h"
#include "speedManager.h"
#include "pauseManager.h"

#include "manager.h"
#include "scene.h"

#include "pauseScreen.h"
#include "pauseButton.h"

bool PauseManager::m_IsPause = false;
std::vector<GameObject*> PauseManager::m_PauseObject;

void PauseManager::Init()
{
	m_IsPause = false;
	m_PauseObject.clear();
}

void PauseManager::Update()
{
	if (Keyboard_IsKeyDownTrigger(KK_P))
	{
		ChangePause();
	}
}

void PauseManager::Uninit()
{

}



void PauseManager::ChangePause()
{
	m_IsPause = !m_IsPause;

	if (m_IsPause)
	{
		GameObject* pScr = Manager::GetScene()->AddGameObject<PauseScreen>(ui);
		m_PauseObject.push_back(pScr);

		pScr = Manager::GetScene()->AddGameObject<PauseButton>(ui);
		m_PauseObject.push_back(pScr); 
	}
	else
	{
		for (GameObject* obj : m_PauseObject)
		{
			obj->SetDestroy();
		}

		m_PauseObject.clear();
	}
}
