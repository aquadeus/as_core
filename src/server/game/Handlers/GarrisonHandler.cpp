#include "Common.h"
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "UpdateMask.h"
#include "NPCHandler.h"
#include "Pet.h"
#include "MapManager.h"
#include "GarrisonMgr.hpp"
#include "CreatureAI.h"
#include "Chat.h"
#include "ScriptMgr.h"
#include "GossipDef.h"
#include "HelperDefines.h"
#include "../../../scripts/Draenor/Garrison/GarrisonScriptData.hpp"
#include "../../scripts/Draenor/Garrison/GarrisonNPC.hpp"

#include "Packets/GarrisonPackets.h"

void WorldSession::HandleGetGarrisonInfoOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    WorldPackets::Garrison::GetGarrisonInfo l_ClientPacket(p_RecvData);
    l_ClientPacket.Read();

    if (l_Garrison->IsLoaded())
        SendGetGarrisonInfoResponse();
    else
        l_Garrison->SetRequested(true);
}

void WorldSession::SendGetGarrisonInfoResponse()
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    l_Garrison->UpdateMissionDistribution(true);

    time_t l_Now = time(nullptr);
    if (l_Now - m_TimeLastGarrisonInfo < 2)
        return;
    else
        m_TimeLastGarrisonInfo = l_Now;

    WorldPackets::Garrison::GetGarrisonInfoResult l_Response;
    l_Response.FactionIndex = l_Garrison->GetGarrisonFactionIndex();

    WorldPackets::Garrison::FollowerSoftCapInfo l_FollowerSoftCap;
    l_FollowerSoftCap.GarrFollowerTypeID = MS::Garrison::eGarrisonFollowerTypeID::FollowerTypeIDDraenor;
    l_FollowerSoftCap.Count = MS::Garrison::eFollowerSoftCaps::SoftCapDraenor;
    l_Response.FollowerSoftCaps.push_back(l_FollowerSoftCap);
    l_FollowerSoftCap.GarrFollowerTypeID = MS::Garrison::eGarrisonFollowerTypeID::FollowerTypeIDShipyard;
    l_FollowerSoftCap.Count = MS::Garrison::eFollowerSoftCaps::SoftCapShipyard;
    l_Response.FollowerSoftCaps.push_back(l_FollowerSoftCap);
    l_FollowerSoftCap.GarrFollowerTypeID = MS::Garrison::eGarrisonFollowerTypeID::FollowerTypeIDClassHall;
    l_FollowerSoftCap.Count = MS::Garrison::eFollowerSoftCaps::SoftCapClassHall;

    std::set<GarrAbilityEffectEntry const*> l_GarrAbilityEffectEntries = GetGarrAbilityEffectEntryByEffectType(MS::Garrison::AbilityEffectTypes::ModFollowerSoftCap);
    for (MS::Garrison::GarrisonTalent const* l_Talent : l_Garrison->GetGarrisonTalents(MS::Garrison::GarrisonType::Type::GarrisonBrokenIsles))
    {
        GarrTalentEntry const* l_GarrTalentEntry = l_Talent->GetEntry();
        if (!l_GarrTalentEntry)
            continue;

        GarrAbilityEntry const* l_GarrAbilityEntry = sGarrAbilityStore.LookupEntry(l_GarrTalentEntry->GarrAbilityID);
        if (!l_GarrAbilityEntry)
            continue;

        for (GarrAbilityEffectEntry const* l_GarrAbilityEffectEntry : l_GarrAbilityEffectEntries)
        {
            if (l_GarrAbilityEffectEntry->AbilityID == l_GarrAbilityEntry->ID)
                l_FollowerSoftCap.Count += l_GarrAbilityEffectEntry->ActionValueFlat;
        }
    }

    l_Response.FollowerSoftCaps.push_back(l_FollowerSoftCap);

    if (l_Garrison->GetDraenorGarrison())
    {
        MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = l_Garrison->GetDraenorGarrison();

        if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
            return;

        std::vector<MS::Garrison::GarrisonPlotInstanceInfoLocation>   l_Plots               = l_DraenorGarrison->GetPlots();
        std::vector<MS::Garrison::GDraenor::GarrisonBuilding>         l_Buildings           = l_DraenorGarrison->GetBuildings();
        std::vector<MS::Garrison::GarrisonMission>                    l_CompletedMission    = l_Garrison->GetCompletedMissions(MS::Garrison::GarrisonType::GarrisonDraenor);
        std::vector<MS::Garrison::GarrisonMission>                    l_Missions            = l_Garrison->GetMissions(MS::Garrison::GarrisonType::GarrisonDraenor);
        std::vector<MS::Garrison::GarrisonFollower>                   l_Followers           = l_Garrison->GetFollowers(MS::Garrison::GarrisonType::GarrisonDraenor);

        WorldPackets::Garrison::GarrisonInfo l_Infos;
        l_Infos.GarrTypeID                      = MS::Garrison::GarrisonType::GarrisonDraenor;
        l_Infos.GarrSiteID                      = l_DraenorGarrison->GetGarrisonSiteLevelEntry()->SiteID;
        l_Infos.GarrSiteLevelID                 = l_DraenorGarrison->GetGarrisonSiteLevelEntry()->SiteLevelID;
        l_Infos.NumFollowerActivationsRemaining = l_Garrison->GetNumFollowerActivationsRemaining(MS::Garrison::GarrisonType::GarrisonDraenor);
        l_Infos.NumMissionsStartedToday         = l_Garrison->GetMissionStartedTodayCount(MS::Garrison::GarrisonType::GarrisonDraenor);
        l_Infos.FollowerSoftCap                 = 20;

        for (auto l_Plot : l_Plots)
        {
            WorldPackets::Garrison::GarrisonPlotInfo l_PacketPlot;
            l_PacketPlot.GarrPlotInstanceID = l_Plot.PlotInstanceID;
            l_PacketPlot.PlotType           = l_DraenorGarrison->GetPlotType(l_Plot.PlotInstanceID);
            l_PacketPlot.PlotPos            = Position(l_Plot.X, l_Plot.Y, l_Plot.Z, l_Plot.O);

            l_Infos.Plots.push_back(l_PacketPlot);
        }

        for (auto l_Building : l_Buildings)
        {
            WorldPackets::Garrison::GarrisonBuildingInfo l_PacketBuilding;
            l_PacketBuilding.GarrPlotInstanceID = l_Building.PlotInstanceID;
            l_PacketBuilding.GarrBuildingID     = l_Building.BuildingID;
            l_PacketBuilding.TimeBuilt          = l_Building.TimeBuiltStart;
            l_PacketBuilding.CurrentGarSpecID   = l_Building.SpecID;
            l_PacketBuilding.TimeSpecCooldown   = l_Building.TimeBuiltEnd;
            l_PacketBuilding.Active             = l_Building.Active;

            l_Infos.Buildings.push_back(l_PacketBuilding);
        }

        for (auto l_Follower : l_Followers)
        {
            WorldPackets::Garrison::GarrisonFollower l_PacketFollower;
            l_PacketFollower.DbID               = l_Follower.DatabaseID;
            l_PacketFollower.GarrFollowerID     = l_Follower.FollowerID;
            l_PacketFollower.Quality            = l_Follower.Quality;
            l_PacketFollower.FollowerLevel      = l_Follower.Level;
            l_PacketFollower.ItemLevelWeapon    = l_Follower.ItemLevelWeapon;
            l_PacketFollower.ItemLevelArmor     = l_Follower.ItemLevelArmor;
            l_PacketFollower.Xp                 = l_Follower.XP;
            l_PacketFollower.Durability         = l_Follower.Durability;
            l_PacketFollower.CurrentBuildingID  = l_Follower.CurrentBuildingID;
            l_PacketFollower.CurrentMissionID   = l_Follower.CurrentMissionID;
            l_PacketFollower.AbilityID          = l_Follower.Abilities;
            l_PacketFollower.ZoneSupportSpellID = l_Follower.ZoneSupportSpellID;
            l_PacketFollower.FollowerStatus     = l_Follower.Flags;
            l_PacketFollower.CustomName         = l_Follower.ShipName;

            l_Infos.Followers.push_back(l_PacketFollower);
        }

        for (auto l_Mission : l_Missions)
        {
            WorldPackets::Garrison::GarrisonMission l_PacketMission;
            l_PacketMission.DbID            = l_Mission.DatabaseID;
            l_PacketMission.MissionRecID    = l_Mission.MissionID;
            l_PacketMission.OfferTime       = l_Mission.OfferTime;
            l_PacketMission.OfferDuration   = l_Mission.OfferMaxDuration;
            l_PacketMission.StartTime       = l_Mission.StartTime;
            l_PacketMission.TravelDuration  = l_Mission.TravelDuration;
            l_PacketMission.MissionDuration = l_Mission.Duration;
            l_PacketMission.MissionState    = l_Mission.State;
            l_PacketMission.Unknown1        = l_Mission.ChestChance;                    ///< @TODO
            l_PacketMission.Unknown2        = l_Mission.Unk;                            ///< @TODO

            l_Infos.Missions.push_back(l_PacketMission);
        }

        for (auto l_Mission : l_Missions)
        {
            std::vector<WorldPackets::Garrison::GarrisonMissionReward> l_MissionRewards;

            auto l_Rewards = sObjectMgr->GetMissionRewards(l_Mission.MissionID);

            if (l_Rewards)
            {
                for (auto l_Reward = l_Rewards->begin(); l_Reward != l_Rewards->end(); ++l_Reward)
                {
                    WorldPackets::Garrison::GarrisonMissionReward l_Current;
                    l_Current.ItemID = (*l_Reward).ItemID;
                    l_Current.Quantity = (*l_Reward).ItemQuantity;
                    l_Current.CurrencyID = (*l_Reward).CurrencyType;
                    l_Current.CurrencyQuantity = (*l_Reward).CurrencyQuantity;
                    l_Current.FollowerXP = (*l_Reward).FollowerXP;
                    l_Current.BonusAbilityID = (*l_Reward).GarrMssnBonusAbilityID;
                    l_Current.Unknown = 0;                                ///< @TODO


                    l_MissionRewards.push_back(l_Current);
                }
            }

            l_Infos.MissionRewards.push_back(l_MissionRewards);
        }

        for (auto l_Mission : l_Missions)
        {
            std::vector<WorldPackets::Garrison::GarrisonMissionReward> l_MissionRewards;

            /// @TODO

            l_Infos.MissionOvermaxRewards.push_back(l_MissionRewards);
        }

        /// @TODO
        ///for (auto l_MissionAreaBonuses : l_MissionAreaBonuses)
        ///{
        ///    WorldPackets::Garrison::GarrisonMissionAreaBonus l_PacketMissionAreaBonus;
        ///    l_PacketMissionAreaBonus.GarrMssnBonusAbilityID = 0;
        ///    l_PacketMissionAreaBonus.StartTime              = time_t(0);
        ///
        ///    l_Infos.MissionAreaBonuses.push_back(l_PacketMissionAreaBonus);
        ///}

        for (auto l_Mission : l_Missions)
            l_Infos.CanStartMission.push_back(true);

        for (auto l_Mission : l_CompletedMission)
            l_Infos.ArchivedMissions.push_back(l_Mission.MissionID);

        l_Response.Garrisons.push_back(l_Infos);
    }

    if (l_Garrison->GetBrokenIslesGarrison())
    {
        MS::Garrison::GBrokenIsles::GarrisonBrokenIsles* l_BrokenIslesGarrison = l_Garrison->GetBrokenIslesGarrison();

        if (!l_BrokenIslesGarrison || !l_BrokenIslesGarrison->GetGarrisonSiteLevelEntry())
            return;

        std::vector<MS::Garrison::GarrisonMission> l_CompletedMission   = l_Garrison->GetCompletedMissions(MS::Garrison::GarrisonType::GarrisonBrokenIsles);
        std::vector<MS::Garrison::GarrisonMission> l_Missions           = l_Garrison->GetMissions(MS::Garrison::GarrisonType::GarrisonBrokenIsles);
        std::vector<MS::Garrison::GarrisonFollower > l_Followers        = l_Garrison->GetFollowers(MS::Garrison::GarrisonType::GarrisonBrokenIsles);
        std::vector<MS::Garrison::GarrisonTalent const*> l_Talents      = l_Garrison->GetGarrisonTalents(MS::Garrison::GarrisonType::GarrisonBrokenIsles);

        WorldPackets::Garrison::GarrisonInfo l_Infos;
        l_Infos.GarrTypeID                      = MS::Garrison::GarrisonType::GarrisonBrokenIsles;
        l_Infos.GarrSiteID                      = l_BrokenIslesGarrison->GetGarrisonSiteLevelEntry()->SiteID;
        l_Infos.GarrSiteLevelID                 = l_BrokenIslesGarrison->GetGarrisonSiteLevelEntry()->SiteLevelID;
        l_Infos.NumFollowerActivationsRemaining = l_Garrison->GetNumFollowerActivationsRemaining(MS::Garrison::GarrisonType::GarrisonBrokenIsles);
        l_Infos.NumMissionsStartedToday         = l_Garrison->GetMissionStartedTodayCount(MS::Garrison::GarrisonType::GarrisonBrokenIsles);
        l_Infos.FollowerSoftCap                 = 20;

        for (auto l_Follower : l_Followers)
        {
            WorldPackets::Garrison::GarrisonFollower l_PacketFollower;
            l_PacketFollower.DbID               = l_Follower.DatabaseID;
            l_PacketFollower.GarrFollowerID     = l_Follower.FollowerID;
            l_PacketFollower.Quality            = l_Follower.Quality;
            l_PacketFollower.FollowerLevel      = l_Follower.Level;
            l_PacketFollower.ItemLevelWeapon    = l_Follower.ItemLevelWeapon;
            l_PacketFollower.ItemLevelArmor     = l_Follower.ItemLevelArmor;
            l_PacketFollower.Xp                 = l_Follower.XP;
            l_PacketFollower.Durability         = l_Follower.Durability;
            l_PacketFollower.CurrentBuildingID  = l_Follower.CurrentBuildingID;
            l_PacketFollower.CurrentMissionID   = l_Follower.CurrentMissionID;
            l_PacketFollower.AbilityID          = l_Follower.Abilities;
            l_PacketFollower.ZoneSupportSpellID = l_Follower.ZoneSupportSpellID;
            l_PacketFollower.FollowerStatus     = l_Follower.Flags;
            l_PacketFollower.CustomName         = l_Follower.ShipName;

            l_Infos.Followers.push_back(l_PacketFollower);
        }

        for (auto l_Mission : l_Missions)
        {
            WorldPackets::Garrison::GarrisonMission l_PacketMission;
            l_PacketMission.DbID            = l_Mission.DatabaseID;
            l_PacketMission.MissionRecID    = l_Mission.MissionID;
            l_PacketMission.OfferTime       = l_Mission.OfferTime;
            l_PacketMission.OfferDuration   = l_Mission.OfferMaxDuration;
            l_PacketMission.StartTime       = l_Mission.StartTime;
            l_PacketMission.TravelDuration  = l_Mission.TravelDuration;
            l_PacketMission.MissionDuration = l_Mission.Duration;
            l_PacketMission.MissionState    = l_Mission.State;
            l_PacketMission.Unknown1        = l_Mission.ChestChance;                            ///< @TODO
            l_PacketMission.Unknown2        = l_Mission.Unk;                            ///< @TODO

            l_Infos.Missions.push_back(l_PacketMission);
        }

        for (auto l_Mission : l_Missions)
        {
            std::vector<WorldPackets::Garrison::GarrisonMissionReward> l_MissionRewards;

            auto l_Rewards = sObjectMgr->GetMissionRewards(l_Mission.MissionID);

            if (l_Rewards)
            {
                for (auto l_Reward = l_Rewards->begin(); l_Reward != l_Rewards->end(); ++l_Reward)
                {
                    WorldPackets::Garrison::GarrisonMissionReward l_Current;
                    l_Current.ItemID = (*l_Reward).ItemID;
                    l_Current.Quantity = (*l_Reward).ItemQuantity;
                    l_Current.CurrencyID = (*l_Reward).CurrencyType;
                    l_Current.CurrencyQuantity = (*l_Reward).CurrencyQuantity;
                    l_Current.FollowerXP = (*l_Reward).FollowerXP;
                    l_Current.BonusAbilityID = (*l_Reward).GarrMssnBonusAbilityID;
                    l_Current.Unknown = 0;                                ///< @TODO

                    l_MissionRewards.push_back(l_Current);
                }
            }

            l_Infos.MissionRewards.push_back(l_MissionRewards);
        }

        for (auto l_Mission : l_Missions)
        {
            std::vector<WorldPackets::Garrison::GarrisonMissionReward> l_MissionRewards;

            auto l_Rewards = sObjectMgr->GetMissionRewards(l_Mission.MissionID);

            if (l_Rewards)
            {
                for (auto l_Reward = l_Rewards->begin(); l_Reward != l_Rewards->end(); ++l_Reward)
                {
                    if (!(*l_Reward).BonusItemID && !(*l_Reward).BonusGold)
                        continue;

                    WorldPackets::Garrison::GarrisonMissionReward l_Current;
                    l_Current.ItemID = (*l_Reward).BonusItemID;
                    l_Current.Quantity = (*l_Reward).BonusItemID != 0;
                    l_Current.CurrencyQuantity = (*l_Reward).BonusGold;

                    l_MissionRewards.push_back(l_Current);
                }
            }

            l_Infos.MissionOvermaxRewards.push_back(l_MissionRewards);
        }

        int j = 0;
        l_Infos.Talents.resize(l_Talents.size());
        for (auto l_Talent : l_Talents)
        {
            auto& l_PacketTalent = l_Infos.Talents[j++];
            l_PacketTalent.GarrTalentID         = l_Talent->TalentID;
            l_PacketTalent.ResearchStartTime    = l_Talent->StartResearchTime;
            l_PacketTalent.Flags                = l_Talent->Flags;
        }

        for (auto l_Mission : l_Missions)
            l_Infos.CanStartMission.push_back(true);

        for (auto l_Mission : l_CompletedMission)
            l_Infos.ArchivedMissions.push_back(l_Mission.MissionID);

        l_Response.Garrisons.push_back(l_Infos);
    }

    SendPacket(l_Response.Write());

    if (auto l_DraenorGarrison = l_Garrison->GetDraenorGarrison())
    {
        WorldPackets::Garrison::GarrisonRequestBlueprintAndSpecializationDataResult l_BlueprintAndSpecializationResult;
        l_BlueprintAndSpecializationResult.GarrTypeID           = MS::Garrison::GarrisonType::GarrisonDraenor;
        l_BlueprintAndSpecializationResult.BlueprintsKnown      = l_DraenorGarrison->GetKnownBlueprints();
        l_BlueprintAndSpecializationResult.SpecializationsKnown = l_DraenorGarrison->GetKnownSpecializations();

        SendPacket(l_BlueprintAndSpecializationResult.Write());
    }
}
void WorldSession::HandleRequestGarrisonRequestBlueprintAndSpecializationDataOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    auto l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison)
        return;

    WorldPackets::Garrison::GarrisonRequestBlueprintAndSpecializationDataResult l_BlueprintAndSpecializationResult;
    l_BlueprintAndSpecializationResult.GarrTypeID           = MS::Garrison::GarrisonType::GarrisonDraenor;
    l_BlueprintAndSpecializationResult.BlueprintsKnown      = l_DraenorGarrison->GetKnownBlueprints();
    l_BlueprintAndSpecializationResult.SpecializationsKnown = l_DraenorGarrison->GetKnownSpecializations();

    SendPacket(l_BlueprintAndSpecializationResult.Write());
}

void WorldSession::HandleRequestGarrisonUpgradeableOpcode(WorldPacket& /*p_RecvData*/)
{
    if (!m_Player)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    uint32 l_ResultValue = 0;
    l_DraenorGarrison->CanUpgrade(l_ResultValue);

    WorldPackets::Garrison::GarrisonIsUpgradeableResult l_Result;
    l_Result.Result = l_ResultValue;

    SendPacket(l_Result.Write());
}

void WorldSession::HandleUpgradeGarrisonOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    uint64 l_NpcGUID = 0;
    p_RecvData.readPackGUID(l_NpcGUID);

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, UNIT_NPC_FLAG2_GARRISON_ARCHITECT);

    WorldPackets::Garrison::GarrisonUpgradeResult l_Result;
    l_Result.GarrSiteID = 0;    ///< We send zero here, because we trigger the movie from the server

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleUpgradeGarrisonOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    if (!l_DraenorGarrison->CanUpgrade(l_Result.Result))
    {
        SendPacket(l_Result.Write());
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleUpgradeGarrisonOpcode - Can't upgrade");
        return;
    }

    l_DraenorGarrison->Upgrade();
    SendPacket(l_Result.Write());
}

void WorldSession::HandleRequestLandingPageShipmentInfoOpcode(WorldPacket& /*p_RecvData*/)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    std::vector<MS::Garrison::GarrisonWorkOrder> l_WorkOrders = l_Garrison->GetWorkOrders();

    WorldPacket l_Data(SMSG_GARRISON_LANDING_PAGE_SHIPMENT_INFO, 1024);
    l_Data << uint32(0);
    l_Data << uint32(l_WorkOrders.size());

    for (uint32 l_I = 0; l_I < l_WorkOrders.size(); ++l_I)
    {
        uint32 l_Duration = 0;

        CharShipmentEntry const* l_Entry = sCharShipmentStore.LookupEntry(l_WorkOrders[l_I].ShipmentID);

        if (l_Entry)
            l_Duration = l_Entry->Duration;

        /// @TODO http://www.mmo-champion.com/content/4662-Patch-6-1-Iron-Horde-Scrap-Meltdown-Garrison-Vendor-Rush-Orders-Blue-Posts
        l_Data << uint32(l_WorkOrders[l_I].ShipmentID);
        l_Data << uint64(l_WorkOrders[l_I].DatabaseID);

        if (m_Player->GetDraenorGarrison() && m_Player->GetDraenorGarrison()->GetBuilding(l_WorkOrders[l_I].PlotInstanceID).DatabaseID != 0)
            l_Data << uint64(m_Player->GetDraenorGarrison()->GetBuilding(l_WorkOrders[l_I].PlotInstanceID).FollowerAssigned);
        else
            l_Data << uint64(0);

        l_Data << uint32(l_WorkOrders[l_I].CreationTime);
        l_Data << uint32(l_WorkOrders[l_I].CompleteTime - l_WorkOrders[l_I].CreationTime);
        l_Data << uint32(0);                                    ///< Rewarded XP
    }

    SendPacket(&l_Data);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void WorldSession::HandleGarrisonGetBuildingLandmarkOpcode(WorldPacket& /*p_RecvData*/)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    std::vector<MS::Garrison::GDraenor::GarrisonBuilding> l_Buildings = l_DraenorGarrison->GetBuildings();

    WorldPackets::Garrison::GarrisonBuildingLandmarks l_Result;
    for (uint32 l_I = 0; l_I < l_Buildings.size(); ++l_I)
    {
        WorldPackets::Garrison::GarrisonBuildingLandmark l_Landmark;

        MS::Garrison::GarrisonPlotInstanceInfoLocation l_PlotLocation = l_DraenorGarrison->GetPlot(l_Buildings[l_I].PlotInstanceID);
        uint32 l_SiteLevelPlotInstanceID = 0;
        uint32 l_BuildingPlotInstanceID  = 0;

        for (uint32 l_Y = 0; l_Y < sGarrSiteLevelPlotInstStore.GetNumRows(); ++l_Y)
        {
            const GarrSiteLevelPlotInstEntry* l_Entry = sGarrSiteLevelPlotInstStore.LookupEntry(l_Y);

            if (l_Entry && l_Entry->PlotInstanceID == l_Buildings[l_I].PlotInstanceID && l_Entry->GarrSiteLevelID == l_DraenorGarrison->GetGarrisonSiteLevelEntry()->SiteLevelID)
            {
                l_SiteLevelPlotInstanceID = l_Entry->ID;
                break;
            }
        }

        for (uint32 l_Y = 0; l_Y < sGarrBuildingPlotInstStore.GetNumRows(); ++l_Y)
        {
            const GarrBuildingPlotInstEntry* l_Entry = sGarrBuildingPlotInstStore.LookupEntry(l_Y);

            if (l_Entry && l_Entry->SiteLevelPlotInstID == l_SiteLevelPlotInstanceID && l_Entry->BuildingID == l_Buildings[l_I].BuildingID)
            {
                l_BuildingPlotInstanceID = l_Entry->ID;
                break;
            }
        }

        l_Landmark.GarrBuildingPlotInstID = l_BuildingPlotInstanceID;
        l_Landmark.Pos = Position(l_PlotLocation.X, l_PlotLocation.Y, l_PlotLocation.Z);

        l_Result.Landmarks.push_back(l_Landmark);
    }

    SendPacket(l_Result.Write());
}

void WorldSession::HandleGarrisonPurchaseBuildingOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    WorldPackets::Garrison::GarrisonPurchaseBuilding l_Query(p_RecvData);
    l_Query.Read();

    sGarrisonDraenorBuildingManager->LearnAllowedBuildings(m_Player, l_DraenorGarrison);

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_Query.NpcGUID, UNIT_NPC_FLAG2_GARRISON_ARCHITECT);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonPurchaseBuildingOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_Query.NpcGUID)));
        return;
    }

    MS::Garrison::GDraenor::PurchaseBuildingResults::Type l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::Ok;

    if (!sGarrBuildingStore.LookupEntry(l_Query.BuildingID))
        l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::InvalidBuildingID;

    if (!l_Result && !sGarrPlotInstanceStore.LookupEntry(l_Query.PlotInstanceID))
        l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::InvalidPlot;

    if (!l_Result && !l_DraenorGarrison->KnownBlueprint(l_Query.BuildingID))
        l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::RequireBluePrint;

    if (!l_Result && l_DraenorGarrison->GetBuildingWithBuildingID(l_Query.BuildingID).BuildingID != 0)
        l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::BuildingExists;

    if (!l_Result && !l_DraenorGarrison->IsBuildingPlotInstanceValid(l_Query.BuildingID, l_Query.PlotInstanceID))
        l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::InvalidPlotBuilding;

    if (!l_Result)
        l_Result = l_DraenorGarrison->CanPurchaseBuilding(l_Query.BuildingID);

    if (!sGarrisonDraenorBuildingManager->IsBluePrintAllowedForPurchasingBuilding(l_Query.BuildingID, m_Player))
    {
        l_Result = MS::Garrison::GDraenor::PurchaseBuildingResults::InvalidBuildingID;

        std::string l_Message = "Building not available yet";
        switch (m_Player->GetSession()->GetSessionDb2Locale())
        {
            case LocaleConstant::LOCALE_frFR:
                l_Message = "Batiment non disponible";
                break;

            case LocaleConstant::LOCALE_esES:
            case LocaleConstant::LOCALE_esMX:
                l_Message = "Edificio no disponible";
                break;

            default:
                break;
        }

        WorldPacket l_Data(SMSG_PRINT_NOTIFICATION, 2 + l_Message.size());
        l_Data.WriteBits(l_Message.size(), 12);
        l_Data.FlushBits();
        l_Data.WriteString(l_Message);

        SendPacket(&l_Data);
    }

    WorldPackets::Garrison::GarrisonPlaceBuildingResult l_PlaceResult;
    l_PlaceResult.GarrTypeID                = MS::Garrison::GarrisonType::GarrisonDraenor;
    l_PlaceResult.Result                    = l_Result;
    l_PlaceResult.PlayActivationCinematic   = false;

    if (l_Result == MS::Garrison::GDraenor::PurchaseBuildingResults::Ok)
    {
        MS::Garrison::GDraenor::GarrisonBuilding l_Building = l_DraenorGarrison->PurchaseBuilding(l_Query.BuildingID, l_Query.PlotInstanceID);

        l_PlaceResult.BuildingInfo.GarrPlotInstanceID   = l_Query.PlotInstanceID;
        l_PlaceResult.BuildingInfo.GarrBuildingID       = l_Query.BuildingID;
        l_PlaceResult.BuildingInfo.TimeBuilt            = l_Building.TimeBuiltStart;
        l_PlaceResult.BuildingInfo.CurrentGarSpecID     = l_Building.SpecID;
        l_PlaceResult.BuildingInfo.TimeSpecCooldown     = l_Building.TimeBuiltEnd;
        l_PlaceResult.BuildingInfo.Active               = l_Building.Active;
    }

    SendPacket(l_PlaceResult.Write());

    m_Player->UpdateCriteria(CRITERIA_TYPE_PLACE_GARRISON_BUILDING, l_Query.BuildingID);
}

void WorldSession::HandleGarrisonCancelConstructionOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    WorldPackets::Garrison::GarrisonCancelConstruction l_Query(p_RecvData);
    l_Query.Read();

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_Query.NpcGUID, UNIT_NPC_FLAG2_GARRISON_ARCHITECT);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonCancelConstructionOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_Query.NpcGUID)));
        return;
    }

    l_DraenorGarrison->CancelConstruction(l_Query.PlotInstanceID);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void WorldSession::HandleGarrisonMissionNPCHelloOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    if (!m_Player->GetGarrison())
        return;

    uint64 l_NpcGUID = 0;

    p_RecvData.readPackGUID(l_NpcGUID);

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, 0);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonMissionNPCHelloOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    SendGarrisonOpenMissionNpc(l_NpcGUID);
}

void WorldSession::HandleGarrisonRequestSetMissionNPC(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    uint64 l_NpcGUID = 0;

    p_RecvData.readPackGUID(l_NpcGUID);

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, 0);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonRequestSetMissionNPC - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    MS::Garrison::eGarrisonFollowerTypeID l_Type = MS::Garrison::eGarrisonFollowerTypeID::FollowerTypeIDClassHall;

    if (m_Player->IsInShipyard())
        l_Type = MS::Garrison::eGarrisonFollowerTypeID::FollowerTypeIDShipyard;
    else if (m_Player->IsInDraenorGarrison())
        l_Type = MS::Garrison::eGarrisonFollowerTypeID::FollowerTypeIDDraenor;

    SendGarrisonSetMissionNpc(l_NpcGUID, l_Type);
}

void WorldSession::HandleGarrisonStartMissionOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    uint64 l_NpcGUID        = 0;
    uint32 l_FollowerCount  = 0;
    uint32 l_MissionID      = 0;

    std::vector<uint64> l_Followers;

    p_RecvData.readPackGUID(l_NpcGUID);
    p_RecvData >> l_FollowerCount;
    p_RecvData >> l_MissionID;

    for (uint32 l_I = 0; l_I < l_FollowerCount; ++l_I)
    {
        uint64 l_FollowerDBID = 0;
        p_RecvData >> l_FollowerDBID;

        l_Followers.push_back(l_FollowerDBID);
    }

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWith(l_NpcGUID, 0);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonStartMissionOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    /// Prevent possible exploits
    if (m_Player->GetMap()->IsDungeon())
    {
        l_Garrison->StartMissionFailed(l_MissionID, l_Followers);
        return;
    }

    l_Garrison->StartMission(l_MissionID, l_Followers);
}

void WorldSession::HandleGarrisonCompleteMissionOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    uint64 l_NpcGUID   = 0;
    uint32 l_MissionID = 0;

    p_RecvData.readPackGUID(l_NpcGUID);
    p_RecvData >> l_MissionID;

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, 0);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonCompleteMissionOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    l_Garrison->CompleteMission(l_MissionID, true);
}

void WorldSession::HandleGarrisonMissionBonusRollOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    uint64 l_NpcGUID    = 0;
    uint32 l_MissionID  = 0;

    p_RecvData.readPackGUID(l_NpcGUID);
    p_RecvData >> l_MissionID;

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, 0);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonMissionBonusRollOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    l_Garrison->DoMissionBonusRoll(l_NpcGUID, l_MissionID);
}

void WorldSession::HandleGarrisonGenerateRecruitsOpcode(WorldPacket& p_RecvData)
{
    if (m_Player == nullptr)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (l_Garrison == nullptr)
        return;

    uint64 l_GUID      = 0;
    uint32 l_TraitID   = 0;
    uint32 l_AbilityID = 0;

    p_RecvData.readPackGUID(l_GUID);
    p_RecvData >> l_AbilityID;
    p_RecvData >> l_TraitID;

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWith(l_GUID, 0);

    if (l_Unit == nullptr)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonMissionBonusRollOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_GUID)));
        return;
    }

    if (l_Unit->ToGarrisonNPCAI())
        l_Unit->ToGarrisonNPCAI()->SendRecruitmentFollowersGenerated(m_Player, l_AbilityID ? l_AbilityID : l_TraitID, 0, l_TraitID ? true : false);
}

void WorldSession::HandleGarrisonSetRecruitmentPreferencesOpcode(WorldPacket& p_RecvData)
{
    if (m_Player == nullptr)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (l_Garrison == nullptr)
        return;


    uint64 l_GUID      = 0; ///< Unused ?
    uint32 l_TraitID   = 0;
    uint32 l_AbilityID = 0;

    p_RecvData.readPackGUID(l_GUID);

    p_RecvData >> l_AbilityID;
    p_RecvData >> l_TraitID;
}

void WorldSession::HandleGarrisonRecruitFollower(WorldPacket& p_RecvData)
{
    if (m_Player == nullptr)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (l_Garrison == nullptr)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    uint64 l_GUID       = 0;

    p_RecvData.readPackGUID(l_GUID);
    uint32 l_FollowerID = 0;
    p_RecvData >> l_FollowerID;

    Creature* l_Unit = m_Player->GetNPCIfCanInteractWith(l_GUID, 0);

    if (l_Unit == nullptr)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonMissionBonusRollOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_GUID)));
        return;
    }

    WorldPacket l_RecruitmentResult(SMSG_GARRISON_RECRUIT_FOLLOWER_RESULT, 64);

    if (l_DraenorGarrison->CanRecruitFollower())
        l_RecruitmentResult << uint32(MS::Garrison::ResultEnum::SUCCESS);
    else
    {
        l_RecruitmentResult << uint32(MS::Garrison::ResultEnum::SUCCESS); ///< need to find appropriate error ID
        m_Player->PlayerTalkClass->SendCloseGossip();
        return;
    }

    std::vector<MS::Garrison::GarrisonFollower> l_WeeklyFollowers = l_DraenorGarrison->GetWeeklyFollowerRecruits(m_Player);

    for (MS::Garrison::GarrisonFollower l_Follower : l_WeeklyFollowers)
    {
        if (l_Follower.FollowerID == l_FollowerID)
        {
            l_Follower.Write(l_RecruitmentResult);
            l_Garrison->AddFollower(l_Follower);
            l_DraenorGarrison->SetCanRecruitFollower(false);
            m_Player->SetCharacterWorldState(CharacterWorldStates::GarrisonTavernBoolCanRecruitFollower, 0);
            break;
        }
    }

    if (m_Player->GetTeamId() == TEAM_ALLIANCE && m_Player->GetQuestStatus(MS::Garrison::GDraenor::Quests::Alliance_TheHeadHunterHarverst) == QUEST_STATUS_INCOMPLETE)
        m_Player->QuestObjectiveSatisfy(39383, 1, 14);
    else if (m_Player->GetTeamId() == TEAM_HORDE && m_Player->GetQuestStatus(MS::Garrison::GDraenor::Quests::Horde_TheHeadHunterHarverst) == QUEST_STATUS_INCOMPLETE)
        m_Player->QuestObjectiveSatisfy(39418, 1, 14);

    m_Player->SendDirectMessage(&l_RecruitmentResult);
    m_Player->PlayerTalkClass->SendCloseGossip();
}

void WorldSession::HandleGarrisonChangeFollowerActivationStateOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    uint64  l_FollowerDBID  = 0;
    bool    l_Desactivate   = false;

    p_RecvData >> l_FollowerDBID;
    l_Desactivate = p_RecvData.ReadBit();

    l_Garrison->ChangeFollowerActivationState(l_FollowerDBID, !l_Desactivate);
}

void WorldSession::HandleGarrisonAssignFollowerToBuilding(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_GarrisonMgr = m_Player->GetGarrison();

    if (!l_GarrisonMgr)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    uint64 l_NpcGUID       = 0;
    uint64 l_FollowerDBID  = 0;
    int32 l_PlotInstanceID = 0;

    p_RecvData.readPackGUID(l_NpcGUID);
    l_PlotInstanceID = p_RecvData.read<int32>();
    p_RecvData >> l_FollowerDBID;

    Creature* l_Creature = m_Player->GetNPCIfCanInteractWithFlag2(l_NpcGUID, UNIT_NPC_FLAG2_GARRISON_ARCHITECT);

    if (!l_Creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonAssignFollowerToBuilding - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    if (l_PlotInstanceID)
    {
        l_DraenorGarrison->AssignFollowerToBuilding(l_FollowerDBID, (uint32)l_PlotInstanceID);

        WorldPacket l_Response(SMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING_RESULT, 1024);

        uint8 l_Result = 0; ///< Always 0 ?

        l_Response << uint64(l_FollowerDBID);
        l_Response << int32(l_Result);
        l_Response << int32(l_PlotInstanceID);

        SendPacket(&l_Response);
    }
}

void WorldSession::HandleGarrisonRemoveFollowerFromBuilding(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_GarrisonMgr = m_Player->GetGarrison();

    if (!l_GarrisonMgr)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    uint64 l_NpcGUID      = 0;
    uint64 l_FollowerDBID = 0;

    p_RecvData.readPackGUID(l_NpcGUID);
    p_RecvData >> l_FollowerDBID;

    Creature* l_Creature = m_Player->GetNPCIfCanInteractWithFlag2(l_NpcGUID, UNIT_NPC_FLAG2_GARRISON_ARCHITECT);

    if (!l_Creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonRemoveFollowerFromBuilding - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    l_DraenorGarrison->AssignFollowerToBuilding(l_FollowerDBID, 0);

    WorldPacket l_Response(SMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING_RESULT, 1024);

    uint8 l_Result = 0; ///< Always 0 ?

    l_Response << uint64(l_FollowerDBID);
    l_Response << int32(l_Result);

    SendPacket(&l_Response);
}

void WorldSession::HandleGarrisonGetShipmentInfoOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    uint64 l_NpcGUID = 0;
    p_RecvData.readPackGUID(l_NpcGUID);

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, UNIT_NPC_FLAG2_GARRISON_SHIPMENT_CRAFTER);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonMissionNPCHelloOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    uint32 l_ShipmentID     = 0;
    uint32 l_OrderAvailable = 0;
    uint32 l_PlotInstanceID = 0;

    if (auto l_DraenorGarrison = m_Player->GetDraenorGarrison())
    {
        l_PlotInstanceID = l_DraenorGarrison->GetCreaturePlotInstanceID(l_NpcGUID);

        if (!!l_PlotInstanceID)
        {
            l_OrderAvailable = l_DraenorGarrison->GetBuildingMaxWorkOrder(l_PlotInstanceID);

            uint32 l_BuildingID = l_DraenorGarrison->GetBuilding(l_PlotInstanceID).BuildingID;

            if (l_BuildingID)
            {
                if (l_Unit->AI())
                    l_ShipmentID = l_Unit->AI()->OnShipmentIDRequest(m_Player);

                if (l_ShipmentID == -1) ///< Comparison of integers of different signs: 'uint32' (aka 'unsigned int') and 'int'
                    l_ShipmentID = sGarrisonShipmentManager->GetShipmentIDForBuilding(l_BuildingID, m_Player, false);
            }
        }
    }

    if (!l_ShipmentID)
    {
        if (l_Unit->GetEntry() == MS::Garrison::GBrokenIsles::eWorkOrderNPC::NPCNomi)
        {
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo> const& l_WorkOrderInfos = MS::Garrison::GBrokenIsles::s_WorkOrderInfos[0];
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo>::const_iterator l_Itr = l_WorkOrderInfos.find(m_Player->m_SpellHelper.GetUint32(eSpellHelpers::CookingWorkorderItem));

            if (l_Itr == l_WorkOrderInfos.end())
                return;

            l_ShipmentID = (*l_Itr).second.WorkOrderID;

            CharShipmentEntry const* l_Shipment = sCharShipmentStore.LookupEntry(l_ShipmentID);

            l_PlotInstanceID = MS::Garrison::GBrokenIsles::eCookingConstant::CookPlotInstanceID;

            l_OrderAvailable = MS::Garrison::GBrokenIsles::eCookingConstant::MaxWorkOrder;
        }
        else
        {
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo> const& l_WorkOrderInfos = MS::Garrison::GBrokenIsles::s_WorkOrderInfos[m_Player->getClass()];
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo>::const_iterator l_Itr = l_WorkOrderInfos.find(l_Unit->GetEntry());

            if (l_Itr != l_WorkOrderInfos.end())
            {
                auto l_TempItr = l_Itr;
                while (l_TempItr != l_WorkOrderInfos.end() && !(*l_TempItr).second.RequiredGarrTalent && (*l_TempItr).first == l_Unit->GetEntry())
                    ++l_TempItr;

                if (l_TempItr != l_WorkOrderInfos.end() && l_Garrison->HasTalent((*l_TempItr).second.RequiredGarrTalent))
                    l_Itr = l_TempItr;

                l_ShipmentID = (*l_Itr).second.WorkOrderID;

                for (auto i : MS::Garrison::GBrokenIsles::m_questStartShipments)
                    if (m_Player->GetQuestStatus(i.first) && m_Player->GetQuestStatus(i.first) != QUEST_STATUS_REWARDED)
                        l_ShipmentID = i.second;

                CharShipmentEntry const* l_Shipment = sCharShipmentStore.LookupEntry(l_ShipmentID);
                CharShipmentContainerEntry  const* l_Container = sCharShipmentContainerStore.LookupEntry(l_Shipment ? l_Shipment->ShipmentContainerID : 0);

                l_PlotInstanceID = (l_ShipmentID * 1000);
                if (uint32 l_FollowerId = l_Garrison->GetFollowerIDForShipmentID(l_ShipmentID))
                    l_OrderAvailable = l_Garrison->GetMaxCount(l_FollowerId) - l_Garrison->GetFollowerCount(l_FollowerId);
                else
                    l_OrderAvailable = l_Container ? l_Container->WorkorderLimit : 0;

                /// Seal of Broken Fate
                if ((!l_Shipment || l_Shipment->ResultItemID == 139460) && ((m_Player->GetCharacterWorldStateValue(CharacterWorldStates::GarrisonWeeklySealOfBrokenFateWorkorder) >= 1)
                    || (m_Player->GetCharacterWorldStateValue(CharacterWorldStates::BrokenIslesWeeklyBonusRollTokenCount) >= 3)
                    || (m_Player->GetCurrency(CurrencyTypes::CURRENCY_TYPE_SEAL_OF_BROKEN_FATE, true) >= 6)))
                    l_OrderAvailable = 0;
            }
            else if (GarrisonNPCAI* l_GarrisonAI = l_Unit->ToGarrisonNPCAI())
            {
                CharShipmentEntry           const* l_Shipment = sCharShipmentStore.LookupEntry(l_GarrisonAI->LastShipmentID);
                CharShipmentContainerEntry  const* l_Container = sCharShipmentContainerStore.LookupEntry(l_Shipment ? l_Shipment->ShipmentContainerID : 0);

                if (l_Shipment && l_Container && l_Container->GarrisonType == MS::Garrison::GarrisonType::GarrisonBrokenIsles && l_Garrison->GetBrokenIslesGarrison())
                {
                    l_ShipmentID = l_GarrisonAI->LastShipmentID;
                    l_PlotInstanceID = (l_ShipmentID * 1000);

                    if (uint32 l_FollowerId = l_Garrison->GetFollowerIDForShipmentID(l_ShipmentID))
                        l_OrderAvailable = l_Garrison->GetMaxCount(l_FollowerId) - l_Garrison->GetFollowerCount(l_FollowerId);
                    else
                        l_OrderAvailable = l_Container->WorkorderLimit;
                }
            }
        }
    }

    bool l_Success = !!l_ShipmentID && !!l_PlotInstanceID;

    WorldPacket l_Response(SMSG_GET_SHIPMENT_INFO_RESPONSE, 1024);
    l_Response.WriteBit(l_Success);
    l_Response.FlushBits();

    if (l_Success)
    {
        std::vector<MS::Garrison::GarrisonWorkOrder> l_WorkOrders = l_Garrison->GetWorkOrders();

        uint32 l_PendingWorkOrderCount = std::count_if(l_WorkOrders.begin(), l_WorkOrders.end(), [l_PlotInstanceID](const MS::Garrison::GarrisonWorkOrder & p_Order) -> bool
        {
            return p_Order.PlotInstanceID == l_PlotInstanceID;
        });

        l_Response << uint32(l_ShipmentID);
        l_Response << uint32(l_OrderAvailable);
        l_Response << uint32(l_PendingWorkOrderCount);
        l_Response << uint32(l_PlotInstanceID);

        for (uint32 l_I = 0; l_I < l_WorkOrders.size(); ++l_I)
        {
            if (l_WorkOrders[l_I].PlotInstanceID != l_PlotInstanceID)
                continue;

            l_Response << uint32(l_WorkOrders[l_I].ShipmentID);
            l_Response << uint64(l_WorkOrders[l_I].DatabaseID);
            l_Response << uint64(0);                                    ///< 6.1.x FollowerID
            l_Response << uint32(l_WorkOrders[l_I].CreationTime);
            l_Response << uint32(l_WorkOrders[l_I].CompleteTime - l_WorkOrders[l_I].CreationTime);
            l_Response << uint32(0);                                    ///< 6.1.x Rewarded XP
        }
    }
    else
    {
        l_Response << uint32(0);
        l_Response << uint32(0);
        l_Response << uint32(0);
        l_Response << uint32(0);
    }

    SendPacket(&l_Response);
}

void WorldSession::HandleGarrisonCreateShipmentOpcode(WorldPacket& p_RecvData)
{
    std::function<void(const std::string &)> l_OnError = [this](const std::string & p_Message) -> void
    {
        if (m_Player->GetSession()->GetSecurity() > SEC_PLAYER)
            ChatHandler(m_Player).PSendSysMessage("HandleGarrisonCreateShipmentOpcode => %s", p_Message.c_str());

        WorldPacket l_Ack(SMSG_CREATE_SHIPMENT_RESPONSE, 16);
        l_Ack << uint64(0);
        l_Ack << uint32(0);
        l_Ack << uint32(MS::Garrison::ResultEnum::ERROR_NO_GARRISON); ///< 0 = success & 1 = error

        m_Player->SendDirectMessage(&l_Ack);
    };

    if (!m_Player)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    uint64 l_NpcGUID = 0;
    uint32 l_Count = 0;

    p_RecvData.readPackGUID(l_NpcGUID);
    p_RecvData >> l_Count;

    /// Min 1 work order
    if (!l_Count)
        l_Count = 1;

    Creature* l_Unit = GetPlayer()->GetNPCIfCanInteractWithFlag2(l_NpcGUID, UNIT_NPC_FLAG2_GARRISON_SHIPMENT_CRAFTER);

    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleGarrisonMissionNPCHelloOpcode - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(l_NpcGUID)));
        return;
    }

    uint32 l_ShipmentID     = 0;
    uint32 l_OrderMax       = 0;
    uint32 l_PlotInstanceID = 0;

    if (l_DraenorGarrison)
    {
        l_PlotInstanceID = l_DraenorGarrison->GetCreaturePlotInstanceID(l_NpcGUID);

        if (!!l_PlotInstanceID)
        {
            l_OrderMax = l_DraenorGarrison->GetBuildingMaxWorkOrder(l_PlotInstanceID);

            uint32 l_BuildingID = l_DraenorGarrison->GetBuilding(l_PlotInstanceID).BuildingID;

            if (l_BuildingID)
            {
                l_ShipmentID = sGarrisonShipmentManager->GetShipmentIDForBuilding(l_BuildingID, m_Player, true);

                if (l_ShipmentID == MS::Garrison::GDraenor::Barn::ShipmentIDS::ShipmentFurredBeast)
                {
                    if (l_Unit->AI())
                        l_ShipmentID = l_Unit->AI()->OnShipmentIDRequest(m_Player);
                }

                sScriptMgr->OnShipmentCreated(m_Player, l_Unit, l_BuildingID);
            }
        }
    }

    if (!l_ShipmentID)
    {
        if (l_Unit->GetEntry() == MS::Garrison::GBrokenIsles::eWorkOrderNPC::NPCNomi)
        {
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo> const& l_WorkOrderInfos = MS::Garrison::GBrokenIsles::s_WorkOrderInfos[0];
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo>::const_iterator l_Itr = l_WorkOrderInfos.find(m_Player->m_SpellHelper.GetUint32(eSpellHelpers::CookingWorkorderItem));

            if (l_Itr == l_WorkOrderInfos.end())
                return;

            l_ShipmentID = (*l_Itr).second.WorkOrderID;

            CharShipmentEntry const* l_Shipment = sCharShipmentStore.LookupEntry(l_ShipmentID);

            l_PlotInstanceID = MS::Garrison::GBrokenIsles::eCookingConstant::CookPlotInstanceID;

            l_OrderMax = MS::Garrison::GBrokenIsles::eCookingConstant::MaxWorkOrder;
        }
        else
        {
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo> const& l_WorkOrderInfos = MS::Garrison::GBrokenIsles::s_WorkOrderInfos[m_Player->getClass()];
            std::multimap<uint32, MS::Garrison::GBrokenIsles::WorkOrderInfo>::const_iterator l_Itr = l_WorkOrderInfos.find(l_Unit->GetEntry());

            if (l_Itr != l_WorkOrderInfos.end())
            {
                auto l_TempItr = l_Itr;
                while (l_TempItr != l_WorkOrderInfos.end() && !(*l_TempItr).second.RequiredGarrTalent && (*l_TempItr).first == l_Unit->GetEntry())
                    ++l_TempItr;

                if (l_TempItr != l_WorkOrderInfos.end() && l_Garrison->HasTalent((*l_TempItr).second.RequiredGarrTalent))
                    l_Itr = l_TempItr;

                l_ShipmentID = (*l_Itr).second.WorkOrderID;

                for (auto i : MS::Garrison::GBrokenIsles::m_questStartShipments)
                    if (m_Player->GetQuestStatus(i.first) && m_Player->GetQuestStatus(i.first) != QUEST_STATUS_REWARDED)
                        l_ShipmentID = i.second;

                CharShipmentEntry const* l_Shipment = sCharShipmentStore.LookupEntry(l_ShipmentID);
                CharShipmentContainerEntry  const* l_Container = sCharShipmentContainerStore.LookupEntry(l_Shipment ? l_Shipment->ShipmentContainerID : 0);
                l_PlotInstanceID = (l_ShipmentID * 1000);
                l_OrderMax = l_Container ? l_Container->WorkorderLimit : 0;
            }
            else if (GarrisonNPCAI* l_GarrisonAI = l_Unit->ToGarrisonNPCAI())
            {
                CharShipmentEntry           const* l_Shipment = sCharShipmentStore.LookupEntry(l_GarrisonAI->LastShipmentID);
                CharShipmentContainerEntry  const* l_Container = sCharShipmentContainerStore.LookupEntry(l_Shipment ? l_Shipment->ShipmentContainerID : 0);

                if (l_Shipment && l_Container && l_Container->GarrisonType == MS::Garrison::GarrisonType::GarrisonBrokenIsles && l_Garrison->GetBrokenIslesGarrison())
                {
                    l_ShipmentID = l_GarrisonAI->LastShipmentID;
                    l_PlotInstanceID = (l_ShipmentID * 1000);
                    l_OrderMax = l_Container->WorkorderLimit;
                }
            }
        }
    }

    if (!l_ShipmentID || !l_PlotInstanceID)
    {
        l_OnError("Invalid ShipmentID or PlotInstanceID");
        return;
    }

    for (uint32 l_OrderI = 0; l_OrderI < l_Count; ++l_OrderI)
    {
        if (((int32)l_OrderMax - (int32)l_Garrison->GetWorkOrderCount(l_PlotInstanceID)) < 1)
        {
            l_OnError("Max work order for this building reached");
            return;
        }

        const CharShipmentEntry * l_ShipmentEntry = sCharShipmentStore.LookupEntry(l_ShipmentID);

        if (!l_ShipmentEntry)
        {
            l_OnError("Shipment entry not found");
            return;
        }

        const SpellInfo * l_Spell = sSpellMgr->GetSpellInfo(l_ShipmentEntry->SpellID);

        if (!l_Spell)
        {
            l_OnError("Shipment spell not found");
            return;
        }

        bool l_HasReagents = true;
        for (uint32 l_I = 0; l_I < MAX_SPELL_REAGENTS; ++l_I)
        {
            uint32 l_ItemEntry = l_Spell->Reagent[l_I];
            uint32 l_ItemCount = l_Spell->ReagentCount[l_I];

            if (!l_ItemEntry || !l_ItemCount)
                continue;

            if (!m_Player->HasItemCount(l_ItemEntry, l_ItemCount, false, true))
                l_HasReagents = false;
        }

        if (l_Spell->CurrencyID)
        {
            if (!m_Player->HasCurrency(l_Spell->CurrencyID, l_Spell->CurrencyCount))
                l_HasReagents = false;
        }

        if (!l_HasReagents)
        {
            l_OnError("Doesn't have reagents");
            return;
        }

        for (uint32 l_I = 0; l_I < MAX_SPELL_REAGENTS; ++l_I)
        {
            uint32 l_ItemEntry = l_Spell->Reagent[l_I];
            uint32 l_ItemCount = l_Spell->ReagentCount[l_I];

            if (!l_ItemEntry || !l_ItemCount)
                continue;

            m_Player->DestroyItemCount(l_ItemEntry, l_ItemCount, true);
        }

        if (l_Spell->CurrencyID)
            m_Player->ModifyCurrency(l_Spell->CurrencyID, -int32(l_Spell->CurrencyCount), false);

        m_Player->CastSpell(m_Player, l_Spell, TRIGGERED_FULL_MASK);

        uint64 l_DatabaseID = l_Garrison->StartWorkOrder(l_PlotInstanceID, l_ShipmentID);

        /// Opening the Test Kitchen - KillCredit
        if (l_ShipmentID == 247)
            m_Player->KilledMonsterCredit(102559);

        WorldPacket l_Ack(SMSG_CREATE_SHIPMENT_RESPONSE, 16);
        l_Ack << uint64(l_DatabaseID);
        l_Ack << uint32(l_ShipmentID);
        l_Ack << uint32(l_DatabaseID != 0);

        m_Player->SendDirectMessage(&l_Ack);
    }
}

void WorldSession::HandleGarrisonFollowerRename(WorldPacket& p_RecvData)
{
    uint64 l_DatabaseID;
    uint32 l_NameLen;
    std::string l_Name;

    p_RecvData >> l_DatabaseID;
    l_NameLen = p_RecvData.ReadBits(7);
    l_Name = p_RecvData.ReadString(l_NameLen);

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    MS::Garrison::GDraenor::GarrisonDraenor * l_DraenorGarrison = m_Player->GetDraenorGarrison();

    if (!l_DraenorGarrison || !l_DraenorGarrison->GetGarrisonSiteLevelEntry())
        return;

    l_DraenorGarrison->RenameFollower(l_DatabaseID, l_Name);
}

void WorldSession::HandleGarrisonRemoveFollowerOpcode(WorldPacket& p_RecvData)
{
    uint64 l_Guid, l_DatabaseID;

    p_RecvData.readPackGUID(l_Guid);
    p_RecvData >> l_DatabaseID;

    Creature* l_NPC = m_Player->GetNPCIfCanInteractWithFlag2(l_Guid, UNIT_NPC_FLAG2_GARRISON_MISSION_NPC);

    if (!l_NPC)
        return;

    MS::Garrison::Manager* l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    l_Garrison->RemoveFollower(l_DatabaseID);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void WorldSession::SendGarrisonOpenArchitect(uint64 p_CreatureGUID)
{
    MS::Garrison::Manager * l_Garrison = m_Player->GetGarrison();

    if (!l_Garrison)
        return;

    WorldPacket l_Data(SMSG_GARRISON_OPEN_ARCHITECT, 18);
    l_Data.appendPackGUID(p_CreatureGUID);

    SendPacket(&l_Data);
}

void WorldSession::SendGarrisonOpenMissionNpc(uint64 p_CreatureGUID)
{
    Creature* l_NPC = sObjectAccessor->FindCreature(p_CreatureGUID);

    if (!l_NPC)
        return;

    if (m_Player->IsInDraenorGarrison() || m_Player->IsInShipyard())
    {
        MS::Garrison::GarrisonType::Type l_Type = MS::Garrison::GarrisonType::GarrisonDraenor;

        Creature* l_Creature = sObjectAccessor->FindCreature(p_CreatureGUID);

        WorldPacket l_Data(SMSG_ADVENTURE_LIST_UPDATE, 22);
        uint32 l_Count = 0;
        l_Data << uint32(l_Type);
        l_Data << uint32(0);            ///< Unk

        l_Data << uint32(l_Count);      ///< Unk
        for (uint32 l_I = 0; l_I < l_Count; ++l_I)
            l_Data << uint32(0);        ///< Unk

        l_Data.WriteBit(1);             ///< Unk
        l_Data.WriteBit(0);             ///< Unk
        l_Data.FlushBits();             ///< Unk
        SendPacket(&l_Data);
    }
    else
    {
        SendAdventureMapOpen(p_CreatureGUID);
        GetPlayer()->PlayerTalkClass->SendCloseGossip();
    }
}

void WorldSession::SendGarrisonSetMissionNpc(uint64 p_CreatureGUID, uint32 p_Type)
{
    Creature* l_Creature = sObjectAccessor->FindCreature(p_CreatureGUID);

    WorldPacket l_Data(SMSG_GARRISON_OPEN_MISSION_NPC, 22);
    l_Data.appendPackGUID(p_CreatureGUID);
    l_Data << uint32(p_Type);
    SendPacket(&l_Data);
}


void WorldSession::HandleGarrisonResearchTalent(WorldPacket& p_RecvData)
{
    uint32 l_ID;
    p_RecvData >> l_ID;

    auto l_Entry = sGarrTalentStore.LookupEntry(l_ID);
    auto l_Garrison  = m_Player->GetGarrison();

    if (!l_Entry || !l_Garrison)
        return;

    bool l_Replace = l_Garrison->HasATalentInRow(l_Entry) ? true : false;

    if (!l_Garrison->CanAddTalent(l_Entry, l_Replace))
        return;

    l_Garrison->AddTalent(l_Entry, true, l_Replace);
}
