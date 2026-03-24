/*============================================================
	[fade.cpp]
	▼題名 : フェード画面
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "fade.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "player.h"

ID3D11ShaderResourceView* Fade::m_Texture;
ID3D11Buffer* Fade::m_VertexBuffer;
ID3D11VertexShader* Fade::m_VertexShader;
ID3D11PixelShader* Fade::m_PixelShader;
ID3D11InputLayout* Fade::m_VertexLayout;

XMFLOAT4 Fade::m_Color;

bool Fade::m_IsFadeIn;
bool Fade::m_IsActive;
bool Fade::m_ChengeScene;
int Fade::m_Frame;
int Fade::m_FadeFrame;

void Fade::Init()
{
	VERTEX_3D vertex[VER_NUM];

	float w = SCREEN_WIDTH;
	float h = SCREEN_HEIGHT;
	float x = 0.0f;
	float y = 0.0f;

	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

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

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	m_Texture = Texture::Load("asset\\texture\\white.png");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Frame = 0;
	m_FadeFrame = 0;
	m_IsFadeIn = false;
	m_IsActive = false;
	m_IsFadeIn = false;
	m_Color = { 0.0f,0.0f,0.0f,0.0f };
}

void Fade::Update()
{
	if (Camera::m_BuildMode) return;

	if (!m_IsActive)return;

	m_Frame++;

	if(m_IsFadeIn)
		m_Color = { 0.0f,0.0f,0.0f,float(m_Frame) / float(m_FadeFrame) };
	else 
		m_Color = { 0.0f,0.0f,0.0f,1.0f - (float(m_Frame) / float(m_FadeFrame)) };

	if (m_FadeFrame == m_Frame)
	{
		m_IsActive = false;
	}
}

void Fade::Draw()
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
	rot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	trans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	world = scale * rot * trans;

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

void Fade::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

void Fade::SetFadeIn(int frame)
{
	m_Frame = 0;
	m_FadeFrame = frame;
	m_IsFadeIn = true;
	m_IsActive = true;
	m_Color = { 0.0f,0.0f,0.0f,0.0f };
}

void Fade::SetFadeOut(int frame)
{
	m_Frame = 0;
	m_FadeFrame = frame;
	m_IsFadeIn = false;
	m_IsActive = true;
	m_Color = { 0.0f,0.0f,0.0f,1.0f };
}

void Fade::SetFadeScene(int frame)
{
	m_Frame = 0;
	m_FadeFrame = frame;
	m_IsFadeIn = false;
	m_IsActive = true;
	m_IsFadeIn = true;
	m_Color = { 0.0f,0.0f,0.0f,1.0f };
}
