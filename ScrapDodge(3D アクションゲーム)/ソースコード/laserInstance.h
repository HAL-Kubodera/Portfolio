/*============================================================
	[laserInstance.h]
	▼題名 : 敵のレーザーのインスタンス描画
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _LASERINSTANCE_H_
#define _LASERINSTANCE_H_

#include "GameObject.h"

class LaserInstance : public GameObject
{
private:
	ID3D11ShaderResourceView* m_Texture;

	ID3D11Buffer* m_PositionBuffer;
	ID3D11ShaderResourceView* m_PositionSRV;

	ID3D11Buffer* m_VertexBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	int m_LaserNum = 0;

	static constexpr int VER_NUM = 4;
	static constexpr int LASER_NUM = 100000;

public:

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "LaserInstance";
	}
};

#endif //_LASERINSTANCE_H