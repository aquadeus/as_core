////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "WorldQuestMgr.h"
#include "GarrisonFollower.hpp"
#include "GarrisonMgr.hpp"
#include "POIMgr.hpp"

MS::WorldQuest::Generator::Generator() : m_NextResetTime(0)
{
    m_UpdateTimer.SetInterval(5000);
}

bool MS::WorldQuest::Generator::AddQuest(Template const* p_Entry, uint32 p_CreationTime, uint32 p_Duration, bool p_SendUpdateToEveryone, bool p_Loading)
{
    if (!p_Entry || m_ActiveWorldQuests.find(p_Entry->GetID()) != m_ActiveWorldQuests.end())
        return false;

    if (!p_Duration)
        p_Duration = p_Entry->GenerateDuration();

    if (!p_CreationTime)
        p_CreationTime = time(nullptr);

    WorldQuestPtr l_Quest = std::make_shared<WorldQuest>();
    l_Quest->m_ID = p_Entry->GetID();
    l_Quest->m_Template = p_Entry;
    l_Quest->m_Duration = p_Duration;
    l_Quest->m_CreationTime = p_CreationTime;

    m_ActiveWorldQuests.insert({ l_Quest->m_ID, l_Quest });

    if (p_Loading)
        return true;

    if (p_Entry->GetWorldQuestType() == WorldQuestType::Bounty)
    {
        auto l_Trans = CharacterDatabase.BeginTransaction();
        auto const& l_Objectives = p_Entry->GetQuestTemplate()->QuestObjectives;

        for (auto const& l_Objective : l_Objectives)
        {
            if (l_Objective.Type != QUEST_OBJECTIVE_TYPE_CRITERIA_TREE)
                continue;

            uint32 l_CriteriaTree = l_Objective.ObjectID;

            auto const& l_Tree = sCriteriaMgr->GetCriteriaTree(l_CriteriaTree);

            if (!l_Tree)
                continue;

            sWorld->GetSessionsLock().acquire_read();
            for (auto l_Iter = sWorld->GetAllSessions().begin(); l_Iter != sWorld->GetAllSessions().end(); ++l_Iter)
            {
                auto l_Player = l_Iter->second->GetPlayer();
                if (!l_Player)
                    continue;

                l_Player->AddCriticalOperation([l_Tree, l_Player]()->void
                {
                    l_Player->ResetCriteriaTree(l_Tree);
                });
            }
            sWorld->GetSessionsLock().release();

            for (auto const& l_Child : l_Tree->Children)
            {
                if (!l_Child->Criteria)
                    continue;

                l_Trans->PAppend("DELETE FROM character_achievement_progress WHERE criteria = %i;", l_Child->Criteria->ID);
            }
        }

        CharacterDatabase.CommitTransaction(l_Trans);
    }

    CharacterDatabase.PExecute("DELETE FROM character_queststatus_rewarded WHERE quest = %i;", p_Entry->GetID());
    CharacterDatabase.PExecute("DELETE FROM character_object_lockout WHERE QuestId = %i;", p_Entry->GetID());

    if (p_SendUpdateToEveryone)
    {
       if (p_Entry->GetWorldStateID())
            SendWorldStateUpdateToAll(p_Entry->GetWorldStateID(), p_Entry->GetWorldStateValue());

        SendWorldQuestsUpdateToAll();
    }

    sWorld->GetSessionsLock().acquire_read();
    for (auto l_Iter = sWorld->GetAllSessions().begin(); l_Iter != sWorld->GetAllSessions().end(); ++l_Iter)
    {
        auto l_Player = l_Iter->second->GetPlayer();
        if (!l_Player)
            continue;

        l_Player->AddCriticalOperation([l_Player, p_Entry]()->void
        {
            l_Player->RemoveRewardedQuest(p_Entry->GetID());
            l_Player->GetWorldQuestManager()->GenerateReward(p_Entry->GetID());
            l_Player->RemoveObjectsFromLockoutListByQuest(p_Entry->GetID(), false);

            if (p_Entry->GetWorldQuestType() == MS::WorldQuest::WorldQuestType::Bounty && !l_Player->HasQuest(p_Entry->GetID()))
            {
                l_Player->SetQuestStatus(p_Entry->GetID(), QuestStatus::QUEST_STATUS_INCOMPLETE);
                l_Player->AddQuest(p_Entry->GetQuestTemplate(), l_Player);

                auto l_Slot = l_Player->FindQuestSlot(p_Entry->GetID());

                if (l_Slot < MAX_QUEST_LOG_SIZE)
                {
                    for (auto const& l_Objective : p_Entry->GetQuestTemplate()->QuestObjectives)
                    {
                        l_Player->SetQuestSlotCounter(l_Slot, l_Objective.Index, l_Player->GetQuestObjectiveCounter(l_Objective.ID));
                    }
                }
            }
        });
    }
    sWorld->GetSessionsLock().release();


    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_WORLD_QUEST);
    l_Stmt->setUInt32(0, p_Entry->GetID());
    l_Stmt->setUInt32(1, p_CreationTime);
    l_Stmt->setUInt32(2, p_Duration);
    CharacterDatabase.Execute(l_Stmt);
    return true;
}

bool MS::WorldQuest::Generator::HasQuestActive(uint32 p_ID)
{
    auto l_Iter = m_ActiveWorldQuests.find(p_ID);
    return l_Iter != m_ActiveWorldQuests.end() ? true : false;
}

MS::WorldQuest::ActiveWorldQuestsList const& MS::WorldQuest::Generator::GetActiveWorldQuests()
{
    return m_ActiveWorldQuests;
}

void MS::WorldQuest::Generator::SendWorldStateUpdateToAll(uint32 p_WorldState, uint32 p_Value, uint32 p_QuestID /*= 0*/)
{
    sWorld->GetSessionsLock().acquire_read();
    for (auto l_Iter = sWorld->GetAllSessions().begin(); l_Iter != sWorld->GetAllSessions().end(); ++l_Iter)
    {
        auto l_Player = l_Iter->second->GetPlayer();
        if (!l_Player)
            continue;

        l_Player->AddCriticalOperation([l_Player, p_QuestID, p_WorldState, p_Value]()->void
        {
            if (l_Player->GetWorldQuestManager()->CanHaveWorldQuest(p_QuestID))
                l_Player->SendUpdateWorldState(p_WorldState, p_Value);
        });
    }
    sWorld->GetSessionsLock().release();
}

void MS::WorldQuest::Generator::SendWorldQuestsUpdateToAll()
{
    sWorld->GetSessionsLock().acquire_read();
    for (auto l_Iter = sWorld->GetAllSessions().begin(); l_Iter != sWorld->GetAllSessions().end(); ++l_Iter)
    {
        auto l_Player = l_Iter->second->GetPlayer();
        if (!l_Player)
            continue;

        l_Player->AddCriticalOperation([l_Player]()->void
        {
            l_Player->SendWorldQuestUpdate();
        });
    }
    sWorld->GetSessionsLock().release();
}

void MS::WorldQuest::Generator::Update(uint32 const p_Diff)
{
    m_UpdateTimer.Update(p_Diff);

    if (m_UpdateTimer.Passed())
    {
        std::list<uint32> l_QuestsToRemove;

        m_ActiveWorldQuests.safe_foreach(true, [&l_QuestsToRemove](ActiveWorldQuestsList::iterator& p_Itr) -> void
        {
            uint32 l_EndTime = p_Itr->second->m_CreationTime + p_Itr->second->m_Duration;

            if (time(nullptr) > l_EndTime)
                l_QuestsToRemove.push_back(p_Itr->first);
        });

        if (l_QuestsToRemove.size())
        {
            sWorld->GetSessionsLock().acquire_read();
            for (auto l_QuestID : l_QuestsToRemove)
            {
                for (auto l_Iter = sWorld->GetAllSessions().begin(); l_Iter != sWorld->GetAllSessions().end(); ++l_Iter)
                {
                    auto l_Player = l_Iter->second->GetPlayer();
                    if (!l_Player)
                        continue;

                    l_Player->AddCriticalOperation([l_Player, l_QuestID]()->void
                    {
                        l_Player->GetWorldQuestManager()->Expired(l_QuestID);
                    });
                }
            }
            sWorld->GetSessionsLock().release();

            auto l_Trans = CharacterDatabase.BeginTransaction();
            {
                for (auto l_QuestID : l_QuestsToRemove)
                {
                    auto l_Quest = sObjectMgr->GetQuestTemplate(l_QuestID);

                    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_WORLD_QUEST_REWARDS_BY_QUEST_ID);
                    l_Stmt->setUInt32(0, l_QuestID);
                    l_Trans->Append(l_Stmt);

                    l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_WORLD_QUEST);
                    l_Stmt->setUInt32(0, l_QuestID);
                    l_Trans->Append(l_Stmt);

                    l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_BY_QUEST_ID);
                    l_Stmt->setUInt32(0, l_QuestID);
                    l_Trans->Append(l_Stmt);

                    for (auto const& l_Objective : l_Quest->QuestObjectives)
                    {
                        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_QUESTSTATUS_OBJECTIVES);
                        l_Stmt->setUInt32(0, l_Objective.ID);
                        l_Trans->Append(l_Stmt);
                    }
                }
            }

            CharacterDatabase.CommitTransaction(l_Trans);

            for (auto l_QuestID : l_QuestsToRemove)
            {
                auto l_Quest = m_ActiveWorldQuests.find(l_QuestID);
                sWorld->GetSessionsLock().acquire_read();
                for (auto l_Iter = sWorld->GetAllSessions().begin(); l_Iter != sWorld->GetAllSessions().end(); ++l_Iter)
                {
                    auto l_Player = l_Iter->second->GetPlayer();
                    if (!l_Player)
                        continue;

                    bool l_IsNoBounty = l_Quest->second->m_Template->GetWorldQuestType() != WorldQuestType::Bounty;
                    uint32 l_WorldStateID = l_Quest->second->m_Template->GetWorldStateID();
                    l_Player->AddCriticalOperation([l_Player, l_IsNoBounty, l_WorldStateID]()->void
                    {
                        if (l_IsNoBounty)
                            l_Player->SetWorldState(l_WorldStateID, 0);

                        l_Player->SendWorldQuestUpdate();
                    });
                }
                sWorld->GetSessionsLock().release();

                m_ActiveWorldQuests.erase(l_Quest);
            }
        }

        m_UpdateTimer.Reset();
    }

    if (m_NextResetTime && m_NextResetTime < time(nullptr))
    {
        m_NextResetTime = time(nullptr) + HOUR * 12;
        Reset(true);
        sWorld->setWorldState(WorldStates::WS_DAILY_WORLD_QUEST_RESET, m_NextResetTime);
    }
}

bool MS::WorldQuest::Generator::CanBePicked(uint32 p_QuestID)
{
    auto l_Iter = m_ActiveWorldQuests.find(p_QuestID);

    if (l_Iter == m_ActiveWorldQuests.end())
        return false;

    return time(nullptr) < l_Iter->second->m_CreationTime + l_Iter->second->m_Duration;
}

bool MS::WorldQuest::Generator::ExpireQuest(uint32 p_QuestID)
{
    auto l_Iter = m_ActiveWorldQuests.find(p_QuestID);
    if (l_Iter == m_ActiveWorldQuests.end())
        return false;

    l_Iter->second->m_Duration = 0;
    return true;
}

int32 MS::WorldQuest::Generator::GetTimeRemaining(uint32 p_QuestID)
{
    auto l_Itr = m_ActiveWorldQuests.find(p_QuestID);
    return l_Itr == m_ActiveWorldQuests.end() ? 0 : (l_Itr->second->m_CreationTime + l_Itr->second->m_Duration) - time(nullptr);
}

void MS::WorldQuest::Generator::ModifyAllDurations(int32 l_Duration)
{
    m_ActiveWorldQuests.safe_foreach(true, [l_Duration](ActiveWorldQuestsList::iterator& p_Itr) -> void
    {
        if (l_Duration < 0 && p_Itr->second->m_Duration < abs(l_Duration))
            p_Itr->second->m_Duration = 0;
        else
            p_Itr->second->m_Duration += l_Duration;
    });
}

void MS::WorldQuest::Generator::Load()
{
    uint32 l_OldMSTime = getMSTime();
    uint32 l_Count = 0;

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_WORLD_QUESTS);
    PreparedQueryResult l_Ress = CharacterDatabase.Query(l_Stmt);

    if (l_Ress)
    {
        do
        {
            Field* l_Data = l_Ress->Fetch();

            if (AddQuest(Template::GetTemplate(l_Data[0].GetUInt32()), l_Data[1].GetUInt32(), l_Data[2].GetUInt32(), false, true))
                ++l_Count;
        }
        while (l_Ress->NextRow());
    }

    sWorldQuestGenerator->LoadTimer();
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u World Quests in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

uint32 g_AvailableBountyQuests[] = { 42170, 42420, 42422, 42233, 42234, 42421, 48641, 48639, 48642 };

MS::WorldQuest::WorldQuestType g_ProfessionsWQTypes[] =
{
    MS::WorldQuest::WorldQuestType::Alchemy,
    MS::WorldQuest::WorldQuestType::Blacksmithing,
    MS::WorldQuest::WorldQuestType::Cooking,
    MS::WorldQuest::WorldQuestType::Enchanting,
    MS::WorldQuest::WorldQuestType::Engineering,
    MS::WorldQuest::WorldQuestType::Fishing,
    MS::WorldQuest::WorldQuestType::Herbalism,
    MS::WorldQuest::WorldQuestType::Inscription,
    MS::WorldQuest::WorldQuestType::Jewelcrafting,
    MS::WorldQuest::WorldQuestType::Leatherworking,
    MS::WorldQuest::WorldQuestType::Mining,
    MS::WorldQuest::WorldQuestType::Skinning,
    MS::WorldQuest::WorldQuestType::Tailoring,
};

void MS::WorldQuest::Generator::Reset(bool p_Morning)
{
    uint32 l_FactionsToPick[] = 
    {
        Factions::Dreamwalkers, Factions::CourtOfFadoris, Factions::HighmountainTribes, Factions::Valarjar, Factions::TheNighfallen, ///< Broken Shore factions
        Factions::ArmiesOfLegionfall, Factions::ArmyOfTheLight, Factions::ArgussianReach,                                            ///< Argus factions
    };

    for (uint32 l_Faction : l_FactionsToPick)
    {
        auto const& l_Grp = Template::GetTemplatesBySecondaryFaction(l_Faction);

        WorldQuestList l_NonActiveQuests;

        std::for_each(l_Grp.begin(), l_Grp.end(), [&l_NonActiveQuests](auto l_Template)->void
        {
            if (sWorldQuestGenerator->HasQuestActive(l_Template->GetID()))
                return;

            switch (l_Template->GetWorldQuestType())
            {
                case WorldQuestType::EpicEliteWorldQuest:
                case WorldQuestType::Bounty:
                case WorldQuestType::LegionInvasionWQ:
                case WorldQuestType::LegionInvasionEliteWQ:
                    return;
                default:
                    l_NonActiveQuests.push_back(l_Template);
                    break;
            }
        });

        if (l_NonActiveQuests.empty())
            continue;

        uint32 l_Count = std::min(urand(4, 7), static_cast<uint32>(l_NonActiveQuests.size()));
        std::random_shuffle(l_NonActiveQuests.begin(), l_NonActiveQuests.end());

        for (uint32 l_I = 0; l_I < l_Count; l_I++)
        {
            sWorldQuestGenerator->AddQuest(l_NonActiveQuests[l_I]);
            SendWorldStateUpdateToAll(l_NonActiveQuests[l_I]->GetWorldStateID(), l_NonActiveQuests[l_I]->GetWorldStateValue());
        }

        /// Enable all pvp world quest on tournament realm
        if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        {
            for (uint32 l_I = l_Count; l_I < l_NonActiveQuests.size(); l_I++)
            {
                if (l_NonActiveQuests[l_I]->GetWorldQuestType() == WorldQuestType::DailyPvP)
                {
                    sWorldQuestGenerator->AddQuest(l_NonActiveQuests[l_I]);
                    SendWorldStateUpdateToAll(l_NonActiveQuests[l_I]->GetWorldStateID(), l_NonActiveQuests[l_I]->GetWorldStateValue());
                }
            }
        }
    }

    auto l_WorldBosses = Template::GetTemplatesByWorldQuestType(WorldQuestType::EpicEliteWorldQuest);

    if (l_WorldBosses)
    {
        bool l_CanHave = true;
        WorldQuestList l_WorldBossesList;

        for (auto l_Entry : *l_WorldBosses)
        {
            if (HasQuestActive(l_Entry->GetID()))
            {
                if (GetTimeRemaining(l_Entry->GetID()) > 24 * HOUR)
                    l_CanHave = false;
            }
            else
                l_WorldBossesList.push_back(l_Entry);
        }

        if (l_CanHave && !l_WorldBossesList.empty())
        {
            auto const* l_Entry = l_WorldBossesList[urand(0, l_WorldBossesList.size() - 1)];

            if (AddQuest(l_Entry))
                SendWorldStateUpdateToAll(l_Entry->GetWorldStateID(), l_Entry->GetWorldStateValue());
        }
    }

    for (WorldQuestType l_WQType : g_ProfessionsWQTypes)
    {
        auto l_ProfessionWQs = Template::GetTemplatesByWorldQuestType(l_WQType);
        if (!l_ProfessionWQs)
            continue;

        WorldQuestList l_AvailableProfessionWQs;

        for (auto l_Entry : *l_ProfessionWQs)
        {
            if (HasQuestActive(l_Entry->GetID()))
                continue;

            l_AvailableProfessionWQs.push_back(l_Entry);
        }

        if (l_AvailableProfessionWQs.empty())
            continue;

        auto const* l_Entry = l_AvailableProfessionWQs[urand(0, l_AvailableProfessionWQs.size() - 1)];
        if (AddQuest(l_Entry))
            SendWorldStateUpdateToAll(l_Entry->GetWorldStateID(), l_Entry->GetWorldStateValue());
    }

    uint32 l_EmissaryQuest = g_AvailableBountyQuests[GetDayOfTheYear() % (sizeof(g_AvailableBountyQuests) / sizeof(uint32))];

    if (!HasQuestActive(l_EmissaryQuest))
    {
        auto const* l_Entry = MS::WorldQuest::Template::GetTemplate(l_EmissaryQuest);
        if (AddQuest(l_Entry))
            SendWorldStateUpdateToAll(l_Entry->GetWorldStateID(), l_Entry->GetWorldStateValue());
    }

    SendWorldQuestsUpdateToAll();
}

void MS::WorldQuest::Generator::LoadTimer()
{
    m_NextResetTime = sWorld->getWorldState(WorldStates::WS_DAILY_WORLD_QUEST_RESET);

    if (!m_NextResetTime)
    {
        Reset(true);
        m_NextResetTime = time(nullptr) + HOUR * 12;
        sWorld->setWorldState(WorldStates::WS_DAILY_WORLD_QUEST_RESET, m_NextResetTime);
    }
}

MS::WorldQuest::Manager::WorldQuestReward::WorldQuestReward() : m_Money(0)
{
}

MS::WorldQuest::Manager::WorldQuestReward::ItemReward::ItemReward(uint32 p_ID, uint32 p_Count, int8 p_BonusIndex, std::vector<uint32> p_Bonuses) : m_ID(p_ID), m_Count(p_Count), m_ItemContext(p_BonusIndex), m_Bonuses(p_Bonuses)
{
}

MS::WorldQuest::Manager::WorldQuestReward::CurrencyReward::CurrencyReward(uint32 p_ID, uint32 p_Count) : m_ID(p_ID), m_Count(p_Count)
{
}

MS::WorldQuest::Manager::Manager(Player* p_Player) : m_Player(p_Player)
{
}

bool MS::WorldQuest::Manager::HasRewarded(uint32 p_QuestID) const
{
    return m_Player->IsQuestRewarded(p_QuestID);
}

bool MS::WorldQuest::Manager::CanPickQuest(uint32 p_QuestID) const
{
    Quest const* l_Quest = sObjectMgr->GetQuestTemplate(p_QuestID);
    if (!l_Quest)
        return false;

    /// Prevent to send PvP quest to wrong faction but also broken phasing of pvp quests (opposite factions don't enter in the same phase so they just can't PvP)
    if (!m_Player->SatisfyQuestRace(l_Quest, false))
        return false;

    return !HasRewarded(p_QuestID) && GetReward(p_QuestID) && sWorldQuestGenerator->CanBePicked(p_QuestID);
}

void MS::WorldQuest::Manager::GenerateReward(uint32 p_QuestID)
{
    if (!CanHaveWorldQuest(p_QuestID))
        return;

    if (HasRewarded(p_QuestID))
        return;

    if (m_RewardList.find(p_QuestID) != m_RewardList.end())
        return;

    Template const* l_Template = Template::GetTemplate(p_QuestID);

    if (!l_Template)
        return;

    if (l_Template->GetSkillIDRequired() && !m_Player->HasSkill(l_Template->GetSkillIDRequired()))
        return;

    if (!m_Player->SatisfyQuestConditions(l_Template->GetQuestTemplate(), false))
        return;

    WorldQuestReward l_Reward;

    if (l_Template->GetWorldQuestType() != WorldQuestType::Bounty)
    {
        auto l_Loot = l_Template->GenerateLoot(m_Player->getClass());
        if (!l_Loot)
            return;

        switch (l_Loot->m_Type)
        {
            case Globals::LoadType::Money:
                l_Reward.m_Money = urand(l_Loot->m_CountMin, l_Loot->m_CountMax);
                break;
            case Globals::LoadType::Currency:
            {
                l_Reward.m_CurrencyRewards.push_back(WorldQuestReward::CurrencyReward(l_Loot->m_ItemID, urand(l_Loot->m_CountMin, l_Loot->m_CountMax)));
                break;
            }
            case Globals::LoadType::Item:
            {
                int32 l_Count = urand(l_Loot->m_CountMin, l_Loot->m_CountMax);

                uint32 l_ItemId = l_Loot->m_ItemID;

                if (l_Template->GetWorldQuestType() == WorldQuestType::DailyPvP)
                {
                    auto l_Reward = sObjectMgr->GetItemTemplate(l_ItemId);
                    if (l_Reward && (l_Reward->isArmorOrWeapon() || l_Reward->IsArtifactRelic()))
                    {
                        if (auto l_PvPReward = sObjectMgr->GetItemRewardForPvp(m_Player->GetLootSpecId() ? m_Player->GetLootSpecId() : m_Player->GetActiveSpecializationID(), MS::Battlegrounds::RewardGearType::Regular, m_Player))
                        {
                            l_ItemId = l_PvPReward->ItemId;
                            l_Count = 1;
                        }
                    }
                }

                std::vector<uint32> l_ItemBonuses;
                uint8 l_Context = GenerateItemContext(l_Template->GetWorldQuestType(), sObjectMgr->GetItemTemplate(l_ItemId), l_ItemBonuses);

                l_Reward.m_ItemRewards.push_back(WorldQuestReward::ItemReward(l_ItemId, l_Count, l_Context, l_ItemBonuses));
                break;
            }
            default:
                break;
        }
    }
    else
    {
        m_Player->AddQuest(l_Template->GetQuestTemplate(), m_Player);

        auto l_Slot = m_Player->FindQuestSlot(l_Template->GetID());

        if (l_Slot < MAX_QUEST_LOG_SIZE)
        {
            for (auto const& l_Objective : l_Template->GetQuestTemplate()->QuestObjectives)
            {
                m_Player->SetQuestSlotCounter(l_Slot, l_Objective.Index, m_Player->GetQuestObjectiveCounter(l_Objective.ID));
            }
        }
    }

    m_RewardList[p_QuestID] = std::move(l_Reward);
}

void MS::WorldQuest::Manager::Reward(uint32 p_Quest, uint32 p_ChooseReward, bool p_Remove /*= true*/)
{
    uint32 l_QuestBit = GetQuestUniqueBitFlag(p_Quest);

    auto l_Reward = GetReward(p_Quest);

    if (l_Reward && l_QuestBit && !m_Player->IsQuestBitFlaged(l_QuestBit))
    {
        if (l_Reward->m_Money)
        {
            m_Player->ModifyMoney(l_Reward->m_Money);
           // m_Player->SendDisplayToast(0, l_Reward->m_Money, p_Quest, DISPLAY_TOAST_METHOD_WORLD_QUEST_REWARD, TOAST_TYPE_MONEY, false, false);
            m_Player->SendDisplayToast(0, l_Reward->m_Money, p_Quest, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);
        }

        for (auto const& l_Item : l_Reward->m_ItemRewards)
        {
            ItemPosCountVec sDest;
            InventoryResult msg = m_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, l_Item.m_ID, l_Item.m_Count);

            if (msg != EQUIP_ERR_OK)
                continue;

            std::vector<uint32> l_ItemBonuses;
            Item::GenerateItemBonus(l_Item.m_ID, static_cast<ItemContext>(l_Item.m_ItemContext), l_ItemBonuses, true, m_Player);

            if (Item* l_AddedItem = m_Player->StoreNewItem(sDest, l_Item.m_ID, true, Item::GenerateItemRandomPropertyId(l_Item.m_ID), l_ItemBonuses))
            {
                uint32 l_Ilvl = l_AddedItem->GetItemLevel(m_Player);

                if (std::find(l_Item.m_Bonuses.begin(), l_Item.m_Bonuses.end(), ItemBonus::Bonuses::WorldQuestRare) != l_Item.m_Bonuses.end())
                    l_AddedItem->AddItemBonus(ItemBonus::Bonuses::WorldQuestRare);

                if (std::find(l_Item.m_Bonuses.begin(), l_Item.m_Bonuses.end(), ItemBonus::Bonuses::WorldQuestEpic) != l_Item.m_Bonuses.end())
                    l_AddedItem->AddItemBonus(ItemBonus::Bonuses::WorldQuestEpic);

                //m_Player->SendDisplayToast(l_Item.m_ID, l_Item.m_Count, p_Quest, DISPLAY_TOAST_METHOD_WORLD_QUEST_REWARD, TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                m_Player->SendDisplayToast(l_Item.m_ID, l_Item.m_Count, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                m_Player->SendNewItem(l_AddedItem, 1, true, false);
            }
        }

        for (auto const& l_Currency : l_Reward->m_CurrencyRewards)
        {
            m_Player->ModifyCurrency(l_Currency.m_ID, l_Currency.m_Count);
            //m_Player->SendDisplayToast(l_Currency.m_ID, l_Currency.m_Count, p_Quest, DISPLAY_TOAST_METHOD_WORLD_QUEST_REWARD, TOAST_TYPE_NEW_CURRENCY, false, false);
            m_Player->SendDisplayToast(l_Currency.m_ID, l_Currency.m_Count, p_Quest, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
        }

        if (roll_chance_f(25.0f) && m_Player->HasAura(237139)) ///< Power Overwhelming
        {
            auto const& l_QuestItr = sWorldQuestGenerator->GetActiveWorldQuests().find(p_Quest);
            if (l_QuestItr != sWorldQuestGenerator->GetActiveWorldQuests().end())
            {
                if (Template const* l_QuestTemplate = l_QuestItr->second->m_Template)
                {
                    std::vector<uint32> l_TokenIDs;
                    l_QuestTemplate->GetAvailableApTokenIDs(l_TokenIDs);

                    if (!l_TokenIDs.empty())
                    {
                        uint32 l_ItemID = l_TokenIDs[urand(0, l_TokenIDs.size() - 1)];

                        ItemPosCountVec sDest;
                        InventoryResult msg = m_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, l_ItemID, 1);

                        if (msg == EQUIP_ERR_OK)
                        {
                            if (Item* l_AddedItem = m_Player->StoreNewItem(sDest, l_ItemID, true, 0))
                            {
                                m_Player->SendDisplayToast(l_ItemID, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                                m_Player->SendNewItem(l_AddedItem, 1, true, false);
                            }
                        }
                    }
                }
            }
        }

        if (roll_chance_f(25.0f) && m_Player->HasAura(240989)) ///< Heavily Augmented
        {
            uint32 l_ItemID = 140587; ///< Defiled Augment Rune

            ItemPosCountVec sDest;
            InventoryResult msg = m_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, l_ItemID, 1);

            if (msg == EQUIP_ERR_OK)
            {
                if (Item* l_AddedItem = m_Player->StoreNewItem(sDest, l_ItemID, true, 0))
                {
                    m_Player->SendDisplayToast(l_ItemID, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                    m_Player->SendNewItem(l_AddedItem, 1, true, false);
                }
            }
        }

        if (!p_Remove)
            return;

        m_RewardList.erase(m_RewardList.find(p_Quest));
        m_Player->SendWorldQuestUpdate();
    }

    if (!p_Remove)
        return;

    m_Player->SetQuestStatus(p_Quest, QUEST_STATUS_COMPLETE);
    m_Player->RewardQuest(sObjectMgr->GetQuestTemplate(p_Quest), p_ChooseReward, nullptr);

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();
    if (l_Garrison)
    {
        if (MS::Garrison::GarrisonFollower* l_Follower = l_Garrison->GetCombatAllyFollower())
        {
            std::vector<GarrAbilityEffectEntry const*> l_GarrisonAbilityEffectEntries = l_Garrison->GetApplicableGarrAbilitiesEffectsByType(MS::Garrison::AbilityEffectTypes::ModRewardFromWQWithCombatAlly, l_Follower->CurrentMissionID);
            for (GarrAbilityEffectEntry const* l_GarrisonAbilityEffectEntry : l_GarrisonAbilityEffectEntries)
            {
                m_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_ORDER_RESSOURCES, 100);
                break;
            }
        }
    }

    m_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_COMPLETE_WORLD_QUEST, 1, p_Quest);

    WorldPacket l_Data(SMSG_SET_TASK_COMPLETE, 4);
    l_Data << uint32(p_Quest);
    m_Player->SendDirectMessage(&l_Data);

    uint32 l_Slot = m_Player->FindQuestSlot(p_Quest);

    if (l_Slot < MAX_QUEST_LOG_SIZE)
        m_Player->SetQuestSlot(l_Slot, 0);

    m_Player->RemoveActiveQuest(p_Quest, false);

    auto l_Quest = sObjectMgr->GetQuestTemplate(p_Quest);

    for (auto const& l_Objective : l_Quest->QuestObjectives)
    {
        m_Player->SetQuestObjectiveCounter(l_Objective.ID, 0);
    }

    if (l_QuestBit)
        m_Player->SetQuestBit(l_QuestBit, true);
}

MS::WorldQuest::Manager::WorldQuestReward const* MS::WorldQuest::Manager::GetReward(uint32 p_Quest) const
{
    auto l_Iter = m_RewardList.find(p_Quest);
    return l_Iter != m_RewardList.end() ? &l_Iter->second : nullptr;
}

bool MS::WorldQuest::Manager::CanBeNotifiedToClient(uint32 p_Quest) const
{
    auto const* l_Quest = Template::GetTemplate(p_Quest);
    if (!l_Quest)
        return false;

    switch (l_Quest->GetWorldQuestType())
    {
        case WorldQuestType::LegionInvasionWQ:
        case WorldQuestType::LegionInvasionEliteWQ:
            break;
        default:
            if (m_Player->getLevel() < 110)
                return false;
            break;
    }

    return (l_Quest->GetWorldQuestType() == WorldQuestType::Bounty || GetReward(p_Quest));
}

bool MS::WorldQuest::Manager::CanHaveWorldQuest(uint32 p_QuestID) const
{
    auto const* l_Quest = Template::GetTemplate(p_QuestID);
    if (!l_Quest)
        return false;

    switch (l_Quest->GetWorldQuestType())
    {
        case WorldQuestType::LegionInvasionWQ:
            break;
        default:
            if (m_Player->getLevel() < 110)
                return false;
            break;
    }

    return true;
}

void MS::WorldQuest::Manager::Expired(uint32 p_QuestID)
{
    auto l_Iter = m_RewardList.find(p_QuestID);
    if (l_Iter != m_RewardList.end())
        m_RewardList.erase(l_Iter);

    for (uint8 l_Slot = 0; l_Slot < MAX_QUEST_LOG_SIZE; ++l_Slot)
    {
        uint32 l_LogQuest = m_Player->GetQuestSlotQuestId(l_Slot);
        if (l_LogQuest == p_QuestID)
        {
            m_Player->SetQuestSlot(l_Slot, 0);
            m_Player->TakeQuestSourceItem(l_LogQuest, false);
        }
    }

    m_Player->RemoveActiveQuest(p_QuestID, true);

    auto l_Quest = sObjectMgr->GetQuestTemplate(p_QuestID);

    for (auto const& l_Objective : l_Quest->QuestObjectives)
    {
        m_Player->SetQuestObjectiveCounter(l_Objective.ID, 0);
    }
}

void MS::WorldQuest::Manager::OnLogin()
{
    auto const& l_ActiveWorldQuests = sWorldQuestGenerator->GetActiveWorldQuests();
    l_ActiveWorldQuests.safe_foreach(true, [this](ActiveWorldQuestsList::const_iterator& p_Itr) -> void
    {
        GenerateReward(p_Itr->first);
    });

    GenerateWeeklyRatedPvpRewards();

    m_Player->SendWorldQuestUpdate();
    m_Player->SendAllTaskProgress();
}

void MS::WorldQuest::Manager::LoadFromDB(PreparedQueryResult p_Result)
{
    ///        1         2     3   4       5       6
    ///"SELECT quest_id, type, id, amount, arg0, bonuses FROM character_world_quest_rewards WHERE guid = ? ORDER BY quest_id ASC

    if (!p_Result)
        return;

    do
    {
        int l_I = 0;
        Field* l_Data = p_Result->Fetch();

        uint32 l_QuestID         = l_Data[l_I++].GetUInt32();
        Globals::LoadType l_Type = static_cast<Globals::LoadType>(l_Data[l_I++].GetUInt8());
        uint32 l_ID              = l_Data[l_I++].GetUInt32();
        uint32 l_Amount          = l_Data[l_I++].GetUInt32();
        int32 l_Arg0             = l_Data[l_I++].GetUInt32();
        std::string l_Bonuses    = l_Data[l_I++].GetString();

        std::vector<uint32> l_BonusesID;

        Tokenizer bonusTokens(l_Bonuses, ' ');
        for (uint8 i = 0; i < bonusTokens.size(); ++i)
            l_BonusesID.push_back(atoi(bonusTokens[i]));

        if (!l_Amount)
            continue;

        switch (l_Type)
        {
            case Globals::LoadType::Item:
            {
                if (!sObjectMgr->GetItemTemplate(l_ID))
                    continue;

                break;
            }
            case Globals::LoadType::Currency:
            {
                if (!sCurrencyTypesStore.LookupEntry(l_ID))
                    continue;

                break;
            }
            default:
                break;
        }

        auto& l_Reward = m_RewardList[l_QuestID];

        switch (l_Type)
        {
            case Globals::LoadType::Money:
            {
                l_Reward.m_Money = l_Amount;
                break;
            }
            case Globals::LoadType::Item:
            {
                l_Reward.m_ItemRewards.push_back(WorldQuestReward::ItemReward(l_ID, l_Amount, l_Arg0, l_BonusesID));
                break;
            }
            case Globals::LoadType::Currency:
            {
                l_Reward.m_CurrencyRewards.push_back(WorldQuestReward::CurrencyReward(l_ID, l_Amount));
                break;
            }
            default:
                break;
        }
    }
    while (p_Result->NextRow());
}

void MS::WorldQuest::Manager::SaveToDB(SQLTransaction& p_Trans)
{
    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_WORLD_QUEST_REWARDS);
    l_Stmt->setUInt32(0, m_Player->GetGUIDLow());
    p_Trans->Append(l_Stmt);

    if (m_RewardList.empty())
        return;

    std::stringstream ss;
    bool l_First = true;
    ss << "INSERT INTO character_world_quest_rewards (guid, quest_id, type, id, amount, arg0, bonuses) VALUES";

    for (auto const& l_Quest : m_RewardList)
    {
        if (l_Quest.second.m_Money)
        {
            if (l_First)
                l_First = false;
            else
                ss << ", ";

            ss << "(" << m_Player->GetGUIDLow() << ", " << l_Quest.first << ", " << static_cast<uint32>(Globals::LoadType::Money) << ", 0, " << l_Quest.second.m_Money << ", 0, '')";
        }

        if (!l_Quest.second.m_ItemRewards.empty())
        {
            for (auto const& l_Item : l_Quest.second.m_ItemRewards)
            {
                if (l_First)
                    l_First = false;
                else
                    ss << ", ";

                ss << "(" << m_Player->GetGUIDLow() << ", " << l_Quest.first << ", " << static_cast<uint32>(Globals::LoadType::Item) << ", " << l_Item.m_ID << ", " << l_Item.m_Count << ", " << (int32)l_Item.m_ItemContext << ", '";
                for (auto l_Bonuses : l_Item.m_Bonuses)
                    ss << l_Bonuses << " ";
                ss << "')";
            }
        }

        if (!l_Quest.second.m_CurrencyRewards.empty())
        {
            for (auto const& l_Currecy : l_Quest.second.m_CurrencyRewards)
            {
                if (l_First)
                    l_First = false;
                else
                    ss << ", ";

                ss << "(" << m_Player->GetGUIDLow() << ", " << l_Quest.first << ", " << static_cast<uint32>(Globals::LoadType::Currency) << ", " << l_Currecy.m_ID << ", " << l_Currecy.m_Count << ", 0, '')";
            }
        }
    }

    if (l_First)
        return;

    ss << ";";
    p_Trans->PAppend(ss.str().c_str());
}

/// Dungeon wquests need to be handled differently
uint8 g_ItemLevelArray[] =
{
    static_cast<uint8>(ItemContext::Ilvl885),
    static_cast<uint8>(ItemContext::Ilvl890),
    static_cast<uint8>(ItemContext::Ilvl895),
    static_cast<uint8>(ItemContext::Ilvl900),
    static_cast<uint8>(ItemContext::Ilvl905),
    static_cast<uint8>(ItemContext::Ilvl910),
    static_cast<uint8>(ItemContext::Ilvl915),
    static_cast<uint8>(ItemContext::Ilvl920),
    static_cast<uint8>(ItemContext::Ilvl925),
    static_cast<uint8>(ItemContext::Ilvl930),
    static_cast<uint8>(ItemContext::Ilvl930),
    static_cast<uint8>(ItemContext::Ilvl935),
    static_cast<uint8>(ItemContext::Ilvl940),
    static_cast<uint8>(ItemContext::Ilvl945),
    static_cast<uint8>(ItemContext::Ilvl950),
    static_cast<uint8>(ItemContext::Ilvl955),

};

uint8 MS::WorldQuest::Manager::GenerateItemContext(WorldQuestType p_Type, ItemTemplate const* p_Item, std::vector<uint32>& p_Bonuses, BattlegroundBracketType p_Bracket /*= BattlegroundBracketType::Max */) const
{
    uint8 l_Index = 0;
    uint32 l_ChanceToProc = 0;

    /// Only Relic and Gear items proc
    if (!p_Item->isArmorOrWeapon() && !p_Item->IsArtifactRelic())
        return 0;

    switch (p_Type)
    {
        case WorldQuestType::Bounty:
        case WorldQuestType::RareEliteWorldQuest:
        case WorldQuestType::LegionInvasionEliteWQ:
            l_ChanceToProc = 33;
            break;
        case WorldQuestType::RareWorldQuest:
        case WorldQuestType::LegionInvasionWQ:
        case WorldQuestType::Dungeon:
            l_ChanceToProc = 25;
            break;
        case WorldQuestType::DailyPvP:
        case WorldQuestType::RegularWorldQuest:
        default:
            l_ChanceToProc = 20;
            break;
    }

    int32 l_Ilvl = Globals::BaseItemLevel;

    switch (p_Type)
    {
        case WorldQuestType::DailyPvP:
        {
            switch (p_Bracket)
            {
                case BattlegroundBracketType::Arena2v2:
                case BattlegroundBracketType::Arena3v3:
                case BattlegroundBracketType::Battleground10v10:
                {
                    int32 l_Rating = m_Player->GetArenaPersonalRating((uint8)p_Bracket);

                    std::map<uint16/*Rating*/, uint16> l_RewardIlevelBaseForRating =
                    {
                        { 1399, Globals::ItemMaxItemLevelRatedPvP       },
                        { 1599, Globals::ItemMaxItemLevelRatedPvP + 5   },
                        { 1799, Globals::ItemMaxItemLevelRatedPvP + 10  },
                        { 1999, Globals::ItemMaxItemLevelRatedPvP + 15  },
                        { 2199, Globals::ItemMaxItemLevelRatedPvP + 20  },
                        { 2399, Globals::ItemMaxItemLevelRatedPvP + 25  },
                        { 3999, Globals::ItemMaxItemLevelRatedPvP + 30  }
                    };

                    for (auto l_Step : l_RewardIlevelBaseForRating)
                    {
                        if (l_Step.first < l_Rating)
                            continue;

                        l_Ilvl = l_Step.second;
                        break;
                    }

                    break;
                }
                default:
                    break;
            }

            break;
        }
        default:
            break;
    }

    l_Index = (l_Ilvl - Globals::BaseItemLevel) / 5;

    /// Chance to get +5 ilvl
    if (urand(0, 100) <= l_ChanceToProc && p_Type != WorldQuestType::DailyPvP)
    {
        l_Index++;
        l_Ilvl += 5;
    }

    if (p_Type != WorldQuestType::DailyPvP)
        p_Bonuses.push_back(ItemBonus::Bonuses::WorldQuestRare);

    p_Bonuses.push_back(ItemBonus::Bonuses::WorldQuestEpic);

    int32 l_BaseIlvl = p_Item->GetBaseItemLevel();
    int32 l_IlvlModification = l_Ilvl - l_BaseIlvl;

    if (l_IlvlModification >= ItemBonus::g_ItemBonusIlvlNegativeLimit && l_IlvlModification <= ItemBonus::g_ItemBonusIlvlPositiveLimit)
        p_Bonuses.push_back((int32)ItemBonus::Bonuses::IlvlModificationBase + l_IlvlModification);
    else if (l_IlvlModification > 200 && l_IlvlModification <= 400)
        p_Bonuses.push_back((int32)ItemBonus::Bonuses::IlvlModificationBaseOver200 + (l_IlvlModification - 201));

    if (l_Index > 14)
        l_Index = 14;

    return g_ItemLevelArray[l_Index];
}

void MS::WorldQuest::Manager::GenerateWeeklyRatedPvpRewards()
{
    static std::unordered_map<uint32, BattlegroundBracketType> g_RatedBracketWeeklyUIQuest =
    {
        { 45536, BattlegroundBracketType::Arena2v2           },
        { 45537, BattlegroundBracketType::Arena3v3           },
        { 45538, BattlegroundBracketType::Battleground10v10  }
    };

    static std::map<uint16, uint8> g_RewardContextForRating =
    {
        { 1400, 51 },  ///< 840
        { 1600, 51 },  ///< 845
        { 1800, 8  },  ///< 855
        { 2000, 38 },  ///< 865
        { 2200, 39 },  ///< 870

        { 2400, 40 },  ///< 875
        { 4000, 40 },  ///< 880
    };

    /// Show tooltips
    for (auto l_BracketUIQuestReward : g_RatedBracketWeeklyUIQuest)
    {
        uint32      l_QuestID = l_BracketUIQuestReward.first;
        uint32      l_Rating  = m_Player->GetBestRatingOfPreviousWeek((uint8)l_BracketUIQuestReward.second);
        uint8       l_Context = 0;

        for (auto l_Step : g_RewardContextForRating)
        {
            if (l_Step.first < l_Rating)
                continue;

            l_Context = l_Step.second;
            break;
        }

        std::vector<uint32> l_Bonuses;

        WorldQuestReward l_Reward;
        l_Reward.m_ItemRewards.push_back(WorldQuestReward::ItemReward(144455, 1, static_cast<int8>(l_Context), l_Bonuses));
        m_RewardList[l_QuestID] = std::move(l_Reward);
    }

    /// Show objectives in the tooltip (0/10 in 2v2 & 3v3 and 0/3 in 10v10 rbg)
    static std::array<uint32, 3> g_RatedBracketWeeklyObjectivesQuest = { {44891, 44908, 44909} };
    for (auto l_QuestID : g_RatedBracketWeeklyObjectivesQuest)
    {
        Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_QuestID);
        if (!l_Quest || m_Player->HasQuest(l_QuestID) || m_Player->IsQuestRewarded(l_QuestID))
            continue;

        m_Player->AddQuest(l_Quest, m_Player);
    }
}
