/*============================================================
	[particle.h]
	▼題名 : パーティクル
	▼DATE : 2026/01/14
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "GameObject.h"

class Particle : public GameObject
{
public:
	struct PARTICLE
	{
		bool Enable;
		int Life;
		Vector3 Position;
		Vector3 Velocity;
	};

protected:
	ID3D11ShaderResourceView* m_Texture;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	static const int PARTICLE_MAX = 10000;
	PARTICLE m_Particle[PARTICLE_MAX];
	XMFLOAT4 m_Color = {1.0f,1.0f,1.0f,1.0f};
	int m_Life = 0;

	static constexpr int VER_NUM = 4;

private:
	static constexpr int PARTICLE_LIFE = 120;
	static constexpr Vector3 PARTICLE_VELOCITY = { 0.0f,-0.01f,0.0f };

public:

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	virtual void SetColor(XMFLOAT4 color) { m_Color = color; } ;

	const char* GetName() override
	{
		return "Particle";
	}
};

#endif //_EXPLOSION_H