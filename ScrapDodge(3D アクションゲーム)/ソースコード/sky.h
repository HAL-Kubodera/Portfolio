/*============================================================
	[sky.h]
	▼題名 : スカイドーム
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _SKY_H_
#define _SKY_H_

#include "GameObject.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class Sky : public GameObject
{
private:
	static struct MODEL_FBX* m_Model;

	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;

	static bool m_IsLoad;

	static constexpr Vector3 DEFAULT_SCALE{ 50.0f,50.0f,50.0f };

public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "Sky";
	}
};

#endif //_SKY_H_