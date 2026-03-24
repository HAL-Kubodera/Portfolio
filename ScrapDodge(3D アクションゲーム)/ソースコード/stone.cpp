/*============================================================
	[stone.cpp]
	▼題名 : 落ちてくる岩
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "audio.h"
#include "model.h"
#include "renderer.h"
#include "stone.h"
#include "manager.h"
#include "camera.h"
#include "enemy.h"
#include "boxCollision.h"
#include "speedManager.h"
#include "pauseManager.h"

MODEL_FBX* Stone::m_Model = nullptr;
ID3D11VertexShader* Stone::m_VertexShader = nullptr;
ID3D11PixelShader* Stone::m_PixelShader = nullptr;
ID3D11InputLayout* Stone::m_VertexLayout = nullptr;
bool Stone::m_IsLoad = false;
Audio* Stone::m_DropSE = nullptr;

void Stone::Load()
{
	m_Model = ModelLoad("asset\\model\\iwa.fbx", true);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitDepthTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitDepthTexturePS.cso");

	m_DropSE = new Audio();
	m_DropSE->Load("asset\\audio\\stone.wav");

	m_IsLoad = true;
}

void Stone::Init()
{
	if (!m_IsLoad) Load();

	SetTag(Tag_Spike);
}

void Stone::Update()
{
	if (Camera::m_BuildMode) return;
	if (SpeedManager::GetSpeed() == 0.0f)return;
	if (PauseManager::GetPause())return;

	if (m_Frame == 0) SetCollision();

	m_Frame++;

	if (m_Frame <= SHAKE_TIME)
	{
		float x = float(SHAKE_RANGE / 2 - (rand() % SHAKE_RANGE));
		x /= SHAKE_RANGE;

		float z = float(SHAKE_RANGE / 2 - (rand() % SHAKE_RANGE));
		z /= SHAKE_RANGE;

		m_Position.x = m_DefPos.x + x;
		m_Position.z = m_DefPos.z + z;

		if (m_Frame == SHAKE_TIME)
		{
			m_Position.x = m_DefPos.x;
			m_Position.z = m_DefPos.z;
		}
	}

	if (m_Frame <= ANIM_TIME)
	{
		float num = float(AI_MATH_PI / ANIM_FACTOR);
		float lerp = m_Frame / ANIM_TIME;

		lerp = powf(LERP_X, LERP_Y * lerp - LERP_Y) * sinf((lerp * LERP_Y - LERP_REDUCE) * num);

		m_Position.y = m_DefPos.y + lerp * (m_DefPos.y - m_Scale.y / 2);
	}

	if (m_Frame == ANIM_TIME)
		m_DropSE->Play(false, SE_VOLUME);

	if (m_Frame >= LIFE_TIME) SetDestroy();

	UpdateComponent();
}

void Stone::Draw()
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

	DrawComponent();
}

void Stone::Uninit()
{

}

void Stone::SetCollision()
{
	m_DefPos = m_Position;
	BoxCollision* bc = AddComponent<BoxCollision>();
	bc->SetPosition(m_Position);
	bc->SetScale(m_Scale);
}
