/*============================================================
	[tutorialManager.cpp]
	¥‘è–¼ : ƒ`ƒ…[ƒgƒŠƒAƒ‹‚Å‚Ì“®‚«‚ðŠÇ—
	¥DATE : 2026/03/18
	¥AUTHOR : ‹v•ÛŽ› r‰î
============================================================*/
#include "main.h"
#include "renderer.h"
#include "tutorialManager.h"

#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "player.h"
#include "guide.h"
#include "tutorialEnemy.h"
#include "mouseManager.h"
#include "speedManager.h"
#include "fade.h"

int TutorialManager::m_Frame;
int TutorialManager::m_Phase;
bool TutorialManager::m_SetGuide;
bool TutorialManager::m_WasSlow;
Guide* TutorialManager::m_Guide;
TutorialEnemy* TutorialManager::m_Enemy;

void TutorialManager::Init()
{
	m_Frame = 0;
	m_Phase = 0;
	m_SetGuide = false;
	m_WasSlow = false;
	m_Guide = nullptr;
	m_Enemy = Manager::GetScene()->AddGameObject<TutorialEnemy>(object);
	m_Enemy->SetPosition({ 0.0f,0.0f,23.0f });
	m_Enemy->SetScale({ 1.5f,1.5f,1.5f });
}

void TutorialManager::Update()
{
	if(m_Guide == nullptr)
		m_Guide = Manager::GetScene()->GetGameObject<Guide>();

	if (m_Guide == nullptr || m_Enemy == nullptr) return;

	if (m_SetGuide)
	{
		if (!m_Guide->GetIsFinish())
		{
			m_Frame++;
		}

		if (m_Frame == ATTACK_FRAME)
		{
			SetAction();
		}

		switch (m_Phase)
		{
		case 0:
			if (m_Frame >= SHOT_WAIT_FRAME)
				if (NextGuide())
					SetGuide();

			break;
		case 1:
			if (m_Frame >= FRONT_WAIT_FRAME)
				if (NextGuide())
					SetGuide();
			break;
		case 2:
			if (MouseManager::IsMouseRightDown())
				m_WasSlow = true;

			if (m_Frame >= FAN_WAIT_FRAME)
				if (NextGuide())
					SetGuide();
			break;
		case 3:
			if (m_Enemy->GetHp() != m_Enemy->f_MaxHp)
				if (NextGuide())
					SetGuide();
		default:
			break;
		}

		
	}
	else
	{
		SetGuide();
	}
	

#ifdef _DEBUG

#endif // _DEBUG
}

void TutorialManager::SetGuide()
{
	switch (m_Phase)
	{
	case 0:
		m_Guide->MoveExplain();

		break;
	case 1:
		m_Guide->JumpExplain();
		
		break;
	case 2:
		m_Guide->SlowExplain();
		break;
	case 3:
		m_Guide->ShotExplain();
		break;
	case 4:
		m_Guide->GameExplain();
		break;
	default:
		break;
	}

	SpeedManager::SetSpeed(0.0f);
	m_SetGuide = true;
	m_Frame = 0;
}

bool TutorialManager::NextGuide()
{
	Player* player = Manager::GetScene()->GetGameObject<Player>();

	switch (m_Phase)
	{
	case 0:

		if (player->GetHp() == player->f_MaxHp)
		{
			m_Phase++;
			m_SetGuide = false;
			return true;
		}

		player->SetHp(player->f_MaxHp);
		SetGuide();

		break;
	case 1:
		if (player->GetHp() == player->f_MaxHp)
		{
			m_Phase++;
			m_SetGuide = false;
			player->SetSlowCharge(player->SLOW_MAX_CHARGE);
			return true;
		}

		player->SetHp(player->f_MaxHp);
		SetGuide();

		break;
	case 2:
		if (player->GetHp() == player->f_MaxHp && m_WasSlow)
		{
			m_Phase++;
			m_SetGuide = false;
			return true;
		}

		player->SetHp(player->f_MaxHp);
		player->SetSlowCharge(player->SLOW_MAX_CHARGE);
		m_WasSlow = false;
		SetGuide();

		break;
	case 3:
		m_Phase++;
		m_SetGuide = false;
		return true;
		break;
	default:
		break;
	}

	return false;
}

void TutorialManager::SetAction()
{
	switch (m_Phase)
	{
	case 0:
		m_Enemy->SetAttack(A_Laser);

		break;
	case 1:
		m_Enemy->SetAttack(A_Front);
		break;
	case 2:
		m_Enemy->SetAttack(A_Fan);
		break;
	case 3:
		m_Enemy->SetTargetPoint();
		break;
	case 4:
		Manager::GetScene()->SetIsFade(true);
		Fade::SetFadeIn(30);
		break;
	default:
		break;
	}
}

void TutorialManager::Retry()
{
	m_Guide->RetryExplain();
	SpeedManager::SetSpeed(0.0f);
	m_Frame = 0;
}
