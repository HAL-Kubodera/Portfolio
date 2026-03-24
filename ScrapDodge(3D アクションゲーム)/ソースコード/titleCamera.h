/*============================================================
	[titleCamera.h]
	▼題名 : タイトル画面のカメラ
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TITLE_CAMERA_H_
#define _TITLE_CAMERA_H_

#include "GameObject.h"

class TitleCamera : public GameObject
{
private:
	XMMATRIX m_Projection;
	XMMATRIX m_View;

	Vector3 m_Target{0.0f,0.0f,0.0f};

	Vector3 m_ShakeVector = {0.0f,0.0f,0.0f};
	float m_ShakeTime = 0;;

	bool m_IsMovie;
	int m_CurentIndex;
	int m_NextIndex;

	float m_Frame = 0;
	float m_MaxFrame = 60;

	std::vector<Vector3> m_MoviePos;
	std::vector<Vector3> m_MovieRot;
	std::vector<Vector3> m_MovieTarget;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void Shake(Vector3 ShakeVector);
	void SetMovie(int index);

	XMMATRIX GetViewMatrix() { return m_View; }

	//デバッグ用
	static bool m_BuildMode;

	const char* GetName() override
	{
		return "TitleCamera";
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

#endif // _TITLE_CAMERA_H_