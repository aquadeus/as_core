////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "eye_of_azshara.hpp"

enum Says
{
    SAY_SUBMERGE,
    SAY_TOXIC_WOUND
};

enum Spells
{
    SPELL_TOXIC_WOUND           = 191855,
    SPELL_POISON_SPIT           = 192050,
    SPELL_POISON_SPIT_TARG      = 191839,
    SPELL_POISON_SPIT_S1        = 191841,
    SPELL_POISON_SPIT_S2        = 191843,
    SPELL_POISON_SPIT_S3        = 191845,
    SPELL_SUBMERGE              = 191873,
    SPELL_SUM_HYDRA_SPAWN       = 192010,
    SPELL_RAMPAGE               = 191848,
    SPELL_RAMPAGE_TARG          = 191850,
    SPELL_HYDRA_HEAD            = 202680,

    SPELL_BLAZING_NOVA          = 192003,
    SPELL_ARCANE_BLAST          = 192005,

    SPELL_BONUS_ROLL            = 226621
};

enum eEvents
{
    EVENT_TOXIC_WOUND   = 1,
    EVENT_POISON_SPIT   = 2,
    EVENT_SUBMERGE      = 3,
    EVENT_SUM_HYDRA     = 4,
    EVENT_RAMPAGE       = 5
};

uint32 g_PoisonSpells[3] =
{
    SPELL_POISON_SPIT_S1,
    SPELL_POISON_SPIT_S2,
    SPELL_POISON_SPIT_S3
};

Position const g_TeleportPos[6] =
{
    { -3256.36f, 4370.39f, 0.37f, 0.0f },
    { -3294.20f, 4460.52f, -0.6f, 0.0f },
    { -3304.17f, 4405.53f, 0.08f, 0.0f },
    { -3193.61f, 4435.89f, -0.7f, 0.0f },
    { -3199.40f, 4384.95f, 0.16f, 0.0f },
    { -3246.71f, 4479.65f, 0.26f, 0.0f }
};

uint32 g_HydraEntries[2] =
{
    NPC_BLAZING_HYDRA_SPAWN,
    NPC_ARCANE_HYDRA_SPAWN
};

/// Serpentrix - 91808
class boss_serpentrix : public CreatureScript
{
    public:
        boss_serpentrix() : CreatureScript("boss_serpentrix") { }

        struct boss_serpentrixAI : public BossAI
        {
            boss_serpentrixAI(Creature* p_Creature) : BossAI(p_Creature, DATA_SERPENTRIX)
            {
                SetCombatMovement(false);
            }

            int8 m_HealthPct;
            uint8 m_SumGidrCount;
            uint16 m_CheckVictimTimer;

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(191855);

                _Reset();

                m_HealthPct = 66;
                m_CheckVictimTimer = 2000;

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                //Talk(SAY_AGGRO);

                _EnterCombat();

                events.ScheduleEvent(EVENT_TOXIC_WOUND, 6000);
                events.ScheduleEvent(EVENT_POISON_SPIT, 11000);
            }

            void EnterEvadeMode() override
            {
                me->NearTeleportTo(me->GetHomePosition());

                BossAI::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                //Talk(SAY_DEATH);

                _JustDied();

                me->PlayOneShotAnimKitId(10626); ///< Death anim

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_POISON_SPIT_TARG:
                    case SPELL_RAMPAGE_TARG:
                    {
                        DoCast(p_Target, g_PoisonSpells[urand(0, 2)], true);
                        break;
                    }
                    case SPELL_SUBMERGE:
                    {
                        events.ScheduleEvent(EVENT_SUM_HYDRA, 500);
                        break;
                    }
                    case SPELL_SUM_HYDRA_SPAWN:
                    {
                        me->SummonCreature(g_HydraEntries[m_SumGidrCount], p_Target->GetPosition());

                        if (IsHeroicOrMythic())
                            m_SumGidrCount++;

                        break;
                    }
                    case SPELL_TOXIC_WOUND:
                    {
                        Talk(SAY_TOXIC_WOUND, p_Target->GetGUID());
                        break;
                    }
                    case SPELL_POISON_SPIT:
                    {
                        DoCast(p_Target, SPELL_POISON_SPIT_TARG, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(m_HealthPct, p_Damage) && !events.HasEvent(EVENT_SUBMERGE))
                {
                    m_HealthPct = std::max(0, m_HealthPct - 33);
                    me->CastStop();
                    events.ScheduleEvent(EVENT_SUBMERGE, 100);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_CheckVictimTimer <= p_Diff)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_NEAREST, 0, 80.0f, true))
                    {
                        if (!me->IsWithinMeleeRange(l_Target))
                        {
                            if (!me->HasAura(SPELL_RAMPAGE))
                                events.ScheduleEvent(EVENT_RAMPAGE, 500);
                        }
                        else
                            me->InterruptNonMeleeSpells(false, SPELL_RAMPAGE);
                    }
                    m_CheckVictimTimer = 2000;
                }
                else
                    m_CheckVictimTimer -= p_Diff;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_TOXIC_WOUND:
                    {
                        DoCast(SPELL_TOXIC_WOUND);
                        events.ScheduleEvent(EVENT_TOXIC_WOUND, 25000);
                        break;
                    }
                    case EVENT_POISON_SPIT:
                    {
                        DoCast(SPELL_POISON_SPIT);
                        events.ScheduleEvent(EVENT_POISON_SPIT, 9000);
                        break;
                    }
                    case EVENT_SUBMERGE:
                    {
                        Talk(SAY_SUBMERGE);
                        DoCast(SPELL_SUBMERGE);
                        break;
                    }
                    case EVENT_SUM_HYDRA:
                    {
                        m_SumGidrCount = 0;
                        DoCast(SPELL_SUM_HYDRA_SPAWN);
                        break;
                    }
                    case EVENT_RAMPAGE:
                    {
                        DoCast(SPELL_RAMPAGE);
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
            return new boss_serpentrixAI(p_Creature);
        }
};

/// Blazing Hydra Spawn <Spawn of Serpentrix> - 97259
/// Arcane Hydra Spawn <Spawn of Serpentrix> - 97260
class npc_serpentrix_hydras : public CreatureScript
{
    public:
        npc_serpentrix_hydras() : CreatureScript("npc_serpentrix_hydras") { }

        struct npc_serpentrix_hydrasAI : public ScriptedAI
        {
            npc_serpentrix_hydrasAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->isInCombat())
                    me->DespawnOrUnsummon();

                p_Summoner->CastSpell(me, SPELL_HYDRA_HEAD);
                DoZoneInCombat(me, 100.0f);
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
                        if (me->GetEntry() == NPC_BLAZING_HYDRA_SPAWN)
                            DoCast(SPELL_BLAZING_NOVA);
                        else
                        {
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(l_Target, SPELL_ARCANE_BLAST);
                        }

                        events.ScheduleEvent(EVENT_1, 2500);
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
            return new npc_serpentrix_hydrasAI(p_Creature);
        }
};

/// Submerge - 191873
class spell_serpentrix_submerge_teleport : public SpellScriptLoader
{
    public:
        spell_serpentrix_submerge_teleport() : SpellScriptLoader("spell_serpentrix_submerge_teleport") { }

        class spell_serpentrix_submerge_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_serpentrix_submerge_teleport_SpellScript);

            void HandleScriptEffect(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint8 l_Rand = urand(0, 5);
                if (l_Caster->GetDistance(g_TeleportPos[l_Rand]) < 10.0f)
                    l_Rand -= l_Rand;

                l_Caster->NearTeleportTo(g_TeleportPos[l_Rand]);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_serpentrix_submerge_teleport_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_serpentrix_submerge_teleport_SpellScript();
        }
};

/// Roiling Storm - 196296
class spell_serpentrix_roiling_storm : public SpellScriptLoader
{
    public:
        spell_serpentrix_roiling_storm() : SpellScriptLoader("spell_serpentrix_roiling_storm") { }

        enum eSpell
        {
            SpellRoilingStorm = 196298
        };

        class spell_serpentrix_roiling_storm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_serpentrix_roiling_storm_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    float l_Distance    = 8.0f + float(int32(p_AurEff->GetTickNumber() / 2) * 8.0f);
                    float l_Angle       = l_Caster->m_orientation;

                    /// Left
                    if (p_AurEff->GetTickNumber() % 2)
                        l_Angle -= M_PI / 6.0f;
                    /// Right
                    else
                        l_Angle += M_PI / 6.0f;

                    float l_X   = l_Caster->m_positionX + l_Distance * std::cos(l_Angle);
                    float l_Y   = l_Caster->m_positionY + l_Distance * std::sin(l_Angle);
                    float l_Z   = l_Caster->GetMap()->GetHeight(l_X, l_Y, l_Caster->m_positionZ);

                    Position l_Pos =
                    {
                        l_X,
                        l_Y,
                        l_Z,
                        l_Angle
                    };

                    l_Caster->CastSpell(l_Pos, eSpell::SpellRoilingStorm, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_serpentrix_roiling_storm_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_serpentrix_roiling_storm_AuraScript();
        }
};

/// Spray Sand - 196127
class spell_serpentrix_spray_sand : public SpellScriptLoader
{
    public:
        spell_serpentrix_spray_sand() : SpellScriptLoader("spell_serpentrix_spray_sand") { }

        class spell_serpentrix_spray_sand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_serpentrix_spray_sand_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_serpentrix_spray_sand_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectApplyFn(spell_serpentrix_spray_sand_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_serpentrix_spray_sand_AuraScript();
        }
};

/// Chaotic Tempest - 196290
class spell_serpentrix_chaotic_tempest : public SpellScriptLoader
{
    public:
        spell_serpentrix_chaotic_tempest() : SpellScriptLoader("spell_serpentrix_chaotic_tempest") { }

        enum eSpell
        {
            SpellChaoticTempestMissile = 196292
        };

        class spell_serpentrix_chaotic_tempest_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_serpentrix_chaotic_tempest_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    for (uint8 l_I = 0; l_I < 6; ++l_I)
                    {
                        Position l_Pos = l_Caster->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(3.0f, 30.0f), frand(0.0f, 2.0f * M_PI), true);

                        l_Caster->CastSpell(l_Pos, eSpell::SpellChaoticTempestMissile, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_serpentrix_chaotic_tempest_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_serpentrix_chaotic_tempest_AuraScript();
        }
};

/// Tail Whip - 196287
class spell_serpentrix_tail_whip : public SpellScriptLoader
{
    public:

        spell_serpentrix_tail_whip() : SpellScriptLoader("spell_serpentrix_tail_whip") { }

        class spell_serpentrix_tail_whip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_serpentrix_tail_whip_SpellScript);

            SpellCastResult CheckTarget()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                std::list<Player*> l_PlayerList;
                l_Caster->GetPlayerListInGrid(l_PlayerList, 10.0f);

                if (!l_PlayerList.empty())
                {
                    l_PlayerList.remove_if([l_Caster](Player* p_Player) -> bool
                    {
                        if (!l_Caster->isInBack(p_Player, (50.0f * M_PI / 180.0f)))
                            return true;

                        return false;
                    });
                }

                if (l_PlayerList.empty())
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_serpentrix_tail_whip_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_serpentrix_tail_whip_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_serpentrix()
{
    /// Boss
    new boss_serpentrix();

    /// Creature
    new npc_serpentrix_hydras();

    /// Spells
    new spell_serpentrix_submerge_teleport();
    new spell_serpentrix_roiling_storm();
    new spell_serpentrix_spray_sand();
    new spell_serpentrix_chaotic_tempest();
    new spell_serpentrix_tail_whip();
}
#endif
