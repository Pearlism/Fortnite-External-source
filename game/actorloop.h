#pragma once
#include "../offsets SDK/sdk.h"
#include "../settings/settings.h"
#include "drawing.h"
std::string getRank(int tier) {
	switch (tier) {
	case 0:  return "Unranked";
	case 1:  return "Bronze 2";
	case 2:  return "Bronze 3";
	case 3:  return "Silver 1";
	case 4:  return "Silver 2";
	case 5:  return "Silver 3";
	case 6:  return "Gold 1";
	case 7:  return "Gold 2";
	case 8:  return "Gold 3";
	case 9:  return "Platinum 1";
	case 10: return "Platinum 2";
	case 11: return "Platinum 3";
	case 12: return "Diamond 1";
	case 13: return "Diamond 2";
	case 14: return "Diamond 3";
	case 15: return "Elite";
	case 16: return "Champion";
	case 17: return "Unreal";
	default: return "Unranked";
	}
}
ImVec4 getRankColor(int tier) {
	switch (tier) {
	case 0:  return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);
	case 3:  return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	case 2:  return ImVec4(0.8f, 0.5f, 0.2f, 1.0f);
	case 1:  return ImVec4(0.8f, 0.5f, 0.2f, 1.0f);
	case 5:  return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	case 6:  return ImVec4(1.0f, 0.843f, 0.0f, 1.0f);
	case 4:  return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	case 8:  return ImVec4(1.0f, 0.843f, 0.0f, 1.0f);
	case 7:  return ImVec4(1.0f, 0.843f, 0.0f, 1.0f);
	case 9:  return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
	case 10: return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
	case 11: return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
	case 14: return ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
	case 12: return ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
	case 15: return ImVec4(0.9f, 0.85f, 0.6f, 1.0f);
	case 13: return ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
	case 16: return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	case 17: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);
	default: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);
	}
}

void world_esp()
{
	auto entities = get_loot_cache_snapshot();
	for (const auto& entity : entities)
	{
		if (!entity.is_valid())
			continue;




		if (entity.distance >150) // add max distance here
			continue;


		Vector2 screen = project_world_to_screen(entity.pos);
		//if (!Custom::is_within_screen_bounds(screen)) // add in screen check here
			//continue;

		if (screen.x == 0 && screen.y == 0)
			continue;

		float dist_m = entity.distance;

		if (entity.name.find("FortPickupAthena") != std::string::npos || entity.name.find("Fort_Pickup_Creative_C") != std::string::npos || entity.name.find("FortPickup") != std::string::npos)
		{
			std::string name = "";
			auto PrimaryPickupItemEntry = read<uintptr_t>(entity.pawn + 0x370 + 0x10);

			auto item_name = read<uintptr_t>((PrimaryPickupItemEntry) + 0x40);

			if (!Valid(item_name)) name = "Invalid Item";

			auto f_data = read<uintptr_t>(item_name + 0x20);
			if (!Valid(f_data)) name =  "Invalid Data";

			int f_length = read<int32_t>(item_name + 0x28);
			if (f_length <= 0 || f_length > 100) name =  "Invalid Length";

			if (f_length <= 0 || f_length > 100 || !Valid(f_data))
			{
				name = "Invalid";
			}

			std::unique_ptr<wchar_t[]> buffer(new wchar_t[f_length]);
			kernel->ReadPhysical((PVOID)f_data, (PVOID)buffer.get(), f_length * sizeof(wchar_t));

			std::wstring wstr_buf(buffer.get());
			name =  std::string(wstr_buf.begin(), wstr_buf.end());
			
			ImVec2 text_size = ImGui::CalcTextSize(name.c_str());
			ImVec2 pos_text(screen.x - text_size.x / 2, screen.y - 18);


			ImGui::GetForegroundDrawList()->AddText(
				ImGui::GetFont(),
				13.0f,
				(pos_text),
				ImColor(255, 255, 255),
				name.c_str()
			);
		}
	}
}

struct players {
	uintptr_t player_state;
	uintptr_t pawn_private;
	uintptr_t Mesh;
	uintptr_t actorRootComponent;
	Vector3 location;
};

std::vector<players> EntityListA;
std::vector<players> templist;
std::shared_mutex EntityListMutex;
void caching( )
{
	while (true)
	{
		templist.clear();

		cache::uworld = read<uintptr_t>(kernel->ProcessBase + UWORLD);
		cache::game_instance = read<uintptr_t>(cache::uworld + GAME_INSTANCE);
		cache::local_players = read<uintptr_t>(read<uintptr_t>(cache::game_instance + LOCAL_PLAYERS));
		cache::player_controller = read<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
		cache::local_pawn = read<uintptr_t>(cache::player_controller + LOCAL_PAWN);
		cache::localRelativeLocation = read<Vector3>(cache::root_component + RELATIVE_LOCATION);
		if (cache::local_pawn != 0)
		{
			cache::root_component = read<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
			cache::relative_location = read<Vector3>(cache::root_component + RELATIVE_LOCATION);
			cache::player_state = read<uintptr_t>(cache::local_pawn + PLAYER_STATE);
			cache::my_team_id = read<int>(cache::player_state + TEAM_INDEX);
		}
		cache::game_state = read<uintptr_t>(cache::uworld + GAME_STATE);
		cache::player_array = read<uintptr_t>(cache::game_state + PLAYER_ARRAY);
		cache::player_count = read<int>(cache::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
		cache::closest_distance = FLT_MAX;
		cache::closest_mesh = NULL;
		for (int i = 0; i < cache::player_count; i++)
		{
			uintptr_t player_state = read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
			if (!player_state) continue;
			//int player_team_id = read<int>(player_state + TEAM_INDEX); //self esp
			//if (player_team_id == cache::my_team_id) continue; //self esp
			uintptr_t pawn_private = read<uintptr_t>(player_state + PAWN_PRIVATE);
			//if (!pawn_private || pawn_private == cache::local_pawn) continue; //self esp
			if (!pawn_private) continue;
			if (pawn_private == cache::local_pawn && !settings::visuals::selfesp) continue;
			uintptr_t mesh = read<uintptr_t>(pawn_private + MESH);
			if (!mesh) continue;
			/*Vector3 head3d = get_entity_bone(mesh, 110);
			Vector2 head2d = project_world_to_screen(Vector3(head3d.x, head3d.y, head3d.z + 15));
			Vector3 bottom3d = get_entity_bone(mesh, 0);
			Vector2 bottom2d = project_world_to_screen(bottom3d);*/
			/*float box_height = abs(head2d.y - bottom2d.y);
			float box_width = box_height * 0.30f;*/
			uintptr_t actorRootComponent = read<uintptr_t>(pawn_private + ROOT_COMPONENT);
			Vector3 actorRelativeLocation = read<Vector3>(actorRootComponent + RELATIVE_LOCATION);

			players entity{};

			entity.actorRootComponent = actorRootComponent;
			entity.Mesh = mesh;
			entity.pawn_private = pawn_private;
			entity.location = actorRelativeLocation;
			entity.player_state = player_state;
			templist.push_back(entity);
			//float distance = cache::localRelativeLocation.distance(actorRelativeLocation) / 100.0f;
		}
		EntityListA.swap(templist);
		/*{
			std::unique_lock lock(EntityListMutex);
			std::swap(EntityListA, templist);
		}*/
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}
void actor()
{
	for (auto player : EntityListA)
	{
		auto mesh = player.Mesh;

		Vector3 head3d = get_entity_bone(mesh, 110);
		Vector2 head2d = project_world_to_screen(Vector3(head3d.x, head3d.y, head3d.z + 15));
		Vector3 bottom3d = get_entity_bone(mesh, 0);
		Vector2 bottom2d = project_world_to_screen(bottom3d);
		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.30f;
		uintptr_t actorRootComponent = player.actorRootComponent;
		Vector3 actorRelativeLocation = player.location;
		float distance = cache::localRelativeLocation.distance(actorRelativeLocation) / 100.0f;
		double dx = head2d.x - screen_center_x;
		double dy = head2d.y - screen_center_y;
		auto draw_list = ImGui::GetForegroundDrawList();
		if (settings::visuals::box)
		{
			auto color = is_visible ? settings::colors::icBoxColorVisible : settings::colors::icBoxColorInvisible;
			if (settings::visuals::boxType == boxType::normal)
				drawbox(head2d.x - box_width / 2, head2d.y, box_width, box_height, color);
			else if (settings::visuals::boxType == boxType::corner)
				drawcornerbox(head2d.x - box_width / 2, head2d.y, box_width, box_height, color);
		}
		if (settings::visuals::name)
		{
			std::string playerUsername = GetPlayerName(player.player_state);
			ImVec2 textSize = ImGui::CalcTextSize(playerUsername.c_str());
			ImVec2 rectPos = ImVec2(head2d.x - textSize.x / 2 - 10.0f, head2d.y - textSize.y - 20.0f);
			ImVec2 rectSize = ImVec2(textSize.x + 2 * 10.0f, textSize.y + 2 * 5.0f);
			ImGui::GetForegroundDrawList()->AddRectFilled(
				rectPos,
				ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y),
				ImColor(10, 10, 10),
				0.0f
			);
			ImGui::GetForegroundDrawList()->AddRectFilled(
				rectPos,
				ImVec2(rectPos.x + rectSize.x, rectPos.y + 2.0f),
				ImColor(0, 128, 255)
			);
			ImGui::GetForegroundDrawList()->AddText(
				ImGui::GetFont(),
				13.0f,
				ImVec2(rectPos.x + (rectSize.x - textSize.x) / 2, rectPos.y + (rectSize.y - textSize.y) / 2),
				ImColor(255, 255, 255),
				playerUsername.c_str()
			);
		}
		if (settings::visuals::rank)
		{
			uintptr_t habaneroComponent = read<uintptr_t>(player.player_state + HABANERO_COMPONENT);
			uint32_t rank = read<uint32_t>(habaneroComponent + RANKED_PROGRESS + 0x10);
			std::string rankName = getRank(rank);
			ImVec2 textSize = ImGui::CalcTextSize(rankName.c_str());
			float offsetY = settings::visuals::distance ? 25.0f : 10.0f;
			ImVec2 rectPos = ImVec2(bottom2d.x - textSize.x / 2 - 10.0f, bottom2d.y + offsetY - textSize.y / 2 - 5.0f);
			ImVec2 rectSize = ImVec2(textSize.x + 2 * 10.0f, textSize.y + 2 * 5.0f);
			ImGui::GetForegroundDrawList()->AddRectFilled(
				rectPos,
				ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y),
				ImColor(10, 10, 10),
				0.0f
			);
			ImGui::GetForegroundDrawList()->AddRectFilled(
				rectPos,
				ImVec2(rectPos.x + rectSize.x, rectPos.y + 2.0f),
				ImColor(0, 128, 255)
			);
			ImGui::GetForegroundDrawList()->AddText(
				ImGui::GetFont(),
				13.0f,
				ImVec2(rectPos.x + (rectSize.x - textSize.x) / 2, rectPos.y + (rectSize.y - textSize.y) / 2),
				ImColor(255, 255, 255),
				rankName.c_str()
			);
		}
		if (settings::visuals::distance)
		{
			char dist[64];
			sprintf_s(dist, "%.fm", distance);
			ImVec2 textSize = ImGui::CalcTextSize(dist);
			ImVec2 rectPos = ImVec2(bottom2d.x - textSize.x / 2 - 6.0f, bottom2d.y + 10 - textSize.y / 2 - 3.0f);
			ImVec2 rectSize = ImVec2(textSize.x + 2 * 6.0f, textSize.y + 2 * 3.0f);
			ImGui::GetForegroundDrawList()->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), ImColor(10, 10, 10), 4.0f);
			ImGui::GetForegroundDrawList()->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + 2), ImColor(0, 128, 255), 4.0f);
			ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 12.0f, ImVec2(rectPos.x + (rectSize.x - textSize.x) / 2, rectPos.y + (rectSize.y - textSize.y) / 2), ImColor(255, 255, 255), dist);
		}
		if (settings::visuals::platform)
		{
			uintptr_t platformPtr = read<uintptr_t>(player.player_state + PLATFORM);
			wchar_t platformChar[64] = { 0 };
			kernel->ReadPhysical(reinterpret_cast<PVOID>(platformPtr), reinterpret_cast<uint8_t*>(platformChar), sizeof(platformChar));
			std::wstring platformWstr(platformChar);
			std::string platformStr(platformWstr.begin(), platformWstr.end());
			ImVec2 textSize = ImGui::CalcTextSize(platformStr.c_str());
			float offsetY = settings::visuals::name ? 25.0f : 10.0f;
			ImVec2 rectPos = ImVec2(bottom2d.x - textSize.x / 2 - 6.0f, bottom2d.y - offsetY - textSize.y / 2 - 3.0f);
			ImVec2 rectSize = ImVec2(textSize.x + 2 * 6.0f, textSize.y + 2 * 3.0f);
			ImGui::GetForegroundDrawList()->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), ImColor(10, 10, 10), 4.0f);
			ImGui::GetForegroundDrawList()->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + 2), ImColor(0, 128, 255), 4.0f);
			ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 12.0f, ImVec2(rectPos.x + (rectSize.x - textSize.x) / 2, rectPos.y + (rectSize.y - textSize.y) / 2), ImColor(255, 255, 255), platformStr.c_str());
		}
		if (settings::visuals::skeleton) {
			std::vector<Vector3> bones = {
				get_entity_bone(mesh, 66),  // neck
				get_entity_bone(mesh, 9),   // left shoulder
				get_entity_bone(mesh, 10),  // left elbow
				get_entity_bone(mesh, 11),  // left hand
				get_entity_bone(mesh, 38),  // right shoulder
				get_entity_bone(mesh, 39),  // right elbow
				get_entity_bone(mesh, 40),  // right hand
				get_entity_bone(mesh, 2),   // pelvis
				get_entity_bone(mesh, 71),  // left hip
				get_entity_bone(mesh, 72),  // left knee
				get_entity_bone(mesh, 78),  // right hip
				get_entity_bone(mesh, 79),  // right knee
				get_entity_bone(mesh, 75),  // left foot
				get_entity_bone(mesh, 82),  // right foot
				get_entity_bone(mesh, 67)   // head
			};
			std::vector<Vector2> screenPositions(bones.size());
			for (size_t i = 0; i < bones.size(); ++i) {
				screenPositions[i] = project_world_to_screen(bones[i]);
			}
			ImU32 color = is_visible(mesh) ? settings::colors::icSkeletonColorVisible : settings::colors::icSkeletonColorInvisible;
			ImU32 outline = IM_COL32(0, 0, 0, 255); //outline col
			auto draw_line = [&](int idx1, int idx2) {
				ImVec2 p1(screenPositions[idx1].x, screenPositions[idx1].y);
				ImVec2 p2(screenPositions[idx2].x, screenPositions[idx2].y);
				ImGui::GetForegroundDrawList()->AddLine(p1, p2, outline, 3.5f);
				ImGui::GetForegroundDrawList()->AddLine(p1, p2, color, 2.0f);
				};
			draw_line(1, 4);   
			draw_line(1, 2);   
			draw_line(2, 3);  
			draw_line(4, 5);   
			draw_line(5, 6);   
			draw_line(0, 7);   
			draw_line(7, 8);   
			draw_line(8, 9);  
			draw_line(9, 12);  
			draw_line(7, 10);  
			draw_line(10, 11); 
			draw_line(11, 13); 
		}
		if (settings::visuals::debug)
		{
			char debugLines[5][128];
			sprintf_s(debugLines[0], "UWorld: 0x%llX", cache::uworld);
			sprintf_s(debugLines[1], "GameInstance: 0x%llX", cache::game_instance);
			sprintf_s(debugLines[2], "GameState: 0x%llX", cache::game_state);
			sprintf_s(debugLines[3], "PlayerArray: 0x%llX", cache::player_array);
			sprintf_s(debugLines[4], "Driver Base: 0x%llX", kernel->ProcessBase);
			ImFont* font = ImGui::GetFont();
			float fontSize = 14.0f;
			float padding = 10.0f;
			float spacing = 2.0f;
			float maxWidth = 0.0f;
			float totalHeight = 0.0f;
			ImVec2 textSizes[5];
			for (int i = 0; i < 5; ++i)
			{
				textSizes[i] = ImGui::CalcTextSize(debugLines[i]);
				if (textSizes[i].x > maxWidth)
					maxWidth = textSizes[i].x;
				totalHeight += textSizes[i].y + spacing;
			}
			ImVec2 rectPos = ImVec2(150, 25);
			ImVec2 rectSize = ImVec2(maxWidth + 2 * padding, totalHeight + 2 * padding);
			ImGui::GetForegroundDrawList()->AddRectFilled(
				rectPos,
				ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y),
				ImColor(10, 10, 10)
			);
			ImGui::GetForegroundDrawList()->AddRectFilled(
				rectPos,
				ImVec2(rectPos.x + rectSize.x, rectPos.y + 2),
				ImColor(0, 128, 255)
			);
			float y = rectPos.y + padding;
			for (int i = 0; i < 5; ++i)
			{
				float x = rectPos.x + (rectSize.x - textSizes[i].x) / 2;
				ImGui::GetForegroundDrawList()->AddText(
					font, fontSize, ImVec2(x, y), ImColor(255, 255, 255), debugLines[i]
				);
				y += textSizes[i].y + spacing;
			}
		}
		//more options
	}
}