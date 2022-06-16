////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "the_sentinax.hpp"
#include "BrokenIslesPCH.h"
#include "ScriptMgr.h"
#include "MapManager.h"
#include "CreatureTextMgr.h"

namespace Sentinax
{
    class POI_Sentinax : public POIScript
    {
        public:
            POI_Sentinax() : POIScript("POI_Sentinax")
            {
                g_CurrentSentinaxData = nullptr;
            }

            void OnCreateBaseMap(Map* p_Map, uint32 p_ZoneID) override
            {
                if (p_Map->GetId() != 1220 || p_ZoneID != 7543)
                    return;

                SummonSentinax(p_Map);
            }

            void OnPOIAppears(AreaPOI const& p_AreaPOI) override
            {
                if (!IsSentinax(p_AreaPOI))
                    return;

                if (Sentinax::g_SentinaxDatas.find(p_AreaPOI.GetAreaPoiID()) == Sentinax::g_SentinaxDatas.end())
                    return;

                g_CurrentSentinaxData = Sentinax::g_SentinaxDatas[p_AreaPOI.GetAreaPoiID()];

                std::list<Map*> l_Maps = sMapMgr->GetMapsPerZoneId(1220, 7543);
                for (Map* l_Map : l_Maps)
                {
                    l_Map->AddTask([this, l_Map]()->void
                    {
                        SummonSentinax(l_Map);
                    });
                }
            }

            void OnPOIDisappears(AreaPOI const& p_AreaPOI) override
            {
                if (!IsSentinax(p_AreaPOI))
                    return;

                g_CurrentSentinaxData = nullptr;

                for (uint64 l_Guid : m_SentinaxGUIDs)
                {
                    GameObject* l_Sentinax = sObjectAccessor->FindGameObject(l_Guid);
                    if (l_Sentinax)
                    {
                        l_Sentinax->GetMap()->AddTask([l_Sentinax]()->void
                        {
                            if (Creature* l_SentinaxLaser = l_Sentinax->FindNearestCreature(eCreatures::SentinaxLaser, 150.0f))
                                l_SentinaxLaser->DespawnOrUnsummon();

                            l_Sentinax->Delete();
                        });
                    }
                }

                m_SentinaxGUIDs.clear();
            }

        private:
            void SummonSentinax(Map* p_Map)
            {
                if (!g_CurrentSentinaxData)
                    return;

                GameObject* l_GameObject = p_Map->SummonGameObject(eGameObjects::GobSentinax, g_CurrentSentinaxData->SentinaxPosition, eGoBCustomFlags::CustomFlagBig, true);
                if (!l_GameObject)
                    return;

                m_SentinaxGUIDs.insert(l_GameObject->GetGUID());

                Position l_Pos;
                l_GameObject->SimplePosXYRelocationByAngle(l_Pos, 10.0f, -M_PI / 2, false);
                l_Pos.m_positionZ += 155.0f;

                l_GameObject->SummonCreature(Sentinax::SentinaxLaser, l_Pos, TEMPSUMMON_MANUAL_DESPAWN, 0, 0, 0, nullptr, nullptr, true);
            }

            bool IsSentinax(AreaPOI const& p_AreaPOI) const
            {
                return g_SentinaxDatas.find(p_AreaPOI.GetAreaPoiID()) != g_SentinaxDatas.end();
            }

        private:
            std::set<uint64> m_SentinaxGUIDs;
    };

    /// Sentinax - 268706
    /// Last Update 7.3.2 - Build 25549
    class go_sentinax : public GameObjectScript
    {
        public:
            go_sentinax() : GameObjectScript("go_sentinax") { }

            struct go_sentinaxAI : public GameObjectAI
            {
                public:
                    go_sentinaxAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject),
                        m_UpdatePlayerCooldown(0), m_UpdateTime(1 * IN_MILLISECONDS),
                        m_IsInCooldown(0), m_AreatriggerCooldown(0), m_ActivePortals(0) { }

                    void UpdateAI(uint32 p_Diff) override
                    {
                        m_UpdatePlayerCooldown -= p_Diff;
                        m_AreatriggerCooldown -= p_Diff;

                        if (m_UpdatePlayerCooldown > 0)
                            return;

                        UpdateAffectedPlayers();

                        if (m_IsInCooldown)
                        {
                            m_IsInCooldown -= std::min(m_IsInCooldown, p_Diff);
                            if (!m_IsInCooldown)
                                SendTextToPlayers(eBroadcastTextID::SentinaxFinishedRecharging);

                            return;
                        }

                        if (m_AreatriggerCooldown > 0)
                            return;

                        if (m_AffectedPlayers.empty())
                            return;

                        auto l_Itr = m_AffectedPlayers.begin();
                        std::advance(l_Itr, urand(0, m_AffectedPlayers.size() - 1));

                        Player* l_Player = sObjectAccessor->GetPlayer(*go, *l_Itr);
                        if (!l_Player || l_Player->IsFlying())
                            return;

                        m_AreatriggerCooldown = 60 * IN_MILLISECONDS;

                        go->CastSpell(l_Player, eSentinaxSpells::SentinaxDestructionZone);
                    }

                    void SetData64(uint32 p_ActionID, uint64 p_GUID) override
                    {
                        switch (p_ActionID)
                        {
                            case eActions::SetInCooldown:
                            {
                                m_IsInCooldown = 2 * MINUTE * IN_MILLISECONDS;
                                SendTextToPlayers(eBroadcastTextID::SentinaxStartsRecharging);
                                break;
                            }
                            case eActions::SummonedPortals:
                            {
                                SendTextToPlayers(eBroadcastTextID::PlayerSummonedPortals, true, p_GUID);
                                m_ActivePortals += 3;
                                break;
                            }
                            case eActions::ExtendPortals:
                            {
                                SendTextToPlayers(eBroadcastTextID::PlayerExtendedPortals, true, p_GUID);
                                break;
                            }
                            case eActions::EmpowerPortals:
                            {
                                SendTextToPlayers(eBroadcastTextID::PlayerEmpoweredPortals, true, p_GUID);
                                break;
                            }
                            case eActions::SummonedBoss:
                            {
                                SendTextToPlayers(eBroadcastTextID::PlayerSummonedLieutenant, false, p_GUID);
                                break;
                            }
                            case eActions::DespawnedPortal:
                            {
                                m_ActivePortals--;
                                break;
                            }
                            default:
                                break;
                        }
                    }

                    uint32 GetData(uint32 p_Data) override
                    {
                        switch (p_Data)
                        {
                            case eActions::PortalCount:
                                return m_ActivePortals;
                            default:
                                break;
                        }

                        return 0;
                    }

                    void OnDelete() override
                    {
                        for (auto l_Itr = m_AffectedPlayers.begin(); l_Itr != m_AffectedPlayers.end(); ++l_Itr)
                        {
                            Player* l_Player = sObjectAccessor->GetPlayer(*go, *l_Itr);
                            if (!l_Player)
                                continue;

                            l_Player->RemoveAura(eSentinaxSpells::TheShadowOfTheSentinax);
                            l_Player->RemoveAura(eSentinaxSpells::SentinaxCooldown);
                        }
                    }

                private:
                    void UpdateAffectedPlayers()
                    {
                        m_UpdatePlayerCooldown = m_UpdateTime;
                        for (auto l_Itr = m_AffectedPlayers.begin(); l_Itr != m_AffectedPlayers.end();)
                        {
                            Player* l_Player = sObjectAccessor->GetPlayer(*go, *l_Itr);
                            if (!l_Player || l_Player->GetAreaId() != go->GetAreaId())
                            {
                                if (l_Player)
                                {
                                    l_Player->RemoveAura(eSentinaxSpells::TheShadowOfTheSentinax);
                                    if (m_IsInCooldown)
                                        l_Player->RemoveAura(eSentinaxSpells::SentinaxCooldown);
                                }

                                l_Itr = m_AffectedPlayers.erase(l_Itr);
                                continue;
                            }

                            ++l_Itr;
                        }

                        Map::PlayerList const& l_PlayerList = go->GetMap()->GetPlayers();
                        if (l_PlayerList.isEmpty())
                            return;

                        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (l_Player->GetAreaId() == go->GetAreaId())
                                {
                                    l_Player->CastSpell(l_Player, eSentinaxSpells::TheShadowOfTheSentinax, true);
                                    if (m_IsInCooldown)
                                        l_Player->CastSpell(l_Player, eSentinaxSpells::SentinaxCooldown, true);

                                    m_AffectedPlayers.insert(l_Player->GetGUID());
                                }
                            }
                        }
                    }

                    void SendTextToPlayers(eBroadcastTextID p_TextId, bool p_InChat = false, uint64 p_PlayerGUID = 0)
                    {
                        if (p_InChat)
                        {
                            Player* l_SourcePlayer = sObjectAccessor->GetPlayer(*go, p_PlayerGUID);
                            if (!l_SourcePlayer)
                                return;

                            PlayerTextBuilder l_CreatureTextBuilder(l_SourcePlayer, CHAT_MSG_MONSTER_EMOTE, p_TextId, LANG_UNIVERSAL);
                            CreatureTextLocalizer<PlayerTextBuilder> l_CreatureTextLocalizer(l_CreatureTextBuilder, CHAT_MSG_MONSTER_EMOTE);

                            for (uint64 l_PlayerGuid : m_AffectedPlayers)
                            {
                                Player* l_Player = sObjectAccessor->GetPlayer(*go, l_PlayerGuid);
                                if (!l_Player)
                                    continue;

                                l_CreatureTextLocalizer(l_Player);
                            }

                            return;
                        }
                        else
                        {
                            for (uint64 l_PlayerGuid : m_AffectedPlayers)
                            {
                                Player* l_SourcePlayer = sObjectAccessor->GetPlayer(*go, l_PlayerGuid);
                                if (!l_SourcePlayer)
                                    return;

                                PlayerTextBuilder l_CreatureTextBuilder(l_SourcePlayer, CHAT_MSG_RAID_BOSS_EMOTE, p_TextId, LANG_UNIVERSAL);
                                CreatureTextLocalizer<PlayerTextBuilder> l_CreatureTextLocalizer(l_CreatureTextBuilder, CHAT_MSG_RAID_BOSS_EMOTE);

                                Player* l_Player = sObjectAccessor->GetPlayer(*go, l_PlayerGuid);
                                if (!l_Player)
                                    continue;

                                l_CreatureTextLocalizer(l_Player);
                            }
                        }

                    }

                    std::set<uint64> m_AffectedPlayers;
                    int32 m_UpdatePlayerCooldown;
                    uint32 m_IsInCooldown;

                    int32 m_AreatriggerCooldown;

                    int32 m_ActivePortals;

                    /// Config
                    uint32  m_UpdateTime;
            };

            GameObjectAI* GetAI(GameObject* p_GameObject) const override
            {
                return new go_sentinaxAI(p_GameObject);
            }
    };

    /// Called for Shadow of the Sentinax - 240640
    /// Last Update 7.3.2 - Build 25549
    class PlayerScript_Sentinax : public PlayerScript
    {
        public:
            PlayerScript_Sentinax() : PlayerScript("PlayerScript_Sentinax") { }

            void OnTeleport(Player* p_Player, const SpellInfo* p_SpellInfo, bool p_WorldPort) override
            {
                p_Player->RemoveAura(eSentinaxSpells::TheShadowOfTheSentinax);
                p_Player->RemoveAura(eSentinaxSpells::SentinaxCooldown);
            }
    };

    /// Sentinax Destruction Zone - 10241 (SpellId : 240852)
    /// Last Update 7.3.2 - Build 25549
    class at_sentinax_destruction_zone : public AreaTriggerEntityScript
    {
        public:
            at_sentinax_destruction_zone() : AreaTriggerEntityScript("at_sentinax_destruction_zone"),
                m_RePathTimer(500), m_TargetGUID(0), m_AreatriggerSpeed(7) { }

            int32 m_RePathTimer;
            uint64 m_TargetGUID;
            uint32 m_AreatriggerSpeed;

            void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
            {
                m_RePathTimer -= p_Time;
                if (m_RePathTimer > 0)
                    return;

                m_RePathTimer += 1 * IN_MILLISECONDS;

                Player* l_Target = sObjectAccessor->GetPlayer(*p_AreaTrigger, m_TargetGUID);
                if (!m_TargetGUID)
                {
                    l_Target = p_AreaTrigger->FindNearestPlayer(10.0f);
                    m_TargetGUID = l_Target ? l_Target->GetGUID() : 0;
                }

                if (!l_Target)
                {
                    m_TargetGUID = 0;
                    return;
                }

                if (p_AreaTrigger->GetExactDist2d(l_Target) < 1.5f)
                    return;

                Position l_Dest;
                p_AreaTrigger->SimplePosXYRelocationByAngle(l_Dest, m_AreatriggerSpeed, p_AreaTrigger->GetAngle(l_Target), true);
                p_AreaTrigger->MoveAreaTrigger(l_Dest, m_AreatriggerSpeed, true, true);
            }

            AreaTriggerEntityScript* GetAI() const override
            {
                return new at_sentinax_destruction_zone();
            }
    };

    /// Called by Domination Portal - 240123
    /// Called by Firestorm Portal - 240299
    /// Called by Carnage Portal - 240302
    /// Called by Warbeasts Portal - 240305
    /// Called by Engineering Portal - 240308
    /// Called by Torment Portal - 240311
    /// Called by Greater Domination Portal - 240297
    /// Called by Greater Firestorm Portal - 240300
    /// Called by Greater Carnage Portal - 240303
    /// Called by Greater Warbeasts Portal - 240306
    /// Called by Greater Engineering Portal - 240309
    /// Called by Greater Torment Portal - 240312
    /// Last Update 7.3.2 - Build 25549
    class spell_sentinax_portal : public SpellScriptLoader
    {
        public:
            spell_sentinax_portal() : SpellScriptLoader("spell_sentinax_portal") { }

            class spell_sentinax_portal_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_portal_SpellScript);

                SpellCastResult CheckCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                    if (!l_Caster->HasAura(eSentinaxSpells::TheShadowOfTheSentinax) ||
                        l_Caster->HasAura(eSentinaxSpells::SentinaxCooldown))
                        return SpellCastResult::SPELL_FAILED_CASTER_AURASTATE;

                    GameObject* l_Sentinax = l_Caster->FindNearestGameObject(eGameObjects::GobSentinax, 400.0f);
                    if (!l_Sentinax || !l_Sentinax->AI())
                        return SpellCastResult::SPELL_FAILED_CASTER_AURASTATE;

                    if (l_Sentinax->AI()->GetData(eActions::PortalCount) > 6)
                        return SpellCastResult::SPELL_FAILED_CASTER_AURASTATE;

                    return SpellCastResult::SPELL_CAST_OK;
                }

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Unit* l_SentinaxLaser = l_Caster->FindNearestCreature(Sentinax::SentinaxLaser, 400.0f);
                    if (!l_SentinaxLaser)
                        return;

                    Creature* l_GroundLaser = l_SentinaxLaser->SummonCreature(Sentinax::GroundLaser, *l_Caster, TEMPSUMMON_MANUAL_DESPAWN, 0, 0, 0, nullptr, nullptr, true, true);
                    if (!l_GroundLaser || !l_GroundLaser->IsAIEnabled)
                        return;

                    l_GroundLaser->CastSpell(l_SentinaxLaser, eSentinaxSpells::Laser, true);
                    l_GroundLaser->GetAI()->SetData(GetBeaconCategory(), GetBeaconLevel());
                    l_GroundLaser->GetAI()->SetGUID(l_Caster->GetGUID());
                }

                uint32 GetBeaconCategory()
                {
                    return g_BeaconsData.find(m_scriptSpellId) != g_BeaconsData.end() ? g_BeaconsData[m_scriptSpellId].first : 0;
                }

                uint32 GetBeaconLevel()
                {
                    return g_BeaconsData.find(m_scriptSpellId) != g_BeaconsData.end() ? g_BeaconsData[m_scriptSpellId].second : 0;
                }

                void Register() override
                {
                    OnCheckCast += SpellCheckCastFn(spell_sentinax_portal_SpellScript::CheckCast);
                    AfterCast += SpellCastFn(spell_sentinax_portal_SpellScript::HandleAfterCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_portal_SpellScript();
            }
    };

    /// Called by Summon Thanotalion - 240298
    /// Called by Summon Skulguloth - 240301
    /// Called by Summon General Xillious - 240304
    /// Called by Summon An'thyna - 240307
    /// Called by Summon Fel Obliterator - 240310
    /// Called by Summon Illisthyndria - 240313
    /// Last Update 7.3.2 - Build 25549
    class spell_sentinax_summon_boss : public SpellScriptLoader
    {
        public:
            spell_sentinax_summon_boss() : SpellScriptLoader("spell_sentinax_summon_boss") { }

            class spell_sentinax_summon_boss_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_summon_boss_SpellScript);

                SpellCastResult CheckCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return SPELL_FAILED_DONT_REPORT;

                    if (!l_Caster->HasAura(eSentinaxSpells::TheShadowOfTheSentinax) ||
                        l_Caster->HasAura(eSentinaxSpells::SentinaxCooldown))
                        return SPELL_FAILED_CASTER_AURASTATE;

                    return SPELL_CAST_OK;
                }

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Unit* l_SentinaxLaser = l_Caster->FindNearestCreature(Sentinax::SentinaxLaser, 400.0f);
                    if (!l_SentinaxLaser)
                        return;

                    bool l_Found = false;
                    float l_MinimalDistance = 0.0f;
                    Position l_Spawn;
                    for (Position l_Pos : g_CurrentSentinaxData->PortalPositions[GetBeaconCategory()])
                    {
                        float l_Distance = l_Caster->GetExactDist2d(&l_Pos);
                        if (l_Found && l_Distance > l_MinimalDistance)
                            continue;

                        l_MinimalDistance = l_Distance;
                        l_Spawn = l_Pos;
                        l_Found = true;
                    }

                    if (!l_Found)
                        return;

                    l_Caster->GetMap()->SummonCreature(GetBossEntry(), l_Spawn);
                    SpawnWave(l_SentinaxLaser, l_Spawn);
                }

                void SpawnWave(Unit* p_SentinaxLaser, Position p_Spawn)
                {
                    for (uint32 l_Level = 1; l_Level <= 2; ++l_Level)
                    {
                        auto l_MonsterPool = g_MonsterPools.find(MAKE_PAIR64(GetBeaconCategory(), l_Level));
                        if (l_MonsterPool == g_MonsterPools.end())
                            continue;

                        if (l_MonsterPool->second->Entries.empty())
                            continue;

                        for (uint32 l_I = 0; l_I < l_MonsterPool->second->Count; ++l_I)
                        {
                            auto l_Itr = l_MonsterPool->second->Entries.begin();
                            std::advance(l_Itr, urand(0, l_MonsterPool->second->Entries.size() - 1));

                            Creature* l_Creature = p_SentinaxLaser->GetMap()->SummonCreature(*l_Itr, p_Spawn);
                            if (!l_Creature || !l_Creature->IsAIEnabled)
                                continue;

                            l_Creature->GetAI()->SetData(eDatas::SpawnIndex, l_I);
                            l_Creature->GetAI()->SetGUID(p_SentinaxLaser->GetGUID(), 1);
                        }
                    }
                }

                uint32 GetBeaconCategory()
                {
                    return g_BeaconsData.find(m_scriptSpellId) != g_BeaconsData.end() ? g_BeaconsData[m_scriptSpellId].first : 0;
                }

                uint32 GetBossEntry()
                {
                    return g_BeaconsData.find(m_scriptSpellId) != g_BeaconsData.end() ? g_BeaconsData[m_scriptSpellId].second : 0;
                }

                void Register() override
                {
                    OnCheckCast += SpellCheckCastFn(spell_sentinax_summon_boss_SpellScript::CheckCast);
                    AfterCast += SpellCastFn(spell_sentinax_summon_boss_SpellScript::HandleAfterCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_summon_boss_SpellScript();
            }
    };

    /// Sentinax Ground Laser - 121098
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_ground_laser : public CreatureScript
    {
        public:
            npc_sentinax_ground_laser() : CreatureScript("npc_sentinax_ground_laser") { }

            struct npc_sentinax_ground_laserAI : public ScriptedAI
            {
                npc_sentinax_ground_laserAI(Creature* p_Creature) : ScriptedAI(p_Creature)
                {
                    me->SetSpeed(MOVE_RUN, 3.0f, true);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }

                enum eMoveId
                {
                    ReachedPortal   = 10,
                    CircleDone      = 20
                };

                Position m_Portal;
                std::vector<Position> m_PortalsPosition;
                uint32 m_BeaconCategory = 0;
                uint32 m_BeaconLevel = 0;
                uint64 m_CasterGUID = 0;

                void SetGUID(uint64 p_GUID, int32 /*p_ID*/) override
                {
                    m_CasterGUID = p_GUID;
                }

                void SetData(uint32 p_BeaconCategory, uint32 p_BeaconLevel) override
                {
                    if (!g_CurrentSentinaxData || !p_BeaconLevel)
                        return;

                    m_BeaconCategory = p_BeaconCategory;
                    m_BeaconLevel = p_BeaconLevel;

                    float l_MinimalDistance = 0.0f;
                    bool l_Found = false;
                    for (Position l_Pos : g_CurrentSentinaxData->PortalPositions[p_BeaconCategory])
                    {
                        m_PortalsPosition.push_back(l_Pos);
                        float l_Distance = me->GetExactDist2d(&l_Pos);
                        if (l_Found && l_Distance > l_MinimalDistance)
                            continue;

                        l_MinimalDistance = l_Distance;
                        m_Portal = l_Pos;
                        l_Found = true;
                    }

                    if (!l_Found)
                        me->DespawnOrUnsummon();

                    me->DespawnOrUnsummon(20 * IN_MILLISECONDS);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMoveId::ReachedPortal, m_Portal);
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMoveId::ReachedPortal:
                        {
                            Creature* l_SentinaxLaser = me->FindNearestCreature(Sentinax::SentinaxLaser, 500.0f);
                            if (!l_SentinaxLaser)
                                break;

                            GameObject* l_Sentinax = me->FindNearestGameObject(eGameObjects::GobSentinax, 400.0f);
                            if (!l_Sentinax || !l_Sentinax->AI())
                                break;

                            bool l_AlreadySpawned = false;
                            if (GameObject* l_SpawnedPortal = me->FindNearestGameObject(eGameObjects::Portal, 1.0f))
                            {
                                l_AlreadySpawned = true;
                                if (l_SpawnedPortal->AI())
                                {
                                    if (m_BeaconLevel > l_SpawnedPortal->AI()->GetData(eDatas::PortalLevel))
                                        l_Sentinax->AI()->SetData64(eActions::EmpowerPortals, m_CasterGUID);
                                    else
                                        l_Sentinax->AI()->SetData64(eActions::ExtendPortals, m_CasterGUID);
                                }
                            }

                            if (l_AlreadySpawned)
                            {
                                std::list<GameObject*> l_Portals;
                                me->GetGameObjectListWithEntryInGrid(l_Portals, eGameObjects::Portal, 300.0f);
                                for (GameObject* l_Portal : l_Portals)
                                {
                                    if (l_Portal->AI())
                                        l_Portal->AI()->SetData(m_BeaconCategory, m_BeaconLevel);
                                }
                            }
                            else
                            {
                                l_Sentinax->AI()->SetData64(eActions::SummonedPortals, m_CasterGUID);
                                for (Position const& l_PortalPosition : m_PortalsPosition)
                                {
                                    GameObject* l_Portal = l_SentinaxLaser->SummonGameObject(Sentinax::Portal, l_PortalPosition,
                                        Position(), 0, 0, nullptr, 100, 0, false, true, eGoBCustomFlags::CustomFlagBig, true);
                                    if (!l_Portal)
                                        continue;

                                    if (!l_Portal->AI())
                                    {
                                        l_Portal->Delete();
                                        continue;
                                    }

                                    l_Portal->AI()->SetData(m_BeaconCategory, m_BeaconLevel);
                                }
                            }

                            me->GetMotionMaster()->Clear();
                            std::vector<G3D::Vector3> l_Path = GetCirclePath(m_Portal);
                            me->GetMotionMaster()->MoveSmoothPath(eMoveId::CircleDone, l_Path.data(), l_Path.size(), false);
                            break;
                        }
                        case eMoveId::CircleDone:
                        {
                            me->DespawnOrUnsummon();
                            break;
                        }
                        default:
                        {
                            me->DespawnOrUnsummon();
                            break;
                        }
                    }
                }

                std::vector<G3D::Vector3> GetCirclePath(Position const& p_CenterPos)
                {
                    float l_Radius = 6.5f;
                    std::vector<G3D::Vector3> l_Path;
                    l_Path.push_back(p_CenterPos.AsVector3());
                    for (uint32 l_I = 0; l_I < 7; ++l_I)
                    {
                        Position l_Pos;
                        p_CenterPos.SimplePosXYRelocationByAngle(l_Pos, l_Radius, l_I * M_PI / 3.0f);
                        l_Path.push_back(l_Pos.AsVector3());
                    }

                    return l_Path;
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_ground_laserAI(p_Creature);
            }
    };

    /// Sentinax Portal - 268705
    /// Last Update 7.3.2 - Build 25549
    class go_sentinax_portal : public GameObjectScript
    {
        public:
            go_sentinax_portal() : GameObjectScript("go_sentinax_portal") { }

            struct go_sentinax_portalAI : public GameObjectAI
            {
                public:
                    go_sentinax_portalAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject)
                    {
                        m_BeaconCategory = 0;
                        m_BeaconLevel = 0;
                        m_Timer = eTimers::PortalStart;
                        m_TotalTimer = eTimers::MaxTimer;
                        m_DespawnPlanned = false;
                        m_Started = false;

                        PortalLevel l_Level1 = PortalLevel();
                        l_Level1.WavesLeft = 3;
                        l_Level1.SpawnedThisWave = 0;
                        m_PortalLevels[1] = l_Level1;

                        PortalLevel l_Level2 = PortalLevel();
                        l_Level2.WavesLeft = 3;
                        l_Level2.SpawnedThisWave = 0;
                        m_PortalLevels[2] = l_Level2;

                        m_WaveTimer = eTimers::WaveTimer;
                    }

                    uint32 GetData(uint32 p_DataID) override
                    {
                        switch (p_DataID)
                        {
                            case eDatas::PortalLevel:
                                return m_BeaconLevel;
                            default:
                                break;
                        }

                        return 0;
                    }

                    void SetData(uint32 p_BeaconCategory, uint32 p_BeaconLevel) override
                    {
                        if (!p_BeaconLevel)
                        {
                            GameObject* l_Go = go;
                            go->AddDelayedEvent([l_Go]() -> void
                            {
                                GameObject* l_Sentinax = l_Go->FindNearestGameObject(eGameObjects::GobSentinax, 500.0f);
                                if (!l_Sentinax || !l_Sentinax->AI())
                                    return;

                                l_Sentinax->AI()->SetData64(eActions::DespawnedPortal, 0);

                                l_Go->Delete();
                            }, 1);
                            return;
                        }

                        if (m_BeaconCategory && m_BeaconCategory != p_BeaconCategory)
                            return;

                        if (!m_BeaconCategory)
                        {
                            m_BeaconCategory = p_BeaconCategory;
                            go->SendGameObjectActivateAnimKit(8602, true);
                        }
                        else
                        {
                            m_TotalTimer = std::min<int32>(m_TotalTimer + eTimers::AdditionalTimer, eTimers::MaxTimer);
                            m_PortalLevels[1].WavesLeft += 2;
                            m_PortalLevels[2].WavesLeft += 2;
                        }

                        if (p_BeaconLevel <= m_BeaconLevel)
                            return;

                        m_BeaconLevel = p_BeaconLevel;

                        switch (m_BeaconLevel)
                        {
                            case 1:
                                go->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, eGameObjects::PortalAdditionalVisual);
                                break;
                            case 2:
                                go->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, eGameObjects::GreaterPortalAdditionalVisual);
                                break;
                            default:
                                break;
                        }
                    }

                    void InitialSpawn()
                    {
                        for (uint32 l_Level = 1; l_Level <= 2; ++l_Level)
                        {
                            PortalLevel& l_CurrentLevel = m_PortalLevels[l_Level];

                            auto l_MonsterPool = g_MonsterPools.find(MAKE_PAIR64(m_BeaconCategory, l_Level));
                            if (l_MonsterPool == g_MonsterPools.end())
                                continue;

                            if (l_MonsterPool->second->Entries.empty())
                                continue;

                            for (uint32 l_I = 0; l_I < l_MonsterPool->second->Count; ++l_I)
                            {
                                if (m_BeaconLevel < l_Level)
                                {
                                    l_CurrentLevel.CreaturesGUID[l_I] = 0;
                                    continue;
                                }

                                auto l_Itr = l_MonsterPool->second->Entries.begin();
                                std::advance(l_Itr, urand(0, l_MonsterPool->second->Entries.size() - 1));

                                Creature* l_Creature = go->SummonCreature(*l_Itr, *go, TEMPSUMMON_MANUAL_DESPAWN, 0, 0, 0, nullptr, nullptr, false, true);
                                if (!l_Creature || !l_Creature->IsAIEnabled)
                                    continue;

                                l_Creature->GetAI()->SetGUID(go->GetGUID());
                                l_Creature->GetAI()->SetData(eDatas::SpawnIndex, l_I);

                                l_CurrentLevel.CreaturesGUID[l_I] = l_Creature->GetGUID();
                                l_CurrentLevel.SpawnedThisWave++;
                            }
                        }
                    }

                    void UpdateSpawn()
                    {
                        for (uint32 l_Level = 1; l_Level <= m_BeaconLevel; ++l_Level)
                        {
                            PortalLevel& l_CurrentLevel = m_PortalLevels[l_Level];

                            auto l_MonsterPool = g_MonsterPools.find(MAKE_PAIR64(m_BeaconCategory, l_Level));
                            if (l_MonsterPool == g_MonsterPools.end())
                                continue;

                            for (auto l_Itr = l_CurrentLevel.CreaturesGUID.begin(); l_Itr != l_CurrentLevel.CreaturesGUID.end(); ++l_Itr)
                            {
                                if (l_CurrentLevel.SpawnedThisWave >= l_MonsterPool->second->Count)
                                    continue;

                                Creature* l_Creature = sObjectAccessor->GetCreature(*go, l_Itr->second);
                                if (!l_Creature || !l_Creature->isAlive())
                                {
                                    auto l_MonsterPoolItr = l_MonsterPool->second->Entries.begin();
                                    std::advance(l_MonsterPoolItr, urand(0, l_MonsterPool->second->Entries.size() - 1));

                                    Creature* l_Creature = go->SummonCreature(*l_MonsterPoolItr, *go, TEMPSUMMON_MANUAL_DESPAWN, 0, 0, 0, nullptr, nullptr, false, true);
                                    if (!l_Creature || !l_Creature->IsAIEnabled)
                                        continue;

                                    l_Creature->GetAI()->SetGUID(go->GetGUID());
                                    l_Creature->GetAI()->SetData(eDatas::SpawnIndex, l_Itr->first);

                                    l_CurrentLevel.CreaturesGUID[l_Itr->first] = l_Creature->GetGUID();
                                    l_CurrentLevel.SpawnedThisWave++;

                                    continue;
                                }
                            }
                        }
                    }

                    void UpdateAI(uint32 p_Diff) override
                    {
                        m_TotalTimer -= p_Diff;
                        if (m_TotalTimer < 0)
                        {
                            if (m_DespawnPlanned)
                                return;

                            GameObject* l_Go = go;
                            go->AddDelayedEvent([l_Go]() -> void
                            {
                                GameObject* l_Sentinax = l_Go->FindNearestGameObject(eGameObjects::GobSentinax, 500.0f);
                                if (!l_Sentinax || !l_Sentinax->AI())
                                    return;

                                l_Sentinax->AI()->SetData64(eActions::DespawnedPortal, 0);

                                l_Go->Delete();
                            }, eTimers::DespawnTimer);

                            m_DespawnPlanned = true;
                            return;
                        }

                        m_WaveTimer -= p_Diff;
                        if (m_WaveTimer < 0)
                        {
                            m_WaveTimer = eTimers::WaveTimer;
                            m_PortalLevels[1].WavesLeft--;
                            m_PortalLevels[2].WavesLeft--;
                            m_PortalLevels[1].SpawnedThisWave = 0;
                            m_PortalLevels[2].SpawnedThisWave = 0;
                        }

                        m_Timer -= p_Diff;
                        if (m_Timer < 0)
                        {
                            m_Timer = eTimers::UpdateCooldown;

                            if (!m_Started)
                            {
                                InitialSpawn();
                                m_Started = true;
                            }
                            else
                                UpdateSpawn();
                        }
                    }

                private:
                    struct PortalLevel
                    {
                        std::map<uint32, uint64> CreaturesGUID;
                        uint32 SpawnedThisWave;
                        uint32 WavesLeft;
                    };

                    std::map<uint32, PortalLevel> m_PortalLevels;

                    uint32 m_BeaconCategory;
                    uint32 m_BeaconLevel;
                    int32 m_Timer;
                    int32 m_WaveTimer;
                    int32 m_TotalTimer;
                    bool m_DespawnPlanned;
                    bool m_Started;
            };

            GameObjectAI* GetAI(GameObject* p_GameObject) const override
            {
                return new go_sentinax_portalAI(p_GameObject);
            }
    };

    /// Sentinax Creatures Shared AI
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_creatures : public CreatureScript
    {
        public:
            npc_sentinax_creatures() : CreatureScript("npc_sentinax_creatures") { }

            struct npc_sentinax_creaturesAI : public LegionCombatAI
            {
                npc_sentinax_creaturesAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
                {
                    m_SummonerGUID = 0;
                    m_SummonedByCreature = false;
                }

                uint64 m_SummonerGUID;
                bool m_SummonedByCreature;

                void SetGUID(uint64 p_Guid, int32 p_ID /* = 0 */) override
                {
                    if (p_ID)
                        m_SummonedByCreature = true;

                    m_SummonerGUID = p_Guid;
                }

                void IsSummonedByGob(GameObject const* p_GameObject) override
                {
                    me->CastSpell(me, eSentinaxSpells::SpawnEffect, true);
                }

                void EnterCombat(Unit* p_Who) override
                {
                    LegionCombatAI::EnterCombat(p_Who);
                    me->GetMotionMaster()->Clear();
                }

                void UpdateAI(uint32 const p_Diff) override
                {
                    LegionCombatAI::UpdateAI(p_Diff);

                    if (m_SummonedByCreature)
                    {
                        if (Creature* l_Creature = sObjectAccessor->GetCreature(*me, m_SummonerGUID))
                            return;
                    }
                    else
                    {
                        if (GameObject* l_GameObject = sObjectAccessor->GetGameObject(*me, m_SummonerGUID))
                            return;
                    }

                    if (!me->isInCombat() && me->isAlive())
                        me->DespawnOrUnsummon();
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_creaturesAI(p_Creature);
            }
    };

    /// Sentinax Boss Shared AI
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_bosses : public CreatureScript
    {
        public:
            npc_sentinax_bosses() : CreatureScript("npc_sentinax_boss") { }

            struct npc_sentinax_bossAI : public LegionCombatAI
            {
                npc_sentinax_bossAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

                void InitializeAI() override
                {
                    LegionCombatAI::InitializeAI();

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        GameObject* l_Sentinax = me->FindNearestGameObject(eGameObjects::GobSentinax, 400.0f);
                        if (!l_Sentinax || !l_Sentinax->AI())
                            return;

                        l_Sentinax->AI()->SetData64(eActions::SummonedBoss, 0);

                        me->CastSpell(me, eSentinaxSpells::BossSpawnEffect, true);
                    });
                }

                void JustDied(Unit* p_Killer) override
                {
                    LegionCombatAI::JustDied(p_Killer);

                    GameObject* l_Sentinax = me->FindNearestGameObject(eGameObjects::GobSentinax, 400.0f);
                    if (!l_Sentinax || !l_Sentinax->AI())
                        return;

                    l_Sentinax->AI()->SetData64(eActions::SetInCooldown, 0);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_bossAI(p_Creature);
            }
    };

    /// Template to use to create sentinax Boss AI
    /// just replace "boss_name" everywhere by the name of the boss
/*
    /// Sentinax Bosses Shared AI
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_boss_name : public CreatureScript
    {
        public:
            npc_sentinax_boss_name() : CreatureScript("npc_sentinax_boss_name") { }

            struct npc_sentinax_boss_nameAI : public npc_sentinax_bosses::npc_sentinax_bossAI
            {
                npc_sentinax_boss_nameAI(Creature* p_Creature) : npc_sentinax_bosses::npc_sentinax_bossAI(p_Creature) { }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_boss_nameAI(p_Creature);
            }
    };*/

    /// Domination Portal Regular Creatures Movement AI
    /// Called by Hungering Stalker - 120575
    /// Called by Infectious Stalker - 120576
    /// Called by Darkfiend Devourer - 120577
    /// Called by Battlesworn Nether Mage - 120578
    /// Called by Felsworn Chaos Mage - 120579
    /// Called by Dark Ritualist - 120580
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_domination_creatures_movement : public CreatureScript
    {
        public:
            npc_sentinax_domination_creatures_movement() : CreatureScript("npc_sentinax_domination_creature_movement") { }

            struct npc_sentinax_domination_creatures_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_domination_creatures_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                std::array<std::pair<float, float>, eSpawnCounts::DominationLevel1> m_SpawnTargets =
                {{
                    { 12.0f, -M_PI / 4.0f },
                    { 12.0f, -M_PI / 6.0f },
                    { 12.0f, M_PI / 6.0f },
                    { 12.0f, M_PI / 4.0f }
                }};

                Position m_OriginalPosition;

                enum eMovesId
                {
                    ReachedDestination = 10
                };

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            if (p_Value >= m_SpawnTargets.size())
                                break;

                            m_OriginalPosition = *me;

                            Position l_Dest;
                            me->SimplePosXYRelocationByAngle(l_Dest, m_SpawnTargets[p_Value].first, m_SpawnTargets[p_Value].second);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMovesId::ReachedDestination, l_Dest.AsVector3());
                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::ReachedDestination:
                            me->SetOrientation(m_OriginalPosition.GetOrientation());
                            break;
                        default:
                            break;
                    }
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_domination_creatures_movementAI(p_Creature);
            }
    };

    /// Firestorm Portal Regular Creatures Movement AI
    /// Called by Blazing Imp - 120627
    /// Called by Virulent Imp - 120628
    /// Called by Darkfire Fiend -120629
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_firestorm_creatures_movement : public CreatureScript
    {
        public:
            npc_sentinax_firestorm_creatures_movement() : CreatureScript("npc_sentinax_firestorm_creatures_movement") { }

            struct npc_sentinax_firestorm_creatures_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_firestorm_creatures_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature)
                {
                    m_TimerInit = false;
                }

                std::array<uint32, eSpawnCounts::FirestormLevel1> m_SpawnTimers =
                {{
                    500, 1000, 1000, 1500, 1500, 4000, 4500, 4500, 5000, 5000
                }};

                int32 m_Timer;
                bool m_TimerInit;

                enum eMovesId
                {
                    ReachedDestination = 10
                };

                void IsSummonedByGob(GameObject const* p_GameObject) override
                {
                    /// npc_sentinax_creaturesAI::IsSummonedByGob(p_GameObject) must not be called for these creatures
                    LegionCombatAI::IsSummonedByGob(p_GameObject);
                    me->SetVisible(false);
                }

                bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
                {
                    if (!me->IsVisible())
                        return false;

                    return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            if (p_Value >= m_SpawnTimers.size())
                                break;

                            m_TimerInit = true;
                            m_Timer = m_SpawnTimers[p_Value];
                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::ReachedDestination:
                        {
                            me->SetHomePosition(me->GetPosition());
                            me->GetMotionMaster()->Clear();
                            me->SetWalk(true);
                            me->GetMotionMaster()->MoveRandom(3.0f);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }

                void UpdateAI(uint32 const p_Diff) override
                {
                    npc_sentinax_creaturesAI::UpdateAI(p_Diff);

                    if (!m_TimerInit)
                        return;

                    m_Timer -= p_Diff;
                    if (m_Timer > 0)
                        return;

                    me->SetVisible(true);
                    me->CastSpell(me, eSentinaxSpells::SpawnEffect, true);
                    me->GetMotionMaster()->Clear();
                    Position l_Destination;
                    me->SimplePosXYRelocationByAngle(l_Destination, 10.0f, frand(-M_PI / 4.0f, M_PI / 4.0f));
                    me->GetMotionMaster()->MovePoint(eMovesId::ReachedDestination, l_Destination);

                    m_TimerInit = false;
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_firestorm_creatures_movementAI(p_Creature);
            }
    };

    /// Firestorm Portal Elite Creatures Movement AI
    /// Called by Hellfire Infernal - 120631
    /// Called by Vile Mother - 120640
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_firestorm_elites_movement : public CreatureScript
    {
        public:
            npc_sentinax_firestorm_elites_movement() : CreatureScript("npc_sentinax_firestorm_elites_movement") { }

            struct npc_sentinax_firestorm_elites_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_firestorm_elites_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                enum eMovesId
                {
                    Going   = 10,
                    Coming  = 20
                };

                std::stack<Position> m_Path;

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            if (me->GetEntry() == eCreatures::HellfireInfernal)
                                me->CastSpell(me, eSentinaxSpells::InfernalSpawnEffect, true);

                            AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                            {
                                if (me->isInCombat())
                                    return;

                                me->SetWalk(true);
                                me->SetOrientation(me->GetOrientation() + frand(-M_PI / 2.0f, M_PI / 2.0f));
                                GenerateNextDestination();
                            });
                            break;
                        }
                        default:
                            break;
                    }
                }

                void GenerateNextDestination()
                {
                    Position l_Dest;
                    float l_Angle = 0.0f;
                    bool l_Found = false;
                    float l_Sign = -1.0f;
                    float l_Counter = M_PI / 12.0f;
                    while (l_Angle < (M_PI / 2.0f) && l_Angle > (-M_PI / 2.0f))
                    {
                        me->SimplePosXYRelocationByAngle(l_Dest, 10.0f, l_Angle);
                        if (me->IsWithinLOS(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ))
                        {
                            if (std::abs(me->GetMap()->GetHeight(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ) - l_Dest.m_positionZ) < 4.0f)
                            {
                                l_Found = true;
                                break;
                            }
                        }

                        l_Sign *= -1.0f;
                        l_Angle += (l_Sign *l_Counter);
                        l_Counter += M_PI / 12.0f;
                    }

                    if (!l_Found || (me->GetHomePosition().GetExactDist2d(me) > 40.0f))
                    {
                        if (m_Path.empty())
                            return;

                        TurnBack();
                        return;
                    }

                    m_Path.push(l_Dest);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMovesId::Going, l_Dest);
                }

                void TurnBack()
                {
                    if (m_Path.empty())
                    {
                        GenerateNextDestination();
                        return;
                    }

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMovesId::Coming, m_Path.top());

                    m_Path.pop();
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::Going:
                        {
                            GenerateNextDestination();
                            break;
                        }
                        case eMovesId::Coming:
                        {
                            TurnBack();
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_firestorm_elites_movementAI(p_Creature);
            }
    };

    /// Carnage Portal Regular Creatures Movement AI
    /// Felguard Invader - 120658
    /// Legion Vanguard - 120659
    /// Felblade Sentry - 120660
    /// Wrathguard -120661
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_carnage_creatures_movement : public CreatureScript
    {
        public:
            npc_sentinax_carnage_creatures_movement() : CreatureScript("npc_sentinax_carnage_creatures_movement") { }

            struct npc_sentinax_carnage_creatures_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_carnage_creatures_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature)
                {
                    m_IsLeader = false;
                }

                enum eMovesId
                {
                    StepReached   = 10
                };

                bool m_IsLeader;
                std::vector<uint64> m_Group;

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            me->SetWalk(true);
                            if (p_Value)
                                break;

                            m_IsLeader = true;
                            AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                            {
                                if (me->isInCombat())
                                    return;

                                std::list<Creature*> l_Group;
                                me->GetCreatureListWithEntryInGrid(l_Group, eCreatures::FelguardInvader, 2.0f);
                                me->GetCreatureListWithEntryInGrid(l_Group, eCreatures::LegionVanguard, 2.0f);
                                me->GetCreatureListWithEntryInGrid(l_Group, eCreatures::FelbladeSentry, 2.0f);
                                me->GetCreatureListWithEntryInGrid(l_Group, eCreatures::Wrathguard, 2.0f);

                                for (Creature* l_Creature : l_Group)
                                {
                                    if (l_Creature->GetGUID() == me->GetGUID())
                                        continue;

                                    m_Group.push_back(l_Creature->GetGUID());

                                    if (m_Group.size() >= 3)
                                        break;
                                }

                                GenerateNextDestination();
                            });
                            break;
                        }
                        default:
                            break;
                    }
                }

                void GenerateNextDestination()
                {
                    Position l_Dest;
                    float l_Angle = M_PI / 10.0f;
                    bool l_Found = false;
                    float l_Sign = -1.0f;
                    float l_Counter = M_PI / 12.0f;
                    while (l_Angle < (M_PI / 2.0f) && l_Angle > (-M_PI / 2.0f))
                    {
                        me->SimplePosXYRelocationByAngle(l_Dest, 10.0f, l_Angle);
                        if (me->IsWithinLOS(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ))
                        {
                            if (std::abs(me->GetMap()->GetHeight(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ) - l_Dest.m_positionZ) < 7.0f)
                            {
                                l_Found = true;
                                break;
                            }
                        }

                        l_Sign *= -1.0f;
                        l_Angle += (l_Sign *l_Counter);
                        l_Counter += M_PI / 12.0f;
                    }

                    if (!l_Found)
                        return;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMovesId::StepReached, l_Dest);

                    if (m_Group.empty())
                        return;

                    for (uint32 l_I = 0; l_I < m_Group.size(); ++l_I)
                    {
                        Creature* l_Creature = sObjectAccessor->GetCreature(*me, m_Group[l_I]);
                        if (!l_Creature || l_Creature->isInCombat())
                            continue;

                        Position l_FollowerDest;
                        float l_Dist;
                        float l_Angle;
                        switch (l_I)
                        {
                            case 0:
                                l_Dist = 4.0f;
                                l_Angle = M_PI / 2.0;
                                break;
                            case 1:
                                l_Dist = 4.0f;
                                l_Angle = M_PI;
                                break;
                            case 2:
                                l_Dist = 5.65f; ///< sqrt(4² + 4²)
                                l_Angle = 3.0f * M_PI / 4.0f;
                                break;
                            default:
                                continue;
                        }

                        l_Dest.SimplePosXYRelocationByAngle(l_FollowerDest, l_Dist, l_Angle);
                        l_Creature->GetMotionMaster()->Clear();
                        l_Creature->GetMotionMaster()->MovePoint(0, l_FollowerDest);
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::StepReached:
                        {
                            if (m_IsLeader)
                                GenerateNextDestination();

                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_carnage_creatures_movementAI(p_Creature);
            }
    };

    /// Carnage Portal Elites Creatures Movement AI
    /// Doombringer - 120662
    /// Doomguard Firecaller - 120663
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_carnage_elites_movement : public CreatureScript
    {
        public:
            npc_sentinax_carnage_elites_movement() : CreatureScript("npc_sentinax_carnage_elites_movement") { }

            struct npc_sentinax_carnage_elites_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_carnage_elites_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature)
                {
                    m_FinalOrientation = 0.0f;
                }

                enum eMovesId
                {
                    DestinationReached = 10
                };

                float m_FinalOrientation;

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            me->SetWalk(true);

                            float l_Angle = 5.0f * M_PI / 12.0f;
                            float l_OrientationOffset = - M_PI / 2.0f;
                            if (p_Value)
                            {
                                l_Angle *= -1.0f;
                                l_OrientationOffset *= -1.0f;
                            }

                            m_FinalOrientation = me->GetOrientation() + l_Angle + l_OrientationOffset;

                            Position l_Dest;
                            me->SimplePosXYRelocationByAngle(l_Dest, 10.0f, l_Angle);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMovesId::DestinationReached, l_Dest);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::DestinationReached:
                        {
                            me->GetMotionMaster()->Clear();
                            me->SetFacingTo(m_FinalOrientation);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_carnage_elites_movementAI(p_Creature);
            }
    };

    /// Warbeasts Portal Regular Creatures Movement AI
    /// Called by Legion Webspewer - 120666
    /// Called by Legion Venomretcher - 120667
    /// Called by Spellstalker - 120668
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_warbeasts_creatures_movement : public CreatureScript
    {
        public:
            npc_sentinax_warbeasts_creatures_movement() : CreatureScript("npc_sentinax_warbeasts_creatures_movement") { }

            struct npc_sentinax_warbeasts_creatures_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_warbeasts_creatures_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                enum eMovesId
                {
                    ReachedDestination = 10
                };

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            me->GetMotionMaster()->Clear();
                            Position l_Destination;
                            me->SimplePosXYRelocationByAngle(l_Destination, 10.0f, frand(-M_PI / 2.0f, M_PI / 2.0f));
                            me->GetMotionMaster()->MovePoint(eMovesId::ReachedDestination, l_Destination);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::ReachedDestination:
                        {
                            me->SetHomePosition(me->GetPosition());
                            me->GetMotionMaster()->Clear();
                            me->SetWalk(true);
                            me->GetMotionMaster()->MoveRandom(5.0f);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_warbeasts_creatures_movementAI(p_Creature);
            }
    };

    /// Warbeasts Portal Regular Creatures Movement AI
    /// Called by Dark Felshrieker - 120669
    /// Called by Rotwing Ripper - 120674
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_warbeasts_elites_movement : public CreatureScript
    {
        public:
            npc_sentinax_warbeasts_elites_movement() : CreatureScript("npc_sentinax_warbeasts_elites_movement") { }

            struct npc_sentinax_warbeasts_elites_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_warbeasts_elites_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                enum eMovesId
                {
                    ReachedDestination  = 10,
                    CircleDone          = 15,
                    ReachedGround       = 20
                };

                std::vector<G3D::Vector3> m_CirclePath;

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            SetFlyMode(true);

                            Position l_Destination;
                            me->SimplePosXYRelocationByAngle(l_Destination, frand(10.0f, 25.0f), frand(- 3.0f * M_PI / 4.0f, 3.0f * M_PI / 4.0f));
                            l_Destination.m_positionZ += frand(10.0f, 25.0f);

                            std::vector<G3D::Vector3> m_TakeOffPath;
                            m_TakeOffPath.push_back(me->GetPosition().AsVector3());
                            m_TakeOffPath.push_back(l_Destination.AsVector3());

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(eMovesId::ReachedDestination, m_TakeOffPath.data(), m_TakeOffPath.size());
                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::ReachedDestination:
                        {
                            float l_Angle = me->GetHomePosition().GetAngle(me->m_positionX, me->m_positionY);
                            float l_Dist = me->GetHomePosition().GetExactDist2d(me);
                            float l_Direction = urand(0, 1) ? -1.0f : 1.0f;
                            for (uint32 l_I = 0; l_I < 12; ++l_I)
                            {
                                Position l_Step;
                                me->GetHomePosition().SimplePosXYRelocationByAngle(l_Step, l_Dist, l_Angle, true);
                                l_Step.m_positionZ = me->m_positionZ;
                                m_CirclePath.push_back(l_Step.AsVector3());
                                l_Angle += (M_PI / 6.0f) * l_Direction;
                            }

                        /// No Break intended
                        }
                        case eMovesId::CircleDone:
                        {
                            if (m_CirclePath.empty() || me->isInCombat())
                                break;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(eMovesId::CircleDone, m_CirclePath.data(), m_CirclePath.size());
                            break;
                        }
                        case eMovesId::ReachedGround:
                        {
                            SetFlyMode(false);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMovesId::ReachedGround, *p_Who);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_warbeasts_elites_movementAI(p_Creature);
            }
    };

    /// Engineering Portal Regular Creatures Movement AI
    /// Called by Putrid Alchemist - 120676
    /// Called by Cursed Pillager - 120677
    /// Called by Gan'Arg Engineer - 120678
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_engineering_creatures_movement : public CreatureScript
    {
        public:
            npc_sentinax_engineering_creatures_movement() : CreatureScript("npc_sentinax_engineering_creatures_movement") { }

            struct npc_sentinax_engineering_creatures_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_engineering_creatures_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                enum eMovesId
                {
                    ReachedDestination = 10
                };

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            me->GetMotionMaster()->Clear();
                            Position l_Destination;
                            me->SimplePosXYRelocationByAngle(l_Destination, 5.0f, frand(-M_PI / 4.0f, M_PI / 4.0f));
                            me->GetMotionMaster()->MovePoint(eMovesId::ReachedDestination, l_Destination);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::ReachedDestination:
                        {
                            me->SetHomePosition(me->GetPosition());
                            me->GetMotionMaster()->Clear();
                            me->SetWalk(true);
                            me->GetMotionMaster()->MoveRandom(4.0f);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_engineering_creatures_movementAI(p_Creature);
            }
    };

    /// Engineering Portal Elites Creatures Movement AI
    /// Mo'Arg Bonecrusher - 120679
    /// Mo'Arg Brute - 120680
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_engineering_elites_movement : public CreatureScript
    {
        public:
            npc_sentinax_engineering_elites_movement() : CreatureScript("npc_sentinax_engineering_elites_movement") { }

            struct npc_sentinax_engineering_elites_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_engineering_elites_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature)
                {
                    m_FinalOrientation = 0.0f;
                }

                enum eMovesId
                {
                    DestinationReached = 10
                };

                float m_FinalOrientation;

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            me->CastSpell(me, eSentinaxSpells::DemonSpawn, true);
                            me->SetDisplayId(11686);
                            AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this, p_Value]() -> void
                            {
                                me->RestoreDisplayId();
                                me->SetWalk(true);

                                float l_Angle = 5.0f * M_PI / 12.0f;
                                float l_OrientationOffset = -M_PI / 2.0f;
                                if (p_Value)
                                {
                                    l_Angle *= -1.0f;
                                    l_OrientationOffset *= -1.0f;
                                }

                                m_FinalOrientation = me->GetOrientation() + l_Angle + l_OrientationOffset;

                                Position l_Dest;
                                me->SimplePosXYRelocationByAngle(l_Dest, 10.0f, l_Angle);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MovePoint(eMovesId::DestinationReached, l_Dest);
                            });

                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::DestinationReached:
                        {
                            me->GetMotionMaster()->Clear();
                            me->SetFacingTo(m_FinalOrientation);
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_engineering_elites_movementAI(p_Creature);
            }
    };

    /// Torment Portal Regular Creatures Movement AI
    /// Called by Legion Painmaiden - 120682
    /// Called by Sentinax Interrogator - 120683
    /// Called by Sentinax Observer - 120684
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_torment_creatures_movement : public CreatureScript
    {
        public:
            npc_sentinax_torment_creatures_movement() : CreatureScript("npc_sentinax_torment_creatures_movement") { }

            struct npc_sentinax_torment_creatures_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_torment_creatures_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                std::array<std::pair<float, float>, eSpawnCounts::TormentLevel1> m_SpawnTargets =
                {{
                    { 12.0f, -M_PI / 4.0f },
                    { 12.0f, -M_PI / 6.0f },
                    { 12.0f, M_PI / 6.0f },
                    { 12.0f, M_PI / 4.0f }
                }};

                Position m_OriginalPosition;

                enum eMovesId
                {
                    ReachedDestination = 10
                };

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            bool l_HasDemonSpawnEffect = me->GetEntry() != eCreatures::LegionPainmaiden;
                            if (l_HasDemonSpawnEffect)
                            {
                                me->CastSpell(me, eSentinaxSpells::DemonSpawn, true);
                                me->SetDisplayId(11686);
                            }

                            AddTimedDelayedOperation(l_HasDemonSpawnEffect ? 2 * IN_MILLISECONDS : 1, [p_Value, this]() -> void
                            {
                                me->RestoreDisplayId();

                                if (p_Value >= m_SpawnTargets.size())
                                    return;

                                m_OriginalPosition = *me;

                                Position l_Dest;
                                me->SimplePosXYRelocationByAngle(l_Dest, m_SpawnTargets[p_Value].first, m_SpawnTargets[p_Value].second);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MovePoint(eMovesId::ReachedDestination, l_Dest.AsVector3());
                            });

                            break;
                        }
                        default:
                            break;
                    }
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::ReachedDestination:
                            me->SetOrientation(m_OriginalPosition.GetOrientation());
                            break;
                        default:
                            break;
                    }
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_torment_creatures_movementAI(p_Creature);
            }
    };

    /// Torment Portal Elite Creatures Movement AI
    /// Called by Hellfire Infernal - 120631
    /// Called by Vile Mother - 120640
    /// Last Update 7.3.2 - Build 25549
    class npc_sentinax_torment_elites_movement : public CreatureScript
    {
        public:
            npc_sentinax_torment_elites_movement() : CreatureScript("npc_sentinax_torment_elites_movement") { }

            struct npc_sentinax_torment_elites_movementAI : public npc_sentinax_creatures::npc_sentinax_creaturesAI
            {
                npc_sentinax_torment_elites_movementAI(Creature* p_Creature) : npc_sentinax_creatures::npc_sentinax_creaturesAI(p_Creature) { }

                enum eMovesId
                {
                    Going   = 10,
                    Coming  = 20
                };

                std::stack<Position> m_Path;

                void SetData(uint32 p_ID, uint32 p_Value) override
                {
                    switch (p_ID)
                    {
                        case eDatas::SpawnIndex:
                        {
                            AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this, p_Value]() -> void
                            {
                                if (me->isInCombat())
                                    return;

                                me->SetWalk(true);
                                me->SetOrientation(me->GetOrientation() + (p_Value ? -M_PI / 2.0f : M_PI /2.0f ));
                                GenerateNextDestination();
                            });
                            break;
                        }
                        default:
                            break;
                    }
                }

                void GenerateNextDestination()
                {
                    Position l_Dest;
                    float l_Angle = 0.0f;
                    bool l_Found = false;
                    float l_Sign = -1.0f;
                    float l_Counter = M_PI / 12.0f;
                    while (l_Angle < (M_PI / 2.0f) && l_Angle > (-M_PI / 2.0f))
                    {
                        me->SimplePosXYRelocationByAngle(l_Dest, 10.0f, l_Angle);
                        if (me->IsWithinLOS(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ))
                        {
                            if (std::abs(me->GetMap()->GetHeight(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ) - l_Dest.m_positionZ) < 4.0f)
                            {
                                l_Found = true;
                                break;
                            }
                        }

                        l_Sign *= -1.0f;
                        l_Angle += (l_Sign *l_Counter);
                        l_Counter += M_PI / 12.0f;
                    }

                    if (!l_Found || (me->GetHomePosition().GetExactDist2d(me) > 50.0f))
                    {
                        if (m_Path.empty())
                            return;

                        TurnBack();
                        return;
                    }

                    m_Path.push(l_Dest);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMovesId::Going, l_Dest);
                }

                void TurnBack()
                {
                    if (m_Path.empty())
                    {
                        GenerateNextDestination();
                        return;
                    }

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMovesId::Coming, m_Path.top());

                    m_Path.pop();
                }

                void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
                {
                    switch (p_ID)
                    {
                        case eMovesId::Going:
                        {
                            GenerateNextDestination();
                            break;
                        }
                        case eMovesId::Coming:
                        {
                            TurnBack();
                            break;
                        }
                        default:
                            break;
                    }
                }

                void EnterCombat(Unit* p_Who) override
                {
                    npc_sentinax_creaturesAI::EnterCombat(p_Who);
                    me->SetWalk(false);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_torment_elites_movementAI(p_Creature);
            }
    };

    /// Called by Chaos Glare - 242069
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_chaos_glare : public SpellScriptLoader
    {
        public:
            spell_sentinax_chaos_glare() : SpellScriptLoader("spell_sentinax_chaos_glare") { }

            class spell_sentinax_chaos_glare_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_chaos_glare_SpellScript);

                enum eSpells
                {
                    Fear        = 242084,
                    Disoriented = 242085,
                    Polymorph   = 242088,
                    Sleep       = 242090,

                    MaxSpell    = 4
                };

                std::array<uint32, eSpells::MaxSpell> m_Debuffs
                {{
                    eSpells::Fear,
                    eSpells::Disoriented,
                    eSpells::Polymorph,
                    eSpells::Sleep
                }};

                void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetHitUnit();
                    if (!l_Caster || !l_Target)
                        return;

                    if (!l_Target->HasInArc(M_PI, l_Caster))
                        return;

                    l_Caster->CastSpell(l_Target, m_Debuffs[urand(0, (eSpells::MaxSpell - 1))], true);
                }

                void Register() override
                {
                    OnEffectHitTarget += SpellEffectFn(spell_sentinax_chaos_glare_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_chaos_glare_SpellScript();
            }
    };

    /// Called by Sickening Strike - 182145
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_sickening_strike : public SpellScriptLoader
    {
        public:
            spell_sentinax_sickening_strike() : SpellScriptLoader("spell_sentinax_sickening_strike") { }

            class spell_sentinax_sickening_strike_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_sickening_strike_SpellScript);

                enum eSpells
                {
                    Infected = 182098
                };

                void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetHitUnit();
                    if (!l_Caster || !l_Target)
                        return;

                    l_Caster->CastSpell(l_Target, eSpells::Infected, true);
                }

                void Register() override
                {
                    OnEffectHitTarget += SpellEffectFn(spell_sentinax_sickening_strike_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_sickening_strike_SpellScript();
            }
    };

    /// Called by Chaos Volley - 185808
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_chaos_volley : public SpellScriptLoader
    {
        public:
            spell_sentinax_chaos_volley() : SpellScriptLoader("spell_sentinax_chaos_volley") { }

            class spell_sentinax_chaos_volley_AuraScript : public AuraScript
            {
                PrepareAuraScript(spell_sentinax_chaos_volley_AuraScript);

                enum eSpells
                {
                    ChaosVolleyDamage = 185809
                };

                void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetTarget();
                    if (!l_Caster || !l_Target)
                        return;

                    l_Caster->CastSpell(l_Target, eSpells::ChaosVolleyDamage, true);
                }

                void Register() override
                {
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_sentinax_chaos_volley_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_sentinax_chaos_volley_AuraScript();
            }
    };

    /// Called by Chaos Volley - 185809
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_chaos_volley_damage : public SpellScriptLoader
    {
        public:
            spell_sentinax_chaos_volley_damage() : SpellScriptLoader("spell_sentinax_chaos_volley_damage") { }

            class spell_sentinax_chaos_volley_damage_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_chaos_volley_damage_SpellScript);

                enum eSpells
                {
                    ChaosVolleySummon = 185812
                };

                void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
                {
                    Unit* l_Target = GetHitUnit();
                    if (!l_Target)
                        return;

                    l_Target->CastSpell(l_Target, eSpells::ChaosVolleySummon, true);
                }

                void Register() override
                {
                    OnEffectHitTarget += SpellEffectFn(spell_sentinax_chaos_volley_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_chaos_volley_damage_SpellScript();
            }
    };

    /// Called by Beaming Gaze - 210156
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_beaming_gaze : public SpellScriptLoader
    {
        public:
            spell_sentinax_beaming_gaze() : SpellScriptLoader("spell_sentinax_beaming_gaze") { }

            class spell_sentinax_beaming_gaze_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_beaming_gaze_SpellScript);

                enum eSpells
                {
                    BeamingGazeSummon = 210157
                };

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Player* l_Target = l_Caster->FindNearestPlayer(10.0f);
                    if (!l_Target)
                        return;

                    Position l_SpawnPosition;
                    float l_Angle = M_PI / 3.0f;
                    for (uint32 l_I = 0; l_I < 3; ++l_I)
                    {
                        l_Target->SimplePosXYRelocationByAngle(l_SpawnPosition, 6.0f, l_Angle);
                        l_SpawnPosition.m_orientation = l_Target->GetOrientation() + l_Angle - (M_PI / 2.0f);
                        l_SpawnPosition.m_positionZ += 4.0f;
                        l_Caster->CastSpell(l_SpawnPosition, eSpells::BeamingGazeSummon, true);

                        l_Angle += 2.0f * M_PI / 3.0f;
                    }
                }

                void Register() override
                {
                    AfterCast += SpellCastFn(spell_sentinax_beaming_gaze_SpellScript::HandleAfterCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_beaming_gaze_SpellScript();
            }
    };

    /// Called by Beaming Gaze - 106053
    /// Last Update 7.3.5 - Build 25996
    class npc_sentinax_beaming_gaze : public CreatureScript
    {
        public:
            npc_sentinax_beaming_gaze() : CreatureScript("npc_sentinax_beaming_gaze") { }

            struct npc_sentinax_beaming_gazeAI : public Scripted_NoMovementAI
            {
                npc_sentinax_beaming_gazeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
                {
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                }

                enum eSpells
                {
                    BeamingGazeAreatrigger = 210161
                };

                void IsSummonedBy(Unit* /*p_Summoner*/)
                {
                    me->CastSpell(me, eSpells::BeamingGazeAreatrigger, true);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveRotate(10 * IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_RIGHT);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_beaming_gazeAI(p_Creature);
            }
    };

    /// Fel Obliterator - 120681
    /// Last Update 7.3.5 - Build 25996
    class npc_sentinax_fel_obliterator : public CreatureScript
    {
        public:
            npc_sentinax_fel_obliterator() : CreatureScript("npc_sentinax_fel_obliterator") { }

            struct npc_sentinax_fel_obliteratorAI : public npc_sentinax_bosses::npc_sentinax_bossAI
            {
                npc_sentinax_fel_obliteratorAI(Creature* p_Creature) : npc_sentinax_bosses::npc_sentinax_bossAI(p_Creature) { }

                enum eEvents
                {
                    EventFelOrb = 1
                };

                bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
                {
                    switch (p_EventID)
                    {
                        case eEvents::EventFelOrb:
                        {
                            std::list<Unit*> l_Targets;
                            float l_Radius = GetEventData(p_EventID).AttackDist;
                            JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck l_Check(me, l_Radius);
                            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> l_Searcher(me, l_Targets, l_Check);
                            me->VisitNearbyObject(l_Radius, l_Searcher);

                            if (l_Targets.empty())
                                return false;

                            l_Targets.sort(JadeCore::ObjectDistanceOrderPredPlayer(me, true));
                            uint32 l_TargetCount = std::min<uint32>(l_Targets.size(), 3);

                            auto l_Itr = l_Targets.begin();
                            for (uint32 l_I = 0; l_I < l_TargetCount; ++l_I)
                                me->CastSpell((*l_Itr), GetEventData(p_EventID).SpellID, true);

                            return true;
                        }
                        default:
                            break;
                    }

                    return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }
            };

            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_sentinax_fel_obliteratorAI(p_Creature);
            }
    };

    /// Called by Overrun - 226209
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_overrun : public SpellScriptLoader
    {
        public:
            spell_sentinax_overrun() : SpellScriptLoader("spell_sentinax_overrun") { }

            class spell_sentinax_overrun_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_overrun_SpellScript);

                enum eSpells
                {
                    OverrunDash         = 226210,
                    OverrunAreatrigger  = 226212
                };

                void FilterTargets(std::list<WorldObject*>& p_Targets)
                {
                    Unit* l_Caster = GetCaster();
                    if (p_Targets.empty() || !l_Caster)
                        return;

                    p_Targets.remove_if([](WorldObject* p_Object) -> bool
                    {
                        if (p_Object->IsPlayer())
                            return false;

                        return true;
                    });

                    if (p_Targets.empty())
                        return;

                    p_Targets.sort(JadeCore::DistanceOrderPred(l_Caster));
                    WorldObject* l_Target = p_Targets.back();

                    p_Targets.clear();
                    p_Targets.push_back(l_Target);
                }

                void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetHitUnit();
                    if (!l_Caster || !l_Target)
                        return;

                    l_Caster->CastSpell(l_Caster, eSpells::OverrunAreatrigger, true);
                    l_Caster->CastSpell(l_Target, eSpells::OverrunDash, true);
                }

                void Register() override
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sentinax_overrun_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnEffectHitTarget += SpellEffectFn(spell_sentinax_overrun_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_overrun_SpellScript();
            }
    };

    /// Called by Fel Growth - 240924
    /// Called for item Sentinax Beacon of Fel Growth - 146922
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_fel_growth : public SpellScriptLoader
    {
        public:
            spell_sentinax_fel_growth() : SpellScriptLoader("spell_sentinax_fel_growth") { }

            class spell_sentinax_fel_growth_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_fel_growth_SpellScript);

                enum eSpells
                {
                    FelGrowthRoot     = 240927,
                    FelGrowthSpawn    = 240926
                };

                SpellCastResult CheckCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return SPELL_FAILED_DONT_REPORT;

                    if (l_Caster->isInCombat())
                        return SPELL_FAILED_TARGET_IN_COMBAT;

                    if (!l_Caster->HasAura(eSentinaxSpells::TheShadowOfTheSentinax) ||
                        l_Caster->HasAura(eSentinaxSpells::SentinaxCooldown))
                        return SPELL_FAILED_CASTER_AURASTATE;

                    return SPELL_CAST_OK;
                }

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->CastSpell(l_Caster, eSpells::FelGrowthRoot, true);

                    for (uint32 l_I = 0; l_I < 6; l_I++)
                    {
                        Position l_Dest;
                        l_Caster->SimplePosXYRelocationByAngle(l_Dest, 4.0f, static_cast<float>(l_I) * M_PI / 3.0f, true);
                        l_Dest.m_orientation = static_cast<float>(l_I) * M_PI / 3.0f + M_PI;
                        l_Caster->CastSpell(l_Dest, eSpells::FelGrowthSpawn, true);
                    }
                }

                void Register() override
                {
                    AfterCast += SpellCastFn(spell_sentinax_fel_growth_SpellScript::HandleAfterCast);
                    OnCheckCast += SpellCheckCastFn(spell_sentinax_fel_growth_SpellScript::CheckCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_fel_growth_SpellScript();
            }
    };

    /// Called by Petrification - 240888
    /// Called for item Sentinax Beacon of Petrification - 146923
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_petrification : public SpellScriptLoader
    {
        public:
            spell_sentinax_petrification() : SpellScriptLoader("spell_sentinax_petrification") { }

            class spell_sentinax_petrification_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_petrification_SpellScript);

                enum eSpells
                {
                    PetrificationStun   = 240903,
                    PetrificationSpawn  = 240895
                };

                SpellCastResult CheckCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return SPELL_FAILED_DONT_REPORT;

                    if (l_Caster->isInCombat())
                        return SPELL_FAILED_TARGET_IN_COMBAT;

                    if (!l_Caster->HasAura(eSentinaxSpells::TheShadowOfTheSentinax) ||
                        l_Caster->HasAura(eSentinaxSpells::SentinaxCooldown))
                        return SPELL_FAILED_CASTER_AURASTATE;

                    return SPELL_CAST_OK;
                }

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->CastSpell(l_Caster, eSpells::PetrificationStun, true);

                    for (uint32 l_I = 0; l_I < 6; l_I++)
                    {
                        Position l_Dest;
                        l_Caster->SimplePosXYRelocationByAngle(l_Dest, 4.0f, static_cast<float>(l_I) * M_PI / 3.0f, true);
                        l_Dest.m_orientation = static_cast<float>(l_I) * M_PI / 3.0f + M_PI;
                        l_Caster->CastSpell(l_Dest, eSpells::PetrificationSpawn, true);
                    }
                }

                void Register() override
                {
                    AfterCast += SpellCastFn(spell_sentinax_petrification_SpellScript::HandleAfterCast);
                    OnCheckCast += SpellCheckCastFn(spell_sentinax_petrification_SpellScript::CheckCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_petrification_SpellScript();
            }
    };

    /// Called by Bloodstrike - 240936
    /// Called for item Sentinax Beacon of the Bloodstrike - 147355
    /// Last Update 7.3.5 - Build 25996
    class spell_sentinax_bloodstrike : public SpellScriptLoader
    {
        public:
            spell_sentinax_bloodstrike() : SpellScriptLoader("spell_sentinax_bloodstrike") { }

            class spell_sentinax_bloodstrike_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_sentinax_bloodstrike_SpellScript);

                enum eSpells
                {
                    BloodstrikeDoT      = 240939,
                    BloodstrikeSpawn    = 240937
                };

                SpellCastResult CheckCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return SPELL_FAILED_DONT_REPORT;

                    if (l_Caster->isInCombat())
                        return SPELL_FAILED_TARGET_IN_COMBAT;

                    if (!l_Caster->HasAura(eSentinaxSpells::TheShadowOfTheSentinax) ||
                        l_Caster->HasAura(eSentinaxSpells::SentinaxCooldown))
                        return SPELL_FAILED_CASTER_AURASTATE;

                    return SPELL_CAST_OK;
                }

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->CastSpell(l_Caster, eSpells::BloodstrikeDoT, true);

                    for (uint32 l_I = 0; l_I < 6; l_I++)
                    {
                        Position l_Dest;
                        l_Caster->SimplePosXYRelocationByAngle(l_Dest, 4.0f, static_cast<float>(l_I) * M_PI / 3.0f, true);
                        l_Dest.m_orientation = static_cast<float>(l_I) * M_PI / 3.0f + M_PI;
                        l_Caster->CastSpell(l_Dest, eSpells::BloodstrikeSpawn, true);
                    }
                }

                void Register() override
                {
                    AfterCast += SpellCastFn(spell_sentinax_bloodstrike_SpellScript::HandleAfterCast);
                    OnCheckCast += SpellCheckCastFn(spell_sentinax_bloodstrike_SpellScript::CheckCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_sentinax_bloodstrike_SpellScript();
            }
    };
}

#ifndef __clang_analyzer__
void AddSC_the_sentinax()
{
    /// Sentinax
    new Sentinax::POI_Sentinax();
    new Sentinax::go_sentinax();
    new Sentinax::PlayerScript_Sentinax();
    new Sentinax::at_sentinax_destruction_zone();

    /// Portal/Summons
    new Sentinax::spell_sentinax_portal();
    new Sentinax::spell_sentinax_summon_boss();
    new Sentinax::npc_sentinax_ground_laser();
    new Sentinax::go_sentinax_portal();

    /// Parent AIs
    new Sentinax::npc_sentinax_creatures();
    new Sentinax::npc_sentinax_bosses();

    /// Creatures
    new Sentinax::npc_sentinax_domination_creatures_movement();
    new Sentinax::npc_sentinax_firestorm_creatures_movement();
    new Sentinax::npc_sentinax_firestorm_elites_movement();
    new Sentinax::npc_sentinax_carnage_creatures_movement();
    new Sentinax::npc_sentinax_carnage_elites_movement();
    new Sentinax::npc_sentinax_warbeasts_creatures_movement();
    new Sentinax::npc_sentinax_warbeasts_elites_movement();
    new Sentinax::npc_sentinax_engineering_creatures_movement();
    new Sentinax::npc_sentinax_engineering_elites_movement();
    new Sentinax::npc_sentinax_torment_creatures_movement();
    new Sentinax::npc_sentinax_torment_elites_movement();

    /// Spells
    new Sentinax::spell_sentinax_chaos_glare();
    new Sentinax::spell_sentinax_sickening_strike();
    new Sentinax::spell_sentinax_chaos_volley();
    new Sentinax::spell_sentinax_chaos_volley_damage();
    new Sentinax::spell_sentinax_beaming_gaze();
    new Sentinax::npc_sentinax_beaming_gaze();

    /// Bosses
    new Sentinax::npc_sentinax_fel_obliterator();
    new Sentinax::spell_sentinax_overrun();

    /// Ressources Beacons
    new Sentinax::spell_sentinax_fel_growth();
    new Sentinax::spell_sentinax_petrification();
    new Sentinax::spell_sentinax_bloodstrike();
}
#endif
