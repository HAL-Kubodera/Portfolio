/*============================================================
	[loadCamera.cpp]
	▼題名 : ローディング画面のカメラ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "loadcamera.h"

#include "player.h"
#include "manager.h"
#include "scene.h"
#include "mousemanager.h"

#define SPEED (0.3f)
#define TURN_SPEED (0.1f)

bool LoadCamera::m_BuildMode = false;

void LoadCamera::Init()
{
	//カメラステータス 高いところから見下ろし
	m_Position = { 0.0f,15.0f,-25.0f };
	m_Rotation.x = 0.5f;
	m_Target = { 0.0f, 2.0f, -7.0f };
}

void LoadCamera::Update()
{
#ifdef _DEBUG
	if (Keyboard_IsKeyDownTrigger(KK_T))
	{
		m_BuildMode = !m_BuildMode;
	}
#endif

	if (m_BuildMode)
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

			m_Rotation.y += x * 0.001f;
			m_Rotation.x += y * 0.001f;

			if (m_Rotation.x <= -2.0f)
				m_Rotation.x = -2.0f;
			if (m_Rotation.x >= 2.0f)
				m_Rotation.x = 2.0f;
		}
		else
			while (ShowCursor(TRUE) <= 0);

		//カメラ方向変更
		if (Keyboard_IsKeyDown(KK_RIGHT))
		{
			m_Rotation.y += TURN_SPEED;
		}
		if (Keyboard_IsKeyDown(KK_LEFT))
		{
			m_Rotation.y -= TURN_SPEED;
		}
		if (Keyboard_IsKeyDown(KK_UP))
		{
			m_Rotation.x -= TURN_SPEED;

			if (m_Rotation.x <= -XM_PIDIV2)
				m_Rotation.x = -XM_PIDIV2;
		}
		if (Keyboard_IsKeyDown(KK_DOWN))
		{
			m_Rotation.x += TURN_SPEED;

			if (m_Rotation.x >= XM_PIDIV2)
				m_Rotation.x = XM_PIDIV2;
		}

		//カメラ位置移動
		if (Keyboard_IsKeyDown(KK_A))
		{
			m_Target += GetLeft() * SPEED;
		}
		if (Keyboard_IsKeyDown(KK_D))
		{
			m_Target += GetRight() * SPEED;
		}
		if (Keyboard_IsKeyDown(KK_W))
		{
			m_Target += GetForward() * SPEED;
		}
		if (Keyboard_IsKeyDown(KK_S))
		{
			m_Target += GetBack() * SPEED;
		}
		if (Keyboard_IsKeyDown(KK_SPACE))
		{
			m_Target.y += SPEED;
		}
		if (Keyboard_IsKeyDown(KK_LEFTCONTROL))
		{
			m_Target.y -= SPEED;
		}


		m_Position = m_Target +
			Vector3(-sinf(m_Rotation.y), sinf(m_Rotation.x), -cosf(m_Rotation.y)) * 3.0f;
	}
	else
	{
		//while (ShowCursor(FALSE) >= 0);

		//カメラシェイク
		m_Target += m_ShakeVector * cosf(m_ShakeTime);
		m_ShakeTime += 0.9f;
		m_ShakeVector *= 0.9f;

		//カメラステータス
		/*m_Position = { 0.0f,15.0f,-25.0f };
		m_Rotation.x = 0.5f;
		m_Target = Vector3(0.0f, 2.0f, -7.0f);*/
	}

	Renderer::SetCameraPosition(m_Position);
}

void LoadCamera::Draw()
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
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(GetWindow(), &pt);

	int x = MouseManager::GetMouseX();
	int y = MouseManager::GetMouseY();

	ImGui::Begin("mouse");
	ImGui::InputInt("x", &x);
	ImGui::InputInt("y", &y);

	ImGui::End();
#endif
}

void LoadCamera::Uninit()
{
	
}

void LoadCamera::Shake(Vector3 ShakeVector)
{
	m_ShakeVector = ShakeVector;
	m_ShakeTime = 0.0f;
}
