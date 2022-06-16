////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "court_of_stars.hpp"

DoorData const g_DdoorData[] =
{
    { GO_MELANDRUS_DOOR_1,     DATA_TALIXAE,             DOOR_TYPE_PASSAGE,     BOUNDARY_NONE },
    { 0,                       0,                        DOOR_TYPE_ROOM,        BOUNDARY_NONE }
};

class instance_court_of_stars : public InstanceMapScript
{
    public:
        instance_court_of_stars() : InstanceMapScript("instance_court_of_stars", 1571) { }

        struct instance_court_of_stars_InstanceMapScript : public InstanceScript
        {
            instance_court_of_stars_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                m_Map = p_Map;
            }

            Map* m_Map;

            uint64 m_BossGerdoGuid = 0;
            uint64 m_BossTalixaeGuid = 0;
            uint64 m_BossMelandrusGuid = 0;

            /// Spy event
            uint64 m_GuidEvent = 0;

            uint64 m_DemonDoorGuid = 0;

            uint32 m_HintCounter = 0;
            uint32 m_CheckPosDiff = 0;
            int32 m_CheckPosMod = 0;

            /// Misc
            uint64 m_SignalLanternGuid = 0;
            uint64 m_NpcCheckPointGuid = 0;
            uint64 m_CaptainDoorGuid = 0;
            uint64 m_MelandrusDoor2Guid = 0;

            std::list<uint64> m_BeaconsGuids;
            std::list<uint64> m_SpiesGUIDs;
            std::vector<uint32> m_Hints = { DATA_FIRST_HINT, DATA_SECOND_HINT, DATA_THIRD_HINT, DATA_FOURTH_HINT, DATA_FIFTH_HINT, DATA_SIXTH_HINT };
            std::map<uint32, uint32> m_RumormongerData; ///< GuidLow of the creature, Hint index

            /// Random Items event
            std::list<uint64> m_ItemsGUIDs;
            bool m_GuardiansStunned = false;
            uint32 m_CountAreaSecuted = 0;
            uint32 m_NobleCount = 0;
            uint32 m_NobleComplete = 0;

            void Initialize() override
            {
                LoadDoorData(g_DdoorData);

                m_CheckPosMod = 0;
                m_CheckPosDiff = 1 * TimeConstants::IN_MILLISECONDS;

                /// Handle Random items spawns
                m_Map->AddTask([=]()-> void
                {
                    std::vector<uint8> l_RedMarkIndexes;
                    std::vector<uint8> m_RedMarkIndexUsed;
                    uint32 m_RedMarksCount = urand(1, 2);

                    std::vector<uint8> l_BlueMarkIndexes;
                    std::vector<uint8> m_BlueMarkIndexUsed;
                    uint32 m_BlueMarksCount = urand(2, 3);

                    /// Generations Red Mark
                    for (uint8 l_Index = 0; l_Index < g_EventNpcRedMarks.size(); l_Index++)
                        l_RedMarkIndexes.push_back(l_Index);

                    /// Generations Blue Mark
                    for (uint8 l_Index = 0; l_Index < g_EventNpcBlueMarks.size(); l_Index++)
                        l_BlueMarkIndexes.push_back(l_Index);

                    while (m_RedMarkIndexUsed.size() < (m_RedMarksCount + 1))
                    {
                        std::shuffle(l_RedMarkIndexes.begin(), l_RedMarkIndexes.end(), g_RandomGenerator);
                        uint32 l_ChosenIndex = l_RedMarkIndexes.back();
                        m_RedMarkIndexUsed.push_back(l_ChosenIndex);

                        if (Creature* l_Creature = instance->SummonCreature(g_EventNpcRedMarks[l_ChosenIndex], g_EventRedMarksPos[l_ChosenIndex]))
                            m_ItemsGUIDs.push_back(l_Creature->GetGUID());

                        l_RedMarkIndexes.pop_back();
                    }

                    while (m_BlueMarkIndexUsed.size() < (m_BlueMarksCount + 1))
                    {
                        std::shuffle(l_BlueMarkIndexes.begin(), l_BlueMarkIndexes.end(), g_RandomGenerator);
                        uint32 l_ChosenIndex = l_BlueMarkIndexes.back();
                        m_BlueMarkIndexUsed.push_back(l_ChosenIndex);

                        if (Creature* l_Creature = instance->SummonCreature(g_EventNpcBlueMarks[l_ChosenIndex], g_EventBlueMarksPos[l_ChosenIndex]))
                            m_ItemsGUIDs.push_back(l_Creature->GetGUID());

                        l_BlueMarkIndexes.pop_back();
                    }

                    /*///< Only for test
                    for (uint8 l_Index = 0; l_Index < g_EventNpcRedMarks.size(); l_Index++)
                        instance->SummonCreature(g_EventNpcRedMarks[l_Index], g_EventRedMarksPos[l_Index]);

                    for (uint8 l_Index = 0; l_Index < g_EventNpcBlueMarks.size(); l_Index++)
                        instance->SummonCreature(g_EventNpcBlueMarks[l_Index], g_EventBlueMarksPos[l_Index]);*/
                });

                m_CountAreaSecuted = 0;
                m_NobleCount = 0;
                m_NobleComplete = 0;
            }

            bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
            {
                switch (p_Target->GetEntry())
                {
                    case eCreatures::NPC_PATROL_CAPTAIN_GERDO:
                    case eCreatures::NPC_TALIXAE:
                    case eCreatures::NPC_ADVISOR_MELANDRUS:
                    {
                        return false;
                    }

                    case eCreatures::NPC_VIGILANT_WATCH:
                    {
                        return false;
                    }

                    default: 
                        return true;
                }

                return true;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case NPC_PATROL_CAPTAIN_GERDO:
                    {
                        m_BossGerdoGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_TALIXAE:
                    {
                        m_BossTalixaeGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_ADVISOR_MELANDRUS:
                    {
                        m_BossMelandrusGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_CHECKPOINT_BEFORE_2ND_BOSS:
                    {
                        m_NpcCheckPointGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_LYLETH_LUNASTRE:
                    {
                        p_Creature->SetDisplayId(11686);
                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    case NPC_GERDO_ARCANE_BEACON:
                    {
                        m_BeaconsGuids.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case NPC_SUSPICIOUS_NOBLE:
                    {
                        if (!m_GuidEvent && m_SpiesGUIDs.empty())
                        {
                            uint32 l_ChosenDisplayID = g_SpiesDisplayIDs[urand(0, g_SpiesDisplayIDs.size() - 1)];
                            p_Creature->SetDisplayId(l_ChosenDisplayID);
                            m_GuidEvent = p_Creature->GetGUID();

                            break;
                        }

                        uint32 l_AlternativeDisplayID = 0;

                        if (Creature* l_Main = instance->GetCreature(m_GuidEvent))
                        {
                            do
                            {
                                l_AlternativeDisplayID = g_SpiesDisplayIDs[urand(0, g_SpiesDisplayIDs.size() - 1)];
                            } while (l_AlternativeDisplayID == l_Main->GetDisplayId());
                        }

                        p_Creature->SetDisplayId(l_AlternativeDisplayID);
                        m_SpiesGUIDs.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case NPC_CHATTY_RUMORMONGER:
                    {
                        p_Creature->AddAura(133297, p_Creature); ///< Chat Bubble

                        /// Only 5 creatures with this entry are spawned, so 1 hint won't be used by any of them.
                        uint32 l_ChosenHint = m_Hints[urand(0, m_Hints.size() - 1)];

                        m_RumormongerData[p_Creature->GetGUIDLow()] = l_ChosenHint;
                        m_Hints.erase(std::remove(m_Hints.begin(), m_Hints.end(), l_ChosenHint));
                        break;
                    }
                    case NPC_SIGNAL_LANTERN:
                    {
                        m_SignalLanternGuid = p_Creature->GetGUID();
                        break;
                    }
                    case 108419: ///< Automated Sweepwr
                    {
                        p_Creature->SetWalk(true);
                        p_Creature->GetMotionMaster()->Clear(false);
                        p_Creature->GetMotionMaster()->MoveRandom(3.0f);
                        break;
                    }
                    case 105410: ///< Suramar Musician
                    {
                        p_Creature->AddAura(208900, p_Creature);
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
                    case GO_MELANDRUS_DOOR_1:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case GO_GERDO_DOOR:
                    {
                        p_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);

                        p_GameObject->ResetLootMode();
                        p_GameObject->ResetDoorOrButton();

                        m_DemonDoorGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_CAPTAIN_DOOR:
                    {
                        p_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);

                        p_GameObject->SetLootState(LootState::GO_READY);
                        p_GameObject->UseDoorOrButton(1 * TimeConstants::IN_MILLISECONDS);

                        m_CaptainDoorGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_MELANDRUS_DOOR_2:
                    {
                        AddDoor(p_GameObject, true);
                        p_GameObject->SetGoState(GO_STATE_READY);
                        m_MelandrusDoor2Guid = p_GameObject->GetGUID();

                        break;
                    }
                    case GO_MALRODIS_DOOR: ///< Melrodi Door
                    {
                        p_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);
                        bool l_Ok = false;
                        for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                        {
                            if (l_Ok)
                                continue;

                            if (Player* l_Player = itr->getSource())
                            {
                                if (l_Player->HasQuest(42784)) ///< The Deceitful Student
                                {
                                    p_GameObject->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);
                                    break;
                                }
                            }
                        }

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
                    case GO_MELANDRUS_DOOR_1:
                    case GO_MELANDRUS_DOOR_2:
                    {
                        AddDoor(p_GameObject, false);
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

                return true;
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                switch (p_Type)
                {
                    case DATA_BEACON_ACTIVATE:
                    {
                        if (Creature* l_Gerdo = instance->GetCreature(m_BossGerdoGuid))
                        {
                            for (uint64 l_Iter : m_BeaconsGuids)
                            {
                                if (Creature* l_Beacon = instance->GetCreature(l_Iter))
                                {
                                    if (l_Beacon && !l_Beacon->HasAura(SPELL_DISABLED))
                                        l_Beacon->CastSpell(l_Beacon, SPELL_CALL_VIGILANT_NIGHTWATCH, true, 0, 0, l_Gerdo->GetGUID());
                                }
                            }
                        }

                        break;
                    }
                    case NPC_GUARDIAN_CONSTRUCT:
                    {
                        m_GuardiansStunned = true;
                        break;
                    }
                    case DATA_AREA_SECURED:
                    {
                        ++m_CountAreaSecuted;
                        if (Creature* l_Gerdo = instance->GetCreature(m_BossGerdoGuid))
                        {
                            if (Aura* l_AreaSecuredAura = l_Gerdo->GetAura(227147))
                            {
                                if (l_AreaSecuredAura->GetStackAmount() == 1)
                                    l_AreaSecuredAura->Remove();
                                else
                                    l_AreaSecuredAura->SetStackAmount(l_AreaSecuredAura->GetStackAmount() - 1);
                            }
                        }
                        break;
                    }
                    case DATA_NOBLE_COUNT:
                    {
                        ++m_NobleCount;
                        break;
                    }
                    case DATA_NOBLE_COMPLETE:
                    {
                        ++m_NobleComplete;
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == DATA_NOBLE_COUNT)
                    return m_NobleCount;
                if (p_Type == DATA_NOBLE_COMPLETE)
                    return m_NobleComplete;

                if (p_Type == DATA_AREA_SECURED)
                    return m_CountAreaSecuted;
                /// Here, p_Type is supposed to be the GUID_LOW of Rumormongers
                for (auto l_Itr : m_RumormongerData)
                {
                    if (l_Itr.first == p_Type)
                        return l_Itr.second;
                }

                /// Only in this case
                if (p_Type == NPC_GUARDIAN_CONSTRUCT)
                    return m_GuardiansStunned;

                return 0;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_CAPTAIN_GERDO:
                        return m_BossGerdoGuid;
                    case DATA_TALIXAE:
                        return m_BossTalixaeGuid;
                    case DATA_MELANDRUS:
                        return m_BossMelandrusGuid;
                    case NPC_SUSPICIOUS_NOBLE:
                        return m_GuidEvent;
                    case NPC_SIGNAL_LANTERN:
                       return m_SignalLanternGuid;
                    case GO_GERDO_DOOR:
                        return m_DemonDoorGuid;
                    case GO_CAPTAIN_DOOR:
                        return m_CaptainDoorGuid;
                    case GO_MELANDRUS_DOOR_2:
                        return m_MelandrusDoor2Guid;
                }

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                Position l_Pos = Position();

                if (Creature* l_Guillermo = sObjectAccessor->FindCreature(GetData64(DATA_CAPTAIN_GERDO)))
                {
                    if (Creature* l_Tal = sObjectAccessor->FindCreature(GetData64(DATA_TALIXAE)))
                    {
                        if (!l_Guillermo->isDead())
                        {
                            if (m_CheckPosMod == 0)
                                l_Pos = { 961.7474f, 3463.6980f, 2.1145f, 5.436f };
                            else
                                l_Pos = { 1055.90f, 3454.62f, 22.f, 0.94f};
                        }
                        else if (l_Guillermo->isDead() && !l_Tal->isDead())
                            l_Pos = { 1296.3271f, 3471.5125f, 30.2650f, 4.485f };
                        else if (l_Tal->isDead())
                            l_Pos = { 1111.9839f, 3470.3528f, 19.8383f, 4.555f };
                    }
                }
                else
                {
                    if (m_CheckPosMod == 0)
                        l_Pos = { 961.7474f, 3463.6980f, 2.1145f, 5.436f };
                    else
                        l_Pos = { 1055.90f, 3454.62f, 22.f, 0.94f};
                }

                p_X = l_Pos.m_positionX;
                p_Y = l_Pos.m_positionY;
                p_Z = l_Pos.m_positionZ;
                p_O = l_Pos.m_orientation;
                p_MapID = 1571;
            }

            void Update(uint32 p_Diff) override
            {
                UpdateOperations(p_Diff);

                /// Challenge
                InstanceScript::Update(p_Diff);

                if (m_CheckPosDiff <= p_Diff)
                {
                    if (m_CheckPosMod > 2)
                        return;

                    Position l_Pos = Position();

                    bool l_Ok = false;

                    uint32 l_ConversationArra[2] =
                    {
                        2294, ///< It seems the Duskwatch... 2. Along he way, you'll want to ..
                        2291  ///< Watch out up here. I've... 2. Explore the area and find a..
                    };

                    switch (m_CheckPosMod)
                    {
                        case 0:
                        {
                            l_Pos = { 1060.160f, 3460.134f, 21.776f, 1.00747f };
                            break;
                        }
                        case 1: ///< 2291
                        {
                            l_Pos = { 1235.48f, 3435.18f, 55.41f, 2.839918f };
                            break;
                        }
                        default:
                            break;
                    }

                    uint64 l_Guid = 0;

                    for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                    {
                        if (l_Ok)
                            continue;

                        if (Player* l_Player = itr->getSource())
                        {
                            if (l_Player->GetDistance(l_Pos) <= 20.0f)
                            {
                                l_Ok = true;
                                l_Guid = l_Player->GetGUID();
                            }
                        }
                    }

                    if (l_Ok)
                    {
                        if (m_CheckPosMod == 1)
                        {
                            if (GameObject* l_Door = instance->GetGameObject(GetData64(GO_GERDO_DOOR)))
                                l_Door->UseDoorOrButton();
                        }

                        if (Player* l_Plr = sObjectAccessor->FindPlayer(l_Guid))
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), l_ConversationArra[m_CheckPosMod], l_Plr, nullptr, *l_Plr))
                                delete l_Conversation;
                        }

                        m_CheckPosMod++;
                    }

                    m_CheckPosDiff = 500;
                }
                else
                    m_CheckPosDiff -= p_Diff;
            }

        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_court_of_stars_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_court_of_stars()
{
    new instance_court_of_stars();
}
#endif
