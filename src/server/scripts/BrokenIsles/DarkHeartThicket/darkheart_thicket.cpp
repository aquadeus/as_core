#include "darkheart_thicket.hpp"

/// Dreadsoul Poisoner - 101679
class npc_dreadsoul_poisoner : public CreatureScript
{
    public:
        npc_dreadsoul_poisoner() : CreatureScript("npc_dreadsoul_poisoner")
        {}

        enum eSpells
        {
            SpellSpiderForm = 200666,
            SpellNightmareToxin = 200684,
        };

        enum eEvents
        {
            EventNightmareToxin = 1,
            EventSpiderForm,
        };

        struct npc_dreadsoul_poisoner_AI : public ScriptedAI
        {
            explicit npc_dreadsoul_poisoner_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                me->RemoveAllAuras();
                SetEquipmentSlots(true);
                me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
            }

            void EnterCombat(Unit* /**/) override
            {
                SetEquipmentSlots(true);
                me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);

                DoCast(me, eSpells::SpellSpiderForm, false);
            }

            void OwnerInterrupted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellSpiderForm)
                    events.ScheduleEvent(eEvents::EventSpiderForm, 100);
            }

            void OnSpellFinished(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellSpiderForm)
                {
                    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                    me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
                    events.RescheduleEvent(eEvents::EventNightmareToxin, urand(8, 12) * IN_MILLISECONDS);
                }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventNightmareToxin:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellNightmareToxin, true);

                        events.ScheduleEvent(eEvents::EventNightmareToxin, urand(10, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSpiderForm:
                    {
                        DoCast(me, eSpells::SpellSpiderForm);
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                ExecuteEvent(events.ExecuteEvent());

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_dreadsoul_poisoner_AI(p_Me);
        }
};

/// Fixate - 198477
class spell_dht_fixate : public SpellScriptLoader
{
    public:
        spell_dht_fixate() : SpellScriptLoader("spell_dht_fixate")
        {}

        enum eSpells
        {
            SpellSpewCorruptionAT  = 218758
        };

        class spell_dht_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dht_fixate_AuraScript);

            bool Load() override
            {
                m_Timer = 0;
                return true;
            }

            void HandleOnUpdate(uint32 const p_Diff)
            {
                if (!GetCaster())
                    return;

                m_Timer += p_Diff;

                if (m_Timer >= IN_MILLISECONDS)
                {
                    m_Timer = 0;
                    GetCaster()->CastSpell(GetCaster(), eSpells::SpellSpewCorruptionAT, true);
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_dht_fixate_AuraScript::HandleOnUpdate);
            }

            uint32 m_Timer;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dht_fixate_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_darkheart_thicket()
{
    /// Creatures
    new npc_dreadsoul_poisoner();

    /// Spells
    new spell_dht_fixate();
}
#endif