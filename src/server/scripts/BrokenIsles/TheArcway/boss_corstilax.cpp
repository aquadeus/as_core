////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"

enum eSpells
{
    SuppressionProtocol         = 196047,
    SuppressionProtocolFixate   = 196068,
    SuppressionProtocalDamage   = 196070,
    Quarantine                  = 195791,
    QuarantineStun              = 195804,
    QuarantineScript            = 195846,
    QuarantineRideVehicle       = 195820,
    ShatterQuarantine           = 195826,
    CleansingForce              = 196115,
    CleansingForceAreatrigger   = 196088,
    DestabilizedOrb             = 220481,
    DestabilizedOrbAreatrigger  = 220482,
    EnergyBurstVisual           = 195516,
    EnergyBurstAreatrigger      = 195523,
    NightwellEnergy             = 195362,
    SpawnVisual                 = 217725
};

enum eEvents
{
    EventProtocol           = 1,
    EventQuarantine         = 2,
    EventCleansingForce     = 3,
    EventEnergyBurst        = 4,
    EventDestabilizedOrb    = 5
};

enum eNPCs
{
    NightfallenEnergyDummy       = 68553,
    QuarantineCrystal            = 99560
};

Position const g_CenterPos = { 3147.14f, 4887.08f, 617.62f, 0.0f };

/// Corstilax - 98205
class boss_corstilax : public CreatureScript
{
    public:
        boss_corstilax() : CreatureScript("boss_corstilax") { }

        struct boss_corstilaxAI : public BossAI
        {
            boss_corstilaxAI(Creature* p_Creature) : BossAI(p_Creature, DATA_CORSTILAX)
            {
                m_CastingProtocol = false;
                m_EncounterStarted = false;
            }

            bool m_CastingProtocol;
            bool m_EncounterStarted;

            void Reset() override
            {
                _Reset();

                m_CastingProtocol = false;
                m_EncounterStarted = false;

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetFacingTo(2.78f);
                    me->SetOrientation(2.78f);
                });

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eNPCs::NightfallenEnergyDummy, 100.0f))
                        DoCast(l_Creature, eSpells::SpawnVisual, false);
                });

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::NightwellEnergy);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SuppressionProtocolFixate);
                }
            }

            void OnTaunt(Unit* p_Taunter) override
            {
                if (!p_Taunter->IsPlayer())
                    return;

                EnterCombat(p_Taunter);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (m_EncounterStarted)
                    return;

                m_EncounterStarted = true;
                _EnterCombat();

                if (Creature* l_Creature = me->FindNearestCreature(eNPCs::NightfallenEnergyDummy, 100.0f))
                    l_Creature->RemoveAura(eSpells::SpawnVisual);

                me->RemoveAura(eSpells::SpawnVisual);
                me->InterruptNonMeleeSpells(true);

                events.ScheduleEvent(eEvents::EventProtocol, 5000);
                events.ScheduleEvent(eEvents::EventQuarantine, 22000);
                events.ScheduleEvent(eEvents::EventCleansingForce, 30000);
                events.ScheduleEvent(eEvents::EventEnergyBurst, 20000);
                events.ScheduleEvent(eEvents::EventDestabilizedOrb, 13000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::NightwellEnergy);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SuppressionProtocolFixate);
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SuppressionProtocol:
                    {
                        if (Creature* l_Protocol = me->SummonCreature(NPC_SUPPRESSION_PROTOCOL, p_Target->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 12000))
                        {
                            l_Protocol->CastSpell(p_Target, eSpells::SuppressionProtocolFixate, true);

                            if (p_Target->IsPlayer() && l_Protocol->IsAIEnabled)
                                l_Protocol->AI()->Talk(0, p_Target->ToPlayer()->GetGUID());
                        }

                        DoCast(eSpells::SuppressionProtocalDamage);
                        break;
                    }
                    case eSpells::DestabilizedOrb:
                    {
                        if (Creature* l_Orb = me->SummonCreature(NPC_DESTABLIZED_ORB, p_Target->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 60000))
                            l_Orb->CastSpell(l_Orb, eSpells::DestabilizedOrbAreatrigger, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventProtocol:
                    {
                        uint64 l_GUID = me->GetGUID();

                        /// Somehow the boss is a bitch and not rooted with that channeling spell. What a cunt.
                        me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                        me->SetReactState(REACT_PASSIVE);
                        m_CastingProtocol = true;
                        DoCast(eSpells::SuppressionProtocol);

                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void { m_CastingProtocol = false; });

                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                        {
                            Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                            if (!l_Creature || !l_Creature->IsAIEnabled)
                                return;

                            l_Creature->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                            l_Creature->SetReactState(REACT_AGGRESSIVE);
                        });

                        events.ScheduleEvent(eEvents::EventProtocol, 46000);
                        break;
                    }
                    case eEvents::EventQuarantine:
                    {
                        DoCast(eSpells::Quarantine);
                        events.ScheduleEvent(eEvents::EventQuarantine, 46000);
                        break;
                    }
                    case eEvents::EventCleansingForce:
                    {
                        DoCast(eSpells::CleansingForce);
                        events.ScheduleEvent(eEvents::EventCleansingForce, 46000);
                        break;
                    }
                    case eEvents::EventEnergyBurst:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 100.0f, false);

                        for (Player* l_Player : l_PlayerList)
                            me->CastSpell(l_Player, eSpells::NightwellEnergy, false);

                        if (Creature* l_Pipe = me->SummonCreature(NPC_PIPE_STALKER, 3101.87f, 4908.10f, 622.39f, 5.86f, TEMPSUMMON_TIMED_DESPAWN, 11000))
                        {
                            l_Pipe->CastSpell(l_Pipe, eSpells::EnergyBurstVisual, true);

                            for (uint8 l_I = 0; l_I < 9; ++l_I)
                            {
                                float l_Angle = (float)rand_norm() * static_cast<float>(2 * M_PI);
                                float l_Dist = 40.0f * (float)rand_norm();
                                float l_X = g_CenterPos.GetPositionX() + l_Dist * std::cos(l_Angle);
                                float l_Y = g_CenterPos.GetPositionY() + l_Dist * std::sin(l_Angle);
                                JadeCore::NormalizeMapCoord(l_X);
                                JadeCore::NormalizeMapCoord(l_Y);
                                l_Pipe->CastSpell(l_X, l_Y, 617.63f, eSpells::EnergyBurstAreatrigger, TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_LOS));
                            }
                        }

                        events.ScheduleEvent(eEvents::EventEnergyBurst, 35000);
                        break;
                    }
                    case eEvents::EventDestabilizedOrb:
                    {
                        DoCast(eSpells::DestabilizedOrb);
                        events.ScheduleEvent(eEvents::EventDestabilizedOrb, 35000);
                        break;
                    }
                    default:
                        break;
                }
                if (!m_CastingProtocol)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_corstilaxAI(p_Creature);
        }
};

/// Suppression Protocol - 99628
class npc_corstilax_suppression_protocol : public CreatureScript
{
    public:
        npc_corstilax_suppression_protocol() : CreatureScript("npc_corstilax_suppression_protocol") { }

        struct npc_corstilax_suppression_protocolAI : public ScriptedAI
        {
            npc_corstilax_suppression_protocolAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            uint64 m_PlayerTarget = 0;

            void Reset() override { }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SuppressionProtocolFixate)
                {
                    m_PlayerTarget = p_Target->GetGUID();
                    events.ScheduleEvent(EVENT_1, 500);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (events.ExecuteEvent() == EVENT_1)
                {
                    if (Unit* l_Target = Unit::GetUnit(*me, m_PlayerTarget))
                        me->GetMotionMaster()->MovePoint(1, l_Target->GetPosition());

                    events.ScheduleEvent(EVENT_1, 1000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_corstilax_suppression_protocolAI(p_Creature);
        }
};

/// Quarantine - 99560
class npc_corstilax_quarantine : public CreatureScript
{
    public:
        npc_corstilax_quarantine() : CreatureScript("npc_corstilax_quarantine") { }

        struct npc_corstilax_quarantineAI : public ScriptedAI
        {
            npc_corstilax_quarantineAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->setFaction(35);
                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
            }

            uint64 m_PlayerTarget = 0;
            uint16 m_DespawnTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->IsPlayer())
                    return;

                m_DespawnTimer = 21000;
                m_PlayerTarget = p_Summoner->GetGUID();
                p_Summoner->CastSpell(me, eSpells::QuarantineRideVehicle, true);
                Talk(0, p_Summoner->ToPlayer()->GetGUID());
            }

            void OnSpellClick(Unit* clicker) override
            {
                if (!clicker->HasAura(eSpells::QuarantineStun))
                    clicker->CastSpell(me, eSpells::ShatterQuarantine);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_DespawnTimer)
                {
                    if (m_DespawnTimer <= p_Diff)
                    {
                        me->DespawnOrUnsummon();
                        m_DespawnTimer = 0;
                    }
                    else
                        m_DespawnTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_corstilax_quarantineAI(p_Creature);
        }
};

class spell_corstilax_shatter_quarantine : public SpellScriptLoader
{
    public:
        spell_corstilax_shatter_quarantine() : SpellScriptLoader("spell_corstilax_shatter_quarantine") { }

        class spell_corstilax_shatter_quarantine_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_corstilax_shatter_quarantine_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Creature*> l_CreatureList;
                std::list<Player*> l_PlayerList;
                l_Caster->GetCreatureListInGrid(l_CreatureList, 20.0f);
                l_Caster->GetPlayerListInGrid(l_PlayerList, 20.0f);

                for (Player* l_Player : l_PlayerList)
                {
                    if (l_Player->isAlive() && l_Player->HasAura(eSpells::QuarantineStun))
                        l_Player->RemoveAurasDueToSpell(eSpells::QuarantineStun);
                }

                for (Creature* l_Creature : l_CreatureList)
                {
                    if (l_Creature->GetEntry() == eNPCs::QuarantineCrystal)
                        l_Creature->DespawnOrUnsummon();
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_corstilax_shatter_quarantine_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_corstilax_shatter_quarantine_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Cleansing Force - 196115
class spell_corstilax_cleansing_force : public SpellScriptLoader
{
    public:
        spell_corstilax_cleansing_force() : SpellScriptLoader("spell_corstilax_cleansing_force") { }

        class spell_corstilax_cleansing_force_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_corstilax_cleansing_force_AuraScript);

            uint8 m_Stack = 0;
            uint16 m_Timer = 2000;

            void OnUpdate(uint32 p_Diff, AuraEffect* /*p_AurEff*/)
            {
                if (m_Timer <= p_Diff)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    m_Timer = 2000;

                    std::list<Player*> l_PlayerList;
                    GetPlayerListInGrid(l_PlayerList, l_Caster, 40.0f);

                    for (Player* l_Player : l_PlayerList)
                    {
                        if (Aura* l_Aura = l_Player->GetAura(eSpells::NightwellEnergy))
                        {
                            m_Stack = l_Aura->GetStackAmount();

                            if (m_Stack > 1)
                                l_Aura->SetStackAmount(m_Stack - 1);
                            else
                                l_Aura->Remove();
                        }
                    }
                }
                else
                    m_Timer -= p_Diff;
            }

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CleansingForceAreatrigger, true);
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_corstilax_cleansing_force_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectApply += AuraEffectApplyFn(spell_corstilax_cleansing_force_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_corstilax_cleansing_force_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_corstilax()
{
    /// Boss
    new boss_corstilax();

    /// Creatures
    new npc_corstilax_suppression_protocol();
    new npc_corstilax_quarantine();

    /// Spell
    new spell_corstilax_cleansing_force();
    new spell_corstilax_shatter_quarantine();
}
#endif
