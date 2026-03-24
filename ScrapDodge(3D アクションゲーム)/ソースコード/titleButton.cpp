/*============================================================
	[titleButton.cpp]
	▼題名 : タイトル画面から遷移するためのボタン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "audio.h"
#include "text.h"
#include "renderer.h"
#include "polygon.h"
#include "titlebutton.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "fade.h"
#include "player.h"

void TitleButton::Init()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(-0.5f, 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0,0 };

	vertex[1].Position = XMFLOAT3(0.5f, 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1,0 };

	vertex[2].Position = XMFLOAT3(-0.5f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0,1 };

	vertex[3].Position = XMFLOAT3(0.5f, 1.0f, 0.0f);
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

	m_Texture = Texture::Load("asset\\texture\\button.png");
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_SelectSE = new Audio;
	m_SelectSE->Load("asset\\audio\\select.wav");
}

void TitleButton::Update()
{
	if (Camera::m_BuildMode) return;

	if (Keyboard_IsKeyDownTrigger(KK_UP))
	{
		if (m_SelectIndex != 0)
		{
			m_SelectIndex--;
		}
	}
	if (Keyboard_IsKeyDownTrigger(KK_DOWN))
	{
		if (m_SelectIndex != BUTTON_NUM - 1)
		{
			m_SelectIndex++;
		}
	}

	if (Keyboard_IsKeyDownTrigger(KK_ENTER))
	{
		if (Manager::GetScene()->GetIsFade() == true) return;

		if (m_SelectIndex == START)
		{
			Manager::GetScene()->SetIsFade(true);
			Fade::SetFadeIn(30);
			m_SelectSE->Play();
		}
		else if (m_SelectIndex == EXIT)
		{
			PostQuitMessage(0);
			m_SelectSE->Play();
		}
	}

	UpdateComponent();
}

void TitleButton::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	Renderer::SetWorldViewProjection2D();

	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
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

	float w = 0.0f;
	float h = 0.0f;
	float x = 0.0f;
	float y = BUTTON_POS.y;

	UpdateText();

	for (int i = 0; i < BUTTON_NUM; i++)
	{
		//根元のバー
		{
			//頂点データ書き換え
			D3D11_MAPPED_SUBRESOURCE msr;
			Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

			y += TEXT_GAP;
			x = BUTTON_POS.x;

			if (i == m_SelectIndex)
			{
				w = SELECT_SIZE.x;
				h = SELECT_SIZE.y;
			}
			else
			{
				w = DEFAULT_SIZE.x;
				h = DEFAULT_SIZE.y;
			}

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

			vertex[0].Position = XMFLOAT3(x - w / 2, y - h / 2, 0.0f);
			vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[0].Diffuse = color;
			vertex[0].TexCoord = { tx,ty };

			vertex[1].Position = XMFLOAT3(x + w / 2, y - h / 2, 0.0f);
			vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[1].Diffuse = color;
			vertex[1].TexCoord = { tx + tw,ty };

			vertex[2].Position = XMFLOAT3(x - w / 2, y + h / 2, 0.0f);
			vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[2].Diffuse = color;
			vertex[2].TexCoord = { tx,ty + th };

			vertex[3].Position = XMFLOAT3(x + w / 2, y + h / 2, 0.0f);
			vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[3].Diffuse = color;
			vertex[3].TexCoord = { tx + tw, ty + th };

			Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

			//ポリゴン描画
			Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
		}
	}

	DrawComponent();
}

void TitleButton::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);

	m_SelectSE->Uninit();
	delete m_SelectSE;
}

void TitleButton::SetText()
{
	m_Start = AddComponent<Text>();
	m_Start->SetWard("START");
	m_Start->SetFollow(false);

	m_Exit = AddComponent<Text>();
	m_Exit->SetWard("EXIT");
	m_Exit->SetFollow(false);
}

void TitleButton::UpdateText()
{
	float y = BUTTON_POS.y;

	if (m_SelectIndex == START)
	{
		m_Start->SetPosition(SCREEN_WIDTH - SELECT_SIZE.x + SELECT_POS.x, y + SELECT_POS.y, 0.0f);
		m_Start->SetScale(SELECT_TEXT_SIZE);
		m_Start->SetGap({ SELECT_TEXT_SIZE ,0.0f,0.0f });
	}
	else
	{
		m_Start->SetPosition(SCREEN_WIDTH - DEFAULT_SIZE.x + TEXT_POS.x, y + TEXT_POS.y, 0.0f);
		m_Start->SetScale(DEFAULT_TEXT_SIZE);
		m_Start->SetGap({ DEFAULT_TEXT_SIZE ,0.0f,0.0f });
	}

	if (m_SelectIndex == EXIT)
	{
		m_Exit->SetPosition(SCREEN_WIDTH - SELECT_SIZE.x + SELECT_POS.x + SELECT_TEXT_SIZE / 2, y + TEXT_GAP + SELECT_POS.y, 0.0f);
		m_Exit->SetScale(SELECT_TEXT_SIZE);
		m_Exit->SetGap({ SELECT_TEXT_SIZE ,0.0f,0.0f });
	}
	else
	{
		m_Exit->SetPosition(SCREEN_WIDTH - DEFAULT_SIZE.x + TEXT_POS.x + DEFAULT_TEXT_SIZE / 2, y + TEXT_GAP + TEXT_POS.y, 0.0f);
		m_Exit->SetScale(DEFAULT_TEXT_SIZE);
		m_Exit->SetGap({ DEFAULT_TEXT_SIZE ,0.0f,0.0f });
	}
}