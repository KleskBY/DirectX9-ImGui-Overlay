#include "main.h"

char WINNAME[16] = " ";
int testint = 1;
float colortest[4] = { 1.f,0.f,1.f,1.f };
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0; // Disable ALT application menu
		break;
	case WM_NCHITTEST:
	{
		ImVec2 Shit = ImGui::GetMousePos();
		if (Shit.y < 25 && Shit.x < 175)
		{
			LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
			if (hit == HTCLIENT) hit = HTCAPTION;
			return hit;
		}
		else break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int main(int, char**)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = WINNAME;
	wc.lpszMenuName = WINNAME;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	RegisterClassEx(&wc);
	g_hwnd = CreateWindowEx(WS_EX_TOPMOST /*| WS_EX_TRANSPARENT*/ | WS_EX_LAYERED, WINNAME, WINNAME, WS_POPUP, 0, 0, Width, Height, 0, 0, 0, 0);
	
	SetLayeredWindowAttributes(g_hwnd, 0, 255, LWA_ALPHA);
	SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	DwmExtendFrameIntoClientArea(g_hwnd, &MARGIN);

	if (CreateDeviceD3D(g_hwnd) < 0)
	{
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(g_hwnd);
	SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFont* DefaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ImGuiStyle& style = ImGui::GetStyle();
	
	ImGui_ImplWin32_Init(g_hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (GetAsyncKeyState(VK_INSERT))
		{
			Beep(300, 150);
			if (!ShowMenu) SetWindowLong(g_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
			else SetWindowLong(g_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST);
			ShowMenu = !ShowMenu;

			while (GetAsyncKeyState(VK_INSERT)) {}
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		else if (GetAsyncKeyState(VK_END)) exit(1);
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		if (!open) ExitProcess(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		ResetDevice();
		g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (ShowMenu)
			{
				ImGui::Begin("Overlay test", &open, ImVec2(200, 200), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				ImGui::Text("TEST");
				ImGui::Button("MYTEST");
				ImGui::InputInt("testInt", &testint);
				ImGui::ColorPicker4("Color test", colortest);
				ImGui::End();
			}

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(g_hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}
