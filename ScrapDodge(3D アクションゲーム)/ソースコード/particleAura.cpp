/*============================================================
	[particleAura.cpp]
	▼題名 : ダメージパーティクル
	▼DATE : 2026/01/14
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "particleAura.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"

void ParticleAura::Init()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0,0 };

	vertex[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1,0 };

	vertex[2].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0,1 };

	vertex[3].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
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

	m_Life = PARTICLE_LIFE;

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].Enable = false;
	}

	m_Scale = PARTICLE_SCALE;
}

void ParticleAura::Update()
{
	int cnt = COUNT_PER_FRAME;

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == false)
		{
			m_Particle[i].Enable = true;
			m_Particle[i].Life = PARTICLE_LIFE / 10;
			m_Particle[i].Position.x = m_Position.x + ((rand() % PARTICLE_RANGE_X) - PARTICLE_RANGE_X / 2) / REDUCE_FACTOR_X;
			m_Particle[i].Position.y = m_Position.y + (rand() % PARTICLE_RANGE_Y) / REDUCE_FACTOR_Y;
			m_Particle[i].Position.z = m_Position.z + ((rand() % PARTICLE_RANGE_Z) - PARTICLE_RANGE_Z / 2) / REDUCE_FACTOR_Z;
			m_Particle[i].Velocity = PARTICLE_VELOCITY;

			cnt--;

			if(cnt == 0)
				break;
		}
	}

	Particle::Update();
}

void ParticleAura::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}