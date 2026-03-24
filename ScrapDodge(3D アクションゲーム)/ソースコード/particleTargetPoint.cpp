/*============================================================
	[particleTargetPoint.cpp]
	▼題名 : ダメージパーティクル
	▼DATE : 2026/01/14
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "particleTargetPoint.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "speedManager.h"
#include "pauseManager.h"
#include "targetPoint.h"

void ParticleTargetPoint::Init()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0,0 };

	vertex[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1,0 };

	vertex[2].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0,1 };

	vertex[3].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
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

	m_Texture = Texture::Load("asset\\texture\\Particle.png");
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].Enable = false;
	}

	m_Scale = DEFAULT_SCALE;
}

void ParticleTargetPoint::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f)return;
	if (PauseManager::GetPause())return;

	for (int num = 0; num < ROOP_PARTICLE_NUM; num++)
	{
		float x = cos(m_Rot * RADIAN) * CIRCLE_RADIUS;
		float y = sin(m_Rot * RADIAN) * CIRCLE_RADIUS + CIRCLE_Y_OFFSET;
	

		for (int i = 0; i < PARTICLE_MAX; i++)
		{
			if (m_Particle[i].Enable == false)
			{
				m_Particle[i].Enable = true;
				m_Particle[i].Position = m_Position;
				m_Particle[i].Velocity = Vector3(x, y, DISTANCE);
				m_Particle[i].Life = PARTICLE_LIFE;

				break;
			}
		}

		m_Rot += ADD_ROT;
	}

	m_Rot++;

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			m_Particle[i].Position += m_Particle[i].Velocity;
			m_Particle[i].Life--;

			if (m_Particle[i].Life == 0)
				m_Particle[i].Enable = false;
		}
	}

	if (Manager::GetScene()->GetGameObject<TargetPoint>() == nullptr)
		SetDestroy();
}

void ParticleTargetPoint::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}