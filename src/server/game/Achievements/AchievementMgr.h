////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CriteriaHandler.h"
#include <bitset>

class Guild;
class Scenario;

#define LAST_ACHIEVEMENT_ID 15000

struct AchievementReward
{
    uint32 TitleId[2];
    uint32 ItemId;
    uint32 SenderCreatureId;
    std::string Subject;
    std::string Body;
    uint32 MailTemplateId;
    uint32 LearnSpell;
    uint32 CastSpell;
};

struct AchievementRewardLocale
{
    std::vector<std::string> Subject;
    std::vector<std::string> Body;
};

struct CompletedAchievementData
{
    std::time_t Date = std::time_t(0);
    std::set<uint64> CompletingPlayers;
    bool Changed;
};

class AchievementMgr : public CriteriaHandler
{
public:
    AchievementMgr();
    ~AchievementMgr();

    void CheckAllAchievementCriteria(Player* referencePlayer);

    virtual void CompletedAchievement(AchievementEntry const* entry, Player* referencePlayer) = 0;
    bool HasAchieved(uint32 achievementId) const;
    bool HasAccountAchieved(uint32 achievementId) const;
    uint32 GetAchievementProgress(uint32 p_AchievementID);
    uint32 GetAchievementPoints() const;
    bool IsCompletedAchievement(AchievementEntry const* entry, bool p_ForceAccountWide = false);

    CriteriaProgressMap const* GetCriteriaProgressMap()
    {
        return &_criteriaProgress;
    }

protected:
    bool CanUpdateCriteriaTree(Criteria const* criteria, CriteriaTree const* tree, Player* referencePlayer) const override;
    bool CanCompleteCriteriaTree(CriteriaTree const* tree) override;
    virtual void CompletedCriteriaTree(CriteriaTree const* tree, Player* referencePlayer) override;
    void AfterCriteriaTreeUpdate(CriteriaTree const* tree, Player* referencePlayer) override;

    bool RequiredAchievementSatisfied(uint32 achievementId) const override;

protected:
    std::unordered_map<uint32, CompletedAchievementData> _completedAchievements;
    std::unordered_map<uint32, CompletedAchievementData> m_AccountCompletedAchievements;
    std::bitset<LAST_ACHIEVEMENT_ID> m_CompletedAchievementsCheck;  ///< way faster to check a bitset than a unorderedmap
    std::bitset<LAST_ACHIEVEMENT_ID> m_AccountCompletedAchievementsCheck;  ///< way faster to check a bitset than a unorderedmap
    uint32 _achievementPoints;
};

class PlayerAchievementMgr : public AchievementMgr
{
public:
    explicit PlayerAchievementMgr(Player* owner);

    void Reset() override;

    static void DeleteFromDB(uint64 const& guid);
    /// Load achievement from database
    /// @p_AccountAchievementResult : Account wide achievement
    /// @p_AchievementResult        : Player local achievement
    /// @p_AccountCriteriaResult    : Account wide criterias progress
    /// @p_CriteriaResult           : Player local criterias progress
    void LoadFromDB(PreparedQueryResult p_AccountAchievementResult, PreparedQueryResult p_AchievementResult, PreparedQueryResult p_AccountCriteriaResult, PreparedQueryResult p_CriteriaResult);
    /// Save achievement to the database
    /// @p_AccountTransaction : Transaction for account wide data
    /// @p_CharacterTransaction : Transaction for character local data
    void SaveToDB(SQLTransaction& p_AccountTransaction, SQLTransaction& p_CharacterTransaction);

    void ResetCriteria(CriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, bool evenIfCriteriaComplete = false);

    void SendAllData(Player const* receiver) const override;
    void SendAchievementInfo(Player* receiver, uint32 achievementId = 0) const;

    void CompletedAchievement(AchievementEntry const* entry, Player* referencePlayer) override;

    bool ModifierTreeSatisfied(uint32 modifierTreeId) const;

protected:
    void SendCriteriaUpdate(Criteria const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const override;
    void SendCriteriaProgressRemoved(uint32 p_ID, Criteria const* p_Criteria = nullptr, CriteriaProgress const* p_Progress = nullptr) override;

    void SendAchievementEarned(AchievementEntry const* achievement) const;

    void SendPacket(WorldPacket* data) const override;

    std::string GetOwnerInfo() const override;
    CriteriaList const& GetCriteriaByType(CriteriaTypes type) const override;

    void CompletedCriteriaTree(CriteriaTree const* tree, Player* referencePlayer) override;

private:
    Player* _owner;
};

class GuildAchievementMgr : public AchievementMgr
{
public:
    explicit GuildAchievementMgr(Guild* owner);

    void Reset() override;

    static void DeleteFromDB(uint64 const& guid);
    void LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult);
    void SaveToDB(SQLTransaction& trans);

    void SendAllData(Player const* receiver) const override;
    void SendAchievementInfo(Player* receiver, uint32 achievementId = 0) const;
    void SendAllTrackedCriterias(Player* receiver, std::set<uint32> const& trackedCriterias) const;

    void CompletedAchievement(AchievementEntry const* entry, Player* referencePlayer) override;

protected:
    void SendCriteriaUpdate(Criteria const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const override;
    void SendCriteriaProgressRemoved(uint32 p_ID, Criteria const* p_Criteria = nullptr, CriteriaProgress const* p_Progress = nullptr) override;

    void SendAchievementEarned(AchievementEntry const* achievement) const;

    void SendPacket(WorldPacket* data) const override;

    std::string GetOwnerInfo() const override;
    CriteriaList const& GetCriteriaByType(CriteriaTypes type) const override;

private:
    Guild* _owner;
};

class ScenarioAchievementMgr : public AchievementMgr
{
public:
    explicit ScenarioAchievementMgr(Scenario* owner);

    void Reset() override;

    static void DeleteFromDB(uint64 const& guid);
    void LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult);
    void SaveToDB(SQLTransaction& trans);

    void SendAllData(Player const* receiver) const override;
    void SendAchievementInfo(Player* receiver, uint32 achievementId = 0) const;
    void SendAllTrackedCriterias(Player* receiver, std::set<uint32> const& trackedCriterias) const;

    void CompletedAchievement(AchievementEntry const* entry, Player* referencePlayer) override;

    bool CanCompleteCriteriaTree(CriteriaTree const* tree) override;

protected:
    void SendCriteriaUpdate(Criteria const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const override;
    void SendCriteriaProgressRemoved(uint32 p_ID, Criteria const* p_Criteria = nullptr, CriteriaProgress const* p_Progress = nullptr) override;

    void SendAchievementEarned(AchievementEntry const* achievement) const;

    void SendPacket(WorldPacket* data) const override;

    std::string GetOwnerInfo() const override;
    CriteriaList const& GetCriteriaByType(CriteriaTypes type) const override;

private:
    Scenario* _owner;
};

class AchievementGlobalMgr
{
    AchievementGlobalMgr() { }
    ~AchievementGlobalMgr() { }

public:
    static AchievementGlobalMgr* Instance();

    std::vector<AchievementEntry const*> const* GetAchievementByReferencedId(uint32 id) const;
    AchievementReward const* GetAchievementReward(AchievementEntry const* achievement) const;
    AchievementRewardLocale const* GetAchievementRewardLocale(AchievementEntry const* achievement) const;

    bool IsRealmCompleted(AchievementEntry const* achievement) const;
    void SetRealmCompleted(AchievementEntry const* achievement);

    void LoadAchievementReferenceList();
    void LoadCompletedAchievements();
    void LoadRewards();
    void LoadRewardLocales();

private:
    // store achievements by referenced achievement id to speed up lookup
    std::unordered_map<uint32, std::vector<AchievementEntry const*>> _achievementListByReferencedId;

    // store realm first achievements
    // std::chrono::system_clock::time_point::min() is a placeholder value for realm firsts not yet completed
    // std::chrono::system_clock::time_point::max() is a value assigned to realm firsts complete before worldserver started
    std::unordered_map<uint32, uint32> m_AllCompletedAchievements;

    std::unordered_map<uint32, AchievementReward> _achievementRewards;
    std::unordered_map<uint32, AchievementRewardLocale> _achievementRewardLocales;
};

#define sAchievementMgr AchievementGlobalMgr::Instance()