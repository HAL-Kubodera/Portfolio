/*============================================================
	[stone.h]
	¥‘è–¼ : —Ž‚¿‚Ä‚­‚éŠâ
	¥DATE : 2026/03/18
	¥AUTHOR : ‹v•ÛŽ› r‰î
============================================================*/
#ifndef _STONE_H_
#define _STONE_H_

#include "GameObject.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class Stone : public GameObject
{
private:
	static struct MODEL_FBX* m_Model;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static bool m_IsLoad;

	float m_Frame = 0;
	Vector3 m_DefPos = {};

	static constexpr float SHAKE_TIME = 80;
	static constexpr float LIFE_TIME = 120;

	static constexpr int SHAKE_RANGE = 100;
	static constexpr float ANIM_FACTOR = 1.5f;
	static constexpr float LERP_REDUCE = 10.75f;

	static constexpr int LERP_X = 2;
	static constexpr int LERP_Y = 10;

	static class Audio* m_DropSE;
	static constexpr float SE_VOLUME = 0.5f;
	
public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetCollision();

	static constexpr float ANIM_TIME = 100;
	
	const char* GetName() override
	{
		return "Stone";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::InputFloat("frame", &m_Frame);


			ImGui::TreePop();
		}
	}
};

#endif //_STONE_H_