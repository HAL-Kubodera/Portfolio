/*============================================================
	[robot.h]
	▼題名 : タイトル画面のロボ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "GameObject.h"

class Robot : public GameObject
{
private:
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	//移動系
	static constexpr float WALK_VEL = 0.03f;

	//回転
	bool m_IsRotate = false;
	float m_OldDirection = 0.0f;
	float m_Direction = 0.0f;
	float m_RotationFrame = 0.0f;
	static constexpr int m_RotationFrameMax = 10;

	//モデル・アニメーション
	class AnimationModel* m_AnimationModel = nullptr;
	std::string m_AnimationName = {};
	std::string m_AnimationNameNext = {};
	float m_AnimationBlend = 0.0f;

	unsigned int m_Frame = 0;

	static constexpr Vector3 DEFAULT_POSITION = { 0.0f,-0.55f,1.2f };
	static constexpr Vector3 DEFAULT_SCALE = { 1.5f,1.5f,1.5f };

	static constexpr int FIRST_ANIM_FRAME = 100;
	static constexpr int SECOND_ANIM_FRAME = 190;
	static constexpr int THIRD_ANIM_FRAME = 300;


	void UpdateRotation();
public:

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "Robot";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{

			ImGui::TreePop();
		}
	}

	void SetAnimation(std::string animName);
};

#endif // _ROBOT_H_