////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "maw_of_souls.hpp"

DoorData const g_DoorData[] =
{
    { GO_HARBARON_DOOR,      DATA_HARBARON,       DOOR_TYPE_PASSAGE,       BOUNDARY_NONE },
    { 0,                     0,                   DOOR_TYPE_ROOM,          BOUNDARY_NONE }
};

class instance_maw_of_souls : public InstanceMapScript
{
    public:
        instance_maw_of_souls() : InstanceMapScript("instance_maw_of_souls", 1492) { }

        struct instance_maw_of_souls_InstanceMapScript : public InstanceScript
        {
            instance_maw_of_souls_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            uint64 m_YmironGuid;
            uint64 m_YmironGornGuid;
            uint64 m_HelyaGuid;
            uint64 m_ShipGuid;
            uint64 m_HelyaChestGuid;
            uint64 m_SkjalWallGuid;

            uint32 m_YmironRPEvent;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);

                m_YmironGornGuid = 0;
                m_HelyaGuid      = 0;
                m_ShipGuid       = 0;
                m_HelyaChestGuid = 0;
                m_SkjalWallGuid  = 0;
                m_YmironRPEvent  = 0;
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
                    case eCreatures::NPC_DESTRUCTOR_TENTACLE:
                    case eCreatures::NPC_DESTRUCTOR_TENTACLE_2:
                    {
                        return false;
                    }

                    case eCreatures::NPC_SKJAL:
                    {
                        if (p_Affix == Affixes::Teeming)
                            return false;
                        
                        return false;
                    }

                    case eCreatures::NPC_RUNECARVER:
                    {
                        if (p_Affix == Affixes::Bolstering || p_Affix == Affixes::Sanguine)
                            return false;

                        return false;
                    }

                    default:
                        break;
                }

                return true;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case NPC_YMIRON:
                    {
                        m_YmironGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_HELYA:
                    {
                        p_Creature->m_SightDistance = 150.0f;
                        p_Creature->SetVisible(false);
                        m_HelyaGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_HELARJAR_CHAMPION:
                    case NPC_THE_GRIMEWALKER:
                    {
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
                        break;
                    }
                    case NPC_SEACURSED_ROWER:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
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
                    case GO_YMIRON_GORN:
                    {
                        m_YmironGornGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_SHIP:
                    {
                        m_ShipGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_HELYA_CHEST:
                    {
                        m_HelyaChestGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case GO_HARBARON_DOOR:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case GO_SKJAL_WALL_OF_SOULS:
                    {
                        m_SkjalWallGuid = p_GameObject->GetGUID();
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
                    case DATA_HELYA:
                    {
                        if (p_State == DONE)
                        {
                            if (instance->GetDifficultyID() != Difficulty::DifficultyMythicKeystone)
                            {
                                if (GameObject* l_Chest = instance->GetGameObject(m_HelyaChestGuid))
                                    l_Chest->SetRespawnTime(86400);
                            }

                            instance->SummonGameObject(GO_PORTAL_TO_STORMHEIM, { 2940.787f, 915.6528f, 513.0204f, 1.615085f });
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
                    case DATA_YMIRON:
                        break;
                    case DATA_HARBARON:
                        if (GetBossState(DATA_YMIRON) != DONE)
                            return false;
                        break;
                    case DATA_HELYA:
                        if (GetBossState(DATA_HARBARON) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (GetBossState(eData::DATA_HARBARON) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAfterHarbaron);
                else if (GetBossState(eData::DATA_YMIRON) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAfterYmiron);

                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream l_SaveStream;
                l_SaveStream << "M O S " << GetBossSaveData() << m_YmironRPEvent;

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

                char l_DataHead1, l_DataHead2, l_DataHead3;

                std::istringstream l_LoadStream(p_In);

                l_LoadStream >> l_DataHead1 >> l_DataHead2 >> l_DataHead3;

                if (l_DataHead1 == 'M' && l_DataHead2 == 'O' && l_DataHead3 == 'S')
                {
                    for (uint8 l_I = 0; l_I < MAX_ENCOUNTER; ++l_I)
                    {
                        uint32 l_TmpState;

                        l_LoadStream >> l_TmpState;

                        if (l_TmpState == IN_PROGRESS || l_TmpState > SPECIAL)
                            l_TmpState = NOT_STARTED;

                        SetBossState(l_I, EncounterState(l_TmpState));
                    }

                    l_LoadStream >> m_YmironRPEvent;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void SetData(uint32 p_Data, uint32 p_State) override
            {
                InstanceScript::SetData(p_Data, p_State);

                switch (p_Data)
                {
                    case eData::DATA_SKJAL_WALL:
                    {
                        if (GameObject* l_Wall = instance->GetGameObject(m_SkjalWallGuid))
                            l_Wall->SetGoState(GOState::GO_STATE_ACTIVE);

                        break;
                    }
                    case eData::DATA_YMIRON_RP_EVENT:
                    {
                        m_YmironRPEvent = p_State;

                        if (p_State == EncounterState::DONE)
                            SaveToDB();

                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_YMIRON:
                        return m_YmironGuid;
                    case DATA_YMIRON_GORN:
                        return m_YmironGornGuid;
                    case DATA_HELYA:
                        return m_HelyaGuid;
                    case DATA_SHIP:
                        return m_ShipGuid;
                    default:
                        break;
                }

                return 0;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_maw_of_souls_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_maw_of_souls()
{
    new instance_maw_of_souls();
}
#endif
