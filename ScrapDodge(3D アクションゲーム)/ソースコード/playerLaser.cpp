/*============================================================
	[playerLaser.cpp]
	▼題名 : プレイヤーのレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "playerlaser.h"
#include "audio.h"

#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "renderer.h"
#include "texture.h"
#include "boxcollision.h"
#include "player.h"
#include "speedManager.h"
#include "pauseManager.h"
#include <vector>

ID3D11ShaderResourceView* PlayerLaser::m_Texture = nullptr;
ID3D11Buffer* PlayerLaser::m_VertexBuffer = nullptr;
ID3D11VertexShader* PlayerLaser::m_VertexShader = nullptr;
ID3D11PixelShader* PlayerLaser::m_PixelShader = nullptr;
ID3D11InputLayout* PlayerLaser::m_VertexLayout = nullptr;
bool PlayerLaser::m_IsLoad = false;
Audio* PlayerLaser::m_ShotSE = nullptr;

void PlayerLaser::Load()
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

	m_ShotSE = new Audio();
	m_ShotSE->Load("asset\\audio\\shot.wav");

	m_IsLoad = true;
}

void PlayerLaser::Init()
{
	if (!m_IsLoad) Load();

	m_IsDead = false;
	m_Life = 300;

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].Enable = false;
	}

	SetTag(Tag_Attack);
	m_IsCharge = false;
}

void PlayerLaser::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f);
	if (PauseManager::GetPause())return;

	Player* pPlayer = Manager::GetScene()->GetGameObject<Player>();

	if (!m_IsDead)
	{
		if (m_IsCharge)
		{
			//3コ描画してビームのように滑らかに
			for (int first = 0; first < PARTICLE_MAX; first++)
			{
				if (m_Particle[first].Enable == false)
				{
					m_Particle[first].Enable = true;
					m_Particle[first].Life = 10;
					m_Particle[first].Position = m_Position;
					m_Particle[first].Scale = m_Scale;
					m_Particle[first].Velocity = { 0.0f,0.0f,0.0f };

					for (int second = first; second < PARTICLE_MAX; second++)
					{
						if (m_Particle[second].Enable == false)
						{
							m_Particle[second].Enable = true;
							m_Particle[second].Life = 10;
							m_Particle[second].Position = m_Position + (m_Velocity / 3);
							m_Particle[second].Scale = m_Scale;
							m_Particle[second].Velocity = { 0.0f,0.0f,0.0f };

							for (int third = first; third < PARTICLE_MAX; third++)
							{
								if (m_Particle[third].Enable == false)
								{
									m_Particle[third].Enable = true;
									m_Particle[third].Life = 10;
									m_Particle[third].Position = m_Position + (m_Velocity / 3) * 2;
									m_Particle[third].Scale = m_Scale;
									m_Particle[third].Velocity = { 0.0f,0.0f,0.0f };

									for (int forth = third; forth < PARTICLE_MAX; forth++)
									{
										if (m_Particle[forth].Enable == false)
										{
											m_Particle[forth].Enable = true;
											m_Particle[forth].Life = 10;
											m_Particle[forth].Position = m_Position + (m_Velocity / 3) * 3;
											m_Particle[forth].Scale = m_Scale;
											m_Particle[forth].Velocity = { 0.0f,0.0f,0.0f };

											break;
										}
									}

									break;
								}
							}

							break;
						}
					}

					break;
				}
			}

			m_Position += m_Velocity * SpeedManager::GetSpeed();
			m_length += m_Velocity * SpeedManager::GetSpeed();
		}
		else
		{
			if (pPlayer != nullptr)
			{
				float scale = pPlayer->GetShotFrame();
				scale*= 3.0f;
				Vector3 position = pPlayer->GetPosition();
				position.y += 1;
				position.z += 1;

				m_Particle[0].Enable = true;
				m_Particle[0].Life = 10;
				m_Particle[0].Position = position;
				m_Particle[0].Scale = { scale,scale,scale };
				m_Particle[0].Velocity = { 0.0f,0.0f,0.0f };

				if (pPlayer->GetShotFrame() == 1.0f)
				{
					m_IsCharge = true;
					m_Position = position;
					SetTarget(m_Target);
					m_ShotSE->Play();
				}
				else if (pPlayer->GetShotFrame() == 0.0f)
				{
					SetDestroy();
				}
			}
		}
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
 
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			if (m_Particle[i].Position.x >= 25 || m_Particle[i].Position.z >= 25
				|| m_Particle[i].Position.x <= -25 || m_Particle[i].Position.z <= -25) SetDestroy();
		}
	}

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

void PlayerLaser::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	Camera* camera = Manager::GetScene()->GetGameObject<Camera>();
	XMMATRIX view = camera->GetViewMatrix();

	//ビューの逆行列
	XMMATRIX invView;
	invView = XMMatrixInverse(nullptr, view);//逆行列
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	//マテリアル設定
	MATERIAL material{};
	material.Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブとポロ時設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デプスバッファをオフ
	Renderer::SetDepthEnable(false);

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			XMMATRIX world, scale, trans;

			scale = XMMatrixScaling(m_Particle[i].Scale.x,
				m_Particle[i].Scale.y,
				m_Particle[i].Scale.z);

			trans = XMMatrixTranslation(
				m_Particle[i].Position.x,
				m_Particle[i].Position.y,
				m_Particle[i].Position.z);

			world = scale * invView * trans;

			Renderer::SetWorldMatrix(world);

			//ポリゴン描画
			Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
		}
	}

	Renderer::SetATCEnable(false);
	Renderer::SetDepthEnable(true);

	DrawComponent();
}

void PlayerLaser::Uninit()
{
	UninitComponent();
}

void PlayerLaser::SetTarget(GameObject* target)
{
	m_Target = target;

	m_Velocity = m_Target->GetPosition() - m_Position;
	m_Velocity.normalize();
}

void PlayerLaser::SetCollision()
{
	//コンポーネント
	BoxCollision* bc = AddComponent<BoxCollision>();
	bc->SetScale(1.0f, 1.0f, 9.0f);

	bc->SetLocalPosition(m_Velocity * -5);
	bc->SetPosition(m_Position + m_Velocity * -5);
	bc->SetRotation(0.0f, atan2f(m_Velocity.x, m_Velocity.z), 0.0f);
	bc->SetTargetTag(Tag_Spike);
	bc->Update();
}


