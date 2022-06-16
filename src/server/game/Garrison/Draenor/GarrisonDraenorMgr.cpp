////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonMgr.hpp"
#include "../../../scripts/Draenor/Garrison/GarrisonScriptData.hpp"
#include "Garrison/GarrisonMgr.hpp"
#include "Player.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "DisableMgr.h"

namespace MS { namespace Garrison { namespace GDraenor
{
    /// Constructor
    GarrisonDraenor::GarrisonDraenor(Manager* p_Manager, Player* p_Owner)
        : m_Manager(p_Manager), m_Owner(p_Owner)
    {
        m_GarrisonLevel     = 1;
        m_GarrisonLevelID   = 0;
        m_GarrisonSiteID    = 0;

        m_CacheLastUsage    = time(0);

        m_CacheGameObjectGUID = 0;

        m_CacheLastTokenAmount = 0;

        m_GarrisonScriptID = 0;
        m_CanRecruitFollower = p_Owner->HasCharacterWorldState(CharacterWorldStates::GarrisonTavernBoolCanRecruitFollower) ? p_Owner->GetCharacterWorldStateValue(CharacterWorldStates::GarrisonTavernBoolCanRecruitFollower) : 1;

        /// Select Garrison site ID
        switch (m_Manager->GetGarrisonFactionIndex())
        {
            case MS::Garrison::FactionIndex::Alliance:
                m_GarrisonSiteID = 2;
                break;

            case MS::Garrison::FactionIndex::Horde:
                m_GarrisonSiteID = 71;
                break;

            case MS::Garrison::FactionIndex::Max:
            default:
                ASSERT(false);
                break;

        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Create the garrison
    void GarrisonDraenor::Create()
    {
        /// At creation, the garrison cache contains 50 token
        m_CacheLastUsage = time(0) - (200 * Globals::CacheTokenGenerateTime);

        std::ostringstream l_KnownBluePrintsStr;

        for (uint32 l_I = 0; l_I < m_KnownBlueprints.size(); ++l_I)
            l_KnownBluePrintsStr << m_KnownBlueprints[l_I] << ' ';

        std::ostringstream l_KnownSpecializationsStr;

        for (uint32 l_I = 0; l_I < m_KnownSpecializations.size(); ++l_I)
            l_KnownSpecializationsStr << m_KnownSpecializations[l_I] << ' ';

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_DRAENOR);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, m_Manager->GetGarrisonID());
        l_Stmt->setUInt32(l_Index++, m_GarrisonLevel);
        l_Stmt->setString(l_Index++, l_KnownBluePrintsStr.str());
        l_Stmt->setString(l_Index++, l_KnownSpecializationsStr.str());
        l_Stmt->setUInt32(l_Index++, m_CacheLastUsage);

        CharacterDatabase.AsyncQuery(l_Stmt);

        Init();

        /// Garrison ability
        if (!m_Owner->HasSpell(GARRISON_SPELL_GARR_ABILITY))
            m_Owner->learnSpell(GARRISON_SPELL_GARR_ABILITY, false);

        uint32 l_QuestEntry = 0;

        /// Select Garrison site ID
        switch (m_Manager->GetGarrisonFactionIndex())
        {
            case MS::Garrison::FactionIndex::Alliance:
                l_QuestEntry = BaseQuests::FoundedGarrisonA;
                break;

            case MS::Garrison::FactionIndex::Horde:
                l_QuestEntry = BaseQuests::FoundedGarrisonH;
                break;

            case MS::Garrison::FactionIndex::Max:
            default:
                ASSERT(false);
                break;

        }

        /// Minimap icons are completed quest, reason => BLIZZARD !!!!!!!!!!!!!!
        if (m_Owner->GetQuestStatus(l_QuestEntry) != QUEST_STATUS_REWARDED)
        {
            if (const Quest* l_Quest = sObjectMgr->GetQuestTemplate(l_QuestEntry))
            {
                m_Owner->AddQuest(l_Quest, m_Owner);
                m_Owner->CompleteQuest(l_QuestEntry);
            }
        }

        sGarrisonDraenorBuildingManager->LearnAllowedBuildings(m_Owner, this);

        /// Learn garrison taxinodes
        if (m_Owner->getFaction() == HORDE)
            m_Owner->m_taxi.SetTaximaskNode(TaxiNodes::Horde);
        else
            m_Owner->m_taxi.SetTaximaskNode(TaxiNodes::Alliance);
    }

    /// Load
    bool GarrisonDraenor::Load(PreparedQueryResult p_GarrisonResult, PreparedQueryResult p_BuildingsResult)
    {
        if (p_GarrisonResult)
        {
            Field* l_Fields = p_GarrisonResult->Fetch();

            m_GarrisonLevel = l_Fields[0].GetUInt32();

            if (!l_Fields[1].GetString().empty())
            {
                Tokenizer l_BluePrints(l_Fields[1].GetString(), ' ');

                for (Tokenizer::const_iterator l_It = l_BluePrints.begin(); l_It != l_BluePrints.end(); ++l_It)
                    m_KnownBlueprints.push_back(atol(*l_It));
            }

            /// Update criterias.
            /// Use delayed event because criterias cannot be sent while the player is loading.
            Player* l_Player = m_Owner;
            l_Player->AddDelayedEvent([=]() -> void
            {
                l_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT);
            }, 1);

            if (!l_Fields[2].GetString().empty())
            {
                Tokenizer l_Specializations(l_Fields[2].GetString(), ' ');

                for (Tokenizer::const_iterator l_It = l_Specializations.begin(); l_It != l_Specializations.end(); ++l_It)
                    m_KnownSpecializations.push_back(atol(*l_It));
            }

            m_CacheLastUsage = l_Fields[3].GetUInt32();

            if (p_BuildingsResult)
            {
                do
                {
                    l_Fields = p_BuildingsResult->Fetch();

                    GarrisonBuilding l_Building;
                    l_Building.DatabaseID       = l_Fields[0].GetUInt32();
                    l_Building.PlotInstanceID   = l_Fields[1].GetUInt32();
                    l_Building.BuildingID       = l_Fields[2].GetUInt32();
                    l_Building.SpecID           = l_Fields[3].GetUInt32();
                    l_Building.TimeBuiltStart   = l_Fields[4].GetUInt32();
                    l_Building.TimeBuiltEnd     = l_Fields[5].GetUInt32();
                    l_Building.FollowerAssigned = l_Fields[6].GetUInt32();
                    l_Building.Active           = l_Fields[7].GetBool();
                    l_Building.GatheringData    = l_Fields[8].GetString();

                    if (!l_Building.Active && time(0) > l_Building.TimeBuiltEnd)
                        l_Building.BuiltNotified = true;    ///< Auto notify by info packet

                    m_Buildings.push_back(l_Building);
                } while (p_BuildingsResult->NextRow());
            }

            Init();

            if (!GetGarrisonSiteLevelEntry())
                return false;

            /// Garrison ability
            if (!m_Owner->HasSpell(GARRISON_SPELL_GARR_ABILITY))
                m_Owner->learnSpell(GARRISON_SPELL_GARR_ABILITY, false);

            uint32 l_QuestEntry = 0;
            /// Select Garrison site ID
            switch (m_Manager->GetGarrisonFactionIndex())
            {
                case MS::Garrison::FactionIndex::Alliance:
                    l_QuestEntry = BaseQuests::FoundedGarrisonA;
                    break;

                case MS::Garrison::FactionIndex::Horde:
                    l_QuestEntry = BaseQuests::FoundedGarrisonH;
                    break;

                case MS::Garrison::FactionIndex::Max:
                default:
                    ASSERT(false);
                    break;

            }

            /// Minimap icons are completed quest, reason => BLIZZARD !!!!!!!!!!!!!!
            if (m_Owner->GetQuestStatus(l_QuestEntry) != QUEST_STATUS_REWARDED)
            {
                if (const Quest* l_Quest = sObjectMgr->GetQuestTemplate(l_QuestEntry))
                {
                    m_Owner->AddQuest(l_Quest, m_Owner);
                    m_Owner->CompleteQuest(l_QuestEntry);
                }
            }

            sGarrisonDraenorBuildingManager->LearnAllowedBuildings(m_Owner, this);

            return true;
        }

        return false;
    }

    /// Save this garrison to DB
    void GarrisonDraenor::Save(SQLTransaction p_Transaction)
    {
        std::ostringstream l_KnownBluePrintsStr;

        for (uint32 l_I = 0; l_I < m_KnownBlueprints.size(); ++l_I)
            l_KnownBluePrintsStr << m_KnownBlueprints[l_I] << ' ';

        std::ostringstream l_KnownSpecializationsStr;

        for (uint32 l_I = 0; l_I < m_KnownSpecializations.size(); ++l_I)
            l_KnownSpecializationsStr << m_KnownSpecializations[l_I] << ' ';

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON_DRAENOR);

        std::string l_KnownBluePrints = l_KnownBluePrintsStr.str();
        if (l_KnownBluePrints.empty())
            l_KnownBluePrints = " ";

        std::string l_KnowSpecializations = l_KnownSpecializationsStr.str();
        if (l_KnowSpecializations.empty())
            l_KnowSpecializations = " ";

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, m_GarrisonLevel);
        l_Stmt->setString(l_Index++, l_KnownBluePrints);
        l_Stmt->setString(l_Index++, l_KnowSpecializations);
        l_Stmt->setUInt32(l_Index++, m_CacheLastUsage);
        l_Stmt->setUInt32(l_Index++, m_Manager->GetGarrisonID());

        p_Transaction->Append(l_Stmt);

        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            PreparedStatement* l_BuildingStatement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON_DRAENOR_BUILDING);

            l_Index = 0;
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].PlotInstanceID);
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].BuildingID);
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].SpecID);
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].TimeBuiltStart);
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].TimeBuiltEnd);
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].FollowerAssigned);
            l_BuildingStatement->setBool  (l_Index++, m_Buildings[l_I].Active);
            l_BuildingStatement->setString(l_Index++, m_Buildings[l_I].GatheringData);
            l_BuildingStatement->setUInt32(l_Index++, m_Buildings[l_I].DatabaseID);
            l_BuildingStatement->setUInt32(l_Index++, m_Manager->GetGarrisonID());

            p_Transaction->Append(l_BuildingStatement);
        }
    }

    /// Delete garrison
    void GarrisonDraenor::DeleteFromDB(uint64 p_PlayerGUID, SQLTransaction p_Transation)
    {
        PreparedStatement* l_Stmt;

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_DRAENOR_BUILDINGS);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_DRAENOR);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update the garrison
    void GarrisonDraenor::Update()
    {
        /// Update buildings
        UpdateBuildings();
        /// Update cache
        UpdateCache();
        /// Update garrison ability
        UpdateGarrisonAbility();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Set garrison level
    void GarrisonDraenor::SetLevel(uint32 p_Level)
    {
        m_GarrisonLevel = p_Level;

        UninitPlots();
        Init();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get garrison cache token count
    uint32 GarrisonDraenor::GetGarrisonCacheTokenCount() const
    {
        return m_CacheLastTokenAmount;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get terrain swaps
    void GarrisonDraenor::GetUIWorldMapAreaSwaps(std::set<uint32>& p_UIWorldMapAreaSwaps) const
    {
        if (!GetGarrisonSiteLevelEntry())
            return;

        if (m_Manager->GetGarrisonFactionIndex() == MS::Garrison::FactionIndex::Horde)
        {
            switch (GetGarrisonSiteLevelEntry()->Level)
            {
                case 1:
                    p_UIWorldMapAreaSwaps.emplace(UI_WORLD_MAP_AREA_SWAP_GARRISON_FF_HORDE_TIER_1);
                    break;

                case 2:
                    p_UIWorldMapAreaSwaps.emplace(UI_WORLD_MAP_AREA_SWAP_GARRISON_FF_HORDE_TIER_2);
                    break;

                case 3:
                    p_UIWorldMapAreaSwaps.emplace(UI_WORLD_MAP_AREA_SWAP_GARRISON_FF_HORDE_TIER_3);
                    break;
            }
        }
        else
        {
            switch (GetGarrisonSiteLevelEntry()->Level)
            {
                case 1:
                    p_UIWorldMapAreaSwaps.emplace(UI_WORLD_MAP_AREA_SWAP_GARRISON_SMV_ALLIANCE_TIER_1);
                    break;

                case 2:
                    p_UIWorldMapAreaSwaps.emplace(UI_WORLD_MAP_AREA_SWAP_GARRISON_SMV_ALLIANCE_TIER_2);
                    break;

                case 3:
                    p_UIWorldMapAreaSwaps.emplace(UI_WORLD_MAP_AREA_SWAP_GARRISON_SMV_ALLIANCE_TIER_3);
                    break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get garrison script
    Interfaces::GarrisonSite* GarrisonDraenor::GetGarrisonScript() const
    {
        return m_GarrisonScriptID ? sObjectMgr->GetGarrisonSiteBase(m_GarrisonScriptID) : nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Can upgrade the garrison
    bool GarrisonDraenor::CanUpgrade(uint32 & p_Reason) const
    {
        p_Reason = ResultEnum::SUCCESS;

        if (m_Owner->isGameMaster() && m_GarrisonLevel < 3)
            return true;

        if (!m_Owner->HasCurrency(Globals::CurrencyID, GetGarrisonSiteLevelEntry()->UpgradeRessourceCost))
        {
            p_Reason = ResultEnum::ERROR_NOT_ENOUGH_CURRENCY;
            return false;
        }

        if (!m_Owner->HasEnoughMoney((int64)GetGarrisonSiteLevelEntry()->UpgradeMoneyCost))
        {
            p_Reason = ResultEnum::ERROR_NOT_ENOUGH_GOLD;
            return false;
        }

        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (l_GarrisonScript)
            return l_GarrisonScript->CanUpgrade(m_Owner, m_GarrisonLevel);

        return false;
    }

    /// Upgrade the garrison
    void GarrisonDraenor::Upgrade()
    {
        if (!m_Owner->isGameMaster())
        {
            m_Owner->ModifyCurrency(Globals::CurrencyID, -((int32)GetGarrisonSiteLevelEntry()->UpgradeRessourceCost));
            m_Owner->ModifyMoney(-((int64)GetGarrisonSiteLevelEntry()->UpgradeMoneyCost), "GarrisonDraenor::Upgrade");
        }

        SetLevel(m_GarrisonLevel + 1);

        m_Owner->UpdateCriteria(CRITERIA_TYPE_UPGRADE_GARRISON, m_GarrisonLevel + 1);

        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (l_GarrisonScript)
            l_GarrisonScript->OnUpgrade(m_Owner);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Reward garrison cache content
    void GarrisonDraenor::RewardGarrisonCache()
    {
        m_Owner->SendDisplayToast(Globals::CurrencyID, m_CacheLastTokenAmount, 0, DISPLAY_TOAST_METHOD_GARRISON_CACHE, TOAST_TYPE_NEW_CURRENCY, false, false);
        m_Owner->ModifyCurrency(Globals::CurrencyID, m_CacheLastTokenAmount, false);

        m_CacheLastTokenAmount  = 0;
        m_CacheLastUsage        = time(0);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// When the garrison owner enter in the garrisson (@See Player::UpdateArea)
    void GarrisonDraenor::OnPlayerEnter()
    {
        InitPlots();    ///< AKA update plots

        /// Enable AI Client collision manager
        m_Owner->SetFlag(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_AI_OBSTACLE);

        for (std::map<uint32, uint64>::iterator l_It = m_PlotsActivateGob.begin(); l_It != m_PlotsActivateGob.end(); ++l_It)
        {
            if (GameObject* l_Gob = HashMapHolder<GameObject>::Find(l_It->second))
                l_Gob->SendGameObjectActivateAnimKit(1696);
        }
    }

    /// When the garrison owner leave the garrisson (@See Player::UpdateArea)
    void GarrisonDraenor::OnPlayerLeave()
    {
        /// Prevent call from World::Update thread
        if (!Map::GetCurrentMapThread())
            return;

        UninitPlots();

        if (m_CacheGameObjectGUID)
        {
            GameObject* l_Cache = Map::GetCurrentMapThread()->GetGameObject(m_CacheGameObjectGUID);

            if (l_Cache)
            {
                uint64 l_Guid = m_CacheGameObjectGUID;
                Map* l_Map = l_Cache->GetMap();
                l_Cache->GetMap()->AddTask([l_Guid, l_Map]()->void
                {
                    GameObject* l_Cache = l_Map->GetGameObject(l_Guid);
                    if (l_Cache)
                    {
                        l_Cache->DestroyForNearbyPlayers();
                        l_Cache->AddObjectToRemoveList();
                    }
                });
            }

            m_CacheGameObjectGUID = 0;
        }

        m_Owner->SetPhaseMask(1, true);

        /// Disable AI Client collision manager
        m_Owner->RemoveFlag(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_AI_OBSTACLE);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// set last used activation game  object
    void GarrisonDraenor::SetLastUsedActivationGameObject(uint64 p_Guid)
    {
        m_LastUsedActivationGameObject = p_Guid;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get GarrSiteLevelEntry for current garrison
    const GarrSiteLevelEntry* GarrisonDraenor::GetGarrisonSiteLevelEntry() const
    {
        return sGarrSiteLevelStore.LookupEntry(m_GarrisonLevelID);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get slots for level
    std::vector<GarrisonPlotInstanceInfoLocation> GarrisonDraenor::GetPlots()
    {
        return m_Plots;
    }
    /// Get plot by position
    GarrisonPlotInstanceInfoLocation GarrisonDraenor::GetPlot(float p_X, float p_Y, float p_Z)
    {
        GarrisonPlotInstanceInfoLocation l_Plot = { 0, 0, 0.0f, 0.0f, 0.0f, 0.0f} ;

        if (m_Owner && !m_Owner->IsInDraenorGarrison())
            return l_Plot;

        Position                            l_Position;

        memset(&l_Plot, 0, sizeof(GarrisonPlotInstanceInfoLocation));

        l_Position.m_positionX = p_X;
        l_Position.m_positionY = p_Y;
        l_Position.m_positionZ = p_Z;

        for (uint32 l_I = 0; l_I < m_Plots.size(); ++l_I)
        {
            if (l_Position.GetExactDist2d(m_Plots[l_I].X, m_Plots[l_I].Y) < l_Position.GetExactDist2d(l_Plot.X, l_Plot.Y))
            {
                /// Specific check for mine, plot surface is way bigger than other plots
                if (GetBuilding(m_Plots[l_I].PlotInstanceID).BuildingID == 61 || l_Position.GetExactDist2d(m_Plots[l_I].X, m_Plots[l_I].Y) < 35.0f)
                    l_Plot = m_Plots[l_I];
            }
        }

        return l_Plot;
    }
    /// Get plot instance plot type
    uint32 GarrisonDraenor::GetPlotType(uint32 p_PlotInstanceID) const
    {
        GarrPlotInstanceEntry const* l_PlotInstanceEntry = sGarrPlotInstanceStore.LookupEntry(p_PlotInstanceID);

        if (!l_PlotInstanceEntry)
            return 0;

        GarrPlotEntry const* l_PlotEntry = sGarrPlotStore.LookupEntry(l_PlotInstanceEntry->PlotID);

        if (!l_PlotEntry)
            return 0;

        return l_PlotEntry->PlotType;
    }
    /// Plot is free ?
    bool GarrisonDraenor::PlotIsFree(uint32 p_PlotInstanceID) const
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
            if (m_Buildings[l_I].PlotInstanceID == p_PlotInstanceID)
                return false;

        return true;
    }
    /// Has plot instance
    bool GarrisonDraenor::HasPlotInstance(uint32 p_PlotInstanceID) const
    {
        for (uint32 l_I = 0; l_I < m_Plots.size(); ++l_I)
            if (m_Plots[l_I].PlotInstanceID == p_PlotInstanceID)
                return true;

        return false;
    }
    /// Get plot location
    GarrisonPlotInstanceInfoLocation GarrisonDraenor::GetPlot(uint32 p_PlotInstanceID) const
    {
        for (uint32 l_I = 0; l_I < m_Plots.size(); ++l_I)
        {
            if (m_Plots[l_I].SiteLevelID == m_GarrisonLevelID && m_Plots[l_I].PlotInstanceID == p_PlotInstanceID)
                return m_Plots[l_I];
        }

        return GarrisonPlotInstanceInfoLocation();
    }
    /// Get plot instance ID by activation game object
    uint32 GarrisonDraenor::GetPlotInstanceIDByActivationGameObject(uint64 p_Guid) const
    {
        for (std::map<uint32, uint64>::const_iterator l_It = m_PlotsActivateGob.begin(); l_It != m_PlotsActivateGob.end(); ++l_It)
            if (l_It->second == p_Guid)
                return l_It->first;

        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Can build building X at slot instance Y
    bool GarrisonDraenor::IsBuildingPlotInstanceValid(uint32 p_BuildingRecID, uint32 p_PlotInstanceID) const
    {
        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(p_BuildingRecID);

        if (!l_BuildingEntry)
            return false;

        GarrPlotInstanceEntry const* l_PlotInstanceEntry = sGarrPlotInstanceStore.LookupEntry(p_PlotInstanceID);

        if (!l_PlotInstanceEntry)
            return false;

        /// Search building plot ID
        for (uint32 l_I = 0; l_I < sGarrPlotBuildingStore.GetNumRows(); ++l_I)
        {
            GarrPlotBuildingEntry const* l_PlotBuildingEntry = sGarrPlotBuildingStore.LookupEntry(l_I);

            if (l_PlotBuildingEntry && l_PlotBuildingEntry->PlotID == l_PlotInstanceEntry->PlotID && l_PlotBuildingEntry->BuildingID == p_BuildingRecID)
                return true;
        }

        return false;
    }
    /// Player fill all condition
    PurchaseBuildingResults::Type GarrisonDraenor::CanPurchaseBuilding(uint32 p_BuildingRecID) const
    {
        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(p_BuildingRecID);

        if (!l_BuildingEntry)
            return PurchaseBuildingResults::InvalidBuildingID;

        if (l_BuildingEntry->CostCurrencyID != 0)
        {
            if (!m_Owner->HasCurrency(l_BuildingEntry->CostCurrencyID, l_BuildingEntry->CostCurrencyAmount))
                return PurchaseBuildingResults::NotEnoughCurrency;

            if (!m_Owner->HasEnoughMoney((uint64)l_BuildingEntry->CostMoney * GOLD))
                return PurchaseBuildingResults::NotEnoughGold;
        }

        return PurchaseBuildingResults::Ok;
    }
    /// PurchaseBuilding
    GarrisonBuilding GarrisonDraenor::PurchaseBuilding(uint32 p_BuildingRecID, uint32 p_PlotInstanceID, bool p_Triggered)
    {
        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(p_BuildingRecID);

        GarrisonBuilding l_Building;
        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (!l_BuildingEntry)
            return l_Building;

        if (!PlotIsFree(p_PlotInstanceID))
        {
            GarrPlotInstanceEntry const* l_PlotInstanceEntry = sGarrPlotInstanceStore.LookupEntry(p_PlotInstanceID);
            GarrisonBuilding l_Building = GetBuilding(p_PlotInstanceID);
            GarrBuildingEntry const* l_OldBuildingEntry = sGarrBuildingStore.LookupEntry(l_Building.BuildingID);
            bool l_ForUpgrade = false;

            if (l_OldBuildingEntry && l_PlotInstanceEntry && l_OldBuildingEntry->Type == l_BuildingEntry->Type && l_OldBuildingEntry->Level < l_BuildingEntry->Level && l_GarrisonScript)
            {
                m_LastPlotBuildingType.insert(std::make_pair(l_PlotInstanceEntry->PlotID, Building::Type(l_OldBuildingEntry->Type)));
                l_ForUpgrade = true;
            }

            DeleteBuilding(p_PlotInstanceID, false, l_ForUpgrade);
        }

        if (l_BuildingEntry->CostCurrencyID != 0 && !p_Triggered)
            m_Owner->ModifyCurrency(l_BuildingEntry->CostCurrencyID, -(int32)l_BuildingEntry->CostCurrencyAmount);

        if (l_BuildingEntry->CostMoney != 0 && !p_Triggered)
            m_Owner->ModifyMoney(-(int64)(l_BuildingEntry->CostMoney * GOLD), "GarrisonDraenor::PurchaseBuilding");

        if (!p_Triggered)
        {
            WorldPacket l_PlotRemoved(SMSG_GARRISON_PLOT_REMOVED, 4);
            l_PlotRemoved << uint32(p_PlotInstanceID);
            m_Owner->SendDirectMessage(&l_PlotRemoved);
        }

        uint32 l_BuildingTime = l_BuildingEntry->BuildDuration;

        if (l_GarrisonScript)
            l_BuildingTime = l_GarrisonScript->OnPrePurchaseBuilding(m_Owner, p_BuildingRecID, l_BuildingTime);

        l_Building.DatabaseID       = sObjectMgr->GetNewGarrisonBuildingID();
        l_Building.BuildingID       = p_BuildingRecID;
        l_Building.PlotInstanceID   = p_PlotInstanceID;
        l_Building.TimeBuiltStart   = time(nullptr);
        l_Building.TimeBuiltEnd     = time(nullptr) + l_BuildingTime;           ///< 5/5/1905 18:45:05
        l_Building.SpecID           = 0;
        l_Building.Active           = false;
        l_Building.BuiltNotified    = false;

        if (l_BuildingEntry->Flags == BuildingCategory::Prebuilt)
        {
            l_Building.TimeBuiltStart   = time(nullptr) - l_BuildingTime;
            l_Building.TimeBuiltEnd     = time(nullptr);
            l_Building.Active           = false;
            l_Building.BuiltNotified    = true;
        }
        else if (p_Triggered)
            l_Building.TimeBuiltEnd = l_Building.TimeBuiltStart;

        if (l_BuildingEntry->Type == Globals::ShipyardBuildingType)
        {
            l_Building.Active           = true;
            l_Building.BuiltNotified    = true;
        }

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_DRAENOR_BUILDING);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, l_Building.DatabaseID);
        l_Stmt->setUInt32(l_Index++, m_Manager->GetGarrisonID());
        l_Stmt->setUInt32(l_Index++, l_Building.PlotInstanceID);
        l_Stmt->setUInt32(l_Index++, l_Building.BuildingID);
        l_Stmt->setUInt32(l_Index++, l_Building.SpecID);
        l_Stmt->setUInt32(l_Index++, l_Building.TimeBuiltStart);
        l_Stmt->setUInt32(l_Index++, l_Building.TimeBuiltEnd);
        l_Stmt->setUInt32(l_Index++, l_Building.FollowerAssigned);
        l_Stmt->setBool(l_Index++, l_Building.Active);
        l_Stmt->setString(l_Index++, l_Building.GatheringData);

        CharacterDatabase.AsyncQuery(l_Stmt);

        m_Buildings.push_back(l_Building);

        UpdatePlot(p_PlotInstanceID);

        if (l_GarrisonScript)
            l_GarrisonScript->OnPurchaseBuilding(m_Owner, p_BuildingRecID);

        return l_Building;
    }
    /// Get building
    GarrisonBuilding GarrisonDraenor::GetBuilding(uint32 p_PlotInstanceID) const
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
            if (m_Buildings[l_I].PlotInstanceID == p_PlotInstanceID)
                return m_Buildings[l_I];

        return GarrisonBuilding();
    }
    /// Get building
    GarrisonBuilding GarrisonDraenor::GetBuildingWithBuildingID(uint32 p_BuildingID) const
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        if (m_Buildings[l_I].BuildingID == p_BuildingID)
            return m_Buildings[l_I];

        return GarrisonBuilding();
    }
    /// Get buildings
    std::vector<GarrisonBuilding> GarrisonDraenor::GetBuildings() const
    {
        std::vector<GarrisonBuilding> l_Buildings;

        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            /// Don't include the building if the owner doesn't have access to it
            if (!sGarrisonDraenorBuildingManager->MatchsConditionsForBuilding(m_Buildings[l_I].BuildingID, m_Owner))
                continue;

            l_Buildings.push_back(m_Buildings[l_I]);
        }

        return l_Buildings;
    }
    /// Get Building object
    GarrisonBuilding* GarrisonDraenor::GetBuildingObject(uint32 p_PlotInstanceID)
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); l_I++)
        {
            if (m_Buildings[l_I].PlotInstanceID == p_PlotInstanceID)
                return &m_Buildings[l_I];
        }

        return nullptr;
    }
    /// Get building passive ability effects
    std::vector<uint32> GarrisonDraenor::GetBuildingsPassiveAbilityEffects() const
    {
        std::vector<uint32> l_PassiveEffects;

        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            /// Don't include prebuilt building passives if the owner doesn't have access to it
            if (!sGarrisonDraenorBuildingManager->MatchsConditionsForBuilding(m_Buildings[l_I].BuildingID, m_Owner))
                continue;

            GarrBuildingEntry const* l_BuildingTemplate = sGarrBuildingStore.LookupEntry(m_Buildings[l_I].BuildingID);

            if (l_BuildingTemplate && l_BuildingTemplate->FollowerGarrAbilityEffectID && sGarrAbilityEffectStore.LookupEntry(l_BuildingTemplate->FollowerGarrAbilityEffectID) != nullptr)
                l_PassiveEffects.push_back(l_BuildingTemplate->FollowerGarrAbilityEffectID);
        }

        return l_PassiveEffects;
    }
    /// Activate building
    void GarrisonDraenor::ActivateBuilding(uint32 p_PlotInstanceID)
    {
        GarrisonBuilding* l_Building = nullptr;

        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            if (m_Buildings[l_I].PlotInstanceID == p_PlotInstanceID)
            {
                l_Building = &m_Buildings[l_I];
                break;
            }
        }

        if (!l_Building)
            return;

        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_Building->BuildingID);

        if (!l_BuildingEntry)
            return;

        l_Building->Active = true;

        UpdatePlot(p_PlotInstanceID);

        WorldPacket l_Packet(SMSG_GARRISON_BUILDING_ACTIVATED, 4);
        l_Packet << uint32(p_PlotInstanceID);
        m_Owner->SendDirectMessage(&l_Packet);

        m_Owner->UpdateCriteria(CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING, l_Building->BuildingID);

        UpdateStats();

        switch (l_BuildingEntry->Type)
        {
            case Building::Type::Fishing:
            case Building::Type::Mine:
            case Building::Type::Farm:
            {
                WorldPacket l_NullPacket;
                m_Owner->GetSession()->HandleGetGarrisonInfoOpcode(l_NullPacket);
                break;
            }
        }

        if (GetGarrisonScript())
        {
            GarrPlotInstanceEntry const* l_PlotInstanceEntry = sGarrPlotInstanceStore.LookupEntry(p_PlotInstanceID);

            if (l_PlotInstanceEntry && !m_LastPlotBuildingType.empty() && m_LastPlotBuildingType.find(l_PlotInstanceEntry->PlotID) != m_LastPlotBuildingType.end() && m_LastPlotBuildingType[l_PlotInstanceEntry->PlotID] == l_BuildingEntry->Type)
            {
                GetGarrisonScript()->OnUpgradeBuilding(m_Owner, l_BuildingEntry->ID);
                m_LastPlotBuildingType.erase(l_PlotInstanceEntry->PlotID);
            }
            else
                GetGarrisonScript()->OnBuildingActivated(m_Owner, l_Building->BuildingID);
        }
    }
    /// Activate building
    void GarrisonDraenor::ActivateBuilding()
    {
        uint32 l_PlotInstance = GetPlotInstanceIDByActivationGameObject(m_LastUsedActivationGameObject);

        if (l_PlotInstance)
            ActivateBuilding(l_PlotInstance);
    }
    /// Cancel construction
    void GarrisonDraenor::CancelConstruction(uint32 p_PlotInstanceID)
    {
        if (!HasPlotInstance(p_PlotInstanceID))
            return;

        uint32 l_BuildingID = GetBuilding(p_PlotInstanceID).BuildingID;

        if (!l_BuildingID)
            return;

        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_BuildingID);

        if (!l_BuildingEntry)
            return;

        DeleteBuilding(p_PlotInstanceID, true, false);

        if (l_BuildingEntry->CostCurrencyID != 0)
            m_Owner->ModifyCurrency(l_BuildingEntry->CostCurrencyID, (int32)l_BuildingEntry->CostCurrencyAmount);

        if (l_BuildingEntry->CostMoney != 0)
            m_Owner->ModifyMoney(l_BuildingEntry->CostMoney, "GarrisonDraenor::CancelConstruction");
    }
    /// Delete building
    void GarrisonDraenor::DeleteBuilding(uint32 p_PlotInstanceID, bool p_Canceled, bool p_RemoveForUpgrade)
    {
        if (!HasPlotInstance(p_PlotInstanceID))
            return;

        uint32 l_BuildingID = GetBuilding(p_PlotInstanceID).BuildingID;

        if (!l_BuildingID)
            return;

        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_BuildingID);

        if (!l_BuildingEntry)
            return;

        if (GetGarrisonScript())
        {
            GetGarrisonScript()->OnDeleteBuilding(m_Owner, l_BuildingID, l_BuildingEntry->Type, p_RemoveForUpgrade);
        }

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_DRAENOR_BUILDING);
        l_Stmt->setUInt32(0, GetBuilding(p_PlotInstanceID).DatabaseID);
        CharacterDatabase.AsyncQuery(l_Stmt);

        for (std::vector<GarrisonBuilding>::iterator l_It = m_Buildings.begin(); l_It != m_Buildings.end(); ++l_It)
        {
            if (l_It->BuildingID == l_BuildingID)
            {
                m_Buildings.erase(l_It);
                break;
            }
        }

        if (p_Canceled)
        {
            GarrPlotInstanceEntry const* l_PlotInstanceEntry = sGarrPlotInstanceStore.LookupEntry(p_PlotInstanceID);

            if (l_PlotInstanceEntry && !m_LastPlotBuildingType.empty() && m_LastPlotBuildingType.find(l_PlotInstanceEntry->PlotID) != m_LastPlotBuildingType.end())
                m_LastPlotBuildingType.erase(l_PlotInstanceEntry->PlotID);
        }

        AssignFollowerToBuilding(GetBuilding(p_PlotInstanceID).FollowerAssigned, 0);

        UpdatePlot(p_PlotInstanceID);

        GarrisonPlotInstanceInfoLocation l_PlotLocation = GetPlot(p_PlotInstanceID);

        WorldPacket l_PlotPlacedPacket(SMSG_GARRISON_PLOT_PLACED, 24);
        l_PlotPlacedPacket << uint32(GarrisonType::GarrisonDraenor);

        l_PlotPlacedPacket << uint32(p_PlotInstanceID);
        l_PlotPlacedPacket << float(l_PlotLocation.X);
        l_PlotPlacedPacket << float(l_PlotLocation.Y);
        l_PlotPlacedPacket << float(l_PlotLocation.Z);
        l_PlotPlacedPacket << float(l_PlotLocation.O);
        l_PlotPlacedPacket << uint32(GetPlotType(p_PlotInstanceID));

        m_Owner->SendDirectMessage(&l_PlotPlacedPacket);

        WorldPacket l_BuildingRemovedPacket(SMSG_GARRISON_BUILDING_REMOVED, 12);
        l_BuildingRemovedPacket << uint32(GarrisonType::GarrisonDraenor);
        l_BuildingRemovedPacket << uint32(PurchaseBuildingResults::Ok);
        l_BuildingRemovedPacket << uint32(p_PlotInstanceID);
        l_BuildingRemovedPacket << uint32(l_BuildingID);

        m_Owner->SendDirectMessage(&l_BuildingRemovedPacket);
    }
    /// Has active building
    bool GarrisonDraenor::HasActiveBuilding(uint32 p_BuildingID) const
    {
        for (std::vector<GarrisonBuilding>::const_iterator l_It = m_Buildings.begin(); l_It != m_Buildings.end(); ++l_It)
        {
            if (l_It->BuildingID == p_BuildingID && l_It->Active)
                return true;
        }

        return false;
    }
    /// Has building type
    bool GarrisonDraenor::HasBuildingType(Building::Type p_BuildingType, bool p_DontNeedActive) const
    {
        for (std::vector<GarrisonBuilding>::const_iterator l_It = m_Buildings.begin(); l_It != m_Buildings.end(); ++l_It)
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_It->BuildingID);

            if (!l_BuildingEntry)
                continue;

            if (l_BuildingEntry->Type == p_BuildingType && (p_DontNeedActive ? true : (*l_It).Active == true))
                return true;
        }

        return false;
    }
    /// Get building with type
    GarrisonBuilding GarrisonDraenor::GetBuildingWithType(Building::Type p_BuildingType, bool p_DontNeedActive) const
    {
        for (GarrisonBuilding l_Building : m_Buildings)
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_Building.BuildingID);

            if (!l_BuildingEntry)
                continue;

            if (l_BuildingEntry->Type == p_BuildingType && (p_DontNeedActive ? true : l_Building.Active == true))
                return l_Building;
        }

        return GarrisonBuilding();
    }

    uint32 GarrisonDraenor::GetBuildingLevel(GarrisonBuilding p_Building) const
    {
        if (GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(p_Building.BuildingID))
            return l_BuildingEntry->Level;

        return 0;
    }

    /// Get building max work order
    uint32 GarrisonDraenor::GetBuildingMaxWorkOrder(uint32 p_PlotInstanceID) const
    {
        if (!HasPlotInstance(p_PlotInstanceID))
            return 0;

        uint32 l_BuildingID = GetBuilding(p_PlotInstanceID).BuildingID;

        if (!l_BuildingID)
            return 0;

        GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_BuildingID);

        if (!l_BuildingEntry)
            return 0;

        uint32 l_MaxWorkOrder = 0;

        for (uint32 l_CurrentValue : gGarrisonBuildingMaxWorkOrderPerBuildingLevel)
        {
            if (l_CurrentValue == l_BuildingEntry->MaxShipments)
            {
                l_MaxWorkOrder += l_BuildingEntry->MaxShipments;
                break;
            }
        }

        if (!l_MaxWorkOrder)
            return 0;

        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            GarrBuildingEntry const* l_Building = sGarrBuildingStore.LookupEntry(m_Buildings[l_I].BuildingID);

            if (!l_Building)
                continue;

            if (l_Building->Type != Building::Type::StoreHouse)
                continue;

            l_MaxWorkOrder += l_Building->MaxShipments;
        }

        return l_MaxWorkOrder;
    }

    uint8 GarrisonDraenor::CalculateAssignedFollowerShipmentBonus(uint32 p_PlotInstanceID)
    {
        GarrisonFollower* l_Follower = m_Manager->GetFollower(GetBuilding(p_PlotInstanceID).FollowerAssigned);

        if (l_Follower == nullptr)
            return 1;

        return HasRequiredFollowerAssignedAbility(p_PlotInstanceID) ? roll_chance_i(5 * (l_Follower->Level - 90) + 50) + 1 : 1;
    }

    GarrisonFollower* GarrisonDraenor::GetAssignedFollower(uint32 p_PlotInstanceID)
    {
        return m_Manager->GetFollowerWithDatabaseID(GetBuilding(p_PlotInstanceID).FollowerAssigned);
    }

    bool GarrisonDraenor::HasRequiredFollowerAssignedAbility(uint32 p_PlotInstanceID)
    {
        GarrisonFollower* l_Follower = m_Manager->GetFollowerWithDatabaseID(GetBuilding(p_PlotInstanceID).FollowerAssigned);

        if (l_Follower == nullptr)
            return false;

        GarrBuildingEntry const* l_Building = sGarrBuildingStore.LookupEntry(GetBuilding(p_PlotInstanceID).BuildingID);

        if (l_Building == nullptr)
            return false;

        for (uint32 l_Ability : l_Follower->Abilities)
        {
            if (l_Ability == l_Building->FollowerRequiredGarrAbilityID)
                return true;
        }

        return false;
    }

    uint32 GarrisonDraenor::CalculateArmoryWorkOrder() const
    {
        std::vector<uint32> l_UncommonRewards = { 114745, 114807, 114128, 114616 };
        std::vector<uint32> l_RareRewards     = { 114808, 114806, 114129, 114081 };
        std::vector<uint32> l_EpicRewards     = { 114822, 114746, 114131, 114622 };

        uint32 l_Chance = urand(0, 100);

        if (l_Chance > 40)
            return l_UncommonRewards[urand(0, l_UncommonRewards.size() - 1)];
        else if (l_Chance > 10)
            return l_RareRewards[urand(0, l_RareRewards.size() - 1)];
        else
            return l_EpicRewards[urand(0, l_EpicRewards.size() - 1)];

        return 0;
    }

    bool GarrisonDraenor::FillSanctumWorkOrderRewards(std::map<uint32, uint32>& l_RewardItems, std::map<CurrencyTypes, uint32>& l_RewardCurrencies)
    {
        uint8 l_Chance = urand(1, 100);
        /// 50%
        if (l_Chance >= 50)
        {
            uint32 l_Count = urand(100, 150) * CURRENCY_PRECISION;
            auto l_Itr = l_RewardCurrencies.find(CurrencyTypes::CURRENCY_TYPE_HONOR_POINTS);

            if (l_Itr != l_RewardCurrencies.end())
                l_Itr->second += l_Count;
            else
                l_RewardCurrencies.insert({CurrencyTypes::CURRENCY_TYPE_HONOR_POINTS, l_Count});

            return true;
        }
        /// 30%
        else if (l_Chance >= 20)
        {
            uint32 l_CurrencyCount = 0;

            if (m_Owner->GetCurrencyWeekCap(CurrencyTypes::CURRENCY_TYPE_CONQUEST_META_ARENA_BG, false) > m_Owner->GetCurrencyOnWeek(CURRENCY_TYPE_CONQUEST_META_ARENA_BG, false))
            {

                l_CurrencyCount = urand(75, 150) * CURRENCY_PRECISION;

                auto l_Itr = l_RewardCurrencies.find(CurrencyTypes::CURRENCY_TYPE_CONQUEST_META_ARENA_BG);
                if (l_Itr != l_RewardCurrencies.end())
                    l_Itr->second += l_CurrencyCount;
                else
                    l_RewardCurrencies.insert({ CurrencyTypes::CURRENCY_TYPE_CONQUEST_META_ARENA_BG, l_CurrencyCount });

                return true;
            }

            l_CurrencyCount = urand(100, 150) * CURRENCY_PRECISION;

            auto l_Itr = l_RewardCurrencies.find(CurrencyTypes::CURRENCY_TYPE_HONOR_POINTS);
            if (l_Itr != l_RewardCurrencies.end())
                l_Itr->second += l_CurrencyCount;
            else
                l_RewardCurrencies.insert({ CurrencyTypes::CURRENCY_TYPE_HONOR_POINTS, l_CurrencyCount });

            return true;
        }
        /// 20%
        else
        {
            LootTemplate const* l_LootTemplate = LootTemplates_Item.GetLootFor(m_Owner->GetTeam() == ALLIANCE ? 126906 : 126901); //< Golden Strongbox Loot

            std::list<ItemTemplate const*> l_LootTable;
            std::vector<uint32> l_Items;
            uint32 l_SpecID = m_Owner->GetLootSpecId() ? m_Owner->GetLootSpecId() : m_Owner->GetActiveSpecializationID();

            l_LootTemplate->FillAutoAssignationLoot(l_LootTable);

            if (!l_SpecID)
                l_SpecID = m_Owner->GetDefaultSpecId();

            for (ItemTemplate const* l_Template : l_LootTable)
            {
                if ((l_Template->AllowableClass && !(l_Template->AllowableClass & m_Owner->getClassMask())) ||
                    (l_Template->AllowableRace && !(l_Template->AllowableRace & m_Owner->getRaceMask())))
                    continue;

                if (l_Template->HasSpec(static_cast<SpecIndex>(l_SpecID), m_Owner->getLevel()))
                    l_Items.push_back(l_Template->ItemId);
            }

            if ((uint32)l_Items.size())
                l_RewardItems.insert({ l_Items[urand(0, l_Items.size())], 1 });
        }

        return false;
    }

    void GarrisonDraenor::InsertNewCreatureInPlotDatas(uint32 p_PlotInstanceID, uint64 p_Guid)
    {
        m_PlotsCreatures[p_PlotInstanceID].push_back(p_Guid);
    }

    /// Get creature plot instance ID
    uint32 GarrisonDraenor::GetCreaturePlotInstanceID(uint64 p_GUID) const
    {
        for (auto & l_Pair : m_PlotsCreatures)
        {
            auto l_It = std::find(l_Pair.second.begin(), l_Pair.second.end(), p_GUID);

            if (l_It != l_Pair.second.end())
                return l_Pair.first;
        }

        return 0;
    }

    /// Get gameobject plot instance ID
    uint32 GarrisonDraenor::GetGameObjectPlotInstanceID(uint64 p_GUID) const
    {
        for (auto & l_Pair : m_PlotsGameObjects)
        {
            auto l_It = std::find(l_Pair.second.begin(), l_Pair.second.end(), p_GUID);

            if (l_It != l_Pair.second.end())
                return l_Pair.first;
        }

        return 0;
    }

    /// Get building gathering data
    /// @p_PlotInstanceID : Plot building location
    std::string GarrisonDraenor::GetBuildingGatheringData(uint32 p_PlotInstanceID)
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            if (m_Buildings[l_I].PlotInstanceID == p_PlotInstanceID)
                return m_Buildings[l_I].GatheringData;
        }

        return "";
    }

    /// Set building gathering data
    /// @p_PlotInstanceID   : Plot building location
    /// @p_Data             : Gathering data
    void GarrisonDraenor::SetBuildingGatheringData(uint32 p_PlotInstanceID, std::string p_Data)
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            if (m_Buildings[l_I].PlotInstanceID == p_PlotInstanceID)
            {
                m_Buildings[l_I].GatheringData = p_Data;
                return;
            }
        }
    }

    /// Get list of creature in a specific building type
    /// @p_Type : Building type
    std::vector<uint64> GarrisonDraenor::GetBuildingCreaturesByBuildingType(Building::Type p_Type)
    {
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(m_Buildings[l_I].BuildingID);

            if (!l_BuildingEntry)
                continue;

            if (l_BuildingEntry->Type == p_Type && m_Buildings[l_I].Active == true)
                return m_PlotsCreatures[m_Buildings[l_I].PlotInstanceID];
        }

        return std::vector<uint64>();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get known blueprints
    std::vector<int32> GarrisonDraenor::GetKnownBlueprints() const
    {
        return m_KnownBlueprints;
    }

    /// Learn blue print
    bool GarrisonDraenor::LearnBlueprint(uint32 p_BuildingRecID)
    {
        LearnBluePrintResults::Type l_ResultCode;

        if (std::find(m_KnownBlueprints.begin(), m_KnownBlueprints.end(), p_BuildingRecID) == m_KnownBlueprints.end())
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(p_BuildingRecID);

            if (l_BuildingEntry)
            {
                m_KnownBlueprints.push_back(p_BuildingRecID);
                l_ResultCode = LearnBluePrintResults::Learned;

                m_Owner->UpdateCriteria(CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT);
            }
            else
            {
                l_ResultCode = LearnBluePrintResults::UnableToLearn;
            }
        }
        else
        {
            l_ResultCode = LearnBluePrintResults::Known;
        }

        WorldPacket l_Result(SMSG_GARRISON_LEARN_BLUEPRINT_RESULT, 8);
        l_Result << uint32(GarrisonType::GarrisonDraenor);
        l_Result << uint32(l_ResultCode);
        l_Result << uint32(p_BuildingRecID);

        m_Owner->SendDirectMessage(&l_Result);

        return true;
    }

    /// Known blue print
    bool GarrisonDraenor::KnownBlueprint(uint32 p_BuildingRecID) const
    {
        return std::find(m_KnownBlueprints.begin(), m_KnownBlueprints.end(), p_BuildingRecID) != m_KnownBlueprints.end();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get known specializations
    std::vector<int32> GarrisonDraenor::GetKnownSpecializations() const
    {
        return m_KnownSpecializations;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Init
    void GarrisonDraenor::Init()
    {
        InitDataForLevel();
        InitPlots();
        UpdateStats();
    }

    /// Init data for level
    void GarrisonDraenor::InitDataForLevel()
    {
        GarrSiteLevelEntry const* l_SiteEntry = nullptr;

        /// Search garrison site entry by SideID & Level
        for (uint32 l_I = 0; l_I < sGarrSiteLevelStore.GetNumRows(); ++l_I)
        {
            GarrSiteLevelEntry const* l_CurrentSiteEntry = sGarrSiteLevelStore.LookupEntry(l_I);

            if (l_CurrentSiteEntry && l_CurrentSiteEntry->Level == m_GarrisonLevel && l_CurrentSiteEntry->SiteID == m_GarrisonSiteID)
            {
                l_SiteEntry = l_CurrentSiteEntry;
                break;
            }
        }

        if (l_SiteEntry == nullptr)
        {
            sLog->outAshran("Garrison::InitDataForLevel() not data found");
            assert(false && "Garrison::InitDataForLevel() not data found");
            return;
        }

        m_GarrisonLevelID = l_SiteEntry->SiteLevelID;

        /// Find all plots for garrison level
        m_Plots.clear();

        for (uint32 l_I = 0; l_I < Globals::PlotInstanceCount; ++l_I)
        {
            if (gGarrisonPlotInstanceInfoLocation[l_I].SiteLevelID == m_GarrisonLevelID)
                m_Plots.push_back(gGarrisonPlotInstanceInfoLocation[l_I]);
        }

        /// Add prebuilt buildings
        for (uint32 l_I = 0; l_I < sGarrBuildingStore.GetNumRows(); ++l_I)
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_I);

            if (!l_BuildingEntry || l_BuildingEntry->Flags != BuildingCategory::Prebuilt)
                continue;

            if (HasBuildingType((Building::Type)l_BuildingEntry->Type, true))
                continue;

            uint32 l_PlotID = 0;

            /// Search building plot ID
            for (uint32 l_I = 0; l_I < sGarrPlotBuildingStore.GetNumRows(); ++l_I)
            {
                GarrPlotBuildingEntry const* l_PlotBuildingEntry = sGarrPlotBuildingStore.LookupEntry(l_I);

                if (l_PlotBuildingEntry && l_PlotBuildingEntry->BuildingID == l_BuildingEntry->ID)
                {
                    l_PlotID = l_PlotBuildingEntry->PlotID;
                    break;
                }
            }

            if (!l_PlotID)
                continue;

            uint32 l_PlotInstanceID = 0;

            /// Search building plot instance ID
            for (uint32 l_I = 0; l_I < sGarrBuildingStore.GetNumRows(); ++l_I)
            {
                GarrPlotInstanceEntry const* l_PlotInstanceEntry = sGarrPlotInstanceStore.LookupEntry(l_I);

                if (l_PlotInstanceEntry && l_PlotInstanceEntry->PlotID == l_PlotID)
                {
                    l_PlotInstanceID = l_PlotInstanceEntry->InstanceID;
                    break;
                }
            }

            if (!l_PlotInstanceID || !HasPlotInstance(l_PlotInstanceID))
                continue;

            PurchaseBuilding(l_BuildingEntry->ID, l_PlotInstanceID, true);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Init Game objects
    void GarrisonDraenor::InitPlots()
    {
        if (!m_Owner->IsInDraenorGarrison())
            return;

        for (uint32 l_I = 0; l_I < m_Plots.size(); ++l_I)
            UpdatePlot(m_Plots[l_I].PlotInstanceID);
    }

    /// Uninit plots
    void GarrisonDraenor::UninitPlots()
    {
        for (std::map<uint32, uint64>::iterator l_It = m_PlotsActivateGob.begin(); l_It != m_PlotsActivateGob.end(); ++l_It)
        {
            GameObject* l_Gob = HashMapHolder<GameObject>::Find(l_It->second);

            if (l_Gob)
            {
                l_Gob->DestroyForNearbyPlayers();
                l_Gob->AddObjectToRemoveList();
            }
        }

        m_PlotsActivateGob.clear();

        for (std::map<uint32, uint64>::iterator l_It = m_PlotsGob.begin(); l_It != m_PlotsGob.end(); ++l_It)
        {
            GameObject* l_Gob = HashMapHolder<GameObject>::Find(l_It->second);

            if (l_Gob)
            {
                l_Gob->DestroyForNearbyPlayers();
                l_Gob->AddObjectToRemoveList();
            }
        }

        m_PlotsGob.clear();

        for (std::map<uint32, std::vector<uint64>>::iterator l_It = m_PlotsGameObjects.begin(); l_It != m_PlotsGameObjects.end(); ++l_It)
        {
            for (uint32 l_Y = 0; l_Y < l_It->second.size(); ++l_Y)
            {
                GameObject* l_Gob = HashMapHolder<GameObject>::Find(l_It->second[l_Y]);

                if (l_Gob)
                {
                    l_Gob->DestroyForNearbyPlayers();
                    l_Gob->AddObjectToRemoveList();
                }
            }
        }

        m_PlotsGameObjects.clear();

        for (std::map<uint32, std::vector<uint64>>::iterator l_It = m_PlotsCreatures.begin(); l_It != m_PlotsCreatures.end(); ++l_It)
        {
            for (uint32 l_Y = 0; l_Y < l_It->second.size(); ++l_Y)
            {
                Creature* l_Crea = HashMapHolder<Creature>::Find(l_It->second[l_Y]);

                if (l_Crea)
                {
                    l_Crea->DestroyForNearbyPlayers();
                    l_Crea->AddObjectToRemoveList();

                    if (l_Crea->AI())
                        l_Crea->AI()->SetData(CreatureAIDataIDs::DespawnData, 0);
                }
            }
        }

        m_PlotsCreatures.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update plot game object
    void GarrisonDraenor::UpdatePlot(uint32 p_PlotInstanceID)
    {
        if (!m_Owner->IsInDraenorGarrison())
            return;

        Map* l_Map = Map::GetCurrentMapThread();
        if (!l_Map)
            return;

        GarrisonPlotInstanceInfoLocation    l_PlotInfo = GetPlot(p_PlotInstanceID);
        GarrisonBuilding                    l_Building = GetBuilding(p_PlotInstanceID);

        if (m_PlotsGob[p_PlotInstanceID] != 0)
        {
            GameObject* l_Gob = HashMapHolder<GameObject>::Find(m_PlotsGob[p_PlotInstanceID]);

            if (l_Gob)
            {
                m_Owner->SendObjectDeSpawnAnim(m_PlotsGob[p_PlotInstanceID]);
                l_Gob->DestroyForNearbyPlayers();
                l_Gob->AddObjectToRemoveList();
            }

            m_PlotsGob[p_PlotInstanceID] = 0;
        }

        m_PlotsWorkOrderGob[p_PlotInstanceID] = 0;

        uint32 l_GobEntry = 0;
        bool l_SpanwActivateGob = false;
        bool l_IsPlotBuilding = false;

        if (PlotIsFree(p_PlotInstanceID))
        {
            l_GobEntry = gGarrisonEmptyPlotGameObject[GetPlotType(p_PlotInstanceID) + (m_Manager->GetGarrisonFactionIndex() * PlotTypes::Max)];
        }
        else
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_Building.BuildingID);

            if (!l_BuildingEntry)
                return;

            if (!l_Building.Active &&
                (l_BuildingEntry->Type != Building::Type::Mine &&
                 l_BuildingEntry->Type != Building::Type::Farm &&
                 l_BuildingEntry->Type != Building::Type::Fishing &&
                 l_BuildingEntry->Type != Building::Type::PetMenagerie))
            {
                l_GobEntry = gGarrisonBuildingPlotGameObject[GetPlotType(p_PlotInstanceID) + (m_Manager->GetGarrisonFactionIndex() * PlotTypes::Max)];

                if (time(0) > l_Building.TimeBuiltEnd)
                    l_SpanwActivateGob  = true;

                l_IsPlotBuilding = true;
            }
            else
            {
                l_GobEntry = l_BuildingEntry->GameObjects[m_Manager->GetGarrisonFactionIndex()];

                if (!l_Building.Active && l_BuildingEntry->Level > 1 &&
                    (l_BuildingEntry->Type == Building::Type::Mine ||
                     l_BuildingEntry->Type == Building::Type::Farm ||
                     l_BuildingEntry->Type == Building::Type::Fishing ||
                     l_BuildingEntry->Type == Building::Type::PetMenagerie))
                {
                    uint32 l_TargetLevel = l_BuildingEntry->Level - 1;

                    for (uint32 l_I = 0; l_I < sGarrBuildingStore.GetNumRows(); ++l_I)
                    {
                        GarrBuildingEntry const* l_CurrentEntry = sGarrBuildingStore.LookupEntry(l_I);

                        if (!l_CurrentEntry || l_CurrentEntry->Type != l_BuildingEntry->Type || l_CurrentEntry->Level != l_TargetLevel)
                            continue;

                        l_GobEntry = l_CurrentEntry->GameObjects[m_Manager->GetGarrisonFactionIndex()];

                        if (time(0) > l_Building.TimeBuiltEnd)
                            l_SpanwActivateGob  = true;

                        break;
                    }
                }
            }
        }

        if (l_GobEntry != 0)
        {
            GarrBuildingEntry const* l_BuildingEntry = sGarrBuildingStore.LookupEntry(l_Building.BuildingID);
            GameObject* l_Gob = m_Owner->SummonGameObject(l_GobEntry, l_PlotInfo.X, l_PlotInfo.Y, l_PlotInfo.Z, l_PlotInfo.O, 0, 0, 0, 0, 0, 0, 0, 255, 0, true);

            if (l_Gob)
            {
                m_PlotsGob[p_PlotInstanceID] = l_Gob->GetGUID();

                if (m_PlotsGameObjects[p_PlotInstanceID].size() != 0 || m_PlotsCreatures[p_PlotInstanceID].size() != 0)
                {
                    for (uint32 l_I = 0; l_I < m_PlotsCreatures[p_PlotInstanceID].size(); ++l_I)
                    {
                        Creature* l_Crea = HashMapHolder<Creature>::Find(m_PlotsCreatures[p_PlotInstanceID][l_I]);

                        if (l_Crea)
                        {
                            l_Crea->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_HIDE_MODEL);
                            l_Crea->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);

                            if (l_Crea->AI())
                                l_Crea->AI()->SetData(CreatureAIDataIDs::DespawnData, 0);

                            UpdateData l_UpdateData(m_Owner->GetMapId());
                            WorldPacket l_UpdatePacket;

                            l_Crea->BuildValuesUpdateBlockForPlayer(&l_UpdateData, m_Owner);

                            if (l_UpdateData.HasData())
                            {
                                if (l_UpdateData.BuildPacket(&l_UpdatePacket))
                                    m_Owner->SendDirectMessage(&l_UpdatePacket);
                            }

                            l_Crea->DestroyForNearbyPlayers();
                            l_Crea->AddObjectToRemoveList();
                        }
                    }

                    m_PlotsCreatures[p_PlotInstanceID].clear();

                    for (uint32 l_I = 0; l_I < m_PlotsGameObjects[p_PlotInstanceID].size(); ++l_I)
                    {
                        GameObject* l_Gob = HashMapHolder<GameObject>::Find(m_PlotsGameObjects[p_PlotInstanceID][l_I]);

                        if (l_Gob)
                        {
                            m_Owner->SendObjectDeSpawnAnim(m_PlotsGameObjects[p_PlotInstanceID][l_I]);
                            l_Gob->DestroyForNearbyPlayers();
                            l_Gob->AddObjectToRemoveList();
                        }
                    }

                    m_PlotsGameObjects[p_PlotInstanceID].clear();
                }

                G3D::Vector3 l_NonRotatedPosition;
                {
                    G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
                    l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_PlotInfo.O);

                    /// transform plot coord
                    l_NonRotatedPosition = l_Mat * G3D::Vector3(l_PlotInfo.X, l_PlotInfo.Y, l_PlotInfo.Z);
                }

                std::vector<GarrisonPlotBuildingContent> l_Contents;

                if (l_IsPlotBuilding)
                    l_Contents = sObjectMgr->GetGarrisonPlotBuildingContent(GetPlotType(p_PlotInstanceID), m_Manager->GetGarrisonFactionIndex());
                else if ((l_Building.Active ||
                         (l_BuildingEntry && (l_BuildingEntry->Type == Building::Type::Mine
                                           || l_BuildingEntry->Type == Building::Type::PetMenagerie
                                           || l_BuildingEntry->Type == Building::Type::Fishing
                                           || l_BuildingEntry->Type == Building::Type::Farm))
                         )
                        && l_Building.BuildingID)
                {
                    uint32 l_BuildingID = l_Building.BuildingID;

                    if (!l_Building.Active && l_BuildingEntry->Level > 1 &&
                        (l_BuildingEntry->Type == Building::Type::Mine ||
                         l_BuildingEntry->Type == Building::Type::Farm ||
                         l_BuildingEntry->Type == Building::Type::Fishing ||
                         l_BuildingEntry->Type == Building::Type::PetMenagerie))
                    {
                        uint32 l_TargetLevel = l_BuildingEntry->Level - 1;

                        for (uint32 l_I = 0; l_I < sGarrBuildingStore.GetNumRows(); ++l_I)
                        {
                            GarrBuildingEntry const* l_CurrentEntry = sGarrBuildingStore.LookupEntry(l_I);

                            if (!l_CurrentEntry || l_CurrentEntry->Type != l_BuildingEntry->Type || l_CurrentEntry->Level != l_TargetLevel)
                                continue;

                            l_BuildingID = l_CurrentEntry->ID;
                            break;
                        }
                    }

                    l_Contents = sObjectMgr->GetGarrisonPlotBuildingContent(-(int32)l_BuildingID, m_Manager->GetGarrisonFactionIndex());
                }

                for (uint32 l_I = 0; l_I < l_Contents.size(); ++l_I)
                {
                    if (l_IsPlotBuilding && l_Contents[l_I].PlotTypeOrBuilding < 0)
                        continue;

                    if (!l_IsPlotBuilding)
                    {
                        if (!l_Building.BuildingID)
                            continue;

                        int32 l_NegPlotTypeOrBuilding = -l_Contents[l_I].PlotTypeOrBuilding;

                        if (l_Building.Active && l_NegPlotTypeOrBuilding != l_Building.BuildingID) ///< Comparison of integers of different signs: 'const uint32' (aka 'const unsigned int') and 'const int32' (aka 'const int')
                            continue;
                    }

                    G3D::Vector3 l_Position = G3D::Vector3(l_Contents[l_I].X, l_Contents[l_I].Y, 0);

                    G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
                    l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), l_PlotInfo.O);

                    l_Position.x += l_NonRotatedPosition.x;
                    l_Position.y += l_NonRotatedPosition.y;

                    l_Position = l_Mat * l_Position;

                    l_Position.z = l_Contents[l_I].Z + l_PlotInfo.Z;

                    if (l_Contents[l_I].CreatureOrGob > 0)
                    {
                        Creature* l_Creature = m_Owner->SummonCreature(l_Contents[l_I].CreatureOrGob, l_Position.x, l_Position.y, l_Position.z, l_Contents[l_I].O + l_PlotInfo.O, TEMPSUMMON_MANUAL_DESPAWN);

                        if (l_Creature)
                        {
                            m_PlotsCreatures[p_PlotInstanceID].push_back(l_Creature->GetGUID());

                            if (l_Creature->AI())
                            {
                                if (l_IsPlotBuilding)
                                    l_Creature->AI()->SetData(CreatureAIDataIDs::Builder, 1);
                                else
                                {
                                    l_Creature->AI()->SetGUID(m_Owner->GetGUID(), CreatureAIDataIDs::OwnerGuid); ///< Value first, data index next
                                    l_Creature->AI()->SetData(CreatureAIDataIDs::BuildingID,    -l_Contents[l_I].PlotTypeOrBuilding);
                                    l_Creature->AI()->SetData(CreatureAIDataIDs::PlotInstanceID, p_PlotInstanceID | (GetGarrisonSiteLevelEntry()->SiteLevelID << 16));
                                }
                            }
                        }
                    }
                    else
                    {
                        GameObject* l_Cosmetic = m_Owner->SummonGameObject(-l_Contents[l_I].CreatureOrGob, l_Position.x, l_Position.y, l_Position.z, l_Contents[l_I].O + l_PlotInfo.O, 0, 0, 0, 0, 0);

                        if (l_Cosmetic)
                        {
                            m_PlotsGameObjects[p_PlotInstanceID].push_back(l_Cosmetic->GetGUID());

                            if (l_Cosmetic->GetGoType() == GAMEOBJECT_TYPE_GARRISON_SHIPMENT)
                                m_PlotsWorkOrderGob[p_PlotInstanceID] = l_Cosmetic->GetGUID();
                        }
                    }
                }

                if (m_PlotsActivateGob[p_PlotInstanceID] != 0)
                {
                    GameObject* l_Gob = HashMapHolder<GameObject>::Find(m_PlotsActivateGob[p_PlotInstanceID]);

                    if (l_Gob)
                    {
                        m_Owner->SendObjectDeSpawnAnim(m_PlotsActivateGob[p_PlotInstanceID]);
                        l_Gob->DestroyForNearbyPlayers();
                        l_Gob->AddObjectToRemoveList();
                    }
                }

                if (l_SpanwActivateGob)
                {
                    G3D::Vector3 l_FinalPosition;
                    float l_FinalOrientation = l_PlotInfo.O;

                    if (   l_BuildingEntry->Type == Building::Type::Farm
                        || l_BuildingEntry->Type == Building::Type::Mine
                        || l_BuildingEntry->Type == Building::Type::PetMenagerie
                        || l_BuildingEntry->Type == Building::Type::Fishing)
                    {
                        uint32 l_Lvl = GetGarrisonSiteLevelEntry()->Level;

                        if (m_Manager->GetGarrisonFactionIndex() == MS::Garrison::FactionIndex::Alliance)
                        {
                            switch (l_BuildingEntry->Type)
                            {
                                case Building::Type::Farm:
                                    l_FinalPosition     = (l_Lvl == 2 ? G3D::Vector3(1859.0985f, 155.4274f, 79.0399f) : G3D::Vector3(1855.8151f, 151.5068f, 78.4132f));
                                    l_FinalOrientation  = (l_Lvl == 2 ? 0.956567f : 0.857591f);
                                    break;
                                case Building::Type::Mine:
                                    l_FinalPosition     = (l_Lvl == 2 ? G3D::Vector3(1898.2411f,  89.8438f, 83.5268f) : G3D::Vector3(1898.6614f,  88.5848f, 83.5269f));
                                    l_FinalOrientation  = (l_Lvl == 2 ? 0.673824f : 0.555211f);
                                    break;
                                case Building::Type::Fishing:
                                    l_FinalPosition     = (l_Lvl == 2 ? G3D::Vector3(2010.6321f, 166.1842f, 83.5260f) : G3D::Vector3(2013.2568f, 166.8641f, 83.7605f));
                                    l_FinalOrientation  = (l_Lvl == 2 ? 3.756512f : 3.879798f);
                                    break;
                                case Building::Type::PetMenagerie:    ///< Only level 3 garrison
                                    l_FinalPosition     = G3D::Vector3(1909.9861f, 328.0322f, 88.9653f);
                                    l_FinalOrientation  = 5.132511f;
                                    break;
                            }
                        }
                        else        ///< FactionIndex::Horde
                        {
                            switch (l_BuildingEntry->Type)
                            {
                                case Building::Type::Farm:
                                    l_FinalPosition     = (l_Lvl == 2 ? G3D::Vector3(5433.7769f, 4574.0503f, 136.0184f) : G3D::Vector3(5431.2168f, 4573.4658f, 136.1743f));
                                    l_FinalOrientation  = (l_Lvl == 2 ? 5.840217f : 5.998074f);
                                    break;
                                case Building::Type::Mine:
                                    l_FinalPosition     = (l_Lvl == 2 ? G3D::Vector3(5476.3716f, 4446.7773f, 144.4951f) : G3D::Vector3(5474.9707f, 4443.2588f, 144.6435f));
                                    l_FinalOrientation  = (l_Lvl == 2 ? 0.955031f : 0.99013f);
                                    break;
                                case Building::Type::Fishing:
                                    l_FinalPosition     = (l_Lvl == 2 ? G3D::Vector3(5476.4160f, 4613.2881f, 134.4511f) : G3D::Vector3(5478.0010f, 4614.4854f, 134.4501f));
                                    l_FinalOrientation  = (l_Lvl == 2 ? 5.023405f : 5.306918f);
                                    break;
                                case Building::Type::PetMenagerie:    ///< Only level 3 garrison
                                    l_FinalPosition     = G3D::Vector3(5620.6782f, 4649.7178f, 142.2780f);
                                    l_FinalOrientation  = 4.230919f;
                                    break;
                            }
                        }
                    }
                    else
                    {
                        /// For this part we use an matrix to transform plot coord in, order to get the position without the rotation
                        /// Once we have the "non rotated" position, we compute activation game object position in a 2 dimensional system
                        /// And after we reapply the rotation on coords to transform and get the correct final position
                        G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
                        l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_PlotInfo.O);

                        /// transform plot coord
                        G3D::Vector3 l_NonRotatedPosition = l_Mat * G3D::Vector3(l_PlotInfo.X, l_PlotInfo.Y, l_PlotInfo.Z);

                        GameObjectDisplayInfoEntry const* l_GobDispInfo = sGameObjectDisplayInfoStore.LookupEntry(l_Gob->GetDisplayId());

                        /// Work with plot AABB
                        if (l_GobDispInfo)
                        {
                            /// Get AABB on X axis
                            float l_XAxisSize = fabs(l_GobDispInfo->maxX - l_GobDispInfo->minX) * l_Gob->GetFloatValue(OBJECT_FIELD_SCALE);

                            /// We use a "diminish return" on box size for big plots
                            l_NonRotatedPosition.x += l_XAxisSize * (gGarrisonBuildingPlotAABBDiminishReturnFactor[GetPlotType(p_PlotInstanceID) + (m_Manager->GetGarrisonFactionIndex() * PlotTypes::Max)] / l_XAxisSize);
                        }

                        l_Mat = G3D::Matrix3::identity();
                        l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), l_PlotInfo.O);

                        /// Reapply the rotation on coords
                        l_FinalPosition = l_Mat * l_NonRotatedPosition;
                    }

                    m_Owner->UpdateCriteria(CRITERIA_TYPE_UPGRADE_GARRISON_BUILDING, l_BuildingEntry->ID, l_BuildingEntry->Level);

                    uint32 l_AnimProgress   = 0;
                    uint32 l_Health         = 255;

                    GameObject* l_ActivationGob = m_Owner->SummonGameObject(gGarrisonBuildingActivationGameObject[m_Manager->GetGarrisonFactionIndex()], l_FinalPosition.x, l_FinalPosition.y, l_FinalPosition.z, l_FinalOrientation, 0, 0, 0, 0, 0, 0, 0, l_AnimProgress, l_Health);

                    if (l_ActivationGob)
                    {
                        m_PlotsActivateGob[p_PlotInstanceID] = l_ActivationGob->GetGUID();

                        l_ActivationGob->SendGameObjectActivateAnimKit(1696);
                    }
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update garrison stats
    void GarrisonDraenor::UpdateStats()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    bool GarrisonDraenor::CanRecruitFollower() const
    {
        return m_CanRecruitFollower;
    }

    void GarrisonDraenor::SetCanRecruitFollower(bool p_Apply)
    {
        m_CanRecruitFollower = p_Apply;
    }

    /// Assign a follower to a building
    void GarrisonDraenor::AssignFollowerToBuilding(uint64 p_FollowerDBID, uint32 p_PlotInstanceID)
    {
        GarrisonFollower* l_Follower = m_Manager->GetFollowerWithDatabaseID(p_FollowerDBID);

        if (l_Follower == nullptr)
            return;

        GarrisonBuilding* l_Building = GetBuildingObject(p_PlotInstanceID);

        if (l_Building == nullptr)
        {
            bool l_NeedSave = false;
            if (p_PlotInstanceID == 0)
            {
                for (GarrisonBuilding& l_Building : m_Buildings)
                {
                    if (l_Building.FollowerAssigned == p_FollowerDBID)
                    {
                        l_Building.FollowerAssigned = 0;
                        break;
                    }
                }

                l_NeedSave = true;
            }
            if (l_Follower->CurrentBuildingID != 0)
            {
                l_Follower->CurrentBuildingID = 0;
                l_NeedSave = true;
            }

            if (l_NeedSave)
                m_Manager->Save();

            UpdatePlot(p_PlotInstanceID);

            return;
        }

        l_Building->FollowerAssigned = p_FollowerDBID;
        l_Follower->CurrentBuildingID = l_Building->BuildingID;

        m_Manager->Save();
        UpdatePlot(p_PlotInstanceID);
    }

    /// Returns if the specific follower type can be renamed
    bool GarrisonDraenor::CanRenameFollowerType(uint32 p_FollowerType) const
    {
        GarrFollowerTypeEntry const* l_Entry = sGarrFollowerTypeStore.LookupEntry(p_FollowerType);
        return l_Entry ? l_Entry->Flags & FollowerTypeFlags::CanRenameFollower : false;
    }
    /// Renames the specified follower
    bool GarrisonDraenor::RenameFollower(uint32 p_DatabaseID, std::string p_FollowerName)
    {
        GarrisonFollower * l_Follower = m_Manager->GetFollowerWithDatabaseID(p_DatabaseID);

        if (!l_Follower)
            return false;

        if (!CanRenameFollowerType(l_Follower->GetEntry()->Type))
            return false;

        l_Follower->ShipName = p_FollowerName;
        l_Follower->SendFollowerUpdate(m_Owner);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update building
    void GarrisonDraenor::UpdateBuildings()
    {
        /// Update building in construction
        for (uint32 l_I = 0; l_I < m_Buildings.size(); ++l_I)
        {
            GarrisonBuilding* l_Building = &m_Buildings[l_I];

            if (!l_Building->Active && !l_Building->BuiltNotified && time(0) > l_Building->TimeBuiltEnd)
            {
                l_Building->BuiltNotified = true;

                /// Nothing more needed, client auto deduce notification
                UpdatePlot(l_Building->PlotInstanceID);
            }
        }

        if (!m_Owner->IsInDraenorGarrison())
            return;

        for (uint32 l_PlotI = 0; l_PlotI < m_Plots.size(); ++l_PlotI)
        {
            uint32 l_PlotInstanceID = m_Plots[l_PlotI].PlotInstanceID;

            if (m_PlotsWorkOrderGob[l_PlotInstanceID] == 0)
                continue;

            GameObject* l_WorkOrderGameObject = HashMapHolder<GameObject>::Find(m_PlotsWorkOrderGob[l_PlotInstanceID]);

            if (!l_WorkOrderGameObject)
                continue;

            std::vector<GarrisonWorkOrder*> l_PlotWorkOrder;
            auto l_WorkOrders = m_Manager->GetWorkOrders();
            for (uint32 l_OrderI = 0; l_OrderI < l_WorkOrders.size(); ++l_OrderI)
            {
                if (l_WorkOrders[l_OrderI].PlotInstanceID == l_PlotInstanceID)
                    l_PlotWorkOrder.push_back(&l_WorkOrders[l_OrderI]);
            }

            if (l_PlotWorkOrder.size() > 0)
            {
                CharShipmentEntry const* l_ShipmentEntry = sCharShipmentStore.LookupEntry(l_PlotWorkOrder[0]->ShipmentID);

                if (l_ShipmentEntry == nullptr)
                    continue;

                CharShipmentContainerEntry const* l_ShipmentContainerEntry = sCharShipmentContainerStore.LookupEntry(l_ShipmentEntry->ShipmentContainerID);

                if (l_ShipmentContainerEntry == nullptr)
                    continue;

                bool l_Complete = false;
                uint32 l_GobDisplayID = l_WorkOrderGameObject->GetGOInfo() ? l_WorkOrderGameObject->GetGOInfo()->displayId : 0;
                uint32 l_CurrentTimeStamp = time(0);
                uint32 l_ShipmentsSize = (uint32)l_PlotWorkOrder.size();

                for (uint32 l_OrderI = 0; l_OrderI < l_ShipmentsSize; ++l_OrderI)
                {
                    if (l_PlotWorkOrder[l_OrderI]->CompleteTime <= l_CurrentTimeStamp)
                        l_Complete = true;
                }

                if (l_ShipmentsSize < l_ShipmentContainerEntry->ShipmentAmountNeeded[0])
                    l_GobDisplayID = l_ShipmentContainerEntry->OverrideDisplayID1;
                else
                {
                    for (int8 l_Itr = 1; l_Itr >= 0; --l_Itr)
                    {
                        if (l_ShipmentsSize >= l_ShipmentContainerEntry->ShipmentAmountNeeded[l_Itr])
                            l_GobDisplayID = l_ShipmentContainerEntry->OverrideIfAmountMet[l_Itr];
                    }
                }

                if (l_Complete)
                {
                    l_WorkOrderGameObject->SetDisplayId(l_GobDisplayID);
                    l_WorkOrderGameObject->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_ACTIVATED);
                }
                else
                {
                    if (l_ShipmentContainerEntry->OverrideDisplayIfNotNull) ///< Shipyards related
                        l_WorkOrderGameObject->SetDisplayId(l_ShipmentContainerEntry->OverrideDisplayIfNotNull);
                    else
                    {
                        if (GameObjectTemplate const* l_GobTemplate = l_WorkOrderGameObject->GetGOInfo())
                        {
                            if (!l_GobTemplate->garrisonShipment.LargeAOI) ///< Shipyard ?
                                l_WorkOrderGameObject->SetDisplayId(l_GobTemplate->displayId);
                        }
                    }

                    l_WorkOrderGameObject->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_ACTIVATED);
                }
            }
            else
            {
                if (GameObjectTemplate const* l_GobTemplate = l_WorkOrderGameObject->GetGOInfo())
                {
                    if (!l_GobTemplate->garrisonShipment.LargeAOI) ///< Shipyard ?
                        l_WorkOrderGameObject->SetDisplayId(l_GobTemplate->displayId);
                }

                l_WorkOrderGameObject->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_ACTIVATED);
            }
        }
    }

    /// Update cache
    void GarrisonDraenor::UpdateCache()
    {
        if (!m_Owner->IsInDraenorGarrison())
            return;

        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        /// Update garrison cache
        if (m_CacheGameObjectGUID && HashMapHolder<GameObject>::Find(m_CacheGameObjectGUID) == nullptr)
            m_CacheGameObjectGUID = 0;

        if (!l_GarrisonScript || !l_GarrisonScript->CanUseDraenorGarrisonCache(m_Owner))
        {
            if (m_CacheGameObjectGUID != 0)
            {
                GameObject* l_Cache = HashMapHolder<GameObject>::Find(m_CacheGameObjectGUID);

                if (l_Cache)
                {
                    l_Cache->DestroyForNearbyPlayers();
                    l_Cache->AddObjectToRemoveList();
                }

                m_CacheGameObjectGUID = 0;
            }

            return;
        }

        uint32 l_NumRessourceGenerated = std::min((uint32)((time(0) - m_CacheLastUsage) / Globals::CacheTokenGenerateTime), (uint32)Globals::CacheMaxToken);

        if (!m_CacheGameObjectGUID)
        {
            m_CacheLastTokenAmount = l_NumRessourceGenerated;
            m_Owner->SendUpdateWorldState(WorldStates::CacheNumToken, l_NumRessourceGenerated);

            if (l_NumRessourceGenerated >= Globals::CacheMinToken)
            {
                /// Get display ID
                uint32 l_DisplayIDOffset    = l_NumRessourceGenerated == Globals::CacheMaxToken ? 2 : ((l_NumRessourceGenerated > Globals::CacheHeftyToken) ? 1 : 0);
                const uint32 & l_DisplayID  = gGarrisonCacheGameObjectID[(m_Manager->GetGarrisonFactionIndex() * 3) + l_DisplayIDOffset];

                /// Destroy old cache if exist
                GameObject* l_Cache = HashMapHolder<GameObject>::Find(m_CacheGameObjectGUID);

                if (l_Cache)
                {
                    l_Cache->DestroyForNearbyPlayers();
                    l_Cache->AddObjectToRemoveList();
                }

                m_CacheGameObjectGUID = 0;

                /// Create new one
                if (m_Owner->IsInDraenorGarrison())
                {
                    /// Extract new location
                    const GarrisonCacheInfoLocation& l_Location = gGarrisonCacheInfoLocation[(m_Manager->GetGarrisonFactionIndex() * Globals::MaxLevel) + (m_GarrisonLevel - 1)];
                    l_Cache = m_Owner->SummonGameObject(l_DisplayID, l_Location.X, l_Location.Y, l_Location.Z, l_Location.O, 0, 0, 0, 0, 0);

                    if (l_Cache)
                        m_CacheGameObjectGUID = l_Cache->GetGUID();
                }

            }
        }

        if (m_CacheGameObjectGUID && l_NumRessourceGenerated < Globals::CacheMinToken)
        {
            GameObject* l_Cache = HashMapHolder<GameObject>::Find(m_CacheGameObjectGUID);

            if (l_Cache)
            {
                l_Cache->DestroyForNearbyPlayers();
                l_Cache->AddObjectToRemoveList();
            }

            m_CacheGameObjectGUID = 0;
        }
    }
    /// Update garrison ability
    void GarrisonDraenor::UpdateGarrisonAbility()
    {
        if (!m_Owner->IsInWorld())
            return;

        uint32 l_AbilityOverrideSpellID = 0;

        switch (m_Manager->GetGarrisonFactionIndex())
        {
            case MS::Garrison::FactionIndex::Alliance:
                l_AbilityOverrideSpellID = GARRISON_SPELL_GARR_ABILITY_ALLIANCE_BASE;
                break;

            case MS::Garrison::FactionIndex::Horde:
                l_AbilityOverrideSpellID = GARRISON_SPELL_GARR_ABILITY_HORDE_BASE;
                break;

            case MS::Garrison::FactionIndex::Max:
            default:
                ASSERT(false);
                break;

        }

        bool l_Cond1 = m_Owner->IsInDraenorGarrison() || m_Owner->GetMapId() == Globals::BaseMap;
        bool l_Cond2 = GetGarrisonSiteLevelEntry()->Level == 1 ? (m_Owner->IsQuestRewarded(Quests::Alliance_BuildYourBarracks) || m_Owner->IsQuestRewarded(Quests::Horde_BuildYourBarracks)) : true;

        if (l_Cond1 && l_Cond2)
        {
            if (!m_Owner->HasAura(l_AbilityOverrideSpellID))
                m_Owner->AddAura(l_AbilityOverrideSpellID, m_Owner);
        }
        else
        {
            if (m_Owner->HasAura(l_AbilityOverrideSpellID))
                m_Owner->RemoveAura(l_AbilityOverrideSpellID);
        }
    }









    uint32 GarrisonDraenor::GetFollowersCountBarracksBonus()
    {
        return HasActiveBuilding(Building::ID::Barracks_Barracks_Level3) ? 5 : 0;
    }

    void GarrisonDraenor::SendPacketToOwner(WorldPacket* p_Data)
    {
        m_Owner->SendDirectMessage(p_Data);
    }

    bool GarrisonDraenor::HasCrewAbility(GarrisonFollower const& p_Follower) const
    {
        for (auto l_Ability : p_Follower.Abilities)
        {
            GarrAbilityEntry const* l_GarrAbility = sGarrAbilityStore.LookupEntry(l_Ability);
            if (!l_GarrAbility)
                continue;

            if (l_GarrAbility->Flags == 1 || l_GarrAbility->Flags == 9 || l_GarrAbility->Flags == 3 || l_GarrAbility->Flags == 5)
                return true;
        }

        return false;
    }

    uint32 GarrisonDraenor::GenerateCrewAbilityIdForShip(GarrisonFollower const& p_Follower)
    {
        if (p_Follower.GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDShipyard)
            return 0;

        if (HasCrewAbility(p_Follower))
            return 0;

        uint32 l_Type = m_Manager->GetGarrisonFactionIndex() == MS::Garrison::FactionIndex::Alliance ? 9 : 5;

        std::vector<uint32> l_PossibleEntiers;

        for (uint32 l_ID = 0; l_ID < sGarrAbilityStore.GetNumRows(); ++l_ID)
        {
            GarrAbilityEntry const* l_Entry = sGarrAbilityStore.LookupEntry(l_ID);

            if (!l_Entry)
                continue;

            /// Neutral == 1
            if ((l_Entry->Flags == l_Type || l_Entry->Flags == 1) && l_Entry->FollowerType == eGarrisonFollowerTypeID::FollowerTypeIDShipyard)
                l_PossibleEntiers.push_back(l_ID);
        }

        if (!l_PossibleEntiers.size())
            return 0;

        return l_PossibleEntiers[urand(0, l_PossibleEntiers.size() - 1)];
    }


    bool GarrisonDraenor::HasShipyard() const
    {
        return GetShipyardLevel() != 0;
    }

    uint32 GarrisonDraenor::GetShipyardLevel() const
    {
        uint32 l_Level = 0;

        auto l_Building = std::find_if(m_Buildings.begin(), m_Buildings.end(), [&l_Level](GarrisonBuilding l_Building) -> bool ///< l_Building is unused
        {
            auto l_GarrBuildingEntry = sGarrBuildingStore.LookupEntry(l_Building.BuildingID);

            if (l_GarrBuildingEntry)
            {
                if (l_GarrBuildingEntry->Type == Globals::ShipyardBuildingType)
                {
                    l_Level = l_GarrBuildingEntry->Level;
                    return true;
                }
                return false;
            }
            return false;
        });

        return l_Level;
    }

    uint32 GarrisonDraenor::GetShipyardMapId() const
    {
        return !HasShipyard() || !m_Owner ? -1 : m_Manager->GetGarrisonFactionIndex() == MS::Garrison::FactionIndex::Alliance ? ShipyardMapId::Alliance : ShipyardMapId::Horde;
    }

    void GarrisonDraenor::GetShipyardUIWorldMapAreaIDSwaps(std::set<uint32>& p_UIWorldMapAreaSwaps) const
    {
        switch (m_Manager->GetGarrisonFactionIndex())
        {
            case MS::Garrison::FactionIndex::Alliance:
            {
                p_UIWorldMapAreaSwaps.emplace(ShipyardUIWorldMapAreaSwaps::Part1);
                p_UIWorldMapAreaSwaps.emplace(ShipyardUIWorldMapAreaSwaps::Part2);
                p_UIWorldMapAreaSwaps.emplace(ShipyardUIWorldMapAreaSwaps::Part3);
                break;
            }
            case MS::Garrison::FactionIndex::Horde:
            default:
                break;
        }
    }

    bool GarrisonDraenor::CreateShipyardBySpell()
    {
        if (HasShipyard())
            return false;

        if (!GetGarrisonSiteLevelEntry() || GetGarrisonSiteLevelEntry()->Level != 3)
            return false;

        WorldPacket l_Data(SMSG_GARRISON_PLACE_BUILDING_RESULT, 26);
        l_Data << uint32(GarrisonType::GarrisonDraenor);
        l_Data << uint32(PurchaseBuildingResults::Ok);

        GarrisonBuilding l_Building = PurchaseBuilding(Globals::ShipyardBuildingID, Globals::ShipyardPlotID);

        l_Data << uint32(l_Building.PlotInstanceID);
        l_Data << uint32(l_Building.BuildingID);
        l_Data << uint32(l_Building.TimeBuiltStart);
        l_Data << uint32(l_Building.SpecID);
        l_Data << uint32(l_Building.TimeBuiltEnd);
        l_Data.WriteBit(l_Building.Active);
        l_Data.FlushBits();

        l_Data.WriteBit(true);                      ///< Play activation cinematic
        l_Data.FlushBits();

        SendPacketToOwner(&l_Data);

        if (m_Owner->GetAreaId() == gGarrisonShipyardAreaID[m_Manager->GetGarrisonFactionIndex()])
            m_Owner->_SetInShipyard();

        return true;
    }

    bool GarrisonDraenor::CheckGarrisonStablesQuestsConditions(uint32 p_QuestID, Player* p_Player)
    {
        using namespace MS::Garrison::GDraenor::StablesData::Alliance;
        using namespace MS::Garrison::GDraenor::StablesData::Horde;

        if (std::find(FannyQuestGiver::g_BoarQuests.begin(), FannyQuestGiver::g_BoarQuests.end(), p_QuestID) != FannyQuestGiver::g_BoarQuests.end() ||
            std::find(TormakQuestGiver::g_BoarQuests.begin(), TormakQuestGiver::g_BoarQuests.end(), p_QuestID) != TormakQuestGiver::g_BoarQuests.end())
        {
            if (!p_Player->HasAura(MS::Garrison::GDraenor::StablesData::TrainingMountsAuras::RockstuckTrainingMountAura))
                return false;
        }
        else if (std::find(FannyQuestGiver::g_ClefthoofQuests.begin(), FannyQuestGiver::g_ClefthoofQuests.end(), p_QuestID) != FannyQuestGiver::g_ClefthoofQuests.end() ||
                 std::find(TormakQuestGiver::g_ClefthoofQuests.begin(), TormakQuestGiver::g_ClefthoofQuests.end(), p_QuestID) != TormakQuestGiver::g_ClefthoofQuests.end())
        {
            if (!p_Player->HasAura(MS::Garrison::GDraenor::StablesData::TrainingMountsAuras::IcehoofTrainingMountAura))
                return false;
        }
        else if (std::find(FannyQuestGiver::g_ElekkQuests.begin(), FannyQuestGiver::g_ElekkQuests.end(), p_QuestID) != FannyQuestGiver::g_ElekkQuests.end() ||
                 std::find(TormakQuestGiver::g_ElekkQuests.begin(), TormakQuestGiver::g_ElekkQuests.end(), p_QuestID) != TormakQuestGiver::g_ElekkQuests.end())
        {
            if (!p_Player->HasAura(MS::Garrison::GDraenor::StablesData::TrainingMountsAuras::MeadowstomperTrainingMountAura))
                return false;
        }
        else if (std::find(KeeganQuestGiver::g_RiverbeastQuests.begin(), KeeganQuestGiver::g_RiverbeastQuests.end(), p_QuestID) != KeeganQuestGiver::g_RiverbeastQuests.end() ||
                 std::find(SagePalunaQuestGiver::g_RiverbeastQuests.begin(), SagePalunaQuestGiver::g_RiverbeastQuests.end(), p_QuestID) != SagePalunaQuestGiver::g_RiverbeastQuests.end())
        {
            if (!p_Player->HasAura(MS::Garrison::GDraenor::StablesData::TrainingMountsAuras::RiverwallowTrainingMountAura))
                return false;
        }
        else if (std::find(KeeganQuestGiver::g_TalbukQuests.begin(), KeeganQuestGiver::g_TalbukQuests.end(), p_QuestID) != KeeganQuestGiver::g_TalbukQuests.end() ||
                 std::find(SagePalunaQuestGiver::g_TalbukQuests.begin(), SagePalunaQuestGiver::g_TalbukQuests.end(), p_QuestID) != SagePalunaQuestGiver::g_TalbukQuests.end())
        {
            if (!p_Player->HasAura(MS::Garrison::GDraenor::StablesData::TrainingMountsAuras::SilverpeltTrainingMountAura))
                return false;
        }
        else if (std::find(KeeganQuestGiver::g_WolfQuests.begin(), KeeganQuestGiver::g_WolfQuests.end(), p_QuestID) != KeeganQuestGiver::g_WolfQuests.end() ||
                 std::find(SagePalunaQuestGiver::g_WolfQuests.begin(), SagePalunaQuestGiver::g_WolfQuests.end(), p_QuestID) != SagePalunaQuestGiver::g_WolfQuests.end())
        {
            if (!p_Player->HasAura(MS::Garrison::GDraenor::StablesData::TrainingMountsAuras::SnarlerTrainingMountAura))
                return false;
        }

        return true;
    }

    bool GarrisonDraenor::IsTrainingMount()
    {
        return m_Owner->HasAura(StablesData::TrainingMountsAuras::RockstuckTrainingMountAura)
            || m_Owner->HasAura(StablesData::TrainingMountsAuras::IcehoofTrainingMountAura)
            || m_Owner->HasAura(StablesData::TrainingMountsAuras::MeadowstomperTrainingMountAura)
            || m_Owner->HasAura(StablesData::TrainingMountsAuras::RiverwallowTrainingMountAura)
            || m_Owner->HasAura(StablesData::TrainingMountsAuras::SilverpeltTrainingMountAura)
            || m_Owner->HasAura(StablesData::TrainingMountsAuras::SnarlerTrainingMountAura);
    }

    void GarrisonDraenor::HandleStablesAuraBonuses(bool p_Remove /*= false*/)
    {
        if (m_Owner->GetMapId() != 1116) ///< MapID Draenor
            return;

        using namespace StablesData;

        if (!p_Remove)
        {
            switch (GetBuildingLevel(GetBuildingWithType(Building::Type::Stable)))
            {
                case 1:
                    m_Owner->AddAura(BonusAuras::StablesAuraLevel1, m_Owner);
                    break;
                case 2:
                    m_Owner->AddAura(BonusAuras::StablesAuraLevel2, m_Owner);
                    m_Owner->SetFlag(UNIT_FIELD_FLAGS_3, UNIT_FLAG3_CAN_FIGHT_WITHOUT_DISMOUNT);
                    break;
                case 3:
                    m_Owner->AddAura(BonusAuras::StablesAuraLevel3, m_Owner);
                    m_Owner->SetFlag(UNIT_FIELD_FLAGS_3, UNIT_FLAG3_CAN_FIGHT_WITHOUT_DISMOUNT);
                    break;
            }
        }
        else
        {
            switch (GetBuildingLevel(GetBuildingWithType(Building::Type::Stable)))
            {
                case 1:
                    m_Owner->RemoveAura(BonusAuras::StablesAuraLevel1);
                    break;
                case 2:
                    m_Owner->RemoveAura(BonusAuras::StablesAuraLevel2);
                    m_Owner->RemoveFlag(UNIT_FIELD_FLAGS_3, UNIT_FLAG3_CAN_FIGHT_WITHOUT_DISMOUNT);
                    break;
                case 3:
                    m_Owner->RemoveAura(BonusAuras::StablesAuraLevel3);
                    m_Owner->RemoveFlag(UNIT_FIELD_FLAGS_3, UNIT_FLAG3_CAN_FIGHT_WITHOUT_DISMOUNT);
                    break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////
    //////////////////////// DAILY TAVERN DATA ///////////////////////////
    //////////////////////////////////////////////////////////////////////

    void GarrisonDraenor::AddGarrisonDailyTavernData(uint32 p_Data)
    {
        SetGarrisonDailyTavernData(p_Data);
        m_Owner->SaveToDB(false);
    }

    void GarrisonDraenor::SetGarrisonDailyTavernData(uint32 p_Data)
    {
        GetGarrisonDailyTavernDatas().push_back(p_Data);
    }

    void GarrisonDraenor::CleanGarrisonDailyTavernData()
    {
        GetGarrisonDailyTavernDatas().clear();
    }

    void GarrisonDraenor::ResetGarrisonDailyTavernData()
    {
        if (roll_chance_i(50))
        {
            uint32 l_Entry = TavernDatas::g_QuestGiverEntries[urand(0, TavernDatas::g_QuestGiverEntries.size() - 1)];

            CleanGarrisonDailyTavernData();
            AddGarrisonDailyTavernData(l_Entry);
        }
        else
        {
            uint32 l_FirstEntry = TavernDatas::g_QuestGiverEntries[urand(0, TavernDatas::g_QuestGiverEntries.size() - 1)];
            uint32 l_SecondEntry = 0;

            do
                l_SecondEntry = TavernDatas::g_QuestGiverEntries[urand(0, TavernDatas::g_QuestGiverEntries.size() - 1)];
            while (l_SecondEntry == l_FirstEntry);

            CleanGarrisonDailyTavernData();
            AddGarrisonDailyTavernData(l_FirstEntry);
            AddGarrisonDailyTavernData(l_SecondEntry);
        }
    }

    //////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////
    //////////////////////// WEEKLY TAVERN DATA //////////////////////////
    //////////////////////////////////////////////////////////////////////

    void GarrisonDraenor::AddGarrisonWeeklyTavernData(WeeklyTavernData p_Data)
    {
        SetGarrisonWeeklyTavernData(p_Data);
        m_Owner->SaveToDB(false);
    }

    void GarrisonDraenor::SetGarrisonWeeklyTavernData(WeeklyTavernData p_Data)
    {
        GetGarrisonWeeklyTavernDatas().push_back(p_Data);
    }

    void GarrisonDraenor::CleanGarrisonWeeklyTavernData()
    {
        GetGarrisonWeeklyTavernDatas().clear();
    }

    void GarrisonDraenor::ResetGarrisonWeeklyTavernData()
    {
        CleanGarrisonWeeklyTavernData();

        m_Owner->SetCharacterWorldState(CharacterWorldStates::GarrisonTavernBoolCanRecruitFollower, 1);
        SetCanRecruitFollower(true);
    }

    //////////////////////////////////////////////////////////////////////

    void GarrisonDraenor::ResetGarrisonWorkshopData(Player* p_Player)
    {
        using namespace WorkshopGearworks;

        if (p_Player->GetTeamId() == TEAM_ALLIANCE)
        {
            if (!p_Player->IsQuestRewarded(Quests::Alliance_UnconventionalInventions))
                return;
        }
        else if (p_Player->GetTeamId() == TEAM_HORDE)
        {
            if (!p_Player->IsQuestRewarded(Quests::Horde_UnconventionalInventions))
                return;
        }

        uint32 l_Worldstate = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::GarrisonWorkshopGearworksInvention);
        std::vector<uint32> l_Inventions;

        for (uint32 l_Value : g_FirstLevelInventions)
            l_Inventions.push_back(l_Value);

        if (GetBuildingLevel(GetBuildingWithType(Building::Type::Workshop)) > 1)
        {
            for (uint32 l_Value : g_SecondLevelInventions)
                l_Inventions.push_back(l_Value);
        }

        for (std::vector<uint32>::iterator l_Itr = l_Inventions.begin(); l_Itr != l_Inventions.end();)
        {
            if (std::find(l_Inventions.begin(), l_Inventions.end(), *l_Itr) != l_Inventions.end())
            {
                ++l_Itr;
                continue;
            }

            if (*l_Itr == l_Worldstate)
                l_Itr = l_Inventions.erase(l_Itr);

            if (p_Player->GetTeamId() == TEAM_ALLIANCE && *l_Itr == GobPrototypeMechanoHog)
                l_Itr = l_Inventions.erase(l_Itr);

            if (p_Player->GetTeamId() == TEAM_HORDE && *l_Itr == GobPrototypeMekgineersChopper)
                l_Itr = l_Inventions.erase(l_Itr);
        }

        uint32 l_Entry = l_Inventions[urand(0, l_Inventions.size() - 1)];

        ItemTemplate const* l_ItemProto = sObjectMgr->GetItemTemplate(g_GobItemRelations[l_Entry]);

        if (l_ItemProto == nullptr)
            return;

        p_Player->SetCharacterWorldState(CharacterWorldStates::GarrisonWorkshopGearworksInvention, l_Entry);
        p_Player->SetCharacterWorldState(CharacterWorldStates::GarrisonWorkshopGearworksInventionCharges, l_ItemProto->Spells[0].SpellCharges);
    }

    void GarrisonDraenor::ResetGarrisonTradingPostData(Player* p_Player)
    {
        std::vector<uint32> l_HordeTradersEntries = { 86778, 86777, 86779, 86776, 86683 };
        std::vector<uint32> l_AllianceTradersEntries = { 87203, 87202, 87200, 87201, 87204 };
        std::vector<uint32> l_TradingPostShipments = { 138, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 196 };
        uint32 l_Entry = p_Player->GetTeamId() == TEAM_ALLIANCE ? l_AllianceTradersEntries[urand(0, l_AllianceTradersEntries.size() - 1)] : l_HordeTradersEntries[urand(0, l_HordeTradersEntries.size() - 1)];

        p_Player->SetCharacterWorldState(CharacterWorldStates::GarrisonTradingPostDailyRandomShipment, l_TradingPostShipments[urand(0, l_TradingPostShipments.size() - 1)]);
        p_Player->SetCharacterWorldState(CharacterWorldStates::GarrisonTradingPostDailyRandomTrader, l_Entry);
    }

    std::vector<GarrisonFollower> GarrisonDraenor::GetWeeklyFollowerRecruits(Player* p_Player)
    {
        std::vector<GarrisonFollower> l_Followers;

        std::vector<WeeklyTavernData> l_Data = GetGarrisonWeeklyTavernDatas();

        for (WeeklyTavernData l_Datum : l_Data)
        {
            GarrisonFollower l_Follower = m_Manager->GenerateNewFollowerEntity(l_Datum.FollowerID);

            l_Follower.Abilities.clear();

            for (uint32 l_Ability : l_Datum.Abilities)
                l_Follower.Abilities.push_back(l_Ability);

            l_Followers.push_back(l_Follower);
        }

        return l_Followers;
    }

}   ///< namespace GDraenor
}   ///< namespace Garrison
}   ///< namespace MS
