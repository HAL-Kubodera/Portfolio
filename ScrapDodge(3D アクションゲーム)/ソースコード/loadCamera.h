/*============================================================
	[loadCamera.h]
	▼題名 : ローディング画面のカメラ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _LOAD_CAMERA_H_
#define _LOAD_CAMERA_H_

#include "GameObject.h"

class LoadCamera : public GameObject
{
private:
	XMMATRIX m_Projection;
	XMMATRIX m_View;

	Vector3 m_Target{0.0f,0.0f,0.0f};

	Vector3 m_ShakeVector = {0.0f,0.0f,0.0f};
	float m_ShakeTime = 0;;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void Shake(Vector3 ShakeVector);

	XMMATRIX GetViewMatrix() { return m_View; }
	void SetTarget(Vector3 target) { m_Target = target; }

	//デバッグ用
	static bool m_BuildMode;

	const char* GetName() override
	{
		return "LoadCamera";
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

#endif // _LOAD_CAMERA_H_