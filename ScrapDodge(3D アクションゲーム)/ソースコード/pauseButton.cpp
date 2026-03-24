/*
ファイル : pauseButton.cpp
  名 前  : Shunsuke Kubodera
  日 付  : 2026/02/16
*/

#include "main.h"
#include "audio.h"
#include "text.h"
#include "renderer.h"
#include "polygon.h"
#include "pausebutton.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "fade.h"
#include "player.h"
#include "title.h"
#include "pauseManager.h"

void PauseButton::Init()
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

	SetText();
}

void PauseButton::Update()
{
	if (Camera::m_BuildMode) return;

	if (Manager::GetScene()->GetIsFade() == true && Fade::GetActive() == false)
	{
		if (m_SelectIndex == TITLE)
		{
			Manager::SetScene<Title>();
		}
	}

	if (Manager::GetScene()->GetIsFade() == true) return;

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
			PauseManager::ChangePause();
			m_SelectSE->Play();
		}
		else if (m_SelectIndex == OPTION)
		{
			PauseManager::ChangePause();
			m_SelectSE->Play();
		}
		else if (m_SelectIndex == TITLE)
		{
			Manager::GetScene()->SetIsFade(true);
			Fade::SetFadeIn(30);
			m_SelectSE->Play();
		}
	}

	UpdateComponent();
}

void PauseButton::Draw()
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

void PauseButton::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);

	m_SelectSE->Uninit();
	delete m_SelectSE;
}

void PauseButton::SetText()
{
	m_Start = AddComponent<Text>();
	m_Start->SetWard("START");
	m_Start->SetFollow(false);

	m_Option = AddComponent<Text>();
	m_Option->SetWard("OPTION");
	m_Option->SetFollow(false);

	m_Title = AddComponent<Text>();
	m_Title->SetWard("TITLE");
	m_Title->SetFollow(false);
}

void PauseButton::UpdateText()
{
	if (m_SelectIndex == START)
	{
		m_Start->SetPosition(TEXT_POS.x - SELECT_TEXT_SIZE * 2.5f, SELECT_POS.y, 0.0f);
		m_Start->SetScale(SELECT_TEXT_SIZE);
		m_Start->SetGap({ SELECT_TEXT_SIZE ,0.0f,0.0f });
	}
	else
	{
		m_Start->SetPosition(TEXT_POS.x - DEFAULT_TEXT_SIZE * 2.5f,  TEXT_POS.y, 0.0f);
		m_Start->SetScale(DEFAULT_TEXT_SIZE);
		m_Start->SetGap({ DEFAULT_TEXT_SIZE ,0.0f,0.0f });
	}

	if (m_SelectIndex == OPTION)
	{
		float size = SELECT_TEXT_SIZE - 5;

		m_Option->SetPosition(TEXT_POS.x - size * 3.0f, (TEXT_GAP * 1) + SELECT_POS.y, 0.0f);
		m_Option->SetScale(size);
		m_Option->SetGap({ size  ,0.0f,0.0f });
	}
	else
	{
		float size = DEFAULT_TEXT_SIZE - 5;

		m_Option->SetPosition(TEXT_POS.x - size * 3.0f, (TEXT_GAP * 1) + TEXT_POS.y, 0.0f);
		m_Option->SetScale(size);
		m_Option->SetGap({ size ,0.0f,0.0f });
	}

	if (m_SelectIndex == TITLE)
	{
		m_Title->SetPosition(TEXT_POS.x - SELECT_TEXT_SIZE * 2.5f, (TEXT_GAP * 2) + SELECT_POS.y, 0.0f);
		m_Title->SetScale(SELECT_TEXT_SIZE);
		m_Title->SetGap({ SELECT_TEXT_SIZE ,0.0f,0.0f });
	}
	else
	{
		m_Title->SetPosition(TEXT_POS.x - DEFAULT_TEXT_SIZE * 2.5f, (TEXT_GAP * 2) + TEXT_POS.y, 0.0f);
		m_Title->SetScale(DEFAULT_TEXT_SIZE);
		m_Title->SetGap({ DEFAULT_TEXT_SIZE ,0.0f,0.0f });
	}
}