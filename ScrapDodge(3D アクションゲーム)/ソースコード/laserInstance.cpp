/*============================================================
	[laserInstance.cpp]
	▼題名 : 敵のレーザーのインスタンス描画
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "laserinstance.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"

#include "laser.h"

void LaserInstance::Init()
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

	m_Texture = Texture::Load("asset\\texture\\particle.png");

	//ストラクチャーバッファ
	{
		Vector3* position = new Vector3[LASER_NUM]{};

		for (int i = 0; i < LASER_NUM; i++)
		{
			position[i] = { 0.0f,0.0f,0.0f };
		}

		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Vector3) * LASER_NUM;
		bd.StructureByteStride = sizeof(Vector3);
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd{};
		sd.pSysMem = position;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_PositionBuffer);

		delete[] position;

		//シェーダリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		srvd.Format = DXGI_FORMAT_UNKNOWN;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvd.Buffer.FirstElement = 0;
		srvd.Buffer.NumElements = LASER_NUM;

		Renderer::GetDevice()->CreateShaderResourceView(m_PositionBuffer, &srvd, &m_PositionSRV);
	}

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\instanceVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\instancePS.cso");
}

void LaserInstance::Update()
{
	//ストラクチャーバッファ
	{
		Vector3* position = new Vector3[LASER_NUM]{};

		std::vector<Laser*> lasers = Manager::GetScene()->GetGameObjects<Laser>();

		m_LaserNum = 0;

		for (auto laser : lasers)
		{
			Laser::PARTICLE* particle = laser->GetParticle();

			for (int i = 0; i < laser->PARTICLE_MAX; i++)
			{
				if (m_LaserNum >= LASER_NUM) break;

				if (particle[i].Enable == true)
				{
					position[m_LaserNum] = { 0.0f,0.0f,0.0f };

					position[m_LaserNum].x = particle[i].Position.x;
					position[m_LaserNum].z = particle[i].Position.z;
					position[m_LaserNum].y = particle[i].Position.y;

					m_LaserNum++;
				}
			}
			
		}

		//頂点データ書き換え
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(m_PositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pos = (VERTEX_3D*)msr.pData;

		memcpy(pos, position, sizeof(Vector3) * LASER_NUM);

		Renderer::GetDeviceContext()->Unmap(m_PositionBuffer, 0);

		delete[] position;

		//シェーダリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		srvd.Format = DXGI_FORMAT_UNKNOWN;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvd.Buffer.FirstElement = 0;
		srvd.Buffer.NumElements = LASER_NUM;

		Renderer::GetDevice()->CreateShaderResourceView(m_PositionBuffer, &srvd, &m_PositionSRV);
	}
}

void LaserInstance::Draw()
{
	//画面外は描画しない
	Camera* camera = Manager::GetScene()->GetGameObject<Camera>();

	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	XMMATRIX view = camera->GetViewMatrix();

	//ビューの逆行列
	XMMATRIX invView;
	invView = XMMatrixInverse(nullptr, view);//逆行列
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	XMMATRIX world, scale, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * invView * trans;

	Renderer::SetWorldMatrix(world);

	//マテリアル設定
	MATERIAL material{};
	material.Diffuse = m_Color;

	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	Renderer::GetDeviceContext()->VSSetShaderResources(2, 1, &m_PositionSRV);

	//プリミティブとポロ時設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デプスバッファをオフ
	Renderer::SetDepthEnable(false);

	//ポリゴン描画
	Renderer::GetDeviceContext()->DrawInstanced(VER_NUM, m_LaserNum, 0,0);

	Renderer::SetDepthEnable(true);
}

void LaserInstance::Uninit()
{
	SAFE_RELEASE(m_PositionBuffer);
	SAFE_RELEASE(m_PositionSRV);

	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}