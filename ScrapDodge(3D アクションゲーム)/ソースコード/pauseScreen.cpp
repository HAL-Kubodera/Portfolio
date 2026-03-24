/*
ファイル : pauseScreen.cpp
  名 前  : Shunsuke Kubodera
  日 付  : 2026/01/22
*/

#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "pausescreen.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "player.h"
#include "speedManager.h"

ID3D11ShaderResourceView* PauseScreen::m_Texture = nullptr;
ID3D11Buffer* PauseScreen::m_VertexBuffer = nullptr;
ID3D11VertexShader* PauseScreen::m_VertexShader = nullptr;
ID3D11PixelShader* PauseScreen::m_PixelShader = nullptr;
ID3D11InputLayout* PauseScreen::m_VertexLayout = nullptr;
bool PauseScreen::m_IsLoad = false;

void PauseScreen::Load()
{
	if (m_IsLoad)return;

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
	vertex[0].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.7f);
	vertex[0].TexCoord = { tx,ty };

	vertex[1].Position = XMFLOAT3(x + w, y, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.7f);
	vertex[1].TexCoord = { tx + tw,ty };

	vertex[2].Position = XMFLOAT3(x, y + h, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.7f);
	vertex[2].TexCoord = { tx,ty + th };

	vertex[3].Position = XMFLOAT3(x + w, y + h, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.7f);
	vertex[3].TexCoord = { tx + tw, ty + th };


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Texture = Texture::Load("asset\\texture\\white.png");
	
	m_IsLoad = true;
}

void PauseScreen::Init()
{
	if (!m_IsLoad) Load();

	m_Color = { 1.0f,0.0f,0.0f,0.0f };
}

void PauseScreen::Update()
{
	if (Camera::m_BuildMode) return;

}

void PauseScreen::Draw()
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

	//ポリゴン描画
	Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
}

void PauseScreen::Uninit()
{
}