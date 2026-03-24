/*
ファイル : pauseScreen.h
  名 前  : Shunsuke Kubodera
  日 付  : 2026/01/22
*/
#ifndef _PAUSESCREEN_H_
#define _PAUSESCREEN_H_

#include "GameObject.h"

class PauseScreen : public GameObject
{
private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;		
	static bool m_IsLoad;



	static constexpr float MAX_FRAME = 40;
	static constexpr int VER_NUM = 4;

public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "PauseScreen";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::Text("Status");

			ImGui::TreePop();
		}
	}
};

#endif //_PAUSESCREEN_H