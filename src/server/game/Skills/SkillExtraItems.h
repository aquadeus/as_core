////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_SKILL_EXTRA_ITEMS_H
#define TRINITY_SKILL_EXTRA_ITEMS_H

#include "Common.h"

// predef classes used in functions
class Player;
// returns true and sets the appropriate info if the player can create extra items with the given spellId
bool canCreateExtraItems(Player* player, uint32 spellId, float &additionalChance, uint8 &additionalMax);
// function to load the extra item creation info from DB
void LoadSkillExtraItemTable();

bool CanCreateRank3ExtraItems(Player* p_Player, uint32 p_SpellId, uint8 &p_AdditionalCount);

/// Info: https://www.reddit.com/r/woweconomy/comments/6078nz/rank_3_flasks_5000_casts_stats_inside/
constexpr int32 m_AllChances = 1000;
constexpr std::array<int, 10> m_Rank3Chance =
{
    {
        6930,
        2350,
        430,
        130,
        60,
        40,
        20,
        17,
        13,
        10
    }
};

constexpr std::array<int, 24> m_AlchemyRank3Spells =
{
    {
        188300, ///< Ancient Healing Potion
        188303, ///< Ancient Mana Potion
        188306, ///< Ancient Rejuvenation Potion
        188309, ///< Draught of Raw Magic
        188312, ///< Sylvan Eelixir
        188315, ///< Avalanche Elixir
        188318, ///< Skaggldrynk
        188321, ///< Skystep Potion
        188324, ///< Infernal Alchemist Stone
        188327, ///< Potion of Deadly Grace
        188330, ///< Potion of the Old War
        188333, ///< Unbending Potion
        188336, ///< Leytorrent Potion
        188339, ///< Flask of The Whispered Pact
        188342, ///< Flask of The Seventh Demon
        188345, ///< Flask of The Countless Armies
        188348, ///< Flask of Ten Thousand Scars
        188351, ///< Spirit Cauldron
        188802, ///< Wild Transmutation
        229220, ///< Potion of Prolonged Power
        247622, ///< Lightblood Elixir
        247691, ///< Tears of the Naaru
        247696, ///< Astral Alchemist Stone
        251658  ///< Astral Healing Potion
    }
};

#endif
