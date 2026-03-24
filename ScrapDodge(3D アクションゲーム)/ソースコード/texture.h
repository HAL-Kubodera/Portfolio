/*============================================================
	[texture.h]
	▼題名 : テクスチャの変換
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <unordered_map>
#include <string>
#include <d3d11.h>

class Texture
{
private:
	static std::unordered_map<std::string, ID3D11ShaderResourceView*> m_TexturePool;

public:
	static ID3D11ShaderResourceView* Load(const char* FileName);
};

#endif // _TEXTURE_H_