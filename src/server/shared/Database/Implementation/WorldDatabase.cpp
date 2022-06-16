////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "WorldDatabase.h"

void WorldDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_WORLDDATABASE_STATEMENTS);

    PREPARE_STATEMENT(WORLD_SEL_QUEST_POOLS, "SELECT entry, pool_entry FROM pool_quest", CONNECTION_SYNCH)
    PREPARE_STATEMENT(WORLD_DEL_CRELINKED_RESPAWN, "DELETE FROM linked_respawn WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(WORLD_REP_CREATURE_LINKED_RESPAWN, "REPLACE INTO linked_respawn (guid, linkedGuid) VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(WORLD_SEL_CREATURE_TEXT, "SELECT Entry, GroupID, ID, Text, Type, Language, Probability, Emote, Duration, Sound, BroadcastTextID FROM creature_text", CONNECTION_SYNCH)
    PREPARE_STATEMENT(WORLD_SEL_SMART_SCRIPTS, "SELECT entryorguid, linked_id, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_spawnMask, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_x, target_y, target_z, target_o FROM smart_scripts ORDER BY entryorguid, source_type, id, link", CONNECTION_SYNCH)
    PREPARE_STATEMENT(WORLD_SEL_SMARTAI_WP, "SELECT entry, pointid, position_x, position_y, position_z FROM waypoints ORDER BY entry, pointid", CONNECTION_SYNCH)
    PREPARE_STATEMENT(WORLD_DEL_GAMEOBJECT, "DELETE FROM gameobject WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_EVENT_GAMEOBJECT, "DELETE FROM game_event_gameobject WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_GRAVEYARD_ZONE, "INSERT INTO game_graveyard_zone (id, ghost_zone, faction) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_GRAVEYARD_ZONE, "DELETE FROM game_graveyard_zone WHERE id = ? AND ghost_zone = ? AND faction = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_GAME_TELE, "INSERT INTO game_tele (id, position_x, position_y, position_z, orientation, map, name) VALUES (?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_GAME_TELE, "DELETE FROM game_tele WHERE name = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_NPC_VENDOR, "INSERT INTO npc_vendor (entry, item, maxcount, incrtime, extendedcost, type, PlayerConditionID) VALUES(?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_NPC_VENDOR, "DELETE FROM npc_vendor WHERE entry = ? AND item = ? AND type = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_NPC_VENDOR_REF, "SELECT item, maxcount, incrtime, ExtendedCost, type, PlayerConditionID, Bonuses FROM npc_vendor WHERE entry = ? AND type = ? ORDER BY slot ASC", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_MOVEMENT_TYPE, "UPDATE creature SET MovementType = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_FACTION, "UPDATE creature_template SET faction = ? WHERE entry = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_NPCFLAG, "UPDATE creature_template SET npcflag = ? WHERE entry = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_NPCFLAG2, "UPDATE creature_template SET npcflag2 = ? WHERE entry = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_POSITION, "UPDATE creature SET position_x = ?, position_y = ?, position_z = ?, orientation = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_SPAWN_DISTANCE, "UPDATE creature SET spawndist = ?, MovementType = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_SPAWN_TIME_SECS, "UPDATE creature SET spawntimesecs = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_CREATURE_FORMATION, "INSERT INTO creature_formations (leaderLinkedId, memberLinkedId, dist, angle, groupAI) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_WAYPOINT_DATA, "INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_WAYPOINT_DATA, "DELETE FROM waypoint_data WHERE id = ? AND point = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_DATA_POINT, "UPDATE waypoint_data SET point = point - 1 WHERE id = ? AND point > ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_DATA_POSITION, "UPDATE waypoint_data SET position_x = ?, position_y = ?, position_z = ? where id = ? AND point = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_DATA_WPGUID, "UPDATE waypoint_data SET wpguid = ? WHERE id = ? and point = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_MAX_ID, "SELECT MAX(id) FROM waypoint_data", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_MAX_POINT, "SELECT MAX(point) FROM waypoint_data WHERE id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_BY_ID, "SELECT point, position_x, position_y, position_z, orientation, move_type, speed, delay, action, action_chance FROM waypoint_data WHERE id = ? ORDER BY point", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_SCRIPT_BY_ID, "SELECT point, position_x, position_y, position_z, orientation, move_type, delay, action, action_chance FROM waypoint_data_script WHERE id = ? ORDER BY point", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_POS_BY_ID, "SELECT point, position_x, position_y, position_z FROM waypoint_data WHERE id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_POS_FIRST_BY_ID, "SELECT position_x, position_y, position_z FROM waypoint_data WHERE point = 1 AND id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_POS_LAST_BY_ID, "SELECT position_x, position_y, position_z, orientation FROM waypoint_data WHERE id = ? ORDER BY point DESC LIMIT 1", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_BY_WPGUID, "SELECT id, point FROM waypoint_data WHERE wpguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_ALL_BY_WPGUID, "SELECT id, point, delay, move_type, action, action_chance FROM waypoint_data WHERE wpguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_DATA_ALL_WPGUID, "UPDATE waypoint_data SET wpguid = 0", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_BY_POS, "SELECT id, point FROM waypoint_data WHERE (abs(position_x - ?) <= ?) and (abs(position_y - ?) <= ?) and (abs(position_z - ?) <= ?)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_WPGUID_BY_ID, "SELECT wpguid FROM waypoint_data WHERE id = ? and wpguid <> 0", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_DATA_ACTION, "SELECT DISTINCT action FROM waypoint_data", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_SCRIPTS_MAX_ID, "SELECT MAX(guid) FROM waypoint_scripts", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_INS_CREATURE_ADDON, "INSERT INTO creature_addon(linked_id, path_id) VALUES (?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_ADDON_PATH, "UPDATE creature_addon SET path_id = ? WHERE linked_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_CREATURE_ADDON, "DELETE FROM creature_addon WHERE linked_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_CREATURE_ADDON_BY_ANIMKIT, "INSERT INTO creature_addon (linked_id, aiAnimKit) VALUES (?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_ADDON_ANIMKIT, "UPDATE creature_addon SET aiAnimKit = ? WHERE linked_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_CREATURE_ADDON_BY_EMOTESTATE, "INSERT INTO creature_addon (linked_id, emote) VALUES (?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_ADDON_EMOTESTATE, "UPDATE creature_addon SET emote = ? WHERE linked_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_CREATURE_ADDON_BY_GUID, "SELECT linked_id FROM creature_addon WHERE linked_id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_INS_WAYPOINT_SCRIPT, "INSERT INTO waypoint_scripts (guid) VALUES (?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_WAYPOINT_SCRIPT, "DELETE FROM waypoint_scripts WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_SCRIPT_ID, "UPDATE waypoint_scripts SET id = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_SCRIPT_X, "UPDATE waypoint_scripts SET x = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_SCRIPT_Y, "UPDATE waypoint_scripts SET y = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_SCRIPT_Z, "UPDATE waypoint_scripts SET z = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_UPD_WAYPOINT_SCRIPT_O, "UPDATE waypoint_scripts SET o = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_SCRIPT_ID_BY_GUID, "SELECT id FROM waypoint_scripts WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_DEL_CREATURE, "DELETE FROM creature WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_COMMANDS, "SELECT name, security, security_animator, help FROM command", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_CREATURE_TEMPLATE, "SELECT gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, npcflag2, speed_walk, speed_run, speed_fly, scale, dmgschool, dmg_multiplier, baseattacktime, rangeattacktime, baseVariance, rangeVariance, unit_class, unit_flags, unit_flags2, unit_flags3, dynamicflags, WorldEffectID, trainer_type, trainer_spell, trainer_class, trainer_race, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Mana_mod_extra, Armor_mod, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, ScaleLevelMin, ScaleLevelMax, ScaleLevelDelta FROM creature_template WHERE entry = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_CREATURE_TEMPLATE_WDB, "SELECT Name1, Name2, Name3, Name4, NameAlt1, NameAlt2, NameAlt3, NameAlt4, Title, TitleAlt, CursorName, Type, TypeFlags, TypeFlags2, RequiredExpansion, Family, Classification, MovementInfoID, HpMulti, PowerMulti, Leader, KillCredit1, KillCredit2, VignetteID, DisplayId1, DisplayId2, DisplayId3, DisplayId4, FlagQuest, QuestItem1, QuestItem2, QuestItem3, QuestItem4, QuestItem5, QuestItem6, QuestItem7, QuestItem8, QuestItem9, QuestItem10 FROM creature_template_wdb WHERE entry = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WAYPOINT_SCRIPT_BY_ID, "SELECT guid, delay, command, datalong, datalong2, dataint, x, y, z, o FROM waypoint_scripts WHERE id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_IP2NATION_COUNTRY, "SELECT c.country FROM ip2nationCountries c, ip2nation i WHERE i.ip < ? AND c.code = i.country ORDER BY i.ip DESC LIMIT 0,1", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_CREATURE_BY_ID, "SELECT guid FROM creature WHERE id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_GAMEOBJECT_NEAREST, "SELECT guid, id, position_x, position_y, position_z, map, (POW(position_x - ?, 2) + POW(position_y - ?, 2) + POW(position_z - ?, 2)) AS order_ FROM gameobject WHERE map = ? AND (POW(position_x - ?, 2) + POW(position_y - ?, 2) + POW(position_z - ?, 2)) <= ? ORDER BY order_", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_CREATURE_NEAREST,   "SELECT guid, id, position_x, position_y, position_z, map, (POW(position_x - ?, 2) + POW(position_y - ?, 2) + POW(position_z - ?, 2)) AS order_ FROM creature WHERE map = ? AND (POW(position_x - ?, 2) + POW(position_y - ?, 2) + POW(position_z - ?, 2)) <= ? ORDER BY order_", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_INS_CREATURE, "INSERT INTO creature (guid, id , map, zoneId, areaId, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, MovementType, npcflag, npcflag2, unit_flags, unit_flags2, unit_flags3, dynamicflags, WorldEffectID, isActive) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_GAME_EVENT_CREATURE, "DELETE FROM game_event_creature WHERE linkedId = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_DEL_GAME_EVENT_MODEL_EQUIP, "DELETE FROM game_event_model_equip WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_GAMEOBJECT, "INSERT INTO gameobject (guid, id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state, isActive) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_INS_DISABLES, "INSERT INTO disables (entry, sourceType, flags, comment) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_DISABLES, "SELECT entry FROM disables WHERE entry = ? AND sourceType = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_DEL_DISABLES, "DELETE FROM disables WHERE entry = ? AND sourceType = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_BLACKMARKET_TEMPLATE, "SELECT id, itemEntry, itemCount, seller, startBid, duration, chance FROM blackmarket_template;", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_MAX_CREATURE_GUID, "SELECT max(guid) FROM creature", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_WORLD_QUEST_TEMPLATE, "SELECT entry, overrideAreaGroupID, areaID, enabled, overrideVariableID, overrideVariableValue FROM world_quest_template ORDER BY entry ASC", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_WORLD_QUEST_LOOT, "SELECT quest_id, group_id, chance FROM world_quest_loot ORDER BY quest_id ASC", CONNECTION_BOTH);
    PREPARE_STATEMENT(WORLD_SEL_WORLD_QUEST_LOOT_GROUP, "SELECT group_id, loot_type, loot_item_id, loot_count_min, loot_count_max, flags FROM world_quest_loot_group ORDER BY group_id ASC", CONNECTION_BOTH);

    PREPARE_STATEMENT(WORLD_DEL_EVENTOBJECT, "DELETE FROM eventobject WHERE guid = ?", CONNECTION_ASYNC);;
    PREPARE_STATEMENT(WORLD_INS_EVENTOBJECT, "INSERT INTO eventobject (guid, id , map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(WORLD_SEL_EVENTOBJECT_NEAREST, "SELECT guid, id, position_x, position_y, position_z, map, (POW(position_x - ?, 2) + POW(position_y - ?, 2) + POW(position_z - ?, 2)) AS order_ FROM eventobject WHERE map = ? AND (POW(position_x - ?, 2) + POW(position_y - ?, 2) + POW(position_z - ?, 2)) <= ? ORDER BY order_", CONNECTION_SYNCH);

    PREPARE_STATEMENT(WORLD_SEL_SPELL_SCRIPTING, "SELECT SpellID, Hook, EffectId, Action, ActionSpellId, TargetSpellId, TargetEffectId, TargetAuraType, ActionCaster, OriginalCaster, ActionTarget, Triggered, AuraState, ExcludeAuraState, TargetAuraState, TargetExcludeAuraState, Calculation, DataSource, DataEffectId, ArtifactTraitId, DataStat, ActionSpellList FROM spell_scripting", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_AURA_SCRIPTS, "SELECT SpellID, Hook, EffectId, Action, ActionSpellId, TargetSpellId, TargetEffectId, TargetAuraType, ActionCaster, OriginalCaster, ActionTarget, Triggered, AuraState, ExcludeAuraState, TargetAuraState, TargetExcludeAuraState, Calculation, DataSource, DataEffectId, ArtifactTraitId, DataStat, ActionSpellList FROM aura_scripts", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_DEL_SPELL_SCRIPTING, "DELETE FROM spell_scripting WHERE State = 0", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_DEL_AURA_SCRIPTS, "DELETE FROM aura_scripts WHERE State = 0", CONNECTION_SYNCH);

    PREPARE_STATEMENT(WORLD_SEL_RAF_REWARDS, "SELECT `Index`, `Name`, Subtitle, ModelDisplayId, `Flags`, ItemId, Quantity FROM recruit_a_friend_rewards", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_SEL_RAF_REWARDS_LOCALE, "SELECT `Index`, Locale, `Name`, Subtitle FROM recruit_a_friend_rewards_locale", CONNECTION_SYNCH);

    PREPARE_STATEMENT(WORLD_UPD_CREATURE_UPPER_LINKED_ID, "UPDATE creature SET linked_id = UPPER(linked_id)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_GAME_EVENT_CREATURE_UPPER_LINKED_ID, "UPDATE game_event_creature SET linkedId = UPPER(linkedId)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_POOL_CREATURE_UPPER_LINKED_ID, "UPDATE pool_creature SET linkedId = UPPER(linkedId)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_ADDON_UPPER_LINKED_ID, "UPDATE creature_addon SET linked_id = UPPER(linked_id)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_CREATURE_FORMATION_UPPER_LINKED_ID, "UPDATE creature_formations SET memberLinkedId = UPPER(memberLinkedId), leaderLinkedId = UPPER(leaderLinkedId)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(WORLD_UPD_SMART_SCRIPT_UPPER_LINKED_ID, "UPDATE smart_scripts SET linked_id = UPPER(linked_id)", CONNECTION_SYNCH);
}
