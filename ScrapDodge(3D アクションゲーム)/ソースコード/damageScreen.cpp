/*============================================================
	[damageScreen.cpp]
	▼題名 : 被弾時の画面エフェクト
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "damagescreen.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "player.h"
#include "speedManager.h"

ID3D11ShaderResourceView* DamageScreen::m_Texture = nullptr;
ID3D11Buffer* DamageScreen::m_VertexBuffer = nullptr;
ID3D11VertexShader* DamageScreen::m_VertexShader = nullptr;
ID3D11PixelShader* DamageScreen::m_PixelShader = nullptr;
ID3D11InputLayout* DamageScreen::m_VertexLayout = nullptr;
bool DamageScreen::m_IsLoad = false;

float DamageScreen::m_Frame;
bool DamageScreen::m_IsDamage;

void DamageScreen::Load()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0,0 };

	vertex[1].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1,0 };

	vertex[2].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0,1 };

	vertex[3].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = { 1,1 };


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Texture = Texture::Load("asset\\texture\\screen.png");
	
	m_IsLoad = true;
}

void DamageScreen::Init()
{
	if (!m_IsLoad) Load();

	m_Color = { 1.0f,0.0f,0.0f,0.0f };
}

void DamageScreen::Update()
{
	if (Camera::m_BuildMode) return;

	if (!m_IsDamage)return;

	m_Frame--;
	m_Color.w = m_Frame / MAX_FRAME;

	if (m_Frame == 0)
	{
		m_IsDamage = false;
	}
}

void DamageScreen::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	Renderer::SetWorldViewProjection2D();

	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	trans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	world = scale * rot * trans;

	Renderer::SetWorldMatrix(world);

	//マテリアル設定
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);

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


	//根元のバー
	{
		//頂点データ書き換え
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		float w = SCREEN_WIDTH;
		float h = SCREEN_HEIGHT;
		float x = 0.0f;
		float y = 0.0f;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = XMFLOAT3(x, y, 0.0f);
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = m_Color;
		vertex[0].TexCoord = { tx,ty };

		vertex[1].Position = XMFLOAT3(x + w, y, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = m_Color;
		vertex[1].TexCoord = { tx + tw,ty };

		vertex[2].Position = XMFLOAT3(x, y + h, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = m_Color;
		vertex[2].TexCoord = { tx,ty + th };

		vertex[3].Position = XMFLOAT3(x + w, y + h, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = m_Color;
		vertex[3].TexCoord = { tx + tw, ty + th };

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

		//ポリゴン描画
		Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
	}
}

void DamageScreen::Uninit()
{
}

void DamageScreen::Damage()
{
	m_IsDamage = true;
	m_Frame = MAX_FRAME;
}
