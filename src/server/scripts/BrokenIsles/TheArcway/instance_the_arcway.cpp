////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"

DoorData const g_DoorData_1[] =
{
    { GO_DOOR_8,                 MAX_ENCOUNTER,        DOOR_TYPE_RANDOM,    BOUNDARY_NONE },
    { GO_DOOR_9,                 DATA_CORSTILAX,       DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_D,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_G,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_CORSTILAX_DOOR_2,       DATA_CORSTILAX,       DOOR_TYPE_ROOM,      BOUNDARY_NONE },
    { GO_DOOR_3,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_4,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_B,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_K,                 DATA_CORSTILAX,       DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_A,                 MAX_ENCOUNTER,        DOOR_TYPE_RANDOM,    BOUNDARY_NONE },
    { 0,                         0,                    DOOR_TYPE_PASSAGE,   BOUNDARY_NONE }
};

/// Blue path
DoorData const g_DoorData_2[] =
{
    { GO_DOOR_8,                 MAX_ENCOUNTER,        DOOR_TYPE_RANDOM,    BOUNDARY_NONE },
    { GO_CORSTILAX_DOOR_2,       DATA_CORSTILAX,       DOOR_TYPE_ROOM,      BOUNDARY_NONE },
    { GO_DOOR_9,                 DATA_CORSTILAX,       DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_D,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_G,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_4,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_B,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_K,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_A,                 DATA_XAKAL,           DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { 0,                         0,                    DOOR_TYPE_PASSAGE,   BOUNDARY_NONE }
};

/// Red path
DoorData const g_DoorData_3[] =
{
    { GO_DOOR_8,                 DATA_IVANYR,          DOOR_TYPE_ROOM,      BOUNDARY_NONE },
    { GO_DOOR_9,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_D,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_G,                 DATA_IVANYR,          DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_CORSTILAX_DOOR_2,       DATA_CORSTILAX,       DOOR_TYPE_ROOM,      BOUNDARY_NONE },
    { GO_DOOR_3,                 DATA_CORSTILAX,       DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_4,                 DATA_CORSTILAX,       DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_B,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_K,                 DATA_NALTIRA,         DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { GO_DOOR_A,                 DATA_XAKAL,           DOOR_TYPE_PASSAGE,   BOUNDARY_NONE },
    { 0,                         0,                    DOOR_TYPE_PASSAGE,   BOUNDARY_NONE }
};

class instance_the_arcway : public InstanceMapScript
{
    public:
        instance_the_arcway() : InstanceMapScript("instance_the_arcway", 1516) { }

        struct instance_the_arcway_InstanceMapScript : public InstanceScript
        {
            instance_the_arcway_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            bool m_InitEnterState;
            bool m_TwoSay;
            bool m_VandrosSpawned;

            uint8 m_RandTeleport;
            uint8 m_OpenDoors;
            uint32 m_TwoSayTimer;
            uint32 m_CheckVandrosTimer;

            /// Boss GUIDs
            uint64 m_CorstilaxGUID;
            uint64 m_XakalGUID;
            uint64 m_IvanyrGUID;
            uint64 m_NaltirGUID;
            uint64 m_VandrosGUID;

            std::set<uint64> m_DoorGUIDs;

            void Initialize() override
            {
                m_InitEnterState = false;
                m_TwoSay = false;
                m_VandrosSpawned = false;
                m_TwoSayTimer = 0;
                m_CheckVandrosTimer = 5000;
                if (!m_InitEnterState && (instance->IsChallengeMode() || instance->IsMythic()))
                {
                    m_OpenDoors = (uint8)urand(1, 2);
                    if (m_OpenDoors == 1)
                        LoadDoorData(g_DoorData_1);
                    else
                        LoadDoorData(g_DoorData_3);
                }
                else if (!m_InitEnterState && (instance->IsNormal() || instance->IsHeroic()))
                {
                    m_OpenDoors = (uint8)urand(1, 2);
                    if (m_OpenDoors == 1)
                        LoadDoorData(g_DoorData_1);
                    else
                        LoadDoorData(g_DoorData_3);
                }
                else if (!m_InitEnterState && (instance->IsNormal() || instance->IsHeroic()))
                {
                    m_OpenDoors = (uint8)urand(1, 3);
                    switch (m_OpenDoors)
                    {
                        case 1:
                            LoadDoorData(g_DoorData_1);
                            break;
                        case 2:
                            LoadDoorData(g_DoorData_2);
                            break;
                        case 3:
                            LoadDoorData(g_DoorData_3);
                            break;
                        default:
                            LoadDoorData(g_DoorData_1);
                            break;
                    }
                }
            }

            uint8 GetOpenDoors() { return m_OpenDoors; }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_DOOR_A:
                    {
                        AddDoor(p_GameObject, true);
                        if (m_OpenDoors == 1)
                            HandleGameObject(0, true, p_GameObject);
                        m_DoorGUIDs.insert(p_GameObject->GetGUID());
                        break;
                    }
                    case GO_DOOR_8:
                    {
                        AddDoor(p_GameObject, true);
                        if (m_OpenDoors == 3 && !m_InitEnterState)
                            HandleGameObject(0, true, p_GameObject);
                        m_DoorGUIDs.insert(p_GameObject->GetGUID());
                        break;
                    }
                    case GO_DOOR_3:
                    {
                        if (m_OpenDoors == 2)
                        {
                            HandleGameObject(0, true, p_GameObject);
                            break;
                        }

                        AddDoor(p_GameObject, true);
                        m_DoorGUIDs.insert(p_GameObject->GetGUID());
                        break;
                    }
                    case GO_CORSTILAX_DOOR_1:
                    case GO_CORSTILAX_DOOR_2:
                    case GO_DOOR_D:
                    case GO_DOOR_G:
                    case GO_DOOR_9:
                    case GO_DOOR_B:
                    case GO_DOOR_4:
                    case GO_DOOR_K:
                    {
                        AddDoor(p_GameObject, true);
                        m_DoorGUIDs.insert(p_GameObject->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_CORSTILAX_DOOR_1:
                    case GO_CORSTILAX_DOOR_2:
                    case GO_DOOR_A:
                    case GO_DOOR_D:
                    case GO_DOOR_G:
                    case GO_DOOR_9:
                    case GO_DOOR_3:
                    case GO_DOOR_4:
                    case GO_DOOR_B:
                    case GO_DOOR_K:
                    {
                        m_DoorGUIDs.erase(p_GameObject->GetGUID());
                        AddDoor(p_GameObject, false);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::NPC_CORSTILAX:
                        m_CorstilaxGUID = p_Creature->GetGUID();
                        break;
                    case eCreatures::NPC_GENERAL_XAKAL:
                        m_XakalGUID = p_Creature->GetGUID();
                        break;
                    case eCreatures::NPC_NALTIRA:
                        m_NaltirGUID = p_Creature->GetGUID();
                        break;
                    case eCreatures::NPC_IVANYR:
                        m_IvanyrGUID = p_Creature->GetGUID();
                        break;
                    case eCreatures::NPC_ADVISOR_VANDROS:
                        m_VandrosGUID = p_Creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                switch (p_Type)
                {
                    case DATA_RAND_TELEPORT:
                    {
                        m_RandTeleport = p_Data;
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_RAND_TELEPORT:
                        return m_RandTeleport;
                    default:
                        break;
                }

                return 0;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_IVANYR:
                        return m_IvanyrGUID;
                    case DATA_VANDROS:
                        return m_VandrosGUID;
                    default:
                        break;
                }

                return 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_InitEnterState)
                {
                    uint64 l_Guid = p_Player->GetGUID();
                    AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                    {
                        if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                        {
                            if (l_Player->GetMapId() != instance->GetId())
                                return;

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3112, l_Player, nullptr, *l_Player))
                                delete l_Conversation;
                        }
                    });
                    m_InitEnterState = true;
                }
            }

            void OpenAllDoors()
            {
                for (uint64 l_DoorGUID : m_DoorGUIDs)
                {
                    GameObject* l_Door = sObjectAccessor->FindGameObject(l_DoorGUID);
                    if (!l_Door)
                        return;

                    UpdateDoorState(l_Door);
                }
            }

            void ActivateVandros()
            {
                if (!instance)
                    return;

                Creature* l_Vandros = instance->GetCreature(m_VandrosGUID);
                if (!l_Vandros)
                    return;

                if (l_Vandros->IsAIEnabled)
                {
                    l_Vandros->SetPhaseMask(3, true);
                    l_Vandros->AI()->DoAction(2);
                    l_Vandros->AI()->ZoneTalk(0);
                }
            }

            bool CheckRequiredBosses(uint32 p_BossId, Player const* p_Player) const override
            {
                if (p_BossId == DATA_VANDROS)
                {
                    for (uint8 l_I = 0; l_I < DATA_VANDROS; l_I++)
                    {
                        if (GetBossState(l_I) != DONE)
                            return false;
                    }

                    return true;
                }

                return InstanceScript::CheckRequiredBosses(p_BossId, p_Player);
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (GetBossState(eData::DATA_XAKAL) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::XAKAL_CHECKPOINT);
                else if (GetBossState(eData::DATA_NALTIRA) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::NALTIRA_CHECKPOINT);
                else if (GetBossState(eData::DATA_CORSTILAX) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::CORSTILAX_CHECKPOINT);
                else if (GetBossState(eData::DATA_IVANYR) == DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::IVANYR_CHECKPOINT);

                if (l_Loc == nullptr)
                    return;

                p_X = l_Loc->x;
                p_Y = l_Loc->y;
                p_Z = l_Loc->z;
                p_O = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            void Update(uint32 p_Diff) override
            {
                /// Challenge
                InstanceScript::Update(p_Diff);

                if (m_CheckVandrosTimer <= p_Diff)
                {
                    if (CountDefeatedEncounters() >= 4 && !m_VandrosSpawned)
                    {
                        m_VandrosSpawned = true;
                        OpenAllDoors();
                        ActivateVandros();
                    }
                    else
                        m_CheckVandrosTimer = 5000;
                }
                else
                    m_CheckVandrosTimer -= p_Diff;
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
                    case eCreatures::NPC_VICIOUS_MANAFANG:
                    case eCreatures::NPC_VICIOUS_MANAFANG_2:
                    case eCreatures::NPC_ENCHANTED_BROODLING:
                    case eCreatures::NPC_TIMELESS_WRATH:
                        return false;

                    default:
                        return true;
                }

                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_arcway_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_the_arcway()
{
    new instance_the_arcway();
}
#endif
