////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "boss_ilgynoth.hpp"

enum Says
{
    SAY_AGGRO       = 0,
    SAY_ANNOUNCE    = 4,
    SAY_EVADE       = 5,
    SAY_DEATH       = 6,
    SAY_WAITING     = 7
};

enum Spells
{
    /// Ilgynoth
    SPELL_DARK_RECONSTITUTION       = 210781,
    SPELL_FINAL_TORPOR              = 223121,
    SPELL_CURSED_BLOOD              = 215128,
    SPELL_CURSED_BLOOD_EXPLOSION    = 215143,
    SPELL_DARKEST_NIGHTMARE_AT      = 210786,
    SPELL_KNOCKBACK_AT              = 224929,
    SPELL_INTERFERE_TARGETTING      = 210780,
    SPELL_RECOVERY_TELEPORT         = 222531,
    SPELL_KILL_WHISPER              = 222173, ///< Il'gynoth Kill Whisper

    /// Eye ilgynoth
    SPELL_NIGHTMARE_GAZE            = 210931,
    SPELL_SUM_NIGHTMARE_HORROR      = 210289,

    /// Mythic
    SPELL_DEATH_BLOSSOM             = 218415, ///< Remove AT cast
    SPELL_DEATH_BLOSSOM_AT          = 215761,
    SPELL_DEATH_BLOSSOM_VISUAL      = 215763,
    SPELL_DEATH_BLOSSOM_DMG         = 215836,
    SPELL_DISPERSED_SPORES          = 215845,
    SPELL_VIOLENT_BLOODBURST        = 215971,
    SPELL_VIOLENT_BLOODBURST_AT     = 215975,
    SPELL_SUM_SHRIVELED_EYESTALK    = 216131,

    /// Dominator Tentacle - 105304
    SPELL_NIGHTMARISH_FURY          = 215234,
    SPELL_GROUND_SLAM_DMG           = 208689,
    SPELL_GROUND_SLAM_VISUAL        = 212723,
    SPELL_RUPTURING_ROAR            = 208685,

    /// Deathglare Tentacle - 105322
    SPELL_MIND_FLAY                 = 208697,

    /// Corruptor Tentacle - 105383
    SPELL_SPEW_CORRUPTION           = 208928,
    SPELL_NIGHTMARE_CORRUPTION      = 208931,

    /// Nightmare Horror - 105591
    SPELL_SEEPING_CORRUPTION        = 209387,
    SPELL_EYE_OF_FATE               = 210984,

    /// Nightmare Ichor - 105721
    SPELL_FIXATE                    = 210099,
    SPELL_NIGHTMARE_EXPLOSION       = 209471,
    SPELL_NIGHTMARE_EXPLOSION_PCT   = 210048,
    SPELL_REABSORPTION              = 212942,

    /// Other
    SPELL_SPAWN_BLOOD               = 209965
};

enum eEvents
{
    EVENT_DARK_RECONSTITUTION = 1,
    EVENT_CURSED_BLOOD,
    EVENT_EYE_RESPAWN,
    EVENT_ANNOUNCE,
    EVENT_ANNOUNCE_FINAL,
    EVENT_CHECK_TARGET_DISTANCE,
    EVENT_WHISPER_WAITING,

    /// Mythic
    EVENT_SUM_SHRIVELED_EYESTALK,
    EVENT_DEATH_BLOSSOM,
    EVENT_VIOLENT_BLOODBURST
};

/// Il'gynoth <The Heart of Corruption> - 105393
class boss_ilgynoth : public CreatureScript
{
    public:
        boss_ilgynoth() : CreatureScript("boss_ilgynoth") { }

        struct boss_ilgynothAI : public BossAI
        {
            boss_ilgynothAI(Creature* p_Creature) : BossAI(p_Creature, DATA_ILGYNOTH)
            {
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
                m_AnnText = 1;
                m_Intro = false;
            }

            FormationInfo* m_GroupMember = nullptr;
            bool m_SpecialPhase;
            bool m_FinalTorpor;
            int8 m_AnnText;

            int8 m_CursedBloodCount;
            uint8 m_FirstPhaseCounter;

            bool m_Intro;

            EventMap m_CosmeticEvents;

            void Reset() override
            {
                me->RemoveAura(SPELL_INTERFERE_TARGETTING);

                _Reset();
                summons.DespawnAll();
                RemoveAT();

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    CreateGroupAndSummonTrash();
                });

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);

                if (instance != nullptr)
                {
                    instance->SetData(DATA_EYE_ILGYNOTH, IN_PROGRESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INTERFERE_TARGETTING);
                }

                m_SpecialPhase = false;
                m_FinalTorpor = false;
                DoCast(me, SPELL_KNOCKBACK_AT, true);

                if (IsMythic())
                {
                    /// Il'gynoth's health has been tripled...
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        /// Hackfixed HP (from wowhead)
                        me->SetMaxHealth(1339805184);
                        me->SetFullHealth();
                    });
                }

                TeleportPlayer();

                m_CursedBloodCount = 0;
                m_FirstPhaseCounter = 0;

                m_CosmeticEvents.Reset();
                m_CosmeticEvents.ScheduleEvent(EVENT_WHISPER_WAITING, 10000);
            }

            void CreateGroupAndSummonTrash()
            {
                if (m_GroupMember)
                    sFormationMgr->RemoveCreatureFromGroup(me->GetFormation(), me);

                m_GroupMember = sFormationMgr->CreateCustomFormation(me);

                me->SummonCreature(NPC_EYE_OF_ILGYNOTH, g_SumPos[0]);
                me->SummonCreature(NPC_DOMINATOR_TENTACLE, g_SumPos[1]);
                me->SummonCreature(NPC_DOMINATOR_TENTACLE, g_SumPos[2]);
            }

            void RemoveAT()
            {
                std::list<AreaTrigger*> l_List;
                me->GetAreaTriggerListWithSpellIDInRange(l_List, SPELL_NIGHTMARE_CORRUPTION, 150.0f);

                for (AreaTrigger* l_AT : l_List)
                    l_AT->Despawn();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_CosmeticEvents.Reset();

                /// First P1
                for (SpawnData l_Data : g_SpawnPattern[m_FirstPhaseCounter])
                {
                    AddTimedDelayedOperation(l_Data.SpawnTimer, [this, l_Data]() -> void
                    {
                        if (!l_Data.SpawnEntry)
                            return;

                        Position l_Pos = Position();
                        if (l_Data.SpawnPos.IsNearPosition(&l_Pos, 0.1f))
                            me->SummonCreature(l_Data.SpawnEntry, g_RandomPositions[urand(0, uint32(g_RandomPositions.size() - 1))]);
                        else
                            me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos);
                    });
                }

                DoCast(me, SPELL_INTERFERE_TARGETTING, true);

                _EnterCombat();
                EntryCheckPredicate l_Pred1(NPC_EYE_OF_ILGYNOTH);
                EntryCheckPredicate l_Pred2(NPC_DOMINATOR_TENTACLE);
                summons.DoAction(true, l_Pred1);
                summons.DoAction(true, l_Pred2);
                DoCast(me, SPELL_DARKEST_NIGHTMARE_AT, true);
                TeleportPlayer();
                TalkWhisper(SAY_AGGRO);

                events.ScheduleEvent(EVENT_CHECK_TARGET_DISTANCE, 2000);
            }

            void EnterEvadeMode() override
            {
                /// Idk why, but boss keeps combat state, it causes an EnterEvadeMode
                if (!me->isAlive())
                    return;

                CreatureAI::EnterEvadeMode();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(DATA_ILGYNOTH, EncounterState::FAIL);

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                summons.DespawnAll();
                RemoveAT();

                if (instance)
                {
                    instance->SetData(DATA_EYE_ILGYNOTH, DONE);

                    instance->AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        instance->SetBossState(DATA_ILGYNOTH, EncounterState::DONE);
                    });

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INTERFERE_TARGETTING);
                }

                TalkWhisper(SAY_DEATH);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                /// Possible fix for Il'gynoth suicide preventing players from getting loots
                if (p_SpellInfo && p_SpellInfo->Id == SPELL_CURSED_BLOOD_EXPLOSION)
                    p_Damage = 0;
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                switch (p_Summon->GetEntry())
                {
                    case NPC_EYE_OF_ILGYNOTH:
                    case NPC_DOMINATOR_TENTACLE:
                        if (CreatureGroup* l_Formation = me->GetFormation())
                            l_Formation->AddMember(p_Summon, m_GroupMember);
                        break;
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer) override
            {
                if (p_Summon->GetEntry() == NPC_EYE_OF_ILGYNOTH)
                {
                    events.Reset();
                    m_SpecialPhase = true;

                    if (instance)
                        instance->SetData(DATA_EYE_ILGYNOTH, DONE);

                    /// Second phase, no cast limit
                    if (m_CursedBloodCount)
                        m_CursedBloodCount = -1;

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    events.ScheduleEvent(EVENT_DARK_RECONSTITUTION, 5000);
                    events.ScheduleEvent(EVENT_CURSED_BLOOD, 23000);

                    if (IsMythic())
                    {
                        if (m_FirstPhaseCounter > 0)
                            events.ScheduleEvent(EVENT_SUM_SHRIVELED_EYESTALK, 16000);

                        events.ScheduleEvent(EVENT_VIOLENT_BLOODBURST, 10000);

                        Unit* l_RealKiller = p_Killer->GetAnyOwner() ? p_Killer->GetAnyOwner() : p_Killer;

                        /// ...but defeating the Eye of Il'gynoth will cause Il'gynoth to suffer 33% of her maximum health as damage
                        if (me->GetHealthPct() > 33.0f)
                            l_RealKiller->DealDamage(me, me->CountPctFromMaxHealth(33.0f));
                        else
                            l_RealKiller->Kill(me);
                    }
                }
            }

            void TeleportPlayer()
            {
                std::list<Player*> l_Players;
                GetPlayerListInGrid(l_Players, me, 25.0f);

                for (Player* const& l_Player : l_Players)
                    l_Player->CastSpell(l_Player, SPELL_RECOVERY_TELEPORT, true);
            }

            void SummomRandPos(uint8 p_Count, uint32 p_Entry)
            {
                for (uint8 l_I = 0; l_I < p_Count; l_I++)
                {
                    uint8 l_Rand = urand(1, 2);
                    float l_Angle = frand(0.0f , 2.0f * M_PI);
                    float l_X = g_SumPos[l_Rand].GetPositionX() + (5.0f * std::cos(l_Angle));
                    float l_Y = g_SumPos[l_Rand].GetPositionY() + (5.0f * std::sin(l_Angle));

                    me->SummonCreature(p_Entry, l_X, l_Y, g_SumPos[l_Rand].GetPositionZ());
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                m_CosmeticEvents.Update(p_Diff);

                if (m_CosmeticEvents.ExecuteEvent() == EVENT_WHISPER_WAITING)
                {
                    TalkWhisper(SAY_WAITING);
                    m_CosmeticEvents.ScheduleEvent(EVENT_WHISPER_WAITING, 20000);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_DARK_RECONSTITUTION:
                    {
                        ClearDelayedOperations();

                        DoCast(m_FinalTorpor ? SPELL_FINAL_TORPOR : SPELL_DARK_RECONSTITUTION);
                        m_FinalTorpor = true;
                        events.ScheduleEvent(EVENT_ANNOUNCE_FINAL, 50000);
                        events.ScheduleEvent(EVENT_EYE_RESPAWN, 56000);
                        events.ScheduleEvent(EVENT_ANNOUNCE, 10000);
                        m_AnnText = 1;
                        break;
                    }
                    case EVENT_CURSED_BLOOD:
                    {
                        /// Every fifteen seconds - three times during the phase
                        if (m_CursedBloodCount >= 3)
                            break;

                        /// Don't count during second phase, no limitation
                        if (m_CursedBloodCount != -1)
                            ++m_CursedBloodCount;

                        DoCast(me, SPELL_CURSED_BLOOD, true);
                        events.ScheduleEvent(EVENT_CURSED_BLOOD, 27000);
                        break;
                    }
                    case EVENT_SUM_SHRIVELED_EYESTALK:
                    {
                        Position l_Pos;
                        float l_Angle = 0.0f;
                        for (uint8 l_I = 0; l_I < 6; l_I++)
                        {
                            l_Angle += 1.0f;
                            me->GetNearPosition(l_Pos, 20.0f, l_Angle);
                            me->CastSpell(l_Pos, SPELL_SUM_SHRIVELED_EYESTALK, true);
                        }
                        events.ScheduleEvent(EVENT_SUM_SHRIVELED_EYESTALK, 20000);
                        break;
                    }
                    case EVENT_EYE_RESPAWN:
                    {
                        /// Second P1
                        ++m_FirstPhaseCounter;

                        /// There is no third P1, raid should be wiped after second P2
                        if (m_FirstPhaseCounter > 1)
                            return;

                        for (SpawnData l_Data : g_SpawnPattern[m_FirstPhaseCounter])
                        {
                            AddTimedDelayedOperation(l_Data.SpawnTimer, [this, l_Data]() -> void
                            {
                                if (!l_Data.SpawnEntry)
                                    return;

                                me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos);
                            });
                        }

                        events.Reset();
                        if (instance)
                            instance->SetData(DATA_EYE_ILGYNOTH, IN_PROGRESS);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SummonCreature(NPC_EYE_OF_ILGYNOTH, g_SumPos[0]);
                        TeleportPlayer();
                        TalkWhisper(SAY_EVADE);
                        m_SpecialPhase = false;
                        events.ScheduleEvent(EVENT_CHECK_TARGET_DISTANCE, 2000);
                        break;
                    }
                    case EVENT_ANNOUNCE_FINAL:
                    {
                        Talk(SAY_ANNOUNCE);
                        break;
                    }
                    case EVENT_ANNOUNCE:
                    {
                        TalkWhisper(m_AnnText);
                        if (m_AnnText == 1)
                        {
                            m_AnnText++;
                            events.ScheduleEvent(EVENT_ANNOUNCE, 20000);
                        }
                        break;
                    }
                    case EVENT_CHECK_TARGET_DISTANCE:
                    {
                        if (!m_SpecialPhase)
                            TeleportPlayer();

                        bool l_ClotestPlayers = false;
                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();

                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = me->GetUnit(*me, l_Ref->getUnitGuid()))
                            {
                                if (me->GetDistance(l_Target) < 150.0f)
                                {
                                    l_ClotestPlayers = true;
                                    break;
                                }
                            }
                        }

                        if (!l_ClotestPlayers)
                        {
                            EnterEvadeMode();
                            return;
                        }

                        events.ScheduleEvent(EVENT_CHECK_TARGET_DISTANCE, 2000);
                        break;
                    }
                    case EVENT_VIOLENT_BLOODBURST:
                    {
                        float l_OStep   = M_PI / 6.0f;
                        float l_OStart  = frand(0.0f, 2.0f * M_PI);
                        float l_CurO    = l_OStart;
                        uint8 l_Count   = 0;

                        for (uint8 l_I = 0; l_I < 6; ++l_I)
                        {
                            if (l_Count == 3)
                            {
                                l_OStart    = Position::NormalizeOrientation(l_OStart + M_PI);
                                l_CurO      = l_OStart;
                            }

                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionX   += 10.0f * std::cos(l_CurO);
                            l_Pos.m_positionY   += 10.0f * std::sin(l_CurO);
                            l_Pos.m_orientation  = me->GetAngle(&l_Pos);

                            DoCast(l_Pos, SPELL_VIOLENT_BLOODBURST, true);

                            l_CurO += l_OStep;

                            Position::NormalizeOrientation(l_CurO);

                            ++l_Count;
                        }

                        events.ScheduleEvent(EVENT_VIOLENT_BLOODBURST, 25000);
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                return m_Intro != 0;
            }

            void SetData(uint32 p_Type, uint32 p_Value) override
            {
                m_Intro = p_Value != 0;
            }

            void TalkWhisper(uint8 p_Text)
            {
                Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->GetDistance(me) <= 250.0f)
                            sCreatureTextMgr->SendChat(me, p_Text, l_Player->GetGUID(), CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_ilgynothAI (p_Creature);
        }
};

/// Eye of Il'gynoth - 105906
class npc_eye_of_ilgynoth : public CreatureScript
{
    public:
        npc_eye_of_ilgynoth() : CreatureScript("npc_eye_of_ilgynoth") { }

        struct npc_eye_of_ilgynothAI : public ScriptedAI
        {
            npc_eye_of_ilgynothAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();

                m_IntroDone = false;

                SetCombatMovement(false);
            }

            InstanceScript* m_Instance;

            bool m_IntroDone;

            uint64 m_CollisionBoxGuid;

            bool CanScale() const override
            {
                return false;
            }

            void Reset() override { }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_1, 2000);
                events.ScheduleEvent(EVENT_2, 65000); ///< Summon Nightmare Horror
                events.ScheduleEvent(EVENT_3, 2000);  ///< Safety check for health

                if (me->GetMap()->IsMythic())
                    events.ScheduleEvent(EVENT_DEATH_BLOSSOM, 60000);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                if (m_Instance)
                {
                    if (Creature* l_Boss = Creature::GetCreature(*me, m_Instance->GetData64(NPC_ILGYNOTH)))
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->AI()->EnterEvadeMode();
                    }
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 250.0f))
                {
                    m_IntroDone = true;

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3617, p_Who, nullptr, *p_Who))
                        delete l_Conversation;

                    if (m_Instance)
                    {
                        if (Creature* l_Boss = Creature::GetCreature(*me, m_Instance->GetData64(NPC_ILGYNOTH)))
                        {
                            if (l_Boss->IsAIEnabled)
                                l_Boss->AI()->SetData(0, m_IntroDone);
                        }
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (GameObject* l_Gob = GameObject::GetGameObject(*me, m_CollisionBoxGuid))
                    l_Gob->Delete();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->isInCombat())
                {
                    DoZoneInCombat();
                    DoAction(true);
                }

                if (Creature* l_Boss = p_Summoner->ToCreature())
                {
                    if (l_Boss->IsAIEnabled)
                        m_IntroDone = l_Boss->AI()->GetData() != 0;
                }

                if (GameObject* l_Gob = me->SummonGameObject(GO_EYE_OF_ILGYNOTH_DOOR, { -12818.6f, 12328.9f, -250.654f, 0.680677f }))
                    m_CollisionBoxGuid = l_Gob->GetGUID();
            }

            void DoAction(int32 const p_Action) override
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
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
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 120.0f, true))
                        {
                            DoResetThreat();
                            me->AddThreat(l_Target, 100000.0f);
                            DoCast(l_Target, SPELL_NIGHTMARE_GAZE);
                        }

                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Summoner = me->GetAnyOwner())
                        {
                            if (Creature* l_Horror = l_Summoner->SummonCreature(NPC_NIGHTMARE_HORROR, g_SumPos[3]))
                            {
                                DoCast(l_Horror, SPELL_SUM_NIGHTMARE_HORROR);
                                l_Horror->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                            }
                        }

                        Talk(0);
                        events.ScheduleEvent(EVENT_2, 3 * MINUTE * IN_MILLISECONDS + 40 * IN_MILLISECONDS); ///< 3m40s
                        break;
                    }
                    case EVENT_DEATH_BLOSSOM:
                    {
                        DoCast(SPELL_DEATH_BLOSSOM);
                        if (Unit* l_Summoner = me->GetAnyOwner())
                        {
                            std::list<uint8> l_RandList;
                            for (uint8 l_I = 0; l_I < 22; l_I++)
                                l_RandList.push_back(l_I);

                            JadeCore::Containers::RandomResizeList(l_RandList, 14);

                            for (std::list<uint8>::iterator l_Iter = l_RandList.begin(); l_Iter != l_RandList.end(); ++l_Iter)
                            {
                                if (Creature* l_Blossom = l_Summoner->SummonCreature(NPC_DEATH_BLOSSOM, g_BlossPos[*l_Iter], TEMPSUMMON_TIMED_DESPAWN, 16000))
                                {
                                    l_Blossom->CastSpell(l_Blossom, SPELL_DEATH_BLOSSOM_AT, true);
                                    l_Blossom->CastSpell(l_Blossom, SPELL_DEATH_BLOSSOM_VISUAL, true);
                                }
                            }
                        }
                        events.ScheduleEvent(EVENT_DEATH_BLOSSOM, 100000); ///< 1m40s
                        break;
                    }
                    case EVENT_3:
                    {
                        if (me->GetHealth() == 1)
                        {
                            me->Kill(me);
                            break;
                        }

                        events.ScheduleEvent(EVENT_3, 2000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eye_of_ilgynothAI(p_Creature);
        }
};

/// Dominator Tentacle - 105304
/// Deathglare Tentacle - 105322
/// Corruptor Tentacle - 105383
/// Shriveled Eyestalk - 108821
class npc_ilgynoth_tentacles : public CreatureScript
{
    public:
        npc_ilgynoth_tentacles() : CreatureScript("npc_ilgynoth_tentacles") { }

        struct npc_ilgynoth_tentaclesAI : public ScriptedAI
        {
            npc_ilgynoth_tentaclesAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* m_Instance;
            Position m_GroundPos[73];
            uint8 m_BloodCount;
            uint16 m_GroundCounter;
            uint16 m_GroundTimer;

            void Reset() override
            {
                m_GroundCounter = 0;

                if ((me->GetEntry() == NPC_DOMINATOR_TENTACLE || me->GetEntry() == NPC_SHRIVELED_EYESTALK) && IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                switch (me->GetEntry())
                {
                    case NPC_SHRIVELED_EYESTALK:
                        p_Mod = 1.0f;
                        break;
                    case NPC_DOMINATOR_TENTACLE:
                        p_Mod = 3.4235254f;
                        break;
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoZoneInCombat();

                switch (me->GetEntry())
                {
                    case NPC_DOMINATOR_TENTACLE:
                        events.ScheduleEvent(EVENT_1, 7000);
                        events.ScheduleEvent(EVENT_2, 13000);
                        events.ScheduleEvent(EVENT_3, 4000);
                        break;
                    case NPC_DEATHGLARE_TENTACLE:
                    case NPC_SHRIVELED_EYESTALK:
                        events.ScheduleEvent(EVENT_4, 2000);
                        break;
                    case NPC_CORRUPTOR_TENTACLE:
                        events.ScheduleEvent(EVENT_5, 2000);
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                if (m_Instance)
                {
                    if (Creature* l_Boss = Creature::GetCreature(*me, m_Instance->GetData64(NPC_ILGYNOTH)))
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->AI()->EnterEvadeMode();
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->GetEntry() == NPC_SHRIVELED_EYESTALK)
                    return;

                if (me->GetEntry() == NPC_CORRUPTOR_TENTACLE)
                    m_BloodCount = 1;
                else if (me->GetEntry() == NPC_DOMINATOR_TENTACLE)
                    m_BloodCount = 4;
                else
                    m_BloodCount = 2;

                if (Unit* l_Summoner = me->GetAnyOwner())
                {
                    for (uint8 l_I = 0; l_I < m_BloodCount; l_I++)
                        me->CastSpell(me, SPELL_SPAWN_BLOOD, true, nullptr, nullptr, l_Summoner->GetGUID());
                }

                if (me->GetEntry() == NPC_DOMINATOR_TENTACLE)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void DoAction(int32 const p_Action) override
            {
                if (me->GetEntry() == NPC_DOMINATOR_TENTACLE && m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_GroundTimer = 0;

                if (p_Summoner->isInCombat())
                    DoZoneInCombat();

                if (me->GetEntry() == NPC_SHRIVELED_EYESTALK)
                    p_Summoner->CastSpell(me, SPELL_INTERFERE_TARGETTING, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_GroundTimer)
                {
                    if (m_GroundTimer <= p_Diff)
                    {
                        m_GroundTimer = 100;

                        for (uint8 l_I = 0; l_I < 4; l_I++)
                        {
                            if (m_GroundCounter > 72)
                            {
                                m_GroundCounter = 0;
                                break;
                            }

                            me->CastSpell(m_GroundPos[m_GroundCounter], SPELL_GROUND_SLAM_VISUAL, true);
                            m_GroundCounter++;
                        }

                        if (m_GroundCounter >= 72)
                        {
                            m_GroundTimer = 0;
                            m_GroundCounter = 0;
                        }
                    }
                    else
                        m_GroundTimer -= p_Diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_NIGHTMARISH_FURY))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCastVictim(SPELL_NIGHTMARISH_FURY);
                        events.ScheduleEvent(EVENT_1, 11000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                        {
                            DoCast(l_Target, SPELL_GROUND_SLAM_DMG);
                            float l_Dist = 0.0f;
                            float l_Angle = me->GetRelativeAngle(l_Target);
                            m_GroundCounter = 0;

                            for (uint16 l_I = 0; l_I < 73; l_I++)
                            {
                                me->GetClosePoint(m_GroundPos[l_I].m_positionX, m_GroundPos[l_I].m_positionY, m_GroundPos[l_I].m_positionZ, 1.0f, l_Dist, l_Angle + frand(-0.12f, 0.12f));
                                l_Dist += 0.54f;
                            }
                        }

                        events.ScheduleEvent(EVENT_2, 22000);
                        m_GroundTimer = 2000;
                        break;
                    }
                    case EVENT_3:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_NEAREST, 0, 100.0f, true))
                        {
                            if (!me->IsWithinMeleeRange(l_Target))
                                DoCast(SPELL_RUPTURING_ROAR);
                        }

                        events.ScheduleEvent(EVENT_3, 3000);
                        break;
                    }
                    case EVENT_4:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 120.0f, true))
                            DoCast(l_Target, SPELL_MIND_FLAY);

                        events.ScheduleEvent(EVENT_4, urand(9, 12) * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_5:
                    {
                        DoCast(me, SPELL_SPEW_CORRUPTION, true);
                        events.ScheduleEvent(EVENT_5, 10000);
                        break;
                    }
                    default:
                        break;
                }

                if (me->GetEntry() != NPC_DEATHGLARE_TENTACLE)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ilgynoth_tentaclesAI(p_Creature);
        }
};

/// Nightmare Horror - 105591
class npc_ilgynoth_nightmare_horror : public CreatureScript
{
    public:
        npc_ilgynoth_nightmare_horror() : CreatureScript("npc_ilgynoth_nightmare_horror") { }

        struct npc_ilgynoth_nightmare_horrorAI : public ScriptedAI
        {
            npc_ilgynoth_nightmare_horrorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                m_Instance = me->GetInstanceScript();
            }

            InstanceScript* m_Instance;
            int32 m_Power;
            uint16 m_TickPowerTimer;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();

                me->setPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                m_TickPowerTimer = 1000;
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.81731f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 120.0f);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SUM_NIGHTMARE_HORROR)
                    events.ScheduleEvent(EVENT_1, 5000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAura(SPELL_SEEPING_CORRUPTION);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (Unit* l_Summoner = me->GetAnyOwner())
                {
                    for (uint8 l_I = 0; l_I < 4; l_I++)
                        me->CastSpell(me, SPELL_SPAWN_BLOOD, true, nullptr, nullptr, l_Summoner->GetGUID());
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_TickPowerTimer <= p_Diff)
                {
                    m_TickPowerTimer = 500;

                    if (me->GetReactState() == REACT_AGGRESSIVE)
                    {
                        m_Power = me->GetPower(POWER_ENERGY);

                        if (m_Power < 100)
                            me->SetPower(POWER_ENERGY, m_Power + 5);
                        else
                            DoCast(SPELL_EYE_OF_FATE);
                    }
                }
                else
                    m_TickPowerTimer -=p_Diff;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (m_Instance)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                        DoCast(me, SPELL_SEEPING_CORRUPTION, true);
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
            return new npc_ilgynoth_nightmare_horrorAI(p_Creature);
        }
};

/// Nightmare Ichor - 105721
class npc_ilgynoth_nightmare_ichor : public CreatureScript
{
    public:
        npc_ilgynoth_nightmare_ichor() : CreatureScript("npc_ilgynoth_nightmare_ichor") { }

        struct npc_ilgynoth_nightmare_ichorAI : public ScriptedAI
        {
            npc_ilgynoth_nightmare_ichorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
            }

            bool m_Explosion;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_Explosion = false;
                DoZoneInCombat(me, 120.0f);
                events.ScheduleEvent(EVENT_1, 1000);
                events.ScheduleEvent(EVENT_3, 2000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_FIXATE)
                {
                    DoStopAttack();
                    DoResetThreat();
                    me->AddThreat(p_Target, 100000.0f);
                    AttackStart(p_Target);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_NIGHTMARE_EXPLOSION)
                {
                    DoCast(me, SPELL_NIGHTMARE_EXPLOSION_PCT, true);
                    me->Kill(me);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;

                    if (!m_Explosion)
                    {
                        m_Explosion = true;
                        events.Reset();
                        me->InterruptNonMeleeSpells(false);
                        DoStopAttack();
                        me->StopMoving();
                        DoCast(SPELL_NIGHTMARE_EXPLOSION);
                        ApplyAllImmunities(true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (!me->getVictim() || !me->getVictim()->HasAura(SPELL_FIXATE, me->GetGUID()))
                        {
                            DoCast(me, SPELL_FIXATE, true);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                        }

                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    }
                    case EVENT_3:
                    {
                        if (Unit* l_Summoner = me->GetAnyOwner())
                        {
                            if (me->GetDistance(l_Summoner) <= 20.0f)
                            {
                                m_Explosion = true;
                                DoStopAttack();
                                me->DespawnOrUnsummon(1000);
                                DoCast(me, SPELL_REABSORPTION, true);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                                break;
                            }
                        }

                        events.ScheduleEvent(EVENT_3, 2000);
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
            return new npc_ilgynoth_nightmare_ichorAI(p_Creature);
        }
};

/// Death Blossom - 218415
class spell_ilgynoth_death_blossom : public SpellScriptLoader
{
    public:
        spell_ilgynoth_death_blossom() : SpellScriptLoader("spell_ilgynoth_death_blossom") { }

        class spell_ilgynoth_death_blossom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ilgynoth_death_blossom_SpellScript);

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                {
                    if (GetHitUnit()->HasAura(SPELL_DEATH_BLOSSOM_VISUAL))
                    {
                        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DISPERSED_SPORES);
                        GetHitUnit()->RemoveAurasDueToSpell(SPELL_DEATH_BLOSSOM_VISUAL);
                    }
                    else
                        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DEATH_BLOSSOM_DMG);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ilgynoth_death_blossom_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ilgynoth_death_blossom_SpellScript();
        }
};

/// Cursed Blood - 215128
class spell_ilgynoth_cursed_blood : public SpellScriptLoader
{
    public:
        spell_ilgynoth_cursed_blood() : SpellScriptLoader("spell_ilgynoth_cursed_blood") { }

        class spell_ilgynoth_cursed_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ilgynoth_cursed_blood_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget() || GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(GetTarget(), SPELL_CURSED_BLOOD_EXPLOSION, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_ilgynoth_cursed_blood_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ilgynoth_cursed_blood_AuraScript();
        }
};

/// Nightmare Explosion - 209471
class spell_ilgynoth_nightmare_explosion : public SpellScriptLoader
{
    public:
        spell_ilgynoth_nightmare_explosion() : SpellScriptLoader("spell_ilgynoth_nightmare_explosion") { }

        class spell_ilgynoth_nightmare_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ilgynoth_nightmare_explosion_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster() && !GetCaster()->GetMap()->IsMythic())
                    p_Targets.clear();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ilgynoth_nightmare_explosion_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ilgynoth_nightmare_explosion_SpellScript();
        }
};

/// Nightmare Corruption - 208931
class areatrigger_ilgynoth_nightmare_corruption : public AreaTriggerEntityScript
{
    public:
        areatrigger_ilgynoth_nightmare_corruption() : AreaTriggerEntityScript("areatrigger_ilgynoth_nightmare_corruption") { }

        enum eData
        {
            SpellVisualID = 97623
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, eData::SpellVisualID);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 10000);

            /// Sniffed Values
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1065353216);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1065353216);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 1077936128);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_ilgynoth_nightmare_corruption();
        }
};

/// Violent Bloodburst - 215975
class areatrigger_ilgynoth_violent_bloodburst : public AreaTriggerEntityScript
{
    public:
        areatrigger_ilgynoth_violent_bloodburst() : AreaTriggerEntityScript("areatrigger_ilgynoth_violent_bloodburst") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 10000);

            /// Sniffed Values
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 2);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 6.5f); ///< 55% per second
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1.0f);
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            float l_PctPerMSec = 55.0f / 1000.0f;
            float l_BaseRadius = p_AreaTrigger->GetAreaTriggerInfo().Radius;
            float l_CurrRadius = p_AreaTrigger->GetRadius();

            l_CurrRadius += CalculatePct(l_BaseRadius, l_PctPerMSec * p_Time);

            p_AreaTrigger->SetRadius(std::min(l_CurrRadius, l_BaseRadius * p_AreaTrigger->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4)));
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_ilgynoth_violent_bloodburst();
        }
};

/// Death Blossom - 215761
class areatrigger_ilgynoth_death_blossom : public AreaTriggerEntityScript
{
    public:
        areatrigger_ilgynoth_death_blossom() : AreaTriggerEntityScript("areatrigger_ilgynoth_death_blossom") { }

        std::set<uint64> m_Targets;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            m_Targets.insert(p_Target->GetGUID());
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            m_Targets.erase(p_Target->GetGUID());

            /// If a player is still soaking the Blossom, don't reapply the visual
            if (!m_Targets.empty())
                return true;

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_ilgynoth_death_blossom();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ilgynoth()
{
    /// Boss
    new boss_ilgynoth();

    /// Creatures
    new npc_eye_of_ilgynoth();
    new npc_ilgynoth_tentacles();
    new npc_ilgynoth_nightmare_horror();
    new npc_ilgynoth_nightmare_ichor();

    /// Spells
    new spell_ilgynoth_death_blossom();
    new spell_ilgynoth_nightmare_explosion();
    new spell_ilgynoth_cursed_blood();

    /// AreaTriggers
    new areatrigger_ilgynoth_nightmare_corruption();
    new areatrigger_ilgynoth_violent_bloodburst();
    new areatrigger_ilgynoth_death_blossom();
}
#endif
