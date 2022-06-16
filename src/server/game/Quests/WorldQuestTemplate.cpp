///////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014-2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "WorldQuestTemplate.h"

#define HolderInstance ACE_Singleton<MS::WorldQuest::Internal::Holder, ACE_Null_Mutex>::instance()

MS::WorldQuest::Template::Template(uint32 p_ID) : m_ID(p_ID)
{
}

bool MS::WorldQuest::Template::Load(Field* p_Fields)
{
    m_QuestTemplate = sObjectMgr->GetQuestTemplate(m_ID);
    auto l_CliTaskEntry = sQuestV2CliTaskStore.LookupEntry(m_ID);
    BountyEntry const* l_BountyEntry = nullptr;

    if (!l_CliTaskEntry)
    {
        for (uint32 l_ID = 0; l_ID < sBountyStore.GetNumRows(); ++l_ID)
        {
            if (auto l_Entry = sBountyStore.LookupEntry(l_ID))
            {
                if (l_Entry->m_QuestID == m_ID)
                    l_BountyEntry = l_Entry;
            }
        }
    }

    if (!m_QuestTemplate)
        return false;

    if ((!l_CliTaskEntry || !l_CliTaskEntry->QuestInfoId) && !l_BountyEntry)
        return false;

    if (l_CliTaskEntry)
    {
        if (auto l_WorldStateExpEntry = sWorldStateExpressionStore.LookupEntry(l_CliTaskEntry->WorldStateExpressionID))
        {
            char l_WorldStateNumber[5];
            l_WorldStateNumber[4] = 0;
            l_WorldStateNumber[0] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[4 + 2];
            l_WorldStateNumber[1] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[4 + 3];
            l_WorldStateNumber[2] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[4 + 0];
            l_WorldStateNumber[3] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[4 + 1];

            m_WorldStateID = strtoul(l_WorldStateNumber, nullptr, 16);

            char l_WorldStateValue[9];
            l_WorldStateValue[8] = 0;
            l_WorldStateValue[0] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 6];
            l_WorldStateValue[1] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 7];
            l_WorldStateValue[2] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 4];
            l_WorldStateValue[3] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 5];
            l_WorldStateValue[4] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 2];
            l_WorldStateValue[5] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 3];
            l_WorldStateValue[6] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 0];
            l_WorldStateValue[7] = l_WorldStateExpEntry->Expression->Get(sWorld->GetDefaultDb2Locale())[18 + 1];

            m_WorldStateValue = strtoul(l_WorldStateValue, nullptr, 16);

        }
        else
        {
            m_WorldStateID = 0;
            m_WorldStateValue = 0;
        }

        m_PlayerConditionID = l_CliTaskEntry->PlayerConditionID;
        m_SkillIDRequired = l_CliTaskEntry->FiltMinSkillId;
        m_RequiredLevel = l_CliTaskEntry->FiltMinLevel;
        m_WorldQuestType = static_cast<WorldQuestType>(l_CliTaskEntry->QuestInfoId);

        if (!m_QuestTemplate->RewardFactionId[0] || !m_QuestTemplate->RewardFactionId[1])
        {
            m_FactionIDs[0] = std::max(m_QuestTemplate->RewardFactionId[0], m_QuestTemplate->RewardFactionId[1]);
            m_FactionIDs[1] = m_FactionIDs[0];
        }
        else
        {
            m_FactionIDs[0] = m_QuestTemplate->RewardFactionId[0];
            m_FactionIDs[1] = m_QuestTemplate->RewardFactionId[1];

            if (m_FactionIDs[1] == Factions::Wardens || m_FactionIDs[1] == Factions::KirimTor)
                std::swap(m_FactionIDs[0], m_FactionIDs[1]);
        }

        int x = 1;
        int32 l_AreaGroupID = p_Fields[x++].GetInt32();
        m_AreaID = p_Fields[x++].GetInt32();
        m_Enabled = p_Fields[x++].GetUInt8();
        uint32 l_OverrideVariableID = p_Fields[x++].GetUInt32();
        uint32 l_OverrideVariableValue = p_Fields[x++].GetUInt32();
        if (l_OverrideVariableID)
        {
            m_WorldStateID = l_OverrideVariableID;
            m_WorldStateValue = l_OverrideVariableValue;
        }

        if (m_AreaID < 1)
            m_AreaID = -1;

        if (l_AreaGroupID)
        {
            l_AreaGroupID = l_AreaGroupID == -1 ? m_QuestTemplate->GetAreaGroupId() : l_AreaGroupID;

            if (l_AreaGroupID)
                HolderInstance->m_WorldQuestAreaGroupMap[m_ID] = l_AreaGroupID;
        }

        if (m_AreaID > 0 && !l_AreaGroupID)
            HolderInstance->m_WorldQuestAreaGroupMap[m_ID] = -1;
    }
    else
    {
        m_WorldStateID = 12216;     ///< IDK how blizz gets this one ????
        m_WorldStateValue = 1;
        m_PlayerConditionID = 0;
        m_SkillIDRequired = 0;
        m_RequiredLevel = 110;
        m_WorldQuestType = Bounty;
        m_FactionIDs[0] = l_BountyEntry->m_FactionID;
        m_FactionIDs[1] = 0;
        m_Enabled = p_Fields[3].GetUInt8();
        m_AreaID = -1;

        HolderInstance->m_WorldQuestAreaGroupMap[m_ID] = -1;
    }

    return true;
}

MS::WorldQuest::Template const* MS::WorldQuest::Template::GetTemplate(uint32 p_ID)
{
    auto l_Iter = HolderInstance->m_WorldQuestMap.find(p_ID);
    return l_Iter != HolderInstance->m_WorldQuestMap.end() ? l_Iter->second : nullptr;
}

MS::WorldQuest::WorldQuestMap const& MS::WorldQuest::Template::GetTemplates()
{
    return  HolderInstance->m_WorldQuestMap;
}

MS::WorldQuest::WorldQuestList const* MS::WorldQuest::Template::GetTemplatesByWorldQuestType(WorldQuestType p_Type)
{
    auto l_Iter = HolderInstance->m_WorldQuestsByTypeList.find(p_Type);
    return l_Iter != HolderInstance->m_WorldQuestsByTypeList.end() ? &l_Iter->second : nullptr;
}

MS::WorldQuest::WorldQuestAreaGroupMap const& MS::WorldQuest::Template::GetAreaGroupMap()
{
    return HolderInstance->m_WorldQuestAreaGroupMap;
}

MS::WorldQuest::WorldQuestList const& MS::WorldQuest::Template::GetTemplatesBySecondaryFaction(uint32 p_Faction)
{
    return HolderInstance->m_WorldQuestsBySecondaryFactionList[p_Faction];
}

void MS::WorldQuest::Template::LoadAllWorldQuests()
{
    if (!HolderInstance->m_WorldQuestMap.empty())
        return;

    uint32 l_Count = 0;
    uint32 l_OldMSTime = getMSTime();

    PreparedQueryResult l_Ress = WorldDatabase.Query(WorldDatabase.GetPreparedStatement(WORLD_SEL_WORLD_QUEST_TEMPLATE));

    if (!l_Ress)
        return;

    do
    {
        Field* l_Field = l_Ress->Fetch();
        Template* l_Template = new Template(l_Field[0].GetUInt32());

        if (l_Template->Load(l_Field))
        {
            HolderInstance->m_WorldQuestMap[l_Template->GetID()] = l_Template;
            HolderInstance->m_WorldQuestsByTypeList[l_Template->GetWorldQuestType()].push_back(l_Template);
            HolderInstance->m_WorldQuestsByFactionList[l_Template->GetPrimaryFaction()].push_back(l_Template);
            HolderInstance->m_WorldQuestsBySecondaryFactionList[l_Template->GetSecondaryFaction()].push_back(l_Template);
            ++l_Count;
        }
        else
            delete l_Template;
    }
    while (l_Ress->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u World Quest Templates in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void MS::WorldQuest::Template::LoadLootTemplates()
{
    HolderInstance->m_WorldQuestLootGroupMap.clear();
    HolderInstance->m_WorldQuestLootMap.clear();

    uint32 l_Count = 0;
    uint32 l_GroupCount = 0;
    uint32 l_OldMSTime = getMSTime();

    PreparedQueryResult l_Ress = WorldDatabase.Query(WorldDatabase.GetPreparedStatement(WORLD_SEL_WORLD_QUEST_LOOT_GROUP));

    if (!l_Ress)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 World Quest loot entries");
        return;
    }

    do
    {
        int l_I = 0;
        bool l_IsValid = true;

        Field* l_Fields = l_Ress->Fetch();

        uint32 l_ID = l_Fields[l_I++].GetUInt32();

        WorldQuestLootGroupEntry l_Entry;
        l_Entry.m_Type = static_cast<Globals::LoadType>(l_Fields[l_I++].GetUInt8());
        l_Entry.m_ItemID = l_Fields[l_I++].GetUInt32();
        l_Entry.m_CountMin = l_Fields[l_I++].GetUInt32();
        l_Entry.m_CountMax = l_Fields[l_I++].GetUInt32();
        l_Entry.m_Flags = l_Fields[l_I++].GetUInt32();

        if (!l_Entry.m_CountMin)
            l_Entry.m_CountMin = 1;

        if (!l_Entry.m_CountMax)
            l_Entry.m_CountMax = 1;

        if (l_Entry.m_CountMax < l_Entry.m_CountMin)
        {
            sLog->outError(LOG_FILTER_SERVER_LOADING, "Loot group %i, load type %i, item_id %i has invalid count fields", l_ID, static_cast<int>(l_Entry.m_Type), l_Entry.m_ItemID);
            continue;
        }

        switch (l_Entry.m_Type)
        {
            case Globals::LoadType::Currency:
            {
                if (!sCurrencyTypesStore.LookupEntry(l_Entry.m_ItemID))
                {
                    sLog->outError(LOG_FILTER_SERVER_LOADING, "Loot group %i, load type %i, item_id %i has invalid currency id", l_ID, static_cast<int>(l_Entry.m_Type), l_Entry.m_ItemID);
                    continue;
                }
                break;
            }
            case Globals::LoadType::Item:
            {
                if (!sObjectMgr->GetItemTemplate(l_Entry.m_ItemID))
                {
                    sLog->outError(LOG_FILTER_SERVER_LOADING, "Loot group %i, load type %i, item_id %i has invalid item id", l_ID, static_cast<int>(l_Entry.m_Type), l_Entry.m_ItemID);
                    continue;
                }
                break;
            }
            default:
                break;
        }

        if (l_Entry.m_Type != Globals::LoadType::Item)
        {
            for (uint32 l_X = 0; l_X < Classes::MAX_CLASSES + 1; ++l_X)
            {
                HolderInstance->m_WorldQuestLootGroupMap[l_ID][l_X].push_back(l_Entry);
            }
        }
        else
        {
            auto l_Template = sObjectMgr->GetItemTemplate(l_Entry.m_ItemID);

            if (!l_Template)
                continue;

            HolderInstance->m_WorldQuestLootGroupMap[l_ID][Classes::MAX_CLASSES].push_back(l_Entry);

            for (uint32 l_X = Classes::CLASS_WARRIOR; l_X < Classes::MAX_CLASSES; ++l_X)
            {
                if (l_Template->HasClassSpec(l_X, 110))
                    HolderInstance->m_WorldQuestLootGroupMap[l_ID][l_X].push_back(l_Entry);
            }

        }
        ++l_GroupCount;
    }
    while (l_Ress->NextRow());

    l_Ress = WorldDatabase.Query(WorldDatabase.GetPreparedStatement(WORLD_SEL_WORLD_QUEST_LOOT));

    if (l_Ress)
    {
        auto& l_LootGroups = HolderInstance->m_WorldQuestLootGroupMap;
        do
        {
            int l_I = 0;
            bool l_IsValid = true;

            Field* l_Fields = l_Ress->Fetch();

            uint32 l_ID = l_Fields[l_I++].GetUInt32();

            WorldQuestLoot l_Entry;
            l_Entry.l_GroupID = l_Fields[l_I++].GetUInt32();
            l_Entry.l_Chance = l_Fields[l_I++].GetUInt32();

            if (l_LootGroups.find(l_Entry.l_GroupID) == l_LootGroups.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Loot %i, group id %i does not exist", l_ID, l_Entry.l_GroupID);
                continue;
            }

            if (!Template::GetTemplate(l_ID))
            {
                sLog->outError(LOG_FILTER_SQL, "World Quest template %i does not exist", l_ID);
                continue;
            }

            HolderInstance->m_WorldQuestLootMap[l_ID].push_back(l_Entry);
            ++l_Count;
        }
        while (l_Ress->NextRow());
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %i loot groups and %i quest loots in %u ms", l_GroupCount, l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

MS::WorldQuest::WorldQuestLootGroupEntry const* MS::WorldQuest::Template::GenerateLoot(uint8 p_PlayerClass) const
{
    auto l_Loot = HolderInstance->m_WorldQuestLootMap.find(m_ID);
    if (l_Loot == HolderInstance->m_WorldQuestLootMap.end())
        return nullptr;

    uint32 l_TotalChance = 0;

    for (auto l_Entry : l_Loot->second)
        l_TotalChance += l_Entry.l_Chance;

    if (!l_TotalChance)
        return nullptr;

    uint32 l_Roll = urand(1, l_TotalChance);
    uint32 l_Add = 0;

    for (auto l_Entry : l_Loot->second)
    {
        if (l_Roll <= l_Add + l_Entry.l_Chance)
        {
            auto& l_Group = HolderInstance->m_WorldQuestLootGroupMap[l_Entry.l_GroupID][p_PlayerClass].empty() ? HolderInstance->m_WorldQuestLootGroupMap[l_Entry.l_GroupID][Classes::MAX_CLASSES] : HolderInstance->m_WorldQuestLootGroupMap[l_Entry.l_GroupID][p_PlayerClass];
            return &l_Group[urand(0, l_Group.size() - 1)];
        }
        else
            l_Add += l_Entry.l_Chance;
    }

    const_cast<Template*>(this)->GenerateAvailableApTokenIDs();

    return nullptr;
}

void MS::WorldQuest::Template::GenerateAvailableApTokenIDs()
{
    auto l_Loot = HolderInstance->m_WorldQuestLootMap.find(m_ID);
    if (l_Loot == HolderInstance->m_WorldQuestLootMap.end())
        return;

    std::set<uint32> l_TokenIDs;

    for (auto l_Entry : l_Loot->second)
    {
        auto& l_Group = HolderInstance->m_WorldQuestLootGroupMap[l_Entry.l_GroupID][Classes::MAX_CLASSES];
        for (WorldQuestLootGroupEntry const& l_GroupEntry : l_Group)
        {
            if (ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_GroupEntry.m_ItemID))
            {
                bool l_IsAPToken = false;
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(l_Template->Spells[i].SpellId))
                    {
                        if (spellInfo->HasEffect(SPELL_EFFECT_GIVE_ARTIFACT_POWER))
                        {
                            l_IsAPToken = true;
                            break;
                        }
                    }
                }

                if (l_IsAPToken)
                    l_TokenIDs.insert(l_GroupEntry.m_ItemID);
            }
        }
    }

    m_AvailableApTokens.clear();

    for (uint32 l_TokenID : l_TokenIDs)
        m_AvailableApTokens.push_back(l_TokenID);
}


void MS::WorldQuest::Template::GetAvailableApTokenIDs(std::vector<uint32>& p_TokenIds) const
{
    p_TokenIds = m_AvailableApTokens;
}

uint32 MS::WorldQuest::Template::GenerateDuration() const
{
    uint32 m_Duration;

    switch (m_ID)
    {
        case 43798: ///< DANGER: Kosumoth the Hungering
            return 2 * WEEK;
        default:
            break;
    }

    if (m_WorldQuestType == WorldQuestType::EpicEliteWorldQuest)
        m_Duration = 8 * DAY;
    else if (m_WorldQuestType == WorldQuestType::Bounty)
        m_Duration = (3 * DAY) - (3 * HOUR);
    else
        m_Duration = (urand(0, 99) < 50 ? 1 : 2) * DAY;

    return m_Duration;
}

MS::WorldQuest::Internal::Holder::~Holder()
{
    for (auto const& l_Iter : m_WorldQuestMap)
    {
        delete l_Iter.second;
    }
}
