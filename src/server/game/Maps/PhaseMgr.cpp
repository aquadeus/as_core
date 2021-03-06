////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "PhaseMgr.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "ConditionMgr.h"
#ifndef CROSS
#include "GarrisonMgr.hpp"
#endif /* not CROSS */

//////////////////////////////////////////////////////////////////
// Updating

PhaseMgr::PhaseMgr(Player* _player) : player(_player), phaseData(_player), _UpdateFlags(0)
{
    _PhaseDefinitionStore = sObjectMgr->GetPhaseDefinitionStore();
    _SpellPhaseStore = sObjectMgr->GetSpellPhaseStore();
}

void PhaseMgr::Update()
{
    if (IsUpdateInProgress())
        return;

    if (_UpdateFlags & PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED)
        phaseData.SendPhaseshiftToPlayer();

    if (_UpdateFlags & PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED)
        phaseData.SendPhaseMaskToPlayer();

    _UpdateFlags = 0;
}

void PhaseMgr::ForceMapShiftUpdate()
{
    phaseData.SendPhaseshiftToPlayer();
}

void PhaseMgr::RemoveUpdateFlag(PhaseUpdateFlag updateFlag)
{
    _UpdateFlags &= ~updateFlag;

    if (updateFlag == PHASE_UPDATE_FLAG_ZONE_UPDATE)
    {
        // Update zone changes
        if (phaseData.HasActiveDefinitions())
            phaseData.ResetDefinitions();

        _UpdateFlags |= (PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED | PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED);

        if (_PhaseDefinitionStore->find(player->GetZoneId()) != _PhaseDefinitionStore->end())
            Recalculate();
    }

    Update();
}

/////////////////////////////////////////////////////////////////
// Notifier

void PhaseMgr::NotifyConditionChanged(PhaseUpdateData const& updateData)
{
    if (NeedsPhaseUpdateWithData(updateData))
    {
        Recalculate();
        Update();
    }
}

//////////////////////////////////////////////////////////////////
// Phasing Definitions

void PhaseMgr::Recalculate()
{
    if (phaseData.HasActiveDefinitions())
    {
        phaseData.ResetDefinitions();
        _UpdateFlags |= (PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED | PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED);
    }

    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr != _PhaseDefinitionStore->end())
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            if (CheckDefinition(&(*phase)) && !player->HasTimingForPhase(phase->phaseId))
            {
                phaseData.AddPhaseDefinition(&(*phase));

                if (phase->phasemask)
                    _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

                if (phase->phaseId || phase->terrainswapmap || phase->uiworldmaparea)
                    _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

                if (phase->IsLastDefinition())
                    break;
            }
        }
    }

#ifndef CROSS
    if (player->GetGarrison() && (player->GetMapId() == MS::Garrison::GDraenor::Globals::BaseMap || player->IsInDraenorGarrison()))
        _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;
#endif
}

inline bool PhaseMgr::CheckDefinition(PhaseDefinition const* phaseDefinition)
{
    return sConditionMgr->IsObjectMeetPhaseCondition(phaseDefinition->zoneId, phaseDefinition->entry, player);
}

bool PhaseMgr::NeedsPhaseUpdateWithData(PhaseUpdateData const updateData) const
{
    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr != _PhaseDefinitionStore->end())
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            if (ConditionContainer const* conditionList = sConditionMgr->GetConditionsForPhaseDefinition(phase->zoneId, phase->entry))
            {
                for (ConditionContainer::const_iterator condition = conditionList->begin(); condition != conditionList->end(); ++condition)
                    if (updateData.IsConditionRelated(*condition))
                        return true;
            }
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////
// Auras

void PhaseMgr::RegisterPhasingAuraEffect(AuraEffect const* auraEffect)
{
    PhaseInfo phaseInfo;

    if (auraEffect->GetMiscValue())
    {
        _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
        phaseInfo.phasemask = auraEffect->GetMiscValue();
    }
    else
    {
        SpellPhaseStore::const_iterator itr = _SpellPhaseStore->find(auraEffect->GetId());
        if (itr != _SpellPhaseStore->end())
        {
            if (itr->second.phasemask)
            {
                _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
                phaseInfo.phasemask = itr->second.phasemask;
            }

            if (itr->second.terrainswapmap)
                phaseInfo.terrainswapmap = itr->second.terrainswapmap;

            if (itr->second.uiworldmaparea)
                phaseInfo.uiworldmaparea = itr->second.uiworldmaparea;
        }
    }

    phaseInfo.phaseId = auraEffect->GetMiscValueB();

    if (phaseInfo.NeedsClientSideUpdate())
        _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

    phaseData.AddAuraInfo(auraEffect->GetId(), phaseInfo);

    Update();
}

void PhaseMgr::UnRegisterPhasingAuraEffect(AuraEffect const* auraEffect)
{
    _UpdateFlags |= phaseData.RemoveAuraInfo(auraEffect->GetId());

    Update();
}

//////////////////////////////////////////////////////////////////
// Commands

void PhaseMgr::SendDebugReportToPlayer(Player* const debugger)
{
    ChatHandler(debugger).PSendSysMessage(LANG_PHASING_REPORT_STATUS, player->GetName(), player->GetZoneId(), player->getLevel(), player->GetTeamId(), _UpdateFlags);

    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr == _PhaseDefinitionStore->end())
        ChatHandler(debugger).PSendSysMessage(LANG_PHASING_NO_DEFINITIONS, player->GetZoneId());
    else
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            if (CheckDefinition(&(*phase)))
                ChatHandler(debugger).PSendSysMessage(LANG_PHASING_SUCCESS, phase->entry, phase->IsNegatingPhasemask() ? "negated Phase" : "Phase", phase->phasemask);
            else
                ChatHandler(debugger).PSendSysMessage(LANG_PHASING_FAILED, phase->phasemask, phase->entry, phase->zoneId);

            if (phase->IsLastDefinition())
            {
                ChatHandler(debugger).PSendSysMessage(LANG_PHASING_LAST_PHASE, phase->phasemask, phase->entry, phase->zoneId);
                break;
            }
        }
    }

    ChatHandler(debugger).PSendSysMessage(LANG_PHASING_LIST, phaseData._PhasemaskThroughDefinitions, phaseData._PhasemaskThroughAuras, 0);

    ChatHandler(debugger).PSendSysMessage(LANG_PHASING_PHASEMASK, phaseData.GetPhaseMaskForSpawn(), player->GetPhaseMask());
}

//////////////////////////////////////////////////////////////////
// Phase Data

uint32 PhaseData::GetCurrentPhasemask() const
{
    if (player->isGameMaster())
        return PHASEMASK_ANYWHERE;

    return GetPhaseMaskForSpawn();
}

inline uint32 PhaseData::GetPhaseMaskForSpawn() const
{
    uint32 const phase = (_PhasemaskThroughDefinitions | _PhasemaskThroughAuras | player->GetZonePhaseMask());
    return (phase ? phase : PHASEMASK_NORMAL);
}

void PhaseData::SendPhaseMaskToPlayer()
{
    // Server side update
    uint32 const phasemask = GetCurrentPhasemask();
    if (player->GetPhaseMask() == phasemask)
        return;

    player->SetPhaseMask(phasemask, false);

    if (player->IsVisible())
        player->UpdateObjectVisibility();
}

void PhaseData::SendPhaseshiftToPlayer()
{
    // Client side update
    std::set<uint32> l_PhaseIDs;
    std::set<uint32> l_TerrainSwaps;
    std::set<uint32> l_InactiveTerrainSwap;
    std::set<uint32> l_UIWorldMapAreaIDSwap;;

    /// 1 - Check phases, terrain swaps and ui world map area id swap  the client is in
    for (PhaseInfoContainer::const_iterator l_IT = spellPhaseInfo.begin(); l_IT != spellPhaseInfo.end(); ++l_IT)
    {
        if (l_IT->second.terrainswapmap)
            l_TerrainSwaps.insert(l_IT->second.terrainswapmap);

        if (l_IT->second.phaseId)
            l_PhaseIDs.insert(l_IT->second.phaseId);

        if (l_IT->second.uiworldmaparea)
            l_UIWorldMapAreaIDSwap.insert(l_IT->second.uiworldmaparea);
    }

    for (std::list<PhaseDefinition const*>::const_iterator l_IT = activePhaseDefinitions.begin(); l_IT != activePhaseDefinitions.end(); ++l_IT)
    {
        if ((*l_IT)->phaseId)
            l_PhaseIDs.insert((*l_IT)->phaseId);

        if ((*l_IT)->terrainswapmap)
            l_TerrainSwaps.insert((*l_IT)->terrainswapmap);

        if ((*l_IT)->uiworldmaparea)
            l_UIWorldMapAreaIDSwap.insert((*l_IT)->uiworldmaparea);
    }

    /// 2 - Determine the added phases and the removed phases
    std::set<uint32> l_RemovedPhases;
    std::set<uint32> l_AddedPhases;

    for (uint32 l_PhaseID : l_PhaseIDs)
    {
        if (!m_ActivePhasesIDs.count(l_PhaseID))
            l_AddedPhases.insert(l_PhaseID);
    }

    for (uint32 l_PhaseID : m_ActivePhasesIDs)
    {
        if (!l_PhaseIDs.count(l_PhaseID))
            l_RemovedPhases.insert(l_PhaseID);
    }

    /// 3 - Register the new phases and delete the old ones
    for (uint32 l_NewPhase : l_AddedPhases)
    {
        AddActivePhaseID(l_NewPhase);
        player->SetInPhase(l_NewPhase, false, true);

        for (Unit* l_Control : player->m_Controlled)
            l_Control->SetInPhase(l_NewPhase, false, true);
    }

    for (uint32 l_ToRemovePhase : l_RemovedPhases)
    {
        RemoveActivePhaseID(l_ToRemovePhase);
        player->SetInPhase(l_ToRemovePhase, false, false);

        for (Unit* l_Control : player->m_Controlled)
            l_Control->SetInPhase(l_ToRemovePhase, false, false);
    }

#ifndef CROSS
    if (player->GetDraenorGarrison() && player->GetDraenorGarrison()->GetGarrisonSiteLevelEntry())
    {
        if (player->GetMapId() == MS::Garrison::GDraenor::Globals::BaseMap || player->IsInDraenorGarrison())
        {
            l_TerrainSwaps.insert(player->GetDraenorGarrison()->GetGarrisonSiteLevelEntry()->MapID);
            player->GetDraenorGarrison()->GetUIWorldMapAreaSwaps(l_UIWorldMapAreaIDSwap);
        }

        if (player->GetDraenorGarrison()->HasShipyard() && (player->GetMapId() == MS::Garrison::GDraenor::Globals::BaseMap || player->IsInShipyard() || player->IsInDraenorGarrison()))
        {
            /// Blizz are weird people
            l_TerrainSwaps.insert(MS::Garrison::GDraenor::ShipyardMapId::Alliance);
            l_TerrainSwaps.insert(MS::Garrison::GDraenor::ShipyardMapId::Horde);
            player->GetDraenorGarrison()->GetShipyardUIWorldMapAreaIDSwaps(l_UIWorldMapAreaIDSwap);
        }
    }
#endif

    player->GetSession()->SendSetPhaseShift(l_PhaseIDs, l_TerrainSwaps, l_InactiveTerrainSwap, l_UIWorldMapAreaIDSwap);

    if (player->IsVisible() && player->IsInWorld())
        player->UpdateObjectVisibility();

    for (Unit* l_Control : player->m_Controlled)
    {
        if (l_Control->IsVisible() && l_Control->IsInWorld())
            l_Control->UpdateObjectVisibility();
    }
}

void PhaseData::AddPhaseDefinition(PhaseDefinition const* phaseDefinition)
{
    if (phaseDefinition->IsOverwritingExistingPhases())
    {
        activePhaseDefinitions.clear();
        _PhasemaskThroughDefinitions = phaseDefinition->phasemask;
    }
    else
    {
        if (phaseDefinition->IsNegatingPhasemask())
            _PhasemaskThroughDefinitions &= ~phaseDefinition->phasemask;
        else
            _PhasemaskThroughDefinitions |= phaseDefinition->phasemask;
    }

    activePhaseDefinitions.push_back(phaseDefinition);
}

void PhaseData::AddAuraInfo(uint32 const spellId, PhaseInfo phaseInfo)
{
    if (phaseInfo.phasemask)
        _PhasemaskThroughAuras |= phaseInfo.phasemask;

    spellPhaseInfo[spellId] = phaseInfo;
}

uint32 PhaseData::RemoveAuraInfo(uint32 const spellId)
{
    PhaseInfoContainer::const_iterator rAura = spellPhaseInfo.find(spellId);
    if (rAura != spellPhaseInfo.end())
    {
        uint32 updateflag = 0;

        if (rAura->second.NeedsClientSideUpdate())
            updateflag |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

        if (rAura->second.NeedsServerSideUpdate())
        {
            _PhasemaskThroughAuras = 0;

            updateflag |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

            spellPhaseInfo.erase(rAura);

            for (PhaseInfoContainer::const_iterator itr = spellPhaseInfo.begin(); itr != spellPhaseInfo.end(); ++itr)
                _PhasemaskThroughAuras |= itr->second.phasemask;
        }

        return updateflag;
    }
    else
        return 0;
}

//////////////////////////////////////////////////////////////////
// Phase Update Data

void PhaseUpdateData::AddQuestUpdate(uint32 const questId)
{
    AddConditionType(CONDITION_QUESTREWARDED);
    AddConditionType(CONDITION_QUESTTAKEN);
    AddConditionType(CONDITION_QUEST_COMPLETE);
    AddConditionType(CONDITION_QUEST_NONE);
    AddConditionType(CONDITION_QUESTSTATE);

    _questId = questId;
}

bool PhaseUpdateData::IsConditionRelated(Condition const* condition) const
{
    switch (condition->ConditionType)
    {
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        case CONDITION_QUEST_COMPLETE:
        case CONDITION_QUEST_NONE:
        case CONDITION_QUESTSTATE:
            return condition->ConditionValue1 == _questId && ((1 << condition->ConditionType) & _conditionTypeFlags);
        default:
            return (1 << condition->ConditionType) & _conditionTypeFlags;
    }
}

bool PhaseMgr::IsConditionTypeSupported(ConditionTypes const conditionType)
{
    switch (conditionType)
    {
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        case CONDITION_QUEST_COMPLETE:
        case CONDITION_QUEST_NONE:
        case CONDITION_TEAM:
        case CONDITION_CLASS:
        case CONDITION_RACE:
        case CONDITION_INSTANCE_DATA:
        case CONDITION_LEVEL:
        case CONDITION_QUESTSTATE:
            return true;
        /// Others conditions are supported as well but won't trigger a phase update when they changes
        default:
            return true;
    }
}
