/*============================================================
	[loadText.h]
	Å•ëËñº : LOADÇÃï∂éö
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _LOADTEXT_H_
#define _LOADTEXT_H_

#include "GameObject.h"
#include "modelRenderer.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class LoadText : public GameObject
{
private:
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	enum AnimationType
	{
		DownBlend,
		UpReset,
	};

	class Audio* m_SE = nullptr;

	static constexpr int TEXT_NUM = 4;
	static constexpr int MORPHING_NUM = 2;

	struct TextModel
	{
		class ModelRenderer* modelRenderer = nullptr;;
		MODEL_OBJ modelObj[MORPHING_NUM] = {};

		AnimationType animType = DownBlend;

		Vector3 position = { 0.0f,0.0f,0.0f };
		Vector3 rotation = { 0.0f,0.0f,0.0f };
		Vector3 scale = { 0.0f,0.0f,0.0f };
		Vector3 velocity = { 0.0f,0.0f,0.0f };

		float blendFrame = 0.0f;

		bool isAnim = false;
		bool isBlend = false;
	};
	
	std::vector<TextModel> m_Model;

	int m_Index = 0;
	float m_Blend = 0.0f;
	float m_Frame = 0.0f;

	//íËêî
	static constexpr float FIRST_GRAVITY = -0.45f; //èâë¨ÇÃèdóÕ
	static constexpr float BUOYANCY = 0.03f; //ïÇóÕ
	static constexpr float BLEND_MAX_FRAME = 5;
	static constexpr float TEXT_GAP = 5.0f;
	static constexpr int ANIM_START = 10;

	static constexpr float SE_VOLUME = 0.5f;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetAnimation(int i,AnimationType type);

	const char* GetName() override
	{
		return "TitleText";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			for (int i = 0; i < TEXT_NUM; i++)
			{
				std::string label = "Text : " + std::to_string(i);

				if (ImGui::TreeNode(label.c_str()))
				{
					ImGui::Text("TextModel");
					ImGui::InputFloat3("position", &m_Model[i].position.x);
					ImGui::InputFloat3("rotation", &m_Model[i].rotation.x);
					ImGui::InputFloat3("scale", &m_Model[i].scale.x);
					ImGui::InputFloat3("velocity", &m_Model[i].velocity.x);
					ImGui::InputFloat("blendFrame", &m_Model[i].blendFrame);
					ImGui::Checkbox("isAnim", &m_Model[i].isAnim);
					ImGui::Checkbox("isBlend", &m_Model[i].isBlend);

					ImGui::Text("Function");

					if(ImGui::Button("DownBlend"))SetAnimation(i,DownBlend);
					if (ImGui::Button("UpReset"))SetAnimation(i, UpReset);

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}
};

#endif //_LOADTEXT_H_