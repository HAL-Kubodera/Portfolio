/*============================================================
	[wall.cpp]
	▼題名 : フィールドの壁
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "wall.h"
#include "model.h"
#include "manager.h"
#include "camera.h"
#include "boxCollision.h"

MODEL_FBX* Wall::m_Model;

ID3D11VertexShader* Wall::m_VertexShader;
ID3D11PixelShader* Wall::m_PixelShader;
ID3D11InputLayout* Wall::m_VertexLayout;
bool Wall::m_IsLoad;

void Wall::Load()
{
	m_Model = ModelLoad("asset\\model\\iwa.fbx", true);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitDepthTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitDepthTexturePS.cso");

	m_IsLoad = true;
}

void Wall::Init()
{
	if (!m_IsLoad)Load();

	SetModelColor(m_Model, m_Color.x, m_Color.y, m_Color.z, m_Color.w);

	AddComponent<BoxCollision>();

	SetTag(Tag_Wall);
}

void Wall::Update()
{
	BoxCollision* bc = GetComponent<BoxCollision>();
	bc->SetPosition(m_Position);
	bc->SetScale(m_Scale);
	bc->SetRotation(m_Rotation);

	UpdateComponent();
}

void Wall::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;

	Renderer::SetWorldMatrix(world);

	DebugSelectColor();

	ModelDraw(m_Model);

	DrawComponent();
}

void Wall::Uninit()
{
}

void Wall::DebugSelectColor()
{
#ifdef _DEBUG
	if (m_ShowInspecter)
	{
		if (m_DebugFrame < 10)
		{
			m_DebugFrame++;

			SetModelColor(m_Model, m_Color.x, m_Color.y, m_Color.z, m_Color.w);
		}
		else if (m_DebugFrame < 20)
		{
			m_DebugFrame++;

			SetModelColor(m_Model, m_Color.x, m_Color.y, m_Color.z, 0.6f);

			if (m_DebugFrame == 20)
				m_DebugFrame = 0;
		}
	}
#endif // DEBUG
}
