/*============================================================
	[main.cpp]
	▼題名 : メイン関数
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "manager.h"
#include "keyboard.h"
#include "renderer.h"
#include "mouseManager.h"
#include <windowsx.h>

#include <thread>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

const char* CLASS_NAME = "AppClass";
const char* WINDOW_NAME = "DX11ゲーム";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND g_Window;

HWND GetWindow()
{
	return g_Window;
}

static DWORD g_CountFPS = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


	WNDCLASSEX wcex;
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = 0;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = CLASS_NAME;
		wcex.hIconSm = nullptr;

		RegisterClassEx(&wcex);


		RECT rc = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		g_Window = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	}

	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);


	Manager::Init();

	ShowWindow(g_Window, nCmdShow);
	UpdateWindow(g_Window);

	DWORD dwExecLastTime;
	DWORD dwCurrentTime;
	DWORD dwFPSLastTime = 0;
	DWORD dwFrameCount = 0;
	timeBeginPeriod(1);
	dwExecLastTime = timeGetTime();
	dwCurrentTime = 0;

	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark(); // スタイル設定（自由）

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_Window);
	ImGui_ImplDX11_Init(Renderer::GetDevice(), Renderer::GetDeviceContext());

	MSG msg;
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 1000) // 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime; // FPSを測定した時刻を保存
				dwFrameCount = 0; // カウントをクリア
			}
			if((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;
#ifdef _DEBUG // デバッグ版の時だけFPSを表示する
				std::stringstream caption;
				caption << WINDOW_NAME << " FPS:" << g_CountFPS;
				SetWindowText(g_Window, caption.str().c_str());
#endif

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				{
					ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGui::SetNextWindowPos(viewport->WorkPos);
					ImGui::SetNextWindowSize(viewport->WorkSize);
					ImGui::SetNextWindowViewport(viewport->ID);

					ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoMove |
						ImGuiWindowFlags_NoBringToFrontOnFocus |
						ImGuiWindowFlags_NoNavFocus |
						ImGuiWindowFlags_NoBackground; // 透明にする

					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

					ImGui::Begin("MainDockSpace", nullptr, window_flags);
					ImGui::PopStyleVar(2);

					// ドッキングスペースを確保
					ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
					ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

					ImGui::End();
				}

				Manager::Update();
				Manager::Draw();
				
				ImGuiIO& io = ImGui::GetIO();
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
				}

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);

	UnregisterClass(CLASS_NAME, wcex.hInstance);

	Manager::Uninit();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	CoUninitialize();

	return (int)msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_ACTIVATEAPP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard_ProcessMessage(uMsg, wParam, lParam);
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				DestroyWindow(hWnd);
				break;
			}

			Keyboard_ProcessMessage(uMsg, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			MouseManager::SetMousePosition(x, y);
			break;
		}
		case WM_LBUTTONDOWN:
			MouseManager::SetMouseLeftDown(true);
			break;
		case WM_LBUTTONUP:
			MouseManager::SetMouseLeftDown(false);
			break;
		case WM_RBUTTONDOWN:
			MouseManager::SetMouseRightDown(true);
			break;
		case WM_RBUTTONUP:
			MouseManager::SetMouseRightDown(false);
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//ImGuiContext* GetImGuiContext()
//{
//	return g_Ctx;
//}