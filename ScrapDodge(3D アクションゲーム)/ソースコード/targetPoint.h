/*============================================================
	[targetPoint.h]
	Å•ëËñº : çUåÇñ⁄ïWÇÃï\é¶
	Å•DATE : 2025/12/29
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _TARGETPOINT_H_
#define _TARGETPOINT_H_

#include "GameObject.h"

class TargetPoint : public GameObject
{
private:
	ID3D11ShaderResourceView* m_Texture;
	ID3D11ShaderResourceView* m_KeyTexture;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	static constexpr int VER_NUM = 4;

	static constexpr Vector3 CAMERA_SHAKE = { 6.0f,6.0f,2.0f };
	static constexpr int HITSTOP = 10;

	static constexpr Vector3 TARGET_SCALE = { 2.8f, 2.8f, 1.0f };
public:

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "TargetPoint";
	}
};

#endif //_TARGETPOINT_H