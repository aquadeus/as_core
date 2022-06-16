////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "darkheart_thicket.hpp"

DoorData const g_DoorData[] =
{
    {GO_GLAIDALIS_FIRE_DOOR,     DATA_GLAIDALIS,     DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_DRESARON_FIRE_DOOR,      DATA_DRESARON,      DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {0,                          0,                  DOOR_TYPE_ROOM,     BOUNDARY_NONE}
};

class instance_darkheart_thicket : public InstanceMapScript
{
    public:
        instance_darkheart_thicket() : InstanceMapScript("instance_darkheart_thicket", 1466) { }

        struct instance_darkheart_thicket_InstanceMapScript : public InstanceScript
        {
            instance_darkheart_thicket_InstanceMapScript(Map* map) : InstanceScript(map) 
            {
                SetBossNumber(MAX_ENCOUNTER);

                MalfurionGUID = 0;
                MalfurionCageGUID = 0;
                InvisDoorGUID = 0;

                m_IntroDone = false;
            }

            bool m_IntroDone;

            uint64 MalfurionGUID;
            uint64 MalfurionCageGUID;
            uint64 InvisDoorGUID;
            uint64 m_OakheartGuid = 0;

            uint8 m_NightmareCount = 0;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;
            
                return true;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_MALFURION_STORMRAGE:    
                        MalfurionGUID = creature->GetGUID();
                        break;
                    case NPC_NIGHTMARE_BINDINGS:
                        MalfurionCageGUID = creature->GetGUID();
                        break;
                    case NPC_NIGHTMARE_DWELLER:
                        ++m_NightmareCount;
                        break;
                    case NPC_OAKHEART:
                        m_OakheartGuid = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_GLAIDALIS_FIRE_DOOR:
                    case GO_DRESARON_FIRE_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_GLAIDALIS_INVIS_DOOR:
                        InvisDoorGUID = go->GetGUID();
                       break;
                    default:
                        break;
                }
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
            {
                InstanceScript::CreatureDiesForScript(p_Creature, p_Killer);

                switch (p_Creature->GetEntry())
                {
                    case NPC_NIGHTMARE_DWELLER:
                    {
                        if (m_NightmareCount <= 0)
                            break;

                        --m_NightmareCount;

                        if (!m_NightmareCount)
                        {
                            Creature* l_Oakheart = instance->GetCreature(m_OakheartGuid);
                            if (!l_Oakheart || !l_Oakheart->IsAIEnabled)
                                break;

                            l_Oakheart->AI()->DoAction(0);
                        }

                        break;
                    }
                    default:
                        break;
                }
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

                return true;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case NPC_MALFURION_STORMRAGE:
                        return MalfurionGUID;
                    case NPC_NIGHTMARE_BINDINGS:
                        return MalfurionCageGUID;
                    case GO_GLAIDALIS_INVIS_DOOR:
                        return InvisDoorGUID;
                }

                return 0;
            }

            void OnCreatureKilled(Creature* p_Creature, Player* p_Killer) override
            {
                if (m_IntroDone)
                    return;

                if (p_Creature->GetEntry() == eCreatures::NPC_DREADSOUL_RUINER)
                {
                    Position l_Pos = p_Creature->GetHomePosition();
                    Position l_CheckPos = Position(3071.630f, 1956.18f, 203.201f, 0.0f);
                    if (l_Pos.IsNearPosition(&l_CheckPos, 1.5f))
                    {
                        m_IntroDone = true;
                        DoCastSpellOnPlayers(eGlobalSpells::SPELL_ENTRANCE_CONVERSATION);
                    }
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                uint32 l_GraveyardID = 5334;

                if (GetBossState(eData::DATA_GLAIDALIS) == DONE)
                    l_GraveyardID = 5874;

                if (WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(l_GraveyardID))
                {
                    p_X = l_Loc->x;
                    p_Y = l_Loc->y;
                    p_Z = l_Loc->z;
                    p_O = l_Loc->o;
                    p_MapID = l_Loc->MapID;
                }
            }

            bool CheckRequiredBosses(uint32 p_BossID, Player const* p_Player) const override
            {
                if (!InstanceScript::CheckRequiredBosses(p_BossID, p_Player))
                    return false;

                switch (p_BossID)
                {
                    case DATA_GLAIDALIS:
                        break;
                    case DATA_OAKHEART:
                        if (GetBossState(DATA_GLAIDALIS) != DONE)
                            return false;
                        break;
                    case DATA_DRESARON:
                        if (GetBossState(DATA_OAKHEART) != DONE)
                            return false;
                        break;
                    case DATA_XAVIUS:
                        if (GetBossState(DATA_DRESARON) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_darkheart_thicket_InstanceMapScript(map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_darkheart_thicket()
{
    new instance_darkheart_thicket();
}
#endif
