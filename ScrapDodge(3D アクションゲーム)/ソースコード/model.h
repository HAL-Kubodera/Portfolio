/*============================================================
	[model.cpp]
	Å•ëËñº : fbxÇÃÉÇÉfÉã
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _MODEL_H_
#define _MODEL_H_

#include <unordered_map>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

struct MODEL_FBX
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	XMFLOAT4 color;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;
};


MODEL_FBX* ModelLoad(const char* FileName, bool Maya);
void ModelDraw(MODEL_FBX* model, ID3D11ShaderResourceView* SRV = nullptr);
void ModelRelease(MODEL_FBX* model);
void SetModelColor(MODEL_FBX* model, float r, float g, float b, float a);

#endif // _MODEL_H_