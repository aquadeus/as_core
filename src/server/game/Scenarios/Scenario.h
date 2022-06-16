////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SCENARIO_H
#define SCENARIO_H

#include "Common.h"
#include "AchievementMgr.h"
#include "Challenge.h"

#include <mutex>

class Challenge;

struct ScenarioStepEntry;

typedef std::vector<ScenarioStepEntry const*> ScenarioSteps;

namespace WorldPackets
 {
     namespace Scenario
     {
         struct BonusObjectiveData;
     }
 }

enum ScenarioType
{
    SCENARIO_TYPE_SCENARIO          = 0,
    SCENARIO_TYPE_CHALLENGE_MODE    = 1,
    SCENARIO_TYPE_PROVING_GROUND    = 2,
    SCENARIO_TYPE_DUNGEON           = 3,
    SCENARIO_TYPE_RAID              = 4,
    SCENARIO_TYPE_SOLO              = 5
};

enum ScenarioStepState
{
    SCENARIO_STEP_INVALID       = 0,
    SCENARIO_STEP_NOT_STARTED   = 1,
    SCENARIO_STEP_IN_PROGRESS   = 2,
    SCENARIO_STEP_DONE          = 3
};

class Scenario
{
    public:
        Scenario(Map* map, LFGDungeonEntry const* p_DungeonData, Player* p_Player, bool p_Find, uint32 p_ScenarioGuid);
        ~Scenario();

        void CreateChallenge(Player* p_Player, uint32 p_MapChallengeID);

        uint32 GetInstanceId() const { return m_InstanceId; }
        uint32 GetScenarioGuid() const { return m_ScenarioGuid; }
        Map* GetMap() { return m_CurMap; }
        uint32 GetScenarioId() const { return m_ScenarioId; }
        uint32 GetCurrentStep() const { return m_CurrentStep; }

        void SetStepState(ScenarioStepEntry const* p_Step, ScenarioStepState p_State) { m_StepStates[p_Step] = p_State; }
        ScenarioStepState GetStepState(ScenarioStepEntry const* p_Step) const;

        bool IsCompleted() const;
        uint8 UpdateCurrentStep(bool p_Loading);

        void CompletedCriteriaTree(ScenarioStepEntry const* p_StepEntry);
        void CompleteStep(ScenarioStepEntry const* p_ScenarioStepEntry);

        void SetCurrentStep(uint8 p_Step);
        void Reward(uint32 p_RewardStep);

        ScenarioAchievementMgr& GetAchievementMgr() { return m_AchievementMgr; }
        ScenarioAchievementMgr const& GetAchievementMgr() const { return m_AchievementMgr; }
        void UpdateAchievementCriteria(CriteriaTypes p_Type, uint32 p_MiscValue1 = 0, uint32 m_MiscValue2 = 0, uint32 m_MiscValue3 = 0, Unit* p_Unit = nullptr, Player* p_ReferencePlayer = nullptr);
        void StartCriteriaTimer(CriteriaTimedTypes p_Type, uint32 p_Entry);
        void RemoveCriteriaTimer(CriteriaTimedTypes p_Type, uint32 p_Entry);

        std::vector<WorldPackets::Scenario::BonusObjectiveData> GetBonusObjectivesData();
        void SendStepUpdate(Player* p_Player = nullptr, bool p_Full = false);
        void SendCriteriaUpdate(Criteria const* p_Entry, CriteriaProgress const* p_Progress, uint32 timeElapsed = 0);
        void BroadCastPacket(const WorldPacket* p_Data);
        uint32 GetScenarioCriteriaByStep(uint8 p_Step);

        bool CanUpdateCriteria(uint32 p_CriteriaTreeId, uint32 p_RecursTree = 0) const;

        Challenge* const GetChallenge() { return m_Challenge; }

        bool IsViable() const { return m_IsViable; }

    protected:
        Challenge* m_Challenge;
        ScenarioEntry const* m_ScenarioEntry;
        std::map<ScenarioStepEntry const*, ScenarioStepState> m_StepStates;

        uint32 m_InstanceId;
        uint32 m_ScenarioId;
        uint32 m_ScenarioGuid;
        LFGDungeonEntry const* m_DungeonData;
        ScenarioAchievementMgr m_AchievementMgr;
        Map* m_CurMap;

        uint8 m_CurrentStep;
        uint32 m_CurrentTree;
        ScenarioSteps m_Steps;
        std::vector<uint32> ActiveSteps;

        bool m_Rewarded;
        bool m_IsViable;

        std::mutex m_ActiveStepsLock;
};

#endif
