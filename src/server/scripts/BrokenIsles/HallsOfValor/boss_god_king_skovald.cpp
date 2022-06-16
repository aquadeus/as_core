////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "halls_of_valor.hpp"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_RAGNAROK        = 1,
    SAY_RAGNAROK_EMOTE  = 2,
    SAY_EGIDA           = 3,
    SAY_DEATH           = 4,
    SAY_SAVAGED_BLADE   = 5,
    SAY_KILL            = 6,
};

enum Spells
{
    SPELL_ZERO_REGEN            = 78725,
    SPELL_CONVERSATION          = 193975,

    SPELL_FELBLAZE_RUSH         = 193658,
    SPELL_RAGNAROK              = 193826,
    SPELL_RAGNAROK_DAMAGES      = 193827,
    SPELL_RAGNAROK_BUFF         = 202494,
    SPELL_AEGIS_SPAWN           = 193940,
    SPELL_CLAIM_THE_AEGIS       = 194112,
    SPELL_SAVAGE_BLADE          = 193668,
    SPELL_FLAME_OF_WOE_SEARCHER = 207510,
    SPELL_FLAME_OF_WOE_MISSILE  = 207509,

    /// Aegis
    SPELL_AEGIS_VISUAL_SHIELD   = 193769,
    SPELL_AEGIS_OVERRIDE        = 193783,

    /// Kings
    SPELL_CALL_ANCESTOR         = 200969,
    SPELL_WICKED_DAGGER         = 199674,
    SPELL_UNRULY_YELL           = 199726,
    SPELL_SEVER                 = 199652,
};

enum eEvents
{
    EVENT_INTRO_COMPLETE    = 1,
    EVENT_FELBLAZE_RUSH     = 2,
    EVENT_RAGNAROK          = 3,
    EVENT_PICK_AEGIS        = 4,
    EVENT_DROP_AEGIS        = 5,
    EVENT_SAVAGE_BLADE      = 6
};

enum eObjectives
{
    AEGIS_OF_AGGRAMAR           = 110425
};

Position const g_KingsPos[4] =
{
    { 2387.87f, 506.50f, 749.30f, 0.57f }, ///< NPC_KING_HALDOR - 95843
    { 2388.01f, 551.16f, 749.30f, 5.67f }, ///< NPC_KING_BJORN  - 97081
    { 2409.10f, 490.69f, 749.30f, 1.16f }, ///< NPC_KING_RANULF - 97083
    { 2408.56f, 568.34f, 749.30f, 5.11f }  ///< NPC_KING_TOR    - 97084
};

uint32 g_OdynKings[4] =
{
    NPC_KING_HALDOR,
    NPC_KING_BJORN,
    NPC_KING_RANULF,
    NPC_KING_TOR
};

Position const g_CenterOfRoom = { 2429.09f, 529.0f, 749.0f };

/// God King Skovald - 95675
class boss_god_king_skovald : public CreatureScript
{
    public:
        boss_god_king_skovald() : CreatureScript("boss_god_king_skovald") { }

        struct boss_god_king_skovaldAI : public BossAI
        {
            boss_god_king_skovaldAI(Creature* p_Creature) : BossAI(p_Creature, DATA_SKOVALD)
            {
                if (me->isAlive())
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetVisible(false);
                    SummonOdynKings();
                    m_Intro = true;
                    m_KingDiedCount = 0;
                }
            }

            bool m_Intro;
            bool m_AegisEvent;
            bool m_RagnarokScheduled;
            uint8 m_KingDiedCount;
            uint8 m_Power, m_PowerCounter;
            uint16 m_PowerTimer;

            void Reset() override
            {
                _Reset();

                m_AegisEvent = false;
                m_RagnarokScheduled = false;

                DoCast(me, SPELL_ZERO_REGEN, true);

                me->SetPower(POWER_ENERGY, 80);
                me->RemoveAurasDueToSpell(SPELL_RAGNAROK_BUFF);
                me->RemoveAllAreasTrigger();

                m_PowerTimer = 1000;

                if (!m_Intro && instance)
                {
                    me->SetReactState(REACT_AGGRESSIVE);

                    if (Creature* l_Odyn = instance->instance->GetCreature(instance->GetData64(DATA_ODYN)))
                        l_Odyn->CastSpell(l_Odyn, SPELL_AEGIS_SPAWN, true);
                }
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);

                _EnterCombat();

                events.ScheduleEvent(EVENT_FELBLAZE_RUSH, 7000);
                events.ScheduleEvent(EVENT_SAVAGE_BLADE, 24000);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                if (instance != nullptr)
                {
                    if (Creature* l_Odyn = instance->instance->GetCreature(instance->GetData64(DATA_ODYN)))
                        l_Odyn->CastSpell({ 2405.39f, 528.123f, 749.212f, 0.0f }, SPELL_AEGIS_SPAWN, true);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();

                me->RemoveAllAreasTrigger();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AEGIS_OVERRIDE);
                    instance->instance->ForEachPlayer([&](Player* p_Player) -> void
                    {
                        p_Player->KilledMonsterCredit(eObjectives::AEGIS_OF_AGGRAMAR);
                    });
                }
            }

            void SummonOdynKings()
            {
                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (instance && instance->GetData(DATA_SKOVALD_EVENT) == DONE)
                    {
                        m_KingDiedCount = 4;

                        if (Creature* l_Odyn = Creature::GetCreature(*me, instance->GetData64(DATA_ODYN)))
                        {
                            if (l_Odyn->IsAIEnabled)
                                l_Odyn->AI()->Talk(1);

                            l_Odyn->CastSpell({ 2405.39f, 528.123f, 749.212f, 0.0f }, SPELL_AEGIS_SPAWN, true);
                        }
                    }
                    else
                    {
                        for (uint8 l_I = 0; l_I < 4; l_I++)
                            me->SummonCreature(g_OdynKings[l_I], g_KingsPos[l_I]);
                    }
                });
            }

            void KingsEventComplete()
            {
                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetVisible(true);
                    me->SetPower(POWER_ENERGY, 80);
                    me->GetMotionMaster()->MovePoint(1, 2411.03f, 528.72f, 748.99f);

                    DoCast(me, SPELL_CONVERSATION, true);
                });
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (!m_Intro)
                    return;

                switch (p_Summon->GetEntry())
                {
                    case NPC_KING_HALDOR:
                    case NPC_KING_BJORN:
                    case NPC_KING_RANULF:
                    case NPC_KING_TOR:
                        m_KingDiedCount++;
                        break;
                    default:
                        break;
                }

                /// End of event if all four kings are dead
                if (m_KingDiedCount == 4)
                {
                    if (Creature* l_Odyn = Creature::GetCreature(*me, instance->GetData64(DATA_ODYN)))
                    {
                        if (l_Odyn->IsAIEnabled)
                            l_Odyn->AI()->Talk(1);

                        l_Odyn->CastSpell({ 2405.39f, 528.123f, 749.212f, 0.0f }, SPELL_AEGIS_SPAWN, true);
                    }

                    if (instance->GetData(DATA_SKOVALD_EVENT) != DONE)
                        instance->SetData(DATA_SKOVALD_EVENT, DONE);
                }
                /// Otherwise, reenable other kings
                else
                {
                    std::array<uint32, 4> l_Kings = { { NPC_KING_HALDOR, NPC_KING_BJORN, NPC_KING_RANULF, NPC_KING_TOR } };

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        for (uint32 l_Entry : l_Kings)
                        {
                            if (p_Summon->GetEntry() == l_Entry)
                                continue;

                            if (Creature* l_King = Creature::GetCreature(*me, l_Instance->GetData64(l_Entry)))
                            {
                                if (!l_King->isAlive())
                                    continue;

                                l_King->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            }
                        }
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    events.ScheduleEvent(EVENT_INTRO_COMPLETE, 24000);
                }
                else if (p_ID == 2)
                {
                    ClearDelayedOperations();
                    events.DelayEvents(3000);
                    DoCast(SPELL_CLAIM_THE_AEGIS);
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == 193988)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->SetFacingToObject(l_Target);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                        DoCast(me, 193983, true);
                        m_AegisEvent = false;
                        Talk(SAY_EGIDA);
                        events.ScheduleEvent(EVENT_DROP_AEGIS, 1000);
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_SAVAGE_BLADE:
                    {
                        if (p_Target->IsPlayer() && !p_Target->IsActiveSaveAbility())
                            DoCast(p_Target, 193686, true);

                        break;
                    }
                    case SPELL_FLAME_OF_WOE_SEARCHER:
                    {
                        DoCast(p_Target, SPELL_FLAME_OF_WOE_MISSILE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_RAGNAROK)
                    m_RagnarokScheduled = false;
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (m_Intro && m_KingDiedCount == 4)
                    KingsEventComplete();
                else
                    events.ScheduleEvent(EVENT_PICK_AEGIS, 500);
            }

            uint32 GetData(uint32 p_ID /*= 0*/) override
            {
                return m_KingDiedCount;
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    Talk(Says::SAY_KILL);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() && !m_Intro)
                    return;


                events.Update(p_Diff);

                if (me->isInCombat())
                {
                    if (me->GetExactDist(&g_CenterOfRoom) > 85.0f)
                    {
                        EnterEvadeMode();
                        return;
                    }

                    if (m_PowerTimer <= p_Diff)
                    {
                        m_Power = me->GetPower(POWER_ENERGY);

                        if (m_Power < 100)
                        {
                            if (m_PowerCounter < 2)
                            {
                                m_PowerCounter++;
                                me->SetPower(POWER_ENERGY, m_Power + 2);
                            }
                            else
                            {
                                m_PowerCounter = 0;
                                me->SetPower(POWER_ENERGY, m_Power + 1);
                            }
                        }
                        else if (!events.HasEvent(EVENT_RAGNAROK) && !m_RagnarokScheduled)
                            events.ScheduleEvent(EVENT_RAGNAROK, 500);

                        m_PowerTimer = 1000;
                    }
                    else
                        m_PowerTimer -=p_Diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || m_AegisEvent)
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_INTRO_COMPLETE:
                    {
                        m_Intro = false;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    case EVENT_FELBLAZE_RUSH:
                    {
                        DoCast(SPELL_FELBLAZE_RUSH);
                        events.ScheduleEvent(EVENT_FELBLAZE_RUSH, urand(7, 16) * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_RAGNAROK:
                    {
                        m_RagnarokScheduled = true;

                        DoCast(SPELL_RAGNAROK);

                        Talk(SAY_RAGNAROK);
                        Talk(SAY_RAGNAROK_EMOTE);
                        break;
                    }
                    case EVENT_PICK_AEGIS:
                    {
                        if (Creature* l_Aegis = me->FindNearestCreature(NPC_AEGIS_OF_AGGRAMAR, 60.0f))
                        {
                            /// Skovald is supposed to pick Aegis only after it has been dropped by player
                            if (l_Aegis->GetAnyOwner() && !l_Aegis->GetAnyOwner()->IsPlayer())
                                break;

                            m_AegisEvent = true;
                            DoStopAttack();

                            me->SetReactState(REACT_PASSIVE);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(2, l_Aegis->GetPositionX(), l_Aegis->GetPositionY(), l_Aegis->GetPositionZ());
                        }
                        else
                            break;

                        /// Just for safety
                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_AegisEvent = false;
                        });

                        break;
                    }
                    case EVENT_DROP_AEGIS:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    case EVENT_SAVAGE_BLADE:
                    {
                        if (me->getVictim())
                        {
                            Talk(Says::SAY_SAVAGED_BLADE);
                            DoCast(me->getVictim(), SPELL_SAVAGE_BLADE, false);
                        }

                        events.ScheduleEvent(EVENT_SAVAGE_BLADE, 19500);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_god_king_skovaldAI (creature);
        }
};

/// King Haldor - 95843
/// King Bjorn - 97081
/// King Ranulf - 97083
/// King Tor - 97084
class npc_generic_odyn_kings : public CreatureScript
{
    public:
        npc_generic_odyn_kings() : CreatureScript("npc_generic_odyn_kings") { }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();

            switch (p_Action)
            {
                case 1:
                {
                    if (p_Creature->IsAIEnabled)
                    {
                        p_Creature->AI()->SetGUID(p_Player->GetGUID());
                        p_Creature->AI()->DoAction(0);
                    }

                    p_Player->PlayerTalkClass->SendCloseGossip();
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        struct npc_generic_odyn_kingsAI : public ScriptedAI
        {
            npc_generic_odyn_kingsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_SpeakerGuid;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (!IsChallengeMode())
                    return;

                switch (me->GetEntry()) ///< All 4 should have the same HP (confirmed by retail vids)
                {
                    case NPC_KING_HALDOR:
                    case NPC_KING_TOR:
                        p_Mod = 3.76f;
                        break;
                    case NPC_KING_BJORN:
                        p_Mod = 3.95f;
                        break;
                    case NPC_KING_RANULF:
                        p_Mod = 2.64f;
                        break;
                    default:
                        break;
                }
            }

            void Reset() override
            {
                m_SpeakerGuid = 0;

                events.Reset();
                summons.DespawnAll();

                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                me->setFaction(35);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BIFROST_AURA);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ODYNS_BLESSING);
                }

                events.ScheduleEvent(EVENT_1, urand(24000, 27000));
                events.ScheduleEvent(EVENT_2, urand(16000, 18000));
                events.ScheduleEvent(EVENT_3, 10000);
                events.ScheduleEvent(EVENT_4, 7000);
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);

                Reset();
            }

            void SpellHit(Unit* /**/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSharedSpells::SPELL_MUG_OF_MEAD)
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                        l_Instance->SetData(eData::DATA_MUG_OF_MEAD_ACHIEVEMENT, 0);
                }
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();

                me->AddUnitState(UNIT_STATE_EVADE);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTargetedHome(true);

                me->SetLastDamagedTime(0);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                Talk(0);

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                uint32 l_DeadKings          = 0;
                uint64 l_OtherKingGuid      = 0;
                InstanceScript* l_Instance  = me->GetInstanceScript();

                if (l_Instance != nullptr)
                {
                    if (Creature* l_Skovald = Creature::GetCreature(*me, l_Instance->GetData64(DATA_SKOVALD)))
                    {
                        if (l_Skovald->IsAIEnabled)
                            l_DeadKings = l_Skovald->AI()->GetData();
                    }
                }

                /// When there are only two Kings left, both Kings will turn hostile when one of them is challenged
                if (l_DeadKings >= 2)
                {
                    std::list<Creature*> l_KingsList;

                    for (uint8 l_I = 0; l_I < 4; ++l_I)
                    {
                        if (g_OdynKings[l_I] == me->GetEntry())
                            continue;

                        me->GetCreatureListWithEntryInGrid(l_KingsList, g_OdynKings[l_I], 80.0f);
                    }

                    if (!l_KingsList.empty())
                    {
                        l_KingsList.remove_if([this](Creature* p_Creature) -> bool
                        {
                            if (p_Creature->isDead())
                                return true;

                            return false;
                        });
                    }

                    if (l_KingsList.empty())
                        return;

                    /// Second king doesn't talk
                    if (Creature* l_OtherKing = l_KingsList.front())
                        l_OtherKingGuid = l_OtherKing->GetGUID();
                }
                /// Otherwise, disable other kings while this one's alive
                else
                {
                    std::array<uint32, 4> l_Kings = { { NPC_KING_HALDOR, NPC_KING_BJORN, NPC_KING_RANULF, NPC_KING_TOR } };

                    if (l_Instance)
                    {
                        for (uint32 l_Entry : l_Kings)
                        {
                            if (me->GetEntry() == l_Entry)
                                continue;

                            if (Creature* l_King = Creature::GetCreature(*me, l_Instance->GetData64(l_Entry)))
                            {
                                if (!l_King->isAlive())
                                    continue;

                                l_King->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            }
                        }
                    }
                }

                me->CastSpell(me, ChallengeSpells::ChallengersMight, true); ///< Those guys are neutral by default (Faction 35 so Core doesn't apply this aura to them when they are spawned)

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this, l_OtherKingGuid]() -> void
                {
                    me->setFaction(FACTION_MONSTER_2);

                    Player* l_Player = Player::GetPlayer(*me, m_SpeakerGuid);
                    if (l_Player != nullptr)
                        AttackStart(l_Player);

                    if (l_OtherKingGuid)
                    {
                        if (Creature* l_King = Creature::GetCreature(*me, l_OtherKingGuid))
                        {
                            l_King->setFaction(FACTION_MONSTER_2);
                            l_King->CastSpell(l_King, ChallengeSpells::ChallengersMight, true); ///< Those guys are neutral by default (Faction 35 so Core doesn't apply this aura to them when they are spawned)

                            if (l_King->IsAIEnabled && l_Player != nullptr)
                                l_King->AI()->AttackStart(l_Player);
                        }
                    }
                });
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_SpeakerGuid = p_Guid;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();

                Talk(1);

                switch (me->GetEntry())
                {
                    case NPC_KING_TOR:
                        me->CastSpell(me, 199614, true);
                        break;
                    case NPC_KING_RANULF:
                        me->CastSpell(me, 199622, true);
                        break;
                    case NPC_KING_BJORN:
                        me->CastSpell(me, 199621, true);
                        break;
                    case NPC_KING_HALDOR:
                        me->CastSpell(me, 199620, true);
                        break;
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
                        me->CastSpell(me, SPELL_CALL_ANCESTOR);
                        events.ScheduleEvent(EVENT_1, urand(22000, 26000));
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_WICKED_DAGGER);
                        events.ScheduleEvent(EVENT_2, urand(16000, 18000));
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_UNRULY_YELL);
                        events.ScheduleEvent(EVENT_4, 20000);
                        break;
                    }
                    case EVENT_4:
                    {
                        DoCast(SPELL_SEVER);
                        events.ScheduleEvent(EVENT_4, 12000);
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
            return new npc_generic_odyn_kingsAI(p_Creature);
        }
};

/// Aegis of Aggramar - 98364
class npc_skovald_aegis_of_aggramar : public CreatureScript
{
    public:
        npc_skovald_aegis_of_aggramar() : CreatureScript("npc_skovald_aegis_of_aggramar") { }

        struct npc_skovald_aegis_of_aggramarAI : public ScriptedAI
        {
            npc_skovald_aegis_of_aggramarAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            }

            InstanceScript* m_Instance;

            bool m_Click;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                me->SetUInt32Value(UNIT_FIELD_INTERACT_SPELL_ID, SPELL_AEGIS_OVERRIDE);

                if (p_Summoner->IsPlayer() && m_Instance)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    if (Creature* l_Skovald = m_Instance->instance->GetCreature(m_Instance->GetData64(DATA_SKOVALD)))
                    {
                        if (l_Skovald->isInCombat() && l_Skovald->IsAIEnabled)
                            l_Skovald->AI()->DoAction(true);
                    }
                }

                m_Click = false;
                DoCast(me, SPELL_AEGIS_VISUAL_SHIELD, true);
                Talk(0);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (!m_Click)
                {
                    m_Click = true;
                    p_Clicker->CastSpell(p_Clicker, SPELL_AEGIS_OVERRIDE, true);

                    if (m_Instance)
                    {
                        if (Creature* l_Skovald = Creature::GetCreature(*me, m_Instance->GetData64(DATA_SKOVALD)))
                        {
                            if (l_Skovald->IsAIEnabled)
                                l_Skovald->AI()->DoAction(0);
                        }
                    }

                    me->DespawnOrUnsummon();
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_CLAIM_THE_AEGIS)
                {
                    DoCast(p_Caster, 193988, true);
                    me->DespawnOrUnsummon(1);
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skovald_aegis_of_aggramarAI(p_Creature);
        }
};

/// Honored Ancestor - 101326
class npc_skovald_honored_ancestor : public CreatureScript
{
    public:
        npc_skovald_honored_ancestor() : CreatureScript("npc_skovald_honored_ancestor") { }

        struct npc_skovald_honored_ancestorAI : public ScriptedAI
        {
            npc_skovald_honored_ancestorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetWalk(true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            uint64 m_OwnerGuid;
            bool m_MoveComplete;
            uint16 m_MoveTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_OwnerGuid = p_Summoner->GetGUID();
                DoCast(me, 199745, true); ///< Visual
                DoCast(me, 199738, true); ///< Visual
                m_MoveTimer = 1000;
                m_MoveComplete = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_MoveComplete)
                {
                    if (m_MoveTimer <= p_Diff)
                    {
                        Unit* l_Owner = Unit::GetUnit(*me, m_OwnerGuid);
                        if (!l_Owner || !l_Owner->IsInWorld() || !l_Owner->isAlive())
                        {
                            me->DespawnOrUnsummon();
                            return;
                        }

                        if (me->GetDistance(l_Owner) < 3.0f)
                        {
                            m_MoveComplete = true;
                            me->DespawnOrUnsummon(3000);
                            DoCast(l_Owner, 199747, true);
                            me->SetVisible(false);
                        }
                        else
                        {
                            if (!me->HasUnitState(UNIT_STATE_STUNNED))
                                me->GetMotionMaster()->MovePoint(1, l_Owner->GetPosition());
                        }

                        m_MoveTimer = 1000;
                    }
                    else
                        m_MoveTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skovald_honored_ancestorAI(p_Creature);
        }
};

/// Flame of Woe - 104822
class npc_skovald_flame_of_woe : public CreatureScript
{
    public:
        npc_skovald_flame_of_woe() : CreatureScript("npc_skovald_flame_of_woe") { }

        enum eSpells
        {
            SpellConsumingFlame         = 221093,
            SpellInfernalFlamesPeriodic = 221094,
        };

        struct npc_skovald_flame_of_woeAI : public ScriptedAI
        {
            npc_skovald_flame_of_woeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.5f);
            }

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoZoneInCombat(me, 100.0f);
                events.ScheduleEvent(EVENT_1, 1000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(193708))
                {
                    if (me->getVictim() && me->getVictim()->HasAura(193708))
                    {
                        DoMeleeAttackIfReady();
                        return;
                    }

                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        events.ScheduleEvent(EVENT_1, 2000);

                        if (me->getVictim() && me->getVictim()->HasAura(193708))
                            break;

                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                        {
                            DoResetThreat();
                            AttackStart(l_Target);
                            me->AddThreat(l_Target, 100000.0f);
                            DoCast(l_Target, 193708, true);
                        }

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
            return new npc_skovald_flame_of_woeAI(p_Creature);
        }
};

///< Valhalla Bridge - 246144
struct go_hov_valhalla_bridge : public GameObjectAI
{
    explicit go_hov_valhalla_bridge(GameObject* p_Go) : GameObjectAI(p_Go)
    {}

    bool m_Activated;

    std::vector<G3D::Vector2> m_BridgeBounds =
    {
        { 2603.9299f, 547.9202f },
        { 2607.7312f, 510.6500f },
        { 3161.8838f, 510.1758f },
        { 3170.2549f, 528.5620f },
        { 3161.0771f, 547.4631f },
    };

    void Reset() override
    {
        m_Timer = 0;
    }

    void MoveInLineOfSight(Unit* p_Who) override
    {
        if (go->GetGoState() == GOState::GO_STATE_ACTIVE)
            return;

        if (!p_Who->IsPlayer())
            return;

        if (p_Who->GetDistance(go) > 10.0f)
            return;

        if (!p_Who->HasAura(SPELL_ODYNS_BLESSING))
            return;

        go->SetGoState(GO_STATE_ACTIVE);

        uint64 l_WhoGuid = p_Who->GetGUID();
        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_WhoGuid]() -> void
        {
            if (Unit* l_Who = Unit::GetUnit(*go, l_WhoGuid))
            {
                if (InstanceScript* l_Instance = go->GetInstanceScript())
                {
                    if (GameObject* l_Bridge = GameObject::GetGameObject(*go, l_Instance->GetData64(GO_VALHALLA_BRIDGE)))
                        l_Bridge->SetGoState(GO_STATE_ACTIVE);
                }
            }
        });
    }

    void CheckPlayersInBridge()
    {
        Map::PlayerList const & l_Players = go->GetMap()->GetPlayers();

        for (auto l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
        {
            Player* l_Player = l_Itr->getSource();

            if (!l_Player)
                continue;

            if (l_Player->IsInPolygon(m_BridgeBounds))
            {
                if (!l_Player->HasAura(eSharedSpells::SPELL_BIFROST_AURA))
                    l_Player->CastSpell(l_Player, eSharedSpells::SPELL_BIFROST_AURA, true);
            }
            else
                l_Player->RemoveAura(eSharedSpells::SPELL_BIFROST_AURA);
        }
    }

    void UpdateAI(uint32 p_Diff) override
    {
        UpdateOperations(p_Diff);

        if (go->GetGoState() != GOState::GO_STATE_ACTIVE)
            return;

        m_Timer += p_Diff;

        if (m_Timer >= IN_MILLISECONDS)
        {
            m_Timer = 0;
            CheckPlayersInBridge();
        }
    }

    uint32 m_Timer;
};

/// The Gateway of Splendor - 244664

/// Ragnarok - 193826
class spell_skovald_ragnarok : public SpellScriptLoader
{
    public:
        spell_skovald_ragnarok() : SpellScriptLoader("spell_skovald_ragnarok") { }

        class spell_skovald_ragnarok_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skovald_ragnarok_AuraScript);

            uint16 m_CheckTimer = 1500;

            void OnUpdate(uint32 p_Diff, AuraEffect* /*p_AurEff*/)
            {
                if (m_CheckTimer <= p_Diff)
                    m_CheckTimer = 1500;
                else
                {
                    m_CheckTimer -= p_Diff;
                    return;
                }

                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->CastSpell(l_Caster, SPELL_RAGNAROK_DAMAGES, true);
                    l_Caster->CastSpell(l_Caster, SPELL_RAGNAROK_BUFF, true);
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_skovald_ragnarok_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skovald_ragnarok_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Player Aegis of Aggramar - 193743
class spell_skovald_player_aegis : public SpellScriptLoader
{
    public:
        spell_skovald_player_aegis() : SpellScriptLoader("spell_skovald_player_aegis") { }

        class spell_skovald_player_aegis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skovald_player_aegis_AuraScript);

            enum eSpells
            {
                RagnarokDmg     = 193827,
                FelblazeRushDmg = 193660,
                SavageBladeDmg  = 193668
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                    p_Amount = -1; ///< Prevent the aura from being removed by ambiant damage (Dots, pools, etc)
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                SpellInfo const* l_DamagingSpell = p_DmgInfo.GetSpellInfo();
                if (!l_DamagingSpell)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                switch (l_DamagingSpell->Id) ///< Player Aegis of Aggramar is supposed to block only those damages
                {
                    case eSpells::RagnarokDmg:
                    case eSpells::FelblazeRushDmg:
                    case eSpells::SavageBladeDmg:
                        p_AbsorbAmount = p_DmgInfo.GetAmount();
                        break;
                    default:
                        p_AbsorbAmount = 0;
                        break;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_skovald_player_aegis_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_skovald_player_aegis_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skovald_player_aegis_AuraScript();
        }
};

/// Aegis of Aggramar - 193991
class spell_skovald_drop_aegis : public SpellScriptLoader
{
    public:
        spell_skovald_drop_aegis() : SpellScriptLoader("spell_skovald_drop_aegis") { }

        class spell_skovald_drop_aegis_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skovald_drop_aegis_SpellScript);

            void HandleScript(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                PreventHitEffect(p_EffIndex);

                WorldLocation* l_Dest = GetHitDest();
                if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(DATA_SKOVALD) == IN_PROGRESS)
                        l_Caster->CastSpell(l_Dest, 193781, true);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_skovald_drop_aegis_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skovald_drop_aegis_SpellScript();
        }
};

/// Aegis of Aggramar - 193983, 193783
class spell_skovald_aegis_remove : public SpellScriptLoader
{
    public:
        spell_skovald_aegis_remove() : SpellScriptLoader("spell_skovald_aegis_remove") { }

        class spell_skovald_aegis_remove_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skovald_aegis_remove_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Position l_Pos = l_Target->GetPosition();

                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, l_Pos.GetAngle(&g_CenterOfRoom), true);

                l_Target->CastSpell(l_Pos, 193991, true);

                l_Target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Target = GetUnitOwner();
                if (l_Target == nullptr || l_Target->IsPlayer())
                    return;

                /// Infinite amount for Skovald, must absorb only from the front
                p_Amount = -1;
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Unit* l_Attacker = p_DmgInfo.GetActor())
                {
                    /// Don't absorb damages from behind
                    if (!l_Target->isInFront(l_Attacker))
                        p_AbsorbAmount = 0;
                    else
                        p_AbsorbAmount = p_DmgInfo.GetAmount();
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case 193983:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_skovald_aegis_remove_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_skovald_aegis_remove_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                        OnEffectAbsorb += AuraEffectAbsorbFn(spell_skovald_aegis_remove_AuraScript::OnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                        break;
                    default:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_skovald_aegis_remove_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skovald_aegis_remove_AuraScript();
        }
};

/// Aegis of Aggramar (buff) - 193765
class spell_skovald_aegis_buff : public SpellScriptLoader
{
    public:
        spell_skovald_aegis_buff() : SpellScriptLoader("spell_skovald_aegis_buff") { }

        class spell_skovald_aegis_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skovald_aegis_buff_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(DATA_SKOVALD) != IN_PROGRESS)
                        return;
                }

                l_Target->RemoveAura(SPELL_AEGIS_OVERRIDE);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_skovald_aegis_buff_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skovald_aegis_buff_AuraScript();
        }
};

/// Infernal Flames Add - 221094
class spell_skovald_infernal_flames_add : public SpellScriptLoader
{
    public:
        spell_skovald_infernal_flames_add() : SpellScriptLoader("spell_skovald_infernal_flames_add")
        {}

        enum eSpells
        {
            SpellInfernalFlameTrigger   = 221096,
            SpellInfernalFlameRandom    = 221095,
        };

        class spell_skovald_infernal_flames_add_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skovald_infernal_flames_add_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellInfernalFlameRandom, true);
                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellInfernalFlameTrigger, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_skovald_infernal_flames_add_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skovald_infernal_flames_add_AuraScript();
        }
};

/// Bifrost Aura activation
class eventobject_skovald_bifrost : public EventObjectScript
{
    public:
        eventobject_skovald_bifrost() : EventObjectScript("eventobject_skovald_bifrost") { }

        bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/) override
        {
            if (p_Player->m_positionX >= 3185.0f || p_Player->m_positionX <= 2590.0f)
                return false;

            if (!p_Player->HasAura(SPELL_BIFROST_AURA))
                p_Player->CastSpell(p_Player, SPELL_BIFROST_AURA, true);

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_god_king_skovald()
{
    /// Boss
    new boss_god_king_skovald();

    /// Creatures
    new npc_generic_odyn_kings();
    new npc_skovald_aegis_of_aggramar();
    new npc_skovald_honored_ancestor();
    new npc_skovald_flame_of_woe();

    /// GameObject
    RegisterGameObjectAI(go_hov_valhalla_bridge);

    /// Spells
    new spell_skovald_ragnarok();
    new spell_skovald_drop_aegis();
    new spell_skovald_aegis_remove();
    new spell_skovald_aegis_buff();
    new spell_skovald_infernal_flames_add();
    new spell_skovald_player_aegis();

    /// EventObject
    new eventobject_skovald_bifrost();
}
#endif
