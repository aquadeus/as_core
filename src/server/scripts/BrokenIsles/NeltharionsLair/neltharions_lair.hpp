////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef NELTHARIONS_LAIR_HPP
# define NELTHARIONS_LAIR_HPP

# include "BrokenIslesPCH.h"
# include "Challenge.h"

static void CastSpellToPlayers(Unit* p_Caster, uint32 p_SpellID, bool p_Triggered)
{
    if (p_Caster == nullptr || !p_Caster->GetMap()->IsMythic())
        return;

    Map::PlayerList const& l_Players = p_Caster->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
        {
            if (p_Caster != nullptr)
                p_Caster->CastSpell(l_Player, p_SpellID, p_Triggered);
            else
                l_Player->CastSpell(l_Player, p_SpellID, p_Triggered);
        }
    }
}

enum eData
{
    DATA_ROKMORA      = 0,
    DATA_ULAROGG      = 1,
    DATA_NARAXAS      = 2,
    DATA_DARGRUL      = 3,
    MAX_ENCOUNTER
};

enum eCreatures
{
    NPC_SPIRITWALKER_EBONHORN   = 100701,
    NPC_SPIRITWALKER_EBONHORN_2 = 113526,
    NPC_NAVAROGG                = 100700,
    NPC_NAVARROGG_QUEST_OBJ     = 97304,
    NPC_COSMETIC_DRUDGE         = 105633,
    NPC_TALKING_DRUDGE          = 105636,
    NPC_DRUDGE_1                = 92350,
    NPC_DRUDGE_2                = 105720,
    NPC_EMBERSHARD_SCORPION     = 98406,
    NPC_STONECLAW_HUNTER        = 91332,
    NPC_DRUMS_OF_WAR            = 92387,

    NPC_COSMETIC_ULAROGG        = 105300,
    NPC_BOSS_ULAROGG            = 91004,
    NPC_ROKMORA                 = 91003,
    NPC_EMPTY_BARREL            = 92473,
    NPC_NAVARROGG_DRUDGE        = 113197,

    NPC_BELLOWING_IDOL          = 98081,
    NPC_BELLOWING_IDOL_2        = 100818,

    /// Naraxas
    NPC_NARAXAS                 = 91005,
    NPC_BLIGHTSHARD_SHAPER      = 105766,
    NPC_WORMSPEAKER_DEVOUT      = 101075,
    NPC_ANGRY_CROWD             = 109137,

    /// Dargul
    NPC_DARGRUL                 = 91007,
    NPC_MOLTEN_CHARSKIN         = 101476,
    NPC_CRYSTAL_WALL_STALKER    = 101593,
    NPC_EMBERHUSK_RIDER         = 102295
};

enum eGameObjects
{
    GO_ROKMORA_DOOR             = 248871,
    GO_CAVE_IN_DOOR             = 254854,
    GO_ULAROGG_DOOR             = 248765,
    GO_NARAXAS_DOOR             = 248764,
    GO_NARAXAS_EXIT_DOOR_1      = 248833,
    GO_NARAXAS_EXIT_DOOR_2      = 248851,
    GO_NARAXAS_COLLISION        = 265579,
    GO_REMAINS_OF_THE_FALLEN    = 251482,
    GO_CRYSTAL_WALL_COLLISION   = 246251
};

enum eNeltharionSpells
{
    SpellEntranceRun    = 209888,
    SpellWormSafeFall   = 205288,
    SpellPermFeignDeath = 159474,
    SpellHunterBasilik  = 202203
};

enum eNeltharionLocations
{
    LocWormDefeatedCheckpoint   = 5738,
    LocMidwayCheckpoint         = 5357,
    LocStoneGiantCheckpoint     = 5737
};

/// Used to teleport players if CheckRequiredBosses is not passed.
Position const g_StartPos = { 2933.07f, 1116.81f, 105.91f, 1.89f };

#endif ///< NELTHARIONS_LAIR_HPP
