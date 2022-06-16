////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "broken_shore.hpp"
#include "BrokenIslesPCH.h"
#include "ContributionMgr.hpp"
#include "ScriptMgr.h"
#include "GarbageCollector.h"

namespace BrokenShore
{
    class Contribution_LegionFall : public ContributionScript
    {
        public:
            Contribution_LegionFall() : ContributionScript("Contribution_LegionFall")
            {
                Load();
                m_NeedUpdateBuildings = false;
            }

        public:
            std::map<uint32, std::set<uint32>> m_BuildingXSpellIDs;
            std::map<uint32, Quest const*> m_SpellIDXQuest;

            std::mutex m_GameobjectContributionLock;
            std::map<uint32, std::set<uint64>> m_GameObjectsForContribution;

            std::queue<uint32> m_ForceUpdateTimers;

            bool m_NeedUpdateBuildings;

            enum eSpells
            {
                WellPrepared = 240987
            };

            enum eWorldQuests
            {
                CathedralOfEternalNight_FelBeast            = 46867,
                CathedralOfEternalNight_InfernalDead        = 46868,
                CathedralOfEternalNight_MistressOfBlades    = 46844,
                CathedralOfEternalNight_MotherOfFlame       = 46865,
                CathedralOfEternalNight_TemptressOfPain     = 46869,

                Apocron                                     = 47061,
                Brutallus                                   = 46947,
                Malificus                                   = 46948,
                Sivash                                      = 46945
            };

            std::vector<uint32> m_CathedralWorldQuests =
            {
                eWorldQuests::CathedralOfEternalNight_TemptressOfPain,
                eWorldQuests::CathedralOfEternalNight_MistressOfBlades,
                eWorldQuests::CathedralOfEternalNight_FelBeast,
                eWorldQuests::CathedralOfEternalNight_MotherOfFlame,
                eWorldQuests::CathedralOfEternalNight_InfernalDead
            };

            std::vector<uint32> m_WorldBossesWorldQuests =
            {
                eWorldQuests::Sivash,
                eWorldQuests::Brutallus,
                eWorldQuests::Apocron,
                eWorldQuests::Malificus
            };

        public:
            void OnWorldObjectCreate(WorldObject const* p_WorldObject) override
            {
                GameObject const* l_GameObject = p_WorldObject->ToGameObject();
                if (!l_GameObject)
                    return;

                if (std::find(g_BrokenShoreBuildingGameObjects.begin(), g_BrokenShoreBuildingGameObjects.end(), l_GameObject->GetEntry()) == g_BrokenShoreBuildingGameObjects.end())
                    return;

                m_GameobjectContributionLock.lock();

                switch (l_GameObject->GetEntry())
                {
                    case eGameObjects::GobMageTower:
                    {
                        m_GameObjectsForContribution[eBrokenShoreContributions::MageTower].insert(l_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::GobCommandCenter:
                    {
                        m_GameObjectsForContribution[eBrokenShoreContributions::CommandCenter].insert(l_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::GobNetherDisruptor:
                    {
                        m_GameObjectsForContribution[eBrokenShoreContributions::NetherDisruptor].insert(l_GameObject->GetGUID());
                        break;
                    }
                    default:
                        break;
                }

                m_GameobjectContributionLock.unlock();

                m_NeedUpdateBuildings = true;
            }

            void OnChangeState(Contribution const* p_Contribution) override
            {
                switch (p_Contribution->State)
                {
                    case eContributionStates::Building:
                    {
                        break;
                    }
                    case eContributionStates::Active:
                    {
                        switch (p_Contribution->ID)
                        {
                            case eBrokenShoreContributions::CommandCenter:
                                ActivateCommandCenter();
                                break;
                            case eBrokenShoreContributions::NetherDisruptor:
                                ActivateNetherDisruptor();
                                break;
                            default:
                                break;
                        }

                        /// Add Primary buff to all players in Broken Isles
                        auto l_BuffItr = p_Contribution->BuffData.find(0);
                        if (l_BuffItr == p_Contribution->BuffData.end())
                            break;

                        ManagedWorldStateBuffEntry const* l_ManagedWorldStateBuffEntry = l_BuffItr->second;
                        if (!l_ManagedWorldStateBuffEntry)
                            break;

                        uint32 l_BuffID = l_ManagedWorldStateBuffEntry->BuffSpellID;

                        SessionMap const& l_Sessions = sWorld->GetAllSessions();
                        sWorld->GetSessionsLock().acquire_read();
                        for (SessionMap::const_iterator l_Itr = l_Sessions.begin(); l_Itr != l_Sessions.end(); ++l_Itr)
                        {
                            if (Player* l_Player = l_Itr->second->GetPlayer())
                            {
                                if (!l_Player->IsInWorld() || l_Player->GetMapId() != 1220)
                                    continue;

                                l_Player->AddCriticalOperation([l_Player, l_BuffID]() -> void
                                {
                                    l_Player->CastSpell(l_Player, l_BuffID, true);
                                });
                            }
                        }
                        sWorld->GetSessionsLock().release();
                        break;
                    }
                    case eContributionStates::UnderAttack:
                    {
                        uint32 l_ForceUpdateTimer = p_Contribution->GetHalfCurrentStateTimer();
                        if (!l_ForceUpdateTimer)
                            break;

                        m_ForceUpdateTimers.push(l_ForceUpdateTimer);
                        break;
                    }
                    case eContributionStates::Destroyed:
                    {
                        /// Remove Primary and Secondary buff from all players in Broken Isles
                        std::set<uint32> l_Buffs;
                        std::set<uint32> l_QuestIDs;

                        auto l_Itr = m_BuildingXSpellIDs.find(p_Contribution->ID);
                        if (l_Itr == m_BuildingXSpellIDs.end())
                            break;

                        for (uint32 l_SpellID : l_Itr->second)
                        {
                            l_Buffs.insert(l_SpellID);

                            auto l_QuestItr = m_SpellIDXQuest.find(l_SpellID);
                            if (l_QuestItr == m_SpellIDXQuest.end())
                                continue;

                            if (!l_QuestItr->second)
                                continue;

                            l_QuestIDs.insert(l_QuestItr->second->GetQuestId());
                        }

                        SessionMap const& l_Sessions = sWorld->GetAllSessions();
                        sWorld->GetSessionsLock().acquire_read();
                        for (SessionMap::const_iterator l_Itr = l_Sessions.begin(); l_Itr != l_Sessions.end(); ++l_Itr)
                        {
                            if (Player* l_Player = l_Itr->second->GetPlayer())
                            {
                                if (!l_Player->IsInWorld() || l_Player->GetMapId() != 1220)
                                    continue;

                                l_Player->AddCriticalOperation([l_Player, l_Buffs, l_QuestIDs]() -> void
                                {
                                    for (uint32 l_BuffID : l_Buffs)
                                        l_Player->RemoveAura(l_BuffID);

                                    for (uint32 l_QuestID : l_QuestIDs)
                                        l_Player->RemoveActiveQuest(l_QuestID);
                                });
                            }
                        }
                        sWorld->GetSessionsLock().release();

                        break;
                    }
                    default:
                        break;
                }

                UpdateCurrentDailyQuest();
                m_NeedUpdateBuildings = true;
            }

            void OnPlayerContribute(Player* p_Player, Contribution const* p_Contribution) override
            {
                switch (p_Contribution->ID)
                {
                    case eBrokenShoreContributions::MageTower:
                        p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_LEGIONFALL_BUILDING_MAGE_TOWER, 1);
                        break;
                    case eBrokenShoreContributions::CommandCenter:
                        p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_LEGIONFALL_BUILDING_COMMAND_TOWER, 1);
                        break;
                    case eBrokenShoreContributions::NetherDisruptor:
                        p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_LEGIONFALL_BUILDING_NETHER_TOWER, 1);
                        break;
                    default:
                        break;
                }
            }

            void Update(uint32 const p_GameTime) override
            {
                bool l_Updated = false;
                if (m_NeedUpdateBuildings)
                {
                    UpdateBuildings();
                    l_Updated = true;
                    m_NeedUpdateBuildings = false;
                }

                if (m_ForceUpdateTimers.empty())
                    return;

                uint32 l_NextForceUpdate = m_ForceUpdateTimers.front();
                if (l_NextForceUpdate < p_GameTime)
                    return;

                m_ForceUpdateTimers.pop();

                if (!l_Updated)
                    UpdateBuildings();
            }

            void OnReloadQuestTemplates() override
            {
                Load();
            }

            void OnPlayerLogin(Player* p_Player) override
            {
                /// Remove all Buffs from currently inactive buildings
                for (uint32 l_ContributionID : g_BrokenShoreBuildingContributions)
                {
                    Contribution l_Contribution = sContributionMgr->GetContribution(l_ContributionID);
                    if (!l_Contribution)
                        continue;

                    uint32 l_Except = 0;
                    if (l_Contribution.IsActive())
                    {
                        auto l_BuffItr = l_Contribution.BuffData.find(l_Contribution.OccurenceValue);
                        if (l_BuffItr != l_Contribution.BuffData.end())
                        {
                            if (ManagedWorldStateBuffEntry const* l_ManagedWorldStateBuffEntry = l_BuffItr->second)
                                l_Except = l_ManagedWorldStateBuffEntry->BuffSpellID;
                        }
                    }

                    auto l_Itr = m_BuildingXSpellIDs.find(l_ContributionID);
                    if (l_Itr == m_BuildingXSpellIDs.end())
                        continue;

                    for (uint32 l_SpellID : l_Itr->second)
                    {
                        if (l_SpellID == l_Except)
                            continue;

                        p_Player->RemoveAura(l_SpellID);

                        auto l_QuestItr = m_SpellIDXQuest.find(l_SpellID);
                        if (l_QuestItr == m_SpellIDXQuest.end())
                            continue;

                        if (!l_QuestItr->second)
                            continue;

                        p_Player->RemoveActiveQuest(l_QuestItr->second->GetQuestId());
                    }
                }
            }

            void OnPlayerEnterMap(Player* p_Player, Map* p_Map) override
            {
                /// Remove all buffs from buildings
                if (!IsBrokenIsles(p_Map))
                {
                    for (auto l_Itr : m_SpellIDXQuest)
                        p_Player->RemoveAura(l_Itr.first);

                    return;
                }

                std::set<uint32> l_DailiesSet;
                std::vector<uint32> l_Dailies = p_Player->GetDynamicValues(PLAYER_DYNAMIC_FIELD_DAILY_QUESTS_COMPLETED);
                for (uint32 l_QuestID : l_Dailies)
                    l_DailiesSet.insert(l_QuestID);

                /// Add primary buff for Active buildings (And Secondary if the player already done the quest this day)
                for (uint32 const& l_ContributionID : g_BrokenShoreBuildingContributions)
                {
                    Contribution l_Contribution = sContributionMgr->GetContribution(l_ContributionID);
                    if (!l_Contribution || !l_Contribution.IsActive())
                        continue;

                    std::set<uint32> l_OccurenceValues = { l_Contribution.OccurenceValue, 0 };

                    for (uint32 const& l_OccurenceValue : l_OccurenceValues)
                    {
                        auto l_BuffItr = l_Contribution.BuffData.find(l_OccurenceValue);
                        if (l_BuffItr == l_Contribution.BuffData.end())
                            continue;

                        ManagedWorldStateBuffEntry const* l_ManagedWorldStateBuffEntry = l_BuffItr->second;
                        if (!l_ManagedWorldStateBuffEntry)
                            continue;

                        if (l_OccurenceValue)
                        {
                            auto l_QuestItr = m_SpellIDXQuest.find(l_ManagedWorldStateBuffEntry->BuffSpellID);
                            if (l_QuestItr == m_SpellIDXQuest.end())
                                continue;

                            if (!l_QuestItr->second)
                                continue;

                            if (l_DailiesSet.find(l_QuestItr->second->GetQuestId()) == l_DailiesSet.end())
                                continue;
                        }

                        p_Player->CastSpell(p_Player, l_ManagedWorldStateBuffEntry->BuffSpellID, true);
                    }
                }

                if (p_Map->GetId() != 1220)
                    p_Player->RemoveAura(eSpells::WellPrepared);
            }

            static void PlayerDisruptedPortal(uint64 p_GUID)
            {
                if (g_DailyPortals.find(p_GUID) == g_DailyPortals.end())
                    g_DailyPortals[p_GUID] = 0;

                g_DailyPortals[p_GUID]++;
            }

            static bool PlayerCanDisruptPortal(uint64 p_GUID)
            {
                auto l_Itr = g_DailyPortals.find(p_GUID);
                return (l_Itr == g_DailyPortals.end() || l_Itr->second < 50);
            }

        private:
            void UpdateBuildings()
            {
                m_GameobjectContributionLock.lock();
                for (uint32 const& l_ContributionID : g_BrokenShoreBuildingContributions)
                {
                    Contribution l_Contribution = sContributionMgr->GetContribution(l_ContributionID);
                    if (!l_Contribution)
                        continue;

                    std::set<uint64> l_RemoveList;

                    for (uint64 const& l_GameObjectGUID : m_GameObjectsForContribution[l_ContributionID])
                    {
                        GameObject* l_GameObject = sObjectAccessor->FindGameObject(l_GameObjectGUID);
                        if (!l_GameObject || !l_GameObject->IsInWorld())
                        {
                            l_RemoveList.insert(l_GameObjectGUID);
                            continue;
                        }

                        GameObjectTemplate const* l_GameObjectTemplate = l_GameObject->GetGOInfo();
                        if (!l_GameObjectTemplate)
                            continue;

                        DestructibleModelDataEntry const* l_DestructibleModelDataEntry = sDestructibleModelDataStore.LookupEntry(l_GameObjectTemplate->destructibleBuilding.DestructibleModelRec);
                        if (!l_DestructibleModelDataEntry)
                            continue;

                        uint32 l_DisplayId = l_GameObject->GetDisplayId();

                        l_GameObject->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DAMAGED | GameObjectFlags::GO_FLAG_DESTROYED);

                        switch (l_Contribution.State)
                        {
                            case eContributionStates::Building:
                            {
                                l_DisplayId = l_DestructibleModelDataEntry->SmokeDisplayId;
                                l_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DAMAGED | GameObjectFlags::GO_FLAG_DESTROYED);
                                break;
                            }
                            case eContributionStates::Active:
                            {
                                l_DisplayId = l_DestructibleModelDataEntry->RebuildingDisplayId;
                                l_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DESTROYED);
                                break;
                            }
                            case eContributionStates::UnderAttack:
                            {
                                l_DisplayId = l_DestructibleModelDataEntry->RebuildingDisplayId;
                                l_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DESTROYED);

                                if (l_Contribution.IsAfterHalfOfCurrentState())
                                {
                                    l_DisplayId = l_DestructibleModelDataEntry->DestroyedDisplayId;
                                    l_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DAMAGED);
                                }

                                break;
                            }
                            case eContributionStates::Destroyed:
                            {
                                l_DisplayId = l_DestructibleModelDataEntry->DamagedDisplayId;
                                break;
                            }
                            default:
                                break;
                        }

                        l_GameObject->SetDisplayId(l_DisplayId);
                    }

                    for (uint64 const& l_GUID : l_RemoveList)
                        m_GameObjectsForContribution[l_ContributionID].erase(l_GUID);
                }
                m_GameobjectContributionLock.unlock();
            }

            bool IsBrokenIsles(Map const* p_Map)
            {
                switch (p_Map->GetId())
                {
                    case 1220: ///< Continent

                    case 1456: ///< Eye of Azshara
                    case 1458: ///< Neltharion's Lair
                    case 1466: ///< Darkheart Thicket
                    case 1477: ///< Halls of Valor
                    case 1492: ///< Maw Of Souls
                    case 1493: ///< Vault of the Wardens
                    case 1516: ///< The Arcway
                    case 1544: ///< Assault of Violet Hold
                    case 1571: ///< Court of Stars
                    case 1651: ///< Return to Karazhan
                    case 1677: ///< Cathedral of Eternal Night

                    case 1520: ///< The Emerald Nightmare
                    case 1648: ///< Trial of Valor
                    case 1530: ///< The Nighthold
                    case 1676: ///< Tomb of Sargeras

                    case 1704: ///< Val'sharah Assault
                    case 1705: ///< Azsuna Assault
                    case 1706: ///< Highmountain Assault
                    case 1707: ///< Stormheim Assault
                        return true;
                    default:
                        return false;
                }

                return false;
            }

            void UpdateCurrentDailyQuest()
            {
                g_CurrentDailyQuest.clear();

                for (uint32 const& l_ContributionID : g_BrokenShoreBuildingContributions)
                {
                    Contribution l_Contribution = sContributionMgr->GetContribution(l_ContributionID);
                    if (!l_Contribution || !l_Contribution.IsActive())
                        continue;

                    auto l_BuffItr = l_Contribution.BuffData.find(l_Contribution.OccurenceValue);
                    if (l_BuffItr == l_Contribution.BuffData.end())
                        continue;

                    ManagedWorldStateBuffEntry const* l_ManagedWorldStateBuffEntry = l_BuffItr->second;
                    if (!l_ManagedWorldStateBuffEntry)
                        continue;

                    auto l_QuestItr = m_SpellIDXQuest.find(l_ManagedWorldStateBuffEntry->BuffSpellID);
                    if (l_QuestItr == m_SpellIDXQuest.end())
                        continue;

                    if (!l_QuestItr->second)
                        continue;

                    g_CurrentDailyQuest.insert(l_QuestItr->second->GetQuestId());
                }
            }

            void Load()
            {
                m_SpellIDXQuest.clear();
                m_BuildingXSpellIDs.clear();

                std::set<uint32> l_SpellIDs;
                for (uint32 l_Index = 0; l_Index < sManagedWorldStateBuffStore.GetNumRows(); ++l_Index)
                {
                    ManagedWorldStateBuffEntry const* l_ManagedWorldStateBuffEntry = sManagedWorldStateBuffStore.LookupEntry(l_Index);
                    if (!l_ManagedWorldStateBuffEntry)
                        continue;

                    l_SpellIDs.insert(l_ManagedWorldStateBuffEntry->BuffSpellID);

                    m_BuildingXSpellIDs[l_ManagedWorldStateBuffEntry->ManagedWorldStateID].insert(l_ManagedWorldStateBuffEntry->BuffSpellID);
                }

                for (uint32 l_SpellID : l_SpellIDs)
                    m_SpellIDXQuest[l_SpellID] = 0;

                for (auto l_Itr : sObjectMgr->GetQuestTemplates())
                {
                    Quest* l_Quest = l_Itr.second;
                    if (!l_Quest)
                        continue;

                    if (l_SpellIDs.find(l_Quest->GetRewSpellCast()) == l_SpellIDs.end())
                        continue;

                    m_SpellIDXQuest[l_Quest->GetRewSpellCast()] = l_Quest;
                }

                UpdateCurrentDailyQuest();
            }

            void ActivateCommandCenter()
            {
                if (m_CathedralWorldQuests.empty())
                    return;

                bool l_Active = false;
                for (uint32 l_QuestID : m_CathedralWorldQuests)
                {
                    if (sWorldQuestGenerator->HasQuestActive(l_QuestID))
                        l_Active = true;
                }

                if (l_Active)
                    return;

                Contribution l_Contribution = sContributionMgr->GetContribution(eBrokenShoreContributions::CommandCenter);
                if (!l_Contribution)
                    return;

                MS::WorldQuest::Template const* l_Template = MS::WorldQuest::Template::GetTemplate(m_CathedralWorldQuests[l_Contribution.OccurenceValue % 5]);
                if (!l_Template)
                    return;

                sWorldQuestGenerator->AddQuest(l_Template, l_Contribution.StartTime, sContributionMgr->CalculateActiveDuration(&l_Contribution), true);
            }

            void ActivateNetherDisruptor()
            {
                if (m_WorldBossesWorldQuests.empty())
                    return;

                bool l_Active = false;
                for (uint32 l_QuestID : m_WorldBossesWorldQuests)
                {
                    if (sWorldQuestGenerator->HasQuestActive(l_QuestID))
                        l_Active = true;
                }

                if (l_Active)
                    return;

                Contribution l_Contribution = sContributionMgr->GetContribution(eBrokenShoreContributions::NetherDisruptor);
                if (!l_Contribution)
                    return;

                MS::WorldQuest::Template const* l_Template = MS::WorldQuest::Template::GetTemplate(m_WorldBossesWorldQuests[l_Contribution.OccurenceValue % 4]);
                if (!l_Template)
                    return;

                sWorldQuestGenerator->AddQuest(l_Template, l_Contribution.StartTime, sContributionMgr->CalculateActiveDuration(&l_Contribution), true);
            }
    };

    class condition_broken_shore_buffs : public ConditionScript
    {
        public:
            condition_broken_shore_buffs() : ConditionScript("condition_broken_shore_buffs") { }

            bool OnConditionCheck(Condition const* p_Condition, ConditionSourceInfo& /*p_SourceInfo*/) override
            {
                return std::find(g_CurrentDailyQuest.begin(), g_CurrentDailyQuest.end(), p_Condition->SourceEntry) != g_CurrentDailyQuest.end();
            }
    };

    /// Called by Light as a Feather - 240980
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_light_as_a_feather : public SpellScriptLoader
    {
        public:
            spell_broken_shore_light_as_a_feather() : SpellScriptLoader("spell_broken_shore_light_as_a_feather") { }

            class spell_broken_shore_light_as_a_feather_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_broken_shore_light_as_a_feather_AuraScript);

                enum eSpells
                {
                    LightAsAFeather = 241538
                };

                void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    if (!l_Caster->IsMounted())
                    {
                        l_Caster->RemoveAura(eSpells::LightAsAFeather);
                        return;
                    }

                    if (!l_Caster->HasAura(eSpells::LightAsAFeather))
                        l_Caster->CastSpell(l_Caster, eSpells::LightAsAFeather, true);
                }

                void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->RemoveAura(eSpells::LightAsAFeather);
                }

                void Register() override
                {
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_broken_shore_light_as_a_feather_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_light_as_a_feather_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_broken_shore_light_as_a_feather_AuraScript();
            }
    };

    /// Called for War Effort - 239966
    /// Last Update 7.3.5 - Build 25996
    class PlayerScript_War_Effort : public PlayerScript
    {
        public:
            PlayerScript_War_Effort() : PlayerScript("PlayerScript_War_Effort") { }

            enum eSpells
            {
                WarEffort = 239966
            };

            void OnModifyCurrency(Player* p_Player, uint32 p_CurrencyID, int32& p_Count, bool p_IgnoreMultipliers) override
            {
                if (p_IgnoreMultipliers)
                    return;

                if (p_CurrencyID != CurrencyTypes::CURRENCY_TYPE_LEGIONFALL_WAR_SUPLIES)
                    return;

                if (!roll_chance_f(25.0f) || !p_Player->HasAura(eSpells::WarEffort))
                    return;

                p_Count *= 2;
            }
    };

    /// Called for Netherstorm - 239969
    /// Last Update 7.3.5 - Build 25996
    class PlayerScript_Netherstorm : public PlayerScript
    {
        public:
            PlayerScript_Netherstorm() : PlayerScript("PlayerScript_Netherstorm") { }

            enum eSpells
            {
                Netherstorm = 239969
            };

            void OnModifyCurrency(Player* p_Player, uint32 p_CurrencyID, int32& p_Count, bool p_IgnoreMultipliers) override
            {
                if (p_IgnoreMultipliers)
                    return;

                if (p_CurrencyID != CurrencyTypes::CURRENCY_TYPE_NETHERSHARD)
                    return;

                if (!roll_chance_f(25.0f) || !p_Player->HasAura(eSpells::Netherstorm))
                    return;

                p_Count *= 2;
            }
    };

    /// Called by Reinforced Reins - 240985
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_reinforced_reins : public SpellScriptLoader
    {
        public:
            spell_broken_shore_reinforced_reins() : SpellScriptLoader("spell_broken_shore_reinforced_reins") { }

            class spell_broken_shore_reinforced_reins_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_broken_shore_reinforced_reins_AuraScript);

                enum eSpells
                {
                    ReinforcedReins = 241540
                };

                void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    if (!l_Caster->IsMounted())
                    {
                        l_Caster->RemoveAura(eSpells::ReinforcedReins);
                        return;
                    }

                    if (!l_Caster->HasAura(eSpells::ReinforcedReins))
                        l_Caster->CastSpell(l_Caster, eSpells::ReinforcedReins, true);
                }

                void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->RemoveAura(eSpells::ReinforcedReins);
                }

                void Register() override
                {
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_broken_shore_reinforced_reins_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_reinforced_reins_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_broken_shore_reinforced_reins_AuraScript();
            }
    };

    /// Called by Fel Treasures - 239645
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_fel_treasures : public SpellScriptLoader
    {
        public:
            spell_broken_shore_fel_treasures() : SpellScriptLoader("spell_broken_shore_fel_treasures") { }

            class spell_broken_shore_fel_treasures_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_broken_shore_fel_treasures_AuraScript);

                void HandleAfterApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->SetInPhase(m_scriptSpellId, true, true);
                }

                void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->SetInPhase(m_scriptSpellId, true, false);
                }

                void Register() override
                {
                    AfterEffectApply += AuraEffectRemoveFn(spell_broken_shore_fel_treasures_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_fel_treasures_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_broken_shore_fel_treasures_AuraScript();
            }
    };

    /// Called by Fel Treasures - 239648
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_forces_of_the_order : public SpellScriptLoader
    {
        public:
            spell_broken_shore_forces_of_the_order() : SpellScriptLoader("spell_broken_shore_forces_of_the_order") { }

            class spell_broken_shore_forces_of_the_order_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_broken_shore_forces_of_the_order_AuraScript);

                enum eMissions
                {
                    EliteStrikeTheSentinax          = 1697,
                    EliteStrikeLegionSupplyCache    = 1698,
                    EliteStrikeLostTemple           = 1699
                };

                enum eSpells
                {
                    DancingBlade            = 241861,
                    Landslide               = 241888,
                    AncientManashards       = 238541,
                    ThornyEntanglement      = 241750,
                    ProtectionOfTheCourt    = 241695,
                    EyeOfTheKirinTor        = 241910,
                    EyeOfTheKirinTorBuff    = 241911
                };

                enum eZoneIDs
                {
                    Azsuna              = 7334,
                    Stormheim           = 7541,
                    Highmountain        = 7503,
                    Suramar             = 7637,
                    Valsharah           = 7558,
                    TheBrokenShore      = 7543
                };

                void HandleAfterApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    MS::Garrison::Manager* l_Garrison = l_Player->GetGarrison();
                    if (!l_Garrison)
                        return;

                    Contribution l_Contribution = sContributionMgr->GetContribution(eBrokenShoreContributions::CommandCenter);
                    if (!l_Contribution)
                        return;

                    uint32 l_Time = sWorld->GetGameTime();
                    uint32 l_OfferDuration = l_Contribution.GetEndStateTime() + (l_Contribution.State == eContributionStates::Active ? 24 * TimeConstants::HOUR : 0) - l_Time;

                    l_Garrison->AddMission(eMissions::EliteStrikeTheSentinax, l_OfferDuration);
                    l_Garrison->AddMission(eMissions::EliteStrikeLegionSupplyCache, l_OfferDuration);
                    l_Garrison->AddMission(eMissions::EliteStrikeLostTemple, l_OfferDuration);
                }

                void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
                {
                    AuraEffect* l_AuraEffect = const_cast<AuraEffect*>(p_AuraEffect);

                    Unit* l_Target = GetTarget();
                    if (!l_Target || !l_Target->isInCombat())
                    {
                        l_AuraEffect->SetAmount(45);
                        return;
                    }

                    l_AuraEffect->SetAmount(p_AuraEffect->GetAmount() + 5);

                    if (l_AuraEffect->GetAmount() < MINUTE)
                        return;

                    if (!roll_chance_f(10.0f + ((l_AuraEffect->GetAmount() - MINUTE) / 5)))
                        return;

                    l_AuraEffect->SetAmount(0);

                    switch (l_Target->GetZoneId())
                    {
                        case eZoneIDs::Azsuna:
                            l_Target->CastSpell(l_Target, eSpells::ProtectionOfTheCourt, true);
                            break;
                        case eZoneIDs::Valsharah:
                            l_Target->CastSpell(l_Target, eSpells::ThornyEntanglement, true);
                            break;
                        case eZoneIDs::Highmountain:
                            if (!l_Target->getVictim())
                                break;

                            l_Target->CastSpell(l_Target->getVictim(), eSpells::Landslide, true);
                            break;
                        case eZoneIDs::Stormheim:
                            if (!l_Target->getVictim())
                                break;

                            l_Target->CastSpell(l_Target->getVictim(), eSpells::DancingBlade, true);
                            break;
                        case eZoneIDs::Suramar:
                            l_Target->CastSpell(l_Target, eSpells::AncientManashards, true);
                            break;
                        case eZoneIDs::TheBrokenShore:
                        {
                            if (l_Target->HasAura(eSpells::EyeOfTheKirinTorBuff))
                                break;

                            std::list<AreaTrigger*> l_ATs;
                            l_Target->GetAreaTriggerListWithSpellIDInRange(l_ATs, eSpells::EyeOfTheKirinTor, 15.0f);

                            if (l_ATs.size())
                                break;

                            l_Target->CastSpell(l_Target, eSpells::EyeOfTheKirinTor, true);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void Register() override
                {
                    AfterEffectApply += AuraEffectRemoveFn(spell_broken_shore_forces_of_the_order_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_broken_shore_forces_of_the_order_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_broken_shore_forces_of_the_order_AuraScript();
            }
    };

    /// Called by Epic Hunter - 239647
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_epic_hunter : public SpellScriptLoader
    {
        public:
            spell_broken_shore_epic_hunter() : SpellScriptLoader("spell_broken_shore_epic_hunter") { }

            class spell_broken_shore_epic_hunter_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_broken_shore_epic_hunter_AuraScript);

                void HandleAfterApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->SetInPhase(m_scriptSpellId, true, true);
                }

                void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->SetInPhase(m_scriptSpellId, true, false);
                }

                void Register() override
                {
                    AfterEffectApply += AuraEffectRemoveFn(spell_broken_shore_epic_hunter_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_epic_hunter_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_broken_shore_epic_hunter_AuraScript();
            }
    };

    /// Called by Disrupting Nether Portal - 240605
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_disrupting_nether_portal : public SpellScriptLoader
    {
        public:
            spell_broken_shore_disrupting_nether_portal() : SpellScriptLoader("spell_broken_shore_disrupting_nether_portal") { }

            class spell_broken_shore_disrupting_nether_portal_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_broken_shore_disrupting_nether_portal_SpellScript);

                enum eRegeants
                {
                    NetherPortalDisruptor = 147775
                };

                enum eCreatures
                {
                    DementedShivarra        = 121090,
                    MalformedTerrorguard    = 121056,
                    UnstableAbyssal         = 121051,
                    RuinousOverfiend        = 121108,
                    LambentFelhunter        = 121077,
                    DerangedSuccubus        = 121073,
                    VolatileImp             = 121068,
                    BalefulKnightCaptain    = 121049,
                    AnomalousObserver       = 121092,
                    WarpedVoidlord          = 121088
                };

                enum ePhases
                {
                    NetherPortalPhases = 239647
                };

                std::vector<uint32> m_NetherPortalCreatures =
                {
                    eCreatures::DementedShivarra,
                    eCreatures::MalformedTerrorguard,
                /// eCreatures::UnstableAbyssal,        ///< Missing Sniff
                    eCreatures::RuinousOverfiend,
                    eCreatures::LambentFelhunter,
                    eCreatures::DerangedSuccubus,
                    eCreatures::VolatileImp,
                    eCreatures::BalefulKnightCaptain,
                    eCreatures::AnomalousObserver,
                    eCreatures::WarpedVoidlord
                };

                SpellCastResult HandleCheckCast()
                {
                    Unit* l_Caster = GetOriginalCaster();
                    if (!l_Caster)
                        return SPELL_FAILED_DONT_REPORT;

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return SPELL_FAILED_DONT_REPORT;

                    if (!Contribution_LegionFall::PlayerCanDisruptPortal(l_Player->GetGUID()))
                        return SPELL_FAILED_NOT_READY;

                    if (!l_Player->HasItemCount(eRegeants::NetherPortalDisruptor, 1))
                        return SPELL_FAILED_REAGENTS;

                    l_Player->DestroyItemCount(eRegeants::NetherPortalDisruptor, 1, true);
                    return SPELL_CAST_OK;
                }

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_OriginalCaster = GetOriginalCaster();
                    if (!l_Caster || !l_OriginalCaster)
                        return;

                    Contribution_LegionFall::PlayerDisruptedPortal(l_OriginalCaster->GetGUID());

                    if (m_NetherPortalCreatures.empty())
                        return;

                    uint8 l_Count = 0;

                    Creature* l_Summon = nullptr;

                    while (!l_Summon && l_Count < 10)
                    {
                        l_Count++;
                        l_Summon = l_Caster->SummonCreature(m_NetherPortalCreatures[urand(0, m_NetherPortalCreatures.size() - 1)], *l_Caster, TEMPSUMMON_TIMED_DESPAWN, 6 * MINUTE * IN_MILLISECONDS);
                    }

                    if (l_Summon)
                    {
                        l_Summon->SetInPhase(ePhases::NetherPortalPhases, true, true);

                        l_Caster->SetInPhase(ePhases::NetherPortalPhases, true, false);
                        l_Caster->AddDelayedEvent([l_Caster]() -> void
                        {
                            l_Caster->SetInPhase(ePhases::NetherPortalPhases, true, true);
                        }, 30 * MINUTE * IN_MILLISECONDS);
                    }
                }

                void Register() override
                {
                    OnCheckCast += SpellCheckCastFn(spell_broken_shore_disrupting_nether_portal_SpellScript::HandleCheckCast);
                    AfterCast += SpellCastFn(spell_broken_shore_disrupting_nether_portal_SpellScript::HandleAfterCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_broken_shore_disrupting_nether_portal_SpellScript();
            }
    };

    /// Called by Create Ring - 240418
    /// Called by Create Trinket - 240421
    /// Called by Create Glove - 240422
    /// Called by Create Legs - 240415
    /// Called by Create Cloak - 240420
    /// Called by Create Neck - 240419
    /// Called by Create Bracer - 240411
    /// Called by Create Belt - 240417
    /// Called by Create Helm - 240414
    /// Called by Create Shoulder - 240416
    /// Called by Create Boot - 240413
    /// Called by Create Chest - 240412
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_create_stuff : public SpellScriptLoader
    {
        public:
            spell_broken_shore_create_stuff() : SpellScriptLoader("spell_broken_shore_create_stuff") { }

            class spell_broken_shore_create_stuff_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_broken_shore_create_stuff_SpellScript);

                uint32 m_ItemID = 0;

                SpellCastResult HandleCheckCast()
                {
                    Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                    if (!l_Player)
                        return SpellCastResult::SPELL_FAILED_ERROR;

                    LootStore& l_LootStore = LootTemplates_Spell;
                    LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(GetSpellInfo()->Id);
                    if (l_LootTemplate == nullptr)
                        return SpellCastResult::SPELL_FAILED_ERROR;

                    std::list<ItemTemplate const*> l_LootTable;
                    std::vector<uint32> l_Items;
                    l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, true);

                    if (l_LootTable.empty())
                        return SpellCastResult::SPELL_FAILED_ERROR;

                    uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                    for (ItemTemplate const* l_Template : l_LootTable)
                    {
                        bool l_BreakLoop = false;
                        switch (l_Template->InventoryType)
                        {
                            case InventoryType::INVTYPE_HEAD:
                            case InventoryType::INVTYPE_SHOULDERS:
                            case InventoryType::INVTYPE_CHEST:
                            case InventoryType::INVTYPE_WAIST:
                            case InventoryType::INVTYPE_LEGS:
                            case InventoryType::INVTYPE_FEET:
                            case InventoryType::INVTYPE_WRISTS:
                            case InventoryType::INVTYPE_HANDS:
                            {
                                if (l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
                                {
                                    m_ItemID = l_Template->ItemId;
                                    l_BreakLoop = true;
                                }

                                break;
                            }
                            case InventoryType::INVTYPE_CLOAK:
                            case InventoryType::INVTYPE_NECK:
                            case InventoryType::INVTYPE_FINGER:
                            {
                                m_ItemID = l_Template->ItemId;
                                l_BreakLoop = true;
                                break;
                            }
                            default:
                            {
                                for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_STATS; ++l_I)
                                {
                                    if (l_Template->ItemStat[l_I].ItemStatValue != 0)
                                    {
                                        Stats l_NeededStat = Stats::MAX_STATS;

                                        switch (ItemModType(l_Template->ItemStat[l_I].ItemStatType))
                                        {
                                            case ItemModType::INTELLECT:
                                            {
                                                l_NeededStat = Stats::STAT_INTELLECT;
                                                break;
                                            }
                                            case ItemModType::AGILITY:
                                            {
                                                l_NeededStat = Stats::STAT_AGILITY;
                                                break;
                                            }
                                            case ItemModType::STRENGTH:
                                            {
                                                l_NeededStat = Stats::STAT_STRENGTH;
                                                break;
                                            }
                                            default:
                                                break;
                                        }

                                        if (l_Player->GetPrimaryStat() == l_NeededStat)
                                        {
                                            m_ItemID = l_Template->ItemId;
                                            l_BreakLoop = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        }

                        if (l_BreakLoop)
                            break;
                    }

                    if (!m_ItemID)
                        return SpellCastResult::SPELL_FAILED_ERROR;

                    ItemPosCountVec l_Destination;
                    InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                    if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    {
                        l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }

                    return SpellCastResult::SPELL_CAST_OK;
                }

                void HandleCreateItem(SpellEffIndex p_EffectIndex)
                {
                    PreventHitDefaultEffect(p_EffectIndex);

                    Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                    if (!l_Player || !m_ItemID)
                        return;

                    ItemContext l_Context = ItemContext::Ilvl850;

                    ItemPosCountVec l_Destination;
                    InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                    if (l_Message != InventoryResult::EQUIP_ERR_OK)
                        return;

                    std::vector<uint32> l_RandomPropertiesPool = { 1707, 1692, 1712, 1697, 1677, 1687 };
                    std::vector<uint32> l_BonusList = { 3574, 3528 };
                    l_BonusList.push_back(l_RandomPropertiesPool[urand(0, l_RandomPropertiesPool.size() - 1)]);

                    Item::GenerateItemBonus(m_ItemID, l_Context, l_BonusList, false, l_Player);
                    if (Item* l_Item = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_BonusList))
                    {
                        l_Player->SendNewItem(l_Item, 1, false, true, false, 0, l_Context);
                        l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_BonusList);
                    }
                }

                void Register() override
                {
                    OnCheckCast += SpellCheckCastFn(spell_broken_shore_create_stuff_SpellScript::HandleCheckCast);
                    OnEffectHitTarget += SpellEffectFn(spell_broken_shore_create_stuff_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_LOOT_BONUS);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_broken_shore_create_stuff_SpellScript();
            }
    };

    /// Called by Create Ring - 242862
    /// Called by Create Trinket - 242864
    /// Called by Create Glove - 242858
    /// Called by Create Legs - 242860
    /// Called by Create Cloak - 242857
    /// Called by Create Neck - 242861
    /// Called by Create Bracer - 242842
    /// Called by Create Belt - 242854
    /// Called by Create Helm - 242859
    /// Called by Create Shoulder - 242863
    /// Called by Create Boot - 242855
    /// Called by Create Chest - 242856
    /// Called by Create Relic - 243074
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_create_stuff_relinquished : public SpellScriptLoader
    {
        public:
            spell_broken_shore_create_stuff_relinquished() : SpellScriptLoader("spell_broken_shore_create_stuff_relinquished") { }

            class spell_broken_shore_create_stuff_relinquished_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_broken_shore_create_stuff_relinquished_SpellScript);

                enum eMaps
                {
                    EmeraldNightmare    = 1520,
                    TrialOfValor        = 1648,
                    TheNighthold        = 1530
                };

                enum eSpells
                {
                    CreateLegs      = 242860,
                    CreateBelt      = 242854,
                    CreateTrinket   = 242864,
                    CreateShoulder  = 242863,
                    CreateRing      = 242862,
                    CreateNeck      = 242861,
                    CreateRelic     = 243074,
                    CreateHelm      = 242859,
                    CreateGlove     = 242858,
                    CreateCloak     = 242857,
                    CreateChest     = 242856,
                    CreateBoot      = 242855,
                    CreateBracer    = 242842
                };

                uint32 m_ItemID = 0;

                bool m_LootInit = false;
                std::array<std::vector<uint32>, InventoryType::MAX_INVTYPE> m_Loots;
                std::vector<uint32> m_Relics;

                void GenerateLootLists()
                {
                    std::vector<uint32> l_Maps = *GetChallengeMaps();
                    l_Maps.push_back(eMaps::EmeraldNightmare);
                    l_Maps.push_back(eMaps::TrialOfValor);
                    l_Maps.push_back(eMaps::TheNighthold);
                    for (uint32 l_MapID : l_Maps)
                    {
                        for (uint32 l_LootID : GetItemLootPerMap(l_MapID))
                        {
                            ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_LootID);
                            if (l_Template->GetInventoryType() != INVTYPE_NON_EQUIP)
                            {
                                m_Loots[l_Template->GetInventoryType()].push_back(l_LootID);
                                continue;
                            }

                            if (!l_Template->IsArtifactRelic())
                                continue;

                            m_Relics.push_back(l_LootID);
                        }
                    }

                    m_LootInit = true;
                }

                SpellCastResult HandleCheckCast()
                {
                    if (!m_LootInit)
                        GenerateLootLists();

                    Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                    if (!l_Player)
                        return SpellCastResult::SPELL_FAILED_ERROR;

                    uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                    std::vector<uint32> l_PossibleLoots;

                    InventoryType l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;
                    switch (m_scriptSpellId)
                    {
                         case eSpells::CreateLegs:
                             l_InventoryType = InventoryType::INVTYPE_LEGS;
                            break;
                        case eSpells::CreateBelt:
                            l_InventoryType = InventoryType::INVTYPE_WAIST;
                            break;
                        case eSpells::CreateTrinket:
                            l_InventoryType = InventoryType::INVTYPE_TRINKET;
                            break;
                        case eSpells::CreateShoulder:
                            l_InventoryType = InventoryType::INVTYPE_SHOULDERS;
                            break;
                        case eSpells::CreateRing:
                            l_InventoryType = InventoryType::INVTYPE_FINGER;
                            break;
                        case eSpells::CreateNeck:
                            l_InventoryType = InventoryType::INVTYPE_NECK;
                            break;
                        case eSpells::CreateHelm:
                            l_InventoryType = InventoryType::INVTYPE_HEAD;
                            break;
                        case eSpells::CreateGlove:
                            l_InventoryType = InventoryType::INVTYPE_HANDS;
                            break;
                        case eSpells::CreateCloak:
                            l_InventoryType = InventoryType::INVTYPE_CLOAK;
                            break;
                        case eSpells::CreateChest:
                            l_InventoryType = InventoryType::INVTYPE_CHEST;
                            break;
                        case eSpells::CreateBoot:
                            l_InventoryType = InventoryType::INVTYPE_FEET;
                            break;
                        case eSpells::CreateBracer:
                            l_InventoryType = InventoryType::INVTYPE_WRISTS;
                            break;
                        case eSpells::CreateRelic:
                        default:
                            l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;
                            break;
                    }

                    /// Legendary
                    if (l_Player->CompleteLegendaryActivity(LegendaryActivity::RelinquishedToken, 0, true, l_InventoryType))
                        return SpellCastResult::SPELL_CAST_OK;

                    if (m_scriptSpellId == eSpells::CreateRelic)
                        l_PossibleLoots = m_Relics;
                    else
                        l_PossibleLoots = m_Loots[l_InventoryType];

                    std::random_device l_RandomDevice;
                    std::mt19937 l_RandomGenerator(l_RandomDevice());
                    std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

                    for (uint32 l_LootID : l_PossibleLoots)
                    {
                        ItemTemplate const* l_ItemTemplate = sObjectMgr->GetItemTemplate(l_LootID);
                        if (!l_ItemTemplate || !l_ItemTemplate->IsStuff())
                            continue;

                        if (!l_ItemTemplate->IsUsableBySpecialization(l_SpecID, l_Player->getLevel()))
                            continue;

                        m_ItemID = l_LootID;
                        break;
                    }

                    if (!m_ItemID)
                        return SpellCastResult::SPELL_FAILED_ERROR;

                    ItemPosCountVec l_Destination;
                    InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                    if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    {
                        l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }

                    return SpellCastResult::SPELL_CAST_OK;
                }

                void HandleCreateItem(SpellEffIndex p_EffectIndex)
                {
                    PreventHitDefaultEffect(p_EffectIndex);

                    Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                    if (!l_Player || !m_ItemID)
                        return;

                    ItemContext l_Context = ItemContext::PvPRanked6;

                    ItemPosCountVec l_Destination;
                    InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                    if (l_Message != InventoryResult::EQUIP_ERR_OK)
                        return;

                    std::vector<uint32> l_BonusList = { 3573, 3528 };
                    Item::GenerateItemBonus(m_ItemID, l_Context, l_BonusList, false, l_Player);

                    if (Item* l_Item = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_BonusList))
                    {
                        l_Player->SendNewItem(l_Item, 1, false, true, false, 0, l_Context);
                        l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_BonusList);
                    }
                }

                void Register() override
                {
                    OnCheckCast += SpellCheckCastFn(spell_broken_shore_create_stuff_relinquished_SpellScript::HandleCheckCast);
                    OnEffectHitTarget += SpellEffectFn(spell_broken_shore_create_stuff_relinquished_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_LOOT_BONUS);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_broken_shore_create_stuff_relinquished_SpellScript();
            }
    };

    /// Called by Unstable Tether - 243069
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_unstable_tether : public SpellScriptLoader
    {
        public:
            spell_broken_shore_unstable_tether() : SpellScriptLoader("spell_broken_shore_unstable_tether") { }

            class spell_broken_shore_unstable_tether_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_broken_shore_unstable_tether_AuraScript);

                void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Creature* l_Creature = l_Caster->ToCreature();
                    if (!l_Creature)
                        return;

                    l_Creature->DespawnOrUnsummon(1);
                }

                void Register() override
                {
                    AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_unstable_tether_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_broken_shore_unstable_tether_AuraScript();
            }
    };

    /// Dancing Blade - 241861
    /// Last Update 7.3.5 - Build 25996
    class spell_npc_dancing_blade : public CreatureScript
    {
        public:
            spell_npc_dancing_blade() : CreatureScript("npc_dancing_blade") { }

            struct spell_npc_dancing_bladeAI : public ScriptedAI
            {
                spell_npc_dancing_bladeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

                enum eSpells
                {
                    DancingBlade = 241860
                };

                void IsSummonedBy(Unit* p_Summoner) override
                {
                    if (!p_Summoner)
                        return;

                    me->CastSpell(me, eSpells::DancingBlade, true, nullptr, nullptr, p_Summoner->GetGUID());
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const
            {
                return new spell_npc_dancing_bladeAI(p_Creature);
            }
    };

    /// Called by Landslide - 241888
    /// Last Update 7.3.5 - Build 25996
    class spell_broken_shore_landslide : public SpellScriptLoader
    {
        public:
            spell_broken_shore_landslide() : SpellScriptLoader("spell_broken_shore_landslide") { }

            class spell_broken_shore_landslide_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_broken_shore_landslide_SpellScript);

                enum eSpells
                {
                    Landslide = 241889
                };

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    for (uint8 l_I = 0; l_I < 25; l_I++)
                    {
                        Position l_Pos;
                        l_Caster->SimplePosXYRelocationByAngle(l_Pos, frand(0.0f, 25.0f), frand(- M_PI / 6.0f, M_PI / 6.0f));
                        l_Caster->CastSpell(l_Pos, eSpells::Landslide, true);
                    }
                }

                void Register() override
                {
                    AfterCast += SpellCastFn(spell_broken_shore_landslide_SpellScript::HandleAfterCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_broken_shore_landslide_SpellScript();
            }
    };

    /// Called by Ancient Manashards - 238541
    /// Last Update 7.3.5 - Build 26365
    class spell_broken_shore_ancient_manashards : public SpellScriptLoader
    {
        public:
            spell_broken_shore_ancient_manashards() : SpellScriptLoader("spell_broken_shore_ancient_manashards") { }

            class spell_broken_shore_ancient_manashards_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_broken_shore_ancient_manashards_SpellScript);

                enum eSpells
                {
                    AncientManashards = 238542
                };

                void HandleAfterHit()
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetHitUnit();
                    if (!l_Caster || !l_Target)
                        return;

                    l_Caster->CastSpell(l_Target, eSpells::AncientManashards, true);
                }

                void FilterTargets(std::list<WorldObject*>& p_Targets)
                {
                    if (p_Targets.empty())
                        return;

                    JadeCore::RandomResizeList(p_Targets, 1);
                }

                void Register() override
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_broken_shore_ancient_manashards_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                    AfterHit += SpellHitFn(spell_broken_shore_ancient_manashards_SpellScript::HandleAfterHit);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_broken_shore_ancient_manashards_SpellScript();
            }
    };

    /// Called for War Effort - 239966
    /// Last Update 7.3.5 - Build 25996
    class PlayerScript_Legendary_Questlines : public PlayerScript
    {
        public:
            PlayerScript_Legendary_Questlines() : PlayerScript("PlayerScript_Legendary_Questlines") { }

            enum eQuests
            {
                FashionHistoryAndAPhilosophyOfStyle = 46804,
                /// Cloth
                TheLegendOfTheThreads   = 46678,
                DrapingsOfTheAncients   = 46682,
                TheThreadOfShadow       = 46679,
                TheThreadOfStarlight    = 46680,
                TheThreadOfSouls        = 46681,
                StarweaveAndShadowcloth = 46683,
                /// Leather
                LeatherOfTheAncients    = 46688,
                TheWispAndTheSea        = 46686,
                TheWispAndTheShadow     = 46685,
                TheWispAndTheNightmare  = 46687,
                WispTouchedElderhide    = 46689,
                /// Mail
                MailOfTheAncients       = 46694,
                TheOwlAndTheTraitor     = 46691,
                TheOwlAndTheObservor    = 46693,
                TheOwlAndTheDreadlord   = 46692,
                PrimeWardenscale        = 46695,
                /// Plate
                ArmorOfTheAncients      = 46700,
                TheGodfighter           = 46697,
                TheDragonHunter         = 46698,
                TheHellslayer           = 46699,
                HammerOfForgottenHeroes = 46701
            };

            std::map<uint32, std::vector<uint32>> const m_QuestLinesByLastQuest =
            {
                {
                    eQuests::StarweaveAndShadowcloth,
                    {
                        eQuests::TheLegendOfTheThreads,
                        eQuests::DrapingsOfTheAncients,
                        eQuests::TheThreadOfShadow,
                        eQuests::TheThreadOfStarlight,
                        eQuests::TheThreadOfSouls,
                        eQuests::StarweaveAndShadowcloth
                    }
                },
                {
                    eQuests::WispTouchedElderhide,
                    {
                        eQuests::LeatherOfTheAncients,
                        eQuests::TheWispAndTheSea,
                        eQuests::TheWispAndTheShadow,
                        eQuests::TheWispAndTheNightmare,
                        eQuests::WispTouchedElderhide
                    }
                },
                {
                    eQuests::PrimeWardenscale,
                    {
                        eQuests::MailOfTheAncients,
                        eQuests::TheOwlAndTheTraitor,
                        eQuests::TheOwlAndTheObservor,
                        eQuests::TheOwlAndTheDreadlord,
                        eQuests::PrimeWardenscale
                    }
                },
                {
                    eQuests::HammerOfForgottenHeroes,
                    {
                        eQuests::ArmorOfTheAncients,
                        eQuests::TheGodfighter,
                        eQuests::TheDragonHunter,
                        eQuests::TheHellslayer,
                        eQuests::HammerOfForgottenHeroes
                    }
                },
            };

            void OnQuestReward(Player* p_Player, const Quest* p_Quest) override
            {
                auto l_Itr = m_QuestLinesByLastQuest.find(p_Quest->GetQuestId());
                if (l_Itr == m_QuestLinesByLastQuest.end())
                    return;

                std::ostringstream l_Query;
                l_Query << "DELETE FROM character_queststatus_rewarded WHERE guid = " << p_Player->GetRealGUIDLow() << " AND quest IN (";

                for (uint32 l_QuestId : l_Itr->second)
                {
                    p_Player->RemoveRewardedQuest(l_QuestId);
                    l_Query << l_QuestId << ", ";
                }

                p_Player->RemoveRewardedQuest(eQuests::FashionHistoryAndAPhilosophyOfStyle);
                l_Query << eQuests::FashionHistoryAndAPhilosophyOfStyle << ");";
                CharacterDatabase.PExecute(l_Query.str().c_str());
            }
    };

};

#ifndef __clang_analyzer__
void AddSC_broken_shore()
{
    new BrokenShore::Contribution_LegionFall();
    new BrokenShore::condition_broken_shore_buffs();

    new BrokenShore::spell_broken_shore_light_as_a_feather();
    new BrokenShore::PlayerScript_War_Effort();
    new BrokenShore::PlayerScript_Netherstorm();
    new BrokenShore::spell_broken_shore_reinforced_reins();
    new BrokenShore::spell_broken_shore_fel_treasures();
    new BrokenShore::spell_broken_shore_forces_of_the_order();
    new BrokenShore::spell_broken_shore_epic_hunter();
    new BrokenShore::spell_broken_shore_disrupting_nether_portal();

    new BrokenShore::spell_broken_shore_create_stuff();
    new BrokenShore::spell_broken_shore_create_stuff_relinquished();

    new BrokenShore::spell_broken_shore_unstable_tether();
    new BrokenShore::spell_npc_dancing_blade();
    new BrokenShore::spell_broken_shore_landslide();
    new BrokenShore::spell_broken_shore_ancient_manashards();

    new BrokenShore::PlayerScript_Legendary_Questlines();
}
#endif
