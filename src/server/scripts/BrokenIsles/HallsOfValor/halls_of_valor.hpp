////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef HALLS_OF_VALOR_HPP
# define HALLS_OF_VALOR_HPP

# include "BrokenIslesPCH.h"
# include "Challenge.h"

enum eData
{
    DATA_HYMDALL    = 0,
    DATA_HYRJA      = 1,
    DATA_FENRYR     = 2,
    DATA_SKOVALD    = 3,
    DATA_ODYN       = 4,
    MAX_ENCOUNTER,

    DATA_HYRJA_EVENT_COMPLETE,
    DATA_HYRJA_EXPEL_LIGHT_STACKS,
    DATA_FENRYR_EVENT,
    DATA_RUNES_ACTIVATED,
    DATA_RUNES_DEACTIVATED,
    DATA_SKOVALD_EVENT,
    DATA_MUG_OF_MEAD_ACHIEVEMENT,
    DATA_SURGE_PROTECTOR
};

enum eCreatures
{
    /// Hymdall summons
    BOSS_HYMDALL                    = 94960,
    NPC_STORM_DRAKE                 = 97788,
    NPC_STATIC_FIELD                = 97795,
    NPC_DANCING_BLADE               = 97960,

    /// Hyrja
    NPC_HYRJA                       = 95833,
    NPC_OLMYR_THE_ENLIGHTENED       = 97202,
    NPC_SOLSTEN                     = 97219,

    /// Fenryr
    NPC_BOSS_FENRYR                 = 99868,
    NPC_FENRYR                      = 95674,
    NPC_FENRYR_TAMEABLE             = 119990,

    /// Skovald
    NPC_GOD_KING_SKOVALD            = 95675,
    NPC_KING_HALDOR                 = 95843,
    NPC_KING_BJORN                  = 97081,
    NPC_KING_RANULF                 = 97083,
    NPC_KING_TOR                    = 97084,
    NPC_AEGIS_OF_AGGRAMAR           = 98364,

    /// Odyn
    NPC_ODYN                        = 95676,
    NPC_SPEAR_OF_LIGHT              = 100575,
    NPC_SPOILS_CHEST_VISUAL         = 101459,
    NPC_CHOSEN_OF_EYIR              = 101638,
    NPC_HEART_OF_ZIN_AZSHARI        = 100877,
    NPC_STORMFORGED_OBLITERATOR     = 102019,

    /// Trashes
    NPC_VALARJAR_ASPIRANT           = 101637,
    NPC_VALARJAR_SHIELDMAIDEN       = 101639,
    NPC_FIELD_OF_THE_ETERNAL_HUNT   = 101697,
    NPC_HALL_OF_VALOR_TELEPORTER    = 101712,
    NPC_STORM_DRAKE_ACHIEVEMENT     = 97068,
    NPC_VALARJAR_STORMRIDER         = 94968,
    NPC_VALARJAR_CHAMPION           = 97087,
    NPC_VALARJAR_RUNECARVER         = 96664,
    NPC_VALARJAR_MYSTIC             = 95834,
    NPC_VALARJAR_THUNDERCALLER      = 95842,
    NPC_TRAINED_HAWK                = 99828,
    NPC_GILDEFUR_STAG               = 96609
};

enum eGameObjects
{
    GO_HYMDALL_ENTER_DOOR   = 245700,
    GO_HYMDALL_EXIT_DOOR    = 245701,
    GO_HYMDALL_CACHE        = 258968,
    GO_HYRJA_DOOR           = 245702,
    GO_TRACES_OF_FENRIR     = 246272,
    GO_GATES_OF_GLORY_DOOR  = 244664,
    GO_BRIDGE_OF_GLORY      = 246144,
    GO_ODYN_CHEST           = 245847,
    GO_ODYN_BALCONY         = 245884,

    GO_RUNIC_BRAND_PURE     = 245698,
    GO_RUNIC_BRAND_RED      = 245697,
    GO_RUNIC_BRAND_YELLOW   = 245695,
    GO_RUNIC_BRAND_BLUE     = 245696,
    GO_RUNIC_BRAND_GREEN    = 245699,

    GO_VALHALLA_BRIDGE      = 246144,
    GO_AEGIS_OF_AGGRAMAR    = 251991
};

enum eSharedSpells
{
    SPELL_ODYNS_BLESSING    = 202160,
    SPELL_THREE_BOSS_DEFEAT = 202159,
    SPELL_BIFROST_AURA      = 192635,

    /// Achievement
    SPELL_MUG_OF_MEAD       = 202298,
    SPELL_KILLING_BLOW      = 207753,
};

enum eAchievements
{
    I_GO_WHAT_YOU_MEAD  = 10542,
    STAG_PARTY          = 10544,
    SURGE_PROTECTOR     = 10543
};

enum eQuests
{
    QUEST_ODYNS_BLESSING    = 40615
};

Position const g_OdynsSpoilPos = { 2429.50f, 528.97f, 749.08f, 0.0f };
Position const g_FenryrTameablePos = { 2872.63f, 3242.90f, 584.81f, 0.0f };

#endif ///< HALLS_OF_VALOR_HPP
