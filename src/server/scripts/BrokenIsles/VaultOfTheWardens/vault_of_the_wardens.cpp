////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "vault_of_the_wardens.hpp"

/// Felsworn Infester - 96587
class npc_votw_felsworn_infester : public CreatureScript
{
    public:
        npc_votw_felsworn_infester() : CreatureScript("npc_votw_felsworn_infester") { }

        struct npc_votw_felsworn_infesterAI : public ScriptedAI
        {
            npc_votw_felsworn_infesterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Corrupt     = 191595,
                Nightmares  = 193069
            };

            void Reset() override { }

            int32 m_NightmaresTimer;

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_NightmaresTimer = 10 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_NightmaresTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_NightmaresTimer < 0)
                {
                    Unit* l_Victim = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank);
                    if (!l_Victim)
                        return;

                    DoCast(l_Victim, eSpells::Nightmares);

                    m_NightmaresTimer = 10 * IN_MILLISECONDS;
                    return;
                }

                Unit* l_Victim = SelectTarget(SELECT_TARGET_TOPAGGRO);
                if (!l_Victim)
                {
                    EnterEvadeMode();
                    return;
                }

                bool l_CannotCast = me->HasAuraType(AuraType::SPELL_AURA_MOD_SILENCE) || me->HasAuraType(AuraType::SPELL_AURA_MOD_PACIFY) || me->HasAuraType(AuraType::SPELL_AURA_MOD_PACIFY_SILENCE);
                if (l_CannotCast || !me->FindNearestPlayer(20.0f))
                {
                    AttackStart(l_Victim, true);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(l_Victim);

                    DoMeleeAttackIfReady();
                    return;
                }

                me->StopMoving();

                AttackStart(l_Victim, false);

                me->CastSpell(me, eSpells::Corrupt, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_felsworn_infesterAI(p_Creature);
        }
};

/// Immoliant Fury - 96584
class npc_votw_immoliant_fury : public CreatureScript
{
    public:
        npc_votw_immoliant_fury() : CreatureScript("npc_votw_immoliant_fury") { }

        struct npc_votw_immoliant_furyAI : public ScriptedAI
        {
            npc_votw_immoliant_furyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Strike = 191540,
                Cleave = 191538,
                Slide  = 191557
            };

            bool m_NextAttackIsStrike;
            int32 m_NextMeleeTimer;
            int32 m_SlideTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_SlideTimer = 10 * IN_MILLISECONDS;
                m_NextMeleeTimer = 200;
                m_NextAttackIsStrike = true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_SlideTimer -= p_Diff;
                m_NextMeleeTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_SlideTimer < 0)
                {
                    DoCastVictim(eSpells::Slide);
                    m_SlideTimer = 10 * IN_MILLISECONDS;
                    return;
                }

                if (m_NextMeleeTimer < 0)
                {
                    if (m_NextAttackIsStrike)
                    {
                        DoCastVictim(eSpells::Strike);
                        m_NextAttackIsStrike = false;
                    }
                    else
                    {
                        DoCastVictim(eSpells::Cleave);
                        m_NextAttackIsStrike = true;
                    }

                    m_NextMeleeTimer = 700;
                    return;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_immoliant_furyAI(p_Creature);
        }
};

/// Shadow Hunter - 98926
class npc_votw_shadow_hunter : public CreatureScript
{
    public:
        npc_votw_shadow_hunter() : CreatureScript("npc_votw_shadow_hunter") { }

        struct npc_votw_shadow_hunterAI : public ScriptedAI
        {
            npc_votw_shadow_hunterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                VitalStrike     = 194588,
                Metamorphosis   = 194611
            };

            bool m_Metamorphosis;
            int32 m_VitalStrikeTimer;

            void Reset() override { }

            void DamageTaken(Unit* p_Attaker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(40) && !m_Metamorphosis)
                {
                    DoCast(eSpells::Metamorphosis);
                    m_Metamorphosis = true;
                }
            }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                me->RemoveAurasDueToSpell(eSpells::Metamorphosis);
                m_Metamorphosis = false;
                m_VitalStrikeTimer = 4 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_VitalStrikeTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_VitalStrikeTimer < 0)
                {
                    DoCastVictim(eSpells::VitalStrike);
                    m_VitalStrikeTimer = 8 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_shadow_hunterAI(p_Creature);
        }
};

/// Fel Infused Fury - 99956
class npc_votw_fel_infused_fury : public CreatureScript
{
    public:
        npc_votw_fel_infused_fury() : CreatureScript("npc_votw_fel_infused_fury") { }

        struct npc_votw_fel_infused_furyAI : public ScriptedAI
        {
            npc_votw_fel_infused_furyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Metamorphosis   = 196787,
                UnleashFury     = 196799,
                FelGaze         = 196796
            };

            bool m_Metamorphosis;
            bool m_UnleashFury;
            int32 m_FelGazeTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                me->RemoveAurasDueToSpell(eSpells::Metamorphosis);
                m_Metamorphosis = false;
                m_UnleashFury = false;
                m_FelGazeTimer = 8 * IN_MILLISECONDS;
            }

            void DamageTaken(Unit* p_Attaker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(55) && !m_Metamorphosis)
                {
                    DoCast(eSpells::Metamorphosis);
                    m_Metamorphosis = true;
                    return;
                }

                if (me->HealthBelowPct(35) && !m_UnleashFury)
                {
                    DoCast(eSpells::UnleashFury);
                    m_UnleashFury = true;
                    return;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::FelGaze:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!me->isInCombat())
                    return;

                m_FelGazeTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (!m_Metamorphosis && m_FelGazeTimer < 0)
                {
                    me->SetFacingToObject(me->getVictim());
                    me->NearTeleportTo(*me);

                    DoCastVictim(eSpells::FelGaze);

                    AddTimedDelayedOperation(50, [this]() -> void
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    });

                    m_FelGazeTimer = 15 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_fel_infused_furyAI(p_Creature);
        }
};

/// Felsworn Myrmidon - 98954
class npc_votw_felsworn_myrmidon : public CreatureScript
{
    public:
        npc_votw_felsworn_myrmidon() : CreatureScript("npc_votw_felsworn_myrmidon") { }

        struct npc_votw_felsworn_myrmidonAI : public ScriptedAI
        {
            npc_votw_felsworn_myrmidonAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DeafeningScreech = 191735,
            };

            int32 m_DeafeningScreechTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_DeafeningScreechTimer = 6 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_DeafeningScreechTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_DeafeningScreechTimer < 0)
                {
                    me->SetFacingToObject(me->getVictim());
                    me->NearTeleportTo(*me);
                    DoCastVictim(eSpells::DeafeningScreech);
                    m_DeafeningScreechTimer = 8 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_felsworn_myrmidonAI(p_Creature);
        }
};

/// Viletongue Belcher - 96480
class npc_votw_viletongue_belcher : public CreatureScript
{
    public:
        npc_votw_viletongue_belcher() : CreatureScript("npc_votw_viletongue_belcher") { }

        struct npc_votw_viletongue_belcherAI : public ScriptedAI
        {
            npc_votw_viletongue_belcherAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                FieryBreath = 191691
            };

            int32 m_FieryBreathTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_FieryBreathTimer = urand(3, 8) * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_FieryBreathTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_FieryBreathTimer < 0)
                {
                    DoCastVictim(eSpells::FieryBreath);
                    m_FieryBreathTimer = 15 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_viletongue_belcherAI(p_Creature);
        }
};

/// Foul Mother - 98533
class npc_votw_foul_mother : public CreatureScript
{
    public:
        npc_votw_foul_mother() : CreatureScript("npc_votw_foul_mother") { }

        struct npc_votw_foul_motherAI : public Scripted_NoMovementAI
        {
            npc_votw_foul_motherAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                AddTimedDelayedOperation(2000, [this]() -> void
                {
                    DoCast(eSpells::ImpMother);
                });
            }

            enum eSpells
            {
                ImpMother               = 194806,
                Mortar                  = 194036,
                MortarSearcher          = 194035,
                AMothersLove            = 194064
            };

            enum eNPCs
            {
                BlazingImp = 98963
            };

            int32 m_MortarTimer;
            int32 m_AMothersLoveTimer;
            int32 m_BlazingImpTimer;
            int32 m_CheckVictimTimer;

            void Reset() override
            {
                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_MortarTimer = 5 * IN_MILLISECONDS;
                m_AMothersLoveTimer = 8 * IN_MILLISECONDS;
                m_BlazingImpTimer = 10 * IN_MILLISECONDS;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::AMothersLove:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });

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
                    case eSpells::MortarSearcher:
                    {
                        DoCast(p_Target, eSpells::Mortar, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!me->isInCombat())
                    return;

                m_MortarTimer -= p_Diff;
                m_AMothersLoveTimer -= p_Diff;
                m_BlazingImpTimer -= p_Diff;
                m_CheckVictimTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                    return;

                bool m_IsInMeleeRange = true;
                if (m_CheckVictimTimer < 0)
                {
                    if (me->getVictim())
                    {
                        if (!me->IsWithinMeleeRange(me->getVictim()))
                            m_IsInMeleeRange = false;
                    }

                    m_CheckVictimTimer = 2 * IN_MILLISECONDS;
                }

                if (m_MortarTimer < 0 || !m_IsInMeleeRange)
                {
                    me->CastSpell(me, eSpells::MortarSearcher, false);
                    m_MortarTimer = 7 * IN_MILLISECONDS;
                    return;
                }

                if (m_AMothersLoveTimer < 0)
                {
                    me->SetFacingToObject(l_Victim);
                    me->NearTeleportTo(*me);

                    DoCastVictim(eSpells::AMothersLove);

                    AddTimedDelayedOperation(50, [this]() -> void
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    });

                    m_AMothersLoveTimer = 10 * IN_MILLISECONDS;
                    return;
                }

                if (m_BlazingImpTimer < 0)
                {
                    me->SummonCreature(eNPCs::BlazingImp, *me);
                    m_BlazingImpTimer = 20 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_foul_motherAI(p_Creature);
        }
};

/// Blazing Imp - 98963
class npc_votw_blazing_imp : public CreatureScript
{
    public:
        npc_votw_blazing_imp() : CreatureScript("npc_votw_blazing_imp") { }

        struct npc_votw_blazing_impAI : public ScriptedAI
        {
            npc_votw_blazing_impAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                FireBlast = 194675,
                Implosion = 194682
            };

            bool m_MustDie;
            bool m_Implosion;

            void Reset() override
            {
                m_MustDie = false;
                m_Implosion = false;
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_MustDie && m_Implosion && p_Attacker->GetGUID() != me->GetGUID())
                {
                    p_Damage = 0;
                    return;
                }

                if (!m_MustDie && !m_Implosion && me->GetHealth() < p_Damage)
                {
                    p_Damage = 0;
                    m_Implosion = true;

                    me->InterruptNonMeleeSpells(true);

                    DoCast(me, eSpells::Implosion);
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::Implosion)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        m_Implosion = false;
                        m_MustDie = true;
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                UpdateOperations(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                {
                    EnterEvadeMode();
                    return;
                }

                if (me->GetDistance2d(l_Victim) > 15)
                {
                    me->GetMotionMaster()->MoveChase(l_Victim);
                    return;
                }

                AttackStart(l_Victim, false);

                me->CastSpell(l_Victim, eSpells::FireBlast, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_blazing_impAI(p_Creature);
        }
};

/// Fel Scorcher - 102583
class npc_votw_fel_scorcher : public CreatureScript
{
    public:
        npc_votw_fel_scorcher() : CreatureScript("npc_votw_fel_scorcher") { }

        struct npc_votw_fel_scorcherAI : public ScriptedAI
        {
            npc_votw_fel_scorcherAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                InfernoBlast    = 202661,
                Detonate        = 202663
            };

            void Reset() override { }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(eSpells::Detonate);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                {
                    EnterEvadeMode();
                    return;
                }

                if (me->GetDistance2d(l_Victim) > 15)
                {
                    me->GetMotionMaster()->MoveChase(l_Victim);
                    return;
                }

                AttackStart(l_Victim, false);

                me->CastSpell(l_Victim, eSpells::InfernoBlast, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_votw_fel_scorcherAI(p_Creature);
        }
};

/// Barbed Spiderling - 97677
class npc_votw_barbed_spiderling : public CreatureScript
{
    public:
        npc_votw_barbed_spiderling() : CreatureScript("npc_votw_barbed_spiderling") { }

        enum eSpell
        {
            SpellBarbedSpit = 193951
        };

        enum eEvent
        {
            EventBarbedSpit = 1
        };

        struct npc_votw_barbed_spiderlingAI : public ScriptedAI
        {
            npc_votw_barbed_spiderlingAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Fleeing;
            bool m_IsVeiled = false;

            void Reset() override
            {
                m_Fleeing = false;

                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvent::EventBarbedSpit, 1000);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_Fleeing && me->HealthBelowPctDamaged(20, p_Damage))
                {
                    m_Fleeing = true;

                    me->DoFleeToGetAssistance();
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode /*p_RemoveMode*/) override
            {
                if (p_SpellID == eVotWSpells::SpellVeiledInShadow)
                    m_IsVeiled = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (IsNearElunesLight(me))
                {
                    me->RemoveAura(eVotWSpells::SpellVeiledInShadow);

                    DoCast(me, eVotWSpells::SpellRevealed, true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_NO_ACTIONS | eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
                }
                else
                {
                    me->RemoveAura(eVotWSpells::SpellRevealed);

                    if (!m_IsVeiled)
                    {
                        me->SendClearTarget();
                        DoCast(me, eVotWSpells::SpellVeiledInShadow, true);
                        m_IsVeiled = true;
                    }

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_NO_ACTIONS | eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventBarbedSpit:
                    {
                        DoCast(me, eSpell::SpellBarbedSpit);
                        events.ScheduleEvent(eEvent::EventBarbedSpit, 8000);
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
            return new npc_votw_barbed_spiderlingAI(p_Creature);
        }
};

/// Aranasi Broodmother - 97678
class npc_votw_aranasi_broodmother : public CreatureScript
{
    public:
        npc_votw_aranasi_broodmother() : CreatureScript("npc_votw_aranasi_broodmother") { }

        enum eSpells
        {
            SpellRazorDummy     = 207174,
            SpellRazorMissile   = 193973,
            SpellPull           = 193998
        };

        enum eEvents
        {
            EventRazor = 1,
            EventPull
        };

        struct npc_votw_aranasi_broodmotherAI : public ScriptedAI
        {
            npc_votw_aranasi_broodmotherAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_IsVeiled = false;

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventRazor, 8500);
                events.ScheduleEvent(eEvents::EventPull, 11000);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellRazorDummy:
                    {
                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                Position l_Pos = l_Target->GetPosition();

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, frand(0.0f, 2.0f * M_PI));

                                DoCast(l_Pos, eSpells::SpellRazorMissile, true);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode /*p_RemoveMode*/) override
            {
                if (p_SpellID == eVotWSpells::SpellVeiledInShadow)
                    m_IsVeiled = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (IsNearElunesLight(me))
                {
                    me->RemoveAura(eVotWSpells::SpellVeiledInShadow);

                    DoCast(me, eVotWSpells::SpellRevealed, true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_NO_ACTIONS | eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
                }
                else
                {
                    me->RemoveAura(eVotWSpells::SpellRevealed);

                    if (!m_IsVeiled)
                    {
                        me->SendClearTarget();
                        DoCast(me, eVotWSpells::SpellVeiledInShadow, true);
                        m_IsVeiled = true;
                    }

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_NO_ACTIONS | eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventRazor:
                    {
                        DoCast(me, eSpells::SpellRazorDummy, true);
                        events.ScheduleEvent(eEvents::EventRazor, 12000);
                        break;
                    }
                    case eEvents::EventPull:
                    {
                        DoCast(me, eSpells::SpellPull, true);
                        events.ScheduleEvent(eEvents::EventPull, 15000);
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
            return new npc_votw_aranasi_broodmotherAI(p_Creature);
        }
};

/// Spirit of Vengeance - 100364
class npc_votw_spirit_of_vengeance : public CreatureScript
{
    public:
        npc_votw_spirit_of_vengeance() : CreatureScript("npc_votw_spirit_of_vengeance") { }

        enum eSpells
        {
            SpellKnockdownKick  = 214989,
            SpellTurnKick       = 189469,
            SpellThrow          = 214988
        };

        enum eEvents
        {
            EventKick = 1,
            EventGlaive
        };

        struct npc_votw_spirit_of_vengeanceAI : public ScriptedAI
        {
            npc_votw_spirit_of_vengeanceAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_IsVeiled = false;

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventKick, 3500);
                events.ScheduleEvent(eEvents::EventGlaive, 7500);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellKnockdownKick:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            if (Unit* l_Target = me->getVictim())
                                DoCast(l_Target, eSpells::SpellTurnKick);
                        });

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
                    case eSpells::SpellKnockdownKick:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        if (p_Target->HasAura(eVotWSpells::SpellElunesLightOverride) && p_Target->HasAura(eVotWSpells::SpellElunesLightOrbVisual2))
                        {
                            p_Target->RemoveAura(eVotWSpells::SpellElunesLightOverride);
                            p_Target->RemoveAura(eVotWSpells::SpellElunesLightOrbVisual2);

                            Position l_Pos = p_Target->GetPosition();

                            l_Pos.GetRandomNearPosition(l_Pos, 10.0f, p_Target);

                            p_Target->CastSpell(l_Pos, eVotWSpells::SpellElunesLightSummon, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode /*p_RemoveMode*/) override
            {
                if (p_SpellID == eVotWSpells::SpellVeiledInShadow)
                    m_IsVeiled = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (IsNearElunesLight(me))
                {
                    me->RemoveAura(eVotWSpells::SpellVeiledInShadow);

                    DoCast(me, eVotWSpells::SpellRevealed, true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_NO_ACTIONS | eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
                }
                else
                {
                    me->RemoveAura(eVotWSpells::SpellRevealed);

                    if (!m_IsVeiled)
                    {
                        me->SendClearTarget();
                        DoCast(me, eVotWSpells::SpellVeiledInShadow, true);
                        m_IsVeiled = true;
                    }

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_NO_ACTIONS | eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
                }

                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventKick:
                    {
                        if (Unit* l_Target = me->getVictim())
                            DoCast(l_Target, eSpells::SpellKnockdownKick);
                        events.ScheduleEvent(eEvents::EventKick, 13000);
                        break;
                    }
                    case eEvents::EventGlaive:
                    {
                        DoCast(me, eSpells::SpellThrow);
                        events.ScheduleEvent(eEvents::EventGlaive, 11000);
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
            return new npc_votw_spirit_of_vengeanceAI(p_Creature);
        }
};

/// Shadowmoon Warlock - 99704
class npc_votw_shadowmoon_warlock : public CreatureScript
{
    public:
        npc_votw_shadowmoon_warlock() : CreatureScript("npc_votw_shadowmoon_warlock") { }

        enum eSpells
        {
            SpellShadowBolt         = 196206,
            SpellCurseOfTongues     = 13338,
            SpellCorruption         = 196207,
            SpellSeedOfCorruption   = 196208
        };

        enum eEvents
        {
            EventShadowBolt = 1,
            EventCurseOfTongues,
            EventCorruption,
            EventSeedOfCorruption
        };

        struct npc_votw_shadowmoon_warlockAI : public ScriptedAI
        {
            npc_votw_shadowmoon_warlockAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventShadowBolt, urand(7000, 9000));
                events.ScheduleEvent(eEvents::EventCurseOfTongues, urand(11000, 13000));
                events.ScheduleEvent(eEvents::EventCorruption, urand(14000, 16000));
                events.ScheduleEvent(eEvents::EventSeedOfCorruption, urand(36000, 37000));
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
                    case eEvents::EventShadowBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellShadowBolt);

                        events.ScheduleEvent(eEvents::EventShadowBolt, urand(7000, 9000));
                        break;
                    }
                    case eEvents::EventCurseOfTongues:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellCurseOfTongues);

                        events.ScheduleEvent(eEvents::EventCurseOfTongues, urand(11000, 13000));
                        break;
                    }
                    case eEvents::EventCorruption:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellCorruption);

                        events.ScheduleEvent(eEvents::EventCorruption, urand(14000, 16000));
                        break;
                    }
                    case eEvents::EventSeedOfCorruption:
                    {
                        DoCastVictim(eSpells::SpellSeedOfCorruption);
                        events.ScheduleEvent(eEvents::EventSeedOfCorruption, urand(36000, 37000));
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
            return new npc_votw_shadowmoon_warlockAI(p_Creature);
        }
};

/// A Mother's Love - 194064
class spell_votw_foul_mother_a_mothers_love : public SpellScriptLoader
{
    public:
        spell_votw_foul_mother_a_mothers_love() : SpellScriptLoader("spell_votw_foul_mother_a_mothers_love") { }

        class spell_votw_foul_mother_a_mothers_love_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_votw_foul_mother_a_mothers_love_SpellScript);

            enum eSpells
            {
                AMothersLoveAreatrigger = 194070
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AMothersLoveAreatrigger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_votw_foul_mother_a_mothers_love_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_votw_foul_mother_a_mothers_love_SpellScript();
        }
};

/// Implosion - 194682
class spell_votw_blazing_imp_implosion : public SpellScriptLoader
{
    public:
        spell_votw_blazing_imp_implosion() : SpellScriptLoader("spell_votw_blazing_imp_implosion") { }

        class spell_votw_blazing_imp_implosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_votw_blazing_imp_implosion_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo || ! l_SpellInfo->Effects[EFFECT_0].RadiusEntry)
                    return;

                float l_Radius = l_SpellInfo->Effects[EFFECT_0].RadiusEntry->RadiusMax;

                std::list<Player*> l_Players;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(l_Caster, l_Radius, false);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(l_Caster, l_Players, l_Check);
                l_Caster->VisitNearbyWorldObject(l_Radius, l_Searcher);

                for (Player* l_Player : l_Players)
                    l_Player->CastSpell(l_Caster, l_SpellInfo->Effects[EFFECT_0].BasePoints, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_votw_blazing_imp_implosion_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_votw_blazing_imp_implosion_SpellScript();
        }
};

/// Immolated - 202662
class spell_votw_fel_scorcher_immolated : public SpellScriptLoader
{
    public:
        spell_votw_fel_scorcher_immolated() : SpellScriptLoader("spell_votw_fel_scorcher_immolated") { }

        class spell_votw_fel_scorcher_immolated_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_votw_fel_scorcher_immolated_SpellScript);

            enum eNPCs
            {
                FelScorcher = 102583
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<WorldObject*> l_RealTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Creature* l_Creature = l_Target->ToCreature();
                    if (!l_Creature || (l_Creature->GetEntry() != eNPCs::FelScorcher))
                        continue;

                    if (!l_Creature->IsWithinDistInMap(l_Creature, 30.0f))
                        continue;

                    l_RealTargets.push_back(l_Target);
                }

                p_Targets = l_RealTargets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_votw_fel_scorcher_immolated_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_votw_fel_scorcher_immolated_SpellScript();
        }
};

/// Pull - 193997
class spell_votw_pull : public SpellScriptLoader
{
    public:
        spell_votw_pull() : SpellScriptLoader("spell_votw_pull") { }

        class spell_votw_pull_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_votw_pull_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Target = GetTarget()->ToPlayer())
                        l_Target->SendApplyMovementForce(l_Caster->GetGUID(), true, l_Caster->GetPosition(), 5.0f, 1);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Target = GetTarget()->ToPlayer())
                        l_Target->SendApplyMovementForce(l_Caster->GetGUID(), false, Position());
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_votw_pull_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_votw_pull_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_votw_pull_AuraScript();
        }
};

/// Warden Dungeon - Barbed Web - 9775
class at_votw_barbed_web : public AreaTriggerEntityScript
{
    public:
        at_votw_barbed_web() : AreaTriggerEntityScript("at_votw_barbed_web") { }

        enum eSpell
        {
            SpellBarbedWebRemove = 215023
        };

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (p_Target->HasAura(eVotWSpells::SpellElunesLightOverride) && p_Target->HasAura(eVotWSpells::SpellElunesLightOrbVisual2))
            {
                p_Target->CastSpell(p_AreaTrigger->GetPosition(), eSpell::SpellBarbedWebRemove, true);

                p_AreaTrigger->Remove();
                return true;
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_votw_barbed_web();
        }
};

#ifndef __clang_analyzer__
void AddSC_vault_of_the_wardens()
{
    /// Trashes
    new npc_votw_felsworn_infester();
    new npc_votw_immoliant_fury();
    new npc_votw_shadow_hunter();
    new npc_votw_fel_infused_fury();
    new npc_votw_felsworn_myrmidon();
    new npc_votw_viletongue_belcher();
    new npc_votw_foul_mother();
    new npc_votw_blazing_imp();
    new npc_votw_fel_scorcher();
    new npc_votw_barbed_spiderling();
    new npc_votw_aranasi_broodmother();
    new npc_votw_spirit_of_vengeance();
    new npc_votw_shadowmoon_warlock();

    /// Spells
    new spell_votw_blazing_imp_implosion();
    new spell_votw_foul_mother_a_mothers_love();
    new spell_votw_fel_scorcher_immolated();
    new spell_votw_pull();

    /// AreaTrigger
    new at_votw_barbed_web();
}
#endif
