/*============================================================
	[myComponent.cpp]
	▼題名 : コンポーネントの基底クラス
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#pragma once
#include "main.h"
#include <string>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

struct DEBUG_COMPONENT_DATA
{
	Vector3* m_Position;
	Vector3* m_Scale;
	Vector3* m_Rotation;
	XMFLOAT4* m_Color;
};

class MyComponent
{
protected:
	bool m_Destroy = false;
	bool m_IsFollow = true;

	class GameObject* m_Parent = nullptr;

	Vector3 m_Position{ 0.0f,0.0f,0.0f };
	Vector3 m_Rotation{ 0.0f,0.0f,0.0f };
	Vector3 m_Scale{ 1.0f,1.0f,1.0f };
	XMFLOAT4 m_Color{ 1.0f,1.0f,1.0f,1.0f };
	Vector3 m_Offset{ 0.0f,0.0f,0.0f };

	//デバッグ用
	DEBUG_COMPONENT_DATA m_Data;
	std::string m_GuiName;

public:
	MyComponent()
	{
		m_Data.m_Position = &m_Position;
		m_Data.m_Rotation = &m_Rotation;
		m_Data.m_Scale = &m_Scale;
		m_Data.m_Color = &m_Color;
	}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;

	virtual const char* GetName()
	{
		return m_GuiName.c_str();
	}

	virtual void DispUniqueGui()
	{
		ImGui::Separator();


		if (ImGui::TreeNode("Transform"))
		{
			ImGui::DragFloat3("position", &m_Position.x);
			ImGui::DragFloat3("scale", &m_Scale.x);
			ImGui::DragFloat3("rotation", &m_Rotation.x);
			ImGui::DragFloat3("offset", &m_Offset.x);
			ImGui::TreePop();
		}

		ImGui::Separator();
	}

	void SetDestroy() { m_Destroy = true; }

	bool Destroy()
	{
		if (m_Destroy)
		{
			Uninit();
			delete this;
			return true;
		}
		else
		{
			return false;
		}
	}

	void SetName(std::string name) { m_GuiName = name; }

	void SetFollow(bool flg) { m_IsFollow = flg; }

	void SetParent(GameObject* obj)
	{
		m_Parent = obj;
	}

	void SetPosition(Vector3 Position)
	{
		m_Position = Position;
	}

	void SetPosition(float x, float y, float z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;
	}

	void SetScale(Vector3 Size)
	{
		m_Scale = Size;
	}

	void SetScale(float x, float y, float z)
	{
		m_Scale.x = x;
		m_Scale.y = y;
		m_Scale.z = z;
	}

	void SetScale(float scale)
	{
		m_Scale.x = scale;
		m_Scale.y = scale;
		m_Scale.z = scale;
	}

	void SetRotation(Vector3 Rotation)
	{
		m_Rotation = Rotation;
	}

	void SetRotation(float x, float y, float z)
	{
		m_Rotation.x = x;
		m_Rotation.y = y;
		m_Rotation.z = z;
	}

	void SetOffset(float x, float y, float z)
	{
		m_Offset.x = x;
		m_Offset.y = y;
		m_Offset.z = z;
	}
	
	void SetOffset(Vector3 offset)
	{
		m_Offset = offset;
	}

	void SetColor(XMFLOAT4 color)
	{
		m_Color = color;
	}

	bool GetFollow() { return m_IsFollow; }

	GameObject* GetParent()
	{
		return m_Parent;
	}

	Vector3 GetPosition()
	{
		return m_Position;
	}

	Vector3 GetScale()
	{
		return m_Scale;
	}

	Vector3 GetRotation()
	{
		return m_Rotation;
	}

	Vector3 GetOffset()
	{
		return m_Offset;
	}

	Vector3 GetRight()
	{
		XMMATRIX matrix;
		matrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 right;
		XMStoreFloat3((XMFLOAT3*)&right, matrix.r[0]);

		return right;
	}

	Vector3 GetLeft()
	{
		XMMATRIX matrix;
		matrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 left;
		XMStoreFloat3((XMFLOAT3*)&left, -matrix.r[0]);

		return left;
	}

	Vector3 GetUp()
	{
		XMMATRIX matrix;
		matrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 up;
		XMStoreFloat3((XMFLOAT3*)&up, matrix.r[1]);

		return up;
	}

	Vector3 GetForward()
	{
		XMMATRIX matrix;
		matrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 forward;
		XMStoreFloat3((XMFLOAT3*)&forward, matrix.r[2]);

		return forward;
	}

	Vector3 GetBack()
	{
		XMMATRIX matrix;
		matrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 back;
		XMStoreFloat3((XMFLOAT3*)&back, -matrix.r[2]);

		return back;
	}

	float GetDistance(Vector3 Position)
	{
		//Positionはこのオブジェクトと比較する位置
		Vector3 direction = m_Position - Position;

		return direction.length();
	}

	DEBUG_COMPONENT_DATA* GetDebugData()
	{
		return &m_Data;
	}
};