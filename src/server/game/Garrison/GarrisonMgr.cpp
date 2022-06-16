////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonMgr.hpp"
#include "Player.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "DisableMgr.h"
#include "GarrisonPackets.h"

namespace MS { namespace Garrison
{
    /// Constructor
    Manager::Manager(Player* p_Owner)
        : m_Owner(p_Owner), m_ID(0), m_GarrisonScriptID(0), m_GarrisonDraenor(nullptr), m_GarrisonBrokenIsles(nullptr), m_MissionDistributionLastUpdate(time(nullptr)), m_Loaded(false), m_Requested(false)
    {
        m_NumFollowerActivation[GarrisonType::GarrisonDraenor]      = 1;
        m_NumFollowerActivation[GarrisonType::GarrisonBrokenIsles]  = 1;
        m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonDraenor]        = time(nullptr);
        m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonBrokenIsles]    = time(nullptr);
    }

    /// Destructor
    Manager::~Manager()
    {
        if (m_GarrisonDraenor)
        {
            delete m_GarrisonDraenor;
            m_GarrisonDraenor = nullptr;
        }

        if (m_GarrisonBrokenIsles)
        {
            delete m_GarrisonBrokenIsles;
            m_GarrisonBrokenIsles = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Create the garrison
    void Manager::Create(MS::Garrison::GarrisonType::Type p_Type)
    {
        if (!m_ID)
        {
            m_ID = sObjectMgr->GetNewGarrisonID();

            PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON);

            uint32 l_Index = 0;
            l_Stmt->setUInt32(l_Index++, m_ID);
            l_Stmt->setUInt32(l_Index++, m_Owner->GetGUID());
            l_Stmt->setUInt32(l_Index++, m_NumFollowerActivation[GarrisonType::GarrisonDraenor]);
            l_Stmt->setUInt32(l_Index++, m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonDraenor]);
            l_Stmt->setUInt32(l_Index++, GetMissionStartedTodayCount(GarrisonType::GarrisonDraenor));
            l_Stmt->setUInt32(l_Index++, m_NumFollowerActivation[GarrisonType::GarrisonBrokenIsles]);
            l_Stmt->setUInt32(l_Index++, m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonBrokenIsles]);
            l_Stmt->setUInt32(l_Index++, GetMissionStartedTodayCount(GarrisonType::GarrisonBrokenIsles));

            CharacterDatabase.AsyncQuery(l_Stmt);
        }

        if (p_Type == GarrisonType::GarrisonDraenor && !m_GarrisonDraenor)
        {
            m_GarrisonDraenor = new GDraenor::GarrisonDraenor(this, m_Owner);
            m_GarrisonDraenor->Create();
        }
        else if (p_Type == GarrisonType::GarrisonBrokenIsles && !m_GarrisonBrokenIsles)
        {
            m_GarrisonBrokenIsles = new GBrokenIsles::GarrisonBrokenIsles(this, m_Owner);
            m_GarrisonBrokenIsles->Create();
        }

        /// Force mission distribution update
        m_MissionDistributionLastUpdate = 0;
        m_Loaded = true;

        SendRemoteInformations();
    }

    /// Load
    bool Manager::Load(  PreparedQueryResult p_GarrisonResult,          PreparedQueryResult p_FollowersResult,          PreparedQueryResult p_MissionsResult, PreparedQueryResult p_WorkOrderResult, PreparedQueryResult p_TalentResult,
                         PreparedQueryResult p_DraenorGarrisonResult,   PreparedQueryResult p_DraenorBuildingsResult,
                         PreparedQueryResult p_BrokenIslesGarrisonResult)
    {
        Field* l_Fields = nullptr;

        if (p_GarrisonResult)
        {
            l_Fields = p_GarrisonResult->Fetch();

            m_ID                                                                        = l_Fields[0].GetUInt32();
            m_NumFollowerActivation[GarrisonType::GarrisonDraenor]                      = l_Fields[1].GetUInt32();
            m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonDraenor]        = l_Fields[2].GetUInt32();
            m_MissionsStartedToday[GarrisonType::GarrisonDraenor]                       = l_Fields[3].GetUInt32();
            m_NumFollowerActivation[GarrisonType::GarrisonBrokenIsles]                  = l_Fields[4].GetUInt32();
            m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonBrokenIsles]    = l_Fields[5].GetUInt32();
            m_MissionsStartedToday[GarrisonType::GarrisonBrokenIsles]                   = l_Fields[6].GetUInt32();
        }
        else
        {
            m_Loaded = true;
            return false;
        }

        std::map<uint32, std::set<uint64>> l_FollowerPerMissions;

        if (p_FollowersResult)
        {
            do
            {
                l_Fields = p_FollowersResult->Fetch();

                GarrisonFollower l_Follower;
                l_Follower.DatabaseID = l_Fields[0].GetUInt32();
                l_Follower.FollowerID = l_Fields[1].GetUInt32();
                l_Follower.Level = l_Fields[2].GetUInt32();
                l_Follower.XP = l_Fields[3].GetUInt32();
                l_Follower.Durability = l_Fields[4].GetUInt32();
                l_Follower.Quality = l_Fields[5].GetUInt32();
                l_Follower.ItemLevelArmor = l_Fields[6].GetUInt32();
                l_Follower.ItemLevelWeapon = l_Fields[7].GetUInt32();
                l_Follower.CurrentMissionID = l_Fields[8].GetUInt32();
                l_Follower.CurrentBuildingID = l_Fields[9].GetUInt32();

                l_FollowerPerMissions[l_Follower.CurrentMissionID].insert(l_Follower.DatabaseID);

                std::set<uint32> l_AbilitiesFromSpell;
                if (!l_Fields[10].GetString().empty())
                {
                    Tokenizer l_Abilities(l_Fields[10].GetString(), ' ');

                    for (Tokenizer::const_iterator l_It = l_Abilities.begin(); l_It != l_Abilities.end(); ++l_It)
                    {
                        uint32 l_AbilityID = atol(*l_It);
                        if (sSpellMgr->IsFollowerAbilityFromSpell(l_AbilityID))
                            l_AbilitiesFromSpell.insert(l_AbilityID);
                        else
                            l_Follower.Abilities.push_back(l_AbilityID);
                    }
                }

                if (l_AbilitiesFromSpell.size() <= 3)
                {
                    for (uint32 l_Ability : l_AbilitiesFromSpell)
                        l_Follower.Abilities.push_back(l_Ability);
                }

                l_Follower.ZoneSupportSpellID = l_Fields[11].GetUInt32();
                l_Follower.Flags = l_Fields[12].GetUInt32();
                l_Follower.ShipName = l_Fields[13].GetString();

                if (auto l_Entry = l_Follower.GetEntry())
                    l_Follower.GarrClassSpecID      = m_Owner->getFaction() == FactionIndex::Alliance ? l_Entry->AllianceGarrClassSecID : l_Entry->HordeGarrClassSecID;

                m_Followers.push_back(l_Follower);
                GenerateFollowerAbilities(l_Follower.DatabaseID, false);

            } while (p_FollowersResult->NextRow());
        }

        if (p_MissionsResult)
        {
            do
            {
                l_Fields = p_MissionsResult->Fetch();

                GarrisonMission l_Mission;
                l_Mission.DatabaseID        = l_Fields[0].GetUInt32();
                l_Mission.MissionID         = l_Fields[1].GetUInt32();

                GarrMissionEntry const* l_MissionEntry = sGarrMissionStore.LookupEntry(l_Mission.MissionID);

                l_Mission.OfferTime         = l_Fields[2].GetUInt32();
                l_Mission.OfferMaxDuration  = l_Fields[3].GetUInt32();
                l_Mission.StartTime         = l_Fields[4].GetUInt32();
                l_Mission.TravelDuration    = GetMissionTravelDuration(l_Mission.MissionID);
                l_Mission.State             = (Mission::State)l_Fields[5].GetUInt32();
                l_Mission.ChestChance       = l_Fields[6].GetUInt32();
                l_Mission.Duration          = l_Fields[7].GetUInt32();
                l_Mission.IsLethalToTroops  = l_Fields[8].GetBool();

                if (l_FollowerPerMissions.find(l_Mission.MissionID) != l_FollowerPerMissions.end())
                {
                    if (l_FollowerPerMissions[l_Mission.MissionID].size() > l_MissionEntry->RequiredFollowersCount)
                        continue;
                }

                if (DisableMgr::IsDisabledFor(DISABLE_TYPE_GARRISON_MISSION, l_Mission.MissionID, m_Owner))
                    continue;

                if ((!l_Mission.OfferMaxDuration || (l_Mission.OfferTime + l_Mission.OfferMaxDuration) > time(nullptr)) || l_Mission.State == Mission::State::InProgress)
                    m_Missions.push_back(l_Mission);
                else
                {
                    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_MISSION);
                    l_Stmt->setUInt32(0, l_Mission.DatabaseID);
                    CharacterDatabase.AsyncQuery(l_Stmt);
                }

            } while (p_MissionsResult->NextRow());
        }

        if (p_WorkOrderResult)
        {
            do
            {
                l_Fields = p_WorkOrderResult->Fetch();

                GarrisonWorkOrder l_Order;
                l_Order.DatabaseID        = l_Fields[0].GetUInt32();
                l_Order.PlotInstanceID    = l_Fields[1].GetUInt32();
                l_Order.ShipmentID        = l_Fields[2].GetUInt32();
                l_Order.CreationTime      = l_Fields[3].GetUInt32();
                l_Order.CompleteTime      = l_Fields[4].GetUInt32();

                m_WorkOrders.push_back(l_Order);
            }
            while (p_WorkOrderResult->NextRow());
        }

        if (p_TalentResult)
        {
            bool l_AnythingApplied = false;

            do
            {
                l_Fields = p_TalentResult->Fetch();

                GarrisonTalent l_Talent;
                l_Talent.TalentID           = l_Fields[0].GetUInt32();
                l_Talent.StartResearchTime  = l_Fields[1].GetUInt32();
                l_Talent.Flags              = l_Fields[2].GetUInt32();

                auto l_Entry = sGarrTalentStore.LookupEntry(l_Talent.TalentID);
                if (!l_Entry)
                    continue;

                auto l_Tree = sGarrTalentTreeStore.LookupEntry(l_Entry->TalentTreeID);
                if (!l_Tree)
                    continue;

                l_Talent.GarrisonType = l_Tree->GarrisonType;
                m_Talents.push_back(l_Talent);

                if (!l_Talent.IsBeingResearched())
                {
                    l_AnythingApplied = true;
                    ApplyTalentEffect(l_Entry, true, false);
                }

            } while (p_TalentResult->NextRow());

            if (l_AnythingApplied)
                m_Owner->UpdateObjectVisibility(false);
        }

        /// Force mission distribution update
        m_MissionDistributionLastUpdate = 0;

        CleanupMissions();
        RewardMissingFollowers();
        CleanupOfferMissions();
        UnstuckFollowers();

        if (p_DraenorGarrisonResult && p_DraenorGarrisonResult->GetRowCount() >= 1)
        {
            m_GarrisonDraenor = new GDraenor::GarrisonDraenor(this, m_Owner);

            if (!m_GarrisonDraenor->Load(p_DraenorGarrisonResult, p_DraenorBuildingsResult))
            {
                delete m_GarrisonDraenor;
                m_GarrisonDraenor = nullptr;
            }
        }

        if (p_BrokenIslesGarrisonResult && p_BrokenIslesGarrisonResult->GetRowCount() >= 1)
        {
            m_GarrisonBrokenIsles = new GBrokenIsles::GarrisonBrokenIsles(this, m_Owner);

            if (!m_GarrisonBrokenIsles->Load(p_BrokenIslesGarrisonResult))
            {
                delete m_GarrisonBrokenIsles;
                m_GarrisonBrokenIsles = nullptr;
            }
        }

        m_Loaded = true;

        if (m_Requested)
        {
            m_Requested = false;
            m_Owner->GetSession()->SendGetGarrisonInfoResponse();
        }

        return true;
    }
    /// Save this garrison to DB
    void Manager::Save()
    {
        if (!m_ID)
            return;

        SQLTransaction l_GarrisonTransaction = CharacterDatabase.BeginTransaction();

        uint32 l_Index = 0;
        PreparedStatement* l_GarrStatement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON);
        l_GarrStatement->setUInt32(l_Index++, m_NumFollowerActivation[GarrisonType::GarrisonDraenor]);
        l_GarrStatement->setUInt32(l_Index++, m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonDraenor]);
        l_GarrStatement->setInt32(l_Index++, GetMissionStartedTodayCount(GarrisonType::GarrisonDraenor));
        l_GarrStatement->setUInt32(l_Index++, m_NumFollowerActivation[GarrisonType::GarrisonBrokenIsles]);
        l_GarrStatement->setUInt32(l_Index++, m_NumFollowerActivationRegenTimestamp[GarrisonType::GarrisonBrokenIsles]);
        l_GarrStatement->setInt32(l_Index++, GetMissionStartedTodayCount(GarrisonType::GarrisonBrokenIsles));
        l_GarrStatement->setUInt32(l_Index++, m_ID);

        l_GarrisonTransaction->Append(l_GarrStatement);

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if ((!m_Missions[l_I].OfferMaxDuration || (m_Missions[l_I].OfferTime + m_Missions[l_I].OfferMaxDuration) > time(nullptr)) || m_Missions[l_I].State == Mission::State::InProgress || m_Missions[l_I].State == Mission::State::Completed)
            {
                PreparedStatement* l_MissionStmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON_MISSION);

                l_Index = 0;
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].MissionID);
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].OfferTime);
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].OfferMaxDuration);
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].StartTime);
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].State);
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].ChestChance);
                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].Duration);
                l_MissionStmt->setBool(l_Index++, m_Missions[l_I].IsLethalToTroops);

                l_MissionStmt->setUInt32(l_Index++, m_Missions[l_I].DatabaseID);
                l_MissionStmt->setUInt32(l_Index++, m_ID);

                l_GarrisonTransaction->Append(l_MissionStmt);
            }
            else
            {
                PreparedStatement * l_MissionStmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_MISSION);
                l_MissionStmt->setUInt32(0, m_Missions[l_I].DatabaseID);
                l_GarrisonTransaction->Append(l_MissionStmt);
            }
        }

        for (uint32 l_I = 0; l_I < m_Followers.size(); ++l_I)
        {
            std::ostringstream l_Abilities;

            for (uint32 l_Y = 0; l_Y < m_Followers[l_I].Abilities.size(); ++l_Y)
                l_Abilities << m_Followers[l_I].Abilities[l_Y] << ' ';

            PreparedStatement* l_FollowerStmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON_FOLLOWER);

            l_Index = 0;
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].FollowerID);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].Level);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].XP);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].Durability);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].Quality);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].ItemLevelArmor);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].ItemLevelWeapon);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].CurrentMissionID);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].CurrentBuildingID);
            l_FollowerStmt->setString(l_Index++, l_Abilities.str());
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].ZoneSupportSpellID);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].Flags);
            l_FollowerStmt->setString(l_Index++, m_Followers[l_I].ShipName);
            l_FollowerStmt->setUInt32(l_Index++, m_Followers[l_I].DatabaseID);
            l_FollowerStmt->setUInt32(l_Index++, m_ID);

            l_GarrisonTransaction->Append(l_FollowerStmt);
        }

        for (auto const& l_Talent : m_Talents)
        {
            PreparedStatement* l_TalentStmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON_TALENT);

            l_Index = 0;
            l_TalentStmt->setUInt32(l_Index++, l_Talent.StartResearchTime);
            l_TalentStmt->setUInt32(l_Index++, l_Talent.Flags);
            l_TalentStmt->setUInt32(l_Index++, m_ID);
            l_TalentStmt->setUInt32(l_Index++, l_Talent.TalentID);

            l_GarrisonTransaction->Append(l_TalentStmt);
        }

        if (m_GarrisonDraenor)
            m_GarrisonDraenor->Save(l_GarrisonTransaction);

        if (m_GarrisonBrokenIsles)
            m_GarrisonBrokenIsles->Save(l_GarrisonTransaction);

        CharacterDatabase.CommitTransaction(l_GarrisonTransaction);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Delete garrison
    void Manager::DeleteFromDB(uint64 p_PlayerGUID, SQLTransaction p_Transation)
    {
        PreparedStatement* l_Stmt;

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_MISSIONS);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_FOLLOWERS);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_WORKORDERS);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_TALENTS);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);

        GDraenor::GarrisonDraenor::DeleteFromDB(p_PlayerGUID, p_Transation);
        GBrokenIsles::GarrisonBrokenIsles::DeleteFromDB(p_PlayerGUID, p_Transation);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update the garrison
    void Manager::Update()
    {
        /// Update followers
        UpdateFollowers();
        /// Update work order
        UpdateWorkOrders();
        /// Update mission distribution
        UpdateMissionDistribution();
        /// Update talents
        UpdateTalents();

        if (m_GarrisonDraenor)
            m_GarrisonDraenor->Update();

        if (m_GarrisonBrokenIsles)
            m_GarrisonBrokenIsles->Update();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get Garrison Faction Index
    FactionIndex::Type Manager::GetGarrisonFactionIndex() const
    {
        assert(m_Owner);

        switch (m_Owner->GetTeam())
        {
            case ALLIANCE:
                return FactionIndex::Alliance;

            case HORDE:
                return FactionIndex::Horde;
        }

        return FactionIndex::Horde;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get dreanor garrison
    GDraenor::GarrisonDraenor * Manager::GetDraenorGarrison() const
    {
        return m_GarrisonDraenor;
    }
    /// Get dreanor garrison
    GBrokenIsles::GarrisonBrokenIsles * Manager::GetBrokenIslesGarrison() const
    {
        return m_GarrisonBrokenIsles;
    }

    //////////////////////////////////////////////////////////////////////////
    /// Scripts
    //////////////////////////////////////////////////////////////////////////

    /// Get garrison script
    Interfaces::GarrisonSite* Manager::GetGarrisonScript() const
    {
        return m_GarrisonScriptID ? sObjectMgr->GetGarrisonSiteBase(m_GarrisonScriptID) : nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// When the garrison owner enter in the garrisson (@See Player::UpdateArea)
    void Manager::OnPlayerEnter()
    {
        if (m_GarrisonDraenor)
            m_GarrisonDraenor->OnPlayerEnter();
        if (m_GarrisonBrokenIsles)
            m_GarrisonBrokenIsles->OnPlayerEnter();
    }
    /// When the garrison owner leave the garrisson (@See Player::UpdateArea)
    void Manager::OnPlayerLeave()
    {
        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (l_GarrisonScript)
            m_Owner->SetPhaseMask(l_GarrisonScript->GetPhaseMask(m_Owner), true);

        if (m_GarrisonDraenor)
            m_GarrisonDraenor->OnPlayerLeave();
        if (m_GarrisonBrokenIsles)
            m_GarrisonBrokenIsles->OnPlayerLeave();
    }
    /// When the garrison owner started a quest
    void Manager::OnQuestStarted(const Quest* p_Quest)
    {
        /// Update phasing
        if (Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript())
            m_Owner->SetPhaseMask(l_GarrisonScript->GetPhaseMask(m_Owner), true);
    }
    /// When the garrison owner reward a quest
    void Manager::OnQuestReward(const Quest* p_Quest)
    {
        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (l_GarrisonScript)
        {
            /// Broadcast event
            l_GarrisonScript->OnQuestReward(m_Owner, p_Quest);
            /// Update phasing
            m_Owner->SetPhaseMask(l_GarrisonScript->GetPhaseMask(m_Owner), true);
        }
    }
    /// When the garrison owner abandon a quest
    void Manager::OnQuestAbandon(const Quest* p_Quest)
    {
        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (l_GarrisonScript)
        {
            /// Broadcast event
            l_GarrisonScript->OnQuestAbandon(m_Owner, p_Quest);
            /// Update phasing
            m_Owner->SetPhaseMask(l_GarrisonScript->GetPhaseMask(m_Owner), true);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// When the owner player change level
    /// @p_Level : New owner level
    void Manager::OnOwnerLevelChange(uint32 p_Level)
    {
        Interfaces::GarrisonSite* l_GarrisonScript = GetGarrisonScript();

        if (l_GarrisonScript)
        {
            /// Broadcast event
            l_GarrisonScript->OnOwnerLevelChange(p_Level);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Send remote informations
    void Manager::SendRemoteInformations()
    {
        WorldPackets::Garrison::GarrisonRemoteInfo l_RemoteInfos;

        if (GetDraenorGarrison())
        {
            auto l_DraenorGarrison = GetDraenorGarrison();

            std::vector<MS::Garrison::GDraenor::GarrisonBuilding> l_Buildings = l_DraenorGarrison->GetBuildings();

            WorldPackets::Garrison::GarrisonRemoteSiteInfo l_SiteInfos;
            if (GarrSiteLevelEntry const* l_GarrSiteLevelEntry = l_DraenorGarrison->GetGarrisonSiteLevelEntry())
                l_SiteInfos.GarrSiteLevelID = l_GarrSiteLevelEntry->SiteLevelID;

            for (auto l_Building : l_Buildings)
            {
                WorldPackets::Garrison::GarrisonRemoteBuildingInfo l_PacketBuilding;
                l_PacketBuilding.GarrPlotInstanceID = l_Building.PlotInstanceID;
                l_PacketBuilding.GarrBuildingID     = l_Building.BuildingID;

                l_SiteInfos.Buildings.push_back(l_PacketBuilding);
            }

            l_RemoteInfos.Sites.push_back(l_SiteInfos);
        }

        if (GetBrokenIslesGarrison())
        {
            auto l_DraenorBrokenIsles = GetBrokenIslesGarrison();

            ///std::vector<MS::Garrison::GDraenor::GarrisonBuilding> l_Buildings = l_DraenorBrokenIsles->GetBuildings();

            WorldPackets::Garrison::GarrisonRemoteSiteInfo l_SiteInfos;
            l_SiteInfos.GarrSiteLevelID = l_DraenorBrokenIsles->GetGarrisonSiteLevelEntry()->SiteLevelID;

            ///for (auto l_Building : l_Buildings)
            ///{
            ///    WorldPackets::Garrison::GarrisonRemoteBuildingInfo l_PacketBuilding;
            ///    l_PacketBuilding.GarrPlotInstanceID = l_Building.PlotInstanceID;
            ///    l_PacketBuilding.GarrBuildingID     = l_Building.BuildingID;
            ///
            ///    l_SiteInfos.Buildings.push_back(l_PacketBuilding);
            ///}

            l_RemoteInfos.Sites.push_back(l_SiteInfos);
        }

        m_Owner->SendDirectMessage(l_RemoteInfos.Write());
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get garrison ID
    uint32 Manager::GetGarrisonID()
    {
        return m_ID;
    }

}   ///< namespace Garrison
}   ///< namespace MS


