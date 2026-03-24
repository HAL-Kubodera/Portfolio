/*============================================================
	[wall.h]
	¥‘è–¼ : ƒtƒB[ƒ‹ƒh‚Ì•Ç
	¥DATE : 2026/03/18
	¥AUTHOR : ‹v•ÛŽ› r‰î
============================================================*/
#ifndef _WALL_H_
#define _WALL_H_

#include "GameObject.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class Wall : public GameObject
{
private:
	static struct MODEL_FBX* m_Model;

	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;

	static bool m_IsLoad;

	void DebugSelectColor();
public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "Wall";
	}
};

#endif //_WALL_H_