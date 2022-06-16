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
    SAY_AGGRO,
    SAY_SAP_SOUL,
    SAY_SAP_SOUL_WARN,
    SAY_PRISON,
    SAY_TELEPORT_WARN,
    SAY_SLAY,
    SAY_WIPE,
    SAY_DEATH
};

enum Spells
{
    SPELL_SAP_SOUL              = 200905,
    SPELL_MYTHIC_SAP_SOUL       = 206303,
    SPELL_SAP_SOUL_DEBUFF       = 200904,
    SPELL_TELEPORT              = 200898,
    SPELL_OPEN_PRISONS_AURA     = 196821,
    SPELL_OPEN_PRISONS_CENTR    = 196993,
    SPELL_OPEN_PRISONS_LEFT     = 197009,
    SPELL_OPEN_PRISONS_RIGHT    = 197010,

    /// Other
    SPELL_TORMENT               = 206470,
    SPELL_SUMMON_TORMENTING_ORB = 214970,
    SPELL_INQUISITIVE_STARE     = 212564,
    SPELL_SKAGGLDRYNK           = 188023,

    /// Grimoira
    SPELL_MORTAR_MISSILE        = 216317,
    SPELL_A_MOTHERS_LOVE_CAST   = 194064,
    SPELL_A_MOTHERS_LOVE_AT     = 194070,
    SPELL_A_MOTHERS_LOVE_DOT    = 194071,
    SPELL_MESMERIZE             = 216451,
    SPELL_DRAIN_PERIODIC        = 216435,
    SPELL_INVIGORATED           = 216484,
    SPELL_GRAVITY_CAST          = 194074,
    SPELL_GRAVITY_GRIP          = 194081
};

enum eEvents
{
    EVENT_SAP_SOUL          = 1,
    EVENT_TELEPORT          = 2,
    EVENT_OPEN_PRISON       = 3,
    EVENT_SUM_PRISON_ADDS   = 4,
    EVENT_TORMENTING_ORB    = 5
};

Position const g_TeleportPos[4] =
{
    { 4423.45f, -423.79f, 119.87f, 2.33f }, ///< Orks
    { 4478.04f, -423.82f, 119.87f, 0.75f }, ///< Mogu
    { 4478.52f, -478.88f, 119.87f, 5.44f }, ///< Void
    { 4423.03f, -478.81f, 119.87f, 3.90f }  ///< Corruption
};

Position const g_SummonPos[14] =
{
    { 4417.47f, -417.87f, 120.39f, 5.50f }, ///< Orks centr - 99704
    { 4405.09f, -418.98f, 120.15f, 4.65f }, ///< Orks left  - 99644
    { 4418.89f, -405.60f, 120.23f, 0.08f }, ///< Orks right - 99645
    { 4483.31f, -418.44f, 119.96f, 3.97f }, ///< Mogu centr - 99675
    { 4481.22f, -405.33f, 119.96f, 3.13f }, ///< Mogu left  - 99676
    { 4496.58f, -419.27f, 120.39f, 4.91f }, ///< Mogu right - 99676
    { 4484.36f, -484.62f, 120.31f, 2.36f }, ///< Void centr - 99655
    { 4496.40f, -482.99f, 120.15f, 1.47f }, ///< Void left  - 99657
    { 4483.17f, -496.78f, 120.15f, 3.25f }, ///< Void right - 99651
    { 4419.79f, -496.68f, 120.23f, 0.11f }, ///< Corruption - 99678
    { 4420.24f, -496.51f, 120.23f, 0.00f }, ///< Corruption - 99678
    { 4418.33f, -483.19f, 120.40f, 0.75f }, ///< Corruption - 99678
    { 4417.92f, -483.75f, 120.40f, 0.77f }, ///< Corruption - 99678
    { 4404.61f, -481.96f, 120.24f, 1.70f }  ///< Corruption - 99678
};

Position const g_CenterPos = { 4450.84f, -451.134f, 118.95f, 4.6774f };

/// Inquisitor Tormentorum - 96015
class boss_inquisitor_tormentorum : public CreatureScript
{
    public:
        boss_inquisitor_tormentorum() : CreatureScript("boss_inquisitor_tormentorum") { }

        struct boss_inquisitor_tormentorumAI : public BossAI
        {
            boss_inquisitor_tormentorumAI(Creature* p_Creature) : BossAI(p_Creature, DATA_TORMENTORUM) { }

            uint8 m_HealthPct;
            uint8 m_TeleportRand[2];
            uint8 m_TeleportCount;

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                m_HealthPct = 70;

                summons.DespawnAll();

                m_TeleportCount = 0;

                CoordSelection();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    DoCast(me, SPELL_TORMENT, true);
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                m_TeleportCount = 0;

                me->InterruptNonMeleeSpells(true);

                Talk(SAY_AGGRO);
                _EnterCombat();

                events.ScheduleEvent(EVENT_SAP_SOUL, 13000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_TORMENTING_ORB, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                RemoveEncounterAuras();
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                Talk(SAY_WIPE);

                RemoveEncounterAuras();
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(SAY_SLAY);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if ((p_SpellInfo->Id == SPELL_SAP_SOUL || p_SpellInfo->Id == SPELL_MYTHIC_SAP_SOUL) && p_Target->IsPlayer())
                    sCreatureTextMgr->SendChat(me, SAY_SAP_SOUL_WARN, p_Target->GetGUID(), CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPctDamaged(m_HealthPct, p_Damage) && m_HealthPct >= 40)
                {
                    m_HealthPct -= 30;
                    DoStopAttack();
                    me->StopMoving();
                    events.ScheduleEvent(EVENT_TELEPORT, 500);
                }
            }

            void CoordSelection()
            {
                std::list<uint8> l_RandList;
                for (uint8 l_I = 0; l_I < 4; l_I++)
                    l_RandList.push_back(l_I);

                JadeCore::Containers::RandomResizeList(l_RandList, 2);

                for (std::list<uint8>::iterator l_Iter = l_RandList.begin(); l_Iter != l_RandList.end(); ++l_Iter)
                {
                    m_TeleportRand[m_TeleportCount] = *l_Iter;
                    m_TeleportCount++;
                }
            }

            void OpenPrisonSumAdds()
            {
                std::list<GameObject*> l_GoList;

                for (uint32 l_I = GO_CAGE_1; l_I < GO_CAGE_12 + 1; l_I++)
                    me->GetGameObjectListWithEntryInGrid(l_GoList, l_I, 30);

                for (std::list<GameObject*>::iterator l_Iter = l_GoList.begin(); l_Iter != l_GoList.end(); ++l_Iter)
                    (*l_Iter)->SetGoState(GO_STATE_ACTIVE);

                switch (m_TeleportRand[m_TeleportCount])
                {
                    case 0:
                    {
                        me->SummonCreature(NPC_SHADOWMOON_WARLOCK, g_SummonPos[0]);
                        me->SummonCreature(NPC_FELGUARD_ANNIHILATOR, g_SummonPos[1]);
                        me->SummonCreature(NPC_SHADOWMOON_TECHNICIAN, g_SummonPos[2]);
                        break;
                    }
                    case 1:
                    {
                        me->SummonCreature(NPC_ENORMOUS_STONE_QUILEN, g_SummonPos[3]);
                        me->SummonCreature(NPC_MOGUSHAN_SECRET_KEEPER, g_SummonPos[4]);
                        me->SummonCreature(NPC_MOGUSHAN_SECRET_KEEPER, g_SummonPos[5]);
                        break;
                    }
                    case 2:
                    {
                        me->SummonCreature(NPC_VOID_TOUCHED_JUGGERNAUT, g_SummonPos[6]);
                        me->SummonCreature(NPC_DERANGED_MINDFLAYER, g_SummonPos[7]);
                        me->SummonCreature(NPC_FACELESS_VOIDCASTER, g_SummonPos[8]);
                        break;
                    }
                    case 3:
                    {
                        for (uint8 l_I = 9; l_I < 14; l_I++)
                            me->SummonCreature(NPC_LINGERING_CORRUPTION, g_SummonPos[l_I]);

                        break;
                    }
                    default:
                        break;
                }

                m_TeleportCount++;
                me->SetReactState(REACT_AGGRESSIVE);
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
                    case EVENT_SAP_SOUL:
                    {
                        Talk(SAY_SAP_SOUL);

                        if (IsMythic())
                            DoCast(SPELL_MYTHIC_SAP_SOUL);
                        else
                            DoCast(SPELL_SAP_SOUL);

                        me->AddUnitState(UNIT_STATE_CASTING);
                        me->StopMoving();
                        events.ScheduleEvent(EVENT_SAP_SOUL, IsMythic() ? 15400 : 21500);
                        break;
                    }
                    case EVENT_TELEPORT:
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED);
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        me->GetMotionMaster()->Clear();
                        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                        me->CastSpell(g_TeleportPos[m_TeleportRand[m_TeleportCount]], SPELL_TELEPORT, false);
                        Talk(SAY_PRISON);
                        Talk(SAY_TELEPORT_WARN);
                        events.ScheduleEvent(EVENT_OPEN_PRISON, 2500);
                        break;
                    }
                    case EVENT_OPEN_PRISON:
                    {
                        me->SetFacingTo(g_TeleportPos[m_TeleportRand[m_TeleportCount]].GetOrientation());

                        DoCast(SPELL_OPEN_PRISONS_AURA);

                        AddTimedDelayedOperation(5100, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED);
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            me->GetMotionMaster()->Clear();

                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                AttackStart(l_Target);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Target);
                            }

                            AddTimedDelayedOperation(2000, [this]() -> void
                            {
                                me->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                            });
                        });

                        events.ScheduleEvent(EVENT_SUM_PRISON_ADDS, 5000);
                        break;
                    }
                    case EVENT_SUM_PRISON_ADDS:
                    {
                        OpenPrisonSumAdds();
                        break;
                    }
                    case EVENT_TORMENTING_ORB:
                    {
                        float l_D = frand(15.0f, 40.0f);
                        float l_A = frand(0.0f, 2.0f * M_PI);
                        float l_X = g_CenterPos.m_positionX + l_D * std::cos(l_A);
                        float l_Y = g_CenterPos.m_positionY + l_D * std::sin(l_A);
                        float l_Z = g_CenterPos.m_positionZ;

                        DoCast({ l_X, l_Y, l_Z, l_A }, SPELL_SUMMON_TORMENTING_ORB);
                        events.ScheduleEvent(EVENT_TORMENTING_ORB, 15000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void RemoveEncounterAuras()
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SAP_SOUL_DEBUFF);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_inquisitor_tormentorumAI(p_Creature);
        }
};

/// Tormenting Orb - 107114
class npc_tormentorum_tormenting_orb : public CreatureScript
{
    public:
        npc_tormentorum_tormenting_orb() : CreatureScript("npc_tormentorum_tormenting_orb") { }

        struct npc_tormentorum_tormenting_orbAI : public Scripted_NoMovementAI
        {
            npc_tormentorum_tormenting_orbAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(REACT_PASSIVE);

                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

                events.ScheduleEvent(EVENT_1, 100);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (events.ExecuteEvent() == EVENT_1)
                {
                    DoCastAOE(SPELL_INQUISITIVE_STARE, true);

                    events.ScheduleEvent(EVENT_1, 10000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tormentorum_tormenting_orbAI(p_Creature);
        }
};

/// Grimoira <Devourer of Entrails> - 105824
class npc_tormentorum_grimoira : public CreatureScript
{
    public:
        npc_tormentorum_grimoira() : CreatureScript("npc_tormentorum_grimoira") { }

        struct npc_tormentorum_grimoiraAI : public Scripted_NoMovementAI
        {
            npc_tormentorum_grimoiraAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();

                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                /// Disabled in mythic+ mode
                if (me->GetMap()->IsChallengeMode())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                events.ScheduleEvent(EVENT_1, 7000);
                events.ScheduleEvent(EVENT_2, 12000);
                events.ScheduleEvent(EVENT_3, 30000);
                events.ScheduleEvent(EVENT_4, 35000);
                events.ScheduleEvent(EVENT_5, 5000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_A_MOTHERS_LOVE_DOT:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        p_Target->ToPlayer()->KilledMonsterCredit(NPC_BILE_CREDIT);
                        break;
                    }
                    case SPELL_GRAVITY_CAST:
                    {
                        DoCast(p_Target, SPELL_GRAVITY_GRIP, true);
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
                    case EVENT_1:
                    {
                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            AddTimedDelayedOperation(l_I * 100, [this]() -> void
                            {
                                Position l_Pos = me->GetPosition();

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(10.0f, 30.0f), frand(0.0f, 2.0f * M_PI));

                                DoCast(l_Pos, SPELL_MORTAR_MISSILE, true);
                            });
                        }

                        events.ScheduleEvent(EVENT_1, 20000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_A_MOTHERS_LOVE_CAST);
                        events.ScheduleEvent(EVENT_2, 55000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(me, SPELL_MESMERIZE);
                        events.ScheduleEvent(EVENT_3, 30000);
                        break;
                    }
                    case EVENT_4:
                    {
                        DoCast(me, SPELL_DRAIN_PERIODIC);
                        events.ScheduleEvent(EVENT_4, 30000);
                        break;
                    }
                    case EVENT_5:
                    {
                        DoCast(me, SPELL_GRAVITY_CAST);
                        events.ScheduleEvent(EVENT_5, 5000);
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
            return new npc_tormentorum_grimoiraAI(p_Creature);
        }
};

/// Void-Touched Juggernaut - 99655
class npc_tormentorum_void_touched_juggernaut : public CreatureScript
{
    public:
        npc_tormentorum_void_touched_juggernaut() : CreatureScript("npc_tormentorum_void_touched_juggernaut") { }

        enum eSpells
        {
            SpellEnrage = 212561,
            SpellCleave = 40504
        };

        struct npc_tormentorum_void_touched_juggernautAI : public ScriptedAI
        {
            npc_tormentorum_void_touched_juggernautAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Enrage;

            void Reset() override
            {
                m_Enrage = false;
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_Enrage && me->HealthBelowPctDamaged(50, p_Damage))
                {
                    DoCast(me, eSpells::SpellEnrage, true);
                    m_Enrage = true;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                DoCastVictim(eSpells::SpellCleave);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tormentorum_void_touched_juggernautAI(p_Creature);
        }
};

/// Deranged Mindflayer - 99657
class npc_tormentorum_deranged_mindflayer : public CreatureScript
{
    public:
        npc_tormentorum_deranged_mindflayer() : CreatureScript("npc_tormentorum_deranged_mindflayer") { }

        enum eMindSpells
        {
            SpellMindFlay           = 196508,
            SpellFrighteningShout   = 201488,
            SpellBattleShout        = 26043
        };

        enum eMindEvents
        {
            EventMindFlay = 1,
            EventShout,
            EventBattleShout
        };

        struct npc_tormentorum_deranged_mindflayerAI : public ScriptedAI
        {
            npc_tormentorum_deranged_mindflayerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eMindEvents::EventMindFlay, 6500);
                events.ScheduleEvent(eMindEvents::EventShout, 28500);
                events.ScheduleEvent(eMindEvents::EventBattleShout, 46500);
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
                    case eMindEvents::EventMindFlay:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eMindSpells::SpellMindFlay);

                        events.ScheduleEvent(eMindEvents::EventMindFlay, 12000);
                        break;
                    }
                    case eMindEvents::EventShout:
                    {
                        DoCast(me, eMindSpells::SpellFrighteningShout);
                        events.ScheduleEvent(eMindEvents::EventShout, 25000);
                        break;
                    }
                    case eMindEvents::EventBattleShout:
                    {
                        DoCast(me, eMindSpells::SpellBattleShout);
                        events.ScheduleEvent(eMindEvents::EventBattleShout, 60000);
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
            return new npc_tormentorum_deranged_mindflayerAI(p_Creature);
        }
};

/// Faceless Voidcaster - 99651
class npc_tormentorum_faceless_voidcaster : public CreatureScript
{
    public:
        npc_tormentorum_faceless_voidcaster() : CreatureScript("npc_tormentorum_faceless_voidcaster") { }

        enum eFacelessSpells
        {
            SpellShadowCrashMissile = 199915,
            SpellVoidbolt           = 214771,
            SpellBattleShout        = 26043
        };

        enum eFacelessEvents
        {
            EventShadowCrash = 1,
            EventVoidbolt,
            EventBattleShout
        };

        struct npc_tormentorum_faceless_voidcasterAI : public ScriptedAI
        {
            npc_tormentorum_faceless_voidcasterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eFacelessEvents::EventShadowCrash, 16000);
                events.ScheduleEvent(eFacelessEvents::EventVoidbolt, 8000);
                events.ScheduleEvent(eFacelessEvents::EventBattleShout, 46500);
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
                    case eFacelessEvents::EventShadowCrash:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eFacelessSpells::SpellShadowCrashMissile, true);

                        events.ScheduleEvent(eFacelessEvents::EventShadowCrash, 14000);
                        break;
                    }
                    case eFacelessEvents::EventVoidbolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eFacelessSpells::SpellVoidbolt);

                        events.ScheduleEvent(eFacelessEvents::EventVoidbolt, 12000);
                        break;
                    }
                    case eFacelessEvents::EventBattleShout:
                    {
                        DoCast(me, eFacelessSpells::SpellBattleShout);
                        events.ScheduleEvent(eFacelessEvents::EventBattleShout, 60000);
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
            return new npc_tormentorum_faceless_voidcasterAI(p_Creature);
        }
};

/// Mogu'Shan Secret Keeper - 99676
class npc_tormentorum_mogushan_secret_keeper : public CreatureScript
{
    public:
        npc_tormentorum_mogushan_secret_keeper() : CreatureScript("npc_tormentorum_mogushan_secret_keeper") { }

        enum eKeeperSpells
        {
            SpellStoneBlock     = 204905,
            SpellFleshToStone   = 203685
        };

        enum eKeeperEvents
        {
            EventStoneBlock = 1,
            EventFleshToStone
        };

        struct npc_tormentorum_mogushan_secret_keeperAI : public ScriptedAI
        {
            npc_tormentorum_mogushan_secret_keeperAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eKeeperEvents::EventStoneBlock, 5000);
                events.ScheduleEvent(eKeeperEvents::EventFleshToStone, 8000);
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
                    case eKeeperEvents::EventStoneBlock:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eKeeperSpells::SpellStoneBlock);

                        events.ScheduleEvent(eKeeperEvents::EventStoneBlock, 5000);
                        break;
                    }
                    case eKeeperEvents::EventFleshToStone:
                    {
                        DoCast(me, eKeeperSpells::SpellFleshToStone, true);
                        events.ScheduleEvent(eKeeperEvents::EventFleshToStone, 13000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tormentorum_mogushan_secret_keeperAI(p_Creature);
        }
};

/// Lingering Corruption - 99678
class npc_tormentorum_lingering_corruption : public CreatureScript
{
    public:
        npc_tormentorum_lingering_corruption() : CreatureScript("npc_tormentorum_lingering_corruption") { }

        enum eCorruptionSpells
        {
            SpellCorruptedTouch = 206019
        };

        struct npc_tormentorum_lingering_corruptionAI : public ScriptedAI
        {
            npc_tormentorum_lingering_corruptionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eCorruptionSpells::SpellCorruptedTouch, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tormentorum_lingering_corruptionAI(p_Creature);
        }
};

/// Open Prisons - 196992
class spell_tormentorum_open_prisons : public SpellScriptLoader
{
    public:
        spell_tormentorum_open_prisons() : SpellScriptLoader("spell_tormentorum_open_prisons") { }

        class spell_tormentorum_open_prisons_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tormentorum_open_prisons_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster() || !GetHitGObj())
                    return;

                switch (GetHitGObj()->GetEntry())
                {
                    case GO_CAGE_2:
                    case GO_CAGE_5:
                    case GO_CAGE_8:
                    case GO_CAGE_11:
                    {
                        GetCaster()->CastSpell(GetHitGObj(), SPELL_OPEN_PRISONS_CENTR, true);
                        break;
                    }
                    case GO_CAGE_1:
                    case GO_CAGE_4:
                    case GO_CAGE_7:
                    case GO_CAGE_12:
                    {
                        GetCaster()->CastSpell(GetHitGObj(), SPELL_OPEN_PRISONS_LEFT, true);
                        break;
                    }
                    case GO_CAGE_3:
                    case GO_CAGE_6:
                    case GO_CAGE_10:
                    case GO_CAGE_9:
                    {
                        GetCaster()->CastSpell(GetHitGObj(), SPELL_OPEN_PRISONS_RIGHT, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tormentorum_open_prisons_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tormentorum_open_prisons_SpellScript();
        }
};

/// Inquisitive Stare - 212564
class spell_tormentorum_inquisitive_stare : public SpellScriptLoader
{
    public:
        spell_tormentorum_inquisitive_stare() : SpellScriptLoader("spell_tormentorum_inquisitive_stare") { }

        class spell_tormentorum_inquisitive_stare_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tormentorum_inquisitive_stare_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        if (l_Target->isInFront(l_Caster))
                            PreventDefaultAction();
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tormentorum_inquisitive_stare_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tormentorum_inquisitive_stare_AuraScript();
        }
};

/// A Mother's Love - 194064
class spell_tormentorum_a_mothers_love : public SpellScriptLoader
{
    public:
        spell_tormentorum_a_mothers_love() : SpellScriptLoader("spell_tormentorum_a_mothers_love") { }

        class spell_tormentorum_a_mothers_love_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tormentorum_a_mothers_love_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, SPELL_A_MOTHERS_LOVE_AT, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tormentorum_a_mothers_love_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tormentorum_a_mothers_love_AuraScript();
        }
};

/// Drain - 216433
class spell_tormentorum_drain : public SpellScriptLoader
{
    public:
        spell_tormentorum_drain() : SpellScriptLoader("spell_tormentorum_drain") { }

        class spell_tormentorum_drain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tormentorum_drain_AuraScript);

            int32 m_Amount = 0;

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                m_Amount = p_Amount;
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                int32 l_Amount = p_AurEff->GetAmount();

                l_Amount += CalculatePct(m_Amount, 10);

                const_cast<AuraEffect*>(p_AurEff)->SetAmount(l_Amount);

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        bool l_New = false;
                        if (!l_Target->HasAura(SPELL_INVIGORATED))
                        {
                            l_New = true;
                            l_Caster->CastSpell(l_Target, SPELL_INVIGORATED, true);
                        }

                        for (uint8 l_I = 0; l_I < 2; ++l_I)
                        {
                            if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(SPELL_INVIGORATED, l_I))
                                l_AurEff->SetAmount(l_New ? 10 : l_AurEff->GetAmount() + 10);
                        }
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_tormentorum_drain_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tormentorum_drain_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tormentorum_drain_AuraScript();
        }
};

/// Corrupted Touch - 206019
class spell_tormentorum_corrupted_touch : public SpellScriptLoader
{
    public:
        spell_tormentorum_corrupted_touch() : SpellScriptLoader("spell_tormentorum_corrupted_touch") { }

        class spell_tormentorum_corrupted_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tormentorum_corrupted_touch_AuraScript);

            enum eTriggerSpell
            {
                SpellCorruptedTouch = 206020
            };

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eTriggerSpell::SpellCorruptedTouch, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tormentorum_corrupted_touch_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tormentorum_corrupted_touch_AuraScript();
        }
};

/// Grimoira's Door - 246113
class go_tormentorum_grimoiras_door : public GameObjectScript
{
    public:
        go_tormentorum_grimoiras_door() : GameObjectScript("go_tormentorum_grimoiras_door") { }

        struct go_tormentorum_grimoiras_doorAI : public GameObjectAI
        {
            go_tormentorum_grimoiras_doorAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            bool m_Activated;

            uint32 m_CheckPlayersTimer = 0;

            void Reset() override
            {
                m_Activated = false;

                m_CheckPlayersTimer = 1000;
            }

            void UpdateAI(uint32 p_Diff) override
            {
                if (m_Activated)
                    return;

                /// Optional boss isn't reachable until the death of the two first ones
                if (InstanceScript* l_Instance = go->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(DATA_SALTHERIL) != DONE || l_Instance->GetBossState(DATA_TORMENTORUM) != DONE)
                        return;
                }

                if (m_CheckPlayersTimer)
                {
                    if (m_CheckPlayersTimer <= p_Diff)
                    {
                        std::list<Player*> l_PlayerList;
                        go->GetPlayerListInGrid(l_PlayerList, 15.0f);

                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player->IsPlayer())
                                continue;

                            if (l_Player->GetDistance(go) > 15.0f)
                                continue;

                            if (!l_Player->HasAura(SPELL_SKAGGLDRYNK))
                                continue;

                            m_Activated = true;

                            go->SetGoState(GO_STATE_ACTIVE);
                            return;
                        }

                        m_CheckPlayersTimer = 1000;
                    }
                    else
                        m_CheckPlayersTimer -= p_Diff;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_tormentorum_grimoiras_doorAI(p_GameObject);
        }
};

/// Sapped Soul - 200904
class spell_tormentorum_sapped_soul : public AuraScript
{
    PrepareAuraScript(spell_tormentorum_sapped_soul);

    void HandleOnProc(ProcEventInfo& p_EventInfo)
    {
        Unit* l_Owner = GetUnitOwner();

        if (!l_Owner || !l_Owner->ToPlayer() || !p_EventInfo.GetDamageInfo())
            return;

        SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();

        if (l_ProcSpell)
        {
            uint32 l_PlusCd = GetSpellInfo()->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS;
            l_Owner->ToPlayer()->AddSpellCooldown(l_ProcSpell->Id, 0, l_PlusCd + l_ProcSpell->RecoveryTime, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_tormentorum_sapped_soul::HandleOnProc);
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_inquisitor_tormentorum()
{
    /// Boss
    new boss_inquisitor_tormentorum();

    /// Creatures
    new npc_tormentorum_tormenting_orb();
    new npc_tormentorum_grimoira();
    new npc_tormentorum_void_touched_juggernaut();
    new npc_tormentorum_deranged_mindflayer();
    new npc_tormentorum_faceless_voidcaster();
    new npc_tormentorum_mogushan_secret_keeper();
    new npc_tormentorum_lingering_corruption();

    /// Spells
    new spell_tormentorum_open_prisons();
    new spell_tormentorum_inquisitive_stare();
    new spell_tormentorum_a_mothers_love();
    new spell_tormentorum_drain();
    new spell_tormentorum_corrupted_touch();
    RegisterAuraScript(spell_tormentorum_sapped_soul);

    /// GameObject
    new go_tormentorum_grimoiras_door();
}
#endif
