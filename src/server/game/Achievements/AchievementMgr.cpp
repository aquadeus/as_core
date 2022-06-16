////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#include "AchievementMgr.h"
#include "AchievementPackets.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "GuildMgr.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Scenario.h"

struct VisibleAchievementCheck
{
    AchievementEntry const* operator()(std::pair<uint32, CompletedAchievementData> const& val)
    {
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(val.first);
        if (achievement && !(achievement->Flags & ACHIEVEMENT_FLAG_HIDDEN))
            return achievement;
        return nullptr;
    }
};

namespace JadeCore
{
    class AchievementChatBuilder
    {
    public:
        AchievementChatBuilder(Player const& p_Player, ChatMsg p_MsgType, int32 p_TextID, uint32 p_AchievementID)
            : m_Player(p_Player), m_MsgType(p_MsgType), m_TextID(p_TextID), m_AchievementID(p_AchievementID) { }
        void operator()(WorldPacket& p_Data, LocaleConstant p_LocIDX)
        {
            char const* l_Text = sObjectMgr->GetTrinityString(m_TextID, p_LocIDX);

            ChatHandler::FillMessageData(&p_Data, m_Player.GetSession(), m_MsgType, LANG_UNIVERSAL, NULL, m_Player.GetGUID(), l_Text, NULL, NULL, m_AchievementID);
        }

    private:
        Player const& m_Player;
        ChatMsg m_MsgType;
        int32 m_TextID;
        uint32 m_AchievementID;
    };
}
AchievementMgr::AchievementMgr() : _achievementPoints(0) { }

AchievementMgr::~AchievementMgr() { }

/**
* called at player login. The player might have fulfilled some achievements when the achievement system wasn't working yet
*/
void AchievementMgr::CheckAllAchievementCriteria(Player* referencePlayer)
{
    // suppress sending packets
    for (uint32 i = 0; i < CRITERIA_TYPE_TOTAL; ++i)
        UpdateCriteria(CriteriaTypes(i), 0, 0, 0, NULL, referencePlayer);
}

bool AchievementMgr::HasAchieved(uint32 achievementId) const
{
    return m_CompletedAchievementsCheck.test(achievementId);
}

bool AchievementMgr::HasAccountAchieved(uint32 achievementId) const
{
    return HasAchieved(achievementId) || m_AccountCompletedAchievementsCheck.test(achievementId);
}

uint32 AchievementMgr::GetAchievementProgress(uint32 p_AchievementID)
{
    AchievementEntry const* l_Entry = sAchievementStore.LookupEntry(p_AchievementID);
    if (!l_Entry)
        return 0;

    if (!(l_Entry->Flags & AchievementFlags::ACHIEVEMENT_FLAG_COUNTER))
        return HasAchieved(p_AchievementID);

    CriteriaTree const* l_Tree = sCriteriaMgr->GetCriteriaTree(l_Entry->CriteriaTree);
    if (!l_Tree)
        return false;

    uint64 l_Progress = 0;
    CriteriaMgr::WalkCriteriaTree(l_Tree, [this, &l_Progress](CriteriaTree const* p_CriteriaTree)
    {
        if (p_CriteriaTree->Criteria)
        {
            if (CriteriaProgress const* l_CriteriaProgress = GetCriteriaProgress(p_CriteriaTree->Criteria))
                l_Progress += l_CriteriaProgress->Counter;
        }
    });

    return l_Progress >= l_Tree->Entry->Amount;
}

uint32 AchievementMgr::GetAchievementPoints() const
{
    return _achievementPoints;
}

bool AchievementMgr::CanUpdateCriteriaTree(Criteria const* criteria, CriteriaTree const* tree, Player* referencePlayer) const
{
    AchievementEntry const* achievement = tree->Achievement;

    if (achievement)
    {
        if (HasAchieved(achievement->ID))
        {
            sLog->outTrace(LOG_FILTER_ACHIEVEMENTSYS, "AchievementMgr::CanUpdateCriteriaTree: (Id: %u Type %s Achievement %u) Achievement already earned",
                criteria->ID, CriteriaMgr::GetCriteriaTypeString(criteria->Entry->Type), achievement->ID);
            return false;
        }

        if (achievement->MapID != -1 && referencePlayer->GetMapId() != uint32(achievement->MapID))
        {
            sLog->outTrace(LOG_FILTER_ACHIEVEMENTSYS, "AchievementMgr::CanUpdateCriteriaTree: (Id: %u Type %s Achievement %u) Wrong map",
                criteria->ID, CriteriaMgr::GetCriteriaTypeString(criteria->Entry->Type), achievement->ID);
            return false;
        }

        if ((achievement->Faction == ACHIEVEMENT_FACTION_HORDE    && referencePlayer->GetTeam() != HORDE) ||
            (achievement->Faction == ACHIEVEMENT_FACTION_ALLIANCE && referencePlayer->GetTeam() != ALLIANCE))
        {
            sLog->outTrace(LOG_FILTER_ACHIEVEMENTSYS, "AchievementMgr::CanUpdateCriteriaTree: (Id: %u Type %s Achievement %u) Wrong faction",
                criteria->ID, CriteriaMgr::GetCriteriaTypeString(criteria->Entry->Type), achievement->ID);
            return false;
        }
    }

    return CriteriaHandler::CanUpdateCriteriaTree(criteria, tree, referencePlayer);
}

bool AchievementMgr::CanCompleteCriteriaTree(CriteriaTree const* tree)
{
    AchievementEntry const* achievement = tree->Achievement;
    if (!achievement)
        return true;

    // counter can never complete
    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return false;

    if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
    {
#ifdef CROSS
        // TODO: create LoadCompletedAchievements for the cross server (collecting and synchronizing achievements from all game servers)
        return false;
#else
        // someone on this realm has already completed that achievement
        if (sAchievementMgr->IsRealmCompleted(achievement))
            return false;
#endif
    }

    return true;
}

void PlayerAchievementMgr::CompletedCriteriaTree(CriteriaTree const* tree, Player* referencePlayer)
{
    if (_owner)
        _owner->QuestObjectiveSatisfy(tree->ID, tree->Entry->Amount, QUEST_OBJECTIVE_TYPE_CRITERIA_TREE, _owner->GetGUID(), true);

    AchievementMgr::CompletedCriteriaTree(tree, referencePlayer);
}

void AchievementMgr::CompletedCriteriaTree(CriteriaTree const* tree, Player* referencePlayer)
{
    AchievementEntry const* achievement = tree->Achievement;
    if (!achievement)
        return;

    // counter can never complete
    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return;

    // already completed and stored
    if (HasAchieved(achievement->ID))
        return;

    if (IsCompletedAchievement(achievement))
        CompletedAchievement(achievement, referencePlayer);
}

void AchievementMgr::AfterCriteriaTreeUpdate(CriteriaTree const* tree, Player* referencePlayer)
{
    AchievementEntry const* achievement = tree->Achievement;
    if (!achievement)
        return;

    // check again the completeness for SUMM and REQ COUNT achievements,
    // as they don't depend on the completed criteria but on the sum of the progress of each individual criteria
    if (achievement->Flags & ACHIEVEMENT_FLAG_SUMM)
        if (IsCompletedAchievement(achievement))
            CompletedAchievement(achievement, referencePlayer);

    if (std::vector<AchievementEntry const*> const* achRefList = sAchievementMgr->GetAchievementByReferencedId(achievement->ID))
        for (AchievementEntry const* refAchievement : *achRefList)
            if (IsCompletedAchievement(refAchievement))
                CompletedAchievement(refAchievement, referencePlayer);
}

bool AchievementMgr::IsCompletedAchievement(AchievementEntry const* entry, bool p_ForceAccountWide)
{
    // counter can never complete
    if (entry->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return false;

    CriteriaTree const* tree = sCriteriaMgr->GetCriteriaTree(entry->CriteriaTree);
    if (!tree)
        return false;

    // For SUMM achievements, we have to count the progress of each criteria of the achievement.
    // Oddly, the target count is NOT contained in the achievement, but in each individual criteria
    if (entry->Flags & ACHIEVEMENT_FLAG_SUMM)
    {
        uint64 progress = 0;
        CriteriaMgr::WalkCriteriaTree(tree, [this, &progress](CriteriaTree const* criteriaTree)
        {
            if (criteriaTree->Criteria)
                if (CriteriaProgress const* criteriaProgress = this->GetCriteriaProgress(criteriaTree->Criteria))
                    progress += criteriaProgress->Counter;
        });
        return progress >= tree->Entry->Amount;
    }

    return IsCompletedCriteriaTree(tree);
}

bool AchievementMgr::RequiredAchievementSatisfied(uint32 achievementId) const
{
    return HasAchieved(achievementId);
}

PlayerAchievementMgr::PlayerAchievementMgr(Player* owner) : _owner(owner)
{
}

void PlayerAchievementMgr::Reset()
{
    AchievementMgr::Reset();

    for (auto iter = _completedAchievements.begin(); iter != _completedAchievements.end(); ++iter)
    {
        WorldPackets::Achievement::AchievementDeleted achievementDeleted;
        achievementDeleted.AchievementID = iter->first;
        SendPacket(achievementDeleted.Write());
    }

    _completedAchievements.clear();
    m_CompletedAchievementsCheck.reset();
    _achievementPoints = 0;
    DeleteFromDB(_owner->GetGUID());

    // re-fill data
    CheckAllAchievementCriteria(_owner);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void PlayerAchievementMgr::DeleteFromDB(uint64 const& guid)
{
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT);
    stmt->setUInt32(0, GUID_LOPART(guid));
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_PROGRESS);
    stmt->setUInt32(0, GUID_LOPART(guid));
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

/// Load achievement from database
/// @p_AccountAchievementResult : Account wide achievement
/// @p_AchievementResult        : Player local achievement
/// @p_AccountCriteriaResult    : Account wide criterias progress
/// @p_CriteriaResult           : Player local criterias progress
void PlayerAchievementMgr::LoadFromDB(PreparedQueryResult p_AccountAchievementResult, PreparedQueryResult p_AchievementResult, PreparedQueryResult p_AccountCriteriaResult, PreparedQueryResult p_CriteriaResult)
{
    if (p_AccountAchievementResult)
    {
        do
        {
            Field* l_Fields = p_AccountAchievementResult->Fetch();
            uint32 l_AchievementID = l_Fields[0].GetUInt32();

            /// Must not happen: cleanup at server startup in sAchievementMgr->LoadCompletedAchievements()
            AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(l_AchievementID);
            if (!l_Achievement)
                continue;

            CompletedAchievementData& l_CompletedAchievement = m_AccountCompletedAchievements[l_AchievementID];
            l_CompletedAchievement.Date     = time_t(l_Fields[1].GetUInt32());
            l_CompletedAchievement.Changed  = false;

            if ((l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT) != 0)
            {
                m_CompletedAchievementsCheck.set(l_AchievementID);

                _achievementPoints += l_Achievement->Points;

                /// Title achievement rewards are retroactive
                if (AchievementReward const* l_Reward = sAchievementMgr->GetAchievementReward(l_Achievement))
                {
                    if (uint32 l_TitleID = l_Reward->TitleId[Player::TeamForRace(_owner->getRace()) == ALLIANCE ? 0 : 1])
                    {
                        if (CharTitlesEntry const* l_TitleEntry = sCharTitlesStore.LookupEntry(ObjectMgr::ChangeTitleIdForGender(l_TitleID, _owner->getGender())))
                            _owner->SetTitle(l_TitleEntry);
                    }
                }
            }
            else
                m_AccountCompletedAchievementsCheck.set(l_AchievementID);

        } while (p_AccountAchievementResult->NextRow());
    }

    if (p_AchievementResult)
    {
        do
        {
            Field* l_Fields         = p_AchievementResult->Fetch();
            uint32 l_AchievementID  = l_Fields[0].GetUInt32();

            /// Must not happen: cleanup at server startup in sAchievementMgr->LoadCompletedAchievements()
            AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(l_AchievementID);
            if (!l_Achievement)
                continue;

            CompletedAchievementData& l_CompletedAchievement = _completedAchievements[l_AchievementID];
            m_CompletedAchievementsCheck.set(l_AchievementID);
            l_CompletedAchievement.Date     = time_t(l_Fields[1].GetUInt32());
            l_CompletedAchievement.Changed  = false;

            if ((l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT) != 0)
                continue;

            _achievementPoints += l_Achievement->Points;

            /// Title achievement rewards are retroactive
            if (AchievementReward const* l_Reward = sAchievementMgr->GetAchievementReward(l_Achievement))
            {
                if (uint32 l_TitleID = l_Reward->TitleId[Player::TeamForRace(_owner->getRace()) == ALLIANCE ? 0 : 1])
                {
                    if (CharTitlesEntry const* l_TitleEntry = sCharTitlesStore.LookupEntry(l_TitleID))
                        _owner->SetTitle(l_TitleEntry);
                }
            }

        } while (p_AchievementResult->NextRow());
    }

    if (p_AccountCriteriaResult)
    {
        time_t l_NowTimeSec = time(NULL);
        do
        {
            Field* l_Fields = p_AccountCriteriaResult->Fetch();

            uint32 l_CriteriaID         = l_Fields[0].GetUInt32();
            uint64 l_CriteriaCounter    = l_Fields[1].GetUInt64();
            time_t l_CriteriaDate       = time_t(l_Fields[2].GetUInt32());

            Criteria const* l_Criteria = sCriteriaMgr->GetCriteria(l_CriteriaID);
            if (!l_Criteria)
                continue;

            if (l_Criteria->Entry->StartTimer && time_t(l_CriteriaDate + l_Criteria->Entry->StartTimer) < l_NowTimeSec)
                continue;

            CriteriaProgress& l_CriteriaProgress = _criteriaProgress[l_CriteriaID];
            l_CriteriaProgress.Counter  = l_CriteriaCounter;
            l_CriteriaProgress.Date     = l_CriteriaDate;
            l_CriteriaProgress.Changed  = false;
        } while (p_AccountCriteriaResult->NextRow());
    }

    if (p_CriteriaResult)
    {
        time_t l_NowTimeSec = time(NULL);
        do
        {
            Field* l_Fields = p_CriteriaResult->Fetch();

            uint32 l_CriteriaID         = l_Fields[0].GetUInt32();
            uint64 l_CriteriaCounter    = l_Fields[1].GetUInt64();
            time_t l_CriteriaDate       = time_t(l_Fields[2].GetUInt32());

            Criteria const* l_Criteria = sCriteriaMgr->GetCriteria(l_CriteriaID);
            if (!l_Criteria)
            {
                /// Removing non-existing criteria data for all characters
                sLog->outError(LOG_FILTER_ACHIEVEMENTSYS, "Non-existing achievement criteria %u data has been removed from the table `character_achievement_progress`.", l_CriteriaID);

                PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA);
                l_Stmt->setUInt32(0, uint16(l_CriteriaID));
                CharacterDatabase.Execute(l_Stmt);

                continue;
            }

            if (l_Criteria->Entry->StartTimer && time_t(l_CriteriaDate + l_Criteria->Entry->StartTimer) < l_NowTimeSec)
                continue;

            CriteriaProgress& l_CriteriaProgress = _criteriaProgress[l_CriteriaID];
            l_CriteriaProgress.Counter  = l_CriteriaCounter;
            l_CriteriaProgress.Date     = l_CriteriaDate;
            l_CriteriaProgress.Changed  = false;
        } while (p_CriteriaResult->NextRow());
    }
}

/// Save achievement to the database
/// @p_AccountTransaction : Transaction for account wide data
/// @p_CharacterTransaction : Transaction for character local data
void PlayerAchievementMgr::SaveToDB(SQLTransaction& p_AccountTransaction, SQLTransaction& p_CharacterTransaction)
{
    PreparedStatement* l_Statement = nullptr;

    if (!_completedAchievements.empty())
    {
        for (auto l_It = _completedAchievements.begin(); l_It != _completedAchievements.end(); ++l_It)
        {
            if (!l_It->second.Changed)
                continue;

            l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_BY_ACHIEVEMENT);
            l_Statement->setUInt32(0, l_It->first);
            l_Statement->setUInt32(1, _owner->GetGUIDLow());
            p_CharacterTransaction->Append(l_Statement);

            l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ACHIEVEMENT);
            l_Statement->setUInt32(0, _owner->GetGUIDLow());
            l_Statement->setUInt32(1, l_It->first);
            l_Statement->setUInt32(2, uint32(l_It->second.Date));
            p_CharacterTransaction->Append(l_Statement);

            l_It->second.Changed = false;
        }
    }

    if (!m_AccountCompletedAchievements.empty() && sWorld->CanBeSaveInLoginDatabase())
    {
        for (auto l_It = m_AccountCompletedAchievements.begin(); l_It != m_AccountCompletedAchievements.end(); ++l_It)
        {
            if (!l_It->second.Changed)
                continue;

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_ACHIEVEMENT_BY_ACHIEVEMENT);
            l_Statement->setUInt32(0, l_It->first);
            l_Statement->setUInt32(1, _owner->GetSession()->GetAccountId());
            p_AccountTransaction->Append(l_Statement);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_ACHIEVEMENT);
            l_Statement->setUInt32(0, _owner->GetSession()->GetAccountId());
            l_Statement->setUInt32(1, l_It->first);
            l_Statement->setUInt32(2, uint32(l_It->second.Date));
            p_AccountTransaction->Append(l_Statement);

            l_It->second.Changed = false;
        }
    }

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_REP_CHAR_ACHIEVEMENT_COUNT);
    l_Statement->setUInt32(0, _owner->GetGUIDLow());
    l_Statement->setUInt32(1, _achievementPoints);
    p_CharacterTransaction->Append(l_Statement);

    if (!_criteriaProgress.empty())
    {
        for (auto l_It = _criteriaProgress.begin(); l_It != _criteriaProgress.end(); ++l_It)
        {
            if (!l_It->second.Changed)
                continue;

            Criteria const* l_Criteria = sCriteriaMgr->GetCriteria(l_It->first);
            if (!l_Criteria)
                continue;

            if (l_Criteria->AccountBind && sWorld->CanBeSaveInLoginDatabase())
            {
                l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_ACHIEVEMENT_PROGRESS_BY_CRITERIA);
                l_Statement->setUInt32(0, _owner->GetSession()->GetAccountId());
                l_Statement->setUInt32(1, l_It->first);
                p_AccountTransaction->Append(l_Statement);

                if (l_It->second.Counter)
                {
                    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_ACHIEVEMENT_PROGRESS);
                    l_Statement->setUInt32(0, _owner->GetSession()->GetAccountId());
                    l_Statement->setUInt32(1, l_It->first);
                    l_Statement->setUInt64(2, l_It->second.Counter);
                    l_Statement->setUInt32(3, uint32(l_It->second.Date));
                    p_AccountTransaction->Append(l_Statement);
                }
            }
            else
            {
                l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_PROGRESS_BY_CRITERIA);
                l_Statement->setUInt32(0, _owner->GetGUIDLow());
                l_Statement->setUInt32(1, l_It->first);
                p_CharacterTransaction->Append(l_Statement);

                if (l_It->second.Counter)
                {
                    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ACHIEVEMENT_PROGRESS);
                    l_Statement->setUInt32(0, _owner->GetGUIDLow());
                    l_Statement->setUInt32(1, l_It->first);
                    l_Statement->setUInt64(2, l_It->second.Counter);
                    l_Statement->setUInt32(3, uint32(l_It->second.Date));
                    p_CharacterTransaction->Append(l_Statement);
                }
            }

            l_It->second.Changed = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void PlayerAchievementMgr::ResetCriteria(CriteriaTypes type, uint64 miscValue1, uint64 miscValue2, bool evenIfCriteriaComplete)
{
    sLog->outDebug(LOG_FILTER_ACHIEVEMENTSYS, "PlayerAchievementMgr::ResetCriteria(%u, " UI64FMTD ", " UI64FMTD ")", type, miscValue1, miscValue2);

    // disable for gamemasters with GM-mode enabled
    if (_owner->isGameMaster())
        return;

    CriteriaList const& achievementCriteriaList = GetCriteriaByType(type);
    for (Criteria const* achievementCriteria : achievementCriteriaList)
    {
        if (achievementCriteria->Entry->FailEvent != miscValue1 || (achievementCriteria->Entry->FailAsset && achievementCriteria->Entry->FailAsset != miscValue2))
            continue;

        std::vector<CriteriaTree const*> const* trees = sCriteriaMgr->GetCriteriaTreesByCriteria(achievementCriteria->ID);
        bool allComplete = true;
        for (CriteriaTree const* tree : *trees)
        {
            // don't update already completed criteria if not forced or achievement already complete
            if (!(IsCompletedCriteriaTree(tree) && !evenIfCriteriaComplete) || !HasAchieved(tree->Achievement->ID))
            {
                allComplete = false;
                break;
            }
        }

        if (allComplete)
            continue;

        RemoveCriteriaProgress(achievementCriteria);
    }
}

void PlayerAchievementMgr::SendAllData(Player const* /*receiver*/) const
{
    VisibleAchievementCheck l_FilterInvisible;

    WorldPackets::Achievement::AllAchievementData l_AchievementData;
    l_AchievementData.Data.Earned.reserve(_completedAchievements.size() + m_AccountCompletedAchievements.size());
    l_AchievementData.Data.Progress.reserve(_criteriaProgress.size());

    WorldPackets::Achievement::AllAccountCriteria l_AllAccountCriteria;
    l_AllAccountCriteria.Progress.reserve(_criteriaProgress.size());

    for (auto l_It = _completedAchievements.begin(); l_It != _completedAchievements.end(); ++l_It)
    {
        AchievementEntry const* l_Achievement = l_FilterInvisible(*l_It);
        if (!l_Achievement || (l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT) != 0)
            continue;

        WorldPackets::Achievement::EarnedAchievement l_EarnedAchievement;
        l_EarnedAchievement.Id     = l_It->first;
        l_EarnedAchievement.Date   = l_It->second.Date;

        if (!(l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT))
        {
            l_EarnedAchievement.Owner                  = _owner->GetGUID();        ///< @TODO
            l_EarnedAchievement.VirtualRealmAddress    = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);
            l_EarnedAchievement.NativeRealmAddress     = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);
        }

        l_AchievementData.Data.Earned.push_back(l_EarnedAchievement);
    }
    for (auto l_It = m_AccountCompletedAchievements.begin(); l_It != m_AccountCompletedAchievements.end(); ++l_It)
    {
        AchievementEntry const* l_Achievement = l_FilterInvisible(*l_It);

        if (!l_Achievement || !(l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT))
            continue;

        bool l_OwnerHaveAchieved = m_CompletedAchievementsCheck.test(l_Achievement->ID);

        WorldPackets::Achievement::EarnedAchievement l_EarnedAchievement;
        l_EarnedAchievement.Id                  = l_It->first;
        l_EarnedAchievement.Date                = l_It->second.Date;
        l_EarnedAchievement.Owner               = l_OwnerHaveAchieved ? _owner->GetGUID() : -1;
        l_EarnedAchievement.VirtualRealmAddress = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);
        l_EarnedAchievement.NativeRealmAddress  = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);

        l_AchievementData.Data.Earned.push_back(l_EarnedAchievement);
    }

    for (auto l_It = _criteriaProgress.begin(); l_It != _criteriaProgress.end(); ++l_It)
    {
        WorldPackets::Achievement::CriteriaProgress l_Progress;
        l_Progress.Id               = l_It->first;
        l_Progress.Quantity         = l_It->second.Counter;
        l_Progress.Player           = l_It->second.PlayerGUID;
        l_Progress.Flags            = 0;
        l_Progress.Date             = l_It->second.Date;
        l_Progress.TimeFromStart    = 0;
        l_Progress.TimeFromCreate   = 0;

        l_AchievementData.Data.Progress.push_back(l_Progress);

        if (Criteria const* l_Criteria = sCriteriaMgr->GetCriteria(l_It->first))
        {
            if (l_Criteria->AccountBind)
                l_AllAccountCriteria.Progress.push_back(l_Progress);
        }
    }

    SendPacket(l_AchievementData.Write());
    SendPacket(l_AllAccountCriteria.Write());
}

void PlayerAchievementMgr::SendAchievementInfo(Player* receiver, uint32 /*achievementId = 0 */) const
{
    VisibleAchievementCheck l_FilterInvisible;
    WorldPackets::Achievement::RespondInspectAchievements l_InspectedAchievements;
    l_InspectedAchievements.Player = _owner->GetGUID();
    l_InspectedAchievements.Data.Earned.reserve(_completedAchievements.size() + m_AccountCompletedAchievements.size());
    l_InspectedAchievements.Data.Progress.reserve(_criteriaProgress.size());

    for (auto l_It = _completedAchievements.begin(); l_It != _completedAchievements.end(); ++l_It)
    {
        AchievementEntry const* l_Achievement = l_FilterInvisible(*l_It);
        if (!l_Achievement || (l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT) != 0)
            continue;

        WorldPackets::Achievement::EarnedAchievement l_EarnedAchievement;
        l_EarnedAchievement.Id      = l_It->first;
        l_EarnedAchievement.Date    = l_It->second.Date;

        l_InspectedAchievements.Data.Earned.push_back(l_EarnedAchievement);
    }
    for (auto l_It = m_AccountCompletedAchievements.begin(); l_It != m_AccountCompletedAchievements.end(); ++l_It)
    {
        AchievementEntry const* l_Achievement = l_FilterInvisible(*l_It);

        if (!l_Achievement || !(l_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT))
            continue;

        bool l_OwnerHaveAchieved = m_CompletedAchievementsCheck.test(l_Achievement->ID);

        WorldPackets::Achievement::EarnedAchievement l_EarnedAchievement;
        l_EarnedAchievement.Id                  = l_It->first;
        l_EarnedAchievement.Date                = l_It->second.Date;
        l_EarnedAchievement.Owner               = l_OwnerHaveAchieved ? _owner->GetGUID() : -1;
        l_EarnedAchievement.VirtualRealmAddress = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);
        l_EarnedAchievement.NativeRealmAddress  = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);

        l_InspectedAchievements.Data.Earned.push_back(l_EarnedAchievement);
    }

    for (auto l_It = _criteriaProgress.begin(); l_It != _criteriaProgress.end(); ++l_It)
    {
        WorldPackets::Achievement::CriteriaProgress l_CriteriaProgress;
        l_CriteriaProgress.Id               = l_It->first;
        l_CriteriaProgress.Quantity         = l_It->second.Counter;
        l_CriteriaProgress.Player           = l_It->second.PlayerGUID;
        l_CriteriaProgress.Flags            = 0;
        l_CriteriaProgress.Date             = l_It->second.Date;
        l_CriteriaProgress.TimeFromStart    = 0;
        l_CriteriaProgress.TimeFromCreate   = 0;

        l_InspectedAchievements.Data.Progress.push_back(l_CriteriaProgress);
    }

    receiver->GetSession()->SendPacket(l_InspectedAchievements.Write());
}

void PlayerAchievementMgr::CompletedAchievement(AchievementEntry const* achievement, Player* referencePlayer)
{
    // disable for gamemasters with GM-mode enabled
    if (_owner->isGameMaster())
        return;

    if ((achievement->Faction == ACHIEVEMENT_FACTION_HORDE    && referencePlayer->GetTeam() != HORDE) ||
        (achievement->Faction == ACHIEVEMENT_FACTION_ALLIANCE && referencePlayer->GetTeam() != ALLIANCE))
        return;

    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER || HasAchieved(achievement->ID))
        return;

    /// @TODO : guild achievement
#ifndef CROSS
    if (achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_NEWS)
        if (Guild* guild = referencePlayer->GetGuild())
            guild->GetNewsLog().AddNewEvent(GUILD_NEWS_PLAYER_ACHIEVEMENT, time(NULL), referencePlayer->GetGUID(), achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_HEADER, achievement->ID);
#endif
    if (!_owner->GetSession()->PlayerLoading())
    {
        SendAchievementEarned(achievement);
        sScriptMgr->OnAchievementComplete(referencePlayer, achievement);
    }

    sLog->outDebug(LOG_FILTER_ACHIEVEMENTSYS, "PlayerAchievementMgr::CompletedAchievement(%u). %s", achievement->ID, GetOwnerInfo().c_str());

    /// Account wide achievement are saved for player & account to avoid loading difficulties
    if (m_AccountCompletedAchievements[achievement->ID].Date == 0)
    {
        CompletedAchievementData& l_CompletedAchievement = m_AccountCompletedAchievements[achievement->ID];
        l_CompletedAchievement.Date     = time(NULL);
        l_CompletedAchievement.Changed  = true;
    }

    CompletedAchievementData& l_CompletedAchievement = _completedAchievements[achievement->ID];
    m_CompletedAchievementsCheck.set(achievement->ID);
    l_CompletedAchievement.Date     = time(NULL);
    l_CompletedAchievement.Changed  = true;

    if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
        sAchievementMgr->SetRealmCompleted(achievement);

    if (!(achievement->Flags & ACHIEVEMENT_FLAG_TRACKING_FLAG))
        _achievementPoints += achievement->Points;

    UpdateCriteria(CRITERIA_TYPE_COMPLETE_ACHIEVEMENT, 0, 0, 0, NULL, referencePlayer);
    UpdateCriteria(CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS, achievement->Points, 0, 0, NULL, referencePlayer);

    // reward items and titles if any
    AchievementReward const* reward = sAchievementMgr->GetAchievementReward(achievement);

    // no rewards
    if (!reward)
        return;

    // titles
    //! Currently there's only one achievement that deals with gender-specific titles.
    //! Since no common attributes were found, (not even in titleRewardFlags field)
    //! we explicitly check by ID. Maybe in the future we could move the achievement_reward
    //! condition fields to the condition system.
    if (uint32 titleId = reward->TitleId[(_owner->GetTeam() == ALLIANCE ? 0 : 1)])
        if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(ObjectMgr::ChangeTitleIdForGender(titleId, _owner->getGender())))
            _owner->SetTitle(titleEntry);

    // mail
    if (reward->SenderCreatureId)
    {
#ifndef CROSS
        MailDraft draft(uint16(reward->MailTemplateId));

        if (!reward->MailTemplateId)
        {
            // subject and text
            std::string subject = reward->Subject;
            std::string text = reward->Body;

            LocaleConstant localeConstant = _owner->GetSession()->GetSessionDbLocaleIndex();
            if (localeConstant >= LOCALE_enUS)
            {
                if (AchievementRewardLocale const* loc = sAchievementMgr->GetAchievementRewardLocale(achievement))
                {
                    ObjectMgr::GetLocaleString(loc->Subject, localeConstant, subject);
                    ObjectMgr::GetLocaleString(loc->Body, localeConstant, text);
                }
            }

            draft = MailDraft(subject, text);
        }

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        Item* item = reward->ItemId ? Item::CreateItem(reward->ItemId, 1, _owner) : NULL;
        if (item)
        {
            // save new item before send
            item->SaveToDB(trans);                               // save for prevent lost at next mail load, if send fail then item will deleted

            // item
            draft.AddItem(item);
        }

        draft.SendMailTo(trans, _owner, MailSender(MAIL_CREATURE, uint64(reward->SenderCreatureId)));
        CharacterDatabase.CommitTransaction(trans);
#else
#ifndef CROSS /// @TODO guild achievement
        if (InterRealmClient* client = GetOwner()->GetSession()->GetInterRealmClient())
            client->SendAchievementReward(GetOwner()->GetRealGUID(), achievement->ID);
#endif
#endif
    }

    // spells
    if (reward->LearnSpell)
        _owner->AddDelayedEvent([player = _owner, rew = reward]() -> void { player->learnSpell(rew->LearnSpell, false); }, 10);

    if (reward->CastSpell)
        _owner->AddDelayedEvent([player = _owner, rew = reward]() -> void { player->CastSpell(player, rew->CastSpell, true); }, 10);
}

bool PlayerAchievementMgr::ModifierTreeSatisfied(uint32 modifierTreeId) const
{
    return AdditionalRequirementsSatisfied(sCriteriaMgr->GetModifierTree(modifierTreeId), 0, 0, nullptr, _owner);
}

void PlayerAchievementMgr::SendCriteriaUpdate(Criteria const* criteria, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const
{
    WorldPackets::Achievement::CriteriaUpdate l_CriteriaUpdate;
    l_CriteriaUpdate.CriteriaID     = criteria->ID;
    l_CriteriaUpdate.Quantity       = progress->Counter;
    l_CriteriaUpdate.PlayerGUID     = _owner->GetGUID();
    l_CriteriaUpdate.Flags          = 0;

    if (criteria->Entry->StartTimer)
        l_CriteriaUpdate.Flags = timedCompleted ? 1 : 0; // 1 is for keeping the counter at 0 in client

    l_CriteriaUpdate.CurrentTime    = progress->Date;
    l_CriteriaUpdate.ElapsedTime    = timeElapsed;
    l_CriteriaUpdate.CreationTime   = 0;

    SendPacket(l_CriteriaUpdate.Write());

    if (criteria->AccountBind)
    {
        WorldPackets::Achievement::AccountCriteriaUpdate l_AccountCriteriaUpdate;
        l_AccountCriteriaUpdate.CriteriaID      = criteria->ID;
        l_AccountCriteriaUpdate.Quantity        = progress->Counter;
        l_AccountCriteriaUpdate.PlayerGUID      = _owner->GetGUID();
        l_AccountCriteriaUpdate.Flags           = 0;

        if (criteria->Entry->StartTimer)
            l_AccountCriteriaUpdate.Flags = timedCompleted ? 1 : 0; // 1 is for keeping the counter at 0 in client

        l_AccountCriteriaUpdate.CurrentTime     = progress->Date;
        l_AccountCriteriaUpdate.ElapsedTime     = timeElapsed;
        l_AccountCriteriaUpdate.CreationTime    = 0;

        SendPacket(l_AccountCriteriaUpdate.Write());
    }
}

void PlayerAchievementMgr::SendCriteriaProgressRemoved(uint32 p_ID, Criteria const* /*p_Criteria = nullptr*/, CriteriaProgress const* /*p_Progress = nullptr*/)
{
    WorldPackets::Achievement::CriteriaDeleted criteriaDeleted;
    criteriaDeleted.CriteriaID = p_ID;
    SendPacket(criteriaDeleted.Write());
}

void PlayerAchievementMgr::SendAchievementEarned(AchievementEntry const* achievement) const
{
    // Don't send for achievements with ACHIEVEMENT_FLAG_HIDDEN
    if (achievement->Flags & ACHIEVEMENT_FLAG_HIDDEN)
        return;

    sLog->outDebug(LOG_FILTER_ACHIEVEMENTSYS, "PlayerAchievementMgr::SendAchievementEarned(%u)", achievement->ID);

    if (!(achievement->Flags & ACHIEVEMENT_FLAG_TRACKING_FLAG))
    {
#ifndef CROSS /// @TODO guild achievement
        if (Guild* guild = sGuildMgr->GetGuildById(_owner->GetGuildId()))
        {
            JadeCore::AchievementChatBuilder say_builder(*_owner, CHAT_MSG_GUILD_ACHIEVEMENT, LANG_ACHIEVEMENT_EARNED, achievement->ID);
            JadeCore::LocalizedPacketDo<JadeCore::AchievementChatBuilder> say_do(say_builder);
            guild->BroadcastWorker(say_do, _owner);
        }
#endif
        if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_KILL | ACHIEVEMENT_FLAG_REALM_FIRST_REACH))
        {
            // broadcast realm first reached
            /*WorldPackets::Achievement::ServerFirstAchievement serverFirstAchievement;
            serverFirstAchievement.Name = _owner->GetName();
            serverFirstAchievement.PlayerGUID = _owner->GetGUID();
            serverFirstAchievement.AchievementID = achievement->ID;
            sWorld->SendGlobalMessage(serverFirstAchievement.Write());*/
        }
        // if player is in world he can tell his friends about new achievement
        else if (_owner->IsInWorld() && !sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        {
            JadeCore::AchievementChatBuilder say_builder(*_owner, CHAT_MSG_ACHIEVEMENT, LANG_ACHIEVEMENT_EARNED, achievement->ID);
            JadeCore::LocalizedPacketDo<JadeCore::AchievementChatBuilder> say_do(say_builder);
            JadeCore::PlayerDistWorker<JadeCore::LocalizedPacketDo<JadeCore::AchievementChatBuilder> > say_worker(_owner, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY), say_do);

            _owner->VisitNearbyWorldObject(sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY), say_worker);
        }
    }

    WorldPackets::Achievement::AchievementEarned achievementEarned;
    achievementEarned.Sender = _owner->GetGUID();
    achievementEarned.Earner = _owner->GetGUID();
    achievementEarned.EarnerNativeRealm = achievementEarned.EarnerVirtualRealm = _owner->GetUInt32Value(PLAYER_FIELD_VIRTUAL_PLAYER_REALM);
    achievementEarned.AchievementID = achievement->ID;
    achievementEarned.Time = time(NULL);
    if (!(achievement->Flags & ACHIEVEMENT_FLAG_TRACKING_FLAG))
        _owner->SendMessageToSet(achievementEarned.Write(), true);
    else
        _owner->SendDirectMessage(achievementEarned.Write());
}

void PlayerAchievementMgr::SendPacket(WorldPacket* data) const
{
    _owner->SendDirectMessage(data);
}

CriteriaList const& PlayerAchievementMgr::GetCriteriaByType(CriteriaTypes type) const
{
    return sCriteriaMgr->GetPlayerCriteriaByType(type);
}

/// GuildAchievementMgr
GuildAchievementMgr::GuildAchievementMgr(Guild* owner) : _owner(owner)
{
}

void GuildAchievementMgr::Reset()
{
#ifndef CROSS /// @TODO guild achievement
    AchievementMgr::Reset();

    uint64 guid = _owner->GetGUID();
    for (auto iter = _completedAchievements.begin(); iter != _completedAchievements.end(); ++iter)
    {
        WorldPackets::Achievement::GuildAchievementDeleted guildAchievementDeleted;
        guildAchievementDeleted.AchievementID = iter->first;
        guildAchievementDeleted.GuildGUID = guid;
        guildAchievementDeleted.TimeDeleted = time(NULL);
        SendPacket(guildAchievementDeleted.Write());
    }

    _achievementPoints = 0;
    _completedAchievements.clear();
    m_CompletedAchievementsCheck.reset();
    DeleteFromDB(guid);
#endif
}

void GuildAchievementMgr::DeleteFromDB(uint64 const& guid)
{
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ALL_GUILD_ACHIEVEMENTS);
    stmt->setUInt32(0, GUID_LOPART(guid));
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ALL_GUILD_ACHIEVEMENT_CRITERIA);
    stmt->setUInt32(0, GUID_LOPART(guid));
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void GuildAchievementMgr::LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult)
{
    if (achievementResult)
    {
        do
        {
            Field* fields = achievementResult->Fetch();
            uint32 achievementid = fields[0].GetUInt32();

            // must not happen: cleanup at server startup in sAchievementMgr->LoadCompletedAchievements()
            AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementid);
            if (!achievement)
                continue;

            CompletedAchievementData& ca = _completedAchievements[achievementid];
            m_CompletedAchievementsCheck.set(achievementid);
            ca.Date = time_t(fields[1].GetUInt32());
            Tokenizer guids(fields[2].GetString(), ' ');
            for (uint32 i = 0; i < guids.size(); ++i)
                ca.CompletingPlayers.insert(MAKE_NEW_GUID(atol(guids[i]), 0, HIGHGUID_PLAYER));

            ca.Changed = false;

            _achievementPoints += achievement->Points;
        } while (achievementResult->NextRow());
    }

    if (criteriaResult)
    {
        time_t now = time(NULL);
        do
        {
            Field* fields = criteriaResult->Fetch();
            uint32 id = fields[0].GetUInt32();
            uint64 counter = fields[1].GetUInt64();
            time_t date = time_t(fields[2].GetUInt32());
            uint32 guid = fields[3].GetUInt32();

            Criteria const* criteria = sCriteriaMgr->GetCriteria(id);
            if (!criteria)
            {
                // we will remove not existed criteria for all guilds
                sLog->outError(LOG_FILTER_ACHIEVEMENTSYS, "Non-existing achievement criteria %u data removed from table `guild_achievement_progress`.", id);

                PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA_GUILD);
                stmt->setUInt32(0, uint16(id));
                CharacterDatabase.Execute(stmt);
                continue;
            }

            if (criteria->Entry->StartTimer && time_t(date + criteria->Entry->StartTimer) < now)
                continue;

            CriteriaProgress& progress = _criteriaProgress[id];
            progress.Counter = counter;
            progress.Date = date;
            progress.PlayerGUID = MAKE_NEW_GUID(guid, 0, HIGHGUID_PLAYER);
            progress.Changed = false;
        } while (criteriaResult->NextRow());
    }
}

void GuildAchievementMgr::SaveToDB(SQLTransaction& trans)
{
#ifndef CROSS /// @TODO guild achievement
    PreparedStatement* stmt;
    std::ostringstream guidstr;
    for (auto itr = _completedAchievements.begin(); itr != _completedAchievements.end(); ++itr)
    {
        if (!itr->second.Changed)
            continue;

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_ACHIEVEMENT);
        stmt->setUInt64(0, _owner->GetId());
        stmt->setUInt32(1, itr->first);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ACHIEVEMENT);
        stmt->setUInt64(0, _owner->GetId());
        stmt->setUInt32(1, itr->first);
        stmt->setUInt32(2, uint32(itr->second.Date));
        for (std::set<uint64>::const_iterator gItr = itr->second.CompletingPlayers.begin(); gItr != itr->second.CompletingPlayers.end(); ++gItr)
            guidstr << GUID_LOPART(*gItr) << ',';

        stmt->setString(3, guidstr.str());
        trans->Append(stmt);

        guidstr.str("");
    }

    for (auto itr = _criteriaProgress.begin(); itr != _criteriaProgress.end(); ++itr)
    {
        if (!itr->second.Changed)
            continue;

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_ACHIEVEMENT_CRITERIA);
        stmt->setUInt32(0, _owner->GetId());
        stmt->setUInt32(1, itr->first);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ACHIEVEMENT_CRITERIA);
        stmt->setUInt32(0, _owner->GetId());
        stmt->setUInt32(1, itr->first);
        stmt->setUInt64(2, itr->second.Counter);
        stmt->setUInt32(3, uint32(itr->second.Date));
        stmt->setUInt32(4, GUID_LOPART(itr->second.PlayerGUID));
        trans->Append(stmt);
    }
#endif
}

void GuildAchievementMgr::SendAllData(Player const* receiver) const
{
    VisibleAchievementCheck filterInvisible;
    WorldPackets::Achievement::AllGuildAchievements allGuildAchievements;
    allGuildAchievements.Earned.reserve(_completedAchievements.size());

    for (auto itr = _completedAchievements.begin(); itr != _completedAchievements.end(); ++itr)
    {
        AchievementEntry const* achievement = filterInvisible(*itr);
        if (!achievement)
            continue;

        WorldPackets::Achievement::EarnedAchievement earned;
        earned.Id = itr->first;
        earned.Date = itr->second.Date;
        allGuildAchievements.Earned.push_back(earned);
    }

    receiver->GetSession()->SendPacket(allGuildAchievements.Write());
}

void GuildAchievementMgr::SendAchievementInfo(Player* receiver, uint32 achievementId /*= 0*/) const
{
    WorldPackets::Achievement::GuildCriteriaUpdate guildCriteriaUpdate;
    if (AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementId))
    {
        if (CriteriaTree const* tree = sCriteriaMgr->GetCriteriaTree(achievement->CriteriaTree))
        {
            CriteriaMgr::WalkCriteriaTree(tree, [this, &guildCriteriaUpdate](CriteriaTree const* node)
            {
                if (node->Criteria)
                {
                    auto progress = this->_criteriaProgress.find(node->Criteria->ID);
                    if (progress != this->_criteriaProgress.end())
                    {
                        WorldPackets::Achievement::GuildCriteriaProgress guildCriteriaProgress;
                        guildCriteriaProgress.CriteriaID = node->Criteria->ID;
                        guildCriteriaProgress.DateCreated = 0;
                        guildCriteriaProgress.DateStarted = 0;
                        guildCriteriaProgress.DateUpdated = progress->second.Date;
                        guildCriteriaProgress.Quantity = progress->second.Counter;
                        guildCriteriaProgress.PlayerGUID = progress->second.PlayerGUID;
                        guildCriteriaProgress.Flags = 0;

                        guildCriteriaUpdate.Progress.push_back(guildCriteriaProgress);
                    }
                }
            });
        }
    }

    receiver->GetSession()->SendPacket(guildCriteriaUpdate.Write());
}

void GuildAchievementMgr::SendAllTrackedCriterias(Player* receiver, std::set<uint32> const& trackedCriterias) const
{
    WorldPackets::Achievement::GuildCriteriaUpdate guildCriteriaUpdate;
    guildCriteriaUpdate.Progress.reserve(trackedCriterias.size());

    for (uint32 criteriaId : trackedCriterias)
    {
        auto progress = _criteriaProgress.find(criteriaId);
        if (progress == _criteriaProgress.end())
            continue;

        WorldPackets::Achievement::GuildCriteriaProgress guildCriteriaProgress;
        guildCriteriaProgress.CriteriaID = criteriaId;
        guildCriteriaProgress.DateCreated = 0;
        guildCriteriaProgress.DateStarted = 0;
        guildCriteriaProgress.DateUpdated = progress->second.Date;
        guildCriteriaProgress.Quantity = progress->second.Counter;
        guildCriteriaProgress.PlayerGUID = progress->second.PlayerGUID;
        guildCriteriaProgress.Flags = 0;

        guildCriteriaUpdate.Progress.push_back(guildCriteriaProgress);
    }

    receiver->GetSession()->SendPacket(guildCriteriaUpdate.Write());
}

void GuildAchievementMgr::CompletedAchievement(AchievementEntry const* achievement, Player* referencePlayer)
{
#ifndef CROSS /// @TODO guild achievement
    sLog->outDebug(LOG_FILTER_ACHIEVEMENTSYS, "GuildAchievementMgr::CompletedAchievement(%u)", achievement->ID);

    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER || HasAchieved(achievement->ID))
        return;

    if (achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_NEWS)
        if (Guild* guild = referencePlayer->GetGuild())
            guild->GetNewsLog().AddNewEvent(GUILD_NEWS_PLAYER_ACHIEVEMENT, time(NULL), referencePlayer->GetGUID(), achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_HEADER, achievement->ID);

    SendAchievementEarned(achievement);
    CompletedAchievementData& ca = _completedAchievements[achievement->ID];
    m_CompletedAchievementsCheck.set(achievement->ID);
    ca.Date = time(NULL);
    ca.Changed = true;

    if (achievement->Flags & ACHIEVEMENT_FLAG_SHOW_GUILD_MEMBERS)
    {
        if (referencePlayer->GetGuildId() == _owner->GetId())
            ca.CompletingPlayers.insert(referencePlayer->GetGUID());

        if (GroupPtr group = referencePlayer->GetGroup())
        {
            group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Member)
            {
                if (Player const* groupMember = l_Member->player)
                {
                    if (groupMember->GetGuildId() == _owner->GetId())
                        ca.CompletingPlayers.insert(groupMember->GetGUID());
                }
            });
        }
    }

    if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
        sAchievementMgr->SetRealmCompleted(achievement);

    if (!(achievement->Flags & ACHIEVEMENT_FLAG_TRACKING_FLAG))
        _achievementPoints += achievement->Points;

    UpdateCriteria(CRITERIA_TYPE_COMPLETE_ACHIEVEMENT, 0, 0, 0, NULL, referencePlayer);
    UpdateCriteria(CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS, achievement->Points, 0, 0, NULL, referencePlayer);
    UpdateCriteria(CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS, achievement->Points, 0, 0, NULL, referencePlayer);
#endif
}

void GuildAchievementMgr::SendCriteriaUpdate(Criteria const* entry, CriteriaProgress const* progress, uint32 /*timeElapsed*/, bool /*timedCompleted*/) const
{
#ifndef CROSS /// @TODO guild achievement
    WorldPackets::Achievement::GuildCriteriaUpdate guildCriteriaUpdate;
    guildCriteriaUpdate.Progress.resize(1);

    WorldPackets::Achievement::GuildCriteriaProgress& guildCriteriaProgress = guildCriteriaUpdate.Progress[0];
    guildCriteriaProgress.CriteriaID = entry->ID;
    guildCriteriaProgress.DateCreated = 0;
    guildCriteriaProgress.DateStarted = 0;
    guildCriteriaProgress.DateUpdated = progress->Date;
    guildCriteriaProgress.Quantity = progress->Counter;
    guildCriteriaProgress.PlayerGUID = progress->PlayerGUID;
    guildCriteriaProgress.Flags = 0;

    _owner->BroadcastPacketIfTrackingAchievement(guildCriteriaUpdate.Write(), entry->ID);
#endif
}

void GuildAchievementMgr::SendCriteriaProgressRemoved(uint32 p_ID, Criteria const* /*p_Criteria = nullptr*/, CriteriaProgress const* /*p_Progress = nullptr*/)
{
#ifndef CROSS /// @TODO guild achievement
    WorldPackets::Achievement::GuildCriteriaDeleted guildCriteriaDeleted;
    guildCriteriaDeleted.GuildGUID = _owner->GetGUID();
    guildCriteriaDeleted.CriteriaID = p_ID;
    SendPacket(guildCriteriaDeleted.Write());
#endif
}

void GuildAchievementMgr::SendAchievementEarned(AchievementEntry const* achievement) const
{
#ifndef CROSS /// @TODO guild achievement
    if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_KILL | ACHIEVEMENT_FLAG_REALM_FIRST_REACH))
    {
        // broadcast realm first reached
       /* WorldPackets::Achievement::ServerFirstAchievement serverFirstAchievement;
        serverFirstAchievement.Name = _owner->GetName();
        serverFirstAchievement.PlayerGUID = _owner->GetGUID();
        serverFirstAchievement.AchievementID = achievement->ID;
        serverFirstAchievement.GuildAchievement = true;
        sWorld->SendGlobalMessage(serverFirstAchievement.Write());*/
    }

    WorldPackets::Achievement::GuildAchievementEarned guildAchievementEarned;
    guildAchievementEarned.AchievementID = achievement->ID;
    guildAchievementEarned.GuildGUID = _owner->GetGUID();
    guildAchievementEarned.TimeEarned = time(NULL);
    SendPacket(guildAchievementEarned.Write());
#endif
}

void GuildAchievementMgr::SendPacket(WorldPacket* data) const
{
#ifndef CROSS /// @TODO guild achievement
    _owner->BroadcastPacket(data);
#endif
}

CriteriaList const& GuildAchievementMgr::GetCriteriaByType(CriteriaTypes type) const
{
    return sCriteriaMgr->GetGuildCriteriaByType(type);
}

/// ScenarioAchievementMgr
ScenarioAchievementMgr::ScenarioAchievementMgr(Scenario* owner) : _owner(owner)
{
}

void ScenarioAchievementMgr::Reset()
{
    AchievementMgr::Reset();
}

void ScenarioAchievementMgr::DeleteFromDB(uint64 const& guid)
{
}

void ScenarioAchievementMgr::LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult)
{
}

void ScenarioAchievementMgr::SaveToDB(SQLTransaction& trans)
{
}

void ScenarioAchievementMgr::SendAllData(Player const* receiver) const
{
}

void ScenarioAchievementMgr::SendAchievementInfo(Player* receiver, uint32 achievementId /*= 0*/) const
{
}

void ScenarioAchievementMgr::SendAllTrackedCriterias(Player* receiver, std::set<uint32> const& trackedCriterias) const
{
}

void ScenarioAchievementMgr::CompletedAchievement(AchievementEntry const* achievement, Player* referencePlayer)
{
}

bool ScenarioAchievementMgr::CanCompleteCriteriaTree(CriteriaTree const* tree)
{
    switch (tree->ID)
    {
        case 56893: ///< End of the Risen Threat - Timed part
        case 56895: ///< End of the Risen Threat - Timed part
            return false;
        default:
            break;
    }

    return true;
}

void ScenarioAchievementMgr::SendCriteriaUpdate(Criteria const* entry, CriteriaProgress const* progress, uint32 /*timeElapsed*/, bool /*timedCompleted*/) const
{
    // FIXME
    _owner->SendCriteriaUpdate(entry, progress);
    // if (timedCompleted)
        _owner->UpdateCurrentStep(false);
}

void ScenarioAchievementMgr::SendCriteriaProgressRemoved(uint32 /*p_ID*/, Criteria const* p_Criteria /*= nullptr*/, CriteriaProgress const* p_Progress /*= nullptr*/)
{
    if (!p_Criteria || !p_Progress)
        return;

    _owner->SendCriteriaUpdate(p_Criteria, p_Progress);
}

void ScenarioAchievementMgr::SendAchievementEarned(AchievementEntry const* achievement) const
{
}

void ScenarioAchievementMgr::SendPacket(WorldPacket* data) const
{
}

CriteriaList const& ScenarioAchievementMgr::GetCriteriaByType(CriteriaTypes type) const
{
    return sCriteriaMgr->GetScenarioCriteriaByType(type);
}

std::string ScenarioAchievementMgr::GetOwnerInfo() const
{
    return "";
}

std::string PlayerAchievementMgr::GetOwnerInfo() const
{
    char l_Buffer[2048];
    sprintf(l_Buffer, "%u %s", _owner->GetGUIDLow(), _owner->GetName());

    return l_Buffer;
}

std::string GuildAchievementMgr::GetOwnerInfo() const
{
#ifndef CROSS /// @TODO guild achievement
    char l_Buffer[2048];
    sprintf(l_Buffer, "Guild %u %s", _owner->GetId(), _owner->GetName().c_str());

    return l_Buffer;
#endif
    return "";
}

AchievementGlobalMgr* AchievementGlobalMgr::Instance()
{
    static AchievementGlobalMgr instance;
    return &instance;
}

std::vector<AchievementEntry const*> const* AchievementGlobalMgr::GetAchievementByReferencedId(uint32 id) const
{
    auto itr = _achievementListByReferencedId.find(id);
    return itr != _achievementListByReferencedId.end() ? &itr->second : NULL;
}

AchievementReward const* AchievementGlobalMgr::GetAchievementReward(AchievementEntry const* achievement) const
{
    auto iter = _achievementRewards.find(achievement->ID);
    return iter != _achievementRewards.end() ? &iter->second : NULL;
}

AchievementRewardLocale const* AchievementGlobalMgr::GetAchievementRewardLocale(AchievementEntry const* achievement) const
{
    auto iter = _achievementRewardLocales.find(achievement->ID);
    return iter != _achievementRewardLocales.end() ? &iter->second : NULL;
}

bool AchievementGlobalMgr::IsRealmCompleted(AchievementEntry const* p_Achievement) const
{
    auto l_Iter = m_AllCompletedAchievements.find(p_Achievement->ID);
    if (l_Iter == m_AllCompletedAchievements.end())
        return false;

    if (l_Iter->second == 0)
        return false;

    /// Allow completing the realm first kill for entire minute after first person did it
    /// it may allow more than one group to achieve it (highly unlikely)
    /// but apparently this is how blizz handles it as well
    if (p_Achievement->Flags & (AchievementFlags::ACHIEVEMENT_FLAG_REALM_FIRST_KILL | AchievementFlags::ACHIEVEMENT_FLAG_REALM_FIRST_REACH))
        return (time(nullptr) - l_Iter->second) > 60;

    return true;
}

void AchievementGlobalMgr::SetRealmCompleted(AchievementEntry const* p_Achievement)
{
    if (IsRealmCompleted(p_Achievement) || m_AllCompletedAchievements[p_Achievement->ID] > 0)
        return;

    m_AllCompletedAchievements[p_Achievement->ID] = time(nullptr);
}

//==========================================================
void AchievementGlobalMgr::LoadAchievementReferenceList()
{
    uint32 oldMSTime = getMSTime();

    if (sAchievementStore.GetNumRows() == 0)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded 0 achievement references.");
        return;
    }

    uint32 count = 0;

    for (uint32 entryId = 0; entryId < sAchievementStore.GetNumRows(); ++entryId)
    {
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(entryId);
        if (!achievement || !achievement->SharesCriteria)
            continue;

        _achievementListByReferencedId[achievement->SharesCriteria].push_back(achievement);
        ++count;
    }

    // Once Bitten, Twice Shy (10 player) - Icecrown Citadel
    if (AchievementEntry const* achievement = sAchievementStore.LookupEntry(4539))
        const_cast<AchievementEntry*>(achievement)->MapID = 631;    // Correct map requirement (currently has Ulduar); 6.0.3 note - it STILL has ulduar requirement

    sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded %u achievement references in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadCompletedAchievements()
{
    uint32 l_OldMSTime = getMSTime();

    /// Populate m_AllCompletedAchievements with all realm first achievement ids to make multithreaded access safer
    /// while it will not prevent races, it will prevent crashes that happen because std::unordered_map key was added
    /// instead the only potential race will happen on value associated with the key
    for (int l_I = 0; l_I < sAchievementStore.GetNumRows(); ++l_I)
    {
        AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(l_I);
        if (!l_Achievement)
            continue;

        if (l_Achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
            m_AllCompletedAchievements[l_Achievement->ID] = 0;
    }

    std::string l_Query = "SELECT achievement, date FROM character_achievement WHERE achievement IN (";
    std::string l_IDs;

    bool l_First = true;
    for (auto const& l_Iter : m_AllCompletedAchievements)
    {
        if (!l_First)
            l_IDs += ", ";

        l_IDs += std::to_string(l_Iter.first);
        l_First = false;
    }

    l_Query += l_IDs;
    l_Query += ") GROUP BY achievement";

    QueryResult l_Result = CharacterDatabase.Query(l_Query.c_str());
    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING,  ">> Loaded 0 realm first completed achievements. DB table `character_achievement` is empty.");
        return;
    }

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_AchievementID = l_Fields[0].GetUInt32();
        uint32 l_Time = l_Fields[1].GetUInt32();
        AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(l_AchievementID);
        if (!l_Achievement)
        {
            /// Remove non-existing achievements from all characters
            sLog->outError(LogFilterType::LOG_FILTER_ACHIEVEMENTSYS, "Non-existing achievement %u data has been removed from the table `character_achievement`.", l_AchievementID);

            PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CharacterDatabaseStatements::CHAR_DEL_INVALID_ACHIEVMENT);
            l_Statement->setUInt32(0, l_AchievementID);
            CharacterDatabase.Execute(l_Statement);

            continue;
        }
        else if (l_Achievement->Flags & (AchievementFlags::ACHIEVEMENT_FLAG_REALM_FIRST_REACH | AchievementFlags::ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
            m_AllCompletedAchievements[l_AchievementID] = l_Time;
    }
    while (l_Result->NextRow());

    l_Query = "SELECT achievement, date FROM guild_achievement WHERE achievement IN (";
    l_Query += l_IDs;
    l_Query += ") GROUP BY achievement";

    l_Result = CharacterDatabase.Query(l_Query.c_str());

    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 realm first completed achievements. DB table `guild_achievement` is empty.");
        return;
    }

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_AchievementID = l_Fields[0].GetUInt32();
        uint32 l_Time = l_Fields[1].GetUInt32();
        AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(l_AchievementID);
        if (!l_Achievement)
        {
            /// Remove non-existing achievements from all characters
            sLog->outError(LogFilterType::LOG_FILTER_ACHIEVEMENTSYS, "Non-existing achievement %u data has been removed from the table `guild_achievement`.", l_AchievementID);

            PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CharacterDatabaseStatements::CHAR_DEL_INVALID_GUILD_ACHIEVMENT);
            l_Statement->setUInt32(0, l_AchievementID);
            CharacterDatabase.Execute(l_Statement);

            continue;
        }
        else if (l_Achievement->Flags & (AchievementFlags::ACHIEVEMENT_FLAG_REALM_FIRST_REACH | AchievementFlags::ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
            m_AllCompletedAchievements[l_AchievementID] = l_Time;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING,  ">> Loaded %lu realm first completed achievements in %u ms.", (unsigned long)m_AllCompletedAchievements.size(), GetMSTimeDiffToNow(l_OldMSTime));
}

void AchievementGlobalMgr::LoadRewards()
{
    uint32 oldMSTime = getMSTime();

    _achievementRewards.clear();                           // need for reload case

    //                                                  0      1        2       3     4       5        6     7              8           9
    QueryResult result = WorldDatabase.Query("SELECT entry, title_A, title_H, item, sender, subject, text, mailTemplate, learnSpell, castSpell FROM achievement_reward");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING,  ">> Loaded 0 achievement rewards. DB table `achievement_reward` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(entry);
        if (!achievement)
        {
            sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` contains a wrong achievement entry (Entry: %u), ignored.", entry);
            continue;
        }

        AchievementReward reward;
        reward.TitleId[0]       = fields[1].GetUInt32();
        reward.TitleId[1]       = fields[2].GetUInt32();
        reward.ItemId           = fields[3].GetUInt32();
        reward.SenderCreatureId = fields[4].GetUInt32();
        reward.Subject          = fields[5].GetString();
        reward.Body             = fields[6].GetString();
        reward.MailTemplateId   = fields[7].GetUInt32();
        reward.LearnSpell       = fields[8].GetUInt32();
        reward.CastSpell        = fields[9].GetUInt32();

        // must be title or mail at least
        if (!reward.TitleId[0] && !reward.TitleId[1] && !reward.SenderCreatureId)
        {
            sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) does not contain title or item reward data. Ignored.", entry);
            continue;
        }

        if (achievement->Faction == ACHIEVEMENT_FACTION_ANY && (!reward.TitleId[0] ^ !reward.TitleId[1]))
            sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) contains the title (A: %u H: %u) for only one team.", entry, reward.TitleId[0], reward.TitleId[1]);

        if (reward.TitleId[0])
        {
            CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(reward.TitleId[0]);
            if (!titleEntry)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) contains an invalid title id (%u) in `title_A`, set to 0", entry, reward.TitleId[0]);
                reward.TitleId[0] = 0;
            }
        }

        if (reward.TitleId[1])
        {
            CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(reward.TitleId[1]);
            if (!titleEntry)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) contains an invalid title id (%u) in `title_H`, set to 0", entry, reward.TitleId[1]);
                reward.TitleId[1] = 0;
            }
        }

        //check mail data before item for report including wrong item case
        if (reward.SenderCreatureId)
        {
            if (!sObjectMgr->GetCreatureTemplate(reward.SenderCreatureId))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) contains an invalid creature entry %u as sender, mail reward skipped.", entry, reward.SenderCreatureId);
                reward.SenderCreatureId = 0;
            }
        }
        else
        {
            if (reward.ItemId)
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) does not have sender data, but contains an item reward. Item will not be rewarded.", entry);

            if (!reward.Subject.empty())
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) does not have sender data, but contains a mail subject.", entry);

            if (!reward.Body.empty())
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) does not have sender data, but contains mail text.", entry);

            if (reward.MailTemplateId)
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) does not have sender data, but has a MailTemplateId.", entry);
        }

        if (reward.MailTemplateId)
        {
            if (!sMailTemplateStore.LookupEntry(reward.MailTemplateId))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) is using an invalid MailTemplateId (%u).", entry, reward.MailTemplateId);
                reward.MailTemplateId = 0;
            }
            else if (!reward.Subject.empty() || !reward.Body.empty())
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) is using MailTemplateId (%u) and mail subject/text.", entry, reward.MailTemplateId);
        }

        if (reward.ItemId)
        {
            if (!sObjectMgr->GetItemTemplate(reward.ItemId))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `achievement_reward` (Entry: %u) contains an invalid item id %u, reward mail will not contain the rewarded item.", entry, reward.ItemId);
                reward.ItemId = 0;
            }
        }

        _achievementRewards[entry] = reward;
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded %u achievement rewards in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadRewardLocales()
{
    uint32 oldMSTime = getMSTime();

    _achievementRewardLocales.clear();                       // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, subject_loc1, text_loc1, subject_loc2, text_loc2, subject_loc3, text_loc3, subject_loc4, text_loc4, "
                                                    "subject_loc5, text_loc5, subject_loc6, text_loc6, subject_loc7, text_loc7, subject_loc8, text_loc8, subject_loc9, text_loc9,"
                                                    "subject_loc10, text_loc10 FROM locales_achievement_reward");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded 0 achievement reward locale strings.  DB table `locales_achievement_reward` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (_achievementRewards.find(entry) == _achievementRewards.end())
        {
            sLog->outError(LOG_FILTER_SQL,  "Table `locales_achievement_reward` (Entry: %u) contains locale strings for a non-existing achievement reward.", entry);
            continue;
        }

        AchievementRewardLocale& data = _achievementRewardLocales[entry];

        for (uint8 i = TOTAL_LOCALES - 1; i > 0; --i)
        {
            LocaleConstant locale = (LocaleConstant) i;
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1)].GetString(), locale, data.Subject);
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1) + 1].GetString(), locale, data.Body);
        }
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded %u achievement reward locale strings in %u ms.", uint32(_achievementRewardLocales.size()), GetMSTimeDiffToNow(oldMSTime));
}
