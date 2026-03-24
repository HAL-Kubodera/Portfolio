/*============================================================
	[damageArea.cpp]
	▼題名 : ダメージエリア
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "damagearea.h"
#include "pauseManager.h"

#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "enemy.h"
#include "stone.h"
#include "laserCylinder.h"

MODEL_FBX* DamageArea::m_Model = nullptr;
MODEL_FBX* DamageArea::m_ModelDeep = nullptr;
ID3D11VertexShader* DamageArea::m_VertexShader = nullptr;
ID3D11PixelShader* DamageArea::m_PixelShader = nullptr;
ID3D11InputLayout* DamageArea::m_VertexLayout = nullptr;
bool DamageArea::m_IsLoad = false;

void DamageArea::Load()
{
	m_Model = ModelLoad("asset\\model\\damageArea.fbx", true);
	m_ModelDeep = ModelLoad("asset\\model\\damageAreaDeep.fbx", true);
	SetModelColor(m_Model, 1.0f, 1.0f, 1.0f, 0.18f);
	SetModelColor(m_ModelDeep, 1.0f, 1.0f, 1.0f, 1.0f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitDepthTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitDepthTexturePS.cso");

	m_IsLoad = true;
}

void DamageArea::Init()
{
	if (!m_IsLoad) Load();

	m_lifeTime = 0;
}

void DamageArea::Update()
{
	if (Camera::m_BuildMode) return;
	if (PauseManager::GetPause()) return;

	m_lifeTime--;

	if (m_lifeTime == 0)
	{
		SetDestroy();
	}

	switch (m_Mode)
	{
	case A_Laser:
		if (m_Target != nullptr)
		{
			m_Velocity = m_Target->GetPosition() - m_EnemyPos;
			m_Velocity.normalize();

			SetRotation(atan2f(m_Velocity.y, m_Velocity.z), atan2f(m_Velocity.x, m_Velocity.z), 0.0f);
			SetPosition(m_EnemyPos + m_Velocity * (m_Scale.z / 2));
		}

		break;
	case A_Fan:
		
		break;
	case A_DropStone:

		break;
	case A_Cylinder:
	{
		
		break;
	}
	default:
		break;
	}
}

void DamageArea::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
	
	switch (m_Mode)
	{
	case A_Laser:
		{
			//マトリクス設定
			XMMATRIX world, scale, rot, trans;
			scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
			rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
			trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
			world = scale * rot * trans;

			Renderer::SetWorldMatrix(world);

			SetModelColor(m_Model, 1.0f, 1.0f, 1.0f, 0.18f);

			ModelDraw(m_Model);
		}

		break;
	case A_Fan:
		{
			//マトリクス設定
			XMMATRIX world, scale, rot, trans;
			scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
			rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
			trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
			world = scale * rot * trans;

			Renderer::SetWorldMatrix(world);

			ModelDraw(m_Model);
		}

		break;
	case A_DropStone:

		{
			XMMATRIX world, scale, rot, trans;
			scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
			rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
			trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
			world = scale * rot * trans;

			Renderer::SetWorldMatrix(world);

			ModelDraw(m_Model);
		}
		{
			float scaleRatio = 1.0f - ((float)m_lifeTime / Stone::ANIM_TIME);

			XMMATRIX world, scale, rot, trans;
			scale = XMMatrixScaling(m_Scale.x * scaleRatio, m_Scale.y, m_Scale.z * scaleRatio);
			rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
			trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
			world = scale * rot * trans;

			Renderer::SetWorldMatrix(world);

			ModelDraw(m_ModelDeep);
		}
		
		break;
	case A_Cylinder:

	{
		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		world = scale * rot * trans;

		Renderer::SetWorldMatrix(world);

		ModelDraw(m_Model);
	}
	{
		float scaleRatio = 1.0f - ((float)m_lifeTime / Enemy::SHOT_PER_FRAME[m_Mode]);

		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(m_Scale.x * scaleRatio, m_Scale.y, m_Scale.z * scaleRatio);
		rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		world = scale * rot * trans;

		Renderer::SetWorldMatrix(world);

		ModelDraw(m_ModelDeep);
	}

		break;
	default:
		break;
	}
}

void DamageArea::Uninit()
{
}

void DamageArea::SetLaserArea(GameObject* target ,Vector3 position, Vector3 scale ,int frame)
{
	m_Mode = A_Laser;
	m_EnemyPos = position;
	m_Target = target;
	m_Velocity = m_Target->GetPosition() - m_EnemyPos;
	m_lifeTime = frame;

	SetRotation(0.0f, atan2f(m_Velocity.x, m_Velocity.z), 0.0f);
	SetScale(scale);
	SetPosition(m_EnemyPos + m_Velocity * (m_Scale.z / 2));
}

void DamageArea::SetFanArea(Vector3 position, Vector3 velocity, Vector3 scale, int frame)
{
	m_Mode = A_Fan;
	m_EnemyPos = position;
	m_Velocity = velocity;
	m_lifeTime = frame;

	SetRotation(0.0f, atan2f(m_Velocity.x, m_Velocity.z), 0.0f);
	SetScale(scale);
	SetPosition(m_EnemyPos + m_Velocity * (m_Scale.z / 2));
}
