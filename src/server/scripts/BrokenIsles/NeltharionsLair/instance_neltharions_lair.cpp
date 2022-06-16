////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "neltharions_lair.hpp"

DoorData const g_DoorData[] =
{
    { GO_ROKMORA_DOOR,           DATA_ROKMORA,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { GO_ULAROGG_DOOR,           DATA_ULAROGG,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { GO_NARAXAS_DOOR,           DATA_NARAXAS,       DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { GO_NARAXAS_EXIT_DOOR_1,    DATA_NARAXAS,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { GO_NARAXAS_EXIT_DOOR_2,    DATA_NARAXAS,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { GO_NARAXAS_COLLISION,      DATA_NARAXAS,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { 0,                         0,                  DOOR_TYPE_PASSAGE,      BOUNDARY_NONE }
};

class instance_neltharions_lair : public InstanceMapScript
{
    public:
        instance_neltharions_lair() : InstanceMapScript("instance_neltharions_lair", 1458) { }

        struct instance_neltharions_lair_InstanceMapScript : public InstanceScript
        {
            instance_neltharions_lair_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);

                m_NavaroggGuid = 0;
                m_ElderhornGuid = 0;

                m_RokmoraGuid = 0;
                m_CosmeticUlarogg = 0;

                m_NaraxasCacheGuid = 0;

                m_DargrulGuid = 0;
            }

            uint64 m_NavaroggGuid;
            uint64 m_ElderhornGuid;

            uint64 m_RokmoraGuid;
            uint64 m_CosmeticUlarogg;

            uint64 m_NaraxasCacheGuid;

            uint64 m_DargrulGuid;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_ROKMORA_DOOR:
                    case GO_ULAROGG_DOOR:
                    case GO_NARAXAS_DOOR:
                    case GO_NARAXAS_EXIT_DOOR_1:
                    case GO_NARAXAS_EXIT_DOOR_2:
                    case GO_NARAXAS_COLLISION:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case GO_REMAINS_OF_THE_FALLEN:
                    {
                        m_NaraxasCacheGuid = p_GameObject->GetGUID();
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
                    case GO_ROKMORA_DOOR:
                    case GO_ULAROGG_DOOR:
                    case GO_NARAXAS_DOOR:
                    case GO_NARAXAS_EXIT_DOOR_1:
                    case GO_NARAXAS_EXIT_DOOR_2:
                    case GO_NARAXAS_COLLISION:
                    {
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
                    case NPC_NAVAROGG:
                    {
                        m_NavaroggGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_SPIRITWALKER_EBONHORN:
                    {
                        m_ElderhornGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_ROKMORA:
                    {
                        m_RokmoraGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_COSMETIC_ULAROGG:
                    {
                        m_CosmeticUlarogg = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_DARGRUL:
                    {
                        m_DargrulGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_EMBERSHARD_SCORPION:
                    {
                        Position l_Pos = { 2948.37f, 1217.93f, 101.896f, 2.48148f };

                        if (p_Creature->IsNearPosition(&l_Pos, 0.5f))
                            p_Creature->CastSpell(p_Creature, eNeltharionSpells::SpellPermFeignDeath, true);

                        break;
                    }
                    case NPC_STONECLAW_HUNTER:
                    {
                        p_Creature->DelayedCastSpell(p_Creature, eNeltharionSpells::SpellHunterBasilik, true, 1);
                        break;
                    }
                    case NPC_DRUMS_OF_WAR:
                    {
                        p_Creature->SetReactState(REACT_PASSIVE);
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
                    case NPC_NAVAROGG:
                        return m_NavaroggGuid;
                    case NPC_SPIRITWALKER_EBONHORN:
                        return m_ElderhornGuid;
                    case NPC_ROKMORA:
                        return m_RokmoraGuid;
                    case NPC_COSMETIC_ULAROGG:
                        return m_CosmeticUlarogg;
                    case GO_REMAINS_OF_THE_FALLEN:
                        return m_NaraxasCacheGuid;
                    case NPC_DARGRUL:
                        return m_DargrulGuid;
                    default:
                        break;
                }

                return 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Navarogg = instance->GetCreature(m_NavaroggGuid))
                    {
                        if (l_Navarogg->IsAIEnabled)
                            l_Navarogg->AI()->DoAction(0);
                    }

                    if (Creature* l_Elderhorn = instance->GetCreature(m_ElderhornGuid))
                    {
                        if (l_Elderhorn->IsAIEnabled)
                            l_Elderhorn->AI()->DoAction(0);
                    }
                });
            }

            void HandlePlayerStartFalling(Player* p_FallingPlr) override
            {
                if (p_FallingPlr->m_positionZ < 340.0f || !p_FallingPlr->IsMoving())
                    return;

                p_FallingPlr->CastSpell(p_FallingPlr, eNeltharionSpells::SpellEntranceRun, true);

                std::vector<G3D::Vector3> l_Path =
                {
                    { 2950.880f, 1014.229f, 325.4731f },
                    { 2958.365f, 1018.120f, 318.2576f },
                    { 2982.388f, 1037.797f, 305.1869f },
                    { 2988.570f, 1066.708f, 289.7835f },
                    { 2976.042f, 1089.498f, 273.5788f },
                    { 2944.292f, 1097.300f, 256.6184f },
                    { 2923.748f, 1085.398f, 245.9293f },
                    { 2910.531f, 1070.689f, 235.4604f },
                    { 2887.073f, 1081.453f, 222.4978f },
                    { 2870.568f, 1098.023f, 184.3552f },
                    { 2864.102f, 1101.613f, 100.5632f }
                };

                p_FallingPlr->SetSpeed(UnitMoveType::MOVE_FLIGHT, 5.5f);
                p_FallingPlr->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                p_FallingPlr->GetMotionMaster()->MoveSmoothFlyPath(0, l_Path.data(), l_Path.size());

                uint64 l_Guid = p_FallingPlr->GetGUID();
                AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS + 500, [this, l_Guid]() -> void
                {
                    if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                    {
                        l_Player->RemoveAura(eNeltharionSpells::SpellEntranceRun);
                        l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);
                        l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                    }
                });
            }

            bool SetBossState(uint32 p_Type, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Type, p_State))
                    return false;

                switch (p_Type)
                {
                    case DATA_NARAXAS:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            if (instance->GetDifficultyID() != Difficulty::DifficultyMythicKeystone)
                            {
                                if (GameObject* l_Cache = instance->GetGameObject(m_NaraxasCacheGuid))
                                    l_Cache->SetRespawnTime(86400);
                            }

                            DoCastSpellOnPlayers(eNeltharionSpells::SpellWormSafeFall);
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

                switch (p_BossID)
                {
                    case DATA_ULAROGG:
                    {
                        if (GetBossState(DATA_ROKMORA) != DONE)
                            return false;
                        break;
                    }
                    case DATA_NARAXAS:
                    {
                        if (GetBossState(DATA_ULAROGG) != DONE)
                            return false;
                        break;
                    }
                    case DATA_DARGRUL:
                    {
                        if (GetBossState(DATA_NARAXAS) != DONE)
                            return false;
                        break;
                    }
                    case DATA_ROKMORA:
                    default:
                        break;
                }

                return true;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (GetBossState(eData::DATA_NARAXAS) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eNeltharionLocations::LocWormDefeatedCheckpoint);
                else if (GetBossState(eData::DATA_ULAROGG) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eNeltharionLocations::LocMidwayCheckpoint);
                else if (GetBossState(eData::DATA_ROKMORA) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eNeltharionLocations::LocStoneGiantCheckpoint);

                if (l_Loc == nullptr)
                    return;

                p_X = l_Loc->x;
                p_Y = l_Loc->y;
                p_Z = l_Loc->z;
                p_O = l_Loc->o;
                p_MapID = l_Loc->MapID;
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
                    case eCreatures::NPC_ROKMORA:
                    case eCreatures::NPC_COSMETIC_ULAROGG:
                    case eCreatures::NPC_BOSS_ULAROGG:
                    case eCreatures::NPC_DARGRUL:
                    case eCreatures::NPC_NARAXAS:
                    {
                        return false;
                    }

                    default: break;
                }

                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_neltharions_lair_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_neltharions_lair()
{
    new instance_neltharions_lair();
}
#endif
