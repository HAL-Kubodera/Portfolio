/*============================================================
	[laser.cpp]
	▼題名 : 敵のレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "laser.h"
#include "audio.h"

#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "titleCamera.h"
#include "renderer.h"
#include "texture.h"
#include "boxcollision.h"
#include "player.h"
#include "speedManager.h"
#include "pauseManager.h"
#include "shadow.h"
#include <vector>

ID3D11ShaderResourceView* Laser::m_Texture = nullptr;
ID3D11Buffer* Laser::m_VertexBuffer = nullptr;
ID3D11VertexShader* Laser::m_VertexShader = nullptr;
ID3D11PixelShader* Laser::m_PixelShader = nullptr;
ID3D11InputLayout* Laser::m_VertexLayout = nullptr;
bool Laser::m_IsLoad = false;

void Laser::Load()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex[0].TexCoord = { 0,0 };

	vertex[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex[1].TexCoord = { 1,0 };

	vertex[2].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex[2].TexCoord = { 0,1 };

	vertex[3].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex[3].TexCoord = { 1,1 };

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Texture = Texture::Load("asset\\texture\\Particle.png");

	m_IsLoad = true;
}

void Laser::Init()
{
	if (!m_IsLoad) Load();

	m_IsDead = false;

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].Enable = false;
	}

	SetTag(Tag_Damage);
}

void Laser::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f)return;
	if (PauseManager::GetPause()) return;

	if (!m_IsDead)
	{
		m_GenerateFrame += SpeedManager::GetSpeed();

		if (m_GenerateFrame >= 1.0f)
		{
			GenerateParticle();
			m_GenerateFrame -= 1.0f;
		}

		Vector3 velocity = m_Velocity * SpeedManager::GetSpeed();
		m_Position += velocity * m_Speed;
	}
	else
	{
		for (int i = 0; i < PARTICLE_MAX; i++)
		{
			if (m_Particle[i].Enable == true)
			{
				break;
			}
		}

		SetDestroy();
	}

	LifeCheck();
	StageCheck();

	UpdateComponent();

	BoxCollision* bc = GetComponent<BoxCollision>();


	if (bc != nullptr)
	{
		std::vector<bool> isHit = bc->GetIsHit();

		if (isHit[0])//壁
		{
			m_IsDead = true;
		}
	}
}

void Laser::Draw()
{

	DrawComponent();
}

void Laser::Uninit()
{
	UninitComponent();
}

void Laser::LifeCheck()
{
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			m_Particle[i].Life -= SpeedManager::GetSpeed();
			if (m_Particle[i].Life <= 0)
				m_Particle[i].Enable = false;
		}
	}
}

void Laser::StageCheck()
{
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			//範囲外処理
			if (m_Particle[i].Position.x >= STAGE_RANGE.x || m_Particle[i].Position.z >= STAGE_RANGE.z
				|| m_Particle[i].Position.x <= -STAGE_RANGE.x || m_Particle[i].Position.z <= -STAGE_RANGE.z
				|| m_Particle[i].Position.y <= STAGE_RANGE.y) SetDestroy();
		}
	}
}

void Laser::GenerateParticle()
{
	for (int num = 0; num < LASER_PARTICLE_NUM; num++)
	{
		for (int i = 0; i < PARTICLE_MAX; i++)
		{
			if (m_Particle[i].Enable == false)
			{
				m_Particle[i].Enable = true;
				m_Particle[i].Life = PARTICLE_LIFE;
				m_Particle[i].Position = m_Position + ((m_Velocity * m_Speed) / LASER_PARTICLE_NUM) * num;

				break;
			}
		}
	}
}

void Laser::SetTarget(GameObject* target)
{
	m_Target = target;

	m_Velocity = m_Target->GetPosition() - m_Position;
	m_Velocity.normalize();
}

void Laser::SetCollision(int n)
{
	if (n == LASER_DEFAULT)
	{
		//コンポーネント
		BoxCollision* bc = AddComponent<BoxCollision>();
		bc->SetScale(DEFAULT_COLLISION_SCALE.x, DEFAULT_COLLISION_SCALE.y, DEFAULT_COLLISION_SCALE.z * m_Speed);

		bc->SetLocalPosition(m_Velocity * DEFAULT_COLLISION_POS_FACTOR);
		bc->SetPosition(m_Position + m_Velocity * DEFAULT_COLLISION_POS_FACTOR);
		bc->SetRotation(0.0f, atan2f(m_Velocity.x, m_Velocity.z), 0.0f);
		bc->SetTargetTag(Tag_Spike);
		bc->Update();
	}
	else if (n == LASER_SLOW)
	{
		//コンポーネント
		BoxCollision* bc = AddComponent<BoxCollision>();
		bc->SetScale(SLOW_COLLISION_SCALE);

		bc->SetLocalPosition(-m_Velocity * SLOW_COLLISION_POS_FACTOR);
		bc->SetPosition(m_Position + -m_Velocity * SLOW_COLLISION_POS_FACTOR);
		bc->SetRotation(0.0f, atan2f(m_Velocity.x, m_Velocity.z), 0.0f);
		bc->SetTargetTag(Tag_Spike);
		bc->Update();
	}
}

void Laser::SetSpeed(float speed)
{
	m_Speed = speed;
}

void Laser::SetShadow()
{
	Shadow* pShadow = AddComponent<Shadow>();
	pShadow->SetParent(this);
	//サイズから参照するように変更
	pShadow->SetDefaultPos(-m_Velocity * 5 * m_Speed);

	Vector3 vel = m_Velocity;
	vel.normalize();
	Vector3 pos = { abs(vel.x) * SHADOW_SCALE.x * m_Speed,abs(vel.z) * SHADOW_SCALE.y * m_Speed,SHADOW_SCALE .z};
	pos += SHADOW_OFFSET;
	pShadow->SetDefaultScale(pos);
	pShadow->SetFreezeRot(false);
}


