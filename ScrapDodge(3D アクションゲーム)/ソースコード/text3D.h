/*============================================================
	[text3D.h]
	▼題名 : ビルボードのテキスト
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TEXT_3D_H_
#define _TEXT_3D_H_

#include "myComponent.h"

class Text3D : public MyComponent
{
private:
	ID3D11ShaderResourceView* m_HiraTexture;
	ID3D11ShaderResourceView* m_VoicedHiraTexture;
	ID3D11ShaderResourceView* m_KataTexture;
	ID3D11ShaderResourceView* m_VoicedKataTexture;
	ID3D11ShaderResourceView* m_AlphaTexture;
	ID3D11ShaderResourceView* m_SpecialTexture;

	ID3D11Buffer* m_VertexBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	Vector3 m_Gap;
	int m_DispNum = -1;
	int m_ReturnNum = -1;
	const char* m_Ward = "\0";

	static constexpr int VER_NUM = 4;

	static constexpr int TEX_WIDTH = 10;
	static constexpr int TEX_HEIGHT = 5;
	static constexpr int HIRAGANA = -126;
	static constexpr int KATAKANA = -125;
	static constexpr int SPECIAL = -127;

	static constexpr int CHAR_BYTE = 2;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetGap(Vector3 gap) { m_Gap = gap; }
	void SetGap(float x, float y, float z)
	{
		m_Gap.x = x;
		m_Gap.y = y;
		m_Gap.z = z;
	}
	void SetDispNum(int num) { m_DispNum = num; }
	void SetReturnNum(int num) { m_ReturnNum = num; }
	void SetWard(const char* ward) { m_Ward = ward; }
	int GetWardNum();

	const char* GetName() override
	{
		return "Text3D";
	}

	void DispUniqueGui() override
	{
		if (ImGui::TreeNode(GetName()))
		{
			MyComponent::DispUniqueGui();

			ImGui::DragFloat3("gap", &m_Gap.x);

			ImGui::TreePop();
		}
	}
};

#endif //_TEXT_3D_H