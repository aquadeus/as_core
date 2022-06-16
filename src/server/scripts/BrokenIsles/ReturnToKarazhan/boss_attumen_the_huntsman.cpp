////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

Position const g_CenterPos = { -11125.33f, -1924.78f, 49.22f };

/// Midnight - 114264
class boss_rtk_midnight : public CreatureScript
{
    public:
        boss_rtk_midnight() : CreatureScript("boss_rtk_midnight") { }

        struct boss_rtk_midnightAI : public BossAI
        {
            boss_rtk_midnightAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataMidnight) { }

            enum eTalks
            {
                Death = 3
            };

            enum eSpells
            {
                StampVisual                 = 227640,
                IntangiblePresence          = 227404,
                IntangiblePresenceVisual1   = 227436,
                MightyStomp                 = 227363,
                MightyStompVisual           = 228670,
                SpectralCharge              = 227365,
                SpectralChargeAreatrigger   = 227367,
                Shimmering                  = 227422,
                Riderless                   = 227338,
                Mezair                      = 227339,
                Enrage                      = 228895
            };

            enum ePhases
            {
                Phase1,
                Phase2
            };

            enum eEvents
            {
                EventIntangiblePresence = 1,
                EventMightyStomp,
                EventSpectralCharge,
                CheckHealth,
                EventMezair,
                MovingToHuntsman,
                DoSpellAttack
            };

            bool m_Aggro;

            bool m_PhaseRiderless;
            bool m_MidnightAttack;
            bool m_IsFirstPhase1;
            bool m_FirstSpectralCharge;
            uint8 m_SpectralCount;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.2f;
            }

            void Reset() override
            {
                _Reset();
                summons.DespawnAll();
                m_PhaseRiderless = false;
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void KilledUnit(Unit* p_Victim) override
            {
                InstanceScript* l_InstanceScript = me->GetInstanceScript();
                EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                if (l_InstanceScript && l_InstanceScript->IsWipe())
                    summons.DoAction(ACTION_8, l_Pred);
                else
                    summons.DoAction(ACTION_7, l_Pred);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                m_IsFirstPhase1 = true;
                m_FirstSpectralCharge = true;
                DefaultEvents(ePhases::Phase1);

                m_Aggro = true;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                summons.DespawnEntry(eCreatures::IntangiblePresence);

                for (SummonList::const_iterator l_Iter = summons.begin(); l_Iter != summons.end(); ++l_Iter)
                {
                    if (Creature* l_Summon = Unit::GetCreature(*me, *l_Iter))
                    {
                        if (l_Summon->IsAIEnabled)
                            l_Summon->AI()->Talk(eTalks::Death);

                        l_Summon->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                    }
                }
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::DataMidnight, EncounterState::FAIL);
            }

            void DefaultEvents(uint8 p_PhaseEvent)
            {
                events.Reset();

                switch (p_PhaseEvent)
                {
                    case ePhases::Phase1:
                    {
                        if (!me->HasAura(eSpells::Enrage))
                        {
                            events.ScheduleEvent(eEvents::EventIntangiblePresence, 6000);
                            if (!m_IsFirstPhase1)
                                events.ScheduleEvent(eEvents::EventSpectralCharge, 5000);
                        }
                        events.ScheduleEvent(eEvents::EventMightyStomp, 14000);
                        events.ScheduleEvent(eEvents::DoSpellAttack, 1000);

                        break;
                    }
                    case ePhases::Phase2:
                    {
                        m_IsFirstPhase1 = false;
                        events.ScheduleEvent(eEvents::CheckHealth, 1000);
                        events.ScheduleEvent(eEvents::EventMezair, 3000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpectralCharge:
                    {
                        uint8 l_MaxSpectralCount = m_FirstSpectralCharge ? 3 : 1;

                        m_SpectralCount++;

                        AddTimedDelayedOperation(15 * IN_MILLISECONDS, [this]() -> void
                        {
                            m_SpectralCount = 0;
                        });

                        if (m_SpectralCount == 3)
                            m_FirstSpectralCharge = false;

                        if (m_SpectralCount < l_MaxSpectralCount)
                            events.ScheduleEvent(eEvents::EventSpectralCharge, 3000);
                        else
                        {
                            events.ScheduleEvent(eEvents::EventSpectralCharge, 40000);
                            EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                            summons.DoAction(ACTION_6, l_Pred);
                        }

                        float l_Angle = frand(0, 2 * M_PI);
                        uint32 l_FirstRowSize = urand(19, 24);
                        uint32 l_MiddleRowSize = urand(5, 36 - l_FirstRowSize);
                        for (uint32 l_I = 0; l_I < (36 - l_MiddleRowSize); ++l_I)
                        {
                            float l_Offset;
                            Position l_Pos = g_CenterPos;
                            if (l_I < l_FirstRowSize)
                                l_Offset = (l_I * 2.0f) - 40.0f;
                            else
                                l_Offset = ((l_I + l_MiddleRowSize) * 2.0f) - 40.0f;

                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Offset, l_Pos.NormalizeOrientation(l_Angle - (M_PI / 2.0f)), true);
                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, -20.0f, l_Pos.NormalizeOrientation(l_Angle - M_PI), true);
                            l_Pos.SetOrientation(l_Angle - M_PI);
                            me->CastSpell(l_Pos, eSpells::SpectralChargeAreatrigger, true);
                        }
                        for (uint32 l_I = 0; l_I < l_MiddleRowSize; ++l_I)
                        {
                            float l_Offset;
                            Position l_Pos = g_CenterPos;

                            l_Offset = ((l_I + l_FirstRowSize) * 2.0f) - 40.0f;
                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Offset, l_Pos.NormalizeOrientation(l_Angle - (M_PI / 2.0f)), true);
                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, 20.0f, l_Pos.NormalizeOrientation(l_Angle - M_PI), true);
                            l_Pos.SetOrientation(l_Angle);

                            me->CastSpell(l_Pos, eSpells::SpectralChargeAreatrigger, true);
                        }
                        break;
                    }
                    case eSpells::MightyStomp:
                    {
                        DoCast(eSpells::MightyStompVisual);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(51) && !m_PhaseRiderless)
                {
                    m_PhaseRiderless = true;
                    me->StopAttack();
                    DoCast(me, eSpells::Riderless, true);
                    DefaultEvents(ePhases::Phase2);
                    EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                    summons.DoAction(ACTION_1, l_Pred);
                    if (me->GetVehicleKit())
                        me->GetVehicleKit()->RemoveAllPassengers();
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::IntangiblePresenceVisual1)
                {
                    if (Creature* l_Summon = me->SummonCreature(eCreatures::IntangiblePresence, p_Target->GetPosition()))
                    {
                        l_Summon->CastSpell(p_Target, 46598, true);
                        l_Summon->CastSpell(l_Summon, eSpells::Shimmering, true);
                    }
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == eCreatures::AttumenTheHuntsman)
                {
                    p_Summon->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                    m_PhaseRiderless = true;
                    events.Reset();
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveAurasDueToSpell(eSpells::Riderless);
                    DoCast(me, eSpells::Enrage, true);
                    DefaultEvents(ePhases::Phase1);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == POINT_MOTION_TYPE && p_ID == 227339) ///< Charge - SPELL_MEZAIR
                    events.ScheduleEvent(eEvents::MovingToHuntsman, 500);

                if (p_ID == 1)
                {
                    if (Creature* l_Attumen = GetClosestCreatureWithEntry(me, eCreatures::AttumenTheHuntsman, 50.0f))
                    {
                        me->SetFacingToObject(l_Attumen);
                        me->CastSpell(l_Attumen, eSpells::Mezair, false);
                    }
                    events.ScheduleEvent(eEvents::EventMezair, 6000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetPositionZ() >= 54.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if (m_Aggro && me->getVictim())
                {
                    /// Once players Engage Attument, All unkilled mobs in Stables should instantly engage players
                    std::list<Creature*> l_NearbyCreatures;
                    me->GetCreatureListInGrid(l_NearbyCreatures, 50.0f);
                    for (Creature* l_Creature : l_NearbyCreatures)
                    {
                        if (l_Creature->GetPositionZ() > 55.0f)
                            continue;

                        if (l_Creature->GetEntry() == eCreatures::AttumenTheHuntsman
                            || l_Creature->GetEntry() == eCreatures::Midnight)
                            continue;

                        if (!l_Creature->IsValidAttackTarget(me->getVictim()))
                            continue;

                        if (!l_Creature->isAlive())
                            continue;

                        l_Creature->SetInCombatWithZone();

                        l_Creature->AddThreat(me->getVictim(), 1.0f);
                    }

                    m_Aggro = false;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 45.0f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventIntangiblePresence:
                    {
                        EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                        summons.DoAction(ACTION_3, l_Pred);
                        DoCast(eSpells::IntangiblePresence);
                        events.ScheduleEvent(eEvents::EventIntangiblePresence, 58000);
                        break;
                    }
                    case eEvents::EventMightyStomp:
                    {
                        EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                        summons.DoAction(ACTION_4, l_Pred);
                        DoCast(eSpells::MightyStomp);
                        events.ScheduleEvent(eEvents::EventMightyStomp, 14000);
                        break;
                    }
                    case eEvents::EventSpectralCharge:
                    {
                        DoCast(eSpells::SpectralCharge);
                        break;
                    }
                    case eEvents::CheckHealth:
                    {
                        if (me->GetHealthPct() >= 99.0f)
                        {
                            m_PhaseRiderless = false;
                            EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                            summons.DoAction(ACTION_2, l_Pred);
                            me->RemoveAurasDueToSpell(eSpells::Riderless);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DefaultEvents(ePhases::Phase1);
                            break;
                        }

                        events.ScheduleEvent(eEvents::CheckHealth, 1000);
                        break;
                    }
                    case eEvents::EventMezair:
                    {
                        if (Creature* l_Huntsman = me->FindNearestCreature(eCreatures::AttumenTheHuntsman, 80.0f))
                        {
                            Position l_TargetPos;
                            l_Huntsman->GetFirstCollisionPosition(l_TargetPos, 10.0f, frand(0.0f, 2.0f * M_PI));
                            me->GetMotionMaster()->MovePoint(1, l_TargetPos);
                        }
                        break;
                    }
                    case eEvents::MovingToHuntsman:
                    {
                        if (Creature* l_Huntsman = me->FindNearestCreature(eCreatures::AttumenTheHuntsman, 80.0f))
                        {
                            Position l_Pos;
                            l_Huntsman->GetNearPosition(l_Pos, 6.0f, frand(0.0f, 6.28f));
                            me->GetMotionMaster()->MovePoint(2, l_Pos);
                        }

                        break;
                    }
                    case eEvents::DoSpellAttack:
                    {
                        if (!m_PhaseRiderless)
                        {
                            DoSpellAttackIfReady(eSpells::StampVisual);
                            events.ScheduleEvent(eEvents::DoSpellAttack, 2000);
                            EntryCheckPredicate l_Pred(eCreatures::AttumenTheHuntsman);
                            summons.DoAction(ACTION_5, l_Pred);
                        }
                        break;
                    }
                    default:
                        break;
                }

                if (m_PhaseRiderless)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_rtk_midnightAI (p_Creature);
        }
};

/// Attumen the Huntsman - 114262
class npc_attumen_the_huntsman : public CreatureScript
{
    public:
        npc_attumen_the_huntsman() : CreatureScript("npc_attumen_the_huntsman") { }

        struct npc_attumen_the_huntsmanAI : public ScriptedAI
        {
            npc_attumen_the_huntsmanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eTalks
            {
                Aggro                   = 0,
                TalkIntangiblePresence  = 1,
                Riderless               = 2,
                Death                   = 3,
                MightyStomp             = 4,
                BackOnHorse             = 5,
                SpectralCharge          = 6,
                PlayerKilled            = 7,
                Wipe                    = 8,
                ConvIntroTalk1          = 9,
                ConvIntroTalk2          = 10
            };

            enum eSpells
            {
                MountedStrike   = 227636,
                AnimDismounted  = 227474, ///< Bug anim
                MortalStrike    = 227493,
                Stamp           = 227638,
                SharedSuffering = 228852
            };

            enum eEvents
            {
                EventMortalStrike = 1,
                EventSharedSuffering,
                EventCheckVictim,
                DoSpellAttack
            };

            bool m_Ride;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.76f;
            }

            void Reset() override
            {
                events.Reset();
                m_Ride = true;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                events.ScheduleEvent(eEvents::EventCheckVictim, 1000);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim->IsPlayer())
                {
                    InstanceScript* l_InstanceScript = me->GetInstanceScript();
                    if (l_InstanceScript && l_InstanceScript->IsWipe())
                        Talk(eTalks::Wipe);
                    else
                        Talk(eTalks::PlayerKilled);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (me->isAlive())
                            Talk(eTalks::Riderless);
                        m_Ride = false;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        events.ScheduleEvent(eEvents::EventMortalStrike, 10000);
                        events.ScheduleEvent(eEvents::EventSharedSuffering, 18000);
                        break;
                    }
                    case ACTION_2:
                    {
                        m_Ride = true;
                        events.Reset();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Talk(eTalks::BackOnHorse);
                        if (Unit* l_Owner = me->GetAnyOwner())
                            DoCast(l_Owner, 46598, true); ///< Ride veh
                        break;
                    }
                    case ACTION_3:
                    {
                        if (me->isAlive())
                            Talk(eTalks::TalkIntangiblePresence);
                        break;
                    }
                    case ACTION_4:
                    {
                        if (me->isAlive())
                            Talk(eTalks::MightyStomp);
                        break;
                    }
                    case ACTION_5:
                    {
                        events.ScheduleEvent(eEvents::DoSpellAttack, 1000);
                        break;
                    }
                    case ACTION_6:
                    {
                        Talk(eTalks::SpectralCharge);
                        break;
                    }
                    case ACTION_7:
                    {
                        Talk(eTalks::PlayerKilled);
                        break;
                    }
                    case ACTION_8:
                    {
                        Talk(eTalks::Wipe);
                        break;
                    }
                    case ACTION_9:
                    {
                        Talk(eTalks::ConvIntroTalk1);
                        AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::ConvIntroTalk2);
                        });
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::MountedStrike)
                    me->CastSpell(p_Target, eSpells::Stamp, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->GetDistance(me->GetHomePosition()) > 45.0f || me->GetPositionZ() >= 54.0f)
                    EnterEvadeMode();

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (me->IsStopped())
                    {
                        if (Spell const* l_CurrentGenericSpell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                        {
                            if (SpellInfo const* l_SpellInfo = l_CurrentGenericSpell->GetSpellInfo())
                            {
                                if (l_SpellInfo->Id == eSpells::SharedSuffering)
                                    me->GetMotionMaster()->MoveChase(me->getVictim());
                            }
                        }
                    }
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventMortalStrike:
                    {
                        DoCastVictim(eSpells::MortalStrike);
                        events.ScheduleEvent(eEvents::EventMortalStrike, 16000);
                        break;
                    }
                    case eEvents::EventSharedSuffering:
                    {
                        DoCast(eSpells::SharedSuffering);
                        break;
                    }
                    case eEvents::EventCheckVictim:
                    {
                        if (m_Ride)
                        {
                            if (Unit* l_Owner = me->GetAnyOwner())
                            {
                                if (l_Owner->getVictim() && me->getVictim() && l_Owner->getVictim() != me->getVictim())
                                {
                                    DoResetThreat();
                                    me->AddThreat(l_Owner->getVictim(), 10000.0f);
                                }
                            }
                        }
                        events.ScheduleEvent(eEvents::EventCheckVictim, 1000);
                        break;
                    }
                    case eEvents::DoSpellAttack:
                    {
                        if (me->GetVehicleCreatureBase())
                            DoSpellAttackIfReady(eSpells::MountedStrike, TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE);
                        break;
                    }
                    default:
                        break;
                }

                if (!me->GetVehicleCreatureBase())
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_attumen_the_huntsmanAI(p_Creature);
        }
};

/// Intangible Pressnce - 114315
class npc_rtk_intangible_pressence : public CreatureScript
{
    public:
        npc_rtk_intangible_pressence() : CreatureScript("npc_rtk_intangible_pressence") { }

        struct npc_rtk_intangible_pressenceAI : public ScriptedAI
        {
            npc_rtk_intangible_pressenceAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                m_Instance = me->GetInstanceScript();
            }

            enum eSpells
            {
                IntangiblePresenceDamage1   = 227415,
                IntangiblePresence          = 227404
            };

            InstanceScript* m_Instance;

            void Reset() override
            {
                events.ScheduleEvent(EVENT_1, 3000);
                events.ScheduleEvent(EVENT_2, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->CastSpell(me, eSpells::IntangiblePresenceDamage1, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                        events.ScheduleEvent(EVENT_1, 3000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (!me->GetVehicleBase() && m_Instance)
                        {
                            m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::IntangiblePresence);
                            me->DespawnOrUnsummon(100);
                            break;
                        }
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_intangible_pressenceAI(p_Creature);
        }
};

/// Intangible Presence - 227404
class spell_rtk_intangible_presence : public SpellScriptLoader
{
    public:
        spell_rtk_intangible_presence() : SpellScriptLoader("spell_rtk_intangible_presence") {}

        class spell_rtk_intangible_presence_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_intangible_presence_SpellScript);

            enum eSpells
            {
                IntangiblePresenceVisual1   = 227436,
                IntangiblePresenceVisual2   = 227437, ///< Random target
                IntangiblePresence          = 227404,
            };

            uint8 m_TargetCount = 0;
            uint8 m_RandTarget = 1;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                for (WorldObject* l_Target : p_Targets)
                {
                    if (l_Target->ToPlayer() && l_Target->ToPlayer()->HasAura(eSpells::IntangiblePresence))
                        l_Target->ToPlayer()->RemoveAurasDueToSpell(eSpells::IntangiblePresence);
                }

                if (p_Targets.size() > 1)
                    m_RandTarget = urand(1, p_Targets.size());
            }

            void HandleAfterHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                m_TargetCount++;

                if (m_RandTarget == m_TargetCount)
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::IntangiblePresenceVisual1, true);
                else
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::IntangiblePresenceVisual2, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_intangible_presence_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_rtk_intangible_presence_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_intangible_presence_SpellScript();
        }

        class spell_rtk_intangible_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_intangible_presence_AuraScript);

            enum eSpells
            {
                IntangiblePresenceDamage2   = 227419,
                IntangiblePresence          = 227404
            };

            void HandleDispel(DispelInfo* p_DispelInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* l_Owner = GetUnitOwner())
                {
                    if (Vehicle* l_Vehicle = l_Owner->GetVehicleKit())
                    {
                        if (!l_Vehicle->GetPassenger(0))
                        {
                            p_DispelInfo->SetRemovedCharges(0);
                            l_Owner->CastSpell(l_Owner, eSpells::IntangiblePresenceDamage2, true, nullptr, nullptr, l_Caster->GetGUID());
                        }
                        else
                        {
                            l_Caster->AddDelayedEvent([l_Caster]() -> void
                            {
                                std::list<Player*> l_PlayerList;
                                GetPlayerListInGrid(l_PlayerList, l_Caster, 100.0f);
                                for (Player* l_Player : l_PlayerList)
                                    l_Player->RemoveAurasDueToSpell(eSpells::IntangiblePresence);
                            }, 10);
                        }
                    }
                }
            }

            void Register() override
            {
                OnDispel += AuraDispelFn(spell_rtk_intangible_presence_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_intangible_presence_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_attumen_the_huntsman()
{
    /// Boss
    new boss_rtk_midnight();

    /// Creatures
    new npc_attumen_the_huntsman();
    new npc_rtk_intangible_pressence();

    /// Spell
    new spell_rtk_intangible_presence();
}
#endif
