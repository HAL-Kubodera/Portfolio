/*============================================================
	[titleGround.h]
	▼題名 : タイトル画面のフィールド
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TITLE_GROUND_H_
#define _TITLE_GROUND_H_

#include "gameObject.h"

class TitleGround : public GameObject
{
private:

	ID3D11Buffer* m_VertexBuffer{};
	ID3D11Buffer* m_IndexBuffer{};

	ID3D11ShaderResourceView* m_Texture{};
	ID3D11ShaderResourceView* m_EnvTexture{};

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader* m_PixelShader{};
	ID3D11InputLayout* m_VertexLayout{};

	VERTEX_3D m_Vertex[21][21];

	float m_Frame = 0;
	float MOVE_PER_FRAME = 5;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	const char* GetName() override
	{
		return "TitleGround";
	}
};

#endif // TITLE_GROUND_H_