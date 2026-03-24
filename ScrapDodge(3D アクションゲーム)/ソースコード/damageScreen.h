/*============================================================
	[damageScreen.h]
	Å•ëËñº : îÌíeéûÇÃâÊñ ÉGÉtÉFÉNÉg
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _DAMAGE_SCREEN_H_
#define _DAMAGE_SCREEN_H_

#include "GameObject.h"

class DamageScreen : public GameObject
{
private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;		
	static bool m_IsLoad;

	static float m_Frame;
	static bool m_IsDamage;

	static constexpr float MAX_FRAME = 40;
	static constexpr int VER_NUM = 4;

public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	static void Damage();

	const char* GetName() override
	{
		return "DamageScreen";
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

#endif //_DAMAGE_SCREEN_H