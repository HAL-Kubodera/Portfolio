/*============================================================
	[fade.h]
	▼題名 : フェード画面
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _FADE_H_
#define _FADE_H_

class Fade
{
private:
	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;

	static XMFLOAT4 m_Color;

	static bool m_IsFadeIn;
	static bool m_IsActive;
	static bool m_ChengeScene;

	static int m_Frame;
	static int m_FadeFrame;

	static constexpr int VER_NUM = 4;

public:
	static void Init();
	static void Update();
	static void Draw();
	static void Uninit();

	static bool GetActive() { return m_IsActive; }
	static void SetFadeIn(int frame);
	static void SetFadeOut(int frame);
	static void SetFadeScene(int frame);
};

#endif //_PLAYERHP_H