////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "halls_of_valor.hpp"

/// Valarjar Falconer - 99804
class npc_hov_valarjar_falconer : public CreatureScript
{
    public:
        npc_hov_valarjar_falconer() : CreatureScript("npc_hov_valarjar_falconer") { }

        struct npc_hov_valarjar_falconerAI : public ScriptedAI
        {
            npc_hov_valarjar_falconerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_HawkGuid = 0;

            void Reset() override
            {
                if (m_HawkGuid)
                {
                    if (Creature* l_Hawk = Creature::GetCreature(*me, m_HawkGuid))
                        l_Hawk->DespawnOrUnsummon();
                }
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (Creature* l_Hawk = me->SummonCreature(NPC_TRAINED_HAWK, me->GetPosition()))
                {
                    m_HawkGuid = l_Hawk->GetGUID();

                    if (l_Hawk->IsAIEnabled)
                        l_Hawk->AI()->AttackStart(p_Attacker);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hov_valarjar_falconerAI(p_Creature);
        }
};

/// Trained Hawk - 99828
class npc_hov_trained_hawk : public CreatureScript
{
    public:
        npc_hov_trained_hawk() : CreatureScript("npc_hov_trained_hawk") { }

        enum eSpell
        {
            SpellTalonRake = 203800
        };

        struct npc_hov_trained_hawkAI : public ScriptedAI
        {
            npc_hov_trained_hawkAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_1, urand(3000, 6000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == EVENT_1)
                {
                    DoCastVictim(eSpell::SpellTalonRake);

                    events.ScheduleEvent(EVENT_1, urand(5000, 8000));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hov_trained_hawkAI(p_Creature);
        }
};

/// Valarjar Trapper - 96934
class npc_hov_valarjar_trapper : public CreatureScript
{
    public:
        npc_hov_valarjar_trapper() : CreatureScript("npc_hov_valarjar_trapper") { }

        enum eSpells
        {
            SpellQuickChop          = 199361,
            SpellBearTrapMissile    = 199341
        };

        struct npc_hov_valarjar_trapperAI : public ScriptedAI
        {
            npc_hov_valarjar_trapperAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_1, urand(5000, 8000));
                events.ScheduleEvent(EVENT_2, urand(5000, 8000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCastVictim(eSpells::SpellQuickChop);

                        events.ScheduleEvent(EVENT_1, urand(8000, 11000));
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellBearTrapMissile, true);

                        events.ScheduleEvent(EVENT_2, urand(10000, 13000));
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
            return new npc_hov_valarjar_trapperAI(p_Creature);
        }
};

/// Bear Trap - 101085
class npc_hov_bear_trap : public CreatureScript
{
    public:
        npc_hov_bear_trap() : CreatureScript("npc_hov_bear_trap") { }

        enum eSpells
        {
            SpellBearTrapAT     = 199333,
            SpellBearTrapStun   = 199340,
            SpellBearTrapDoT    = 199337
        };

        struct npc_hov_bear_trapAI : public Scripted_NoMovementAI
        {
            npc_hov_bear_trapAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellBearTrapAT, true);
            }

            void JustDespawned() override
            {
                me->RemoveAllAreasTrigger();
            }

            void AreaTriggerDespawned(AreaTrigger* /*p_AreaTrigger*/, bool /*p_Expire*/) override
            {
                me->DespawnOrUnsummon(1);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBearTrapStun:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                            l_Owner->CastSpell(p_Target, eSpells::SpellBearTrapDoT, true);

                        me->RemoveAllAreasTrigger();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hov_bear_trapAI(p_Creature);
        }
};

/// Storm Drake (Field of the Eternal Hunt) - 99891
class npc_hov_storm_drake : public CreatureScript
{
    public:
        npc_hov_storm_drake() : CreatureScript("npc_hov_storm_drake") { }

        enum eSpells
        {
            SpellLightningBreath    = 198888,
            SpellCracklingStorm     = 198892,
            SpellKillingBlow        = 207753
        };

        struct npc_hov_storm_drakeAI : public ScriptedAI
        {
            npc_hov_storm_drakeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            Position const m_MovePos = { 3164.99f, 2963.536f, 628.65f, 3.96722f };

            bool m_Moved = false;

            void Reset() override
            {
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                SetFlyMode(true);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(p_Attacker);

                SetFlyMode(false);

                events.Reset();

                events.ScheduleEvent(EVENT_1, urand(15000, 17000));
                events.ScheduleEvent(EVENT_2, urand(17000, 23000));

                if (IsMythic())
                    events.ScheduleEvent(EVENT_3, 200);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_Moved || !p_Who->IsPlayer() || me->GetDistance(p_Who) > 90.0f)
                    return;

                if (me->GetDistance(m_MovePos) > 60.0f)
                    return;

                m_Moved = true;

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(1, m_MovePos);

                me->SetHomePosition(m_MovePos);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == MovementGeneratorType::POINT_MOTION_TYPE && p_ID == 1)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        SetFlyMode(false);
                    });
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Aura* l_Aura = me->GetAura(eSpells::SpellKillingBlow))
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance && IsMythic() && l_Aura->GetStackAmount() >= 10)
                        l_Instance->DoCompleteAchievement(STAG_PARTY);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, eSpells::SpellLightningBreath);

                        events.ScheduleEvent(EVENT_1, urand(15000, 17000));
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellCracklingStorm);

                        events.ScheduleEvent(EVENT_2, urand(17000, 23000));
                        break;
                    }
                    case EVENT_3:
                    {
                        if (Creature* l_Stag = me->FindNearestCreature(NPC_GILDEFUR_STAG, 7.0f))
                        {
                            if (l_Stag->isAlive() && !l_Stag->HasAura(29266))
                                DoCast(l_Stag, eSpells::SpellKillingBlow, true);
                        }

                        events.ScheduleEvent(EVENT_3, 200);
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
            return new npc_hov_storm_drakeAI(p_Creature);
        }
};

/// Crackle - 199805
class spell_hov_crackle : public SpellScriptLoader
{
    public:
        spell_hov_crackle() : SpellScriptLoader("spell_hov_crackle")
        {}

        enum eSpells
        {
            SpellCrackleMissile = 199807
        };

        class spell_hov_crackle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hov_crackle_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                    return;

                Position l_Pos;

                Unit* l_Caster = GetCaster();

                l_Caster->GetNearPosition(l_Pos, 20.f, 0.f);

                G3D::Vector2 l_Src = { l_Caster->m_positionX, l_Caster->m_positionY };
                G3D::Vector2 l_Tgt = { l_Pos.m_positionX, l_Pos.m_positionY };
                G3D::Vector2 l_Dir = l_Tgt - l_Src;

                float l_Pct = 0.1f;

                for (uint8 l_Itr = 0; l_Itr < 10; ++l_Itr)
                {
                    l_Tgt = (l_Src + (l_Dir * l_Pct));

                    l_Pos.m_positionX = l_Tgt.x;
                    l_Pos.m_positionY = l_Tgt.y;
                    l_Pct += 0.2f;

                    l_Caster->CastSpell(l_Pos, eSpells::SpellCrackleMissile, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hov_crackle_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hov_crackle_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_halls_of_valor()
{
    /// NPCs
    new npc_hov_valarjar_falconer();
    new npc_hov_trained_hawk();
    new npc_hov_valarjar_trapper();
    new npc_hov_bear_trap();
    new npc_hov_storm_drake();

    /// Spells
    new spell_hov_crackle();
}
#endif
