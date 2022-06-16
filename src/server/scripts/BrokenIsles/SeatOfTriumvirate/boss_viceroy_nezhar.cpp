////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2020 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "seat_of_triumvirate.hpp"

enum eSpells
{
    /// Viceroy Nezhar
    SpellVoidPhased         = 246913,
    SpellDarkBlast          = 244750,
    SpellUmbralTentacles    = 244769,
    SpellCollapsingVoidAT   = 244789,
    SpellHowlingDark        = 244751,
    SpellEntropicForce      = 246324,
    SpellEntropicForceAT    = 246326,
    SpellSummonTentancle    = 244770,
    SpellVoidLashing        = 244916,
    SpellUnstableEntrance   = 249082,
    SpellDarkBulwark        = 248804,
    SpellEtherealGuardLeft  = 249336,
    SpellEtherealGuardRight = 249335,
    SpellEternalTwilight    = 248736,
    SpellCallForHelp        = 249306,
    SpellCollapsingVoidDot  = 244906,
};

enum eEvents
{
    EventDarkBlast = 1,
    EventHowlingDark,
    EventEntropicForce,
    EventSummonTentacles,
    EventDarkBulwark,
};

enum eTalks
{
    TalkAggro,
    TalkEternalTwilight,
    TalkKill,
    TalkDeath,
};

/// Viceroy Nezhar - 124874
class boss_viceroy_nezhar : public CreatureScript
{
    public:
        boss_viceroy_nezhar() : CreatureScript("boss_viceroy_nezhar")
        {}

        struct boss_viceroy_nezhar_AI : public BossAI
        {
            explicit boss_viceroy_nezhar_AI(Creature* p_Me) : BossAI(p_Me, eData::DataNezhar)
            {
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::SpellCallForHelp)
                    DoZoneInCombat(p_Target->ToCreature());
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellDarkBulwark)
                    m_ShieldStacks++;
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(eTalks::TalkDeath);
                BossAI::JustDied(p_Killer);

                HandleCollapsingVoid(false);
                me->DespawnAreaTriggersInArea(eSpells::SpellCollapsingVoidAT, 250.f);

                instance->AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    HandleCollapsingVoid(false);
                });

                if (!IsChallengeMode())
                    instance->SendScenarioEventToAllPlayers(eScenarioEvents::NezharDeath);
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                p_Value = 0;
            }

            void PowerModified(Powers p_Power, int32 p_Value) override
            {
                if (!me->isInCombat())
                    return;

                if (p_Power == Powers::POWER_ENERGY &&
                    p_Value == 100)
                {
                    if (IsMythic() && !m_IsProtected)
                    {
                        events.DelayEvents(12 * IN_MILLISECONDS);
                        me->SetReactState(REACT_PASSIVE);
                        me->StopAttack();
                        me->GetMotionMaster()->Clear();
                        m_IsProtected = true;
                        events.ScheduleEvent(eEvents::EventDarkBulwark, 1);
                    }
                }
            }

            void HandleCollapsingVoid(bool p_Enable)
            {
                if (instance == nullptr)
                    return;

                if (Unit* l_Center = sObjectAccessor->FindUnit(instance->GetData64(eCreatures::NpcCenterPoint)))
                {
                    if (p_Enable)
                        l_Center->CastSpell(l_Center, eSpells::SpellCollapsingVoidAT, true);
                    else
                    {
                        if (AreaTrigger* l_At = me->FindNearestAreaTrigger(eSpells::SpellCollapsingVoidAT, 150.f))
                        {
                            l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                            l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                            l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                            l_At->SetDuration(100);
                            l_Center->DelayedRemoveAura(eSpells::SpellCollapsingVoidAT, IN_MILLISECONDS);
                        }
                    }
                }
            }

            void CheckDistToEvade()
            {
                Position l_Center = me->GetHomePosition();

                if (me->GetDistance2d(l_Center.m_positionX, l_Center.m_positionY) >= 61.f)
                {
                    EnterEvadeMode();
                    return;
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellEternalTwilight)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetPower(POWER_ENERGY, 0);
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_SpellID == eSpells::SpellDarkBulwark)
                {
                    m_ShieldStacks--;
                    if (m_ShieldStacks <= 0)
                    {
                        m_IsProtected = false;
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        me->SetReactState(REACT_AGGRESSIVE);
                    }
                }
                else if (p_SpellID == eSpells::SpellEntropicForce)
                {
                    me->DespawnAreaTriggersInArea(eSpells::SpellEntropicForceAT, 250.f);
                    instance->DoRemoveForcedMovementsOnPlayers();
                }
            }

            void CheckIntro()
            {
                if (instance == nullptr)
                    return;

                if (instance->GetBossState(eData::DataSaprish) == DONE)
                    me->RemoveAura(eSpells::SpellVoidPhased);
                else
                    me->CastSpell(me, eSpells::SpellVoidPhased, true);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.23f;
            }

            void Reset() override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->ClearUnitState(UNIT_STATE_ROOT);
                BossAI::Reset();

                me->DespawnAreaTriggersInArea(eSpells::SpellCollapsingVoidAT, 250.f);

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    CheckIntro();
                    HandleCollapsingVoid(false);
                });
            }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);

                me->SetPower(Powers::POWER_ENERGY, 0);

                m_RegenTimer = 0;

                m_IsProtected = false;
                _EnterCombat();

                HandleCollapsingVoid(true);

                DoCastAOE(eSpells::SpellCallForHelp);

                events.ScheduleEvent(eEvents::EventDarkBlast, IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventHowlingDark, 17 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSummonTentacles, 13 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventEntropicForce, 33 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon->GetEntry() == eCreatures::NpcShadowguardVoidTender)
                {
                    uint64 l_SummonerGUID = me->GetGUID();

                    p_Summon->AI()->ApplyAllImmunities(true);

                    p_Summon->AddDelayedEvent([p_Summon, l_SummonerGUID]() -> void
                    {
                        if (Unit* l_Owner = sObjectAccessor->GetCreature(*p_Summon, l_SummonerGUID))
                        {
                            p_Summon->SetReactState(REACT_PASSIVE);
                            p_Summon->StopAttack();
                            p_Summon->StopMoving();
                            p_Summon->GetMotionMaster()->Clear();
                            p_Summon->SetFacingTo(p_Summon->GetAngle(l_Owner));
                            p_Summon->DelayedCastSpell(l_Owner, eSpells::SpellDarkBulwark, true, 1);
                        }
                    }, 1 * TimeConstants::IN_MILLISECONDS);
                }
                
                BossAI::JustSummoned(p_Summon);
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventDarkBlast:
                    {
                        DoCastVictim(eSpells::SpellDarkBlast, false);
                        events.ScheduleEvent(eEvents::EventDarkBlast, 2 * IN_MILLISECONDS + 200);
                        break;
                    }

                    case eEvents::EventDarkBulwark:
                    {
                        m_ShieldStacks = 0;

                        Talk(eTalks::TalkEternalTwilight);

                        me->SetPower(Powers::POWER_ENERGY, 0);

                        me->AddUnitState(UNIT_STATE_ROOT);

                        DoCast(me, eSpells::SpellEtherealGuardLeft, true);
                        DoCast(me, eSpells::SpellEtherealGuardRight, true);
                        
                        me->DelayedCastSpell(me, eSpells::SpellEternalTwilight, false, 3200);

                        break;
                    }

                    case eEvents::EventEntropicForce:
                    {
                        DoCastAOE(eSpells::SpellEntropicForce);

                        AddTimedDelayedOperation(5000, [this]() -> void
                        {
                            instance->DoRemoveForcedMovementsOnPlayers();
                        });

                        events.ScheduleEvent(eEvents::EventEntropicForce, 60 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventHowlingDark:
                    {
                        DoCastAOE(eSpells::SpellHowlingDark);
                        events.ScheduleEvent(eEvents::EventHowlingDark, 32 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSummonTentacles:
                    {
                        DoCastAOE(eSpells::SpellUmbralTentacles);
                        events.ScheduleEvent(eEvents::EventSummonTentacles, 30 * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void RegenPower(uint32 const p_Diff)
            {
                m_RegenTimer += p_Diff;

                if (m_RegenTimer >= IN_MILLISECONDS)
                {
                    m_RegenTimer = 0;
                    me->ModifyPower(POWER_ENERGY, 2);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                CheckDistToEvade();

                RegenPower(p_Diff);

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }

            uint8 m_ShieldStacks;
            uint32 m_RegenTimer;
            bool m_IsProtected;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_viceroy_nezhar_AI(p_Me);
        }
};

/// Umbral Tentacle - 249082
class npc_nezhar_umbral_tentacle : public CreatureScript
{
    public:
        npc_nezhar_umbral_tentacle() : CreatureScript("npc_nezhar_umbral_tentacle")
        {}

        enum eEvents
        {
            EventVoidLashing = 1,
        };

        struct npc_nezhar_umbral_tentacle_AI : public ScriptedAI
        {
            explicit npc_nezhar_umbral_tentacle_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventVoidLashing, 2 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void IsSummonedBy(Unit* /**/) override
            {
                DoCast(me, eSpells::SpellUnstableEntrance, true);
                DoZoneInCombat(me, 100.f);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventVoidLashing:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellVoidLashing);

                        events.ScheduleEvent(eEvents::EventVoidLashing, 8 * IN_MILLISECONDS);
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

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nezhar_umbral_tentacle_AI(p_Me);
        }
};

/// Umbral Tentacle - 244769
class spell_nezhar_umbral_tentacle : public SpellScriptLoader
{
    public:
        spell_nezhar_umbral_tentacle() : SpellScriptLoader("spell_nezhar_umbral_tentacle")
        {}

        class spell_nezhar_umbral_tentacle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nezhar_umbral_tentacle_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitEntryCheck(eCreatures::NpcTentacleStalker, 0, 0));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nezhar_umbral_tentacle_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nezhar_umbral_tentacle_SpellScript();
        }
};

/// Collapsing Void - 244789
class at_nezhar_collapsing_void : public AreaTriggerEntityScript
{
    public:
        at_nezhar_collapsing_void() : AreaTriggerEntityScript("at_nezhar_collapsing_void")
        {}

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            if (p_Target->IsPlayer())
                p_Target->CastSpell(p_Target, eSpells::SpellCollapsingVoidDot, true);

            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target && p_Target->IsPlayer())
                p_Target->RemoveAura(eSpells::SpellCollapsingVoidDot);

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nezhar_collapsing_void();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_viceroy_nezhar()
{
    new boss_viceroy_nezhar();
    new npc_nezhar_umbral_tentacle();

    /// Spells
    new spell_nezhar_umbral_tentacle();

    new at_nezhar_collapsing_void();
}
#endif
