/*============================================================
	[shadow.h]
	Å•ëËñº : âe
	Å•DATE : 2026/01/16
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _SHADOW_H_
#define _SHADOW_H_

#include "MyComponent.h"

class Shadow : public MyComponent
{
private:
	ID3D11ShaderResourceView* m_Texture;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	GameObject* m_Parent;
	Vector3 m_DefaultPos;
	Vector3 m_DefaultScale;

	bool m_FreezeRot = true;

	static constexpr int VER_NUM = 4;
	static constexpr float HEIGHT_OFFSET = 0.01f;
	static constexpr float SCALE_FACTOR = 0.5f;
	static constexpr float COLOR_FACTOR = 0.1f;

public:

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetFreezeRot(bool flg) { m_FreezeRot = flg; }
	void SetParent(GameObject* parent) { m_Parent = parent; }
	void SetDefaultPos(Vector3 pos) { m_DefaultPos = pos; }
	void SetDefaultScale(Vector3 scale) { m_DefaultScale = scale; }

	const char* GetName() override
	{
		return "Shadow";
	}

	void DispUniqueGui() override
	{
		if (ImGui::TreeNode(GetName()))
		{

			ImGui::TreePop();
		}
	}
};

#endif //_SHADOW_H