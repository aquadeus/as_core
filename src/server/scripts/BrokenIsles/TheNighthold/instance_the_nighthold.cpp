////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

DoorData const g_DoorData[] =
{
    { eGameObjects::GoSkorpyronDoor1,           eData::DataSkorpyron,           DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoSkorpyronDoor2,           eData::DataSkorpyron,           DoorType::DOOR_TYPE_PASSAGE,      BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoAnomalyDoor1,             eData::DataChronomaticAnomaly,  DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoAnomalyDoor2,             eData::DataChronomaticAnomaly,  DoorType::DOOR_TYPE_PASSAGE,      BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoTrilliaxDoor1,            eData::DataTrilliax,            DoorType::DOOR_TYPE_PASSAGE,      BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoTrilliaxDoor2,            eData::DataTrilliax,            DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoAstromancerDoor,          eData::DataStarAugurEtraeus,    DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoBotanistDoorRight,        eData::DataHighBotanistTelarn,  DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoBotanistDoorLeft,         eData::DataHighBotanistTelarn,  DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoKrosusTeleporterDoor,     eData::DataKrosus,              DoorType::DOOR_TYPE_PASSAGE,      BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoElisandeDoorNE,           eData::DataElisande,            DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoElisandeDoorNW,           eData::DataElisande,            DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoElisandeDoorSW,           eData::DataElisande,            DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
    { 0,                                        0,                              DoorType::DOOR_TYPE_ROOM,         BoundaryType::BOUNDARY_NONE },
};

class instance_the_nighthold : public InstanceMapScript
{
    public:
        instance_the_nighthold() : InstanceMapScript("instance_the_nighthold", 1530) { }

        struct instance_the_nighthold_InstanceMapScript : InstanceScript
        {
            instance_the_nighthold_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(eData::MaxEncounters);
            }

            std::list<uint64> m_EncountersGuid;

            bool m_FirstIntro;
            bool m_HasQuickAccess;
            bool m_Initialized;

            uint32 m_DungeonID;

            uint8 m_SecondTrashDead;
            uint8 m_PreAlurielTrashDead;
            uint8 m_PromenadeGuardsDead;
            uint64 m_EntranceTriggerGuid;
            uint64 m_QuickAccessDoorGuid;
            uint64 m_EntranceThalyssraGuid;
            uint64 m_ThalyssraGuid;
            uint64 m_SecondThalyssraGuid;
            uint64 m_GenericStalkerGuid;
            uint64 m_TrilliaxImpDoorGuid[2];

            uint32 m_TrilliaxIntro;
            uint32 m_AnomalyIntro;
            uint32 m_PreAlurielTrash;
            uint32 m_SecondWingInitialTrash;
            uint32 m_StarAugurEtraeusTrashIntro;

            uint64 m_DoorAnomalyPre;
            uint64 m_DoorArcingDepthsGuid;
            uint64 m_StairsToEtraeusDoorGuid;
            std::pair<uint64, uint64> m_NightholdSpireDoorGuid;
            std::vector<uint64> m_MysteriousFruits;

            uint64 m_KrosusChestGuid;

            uint64 m_GuldanGuid;
            uint64 m_GuldanPlatformGuid;
            uint64 m_GuldanCenterPlatform;
            uint64 m_GuldanCenterSphere;
            uint64 m_GuldanCenterRune;
            uint64 m_GuldanChestGuid;

            uint64 m_ArchmageKhadgarGuid;
            uint64 m_IllidanPrisonGuid;

            std::vector<uint64> m_AlurielPreStairsNpcs;

            std::map<uint32, uint64> m_GuldanCreatures;
            std::map<uint32, uint64> m_GuldanGameObjects;

            bool m_SkorpyronAchiev;
            bool m_AnomalyAchiev;
            bool m_TrilliaxAchiev;
            bool m_GuldanAchievement;

            uint32 m_AnomalySpotlightCount;
            uint32 m_ToxicSlicesCount;
            uint32 m_EyeOfGuldanKillTime;
            uint32 m_EyeOfGuldanKillCount;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);

                m_FirstIntro                    = true;
                m_HasQuickAccess                = false;
                m_Initialized                   = false;

                m_DungeonID                     = 0;

                m_SecondTrashDead               = 0;
                m_PromenadeGuardsDead           = 0;
                m_PreAlurielTrashDead           = 0;
                m_EntranceTriggerGuid           = 0;
                m_EntranceThalyssraGuid         = 0;
                m_QuickAccessDoorGuid           = 0;
                m_SecondThalyssraGuid           = 0;
                m_ThalyssraGuid                 = 0;
                m_GenericStalkerGuid            = 0;
                m_TrilliaxImpDoorGuid[0]        = 0;
                m_TrilliaxImpDoorGuid[1]        = 0;
                m_TrilliaxIntro                 = 0;
                m_AnomalyIntro                  = 0;
                m_DoorArcingDepthsGuid          = 0;
                m_DoorAnomalyPre                = 0;
                m_StairsToEtraeusDoorGuid       = 0;
                m_SecondWingInitialTrash        = EncounterState::NOT_STARTED;
                m_StarAugurEtraeusTrashIntro    = EncounterState::NOT_STARTED;
                m_PreAlurielTrash               = EncounterState::NOT_STARTED;
                m_NightholdSpireDoorGuid        = std::make_pair(0, 0);

                m_KrosusChestGuid               = 0;

                m_GuldanGuid                    = 0;
                m_GuldanPlatformGuid            = 0;
                m_GuldanCenterPlatform          = 0;
                m_GuldanCenterSphere            = 0;
                m_GuldanCenterRune              = 0;
                m_GuldanChestGuid               = 0;

                m_ArchmageKhadgarGuid           = 0;
                m_IllidanPrisonGuid             = 0;

                m_SkorpyronAchiev               = false;
                m_AnomalyAchiev                 = false;
                m_TrilliaxAchiev                = instance->IsLFR() ? false : true;
                m_GuldanAchievement             = false;

                m_AnomalySpotlightCount         = 0;
                m_ToxicSlicesCount              = 0;
                m_EyeOfGuldanKillTime           = 0;
                m_EyeOfGuldanKillCount          = 0;
            }

            void FillInitialWorldStates(ByteBuffer& p_Buffer) override
            {
                p_Buffer << uint32(eAchievementsData::CageRematchWorldStateID) << uint32(m_SkorpyronAchiev);
            }

            void StartAlurielDeadConversation()
            {
                Creature* l_Aluriel = instance->GetCreature(GetData64(eCreatures::BossSpellbladeAluriel));
                Creature* l_Allied = nullptr;

                if (l_Aluriel == nullptr)
                    return;

                float l_Angle = 0;
                Position l_Pos;
                float l_Radius = 5.5f;

                for (const uint64 & l_Itr : m_AlurielPreStairsNpcs)
                {
                    l_Allied = sObjectAccessor->GetCreature(*l_Aluriel, l_Itr);

                    l_Aluriel->GetNearPosition(l_Pos, l_Radius, l_Angle);
                    l_Angle += ((2.f * float(M_PI)) / 9.f);

                    if (l_Allied != nullptr)
                    {
                        l_Allied->SetVisible(true);

                        if (l_Allied->GetEntry() == eCreatures::NpcKhadgar)
                        {
                            l_Allied->NearTeleportTo(g_KhadgarAlurielStairTeleportPos, true);

                            if (!l_Allied->IsAIEnabled)
                                continue;

                            l_Allied->GetAI()->DoAction(eActions::ActionAlurielOutro);
                        }
                        else if (l_Allied->GetEntry() == eCreatures::NpcKirinTorGuardian && l_Allied->GetEntry() == eCreatures::NpcKirinTorGuardian2)
                            continue;
                        else
                            l_Allied->GetMotionMaster()->MovePoint(eSharedPoints::PointAlurielDead, l_Pos, true);

                        l_Allied->SetHomePosition(l_Pos);
                    }
                }

                l_Angle = 0.f;
                l_Radius += 6.5f;

                for (const uint64 & l_Itr : m_AlurielPreStairsNpcs)
                {
                    l_Allied = sObjectAccessor->GetCreature(*l_Aluriel, l_Itr);

                    l_Aluriel->GetNearPosition(l_Pos, l_Radius, l_Angle);
                    l_Angle += ((2.f * float(M_PI)) / 9.f);

                    if (l_Allied == nullptr)
                        continue;

                    if (l_Allied->GetEntry() != eCreatures::NpcKirinTorGuardian && l_Allied->GetEntry() != eCreatures::NpcKirinTorGuardian2)
                        continue;

                    l_Allied->GetMotionMaster()->MovePoint(eSharedPoints::PointAlurielDead, l_Pos, true);
                    l_Allied->SetHomePosition(l_Pos);
                }
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_HasQuickAccess)
                {
                    if ((instance->IsMythic() && p_Player->IsQuestRewarded(eNHQuests::QuickAccessMythic)) ||
                        (instance->IsHeroic() && p_Player->IsQuestRewarded(eNHQuests::QuickAccessHeroic)) ||
                        (instance->IsNormal() && p_Player->IsQuestRewarded(eNHQuests::QuickAccessNormal)))
                        m_HasQuickAccess = true;
                }

                if (m_FirstIntro)
                {
                    m_FirstIntro = false;

                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Creature* l_Thalyssra = instance->GetCreature(m_EntranceThalyssraGuid))
                        {
                            if (l_Thalyssra->IsAIEnabled)
                                l_Thalyssra->AI()->DoAction(eActions::ActionThalyssraRaidIntro);
                        }
                    });
                }

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    m_DungeonID = p_Player->GetGroup() ? sLFGMgr->GetDungeon(p_Player->GetGroup()->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    std::vector<uint64> m_DisabledGoBs;

                    uint32 l_DisabledMask = 0;

                    switch (m_DungeonID)
                    {
                        case eDungeonIDs::DungeonArcingAqueducts:
                        {
                            l_DisabledMask |= (1 << eData::DataSpellbladeAluriel);
                            l_DisabledMask |= (1 << eData::DataStarAugurEtraeus);
                            l_DisabledMask |= (1 << eData::DataHighBotanistTelarn);
                            l_DisabledMask |= (1 << eData::DataKrosus);
                            l_DisabledMask |= (1 << eData::DataTichondrius);
                            l_DisabledMask |= (1 << eData::DataElisande);
                            l_DisabledMask |= (1 << eData::DataGuldan);

                            m_DisabledGoBs.push_back(m_DoorArcingDepthsGuid);
                            m_DisabledGoBs.push_back(m_KrosusChestGuid);
                            m_DisabledGoBs.push_back(m_GuldanChestGuid);
                            break;
                        }
                        case eDungeonIDs::DungeonRoyalAthaneum:
                        {
                            l_DisabledMask |= (1 << eData::DataSkorpyron);
                            l_DisabledMask |= (1 << eData::DataChronomaticAnomaly);
                            l_DisabledMask |= (1 << eData::DataTrilliax);
                            l_DisabledMask |= (1 << eData::DataKrosus);
                            l_DisabledMask |= (1 << eData::DataTichondrius);
                            l_DisabledMask |= (1 << eData::DataElisande);
                            l_DisabledMask |= (1 << eData::DataGuldan);

                            m_DisabledGoBs.push_back(m_KrosusChestGuid);
                            m_DisabledGoBs.push_back(m_GuldanChestGuid);
                            break;
                        }
                        case eDungeonIDs::DungeonNightspire:
                        {
                            l_DisabledMask |= (1 << eData::DataSkorpyron);
                            l_DisabledMask |= (1 << eData::DataChronomaticAnomaly);
                            l_DisabledMask |= (1 << eData::DataTrilliax);
                            l_DisabledMask |= (1 << eData::DataSpellbladeAluriel);
                            l_DisabledMask |= (1 << eData::DataStarAugurEtraeus);
                            l_DisabledMask |= (1 << eData::DataHighBotanistTelarn);
                            l_DisabledMask |= (1 << eData::DataGuldan);

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Aluriel = instance->GetCreature(GetData64(eCreatures::BossSpellbladeAluriel)))
                                {
                                    l_Aluriel->SetVisible(false);
                                    l_Aluriel->SetReactState(ReactStates::REACT_PASSIVE);
                                }
                            });

                            m_DisabledGoBs.push_back(m_StairsToEtraeusDoorGuid);
                            m_DisabledGoBs.push_back(m_GuldanChestGuid);
                            break;
                        }
                        case eDungeonIDs::DungeonBetrayersRise:
                        {
                            l_DisabledMask |= (1 << eData::DataSkorpyron);
                            l_DisabledMask |= (1 << eData::DataChronomaticAnomaly);
                            l_DisabledMask |= (1 << eData::DataTrilliax);
                            l_DisabledMask |= (1 << eData::DataSpellbladeAluriel);
                            l_DisabledMask |= (1 << eData::DataStarAugurEtraeus);
                            l_DisabledMask |= (1 << eData::DataHighBotanistTelarn);
                            l_DisabledMask |= (1 << eData::DataKrosus);
                            l_DisabledMask |= (1 << eData::DataTichondrius);
                            l_DisabledMask |= (1 << eData::DataElisande);
                            break;
                        }
                        default:
                            break;
                    }

                    SetDisabledBosses(l_DisabledMask);

                    for (uint64 l_Guid : m_DisabledGoBs)
                    {
                        if (GameObject* l_Door = instance->GetGameObject(l_Guid))
                            l_Door->SetGoState(GOState::GO_STATE_READY);
                    }
                }

                /// Don't teleport again on LFR, entrances points are already fine
                if (!m_DungeonID)
                {
                    uint64 l_Guid = p_Player->GetGUID();
                    AddTimedDelayedOperation(200, [this, l_Guid]() -> void
                    {
                        uint32 l_Loc = 0;

                        SetSafeLocation(l_Loc);

                        if (l_Loc == 0)
                            return;

                        if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                            l_Player->NearTeleportTo(l_Loc);
                    });
                }
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::NpcSkorpyron:
                    case eCreatures::NpcChromaticAnomaly:
                    case eCreatures::NpcTrilliax:
                    case eCreatures::BossSpellbladeAluriel:
                    case eCreatures::BossStarAugurEtraeus:
                    {
                        m_EncountersGuid.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::BossHighBotanistTelarn:
                    case eCreatures::NpcSolaristTelarn:
                    case eCreatures::NpcNaturalistTelarn:
                    case eCreatures::NpcArcanistTelarn:
                    {
                        if (!instance->IsMythic())
                            AddToDamageManager(p_Creature);

                        m_EncountersGuid.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::NpcKirinTorGuardian:
                    case eCreatures::NpcKirinTorGuardian2:
                    {
                        if (p_Creature->IsNearPosition(&g_ThalysrraSecondPos, 10.f))
                            m_AlurielPreStairsNpcs.push_back(p_Creature->GetGUID());

                        break;
                    }
                    case eCreatures::NpcThalyssra:
                    {
                        Position l_GuldanPos = { 283.601f, 3108.45f, 465.933f, 1.6597f };

                        if (p_Creature->IsNearPosition(&l_GuldanPos, 0.5f))
                        {
                            m_GuldanCreatures[p_Creature->GetEntry()] = p_Creature->GetGUID();
                            p_Creature->SetVisible(false);
                            break;
                        }

                        if (p_Creature->IsNearPosition(&g_EntrancePos, 0.5f))
                            m_EntranceThalyssraGuid = p_Creature->GetGUID();
                        else if (p_Creature->IsNearPosition(&g_ThalysrraSecondPos, 2.f))
                        {
                            m_SecondThalyssraGuid = p_Creature->GetGUID();
                            m_AlurielPreStairsNpcs.push_back(p_Creature->GetGUID());
                        }
                        else
                            m_ThalyssraGuid = p_Creature->GetGUID();

                        break;
                    }
                    case eCreatures::NpcDeadCrystallineScorpid:
                    case eCreatures::NpcFallenDefender:
                    {
                        p_Creature->CastSpell(p_Creature, eSharedSpells::SpellFakeDeath, true);
                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    }
                    case eCreatures::NpcSLGGenericStalker:
                    {
                        m_GenericStalkerGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcGulDan:
                    {
                        m_GuldanGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcArchmageKhadgar:
                    {
                        m_ArchmageKhadgarGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcIllidanStormragePrison:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                        m_IllidanPrisonGuid = p_Creature->GetGUID();
                        break;
                    }

                    case eCreatures::NpcKhadgar:
                    {
                        if (p_Creature->IsNearPosition(&g_AlurielStairsNpcPos, 20.f))
                        {
                            m_AlurielPreStairsNpcs.push_back(p_Creature->GetGUID());
                            p_Creature->SetVisible(false);
                        }
                        break;
                    }

                    case eCreatures::NpcVictoire:
                    case eCreatures::NpcTyrandeWhisperwind:
                    case eCreatures::NpcSilgryn:
                    case eCreatures::NpcArluelle:
                    case eCreatures::NpcArcanistValtrois:
                    case eCreatures::NpcLadyLiadrin:
                    case eCreatures::NpcGrandMagisterRommath:
                    case eCreatures::NpcIllidanStormrage:
                    case eCreatures::NpcKal:
                    {
                        p_Creature->SetVisible(false);

                        if (p_Creature->IsNearPosition(&g_AlurielStairsNpcPos, 20.f))
                            m_AlurielPreStairsNpcs.push_back(p_Creature->GetGUID());
                        else
                            m_GuldanCreatures[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcWorldTrigger:
                    {
                        p_Creature->SetVisible(false);
                        break;
                    }
                    case eCreatures::NpcSLGGenericMoPLargeAoI:
                    {
                        Position l_Pos = { -67.61111f, 3421.665f, -255.2137f, 0.7465991f };

                        if (p_Creature->IsNearPosition(&l_Pos, 0.1f))
                        {
                            m_EntranceTriggerGuid = p_Creature->GetGUID();

                            p_Creature->CastSpell(p_Creature, eSharedSpells::SpellDoorLock, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::BossHighBotanistTelarn:
                    case eCreatures::NpcSolaristTelarn:
                    case eCreatures::NpcNaturalistTelarn:
                    case eCreatures::NpcArcanistTelarn:
                    {
                        if (!instance->IsMythic())
                            RemoveFromDamageManager(p_Creature);
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
                    case eGameObjects::GoSkorpyronDoor1:
                    case eGameObjects::GoSkorpyronDoor2:
                    case eGameObjects::GoAnomalyDoor1:
                    case eGameObjects::GoAnomalyDoor2:
                    case eGameObjects::GoTrilliaxDoor1:
                    case eGameObjects::GoTrilliaxDoor2:
                    case eGameObjects::GoAstromancerDoor:
                    case eGameObjects::GoBotanistDoorRight:
                    case eGameObjects::GoBotanistDoorLeft:
                    case eGameObjects::GoKrosusTeleporterDoor:
                    case eGameObjects::GoElisandeDoorNE:
                    case eGameObjects::GoElisandeDoorNW:
                    case eGameObjects::GoElisandeDoorSW:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case eGameObjects::GoArcingDepthsExit:
                    {
                        m_DoorArcingDepthsGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoAnomalyPre:
                    {
                        if (m_AnomalyIntro == EncounterState::DONE)
                            p_GameObject->SetGoState(GOState::GO_STATE_ACTIVE);

                        m_DoorAnomalyPre = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoTrilliaxDoor3:
                    {
                        m_TrilliaxImpDoorGuid[0] = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoTrilliaxDoor4:
                    {
                        m_TrilliaxImpDoorGuid[1] = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoGulDanCenterPlatform:
                    {
                        p_GameObject->SendGameObjectActivateAnimKit(0);
                        m_GuldanCenterPlatform = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoEyeOfAmanthulFloorSphere:
                    {
                        p_GameObject->SendGameObjectActivateAnimKit(0);
                        m_GuldanCenterSphere = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoEyeOfAmanthulFloorRune:
                    {
                        p_GameObject->SendGameObjectActivateAnimKit(0);
                        m_GuldanCenterRune = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoGulDanRoom:
                    {
                        m_GuldanPlatformGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoKrosusChest:
                    {
                        m_KrosusChestGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoGiftOfTheNightborne:
                    {
                        m_GuldanChestGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoEnergyConduitSW:
                    case eGameObjects::GoEnergyConduitNW:
                    case eGameObjects::GoEnergyConduitSE:
                    case eGameObjects::GoEnergyConduitNE:
                    case eGameObjects::GoFocusingStatueNE:
                    case eGameObjects::GoFocusingStatueNW:
                    case eGameObjects::GoFocusingStatueSE:
                    case eGameObjects::GoFocusingStatueSW:
                    case eGameObjects::GoEyeOfAmanthulWorldS:
                    {
                        m_GuldanGameObjects[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoAstromancerTowerDoor:
                    {
                        m_StairsToEtraeusDoorGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoNightholdSpireDoorFront:
                    case eGameObjects::GoNightholdSpireDoorBack:
                    {
                        AddDoor(p_GameObject, true);

                        if (m_NightholdSpireDoorGuid.first == 0)
                            m_NightholdSpireDoorGuid.first = p_GameObject->GetGUID();
                        else
                            m_NightholdSpireDoorGuid.second = p_GameObject->GetGUID();

                        break;
                    }
                    case eGameObjects::GoMysteriousFruit:
                    {
                        m_MysteriousFruits.push_back(p_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::GoSkorpyronTeleporterDoor:
                    {
                        m_QuickAccessDoorGuid = p_GameObject->GetGUID();
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
                    case eGameObjects::GoSkorpyronDoor1:
                    case eGameObjects::GoSkorpyronDoor2:
                    case eGameObjects::GoAnomalyDoor1:
                    case eGameObjects::GoAnomalyDoor2:
                    case eGameObjects::GoTrilliaxDoor1:
                    case eGameObjects::GoTrilliaxDoor2:
                    case eGameObjects::GoAstromancerDoor:
                    case eGameObjects::GoNightholdSpireDoorFront:
                    case eGameObjects::GoNightholdSpireDoorBack:
                    case eGameObjects::GoBotanistDoorRight:
                    case eGameObjects::GoBotanistDoorLeft:
                    case eGameObjects::GoKrosusTeleporterDoor:
                    case eGameObjects::GoElisandeDoorNE:
                    case eGameObjects::GoElisandeDoorNW:
                    case eGameObjects::GoElisandeDoorSW:
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
                if (p_Unit == nullptr)
                    return;

                switch (p_Unit->GetEntry())
                {
                    case eCreatures::NpcTrashVolatileScorpid:
                    case eCreatures::NpcTrashAcidmawScorpid:
                    {
                        std::vector<Position> l_CheckPos =
                        {
                            { -47.8235f, 3423.03f, -265.83f, 0.0f },
                            { -37.9261f, 3451.74f, -265.25f, 0.0f }
                        };

                        for (Position l_Pos : l_CheckPos)
                        {
                            Position l_HomePos = p_Unit->ToCreature()->GetHomePosition();
                            if (!l_Pos.IsNearPosition(&l_HomePos, 1.0f))
                                continue;

                            if (Creature* l_Talysra = instance->GetCreature(m_EntranceThalyssraGuid))
                            {
                                if (l_Talysra->IsAIEnabled)
                                    l_Talysra->AI()->SetData(eData::DataFirstTrashesKilled, p_Unit->GetEntry());
                            }
                        }

                        break;
                    }
                    case eCreatures::NpcFulminant:
                    case eCreatures::NpcChaotoid:
                    {
                        if (Creature* l_Stalker = instance->GetCreature(m_GenericStalkerGuid))
                        {
                            if (l_Stalker->IsAIEnabled)
                                l_Stalker->AI()->SetGUID(p_Unit->GetGUID());
                        }

                        break;
                    }
                    case eCreatures::NpcFallenDefender:
                    case eCreatures::NpcDuskwatchSentinel:
                    case eCreatures::NpcDuskwatchBattleMagus:
                    case eCreatures::NpcShamblingHungerer:
                    case eCreatures::NpcWitheredSkulker:
                    case eCreatures::NpcChronowraith:
                    {
                        m_SecondTrashDead++;

                        if (m_SecondTrashDead >= g_MaxTrashDiedDepthsSecondWing && GetData(DataSecondTrashesKilled) != EncounterState::DONE && GetBossState(eData::DataTrilliax) == EncounterState::DONE)
                        {
                            m_PreAlurielTrashDead = 0;

                            if (Creature * l_Thalysrra = instance->GetCreature(m_SecondThalyssraGuid))
                            {
                                if (l_Thalysrra->IsAIEnabled)
                                    l_Thalysrra->GetAI()->SetData(eData::DataSecondTrashesKilled, EncounterState::DONE);

                                SetData(eData::DataSecondTrashesKilled, EncounterState::DONE);
                            }
                        }
                        else if (GetData(DataSecondTrashesKilled) == EncounterState::DONE && GetData(eData::DataPreAlurielTrash) == EncounterState::IN_PROGRESS)
                        {
                            m_PreAlurielTrashDead++;

                            if (m_PreAlurielTrashDead >= 7)
                            {
                                if (Creature * l_Thalysrra = instance->GetCreature(m_SecondThalyssraGuid))
                                {
                                    if (l_Thalysrra->IsAIEnabled)
                                        l_Thalysrra->GetAI()->SetData(eData::DataPreAlurielTrash, EncounterState::DONE);

                                    SetData(eData::DataPreAlurielTrash, EncounterState::DONE);
                                }
                            }

                        }

                        break;
                    }

                    case eCreatures::NpcPromenadeGuard:
                    {
                        m_PromenadeGuardsDead++;

                        if (m_PromenadeGuardsDead >= 8)
                        {
                            Creature* l_Thalysrra   = instance->GetCreature(GetData64(eCreatures::NpcThalyssra));
                            Creature* l_Aluriel     = instance->GetCreature(GetData64(eCreatures::BossSpellbladeAluriel));

                            if (l_Thalysrra != nullptr && l_Thalysrra->IsAIEnabled)
                                l_Thalysrra->AI()->Talk(eSharedTalks::TalkAlurielPromenadeGuardsDead);

                            if (l_Aluriel != nullptr && l_Aluriel->IsAIEnabled)
                                l_Aluriel->AI()->DelayTalk(10, eSharedTalks::TalkAlurielPromenadeGuardsDead);
                        }

                        break;
                    }

                    case eCreatures::NpcEyeOfGuldan:
                    {
                        /// No need to process if criteria is arleady completed
                        if (m_GuldanAchievement)
                            break;

                        if (!m_EyeOfGuldanKillTime)
                            m_EyeOfGuldanKillTime = uint32(time(nullptr));

                        if ((++m_EyeOfGuldanKillCount) >= 16)
                            m_GuldanAchievement = true;

                        break;
                    }
                    default:
                        break;
                }
            }

            bool CheckBossesFromSecondWing() const
            {
                return (GetBossState(eData::DataSpellbladeAluriel) == EncounterState::DONE &&
                        GetBossState(eData::DataHighBotanistTelarn) == EncounterState::DONE &&
                        GetBossState(eData::DataStarAugurEtraeus) == EncounterState::DONE);
            }

            bool CheckBossesFromThirdWing() const
            {
                return GetBossState(eData::DataTichondrius) == EncounterState::DONE;
            }

            bool CheckThirdLFRWingBosses() const
            {
                return GetBossState(eData::DataKrosus) == EncounterState::DONE && GetBossState(eData::DataTichondrius) == EncounterState::DONE;
            }

            void HandleNightholdSpireDoors()
            {
                if ((CheckBossesFromSecondWing() && CheckBossesFromThirdWing()) || (instance->IsLFR() && m_DungeonID == eDungeonIDs::DungeonNightspire && CheckThirdLFRWingBosses()))
                {
                    HandleGameObject(m_NightholdSpireDoorGuid.first, true);
                    HandleGameObject(m_NightholdSpireDoorGuid.second, true);
                }
                else
                {
                    HandleGameObject(m_NightholdSpireDoorGuid.first, false);
                    HandleGameObject(m_NightholdSpireDoorGuid.second, false);
                }
            }

            void RespawnFruits()
            {
                for (const uint64 & l_Itr : m_MysteriousFruits)
                {
                    GameObject* l_Gob = instance->GetGameObject(l_Itr);

                    if (l_Gob)
                        l_Gob->Respawn();
                }
            }

            bool SetBossState(uint32 p_ID, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_ID, p_State))
                    return false;

                HandleNightholdSpireDoors();

                switch (p_ID)
                {
                    case eData::DataSkorpyron:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            if (Creature* l_Talysra = instance->GetCreature(m_EntranceThalyssraGuid))
                            {
                                if (l_Talysra->IsAIEnabled)
                                    l_Talysra->AI()->DoAction(eActions::ActionMoveAfterFirstBoss);
                            }

                            if (m_SkorpyronAchiev)
                                DoCompleteAchievement(eAchievementsData::CageRematchAchievID);
                        }

                        break;
                    }
                    case eData::DataChronomaticAnomaly:
                    {
                        if (p_State == EncounterState::DONE && m_AnomalyAchiev)
                            DoCompleteAchievement(eAchievementsData::GrandOpening);
                        else
                        {
                            m_AnomalyAchiev         = false;
                            m_AnomalySpotlightCount = 0;
                        }

                        break;
                    }
                    case eData::DataTrilliax:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            m_SecondTrashDead = 0;
                            SetData(eData::DataSecondTrashesKilled, EncounterState::IN_PROGRESS);

                            if (m_TrilliaxAchiev)
                                DoCompleteAchievement(eAchievementsData::GlutenFree);
                        }
                        else
                        {
                            m_TrilliaxAchiev    = instance->IsLFR() ? false : true;
                            m_ToxicSlicesCount  = 0;
                        }

                        break;
                    }
                    case eData::DataSpellbladeAluriel:
                    {
                        if (p_State == EncounterState::DONE)
                            StartAlurielDeadConversation();

                        break;
                    }
                    case eData::DataKrosus:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID());
                            if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) != ItemContext::None)
                                l_DisabledLoots = false;

                            if (!l_DisabledLoots)
                                l_DisabledLoots = sObjectMgr->IsDisabledEncounter(eData::DataKrosusEncounterID, instance->GetDifficultyID());

                            if (!l_DisabledLoots)
                            {
                                if (GameObject* l_Chest = instance->GetGameObject(m_KrosusChestGuid))
                                    l_Chest->SetRespawnTime(TimeConstants::WEEK);
                            }
                        }

                        break;
                    }
                    case eData::DataGuldan:
                    {
                        if (p_State == EncounterState::DONE)
                        {
                            bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID());
                            if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) != ItemContext::None)
                                l_DisabledLoots = false;

                            if (!l_DisabledLoots)
                                l_DisabledLoots = sObjectMgr->IsDisabledEncounter(eData::DataGuldanEncounterID, instance->GetDifficultyID());

                            if (!l_DisabledLoots)
                            {
                                if (!instance->IsLFR() && m_GuldanAchievement)
                                    DoCompleteAchievement(eAchievementsData::IveGotMyEyesOnYou);

                                /// Mythic drops loots from The Demon Within's corpse
                                if (!instance->IsMythic())
                                {
                                    if (GameObject* l_Chest = instance->GetGameObject(m_GuldanChestGuid))
                                        l_Chest->SetRespawnTime(TimeConstants::WEEK);
                                }
                            }

                            for (auto const& l_Iter : m_GuldanCreatures)
                            {
                                if (Creature* l_Creature = instance->GetCreature(l_Iter.second))
                                    l_Creature->SetVisible(true);
                            }

                            if (Creature* l_Prison = instance->GetCreature(m_IllidanPrisonGuid))
                                l_Prison->SetVisible(false);
                        }
                        else
                        {
                            m_GuldanAchievement     = false;
                            m_EyeOfGuldanKillCount  = 0;
                            m_EyeOfGuldanKillTime   = 0;
                        }

                        break;
                    }
                    case eData::DataHighBotanistTelarn:
                    {
                        if (instance->IsLFR())
                            break;

                        if (p_State == EncounterState::DONE)
                        {
                            auto & l_Players = instance->GetPlayers();
                            bool l_Result = true;

                            for (auto & l_Itr : l_Players)
                            {
                                if (l_Itr.getSource() == nullptr)
                                    continue;

                                if (!l_Itr.getSource()->HasAura(eSharedSpells::SpellArcaneExposure))
                                {
                                    l_Result = false;
                                    break;
                                }
                            }

                            if (l_Result)
                                DoCompleteAchievement(eAchievementsData::FruitOfAllEvil);

                            if (instance->IsMythic())
                                DoCompleteAchievement(eAchievementsData::BotanistMythic);
                        }
                        else if (p_State == EncounterState::FAIL || p_State == EncounterState::NOT_STARTED)
                        {
                            RespawnFruits();
                            std::for_each(g_FormsDead.begin(), g_FormsDead.end(), [](FormDead& p_Itr)
                            {
                                p_Itr.m_Dead        = false;
                                p_Itr.m_FirstDead   = false;
                            });
                        }

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
                    case eData::DataTrilliaxIntro:
                    {
                        m_TrilliaxIntro = p_Data;
                        SaveToDB();
                        break;
                    }
                    case eData::DataTrilliaxImprintDoor:
                    {
                        switch (p_Data)
                        {
                            case 0:
                            {
                                HandleGameObject(m_TrilliaxImpDoorGuid[0], false);
                                break;
                            }
                            case 1:
                            {
                                HandleGameObject(m_TrilliaxImpDoorGuid[0], true);
                                break;
                            }
                            case 2:
                            {
                                HandleGameObject(m_TrilliaxImpDoorGuid[1], false);
                                break;
                            }
                            case 3:
                            {
                                HandleGameObject(m_TrilliaxImpDoorGuid[1], true);
                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eData::DataSkorpyronIntro:
                    {
                        if (Creature* l_Talysra = instance->GetCreature(m_EntranceThalyssraGuid))
                        {
                            if (l_Talysra->IsAIEnabled)
                                l_Talysra->AI()->DoAction(eActions::ActionMoveFirstBoss);
                        }

                        break;
                    }
                    case eData::DataAnomalyIntro:
                    {
                        switch (p_Data)
                        {
                            case 0:
                            {
                                if (Creature* l_Talysra = instance->GetCreature(m_EntranceThalyssraGuid))
                                {
                                    if (l_Talysra->IsAIEnabled)
                                        l_Talysra->AI()->DoAction(eActions::ActionFirstMoveToAnomaly);
                                }

                                break;
                            }
                            case 1:
                            {
                                if (Creature* l_Talysra = instance->GetCreature(m_EntranceThalyssraGuid))
                                {
                                    if (l_Talysra->IsAIEnabled)
                                        l_Talysra->AI()->DoAction(eActions::ActionSecondMoveToAnomaly);
                                }

                                break;
                            }
                            case 2:
                            {
                                m_AnomalyIntro = EncounterState::DONE;
                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eData::DataSkorpyronAchiev:
                    {
                        m_SkorpyronAchiev = p_Data != 0;

                        DoUpdateWorldState(eAchievementsData::CageRematchWorldStateID, m_SkorpyronAchiev);
                        break;
                    }
                    case eData::DataAnomalyAchiev:
                    {
                        if (!instance->IsLFR() && ++m_AnomalySpotlightCount >= eAchievementsData::GrandOpeningCount)
                            m_AnomalyAchiev = true;

                        break;
                    }
                    case eData::DataAstrologerJarin:
                    {
                        m_StarAugurEtraeusTrashIntro = p_Data;

                        if (p_Data == EncounterState::DONE)
                        {
                            HandleGameObject(m_StairsToEtraeusDoorGuid, true);
                            GameObject* l_Door = instance->GetGameObject(m_StairsToEtraeusDoorGuid);

                            if (l_Door != nullptr)
                                l_Door->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                        else
                            HandleGameObject(m_StairsToEtraeusDoorGuid, false);

                        break;
                    }
                    case eData::DataPreAlurielTrash:
                    {
                        SaveToDB();
                        m_PreAlurielTrash = p_Data;
                        break;
                    }
                    case eData::DataTrilliaxAchiev:
                    {
                        if (!instance->IsLFR() && ++m_ToxicSlicesCount > eAchievementsData::GlutenFreeMaxCount)
                            m_TrilliaxAchiev = false;

                        break;
                    }
                    case eData::DataSecondTrashesKilled:
                    {
                        if (p_Data == EncounterState::DONE)
                            SaveToDB();

                        m_SecondWingInitialTrash = p_Data;
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
                    case eData::DataTrilliaxIntro:
                        return m_TrilliaxIntro;
                    case eData::DataAnomalyIntro:
                        return m_AnomalyIntro;
                    case eData::DataSecondTrashesKilled:
                        return m_SecondWingInitialTrash;
                    case eData::DataPreAlurielTrash:
                        return m_PreAlurielTrash;
                    case eData::DataHasQuickAccess:
                        return m_HasQuickAccess;
                    default:
                        break;
                }

                return 0;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                for (uint64 l_Guid : m_EncountersGuid)
                {
                    if (Creature* l_Creature = instance->GetCreature(l_Guid))
                    {
                        if (l_Creature->GetEntry() == p_Type)
                            return l_Guid;
                    }
                }

                switch (p_Type)
                {
                    case eCreatures::NpcThalyssra:
                        return m_ThalyssraGuid;
                    case eCreatures::NpcArchmageKhadgar:
                        return m_ArchmageKhadgarGuid;
                    case eCreatures::NpcIllidanStormragePrison:
                        return m_IllidanPrisonGuid;
                    case eCreatures::NpcGulDan:
                        return m_GuldanGuid;
                    case eGameObjects::GoAnomalyPre:
                        return m_DoorAnomalyPre;
                    case eGameObjects::GoEnergyConduitSW:
                    case eGameObjects::GoEnergyConduitNW:
                    case eGameObjects::GoEnergyConduitSE:
                    case eGameObjects::GoEnergyConduitNE:
                    case eGameObjects::GoFocusingStatueNE:
                    case eGameObjects::GoFocusingStatueNW:
                    case eGameObjects::GoFocusingStatueSE:
                    case eGameObjects::GoFocusingStatueSW:
                    case eGameObjects::GoEyeOfAmanthulWorldS:
                    {
                        auto l_Iter = m_GuldanGameObjects.find(p_Type);
                        if (l_Iter == m_GuldanGameObjects.end())
                            return 0;

                        return l_Iter->second;
                    }
                    case eGameObjects::GoGulDanRoom:
                        return m_GuldanPlatformGuid;
                    case eGameObjects::GoGulDanCenterPlatform:
                        return m_GuldanCenterPlatform;
                    case eGameObjects::GoEyeOfAmanthulFloorSphere:
                        return m_GuldanCenterSphere;
                    case eGameObjects::GoEyeOfAmanthulFloorRune:
                        return m_GuldanCenterRune;
                    default:
                        break;
                }

                return 0;
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
                    case eData::DataChronomaticAnomaly:
                    {
                        if (GetBossState(eData::DataSkorpyron) != EncounterState::DONE)
                            return false;

                        break;
                    }
                    case eData::DataTrilliax:
                    {
                        if (GetBossState(eData::DataChronomaticAnomaly) != EncounterState::DONE)
                            return false;

                        break;
                    }
                    case eData::DataSpellbladeAluriel:
                    case eData::DataHighBotanistTelarn:
                    case eData::DataStarAugurEtraeus:
                    {
                        if (GetBossState(eData::DataTrilliax) != EncounterState::DONE)
                            return false;

                        break;
                    }
                    default:
                        break;
                }

                return true;
            }

            std::string GetSaveData() override
            {
                std::ostringstream l_SaveStream;

                l_SaveStream << "N H " << GetBossSaveData() << m_SecondWingInitialTrash << m_AnomalyIntro << m_TrilliaxIntro <<
                m_PreAlurielTrash << m_StarAugurEtraeusTrashIntro;

                return l_SaveStream.str();
            }

            void Load(char const* p_Data) override
            {
                if (!p_Data)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(p_Data);

                char l_DataHead1, l_DataHead2;

                std::istringstream l_LoadStream(p_Data);
                l_LoadStream >> l_DataHead1 >> l_DataHead2;

                if (l_DataHead1 == 'N' && l_DataHead2 == 'H')
                {
                    for (uint32 l_I = 0; l_I < eData::MaxEncounters; ++l_I)
                    {
                        uint32 l_TmpState;

                        l_LoadStream >> l_TmpState;

                        if (l_TmpState == EncounterState::IN_PROGRESS || l_TmpState > EncounterState::SPECIAL)
                            l_TmpState = EncounterState::NOT_STARTED;

                        SetBossState(l_I, EncounterState(l_TmpState));
                    }

                    l_LoadStream >> m_SecondWingInitialTrash;
                    l_LoadStream >> m_AnomalyIntro;
                    l_LoadStream >> m_TrilliaxIntro;
                    l_LoadStream >> m_PreAlurielTrash;
                    l_LoadStream >> m_StarAugurEtraeusTrashIntro;

                    HandleNightholdSpireDoors();
                    SetData(eData::DataAstrologerJarin, m_StarAugurEtraeusTrashIntro);
                    SetData(eData::DataSecondTrashesKilled, m_SecondWingInitialTrash);
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void SetSafeLocation(uint32& p_LocId)
            {
                if (GetBossState(eData::DataSpellbladeAluriel) == EncounterState::DONE)
                    p_LocId = eLocations::LocCaptainQuartersEntrance;
                else if (GetData(eData::DataPreAlurielTrash) == EncounterState::DONE)
                    p_LocId = eLocations::LocAlurielPreStairs;
                else if (GetBossState(eData::DataTrilliax) == EncounterState::DONE)
                    p_LocId = eLocations::LocCatacombStairs;
                else if (GetBossState(eData::DataChronomaticAnomaly) == EncounterState::DONE)
                    p_LocId = eLocations::LocAnomalysRoom;
                else if (GetBossState(eData::DataSkorpyron) == EncounterState::DONE)
                    p_LocId = eLocations::LocScorpionRoom;
                else
                    p_LocId = eLocations::LocEntrance;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                if (instance->IsLFR() && m_DungeonID == eDungeonIDs::DungeonBetrayersRise)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocGuldanRoom);
                else if (GetBossState(eData::DataSpellbladeAluriel) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocCaptainQuartersEntrance);
                else if (GetData(eData::DataPreAlurielTrash) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocAlurielPreStairs);
                else if (instance->IsLFR() && m_DungeonID == eDungeonIDs::DungeonRoyalAthaneum)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocLFRWing2Entrance);
                else if (GetBossState(eData::DataTrilliax) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocCatacombStairs);
                else if (GetBossState(eData::DataChronomaticAnomaly) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocAnomalysRoom);
                else if (GetBossState(eData::DataSkorpyron) == EncounterState::DONE)
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocScorpionRoom);
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

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (m_HasQuickAccess && m_EntranceTriggerGuid)
                {
                    if (Creature* l_Trigger = instance->GetCreature(m_EntranceTriggerGuid))
                    {
                        if (l_Trigger->FindNearestPlayer(30.0f))
                        {
                            m_EntranceTriggerGuid = 0;

                            Map::PlayerList const& l_Players = instance->GetPlayers();
                            for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    sCreatureTextMgr->SendChat(l_Trigger, 4, l_Player->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);
                            }

                            l_Trigger->RemoveAllAuras();

                            if (GameObject* l_Door = instance->GetGameObject(m_QuickAccessDoorGuid))
                                l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
                        }
                    }
                }

                if (m_EyeOfGuldanKillTime)
                {
                    if (uint32(time(nullptr)) > (m_EyeOfGuldanKillTime + 3))
                    {
                        m_EyeOfGuldanKillTime   = 0;
                        m_EyeOfGuldanKillCount  = 0;
                        m_GuldanAchievement     = false;
                    }
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_nighthold_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_the_nighthold()
{
    new instance_the_nighthold();
}
#endif
