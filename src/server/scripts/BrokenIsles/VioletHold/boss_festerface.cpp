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
    SAY_AGGRO           = 0,
    SAY_VOMIT           = 1,
    SAY_YUM             = 2,
   // SAY_          = 3  //Taste better going in!
    SAY_DEATH           = 4,
};

enum Spells
{
    SPELL_FESTERFACE_AGGRO      = 202000,
    SPELL_CONGEALING_VOMIT      = 201598,
    SPELL_CONGEALING_VOMIT_VIS  = 201624,
    SPELL_CONGEALING_VOMIT_SUM  = 201569,
    SPELL_YUM                   = 201715,

    SPELL_GREEN_SLIME_COSMETICS = 205156,
    SPELL_FETID_STENCH          = 201476,
    SPELL_RECONGEALING          = 201495,
    SPELL_RECONGEALING_SEARCH   = 202265, //Search Festerface
    SPELL_UH_OH                 = 201729,
    SPELL_NECROTIC_AURA         = 201753,

    SPELL_BONUS_LOOT            = 226659
};

enum eEvents
{
    EVENT_CONGEALING_VOMIT    = 1,
    EVENT_UH_OH               = 2
};

/// Festerface - 101995
class boss_festerface : public CreatureScript
{
public:
    boss_festerface() : CreatureScript("boss_festerface") {}

    struct boss_festerfaceAI : public BossAI
    {
        boss_festerfaceAI(Creature* creature) : BossAI(creature, DATA_FESTERFACE) 
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            removeloot = false;
        }

        bool removeloot;
        bool activeEnergy;
        uint32 m_RegenTimer;

        void Reset() override
        {
            _Reset();

            if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                me->SetReactState(REACT_DEFENSIVE);

            if (IsHeroicOrMythic())
            {
                me->setPowerType(Powers::POWER_ENERGY);
                me->SetPower(Powers::POWER_ENERGY, 0);
                me->SetMaxPower(Powers::POWER_ENERGY, 100);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
            }

            activeEnergy = false;
            m_RegenTimer = 0;

            if (instance)
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NECROTIC_AURA);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();
            DoCast(me, SPELL_FESTERFACE_AGGRO, true);

            events.ScheduleEvent(EVENT_CONGEALING_VOMIT, 4000); //39:37, 40:28, 41:19

            if (IsHeroicOrMythic())
            {
                activeEnergy = true;
                m_RegenTimer = 1 * TimeConstants::IN_MILLISECONDS;
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();

            if (removeloot)
            {
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                me->ClearLootContainers();
            }

            CastSpellToPlayers(me, SPELL_BONUS_LOOT, true);
        }

        void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer) override
        {
            if (p_Summon->GetEntry() == NPC_BLACK_BILE && instance)
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NECROTIC_AURA);
                instance->SetData(eData::DATA_BLACK_BILE_KILLED, 1);
            }
        }

        void RegeneratePower(Powers p_Power, int32& p_Value) override
        {
            /// Only regens by script
            p_Value = 0;
        }

        void OnSpellCasted(SpellInfo const* p_SpellInfo) override
        {
            if (p_SpellInfo->Id == SPELL_UH_OH)
            {
                if (instance)
                    instance->DoCastSpellOnPlayers(SPELL_NECROTIC_AURA);

                activeEnergy = true;
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (spell->Id == SPELL_RECONGEALING_SEARCH)
            {
                DoCast(caster, SPELL_YUM);
                Talk(SAY_YUM);
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

            if (activeEnergy)
            {
                if (m_RegenTimer <= diff)
                {
                    if (me->GetPower(POWER_ENERGY) >= 100)
                    {
                        events.ScheduleEvent(EVENT_UH_OH, 0);
                        activeEnergy = false;
                    }
                    else
                        me->ModifyPower(POWER_ENERGY, 2);

                    m_RegenTimer = 1000;
                }
                else
                    m_RegenTimer -= diff;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CONGEALING_VOMIT:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(target, SPELL_CONGEALING_VOMIT);
                        Talk(SAY_VOMIT);
                        events.ScheduleEvent(EVENT_CONGEALING_VOMIT, 51000);
                        break;
                    case EVENT_UH_OH:
                        DoCast(me, SPELL_UH_OH);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_festerfaceAI (creature);
    }
};

//102158
class npc_festerface_congealed_goo : public CreatureScript
{
public:
    npc_festerface_congealed_goo() : CreatureScript("npc_festerface_congealed_goo") {}

    struct npc_festerface_congealed_gooAI : public ScriptedAI
    {
        npc_festerface_congealed_gooAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override
        {
            DoCast(me, SPELL_GREEN_SLIME_COSMETICS, true);
            DoCast(me, SPELL_FETID_STENCH, true);
            Talk(0); //warning
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, SpellInfo const* p_SpellInfo) override
        {
            if (damage >= me->GetHealth())
            {
                if (!me->HasAura(SPELL_RECONGEALING))
                {
                    DoCast(me, SPELL_RECONGEALING, true);
                }
                damage = 0;
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)  override
        {
            if (spell->Id == SPELL_YUM)
                me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_festerface_congealed_gooAI(creature);
    }
};

//201598
class spell_festerface_congealing_vomit : public SpellScriptLoader
{
    public:
        spell_festerface_congealing_vomit() : SpellScriptLoader("spell_festerface_congealing_vomit") {}

        class spell_festerface_congealing_vomit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_festerface_congealing_vomit_SpellScript);

            uint8 castCount;

            void HandleOnHit(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                castCount = GetSpellInfo()->Effects[EFFECT_1].CalcValue();

                Position pos;
                float dist = 0.0f;
                float angle = 0.0f;

                for(uint16 i = 0; i < 44; i++)
                {
                    angle = frand(-0.3f, 0.3f);
                    caster->GetNearPosition(pos, dist+i, angle);
                    caster->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_CONGEALING_VOMIT_VIS, true);
                }

                dist = 10.0f;

                for (uint8 i = 0; i < castCount; i++)
                {
                    angle = frand(-0.3f, 0.3f);
                    caster->GetNearPosition(pos, dist, angle);
                    caster->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_CONGEALING_VOMIT_SUM, true);

                    dist += 10.0f;
                }
            }

            void Register() override
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_festerface_congealing_vomit_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_festerface_congealing_vomit_SpellScript();
        }
};

//201495
class spell_festerface_recongealing : public SpellScriptLoader
{
    public:
        spell_festerface_recongealing() : SpellScriptLoader("spell_festerface_recongealing") { }

        class spell_festerface_recongealing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_festerface_recongealing_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->RemoveAurasDueToSpell(SPELL_FETID_STENCH);
                GetCaster()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                GetCaster()->SetHealth(GetCaster()->GetMaxHealth());
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_FETID_STENCH, true);
                GetCaster()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_festerface_recongealing_AuraScript::OnApply, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_festerface_recongealing_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_festerface_recongealing_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_festerface()
{
    new boss_festerface();
    new npc_festerface_congealed_goo();
    new spell_festerface_congealing_vomit();
    new spell_festerface_recongealing();
}
#endif
