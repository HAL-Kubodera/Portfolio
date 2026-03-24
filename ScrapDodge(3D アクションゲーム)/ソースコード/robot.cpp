/*============================================================
	[robot.cpp]
	▼題名 : タイトル画面のロボ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "robot.h"
#include "animationModel.h"
#include "audio.h"

#include "manager.h"
#include "scene.h"
#include "titlecamera.h"
#include "enemy.h"
#include "meshField.h"
#include "Boxcollision.h"
#include "Result.h"
#include "playerHp.h"
#include "playerLaser.h"

void Robot::Init()
{
	m_AnimationModel = new AnimationModel();
	m_AnimationModel->Load("asset\\model\\player_Run.fbx");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Run.fbx", "Idle");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Run.fbx", "Run");

	m_AnimationName = "Idle";
	m_AnimationNameNext = "Idle";

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Position = DEFAULT_POSITION;
	m_Scale = DEFAULT_SCALE;
	m_Rotation.y = -XM_PIDIV2;
}

void Robot::Update()
{
	if (TitleCamera::m_BuildMode) return;

	m_AnimationModel->Update(m_AnimationName.c_str(), m_Frame, m_AnimationNameNext.c_str(), m_Frame, m_AnimationBlend);

	m_Frame++;

	TitleCamera* camera = Manager::GetScene()->GetGameObject<TitleCamera>();

	Vector3 rotation = camera->GetRotation();

	if (m_Frame == FIRST_ANIM_FRAME)
	{
		Vector3 velocity = { 0.0f,0.0f,2.0f };
		velocity.y = 0;
		velocity.normalize();
		m_Velocity = velocity * WALK_VEL;
		m_IsRotate = true;
		m_OldDirection = m_Rotation.y;
		m_Direction = rotation.y + XM_PIDIV2;
	}

	if (m_Frame == SECOND_ANIM_FRAME)
	{
		Vector3 velocity = { 0.0f,0.0f,-2.0f };
		velocity.y = 0;
		velocity.normalize();
		m_Velocity = velocity * WALK_VEL;
		m_IsRotate = true;
		m_OldDirection = m_Rotation.y;
		m_Direction = rotation.y - XM_PIDIV2;
	}

	if (m_Frame == THIRD_ANIM_FRAME)
	{
		m_Velocity = { 0.0f,0.0f,0.0f };
		m_IsRotate = true;
		m_OldDirection = m_Rotation.y;
		m_Direction = rotation.y;
	}

	m_Position += m_Velocity;

	UpdateRotation();

	UpdateComponent();
}

void Robot::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	XMMATRIX parentMatrix;

	//親の描画
	{
		//マトリクス設定
		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		world = scale * rot * trans;
		parentMatrix = rot * trans;

		Renderer::SetWorldMatrix(world);

		m_AnimationModel->Draw();
	}

	DrawComponent();
}

void Robot::Uninit()
{
	UninitComponent();

	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

void Robot::SetAnimation(std::string animName)
{
	if (m_AnimationNameNext != animName)
	{
		m_AnimationName = m_AnimationNameNext;
		m_AnimationNameNext = animName;
		m_AnimationBlend = 0.0f;
	}
}

void Robot::UpdateRotation()
{
	if (m_IsRotate)
	{
		m_RotationFrame++;

		float delta = m_Direction - m_OldDirection;

		while (delta > XM_PI)  delta -= XM_2PI;
		while (delta < -XM_PI) delta += XM_2PI;

		float t = (float)m_RotationFrame / m_RotationFrameMax;
		m_Rotation.y = m_OldDirection + delta * t;

		if (m_RotationFrame == m_RotationFrameMax)
		{
			m_IsRotate = false;
			m_RotationFrame = 0;
		}
	}
}