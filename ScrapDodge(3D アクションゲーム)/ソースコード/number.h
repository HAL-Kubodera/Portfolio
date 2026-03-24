/*============================================================
	[number.h]
	Å•ëËñº : êîéö
	Å•DATE : 2025/09/03
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _NUMBER_H_
#define _NUMBER_H_

#include "MyComponent.h"

class Number : public MyComponent
{
private:
	ID3D11ShaderResourceView* m_NumberTexture = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	XMFLOAT4 m_Color = {0.0f,0.0f,0.0,0.0f};

	int m_Number = 0;
	int m_DispNum = 0;
	float m_Distance = 0;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetNumber(int number, int dispNum);
	void SetDistance(float distance) { m_Distance = distance; }

	const char* GetName()
	{
		return m_GuiName.c_str();
	}

	void DispUniqueGui() override
	{
		if (ImGui::TreeNode(GetName()))
		{
			ImGui::DragFloat3("position", &m_Position.x);
			ImGui::DragFloat3("scale", &m_Scale.x);
			ImGui::DragFloat3("rotation", &m_Rotation.x);

			ImGui::InputInt("Number", &m_Number);
			ImGui::InputInt("DispNumber", &m_DispNum);
			ImGui::InputFloat("Distance", &m_Distance);

			ImGui::TreePop();
		}
	}
};

#endif //_NUMBER_H_