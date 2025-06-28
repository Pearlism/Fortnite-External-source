#pragma once
inline bool get_menu = true;
inline int tab = 0;


namespace updater {

    void run();
    void env_run();
    void CacheLevels();
} // namespace updater

enum boxType : int
{
    normal = 0,
    corner = 1,
};
inline int boxValues[] = {
    boxType::normal,
    boxType::corner,
};
inline const char* getBoxTypeName(int type)
{
    switch (type)
    {
    case boxType::normal: return "Normal";
    case boxType::corner: return "Corner";
    default: return "Unknown";
    }
}
namespace settings
{
	namespace visuals
	{
		inline bool enable = true;
        inline bool radar = false;
        inline bool ignore_teamates = true;
        inline bool box = true;
        inline bool skeleton = false;
        inline bool line = false;
        inline bool name = false;
        inline bool platform = false;
        inline bool distance = false;
        inline bool rank = false;
        inline float renderDistance = 300.0f;
        inline int boxType = boxType::normal;
        inline bool selfesp = false;
        inline bool debug = false;
	}
    namespace colors
    {
        inline ImColor icCrosshairColor = ImColor(255, 255, 255);
        inline ImColor icFovColor = ImColor(255, 255, 255);
        inline ImColor icBoxColorVisible = ImColor(255, 255, 255);
        inline ImColor icBoxColorInvisible = ImColor(255, 0, 0);
        inline ImColor icSkeletonColorVisible = ImColor(255, 255, 255);
        inline ImColor icSkeletonColorInvisible = ImColor(255, 255, 255);
        inline ImColor icTracerColorVisible = ImColor(255, 255, 255);
        inline ImColor icTracerColorInvisible = ImColor(255, 0, 0);
    }
    namespace world
    {
        inline bool uncommon_loot = false;
        inline bool common_loot = false;
        inline bool llamas = false;
        inline bool rare_loot = false;
        inline bool foodbox = false;
        inline bool epic_loot = false;
        inline bool legendary_loot = false;
        inline bool mythic_loot = false;
        inline bool chests = false;
        inline bool ammobox = false;
        inline bool vehicle = false;
        inline bool spirits = false;
        inline int render_distance_loot = 350;
        inline int render_distance_ammo = 350;
        inline int render_distance_vehicle = 350;
        inline int render_distance_spirits = 350;
        inline int render_distance_llama = 350;
        inline int render_distance_chest = 350;
        inline bool enableOutlinesContainers = false;
        inline bool enableOutlinesLoot = false;
    }
}
