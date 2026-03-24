/*============================================================
	[enemy.h]
	▼題名 : 敵(ボス)
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "GameObject.h"

enum AttackMode
{
	A_Laser = 0,
	A_Fan,
	A_DropStone,
	A_Front,
	A_Side,
	A_Cylinder,
	A_Max,
};

class Enemy : public GameObject
{
protected:
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static class ModelRenderer* m_ModelRenderer;
	static bool m_IsLoad;

	//ステータス
	int m_Level = 1;
	static constexpr int m_MaxLevel = 3;
	int m_MaxHp = 15;
	int m_Hp = m_MaxHp;
	static constexpr int DAMAGE_PER_SHOT = 5;

	float m_WaitFrame = 0;
	static constexpr float WAIT_MAX_FRAME = 120;

	//攻撃
	int m_AttackNum = 0;

	static constexpr Vector3 SHOT_POS = { 0.0f,13.5f,22.5f }; //発射口
	static constexpr float SHOT_HEIGHT = 1.0f;//レーザーを浮かせる高さ

	AttackMode m_Mode[m_MaxLevel] = {};
	AttackMode m_NextMode[m_MaxLevel] = {};

	//モデル・アニメーション
	static class AnimationModel* m_AnimationModel;
	std::string m_AnimationName = {};
	std::string m_AnimationNameNext = {};
	float m_AnimationBlend = 0.0f;
	int m_AnimFrame = 0;

	static class Audio* m_LaserSE;
	static class Audio* m_CylinderSE;
	static class Audio* m_DamageSE;

	static constexpr float DAMAGE_SE_VOLUME = 0.7f;

private:

	class State
	{
	protected:
		Enemy* m_Parent = nullptr;
		int m_Index = -1;
	public:
		virtual void Update() = 0;

		template<typename T>
		void ChangeState()
		{
			Enemy* parent = m_Parent;
			int index = m_Index;

			delete m_Parent->m_State[m_Index];
			parent->m_State[index] = new T(parent, index);
		}
	};

	class StateIdle : public State
	{
	private:
		static constexpr float COOL_TIME_MAX = 120;
		float m_CoolTime = COOL_TIME_MAX;
	public:
		StateIdle(Enemy* pEnemy, int index)
		{
			m_Parent = pEnemy;
			m_Index = index;
			pEnemy->SetMode(AttackMode(rand() % A_Max), index);
		}

		void Update();
	};

	class StateAttack : public State
	{
	protected:
		float m_AttackFrame = 0;
	public:
		virtual void Update() = 0;
	};

	class StateLaser : public StateAttack
	{
	private:
		static constexpr int SHOT_PER_FRAME = 30;
		static constexpr int NUM_PER_LEVEL = 3;

		static constexpr float LASER_SPEED = 1.0f;
		static constexpr Vector3 DAMAGEAREA_SCALE = { 1.0f,2.0f,50.0f };

		static constexpr float SE_VOLUME = 0.5f;
	public:
		StateLaser(Enemy* pEnemy, int index);

		void Update() override;
		void SetDamageArea();
	};

	class StateFan : public StateAttack
	{
	private:
		float m_ShotRot = 0;
		bool m_IsRight = false;

		static constexpr int SHOT_PER_FRAME = 13;
		static constexpr int NUM_PER_LEVEL = 13;

		static constexpr float LASER_SPEED = 1.0f;
		static constexpr float ADD_ROT_PER_SHOT = 10.0f;
		static constexpr float RIGHT_START_ROT = 200.0f;
		static constexpr float LEFT_START_ROT = 330.0f;

		static constexpr float SE_VOLUME = 0.1f;

		static constexpr Vector3 DAMAGEAREA_SCALE = { 1.0f,2.0f,50.0f };
	public:
		StateFan(Enemy* pEnemy, int index);

		void Update() override;
		void SetDamageArea();
	};

	class StateStone : public StateAttack
	{
	private:
		class Stone* m_Attack = nullptr;

		static constexpr int SHOT_PER_FRAME = 15;
		static constexpr int NUM_PER_LEVEL = 2;

		static constexpr int STONE_SPAWN_RANGE = 28;
		static constexpr float STONE_HEIGHT = 20.0f;
		static constexpr Vector3 STONE_SCALE = { 7.0f, 3.0f, 7.0f };
		static constexpr Vector3 DAMAGEAREA_SCALE = { 7.0f, 0.1f, 7.0f };
	public:
		StateStone(Enemy* pEnemy, int index);

		void Update() override;
		void SetDamageArea(Vector3 stonePos);
	};

	class StateFront : public StateAttack
	{
	private:
		bool m_IsRight = false;

		static constexpr int SHOT_PER_FRAME = 45;
		static constexpr int NUM_PER_LEVEL = 20;

		static constexpr float LASER_SPEED = 0.3f;
		static constexpr Vector3 RIGHT_DAMAGE_AREA_POS = { 10.0f,0.0f,0.0f };
		static constexpr Vector3 LEFT_DAMAGE_AREA_POS = { -10.0f,0.0f,0.0f };
		static constexpr Vector3 LASER_VELOCITY = { 0.0f,0.0f,-1.0f };
		static constexpr Vector3 DAMAGE_AREA_SCALE = { 20.0f,1.0f,40.0f };
	public:
		StateFront(Enemy* pEnemy, int index);

		void Update() override;
		void SetDamageArea();
	};

	class StateSide : public StateAttack
	{
	private:
		bool m_IsFront = false;

		static constexpr int SHOT_PER_FRAME = 45;
		static constexpr int NUM_PER_LEVEL = 20;

		static constexpr float LASER_SPEED = 0.3f;
		static constexpr Vector3 FRONT_DAMAGE_AREA_POS = { 0.0f,0.0f,10.0f };
		static constexpr Vector3 BACK_DAMAGE_AREA_POS = { 0.0f,0.0f,-10.0f };
		static constexpr Vector3 RIGHT_LASER_POS = { 19.0f,0.0f,0.0f };
		static constexpr Vector3 LEFT_LASER_POS = { -19.0f,0.0f,0.0f };
		static constexpr Vector3 RIGHT_LASER_VELOCITY = { -1.0f,0.0f,0.0f };
		static constexpr Vector3 LEFT_LASER_VELOCITY = { 1.0f,0.0f,0.0f };
		static constexpr Vector3 DAMAGE_AREA_SCALE = { 20.0f,1.0f,40.0f };
	public:
		StateSide(Enemy* pEnemy, int index);

		void Update() override;
		void SetDamageArea();
	};

	class StateCylinder : public StateAttack
	{
	private:
		class LaserCylinder* m_Attack = nullptr;
		bool m_IsFront = false;

		static constexpr int SHOT_PER_FRAME = 15;
		static constexpr int NUM_PER_LEVEL = 4;

		static constexpr int CYLINDER_SPAWN_RANGE_X = 28;
		static constexpr int CYLINDER_SPAWN_RANGE_Y = 32;
		static constexpr Vector3 DAMAGE_AREA_SCALE = { 1.0f,0.1f,1.0f };

		static constexpr float SE_VOLUME = 0.3f;
	public:
		StateCylinder(Enemy* pEnemy, int index);

		void Update() override;
		class DamageArea* SetDamageArea(Vector3 basePos);
	};

	class StateStun : public State
	{
	private:
		static constexpr Vector3 TARGETPOINT_POS = { 0.0f, 12.0f, 18.0f };
	public:
		StateStun(Enemy* pEnemy, int index);

		void Update();
	};

	State* m_State[3];
public:
	
	int& f_MaxHp = m_MaxHp;

	//フレームステータス
	static constexpr int SHOT_PER_FRAME[A_Max] = { 30,13,15,45,45,45 }; //何フレームごとにレーザーが発射されるか
	static constexpr int NUM_PER_LEVEL[A_Max] = { 3,13,2,20,20,2 }; //レベルごとに何個の攻撃が出るか
	static constexpr int STUN_FOR_ATTACKNUM[m_MaxLevel] = { 5,10,15 };

	void SetHp(int hp) { m_Hp = hp; }
	int GetHp() { return m_Hp; }
	Vector3 GetShotPos() { return SHOT_POS; }

	State* GetState(int level) { return m_State[level]; }
	void SetMode(AttackMode mode, int level) { m_Mode[level] = mode; }
	AttackMode GetMode(int level) { return m_Mode[level]; }
	int GetLevel() { return m_Level; }
	void AddAttackNum() { m_AttackNum++; }
	bool GetIsStun(){
		if (m_AttackNum >= STUN_FOR_ATTACKNUM[m_Level - 1]) return true;
		else return false;
	}

	bool GetIsAttack()
	{
		for (int i = 0; i < GetLevel(); i++)
		{
			StateAttack* find = dynamic_cast<StateAttack*>(m_State[i]);

			if (find != nullptr)
				return true;
		}
		
		return false;
	}

	virtual void TakeDamage();

	const char* GetName() override
	{
		return "Enemy";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			const char* modes[A_Max] = {"Laser","Fan","Stone","Front","Side","Cylinder"};
			const char* nextModes[A_Max] = { "Laser","Fan","Stone","Front","Side","Cylinder"};

			for(int level = 0;level < m_Level;level++)
			{
				std::string label = "Level : " + std::to_string(level + 1);

				if (ImGui::TreeNode(label.c_str()))
				{
					if (ImGui::BeginCombo("Mode", modes[m_Mode[level]]))
					{
						for (int tagNum = 0; tagNum < A_Max; tagNum++)
						{
							if (ImGui::Selectable(modes[tagNum]))
							{
								m_Mode[level] = (AttackMode)tagNum;
							}
						}

						ImGui::EndCombo();
					}

					if (ImGui::BeginCombo("NextMode", nextModes[m_NextMode[level]]))
					{
						for (int tagNum = 0; tagNum < A_Max; tagNum++)
						{
							if (ImGui::Selectable(nextModes[tagNum]))
							{
								m_NextMode[level] = (AttackMode)tagNum;
							}
						}

						ImGui::EndCombo();
					}

					ImGui::TreePop();
				}
			}

			if (ImGui::TreeNode("Status"))
			{
				ImGui::DragInt("HP", &m_Hp);

				ImGui::Text("Stun");
				ImGui::InputInt("AttackNum", &m_AttackNum);

				ImGui::Text("Frame");

				ImGui::Text("StunForAttackNum");

				for (int level = 0; level < m_MaxLevel; level++)
				{
					std::string label = "Stun : " + std::to_string(level + 1);
				}

				ImGui::TreePop();
			}
			
			ImGui::TreePop();
		}
	}

	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;
};

#endif //_ENEMY_H_