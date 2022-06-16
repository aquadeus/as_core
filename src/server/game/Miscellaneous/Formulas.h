////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_FORMULAS_H
#define TRINITY_FORMULAS_H

#include "World.h"
#include "SharedDefines.h"
#include "ScriptMgr.h"
#include "GameTables.h"

namespace JadeCore
{
    inline uint32 GetExpansionForLevel(uint32 level)
    {
        if (level < 60)
            return EXPANSION_VANILLA;
        else if (level < 70)
            return EXPANSION_THE_BURNING_CRUSADE;
        else if (level < 80)
            return EXPANSION_WRATH_OF_THE_LICH_KING;
        else if (level < 85)
            return EXPANSION_CATACLYSM;
        else if (level < 90)
            return EXPANSION_MISTS_OF_PANDARIA;
        else if (level < 100)
            return EXPANSION_WARLORDS_OF_DRAENOR;
        else if (level < 110)
            return EXPANSION_LEGION;
        else
            return EXPANSION_LEGION;
    }

    namespace Honor
    {
        inline float hk_honor_at_level_f(uint8 level, float multiplier = 1.0f)
        {
            float honor = multiplier * level * 0.01705f;    ///< Rand number -> TODO calculate me somehow
            sScriptMgr->OnHonorCalculation(honor, level, multiplier);
            return honor;
        }

        inline uint32 hk_honor_at_level(uint8 level, float multiplier = 1.0f)
        {
            return uint32(ceil(hk_honor_at_level_f(level, multiplier)));
        }
    }
    namespace XP
    {
        inline uint8 GetGrayLevel(uint8 pl_level)
        {
            uint8 level;

            if (pl_level < 7)
                level = 0;
            else if (pl_level < 35)
            {
                uint8 count = 0;

                for (int i = 15; i <= pl_level; ++i)
                    if (i % 5 == 0) ++count;

                level = (pl_level - 7) - (count - 1);
            }
            else
                level = pl_level - 10;

            sScriptMgr->OnGrayLevelCalculation(level, pl_level);
            return level;
        }

        inline XPColorChar GetColorCode(uint8 pl_level, uint8 mob_level)
        {
            XPColorChar color;

            if (mob_level >= pl_level + 5)
                color = XP_RED;
            else if (mob_level >= pl_level + 3)
                color = XP_ORANGE;
            else if (mob_level >= pl_level - 2)
                color = XP_YELLOW;
            else if (mob_level > GetGrayLevel(pl_level))
                color = XP_GREEN;
            else
                color = XP_GRAY;

            sScriptMgr->OnColorCodeCalculation(color, pl_level, mob_level);
            return color;
        }

        inline uint8 GetZeroDifference(uint8 pl_level)
        {
            uint8 diff;

            if (pl_level < 4)
                diff = 5;
            else if (pl_level < 10)
                diff = 6;
            else if (pl_level < 12)
                diff = 7;
            else if (pl_level < 16)
                diff = 8;
            else if (pl_level < 20)
                diff = 9;
            else if (pl_level < 30)
                diff = 11;
            else if (pl_level < 40)
                diff = 12;
            else if (pl_level < 45)
                diff = 13;
            else if (pl_level < 50)
                diff = 14;
            else if (pl_level < 55)
                diff = 15;
            else if (pl_level < 60)
                diff = 16;
            else
                diff = 17;

            sScriptMgr->OnZeroDifferenceCalculation(diff, pl_level);
            return diff;
        }

        inline uint32 BaseGain(uint8 pl_level, uint8 mob_level)
        {
            uint32 baseGain;

            XPTableEntry const* xpPlayer = g_XPTable.LookupEntry(pl_level);
            XPTableEntry const* xpMob = g_XPTable.LookupEntry(mob_level);

            int32 l_LevelDiff = (int32)mob_level - (int32)pl_level;

            if (l_LevelDiff > 6 || !xpPlayer || !xpMob)
            {
                baseGain = 0;
                return baseGain;
            }

            if (mob_level >= pl_level)
                baseGain = uint32(round(xpPlayer->m_PerKill * (1 + 0.05f * std::min(mob_level - pl_level, 4))));
            else
            {
                if (mob_level > GetGrayLevel(pl_level))
                    baseGain = uint32(round(xpMob->m_PerKill * ((1 - ((pl_level - mob_level) / float(GetZeroDifference(pl_level)))) * (xpMob->m_Dividor / xpPlayer->m_Dividor))));
                else
                    baseGain = 0;
            }

            sScriptMgr->OnBaseGainCalculation(baseGain, pl_level, mob_level);
            return baseGain;
        }

        inline uint32 Gain(Player* player, Unit* u)
        {
            Creature* creature = u->ToCreature();
            uint32 gain;

            if ((u->GetTypeId() == TYPEID_UNIT && (creature->isTotem() || creature->isPet() ||
                creature->GetFlagsExtra() & CREATURE_FLAG_EXTRA_NO_XP_AT_KILL)) ||
                creature->GetCreatureTemplate()->type == CREATURE_TYPE_CRITTER)
                gain = 0;
            else
            {
                gain = BaseGain(player->getLevel(), u->getLevelForTarget(player));

                if (gain != 0 && u->GetTypeId() == TYPEID_UNIT && creature->isElite())
                {
                    // Players get only 10% xp for killing creatures of lower expansion levels than himself
                    if ((uint32(creature->GetCreatureTemplate()->RequiredExpansion) < GetExpansionForLevel(player->getLevel())))
                        gain = uint32(round(gain / 10.0f));

                    // Elites in instances have a 2.75x XP bonus instead of the regular 2x world bonus.
                    if (u->GetMap() && u->GetMap()->IsDungeon())
                        gain = uint32(gain * 2.75f);
                    else
                        gain *= 2;
                }

                float KillXpRate = 1.0f;
                KillXpRate = sWorld->getRate(RATE_XP_KILL);
                gain = uint32(gain * KillXpRate);
            }

            sScriptMgr->OnGainCalculation(gain, player, u);
            return gain;
        }

        inline float xp_in_group_rate(uint32 count, bool isRaid)
        {
            float rate;

            if (isRaid)
            {
                // FIXME: Must apply decrease modifiers depending on raid size.
                rate = 1.0f;
            }
            else
            {
                switch (count)
                {
                    case 0:
                    case 1:
                    case 2:
                        rate = 1.0f;
                        break;
                    case 3:
                        rate = 1.166f;
                        break;
                    case 4:
                        rate = 1.3f;
                        break;
                    case 5:
                    default:
                        rate = 1.4f;
                }
            }

            sScriptMgr->OnGroupRateCalculation(rate, count, isRaid);
            return rate;
        }
    }

    namespace Currency
    {
        inline uint32 ConquestRatingCalculator(uint32 rate)
        {
            if (rate <= 1500)
                return 1800; // Default conquest points
            else if (rate > 3000)
                rate = 3600;

            // http://www.arenajunkies.com/topic/179536-conquest-point-cap-vs-personal-rating-chart/page__st__60#entry3085246
            return uint32(1.4326 * ((1511.26 / (1 + 1639.28 / exp(0.00412 * rate))) + 1050.15));
        }

        inline uint32 BgConquestRatingCalculator(uint32 rate)
        {
            // WowWiki: Battleground ratings receive a bonus of 22.2% to the cap they generate
            return uint32((ConquestRatingCalculator(rate) * 1.222f) + 0.5f);
        }
    }
}

#endif
