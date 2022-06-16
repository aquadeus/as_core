////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "GarrisonDraenorBuilding.hpp"
#include "GarrisonDraenorBuildingManager.hpp"
#include "GarrisonDraenorConstants.hpp"
#include "../Interfaces/Interface_GarrisonSite.hpp"
#include "../GarrisonMgrStructures.hpp"
#include "../GarrisonMgrConstants.hpp"
#include "../GarrisonFollower.hpp"
#include "../GarrisonMission.hpp"
#include "../GarrisonWorkOrder.hpp"
#include "../GarrisonShipmentManager.hpp"
#include "../../../scripts/Draenor/Garrison/GarrisonScriptData.hpp"

namespace MS { namespace Garrison {

    class Manager;

namespace GDraenor
{
    /// Garrison manager class
    class GarrisonDraenor
    {
        public:
            /// Constructor
            GarrisonDraenor(Manager* p_Manager, Player* p_Owner);

            /// Create the garrison
            void Create();
            /// Load
            bool Load(PreparedQueryResult p_GarrisonResult, PreparedQueryResult p_BuildingsResult);

            /// Save this garrison to DB
            void Save(SQLTransaction p_Transaction);

            /// Delete garrison
            static void DeleteFromDB(uint64 p_PlayerGUID, SQLTransaction p_Transation);

            /// Update the garrison
            void Update();

            /// Set garrison level
            void SetLevel(uint32 p_Level);

            /// Reward garrison cache content
            void RewardGarrisonCache();
            /// Get garrison cache token count
            uint32 GetGarrisonCacheTokenCount() const;

            /// Get terrain swaps
            void GetUIWorldMapAreaSwaps(std::set<uint32>& p_UIWorldMapAreaSwaps) const;

            /// Get garrison script
            Interfaces::GarrisonSite* GetGarrisonScript() const;

            /// Can upgrade the garrison
            bool CanUpgrade(uint32 & p_Reason) const;
            /// Upgrade the garrison
            void Upgrade();

            /// When the garrison owner enter in the garrisson (@See Player::UpdateArea)
            void OnPlayerEnter();
            /// When the garrison owner leave the garrisson (@See Player::UpdateArea)
            void OnPlayerLeave();

            /// set last used activation game object
            void SetLastUsedActivationGameObject(uint64 p_Guid);

            /// Get GarrSiteLevelEntry for current garrison
            const GarrSiteLevelEntry* GetGarrisonSiteLevelEntry() const;

        /// Plot section
        public:
            /// Get plots for level
            std::vector<GarrisonPlotInstanceInfoLocation> GetPlots();
            /// Get plot by position
            GarrisonPlotInstanceInfoLocation GetPlot(float p_X, float p_Y, float p_Z);
            /// Get plot instance plot type
            uint32 GetPlotType(uint32 p_PlotInstanceID) const;
            /// Plot is free ?
            bool PlotIsFree(uint32 p_PlotInstanceID) const;
            /// Has plot instance
            bool HasPlotInstance(uint32 p_PlotInstanceID) const;
            /// Get plot location
            GarrisonPlotInstanceInfoLocation GetPlot(uint32 p_PlotInstanceID) const;
            /// Get plot instance ID by activation game object
            uint32 GetPlotInstanceIDByActivationGameObject(uint64 p_Guid) const;
            /// Return Daily Tavern Datas
            std::vector<uint32>& GetGarrisonDailyTavernDatas() { return m_GarrisonDailyTavernData; };
            /// Return Weekly Tavern Datas
            std::vector<WeeklyTavernData>& GetGarrisonWeeklyTavernDatas() { return m_GarrisonWeeklyTavernData; };

        /// Blueprint section
        public:
            /// Get known blueprints
            std::vector<int32> GetKnownBlueprints() const;
            /// Learn blue print
            bool LearnBlueprint(uint32 p_BuildingRecID);
            /// Known blue print
            bool KnownBlueprint(uint32 p_BuildingRecID) const;

        /// Follower section
        public:
            /// Set Follower Recruit
            bool CanRecruitFollower() const;
            /// Set Follower Recruit
            void SetCanRecruitFollower(bool p_Apply);

            /// Assign a follower to a building
            void AssignFollowerToBuilding(uint64 p_FollowerDBID, uint32 p_PlotInstanceID);

            /// Returns if the specific follower type can be renamed
            bool CanRenameFollowerType(uint32 p_FollowerType) const;
            /// Renames the specified follower
            bool RenameFollower(uint32 p_DatabaseID, std::string p_FollowerName);

        /// Building section
        public:
            /// Can build building X at slot instance Y
            bool IsBuildingPlotInstanceValid(uint32 p_BuildingRecID, uint32 p_PlotInstanceID) const;
            /// Player fill all condition
            PurchaseBuildingResults::Type CanPurchaseBuilding(uint32 p_BuildingRecID) const;
            /// PurchaseBuilding
            GarrisonBuilding PurchaseBuilding(uint32 p_BuildingRecID, uint32 p_PlotInstanceID, bool p_Triggered = false);
            /// Get building
            GarrisonBuilding GetBuilding(uint32 p_PlotInstanceID) const;
            /// Get Building with ID
            GarrisonBuilding GetBuildingWithBuildingID(uint32 p_BuildingID) const;
            /// Get building object
            GarrisonBuilding* GetBuildingObject(uint32 p_PlotInstanceID);
            /// Get buildings
            std::vector<GarrisonBuilding> GetBuildings() const;
            /// Get building passive ability effects
            std::vector<uint32> GetBuildingsPassiveAbilityEffects() const;
            /// Activate building
            void ActivateBuilding(uint32 p_PlotInstanceID);
            /// Activate building
            void ActivateBuilding();
            /// Cancel construction
            void CancelConstruction(uint32 p_PlotInstanceID);
            /// Delete building
            void DeleteBuilding(uint32 p_PlotInstanceID, bool p_Canceled, bool p_RemoveForUpgrade);
            /// Has active building
            bool HasActiveBuilding(uint32 p_BuildingID) const;
            /// Has building type
            bool HasBuildingType(Building::Type p_BuildingType, bool p_DontNeedActive = false) const;
            /// Get building with type
            GarrisonBuilding GetBuildingWithType(Building::Type p_BuildingType, bool p_DontNeedActive = false) const;
            /// Get Level of the building
            uint32 GetBuildingLevel(GarrisonBuilding p_Building) const;
            /// Get building max work order
            uint32 GetBuildingMaxWorkOrder(uint32 p_PlotInstanceID) const;
            /// Calculate the chance to double the work order of the building, depending of the follower
            uint8 CalculateAssignedFollowerShipmentBonus(uint32 p_PlotInstanceID);
            /// Generates random reward for Armory work order
            uint32 CalculateArmoryWorkOrder() const;
            /// Fills reward maps in work orders algorithm for Gladiator's Sanctum
            bool FillSanctumWorkOrderRewards(std::map<uint32, uint32>& l_RewardItems, std::map<CurrencyTypes, uint32>& l_RewardedCurrencies);
            /// Get follower assigned to building from plot instance ID
            GarrisonFollower* GetAssignedFollower(uint32 p_PlotInstanceID);
            /// Checks if the building has the required follower assigned to apply bonus
            bool HasRequiredFollowerAssignedAbility(uint32 p_PlotInstanceID);
            /// Add new creature in plot datas, that way any summoned creature can be detected as part of the building
            void InsertNewCreatureInPlotDatas(uint32 p_PlotInstanceID, uint64 p_Guid);
            /// Get creature plot instance ID
            uint32 GetCreaturePlotInstanceID(uint64 p_GUID) const;
            /// Get gameobject plot instance ID
            uint32 GetGameObjectPlotInstanceID(uint64 p_GUID) const;
            /// Get building gathering data
            /// @p_PlotInstanceID : Plot building location
            std::string GetBuildingGatheringData(uint32 p_PlotInstanceID);
            /// Set building gathering data
            /// @p_PlotInstanceID   : Plot building location
            /// @p_Data             : Gathering data
            void SetBuildingGatheringData(uint32 p_PlotInstanceID, std::string p_Data);
            /// Get list of creature in a specific building type
            /// @p_Type : Building type
            std::vector<uint64> GetBuildingCreaturesByBuildingType(Building::Type p_Type);
            /// Get Garrison Level
            uint32 GetGarrisonLevel() { return m_GarrisonLevel; };
            /// Check if the players has the right mount
            bool CheckGarrisonStablesQuestsConditions(uint32 p_QuestID, Player* p_Player);
            /// Checks training mounts auras
            bool IsTrainingMount();
            /// Add auras/flags depending on the building level
            void HandleStablesAuraBonuses(bool p_Remove = false);

        public:
            /// Garrison Reset Systems
            void AddGarrisonDailyTavernData(uint32 p_Data);
            void SetGarrisonDailyTavernData(uint32 p_Data);
            void CleanGarrisonDailyTavernData();
            void ResetGarrisonDailyTavernData();
            void AddGarrisonWeeklyTavernData(WeeklyTavernData p_Data);
            void SetGarrisonWeeklyTavernData(WeeklyTavernData p_Data);
            void CleanGarrisonWeeklyTavernData();
            void ResetGarrisonWeeklyTavernData();
            void ResetGarrisonWorkshopData(Player* p_Player);
            void ResetGarrisonTradingPostData(Player* p_Player);
            std::vector<GarrisonFollower> GetWeeklyFollowerRecruits(Player* p_Player);

            /// Get known specializations
            std::vector<int32> GetKnownSpecializations() const;

            uint32 GetFollowersCountBarracksBonus();

            /// Sends a packet to owner
            void SendPacketToOwner(WorldPacket* p_Data);

            /// Checks if ship has crew
            bool HasCrewAbility(GarrisonFollower const& p_Follower) const;

            /// Generates follower ability (crew) for the specified shit
            uint32 GenerateCrewAbilityIdForShip(GarrisonFollower const& p_Follower);

            /// Returns if player has shipyard or not
            bool HasShipyard() const;

            /// Returns the shipyard level
            uint32 GetShipyardLevel() const;

            /// Returns mapId of shipyard
            uint32 GetShipyardMapId() const;

            /// Get shipyard terain swap
            void GetShipyardUIWorldMapAreaIDSwaps(std::set<uint32>& p_TerrainSwaps) const;

            /// Create shipyard by spell
            bool CreateShipyardBySpell();

            /// Update plot gameobject
            void UpdatePlot(uint32 p_PlotInstanceID);

        public:
            /// Replace garrison script
            void _SetGarrisonScriptID(uint32 p_ID)
            {
                m_GarrisonScriptID = p_ID;
            }

        private:
            /// Init
            void Init();
            /// Init data for level
            void InitDataForLevel();

            /// Init Game objects
            void InitPlots();
            /// Uninit plots
            void UninitPlots();

            /// Update garrison stats
            void UpdateStats();

            /// Update buildings
            void UpdateBuildings();
            /// Update cache
            void UpdateCache();
            /// Update garrison ability
            void UpdateGarrisonAbility();

        private:
            Manager*    m_Manager;          ///< Garrison manager
            Player*     m_Owner;            ///< Garrison owner
            uint32      m_GarrisonLevel;    ///< Garrison level
            uint32      m_GarrisonLevelID;  ///< Garrison level ID
            uint32      m_GarrisonSiteID;   ///< Garrison site ID
            uint32      m_CacheLastUsage;
            uint64      m_LastUsedActivationGameObject;
            uint64      m_CacheGameObjectGUID;
            uint32      m_CacheLastTokenAmount;
            bool        m_CanRecruitFollower;

            std::vector<GarrisonPlotInstanceInfoLocation>   m_Plots;
            std::vector<GarrisonBuilding>                   m_Buildings;
            std::vector<int32>                              m_KnownBlueprints;
            std::vector<int32>                              m_KnownSpecializations;
            std::vector<uint32>                             m_GarrisonDailyTavernData;
            std::vector<WeeklyTavernData>                   m_GarrisonWeeklyTavernData;

            std::map<uint32, uint64>                m_PlotsGob;
            std::map<uint32, uint64>                m_PlotsActivateGob;
            std::map<uint32, uint64>                m_PlotsWorkOrderGob;
            std::map<uint32, std::vector<uint64>>   m_PlotsGameObjects;
            std::map<uint32, std::vector<uint64>>   m_PlotsCreatures;
            std::map<uint32, Building::Type>        m_LastPlotBuildingType; ///< <PlotID, BuildingType>

            uint32 m_GarrisonScriptID;

    };

}   ///< namespace GDraenor
}   ///< namespace Garrison
}   ///< namespace MS

