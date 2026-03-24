/*============================================================
	[camera.h]
	▼題名 : カメラ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GameObject.h"

class Camera : public GameObject
{
private:
	XMMATRIX m_Projection;
	XMMATRIX m_View;

	Vector3 m_Target{0.0f,0.0f,0.0f};
	Vector3 m_TargetPoint = {};

	static Vector3 m_ShakeVector;
	static float m_ShakeTime;

	float m_AnimFrame = 0;


	//ステータス
	static constexpr float BASE_HEIGHT = 1.0f;
	static constexpr float ADDOFFSET_Y = 4.0f;
	static constexpr float ADDOFFSET_Z = 8.0f;
	static constexpr float DIVOFFSET_Z = 7.0f;
	static constexpr float ADDOFFSET_DISTANCE = -20.5f;
	static constexpr float SHAKE_RESIST = 0.9f;

	//ターゲットポイント用
	static constexpr float TPOINT_ANIM_TARGET_FRAME = 50.0f;
	static constexpr float TPOINT_ADDOFFSET_Y = 1.0f;
	static constexpr float TPOINT_ADDOFFSET_Z = 5.0f;

	//クリア時演出
	static constexpr float CLEAR_ANIM_TARGET_FRAME = 15.0f;
	static constexpr float CLEAR_ANIM_POSITION_FRAME = 30.0f + CLEAR_ANIM_TARGET_FRAME;
	static constexpr float CLEAR_ANIM_END_FRAME = 200.0f + CLEAR_ANIM_POSITION_FRAME;
	static constexpr int CLEAR_ADD_PARTICLE_FRAME = 8;
	static constexpr float CLEAR_HEIGHT_OFFSET = -3.0f;
	static constexpr int CLEAR_EFFECT_RANGE = 20;
	static constexpr XMFLOAT4 CLEAR_EFFECT_COLOR = { 1.0f,0.0f,0.0f,1.0f };

	//死亡時演出
	static constexpr float DEAD_ANIM_TARGET_FRAME = 3.0f;
	static constexpr float DEAD_ANIM_POSITION_FRAME = 15.0f + DEAD_ANIM_TARGET_FRAME;
	static constexpr float DEAD_ANIM_END_FRAME = 200.0f + DEAD_ANIM_POSITION_FRAME;
	static constexpr float DEAD_ROT_SPEED = 0.03f;
	static constexpr float DEAD_TARGET_DISTANCE = 4.0f;
	static constexpr Vector3 DEAD_ANIM_BASEPOSITION = {0.0f,0.8f,0.0f};

	//デバック時カメラ用
	static constexpr float BUILD_TARGET_DISTANCE = 3.0f;
	static constexpr float BUILD_MOUSE_MOVE_FACTOR = 0.001f;
	static constexpr float BUILD_ROTATION_RANGE = 2.0f;
	static constexpr float BUILD_SPEED = 0.3f;
	static constexpr float BUILD_TURN_SPEED = 0.1f;

	void UpdateTargetPointCamera();
	void UpdateClearCamera();
	void UpdateDeadCamera();

	//デバッグ用
	void SetBuildModePosition();
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	static void Shake(Vector3 ShakeVector);

	XMMATRIX GetViewMatrix() { return m_View; }
	void SetTarget(Vector3 target) { m_Target = target; }

	//デバッグ用
	static bool m_BuildMode;
	
	const char* GetName() override
	{
		return "Camera";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::DragFloat3("Target", &m_Target.x);

			ImGui::Text("Cheat");
			ImGui::Checkbox("BuildMode", &m_BuildMode);

			ImGui::TreePop();
		}
	}
};

#endif // _CAMERA_H_