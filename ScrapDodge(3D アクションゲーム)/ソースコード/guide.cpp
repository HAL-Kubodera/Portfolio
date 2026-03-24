/*============================================================
	[guide.cpp]
	▼題名 : チュートリアルのガイド
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "guide.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"
#include "player.h"
#include "speedManager.h"
#include "mouseManager.h"
#include "text.h"

ID3D11ShaderResourceView* Guide::m_FaceTexture[Guide_Face_Max] = {};
ID3D11ShaderResourceView* Guide::m_ChatBoxTexture = nullptr;
ID3D11ShaderResourceView* Guide::m_CurtainTexture = nullptr;
ID3D11Buffer* Guide::m_VertexBuffer = nullptr;
ID3D11VertexShader* Guide::m_VertexShader = nullptr;
ID3D11PixelShader* Guide::m_PixelShader = nullptr;
ID3D11InputLayout* Guide::m_VertexLayout = nullptr;

bool Guide::m_IsLoad = false;

void Guide::Load()
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

	m_FaceTexture[Guide_Face_Default] = Texture::Load("asset\\texture\\playerIcon_Default.png");
	m_FaceTexture[Guide_Face_Smile] = Texture::Load("asset\\texture\\playerIcon_Smile.png");
	m_FaceTexture[Guide_Face_Sad] = Texture::Load("asset\\texture\\playerIcon_Sad.png");
	m_ChatBoxTexture = Texture::Load("asset\\texture\\chatBox.png");
	m_CurtainTexture = Texture::Load("asset\\texture\\white.png");

	m_IsLoad = true;
}

void Guide::Init()
{
	if (!m_IsLoad) Load();

	m_Text = AddComponent<Text>();
	m_Text->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	m_Text->SetFollow(false);
	m_Text->SetScale(TEXT_SCALE);
	m_Text->SetGap({ TEXT_SCALE,TEXT_SCALE,TEXT_SCALE });
	m_Text->SetPosition(TEXT_POS);
	m_Text->SetReturnNum(TEXT_RETURN_NUM);

	m_Enter = AddComponent<Text>();
	m_Enter->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
	m_Enter->SetWard("PRESS[ANY[KEY");
	m_Enter->SetFollow(false);
	m_Enter->SetScale(TEXT_SCALE);
	m_Enter->SetGap({ TEXT_SCALE,TEXT_SCALE,TEXT_SCALE });
	m_Enter->SetPosition(ENTER_POS);
}

void Guide::Update()
{
	if (Camera::m_BuildMode) return;

	m_Frame++;

	int dispNum = int(m_Frame / NUM_PER_FRAME);
	m_Text->SetDispNum(dispNum);

	if (Keyboard_IsAnyKeyTrigger())
	{
		if (dispNum >= m_WardNum)
		{
			m_IsDisp = false;
			SpeedManager::SetSpeed(1.0f);
		}
		else
		{
			m_Frame += DISP_NUM;
		}
	}

	if (m_IsDisp)
	{
		if (m_IsAlpha)
		{
			m_AlphaFrame--;

			if (m_AlphaFrame == 0)
			{
				m_IsAlpha = false;
			}
		}
		else
		{
			m_AlphaFrame++;

			if (m_AlphaFrame == ALPHA_MAX_FRAME)
			{
				m_IsAlpha = true;
			}
		}

		m_Enter->SetColor({ 1.0f,1.0f,1.0f, m_AlphaFrame / ALPHA_MAX_FRAME });
	}

	UpdateComponent();
}

void Guide::Draw()
{
	if (!m_IsDisp)return;
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


	//プリミティブとポロ時設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	DrawCurtain();
	DrawCharacter();
	DrawChatBox();

	DrawComponent();
}

void Guide::Uninit()
{

}

void Guide::DrawCurtain()
{
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_CurtainTexture);

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

	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	//ポリゴン描画
	Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
}

void Guide::DrawCharacter()
{
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_FaceTexture[m_Face]);

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	float w = CHARA_SCALE.x;
	float h = CHARA_SCALE.y;
	float x = CHARA_POS.x;
	float y = CHARA_POS.y;

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

void Guide::DrawChatBox()
{
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_ChatBoxTexture);

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	float w = CHATBOX_SCALE.x;
	float h = CHATBOX_SCALE.y;
	float x = CHATBOX_POS.x;
	float y = CHATBOX_POS.y;

	float tw = -1.0f;
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

//挨拶
void Guide::MoveExplain()
{
	m_Text->SetWard("まずはテキからのコウゲキをWASDのイドウでよけてみよう");
	m_Text->SetDispNum(0);
	m_WardNum = m_Text->GetWardNum();
	m_Face = Guide_Face_Default;
	m_IsDisp = true;
	m_Frame = 0;
}

void Guide::JumpExplain()
{
	m_Text->SetWard("つぎはSPACEのジャンプとイドウをくみあわせてよけてみよう");
	m_Text->SetDispNum(0);
	m_WardNum = m_Text->GetWardNum();
	m_Face = Guide_Face_Smile;
	m_IsDisp = true;
	m_Frame = 0;
}

void Guide::SlowExplain()
{
	m_Text->SetWard("つぎはみぎマウスボタンをながおししてスロウモードをハツドウしよう");
	m_Text->SetDispNum(0);
	m_WardNum = m_Text->GetWardNum();
	m_Face = Guide_Face_Smile;
	m_IsDisp = true;
	m_Frame = 0;
}

void Guide::ShotExplain()
{
	m_Text->SetWard("さいごにつかれてスタンしたテキをひだりマウスボタンでコウゲキしよう");
	m_Text->SetDispNum(0);
	m_WardNum = m_Text->GetWardNum();
	m_Face = Guide_Face_Smile;
	m_IsDisp = true;
	m_Frame = 0;
}

void Guide::GameExplain()
{
	m_Text->SetWard("チュートリアルはここまで！ゲームをたのしんでね");
	m_Text->SetDispNum(0);
	m_WardNum = m_Text->GetWardNum();
	m_Face = Guide_Face_Smile;
	m_IsDisp = true;
	m_Frame = 0;
}

void Guide::RetryExplain()
{
	m_Text->SetWard("おしい！もういちどチャレンジしてみよう");
	m_Text->SetDispNum(0);
	m_WardNum = m_Text->GetWardNum();
	m_Face = Guide_Face_Sad;
	m_IsDisp = true;
	m_Frame = 0;
}

bool Guide::GetIsFinish()
{
	return m_IsDisp;
}

