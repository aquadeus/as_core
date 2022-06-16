////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "trial_of_valor.hpp"

DoorData const g_DoorData[] =
{
    { eTovGameObjects::GameObjectGuarmBossDoor01,      eTovData::DataBossGuarm, DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eTovGameObjects::GameObjectGuarmBossDoor02,      eTovData::DataBossGuarm, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { 0,                                               0,                       DOOR_TYPE_ROOM,       BOUNDARY_NONE }   ///< End
};

/// Trial of Valor - 1648
class instance_trial_of_valor : public InstanceMapScript
{
    public:
        instance_trial_of_valor() : InstanceMapScript("instance_trial_of_valor", 1648) { }

        struct instance_trial_of_valor_InstanceMapScript : public InstanceScript
        {
            instance_trial_of_valor_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                m_Initialized = false;
            }

            bool m_Initialized;

            /// Odyn
            uint64 m_OdynGuid;
            uint64 m_OdynDoorGuid; ///< Encounter is weird so.. didn't found how to handle it with the door system.
            uint64 m_OdynChestGuid;
            uint64 m_HymdallGuid;
            uint64 m_HyrjaGuid;

            uint64 m_GateWayOfSplendorGuid;
            uint64 m_OdynGateGuid;
            uint64 m_ValhallaBridgeGuid;

            uint64 m_RunesGuid[5]; ///< NKIAH

            uint32 m_CheckIfBelowFloorDiff;

            bool m_HaustvaldIntro;
            bool m_HaustvaldCheckPoints[3] = { false, false, false };

            uint32 m_CheckIfHaustvaldDiff;
            uint32 m_CheckIfNearPortalToHelheimDiff;

            /// Guarm
            uint64 m_GuarmGuid;
            uint64 m_GuarmChewToyGuid;

            /// Helya
            uint64 m_HelyaGuid;

            std::set<uint64> m_RitualStoneGuids;

            bool m_GuarmPulledOnce = false;

            void Initialize() override
            {
                SetBossNumber(eTovData::DataMaxBosses);

                LoadDoorData(g_DoorData);

                /// Odyn
                m_Initialized                = true;

                /// General
                m_ValhallaBridgeGuid         = 0;

                m_OdynGuid                   = 0;
                m_OdynDoorGuid               = 0;
                m_OdynChestGuid              = 0;
                m_HymdallGuid                = 0;
                m_HyrjaGuid                  = 0;

                m_HaustvaldIntro = 0;
                m_CheckIfBelowFloorDiff = 0;
                m_CheckIfHaustvaldDiff = 0;

                m_HaustvaldCheckPoints[0] = false;
                m_HaustvaldCheckPoints[1] = false;
                m_HaustvaldCheckPoints[2] = false;

                m_CheckIfNearPortalToHelheimDiff = 3 * TimeConstants::IN_MILLISECONDS;

                /// Guarm
                m_GuarmGuid = 0;
                m_GuarmChewToyGuid = 0;

                /// Helya
                m_HelyaGuid                  = 0;
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                for (uint8 l_I = 0; l_I < 5; l_I++)
                {
                    p_Player->RemoveAura(g_FixationSpells[l_I]);
                    p_Player->RemoveAura(g_RunicBrandSpells[l_I]);
                    p_Player->RemoveAura(g_MythicBranded[l_I]);
                }
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                for (uint8 l_I = 0; l_I < 5; l_I++)
                {
                    p_Player->RemoveAura(g_FixationSpells[l_I]);
                    p_Player->RemoveAura(g_RunicBrandSpells[l_I]);
                    p_Player->RemoveAura(g_MythicBranded[l_I]);
                }
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eTovCreatures::CreatureWorldTrigger:
                    {
                        p_Creature->SetDisplayId(11686);
                        break;
                    }
                    case eTovCreatures::CreatureAudience:
                    case eTovCreatures::CreatureAudience01:
                    case eTovCreatures::CreatureAudience02:
                    case eTovCreatures::CreatureAudience03:
                    {
                        if (roll_chance_i(50))
                            return;

                        std::vector<uint32> l_Emotes =
                        {
                            71, 15, 274
                        };

                        p_Creature->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, l_Emotes[irand(0, 2)]);
                        break;
                    }
                    case eTovCreatures::CreatureVikingARpFriendly:
                    case eTovCreatures::CreatureVikingARpFriendly02:
                    case eTovCreatures::CreatureVikingARpFriendly03:
                    {
                        p_Creature->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 1);
                        break;
                    }
                    case 19871: ///< World trigger for Shield of Light
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        p_Creature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        break;
                    }
                    case eTovCreatures::CreatureBossOdyn:
                    {
                        m_OdynGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eTovCreatures::CreatureBossHelya:
                    {
                        m_HelyaGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eTovCreatures::CreatureGuarmHeadLeft:
                    case eTovCreatures::CreatureGuarmHeadMiddle:
                    case eTovCreatures::CreatureGuarmHeadRight:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        break;
                    }
                    case eTovCreatures::CreatureGuarmChewToy:
                    {
                        m_GuarmChewToyGuid = p_Creature->GetGUID();

                        p_Creature->SetDisplayId(11686);

                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    case eTovCreatures::CreatureBossGuarm:
                    {
                        m_GuarmGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eTovCreatures::CreatureHymdall:
                    {
                        m_HymdallGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eTovCreatures::CreatureHyrja:
                    {
                        m_HyrjaGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eTovCreatures::CreatureValkyrShieldMaiden:
                    {
                        p_Creature->CastSpell(p_Creature, eTovSpells::SpellKneelingVisual, true);
                        break;
                    }
                    case eTovCreatures::CreatureSpearOfLight:
                    {
                        p_Creature->DespawnOrUnsummon(3 * TimeConstants::IN_MILLISECONDS);

                        p_Creature->SetDisplayId(11686);
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                        break;
                    }
                    case eTovCreatures::CreatureVikingBlueGuard:
                    {
                        p_Creature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        break;
                    }
                    case eTovCreatures::CreatureGraspingTentacleLeft:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->CastSpell(p_Creature, eTovSpells::SpellHelyaTentacleLeft, true);
                        break;
                    }
                    case eTovCreatures::CreatureGraspingTentacleRight:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->CastSpell(p_Creature, eTovSpells::SpellHelyaTentacleRight, true);
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
                    case eTovGameObjects::GameObjectOdynGate:
                    {
                        p_GameObject->GetMap()->SetObjectVisibility(1000.0f);
                        m_OdynDoorGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectOdynChest:
                    {
                        m_OdynChestGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectValhallaBridge001:
                    {
                        p_GameObject->GetMap()->SetObjectVisibility(1000.0f);
                        m_ValhallaBridgeGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectOdynRuneSigilN:
                    {
                        m_RunesGuid[0] = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectOdynRuneSigilK:
                    {
                        m_RunesGuid[1] = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectOdynRuneSigilI:
                    {
                        m_RunesGuid[2] = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectOdynRuneSigilA:
                    {
                        m_RunesGuid[3] = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectOdynRuneSigilH:
                    {
                        m_RunesGuid[4] = p_GameObject->GetGUID();
                        break;
                    }
                    case eTovGameObjects::GameObjectGuarmBossDoor01:
                    case eTovGameObjects::GameObjectGuarmBossDoor02:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case eTovGameObjects::GameObjectRitualStone:
                    {
                        m_RitualStoneGuids.insert(p_GameObject->GetGUID());
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
                    case eTovCreatures::CreatureDarkSeraph:
                    case eTovCreatures::CreatureKvaldirTideWitch:
                    case eTovCreatures::CreatureDeepbrineMonstruosity:
                    case eTovCreatures::CreatureKvaldirReefcaller:
                    case eTovCreatures::CreatureAncientBonethrall:
                    case eTovCreatures::CreatureRotsoulGiant:
                    case eTovCreatures::CreatureKvaldirCoralMaiden:
                    {
                        if (Creature* l_Helya = instance->GetCreature(m_HelyaGuid))
                        {
                            if (l_Helya->IsAIEnabled)
                                l_Helya->AI()->SetGUID(p_Unit->GetGUID());
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eTovData::DataHelyaRitualStones:
                    {
                        for (uint64 l_Guid : m_RitualStoneGuids)
                        {
                            if (GameObject* l_Stone = instance->GetGameObject(l_Guid))
                                l_Stone->SetGoState(p_Value ? GOState::GO_STATE_READY : GOState::GO_STATE_ACTIVE);
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
                    case eTovData::DataBossHelya:
                    {
                        switch (p_State)
                        {
                            case EncounterState::DONE:
                            {
                                /// "Test of the Chosen" aura is already checked with additional conditions
                                if (instance->IsMythic())
                                    DoCompleteAchievement(eTovAchievements::TheChosen);

                                /// Spawn Odyn's and relative's cosmetics
                                {
                                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        if (Creature* l_Odyn = instance->SummonCreature(eTovCreatures::CreatureOutroOdyn, { 498.5955f, 640.2118f, 90.02425f, 1.167363f }))
                                        {
                                            l_Odyn->SummonGameObject(eTovGameObjects::GameObjectLightRay1, { 569.1458f, 571.0018f, 26.45635f, 3.125252f });
                                            l_Odyn->SummonGameObject(eTovGameObjects::GameObjectLightRay1, { 435.2552f, 625.0625f, 17.68612f, 1.785507f });
                                            l_Odyn->SummonGameObject(eTovGameObjects::GameObjectLightRay2, { 495.9306f, 598.8177f, -23.0224f, 3.125252f });
                                        }

                                        uint32 l_Time = 6 * TimeConstants::IN_MILLISECONDS;
                                        AddTimedDelayedOperation(l_Time, [this]() -> void
                                        {
                                            instance->SummonCreature(eTovCreatures::CreatureOutroHyrja,     { 519.0164f, 574.5577f, 57.12081f, 1.816785f });
                                            instance->SummonCreature(eTovCreatures::CreatureOutroEyir,      { 501.4125f, 563.9207f, 60.65930f, 1.087068f });
                                            instance->SummonCreature(eTovCreatures::CreatureOutroHymdall,   { 485.6389f, 644.7465f, 1.926481f, 1.167363f });
                                        });

                                        l_Time += 4 * TimeConstants::IN_MILLISECONDS;
                                        AddTimedDelayedOperation(l_Time, [this]() -> void
                                        {
                                            instance->SummonCreature(eTovCreatures::CreatureOutroStormforgedSentinel, { 463.908f, 664.0347f, 5.285385f, 0.3524198f });
                                            instance->SummonCreature(eTovCreatures::CreatureOutroStormforgedSentinel, { 473.2535f, 649.8854f, 5.112077f, 0.6415704f });
                                        });

                                        l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                                        AddTimedDelayedOperation(l_Time, [this]() -> void
                                        {
                                            if (Creature* l_Chosen = instance->SummonCreature(eTovCreatures::CreatureOutroChosenOfEyir, { 565.3877f, 575.309f, 66.28792f, 5.226926f }))
                                            {
                                                if (l_Chosen->IsAIEnabled)
                                                    l_Chosen->AI()->SetData(0, 0);
                                            }

                                            if (Creature* l_Chosen = instance->SummonCreature(eTovCreatures::CreatureOutroChosenOfEyir, { 537.0193f, 555.2808f, 62.34981f, 0.5324915f }))
                                            {
                                                if (l_Chosen->IsAIEnabled)
                                                    l_Chosen->AI()->SetData(0, 1);
                                            }
                                        });

                                        l_Time += 2 * TimeConstants::IN_MILLISECONDS + 500;
                                        AddTimedDelayedOperation(l_Time, [this]() -> void
                                        {
                                            instance->SummonCreature(eTovCreatures::CreatureOutroStormforgedSentinel, { 458.8663f, 671.7118f, 9.075208f, 0.2078661f });
                                            instance->SummonCreature(eTovCreatures::CreatureOutroStormforgedSentinel, { 467.6684f, 656.8646f, 5.112077f, 0.4892152f });
                                        });
                                    });
                                }

                                /// No break
                            }
                            case EncounterState::FAIL:
                            {
                                for (uint64 l_Guid : m_RitualStoneGuids)
                                {
                                    if (GameObject* l_Stone = instance->GetGameObject(l_Guid))
                                    {
                                        l_Stone->SendGameObjectActivateAnimKit(0);
                                        l_Stone->SetGoState(GOState::GO_STATE_ACTIVE);
                                    }
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eTovData::DataBossGuarm:
                    {
                        switch (p_State)
                        {
                            case EncounterState::DONE:
                            {
                                /// Call conversation
                                if (Creature* l_Odyn = instance->GetCreature(m_OdynGuid))
                                {
                                    if (CreatureAI* l_AI = l_Odyn->AI())
                                        l_AI->Talk(20);
                                }

                                break;
                            }
                            case EncounterState::IN_PROGRESS:
                            {
                                m_GuarmPulledOnce = true;
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

                return true;
            }

            bool CheckRequiredBosses(uint32 p_BossID, Player const* p_Player) const override
            {
                if (!InstanceScript::CheckRequiredBosses(p_BossID, p_Player))
                    return false;

                if (p_BossID == eTovData::DataBossOdyn)
                    return true;

                if (GetBossState(p_BossID - 1) != EncounterState::DONE)
                    return false;

                return true;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case eTovData::DataBossOdyn:
                    case eTovCreatures::CreatureBossOdyn:
                        return m_OdynGuid;
                    case eTovCreatures::CreatureHyrja:
                        return m_HyrjaGuid;
                    case eTovCreatures::CreatureHymdall:
                        return m_HymdallGuid;
                    case eTovCreatures::CreatureBossHelya:
                        return m_HelyaGuid;
                    case eTovData::DataBossGuarm:
                    case eTovCreatures::CreatureBossGuarm:
                        return m_GuarmGuid;
                    case eTovGameObjects::GameObjectGatewayOfSplendor:
                        return m_GateWayOfSplendorGuid;
                    case eTovGameObjects::GameObjectOdynRuneSigilN:
                        return m_RunesGuid[0];
                    case eTovGameObjects::GameObjectOdynRuneSigilK:
                        return m_RunesGuid[1];
                    case eTovGameObjects::GameObjectOdynRuneSigilI:
                        return m_RunesGuid[2];
                    case eTovGameObjects::GameObjectOdynRuneSigilA:
                        return m_RunesGuid[3];
                    case eTovGameObjects::GameObjectOdynRuneSigilH:
                        return m_RunesGuid[4];
                    case eTovGameObjects::GameObjectOdynGate:
                        return m_OdynDoorGuid;
                    case eTovGameObjects::GameObjectOdynChest:
                        return m_OdynChestGuid;
                        break;
                }

                return 0;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (m_CheckIfBelowFloorDiff <= p_Diff)
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->GetPositionZ() <= 609.240f && l_Player->GetAreaId() == 8440)
                            {
                                if (!l_Player->IsBeingTeleported())
                                {
                                    l_Player->NearTeleportTo(2593.1091f, 528.0110f, 748.9397f, 0.0f, false);

                                    l_Player->ResurrectPlayer(1.0f);
                                    l_Player->SpawnCorpseBones();
                                    l_Player->SaveToDB();
                                }
                            }
                        }
                    }

                    m_CheckIfBelowFloorDiff = 3 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_CheckIfBelowFloorDiff -= p_Diff;

                if (m_CheckIfHaustvaldDiff <= p_Diff)
                {
                    if (!m_HaustvaldCheckPoints[2])
                    {
                        Map::PlayerList const& l_Players = instance->GetPlayers();

                        if (!l_Players.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                {
                                    std::vector<uint32> l_CreatureFormation;

                                    bool m_Ok = false;

                                    if (!m_Ok && l_Player->m_positionZ <= 76.f && !m_HaustvaldCheckPoints[0])
                                    {
                                        m_Ok = true;
                                        m_HaustvaldCheckPoints[0] = true;

                                        l_CreatureFormation = { eTovCreatures::CreatureKvaldirReefcaller, eTovCreatures::CreatureKvaldirSpiritTender, eTovCreatures::CreatureKvaldirReefcaller };
                                    }

                                    if (!m_Ok && l_Player->m_positionZ <= 42.734f && m_HaustvaldCheckPoints[0] && !m_HaustvaldCheckPoints[1])
                                    {
                                        m_Ok = true;
                                        m_HaustvaldCheckPoints[1] = true;

                                        l_CreatureFormation = { eTovCreatures::CreatureKvaldirSpiritTender, eTovCreatures::CreatureKvaldirSpiritTender };
                                    }

                                    if (!m_Ok && l_Player->m_positionZ <= 30.749f && m_HaustvaldCheckPoints[1] && !m_HaustvaldCheckPoints[2])
                                    {
                                        m_Ok = true;
                                        m_HaustvaldCheckPoints[2] = true;

                                        l_CreatureFormation = { eTovCreatures::CreatureKvaldirSpiritTender, eTovCreatures::CreatureKvaldirReefcaller, eTovCreatures::CreatureKvaldirSpiritTender, eTovCreatures::CreatureKvaldirReefcaller, eTovCreatures::CreatureKvaldirSpiritTender };
                                    }

                                    if (m_Ok)
                                    {
                                        uint32 l_Size = l_CreatureFormation.size();
                                        uint8 l_Count = 0;

                                        for (uint32 l_Itr : l_CreatureFormation)
                                        {
                                            if (l_Count >= l_Size)
                                            {
                                                l_Count = 0;
                                                continue;
                                            }

                                            Position l_Pos = { g_HaustvaldReinforcemanSpawnFormation[l_Count].x, g_HaustvaldReinforcemanSpawnFormation[l_Count].y, g_HaustvaldReinforcemanSpawnFormation[l_Count].z, g_HaustvaldReinforcementSpawnOrientation };

                                            if (Creature* l_Kvaldir = instance->SummonCreature(l_Itr, l_Pos, nullptr, 1 * TimeConstants::WEEK))
                                            {
                                                l_Kvaldir->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                                float l_Distance = 0.0f;

                                                Position l_Dest;

                                                if (Player* l_Nearest = l_Kvaldir->FindNearestPlayer(200.f))
                                                {
                                                    l_Kvaldir->SetSpeed(UnitMoveType::MOVE_RUN, 1.35f, true);

                                                    if (UnitAI* l_AI = l_Kvaldir->AI())
                                                        l_AI->AttackStart(l_Nearest);
                                                }
                                            }

                                            l_Count++;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    m_CheckIfHaustvaldDiff = 18 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_CheckIfHaustvaldDiff -= p_Diff;

                if (m_CheckIfNearPortalToHelheimDiff <= p_Diff)
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();

                    if (!l_Players.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                        {
                            if (Player* l_Itr = l_Iter->getSource())
                            {
                                if ((l_Itr == nullptr) || l_Itr->isDead() || !l_Itr->FindNearestGameObject(eTovGameObjects::GameObjectPortalToHelheim, 5.0f) || l_Itr->IsBeingTeleportedNear())
                                    continue;

                                if (l_Itr->GetPositionZ() <= -11.326f && m_HaustvaldCheckPoints[2]) ///< After reinforce event.
                                    l_Itr->NearTeleportTo(g_HaustvaldSecondPortal);
                                else if (l_Itr->GetPositionZ() >= 30.69f)
                                    l_Itr->NearTeleportTo(g_HaustvaldFirstPortal);
                            }
                        }
                    }

                    m_CheckIfNearPortalToHelheimDiff = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_CheckIfNearPortalToHelheimDiff -= p_Diff;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream l_SaveStream;
                l_SaveStream << "H V " << GetBossSaveData() << " ";

                OUT_SAVE_INST_DATA_COMPLETE;
                return l_SaveStream.str();
            }

            void Load(char const* p_In) override
            {
                if (!p_In)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(p_In);

                char l_DataHead1;
                char l_DataHead2;

                std::istringstream l_LoadStream(p_In);

                l_LoadStream >> l_DataHead1 >> l_DataHead2;

                if (l_DataHead1 == 'H' && l_DataHead2 == 'V')
                {
                    for (uint8 l_I = 0; l_I < 3; ++l_I)
                    {
                        uint32 l_TmpState;

                        l_LoadStream >> l_TmpState;

                        if (l_TmpState == IN_PROGRESS || l_TmpState > SPECIAL)
                            l_TmpState = NOT_STARTED;

                        SetBossState(l_I, EncounterState(l_TmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (GetBossState(eTovData::DataBossGuarm) == EncounterState::DONE || m_GuarmPulledOnce)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eToVLocations::LocHelheimRaidHelheimEntrance);
                else
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eToVLocations::LocHelheimRaidEntrance);

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
            return new instance_trial_of_valor_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_trial_of_valor()
{
    new instance_trial_of_valor();
}
#endif
