/*============================================================
	[laserCylinder.cpp]
	▼題名 : 円柱状のレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"

#include "renderer.h"
#include "lasercylinder.h"

#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "enemy.h"
#include "boxCollision.h"
#include "particleCylinder.h"

#include "speedManager.h"
#include "pauseManager.h"

void LaserCylinder::Init()
{
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitDepthTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitDepthTexturePS.cso");

	m_Scale = { 0.0f,0.0f,0.0f };
	BoxCollision* bc = AddComponent<BoxCollision>();
	bc->SetScale(m_Scale);
	SetTag(Tag_Damage);
}

void LaserCylinder::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f)return;
	if (PauseManager::GetPause())return;

	if (!m_IsSpawn)
	{
		CheckIsSpawn();

		return;
	}

	m_Frame++;

	//時間をかけて大きくなる
	if (m_Frame <= SCALE_MAX_FRAME)
	{
		float scale = m_Frame / SCALE_MAX_FRAME;
		m_Scale = { scale,SCALE_HEIGHT,scale };

		BoxCollision* bc = GetComponent<BoxCollision>();
		bc->SetScale(m_Scale);

		m_Particle->SetDistance(scale / 2);
	}
	
	m_Particle->SetPosition(m_Position);

	if (m_Frame == LIFE_FRAME)
	{
		SetDestroy();
		m_Particle->SetDestroy();
	}

	UpdateComponent();
}

void LaserCylinder::Draw()
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

	DrawComponent();
}

void LaserCylinder::Uninit()
{
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

void LaserCylinder::CheckIsSpawn()
{
	m_Frame += SpeedManager::GetSpeed();

	float scaleRatio = ((float)m_Frame / Enemy::SHOT_PER_FRAME[A_Cylinder]);

	if (scaleRatio >= 1.0f)
	{
		m_IsSpawn = true;
		m_Frame = 0;

		m_Position.y = CYLINDER_BASE_HEIGHT;
		SetParticle();
	}
}

void LaserCylinder::SetParticle()
{
	m_Particle = Manager::GetScene()->AddGameObject<ParticleCylinder>(object);
	m_Particle->SetPosition(m_Position);
}
