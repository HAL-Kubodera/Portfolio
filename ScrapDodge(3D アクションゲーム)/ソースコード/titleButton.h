/*============================================================
	[titleButton.h]
	Å•ëËñº : É^ÉCÉgÉãâÊñ Ç©ÇÁëJà⁄Ç∑ÇÈÇΩÇﬂÇÃÉ{É^Éì
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _TITLE_BUTTON_H_
#define _TITLE_BUTTON_H_

#include "GameObject.h"

class TitleButton : public GameObject
{
private:
	ID3D11ShaderResourceView* m_Texture = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	int m_SelectIndex = 0;

	static constexpr int BUTTON_NUM = 2;
	static constexpr int START = 0;
	static constexpr int EXIT = 1;

	static constexpr XMFLOAT2 SELECT_POS = { 125.0f,170.0f };
	static constexpr XMFLOAT2 TEXT_POS = { 50.0f,175.0f };
	static constexpr XMFLOAT2 BUTTON_POS = { 1025.0f,230.0f };
	static constexpr float TEXT_GAP = 200.0f;
	static constexpr int VER_NUM = 4;


	static constexpr XMFLOAT2 SELECT_SIZE = { 500,300 };
	static constexpr XMFLOAT2 DEFAULT_SIZE = { 400,250 };
	static constexpr float SELECT_TEXT_SIZE = 50.0f;
	static constexpr float DEFAULT_TEXT_SIZE = 40.0f;

	class Text* m_Start = nullptr;
	class Text* m_Exit = nullptr;

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
		return "TitleButton";
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

#endif //_TITLE_BUTTON_H