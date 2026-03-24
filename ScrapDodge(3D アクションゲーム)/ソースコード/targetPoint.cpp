/*============================================================
	[targetPoint.cpp]
	▼題名 : 攻撃目標の表示
	▼DATE : 2025/12/29
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "targetPoint.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "boxCollision.h"
#include "enemy.h"
#include "particleCrash.h"
#include "speedManager.h"

void TargetPoint::Init()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0.0f,0.0f };

	vertex[1].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1.0f,0.0f };

	vertex[2].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0.0f , 1.0f };

	vertex[3].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = { 1.0f, 1.0f };


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	m_Texture = Texture::Load("asset\\texture\\targetPoint.png");
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	BoxCollision* bc = AddComponent<BoxCollision>();
	bc->SetTargetTag(Tag_Attack);
}

void TargetPoint::Update()
{
	UpdateComponent();

	BoxCollision* bc = GetComponent<BoxCollision>();


	if (bc != nullptr)
	{
		std::vector<bool> isHit = bc->GetIsHit();

		if (isHit[0])
		{
			Enemy* pEnemy = Manager::GetScene()->GetGameObject<Enemy>();

			pEnemy->TakeDamage();

			Camera::Shake(CAMERA_SHAKE);
			SpeedManager::HitStop(HITSTOP);

			ParticleCrash* pParticleCrash = Manager::GetScene()->AddGameObject<ParticleCrash>(object);
			pParticleCrash->SetPosition(m_Position);
			pParticleCrash->SetColor({ 0.0f,1.0f,1.0f,1.0f });

			SetDestroy();
		}
	}
}

void TargetPoint::Draw()
{

	Renderer::SetDepthEnable(false);

	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float width = m_Scale.x / 2;
	float height = m_Scale.y / 2;

	vertex[0].Position = XMFLOAT3(-width, height, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0.0f,0.0f };

	vertex[1].Position = XMFLOAT3(width, height, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1.0f,0.0f };

	vertex[2].Position = XMFLOAT3(-width, -height, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0.0f , 1.0f };

	vertex[3].Position = XMFLOAT3(width, -height, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = { 1.0f, 1.0f };

	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

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
	material.Diffuse = m_Color;

	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	{
		XMMATRIX world, scale, trans;
		scale = XMMatrixScaling(TARGET_SCALE.x, TARGET_SCALE.y, TARGET_SCALE.z);
		trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		world = scale * invView * trans;

		Renderer::SetWorldMatrix(world);

		//テクスチャ設定
		Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

		//プリミティブとポロ時設定
		Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


		//ポリゴン描画
		Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
	}

	Renderer::SetDepthEnable(true);

	DrawComponent();
}

void TargetPoint::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}