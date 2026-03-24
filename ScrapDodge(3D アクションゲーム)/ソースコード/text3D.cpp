/*============================================================
	[text3D.cpp]
	▼題名 : ビルボードのテキスト
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "polygon.h"
#include "text3d.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "texture.h"
#include "number.h"

void Text3D::Init()
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

	m_HiraTexture = Texture::Load("asset\\texture\\HiraText.png");
	m_VoicedHiraTexture = Texture::Load("asset\\texture\\VoicedHiraText.png");
	m_KataTexture = Texture::Load("asset\\texture\\KataText.png");
	m_VoicedKataTexture = Texture::Load("asset\\texture\\VoicedKataText.png");
	m_AlphaTexture = Texture::Load("asset\\texture\\AlphaText.png");
	m_SpecialTexture = Texture::Load("asset\\texture\\SpecialText.png");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Text3D::Update()
{
	if (Camera::m_BuildMode) return;

	
}

void Text3D::Draw()
{
	
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	Camera* camera = Manager::GetScene()->GetGameObject<Camera>();
	XMMATRIX view = camera->GetViewMatrix();

	//ビューの逆行列
	XMMATRIX invView;
	invView = XMMatrixInverse(nullptr, view);//逆行列
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	XMMATRIX world, scale, trans;
	scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	trans = XMMatrixTranslation(m_Position.x, m_Position.y , m_Position.z);
	world = scale * invView * trans;

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

	//プリミティブとポロ時設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Renderer::SetDepthEnable(false);

	const char* fontHiraTable = "あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん";
	const char* fontKataTable = "アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン";
	const char* fontVoicedHiraTable = "がぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽゃゅょ";
	const char* fontVoicedKataTable = "ガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポャュョ";
	const char* fontSpecialTable = "！？ー";

	int num = 0;
	ID3D11ShaderResourceView* texture = nullptr;

	for (int i = 0; m_Ward[i] != '\0'; )
	{
		if (m_DispNum == num) break;

		int index = 0;
		char char1 = m_Ward[i];
		int returnNum = 0;
		if (m_ReturnNum != -1)
			returnNum = num / m_ReturnNum;

		if (char1 == HIRAGANA)
		{
			char char2 = m_Ward[i + 1];

			for (int j = 0; fontHiraTable[j * CHAR_BYTE] != '\0'; j++)
			{
				if (fontHiraTable[j * CHAR_BYTE] == char1 && fontHiraTable[j * CHAR_BYTE + 1] == char2)
				{
					index = j;
					i += CHAR_BYTE;

					if (texture != m_HiraTexture)
					{
						texture = m_HiraTexture;

						//テクスチャ設定
						Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
					}

					break;
				}
			}

			for (int j = 0; fontVoicedHiraTable[j * CHAR_BYTE] != '\0'; j++)
			{
				if (fontVoicedHiraTable[j * CHAR_BYTE] == char1 && fontVoicedHiraTable[j * CHAR_BYTE + 1] == char2)
				{
					index = j;
					i += CHAR_BYTE;

					if (texture != m_VoicedHiraTexture)
					{
						texture = m_VoicedHiraTexture;

						//テクスチャ設定
						Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
					}

					break;
				}
			}
		}
		else if (char1 == KATAKANA)
		{
			char char2 = m_Ward[i + 1];

			for (int j = 0; fontKataTable[j * CHAR_BYTE] != '\0'; j++)
			{
				if (fontKataTable[j * CHAR_BYTE] == char1 && fontKataTable[j * CHAR_BYTE + 1] == char2)
				{
					index = j;
					i += CHAR_BYTE;

					if (texture != m_KataTexture)
					{
						texture = m_KataTexture;

						//テクスチャ設定
						Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
					}

					break;
				}
			}

			for (int j = 0; fontVoicedKataTable[j * CHAR_BYTE] != '\0'; j++)
			{
				if (fontVoicedKataTable[j * CHAR_BYTE] == char1 && fontVoicedKataTable[j * CHAR_BYTE + 1] == char2)
				{
					index = j;
					i += CHAR_BYTE;

					if (texture != m_VoicedKataTexture)
					{
						texture = m_VoicedKataTexture;

						//テクスチャ設定
						Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
					}

					break;
				}
			}
		}
		else if (char1 == SPECIAL)
		{
			char char2 = m_Ward[i + 1];

			for (int j = 0; fontSpecialTable[j * CHAR_BYTE] != '\0'; j++)
			{
				if (fontSpecialTable[j * CHAR_BYTE] == char1 && fontSpecialTable[j * CHAR_BYTE + 1] == char2)
				{
					index = j;
					i += CHAR_BYTE;

					if (texture != m_SpecialTexture)
					{
						texture = m_SpecialTexture;

						//テクスチャ設定
						Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
					}

					break;
				}
			}
		}
		else
		{
			index = char1 - 'A';

			if (texture != m_AlphaTexture)
			{
				texture = m_AlphaTexture;

				//テクスチャ設定
				Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
			}

			i++;
		}

		//頂点データ書き換え
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		float w = m_Scale.x;
		float h = m_Scale.y;

		float x = ((num - (returnNum * m_ReturnNum)) * m_Gap.x) - (GetWardNum() / 2) * m_Gap.x;;
		float y = (returnNum * m_Gap.y);

		int u = (index % TEX_WIDTH);
		int v = (index / TEX_WIDTH);

		float tw = 1.0f / TEX_WIDTH;
		float th = 1.0f / TEX_HEIGHT;
		float tx = u * tw;
		float ty = v * th;

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = XMFLOAT3(x, y + h, 0.0f);
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = m_Color;
		vertex[0].TexCoord = { tx,ty };

		vertex[1].Position = XMFLOAT3(x + w, y + h, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = m_Color;
		vertex[1].TexCoord = { tx + tw,ty };

		vertex[2].Position = XMFLOAT3(x, y, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = m_Color;
		vertex[2].TexCoord = { tx,ty + th };

		vertex[3].Position = XMFLOAT3(x + w, y, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = m_Color;
		vertex[3].TexCoord = { tx + tw, ty + th };

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);
		
		//ポリゴン描画
		Renderer::GetDeviceContext()->Draw(VER_NUM, 0);

		num++;
	}

	
	Renderer::SetDepthEnable(true);
}

void Text3D::Uninit()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

int Text3D::GetWardNum()
{
	const char* fontHiraTable = "あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん";
	const char* fontKataTable = "アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン";
	const char* fontVoicedHiraTable = "がぎぐげござじずぜぞだぢづでどばびぶべぼ";
	const char* fontVoicedKataTable = "ガギグゲゴザジズゼゾダヂヅデドバビブベボ";
	const char* fontSpecialTable = "！？ー";

	int num = 0;

	for (int i = 0; m_Ward[i] != '\0'; )
	{
		char char1 = m_Ward[i];

		if (char1 == HIRAGANA)
		{
			i += CHAR_BYTE;
		}
		else if (char1 == KATAKANA)
		{
			i += CHAR_BYTE;
		}
		else if (char1 == SPECIAL)
		{
			i += CHAR_BYTE;
		}
		else
		{
			i++;
		}

		num++;
	}

	return num;
}
