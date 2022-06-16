////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Challenge.h"
#include "ChallengeModePackets.h"
#include "ChallengeMgr.h"
#include "Map.h"
#include "Chat.h"
#include "Group.h"
#include "CreatureAI.h"

Challenge::Challenge(Map* p_Map, Player* p_Player, uint32 p_InstanceID, uint32 p_MapChallengeID) : InstanceScript(p_Map)
{
    if (!p_Player)
    {
        m_CanRun = false;
        return;
    }

    m_CanRun         = true;
    m_Creator        = p_Player->GetGUID();
    m_InstanceID     = p_InstanceID;
    m_ChallengeTimer = 0;
    m_QuakeTimer     = 0;
    m_Complete       = false;
    m_Run            = false;
    m_Item           = nullptr;
    m_InstanceScript = nullptr;
    m_deathCounter   = 0;

    m_Map    = p_Map;
    m_MapID = m_Map->GetId();

    auto l_Group = p_Player->GetGroup();

    m_ChallengeEntry = l_Group ? l_Group->m_challengeEntry : p_Player->m_challengeEntry;
    if (!m_ChallengeEntry)
    {
        ChatHandler(p_Player).PSendSysMessage("Error: Is not a challenge map.");
        m_CanRun = false;
        return;
    }

    for (uint8 i = 0; i < CHALLENGE_TIMER_LEVEL_3; i++)
        m_ChestTimers[i] = m_ChallengeEntry->ChestTimer[i];

    m_Challengers.insert(m_Creator);

    if (l_Group)
    {
        m_CanRun = false;
        m_ownerGuid = l_Group->m_ChallengeOwner;
        m_itemGuid = l_Group->m_ChallengeItem;

        if (Player* l_KeyOwner = ObjectAccessor::FindPlayer(m_ownerGuid))
        {
            m_Item = l_KeyOwner->GetItemByGuid(m_itemGuid);
            if (m_Item != nullptr)
                m_CanRun = true;
        }

        l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Member)
        {
            m_Challengers.insert(l_Member->guid);
        });

        if (!m_CanRun)
        {
            ChatHandler(p_Player).PSendSysMessage("Error: Key not found.");
            return;
        }
    }
    else
    {
        m_CanRun = false;
        m_ownerGuid = p_Player->GetGUID();
        if (auto key = p_Player->GetItemByEntry(138019))
        {
            m_itemGuid = key->GetGUID();
            m_Item = key;
            m_CanRun = true;
            m_Challengers.insert(m_ownerGuid);
        }
    }

    if (!m_Item)
    {
        ChatHandler(p_Player).PSendSysMessage("Error: Key not found.");
        m_CanRun = false;
        return;
    }

    m_Affixes.fill(0);
    m_ChallengeLevel = m_Item->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL);
    m_RewardLevel = CHALLENGE_NOT_IN_TIMER;

    if (m_ChallengeLevel > 3)
        m_Affixes[0] = m_Item->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1);
    if (m_ChallengeLevel > 6)
        m_Affixes[1] = m_Item->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2);
    if (m_ChallengeLevel > 9)
        m_Affixes[2] = m_Item->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3);

    for (auto const& l_Affix : m_Affixes)
        m_AffixesTest.set(l_Affix);

    m_OutOfTime = false;
}

Challenge::~Challenge() { }

void Challenge::OnPlayerEnterForScript(Player* p_Player)
{
    if (!p_Player)
        return;

    if (m_Run)
        p_Player->AddDelayedEvent([p_Player, _c=this]() -> void { if (_c && p_Player) _c->SendDeathCount(p_Player); }, 500);
        
    //if (m_Challengers.size() < 5)
    //    m_Challengers.insert(p_Player->GetGUID());

    p_Player->CastSpell(p_Player, ChallengeSpells::ChallengersBurden, true);
}

void Challenge::OnPlayerLeaveForScript(Player* p_Player)
{
    if (!p_Player)
        return;

    p_Player->RemoveAura(ChallengeSpells::ChallengersBurden);
}

void Challenge::OnPlayerDiesForScript(Player* p_Player)
{
    if (!p_Player)
        return;

    AuraEffect* l_AuraEff = p_Player->GetAuraEffect(ChallengeSpells::ChallengersBurden, EFFECT_0);
    if (!l_AuraEff)
        return;

    IncreaseDeathCounter();
    ModChallengeTimer(l_AuraEff->GetAmount());
    SendStartElapsedTimer();
    SendDeathCount();
}

void Challenge::OnCreatureCreateForScript(Creature* p_Creature)
{
    if (!p_Creature)
        return;

    if (p_Creature->IsHostileToPlayers() && !p_Creature->isTrigger() && p_Creature->GetEntry() != 27829) /// DK's Gargoyle
        p_Creature->DelayedCastSpell(p_Creature, ChallengeSpells::ChallengersMight, true, 1500);

    constexpr uint32 l_TeemingPhaseMask = 1024;

    if (p_Creature->GetPhaseMask() == l_TeemingPhaseMask)
    {
        if (HasAffix(Affixes::Teeming))
            p_Creature->SetPhaseMask(1, true);
        else
            p_Creature->SetPhaseMask(0, true);
    }
}

void Challenge::OnCreatureUpdateDifficulty(Creature* p_Creature)
{
    if (!p_Creature)
        return;

    if (p_Creature->IsHostileToPlayers() && !p_Creature->isTrigger() && p_Creature->GetEntry() != 27829) /// DK's Gargoyle
        p_Creature->DelayedCastSpell(p_Creature, ChallengeSpells::ChallengersMight, true, 1500);

    constexpr uint32 l_TeemingPhaseMask = 1024;

    if (p_Creature->GetPhaseMask() == l_TeemingPhaseMask)
    {
        if (HasAffix(Affixes::Teeming))
            p_Creature->SetPhaseMask(1, true);
        else
            p_Creature->SetPhaseMask(0, true);
    }
}

void Challenge::OnCreatureRemoveForScript(Creature* /*p_Creature*/) { }

void Challenge::EnterCombatForScript(Creature* p_Creature, Unit* /*p_Enemy*/)
{
    if (!p_Creature || p_Creature->isPet() || p_Creature->isTrigger() || p_Creature->IsPetGuardianStuff())
        return;

    if (!p_Creature->HasAura(ChallengeSpells::ChallengersMight) && p_Creature->GetEntry() != 27829) /// DK's Gargoyle
       p_Creature->AddAura(ChallengeSpells::ChallengersMight, p_Creature);
}

void Challenge::CreatureDiesForScript(Creature* p_Creature, Unit* /*p_Killer*/)
{
    if (!p_Creature)
        return;

    if (p_Creature->IsDungeonBoss() || (p_Creature->GetInstanceScript() && p_Creature->GetInstanceScript()->GetEncounterIDForBoss(p_Creature)))
        return;

    if (p_Creature->IsPetGuardianStuff())
        return;

    if (p_Creature->GetCreatureTemplate()->type == CREATURE_TYPE_CRITTER)
        return;

    if (!p_Creature->IsHostileToPlayers())
        return;

    if (p_Creature->GetDBTableGUIDLow())
    {
        InstanceScript* l_Instance = GetInstanceScript();

        if (l_Instance == nullptr)
            return;

        if (HasAffix(Affixes::Bolstering) && l_Instance->IsValidTargetForAffix(p_Creature, Affixes::Bolstering))
            p_Creature->CastSpell(p_Creature, ChallengeSpells::ChallengerBolstering);

        if (HasAffix(Affixes::Sanguine) && l_Instance->IsValidTargetForAffix(p_Creature, Affixes::Sanguine))
            p_Creature->CastSpell(p_Creature, ChallengeSpells::ChallengerSanguine);

        if (HasAffix(Affixes::Bursting) && l_Instance->IsValidTargetForAffix(p_Creature, Affixes::Bursting))
            p_Creature->CastSpell(p_Creature, ChallengeSpells::ChallengerBurst);
    }
}

void Challenge::Update(uint32 p_Diff)
{
    m_Functions.Update(p_Diff);

    if (m_Complete || !m_Run) ///< Stop update if complete
        return;

    if (HasAffix(Affixes::Quaking))
    {
        if (m_QuakeTimer <= p_Diff)
        {
            Map::PlayerList const& l_Players = m_Map->GetPlayers();
            for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
            {
                if (Player* l_Player = l_Itr->getSource())
                    if (!l_Player->isGameMaster())
                         l_Player->AddAura(ChallengeSpells::ChallengerQuake, l_Player);
            }
            uint32 quakingRandom[3] = { 20, 40, 60 };
            m_QuakeTimer = quakingRandom[urand(0, 2)] * IN_MILLISECONDS;
        }
        else
            m_QuakeTimer -= p_Diff;
    }

    m_ChallengeTimer += p_Diff;

    if (!m_OutOfTime && GetChallengeProgresTime() > m_ChallengeEntry->ChestTimer[0])
        m_OutOfTime = true;
}

void Challenge::ModifyKeystoneLevel(uint8 p_Level)
{
    m_Item = nullptr;

    if (Player* keyOwner = ObjectAccessor::FindPlayer(m_ownerGuid))
        m_Item = keyOwner->GetItemByGuid(m_itemGuid);

    if (m_Item)
    {
        m_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL, p_Level);

        m_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, p_Level > 3 ? sChallengeMgr->GetCurrentAffixes()[0] : 0);
        m_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, p_Level > 6 ? sChallengeMgr->GetCurrentAffixes()[1] : 0);
        m_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, p_Level > 9 ? sChallengeMgr->GetCurrentAffixes()[2] : 0);

        m_Item->SetState(ITEM_CHANGED);
    }
}

void Challenge::Start()
{
    if (!m_CanRun)
        return;

    SetWorldStates(true);

    BroadcastPacket(WorldPackets::ChallengeMode::ChallengeModeReset(m_MapID).Write());

    SendStartTimer();
    SendChallengeModeStart();

    /// Same behavior as RemoveArenaSpellCooldowns - Remove all cooldowns <= 10min
    DoRemoveSpellCooldownOnPlayers();

    if (Player* l_Owner = sObjectAccessor->FindPlayer(m_ownerGuid))
    {
        std::list<Creature*> l_Portals;

        l_Owner->GetCreatureListWithEntryInGridAppend(l_Portals, 54569, 250.f);
        l_Owner->GetCreatureListWithEntryInGridAppend(l_Portals, 47319, 250.f);
        l_Owner->GetCreatureListWithEntryInGridAppend(l_Portals, 59271, 250.f);
        l_Owner->GetCreatureListWithEntryInGridAppend(l_Portals, 59262, 250.f);

        for (Creature* l_Itr : l_Portals)
            l_Itr->DespawnOrUnsummon();
    }

    ModifyKeystoneLevel(std::max((uint32)2, m_ChallengeLevel - 1));

    if (m_Item == nullptr)
        CharacterDatabase.PExecute("UPDATE item_instance SET challengeLevel = GREATEST(2,challengeLevel -1) WHERE owner_guid = %u AND guid = %u", GUID_LOPART(m_ownerGuid), GUID_LOPART(m_itemGuid));

    m_Functions.AddFunction([this]() -> void
    {
        SendStartElapsedTimer();

        for (uint64 l_Guid : GetInstanceScript()->m_ChallengeDoorGuids)
        {
            if (GameObject* l_ChallengeDoor = sObjectAccessor->FindGameObject(l_Guid))
                l_ChallengeDoor->Delete();
        }

        m_Run = true;

        GetInstanceScript()->StartChallengeCombatResurrection();
        GetInstanceScript()->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_GAIN_COMBAT_RESURRECTION_CHARGE);
        GetInstanceScript()->OnStartChallenge(m_Item);

        if (HasAffix(Affixes::Quaking))
        {
            uint32 quakingRandom[3] = { 20, 40, 60 };
            m_QuakeTimer = quakingRandom[urand(0, 2)] * IN_MILLISECONDS;
        }

    }, m_Functions.CalculateTime(10 * IN_MILLISECONDS));
}

void Challenge::Complete()
{
    m_Complete = true;

    HitTimer();

    WorldPacket l_Packet(SMSG_STOP_ELAPSED_TIMER);
    l_Packet << uint32(WORLD_TIMER_TYPE_CHALLENGE_MODE);
    l_Packet.WriteBit(false);
    l_Packet.FlushBits();
    BroadcastPacket(&l_Packet);

    WorldPackets::ChallengeMode::ChallengeModeComplete l_Complete;
    l_Complete.ChallengeID = m_ChallengeEntry ? m_ChallengeEntry->ID : 0;
    l_Complete.MapID = m_MapID;
    l_Complete.CompletionMilliseconds = GetChallengeProgresTime() * IN_MILLISECONDS;
    l_Complete.RewardLevel = m_RewardLevel;
    l_Complete.IsCompletedInTimer = !m_OutOfTime;
    BroadcastPacket(l_Complete.Write());

    if (GetKeystone() != nullptr)
    {
        m_Item = GetKeystone();

        MapChallengeModeEntry const* l_MapChallenge = nullptr;
        while (!l_MapChallenge)
            l_MapChallenge = JadeCore::Containers::SelectRandomContainerElement(*GetLegionChallengeModeEntries()).second;

        m_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID, l_MapChallenge->ID);

        if (!m_OutOfTime)
            ModifyKeystoneLevel(std::min(m_ChallengeLevel + m_RewardLevel, 35u));

        if (Player* l_KeyOwner = GetKeystoneOwner())
            m_Item->SetState(ITEM_CHANGED, l_KeyOwner);
    }
    if (auto owner = GetKeystoneOwner())
    {
        auto group = owner->GetGroup();
        if (group)
            group->m_challengeEntry = nullptr;
        else
            owner->m_challengeEntry = nullptr;
    }

    ChallengeData* l_ChallengeData = new ChallengeData;
    l_ChallengeData->ID             = sObjectMgr->GetNewScenarioID();
    l_ChallengeData->MapID          = m_MapID;
    l_ChallengeData->ChallengeID    = m_ChallengeEntry ? m_ChallengeEntry->ID : 0;
    l_ChallengeData->RecordTime     = GetChallengeProgresTime();
    l_ChallengeData->Date           = time(nullptr);
    l_ChallengeData->ChallengeLevel = m_ChallengeLevel;
    l_ChallengeData->TimerLevel     = m_RewardLevel;
    l_ChallengeData->Affixes        = m_Affixes;
    l_ChallengeData->GuildID        = 0;
    l_ChallengeData->ChestID        = GUID_ENPART(GetInstanceScript()->m_ChallengeChestFirst);

    std::map<uint32, uint32> l_GuildCounter;
    Map::PlayerList const& l_Players = m_Map->GetPlayers();
    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        if (Player* l_Player = l_Itr->getSource())
        {
            if (l_Player->isGameMaster())
                continue;

            l_Player->CombatStop();

            ChallengeMember l_Member;
            l_Member.Guid           = l_Player->GetGUID();
            l_Member.SpecId         = l_Player->GetUInt32Value(PLAYER_FIELD_CURRENT_SPEC_ID);
            l_Member.Date           = time(nullptr);
            l_Member.ChallengeLevel = m_ChallengeLevel;
            l_Member.ChestID        = GUID_ENPART(GetInstanceScript()->m_ChallengeChestFirst);
            l_Member.GuildId        = l_Player->GetGuildId();

            if (l_Player->GetGuildId())
                l_GuildCounter[l_Player->GetGuildId()]++;

            l_ChallengeData->member.insert(l_Member);

            if (sChallengeMgr->CheckBestMemberMapId(l_Member.Guid, l_ChallengeData))
                SendChallengeModeNewPlayerRecord(l_Player);

            SendChallengeModeMapStatsUpdate(l_Player);

            l_Player->UpdateCriteria(CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE, m_MapID, m_RewardLevel);
            l_Player->RemoveAura(ChallengeSpells::ChallengersBurden);

            m_LuckyChallengersChest[252680].insert(l_Player->GetGUID());
        }
    }

    /// If the challenge is completed, the chest contains at least 2 items
    uint32 l_ItemGearReward = m_RewardLevel >= CHALLENGE_TIMER_LEVEL_1 ? 3 : 2;

    /// Level 15 challenge completed in time give 4 items
    if (m_ChallengeLevel >= 15)
    {
        uint32 l_TotalPieces = 2;
        uint32 l_ExtraPieceChance = 0.0f;

        switch (m_ChallengeLevel)
        {
            case 15:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 2 : 3;
                break;
            case 16:
                l_ExtraPieceChance = 40.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 2 : 3;
                break;
            case 17:
                l_ExtraPieceChance = 80.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 2 : 3;
                break;
            case 18:
                l_ExtraPieceChance = 20.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 3 : 4;
                break;
            case 19:
                l_ExtraPieceChance = 60.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 3 : 4;
                break;
            case 20:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 4 : 5;
                break;
            case 21:
                l_ExtraPieceChance = 40.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 4 : 5;
                break;
            case 22:
                l_ExtraPieceChance = 80.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 4 : 5;
                break;
            case 23:
                l_ExtraPieceChance = 20.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 5 : 6;
                break;
            case 24:
                l_ExtraPieceChance = 60.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 5 : 6;
                break;
            case 25:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 6 : 7;
                break;
            case 26:
                l_ExtraPieceChance = 40.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 6 : 7;
                break;
            case 27:
                l_ExtraPieceChance = 80.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 6 : 7;
                break;
            case 28:
                l_ExtraPieceChance = 20.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 7 : 8;
                break;
            case 29:
                l_ExtraPieceChance = 60.0f;
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 7 : 8;
                break;
            case 30:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 8 : 9;
                break;
            case 31:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 8 : 9;
                l_ExtraPieceChance = 40.0f;
                break;
            case 32:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 8 : 9;
                l_ExtraPieceChance = 80.0f;
                break;
            case 33:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 9 : 10;
                l_ExtraPieceChance = 20.0f;
                break;
            case 34:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 9 : 10;
                l_ExtraPieceChance = 60.0f;
                break;
            case 35:
                l_TotalPieces = m_RewardLevel == ChallengeMode::CHALLENGE_NOT_IN_TIMER ? 10 : 11;
                break;
            default:
                break;
        }

        if (roll_chance_i(l_ExtraPieceChance))
            l_TotalPieces++;

        l_ItemGearReward = l_TotalPieces;
    }

    JadeCore::Containers::RandomResizeSet(m_LuckyChallengersChest[252680], l_ItemGearReward);

    uint8 l_Count = l_ItemGearReward;
    uint8 l_Extra = 0;
    std::list<Player*> l_PlayerList;

    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        if (Player* l_Player = l_Itr->getSource())
        {
            if (l_Player->isGameMaster())
                continue;

            if (!IsLuckyChallengerForChest(l_Player->GetGUID(), 252680))
                continue;

            if (l_Count == 0)
                break;

            l_PlayerList.push_back(l_Player);
        }
    }

    JadeCore::RandomResizeList(l_PlayerList, l_PlayerList.size());

    while (l_ItemGearReward >= l_PlayerList.size())
    {
        std::for_each(l_PlayerList.begin(), l_PlayerList.end(), [&](Player* l_Player)
        {
            m_AssignedLoot[l_Player->GetGUID()]++;
        });

        l_ItemGearReward -= l_PlayerList.size();
    }

    while (l_ItemGearReward > 0)
    {
        for (Player* l_Player : l_PlayerList)
        {
            if (roll_chance_i(50))
            {
                m_AssignedLoot[l_Player->GetGUID()]++;
                l_ItemGearReward--;

                if (l_ItemGearReward == 0)
                    break;
            }
        }
    }

    for (auto const& l_Itr : l_GuildCounter)
        if (l_Itr.second >= 3)
            l_ChallengeData->GuildID = l_Itr.first;

    sChallengeMgr->SetChallengeMapData(l_ChallengeData->ID, l_ChallengeData);
    sChallengeMgr->CheckBestMapId(l_ChallengeData);
    sChallengeMgr->CheckBestGuildMapId(l_ChallengeData);
    sChallengeMgr->SaveChallengeToDB(l_ChallengeData);
}

void Challenge::HitTimer()
{
    if (GetChallengeProgresTime() < m_ChestTimers[2])
        m_RewardLevel = CHALLENGE_TIMER_LEVEL_3; ///< 3 level + 3 items + 50% chance have nother item
    else if (GetChallengeProgresTime() < m_ChestTimers[1])
        m_RewardLevel = CHALLENGE_TIMER_LEVEL_2; ///< 2 level + 3 items + 25% chance have another item
    else if (GetChallengeProgresTime() < m_ChestTimers[0])
        m_RewardLevel = CHALLENGE_TIMER_LEVEL_1; ///< 1 level + 3 items
    else
        m_RewardLevel = CHALLENGE_NOT_IN_TIMER; ///< 2 items

    if (!instance || !GetInstanceScript())
        return;

    if (GameObject* l_Chest = instance->GetGameObject(GetInstanceScript()->m_ChallengeChestFirst))
        l_Chest->SetRespawnTime(7 * DAY);

    auto l_Objects = instance->GetObjectsOnMap();
    for (auto l_Itr : l_Objects)
    {
        Creature* l_Creature = l_Itr->ToCreature();

        if (!l_Creature || l_Creature->GetSpellModOwner() || l_Creature->isDead())
            continue;

        if (l_Creature->IsDungeonBoss())
            continue;

        l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
        l_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags(eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC));

        if (l_Creature->isInCombat())
        {
            l_Creature->CombatStop(true);

            if (l_Creature->IsAIEnabled)
                l_Creature->AI()->EnterEvadeMode();
        }
    }
}

void Challenge::BroadcastPacket(WorldPacket const* p_Data) const
{
    Map::PlayerList const& l_Players = m_Map->GetPlayers();
    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        if (Player* l_Player = l_Itr->getSource())
            l_Player->SendDirectMessage(p_Data);
    }
}

uint32 Challenge::GetChallengeProgresTime()
{
    if (!m_ChallengeTimer)
        return 0;

    return m_ChallengeTimer / IN_MILLISECONDS;
}

void Challenge::ModChallengeTimer(uint32 p_Timer)
{
    if (!p_Timer)
        return;

    m_ChallengeTimer += p_Timer * IN_MILLISECONDS;
}

void Challenge::ResetGo()
{
    for (uint64 p_Guid : GetInstanceScript()->m_ChallengeDoorGuids)
    {
        if (GameObject* l_ChallengeDoor = sObjectAccessor->FindGameObject(p_Guid))
            l_ChallengeDoor->SetGoState(GOState::GO_STATE_READY);
    }

    if (GameObject* l_ChallengeOrb = instance->GetGameObject(GetInstanceScript()->m_ChallengeOrbGuid))
    {
        l_ChallengeOrb->SetGoState(GOState::GO_STATE_READY);
        l_ChallengeOrb->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NODESPAWN);
    }
}

void Challenge::SendStartTimer(Player* p_Player)
{
    WorldPacket l_Packet(SMSG_START_TIMER);
    l_Packet << uint32(MiscChallengeData::ChallengeDelayTimer);
    l_Packet << uint32(MiscChallengeData::ChallengeDelayTimer);
    l_Packet << int32(WORLD_TIMER_TYPE_CHALLENGE_MODE);

    if (p_Player)
        p_Player->SendDirectMessage(&l_Packet);
    else
        BroadcastPacket(&l_Packet);
}

void Challenge::SendStartElapsedTimer(Player* p_Player)
{
    WorldPacket l_Packet(SMSG_START_ELAPSED_TIMER);
    l_Packet << uint32(WORLD_TIMER_TYPE_CHALLENGE_MODE);
    l_Packet << uint32(GetChallengeProgresTime());

    if (p_Player)
        p_Player->SendDirectMessage(&l_Packet);
    else
        BroadcastPacket(&l_Packet);
}

void Challenge::SendChallengeModeStart(Player* p_Player)
{
    WorldPackets::ChallengeMode::ChallengeModeStart l_Start;
    l_Start.MapID                 = m_MapID;
    l_Start.ChallengeID           = m_ChallengeEntry ? m_ChallengeEntry->ID : 0;
    l_Start.StartedChallengeLevel = m_ChallengeLevel;
    l_Start.IsKeyCharged          = true;
    l_Start.Affixes               = m_Affixes;

    if (p_Player)
        p_Player->SendDirectMessage(l_Start.Write());
    else
        BroadcastPacket(l_Start.Write());
}

void Challenge::SendChallengeModeNewPlayerRecord(Player* p_Player)
{
    WorldPackets::ChallengeMode::ChallengeModeNewPlayerRecord newRecord;
    newRecord.MapID = m_MapID;
    newRecord.CompletionMilliseconds = GetChallengeProgresTime();
    newRecord.StartedChallengeLevel = m_RewardLevel;

    if (p_Player)
        p_Player->SendDirectMessage(newRecord.Write());
}

void Challenge::SendChallengeModeMapStatsUpdate(Player* p_Player)
{
    ChallengeByMap* l_BestMap = sChallengeMgr->BestForMember(p_Player->GetGUID());
    if (!l_BestMap)
        return;

    ChallengeByMap::iterator l_Itr = l_BestMap->find(m_ChallengeEntry->ID);
    if (l_Itr == l_BestMap->end())
        return;

    ChallengeData* l_Best = l_Itr->second;
    if (!l_Best)
        return;

    WorldPackets::ChallengeMode::ChallengeModeMapStatsUpdate l_Update;
    l_Update.Stats.MapId = m_MapID;
    l_Update.Stats.BestCompletionMilliseconds = l_Best->RecordTime;
    l_Update.Stats.LastCompletionMilliseconds = GetChallengeProgresTime();
    l_Update.Stats.BestMedalDate = l_Best->Date;
    l_Update.Stats.Affixes = l_Best->Affixes;
    l_Update.Stats.ChallengeID = m_ChallengeEntry ? m_ChallengeEntry->ID : 0;

    ChallengeMemberList l_Members = l_Best->member;
    for (auto const& l_Itr : l_Members)
        l_Update.Stats.BestSpecID.emplace_back(l_Itr.SpecId);

    if (p_Player)
        p_Player->SendDirectMessage(l_Update.Write());
}

void Challenge::SummonWall(Player* p_Player)
{
    for (uint64 l_Guid : GetInstanceScript()->m_ChallengeDoorGuids)
    {
        if (GameObject* l_ChallengeDoor = sObjectAccessor->FindGameObject(l_Guid))
            l_ChallengeDoor->SetRespawnTime(7 * DAY);
    }
}

void Challenge::SetWorldStates(bool p_Enable)
{
    Map::PlayerList const& l_Players = m_Map->GetPlayers();
    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        if (Player* l_Player = l_Itr->getSource())
        {
            SetWorldStatesForPlayer(p_Enable, l_Player);
        }
    }
}

void Challenge::SetWorldStatesForPlayer(bool p_Enable, Player* p_Player)
{
    if (p_Enable)
    {
        uint32 l_ChallengeLevel = GetChallengeLevel();

        if (l_ChallengeLevel >= 2)
            p_Player->SetWorldState(InstanceWorldStates::MythicLevel2, 1);

        if (l_ChallengeLevel >= 5)
            p_Player->SetWorldState(InstanceWorldStates::MythicLevel5, 1);

        if (l_ChallengeLevel >= 10)
            p_Player->SetWorldState(InstanceWorldStates::MythicLevel10, 1);

        if (l_ChallengeLevel >= 15)
            p_Player->SetWorldState(InstanceWorldStates::MythicLevel15, 1);
    }
    else
    {
        p_Player->SetWorldState(InstanceWorldStates::MythicLevel2, 0);
        p_Player->SetWorldState(InstanceWorldStates::MythicLevel5, 0);
        p_Player->SetWorldState(InstanceWorldStates::MythicLevel10, 0);
        p_Player->SetWorldState(InstanceWorldStates::MythicLevel15, 0);
    }
}

void Challenge::SendDeathCount(Player* player/* = nullptr*/)
{
    WorldPackets::ChallengeMode::ChallengeModeUpdateDeathCount packet;
    packet.DeathCount = m_deathCounter;

    if (player)
        player->SendDirectMessage(packet.Write());
    else
        BroadcastPacket(packet.Write());
}