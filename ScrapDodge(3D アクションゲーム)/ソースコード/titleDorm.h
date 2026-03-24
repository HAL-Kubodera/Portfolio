/*============================================================
	[titleDorm.h]
	▼題名 : タイトル画面のスカイドーム
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TITLE_DORM_H_
#define _TITLE_DORM_H_

#include "GameObject.h"

class TitleDorm : public GameObject
{
private:
	class MODEL_FBX* m_Model;

	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	static constexpr Vector3 DEFAULT_POS = { 0.0f,-4.0f,0.0f };
	static constexpr Vector3 DEFAULT_SCALE = { 20.0f,20.0f,20.0f };
	static constexpr float ROTATE_SPEED = 0.01f;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "TitleDorm";
	}
};

#endif // _TITLE_DORM_H_