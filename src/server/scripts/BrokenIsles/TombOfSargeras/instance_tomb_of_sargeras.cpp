////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "Creature.h"
#include "tomb_of_sargeras.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Map.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "LFGMgr.h"
#include "Group.h"

DoorData const g_DoorData[] =
{
    { eGameObjects::GoDoorRoomGorothS,          eData::DataGoroth,              DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoDoorRoomGorothW,          eData::DataGoroth,              DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { eGameObjects::GoDoorRoomHarjatanS,        eData::DataHarjatan,            DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoDoorRoomHarjatanW,        eData::DataHarjatan,            DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { eGameObjects::GoDoorRoomSasszineS,        eData::DataMistressSasszine,    DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoGolganethTidePillarPlace, eData::DataMistressSasszine,    DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { eGameObjects::GoDoorRoomInquisition,      eData::DataDemonicInquisition,  DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoKhazgorothPillarPlace,    eData::DataDemonicInquisition,  DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { eGameObjects::GoDoorSistersOfTheMoon,     eData::DataSistersOfTheMoon,    DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoDoorPassageDesolateHost,  eData::DataSistersOfTheMoon,    DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { eGameObjects::GoDoorDesolateHost,         eData::DataDesolateHost,        DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoTearsOfElunePillarPlace,  eData::DataDesolateHost,        DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { eGameObjects::GoFelstormCollision,        eData::DataMaidenOfVigilance,   DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { eGameObjects::GoDoorPreFallenAvatar,      eData::DataFallenAvatar,        DOOR_TYPE_ROOM,     BOUNDARY_NONE },
    { 0,                                        0,                              DOOR_TYPE_ROOM,     BOUNDARY_NONE }
};

using ObjectInfo = std::pair<uint32, uint64>;

class instance_tomb_of_sargeras : public InstanceMapScript
{
    public:
        instance_tomb_of_sargeras() : InstanceMapScript(TOSScriptName, 1676) { }

        struct instance_tomb_of_sargeras_InstanceMapScript : public InstanceScript
        {
            instance_tomb_of_sargeras_InstanceMapScript(Map* map) : InstanceScript(map) { }

            void Initialize() override
            {
                GarrothGuid = 0;
                HarjatanGuid = 0;
                MistressSasszineGuid = 0;
                SetBossNumber(eData::MaxEncounters);
                LoadDoorData(g_DoorData);
                NoBossAtriganGUID = 0;

                for (uint8 i = 0; i < 5; ++i)
                     m_NpcPreGorothGUID[i] = 0;

                for (uint8 l = 0; l < 4; ++l)
                     GoDoorPreSistersOfTheMoonGUID[l] = 0;

                for(uint8 j = 0; j < 3; ++j)
                    m_uiDialogs[j] = NOT_STARTED;

                NoBossMistressSasszineGuid = 0;

                for(uint8 k = 0; k < 2; ++k)
                {
                    GoCrashingCometGUID[k] = 0;
                    GoDoorRoomGorothGUID[k] = 0;
                    GoDoorPreSistersOfTheMoonControlUnitGUID[k] = 0;
                }

                DoorBeforeInquisitionGuid = 0;
                GoDoorPreSistersOfTheMoonMechanismGUID = 0;
            }

            uint64 GarrothGuid;
            uint64 HarjatanGuid;
            uint64 MistressSasszineGuid;
            uint32 m_uiDialogs[3];
            uint64 m_NpcPreGorothGUID[5];
            uint64 NoBossAtriganGUID;
            uint64 NoBossMistressSasszineGuid;
            uint64 GoCrashingCometGUID[2];
            uint64 BossAtriganGUID;
            uint64 BossBelacGUID;
            uint64 DoorBeforeInquisitionGuid;
            uint64 GoDoorPreSistersOfTheMoonGUID[4];
            uint64 GoDoorRoomGorothGUID[2];
            uint64 GoDoorPreSistersOfTheMoonMechanismGUID;
            uint64 GoDoorPreSistersOfTheMoonControlUnitGUID[2];

            uint64 m_FallenAvatarElevator = 0;

            uint64 m_IxallonGuid = 0;
            uint64 m_KilJaedenGuid = 0;
            uint64 m_VelenGuid = 0;
            uint64 m_IllidanStormrage = 0;
            uint64 m_SkyboxGuid = 0;

            uint64 m_BridgeSouthGuid = 0;
            uint64 m_BridgeNorthGuid = 0;

            uint32 m_DungeonID = 0;

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::NpcAegwynnTrap:
                    {
                        p_Creature->CastSpell(p_Creature, eSharedSpells::SpellAegwynnTraps, true);
                        break;
                    }
                    case eCreatures::NpcMageKadgar:
                         m_NpcPreGorothGUID[0] = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcIllidari:
                         m_NpcPreGorothGUID[1] = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcMaievShadowsong:
                        m_NpcPreGorothGUID[2] = p_Creature->GetGUID();
                        break;
                    case eCreatures::NpcBreach:
                         m_NpcPreGorothGUID[3] = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcProphetVelen:
                         m_NpcPreGorothGUID[4] = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcEventAtrigan:
                         NoBossAtriganGUID = p_Creature->GetGUID();
                         break;
                    case NpcEventMistressSasszine: /// no boss
                         NoBossMistressSasszineGuid = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcGoroth:
                         GarrothGuid = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcHarjatan:
                         HarjatanGuid = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcMistressSasszine:
                         MistressSasszineGuid = p_Creature->GetGUID();
                         break;
                    case eCreatures::BossHuntressKasparian:
                    case eCreatures::BossPriestessLunaspyre:
                    case eCreatures::BossCaptainYathaeMoonStrike:
                    {
                        AddToDamageManager(p_Creature, 0);
                        m_ObjectGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }
                    case eCreatures::BossEngineOfSouls:
                    case eCreatures::BossSoulQueenDejahna:
                    {
                        m_ObjectGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        AddToDamageManager(p_Creature, 1);
                        break;
                    }
                    case eCreatures::NpcImageOfAegwynn:
                    case eCreatures::BossDesolateHost:
                    {
                        m_ObjectGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }
                    case eCreatures::NpcAtrigan:
                         BossAtriganGUID = p_Creature->GetGUID();
                         break;
                    case eCreatures::NpcBelac:
                         BossBelacGUID = p_Creature->GetGUID();
                         break;
                    case eCreatures::BossKilJaeden:
                    {
                        m_KilJaedenGuid = p_Creature->GetGUID();
                        p_Creature->setActive(true);
                        p_Creature->SetBig(true);
                        break;
                    }
                    case eCreatures::BossProphetVelen:
                    {
                        m_VelenGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcIllidanStormrage:
                    {
                        m_IllidanStormrage = p_Creature->GetGUID();
                        break;
                    }

                    case eCreatures::NpcDurganStonestorm:
                    case eCreatures::NpcWindRushTotem:
                    {
                        m_ObjectGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));

                        if (GetBossState(eData::DataGoroth) == EncounterState::DONE)
                            p_Creature->SetVisible(true);
                        else
                            p_Creature->SetVisible(false);

                        break;
                    }

                    case eCreatures::NpcTidestoneGolganethImage:
                    case eCreatures::NpcTearsOfEluneImage:
                    case eCreatures::NpcHammerKhazgorothImage:
                    case eCreatures::NpcAegisOfAggramarImage:
                    {
                        m_ObjectGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        break;
                    }
                    case eCreatures::NpcTidestoneofGolganneth:
                    case eCreatures::NpcHammerOfKhazgoroth:
                    case eCreatures::NpcTearsOfElune:
                    {
                        m_ObjectGuids.push_back(ObjectInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                        p_Creature->SetVisible(false);
                        break;
                    }
                    case eCreatures::NpcIncubatedEgg:
                    case eCreatures::NpcMythicEggController:
                    {
                        if (!instance->IsMythic())
                            p_Creature->SetVisible(false);
                        break;
                    }
                    case eCreatures::NpcIxallonTheSoulbreaker:
                    {
                        m_IxallonGuid = p_Creature->GetGUID();
                        p_Creature->setActive(true);
                        p_Creature->SetBig(true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* p_Creature) override
            {
                uint32 l_Entry = p_Creature->GetEntry();

                switch (l_Entry)
                {
                    case eCreatures::BossCaptainYathaeMoonStrike:
                    case eCreatures::BossHuntressKasparian:
                    case eCreatures::BossPriestessLunaspyre:
                    {
                        RemoveFromDamageManager(p_Creature, 0);
                        break;
                    }
                    case eCreatures::BossEngineOfSouls:
                    case eCreatures::BossSoulQueenDejahna:
                    {
                        RemoveFromDamageManager(p_Creature, 1);
                        break;
                    }
                    default:
                        break;
                }

                auto l_Itr = std::find_if(m_ObjectGuids.begin(), m_ObjectGuids.end(), [&l_Entry] (ObjectInfo const & p_Itr)
                {
                    return p_Itr.first == l_Entry;
                });

                if (l_Itr != m_ObjectGuids.end())
                    m_ObjectGuids.erase(l_Itr);
            }

            void OnGameObjectCreate(GameObject* p_Go) override
            {
                auto l_Itr = std::find_if(std::begin(g_DoorData), std::end(g_DoorData), [&p_Go](const DoorData & p_Itr)
                {
                    return p_Itr.entry == p_Go->GetEntry();
                });

                if (l_Itr != std::end(g_DoorData))
                    InstanceScript::AddDoor(p_Go, true);

                switch (p_Go->GetEntry())
                {
                    case eGameObjects::GoCrashingComet:
                    {
                        GoCrashingCometGUID[0] = p_Go->GetGUID();

                        if (m_uiDialogs[0] == DONE)
                            p_Go->Delete();

                        break;
                    }
                    case eGameObjects::GoCrashingComet2:
                    {
                        GoCrashingCometGUID[1] = p_Go->GetGUID();

                        if (m_uiDialogs[0] == DONE)
                            p_Go->Delete();

                        break;
                    }
                    case eGameObjects::GoKJSkybox:
                    {
                        m_SkyboxGuid = p_Go->GetGUID();
                        break;
                    }
                    case eGameObjects::GoDoorBeforeInquisition:
                    {
                        DoorBeforeInquisitionGuid = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorPreSistersOfTheMoon:
                    {
                        GoDoorPreSistersOfTheMoonGUID[0] = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorPreSistersOfTheMoon2:
                    {
                        GoDoorPreSistersOfTheMoonGUID[1] = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorPreSistersOfTheMoon3:
                    {
                        GoDoorPreSistersOfTheMoonMechanismGUID = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        break;
                    }
                    case eGameObjects::GoDoorPreSistersOfTheMoon4:
                    {
                        GoDoorPreSistersOfTheMoonGUID[2] = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorPreSistersOfTheMoon5:
                    {
                        GoDoorPreSistersOfTheMoonGUID[3] = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorRoomGorothN:
                    {
                        GoDoorRoomGorothGUID[0] = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorRoomGorothE:
                    {
                        GoDoorRoomGorothGUID[1] = p_Go->GetGUID();
                        p_Go->SetGoState(GO_STATE_READY);

                        if (GetBossState(eData::DataGoroth) == DONE && (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone))
                            p_Go->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoDoorBeforeMaidenRoom:
                    {
                        m_ObjectGuids.push_back(ObjectInfo(p_Go->GetEntry(), p_Go->GetGUID()));

                        if (GetBossState(eData::DataDesolateHost) == EncounterState::DONE &&
                            GetBossState(eData::DataDemonicInquisition) == EncounterState::DONE &&
                            GetBossState(eData::DataMistressSasszine) == EncounterState::DONE)
                            HandleGameObject(0, true, p_Go);
                        else
                            HandleGameObject(0, false, p_Go);

                        break;
                    }
                    case eGameObjects::GoElevatorToFallenAvatar:
                    {
                        /// Sniffed values
                        ///p_Go->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_LEVEL, 579331687);
                        ///p_Go->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_PERCENT_HEALTH, 4278192920);

                        m_FallenAvatarElevator = p_Go->GetGUID();
                        break;
                    }
                    case eGameObjects::GoBridgeNorth:
                    {
                        m_BridgeNorthGuid = p_Go->GetGUID();
                        break;
                    }
                    case eGameObjects::GoBridgeSouth:
                    {
                        m_BridgeSouthGuid = p_Go->GetGUID();
                        break;
                    }
                    case GoDoorPreSistersOfTheMoonControlUnit:
                    {
                        GoDoorPreSistersOfTheMoonControlUnitGUID[0] = p_Go->GetGUID();
                        p_Go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        break;
                    }
                    case GoDoorPreSistersOfTheMoonControlUnit2:
                    {
                        GoDoorPreSistersOfTheMoonControlUnitGUID[1] = p_Go->GetGUID();
                        p_Go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                auto l_Itr = std::find_if(std::begin(g_DoorData), std::end(g_DoorData), [&p_GameObject](const DoorData & p_Itr)
                {
                    return p_Itr.entry == p_GameObject->GetEntry();
                });

                if (l_Itr != std::end(g_DoorData))
                    InstanceScript::AddDoor(p_GameObject, false);
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::BossCaptainYathaeMoonStrike:
                    case eCreatures::BossHuntressKasparian:
                    case eCreatures::BossPriestessLunaspyre:
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            SetBossState(eData::DataSistersOfTheMoon, EncounterState::DONE);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            bool IsInteractingPossible(Unit const* p_Caster, Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (GetBossState(eData::DataDesolateHost) != EncounterState::IN_PROGRESS)
                    return true;

                if (p_Caster == p_Target)
                    return true;

                if (!p_Caster->IsPlayer())
                    return true;

                if (p_Caster->HasAura(eSharedSpells::SpellSpiritRealm))
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);

                return true;
            }

            bool IsValidEncounter(EncounterCreditType p_Type, Unit* p_Source) const override
            {
                if (instance->IsLFR())
                    return true;

                if (p_Source->GetEntry() == eCreatures::BossEngineOfSouls)
                {
                    Creature* l_Host = p_Source->FindNearestCreature(eCreatures::BossDesolateHost, 250.f, false);

                    return l_Host != nullptr;
                }

                return true;
            }

            uint64 GetData64(uint32 p_Data) override
            {
                switch (p_Data)
                {
                    case eData::DataMageKadgar:
                         return m_NpcPreGorothGUID[0];
                    case eData::DataIllidari:
                         return m_NpcPreGorothGUID[1];
                    case eData::DataMaievShadowsong:
                         return m_NpcPreGorothGUID[2];
                    case eData::DataBreach:
                         return m_NpcPreGorothGUID[3];
                    case eData::DataProphetVelen:
                         return m_NpcPreGorothGUID[4];
                    case eData::DataNoBossMistressSasszine:
                         return NoBossMistressSasszineGuid;
                    case eData::DataGoroth:
                         return GarrothGuid;
                    case eData::DataHarjatan:
                         return HarjatanGuid;
                    case eData::DataMistressSasszine:
                         return MistressSasszineGuid;
                    case eData::DataNoBossAtrigan:
                         return NoBossAtriganGUID;
                    case eCreatures::NpcAtrigan:
                         return BossAtriganGUID;
                    case eCreatures::NpcBelac:
                         return BossBelacGUID;
                    case eCreatures::BossKilJaeden:
                        return m_KilJaedenGuid;
                    case eCreatures::BossProphetVelen:
                        return m_VelenGuid;
                    case eCreatures::NpcIllidanStormrage:
                        return m_IllidanStormrage;
                    case eGameObjects::GoKJSkybox:
                        return m_SkyboxGuid;
                    case eCreatures::NpcIxallonTheSoulbreaker:
                        return m_IxallonGuid;
                    case eGameObjects::GoBridgeNorth:
                        return m_BridgeNorthGuid;
                    case eGameObjects::GoBridgeSouth:
                        return m_BridgeSouthGuid;
                    case eGameObjects::GoDoorPreSistersOfTheMoon3:
                        return GoDoorPreSistersOfTheMoonMechanismGUID;
                    default:
                        break;
                }

                auto l_Itr = std::find_if(m_ObjectGuids.begin(), m_ObjectGuids.end(), [p_Data] (ObjectInfo const & p_Itr) -> bool
                {
                    return p_Itr.first == p_Data;
                });

                if (l_Itr != m_ObjectGuids.end())
                    return l_Itr->second;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case eData::DataPreEventGoroth:
                    {
                         m_uiDialogs[0] = data;
                         if (data == DONE)
                             SaveToDB();
                         break;
                    }
                    case eData::DataPreEventHarjatan:
                    {
                         m_uiDialogs[1] = data;
                         if (data == DONE)
                             SaveToDB();
                         break;
                    }
                    case eData::DataPreEventDemonicInquisition:
                    {
                         m_uiDialogs[2] = data;
                         if (data == DONE)
                             SaveToDB();
                         break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) override
            {
                switch (type)
                {
                    case eData::DataPreEventGoroth:
                         return m_uiDialogs[0];
                    case eData::DataPreEventHarjatan:
                         return m_uiDialogs[1];
                    case eData::DataPreEventDemonicInquisition:
                         return m_uiDialogs[2];
                    case eData::DataDungeonID:
                        return m_DungeonID;
                    default:
                        break;
                }

                return 0;
            }

            void ActivatePillarOfCreation(uint32 const p_Entry)
            {
                Creature* l_Pillar = instance->GetCreature(GetData64(p_Entry));

                if (l_Pillar == nullptr)
                    return;

                l_Pillar->SetVisible(true);
            }

            void ActivatePillarOfCreationImage(uint32 const p_Entry)
            {
                Creature* l_Pillar = instance->GetCreature(GetData64(p_Entry));

                if (l_Pillar == nullptr)
                    return;

                l_Pillar->RemoveAllAuras();
            }

            bool SetBossState(uint32 p_Type, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Type, p_State))
                    return false;

                if (p_State == IN_PROGRESS)
                    DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellCurseOfGuldan);

                switch (p_Type)
                {
                    case eData::DataMistressSasszine:
                    {
                        if (p_State == DONE)
                        {
                            ActivatePillarOfCreation(eCreatures::NpcTidestoneofGolganneth);
                            ActivatePillarOfCreationImage(eCreatures::NpcTidestoneGolganethImage);
                        }

                        break;
                    }
                    case eData::DataGoroth:
                    {
                         if (p_State == DONE)
                         {
                             if (Creature* mageKadgar = instance->GetCreature(m_NpcPreGorothGUID[0]))
                             {
                                 mageKadgar->RemoveAurasDueToSpell(235453);
                                 mageKadgar->SetSpeed(UnitMoveType::MOVE_RUN, 1.85714f, true);
                                 mageKadgar->SetSpeed(UnitMoveType::MOVE_WALK, 3.2f, true);
                                 mageKadgar->GetMotionMaster()->MovePoint(1, PositionTeleKadgar[1], true);
                             }

                             if (Creature* Illidari = instance->GetCreature(m_NpcPreGorothGUID[1]))
                                 Illidari->GetMotionMaster()->MovePoint(1, PositionTeleIllidari[1], true);

                             if (Creature* Maiev = instance->GetCreature(m_NpcPreGorothGUID[2]))
                                 Maiev->GetMotionMaster()->MovePoint(1, PositionTeleMaiev[1], true);

                             if (Creature* Velen = instance->GetCreature(m_NpcPreGorothGUID[4]))
                             {
                                 Velen->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                                 Velen->GetMotionMaster()->MovePoint(1, PositionTeleProphetVelen[1], true);
                             }

                             if (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone) ///< Non LFR
                             {
                                 if (GameObject* l_DoorBeforeInquisition = instance->GetGameObject(DoorBeforeInquisitionGuid))
                                      l_DoorBeforeInquisition->SetGoState(GO_STATE_ACTIVE);

                                 for (uint8 j = 0; j < 4; ++j)
                                      if (GameObject* l_DoorPreSistersOfTheMoon = instance->GetGameObject(GoDoorPreSistersOfTheMoonGUID[j]))
                                          l_DoorPreSistersOfTheMoon->SetGoState(GO_STATE_ACTIVE);

                                 for (uint8 k = 0; k < 2; ++k)
                                 {
                                     if (GameObject* l_GoDoorRoomGoroth = instance->GetGameObject(GoDoorRoomGorothGUID[k]))
                                         l_GoDoorRoomGoroth->SetGoState(GO_STATE_ACTIVE);
                                 }
                             }

                             if (Creature* l_Durgan = instance->GetCreature(GetData64(eCreatures::NpcDurganStonestorm)))
                                 l_Durgan->SetVisible(true);

                             if (Creature* l_Totem = instance->GetCreature(GetData64(eCreatures::NpcWindRushTotem)))
                             {
                                 l_Totem->SetVisible(true);
                                 l_Totem->CastSpell(l_Totem, eSharedSpells::SpellWindRushTotem, true);
                             }
                         }
                         break;
                    }
                    case eData::DataDemonicInquisition:
                    {
                        if (p_State == DONE)
                        {
                            ActivatePillarOfCreation(eCreatures::NpcHammerOfKhazgoroth);
                            ActivatePillarOfCreationImage(eCreatures::NpcHammerKhazgorothImage);
                        }

                        if (p_State == DONE && instance->IsLFR() && m_DungeonID == eDungeonIDs::DungeonIDWailingHalls) ///< Special for LFR second wings
                        {
                             if (GameObject* l_DoorBeforeInquisition = instance->GetGameObject(DoorBeforeInquisitionGuid))
                                 l_DoorBeforeInquisition->SetGoState(GO_STATE_ACTIVE);

                             for (uint8 j = 0; j < 4; ++j)
                                  if (GameObject* l_DoorPreSistersOfTheMoon = instance->GetGameObject(GoDoorPreSistersOfTheMoonGUID[j]))
                                      l_DoorPreSistersOfTheMoon->SetGoState(GO_STATE_ACTIVE);

                             if (GameObject* l_DoorPreSistersOfTheMoon3 = instance->GetGameObject(GoDoorPreSistersOfTheMoonMechanismGUID))
                                 l_DoorPreSistersOfTheMoon3->SetGoState(GO_STATE_ACTIVE);

                             for (uint8 k = 0; k < 2; ++k)
                             {
                                 if (GameObject* l_GoDoorRoomGoroth = instance->GetGameObject(GoDoorRoomGorothGUID[k]))
                                     l_GoDoorRoomGoroth->SetGoState(GO_STATE_ACTIVE);
                             }
                        }
                        break;
                    }

                    case eData::DataDesolateHost:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            ActivatePillarOfCreation(eCreatures::NpcTearsOfElune);
                            ActivatePillarOfCreationImage(eCreatures::NpcTearsOfEluneImage);
                        }

                        if (p_State == EncounterState::DONE ||
                            p_State == EncounterState::FAIL)
                        {
                            DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellSpiritRealm);
                            DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellCorporealRealm);
                            DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellSpiritRealmVisual);
                        }

                        break;
                    }
                    case eData::DataMaidenOfVigilance:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            /// Start Elevator
                            ///if (GameObject* l_Elevator = instance->GetGameObject(m_FallenAvatarElevator))
                            ///{
                            ///    /// Sniffed values
                            ///    l_Elevator->SetUInt32Value(EObjectFields::OBJECT_FIELD_DYNAMIC_FLAGS, 12255232);
                            ///    l_Elevator->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_LEVEL, 581592989);
                            ///    l_Elevator->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_PERCENT_HEALTH, 4278192922);
                            ///}
                        }

                        break;
                    }
                    case eData::DataKiljaeden:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID());
                            if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) != ItemContext::None)
                                l_DisabledLoots = false;

                            if (!l_DisabledLoots)
                                l_DisabledLoots = sObjectMgr->IsDisabledEncounter(eEncounters::EncounterKilJaeden, instance->GetDifficultyID());

                            if (!l_DisabledLoots)
                            {
                                if (instance->IsHeroicOrMythic())
                                    DoCompleteAchievement(eAchievements::AheadOfTheCurveKilJaeden);

                                if (instance->IsMythic())
                                    DoCompleteAchievement(eAchievements::CuttingEdgeKilJaeden);

                                instance->SummonGameObject(eGameObjects::GoKJChest, { -254.316f, 5950.06f, 165.683f, 5.57026f });
                            }

                            DoCastSpellOnPlayers(eSharedSpells::SpellArgusFilter);
                        }

                        break;
                    }
                    case eData::DataHarjatan:
                    {
                         if (p_State == EncounterState::DONE)
                         {
                             if (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone) ///< Non LFR
                             {
                                 for (uint8 j = 0; j < 2; ++j)
                                      if (GameObject* l_DoorsControlUnit = instance->GetGameObject(GoDoorPreSistersOfTheMoonControlUnitGUID[j]))
                                          l_DoorsControlUnit->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                             }
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

                /// Don't need to check bosses for LFR
                if (instance->IsLFR())
                    return true;

                switch (p_BossID)
                {
                    case eData::DataMaidenOfVigilance:
                    {
                        if (GetBossState(eData::DataDemonicInquisition) != EncounterState::DONE ||
                            GetBossState(eData::DataHarjatan)           != EncounterState::DONE ||
                            GetBossState(eData::DataMistressSasszine)   != EncounterState::DONE ||
                            GetBossState(eData::DataSistersOfTheMoon)   != EncounterState::DONE ||
                            GetBossState(eData::DataDesolateHost)       != EncounterState::DONE)
                            return false;

                        break;
                    }
                    case eData::DataFallenAvatar:
                    case eData::DataKiljaeden:
                    {
                        if (GetBossState(p_BossID - 1) != EncounterState::DONE)
                            return false;

                        break;
                    }
                    default:
                        break;
                }

                return true;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                p_Player->SetPhaseMask(ePhasesMasks::PhaseCorporealRealm, true);

                if (m_DungeonID == eDungeonIDs::DungeonIDNone && instance->IsLFR())
                {
                    m_DungeonID = (p_Player->GetGroup() ? sLFGMgr->GetDungeon(p_Player->GetGroup()->GetGUID()) : 0);

                    if (m_DungeonID != eDungeonIDs::DungeonIDTheGatesofHell)
                    {
                        if (Creature* l_Goroth = instance->GetCreature(GarrothGuid))
                        {
                            l_Goroth->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            l_Goroth->SetVisible(false);
                        }

                        if (Creature* l_NoBossAtrigan = instance->GetCreature(NoBossAtriganGUID))
                           l_NoBossAtrigan->SetVisible(false);
                    }

                    if (m_DungeonID == eDungeonIDs::DungeonIDWailingHalls && GetBossState(eData::DataDemonicInquisition) == DONE) ///< Special for LFR second wings
                    {
                         if (GameObject* l_DoorBeforeInquisition = instance->GetGameObject(DoorBeforeInquisitionGuid))
                             l_DoorBeforeInquisition->SetGoState(GO_STATE_ACTIVE);

                         for (uint8 j = 0; j < 4; ++j)
                              if (GameObject* l_DoorPreSistersOfTheMoon = instance->GetGameObject(GoDoorPreSistersOfTheMoonGUID[j]))
                                  l_DoorPreSistersOfTheMoon->SetGoState(GO_STATE_ACTIVE);

                         if (GameObject* l_DoorPreSistersOfTheMoon3 = instance->GetGameObject(GoDoorPreSistersOfTheMoonMechanismGUID))
                             l_DoorPreSistersOfTheMoon3->SetGoState(GO_STATE_ACTIVE);

                         for (uint8 k = 0; k < 2; ++k)
                         {
                             if (GameObject* l_GoDoorRoomGoroth = instance->GetGameObject(GoDoorRoomGorothGUID[k]))
                                 l_GoDoorRoomGoroth->SetGoState(GO_STATE_ACTIVE);
                         }
                    }
                }
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->IsFalling() && l_Player->isAlive())
                        {
                            if (l_Player->m_positionZ < 2886.0f)
                            {
                                switch (GetBossState(eData::DataMaidenOfVigilance))
                                {
                                    /// After Maiden of Vigilance, put a safe fall buff on falling players
                                    case EncounterState::DONE:
                                    {
                                        if (l_Player->GetExactDist(&g_MaidenCenterPos) >= 15.0f && l_Player->GetExactDist(&g_MaidenCenterPos) < 50.0f && l_Player->IsFalling())
                                            l_Player->CastSpell(l_Player, eSharedSpells::SpellSafeFall30s, true);

                                        break;
                                    }
                                    /// Before and during Maiden of Vigilance, kill players after falling further away than 95 yards
                                    default:
                                    {
                                        if (l_Player->GetExactDist(&g_MaidenCenterPos) >= 95.0f && l_Player->GetExactDist(&g_MaidenCenterPos) < 150.0f)
                                            l_Player->Kill(l_Player);

                                        break;
                                    }
                                }

                                /// Sister Dead Zone
                                float l_Dist = l_Player->GetDistance2d(g_SistersDeadZoneCenter.GetPositionX(), g_SistersDeadZoneCenter.GetPositionY());

                                if (l_Dist < 55.f && l_Player->GetPositionZ() < 2880.f)
                                {
                                    l_Player->Kill(l_Player);
                                    return;
                                }

                                /// Desolate Dead Zone
                                l_Dist = l_Player->GetDistance2d(g_DesolateCenterRoom.GetPositionX(), g_DesolateCenterRoom.GetPositionY());

                                if (l_Dist <= 68.f && l_Player->GetPositionZ() < 2870.f)
                                {
                                    l_Player->Kill(l_Player);
                                    return;
                                }
                            }
                            else if (l_Player->m_positionZ < 5335.0f && l_Player->GetExactDist(&g_KilJaedenCenterPos) < 200.0f)
                                l_Player->Kill(l_Player);
                        }
                    }
                }
            }

            std::string GetDialogSaveData()
            {
                std::ostringstream saveStream;
                for (uint8 i = 0; i < 3; i++)
                    saveStream << (uint32)m_uiDialogs[i] << " ";
                return saveStream.str();
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "T S " << GetBossSaveData() << GetDialogSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* strIn) override
            {
                if (!strIn)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(strIn);

                char dataHead1, dataHead2;

                std::istringstream loadStream(strIn);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'T' && dataHead2 == 'S')
                {
                    for (uint8 i = 0; i < eData::MaxEncounters; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    for (uint8 j = 0; j < 3; ++j)
                    {
                        uint32 tmpEvent;
                        loadStream >> tmpEvent;
                        if (tmpEvent == IN_PROGRESS || tmpEvent > SPECIAL)
                            tmpEvent = NOT_STARTED;

                        m_uiDialogs[j] = tmpEvent;
                    }

                    if (m_uiDialogs[0] == DONE)
                    {
                        for (uint8 k = 0; k < 2; ++k)
                        {
                            if (GameObject* crashingComet = instance->GetGameObject(GoCrashingCometGUID[k]))
                                crashingComet->Delete();
                        }

                        if (Creature* mageKadgar = instance->GetCreature(m_NpcPreGorothGUID[0]))
                        {
                            mageKadgar->RemoveAurasDueToSpell(235453);
                            if (GetBossState(eData::DataGoroth) != DONE)
                                mageKadgar->CastSpell(PositionTeleKadgar[0], 241303, true);
                            else
                                mageKadgar->CastSpell(PositionTeleKadgar[1], 241303, true);
                        }

                        if (Creature* Illidari = instance->GetCreature(m_NpcPreGorothGUID[1]))
                        {
                            if (GetBossState(eData::DataGoroth) != DONE)
                                Illidari->CastSpell(PositionTeleIllidari[0], 241303, true);
                            else
                                Illidari->CastSpell(PositionTeleIllidari[1], 241303, true);
                        }

                        if (Creature* Maiev = instance->GetCreature(m_NpcPreGorothGUID[2]))
                        {
                            if (GetBossState(eData::DataGoroth) != DONE)
                                Maiev->CastSpell(PositionTeleMaiev[0], 241303, true);
                            else
                                Maiev->CastSpell(PositionTeleMaiev[1], 241303, true);
                        }

                        if (Creature* Velen = instance->GetCreature(m_NpcPreGorothGUID[4]))
                        {
                            Velen->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                            if (GetBossState(eData::DataGoroth) != DONE)
                                Velen->CastSpell(PositionTeleProphetVelen[0], 241303, true);
                            else
                                Velen->CastSpell(PositionTeleProphetVelen[1], 241303, true);
                        }

                        if (Creature* NoBossAtrigan = instance->GetCreature(NoBossAtriganGUID))
                             NoBossAtrigan->SetVisible(false);
                    }

                    if (m_uiDialogs[1] == DONE)
                    {
                        if (Creature* NoBossMistressSasszine = instance->GetCreature(NoBossMistressSasszineGuid))
                            NoBossMistressSasszine->SetVisible(false);
                    }

                    if (GetBossState(eData::DataGoroth) == DONE)
                    {
                       if (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone) ///< Non LFR
                       {
                           if (GameObject* l_DoorBeforeInquisition = instance->GetGameObject(DoorBeforeInquisitionGuid))
                                l_DoorBeforeInquisition->SetGoState(GO_STATE_ACTIVE);

                           for (uint8 j = 0; j < 4; ++j)
                                if (GameObject* l_DoorPreSistersOfTheMoon = instance->GetGameObject(GoDoorPreSistersOfTheMoonGUID[j]))
                                    l_DoorPreSistersOfTheMoon->SetGoState(GO_STATE_ACTIVE);

                           for (uint8 k = 0; k < 2; ++k)
                           {
                               if (GameObject* l_GoDoorRoomGoroth = instance->GetGameObject(GoDoorRoomGorothGUID[k]))
                                   l_GoDoorRoomGoroth->SetGoState(GO_STATE_ACTIVE);
                           }
                       }
                    }

                    if (GetBossState(eData::DataHarjatan) == DONE)
                    {
                       if (!instance->IsLFR() || m_DungeonID == eDungeonIDs::DungeonIDNone) ///< Non LFR
                       {
                           for (uint8 j = 0; j < 2; ++j)
                                if (GameObject* l_DoorsControlUnit = instance->GetGameObject(GoDoorPreSistersOfTheMoonControlUnitGUID[j]))
                                    l_DoorsControlUnit->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                       }
                    }
                }

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                switch (m_DungeonID)
                {
                    case eDungeonIDs::DungeonIDWailingHalls:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationWailingHalls);
                        break;
                    }
                    case eDungeonIDs::DungeonIDDeceiversFall:
                    {
                        if (GetBossState(eData::DataKiljaeden) == EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAfterKJDeath);
                        else
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationKJSpaceship);

                        break;
                    }
                    case eDungeonIDs::DungeonIDChamberOfTheAvatar:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationChamberOfTheAvatar);
                        break;
                    }
                    default:
                    {
                        if (GetBossState(eData::DataKiljaeden) == EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAfterKJDeath);
                        else if (GetBossState(eData::DataFallenAvatar) == EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationKJSpaceship);
                        else if (GetBossState(eData::DataMaidenOfVigilance) == EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAvatarChamber);
                        else if (GetBossState(eData::DataGoroth) == EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationChamberOfMoon);
                        else
                            l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationRaidEntrance);

                        break;
                    }
                }

                if (!l_Loc)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            private:
                std::vector<ObjectInfo> m_ObjectGuids;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_tomb_of_sargeras_InstanceMapScript(map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_tomb_of_sargeras()
{
    new instance_tomb_of_sargeras();
}
#endif
