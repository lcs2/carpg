// character class
#include "Pch.h"
#include "GameCore.h"
#include "Class.h"
#include "UnitData.h"

//-----------------------------------------------------------------------------
ClassInfo ClassInfo::classes[(int)Class::MAX] = {
	ClassInfo(Class::BARBARIAN, "barbarian", "base_warrior", "icon_barbarian.png", false, nullptr),
	ClassInfo(Class::BARD, "bard", "base_rogue", "icon_bard.png", false, nullptr),
	ClassInfo(Class::CLERIC, "cleric", "base_warrior", "icon_cleric.png", false, nullptr),
	ClassInfo(Class::DRUID, "druid", "base_hunter", "icon_druid.png", false, nullptr),
	ClassInfo(Class::HUNTER, "hunter", "base_hunter", "icon_hunter.png", true, "summon_wolf"),
	ClassInfo(Class::MAGE, "mage", "base_rogue", "icon_mage.png", false, nullptr),
	ClassInfo(Class::MONK, "monk", "base_rogue", "icon_monk.png", false, nullptr),
	ClassInfo(Class::PALADIN, "paladin", "base_warrior", "icon_paladin.png", false, nullptr),
	ClassInfo(Class::ROGUE, "rogue", "base_rogue", "icon_rogue.png", true, "dash"),
	ClassInfo(Class::WARRIOR, "warrior", "base_warrior", "icon_warrior.png", true, "bull_charge")
};

//=================================================================================================
ClassInfo* ClassInfo::Find(const string& id)
{
	for(ClassInfo& c : ClassInfo::classes)
	{
		if(id == c.id)
			return &c;
	}

	return nullptr;
}

//=================================================================================================
void ClassInfo::Validate(uint& err)
{
	for(int i = 0; i < (int)Class::MAX; ++i)
	{
		ClassInfo& ci = ClassInfo::classes[i];
		if(ci.class_id != (Class)i)
		{
			++err;
			Warn("Test: Class %s: id mismatch.", ci.id);
		}
		if(ci.name.empty())
		{
			++err;
			Warn("Test: Class %s: empty name.", ci.id);
		}
		if(ci.desc.empty())
		{
			++err;
			Warn("Test: Class %s: empty desc.", ci.id);
		}
		if(ci.about.empty())
		{
			++err;
			Warn("Test: Class %s: empty about.", ci.id);
		}
		if(!ci.icon)
		{
			++err;
			Warn("Test: Class %s: missing icon file '%s'.", ci.id, ci.icon_file);
		}
		if(IsPickable(ci.class_id))
		{
			if(!ci.unit_data_id)
			{
				++err;
				Warn("Test: Class %s: missing unit data.", ci.id);
			}
			else if(!ci.unit_data)
			{
				++err;
				Warn("Test: Class %s: invalid unit data '%s'.", ci.id, ci.unit_data_id);
			}
			else if(!ci.unit_data->stat_profile)
			{
				++err;
				Warn("Test: Class %s: missing profile.", ci.id);
			}
			else if(ci.unit_data->stat_profile->subprofiles.empty())
			{
				++err;
				Warn("Test: Class %s: missing subprofiles.", ci.id);
			}
			else
			{
				for(StatProfile::Subprofile* sub : ci.unit_data->stat_profile->subprofiles)
				{
					if(sub->perks[StatProfile::MAX_PERKS - 1].perk == Perk::None)
					{
						++err;
						Warn("Test: Subprofile %s.%s: Missing perks.", ci.unit_data->stat_profile->id.c_str(), sub->id.c_str());
					}
					else if(sub->tag_skills[StatProfile::MAX_TAGS - 1] == SkillId::NONE)
					{
						++err;
						Warn("Test: Subprofile %s.%s: Missing tag skills.", ci.unit_data->stat_profile->id.c_str(), sub->id.c_str());
					}
				}
			}
		}
	}
}

//=================================================================================================
UnitData& ClassInfo::GetUnitData(Class clas, bool crazy)
{
	cstring id = nullptr;

	switch(clas)
	{
	default:
		assert(0);
	case Class::WARRIOR:
		id = (crazy ? "crazy_warrior" : "hero_warrior");
		break;
	case Class::HUNTER:
		id = (crazy ? "crazy_hunter" : "hero_hunter");
		break;
	case Class::ROGUE:
		id = (crazy ? "crazy_rogue" : "hero_rogue");
		break;
	case Class::MAGE:
		id = (crazy ? "crazy_mage" : "hero_mage");
		break;
	}

	return *UnitData::Get(id);
}

//=================================================================================================
Class ClassInfo::GetRandom()
{
	switch(Rand() % 7)
	{
	default:
	case 0:
	case 1:
		return Class::WARRIOR;
	case 2:
	case 3:
		return Class::HUNTER;
	case 4:
	case 5:
		return Class::ROGUE;
	case 6:
		return Class::MAGE;
	}
}

//=================================================================================================
Class ClassInfo::GetRandomPlayer()
{
	static vector<Class> classes;
	if(classes.empty())
	{
		for(ClassInfo& ci : ClassInfo::classes)
		{
			if(ci.pickable)
				classes.push_back(ci.class_id);
		}
	}
	return RandomItem(classes);
}
