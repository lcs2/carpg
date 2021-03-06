#include "Pch.h"
#include "GameCore.h"
#include "LevelArea.h"
#include "World.h"
#include "City.h"
#include "MultiInsideLocation.h"
#include "SingleInsideLocation.h"
#include "Level.h"
#include "Net.h"
#include "Game.h"
#include "GameFile.h"
#include "BitStreamFunc.h"
#include "ParticleSystem.h"

static ObjectPool<LevelAreaContext> LevelAreaContextPool;

LevelArea::~LevelArea()
{
	DeleteElements(units);
	DeleteElements(objects);
	DeleteElements(usables);
	DeleteElements(doors);
	DeleteElements(chests);
	DeleteElements(items);
	DeleteElements(traps);
}

void LevelArea::Save(GameWriter& f)
{
	bool local = (tmp != nullptr);

	f << units.size();
	for(Unit* unit : units)
		unit->Save(f, local);

	f << objects.size();
	for(Object* object : objects)
		object->Save(f);

	f << usables.size();
	for(Usable* usable : usables)
		usable->Save(f, local);

	f << doors.size();
	for(Door* door : doors)
		door->Save(f, local);

	f << chests.size();
	for(Chest* chest : chests)
		chest->Save(f, local);

	f << items.size();
	for(GroundItem* item : items)
		item->Save(f);

	f << traps.size();
	for(Trap* trap : traps)
		trap->Save(f, local);

	f << bloods.size();
	for(Blood& blood : bloods)
		blood.Save(f);

	f << lights.size();
	for(Light& light : lights)
		light.Save(f);

	if(tmp)
		tmp->Save(f);
}

void LevelArea::Load(GameReader& f, bool local, old::LoadCompatibility compatibility)
{
	if(local && !tmp)
		tmp = TmpLevelArea::Get();

	switch(compatibility)
	{
	case old::LoadCompatibility::None:
		{
			units.resize(f.Read<uint>());
			for(Unit*& unit : units)
			{
				unit = new Unit;
				Unit::AddRefid(unit);
				unit->Load(f, local);
			}

			objects.resize(f.Read<uint>());
			for(Object*& object : objects)
			{
				object = new Object;
				object->Load(f);
			}

			usables.resize(f.Read<uint>());
			for(Usable*& usable : usables)
			{
				usable = new Usable;
				Usable::AddRefid(usable);
				usable->Load(f, local);
			}

			doors.resize(f.Read<uint>());
			for(Door*& door : doors)
			{
				door = new Door;
				door->Load(f, local);
			}

			chests.resize(f.Read<uint>());
			for(Chest*& chest : chests)
			{
				chest = new Chest;
				chest->Load(f, local);
			}

			items.resize(f.Read<uint>());
			for(GroundItem*& item : items)
			{
				item = new GroundItem;
				GroundItem::AddRefid(item);
				item->Load(f);
			}

			traps.resize(f.Read<uint>());
			for(Trap*& trap : traps)
			{
				trap = new Trap;
				trap->Load(f, local);
			}

			bloods.resize(f.Read<uint>());
			for(Blood& blood : bloods)
				blood.Load(f);

			lights.resize(f.Read<uint>());
			for(Light& light : lights)
				light.Load(f);
		}
		break;
	case old::LoadCompatibility::InsideBuilding:
		{
			units.resize(f.Read<uint>());
			for(Unit*& unit : units)
			{
				unit = new Unit;
				Unit::AddRefid(unit);
				unit->Load(f, local);
				unit->area_id = area_id;
			}

			doors.resize(f.Read<uint>());
			for(Door*& door : doors)
			{
				door = new Door;
				door->Load(f, local);
			}

			objects.resize(f.Read<uint>());
			for(Object*& object : objects)
			{
				object = new Object;
				object->Load(f);
			}

			items.resize(f.Read<uint>());
			for(GroundItem*& item : items)
			{
				item = new GroundItem;
				GroundItem::AddRefid(item);
				item->Load(f);
			}

			usables.resize(f.Read<uint>());
			for(Usable*& usable : usables)
			{
				usable = new Usable;
				Usable::AddRefid(usable);
				usable->Load(f, local);
			}

			bloods.resize(f.Read<uint>());
			for(Blood& blood : bloods)
				blood.Load(f);

			lights.resize(f.Read<uint>());
			for(Light& light : lights)
				light.Load(f);
		}
		break;
	case old::LoadCompatibility::InsideLocationLevel:
		{
			units.resize(f.Read<uint>());
			for(Unit*& unit : units)
			{
				unit = new Unit;
				Unit::AddRefid(unit);
				unit->Load(f, local);
				unit->area_id = area_id;
			}

			chests.resize(f.Read<uint>());
			for(Chest*& chest : chests)
			{
				chest = new Chest;
				chest->Load(f, local);
			}

			objects.resize(f.Read<uint>());
			for(Object*& object : objects)
			{
				object = new Object;
				object->Load(f);
			}

			doors.resize(f.Read<uint>());
			for(Door*& door : doors)
			{
				door = new Door;
				door->Load(f, local);
			}

			items.resize(f.Read<int>());
			for(GroundItem*& item : items)
			{
				item = new GroundItem;
				GroundItem::AddRefid(item);
				item->Load(f);
			}

			usables.resize(f.Read<uint>());
			for(Usable*& usable : usables)
			{
				usable = new Usable;
				Usable::AddRefid(usable);
				usable->Load(f, local);
			}

			bloods.resize(f.Read<uint>());
			for(Blood& blood : bloods)
				blood.Load(f);

			lights.resize(f.Read<uint>());
			for(Light& light : lights)
				light.Load(f);
		}
		break;
	case old::LoadCompatibility::InsideLocationLevelTraps:
		{
			traps.resize(f.Read<uint>());
			for(Trap*& trap : traps)
			{
				trap = new Trap;
				trap->Load(f, local);
			}
		}
		break;
	case old::LoadCompatibility::OutsideLocation:
		{
			units.resize(f.Read<uint>());
			for(Unit*& unit : units)
			{
				unit = new Unit;
				Unit::AddRefid(unit);
				unit->Load(f, local);
				unit->area_id = area_id;
			}

			objects.resize(f.Read<uint>());
			for(Object*& object : objects)
			{
				object = new Object;
				object->Load(f);
			}

			chests.resize(f.Read<uint>());
			for(Chest*& chest : chests)
			{
				chest = new Chest;
				chest->Load(f, local);
			}

			items.resize(f.Read<uint>());
			for(GroundItem*& item : items)
			{
				item = new GroundItem;
				GroundItem::AddRefid(item);
				item->Load(f);
			}

			usables.resize(f.Read<uint>());
			for(Usable*& usable : usables)
			{
				usable = new Usable;
				Usable::AddRefid(usable);
				usable->Load(f, local);
			}

			bloods.resize(f.Read<uint>());
			for(Blood& blood : bloods)
				blood.Load(f);
		}
		break;
	}

	if(tmp && Any(compatibility, old::LoadCompatibility::None, old::LoadCompatibility::InsideBuilding))
		tmp->Load(f);
}

void LevelArea::Write(BitStreamWriter& f)
{
	// units
	f.Write(units.size());
	for(Unit* unit : units)
		unit->Write(f);
	// objects
	f.Write(objects.size());
	for(Object* object : objects)
		object->Write(f);
	// usable objects
	f.Write(usables.size());
	for(Usable* usable : usables)
		usable->Write(f);
	// doors
	f.Write(doors.size());
	for(Door* door : doors)
		door->Write(f);
	// chests
	f.Write(chests.size());
	for(Chest* chest : chests)
		chest->Write(f);
	// ground items
	f.Write(items.size());
	for(GroundItem* item : items)
		item->Write(f);
	// traps
	f.Write(traps.size());
	for(Trap* trap : traps)
		trap->Write(f);
	// bloods
	f.Write(bloods.size());
	for(Blood& blood : bloods)
		blood.Write(f);
	// lights
	f.Write(lights.size());
	for(Light& light : lights)
		light.Write(f);
}

bool LevelArea::Read(BitStreamReader& f)
{
	if(!tmp)
		tmp = TmpLevelArea::Get();

	// units
	uint count;
	f >> count;
	if(!f.Ensure(Unit::MIN_SIZE * count))
	{
		Error("Broken packet for unit count.");
		return false;
	}
	units.resize(count);
	for(Unit*& unit : units)
	{
		unit = new Unit;
		if(!unit->Read(f))
		{
			Error("Broken packet for units.");
			return false;
		}
		unit->area_id = area_id;
	}

	// objects
	f >> count;
	if(!f.Ensure(count * Object::MIN_SIZE))
	{
		Error("Broken packet for object count.");
		return false;
	}
	objects.resize(count);
	for(Object*& object : objects)
	{
		object = new Object;
		if(!object->Read(f))
		{
			Error("Broken packet for objects.");
			return false;
		}
	}

	// usable objects
	f >> count;
	if(!f.Ensure(Usable::MIN_SIZE * count))
	{
		Error("Broken packet for usable object count.");
		return false;
	}
	usables.resize(count);
	for(Usable*& usable : usables)
	{
		usable = new Usable;
		if(!usable->Read(f))
		{
			Error("Broken packet for usable objects.");
			return false;
		}
	}

	// doors
	f >> count;
	if(!f.Ensure(count * Door::MIN_SIZE))
	{
		Error("Broken packet for door count.");
		return false;
	}
	doors.resize(count);
	for(Door*& door : doors)
	{
		door = new Door;
		if(!door->Read(f))
		{
			Error("Broken packet for doors.");
			return false;
		}
	}

	// chests
	f >> count;
	if(!f.Ensure(count * Chest::MIN_SIZE))
	{
		Error("Read level: Broken chest count.");
		return false;
	}
	chests.resize(count);
	for(Chest*& chest : chests)
	{
		chest = new Chest;
		if(!chest->Read(f))
		{
			Error("Read level: Broken chest.");
			return false;
		}
	}

	// ground items
	f >> count;
	if(!f.Ensure(count * GroundItem::MIN_SIZE))
	{
		Error("Broken packet for ground item count.");
		return false;
	}
	items.resize(count);
	for(GroundItem*& item : items)
	{
		item = new GroundItem;
		if(!item->Read(f))
		{
			Error("Broken packet for ground items.");
			return false;
		}
	}

	// traps
	f >> count;
	if(!f.Ensure(count * Trap::MIN_SIZE))
	{
		Error("Read level: Broken packet for inside location trap count.");
		return false;
	}
	traps.resize(count);
	for(Trap*& trap : traps)
	{
		trap = new Trap;
		if(!trap->Read(f))
		{
			Error("Read level: Broken packet for inside location trap.");
			return false;
		}
	}

	// bloods
	f >> count;
	if(!f.Ensure(count * Blood::MIN_SIZE))
	{
		Error("Broken packet for blood count.");
		return false;
	}
	bloods.resize(count);
	for(Blood& blood : bloods)
		blood.Read(f);
	if(!f)
	{
		Error("Broken packet for bloods.");
		return false;
	}

	// lights
	f >> count;
	if(!f.Ensure(count * Light::MIN_SIZE))
	{
		Error("Broken packet for light count.");
		return false;
	}
	lights.resize(count);
	for(Light& light : lights)
		light.Read(f);
	if(!f)
	{
		Error("Broken packet for lights.");
		return false;
	}

	return true;
}

cstring LevelArea::GetName()
{
	switch(area_type)
	{
	case Type::Inside:
		return "Inside";
	case Type::Outside:
		return "Outside";
	default:
		return Format("Building%d", area_id);
	}
}

void LevelArea::BuildRefidTables()
{
	for(Unit* unit : units)
		Unit::AddRefid(unit);
	for(Usable* usable : usables)
		Usable::AddRefid(usable);
	for(GroundItem* item : items)
		GroundItem::AddRefid(item);
	if(tmp)
	{
		for(ParticleEmitter* pe : tmp->pes)
			ParticleEmitter::AddRefid(pe);
		for(TrailParticleEmitter* tpe : tmp->tpes)
			TrailParticleEmitter::AddRefid(tpe);
	}
}

Unit* LevelArea::FindUnit(UnitData* ud)
{
	assert(ud);

	for(Unit* unit : units)
	{
		if(unit->data == ud)
			return unit;
	}

	return nullptr;
}

Usable* LevelArea::FindUsable(BaseUsable* base)
{
	assert(base);

	for(Usable* use : usables)
	{
		if(use->base == base)
			return use;
	}

	return nullptr;
}

bool LevelArea::RemoveItem(const Item* item)
{
	assert(item);

	// szukaj w zw�okach
	{
		Unit* unit;
		int slot;
		if(FindItemInCorpse(item, &unit, &slot))
		{
			unit->items.erase(unit->items.begin() + slot);
			return true;
		}
	}

	// szukaj na ziemi
	if(RemoveGroundItem(item))
		return true;

	// szukaj w skrzyni
	{
		Chest* chest;
		int slot;
		if(FindItemInChest(item, &chest, &slot))
		{
			chest->items.erase(chest->items.begin() + slot);
			return true;
		}
	}

	return false;
}

//=================================================================================================
bool LevelArea::FindItemInCorpse(const Item* item, Unit** unit, int* slot)
{
	assert(item);

	for(vector<Unit*>::iterator it = units.begin(), end = units.end(); it != end; ++it)
	{
		if(!(*it)->IsAlive())
		{
			int item_slot = (*it)->FindItem(item);
			if(item_slot != Unit::INVALID_IINDEX)
			{
				if(unit)
					*unit = *it;
				if(slot)
					*slot = item_slot;
				return true;
			}
		}
	}

	return false;
}

//=================================================================================================
bool LevelArea::RemoveGroundItem(const Item* item)
{
	assert(item);

	for(vector<GroundItem*>::iterator it = items.begin(), end = items.end(); it != end; ++it)
	{
		if((*it)->item == item)
		{
			delete *it;
			if(it + 1 != end)
				std::iter_swap(it, end - 1);
			items.pop_back();
			return true;
		}
	}

	return false;
}

bool LevelArea::FindItemInChest(const Item* item, Chest** chest, int* slot)
{
	assert(item);

	for(vector<Chest*>::iterator it = chests.begin(), end = chests.end(); it != end; ++it)
	{
		int item_slot = (*it)->FindItem(item);
		if(item_slot != -1)
		{
			if(chest)
				*chest = *it;
			if(slot)
				*slot = item_slot;
			return true;
		}
	}

	return false;
}

Object* LevelArea::FindObject(BaseObject* base_obj)
{
	assert(base_obj);

	for(Object* obj : objects)
	{
		if(obj->base == base_obj)
			return obj;
	}

	return nullptr;
}

Chest* LevelArea::FindChestInRoom(const Room& p)
{
	for(Chest* chest : chests)
	{
		if(p.IsInside(chest->pos))
			return chest;
	}

	return nullptr;
}

Chest* LevelArea::GetRandomFarChest(const Int2& pt)
{
	vector<pair<Chest*, float>> far_chests;
	float close_dist = -1.f;
	Vec3 pos = PtToPos(pt);

	// znajd� 5 najdalszych skrzyni
	for(vector<Chest*>::iterator it = chests.begin(), end = chests.end(); it != end; ++it)
	{
		float dist = Vec3::Distance2d(pos, (*it)->pos);
		if(dist > close_dist)
		{
			if(far_chests.empty())
				far_chests.push_back(pair<Chest*, float>(*it, dist));
			else
			{
				for(vector<pair<Chest*, float>>::iterator it2 = far_chests.begin(), end2 = far_chests.end(); it2 != end2; ++it2)
				{
					if(dist > it2->second)
					{
						far_chests.insert(it2, pair<Chest*, float>(*it, dist));
						break;
					}
				}
			}
			if(far_chests.size() > 5u)
			{
				far_chests.pop_back();
				close_dist = far_chests.back().second;
			}
		}
	}

	int index;
	if(far_chests.size() != 5u)
	{
		// je�eli skrzyni jest ma�o wybierz najdalsz�
		index = 0;
	}
	else if(chests.size() < 10u)
	{
		// je�eli skrzyni by�o mniej ni� 10 to cz�� mo�e by� za blisko
		index = Rand() % (chests.size() - 5);
	}
	else
		index = Rand() % 5;

	return far_chests[index].first;
}

bool LevelArea::HaveUnit(Unit* unit)
{
	assert(unit);

	for(Unit* u : units)
	{
		if(u == unit)
			return true;
	}

	return false;
}

//=================================================================================================
Chest* LevelArea::FindChestWithItem(const Item* item, int* index)
{
	assert(item);

	for(Chest* chest : chests)
	{
		int idx = chest->FindItem(item);
		if(idx != -1)
		{
			if(index)
				*index = idx;
			return chest;
		}
	}

	return nullptr;
}

//=================================================================================================
Chest* LevelArea::FindChestWithQuestItem(int quest_refid, int* index)
{
	for(Chest* chest : chests)
	{
		int idx = chest->FindQuestItem(quest_refid);
		if(idx != -1)
		{
			if(index)
				*index = idx;
			return chest;
		}
	}

	return nullptr;
}

//=================================================================================================
Door* LevelArea::FindDoor(const Int2& pt)
{
	for(Door* door : doors)
	{
		if(door->pt == pt)
			return door;
	}

	return nullptr;
}


//=================================================================================================
// Get area levels for selected location and level (in multilevel dungeon not generated levels are ignored for -1)
// Level have special meaning here
// >= 0 (dungeon_level, building index)
// -1 whole location
// -2 outside part of city/village
ForLocation::ForLocation(int loc, int level)
{
	ctx = LevelAreaContextPool.Get();
	ctx->entries.clear();

	bool active = (W.GetCurrentLocationIndex() == loc);
	Location* l = W.GetLocation(loc);
	assert(l->last_visit != -1);

	switch(l->type)
	{
	case L_CITY:
		{
			City* city = static_cast<City*>(l);
			if(level == -1)
			{
				ctx->entries.resize(city->inside_buildings.size() + 1);
				LevelAreaContext::Entry& e = ctx->entries[0];
				e.active = active;
				e.area = city;
				e.level = -2;
				e.loc = loc;
				for(int i = 0, len = (int)city->inside_buildings.size(); i < len; ++i)
				{
					LevelAreaContext::Entry& e2 = ctx->entries[i + 1];
					e2.active = active;
					e2.area = city->inside_buildings[i];
					e2.level = i;
					e2.loc = loc;
				}
			}
			else if(level == -2)
			{
				LevelAreaContext::Entry& e = Add1(ctx->entries);
				e.active = active;
				e.area = city;
				e.level = -2;
				e.loc = loc;
			}
			else
			{
				assert(level >= 0 && level < (int)city->inside_buildings.size());
				LevelAreaContext::Entry& e = Add1(ctx->entries);
				e.active = active;
				e.area = city->inside_buildings[level];
				e.level = level;
				e.loc = loc;
			}
		}
		break;
	case L_CAVE:
	case L_DUNGEON:
	case L_CRYPT:
		{
			InsideLocation* inside = static_cast<InsideLocation*>(l);
			if(inside->IsMultilevel())
			{
				MultiInsideLocation* multi = static_cast<MultiInsideLocation*>(inside);
				if(level == -1)
				{
					ctx->entries.resize(multi->generated);
					for(int i = 0; i < multi->generated; ++i)
					{
						LevelAreaContext::Entry& e = ctx->entries[i];
						e.active = (active && L.dungeon_level == i);
						e.area = &multi->levels[i];
						e.level = i;
						e.loc = loc;
					}
				}
				else
				{
					assert(level >= 0 && level < multi->generated);
					LevelAreaContext::Entry& e = Add1(ctx->entries);
					e.active = (active && L.dungeon_level == level);
					e.area = &multi->levels[level];
					e.level = level;
					e.loc = loc;
				}
			}
			else
			{
				assert(level == -1 || level == 0);
				SingleInsideLocation* single = static_cast<SingleInsideLocation*>(inside);
				LevelAreaContext::Entry& e = Add1(ctx->entries);
				e.active = active;
				e.area = single;
				e.level = 0;
				e.loc = loc;
			}
		}
		break;
	case L_FOREST:
	case L_MOONWELL:
	case L_ENCOUNTER:
	case L_CAMP:
		{
			assert(level == -1);
			OutsideLocation* outside = static_cast<OutsideLocation*>(l);
			LevelAreaContext::Entry& e = Add1(ctx->entries);
			e.active = active;
			e.area = outside;
			e.level = -1;
			e.loc = loc;
		}
		break;
	default:
		assert(0);
		break;
	}
}

//=================================================================================================
ForLocation::~ForLocation()
{
	LevelAreaContextPool.Free(ctx);
}

//=================================================================================================
GroundItem* LevelAreaContext::FindQuestGroundItem(int quest_refid, LevelAreaContext::Entry** entry, int* item_index)
{
	for(LevelAreaContext::Entry& e : entries)
	{
		for(int i = 0, len = (int)e.area->items.size(); i < len; ++i)
		{
			GroundItem* it = e.area->items[i];
			if(it->item->IsQuest(quest_refid))
			{
				if(entry)
					*entry = &e;
				if(item_index)
					*item_index = i;
				return it;
			}
		}
	}

	return nullptr;
}

//=================================================================================================
// search only alive enemies for now
Unit* LevelAreaContext::FindUnitWithQuestItem(int quest_refid, LevelAreaContext::Entry** entry, int* unit_index, int* item_iindex)
{
	Game& game = Game::Get();
	for(LevelAreaContext::Entry& e : entries)
	{
		for(int i = 0, len = (int)e.area->units.size(); i < len; ++i)
		{
			Unit* unit = e.area->units[i];
			if(unit->IsAlive() && unit->IsEnemy(*game.pc->unit))
			{
				int iindex = unit->FindQuestItem(quest_refid);
				if(iindex != Unit::INVALID_IINDEX)
				{
					if(entry)
						*entry = &e;
					if(unit_index)
						*unit_index = i;
					if(item_iindex)
						*item_iindex = iindex;
					return unit;
				}
			}
		}
	}

	return nullptr;
}

//=================================================================================================
bool LevelAreaContext::FindUnit(Unit* unit, LevelAreaContext::Entry** entry, int* unit_index)
{
	assert(unit);

	for(LevelAreaContext::Entry& e : entries)
	{
		for(int i = 0, len = (int)e.area->units.size(); i < len; ++i)
		{
			Unit* unit2 = e.area->units[i];
			if(unit == unit2)
			{
				if(entry)
					*entry = &e;
				if(unit_index)
					*unit_index = i;
				return true;
			}
		}
	}

	return false;
}

//=================================================================================================
Unit* LevelAreaContext::FindUnit(UnitData* data, LevelAreaContext::Entry** entry, int* unit_index)
{
	assert(data);

	for(LevelAreaContext::Entry& e : entries)
	{
		for(int i = 0, len = (int)e.area->units.size(); i < len; ++i)
		{
			Unit* unit = e.area->units[i];
			if(unit->data == data)
			{
				if(entry)
					*entry = &e;
				if(unit_index)
					*unit_index = i;
				return unit;
			}
		}
	}

	return nullptr;
}

//=================================================================================================
Unit* LevelAreaContext::FindUnit(delegate<bool(Unit*)> clbk, LevelAreaContext::Entry** entry, int* unit_index)
{
	for(LevelAreaContext::Entry& e : entries)
	{
		for(int i = 0, len = (int)e.area->units.size(); i < len; ++i)
		{
			Unit* unit2 = e.area->units[i];
			if(clbk(unit2))
			{
				if(entry)
					*entry = &e;
				if(unit_index)
					*unit_index = i;
				return unit2;
			}
		}
	}

	return nullptr;
}

//=================================================================================================
bool LevelAreaContext::RemoveQuestGroundItem(int quest_refid)
{
	LevelAreaContext::Entry* entry;
	int index;
	GroundItem* item = FindQuestGroundItem(quest_refid, &entry, &index);
	if(item)
	{
		if(entry->active && Net::IsOnline())
		{
			NetChange& c = Add1(Net::changes);
			c.type = NetChange::REMOVE_ITEM;
			c.id = item->netid;
		}
		RemoveElementIndex(entry->area->items, index);
		return true;
	}
	else
		return false;
}

//=================================================================================================
// search only alive enemies for now
bool LevelAreaContext::RemoveQuestItemFromUnit(int quest_refid)
{
	LevelAreaContext::Entry* entry;
	int item_iindex;
	Unit* unit = FindUnitWithQuestItem(quest_refid, &entry, nullptr, &item_iindex);
	if(unit)
	{
		unit->RemoveItem(item_iindex, entry->active);
		return true;
	}
	else
		return false;
}

//=================================================================================================
// only remove alive units for now
bool LevelAreaContext::RemoveUnit(Unit* unit)
{
	assert(unit);

	LevelAreaContext::Entry* entry;
	int unit_index;
	if(FindUnit(unit, &entry, &unit_index))
	{
		if(entry->active)
		{
			unit->to_remove = true;
			L.to_remove.push_back(unit);
		}
		else
			RemoveElementIndex(entry->area->units, unit_index);
		return true;
	}
	else
		return false;
}


void TmpLevelArea::Clear()
{
	DeleteElements(explos);
	DeleteElements(electros);
	drains.clear();
	bullets.clear();
	colliders.clear();
	DeleteElements(pes);
	DeleteElements(tpes);
}

void TmpLevelArea::Save(GameWriter& f)
{
	f << pes.size();
	for(ParticleEmitter* pe : pes)
		pe->Save(f);

	f << tpes.size();
	for(TrailParticleEmitter* tpe : tpes)
		tpe->Save(f);

	f << explos.size();
	for(Explo* explo : explos)
		explo->Save(f);

	f << electros.size();
	for(Electro* electro : electros)
		electro->Save(f);

	f << drains.size();
	for(Drain& drain : drains)
		drain.Save(f);

	f << bullets.size();
	for(Bullet& bullet : bullets)
		bullet.Save(f);
}

void TmpLevelArea::Load(GameReader& f)
{
	pes.resize(f.Read<uint>());
	for(ParticleEmitter*& pe : pes)
	{
		pe = new ParticleEmitter;
		ParticleEmitter::AddRefid(pe);
		pe->Load(f);
	}

	tpes.resize(f.Read<uint>());
	for(TrailParticleEmitter*& tpe : tpes)
	{
		tpe = new TrailParticleEmitter;
		TrailParticleEmitter::AddRefid(tpe);
		tpe->Load(f);
	}

	explos.resize(f.Read<uint>());
	for(Explo*& explo : explos)
	{
		explo = new Explo;
		explo->Load(f);
	}

	electros.resize(f.Read<uint>());
	for(Electro*& electro : electros)
	{
		electro = new Electro;
		electro->Load(f);
	}

	drains.resize(f.Read<uint>());
	for(Drain& drain : drains)
		drain.Load(f);

	bullets.resize(f.Read<uint>());
	for(Bullet& bullet : bullets)
		bullet.Load(f);
}
