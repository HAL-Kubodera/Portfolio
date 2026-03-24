#pragma once

#include "GameObject.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class Polygon2D : public GameObject
{
private:
	 ID3D11Buffer* m_VertexBuffer;

	 ID3D11VertexShader* m_VertexShader;
	 ID3D11PixelShader* m_PixelShader;
	 ID3D11InputLayout* m_VertexLayout;

	 ID3D11ShaderResourceView* m_Texture;

	 float m_Row;
	 float m_Col;

	 float m_MaxRow;
	 float m_MaxCol;

	 static constexpr int VER_NUM = 4;

public:
	Polygon2D()
	{
		SetUV(1, 1, 1, 1);
	}

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetInfo(Vector3 Position, Vector3 Scale, Vector3 Rotation, const char* fileName);

	void SetSize(float Width, float Height);
	void SetUV(float Row, float Col, float MaxRow, float MaxCol);
	void SetTexture(const char* fileName);
};