////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

/// Last Update 7.1.5 Build 23420
/// Charitable Donation - 228010
class spell_rtk_charitable_donation : public SpellScriptLoader
{
    public:
        spell_rtk_charitable_donation() : SpellScriptLoader("spell_rtk_charitable_donation") { }

        class spell_rtk_charitable_donation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_charitable_donation_SpellScript);

            enum eSpells
            {
                DeadBroke = 228014
            };

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || (l_Caster->GetEntry() != eCreatures::SpectralPatron) || !l_Target || l_Target->isPet())
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DeadBroke, true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_rtk_charitable_donation_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_charitable_donation_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Purify - 232084
class spell_rtk_purify : public SpellScriptLoader
{
    public:
        spell_rtk_purify() : SpellScriptLoader("spell_rtk_purify") { }

        class spell_rtk_purify_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_purify_SpellScript);

            enum eSpells
            {
                PurifyDraw = 232087
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_SpellInfo || !l_Caster)
                    return;

                WorldObject* l_Target = nullptr;
                for (WorldObject* l_WorldObject : p_Targets)
                {
                    Unit* l_UnitTarget = l_WorldObject->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    DispelChargesList l_DispelList;
                    l_UnitTarget->GetDispellableAuraList(l_Caster, 1 << l_SpellInfo->Effects[EFFECT_0].MiscValue, l_DispelList);

                    if (l_DispelList.empty())
                        continue;

                    l_Target = l_UnitTarget;
                    break;
                }

                p_Targets.clear();

                if (l_Target)
                    p_Targets.push_back(l_Target);
            }

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::PurifyDraw, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_purify_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHit += SpellEffectFn(spell_rtk_purify_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_purify_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Flashlight - 227977
class spell_rtk_flashlight : public SpellScriptLoader
{
    public:
        spell_rtk_flashlight() : SpellScriptLoader("spell_rtk_flashlight") { }

        class spell_rtk_flashlight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_flashlight_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<WorldObject*> l_RealTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    if (l_UnitTarget->HasInArc(M_PI, l_Caster))
                        l_RealTargets.push_back(l_UnitTarget);
                }

                p_Targets = l_RealTargets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_flashlight_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_flashlight_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Phantom Guest - 114625
class npc_rtk_phantom_guest : public CreatureScript
{
    public:
        npc_rtk_phantom_guest() : CreatureScript("npc_rtk_phantom_guest") { }

        struct npc_rtk_phantom_guestAI : public ScriptedAI
        {
            npc_rtk_phantom_guestAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                /// Tank
                SearingPain     = 29492,

                /// Mono Target Damage
                CurseOfAgony    = 29930,
                Punch           = 29581,
                Immolate        = 29928,
                Impale          = 29583,

                /// AoE Damage
                HolyNova        = 29514,
                GoblinDragonGun = 29513,
                ThrowDynamite   = 29579,

                /// Healing spell
                Heal            = 29580
            };

            std::vector<uint32> m_MonoTargetDamageSpells =
            {
                eSpells::CurseOfAgony,
                eSpells::Punch,
                eSpells::Immolate,
                eSpells::Impale
            };

            std::vector<uint32> m_AOETargetDamageSpells =
            {
                eSpells::HolyNova,
                eSpells::GoblinDragonGun,
                eSpells::ThrowDynamite
            };

            int32 m_TankSpellTimer;
            int32 m_MonoTargetSpellTimer;
            int32 m_AOESpellTimer;
            int32 m_HealingSpellTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                int32 m_TankSpellTimer = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
                int32 m_MonoTargetSpellTimer = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
                int32 m_AOESpellTimer = urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS);
                int32 m_HealingSpellTimer = urand(15 * IN_MILLISECONDS, 18 * IN_MILLISECONDS);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::GoblinDragonGun:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        }, 100);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_TankSpellTimer -= p_Diff;
                m_MonoTargetSpellTimer -= p_Diff;
                m_AOESpellTimer -= p_Diff;
                m_HealingSpellTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_TankSpellTimer < 0)
                {
                    DoCastVictim(eSpells::SearingPain);
                    m_TankSpellTimer = urand(5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS);
                }

                if (m_MonoTargetSpellTimer < 0)
                {
                    Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true);
                    if (!l_Victim)
                        return;

                    me->CastSpell(l_Victim, m_MonoTargetDamageSpells[urand(0, m_MonoTargetDamageSpells.size() - 1)], false);
                    m_MonoTargetSpellTimer = urand(10 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
                    return;
                }

                if (m_AOESpellTimer < 0)
                {
                    uint32 l_AoeSpellCount = 3;
                    Unit* l_RangedVictim = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllRanged);
                    if (!l_RangedVictim)
                        l_AoeSpellCount--;

                    uint32 l_SpellId = m_AOETargetDamageSpells[urand(0, l_AoeSpellCount - 1)];
                    switch (l_SpellId)
                    {
                        case eSpells::ThrowDynamite:
                        {
                            me->CastSpell(l_RangedVictim, eSpells::ThrowDynamite, false);
                            break;
                        }
                        case eSpells::GoblinDragonGun:
                        {
                            Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true);
                            if (!l_Victim)
                                return;

                            me->SetFacingToObject(l_Victim);
                            me->NearTeleportTo(*me);
                            me->CastSpell(l_Victim, eSpells::GoblinDragonGun, false);
                            me->AddDelayedEvent([this]() -> void
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            }, 50);
                            break;
                        }
                        case eSpells::HolyNova:
                        {
                            DoCast(eSpells::HolyNova);
                            break;
                        }
                        default:
                            break;
                    }

                    m_AOESpellTimer = urand(10 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
                }

                if (m_HealingSpellTimer < 0)
                {
                    float l_Radius = 30.0f;
                    UnitList l_Allies;
                    JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(me, me, l_Radius);
                    JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(me, l_Allies, u_check);
                    me->VisitNearbyObject(l_Radius, l_Searcher);

                    for (Unit* l_Ally : l_Allies)
                    {
                        if (l_Ally->GetHealthPct() < 90.0f)
                        {
                            me->CastSpell(l_Ally, eSpells::Heal, false);
                            break;
                        }
                    }

                    m_HealingSpellTimer = urand(15 * IN_MILLISECONDS, 17 * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_phantom_guestAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Volatile Charge - 228320
class spell_rtk_volatile_charge : public SpellScriptLoader
{
    public:
        spell_rtk_volatile_charge() : SpellScriptLoader("spell_rtk_volatile_charge") { }

        class spell_rtk_volatile_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_volatile_charge_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 2);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_Caster || !l_SpellInfo)
                    return;

                l_Caster->CastSpell(l_Target, l_SpellInfo->Effects[EFFECT_0].BasePoints, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_volatile_charge_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_rtk_volatile_charge_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_volatile_charge_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Volatile Charge - 228331
class spell_rtk_volatile_charge_explosion : public SpellScriptLoader
{
    public:
        spell_rtk_volatile_charge_explosion() : SpellScriptLoader("spell_rtk_volatile_charge_explosion") { }

        class spell_rtk_volatile_charge_explosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_volatile_charge_explosion_AuraScript);

            enum eSpells
            {
                VolatileChargeExplosion = 228333
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::VolatileChargeExplosion, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_volatile_charge_explosion_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_volatile_charge_explosion_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Howl - 228567
class spell_rtk_howl : public SpellScriptLoader
{
    public:
        spell_rtk_howl() : SpellScriptLoader("spell_rtk_howl") { }

        class spell_rtk_howl_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_howl_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Creature)
                    return;

                l_Creature->CallAssistance(20.0f);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rtk_howl_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_howl_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Reformed Maiden - 114783
class npc_rtk_reformed_maiden : public CreatureScript
{
    public:
        npc_rtk_reformed_maiden() : CreatureScript("npc_rtk_reformed_maiden") { }

        struct npc_rtk_reformed_maidenAI : public ScriptedAI
        {
            npc_rtk_reformed_maidenAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Flirt           = 228526,
                Heartbreaker    = 228528,
                Seduction       = 86545,
                KissOfDeath     = 151384
            };

            enum eDisplay
            {
                Succubus = 4162
            };

            int32 m_FlirtTimer;
            int32 m_SeductionTimer;
            int32 m_KissOfDeathTimer;
            bool m_Heartbreaker;
            bool m_SecondPhase;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                me->SetDisplayId(me->GetNativeDisplayId());
                m_FlirtTimer = 2 * IN_MILLISECONDS;
                m_Heartbreaker = false;
                m_SecondPhase = false;
            }

            void DamageTaken(Unit* /*p_Attaker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_SecondPhase && (me->GetHealthPct() < 50.0f))
                {
                    me->SetDisplayId(eDisplay::Succubus);
                    m_Heartbreaker = true;
                    m_SecondPhase = true;
                    m_SeductionTimer = 5 * IN_MILLISECONDS;
                    m_KissOfDeathTimer = 2 * IN_MILLISECONDS;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_FlirtTimer -= p_Diff;
                m_SeductionTimer -= p_Diff;
                m_KissOfDeathTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (!m_SecondPhase && m_FlirtTimer < 0)
                {
                    DoCastVictim(eSpells::Flirt);
                    m_FlirtTimer = 30 * IN_MILLISECONDS;
                    return;
                }

                if (m_Heartbreaker)
                {
                    DoCast(eSpells::Heartbreaker);
                    m_Heartbreaker = false;
                }

                if (m_SecondPhase && (m_SeductionTimer < 0))
                {
                    Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true);
                    if (!l_Victim)
                        return;

                    me->CastSpell(l_Victim, eSpells::Seduction, false);
                    m_SeductionTimer = 12 * IN_MILLISECONDS;
                }

                if (m_SecondPhase && (m_KissOfDeathTimer < 0))
                {
                    DoCastVictim(eSpells::KissOfDeath);
                    m_KissOfDeathTimer = 20 * IN_MILLISECONDS;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_reformed_maidenAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Heartbreaker - 228528
class spell_rtk_heartbreaker : public SpellScriptLoader
{
    public:
        spell_rtk_heartbreaker() : SpellScriptLoader("spell_rtk_heartbreaker") { }

        class spell_rtk_heartbreaker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_heartbreaker_SpellScript);

            enum eSpells
            {
                Flirt = 228526
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                std::list<WorldObject*> l_RealTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    if (!l_UnitTarget->HasAura(eSpells::Flirt))
                        continue;

                    l_RealTargets.push_back(l_UnitTarget);
                }

                p_Targets = l_RealTargets;
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::Flirt);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_heartbreaker_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_rtk_heartbreaker_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_heartbreaker_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Virtuous Lady - 114792
class npc_rtk_virtuous_lady : public CreatureScript
{
    public:
        npc_rtk_virtuous_lady() : CreatureScript("npc_rtk_virtuous_lady") { }

        struct npc_rtk_virtuous_ladyAI : public LegionCombatAI
        {
            npc_rtk_virtuous_ladyAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                CharmingPerfume     = 1,
                ShadowBolt          = 2,
                ShadowBoltVolley    = 3
            };

            enum eDisplay
            {
                Undead = 16744
            };

            bool m_SecondPhase;

            void EnterCombat(Unit* p_Ennemy) override
            {
                me->SetDisplayId(me->GetNativeDisplayId());
                m_SecondPhase = false;

                LegionCombatAI::EnterCombat(p_Ennemy);
            }

            void DamageTaken(Unit* /*p_Attaker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_SecondPhase && (me->GetHealthPct() < 50.0f))
                {
                    me->SetDisplayId(eDisplay::Undead);
                    m_SecondPhase = true;
                }
            }

            bool AdditionalChecks(uint32 p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::CharmingPerfume:
                        return !m_SecondPhase;
                    case eEvents::ShadowBolt:
                    case eEvents::ShadowBoltVolley:
                        return m_SecondPhase;
                    default:
                        break;
                }

                return true;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_virtuous_ladyAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Wholesome Hostess - 114796
class npc_rtk_wholesome_hostess : public CreatureScript
{
    public:
        npc_rtk_wholesome_hostess() : CreatureScript("npc_rtk_wholesome_hostess") { }

        struct npc_rtk_wholesome_hostessAI : public ScriptedAI
        {
            npc_rtk_wholesome_hostessAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                AlluringAura    = 228575,
                BansheeWail     = 228625
            };

            enum eDisplay
            {
                Banshee = 984
            };

            int32 m_BansheeWailTimer;
            bool m_SecondPhase;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                me->SetDisplayId(me->GetNativeDisplayId());
                DoCast(eSpells::AlluringAura);
                m_SecondPhase = false;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::BansheeWail:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        }, 100);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attaker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_SecondPhase && (me->GetHealthPct() < 50.0f))
                {
                    me->SetDisplayId(eDisplay::Banshee);
                    me->RemoveAurasDueToSpell(eSpells::AlluringAura);
                    m_SecondPhase = true;
                    m_BansheeWailTimer = 4 * IN_MILLISECONDS;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_BansheeWailTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_SecondPhase && (m_BansheeWailTimer < 0))
                {
                    me->SetFacingToObject(me->getVictim());
                    me->NearTeleportTo(*me);
                    DoCastVictim(eSpells::BansheeWail);
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    }, 50);
                    m_BansheeWailTimer = 10 * IN_MILLISECONDS;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_wholesome_hostessAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Allured - 228576
class spell_rtk_allured : public SpellScriptLoader
{
    public:
        spell_rtk_allured() : SpellScriptLoader("spell_rtk_allured") { }

        class spell_rtk_allured_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_allured_AuraScript);

            enum eSpells
            {
                Lured       = 228578,
                LuredCharm  = 228607
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Aura || !l_Caster || !l_Target)
                    return;

                if (l_Aura->GetStackAmount() >= 100)
                {
                    l_Caster->CastSpell(l_Target, eSpells::Lured, true);
                    l_Caster->CastSpell(l_Caster, eSpells::LuredCharm, true);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rtk_allured_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_allured_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Lured - 228607
class spell_rtk_lured : public SpellScriptLoader
{
    public:
        spell_rtk_lured() : SpellScriptLoader("spell_rtk_lured") { }

        class spell_rtk_lured_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_lured_AuraScript);

            enum eSpells
            {
                Lured = 228578
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->RemoveAura(eSpells::Lured, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_rtk_lured_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_lured_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Brand - 228608
class spell_rtk_burning_brand : public SpellScriptLoader
{
    public:
        spell_rtk_burning_brand() : SpellScriptLoader("spell_rtk_burning_brand") { }

        class spell_rtk_burning_brand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_burning_brand_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_burning_brand_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_burning_brand_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Brand - 228610
class spell_rtk_burning_brand_dot : public SpellScriptLoader
{
    public:
        spell_rtk_burning_brand_dot() : SpellScriptLoader("spell_rtk_burning_brand_dot") { }

        class spell_rtk_burning_brand_dot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_burning_brand_dot_AuraScript);

            enum eSpells
            {
                BurningBrandExplosion = 228612
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Target->CastSpell(l_Target, eSpells::BurningBrandExplosion, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_burning_brand_dot_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_burning_brand_dot_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Brand - 228612
class spell_rtk_burning_brand_explosion : public SpellScriptLoader
{
    public:
        spell_rtk_burning_brand_explosion() : SpellScriptLoader("spell_rtk_burning_brand_explosion") { }

        class spell_rtk_burning_brand_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_burning_brand_explosion_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (WorldObject* l_Target : p_Targets)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    if (l_UnitTarget == l_Caster)
                    {
                        p_Targets.remove(l_UnitTarget);
                        break;
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_burning_brand_explosion_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_burning_brand_explosion_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Spectral Stable Hand - 114803
class npc_rtk_spectral_stable_hand : public CreatureScript
{
    public:
        npc_rtk_spectral_stable_hand() : CreatureScript("npc_rtk_spectral_stable_hand") { }

        struct npc_rtk_spectral_stable_handAI : public LegionCombatAI
        {
            npc_rtk_spectral_stable_handAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eSpells
            {
                WhipRage = 233210,
                HealingTouch = 228606
            };

            enum eEvents
            {
                EventWhipRage        = 2,
                EventHealingTouch    = 3
            };

            enum eNPCs
            {
                SpectralCharger = 114804
            };

            Unit* GetTarget(std::list<Unit*>& p_TargetList, uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventHealingTouch:
                    {
                        for (Unit* l_Target : p_TargetList)
                        {
                            if (l_Target->GetEntry() != eNPCs::SpectralCharger)
                                continue;

                            if (l_Target->GetHealthPct() > 50.0f)
                                continue;

                            return l_Target;
                        }

                        return nullptr;
                    }
                    case eEvents::EventWhipRage:
                    {
                        for (Unit* l_Target : p_TargetList)
                        {
                            if (l_Target->GetEntry() != eNPCs::SpectralCharger)
                                continue;

                            if (l_Target->HasAura(eSpells::WhipRage))
                                continue;

                            return l_Target;
                        }

                        return nullptr;
                    }
                    default:
                        break;
                }

                return LegionCombatAI::GetTarget(p_TargetList, p_EventID);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_spectral_stable_handAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Caustic Venom - 228995
class spell_rtk_caustic_venom : public SpellScriptLoader
{
    public:
        spell_rtk_caustic_venom() : SpellScriptLoader("spell_rtk_caustic_venom") { }

        class spell_rtk_caustic_venom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_caustic_venom_AuraScript);

            enum eSpells
            {
                CausticPool = 228994
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CausticPool, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rtk_caustic_venom_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_caustic_venom_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Coldmist Stalker - 115115
class npc_rtk_coldmist_stalker : public CreatureScript
{
    public:
        npc_rtk_coldmist_stalker() : CreatureScript("npc_rtk_coldmist_stalker") { }

        struct npc_rtk_coldmist_stalkerAI : public LegionCombatAI
        {
            npc_rtk_coldmist_stalkerAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            void OnExitVehicle(Unit* p_Vehicle, Position& p_ExitPos) override
            {
                AddTimedDelayedOperation(1, [this, p_ExitPos]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, p_ExitPos);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_coldmist_stalkerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Coldmist Stalker Web - 8924
class at_rtk_coldmist_stalker_web : public AreaTriggerEntityScript
{
    public:
        at_rtk_coldmist_stalker_web() : AreaTriggerEntityScript("at_rtk_coldmist_stalker_web") { }

        enum eSpells
        {
            ColdmistStalkerWeb = 228980
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return true;

            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return true;

            Vehicle* l_Vehicle = l_Caster->GetVehicleKit();
            if (!l_Vehicle)
                return true;

            Unit* l_Passenger = l_Vehicle->GetPassenger(0);
            if (!l_Passenger)
                return true;

            l_Passenger->ExitVehicle();
            l_Caster->RemoveAura(eSpells::ColdmistStalkerWeb);

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_rtk_coldmist_stalker_web();
        }
};


/// Last Update 7.1.5 Build 23420
/// Entrance Door - 259690
class go_rtk_entrance_door : public GameObjectScript
{
    public:
        go_rtk_entrance_door() : GameObjectScript("go_rtk_entrance_door") {}

        enum eActions
        {
            TriggerEntrance
        };

        bool OnGossipHello(Player* p_Player, GameObject* p_GameObject) override
        {
            InstanceScript* l_Instance = p_GameObject->GetInstanceScript();
            if (!l_Instance)
                return false;

            uint64 l_KhadgarGUID = l_Instance->GetData64(eCreatures::ArchmageKhadgarEntrance);
            Creature* l_Creature = Creature::GetCreature(*p_GameObject, l_KhadgarGUID);
            if (!l_Creature || !l_Creature->IsAIEnabled)
                return false;

            l_Creature->GetAI()->DoAction(eActions::TriggerEntrance);

            return true;
        }
};

/// Last Update 7.1.5 Build 23420
/// Khadgar (Entrance) - 115496
class npc_rtk_entrance_khadgar : public CreatureScript
{
    public:
        npc_rtk_entrance_khadgar() : CreatureScript("npc_rtk_entrance_khadgar") { }

        struct npc_rtk_entrance_khadgarAI : public ScriptedAI
        {
            npc_rtk_entrance_khadgarAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eActions
            {
                TriggerEntrance
            };

            enum eTalks
            {
                EntranceTalk1,
                EntranceTalk2,
                EntranceEmote
            };

            enum eSpells
            {
                RavenForm = 229645
            };

            std::vector<G3D::Vector3> m_FlightPass =
            {
                {-11057.85f, -1973.069f, 102.069f},
                {-11063.27f, -1989.213f, 116.223f},
                {-11069.94f, -2006.136f, 115.250f}
            };

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::TriggerEntrance:
                    {
                        Talk(eTalks::EntranceEmote);

                        AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::EntranceTalk1);
                            me->SetWalk(true);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(0, -11057.85f, -1973.069f, 102.069f);
                        });

                        AddTimedDelayedOperation(15 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::EntranceTalk2);
                        });

                        AddTimedDelayedOperation(24 * IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(eSpells::RavenForm);
                        });

                        AddTimedDelayedOperation(25 * IN_MILLISECONDS, [this]() -> void
                        {
                            SetFlyMode(true);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(0, m_FlightPass.data(), m_FlightPass.size());
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
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_entrance_khadgarAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Peta Venkner - 114554
class npc_rtk_peta_venkner : public CreatureScript
{
    public:
        npc_rtk_peta_venkner() : CreatureScript("npc_rtk_peta_venkner") { }

        struct npc_rtk_peta_venknerAI : public ScriptedAI
        {
            npc_rtk_peta_venknerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eTalks
            {
                FeelingFunky                = 0,
                EgonaFortunateIndividual    = 0
            };

            enum eSpells
            {
                Slimed          = 228650,
                GhostEquipment  = 232686
            };

            bool m_EventStarted;
            bool m_EventEnded;

            Position m_DestPeta =  { -10949.27f, -1916.82f, 78.87f };
            Position m_DestEgona = { -10950.28f, -1914.59f, 78.87f };

            void Reset() override
            {
                m_EventStarted = false;
                m_EventEnded = false;
                me->AddAura(eSpells::Slimed, me);
                me->AddAura(eSpells::GhostEquipment, me);
                ScheduleOperations();
            }

            void ScheduleOperations()
            {
                me->AddAura(eSpells::GhostEquipment, me);
                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                {
                    if (me->FindNearestPlayer(10.0f))
                        m_EventStarted = true;

                    Creature* l_Egona = me->FindNearestCreature(eCreatures::EgonaSpangly, 5.0f);
                    if (!l_Egona || !l_Egona->IsAIEnabled)
                        return;

                    l_Egona->GetAI()->DoAction(0);
                });
            }

            void LastOperationCalled() override
            {
                if (!m_EventStarted)
                {
                    ScheduleOperations();
                    return;
                }

                if (!m_EventEnded)
                {
                    AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::FeelingFunky);
                    });

                    AddTimedDelayedOperation(7 * IN_MILLISECONDS, [this]() -> void
                    {
                        Creature* l_Egona = me->FindNearestCreature(eCreatures::EgonaSpangly, 5.0f);
                        if (!l_Egona || !l_Egona->IsAIEnabled)
                            return;

                        static_cast<CreatureAI*>(l_Egona->GetAI())->Talk(eTalks::EgonaFortunateIndividual);
                    });

                    AddTimedDelayedOperation(12 * IN_MILLISECONDS, [this]() -> void
                    {
                        Creature* l_Egona = me->FindNearestCreature(eCreatures::EgonaSpangly, 5.0f);
                        if (!l_Egona || !l_Egona->IsAIEnabled)
                            return;

                        l_Egona->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->Clear();

                        l_Egona->GetMotionMaster()->MovePoint(10, m_DestEgona);
                        me->GetMotionMaster()->MovePoint(10, m_DestPeta);
                        m_EventEnded = true;
                    });

                    return;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == 10)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_peta_venknerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Egona Spangly - 114552
class npc_rtk_egona_spangly : public CreatureScript
{
    public:
        npc_rtk_egona_spangly() : CreatureScript("npc_rtk_egona_spangly") { }

        struct npc_rtk_egona_spanglyAI : public ScriptedAI
        {
            npc_rtk_egona_spanglyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                GhostEquipment  = 232686,
                GhostTrap       = 227908,
                GhostBeam       = 232695
            };

            bool m_EventStarted;

            void ScheduleOperations()
            {
                me->AddAura(eSpells::GhostEquipment, me);
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Target = me->FindNearestCreature(eCreatures::PhantomGuest, 2.0f);
                    if (!l_Target)
                        return;

                    me->CastSpell(l_Target, eSpells::GhostTrap, true);
                    me->CastSpell(l_Target, eSpells::GhostBeam, true);
                });

                AddTimedDelayedOperation(120 * IN_MILLISECONDS, []() -> void {});
            }

            void DoAction(int32 const p_Action) override
            {
                m_EventStarted = true;
            }

            void Reset() override
            {
                m_EventStarted = false;
                ScheduleOperations();
            }

            void LastOperationCalled() override
            {
                if (!m_EventStarted)
                    ScheduleOperations();
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == 10)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_egona_spanglyAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Gather Soul Essence - 229007
class spell_rtk_gather_soul_essence : public SpellScriptLoader
{
    public:
        spell_rtk_gather_soul_essence() : SpellScriptLoader("spell_rtk_gather_soul_essence") { }

        class spell_rtk_gather_soul_essence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_gather_soul_essence_AuraScript);

            enum eSpells
            {
                MedivhsEcho     = 229074,
                MedivhsPresence = 229077
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_BaseAura = p_AuraEffect->GetBase();
                if (!l_BaseAura || l_BaseAura->GetDuration())
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return;

                InstanceScript* l_Instance = l_Creature->GetInstanceScript();
                if (l_Instance)
                    l_Instance->SetData(eData::CancelCristalTimer, 0);

                l_Creature->DespawnOrUnsummon();

                if (!l_Caster->HasAura(eSpells::MedivhsEcho))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::MedivhsEcho);
                    Aura* l_Aura = l_Caster->GetAura(eSpells::MedivhsEcho);
                    if (!l_Aura)
                        return;

                    l_Aura->SetDuration(7 * IN_MILLISECONDS * MINUTE);
                    return;
                }

                Aura* l_Aura = l_Caster->GetAura(eSpells::MedivhsEcho);
                if (!l_Aura)
                    return;

                if (l_Aura->GetStackAmount() < 4)
                {
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() + 1);
                    l_Aura->SetDuration(l_Aura->GetDuration() + 5 * IN_MILLISECONDS * MINUTE);
                    return;
                }

                l_Caster->RemoveAurasDueToSpell(eSpells::MedivhsEcho);
                l_Caster->CastSpell(l_Caster, eSpells::MedivhsPresence, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_gather_soul_essence_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_gather_soul_essence_AuraScript();
        }
};

/// Khadgar (Library) - 115501
class npc_rtk_library_khadgar : public CreatureScript
{
    public:
        npc_rtk_library_khadgar() : CreatureScript("npc_rtk_library_khadgar") { }

        struct npc_rtk_library_khadgarAI : public ScriptedAI
        {
            npc_rtk_library_khadgarAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->CastSpell(me, eSpells::Vision, true);
                me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);
            }

            enum eSpells
            {
                Vision = 229485
            };

            void Reset() override
            {
                AddTimedDelayedOperation(14 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, 0);
                });

                DelayTalk(15, 0);

                AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]() -> void
                {
                    me->DespawnOrUnsummon();
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_library_khadgarAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Anduin Lothar - 115489
class npc_rtk_anduin_lothar : public CreatureScript
{
    public:
        npc_rtk_anduin_lothar() : CreatureScript("npc_rtk_anduin_lothar") { }

        struct npc_rtk_anduin_lotharAI : public ScriptedAI
        {
            npc_rtk_anduin_lotharAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_MaidenEvent = me->m_positionX < 0.0f;

                if (m_MaidenEvent)
                {
                    me->SetWalk(true);
                    me->SetVisible(false);
                }
                else
                    me->CastSpell(me, eSpells::Vision, true);

                me->SetSheath(SheathState::SHEATH_STATE_UNARMED);
            }

            enum eTalks
            {
                ConvSceneTalk1,
                ConvSceneTalk2
            };

            enum eSpells
            {
                Vision = 229485
            };

            bool m_MaidenEvent;

            std::vector<G3D::Vector3> m_WalkPathLibrary =
            {
                { 4141.20f, -2070.01f, 727.82f },
                { 4132.59f, -2073.46f, 727.82f }
            };

            std::vector<G3D::Vector3> m_WalkingPathMaiden =
            {
                { -10910.37f, -2056.85f, 92.18f },
                { -10909.29f, -2062.56f, 92.18f },
                { -10890.35f, -2076.57f, 92.17f },
                { -10859.98f, -2101.67f, 91.75f },
                { -10855.65f, -2106.00f, 92.17f },
                { -10854.29f, -2107.63f, 92.17f },
                { -10851.47f, -2107.04f, 92.17f }
            };

            void Reset() override
            {
                if (m_MaidenEvent)
                {
                    me->AddAura(eSpells::Vision, me);
                    me->SetVisible(true);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePath(10, m_WalkingPathMaiden.data(), m_WalkingPathMaiden.size(), true);

                    AddTimedDelayedOperation(11000, [this]() -> void
                    {
                        Talk(eTalks::ConvSceneTalk1);
                    });

                    AddTimedDelayedOperation(29500, [this]() -> void
                    {
                        Talk(eTalks::ConvSceneTalk2);
                    });
                }
                else
                {
                    DelayTalk(4, 2);
                    AddTimedDelayedOperation(23 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MoveSmoothPath(0, m_WalkPathLibrary.data(), m_WalkPathLibrary.size(), true);
                    });

                    AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->DespawnOrUnsummon();
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == 10)
                {
                    Creature* l_Medivh = me->FindNearestCreature(eCreatures::MedivhMaiden, 10.0f);
                    if (!l_Medivh)
                        return;

                    me->SetFacingToObject(l_Medivh);
                    AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_anduin_lotharAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Legacy Of The Mountain King - 228807
class spell_rtk_legacy_of_the_mountain_king : public SpellScriptLoader
{
    public:
        spell_rtk_legacy_of_the_mountain_king() : SpellScriptLoader("spell_rtk_legacy_of_the_mountain_king") { }

        class spell_rtk_legacy_of_the_mountain_king_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_legacy_of_the_mountain_king_AuraScript);

            enum eSpells
            {
                WillOfTheMountainKing = 228825
            };

            bool m_IsAbove25 = false;

            void HandlePeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->GetHealthPct() > 25.0f)
                {
                    m_IsAbove25 = true;
                    return;
                }

                if (!m_IsAbove25)
                    return;

                m_IsAbove25 = false;

                l_Caster->CastSpell(l_Caster, eSpells::WillOfTheMountainKing, static_cast<TriggerCastFlags>(~TriggerCastFlags::TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD));
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rtk_legacy_of_the_mountain_king_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_legacy_of_the_mountain_king_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Torment of the Worgen - 228849
class spell_rtk_torment_of_the_worgen : public SpellScriptLoader
{
    public:
        spell_rtk_torment_of_the_worgen() : SpellScriptLoader("spell_rtk_torment_of_the_worgen") { }

        class spell_rtk_torment_of_the_worgen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_torment_of_the_worgen_AuraScript);

            enum eSpells
            {
                FrenzyOfTheWorgen = 228850
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return false;

                uint64 l_TargetHealth = l_Target->GetHealth();

                l_TargetHealth = l_TargetHealth + l_DamageInfo->GetAmount();

                if (l_TargetHealth > l_Target->CountPctFromMaxHealth(30.0f))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FrenzyOfTheWorgen, static_cast<TriggerCastFlags>(~TriggerCastFlags::TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD));
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rtk_torment_of_the_worgen_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_rtk_torment_of_the_worgen_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_torment_of_the_worgen_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called Redemption of the Fallen - 228859
class spell_rtk_redemption_of_the_fallen : public SpellScriptLoader
{
    public:
        spell_rtk_redemption_of_the_fallen() : SpellScriptLoader("spell_rtk_redemption_of_the_fallen") { }

        class spell_rtk_redemption_of_the_fallen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_redemption_of_the_fallen_AuraScript);

            enum eSpells
            {
                RedemptionOfTheFallen = 228858
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return false;

                uint64 l_TargetHealth = l_Target->GetHealth();

                l_TargetHealth = l_TargetHealth - l_DamageInfo->GetAmount();

                if (l_TargetHealth > l_Target->CountPctFromMaxHealth(25.0f))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RedemptionOfTheFallen, static_cast<TriggerCastFlags>(~TriggerCastFlags::TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD));
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rtk_redemption_of_the_fallen_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_rtk_redemption_of_the_fallen_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_redemption_of_the_fallen_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Wrath of the Titans - 228917
class spell_rtk_wrath_of_the_titans : public SpellScriptLoader
{
    public:
        spell_rtk_wrath_of_the_titans() : SpellScriptLoader("spell_rtk_wrath_of_the_titans") { }

        class spell_rtk_wrath_of_the_titans_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_wrath_of_the_titans_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                int32 l_Damage = GetHitDamage();

                float l_Distance = l_Caster->GetExactDist2d(l_Target);

                SetHitDamage(l_Damage * (l_Distance / 40.0f));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rtk_wrath_of_the_titans_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_wrath_of_the_titans_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Nullification - 230083
class spell_rtk_nullification : public SpellScriptLoader
{
    public:
        spell_rtk_nullification() : SpellScriptLoader("spell_rtk_nullification") { }

        class spell_rtk_nullification_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_nullification_SpellScript);

            enum eSpells
            {
                NullificationMissile = 230098
            };

            void HandleAfterCast()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                Position l_TargetPos;
                l_Target->GetRandomNearPosition(l_TargetPos, 25.0f);
                l_Caster->CastSpell(l_TargetPos, eSpells::NullificationMissile, true, nullptr, nullptr, l_Target->GetGUID());
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rtk_nullification_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_nullification_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Mana-Gorged Wyrm - 114364
class npc_rtk_mana_gorged_wyrm : public CreatureScript
{
    public:
        npc_rtk_mana_gorged_wyrm() : CreatureScript("npc_rtk_mana_gorged_wyrm") { }

        struct npc_rtk_mana_gorged_wyrmAI : public LegionCombatAI
        {
            npc_rtk_mana_gorged_wyrmAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                SiphonEnergy = 1
            };

            bool AdditionalChecks(uint32 p_EventID)
            {
                switch (p_EventID)
                {
                    case eEvents::SiphonEnergy:
                    {
                        if (me->GetPowerPct(POWER_MANA) > 99.0f)
                            return false;
                    }
                    default:
                        return LegionCombatAI::AdditionalChecks(p_EventID);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_mana_gorged_wyrmAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Siphon Energy - 228362
class spell_rtk_siphon_energy : public SpellScriptLoader
{
    public:
        spell_rtk_siphon_energy() : SpellScriptLoader("spell_rtk_siphon_energy") { }

        class spell_rtk_siphon_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_siphon_energy_AuraScript);

            enum eSpells
            {
                AblativeShield = 227548
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (l_Target->HasAura(eSpells::AblativeShield))
                {
                    if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::AblativeShield, EFFECT_0))
                        l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() - p_AuraEffect->GetAmount());
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rtk_siphon_energy_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_MANA_LEECH);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_siphon_energy_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Bat - 115484
class npc_rtk_fel_bat : public CreatureScript
{
    public:
        npc_rtk_fel_bat() : CreatureScript("npc_rtk_fel_bat") { }

        struct npc_rtk_fel_batAI : public LegionCombatAI
        {
            npc_rtk_fel_batAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eSpells
            {
                FelMortar = 229595
            };

            enum eEvents
            {
                EventFelMortars = 1
            };

            bool m_HasBeenTaunted;

            void Reset() override
            {
                LegionCombatAI::Reset();
                SetFlyMode(true);
                m_HasBeenTaunted = false;
            }

            bool CanChase() override
            {
                return m_HasBeenTaunted;
            }

            void OnTaunt(Unit* /*p_Taunter*/) override
            {
                m_HasBeenTaunted = true;
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventFelMortars:
                    {
                        if (m_HasBeenTaunted)
                            return true;

                        Player* l_Player = me->FindNearestPlayer(25.0f);
                        if (!l_Player)
                            return false;

                        float l_Angle = me->GetAngle(l_Player);
                        Position l_TargetPos;
                        float l_Dist = me->GetExactDist2d(l_Player) - 3.0f;
                        for (uint32 l_I = 0; l_I < 3; ++l_I)
                        {
                            me->SimplePosXYRelocationByAngle(l_TargetPos, l_Dist, l_Angle, true);
                            l_TargetPos.m_positionZ = l_Player->GetPositionZ();
                            AddTimedDelayedOperation(200 * l_I, [this, l_TargetPos]() -> void
                            {
                                me->CastSpell(l_TargetPos, eSpells::FelMortar, true);
                            });

                            l_Dist += 3.0f;
                        }

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_fel_batAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Bomb - 9023
class at_rtk_fel_bomb : public AreaTriggerEntityScript
{
    public:
        at_rtk_fel_bomb() : AreaTriggerEntityScript("at_rtk_fel_bomb") { }

        enum eSpells
        {
            FelBombDot          = 229677,
            FelBombExplosion    = 229678
        };

        int32 p_Timer = 1000;
        uint32 m_TickSoaked = 0;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            p_Timer -= p_Time;
            if (p_Timer > 0)
                return;

            p_Timer += 1000;

            bool l_Soaked = false;
            for (uint64 l_TargetGUID : *p_AreaTrigger->GetAffectedPlayers())
            {
                Unit* l_Target = Unit::GetUnit(*p_AreaTrigger, l_TargetGUID);
                if (!l_Target || !l_Target->IsPlayer())
                    continue;

                p_AreaTrigger->CastSpellByOwnerFromATLocation(l_Target, eSpells::FelBombDot);
                l_Soaked = true;
            }

            if (l_Soaked)
                m_TickSoaked++;

            if (m_TickSoaked >= 10)
                p_AreaTrigger->Despawn();
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_TickSoaked >= 10)
                return;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->CastSpell(*p_AreaTrigger, eSpells::FelBombExplosion, true);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_rtk_fel_bomb();
        }
};

/// Last Update 7.1.5 Build 23420
/// Gleeful Immolator - 115533
class npc_rtk_gleeful_immolator : public CreatureScript
{
    public:
        npc_rtk_gleeful_immolator() : CreatureScript("npc_rtk_gleeful_immolator") { }

        struct npc_rtk_gleeful_immolatorAI : public LegionCombatAI
        {
            npc_rtk_gleeful_immolatorAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                Unit* l_Summoner = me->FindNearestCreature(eCreatures::InfusedPyromancer, 20.0f);
                if (l_Summoner)
                    me->SetFacingToObject(l_Summoner);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eEvents
            {
                EventGleefulImmoliationVisual   = 2
            };

            enum eSpells
            {
                GleefulImmoliationVisual    = 229943
            };

            bool CanChase() override
            {
                return false;
            }

            bool CanAutoAttack() override
            {
                return false;
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventGleefulImmoliationVisual:
                    {
                        float l_Dist = frand(2.0f, 15.0f);
                        Position l_Pos;
                        me->SimplePosXYRelocationByAngle(l_Pos, l_Dist, 0.0f);
                        me->CastSpell(l_Pos, eSpells::GleefulImmoliationVisual, true);

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_gleeful_immolatorAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Wrathguard Flamebringer - 115757
class npc_rtk_wrathguard_flamebringer : public CreatureScript
{
    public:
        npc_rtk_wrathguard_flamebringer() : CreatureScript("npc_rtk_wrathguard_flamebringer") { }

        struct npc_rtk_wrathguard_flamebringerAI : public LegionCombatAI
        {
            npc_rtk_wrathguard_flamebringerAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                EventSlide          = 3,
                FirstBurningTile    = 4,
                OtherBurningTiles   = 5
            };

            enum eSpells
            {
                BurningTile = 229989
            };

            std::vector<Position> m_BrownBurningTilePos =
            {
                { -4599.5f, -2314.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4599.5f, -2304.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4599.5f, -2324.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4589.5f, -2314.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4589.5f, -2324.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4609.5f, -2314.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4609.5f, -2324.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4599.5f, -2334.5f, 2868.07f, float(M_PI) / 4.0f }
            };

            std::vector<Position> m_WhiteBurningTilePos =
            {
                { -4614.5f, -2309.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4614.5f, -2319.5f, 2868.07f, float(M_PI) / 4.0f },

                { -4604.5f, -2309.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4604.5f, -2319.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4604.5f, -2329.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4604.5f, -2339.5f, 2868.07f, float(M_PI) / 4.0f },

                { -4594.5f, -2309.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4594.5f, -2319.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4594.5f, -2329.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4594.5f, -2339.5f, 2868.07f, float(M_PI) / 4.0f },

                { -4584.5f, -2309.5f, 2868.07f, float(M_PI) / 4.0f },
                { -4584.5f, -2319.5f, 2868.07f, float(M_PI) / 4.0f }
            };

            bool CanAutoAttack() override
            {
                return false;
            }

            void EnterCombat(Unit* p_Who) override
            {
                LegionCombatAI::EnterCombat(p_Who);

                Creature* l_ShadeOfMedivh = me->FindNearestCreature(eCreatures::ShadeOfMedivh, 250.0f);
                if (!l_ShadeOfMedivh || !l_ShadeOfMedivh->IsAIEnabled)
                    return;

                l_ShadeOfMedivh->GetAI()->DoAction(ACTION_2);
            }

            bool AdditionalChecks(uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::FirstBurningTile:
                        return !me->isInCombat();
                    default:
                        return LegionCombatAI::AdditionalChecks(p_EventID);
                }

                return false;
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::FirstBurningTile:
                    {
                        for (Position const& l_Pos : m_BrownBurningTilePos)
                            me->CastSpell(l_Pos, eSpells::BurningTile, true);

                        return true;
                    }
                    case eEvents::OtherBurningTiles:
                    {
                        for (Position const& l_Pos : m_WhiteBurningTilePos)
                            me->CastSpell(l_Pos, eSpells::BurningTile, true);

                        AddTimedDelayedOperation(1500, [this]() -> void
                        {
                            for (Position const& l_Pos : m_BrownBurningTilePos)
                                me->CastSpell(l_Pos, eSpells::BurningTile, true);
                        });

                        return true;
                    }
                    case eEvents::EventSlide:
                    {
                        Position l_Dest = *p_Target;
                        p_Target->GetNearPoint(nullptr, l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ, me->GetObjectSize(), 0.0f, p_Target->GetOrientation() + M_PI);
                        me->NearTeleportTo(l_Dest);
                        AttackStart(p_Target, true);

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                LegionCombatAI::UpdateAI(p_Diff);
                Unit* l_Victim = me->getVictim();
                if (!l_Victim || me->IsWithinMeleeRange(l_Victim))
                    return;

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(l_Victim);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_wrathguard_flamebringerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Stampede - 229698
class spell_rtk_stampede : public SpellScriptLoader
{
    public:
        spell_rtk_stampede() : SpellScriptLoader("spell_rtk_stampede") { }

        class spell_rtk_stampede_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_stampede_SpellScript);

            enum eSpells
            {
                Stampede = 229697
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_FirstRowSize = urand(1, 15);
                uint32 l_MiddleRowSize = 2;
                for (uint32 l_I = 0; l_I < (18 - l_MiddleRowSize); ++l_I)
                {
                    float l_Offset;
                    Position l_Pos = *l_Caster;
                    if (l_I < l_FirstRowSize)
                        l_Offset = (l_I * 2.0f) - 18.0f;
                    else
                        l_Offset = ((l_I + l_MiddleRowSize) * 2.0f) - 18.0f;

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Offset, l_Pos.NormalizeOrientation((M_PI / 2.0f)));
                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 20.0f, l_Pos.NormalizeOrientation(M_PI));
                    l_Pos.SetOrientation(l_Caster->GetOrientation());
                    l_Caster->CastSpell(l_Pos, eSpells::Stampede, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rtk_stampede_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_stampede_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_return_to_karazhan()
{
    /// Opera Hall Balcony
    new spell_rtk_charitable_donation();

    /// Opera Backstage
    new spell_rtk_purify();

    /// Opera Hall
    new spell_rtk_flashlight();

    /// Upper Banquet Hall
    new npc_rtk_phantom_guest();

    /// Lower Banquet Hall
    new spell_rtk_volatile_charge();
    new spell_rtk_volatile_charge_explosion();

    /// Scullery
    new spell_rtk_howl();

    /// Guest Chambers
    new npc_rtk_reformed_maiden();
    new spell_rtk_heartbreaker();
    new npc_rtk_virtuous_lady();
    new npc_rtk_wholesome_hostess();
    new spell_rtk_allured();
    new spell_rtk_lured();

    /// Stables
    new spell_rtk_burning_brand();
    new spell_rtk_burning_brand_dot();
    new spell_rtk_burning_brand_explosion();
    new npc_rtk_spectral_stable_hand();

    /// Servants Quarter
    new spell_rtk_caustic_venom();
    new npc_rtk_coldmist_stalker();
    new at_rtk_coldmist_stalker_web();

    /// Events
    new go_rtk_entrance_door();
    new npc_rtk_entrance_khadgar();
    new npc_rtk_peta_venkner();
    new npc_rtk_egona_spangly();
    new spell_rtk_gather_soul_essence();
    new npc_rtk_library_khadgar();
    new npc_rtk_anduin_lothar();

    /// Books
    new spell_rtk_legacy_of_the_mountain_king();
    new spell_rtk_torment_of_the_worgen();
    new spell_rtk_redemption_of_the_fallen();
    new spell_rtk_wrath_of_the_titans();

    /// Menagerie
    new npc_rtk_mana_gorged_wyrm();
    new spell_rtk_nullification();
    new spell_rtk_siphon_energy();

    /// Library
    new npc_rtk_fel_bat();
    new at_rtk_fel_bomb();
    new npc_rtk_gleeful_immolator();
    new npc_rtk_wrathguard_flamebringer();

    /// Giant Library
    new spell_rtk_stampede();
}
#endif
