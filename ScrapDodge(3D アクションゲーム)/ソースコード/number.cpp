/*============================================================
	[number.cpp]
	▼題名 : 数字
	▼DATE : 2025/09/03
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "number.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"

#define VER_NUM (4)	//必要な頂点の数

#define TEX_PATTERNX (5)
#define TEX_PATTERNY (5)

void Number::Init()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0,0 };

	vertex[1].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1,0 };

	vertex[2].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0,1 };

	vertex[3].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = { 1,1 };


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	m_NumberTexture = Texture::Load("asset\\texture\\number.png");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	SetFollow(false);

	m_GuiName = m_GuiName + "Number";
}

void Number::Update()
{

}

void Number::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	int value = m_Number;

	//現在の数
	for (int i = 0; i < m_DispNum; i++) {
		//頂点データ書き換え
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		float x = m_Position.x + (m_Distance + m_Scale.x) * (m_DispNum - 1 - i);
		float y = m_Position.y;
		float w = m_Scale.x;
		float h = m_Scale.y;

		int num = value % 10;
		value /= 10;

		float tw = 1.0f / TEX_PATTERNX;
		float th = 1.0f / TEX_PATTERNY;
		float tx = (num % TEX_PATTERNX) * tw;
		float ty = (num / TEX_PATTERNY) * th;

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = XMFLOAT3(x, y, 0.0f);
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = { tx,ty };

		vertex[1].Position = XMFLOAT3(x + w, y, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = { tx + tw,ty };

		vertex[2].Position = XMFLOAT3(x, y + h, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = { tx,ty + th };

		vertex[3].Position = XMFLOAT3(x + w, y + h, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = { tx + tw, ty + th };

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

		//マトリクス設定
		Renderer::SetWorldViewProjection2D();

		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		trans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		world = scale * rot * trans;

		Renderer::SetWorldMatrix(world);

		//マテリアル設定
		MATERIAL material{};
		material.Diffuse = { 1.0f,1.0f,1.0f,1.0f };
		material.TextureEnable = true;
		Renderer::SetMaterial(material);

		//頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

		//テクスチャ設定
		Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_NumberTexture);

		//プリミティブとポロ時設定
		Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//ポリゴン描画
		Renderer::GetDeviceContext()->Draw(VER_NUM, 0);
	}
}

void Number::SetNumber(int number, int dispNum)
{
	m_Number = number;
	m_DispNum = dispNum;
}

void Number::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}
