#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <d3d9.h>
#include "../game/actorloop.h"
#include "../settings/settings.h"
#include "../external/gui.h"
#include "../offsets SDK/sdk.h"
#include "../kernel/coms.h"
#include "../Offsets SDK/offsets.h"
#include <chrono>
#include <string>
#include "../freetype/include/freetype/freetype.h"
#pragma comment(lib, "d3d9.lib")
#include <dwmapi.h>
#include <windows.h>
#include <psapi.h>
#include <string>
#include "fonts.hxx"
#include "../imgui/imgui_freetype.h"
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dwmapi.lib")
IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND MyWnd = NULL;
DWORD ProcId;

int getFps()
{
	using namespace std::chrono;
	static int count = 0;
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();
	static int fps = 0;
	count++;
	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}
	return fps;
}
namespace font
{
	inline ImFont* icomoon = nullptr;

	inline ImFont* icomoon_widget = nullptr;
	inline ImFont* icomoon_widget2 = nullptr;


}
HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = MyWnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = width;
	p_params.BackBufferHeight = height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, MyWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(MyWnd);
	ImGui_ImplDX9_Init(p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFontAtlas* fontAtlas = io.Fonts;
	ImFontConfig arialConfig;
	arialConfig.FontDataOwnedByAtlas = false;
	ImFont* arialFont = fontAtlas->AddFontFromFileTTF("C:\\Windows\\Fonts\\Bahnschrift.ttf", 14.0f, &arialConfig);
	io.Fonts = fontAtlas;
	ImFontConfig cfg;
	cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
	
	// Load icon font with default glyph ranges
	font::icomoon = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 20.f, &cfg, io.Fonts->GetGlyphRangesDefault());
	font::icomoon_widget = io.Fonts->AddFontFromMemoryTTF(icomoon_widget, sizeof(icomoon_widget), 15.f, &cfg, io.Fonts->GetGlyphRangesDefault());
	font::icomoon_widget2 = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 16.f, &cfg, io.Fonts->GetGlyphRangesDefault());
	io.IniFilename = 0;
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->WindowRounding = 0.0f;
	style->WindowBorderSize = 4;
	style->FrameBorderSize = 1;
	style->FramePadding = { 0, 0 };
	style->Colors[ImGuiCol_WindowBg] = ImColor(10, 10, 10);
	style->Colors[ImGuiCol_TitleBg] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_Border] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_ChildBg] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_FrameBg] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_PopupBg] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_CheckMark] = ImColor(0, 128, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(0, 128, 255);
	style->Colors[ImGuiCol_Button] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_Separator] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_SeparatorActive] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_SeparatorHovered] = ImColor(25, 25, 25);
	p_object->Release();
	return S_OK;
}
static inline bool find_discord_overlay_window(HWND& hwnd_out) {
	HWND hwnd = nullptr;

	while ((hwnd = FindWindowExA(nullptr, hwnd, "Chrome_WidgetWin_1", nullptr)) != nullptr) {
		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (hProcess) {
			char exePath[MAX_PATH];
			if (GetModuleFileNameExA(hProcess, nullptr, exePath, MAX_PATH)) {
				std::string path = exePath;
				if (path.find("Discord") != std::string::npos) {
					hwnd_out = hwnd;
					CloseHandle(hProcess);
					return true;
				}
			}
			CloseHandle(hProcess);
		}
	}
	return false;
}
void overlay()
{
	HWND hwnd_out = nullptr;
	if (!find_discord_overlay_window(hwnd_out)) {
		MessageBoxA(0, "Couldn't find Discord overlay window.", "Error", MB_ICONERROR);
		return;
	} 
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(hwnd_out, &margin);
	SetWindowLong(hwnd_out, GWL_EXSTYLE, GetWindowLong(hwnd_out, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(hwnd_out, RGB(0, 0, 0), 255, LWA_ALPHA);
	ShowWindow(hwnd_out, SW_SHOW);
	UpdateWindow(hwnd_out);
	MyWnd = hwnd_out;
}
int selected_tab = 0; 
void show_menu()
{
	char watermarkText[64];
	sprintf_s(watermarkText, "Landens Base | %i FPS", getFps());
	ImVec2 wmTextSize = ImGui::CalcTextSize(watermarkText);
	ImVec2 rectSize = ImVec2(wmTextSize.x + 20.0f, wmTextSize.y + 10.0f);
	ImVec2 rectPos = ImVec2(5, 5);

	auto draw = ImGui::GetForegroundDrawList();
	draw->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), ImColor(10, 10, 10));
	draw->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + 2), ImColor(0, 128, 255));
	draw->AddText(ImGui::GetFont(), 14.0f,
		ImVec2(rectPos.x + (rectSize.x - wmTextSize.x) / 2, rectPos.y + (rectSize.y - wmTextSize.y) / 2),
		ImColor(255, 255, 255), watermarkText);

	if (GetAsyncKeyState(VK_INSERT) & 1)
		get_menu = !get_menu;

	if (get_menu)
	{
		ImGui::SetNextWindowSize({ 640, 400 });
		ImGui::Begin("ImGui Menu", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		struct TabData { const char* icon; };
		TabData tabs[] = { { "b" }, { "c" }, { "e" } };
		int tab_count = IM_ARRAYSIZE(tabs);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

		for (int i = 0; i < tab_count; i++)
		{
			if (i > 0) ImGui::SameLine();

			ImGui::PushFont(font::icomoon);
			if (ImGui::Button(tabs[i].icon, ImVec2(40, 30)))
				selected_tab = i;
			ImGui::PopFont();
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (selected_tab == 0)
		{
			ImGui::Checkbox("Enable Visuals", &settings::visuals::enable);
			if (settings::visuals::enable)
			{
				ImGui::Checkbox("Name", &settings::visuals::name);
				ImGui::Checkbox("Self ESP", &settings::visuals::selfesp);
				ImGui::Checkbox("Platform", &settings::visuals::platform);
				ImGui::Checkbox("Distance", &settings::visuals::distance);
				ImGui::Checkbox("Rank", &settings::visuals::rank);
				ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
				ImGui::Checkbox("Box", &settings::visuals::box);

				if (ImGui::BeginCombo("Box Type", getBoxTypeName(settings::visuals::boxType)))
				{
					for (auto boxType : boxValues)
					{
						if (ImGui::Selectable(getBoxTypeName(boxType), settings::visuals::boxType == boxType))
							settings::visuals::boxType = boxType;
					}
					ImGui::EndCombo();
				}
			}
		}
		else if (selected_tab == 1)
		{ 
			//aimbot tab 
		} 
		else if (selected_tab == 2)
		{
			ImGui::Checkbox("Debug", &settings::visuals::debug);
			ImGui::Checkbox("Loot ESP", &settings::visuals::worldesp);
			ImGui::SliderFloat("Render Distance", &settings::visuals::renderDistance, 100.0f, 1000.0f, "%.0fm");
			ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_Float;
			if (ImGui::ColorButton("##color", settings::colors::icBoxColorVisible, ImGuiColorEditFlags_NoTooltip, ImVec2(24, 24)))
			{
				ImGui::OpenPopup("color_picker_popup");
			}
			if (ImGui::BeginPopup("color_picker_popup"))
			{
				ImGui::ColorPicker4("##picker", (float*)&settings::colors::icBoxColorVisible.Value, flags);
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}
}
HWND GetProcWindow(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
	{
		auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
		uint32_t processid = 0;
		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
		{
			SetLastError((uint32_t)-1);
			pparams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);
	if (!bresult && GetLastError() == -1 && params.first) return params.first;
	return 0;
}

WPARAM RenderLoop()
{
	static RECT old_rc;
	ZeroMemory(&messager, sizeof(MSG));
	while (messager.message != WM_QUIT)
	{
		if (PeekMessage(&messager, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}
		if (MyWnd == NULL) exit(0);
		HWND active_wnd = GetForegroundWindow();
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		if (MyWnd != NULL) {
			GetWindowRect(MyWnd, &rc);
			xy.x = rc.left;
			xy.y = rc.top;
		}
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			p_params.BackBufferWidth = width;
			p_params.BackBufferHeight = height;
			SetWindowPos(MyWnd, HWND_TOPMOST, rc.left, rc.top, width, height, SWP_NOREDRAW | SWP_NOACTIVATE | SWP_SHOWWINDOW);
			SetWindowLong(MyWnd, GWL_EXSTYLE, GetWindowLong(MyWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
			SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
			p_device->Reset(&p_params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		actor();
		show_menu();
		ImGui::EndFrame();
		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (p_device != 0)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != 0) p_object->Release();
	DestroyWindow(MyWnd);
	return messager.wParam;
}
