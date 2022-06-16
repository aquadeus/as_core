////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "Config.h"

class duel_reset: public PlayerScript
{
    public:
        duel_reset() : PlayerScript("duel_reset") {}

        void OnDuelEnd(Player* p_Winner, Player* p_Looser, DuelCompleteType type)
        {
            /// To be sure that this script will work only on Fun server
            if (sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL) < 110 || !p_Winner || !p_Looser)
                return;

            if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ON_FINISH))
            {
                if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ONLY_IN_ELWYNN_AND_DUROTAR))
                {
                    if (p_Winner->GetZoneId() == 14 || p_Winner->GetZoneId() == 12)
                    {
                        if (type == DUEL_WON)
                        {
                            p_Winner->RemoveArenaSpellCooldowns();
                            p_Looser->RemoveArenaSpellCooldowns();
                            p_Winner->SetHealth(p_Winner->GetMaxHealth());
                            p_Looser->SetHealth(p_Looser->GetMaxHealth());

                            if (p_Winner->getPowerType() == POWER_MANA)
                                p_Winner->SetPower(POWER_MANA, p_Winner->GetMaxPower(POWER_MANA));

                            if (p_Looser->getPowerType() == POWER_MANA)
                                p_Looser->SetPower(POWER_MANA, p_Looser->GetMaxPower(POWER_MANA));
                        }

                        p_Winner->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                    }
                }
                else
                {
                    if (type == DUEL_WON)
                    {
                        p_Winner->RemoveArenaSpellCooldowns();
                        p_Looser->RemoveArenaSpellCooldowns();
                        p_Winner->SetHealth(p_Winner->GetMaxHealth());
                        p_Looser->SetHealth(p_Looser->GetMaxHealth());

                        if (p_Winner->getPowerType() == POWER_MANA)
                            p_Winner->SetPower(POWER_MANA, p_Winner->GetMaxPower(POWER_MANA));

                        if (p_Looser->getPowerType() == POWER_MANA)
                            p_Looser->SetPower(POWER_MANA, p_Looser->GetMaxPower(POWER_MANA));
                    }

                    p_Winner->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                }
            }
        }

        void OnDuelStart(Player* p_Player1, Player* p_Player2)
        {
            /// To be sure that this script will work only on Fun server
            if (sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL) < 110 || !p_Player1 || !p_Player2)
                return;

            if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ON_START))
            {
                if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ONLY_IN_ELWYNN_AND_DUROTAR))
                {
                    if (p_Player1->GetZoneId() == 14 || p_Player1->GetZoneId() == 12)
                    {
                        p_Player1->RemoveArenaSpellCooldowns();
                        p_Player2->RemoveArenaSpellCooldowns();
                        p_Player1->SetHealth(p_Player1->GetMaxHealth());
                        p_Player2->SetHealth(p_Player2->GetMaxHealth());

                        if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_RESET_ENERGY_ON_START))
                        {
                            for (int l_Power = POWER_MANA; l_Power < MAX_POWERS_PER_CLASS; ++l_Power)
                                p_Player1->SetPower((Powers)l_Power, p_Player1->GetDefaultValuePower((Powers)l_Power));

                            for (int l_Power = POWER_MANA; l_Power < MAX_POWERS_PER_CLASS; ++l_Power)
                                p_Player2->SetPower((Powers)l_Power, p_Player2->GetDefaultValuePower((Powers)l_Power));
                        }

                        if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_MAX_ENERGY_ON_START))
                        {
                            for (int l_Power = POWER_MANA; l_Power < MAX_POWERS_PER_CLASS; ++l_Power)
                                p_Player1->SetPower((Powers)l_Power, p_Player1->GetMaxPower((Powers)l_Power));

                            for (int l_Power = POWER_MANA; l_Power < MAX_POWERS_PER_CLASS; ++l_Power)
                                p_Player2->SetPower((Powers)l_Power, p_Player2->GetMaxPower((Powers)l_Power));
                        }
                    }
                }
                else
                {
                    p_Player1->RemoveArenaSpellCooldowns();
                    p_Player2->RemoveArenaSpellCooldowns();
                    p_Player1->SetHealth(p_Player1->GetMaxHealth());
                    p_Player2->SetHealth(p_Player2->GetMaxHealth());

                    if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_MAX_ENERGY_ON_START))
                    {
                        for (int l_Power = POWER_MANA; l_Power < MAX_POWERS_PER_CLASS; ++l_Power)
                            p_Player1->SetPower((Powers)l_Power, p_Player1->GetMaxPower((Powers)l_Power));

                        for (int l_Power = POWER_MANA; l_Power < MAX_POWERS_PER_CLASS; ++l_Power)
                            p_Player2->SetPower((Powers)l_Power, p_Player2->GetMaxPower((Powers)l_Power));
                    }
                }
            }
        }
};

class duel_reset_aura_script: public PlayerScript
{
    public:
        duel_reset_aura_script() : PlayerScript("duel_reset_aura_script") {}

        void OnDuelStart(Player* p_Player1, Player* p_Player2)
        {
            if (!p_Player1 || !p_Player2)
                return;

            if (p_Player1->HasAuraType(SPELL_AURA_RESET_COOLDOWNS_BEFORE_DUEL))
                p_Player1->RemoveArenaSpellCooldowns(true);

            if (p_Player2->HasAuraType(SPELL_AURA_RESET_COOLDOWNS_BEFORE_DUEL))
                p_Player2->RemoveArenaSpellCooldowns(true);
        }
};

#ifndef __clang_analyzer__
void AddSC_DuelReset()
{
    new duel_reset();
    new duel_reset_aura_script();
}
#endif
