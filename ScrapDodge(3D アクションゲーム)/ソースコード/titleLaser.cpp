/*============================================================
	[titleLaser.cpp]
	▼題名 : タイトル画面で飛び交っているレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "scene.h"
#include "texture.h"
#include "manager.h"
#include "titleCamera.h"
#include "titleLaser.h"
#include "laser.h"

void TitleLaser::Init()
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

	m_Texture = Texture::Load("asset\\texture\\Particle.png");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");


	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].Enable = false;
	}

	m_Frame = 0;
}

void TitleLaser::Update()
{
	if (TitleCamera::m_BuildMode)return;

	m_Frame++;

	if (m_Frame == m_MaxFrame)
	{
		m_Position = m_DefaultPosition;

		m_Frame = 0;
	}


	//3コ描画してビームのように滑らかに
	for (int first = 0; first < PARTICLE_MAX; first++)
	{
		if (m_Particle[first].Enable == false)
		{
			//m_Particle[first].Enable = true;
			m_Particle[first].Life = 10;
			m_Particle[first].Position = m_Position;
			m_Particle[first].Velocity = { 0.0f,0.0f,0.0f };

			for (int second = first; second < PARTICLE_MAX; second++)
			{
				if (m_Particle[second].Enable == false)
				{
					m_Particle[second].Enable = true;
					m_Particle[second].Life = 10;
					m_Particle[second].Position = m_Position + (m_Velocity / 3);
					m_Particle[second].Velocity = { 0.0f,0.0f,0.0f };

					for (int third = second; third < PARTICLE_MAX; third++)
					{
						if (m_Particle[third].Enable == false)
						{
							m_Particle[third].Enable = true;
							m_Particle[third].Life = 10;
							m_Particle[third].Position = m_Position + (m_Velocity / 3) * 2;
							m_Particle[third].Velocity = { 0.0f,0.0f,0.0f };

							for (int forth = third; forth < PARTICLE_MAX; forth++)
							{
								if (m_Particle[forth].Enable == false)
								{
									m_Particle[forth].Enable = true;
									m_Particle[forth].Life = 10;
									m_Particle[forth].Position = m_Position + (m_Velocity / 3) * 3;
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

	m_Position += m_Velocity;
	
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
}

void TitleLaser::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	TitleCamera* camera = Manager::GetScene()->GetGameObject<TitleCamera>();
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
	//Renderer::SetATCEnable(true);
	//Renderer::SetAddBlendEnable(true);

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			XMMATRIX world, scale, trans;
			scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
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

	//Renderer::SetAddBlendEnable(false);
	Renderer::SetATCEnable(false);
	Renderer::SetDepthEnable(true);
}

void TitleLaser::Uninit()
{

}
