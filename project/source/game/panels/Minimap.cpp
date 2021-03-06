#include "Pch.h"
#include "GameCore.h"
#include "Minimap.h"
#include "Game.h"
#include "InsideLocation.h"
#include "City.h"
#include "Team.h"
#include "Portal.h"
#include "Level.h"
#include "ResourceManager.h"

enum UnitType
{
	UNIT_ME,
	UNIT_TEAM,
	UNIT_ENEMY,
	UNIT_NPC,
	UNIT_CORPSE
};

//=================================================================================================
Minimap::Minimap()
{
	visible = false;
}

//=================================================================================================
void Minimap::LoadData()
{
	auto& tex_mgr = ResourceManager::Get<Texture>();
	tex_mgr.AddLoadTask("mini_unit.png", tUnit[UNIT_ME]);
	tex_mgr.AddLoadTask("mini_unit2.png", tUnit[UNIT_TEAM]);
	tex_mgr.AddLoadTask("mini_unit3.png", tUnit[UNIT_ENEMY]);
	tex_mgr.AddLoadTask("mini_unit4.png", tUnit[UNIT_NPC]);
	tex_mgr.AddLoadTask("mini_unit5.png", tUnit[UNIT_CORPSE]);
	tex_mgr.AddLoadTask("schody_dol.png", tStairsDown);
	tex_mgr.AddLoadTask("schody_gora.png", tStairsUp);
	tex_mgr.AddLoadTask("mini_bag.png", tBag);
	tex_mgr.AddLoadTask("mini_bag2.png", tBagImportant);
	tex_mgr.AddLoadTask("mini_portal.png", tPortal);
	tex_mgr.AddLoadTask("mini_chest.png", tChest);
}

//=================================================================================================
void Minimap::Draw(ControlDrawData*)
{
	Game& game = Game::Get();
	LevelArea& area = *L.local_area;
	LOCATION type = L.location->type;

	// map texture
	Rect r = { global_pos.x, global_pos.y, global_pos.x + size.x, global_pos.y + size.y };
	Rect r_part = { 0, 0, minimap_size, minimap_size };
	GUI.DrawSpriteRectPart(game.tMinimap, r, r_part, Color::Alpha(140));

	// stairs
	if(type == L_DUNGEON || type == L_CRYPT || type == L_CAVE)
	{
		InsideLocation* inside = (InsideLocation*)L.location;
		InsideLocationLevel& lvl = inside->GetLevelData();

		if(inside->HaveDownStairs() && IS_SET(lvl.map[lvl.staircase_down(lvl.w)].flags, Tile::F_REVEALED))
			GUI.DrawSprite(tStairsDown, Int2(TileToPoint(lvl.staircase_down)) - Int2(16, 16), Color::Alpha(180));
		if(inside->HaveUpStairs() && IS_SET(lvl.map[lvl.staircase_up(lvl.w)].flags, Tile::F_REVEALED))
			GUI.DrawSprite(tStairsUp, Int2(TileToPoint(lvl.staircase_up)) - Int2(16, 16), Color::Alpha(180));
	}

	// portals
	Portal* p = L.location->portal;
	InsideLocationLevel* lvl = nullptr;
	if(L.location->type == L_DUNGEON || L.location->type == L_CRYPT || L.location->type == L_CAVE)
		lvl = &((InsideLocation*)L.location)->GetLevelData();
	while(p)
	{
		if(!lvl || (L.dungeon_level == p->at_level && lvl->IsTileVisible(p->pos)))
			GUI.DrawSprite(tPortal, Int2(TileToPoint(PosToPt(p->pos))) - Int2(24, 8), Color::Alpha(180));
		p = p->next_portal;
	}

	// chests
	for(Chest* chest : area.chests)
	{
		if(!lvl || lvl->IsTileVisible(chest->pos))
		{
			m1 = Matrix::Transform2D(&Vec2(16, 16), 0.f, &Vec2(0.5f, 0.5f), nullptr, 0.f, &(PosToPoint(Vec2(chest->pos.x, chest->pos.z)) - Vec2(16, 16)));
			GUI.DrawSpriteTransform(tChest, m1, Color::Alpha(140));
		}
	}

	// items
	LocalVector<GroundItem*> important_items;
	for(vector<GroundItem*>::iterator it = area.items.begin(), end = area.items.end(); it != end; ++it)
	{
		if(IS_SET((*it)->item->flags, ITEM_IMPORTANT))
			important_items->push_back(*it);
		else if(!lvl || lvl->IsTileVisible((*it)->pos))
		{
			m1 = Matrix::Transform2D(&Vec2(16, 16), 0.f, &Vec2(0.25f, 0.25f), nullptr, 0.f, &(PosToPoint(Vec2((*it)->pos.x, (*it)->pos.z)) - Vec2(16, 16)));
			GUI.DrawSpriteTransform(tBag, m1, Color::Alpha(140));
		}
	}
	for(vector<GroundItem*>::iterator it = important_items->begin(), end = important_items->end(); it != end; ++it)
	{
		if(!lvl || lvl->IsTileVisible((*it)->pos))
		{
			m1 = Matrix::Transform2D(&Vec2(16, 16), 0.f, &Vec2(0.25f, 0.25f), nullptr, 0.f, &(PosToPoint(Vec2((*it)->pos.x, (*it)->pos.z)) - Vec2(16, 16)));
			GUI.DrawSpriteTransform(tBagImportant, m1, Color::Alpha(140));
		}
	}

	// team members
	for(Unit& unit : Team.members)
	{
		m1 = Matrix::Transform2D(&Vec2(16, 16), 0.f, &Vec2(0.25f, 0.25f), &Vec2(16, 16), unit.rot, &(PosToPoint(GetMapPosition(unit)) - Vec2(16, 16)));
		GUI.DrawSpriteTransform(tUnit[&unit == game.pc->unit ? UNIT_ME : UNIT_TEAM], m1, Color::Alpha(140));
	}

	// other units
	for(vector<Unit*>::iterator it = area.units.begin(), end = area.units.end(); it != end; ++it)
	{
		Unit& u = **it;
		if((u.IsAlive() || u.mark) && !u.IsTeamMember() && (!lvl || lvl->IsTileVisible(u.pos)))
		{
			m1 = Matrix::Transform2D(&Vec2(16, 16), 0.f, &Vec2(0.25f, 0.25f), &Vec2(16, 16), (*it)->rot, &(PosToPoint(GetMapPosition(u)) - Vec2(16, 16)));
			GUI.DrawSpriteTransform(tUnit[u.IsAlive() ? (u.IsEnemy(*game.pc->unit) ? UNIT_ENEMY : UNIT_NPC) : UNIT_CORPSE], m1, Color::Alpha(140));
		}
	}

	if(L.city_ctx)
	{
		// building names
		for(Text& text : texts)
		{
			Int2 pt(Convert(text.pos));
			Rect rect = { pt.x - text.size.x / 2, pt.y - text.size.y / 2, pt.x + text.size.x / 2, pt.y + text.size.y / 2 };
			GUI.DrawText(GUI.default_font, text.text, DTF_SINGLELINE, Color::Black, rect);
		}

		// lines from building name to building
		GUI.LineBegin();
		for(vector<Text>::iterator it = texts.begin(), end = texts.end(); it != end; ++it)
		{
			Vec2 pts[2] = {
				Convert(it->pos),
				Convert(it->anchor)
			};
			GUI.DrawLine(pts, 1, Color(0, 0, 0, 140));
		}
		GUI.LineEnd();
	}

	// location name
	Rect rect = { 0,0,GUI.wnd_size.x - 8,GUI.wnd_size.y - 8 };
	GUI.DrawText(GUI.default_font, L.GetCurrentLocationText(), DTF_RIGHT | DTF_OUTLINE, Color(255, 0, 0, 222), rect);
}

//=================================================================================================
void Minimap::Update(float dt)
{
	if(L.city_ctx)
	{
		for(vector<Text>::iterator it = texts.begin(), end = texts.end(); it != end; ++it)
		{
			for(vector<Text>::iterator it2 = texts.begin(); it2 != end; ++it2)
			{
				if(it == it2)
					continue;

				Vec2 pt1 = Convert(it->pos),
					pt2 = Convert(it2->pos);
				float w1 = float(it->size.x + 2) / 2,
					h1 = float(it->size.y + 2) / 2,
					w2 = float(it2->size.x + 2) / 2,
					h2 = float(it2->size.y + 2) / 2;

				if(RectangleToRectangle(pt1.x - w1, pt1.y - h1, pt1.x + w1, pt1.y + h1,
					pt2.x - w2, pt2.y - h2, pt2.x + w2, pt2.y + h2))
				{
					Vec2 dir = it->pos - it2->pos;
					dir.Normalize();
					it->pos += dir * dt;
				}
			}
		}
	}

	if(focus && Key.Focus() && Key.PressedRelease(VK_ESCAPE))
		Hide();
}

//=================================================================================================
void Minimap::Event(GuiEvent e)
{
	if(e == GuiEvent_Resize)
	{
		city = nullptr;
		Build();
	}
}

//=================================================================================================
void Minimap::Show()
{
	visible = true;
	Build();
}

//=================================================================================================
void Minimap::Hide()
{
	visible = false;
}

//=================================================================================================
void Minimap::Build()
{
	minimap_size = L.minimap_size;
	if(L.city_ctx && L.city_ctx != city)
	{
		city = L.city_ctx;
		texts.clear();

		for(CityBuilding& b : city->buildings)
		{
			if(IS_SET(b.building->flags, Building::HAVE_NAME))
			{
				Text& text = Add1(texts);
				text.text = b.building->name.c_str();
				text.size = GUI.default_font->CalculateSize(b.building->name);
				text.pos = text.anchor = TransformTile(b.pt);
			}
		}
	}
}

//=================================================================================================
Vec2 Minimap::GetMapPosition(Unit& unit)
{
	if(!L.city_ctx || unit.area_id == LevelArea::OUTSIDE_ID)
		return Vec2(unit.pos.x, unit.pos.z);
	else
	{
		Building* building = L.city_ctx->inside_buildings[unit.area_id]->building;
		for(CityBuilding& b : L.city_ctx->buildings)
		{
			if(b.building == building)
				return Vec2(float(b.pt.x * 2), float(b.pt.y * 2));
		}
	}
	assert(0);
	return Vec2(-1000, -1000);
}
