////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "UnitAI.h"
#include "Player.h"
#include "Creature.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "Spell.h"
#include "CreatureAIImpl.h"

void UnitAI::AttackStart(Unit* victim, bool p_Melee /*= true*/)
{
    if (victim && me->Attack(victim, true))
        me->GetMotionMaster()->MoveChase(victim);
}

void UnitAI::AttackStartCaster(Unit* victim, float dist)
{
    if (victim && me->Attack(victim, false))
        me->GetMotionMaster()->MoveChase(victim, dist);
}

void UnitAI::DoMeleeAttackIfReady()
{
    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    Unit* l_Victim = me->getVictim();

    if (l_Victim != nullptr && !l_Victim->isAlive())
        return;

    if (!me->IsWithinMeleeRange(l_Victim))
        return;

    /// Make sure our attack is ready and we aren't currently casting before checking distance
    if (me->isAttackReady())
    {
        me->AttackerStateUpdate(l_Victim);
        me->resetAttackTimer();
    }

    if (me->haveOffhandWeapon() && me->isAttackReady(WeaponAttackType::OffAttack))
    {
        me->AttackerStateUpdate(l_Victim, WeaponAttackType::OffAttack);
        me->resetAttackTimer(WeaponAttackType::OffAttack);
    }
}

bool UnitAI::DoSpellAttackIfReady(uint32 spell, TriggerCastFlags triggerFlags)
{
    if (me->HasUnitState(UNIT_STATE_CASTING))
        return true;

    if (me->isAttackReady())
    {
        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell))
        {
            if (me->IsWithinCombatRange(me->getVictim(), spellInfo->GetMaxRange(false)))
            {
                me->CastSpell(me->getVictim(), spell, triggerFlags);
                me->resetAttackTimer();
            }
            else
                return false;
        }
        else
            return false;
    }
    return true;
}

Unit* UnitAI::SelectTarget(SelectAggroTarget targetType, uint32 position, float dist, bool playerOnly, int32 aura)
{
    return SelectTarget(targetType, position, DefaultTargetSelector(me, dist, playerOnly, aura));
}

Unit* UnitAI::SelectTarget(SelectAggroTarget p_TargetType, uint32 p_Position, float p_Dist, bool p_Player, std::vector<int32> p_ExcludeAuras)
{
    return SelectTarget(p_TargetType, p_Position, DefaultTargetSelector(me, p_Dist, p_Player, p_ExcludeAuras));
}

void UnitAI::SelectTargetList(std::list<Unit*>& targetList, uint32 num, SelectAggroTarget targetType, float dist, bool playerOnly, int32 aura)
{
    SelectTargetList(targetList, DefaultTargetSelector(me, dist, playerOnly, aura), num, targetType);
}

void UnitAI::SelectTargetList(std::list<Unit*>& p_TargetList, uint32 p_Num, SelectAggroTarget p_TargetType, float p_Dist, bool p_Player, std::vector<int32> p_ExcludeAuras)
{
    SelectTargetList(p_TargetList, DefaultTargetSelector(me, p_Dist, p_Player, p_ExcludeAuras), p_Num, p_TargetType);
}

void UnitAI::SelectRangedTargetList(std::list<Player*>& p_PlayerList, bool p_AllowHeal /*= true*/, int32 p_CheckAura /*= 0*/) const
{
    std::list<HostileReference*> const& l_ThreatList = me->getThreatManager().getThreatList();
    if (l_ThreatList.empty())
        return;

    std::list<Player*> l_TargetList;
    for (HostileReference* l_Iter : l_ThreatList)
    {
        if (l_Iter->getTarget()->IsPlayer())
            l_TargetList.push_back(l_Iter->getTarget()->ToPlayer());
    }

    if (l_TargetList.empty())
        return;

    l_TargetList.remove_if([&](Player* p_Player) -> bool
    {
        if (!p_Player->IsRangedDamageDealer(p_AllowHeal))
            return true;

        if (p_CheckAura)
        {
            if (p_CheckAura > 0)
            {
                if (!p_Player->HasAura(p_CheckAura))
                    return true;
            }
            else
            {
                if (p_Player->HasAura(-p_CheckAura))
                    return true;
            }
        }

        return false;
    });

    if (l_TargetList.empty())
        return;

    JadeCore::Containers::RandomResizeList(l_TargetList, l_TargetList.size());
    p_PlayerList = l_TargetList;
}

Player* UnitAI::SelectRangedTarget(bool p_AllowHeal /*= true*/, int32 p_CheckAura /*= 0*/) const
{
    std::list<HostileReference*> const& l_ThreatList = me->getThreatManager().getThreatList();
    if (l_ThreatList.empty())
        return nullptr;

    std::list<Player*> l_TargetList;
    for (HostileReference* l_Iter : l_ThreatList)
    {
        if (l_Iter->getTarget()->IsPlayer())
            l_TargetList.push_back(l_Iter->getTarget()->ToPlayer());
    }

    if (l_TargetList.empty())
        return nullptr;

    l_TargetList.remove_if([&](Player* p_Player) -> bool
    {
        if (!p_Player->IsRangedDamageDealer(p_AllowHeal))
            return true;

        if (p_CheckAura)
        {
            if (p_CheckAura > 0)
            {
                if (!p_Player->HasAura(p_CheckAura))
                    return true;
            }
            else
            {
                if (p_Player->HasAura(-p_CheckAura))
                    return true;
            }
        }

        return false;
    });

    if (l_TargetList.empty())
        return nullptr;

    JadeCore::Containers::RandomResizeList(l_TargetList, 1);

    return l_TargetList.front();
}

Player* UnitAI::SelectMeleeTarget(bool p_AllowTank /*= false*/) const
{
    std::list<HostileReference*> const& l_ThreatList = me->getThreatManager().getThreatList();
    if (l_ThreatList.empty())
        return nullptr;

    std::list<Player*> l_TargetList;
    for (HostileReference* l_Iter : l_ThreatList)
    {
        if (l_Iter->getTarget()->IsPlayer())
            l_TargetList.push_back(l_Iter->getTarget()->ToPlayer());
    }

    if (l_TargetList.empty())
        return nullptr;

    l_TargetList.remove_if([this, p_AllowTank](Player* p_Player) -> bool
    {
        if (!p_Player->IsMeleeDamageDealer(p_AllowTank))
            return true;

        return false;
    });

    if (l_TargetList.empty())
        return nullptr;

    JadeCore::Containers::RandomResizeList(l_TargetList, 1);

    return l_TargetList.front();
}

Player* UnitAI::SelectPlayerTarget(eTargetTypeMask p_TypeMask, std::vector<int32> p_ExcludeAuras /*= { }*/, float p_Dist /*= 0.0f*/, std::vector<uint64> p_ExcludeGuids /*= { }*/, bool IsFallback /*= false*/)
{
    std::list<HostileReference*> const& l_ThreatList = me->getThreatManager().getThreatList();
    if (l_ThreatList.empty())
        return nullptr;

    std::list<Player*> l_TargetList;
    for (HostileReference* l_Iter : l_ThreatList)
    {
        if (l_Iter->getTarget()->IsPlayer())
            l_TargetList.push_back(l_Iter->getTarget()->ToPlayer());
    }

    if (l_TargetList.empty())
        return nullptr;

    l_TargetList.remove_if([&](Player* p_Player) -> bool
    {
        /// Remove dead targets or Spirit of Redemption
        if (!p_Player->isAlive() || p_Player->HasAura(27827))
            return true;

        /// Doesn't allow tanks
        if (!(p_TypeMask & eTargetTypeMask::TypeMaskTank))
        {
            if (p_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                return true;
        }

        /// Doesn't allow melee
        if (!(p_TypeMask & eTargetTypeMask::TypeMaskMelee))
        {
            if (p_Player->IsMeleeDamageDealer(false, false))
                return true;
        }

        /// Doesn't allow healer
        if (!(p_TypeMask & eTargetTypeMask::TypeMaskHealer))
        {
            if (p_Player->GetRoleForGroup() == Roles::ROLE_HEALER)
                return true;
        }

        /// Doesn't allow range
        if (!(p_TypeMask & eTargetTypeMask::TypeMaskRanged))
        {
            if (p_Player->IsRangedDamageDealer(false))
                return true;
        }

        /// Doesn't allow mw monks and holy pals
        if (p_TypeMask == eTargetTypeMask::TypeMaskAllRanged)
        {
            if (p_Player->GetActiveSpecializationID() == SpecIndex::SPEC_MONK_MISTWEAVER || p_Player->GetActiveSpecializationID() == SpecIndex::SPEC_PALADIN_HOLY)
                return true;
        }

        /// Doesn't allow healers beside mw monks and holy pals
        if (p_TypeMask == eTargetTypeMask::TypeMaskAllMelee)
        {
            if (p_Player->GetActiveSpecializationID() != SpecIndex::SPEC_MONK_MISTWEAVER && p_Player->GetActiveSpecializationID() != SpecIndex::SPEC_PALADIN_HOLY)
                return true;
        }

        for (int32 l_Aura : p_ExcludeAuras)
        {
            if (l_Aura > 0)
            {
                if (!p_Player->HasAura(l_Aura))
                    return true;
            }
            else
            {
                if (p_Player->HasAura(-l_Aura))
                    return true;
            }
        }

        if (p_Dist > 0.0f && !me->IsWithinCombatRange(p_Player, p_Dist))
            return true;

        if (p_Dist < 0.0f && me->IsWithinCombatRange(p_Player, -p_Dist))
            return true;

        if (!p_ExcludeGuids.empty())
        {
            for (uint64& l_Guid : p_ExcludeGuids)
            {
                if (p_Player->GetGUID() == l_Guid)
                    return true;
            }
        }

        return false;
    });

    if (l_TargetList.empty())
    {
        if (!IsFallback)
        {
            if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, p_ExcludeAuras, p_Dist, p_ExcludeGuids, true))
                return l_Target;
            else if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll, p_ExcludeAuras, p_Dist, p_ExcludeGuids, true))
                return l_Target;
        }

        return nullptr;
    }

    JadeCore::Containers::RandomResizeList(l_TargetList, 1);

    return l_TargetList.front();
}

Player* UnitAI::SelectMainTank() const
{
    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
    if (l_ThreatList.empty())
        return nullptr;

    l_ThreatList.remove_if([this](HostileReference* p_HostileRef) -> bool
    {
        Player* l_Player = p_HostileRef->getTarget()->ToPlayer();
        if (l_Player == nullptr)
            return true;

        if (l_Player->GetRoleForGroup() != Roles::ROLE_TANK)
            return true;

        return false;
    });

    if (l_ThreatList.empty())
        return nullptr;

    l_ThreatList.sort(JadeCore::ThreatOrderPred());

    return l_ThreatList.front()->getTarget()->ToPlayer();
}

Player* UnitAI::SelectOffTank() const
{
    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
    if (l_ThreatList.empty())
        return nullptr;

    l_ThreatList.remove_if([this](HostileReference* p_HostileRef) -> bool
    {
        Player* l_Player = p_HostileRef->getTarget()->ToPlayer();
        if (l_Player == nullptr)
            return true;

        if (l_Player->GetRoleForGroup() != Roles::ROLE_TANK)
            return true;

        return false;
    });

    if (l_ThreatList.empty())
        return nullptr;

    l_ThreatList.sort(JadeCore::ThreatOrderPred());

    return l_ThreatList.back()->getTarget()->ToPlayer();
}

float UnitAI::DoGetSpellMaxRange(uint32 spellId, bool positive)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    return spellInfo ? spellInfo->GetMaxRange(positive) : 0;
}

void UnitAI::DoAddAuraToAllHostilePlayers(uint32 spellid)
{
    if (me->isInCombat())
    {
        std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
        for (std::list<HostileReference*>::iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        {
            if (Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                if (unit->IsPlayer())
                    me->AddAura(spellid, unit);
        }
    }
}

void UnitAI::DoCastToAllHostilePlayers(uint32 spellid, bool triggered)
{
    if (me->isInCombat())
    {
        std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
        for (std::list<HostileReference*>::iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        {
            if (Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                if (unit->IsPlayer())
                    me->CastSpell(unit, spellid, triggered);
        }
    }
}

void UnitAI::DoCastToAllPlayers(uint32 p_SpellID, bool p_Triggered /*= false*/)
{
    Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            DoCast(l_Player, p_SpellID, p_Triggered);
    }
}

void UnitAI::DoCast(uint32 spellId, bool triggered /*= false*/)
{
    Unit* target = NULL;
    //sLog->outError(LOG_FILTER_GENERAL, "aggre %u %u", spellId, (uint32)AISpellInfo[spellId].target);
    switch (AISpellInfo[spellId].target)
    {
        default:
        case AITARGET_SELF:
           target = me;
           break;
        case AITARGET_VICTIM:
            target = me->getVictim();
            break;
        case AITARGET_ENEMY:
        {
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
            {
                bool playerOnly = spellInfo->AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0, spellInfo->GetMaxRange(false), playerOnly);
            }
            break;
        }
        case AITARGET_ALLY:
            target = me;
            break;
        case AITARGET_BUFF:
            target = me;
                break;
        case AITARGET_DEBUFF:
        {
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
            {
                bool playerOnly = spellInfo->AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                float range = spellInfo->GetMaxRange(false);

                DefaultTargetSelector targetSelector(me, range, playerOnly, -(int32)spellId);
                if (!(spellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_VICTIM)
                    && targetSelector(me->getVictim()))
                    target = me->getVictim();
                else
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0, targetSelector);
            }
            break;
        }
    }

    if (target)
        me->CastSpell(target, spellId, triggered);
}

#define UPDATE_TARGET(a) {if (AIInfo->target<a) AIInfo->target=a;}

AISpellInfoType* UnitAI::AISpellInfo = nullptr;

void UnitAI::FillAISpellInfo()
{
    if (AISpellInfo)
        delete[] AISpellInfo;

    AISpellInfo = new AISpellInfoType[sSpellMgr->GetSpellInfoStoreSize()];

    AISpellInfoType* AIInfo = AISpellInfo;
    const SpellInfo* spellInfo;

    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i, ++AIInfo)
    {
        spellInfo = sSpellMgr->GetSpellInfo(i);
        if (!spellInfo)
            continue;

        if (spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD)
            AIInfo->condition = AICOND_DIE;
        else if (spellInfo->IsPassive() || spellInfo->GetDuration() == -1)
            AIInfo->condition = AICOND_AGGRO;
        else
            AIInfo->condition = AICOND_COMBAT;

        if (AIInfo->cooldown < spellInfo->RecoveryTime)
            AIInfo->cooldown = spellInfo->RecoveryTime;

        if (!spellInfo->GetMaxRange(false))
            UPDATE_TARGET(AITARGET_SELF)
        else
        {
            for (uint8 j = 0; j < spellInfo->EffectCount; ++j)
            {
                uint32 targetType = spellInfo->Effects[j].TargetA.GetTarget();

                if (targetType == TARGET_UNIT_TARGET_ENEMY
                    || targetType == TARGET_DEST_TARGET_ENEMY)
                    UPDATE_TARGET(AITARGET_VICTIM)
                else if (targetType == TARGET_UNIT_DEST_AREA_ENEMY)
                    UPDATE_TARGET(AITARGET_ENEMY)

                if (spellInfo->Effects[j].IsApplyingAura())
                {
                    if (targetType == TARGET_UNIT_TARGET_ENEMY)
                        UPDATE_TARGET(AITARGET_DEBUFF)
                    else if (spellInfo->IsPositive())
                        UPDATE_TARGET(AITARGET_BUFF)
                }
            }
        }
        AIInfo->realCooldown = spellInfo->RecoveryTime + spellInfo->StartRecoveryTime;
        AIInfo->maxRange = spellInfo->GetMaxRange(false) * 3 / 4;
    }
}

//Enable PlayerAI when charmed
void PlayerAI::OnCharmed(bool apply) { me->IsAIEnabled = apply; }

void SimpleCharmedAI::UpdateAI(const uint32 /*diff*/)
{
  Creature* charmer = me->GetCharmer()->ToCreature();

    //kill self if charm aura has infinite duration
    if (charmer->IsInEvadeMode())
    {
        Unit::AuraEffectList const& auras = me->GetAuraEffectsByType(SPELL_AURA_MOD_CHARM);
        for (Unit::AuraEffectList::const_iterator iter = auras.begin(); iter != auras.end(); ++iter)
            if ((*iter)->GetCasterGUID() == charmer->GetGUID() && (*iter)->GetBase()->IsPermanent())
            {
                charmer->Kill(me);
                return;
            }
    }

    if (!charmer->isInCombat())
        me->GetMotionMaster()->MoveFollow(charmer, PET_FOLLOW_DIST, me->GetFollowAngle());

    Unit* target = me->getVictim();
    if (!target || !charmer->IsValidAttackTarget(target))
        AttackStart(charmer->SelectNearestTargetInAttackDistance());
}

SpellTargetSelector::SpellTargetSelector(Unit* caster, uint32 spellId) :
    _caster(caster), _spellInfo(sSpellMgr->GetSpellForDifficultyFromSpell(sSpellMgr->GetSpellInfo(spellId), caster))
{
    ASSERT(_spellInfo);
}

bool SpellTargetSelector::operator()(Unit const* target) const
{
    if (!target)
        return false;

    if (_spellInfo->CheckTarget(_caster, target) != SPELL_CAST_OK)
        return false;

    // copypasta from Spell::CheckRange
    float minRange = 0.0f;
    float maxRange = 0.0f;
    float rangeMod = 0.0f;
    if (_spellInfo->RangeEntry)
    {
        if (_spellInfo->RangeEntry->Flags & SPELL_RANGE_MELEE)
        {
            rangeMod = _caster->GetCombatReach() + 4.0f / 3.0f;
            if (target)
                rangeMod += target->GetCombatReach();
            else
                rangeMod += _caster->GetCombatReach();

            rangeMod = std::max(rangeMod, NOMINAL_MELEE_RANGE);
        }
        else
        {
            float meleeRange = 0.0f;
            if (_spellInfo->RangeEntry->Flags & SPELL_RANGE_RANGED)
            {
                meleeRange = _caster->GetCombatReach() + 4.0f / 3.0f;
                if (target)
                    meleeRange += target->GetCombatReach();
                else
                    meleeRange += _caster->GetCombatReach();

                meleeRange = std::max(meleeRange, NOMINAL_MELEE_RANGE);
            }

            minRange = _caster->GetSpellMinRangeForTarget(target, _spellInfo) + meleeRange;
            maxRange = _caster->GetSpellMaxRangeForTarget(target, _spellInfo);

            if (target)
            {
                rangeMod = _caster->GetCombatReach();
                rangeMod += target->GetCombatReach();

                if (minRange > 0.0f && !(_spellInfo->RangeEntry->Flags & SPELL_RANGE_RANGED))
                    minRange += rangeMod;
            }
        }

        if (target && _caster->IsMoving() && target->IsMoving() && !_caster->IsWalking() && !target->IsWalking() &&
            (_spellInfo->RangeEntry->Flags & SPELL_RANGE_MELEE || target->GetTypeId() == TYPEID_PLAYER))
            rangeMod += 5.0f / 3.0f;
    }

    maxRange += rangeMod;

    minRange *= minRange;
    maxRange *= maxRange;

    if (target && target != _caster)
    {
        if (_caster->GetExactDistSq(target) > maxRange)
            return false;

        if (minRange > 0.0f && _caster->GetExactDistSq(target) < minRange)
            return false;
    }
    return true;
}

bool NonTankTargetSelector::operator()(Unit const* target) const
{
    if (!target)
        return false;

    if (_playerOnly && target->GetTypeId() != TYPEID_PLAYER)
        return false;

    return target != _source->getVictim();
}
