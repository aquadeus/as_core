////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEF_FIRELANDS_H
#define DEF_FIRELANDS_H

#include "KalimdorPCH.h"
#include "AccountMgr.h"

#define FLScriptName "instance_firelands"

const Position FLEntrancePos = {-547.313f, 318.42f, 115.473f, 5.91667f}; // Firelands Entrance

enum Data
{
    DATA_SHANNOX                = 0,
    DATA_RHYOLITH               = 1,
    DATA_BETHTILAC              = 2,
    DATA_ALYSRAZOR              = 3,
    DATA_BALEROC                = 4,
    DATA_STAGHELM               = 5,
    DATA_RAGNAROS               = 6,
    DATA_RIPLIMB                = 7,
    DATA_RAGEFACE               = 8,
    DATA_RHYOLITH_HEALTH_SHARED = 9,
    DATA_EVENT                  = 10,
    DATA_RAGNAROS_FLOOR         = 11,
    DATA_RAGNAROS_CACHE_10      = 12,
    DATA_RAGNAROS_CACHE_25      = 13,
    DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_BRUSHFIRE         = 14,
    DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_LAVA_SPEW         = 15,
    DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_INCENDIARY_CLOUD  = 16,
    DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_FIERY_TORNADO     = 17,
};

enum CreatureIds
{
    NPC_SHANNOX                     = 53691,
    NPC_RAGEFACE                    = 53695,
    NPC_RIPLIMB                     = 53694,
    NPC_RHYOLITH                    = 52558,
    NPC_BETHTILAC                   = 52498,
    NPC_ALYSRAZOR                   = 52530,
    NPC_BALEROC                     = 53494,
    NPC_STAGHELM                    = 52571,
    NPC_RAGNAROS                    = 52409,
    NPC_VOLCANUS                    = 53833,

    // alysrazor event
    NPC_BLAZING_MONSTROSITY_LEFT    = 53786,
    NPC_BLAZING_MONSTROSITY_RIGHT   = 53791,
    NPC_EGG_PILE                    = 53795,
    NPC_HARBINGER_OF_FLAME          = 53793,
    NPC_MOLTEN_EGG_TRASH            = 53914,
    NPC_SMOULDERING_HATCHLING       = 53794,

    NPC_CIRCLE_OF_THRONES_PORTAL    = 54247,
};

enum GameobjectIds
{
    GO_CIRCLE_OF_THORNS_PORTAL1 = 209137,
    GO_CIRCLE_OF_THORNS_PORTAL2 = 209346,
    GO_CIRCLE_OF_THORNS_PORTAL3 = 209098,

    GO_BRIDGE_OF_RHYOLITH       = 209255,
    GO_FIRE_WALL_BALEROC        = 209066,
    GO_RAID_BRIDGE_FORMING      = 209277,
    GO_RAGNAROS_FLOOR           = 208835,
    GO_STICKY_WEB               = 208877,
    GO_MOLTEN_METEOR            = 208966,
    GO_FIRE_WALL_FANDRAL_1      = 208906,
    GO_FIRE_WALL_FANDRAL_2      = 208873,
    GO_SULFURON_KEEP            = 209073,
    GO_CACHE_OF_THE_FIRELORD_10 = 208967,
    GO_CACHE_OF_THE_FIRELORD_25 = 209261,
};

enum QuestDefines
{
    // quest
    GO_BRANCH_OF_NORDRASSIL                     = 209100,

    // Cannot find rhyolith fragment, summon it manualy
    SPELL_CREATE_EMBERSTONE_FRAGMENT            = 100518,
    SPELL_CREATE_CHITINOUS_FRAGMENT             = 100520,
    SPELL_CREATE_PYRESHELL_FRAGMENT             = 100519,

    SPELL_CHARGED_RHYOLITH_FOCUS                = 100481,
    SPELL_CHARGED_EMBERSTONE_FOCUS              = 100499,
    SPELL_CHARGED_CHITINOUS_FOCUS               = 100501,
    SPELL_CHARGED_PYRESHELL_FOCUS               = 100500,

    SPELL_TRANSFORM_CHARGED_RHYOLITH_FOCUS      = 100477,
    SPELL_TRANSFORM_CHARGED_EMBERSTONE_FOCUS    = 100496,
    SPELL_TRANSFORM_CHARGED_CHITINOUS_FOCUS     = 100498,
    SPELL_TRANSFORM_CHARGED_PYRESHELL_FOCUS     = 100497,

    GO_RHYOLITH_FRAGMENT                        = 209033,
    GO_EMBERSTONE_FRAGMENT                      = 209035,
    GO_PYRESHELL_FRAGMENT                       = 209036,
    GO_OBSIDIAN_FLECKED_CHITIN                  = 209037,

    NPC_DULL_RHYOLITH_FOCUS                     = 53951,
    NPC_DULL_EMBERSTONE_FOCUS                   = 53968,
    NPC_DULL_CHITINOUS_FOCUS                    = 53970,
    NPC_DULL_PYRESHELL_FOCUS                    = 53963,

    NPC_CHARGED_RHYOLITH_FOCUS                  = 53955,
    NPC_CHARGED_EMBERSTONE_FOCUS                = 53969,
    NPC_CHARGED_CHITINOUS_FOCUS                 = 53971,
    NPC_CHARGED_PYRESHELL_FOCUS                 = 53967,

    EVENT_PORTALS                               = 28888,

    ITEM_CHARGED_RHYOLITE_FOCUS                 = 71008,
    ITEM_CHARGED_CHITINOUS_FOCUS                = 71017,
    ITEM_CHARGED_EMBERSTONE_FOCUS               = 71015,
    ITEM_CHARGED_PYRESHELL_FOCUS                = 71016,

    SPELL_LEGENDARY_PORTAL_OPENING              = 101029,
    SPELL_BRANCH_OF_NORDRASSIL_WIN_COSMETIC     = 100326,
    SPELL_SMOLDERING_AURA                       = 101093,
    SPELL_SIPHON_ESSENCE_CREDIT                 = 101149,
    SPELL_HEART_OF_RAGNAROS_CREATE              = 101125,

    QUEST_HEART_OF_FLAME_ALLIANCE               = 29307,
    QUEST_HEART_OF_FLAME_HORDE                  = 29308,
    QUEST_OBJECTIVE_HEART_OF_FLAME_ALLIANCE     = 252978,
    QUEST_OBJECTIVE_HEART_OF_FLAME_HORDE        = 253208
};

enum AchievementCriterias
{
    CRITERIA_DO_A_BARREL_ROLL_BRUSHFIRE           = 17533,
    CRITERIA_DO_A_BARREL_ROLL_LAVA_SPEW           = 17535,
    CRITERIA_DO_A_BARREL_ROLL_INCENDIARY_CLOUD    = 17536,
    CRITERIA_DO_A_BARREL_ROLL_FIERY_TORNADO       = 17538
};

static void AddSmoulderingAura(Creature* pCreature)
{
    Map::PlayerList const &PlayerList = pCreature->GetMap()->GetPlayers();
    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* pPlayer = i->getSource())
                if (pPlayer->GetQuestStatus(QUEST_HEART_OF_FLAME_ALLIANCE) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_HEART_OF_FLAME_HORDE) == QUEST_STATUS_INCOMPLETE)
                {
                    pCreature->CastSpell(pCreature, SPELL_SMOLDERING_AURA, true);
                    break;
                }
}

enum eEncounterIDs
{
    Bethtilac   = 1197,
    Rhyolith    = 1204,
    Shannox     = 1205,
    Alysrazor   = 1206,
    Baleroc     = 1200,
    Majordomo   = 1185,
    Ragnaros    = 1203
};

#endif
