/*============================================================
	[texture.cpp]
	▼題名 : テクスチャの変換
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "texture.h"
#include "main.h"
#include "renderer.h"
#include "manager.h"

std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture::m_TexturePool;

ID3D11ShaderResourceView* Texture::Load(const char* FileName)
{
	if (m_TexturePool.count(FileName) > 0)
	{
		return m_TexturePool[FileName];
	}

	wchar_t wFileName[512];
	mbstowcs(wFileName, FileName, strlen(FileName) + 1);

	//テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	ID3D11ShaderResourceView* texture;
	LoadFromWICFile(wFileName, WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
	assert(texture);

	m_TexturePool[FileName] = texture;

	return texture;
}
