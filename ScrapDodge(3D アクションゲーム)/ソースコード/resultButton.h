/*============================================================
	[resultButton.h]
	Å•ëËñº : ÉäÉUÉãÉgâÊñ Ç©ÇÁëJà⁄Ç∑ÇÈÇΩÇﬂÇÃÉ{É^Éì
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _RESULT_BUTTON_H_
#define _RESULT_BUTTON_H_

#include "GameObject.h"

class ResultButton : public GameObject
{
private:
	ID3D11ShaderResourceView* m_Texture = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	int m_SelectIndex = 0;

	static constexpr int BUTTON_NUM = 2;
	static constexpr int RETRY = 0;
	static constexpr int TITLE = 1;

	static constexpr XMFLOAT2 SELECT_POS = { 205.0f,-25.0f };
	static constexpr XMFLOAT2 TEXT_POS = { 230.0f,-20.0f };
	static constexpr XMFLOAT2 BUTTON_POS = { 330.0f,590.0f };
	static constexpr float TEXT_GAP = 600.0f;
	static constexpr float SELECT_TEXT_SIZE = 50.0f;
	static constexpr float DEFAULT_TEXT_SIZE = 40.0f;
	static constexpr XMFLOAT2 SELECT_BUTTON_SIZE = { 500,300 };
	static constexpr XMFLOAT2 DEFAULT_BUTTON_SIZE = { 400,250 };

	static constexpr int VER_NUM = 4;

	class Text* m_Retry = nullptr;
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
		return "ResultButton";
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

#endif //_RESULT_BUTTON_H