/*============================================================
	[speedManager.cpp]
	¥‘è–¼ : ¢ŠE‚Ì‘¬‚³‚ğŠÇ—‚·‚éƒ}ƒl[ƒWƒƒ[
	¥DATE : 2026/03/18
	¥AUTHOR : ‹v•Û› r‰î
============================================================*/
#include "main.h"
#include "renderer.h"
#include "speedManager.h"

#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "player.h"

float SpeedManager::m_Speed;
bool SpeedManager::m_StopTime;
int SpeedManager::m_Frame;
SPEEDMODE SpeedManager::m_Mode;

void SpeedManager::Init()
{
	m_Speed = 1.0f;
	m_Frame = 0;
	m_StopTime = false;
	m_Mode = Speed_Default;
}

void SpeedManager::Update()
{
	if (m_StopTime)
	{
		m_Frame--;

		if (m_Frame == 0)
		{
			m_StopTime = false;
			m_Speed = 1.0f;
		}

		return;
	}

	switch (m_Mode)
	{
	case Speed_Default:
		break;
	case Speed_Slow:

		
		break;
	case Speed_Dead:
		
		if (m_Frame == 0)
		{
			m_Speed = 1.0f;
		}

		m_Frame--;

		break;
	case Speed_Clear:
		break;
	default:
		break;
	}
}

float SpeedManager::GetTimeLimit()
{
	float timeLimit = 1.0f - ((float)m_Frame / (float)m_SlowEndFrame);

	return timeLimit;
}

void SpeedManager::HitStop(int frame)
{
	m_StopTime = true;
	m_Speed = 0.0f;
	m_Frame = frame;
}

void SpeedManager::PlayerDead(float slowTime, float speed)
{
	m_Frame = (int)slowTime;
	m_Speed = speed;
	m_Mode = Speed_Dead;
}
