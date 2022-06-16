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
    SAY_AGGRO,
    SAY_NIGHTMARE,
    SAY_CRUSHING,
    SAY_ROOTS,
    SAY_EARTH,
    SAY_DEATH
};

enum Spells
{
    SPELL_CANCEL_DEEP_ROOTS     = 165953,
    SPELL_SHATTERED_EARTH       = 204666,
    SPELL_STRANGLING_ROOTS      = 204574,
    SPELL_STRANGLING_ROOTS_AT   = 199067,
    SPELL_SUBMERGED_AURA        = 220519,
    SPELL_NIGHTMARE_BREATH      = 204667,
    SPELL_CRUSHING_GRIP         = 204611,

    /// Heroic
    SPELL_UPROOT                = 212786,

    /// End
    SPELL_TALK                  = 202883,

    SPELL_BONUS_ROLL            = 226613,

    /// RP
    SPELL_PETRIFIED_BARK        = 164713,
    SPELL_CANCEL_PETRIFIED_BARK = 164719
};

enum eEvents
{
    EVENT_SHATTERED_EARTH       = 1,
    EVENT_STRANGLING_ROOTS      = 2,
    EVENT_NIGHTMARE_BREATH      = 3,
    EVENT_CRUSHING_GRIP         = 4,
    EVENT_REMOVE_PASSENGERS     = 5,

    /// Heroic
    EVENT_UPROOT                = 6
};

/// Oakheart - 103344
class boss_oakheart : public CreatureScript
{
    public:
        boss_oakheart() : CreatureScript("boss_oakheart") { }

        struct boss_oakheartAI : public BossAI
        {
            boss_oakheartAI(Creature* p_Creature) : BossAI(p_Creature, DATA_OAKHEART) { }

            bool m_IntroDone = false;

            void Reset() override
            {
                _Reset();

                if (me->GetVehicleKit())
                    me->GetVehicleKit()->RemoveAllPassengers();

                if (!m_IntroDone)
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    DoCast(me, SPELL_PETRIFIED_BARK, true);

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
                else
                {
                    DoCast(me, SPELL_CANCEL_PETRIFIED_BARK, true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                m_IntroDone = true;

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Reset();
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                events.ScheduleEvent(EVENT_SHATTERED_EARTH, 6000);
                events.ScheduleEvent(EVENT_STRANGLING_ROOTS, 14000);
                events.ScheduleEvent(EVENT_NIGHTMARE_BREATH, 22000);
                events.ScheduleEvent(EVENT_CRUSHING_GRIP, 30000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_UPROOT, 34000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
                DoCast(SPELL_TALK);
                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void PassengerBoarded(Unit* p_Who, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply)
                {
                    DoCast(204644);

                    p_Who->ClearUnitState(UnitState::UNIT_STATE_ONVEHICLE);
                }
                else
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Unit* l_Target = me->getVictim())
                        {
                            AttackStart(l_Target);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Target);
                        }
                    });
                }
            }

            void SpellHit(Unit* /*p_Target*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == 204644)
                {
                    events.ScheduleEvent(EVENT_REMOVE_PASSENGERS, 6000);

                    if (Unit* l_Passenger = me->GetVehicleKit()->GetPassenger(0))
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 0.0f, true);
                        if (l_Target)
                        {
                            l_Passenger->CastSpell(l_Target, 204651);
                            events.CancelEvent(EVENT_REMOVE_PASSENGERS);
                        }
                        else
                            l_Passenger->CastSpell(l_Passenger, 204651);

                        Talk(SAY_CRUSHING);
                    }
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
                    case EVENT_SHATTERED_EARTH:
                    {
                        DoCast(SPELL_SHATTERED_EARTH);
                        Talk(SAY_EARTH);
                        events.ScheduleEvent(EVENT_SHATTERED_EARTH, 40000);
                        break;
                    }
                    case EVENT_STRANGLING_ROOTS:
                    {
                        DoCast(SPELL_STRANGLING_ROOTS);
                        Talk(SAY_ROOTS);
                        events.ScheduleEvent(EVENT_STRANGLING_ROOTS, 22000);
                        break;
                    }
                    case EVENT_NIGHTMARE_BREATH:
                    {
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_NIGHTMARE_BREATH);
                        Talk(SAY_NIGHTMARE);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BREATH, 32000);
                        break;
                    }
                    case EVENT_CRUSHING_GRIP:
                    {
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_CRUSHING_GRIP);
                        events.ScheduleEvent(EVENT_CRUSHING_GRIP, 30000);
                        break;
                    }
                    case EVENT_REMOVE_PASSENGERS:
                    {
                        if (me->GetVehicleKit())
                            me->GetVehicleKit()->RemoveAllPassengers();
                        break;
                    }
                    case EVENT_UPROOT:
                    {
                        DoCast(SPELL_UPROOT);
                        events.ScheduleEvent(EVENT_UPROOT, 34000);
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
            return new boss_oakheartAI(p_Creature);
        }
};

/// Strangling Roots - 100991
class npc_oakheart_strangling_roots : public CreatureScript
{
    public:
        npc_oakheart_strangling_roots() : CreatureScript("npc_oakheart_strangling_roots") { }

        struct npc_oakheart_strangling_rootsAI : public ScriptedAI
        {
            npc_oakheart_strangling_rootsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_STRANGLING_ROOTS_AT, true);
                DoCast(me, SPELL_SUBMERGED_AURA, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_oakheart_strangling_rootsAI(p_Creature);
        }
};

/// Rotheart Keeper - 99359
class npc_oakheart_rotheart_keeper : public CreatureScript
{
    public:
        npc_oakheart_rotheart_keeper() : CreatureScript("npc_oakheart_rotheart_keeper") { }

        enum eSpells
        {
            SpellVileMushroomSearcher   = 198799,
            SpellVileMushroomMissile    = 220369
        };

        struct npc_oakheart_rotheart_keeperAI : public ScriptedAI
        {
            npc_oakheart_rotheart_keeperAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Killer*/) override
            {
                events.ScheduleEvent(EVENT_1, 4000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellVileMushroomSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellVileMushroomMissile, true);
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
                        DoCast(me, eSpells::SpellVileMushroomSearcher);
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
            return new npc_oakheart_rotheart_keeperAI(p_Creature);
        }
};

/// Vile Mushroom - 100960
class npc_oakheart_vile_mushroom : public CreatureScript
{
    public:
        npc_oakheart_vile_mushroom() : CreatureScript("npc_oakheart_vile_mushroom") { }

        enum eSpells
        {
            VileMushroomVisual  = 198937,
            VileMushroomAT      = 198915,
            VileMushroomTrigger = 198907,
            VileBurstExplosion  = 198916
        };

        struct npc_oakheart_vile_mushroomAI : public ScriptedAI
        {
            npc_oakheart_vile_mushroomAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Triggered = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                SetCombatMovement(false);

                me->SetReactState(REACT_PASSIVE);

                DoCast(me, eSpells::VileMushroomVisual, true);
                DoCast(me, eSpells::VileMushroomAT, true);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (!m_Triggered && p_SpellInfo->Id == eSpells::VileMushroomTrigger)
                {
                    m_Triggered = true;

                    DoCast(me, eSpells::VileBurstExplosion, true);

                    me->DespawnOrUnsummon(100);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_oakheart_vile_mushroomAI(p_Creature);
        }
};

/// Tormenting Eye - 204243
class spell_oakheart_tormenting_eye : public SpellScriptLoader
{
    public:
        spell_oakheart_tormenting_eye() : SpellScriptLoader("spell_oakheart_tormenting_eye") { }

        enum eSpell
        {
            SpellTormentingFear = 204246
        };

        class spell_oakheart_tormenting_eye_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oakheart_tormenting_eye_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();
                if (l_Mode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (Unit* l_Target = GetTarget())
                            l_Caster->CastSpell(l_Target, eSpell::SpellTormentingFear, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_oakheart_tormenting_eye_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oakheart_tormenting_eye_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_oakheart()
{
    /// Boss
    new boss_oakheart();

    /// Creatures
    new npc_oakheart_strangling_roots();
    new npc_oakheart_rotheart_keeper();
    new npc_oakheart_vile_mushroom();

    /// Spell
    new spell_oakheart_tormenting_eye();
}
#endif
