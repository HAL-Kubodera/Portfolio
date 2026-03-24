/*============================================================
	[slowGauge.h]
	Å•ëËñº : ÉXÉçÉEÉQÅ[ÉWÇÃUI
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _SLOWGAUGE_H_
#define _SLOWGAUGE_H_

#include "GameObject.h"

class SlowGauge : public GameObject
{
private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static bool m_IsLoad;

	class Player* m_Player = nullptr;

	float m_Gauge = 0;
	float m_MaxGauge = 0;

	static constexpr int VER_NUM = 4;

	static constexpr Vector3 SLOW_BAR_POS = { -50.0f,600.0f,0.0f };
	static constexpr Vector3 SLOW_BAR_SIZE = { 400.0f,150.0f,0.0f };

	static constexpr float START_UV = 0.225f;
	static constexpr float END_UV = 0.53f;

	static constexpr XMFLOAT4 DEFAULT_COLOR = { 0.0,1.0f,0.0f,1.0f };
	static constexpr XMFLOAT4 EMPTY_COLOR = { 0.0f,0.0f,0.0f,1.0f };

	static constexpr Vector3 TEXT_OFFSET = { 60.0f, 40.0f,0.0f };
	static constexpr float TEXT_SIZE = 30.0f;

	void SetRenderer();
	void DrawEmptyBar();
	void DrawSlowGaugeBar();
public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "SlowGauge";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::Text("Status");
			ImGui::InputFloat("HP", &m_Gauge);
			ImGui::InputFloat("MaxHP", &m_MaxGauge);

			ImGui::TreePop();
		}
	}
};

#endif //_SLOWGAUGE_H