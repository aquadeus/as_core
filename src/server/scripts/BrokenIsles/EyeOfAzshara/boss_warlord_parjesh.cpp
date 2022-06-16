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
    SAY_INTRO       = 0,
    SAY_INTRO_1     = 1,
    SAY_AGGRO       = 2,
    SAY_CALL        = 3,
    SAY_SPEAR_NPC   = 4,
    SAY_TARGET      = 5,
    SAY_SPEAR_PLR   = 6,
    SAY_ENRAGE      = 7,
    SAY_DEATH       = 8
};

enum Spells
{
    SPELL_EMPTY_ENERGY              = 202146,
    SPELL_ENERGIZE                  = 202143,
    SPELL_CALL_REINFORCEMENTS_1     = 192072,
    SPELL_CALL_REINFORCEMENTS_2     = 192073,
    SPELL_CALL_REINFORCEMENTS_BOTH  = 196563, ///< heroic +
    SPELL_THROW_SPEAR               = 192131,
    SPELL_CRASHING_WAVE             = 191900,
    SPELL_IMPALING_SPEAR_FILTER     = 191927,
    SPELL_IMPALING_SPEAR_FIXATE     = 192094,
    SPELL_IMPALING_SPEAR            = 191946,
    SPELL_IMPALING_SPEAR_DMG_NPC    = 191975,
    SPELL_IMPALING_SPEAR_DMG_PLR    = 191977,
    SPELL_IMPALING_SPEAR_KNOCK      = 193183,
    SPELL_ENRAGE                    = 197064,

    SPELL_BONUS_ROLL                = 226618
};

enum eEvents
{
    EVENT_CALL_REINFORC     = 1,
    EVENT_THROW_SPEAR       = 2,
    EVENT_CRASHING_WAVE     = 3,
    EVENT_IMPALING_SPEAR    = 4
};

enum eMisc
{
    DATA_TARGET_GUID
};

/// Warlord Parjesh - 91784
class boss_warlord_parjesh : public CreatureScript
{
    public:
        boss_warlord_parjesh() : CreatureScript("boss_warlord_parjesh") { }

        struct boss_warlord_parjeshAI : public BossAI
        {
            boss_warlord_parjeshAI(Creature* p_Creature) : BossAI(p_Creature, DATA_PARJESH)
            {
               m_IntroDone = false;
               m_IntroDone1 = false;
            }

            bool m_RandSum;
            bool m_Enrage;
            bool m_IntroDone;
            bool m_IntroDone1;
            uint64 m_TargetPlayer;

            bool m_Achievement = true;

            void SetGUID(uint64 p_Guid, int32 p_Type) override
            {
                if (p_Type == DATA_TARGET_GUID)
                    m_TargetPlayer = p_Guid;
            }

            uint64 GetGUID(int32 p_Type) override
            {
                if (p_Type == DATA_TARGET_GUID)
                    return m_TargetPlayer;

                return 0;
            }

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                _Reset();

                me->RemoveAurasDueToSpell(SPELL_ENERGIZE);
                me->RemoveAurasDueToSpell(SPELL_ENRAGE);

                DoCast(me, SPELL_EMPTY_ENERGY, true);
                m_RandSum = false;
                m_Enrage = false;

                m_Achievement = true;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                DoCast(me, SPELL_ENERGIZE, true);

                events.ScheduleEvent(EVENT_CALL_REINFORC, 3000);
                events.ScheduleEvent(EVENT_THROW_SPEAR, 11000);
                events.ScheduleEvent(EVENT_CRASHING_WAVE, 3000);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 190.0f))
                {
                   m_IntroDone = true;
                   ZoneTalk(SAY_INTRO);
                }

                if (!m_IntroDone1 && me->IsWithinDistInMap(p_Who, 100.0f))
                {
                   m_IntroDone1 = true;
                   ZoneTalk(SAY_INTRO_1);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);

                if (instance && IsMythic() && m_Achievement)
                    instance->DoCompleteAchievement(eEoAAchievements::ButYouSayHesJustAFriend);
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                events.ScheduleEvent(EVENT_IMPALING_SPEAR, 0);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_IMPALING_SPEAR_FILTER:
                    {
                        Talk(SAY_TARGET, p_Target->GetGUID());
                        SetGUID(p_Target->GetGUID(), DATA_TARGET_GUID);
                        break;
                    }
                    case SPELL_IMPALING_SPEAR_DMG_NPC:
                    {
                        Talk(SAY_SPEAR_NPC);
                        DoCast(p_Target, SPELL_IMPALING_SPEAR_KNOCK, true);
                        break;
                    }
                    case SPELL_IMPALING_SPEAR_DMG_PLR:
                    {
                        Talk(SAY_SPEAR_PLR);
                        DoCast(p_Target, SPELL_IMPALING_SPEAR_KNOCK, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                /// Defeat Warlord Parjesh without allowing any Hatecoil Shellbreakers to die
                if (p_Summon->GetEntry() == NPC_HATECOIL_SHELLBREAKER)
                    m_Achievement = false;
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(30, p_Damage) && !m_Enrage)
                {
                    m_Enrage = true;
                    DoCast(me, SPELL_ENRAGE, true);
                    Talk(SAY_ENRAGE);
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
                    case EVENT_CALL_REINFORC:
                    {
                        /// Spawn boths at the same time in heroic & mythic
                        if (IsHeroicOrMythic())
                            DoCast(SPELL_CALL_REINFORCEMENTS_BOTH);
                        else
                        {
                            if (!m_RandSum)
                            {
                                DoCast(SPELL_CALL_REINFORCEMENTS_1);
                                m_RandSum = true;
                            }
                            else
                            {
                                DoCast(SPELL_CALL_REINFORCEMENTS_2);
                                m_RandSum = false;
                            }
                        }

                        Talk(SAY_CALL);
                        events.ScheduleEvent(EVENT_CALL_REINFORC, 20000);
                        break;
                    }
                    case EVENT_THROW_SPEAR:
                    {
                        if (Unit* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, {}, 80.0f))
                            DoCast(l_Target, SPELL_THROW_SPEAR);

                        events.ScheduleEvent(EVENT_THROW_SPEAR, 16000);
                        break;
                    }
                    case EVENT_CRASHING_WAVE:
                    {
                        DoCast(SPELL_CRASHING_WAVE);
                        events.ScheduleEvent(EVENT_CRASHING_WAVE, 3000);
                        break;
                    }
                    case EVENT_IMPALING_SPEAR:
                    {
                        m_TargetPlayer = 0;
                        DoCast(me, SPELL_IMPALING_SPEAR_FILTER, true);
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
            return new boss_warlord_parjeshAI(p_Creature);
        }
};

/// Wandering Shellback - 91785
class npc_wandering_shellback : public CreatureScript
{
    public:
        npc_wandering_shellback() : CreatureScript("npc_wandering_shellback") {}

        struct npc_wandering_shellbackAI : public ScriptedAI
        {
            npc_wandering_shellbackAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Who*/)  override
            {
                events.ScheduleEvent(EVENT_1, 15000);
                events.ScheduleEvent(EVENT_2, 25000);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == 197141)
                {
                    DoCast(me, 15716);
                    DoZoneInCombat(me, 50.0f);
                }

                if (me->HasAura(195103) && p_Caster->GetTypeId() == TYPEID_PLAYER)
                    me->CastSpell(p_Caster, 195104);
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
                        DoCast(195105);
                        events.ScheduleEvent(EVENT_1, 15000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(195103);
                        events.ScheduleEvent(EVENT_2, 25000);
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
            return new npc_wandering_shellbackAI (p_Creature);
        }
};

/// Impaling Spear - 191946
class spell_parjesh_impaling_spear : public SpellScriptLoader
{
    public:
        spell_parjesh_impaling_spear() : SpellScriptLoader("spell_parjesh_impaling_spear") { }

        class spell_parjesh_impaling_spear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_parjesh_impaling_spear_SpellScript);

            uint64 m_TargetGuid = 0;
            uint64 m_ObjectTargetGuid = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || p_Targets.empty())
                    return;

                m_TargetGuid = l_Caster->IsAIEnabled ? l_Caster->AI()->GetGUID(DATA_TARGET_GUID) : 0;
                if (!m_TargetGuid)
                {
                    p_Targets.clear();
                    return;
                }

                Player* l_Player = ObjectAccessor::FindPlayer(m_TargetGuid);
                if (!l_Player)
                {
                    p_Targets.clear();
                    return;
                }

                std::list<Creature*> l_Creatures;

                l_Caster->GetCreatureListWithEntryInGrid(l_Creatures, NPC_HATECOIL_SHELLBREAKER, 100.0f);
                l_Caster->GetCreatureListWithEntryInGrid(l_Creatures, NPC_HATECOIL_CRESTRIDER, 100.0f);

                for (auto const& l_Creature : l_Creatures)
                {
                    if (l_Creature->isAlive())
                        p_Targets.push_back(l_Creature);
                }

                for (std::list<WorldObject*>::const_iterator l_Iter = p_Targets.begin(); l_Iter != p_Targets.end(); ++l_Iter)
                {
                    if (!(*l_Iter)->ToUnit())
                        continue;

                    if (Unit* l_Unit = (*l_Iter)->ToUnit())
                    {
                        if (l_Unit->isPet() || l_Unit->isHunterPet() || l_Unit->isTotem() || l_Unit->isGuardian())
                            continue;
                    }

                    if ((*l_Iter)->IsInBetween(l_Caster, l_Player, 3.0f))
                    {
                        Unit* l_Target = nullptr;
                        if (!m_ObjectTargetGuid || ((l_Target = Unit::GetUnit(*l_Caster, m_ObjectTargetGuid)) && l_Caster->GetDistance(l_Target) > l_Caster->GetDistance((*l_Iter))))
                            m_ObjectTargetGuid = (*l_Iter)->GetGUID();
                    }
                }

                if (!m_ObjectTargetGuid)
                    m_ObjectTargetGuid = l_Player->GetGUID();

                p_Targets.clear();

                if (Unit* l_Target = Unit::GetUnit(*l_Caster, m_ObjectTargetGuid))
                    p_Targets.push_back(l_Target);
            }

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (GetHitUnit()->GetGUID() != m_ObjectTargetGuid)
                    return;

                if (GetHitUnit()->GetTypeId() != TYPEID_PLAYER)
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_IMPALING_SPEAR_DMG_NPC, true);
                else
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_IMPALING_SPEAR_DMG_PLR, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_parjesh_impaling_spear_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST_2);
                OnHit += SpellHitFn(spell_parjesh_impaling_spear_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_parjesh_impaling_spear_SpellScript();
        }
};

/// Called by Call Reinforcements (196563)
/// Used in heroic/mythic/mythic+
class spell_parjesh_call_reinforcements : public SpellScriptLoader
{
    public:
        spell_parjesh_call_reinforcements() : SpellScriptLoader("spell_parjesh_call_reinforcements") { }

        class spell_parjesh_call_reinforcements_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_parjesh_call_reinforcements_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();

                l_Caster->CastSpell(l_Caster, SPELL_CALL_REINFORCEMENTS_1, true);
                l_Caster->CastSpell(l_Caster, SPELL_CALL_REINFORCEMENTS_2, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_parjesh_call_reinforcements_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_parjesh_call_reinforcements_SpellScript();
        }
};

/// Armorshell - 196175
class spell_parjesh_armorshell : public SpellScriptLoader
{
    public:
        spell_parjesh_armorshell() : SpellScriptLoader("spell_parjesh_armorshell") { }

        enum eSpell
        {
            SpellArmorshellShrapnel = 196183
        };

        class spell_parjesh_armorshell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_parjesh_armorshell_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();
                if (l_Mode == AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                {
                    if (Unit* l_Caster = GetCaster())
                        l_Caster->CastSpell(l_Caster, eSpell::SpellArmorshellShrapnel, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_parjesh_armorshell_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_parjesh_armorshell_AuraScript();
        }
};

/// Bubble Shield - 196031
class spell_parjesh_bubble_shield : public SpellScriptLoader
{
    public:
        spell_parjesh_bubble_shield() : SpellScriptLoader("spell_parjesh_bubble_shield") { }

        enum eSpell
        {
            SpellFeedback = 196035
        };

        class spell_parjesh_bubble_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_parjesh_bubble_shield_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();
                if (l_Mode == AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                {
                    if (Unit* l_Caster = GetCaster())
                        l_Caster->CastSpell(l_Caster, eSpell::SpellFeedback, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_parjesh_bubble_shield_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_parjesh_bubble_shield_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_warlord_parjesh()
{
    /// Boss
    new boss_warlord_parjesh();

    /// Creature
    new npc_wandering_shellback();

    /// Spells
    new spell_parjesh_impaling_spear();
    new spell_parjesh_call_reinforcements();
    new spell_parjesh_armorshell();
    new spell_parjesh_bubble_shield();
}
#endif
