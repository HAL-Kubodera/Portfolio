/*============================================================
	[GameObject.h]
	▼題名 : ゲームオブジェクトの基底クラス
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "main.h"
#include "myComponent.h"

#include <list>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

enum Tag
{
	Tag_Normal,
	Tag_Field,
	Tag_Player,
	Tag_Attack,
	Tag_Damage,
	Tag_Spike,
	Tag_Wall,
	Tag_Max,
};

static const char* TagNames[] = {
	"Normal",
	"Field",
	"Player",
	"Attack",
	"Damage",
	"Spike",
	"Wall"
};

struct DEBUG_DATA
{
	Vector3* m_Position;
	Vector3* m_Scale;
	Vector3* m_Rotation;
	XMFLOAT4* m_Color;
	std::list<MyComponent*>* m_Components;
};

class GameObject
{
protected:
	bool m_Destroy = false;

	Vector3 m_Position{ 0.0f,0.0f,0.0f };
	Vector3 m_Velocity{ 0.0f,0.0f,0.0f };
	Vector3 m_Rotation{ 0.0f,0.0f,0.0f };
	Vector3 m_Scale{ 1.0f,1.0f,1.0f };
	XMFLOAT4 m_Color{ 1.0f,1.0f,1.0f,1.0f };
	std::list<MyComponent*> m_Components;

	DEBUG_DATA m_Data;
	Tag m_Tag = Tag_Normal;

	//デバッグ用
	bool m_ShowInspecter = false;
	int m_DebugFrame = 0;
public:
	GameObject()
	{
		m_Data.m_Position = &m_Position;
		m_Data.m_Rotation = &m_Rotation;
		m_Data.m_Scale = &m_Scale;
		m_Data.m_Color = &m_Color;
		m_Data.m_Components = &m_Components;
	}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;

	template<typename T>
	T* AddComponent()
	{
		T* component = new T();

		component->SetParent(this);

		std::string name = std::to_string(m_Components.size()) + " : ";

		component->SetName(name);
		component->Init();
		m_Components.push_back(component);

		return component;
	}

	template<typename T>
	T* GetComponent()
	{
		for (auto component : m_Components)
		{
			T* find = dynamic_cast<T*>(component);

			if (find != nullptr)
				return find;
		}

		return nullptr;
	}

	template<typename T>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> finds;

		for (auto component : m_Components)
		{
			T* find = dynamic_cast<T*>(component);

			if (find != nullptr)
				finds.push_back(find);
		}

		return finds;
	}

	void UpdateComponent()
	{
		for (auto component : m_Components)
		{
			bool isFollow = component->GetFollow();

			if (isFollow == true)
			{
				component->SetPosition(m_Position + component->GetOffset());
			}
			
			component->Update();
		}
	}

	void DrawComponent()
	{
		for (auto component : m_Components)
		{
			component->Draw();
		}
	}

	void UninitComponent()
	{
		for (auto component : m_Components)
		{
			component->Uninit();
			delete component;
		}
	}

	virtual const char* GetName()
	{
		return "GameObject";
	}

	virtual void DispUniqueGui()
	{
		DEBUG_DATA* pData = GetDebugData();
		Vector3* pos = pData->m_Position;
		Vector3* scale = pData->m_Scale;
		Vector3* rot = pData->m_Rotation;
		XMFLOAT4* color = pData->m_Color;
		std::list<MyComponent*>* components = pData->m_Components;

		ImGui::Separator();

		if (ImGui::BeginCombo("Tag", TagNames[static_cast<int>(GetTag())]))
		{
			for (int tagNum = 0; tagNum < Tag_Max; tagNum++)
			{
				if (ImGui::Selectable(TagNames[tagNum]))
				{
					SetTag((Tag)tagNum);
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Transform"))
		{
			ImGui::DragFloat3("position", &pos->x);
			ImGui::DragFloat3("scale", &scale->x);
			ImGui::DragFloat3("rotation", &rot->x);
			ImGui::TreePop();
		}

		ImGui::Separator();
	}

	void SetDestroy() { m_Destroy = true; }
	bool GetDestroy() { return m_Destroy; }

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

	void SetVelocity(Vector3 Size)
	{
		m_Velocity = Size;
	}

	void SetVelocity(float x, float y, float z)
	{
		m_Velocity.x = x;
		m_Velocity.y = y;
		m_Velocity.z = z;
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

	void SetTag(Tag tag)
	{
		m_Tag = tag;
	}

	Vector3 GetPosition()
	{
		return m_Position;
	}

	Vector3 GetVelocity()
	{
		return m_Velocity;
	}

	Vector3 GetScale()
	{
		return m_Scale;
	}

	Vector3 GetRotation()
	{
		return m_Rotation;
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

	Tag GetTag()
	{
		return m_Tag;
	}

	std::list<MyComponent*> GetComponents()
	{
		return m_Components;
	}

	float GetDistance(Vector3 Position)
	{
		//Positionはこのオブジェクトと比較する位置
		Vector3 direction = m_Position - Position;

		return direction.length();
	}

	//GUI用
	DEBUG_DATA* GetDebugData()
	{
		return &m_Data;
	}

	void SetShowInspecter(bool flg)
	{
		m_ShowInspecter = flg;
	}

	bool GetShowInspecter()
	{
		return m_ShowInspecter;
	}
};

#endif // _GAME_OBJECT_H_