/*============================================================
	[enemy.cpp]
	▼題名 : 敵(ボス)
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "enemy.h"
#include "manager.h"
#include "scene.h"
#include "audio.h"

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
#include "targetPoint.h"
#include "particleTargetPoint.h"

ID3D11VertexShader* Enemy::m_VertexShader = nullptr;
ID3D11PixelShader* Enemy::m_PixelShader = nullptr;
ID3D11InputLayout* Enemy::m_VertexLayout = nullptr;
class ModelRenderer* Enemy::m_ModelRenderer = nullptr;
AnimationModel* Enemy::m_AnimationModel = nullptr;
bool Enemy::m_IsLoad = false;
Audio* Enemy::m_LaserSE = nullptr;
Audio* Enemy::m_CylinderSE = nullptr;
Audio* Enemy::m_DamageSE = nullptr;

void Enemy::Load()
{
	m_AnimationModel = new AnimationModel();
	m_AnimationModel->Load("asset\\model\\Enemy_Idle.fbx");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_LaserSE = new Audio;
	m_LaserSE->Load("asset\\audio\\shot.wav");
	m_CylinderSE = new Audio;
	m_CylinderSE->Load("asset\\audio\\cylinder.wav");
	m_DamageSE = new Audio;
	m_DamageSE->Load("asset\\audio\\damage.wav");

	m_IsLoad = true;
}

void Enemy::Init()
{
	if (!m_IsLoad) Load();

	m_AnimationName = "Idle";
	m_AnimationNameNext = "Idle";

	m_State[m_Level - 1] = new StateIdle(this, 0);
}

void Enemy::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f)return;
	if (PauseManager::GetPause()) return;

	if (m_Hp == 0)
	{
		SpeedManager::SetMode(Speed_Clear);
		SpeedManager::SetSpeed(0.0f);

		return;
	}

	for (int level = 0; level < m_Level; level++)
	{
		m_State[level]->Update();
	}
	
	UpdateComponent();
}

void Enemy::Draw()
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

void Enemy::Uninit()
{
}

void Enemy::TakeDamage()
{
	m_Hp -= DAMAGE_PER_SHOT;
	m_AttackNum = 0;
	m_WaitFrame = 0;

	for (int level = 0; level < m_Level; level++)
	{
		m_State[level]->ChangeState<StateIdle>();
	}

	m_Level++;

	m_State[m_Level - 1] = new StateIdle(this, m_Level - 1);

	m_DamageSE->Play(false, DAMAGE_SE_VOLUME);
}

void Enemy::StateIdle::Update()
{
	m_CoolTime -= SpeedManager::GetSpeed();

	if (m_CoolTime <= 0)
	{
		switch (m_Parent->GetMode(m_Index))
		{
		case A_Laser:
		{
			ChangeState<StateLaser>();

			break;
		}
		case A_Fan:
		{
			ChangeState<StateFan>();

			break;
		}
		case A_DropStone:
		{
			ChangeState<StateStone>();

			break;
		}
		case A_Front:
		{
			ChangeState<StateFront>();

			break;
		}
		break;
		case A_Side:
		{
			ChangeState<StateSide>();

			break;
		}
		break;
		case A_Cylinder:
		{
			ChangeState<StateCylinder>();

			break;
		}
		break;
		default:
			break;
		}
	}
	
}

Enemy::StateStun::StateStun(Enemy* pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;
}

void Enemy::StateStun::Update()
{
	if(!m_Parent->GetIsAttack() && Manager::GetScene()->GetGameObject<TargetPoint>() == nullptr)
	{
		TargetPoint* pTPoint = Manager::GetScene()->AddGameObject<TargetPoint>(object);
		pTPoint->SetPosition(TARGETPOINT_POS);

		ParticleTargetPoint* pParticleCrash = Manager::GetScene()->AddGameObject<ParticleTargetPoint>(object);
		pParticleCrash->SetPosition({ TARGETPOINT_POS });
		pParticleCrash->SetColor({ 0.0f,1.0f,1.0f,1.0f });
	}
}

//レーザーコウゲキ プレイヤーに向けてレベル毎に3っつのレーザーを発射する
Enemy::StateLaser::StateLaser(Enemy * pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;

	SetDamageArea();
}

void Enemy::StateLaser::Update()
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
			m_Parent->AddAttackNum();

			if(m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
		else
		{
			SetDamageArea();
		}
	}
}

void Enemy::StateLaser::SetDamageArea()
{
	GameObject* target = Manager::GetScene()->GetGameObject<Player>();

	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	dArea->SetPosition(SHOT_POS);
	dArea->SetLaserArea(target, SHOT_POS, DAMAGEAREA_SCALE, SHOT_PER_FRAME);
}

//扇状のレーザー 右か左からレーザーを発射 レベル分往復する
Enemy::StateFan::StateFan(Enemy* pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;

	m_IsRight = (rand() % 2);

	if (m_IsRight) m_ShotRot = RIGHT_START_ROT;
	else m_ShotRot = LEFT_START_ROT;

	SetDamageArea();
}

void Enemy::StateFan::Update()
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
			m_Parent->AddAttackNum();

			if (m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
		else
		{
			SetDamageArea();
		}
	}
}

void Enemy::StateFan::SetDamageArea()
{
	Vector3 velocity = { cosf(m_ShotRot * RADIAN), 0.0f, sinf(m_ShotRot * RADIAN) };
	velocity.normalize();

	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	dArea->SetPosition(m_Parent->GetPosition());
	dArea->SetFanArea(m_Parent->GetPosition(), velocity, DAMAGEAREA_SCALE, SHOT_PER_FRAME);
}


Enemy::StateStone::StateStone(Enemy* pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;
}

void Enemy::StateStone::Update()
{
	m_AttackFrame++;

	if (m_Attack == nullptr && (int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		for (int i = 0; i < m_Parent->GetLevel() * NUM_PER_LEVEL; i++)
		{
			float x = float((STONE_SPAWN_RANGE / 2) - rand() % STONE_SPAWN_RANGE);
			float z = float((STONE_SPAWN_RANGE / 2) - rand() % STONE_SPAWN_RANGE);
			Vector3 stonePos = { x, STONE_HEIGHT, z };

			m_Attack = Manager::GetScene()->AddGameObject<Stone>(object);
			m_Attack->SetPosition(stonePos);
			m_Attack->SetScale(STONE_SCALE);
			stonePos.y = 0.0f;

			SetDamageArea(stonePos);
		}
	}

	if (m_Attack != nullptr && m_Attack->GetDestroy())
	{
		m_Parent->AddAttackNum();
		if (m_Parent->GetIsStun()) ChangeState<StateStun>();
		else ChangeState<StateIdle>();
	}
}

void Enemy::StateStone::SetDamageArea(Vector3 stonePos)
{
	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	dArea->SetPosition(stonePos);
	dArea->SetScale(DAMAGEAREA_SCALE);
	dArea->SetLifeTime((int)Stone::ANIM_TIME);
	dArea->SetMode(A_DropStone);
}

Enemy::StateFront::StateFront(Enemy* pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;

	m_IsRight = rand() % 2;

	SetDamageArea();
}

void Enemy::StateFront::Update()
{
	m_AttackFrame++;

	if ((int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		for (int i = 0; i < NUM_PER_LEVEL; i++)
		{
			Laser* laser = Manager::GetScene()->AddGameObject<Laser>(object);
			Vector3 basePos = m_Parent->GetPosition();

			if (m_IsRight) basePos.x = (float)i;
			else basePos.x = -(float)i;

			basePos.y += SHOT_HEIGHT;

			laser->SetPosition(basePos);

			Vector3 velocity = LASER_VELOCITY;
			velocity.normalize();
			laser->SetVelocity(velocity);
			laser->SetSpeed(LASER_SPEED);
			laser->SetCollision(Laser::LASER_SLOW);
			laser->SetShadow();
		}


		if (m_Parent->GetLevel() == m_AttackFrame / SHOT_PER_FRAME)
		{
			m_Parent->AddAttackNum();

			if (m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
	}
}

void Enemy::StateFront::SetDamageArea()
{
	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	Vector3 pos;

	if (m_IsRight) pos = RIGHT_DAMAGE_AREA_POS;
	else pos = LEFT_DAMAGE_AREA_POS;

	dArea->SetPosition(pos);
	dArea->SetScale(DAMAGE_AREA_SCALE);
	dArea->SetRotation({ 0.0f,0.0f,0.0f });
	dArea->SetLifeTime(SHOT_PER_FRAME);
}

Enemy::StateSide::StateSide(Enemy* pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;

	m_IsFront = rand() % 2;

	SetDamageArea();
}

void Enemy::StateSide::Update()
{
	m_AttackFrame++;

	if ((int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		bool right = rand() % 2;

		for (int i = 0; i < NUM_PER_LEVEL; i++)
		{
			Laser* laser = Manager::GetScene()->AddGameObject<Laser>(object);
			Vector3 basePos;

			if (right) basePos = RIGHT_LASER_POS;
			else  basePos = LEFT_LASER_POS;

			if (m_IsFront) basePos.z = (float)i;
			else basePos.z = -(float)i;

			basePos.y += SHOT_HEIGHT;

			laser->SetPosition(basePos);

			Vector3 velocity;

			if (right) velocity = RIGHT_LASER_VELOCITY;
			else  velocity = LEFT_LASER_VELOCITY;

			velocity.normalize();
			laser->SetVelocity(velocity);
			laser->SetSpeed(LASER_SPEED);
			laser->SetCollision(Laser::LASER_SLOW);
			laser->SetShadow();
		}


		if (m_Parent->GetLevel() == m_AttackFrame / SHOT_PER_FRAME)
		{
			m_Parent->AddAttackNum();

			if (m_Parent->GetIsStun()) ChangeState<StateStun>();
			else ChangeState<StateIdle>();
		}
	}
}

void Enemy::StateSide::SetDamageArea()
{
	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	Vector3 pos;

	//奥か手前か
	if (m_IsFront) pos = FRONT_DAMAGE_AREA_POS;
	else pos = BACK_DAMAGE_AREA_POS;

	dArea->SetPosition(pos);
	dArea->SetScale(DAMAGE_AREA_SCALE);
	dArea->SetRotation({ 0.0f,XM_PIDIV2,0.0f });
	dArea->SetLifeTime(SHOT_PER_FRAME);
}

Enemy::StateCylinder::StateCylinder(Enemy* pEnemy, int index)
{
	m_Parent = pEnemy;
	m_Index = index;
}

void Enemy::StateCylinder::Update()
{
	m_AttackFrame++;

	if (m_Attack == nullptr && (int)m_AttackFrame % SHOT_PER_FRAME == 0)
	{
		m_CylinderSE->Play(false, SE_VOLUME);

		for (int i = 0; i < m_Parent->GetLevel() * NUM_PER_LEVEL; i++)
		{
			float x = float(CYLINDER_SPAWN_RANGE_X / 2 - rand() % CYLINDER_SPAWN_RANGE_X);
			float z = float(CYLINDER_SPAWN_RANGE_Y / 2 - rand() % CYLINDER_SPAWN_RANGE_Y);
			Vector3 basePos = { x, 0.0f, z };

			DamageArea* dArea = SetDamageArea(basePos);

			m_Attack = Manager::GetScene()->AddGameObject<LaserCylinder>(object);

			m_Attack->SetPosition(basePos);
			m_Attack->SetDamageArea(dArea);
		}
	}

	if (m_Attack != nullptr && m_Attack->GetDestroy())
	{
		m_Parent->AddAttackNum();
		if (m_Parent->GetIsStun()) ChangeState<StateStun>();
		else ChangeState<StateIdle>();
	}
}

DamageArea* Enemy::StateCylinder::SetDamageArea(Vector3 basePos)
{
	DamageArea* dArea = Manager::GetScene()->AddGameObject<DamageArea>(area);
	dArea->SetPosition(basePos);
	dArea->SetScale(DAMAGE_AREA_SCALE);
	dArea->SetLifeTime(SHOT_PER_FRAME);
	dArea->SetMode(A_Cylinder);

	return dArea;
}