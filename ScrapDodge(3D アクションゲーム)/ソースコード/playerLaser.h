/*============================================================
	[playerLaser.cpp]
	▼題名 : プレイヤーのレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PLAYER_LASER_H_
#define _PLAYER_LASER_H_

#include "GameObject.h"
#include "model.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class PlayerLaser : public GameObject
{
private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static bool m_IsLoad;

	struct Particle
	{
		bool Enable;
		int Life;
		Vector3 Position;
		Vector3 Scale;
		Vector3 Velocity;
	};

	static const int PARTICLE_MAX = 10000;
	Particle m_Particle[PARTICLE_MAX];

	XMFLOAT4 m_Color = {0.0f,0.0f,0.0f,0.0f};

	bool m_IsCharge = false;
	bool m_IsDead = false;
	int m_Life = 0;

	Vector3 m_length = {0.0f,0.0f,0.0f};
	GameObject* m_Target = nullptr;

	static class Audio* m_ShotSE;

	static constexpr int VER_NUM = 4;

public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "PlayerLaser";
	}

	void DispUniqueGui() override
	{
		if (ImGui::TreeNode(GetName()))
		{
			GameObject::DispUniqueGui();

			ImGui::DragFloat3("velocity", &m_Velocity.x);
			ImGui::ColorEdit4("color", &m_Color.x);

			ImGui::TreePop();
		}
	}

	void SetTarget(GameObject* target);
	void SetCollision();

	void SetIsDead(bool dead = true) { m_IsDead = dead; }
	bool GetIsDead() { return m_IsDead; }
};

#endif //_PLAYER_LASER_H_