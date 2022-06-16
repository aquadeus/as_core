////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "darkheart_thicket.hpp"

enum Says
{
    SAY_EVENT_BOSS = 0,
    SAY_AGGRO      = 1,
    SAY_NIGHTFALL  = 2,
    SAY_RAMPAGE    = 3,
    SAY_GREIEVOUS  = 4,
    SAY_20         = 5,
    SAY_DEATH      = 6,

    /// Event
    SAY_EVENT       = 0
};

enum Spells
{
    SPELL_PRE_GRIEVOUS_LEAP     = 197709,
    SPELL_GRIEVOUS_LEAP         = 196346,
    SPELL_GRIEVOUS_LEAP_MORPH   = 196348,
    SPELL_GRIEVOUS_LEAP_RETURN  = 198095,
    SPELL_GRIEVOUS_LEAP_DOT     = 196376,
    SPELL_GRIEVOUS_LEAP_AOE_1   = 196354,
    SPELL_GRIEVOUS_LEAP_AOE_2   = 198269,
    SPELL_GRIEVOUS_RIP          = 225484,
    SPELL_PRIMAL_RAMPAGE_MORPH  = 198360,
    SPELL_PRIMAL_RAMPAGE_KNOCK  = 198376,
    SPELL_PRIMAL_RAMPAGE_CHARGE = 198379,
    SPELL_PRIMAL_RAMPAGE_REMOVE = 198387,
    SPELL_NIGHTFALL             = 198400,
    SPELL_NIGHTFALL_DOT         = 198408,
    SPELL_NIGHTMARE_FIXATE      = 198477,
    SPELL_NIGHTFALL_MISSILE     = 198401,
    SPELL_NIGHTFALL_M_MISSILE   = 212464,
    SPELL_NIGHTFALL_SMALL_AT    = 198464,

    /// Pre-event
    SPELL_VISUAL                = 197642,

    /// End
    SPELL_TALK                  = 202882,

    SPELL_BONUS_ROLL            = 226610
};

enum eEvents
{
    EVENT_PRE_GRIEVOUS_LEAP     = 1,
    EVENT_GRIEVOUS_LEAP         = 2,
    EVENT_PRIMAL_RAMPAGE        = 3,
    EVENT_NIGHTFALL             = 4
};

/// Arch Druid Glaidalis - 96512
class boss_arch_druid_glaidalis : public CreatureScript
{
    public:
        boss_arch_druid_glaidalis() : CreatureScript("boss_arch_druid_glaidalis") { }

        struct boss_arch_druid_glaidalisAI : public BossAI
        {
            boss_arch_druid_glaidalisAI(Creature* p_Creature) : BossAI(p_Creature, DATA_GLAIDALIS)
            {
                m_IntroEvent = false;
                m_Intro = false;
                m_Say20 = false;

                if (instance)
                    instance->HandleGameObject(instance->GetData64(GO_GLAIDALIS_INVIS_DOOR), true);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            bool m_IntroEvent;
            bool m_Intro;
            bool m_Say20;

            uint64 m_GrievousDruid;

            void Reset() override
            {
                _Reset();
                m_Say20 = false;

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTFALL_DOT);

                me->RemoveAllAreasTrigger();

                m_GrievousDruid = 0;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                events.ScheduleEvent(EVENT_GRIEVOUS_LEAP, 6000);
                events.ScheduleEvent(EVENT_PRIMAL_RAMPAGE, 14000);
                events.ScheduleEvent(EVENT_NIGHTFALL, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
                me->RemoveAllAreasTrigger();

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTFALL_DOT);

                DoCast(me, SPELL_TALK, true);
                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != EFFECT_MOTION_TYPE && p_Type != POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case SPELL_PRE_GRIEVOUS_LEAP:
                    {
                        if (Creature* l_Druid = Creature::GetCreature(*me, m_GrievousDruid))
                            me->Kill(l_Druid);
                        events.ScheduleEvent(EVENT_PRE_GRIEVOUS_LEAP, 500);
                        break;
                    }
                    case SPELL_GRIEVOUS_LEAP_MORPH:
                    {
                        if (Unit* l_Victim = me->getVictim())
                            DoCast(l_Victim, SPELL_GRIEVOUS_LEAP_RETURN);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_GRIEVOUS_LEAP_AOE_1:
                    case SPELL_GRIEVOUS_LEAP_AOE_2:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        DoCast(p_Target, SPELL_GRIEVOUS_LEAP_DOT, true);
                        break;
                    }
                    case SPELL_NIGHTFALL:
                    {
                        if (IsMythic())
                            DoCast(p_Target, SPELL_NIGHTFALL_M_MISSILE, true);
                        else
                            DoCast(p_Target, SPELL_NIGHTFALL_MISSILE, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || p_Who->ToPlayer()->isGameMaster())
                    return;

                if (!m_Intro && me->IsWithinDistInMap(p_Who, 48.0f) && instance)
                {
                    m_Intro = true;

                    instance->HandleGameObject(instance->GetData64(GO_GLAIDALIS_INVIS_DOOR), false);

                    std::list<Creature*> l_List;
                    me->GetCreatureListWithEntryInGrid(l_List, NPC_DRUIDIC_PRESERVER, 150.0f);

                    if (!l_List.empty())
                    {
                        Talk(SAY_EVENT_BOSS);

                        for (Creature* l_Iter : l_List)
                        {
                            if (Creature* target = l_Iter->FindNearestCreature(100404, 50.0f, true))
                                l_Iter->CastSpell(target, SPELL_VISUAL);
                        }

                        auto l_Iter = l_List.begin();
                        std::advance(l_Iter, urand(0, l_List.size() - 1));

                        if ((*l_Iter)->IsAIEnabled)
                            (*l_Iter)->AI()->Talk(SAY_EVENT);
                    }

                    events.ScheduleEvent(EVENT_PRE_GRIEVOUS_LEAP, 3000);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPctDamaged(20, p_Damage) && !m_Say20)
                {
                    m_Say20 = true;
                    Talk(SAY_20);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if ((!UpdateVictim() && m_IntroEvent))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 70.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_PRE_GRIEVOUS_LEAP:
                    {
                        if (Creature* l_Target = me->FindNearestCreature(NPC_DRUIDIC_PRESERVER, 200.0f, true))
                        {
                            me->CastSpell(l_Target, SPELL_PRE_GRIEVOUS_LEAP);
                            m_GrievousDruid = l_Target->GetGUID();
                        }
                        else
                        {
                            BossAI::EnterEvadeMode();

                            if (Creature* l_Trigger = me->FindNearestCreature(102851, 200.0f, true))
                                l_Trigger->RemoveAurasDueToSpell(203257);

                            if (instance)
                                instance->HandleGameObject(instance->GetData64(GO_GLAIDALIS_INVIS_DOOR), true);

                            m_IntroEvent = true;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        }

                        break;
                    }
                    case EVENT_GRIEVOUS_LEAP:
                    {
                        if (urand(0, 1) == 1)
                            Talk(SAY_GREIEVOUS);

                        DoCast(SPELL_GRIEVOUS_LEAP);
                        events.ScheduleEvent(EVENT_GRIEVOUS_LEAP, 16000);
                        break;
                    }
                    case EVENT_PRIMAL_RAMPAGE:
                    {
                        if (urand(0, 1) == 1)
                            Talk(SAY_RAMPAGE);

                        DoCast(me, SPELL_PRIMAL_RAMPAGE_MORPH, true);

                        if (Unit* l_Target = me->getVictim())
                        {
                            DoCast(l_Target, SPELL_PRIMAL_RAMPAGE_KNOCK, true);

                            Position l_Pos = *me;
                            l_Pos.m_positionX += 30.0f * cos(l_Pos.m_orientation);
                            l_Pos.m_positionY += 30.0f * sin(l_Pos.m_orientation);

                            me->CastSpell(l_Pos, SPELL_PRIMAL_RAMPAGE_CHARGE, false);
                        }

                        events.ScheduleEvent(EVENT_PRIMAL_RAMPAGE, 16000);
                        break;
                    }
                    case EVENT_NIGHTFALL:
                    {
                        Talk(SAY_NIGHTFALL);
                        DoCast(SPELL_NIGHTFALL);
                        events.ScheduleEvent(EVENT_NIGHTFALL, 16000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_arch_druid_glaidalisAI(p_Creature);
        }
};

/// Nightmare Abomination - 102962
class npc_arch_druid_nightmare_abomination : public CreatureScript
{
    public:
        npc_arch_druid_nightmare_abomination() : CreatureScript("npc_arch_druid_nightmare_abomination") { }

        struct npc_arch_druid_nightmare_abominationAI : public ScriptedAI
        {
            npc_arch_druid_nightmare_abominationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();

                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                    DoCast(l_Target, SPELL_NIGHTMARE_FIXATE, true);

                events.ScheduleEvent(EVENT_1, 2000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_NIGHTFALL_SMALL_AT, true);
                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arch_druid_nightmare_abominationAI(p_Creature);
        }
};

/// Dreadsoul Ruiner - 95771
class npc_arch_druid_dreadsoul_ruiner : public CreatureScript
{
    public:
        npc_arch_druid_dreadsoul_ruiner() : CreatureScript("npc_arch_druid_dreadsoul_ruiner") { }

        enum eSpells
        {
            SpellStarShowerSearcher = 200658,
            SpellStarShowerMissile  = 204383,
            SpellDespair            = 200642
        };

        struct npc_arch_druid_dreadsoul_ruinerAI : public ScriptedAI
        {
            npc_arch_druid_dreadsoul_ruinerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Killer*/) override
            {
                events.ScheduleEvent(EVENT_1, 2000); ///< Star Shower
                events.ScheduleEvent(EVENT_2, 3000); ///< Despair
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellStarShowerSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellStarShowerMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, eSpells::SpellStarShowerSearcher);
                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = me->getVictim())
                            DoCast(l_Target, eSpells::SpellDespair);

                        events.ScheduleEvent(EVENT_2, 6000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arch_druid_dreadsoul_ruinerAI(p_Creature);
        }
};

/// Frenzied Nightclaw - 95772
class npc_arch_druid_frenzied_nightclaw : public CreatureScript
{
    public:
        npc_arch_druid_frenzied_nightclaw() : CreatureScript("npc_arch_druid_frenzied_nightclaw") { }

        enum eSpells
        {
            SpellFranticLeap    = 200618,
            SpellFranticRip     = 200620,
            SpellGrievousRip    = 225484
        };

        struct npc_arch_druid_frenzied_nightclawAI : public ScriptedAI
        {
            npc_arch_druid_frenzied_nightclawAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Killer*/) override
            {
                events.ScheduleEvent(EVENT_1, 5000);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == eSpells::SpellFranticLeap)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGuid);

                    if (IsMythic())
                        DoCast(l_Target, eSpells::SpellGrievousRip, true);
                    else
                        DoCast(l_Target, eSpells::SpellFranticRip, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        Unit* l_Target = SelectRangedTarget(true);

                        if (l_Target == nullptr)
                            l_Target = SelectMeleeTarget(false);

                        if (l_Target == nullptr)
                            l_Target = me->getVictim();

                        if (l_Target != nullptr)
                        {
                            m_TargetGuid = l_Target->GetGUID();
                            DoCast(l_Target, eSpells::SpellFranticLeap);
                        }

                        events.ScheduleEvent(EVENT_1, urand(8, 16) * IN_MILLISECONDS);
                        break;
                    }

                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            private:
                uint64 m_TargetGuid;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arch_druid_frenzied_nightclawAI(p_Creature);
        }
};

/// Crazed Razorbeak - 95766
class npc_arch_druid_crazed_razorbeak : public CreatureScript
{
    public:
        npc_arch_druid_crazed_razorbeak() : CreatureScript("npc_arch_druid_crazed_razorbeak") { }

        enum eSpells
        {
            SpellPropellingChargeCast   = 200768,
            SpellPropellingChargeRemove = 200770
        };

        struct npc_arch_druid_crazed_razorbeakAI : public ScriptedAI
        {
            npc_arch_druid_crazed_razorbeakAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Killer*/) override
            {
                events.ScheduleEvent(EVENT_1, 5000);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellPropellingChargeCast)
                {
                    me->SetReactState(REACT_PASSIVE);

                    Position l_Pos = *me;

                    l_Pos.m_positionX += 25.0f * std::cos(me->m_orientation);
                    l_Pos.m_positionY += 25.0f * std::sin(me->m_orientation);
                    l_Pos.m_positionZ = me->GetMap()->GetHeight(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                    float l_Angle   = me->GetRelativeAngle(l_Pos.GetPositionX(), l_Pos.GetPositionY());
                    float l_Dist    = me->GetDistance(l_Pos);

                    me->GetFirstCollisionPosition(l_Pos, l_Dist, l_Angle);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveCharge(&l_Pos);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == EVENT_CHARGE)
                {
                    DoCast(me, eSpells::SpellPropellingChargeRemove);
                    me->SetReactState(REACT_AGGRESSIVE);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();

                        if (Unit* l_Target = me->getVictim())
                            AttackStart(l_Target);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, eSpells::SpellPropellingChargeCast);
                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arch_druid_crazed_razorbeakAI(p_Creature);
        }
};

/// Grievous Leap - 196346
class spell_glaidalis_grievous_leap : public SpellScriptLoader
{
    public:
        spell_glaidalis_grievous_leap() : SpellScriptLoader("spell_glaidalis_grievous_leap") { }

        class spell_glaidalis_grievous_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_glaidalis_grievous_leap_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Creature* l_Glaidalis = GetCaster()->ToCreature();
                if (!l_Glaidalis)
                    return;

                p_Targets.clear();

                if (l_Glaidalis->IsAIEnabled)
                {
                    if (Unit* l_Target = l_Glaidalis->AI()->SelectTarget(SELECT_TARGET_FARTHEST, 0, 55.0f, true))
                        p_Targets.push_back(l_Target);
                }
            }

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_GRIEVOUS_LEAP_MORPH, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_glaidalis_grievous_leap_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_glaidalis_grievous_leap_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_glaidalis_grievous_leap_SpellScript();
        }
};

/// Grievous Tear - 196376
/// Grievous Rip - 225484
class spell_glaidalis_grievous_tear : public SpellScriptLoader
{
    public:
        spell_glaidalis_grievous_tear() : SpellScriptLoader("spell_glaidalis_grievous_tear") { }

        class spell_glaidalis_grievous_tear_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_glaidalis_grievous_tear_AuraScript);

            void OnUpdate(uint32 p_Diff, AuraEffect* p_AurEff)
            {
                WorldObject* l_Target = p_AurEff->GetBase()->GetOwner();
                if (!l_Target || !l_Target->ToUnit())
                    return;

                Unit* l_UnitTarget = l_Target->ToUnit();

                if (l_UnitTarget->HealthAbovePct(90))
                {
                    uint32 l_ID = m_scriptSpellId;
                    l_UnitTarget->AddDelayedEvent([l_UnitTarget, l_ID]() -> void
                    {
                        l_UnitTarget->RemoveAurasDueToSpell(l_ID);
                    }, 100);
                }
            }

            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo& /*p_DmgInfo*/, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case SPELL_GRIEVOUS_LEAP_DOT:
                    {
                        OnEffectUpdate += AuraEffectUpdateFn(spell_glaidalis_grievous_tear_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                        OnEffectAbsorb += AuraEffectAbsorbFn(spell_glaidalis_grievous_tear_AuraScript::Absorb, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                        break;
                    }
                    case SPELL_GRIEVOUS_RIP:
                    {
                        OnEffectUpdate += AuraEffectUpdateFn(spell_glaidalis_grievous_tear_AuraScript::OnUpdate, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
                        OnEffectAbsorb += AuraEffectAbsorbFn(spell_glaidalis_grievous_tear_AuraScript::Absorb, EFFECT_2, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_glaidalis_grievous_tear_AuraScript();
        }
};

/// Nightmare Toxin - 200684
class spell_nightmare_toxin : public SpellScriptLoader
{
    public:
        spell_nightmare_toxin() : SpellScriptLoader("spell_nightmare_toxin") { }

        class spell_nightmare_toxin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nightmare_toxin_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (Creature* l_Spider = l_Target->FindNearestCreature(101679, 40.0f, true))
                    l_Spider->CastSpell(l_Target, 200686, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_nightmare_toxin_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nightmare_toxin_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_arch_druid_glaidalis()
{
    /// Boss
    new boss_arch_druid_glaidalis();

    /// Creatures
    new npc_arch_druid_nightmare_abomination();
    new npc_arch_druid_dreadsoul_ruiner();
    new npc_arch_druid_frenzied_nightclaw();
    new npc_arch_druid_crazed_razorbeak();

    /// Spells
    new spell_glaidalis_grievous_leap();
    new spell_glaidalis_grievous_tear();
    new spell_nightmare_toxin();
}
#endif
