/*============================================================
	[enemyHp.h]
	Å•ëËñº : ìGÇÃëÃóÕUI
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _ENEMYHP_H_
#define _ENEMYHP_H_

#include "GameObject.h"

class EnemyHp : public GameObject
{
private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static bool m_IsLoad;

	class Enemy* m_Enemy;

	float m_Hp = 0;
	float m_DispHp = 0;
	float m_MaxHp = 0;

	int m_Frame = 0;

	float m_Diff = 0;

	//ÉXÉeÅ[É^ÉX
	static constexpr int DISP_HP_FRAME = 45;
	static constexpr int REDUCE_ANIM_FRAME = 15;

	static constexpr int VER_NUM = 4;

	static constexpr Vector3 HP_BAR_POS = { 660.0f,-60.0f,0.0f };
	static constexpr Vector3 HP_BAR_SIZE = { 800.0f,300.0f,0.0f };

	static constexpr float START_UV = 0.225f;
	static constexpr float END_UV = 0.53f;

	static constexpr float PINCH_HP_RATIO = 0.4f;

	static constexpr XMFLOAT4 DEFAULT_COLOR = { 0.0,1.0f,0.0f,1.0f };
	static constexpr XMFLOAT4 DAMAGE_COLOR = { 1.0,0.0f,0.0f,1.0f };
	static constexpr XMFLOAT4 REDUCE_COLOR = { 1.0f,1.0f,0.0f,1.0f };
	static constexpr XMFLOAT4 EMPTY_COLOR = { 0.0f,0.0f,0.0f,1.0f };

	static constexpr Vector3 TEXT_OFFSET = { 190.0f, 80.0f,0.0f };
	static constexpr float TEXT_SIZE = 50.0f;
public:
	
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetRenderer();
	void DrawEmptyBar();
	void DrawReduceBar();
	void DrawHpBar();

	const char* GetName() override
	{
		return "EnemyHp";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::Text("Status");
			ImGui::InputFloat("HP", &m_Hp);
			ImGui::InputFloat("MaxHP", &m_MaxHp);
			ImGui::InputFloat("DispHP", &m_DispHp);

			ImGui::Text("frame");
			ImGui::InputInt("frame", &m_Frame);


			ImGui::TreePop();
		}
	}
};

#endif //_ENEMYHP_H