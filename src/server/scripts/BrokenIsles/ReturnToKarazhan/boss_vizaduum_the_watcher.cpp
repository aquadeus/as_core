////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

Position const g_CommandPos[3] =
{
    { 3397.11f, -2142.85f, 977.43f, 6.1f }, ///< Second bombardment
    { 3496.74f, -1959.66f, 996.70f, 5.8f },
    { 3800.63f, -1876.49f, 869.97f, 4.1f }  ///< Lasers
};

Position const g_HarvestPos[4] =
{
    { 3939.42f, -2007.51f, 926.93f, 0.0f },
    { 3974.58f, -2001.94f, 927.57f, 0.0f },
    { 3948.79f, -1980.31f, 926.93f, 0.0f },
    { 3964.87f, -2028.79f, 927.57f, 0.0f }
};

Position const g_GuardPos[8] =
{
    { 3471.18f, -2030.16f, 1039.81f, 1.11f },
    { 3454.24f, -2021.22f, 1039.92f, 0.86f },
    { 3491.31f, -1947.53f, 1039.72f, 1.76f },
    { 3483.37f, -1964.85f, 1039.72f, 1.01f },
    { 3473.50f, -1976.96f, 1039.72f, 4.01f },
    { 3500.32f, -1979.89f, 1039.72f, 4.66f },
    { 3504.10f, -1965.63f, 1039.72f, 1.14f },
    { 3512.49f, -1948.06f, 1039.72f, 0.47f }
};

Position const g_SpitterPos[4] =
{
    { 3453.50f, -2028.14f, 1065.27f, 0.0f },
    { 3454.75f, -1963.09f, 1059.92f, 0.0f },
    { 3526.33f, -2001.64f, 1059.92f, 0.0f },
    { 3532.10f, -1962.30f, 1067.61f, 0.0f }
};

/// Last Update 7.1.5 Build 23420
/// Viz'aduum the Watcher - 114790
class boss_vizaduum_the_watcher : public CreatureScript
{
    public:
        boss_vizaduum_the_watcher() : CreatureScript("boss_vizaduum_the_watcher") { }

        struct boss_vizaduum_the_watcherAI : public BossAI
        {
            boss_vizaduum_the_watcherAI(Creature* p_Creature) : BossAI(p_Creature, eData::Vizaduum) { }

            enum eTalks
            {
                Intro1                  = 0,
                Intro2                  = 1,
                Intro3                  = 2,
                TalkFelBeam             = 3,
                TalkBombardment         = 4,
                PlayerKilled            = 5,
                TalkChaoticShadows      = 6,
                Aggro                   = 7,
                FelBeamWarning          = 8,
                DemonicPortalWarning    = 9,
                Death                   = 10,
                TalkStabilizeRift       = 11,
                TalkMovingToShip        = 12,
                TalkMovingToShip2       = 13
            };

            enum eSpells
            {
                BurningBlast            = 229083,
                CommandFelBeam          = 229242,

                FelFlamesAT             = 229249, ///< Dmg 229250
                CommandBombardment      = 229284,
                DisintegrateFilter      = 229153,
                DisintegrateDamage      = 229151,
                ChaoticShadows          = 229159,

                SoulHarvest             = 229906,
                StabilizeRift           = 230084,
                StabilizeRiftVisual     = 230086,
                StabilizeRiftDummy      = 230089,

                LegionSpawn             = 230280,

                /// Change phase
                DemonicPortalSummon     = 229607,
                DemonicPortalAT         = 229610, ///< Fly Auras from player: 229599
                DemonicPortalScale      = 229598,

                /// Summons
                BombardmentAura1        = 229287,
                BombardmentAura2        = 229288,

                /// Khadgar & Medivh
                EnergyPortalSpawn       = 229651   ///< Medivh
            };

            enum eEvents
            {
                EventBurningBlast       = 1,
                EventCommandFelBeam     = 2,
                EventsDisintegrate      = 3,
                EventChaoticShadows     = 4,
                EventCommandBombardment = 5,
                EventDemonicPortal1     = 6,
                EventsDemonicPortal2    = 7,
                EventDemonicPortal3     = 8,
                EventCheckPlayer        = 9,
                EventStabilizeRift      = 10,
                EventSummonGuard        = 11
            };

            enum ePhases
            {
                Phase1      = 1,
                Phase2      = 2,
                Phase3      = 3,
                PhaseChange
            };

            uint64 m_CannonTargetGuid;
            uint8 m_HealthPct;
            uint16 m_SumGuardTimer;
            uint8 m_Phase;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.86f;
            }

            void Reset() override
            {
                _Reset();
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                me->SetReactState(REACT_AGGRESSIVE);
                m_HealthPct = 66;
                m_SumGuardTimer = 25000;

                for (uint8 l_I = 0; l_I < 3; l_I++)
                    me->SummonCreature(eCreatures::CommandShip, g_CommandPos[l_I]);

                for (uint8 l_I = 0; l_I < 4; l_I++)
                    Creature* l_Summon = me->SummonCreature(eCreatures::SoulHarvester, g_HarvestPos[l_I]);
            }

            void EnterCombat(Unit* p_Who) override
            {
                std::list<Creature*> l_SoulHarversters;
                me->GetCreatureListWithEntryInGrid(l_SoulHarversters, eCreatures::SoulHarvester, 400.0f);
                for (Creature* l_SoulHarverster : l_SoulHarversters)
                    l_SoulHarverster->AddThreat(p_Who, 1.0f);

                Talk(eTalks::Aggro);

                _EnterCombat();

                DefaultEvent(ePhases::Phase1);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(3657.5f, -2125.8f, 815.681f, 4.99f); ///< Home
                me->Respawn(true, false, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::Vizaduum, EncounterState::FAIL);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);
                _JustDied();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        Talk(eTalks::Intro1);

                        AddTimedDelayedOperation(11 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Intro2);
                        });

                        AddTimedDelayedOperation(22 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Intro3);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void DefaultEvent(uint8 p_Phase)
            {
                events.Reset();

                m_Phase = p_Phase;

                switch (p_Phase)
                {
                    case ePhases::Phase1:
                    {
                        events.ScheduleEvent(eEvents::EventBurningBlast, 2000);
                        events.ScheduleEvent(eEvents::EventCommandFelBeam, 6000);
                        events.ScheduleEvent(eEvents::EventsDisintegrate, 11000);
                        events.ScheduleEvent(eEvents::EventChaoticShadows, 16000);
                        events.ScheduleEvent(eEvents::EventCommandBombardment, 26000);
                        break;
                    }
                    case ePhases::Phase2:
                    {
                        events.ScheduleEvent(eEvents::EventBurningBlast, 2000);
                        events.ScheduleEvent(eEvents::EventsDisintegrate, 11000);
                        events.ScheduleEvent(eEvents::EventChaoticShadows, 16000);
                        events.ScheduleEvent(eEvents::EventCommandBombardment, 26000);
                        break;
                    }
                    case ePhases::Phase3:
                    {
                        events.ScheduleEvent(eEvents::EventBurningBlast, 2000);
                        events.ScheduleEvent(eEvents::EventsDisintegrate, 11000);
                        events.ScheduleEvent(eEvents::EventChaoticShadows, 16000);
                        events.ScheduleEvent(eEvents::EventSummonGuard, 2000);
                        break;
                    }
                    case ePhases::PhaseChange:
                    {
                        me->StopAttack();
                        events.ScheduleEvent(eEvents::EventDemonicPortal1, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                if (p_ID == eData::CannonTargetGUID)
                {
                    m_CannonTargetGuid = p_Guid;
                    Talk(eTalks::FelBeamWarning, p_Guid);
                }
            }

            uint64 GetGUID(int32 p_ID) override
            {
                if (p_ID == eData::CannonTargetGUID)
                    return m_CannonTargetGuid;

                return 0;
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == eData::VizaduumHPPhase)
                    return m_HealthPct;

                if (p_Type == 0)
                    return m_Phase;

                return 0;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case  eSpells::DisintegrateDamage:
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
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
                    case eSpells::CommandBombardment:
                    {
                        std::list<Creature*> l_CommandShips;
                        me->GetCreatureListWithEntryInGrid(l_CommandShips, eCreatures::CommandShip, 700.0f);
                        Unit* l_Target = nullptr;
                        switch (m_Phase)
                        {
                            case ePhases::Phase1:
                            {
                                for (Creature* l_Creature : l_CommandShips)
                                {
                                    if (l_Creature->GetPositionZ() > 970.0f && l_Creature->GetPositionZ() < 980.0f)
                                    {
                                        l_Target = l_Creature;
                                        break;
                                    }
                                }
                                break;
                            }
                            case ePhases::Phase2:
                            {
                                for (Creature* l_Creature : l_CommandShips)
                                {
                                    if (l_Creature->GetPositionZ() > 990.0f)
                                    {
                                        l_Target = l_Creature;
                                        break;
                                    }
                                }
                                break;
                            }
                            default:
                                break;
                        }

                        if (l_Target == nullptr)
                            break;

                        l_Target->CastSpell(l_Target, eSpells::BombardmentAura1, true);
                        l_Target->CastSpell(l_Target, eSpells::BombardmentAura2, true);
                        break;
                    }
                    case eSpells::StabilizeRift:
                    {
                        Talk(eTalks::TalkStabilizeRift);
                        p_Target->CastSpell(p_Target, eSpells::StabilizeRiftVisual, true); ///< Visual
                        break;
                    }
                    case eSpells::StabilizeRiftDummy: ///< Stabilize Rift
                    {
                        m_SumGuardTimer = 10000;
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(m_HealthPct))
                {
                    m_HealthPct == 66 ? m_HealthPct = 33 : m_HealthPct = 0;
                    DefaultEvent(ePhases::PhaseChange);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == WAYPOINT_MOTION_TYPE)
                {
                    if ((p_ID == 6 && m_HealthPct) || (p_ID == 7 && !m_HealthPct))
                    {
                        DoResetThreat();
                        SetFlyMode(false);
                        me->RemoveAurasDueToSpell(eSpells::DemonicPortalScale);
                        events.ScheduleEvent(eEvents::EventCheckPlayer, 1000);

                        me->SetDisableGravity(false);
                        me->SetCanFly(false);

                        if (!m_HealthPct)
                        {
                            me->SetFacingTo(1.1f);
                            events.ScheduleEvent(eEvents::EventStabilizeRift, 2000);

                            for (uint8 l_I = 2; l_I < 8; l_I++)
                                me->SummonCreature(eCreatures::FelguardSentry, g_GuardPos[l_I]);

                            for (uint8 l_I = 0; l_I < 4; l_I++)
                                me->SummonCreature(eCreatures::ShadowSpitter, g_SpitterPos[l_I]);
                        }
                        else
                            me->SetFacingTo(5.61f);
                    }
                }
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
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
                    case eEvents::EventBurningBlast:
                    {
                        DoCastVictim(eSpells::BurningBlast);
                        events.ScheduleEvent(eEvents::EventBurningBlast, 5000);
                        break;
                    }
                    case eEvents::EventCommandFelBeam:
                    {
                        m_CannonTargetGuid = 0;
                        Talk(eTalks::TalkFelBeam);
                        DoCast(eSpells::CommandFelBeam);
                        events.ScheduleEvent(eEvents::EventCommandFelBeam, 40000);
                        break;
                    }
                    case eEvents::EventsDisintegrate:
                    {
                        DoCast(eSpells::DisintegrateFilter);
                        events.ScheduleEvent(eEvents::EventsDisintegrate, 11000);
                        break;
                    }
                    case eEvents::EventChaoticShadows:
                    {
                        Talk(eTalks::TalkChaoticShadows);
                        DoCast(eSpells::ChaoticShadows);
                        events.ScheduleEvent(eEvents::EventChaoticShadows, 30000);
                        break;
                    }
                    case eEvents::EventCommandBombardment:
                    {
                        Talk(eTalks::TalkBombardment);
                        DoCast(eSpells::CommandBombardment);
                        events.ScheduleEvent(eEvents::EventCommandBombardment, 25000);
                        break;
                    }
                    case eEvents::EventDemonicPortal1:
                    {
                        ///DoCast(me, SPELL_DEMONIC_PORTAL_SUM, true);
                        /// HACK!!!
                        float l_X, l_Y, l_Z;
                        me->GetClosePoint(l_X, l_Y, l_Z, me->GetObjectSize(), 4.0f);
                        me->SummonCreature(eCreatures::DemonicPortal, l_X, l_Y, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10000);
                        Talk(eTalks::DemonicPortalWarning);
                        events.ScheduleEvent(eEvents::EventsDemonicPortal2, 500);
                        break;
                    }
                    case eEvents::EventsDemonicPortal2:
                    {
                        DoCast(me, eSpells::DemonicPortalAT, true);
                        events.ScheduleEvent(eEvents::EventDemonicPortal3, 3000);
                        break;
                    }
                    case eEvents::EventDemonicPortal3:
                    {
                        SetFlyMode(true);
                        DoCast(me, eSpells::DemonicPortalScale, true);

                        if (m_HealthPct == 33)
                            Talk(eTalks::TalkMovingToShip);
                        else if (m_HealthPct == 0)
                            Talk(eTalks::TalkMovingToShip2);

                        me->GetMotionMaster()->Clear();
                        me->SetDisableGravity(true);
                        me->SetCanFly(true);

                        if (m_HealthPct)
                            me->GetMotionMaster()->MovePath(me->GetEntry() * 100, false);
                        else
                            me->GetMotionMaster()->MovePath(me->GetEntry() * 100 + 1, false);

                        break;
                    }
                    case eEvents::EventCheckPlayer:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_NEAREST, 0, 30.0f, true))
                        {
                            me->InterruptNonMeleeSpells(false);
                            me->AddThreat(l_Target, 1000.0f);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DefaultEvent(m_HealthPct ? ePhases::Phase2 : ePhases::Phase3);
                        }
                        else
                        {
                            if (m_HealthPct)
                            {
                                std::vector<float> l_Orientations = { 5.61f, 5.50f, 5.72f };
                                me->SetOrientation(l_Orientations[urand(0, 2)]);
                                me->NearTeleportTo(*me);

                                Position l_Pos;
                                me->SimplePosXYRelocationByAngle(l_Pos, 100.0f, 0.0f);

                                me->CastSpell(l_Pos, eSpells::DisintegrateDamage, false);
                            }

                            events.ScheduleEvent(eEvents::EventCheckPlayer, 3000);
                        }

                        break;
                    }
                    case eEvents::EventStabilizeRift:
                    {
                        DoCast(eSpells::StabilizeRift);
                        break;
                    }
                    case eEvents::EventSummonGuard:
                    {
                        for (uint8 l_I = 0; l_I < 2; l_I++)
                        {
                            if (Creature* l_Summon = me->SummonCreature(eCreatures::FelguardSentry, g_GuardPos[l_I]))
                            {
                                l_Summon->CastSpell(l_Summon, eSpells::LegionSpawn, true);

                                if (l_Summon->IsAIEnabled)
                                    l_Summon->AI()->DoZoneInCombat(l_Summon, 200.0f);
                            }
                        }

                        events.ScheduleEvent(eEvents::EventSummonGuard, m_SumGuardTimer);
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
            return new boss_vizaduum_the_watcherAI(p_Creature);
        }
};

/// Fel Cannon - 115274
class npc_vizaduum_fel_cannon : public CreatureScript
{
    public:
        npc_vizaduum_fel_cannon() : CreatureScript("npc_vizaduum_fel_cannon") { }

        struct npc_vizaduum_fel_cannonAI : public ScriptedAI
        {
            npc_vizaduum_fel_cannonAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                FelBeamAT = 229244 ///< Dmg 229248
            };

            InstanceScript* m_Instance;
            uint64 m_TargetGuid;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (m_Instance)
                {
                    if (Creature* boss = m_Instance->instance->GetCreature(m_Instance->GetData64(eCreatures::VizaduumTheWatcher)))
                        m_TargetGuid = boss->GetAI()->GetGUID(eData::CannonTargetGUID);
                }

                events.ScheduleEvent(EVENT_1, 1000);
                events.ScheduleEvent(EVENT_2, 2000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Summoner = me->GetAnyOwner())
                            l_Summoner->CastSpell(me, eSpells::FelBeamAT, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Player* l_Player = Unit::GetPlayer(*me, m_TargetGuid))
                            me->GetMotionMaster()->MovePoint(1, l_Player->GetPosition());
                        else
                        {
                            me->DespawnOrUnsummon(100);
                            break;
                        }

                        events.ScheduleEvent(EVENT_2, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vizaduum_fel_cannonAI(p_Creature);
        }
};

/// Shadow Spitter - 115734
class npc_vizaduum_shadow_spitter : public CreatureScript
{
    public:
        npc_vizaduum_shadow_spitter() : CreatureScript("npc_vizaduum_shadow_spitter") { }

        struct npc_vizaduum_shadow_spitterAI : public ScriptedAI
        {
            npc_vizaduum_shadow_spitterAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                SetFlyMode(true);
            }

            enum eSpells
            {
                ShadowPhlegm = 230066
            };

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 3000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (!me->isInCombat())
                            DoZoneInCombat(me, 200.0f);

                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        {
                            me->SetFacingToObject(l_Target);
                            DoCast(l_Target, eSpells::ShadowPhlegm, true);
                        }

                        events.ScheduleEvent(EVENT_1, 5000);
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
            return new npc_vizaduum_shadow_spitterAI(p_Creature);
        }
};

/// Acquiring Target - 229241
class spell_vizaduum_acquiring_target : public SpellScriptLoader
{
    public:
        spell_vizaduum_acquiring_target() : SpellScriptLoader("spell_vizaduum_acquiring_target") { }

        class spell_vizaduum_acquiring_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vizaduum_acquiring_target_SpellScript);

            void HandleScript(SpellEffIndex /*p_EffIndex*/)
            {
                if (GetHitUnit())
                {
                    if (InstanceScript* l_Instance = GetHitUnit()->GetInstanceScript())
                    {
                        if (Creature* l_Boss = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::VizaduumTheWatcher)))
                        {
                            if (l_Boss->IsAIEnabled)
                                l_Boss->AI()->SetGUID(GetHitUnit()->GetGUID(), eData::CannonTargetGUID);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_vizaduum_acquiring_target_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vizaduum_acquiring_target_SpellScript();
        }
};

/// Bombardment - 229286
class spell_vizaduum_bombardment : public SpellScriptLoader
{
    public:
        spell_vizaduum_bombardment() : SpellScriptLoader("spell_vizaduum_bombardment") { }

        class spell_vizaduum_bombardment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vizaduum_bombardment_SpellScript);

            std::vector<G3D::Vector2> m_Corners =
            {
               { 3892.38f, -1969.68f },
               { 3911.80f, -1949.02f },
               { 4010.61f, -2022.70f },
               { 3985.04f, -2052.00f }
            };

            Position const m_Center = { 3915.81f, -1971.17f };

            void HandleTriggerEffect(SpellEffIndex /*p_EffIndex*/)
            {
                Position l_Pos;

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Boss = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::VizaduumTheWatcher)))
                    {
                        if (l_Boss->IsAIEnabled && l_Boss->GetAI()->GetData(eData::VizaduumHPPhase) == 66)
                            l_Pos = { 3666.64f, -2159.92f, 815.59f, 0.0f };
                        else if (l_Boss->IsAIEnabled && l_Boss->GetAI()->GetData(eData::VizaduumHPPhase) == 33)
                            l_Pos = { 3943.07f, -1993.89f, 927.00f, 0.0f };
                    }
                }

                WorldLocation l_Loc;
                l_Pos.SimplePosXYRelocationByAngle(l_Loc, frand(1.0f, 50.0f), frand(0.0f, 6.28f));
                SetExplTargetDest(l_Loc);
            }

            void HandleTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Boss = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::VizaduumTheWatcher)))
                    {
                        if (l_Boss->IsAIEnabled && l_Boss->GetAI()->GetData(eData::VizaduumHPPhase) == 33)
                        {
                            if (Spell* l_Spell = GetSpell())
                            {
                                if (l_Spell->GetDstTarget() && !IsInPolygon(l_Spell->GetDstTarget()))
                                {
                                    Position l_Target;
                                    m_Center.SimplePosXYRelocationByAngle(l_Target, frand(0.0f, 60.0f), frand(5.47f, 5.74f), true);
                                    l_Target.m_positionZ = l_Spell->GetDstTarget()->m_positionZ;
                                    l_Spell->SetDstTarget(&l_Target);
                                }
                            }
                        }
                    }
                }
            }

            bool IsInPolygon(Position const* p_Target)
            {
                uint32 l_Count = uint32(m_Corners.size());
                uint32 l_J = 0;
                bool l_Ok = false;

                /// Algorithm taken from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
                for (uint32 l_I = 0, l_J = l_Count - 1; l_I < l_Count; l_J = l_I++)
                {
                    if (((m_Corners[l_I].y > p_Target->m_positionY) != (m_Corners[l_J].y > p_Target->m_positionY)) &&
                        (p_Target->m_positionX < (m_Corners[l_J].x - m_Corners[l_I].x) * (p_Target->m_positionY - m_Corners[l_I].y) / (m_Corners[l_J].y - m_Corners[l_I].y) + m_Corners[l_I].x))
                    {
                        l_Ok = !l_Ok;
                    }
                }

                return l_Ok;
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_vizaduum_bombardment_SpellScript::HandleTriggerEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
                OnEffectLaunch += SpellEffectFn(spell_vizaduum_bombardment_SpellScript::HandleTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vizaduum_bombardment_SpellScript();
        }
};

/// Demonic Portal - 229599
class spell_vizaduum_demonic_portal : public SpellScriptLoader
{
    public:
        spell_vizaduum_demonic_portal() : SpellScriptLoader("spell_vizaduum_demonic_portal") { }

        class spell_vizaduum_demonic_portal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_vizaduum_demonic_portal_AuraScript);

            std::vector<G3D::Vector3> m_PathToP2 =
            {
                { 3665.96f, -2101.57f, 850.664f },
                { 3704.35f, -2085.03f, 862.971f },
                { 3819.63f, -2081.29f, 912.320f },
                { 3918.13f, -2056.37f, 950.223f },
                { 3949.94f, -2048.83f, 953.075f },
                { 3985.81f, -2051.15f, 946.149f },
                { 3997.09f, -2036.59f, 927.772f },
            };

            std::vector<G3D::Vector3> m_PathToP3 =
            {
                { 3904.61f, -1980.22f, 951.254f },
                { 3808.19f, -1965.78f, 974.025f },
                { 3727.91f, -1958.07f, 1009.46f },
                { 3661.55f, -1935.29f, 1037.07f },
                { 3581.87f, -1915.87f, 1069.55f },
                { 3533.31f, -1912.06f, 1061.04f },
                { 3518.32f, -1918.74f, 1040.66f },
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    uint8 l_EventPhaseHP = 0;

                    if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
                    {
                        if (Creature* l_Boss = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::VizaduumTheWatcher)))
                            l_EventPhaseHP = l_Boss->IsAIEnabled ? l_Boss->GetAI()->GetData(eData::VizaduumHPPhase) : 0;
                    }

                    l_Player->GetMotionMaster()->MoveIdle();
                    l_Player->SetSpeed(UnitMoveType::MOVE_RUN, 3.2f, true);
                    if (l_EventPhaseHP)
                        l_Player->GetMotionMaster()->MovePath(0, m_PathToP2.data(), m_PathToP2.size(), false);
                    else
                        l_Player->GetMotionMaster()->MovePath(0, m_PathToP3.data(), m_PathToP3.size(), false);
                }

            }

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                if (GetTarget())
                    GetTarget()->UpdateSpeed(UnitMoveType::MOVE_RUN, true);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_vizaduum_demonic_portal_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_vizaduum_demonic_portal_AuraScript::HandleAfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_vizaduum_demonic_portal_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Disintegrate - 229151
class spell_rtk_disintegrate : public SpellScriptLoader
{
    public:
        spell_rtk_disintegrate() : SpellScriptLoader("spell_rtk_disintegrate") { }

        class spell_rtk_disintegrate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_disintegrate_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                WorldLocation const* l_Dest = GetExplTargetDest();
                Unit* l_Caster = GetCaster();
                if (!l_Dest || !l_Caster || p_Targets.empty())
                    return;

                p_Targets.remove_if([l_Dest, l_Caster](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsInAxe(l_Caster, l_Dest, 12.0f))
                        return true;

                    return false;
                });
            }

            void HandleOnPrepare()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Caster = GetCaster();
                if (!l_Spell || !l_Caster)
                    return;

                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Creature || !l_Creature->IsAIEnabled)
                    return;

                if (l_Creature->GetAI()->GetData(0) != 1)
                    l_Spell->SetCastTime(4000);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_rtk_disintegrate_SpellScript::HandleOnPrepare);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_disintegrate_SpellScript::FilterTargets, EFFECT_0, TARGET_ENNEMIES_IN_CYLINDER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_disintegrate_SpellScript::FilterTargets, EFFECT_1, TARGET_ENNEMIES_IN_CYLINDER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_disintegrate_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Disintegrate - 229153
class spell_rtk_disintegrate_filter : public SpellScriptLoader
{
    public:
        spell_rtk_disintegrate_filter() : SpellScriptLoader("spell_rtk_disintegrate_filter") { }

        class spell_rtk_disintegrate_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_disintegrate_filter_SpellScript);

            enum eSpells
            {
                Disintegrate = 229151
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (p_Targets.empty() || !l_Caster)
                    return;

                JadeCore::RandomResizeList(p_Targets, 1);

                l_Caster->SetFacingToObject(p_Targets.front());
                l_Caster->NearTeleportTo(*l_Caster);
                l_Caster->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                Position l_Pos;
                l_Caster->SimplePosXYRelocationByAngle(l_Pos, 100.0f, 0.0f);

                l_Caster->CastSpell(l_Pos, eSpells::Disintegrate, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_disintegrate_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_disintegrate_filter_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Chaotic Shadows - 229159
class spell_rtk_chaotic_shadows : public SpellScriptLoader
{
    public:
        spell_rtk_chaotic_shadows() : SpellScriptLoader("spell_rtk_chaotic_shadows") { }

        enum eSpells
        {
            SpellExplosiveShadows = 229160
        };

        class spell_rtk_chaotic_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_chaotic_shadows_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Targets.empty())
                    return;

                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Creature || !l_Creature->IsAIEnabled)
                    return;

                uint32 l_Targets = l_Creature->GetAI()->GetData(0);
                if (l_Targets > 3)
                {
                    p_Targets.clear();
                    return;
                }

                JadeCore::RandomResizeList(p_Targets, l_Targets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_chaotic_shadows_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_chaotic_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_chaotic_shadows_AuraScript);

            void HandleAfterEffRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                AuraRemoveMode  l_RemoveMode = GetTargetApplication()->GetRemoveMode();

                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL &&
                    l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                l_Owner->CastSpell(l_Owner, eSpells::SpellExplosiveShadows, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_chaotic_shadows_AuraScript::HandleAfterEffRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_chaotic_shadows_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_chaotic_shadows_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Command Fel Beam - 229242
class spell_rtk_command_fel_beam : public SpellScriptLoader
{
    public:
        spell_rtk_command_fel_beam() : SpellScriptLoader("spell_rtk_command_fel_beam") { }

        class spell_rtk_command_fel_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_command_fel_beam_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_RealTarget = nullptr;
                for (WorldObject* l_Target : p_Targets)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    if (l_UnitTarget->GetPositionZ() < 860.0f || l_UnitTarget->GetPositionZ() > 900.0f)
                        continue;

                    l_RealTarget = l_UnitTarget;
                    break;
                }

                if (!l_RealTarget)
                    return;

                p_Targets.clear();
                p_Targets.push_back(l_RealTarget);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_command_fel_beam_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_command_fel_beam_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_command_fel_beam_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Felguard Sentry - 115730
class npc_rtk_felguard_sentry : public CreatureScript
{
    public:
        npc_rtk_felguard_sentry() : CreatureScript("npc_rtk_felguard_sentry") { }

        struct npc_rtk_felguard_sentryAI : public LegionCombatAI
        {
            npc_rtk_felguard_sentryAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                if (me->m_positionY > -2000.0f)
                    me->SetReactState(REACT_PASSIVE);
            }

            enum eEvents
            {
                EventCheckTargets = 2
            };

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                me->SetReactState(REACT_AGGRESSIVE);

                LegionCombatAI::DamageTaken(p_Source, p_Damage, p_SpellInfo);
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventCheckTargets:
                    {
                        if (Player* l_Player = me->FindNearestPlayer(20.0f))
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            return true;
                        }

                        return false;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_felguard_sentryAI(p_Creature);
        }
};

/// Archmage Khadgar - 115497
class npc_archmage_khadgar_vizaduum : public CreatureScript
{
    public:
        npc_archmage_khadgar_vizaduum() : CreatureScript("npc_archmage_khadgar_vizaduum") { }

        struct npc_archmage_khadgar_vizaduumAI : public ScriptedAI
        {
            npc_archmage_khadgar_vizaduumAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                RavenForm       = 229645,
                ArcaneCast      = 196978,
                EnergyPortal    = 229651
            };

            Position const m_MedivhSpawn = { 3456.75f, -2019.77f, 1039.97f, 0.34f };
            Position const m_KhadgarFlyDest = { 3470.38f, -2027.17f, 1039.97f, 1.29f };
            Position const m_KhadgarWalkDest = { 3468.93f, -2021.66f, 1039.88f };

            uint64 m_MedivhGUID = 0;

            void Reset() override
            {
                SetFlyMode(true);
                me->GetMotionMaster()->MovePoint(10, m_KhadgarFlyDest);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        me->RemoveAurasDueToSpell(eSpells::RavenForm);
                        DelayTalk(2, 0);
                        DelayTalk(22, 1);
                        DelayTalk(41, 2);
                        DelayTalk(75, 3);
                        events.ScheduleEvent(EVENT_2, 80000);

                        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                        {
                            if (Creature* l_Medivh = me->SummonCreature(eCreatures::MedivhVizaduum, m_MedivhSpawn))
                            {
                                l_Medivh->CastSpell(l_Medivh, eSpells::EnergyPortal, true);
                                m_MedivhGUID = l_Medivh->GetGUID();
                            }

                            SetFlyMode(false);
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(0, m_KhadgarWalkDest);
                        });

                        AddTimedDelayedOperation(6500, [this]() -> void
                        {
                            Creature* l_Medivh = Creature::GetCreature(*me, m_MedivhGUID);
                            if (!l_Medivh)
                                return;

                            me->SetFacingToObject(l_Medivh);
                        });

                        AddTimedDelayedOperation(80 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetFacingTo(1.075f);
                        });

                        AddTimedDelayedOperation(84 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetFacingTo(0.576f);
                        });

                        AddTimedDelayedOperation(85 * IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, eSpells::ArcaneCast, true);
                            me->SummonGameObject(eGameObjects::PortalDalaran, 3474.21f, -2019.22f, 1039.83f, 0, 0, 0, 0, 0, 1000);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_archmage_khadgar_vizaduumAI(p_Creature);
        }
};

/// Medivh - 114463
class npc_medivh_vizaduum : public CreatureScript
{
    public:
        npc_medivh_vizaduum() : CreatureScript("npc_medivh_vizaduum") { }

        struct npc_medivh_vizaduumAI : public ScriptedAI
        {
            npc_medivh_vizaduumAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                RavenForm = 229645
            };

            Position const m_MedivhWalkDest = { 3461.49f, -2017.88f, 1039.88f };
            Position const m_MedivhWalkAway = { 3457.22f, -2018.05f, 1039.88f };
            std::vector<G3D::Vector3> m_FlyAwayPath =
            {
                {3446.91f, -2025.49f, 1043.49f},
                {3437.86f, -2030.26f, 1041.30f},
                {3434.03f, -2034.71f, 1025.53f}
            };

            void Reset() override
            {
                DelayTalk(11, 0);
                DelayTalk(26, 1);
                DelayTalk(45, 2);
                DelayTalk(51, 3);
                DelayTalk(63, 4);

                AddTimedDelayedOperation(6 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(10, m_MedivhWalkDest);
                });

                AddTimedDelayedOperation(69 * IN_MILLISECONDS, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(11, m_MedivhWalkAway);
                });
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        Creature* l_Khadgar = me->FindNearestCreature(eCreatures::ArchmageKhagdarVizaduum, 30.0f);
                        if (!l_Khadgar)
                            break;

                        me->SetFacingToObject(l_Khadgar);
                        break;
                    }
                    case 11:
                    {
                        me->CastSpell(me, eSpells::RavenForm, true);
                        me->GetMotionMaster()->Clear();
                        SetFlyMode(true);
                        me->SetWalk(true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(0, m_FlyAwayPath.data(), m_FlyAwayPath.size());

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_medivh_vizaduumAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_vizaduum_the_watcher()
{
    /// Boss
    new boss_vizaduum_the_watcher();

    /// Creatures
    new npc_vizaduum_fel_cannon();
    new npc_vizaduum_shadow_spitter();
    new npc_rtk_felguard_sentry();
    new npc_archmage_khadgar_vizaduum();
    new npc_medivh_vizaduum();

    /// Spells
    new spell_vizaduum_acquiring_target();
    new spell_vizaduum_bombardment();
    new spell_vizaduum_demonic_portal();
    new spell_rtk_disintegrate();
    new spell_rtk_disintegrate_filter();
    new spell_rtk_chaotic_shadows();
    new spell_rtk_command_fel_beam();
}
#endif
