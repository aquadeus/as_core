////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Interfaces/Interface_GarrisonSite.hpp"
#include "GarrisonMgrStructures.hpp"
#include "GarrisonMgrConstants.hpp"
#include "GarrisonFollower.hpp"
#include "GarrisonMission.hpp"
#include "GarrisonWorkOrder.hpp"
#include "GarrisonTalent.hpp"
#include "GarrisonShipmentManager.hpp"
#include "Draenor/GarrisonDraenorMgr.hpp"
#include "BrokenIsles/GarrisonBrokenIslesMgr.hpp"

/// This macro enabled compilation of debug code in Dev env
#ifdef _MSC_VER
    #define GARRISON_CHEST_FORMULA_DEBUG 1
#endif

namespace MS { namespace Garrison
{
    /// Garrison manager class
    class Manager
    {
        public:
            /// Constructor
            Manager(Player* p_Owner);
            /// Destructor
            ~Manager();

            /// Create the garrison
            void Create(MS::Garrison::GarrisonType::Type p_Type);
            /// Load
            bool Load(  PreparedQueryResult p_GarrisonResult,           PreparedQueryResult p_FollowersResult,          PreparedQueryResult p_MissionsResult, PreparedQueryResult p_WorkOrderResult, PreparedQueryResult p_TalentResult,
                        PreparedQueryResult p_DraenorGarrisonResult,    PreparedQueryResult p_DraenorBuildingsResult,
                        PreparedQueryResult p_BrokenIslesGarrisonResult);
            /// Save this garrison to DB
            void Save();

            /// Delete garrison
            static void DeleteFromDB(uint64 p_PlayerGUID, SQLTransaction p_Transation);

            /// Update the garrison
            void Update();

            /// Get Garrison Faction Index
            FactionIndex::Type GetGarrisonFactionIndex() const;

            /// Get dreanor garrison
            GDraenor::GarrisonDraenor * GetDraenorGarrison() const;
            /// Get dreanor garrison
            GBrokenIsles::GarrisonBrokenIsles * GetBrokenIslesGarrison() const;

            /// When the garrison owner enter in the garrisson (@See Player::UpdateArea)
            void OnPlayerEnter();
            /// When the garrison owner leave the garrisson (@See Player::UpdateArea)
            void OnPlayerLeave();
            /// When the garrison owner started a quest
            void OnQuestStarted(const Quest* p_Quest);
            /// When the garrison owner reward a quest
            void OnQuestReward(const Quest* p_Quest);
            /// When the garrison owner abandon a quest
            void OnQuestAbandon(const Quest* p_Quest);

            /// When the owner player change level
            /// @p_Level : New owner level
            void OnOwnerLevelChange(uint32 p_Level);

            /// Send remote informations
            void SendRemoteInformations();

            /// Get garrison ID
            uint32 GetGarrisonID();

            std::vector<GarrAbilityEffectEntry const*> GetApplicableGarrAbilitiesEffectsByType(MS::Garrison::AbilityEffectTypes::Type p_Type, uint32 p_MissionRecID);

            void RemovePlayerGarrAbility(uint32 p_GarrAbilityID)
            {
                m_PlayerGarrAbilities.erase(p_GarrAbilityID);
            }

            void AddPlayerGarrAbility(uint32 p_GarrAbilityID)
            {
                m_PlayerGarrAbilities.insert(p_GarrAbilityID);
            }

            bool IsLoaded() const
            {
                return m_Loaded;
            }

            bool IsRequested() const
            {
                return m_Requested;
            }

            void SetRequested(bool p_Requested)
            {
                m_Requested = p_Requested;
            }

        /// Script
        public:
            /// Get garrison script
            Interfaces::GarrisonSite* GetGarrisonScript() const;
            /// Replace garrison script
            void _SetGarrisonScript(uint32 p_ScriptID)
            {
                m_GarrisonScriptID = p_ScriptID;

                if (m_GarrisonDraenor)
                    m_GarrisonDraenor->_SetGarrisonScriptID(p_ScriptID);

                if (m_GarrisonBrokenIsles)
                    m_GarrisonBrokenIsles->_SetGarrisonScriptID(p_ScriptID);
            }

            bool HasGarrisonBrokenIsles() const { return m_GarrisonBrokenIsles != nullptr; }

        #pragma region Followers
        /// Followers
        public:
            /// Add follower
            bool AddFollower(uint32 p_FollowerID);
            /// Add follower with initialized entity
            bool AddFollower(GarrisonFollower p_Follower);
            /// Removes the follower
            bool RemoveFollower(uint32 p_DatabaseID, bool p_Force = false);

            void DespawnBodyguardIfNeeded();
            /// Get Current Combat Ally Follower if any
            GarrisonFollower* GetCombatAllyFollower();
            /// Get followers
            std::vector<GarrisonFollower> GetFollowers(GarrisonType::Type p_Type) const;
            /// Get followers with specific ability
            std::list<GarrisonFollower> GetFollowersWithAbility(uint32 p_AbilityID, bool p_IsTrait);
            /// Get follower
            GarrisonFollower* GetFollower(uint32 p_FollowerID);
            /// Get follower with Database ID
            GarrisonFollower* GetFollowerWithDatabaseID(uint32 p_FollowerDatabaseID);
            /// Gets the follower count of specified type
            uint32 GetTotalFollowerCount(eGarrisonFollowerTypeID p_Type);

            uint32 GetFollowerCount(uint32 p_FollowerID) const;
            uint32 GetMaxCount(uint32 p_FollowerID) const;

            uint32 GetFollowerIDForShipmentID(uint32 p_ShipmentID) const;

            /// Get follower by ID or template
            GarrisonFollower GenerateNewFollowerEntity(uint32 p_FollowerID);

            /// Get activated followers count
            uint32 GetActiveFollowerCount(eGarrisonFollowerTypeID p_FollowerType) const;
            /// Change follower activation state
            void ChangeFollowerActivationState(uint64 p_FollowerDBID, bool p_Active);

            /// Returns error message of upgrade attempt
            SpellCastResult CanUpgradeFollowerItemLevelWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo, SpellEffIndex p_EffIndex) const;
            /// Upgrades follower with spell
            void UpgradeFollowerItemLevelWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo, SpellEffIndex p_EffIndex);

            SpellCastResult CanIncreaseFollowerExperienceWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo) const;
            /// Increase follower experience with spell
            void IncreaseFollowerExperienceWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo, SpellEffIndex p_EffIndex);

            /// Check if any followers has ability in parameter
            bool HasFollowerAbility(uint32 p_AbilityID) const;
            /// Reroll Follower Abilities
            void GenerateFollowerAbilities(uint32 p_FollowerID, bool p_Reset = true, bool p_Abilities = true, bool p_Traits = true, bool p_Update = false);

            /// Returns error message of the attempt
            SpellCastResult CanLearnTrait(uint32 p_FollowerID, uint32 p_Slot, SpellInfo const* p_SpellInfo, uint32 p_EffIndex) const;
            /// Learns the specific trait
            void LearnFollowerTrait(uint32 p_FollowerID, uint32 p_Slot, SpellInfo const* p_SpellInfo, uint32 p_EffIndex);

            /// Get num follower activation remaining
            uint32 GetNumFollowerActivationsRemaining(GarrisonType::Type p_Type) const;

        /// Followers
        private:
            /// Returns the cap of the specified follower type
            uint32 GetFollowerSoftCap(eGarrisonFollowerTypeID p_FollowerType) const;

            /// Returns if the mission with specified follower type can be started after soft cap
            bool CanMissionBeStartedAfterSoftCap(uint32 p_FollowerType) const;

            /// Generate random NPC Ability
            uint32 GenerateRandomAbility(uint32 p_FollowerID);
            /// Generate random trait
            uint32 GenerateRandomTrait(uint32 p_Type, std::vector<uint32> const& p_KnownAbilities);
            /// Reroll Follower Abilities
            void GenerateFollowerAbilities(GarrisonFollower& p_Follower, bool p_Reset = true, bool p_Abilities = true, bool p_Traits = true, bool p_Update = false);

            /// Reward missing followers from quests
            void RewardMissingFollowers();
            /// Unstuck followers in a non existent mission
            void UnstuckFollowers();

            /// Update followers
            void UpdateFollowers();
        #pragma endregion

        #pragma region Missions
        /// Missions
        public:
            /// Update mission distribution
            void UpdateMissionDistribution(bool p_Force = false);

            /// Add mission
            bool AddMission(uint32 p_MissionRecID, uint32 p_OfferDuration = 0);
            /// Player have mission
            bool HasMission(uint32 p_MissionRecID) const;

            /// Start mission
            void StartMission(uint32 p_MissionRecID, std::vector<uint64> p_Followers);
            /// Send mission start failed packet
            void StartMissionFailed(uint32 p_MissionRecID, std::vector<uint64> p_Followers);
            /// Remove mission
            void RemoveMission(uint32 p_MissionRecID);

            /// Complete a mission
            void CompleteMission(uint32 p_MissionRecID, bool p_IsResponse);
            /// Do mission bonus roll
            void DoMissionBonusRoll(uint64 p_MissionTableGUID, uint32 p_MissionRecID);
            /// Set mission has complete
            void SetAllInProgressMissionAsComplete();

            /// Get the mission travel time
            uint32 GetMissionTravelDuration(uint32 p_MissionRecID);
            /// Get the mission duration
            uint32 GetMissionDuration(uint32 p_MissionRecID);

            /// Get missions
            std::vector<GarrisonMission> GetMissions(GarrisonType::Type p_Type) const;
            /// Get all completed missions
            std::vector<GarrisonMission> GetCompletedMissions(GarrisonType::Type p_Type) const;
            /// Get all missions in progress
            std::vector<GarrisonMission> GetPendingMissions(GarrisonType::Type p_Type) const;
            /// Check if player has pending mission
            bool HasPendingMission(uint32 p_MissionID);

            /// Get mission with mission ID
            GarrisonMission* GetMissionWithID(uint32 p_MissionID);

            /// Gets the amount of missions started today by garrison type
            uint32 GetMissionStartedTodayCount(GarrisonType::Type p_Type) const;
            /// Reset today mission count
            void ResetMissionStartedTodayCount();

        /// Missions
        private:
            /// Get mission chest chance
            void InitMission(GarrisonMission* p_Mission, std::vector<uint64> const& p_Followers);

            /// Get followers on a mission
            std::vector<GarrisonFollower*> GetMissionFollowers(uint32 p_MissionRecID);
            /// Get mission followers abilities effect
            std::vector<uint32> GetMissionFollowersAbilitiesEffects(uint32 p_MissionRecID);
            /// Get mission followers abilities effect
            std::vector<uint32> GetMissionFollowersAbilitiesEffects(uint32 p_MissionRecID, AbilityEffectTypes::Type p_Type, uint32 p_TargetMask);

            /// Cleanup missions
            void CleanupMissions();
            /// Cleanup missions
            void CleanupOfferMissions();

            /// Evaluate mission conditions
            bool EvaluateMissionConditions(GarrMissionEntry const* p_Entry);

            /// Increase the amount of garrison missions started today by 1
            uint32 IncreaseMissionsStartedTodayCount(GarrisonType::Type p_Type);
        #pragma endregion

        #pragma region WorkOrders
        /// Work orders
        public:
            /// Start new work order
            uint64 StartWorkOrder(uint32 p_PlotInstanceID, uint32 p_ShipmentID);
            /// Delete work order
            void DeleteWorkOrder(uint64 p_DBID);

            /// Get work orders
            std::vector<GarrisonWorkOrder>& GetWorkOrders();
            /// Get in progress work order count
            uint32 GetWorkOrderCount(uint32 p_PlotInstanceID) const;

        /// Work orders
        private:
            /// Update work order
            void UpdateWorkOrders();
        #pragma endregion

        #pragma region Talents
        /// Talents
        public:
            bool CanAddTalent(GarrTalentEntry const* p_Entry, bool p_Replace = false) const;
            bool HasATalentInRow(GarrTalentEntry const* p_Entry) const;
            bool AddTalent(GarrTalentEntry const* p_Entry, bool p_TakeRequirements = true, bool p_Replace = false);
            bool RemoveTalent(GarrTalentEntry const* p_Entry);
            bool HasTalent(uint32 p_ID) const;
            bool IsResearchingTalent() const;
            int8 GetLastResearchedRankOfTalentTree(uint32 p_TalentTreeID) const;
            std::vector<GarrisonTalent const*> GetGarrisonTalents(GarrisonType::Type p_Type) const;
            void SendTalentUpdate(GarrisonTalent const& l_Talent, bool p_Remove = false);
            void ApplyTalentEffect(GarrTalentEntry const* p_Entry, bool p_Apply, bool p_SendVisibilityUpdate);
            bool HasResearchedTalent(uint32 p_ID) const;
        /// Talents
        private:
            /// Update talents
            void UpdateTalents();
        #pragma endregion

        private:
            Player*                             m_Owner;            ///< Garrison owner
            uint32                              m_ID;               ///< Garrison DB ID
            std::vector<GarrisonMission>        m_Missions;
            std::vector<GarrisonFollower>       m_Followers;
            std::vector<GarrisonWorkOrder>      m_WorkOrders;
            std::vector<GarrisonTalent>         m_Talents;
            std::map<uint32, uint32>            m_MissionsStartedToday;

            uint32      m_NumFollowerActivation[GarrisonType::Max];
            uint32      m_NumFollowerActivationRegenTimestamp[GarrisonType::Max];
            uint32      m_MissionDistributionLastUpdate;

            GarrisonMissionReward m_PendingMissionReward;

            uint32 m_GarrisonScriptID;

            GDraenor::GarrisonDraenor           * m_GarrisonDraenor;
            GBrokenIsles::GarrisonBrokenIsles   * m_GarrisonBrokenIsles;

            std::set<uint32> m_PlayerGarrAbilities;

            bool m_Loaded;
            bool m_Requested;
    };

}   ///< namespace Garrison
}   ///< namespace MS


