////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ContributionMgr.hpp"
#include "ScriptMgr.h"

ContributionMgr::ContributionMgr() : m_TimeToUpdate(0), m_TimeToSave(0)
{

}

Contribution::~Contribution()
{

}

Contribution::Contribution() : ID(0), StartTime(0), Progress(0),
    State(0, eContributionStates::MaxState), OccurenceValue(0, 1)
{

}

void Contribution::UpdateState(uint32 p_Time)
{
    StartTime = p_Time;
    sContributionMgr->UpdateWorldStates(this);
    sScriptMgr->OnContributionChangeState(this);
    SaveToDB();

    SessionMap const& l_Sessions = sWorld->GetAllSessions();
    sWorld->GetSessionsLock().acquire_read();
    for (SessionMap::const_iterator l_Itr = l_Sessions.begin(); l_Itr != l_Sessions.end(); ++l_Itr)
    {
        if (Player* l_Player = l_Itr->second->GetPlayer())
        {
            if (!l_Player->IsInWorld())
                continue;

            l_Player->AddCriticalOperation([l_Player]() -> void
            {
                l_Player->GetPhaseMgr().RemoveUpdateFlag(PHASE_UPDATE_FLAG_ZONE_UPDATE);
            });
        }
    }
    sWorld->GetSessionsLock().release();
}

void Contribution::SaveToDB()
{
    PreparedStatement* l_UpdateStmt = CharacterDatabase.GetPreparedStatement(CharacterDatabaseStatements::CHAR_UPD_CONTRIBUTIONS);
    uint32 l_Field = 0;
    l_UpdateStmt->setUInt32(l_Field++, State);
    l_UpdateStmt->setUInt32(l_Field++, StartTime);
    l_UpdateStmt->setUInt32(l_Field++, Progress);
    l_UpdateStmt->setUInt32(l_Field++, OccurenceValue);
    l_UpdateStmt->setUInt32(l_Field++, ID);

    CharacterDatabase.Execute(l_UpdateStmt);
}

uint32 Contribution::GetTotalProgress(uint32 p_Time) const
{
    uint32 l_MinutesElapsed = (p_Time - StartTime) / TimeConstants::MINUTE;
    return Progress + (Data->AccumulationAmountPerMinute * l_MinutesElapsed);
}

uint32 Contribution::GetTotalDepletion(uint32 p_Time) const
{
    uint32 l_MinutesElapsed = (p_Time - StartTime) / TimeConstants::MINUTE;
    return Data->DepletionAmountPerMinute * l_MinutesElapsed;
}

uint32 Contribution::GetEndStateTime() const
{
    switch (State)
    {
        case eContributionStates::Active:
        {
            return StartTime + Data->UpTimeSecs;
        }
        case eContributionStates::UnderAttack:
        {
            uint32 l_TotalTime = Data->DepletionStateTargetValue / Data->DepletionAmountPerMinute;
            return StartTime + (l_TotalTime * TimeConstants::MINUTE);
        }
        case eContributionStates::Destroyed:
        {
            return StartTime + Data->DownTimeSecs;
        }
        default:
            break;
    }

    return 0;
}

bool Contribution::IsActive() const
{
    switch (State)
    {
        case eContributionStates::Active:
        case eContributionStates::UnderAttack:
            return true;
        default:
            break;
    }

    return false;
}


bool Contribution::IsAfterHalfOfCurrentState() const
{
    uint32 l_EndTime = GetEndStateTime();
    if (!l_EndTime)
        return false;

    uint32 l_Now = sWorld->GetGameTime();
    return l_EndTime - l_Now < l_Now - StartTime;
}

uint32 Contribution::GetHalfCurrentStateTimer() const
{
    uint32 l_EndTime = GetEndStateTime();
    if (!l_EndTime)
        return 0;

    return StartTime + ((l_EndTime - StartTime) / 2);
}

void ContributionMgr::Load()
{
    uint32 l_OldMSTime = getMSTime();

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CONTRIBUTIONS);
    PreparedQueryResult l_Result = CharacterDatabase.Query(l_Stmt);
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 contributions. DB table `contributions` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        uint8 l_I = 0;
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        Contribution l_Contribution;

        l_Contribution.ID             = l_Fields[l_I++].GetUInt32();
        l_Contribution.State          = l_Fields[l_I++].GetUInt32();
        l_Contribution.StartTime      = l_Fields[l_I++].GetUInt32();
        l_Contribution.Progress       = l_Fields[l_I++].GetUInt32();
        uint32 l_OcurrenceValue       = l_Fields[l_I++].GetUInt32();

        ContributionEntry const* l_ContributionEntry = sContributionStore.LookupEntry(l_Contribution.ID);
        if (!l_ContributionEntry)
            continue;

        ManagedWorldStateInputEntry const* l_ManagedWorldStateInputEntry = sManagedWorldStateInputStore.LookupEntry(l_ContributionEntry->ManagedWorldStateInputID);
        if (!l_ManagedWorldStateInputEntry)
            continue;

        ManagedWorldStateEntry const* l_ManagedWorldStateEntry = sManagedWorldStateStore.LookupEntry(l_ManagedWorldStateInputEntry->ManagedWorldStateID);
        if (!l_ManagedWorldStateEntry)
            continue;

        PlayerConditionEntry const* l_PlayerConditionEntry = sPlayerConditionStore.LookupEntry(l_ManagedWorldStateInputEntry->ValidInputConditionID);
        if (!l_PlayerConditionEntry)
            continue;

        l_Contribution.RequiredCurrency.first   = l_PlayerConditionEntry->CurrencyID[0];
        l_Contribution.RequiredCurrency.second  = l_PlayerConditionEntry->CurrencyCount[0];

        l_Contribution.Data = l_ManagedWorldStateEntry;
        l_Contribution.InputData = l_ManagedWorldStateInputEntry;

        int32 l_MaxOccurenceValue = 0;
        for (uint32 l_I = 0; l_I < sManagedWorldStateBuffStore.GetNumRows(); ++l_I)
        {
            ManagedWorldStateBuffEntry const* l_ManageWorldStateBuffEntry = sManagedWorldStateBuffStore.LookupEntry(l_I);
            if (!l_ManageWorldStateBuffEntry)
                continue;

            if (l_ManageWorldStateBuffEntry->ManagedWorldStateID != l_ManagedWorldStateEntry->ID)
                continue;

            l_Contribution.BuffData[l_ManageWorldStateBuffEntry->OccurenceValue] = l_ManageWorldStateBuffEntry;

            l_MaxOccurenceValue = std::max(l_MaxOccurenceValue, l_ManageWorldStateBuffEntry->OccurenceValue);
        }

        if (!l_MaxOccurenceValue)
            continue;

        l_Contribution.OccurenceValue = Occurence(l_OcurrenceValue, l_MaxOccurenceValue - 1);

        m_Contributions[l_Contribution.ID] = l_Contribution;
        sScriptMgr->OnContributionChangeState(&l_Contribution);

    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u contributions in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ContributionMgr::Update(uint32 const p_Diff)
{
    m_TimeToUpdate -= p_Diff;
    m_TimeToSave   -= p_Diff;
    if (m_TimeToUpdate > 0)
        return;

    m_TimeToUpdate = CONTRIBUTION_UPDATE_DELAY;

    uint32 l_Time = sWorld->GetGameTime();

    std::lock_guard<std::recursive_mutex> l_Guard(m_ContributionsLock);
    sScriptMgr->UpdateContributions(l_Time);

    for (std::pair<const uint32, Contribution>& l_Contribution : m_Contributions)
    {
        switch (l_Contribution.second.State)
        {
            case eContributionStates::Building:
            {
                if (l_Contribution.second.GetTotalProgress(l_Time) < l_Contribution.second.Data->AccumulationStateTargetValue)
                    continue;

                break;
            }
            case eContributionStates::Active:
            {
                if (l_Contribution.second.StartTime + l_Contribution.second.Data->UpTimeSecs > l_Time)
                    continue;

                l_Contribution.second.Progress = 0;

                break;
            }
            case eContributionStates::UnderAttack:
            {
                if (l_Contribution.second.GetTotalDepletion(l_Time) < l_Contribution.second.Data->DepletionStateTargetValue)
                    continue;

                break;
            }
            case eContributionStates::Destroyed:
            {
                if (l_Contribution.second.StartTime + l_Contribution.second.Data->DownTimeSecs > l_Time)
                    continue;

                break;
            }
            default:
                continue;
        }

        l_Contribution.second.State++;
        l_Contribution.second.UpdateState(l_Time);
    }

    if (m_TimeToSave > 0)
        return;

    m_TimeToSave = CONTRIBUTION_SAVE_DELAY;

    for (std::pair<const uint32, Contribution>& l_Contribution : m_Contributions)
        l_Contribution.second.SaveToDB();
}

uint32 ContributionMgr::GetStartTime(uint32 p_ContributionID) const
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_ContributionsLock);
    auto l_Itr = m_Contributions.find(p_ContributionID);
    if (l_Itr == m_Contributions.end())
        return 0;

    return l_Itr->second.StartTime;
}

uint32 ContributionMgr::GetOccurenceValue(uint32 p_ContributionID) const
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_ContributionsLock);
    auto l_Itr = m_Contributions.find(p_ContributionID);
    if (l_Itr == m_Contributions.end())
        return 0;

    return l_Itr->second.OccurenceValue;
}

void ContributionMgr::PlayerContribution(Player* p_Player, uint32 p_ContributionID)
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_ContributionsLock);
    if (m_Contributions.find(p_ContributionID) == m_Contributions.end())
        return;

    Contribution& l_Contribution = m_Contributions[p_ContributionID];

    if (p_Player->GetCurrency(l_Contribution.RequiredCurrency.first, true) < l_Contribution.RequiredCurrency.second)
        return;

    if (l_Contribution.State != eContributionStates::Building)
        return;

    const Quest* l_TrackerQuest = sObjectMgr->GetQuestTemplate(l_Contribution.InputData->QuestID);
    if (!l_TrackerQuest)
        return;

    l_Contribution.Progress += l_Contribution.RequiredCurrency.second;
    p_Player->RewardQuest(l_TrackerQuest, 0, p_Player);
    p_Player->AddQuest(l_TrackerQuest, p_Player);
    sScriptMgr->OnPlayerContribute(p_Player, &l_Contribution);

    uint32 l_Time = sWorld->GetGameTime();
    if (l_Contribution.GetTotalProgress(l_Time) < l_Contribution.Data->AccumulationStateTargetValue)
        return;

    l_Contribution.State = eContributionStates::Active;
    l_Contribution.OccurenceValue++;
    l_Contribution.UpdateState(l_Time);
}

void ContributionMgr::DebugContribution(uint32 p_ContributionID, uint32 p_State, uint32 p_Progress, uint32 p_OccurenceValue)
{
    if (m_Contributions.find(p_ContributionID) == m_Contributions.end())
        return;

    Contribution& l_Contribution = m_Contributions[p_ContributionID];
    l_Contribution.State = p_State;
    l_Contribution.Progress = p_Progress;
    l_Contribution.OccurenceValue = p_OccurenceValue;
    l_Contribution.UpdateState(sWorld->GetGameTime());
}

void ContributionMgr::InitializeForPlayer(Player* p_Player)
{
    uint32 l_Time = sWorld->GetGameTime();

    std::lock_guard<std::recursive_mutex> l_Guard(m_ContributionsLock);
    for (auto l_Itr = m_Contributions.begin(); l_Itr != m_Contributions.end(); ++l_Itr)
    {
        if (l_Itr->second.State == 0 || l_Itr->second.State > eContributionStates::MaxState)
            continue;

        p_Player->SendUpdateWorldState(l_Itr->second.Data->CurrentStageWorldStateID, l_Itr->second.State);
        p_Player->SendUpdateWorldState(l_Itr->second.Data->OccurencesWorldStateID, l_Itr->second.OccurenceValue);

        if (l_Itr->second.GetTotalProgress(l_Time))
            p_Player->SendUpdateWorldState(l_Itr->second.Data->ProgressWorldStateID, l_Itr->second.GetTotalProgress(l_Time));
        else
            p_Player->SendUpdateWorldState(l_Itr->second.Data->ProgressWorldStateID, l_Itr->second.GetEndStateTime());

        if (!p_Player->HasQuest(l_Itr->second.InputData->QuestID))
        {
            if (const Quest* l_TrackerQuest = sObjectMgr->GetQuestTemplate(l_Itr->second.InputData->QuestID))
                p_Player->AddQuest(l_TrackerQuest, p_Player);
        }
    }
}

void ContributionMgr::UpdateWorldStates(Contribution const* p_Contribution) const
{
    uint32 l_Time = sWorld->GetGameTime();
    SessionMap const& l_Sessions = sWorld->GetAllSessions();
    sWorld->GetSessionsLock().acquire_read();
    for (SessionMap::const_iterator l_Itr = l_Sessions.begin(); l_Itr != l_Sessions.end(); ++l_Itr)
    {
        if (Player* l_Player = l_Itr->second->GetPlayer())
        {
            if (!l_Player->IsInWorld())
                continue;

            l_Player->SendUpdateWorldState(p_Contribution->Data->CurrentStageWorldStateID, p_Contribution->State);
            l_Player->SendUpdateWorldState(p_Contribution->Data->OccurencesWorldStateID, p_Contribution->OccurenceValue);

            if (p_Contribution->GetTotalProgress(l_Time))
                l_Player->SendUpdateWorldState(p_Contribution->Data->ProgressWorldStateID, p_Contribution->GetTotalProgress(l_Time));
            else
                l_Player->SendUpdateWorldState(p_Contribution->Data->ProgressWorldStateID, p_Contribution->GetEndStateTime());
        }
    }
    sWorld->GetSessionsLock().release();
}

Contribution ContributionMgr::GetContribution(uint32 p_ContributionID) const
{
    Contribution l_Copy;
    l_Copy.ID = 0;

    std::lock_guard<std::recursive_mutex> l_Guard(m_ContributionsLock);
    auto l_Itr = m_Contributions.find(p_ContributionID);
    if (l_Itr == m_Contributions.end())
        return l_Copy;

    l_Copy = l_Itr->second;

    return l_Copy;
}

uint32 ContributionMgr::CalculateActiveDuration(Contribution const* p_Contribution) const
{
    return p_Contribution->Data->UpTimeSecs + (p_Contribution->Data->DepletionStateTargetValue / p_Contribution->Data->DepletionAmountPerMinute) * TimeConstants::MINUTE;
}
