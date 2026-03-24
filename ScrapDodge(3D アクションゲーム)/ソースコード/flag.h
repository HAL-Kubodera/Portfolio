/*============================================================
	[flag.h]
	Å•ëËñº : É^ÉCÉgÉãâÊñ ÇÃä¯
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _FLAG_H_
#define _FLAG_H_

#include "gameObject.h"
#include "renderer.h"

class Flag : public GameObject
{
private:

	ID3D11Buffer* m_VertexBuffer{};
	ID3D11Buffer* m_IndexBuffer{};

	ID3D11ShaderResourceView* m_Texture{};
	ID3D11ShaderResourceView* m_EnvTexture{};

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader* m_PixelShader{};
	ID3D11InputLayout* m_VertexLayout{};

	float m_Time = 0.0f;

	static constexpr Vector3 POSITION = { 0.0f,3.0f,15.0f };
	static constexpr int SIZE = 21;
	static constexpr int RADIUS = 10;
	static constexpr Vector3 VERTEX_SCALE = { 0.6f,0.0f,-0.4f };

	static constexpr float SIZE_FACTOR = 0.4f;
	static constexpr float LENGTH_FACTOR = 0.8f;

	static constexpr float FPS = 60.0f;

	VERTEX_3D m_Vertex[SIZE][SIZE];

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	const char* GetName() override
	{
		return "Flag";
	}
};

#endif // _FLAG_H_