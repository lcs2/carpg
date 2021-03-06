#pragma once

#include "Resource.h"

//-----------------------------------------------------------------------------
// Old building enum - required for pre 0.5 compability
namespace old
{
	enum class BUILDING
	{
		B_MERCHANT,
		B_BLACKSMITH,
		B_ALCHEMIST,
		B_TRAINING_GROUNDS,
		B_INN,
		B_CITY_HALL,
		B_VILLAGE_HALL,
		B_BARRACKS,
		B_HOUSE,
		B_HOUSE2,
		B_HOUSE3,
		B_ARENA,
		B_FOOD_SELLER,
		B_COTTAGE,
		B_COTTAGE2,
		B_COTTAGE3,
		B_VILLAGE_INN,
		B_NONE,
		B_VILLAGE_HALL_OLD,
		B_MAX
	};

	Building* Convert(BUILDING building_id);
}

//-----------------------------------------------------------------------------
struct Building
{
	enum TileScheme
	{
		SCHEME_GRASS,
		SCHEME_BUILDING,
		SCHEME_SAND,
		SCHEME_PATH,
		SCHEME_UNIT,
		SCHEME_BUILDING_PART
	};

	enum Flags
	{
		FAVOR_CENTER = 1 << 0,
		FAVOR_ROAD = 1 << 1,
		HAVE_NAME = 1 << 2,
		LIST = 1 << 3
	};

	string id, mesh_id, inside_mesh_id, name;
	Int2 size, shift[4];
	vector<TileScheme> scheme;
	int flags;
	BuildingGroup* group;
	Mesh* mesh, *inside_mesh;
	UnitData* unit;
	ResourceState state;

	Building() : size(0, 0), shift(), flags(0), mesh(nullptr), inside_mesh(nullptr), group(nullptr), unit(nullptr), state(ResourceState::NotLoaded) {}

	static vector<Building*> buildings;
	static Building* TryGet(Cstring id);
	static Building* Get(Cstring id)
	{
		auto building = TryGet(id);
		assert(building);
		return building;
	}
};

//-----------------------------------------------------------------------------
// Building used when constructing map
struct ToBuild
{
	Building* building;
	Int2 pt, unit_pt;
	GameDirection rot;
	bool required;

	ToBuild(Building* building, bool required = true) : building(building), required(required) {}
};
