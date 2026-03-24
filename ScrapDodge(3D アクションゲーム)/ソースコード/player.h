/*============================================================
	[player.cpp]
	▼題名 : プレイヤー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "GameObject.h"
#include "animationModel.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class Player : public GameObject
{
private:
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;

	//パラメータ
	int m_MaxHp = 10;
	int m_Hp = m_MaxHp;
	bool m_CanInput = true;
	float m_Speed = 1.0f;

	//攻撃
	bool m_IsShot = false;
	float m_ShotFrame = 0;
	static constexpr float SHOT_MAX_FRAME = 60;

	//移動系
	Vector3 m_OldPos = m_Position;
	float m_WalkVel = 0.03f;
	float m_JumpVel = 0.80f;
	float m_Gravity = -0.025f;
	float m_Resist = 0.85f;
	bool m_IsRun = false;
	bool m_IsJump = true;
	bool m_IsInputMulti = false;

	//回転
	bool m_IsRotate = false;
	float m_OldDirection = 0.0f;
	float m_Direction = 0.0f;
	float m_RotationFrame = 0.0f;
	static constexpr int ROT_MAX_FRAME = 10;

	//モデル・アニメーション
	static bool m_IsLoad;
	static class AnimationModel* m_AnimationModel;
	std::string m_AnimationName = "Idle";
	std::string m_AnimationNameNext = "Idle";
	float m_AnimationBlend = 0.0f;
	float m_AnimFrame = 0.0f;
	class Particle* m_Aura = nullptr;

	//ダメージ演出用
	Vector3 m_ShakeVector = {0.0f,0.0f,0.0f};
	float m_ShakeTime = 0.0f;
	bool m_IsInvincible = false;
	float m_InvincibleFrame = 0;
	static constexpr int INVINCIBLE_MAX_FRAME = 60;

	//スロウ用
	bool m_IsSlow = false;
	float m_SlowCoolDown = 0;
	static constexpr float SLOW_MAX_COOLDOWN = 20;
	float m_SlowCharge = 0;

	//残像用
	static constexpr int AFTERIMAGE_PER_FRAME = 10;
	static constexpr int AFTER_IMAGE_NUM = 6;

	int m_AfterFrame = 0;
	Vector3 m_AfterImagePos[AFTER_IMAGE_NUM] = {};
	Vector3 m_AfterImageRot[AFTER_IMAGE_NUM] = {};
	float m_AfterImageAnimFrame[AFTER_IMAGE_NUM] = {};
	float m_AfterImageAnimBlend[AFTER_IMAGE_NUM] = {};

	class Camera* m_Camera;

	//サウンド
	static class Audio* m_JumpSE;

	static constexpr XMFLOAT4 DAMAGE_COLOR = { 0.8f, 0.4f, 0.4f, 0.8f };
	static constexpr XMFLOAT4 AFTERIMAGE_COLOR = { 0.8f, 0.4f, 0.4f, 0.8f };

	static constexpr Vector3 COLLISION_SCALE = { 1.0f, 2.0f, 1.0f };
	static constexpr Vector3 COLLISION_LOCALPOS = { 0.0f, 1.0f, 0.0f };
	static constexpr Vector3 SHADOW_SCALE = { 2.0f,2.0f,2.0f };

	static constexpr float SHAKE_FACTOR = 0.9f;
	static constexpr float BLEND_PER_FRAME = 0.15f;

	static constexpr Vector3 LOW_CAMERA_SHAKE = { 0.2f,0.2f,0.2f };
	static constexpr Vector3 LOW_PLAYER_SHAKE = { 0.25f,0.0f,0.25f };
	static constexpr int LOW_HITSTOP = 8;
	static constexpr int LOW_REDUCE_HP = 1;

	static constexpr Vector3 HIGH_CAMERA_SHAKE = { 1.0f,1.0f,1.0f };
	static constexpr Vector3 HIGH_PLAYER_SHAKE = { 0.4f,0.0f,0.4f };
	static constexpr int HIGH_HITSTOP = 12;
	static constexpr int HIGH_REDUCE_HP = 2;

	//インプット
	void UpdateInput();
	void Move();
	void Slow();
	void SlowCharge();
	void Shot();
	void Jump();

	void UpdateShot();

	//ゲームオーバー時
	void SetDeadAnimation();
	void UpdateDeadAnimation();

	//アニメーション関連
	void UpdateAnimation();
	void SetAnimation(std::string animName);

	void UpdateInvincible();

	void UpdateRotation();

	void UpdateHitCollision();
	void UpdateAfterEffect();

	void DrawAfterImage();

	void DrawDebugSystem();

	void DamageShake(Vector3 shakeVector);
	Vector3 GetShakePosition();

public:
	int& f_MaxHp = m_MaxHp;
	static constexpr float SLOW_MAX_CHARGE = 200;

	//死亡時演出
	static constexpr float DEAD_STOPTIME = 60.0f;
	static constexpr float HANDUP_ANIMTIME = 15.0f;
	static constexpr float DEAD_ANIMTIME = 60.0f;

	static void Load();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetHp(int hp) { m_Hp = hp; }
	int GetHp() { return m_Hp; }
	void SetSlowCharge(float charge) { m_SlowCharge = charge; }
	float GetSlowCharge() { return m_SlowCharge; }
	float GetShotFrame() { return m_ShotFrame / SHOT_MAX_FRAME; }

	const char* GetName() override
	{
		return "Player";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::DragFloat3("velocity", &m_Velocity.x);

			ImGui::Text("Status");
			ImGui::InputInt("HP", &m_Hp);
			ImGui::InputFloat("InvincibleFrame", &m_InvincibleFrame);
			ImGui::InputFloat("direciton", &m_Direction);

			ImGui::Text("flag");
			ImGui::Checkbox("Invincible", &m_IsInvincible);

			ImGui::Text("Parameter");
			ImGui::InputFloat("walkVel", &m_WalkVel);
			ImGui::InputFloat("jumpVel", &m_JumpVel);
			ImGui::InputFloat("gravity", &m_Gravity);

			ImGui::Text("Animation");
			ImGui::InputFloat("animFrame", &m_AnimFrame);

			ImGui::TreePop();
		}
	}
};

#endif //_PLAYER_H_