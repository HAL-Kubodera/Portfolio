/*============================================================
	[meshField.cpp]
	▼題名 : フィールド
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "meshField.h"
#include "texture.h"

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

std::vector<ID3D11ShaderResourceView*> MeshField::m_Textures;

ID3D11VertexShader* MeshField::m_VertexShader;
ID3D11PixelShader* MeshField::m_PixelShader;
ID3D11InputLayout* MeshField::m_VertexLayout;
bool MeshField::m_IsLoad = false;

void MeshField::Load()
{
	ID3D11ShaderResourceView* texture = Texture::Load("asset\\texture\\block.png");
	m_Textures.push_back(texture);
	texture = Texture::Load("asset\\texture\\block.png");
	m_Textures.push_back(texture);
	texture = Texture::Load("asset\\texture\\debugTile.png");
	m_Textures.push_back(texture);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\fieldVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\fieldPS.cso");

	m_IsLoad = true;
}

void MeshField::Init()
{
	if (!m_IsLoad)Load();

	LoadMap("asset\\map\\MapHeight.txt");
	LoadTexture("asset\\map\\MapTexture.txt");

	// 頂点バッファ生成
	{
		for (int x = 0; x <= MAPSIZE - 1; x++)
		{
			for (int z = 0; z <= MAPSIZE - 1; z++)
			{
				float y = g_FieldHeight[x][z];
				float a = g_FieldTexture[x][z];

				float width = WIDTH_MIN + z * WIDTH_FACTOR;
				if (width > WIDTH_MAX)width = WIDTH_MAX;

				m_Vertex[x][z].Position = XMFLOAT3((x - ((MAPSIZE - 1) / 2)) * width, y, (z - ((MAPSIZE - 1) / 2)) * -WIDTH_MAX);
				m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);//法線ベクトル
				m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, a);
				m_Vertex[x][z].TexCoord = XMFLOAT2(x * 0.5f, z * 0.5f);
			}
		}

		for (int x = 1; x < MAPSIZE - 1; x++)
		{
			for (int z = 1; z < MAPSIZE - 1; z++)
			{
				Vector3 vx, vz, vn;
				vx.x = m_Vertex[x + 1][z].Position.x - m_Vertex[x - 1][z].Position.x;
				vx.y = m_Vertex[x + 1][z].Position.y - m_Vertex[x - 1][z].Position.y;
				vx.z = m_Vertex[x + 1][z].Position.z - m_Vertex[x - 1][z].Position.z;

				vz.x = m_Vertex[x][z - 1].Position.x - m_Vertex[x][z + 1].Position.x;
				vz.y = m_Vertex[x][z - 1].Position.y - m_Vertex[x][z + 1].Position.y;
				vz.z = m_Vertex[x][z - 1].Position.z - m_Vertex[x][z + 1].Position.z;

				vn = Vector3::cross(vz, vx);
				vn.normalize();

				m_Vertex[x][z].Normal.x = vn.x;
				m_Vertex[x][z].Normal.y = vn.y;
				m_Vertex[x][z].Normal.z = vn.z;
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * MAPSIZE * MAPSIZE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}




	// インデックスバッファ生成
	{
		unsigned int index[((MAPSIZE + 1) * 2) * (MAPSIZE - 1) - 2 ];//124

		int i = 0;
		for (int x = 0; x < MAPSIZE - 1; x++)
		{
			for (int z = 0; z < MAPSIZE; z++)
			{
				index[i] = x * MAPSIZE + z;
				i++;

				index[i] = (x + 1) * MAPSIZE + z;
				i++;
			}

			if (x == MAPSIZE - 2)
				break;

			//縮退ポリゴン
			index[i] = (x + 1) * MAPSIZE + MAPSIZE - 1;
			i++;

			index[i] = (x + 1) * MAPSIZE;
			i++;
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * (((MAPSIZE + 1) * 2) * (MAPSIZE - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}

	m_IsDebug = false;
}


void MeshField::Uninit()
{
}


void MeshField::Update()
{
	
}


void MeshField::Draw()
{
#ifdef _DEBUG

	
#endif

	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// 頂点バッファ設定
	UINT stride = sizeof( VERTEX_3D );
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );

	// インデックスバッファ設定
	Renderer::GetDeviceContext()->IASetIndexBuffer( 
		m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// マテリアル設定
	MATERIAL material;
	ZeroMemory( &material, sizeof(material) );
	material.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	material.TextureEnable = true;
	Renderer::SetMaterial( material );

	// テクスチャ設定
	if(!m_IsDebug)
	{
		for(int i =0;i<m_Textures.size() - 1;i++)
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(i, 1, &m_Textures[i]);
		}
	}
	else
	{
		for (int i = 0; i < m_Textures.size() - 1; i++)
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(i, 1, &m_Textures[m_Textures.size() - 1]);
		}
	}

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// ポリゴン描画
	Renderer::GetDeviceContext()->DrawIndexed(
		((MAPSIZE + 1) * 2)* (MAPSIZE - 1) - 2, 0, 0);

	
}

void MeshField::SaveMap(const char* file)
{
	// ファイルを開く
	std::ofstream ofs(file);

	if (!ofs) return;
	

	for (int i = 0; i < MAPSIZE; i++) 
	{
		for (int j = 0; j < MAPSIZE; j++) 
		{
			ofs << std::fixed << std::setprecision(1) << g_FieldHeight[i][j];
			if (j < MAPSIZE - 1) ofs << " ";
		}
		ofs << "\n";
	}

	ofs.close();
}

void MeshField::SaveTexture(const char* file)
{
	// ファイルを開く
	std::ofstream ofs(file);

	if (!ofs) return;


	for (int i = 0; i < MAPSIZE; i++) 
	{
		for (int j = 0; j < MAPSIZE; j++) 
		{
			ofs << std::fixed << std::setprecision(1) << g_FieldTexture[i][j];
			if (j < MAPSIZE - 1) ofs << " ";
		}
		ofs << "\n";
	}

	ofs.close();
}

void MeshField::LoadMap(const char* filename)
{
	std::ifstream ifs(filename);

	if (!ifs) return;

	for (int i = 0; i < MAPSIZE; i++) 
	{
		for (int j = 0; j < MAPSIZE; j++) 
		{
			ifs >> g_FieldHeight[i][j];
		}
	}
	ifs.close();
}

void MeshField::LoadTexture(const char* filename)
{
	std::ifstream ifs(filename);

	if (!ifs) return;
	

	for (int i = 0; i < MAPSIZE; i++) 
	{
		for (int j = 0; j < MAPSIZE; j++) 
		{
			ifs >> g_FieldTexture[i][j];
		}
	}
	ifs.close();
}

float MeshField::GetHeight(Vector3 Position)
{
	int x, z;

	//ブロック番号算出
	x = int(Position.x / 5.0f + 10.0f);
	z = int(Position.z / -5.0f + 10.0f);

	XMFLOAT3 pos0, pos1, pos2, pos3;

	pos0 = m_Vertex[x][z].Position;
	pos1 = m_Vertex[x + 1][z].Position;
	pos2 = m_Vertex[x][z + 1].Position;
	pos3 = m_Vertex[x + 1][z + 1].Position;

	Vector3 v12, v1p;
	v12.x = pos2.x - pos1.x;
	v12.y = pos2.y - pos1.y;
	v12.z = pos2.z - pos1.z;

	v1p.x = Position.x - pos1.x;
	v1p.y = Position.y - pos1.y;
	v1p.z = Position.z - pos1.z;

	//外積
	float cy = v12.z * v1p.x - v12.x * v1p.z;

	float py;
	Vector3 n;
	if (cy > 0.0f)
	{
		//左上ポリゴン
		Vector3 v10;
		v10.x = pos0.x - pos1.x;
		v10.y = pos0.y - pos1.y;
		v10.z = pos0.z - pos1.z;

		//外積
		n = Vector3::cross(v10, v12);
	}
	else
	{
		//右下ポリゴン
		Vector3 v13;
		v13.x = pos3.x - pos1.x;
		v13.y = pos3.y - pos1.y;
		v13.z = pos3.z - pos1.z;

		//外積
		n = Vector3::cross(v12, v13);
	}

	//高さ取得
	py = -((Position.x - pos1.x) * n.x + (Position.z - pos1.z) * n.z) / n.y + pos1.y;

	return py;
}


