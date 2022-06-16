////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

Position const g_MedivhSEventPos[3] =
{
    { -4581.10f, -2526.68f, 2876.63f, 0.0f },
    { -4618.89f, -2526.83f, 2876.63f, 0.0f },
    { -4599.80f, -2494.75f, 2876.63f, 0.0f }
};

/// Shade of Medivh - 114350
class boss_shade_of_medivh : public CreatureScript
{
    public:
        boss_shade_of_medivh() : CreatureScript("boss_shade_of_medivh") { }

        struct boss_shade_of_medivhAI : public BossAI
        {
            boss_shade_of_medivhAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataShadeOfMedivh)
            {
                SetCombatMovement(false);
                DoCast(eSpells::ArcaneChannelVis);
            }

            enum eEvents
            {
                AlternatingAbility  = 1,
                SpecialFrost        = 2,
                SpecialFire         = 3,
                SpecialArcane       = 4
            };

            enum eTalks
            {
                Aggro               = 0,
                TalkSpecialArcane   = 1,
                TalkSpecialFrost    = 2,
                TalkSpecialFire     = 3,
                Death               = 4,
                Intro               = 5,
                PlayerKilled        = 6,
                TalkInfernoBolt     = 7,
                TalkFrostbite       = 8,
                Wipe                = 9
            };

            enum eSpells
            {
                ArcaneChannelVis        = 189573,
                ManaRegen               = 228582,
                VOController            = 232314, ///< Unk
                Frostbite               = 227592,
                InfernoBolt             = 227615,
                InfernoBoltVis          = 228249,
                PiercingMissiles        = 227628,

                /// Special Arcane Ability
                GuardianImage           = 228334,
                GuardianImageSummon     = 228338,
                Dissolve                = 228572, ///< Invisible
                ReformVisualState1      = 228568,
                ReformVisualState2      = 228569,

                /// Special Fire Ability
                FlameWreathFilter       = 228269,
                FlameWreathAT           = 228257,

                /// Special Frost Ability
                CeaselessWinterAT       = 227779,

                /// Other
                DimensionFall           = 231851, ///< Unk
                DimensionFallMidFall    = 232630, ///< Unk
                SafeFall                = 231462,
                ReformVisual            = 228558,

                /// Achievement
                Paranoid                = 232658
            };

            enum eAchievements
            {
                ScaredStraight = 11432
            };

            bool m_SpecialAbility;
            uint8 m_SummonsDiedCount;
            uint8 m_InterruptRand;
            bool m_Achievement;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.68f;
            }

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetPower(POWER_MANA, 0);
                m_SpecialAbility = false;
                m_InterruptRand = 0;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                m_Achievement = false;
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::DataShadeOfMedivh, EncounterState::FAIL);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                _EnterCombat();
                me->InterruptNonMeleeSpells(false);
                DoCast(me, eSpells::ManaRegen, true);

                events.ScheduleEvent(eEvents::AlternatingAbility, 500);

                m_Achievement = true;
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->instance)
                    {
                        for (Map::PlayerList::const_iterator itr = instance->instance->GetPlayers().begin(); itr != instance->instance->GetPlayers().end(); ++itr)
                        {
                            if (Player* player = itr->getSource())
                            {
                                if (!player->HasAura(eSpells::Paranoid))
                                    m_Achievement = false;
                            }
                        }
                    }
                }
            }

            void JustReachedHome() override
            {
                DoCast(eSpells::ArcaneChannelVis);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);

                if (m_Achievement && IsMythic())
                {
                    if (InstanceScript* instance = me->GetInstanceScript())
                        instance->DoCompleteAchievement(eAchievements::ScaredStraight);
                }

                _JustDied();
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (!p_Victim->IsPlayer())
                    return;

                if (instance && instance->IsWipe())
                    Talk(eTalks::Wipe);
                else
                    Talk(eTalks::PlayerKilled);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::GuardianImage:
                    {
                        m_SummonsDiedCount = 0;
                        DoCast(me, eSpells::Dissolve, true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                        for (uint8 l_I = 0; l_I < 3; l_I++)
                            me->CastSpell(g_MedivhSEventPos[l_I], eSpells::GuardianImageSummon, true);

                        break;
                    }
                    case eSpells::ReformVisual:
                    {
                        if (!m_SummonsDiedCount)
                            DoCast(me, eSpells::ReformVisualState1, true);
                        else if (m_SummonsDiedCount == 1)
                            DoCast(me, eSpells::ReformVisualState2, true);
                        else
                        {
                            me->RemoveAurasDueToSpell(eSpells::ReformVisualState1);
                            me->RemoveAurasDueToSpell(eSpells::ReformVisualState2);
                            me->RemoveAurasDueToSpell(eSpells::Dissolve);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                            me->SetPower(POWER_MANA, 0);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DoCast(me, eSpells::ManaRegen, true);
                            m_SpecialAbility = false;
                            events.ScheduleEvent(eEvents::AlternatingAbility, 500);
                        }

                        m_SummonsDiedCount++;
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::CeaselessWinterAT || p_SpellInfo->Id == eSpells::FlameWreathFilter)
                {
                    me->SetPower(POWER_MANA, 0);
                    m_SpecialAbility = false;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (m_SpecialAbility)
                            break;

                        m_SpecialAbility = true;

                        switch (urand(0, 2))
                        {
                            case 0: ///< Special Frost
                            {
                                events.ScheduleEvent(eEvents::SpecialFrost, 500);
                                break;
                            }
                            case 1: ///< Special Fire
                            {
                                events.ScheduleEvent(eEvents::SpecialFire, 500);
                                break;
                            }
                            case 2: ///< Special Arcane
                            {
                                events.ScheduleEvent(eEvents::SpecialArcane, 500);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    case ACTION_2:
                    {
                        Talk(eTalks::Intro);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* /*p_SpellInfo*/, SpellInfo const* p_CurrSpellInfo, uint32 /*p_SchoolMask*/) override
            {
                switch (p_CurrSpellInfo->Id)
                {
                    case eSpells::Frostbite:
                    {
                        m_InterruptRand = RAND(2, 3);
                        break;
                    }
                    case eSpells::InfernoBolt:
                    {
                        m_InterruptRand = RAND(1, 3);
                        break;
                    }
                    case eSpells::PiercingMissiles:
                    {
                        if (Spell const* l_Spell = me->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL))
                        {
                            if (Unit* l_Target = l_Spell->GetUnitTarget())
                            {
                                if (Aura* l_Aura = l_Target->GetAura(eSpells::PiercingMissiles, me->GetGUID()))
                                    l_Aura->SetScriptData(0, 0);
                            }
                        }

                        m_InterruptRand = RAND(1, 2);
                        break;
                    }
                    default:
                        break;
                }

                events.CancelEvent(eEvents::AlternatingAbility);
                events.ScheduleEvent(eEvents::AlternatingAbility, 500);
            }

            void CastDefAbility()
            {
                switch (!m_InterruptRand ? urand(1, 3) : m_InterruptRand)
                {
                    case 1: ///< Frost
                    {
                        std::vector<int32> l_ExcludeAuras = {eSpells::Frostbite};
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true, l_ExcludeAuras);
                        if (!l_Target)
                            l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true);
                        if (!l_Target)
                            return;

                        if (roll_chance_i(25))
                            Talk(eTalks::TalkFrostbite);
                        DoCast(l_Target, eSpells::Frostbite);
                        break;
                    }
                    case 2: ///< Fire
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true);
                        if (!l_Target)
                            return;

                        if (roll_chance_i(25))
                            Talk(eTalks::TalkInfernoBolt);
                        DoCast(l_Target, eSpells::InfernoBoltVis, true);
                        DoCast(l_Target, eSpells::InfernoBolt);
                        break;
                    }
                    case 3: ///< Arcane
                    {
                        Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO);
                        if (!l_Target)
                            break;

                        DoCast(l_Target, eSpells::PiercingMissiles);
                        break;
                    }
                    default:
                        break;
                }

                m_InterruptRand = 0;
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
                    case eEvents::AlternatingAbility:
                    {
                        CastDefAbility();
                        events.ScheduleEvent(eEvents::AlternatingAbility, 4000);
                        return;
                    }
                    case eEvents::SpecialFrost:
                    {
                        Talk(eTalks::TalkSpecialFrost);
                        DoCast(eSpells::CeaselessWinterAT);
                        return;
                    }
                    case eEvents::SpecialFire:
                    {
                        Talk(eTalks::TalkSpecialFire);
                        DoCast(eSpells::FlameWreathFilter);
                        return;
                    }
                    case eEvents::SpecialArcane:
                    {
                        Talk(eTalks::TalkSpecialArcane);
                        events.CancelEvent(eEvents::AlternatingAbility);
                        me->StopAttack();
                        me->RemoveAurasDueToSpell(eSpells::ManaRegen);
                        DoCast(eSpells::GuardianImage);
                        return;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_shade_of_medivhAI(p_Creature);
        }
};

/// Guardian's Image - 114675
class npc_medivh_guardian_image : public CreatureScript
{
    public:
        npc_medivh_guardian_image() : CreatureScript("npc_medivh_guardian_image") { }

        struct npc_medivh_guardian_imageAI : public Scripted_NoMovementAI
        {
            npc_medivh_guardian_imageAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpells
            {
                ArcaneDissolve  = 228343,
                Shimmer         = 228521,
                ArcaneBolt      = 228991,
                ReformVisual    = 228558
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.37f;
            }

            void Reset() override
            {
                SetCombatMovement(false);

                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::ArcaneDissolve, true);

                events.ScheduleEvent(EVENT_1, 2000);
                events.ScheduleEvent(EVENT_2, 2000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::ReformVisual, true);
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
                        DoCast(me, eSpells::Shimmer, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                        {
                            DoResetThreat();
                            me->AddThreat(l_Target, 100000.0f);
                            DoCast(l_Target, eSpells::ArcaneBolt);
                        }

                        events.ScheduleEvent(EVENT_2, 2000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_medivh_guardian_imageAI (p_Creature);
        }
};

/// Arcane Channel - 189573
class spell_medivh_arcane_channel : public SpellScriptLoader
{
    public:
        spell_medivh_arcane_channel() : SpellScriptLoader("spell_medivh_arcane_channel") { }

        class spell_medivh_arcane_channel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_medivh_arcane_channel_AuraScript);

            Position m_Position;
            uint16 m_TickTimer = 1000;

            void OnUpdate(uint32 p_Diff, AuraEffect* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                if (m_TickTimer <= 0)
                {
                    for (uint8 l_I = 0; l_I < 5; l_I++)
                    {
                        GetCaster()->GetNearPosition(m_Position, 20.0f, frand(-M_PI / 3.0f, M_PI / 3.0f));
                        GetCaster()->SendSpellCreateVisual(GetSpellInfo(), &m_Position);
                    }

                    m_TickTimer = 1000;
                }
                else
                    m_TickTimer -= p_Diff;
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_medivh_arcane_channel_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_medivh_arcane_channel_AuraScript();
        }
};

/// Mana Regen - 228582
class spell_medivh_mana_regen : public SpellScriptLoader
{
    public:
        spell_medivh_mana_regen() : SpellScriptLoader("spell_medivh_mana_regen") { }

        class spell_medivh_mana_regen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_medivh_mana_regen_AuraScript);

            uint8 m_Counter = 0;
            uint8 m_ManaPct = 0;

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat() || !l_Caster->IsAIEnabled)
                    return;

                if (l_Caster->GetPowerPct(POWER_MANA) < 100.0f)
                {
                    if (!m_Counter)
                        m_ManaPct += 4;
                    else
                        m_ManaPct += 3;

                    m_Counter >= 2 ? m_Counter = 0 : m_Counter++;
                    l_Caster->SetPower(POWER_MANA, l_Caster->CountPctFromMaxMana(m_ManaPct));
                }
                else
                {
                    m_ManaPct = 0;
                    l_Caster->AI()->DoAction(ACTION_1);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_medivh_mana_regen_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_medivh_mana_regen_AuraScript();
        }
};

/// Ceaseless Winter - 228232
class spell_medivh_ceaseless_winter : public SpellScriptLoader
{
    public:
        spell_medivh_ceaseless_winter() : SpellScriptLoader("spell_medivh_ceaseless_winter") { }

        enum eSpells
        {
            SpellCeaselessWinterDot = 227806
        };

        class spell_medivh_ceaseless_winter_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_medivh_ceaseless_winter_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetTarget() && GetTarget()->IsMoving())
                {
                    if (Aura* l_Aura = GetTarget()->GetAura(eSpells::SpellCeaselessWinterDot))
                        l_Aura->Remove();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_medivh_ceaseless_winter_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_medivh_ceaseless_winter_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Piercing Missiles - 227628
class spell_rtk_piercing_missiles : public SpellScriptLoader
{
    public:
        spell_rtk_piercing_missiles() : SpellScriptLoader("spell_rtk_piercing_missiles") { }

        class spell_rtk_piercing_missiles_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_piercing_missiles_AuraScript);

            enum eSpells
            {
                PiercingMissilesDebuff = 227644
            };

            bool m_Interrupted = false;

            void SetData(uint32 /*p_Type*/, uint32 /*p_Data*/) override
            {
                m_Interrupted = true;
            }

            void HandleOnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Modes*/)
            {
                if (m_Interrupted)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PiercingMissilesDebuff, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_piercing_missiles_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_piercing_missiles_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Flame Wreath - 228269
class spell_rtk_flame_wreath : public SpellScriptLoader
{
    public:
        spell_rtk_flame_wreath() : SpellScriptLoader("spell_rtk_flame_wreath") { }

        class spell_rtk_flame_wreath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_flame_wreath_SpellScript);

            enum eSpells
            {
                FlameWreath = 228257
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FlameWreath, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rtk_flame_wreath_SpellScript::HandleAfterHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_flame_wreath_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_flame_wreath_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Flame Wreath - 8851
class at_rtk_flame_wreath : public AreaTriggerEntityScript
{
    public:
        at_rtk_flame_wreath() : AreaTriggerEntityScript("at_rtk_flame_wreath") { }

        enum eSpells
        {
            FlameWreath = 228262
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            SpellInfo const* l_SpellInfo = p_Areatrigger->GetSpellInfo();
            if (!l_SpellInfo || !p_Target->IsPlayer())
                return true;

            if (l_SpellInfo->GetMaxDuration() - p_Areatrigger->GetDuration() < 500)
                return false;

            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return true;

            l_Caster->CastSpell(p_Target, eSpells::FlameWreath, true);
            p_Areatrigger->SetDuration(0);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_rtk_flame_wreath();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Ceaseless Winter - 228222
class spell_rtk_ceaseless_winter : public SpellScriptLoader
{
    public:
        spell_rtk_ceaseless_winter() : SpellScriptLoader("spell_rtk_ceaseless_winter") { }

        class spell_rtk_ceaseless_winter_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_ceaseless_winter_AuraScript);

            enum eSpells
            {
                CeaselessWinterDot = 227806
            };

            void HandleOnPeriodic(AuraEffect const* /*p_Auraeffect*/)
            {
                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner)
                    return;

                if (!l_Owner->IsMoving())
                    l_Owner->CastSpell(l_Owner, eSpells::CeaselessWinterDot, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rtk_ceaseless_winter_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_ceaseless_winter_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Paranoid - 232658
class spell_rtk_paranoid : public SpellScriptLoader
{
    public:
        spell_rtk_paranoid() : SpellScriptLoader("spell_rtk_paranoid") { }

        class spell_rtk_paranoid_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_paranoid_AuraScript);

            enum eSpells
            {
                Panic = 232662
            };

            Position m_Position;
            bool m_LastCheckWasPanic;

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_Position = *l_Caster;
            }

            void HandleOnPeriodic(AuraEffect const* /*p_Auraeffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_Position == *l_Caster)
                    return;

                m_Position = *l_Caster;

                if (l_Caster->HasAura(eSpells::Panic))
                {
                    m_LastCheckWasPanic = true;
                    return;
                }

                if (m_LastCheckWasPanic)
                {
                    m_LastCheckWasPanic = false;
                    return;
                }

                l_Caster->CastSpell(l_Caster, eSpells::Panic, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rtk_paranoid_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rtk_paranoid_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_paranoid_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_shade_of_medivh()
{
    /// Boss
    new boss_shade_of_medivh();

    /// Creature
    new npc_medivh_guardian_image();

    /// Spells
    new spell_medivh_arcane_channel();
    new spell_medivh_mana_regen();
    new spell_medivh_ceaseless_winter();
    new spell_rtk_piercing_missiles();
    new spell_rtk_flame_wreath();
    new at_rtk_flame_wreath();
    new spell_rtk_ceaseless_winter();

    /// Achievements
    new spell_rtk_paranoid();

    /// Areatriggers
    new at_rtk_flame_wreath();
}
#endif
