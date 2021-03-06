#pragma once

//-----------------------------------------------------------------------------
#include "Item.h"
#include "UnitData.h"
#include "Class.h"
#include "MeshInstance.h"
#include "HumanData.h"
#include "HeroData.h"
#include "PlayerController.h"
#include "Usable.h"
#include "Effect.h"
#include "Buff.h"
#include "Event.h"

//-----------------------------------------------------------------------------
enum Animation
{
	ANI_NONE = -1,
	ANI_WALK,
	ANI_WALK_BACK,
	ANI_RUN,
	ANI_LEFT,
	ANI_RIGHT,
	ANI_STAND,
	ANI_BATTLE,
	ANI_BATTLE_BOW,
	ANI_DIE,
	ANI_PLAY,
	ANI_KNEELS,
	ANI_IDLE,
	ANI_MAX
};

//-----------------------------------------------------------------------------
enum ACTION
{
	A_NONE,
	A_TAKE_WEAPON, // etapy: 0, 1
	A_SHOOT, // etapy: 0 - naci�ga, 1 - naci�ga ale mo�e strzela�, 2 - po strzale, 3 - wyj�� now� strza��
	A_ATTACK, // etapy: 0 - przygotowanie, 1 - mo�e atakowa�, 2 - po trafieniu
	A_BLOCK,
	A_BASH,
	A_DRINK, // picie napoju (0 - zaczyna pi�, 1-efekt wypicia, 2-schowa� przedmiot)
	A_EAT, // jedzenie (0 - zaczyna je��, 1-odtwarza d�wi�k, 2-efekt, 3-chowa)
	A_PAIN,
	A_CAST,
	A_ANIMATION, // animacja bez obiektu (np drapani si�, rozgl�danie)
	A_ANIMATION2, // u�ywanie obiektu (0-podchodzi, 1-u�ywa, 2-u�ywa d�wi�k, 3-odchodzi)
	A_POSITION, // u�ywa� czego� ale dosta� basha lub umar�, trzeba go przesun�� w normalne miejsce
	A_PICKUP, // p�ki co dzia�a jak animacja, potem doda si� punkt podnoszenia
	A_DASH,
	A_DESPAWN,
	A_PREPARE, // mp client want to use object, waiting for response
	A_STAND_UP,
	A_USE_ITEM,
	A_POSITION_CORPSE, // move corpse that thanks to animation is now not lootable
};

//-----------------------------------------------------------------------------
enum AnimationState
{
	AS_NONE = 0,

	AS_ANIMATION2_MOVE_TO_OBJECT = 0,
	AS_ANIMATION2_USING = 1,
	AS_ANIMATION2_USING_SOUND = 2,
	AS_ANIMATION2_MOVE_TO_ENDPOINT = 3
};

inline bool IsBlocking(ACTION a)
{
	return a == A_ANIMATION || a == A_PICKUP || a == A_DASH || a == A_STAND_UP;
}

//-----------------------------------------------------------------------------
enum WeaponState
{
	WS_HIDDEN,
	WS_HIDING,
	WS_TAKING,
	WS_TAKEN
};

//-----------------------------------------------------------------------------
enum class AutoTalkMode
{
	No,
	Yes,
	Wait,
	Leader
};

//-----------------------------------------------------------------------------
// Frozen state - used to disable unit movement
// used for warping between locations, entering buildings
enum class FROZEN
{
	NO, // not frozen
	ROTATE, // can't move but can rotate (used after warp to arena combat)
	YES, // can't move/rotate
	YES_NO_ANIM, // can't move/rotate but don't change animation to standing (used by cheat 'warp')
};

//-----------------------------------------------------------------------------
enum UnitOrder
{
	ORDER_NONE,
	ORDER_WANDER,
	ORDER_WAIT,
	ORDER_FOLLOW,
	ORDER_LEAVE,
	ORDER_MOVE,
	ORDER_LOOK_AT,
	ORDER_ESCAPE_TO,
	ORDER_ESCAPE_TO_UNIT,
	ORDER_GOTO_INN,
	ORDER_MAX
};

//-----------------------------------------------------------------------------
struct TraderStock
{
	vector<ItemSlot> items;
	int date;
};

//-----------------------------------------------------------------------------
// jednostka w grze
struct Unit
{
	enum LiveState
	{
		ALIVE,
		FALLING,
		FALL,
		DYING,
		DEAD,
		LIVESTATE_MAX
	};

	enum StaminaAction
	{
		SA_DONT_RESTORE,
		SA_RESTORE_SLOW,
		SA_RESTORE,
		SA_RESTORE_MORE
	};

	enum LoadState
	{
		LS_NONE, // < 25%
		LS_LIGHT, // < 50%
		LS_MEDIUM, // < 75%
		LS_HEAVY, // < 100%
		LS_OVERLOADED, // < 200%
		LS_MAX_OVERLOADED // >= 200%
	};

	// used in mp by clients
	enum AiMode
	{
		AI_MODE_DONT_ATTACK = 1 << 0,
		AI_MODE_ASSIST = 1 << 1,
		AI_MODE_IDLE = 1 << 3,
		AI_MODE_ATTACK_TEAM = 1 << 4
	};

	enum class CREATE_MESH
	{
		NORMAL,
		ON_WORLDMAP,
		PRELOAD,
		AFTER_PRELOAD,
		LOAD
	};

	enum MoveType
	{
		MOVE_RUN,
		MOVE_WALK,
		MOVE_RUN_WHEN_NEAR_TEAM
	};

	static const int MIN_SIZE = 36;
	static const float AUTO_TALK_WAIT;
	static const float STAMINA_BOW_ATTACK;
	static const float STAMINA_BASH_ATTACK;
	static const float STAMINA_UNARMED_ATTACK;
	static const float STAMINA_RESTORE_TIMER;
	static const float EAT_SOUND_DIST;
	static const float DRINK_SOUND_DIST;
	static const float ATTACK_SOUND_DIST;
	static const float TALK_SOUND_DIST;
	static const float ALERT_SOUND_DIST;
	static const float PAIN_SOUND_DIST;
	static const float DIE_SOUND_DIST;
	static const float YELL_SOUND_DIST;
	static int netid_counter;

	int netid;
	UnitData* data;
	PlayerController* player;
	AIController* ai;
	HeroData* hero;
	Human* human_data;
	MeshInstance* mesh_inst;
	Animation animation, current_animation;
	LiveState live_state;
	Vec3 pos, visual_pos, prev_pos, target_pos, target_pos2;
	float rot, prev_speed, hp, hpmax, stamina, stamina_max, speed, hurt_timer, talk_timer, timer, use_rot, attack_power, last_bash, alcohol, raise_timer;
	int refs, animation_state, level, gold, attack_id, refid, area_id, in_arena, quest_refid;
	FROZEN frozen;
	ACTION action;
	WeaponType weapon_taken, weapon_hiding;
	WeaponState weapon_state;
	MeshInstance* bow_instance;
	const Item* used_item;
	bool used_item_is_team;
	vector<Effect> effects;
	bool hitted, talking, run_attack, to_remove, temporary, changed, dont_attack, assist, attack_team, fake_unit, moved, mark, running;
	btCollisionObject* cobj;
	Usable* usable;
	UnitEventHandler* event_handler;
	SpeechBubble* bubble;
	SmartPtr<Unit> look_target;
	Unit *guard_target, *summoner;
	int ai_mode;
	enum Busy
	{
		Busy_No,
		Busy_Yes,
		Busy_Talking,
		Busy_Looted,
		Busy_Trading,
		Busy_Tournament
	} busy; // nie zapisywane, powinno by� Busy_No
	EntityInterpolator* interp;
	UnitStats* stats;
	AutoTalkMode auto_talk;
	float auto_talk_timer;
	GameDialog* auto_talk_dialog;
	StaminaAction stamina_action;
	float stamina_timer;
	TraderStock* stock;
	vector<QuestDialog> dialogs;
	vector<Event> events;
	UnitOrder order;
	float order_timer;
	Vec3 order_pos;
	MoveType order_move_type;
	Unit* order_unit;

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	Unit() : mesh_inst(nullptr), hero(nullptr), ai(nullptr), player(nullptr), cobj(nullptr), interp(nullptr), bow_instance(nullptr), fake_unit(false),
		human_data(nullptr), stamina_action(SA_RESTORE_MORE), summoner(nullptr), moved(false), refs(1), stock(nullptr), stats(nullptr), mark(false),
		order(ORDER_NONE) {}
	~Unit();

	void AddRef() { ++refs; }
	void Release();

	float CalculateAttack() const;
	float CalculateAttack(const Item* weapon) const;
	float CalculateBlock(const Item* shield = nullptr) const;
	float CalculateDefense(const Item* armor = nullptr) const;
	// czy �yje i nie le�y na ziemi
	bool IsStanding() const { return live_state == ALIVE; }
	// czy �yje
	bool IsAlive() const { return live_state < DYING; }
	bool IsIdle() const;
	bool IsAssist() const;
	bool IsDontAttack() const;
	bool WantAttackTeam() const;
	byte GetAiMode() const;
	void RecalculateWeight();
	// konsumuje przedmiot (zwraca 0-u�yto ostatni, 1-u�yto nie ostatni, 2-chowa bro�, 3-zaj�ty)
	int ConsumeItem(int index);
	// u�ywa przedmiotu, nie mo�e nic robi� w tej chwili i musi mie� schowan� bro�
	void ConsumeItem(const Consumable& item, bool force = false, bool send = true);
	void ConsumeItemAnim(const Consumable& cons);
	void UseItem(int index);
	void HideWeapon();
	void TakeWeapon(WeaponType type);
	float GetSphereRadius() const
	{
		float radius = data->mesh->head.radius;
		if(data->type == UNIT_TYPE::HUMAN)
			radius *= ((human_data->height - 1)*0.2f + 1.f);
		return radius;
	}
	float GetUnitRadius() const
	{
		if(data->type == UNIT_TYPE::HUMAN)
			return 0.3f * ((human_data->height - 1)*0.2f + 1.f);
		else
			return data->width;
	}
	Vec3 GetColliderPos() const
	{
		if(action != A_ANIMATION2)
			return pos;
		else if(animation_state == AS_ANIMATION2_MOVE_TO_ENDPOINT)
			return target_pos;
		else
			return target_pos2;
	}
	float GetUnitHeight() const
	{
		if(data->type == UNIT_TYPE::HUMAN)
			return 1.73f * ((human_data->height - 1)*0.2f + 1.f);
		else
			return data->mesh->head.bbox.SizeY();
	}
	Vec3 GetPhysicsPos() const
	{
		Vec3 p = pos;
		p.y += max(1.5f, GetUnitHeight()) * 0.5f + 0.4f;
		return p;
	}
	Vec3 GetHeadPoint() const
	{
		Vec3 pt = visual_pos;
		pt.y += GetUnitHeight() * 1.1f;
		return pt;
	}
	Vec3 GetHeadSoundPos() const
	{
		Vec3 pt = visual_pos;
		pt.y += GetUnitHeight() * 0.9f;
		return pt;
	}
	Vec3 GetUnitTextPos() const
	{
		Vec3 pt;
		if(IsStanding())
		{
			pt = visual_pos;
			pt.y += GetUnitHeight();
		}
		else
		{
			pt = GetLootCenter();
			pt.y += 0.5f;
		}
		return pt;
	}
	Vec3 GetEyePos() const;
	float CalculateMaxHp() const;
	float CalculateMaxStamina() const;
	float GetHpp() const { return hp / hpmax; }
	float GetStaminap() const { return stamina / stamina_max; }
	void GetBox(Box& box) const;
	int GetDmgType() const;
	bool IsNotFighting() const
	{
		if(data->type == UNIT_TYPE::ANIMAL)
			return false;
		else
			return (weapon_state == WS_HIDDEN);
	}
	Vec3 GetLootCenter() const;

	float CalculateWeaponPros(const Weapon& weapon) const;
	bool IsBetterWeapon(const Weapon& weapon, int* value = nullptr, int* prev_value = nullptr) const;
	bool IsBetterArmor(const Armor& armor, int* value = nullptr, int* prev_value = nullptr) const;
	bool IsBetterItem(const Item* item, int* value = nullptr, int* prev_value = nullptr, ITEM_SLOT* target_slot = nullptr) const;
	float GetItemAiValue(const Item* item) const;
	bool IsPlayer() const { return (player != nullptr); }
	bool IsClient() const { return IsPlayer() && !player->IsLocal(); }
	bool IsLocal() const { return IsPlayer() && player->IsLocal(); }
	bool IsAI() const { return !IsPlayer(); }
	float GetRotationSpeed() const
	{
		return data->rot_speed * GetMobilityMod(false);
	}
	float GetWalkSpeed() const
	{
		return data->walk_speed * GetMobilityMod(false);
	}
	float GetRunSpeed() const
	{
		return data->run_speed * GetMobilityMod(true);
	}
	bool CanRun() const
	{
		if(IS_SET(data->flags, F_SLOW) || Any(action, A_BLOCK, A_BASH, A_SHOOT, A_USE_ITEM) || (action == A_ATTACK && !run_attack))
			return false;
		else
			return !IsOverloaded();
	}
	void RecalculateHp(bool send = false);
	void RecalculateStamina();
	bool CanBlock() const
	{
		return weapon_state == WS_TAKEN && weapon_taken == W_ONE_HANDED && HaveShield();
	}

	WeaponType GetHoldWeapon() const
	{
		switch(weapon_state)
		{
		case WS_TAKEN:
		case WS_TAKING:
			return weapon_taken;
		case WS_HIDING:
			return weapon_hiding;
		case WS_HIDDEN:
			return W_NONE;
		default:
			assert(0);
			return W_NONE;
		}
	}
	bool IsHoldingMeeleWeapon() const
	{
		return GetHoldWeapon() == W_ONE_HANDED;
	}
	bool IsHoldingBow() const
	{
		return GetHoldWeapon() == W_BOW;
	}
	Vec3 GetFrontPos() const
	{
		return Vec3(
			pos.x + sin(rot + PI) * 2,
			pos.y,
			pos.z + cos(rot + PI) * 2);
	}
	MATERIAL_TYPE GetWeaponMaterial() const
	{
		if(HaveWeapon())
			return GetWeapon().material;
		else
			return data->mat;
	}
	MATERIAL_TYPE GetBodyMaterial() const
	{
		if(HaveArmor())
			return GetArmor().material;
		else
			return data->mat;
	}
	float GetAttackFrame(int frame) const;
	int GetRandomAttack() const;
	void Save(GameWriter& f, bool local);
	void SaveStock(GameWriter& f);
	void Load(GameReader& f, bool local);
	void LoadStock(GameReader& f);
	void Write(BitStreamWriter& f);
	bool Read(BitStreamReader& f);
	Vec3 GetCenter() const
	{
		Vec3 pt = pos;
		pt.y += GetUnitHeight() / 2;
		return pt;
	}
	int FindHealingPotion() const;
	float GetAttackRange() const
	{
		return data->attack_range;
	}
	void ReequipItems();
private:
	void ReequipItemsInternal();
public:
	bool CanUseWeapons() const
	{
		return data->type >= UNIT_TYPE::HUMANOID;
	}
	bool CanUseArmor() const
	{
		return data->type == UNIT_TYPE::HUMAN;
	}
	bool HaveQuestItem(int quest_refid);
	void RemoveQuestItem(int quest_refid);
	void RemoveQuestItemS(Quest* quest);
	bool HaveItem(const Item* item, bool owned = false) const;
	bool HaveItemEquipped(const Item* item) const;
	bool SlotRequireHideWeapon(ITEM_SLOT slot) const;
	float GetAttackSpeed(const Weapon* weapon = nullptr) const;
	float GetAttackSpeedModFromStrength(const Weapon& wep) const
	{
		int str = Get(AttributeId::STR);
		if(str >= wep.req_str)
			return 0.f;
		else if(str * 2 <= wep.req_str)
			return 0.5f;
		else
			return 0.5f * float(wep.req_str - str) / (wep.req_str / 2);
	}
	float GetPowerAttackSpeed() const
	{
		if(HaveWeapon())
			return GetWeapon().GetInfo().power_speed * GetAttackSpeed();
		else
			return 0.33f;
	}
	float GetBowAttackSpeed() const;
	float GetAttackSpeedModFromStrength(const Bow& b) const
	{
		int str = Get(AttributeId::STR);
		if(str >= b.req_str)
			return 0.f;
		else if(str * 2 <= b.req_str)
			return 0.75f;
		else
			return 0.75f * float(b.req_str - str) / (b.req_str / 2);
	}
	bool IsHero() const { return hero != nullptr; }
	bool IsFollower() const { return hero && hero->team_member; }
	bool IsFollowing(Unit* u) const { return order == ORDER_FOLLOW && order_unit == u; }
	bool IsFollowingTeamMember() const { return IsFollower() && order == ORDER_FOLLOW; }
	Class GetClass() const { return data->clas; }
	bool CanFollowWarp() const { return IsHero() && order == ORDER_FOLLOW && in_arena == -1 && frozen == FROZEN::NO; }
	bool IsTeamMember() const
	{
		if(IsPlayer())
			return true;
		else if(IsHero())
			return hero->team_member;
		else
			return false;
	}
	void MakeItemsTeam(bool team);
	void Heal(float heal)
	{
		hp += heal;
		if(hp > hpmax)
			hp = hpmax;
	}
	void NaturalHealing(int days)
	{
		Heal(0.15f * Get(AttributeId::END) * days);
	}
	void HealPoison();
	// szuka przedmiotu w ekwipunku, zwraca i_index (INVALID_IINDEX je�li nie ma takiego przedmiotu)
	static const int INVALID_IINDEX = (-SLOT_INVALID - 1);
	int FindItem(const Item* item, int quest_refid = -1) const;
	int FindItem(delegate<bool(const ItemSlot& slot)> callback) const;
	int FindQuestItem(int quest_refid) const;
	bool FindQuestItem(cstring id, Quest** quest, int* i_index, bool not_active = false, int required_refid = -1);
	void RemoveItem(int iindex, bool active_location = true);
	uint RemoveItem(int i_index, uint count);
	uint RemoveItem(const Item* item, uint count);
	uint RemoveItemS(const string& item_id, uint count);
	int CountItem(const Item* item);
	const string& GetNameS() const
	{
		if(IsPlayer())
			return player->name;
		else if(IsHero() && hero->know_name)
			return hero->name;
		else
			return data->name;
	}
	cstring GetName() const { return GetNameS().c_str(); }
	void SetName(const string& name);
	void ClearInventory();
	bool PreferMelee()
	{
		if(IsFollower())
			return hero->melee;
		else
			return IS_SET(data->flags2, F2_MELEE);
	}
	bool IsImmortal() const
	{
		if(IS_SET(data->flags, F_IMMORTAL))
			return true;
		else if(IsPlayer())
			return player->godmode;
		else
			return false;
	}
	cstring GetRealName() const
	{
		if(IsPlayer())
			return player->name.c_str();
		else if(IsHero() && !hero->name.empty())
			return hero->name.c_str();
		else
			return data->name.c_str();
	}
	void RevealName(bool set_name);
	bool GetKnownName() const;
	void SetKnownName(bool known);

	// szybko�� blokowania aktualnie u�ywanej tarczy (im mniejsza tym lepiej)
	float GetBlockSpeed() const;

	float CalculateMagicResistance() const;
	float GetPoisonResistance() const;
	int CalculateMagicPower() const { return (int)GetEffectSum(EffectId::MagicPower); }
	float GetBackstabMod(const Item* item) const;

	//-----------------------------------------------------------------------------
	// EFFECTS
	//-----------------------------------------------------------------------------
	void AddEffect(Effect& e, bool send = true);
	// dodaj efekt zjadanego przedmiotu
	void ApplyConsumableEffect(const Consumable& item);
	// aktualizuj efekty
	void UpdateEffects(float dt);
	// zako�cz tymczasowe efekty po opuszczeniu lokacji
	void EndEffects(int days = 0, float* o_natural_mod = nullptr);
	Effect* FindEffect(EffectId effect);
	bool FindEffect(EffectId effect, float* value);
	void RemoveEffect(EffectId effect);
	void RemovePoison()
	{
		RemoveEffect(EffectId::Poison);
	}
	void RemoveEffects(bool send = true);
	uint RemoveEffects(EffectId effect, EffectSource source, int source_id, int value);
	float GetEffectSum(EffectId effect) const;
	float GetEffectMul(EffectId effect) const;
	float GetEffectMulInv(EffectId effect) const;
	float GetEffectMax(EffectId effect) const;
	bool HaveEffect(EffectId effect) const;
	void OnAddRemoveEffect(Effect& e);
	void ApplyItemEffects(const Item* item, ITEM_SLOT slot);
	void RemoveItemEffects(const Item* item, ITEM_SLOT slot);

	//-----------------------------------------------------------------------------
	// EQUIPMENT
	//-----------------------------------------------------------------------------
	const Item* slots[SLOT_MAX];
	vector<ItemSlot> items;
	int weight, weight_max;
	//-----------------------------------------------------------------------------
	int GetGold() const { return gold; }
	void ModGold(int gold_change) { SetGold(gold + gold_change); }
	void SetGold(int gold);
	bool HaveWeapon() const { return slots[SLOT_WEAPON] != nullptr; }
	bool HaveBow() const { return slots[SLOT_BOW] != nullptr; }
	bool HaveShield() const { return slots[SLOT_SHIELD] != nullptr; }
	bool HaveArmor() const { return slots[SLOT_ARMOR] != nullptr; }
	bool HaveAmulet() const { return slots[SLOT_AMULET] != nullptr; }
	bool CanWear(const Item* item) const;
	const Weapon& GetWeapon() const
	{
		assert(HaveWeapon());
		return slots[SLOT_WEAPON]->ToWeapon();
	}
	const Bow& GetBow() const
	{
		assert(HaveBow());
		return slots[SLOT_BOW]->ToBow();
	}
	const Shield& GetShield() const
	{
		assert(HaveShield());
		return slots[SLOT_SHIELD]->ToShield();
	}
	const Armor& GetArmor() const
	{
		assert(HaveArmor());
		return slots[SLOT_ARMOR]->ToArmor();
	}
	const Item& GetAmulet() const
	{
		assert(HaveAmulet());
		return *slots[SLOT_AMULET];
	}
	// wyrzuca przedmiot o podanym indeksie, zwraca czy to by� ostatni
	bool DropItem(int index);
	// wyrzuca za�o�ony przedmiot
	void DropItem(ITEM_SLOT slot);
	// wyrzuca kilka przedmiot�w o podanym indeksie, zwraca czy to by� ostatni (count=0 oznacza wszystko)
	bool DropItems(int index, uint count);
	// dodaje przedmiot do ekwipunku, zwraca czy si� zestackowa�
	bool AddItem(const Item* item, uint count, uint team_count);
	// add item and show game message, send net notification, calls preload, refresh inventory if open
	void AddItem2(const Item* item, uint count, uint team_count, bool show_msg = true, bool notify = true);
	void AddItemS(const Item* item, uint count) { AddItem2(item, count, 0u); }
	void AddTeamItemS(const Item* item, uint count) { AddItem2(item, count, count); }
	// dodaje przedmiot i zak�ada je�li nie ma takiego typu, przedmiot jest dru�ynowy
	void AddItemAndEquipIfNone(const Item* item, uint count = 1);
	// zwraca ud�wig postaci (0-brak obci��enia, 1-maksymalne, >1 przeci��ony)
	float GetLoad() const { return float(weight) / weight_max; }
	void CalculateLoad();
	bool IsOverloaded() const
	{
		return weight >= weight_max;
	}
	bool IsMaxOverloaded() const
	{
		return weight >= weight_max * 2;
	}
	LoadState GetLoadState() const
	{
		if(weight < weight_max / 4)
			return LS_NONE;
		else if(weight < weight_max / 2)
			return LS_LIGHT;
		else if(weight < weight_max * 3 / 4)
			return LS_MEDIUM;
		else if(weight < weight_max)
			return LS_HEAVY;
		else if(weight < weight_max * 2)
			return LS_OVERLOADED;
		else
			return LS_MAX_OVERLOADED;
	}
	LoadState GetArmorLoadState(const Item* armor) const;
	// zwraca wag� ekwipunku w kg
	float GetWeight() const
	{
		return float(weight) / 10;
	}
	// zwraca maksymalny ud�wig w kg
	float GetWeightMax() const
	{
		return float(weight_max) / 10;
	}
	bool CanTake(const Item* item, uint count = 1) const
	{
		assert(item && count);
		return weight + item->weight*(int)count <= weight_max;
	}
	const Item* GetIIndexItem(int i_index) const;

	Mesh::Animation* GetTakeWeaponAnimation(bool melee) const;

	bool CanDoWhileUsing() const
	{
		return action == A_ANIMATION2 && animation_state == AS_ANIMATION2_USING && IS_SET(usable->base->use_flags, BaseUsable::ALLOW_USE_ITEM);
	}

	int GetBuffs() const;

	// nie sprawdza czy stoi/�yje/czy chce gada� - tylko akcj�
	bool CanTalk() const
	{
		if(action == A_EAT || action == A_DRINK || action == A_STAND_UP || auto_talk == AutoTalkMode::Leader)
			return false;
		else
			return true;
	}
	bool CanAct();

	int Get(AttributeId a, StatState* state = nullptr) const;
	int Get(SkillId s, StatState* state = nullptr, bool skill_bonus = true) const;
	int GetBase(AttributeId a) const { return stats->attrib[(int)a]; }
	int GetBase(SkillId s) const { return stats->skill[(int)s]; }
	void Set(AttributeId a, int value);
	void Set(SkillId s, int value);
	void ApplyStat(AttributeId a);
	void CalculateStats();
	float CalculateMobility(const Armor* armor = nullptr) const;
	float GetMobilityMod(bool run) const;

	WEAPON_TYPE GetBestWeaponType() const;
	ARMOR_TYPE GetBestArmorType() const;

	void ApplyHumanData(HumanData& hd)
	{
		hd.Set(*human_data);
		human_data->ApplyScale(data->mesh);
	}

	int ItemsToSellWeight() const;

	void SetAnimationAtEnd(cstring anim_name = nullptr);

	float GetArrowSpeed() const
	{
		float s = (float)GetBow().speed;
		s *= 1.f + float(Get(SkillId::BOW)) / 666;
		return s;
	}

	void StartAutoTalk(bool leader = false, GameDialog* dialog = nullptr);
	void SetAutoTalk(bool auto_talk);
	bool GetAutoTalk() const { return auto_talk != AutoTalkMode::No; }
	void SetDontAttack(bool dont_attack);
	bool GetDontAttack() const { return dont_attack; }

	int& GetCredit()
	{
		if(IsPlayer())
			return player->credit;
		else
		{
			assert(IsFollower());
			return hero->credit;
		}
	}

	void UpdateStaminaAction();
	void RemoveStamina(float value);
	void RemoveStaminaBlock(float value);

	void CreateMesh(CREATE_MESH mode);

	void ApplyStun(float length);
	void UseUsable(Usable* usable);
	enum class BREAK_ACTION_MODE
	{
		NORMAL,
		FALL,
		INSTANT
	};
	void BreakAction(BREAK_ACTION_MODE mode = BREAK_ACTION_MODE::NORMAL, bool notify = false, bool allow_animation = false);
	void Fall();
	void TryStandup(float dt);
	void Standup();
	void Die(LevelArea& area, Unit* killer);
	void DropGold(int count);
	bool IsDrunkman() const;
	void PlaySound(SOUND snd, float range);
	void CreatePhysics(bool position = false);
	void UpdatePhysics(const Vec3& pos);
	SOUND GetSound(SOUND_ID sound_id) const;
	void SetWeaponState(bool takes_out, WeaponType type);
	void UpdateInventory(bool notify = true);
	bool IsEnemy(Unit& u, bool ignore_dont_attack = false) const;
	bool IsFriend(Unit& u) const;
	bool IsInvisible() const { return IsPlayer() && player->invisible; }
	void RefreshStock();
	float GetMaxMorale() const { return IS_SET(data->flags, F_COWARD) ? 5.f : 10.f; }
	void AddDialog(Quest_Scripted* quest, GameDialog* dialog);
	void AddDialogS(Quest_Scripted* quest, const string& dialog_id);
	void RemoveDialog(Quest_Scripted* quest, bool cleanup);
	void RemoveDialogS(Quest_Scripted* quest) { RemoveDialog(quest, false); }
	void AddEventHandler(Quest_Scripted* quest, EventType type);
	void RemoveEventHandler(Quest_Scripted* quest, bool cleanup);
	void RemoveEventHandlerS(Quest_Scripted* quest) { RemoveEventHandler(quest, false); }
	void RemoveAllEventHandlers();
	void OrderEscapeToUnit(Unit* unit);
	UnitOrder GetOrder() const { return order; }
	void SetOrder(UnitOrder order);
	void OrderAttack();
	void OrderClear();
	void OrderFollow(Unit* target);
	void OrderMove(const Vec3& pos, MoveType move_type);
	void OrderLookAt(const Vec3& pos);
	void OrderTimer(float time) { order_timer = time; }
	void Talk(cstring text, int play_anim = -1);
	void TalkS(const string& text, int play_anim = -1) { Talk(text.c_str(), play_anim); }
	bool IsBlocking() const { return action == A_BLOCK || (action == A_BASH && animation_state == 0); }
	float GetBlockMod() const { return action == A_BLOCK ? mesh_inst->groups[1].GetBlendT() : 0.5f; }
	float GetStaminaAttackSpeedMod() const;
	float GetBashSpeed() const { return 2.f * GetStaminaAttackSpeedMod(); }

	//-----------------------------------------------------------------------------
	static vector<Unit*> refid_table;
	static vector<pair<Unit**, int>> refid_request;

	// special value for AddRequest to get team leader
	static const int REFID_LEADER = -2;

	static Unit* GetByRefid(int _refid)
	{
		if(_refid == -1 || _refid >= (int)refid_table.size())
			return nullptr;
		else
			return refid_table[_refid];
	}
	static void AddRequest(Unit** unit, int refid)
	{
		assert(unit && refid != -1);
		refid_request.push_back(pair<Unit**, int>(unit, refid));
	}
	static void AddRefid(Unit* unit)
	{
		assert(unit);
		unit->refid = (int)refid_table.size();
		refid_table.push_back(unit);
	}
};

//-----------------------------------------------------------------------------
struct NAMES
{
	// punkty
	static cstring point_weapon;
	static cstring point_hidden_weapon;
	static cstring point_shield;
	static cstring point_shield_hidden;
	static cstring point_bow;
	static cstring point_cast;
	static cstring points[];
	static uint n_points;

	// animacje
	static cstring ani_take_weapon;
	static cstring ani_take_weapon_no_shield;
	static cstring ani_take_bow;
	static cstring ani_move;
	static cstring ani_run;
	static cstring ani_left;
	static cstring ani_right;
	static cstring ani_stand;
	static cstring ani_battle;
	static cstring ani_battle_bow;
	static cstring ani_die;
	static cstring ani_hurt;
	static cstring ani_shoot;
	static cstring ani_block;
	static cstring ani_bash;
	static cstring ani_base[];
	static cstring ani_humanoid[];
	static cstring ani_attacks[];
	static uint n_ani_base;
	static uint n_ani_humanoid;
	static int max_attacks;
};
