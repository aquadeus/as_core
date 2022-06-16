////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "WorldSession.h"
#include "Common.h"

#define CONTRIBUTION_UPDATE_DELAY   5 * IN_MILLISECONDS
#define CONTRIBUTION_SAVE_DELAY     60 * IN_MILLISECONDS

enum eContributionStates
{
    Building = 1,
    Active,
    UnderAttack,
    Destroyed,

    MaxState = Destroyed
};

struct Contribution
{
    public:
        Contribution();
        ~Contribution();

        void UpdateState(uint32 p_Time);

        uint32 GetTotalProgress(uint32 p_Time) const;
        uint32 GetTotalDepletion(uint32 p_Time) const;
        uint32 GetEndStateTime() const;

        bool IsActive() const;

        bool IsAfterHalfOfCurrentState() const;
        uint32 GetHalfCurrentStateTimer() const;

        void SaveToDB();

        operator bool() const { return ID != 0; }

    public: ///< DB Datas
        uint32 ID;
        Occurence State;
        uint32 Progress;
        uint32 StartTime;
        Occurence OccurenceValue;

    public: ///< DBC Datas
        ManagedWorldStateEntry const* Data;
        ManagedWorldStateInputEntry const* InputData;
        std::map<uint32, ManagedWorldStateBuffEntry const*> BuffData;
        std::pair<uint32, uint32> RequiredCurrency;
};

class ContributionMgr
{
    public:
        ContributionMgr();

        void Load();
        void Update(uint32 const p_Diff);
        void InitializeForPlayer(Player* p_Player);

        uint32 GetStartTime(uint32 p_ContributionID) const;
        uint32 GetOccurenceValue(uint32 p_ContributionID) const;

        void PlayerContribution(Player* p_Player, uint32 p_ContributionID);

        void DebugContribution(uint32 p_ContributionID, uint32 p_State, uint32 p_Progress, uint32 p_OccurenceValue);

        void UpdateWorldStates(Contribution const* p_Contribution) const;

        Contribution GetContribution(uint32 p_ContributionID) const;

        uint32 CalculateActiveDuration(Contribution const* p_Contribution) const;

    private:
        mutable std::recursive_mutex m_ContributionsLock;
        std::map<uint32, Contribution> m_Contributions;

        int32 m_TimeToUpdate;
        int32 m_TimeToSave;
};

#define sContributionMgr ACE_Singleton<ContributionMgr, ACE_Null_Mutex>::instance()
