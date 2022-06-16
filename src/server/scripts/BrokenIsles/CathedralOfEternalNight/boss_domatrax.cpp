#include "cathedral_of_eternal_night.hpp"

enum eSpells
{
    /// Domatrax
    SpellDomatraxDissolve   = 241001,
    SpellChaoticEnergy      = 234107,
    SpellFelsoulClave       = 236543,
    SpellPortalImps         = 235827,
    SpellPortalFelguards    = 235881,
    SpellPortalMistress     = 235882,
    SpellApproachingDoom    = 241622,
    SpellFrenzy             = 243157,

    /// Aegis of Aggramar
    SpellItemYellowAura         = 144373,
    SpellAegisPowerBar          = 236421,
    SpellAegisOfAggramarBuff    = 238410,
    SpellAegisOfAggramarAt      = 239350,
    SpellAegisOfAggramarMephistroth = 234125,
};

/// Domatrax - 119542
class boss_domatrax : public CreatureScript
{
    public:
        boss_domatrax() : CreatureScript("boss_domatrax")
        {}

        enum eEvents
        {
            EventChaoticEnergy  = 1,
            EventFelsoulCleave,
            EventPortalDemons,
        };

        enum eTalks
        {
            TalkIntro   = 0,
            TalkAggro,
            TalkChaoticEnergy,
            TalkSummonDemons,
            TalkDead,
            TalkPortalWarn,
            TalkChaoticWarn,
        };

        struct boss_domatrax_AI : public BossAI
        {
            explicit boss_domatrax_AI(Creature* p_Me) : BossAI(p_Me, eData::DataDomatrax)
            {
                m_FirstDemonPortals     = false;
                m_SecondDemonPortals    = false;
                m_Enrage                = false;
                m_Intro                 = false;
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionBossIntro && !m_Intro)
                {
                    me->SetVisible(true);
                    Talk(TalkIntro);
                    m_Intro = true;
                    DoCast(me, eSpells::SpellDomatraxDissolve, true);

                    AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    });
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (p_Summon != nullptr)
                    DoZoneInCombat(p_Summon, 250.f);
            }

            void CleanUp()
            {
                me->DespawnCreaturesInArea(eCreatures::NpcHellblazeFelguard, 120.f);
                me->DespawnCreaturesInArea(eCreatures::NpcHellblazeImp, 120.f);

                if (IsMythic())
                {
                    me->DespawnCreaturesInArea(eCreatures::NpcHellblazeMistress, 120.f);
                    me->DespawnCreaturesInArea(eCreatures::NpcSearingOverfiend, 120.f);
                }

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegisOfAggramarBuff);
            }

            void CheckIntro()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance->GetData(me->GetEntry()) == EncounterState::DONE)
                {
                    m_Intro = true;
                    me->SetVisible(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                }
                else
                {
                    m_Intro = false;
                    me->SetVisible(false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void GetMythicHealthMod(float& p_HealthMod) override
            {
                p_HealthMod = 2.140f;
            }

            void Reset() override
            {
                m_FirstDemonPortals = false;
                m_SecondDemonPortals = false;
                m_Enrage = false;

                BossAI::Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();

                me->SetReactState(REACT_AGGRESSIVE);
                me->ClearUnitState(UNIT_STATE_CANNOT_TURN);

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    CheckIntro();
                    CleanUp();
                });
            }

            void JustDied(Unit* p_Killer) override
            {
                BossAI::JustDied(p_Killer);

                Talk(eTalks::TalkDead);

                CleanUp();

                instance->AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    CleanUp();
                });

                ActivateAegis(false);
            }

            void ActivateAegis(bool p_Activate)
            {
                Creature* l_Aegis = me->FindNearestCreature(eCreatures::NpcAegisOfAggramar, 150.f);

                if (l_Aegis == nullptr)
                    return;

                if (p_Activate)
                {
                    l_Aegis->CastSpell(l_Aegis, eSpells::SpellAegisPowerBar, true);
                    l_Aegis->CastSpell(l_Aegis, eSpells::SpellAegisOfAggramarAt, true);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, l_Aegis);
                }
                else
                {
                    l_Aegis->RemoveAllAuras();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, l_Aegis);

                    if (me->isDead())
                        l_Aegis->DespawnOrUnsummon(IN_MILLISECONDS);
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);
                events.ScheduleEvent(eEvents::EventFelsoulCleave, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventChaoticEnergy, 32 * IN_MILLISECONDS);
                _EnterCombat();
                ActivateAegis(true);
            }

            void EnterEvadeMode() override
            {
                ActivateAegis(false);
                BossAI::EnterEvadeMode();
            }

            void DamageTaken(Unit* /**/, uint32 & /**/, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(90) && !m_FirstDemonPortals)
                {
                    m_FirstDemonPortals = true;
                    events.ScheduleEvent(eEvents::EventPortalDemons, IN_MILLISECONDS);
                }
                else if (HealthBelowPct(50) && !m_SecondDemonPortals)
                {
                    m_SecondDemonPortals = true;
                    events.ScheduleEvent(eEvents::EventPortalDemons, IN_MILLISECONDS);
                }
                else if (HealthBelowPct(25) && !m_Enrage)
                {
                    DoCast(me, eSpells::SpellFrenzy, true);
                    m_Enrage = true;
                }
            }

            void ActivatePortal(Position const & p_Pos)
            {
                Talk(eTalks::TalkPortalWarn);

                uint32 l_SpellId = 0;

                if (m_FirstDemonPortals && !m_SecondDemonPortals)
                    l_SpellId = eSpells::SpellPortalImps;
                else if (m_FirstDemonPortals && m_SecondDemonPortals)
                    l_SpellId = IsMythic() ? eSpells::SpellPortalMistress : eSpells::SpellPortalFelguards;

                Creature* l_Portal = me->SummonCreature(eCreatures::NpcFelPortalDummy, p_Pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                if (l_Portal)
                {
                    l_Portal->CastSpell(l_Portal, l_SpellId, true);
                    
                    if (IsMythic())
                        l_Portal->DelayedCastSpell(l_Portal, eSpells::SpellApproachingDoom, false, 500);
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventFelsoulCleave:
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                            me->AttackStop();
                            me->StopMoving();
                            me->SetFacingToObject(l_Victim);

                            DoCast(me, eSpells::SpellFelsoulClave);
                            AddTimedDelayedOperation(2800, [this]() -> void
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                            });
                        }

                        events.ScheduleEvent(eEvents::EventFelsoulCleave, 18 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventChaoticEnergy:
                    {
                        Talk(eTalks::TalkChaoticEnergy);
                        Talk(eTalks::TalkChaoticWarn);
                        DoCastAOE(eSpells::SpellChaoticEnergy);
                        events.ScheduleEvent(eEvents::EventChaoticEnergy, 38 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventPortalDemons:
                    {
                        Talk(eTalks::TalkSummonDemons);

                        std::array<Position, 4> l_PortalsPositions =
                        {
                            {
                                { -565.811f, 2502.28f, 533.539f, 0.70425f },
                                { -563.040f, 2548.07f, 534.079f, 5.38708f },
                                { -518.479f, 2499.69f, 533.604f, 2.22360f },
                                { -516.764f, 2547.73f, 533.584f, 3.76660f },
                            }
                        };

                        uint32 l_FirstPortal = !m_FirstDemonPortals ? 0 : 2;
                        uint32 l_SecondPortal = !m_FirstDemonPortals ? 1 : 3;

                        ActivatePortal(l_PortalsPositions.at(l_FirstPortal));
                        ActivatePortal(l_PortalsPositions.at(l_SecondPortal));
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                BossAI::UpdateAI(p_Diff);
            }

            private:
                bool m_FirstDemonPortals, m_SecondDemonPortals, m_Enrage, m_Intro;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_domatrax_AI(p_Me);
        }
};

/// Fel Portal Dummy - 121713, 118834
class npc_domatrax_fel_portal : public CreatureScript
{
    public:
        npc_domatrax_fel_portal() : CreatureScript("npc_domatrax_fel_portal")
        {}

        struct npc_domatrax_fel_portal_AI : public ScriptedAI
        {
            explicit npc_domatrax_fel_portal_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.6043f;
            }

            void JustDied(Unit* /**/) override
            {
                me->RemoveAllAuras();
            }

            void Reset() override
            {
                if (IsChallengeMode())
                    me->AddAura(ChallengeSpells::ChallengersMight, me);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                ScriptedAI::JustSummoned(p_Summon);

                if (p_Summon != nullptr)
                    DoZoneInCombat(p_Summon, 250.f);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_domatrax_fel_portal_AI(p_Me);
        }
};

/// Demon Add - 90407, 119157, 118802
class npc_domatrax_demon_add : public CreatureScript
{
    public:
        npc_domatrax_demon_add() : CreatureScript("npc_domatrax_demon_add")
        {}

        enum eEvents
        {
            EventShadowSweep    = 1,
            EventSearingRend,
            EventFelBlast,
        };

        enum eSpells
        {
            SpellShadowSweep    = 238315,
            SpellSearingRend    = 237596,
            SpellFelBlast           = 238489,
            SpellSearingPressence = 241607
        };

        struct npc_domatrax_demon_add_AI : public ScriptedAI
        {
            explicit npc_domatrax_demon_add_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                ScheduleTasks();

                if (!IsChallengeMode())
                    me->RemoveAllAuras();
            }

            void GetMythicHealthMod(float & p_Mod) override
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcHellblazeImp:
                    {
                        p_Mod = 2.031f;
                        break;
                    }

                    case eCreatures::NpcSearingOverfiend:
                    case eCreatures::NpcHellblazeMistress:
                    {
                        p_Mod = 1.689998f;
                        break;
                    }

                    default: break;
                }
            }
            
            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();

                if (IsChallengeMode() && !me->HasAura(ChallengeSpells::ChallengersMight))
                    me->AddAura(ChallengeSpells::ChallengersMight, me);
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ScheduleTasks()
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcHellblazeMistress:
                    {
                        events.ScheduleEvent(eEvents::EventShadowSweep, urand(6, 12) * IN_MILLISECONDS);
                        break;
                    }

                    case eCreatures::NpcHellblazeImp:
                    {
                        events.ScheduleEvent(eEvents::EventFelBlast, 4 * IN_MILLISECONDS);
                        break;
                    }

                    case eCreatures::NpcHellblazeFelguard:
                    {
                        events.ScheduleEvent(eEvents::EventSearingRend, urand(8, 16) * IN_MILLISECONDS);
                        break;
                    }

                    case eCreatures::NpcSearingOverfiend:
                    {
                        me->CastSpell(me, eSpells::SpellSearingPressence, true);
                        break;
                    }

                    default: break;
                }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventSearingRend:
                    {
                        DoCastVictim(eSpells::SpellSearingRend);
                        events.ScheduleEvent(eEvents::EventSearingRend, urand(15, 25)* IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventShadowSweep:
                    {
                        DoCastVictim(eSpells::SpellShadowSweep);
                        events.ScheduleEvent(eEvents::EventShadowSweep, urand(10, 15)* IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelBlast:
                    {
                        DoCastAOE(eSpells::SpellFelBlast);
                        events.ScheduleEvent(eEvents::EventFelBlast, 3 * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_domatrax_demon_add_AI(p_Me);
        }
};

/// Aegis of Aggramar - 118884
class npc_domatrax_aegis_of_aggramar : public CreatureScript
{
    public:
        npc_domatrax_aegis_of_aggramar() : CreatureScript("npc_domatrax_aegis_of_aggramar")
        {}

        struct npc_domatrax_aegis_of_aggramar_AI : public ScriptedAI
        {
            explicit npc_domatrax_aegis_of_aggramar_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void CheckIntro()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance->GetData(eCreatures::BossDomatrax) == EncounterState::DONE)
                {
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->RemoveAllAuras();
                }
                else
                {
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->AddAura(eSpells::SpellItemYellowAura, me);
                }
            }
            
            void Reset() override
            {
                CheckIntro();
            }

            void sGossipHello(Player* p_Player) override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr || p_Player == nullptr)
                    return;

                if (l_Instance->GetData(eCreatures::BossDomatrax) != EncounterState::DONE)
                {
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->RemoveAllAuras();
                    l_Instance->SetData(eCreatures::BossDomatrax, EncounterState::DONE);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_domatrax_aegis_of_aggramar_AI(p_Me);
        }
};

/// Portal Trigger - 235827, 235881, 235883
class spell_domatrax_portal_trigger : public SpellScriptLoader
{
    public:
        spell_domatrax_portal_trigger() : SpellScriptLoader("spell_domatrax_portal_trigger")
        {}

        class spell_domatrax_portal_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_domatrax_portal_trigger_AuraScript);

            void HandlePeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();
                InstanceScript* l_Instance = GetUnitOwner()->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                Creature* l_Boss = sObjectAccessor->GetCreature(*l_Owner, l_Instance->GetData64(eCreatures::BossDomatrax));

                if (l_Boss == nullptr)
                    return;

                switch (GetId())
                {
                    case eSpells::SpellPortalImps:
                    {
                        l_Boss->SummonCreature(eCreatures::NpcHellblazeImp, *l_Owner, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eSpells::SpellPortalFelguards:
                    {
                        l_Boss->SummonCreature(eCreatures::NpcHellblazeFelguard, *l_Owner, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eSpells::SpellPortalMistress:
                    {
                        l_Boss->SummonCreature(eCreatures::NpcHellblazeMistress, *l_Owner, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10 * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_domatrax_portal_trigger_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_domatrax_portal_trigger_AuraScript();
        }
};

/// Felsoul Cleave - 236543
class spell_domatrax_felsoul_cleave : public SpellScriptLoader
{
    public:
        spell_domatrax_felsoul_cleave() : SpellScriptLoader("spell_domatrax_felsoul_cleave")
        {}

        class spell_domatrax_felsoul_cleave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_domatrax_felsoul_cleave_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                
                p_Targets.remove_if([&l_Caster] (WorldObject*& p_Itr) -> bool
                {
                    return !l_Caster->isInFront(p_Itr, float(M_PI)/4.f);
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_domatrax_felsoul_cleave_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_domatrax_felsoul_cleave_SpellScript();
        }
};

/// Aegis of Agrammar - 239350
class at_domatrax_aegis_of_aggramar : public AreaTriggerEntityScript
{
    public:
        at_domatrax_aegis_of_aggramar() : AreaTriggerEntityScript("at_domatrax_aegis_of_aggramar")
        {
            m_IsReducing = false;
        }

        void OnCreate(AreaTrigger* p_At) override
        {
            p_At->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 950);

            p_At->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 1);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.f);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.f);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 1.f); ///< (+200% / TimeToTargetScale / 1000) per second
            p_At->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1.40f);

            m_CurrRadius = m_BaseRadius = p_At->GetRadius();
            m_IsReducing = false;
        }

        bool OnRemoveTarget(AreaTrigger* /**/, Unit* p_Target) override
        {
            if (p_Target->IsPlayer())
                p_Target->RemoveAurasDueToSpell(eSpells::SpellAegisOfAggramarBuff);

            return true;
        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            m_BaseRadius = p_At->GetAreaTriggerInfo().Radius;

            float l_VisualRadius = p_At->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);

            GuidList& l_Targets = *(p_At->GetAffectedPlayers());

            for (auto & l_Itr : l_Targets)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, l_Itr);

                if (l_Target == nullptr || !l_Target->IsPlayer())
                    continue;

                if (p_At->GetExactDist2d(l_Target) >= m_CurrRadius)
                    l_Target->RemoveAurasDueToSpell(eSpells::SpellAegisOfAggramarBuff);
                else
                    l_Target->AddAura(eSpells::SpellAegisOfAggramarBuff, l_Target);
            }

            m_IsReducing = std::any_of(l_Targets.cbegin(), l_Targets.cend(), [&p_At] (auto & p_Itr)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, p_Itr);

                if (l_Target == nullptr)
                    return false;

                if (l_Target->isDead())
                    return false;

                if (l_Target->HasAura(eSpells::SpellAegisOfAggramarBuff))
                    return true;

                return false;
            });

            if (m_IsReducing)
            {
                l_VisualRadius -= CalculatePct(1.f, 0.18f);
                m_CurrRadius = (m_BaseRadius * l_VisualRadius);
            }

            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_VisualRadius); ///< (+200% / TimeToTargetScale / 1000) per second
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_domatrax_aegis_of_aggramar();
        }

    private:
        std::set<uint64> m_TargetsInside;
        float m_CurrRadius, m_BaseRadius;
        bool m_IsReducing;
};

#ifndef __clang_analyzer__
void AddSC_boss_domatrax()
{
    new boss_domatrax();
    new npc_domatrax_demon_add();
    new npc_domatrax_fel_portal();
    new npc_domatrax_aegis_of_aggramar();

    new spell_domatrax_portal_trigger();
    new spell_domatrax_felsoul_cleave();

    new at_domatrax_aegis_of_aggramar();
}
#endif
