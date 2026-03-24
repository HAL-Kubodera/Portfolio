/*============================================================
	[tutorialEnemy.h]
	▼題名 : チュートリアルでの敵
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TUTORIAL_ENEMY_H_
#define _TUTORIAL_ENEMY_H_

#include "enemy.h"

class TutorialEnemy : public Enemy
{
private:

	class State
	{
	protected:
		TutorialEnemy* m_Parent = nullptr;
	public:
		virtual void Update() = 0;

		template<typename T>
		void ChangeState()
		{
			TutorialEnemy* parent = m_Parent;

			delete m_Parent->m_State;
			parent->m_State = new T(parent);
		}
	};

	class StateIdle : public State
	{
	private:

	public:
		StateIdle(TutorialEnemy* pEnemy)
		{
			m_Parent = pEnemy;
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
		static constexpr int NUM_PER_LEVEL = 1;
		static constexpr float LASER_SPEED = 1.0f;
		static constexpr Vector3 DAMAGEAREA_SCALE = { 1.0f,2.0f,50.0f };

		static constexpr float SE_VOLUME = 0.5f;
	public:
		StateLaser(TutorialEnemy* pEnemy);

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

		static constexpr float ADD_ROT_PER_SHOT = 10.0f;
		static constexpr float RIGHT_START_ROT = 200.0f;
		static constexpr float LEFT_START_ROT = 330.0f;

		static constexpr float LASER_SPEED = 1.0f;
		static constexpr float SE_VOLUME = 0.1f;

		static constexpr Vector3 DAMAGEAREA_SCALE = { 1.0f,2.0f,50.0f };
	public:
		StateFan(TutorialEnemy* pEnemy);

		void Update() override;
		void SetDamageArea();
	};

	class StateFront : public StateAttack
	{
	private:
		bool m_IsRight = false;

		static constexpr int SHOT_PER_FRAME = 45;
		static constexpr int NUM_PER_LEVEL = 40;

		static constexpr int BasePos = 20;
		static constexpr float LASER_SPEED = 0.3f;

		static constexpr Vector3 DAMAGEAREA_SCALE = { 40.0f,1.0f,40.0f };
	public:
		StateFront(TutorialEnemy* pEnemy);

		void Update() override;
		void SetDamageArea();
	};

	class StateStun : public State
	{
	private:

	public:
		StateStun(TutorialEnemy* pEnemy);

		void Update();
	};

	State* m_State;

public:

	//フレームステータス
	static constexpr int m_ShotPerFrame[A_Max] = { 15,13,15,45,45,45 }; //何フレームごとにレーザーが発射されるか
	static constexpr int m_NumPerLevel[A_Max] = { 1,13,1,20,20,2 }; //レベルごとに何個の攻撃が出るか
	static constexpr int m_StunForAttackNum[m_MaxLevel] = { 3};

	void SetHp(int hp) { m_Hp = hp; }
	int GetHp() { return m_Hp; }

	Vector3 GetShotPos() { return SHOT_POS; }

	void SetAttack(AttackMode mode);
	void SetTargetPoint();
	void TakeDamage() override;

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

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;
};

#endif // _TUTORIAL_ENEMY_H_