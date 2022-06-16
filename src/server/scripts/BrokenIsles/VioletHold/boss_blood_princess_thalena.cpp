////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

enum Says
{
    SAY_DEATH           = 0,
};

enum Spells
{
    SPELL_SHROUD_OF_SORROW          = 203033,
    SPELL_SHROUD_OF_SORROW_DMG      = 203057,
    SPELL_VAMPYR_KISS               = 202652, //Find target
    SPELL_VAMPYR_KISS_JUMP          = 202676,
    SPELL_VAMPYR_KISS_DMG           = 202766,
    SPELL_VAMPYR_ESSENCE_1          = 202779, //Select target
    SPELL_VAMPYR_ESSENCE_2          = 202810, //Self target
    SPELL_VAMPYR_ESSENCE_MOD        = 202781, //Mod dmg, threat...
    SPELL_FRENZIED_BLOODTHIRST      = 202792,
    SPELL_UNCONTROLLABLE_FRENZY     = 202804,
    SPELL_BLOOD_SWARM               = 202659,
    SPELL_BLOOD_SWARM_TRIG          = 202785,
    SPELL_BLOOD_SWARM_AT            = 202944,
    SPELL_BLOOD_CALL_CAST           = 203381,
    SPELL_BLOOD_CALL_VISUAL         = 203405,
    SPELL_BLOOD_CALL_SUMMON         = 203452,
    SPELL_CONGEALED_BLOOD_DESTRUCT  = 203471,
    SPELL_ETERNAL_HUNGER            = 203462,

    SPELL_BONUS_LOOT                = 226657
};

enum eEvents
{
    EVENT_VAMPYR_KISS = 1,
    EVENT_BLOOD_SWARM,
    EVENT_BLOOD_CALL
};

uint32 vampireAuras[5] =
{
    SPELL_VAMPYR_ESSENCE_1,
    SPELL_VAMPYR_ESSENCE_2,
    SPELL_FRENZIED_BLOODTHIRST,
    SPELL_UNCONTROLLABLE_FRENZY,
    SPELL_VAMPYR_ESSENCE_MOD
};

bool IsVampire(Unit* unit)
{
    for (uint8 i = 0; i < 5; ++i)
        if (unit->HasAura(vampireAuras[i]))
            return true;

    return false;
}

/// Blood-Princess Thal'ena - 102431
class boss_blood_princess_thalena : public CreatureScript
{
    public:
        boss_blood_princess_thalena() : CreatureScript("boss_blood_princess_thalena") { }

        struct boss_blood_princess_thalenaAI : public BossAI
        {
            boss_blood_princess_thalenaAI(Creature* creature) : BossAI(creature, DATA_THALENA)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                removeloot = false;
            }

            bool removeloot;

            void Reset() override
            {
                summons.DespawnAll();

                _Reset();

                for (uint8 i = 0; i < 4; ++i)
                    instance->DoRemoveAurasDueToSpellOnPlayers(vampireAuras[i]);

                if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                    me->SetReactState(REACT_DEFENSIVE);
            }

            void EnterCombat(Unit* who) override
            {
                Conversation* l_Conversation = new Conversation;
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 1231, who, nullptr, *who))
                    delete l_Conversation;

                _EnterCombat();
                DoCast(me, SPELL_SHROUD_OF_SORROW, true);

                events.ScheduleEvent(EVENT_VAMPYR_KISS, 7000);
                events.ScheduleEvent(EVENT_BLOOD_SWARM, 20000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_BLOOD_CALL, 30000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();

                Talk(SAY_DEATH);
                _JustDied();

                for (uint8 i = 0; i < 4; ++i)
                    instance->DoRemoveAurasDueToSpellOnPlayers(vampireAuras[i]);

                if (removeloot)
                {
                    me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                    me->ClearLootContainers();
                }

                CastSpellToPlayers(me, SPELL_BONUS_LOOT, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* spell) override
            {
                if (p_Target == nullptr)
                    return;

                switch (spell->Id)
                {
                    case SPELL_VAMPYR_KISS_JUMP:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        DoCast(p_Target, SPELL_VAMPYR_KISS_DMG, true);
                        break;
                    }
                    case SPELL_BLOOD_SWARM:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        DoCast(me, SPELL_BLOOD_SWARM_TRIG, true);
                        break;
                    }
                    case SPELL_BLOOD_CALL_CAST:
                    {
                        if (p_Target->IsPlayer())
                            break;

                        me->CastSpell(p_Target, SPELL_BLOOD_CALL_VISUAL, true);
                        break;
                    }
                    case SPELL_BLOOD_CALL_VISUAL:
                    {
                        if (p_Target->IsPlayer())
                            break;

                        p_Target->CastSpell(p_Target, SPELL_BLOOD_CALL_SUMMON, true, nullptr, nullptr, me->GetGUID());

                        p_Target->CastSpell(p_Target, SPELL_CONGEALED_BLOOD_DESTRUCT, true);

                        p_Target->RemoveAllAreasTrigger();
                        p_Target->ToCreature()->DespawnOrUnsummon(100);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const action) override
            {
                if (action == ACTION_REMOVE_LOOT)
                    removeloot = true;
            }

            void UpdateAI(uint32 const diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_VAMPYR_KISS:
                    {
                        DoCast(me, SPELL_VAMPYR_KISS, true);
                        Unit* target = me->SelectVictim();
                        Conversation* conversation = new Conversation;
                        if (!conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 1232, target, NULL, *target))
                            delete conversation;

                        break;
                    }
                    case EVENT_BLOOD_SWARM:
                    {
                        DoCast(SPELL_BLOOD_SWARM);
                        events.ScheduleEvent(EVENT_BLOOD_SWARM, 21000);
                        break;
                    }
                    case EVENT_BLOOD_CALL:
                    {
                        DoCast(me, SPELL_BLOOD_CALL_CAST, true);
                        events.ScheduleEvent(EVENT_BLOOD_CALL, 30000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_blood_princess_thalenaAI (creature);
        }
};

/// Blood-Princess Thal'ena (trigger) - 102659
class npc_thalena_pool_of_blood : public CreatureScript
{
public:
    npc_thalena_pool_of_blood() : CreatureScript("npc_thalena_pool_of_blood") {}

    struct npc_thalena_pool_of_bloodAI : public ScriptedAI
    {
        npc_thalena_pool_of_bloodAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override {}

        void IsSummonedBy(Unit* summoner) override
        {
            summoner->CastSpell(me, SPELL_BLOOD_SWARM_AT, true);
        }

        void UpdateAI(uint32 const diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_thalena_pool_of_bloodAI(creature);
    }
};

/// Congealed Blood - 102941
class npc_thalena_congealed_blood : public CreatureScript
{
    public:
        npc_thalena_congealed_blood() : CreatureScript("npc_thalena_congealed_blood") { }

        struct npc_thalena_congealed_bloodAI : public ScriptedAI
        {
            npc_thalena_congealed_bloodAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            uint64 m_SummonerGuid = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Unit* l_Summoner = Unit::GetUnit(*me, m_SummonerGuid))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(l_Summoner);
                    }
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_SummonerGuid)
                {
                    if (Unit* l_Summoner = Unit::GetUnit(*me, m_SummonerGuid))
                    {
                        if (me->GetDistance(l_Summoner) < 1.0f)
                        {
                            me->CastSpell(me, SPELL_ETERNAL_HUNGER, true);
                            me->CastSpell(me, SPELL_CONGEALED_BLOOD_DESTRUCT, true);

                            me->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_thalena_congealed_bloodAI(p_Creature);
        }
};

//203035
class spell_thalena_shroud_of_sorrow : public SpellScriptLoader
{
    public:
        spell_thalena_shroud_of_sorrow() : SpellScriptLoader("spell_thalena_shroud_of_sorrow") { }

        class spell_thalena_shroud_of_sorrow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thalena_shroud_of_sorrow_SpellScript);
            
            uint8 vampireCount = 0;
            uint32 spellId = 0;
            int32 damage = 0;

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> targetList;
                for (auto const& target : targets)
                {
                    if (!IsVampire(target->ToPlayer()))
                        targetList.push_back(target);
                    else
                        vampireCount++;
                }

                targets.clear();
                targets = targetList;
            }

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_SHROUD_OF_SORROW_DMG))
                    damage = spellInfo->Effects[EFFECT_0].BasePoints;

                if (!vampireCount)
                    vampireCount = 1;

                damage *= vampireCount;
                GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_SHROUD_OF_SORROW_DMG, &damage, 0, 0, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thalena_shroud_of_sorrow_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_thalena_shroud_of_sorrow_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_thalena_shroud_of_sorrow_SpellScript();
        }
};

//202805
class spell_thalena_vampiric_bite : public SpellScriptLoader
{
    public:
        spell_thalena_vampiric_bite() : SpellScriptLoader("spell_thalena_vampiric_bite") { }

        class spell_thalena_vampiric_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thalena_vampiric_bite_SpellScript);

            SpellCastResult CheckTarget()
            {
                if (IsVampire(GetExplTargetUnit()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TARGET_VAMPIRES);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster->IsPlayer())
                    return;

                l_Caster->CastSpell(l_Caster, SPELL_VAMPYR_ESSENCE_2, true);
                l_Caster->RemoveAura(SPELL_FRENZIED_BLOODTHIRST);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_thalena_vampiric_bite_SpellScript::CheckTarget);
                OnHit += SpellHitFn(spell_thalena_vampiric_bite_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_thalena_vampiric_bite_SpellScript();
        }
};

//202792
class spell_thalena_frenzied_bloodthirst : public SpellScriptLoader
{
    public:
        spell_thalena_frenzied_bloodthirst() : SpellScriptLoader("spell_thalena_frenzied_bloodthirst") { }

        class spell_thalena_frenzied_bloodthirst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_thalena_frenzied_bloodthirst_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* target = GetTarget();
                if (!target)
                    return;

                if (InstanceScript* instance = target->GetInstanceScript())
                    if (Creature* bloodPrincess = instance->instance->GetCreature(instance->GetData64(DATA_THALENA)))
                    {
                        //bloodPrincess->AI()->Talk(SAY_MIND_CONTROL);
                        bloodPrincess->CastSpell(target, SPELL_UNCONTROLLABLE_FRENZY, true);
                    }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_thalena_frenzied_bloodthirst_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_thalena_frenzied_bloodthirst_AuraScript();
        }
};

/// Vampyr Kiss - 202652
class spell_thalena_vampyr_kiss : public SpellScriptLoader
{
    public:
        spell_thalena_vampyr_kiss() : SpellScriptLoader("spell_thalena_vampyr_kiss") { }

        class spell_thalena_vampyr_kiss_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thalena_vampyr_kiss_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->ToPlayer()->GetRoleForGroup() != Roles::ROLE_HEALER)
                        return true;

                    if (IsVampire(p_Object->ToUnit()))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thalena_vampyr_kiss_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_thalena_vampyr_kiss_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_blood_princess_thalena()
{
    /// Boss
    new boss_blood_princess_thalena();

    /// Creatures
    new npc_thalena_pool_of_blood();
    new npc_thalena_congealed_blood();

    /// Spells
    new spell_thalena_shroud_of_sorrow();
    new spell_thalena_vampiric_bite();
    new spell_thalena_frenzied_bloodthirst();
    new spell_thalena_vampyr_kiss();
}
#endif
