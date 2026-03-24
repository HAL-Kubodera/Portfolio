/*============================================================
	[loadText.cpp]
	▼題名 : LOADの文字
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "loadtext.h"
#include "audio.h"
#include "manager.h"
#include "camera.h"
#include "keyboard.h"
#include "modelRenderer.h"

void LoadText::Init()
{
	for (int i = 0; i < TEXT_NUM; i++)
	{
		m_Model.push_back(TextModel());
		m_Model[i].modelRenderer = new ModelRenderer();
		m_Model[i].position.x = (float)i * TEXT_GAP;
	}

	m_Model[0].modelRenderer->Load("asset\\model\\Text\\l0.obj");
	ModelRenderer::LoadObj("asset\\model\\Text\\l0.obj", &m_Model[0].modelObj[0]);
	ModelRenderer::LoadObj("asset\\model\\Text\\l2.obj", &m_Model[0].modelObj[1]);

	m_Model[1].modelRenderer->Load("asset\\model\\Text\\o1.obj");
	ModelRenderer::LoadObj("asset\\model\\Text\\o1.obj", &m_Model[1].modelObj[0]);
	ModelRenderer::LoadObj("asset\\model\\Text\\o2.obj", &m_Model[1].modelObj[1]);

	m_Model[2].modelRenderer->Load("asset\\model\\Text\\a1.obj");
	ModelRenderer::LoadObj("asset\\model\\Text\\a1.obj", &m_Model[2].modelObj[0]);
	ModelRenderer::LoadObj("asset\\model\\Text\\a2.obj", &m_Model[2].modelObj[1]);

	m_Model[3].modelRenderer->Load("asset\\model\\Text\\d1.obj");
	ModelRenderer::LoadObj("asset\\model\\Text\\d1.obj", &m_Model[3].modelObj[0]);
	ModelRenderer::LoadObj("asset\\model\\Text\\d2.obj", &m_Model[3].modelObj[1]);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");

	for (int i = 0; i < TEXT_NUM; i++)
	{
		//モーフィング
		ID3D11Buffer* vertexBuffer = m_Model[i].modelRenderer->GetVertexBuffer();

		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		for (int v = 0; v < (int)m_Model[i].modelObj[0].VertexNum; v++)
		{
			vertex[v] = m_Model[i].modelObj[0].VertexArray[v];

			vertex[v].Position.x = (m_Model[i].modelObj[0].VertexArray[v].Position.x * (1.0f));
			vertex[v].Position.y = (m_Model[i].modelObj[0].VertexArray[v].Position.y * (1.0f));
			vertex[v].Position.z = (m_Model[i].modelObj[0].VertexArray[v].Position.z * (1.0f));
		}

		Renderer::GetDeviceContext()->Unmap(vertexBuffer, 0);
	}

	m_SE = new Audio();
	m_SE->Load("asset\\audio\\load.wav");
}

void LoadText::Update()
{
	m_Frame++;

	if (m_Frame == ANIM_START) SetAnimation(0, DownBlend);

	for (int i = 0; i < TEXT_NUM; i++)
	{
		switch (m_Model[i].animType)
		{
		case DownBlend:
		{
			if (m_Model[i].isAnim)
			{
				m_Model[i].velocity.y += BUOYANCY;
				m_Model[i].position += m_Model[i].velocity;

				if (m_Model[i].position.y > 0.0f)
				{
					m_Model[i].position.y = 0.0f;
					m_Model[i].isAnim = false;
					m_Model[i].isBlend = true;

					if (i + 1 < TEXT_NUM)SetAnimation(i + 1, DownBlend);
					else SetAnimation(0, UpReset);
				}
			}

			if (m_Model[i].isBlend)
			{
				m_Model[i].blendFrame++;

				float blend = m_Model[i].blendFrame / BLEND_MAX_FRAME;

				//モーフィング
				ID3D11Buffer* vertexBuffer = m_Model[i].modelRenderer->GetVertexBuffer();

				D3D11_MAPPED_SUBRESOURCE msr;
				Renderer::GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

				VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

				for (int v = 0; v < (int)m_Model[i].modelObj[0].VertexNum; v++)
				{
					vertex[v] = m_Model[i].modelObj[0].VertexArray[v];

					vertex[v].Position.x = (m_Model[i].modelObj[0].VertexArray[v].Position.x * (1.0f - blend)) + (m_Model[i].modelObj[1].VertexArray[v].Position.x * blend);
					vertex[v].Position.y = (m_Model[i].modelObj[0].VertexArray[v].Position.y * (1.0f - blend)) + (m_Model[i].modelObj[1].VertexArray[v].Position.y * blend);
					vertex[v].Position.z = (m_Model[i].modelObj[0].VertexArray[v].Position.z * (1.0f - blend)) + (m_Model[i].modelObj[1].VertexArray[v].Position.z * blend);
				}

				Renderer::GetDeviceContext()->Unmap(vertexBuffer, 0);

				if (m_Model[i].blendFrame == BLEND_MAX_FRAME)
				{
					m_Model[i].blendFrame = 0;
					m_Model[i].isBlend = false;
					m_SE->Play(false, SE_VOLUME);
				}
			}
		}
			break;
		case UpReset:
		{
			if (m_Model[i].isAnim)
			{
				m_Model[i].velocity.y += BUOYANCY;
				m_Model[i].position += m_Model[i].velocity;

				if (m_Model[i].position.y > 0.0f)
				{
					m_Model[i].position.y = 0.0f;
					m_Model[i].isAnim = false;
					m_Model[i].isBlend = true;

					if (i + 1 < TEXT_NUM)SetAnimation(i + 1, UpReset);
					else SetAnimation(0, DownBlend);
				}
			}

			if (m_Model[i].isBlend)
			{
				m_Model[i].blendFrame++;

				float blend = m_Model[i].blendFrame / BLEND_MAX_FRAME;

				//モーフィング
				ID3D11Buffer* vertexBuffer = m_Model[i].modelRenderer->GetVertexBuffer();

				D3D11_MAPPED_SUBRESOURCE msr;
				Renderer::GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

				VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

				for (int v = 0; v < (int)m_Model[i].modelObj[0].VertexNum; v++)
				{
					vertex[v] = m_Model[i].modelObj[0].VertexArray[v];

					vertex[v].Position.x = (m_Model[i].modelObj[1].VertexArray[v].Position.x * (1.0f - blend)) + (m_Model[i].modelObj[0].VertexArray[v].Position.x * blend);
					vertex[v].Position.y = (m_Model[i].modelObj[1].VertexArray[v].Position.y * (1.0f - blend)) + (m_Model[i].modelObj[0].VertexArray[v].Position.y * blend);
					vertex[v].Position.z = (m_Model[i].modelObj[1].VertexArray[v].Position.z * (1.0f - blend)) + (m_Model[i].modelObj[0].VertexArray[v].Position.z * blend);
				}

				Renderer::GetDeviceContext()->Unmap(vertexBuffer, 0);

				if (m_Model[i].blendFrame == BLEND_MAX_FRAME)
				{
					m_Model[i].blendFrame = 0;
					m_Model[i].isBlend = false;
					m_SE->Play(false, SE_VOLUME);
				}
			}
		}
			break;
		default:
			break;
		}

		
	}
}

void LoadText::Draw()
{
	for (int i = 0; i < TEXT_NUM; i++)
	{
		//入力レイアウト設定
		Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

		//シェーダ設定
		Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
		Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

		//マトリクス設定
		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y - XM_PIDIV2, m_Rotation.z);
		trans = XMMatrixTranslation(m_Position.x + +m_Model[i].position.x, m_Position.y + m_Model[i].position.y, m_Position.z + m_Model[i].position.z);
		world = scale * rot * trans;

		Renderer::SetWorldMatrix(world);

		m_Model[i].modelRenderer->Draw();
	}
}

void LoadText::Uninit()
{
	for (int i = 0; i < TEXT_NUM; i++)
	{
		for (int t = 0; t < MORPHING_NUM; t++)
		{
			delete m_Model[i].modelObj[t].IndexArray;
			delete m_Model[i].modelObj[t].VertexArray;
			delete m_Model[i].modelObj[t].SubsetArray;
		}

		delete m_Model[i].modelRenderer;
	}


	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
}

void LoadText::SetAnimation(int i,AnimationType type)
{
	m_Model[i].isAnim = !m_Model[i].isAnim;
	m_Model[i].velocity.y = FIRST_GRAVITY;
	m_Model[i].animType = type;
}
