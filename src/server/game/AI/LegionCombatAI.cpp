////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "LegionCombatAI.hpp"

void LegionCombatAI::InitializeAI()
{
    ScriptedAI::InitializeAI();

    ResetEvents();
}

void LegionCombatAI::Reset()
{
    ScriptedAI::Reset();

    ResetEvents();
}

std::array<LegionCombatAI::f_Check, eCombatAIEventsChecks::EventCheckMax> m_CheckFunctions =
{{
    &LegionCombatAI::CheckMeleeNegative,
    &LegionCombatAI::CheckRangedNegative,
    &LegionCombatAI::CheckAoENegative,
    &LegionCombatAI::CheckRangedPositive,
    &LegionCombatAI::CheckAoEPositive,
    &LegionCombatAI::CheckRangedHeal,
    &LegionCombatAI::CheckAoEHeal,
    &LegionCombatAI::CheckNonTankNegative,
    &LegionCombatAI::CheckAoENonSelfPositive,
    &LegionCombatAI::CheckMeleeTopAggro,
    &LegionCombatAI::CheckAoENonSelfPositiveDispel
}};

std::list<Unit*> LegionCombatAI::CheckMeleeNegative(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    for (Unit* l_Target : CheckRangedNegative(p_EventID))
    {
        if (!me->IsWithinMeleeRange(l_Target))
            continue;

        l_PotentialTargets.push_back(l_Target);
        return l_PotentialTargets;
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckRangedNegative(uint32 p_EventID)
{
    std::list<Unit*> l_TargetList;

    float l_Radius = m_EventData[p_EventID - 1].AttackDist;
    for (HostileReference* l_Reference : me->getThreatManager().getThreatList())
    {
        Unit* l_Target = l_Reference->getTarget();
        if (!l_Target)
            continue;

        if (!me->IsValidAttackTarget(l_Target) || !me->IsWithinDist(l_Target, l_Radius) || !me->IsWithinLOSInMap(l_Target))
            continue;

        if (l_Target->HasBreakableByDamageCrowdControlAura(me))
            continue;

        l_TargetList.push_back(l_Target);
    }

    return l_TargetList;
}

std::list<Unit*> LegionCombatAI::CheckAoENegative(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    if (!CheckRangedNegative(p_EventID).empty())
    {
        l_PotentialTargets.push_back(me);
        return l_PotentialTargets;
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckRangedPositive(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;
    float l_Radius = m_EventData[p_EventID - 1].AttackDist;

    if (!l_Radius)
        return l_PotentialTargets;

    std::list<Creature*> l_CreatureList;
    me->GetCreatureListInGrid(l_CreatureList, l_Radius);
    for (Creature* l_Creature : l_CreatureList)
    {
        if (!l_Creature->isAlive())
            continue;

        if (!me->IsWithinLOSInMap(l_Creature))
            continue;

        l_PotentialTargets.push_back(l_Creature);
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckAoEPositive(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    l_PotentialTargets.push_back(me);
    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckRangedHeal(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    for (Unit* l_Creature : CheckRangedPositive(p_EventID))
    {
        if (l_Creature->GetHealthPct() > 90.0f)
            continue;

        l_PotentialTargets.push_back(l_Creature);
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckAoEHeal(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    for (Unit* l_Creature : CheckRangedPositive(p_EventID))
    {
        if (l_Creature->GetHealthPct() > 90.0f)
            continue;

        l_PotentialTargets.push_back(me);
        return l_PotentialTargets;
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckNonTankNegative(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    for (Unit* l_Target : CheckRangedNegative(p_EventID))
    {
        Player* l_Player = l_Target->ToPlayer();
        if (!l_Player || (l_Player->GetRoleForGroup() == Roles::ROLE_TANK))
            continue;

        l_PotentialTargets.push_back(l_Target);
        return l_PotentialTargets;
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckAoENonSelfPositive(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    for (Unit* l_Creature : CheckRangedPositive(p_EventID))
    {
        if (l_Creature->GetGUID() == me->GetGUID())
            continue;

        l_PotentialTargets.push_back(me);
        return l_PotentialTargets;
    }

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckMeleeTopAggro(uint32 /*p_EventID*/)
{
    std::list<Unit*> l_PotentialTargets;

    Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO);
    if (me->IsWithinMeleeRange(l_Target))
        l_PotentialTargets.push_back(l_Target);

    return l_PotentialTargets;
}

std::list<Unit*> LegionCombatAI::CheckAoENonSelfPositiveDispel(uint32 p_EventID)
{
    std::list<Unit*> l_PotentialTargets;

    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_EventData[p_EventID - 1].SpellID);
    if (!l_SpellInfo)
        return l_PotentialTargets;

    for (Unit* l_Creature : CheckRangedPositive(p_EventID))
    {
        if (l_Creature->GetGUID() == me->GetGUID())
            continue;

        DispelChargesList l_DispelList;
        l_Creature->GetDispellableAuraList(me, 1 << l_SpellInfo->Effects[EFFECT_0].MiscValue, l_DispelList);

        if (l_DispelList.empty())
            continue;

        l_PotentialTargets.push_back(me);
        return l_PotentialTargets;
    }

    return l_PotentialTargets;
}

Unit* LegionCombatAI::GetTarget(std::list<Unit*>& p_TargetList, uint32 /*p_EventID*/)
{
    if (p_TargetList.empty())
        return nullptr;

    JadeCore::RandomResizeList(p_TargetList, 1);
    return p_TargetList.front();
}

void LegionCombatAI::EnterCombat(Unit* who)
{
    if (sObjectMgr->GetLastCombatAIUpdateTime() != m_LastUpdate)
    {
        ClearEvents();

        auto l_Events = sObjectMgr->GetCombatAIEvents();
        for (auto l_Itr = l_Events.lower_bound(me->GetEntry()); l_Itr != l_Events.upper_bound(me->GetEntry()); ++l_Itr)
        {
            if (!l_Itr->second.DifficultyMask || (l_Itr->second.DifficultyMask & (1 << (me->GetMap()->GetDifficultyID() - 1))))
                AddEvent(l_Itr->second);
        }

        m_LastUpdate = sObjectMgr->GetLastCombatAIUpdateTime();
    }

    events.Reset();

    uint32 l_Idx = 0;
    for (CombatAIEventData& l_EventData : m_EventData)
    {
        m_EventContextData[l_Idx] = {};
        l_Idx++;

        if (l_EventData.DifficultyMask && !(l_EventData.DifficultyMask & (1 << (me->GetMap()->GetDifficultyID() - 1))))
            continue;

        if (l_EventData.StartMin)
            events.ScheduleEvent(l_Idx, urand(l_EventData.StartMin, l_EventData.StartMax));
    }

    Talk(eCombatAITalks::Aggro);
}

void LegionCombatAI::AttackStart(Unit* p_Victim, bool p_Melee)
{
    if (p_Victim && me->Attack(p_Victim, p_Melee))
    {
        if (p_Melee && CanChase() && !me->HasUnitState(UnitState::UNIT_STATE_CHASE))
            me->GetMotionMaster()->MoveChase(p_Victim);
        else
            me->SetFacingToObject(p_Victim);
    }
    else if (!me->IsWithinMeleeRange(p_Victim) && CanChase() && p_Melee)
    {
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveChase(p_Victim);
    }
}

void LegionCombatAI::UpdateAI(const uint32 p_Diff)
{
    UpdateOperations(p_Diff);
    events.Update(p_Diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    if (!me->isInCombat())
    {
        if (m_HasOffCombatEvents)
        {
            while (uint32 l_EventID = events.ExecuteEvent())
            {
                if (LaunchEvent(l_EventID, nullptr))
                    return;
            }
        }

        return;
    }

    Unit* l_TopAggro = me->SelectVictim();
    if (!l_TopAggro)
    {
        if (!me->IsInEvadeMode())
            EnterEvadeMode();

        return;
    }

    while (uint32 l_EventID = events.ExecuteEvent())
    {
        if (LaunchEvent(l_EventID, l_TopAggro))
            return;
    }

    if (me->GetReactState() == ReactStates::REACT_PASSIVE)
        return;

    AttackStart(l_TopAggro, true);

    if (CanAutoAttack())
        DoMeleeAttackIfReady();
}

bool LegionCombatAI::LaunchEvent(uint32 l_EventID, Unit* p_TopAggro /*= nullptr*/)
{
    if (m_EventData.size() < l_EventID)
        return false;

    CombatAIEventData& l_Event = m_EventData[l_EventID - 1];
    std::list<Unit*> l_Targets;

    if (!AdditionalChecks(l_EventID))
    {
        if (l_Event.RepeatFail)
            events.ScheduleEvent(l_EventID, l_Event.RepeatFail);

        return false;
    }

    l_Targets = (this->*m_CheckFunctions[l_Event.EventCheck])(l_EventID);
    if (l_Targets.empty())
    {
        if (l_Event.RepeatFail)
            events.ScheduleEvent(l_EventID, l_Event.RepeatFail);

        return false;
    }

    Unit* l_Target;
    if (!(l_Target = GetTarget(l_Targets, l_EventID)))
    {
        if (l_Event.RepeatFail)
            events.ScheduleEvent(l_EventID, l_Event.RepeatFail);

        return false;
    }

    if (p_TopAggro && me->GetReactState() != ReactStates::REACT_PASSIVE)
        AttackStart(p_TopAggro);

    if (!ExecuteEvent(l_EventID, l_Target))
    {
        if (l_Event.RepeatFail)
            events.ScheduleEvent(l_EventID, l_Event.RepeatFail);

        return false;
    }

    if (l_Event.RepeatMin)
        events.ScheduleEvent(l_EventID, urand(l_Event.RepeatMin, l_Event.RepeatMax));

    return true;
}

bool LegionCombatAI::ExecuteEvent(uint32 p_EventID, Unit* p_Target)
{
    CombatAIEventData const& l_Event = m_EventData[p_EventID - 1];

    if (l_Event.EventFlags & eEventFlags::DontCastIfTankIsGone)
    {
        Unit* l_Victim = me->getVictim();

        if (l_Victim && !me->IsWithinDist(l_Victim, l_Event.AttackDist))
            return false;
    }

    if (l_Event.EventFlags & eEventFlags::DisableTurn)
    {
        me->SetFacingToObject(p_Target);
        me->NearTeleportTo(*me);
        AddTimedDelayedOperation(50, [this]() -> void
        {
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
        });
    }

    if (l_Event.EventFlags & eEventFlags::DisableMove)
    {
        me->AddUnitState(UnitState::UNIT_STATE_ROOT);
        me->StopMoving();
    }

    me->CastSpell(p_Target, l_Event.SpellID, TriggerCastFlags(TRIGGERED_IGNORE_POWER_AND_REAGENT_COST));
    return true;
}

void LegionCombatAI::OnSpellFinished(SpellInfo const* p_SpellInfo)
{
    for (CombatAIEventData& l_Event : m_EventData)
    {
        if (l_Event.SpellID == p_SpellInfo->Id)
        {
            if (l_Event.EventFlags & eEventFlags::DisableTurn)
            {
                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                });
            }

            if (l_Event.EventFlags & eEventFlags::DisableMove)
                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);

            if (l_Event.EventFlags & eEventFlags::CastOnDeath)
                m_CanDie = true;

            if (l_Event.EventFlags & eEventFlags::DespawnAfterCast)
                me->DespawnOrUnsummon(1);

            if (l_Event.EventFlags & eEventFlags::DieAfterCast)
                me->Kill(me);

            if (l_Event.EventFlags & eEventFlags::CastAfterSpell)
                events.ScheduleEvent(l_Event.AttackDist, 100);
        }
    }
}

void LegionCombatAI::DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo)
{
    for (uint32 l_Idx = 0; l_Idx < m_EventData.size(); ++l_Idx)
    {
        if (m_EventData[l_Idx].EventFlags & eEventFlags::CastOnDeath)
        {
            if (p_Damage > me->GetHealth() && (p_Source->GetGUID() != me->GetGUID()) && !m_CanDie)
            {
                p_Damage = me->GetHealth() - 1;
                if (!m_EventContextData[l_Idx].Prevent)
                {
                    me->InterruptNonMeleeSpells(true);
                    LaunchEvent(l_Idx + 1);
                    m_EventContextData[l_Idx].Prevent = true;
                }
            }
        }

        if (m_EventData[l_Idx].EventFlags & eEventFlags::CastUnderHealthPct)
        {
            if (!m_EventContextData[l_Idx].Prevent && (me->GetHealthPct() < m_EventData[l_Idx].AttackDist))
            {
                events.ScheduleEvent(l_Idx + 1, 0);
                m_EventContextData[l_Idx].Prevent = true;
            }
        }
    }
}

void LegionCombatAI::JustDied(Unit* p_Killer)
{
    Talk(eCombatAITalks::Death);
    for (uint32 l_Idx = 0; l_Idx < m_EventData.size(); ++l_Idx)
    {
        uint32 l_EventFlags = m_EventData[l_Idx].EventFlags;

        if (l_EventFlags & eEventFlags::CastAfterDeath)
            LaunchEvent(l_Idx + 1);
        else if (l_EventFlags & eEventFlags::DespawnSummonsAfterDie)
            me->DespawnCreaturesInArea(m_EventData[l_Idx].SpellID, m_EventData[l_Idx].AttackDist);
        else if (l_EventFlags & eEventFlags::DespawnAreaTriggers)
            me->DespawnAreaTriggersInArea(m_EventData[l_Idx].SpellID, m_EventData[l_Idx].AttackDist);
    }
}

void LegionCombatAI::ResetEvents()
{
    ClearEvents();

    auto l_Events = sObjectMgr->GetCombatAIEvents();
    for (auto l_Itr = l_Events.lower_bound(me->GetEntry()); l_Itr != l_Events.upper_bound(me->GetEntry()); ++l_Itr)
        AddEvent(l_Itr->second);

    events.Reset();
    uint32 l_Idx = 0;
    for (CombatAIEventData& l_EventData : m_EventData)
    {
        m_EventContextData[l_Idx] = {};
        l_Idx++;

        if (l_EventData.StartMin && (l_EventData.EventFlags & eEventFlags::DontCheckInCombat))
        {
            m_HasOffCombatEvents = true;
            events.ScheduleEvent(l_Idx, urand(l_EventData.StartMin, l_EventData.StartMax));
        }
    }
}
