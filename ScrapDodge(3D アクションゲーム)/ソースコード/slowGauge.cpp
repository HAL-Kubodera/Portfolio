/*============================================================
	[slowGauge.cpp]
	▼題名 : スロウゲージのUI
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "slowgauge.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "player.h"
#include "speedManager.h"
#include "text.h"

ID3D11ShaderResourceView* SlowGauge::m_Texture = nullptr;
ID3D11Buffer* SlowGauge::m_VertexBuffer = nullptr;
ID3D11VertexShader* SlowGauge::m_VertexShader = nullptr;
ID3D11PixelShader* SlowGauge::m_PixelShader = nullptr;
ID3D11InputLayout* SlowGauge::m_VertexLayout = nullptr;
bool SlowGauge::m_IsLoad = false;

void SlowGauge::Load()
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

	m_Texture = Texture::Load("asset\\texture\\hpBar.png");

	m_IsLoad = true;
}

void SlowGauge::Init()
{
	if (!m_IsLoad) Load();

	m_Player = Manager::GetScene()->GetGameObject<Player>();

	m_Position = SLOW_BAR_POS;
	m_Scale = SLOW_BAR_SIZE;

	m_MaxGauge = m_Player->SLOW_MAX_CHARGE;
	m_Gauge = m_MaxGauge;
	m_Color = DEFAULT_COLOR;

	Text* pText = AddComponent<Text>();
	pText->SetOffset(TEXT_OFFSET);
	pText->SetScale(TEXT_SIZE);
	pText->SetGap(TEXT_SIZE, TEXT_SIZE, TEXT_SIZE);
	pText->SetWard("SLOW[GAUGE");
}

void SlowGauge::Update()
{
	if (Camera::m_BuildMode) return;

	m_Gauge = m_Player->GetSlowCharge();

	UpdateComponent();
}

void SlowGauge::Draw()
{
	SetRenderer();

	DrawEmptyBar();
	DrawSlowGaugeBar();

	DrawComponent();
}

void SlowGauge::Uninit()
{

}

void SlowGauge::SetRenderer()
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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
}

void SlowGauge::DrawEmptyBar()
{
	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	float w = m_Scale.x;
	float h = m_Scale.y;
	float x = m_Position.x;
	float y = m_Position.y;

	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	vertex[0].Position = XMFLOAT3(x, y, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = EMPTY_COLOR;
	vertex[0].TexCoord = { tx,ty };

	vertex[1].Position = XMFLOAT3(x + w, y, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = EMPTY_COLOR;
	vertex[1].TexCoord = { tx + tw,ty };

	vertex[2].Position = XMFLOAT3(x, y + h, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = EMPTY_COLOR;
	vertex[2].TexCoord = { tx,ty + th };

	vertex[3].Position = XMFLOAT3(x + w, y + h, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = EMPTY_COLOR;
	vertex[3].TexCoord = { tx + tw, ty + th };

	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	//ポリゴン描画
	Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
}

void SlowGauge::DrawSlowGaugeBar()
{
	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//テクスチャの関係で空白部分少し盛る
	float u = START_UV + (m_Gauge / m_MaxGauge) * END_UV;

	float w = m_Scale.x * u;
	float h = m_Scale.y;
	float x = m_Position.x;
	float y = m_Position.y;

	float tw = u;
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
