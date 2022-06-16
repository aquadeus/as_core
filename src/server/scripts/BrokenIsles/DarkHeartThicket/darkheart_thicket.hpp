////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef DARKHEART_THICKET_HPP
# define DARKHEART_THICKET_HPP

# include "BrokenIslesPCH.h"

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
    DATA_GLAIDALIS         = 0,
    DATA_OAKHEART          = 1,
    DATA_DRESARON          = 2,
    DATA_XAVIUS            = 3,
    MAX_ENCOUNTER
};

enum eCreatures
{
    NPC_DREADSOUL_RUINER        = 95771,

    /// Xavius summons
    NPC_NIGHTMARE_BINDINGS      = 101329, ///< Malfurion cage
    NPC_MALFURION_STORMRAGE     = 100652,

    /// Pre-event Glaydalis
    NPC_DRUIDIC_PRESERVER       = 100403,

    /// Oakheart
    NPC_OAKHEART                = 103344,
    NPC_NIGHTMARE_DWELLER       = 101991,

    /// Dresaron summons
    NPC_CORRUPTED_DRAGON_EGG    = 101072,
    NPC_HATESPAWN_WHELPLING     = 101074,
    NPC_BREATH_OF_CORRUPTION    = 101078,
};

enum eGameObjects
{
    GO_GLAIDALIS_FIRE_DOOR      = 246910,
    GO_GLAIDALIS_INVIS_DOOR     = 246905,
    GO_DRESARON_FIRE_DOOR       = 247039
};

enum eGlobalSpells
{
    SPELL_ENTRANCE_CONVERSATION = 202867
};

enum eAchievements
{
    BurningDownTheHouse = 10769,
};

/// Used to teleport players if CheckRequiredBosses is not passed.
Position const g_StartPos = { 3241.77f, 1828.51f, 236.48f, 3.12f };

#endif ///< DARKHEART_THICKET_HPP
