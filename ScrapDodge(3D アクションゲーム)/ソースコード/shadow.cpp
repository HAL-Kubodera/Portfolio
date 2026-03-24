/*============================================================
	[shadow.cpp]
	▼題名 : 影
	▼DATE : 2026/01/16
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "shadow.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"

void Shadow::Init()
{
	VERTEX_3D vertex[VER_NUM];

	vertex[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = { 0.0f,0.0f };

	vertex[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = { 1.0f,0.0f };

	vertex[2].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = { 0.0f , 1.0f };

	vertex[3].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = { 1.0f, 1.0f };


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * VER_NUM;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	m_Texture = Texture::Load("asset\\texture\\shadow.png");
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitDepthTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitDepthTexturePS.cso");

	m_Rotation.x = XM_PIDIV2;
}

void Shadow::Update()
{
	if (Camera::m_BuildMode)return;

	if (m_Parent != nullptr)
	{
		//親の位置を参照してずれを修正
		Vector3 parentPos = m_DefaultPos + m_Parent->GetPosition();
		m_Position.x = parentPos.x;
		m_Position.y = HEIGHT_OFFSET;
		m_Position.z = parentPos.z;

		if (m_FreezeRot == false)
		{
			Vector3 parentRot = m_Parent->GetVelocity();
			parentRot.normalize();
			float rot = 0.0f;

			if (parentRot.x != 0 && parentRot.z != 0)
			{
				rot = parentRot.x / parentRot.z;
			}

			m_Rotation.y = rot;
		}
		
		//親の高さを参照して薄く延ばす
		float scale = 1.0f + (parentPos.y * SCALE_FACTOR);
		m_Scale = m_DefaultScale * scale;
		m_Color.w = 1.0f - (parentPos.y * COLOR_FACTOR);
	}
}

void Shadow::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;

	Renderer::SetWorldMatrix(world);

	//マテリアル設定
	MATERIAL material{};
	material.Diffuse = m_Color;

	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブとポロ時設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Renderer::SetDepthEnable(false);

	//ポリゴン描画
	Renderer::GetDeviceContext()->Draw(VER_NUM, 0);

	Renderer::SetDepthEnable(true);
}

void Shadow::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}