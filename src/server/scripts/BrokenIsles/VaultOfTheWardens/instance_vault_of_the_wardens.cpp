////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "vault_of_the_wardens.hpp"

DoorData const g_DoorData[] =
{
    { GO_SALTHERIL_DOOR_1,       DATA_SALTHERIL,        DOOR_TYPE_ROOM,          BOUNDARY_NONE },
    { GO_SALTHERIL_DOOR_2,       DATA_SALTHERIL,        DOOR_TYPE_PASSAGE,       BOUNDARY_NONE },
    { GO_SALTHERIL_DOOR_3,       DATA_SALTHERIL,        DOOR_TYPE_PASSAGE,       BOUNDARY_NONE },
    { GO_SALTHERIL_DOOR_4,       DATA_SALTHERIL,        DOOR_TYPE_PASSAGE,       BOUNDARY_NONE },
    { GO_SALTHERIL_DOOR_5,       DATA_SALTHERIL,        DOOR_TYPE_PASSAGE,       BOUNDARY_NONE },
    { GO_ASHGOLM_PLATFORM,       DATA_ASHGOLM,          DOOR_TYPE_ROOM,          BOUNDARY_NONE },
    { GO_ASHGOLM_BRIDGE,         DATA_ASHGOLM,          DOOR_TYPE_SPAWN_HOLE,    BOUNDARY_NONE },
    { GO_GLAZER_BRIDGE,          DATA_GLAZER,           DOOR_TYPE_SPAWN_HOLE,    BOUNDARY_NONE },
    { 0,                         0,                     DOOR_TYPE_ROOM,          BOUNDARY_NONE }
};

class instance_vault_of_the_wardens : public InstanceMapScript
{
    public:
        instance_vault_of_the_wardens() : InstanceMapScript("instance_vault_of_the_wardens", 1493) { }

        struct instance_vault_of_the_wardens_InstanceMapScript : public InstanceScript
        {
            instance_vault_of_the_wardens_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            uint64 m_TirathonSaltheril;
            uint64 m_MotherDoorGuid;
            uint64 m_UpdraftGuid;
            uint64 m_BladeDancerIllianna;
            uint64 m_BladeDancerDoor;
            uint64 m_DreadlordMendacius;
            uint64 m_DreadlordDoor;
            uint64 m_AshGolmGuid;
            uint64 m_GlazerGuid;
            uint64 m_CordanaElevatorDoor;
            uint64 m_CordanaDoor;
            uint64 m_CordanaFelsongGuid;

            std::map<uint32, uint64> m_TormCageContainer;
            std::list<uint64> m_AshSysContainer;
            std::set<uint64> m_GlazerOwlStatues;

            uint32 m_UnderMapCheckTimer;

            bool m_CordanaEngagedOnce;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);

                m_TirathonSaltheril = 0;
                m_MotherDoorGuid = 0;
                m_UpdraftGuid = 0;
                m_BladeDancerIllianna = 0;
                m_BladeDancerDoor = 0;
                m_DreadlordMendacius = 0;
                m_DreadlordDoor = 0;
                m_AshGolmGuid = 0;
                m_GlazerGuid = 0;
                m_CordanaElevatorDoor = 0;
                m_CordanaDoor = 0;
                m_CordanaFelsongGuid = 0;

                m_UnderMapCheckTimer = 0;

                m_CordanaEngagedOnce = false;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                m_UnderMapCheckTimer = 500;
            }

            bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
            {
                if (!p_Target)
                    return false;

                if (Creature const* l_Creature = p_Target->ToCreature())
                {
                    if (l_Creature->IsDungeonBoss())
                        return false;
                }

                switch (p_Target->GetEntry())
                {
                    case eCreatures::NPC_CORDANA_FELSONG:
                    case eCreatures::NPC_BLADE_DANCER_ILLIANNA:
                    case eCreatures::NPC_DREADLORD_MENDACIUS:
                    case eCreatures::NPC_GLAVIANA_SOULRENDER:
                        return false;

                    default: return true;
                }

                return true;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case NPC_COUNTERMEASURES:
                    {
                        m_AshSysContainer.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case NPC_TIRATHON_SALTHERIL:
                    {
                        m_TirathonSaltheril = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_BLADE_DANCER_ILLIANNA:
                    {
                        m_BladeDancerIllianna = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_DREADLORD_MENDACIUS:
                    {
                        p_Creature->SetVisible(false);
                        p_Creature->SetReactState(REACT_PASSIVE);
                        m_DreadlordMendacius = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_FOUL_MOTHER:
                    {
                        uint64 l_Guid = p_Creature->GetGUID();
                        AddTimedDelayedOperation(1000, [this, l_Guid]() -> void
                        {
                            if (Creature* l_Creature = instance->GetCreature(l_Guid))
                            {
                                if (!l_Creature->isAlive())
                                {
                                    if (GameObject* l_Door = instance->GetGameObject(m_MotherDoorGuid))
                                        l_Door->SetGoState(GO_STATE_ACTIVE);
                                }
                            }
                        });
                        break;
                    }
                    case NPC_UPDRAFT:
                    {
                        m_UpdraftGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_ASH_GOLM:
                    {
                        m_AshGolmGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_GLAZER:
                    {
                        m_GlazerGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_CORDANA_FELSONG:
                    {
                        m_CordanaFelsongGuid = p_Creature->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_SALTHERIL_DOOR_1:
                    case GO_SALTHERIL_DOOR_2:
                    case GO_SALTHERIL_DOOR_3:
                    case GO_SALTHERIL_DOOR_4:
                    case GO_SALTHERIL_DOOR_5:
                    case GO_ASHGOLM_PLATFORM:
                    case GO_ASHGOLM_BRIDGE:
                    case GO_GLAZER_BRIDGE:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case GO_MOTHER_DOOR:
                    {
                        m_MotherDoorGuid = p_GameObject->GetGUID();
                        p_GameObject->SetGoState(GO_STATE_READY);
                        break;
                    }
                    case GO_TORMENTORUM_DOOR_1:
                    {
                        m_CordanaElevatorDoor = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_TORMENTORUM_DOOR_2:
                    {
                        m_BladeDancerDoor = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_TORMENTORUM_DOOR_3:
                    {
                        m_DreadlordDoor = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_CORDANA_DOOR:
                    {
                        m_CordanaDoor = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }

                if (p_GameObject->GetEntry() >= GO_CAGE_1 && p_GameObject->GetEntry() <= GO_CAGE_12)
                    m_TormCageContainer[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                else if (p_GameObject->GetEntry() >= GO_OWL_STATUE_MIN && p_GameObject->GetEntry() <= GO_OWL_STATUE_MAX)
                    m_GlazerOwlStatues.insert(p_GameObject->GetGUID());
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_SALTHERIL_DOOR_1:
                    case GO_SALTHERIL_DOOR_2:
                    case GO_SALTHERIL_DOOR_3:
                    case GO_SALTHERIL_DOOR_4:
                    case GO_SALTHERIL_DOOR_5:
                    case GO_ASHGOLM_PLATFORM:
                    case GO_ASHGOLM_BRIDGE:
                    case GO_GLAZER_BRIDGE:
                    {
                        AddDoor(p_GameObject, false);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* p_Unit) override
            {
                switch (p_Unit->GetEntry())
                {
                    case NPC_FOUL_MOTHER:
                    {
                        if (GameObject* l_Door = instance->GetGameObject(m_MotherDoorGuid))
                            l_Door->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case NPC_DREADLORD_MENDACIUS:
                    {
                        if (GameObject* l_Door = instance->GetGameObject(m_DreadlordDoor))
                            l_Door->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case NPC_BLADE_DANCER_ILLIANNA:
                    {
                        if (GameObject* l_Door = instance->GetGameObject(m_BladeDancerDoor))
                            l_Door->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case NPC_SPIRIT_OF_VENGEANCE:
                    {
                        Position l_CheckPos = { 4226.097f, -297.0816f, -281.1114f, 6.277769f };

                        if (p_Unit->ToCreature()->GetHomePosition().IsNearPosition(&l_CheckPos, 0.5f))
                        {
                            if (GameObject* l_Door = instance->GetGameObject(m_CordanaDoor))
                                l_Door->SetGoState(GO_STATE_ACTIVE);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 p_Type, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Type, p_State))
                    return false;

                switch (p_Type)
                {
                    case DATA_TORMENTORUM:
                    {
                        switch (p_State)
                        {
                            case NOT_STARTED:
                            {
                                for (auto l_Iter : m_TormCageContainer)
                                {
                                    if (GameObject* l_Cage = instance->GetGameObject(l_Iter.second))
                                        l_Cage->SetGoState(GO_STATE_READY);
                                }

                                break;
                            }
                            case DONE:
                            {
                                if (Creature* l_Dreadlord = instance->GetCreature(m_DreadlordMendacius))
                                {
                                    l_Dreadlord->SetVisible(true);
                                    l_Dreadlord->CastSpell(l_Dreadlord, eVotWSpells::SpellFelDissolveIn, true);
                                    l_Dreadlord->SetReactState(REACT_AGGRESSIVE);
                                }

                                if (Creature* l_Illianna = instance->GetCreature(m_BladeDancerIllianna))
                                {
                                    if (l_Illianna->IsAIEnabled)
                                        l_Illianna->AI()->DoAction(0);
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case DATA_GLAZER:
                    {
                        switch (p_State)
                        {
                            case NOT_STARTED:
                            case FAIL:
                            {
                                for (uint64 l_Guid : m_GlazerOwlStatues)
                                {
                                    if (GameObject* l_Statue = instance->GetGameObject(l_Guid))
                                        l_Statue->SetGoState(GO_STATE_READY);
                                }

                                break;
                            }
                            case DONE:
                            {
                                if (GameObject* l_Door = instance->GetGameObject(m_BladeDancerDoor))
                                    l_Door->SetGoState(GO_STATE_ACTIVE);

                                if (GetBossState(DATA_ASHGOLM) == DONE && GetBossState(DATA_TORMENTORUM) == DONE)
                                {
                                    if (GameObject* l_Door = instance->GetGameObject(m_CordanaElevatorDoor))
                                        l_Door->SetGoState(GO_STATE_ACTIVE);

                                    instance->SummonCreature(NPC_GRIMHORN_THE_ENSLAVER, { 4450.814f, -378.515f, 126.106f, 4.71737f });
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case DATA_ASHGOLM:
                    {
                        switch (p_State)
                        {
                            case DONE:
                            {
                                if (GameObject* l_Door = instance->GetGameObject(m_DreadlordDoor))
                                    l_Door->SetGoState(GO_STATE_ACTIVE);

                                if (GetBossState(DATA_GLAZER) == DONE && GetBossState(DATA_TORMENTORUM) == DONE)
                                {
                                    if (GameObject* l_Door = instance->GetGameObject(m_CordanaElevatorDoor))
                                        l_Door->SetGoState(GO_STATE_ACTIVE);

                                    instance->SummonCreature(NPC_GRIMHORN_THE_ENSLAVER, { 4450.814f, 378.515f, 126.106f, 4.71737f });
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case DATA_CORDANA:
                    {
                        if (p_State == EncounterState::IN_PROGRESS)
                            m_CordanaEngagedOnce = true;

                        break;
                    }
                    default:
                        break;
                }

                return true;
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                switch (p_Type)
                {
                    case DATA_ASHGOLM_SYSTEM:
                    {
                        switch (p_Data)
                        {
                            case DONE:
                            {
                                for (uint64 l_Guid : m_AshSysContainer)
                                {
                                    if (Creature* l_System = instance->GetCreature(l_Guid))
                                        l_System->SetVisible(false);
                                }

                                break;
                            }
                            case SPECIAL:
                            {
                                for (uint64 l_Guid : m_AshSysContainer)
                                {
                                    if (Creature* l_System = instance->GetCreature(l_Guid))
                                        l_System->SetVisible(true);
                                }

                                break;
                            }
                            case IN_PROGRESS:
                            {
                                for (uint64 l_Guid : m_AshSysContainer)
                                {
                                    if (Creature* l_System = instance->GetCreature(l_Guid))
                                    {
                                        if (l_System->IsAIEnabled)
                                            l_System->AI()->DoAction(0);
                                    }
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case DATA_GLAZER_SYSTEM:
                    {
                        /// Only for reset tiles
                        if (p_Data > 0)
                            break;

                        for (uint64 l_Guid : m_GlazerOwlStatues)
                        {
                            if (GameObject* l_Statue = instance->GetGameObject(l_Guid))
                                l_Statue->SetGoState(GO_STATE_READY);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case NPC_TIRATHON_SALTHERIL:
                        return m_TirathonSaltheril;
                    case NPC_BLADE_DANCER_ILLIANNA:
                        return m_BladeDancerIllianna;
                    case NPC_DREADLORD_MENDACIUS:
                        return m_DreadlordMendacius;
                    case NPC_ASH_GOLM:
                        return m_AshGolmGuid;
                    case NPC_GLAZER:
                        return m_GlazerGuid;
                    case NPC_CORDANA_FELSONG:
                        return m_CordanaFelsongGuid;
                    default:
                        break;
                }

                return 0;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (m_UnderMapCheckTimer)
                {
                    if (m_UnderMapCheckTimer <= p_Diff)
                    {
                        Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (!l_Player->isAlive())
                                    continue;

                                if (l_Player->IsNearPosition(&g_UnderTormentorumPos, 50.0f) && l_Player->m_positionZ <= 100.0f)
                                    l_Player->Kill(l_Player);
                            }
                        }

                        m_UnderMapCheckTimer = 500;
                    }
                    else
                        m_UnderMapCheckTimer -= p_Diff;
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (m_CordanaEngagedOnce)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocBoss4EngagedOnce);
                else if (GetBossState(DATA_ASHGOLM) == DONE && GetBossState(DATA_GLAZER) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocBoss2And3Defeated);
                else if (GetBossState(DATA_ASHGOLM) == DONE || GetBossState(DATA_GLAZER) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocBoss2Xor3Defeated);
                else if (GetBossState(DATA_SALTHERIL) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocBoss1Defeated);
                else
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocEntrance);

                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_vault_of_the_wardens_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_vault_of_the_wardens()
{
    new instance_vault_of_the_wardens();
}
#endif
