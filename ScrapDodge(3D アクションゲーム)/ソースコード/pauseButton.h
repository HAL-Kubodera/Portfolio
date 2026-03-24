/*
ファイル : pauseButton.h
  名 前  : Shunsuke Kubodera
  日 付  : 2026/02/16
*/
#ifndef _PAUSEBUTTON_H_
#define _PAUSEBUTTON_H_

#include "GameObject.h"

class PauseButton : public GameObject
{
private:
	ID3D11ShaderResourceView* m_Texture = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	int m_SelectIndex = 0;

	static constexpr int BUTTON_NUM = 3;
	static constexpr int START = 0;
	static constexpr int OPTION = 1;
	static constexpr int TITLE = 2;

	static constexpr Vector3 TEXT_POS = { SCREEN_WIDTH / 2,175.0f ,0.0f };
	static constexpr Vector3 BUTTON_POS = { SCREEN_WIDTH / 2,0.0f ,0.0f };
	static constexpr XMFLOAT2 SELECT_POS = { 125.0f,170.0f };
	static constexpr float TEXT_GAP = 200.0f;
	static constexpr int VER_NUM = 4;

	static constexpr XMFLOAT2 SELECT_SIZE = { 500,300 };
	static constexpr XMFLOAT2 DEFAULT_SIZE = { 400,250 };
	static constexpr float SELECT_TEXT_SIZE = 50.0f;
	static constexpr float DEFAULT_TEXT_SIZE = 40.0f;

	class Text* m_Start = nullptr;
	class Text* m_Option = nullptr;
	class Text* m_Title = nullptr;

	class Audio* m_SelectSE = nullptr;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;
	void SetText();

	void UpdateText();

	const char* GetName() override
	{
		return "PauseButton";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{

			ImGui::TreePop();
		}
	}
};

#endif //_PAUSEBUTTON_H