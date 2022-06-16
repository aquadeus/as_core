////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "AreaTrigger.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "tomb_of_sargeras.h"
#include "InstanceScript.h"

/// NPC 118022 - Infernal Chaosbringer (Pre Gorroth)
class npc_tos_infernal_chaosbringer : public CreatureScript
{
    public:
        npc_tos_infernal_chaosbringer() : CreatureScript("npc_tos_infernal_chaosbringer") { }

        enum eSpells
        {
            SpellImmolationAura   = 242906,
            SpellMassiveEruption  = 242909,
            SpellCosmetic         = 242907
        };

        struct npc_tos_infernal_chaosbringerAI : public ScriptedAI
        {
            npc_tos_infernal_chaosbringerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventImmolationAura = 1,
                EventMassiveEruption,
                EventOneShotEmote
            };

            EventMap m_Events;
            uint32 timerOneShot;

            void Reset() override
            {
                m_Events.Reset();
                timerOneShot = 0;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->CastSpell(me, eSpells::SpellCosmetic, true);
                me->PlayOneShotAnimKitId(7856);
                timerOneShot = 4 * IN_MILLISECONDS;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventImmolationAura, 2 * IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventMassiveEruption, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    if (Creature* l_Goroth = p_instance->instance->GetCreature(p_instance->GetData64(eData::DataGoroth)))
                        if (l_Goroth->AI())
                            l_Goroth->AI()->DoAction(0);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (timerOneShot > 0)
                {
                    if (p_Diff >= timerOneShot)
                    {
                        timerOneShot = 0;
                        me->PlayOneShotAnimKitId(936);
                        me->SetHomePosition(*me);
                    }
                    else
                        timerOneShot -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventImmolationAura:
                            me->CastSpell((Unit*)nullptr, eSpells::SpellImmolationAura, true);
                            m_Events.ScheduleEvent(eEvents::EventImmolationAura, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventMassiveEruption:
                            me->CastSpell((Unit*)nullptr, eSpells::SpellMassiveEruption, false);
                            m_Events.ScheduleEvent(eEvents::EventMassiveEruption, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_infernal_chaosbringerAI(p_Creature);
        }
};

/// NPC 120516 - Razorjaw Swiftfin (Pre Harjatan)
class npc_tos_razorjaw_swiftfin : public CreatureScript
{
    public:
        npc_tos_razorjaw_swiftfin() : CreatureScript("npc_tos_razorjaw_swiftfin") { }

        enum eSpells
        {
            SpellAggressiveMrrgrlrgr  = 240540,
            SpellWildBite             = 240539
        };

        struct npc_tos_razorjaw_swiftfinAI : public ScriptedAI
        {
            npc_tos_razorjaw_swiftfinAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventAggressiveMrrgrlrgr = 1,
                EventWildBite
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
                me->DisableEvadeMode();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventAggressiveMrrgrlrgr, 1 * IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventWildBite, 4 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventAggressiveMrrgrlrgr:
                            DoCast(eSpells::SpellAggressiveMrrgrlrgr);
                            m_Events.ScheduleEvent(eEvents::EventAggressiveMrrgrlrgr, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventWildBite:
                            me->CastSpell((Unit*)nullptr, eSpells::SpellWildBite, false);
                            m_Events.ScheduleEvent(eEvents::EventWildBite, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_razorjaw_swiftfinAI(p_Creature);
        }
};

/// NPC 121004 - Razorjaw Myrmidon (Pre Harjatan)
class npc_tos_razorjaw_myrmidon : public CreatureScript
{
    public:
        npc_tos_razorjaw_myrmidon() : CreatureScript("npc_tos_razorjaw_myrmidon") { }

        enum eSpells
        {
            SpellEnraged  = 241240,
            SpellStab     = 241237
        };

        struct npc_tos_razorjaw_myrmidonAI : public ScriptedAI
        {
            npc_tos_razorjaw_myrmidonAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventStab = 1
            };

            EventMap m_Events;
            bool m_Enrage;

            void Reset() override
            {
                m_Events.Reset();
                m_Enrage = false;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Enrage = false;
                m_Events.ScheduleEvent(eEvents::EventStab, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    if (Creature* l_Sasszine = p_instance->instance->GetCreature(p_instance->GetData64(eData::DataNoBossMistressSasszine)))
                        if (l_Sasszine->AI())
                            l_Sasszine->AI()->DoAction(0);

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!m_Enrage && me->HealthBelowPct(50))
                {
                    DoCast(eSpells::SpellEnraged, true);
                    m_Enrage = true;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventStab:
                            DoCastVictim(eSpells::SpellStab);
                            m_Events.ScheduleEvent(eEvents::EventStab, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_razorjaw_myrmidonAI(p_Creature);
        }
};

/// NPC 117123 - Tidescale Legionnaire (Pre Harjatan)
class npc_tos_tidescale_legionnaire : public CreatureScript
{
    public:
        npc_tos_tidescale_legionnaire() : CreatureScript("npc_tos_tidescale_legionnaire") { }

        enum eSpells
        {
            SpellEnraged         = 247781,
            SpellGrandCleave     = 241179,
            SpellImpalingHarpoon = 241108
        };

        struct npc_tos_tidescale_legionnaireAI : public ScriptedAI
        {
            npc_tos_tidescale_legionnaireAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventImpalingHarpoon = 1,
                EventGrandCleave
            };

            EventMap m_Events;
            bool m_Enrage;

            void Reset() override
            {
                m_Events.Reset();
                m_Enrage = false;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Enrage = false;
                m_Events.ScheduleEvent(eEvents::EventImpalingHarpoon, urand(5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventGrandCleave, 2 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    if (Creature* l_Sasszine = p_instance->instance->GetCreature(p_instance->GetData64(eData::DataNoBossMistressSasszine)))
                        if (l_Sasszine->AI())
                            l_Sasszine->AI()->DoAction(0);

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!m_Enrage && me->HealthBelowPct(50))
                {
                    DoCast(eSpells::SpellEnraged, true);
                    m_Enrage = true;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventImpalingHarpoon:
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(l_Target, eSpells::SpellImpalingHarpoon);
                            m_Events.ScheduleEvent(eEvents::EventImpalingHarpoon, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventGrandCleave:
                            DoCastAOE(eSpells::SpellGrandCleave);
                            m_Events.ScheduleEvent(eEvents::EventGrandCleave, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_tidescale_legionnaireAI(p_Creature);
        }
};

/// NPC 121011 - Tidescale Witch (Pre Harjatan)
class npc_tos_tidescale_witch : public CreatureScript
{
    public:
        npc_tos_tidescale_witch() : CreatureScript("npc_tos_tidescale_witch") { }

        enum eSpells
        {
            SpellEnraged        = 247781,
            SpellArcBolt        = 241290,
            SpellShockingSquall = 241263
        };

        struct npc_tos_tidescale_witchAI : public ScriptedAI
        {
            npc_tos_tidescale_witchAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventArcBolt = 1,
                EventShockingSquall
            };

            EventMap m_Events;
            bool m_Enrage;

            void Reset() override
            {
                m_Events.Reset();
                m_Enrage = false;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Enrage = false;
                m_Events.ScheduleEvent(eEvents::EventArcBolt, 500);
                m_Events.ScheduleEvent(eEvents::EventShockingSquall, 10 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    if (Creature* l_Sasszine = p_instance->instance->GetCreature(p_instance->GetData64(eData::DataNoBossMistressSasszine)))
                        if (l_Sasszine->AI())
                            l_Sasszine->AI()->DoAction(0);

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!m_Enrage && me->HealthBelowPct(50))
                {
                    DoCast(eSpells::SpellEnraged, true);
                    m_Enrage = true;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventShockingSquall:
                            DoCast(eSpells::SpellShockingSquall);
                            m_Events.ScheduleEvent(eEvents::EventShockingSquall, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventArcBolt:
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(l_Target, eSpells::SpellArcBolt);
                            m_Events.ScheduleEvent(eEvents::EventArcBolt, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_tidescale_witchAI(p_Creature);
        }
};

/// NPC 117154 - Razorjaw Acolyte (Pre Harjatan)
class npc_tos_razorjaw_acolyte : public CreatureScript
{
    public:
        npc_tos_razorjaw_acolyte() : CreatureScript("npc_tos_razorjaw_acolyte") { }

        enum eSpells
        {
            SpellZap          = 241305,
            SpellLobLightning = 241306
        };

        struct npc_tos_razorjaw_acolyteAI : public ScriptedAI
        {
            npc_tos_razorjaw_acolyteAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventZap = 1,
                EventLobLightning
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventZap, 500);
                m_Events.ScheduleEvent(eEvents::EventLobLightning, 10 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    if (Creature* l_Sasszine = p_instance->instance->GetCreature(p_instance->GetData64(eData::DataNoBossMistressSasszine)))
                        if (l_Sasszine->AI())
                            l_Sasszine->AI()->DoAction(0);

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventLobLightning:
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                me->CastSpell(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ(), eSpells::SpellLobLightning, false);
                            m_Events.ScheduleEvent(eEvents::EventLobLightning, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventZap:
                            DoCastVictim(eSpells::SpellZap);
                            m_Events.ScheduleEvent(eEvents::EventZap, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_razorjaw_acolyteAI(p_Creature);
        }
};

/// NPC 120482 - Tidescale Seacaller (Pre Mistress Sasszine)
class npc_tos_tidescale_seacaller : public CreatureScript
{
    public:
        npc_tos_tidescale_seacaller() : CreatureScript("npc_tos_tidescale_seacaller") { }

        enum eSpells
        {
            SpellFrostBlast        = 240611,
            SpellEmbraceoftheTides = 240599
        };

        struct npc_tos_tidescale_seacallerAI : public ScriptedAI
        {
            npc_tos_tidescale_seacallerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventFrostBlast = 1,
                EventEmbraceoftheTides
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventFrostBlast, 500);
                m_Events.ScheduleEvent(eEvents::EventEmbraceoftheTides, 10 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventEmbraceoftheTides:
                            DoCastVictim(eSpells::SpellEmbraceoftheTides);
                            m_Events.ScheduleEvent(eEvents::EventEmbraceoftheTides, urand(25 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventFrostBlast:
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(l_Target, eSpells::SpellFrostBlast);
                            m_Events.ScheduleEvent(eEvents::EventFrostBlast, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_tidescale_seacallerAI(p_Creature);
        }
};

/// NPC 120477 - Deep Stalker (Pre Mistress Sasszine)
class npc_tos_deep_stalker : public CreatureScript
{
    public:
        npc_tos_deep_stalker() : CreatureScript("npc_tos_deep_stalker") { }

        enum eSpells
        {
            SpellDarkDepths     = 240273
        };

        struct npc_tos_deep_stalkerAI : public ScriptedAI
        {
            npc_tos_deep_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventDarkDepths = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventDarkDepths, 1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventDarkDepths:
                            DoCast(eSpells::SpellDarkDepths);
                            m_Events.ScheduleEvent(eEvents::EventDarkDepths, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_deep_stalkerAI(p_Creature);
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Dark Depths - 240273
class spell_tos_dark_depths : public SpellScriptLoader
{
    public:
        spell_tos_dark_depths() : SpellScriptLoader("spell_tos_dark_depths") { }

        enum eSpells
        {
            SpellJawsfromtheDeep     = 230276
        };

        class spell_tos_dark_depths_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tos_dark_depths_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                auto l_RandomItr = p_Targets.begin();
                std::advance(l_RandomItr, urand(0, uint32(p_Targets.size() - 1)));
                WorldObject* l_Target = *l_RandomItr;
                p_Targets.clear();
                p_Targets.push_back(l_Target);
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpellJawsfromtheDeep, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tos_dark_depths_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tos_dark_depths_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tos_dark_depths_SpellScript();
        }
};

/// NPC 120473 - Tidescale Combatant (Pre Mistress Sasszine)
class npc_tos_tidescale_combatant : public CreatureScript
{
    public:
        npc_tos_tidescale_combatant() : CreatureScript("npc_tos_tidescale_combatant") { }

        enum eSpells
        {
            SpellTremblingRoar   = 240577,
            SpellSerpentRush     = 240580
        };

        struct npc_tos_tidescale_combatantAI : public ScriptedAI
        {
            npc_tos_tidescale_combatantAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventTremblingRoar = 1,
                EventEmbraceoftheTides
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventTremblingRoar, 500);
                m_Events.ScheduleEvent(eEvents::EventEmbraceoftheTides, 10 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventEmbraceoftheTides:
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(l_Target, eSpells::SpellSerpentRush);
                            m_Events.ScheduleEvent(eEvents::EventEmbraceoftheTides, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventTremblingRoar:
                            DoCast(eSpells::SpellTremblingRoar);
                            m_Events.ScheduleEvent(eEvents::EventTremblingRoar, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_tidescale_combatantAI(p_Creature);
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Serpent Rush - 240580
class spell_tos_serpent_rush : public SpellScriptLoader
{
    public:
        spell_tos_serpent_rush() : SpellScriptLoader("spell_tos_serpent_rush") { }

        class spell_tos_serpent_rush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tos_serpent_rush_SpellScript);

            enum eSpells
            {
                SpellSerpentRushDamage = 240592
            };

            void HandleCharge(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SpellSerpentRushDamage, true); ///< AOE
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tos_serpent_rush_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tos_serpent_rush_SpellScript;
        }
};

/// NPC 120463 - Undersea Custodian (Pre Mistress Sasszine)
class npc_tos_undersea_custodian : public CreatureScript
{
    public:
        npc_tos_undersea_custodian() : CreatureScript("npc_tos_undersea_custodian") { }

        enum eSpells
        {
            SpellLightningStorm   = 240171,
            SpellElectricShock    = 240169
        };

        struct npc_tos_undersea_custodianAI : public ScriptedAI
        {
            npc_tos_undersea_custodianAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventLightningStorm = 1,
                EventElectricShock
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventLightningStorm, 500);
                m_Events.ScheduleEvent(eEvents::EventElectricShock, 10 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventElectricShock:
                            DoCast(eSpells::SpellElectricShock);
                            m_Events.ScheduleEvent(eEvents::EventElectricShock, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case eEvents::EventLightningStorm:
                            DoCast(eSpells::SpellLightningStorm);
                            m_Events.ScheduleEvent(eEvents::EventLightningStorm, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_undersea_custodianAI(p_Creature);
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Lightning Storm - 240171
class spell_tos_lighting_storm : public SpellScriptLoader
{
    public:
        spell_tos_lighting_storm() : SpellScriptLoader("spell_tos_lighting_storm") { }

        enum eSpells
        {
            SpellLightningStormAreaTrigger     = 240173
        };

        class spell_tos_lighting_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tos_lighting_storm_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (p_Targets.size() > 2)
                    JadeCore::RandomResizeList(p_Targets, 2);
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target->GetPosition(), eSpells::SpellLightningStormAreaTrigger, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tos_lighting_storm_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tos_lighting_storm_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tos_lighting_storm_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Lightning Storm - 240173
class spell_at_tos_lightning_storm : public AreaTriggerEntityScript
{
    public:
        spell_at_tos_lightning_storm() : AreaTriggerEntityScript("spell_at_tos_lightning_storm") {}

        enum eSpells
        {
            SpellLightningStormAreaDamage     = 240176
        };

        uint32 m_CheckTimer = 500;
        std::list<uint64> m_Targets;

        void OnUpdate(AreaTrigger* p_At, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;
                if (Unit* l_Caster = p_At->GetCaster())
                {
                    std::list<Unit*> l_TargetList;
                    float l_Radius = 7.5f; ///< radius from areaTrigger

                    std::list<uint64> l_Targets(m_Targets);
                    for (uint64 l_TargetGuid : l_Targets)
                    {
                        Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, l_TargetGuid);
                        if (!l_Target)
                            continue;

                        if (l_Target->HasAura(eSpells::SpellLightningStormAreaDamage) && l_Target->GetDistance(p_At) <= l_Radius)
                            continue;

                        if (l_Target->HasAura(eSpells::SpellLightningStormAreaDamage))
                            l_Target->RemoveAurasDueToSpell(eSpells::SpellLightningStormAreaDamage);

                        m_Targets.remove(l_TargetGuid);
                    }

                    JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_At, l_Caster, l_Radius);
                    JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_At, l_TargetList, l_Check);
                    p_At->VisitNearbyObject(l_Radius, l_Searcher);

                    if (l_TargetList.empty())
                        return;

                    for (Unit* l_Target : l_TargetList)
                    {
                        if (!l_Target->HasAura(eSpells::SpellLightningStormAreaDamage) && l_Caster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true))
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellLightningStormAreaDamage, true);
                            m_Targets.push_back(l_Target->GetGUID());
                        }
                    }
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (!p_AreaTrigger)
                 return;

            auto l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<uint64> l_Targets = m_Targets;
            for (uint64 l_TargetGuid : l_Targets)
            {
                Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, l_TargetGuid);
                if (!l_Target)
                    continue;

                if (l_Target->HasAura(eSpells::SpellLightningStormAreaDamage))
                    l_Target->RemoveAurasDueToSpell(eSpells::SpellLightningStormAreaDamage);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_tos_lightning_storm();
        }
};

/// last update : 7.3.5 Build 26365
/// Called by Vengeance of Elune 241238 and Wrath of Elune 241243 casted by NPC 'Worshiper of Elune' 120697
class spell_npc_buffs_of_elune : public SpellScriptLoader
{
public:
    spell_npc_buffs_of_elune() : SpellScriptLoader("spell_npc_buffs_of_elune") { }

    class spell_npc_buffs_of_elune_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_npc_buffs_of_elune_SpellScript);

        void SelectTargets(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            p_Targets.remove_if([l_Caster](WorldObject* l_Target) -> bool
            {
                if (!l_Target)
                    return true;

                Unit* l_TargetUnit = l_Target->ToUnit();
                if (!l_TargetUnit || l_TargetUnit->IsPlayer() || l_TargetUnit->isPet())
                    return true;

                return false;
            });
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_npc_buffs_of_elune_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_npc_buffs_of_elune_SpellScript();
    }
};

/// NPC 119728 - Prophet Velen
/// Intro Event Goroth
class npc_tos_prophet_velen : public CreatureScript
{
    public:
        npc_tos_prophet_velen() : CreatureScript("npc_tos_prophet_velen") { }

        enum eSpells
        {
            SpellAegisofLight     = 241186,
            SpellPlayScene        = 247931,
            SpellContingency      = 241303,
            SpellSummonInfernal   = 242915,
            SpellFakeDeath        = 35356
        };

        enum eEvents
        {
            EventSayKadgar = 1,
            EventSayIllidari,
            EventSayAtrigan,
            EventStartScene,
            EventCrashingComet,
            EventSayKadgar2,
            EventSummonInfernal,
            EventSummonInfernal2,
            EventSummonInfernal3,
            EventSayGoroth
        };

        enum eTexts
        {
            KadgarTalk1 = 0,
            IllidariTalk = 0,
            AtriganTalk = 0,
            KadgarTalk2 = 1,
            GorothTalk = 0,
            ///< After Defeat Goroth
            IllidaryDialog   = 1,
            AegwynnDialog    = 0,
            AegwynnDialog2   = 1,
            AegwynnDialog3   = 2,
            MageKadgarDialog = 2
        };

        struct npc_tos_prophet_velenAI : public ScriptedAI
        {
            npc_tos_prophet_velenAI(Creature* creature) : ScriptedAI(creature)
            {
                m_Instance = creature->GetInstanceScript();
            }

            EventMap m_Events;
            InstanceScript* m_Instance;
            std::list<Creature*> CreatureList;

            void Reset() override
            {
                m_Events.Reset();
                CreatureList.clear();
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
            }

            void StopAttackAndCombat(Creature* creature)
            {
                creature->CombatStop();
                creature->getThreatManager().clearReferences();
                creature->getThreatManager().resetAllAggro();
            }

            void StartSceneAllPlayerInInstance()
            {
                Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        l_Player->CastSpell(l_Player, eSpells::SpellPlayScene, true); /// need for complete quest
                        l_Player->PlayScene(1930, l_Player); /// from sniff
                    }
                }
            }

            void KillTrashCrashingComet()
            {
                if (!CreatureList.empty())
                {
                    for (std::list<Creature*>::iterator itr = CreatureList.begin(); itr != CreatureList.end(); itr++)
                    {
                        StopAttackAndCombat((*itr));
                        (*itr)->SetFlag(EObjectFields::OBJECT_FIELD_DYNAMIC_FLAGS, UnitDynFlags::UNIT_DYNFLAG_DEAD);
                        (*itr)->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);
                        (*itr)->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    }
                }
                CreatureList.clear();
            }

            void GameObjectActivateOrDelete(uint32 entry, bool active)
            {
                if (GameObject* l_CrashingComet = me->FindNearestGameObject(entry, 250.0f))
                {
                    if (active)
                    {
                        l_CrashingComet->SetLootState(LootState::GO_READY);
                        l_CrashingComet->UseDoorOrButton(20 * IN_MILLISECONDS);
                    }
                    else
                        l_CrashingComet->Delete();
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_Id) override
            {
                if (m_Instance)
                {
                    if (p_Id == 1)
                    {
                        DelayTalk(57, eTexts::MageKadgarDialog);
                        if (Creature* l_Illidari = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataIllidari)))
                            if (l_Illidari->AI())
                                l_Illidari->AI()->Talk(eTexts::IllidaryDialog);

                        if (Creature* Aegwynn = me->FindNearestCreature(eCreatures::NpcImageOfAegwynn, 100.0f))
                        {
                            Aegwynn->CastSpell(Aegwynn, 241351, true);
                            if (Aegwynn->AI())
                            {
                                Aegwynn->AI()->DelayTalk(7, eTexts::AegwynnDialog);
                                Aegwynn->AI()->DelayTalk(23, eTexts::AegwynnDialog2);
                                Aegwynn->AI()->DelayTalk(40, eTexts::AegwynnDialog3);
                            }
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->SelectNearestPlayerNotGM(200.0f) && m_Instance && m_Instance->GetData(eData::DataPreEventGoroth) == NOT_STARTED && (m_Instance->GetData(eData::DataDungeonID) == eDungeonIDs::DungeonIDNone || m_Instance->GetData(eData::DataDungeonID) == eDungeonIDs::DungeonIDTheGatesofHell))
                {
                    if (Creature* l_Goroth = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataGoroth)))
                    {
                       l_Goroth->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                       l_Goroth->SetVisible(false);
                    }

                    me->RemoveAura(eSpells::SpellAegisofLight);
                    me->CastSpell(me, eSpells::SpellAegisofLight, true);
                    m_Instance->SetData(eData::DataPreEventGoroth, IN_PROGRESS);
                    ///< clear container
                    CreatureList.clear();
                    uint32 const l_Entries[4] = { eCreatures::NpcLegionfallSoldierHorde, eCreatures::NpcLegionfallSoldierAlliance, eCreatures::NpcFelguardInvader, eCreatures::NpcFelguardInvader2 };
                    for (uint8 i = 0; i < 4; i++)
                        me->GetCreatureListWithEntryInGrid(CreatureList, l_Entries[i], 100.0f);

                    if (!CreatureList.empty())
                    {
                        for (std::list<Creature*>::iterator itr = CreatureList.begin(); itr != CreatureList.end(); itr++)
                        {
                            (*itr)->RemoveAura(eSpells::SpellFakeDeath);
                            (*itr)->RemoveFlag(EObjectFields::OBJECT_FIELD_DYNAMIC_FLAGS, UnitDynFlags::UNIT_DYNFLAG_DEAD);
                            (*itr)->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);
                            (*itr)->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            if (Unit* target = (*itr)->SelectNearestTargetInAttackDistance(6.f))
                                if ((*itr)->AI())
                                    (*itr)->AI()->AttackStart(target);
                        }
                    }

                    uint32 const l_Datas[3] = { eData::DataMageKadgar, eData::DataIllidari, eData::DataMaievShadowsong };
                    for (uint8 j = 0; j < 3; ++j)
                    {
                        if (Creature* l_Creature = m_Instance->instance->GetCreature(m_Instance->GetData64(l_Datas[j])))
                        {
                            float m_attackRadius = (l_Datas[j] == eData::DataIllidari ? 18.f : 6.f);
                            if (Unit* target = l_Creature->SelectNearestTargetInAttackDistance(m_attackRadius))
                                if (l_Creature->AI())
                                    l_Creature->AI()->AttackStart(target);
                        }
                    }

                    m_Events.ScheduleEvent(eEvents::EventSayKadgar, 6 * IN_MILLISECONDS);
                }

                if (!m_Instance || m_Instance->GetData(eData::DataPreEventGoroth) == NOT_STARTED || m_Instance->GetData(eData::DataPreEventGoroth) == DONE)
                    return;

                m_Events.Update(p_Diff);

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventSayKadgar:
                        {
                            if (Creature* l_Kadgar = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataMageKadgar)))
                                if (l_Kadgar->AI())
                                    l_Kadgar->AI()->Talk(eTexts::KadgarTalk1);
                            m_Events.ScheduleEvent(eEvents::EventSayIllidari, 5 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSayIllidari:
                        {
                            if (Creature* l_Illidari = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataIllidari)))
                                if (l_Illidari->AI())
                                    l_Illidari->AI()->Talk(eTexts::IllidariTalk);
                            m_Events.ScheduleEvent(eEvents::EventSayAtrigan, 9 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSayAtrigan:
                        {
                            if (Creature* l_Atrigan = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataNoBossAtrigan)))
                                if (l_Atrigan->AI())
                                    l_Atrigan->AI()->Talk(eTexts::AtriganTalk);
                            m_Events.ScheduleEvent(eEvents::EventStartScene, 8 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventStartScene:
                        {
                            if (Creature* l_Atrigan = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataNoBossAtrigan)))
                                l_Atrigan->SetVisible(false);

                            StartSceneAllPlayerInInstance();
                            m_Events.ScheduleEvent(eEvents::EventCrashingComet, 2 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventCrashingComet:
                        {
                            GameObjectActivateOrDelete(eGameObjects::GoCrashingComet, true);
                            m_Events.ScheduleEvent(eEvents::EventSayKadgar2, 2 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSayKadgar2:
                        {
                            GameObjectActivateOrDelete(eGameObjects::GoCrashingComet2, true);

                            KillTrashCrashingComet();
                            if (Creature* l_Kadgar = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataMageKadgar)))
                            {
                                if (l_Kadgar->AI())
                                {
                                    StopAttackAndCombat(l_Kadgar);
                                    l_Kadgar->AI()->Talk(eTexts::KadgarTalk2);
                                    me->RemoveAura(eSpells::SpellAegisofLight);
                                    l_Kadgar->RemoveAurasDueToSpell(235453);
                                    l_Kadgar->CastSpell(PositionTeleKadgar[0], SpellContingency, true);
                                }
                            }
                            if (Creature* l_Illidari = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataIllidari)))
                            {
                                StopAttackAndCombat(l_Illidari);
                                l_Illidari->CastSpell(PositionTeleIllidari[0], eSpells::SpellContingency, true);
                            }

                            if (Creature* l_Maiev = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataMaievShadowsong)))
                            {
                                StopAttackAndCombat(l_Maiev);
                                l_Maiev->CastSpell(PositionTeleMaiev[0], eSpells::SpellContingency, true);
                            }

                            StopAttackAndCombat(me);
                            me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                            me->CastSpell(PositionTeleProphetVelen[0], eSpells::SpellContingency, true);
                            m_Events.ScheduleEvent(eEvents::EventSummonInfernal, 5 * IN_MILLISECONDS);
                            m_Events.ScheduleEvent(eEvents::EventSummonInfernal2, 6 * IN_MILLISECONDS);
                            m_Events.ScheduleEvent(eEvents::EventSummonInfernal3, 7 * IN_MILLISECONDS);
                            if (Creature* l_Goroth = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataGoroth)))
                                l_Goroth->SetVisible(true);

                            break;
                        }
                        case eEvents::EventSummonInfernal:
                        {
                            GameObjectActivateOrDelete(eGameObjects::GoCrashingComet, false);
                            GameObjectActivateOrDelete(eGameObjects::GoCrashingComet2, false);
                            if (Creature* l_Atrigan = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataNoBossAtrigan)))
                                l_Atrigan->SetVisible(false);
                            Position const PositionSummonInfernal = { 6130.221f, -801.1302f, 2971.544f, 2.807055f };
                            if (Creature* l_Breach = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataBreach)))
                                l_Breach->CastSpell(PositionSummonInfernal, eSpells::SpellSummonInfernal, true);
                            break;
                        }
                        case eEvents::EventSummonInfernal2:
                        {
                            Position const PositionSummonInfernal = { 6086.503f, -816.4011f, 2971.721f, 2.965497f };
                            if (Creature* l_Breach = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataBreach)))
                                l_Breach->CastSpell(PositionSummonInfernal, eSpells::SpellSummonInfernal, true);
                            break;
                        }
                        case eEvents::EventSummonInfernal3:
                        {
                            Position const PositionSummonInfernal = { 6064.199f, -782.8004f, 2971.544f, 2.827615f };
                            if (Creature* l_Breach = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataBreach)))
                                l_Breach->CastSpell(PositionSummonInfernal, eSpells::SpellSummonInfernal, true);

                            m_Events.ScheduleEvent(eEvents::EventSayGoroth, 10 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSayGoroth:
                        {
                            if (Creature* l_Goroth = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataGoroth)))
                                if (l_Goroth->AI())
                                    l_Goroth->AI()->Talk(eTexts::GorothTalk);
                            m_Instance->SetData(eData::DataPreEventGoroth, DONE);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tos_prophet_velenAI(creature);
        }
};

/// NPC 121184 - Mistress Sasszine
/// Intro Event Harjatan
class npc_tos_mistress_sasszine : public CreatureScript
{
    public:
        npc_tos_mistress_sasszine() : CreatureScript("npc_tos_mistress_sasszine") { }

        enum eTexts
        {
             TALK_INTRO      = 0,
             TALK_INTRO2,
             TALK_KILL_FIST_PACK,
             TALK_KILL_ALL_PACKS,
             TALK_DISSAPEAR,
             TALK_HARJATAN_INTRO = 0
        };

        enum eEvents
        {
            EventSayIntro2 = 1,
            EventSayHarjatan,
            EventSayDissapear,
            EventCompletePreEventHarjatan
        };

        struct npc_tos_mistress_sasszineAI : public ScriptedAI
        {
            npc_tos_mistress_sasszineAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
               m_count = 0;
               m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;
            uint32 m_count;
            EventMap m_Events;
            bool l_Progress = false;

            void Reset() override
            {
                m_Events.Reset();
            }

            void DoAction(int32 const p_Action) override
            {
                if (!m_Instance)
                    return;

                ++m_count;
                if (m_count == 12 && m_Instance && m_Instance->GetData(eData::DataPreEventHarjatan) == IN_PROGRESS)
                {
                   Talk(eTexts::TALK_KILL_ALL_PACKS);
                   m_Events.ScheduleEvent(eEvents::EventSayHarjatan, 7 * IN_MILLISECONDS);
                }
                else if (m_count == 1 && m_Instance && m_Instance->GetData(eData::DataPreEventHarjatan) == IN_PROGRESS)
                   Talk(eTexts::TALK_KILL_FIST_PACK);

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->SelectNearestPlayerNotGM(80.0f) && m_Instance && m_Instance->GetData(eData::DataPreEventHarjatan) == NOT_STARTED)
                {
                    l_Progress = true;
                    m_Instance->SetData(eData::DataPreEventHarjatan, IN_PROGRESS);
                    Talk(eTexts::TALK_INTRO);
                    m_Events.ScheduleEvent(eEvents::EventSayIntro2, 10 * IN_MILLISECONDS);

                    if (Creature* l_Harjatan = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataHarjatan)))
                        l_Harjatan->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }

                if (!m_Instance || !l_Progress)
                    return;

                m_Events.Update(p_Diff);

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventSayIntro2:
                        {
                            Talk(eTexts::TALK_INTRO2);
                            break;
                        }
                        case eEvents::EventSayHarjatan:
                        {
                            if (Creature* l_Harjatan = m_Instance->instance->GetCreature(m_Instance->GetData64(eData::DataHarjatan)))
                            {
                                m_Instance->SetData(eData::DataPreEventHarjatan, DONE);
                                l_Harjatan->HandleEmoteCommand(EMOTE_ONESHOT_BATTLEROAR);
                                if (l_Harjatan->AI())
                                    l_Harjatan->AI()->Talk(eTexts::TALK_HARJATAN_INTRO);

                                l_Harjatan->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            }
                            m_Events.ScheduleEvent(eEvents::EventSayDissapear, 8 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSayDissapear:
                        {
                            Talk(eTexts::TALK_DISSAPEAR);
                            m_Events.ScheduleEvent(eEvents::EventCompletePreEventHarjatan, 10 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventCompletePreEventHarjatan:
                        {
                            l_Progress = false;
                            me->SetVisible(false);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_mistress_sasszineAI(p_Creature);
        }
};

/// NPC 121399 - Inquisitor Sebilus(Pre Demonic Inquisition)
class npc_tos_inquisitor_sebilus : public CreatureScript
{
    public:
        npc_tos_inquisitor_sebilus() : CreatureScript("npc_tos_inquisitor_sebilus") { }

        enum eSpells
        {
            SpellMindshear  = 243289
        };

        struct npc_tos_inquisitor_sebilusAI : public ScriptedAI
        {
            npc_tos_inquisitor_sebilusAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventMindshear = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventMindshear, 1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventMindshear:
                        {
                            DoCast(eSpells::SpellMindshear);
                            m_Events.ScheduleEvent(eEvents::EventMindshear, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_inquisitor_sebilusAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by MindShear- 243289
class spell_tos_mindshear : public SpellScriptLoader
{
    public:
        spell_tos_mindshear() : SpellScriptLoader("spell_tos_mindshear") { }

        class spell_tos_mindshear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tos_mindshear_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(CheckDeadTarget());

                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tos_mindshear_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_tos_mindshear_SpellScript();
        }
};

/// NPC 121473 - Coercitor Nixa(Pre Demonic Inquisition)
class npc_tos_coercitor_nixa : public CreatureScript
{
    public:
        npc_tos_coercitor_nixa() : CreatureScript("npc_tos_coercitor_nixa") { }

        enum eSpells
        {
            SpellLashofDomination  = 243298
        };

        struct npc_tos_coercitor_nixaAI : public ScriptedAI
        {
            npc_tos_coercitor_nixaAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum eEvents
            {
                EventLashofDomination = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventLashofDomination, 4 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventLashofDomination:
                        {
                            DoCast(eSpells::SpellLashofDomination);
                            m_Events.ScheduleEvent(eEvents::EventLashofDomination, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_coercitor_nixaAI(p_Creature);
        }
};

///////////////////////////////////////////////////////////////////////////////////////
/// Pillar of Creations
///////////////////////////////////////////////////////////////////////////////////////

/// Hammer of Khaz'goroth - 121111
/// Tears of Elune - 121110
/// Tidestone of Golganeth - 121105
class npc_tos_pillar_of_creation : public CreatureScript
{
    public:
        npc_tos_pillar_of_creation() : CreatureScript("npc_tos_pillar_of_creation")
        {}

        enum eSpells
        {
            SpellTearsOfEluneConversation   =   240057,
            SpellHammerConversation         =   240053,
            SpellTideStoneConversation      =   240056,
            SpellAllPillarsPlaced           =   240058,
        };

        struct npc_tos_pillar_of_creation_AI : public ScriptedAI
        {
            explicit npc_tos_pillar_of_creation_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void sGossipHello(Player* p_Player) override
            {
                if (p_Player == nullptr)
                    return;

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->RemoveAllAuras();

                switch (me->GetEntry())
                {
                    case eCreatures::NpcTearsOfElune:
                    {
                        p_Player->CastSpell(p_Player, eSpells::SpellTearsOfEluneConversation, true);
                        break;
                    }

                    case eCreatures::NpcHammerOfKhazgoroth:
                    {
                        p_Player->CastSpell(p_Player, eSpells::SpellHammerConversation, true);
                        break;
                    }

                    case eCreatures::NpcTidestoneofGolganneth:
                    {
                        p_Player->CastSpell(p_Player, eSpells::SpellTideStoneConversation, true);
                        break;
                    }

                    default: break;
                }

                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (l_Instance->GetBossState(eData::DataDesolateHost) == EncounterState::DONE &&
                    l_Instance->GetBossState(eData::DataDemonicInquisition) == EncounterState::DONE &&
                    l_Instance->GetBossState(eData::DataMistressSasszine) == EncounterState::DONE)
                {
                    p_Player->DelayedCastSpell(p_Player, eSpells::SpellAllPillarsPlaced, true, 30 * IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tos_pillar_of_creation_AI(p_Me);
        }
};

/// Image of Aegwynn - 119723
class npc_tos_image_of_aegwynn : public CreatureScript
{
    public:
        npc_tos_image_of_aegwynn() : CreatureScript("npc_tos_image_of_aegwynn")
        {}

        enum eTalks
        {
            TalkIntroMaiden = 3,
        };

        struct npc_tos_image_of_aegwynn_AI : public ScriptedAI
        {
            explicit npc_tos_image_of_aegwynn_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_Intro = false;
            }

            void StartReply(uint32 const p_Entry, uint8 p_TalkId, uint32 p_Time)
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;
                
                Creature* l_Creature = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(p_Entry));

                if (l_Creature == nullptr)
                    return;

                if (l_Creature->IsAIEnabled)
                    l_Creature->AI()->DelayTalk(p_Time, p_TalkId);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who == nullptr || m_Intro)
                    return;

                if (!p_Who->IsPlayer() || m_Intro)
                    return;

                if (p_Who->GetDistance2d(me) >= 30.f)
                    return;

                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (l_Instance->GetBossState(eData::DataDesolateHost) == EncounterState::DONE &&
                    l_Instance->GetBossState(eData::DataDemonicInquisition) == EncounterState::DONE &&
                    l_Instance->GetBossState(eData::DataMistressSasszine) == EncounterState::DONE)
                {
                    m_Intro = true;
                    DelayTalk(3, eTalks::TalkIntroMaiden);

                    StartReply(eData::DataMageKadgar, 3, 22);
                    StartReply(eData::DataIllidari, 2, 32);
                    StartReply(eData::DataMageKadgar, 4, 45);
                    StartReply(eData::DataProphetVelen, 10, 70);

                    l_Instance->HandleGameObject(l_Instance->GetData64(eGameObjects::GoDoorBeforeMaidenRoom), true, nullptr);
                }
            }

            bool m_Intro;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tos_image_of_aegwynn_AI(p_Me);
        }
};

///< Illidan Stormrage - 119729
///< Archmage Khadgar - 119726
class npc_tos_allied_npc : public CreatureScript
{
    public:
        npc_tos_allied_npc() : CreatureScript("npc_tos_allied_npc")
        {}

        struct npc_tos_allied_npc_AI : public ScriptedAI
        {
            explicit npc_tos_allied_npc_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tos_allied_npc_AI(p_Me);
        }
};

///////////////////////////////////////////////////////////////////////////////////////
/// Maiden of Viligance's Trashes
///////////////////////////////////////////////////////////////////////////////////////

/// Eonic Defender - 120153
class npc_tos_eonic_defender : public CreatureScript
{
    public:
        npc_tos_eonic_defender() : CreatureScript("npc_tos_eonic_defender") { }

        enum eSpells
        {
            SpellFelCorruptionChannel   = 239461,
            SpellObliteratingSmash      = 239439
        };

        enum eEvent
        {
            EventObliteratingSmash = 1
        };

        struct npc_tos_eonic_defenderAI : public ScriptedAI
        {
            npc_tos_eonic_defenderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::set<uint64> m_CorruptionGuids;

            void EnterCombat(Unit* p_Attacker) override
            {
                for (uint64 const& l_Guid : m_CorruptionGuids)
                {
                    if (Creature* l_Corruption = Creature::GetCreature(*me, l_Guid))
                    {
                        if (l_Corruption->isAlive() && !l_Corruption->isInCombat() && l_Corruption->IsAIEnabled)
                            l_Corruption->AI()->AttackStart(p_Attacker);
                    }
                }

                events.ScheduleEvent(eEvent::EventObliteratingSmash, 4 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFelCorruptionChannel:
                    {
                        m_CorruptionGuids.insert(p_Caster->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventObliteratingSmash:
                    {
                        DoCast(me, eSpells::SpellObliteratingSmash);

                        events.ScheduleEvent(eEvent::EventObliteratingSmash, 13 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tos_eonic_defenderAI(p_Creature);
        }
};

/// Volatile Corruption - 120158
class npc_tos_volatile_corruption : public CreatureScript
{
    public:
        npc_tos_volatile_corruption() : CreatureScript("npc_tos_volatile_corruption") { }

        enum eSpells
        {
            SpellFelCorruptionChannel   = 239461,

            SpellChaoticEruptionSearch  = 241005,
            SpellChaoticEruptionMissile = 241004
        };

        enum eEvent
        {
            EventChaoticEruption = 1
        };

        struct npc_tos_volatile_corruptionAI : public ScriptedAI
        {
            npc_tos_volatile_corruptionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_DefenderGuid = 0;

            void Reset() override
            {
                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eCreatures::NpcEonicDefender, 10.0f))
                    {
                        if (!l_Creature->isAlive())
                            return;

                        m_DefenderGuid = l_Creature->GetGUID();

                        DoCast(l_Creature, eSpells::SpellFelCorruptionChannel, true);

                        l_Creature->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
                    }
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->InterruptNonMeleeSpells(true);

                if (m_DefenderGuid)
                {
                    if (Creature* l_Defender = Creature::GetCreature(*me, m_DefenderGuid))
                    {
                        if (l_Defender->isAlive() && !l_Defender->isInCombat() && l_Defender->IsAIEnabled)
                            l_Defender->AI()->AttackStart(p_Attacker);
                    }
                }

                events.ScheduleEvent(eEvent::EventChaoticEruption, urand(7, 10) * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellChaoticEruptionSearch)
                    DoCast(p_Target, eSpells::SpellChaoticEruptionMissile);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventChaoticEruption:
                    {
                        DoCast(me, eSpells::SpellChaoticEruptionSearch, true);

                        events.ScheduleEvent(eEvent::EventChaoticEruption, urand(7, 10) * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tos_volatile_corruptionAI(p_Creature);
        }
};

///////////////////////////////////////////////////////////////////////////////////////
/// Kil'jaeden's Trashes
///////////////////////////////////////////////////////////////////////////////////////

/// Fleetlord Dominator - 121790
class npc_tos_fleetlord_dominator : public CreatureScript
{
    public:
        npc_tos_fleetlord_dominator() : CreatureScript("npc_tos_fleetlord_dominator") { }

        struct npc_tos_fleetlord_dominatorAI : public ScriptedAI
        {
            npc_tos_fleetlord_dominatorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_KJ = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden)))
                    {
                        if (l_KJ->IsAIEnabled)
                            l_KJ->AI()->DoAction(0);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_fleetlord_dominatorAI(p_Creature);
        }
};

/// Shadow Council Warlock - 117776
class npc_tos_shadow_council_warlock : public CreatureScript
{
    public:
        npc_tos_shadow_council_warlock() : CreatureScript("npc_tos_shadow_council_warlock") { }

        enum eSpells
        {
            SpellChaosBolt  = 243300,
            SpellAgony      = 243299
        };

        enum eEvents
        {
            EventChaosBolt = 1,
            EventAgony
        };

        struct npc_tos_shadow_council_warlockAI : public ScriptedAI
        {
            npc_tos_shadow_council_warlockAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.Reset();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_KJ = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden)))
                    {
                        if (l_KJ->IsAIEnabled)
                            l_KJ->AI()->DoAction(0);
                    }
                }

                events.ScheduleEvent(eEvents::EventChaosBolt, 1 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventAgony, 6 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventChaosBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(l_Target, eSpells::SpellChaosBolt);

                        events.ScheduleEvent(eEvents::EventChaosBolt, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAgony:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(l_Target, eSpells::SpellAgony);

                        events.ScheduleEvent(eEvents::EventAgony, 8 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tos_shadow_council_warlockAI(p_Creature);
        }
};

/// Eredar Bloodmage - 117782
class npc_tos_eredar_bloodmage : public CreatureScript
{
    public:
        npc_tos_eredar_bloodmage() : CreatureScript("npc_tos_eredar_bloodmage") { }

        enum eSpell
        {
            SpellBloodSiphon = 235222
        };

        enum eEvent
        {
            EventBloodSiphon = 1
        };

        struct npc_tos_eredar_bloodmageAI : public ScriptedAI
        {
            npc_tos_eredar_bloodmageAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_KJ = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden)))
                    {
                        if (l_KJ->IsAIEnabled)
                            l_KJ->AI()->DoAction(0);
                    }
                }

                events.Reset();

                events.ScheduleEvent(eEvent::EventBloodSiphon, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventBloodSiphon:
                    {
                        DoCast(me, eSpell::SpellBloodSiphon);

                        events.ScheduleEvent(eEvent::EventBloodSiphon, 9 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tos_eredar_bloodmageAI(p_Creature);
        }
};

/// Ixallon the Soulbreaker - 118930
class npc_tos_ixallon_the_soulbreaker : public CreatureScript
{
    public:
        npc_tos_ixallon_the_soulbreaker() : CreatureScript("npc_tos_ixallon_the_soulbreaker") { }

        enum eSpells
        {
            SpellFelBlastDummy  = 239635,
            SpellFelBlastDamage = 239636
        };

        enum eEvent
        {
            EventFelBlast = 1
        };

        struct npc_tos_ixallon_the_soulbreakerAI : public ScriptedAI
        {
            npc_tos_ixallon_the_soulbreakerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_KJ = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden)))
                    {
                        if (l_KJ->IsAIEnabled)
                            l_KJ->AI()->DoAction(0);
                    }
                }

                events.Reset();

                events.ScheduleEvent(eEvent::EventFelBlast, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFelBlastDummy)
                {
                    DoCast(me, eSpells::SpellFelBlastDamage, true);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventFelBlast:
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellFelBlastDummy);
                        });

                        events.ScheduleEvent(eEvent::EventFelBlast, 20 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tos_ixallon_the_soulbreakerAI(p_Creature);
        }
};

/// Blood Siphon - 235222
class spell_tos_blood_siphon : public SpellScriptLoader
{
    public:
        spell_tos_blood_siphon() : SpellScriptLoader("spell_tos_blood_siphon") { }

        enum eSpells
        {
            SpellBloodSiphonDamage  = 235232,
            SpellBloodSiphonHeal    = 235262
        };

        class spell_tos_blood_siphon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tos_blood_siphon_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        SpellInfo const* l_DamageInfo = sSpellMgr->GetSpellInfo(eSpells::SpellBloodSiphonDamage, l_Caster->GetMap()->GetDifficultyID());
                        if (!l_DamageInfo)
                            return;

                        int32 l_Damage = l_Target->CountPctFromCurHealth(l_DamageInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints);

                        l_Caster->CastCustomSpell(l_Target, eSpells::SpellBloodSiphonDamage, &l_Damage, nullptr, nullptr, true);
                        l_Target->CastCustomSpell(l_Target, eSpells::SpellBloodSiphonHeal, &l_Damage, nullptr, nullptr, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tos_blood_siphon_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tos_blood_siphon_AuraScript();
        }
};

/// Blood Siphon (Heal) - 235262
class spell_tos_blood_siphon_heal : public SpellScriptLoader
{
    public:
        spell_tos_blood_siphon_heal() : SpellScriptLoader("spell_tos_blood_siphon_heal") { }

        class spell_tos_blood_siphon_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tos_blood_siphon_heal_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    return !p_Object->ToUnit() || !p_Object->ToUnit()->isInCombat();
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tos_blood_siphon_heal_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tos_blood_siphon_heal_SpellScript();
        }
};

///< 269840, 269841
class go_sisters_moon_control : public GameObjectScript
{
    public:
        go_sisters_moon_control() : GameObjectScript("go_sisters_moon_control") { }

        struct go_sisters_moon_controlAI : public GameObjectAI
        {
            go_sisters_moon_controlAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject), m_Activated(false) { }

            bool m_Activated;
            bool GossipHello(Player* p_Player) override
            {
                if (!m_Activated)
                {
                    m_Activated = true;
                    if (InstanceScript* p_instance = p_Player->GetInstanceScript())
                        if (GameObject* l_DoorPreSistersOfTheMoon3 = p_instance->instance->GetGameObject(p_instance->GetData64(eGameObjects::GoDoorPreSistersOfTheMoon3)))
                            l_DoorPreSistersOfTheMoon3->SetGoState(GO_STATE_ACTIVE);
                }

                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_sisters_moon_controlAI(p_GameObject);
        }
};

#ifndef __clang_analyzer__
void AddSC_tomb_of_sargeras()
{
    /// Creatures
    new npc_tos_infernal_chaosbringer();
    new npc_tos_razorjaw_swiftfin();
    new npc_tos_razorjaw_myrmidon();
    new npc_tos_tidescale_legionnaire();
    new npc_tos_tidescale_witch();
    new npc_tos_razorjaw_acolyte();
    new npc_tos_tidescale_seacaller();
    new npc_tos_deep_stalker();
    new npc_tos_tidescale_combatant();
    new npc_tos_undersea_custodian();
    new npc_tos_prophet_velen();
    new npc_tos_mistress_sasszine();
    new npc_tos_inquisitor_sebilus();
    new npc_tos_coercitor_nixa();
    new npc_tos_eonic_defender();
    new npc_tos_volatile_corruption();
    new npc_tos_fleetlord_dominator();
    new npc_tos_shadow_council_warlock();
    new npc_tos_eredar_bloodmage();
    new npc_tos_ixallon_the_soulbreaker();
    new npc_tos_pillar_of_creation();
    new npc_tos_image_of_aegwynn();
    new npc_tos_allied_npc();

    /// Spells
    new spell_tos_dark_depths();
    new spell_tos_serpent_rush();
    new spell_tos_lighting_storm();
    new spell_tos_mindshear();
    new spell_npc_buffs_of_elune();
    new spell_tos_blood_siphon();
    new spell_tos_blood_siphon_heal();

    /// AreaTriggers
    new spell_at_tos_lightning_storm();
    
    ///< Gameobject
    new go_sisters_moon_control();
}
#endif
