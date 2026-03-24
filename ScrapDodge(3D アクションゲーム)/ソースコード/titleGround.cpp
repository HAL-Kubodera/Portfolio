/*============================================================
	[titleGround.cpp]
	▼題名 : タイトル画面のフィールド
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "titleGround.h"
#include "texture.h"
#include "titleCamera.h"

#define Size (21)

void TitleGround::Init()
{
	m_Position = { 1.0f,-0.5f,0.0f };

	// 頂点バッファ生成
	{
		for (int x = 0; x < Size; x++)
		{
			for (int z = 0; z < Size; z++)
			{
				m_Vertex[x][z].Position =
					XMFLOAT3((x - 10) * 2.5f, 0.0f, (z - 10) * -0.75f);
				m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				m_Vertex[x][z].TexCoord = XMFLOAT2((float)x / (Size - 1), (float)z / (Size - 1));
			}
		}

		//法線ベクトル算出

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * Size * Size;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}

	// インデックスバッファ生成
	{
		unsigned int index[((Size + 1) * 2) * (Size - 1) - 2];

		int i = 0;
		for (int x = 0; x < Size - 1; x++)
		{
			for (int z = 0; z < Size; z++)
			{
				index[i] = x * Size + z;
				i++;

				index[i] = (x + 1) * Size + z;
				i++;
			}

			if (x == Size - 2)
				break;

			index[i] = (x + 1) * Size + Size - 1;
			i++;

			index[i] = (x + 1) * Size;
			i++;
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * (((Size + 1) * 2) * (Size - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}

	// テクスチャ読み込み
	m_Texture = Texture::Load("asset\\texture\\soil.png");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
		"shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader,
		"shader\\unlitTexturePS.cso");
}


void TitleGround::Uninit()
{
	m_VertexBuffer->Release();
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}


void TitleGround::Update()
{
	if (TitleCamera::m_BuildMode) return;

	m_Frame++;

	int move = int(m_Frame / MOVE_PER_FRAME);

	for (int x = 0; x < Size; x++)
	{
		for (int z = 0; z < Size; z++)
		{
			float mx = float(move - x);

			mx = float((int)mx % (Size - 1));

			float u = mx / (Size - 1);
			float v = (float)z / (Size - 1);

			m_Vertex[x][z].TexCoord = { u,v };
		}
	}

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	memcpy(vertex, m_Vertex, sizeof(VERTEX_3D) * Size * Size);

	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);
}

void TitleGround::Draw()
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
	//Renderer::GetDeviceContext()->Draw(21 * 21, 0);
	Renderer::GetDeviceContext()->DrawIndexed((22 * 2) * 20 - 2, 0, 0);

}


