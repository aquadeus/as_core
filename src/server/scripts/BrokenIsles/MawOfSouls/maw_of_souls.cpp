////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "maw_of_souls.hpp"

// 97097
struct npc_helarjar_champion_mos : public ScriptedAI
{
    npc_helarjar_champion_mos(Creature* creature) : ScriptedAI(creature) {}

    enum Spells
    {
        SPELL_GHOSTLY_RAGE          = 194663,
        SPELL_BONE_CHILLING_SCREAM  = 198405
    };

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustDied(Unit* /*killer*/) override {}

    void EnterCombat(Unit* /*who*/) override
    {
        events.RescheduleEvent(EVENT_1, urand(7000, 9000));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (auto eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_1:
                {
                    DoCast(SPELL_GHOSTLY_RAGE);
                    events.RescheduleEvent(EVENT_1, urand(18000, 20000));
                    events.RescheduleEvent(EVENT_2, urand(7000, 8000));
                    break;
                }
                case EVENT_2:
                {
                    if (auto victim = me->getVictim())
                    {
                        if (victim->GetExactDist(me) < 15.f)
                        {
                            DoCast(SPELL_BONE_CHILLING_SCREAM);
                            break;
                        }
                    }
                    events.RescheduleEvent(EVENT_2, 2000);
                    break;
                }
            }
        }
        DoMeleeAttackIfReady();
    }
};

// 97185
struct npc_grimewalker_mos : public ScriptedAI
{
    npc_grimewalker_mos(Creature* creature) : ScriptedAI(creature) {}

    enum Spells
    {
        SPELL_BONE_HAMMER       = 194583,
        SPELL_BILE_BREATH       = 194099,
        SPELL_POISONOUS_SLAM    = 194106
    };

    EventMap events;
    uint64 lastTarget = 0;

    void Reset() override
    {
        lastTarget = 0;
        events.Reset();
        DoCast(me, SPELL_BONE_HAMMER, true);
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void JustDied(Unit* /*killer*/) override {}

    void EnterCombat(Unit* /*who*/) override
    {
        events.RescheduleEvent(EVENT_1, urand(7000, 9000));
        events.RescheduleEvent(EVENT_2, 5000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (auto eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_1:
                {
                    if (auto vict = me->getVictim())
                    {
                        auto orient = vict->GetOrientation();
                        lastTarget = vict->GetGUID();
                        DeleteOperations();
                        me->StopAttack();
                        if (auto v = ObjectAccessor::GetUnit(*me, lastTarget))
                            DoCast(v, SPELL_BILE_BREATH);
                            
                        events.RescheduleEvent(EVENT_3, 4000);
                        events.RescheduleEvent(EVENT_1, urand(16000, 18000));
                    }
                    break;
                }
                case EVENT_2:
                {
                    me->SetReactState(REACT_PASSIVE);
                    DoCast(SPELL_POISONOUS_SLAM);
                    events.RescheduleEvent(EVENT_2, urand(8000, 9000));
                    AddTimedDelayedOperation(1500, [_me=me]() -> void { _me->SetReactState(REACT_AGGRESSIVE); });
                    break;
                }
                case EVENT_3:
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (auto v = ObjectAccessor::GetUnit(*me, lastTarget))
                        me->Attack(v, true);
                    break;
                }
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// 97365
struct npc_seacursed_mistmender_mos : public ScriptedAI
{
    npc_seacursed_mistmender_mos(Creature* creature) : ScriptedAI(creature) {}

    enum Spells
    {
        SPELL_SHROUD_BOLT       = 194610,
        SPELL_TORRENT_OF_SOULS  = 199514
    };

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustDied(Unit* /*killer*/) override {}

    void EnterCombat(Unit* /*who*/) override
    {
        events.RescheduleEvent(EVENT_1, urand(7000, 9000));
        events.RescheduleEvent(EVENT_2, 5000);
    }

    Unit* HealTarget() const
    {
        std::list<Creature*> list;
        me->GetCreatureListInGrid(list, 40.f);
        if (!list.empty())
        {
            std::list<Creature*> temp;
            for (auto& itr : list)
            {
                if (!itr->isAlive() || !itr->isInCombat() || itr->HealthAbovePct(70))
                    continue;
                temp.push_back(itr);
            }

            if (temp.empty())
                return nullptr;

            temp.sort(JadeCore::HealthPctOrderPred());
            return temp.front();
        }
        return nullptr;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (auto eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_1:
                {
                    DoCastVictim(SPELL_SHROUD_BOLT);
                    events.RescheduleEvent(EVENT_1, urand(6000, 7000));
                    break;
                }
                case EVENT_2:
                {
                    if (auto target = HealTarget())
                    {
                        DoCast(target, SPELL_TORRENT_OF_SOULS);
                        events.RescheduleEvent(EVENT_2, urand(18000, 20000));
                        break;
                    }
                    events.RescheduleEvent(EVENT_2, 2000);
                    break;
                }
            }
        }
        DoMeleeAttackIfReady();
    }
};

// 199514
class spell_mos_torment_of_souls : public AuraScript
{
    PrepareAuraScript(spell_mos_torment_of_souls);

    void OnTick(AuraEffect const* aurEff)
    {
        auto target = GetTarget();
        auto caster = GetCaster();
        if (!target || !caster || !aurEff)
            return;

        target->CastSpell(target, 199516, true, nullptr, aurEff, caster->GetGUID());
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mos_torment_of_souls::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

/// Barbed Spear - 194673
class spell_barbed_spear_aoe_jump : public SpellScriptLoader
{
    public:
        spell_barbed_spear_aoe_jump() : SpellScriptLoader("spell_barbed_spear_aoe_jump") { }


        enum eSpells
        {
            SpellBarbedSpearJump = 191980,
            SpellBarbedSpearMark = 191960,
            SpellBarbedSpearDmg = 194674
        };

        class spell_barbed_spear_aoe_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_barbed_spear_aoe_jump_SpellScript);

            bool l_Casted = false;

            bool Load() override
            {
                l_Casted = false;
                return SpellScript::Load();
            }

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                Aura* l_Barbed = l_Caster->GetAura(eSpells::SpellBarbedSpearMark);

                if (l_Barbed && l_Barbed->GetCaster())
                {
                    Unit* l_DestJump = l_Barbed->GetCaster();

                    l_Target->CastSpell(l_DestJump, eSpells::SpellBarbedSpearJump, true);

                    if (!l_Casted)
                    {
                        l_Casted = true;
                        l_Caster->CastSpell(l_DestJump, eSpells::SpellBarbedSpearJump, true);
                        l_Barbed->GetCaster()->CastSpell(l_Barbed->GetCaster(), eSpells::SpellBarbedSpearDmg, false);
                    }
                }
            }


            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_barbed_spear_aoe_jump_SpellScript::HandleEffectHitTarget, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_barbed_spear_aoe_jump_SpellScript();
        }
};

/// Brackwater Blast - 200208
class spell_mos_brackwater_blast : public SpellScriptLoader
{
    public:
        spell_mos_brackwater_blast() : SpellScriptLoader("spell_mos_brackwater_blast")
        {}

        enum eSpells
        {
            SpellBrackwaterBlastDmg = 201397,
        };

        class spell_mos_brackwater_blast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mos_brackwater_blast_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();

                l_Caster->CastSpell(l_Owner, eSpells::SpellBrackwaterBlastDmg, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mos_brackwater_blast_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mos_brackwater_blast_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_maw_of_souls()
{
    RegisterCreatureAI(npc_helarjar_champion_mos);
    RegisterCreatureAI(npc_grimewalker_mos);
    RegisterCreatureAI(npc_seacursed_mistmender_mos);
    RegisterAuraScript(spell_mos_torment_of_souls);
    new spell_mos_brackwater_blast();
    new spell_barbed_spear_aoe_jump();
}
#endif
