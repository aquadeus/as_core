#include "cathedral_of_eternal_night.hpp"

DoorData const g_DoorData[] =
{
    { eGameObjects::GoGardenDoor,           eData::DataAgronox,                 DoorType::DOOR_TYPE_ROOM,           BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoGardenDoor2,          eData::DataAgronox,                 DoorType::DOOR_TYPE_PASSAGE,        BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoGardenDoor3,          eData::DataAgronox,                 DoorType::DOOR_TYPE_PASSAGE,        BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoGardenDoor4,          eData::DataAgronox,                 DoorType::DOOR_TYPE_PASSAGE,        BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoScornfulDoor,         eData::DataTrashbiteTheScornful,    DoorType::DOOR_TYPE_ROOM,           BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoScornfulDoor2,        eData::DataTrashbiteTheScornful,    DoorType::DOOR_TYPE_PASSAGE,        BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoDomatraxDoor,         eData::DataDomatrax,                DoorType::DOOR_TYPE_ROOM,           BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoDomatraxDoor,         eData::DataMephistroth,             DoorType::DOOR_TYPE_ROOM,           BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoFelBarrier,           eData::DataNalasha,                 DoorType::DOOR_TYPE_PASSAGE,        BoundaryType::BOUNDARY_NONE },
    { 0,                                    0,                                  DoorType::DOOR_TYPE_ROOM,           BoundaryType::BOUNDARY_NONE },
};

using CreatureEntry = uint32;
using ObjectGUID = uint64;

using CreatureInfo = std::pair<CreatureEntry, ObjectGUID>;
using CreatureIntro = std::pair<CreatureEntry, EncounterState>;
using DataEvent = std::pair<eData, EncounterState>;

class instance_cathedral_of_eternal_night : public InstanceMapScript
{
    public:
        instance_cathedral_of_eternal_night() : InstanceMapScript("instance_cathedral_of_eternal_night", 1677)
        {}

        struct instance_cathedral_of_eternal_night_InstanceMapScript : public InstanceScript
        {
            public:
                explicit instance_cathedral_of_eternal_night_InstanceMapScript(InstanceMap* p_Map) : InstanceScript(p_Map)
                {
                    SetBossNumber(DataMaxEncounter);
                    m_AegisGobGUID = 0;
                }

                void Initialize() override
                {
                    m_EncounterGuids.clear();
                    m_BossesIntro.clear();
                    m_DataEvents.clear();

                    m_BatsDead = 0;

                    for (uint32 l_Itr = eData::DataIllidanMaievIntro; l_Itr <= eData::DataIllidanMaievKhadgar; ++l_Itr)
                        m_DataEvents.push_back(DataEvent(eData(l_Itr), EncounterState::NOT_STARTED));

                    m_BossesIntro.push_back(CreatureIntro(eCreatures::BossTrashBiteTheScornful, EncounterState::NOT_STARTED));
                    m_BossesIntro.push_back(CreatureIntro(eCreatures::BossDomatrax, EncounterState::NOT_STARTED));
                    m_BossesIntro.push_back(CreatureIntro(eCreatures::BossMephistroth, EncounterState::NOT_STARTED));

                    InstanceScript::LoadDoorData(g_DoorData);
                }

                void OnGameObjectCreate(GameObject* p_Go) override
                {
                    if (p_Go->GetEntry() == eGameObjects::GoAegisOfAggramar)
                    {
                        m_AegisGobGUID = p_Go->GetGUID();
                        return;
                    }

                    auto l_Itr = std::find_if(std::begin(g_DoorData), std::end(g_DoorData), [&p_Go](const DoorData & p_Itr)
                    {
                        return p_Itr.entry == p_Go->GetEntry();
                    });

                    if (l_Itr != std::end(g_DoorData))
                        InstanceScript::AddDoor(p_Go, true);
                }

                void OnGameObjectRemove(GameObject* p_Go) override
                {
                    auto l_Itr = std::find_if(std::begin(g_DoorData), std::end(g_DoorData), [&p_Go](const DoorData & p_Itr)
                    {
                        return p_Itr.entry == p_Go->GetEntry();
                    });

                    if (l_Itr != std::end(g_DoorData))
                        InstanceScript::AddDoor(p_Go, false);
                }

                void OnPlayerExit(Player* p_Player) override
                {
                    if (p_Player == nullptr)
                        return;

                    p_Player->RemoveAurasDueToSpell(eSharedSpells::SpellAegisOfAggrammarBuff);
                    p_Player->RemoveAurasDueToSpell(eSharedSpells::SpellFairlyExcited);
                    p_Player->RemoveAurasDueToSpell(eSharedSpells::SpellMidlyAmused);
                    p_Player->RemoveAurasDueToSpell(eSharedSpells::SpellModeratelyStirred);
                    p_Player->RemoveAurasDueToSpell(eSharedSpells::SpellTotallyTantalized);
                }

                bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
                {
                    switch (p_Target->GetEntry())
                    {
                        case eCreatures::NpcAegisOfAggramar:
                        case eCreatures::NpcAegisOfAggramarMephistroth:
                        case eCreatures::NpcNalasha:
                        case eCreatures::NpcGazerax:
                        case eCreatures::NpcAegwynn:
                        case eCreatures::NpcDulzak:
                        case eCreatures::NpcMaievShadowsong:
                        case eCreatures::NpcMaievShadowsongScornful:
                        case eCreatures::NpcIllidanStormrage:
                        case eCreatures::NpcImageOfKhadgar:
                        case eCreatures::NpcFelPortalDummy:
                        case eCreatures::NpcFelPortalDummy2:
                        case eCreatures::NpcHeaveCudgel:
                        case eCreatures::NpcSteamyRomanceSaga1:
                        case eCreatures::NpcSteamyRomanceSaga2:
                        case eCreatures::NpcSteamyRomanceSaga3:
                        case eCreatures::NpcSteamyRomanceSaga4:
                        case eCreatures::NpcBuddingFlower:
                        {
                            return false;
                        }

                        case eCreatures::BossAgronox:
                        case eCreatures::BossTrashBiteTheScornful:
                        case eCreatures::BossDomatrax:
                        case eCreatures::BossMephistroth:
                        case eCreatures::NpcFelSpike:
                        case eCreatures::NpcShadowOfMephistroth:
                        case eCreatures::NpcMephistrothDreadwing:
                        case eCreatures::NpcSatiricalBook:
                        case eCreatures::NpcAnimatedBook:
                        case eCreatures::NpcBiographyBook:
                        case eCreatures::NpcHellblazeImp:
                        case eCreatures::NpcHellblazeFelguard:
                        case eCreatures::NpcHellblazeMistress:
                        case eCreatures::NpcSearingOverfiend:
                        case eCreatures::NpcFulminatingLasher:
                        case eCreatures::NpcSucculentLasher:
                        case eCreatures::NpcChokingVines:
                        {
                            return false;
                        }

                        default: return true;
                    }

                    return true;
                }

                void OnCreatureCreate(Creature* p_Creature) override
                {
                    if (p_Creature == nullptr)
                        return;

                    if (IsChallenge() && p_Creature->IsHostileToPlayers())
                    {
                        if (Aura* l_Might = p_Creature->GetAura(ChallengeSpells::ChallengersMight))
                            l_Might->Remove();

                        p_Creature->DelayedCastSpell(p_Creature, ChallengeSpells::ChallengersMight, true, IN_MILLISECONDS + 500);
                    }

                    switch (p_Creature->GetEntry())
                    {
                        case eCreatures::BossDomatrax:
                        case eCreatures::BossMephistroth:
                        case eCreatures::BossTrashBiteTheScornful:
                        {
                            m_BossesIntro.push_back(CreatureIntro(p_Creature->GetEntry(), EncounterState::NOT_STARTED));
                            m_EncounterGuids.push_back(CreatureInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                            break;
                        }
                        case eCreatures::NpcDreadwing:
                        {
                            for (auto& l_Itr : g_DreadBastSummonPos)
                            {
                                if (p_Creature->IsNearPosition(&l_Itr, 5.f))
                                {
                                    p_Creature->SetVisible(false);
                                    p_Creature->SetReactState(REACT_PASSIVE);
                                    return;
                                }
                            }

                            break;
                        }
                        case eCreatures::NpcGazerax:
                        case eCreatures::NpcNalasha:
                        {
                            p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                            p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                            m_EncounterGuids.push_back(CreatureInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                            break;
                        }
                        case eCreatures::NpcDulzak:
                        case eCreatures::BossAgronox:
                        case eCreatures::NpcImageOfKhadgar:
                        case eCreatures::NpcMephistrothIntro:
                        case eCreatures::NpcAegwynn:
                        case eCreatures::NpcDreadlordbatStalker:
                        case eCreatures::NpcMaievShadowsongScornful:
                        case eCreatures::NpcFelPortalImps:
                        case eCreatures::NpcAegisOfAggramarMephistroth:
                        {
                            m_EncounterGuids.push_back(CreatureInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                            break;
                        }
                        case eCreatures::NpcIllidanStormrage:
                        case eCreatures::NpcMaievShadowsong:
                        {
                            const Position l_KhadgarPos = { -594.755f, 2526.8f, 532.455f };

                            if (p_Creature->IsNearPosition(&l_KhadgarPos, 10.f))
                            {
                                m_EncounterGuids.push_back(CreatureInfo(p_Creature->GetEntry(), p_Creature->GetGUID()));
                                p_Creature->SetVisible(false);

                                if (GetBossState(eData::DataDomatrax) == EncounterState::DONE && p_Creature->IsAIEnabled)
                                    p_Creature->GetAI()->DoAction(eSharedActions::ActionDomatraxKilled);
                            }
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
                        case eCreatures::NpcGazerax:
                        {
                            SetData(eCreatures::BossTrashBiteTheScornful, EncounterState::DONE);
                            break;
                        }

                        case eCreatures::BossTrashBiteTheScornful:
                        {
                            Creature* l_Portal = instance->GetCreature(GetData64(eCreatures::NpcFelPortalImps));

                            if (l_Portal != nullptr && l_Portal->IsAIEnabled)
                                l_Portal->GetAI()->DoAction(eSharedActions::ActionScornfulKilled);

                            break;
                        }

                        case eCreatures::NpcDreadwing:
                        {
                            if (p_Unit->FindNearestCreature(eCreatures::NpcDreadlordbatStalker, 100.f) && GetBossState(eData::DataDomatrax) == DONE)
                            {
                                m_BatsDead++;

                                if (m_BatsDead >= 4)
                                {
                                    Creature* l_Illidan = instance->GetCreature(GetData64(eCreatures::NpcIllidanStormrage));

                                    if (l_Illidan != nullptr && l_Illidan->IsAIEnabled)
                                        l_Illidan->GetAI()->DoAction(eSharedActions::ActionDomatraxKilled);

                                    SetData(eCreatures::BossMephistroth, EncounterState::DONE);
                                }
                            }
                            break;
                        }

                        default: break;
                    }
                }

                uint64 GetData64(uint32 p_Type) override
                {
                    auto l_Itr = std::find_if(m_EncounterGuids.begin(), m_EncounterGuids.end(), [&p_Type] (CreatureInfo& p_Itr)
                    {
                        return p_Itr.first == p_Type;
                    });

                    if (l_Itr != m_EncounterGuids.end())
                        return l_Itr->second;

                    return 0;
                }

                uint32 GetData(uint32 p_Type) override
                {
                    auto l_Itr = std::find_if(m_BossesIntro.begin(), m_BossesIntro.end(), [&p_Type] (CreatureIntro& p_Itr)
                    {
                        return p_Itr.first == p_Type;
                    });

                    if (l_Itr != m_BossesIntro.end())
                        return l_Itr->second;

                    auto l_Itr2 = std::find_if(m_DataEvents.begin(), m_DataEvents.end(), [&p_Type] (DataEvent& p_Itr)
                    {
                        return p_Itr.first == p_Type;
                    });

                    if (l_Itr2 != m_DataEvents.end())
                        return l_Itr2->second;

                    return 0;
                }

                void ActivateBats()
                {
                    m_BatsDead = 0;

                    Creature* l_Stalker = instance->GetCreature(GetData64(eCreatures::NpcDreadlordbatStalker));

                    if (l_Stalker != nullptr)
                    {
                        std::list<Creature*> l_Bats;
                        l_Stalker->GetCreatureListWithEntryInGrid(l_Bats, eCreatures::NpcDreadwing, 100.f);

                        for (auto & l_Itr : l_Bats)
                        {
                            if (l_Itr == nullptr)
                                continue;

                            l_Itr->SetVisible(true);
                            l_Itr->SetReactState(REACT_AGGRESSIVE);
                        }
                    }
                }

                bool SetBossState(uint32 p_Id, EncounterState p_State) override
                {
                    if (!InstanceScript::SetBossState(p_Id, p_State))
                        return false;

                    if (p_Id == eData::DataMephistroth)
                    {
                        Creature* l_Aegis = instance->GetCreature(GetData64(eCreatures::NpcAegisOfAggramarMephistroth));

                        if (p_State == EncounterState::DONE)
                        {
                            Creature* l_Aegwynn = instance->GetCreature(GetData64(eCreatures::NpcAegwynn));
                            Creature* l_Maiev = instance->GetCreature(GetData64(eCreatures::NpcMaievShadowsong));
                            Creature* l_Illidan = instance->GetCreature(GetData64(eCreatures::NpcIllidanStormrage));

                            if (l_Aegwynn != nullptr && l_Aegwynn->IsAIEnabled)
                                l_Aegwynn->GetAI()->DoAction(eSharedActions::ActionMephistrothKilled);

                            if (l_Maiev != nullptr && l_Maiev->IsAIEnabled)
                                l_Maiev->GetAI()->DoAction(eSharedActions::ActionMephistrothKilled);

                            if (l_Illidan != nullptr && l_Illidan->IsAIEnabled)
                                l_Illidan->GetAI()->DoAction(eSharedActions::ActionMephistrothKilled);

                            HandleGameObject(m_AegisGobGUID, true, nullptr);

                            if (l_Aegis != nullptr)
                            {
                                l_Aegis->SetHomePosition(g_AegisOfAggramarPos);
                                l_Aegis->NearTeleportTo(g_AegisOfAggramarPos);
                                l_Aegis->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            }
                        }
                        else if (p_State == EncounterState::FAIL)
                        {
                            if (l_Aegis != nullptr)
                            {
                                l_Aegis->SetVisible(true);
                                l_Aegis->NearTeleportTo(l_Aegis->GetHomePosition());
                            }
                        }
                    }
                    else if (p_Id == eData::DataDomatrax)
                    {
                        Creature* l_Aegis = instance->GetCreature(GetData64(eCreatures::NpcAegisOfAggramarMephistroth));

                        if (p_State == EncounterState::DONE)
                        {
                            if (l_Aegis != nullptr)
                            {
                                l_Aegis->SetVisible(true);
                                l_Aegis->NearTeleportTo(l_Aegis->GetHomePosition());
                            }

                            Creature* l_Stalker = instance->GetCreature(GetData64(eCreatures::NpcDreadlordbatStalker));
                            
                            ActivateBats();
                        }
                        else
                        {
                            if (l_Aegis != nullptr)
                            {
                                l_Aegis->SetVisible(false);
                                l_Aegis->NearTeleportTo(l_Aegis->GetHomePosition());
                            }
                        }
                    }

                    return true;
                }

                void SetData(uint32 p_Data, uint32 p_State) override
                {
                    switch (p_Data)
                    {
                        case eCreatures::BossDomatrax:
                        case eCreatures::BossMephistroth:
                        case eCreatures::BossTrashBiteTheScornful:
                        {
                            auto l_Itr = std::find_if(m_BossesIntro.begin(), m_BossesIntro.end(), [&p_Data](CreatureIntro& p_Itr)
                            {
                                return p_Itr.first == p_Data;
                            });

                            if (l_Itr != m_BossesIntro.end())
                            {
                                l_Itr->second = EncounterState(p_State);

                                if (p_State == EncounterState::DONE)
                                {
                                    Creature* l_Boss = instance->GetCreature(GetData64(p_Data));

                                    if (l_Boss != nullptr && l_Boss->IsAIEnabled)
                                        l_Boss->GetAI()->DoAction(eSharedActions::ActionBossIntro);
                                }
                            }

                            break;
                        }

                        case eData::DataIllidanMaievIntro:
                        case eData::DataIllidanMaievStairs:
                        case eData::DataIllidanMaievKhadgar:
                        {
                            auto l_Itr = std::find_if(m_DataEvents.begin(), m_DataEvents.end(), [&p_Data] (DataEvent& p_Itr)
                            {
                                return p_Itr.first == p_Data;
                            });

                            if (l_Itr != m_DataEvents.end())
                                l_Itr->second = EncounterState(p_State);

                            break;
                        }

                        default: break;
                    }

                    if (p_State == EncounterState::DONE)
                        SaveToDB();
                }

                void Load(char const* p_Data) override
                {
                    std::istringstream l_Data(p_Data);
                    uint32 l_State;

                    for (uint8 l_Itr = 0; l_Itr < g_EncounterCount; ++l_Itr)
                    {
                        l_Data >> l_State;

                        if (l_State != EncounterState::DONE)
                            l_State = EncounterState::NOT_STARTED;

                        InstanceScript::SetBossState(l_Itr, EncounterState(l_State));
                    }

                    for (CreatureIntro & l_Itr : m_BossesIntro)
                    {
                        l_Data >> l_State;

                        if (l_State != EncounterState::DONE)
                            l_State = EncounterState::NOT_STARTED;

                        SetData(l_Itr.first, l_State);
                    }

                    for (DataEvent & l_Itr : m_DataEvents)
                    {
                        l_Data >> l_State;

                        if (l_State != EncounterState::DONE)
                            l_State = EncounterState::NOT_STARTED;

                        SetData(l_Itr.first, l_State);
                    }
                }

                std::string GetSaveData() override
                {
                    std::ostringstream l_Data;

                    l_Data << GetBossSaveData();

                    for (CreatureIntro const & p_Itr : m_BossesIntro)
                        l_Data << p_Itr.second << ' ';

                    for (DataEvent const & p_Itr : m_DataEvents)
                        l_Data << p_Itr.second << ' ';


                    return l_Data.str();
                }

                bool CheckRequiredBosses(uint32 p_BossID, Player const* p_Player) const override
                {
                    if (!InstanceScript::CheckRequiredBosses(p_BossID, p_Player))
                        return false;

                    switch (p_BossID)
                    {
                        case eCreatures::BossTrashBiteTheScornful:
                        {
                            return GetBossState(eData::DataAgronox) == EncounterState::DONE;
                        }

                        case eCreatures::BossDomatrax:
                        {
                            return GetBossState(eData::DataTrashbiteTheScornful) == EncounterState::DONE;
                        }

                        case eCreatures::BossMephistroth:
                        {
                            return GetBossState(eData::DataDomatrax) == EncounterState::DONE;
                        }

                        default: break;
                    }

                    return true;
                }

                void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
                {
                    WorldSafeLocsEntry const* l_Loc = nullptr;

                    if (GetBossState(eData::DataAgronox) == EncounterState::DONE)
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocAgronox);
                    else if (GetBossState(eData::DataTrashbiteTheScornful) == EncounterState::DONE)
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocScornful);
                    else if (GetData(eData::DataDomatrax) == EncounterState::DONE)
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocDomatrax);
                    else
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocEntrance);

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
                    InstanceScript::Update(p_Diff);

                    Map::PlayerList const& l_Players = instance->GetPlayers();

                    for (auto & l_Itr : l_Players)
                    {
                        Player* l_Player = l_Itr.getSource();

                        if (l_Player == nullptr)
                            continue;

                        if (l_Player->isGameMaster())
                            continue;

                        if (!l_Player->IsBeingTeleported() && l_Player->GetPositionZ() < 330.f)
                        {
                            uint32 l_Loc = eLocations::LocEntrance;

                            if (GetBossState(eData::DataAgronox) == EncounterState::DONE)
                                l_Loc = eLocations::LocAgronox;
                            else if (GetBossState(eData::DataTrashbiteTheScornful) == EncounterState::DONE)
                                l_Loc = eLocations::LocScornful;
                            else if (GetData(eData::DataDomatrax) == EncounterState::DONE)
                                l_Loc = eLocations::LocDomatrax;

                            l_Player->NearTeleportTo(l_Loc);
                        }
                    }
                }

            private:
                std::vector<CreatureInfo> m_EncounterGuids;
                std::vector<CreatureIntro> m_BossesIntro;
                std::vector<DataEvent> m_DataEvents;
                uint8 m_BatsDead;
                uint64 m_AegisGobGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_cathedral_of_eternal_night_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_cathedral_of_eternal_night()
{
    new instance_cathedral_of_eternal_night();
}
#endif
