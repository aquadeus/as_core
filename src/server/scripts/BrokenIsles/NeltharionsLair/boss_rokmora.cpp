////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "neltharions_lair.hpp"

enum Says
{
    SAY_AGGRO   = 0,
    SAY_RAZOR   = 1,
    SAY_DEATH   = 2
};

enum Spells
{
    SPELL_GAIN_ENERGY               = 193245,
    SPELL_BRITTLE                   = 187714,
    SPELL_INTRO_ULAROGG             = 209390, ///< Boss 01 Intro Ularogg Cast
    SPELL_INTRO_EMERGE              = 209394, ///< Boss 01 Intro Emerge

    SPELL_SHATTER_START_CALL_1      = 198122, ///< Conversation Shatter
    SPELL_SHATTER_END_CALL_1        = 198135, ///< Conversation Shatter
    SPELL_SHATTER_END_CALL_2        = 198136, ///< Conversation Shatter
    SPELL_SHATTER                   = 188114,
    SPELL_SHATTER_FOR_TRASHS        = 200247,
    SPELL_RAZOR_SHARDS_CALL         = 198125, ///< Conversation Razor Shards
    SPELL_RAZOR_SHARDS              = 209718,
    SPELL_RAZOR_SHARDS_VISUAL       = 188207,
    SPELL_RAZOR_SHARDS_DUMMY        = 188169,

    /// Heroic
    SPELL_CRYSTALLINE_GROUND        = 198024,
    SPELL_CRYSTALLINE_GROUND_DMG    = 198028,
    SPELL_RUPTURING_SKITTER         = 215929,

    SPELL_CHOKING_DUST_AT           = 192799,

    SPELL_BONUS_ROLL                = 226640
};

enum eEvents
{
    EVENT_RAZOR_SHARDS          = 1,
    EVENT_CRYSTALLINE_GROUND    = 2
};

/// Rokmora - 91003
class boss_rokmora : public CreatureScript
{
    public:
        boss_rokmora() : CreatureScript("boss_rokmora") { }

        struct boss_rokmoraAI : public BossAI
        {
            boss_rokmoraAI(Creature* p_Creature) : BossAI(p_Creature, DATA_ROKMORA)
            {
                m_IntroDone = false;
            }

            bool m_IntroDone;

            Position m_RazorShardPos;

            void Reset() override
            {
                m_RazorShardPos = Position();

                _Reset();

                DoCast(me, SPELL_BRITTLE, true);

                me->RemoveAurasDueToSpell(SPELL_GAIN_ENERGY);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->setPowerType(Powers::POWER_MANA);
                me->SetMaxPower(POWER_MANA, 100);
                me->SetPower(POWER_MANA, 0);

                if (!m_IntroDone)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Creature* l_Ularogg = me->FindNearestCreature(NPC_COSMETIC_ULAROGG, 50.0f, true))
                            l_Ularogg->CastSpell(l_Ularogg, SPELL_INTRO_ULAROGG);
                        else
                        {
                            m_IntroDone = true;
                            return;
                        }

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                    });
                }

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                DoCast(me, SPELL_GAIN_ENERGY, true);

                events.ScheduleEvent(EVENT_RAZOR_SHARDS, 30000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_CRYSTALLINE_GROUND, 4000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Navarrogg = Creature::GetCreature(*me, l_Instance->GetData64(NPC_NAVAROGG)))
                    {
                        if (l_Navarrogg->IsAIEnabled)
                            l_Navarrogg->AI()->DoAction(3);
                    }

                    if (Creature* l_Ebonhorn = Creature::GetCreature(*me, l_Instance->GetData64(NPC_SPIRITWALKER_EBONHORN)))
                    {
                        if (l_Ebonhorn->IsAIEnabled)
                            l_Ebonhorn->AI()->DoAction(2);
                    }
                }

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_INTRO_EMERGE:
                    {
                        m_IntroDone = true;
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    }
                    case SPELL_RAZOR_SHARDS_DUMMY:
                    {
                        Position l_Pos  = *me;
                        float l_Angle   = 60.0f * M_PI / 180.0f;

                        for (uint8 l_I = 0; l_I < 20; ++l_I)
                        {
                            float l_Distance    = l_Pos.GetExactDist(&m_RazorShardPos) + frand(-2.5f, 10.0f);
                            float l_Orientation = l_Pos.GetAngle(&m_RazorShardPos) + frand(-(l_Angle / 2.0f), l_Angle / 2.0f);

                            float l_X = me->m_positionX + l_Distance * std::cos(l_Orientation);
                            float l_Y = me->m_positionY + l_Distance * std::sin(l_Orientation);
                            float l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, m_RazorShardPos.m_positionZ);

                            me->CastSpell({ l_X, l_Y, l_Z, l_Orientation }, SPELL_RAZOR_SHARDS_VISUAL, true);
                        }

                        AddTimedDelayedOperation(1, [this]() -> void
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
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case SPELL_SHATTER_FOR_TRASHS:
                    {
                        p_Target->CastSpell(p_Target, SPELL_RUPTURING_SKITTER, true);
                        break;
                    }
                    case SPELL_RAZOR_SHARDS:
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        m_RazorShardPos = *p_Target;
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case SPELL_RAZOR_SHARDS:
                    {
                        p_Targets.clear();

                        if (Unit* l_Target = me->getVictim())
                            p_Targets.push_back(l_Target);
                        else if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            p_Targets.push_back(l_Target);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_CRYSTALLINE_GROUND, 4000);
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                p_Value = 0;
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
                    case EVENT_RAZOR_SHARDS:
                    {
                        DoCast(me, SPELL_RAZOR_SHARDS_CALL, true);
                        DoCast(SPELL_RAZOR_SHARDS);
                        Talk(SAY_RAZOR);
                        events.ScheduleEvent(EVENT_RAZOR_SHARDS, 26000);
                        break;
                    }
                    case EVENT_CRYSTALLINE_GROUND:
                    {
                        DoCast(SPELL_CRYSTALLINE_GROUND);
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
            return new boss_rokmoraAI(p_Creature);
        }
};

/// Blightshard Skitter - 97720
class npc_rokmora_blightshard_skitter : public CreatureScript
{
    public:
        npc_rokmora_blightshard_skitter() : CreatureScript("npc_rokmora_blightshard_skitter") { }

        struct npc_rokmora_blightshard_skitterAI : public ScriptedAI
        {
            npc_rokmora_blightshard_skitterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            float m_RemainingHealth;

            void Reset() override
            {
                m_RemainingHealth = 0.0f;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    m_RemainingHealth = me->GetHealthPct();
                    DoCast(me, SPELL_CHOKING_DUST_AT, true);
                    me->DespawnOrUnsummon(30 * TimeConstants::IN_MILLISECONDS);
                }
            }

            float GetFData(uint32 p_ID /*= 0*/) const override
            {
                return m_RemainingHealth;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rokmora_blightshard_skitterAI(p_Creature);
        }
};

/// Gain Energy - 193245
class spell_rokmora_gain_energy : public SpellScriptLoader
{
    public:
        spell_rokmora_gain_energy() : SpellScriptLoader("spell_rokmora_gain_energy") { }

        class spell_rokmora_gain_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rokmora_gain_energy_AuraScript);

            bool m_CheckTalk = false;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster)
                    return;

                if (l_Caster->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (l_Caster->GetPower(POWER_MANA) == 0 && m_CheckTalk)
                {
                    m_CheckTalk = false;
                    uint8 l_Rand = urand(0, 1);
                    l_Caster->CastSpell(l_Caster, l_Rand ? SPELL_SHATTER_END_CALL_1 : SPELL_SHATTER_END_CALL_2, true);
                }

                if (l_Caster->GetPower(POWER_MANA) >= 100)
                {
                    l_Caster->CastSpell(l_Caster, SPELL_SHATTER_START_CALL_1, true);
                    l_Caster->CastSpell(l_Caster, SPELL_SHATTER);

                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(true);

                    m_CheckTalk = true;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rokmora_gain_energy_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rokmora_gain_energy_AuraScript();
        }
};

/// Crystalline Ground - 215898
class spell_rokmora_crystalline_ground : public SpellScriptLoader
{
    public:
        spell_rokmora_crystalline_ground() : SpellScriptLoader("spell_rokmora_crystalline_ground") { }

        class spell_rokmora_crystalline_ground_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rokmora_crystalline_ground_AuraScript);

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (l_Target->IsMoving())
                    l_Target->CastSpell(l_Target, SPELL_CRYSTALLINE_GROUND_DMG, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rokmora_crystalline_ground_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rokmora_crystalline_ground_AuraScript();
        }
};

/// Rputuring Skitter - 215929
class spell_rokmora_rupturing_skitter : public SpellScriptLoader
{
    public:
        spell_rokmora_rupturing_skitter() : SpellScriptLoader("spell_rokmora_rupturing_skitter") { }

        class spell_rokmora_rupturing_skitter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rokmora_rupturing_skitter_SpellScript);

            void HandleDamage()
            {
                if (GetCaster() == nullptr)
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->IsAIEnabled)
                    {
                        float l_HealthPct   = l_Caster->AI()->GetFData();
                        float l_Damage      = GetHitDamage();
                        float l_NewDamage   = l_Damage * (l_HealthPct / 100.0f);

                        SetHitDamage(int32(l_NewDamage));
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rokmora_rupturing_skitter_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rokmora_rupturing_skitter_SpellScript();
        }
};

/// Submerge - 183433
class spell_submerge : public SpellScriptLoader
{
    public:
        spell_submerge() : SpellScriptLoader("spell_submerge") { }

        class spell_submerge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_submerge_AuraScript);

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                std::list<Player*> targets;
                GetPlayerListInGrid(targets, target, 40);
                JadeCore::Containers::RandomResizeList(targets, 1);
                for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (!targets.empty())
                    {
                        target->CastSpell((*itr), 183438);
                        target->CastSpell((*itr), 183430, true);
                    }
                    target->SetVisible(true);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->SetVisible(false);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_submerge_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_submerge_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_submerge_AuraScript();
        }
};

/// Crystalline Ground - 198024
class areatrigger_rokmora_crystalline_ground : public AreaTriggerEntityScript
{
    public:
        areatrigger_rokmora_crystalline_ground() : AreaTriggerEntityScript("areatrigger_rokmora_crystalline_ground"), m_ScaleTime(0) { }

        enum eSpell
        {
            CrystallineGroundDebuff = 215898
        };

        uint32 m_ScaleTime;
        uint32 m_MaxScaleTime = 10 * TimeConstants::IN_MILLISECONDS;

        std::set<uint64> m_AffectedPlayers;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                m_ScaleTime += p_Time;

                std::list<Unit*> l_TargetList;

                float l_BaseRadius  = 45.0f;
                float l_ProgressPCT = float(m_ScaleTime) * 100.0f / float(m_MaxScaleTime);
                float l_RealRadius  = (l_BaseRadius - (l_BaseRadius * (l_ProgressPCT / 100.0f))) + 1.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_BaseRadius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                l_Caster->VisitNearbyObject(l_BaseRadius, l_Searcher);

                if (!l_TargetList.empty())
                {
                    l_TargetList.remove_if([&](Unit* p_Unit) -> bool
                    {
                        if (p_Unit == nullptr)
                            return true;

                        if (p_Unit->GetDistance(*p_AreaTrigger) > l_BaseRadius ||
                            p_Unit->GetDistance(*p_AreaTrigger) > l_RealRadius)
                            return true;

                        return false;
                    });
                }

                std::set<uint64> l_Targets;

                for (Unit* l_Iter : l_TargetList)
                {
                    l_Targets.insert(l_Iter->GetGUID());

                    if (!l_Iter->HasAura(eSpell::CrystallineGroundDebuff))
                    {
                        if (m_AffectedPlayers.find(l_Iter->GetGUID()) == m_AffectedPlayers.end())
                            m_AffectedPlayers.insert(l_Iter->GetGUID());

                        l_Iter->CastSpell(l_Iter, eSpell::CrystallineGroundDebuff, true);
                    }
                }

                for (std::set<uint64>::iterator l_Iter = m_AffectedPlayers.begin(); l_Iter != m_AffectedPlayers.end();)
                {
                    if (l_Targets.find((*l_Iter)) != l_Targets.end())
                    {
                        ++l_Iter;
                        continue;
                    }

                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, (*l_Iter)))
                    {
                        l_Iter = m_AffectedPlayers.erase(l_Iter);
                        l_Unit->RemoveAura(eSpell::CrystallineGroundDebuff);
                        continue;
                    }

                    ++l_Iter;
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                for (uint64 l_Guid : m_AffectedPlayers)
                {
                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, l_Guid))
                        l_Unit->RemoveAura(eSpell::CrystallineGroundDebuff);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_rokmora_crystalline_ground();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_rokmora()
{
    /// Boss
    new boss_rokmora();

    /// Creature
    new npc_rokmora_blightshard_skitter();

    /// Spells
    new spell_rokmora_gain_energy();
    new spell_rokmora_crystalline_ground();
    new spell_rokmora_rupturing_skitter();
    new spell_submerge();

    /// AreaTrigger
    new areatrigger_rokmora_crystalline_ground();
}
#endif
