////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "CombatAI.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "Vehicle.h"
#include "ObjectAccessor.h"
#include "GarrisonMgr.hpp"
#include "GarrisonFollower.hpp"

int AggressorAI::Permissible(const Creature* creature)
{
    // have some hostile factions, it will be selected by IsHostileTo check at MoveInLineOfSight
    if (!creature->isCivilian() && !creature->IsNeutralToAll())
        return PERMIT_BASE_PROACTIVE;

    return PERMIT_BASE_NO;
}

void AggressorAI::UpdateAI(const uint32 /*diff*/)
{
    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
}

// some day we will delete these useless things
int CombatAI::Permissible(const Creature* /*creature*/)
{
    return PERMIT_BASE_NO;
}

int ArcherAI::Permissible(const Creature* /*creature*/)
{
    return PERMIT_BASE_NO;
}

int TurretAI::Permissible(const Creature* /*creature*/)
{
    return PERMIT_BASE_NO;
}

int VehicleAI::Permissible(const Creature* /*creature*/)
{
    return PERMIT_BASE_NO;
}

int BodyGuardAI::Permissible(const Creature* /*creature*/)
{
    return PERMIT_BASE_NO;
}

void CombatAI::InitializeAI()
{
    for (uint32 i = 0; i < MAX_CREATURE_SPELLS; ++i)
        if (me->m_spells[i] && sSpellMgr->GetSpellInfo(me->m_spells[i]))
            spells.push_back(me->m_spells[i]);

    CreatureAI::InitializeAI();
}

void CombatAI::Reset()
{
    events.Reset();
}

void CombatAI::JustDied(Unit* killer)
{
    for (SpellVct::iterator i = spells.begin(); i != spells.end(); ++i)
        if (AISpellInfo[*i].condition == AICOND_DIE)
            me->CastSpell(killer, *i, true);
}

void CombatAI::EnterCombat(Unit* who)
{
    for (SpellVct::iterator i = spells.begin(); i != spells.end(); ++i)
    {
        if (AISpellInfo[*i].condition == AICOND_AGGRO)
            me->CastSpell(who, *i, false);
        else if (AISpellInfo[*i].condition == AICOND_COMBAT)
            events.ScheduleEvent(*i, AISpellInfo[*i].cooldown + rand()%AISpellInfo[*i].cooldown);
    }
}

void CombatAI::UpdateAI(const uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->getVictim() && me->getVictim()->HasBreakableByDamageCrowdControlAura(me))
    {
        me->InterruptNonMeleeSpells(false);
        return;
    }

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    if (uint32 spellId = events.ExecuteEvent())
    {
        DoCast(spellId);
        events.ScheduleEvent(spellId, AISpellInfo[spellId].cooldown + rand()%AISpellInfo[spellId].cooldown);
    }
    else
        DoMeleeAttackIfReady();
}

void CombatAI::SpellInterrupted(uint32 spellId, uint32 unTimeMs)
{
    events.RescheduleEvent(spellId, unTimeMs);
}

/////////////////
//CasterAI
/////////////////

void CasterAI::InitializeAI()
{
    CombatAI::InitializeAI();

    m_attackDist = 30.0f;
    for (SpellVct::iterator itr = spells.begin(); itr != spells.end(); ++itr)
        if (AISpellInfo[*itr].condition == AICOND_COMBAT && m_attackDist > GetAISpellInfo(*itr)->maxRange)
            m_attackDist = GetAISpellInfo(*itr)->maxRange;
    if (m_attackDist == 30.0f)
        m_attackDist = MELEE_RANGE;
}

void CasterAI::EnterCombat(Unit* who)
{
    if (spells.empty())
        return;

    uint32 spell = rand()%spells.size();
    uint32 count = 0;
    for (SpellVct::iterator itr = spells.begin(); itr != spells.end(); ++itr, ++count)
    {
        if (AISpellInfo[*itr].condition == AICOND_AGGRO)
            me->CastSpell(who, *itr, false);
        else if (AISpellInfo[*itr].condition == AICOND_COMBAT)
        {
            uint32 cooldown = GetAISpellInfo(*itr)->realCooldown;
            if (count == spell)
            {
                DoCast(spells[spell]);
                cooldown += me->GetCurrentSpellCastTime(*itr);
            }
            events.ScheduleEvent(*itr, cooldown);
        }
    }
}

void CasterAI::UpdateAI(const uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->getVictim()->HasBreakableByDamageCrowdControlAura(me))
    {
        me->InterruptNonMeleeSpells(false);
        return;
    }

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    if (uint32 spellId = events.ExecuteEvent())
    {
        DoCast(spellId);
        uint32 casttime = me->GetCurrentSpellCastTime(spellId);
        events.ScheduleEvent(spellId, (casttime ? casttime : 500) + GetAISpellInfo(spellId)->realCooldown);
    }
}

//////////////
//ArcherAI
//////////////

ArcherAI::ArcherAI(Creature* c) : CreatureAI(c)
{
    if (!me->m_spells[0])
        sLog->outError(LOG_FILTER_GENERAL, "ArcherAI set for creature (entry = %u) with spell1=0. AI will do nothing", me->GetEntry());

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(me->m_spells[0]);
    m_minRange = spellInfo ? spellInfo->GetMinRange(false) : 0;

    if (!m_minRange)
        m_minRange = MELEE_RANGE;
    me->m_CombatDistance = spellInfo ? spellInfo->GetMaxRange(false) : 0;
    me->m_SightDistance = me->m_CombatDistance;
}

void ArcherAI::AttackStart(Unit* who, bool p_Melee /*= true*/)
{
    if (!who)
        return;

    if (me->IsWithinCombatRange(who, m_minRange))
    {
        if (me->Attack(who, true) && !who->IsFlying())
            me->GetMotionMaster()->MoveChase(who);
    }
    else
    {
        if (me->Attack(who, false) && !who->IsFlying())
            me->GetMotionMaster()->MoveChase(who, me->m_CombatDistance);
    }

    if (who->IsFlying())
        me->GetMotionMaster()->MoveIdle();
}

void ArcherAI::UpdateAI(const uint32 /*diff*/)
{
    if (!UpdateVictim())
        return;

    if (!me->IsWithinCombatRange(me->getVictim(), m_minRange))
        DoSpellAttackIfReady(me->m_spells[0]);
    else
        DoMeleeAttackIfReady();
}

//////////////
//TurretAI
//////////////

TurretAI::TurretAI(Creature* c) : CreatureAI(c)
{
    if (!me->m_spells[0])
        sLog->outError(LOG_FILTER_GENERAL, "TurretAI set for creature (entry = %u) with spell1=0. AI will do nothing", me->GetEntry());

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(me->m_spells[0]);
    m_minRange = spellInfo ? spellInfo->GetMinRange(false) : 0;
    me->m_CombatDistance = spellInfo ? spellInfo->GetMaxRange(false) : 0;
    me->m_SightDistance = me->m_CombatDistance;
}

bool TurretAI::CanAIAttack(const Unit* /*who*/) const
{
    // TODO: use one function to replace it
    if (!me->IsWithinCombatRange(me->getVictim(), me->m_CombatDistance)
        || (m_minRange && me->IsWithinCombatRange(me->getVictim(), m_minRange)))
        return false;
    return true;
}

void TurretAI::AttackStart(Unit* who, bool p_Melee /*= true*/)
{
    if (who)
        me->Attack(who, false);
}

void TurretAI::UpdateAI(const uint32 /*diff*/)
{
    if (!UpdateVictim())
        return;

    DoSpellAttackIfReady(me->m_spells[0]);
}

//////////////
//VehicleAI
//////////////

VehicleAI::VehicleAI(Creature* c) : CreatureAI(c), m_vehicle(c->GetVehicleKit()), m_IsVehicleInUse(false), m_HasConditions(false), m_ConditionsTimer(VEHICLE_CONDITION_CHECK_TIME)
{
    LoadConditions();
    m_DoDismiss = false;
    m_DismissTimer = VEHICLE_DISMISS_TIME;
}

//NOTE: VehicleAI::UpdateAI runs even while the vehicle is mounted
void VehicleAI::UpdateAI(const uint32 diff)
{
    CheckConditions(diff);

    if (m_DoDismiss)
    {
        if (m_DismissTimer < diff)
        {
            m_DoDismiss = false;
            me->SetVisible(false);
            me->DespawnOrUnsummon();
        }
        else
            m_DismissTimer -= diff;
    }
}

void VehicleAI::Reset()
{
    me->SetVisible(true);
}

void VehicleAI::OnCharmed(bool apply)
{
    if (m_IsVehicleInUse && !apply && m_HasConditions)//was used and has conditions
    {
        m_DoDismiss = true;//needs reset
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }
    else if (apply)
        m_DoDismiss = false;//in use again
    m_DismissTimer = VEHICLE_DISMISS_TIME;//reset timer
    m_IsVehicleInUse = apply;
}

void VehicleAI::LoadConditions()
{
    m_HasConditions = sConditionMgr->HasConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE, me->GetEntry());
}

void VehicleAI::CheckConditions(const uint32 diff)
{
    if (m_ConditionsTimer < diff)
    {
        if (m_HasConditions)
        {
            for (SeatMap::iterator itr = m_vehicle->Seats.begin(); itr != m_vehicle->Seats.end(); ++itr)
                if (Unit* passenger = ObjectAccessor::GetUnit(*m_vehicle->GetBase(), itr->second.Passenger))
                {
                    if (Player* player = passenger->ToPlayer())
                    {
                        if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE, me->GetEntry(), player, me))
                        {
                            player->ExitVehicle();
                            return;//check other pessanger in next tick
                        }
                    }
                }
        }
        m_ConditionsTimer = VEHICLE_CONDITION_CHECK_TIME;
    }
    else
        m_ConditionsTimer -= diff;
}

//////////////
//BodyGuardAI
//////////////

BodyGuardAI::BodyGuardAI(Creature* c) : CreatureAI(c)
{
    if (!me->m_spells[0])
        sLog->outError(LOG_FILTER_GENERAL, "BodyGuardAI set for creature (entry = %u) with spell1=0. AI will do nothing", me->GetEntry());

    me->SetDefaultMovementType(FOLLOW_MOTION_TYPE);
}

void BodyGuardAI::InitializeAI()
{
    SpellInfo const* l_SpellInfo = nullptr;
    for (uint32 l_It = 0; l_It < MAX_CREATURE_SPELLS; ++l_It)
        if (me->m_spells[l_It] && sSpellMgr->GetSpellInfo(me->m_spells[l_It]))
        {
            l_SpellInfo = sSpellMgr->GetSpellInfo(me->m_spells[l_It]);
            if (l_It == MAX_CREATURE_SPELLS - 1)
            {
                m_MountId = l_SpellInfo->Id;
                continue;
            }

            if (l_SpellInfo->Effects[l_SpellInfo->GetEffectIndex(SPELL_EFFECT_SCHOOL_DAMAGE)].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                l_SpellInfo->Effects[l_SpellInfo->GetEffectIndex(SPELL_EFFECT_APPLY_AURA)].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY)
                spells.push_back(me->m_spells[l_It]);
            else
                m_HealSpells.push_back(me->m_spells[l_It]);
        }

    m_MountId = 10795; ///< Summon Ivory Raptor for every Bodyguard
    CreatureAI::InitializeAI();
}

void BodyGuardAI::AttackStart(Unit* who, bool p_Melee /*= true*/)
{
    if (who)
        me->Attack(who, false);
}

void BodyGuardAI::EnterCombat(Unit* p_Attacker)
{
    m_Events.ScheduleEvent(EVENT_1, 1);
}

void BodyGuardAI::Reset()
{
    m_Events.Reset();

    if (me->GetMapId() != 1220)
        me->DespawnOrUnsummon(0);

    Unit* l_Caster = me->GetOwner();
    if (!l_Caster)
        return;

    Player* l_Player = l_Caster->ToPlayer();
    if (!l_Player)
        return;

    MS::Garrison::Manager* l_GarrisonMgr = l_Player->GetGarrison();
    if (!l_GarrisonMgr)
        return;

    MS::Garrison::GarrisonFollower* l_Follower = l_GarrisonMgr->GetCombatAllyFollower();
    if (!l_Follower)
        return;

    me->SetLevel(l_Follower->Level);
}

void BodyGuardAI::OwnerDamagedBy(Unit* /* p_Attacker */)
{
    m_Events.ScheduleEvent(EVENT_3, 1);
}

void BodyGuardAI::UpdateAI(const uint32 p_Diff)
{
    m_Events.Update(p_Diff);

    Unit* l_Caster = me->GetOwner();
    if (!l_Caster)
        return;

    Player* l_Player = l_Caster->ToPlayer();
    if (!l_Player)
        return;

    if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
        return;

    switch (m_Events.ExecuteEvent())
    {
        case EVENT_1:
            DoSpellAttackIfReady(spells[urand(0, spells.size() - 1)]);
            m_Events.ScheduleEvent(EVENT_1, 1);
            break;
        case EVENT_2:
            me->SetSpeed(UnitMoveType::MOVE_RUN, l_Player->GetSpeedRate(UnitMoveType::MOVE_RUN), true);
            me->SetSpeed(UnitMoveType::MOVE_FLIGHT, l_Player->GetSpeedRate(UnitMoveType::MOVE_FLIGHT), true);

            ///< Broken Isles Pathfinder, Part Two: Reward: Broken Isles Flying
            if (l_Player->HasAchieved(11446))
                me->SetCanFly(true, true);

            break;
        case EVENT_3:
            me->CastSpell(l_Player, m_HealSpells[urand(0, m_HealSpells.size() - 1)]);
            break;
        default:
            break;
    }

    if (l_Player->IsMounted() && !me->IsMounted())
    {
        if (m_MountId != 0)
            me->CastSpell(me, m_MountId, true);

        m_Events.ScheduleEvent(EVENT_2, 1);
    }

    if (!l_Player->IsMounted() && me->IsMounted())
        me->RemoveAura(m_MountId);

    if (me->getClass() != UnitClass::UNIT_CLASS_MAGE)
        DoMeleeAttackIfReady();
}
