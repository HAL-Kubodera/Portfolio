/*============================================================
	[tutorialEnemy.cpp]
	▼題名 : チュートリアルでの敵
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "audio.h"
#include "tutorialEnemy.h"
#include "modelRenderer.h"
#include "manager.h"
#include "scene.h"

#include "camera.h"
#include "damageArea.h"
#include "laser.h"
#include "stone.h"
#include "laserCylinder.h"
#include "player.h"
#include "boxCollision.h"
#include "result.h"
#include "speedManager.h"
#include "pauseManager.h"
#include "tutorialManager.h"
#include "targetPoint.h"
#include "particleTargetPoint.h"

void TutorialEnemy::Init()
{
	m_AnimationModel = new AnimationModel();
	m_AnimationModel->Load("asset\\model\\Enemy_Idle.fbx");

	m_AnimationName = "Idle";
	m_AnimationNameNext = "Idle";

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_LaserSE = new Audio;
	m_LaserSE->Load("asset\\audio\\shot.wav");
	m_CylinderSE = new Audio;
	m_CylinderSE->Load("asset\\audio\\cylinder.wav");
	m_DamageSE = new Audio;
	m_DamageSE->Load("asset\\audio\\damage.wav");

	m_State = new StateIdle(this);
}

void TutorialEnemy::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f)return;
	if (PauseManager::GetPause())return;

	if (m_Hp == 0)
	{
		SpeedManager::SetMode(Speed_Clear);

		return;
	}

	if (m_WaitFrame != WAIT_MAX_FRAME)
	{
		m_WaitFrame++;
		return;
	}

	m_State->Update();
	
	UpdateComponent();
}

void TutorialEnemy::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;

	Renderer::SetWorldMatrix(world);

	m_AnimationModel->Draw();

	DrawComponent();
}

void TutorialEnemy::Uninit()
{
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

void TutorialEnemy::SetAttack(AttackMode mode)
{
	m_Mode[0] = mode;

	switch (m_Mode[0])
	{
	case A_Laser:
	{
		m_State->ChangeState<StateLaser>();

		break;
	}
	case A_Fan:
	{
		m_State->ChangeState<StateFan>();

		break;
	}
	case A_Front:
	{
		m_State->ChangeState<StateFront>();
	}
	break;
	default:
		break;
	}
}

void TutorialEnemy::SetTargetPoint()
{
	TargetPoint* pTPoint = Manager::GetScene()->AddGameObject<TargetPoint>(object);
	pTPoint->SetPosition(0.0f, 12.0f, 18.0f);

	ParticleTargetPoint* pParticleCrash = Manager::GetScene()->AddGameObject<ParticleTargetPoint>(object);
	pParticleCrash->SetPosition({ 0.0f, 12.0f, 18.0f });
	pParticleCrash->SetColor({ 0.0f,1.0f,1.0f,1.0f });
}

void TutorialEnemy::TakeDamage()
{
	m_Hp-=5;
	m_AttackNum = 0;
	m_Level++;
	m_WaitFrame = 0;
}

void TutorialEnemy::StateIdle::Update()
{

}

TutorialEnemy::StateLaser::StateLaser(TutorialEnemy* pEnemy)
{
	m_Parent = pEnemy;

	SetDamageArea();
}

void TutorialEnemy::StateLaser::Update()
{
	m_AttackFrame++;

	//プレイヤーに対して30フレームの警告の後発射 直線状に攻撃 ダメージエリアのOBBで判定
	if ((int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		m_LaserSE->Play(false, SE_VOLUME);

		Laser* laser = Manager::GetScene()->AddGameObject<Laser>(object);

		GameObject* target = Manager::GetScene()->GetGameObject<Player>();

		if (target != nullptr)
		{
			laser->SetPosition(SHOT_POS);
			laser->SetSpeed(LASER_SPEED);
			laser->SetTarget(target);
			laser->SetCollision(Laser::LASER_DEFAULT);
			laser->SetShadow();
		}

		if (m_Parent->GetLevel() * NUM_PER_LEVEL == m_AttackFrame / SHOT_PER_FRAME)
		{
			if (m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
		else
		{
			SetDamageArea();
		}
	}
}

void TutorialEnemy::StateLaser::SetDamageArea()
{
	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	dArea->SetPosition(SHOT_POS);

	GameObject* target = Manager::GetScene()->GetGameObject<Player>();
	dArea->SetLaserArea(target, SHOT_POS, DAMAGEAREA_SCALE, 60);
}

TutorialEnemy::StateFan::StateFan(TutorialEnemy* pEnemy)
{
	m_Parent = pEnemy;

	m_IsRight = (rand() % 2);

	if (m_IsRight) m_ShotRot = RIGHT_START_ROT;
	else m_ShotRot = LEFT_START_ROT;

	SetDamageArea();
}

void TutorialEnemy::StateFan::Update()
{
	m_AttackFrame++;

	//特別ホーミングはせずにレーザーをディレイを入れて乱射 方向は扇形
	if ((int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		m_LaserSE->Play(false, SE_VOLUME);

		Laser* laser = Manager::GetScene()->AddGameObject<Laser>(object);
		Vector3 pos = m_Parent->GetPosition();
		pos.y += SHOT_HEIGHT;
		laser->SetPosition(pos);

		Vector3 velocity = { cosf(m_ShotRot * RADIAN), 0.0f, sinf(m_ShotRot * RADIAN) };
		velocity.normalize();
		laser->SetVelocity(velocity);
		laser->SetSpeed(LASER_SPEED);
		laser->SetCollision(Laser::LASER_DEFAULT);
		laser->SetShadow();

		if (m_IsRight)
		{
			m_ShotRot += ADD_ROT_PER_SHOT;

			if (m_ShotRot == LEFT_START_ROT)
			{
				m_IsRight = false;
			}
		}
		else
		{
			m_ShotRot -= ADD_ROT_PER_SHOT;

			if (m_ShotRot == RIGHT_START_ROT)
			{
				m_IsRight = true;
			}
		}

		if (m_Parent->GetLevel() * NUM_PER_LEVEL == m_AttackFrame / SHOT_PER_FRAME)
		{
			if (m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
		else
		{
			SetDamageArea();
		}
	}
}

void TutorialEnemy::StateFan::SetDamageArea()
{
	Vector3 velocity = { cosf(m_ShotRot * RADIAN), 0.0f, sinf(m_ShotRot * RADIAN) };
	velocity.normalize();

	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	dArea->SetPosition(m_Parent->GetPosition());
	dArea->SetFanArea(m_Parent->GetPosition(), velocity, DAMAGEAREA_SCALE, SHOT_PER_FRAME);
}

TutorialEnemy::StateFront::StateFront(TutorialEnemy* pEnemy)
{
	m_Parent = pEnemy;

	SetDamageArea();
}

void TutorialEnemy::StateFront::Update()
{
	m_AttackFrame++;

	if ((int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		for (int i = 0; i < NUM_PER_LEVEL; i++)
		{
			Laser* laser = Manager::GetScene()->AddGameObject<Laser>(object);
			Vector3 basePos = m_Parent->GetPosition();

			basePos.x = float(BasePos - i);

			basePos.y += SHOT_HEIGHT;

			laser->SetPosition(basePos);

			Vector3 velocity = { 0.0f, 0.0f, -1.0f };
			velocity.normalize();
			laser->SetVelocity(velocity);
			laser->SetSpeed(LASER_SPEED);
			laser->SetCollision(Laser::LASER_SLOW);
			laser->SetShadow();
		}


		if (m_Parent->GetLevel() == m_AttackFrame / SHOT_PER_FRAME)
		{
			if (m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
	}
}

void TutorialEnemy::StateFront::SetDamageArea()
{
	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	Vector3 pos = { 0.0f,0.0f,0.0f };

	dArea->SetPosition(pos);
	dArea->SetScale(DAMAGEAREA_SCALE);
	dArea->SetRotation({ 0.0f,0.0f,0.0f });
	dArea->SetLifeTime(m_ShotPerFrame[A_Front]);
}

TutorialEnemy::StateStun::StateStun(TutorialEnemy* pEnemy)
{
	m_Parent = pEnemy;
}

void TutorialEnemy::StateStun::Update()
{

}