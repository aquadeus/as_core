////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "vault_of_the_wardens.hpp"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_LAVA_WREATH     = 1,
    SAY_FREEZE          = 2,
    SAY_FREEZE_EMOTE    = 3,
    SAY_BRITTLE         = 4,
    ///SAY_AGGRO           = 5   I don`t know where "Fire has a life of its own."
    SAY_DEATH           = 6,
    SAY_SYSTEM          = 7
};

enum Spells
{
    SPELL_FROZEN                = 200354,
    SPELL_HEAT_UP               = 200981,
    SPELL_SMOLDERING            = 193209,
    SPELL_VOLCANO               = 192621,
    SPELL_LAVA_DOT              = 192519,
    SPELL_LAVA_WREATH           = 192631,
    SPELL_LAVA_WREATH_VIS       = 193354,
    SPELL_LAVA_WREATH_TRIG      = 193353,
    SPELL_LAVA_WREATH_AT        = 192571,
    SPELL_FISSURE               = 192522,
    SPELL_FISSURE_AT_RIGHT      = 193238,
    SPELL_FISSURE_AT_LEFT       = 213214,
    SPELL_FISSURE_MISSILE_RIGHT = 193355,
    SPELL_FISSURE_MISSILE_LEFT  = 213217,
    SPELL_SUM_EMBER             = 195170,

    /// Ember
    SPELL_EMBER_SMOLDERING      = 195188,
    SPELL_SEAR                  = 195332,
    SPELL_FIRED_UP              = 202169,
    SPELL_DETONATE              = 195187,

    /// System security
    SPELL_ACTIVATE_SYSTEM_VIS   = 195189,
    SPELL_ACTIVATE_SYSTEM       = 195213,
    SPELL_FREEZE_ROOM           = 195227,
    SPELL_BRITTLE               = 192517,
    SPELL_ACTIVATE_MEASURES     = 200364,

    /// Trashs
    SPELL_THUNDERCLAP           = 196242,
    SPELL_METEOR                = 196249,
    SPELL_SUMMON_GRIMGUARD      = 202728,
    SPELL_MORTAL_STRIKE         = 16856,
    SPELL_CHARGE                = 199172,
    SPELL_ENRAGE                = 8599,

    /// Updraft mechanic
    SPELL_CHILLED_TO_THE_BONE   = 200202,
    SPELL_UPDRAFT_VISUAL        = 200257
};

enum eEvents
{
    EVENT_VOLCANO           = 1,
    EVENT_LAVA_WREATH       = 2,
    EVENT_FISSURE           = 3,
    EVENT_SUM_EMBER         = 4,
    EVENT_SYSTEM_SECURITY   = 5
};

/// Ashgolm - 95886
class boss_ashgolm : public CreatureScript
{
    public:
        boss_ashgolm() : CreatureScript("boss_ashgolm") { }

        struct boss_ashgolmAI : public BossAI
        {
            boss_ashgolmAI(Creature* p_Creature) : BossAI(p_Creature, DATA_ASHGOLM) { }

            void Reset() override
            {
                _Reset();

                me->RemoveAllAreasTrigger();

                if (instance && instance->GetData(DATA_ASHGOLM_SYSTEM) != DONE)
                    instance->SetData(DATA_ASHGOLM_SYSTEM, DONE);

                std::list<GameObject*> l_VolcanoList;
                me->GetGameObjectListWithEntryInGrid(l_VolcanoList, GO_ASHGOLM_VOLCANO, 80.0f);

                for (GameObject* l_Iter : l_VolcanoList)
                    l_Iter->Delete();

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    DoCast(me, SPELL_FROZEN, true);
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                DoCast(SPELL_HEAT_UP);

                events.ScheduleEvent(EVENT_VOLCANO, 10000);
                events.ScheduleEvent(EVENT_LAVA_WREATH, 25000);
                events.ScheduleEvent(EVENT_FISSURE, 40000);
                events.ScheduleEvent(EVENT_SUM_EMBER, 15000);
                events.ScheduleEvent(EVENT_SYSTEM_SECURITY, 2000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
                me->RemoveAllAreasTrigger();
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_HEAT_UP)
                {
                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        DoCast(me, SPELL_SMOLDERING, true);
                    });
                }
                else if (p_SpellInfo->Id == SPELL_BRITTLE)
                {
                    Talk(SAY_FREEZE);
                    Talk(SAY_FREEZE_EMOTE);

                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        me->DespawnAreaTriggersInArea(Spells::SPELL_LAVA_WREATH_AT, 150.0f);
                        me->DespawnAreaTriggersInArea(Spells::SPELL_FISSURE_AT_LEFT, 150.0f);
                        me->DespawnAreaTriggersInArea(Spells::SPELL_FISSURE_AT_RIGHT, 150.0f);
                        instance->DoRemoveAurasDueToSpellOnPlayers(Spells::SPELL_LAVA_DOT);
                    });

                    events.ScheduleEvent(EVENT_SYSTEM_SECURITY, 80000);
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case SPELL_LAVA_WREATH_AT:
                    {
                        Position l_Pos = p_AreaTrigger->GetPosition();

                        l_Pos.m_positionX += 14.54199f * std::cos(l_Pos.m_orientation);
                        l_Pos.m_positionY += 14.54199f * std::sin(l_Pos.m_orientation);

                        p_AreaTrigger->MoveAreaTrigger(l_Pos, 7500);

                        p_AreaTrigger->m_Functions.AddFunction([p_AreaTrigger]() -> void
                        {
                            p_AreaTrigger->MoveAreaTriggerCircle(p_AreaTrigger->GetSource().GetPosition(), 14.54199f, 45685);
                        }, 7510);

                        break;
                    }
                    case SPELL_FISSURE_AT_LEFT:
                    case SPELL_FISSURE_AT_RIGHT:
                    {
                        bool l_Clockwise    = p_AreaTrigger->GetSpellId() == SPELL_FISSURE_AT_RIGHT;
                        float l_Radius      = g_AshgolmCenterPos.GetExactDist(p_AreaTrigger);

                        if (l_Radius <= 1.0f)
                            break;

                        p_AreaTrigger->m_Functions.AddFunction([p_AreaTrigger, l_Radius, l_Clockwise]() -> void
                        {
                            Position l_Pos      = g_AshgolmCenterPos;
                            l_Pos.m_orientation = p_AreaTrigger->GetOrientation();

                            p_AreaTrigger->SetSource(l_Pos);
                            p_AreaTrigger->MoveAreaTriggerCircle(g_AshgolmCenterPos, l_Radius, 45685, false, l_Clockwise);
                        }, 100);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_LAVA_WREATH:
                    {
                        Talk(SAY_LAVA_WREATH);
                        DoCast(p_Target, SPELL_LAVA_WREATH_VIS, true);

                        Position l_Pos = p_Target->GetPosition();

                        for (uint8 l_I = 0; l_I < 6; l_I++)
                        {
                            me->CastSpell(l_Pos, SPELL_LAVA_WREATH_TRIG, true);

                            l_Pos.m_orientation += M_PI / 3.0f;
                        }

                        break;
                    }
                    case SPELL_FISSURE:
                    {
                        Position l_Pos;

                        float l_Angle = g_AshgolmCenterPos.GetRelativeAngle(p_Target);

                        std::array<float, 4> l_Steps =
                        {
                            {
                                4.5f,
                                17.25f,
                                21.5f,
                                25.75f
                            }
                        };

                        DoCast(g_AshgolmCenterPos, SPELL_FISSURE_MISSILE_RIGHT, true);

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            l_Pos =
                            {
                                g_AshgolmCenterPos.m_positionX + l_Steps[l_I] * std::cos(l_Angle),
                                g_AshgolmCenterPos.m_positionY + l_Steps[l_I] * std::sin(l_Angle),
                                g_AshgolmCenterPos.m_positionZ,
                                l_Angle
                            };

                            l_Pos.m_orientation = g_AshgolmCenterPos.GetAngle(&l_Pos);

                            DoCast(l_Pos, SPELL_FISSURE_MISSILE_RIGHT, true);
                        }

                        l_Angle -= M_PI;

                        l_Steps =
                        {
                            {
                                4.5f,
                                8.75f,
                                21.5f,
                                30.0f
                            }
                        };

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            l_Pos =
                            {
                                g_AshgolmCenterPos.m_positionX + l_Steps[l_I] * std::cos(l_Angle),
                                g_AshgolmCenterPos.m_positionY + l_Steps[l_I] * std::sin(l_Angle),
                                g_AshgolmCenterPos.m_positionZ,
                                l_Angle
                            };

                            l_Pos.m_orientation = g_AshgolmCenterPos.GetAngle(&l_Pos);

                            DoCast(l_Pos, SPELL_FISSURE_MISSILE_LEFT, true);
                        }

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

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_VOLCANO:
                    {
                        if (Player* l_Target = SelectRangedTarget())
                            DoCast(l_Target, SPELL_VOLCANO);

                        events.ScheduleEvent(EVENT_VOLCANO, 20000);
                        break;
                    }
                    case EVENT_LAVA_WREATH:
                    {
                        DoCast(SPELL_LAVA_WREATH);
                        events.ScheduleEvent(EVENT_LAVA_WREATH, 55000);
                        break;
                    }
                    case EVENT_FISSURE:
                    {
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_FISSURE);
                        events.ScheduleEvent(EVENT_FISSURE, 46000);
                        break;
                    }
                    case EVENT_SUM_EMBER:
                    {
                        if (GameObject* l_Volcano = me->FindNearestGameObject(GO_ASHGOLM_VOLCANO, 60.0f))
                        {
                            Position l_Pos = g_AshgolmCenterPos;

                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 30.0f), frand(0.0f, 2.0f * M_PI));

                            l_Volcano->CastSpell(l_Pos, SPELL_SUM_EMBER);
                        }

                        events.ScheduleEvent(EVENT_SUM_EMBER, 15000);
                        break;
                    }
                    case EVENT_SYSTEM_SECURITY:
                    {
                        if (instance && instance->GetData(DATA_ASHGOLM_SYSTEM) != SPECIAL)
                            instance->SetData(DATA_ASHGOLM_SYSTEM, SPECIAL);

                        Talk(SAY_SYSTEM);
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
            return new boss_ashgolmAI(p_Creature);
        }
};

/// Ember - 99233
class npc_ashgolm_ember : public CreatureScript
{
    public:
        npc_ashgolm_ember() : CreatureScript("npc_ashgolm_ember") { }

        struct npc_ashgolm_emberAI : public Scripted_NoMovementAI
        {
            npc_ashgolm_emberAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_FIRED_UP)
                    events.ScheduleEvent(EVENT_1, 500);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED);
                DoCast(me, SPELL_EMBER_SMOLDERING, true);
                events.ScheduleEvent(EVENT_2, 2000);
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
                        DoCast(SPELL_DETONATE);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                            DoCast(l_Target, SPELL_SEAR);
                        events.ScheduleEvent(EVENT_2, 4000);
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
            return new npc_ashgolm_emberAI(p_Creature);
        }
};

/// Countermeasures - 99240
/// Freeze Statue - 99277
class npc_ashgolm_countermeasures : public CreatureScript
{
    public:
        npc_ashgolm_countermeasures() : CreatureScript("npc_ashgolm_countermeasures") { }

        struct npc_ashgolm_countermeasuresAI : public ScriptedAI
        {
            npc_ashgolm_countermeasuresAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                if (me->GetEntry() == NPC_FREEZE_STATUE)
                    DoCast(me, SPELL_ACTIVATE_SYSTEM_VIS, true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetUInt32Value(UNIT_FIELD_INTERACT_SPELL_ID, SPELL_ACTIVATE_MEASURES);

                DoCast(me, SPELL_ACTIVATE_SYSTEM_VIS, true);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                    {
                        uint64 l_Guid = 0;

                        if (Creature* l_Statue = me->FindNearestCreature(NPC_FREEZE_STATUE, 40.0f))
                        {
                            me->SetFacingTo(me->GetAngle(l_Statue));

                            l_Guid = l_Statue->GetGUID();
                        }

                        AddTimedDelayedOperation(1000, [this]() -> void
                        {
                            DoCast(me, SPELL_ACTIVATE_SYSTEM, true);
                        });

                        AddTimedDelayedOperation(6000, [this, l_Guid]() -> void
                        {
                            if (Creature* l_Statue = Creature::GetCreature(*me, l_Guid))
                                l_Statue->CastSpell(l_Statue, SPELL_FREEZE_ROOM);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_ACTIVATE_SYSTEM)
                {
                    AddTimedDelayedOperation(1000, [this]() -> void
                    {
                        me->SetVisible(false);
                    });
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_ACTIVATE_MEASURES)
                {
                    if (m_Instance && m_Instance->GetData(DATA_ASHGOLM_SYSTEM) != IN_PROGRESS)
                        m_Instance->SetData(DATA_ASHGOLM_SYSTEM, IN_PROGRESS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashgolm_countermeasuresAI(p_Creature);
        }
};

/// Dreadlord Mendacius - 99649
class npc_ashgolm_dreadlord_mendacius : public CreatureScript
{
    public:
        npc_ashgolm_dreadlord_mendacius() : CreatureScript("npc_ashgolm_dreadlord_mendacius") { }

        struct npc_ashgolm_dreadlord_mendaciusAI : public ScriptedAI
        {
            npc_ashgolm_dreadlord_mendaciusAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Summons(p_Creature) { }

            SummonList m_Summons;

            void Reset() override
            {
                events.Reset();

                m_Summons.DespawnAll();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 5000);
                events.ScheduleEvent(EVENT_2, 8000);
                events.ScheduleEvent(EVENT_3, 500);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                m_Summons.Summon(p_Summon);
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
                        DoCastAOE(SPELL_THUNDERCLAP);
                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCastAOE(SPELL_METEOR);
                        events.ScheduleEvent(EVENT_2, 10000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_SUMMON_GRIMGUARD);
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
            return new npc_ashgolm_dreadlord_mendaciusAI(p_Creature);
        }
};

/// Grimguard - 99728
class npc_ashgolm_grimguard : public CreatureScript
{
    public:
        npc_ashgolm_grimguard() : CreatureScript("npc_ashgolm_grimguard") { }

        struct npc_ashgolm_grimguardAI : public ScriptedAI
        {
            npc_ashgolm_grimguardAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 3000);
                events.ScheduleEvent(EVENT_2, 8000);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(30, p_Damage) && !me->HasAura(SPELL_ENRAGE))
                    DoCast(SPELL_ENRAGE);
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
                        DoCastVictim(SPELL_MORTAL_STRIKE);
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            DoCast(l_Target, SPELL_CHARGE);

                        events.ScheduleEvent(EVENT_2, 12000);
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
            return new npc_ashgolm_grimguardAI(p_Creature);
        }
};

/// Updraft - 101548
class npc_ashgolm_updraft : public CreatureScript
{
    public:
        npc_ashgolm_updraft() : CreatureScript("npc_ashgolm_updraft") { }

        struct npc_ashgolm_updraftAI : public ScriptedAI
        {
            npc_ashgolm_updraftAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_1, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (events.ExecuteEvent() == EVENT_1)
                {
                    std::list<Player*> l_PlayerList;
                    me->GetPlayerListInGrid(l_PlayerList, 60.0f);

                    for (Player* l_Player : l_PlayerList)
                    {
                        if (l_Player->m_positionZ >= 80.0f)
                            continue;

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Ashgolm = Creature::GetCreature(*me, l_Instance->GetData64(NPC_ASH_GOLM)))
                                l_Ashgolm->getThreatManager().modifyThreatPercent(l_Player, -99);
                        }

                        Position l_Pos = g_AshgolmCenterPos;

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 30.0f, l_Pos.GetAngle(l_Player));

                        l_Player->CastSpell(l_Pos, SPELL_CHILLED_TO_THE_BONE, true);
                        me->CastSpell(me, SPELL_UPDRAFT_VISUAL, true);
                    }

                    events.ScheduleEvent(EVENT_1, 500);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashgolm_updraftAI(p_Creature);
        }
};

/// Frozen - 200354
class spell_ashgolm_frozen : public SpellScriptLoader
{
    public:
        spell_ashgolm_frozen() : SpellScriptLoader("spell_ashgolm_frozen") { }

        class spell_ashgolm_frozen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ashgolm_frozen_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                    l_Caster->SetReactState(REACT_PASSIVE);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                    l_Caster->SetReactState(REACT_AGGRESSIVE);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_ashgolm_frozen_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ashgolm_frozen_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ashgolm_frozen_AuraScript();
        }
};

/// Brittle - 192517
class spell_brittle : public SpellScriptLoader
{
    public:
        spell_brittle() : SpellScriptLoader("spell_brittle") { }

        class spell_brittle_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brittle_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->IsAIEnabled && l_Caster->GetEntry() == 95886)
                        l_Caster->AI()->Talk(SAY_BRITTLE);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_brittle_AuraScript::OnRemove, EFFECT_4, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brittle_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ashgolm()
{
    /// Boss
    new boss_ashgolm();

    /// Creatures
    new npc_ashgolm_ember();
    new npc_ashgolm_countermeasures();
    new npc_ashgolm_dreadlord_mendacius();
    new npc_ashgolm_grimguard();
    new npc_ashgolm_updraft();

    /// Spells
    new spell_ashgolm_frozen();
    new spell_brittle();
}
#endif
