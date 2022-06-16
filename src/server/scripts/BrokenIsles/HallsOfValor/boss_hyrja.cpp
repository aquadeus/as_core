////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "halls_of_valor.hpp"

enum Says
{
    SAY_AGGRO       = 0,
    SAY_EXPEL_LIGHT = 1,
    SAY_DEATH       = 2,
    SAY_WIPE        = 3,
    SAY_KILL        = 4,
};

enum Spells
{
    SPELL_ASCENDANCE                = 191475,
    SPELL_ASCENDANCE_JUMP           = 191478,
    SPELL_EMPOWERMENT_TRACKER       = 192130,
    SPELL_EXPEL_LIGHT               = 192044,
    SPELL_EXPEL_LIGHT_DMG           = 192067,
    SPELL_EXPEL_LIGHT_TRIGGER       = 192048,
    SPELL_SHIELD_OF_LIGHT           = 192018,
    SPELL_MYSTIC_MOD_DMG_THUNDER    = 192132,
    SPELL_MYSTIC_MOD_DMG_HOLY       = 192133,
    SPELL_ARCING_BOLT_SEARCHER      = 191978,
    SPELL_ARCING_BOLT_DAMAGE        = 191976,

    /// Special Ability
    SPELL_EYE_OF_THE_STORM          = 192304,
    SPELL_EYE_OF_THE_STORM_DMG      = 200901,
    SPELL_EYE_OF_THE_STORM_AT       = 203955,
    SPELL_SANCTIFY_TICK             = 192158,
    SPELL_SANCTIFY_AT               = 192163,
    SPELL_SANCTIFY_DAMAGE           = 192206,
    SPELL_SANCTIFY_MYTHIC_AOE       = 215457,

    /// Defender
    SPELL_OLMYR_VISUAL              = 191899,
    SPELL_SOLSTEN_VISUAL            = 192147,
    SPELL_OLMYR_LIGHT               = 192288,
    SPELL_SOLSTEN_ARCING            = 191976,

    SPELL_CONVERSATION              = 198226
};

enum eEvents
{
    EVENT_EXPEL_LIGHT       = 1,
    EVENT_SHIELD_OF_LIGHT   = 2,
    EVENT_SPECIAL_ABILITY   = 3,
    EVENT_ARCING_BOLT       = 4
};

Position const g_FightPos = { 3151.07f, 326.29f, 655.253f, 0.793447f };

/// Hyrja - 95833
class boss_hyrja : public CreatureScript
{
    public:
        boss_hyrja() : CreatureScript("boss_hyrja") { }

        struct boss_hyrjaAI : public BossAI
        {
            boss_hyrjaAI(Creature* p_Creature) : BossAI(p_Creature, DATA_HYRJA)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

                m_EventComplete = false;
                m_DefenderComplete = 0;

                m_DefenderGuids[0] = 0;
                m_DefenderGuids[1] = 0;
            }

            uint8 m_DefenderComplete;
            uint64 m_DefenderGuids[2];
            bool m_EventComplete;

            uint8 m_ExpelLightStacks;

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_INTERRUPT_CAST, true);

                _Reset();
                me->RemoveAurasDueToSpell(SPELL_EMPOWERMENT_TRACKER);
                if (m_EventComplete)
                    DoCast(me, SPELL_ASCENDANCE, true);

                m_ExpelLightStacks = 0;

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    SummonDefenders();
                });
            }

            void JustReachedHome() override
            {
                Talk(Says::SAY_WIPE);
                BossAI::JustReachedHome();

                summons.DespawnAll();

                SummonDefenders();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                DoCast(me, SPELL_EMPOWERMENT_TRACKER, true);

                if (Creature* l_Olmyr = me->GetCreature(*me, m_DefenderGuids[0]))
                    l_Olmyr->CastSpell(l_Olmyr, 192152, true);

                if (Creature* l_Solsten = me->GetCreature(*me, m_DefenderGuids[1]))
                    l_Solsten->CastSpell(l_Solsten, 192150, true);

                events.ScheduleEvent(EVENT_EXPEL_LIGHT, 3000);
                events.ScheduleEvent(EVENT_SPECIAL_ABILITY, 10000);
                events.ScheduleEvent(EVENT_SHIELD_OF_LIGHT, 25000);
                events.ScheduleEvent(EVENT_ARCING_BOLT, 15000);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim != nullptr && p_Victim->IsPlayer())
                    Talk(Says::SAY_KILL);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
                summons.DespawnAll();

                me->SummonCreature(NPC_OLMYR_THE_ENLIGHTENED, 3113.8657f, 304.4067f, 662.3998f, 0.759655f);
                me->SummonCreature(NPC_SOLSTEN, 3128.69f, 289.436f, 662.3998f, 0.759655f);
            }

            void SummonDefenders()
            {
                me->SummonCreature(NPC_OLMYR_THE_ENLIGHTENED, 3141.80f, 362.52f, 655.27f, 4.66f);
                me->SummonCreature(NPC_SOLSTEN, 3187.97f, 316.82f, 655.27f, 3.16f);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == NPC_OLMYR_THE_ENLIGHTENED)
                    m_DefenderGuids[0] = p_Summon->GetGUID();

                if (p_Summon->GetEntry() == NPC_SOLSTEN)
                    m_DefenderGuids[1] = p_Summon->GetGUID();
            }

            void DoAction(int32 const p_Action) override
            {
                if (m_EventComplete)
                    return;

                ++m_DefenderComplete;

                if (m_DefenderComplete == 2)
                {
                    DoCast(SPELL_ASCENDANCE);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        if (instance)
                            instance->DoCombatStopOnPlayers();
                    });
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_ASCENDANCE && !m_EventComplete)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->CastSpell(g_FightPos, SPELL_ASCENDANCE_JUMP, true);
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_ARCING_BOLT_SEARCHER:
                    {
                        me->CastSpell(p_Target, SPELL_ARCING_BOLT_DAMAGE, false);
                        break;
                    }
                    case SPELL_EXPEL_LIGHT:
                    {
                        me->CastSpell(p_Target, SPELL_EXPEL_LIGHT_TRIGGER, true);
                        break;
                    }
                    case SPELL_EXPEL_LIGHT_TRIGGER:
                    {
                        if (m_ExpelLightStacks > 1)
                        {
                            if (Aura* l_Aura = p_Target->GetAura(SPELL_EXPEL_LIGHT_TRIGGER, me->GetGUID()))
                                l_Aura->SetStackAmount(m_ExpelLightStacks);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_SANCTIFY_DAMAGE:
                    {
                        if (IsMythic())
                            me->CastSpell(me, SPELL_SANCTIFY_MYTHIC_AOE, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == SPELL_ASCENDANCE_JUMP)
                {
                    m_EventComplete = true;
                    me->SetHomePosition({ 3151.07f, 326.29f, 655.253f, 0.793447f });
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_HYRJA_EVENT_COMPLETE:
                        return m_EventComplete;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                if (p_ID == DATA_HYRJA_EXPEL_LIGHT_STACKS)
                {
                    if (m_ExpelLightStacks > 1)
                        --m_ExpelLightStacks;
                    else
                        m_ExpelLightStacks = 0;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetExactDist(&g_FightPos) > 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_EXPEL_LIGHT:
                    {
                        if (IsMythic())
                            m_ExpelLightStacks = 3;

                        DoCast(SPELL_EXPEL_LIGHT);
                        Talk(SAY_EXPEL_LIGHT);
                        events.ScheduleEvent(EVENT_EXPEL_LIGHT, 22000);
                        break;
                    }
                    case EVENT_SHIELD_OF_LIGHT:
                    {
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_SHIELD_OF_LIGHT);
                        events.ScheduleEvent(EVENT_SHIELD_OF_LIGHT, 20000);
                        break;
                    }
                    case EVENT_SPECIAL_ABILITY:
                    {
                        Creature* l_Olymr = me->GetCreature(*me, m_DefenderGuids[0]);
                        Creature* l_Solsten = me->GetCreature(*me, m_DefenderGuids[1]);
                        if (!l_Olymr || !l_Solsten)
                            return;

                        if (me->GetDistance(l_Solsten) < me->GetDistance(l_Olymr))
                        {
                            DoCast(me, SPELL_EYE_OF_THE_STORM, true);
                            DoCast(me, SPELL_EYE_OF_THE_STORM_AT, true);
                            DoCast(SPELL_EYE_OF_THE_STORM_DMG);
                        }
                        else
                            DoCast(SPELL_SANCTIFY_TICK);

                        events.ScheduleEvent(EVENT_SPECIAL_ABILITY, 30000);
                        break;
                    }
                    case EVENT_ARCING_BOLT:
                    {
                        me->CastSpell(me, SPELL_ARCING_BOLT_SEARCHER, true);
                        events.ScheduleEvent(EVENT_ARCING_BOLT, 25000);
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
            return new boss_hyrjaAI (p_Creature);
        }
};

/// Olmyr the Enlightened - 97202
/// Solsten - 97219
class npc_hyrja_defenders : public CreatureScript
{
    public:
        npc_hyrja_defenders() : CreatureScript("npc_hyrja_defenders") { }

        enum eEvents
        {
            EventSearingLight   = 1,
            EventSanctify,
            EventEyeOfStorm,
        };

        enum eTalk
        {
            TalkAggro,
            TalkSpirit,
        };

        struct npc_hyrja_defendersAI : public ScriptedAI
        {
            npc_hyrja_defendersAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                if (InstanceScript* l_Instance = p_Creature->GetInstanceScript())
                    m_Ascendance = l_Instance->GetBossState(DATA_HYRJA) == DONE;
            }

            bool m_Ascendance;

            void Reset() override
            {
                events.Reset();

                if (!m_Ascendance)
                {
                    if (Unit* l_Summoner = me->GetAnyOwner())
                    {
                        if (l_Summoner->IsAIEnabled && l_Summoner->GetAI()->GetData(DATA_HYRJA_EVENT_COMPLETE) == 1)
                            SpiritForm();
                    }
                }

                if (IsChallengeMode())
                    me->AddAura(ChallengeSpells::ChallengersMight, me);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->IsAIEnabled && !m_Ascendance)
                    m_Ascendance = p_Summoner->GetAI()->GetData(DATA_HYRJA_EVENT_COMPLETE) == 1;
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    if (!m_Ascendance)
                    {
                        if (Unit* l_Summoner = me->GetAnyOwner())
                        {
                            if (l_Summoner->ToCreature() && l_Summoner->ToCreature()->IsAIEnabled)
                                l_Summoner->ToCreature()->AI()->DoAction(true);
                        }

                        EnterEvadeMode();
                        SpiritForm();
                    }

                    p_Damage = 0;
                }
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (m_Ascendance)
                    return;

                Talk(eTalk::TalkAggro);

                if (me->GetEntry() == NPC_OLMYR_THE_ENLIGHTENED)
                {
                    events.ScheduleEvent(eEvents::EventSearingLight, 4 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventSanctify, 20 * IN_MILLISECONDS);
                }
                else
                    events.ScheduleEvent(eEvents::EventEyeOfStorm, 6 * IN_MILLISECONDS);
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);

                Reset();
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();

                me->AddUnitState(UNIT_STATE_EVADE);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTargetedHome(true);

                me->SetLastDamagedTime(0);
            }

            void SpiritForm()
            {
                m_Ascendance = true;

                Talk(eTalk::TalkSpirit);

                DoStopAttack();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_SERVER_CONTROLLED);

                me->SetReactState(REACT_PASSIVE);

                if (me->GetEntry() == NPC_OLMYR_THE_ENLIGHTENED)
                    DoCast(me, SPELL_OLMYR_VISUAL, true);
                else
                    DoCast(me, SPELL_SOLSTEN_VISUAL, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->GetAnyOwner())
                {
                    if (Creature* l_Hyrja = me->GetAnyOwner()->ToCreature())
                    {
                        if (l_Hyrja->isInCombat() && m_Ascendance && !me->IsWithinLOSInMap(l_Hyrja) && l_Hyrja->IsAIEnabled && l_Hyrja->AI()->GetData(DATA_HYRJA_EVENT_COMPLETE) == 1)
                            l_Hyrja->AI()->EnterEvadeMode();
                    }
                }

                if (m_Ascendance || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 30.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSearingLight:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, Spells::SPELL_OLMYR_LIGHT);

                        events.ScheduleEvent(eEvents::EventSearingLight, 4 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSanctify:
                    {
                        DoCast(me, Spells::SPELL_SANCTIFY_TICK);
                        events.ScheduleEvent(eEvents::EventSanctify, 20 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventEyeOfStorm:
                    {
                        DoCast(me, SPELL_EYE_OF_THE_STORM, true);
                        DoCast(me, SPELL_EYE_OF_THE_STORM_AT, true);
                        DoCast(SPELL_EYE_OF_THE_STORM_DMG);

                        events.ScheduleEvent(eEvents::EventEyeOfStorm, 30 * IN_MILLISECONDS);
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
            return new npc_hyrja_defendersAI(p_Creature);
        }
};

/// Mystic Empowerment Tracker - 192130
class spell_hyrja_empowerment_tracker : public SpellScriptLoader
{
    public:
        spell_hyrja_empowerment_tracker() : SpellScriptLoader("spell_hyrja_empowerment_tracker") { }

        class spell_hyrja_empowerment_tracker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hyrja_empowerment_tracker_AuraScript);

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                /// Thunder
                if (l_Target->HasAura(192008))
                {
                    if (!l_Target->HasAura(191924))
                    {
                        /// On Heroic and Mythic difficulties, Hyrja's stacks do not immediately disappear when she is dragged to the opposite side of the room. The stacks instead decay gradually over time.
                        if (l_Target->GetMap()->IsHeroicOrMythic())
                        {
                            if (Aura* l_Aura = l_Target->GetAura(SPELL_MYSTIC_MOD_DMG_HOLY))
                                l_Aura->ModStackAmount(-2);
                        }
                        else
                            l_Target->RemoveAura(SPELL_MYSTIC_MOD_DMG_HOLY);
                    }

                    l_Target->CastSpell(l_Target, SPELL_MYSTIC_MOD_DMG_THUNDER, true);
                }

                /// Holy
                if (l_Target->HasAura(191924))
                {
                    if (!l_Target->HasAura(192008))
                    {
                        /// On Heroic and Mythic difficulties, Hyrja's stacks do not immediately disappear when she is dragged to the opposite side of the room. The stacks instead decay gradually over time.
                        if (l_Target->GetMap()->IsHeroicOrMythic())
                        {
                            if (Aura* l_Aura = l_Target->GetAura(SPELL_MYSTIC_MOD_DMG_THUNDER))
                                l_Aura->ModStackAmount(-2);
                        }
                        else
                            l_Target->RemoveAura(SPELL_MYSTIC_MOD_DMG_THUNDER);
                    }

                    l_Target->CastSpell(l_Target, SPELL_MYSTIC_MOD_DMG_HOLY, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hyrja_empowerment_tracker_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hyrja_empowerment_tracker_AuraScript();
        }
};

/// Expel Light - 192048
class spell_hyrja_expel_light : public SpellScriptLoader
{
    public:
        spell_hyrja_expel_light() : SpellScriptLoader("spell_hyrja_expel_light") { }

        class spell_hyrja_expel_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hyrja_expel_light_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, SPELL_EXPEL_LIGHT_DMG, true);

                if (l_Caster->GetMap()->IsHeroicOrMythic())
                {
                    if (l_Caster->ToCreature() && l_Caster->ToCreature()->IsAIEnabled)
                        l_Caster->ToCreature()->AI()->SetData(DATA_HYRJA_EXPEL_LIGHT_STACKS, 0);

                    if (p_AurEff->GetBase()->GetStackAmount() > 1)
                        l_Caster->CastSpell(l_Caster, SPELL_EXPEL_LIGHT, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hyrja_expel_light_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hyrja_expel_light_AuraScript();
        }
};

/// Sanctify - 192158
class spell_hyrja_sanctify : public SpellScriptLoader
{
    public:
        spell_hyrja_sanctify() : SpellScriptLoader("spell_hyrja_sanctify") { }

        class spell_hyrja_sanctify_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hyrja_sanctify_AuraScript);

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint8 l_I = 0; l_I < 6; l_I++)
                    l_Caster->CastSpell(l_Caster, SPELL_SANCTIFY_AT, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hyrja_sanctify_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hyrja_sanctify_AuraScript();
        }
};

/// Eye of the Storm - 203963
class spell_hyrja_eye_storm_absorb : public SpellScriptLoader
{
    public:
        spell_hyrja_eye_storm_absorb() : SpellScriptLoader("spell_hyrja_eye_storm_absorb") { }

        class spell_hyrja_eye_storm_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hyrja_eye_storm_absorb_AuraScript);

            uint32 m_AbsorbSave = 0;

            enum eSpells
            {
                ChallengersMight = 206150
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;

                if (Unit* l_Caster = GetCaster())
                {
                    m_AbsorbSave = GetSpellInfo()->Effects[EFFECT_0].CalcValue(l_Caster);

                    InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                    if (!l_Instance)
                        return;

                    Creature* l_Hyrja = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NPC_HYRJA));
                    if (!l_Hyrja)
                        return;

                    if (AuraEffect const* l_AurEff = l_Hyrja->GetAuraEffect(eSpells::ChallengersMight, SpellEffIndex::EFFECT_1))
                        AddPct(m_AbsorbSave, l_AurEff->GetAmount());
                }
            }

            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (p_DmgInfo.GetAmount() < m_AbsorbSave)
                    p_AbsorbAmount = p_DmgInfo.GetAmount();
                else
                    p_AbsorbAmount = m_AbsorbSave;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hyrja_eye_storm_absorb_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_hyrja_eye_storm_absorb_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hyrja_eye_storm_absorb_AuraScript();
        }
};

/// Sanctify - 4885
class at_sanctify : public AreaTriggerScript
{
    public:
        at_sanctify() : AreaTriggerScript("at_sanctify") { }

        bool OnTrigger(Player* p_Player, AreaTriggerEntry const* /*p_AT*/) override
        {
            p_Player->CastSpell(p_Player, SPELL_SANCTIFY_DAMAGE, true);

            if (p_Player->GetMap()->IsMythic())
                p_Player->CastSpell(p_Player, SPELL_SANCTIFY_MYTHIC_AOE, true);

            return true;
        }
};

/// Hyrja introduction
class eventobject_hyrja_intro : public EventObjectScript
{
    public:
        eventobject_hyrja_intro() : EventObjectScript("eventobject_hyrja_intro")
        {
            m_Triggered = false;
        }

        bool m_Triggered;

        bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/) override
        {
            if (m_Triggered)
                return false;

            m_Triggered = true;
            p_Player->CastSpell(p_Player, SPELL_CONVERSATION, true);

            if (InstanceScript* l_Instance = p_Player->GetInstanceScript())
            {
                if (Creature* l_Aspirant = Creature::GetCreature(*p_Player, l_Instance->GetData64(NPC_VALARJAR_ASPIRANT)))
                {
                    std::list<Creature*> l_Maidens;
                    l_Aspirant->GetCreatureListWithEntryInGrid(l_Maidens, NPC_VALARJAR_SHIELDMAIDEN, 30.0f);

                    Position l_Pos = l_Aspirant->GetPosition();

                    l_Pos.m_positionX += 40.0f * std::cos(l_Pos.m_orientation);
                    l_Pos.m_positionY += 40.0f * std::sin(l_Pos.m_orientation);

                    l_Aspirant->SetWalk(false);
                    l_Aspirant->GetMotionMaster()->Clear();
                    l_Aspirant->GetMotionMaster()->MovePoint(1, l_Pos);
                    l_Aspirant->SetHomePosition(l_Pos);

                    l_Aspirant->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);

                    for (Creature* l_Maiden : l_Maidens)
                    {
                        l_Pos = l_Maiden->GetPosition();
                        l_Pos.m_positionX += 40.0f * std::cos(l_Pos.m_orientation);
                        l_Pos.m_positionY += 40.0f * std::sin(l_Pos.m_orientation);

                        l_Maiden->SetWalk(false);
                        l_Maiden->GetMotionMaster()->Clear();
                        l_Maiden->GetMotionMaster()->MovePoint(1, l_Pos);
                        l_Maiden->SetHomePosition(l_Pos);

                        l_Maiden->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                    }
                }
            }

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_hyrja()
{
    /// Boss
    new boss_hyrja();

    /// Creature
    new npc_hyrja_defenders();

    /// Spells
    new spell_hyrja_empowerment_tracker();
    new spell_hyrja_expel_light();
    new spell_hyrja_sanctify();
    new spell_hyrja_eye_storm_absorb();

    /// AreaTrigger
    new at_sanctify();

    /// EventObject
    new eventobject_hyrja_intro();
}
#endif
