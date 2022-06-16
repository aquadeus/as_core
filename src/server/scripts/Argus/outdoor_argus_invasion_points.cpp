////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "OutdoorPvE.h"
#include "ScenarioMgr.h"
#include "CreatureGroups.h"
#include "outdoor_argus_invasion_points.h"

Player* GetAnyPlayer(Unit* p_Unit)
{
    if (p_Unit->GetAnyOwner() && p_Unit->GetAnyOwner()->ToPlayer())
        return p_Unit->GetAnyOwner()->ToPlayer();
    else if (p_Unit->ToPlayer())
        return p_Unit->ToPlayer();
    else
        return nullptr;
}

void IncreaseScenarioProgress(uint8 p_Count, Scenario* p_Scenario, Player* p_Player, uint32 p_Asset)
{
    for (uint8 i = 0; i < p_Count; i++)
        p_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, p_Asset, 0, 0, p_Player, p_Player);
}

bool IsInvasionPointPOI(uint16 p_AreaPoiID)
{
    return g_InvasionPointDatas.find(p_AreaPoiID) != g_InvasionPointDatas.end();
}

class outdoor_argus_invasion_points : public OutdoorPvEMapScript
{
    public:
        outdoor_argus_invasion_points() : OutdoorPvEMapScript("outdoor_argus_invasion_points", 1779) { }

        struct outdoor_argus_invasion_points_OutdoorPvE : public OutdoorPvEScript
        {
            outdoor_argus_invasion_points_OutdoorPvE(Map* p_Map) : OutdoorPvEScript(p_Map) {}

            // All invasion points
            std::vector<ObjectGuid> m_DemonGuids;
            uint32 m_LastBossId = 0;
            uint8 m_Modifier = 0;
            uint8 m_SubModifier = 0;
            EventMap m_Events;
            // Val
            std::vector<ObjectGuid> m_DemonHunterGuids;
            // Aurinor
            std::vector<ObjectGuid> m_TemporalAnomalyGuids;
            // Cengar
            std::vector<ObjectGuid> m_GiantGuids;
            std::vector<ObjectGuid> m_LavaGuids;
            uint8 m_GiansDied = 0;
            // Sangua
            ObjectGuid m_LegionPortalGuid;
            std::map<uint64, Position> m_WyrmtongueBloodhaulers;
            // Naigtal
            std::vector<ObjectGuid> m_MarshShamblerGuids;

            uint32 GetData(uint32 p_DataId) override
            {
                switch (p_DataId)
                {
                case eGlobalDatas::Data_CurrentModifier:
                {
                    return m_Modifier;
                    break;
                }
                default:
                    return 0;
                }
            }

            std::string GetScriptName() override
            {
                return "outdoor_argus_invasion_points";
            }

            void OnScenarioCreate(uint32 p_ScenarioGuid) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(p_ScenarioGuid);
                if (!l_Scenario)
                    return;

                ChooseRandomModifiers(l_Map);
                ApplyModifiers(l_Map, false, false, true, false);
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                switch (p_Player->GetZoneId())
                {
                case eScenarioZones::Zone_Alluradel:
                {
                    p_Player->CastSpell(p_Player, GreaterInvasionPointMistressAlluradel::eSpells::Spell_InvigoratingMushroom, true);
                    break;
                }
                default:
                    break;
                }

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                ApplyModifiers(l_Map, false, false, false, true, nullptr, p_Player);

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Sangua:
                {
                    p_Player->CastSpell(p_Player, InvasionPointSangua::eSpells::Spell_StartScenarioConversation, true);
                    break;
                }
                default:
                    break;
                }
            }

            void OnCreatureCreateForScript(Creature* p_Creature) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                switch (p_Creature->GetZoneId())
                {
                case eScenarioZones::Zone_Alluradel:
                {
                    p_Creature->CastSpell(p_Creature, GreaterInvasionPointMistressAlluradel::eSpells::Spell_InvigoratingMushroom, true);
                    break;
                }
                default:
                    break;
                }

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                ApplyModifiers(l_Map, true, false, false, false, p_Creature);

                if (p_Creature->isSummon() && !p_Creature->GetUInt64Value(UNIT_FIELD_DEMON_CREATOR) && p_Creature->IsHostileToPlayers() &&
                    p_Creature->GetEntry() != InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler &&
                    p_Creature->GetEntry() != InvasionPointVal::eNpcs::Npc_IceTorrent &&
                    p_Creature->GetEntry() != InvasionPointNaigtal::eNpcs::Npc_MaddeningSpores &&
                    p_Creature->GetEntry() != InvasionPointVal::eNpcs::Npc_Blizzard &&
                    p_Creature->GetEntry() != InvasionPointBonich::eNpcs::Npc_SwarmingInsects)
                {
                    p_Creature->DelayedCastSpell(p_Creature, eGlobalSpells::Spell_DemonSpawn, true, 100);
                }

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    switch (p_Creature->GetEntry())
                    {
                    case InvasionPointVal::eNpcs::Npc_DemonHunter:
                    {
                        m_DemonHunterGuids.push_back(p_Creature->GetGUID());
                        break;
                    }
                    default:
                    {
                        if (p_Creature->isSummon() && !p_Creature->GetUInt64Value(UNIT_FIELD_DEMON_CREATOR) &&
                            p_Creature->IsHostileToPlayers())
                        {
                            m_DemonGuids.push_back(p_Creature->GetGUID());
                        }

                        break;
                    }
                    }

                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    switch (p_Creature->GetEntry())
                    {
                    case InvasionPointAurinor::eNpcs::Npc_TemporalAnomaly:
                    {
                        m_TemporalAnomalyGuids.push_back(p_Creature->GetGUID());
                        break;
                    }
                    default:
                    {
                        if ((p_Creature->isSummon() || p_Creature->GetDBTableGUIDLow()) &&
                            p_Creature->IsHostileToPlayers())
                        {
                            m_DemonGuids.push_back(p_Creature->GetGUID());
                        }

                        break;
                    }
                    }

                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    switch (p_Creature->GetEntry())
                    {
                    case InvasionPointCengar::eNpcs::Npc_MagmaGiant:
                    {
                        m_GiantGuids.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case InvasionPointCengar::eNpcs::Npc_BurningHound:
                    case InvasionPointCengar::eNpcs::Npc_BurstingElemental:
                    case InvasionPointCengar::eNpcs::Npc_ConqueringRiftstalker:
                    case InvasionPointCengar::eNpcs::Npc_FelflameInvader:
                    case InvasionPointCengar::eNpcs::Npc_FelflameSubjugator:
                    case InvasionPointCengar::eNpcs::Npc_WickedCorruptor:
                    case InvasionPointCengar::eNpcs::Npc_SearingInfernal:
                    case InvasionPointCengar::eNpcs::Npc_ClubfistObliterator:
                    case InvasionPointCengar::eNpcs::Npc_DiabolicalChainwarden:
                    case InvasionPointCengar::eNpcs::Npc_FelrageMarauder:
                    case InvasionPointCengar::eNpcs::Npc_FrothingRazorwing:
                    case InvasionPointCengar::eNpcs::Npc_BladeswornDisciple:
                    case InvasionPointCengar::eNpcs::Npc_BlacksoulInquisitor:
                    case InvasionPointCengar::eNpcs::Npc_FelfrenzyBerserker:
                    case InvasionPointCengar::eNpcs::Npc_ChitteringFiend:
                    {
                        m_DemonGuids.push_back(p_Creature->GetGUID());
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    if (p_Creature->getFaction() == 16 || p_Creature->getFaction() == 90)
                    {
                        m_DemonGuids.push_back(p_Creature->GetGUID());
                    }

                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    if (!p_Creature->GetUInt64Value(UNIT_FIELD_DEMON_CREATOR) && p_Creature->IsHostileToPlayers() &&
                        p_Creature->GetEntry() != InvasionPointSangua::eNpcs::Npc_LegionPortal)
                    {
                        m_DemonGuids.push_back(p_Creature->GetGUID());
                    }

                    if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_LegionPortal)
                    {
                        m_LegionPortalGuid = p_Creature->GetGUID();
                    }

                    if ((p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_ClubfistObliterator ||
                        p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_NoxiousShadowstalker ||
                        p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_FelflameInvader ||
                        p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_FelflameSubjugator) && p_Creature->isSummon())
                    {
                        p_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                    }

                    if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler && p_Creature->isSummon())
                    {
                        auto l_Itr = m_WyrmtongueBloodhaulers.find(p_Creature->GetGUID());
                        if (l_Itr == m_WyrmtongueBloodhaulers.end())
                        {
                            m_WyrmtongueBloodhaulers[p_Creature->GetGUID()] = p_Creature->GetPosition();
                        }
                    }

                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    if (!p_Creature->isSummon() && p_Creature->IsHostileToPlayers() &&
                        p_Creature->GetEntry() != InvasionPointNaigtal::eNpcs::Npc_WickedCorruptor &&
                        p_Creature->GetEntry() != InvasionPointNaigtal::eNpcs::Npc_MarshShambler)
                    {
                        m_DemonGuids.push_back(p_Creature->GetGUID());
                    }

                    if (p_Creature->GetEntry() == InvasionPointNaigtal::eNpcs::Npc_WickedCorruptor)
                    {
                        p_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 2147516672);
                        p_Creature->SetUInt32Value(UNIT_FIELD_FLAGS_2, 2099200);
                    }

                    if (p_Creature->GetEntry() == InvasionPointNaigtal::eNpcs::Npc_MarshShambler)
                    {
                        m_MarshShamblerGuids.push_back(p_Creature->GetGUID());
                    }

                    if (l_Scenario->GetCurrentStep() == InvasionPointNaigtal::eSteps::Step_SomethingWicked &&
                        p_Creature->IsHostileToPlayers() && p_Creature->isElite() && p_Creature->isSummon())
                    {
                        p_Creature->AddDelayedEvent([p_Creature]() -> void
                        {
                            if (!p_Creature->isInCombat())
                            {
                                p_Creature->LoadPath(InvasionPointNaigtal::ePaths::Path_BossCircle);
                                p_Creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                p_Creature->GetMotionMaster()->Initialize();
                            }
                        }, 5000);
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void OnScenarioNextStep(uint32 p_NewStep) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    switch (p_NewStep)
                    {
                    case InvasionPointVal::eSteps::Step_EnhancedEnemies:
                    {
                        for (auto l_Itr : m_DemonHunterGuids)
                        {
                            if (Creature* l_Creature = l_Map->GetCreature(l_Itr))
                            {
                                l_Creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            }
                        }

                        ForceCastOnPlayers(InvasionPointVal::eSpells::Spell_AfterFirstStageConversation, l_Map);
                        SummonCreaturesWithDelay(InvasionPointVal::g_SecondStageCreaturesSpawnPositions, l_Map, 1000, true);
                        break;
                    }
                    case InvasionPointVal::eSteps::Step_ChangeInLeadership:
                    {
                        ForceCastOnPlayers(InvasionPointVal::eSpells::Spell_AfterSecondStageConversation, l_Map, 7000);
                        SummonRandomBoss(l_Map, InvasionPointVal::g_BossSpawnPos);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    switch (p_NewStep)
                    {
                    case InvasionPointAurinor::eSteps::Step_TemporalAnomalies:
                    {
                        for (auto itr :m_TemporalAnomalyGuids)
                        {
                            if (Creature* l_Creature = l_Map->GetCreature(itr))
                            {
                                l_Creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            }
                        }

                        ForceCastOnPlayers(InvasionPointAurinor::eSpells::Spell_AfterFirstStageConversation, l_Map);
                        SummonCreaturesWithDelay(InvasionPointAurinor::g_SecondStageCreaturesSpawnPositions, l_Map, 3000, true);
                        break;
                    }
                    case InvasionPointAurinor::eSteps::Step_KillTheHead:
                    {
                        ForceCastOnPlayers(InvasionPointAurinor::eSpells::Spell_AfterSecondStageConversation, l_Map, 10000);
                        SummonRandomBoss(l_Map, InvasionPointAurinor::g_BossSpawnPos);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    switch (p_NewStep)
                    {
                    case InvasionPointCengar::eSteps::Step_MagmaFlows:
                    {
                        for (auto l_Itr : m_GiantGuids)
                        {
                            if (Creature* l_Creature = l_Map->GetCreature(l_Itr))
                            {
                                if (l_Creature->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[0])
                                {
                                    l_Creature->AI()->DoAction(InvasionPointCengar::eActions::Action_ActivateMagmaGiant);
                                    break;
                                }
                            }
                        }

                        ForceCastOnPlayers(InvasionPointCengar::eSpells::Spell_AfterFirstStageConversation, l_Map);
                        m_Events.ScheduleEvent(InvasionPointCengar::eEvents::Event_SummonSecondStageCreaturesFirst, 7000);
                        m_Events.ScheduleEvent(InvasionPointCengar::eEvents::Event_SummonSecondStageCreaturesSecond, 8000);
                        m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 1000);
                        break;
                    }
                    case InvasionPointCengar::eSteps::Step_SecureVictory:
                    {
                        ForceCastOnPlayers(InvasionPointCengar::eSpells::Spell_AfterSecondStageConversation, l_Map, 10000);
                        m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 1000);
                        SummonRandomBoss(l_Map, InvasionPointCengar::g_BossSpawnPos);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    switch (p_NewStep)
                    {
                    case InvasionPointBonich::eSteps::Step_PushBackTheLegion:
                    {
                        SummonCreaturesWithDelay(InvasionPointBonich::g_SecondStageCreaturesSpawnPositions, l_Map);
                        ForceCastOnPlayers(InvasionPointBonich::eSpells::Spell_AfterFirstStageConversation, l_Map);
                        break;
                    }
                    case InvasionPointBonich::eSteps::Step_TheLegionsCommander:
                    {
                        ForceCastOnPlayers(InvasionPointBonich::eSpells::Spell_AfterSecondStageConversation, l_Map, 10000);
                        m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 1000);
                        SummonRandomBoss(l_Map, InvasionPointBonich::g_BossSpawnPos);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    switch (p_NewStep)
                    {
                    case InvasionPointSangua::eSteps::Step_StopTheLegion:
                    {
                        if (Creature* l_Creature = l_Map->GetCreature(m_LegionPortalGuid))
                        {
                            l_Creature->SetAIAnimKitId(13792);
                        }

                        m_Events.ScheduleEvent(InvasionPointSangua::eEvents::Event_SummonWyrmtongueBloodhaulers, 8000);
                        SummonCreaturesWithDelay(InvasionPointSangua::g_SecondStageCreaturesSpawnPositions, l_Map);
                        ForceCastOnPlayers(InvasionPointSangua::eSpells::Spell_AfterFirstStageConversation, l_Map);
                        break;
                    }
                    case InvasionPointSangua::eSteps::Step_RestoreBalance:
                    {
                        ForceCastOnPlayers(InvasionPointSangua::eSpells::Spell_AfterSecondStageConversation, l_Map, 10000);
                        SummonRandomBoss(l_Map, InvasionPointSangua::g_BossSpawnPos);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    switch (p_NewStep)
                    {
                    case InvasionPointNaigtal::eSteps::Step_RepelTheLegion:
                    {
                        for (auto l_Itr : m_MarshShamblerGuids)
                        {
                            if (Creature* l_Creature = l_Map->GetCreature(l_Itr))
                            {
                                if (l_Creature->ComputeLinkedId() == InvasionPointNaigtal::g_MarshShamblerLinkedIds[0])
                                {
                                    l_Creature->AI()->DoAction(InvasionPointNaigtal::eActions::Action_FirstMarshShamblerFreed);
                                }
                                else if (l_Creature->ComputeLinkedId() == InvasionPointNaigtal::g_MarshShamblerLinkedIds[1])
                                {
                                    l_Creature->AI()->DoAction(InvasionPointNaigtal::eActions::Action_SecondMarshShamblerFreed);
                                }
                            }
                        }

                        m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 1000);
                        SummonCreaturesWithDelay(InvasionPointNaigtal::g_SecondStageCreaturesSpawnPositionsFirst, l_Map, 1000);
                        SummonCreaturesWithDelay(InvasionPointNaigtal::g_SecondStageCreaturesSpawnPositionsSecond, l_Map, 2000);
                        SummonCreaturesWithDelay(InvasionPointNaigtal::g_SecondStageCreaturesSpawnPositionsThird, l_Map, 3000);
                        SummonCreaturesWithDelay(InvasionPointNaigtal::g_SecondStageCreaturesSpawnPositionsFourth, l_Map, 4000);
                        SummonCreaturesWithDelay(InvasionPointNaigtal::g_SecondStageCreaturesSpawnPositionsFifth, l_Map, 5000);
                        ForceCastOnPlayers(InvasionPointNaigtal::eSpells::Spell_AfterFirstStageConversation, l_Map);
                        break;
                    }
                    case InvasionPointNaigtal::eSteps::Step_SomethingWicked:
                    {
                        ForceCastOnPlayers(InvasionPointNaigtal::eSpells::Spell_AfterSecondStageConversation, l_Map, 10000);
                        SummonRandomBoss(l_Map, InvasionPointNaigtal::g_BossSpawnPos);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                switch (p_Creature->GetZoneId())
                {
                case eScenarioZones::Zone_Folnuna:
                {
                    if (p_Creature->GetEntry() == GreaterInvasionPointMatronFolnuna::eNpcs::Npc_CrazedCorruptor)
                    {
                        auto l_MapItr = GreaterInvasionPointMatronFolnuna::g_CorruptorAndRiftLinkedIds.find(p_Creature->ComputeLinkedId());
                        if (l_MapItr != GreaterInvasionPointMatronFolnuna::g_CorruptorAndRiftLinkedIds.end())
                        {
                            std::list<Creature*> l_CreatureList;
                            p_Creature->GetCreatureListWithEntryInGrid(l_CreatureList, GreaterInvasionPointMatronFolnuna::eNpcs::Npc_SmolderingRift, 200.0f);

                            for (auto l_Itr : l_CreatureList)
                            {
                                if (l_MapItr->second == l_Itr->ComputeLinkedId())
                                {
                                    l_Itr->CastSpell(l_Itr, GreaterInvasionPointMatronFolnuna::eSpells::Spell_RiftCollapse, true);
                                    l_Itr->DespawnOrUnsummon();
                                    break;
                                }
                            }
                        }
                    }

                    break;
                }
                default:
                    break;
                }

                if (p_Creature->GetEntry() == m_LastBossId || p_Creature->GetEntry() == eGlobalNpcs::Npc_InquisitorMeto ||
                    p_Creature->GetEntry() == eGlobalNpcs::Npc_Sotanathor || p_Creature->GetEntry() == eGlobalNpcs::Npc_MistressAlluradel ||
                    p_Creature->GetEntry() == eGlobalNpcs::Npc_MatronFolnuna || p_Creature->GetEntry() == eGlobalNpcs::Npc_PitLordVilemus ||
                    p_Creature->GetEntry() == eGlobalNpcs::Npc_Occularus)
                {
                    if (Map* l_Map = p_Creature->GetMap())
                    {
                        if (!l_Map->IsClosed())
                        {
                            l_Map->CloseMap();
                        }
                    }
                }

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                ApplyModifiers(l_Map, false, true, false, false, p_Creature, nullptr);

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    switch (l_Scenario->GetCurrentStep())
                    {
                    case InvasionPointVal::eSteps::Step_SeizeAFoothold:
                    {
                        IncreaseScenarioProgress(6, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointVal::eAssets::Asset_SeizeAFoothold);
                        break;
                    }
                    case InvasionPointVal::eSteps::Step_ChangeInLeadership:
                    {
                        if (p_Creature->GetEntry() == m_LastBossId)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointVal::eAssets::Asset_ChangeInLeadership);
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    switch (l_Scenario->GetCurrentStep())
                    {
                    case InvasionPointAurinor::eSteps::Step_SeizeAFoothold:
                    {
                        IncreaseScenarioProgress(6, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointAurinor::eAssets::Asset_SeizeAFoothold);
                        break;
                    }
                    case InvasionPointAurinor::eSteps::Step_KillTheHead:
                    {
                        if (p_Creature->GetEntry() == m_LastBossId)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointAurinor::eAssets::Asset_KillTheHead);
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    switch (l_Scenario->GetCurrentStep())
                    {
                    case InvasionPointCengar::eSteps::Step_RiftBreaker:
                    {
                        switch (p_Creature->GetEntry())
                        {
                        case InvasionPointCengar::eNpcs::Npc_CrazedCorruptor:
                        {
                            auto l_MapItr = InvasionPointCengar::g_CorruptorAndRiftLinkedIds.find(p_Creature->ComputeLinkedId());
                            if (l_MapItr != InvasionPointCengar::g_CorruptorAndRiftLinkedIds.end())
                            {
                                std::list<Creature*> l_CreatureList;
                                p_Creature->GetCreatureListWithEntryInGrid(l_CreatureList, InvasionPointCengar::eNpcs::Npc_SmolderingRift, 200.0f);

                                for (auto l_Itr : l_CreatureList)
                                {
                                    if (l_MapItr->second == l_Itr->ComputeLinkedId())
                                    {
                                        l_Itr->CastSpell(l_Itr, InvasionPointCengar::eSpells::Spell_RiftCollapse, true);
                                        l_Itr->DespawnOrUnsummon();
                                        break;
                                    }
                                }
                            }

                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointCengar::eAssets::Asset_RiftBreaker);
                            break;
                        }
                        case InvasionPointCengar::eNpcs::Npc_BurstingElemental:
                        {
                            auto l_Iterator = std::find(m_DemonGuids.begin(), m_DemonGuids.end(), p_Creature->GetGUID());
                            if (l_Iterator != m_DemonGuids.end())
                            {
                                m_DemonGuids.erase(l_Iterator);
                            }

                            break;
                        }
                        default:
                            break;
                        }

                        break;
                    }
                    case InvasionPointCengar::eSteps::Step_MagmaFlows:
                    {
                        if (p_Creature->GetEntry() == InvasionPointCengar::eNpcs::Npc_MagmaGiant)
                        {
                            m_GiansDied++;

                            auto l_Iterator = std::find(m_GiantGuids.begin(), m_GiantGuids.end(), p_Creature->GetGUID());
                            if (l_Iterator != m_GiantGuids.end())
                            {
                                m_GiantGuids.erase(l_Iterator);
                            }

                            for (auto l_Itr : m_GiantGuids)
                            {
                                if (Creature* l_Creature = l_Map->GetCreature(l_Itr))
                                {
                                    if (m_GiansDied == 1 && l_Creature->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[1])
                                    {
                                        l_Creature->AI()->DoAction(InvasionPointCengar::eActions::Action_ActivateMagmaGiant);
                                        break;
                                    }
                                    else if (m_GiansDied == 2 && l_Creature->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[2])
                                    {
                                        l_Creature->AI()->DoAction(InvasionPointCengar::eActions::Action_ActivateMagmaGiant);
                                        break;
                                    }
                                    else if (m_GiansDied == 3 && l_Creature->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[3])
                                    {
                                        l_Creature->AI()->DoAction(InvasionPointCengar::eActions::Action_ActivateMagmaGiant);
                                        break;
                                    }
                                    else if (m_GiansDied == 4 && l_Creature->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[4])
                                    {
                                        l_Creature->AI()->DoAction(InvasionPointCengar::eActions::Action_ActivateMagmaGiant);
                                        break;
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case InvasionPointCengar::eSteps::Step_SecureVictory:
                    {
                        if (p_Creature->GetEntry() == m_LastBossId)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointCengar::eAssets::Asset_SecureVictory);
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    switch (l_Scenario->GetCurrentStep())
                    {
                    case InvasionPointBonich::eSteps::Step_ClearingTheClearing:
                    {
                        if (p_Creature->IsHostileToPlayers())
                        {
                            IncreaseScenarioProgress(7, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointBonich::eAssets::Asset_ClearingTheClearing);
                        }

                        break;
                    }
                    case InvasionPointBonich::eSteps::Step_TheLegionsCommander:
                    {
                        if (p_Creature->GetEntry() == m_LastBossId)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointBonich::eAssets::Asset_TheLegionsCommander);
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    switch (l_Scenario->GetCurrentStep())
                    {
                    case InvasionPointSangua::eSteps::Step_StemTheBleeding:
                    {
                        if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_LegionBloodDrainer)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointSangua::eAssets::Asset_StemTheBleeding);
                        }

                        break;
                    }
                    case InvasionPointSangua::eSteps::Step_StopTheLegion:
                    {
                        if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler && p_Creature->isSummon())
                        {
                            p_Creature->SetReactState(ReactStates::REACT_DEFENSIVE);
                        }

                        if (p_Creature->GetEntry() != InvasionPointSangua::eNpcs::Npc_LegionBloodDrainer &&
                            p_Creature->GetEntry() != InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler &&
                            p_Creature->IsHostileToPlayers() && !p_Creature->GetUInt64Value(UNIT_FIELD_DEMON_CREATOR))
                        {
                            if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_NoxiousShadowstalker)
                            {
                                IncreaseScenarioProgress(23, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointSangua::eAssets::Asset_StopTheLegion);
                            }
                            else if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_ClubfistObliterator)
                            {
                                IncreaseScenarioProgress(23, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointSangua::eAssets::Asset_StopTheLegion);
                            }
                            else
                            {
                                IncreaseScenarioProgress(16, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointSangua::eAssets::Asset_StopTheLegion);
                            }
                        }

                        break;
                    }
                    case InvasionPointSangua::eSteps::Step_RestoreBalance:
                    {
                        if (p_Creature->GetEntry() == m_LastBossId)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointSangua::eAssets::Asset_RestoreBalance);
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    switch (l_Scenario->GetCurrentStep())
                    {
                    case InvasionPointNaigtal::eSteps::Step_ScatteredTotheWinds:
                    {
                        switch (p_Creature->GetEntry())
                        {
                        case InvasionPointNaigtal::eNpcs::Npc_ChitteringFiend:
                        {
                            IncreaseScenarioProgress(3, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointNaigtal::eAssets::Asset_ScatteredTotheWinds);
                            break;
                        }
                        case InvasionPointNaigtal::eNpcs::Npc_CrazedCorruptor:
                        {
                            IncreaseScenarioProgress(20, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointNaigtal::eAssets::Asset_ScatteredTotheWinds);
                            break;
                        }
                        default:
                        {
                            if (p_Creature->IsHostileToPlayers())
                            {
                                IncreaseScenarioProgress(6, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointNaigtal::eAssets::Asset_ScatteredTotheWinds);
                            }

                            break;
                        }
                        }

                        break;
                    }
                    case InvasionPointNaigtal::eSteps::Step_SomethingWicked:
                    {
                        if (p_Creature->GetEntry() == m_LastBossId)
                        {
                            IncreaseScenarioProgress(1, l_Scenario, GetAnyPlayer(p_Killer), InvasionPointNaigtal::eAssets::Asset_SomethingWicked);
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void OnScenarioComplete() override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                if (!l_Map->IsClosed())
                {
                    l_Map->CloseMap();
                }

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 5000);
                    ForceCastOnPlayers(InvasionPointVal::eSpells::Spell_ScenarioCompletedConversation, l_Map);
                    SummonScenarioCompleteObjects(InvasionPointVal::g_IllidariBannersSpawnPositions, InvasionPointVal::g_DemonHunterSpawnPositions, InvasionPointVal::g_PortalSpawnPos, l_Map);
                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 5000);
                    ForceCastOnPlayers(InvasionPointAurinor::eSpells::Spell_ScenarioCompletedConversation, l_Map);
                    SummonScenarioCompleteObjects(InvasionPointAurinor::g_IllidariBannersSpawnPositions, InvasionPointAurinor::g_DemonHunterSpawnPositions, InvasionPointAurinor::g_PortalSpawnPos, l_Map);
                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    ForceCastOnPlayers(InvasionPointCengar::eSpells::Spell_ScenarioCompletedConversation, l_Map);
                    SummonScenarioCompleteObjects(InvasionPointCengar::g_IllidariBannersSpawnPositions, InvasionPointCengar::g_DemonHunterSpawnPositions, InvasionPointCengar::g_PortalSpawnPos, l_Map);
                    ClearVairables();
                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    ForceCastOnPlayers(InvasionPointBonich::eSpells::Spell_ScenarioCompletedConversation, l_Map);
                    SummonScenarioCompleteObjects(InvasionPointBonich::g_IllidariBannersSpawnPositions, InvasionPointBonich::g_DemonHunterSpawnPositions, InvasionPointBonich::g_PortalSpawnPos, l_Map);
                    ClearVairables();
                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    if (Creature* l_Creature = l_Map->GetCreature(m_LegionPortalGuid))
                    {
                        l_Creature->SetAIAnimKitId(2435);
                    }

                    m_Events.Reset();
                    m_Events.ScheduleEvent(eGlobalEvents::Event_UnsummonDemons, 4000);
                    SummonScenarioCompleteObjects(InvasionPointSangua::g_IllidariBannersSpawnPositions, InvasionPointSangua::g_DemonHunterSpawnPositions, InvasionPointSangua::g_PortalSpawnPos, l_Map);
                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    ForceCastOnPlayers(InvasionPointNaigtal::eSpells::Spell_ScenarioCompletedConversation, l_Map);
                    SummonScenarioCompleteObjects(InvasionPointNaigtal::g_IllidariBannersSpawnPositions, InvasionPointNaigtal::g_DemonHunterSpawnPositions, InvasionPointNaigtal::g_PortalSpawnPos, l_Map);
                    ClearVairables();
                    break;
                }
                default:
                    break;
                }
            }

            void OnCreatureRemoveForScript(Creature* p_Creature) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Sangua:
                {
                    if (!l_Scenario->IsCompleted())
                    {
                        if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler
                            && p_Creature->isSummon() && p_Creature->GetWaypointPath())
                        {
                            Position l_SpawnPos;
                            uint32 l_Path = p_Creature->GetWaypointPath();

                            auto l_Itr = m_WyrmtongueBloodhaulers.find(p_Creature->GetGUID());
                            if (l_Itr != m_WyrmtongueBloodhaulers.end())
                            {
                                l_SpawnPos = l_Itr->second;
                                m_WyrmtongueBloodhaulers.erase(l_Itr);
                            }

                            AddTimedDelayedOperation(5000, [l_Map, l_Path, l_SpawnPos]() -> void
                            {
                                if (Creature* l_Creature = l_Map->SummonCreature(InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler, l_SpawnPos))
                                {
                                    l_Creature->DelayedCastSpell(l_Creature, InvasionPointSangua::eSpells::Spell_FelDissolveIn, true, 100);

                                    l_Creature->AddDelayedEvent([l_Creature, l_Path]() -> void
                                    {
                                        l_Creature->LoadPath(l_Path);
                                        l_Creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                        l_Creature->GetMotionMaster()->Initialize();
                                    }, 1000);
                                }
                            });
                        }
                    }

                    if (p_Creature->GetEntry() == InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler)
                    {
                        auto l_Iterator = std::find(m_DemonGuids.begin(), m_DemonGuids.end(), p_Creature->GetGUID());
                        if (l_Iterator != m_DemonGuids.end())
                        {
                            m_DemonGuids.erase(l_Iterator);
                        }
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void Update(uint32 const p_Diff) override
            {
                Map* l_Map = GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                m_Events.Update(p_Diff);
                UpdateOperations(p_Diff);

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    switch (m_Events.ExecuteEvent())
                    {
                    case eGlobalEvents::Event_UnsummonDemons:
                    {
                        UnsummonDemons(l_Map);
                        ClearVairables();
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    switch (m_Events.ExecuteEvent())
                    {
                    case eGlobalEvents::Event_UnsummonDemons:
                    {
                        UnsummonDemons(l_Map);
                        ClearVairables();
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    switch (m_Events.ExecuteEvent())
                    {
                    case InvasionPointCengar::eEvents::Event_SummonSecondStageCreaturesFirst:
                    {
                        if (Creature* l_Leader = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_DiabolicalChainwarden, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[1]))
                        {
                            if (Creature* l_Member = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_ClubfistObliterator, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[0]))
                            {
                                sFormationMgr->AddMemberInLeaderGroup(l_Leader, l_Member, 90.0f, 4.0f);

                                l_Leader->AddDelayedEvent([l_Leader]() -> void
                                {
                                    l_Leader->LoadPath(InvasionPointCengar::eCreaturesPaths::Path_DiabolicalChainwarden);
                                    l_Leader->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                    l_Leader->GetMotionMaster()->Initialize();
                                }, 5000);
                            }
                        }

                        break;
                    }
                    case InvasionPointCengar::eEvents::Event_SummonSecondStageCreaturesSecond:
                    {
                        if (Creature* l_Leader = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_FelfrenzyBerserker, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[6]))
                        {
                            if (Creature* l_FirstMember = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_FrothingRazorwing, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[3]))
                            {
                                if (Creature* l_SecondMember = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_BladeswornDisciple, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[4]))
                                {
                                    sFormationMgr->AddMemberInLeaderGroup(l_Leader, l_FirstMember, 90.0f, 4.0f);
                                    sFormationMgr->AddMemberInLeaderGroup(l_Leader, l_SecondMember, 270.0f, 4.0f);

                                    l_Leader->AddDelayedEvent([l_Leader]() -> void
                                    {
                                        l_Leader->LoadPath(InvasionPointCengar::eCreaturesPaths::Path_FelfrenzyBerserker);
                                        l_Leader->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                        l_Leader->GetMotionMaster()->Initialize();
                                    }, 5000);
                                }
                            }
                        }

                        if (Creature* l_Leader = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_BlacksoulInquisitor, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[5]))
                        {
                            if (Creature* l_FirstMember = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_FelrageMarauder, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[2]))
                            {
                                if (Creature* l_SecondMember = l_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_ChitteringFiend, InvasionPointCengar::g_SecondStageCreaturesSpawnPositions[7]))
                                {
                                    sFormationMgr->AddMemberInLeaderGroup(l_Leader, l_FirstMember, 270.0f, 4.0f);
                                    sFormationMgr->AddMemberInLeaderGroup(l_Leader, l_SecondMember, 90.0f, 4.0f);

                                    l_Leader->AddDelayedEvent([l_Leader]() -> void
                                    {
                                        l_Leader->LoadPath(InvasionPointCengar::eCreaturesPaths::Path_BlacksoulInquisitor);
                                        l_Leader->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                        l_Leader->GetMotionMaster()->Initialize();
                                    }, 5000);
                                }
                            }
                        }

                        break;
                    }
                    case InvasionPointCengar::eEvents::Event_CastVolatileLava:
                    {
                        std::random_device l_RandomDevice;
                        std::mt19937 l_RandomGenerator(l_RandomDevice());
                        std::shuffle(m_LavaGuids.begin(), m_LavaGuids.end(), l_RandomGenerator);

                        if (Creature* l_Creature = l_Map->GetCreature(m_LavaGuids[0]))
                        {
                            Position l_CenterPos = { 662.7379f, 614.3316f, 40.5247f };
                            l_CenterPos.SimplePosXYRelocationByAngle(l_CenterPos, frand(5.0f, 50.0f), frand(0.0f, M_PI * 2.0f), true);

                            l_Creature->CastSpell(l_CenterPos, InvasionPointCengar::eSpells::Spell_VolatileLava, true);
                        }

                        m_Events.ScheduleEvent(InvasionPointCengar::eEvents::Event_CastVolatileLava, 5000);
                        break;
                    }
                    case eGlobalEvents::Event_UnsummonDemons:
                    {
                        UnsummonDemons(l_Map);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    switch (m_Events.ExecuteEvent())
                    {
                    case eGlobalEvents::Event_UnsummonDemons:
                    {
                        UnsummonDemons(l_Map);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    switch (m_Events.ExecuteEvent())
                    {
                    case InvasionPointSangua::eEvents::Event_SummonWyrmtongueBloodhaulers:
                    {
                        if (Creature* l_Creature = l_Map->SummonCreature(InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler, InvasionPointSangua::g_WyrmtongueBloodhaulerSpawnPositions[0]))
                        {
                            l_Creature->DelayedCastSpell(l_Creature, InvasionPointSangua::eSpells::Spell_FelDissolveIn, true, 100);
                            m_WyrmtongueBloodhaulers[l_Creature->GetGUID()] = l_Creature->GetPosition();

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->LoadPath(InvasionPointSangua::eWyrmtongueBloodhaulerPaths::Path_First);
                                l_Creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                l_Creature->GetMotionMaster()->Initialize();
                            }, 1000);
                        }

                        if (Creature* l_Creature = l_Map->SummonCreature(InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler, InvasionPointSangua::g_WyrmtongueBloodhaulerSpawnPositions[1]))
                        {
                            l_Creature->DelayedCastSpell(l_Creature, InvasionPointSangua::eSpells::Spell_FelDissolveIn, true, 100);
                            m_WyrmtongueBloodhaulers[l_Creature->GetGUID()] = l_Creature->GetPosition();

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->LoadPath(InvasionPointSangua::eWyrmtongueBloodhaulerPaths::Path_Second);
                                l_Creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                l_Creature->GetMotionMaster()->Initialize();
                            }, 1000);
                        }

                        if (Creature* l_Creature = l_Map->SummonCreature(InvasionPointSangua::eNpcs::Npc_WyrmtongueBloodhauler, InvasionPointSangua::g_WyrmtongueBloodhaulerSpawnPositions[2]))
                        {
                            l_Creature->DelayedCastSpell(l_Creature, InvasionPointSangua::eSpells::Spell_FelDissolveIn, true, 100);
                            m_WyrmtongueBloodhaulers[l_Creature->GetGUID()] = l_Creature->GetPosition();

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->LoadPath(InvasionPointSangua::eWyrmtongueBloodhaulerPaths::Path_Third);
                                l_Creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                l_Creature->GetMotionMaster()->Initialize();
                            }, 1000);
                        }

                        break;
                    }
                    case eGlobalEvents::Event_UnsummonDemons:
                    {
                        UnsummonDemons(l_Map);
                        ClearVairables();
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    switch (m_Events.ExecuteEvent())
                    {
                    case InvasionPointNaigtal::eEvents::Event_CastEmbeddedSporesOnPlayers:
                    {
                        ForceCastOnPlayers(InvasionPointNaigtal::eSpells::Spell_EmbeddedSpores, l_Map);
                        m_Events.ScheduleEvent(InvasionPointNaigtal::eEvents::Event_CastEmbeddedSporesOnPlayers, 5000);
                        break;
                    }
                    case eGlobalEvents::Event_UnsummonDemons:
                    {
                        UnsummonDemons(l_Map);
                        break;
                    }
                    default:
                        break;
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void ForceCastOnPlayers(uint32 p_SpellId, Map* p_Map, uint16 p_Delay = 0)
            {
                if (p_Delay)
                {
                    AddTimedDelayedOperation(p_Delay, [p_SpellId, p_Map]() -> void
                    {
                        Map::PlayerList const& l_PlayerList = p_Map->GetPlayers();
                        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                l_Player->CastSpell(l_Player, p_SpellId, true);
                            }
                        }
                    });
                }
                else
                {
                    Map::PlayerList const& l_PlayerList = p_Map->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            l_Player->CastSpell(l_Player, p_SpellId, true);
                        }
                    }
                }
            }

            void UnsummonDemons(Map* p_Map)
            {
                for (auto l_Itr : m_DemonGuids)
                {
                    if (Creature* l_Creature = p_Map->GetCreature(l_Itr))
                    {
                        if (!l_Creature->isInCombat() && l_Creature->GetEntry() != m_LastBossId)
                        {
                            uint16 l_Delay = urand(1000, 5000);
                            l_Creature->DelayedCastSpell(l_Creature, eGlobalSpells::Spell_DemonEscape, true, l_Delay);
                            l_Creature->DespawnOrUnsummon(1000 + l_Delay);
                        }
                    }
                }
            }

            void SummonScenarioCompleteObjects(std::vector<Position> p_Banners, std::vector<Position> p_DemonHunters, Position p_Portal, Map* p_Map)
            {
                for (auto l_Itr : p_Banners)
                {
                    if (GameObject* l_GameObject = p_Map->SummonGameObject(eGlobalGobs::Gob_IllidariBanner, l_Itr))
                    {
                        l_GameObject->SetUInt32Value(GAMEOBJECT_FIELD_SPELL_VISUAL_ID, 64943);
                    }
                }

                AddTimedDelayedOperation(2000, [p_Map, p_Portal]() -> void
                {
                    p_Map->SummonCreature(eGlobalNpcs::Npc_InvasionPoint, p_Portal);
                });

                uint16 l_Delay = 4000;

                for (auto l_Itr : p_DemonHunters)
                {
                    AddTimedDelayedOperation(l_Delay, [p_Map, l_Itr]() -> void
                    {
                        if (Creature* l_Creature = p_Map->SummonCreature(eGlobalNpcs::Npc_DemonHunter, l_Itr))
                        {
                            l_Creature->CastSpell(l_Creature, eGlobalSpells::Spell_FelDissolveIn, true);

                            Position l_MovePos = l_Creature->GetPosition();
                            l_MovePos.SimplePosXYRelocationByAngle(l_MovePos, 8.0f, 0.0f, false);

                            l_Creature->SetWalk(true);
                            l_Creature->GetMotionMaster()->MovePoint(eGlobalPoints::Point_DemonHunterMoveEnd, l_MovePos, false);
                        }
                    });

                    l_Delay += 1000;
                }
            }

            void SummonCreaturesWithDelay(std::multimap<uint32, Position> p_Demons, Map* p_Map, uint16 p_Delay = 0, bool p_WithPeriodicDelay = false)
            {
                uint16 l_Delay = p_Delay;

                if (l_Delay > 0)
                {
                    if (p_WithPeriodicDelay)
                    {
                        for (auto l_Itr : p_Demons)
                        {
                            AddTimedDelayedOperation(l_Delay, [p_Map, l_Itr]() -> void
                            {
                                p_Map->SummonCreature(l_Itr.first, l_Itr.second);
                            });

                            l_Delay += 1000;
                        }
                    }
                    else
                    {
                        for (auto l_Itr : p_Demons)
                        {
                            AddTimedDelayedOperation(l_Delay, [p_Map, l_Itr]() -> void
                            {
                                p_Map->SummonCreature(l_Itr.first, l_Itr.second);
                            });
                        }
                    }
                }
                else
                {
                    for (auto l_Itr : p_Demons)
                    {
                        p_Map->SummonCreature(l_Itr.first, l_Itr.second);
                    }
                }
            }

            void SummonRandomBoss(Map* p_Map, Position p_SpawnPos)
            {
                std::vector<uint32> l_Bosses = g_SmallInvasionPointBosses;

                std::random_device l_RandomDevice;
                std::mt19937 l_RandomGenerator(l_RandomDevice());
                std::shuffle(l_Bosses.begin(), l_Bosses.end(), l_RandomGenerator);

                uint32 l_BossEntry = l_Bosses[0];

                if (Creature* l_Creature = p_Map->SummonCreature(l_BossEntry, p_SpawnPos))
                {
                    for (auto l_Itr : g_InvasionPointDatas)
                    {
                        if (l_Itr.second->ScenarioZoneId == l_Creature->GetZoneId())
                        {
                            l_Creature->SetTrackingQuestID(l_Itr.second->QuestId);
                            break;
                        }
                    }
                }

                m_LastBossId = l_BossEntry;
            }

            void ClearVairables()
            {
                m_DemonGuids.clear();
                m_LastBossId = 0;
                m_DemonHunterGuids.clear();
                m_TemporalAnomalyGuids.clear();
                m_GiantGuids.clear();
                m_LavaGuids.clear();
                m_GiansDied = 0;
                m_LegionPortalGuid.Clear();
                m_WyrmtongueBloodhaulers.clear();
                m_MarshShamblerGuids.clear();
                m_Events.Reset();
                m_Modifier = 0;
                m_SubModifier = 0;
            }

            void ChooseRandomModifiers(Map* p_Map)
            {
                Scenario* l_Scenario = sScenarioMgr->GetScenario(p_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    m_Modifier = urand(1, 3);
                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    m_Modifier = urand(1, 3);
                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    m_Modifier = urand(1, 4);
                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    m_Modifier = urand(1, 3);
                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    m_Modifier = urand(1, 2);
                    m_SubModifier = urand(0, 1);
                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    m_Modifier = urand(1, 3);
                    m_SubModifier = urand(1, 3);
                    break;
                }
                default:
                    break;
                }
            }

            void ApplyModifiers(Map* p_Map, bool p_OnCreate, bool p_OnDeath, bool p_OnInit, bool p_OnEnter, Creature* p_Creature = nullptr, Player* p_Player = nullptr)
            {
                Scenario* l_Scenario = sScenarioMgr->GetScenario(p_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                switch (l_Scenario->GetScenarioId())
                {
                case eScenarios::Scenario_Val:
                {
                    if (p_OnInit)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointVal::eModifiers::Mod_BuffetingWinds:
                        {
                            for (auto l_Itr : InvasionPointVal::g_IceTorrentSpawnPositions)
                            {
                                if (Creature* l_Creature = p_Map->SummonCreature(InvasionPointVal::eNpcs::Npc_IceTorrent, l_Itr))
                                {
                                    l_Creature->SetRespawnRadius(30.0f);
                                    l_Creature->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                                    l_Creature->GetMotionMaster()->InitDefault();
                                }
                            }

                            break;
                        }
                        case InvasionPointVal::eModifiers::Mod_Blizzard:
                        {
                            for (auto l_Itr : InvasionPointVal::g_BlizzardSpawnPositions)
                            {
                                if (Creature* l_Creature = p_Map->SummonCreature(InvasionPointVal::eNpcs::Npc_Blizzard, l_Itr))
                                {
                                    l_Creature->SetRespawnRadius(30.0f);
                                    l_Creature->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                                    l_Creature->GetMotionMaster()->InitDefault();
                                }
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }

                    break;
                }
                case eScenarios::Scenario_Aurinor:
                {
                    if (p_OnEnter)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointAurinor::eModifiers::Mod_TemporalPrecognition:
                        {
                            p_Player->CastSpell(p_Player, InvasionPointAurinor::eSpells::Spell_TemporalPrecognition, true);
                            break;
                        }
                        case InvasionPointAurinor::eModifiers::Mod_PowerDilation:
                        {
                            p_Player->CastSpell(p_Player, InvasionPointAurinor::eSpells::Spell_PowerDilation, true);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnInit)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointAurinor::eModifiers::Mod_Decimax:
                        {
                            if (Creature* l_Creature = p_Map->SummonCreature(InvasionPointAurinor::eNpcs::Npc_Decimax, InvasionPointAurinor::g_DecimaxSpawnPos))
                            {
                                l_Creature->AI()->DoAction(InvasionPointAurinor::eActions::Action_DecimaxStartCirclePath);
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }

                    break;
                }
                case eScenarios::Scenario_Cengar:
                {
                    if (p_OnCreate)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointCengar::eModifiers::Mod_FireEnchanted:
                        {
                            if (p_Creature->IsHostileToPlayers())
                            {
                                p_Creature->CastSpell(p_Creature, InvasionPointCengar::eSpells::Spell_FireEnchanted, true);
                            }

                            break;
                        }
                        case InvasionPointCengar::eModifiers::Mod_BurningSteps:
                        {
                            if (p_Creature->IsHostileToPlayers())
                            {
                                p_Creature->CastSpell(p_Creature, InvasionPointCengar::eSpells::Spell_BurningSteps, true);
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnInit)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointCengar::eModifiers::Mod_SearingPower:
                        {
                            p_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_FieryRunes, InvasionPointCengar::g_FieryRunesSpawnPos);
                            break;
                        }
                        case InvasionPointCengar::eModifiers::Mod_VolatileLava:
                        {
                            for (auto l_Itr : InvasionPointCengar::g_LavaSpawnPositions)
                            {
                                if (Creature* l_Creature = p_Map->SummonCreature(InvasionPointCengar::eNpcs::Npc_Lava, l_Itr))
                                {
                                    l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                                    m_LavaGuids.push_back(l_Creature->GetGUID());
                                }
                            }

                            if (!m_Events.HasEvent(InvasionPointCengar::eEvents::Event_CastVolatileLava))
                            {
                                m_Events.ScheduleEvent(InvasionPointCengar::eEvents::Event_CastVolatileLava, 5000);
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }

                    break;
                }
                case eScenarios::Scenario_Bonich:
                {
                    if (p_OnEnter)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointBonich::eModifiers::Mod_SacredVines:
                        {
                            p_Player->CastSpell(p_Player, InvasionPointBonich::eSpells::Spell_SacredVines, true);
                            break;
                        }
                        case InvasionPointBonich::eModifiers::Mod_InvigoratingPollen:
                        {
                            p_Player->CastSpell(p_Player, InvasionPointBonich::eSpells::Spell_InvigoratingMushroom, true);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnCreate)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointBonich::eModifiers::Mod_InvigoratingPollen:
                        {
                            p_Creature->CastSpell(p_Creature, InvasionPointBonich::eSpells::Spell_InvigoratingMushroom, true);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnInit)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointBonich::eModifiers::Mod_SwarmingInsects:
                        {
                            for (auto l_Itr : InvasionPointBonich::g_SwarmingInsectsSpawnPositions)
                            {
                                if (Creature* l_Creature = p_Map->SummonCreature(InvasionPointBonich::eNpcs::Npc_SwarmingInsects, l_Itr))
                                {
                                    l_Creature->SetRespawnRadius(30.0f);
                                    l_Creature->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                                    l_Creature->GetMotionMaster()->InitDefault();
                                }
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }

                    break;
                }
                case eScenarios::Scenario_Sangua:
                {
                    if (p_OnCreate)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointSangua::eModifiers::Mod_CoalescedPowerOnCreate:
                        {
                            if (p_Creature->IsHostileToPlayers())
                            {
                                p_Creature->CastSpell(p_Creature, InvasionPointSangua::eSpells::Spell_CoalescedPowerEndless, true);
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnDeath)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointSangua::eModifiers::Mod_CoalescedPowerOnDeath:
                        {
                            p_Creature->CastSpell(p_Creature, InvasionPointSangua::eSpells::Spell_CoalescedPowerMissile, true);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnInit)
                    {
                        switch (m_SubModifier)
                        {
                        case InvasionPointSangua::eSubModifiers::SubMod_CongealedPlasma:
                        {
                            for (auto l_Itr : InvasionPointSangua::g_CongealedPlasmaSpawnPositions)
                            {
                                p_Map->SummonCreature(InvasionPointSangua::eNpcs::Npc_CongealedPlasma, l_Itr);
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }

                    break;
                }
                case eScenarios::Scenario_Naigtal:
                {
                    if (p_OnCreate)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointNaigtal::eModifiers::Mod_ClickableVines:
                        {
                            if (p_Creature->GetEntry() == InvasionPointNaigtal::eNpcs::Npc_EnergizedVine)
                            {
                                p_Creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            }

                            break;
                        }
                        case InvasionPointNaigtal::eModifiers::Mod_UnclickableVines:
                        {
                            if (p_Creature->GetEntry() == InvasionPointNaigtal::eNpcs::Npc_EnergizedVine)
                            {
                                p_Creature->CastSpell(p_Creature, InvasionPointNaigtal::eSpells::Spell_EnergizedVine, true);
                                p_Creature->CastSpell(p_Creature, InvasionPointNaigtal::eSpells::Spell_EnergizedVineAreatrigger, true);
                            }

                            break;
                        }
                        case InvasionPointNaigtal::eModifiers::Mod_UnpresentedVines:
                        {
                            if (p_Creature->GetEntry() == InvasionPointNaigtal::eNpcs::Npc_EnergizedVine)
                            {
                                p_Creature->SetVisible(false);
                            }

                            break;
                        }
                        default:
                            break;
                        }

                        switch (m_SubModifier)
                        {
                        case InvasionPointNaigtal::eSubModifiers::SubMod_SporeFilled:
                        {
                            if (p_Creature->IsHostileToPlayers())
                            {
                                p_Creature->CastSpell(p_Creature, InvasionPointNaigtal::eSpells::Spell_SporeFilled, true);
                            }

                            break;
                        }
                        case InvasionPointNaigtal::eSubModifiers::SubMod_DarkSpores:
                        {
                            if (p_Creature->GetEntry() == InvasionPointNaigtal::eNpcs::Npc_SinuousDevourer)
                            {
                                p_Creature->CastSpell(p_Creature, InvasionPointNaigtal::eSpells::Spell_SporeLaden, true);
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p_OnInit)
                    {
                        switch (m_Modifier)
                        {
                        case InvasionPointNaigtal::eModifiers::Mod_UnclickableVines:
                        {
                            if (!m_Events.HasEvent(InvasionPointNaigtal::eEvents::Event_CastEmbeddedSporesOnPlayers))
                            {
                                m_Events.ScheduleEvent(InvasionPointNaigtal::eEvents::Event_CastEmbeddedSporesOnPlayers, 1000);
                            }

                            break;
                        }
                        default:
                            break;
                        }

                        switch (m_SubModifier)
                        {
                        case InvasionPointNaigtal::eSubModifiers::SubMod_MaddeningSpores:
                        {
                            for (auto l_Itr : InvasionPointNaigtal::g_MaddeningSporeSpawnPositions)
                            {
                                if (Creature* l_Creature = p_Map->SummonCreature(InvasionPointNaigtal::eNpcs::Npc_MaddeningSpores, l_Itr.second))
                                {
                                    l_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                                    l_Creature->AddDelayedEvent([l_Creature, l_Itr]() -> void
                                    {
                                        l_Creature->LoadPath(l_Itr.first);
                                        l_Creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                                        l_Creature->GetMotionMaster()->Initialize();
                                        l_Creature->CastSpell(l_Creature, InvasionPointNaigtal::eSpells::Spell_MaddeningSporesAreatrigger, true);
                                    }, 5000);
                                }
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    }

                    break;
                }
                default:
                    break;
                }
            }
        };

        OutdoorPvEScript* GetOutdoorPvEScript(Map* p_Map) const override
        {
            return new outdoor_argus_invasion_points_OutdoorPvE(p_Map);
        }
};

void AddSC_outdoor_argus_invasion_points()
{
    new outdoor_argus_invasion_points();
}
