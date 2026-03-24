/*============================================================
	[playerHp.cpp]
	▼題名 : プレイヤーの体力UI
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "playerhp.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "player.h"
#include "text.h"

ID3D11ShaderResourceView* PlayerHp::m_Texture = nullptr;
ID3D11Buffer* PlayerHp::m_VertexBuffer = nullptr;
ID3D11VertexShader* PlayerHp::m_VertexShader = nullptr;
ID3D11PixelShader* PlayerHp::m_PixelShader = nullptr;
ID3D11InputLayout* PlayerHp::m_VertexLayout = nullptr;
bool PlayerHp::m_IsLoad = false;

void PlayerHp::Load()
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

void PlayerHp::Init()
{
	if (!m_IsLoad) Load();

	m_Position = HP_BAR_POS;
	m_Scale = HP_BAR_SIZE;

	m_Player = Manager::GetScene()->GetGameObject<Player>();
	m_MaxHp = (float)m_Player->f_MaxHp;
	m_Hp = m_MaxHp;
	m_DispHp = m_MaxHp;
	m_Color = DEFAULT_COLOR;

	Text* pText = AddComponent<Text>();
	pText->SetOffset(TEXT_OFFSET);
	pText->SetScale(TEXT_SIZE);
	pText->SetGap(TEXT_SIZE, TEXT_SIZE, TEXT_SIZE);
	pText->SetWard("PLAYER[HP");
}

void PlayerHp::Update()
{
	if (Camera::m_BuildMode) return;

	float hp = (float)m_Player->GetHp();

	if (m_Hp != hp)
	{
		m_Hp = hp;
		m_Frame = 0;
		m_Diff = m_Hp - m_DispHp;
		m_Diff /= REDUCE_ANIM_FRAME;
	}

	if (m_DispHp != m_Hp)
	{
		m_Frame++;

		if (m_Frame > DISP_HP_FRAME)
		{
			m_DispHp += m_Diff;

			if (m_Frame == DISP_HP_FRAME + REDUCE_ANIM_FRAME)
			{
				m_DispHp = m_Hp;
				m_Frame = 0;
			}
		}
	}

	if (m_DispHp / m_MaxHp <= PINCH_HP_RATIO)
	{
		m_Color = PINCH_COLOR;
	}

	UpdateComponent();
}

void PlayerHp::Draw()
{
	SetRenderer();
	DrawEmptyBar();
	DrawReduceBar();
	DrawHpBar();

	DrawComponent();
}

void PlayerHp::Uninit()
{
}

void PlayerHp::SetRenderer()
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

void PlayerHp::DrawEmptyBar()
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

void PlayerHp::DrawReduceBar()
{
	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//テクスチャの関係で空白部分少し盛る
	float u = START_UV + (m_DispHp / m_MaxHp) * END_UV;

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
	vertex[0].Diffuse = REDUCE_COLOR;
	vertex[0].TexCoord = { tx,ty };

	vertex[1].Position = XMFLOAT3(x + w, y, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = REDUCE_COLOR;
	vertex[1].TexCoord = { tx + tw,ty };

	vertex[2].Position = XMFLOAT3(x, y + h, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = REDUCE_COLOR;
	vertex[2].TexCoord = { tx,ty + th };

	vertex[3].Position = XMFLOAT3(x + w, y + h, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = REDUCE_COLOR;
	vertex[3].TexCoord = { tx + tw, ty + th };

	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	//ポリゴン描画
	Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
}

void PlayerHp::DrawHpBar()
{
	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//テクスチャの関係で空白部分少し盛る
	float u = START_UV + (m_Hp / m_MaxHp) * END_UV;

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
