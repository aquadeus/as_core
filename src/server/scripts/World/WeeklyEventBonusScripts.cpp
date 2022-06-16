////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptPCH.h"
#include "ScriptMgr.h"

#include "GameEventMgr.h"

#define MAX_WEEKLY_EVENT                    27

class PlayerScript_Weekly_Event_Bonus : public PlayerScript
{
    public:
        PlayerScript_Weekly_Event_Bonus() : PlayerScript("PlayerScript_Weekly_Event_Bonus") {}

        std::array<int32, MAX_WEEKLY_EVENT> m_EventTab =
        {
            {
                599, ///< Pet Battles (Sign Of The Critter)
                600, ///< Pet Battles (Sign Of The Critter)
                601, ///< Pet Battles (Sign Of The Critter)
                602, ///< Battleground (Sign Of Battle)
                603, ///< Battleground (Sign Of Battle)
                604, ///< Battleground (Sign Of Battle)
                605, ///< Legion Dungeon (Sign Of The Warrior)
                606, ///< Legion Dungeon (Sign Of The Warrior)
                607, ///< Legion Dungeon (Sign Of The Warrior)
                610, ///< Skirmishes (Sign Of The Skirmisher)
                611, ///< Skirmishes (Sign Of The Skirmisher)
                612, ///< Skirmishes (Sign Of The Skirmisher)
                613, ///< WorldQuests (Sign Of The Emissary)
                614, ///< WorldQuests (Sign Of The Emissary)
                615, ///< WorldQuests (Sign Of The Emissary)
                616, ///< Timewalking WOTLK
                617, ///< Timewalking WOTLK
                618, ///< Timewalking WOTLK
                622, ///< Timewalking BC
                623, ///< Timewalking BC
                624, ///< Timewalking BC
                628, ///< Timewalking Cataclysm
                629, ///< Timewalking Cataclysm
                630, ///< Timewalking Cataclysm
                652, ///< Timewalking MOP
                654, ///< Timewalking MOP
                656  ///< Timewalking MOP
            }
        };

        std::array<int32, MAX_WEEKLY_EVENT> m_AuraTab = 
        {
            {
                186406, ///< Pet Battles (Sign Of The Critter)
                186406, ///< Pet Battles (Sign Of The Critter)
                186406, ///< Pet Battles (Sign Of The Critter)
                186403, ///< Battleground (Sign Of Battle)
                186403, ///< Battleground (Sign Of Battle)
                186403, ///< Battleground (Sign Of Battle)
                225787, ///< Legion Dungeon (Sign Of The Warrior)
                225787, ///< Legion Dungeon (Sign Of The Warrior)
                225787, ///< Legion Dungeon (Sign Of The Warrior)
                186401, ///< Skirmishes (Sign Of The Skirmisher)
                186401, ///< Skirmishes (Sign Of The Skirmisher)
                186401, ///< Skirmishes (Sign Of The Skirmisher)
                225788, ///< WorldQuests (Sign Of The Emissary)
                225788, ///< WorldQuests (Sign Of The Emissary)
                225788, ///< WorldQuests (Sign Of The Emissary)
                196970, ///< Timewalking WOTLK
                196970, ///< Timewalking WOTLK
                196970, ///< Timewalking WOTLK
                196969, ///< Timewalking BC
                196969, ///< Timewalking BC
                196969, ///< Timewalking BC
                201001, ///< Timewalking Cataclysm
                201001, ///< Timewalking Cataclysm
                201001, ///< Timewalking Cataclysm
                233840, ///< Timewalking MOP
                233840, ///< Timewalking MOP
                233840  ///< Timewalking MOP
            }
        };

        void OnLogin(Player* p_Player)
        {
            for (uint8 i = 0; i < MAX_WEEKLY_EVENT; ++i)
            {
                /// There is 3 timewalking event (Northrend Timewalking Dungeon Event, Outland Timewalking Dungeon Event, Cataclysm Timewalking Dungeon Event)
                /// Devide on two occurence event (95, 96), there is no spell on timewalking event
                if (sGameEventMgr->IsActiveEvent(m_EventTab[i]))
                {
                    if (m_AuraTab[i])
                    {
                        p_Player->AddAura(m_AuraTab[i], p_Player);
                        break;
                    }
                }
            }
        }

        void OnEnterBG(Player* p_Player, uint32 /*p_MapID*/)
        {
            for (uint8 i = 0; i < MAX_WEEKLY_EVENT; ++i)
            {
                /// There is 3 timewalking event (Northrend Timewalking Dungeon Event, Outland Timewalking Dungeon Event, Cataclysm Timewalking Dungeon Event)
                /// Devide on two occurence event (95, 96), there is no spell on timewalking event
                if (sGameEventMgr->IsActiveEvent(m_EventTab[i]))
                {
                    if (m_AuraTab[i])
                    {
                        p_Player->AddAura(m_AuraTab[i], p_Player);
                        break;
                    }
                }
            }
        }

        void OnLogout(Player* p_Player)
        {
            for (uint8 i = 0; i < MAX_WEEKLY_EVENT - 2; ++i)
                p_Player->RemoveAura(m_AuraTab[i]);
        }

        void OnleaveBG(Player* p_Player, uint32 /*p_MapID*/)
        {
            for (uint8 i = 0; i < MAX_WEEKLY_EVENT - 2; ++i)
                p_Player->RemoveAura(m_AuraTab[i]);
        }
};

#ifndef __clang_analyzer__
void AddSC_WeeklyEventBonusScripts()
{
    new PlayerScript_Weekly_Event_Bonus();
};
#endif
