/*============================================================
	[sky.cpp]
	▼題名 : スカイドーム
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "sky.h"

#include "manager.h"
#include "camera.h"
#include "model.h"

MODEL_FBX* Sky::m_Model;
ID3D11VertexShader* Sky::m_VertexShader;
ID3D11PixelShader* Sky::m_PixelShader;
ID3D11InputLayout* Sky::m_VertexLayout;
bool Sky::m_IsLoad;

void Sky::Load()
{
	m_Model = ModelLoad("asset\\model\\sky.fbx", true);
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_IsLoad = true;
}

void Sky::Init()
{
	if (!m_IsLoad) Load();

	m_Scale = DEFAULT_SCALE;
}

void Sky::Update()
{
	
}

void Sky::Draw()
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

	ModelDraw(m_Model);
}

void Sky::Uninit()
{
}
