/*============================================================
	[collision.cpp]
	▼題名 : 当たり判定
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "collision.h"
#include "model.h"
#include "manager.h"
#include "camera.h"

void Collision::Init()
{
	m_Model = ModelLoad("asset\\model\\collision.fbx", true);
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Collision::Update()
{
	if (m_Tag != m_Parent->GetTag())
		m_Tag = m_Parent->GetTag();

	for(int i =0;i<m_IsHit.size();i++)
	{
		m_IsHit[i] = false;
	}

	for (int i = 0; i < m_HitObjects.size(); i++)
	{
		m_HitObjects[i].clear();
	}

	m_Position = m_Parent->GetPosition() + m_localPosition;
}

void Collision::Draw()
{
#ifdef _DEBUG 
	if (!m_IsDisp) return;
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
#endif // _DEBUG 
}

void Collision::Uninit()
{
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

void Collision::SetTargetTag(Tag tag)
{
	m_TargetTag.push_back(tag);
	m_IsHit.push_back(false);
	m_HitObjects.push_back(std::vector<GameObject*>());
}