/*============================================================
	[guide.h]
	▼題名 : チュートリアルのガイド
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _GUIDE_H_
#define _GUIDE_H_

#include "GameObject.h"

enum Guide_Face
{
	Guide_Face_Default,
	Guide_Face_Smile,
	Guide_Face_Sad,

	Guide_Face_Max,
};

class Guide : public GameObject
{
private:
	static ID3D11ShaderResourceView* m_FaceTexture[Guide_Face_Max];
	static ID3D11ShaderResourceView* m_ChatBoxTexture;
	static ID3D11ShaderResourceView* m_CurtainTexture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;

	static bool m_IsLoad;

	Guide_Face m_Face = Guide_Face_Default;
	class Text* m_Text = nullptr;
	class Text* m_Enter = nullptr;

	float m_Frame = 0;
	int m_WardNum = 0;
	float m_AlphaFrame = 0;
	bool m_IsAlpha = false;
	static constexpr float ALPHA_MAX_FRAME = 60;

	bool m_IsDisp = true;

	static constexpr int NUM_PER_FRAME = 7;
	static constexpr int DISP_NUM = 999;

	static constexpr Vector3 CHARA_POS = { 30.0f,350.0f,0.0f };
	static constexpr Vector3 CHARA_SCALE = { 300.0f,450.0f,0.0f };
	static constexpr Vector3 CHATBOX_POS = { 280.0f,300.0f,0.0f };
	static constexpr Vector3 CHATBOX_SCALE = { 1000.0f,450.0f,0.0f };
	static constexpr Vector3 TEXT_POS = { 400.0f,430.0f,0.0f };
	static constexpr float TEXT_SCALE = 40.0f;
	static constexpr int TEXT_RETURN_NUM = 15;

	static constexpr Vector3 ENTER_POS = { 450.0f,660.0f,0.0f };

	static constexpr int VER_NUM = 4;

public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void DrawCurtain();
	void DrawCharacter();
	void DrawChatBox();

	void MoveExplain();
	void JumpExplain();
	void SlowExplain();
	void ShotExplain();
	void GameExplain();
	void RetryExplain();

	bool GetIsFinish();

	const char* GetName() override
	{
		return "Guide";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			const char* modes[Guide_Face_Max] = { "Default","Smile" };

			if (ImGui::BeginCombo("Mode", modes[m_Face]))
			{
				for (int tagNum = 0; tagNum < Guide_Face_Max; tagNum++)
				{
					if (ImGui::Selectable(modes[tagNum]))
					{
						m_Face = (Guide_Face)tagNum;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Text("Status");


			ImGui::TreePop();
		}
	}
};

#endif //_GUIDE_H