/*============================================================
	[model.cpp]
	▼題名 : fbxのモデル
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "model.h"

std::unordered_map<std::string, MODEL_FBX*> g_ModelPool;

MODEL_FBX* ModelLoad( const char *FileName, bool Maya)
{
	if (g_ModelPool.count(FileName) > 0)
	{
		return g_ModelPool[FileName];
	}

	MODEL_FBX* model = new MODEL_FBX;

	const std::string modelPath( FileName );

	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(model->AiScene);

	model->VertexBuffer = new ID3D11Buffer*[model->AiScene->mNumMeshes];
	model->IndexBuffer = new ID3D11Buffer*[model->AiScene->mNumMeshes];


	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				if (Maya)
				{
					vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
					vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
					vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				}
				else
				{
					vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);
					vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
					vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
				}	
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				//assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);

			delete[] index;
		}

	}

	//テクスチャ読み込み
	for(int i = 0; i < (int)model->AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = model->AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture;
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		model->Texture[aitexture->mFilename.data] = texture;
	}

	g_ModelPool[FileName] = model;
	return model;
}

void ModelDraw(MODEL_FBX* model ,ID3D11ShaderResourceView* SRV)
{
	//プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		//テクスチャ設定
		aiString texture;
		aiMaterial* aimaterial = model->AiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		// マテリアル設定
		MATERIAL material = {};

		aiColor4D ambient, diffuse, specular, emission;
		float shininess = 0.0f;

		if (AI_SUCCESS != aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
			ambient = aiColor4D(0.1f, 0.1f, 0.1f, 1.0f);
		if (AI_SUCCESS != aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
			diffuse = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
		if (AI_SUCCESS != aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular))
			specular = aiColor4D(0, 0, 0, 1);
		if (AI_SUCCESS != aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emission))
			emission = aiColor4D(0, 0, 0, 1);
		if (AI_SUCCESS != aimaterial->Get(AI_MATKEY_SHININESS, shininess))
			shininess = 0.0f;

		material.Ambient = XMFLOAT4(ambient.r, ambient.g, ambient.b, ambient.a);
		material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		material.Specular = XMFLOAT4(specular.r, specular.g, specular.b, specular.a);
		material.Emission = XMFLOAT4(emission.r, emission.g, emission.b, emission.a);
		material.Shininess = shininess;

		// テクスチャフラグ
		material.TextureEnable = (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture) == AI_SUCCESS) ? TRUE : FALSE;

		material.Dummy[0] = 0.0f;
		material.Dummy[1] = 0.0f;

		Renderer::SetMaterial(material);

		if (SRV == nullptr)
		{
			if (texture != aiString(""))
				Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &model->Texture[texture.data]);
		}
		else
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &SRV);
		}

		//頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer[m], &stride, &offset);

		//インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(model->IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		//ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}


void ModelRelease(MODEL_FBX* model)
{
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		model->VertexBuffer[m]->Release();
		model->IndexBuffer[m]->Release();
	}

	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : model->Texture)
	{
		pair.second->Release();
	}


	aiReleaseImport(model->AiScene);


	delete model;
}

void SetModelColor(MODEL_FBX* model, float r, float g, float b, float a)
{
	XMFLOAT4 color = model->color;

	if (r == color.x && g == color.y && b == color.z && a == color.w) return;

	model->color.x = r;
	model->color.y = g;
	model->color.z = b;
	model->color.w = a;

	for (UINT m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		//頂点データ書き換え
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(model->VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
			vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			vertex[v].Diffuse = XMFLOAT4(r, g, b, a);
			vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
		}

		Renderer::GetDeviceContext()->Unmap(model->VertexBuffer[m], 0);
	}
}




