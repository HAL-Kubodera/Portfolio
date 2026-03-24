/*============================================================
	[flag.cpp]
	▼題名 : タイトル画面の旗
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "flag.h"
#include "texture.h"

void Flag::Init()
{
	m_Position = POSITION;

	// 頂点バッファ生成
	{
		for (int x = 0; x < SIZE; x++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				m_Vertex[x][z].Position =
					XMFLOAT3((x - RADIUS) * VERTEX_SCALE.x, (z - RADIUS) * VERTEX_SCALE.z, 0.0f);
				m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
				m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				m_Vertex[x][z].TexCoord = XMFLOAT2((float)x / (SIZE - 1), (float)z / (SIZE - 1));
			}
		}


		//法線ベクトル算出



		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * SIZE * SIZE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}




	// インデックスバッファ生成
	{
		unsigned int index[((SIZE + 1) * 2) * (SIZE - 1) - 2];

		int i = 0;
		for (int x = 0; x < SIZE - 1; x++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				index[i] = x * SIZE + z;
				i++;

				index[i] = (x + 1) * SIZE + z;
				i++;
			}

			if (x == SIZE - 2)
				break;

			index[i] = (x + 1) * SIZE + SIZE - 1;
			i++;

			index[i] = (x + 1) * SIZE;
			i++;
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * (((SIZE + 1) * 2) * (SIZE - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}




	// テクスチャ読み込み
	m_Texture = Texture::Load("asset\\texture\\titleLogo.png");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
		"shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader,
		"shader\\unlitTexturePS.cso");
}


void Flag::Uninit()
{
	m_VertexBuffer->Release();
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}


void Flag::Update()
{

	for (int x = 0; x < SIZE; x++)
	{
		for (int z = 0; z < SIZE; z++)
		{
			float dx = m_Vertex[z][x].Position.x - m_Vertex[0][0].Position.x;
			float dz = m_Vertex[z][x].Position.y - m_Vertex[0][0].Position.y;
			float length = sqrtf(dx * dx + dz * dz);

			m_Vertex[x][z].Position.z = SIZE_FACTOR * sinf(m_Time * -1.0f + length * LENGTH_FACTOR);

		}
	}

	m_Time += 1.0f / FPS;



	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	memcpy(vertex, m_Vertex, sizeof(VERTEX_3D) * SIZE * SIZE);

	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

}



void Flag::Draw()
{
	// 入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	// シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// ワールドマトリクス設定
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	Renderer::GetDeviceContext()->IASetIndexBuffer(
		m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// マテリアル設定
	MATERIAL material{};
	//ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	// テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画
	Renderer::GetDeviceContext()->DrawIndexed(((SIZE + 1) * 2) * (SIZE - 1) - 2, 0, 0);
}


