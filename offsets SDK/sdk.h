#pragma once
#include <d3d9.h>
#include <vector>
#include <numbers>
#include "offsets.h"
#include "../kernel/coms.h"
#include "../main/main.h"
#include <mutex>
#include <shared_mutex>
#define M_PI 3.14159265358979323846264338327950288419716939937510

inline int width = GetSystemMetrics(SM_CXSCREEN);
inline int height = GetSystemMetrics(SM_CYSCREEN);
inline int screen_center_x = width / 2;
inline int screen_center_y = height / 2;

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(double _x, double _y) : x(_x), y(_y) {}
	~Vector2() {}
	double x, y;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	~Vector3() {}

	double x, y, z;

	inline double dot(Vector3 v) { return x * v.x + y * v.y + z * v.z; }

	inline double distance(Vector3 v) {
		return sqrt((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z));
	}

	Vector3 operator-(Vector3 v) {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	float Length() const {
		return sqrtf(static_cast<float>(x * x + y * y + z * z));
	}
};


struct FQuat { double x, y, z, w; };
struct FTransform
{
	FQuat rotation;
	Vector3 translation;
	uint8_t pad1c[0x8];
	Vector3 scale3d;
	uint8_t pad2c[0x8];

	D3DMATRIX to_matrix_with_scale()
	{
		D3DMATRIX m{};

		const Vector3 Scale
		(
			(scale3d.x == 0.0) ? 1.0 : scale3d.x,
			(scale3d.y == 0.0) ? 1.0 : scale3d.y,
			(scale3d.z == 0.0) ? 1.0 : scale3d.z
		);

		const double x2 = rotation.x + rotation.x;
		const double y2 = rotation.y + rotation.y;
		const double z2 = rotation.z + rotation.z;
		const double xx2 = rotation.x * x2;
		const double yy2 = rotation.y * y2;
		const double zz2 = rotation.z * z2;
		const double yz2 = rotation.y * z2;
		const double wx2 = rotation.w * x2;
		const double xy2 = rotation.x * y2;
		const double wz2 = rotation.w * z2;
		const double xz2 = rotation.x * z2;
		const double wy2 = rotation.w * y2;

		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;
		m._11 = (1.0f - (yy2 + zz2)) * Scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * Scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * Scale.z;
		m._32 = (yz2 - wx2) * Scale.z;
		m._23 = (yz2 + wx2) * Scale.y;
		m._21 = (xy2 - wz2) * Scale.y;
		m._12 = (xy2 + wz2) * Scale.x;
		m._31 = (xz2 + wy2) * Scale.z;
		m._13 = (xz2 - wy2) * Scale.x;
		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DMATRIX matrix_multiplication(D3DMATRIX pm1, D3DMATRIX pm2)
{
	D3DMATRIX pout{};
	pout._11 = pm1._11 * pm2._11 + pm1._12 * pm2._21 + pm1._13 * pm2._31 + pm1._14 * pm2._41;
	pout._12 = pm1._11 * pm2._12 + pm1._12 * pm2._22 + pm1._13 * pm2._32 + pm1._14 * pm2._42;
	pout._13 = pm1._11 * pm2._13 + pm1._12 * pm2._23 + pm1._13 * pm2._33 + pm1._14 * pm2._43;
	pout._14 = pm1._11 * pm2._14 + pm1._12 * pm2._24 + pm1._13 * pm2._34 + pm1._14 * pm2._44;
	pout._21 = pm1._21 * pm2._11 + pm1._22 * pm2._21 + pm1._23 * pm2._31 + pm1._24 * pm2._41;
	pout._22 = pm1._21 * pm2._12 + pm1._22 * pm2._22 + pm1._23 * pm2._32 + pm1._24 * pm2._42;
	pout._23 = pm1._21 * pm2._13 + pm1._22 * pm2._23 + pm1._23 * pm2._33 + pm1._24 * pm2._43;
	pout._24 = pm1._21 * pm2._14 + pm1._22 * pm2._24 + pm1._23 * pm2._34 + pm1._24 * pm2._44;
	pout._31 = pm1._31 * pm2._11 + pm1._32 * pm2._21 + pm1._33 * pm2._31 + pm1._34 * pm2._41;
	pout._32 = pm1._31 * pm2._12 + pm1._32 * pm2._22 + pm1._33 * pm2._32 + pm1._34 * pm2._42;
	pout._33 = pm1._31 * pm2._13 + pm1._32 * pm2._23 + pm1._33 * pm2._33 + pm1._34 * pm2._43;
	pout._34 = pm1._31 * pm2._14 + pm1._32 * pm2._24 + pm1._33 * pm2._34 + pm1._34 * pm2._44;
	pout._41 = pm1._41 * pm2._11 + pm1._42 * pm2._21 + pm1._43 * pm2._31 + pm1._44 * pm2._41;
	pout._42 = pm1._41 * pm2._12 + pm1._42 * pm2._22 + pm1._43 * pm2._32 + pm1._44 * pm2._42;
	pout._43 = pm1._41 * pm2._13 + pm1._42 * pm2._23 + pm1._43 * pm2._33 + pm1._44 * pm2._43;
	pout._44 = pm1._41 * pm2._14 + pm1._42 * pm2._24 + pm1._43 * pm2._34 + pm1._44 * pm2._44;
	return pout;
}

D3DMATRIX to_matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radpitch = (rot.x * M_PI / 180);
	float radyaw = (rot.y * M_PI / 180);
	float radroll = (rot.z * M_PI / 180);
	float sp = sinf(radpitch);
	float cp = cosf(radpitch);
	float sy = sinf(radyaw);
	float cy = cosf(radyaw);
	float sr = sinf(radroll);
	float cr = cosf(radroll);
	D3DMATRIX matrix{};
	matrix.m[0][0] = cp * cy;
	matrix.m[0][1] = cp * sy;
	matrix.m[0][2] = sp;
	matrix.m[0][3] = 0.f;
	matrix.m[1][0] = sr * sp * cy - cr * sy;
	matrix.m[1][1] = sr * sp * sy + cr * cy;
	matrix.m[1][2] = -sr * cp;
	matrix.m[1][3] = 0.f;
	matrix.m[2][0] = -(cr * sp * cy + sr * sy);
	matrix.m[2][1] = cy * sr - cr * sp * sy;
	matrix.m[2][2] = cr * cp;
	matrix.m[2][3] = 0.f;
	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;
	return matrix;
}

struct Camera
{
	Vector3 location;
	Vector3 rotation;
	float fov;
};

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
};

namespace cache
{
	inline uintptr_t uworld;
	inline uintptr_t game_instance;
	inline uintptr_t local_players;
	inline uintptr_t player_controller;
	inline uintptr_t local_pawn;
	inline uintptr_t root_component;
	inline uintptr_t player_state;
	inline Vector3 relative_location;
	inline Vector3 plocaldistance;
	inline Vector3 localRelativeLocation;
	inline uintptr_t closest_pawn;
	inline int my_team_id;
	inline uintptr_t game_state;
	inline uintptr_t player_array;
	inline int player_count;
	inline float closest_distance;
	inline uintptr_t closest_mesh;
	inline uintptr_t closest_aactor;
	inline uintptr_t current_weapon;
	inline uintptr_t persistent_level;
	inline Camera local_camera;
}
namespace udata {

	struct world
	{
		uintptr_t gworld;
		uintptr_t owning_game_instance;
		uintptr_t local_player;
		uintptr_t player_controller;
		uintptr_t player_state;
		uintptr_t local_pawn;
		uintptr_t root_component;
		uintptr_t Spectator;

		int team_id;

		uintptr_t game_state;
		uintptr_t mesh;
		uintptr_t closeest_actor;
		std::vector<uintptr_t> player_array;
	};

	struct actor
	{
		uintptr_t player_state;
		uintptr_t pawn_private;
		uintptr_t mesh;

		int team_id;
		int kill_score;

		uintptr_t character_movement;
		uintptr_t current_weapon;

		bool knocked;
		bool visibility;

		actor(uintptr_t ps, uintptr_t pawn, uintptr_t m, int tid)
			: player_state(ps), pawn_private(pawn), mesh(m), team_id(tid),
			kill_score(0), character_movement(0), current_weapon(0),
			knocked(false), visibility(false) {
		}
	};

	struct environment
	{
		uintptr_t actor;
		std::string name;
		bool is_pickup;
		float distance;
	};

	inline std::vector<environment> environment_list;
	inline std::vector<environment> temp_environment_list;

	inline world world_t;
	inline std::vector<actor> actor_t;
} 
using item = udata::environment;
struct TArray
{
	std::uintptr_t Array;
	std::uint32_t Count;
	std::uint32_t MaxCount;

	std::uintptr_t Get(std::uint32_t Index)
	{
		return read<std::uintptr_t>(Array + (Index * 0x8));
	}
};
Camera get_view_point()
{
	Camera view_point{};
	uintptr_t location_pointer = read<uintptr_t>(cache::uworld + 0x178);
	uintptr_t rotation_pointer = read<uintptr_t>(cache::uworld + 0x188);
	FNRot fnrot{};
	fnrot.a = read<double>(rotation_pointer);
	fnrot.b = read<double>(rotation_pointer + 0x20);
	fnrot.c = read<double>(rotation_pointer + 0x1D0);
	view_point.location = read<Vector3>(location_pointer);
	view_point.rotation.x = asin(fnrot.c) * (180.0 / M_PI);
	view_point.rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
	view_point.fov = read<float>(cache::player_controller + 0x3AC) * 90.0f;
	return view_point;
}
Vector2 project_world_to_screen(Vector3 world_location)
{
	cache::local_camera = get_view_point();
	D3DMATRIX temp_matrix = to_matrix(cache::local_camera.rotation);
	Vector3 vaxisx = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	Vector3 vaxisy = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	Vector3 vaxisz = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
	Vector3 vdelta = world_location - cache::local_camera.location;
	Vector3 vtransformed = Vector3(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));
	if (vtransformed.z < 1) vtransformed.z = 1;
	return Vector2(screen_center_x + vtransformed.x * ((screen_center_x / tanf(cache::local_camera.fov * M_PI / 360))) / vtransformed.z, screen_center_y - vtransformed.y * ((screen_center_x / tanf(cache::local_camera.fov * M_PI / 360))) / vtransformed.z);
}
Vector3 get_entity_bone(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = read<uintptr_t>(mesh + BONE_ARRAY);
	if (bone_array == 0) bone_array = read<uintptr_t>(mesh + BONE_ARRAY + 0x10); // 0x10
	FTransform bone = read<FTransform>(bone_array + (bone_id * 0x60));
	FTransform component_to_world = read<FTransform>(mesh + COMPONENT_TO_WORLD);
	D3DMATRIX matrix = matrix_multiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}
bool is_visible(uintptr_t mesh)
{
	auto Seconds = read<double>(cache::uworld + 0x190);
	auto LastRenderTime = read<float>(mesh + 0x32c);
	return Seconds - LastRenderTime <= 0.06f;
}
static std::string GetPlayerName(uintptr_t playerState) {
	auto Name = read<uintptr_t>(playerState + PLAYERNAME);
	auto length = read<int>(Name + 0x10);
	auto v6 = (__int64)length;
	if (length <= 0 || length > 255) return std::string("[BOT]");
	auto FText = (uintptr_t)read<__int64>(Name + 0x8);
	wchar_t* Buffer = new wchar_t[length];
	kernel->ReadPhysical(PVOID(static_cast<ULONGLONG>(FText)), Buffer, length * sizeof(wchar_t));
	char v21;
	int v22;
	int i;
	int v25;
	UINT16* v23;
	v21 = v6 - 1;
	if (!(UINT32)v6)
		v21 = 0;
	v22 = 0;
	v23 = (UINT16*)Buffer;
	for (i = (v21) & 3; ; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(UINT32)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}
	std::wstring PlayerName{ Buffer };
	delete[] Buffer;
	return std::string(PlayerName.begin(), PlayerName.end());
}

static int32_t GNames = 0x1885D640;
static int32_t NamePrivate = 0x8;
class FName
{
public:
	int32_t ComparisonIndex;

	FName()
	{
		ComparisonIndex = 0;
	}

	FName(uintptr_t object)
	{
		if (object)
		{
			ComparisonIndex = read<int32_t>(object + NamePrivate);
		}
	}

	std::string ToString() const
	{
		return ToString(ComparisonIndex);
	}

	static std::string ToString(uintptr_t object)
	{
		return FName(object).ToString();
	}

	static std::string ToString(int32_t index)
	{
		int32_t DecryptedIndex = DecryptIndex(index);
		uint64_t NamePoolChunk = read<uint64_t>(kernel->ProcessBase + (GNames + 8 * (DecryptedIndex >> 16) + 16)) + 2 * (uint16_t)DecryptedIndex;
		uint16_t Pool = read<uint16_t>(NamePoolChunk);

		if (((Pool ^ 0x80) & 0x7FE0) == 0)
		{
			DecryptedIndex = DecryptIndex(read<int32_t>(NamePoolChunk + 2));
			NamePoolChunk = read<uint64_t>(kernel->ProcessBase +( GNames + 8 * (DecryptedIndex >> 16) + 16)) + 2 * (uint16_t)DecryptedIndex;
			Pool = read<uint16_t>(NamePoolChunk);
		}

		int32_t Length = ((Pool ^ 0x80u) >> 5) & 0x3FF;

		std::vector<char> NameBuffer(Length + 1);
		kernel->ReadPhysical(PVOID(NamePoolChunk + 2), NameBuffer.data(), Length);
		DecryptFName(NameBuffer.data(), Length);
		return std::string(NameBuffer.data());
	}

	static int32_t DecryptIndex(int32_t index)
	{
		if (index)
		{
			int32_t DecryptedIndex = _rotr(index - 1, 1) - 0x527D388B;
			return DecryptedIndex ? DecryptedIndex : 0xAD82C774;
		}

		return 0;
	}

	static void DecryptFName(char* buffer, int32_t length)
	{
		if (length)
		{
			std::vector<uint8_t> EncryptedBuffer(buffer, buffer + length);

			int32_t v8 = 8962 * length + 21856876;
			for (int32_t i = 0; i < length; i++)
			{
				v8 = 8962 * v8 + 21856876;
				buffer[length - 1 - i] = char(v8 + _rotr8(EncryptedBuffer[i], 1) + 6);
			}
		}

		buffer[length] = '\0';
	}
};
template<typename T>
class tarray {
public:
	uintptr_t base;
	int32_t count;

	tarray() : base(0), count(0) {}
	tarray(uintptr_t address) {
		base = address;
		count = read<int32_t>(address + sizeof(void*)); 
	}

	int32_t size() const {
		return count;
	}

	T operator[](int i) const {
		return read<T>(read<uintptr_t>(base) + i * sizeof(T));
	}

	std::vector<T> get_all() const {
		std::vector<T> out;
		if (!base || count <= 0 || count > 500) return out; 
		uintptr_t arrayBase = read<uintptr_t>(base);
		for (int i = 0; i < count; i++) {
			out.push_back(read<T>(arrayBase + i * sizeof(T)));
		}
		return out;
	}
};


class item1 {
public:
	uintptr_t pawn;
	std::string name, pickup;
	float distance;
	Vector3 pos;

	bool is_valid() const {
		//   return pawn && exc->is_valid(reinterpret_cast<uintptr_t>(pawn));
		return pawn;
	}
};


std::vector<item1> ItemListA;
std::vector<item1> ItemListB;
std::vector<item1>* ReadBuffer1 = &ItemListA;
std::vector<item1>* WriteBuffer1 = &ItemListB;
std::shared_mutex ItemListMutex;

std::thread worldThread;
std::mutex time_mutex;
std::chrono::steady_clock::time_point last_work_time;
std::atomic<bool> should_run = true;


void world_reading_work() {
	while (should_run) {
		WriteBuffer1->clear();

		bool did_work = false;

		if (!cache::uworld || !cache::game_state) {
			did_work = true;

			if (did_work)
			{
				std::lock_guard<std::mutex> lock(time_mutex);
				last_work_time = std::chrono::steady_clock::now();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}


		auto item_levels = read<TArray>(cache::uworld + 0x1e8); // Levels

		if (!item_levels.Count) {
			did_work = true;

			if (did_work)
			{
				std::lock_guard<std::mutex> lock(time_mutex);
				last_work_time = std::chrono::steady_clock::now();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		for (int i = 0; i < item_levels.Count; ++i) {
			auto level = item_levels.Get(i);
			if (!level)
				continue;

			auto actors = read<TArray>(level + 0x208); // AActors

			if (!actors.Count)
				continue;

			for (int j = 0; j < actors.Count; ++j) {
				auto actor = actors.Get(j);
				if (!actor || !Valid(actor))
					continue;

				auto index = read<int>(actor + 0x8);
				if (!index)
					continue;

				std::string name = FName::ToString(index);
				if (name.empty())
					continue;
				//std::cout << "Name -> " << name << std::endl;

				auto root_comp = read<uintptr_t>(actor + ROOT_COMPONENT); 

				auto loc = read<Vector3>(root_comp + RELATIVE_LOCATION); 
				float dist = cache::local_camera.location.distance(loc) / 100.0f;
				if (dist > 150) // add max distance int here if u want
					continue;
				item1 itm;
				itm.pawn = actor;
				itm.name = std::move(name);
				itm.distance = dist;
				itm.pos = loc;

				WriteBuffer1->push_back(itm);
				did_work = true;
			}
		}

		{
			std::unique_lock lock(ItemListMutex);
			std::swap(ReadBuffer1, WriteBuffer1);
		}

		if (did_work) {
			std::lock_guard<std::mutex> lock(time_mutex);
			last_work_time = std::chrono::steady_clock::now();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void watchdog_thread() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(2));

		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point last;

		{
			std::lock_guard<std::mutex> lock(time_mutex);
			last = last_work_time;
		}

		auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last);

		if (duration.count() > 10) {
			std::cout << "[Watchdog] Detected dead thread. Restarting..." << std::endl;
			should_run = false;
			if (worldThread.joinable())
				worldThread.join();
			should_run = true;
			last_work_time = std::chrono::steady_clock::now();
			worldThread = std::thread(world_reading_work);
		}
	}
}
std::vector<item1> get_loot_cache_snapshot() {
	std::shared_lock lock(ItemListMutex);
	return *ReadBuffer1;
}
