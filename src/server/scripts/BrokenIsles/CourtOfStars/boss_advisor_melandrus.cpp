////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "court_of_stars.hpp"

enum Says
{
    SAY_AGGRO       = 2,
    SAY_MAELSTROM   = 3,
    SAY_BLADE       = 4,
    SAY_KILL        = 5,
    SAY_EVADE       = 6,
    SAY_DEATH       = 7
};

enum Spells
{
    SPELL_PIERCING_GALE_FRONT       = 209628,
    SPELL_PIERCING_GALE_BACK        = 209630,
    SPELL_SLICING_MAELSTROM         = 209676,
    SPELL_BLADE_SURGE               = 209602,
    SPELL_WIND_IMAGE                = 209614,
    SPELL_SLICING_MAELSTROM_2       = 209741, ///< Image Melandrus cast
    SPELL_ENVELOPING_WINDS_FILTER   = 224327,
    SPELL_ENVELOPING_WINDS_AT       = 224330,
    SPELL_ENVELOPING_WINDS_STUN     = 224333,

    SPELL_MASQUERADE                = 213213,
    SPELL_UNINVITED_GUEST           = 213233,
    SPELL_RIGHTEOUS_INDIGNATION     = 213304,
    SPELL_CARRION_SWARM             = 214688,
    SPELL_CRIPPLE                   = 214690,
    SPELL_SHADOW_BOLT_VOLLEY        = 214692
};

enum eEvents
{
    EVENT_PIERCING_GALE         = 1,
    EVENT_SLICING_MAELSTROM     = 2,
    EVENT_BLADE_SURGE           = 3,
    EVENT_ENVELOPING_WINDS      = 4
};

Position const g_GroupCheckPos[4] =
{
    { 938.15f, 3147.17f, 52.24f, 0.0f },
    { 931.61f, 3171.08f, 49.57f, 0.0f },
    { 947.74f, 3202.11f, 22.57f, 0.0f },
    { 973.10f, 3166.68f, 22.57f, 0.0f }
};

uint32 const g_PathIDs[4] =
{
    9100408,
    9100407,
    9100406,
    9100405
};

class instance_court_of_stars;

/// Advisor Melandrus <First Blade of Ellisande> - 104218
class boss_advisor_melandrus : public CreatureScript
{
    public:
        boss_advisor_melandrus() : CreatureScript("boss_advisor_melandrus") { }

        struct boss_advisor_melandrusAI : public BossAI
        {
            boss_advisor_melandrusAI(Creature* p_Creature) : BossAI(p_Creature, DATA_MELANDRUS)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                if (Creature* l_Ellisande = me->SummonCreature(NPC_ELLISANDE, 871.28f, 3111.11f, 54.93f, 5.87f))
                    m_EllisandeGuid = l_Ellisande->GetGUID();

                m_InstanceScript = me->GetInstanceScript();

                m_IntroDone = false;
                m_Event = false;
                m_CheckRange = true;
                m_CheckRangeTimer = 1000;
                m_CheckAchievDSE = true;
            }

            Position m_BladeSurgePos;
            uint64 m_EllisandeGuid;
            bool m_IntroDone;
            bool m_Event;
            bool m_Queue;
            bool m_CheckRange;
            uint32 m_HitCounter;
            uint32 m_CheckRangeTimer;
            uint32 m_Timer;
            uint8 m_Text;
            InstanceScript* m_InstanceScript = nullptr;
            bool m_CheckAchievDSE;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void Reset() override
            {
                _Reset();

                m_BladeSurgePos = Position();
                m_HitCounter = 0;

                events.Reset();

                if (m_InstanceScript != nullptr)
                {
                    if (GameObject* l_Door = m_InstanceScript->instance->GetGameObject(m_InstanceScript->GetData64(GO_MELANDRUS_DOOR_2)))
                    {
                        if (m_InstanceScript->GetBossState(DATA_MELANDRUS_EVENT) == EncounterState::DONE)
                        {
                            l_Door->SetGoState(GO_STATE_ACTIVE);
                        }
                    }
                }

                DespawnCreaturesInArea(NPC_IMAGE_MELANDRUS, me);
                DespawnCreaturesInArea(NPC_ENVELOPING_WINDS, me);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();

                if (m_InstanceScript != nullptr)
                {
                    if (GameObject* l_Door = m_InstanceScript->instance->GetGameObject(m_InstanceScript->GetData64(GO_MELANDRUS_DOOR_2)))
                    {
                        l_Door->SetGoState(GO_STATE_READY);

                        if(!l_Door->HasFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE))
                         l_Door->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);
                    }
                }

                events.ScheduleEvent(EVENT_PIERCING_GALE, 6000);
                events.ScheduleEvent(EVENT_SLICING_MAELSTROM, 11000);
                events.ScheduleEvent(EVENT_BLADE_SURGE, 19000);
                events.ScheduleEvent(EVENT_ENVELOPING_WINDS, 10000);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo != nullptr)
                {
                    if (p_SpellInfo->Id == SPELL_BLADE_SURGE)
                    {
                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveCharge(m_BladeSurgePos.m_positionX, m_BladeSurgePos.m_positionY, m_BladeSurgePos.m_positionZ, 55.0f, EVENT_CHARGE, false, 209667);

                        AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                        {
                            if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100, true))
                                me->GetMotionMaster()->MoveChase(l_Target);
                        });
                    }
                }
            }

            void EnterEvadeMode() override
            {
                DespawnCreaturesInArea(NPC_IMAGE_MELANDRUS, me);
                DespawnCreaturesInArea(NPC_ENVELOPING_WINDS, me);
                BossAI::EnterEvadeMode();

                if (m_InstanceScript != nullptr)
                {
                    if (GameObject* l_Door = m_InstanceScript->instance->GetGameObject(m_InstanceScript->GetData64(GO_MELANDRUS_DOOR_2)))
                    {
                        l_Door->SetGoState(GO_STATE_ACTIVE);
                    }
                }

                me->NearTeleportTo(me->GetHomePosition(), false);
                me->Respawn(true, true, 20 * TimeConstants::IN_MILLISECONDS);
                me->SetFacingTo(0.8f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                DespawnCreaturesInArea(NPC_IMAGE_MELANDRUS, me);
                DespawnCreaturesInArea(NPC_ENVELOPING_WINDS, me);

                Position const l_Position = { 914.1691f, 3137.8772f, 54.8299f, 3.666971f };
                Position const l_Dest = { 895.7474f, 3120.1174f, 54.8552f, 3.881383f };

                uint64 l_Guid = 0;

                if (Creature* l_Lilleth = me->SummonCreature(113617, l_Position, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    l_Lilleth->SetReactState(ReactStates::REACT_PASSIVE);

                    l_Guid = l_Lilleth->GetGUID();

                    l_Lilleth->AddAura(229112, l_Lilleth); ///< Stealth

                    l_Lilleth->GetMotionMaster()->MovePoint(0, l_Dest);

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 2405, me, nullptr, *me))
                        delete l_Conversation;
                }

                if (m_InstanceScript != nullptr)
                {
                    if (GameObject* l_Door = m_InstanceScript->instance->GetGameObject(m_InstanceScript->GetData64(GO_MELANDRUS_DOOR_2)))
                    {
                        l_Door->SetGoState(GO_STATE_ACTIVE);
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon != nullptr)
                {
                    if (p_Summon->GetEntry() == 83816) ///< Wolrd Trigger
                    {
                        p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Summon->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                    }
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Event)
                {
                    if (m_EllisandeGuid)
                    {
                        if (m_Timer <= p_Diff)
                        {
                            if (m_Queue)
                                Talk(m_Text);
                            else
                            {
                                if (Creature* l_Ellisande = Creature::GetCreature(*me, m_EllisandeGuid))
                                {
                                    if (l_Ellisande->IsAIEnabled)
                                        l_Ellisande->AI()->Talk(m_Text);
                                }

                                m_Text++;
                            }

                            m_Queue = (m_Queue == true ? false : true);

                            if (m_Text <= 2)
                                m_Timer = 7000;
                            else
                            {
                                m_Event = false;

                                if (Creature* l_Ellisande = Creature::GetCreature(*me, m_EllisandeGuid))
                                {
                                    l_Ellisande->CastSpell(l_Ellisande, 237399, true);
                                    l_Ellisande->DespawnOrUnsummon(1000);
                                }

                                events.Reset();
                                
                                me->SetFacingTo(0.8f);

                                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                                {
                                    me->SetReactState(REACT_DEFENSIVE);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                });
                            }
                        }
                        else
                            m_Timer -= p_Diff;
                    }
                }

                if (m_CheckRange)
                {
                    if (m_CheckRangeTimer <= p_Diff)
                    {
                        if (IsMythic() && m_CheckAchievDSE && m_InstanceScript && m_InstanceScript->GetData(DATA_NOBLE_COUNT) == 1 && m_InstanceScript->GetData(DATA_NOBLE_COMPLETE) == 1)
                        {
                            if (Creature* l_SpyNoble = Creature::GetCreature(*me, m_InstanceScript->GetData64(NPC_SUSPICIOUS_NOBLE)))
                            {
                                if (l_SpyNoble->isDead())
                                {
                                    if (Player* l_Target = me->FindNearestPlayer(37.f))
                                    {
                                        m_CheckAchievDSE = false;
                                        m_InstanceScript->DoCompleteAchievement(eAchievements::ACHIEVEMENT_DROPPING_SOME_EAVES);
                                    }
                                }
                            }
                        }

                        if (Player* l_Target = me->FindNearestPlayer(21.f))
                        {
                            m_IntroDone = true;
                            m_Timer = 1000;
                            m_Text = 0;
                            m_Event = true;
                            m_Queue = true;
                            m_CheckRange = false;
                        }

                        m_CheckRangeTimer = 1000;
                    }
                    else
                        m_CheckRangeTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 35.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    return;
                }
                else
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                switch (events.ExecuteEvent())
                {
                    case EVENT_PIERCING_GALE:
                    {
                        me->SummonCreature(105765, *me); ///< Summon dummy to handle whole Piercing Gale mechanic

                        std::list<Creature*> l_ListCreatures;
                        me->GetCreatureListWithEntryInGrid(l_ListCreatures, NPC_IMAGE_MELANDRUS, 80.0f);

                        for (Creature* l_Itr : l_ListCreatures)
                        {
                            l_Itr->CastSpell(l_Itr, SPELL_PIERCING_GALE_FRONT);
                            l_Itr->CastSpell(l_Itr, SPELL_PIERCING_GALE_BACK);
                        }

                        me->CastSpell(me, SPELL_PIERCING_GALE_FRONT);
                        me->CastSpell(me, SPELL_PIERCING_GALE_BACK);

                        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                        {
                            std::list<Player*> l_ListPlayers;
                            me->GetPlayerListInGrid(l_ListPlayers, 20.0f);

                            l_ListPlayers.remove_if([this](Player* p_Player) -> bool
                            {
                                if (p_Player == nullptr || p_Player->isDead() || me->isInBack(p_Player, 0.30f)) ///< Arcane Beacon
                                    return true;

                                return false;
                            });

                            if (!l_ListPlayers.empty())
                                return;

                            for (Player* l_Itr : l_ListPlayers)
                            {
                                Position l_Pos = *me;

                                l_Pos.m_orientation -= M_PI;

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 1.0f, l_Pos.m_orientation);

                                l_Itr->StopMoving();

                                l_Itr->GetMotionMaster()->Clear(false);
                                l_Itr->GetMotionMaster()->MoveKnockbackFrom(l_Pos.m_positionX, l_Pos.m_positionY, 8.0f, 4.0f);
                            }
                        });

                        events.ScheduleEvent(EVENT_PIERCING_GALE, 20000);
                        break;
                    }
                    case EVENT_SLICING_MAELSTROM:
                    {
                        DoCast(SPELL_SLICING_MAELSTROM);
                        EntryCheckPredicate l_Pred(NPC_IMAGE_MELANDRUS);
                        summons.DoAction(EVENT_SLICING_MAELSTROM, l_Pred);

                        std::list<Creature*> l_ListCreatures;
                        me->GetCreatureListWithEntryInGrid(l_ListCreatures, NPC_IMAGE_MELANDRUS, 80.0f);

                        for (Creature* l_Itr : l_ListCreatures)
                        {
                            l_Itr->CastSpell(l_Itr, SPELL_SLICING_MAELSTROM);
                        }

                        Talk(SAY_MAELSTROM);
                        events.ScheduleEvent(EVENT_SLICING_MAELSTROM, 18000);
                        break;
                    }
                    case EVENT_BLADE_SURGE:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 40.f, true))
                        {
                            m_BladeSurgePos = *l_Target;
                            DoCast(l_Target, SPELL_BLADE_SURGE);
                        }

                        Talk(SAY_BLADE);
                        events.ScheduleEvent(EVENT_BLADE_SURGE, 19000);
                        break;
                    }
                    case EVENT_ENVELOPING_WINDS:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_FARTHEST))
                            me->CastSpell(l_Target, SPELL_ENVELOPING_WINDS_FILTER, true);

                        events.ScheduleEvent(EVENT_ENVELOPING_WINDS, 20000);
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
            return new boss_advisor_melandrusAI(p_Creature);
        }
};

/// Image of Advisor Melandrus - 105754
class npc_image_of_advisor_melandrus : public CreatureScript
{
    public:
        npc_image_of_advisor_melandrus() : CreatureScript("npc_image_of_advisor_melandrus") { }

        struct npc_image_of_advisor_melandrusAI : public ScriptedAI
        {
            npc_image_of_advisor_melandrusAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->setFaction(14);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                DoCast(me, SPELL_WIND_IMAGE, true);

                me->SetOrientation(frand(0.0f, 2 * M_PI));
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == EVENT_PIERCING_GALE)
                    DoCast(SPELL_PIERCING_GALE_FRONT);
                else if (p_Action == EVENT_SLICING_MAELSTROM)
                    DoCast(SPELL_SLICING_MAELSTROM_2);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_image_of_advisor_melandrusAI(p_Creature);
        }
};

/// Suspicious Noble - 107435
class npc_suspicious_noble : public CreatureScript
{
    public:
        npc_suspicious_noble() : CreatureScript("npc_suspicious_noble") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->HasAura(SPELL_MASQUERADE) && !p_Player->HasAura(SPELL_RIGHTEOUS_INDIGNATION))
            {
                p_Player->ADD_GOSSIP_ITEM_DB(19764, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                p_Player->SEND_GOSSIP_MENU(29265, p_Creature->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action != (GOSSIP_ACTION_INFO_DEF + 1) || !p_Creature->GetInstanceScript() || !p_Creature->IsAIEnabled)
                return false;

            if (auto l_AI = CAST_AI(npc_suspicious_noble::npc_suspicious_nobleAI, p_Creature->GetAI()))
            {
                if (InstanceScript* l_InstanceScript = p_Creature->GetInstanceScript())
                {
                    if (p_Creature->GetGUID() == p_Creature->GetInstanceScript()->GetData64(NPC_SUSPICIOUS_NOBLE))
                    {
                        l_InstanceScript->SetData(DATA_NOBLE_COUNT, 0);
                        std::list<Creature*> l_ListChattyRumigonousLists;
                        p_Creature->GetCreatureListWithEntryInGrid(l_ListChattyRumigonousLists, NPC_CHATTY_RUMORMONGER, 300.0f);

                        l_ListChattyRumigonousLists.remove_if([this](Creature* p_Creature) -> bool
                        {
                            if (p_Creature == nullptr || !p_Creature->HasAura(133297)) ///< Arcane Beacon
                                return true;

                            return false;
                        });

                        if (l_ListChattyRumigonousLists.empty())
                        {
                            l_InstanceScript->SetData(DATA_NOBLE_COMPLETE, 0);
                            l_AI->m_Start = true;

                            p_Creature->AI()->Talk(1, p_Player->GetGUID());

                            p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        }
                        else
                        {
                            l_AI->m_PlrGuid = p_Player->GetGUID();

                            if (!p_Player->isGameMaster())
                                p_Creature->AddAura(SPELL_RIGHTEOUS_INDIGNATION, p_Player);
                        }
                    }
                    else
                    {
                        l_AI->m_PlrGuid = p_Player->GetGUID();

                        if (!p_Player->isGameMaster())
                            p_Creature->AddAura(SPELL_RIGHTEOUS_INDIGNATION, p_Player);
                    }
                }
            }

            p_Player->PlayerTalkClass->SendCloseGossip();
            return true;
        }

        struct npc_suspicious_nobleAI : public ScriptedAI
        {
            npc_suspicious_nobleAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                m_Instance = me->GetInstanceScript();
                m_Start = false;
                m_StartTimer = 1000;
                if (m_Instance && me->GetGUID() == m_Instance->GetData64(NPC_SUSPICIOUS_NOBLE) && m_Instance->GetBossState(DATA_MELANDRUS_EVENT) != DONE)
                    m_Instance->SetBossState(DATA_MELANDRUS_EVENT, NOT_STARTED);
            }

            InstanceScript* m_Instance;
            uint32 m_Path;

            uint64 m_PlrGuid = 0;

            bool m_Start;

            uint32 m_ClassEventTimer = 1000;

            uint32 m_StartTimer;

            void Reset() override
            {
                me->SetDynamicValue(UNIT_DYNAMIC_FIELD_WORLD_EFFECTS, 0, 2100);
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(2);
                events.Reset();
                events.ScheduleEvent(EVENT_1, 3000);
                events.ScheduleEvent(EVENT_2, 9000);
                events.ScheduleEvent(EVENT_3, 16000);
            }

            void EnterEvadeMode() override
            {
                Reset();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->LoadCreaturesAddon();
                me->ClearLootContainers();
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_DEFENSIVE);
                me->setFaction(16);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 0)
                {
                    if (m_PlrGuid)
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, m_PlrGuid))
                        {
                            Talk(0, l_Player->GetGUID());
                            l_Player->DelayedCastSpell(l_Player, Spells::SPELL_UNINVITED_GUEST, true, IN_MILLISECONDS);
                        }
                    }
                }
            }

            void JustDied(Unit* p_Who) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SetBossState(DATA_MELANDRUS_EVENT, DONE);

                    if (GameObject* l_Door = l_Instance->instance->GetGameObject(l_Instance->GetData64(GO_MELANDRUS_DOOR_2)))
                        l_Door->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE | GameObjectFlags::GO_FLAG_LOCKED);
                }
            }

            void LastWPReached() override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_DEFENSIVE);
                me->SetHomePosition(me->GetPosition());
                me->setFaction(16);
                me->SetDisplayId(eCreatures::NPC_GERENTH_THE_VILE);
                me->SetFacingTo(5.4f);
            }

            void ExecuteEvent(uint32 const p_EventID)
            {
                switch (p_EventID)
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_CARRION_SWARM);
                        events.RescheduleEvent(EVENT_1, 20 * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_CRIPPLE);
                        events.RescheduleEvent(EVENT_2, urand(20, 25) * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_SHADOW_BOLT_VOLLEY);
                        events.RescheduleEvent(EVENT_3, urand(30, 35) * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (m_Start)
                {
                    if (m_StartTimer <= p_Diff)
                    {
                        if (me->GetPositionZ() > 48.0f)
                        {
                            if (me->GetPositionY() > 3160.80f)
                                m_Path = 9100407;
                            else
                                m_Path = 9100408;
                        }
                        else
                        {
                            if (me->GetPositionY() > 3189.18f)
                                m_Path = 9100406;
                            else
                                m_Path = 9100405;
                        }

                        me->SetWalk(false);

                        me->SetSpeed(MOVE_WALK, 1.1f);
                        me->SetSpeed(MOVE_RUN, 1.1f);
                        me->SetSpeed(MOVE_RUN_BACK, 1.1f);
                        me->GetMotionMaster()->MovePath(m_Path, false);

                        m_Start = false;
                    }
                    else
                        m_StartTimer -= p_Diff;
                }

                ///< Class related
                if (m_ClassEventTimer <= p_Diff)
                {
                    std::list<Player*> l_ListPlayers;
                    me->GetPlayerListInGrid(l_ListPlayers, 30.0f);

                    for (Player* l_Itr : l_ListPlayers)
                    {
                        if (!l_Itr->HasAura(SPELL_MASQUERADE))
                        {
                            m_PlrGuid = l_Itr->GetGUID();

                            if (!l_Itr->HasAura(SPELL_RIGHTEOUS_INDIGNATION))
                            {
                                l_Itr->AddAura(SPELL_RIGHTEOUS_INDIGNATION, l_Itr);
                                if(CreatureAI* l_AI = me->AI())
                                   l_AI->DoAction(0);
                            }
                        }

                        if (me->GetGUID() == me->GetInstanceScript()->GetData64(NPC_SUSPICIOUS_NOBLE))
                        {
                            ///< Truthguard
                            if (l_Itr->GetDistance(*me) <= 15.0f && l_Itr->getClass() == Classes::CLASS_PALADIN && l_Itr->HasItemCount(128866, 1))
                            {
                                if (!l_Itr->HasAura(214856))
                                    l_Itr->AddAura(214856, l_Itr);
                            }
                            else
                                l_Itr->RemoveAura(214856);

                            ///<   Hacked spectral sight
                            if (l_Itr->GetDistance(*me) <= 50.0f && l_Itr->HasAura(188501) && l_Itr->getClass() == Classes::CLASS_DEMON_HUNTER)
                            {
                                if (!me->HasAura(223145))
                                    me->AddAura(223145, me);
                            }
                            else if (me->HasAura(223145))
                                me->RemoveAura(223145);
                        }
                    }

                    m_ClassEventTimer = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_ClassEventTimer -= p_Diff;

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventID = events.ExecuteEvent())
                {
                    ExecuteEvent(l_EventID);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_suspicious_nobleAI(p_Creature);
        }
};

/// Hall Checker - 950004
class npc_advisor_hall_checker : public CreatureScript
{
    public:
        npc_advisor_hall_checker() : CreatureScript("npc_advisor_hall_checker") { }

        struct npc_advisor_hall_checkerAI : public ScriptedAI
        {
            npc_advisor_hall_checkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Conversation = false;
            }

            bool m_Conversation;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (me->IsWithinDistInMap(p_Who, 20.0f))
                {
                    if (!p_Who->HasAura(SPELL_MASQUERADE))
                    {
                        p_Who->CastSpell(p_Who, SPELL_UNINVITED_GUEST, true);
                        return;
                    }

                    if (!m_Conversation)
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 2405, p_Who, nullptr, *p_Who))
                            delete l_Conversation;

                        m_Conversation = true;
                    }
                }
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_advisor_hall_checkerAI(p_Creature);
        }
};

/// Enveloping Winds - 112687
class npc_advisor_enveloping_winds : public CreatureScript
{
    public:
        npc_advisor_enveloping_winds() : CreatureScript("npc_advisor_enveloping_winds") { }

        struct npc_advisor_enveloping_windsAI : public ScriptedAI
        {
            npc_advisor_enveloping_windsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.30f);
                me->SetSpeed(MOVE_WALK, 0.30f);
                I_Despawn = false;
            }
            
            bool I_Despawn;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 500);
                events.ScheduleEvent(EVENT_2, 600);
                events.ScheduleEvent(EVENT_3, 4 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_ENVELOPING_WINDS_AT, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveRandom(15.0f);
                        events.ScheduleEvent(EVENT_2, 5 * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_3:
                    {
                        if (I_Despawn)
                            break;

                        std::list<Player*> l_PlayerList;
                        GetPlayerListInGrid(l_PlayerList, me, 3.0f);

                        if (l_PlayerList.empty())
                        {
                            events.ScheduleEvent(EVENT_3, 200);
                            break;
                        }

                        JadeCore::Containers::RandomResizeList(l_PlayerList, uint32(1));

                        if (Player* l_Player = l_PlayerList.front())
                        {
                            me->CastSpell(l_Player, SPELL_ENVELOPING_WINDS_STUN, true);
                            me->GetMotionMaster()->Clear(true);
                            me->DespawnOrUnsummon(1000);
                            I_Despawn = true;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_advisor_enveloping_windsAI(p_Creature);
        }
};

/// Righteous Indignation - 213304
class spell_righteous_indignation : public SpellScriptLoader
{
    public:
        spell_righteous_indignation() : SpellScriptLoader("spell_righteous_indignation") { }

        class spell_righteous_indignation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_righteous_indignation_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAura(SPELL_MASQUERADE);

                if (Creature* l_Creature = l_Caster->ToCreature())
                {
                    if(CreatureAI* l_AI = l_Creature->AI())
                        l_AI->DoAction(0);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_righteous_indignation_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_righteous_indignation_AuraScript();
        }
};

/// Suspicious Noble - 107435
class npc_cos_chatty_rumormonger : public CreatureScript
{
    public:
        npc_cos_chatty_rumormonger() : CreatureScript("npc_cos_chatty_rumormonger") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player->HasAura(SPELL_MASQUERADE) && p_Player->HasAura(SPELL_RIGHTEOUS_INDIGNATION))
                return false;

            InstanceScript* l_Inst = p_Creature->GetInstanceScript();

            if (l_Inst == nullptr || !p_Creature->IsAIEnabled)
                return false;

            p_Creature->RemoveAllAuras();

            if (Creature* l_SpyNoble = Creature::GetCreature(*p_Creature, l_Inst->GetData64(NPC_SUSPICIOUS_NOBLE))) ///< Returns only real spy
            {
                auto l_HintsItr = g_Hints.find(l_SpyNoble->GetDisplayId());

                if (l_HintsItr == g_Hints.end())
                    return false;

                uint32 l_HintIndex = l_Inst->GetData(p_Creature->GetGUIDLow());

                auto l_TextsItr = g_Texts.find(l_HintsItr->second[l_HintIndex]);

                p_Player->SEND_GOSSIP_MENU(l_TextsItr->second[urand(0, l_TextsItr->second.size() - 1)], p_Creature->GetGUID());
            }

            return true;
        }

        struct npc_cos_chatty_rumormonger_AI : public ScriptedAI
        {
            npc_cos_chatty_rumormonger_AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            uint32 m_Diff;
            uint64 m_PlrGuid;

            void Reset() override
            {
                m_PlrGuid = 0;
                m_Diff = 1000;
                me->SetDynamicValue(UNIT_DYNAMIC_FIELD_WORLD_EFFECTS, 0, 2101);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Diff <= p_Diff)
                {
                    std::list<Player*> l_ListPlayers;
                    me->GetPlayerListInGrid(l_ListPlayers, 30.0f);

                    for (Player* l_Itr : l_ListPlayers)
                    {
                        if (!l_Itr->HasAura(SPELL_MASQUERADE) && !l_Itr->IsBeingTeleported() && !l_Itr->HasAura(Spells::SPELL_RIGHTEOUS_INDIGNATION))
                        {
                            m_PlrGuid = l_Itr->GetGUID();

                            if (m_PlrGuid)
                            {
                                if (Player* l_Player = Player::GetPlayer(*me, m_PlrGuid))
                                {
                                    Talk(0, l_Player->GetGUID());
                                    l_Player->DelayedCastSpell(l_Player, Spells::SPELL_UNINVITED_GUEST, true, IN_MILLISECONDS);
                                }
                            }
                        }
                    }

                    m_Diff = 1000;
                }
                else
                    m_Diff -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_chatty_rumormonger_AI(p_Creature);
        }
};

class npc_cos_arcane_custodian : public CreatureScript
{
    public:
        npc_cos_arcane_custodian() : CreatureScript("npc_cos_arcane_custodian") { }

        struct npc_cos_arcane_custodianAI : public ScriptedAI
        {
            npc_cos_arcane_custodianAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Conversation = false;
            }

            bool m_Conversation;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (me->IsWithinDistInMap(p_Who, 20.0f))
                {
                    if (!p_Who->HasAura(SPELL_MASQUERADE))
                        p_Who->CastSpell(p_Who, Spells::SPELL_UNINVITED_GUEST, true);
                }
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_arcane_custodianAI(p_Creature);
        }
};

class spell_cos_enveloping_wings : public SpellScriptLoader
{
    public:
        spell_cos_enveloping_wings() : SpellScriptLoader("spell_cos_enveloping_wings") { }

        class spell_cos_enveloping_wings_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_enveloping_wings_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || l_Caster->IsPlayer())
                    return;

                if (Player* l_Player = l_Caster->FindNearestPlayer(30.0f))
                    l_Caster->CastSpell(l_Caster, 224328, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_cos_enveloping_wings_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cos_enveloping_wings_SpellScript();
        }
};

class npc_cos_gale_stalker : public CreatureScript
{
    public:
        npc_cos_gale_stalker() : CreatureScript("npc_cos_gale_stalker") { }

        struct npc_cos_gale_stalkerAI : public ScriptedAI
        {
            npc_cos_gale_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            void Reset() override
            {
                me->setFaction(FACTION_HOSTILE);

                me->SetDisplayId(11686);

                me->SetReactState(REACT_PASSIVE);

                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->SetFlag(UNIT_FIELD_FLAGS,  UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->GetEntry() == NPC_ADVISOR_MELANDRUS)
                {
                    me->setFaction(16); ///< Aggressive
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                    p_Summoner->CastSpell(p_Summoner, SPELL_PIERCING_GALE_FRONT, false);
                    /// me->CastSpell(me, SPELL_PIERCING_GALE_BACK, false);

                    Position l_Dest = *me;

                    l_Dest.m_orientation = me->GetOrientation() - M_PI;

                    if (Creature* l_FrontHackFix = p_Summoner->SummonCreature(83816, l_Dest, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 6 * TimeConstants::IN_MILLISECONDS))
                    {
                        l_FrontHackFix->SetDisplayId(11686);
                        l_FrontHackFix->SetReactState(REACT_PASSIVE);
                        l_FrontHackFix->setFaction(FACTION_HOSTILE);
                        l_FrontHackFix->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        l_FrontHackFix->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        l_FrontHackFix->CastSpell(l_FrontHackFix, SPELL_PIERCING_GALE_FRONT, false);
                    }

                    me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                }
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_gale_stalkerAI(p_Creature);
        }
};

class npc_cos_arcane_keys : public CreatureScript
{
    public:
        npc_cos_arcane_keys() : CreatureScript("npc_cos_arcane_keys") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->CastSpell(p_Creature, 214697, false);

            return true;
        }

        struct npc_cos_arcane_keysAI : public ScriptedAI
        {
            npc_cos_arcane_keysAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            void Reset() override
            {
                me->setFaction(FACTION_FRIENDLY);

                me->SetDisplayId(11686);

                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                me->SetReactState(REACT_PASSIVE);

                me->CastSpell(me, 214696, true);

                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NON_ATTACKABLE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_arcane_keysAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Balcony Door - 251848
class go_cos_balcony_door : public GameObjectScript
{
    public:
        go_cos_balcony_door() : GameObjectScript("go_cos_balcony_door") {}

        enum eActions
        {
            TriggerEntrance
        };

        bool OnGossipHello(Player* p_Player, GameObject* p_GameObject) override
        {
            InstanceScript* l_Instance = p_GameObject->GetInstanceScript();
            if (!l_Instance)
                return false;

            /// Only after event is done.
            if (l_Instance->GetBossState(DATA_MELANDRUS_EVENT) != EncounterState::DONE)
                return false;

            p_Player->CastSpell(p_GameObject, 223254, false);

            Conversation* l_Conversation = new Conversation;
            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 2402, p_Player, nullptr, *p_Player))
                delete l_Conversation;

            p_GameObject->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);

        ///    p_GameObject->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
           /// p_GameObject->setFaction(16);

            return false;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_advisor_melandrus()
{
    /// Boss
    new boss_advisor_melandrus();

    /// Creatures
    new npc_image_of_advisor_melandrus();
    new npc_cos_chatty_rumormonger();
    new npc_cos_arcane_custodian();
    new npc_suspicious_noble();
    new npc_advisor_hall_checker();
    new npc_advisor_enveloping_winds();
    new npc_cos_gale_stalker();
    new npc_cos_arcane_keys();

    /// Spell
    new spell_righteous_indignation();
    new spell_cos_enveloping_wings();

    /// GameObjects
    new go_cos_balcony_door();
}
#endif
