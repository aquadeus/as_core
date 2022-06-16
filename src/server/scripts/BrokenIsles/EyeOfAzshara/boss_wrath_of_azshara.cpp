////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "eye_of_azshara.hpp"

enum Says
{
    SAY_START           = 1,
    SAY_AGGRO           = 2,
    SAY_DELUGE          = 3,
    SAY_ARCANE_EMOTE    = 4,
    SAY_ARCANE          = 5,
    SAY_CRY_EMOTE       = 6,
    SAY_CRY             = 7,
    SAY_STORMS          = 8,
    SAY_DEATH           = 9
};

enum Spells
{
    /// Pre-event spells
    SPELL_SURGING_WATERS        = 192632,
    SPELL_TIDAL_WAVE            = 192793,
    SPELL_TIDAL_WAVE_AT         = 192753,
    SPELL_DAMAGE_SELF_20PCT     = 193500,
    SPELL_TEMPEST_ATTUNEMENT    = 193491,
    SPELL_STORM_CONDUIT         = 193196,
    SPELL_RISING_TIDE           = 193497,
    SPELL_VIOLENT_WINDS         = 191797,
    SPELL_PERM_VIOLENT_WINDS    = 192649,

    SPELL_MYSTIC_TORNADO        = 192680,
    SPELL_MASSIVE_DELUGE        = 192620,
    SPELL_ARCANE_BOMB           = 192705,
    SPELL_ARCANE_BOMB_VEH_SEAT  = 192706,
    SPELL_ARCANE_BOMB_VISUAL    = 192711,
    SPELL_ARCANE_BOMB_DMG       = 192708,
    SPELL_CRY_OF_WRATH          = 192985,
    SPELL_RAGING_STORMS         = 192696,
    SPELL_HEAVING_SANDS         = 197164,

    /// Heroic
    SPELL_CRUSHING_DEPTHS       = 197365,
    SPELL_MAGIC_RESONANCE       = 196665,
    SPELL_FROST_RESONANCE       = 196666,

    SPELL_MYSTIC_TORNADO_AT     = 192673,
    SPELL_LIGHTNING_STRIKES_1   = 192989, ///< 2s tick
    SPELL_LIGHTNING_STRIKES_2   = 192990, ///< 7s tick

    SPELL_BONUS_ROLL            = 226624
};

enum eEvents
{
    EVENT_MYSTIC_TORNADO        = 1,
    EVENT_MASSIVE_DELUGE        = 2,
    EVENT_ARCANE_BOMB           = 3,
    EVENT_CRY_OF_WRATH          = 4,
    EVENT_RAGING_STORMS         = 5,
    /// Heroic
    EVENT_CRUSHING_DEPTHS       = 6,
    /// Cosmetic
    EVENT_COSMETIC_TIDAL_WAVE,
    EVENT_COSMETIC_TIDAL_WAVE_2
};

Position const g_NagaPos[4] =
{
    { -3511.70f, 4479.83f, 1.61f, 5.00f },
    { -3474.94f, 4281.46f, 1.89f, 1.65f },
    { -3382.71f, 4354.07f, 1.39f, 2.83f },
    { -3415.23f, 4442.80f, 1.09f, 3.79f }
};

Position const g_TidalWavePos = { -3484.453f, 4386.284f, 0.7607061f, 0.0f };

/// Wrath of Azshara - 96028
class boss_wrath_of_azshara : public CreatureScript
{
    public:
        boss_wrath_of_azshara() : CreatureScript("boss_wrath_of_azshara") { }

        struct boss_wrath_of_azsharaAI : public BossAI
        {
            boss_wrath_of_azsharaAI(Creature* p_Creature) : BossAI(p_Creature, DATA_WRATH_OF_AZSHARA)
            {
                SetCombatMovement(false);
                me->SetVisible(false);
                me->setRegeneratingHealth(false);
                me->SetReactState(REACT_PASSIVE);
                me->setFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                SummonNagas();
                m_NagaDiedCount = 0;
            }

            uint8 m_NagaDiedCount;
            uint16 m_CheckVictimTimer;
            bool m_LowHP;
            bool m_CryOfWrath;

            EventMap m_CosmeticEvents;

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                _Reset();
                m_LowHP = false;
                m_CheckVictimTimer = 2000;

                if (!me->HasAura(SPELL_SURGING_WATERS))
                    DoCast(me, SPELL_SURGING_WATERS, true);

                DoCast(me, SPELL_HEAVING_SANDS, true);

                me->SetHealth(me->CountPctFromMaxHealth(21));

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGIC_RESONANCE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROST_RESONANCE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PERM_VIOLENT_WINDS);

                    instance->SetData(0, false);
                }

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                if (m_NagaDiedCount == 4)
                {
                    for (uint8 l_I = 0; l_I < m_NagaDiedCount; ++l_I)
                        DoCast(me, SPELL_RISING_TIDE, true);
                }

                m_CosmeticEvents.Reset();

                m_CosmeticEvents.ScheduleEvent(eEvents::EVENT_COSMETIC_TIDAL_WAVE, 1000);

                m_CryOfWrath = false;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);

                _EnterCombat();

                DoCast(me, SPELL_TIDAL_WAVE, true);

                events.ScheduleEvent(EVENT_MYSTIC_TORNADO, 9000);
                events.ScheduleEvent(EVENT_MASSIVE_DELUGE, 12000);
                events.ScheduleEvent(EVENT_ARCANE_BOMB, 23000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_CRUSHING_DEPTHS, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                me->RemoveAllAreasTrigger();

                /// Make it lootable
                me->SetFlag(UNIT_FIELD_FLAGS, 0);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGIC_RESONANCE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROST_RESONANCE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PERM_VIOLENT_WINDS);

                    instance->SetData(0, false);
                }

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(10, p_Damage) && !m_LowHP)
                {
                    m_LowHP = true;
                    events.ScheduleEvent(EVENT_CRY_OF_WRATH, 0);
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_ARCANE_BOMB:
                    {
                        if (p_Target->IsPlayer())
                            Talk(SAY_ARCANE_EMOTE, p_Target->ToPlayer()->GetGUID());

                        Talk(SAY_ARCANE);
                        break;
                    }
                    case SPELL_ARCANE_BOMB_VEH_SEAT:
                    {
                        if (Creature* l_Bomb = me->SummonCreature(NPC_ARCANE_BOMB, p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 17000))
                        {
                            l_Bomb->EnterVehicle(p_Target);
                            l_Bomb->CastSpell(me, SPELL_ARCANE_BOMB_VISUAL, TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                            l_Bomb->CastSpell(me, SPELL_ARCANE_BOMB_DMG, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                        }

                        break;
                    }
                    case SPELL_TIDAL_WAVE:
                    {
                        p_Target->CastSpell(p_Target, SPELL_TIDAL_WAVE_AT, true);
                        break;
                    }
                    default:
                        break;
                }

                if (IsHeroicOrMythic())
                {
                    switch (p_SpellInfo->Id)
                    {
                        case 192619: ///< Massive Deluge
                        case SPELL_RAGING_STORMS:
                        {
                            DoCast(p_Target, SPELL_FROST_RESONANCE, true);
                            break;
                        }
                        case SPELL_ARCANE_BOMB_DMG:
                        {
                            DoCast(p_Target, SPELL_MAGIC_RESONANCE, true);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void SummonNagas()
            {
                me->SummonCreature(NPC_MYSTIC_SSAVEH, g_NagaPos[0]);
                me->SummonCreature(NPC_RITUALIST_LESHA, g_NagaPos[1]);
                me->SummonCreature(NPC_CHANNELER_VARISZ, g_NagaPos[2]);
                me->SummonCreature(NPC_BINDER_ASHIOI, g_NagaPos[3]);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case NPC_MYSTIC_SSAVEH:
                    case NPC_RITUALIST_LESHA:
                    case NPC_CHANNELER_VARISZ:
                    case NPC_BINDER_ASHIOI:
                    {
                        m_NagaDiedCount++;
                        DoCast(me, SPELL_RISING_TIDE, true);
                        break;
                    }
                    default:
                        return;
                }

                if (m_NagaDiedCount == 4)
                {
                    me->RemoveAurasDueToSpell(SPELL_STORM_CONDUIT);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetReactState(REACT_DEFENSIVE);
                    me->SetVisible(true);
                    me->SetHealth(me->CountPctFromMaxHealth(21));
                    Talk(SAY_START);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_CRY_OF_WRATH)
                    m_CryOfWrath = true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_CosmeticEvents.Update(p_Diff);

                switch (m_CosmeticEvents.ExecuteEvent())
                {
                    case eEvents::EVENT_COSMETIC_TIDAL_WAVE:
                    {
                        if (Creature* l_Trigger = me->SummonCreature(eCreatures::NPC_TIDAL_WAVE, g_TidalWavePos))
                        {
                            l_Trigger->SetReactState(ReactStates::REACT_PASSIVE);
                            l_Trigger->SetDisplayId(INVISIBLE_CREATURE_MODEL);
                        }

                        DoCast(me, SPELL_TIDAL_WAVE, true);
                        m_CosmeticEvents.ScheduleEvent(eEvents::EVENT_COSMETIC_TIDAL_WAVE_2, 45 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EVENT_COSMETIC_TIDAL_WAVE_2:
                    {
                        DoCast(me, SPELL_TIDAL_WAVE, true);
                        m_CosmeticEvents.ScheduleEvent(eEvents::EVENT_COSMETIC_TIDAL_WAVE_2, m_CryOfWrath ? 20 * TimeConstants::IN_MILLISECONDS : 45 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (m_CheckVictimTimer <= p_Diff)
                {
                    if (me->getVictim())
                    {
                        if (!me->IsWithinMeleeRange(me->getVictim()))
                            events.ScheduleEvent(EVENT_RAGING_STORMS, 500);
                    }

                    m_CheckVictimTimer = 2000;
                }
                else
                    m_CheckVictimTimer -= p_Diff;

                switch (events.ExecuteEvent())
                {
                    case EVENT_MYSTIC_TORNADO:
                    {
                        DoCast(me, SPELL_MYSTIC_TORNADO, true);
                        events.ScheduleEvent(EVENT_MYSTIC_TORNADO, 25000);
                        break;
                    }
                    case EVENT_MASSIVE_DELUGE:
                    {
                        me->CastSpell(me->getVictim(), SPELL_MASSIVE_DELUGE, true);
                        Talk(SAY_DELUGE);
                        events.ScheduleEvent(EVENT_MASSIVE_DELUGE, 48000);
                        break;
                    }
                    case EVENT_ARCANE_BOMB:
                    {
                        DoCast(SPELL_ARCANE_BOMB);
                        events.ScheduleEvent(EVENT_ARCANE_BOMB, 30000);
                        break;
                    }
                    case EVENT_CRY_OF_WRATH:
                    {
                        DoCast(SPELL_CRY_OF_WRATH);
                        Talk(SAY_CRY_EMOTE);
                        Talk(SAY_CRY);

                        if (instance)
                        {
                            instance->SetData(0, true);
                            instance->DoCastSpellOnPlayers(SPELL_VIOLENT_WINDS);
                            instance->DoCastSpellOnPlayers(SPELL_PERM_VIOLENT_WINDS);
                        }

                        break;
                    }
                    case EVENT_RAGING_STORMS:
                    {
                        DoCast(SPELL_RAGING_STORMS);
                        Talk(SAY_STORMS);
                        break;
                    }
                    case EVENT_CRUSHING_DEPTHS:
                    {
                        if (Unit* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, {}, 100.0f))
                            me->CastSpell(l_Target, SPELL_CRUSHING_DEPTHS, false);
                        events.ScheduleEvent(EVENT_CRUSHING_DEPTHS, 40000);
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
            return new boss_wrath_of_azsharaAI(p_Creature);
        }
};

/// Mystic Ssa'veh <The Storm> - 98173
/// Ritualist Lesha <The Raging Sea> - 100248
/// Channeler Varisz <The Locus> - 100249
/// Binder Ashioi <The Cold Wind> - 100250
class npc_wrath_of_azshara_nagas : public CreatureScript
{
    public:
        npc_wrath_of_azshara_nagas() : CreatureScript("npc_wrath_of_azshara_nagas") { }

        struct npc_wrath_of_azshara_nagasAI : public ScriptedAI
        {
            npc_wrath_of_azshara_nagasAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_ActivateConduit;

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_ActivateConduit = false;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                DoCast(me, SPELL_TEMPEST_ATTUNEMENT, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(0);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                me->CastStop();
                events.ScheduleEvent(EVENT_1, 6000);
                events.ScheduleEvent(EVENT_2, 9000);

                if (me->GetEntry() != 98173)
                    events.ScheduleEvent(EVENT_3, 18000);
            }


            void DoAction(int32 const /*p_Action*/) override
            {
                if (m_ActivateConduit)
                    return;

                m_ActivateConduit = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_AGGRESSIVE);
                me->InterruptNonMeleeSpells(false, SPELL_TEMPEST_ATTUNEMENT);
                DoCast(me, SPELL_STORM_CONDUIT, true);
            }

            void JustReachedHome() override
            {
                DoCast(me, SPELL_STORM_CONDUIT, true);
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
                        DoCast(196516);
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (me->GetEntry() == 100250)
                            DoCast(196515);
                        if (me->GetEntry() == 98173)
                            DoCast(196870);
                        if (me->GetEntry() == 100249)
                            DoCast(197105);
                        if (me->GetEntry() == 100248)
                            DoCast(196027);
                        events.ScheduleEvent(EVENT_2, 18000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(192705);
                        events.ScheduleEvent(EVENT_3, 18000);
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
            return new npc_wrath_of_azshara_nagasAI(p_Creature);
        }
};

/// Mystic Tornado - 97673
class npc_wrath_of_azshara_mystic_tornado : public CreatureScript
{
    public:
        npc_wrath_of_azshara_mystic_tornado() : CreatureScript("npc_wrath_of_azshara_mystic_tornado") { }

        struct npc_wrath_of_azshara_mystic_tornadoAI : public ScriptedAI
        {
            npc_wrath_of_azshara_mystic_tornadoAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                me->GetMotionMaster()->MoveRandom(15.0f);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->isInCombat())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                DoCast(me, SPELL_MYSTIC_TORNADO_AT, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_wrath_of_azshara_mystic_tornadoAI(p_Creature);
        }
};

/// Arcane Bomb - 97691
class npc_wrath_of_azshara_arcane_bomb : public CreatureScript
{
    public:
        npc_wrath_of_azshara_arcane_bomb() : CreatureScript("npc_wrath_of_azshara_arcane_bomb") { }

        struct npc_wrath_of_azshara_arcane_bombAI : public ScriptedAI
        {
            npc_wrath_of_azshara_arcane_bombAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(INVISIBLE_CREATURE_MODEL);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->isInCombat())
                {
                    me->DespawnOrUnsummon();
                    return;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (IsHeroicOrMythic())
                {
                    switch (p_SpellInfo->Id)
                    {
                        case SPELL_ARCANE_BOMB_DMG:
                        {
                            DoCast(p_Target, SPELL_MAGIC_RESONANCE, true);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_wrath_of_azshara_arcane_bombAI(p_Creature);
        }
};

/// Violent Winds (perm) - 192649
class spell_wrath_of_azshara_violent_winds : public SpellScriptLoader
{
    public:
        spell_wrath_of_azshara_violent_winds() : SpellScriptLoader("spell_wrath_of_azshara_violent_winds") { }

        class spell_wrath_of_azshara_violent_winds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_wrath_of_azshara_violent_winds_AuraScript);

            Position const m_ForceCenter = { -3486.26f, 4386.87f, -3.58042f, 0.0f };

            G3D::Vector3 m_ForceDirections[4] =
            {
                { -0.0000000874228f,    2.0f,               0.0f },
                { 2.0f,                 0.0f,               0.0f },
                { -2.0f,                -0.00000001748456f, 0.0f },
                { 0.000000002384976f,   -2.0f,              0.0f }
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (Aura* l_Aura = l_Player->GetAura(SPELL_VIOLENT_WINDS, l_Player->GetGUID()))
                    {
                        l_Aura->SetMaxDuration(-1);
                        l_Aura->SetDuration(-1);
                    }
                }
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    /// Just for visual
                    if (Aura* l_Aura = l_Player->GetAura(SPELL_VIOLENT_WINDS, l_Player->GetGUID()))
                        l_Aura->ChangeWindDirection(m_ForceDirections[urand(0, 3)]);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (Aura* l_Aura = l_Player->GetAura(SPELL_VIOLENT_WINDS, l_Player->GetGUID()))
                        l_Aura->Remove();
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_wrath_of_azshara_violent_winds_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_wrath_of_azshara_violent_winds_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_wrath_of_azshara_violent_winds_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_wrath_of_azshara_violent_winds_AuraScript();
        }
};

/// Tidal Waves (damage) - 192801
class spell_wrath_of_azshara_tidal_waves : public SpellScriptLoader
{
    public:
        spell_wrath_of_azshara_tidal_waves() : SpellScriptLoader("spell_wrath_of_azshara_tidal_waves") { }

        class spell_wrath_of_azshara_tidal_waves_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_wrath_of_azshara_tidal_waves_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->IsHeroicOrMythic() && GetHitUnit())
                        l_Caster->CastSpell(GetHitUnit(), SPELL_FROST_RESONANCE, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_wrath_of_azshara_tidal_waves_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_wrath_of_azshara_tidal_waves_SpellScript();
        }
};

/// Mystic Tornado (damages) - 192675
class spell_wrath_of_azshara_mystic_tornado : public SpellScriptLoader
{
    public:
        spell_wrath_of_azshara_mystic_tornado() : SpellScriptLoader("spell_wrath_of_azshara_mystic_tornado") { }

        class spell_wrath_of_azshara_mystic_tornado_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_wrath_of_azshara_mystic_tornado_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->IsHeroicOrMythic() && GetHitUnit())
                        l_Caster->CastSpell(GetHitUnit(), SPELL_MAGIC_RESONANCE, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_wrath_of_azshara_mystic_tornado_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_wrath_of_azshara_mystic_tornado_SpellScript();
        }
};

/// Tidal Wave - 192753
class at_wrath_of_azshara_tidal_wave : public AreaTriggerEntityScript
{
    public:
        at_wrath_of_azshara_tidal_wave() : AreaTriggerEntityScript("at_wrath_of_azshara_tidal_wave") { }

        void BeforeCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 10.77033f);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_wrath_of_azshara_tidal_wave();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_wrath_of_azshara()
{
    /// Boss
    new boss_wrath_of_azshara();

    /// Creatures
    new npc_wrath_of_azshara_nagas();
    new npc_wrath_of_azshara_mystic_tornado();
    new npc_wrath_of_azshara_arcane_bomb();

    /// Spells
    new spell_wrath_of_azshara_violent_winds();
    new spell_wrath_of_azshara_tidal_waves();
    new spell_wrath_of_azshara_mystic_tornado();

    /// AreaTrigger
    new at_wrath_of_azshara_tidal_wave();
}
#endif
