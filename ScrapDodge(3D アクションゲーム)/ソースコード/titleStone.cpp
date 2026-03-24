/*============================================================
	[titleStone.cpp]
	▼題名 : タイトル画面で降ってくる石
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"

#include "model.h"
#include "renderer.h"
#include "titleStone.h"
#include "fade.h"
#include "manager.h"
#include "Titlecamera.h"
#include "enemy.h"

void TitleStone::Init()
{
	m_Model = ModelLoad("asset\\model\\iwa.fbx", true);
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitDepthTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitDepthTexturePS.cso");
}

void TitleStone::Update()
{
	if (TitleCamera::m_BuildMode) return;
	if (Fade::GetActive())return;

	if (m_DelayFrame != 0)
	{
		m_DelayFrame--;
		return;
	}

	if (m_Frame == 0)
	{
		m_Position = m_DefPos;
	}

	m_Frame++;

	float frame = m_Frame - MOVE_FRAME;

	if (frame <= SHAKE_FRAME)
	{
		float x = float(SHAKE_RANGE / 2 - (rand() % SHAKE_RANGE));
		x /= SHAKE_FACTOR;

		float z = float(SHAKE_RANGE / 2 - (rand() % SHAKE_RANGE));
		z /= SHAKE_FACTOR;

		m_Position.x = m_DefPos.x + x;
		m_Position.z = m_DefPos.z + z;

		if (m_Frame == SHAKE_FRAME)
		{
			m_Position.x = m_DefPos.x;
			m_Position.z = m_DefPos.z;
		}
	}

	if (frame <= ANIM_FRAME)
	{
		float num = float(AI_MATH_PI / 1.5f);
		float lerp = frame / ANIM_FRAME;

		lerp = powf(LERP_X, lerp * LERP_Y - LERP_Y) * sinf((lerp * LERP_Y - LERP_REDUCE) * num);

		m_Position.y = m_DefPos.y + lerp * (m_DefPos.y + 0.5f - m_Scale.y / 2);
	}

	if(m_IsMove)
		m_Position.x = m_DefPos.x + ((m_Frame) * 0.1f);

	if (frame >= LIFE_TIME) m_Frame = 0;

	UpdateComponent();
}

void TitleStone::Draw()
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

void TitleStone::Uninit()
{
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}
