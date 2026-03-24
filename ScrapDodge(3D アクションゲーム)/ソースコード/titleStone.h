/*============================================================
	[titleStone.h]
	Å•ëËñº : É^ÉCÉgÉãâÊñ Ç≈ç~Ç¡ÇƒÇ≠ÇÈêŒ
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _TITLE_STONE_H_
#define _TITLE_STONE_H_

#include "GameObject.h"

class TitleStone : public GameObject
{
private:
	struct MODEL_FBX* m_Model;

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	bool m_IsMove = true;
	Vector3 m_DefPos = {};

	float m_Frame = 0;
	float m_DelayFrame = 0;

	static constexpr float MOVE_FRAME = 120;
	static constexpr float SHAKE_FRAME = 80;
	static constexpr float ANIM_FRAME = 100;
	static constexpr float LIFE_TIME = 200;
	
	static constexpr int SHAKE_RANGE = 30;
	static constexpr float ANIM_FACTOR = 1.5f;
	static constexpr float LERP_REDUCE = 10.75f;
	static constexpr float SHAKE_FACTOR = 100;

	static constexpr int LERP_X = 2;
	static constexpr int LERP_Y = 10;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetFrame(float frame) { m_Frame = frame; }
	void SetMove(bool flg) { m_IsMove = flg; }
	void SetDelayFrame(float frame) { m_DelayFrame = frame; }
	void SetDefPos(Vector3 pos) { m_DefPos = m_Position = pos; }
	
	const char* GetName() override
	{
		return "TitleStone";
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

#endif // _TITLE_STONE_H_