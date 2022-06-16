////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2020 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "seat_of_triumvirate.hpp"

DoorData const g_DoorData[] =
{
    { eGameObjects::GoLuraCombatDoor,   eData::DataLura,    DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { 0,                                0,                  DOOR_TYPE_PASSAGE,  BOUNDARY_NONE }
};

using ObjectEntry = uint32;
using ObjectGUID = uint64;

using ObjectInfo        = std::pair<ObjectEntry, ObjectGUID>;
using CreatureEvent     = std::pair<ObjectEntry, EncounterState>;
using DataEvent         = std::pair<eData, EncounterState>;

class instance_seat_of_triumvirate : public InstanceMapScript
{
    public:
        instance_seat_of_triumvirate() : InstanceMapScript("instance_seat_of_triumvirate", 1753) { }

        struct instance_seat_of_triumvirate_MapScript : public InstanceScript
        {
            explicit instance_seat_of_triumvirate_MapScript(InstanceMap* p_InstanceMap) : InstanceScript(p_InstanceMap) { }

            void Initialize() override
            {
                m_EncounterGuids.clear();
                m_CreatureEvents.clear();
                m_ZuraalGuards      = 0;
                m_LuraGuards        = 0;
                m_SaprishPortals    = 0;

                m_PortalBehindSaprishGUID   = 0;
                m_FirstPortalSaprishGUID    = 0;
                m_ControllerGuid    = 0;
                m_TimerHoles = 0;

                InstanceScript::SetBossNumber(g_EncountersMax);
                InstanceScript::LoadDoorData(g_DoorData);

                instance->SetObjectVisibility(2500.f);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                if (p_Creature == nullptr)
                    return;

                if (IsChallenge() && p_Creature->IsHostileToPlayers())
                {
                    if (Aura* l_Might = p_Creature->GetAura(ChallengeSpells::ChallengersMight))
                        l_Might->Remove();

                    p_Creature->DelayedCastSpell(p_Creature, ChallengeSpells::ChallengersMight, true, IN_MILLISECONDS + 500);
                }

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::NpcVoidBladeZedaat:
                    {
                        if (!sWorldQuestGenerator->HasQuestActive(eWorldQuests::VoidBladeZedaat))
                        {
                            p_Creature->SetReactState(REACT_PASSIVE);
                            p_Creature->SetVisible(false);
                            p_Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        }

                        break;
                    }

                    case eCreatures::NpcController:
                    {
                        m_ControllerGuid = p_Creature->GetGUID();
                        break;
                    }

                    case eCreatures::NpcAlleriaZuraal:
                    case eCreatures::NpcAlleriaSaprish:
                    case eCreatures::NpcAlleriaNezhar:
                    case eCreatures::NpcAlleriaLura:
                    case eCreatures::NpcLocusWalkerZuraal:
                    case eCreatures::NpcLocusWalkerNezhar:
                    case eCreatures::NpcLocusWalkerLura:
                    {
                        p_Creature->SetVisible(false);
                        m_CreatureEvents.push_back(CreatureEvent(p_Creature->GetEntry(), EncounterState::NOT_STARTED));
                        m_EncounterGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }

                    case eCreatures::NpcCenterPoint:
                    {
                        m_EncounterGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }

                    case eCreatures::NpcVoidFlayer:
                    {
                        p_Creature->AddUnitState(UNIT_STATE_ROOT);
                        break;
                    }

                    case eCreatures::NpcVoidPortalTrash:
                    {
                        Position l_NezharPortalPos = { 6122.069f, 10366.900f, 19.920f };

                        if (GetData(eCreatures::BossLura) == EncounterState::DONE)
                            p_Creature->SetVisible(false);
                        else if (p_Creature->IsNearPosition(&l_NezharPortalPos, 5.f))
                        {
                            m_PortalVoidNezharGuid = p_Creature->GetGUID();
                            p_Creature->SetVisible(false);
                        }
                        else
                            p_Creature->SetVisible(true);

                        break;
                    }

                    case eCreatures::NpcVoidPortalStalker:
                    {
                        Position l_FirstPortal = g_SaprishPortalsPositions.front();
                        Position l_PortalBehindSaprish = g_SaprishPortalsPositions.back();

                        if (p_Creature->IsNearPosition(&l_FirstPortal, 10.f))
                            m_FirstPortalSaprishGUID = p_Creature->GetGUID();
                        else if (p_Creature->IsNearPosition(&l_PortalBehindSaprish, 10.f))
                            m_PortalBehindSaprishGUID = p_Creature->GetGUID();
                        else
                            m_SaprishPortalsGuids.insert(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }

                    case eCreatures::NpcVoidPortalDummy:
                    {
                        p_Creature->SetVisible(false);
                        break;
                    }

                    case eCreatures::BossZuraalTheAscended:
                    case eCreatures::NpcLuraDoorStalker:
                    case eCreatures::BossLura:
                    {
                        m_CreatureEvents.push_back(CreatureEvent(p_Creature->GetEntry(), EncounterState::NOT_STARTED));
                        m_EncounterGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }

                    case eCreatures::NpcUrjad:
                    case eCreatures::BossViceroyNezhar:
                    {
                        m_EncounterGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }

                    case eCreatures::BossSaprish:
                    {
                        m_CreatureEvents.push_back(CreatureEvent(p_Creature->GetEntry(), EncounterState::NOT_STARTED));
                        m_EncounterGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        AddToDamageManager(p_Creature, 0);
                        break;
                    }

                    case eCreatures::NpcDarkfang:
                    case eCreatures::NpcShadewing:
                    {
                        AddToDamageManager(p_Creature, 0);
                        break;
                    }

                    case eCreatures::NpcFragementedVoidling:
                    {
                        p_Creature->AddDelayedEvent([p_Creature]() -> void
                        {
                            if (Unit* l_Target = p_Creature->SelectNearestTargetInAttackDistance(20.f))
                                p_Creature->CombatStart(l_Target, true);

                        }, 2 * IN_MILLISECONDS);

                        break;
                    }

                    default: break;
                }
            }

            bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
            {
                if (p_Target == nullptr)
                    return false;

                switch (p_Target->GetEntry())
                {
                    case eCreatures::NpcVoidBladeZedaat:
                    case eCreatures::NpcGreatRiftWarden:
                    case eCreatures::NpcDarkfang:
                    case eCreatures::NpcShadewing:
                    case eCreatures::NpcWaningVoid:
                    case eCreatures::NpcVoidTrap:
                    case eCreatures::NpcCoalescedVoid:
                    case eCreatures::NpcSaprishStaff:
                    case eCreatures::NpcCenterPoint:
                    case eCreatures::NpcRiftWardenSaprish:
                    case eCreatures::NpcRiftWarden:
                        return false;

                    case eCreatures::BossLura:
                    case eCreatures::BossSaprish:
                    case eCreatures::BossViceroyNezhar:
                    case eCreatures::BossZuraalTheAscended:
                        return false;

                    case eCreatures::NpcShadowguardSubjugator:
                    {
                        if (Unit* l_Target = const_cast<Unit*>(p_Target))
                        {
                            if (l_Target->IsAIEnabled)
                                return l_Target->GetAI()->GetData(eData::DataBossIntro) != 2;
                        }

                        break;
                    }

                    default: break;
                }

                return true;
            }

            void OnGameObjectCreate(GameObject* p_Go) override
            {
                if (p_Go == nullptr)
                    return;

                switch (p_Go->GetEntry())
                {
                    case eGameObjects::GoSaprishEntranceDoor:
                    {
                        if (GetData(eCreatures::NpcAlleriaSaprish) == EncounterState::DONE)
                            HandleGameObject(0, true, p_Go);
                        else
                            HandleGameObject(0, false, p_Go);

                        m_EncounterGuids.push_back(ObjectInfo(p_Go->GetEntry(), p_Go->GetGUID()));

                        break;
                    }

                    case eGameObjects::GoLuraEntranceDoor:
                    {
                        if (GetData(eCreatures::NpcAlleriaNezhar) == EncounterState::DONE)
                            HandleGameObject(0, true, p_Go);
                        else
                            HandleGameObject(0, false, p_Go);

                        m_EncounterGuids.push_back(ObjectInfo(p_Go->GetEntry(), p_Go->GetGUID()));

                        break;
                    }

                    case eGameObjects::GoLuraCombatDoor:
                    {
                        InstanceScript::AddDoor(p_Go, true);
                        break;
                    }


                    default: break;
                }
            }

            void OnGameObjectRemove(GameObject* p_Go) override
            {
                if (p_Go == nullptr)
                    return;

                switch (p_Go->GetEntry())
                {
                    case eGameObjects::GoLuraCombatDoor:
                    {
                        InstanceScript::AddDoor(p_Go, false);
                        break;
                    }

                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* p_Unit) override
            {
                if (p_Unit == nullptr)
                    return;

                switch (p_Unit->GetEntry())
                {
                    case eCreatures::NpcShadowguardSubjugator:
                    {
                        if (p_Unit->IsAIEnabled)
                        {
                            if (p_Unit->GetAI()->GetData(eData::DataBossIntro) == 2)
                            {
                                m_ZuraalGuards++;

                                if (m_ZuraalGuards >= 4)
                                    StartCreatureEvent(eCreatures::BossZuraalTheAscended);
                            }
                        }

                        break;
                    }

                    case eCreatures::NpcRiftWarden:
                    {
                        m_LuraGuards++;

                        if (m_LuraGuards >= 2)
                        {
                            StartCreatureEvent(eCreatures::BossLura);
                            StartCreatureEvent(eCreatures::NpcAlleriaLura);
                        }

                        break;
                    }

                    case eCreatures::NpcRiftWardenSaprish:
                    {
                        m_SaprishPortals++;

                        if (m_SaprishPortals >= 3)
                        {
                            StartCreatureEvent(eCreatures::BossSaprish);
                            StartCreatureEvent(eCreatures::NpcDarkfang);
                        }
                        else
                        {
                            AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this] () -> void
                            {
                                uint64 l_NextPortal = GetRandomPortalSaprish();

                                if (Unit* l_Portal = instance->GetCreature(l_NextPortal))
                                {
                                    DoCastSpellOnPlayers(GetSaprishConversationWave());
                                    if (l_Portal->IsAIEnabled)
                                        l_Portal->GetAI()->DoAction(eSharedActions::ActionSaprishPortalOpen);
                                }
                            });
                        }

                        break;
                    }
                }
            }

            uint32 GetSaprishConversationWave()
            {
                switch (m_SaprishPortals)
                {
                    case 0:
                        return eSharedSpells::SpellSaprishWaveOneConv;

                    case 1:
                        return eSharedSpells::SpellSaprishWaveTwoConv;

                    case 2:
                        return eSharedSpells::SpellSaprishWaveEndConv;

                    default:
                        return 0;
                }
            }

            void OnCreatureRemove(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::BossSaprish:
                    case eCreatures::NpcDarkfang:
                    case eCreatures::NpcShadewing:
                    {
                        RemoveFromDamageManager(p_Creature, 0);
                        break;
                    }

                    default:
                        break;
                }
            }

            void SetData(uint32 p_Entry, uint32 p_State) override
            {
                auto l_Itr = std::find_if(m_CreatureEvents.begin(), m_CreatureEvents.end(),
                    [&p_Entry](CreatureEvent const & p_Itr) -> bool
                {
                    return p_Entry == p_Itr.first;
                });

                if (l_Itr != m_CreatureEvents.end())
                    l_Itr->second = EncounterState(p_State);

                if (p_State == EncounterState::DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 p_Data) override
            {
                auto l_Itr = std::find_if(m_CreatureEvents.begin(), m_CreatureEvents.end(),
                    [&p_Data](CreatureEvent const & p_Itr) -> bool
                {
                    return p_Data == p_Itr.first;
                });

                if (l_Itr != m_CreatureEvents.end())
                    return l_Itr->second;

                return 0;
            }

            uint64 GetRandomPortalSaprish()
            {
                uint64 l_Ret = 0;

                if (!m_SaprishPortalsGuids.empty())
                {
                    ObjectInfo const l_Portal = JadeCore::Containers::SelectRandomContainerElement(m_SaprishPortalsGuids);

                    l_Ret = l_Portal.second;

                    m_SaprishPortalsGuids.erase(l_Portal);
                }

                return l_Ret;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                if (p_Type == eData::DataFirstPortalSaprish)
                    return m_FirstPortalSaprishGUID;

                if (p_Type == eData::DataPortalBehindSaprish)
                    return m_PortalBehindSaprishGUID;

                auto l_Itr = std::find_if(m_EncounterGuids.begin(), m_EncounterGuids.end(), [&p_Type](ObjectInfo& p_Itr)
                {
                    return p_Itr.first == p_Type;
                });

                if (l_Itr != m_EncounterGuids.end())
                    return l_Itr->second;

                return 0;
            }

            void StartCreatureEvent(uint32 const p_Entry)
            {
                auto l_Itr = std::find_if(m_CreatureEvents.begin(), m_CreatureEvents.end(),
                [&p_Entry] (CreatureEvent const & p_Itr) -> bool
                {
                    return p_Entry == p_Itr.first;
                });

                if (l_Itr != m_CreatureEvents.end())
                {
                    if (l_Itr->second == EncounterState::DONE)
                        return;
                }

                Creature* l_Creature = instance->GetCreature(GetData64(p_Entry));

                if (l_Creature == nullptr)
                    return;

                if (l_Creature->IsAIEnabled)
                    l_Creature->GetAI()->DoAction(eSharedActions::ActionCreatureEvent);
            }

            bool SetBossState(uint32 p_Data, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Data, p_State))
                    return false;

                switch (p_Data)
                {
                    case eData::DataZuraal:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            StartCreatureEvent(eCreatures::NpcAlleriaZuraal);
                            StartCreatureEvent(eCreatures::NpcLocusWalkerZuraal);
                        }

                        break;
                    }

                    case eData::DataSaprish:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            Creature* l_Nezhar = instance->GetCreature(GetData64(eCreatures::BossViceroyNezhar));

                            if (l_Nezhar != nullptr && l_Nezhar->IsAIEnabled)
                                l_Nezhar->AI()->Reset();
                        }
                        break;
                    }

                    case eData::DataNezhar:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            StartCreatureEvent(eCreatures::NpcLocusWalkerNezhar);
                            StartCreatureEvent(eCreatures::NpcAlleriaNezhar);
                        }

                        break;
                    }

                    case eData::DataLura:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            Creature* l_Alleria = instance->GetCreature(GetData64(eCreatures::NpcAlleriaLura));
                            Creature* l_Walker = instance->GetCreature(GetData64(eCreatures::NpcLocusWalkerLura));

                            if (l_Alleria != nullptr && l_Alleria->IsAIEnabled)
                                l_Alleria->GetAI()->DoAction(eSharedActions::ActionLuraOutro);

                            if (l_Walker != nullptr && l_Walker->IsAIEnabled)
                                l_Walker->GetAI()->DoAction(eSharedActions::ActionLuraOutro);
                        }
                        break;
                    }

                    default: break;
                }

                return true;
            }

            std::string GetSaveData() override
            {
                std::ostringstream l_Data;

                l_Data << GetBossSaveData();

                for (CreatureEvent const & p_Itr : m_CreatureEvents)
                    l_Data << p_Itr.second << ' ';

                l_Data << m_ZuraalGuards    << ' ';
                l_Data << m_SaprishPortals  << ' ';
                l_Data << m_LuraGuards      << ' ';

                return l_Data.str();
            }

            void Load(char const* p_Data) override
            {
                std::istringstream l_Data(p_Data);
                uint32 l_State;

                for (uint8 l_Itr = 0; l_Itr < g_EncountersMax; ++l_Itr)
                {
                    l_Data >> l_State;

                    if (l_State != EncounterState::DONE)
                        l_State = EncounterState::NOT_STARTED;

                    InstanceScript::SetBossState(l_Itr, EncounterState(l_State));
                }

                for (CreatureEvent & l_Itr : m_CreatureEvents)
                {
                    l_Data >> l_State;

                    if (l_State != EncounterState::DONE)
                        l_State = EncounterState::NOT_STARTED;

                    l_Itr.second = EncounterState(l_State);
                }

                l_Data >> m_ZuraalGuards;
                l_Data >> m_SaprishPortals;
                l_Data >> m_LuraGuards;

                if (m_ZuraalGuards >= 4 && GetBossState(eData::DataZuraal) != DONE)
                {
                    AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                    {
                       StartCreatureEvent(eCreatures::BossZuraalTheAscended);
                    });
                }

                if (m_SaprishPortals >= 3 && GetBossState(eData::DataSaprish) != DONE)
                {
                    AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                    {
                        DoCastSpellOnPlayers(GetSaprishConversationWave());
                        StartCreatureEvent(eCreatures::BossSaprish);
                        StartCreatureEvent(eCreatures::NpcDarkfang);
                    });
                }

                if (m_LuraGuards >= 2 && GetBossState(eData::DataLura) != DONE)
                {
                    AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                    {
                        StartCreatureEvent(eCreatures::BossLura);
                        StartCreatureEvent(eCreatures::NpcAlleriaLura);
                    });
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (GetBossState(eData::DataNezhar) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocations::LocNezhar);
                else if (GetBossState(eData::DataSaprish) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocations::LocSaprish);
                else if (GetData(eCreatures::BossSaprish) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocations::LocSaprishEv);
                else if (GetBossState(eData::DataZuraal) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocations::LocZuraal);
                else
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocations::LocEntrance);

                if (l_Loc == nullptr)
                    return;

                p_X = l_Loc->x;
                p_Y = l_Loc->y;
                p_Z = l_Loc->z;
                p_O = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            void Update(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                InstanceScript::Update(p_Diff);

                m_TimerHoles += p_Diff;

                if (m_TimerHoles < IN_MILLISECONDS)
                    return;

                m_TimerHoles = 0;

                Map::PlayerList const& l_Targets = instance->GetPlayers();

                for (uint8 l_Index = 0; l_Index < 4; ++l_Index)
                {
                    auto l_Hole = g_VoidBlackHole.at(l_Index);

                    for (auto l_Itr = l_Targets.begin(); l_Itr != l_Targets.end(); ++l_Itr)
                    {
                        Player* l_Target = l_Itr->getSource();

                        if (l_Target == nullptr)
                            continue;

                        float l_Dist = l_Target->GetDistance2d(l_Hole.first.m_positionX, l_Hole.first.m_positionY);

                        if (l_Dist <= l_Hole.second)
                        {
                            if (l_Dist <= 2.75f && !l_Target->HasAura(SpellCrushingDarkness))
                                l_Target->CastSpell(l_Target, eSharedSpells::SpellGrowingPressure, true);

                            if (!l_Target->HasMovementForce(m_ControllerGuid))
                            {
                                m_VoidHoleMap.insert(std::make_pair(l_Target->GetGUID(), l_Index));
                                l_Target->SendApplyMovementForce(m_ControllerGuid, true, l_Hole.first, 1.5f, 1);
                            }
                        }
                        else
                        {
                            auto l_Curr_Target = m_VoidHoleMap.find(l_Target->GetGUID());

                            if (l_Curr_Target != m_VoidHoleMap.end())
                            {
                                if (l_Curr_Target->second == l_Index)
                                {
                                    l_Target->SendApplyMovementForce(m_ControllerGuid, false, l_Hole.first);
                                    m_VoidHoleMap.erase(l_Target->GetGUID());
                                    l_Target->RemoveAurasDueToSpell(eSharedSpells::SpellGrowingPressure);
                                }
                            }
                        }
                    }
                }
            }

            private:
                std::vector<ObjectInfo>     m_EncounterGuids;
                std::set<ObjectInfo>        m_SaprishPortalsGuids;
                std::vector<CreatureEvent>  m_CreatureEvents;
                std::map<uint64, uint8>     m_VoidHoleMap;
                uint8                       m_ZuraalGuards;
                uint8                       m_LuraGuards;
                uint8                       m_SaprishPortals;
                uint64                      m_FirstPortalSaprishGUID;
                uint64                      m_PortalBehindSaprishGUID;
                uint64                      m_PortalVoidNezharGuid;
                uint64                      m_ControllerGuid;
                uint32                      m_TimerHoles;
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_InstanceMap) const override
        {
            return new instance_seat_of_triumvirate_MapScript(p_InstanceMap);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_seat_of_triumvirate()
{
    new instance_seat_of_triumvirate();
}
#endif
