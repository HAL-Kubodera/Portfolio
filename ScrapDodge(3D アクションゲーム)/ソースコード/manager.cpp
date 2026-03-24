/*============================================================
	[manager.cpp]
	▼題名 : ゲームループマネージャー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "manager.h"
#include "renderer.h"

#include "scene.h"
#include "game.h"
#include "title.h"
#include "loading.h" 
#include "tutorial.h"
#include "result.h"
#include "audio.h"
#include "fade.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <list>

Scene* Manager::m_Scene = nullptr;
Scene* Manager::m_SceneNext = nullptr;

void Manager::Init()
{
	Renderer::Init();
	Fade::Init();
	Audio::InitMaster();


	m_Scene = new Title();
	m_Scene->Init();
}

void Manager::Uninit()
{
	m_Scene->Uninit();

	Audio::UninitMaster();
	Fade::Uninit();
	Renderer::Uninit();
}

void Manager::Update()
{
	m_Scene->Update();

	Fade::Update();

	KeyUpdate();
}

void Manager::Draw()
{
	Renderer::Begin();

#ifdef _DEBUG
	ImGui::Begin("Debug");

	if (ImGui::TreeNode("SceneChange"))
	{
		if (ImGui::Button("Title")) {
			SetScene<Title>();
		}

		if (ImGui::Button("Game")) {
			SetScene<Game>();
		}

		if (ImGui::Button("Tutorial")) {
			SetScene<Tutorial>();
		}

		if (ImGui::Button("Loading")) {
			SetScene<Loading>();
		}

		if (ImGui::Button("Result")) {
			SetScene<Result>();
		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif _DEBUG

	m_Scene->Draw();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	Renderer::End();

	if (m_SceneNext != nullptr)
	{
		m_Scene->Uninit();
		delete m_Scene;

		m_Scene = m_SceneNext;
		m_Scene->Init();

		m_SceneNext = nullptr;
	}
}
