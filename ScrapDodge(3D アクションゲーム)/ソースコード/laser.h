/*============================================================
	[laser.h]
	▼題名 : 敵のレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _LASER_H_
#define _LASER_H_

#include "GameObject.h"

class Laser : public GameObject
{
public:
	struct PARTICLE
	{
		bool Enable;
		float Life;
		Vector3 Position;
	};

	static const int PARTICLE_MAX = 10000;
	static constexpr int PARTICLE_LIFE = 10;

private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static bool m_IsLoad;
	
	float m_GenerateFrame = 0.0f;

	PARTICLE m_Particle[PARTICLE_MAX];
	XMFLOAT4 m_Color = {0.0f,0.0f,0.0f,0.0f};
	bool m_IsDead = false;

	float m_Speed = 1.0f;
	GameObject* m_Target = nullptr;
	
	static constexpr int VER_NUM = 4;
	static constexpr Vector3 STAGE_RANGE = { 25.0f,-2.0f,25.0f };

	static constexpr Vector3 DEFAULT_COLLISION_SCALE = { 1.0f, 1.0f, 9.8f };
	static constexpr float DEFAULT_COLLISION_POS_FACTOR = -4.5f;
	static constexpr Vector3 SLOW_COLLISION_SCALE = { 1.0f, 1.0f, 3.6f };
	static constexpr float SLOW_COLLISION_POS_FACTOR = 1.15f;

	static constexpr Vector3 SHADOW_SCALE = { 8.0f,8.0f,1.0f };
	static constexpr Vector3 SHADOW_OFFSET = { 0.4f,0.4f,0.0f };

	static constexpr int LASER_PARTICLE_NUM = 4;

	void LifeCheck();
	void StageCheck();//ステージにいるか
	void GenerateParticle();

public:
	static constexpr int LASER_DEFAULT = 0;
	static constexpr int LASER_SLOW = 1;

	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "Laser";
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
	void SetCollision(int n);
	void SetSpeed(float speed);
	void SetShadow();

	void SetIsDead(bool dead = true) { m_IsDead = dead; }
	bool GetIsDead() { return m_IsDead; }

	PARTICLE* GetParticle() {return m_Particle;}
};

#endif // _LASER_H_