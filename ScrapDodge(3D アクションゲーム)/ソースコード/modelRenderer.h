/*============================================================
	[modelRenderer.h]
	▼題名 : objのモデル
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _MODEL_RENDERER_H_
#define _MODEL_RENDERER_H_

// マテリアル構造体
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
	ID3D11ShaderResourceView*	Texture;

};


// 描画サブセット構造体
struct SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	MODEL_MATERIAL	Material;
};


// モデル構造体
struct MODEL_OBJ
{
	VERTEX_3D		*VertexArray;
	unsigned int	VertexNum;

	unsigned int	*IndexArray;
	unsigned int	IndexNum;

	SUBSET			*SubsetArray;
	unsigned int	SubsetNum;
};

struct MODEL
{
	ID3D11Buffer*	VertexBuffer;
	ID3D11Buffer*	IndexBuffer;

	SUBSET*			SubsetArray;
	unsigned int	SubsetNum;
};


#include "component.h"
#include <string>
#include <unordered_map>


class ModelRenderer : public Component
{
private:

	static std::unordered_map<std::string, MODEL*> m_ModelPool;

	static void LoadModel(const char *FileName, MODEL *Model);
	static void LoadMaterial( const char *FileName, MODEL_MATERIAL **MaterialArray, unsigned int *MaterialNum );

	MODEL* m_Model{};

public:
	static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj);
	static void Preload( const char *FileName );
	static void UnloadAll();


	using Component::Component;

	void Load( const char *FileName );
	void Draw() override;

	ID3D11Buffer* GetVertexBuffer() { return m_Model->VertexBuffer; }
};

#endif //_MODEL_RENDERER_H_