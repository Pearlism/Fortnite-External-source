#pragma once
#include "../imgui/imgui.h"
#define DEBUG_DISPLAY(name, value) \
		if (!(value)) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to read " name "!"); \
		else ImGui::TextColored(ImVec4(0, 1, 0, 1), name ": 0x%llX", value);
void outlinetext(ImFont* font, float fontSize, ImVec2 position, ImColor color, const char* text)
{
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, position, color, text);
}
void drawbox(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x + 1, y + 1), ImVec2(x + w - 1, y + h - 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x + 1, y - 1), ImVec2(x + w - 1, y + h + 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x - 1, y + 1), ImVec2(x + w + 1, y + h - 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0, 1.0f);
}
static void drawcornerbox(float x, float y, float w, float h, ImColor color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y - 1), ImVec2(x - 1, y + (h / 3) - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y - 1), ImVec2(x + (w / 3) - 1, y - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) + 1, y - 1), ImVec2(x + w + 1, y - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w + 1, y - 1), ImVec2(x + w + 1, y + (h / 3) - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y + h - (h / 3) + 1), ImVec2(x - 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y + h + 1), ImVec2(x + (w / 3) - 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) + 1, y + h + 1), ImVec2(x + w + 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w + 1, y + h - (h / 3) + 1), ImVec2(x + w + 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + 1), ImVec2(x + 1, y + (h / 3) + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + 1), ImVec2(x + (w / 3) + 1, y + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) - 1, y + 1), ImVec2(x + w - 1, y + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - 1, y + 1), ImVec2(x + w - 1, y + (h / 3) + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + h - (h / 3) - 1), ImVec2(x + 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + h - 1), ImVec2(x + (w / 3) + 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) - 1, y + h - 1), ImVec2(x + w - 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - 1, y + h - (h / 3) - 1), ImVec2(x + w - 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, 1.0f);
}

namespace updater {

	void run()
	{
		while (true)
		{
			auto uworld = read<uintptr_t>(kernel->ProcessBase + UWORLD);
			auto owning_game_instance = read<uintptr_t>(uworld + GAME_INSTANCE);
			auto local_player = read<uintptr_t>(read<uintptr_t>(owning_game_instance + LOCAL_PLAYERS));
			auto player_controller = read<uintptr_t>(local_player + PLAYER_CONTROLLER);
			auto local_pawn = read<uintptr_t>(player_controller + LOCAL_PAWN);
			auto player_state = read<uintptr_t>(local_pawn + PLAYER_STATE);
			auto root_component = read<uintptr_t>(local_pawn + ROOT_COMPONENT);

			// Store local data
			cache::uworld = uworld;
			cache::local_pawn = local_pawn;
			cache::my_team_id = read<int>(player_state + TEAM_INDEX);

			auto game_state = read<uintptr_t>(uworld + GAME_STATE);
			tarray<uintptr_t> player_array(game_state + PLAYER_ARRAY);
			auto player_list = player_array.get_all();

			udata::world_t.game_state = game_state;
			udata::world_t.player_array = player_list;

			std::vector<udata::actor> actors;
			actors.reserve(player_list.size());
			udata::world_t.mesh = NULL;

			for (const auto& player_state : player_list)
			{
				if (!player_state) continue;

				auto team_id = read<int>(player_state + TEAM_INDEX);
				if (team_id == cache::my_team_id) continue;

				auto pawn_private = read<uintptr_t>(player_state + PAWN_PRIVATE);
				if (!pawn_private || pawn_private == cache::local_pawn) continue;

				auto mesh = read<uintptr_t>(pawn_private + MESH);
				if (!mesh) continue;

				actors.emplace_back(player_state, pawn_private, mesh, team_id);
			}

			udata::actor_t = std::move(actors);

			Sleep(1200);
		}
	}

	uintptr_t RootComponent(uintptr_t actor)
	{
		return read<uintptr_t>(actor + ROOT_COMPONENT);
	}

	Vector3 GetLocation(uintptr_t actor)
	{
		return read<Vector3>(RootComponent(actor) + RELATIVE_LOCATION);
	}

	
}
//void DrawLoot()
//{
//	if (!kernel || !ImGui::GetBackgroundDrawList())
//		return;
//
//	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
//
//	for (const auto& loot : item_pawns)
//	{
//		if (loot.actor == 0 || !loot.is_pickup)
//			continue;
//
//		auto rootComp = read<uintptr_t>(loot.actor + ROOT_COMPONENT);
//		if (rootComp == 0) continue;
//
//		Vector3 worldPos = read<Vector3>(rootComp + RELATIVE_LOCATION);
//		Vector2 screenPos = project_world_to_screen(worldPos);
//		if (screenPos.x <= 0 || screenPos.y <= 0)
//			continue;
//
//		ImColor lootColor = ImColor(255, 255, 0);
//		char buf[128];
//		snprintf(buf, sizeof(buf), "[%.fm] %s", loot.distance, loot.name.c_str());
//
//		drawList->AddText(ImVec2(screenPos.x, screenPos.y), lootColor, buf);
//	}
//}








