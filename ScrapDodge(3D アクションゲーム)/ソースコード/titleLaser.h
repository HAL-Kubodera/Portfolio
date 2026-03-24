/*============================================================
	[titleLaser.h]
	▼題名 : タイトル画面で飛び交っているレーザー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TITLE_LASER_H_
#define _TITLE_LASER_H_

#include "GameObject.h"

class TitleLaser : public GameObject
{
private:
	ID3D11ShaderResourceView* m_Texture = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	struct Particle
	{
		bool Enable;
		int Life;
		Vector3 Position;
		Vector3 Velocity;
	};

	static const int PARTICLE_MAX = 10000;

	float m_Frame;
	float m_MaxFrame = 120;

	Vector3 m_DefaultPosition;

	Particle m_Particle[PARTICLE_MAX];
	
	static constexpr int VER_NUM = 4;
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetDefaultPos(Vector3 pos) 
	{
		m_DefaultPosition = pos; 
		m_Position = m_DefaultPosition;
	}

	void SetMaxFrame(float frame) { m_MaxFrame = frame; }

	const char* GetName() override
	{
		return "TitleLaser";
	}
};

#endif // _TITLE_LASER_H_