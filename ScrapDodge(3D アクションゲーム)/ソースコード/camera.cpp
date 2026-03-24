/*============================================================
	[camera.cpp]
	▼題名 : カメラ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "camera.h"

#include "player.h"
#include "enemy.h"
#include "manager.h"
#include "scene.h"
#include "mousemanager.h"
#include "speedManager.h"
#include "fade.h"
#include "particleCrash.h"
#include "targetPoint.h"

bool Camera::m_BuildMode = false;
Vector3 Camera::m_ShakeVector = { 0.0f,0.0f,0.0f };
float Camera::m_ShakeTime = 0;;

void Camera::Init()
{

}

void Camera::Update()
{
#ifdef _DEBUG
	if (Keyboard_IsKeyDownTrigger(KK_T))
	{
		m_BuildMode = !m_BuildMode;
	}

	if (m_BuildMode)
	{
		SetBuildModePosition();

		return;
	}
#endif

	

	Player* pPlayer = Manager::GetScene()->GetGameObject<Player>();
	Enemy* pEnemy = Manager::GetScene()->GetGameObject<Enemy>();

	if (SpeedManager::GetMode() == Speed_Dead)
	{
		UpdateDeadCamera();
	}
	else if (SpeedManager::GetMode() == Speed_Clear)
	{
		UpdateClearCamera();
	}
	else
	{
		if (Manager::GetScene()->GetGameObject<TargetPoint>() != nullptr)
		{
			if (m_AnimFrame != TPOINT_ANIM_TARGET_FRAME)
				m_AnimFrame++;
		}
		else
		{
			if (m_AnimFrame != 0)
				m_AnimFrame--;
		}

		UpdateTargetPointCamera();
	}

	Renderer::SetCameraPosition(m_Position);
}

void Camera::Draw()
{
	//プロジェクションマトリクス
	m_Projection = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);

	Renderer::SetProjectionMatrix(m_Projection);

	//ビューマトリクス
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_View = XMMatrixLookAtLH(XMLoadFloat3((XMFLOAT3*) & m_Position),
								XMLoadFloat3((XMFLOAT3*)&m_Target),
								XMLoadFloat3(&up));

	Renderer::SetViewMatrix(m_View);

#ifdef _DEBUG

	ImGui::Begin("Debug");

	if (ImGui::Button("CLEAR")) {
		SpeedManager::SetMode(Speed_Clear);
		SpeedManager::SetSpeed(0.0f);
	}

	ImGui::End();
#endif
}

void Camera::Uninit()
{
	
}

void Camera::Shake(Vector3 ShakeVector)
{
	m_ShakeVector = ShakeVector;
	m_ShakeTime = 0.0f;
}

void Camera::UpdateTargetPointCamera()
{
	Player* pPlayer = Manager::GetScene()->GetGameObject<Player>();
	Enemy* pEnemy = Manager::GetScene()->GetGameObject<Enemy>();
	TargetPoint* tPoint = Manager::GetScene()->GetGameObject<TargetPoint>();
	if (tPoint != nullptr) m_TargetPoint = tPoint->GetPosition();

	float lerp = (m_AnimFrame / TPOINT_ANIM_TARGET_FRAME);

	if (m_AnimFrame < TPOINT_ANIM_TARGET_FRAME)
	{
		if (pPlayer != nullptr && pEnemy != nullptr)
		{
			Vector3 bTarget = pEnemy->GetPosition();
			Vector3 aTarget = m_TargetPoint;
			m_Target = bTarget * (1.0f - lerp) + aTarget * lerp;
			m_Target+= m_ShakeVector * cosf(m_ShakeTime);

			//カメラシェイク
			m_ShakeTime += SHAKE_RESIST;
			m_ShakeVector *= SHAKE_RESIST;

			Vector3 bBasePosition = pPlayer->GetPosition();
			float z = pPlayer->GetPosition().z;
			z /= DIVOFFSET_Z;

			bBasePosition.z -= (ADDOFFSET_Z + z);
			bBasePosition.y = (ADDOFFSET_Y + z);

			if (bBasePosition.z < ADDOFFSET_DISTANCE) bBasePosition.z = ADDOFFSET_DISTANCE;
			if (bBasePosition.z > 0) bBasePosition.z = 0;

			Vector3 aBasePosition = pPlayer->GetPosition();

			aBasePosition.z -= TPOINT_ADDOFFSET_Z;
			aBasePosition.y = TPOINT_ADDOFFSET_Y;

			if (aBasePosition.z < ADDOFFSET_DISTANCE) aBasePosition.z = ADDOFFSET_DISTANCE;
			if (aBasePosition.z > -3) aBasePosition.z = -3;

			m_Position = bBasePosition * (1.0f - lerp) + aBasePosition * lerp;
		}
	}
	else
	{
		m_Target = m_TargetPoint + m_ShakeVector * cosf(m_ShakeTime);

		//カメラシェイク
		m_ShakeTime += SHAKE_RESIST;
		m_ShakeVector *= SHAKE_RESIST;

		Vector3 BasePosition = pPlayer->GetPosition();

		BasePosition.z -= TPOINT_ADDOFFSET_Z;
		BasePosition.y = TPOINT_ADDOFFSET_Y;

		if (BasePosition.z < ADDOFFSET_DISTANCE) BasePosition.z = ADDOFFSET_DISTANCE;
		if (BasePosition.z > -3) BasePosition.z = -3;

		m_Position = BasePosition;
	}
}

void Camera::UpdateClearCamera()
{
	Player* pPlayer = Manager::GetScene()->GetGameObject<Player>();
	Enemy* pEnemy = Manager::GetScene()->GetGameObject<Enemy>();

	m_AnimFrame++;

	if (m_AnimFrame <= CLEAR_ANIM_TARGET_FRAME)
	{
		Vector3 beforeTarget = pEnemy->GetPosition();
		Vector3 afterTarget = pEnemy->GetShotPos();

		m_Target = beforeTarget * (BASE_HEIGHT - (m_AnimFrame / CLEAR_ANIM_TARGET_FRAME))
			+ afterTarget * (m_AnimFrame / CLEAR_ANIM_TARGET_FRAME);
	}
	else if (m_AnimFrame <= CLEAR_ANIM_POSITION_FRAME)
	{
		Vector3 beforeBasePosition = pPlayer->GetPosition();
		float z = pPlayer->GetPosition().z;
		z /= DIVOFFSET_Z;

		beforeBasePosition.z -= (ADDOFFSET_Z + z);
		beforeBasePosition.y = (BASE_HEIGHT + z);

		if (beforeBasePosition.z < ADDOFFSET_DISTANCE) beforeBasePosition.z = ADDOFFSET_DISTANCE;
		if (beforeBasePosition.z > 0) beforeBasePosition.z = 0;

		Vector3 beforePos = beforeBasePosition;

		m_Target = pEnemy->GetShotPos();
		Vector3 afterBasePosition = pEnemy->GetShotPos();
		afterBasePosition.y += CLEAR_HEIGHT_OFFSET;
		afterBasePosition.z = pPlayer->GetPosition().z;

		Vector3 afterPos = afterBasePosition;

		m_Position = beforePos * (BASE_HEIGHT - (m_AnimFrame / CLEAR_ANIM_POSITION_FRAME))
			+ afterPos * (m_AnimFrame / CLEAR_ANIM_POSITION_FRAME);
	}
	else if (m_AnimFrame > CLEAR_ANIM_POSITION_FRAME)
	{
		//敵攻撃時のパーティクルを描画
		if ((int)m_AnimFrame % CLEAR_ADD_PARTICLE_FRAME == 0)
		{
			ParticleCrash* pParticleCrash = Manager::GetScene()->AddGameObject<ParticleCrash>(object);

			Vector3 particlePos = pEnemy->GetShotPos();
			particlePos.x += float(((rand() % CLEAR_EFFECT_RANGE) - CLEAR_EFFECT_RANGE / 2));
			particlePos.y += float(((rand() % CLEAR_EFFECT_RANGE) - CLEAR_EFFECT_RANGE / 2));
			particlePos.z += float((rand() % CLEAR_EFFECT_RANGE));

			pParticleCrash->SetPosition(particlePos);
			pParticleCrash->SetColor(CLEAR_EFFECT_COLOR);
		}
	}

	if (m_AnimFrame == DEAD_ANIM_END_FRAME)
	{
		Manager::GetScene()->SetIsFade(true);
		Fade::SetFadeIn(120);
	}
}

void Camera::UpdateDeadCamera()
{
	Player* pPlayer = Manager::GetScene()->GetGameObject<Player>();
	Enemy* pEnemy = Manager::GetScene()->GetGameObject<Enemy>();

	m_AnimFrame++;

	if (m_AnimFrame <= DEAD_ANIM_TARGET_FRAME)
	{
		m_AnimFrame++;

		Vector3 beforeTarget = pEnemy->GetPosition();
		Vector3 afterTarget = pPlayer->GetPosition();

		m_Target = beforeTarget * (BASE_HEIGHT - (m_AnimFrame / DEAD_ANIM_TARGET_FRAME))
			+ afterTarget * (m_AnimFrame / DEAD_ANIM_TARGET_FRAME);
	}
	if (m_AnimFrame < DEAD_ANIM_POSITION_FRAME)
	{
		Vector3 beforeBasePosition = pPlayer->GetPosition();
		float z = pPlayer->GetPosition().z;
		z /= DIVOFFSET_Z;

		beforeBasePosition.z -= (ADDOFFSET_Z + z);
		beforeBasePosition.y = (BASE_HEIGHT + z);

		if (beforeBasePosition.z < ADDOFFSET_DISTANCE) beforeBasePosition.z = ADDOFFSET_DISTANCE;
		if (beforeBasePosition.z > 0) beforeBasePosition.z = 0;

		Vector3 beforePos = beforeBasePosition;

		m_Target = pPlayer->GetPosition();
		Vector3 afterBasePosition = DEAD_ANIM_BASEPOSITION;

		Vector3 afterPos = afterBasePosition + m_Target +
			Vector3(-sinf(m_Rotation.y), sinf(m_Rotation.x), -cosf(m_Rotation.y)) * DEAD_TARGET_DISTANCE;

		m_Position = beforePos * (BASE_HEIGHT - (m_AnimFrame / DEAD_ANIM_POSITION_FRAME))
			+ afterPos * (m_AnimFrame / DEAD_ANIM_POSITION_FRAME);
	}
	if (m_AnimFrame > DEAD_ANIM_POSITION_FRAME)
	{
		//ゲーム終了時にプレイヤーの周りをぐるぐる回るように変更
		m_Target = pPlayer->GetPosition();

		Vector3 BasePosition = DEAD_ANIM_BASEPOSITION;

		m_Rotation.y += DEAD_ROT_SPEED;

		m_Position = BasePosition + m_Target +
			Vector3(-sinf(m_Rotation.y), sinf(m_Rotation.x), -cosf(m_Rotation.y)) * DEAD_TARGET_DISTANCE;
	}

	if (m_AnimFrame == DEAD_ANIM_END_FRAME)
	{
		Manager::GetScene()->SetIsFade(true);
		Fade::SetFadeIn(120);
	}
}

void Camera::SetBuildModePosition()
{
	if (MouseManager::IsMouseRightDown())
	{
		while (ShowCursor(FALSE) >= 0);

		RECT rc;
		POINT center;
		GetClientRect(GetWindow(), &rc);
		center.x = (rc.right - rc.left) / 2;
		center.y = (rc.bottom - rc.top) / 2;

		SetCursorPos(center.x, center.y);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindow(), &pt);

		int x = MouseManager::GetMouseX() - pt.x;
		int y = MouseManager::GetMouseY() - pt.y;

		m_Rotation.y += x * BUILD_MOUSE_MOVE_FACTOR;
		m_Rotation.x += y * BUILD_MOUSE_MOVE_FACTOR;

		if (m_Rotation.x <= -BUILD_ROTATION_RANGE)
			m_Rotation.x = -BUILD_ROTATION_RANGE;
		if (m_Rotation.x >= BUILD_ROTATION_RANGE)
			m_Rotation.x = BUILD_ROTATION_RANGE;
	}
	else
		while (ShowCursor(TRUE) <= 0);

	//カメラ方向変更
	if (Keyboard_IsKeyDown(KK_RIGHT))
	{
		m_Rotation.y += BUILD_TURN_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_LEFT))
	{
		m_Rotation.y -= BUILD_TURN_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_UP))
	{
		m_Rotation.x -= BUILD_TURN_SPEED;

		if (m_Rotation.x <= -XM_PIDIV2)
			m_Rotation.x = -XM_PIDIV2;
	}
	if (Keyboard_IsKeyDown(KK_DOWN))
	{
		m_Rotation.x += BUILD_TURN_SPEED;

		if (m_Rotation.x >= XM_PIDIV2)
			m_Rotation.x = XM_PIDIV2;
	}

	//カメラ位置移動
	if (Keyboard_IsKeyDown(KK_A))
	{
		m_Target += GetLeft() * BUILD_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_D))
	{
		m_Target += GetRight() * BUILD_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_W))
	{
		m_Target += GetForward() * BUILD_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_S))
	{
		m_Target += GetBack() * BUILD_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_SPACE))
	{
		m_Target.y += BUILD_SPEED;
	}
	if (Keyboard_IsKeyDown(KK_LEFTCONTROL))
	{
		m_Target.y -= BUILD_SPEED;
	}


	m_Position = m_Target +
		Vector3(-sinf(m_Rotation.y), sinf(m_Rotation.x), -cosf(m_Rotation.y)) * BUILD_TARGET_DISTANCE;

	Renderer::SetCameraPosition(m_Position);
}
