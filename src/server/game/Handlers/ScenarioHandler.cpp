////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScenarioPackets.h"
#include "WorldSession.h"

void WorldSession::HandleQueryScenarioPOI(WorldPacket& p_Packet)
{
    WorldPackets::Scenario::QueryScenarioPOI l_QueryScenarioPOI(p_Packet);
    l_QueryScenarioPOI.Read();

    WorldPackets::Scenario::ScenarioPOIs l_POIs;

    for (auto& l_TreeID : l_QueryScenarioPOI.MissingScenarioPOITreeIDs)
    {
        WorldPackets::Scenario::ScenarioPOIs::POIData l_Infos;
        l_Infos.CriteriaTreeID = l_TreeID;

        ScenarioPOIVector const* l_Poi = sObjectMgr->GetScenarioPOIVector(l_TreeID);
        if (!l_Poi)
        {
            l_POIs.PoiInfos.push_back(l_Infos);
            continue;
        }

        for (ScenarioPOIVector::const_iterator l_Itr = l_Poi->begin(); l_Itr != l_Poi->end(); ++l_Itr)
        {
            WorldPackets::Scenario::ScenarioPOIs::POIData::BlobData l_BlobData;
            l_BlobData.BlobID            = l_Itr->BlobID;
            l_BlobData.MapID             = l_Itr->MapID;
            l_BlobData.WorldMapAreaID    = l_Itr->WorldMapAreaID;
            l_BlobData.Floor             = l_Itr->Floor;
            l_BlobData.Priority          = l_Itr->Priority;
            l_BlobData.Flags             = l_Itr->Flags;
            l_BlobData.WorldEffectID     = l_Itr->WorldEffectID;
            l_BlobData.PlayerConditionID = l_Itr->PlayerConditionID;


            for (std::vector<ScenarioPOIPoint>::const_iterator l_Itr2 = l_Itr->points.begin(); l_Itr2 != l_Itr->points.end(); ++l_Itr2)
            {
                WorldPackets::Scenario::ScenarioPOIs::POIData::BlobData::POIPointData l_Points;
                l_Points.X = l_Itr2->x;
                l_Points.Y = l_Itr2->y;
                l_BlobData.Points.push_back(l_Points);
            }

            l_Infos.BlobDatas.push_back(l_BlobData);
        }

        l_POIs.PoiInfos.push_back(l_Infos);
    }

    SendPacket(l_POIs.Write());
}
