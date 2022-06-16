////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "InstanceScript.h"
#include "instance_argus_arcatraz.h"

class instance_argus_arcatraz : public InstanceMapScript
{
public:
    instance_argus_arcatraz() : InstanceMapScript("instance_argus_arcatraz", 1746) { }

    struct instance_argus_arcatraz_InstanceMapScript : public InstanceScript
    {
        instance_argus_arcatraz_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

        uint8 m_InstanceState = eInstanceStates::State_SearchTheArcatraz;
        uint8 m_ClosedPortals = 0;
        std::vector<ObjectGuid> m_LegionPortalsGuids;
        std::vector<ObjectGuid> m_LahzaruunMinionGuids;
        ObjectGuid m_StasisDoorGuid;
        ObjectGuid m_StasisDoorBunnyGuid;
        ObjectGuid m_NaaruPrisonGuid;
        ObjectGuid m_HighInquisitorRaalgarGuid;
        ObjectGuid m_LahzaruunGuid;

        void OnPlayerEnter(Player* p_Player) override
        {
            SetInstanceState(p_Player);

            switch (m_InstanceState)
            {
            case eInstanceStates::State_SearchTheArcatraz:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_IntroducingConversation, true);
                break;
            }
            case eInstanceStates::State_EscapeFromTheArcatraz:
            {
                DoStartConversationOnPlayers(eConversations::Conv_EscapeFromTheArcatraz);
                break;
            }
            default:
                break;
            }
        }

        void OnCreatureCreateForScript(Creature* p_Creature) override
        {
            switch (p_Creature->GetEntry())
            {
            case eNpcs::Npc_LegionPortal:
            {
                m_LegionPortalsGuids.push_back(p_Creature->GetGUID());
                break;
            }
            case eNpcs::Npc_GenericBunnyKmart:
            {
                if (p_Creature->ComputeLinkedId() == "F10E99C72B51828F4D4FD587723BA25927A5E71D")
                {
                    m_StasisDoorBunnyGuid = p_Creature->GetGUID();
                }

                if (std::find(g_LahzaruunMinionLinkedIds.begin(), g_LahzaruunMinionLinkedIds.end(), p_Creature->ComputeLinkedId()) != g_LahzaruunMinionLinkedIds.end())
                {
                    m_LahzaruunMinionGuids.push_back(p_Creature->GetGUID());
                }

                break;
            }
            case eNpcs::Npc_Lahzaruun:
            {
                m_LahzaruunGuid = p_Creature->GetGUID();
                break;
            }
            case eNpcs::Npc_HighInquisitorRaalgar:
            {
                m_HighInquisitorRaalgarGuid = p_Creature->GetGUID();
                break;
            }
            default:
            {
                if (std::find(g_LahzaruunMinionLinkedIds.begin(), g_LahzaruunMinionLinkedIds.end(), p_Creature->ComputeLinkedId()) != g_LahzaruunMinionLinkedIds.end())
                {
                    m_LahzaruunMinionGuids.push_back(p_Creature->GetGUID());
                }

                break;
            }
            }
        }

        void OnGameObjectCreate(GameObject* p_GameObject) override
        {
            switch (p_GameObject->GetEntry())
            {
            case eGobs::Gob_StasisDoor:
            {
                m_StasisDoorGuid = p_GameObject->GetGUID();
                break;
            }
            case eGobs::Gob_NaaruPrison:
            {
                m_NaaruPrisonGuid = p_GameObject->GetGUID();
                break;
            }
            default:
                break;
            }
        }

        void SetData(uint32 /*p_Type*/, uint32 p_Data) override
        {
            switch (p_Data)
            {
            case eDatas::Data_ActivatePortals:
            {
                if (m_InstanceState < eInstanceStates::State_CloseLegionPortals)
                {
                    for (auto l_Itr : m_LegionPortalsGuids)
                    {
                        if (Creature* l_Creature = instance->GetCreature(l_Itr))
                        {
                            l_Creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        }
                    }

                    DoStartConversationOnPlayers(eConversations::Conv_ClosePortals);
                    m_InstanceState = eInstanceStates::State_CloseLegionPortals;
                }

                break;
            }
            case eDatas::Data_PortalClosed:
            {
                ++m_ClosedPortals;

                if (m_ClosedPortals == 2)
                {
                    HandleGameObject(m_StasisDoorGuid, true);

                    if (Creature* l_Creature = instance->GetCreature(m_StasisDoorBunnyGuid))
                    {
                        Map::PlayerList const &l_PlayerList = instance->GetPlayers();

                        if (!l_PlayerList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator l_I = l_PlayerList.begin(); l_I != l_PlayerList.end(); ++l_I)
                            {
                                if (Player* l_Player = l_I->getSource())
                                {
                                    l_Creature->AI()->Talk(0, l_Player->GetGUID(), 3);
                                }
                            }
                        }

                        for (auto l_Itr : m_LegionPortalsGuids)
                        {
                            RemoveVisibilityForCreature(l_Itr);
                        }

                        l_Creature->SetVisible(false);
                    }

                    m_InstanceState = eInstanceStates::State_HighInquisitorRaalgarFound;
                }

                break;
            }
            case eDatas::Data_InquisitorFound:
            {
                if (GameObject* l_Gob = instance->GetGameObject(m_NaaruPrisonGuid))
                {
                    l_Gob->Delete();
                }

                if (Creature* l_Creature = instance->GetCreature(m_LahzaruunGuid))
                {
                    l_Creature->AI()->DoAction(eActions::Action_LahzaruunStartEvent);
                }

                m_InstanceState = eInstanceStates::State_LahzaruunSlain;
                break;
            }
            case eDatas::Data_LahzaruunSlained:
            {
                instance->SummonGameObject(eGobs::Gob_PortalToTheVindicaar, g_PortalToTheVindicaarSpawnPos);
                DoStartConversationOnPlayers(eConversations::Conv_EscapeFromTheArcatraz);
                m_InstanceState = eInstanceStates::State_SearchTheArcatraz;
                break;
            }
            default:
                break;
            }
        }

        void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
        {
            WorldSafeLocsEntry const* l_Loc = nullptr;

            switch (m_InstanceState)
            {
            case eInstanceStates::State_SearchTheArcatraz:
            case eInstanceStates::State_CloseLegionPortals:
            {
                l_Loc = sWorldSafeLocsStore.LookupEntry(eWorldSafeLocs::Loc_ArcatrazEnter);
                break;
            }
            case eInstanceStates::State_HighInquisitorRaalgarFound:
            {
                l_Loc = sWorldSafeLocsStore.LookupEntry(eWorldSafeLocs::Loc_ArcatrazSecondFloor);
                break;
            }
            case eInstanceStates::State_LahzaruunSlain:
            case eInstanceStates::State_EscapeFromTheArcatraz:
            {
                l_Loc = sWorldSafeLocsStore.LookupEntry(eWorldSafeLocs::Loc_ArcatrazBoss);
                break;
            }
            default:
                break;
            }

            if (l_Loc == nullptr)
                return;

            p_X = l_Loc->x;
            p_Y = l_Loc->y;
            p_Z = l_Loc->z;
            p_O = l_Loc->o;
            p_MapID = l_Loc->MapID;
        }

        void SetInstanceState(Player* l_Player)
        {
            if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SearchTheArcatraz) == 1)
            {
                m_InstanceState = eInstanceStates::State_CloseLegionPortals;
            }
            if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CloseLegionPortals) == 2)
            {
                m_InstanceState = eInstanceStates::State_HighInquisitorRaalgarFound;
            }
            if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_HighInquisitorRaalgarFound) == 1)
            {
                m_InstanceState = eInstanceStates::State_LahzaruunSlain;
            }
            if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_LahzaruunSlain) == 1)
            {
                m_InstanceState = eInstanceStates::State_EscapeFromTheArcatraz;
            }

            m_ClosedPortals = l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CloseLegionPortals);

            switch (m_InstanceState)
            {
            case eInstanceStates::State_CloseLegionPortals:
            {
                for (auto l_Itr : m_LegionPortalsGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(l_Itr))
                    {
                        l_Creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }

                break;
            }
            case eInstanceStates::State_LahzaruunSlain:
            {
                if (GameObject* l_Gob = instance->GetGameObject(m_NaaruPrisonGuid))
                {
                    l_Gob->Delete();
                }

                if (Creature* l_Creature = instance->GetCreature(m_LahzaruunGuid))
                {
                    l_Creature->AI()->DoAction(eActions::Action_LahzaruunMoveToFightPos);
                }

                RemoveVisibilityForCreature(m_HighInquisitorRaalgarGuid);

                for (auto l_Itr : m_LahzaruunMinionGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(l_Itr))
                    {
                        if (l_Creature->GetEntry() == eNpcs::Npc_EredarEnforcer)
                        {
                            l_Creature->SetFacingTo(0.01745329f);
                            l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 27);
                            l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                            l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
                        }
                        else if (l_Creature->GetEntry() == eNpcs::Npc_EredarEnforcer)
                        {
                            l_Creature->CastStop();
                        }
                        else if (l_Creature->GetEntry() == eNpcs::Npc_GenericBunnyKmart)
                        {
                            RemoveVisibilityForCreature(l_Itr);
                        }
                    }
                }

                break;
            }
            case eInstanceStates::State_EscapeFromTheArcatraz:
            {
                if (GameObject* l_Gob = instance->GetGameObject(m_NaaruPrisonGuid))
                {
                    l_Gob->Delete();
                }

                RemoveVisibilityForCreature(m_HighInquisitorRaalgarGuid);
                RemoveVisibilityForCreature(m_LahzaruunGuid);

                for (auto l_Itr : m_LahzaruunMinionGuids)
                {
                    RemoveVisibilityForCreature(l_Itr);
                }

                instance->SummonGameObject(eGobs::Gob_PortalToTheVindicaar, g_PortalToTheVindicaarSpawnPos);
                break;
            }
            default:
                break;
            }

            if (m_InstanceState > eInstanceStates::State_CloseLegionPortals)
            {
                for (auto l_Itr : m_LegionPortalsGuids)
                {
                    RemoveVisibilityForCreature(l_Itr);
                }
            }

            if (m_InstanceState >= eInstanceStates::State_HighInquisitorRaalgarFound)
            {
                HandleGameObject(m_StasisDoorGuid, true);
                RemoveVisibilityForCreature(m_StasisDoorBunnyGuid);
            }
        }

        void RemoveVisibilityForCreature(uint64 p_Guid)
        {
            if (Creature* l_Creature = instance->GetCreature(p_Guid))
            {
                l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                l_Creature->SetFlag(UNIT_FIELD_FLAGS, 768);
                l_Creature->SetVisible(false);
            }
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_argus_arcatraz_InstanceMapScript(map);
    }
};

void AddSC_instance_argus_arcatraz()
{
    new instance_argus_arcatraz();
}
