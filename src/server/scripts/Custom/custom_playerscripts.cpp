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
#include "../Server/Packets/AchievementPackets.h"
#include "MapManager.h"
#include "GameTables.h"
#include "ChannelMgr.h"
#include "GarrisonMgr.hpp"

class playerscript_mountacular_achievement : public PlayerScript
{
    public:
        playerscript_mountacular_achievement() : PlayerScript("playerscript_mountacular_achievement") { }

        enum eAchievements
        {
            MountacularAlliance     = 9598,
            MountacularHorde        = 9599
        };

        enum eItemRewards
        {
            FelFireHawk             = 69226
        };

        enum eSpells
        {
            FelFireHawkSpell        = 97501
        };

        void OnAchievementEarned(Player* p_Player, AchievementEntry const* p_Achievement, bool& /*p_SendAchievement*/, bool /*p_After*/) override
        {
            if (!p_Player || !p_Achievement)
                return;

            if (p_Achievement == sAchievementStore.LookupEntry(eAchievements::MountacularAlliance) || p_Achievement == sAchievementStore.LookupEntry(eAchievements::MountacularHorde))
            {
                if (!p_Player->HasSpell(eSpells::FelFireHawkSpell) && !p_Player->HasItemCount(eItemRewards::FelFireHawk, 1, true, false))
                    p_Player->AddItem(eItemRewards::FelFireHawk, 1);
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            if (p_Player->HasAchieved(eAchievements::MountacularAlliance) || p_Player->HasAchieved(eAchievements::MountacularHorde))
            {
                if (!p_Player->HasSpell(eSpells::FelFireHawkSpell) && !p_Player->HasItemCount(eItemRewards::FelFireHawk, 1, true, false))
                    p_Player->AddItem(eItemRewards::FelFireHawk, 1);
            }
        }
};

class playerscript_broken_isles_strike_point_quests : public PlayerScript
{
    public:
        playerscript_broken_isles_strike_point_quests()  : PlayerScript("playerscript_broken_isles_strike_point_quests") { }

        enum eZoneQuests
        {
            AzsunaQuest             = 39718,
            ValsharahQuest          = 39731,
            HighmountainQuest       = 39733,
            StormheimHordeQuest     = 39864,
            StormheimAlianceQuest   = 39735
        };

        enum eClassQuests
        {
            Mage                = 41141,
            Paladin             = 39756,
            Priest              = 41019,
            Shaman              = 41510,
            DemonHunter1        = 41099,
            DemonHunter2        = 41069,
            Rogue               = 40997,
            Druid               = 40653,
            DeathKnight         = 39799,
            Hunter              = 40959,
            Monk                = 40795,
            Warlock             = 40824,
            Warrior             = 40585
        };

        enum eQuestCreatures
        {
            ZoneKillCredit      = 97067
        };

        std::array<uint32, 13> l_ClassHallQuests = { { eClassQuests::Mage, eClassQuests::Paladin, eClassQuests::Priest, eClassQuests::Shaman, eClassQuests::DemonHunter1, eClassQuests::DemonHunter2, eClassQuests::Rogue, eClassQuests::Druid, eClassQuests::DeathKnight, eClassQuests::Hunter, eClassQuests::Monk, eClassQuests::Warlock, eClassQuests::Warrior } };

        void OnQuestAccept(Player* p_Player, const Quest* p_Quest)
        {
            if (p_Player == nullptr)
                return;

            if (p_Quest->GetQuestId() == eZoneQuests::AzsunaQuest ||
                p_Quest->GetQuestId() == eZoneQuests::ValsharahQuest ||
                p_Quest->GetQuestId() == eZoneQuests::HighmountainQuest || 
                p_Quest->GetQuestId() == eZoneQuests::StormheimHordeQuest ||
                p_Quest->GetQuestId() == eZoneQuests::StormheimAlianceQuest)
            {
                for (uint32 l_Quest : l_ClassHallQuests)
                {
                    if (p_Player->GetQuestStatus(l_Quest) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (CreatureTemplate const* l_CreatureInfo = sObjectMgr->GetCreatureTemplate(eQuestCreatures::ZoneKillCredit))
                            p_Player->KilledMonster(l_CreatureInfo, 0);

                        p_Player->CompleteQuest(l_Quest);
                    }
                }
            }
        }
};

class PlayerScript_The_Molten_Front_Offensive_Achievement : public PlayerScript
{
    public:
        PlayerScript_The_Molten_Front_Offensive_Achievement() : PlayerScript("PlayerScript_The_Molten_Front_Offensive_Achievement") { }

        enum eAchievements
        {
            Achievement_The_Molten_Front_Offensive = 5866
        };

        enum eSpells
        {
            Spell_Flameward_Hippogryph = 97359
        };

        enum eItems
        {
            Item_Flameward_Hippogryph = 69213
        };

        void OnAchievementEarned(Player* p_Player, AchievementEntry const* p_Achievement, bool& /*p_SendAchievement*/, bool /*p_After*/) override
        {
            if (!p_Player || !p_Achievement)
                return;

            if (p_Achievement == sAchievementStore.LookupEntry(eAchievements::Achievement_The_Molten_Front_Offensive) && !p_Player->HasSpell(eSpells::Spell_Flameward_Hippogryph) && !p_Player->HasItemCount(eItems::Item_Flameward_Hippogryph, 1, true, false))
                p_Player->AddItem(eItems::Item_Flameward_Hippogryph, 1);
        }

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            if (p_Player->HasAchieved(eAchievements::Achievement_The_Molten_Front_Offensive) && !p_Player->HasSpell(eSpells::Spell_Flameward_Hippogryph) && !p_Player->HasItemCount(eItems::Item_Flameward_Hippogryph, 1, true, false))
                p_Player->AddItem(eItems::Item_Flameward_Hippogryph, 1);
        }
};

/// Called for Menagerie - 5877
/// Last Update 7.3.5 Build 26365
class PlayerScript_menagerie_achievement : public PlayerScript
{
    public:
        PlayerScript_menagerie_achievement() : PlayerScript("PlayerScript_menagerie_achievement") { }

        enum eAchievements
        {
            Menagerie = 5877
        };

        enum eSpells
        {
            BrilliantKalliri = 101424
        };

        enum eItems
        {
            ItemBrilliantKaliri = 71387
        };

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            if (p_Player->HasAchieved(eAchievements::Menagerie) && !p_Player->HasSpell(eSpells::BrilliantKalliri) && !p_Player->HasItemCount(eItems::ItemBrilliantKaliri, 1, true, true, true))
                p_Player->learnSpell(eSpells::BrilliantKalliri, false);
        }
};

/// Called for Heirloom Hoarder - 9909
/// Last Update 7.3.5 Build 26365
class PlayerScript_heirloom_hoarder_achievement : public PlayerScript
{
    public:
        PlayerScript_heirloom_hoarder_achievement() : PlayerScript("PlayerScript_heirloom_hoarder_achievement") { }

        enum eAchievements
        {
            HeirloomHoarder = 9909
        };

        enum eSpells
        {
            ChauffeurA = 179245,
            ChauffeurH = 179244
        };

        enum eItems
        {
            ItemChauffeuredChopperA = 122703,
            ItemChauffeuredChopperH = 120968
        };

        void OnAchievementEarned(Player* p_Player, AchievementEntry const* p_Achievement, bool& /*p_SendAchievement*/, bool /*p_After*/) override
        {
            if (p_Player == nullptr || p_Achievement == nullptr)
                return;

            if (p_Achievement->ID == eAchievements::HeirloomHoarder)
            {
                CheckAndAddRewards(p_Player);
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (p_Player == nullptr)
                return;

            if (p_Player->HasAchieved(eAchievements::HeirloomHoarder))
            {
                CheckAndAddRewards(p_Player);
            }
        }

    private:

        void CheckAndAddRewards(Player* p_Player)
        {
            if (!p_Player->HasSpell(eSpells::ChauffeurA) && !p_Player->HasItemCount(eItems::ItemChauffeuredChopperA, 1, true, true, true))
                p_Player->learnSpell(eSpells::ChauffeurA, false);

            if (!p_Player->HasSpell(eSpells::ChauffeurH) && !p_Player->HasItemCount(eItems::ItemChauffeuredChopperH, 1, true, true, true))
                p_Player->learnSpell(eSpells::ChauffeurH, false);
        }
};

/// Called for Mountain o'Mounts - 2536 (Alliance), 2537 (Horde)
/// Last Update 7.3.5 Build 26365
class PlayerScript_mountain_o_mounts_achievement : public PlayerScript
{
    public:
        PlayerScript_mountain_o_mounts_achievement() : PlayerScript("PlayerScript_mountain_o_mounts_achievement") { }

        enum eAchievements
        {
            MountainOMountsA = 2536,
            MountainOMountsH = 2537
        };

        enum eSpells
        {
            BlueDragonhawk  = 61996,
            RedDragonhawk   = 61997
        };

        enum eItems
        {
            ItemBlueDragonhawk  = 44843,
            ItemRedDragonhawk   = 44842
        };

        void OnLogin(Player* p_Player) override
        {
            if (p_Player == nullptr)
                return;

            if (p_Player->GetTeam() == Team::ALLIANCE && p_Player->HasAchieved(eAchievements::MountainOMountsA))
                CheckAndAddRewards(p_Player, eSpells::BlueDragonhawk, eItems::ItemBlueDragonhawk);
            else if (p_Player->GetTeam() == Team::HORDE && p_Player->HasAchieved(eAchievements::MountainOMountsH))
                CheckAndAddRewards(p_Player, eSpells::RedDragonhawk, eItems::ItemRedDragonhawk);
        }

    private:

        void CheckAndAddRewards(Player* p_Player, uint32 p_SpellId, uint32 p_ItemId)
        {
            if (!p_Player->HasSpell(p_SpellId) && !p_Player->HasItemCount(p_ItemId, 1, true, true, true))
                p_Player->learnSpell(p_SpellId, false);
        }
};

class PlayerScript_Add_Prestige_Appearance_Quest : public PlayerScript
{
    public:
        PlayerScript_Add_Prestige_Appearance_Quest() : PlayerScript("PlayerScript_Add_Prestige_Appearance_Quest") { }

        enum eQuests
        {
            Quest_A_Royal_Audience_Alliance   = 42978,
            Quest_A_Royal_Audience_Horde      = 42985,
            Quest_The_Victors_Spoils_Horde    = 42987,
            Quest_The_Victors_Spoils_Alliance = 42982
        };

        enum eItems
        {
            Item_Grand_Marshals_Medal_Of_Valor = 138992,
            Item_High_Warlords_Medal_Of_Valor  = 138996
        };

        enum eAchievements
        {
            Achievement_Prestige_Alliance      = 10743,
            Achievement_Prestige_Horde         = 10745
        };

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            if (p_Player->GetPrestige() < 1)
                return;

            if ((p_Player->GetPrestige() == 1 && p_Player->GetHonorLevel() == 50) || p_Player->GetPrestige() > 1)
            {
                if (p_Player->GetTeamId() == TEAM_ALLIANCE)
                {
                    if (p_Player->HasQuest(eQuests::Quest_A_Royal_Audience_Alliance) || p_Player->GetQuestStatus(eQuests::Quest_A_Royal_Audience_Alliance) == QUEST_STATUS_REWARDED || p_Player->HasItemCount(eItems::Item_Grand_Marshals_Medal_Of_Valor, 1, true, false))
                        return;

                    p_Player->AddItem(eItems::Item_Grand_Marshals_Medal_Of_Valor, 1);
                }
                else if (p_Player->GetTeamId() == TEAM_HORDE)
                {
                    if (p_Player->HasQuest(eQuests::Quest_A_Royal_Audience_Horde) || p_Player->GetQuestStatus(eQuests::Quest_A_Royal_Audience_Horde) == QUEST_STATUS_REWARDED || p_Player->HasItemCount(eItems::Item_High_Warlords_Medal_Of_Valor, 1, true, false))
                        return;

                    p_Player->AddItem(eItems::Item_High_Warlords_Medal_Of_Valor, 1);
                }
            }
        }

        void OnQuestReward(Player* p_Player, const Quest* p_Quest) override
        {
            if (!p_Player || !p_Quest)
                return;

            if (p_Quest->GetQuestId() == eQuests::Quest_The_Victors_Spoils_Alliance)
            {
                if (AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::Achievement_Prestige_Alliance))
                    p_Player->CompletedAchievement(l_AchievementEntry);
            }

            if (p_Quest->GetQuestId() == eQuests::Quest_The_Victors_Spoils_Horde)
            {
                if (AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::Achievement_Prestige_Horde))
                    p_Player->CompletedAchievement(l_AchievementEntry);
            }
        }
};

class PlayerScript_remove_worgen_spell_at_race_change : public PlayerScript
{
    public:
        PlayerScript_remove_worgen_spell_at_race_change() : PlayerScript("PlayerScript_remove_worgen_spell_at_race_change") { }

    void OnLogin(Player* p_Player) override
    {
        if (p_Player->getRace() == RACE_WORGEN)
            return;

        p_Player->removeSpell(68976);
        p_Player->removeSpell(68996);
        p_Player->removeSpell(97709);
        p_Player->removeSpell(68992);
        p_Player->removeSpell(68978);
        p_Player->removeSpell(68975);
        p_Player->removeSpell(87840);
    }
};

class PlayerScript_meta_achievement_midsummer : public PlayerScript
{
    public:
        PlayerScript_meta_achievement_midsummer() : PlayerScript("PlayerScript_meta_achievement_midsummer") { }

        enum eDatas
        {
            BurningHotPoleDance = 271,
            TorchJuggler = 272,
            KingOfTheFireFestival = 1145,
            IceTheFrostLord = 263,
            MaxAchievements = 4,
            DesacrationOfTheAlliance = 1037,
            DesacrationOfTheHorde = 1035,
            TheFiresOfAzerothX = 1036,
            TheFiresOfAzerothY = 1034,
            TheFlameWarden = 1038,
            TheFlameKeeper = 1039
        };

        std::array<uint32, eDatas::MaxAchievements> m_Achievements =
        {
            {
                eDatas::BurningHotPoleDance,
                eDatas::TorchJuggler,
                eDatas::KingOfTheFireFestival,
                eDatas::IceTheFrostLord
            }
        };

        void OnAchievementEarned(Player* p_Player, AchievementEntry const* p_Achievement, bool& p_SendAchievement, bool p_After) override
        {
            if (!p_Player ||!p_Achievement)
                return;

            bool l_AllCompleted = true;

            for (uint32 l_AchievementID : m_Achievements)
            {
                if (!p_Player->HasAchieved(l_AchievementID))
                    l_AllCompleted = false;
            }

            if (!p_Player->HasAchieved(eDatas::DesacrationOfTheHorde) && !p_Player->HasAchieved(eDatas::DesacrationOfTheAlliance))
                l_AllCompleted = false;

            if (!p_Player->HasAchieved(eDatas::TheFiresOfAzerothY) && !p_Player->HasAchieved(eDatas::TheFiresOfAzerothX))
                l_AllCompleted = false;

            if (!p_After)
            {
                if ((p_Achievement == sAchievementStore.LookupEntry(eDatas::TheFlameKeeper) || p_Achievement == sAchievementStore.LookupEntry(eDatas::TheFlameWarden)) && !l_AllCompleted)
                    p_SendAchievement = false;
            }
            else
            {
                if (l_AllCompleted)
                {
                    if (!p_Player->HasAchieved(eDatas::TheFlameKeeper))
                        p_Player->CompletedAchievement(sAchievementStore.LookupEntry(eDatas::TheFlameKeeper));
                    if (!p_Player->HasAchieved(eDatas::TheFlameWarden))
                        p_Player->CompletedAchievement(sAchievementStore.LookupEntry(eDatas::TheFlameWarden));
                }
            }
        }
};

class PlayerScript_artifact_third_relic_unlock : public PlayerScript
{
    public:
        PlayerScript_artifact_third_relic_unlock() : PlayerScript("PlayerScript_artifact_third_relic_unlock") { }

        enum eAchievements
        {
            ForgedForBattle = 10746
        };

        std::array<uint32, 36> g_ThirdRelicBonuses = { { 721,719,718,717,716,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752 } };

        void OnLogin(Player* p_Player) override
        {
            MS::Artifact::Manager* l_ArtifactMgr = p_Player->GetCurrentlyEquippedArtifact();
            if (!l_ArtifactMgr)
                return;

            p_Player->CompletedAchievement(sAchievementStore.LookupEntry(eAchievements::ForgedForBattle));
            if (Item* l_Item = l_ArtifactMgr->ToItem())
            {
                for (uint32 l_Bonus : g_ThirdRelicBonuses)
                {
                    if (l_Item->HasItemBonus(l_Bonus) && l_Bonus != l_Item->GetTemplate()->GetArtifactThirdSlotBonus())
                        l_Item->RemoveItemBonus(l_Bonus);
                }

                l_Item->AddItemBonus(l_Item->GetTemplate()->GetArtifactThirdSlotBonus());
            }
        }
};

class PlayerScript_artifact_reset_artifact_730_release : public PlayerScript
{
    const std::array<uint32, 21> g_ArtifactLevelXPAfterRank35_715 =
    {{
        1915000,
        2010000,
        2110000,
        2215000,
        2325000,
        2440000,
        2560000,
        2690000,
        2825000,
        2965000,
        3115000,
        3270000,
        3435000,
        3605000,
        3785000,
        3975000,
        4175000,
        4385000,
        4605000,
        4835000,
        5075000
    }};

    public:
        PlayerScript_artifact_reset_artifact_730_release() : PlayerScript("PlayerScript_artifact_reset_artifact_730_release") { }

        void ResetArtifactIfNeeded(Player* p_Player)
        {
            MS::Artifact::Manager* l_ArtifactMgr = p_Player->GetCurrentlyEquippedArtifact();
            if (!l_ArtifactMgr || !l_ArtifactMgr->ToItem())
                return;

            bool l_FirstPargon = false;
            bool l_SecondTier  = l_ArtifactMgr->ToItem()->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) >= 1;

            auto& l_Powers = l_ArtifactMgr->GetPowers();
            for (auto l_Power : l_Powers)
            {
                if (ArtifactPowerEntry const* l_ArtifactPowerEntry = sArtifactPowerStore.LookupEntry(l_Power.first))
                {
                    if (l_ArtifactPowerEntry->ArtifactTier > 0)
                        l_SecondTier = true;
                    else if (l_ArtifactPowerEntry->ArtifactTier == 0 && l_Power.second.PurchasedRank >= 1 && l_ArtifactPowerEntry->Flags == 5)
                        l_FirstPargon = true;

                    /// Paragon trait of the first tier
                    if (l_ArtifactPowerEntry->ArtifactTier == 0 && l_ArtifactPowerEntry->Flags == 5 && l_ArtifactMgr->GetLevel() >= 35)
                    {
                        /// Case 1 : More than one trait into the paragon or paragon rank 1 its a artifact from the 7.1.5
                        /// Case 2 : Only one trait in the paragon but the artifact didn't unlocked the tier 1, its artifact from the 7.1.5
                        /// We need to reset the paragon, player will get the tier 1 by re-learning the paragon trait
                        if (l_Power.second.PurchasedRank > 1 || l_ArtifactMgr->ToItem()->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 0)
                        {
                            uint32 l_Compensation = l_ArtifactMgr->GetLevel() - 35;

                            uint64 l_RefundValue = 0;
                            for (int l_I = 0; l_I < std::min(l_Compensation, (uint32)20); l_I++)
                                l_RefundValue += g_ArtifactLevelXPAfterRank35_715[l_I];

                            l_ArtifactMgr->RemoveOrDowngradeTrait(l_ArtifactPowerEntry->ID, true);

                            l_ArtifactMgr->SetArtifactPower(l_RefundValue);
                            break;
                        }
                    }
                }
            }

            /// Force the reset of whole artifact if second tier unlocked while first paragon not aquired
            if (l_SecondTier && !l_FirstPargon)
            {
                uint64 l_NewAmount = l_ArtifactMgr->GetArtifactPower();
                for (uint32 l_I = 0; l_I <= l_ArtifactMgr->GetLevel(); ++l_I)
                {
                    if (ArtifactLevelXPTableEntry const* l_Cost = g_ArtifactLevelXP.LookupEntry(l_I))
                        l_NewAmount += uint64(l_ArtifactMgr->ToItem()->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 1 ? l_Cost->XP2 : l_Cost->XP);
                }

                l_ArtifactMgr->ToItem()->SetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER, 0);
                l_ArtifactMgr->ToItem()->SetState(ItemUpdateState::ITEM_CHANGED, p_Player);

                l_ArtifactMgr->SetArtifactTier(0);

                l_ArtifactMgr->ResetPowers();
                l_ArtifactMgr->SetArtifactPower(l_NewAmount);

                p_Player->SaveToDB();
            }
        }

        void OnLogin(Player* p_Player) override
        {
            ResetArtifactIfNeeded(p_Player);
        }

        void OnModifySpec(Player* p_Player, int32 /*p_OldSpec*/, int32 /*p_NewSpec*/) override
        {
            ResetArtifactIfNeeded(p_Player);
        }
};

class PlayerScript_weekly_rated_bracket_reward : public PlayerScript
{
    public:
        PlayerScript_weekly_rated_bracket_reward() : PlayerScript("PlayerScript_weekly_rated_bracket_reward") { }

        enum eLegoUpgradeData
        {
            ItemCount = 42
        };

        enum eCriterias
        {
            Win10ArenaMatchesInThe3v3Bracket = 34662,
            Win10ArenaMatchesInThe2v2Bracket = 34658,
            Win3RatedBattlegroundMatches     = 34661,
        };

        std::map<eCriterias, uint8> g_BracketsCriterias =
        {
            { eCriterias::Win10ArenaMatchesInThe2v2Bracket,  (uint8)BattlegroundBracketType::Arena2v2          },
            { eCriterias::Win10ArenaMatchesInThe3v3Bracket,  (uint8)BattlegroundBracketType::Arena3v3          },
            { eCriterias::Win3RatedBattlegroundMatches,      (uint8)BattlegroundBracketType::Battleground10v10 }
        };

        std::map<uint8, uint32> g_RatedBracketWinRequiredForGearReward =
        {
            { (uint8)BattlegroundBracketType::Arena2v2,          10 },
            { (uint8)BattlegroundBracketType::Arena3v3,          10 },
            { (uint8)BattlegroundBracketType::Battleground10v10,  3 }
        };

        std::map<uint16/*Rating*/, uint8> g_RewardIlevelBaseForRating =
        {
            { 1399, (uint8)ItemContext::Ilvl910 },
            { 1599, (uint8)ItemContext::Ilvl915 },
            { 1799, (uint8)ItemContext::Ilvl920 },
            { 1999, (uint8)ItemContext::Ilvl925 },
            { 2199, (uint8)ItemContext::Ilvl930 },
            { 2399, (uint8)ItemContext::Ilvl935 },
            { 3999, (uint8)ItemContext::Ilvl940 }
        };

        /// Custom send of the weekly game win criterias to make the PvP reward UI works.
        /// We need a generic system to handle criterias for quests, atm we only handle criterias for the achievements ...
        void UpdateBracketCriterias(Player* p_Player)
        {
            for (auto l_BracketData : g_BracketsCriterias)
            {
                WorldPackets::Achievement::CriteriaUpdate l_CriteriaUpdate;
                l_CriteriaUpdate.CriteriaID   = l_BracketData.first;
                l_CriteriaUpdate.Quantity     = std::min<uint32>(p_Player->GetWeekWins((uint8)l_BracketData.second), l_BracketData.second == (uint8)BattlegroundBracketType::Battleground10v10 ?  3 : 10);
                l_CriteriaUpdate.PlayerGUID   = p_Player->GetGUID();
                l_CriteriaUpdate.Flags        = 0;
                l_CriteriaUpdate.CurrentTime  = time(nullptr);
                l_CriteriaUpdate.ElapsedTime  = 0;
                l_CriteriaUpdate.CreationTime = 0;
                p_Player->GetSession()->SendPacket(l_CriteriaUpdate.Write());
            }
        }

        void OnLogin(Player* p_Player) override
        {
            UpdateBracketCriterias(p_Player);
        }

        /// Since 7.1, players earn one pvp gear piece per week per bracket after they have win 10 time in 2v2/3v3 or 3 time in 10v10 rbg
        /// The ilevel of the reward scale based on the best rating of the previous week in the bracket
        void OnBracketWin(Player* p_Player, BattlegroundBracketType p_Bracket) override
        {
            /// Only rated battlegrounds (2v2, 3v3 & 10v10 rbg)
            auto l_RewardInfo = g_RatedBracketWinRequiredForGearReward.find((uint8)p_Bracket);
            if (l_RewardInfo == g_RatedBracketWinRequiredForGearReward.end())
                return;

            if (p_Player->GetWeekWins(static_cast<uint8>(p_Bracket)) <= l_RewardInfo->second)
                UpdateBracketCriterias(p_Player);

            if (p_Player->GetWeekWins(static_cast<uint8>(p_Bracket)) != l_RewardInfo->second)
                return;

            uint32 l_BestPrevWeekRating = p_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(p_Bracket));
            ItemContext l_IlvlContext   = ItemContext::WorldQuest10;

            for (auto l_Step : g_RewardIlevelBaseForRating)
            {
                if (l_Step.first < l_BestPrevWeekRating)
                    continue;

                l_IlvlContext = static_cast<ItemContext>(l_Step.second);
                break;
            }

            if (ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(p_Player->GetActiveSpecializationID(), l_BestPrevWeekRating > 2000 ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, p_Player))
            {
                ItemPosCountVec sDest;
                InventoryResult msg = p_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, l_GearReward->ItemId, 1);

                if (msg == EQUIP_ERR_OK)
                {
                    std::vector<uint32> l_ItemBonuses;
                    Item::GenerateItemBonus(l_GearReward->ItemId, l_IlvlContext, l_ItemBonuses, true, p_Player, nullptr, false);

                    if (Item* l_AddedItem = p_Player->StoreNewItem(sDest, l_GearReward->ItemId, true, Item::GenerateItemRandomPropertyId(l_GearReward->ItemId), l_ItemBonuses))
                    {
                        p_Player->SendNewItem(l_AddedItem, 1, true, false);
                        p_Player->SendDisplayToast(l_GearReward->ItemId, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                    }
                }
            }

            /// Wakening Essences
            {
                p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, eLegoUpgradeData::ItemCount);
                p_Player->SendDisplayToast(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, eLegoUpgradeData::ItemCount, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
            }
        }
};

class playerscript_remove_wrong_artifacts : public PlayerScript
{
    public:
        playerscript_remove_wrong_artifacts() : PlayerScript("playerscript_remove_wrong_artifacts") { }

        std::array<std::vector<uint32>, Classes::MAX_CLASSES> m_ArtifactsPerClass =
        {
            {
                /// CLASS_NONE
                { },
                /// CLASS_WARRIOR
                {
                    eArtifacts::WarriorArmsA,           ///< Strom'kar, the Warbreaker
                    eArtifacts::WarriorFuryA,           ///< Warswords of the Valarjar
                    eArtifacts::WarriorProtectionA      ///< Scale of the Earth-Warder
                },
                /// CLASS_PALADIN
                {
                    eArtifacts::PaladinHolyA,           ///< The Silver Hand
                    eArtifacts::PaladinProtectionA,     ///< Truthguard
                    eArtifacts::PaladinRetributionA     ///< Ashbringer
                },
                /// CLASS_HUNTER
                {
                    eArtifacts::HunterBeastMasteryA,    ///< Titanstrike
                    eArtifacts::HunterMarksmanshipA,    ///< Thas'dorah, Legacy of the Windrunners
                    eArtifacts::HunterSurvivalA         ///< Talonclaw
                },
                /// CLASS_ROGUE
                {
                    eArtifacts::RogueAssassinationA,    ///< The Kingslayers
                    eArtifacts::RogueOutlawA,           ///< The Dreadblades
                    eArtifacts::RogueSubletyA           ///< Fangs of the Devourer
                },
                /// CLASS_PRIEST
                {
                    eArtifacts::PriestDisciplineA,      ///< Light's Wrath
                    eArtifacts::PriestHolyA,            ///< T'uure, Beacon of the Naaru
                    eArtifacts::PriestShadowA           ///< Xal'atath, Blade of the Black Empire
                },
                /// CLASS_DEATH_KNIGHT
                {
                    eArtifacts::DeathKnightBloodA,      ///< Maw of the Damned
                    eArtifacts::DeathKnightFrostA,      ///< Blades of the Fallen Prince
                    eArtifacts::DeathKnightUnholyA      ///< Apocalypse
                },
                /// CLASS_SHAMAN
                {
                    eArtifacts::ShamanElementalA,       ///< The Fist of Ra-den
                    eArtifacts::ShamanEnhancementA,     ///< Doomhammer
                    eArtifacts::ShamanRestorationA      ///< Sharas'dal, Scepter of Tides
                },
                /// CLASS_MAGE
                {
                    eArtifacts::MageArcaneA,            ///< Aluneth
                    eArtifacts::MageFireA,              ///< Felo'melorn
                    eArtifacts::MageFrostA              ///< Ebonchill
                },
                /// CLASS_WARLOCK
                {
                    eArtifacts::WarlockAfflictionA,     ///< Ulthalesh, the Deadwind Harvester
                    eArtifacts::WarlockDemonologyA,     ///< Skull of the Man'ari
                    eArtifacts::WarlockDestructionA     ///< Scepter of Sargeras
                },
                /// CLASS_MONK
                {
                    eArtifacts::MonkBrewmasterA,        ///< Fu Zan, the Wanderer's Companion
                    eArtifacts::MonkMistweaverA,        ///< Sheilun, Staff of the Mists
                    eArtifacts::MonkWindwalkerA         ///< Fists of the Heavens
                },
                /// CLASS_DRUID
                {
                    eArtifacts::DruidBalanceA,          ///< Scythe of Elune
                    eArtifacts::DruidFeralA,            ///< Fangs of Ashamane
                    eArtifacts::DruidGuardianA,         ///< Claws of Ursoc
                    eArtifacts::DruidRestorationA       ///< G'Hanir, the Mother Tree
                },
                /// CLASS_DEMON_HUNTER
                {
                    eArtifacts::DemonHunterHavocA,      ///< Twinblades of the Deceiver
                    eArtifacts::DemonHunterVengeanceA   ///< Aldrachi Warblades
                }
            }
        };

        void OnLogin(Player* p_Player) override
        {
            if (p_Player == nullptr)
                return;

            /// Remove artifacts for other classes if needed
            bool l_ArtifactRemoved = false;
            for (uint8 l_ClassIter = 0; l_ClassIter < Classes::MAX_CLASSES; ++l_ClassIter)
            {
                if (p_Player->getClass() == l_ClassIter)
                    continue;

                for (uint32 l_Entry : m_ArtifactsPerClass[l_ClassIter])
                {
                    if (Item* l_Item = p_Player->GetItemByEntry(l_Entry))
                    {
                        l_ArtifactRemoved = true;
                        p_Player->DestroyItem(l_Item->GetBagSlot(), l_Item->GetSlot(), true);

                        if (Item* l_Child = p_Player->GetChildItemByGuid(l_Item->GetChildItem()))
                            p_Player->DestroyItem(l_Child->GetBagSlot(), l_Child->GetSlot(), true);
                    }
                }
            }

            if (l_ArtifactRemoved)
                p_Player->SaveToDB();
        }
};

class playerscript_restrict_class_halls : public PlayerScript
{
    private:

        struct AreaInfo
        {
            Classes PlayerClass;
            int32 MapId;
            int32 ZoneId;
            int32 AreaId;
        };

    public:

        playerscript_restrict_class_halls() : PlayerScript("playerscript_restrict_class_halls") { }

        enum eMaps
        {
            HallOfTheGuardian   = 1513,
            BrokenIsles         = 1220,
            SkyHold             = 1479,
            EasternKingdoms     = 0,
            NetherlightTemple   = 1512,
            TheHeartOfAzeroth   = 1469,
            DreadscarRift       = 1107,
            TheWanderingIsle    = 1514,
            TheFelHammer        = 1519
        };

        enum eZonesAreas
        {
            HallOfTheGuardianZone   = 7879, ///< Mage
            TrueshotLodgeArea       = 7877, ///< Hunter
            SkyHoldZone             = 7813, ///< Warrior
            SanctumOfLightArea      = 7638, ///< Paladin
            TheHallOfShadowsArea    = 8011, ///< Rogue
            NetherlightTempleZone   = 7834, ///< Priest
            TheEbonHoldZone         = 7879, ///< Death Knight
            TheMaelstorm            = 7745, ///< Shaman
            DreadscarRiftZone       = 7875, ///< Warlock
            TheWanderingIsleZone    = 7902, ///< Monk
            TheDreamgroveArea       = 7846, ///< Druid
            MardumZone              = 8022  ///< Demon Hunter
        };

        std::vector<AreaInfo> m_Areas =
        {
            { Classes::CLASS_MAGE,         eMaps::HallOfTheGuardian, eZonesAreas::HallOfTheGuardianZone, -1 },
            { Classes::CLASS_HUNTER,       eMaps::BrokenIsles,       -1,                                 eZonesAreas::TrueshotLodgeArea },
            { Classes::CLASS_WARRIOR,      eMaps::SkyHold,           eZonesAreas::SkyHoldZone,           -1 },
            { Classes::CLASS_PALADIN,      eMaps::EasternKingdoms,   -1,                                 eZonesAreas::SanctumOfLightArea },
            { Classes::CLASS_ROGUE,        eMaps::BrokenIsles,       -1,                                 eZonesAreas::TheHallOfShadowsArea },
            { Classes::CLASS_PRIEST,       eMaps::NetherlightTemple, eZonesAreas::NetherlightTempleZone, -1 },
            { Classes::CLASS_DEATH_KNIGHT, eMaps::BrokenIsles,       eZonesAreas::TheEbonHoldZone,       -1 },
            { Classes::CLASS_SHAMAN,       eMaps::TheHeartOfAzeroth, eZonesAreas::TheMaelstorm,          -1 },
            { Classes::CLASS_WARLOCK,      eMaps::DreadscarRift,     eZonesAreas::DreadscarRiftZone,     -1 },
            { Classes::CLASS_MONK,         eMaps::TheWanderingIsle,  eZonesAreas::TheWanderingIsleZone,  -1 },
            { Classes::CLASS_DRUID,        eMaps::BrokenIsles,       -1,                                 eZonesAreas::TheDreamgroveArea },
            { Classes::CLASS_DEMON_HUNTER, eMaps::TheFelHammer,      eZonesAreas::MardumZone,            -1 }
        };

        virtual void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 p_NewAreaID) override
        {
            if (p_Player->isGameMaster())
                return;

            if (p_NewZoneID == p_OldZoneID)
                return;

            if (IsInOtherClassHall(p_Player->getClass(), p_Player->GetMapId(), p_NewZoneID, p_NewAreaID))
            {
                uint32 l_HomeZoneId = 0;
                uint32 l_HomeAreaId = 0;
                sMapMgr->GetZoneAndAreaId(l_HomeZoneId, l_HomeAreaId, p_Player->m_homebindMapId, p_Player->m_homebindX, p_Player->m_homebindY, p_Player->m_homebindZ);

                WorldLocation l_TeleportLoc;

                if (IsInOtherClassHall(p_Player->getClass(), p_Player->m_homebindMapId, l_HomeZoneId, l_HomeAreaId))
                {
                    if (p_Player->GetTeam() == TEAM_ALLIANCE)
                    {
                        l_TeleportLoc.m_mapId = 0;
                        l_TeleportLoc.Relocate(-8866.19f, 671.16f, 97.9034f, 0.0f);
                    }
                    else
                    {
                        l_TeleportLoc.m_mapId = 1;
                        l_TeleportLoc.Relocate(1633.33f, -4439.11f, 17.05f, 0.0f);
                    }
                }
                else
                {
                    l_TeleportLoc.m_mapId = p_Player->m_homebindMapId;
                    l_TeleportLoc.Relocate(p_Player->m_homebindX, p_Player->m_homebindY, p_Player->m_homebindZ, 0.0f);
                }

                p_Player->AddDelayedEvent([p_Player, l_TeleportLoc]() -> void
                {
                    p_Player->TeleportTo(l_TeleportLoc.GetMapId(), l_TeleportLoc.GetPositionX(), l_TeleportLoc.GetPositionY(), l_TeleportLoc.GetPositionZ(), l_TeleportLoc.GetOrientation());
                }, 1);
            }
        }

    private:

        bool IsInOtherClassHall(uint8 p_Class, uint32 p_MapId, uint32 p_ZoneId, uint32 p_AreaId) const
        {
            for (auto const& l_Area : m_Areas)
            {
                if (p_MapId != l_Area.MapId)
                    continue;

                if (l_Area.ZoneId != -1 && p_ZoneId != l_Area.ZoneId)
                    continue;

                if (l_Area.AreaId != -1 && p_AreaId != l_Area.AreaId)
                    continue;

                if (l_Area.PlayerClass != (Classes)p_Class)
                    return true;
            }

            return false;
        }
};

class PlayerScript_Allied_Race_reach_110 : public PlayerScript
{
    enum eQuests
    {
        HeritageOfHighmountain   = 49783,
        HeritageOfTheNightborne  = 49784,
        HeritageOfTheLightforged = 49782,
        HeritageOfTheVoid        = 49928
    };

    public:
        PlayerScript_Allied_Race_reach_110() : PlayerScript("PlayerScript_Allied_Race_reach_110") { }

        uint32 GetQuestID(Player* p_Player)
        {
            switch (p_Player->getRace())
            {
                case Races::RACE_HIGHMOUNTAIN_TAUREN:
                    return eQuests::HeritageOfHighmountain;
                case Races::RACE_NIGHTBORNE:
                    return eQuests::HeritageOfTheNightborne;
                case Races::RACE_LIGHTFORGED_DRAENEI:
                    return eQuests::HeritageOfTheLightforged;
                case Races::RACE_VOID_ELF:
                    return eQuests::HeritageOfTheVoid;
            }

            return 0;
        }

        void OnLevelChanged(Player* p_Player, uint8 p_OldLevel) override
        {
            if (p_Player->getLevel() != 110 || p_OldLevel != 109)
                return;

            uint32 l_QuestId = GetQuestID(p_Player);
            if (!l_QuestId)
                return;

            p_Player->AddQuest(sObjectMgr->GetQuestTemplate(l_QuestId), p_Player);
            p_Player->SetWorldState(l_QuestId, 1);
        }

        /// Attempt to re-add the quest at login if the quest wasn't added because the questlog was full or if the players abandon it
        void OnLogin(Player* p_Player) override
        {
            if (p_Player->getLevel() < 110)
                return;

            uint32 l_QuestId = GetQuestID(p_Player);
            if (!l_QuestId)
                return;

            if (!p_Player->IsQuestRewarded(l_QuestId) && !p_Player->HasQuest(l_QuestId) && p_Player->GetWorldState(l_QuestId) == 1)
                p_Player->AddQuest(sObjectMgr->GetQuestTemplate(l_QuestId), p_Player);
        }
};

class PlayerScript_Broken_Isles_Pathfinder : public PlayerScript
{
    public:
        PlayerScript_Broken_Isles_Pathfinder() : PlayerScript("PlayerScript_Broken_Isles_Pathfinder") { }

        enum eAchivements
        {
            AGloriousCampaign           = 10994,
            GoodSuramaritan             = 11124,

            BrokenIslesPathfinderPart1  = 11190,
            BrokenIslesPathfinderPart2  = 11446
        };

        enum eSpells
        {
            PathfinderPart1 = 226342,
            PathfinderPart2 = 233368
        };

        std::vector<uint32> m_Achievements =
        {
            eAchivements::AGloriousCampaign,
            eAchivements::GoodSuramaritan
        };

        void OnLevelChanged(Player* p_Player, uint8 /*p_OldLevel*/) override
        {
            if (p_Player->getLevel() != 110)
                return;

            for (uint32 l_AchievmentID : m_Achievements)
            {
                AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(l_AchievmentID);
                if (!l_AchievementEntry)
                    continue;

                p_Player->CompletedAchievement(l_AchievementEntry);
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasAchieved(eAchivements::BrokenIslesPathfinderPart1))
            {
                p_Player->learnSpell(eSpells::PathfinderPart1, false, false, false, 0, true);
                p_Player->CastSpell(p_Player, eSpells::PathfinderPart1, true);
            }

            if (p_Player->HasAchieved(eAchivements::BrokenIslesPathfinderPart2))
            {
                p_Player->learnSpell(eSpells::PathfinderPart2, false, false, false, 0, true);
                p_Player->CastSpell(p_Player, eSpells::PathfinderPart2, true);
            }

            if (p_Player->getLevel() != 110)
                return;

            for (uint32 l_AchievmentID : m_Achievements)
            {
                AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(l_AchievmentID);
                if (!l_AchievementEntry)
                    continue;

                p_Player->CompletedAchievement(l_AchievementEntry);
            }
        }

        void OnAchievementEarned(Player* p_Player, AchievementEntry const* p_Achievement, bool& p_SendAchievement, bool p_After) override
        {
            if (p_After)
                return;

            switch (p_Achievement->ID)
            {
                case eAchivements::BrokenIslesPathfinderPart1:
                    p_Player->learnSpell(eSpells::PathfinderPart1, false, false, false, 0, true);
                    p_Player->CastSpell(p_Player, eSpells::PathfinderPart1, true);
                    break;
                case eAchivements::BrokenIslesPathfinderPart2:
                    p_Player->learnSpell(eSpells::PathfinderPart2, false, false, false, 0, true);
                    p_Player->CastSpell(p_Player, eSpells::PathfinderPart2, true);
                    break;
                default:
                    break;
            }
        }

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID) override
        {
            switch (p_SpellID)
            {
                case eSpells::PathfinderPart1:
                {
                    if (!p_Player->HasAchieved(eAchivements::BrokenIslesPathfinderPart1))
                    {
                        AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchivements::BrokenIslesPathfinderPart1);
                        if (!l_AchievementEntry)
                            break;

                        p_Player->CompletedAchievement(l_AchievementEntry);
                    }
                    break;
                }
                case eSpells::PathfinderPart2:
                {
                    if (!p_Player->HasAchieved(eAchivements::BrokenIslesPathfinderPart2))
                    {
                        AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchivements::BrokenIslesPathfinderPart2);
                        if (!l_AchievementEntry)
                            break;

                        p_Player->CompletedAchievement(l_AchievementEntry);
                    }
                    break;
                }
                default:
                    break;
            }
        }
};

class PlayerScript_Unlock_Invasions_WQ : public PlayerScript
{
    enum eAchievement
    {
        UnlockWQ = 11473
    };

    public:
        PlayerScript_Unlock_Invasions_WQ() : PlayerScript("PlayerScript_Unlock_Invasions_WQ") { }

        /// Attempt to re-add the quest at login if the quest wasn't added because the questlog was full or if the players abandon it
        /// TO DO: repair WQ unlock. Should be added after https://ru.wowhead.com/quest=43341 completion
        void OnLogin(Player* p_Player) override
        {
            if (p_Player->getLevel() < 98)
                return;

            AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievement::UnlockWQ); ///< WQ Unlock hidden achievement
            if (!l_AchievementEntry)
                return;

            p_Player->CompletedAchievement(l_AchievementEntry);
        }
};

class playerScript_scene_postmaster_game : public PlayerScript
{
    public:
        playerScript_scene_postmaster_game() : PlayerScript("playerScript_scene_postmaster_game") { }

        enum eQuests
        {
            QuestMailMustFlow = 50247
        };

        enum eCredits
        {
            CreditMailMustFlow = 133038
        };

        enum eAchievements
        {
            AchievementPostHaste = 12431,
            AchievementPriorityMail = 12439
        };

        void OnSceneTriggerEvent(Player* p_Player, uint32 p_SceneInstanceID, std::string p_Event) override
        {
            if (p_Event == "credit_small" && p_Player->HasQuest(eQuests::QuestMailMustFlow))
                p_Player->KilledMonsterCredit(eCredits::CreditMailMustFlow);

            if (p_Event == "credit_large" && !p_Player->HasAchieved(eAchievements::AchievementPostHaste))
                p_Player->CompletedAchievement(sAchievementStore.LookupEntry(eAchievements::AchievementPostHaste));

            if (p_Event == "credit_mythic" && !p_Player->HasAchieved(eAchievements::AchievementPriorityMail))
                p_Player->CompletedAchievement(sAchievementStore.LookupEntry(eAchievements::AchievementPriorityMail));
        }
};

class playerScript_scene_shaman_puzzle_game : public PlayerScript
{
    public:
        playerScript_scene_shaman_puzzle_game() : PlayerScript("playerScript_scene_shaman_puzzle_game") { }

        enum eSpells
        {
            LevelOne    = 203627,
            LevelTwo    = 203628,
            LevelThree  = 203629,
            LevelFour   = 203630,
            LevelFive   = 203631
        };

        void OnSceneTriggerEvent(Player* p_Player, uint32 p_SceneInstanceID, std::string p_Event) override
        {
            if (p_Event == "LVL01WIN")
                p_Player->CastSpell(p_Player, eSpells::LevelOne, true);

            if (p_Event == "LVL02WIN")
                p_Player->CastSpell(p_Player, eSpells::LevelTwo, true);

            if (p_Event == "LVL03WIN")
                p_Player->CastSpell(p_Player, eSpells::LevelThree, true);

            if (p_Event == "LVL04WIN")
                p_Player->CastSpell(p_Player, eSpells::LevelFour, true);

            if (p_Event == "LVL05WIN")
                p_Player->CastSpell(p_Player, eSpells::LevelFive, true);
        }
};

class player_script_paragon_quest_completion : public PlayerScript
{
    public:
        player_script_paragon_quest_completion() : PlayerScript("player_script_paragon_quest_completion") { }

        enum eQuests
        {
            QuestSuppliesFromHighmountain       = 46743,
            QuestSuppliesFromTheValarjar        = 46746,
            QuestSuppliesFromTheCourt           = 46745,
            QuestSuppliesFromTheDreamweavers    = 46747,
            QuestSuppliesFromTheNightfallen     = 46748,
            QuestSuppliesFromTheWardens         = 46749,
            QuestTheBountiesOfLegionfall        = 46777,
            QuestSuppliesFromTheArmyOfLight     = 48977,
            QuestSuppliesFromTheArgussianReach  = 48976
        };

        enum eFactions
        {
            FactionHighmountain                 = 2085,
            FactionValarjar                     = 2086,
            FactionCourtOfFarondis              = 2087,
            FactionTheDreamweavers              = 2088,
            FactionTheNightfallen               = 2089,
            FactionTheWardens                   = 2090,
            FactionLegionfall                   = 2091,
            FactionTheArmyOfLight               = 2166,
            FactionArgussianReach               = 2167
        };

        std::map<uint16, uint16> m_FactionQuests =
        {
            { eQuests::QuestSuppliesFromHighmountain,       eFactions::FactionHighmountain      },
            { eQuests::QuestSuppliesFromTheValarjar,        eFactions::FactionValarjar          },
            { eQuests::QuestSuppliesFromTheCourt,           eFactions::FactionCourtOfFarondis   },
            { eQuests::QuestSuppliesFromTheDreamweavers,    eFactions::FactionTheDreamweavers   },
            { eQuests::QuestSuppliesFromTheNightfallen,     eFactions::FactionTheNightfallen    },
            { eQuests::QuestSuppliesFromTheWardens,         eFactions::FactionTheWardens        },
            { eQuests::QuestTheBountiesOfLegionfall,        eFactions::FactionLegionfall        },
            { eQuests::QuestSuppliesFromTheArmyOfLight,     eFactions::FactionTheArmyOfLight    },
            { eQuests::QuestSuppliesFromTheArgussianReach,  eFactions::FactionArgussianReach    }
        };

        void OnQuestReward(Player* p_Player, Quest const* p_Quest) override
        {
            if (!p_Player || !p_Quest)
                return;

            auto const& l_Iter = m_FactionQuests.find(p_Quest->GetQuestId());
            if (l_Iter == m_FactionQuests.end())
                return;

            /// Reset Paragon reputation to 0, without updating client (client already sets it to 0)
            p_Player->GetReputationMgr().ResetReputation(sFactionStore.LookupEntry(l_Iter->second), false);
        }
};

class playerScript_enter_lfg_channel : public PlayerScript
{
    public:
        playerScript_enter_lfg_channel() : PlayerScript("playerScript_enter_lfg_channel") { }

        enum eChannelIds
        {
            LookingForGroup = 26
        };

        enum eDelays
        {
            Delay = 10 * TimeConstants::IN_MILLISECONDS
        };

        void OnLogin(Player* p_Player) override
        {
            if (!sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_ON_LOGIN_LFG_CHANNEL))
                return;

            p_Player->AddDelayedEvent([p_Player]() -> void
            {
                ChannelMgr* cMgr = channelMgr(p_Player->GetTeam());
                if (!cMgr)
                    return;

                if (ChatChannelsEntry const* l_ChannelEntry = sChatChannelsStore.LookupEntry(eChannelIds::LookingForGroup))
                {
                    /// If the client is for RU but DB2.Locale is EN then we should use RU locale (a custom setting)
                    int32 l_LfgLocale = sWorld->getIntConfig(WorldIntConfigs::CONFIG_ON_LOGIN_LFG_CHANNEL_LOCALE);
                    if (l_LfgLocale < 0 || l_LfgLocale >= LocaleConstant::LOCALE_None)
                        l_LfgLocale = sWorld->GetDefaultDb2Locale();

                    const char* l_ChannelName = l_ChannelEntry->NameLang->Get(l_LfgLocale);
                    if (Channel* l_Channel = cMgr->GetJoinChannel(l_ChannelName, l_ChannelEntry->ID))
                        l_Channel->Join(p_Player->GetGUID(), "");
                }
            }, eDelays::Delay); /// We must join to the channel after the client joins to general channels
        }
};

class PlayerScript_Unlock_Argus_Skybox : public PlayerScript
{
    enum eAchievement
    {
        DeceiversFall = 11790
    };

    enum eSpell
    {
        SpellArgusFilter = 258803
    };

    enum eBody
    {
        ArgusSkybox = 9
    };

    public:
        PlayerScript_Unlock_Argus_Skybox() : PlayerScript("PlayerScript_Unlock_Argus_Skybox") { }

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player->HasAchieved(eAchievement::DeceiversFall))
                return;

            p_Player->CastSpell(p_Player, eSpell::SpellArgusFilter, true);

            p_Player->AddDelayedEvent([p_Player]() -> void
            {
                p_Player->SendUpdateCelestialBody(eBody::ArgusSkybox);
            }, 1 * TimeConstants::IN_MILLISECONDS);
        }
};

///< Reward of achievement 5875 - Littlest Pet Shop
class PlayerScript_littlest_pet_shop_achiev_reward : public PlayerScript
{
    public:
        PlayerScript_littlest_pet_shop_achiev_reward() : PlayerScript("PlayerScript_littlest_pet_shop_achiev_reward") {}
        enum eSpells
        {
            CelestialDragonSpell = 75613
        };
        enum eItemRewards
        {
            CelestialDragon = 54810
        };
        enum eAchievements
        {
            LittlestPetShop = 5875
        };
        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            if (p_Player->HasAchieved(eAchievements::LittlestPetShop))
            {
               if (!p_Player->HasSpell(eSpells::CelestialDragonSpell) && !p_Player->HasItemCount(eItemRewards::CelestialDragon, 1, true, false))
                    p_Player->AddItem(eItemRewards::CelestialDragon, 1);
            }
        }
};

///< Crazy for Cats achievement - 8397
class PlayerScript_crazy_for_cats_achievement : public PlayerScript
{
    public:
        PlayerScript_crazy_for_cats_achievement() : PlayerScript("PlayerScript_crazy_for_cats_achievement") { }

        enum eAchievements
        {
            CrazyForCats = 8397
        };

        enum eTitles
        {
            TheCrazyCatLady = 365,
            TheCrazyCatMan  = 377
        };

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            CharTitlesEntry const* l_TitleMale = sCharTitlesStore.LookupEntry(eTitles::TheCrazyCatMan);
            CharTitlesEntry const* l_TitleFemale = sCharTitlesStore.LookupEntry(eTitles::TheCrazyCatLady);
            if (p_Player->HasAchieved(eAchievements::CrazyForCats))
            {
                if (p_Player->getGender() == GENDER_MALE && !p_Player->HasTitle(l_TitleMale))
                    p_Player->SetTitle(l_TitleMale);
                if (p_Player->getGender() == GENDER_FEMALE && !p_Player->HasTitle(l_TitleFemale))
                    p_Player->SetTitle(l_TitleFemale);
            }
        }
};

class playerscript_for_special_missions_quests : public PlayerScript
{
public:
    playerscript_for_special_missions_quests() : PlayerScript("playerscript_for_special_missions_quests") { }

    // Add Missions for special quests
    std::map<uint32, uint32> m_quest_mission
    {  // quest  mission
        { 43265, 1210 },
        { 42847, 1113 },
    };

    void OnQuestAccept(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        for (auto l_quest_mission : m_quest_mission)
            if (p_Quest->GetQuestId() == l_quest_mission.first)
                if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                    l_Garr->AddMission(l_quest_mission.second);
    }

    void OnQuestAbandon(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        for (auto l_quest_mission : m_quest_mission)
            if (p_Quest->GetQuestId() == l_quest_mission.first)
                if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                    l_Garr->RemoveMission(l_quest_mission.second);
    }

    void OnQuestReward(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        for (auto l_quest_mission : m_quest_mission)
            if (p_Quest->GetQuestId() == l_quest_mission.first)
                if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                    l_Garr->RemoveMission(l_quest_mission.second);
    }

    void OnGarrisonMissionComplete(Player* p_Player, uint32 p_MissionId, bool p_Succeeded)
    {
        if (p_Player == nullptr)
            return;

        if (p_Succeeded)
        {
            for (auto l_quest_mission : m_quest_mission)
                if (p_MissionId == l_quest_mission.second)
                    if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_quest_mission.first))
                        for (QuestObjective l_Objective : l_Quest->QuestObjectives)
                        {
                            p_Player->QuestObjectiveSatisfy(l_Objective.ObjectID, l_Objective.Amount, l_Objective.Type);
                            p_Player->CompleteQuest(l_quest_mission.first);
                        }
        }
        else
        {
            for (auto l_quest_mission : m_quest_mission)
                if (p_MissionId == l_quest_mission.second)
                    if (p_Player->GetQuestStatus(l_quest_mission.first) == QUEST_STATUS_INCOMPLETE)
                        if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                        {
                            l_Garr->RemoveMission(l_quest_mission.second);
                            l_Garr->AddMission(l_quest_mission.second);
                        }
        }
    }
};

class playerscript_for_flash_of_bronze_quests : public PlayerScript
{
public:
    playerscript_for_flash_of_bronze_quests() : PlayerScript("playerscript_for_flash_of_bronze_quests") { }

    enum eAreas
    {
        ShrineOfTwoMoons = 6554,
        ShrineOfSevenStarts = 6142,
    };

    enum eQuests
    {
        HordeQuest = 33230,
        AlianceQuest = 33229,
    };

    void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 p_NewAreaID)
    {
        if (p_Player == nullptr)
            return;

        switch (p_NewAreaID)
        {
            case ShrineOfSevenStarts:
                if (p_Player->GetTeam() == ALLIANCE && p_Player->GetQuestStatus(AlianceQuest) == QUEST_STATUS_NONE)
                    p_Player->AddQuest(sObjectMgr->GetQuestTemplate(AlianceQuest), NULL);
                break;
            case ShrineOfTwoMoons:
                if (p_Player->GetTeam() == HORDE && p_Player->GetQuestStatus(HordeQuest) == QUEST_STATUS_NONE)
                    p_Player->AddQuest(sObjectMgr->GetQuestTemplate(HordeQuest), NULL);
                break;

            default:
                return;
        }
    }
};

class playerscript_free_for_all_more_for_me_achievement : public PlayerScript
{
public:
    playerscript_free_for_all_more_for_me_achievement() : PlayerScript("playerscript_free_for_all_more_for_me_achievement") { }

    enum eAchievements
    {
        FreeForAllMoreForMe = 11474
    };

    enum eSpells
    {
        PrestigiousWarSteed = 193695,
        PrestigiousWarWolf = 204166
    };

    enum eItems
    {
        ReinsOfPrestigiousWarSteed = 129280,
        ReinsOfPrestigiousWarWolf = 143864,
    };

    void OnAchievementComplete(Player* p_Player, AchievementEntry const* p_Achievement) override
    {
        if (!p_Player || !p_Achievement)
            return;

        if (p_Achievement->ID == eAchievements::FreeForAllMoreForMe)
            CheckAndAddRewards(p_Player);
    }

    void OnLogin(Player* p_Player) override
    {
        if (!p_Player)
            return;

        if (p_Player->HasAchieved(eAchievements::FreeForAllMoreForMe))
            CheckAndAddRewards(p_Player);
    }

private:

    void CheckAndAddRewards(Player* p_Player)
    {
        if (p_Player->GetTeam() == ALLIANCE)
        {
            if (!p_Player->HasSpell(eSpells::PrestigiousWarSteed) && !p_Player->HasItemCount(eItems::ReinsOfPrestigiousWarSteed, 1, true, true, true))
                p_Player->AddItem(eItems::ReinsOfPrestigiousWarSteed, 1);
        }
        else
        {
            if (!p_Player->HasSpell(eSpells::PrestigiousWarWolf) && !p_Player->HasItemCount(eItems::ReinsOfPrestigiousWarWolf, 1, true, true, true))
                p_Player->AddItem(eItems::ReinsOfPrestigiousWarWolf, 1);
        }
    }
};

class playerscript_for_timeless_tour : public PlayerScript
{
public:
    playerscript_for_timeless_tour() : PlayerScript("playerscript_for_timeless_tour") { }

    enum eQuests
    {
        TimelessTour = 33161
    };

    std::map<uint32, uint32> m_area_credit
    {  // area   credit
        { 6780, 73577 },
        { 6824, 73576 },
        { 6825, 73579 },
        { 6835, 73578 },
        { 6773, 73574 },
        { 6841, 73575 }
    };

    void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 p_NewAreaID)
    {
        if (p_Player == nullptr || p_Player->GetQuestStatus(eQuests::TimelessTour) != QUEST_STATUS_INCOMPLETE)
            return;

        for (auto l_area_credit : m_area_credit)
            if (p_NewAreaID == l_area_credit.first)
                p_Player->KilledMonsterCredit(l_area_credit.second);
    }
};

class playerscript_for_darkportal_movie : public PlayerScript
{
public:
    playerscript_for_darkportal_movie() : PlayerScript("playerscript_for_darkportal_movie") { }

    enum eMovies
    {
        DarkPortalMovie = 185
    };

    enum eSpells
    {
        DarkPortalTeleport = 167771
    };

    void OnMovieComplete(Player* p_Player, uint32 p_MovieId) override
    {
        if (p_Player == nullptr)
            return;

        if (p_MovieId == eMovies::DarkPortalMovie)
            p_Player->CastSpell(p_Player, eSpells::DarkPortalTeleport);
    }
};

class playerscript_for_noble_event_quest : public PlayerScript
{
public:
    playerscript_for_noble_event_quest() : PlayerScript("playerscript_for_noble_event_quest") { }

    enum eQuests
    {
        ANobleEvent = 44057
    };

    enum eMissions
    {
        OnTheHunt = 1392,
        SecuringAnInvitation = 1393,
        AllDressedUp = 1394,
        JustInTime = 1395
    };

    void OnQuestAccept(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        if (p_Quest->GetQuestId() == eQuests::ANobleEvent)
            if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                l_Garr->AddMission(eMissions::OnTheHunt);
    }

    void OnQuestAbandon(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        if (p_Quest->GetQuestId() == eQuests::ANobleEvent)
            if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
            {
                l_Garr->RemoveMission(eMissions::OnTheHunt);
                l_Garr->RemoveMission(eMissions::SecuringAnInvitation);
                l_Garr->RemoveMission(eMissions::AllDressedUp);
                l_Garr->RemoveMission(eMissions::JustInTime);
            }
    }

    void OnQuestReward(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        if (p_Quest->GetQuestId() == eQuests::ANobleEvent)
            if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
            {
                l_Garr->RemoveMission(eMissions::OnTheHunt);
                l_Garr->RemoveMission(eMissions::SecuringAnInvitation);
                l_Garr->RemoveMission(eMissions::AllDressedUp);
                l_Garr->RemoveMission(eMissions::JustInTime);
            }
    }

    void OnGarrisonMissionComplete(Player* p_Player, uint32 p_MissionId, bool p_Succeeded)
    {
        if (p_Player == nullptr)
            return;

        if (p_MissionId == eMissions::OnTheHunt || p_MissionId == eMissions::SecuringAnInvitation || p_MissionId == eMissions::AllDressedUp || p_MissionId == eMissions::JustInTime)
        {
            if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
            {
                if (p_Succeeded)
                {
                    switch (p_MissionId)
                    {
                        case eMissions::OnTheHunt:
                            l_Garr->AddMission(eMissions::SecuringAnInvitation);
                            break;
                        case eMissions::SecuringAnInvitation:
                            l_Garr->AddMission(eMissions::AllDressedUp);
                            break;
                        case eMissions::AllDressedUp:
                            l_Garr->AddMission(eMissions::JustInTime);
                            break;
                    }
                }
                else
                {
                    switch (p_MissionId)
                    {
                        case eMissions::OnTheHunt:
                            l_Garr->AddMission(eMissions::OnTheHunt);
                            break;
                        case eMissions::SecuringAnInvitation:
                            l_Garr->AddMission(eMissions::SecuringAnInvitation);
                            break;
                        case eMissions::AllDressedUp:
                            l_Garr->AddMission(eMissions::AllDressedUp);
                            break;
                        case eMissions::JustInTime:
                            l_Garr->AddMission(eMissions::JustInTime);
                            break;
                    }
                }
            }
        }
    }
};

class playerscript_and_chew_mana_buns_achievement : public PlayerScript
{
public:
    playerscript_and_chew_mana_buns_achievement() : PlayerScript("playerscript_and_chew_mana_buns_achievement") {}

    enum eData
    {
        AndChewManaBunsAchieve      = 12103,
        AndChewManaBunsSpellReward  = 254260,
    };

    void OnLogin(Player* player) override
    {
        if (!player)
            return;

        if (player->HasAchieved(eData::AndChewManaBunsAchieve) && !player->HasSpell(eData::AndChewManaBunsSpellReward))
            player->AddDelayedEvent([_player = player]() -> void { _player->learnSpell(eData::AndChewManaBunsSpellReward, false); }, 10);
    }
};

class playerscript_for_five_missions_quests : public PlayerScript
{
public:
    playerscript_for_five_missions_quests() : PlayerScript("playerscript_for_five_missions_quests") { }

    enum eQuests
    {
        SteedsOfTheDamned = 43899,
        AggregatesOfAnguish = 43928,
        Lumenstone = 43698
    };

    enum eMissions
    {
        FrozenCrown = 1260,
        TheBaronsSaddle = 1261,
        HeadlessHorseman = 1262,
        Huntsman = 1263,
        ReinsOfShadow = 1264,

        EssenceOfUndeath = 1271,
        EssenceOfFear = 1272,
        EssenceOfShadow = 1273,
        EssenceOfHatred = 1274,
        EssenceOfDarkness = 1275,

        Faronaar = 1285,
        Suramar = 1286,
        Stormheim = 1287,
        BrokenShore = 1288,
        Valsharah = 1289
    };

    struct QuestMissionsInfo
    {
        int32 QuestId;
        int32 OrderId;
        int32 MissionId;
    };

    std::vector<QuestMissionsInfo> m_Info =
    {
        { eQuests::SteedsOfTheDamned, 1, eMissions::FrozenCrown },
        { eQuests::SteedsOfTheDamned, 2, eMissions::TheBaronsSaddle },
        { eQuests::SteedsOfTheDamned, 3, eMissions::HeadlessHorseman },
        { eQuests::SteedsOfTheDamned, 4, eMissions::Huntsman },
        { eQuests::SteedsOfTheDamned, 5, eMissions::ReinsOfShadow },

        { eQuests::AggregatesOfAnguish, 1, eMissions::EssenceOfUndeath },
        { eQuests::AggregatesOfAnguish, 2, eMissions::EssenceOfFear },
        { eQuests::AggregatesOfAnguish, 3, eMissions::EssenceOfShadow },
        { eQuests::AggregatesOfAnguish, 4, eMissions::EssenceOfHatred },
        { eQuests::AggregatesOfAnguish, 5, eMissions::EssenceOfDarkness },

        { eQuests::Lumenstone, 1, eMissions::Faronaar },
        { eQuests::Lumenstone, 2, eMissions::Suramar },
        { eQuests::Lumenstone, 3, eMissions::Stormheim },
        { eQuests::Lumenstone, 4, eMissions::BrokenShore },
        { eQuests::Lumenstone, 5, eMissions::Valsharah },
    };

    void OnQuestAccept(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        for (auto const& l_Info : m_Info)
            if (p_Quest->GetQuestId() == l_Info.QuestId && l_Info.OrderId == 1)
                if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                    l_Garr->AddMission(l_Info.MissionId);
    }

    void OnQuestAbandon(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        for (auto const& l_Info : m_Info)
            if (p_Quest->GetQuestId() == l_Info.QuestId)
                if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                    l_Garr->RemoveMission(l_Info.MissionId);
    }

    void OnQuestReward(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        for (auto const& l_Info : m_Info)
            if (p_Quest->GetQuestId() == l_Info.QuestId)
                if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
                    l_Garr->RemoveMission(l_Info.MissionId);
    }

    void OnGarrisonMissionComplete(Player* p_Player, uint32 p_MissionId, bool p_Succeeded)
    {
        if (p_Player == nullptr)
            return;

        if (MS::Garrison::Manager* l_Garr = p_Player->GetGarrison())
        {
            if (p_Succeeded)
            {
                for (auto const& l_Info : m_Info)
                    if (p_MissionId == l_Info.MissionId && l_Info.OrderId != 5)
                        l_Garr->AddMission(l_Info.MissionId + 1);
            }
            else
            {
                for (auto const& l_Info : m_Info)
                    if (p_MissionId == l_Info.MissionId)
                        l_Garr->AddMission(l_Info.MissionId);
            }
        }
    }
};

class playerscript_for_my_very_own_castle : public PlayerScript
{
public:
    playerscript_for_my_very_own_castle() : PlayerScript("playerscript_for_my_very_own_castle") { }

    enum eQuests
    {
        MyVeryOwnCastle = 36615
    };

    void OnQuestAccept(Player* p_Player, const Quest* p_Quest)
    {
        if (p_Player == nullptr)
            return;

        if (p_Quest->GetQuestId() == eQuests::MyVeryOwnCastle)
            if (auto l_GarrisonMgr = p_Player->GetDraenorGarrison())
                if (l_GarrisonMgr->GetGarrisonLevel() >= 3)
                    for (QuestObjective l_Objective : p_Quest->QuestObjectives)
                    {
                        if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_CRITERIA_TREE)
                            p_Player->QuestObjectiveSatisfy(l_Objective.ObjectID, l_Objective.Amount, l_Objective.Type);
                    }
    }
};

#ifndef __clang_analyzer__
void AddSC_CustomPlayerscripts()
{
    new playerscript_broken_isles_strike_point_quests();
    new PlayerScript_The_Molten_Front_Offensive_Achievement();
    new PlayerScript_menagerie_achievement();
    new PlayerScript_heirloom_hoarder_achievement();
    new PlayerScript_mountain_o_mounts_achievement();
    new PlayerScript_Add_Prestige_Appearance_Quest();
    new PlayerScript_meta_achievement_midsummer();
    new PlayerScript_remove_worgen_spell_at_race_change();
    new PlayerScript_artifact_third_relic_unlock();
    new PlayerScript_weekly_rated_bracket_reward();
    new playerscript_remove_wrong_artifacts();
    new playerscript_restrict_class_halls();
    new PlayerScript_artifact_reset_artifact_730_release();
    new playerscript_mountacular_achievement();
    new PlayerScript_Allied_Race_reach_110();
    new PlayerScript_Broken_Isles_Pathfinder();
    new PlayerScript_Unlock_Invasions_WQ();
    new playerScript_scene_postmaster_game();
    new playerScript_scene_shaman_puzzle_game();
    new player_script_paragon_quest_completion();
    new playerScript_enter_lfg_channel();
    new PlayerScript_Unlock_Argus_Skybox();
    ///new PlayerScript_littlest_pet_shop_achiev_reward();
    new PlayerScript_crazy_for_cats_achievement();
    new playerscript_for_special_missions_quests();
    new playerscript_for_flash_of_bronze_quests();
    new playerscript_free_for_all_more_for_me_achievement();
    new playerscript_for_timeless_tour();
    new playerscript_for_darkportal_movie();
    new playerscript_for_noble_event_quest();
    new playerscript_and_chew_mana_buns_achievement();
    new playerscript_for_five_missions_quests();
    new playerscript_for_my_very_own_castle();
}
#endif
