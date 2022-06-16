
////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "firelands.h"

// areatrigger
// 6929 - quest1
// 6861 - near miniboss 1

#define MAX_ENCOUNTER 7

static const DoorData doordata[] = 
{
    {GO_BRIDGE_OF_RHYOLITH,  DATA_RHYOLITH,  DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {GO_FIRE_WALL_BALEROC,   DATA_BALEROC,   DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {GO_RAID_BRIDGE_FORMING, DATA_BALEROC,   DOOR_TYPE_PASSAGE,     BOUNDARY_NONE},
    {GO_STICKY_WEB,          DATA_BETHTILAC, DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {GO_BRIDGE_OF_RHYOLITH,  DATA_RHYOLITH,  DOOR_TYPE_SPAWN_HOLE,  BOUNDARY_NONE},
    {GO_FIRE_WALL_FANDRAL_1, DATA_STAGHELM,  DOOR_TYPE_PASSAGE,     BOUNDARY_NONE},
    {GO_FIRE_WALL_FANDRAL_2, DATA_STAGHELM,  DOOR_TYPE_PASSAGE,     BOUNDARY_NONE},
    {GO_SULFURON_KEEP,       DATA_RAGNAROS,  DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {0, 0, DOOR_TYPE_ROOM, BOUNDARY_NONE},
};

class instance_firelands : public InstanceMapScript
{
    public:
        instance_firelands() : InstanceMapScript("instance_firelands", 720) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_firelands_InstanceMapScript(map);
        }

        struct instance_firelands_InstanceMapScript : public InstanceScript
        {
            instance_firelands_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);
                uiShannoxGUID = 0;
                uiRiplimbGUID = 0;
                uiRagefaceGUID = 0;
                uiRhyolithGUID = 0;
                uiRagnarosGUID = 0;
                uiRhyolithHealth = 0;
                uiRagnarosFloor = 0;
                uiRagnarosCache10 = 0;
                uiRagnarosCache25 = 0;
                uiVolcanusGUID = 0;
                uiTimer = 0;
                bEvent = false;
                bAchievCriteriaDoABarrelRollBrushfire = true;
                bAchievCriteriaDoABarrelRollLavaSpew = true;
                bAchievCriteriaDoABarrelRollIncendiaryCloud = true;
                bAchievCriteriaDoABarrelRollFieryTornado = true;
                creaturePortals.clear();
                gameobjectPortals.clear();
            }

            void BeforePlayerEnter(Player* pPlayer)
            {
                if (!uiTeamInInstance)
                    uiTeamInInstance = pPlayer->GetTeam();
            }

            void OnCreatureCreate(Creature* pCreature)
            {
                switch (pCreature->GetEntry())
                {
                    case NPC_SHANNOX:
                        uiShannoxGUID = pCreature->GetGUID();
                        break;
                    case NPC_RIPLIMB:
                        uiRiplimbGUID = pCreature->GetGUID();
                        break;
                    case NPC_RAGEFACE:
                        uiRagefaceGUID = pCreature->GetGUID();
                        break;
                    case NPC_CIRCLE_OF_THRONES_PORTAL:
                        creaturePortals.push_back(pCreature);
                        if (uiEvent == DONE)
                        {
                            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pCreature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                        }
                        break;
                    case NPC_RHYOLITH:
                        uiRhyolithGUID = pCreature->GetGUID();
                        break;
                    case NPC_RAGNAROS:
                        uiRagnarosGUID = pCreature->GetGUID();
                        break;
                    case NPC_VOLCANUS:
                        uiVolcanusGUID = pCreature->GetGUID();
                        if (uiEvent == DONE)
                        {
                            pCreature->SetVisible(true);
                            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        else
                        {
                            pCreature->SetVisible(false);
                            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* pGo)
            {
                switch (pGo->GetEntry())
                {
                    case GO_FIRE_WALL_BALEROC:
                    case GO_STICKY_WEB:
                    case GO_RAID_BRIDGE_FORMING:
                    case GO_BRIDGE_OF_RHYOLITH:
                    case GO_FIRE_WALL_FANDRAL_1:
                    case GO_FIRE_WALL_FANDRAL_2:
                    case GO_SULFURON_KEEP:
                        AddDoor(pGo, true);
                        break;
                    case GO_RAGNAROS_FLOOR:
                        uiRagnarosFloor = pGo->GetGUID();
                        break;
                    case GO_CIRCLE_OF_THORNS_PORTAL3:
                        gameobjectPortals.push_back(pGo);
                        if (uiEvent == DONE)
                            HandleGameObject(pGo->GetGUID(), true, pGo);
                        break;
                    case GO_CACHE_OF_THE_FIRELORD_10:
                        uiRagnarosCache10 = pGo->GetGUID();
                        break;
                    case GO_CACHE_OF_THE_FIRELORD_25:
                        uiRagnarosCache25 = pGo->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_RHYOLITH_HEALTH_SHARED:
                        uiRhyolithHealth = data;
                        break;
                    case DATA_EVENT:
                    {
                        uiEvent = data;
                        if (uiEvent == DONE)
                        {
                            if (!gameobjectPortals.empty())
                            {
                                for (std::list<GameObject*>::const_iterator itr = gameobjectPortals.begin(); itr != gameobjectPortals.end(); ++itr)
                                {
                                    if (GameObject* pGo = (*itr)->ToGameObject())
                                        HandleGameObject(pGo->GetGUID(), true, pGo);
                                }
                            }

                            if (!creaturePortals.empty())
                            {
                                for (std::list<Creature*>::const_iterator itr = creaturePortals.begin(); itr != creaturePortals.end(); ++itr)
                                {
                                    if (Creature* pCreature = (*itr)->ToCreature())
                                    {
                                        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                        pCreature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                                    }
                                }
                            }
                            if (Creature* volcanus = instance->GetCreature(uiVolcanusGUID))
                            {
                                volcanus->SetVisible(true);
                                volcanus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            }
                            SaveToDB();
                        }
                        break;
                    }
                    case DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_BRUSHFIRE:
                        if (data == 0)
                            bAchievCriteriaDoABarrelRollBrushfire = false;
                        else
                            bAchievCriteriaDoABarrelRollBrushfire = true;
                        break;
                    case DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_LAVA_SPEW:
                        if (data == 0)
                            bAchievCriteriaDoABarrelRollLavaSpew = false;
                        else
                            bAchievCriteriaDoABarrelRollBrushfire = true;
                        break;
                    case DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_INCENDIARY_CLOUD:
                        if (data == 0)
                            bAchievCriteriaDoABarrelRollIncendiaryCloud = false;
                        else
                            bAchievCriteriaDoABarrelRollBrushfire = true;
                        break;
                    case DATA_ACHIEV_CRITERIA_DO_A_BARREL_ROLL_FIERY_TORNADO:
                        if (data == 0)
                            bAchievCriteriaDoABarrelRollFieryTornado = false;
                        else
                            bAchievCriteriaDoABarrelRollBrushfire = true;
                        break;
                }
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_RHYOLITH_HEALTH_SHARED)
                    return uiRhyolithHealth;
                else if (type == DATA_EVENT)
                    return uiEvent;
                return 0;
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case DATA_SHANNOX: return uiShannoxGUID;
                    case DATA_RIPLIMB: return uiRiplimbGUID;
                    case DATA_RAGEFACE: return uiRagefaceGUID;
                    case DATA_RHYOLITH: return uiRhyolithGUID;
                    case DATA_RAGNAROS: return uiRagnarosGUID;
                    case DATA_RAGNAROS_FLOOR: return uiRagnarosFloor;
                    case DATA_RAGNAROS_CACHE_10: return uiRagnarosCache10;
                    case DATA_RAGNAROS_CACHE_25: return uiRagnarosCache25;
                    default: return 0;
                }
                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                 if (!InstanceScript::SetBossState(type, state))
                     return false;

                 return true;
            }

            bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* /*target*/, uint64 /*miscvalue1*/)
            {
                switch (criteria_id)
                {
                    case CRITERIA_DO_A_BARREL_ROLL_BRUSHFIRE:
                        return (source->isAlive() && bAchievCriteriaDoABarrelRollBrushfire);
                        break;
                    case CRITERIA_DO_A_BARREL_ROLL_LAVA_SPEW:
                        return (source->isAlive() && bAchievCriteriaDoABarrelRollLavaSpew);
                        break;
                    case CRITERIA_DO_A_BARREL_ROLL_INCENDIARY_CLOUD:
                        return (source->isAlive() && bAchievCriteriaDoABarrelRollIncendiaryCloud);
                        break;
                    case CRITERIA_DO_A_BARREL_ROLL_FIERY_TORNADO:
                        return (source->isAlive() && bAchievCriteriaDoABarrelRollFieryTornado);
                        break;
                    default:
                        break;
                }

                return false;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const
            {
                if (player && AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()))
                    return true;

                switch (bossId)
                {
                    case DATA_RAGNAROS:
                        if (GetBossState(DATA_STAGHELM) != DONE)
                            return false;
                        break;
                    case DATA_STAGHELM:
                        if (GetBossState(DATA_BALEROC) != DONE)
                            return false;
                        break;
                    case DATA_BALEROC:
                        if (GetBossState(DATA_SHANNOX) != DONE)
                            return false;
                        if (GetBossState(DATA_ALYSRAZOR) != DONE)
                            return false;
                        if (GetBossState(DATA_BETHTILAC) != DONE)
                            return false;
                        if (GetBossState(DATA_RHYOLITH) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            void ProcessEvent(WorldObject* source, uint32 eventId)
            {
                switch (eventId)
                {
                    case EVENT_PORTALS:
                        if ((uiEvent == DONE) || bEvent)
                            return;
                        bEvent = true;
                        uiTimer = 7000;
                        if (!creaturePortals.empty())
                        {
                            for (std::list<Creature*>::const_iterator itr = creaturePortals.begin(); itr != creaturePortals.end(); ++itr)
                            {
                                if (Creature* pCreature = (*itr)->ToCreature())
                                    pCreature->CastSpell(pCreature, SPELL_LEGENDARY_PORTAL_OPENING);

                                Player* player = source->ToPlayer();
                                if (player)
                                {
                                    player->DestroyItemCount(ITEM_CHARGED_CHITINOUS_FOCUS, 1, true);
                                    player->DestroyItemCount(ITEM_CHARGED_EMBERSTONE_FOCUS, 1, true);
                                    player->DestroyItemCount(ITEM_CHARGED_PYRESHELL_FOCUS, 1, true);
                                    player->DestroyItemCount(ITEM_CHARGED_RHYOLITE_FOCUS, 1, true);
                                }
                            }
                        }
                        break;
                }
            }

            void Update(uint32 diff)
            {
                if (bEvent)
                {
                    if (uiTimer <= diff)
                    {
                        bEvent = false;
                        SetData(DATA_EVENT, DONE);
                    }
                    else
                        uiTimer -= diff;
                }
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "F L " << GetBossSaveData() << uiEvent << ' ';

                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
            }

            void Load(const char* in)
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'F' && dataHead2 == 'L')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 tempEvent = 0;
                    loadStream >> tempEvent;
                    uiEvent = (tempEvent != DONE ? 0 : DONE);

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiTeamInInstance;
                uint32 uiRhyolithHealth;
                uint32 uiEvent;
                uint32 uiTimer;
                bool bEvent;
                bool bAchievCriteriaDoABarrelRollBrushfire;
                bool bAchievCriteriaDoABarrelRollLavaSpew;
                bool bAchievCriteriaDoABarrelRollIncendiaryCloud;
                bool bAchievCriteriaDoABarrelRollFieryTornado;
                uint64 uiShannoxGUID;
                uint64 uiRiplimbGUID;
                uint64 uiRagefaceGUID;
                uint64 uiRagnarosGUID;
                uint64 uiRhyolithGUID;
                uint64 uiRagnarosFloor;
                uint64 uiRagnarosCache10;
                uint64 uiRagnarosCache25;
                uint64 uiVolcanusGUID;
                std::list<GameObject*> gameobjectPortals;
                std::list<Creature*> creaturePortals;
        };
};

#ifndef __clang_analyzer__
void AddSC_instance_firelands()
{
    new instance_firelands();
}
#endif
