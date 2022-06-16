////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Group.h"
#include "ScenarioMgr.h"
#include "LFGMgr.h"
#include "InstanceSaveMgr.h"
#include "WorldSession.h"
#include "ScenarioPackets.h"
#include "InstanceScript.h"
#include "Challenge.h"
#include "OutdoorPvEMgr.h"

Scenario::Scenario(Map* p_Map, LFGDungeonEntry const* p_DungeonData, Player* p_Player, bool p_Find, uint32 p_ScenarioGuid) : m_AchievementMgr(this)
{
    m_IsViable = false;

    if (!p_Map)
        return;

    m_CurMap        = p_Map;
    m_InstanceId    = p_Map->Instanceable() ? p_Map->GetInstanceId() : p_Map->GetInstanceZoneId();
    m_ScenarioGuid  = p_ScenarioGuid;
    m_DungeonData   = p_DungeonData;
    m_CurrentStep   = 0;
    m_CurrentTree     = 0;
    m_Rewarded      = false;
    m_ScenarioId    = m_DungeonData->ScenarioID;
    m_Challenge     = nullptr;

    if (!p_Find)
    {
        if (ScenarioData const* scenarioData = sObjectMgr->GetScenarioOnMap(m_DungeonData->map, p_Map->GetDifficultyID(), p_Player->GetTeam(), p_Player->getClass(), p_DungeonData->ID, p_Map->GetInstanceZoneId() & 0xFFFF))
            m_ScenarioId = scenarioData->ScenarioID;

        if (m_CurMap->IsChallengeMode())
        {
            switch (p_DungeonData->ID)
            {
                case 1475: ///< Upper Return To Karazhan
                {
                    m_ScenarioId = 1308;
                    break;
                }
                case 1474: ///< Lower Return To Karazhan
                {
                    m_ScenarioId = 1309;
                    break;
                }
            }
        }
        else if (p_Player->GetGroup() && p_Player->GetGroup()->isLFGGroup())
        {
            switch (p_DungeonData->ID)
            {
                case 1475: // low
                    m_ScenarioId = 1289;
                    break;
                case 1474: // up
                    m_ScenarioId = 1288;
                    break;
            }
        }
    }

    m_ScenarioEntry = sScenarioStore.LookupEntry(m_ScenarioId);
    if (!m_ScenarioEntry)
        return;

    m_IsViable = true;

    ScenarioSteps const* l_Steps = sScenarioMgr->GetScenarioSteps(m_ScenarioId);

    m_Steps = *l_Steps;
    m_CurrentTree = GetScenarioCriteriaByStep(m_CurrentStep);
    ActiveSteps.push_back(m_Steps[m_CurrentStep]->ID);

    for (auto const& l_Step : m_Steps)
    {
        SetStepState(l_Step, SCENARIO_STEP_NOT_STARTED);
    }

    if (m_CurMap->IsChallengeMode())
    {
        uint32 l_MapChallengeID = 0;

        switch (p_DungeonData->ID)
        {
            case 1475: ///< Upper Return To Karazhan
            {
                l_MapChallengeID = 234;
                break;
            }
            case 1474: ///< Lower Return To Karazhan
            {
                l_MapChallengeID = 227;
                break;
            }
            default:
            {
                for (auto& l_Pair : *GetLegionChallengeModeEntries())
                {
                    if (l_Pair.second->MapID == p_DungeonData->map)
                        l_MapChallengeID = l_Pair.second->ID;
                }

                break;
            }
        }

        CreateChallenge(p_Player, l_MapChallengeID);
    }
}

Scenario::~Scenario()
{
}

void Scenario::CreateChallenge(Player* p_Player, uint32 p_MapChallengeID)
{
    if (!p_Player)
        return;

    Map* l_Map = GetMap();
    if (!l_Map)
        return;

    m_Challenge = new Challenge(l_Map, p_Player, GetInstanceId(), p_MapChallengeID);

    if (!m_Challenge->m_CanRun)
    {
        m_Challenge = nullptr;
        return;
    }

    if (Map* map_ = GetMap())
    {
        if (InstanceMap* l_InstanceMap = map_->ToInstanceMap())
        {
            if (InstanceScript* l_Script = l_InstanceMap->GetInstanceScript())
            {
                l_Script->SetChallenge(m_Challenge);
                m_Challenge->SetInstanceScript(l_Script);
            }
        }
    }

    if (!m_Challenge->GetInstanceScript())
    {
        sLog->outExtChat("#jarvis", "danger", false, "Scenario::CreateChallenge can't set instance script for map challenge id %u", p_MapChallengeID);
        m_Challenge->m_CanRun = false;
        m_Challenge = nullptr;
        return;
    }

    if (ScenarioData const* l_ScenarioData = sObjectMgr->GetScenarioOnMap(l_Map->GetId(), Difficulty::DifficultyMythicKeystone, p_Player->GetTeam(), p_Player->getClass(), m_DungeonData->ID, l_Map->GetInstanceZoneId() & 0xFFFF))
        m_ScenarioId = l_ScenarioData->ScenarioID;

    switch (p_MapChallengeID)
    {
        case 227:
            m_ScenarioId = 1309;
            break;
        case 234: // upper kara
            m_ScenarioId = 1308;
            break;
    }

    m_ScenarioEntry = sScenarioStore.LookupEntry(m_ScenarioId);
    ASSERT(m_ScenarioEntry);

    ScenarioSteps const* l_Steps = sScenarioMgr->GetScenarioSteps(m_ScenarioId);
    ASSERT(l_Steps);

    if (l_Steps->size() <= m_CurrentStep || !m_Steps[m_CurrentStep])
    {
        sLog->outError(LOG_FILTER_CHALLENGE, "CreateChallenge: wrong step %u for scenario %u.", m_CurrentStep, m_ScenarioId);
        return;
    }

    m_Steps = *l_Steps;
    m_CurrentTree = GetScenarioCriteriaByStep(m_CurrentStep);
    ActiveSteps.clear();
    ActiveSteps.push_back(m_Steps[m_CurrentStep]->ID);

    for (auto const& l_Step : m_Steps)
        SetStepState(l_Step, SCENARIO_STEP_NOT_STARTED);

    SetCurrentStep(0);

    m_Challenge->SummonWall(p_Player);
    m_Challenge->Start();
}

bool Scenario::IsCompleted() const
{
    for (auto l_StepEntry : m_Steps)
    {
        if(l_StepEntry->IsBonusObjective())
            continue;

        if(GetStepState(l_StepEntry) != SCENARIO_STEP_DONE)
            return false;
    }

    return true;
}

void Scenario::SetCurrentStep(uint8 p_Step)
{
    m_CurrentStep = p_Step;

    if (m_CurrentStep < m_Steps.size())
        SetStepState(m_Steps[m_CurrentStep], SCENARIO_STEP_IN_PROGRESS);

    m_CurrentTree = GetScenarioCriteriaByStep(m_CurrentStep);

    SendStepUpdate();

    if (m_CurrentStep && m_CurrentStep < m_Steps.size())
    {
        if (Map* l_Map = GetMap())
        {
            sOutdoorPvEMgr->OnScenarioNextStep(l_Map, m_CurrentStep);

            if (InstanceMap* l_InstanceMap = l_Map->ToInstanceMap())
            {
                if (InstanceScript *l_Script = l_InstanceMap->GetInstanceScript())
                {
                    l_Script->setScenarioStep(m_CurrentStep);
                    l_Script->onScenarionNextStep(m_CurrentStep);
                    l_Script->UpdatePhasing();
                }
            }
        }

        ActiveSteps.push_back(m_Steps[m_CurrentStep]->ID);
    }
}

uint8 Scenario::UpdateCurrentStep(bool p_Loading)
{
    for (ScenarioSteps::const_iterator l_Itr = m_Steps.begin(); l_Itr != m_Steps.end(); ++l_Itr)
    {
        if (GetStepState((*l_Itr)) == SCENARIO_STEP_DONE)
            continue;

        CriteriaTreeEntry const* l_CriteriaTree = sCriteriaTreeStore.LookupEntry((*l_Itr)->CriteriaTreeID);
        if (!l_CriteriaTree)
            continue;

        if (GetAchievementMgr().IsCompletedCriteriaTree(sCriteriaMgr->GetCriteriaTree(l_CriteriaTree->ID)))
            CompletedCriteriaTree(*l_Itr);
    }

    return m_CurrentStep;
}

void Scenario::CompletedCriteriaTree(ScenarioStepEntry const* p_StepEntry)
{
    if (!p_StepEntry)
        return;

    if (!p_StepEntry->IsBonusObjective() && p_StepEntry->Step != m_CurrentStep)
    {
        return;
    }

    if (GetStepState(p_StepEntry) == SCENARIO_STEP_DONE)
    {
        return;
    }

    SetStepState(p_StepEntry, SCENARIO_STEP_DONE);
    CompleteStep(p_StepEntry);
}

void Scenario::CompleteStep(ScenarioStepEntry const* p_ScenarioStepEntry)
{
    if (p_ScenarioStepEntry->IsBonusObjective())
        return;

    ScenarioStepEntry const* l_NewStepEntry = nullptr;
    for (auto l_StepEntry : m_Steps)
    {
        if (l_StepEntry->IsBonusObjective())
            continue;

        const ScenarioStepState l_StepState = GetStepState(l_StepEntry);
        if (l_StepState == ScenarioStepState::SCENARIO_STEP_DONE)
        {
            continue;
        }

        if (!l_NewStepEntry || l_StepEntry->Step < l_NewStepEntry->Step)
            l_NewStepEntry = l_StepEntry;
    }

    if (l_NewStepEntry)
        SetCurrentStep(l_NewStepEntry->Step);

    SendStepUpdate();

    if (IsCompleted())
    {
        if (Map* l_Map = GetMap())
        {
            sOutdoorPvEMgr->OnScenarioComplete(l_Map);
        }

        Reward(p_ScenarioStepEntry->Step);
    }
}

uint32 Scenario::GetScenarioCriteriaByStep(uint8 p_Step)
{
    for (ScenarioSteps::const_iterator l_Itr = m_Steps.begin(); l_Itr != m_Steps.end(); ++l_Itr)
    {
        if (p_Step == (*l_Itr)->Step)
            return (*l_Itr)->CriteriaTreeID;
    }

    return 0;
}

void Scenario::Reward(uint32 p_RewardStep)
{
    if (m_Rewarded)
        return;

    m_Rewarded = true;

    uint64 l_GroupGuid = 0;

    Map* l_Map = GetMap();
    if (!l_Map)
        return;

    Quest const* l_Quest = sObjectMgr->GetQuestTemplate(m_Steps[p_RewardStep]->RewardQuestID);

    Map::PlayerList const& l_Players = l_Map->GetPlayers();
    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        Player* l_Player = l_Itr->getSource();
        if (!l_Player || !l_Player->IsInWorld())
            continue;
    
        GroupPtr l_Group = l_Player->GetGroup();

        if (l_GroupGuid == 0)
            l_GroupGuid = l_Group ? l_Group->GetGUID() : 0;

        if (m_Challenge)
            l_Player->UpdateCriteria(CRITERIA_TYPE_COMPLETE_CHALLENGE, m_Challenge->GetChallengeLevel());

        if (l_Quest)
            l_Player->RewardQuest(l_Quest, 0, nullptr, false);

        l_Player->UpdateCriteria(CRITERIA_TYPE_COMPLETE_SCENARIO, m_ScenarioId, 1);
        l_Player->UpdateCriteria(CRITERIA_TYPE_COMPLETE_SCENARIO_COUNT, 1);

        WorldPackets::Scenario::ScenarioCompleted data;
        data.ScenarioID = m_ScenarioId;
        l_Player->SendDirectMessage(data.Write());
    }

    // should not happen
    if (l_GroupGuid == 0 && m_ScenarioEntry->Type != SCENARIO_TYPE_CHALLENGE_MODE)
        return;

    switch (m_ScenarioEntry->Type)
    {
        case SCENARIO_TYPE_SCENARIO:
        case SCENARIO_TYPE_DUNGEON:
        case SCENARIO_TYPE_SOLO:
            if (uint32 l_DungeonId = sLFGMgr->GetDungeon(l_GroupGuid))
            {
                // lfg dungeon that we are in is not current scenario
                if (l_DungeonId != m_DungeonData->ID)
                    return;

                //sLFGMgr->RewardDungeonDoneFor(l_DungeonID, l_Player);
                //sLFGMgr->FinishDungeon(groupGuid, dungeonId);
            }
            break;
        case SCENARIO_TYPE_CHALLENGE_MODE:
            if (m_Challenge)
                m_Challenge->Complete();
            break;
        case SCENARIO_TYPE_RAID: ///< The Broken Shore scenario.
        default:
            break;
    }
}

ScenarioStepState Scenario::GetStepState(ScenarioStepEntry const* p_Step) const
{
    auto const l_Itr = m_StepStates.find(p_Step);
    if (l_Itr != m_StepStates.end())
        return l_Itr->second;

    return SCENARIO_STEP_INVALID;
}

std::vector<WorldPackets::Scenario::BonusObjectiveData> Scenario::GetBonusObjectivesData()
{
    std::vector<WorldPackets::Scenario::BonusObjectiveData> l_BonusObjectivesData;
    for (auto const& l_Step : m_Steps)
    {
        if (!l_Step->IsBonusObjective())
            continue;

        if (sCriteriaMgr->GetCriteriaTree(l_Step->CriteriaTreeID))
        {
            WorldPackets::Scenario::BonusObjectiveData bonusObjectiveData;
            bonusObjectiveData.BonusObjectiveID = l_Step->ID;
            bonusObjectiveData.ObjectiveComplete = GetStepState(l_Step) == SCENARIO_STEP_DONE;
            l_BonusObjectivesData.push_back(bonusObjectiveData);
        }
    }

    return l_BonusObjectivesData;
}

void Scenario::SendStepUpdate(Player* p_Player, bool p_Full)
{
    WorldPackets::Scenario::ScenarioState l_State;
    l_State.BonusObjectives  = GetBonusObjectivesData();
    l_State.ScenarioID       = GetScenarioId();
    l_State.CurrentStep      = m_CurrentStep < m_Steps.size() ? m_Steps[m_CurrentStep]->ID : -1;
    l_State.ScenarioComplete = IsCompleted();
    l_State.ActiveSteps      = ActiveSteps;

    std::vector<ScenarioSpellData> const* l_ScenariosSpells = sObjectMgr->GetScenarioSpells(GetScenarioId());
    if (l_ScenariosSpells)
    {
        for (std::vector<ScenarioSpellData>::const_iterator l_Itr = l_ScenariosSpells->begin(); l_Itr != l_ScenariosSpells->end(); ++l_Itr)
        {
            if ((*l_Itr).StepId == GetCurrentStep())
            {
                WorldPackets::Scenario::ScenarioState::ScenarioSpellUpdate l_SpellUpdate;
                l_SpellUpdate.Usable  = true;
                l_SpellUpdate.SpellID = (*l_Itr).Spells;
                l_State.Spells.emplace_back(l_SpellUpdate);
            }
        }
    }

    if (p_Full)
    {
        CriteriaProgressMap const* l_ProgressMap = GetAchievementMgr().GetCriteriaProgressMap();
        if (!l_ProgressMap->empty())
        {
            for (CriteriaProgressMap::const_iterator l_Itr = l_ProgressMap->begin(); l_Itr != l_ProgressMap->end(); ++l_Itr)
            {
                CriteriaProgress const&  l_TreeProgress      = l_Itr->second;
                CriteriaEntry const*     l_CriteriaEntry     = sCriteriaStore.LookupEntry(l_Itr->first);
                if (!l_CriteriaEntry)
                    continue;

                WorldPackets::Achievement::CriteriaProgress l_Progress;
                l_Progress.Id             = l_CriteriaEntry->ID;
                l_Progress.Quantity       = l_TreeProgress.Counter;
                l_Progress.Player         = l_TreeProgress.PlayerGUID;
                l_Progress.Flags          = 0;
                l_Progress.Date           = time(nullptr) - l_TreeProgress.Date;
                l_Progress.TimeFromStart  = time(nullptr) - l_TreeProgress.Date;
                l_Progress.TimeFromCreate = time(nullptr) - l_TreeProgress.Date;
                l_State.Progress.push_back(l_Progress);
            }
        }
    }

    if (p_Player)
        p_Player->SendDirectMessage(l_State.Write());
    else
        BroadCastPacket(l_State.Write());

    if (p_Full && m_Challenge)
    {
        m_Challenge->SendChallengeModeStart(p_Player);
        m_Challenge->SendStartElapsedTimer(p_Player);
    }
}

void Scenario::SendCriteriaUpdate(Criteria const* p_Criteria, CriteriaProgress const* p_Progress, uint32 timeElapsed /*= 0*/)
{
    if (!p_Criteria->Entry)
        return;

    WorldPackets::Scenario::ScenarioProgressUpdate l_Update;

    WorldPackets::Achievement::CriteriaProgress& progressUpdate = l_Update.Progress;
    progressUpdate.Id = p_Criteria->Entry->ID;
    progressUpdate.Quantity       = p_Progress->Counter;
    progressUpdate.Player         = p_Progress->PlayerGUID;
    progressUpdate.Flags          = 0;
    progressUpdate.Date           = p_Progress->Date;
    progressUpdate.TimeFromStart  = timeElapsed;
    progressUpdate.TimeFromCreate = timeElapsed;

    BroadCastPacket(l_Update.Write());
}

void Scenario::BroadCastPacket(const WorldPacket* p_Data)
{
    Map* l_Map = sMapMgr->FindMap(m_DungeonData->map, m_InstanceId);
    if (!l_Map)
    {
        l_Map = sMapMgr->FindBaseMap(m_DungeonData->map, m_InstanceId);
        if (!l_Map)
            return;
    }

    l_Map->SendToPlayers(p_Data);
}

bool Scenario::CanUpdateCriteria(uint32 p_CriteriaId, uint32 p_RecursTree /*=0*/) const
{
    auto const& l_CTreeList = sCriteriaMgr->GetCriteriaTreeList(p_RecursTree ? p_RecursTree : m_CurrentTree);
    if (!l_CTreeList)
        return false;

    for (std::vector<CriteriaTreeEntry const*>::const_iterator l_Itr = l_CTreeList->begin(); l_Itr != l_CTreeList->end(); ++l_Itr)
    {
        if (CriteriaTreeEntry const* l_CriteriaTree = *l_Itr)
        {
            if (l_CriteriaTree->CriteriaID == 0)
            {
                if (CanUpdateCriteria(p_CriteriaId, l_CriteriaTree->ID))
                    return true;
            }
            else if (l_CriteriaTree->ID == p_CriteriaId)
                return true;
        }
    }

    return false;
}

void Scenario::UpdateAchievementCriteria(CriteriaTypes p_Type, uint32 p_MiscValue1 /*= 0*/, uint32 p_MiscValue2 /*= 0*/, uint32 p_MiscValue3 /*= 0*/, Unit* p_Unit /*= nullptr*/, Player* p_ReferencePlayer /*= NULL*/)
{
    GetAchievementMgr().UpdateCriteria(p_Type, p_MiscValue1, p_MiscValue2, p_MiscValue3, p_Unit, p_ReferencePlayer);
}

void Scenario::StartCriteriaTimer(CriteriaTimedTypes p_Type, uint32 p_Entry)
{
    m_AchievementMgr.StartCriteriaTimer(p_Type, p_Entry);
}

void Scenario::RemoveCriteriaTimer(CriteriaTimedTypes p_Type, uint32 p_Entry)
{
    m_AchievementMgr.RemoveCriteriaTimer(p_Type, p_Entry);
}
