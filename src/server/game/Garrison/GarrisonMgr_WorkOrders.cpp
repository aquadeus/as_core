
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
#include "HelperDefines.h"

namespace MS { namespace Garrison
{
    /// Start new work order
    uint64 Manager::StartWorkOrder(uint32 p_PlotInstanceID, uint32 p_ShipmentID)
    {
        CharShipmentEntry const* l_ShipmentEntry = sCharShipmentStore.LookupEntry(p_ShipmentID);

        if (!l_ShipmentEntry)
            return 0;

        uint32 l_MaxCompleteTime = time(0);

        for (uint32 l_I = 0; l_I < m_WorkOrders.size(); ++l_I)
        {
            if (m_WorkOrders[l_I].PlotInstanceID == p_PlotInstanceID)
                l_MaxCompleteTime = std::max<uint32>(l_MaxCompleteTime, m_WorkOrders[l_I].CompleteTime);
        }

        GarrisonWorkOrder l_WorkOrder;
        l_WorkOrder.DatabaseID      = sObjectMgr->GetNewGarrisonWorkOrderID();
        l_WorkOrder.PlotInstanceID  = p_PlotInstanceID;
        l_WorkOrder.ShipmentID      = p_ShipmentID;
        l_WorkOrder.CreationTime    = l_MaxCompleteTime;
        l_WorkOrder.CompleteTime    = l_MaxCompleteTime + l_ShipmentEntry->Duration;

        PreparedStatement * l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_WORKORDER);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, l_WorkOrder.DatabaseID);
        l_Stmt->setUInt32(l_Index++, m_ID);
        l_Stmt->setUInt32(l_Index++, l_WorkOrder.PlotInstanceID);
        l_Stmt->setUInt32(l_Index++, l_WorkOrder.ShipmentID);
        l_Stmt->setUInt32(l_Index++, l_WorkOrder.CreationTime);
        l_Stmt->setUInt32(l_Index++, l_WorkOrder.CompleteTime);

        CharacterDatabase.AsyncQuery(l_Stmt);

        m_WorkOrders.push_back(l_WorkOrder);

        return l_WorkOrder.DatabaseID;
    }
    /// Delete work order
    void Manager::DeleteWorkOrder(uint64 p_DBID)
    {
        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_WORKORDER);
        l_Stmt->setUInt32(0, p_DBID);
        CharacterDatabase.AsyncQuery(l_Stmt);

        for (std::vector<GarrisonWorkOrder>::iterator l_It = m_WorkOrders.begin(); l_It != m_WorkOrders.end(); ++l_It)
        {
            if (l_It->DatabaseID == p_DBID)
            {
                m_WorkOrders.erase(l_It);
                break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get work orders
    std::vector<GarrisonWorkOrder>& Manager::GetWorkOrders()
    {
        return m_WorkOrders;
    }
    /// Get in progress work order count
    uint32 Manager::GetWorkOrderCount(uint32 p_PlotInstanceID) const
    {
        return (uint32)std::count_if(m_WorkOrders.begin(), m_WorkOrders.end(), [p_PlotInstanceID](const GarrisonWorkOrder& p_Order) -> bool
        {
            return p_Order.PlotInstanceID == p_PlotInstanceID;
        });
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update work order
    void Manager::UpdateWorkOrders()
    {
        bool l_HasCommandBarAura = m_Owner->HasAura(GBrokenIsles::Globals::CommandBarSpell) ||
            (m_Owner->GetAreaId() == 7589 || m_Owner->GetAreaId() == 7508);

        if (l_HasCommandBarAura && GetBrokenIslesGarrison())
        {
            bool l_ForceUpdate = false;
            uint32& l_Counter = m_Owner->m_SpellHelper.GetUint32(eSpellHelpers::WorkorderUpdate);
            l_Counter++;
            if (l_Counter >= 5)
            {
                l_ForceUpdate = true;
                l_Counter = 0;
            }

            std::multimap<uint32, GBrokenIsles::WorkOrderInfo> l_WorkOrderInfos = GBrokenIsles::s_WorkOrderInfos[m_Owner->getClass()];
            for (auto const& l_Itr : GBrokenIsles::s_WorkOrderInfos[0])
                l_WorkOrderInfos.insert(std::multimap<uint32, GBrokenIsles::WorkOrderInfo>::value_type(l_Itr.first, l_Itr.second));

            for (std::pair<uint32, GBrokenIsles::WorkOrderInfo> const& l_Info : l_WorkOrderInfos)
            {
                GBrokenIsles::WorkOrderInfo const& l_Infos = l_Info.second;

                std::vector<GarrisonWorkOrder> l_WorkOrders;
                uint32 l_CompletedCount = 0;

                std::for_each(m_WorkOrders.begin(), m_WorkOrders.end(), [&l_Infos, &l_WorkOrders, &l_CompletedCount](const GarrisonWorkOrder& p_WorkOder)
                {
                    if (p_WorkOder.ShipmentID == l_Infos.WorkOrderID)
                    {
                        static const std::map<uint32, uint32> replaceShipmentId
                        {
                            {324, 285},
                            {326, 272},
                            {328, 269},
                            {325, 278},
                            {333, 275},
                            {331, 260},
                            {322, 282},
                            {327, 257},
                            {321, 254},
                        };
                        if (replaceShipmentId.find(l_Infos.WorkOrderID) != replaceShipmentId.end())
                            const_cast<GBrokenIsles::WorkOrderInfo&>(l_Infos).WorkOrderID = replaceShipmentId.find(l_Infos.WorkOrderID)->second;
                        l_WorkOrders.push_back(p_WorkOder);

                        if (p_WorkOder.CompleteTime <= time(nullptr))
                            l_CompletedCount++;
                    }
                });

                if (l_WorkOrders.empty())
                {
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::InProgress),                     false, false);
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AtLeastOneCompleted),            false, false);
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AllCompleted_LimitNotReached),   false, false);
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AllCompleted_LimitReached),      false, false);

                    continue;
                }

                uint32 l_State = GBrokenIsles::WorkOrderStates::InProgress;
                const CharShipmentEntry*            l_Shipment  = sCharShipmentStore.LookupEntry(l_Infos.WorkOrderID);
                const CharShipmentContainerEntry*   l_Container = sCharShipmentContainerStore.LookupEntry(l_Shipment ? l_Shipment->ShipmentContainerID : 0);

                if (!l_Shipment || !l_Container)
                {
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::InProgress),                     false, false);
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AtLeastOneCompleted),            false, false);
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AllCompleted_LimitNotReached),   false, false);
                    m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AllCompleted_LimitReached),      false, false);

                    continue;
                }

                if (l_CompletedCount != 0)
                {
                    l_State = GBrokenIsles::WorkOrderStates::AtLeastOneCompleted;

                    if (l_CompletedCount == l_WorkOrders.size() && l_CompletedCount != l_Container->ShipmentAmountNeeded[1])
                        l_State = GBrokenIsles::WorkOrderStates::AllCompleted_LimitNotReached;
                    else if (l_CompletedCount == l_Container->ShipmentAmountNeeded[1])
                        l_State = GBrokenIsles::WorkOrderStates::AllCompleted_LimitReached;
                }
                m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::InProgress),                     false,  l_State == GBrokenIsles::WorkOrderStates::InProgress);
                m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AtLeastOneCompleted),            false,  l_State == GBrokenIsles::WorkOrderStates::AtLeastOneCompleted);
                m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AllCompleted_LimitNotReached),   false,  l_State == GBrokenIsles::WorkOrderStates::AllCompleted_LimitNotReached);
                m_Owner->SetInPhase(SHIPMENT_PHASE_ID(l_Infos.WorkOrderID, GBrokenIsles::WorkOrderStates::AllCompleted_LimitReached),      false,  l_State == GBrokenIsles::WorkOrderStates::AllCompleted_LimitReached);
            }

            m_Owner->UpdateObjectVisibility(l_ForceUpdate);
        }
    }
}   ///< namespace Garrison
}   ///< namespace MS
