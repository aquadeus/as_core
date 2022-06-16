////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ChallengeMgr.h"
#include "QueryResult.h"
#include "DatabaseEnv.h"
#include "Challenge.h"

static const uint32 g_AffixesRotationCount = 12;
static Affixes const g_AffixesRotations[g_AffixesRotationCount][3] =
{
    { Affixes::Raging,     Affixes::Volcanic,  Affixes::Tyrannical },
    { Affixes::Teeming,    Affixes::Explosive, Affixes::Fortified  },
    { Affixes::Bolstering, Affixes::Grievous,  Affixes::Tyrannical },
    { Affixes::Sanguine,   Affixes::Volcanic,  Affixes::Fortified  },
    { Affixes::Bursting,   Affixes::Skittish,  Affixes::Tyrannical },
    { Affixes::Teeming,    Affixes::Quaking,   Affixes::Fortified  },
    { Affixes::Raging,     Affixes::Necrotic,  Affixes::Tyrannical },
    { Affixes::Bolstering, Affixes::Skittish,  Affixes::Fortified  },
    { Affixes::Teeming,    Affixes::Necrotic,  Affixes::Tyrannical },
    { Affixes::Sanguine,   Affixes::Grievous,  Affixes::Fortified  },
    { Affixes::Bolstering, Affixes::Explosive, Affixes::Tyrannical },
    { Affixes::Bursting,   Affixes::Quaking,   Affixes::Fortified  },

};

ChallengeMgr::ChallengeMgr()
{
    m_CurrentWeekAffixes.fill(0);

    uint32 l_CurrentWeeklyAffixesIndex = sWorld->getWorldState(WS_CHALLENGE_WEEKLY_AFFIXE_INDEX);

    m_CurrentWeekAffixes[0] = (uint32)g_AffixesRotations[l_CurrentWeeklyAffixesIndex][0];
    m_CurrentWeekAffixes[1] = (uint32)g_AffixesRotations[l_CurrentWeeklyAffixesIndex][1];
    m_CurrentWeekAffixes[2] = (uint32)g_AffixesRotations[l_CurrentWeeklyAffixesIndex][2];
}

ChallengeMgr::~ChallengeMgr()
{
    for (auto l_Itr : m_ChallengeMap)
        delete l_Itr.second;

    m_ChallengeMap.clear();
    m_ChallengesOfMember.clear();
    m_BestForMap.clear();
}

void ChallengeMgr::CheckBestMapId(ChallengeData* p_ChallengeData)
{
    if (!p_ChallengeData)
        return;

    if (!m_BestForMap[p_ChallengeData->ChallengeID] || m_BestForMap[p_ChallengeData->ChallengeID]->RecordTime > p_ChallengeData->RecordTime)
        m_BestForMap[p_ChallengeData->ChallengeID] = p_ChallengeData;
}

void ChallengeMgr::CheckBestGuildMapId(ChallengeData* p_ChallengeData)
{
    if (!p_ChallengeData)
        return;

    for (auto m : p_ChallengeData->member)
    {
        if (!m.GuildId)
            continue;

        if (!m_GuildBest[m.GuildId][p_ChallengeData->ChallengeID] || p_ChallengeData->ChallengeLevel > m_GuildBest[m.GuildId][p_ChallengeData->ChallengeID]->ChallengeLevel ||
            (p_ChallengeData->ChallengeLevel == m_GuildBest[m.GuildId][p_ChallengeData->ChallengeID]->ChallengeLevel &&
                m_GuildBest[m.GuildId][p_ChallengeData->ChallengeID]->RecordTime > p_ChallengeData->RecordTime))
            m_GuildBest[m.GuildId][p_ChallengeData->ChallengeID] = p_ChallengeData;
    }
}

bool ChallengeMgr::CheckBestMemberMapId(uint64 const& guid, ChallengeData* p_ChallengeData)
{
    bool l_IsBest = false;

    ChallengeByMap* l_BestChallengeMap = BestForMember(guid);

    /// We don't have any result for this map yet
    if (!l_BestChallengeMap || l_BestChallengeMap->find(p_ChallengeData->ChallengeID) == l_BestChallengeMap->end())
        l_IsBest = true;
    else
    {
        ChallengeData const* l_ActualBestChallengeData = m_ChallengesOfMember[guid][p_ChallengeData->ChallengeID];

        /// Higher level
        if (p_ChallengeData->ChallengeLevel > l_ActualBestChallengeData->ChallengeLevel)
            l_IsBest = true;

        /// Same level but better time
        if (p_ChallengeData->ChallengeLevel == l_ActualBestChallengeData->ChallengeLevel && p_ChallengeData->RecordTime < l_ActualBestChallengeData->RecordTime)
            l_IsBest = true;
    }

    if (l_IsBest)
        m_ChallengesOfMember[guid][p_ChallengeData->ChallengeID] = p_ChallengeData;

    if (!m_LastForMember[guid][p_ChallengeData->ChallengeID] || m_LastForMember[guid][p_ChallengeData->ChallengeID]->Date < p_ChallengeData->Date)
        m_LastForMember[guid][p_ChallengeData->ChallengeID] = p_ChallengeData;

    if (p_ChallengeData->Date > (sWorld->getNextChallengeKeyReset() - (7 * DAY)))
        m_ChallengeWeekList[guid].insert(p_ChallengeData);

    if (p_ChallengeData->Date < (sWorld->getNextChallengeKeyReset() - (7 * DAY)) && p_ChallengeData->Date > (sWorld->getNextChallengeKeyReset() - (14 * DAY)))
        m_ChallengersOfLastWeek.insert(GUID_LOPART(guid));

    return l_IsBest;
}

void ChallengeMgr::SaveChallengeToDB(ChallengeData const* p_ChallengeData)
{
    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();

    uint8 l_IDx = 0;

    PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE);
    l_Statement->setUInt32(l_IDx++, p_ChallengeData->ID);
    l_Statement->setUInt64(l_IDx++, p_ChallengeData->GuildID);
    l_Statement->setUInt16(l_IDx++, p_ChallengeData->MapID);
    l_Statement->setUInt32(l_IDx++, p_ChallengeData->ChallengeID);
    l_Statement->setUInt32(l_IDx++, p_ChallengeData->RecordTime);
    l_Statement->setUInt32(l_IDx++, p_ChallengeData->Date);
    l_Statement->setUInt8(l_IDx++, p_ChallengeData->ChallengeLevel);
    l_Statement->setUInt8(l_IDx++, p_ChallengeData->TimerLevel);

    std::ostringstream l_AffixesListIDs;
    for (uint16 l_Affixe : p_ChallengeData->Affixes)
    {
        if (l_Affixe)
            l_AffixesListIDs << l_Affixe << ' ';
    }

    l_Statement->setString(l_IDx++, l_AffixesListIDs.str());
    l_Transaction->Append(l_Statement);

    for (auto const& l_Itr : p_ChallengeData->member)
    {
        l_IDx = 0;

        l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE_MEMBER);
        l_Statement->setUInt32(l_IDx++, p_ChallengeData->ID);
        l_Statement->setUInt64(l_IDx++, l_Itr.Guid);
        l_Statement->setUInt16(l_IDx++, l_Itr.SpecId);
        l_Statement->setUInt32(l_IDx++, l_Itr.ChallengeLevel);
        l_Statement->setUInt32(l_IDx++, l_Itr.Date);
        l_Statement->setUInt32(l_IDx++, l_Itr.GuildId);
        l_Transaction->Append(l_Statement);
    }

    CharacterDatabase.CommitTransaction(l_Transaction);
}

void ChallengeMgr::LoadFromDB()
{
    ///                            0       1         2          3               4           5          6             7             8
    std::string l_Query = "SELECT `ID`, `GuildID`, `MapID`, `ChallengeID`, `RecordTime`, `Date`, `ChallengeLevel`, `TimerLevel`, `Affixes` FROM `challenge` WHERE Date > " + std::to_string((sWorld->getWorldState(WS_CHALLENGE_KEY_RESET_TIME) - (7 * DAY)));

    if (QueryResult l_Result = CharacterDatabase.Query(l_Query.c_str()))
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();

            ChallengeData* l_ChallengeData = new ChallengeData;
            l_ChallengeData->ID             = l_Fields[0].GetUInt64();
            l_ChallengeData->GuildID        = l_Fields[1].GetUInt64();
            l_ChallengeData->MapID          = l_Fields[2].GetUInt32();
            l_ChallengeData->ChallengeID    = l_Fields[3].GetUInt32();
            l_ChallengeData->RecordTime     = l_Fields[4].GetUInt32();
            l_ChallengeData->Date           = l_Fields[5].GetUInt32();
            l_ChallengeData->ChallengeLevel = l_Fields[6].GetUInt16();
            l_ChallengeData->TimerLevel     = l_Fields[7].GetUInt16();
            l_ChallengeData->Affixes.fill(0);

            uint8 l_I = 0;
            Tokenizer l_Affixes(l_Fields[8].GetString(), ' ');
            for (auto& affix : l_Affixes)
                l_ChallengeData->Affixes[l_I] = atoul(affix);

            m_ChallengeMap[l_ChallengeData->ID] = l_ChallengeData;
            CheckBestMapId(l_ChallengeData);
        }
        while (l_Result->NextRow());
    }

    l_Query = "SELECT `id`, `member`, `specID`, `ChallengeLevel`, `Date`, GuildId FROM `challenge_member` WHERE Date > " + std::to_string((sWorld->getWorldState(WS_CHALLENGE_KEY_RESET_TIME) - (7 * DAY)));
    if (QueryResult l_Result = CharacterDatabase.Query(l_Query.c_str()))
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();
            ChallengeMember l_Member;
            l_Member.Guid           = l_Fields[1].GetUInt64();
            l_Member.SpecId         = l_Fields[2].GetUInt32();
            l_Member.ChallengeLevel = l_Fields[3].GetUInt16();
            l_Member.Date           = l_Fields[4].GetUInt32();
            l_Member.GuildId        = l_Fields[5].GetUInt32();

            ChallengeMap::iterator l_Itr = m_ChallengeMap.find(l_Fields[0].GetUInt32());
            if (l_Itr == m_ChallengeMap.end())
                continue;

            l_Itr->second->member.insert(l_Member);
            CheckBestMemberMapId(l_Member.Guid, l_Itr->second);
            if (l_Member.GuildId)
                CheckBestGuildMapId(l_Itr->second);
        }
        while (l_Result->NextRow());
    }

    if (QueryResult l_Result = CharacterDatabase.Query("SELECT `guid`, `date`, `ChallengeLevel` FROM `challenge_oplote_loot`"))
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint64 l_Guid = l_Fields[0].GetUInt32();

            OploteLoot& l_LootOplote    = m_OploteWeekLoot[l_Guid];
            l_LootOplote.Date           = l_Fields[1].GetUInt32();
            l_LootOplote.ChallengeLevel = l_Fields[2].GetUInt32();
            l_LootOplote.NeedSave       = false;
            l_LootOplote.Guid           = l_Guid;
        }
        while (l_Result->NextRow());
    }
}

ChallengeData* ChallengeMgr::BestServerChallenge(uint32 p_ChallengeID)
{
    ChallengeByMap::iterator l_Itr = m_BestForMap.find(p_ChallengeID);
    if (l_Itr != m_BestForMap.end())
        return l_Itr->second;

    return nullptr;
}

ChallengeData* ChallengeMgr::BestGuildChallenge(uint32 const& p_GuildID, uint32 p_ChallengeID)
{
    if (!p_GuildID)
        return nullptr;

    GuildBestRecord::iterator l_Itr = m_GuildBest.find(p_GuildID);
    if (l_Itr == m_GuildBest.end())
        return nullptr;

    ChallengeByMap::iterator l_Itr2 = l_Itr->second.find(p_ChallengeID);
    if (l_Itr2 == l_Itr->second.end())
        return nullptr;

    return l_Itr2->second;
}

ChallengeByMap* ChallengeMgr::BestForMember(uint64 const& p_Guid)
{
    ChallengesOfMember::iterator l_Itr = m_ChallengesOfMember.find(p_Guid);
    if (l_Itr != m_ChallengesOfMember.end())
        return &l_Itr->second;

    return nullptr;
}

ChallengeByMap* ChallengeMgr::LastForMember(uint64 const& p_Guid)
{
    ChallengesOfMember::iterator l_Itr = m_LastForMember.find(p_Guid);
    if (l_Itr != m_LastForMember.end())
        return &l_Itr->second;

    return nullptr;
}

ChallengeData* ChallengeMgr::LastForMemberMap(uint64 const& p_Guid, uint32 p_ChallengeID)
{
    if (ChallengeByMap* l_LastResalt = LastForMember(p_Guid))
    {
        ChallengeByMap::iterator l_Itr = l_LastResalt->find(p_ChallengeID);
        if (l_Itr != l_LastResalt->end())
            return l_Itr->second;
    }

    return nullptr;
}

void ChallengeMgr::GenerateCurrentWeekAffixes()
{
    /// affixes same for all players on week
    uint32 l_CurrentWeeklyAffixesIndex = sWorld->getWorldState(WS_CHALLENGE_WEEKLY_AFFIXE_INDEX);
    if (l_CurrentWeeklyAffixesIndex == (g_AffixesRotationCount - 1))
        l_CurrentWeeklyAffixesIndex = 0;
    else
        l_CurrentWeeklyAffixesIndex++;

    m_CurrentWeekAffixes[0] = (uint32)g_AffixesRotations[l_CurrentWeeklyAffixesIndex][0];
    m_CurrentWeekAffixes[1] = (uint32)g_AffixesRotations[l_CurrentWeeklyAffixesIndex][1];
    m_CurrentWeekAffixes[2] = (uint32)g_AffixesRotations[l_CurrentWeeklyAffixesIndex][2];

    sWorld->setWorldState(WS_CHALLENGE_WEEKLY_AFFIXE_INDEX, l_CurrentWeeklyAffixesIndex);
}

bool ChallengeMgr::HasOploteLoot(uint64 const& p_Guid)
{
    OploteLootMap::iterator l_Itr = m_OploteWeekLoot.find(GUID_LOPART(p_Guid));
    if (l_Itr != m_OploteWeekLoot.end())
        return true;

    return false;
}

OploteLoot* ChallengeMgr::GetOploteLoot(uint64 const& p_Guid)
{
    OploteLootMap::iterator l_Itr = m_OploteWeekLoot.find(GUID_LOPART(p_Guid));
    if (l_Itr != m_OploteWeekLoot.end())
        return &l_Itr->second;

    return nullptr;
}

void ChallengeMgr::SaveOploteLootToDB()
{
    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();

    PreparedStatement* l_Statement = nullptr;

    for (auto const& l_Itr : m_OploteWeekLoot)
    {
        if (l_Itr.second.NeedSave)
        {
            l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE_OPLOTE_LOOT);
            l_Statement->setUInt32(0, GUID_LOPART(l_Itr.second.Guid));
            l_Statement->setUInt32(1, l_Itr.second.Date);
            l_Statement->setUInt32(2, l_Itr.second.ChallengeLevel);
            l_Transaction->Append(l_Statement);
        }
    }

    CharacterDatabase.CommitTransaction(l_Transaction);
}

void ChallengeMgr::DeleteOploteLoot(uint64 const& p_Guid, bool p_OnlyFromDatabase)
{
    PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHALLENGE_OPLOTE_LOOT_BY_GUID);
    l_Statement->setUInt32(0, GUID_LOPART(p_Guid));
    CharacterDatabase.Execute(l_Statement);

    if (p_OnlyFromDatabase)
        return;

    m_OploteWeekLoot.erase(GUID_LOPART(p_Guid));
}

void ChallengeMgr::GenerateOploteLoot(uint64 const& p_Guid, uint32 p_ChallengeLevel)
{
    OploteLootMap::iterator l_Itr = m_OploteWeekLoot.find(GUID_LOPART(p_Guid));
    if (l_Itr != m_OploteWeekLoot.end())
    {
        if (l_Itr->second.ChallengeLevel < p_ChallengeLevel)
            l_Itr->second.ChallengeLevel = p_ChallengeLevel;
    }
    else
    {
        OploteLoot& l_LootOplote = m_OploteWeekLoot[GUID_LOPART(p_Guid)];
        l_LootOplote.Date = sWorld->getNextChallengeKeyReset();
        l_LootOplote.ChallengeLevel = p_ChallengeLevel;
        l_LootOplote.NeedSave = true;
        l_LootOplote.Guid = GUID_LOPART(p_Guid);
    }

    SaveOploteLootToDB();
}

void ChallengeMgr::GenerateOploteLoot()
{
    CharacterDatabase.Query("DELETE FROM challenge_oplote_loot WHERE date <= UNIX_TIMESTAMP()");
    CharacterDatabase.Query("UPDATE item_instance SET duration = 0 WHERE itemEntry = 138019");

    m_OploteWeekLoot.clear();

    for (auto const& l_ChallengeList : m_ChallengeWeekList)
    {
        for (auto const& l_ChallengeData : l_ChallengeList.second)
        {
            OploteLootMap::iterator l_Itr = m_OploteWeekLoot.find(GUID_LOPART(l_ChallengeList.first));
            if (l_Itr != m_OploteWeekLoot.end())
            {
                if (l_Itr->second.ChallengeLevel < l_ChallengeData->ChallengeLevel)
                    l_Itr->second.ChallengeLevel = l_ChallengeData->ChallengeLevel;
            }
            else
            {
                OploteLoot& l_LootOplote = m_OploteWeekLoot[GUID_LOPART(l_ChallengeList.first)];
                l_LootOplote.Date           = sWorld->getNextChallengeKeyReset();
                l_LootOplote.ChallengeLevel = l_ChallengeData->ChallengeLevel;
                l_LootOplote.NeedSave       = true;
                l_LootOplote.Guid           = GUID_LOPART(l_ChallengeList.first);
            }
        }
    }

    m_BestForMap.clear();
    m_GuildBest.clear();
    m_ChallengeMap.clear();
    m_LastForMember.clear();
    m_ChallengesOfMember.clear();
    m_ChallengeWeekList.clear();
    SaveOploteLootToDB();
}

bool ChallengeMgr::GetStartPosition(uint32 p_MapChallengeID, float& p_PositionX, float& p_PositionY, float& p_PositionZ, float& p_Orientation)
{
    uint32 WorldSafeLocID = 0;
    switch (p_MapChallengeID)
    {
        case 207: ///< Vault of the Wardens
            WorldSafeLocID = 5105;
            break;
        case 200: ///< Halls of Valor
            WorldSafeLocID = 5098;
            break;
        case 198: ///< Darkheart Thicket
            WorldSafeLocID = 5334;
            break;
        case 199: ///< Black Rook Hold
            WorldSafeLocID = 5352;
            break;
        case 208: ///< Maw Of Souls
            WorldSafeLocID = 5102;
            break;
        case 197: ///< Eye of Azshara
            WorldSafeLocID = 5100;
            break;
        case 210: ///< Court of Stars
            WorldSafeLocID = 5432;
            break;
        case 209: ///< The Arcway
            WorldSafeLocID = 5194;
            break;
        case 206: ///< Neltharion's Lair
            WorldSafeLocID = 5355;
            break;
        case 227: ///< Lower Return To Karazhan
            WorldSafeLocID = 5905;
            break;
        case 234: ///< Upper Return To Karazhan
            WorldSafeLocID = 5906;
            break;
        case 233: ///< Cathedral of Eternal Night
            WorldSafeLocID = 5891;
        case 239: ///< Seat of Triumvirate
            WorldSafeLocID = 6048;
            break;
    }

    if (WorldSafeLocID == 0)
        return false;

    if (WorldSafeLocsEntry const* l_Entry = sWorldSafeLocsStore.LookupEntry(WorldSafeLocID))
    {
        p_PositionX   = l_Entry->x;
        p_PositionY   = l_Entry->y;
        p_PositionZ   = l_Entry->z;
        p_Orientation = l_Entry->o;
        return true;
    }

    return false;
}

bool ChallengeMgr::HasRecordThisWeek(uint64 p_Guid)
{
    return m_ChallengeWeekList.find(p_Guid) != m_ChallengeWeekList.end();
}

bool ChallengeMgr::HasRecordLastWeek(uint64 p_Guid)
{
    return m_ChallengersOfLastWeek.find(GUID_LOPART(p_Guid)) != m_ChallengersOfLastWeek.end();
}
