////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef EYE_OF_AZSHARA_HPP
# define EYE_OF_AZSHARA_HPP

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
    DATA_PARJESH                = 0,
    DATA_HATECOIL               = 1,
    DATA_DEEPBEARD              = 2,
    DATA_SERPENTRIX             = 3,
    DATA_WRATH_OF_AZSHARA       = 4,
    MAX_ENCOUNTER               = 5,

    INVISIBLE_CREATURE_MODEL    = 11686
};

enum eCreatures
{
    /// Parjesh
    NPC_HATECOIL_SHELLBREAKER   = 97264,
    NPC_HATECOIL_CRESTRIDER     = 97269,

    /// Hatecoil
    NPC_SAND_DUNE               = 97853,
    NPC_MONSOON                 = 99852,
    NPC_LADY_HATECOIL           = 91789,
    NPC_SALTSEA_GLOBULE         = 98293,

    /// Deepbeard
    NPC_QUAKE                   = 97916,
    NPC_SKROG_WAVECRASHER       = 91796,

    /// Serpentrix
    NPC_BLAZING_HYDRA_SPAWN     = 97259,
    NPC_ARCANE_HYDRA_SPAWN      = 97260,

    /// Wrath of Azshara
    NPC_WRATH_OF_AZSHARA        = 96028,
    NPC_WEATHERMAN              = 97063,
    NPC_LIGHTNING_STALKER       = 97713,
    NPC_MYSTIC_SSAVEH           = 98173,
    NPC_RITUALIST_LESHA         = 100248,
    NPC_CHANNELER_VARISZ        = 100249,
    NPC_BINDER_ASHIOI           = 100250,
    NPC_ARCANE_BOMB             = 97691,
    NPC_TIDAL_WAVE              = 97739
};

enum eGameObjects
{
    GO_SAND_DUNE                = 244690,
    GO_BUBBLE                   = 240788
};

enum eEoASpells
{
    SpellShelter            = 197134,
    SpellViolentWinds       = 191797,
    SpellLightningStrikes   = 192796,
    SpellEyeOfAzshara       = 199944
};

enum eEoAAchievements
{
    ButYouSayHesJustAFriend = 10456,
    StaySalty               = 10457
};

#endif ///< EYE_OF_AZSHARA_HPP
