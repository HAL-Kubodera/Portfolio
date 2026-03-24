/*============================================================
	[particleCrash.cpp]
	▼題名 : クラッシュパーティクル
	▼DATE : 2026/01/24
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "particleCrash.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"

void ParticleCrash::Init()
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

void ParticleCrash::Update()
{
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == false)
		{
			m_Particle[i].Enable = true;

			m_Particle[i].Position = m_Position;

			//細かくするために大きめにとってあとから割る
			float x = float(rand() % DEFAULT_VEL - (DEFAULT_VEL / 2));
			x /= REDUCE_FACTOR;
			float y = float(rand() % DEFAULT_VEL);
			y /= REDUCE_FACTOR;
			float z = float(rand() % DEFAULT_VEL - (DEFAULT_VEL / 2));
			z /= REDUCE_FACTOR;

			m_Particle[i].Velocity = Vector3(x, y, z);
		}
	}

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			m_Particle[i].Velocity += PARTICLE_VELOCITY;

			m_Particle[i].Position += m_Particle[i].Velocity;
		}
	}

	m_Life--;

	if (m_Life == 0)
		SetDestroy();
}

void ParticleCrash::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}