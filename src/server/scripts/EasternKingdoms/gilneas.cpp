////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2019 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "gilneas.hpp"

/// @Creature : Prince Liam Greymane - 34850
class npc_prince_liam_greymane_34850 : public CreatureScript
{
    public:
        npc_prince_liam_greymane_34850() : CreatureScript("npc_prince_liam_greymane_34850") { }

        enum eEvents
        {
            SayOne = 1,
            SayTwo,
            SayThree
        };

        struct npc_prince_liam_greymane_34850AI : public ScriptedAI
        {
            npc_prince_liam_greymane_34850AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::Lockdown:
                    {
                        if (Creature* l_PanickedCitizen = GetClosestCreatureWithEntry(me, eCreatures::NpcPanickedCitizen, 15.0f))
                            if (l_PanickedCitizen->IsAIEnabled)
                                l_PanickedCitizen->AI()->PersonalTalk(0, p_Player->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }

            void Reset() override
            {
                CreatureAI::Reset();

                events.ScheduleEvent(eEvents::SayOne, 20000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::SayOne:
                    {
                        Talk(0);

                        events.ScheduleEvent(eEvents::SayTwo, 20000);
                        break;
                    }
                    case eEvents::SayTwo:
                    {
                        Talk(1);

                        events.ScheduleEvent(eEvents::SayThree, 20000);
                        break;
                    }
                    case eEvents::SayThree:
                    {
                        Talk(2);

                        events.ScheduleEvent(eEvents::SayOne, 20000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_prince_liam_greymane_34850AI(p_Creature);
        }
};

/// @Creature : Prince Liam Greymane - 34913
class npc_prince_liam_greymane_34913 : public CreatureScript
{
    public:
        npc_prince_liam_greymane_34913() : CreatureScript("npc_prince_liam_greymane_34913") { }

        struct npc_prince_liam_greymane_34913AI : public ScriptedAI
        {
            enum eEvents
            {
                SayOne = 1,
                SayTwo,
                SayThree,
                SayFour,
                SayFive
            };

            npc_prince_liam_greymane_34913AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Intro = false;
            }

            bool m_Intro;

            void Reset() override
            {
                CreatureAI::Reset();

                if (!m_Intro)
                {
                    m_Intro = true;
                    events.ScheduleEvent(eEvents::SayOne, 20000);
                }

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_RampagingWorgen = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcRampagingWorgen, 10.0f))
                        l_Me->CombatStart(l_RampagingWorgen, true);
                }, 1000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::SayOne:
                    {
                        Talk(0);

                        events.ScheduleEvent(eEvents::SayTwo, 60000);
                        break;
                    }
                    case eEvents::SayTwo:
                    {
                        Talk(1);

                        events.ScheduleEvent(eEvents::SayThree, 60000);
                        break;
                    }
                    case eEvents::SayThree:
                    {
                        Talk(2);

                        events.ScheduleEvent(eEvents::SayFour, 60000);
                        break;
                    }
                    case eEvents::SayFour:
                    {
                        Talk(3);

                        events.ScheduleEvent(eEvents::SayFive, 60000);
                        break;
                    }
                    case eEvents::SayFive:
                    {
                        Talk(4);

                        events.ScheduleEvent(eEvents::SayOne, 60000);
                        break;
                    }
                    default:
                        break;
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_prince_liam_greymane_34913AI(p_Creature);
        }
};

/// @Creature : Gilneas City Guard - 34916
class npc_gilneas_city_guard_34916 : public CreatureScript
{
    public:
        npc_gilneas_city_guard_34916() : CreatureScript("npc_gilneas_city_guard_34916") { }

        struct npc_gilneas_city_guard_34916AI : public ScriptedAI
        {
            npc_gilneas_city_guard_34916AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_RampagingWorgen = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcRampagingWorgen, 10.0f))
                        l_Me->CombatStart(l_RampagingWorgen, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilneas_city_guard_34916AI(p_Creature);
        }
};

/// @Creature : Rampaging Worgen - 34884
class npc_rampaging_worgen_34884 : public CreatureScript
{
    public:
        npc_rampaging_worgen_34884() : CreatureScript("npc_rampaging_worgen_34884") { }

        struct npc_rampaging_worgen_34884AI : public ScriptedAI
        {
            npc_rampaging_worgen_34884AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Enraged = false;
            }

            bool m_Enraged;

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_GilneasCityGuard = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcGilneasCityGuard, 10.0f))
                        l_Me->CombatStart(l_GilneasCityGuard, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && (p_Source->GetEntry() == eCreatures::NpcPrinceLiamGreymane || p_Source->GetEntry() == eCreatures::NpcGilneasCityGuard))
                    p_Damage = 0;

                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rampaging_worgen_34884AI(p_Creature);
        }
};

/// @Creature : Frightened Citizen - 35836
/// @Creature : Frightened Citizen - 34981
class npc_frightened_citizen_35836 : public CreatureScript
{
    public:
        npc_frightened_citizen_35836() : CreatureScript("npc_frightened_citizen_35836") { }

        enum eMoves
        {
            MoveToTalk = 1,
            MoveToDespawn
        };

        struct npc_frightened_citizen_35836AI : public ScriptedAI
        {
            npc_frightened_citizen_35836AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            Position m_Position;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
                m_Position = me->GetPosition();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFrightenedCitizen2:
                    case eActions::ActionFrightenedCitizen1:
                    {
                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            Position l_Pos;
                            l_Me->GetNearPosition(l_Pos, 5.0f, 0.0f);
                            l_Me->GetMotionMaster()->MovePoint(eMoves::MoveToTalk, l_Pos);
                        }, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        Talk(0);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->KilledMonsterCredit(eCreatures::NpcGilneasEvacuation);

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;
                        Position l_Position = m_Position;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID, l_Position]() -> void
                        {
                            Position l_Pos;
                            l_Me->GetNearPosition(l_Pos, 10.0f, 0.0f);
                            l_Me->GetMotionMaster()->MovePoint(eMoves::MoveToDespawn, l_Pos);

                            if (l_Me->GetEntry() == eCreatures::NpcFrightenedCitizen2)
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                    if (Creature* l_RampagingWorgen = l_Player->SummonCreature(eCreatures::NpcRampagingWorgen2, l_Position, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                                        l_RampagingWorgen->CombatStart(l_Player, true);

                        }, 1000);

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_frightened_citizen_35836AI(p_Creature);
        }
};

/// @Creature : Lord Godfrey - 35115
class npc_lord_godfrey_35115 : public CreatureScript
{
    public:
        npc_lord_godfrey_35115() : CreatureScript("npc_lord_godfrey_35115") { }

        struct npc_lord_godfrey_35115AI : public ScriptedAI
        {
            npc_lord_godfrey_35115AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) >= 20.0f)
                    return;

                if (l_Player->IsCreatureInInaccessibleList(me->GetGUID()))
                    return;

                if (l_Player->HasQuest(eQuests::WhileYoureAtIt) && l_Player->GetQuestStatus(eQuests::WhileYoureAtIt) != QUEST_STATUS_COMPLETE)
                {
                    l_Player->AddCreatureInInaccessibleList(me->GetGUID());

                    Creature* l_Me = me;
                    uint64 l_PlayerGUID = l_Player->GetGUID();

                    me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                    {
                        if (l_Me->IsAIEnabled)
                            l_Me->AI()->PersonalTalk(0, l_PlayerGUID);
                    }, 2000);

                    me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                    {
                        if (Creature* l_LordGodfrey = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcKingGennGreymane2, 10.0f))
                            if (l_LordGodfrey->IsAIEnabled)
                                l_LordGodfrey->AI()->PersonalTalk(0, l_PlayerGUID);
                    }, 8000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_godfrey_35115AI(p_Creature);
        }
};

/// @Creature : Bloodfang Worgen - 35118
class npc_bloodfang_worgen_35118 : public CreatureScript
{
    public:
        npc_bloodfang_worgen_35118() : CreatureScript("npc_bloodfang_worgen_35118") { }

        struct npc_bloodfang_worgen_35118AI : public ScriptedAI
        {
            npc_bloodfang_worgen_35118AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Enraged = false;
            }

            bool m_Enraged;

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_GilneasCityGuard = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcGilneanRoyalGuard, 15.0f))
                        l_Me->CombatStart(l_GilneasCityGuard, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && (p_Source->GetEntry() == eCreatures::NpcGilneanRoyalGuard || p_Source->GetEntry() == eCreatures::NpcMyriamSpellwaker || p_Source->GetEntry() == eCreatures::NpcSergeantCleese))
                    p_Damage = 0;

                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_bloodfang_worgen_35118AI(p_Creature);
        }
};

/// @Creature : Gilnean Royal Guard - 35232
class npc_gilnean_royal_guard_35232 : public CreatureScript
{
    public:
        npc_gilnean_royal_guard_35232() : CreatureScript("npc_gilnean_royal_guard_35232") { }

        struct npc_gilnean_royal_guard_35232AI : public ScriptedAI
        {
            npc_gilnean_royal_guard_35232AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_BloodfangWorgen = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcBloodfangWorgen, 10.0f))
                        if (!l_BloodfangWorgen->isInCombat())
                            l_Me->CombatStart(l_BloodfangWorgen, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilnean_royal_guard_35232AI(p_Creature);
        }
};

/// @Creature : Sergeant Cleese - 35839
class npc_sergeant_cleese_35839 : public CreatureScript
{
    public:
        npc_sergeant_cleese_35839() : CreatureScript("npc_sergeant_cleese_35839") { }

        struct npc_sergeant_cleese_35839AI : public ScriptedAI
        {
            npc_sergeant_cleese_35839AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_BloodfangWorgen = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcBloodfangWorgen, 10.0f))
                        l_Me->CombatStart(l_BloodfangWorgen, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_sergeant_cleese_35839AI(p_Creature);
        }
};

/// @Creature : Myriam Spellwaker - 35872
class npc_myriam_spellwaker_35872 : public CreatureScript
{
    public:
        npc_myriam_spellwaker_35872() : CreatureScript("npc_myriam_spellwaker_35872") { }

        struct npc_myriam_spellwaker_35872AI : public ScriptedAI
        {
            npc_myriam_spellwaker_35872AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_BloodfangWorgen = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcBloodfangWorgen, 20.0f))
                        l_Me->CombatStart(l_BloodfangWorgen, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                DoSpellAttackIfReady(eSpells::SpellFrostbolt);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_myriam_spellwaker_35872AI(p_Creature);
        }
};

/// @Creature: Sean Dempsey - 35081
class npc_sean_dempsey_35081 : public CreatureScript
{
    public:
        npc_sean_dempsey_35081() : CreatureScript("npc_sean_dempsey_35081") { }

        struct npc_sean_dempsey_35081AI : public ScriptedAI
        {
            npc_sean_dempsey_35081AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_SummonTimer = 0;
                m_EventTimer = 0;
                m_WaveTimer = urand(20000, 25000);
                m_EventActive = false;
                m_RunOnce = true;
            }

            uint32 m_SummonTimer;
            uint32 m_EventTimer;
            uint32 m_WaveTimer;
            bool m_EventActive;
            bool m_RunOnce;

            void _SummonNextWave()
            {
                if (m_RunOnce)
                {
                    me->SummonCreature(eCreatures::NpcWorgenAlpha1, Positions::g_NorthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(eCreatures::NpcWorgenAlpha2, Positions::g_SouthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    m_RunOnce = false;
                }
                else
                {
                    switch (urand(1, 5))
                    {
                        case 1:
                            me->SummonCreature(eCreatures::NpcWorgenAlpha1, Positions::g_NorthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            me->SummonCreature(eCreatures::NpcWorgenAlpha2, Positions::g_SouthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            break;

                        case 2:
                            for (int l_I = 0; l_I < 5; l_I++)
                            {
                                me->SummonCreature(eCreatures::NpcWorgenRunt1, Positions::g_NorthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                                me->SummonCreature(eCreatures::NpcWorgenRunt1, Positions::g_NorthWestRoof2, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            }
                            break;

                        case 3:
                            for (int l_I = 0; l_I < 5; l_I++)
                            {
                                me->SummonCreature(eCreatures::NpcWorgenRunt2, Positions::g_SouthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                                me->SummonCreature(eCreatures::NpcWorgenRunt2, Positions::g_SouthWestRoof2, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            }
                            break;

                        case 4:
                            me->SummonCreature(eCreatures::NpcWorgenAlpha1, Positions::g_NorthRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            me->SummonCreature(eCreatures::NpcWorgenAlpha2, Positions::g_SouthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            break;
                        case 5:
                            for (int l_I = 0; l_I < 5; l_I++)
                            {
                                me->SummonCreature(eCreatures::NpcWorgenRunt1, Positions::g_NorthWestRoof2, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                                me->SummonCreature(eCreatures::NpcWorgenRunt2, Positions::g_SouthWestRoof1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionSeanDempsey:
                    {
                        m_EventActive = true;
                        m_EventTimer = 118500;
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_EventActive)
                    return;

                if (m_EventTimer <= p_Diff)
                {
                    m_EventActive = false;
                    m_RunOnce = true;
                    return;
                }
                else
                {
                    m_EventTimer -= p_Diff;

                    if (m_SummonTimer <= p_Diff)
                    {
                        _SummonNextWave();
                        m_SummonTimer = m_WaveTimer;
                    }
                    else
                        m_SummonTimer -= p_Diff;
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_sean_dempsey_35081AI(creature);
        }
};

/// @Creature : Lord Darius Crowley - 35077
class npc_lord_darius_crowley_35077 : public CreatureScript
{
    public:
        npc_lord_darius_crowley_35077() : CreatureScript("npc_lord_darius_crowley_35077") { }

        struct npc_lord_darius_crowley_35077AI : public ScriptedAI
        {
            npc_lord_darius_crowley_35077AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::BytheSkinofHisTeeth:
                    {
                        DoCast(p_Player, eSpells::SpellGilneasPrison, true);

                        if (Creature* l_SeanDempsey = GetClosestCreatureWithEntry(me, eCreatures::NpcSeanDempsey, 100.0f))
                            if (l_SeanDempsey->IsAIEnabled)
                                l_SeanDempsey->AI()->DoAction(eActions::ActionSeanDempsey);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_darius_crowley_35077AI(p_Creature);
        }
};

/// @Creature: Worgen Runt - 35188
/// @Creature: Worgen Runt - 35456
class npc_worgen_runt_35188 : public CreatureScript
{
    public:
        npc_worgen_runt_35188() : CreatureScript("npc_worgen_runt_35188") { }

        enum eMoves
        {
            MoveToRoof = 1,
            MoveToJump
        };

        enum ePositions
        {
            None,
            NorthWestRoof1,
            NorthWestRoof2,
            SouthWestRoof1,
            SouthWestRoof2
        };

        struct npc_worgen_runt_35188AI : public ScriptedAI
        {
            npc_worgen_runt_35188AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PositionState = ePositions::None;
                m_Enraged = false;
            }

            uint8 m_PositionState;
            bool m_Enraged;

            void Reset() override
            {
                CreatureAI::Reset();

                if (me->IsNearPosition(&Positions::g_NorthWestRoof1, 5.0f))
                {
                    m_PositionState = ePositions::NorthWestRoof1;
                    me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToRoof, Waypoints::g_NorthWestRoof1Move.data(), Waypoints::g_NorthWestRoof1Move.size(), false);
                }
                else if (me->IsNearPosition(&Positions::g_NorthWestRoof2, 5.0f))
                {
                    m_PositionState = ePositions::NorthWestRoof2;
                    me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToRoof, Waypoints::g_NorthWestRoof2Move.data(), Waypoints::g_NorthWestRoof2Move.size(), false);
                }
                else if (me->IsNearPosition(&Positions::g_SouthWestRoof1, 5.0f))
                {
                    m_PositionState = ePositions::SouthWestRoof1;
                    me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToRoof, Waypoints::g_SouthWestRoof1Move.data(), Waypoints::g_SouthWestRoof1Move.size(), false);
                }
                else if (me->IsNearPosition(&Positions::g_SouthWestRoof2, 5.0f))
                {
                    m_PositionState = ePositions::SouthWestRoof2;
                    me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToRoof, Waypoints::g_SouthWestRoof2Move.data(), Waypoints::g_SouthWestRoof2Move.size(), false);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToRoof:
                    {
                        switch (m_PositionState)
                        {
                            case ePositions::NorthWestRoof1:
                            {
                                me->GetMotionMaster()->MoveJump(-1668.52f + frand(-3.0f, 3.0f), 1439.69f + frand(-3.0f, 3.0f), 53.0f, 20.0f, 22.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            case ePositions::NorthWestRoof2:
                            {
                                me->GetMotionMaster()->MoveJump(-1678.04f + frand(-3.0f, 3.0f), 1450.88f + frand(-3.0f, 3.0f), 53.0f, 20.0f, 22.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            case ePositions::SouthWestRoof1:
                            {
                                me->GetMotionMaster()->MoveJump(-1685.521f + frand(-3.0f, 3.0f), 1458.48f + frand(-3.0f, 3.0f), 53.0f, 20.0f, 22.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            case ePositions::SouthWestRoof2:
                            {
                                me->GetMotionMaster()->MoveJump(-1681.81f + frand(-3.0f, 3.0f), 1445.54f + frand(-3.0f, 3.0f), 53.0f, 20.0f, 22.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eMoves::MoveToJump:
                    {
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        if (Player* l_Player = me->SelectNearestPlayer(50.0f))
                            AttackStart(l_Player, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_worgen_runt_35188AI(creature);
        }
};

/// @Creature: Worgen Alpha - 35170
/// @Creature: Worgen Alpha - 35167
class npc_worgen_alpha_35170 : public CreatureScript
{
    public:
        npc_worgen_alpha_35170() : CreatureScript("npc_worgen_alpha_35170") { }

        enum eMoves
        {
            MoveToRoof = 1,
            MoveToJump
        };

        enum ePositions
        {
            None,
            NorthWestRoof1,
            NorthRoof1,
            SouthWestRoof1,
            SouthWestRoof2
        };

        struct npc_worgen_alpha_35170AI : public ScriptedAI
        {
            npc_worgen_alpha_35170AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PositionState = ePositions::None;
                m_Enraged = false;
            }

            uint8 m_PositionState;
            bool m_Enraged;

            void Reset() override
            {
                CreatureAI::Reset();

                if (me->IsNearPosition(&Positions::g_NorthWestRoof1, 5.0f))
                {
                    m_PositionState = ePositions::NorthWestRoof1;
                    me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToRoof, Waypoints::g_NorthWestRoof1Move.data(), Waypoints::g_NorthWestRoof1Move.size(), false);
                }
                else if (me->IsNearPosition(&Positions::g_NorthRoof1, 5.0f))
                {
                    m_PositionState = ePositions::NorthRoof1;
                    me->GetMotionMaster()->MovePoint(eMoves::MoveToRoof, Positions::g_NorthRoof1Move, false);
                }
                else if (me->IsNearPosition(&Positions::g_SouthWestRoof1, 5.0f))
                {
                    m_PositionState = ePositions::SouthWestRoof1;
                    me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToRoof, Waypoints::g_SouthWestRoof1Move.data(), Waypoints::g_SouthWestRoof1Move.size(), false);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToRoof:
                    {
                        switch (m_PositionState)
                        {
                            case ePositions::NorthWestRoof1:
                            {
                                me->GetMotionMaster()->MoveJump(-1668.52f + frand(-3.0f, 3.0f), 1439.69f + frand(-3.0f, 3.0f), 53.0f, 20.0f, 22.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            case ePositions::NorthRoof1:
                            {
                                me->GetMotionMaster()->MoveJump(-1660.17f + frand(-3.0f, 3.0f), 1429.55f + frand(-3.0f, 3.0f), 53.0f, 22.0f, 20.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            case ePositions::SouthWestRoof1:
                            {
                                me->GetMotionMaster()->MoveJump(-1685.521f + frand(-3.0f, 3.0f), 1458.48f + frand(-3.0f, 3.0f), 53.0f, 20.0f, 22.0f, me->GetOrientation(), eMoves::MoveToJump);
                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eMoves::MoveToJump:
                    {
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        if (Player* l_Player = me->SelectNearestPlayer(50.0f))
                            AttackStart(l_Player, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_worgen_alpha_35170AI(creature);
        }
};

/// @Creature : Josiah Avery - 35369
class npc_josiah_avery_35369 : public CreatureScript
{
    public:
        npc_josiah_avery_35369() : CreatureScript("npc_josiah_avery_35369") { }

        struct npc_josiah_avery_35369AI : public ScriptedAI
        {
            npc_josiah_avery_35369AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) >= 10.0f)
                    return;

                if (l_Player->IsCreatureInInaccessibleList(me->GetGUID()))
                    return;

                if (l_Player->GetQuestStatus(eQuests::TheRebelLordsArsenal) == QUEST_STATUS_COMPLETE)
                {
                    l_Player->AddCreatureInInaccessibleList(me->GetGUID());
                    PersonalTalk(0, l_Player->GetGUID());
                }
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::TheRebelLordsArsenal:
                    {
                        p_Player->SummonCreature(eCreatures::NpcGilneanMastiff, Positions::g_GilneanMastiff1, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 5000, 0, p_Player->GetGUID());
                        p_Player->SummonCreature(eCreatures::NpcGilneanMastiff, Positions::g_GilneanMastiff2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 5000, 0, p_Player->GetGUID());

                        if (Creature* l_LornaCrowley = p_Player->SummonCreature(eCreatures::NpcLornaCrowley, Positions::g_LornaCrowley1, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 5000, 0, p_Player->GetGUID()))
                            if (l_LornaCrowley->IsAIEnabled)
                                l_LornaCrowley->AI()->DoAction(eActions::ActionLornaCrowley);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_josiah_avery_35369AI(p_Creature);
        }
};

/// @Creature : Lorna Crowley - 35378
class npc_lorna_crowley_35378 : public CreatureScript
{
    public:
        npc_lorna_crowley_35378() : CreatureScript("npc_lorna_crowley_35378") { }

        struct npc_lorna_crowley_35378AI : public ScriptedAI
        {
            npc_lorna_crowley_35378AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::FromtheShadows:
                    {
                        p_Player->CastSpell(p_Player->GetPosition(), eSpells::SpellSummonGilneanMastiff, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionLornaCrowley:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;
                        uint64 l_JosiahAveryGUID = 0;

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGUID))
                        {
                            if (Creature* l_JosiahAvery = l_Player->SummonCreature(eCreatures::NpcJosiahAvery, Positions::g_JosiahAvery1, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 10000, 0, l_Player->GetGUID()))
                            {
                                l_JosiahAveryGUID = l_JosiahAvery->GetGUID();
                                l_JosiahAvery->SetReactState(ReactStates::REACT_PASSIVE);
                                l_JosiahAvery->CastSpell(l_Player, eSpells::SpellCombatAttack1H, true);
                            }
                        }

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->GetMotionMaster()->MoveReverseJump(Positions::g_JosiahPlayerJump, 1069, 14.00118f, 0);
                        }, 500);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            {
                                if (Creature* l_JosiahAveryTrigger = l_Player->SummonCreature(eCreatures::NpcJosiahEventTrigger, Positions::g_JosiahAvery1, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 5000, 0, l_Player->GetGUID()))
                                    if (l_JosiahAveryTrigger->IsAIEnabled)
                                        l_JosiahAveryTrigger->AI()->PersonalTalk(0, l_Player->GetGUID());
                            }
                        }, 1500);

                        me->AddDelayedEvent([l_Me, l_JosiahAveryGUID]() -> void
                        {
                            if (Creature* l_JosiashAvery = ObjectAccessor::GetCreature(*l_Me, l_JosiahAveryGUID))
                                l_JosiashAvery->GetMotionMaster()->MoveReverseJump(Positions::g_JosiahAveryJump, 1069, 14.00118f, 0);
                        }, 2500);

                        me->AddDelayedEvent([l_Me, l_JosiahAveryGUID]() -> void
                        {
                            if (Creature* l_JosiashAvery = ObjectAccessor::GetCreature(*l_Me, l_JosiahAveryGUID))
                                l_Me->CastSpell(l_JosiashAvery, eSpells::SpellShoot, true);
                        }, 3000);
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lorna_crowley_35378AI(p_Creature);
        }
};

/// @Creature: Gilnean Mastiff - 35631
class npc_gilnean_mastiff_35631 : public CreatureScript
{
    public:
        npc_gilnean_mastiff_35631() : CreatureScript("npc_gilnean_mastiff_35631") { }

        enum eMoves
        {
            MoveToBloodfang = 1
        };

        struct npc_gilnean_mastiff_35631AI : public ScriptedAI
        {
            npc_gilnean_mastiff_35631AI(Creature* creature) : ScriptedAI(creature)
            {
                m_PlayerGUID = 0;
                m_BloodfangLurkerGUID = 0;
            }

            uint64 m_PlayerGUID;
            uint64 m_BloodfangLurkerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void Reset() override
            {
                me->GetCharmInfo()->InitEmptyActionBar(false);
                me->GetCharmInfo()->SetActionBar(0, eSpells::SpellAttackLurker, ActiveStates::ACT_PASSIVE);
                me->SetReactState(ReactStates::REACT_DEFENSIVE);
                me->GetCharmInfo()->SetIsFollowing(true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    if (!l_Player->HasQuest(eQuests::FromtheShadows) || l_Player->isDead())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }

                if (!UpdateVictim())
                {
                    me->GetCharmInfo()->SetIsFollowing(true);
                    me->SetReactState(ReactStates::REACT_DEFENSIVE);
                    return;
                }

                DoMeleeAttackIfReady();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellAttackLurker)
                {
                    if (Creature* l_BloodfangLurker = GetClosestCreatureWithEntry(me, eCreatures::NpcBloodfangLurker, 30.0f))
                    {
                        me->GetMotionMaster()->MoveReverseJump(l_BloodfangLurker->GetPosition(), 1000, 14.0f, eMoves::MoveToBloodfang);
                        m_BloodfangLurkerGUID = l_BloodfangLurker->GetGUID();
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToBloodfang:
                    {
                        if (Creature* l_BloodfangLurker = ObjectAccessor::GetCreature(*me, m_BloodfangLurkerGUID))
                        {
                            l_BloodfangLurker->RemoveAura(eSpells::SpellShadowstalkerStealth);
                            l_BloodfangLurker->AddThreat(me, 1.0f);
                            me->AddThreat(l_BloodfangLurker, 1.0f);
                            AttackStart(l_BloodfangLurker);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Who*/) override
            {
                me->DespawnOrUnsummon(0);
            }

            void KilledUnit(Unit* /*p_Who*/) override
            {
                Reset();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gilnean_mastiff_35631AI(creature);
        }
};

/// @Creature : King Genn Greymane - 35550
class npc_king_genn_greymane_35550 : public CreatureScript
{
    public:
        npc_king_genn_greymane_35550() : CreatureScript("npc_king_genn_greymane_35550") { }

        enum eEvents
        {
            SayOne = 1,
            SayTwo,
            SayThree
        };

        struct npc_king_genn_greymane_35550AI : public ScriptedAI
        {
            npc_king_genn_greymane_35550AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                events.ScheduleEvent(eEvents::SayOne, 20000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::SayOne:
                    {
                        Talk(0);
                        _SummonWaves();

                        events.ScheduleEvent(eEvents::SayTwo, 20000);
                        break;
                    }
                    case eEvents::SayTwo:
                    {
                        Talk(1);
                        _SummonWaves();

                        events.ScheduleEvent(eEvents::SayThree, 20000);
                        break;
                    }
                    case eEvents::SayThree:
                    {
                        Talk(2);
                        _SummonWaves();

                        events.ScheduleEvent(eEvents::SayOne, 20000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void _SummonWaves()
            {
                for (int l_I = 0; l_I < 5; l_I++)
                {
                    if (Creature* l_BloodfangRipperRight = me->SummonCreature(eCreatures::NpcBloodfangRipper, -1785.92f + frand(-10.0f, 10.0f), 1372.57f + frand(-10.0f, 10.0f), 20.00f, 2.128309f, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
                    {
                        l_BloodfangRipperRight->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        l_BloodfangRipperRight->GetMotionMaster()->MovePoint(1, Positions::g_BloodfangRight);
                    }
                }

                for (int l_I = 0; l_I < 5; l_I++)
                {
                    if (Creature* l_BloodfangRipperLeft = me->SummonCreature(eCreatures::NpcBloodfangRipper, -1758.61f + frand(-10.0f, 10.0f), 1386.99f + frand(-10.0f, 10.0f), 20.00f, 2.112601f, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
                    {
                        l_BloodfangRipperLeft->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        l_BloodfangRipperLeft->GetMotionMaster()->MovePoint(1, Positions::g_BloodfangLeft);
                    }
                }
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::SaveKrennanAranas:
                    {
                        if (Creature* l_KingGreymanesHorse = p_Player->SummonCreature(eCreatures::NpcKingGreymanesHorse, Positions::g_GreymanesHorse1, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            if (l_KingGreymanesHorse->IsAIEnabled)
                                l_KingGreymanesHorse->AI()->DoAction(eActions::ActionKingGreymanesHorse);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_king_genn_greymane_35550AI(p_Creature);
        }
};

/// @Creature : King Greymane's Horse - 35905
class npc_king_greymanes_horse_35905 : public CreatureScript
{
    public:
        npc_king_greymanes_horse_35905() : CreatureScript("npc_king_greymanes_horse_35905") { }

        enum eMoves
        {
            MoveToJump = 1,
            MoveToKrennan,
            MoveDespawn
        };

        struct npc_king_greymanes_horse_35905AI : public VehicleAI
        {
            npc_king_greymanes_horse_35905AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_KillCredit = false;
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            bool m_KillCredit;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionKingGreymanesHorse:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->EnterVehicle(l_Me, 0);
                        }, 500);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetSpeed(UnitMoveType::MOVE_RUN, 1.5f, true);
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToJump, Waypoints::g_GreymanesHorseStart.data(), Waypoints::g_GreymanesHorseStart.size(), false);
                        }, 1500);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToJump:
                    {
                        me->GetMotionMaster()->MoveReverseJump(Positions::g_GreymanesHorse2, 2235, 9.609176f, eMoves::MoveToKrennan);
                        break;
                    }
                    case eMoves::MoveToKrennan:
                    {
                        me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        PersonalTalk(0, m_PlayerGUID);

                        if (Creature* l_KrennanAranasTree = GetClosestCreatureWithEntry(me, eCreatures::NpcKrennanAranasTree, 30.0f))
                            if (l_KrennanAranasTree->IsAIEnabled)
                                l_KrennanAranasTree->AI()->PersonalTalk(0, m_PlayerGUID);

                        break;
                    }
                    case eMoves::MoveDespawn:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->ExitVehicle();

                        if (Vehicle* l_Vehicle = me->GetVehicleKit())
                        {
                            if (Unit* l_KrennanAranasUnit = l_Vehicle->GetPassenger(1))
                            {
                                if (Creature* l_KrennanAranas = l_KrennanAranasUnit->ToCreature())
                                {
                                    l_KrennanAranas->ExitVehicle();
                                    l_KrennanAranas->GetMotionMaster()->MoveReverseJump(Positions::g_KrennanAranas1, 1000, 5.0f, 0);
                                    l_KrennanAranas->DespawnOrUnsummon(6000);

                                    if (l_KrennanAranas->IsAIEnabled)
                                        l_KrennanAranas->AI()->PersonalTalk(0, m_PlayerGUID);
                                }
                            }
                        }

                        me->DespawnOrUnsummon(1000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    if (l_Player->GetQuestStatus(eQuests::SaveKrennanAranas) == QUEST_STATUS_COMPLETE && !m_KillCredit)
                    {
                        m_KillCredit = true;

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetControlled(false, UnitState::UNIT_STATE_ROOT);
                            l_Me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                            l_Me->SetSpeed(UnitMoveType::MOVE_RUN, 1.5f, true);
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveDespawn, Waypoints::g_GreymanesHorseEnd.data(), Waypoints::g_GreymanesHorseEnd.size(), false);
                        }, 1500);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_king_greymanes_horse_35905AI(p_Creature);
        }
};

/// @Creature : Krennan Aranas - 35907
class npc_krennan_aranas_35907 : public CreatureScript
{
    public:
        npc_krennan_aranas_35907() : CreatureScript("npc_krennan_aranas_35907") { }

        struct npc_krennan_aranas_35907AI : public ScriptedAI
        {
            npc_krennan_aranas_35907AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (Unit* l_KingGreymanesHorse = l_Player->GetVehicleBase())
                    me->EnterVehicle(l_KingGreymanesHorse, 1);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_krennan_aranas_35907AI(p_Creature);
        }
};

/// @Creature : Lord Godfrey - 35906
class npc_lord_godfrey_35906 : public CreatureScript
{
    public:
        npc_lord_godfrey_35906() : CreatureScript("npc_lord_godfrey_35906") { }

        struct npc_lord_godfrey_35906AI : public PassiveAI
        {
            npc_lord_godfrey_35906AI(Creature* p_Creature) : PassiveAI(p_Creature)
            {
                me->SetInPhase(1429400, true, true);
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::SaveKrennanAranas:
                    {
                        PersonalTalk(0, p_Player->GetGUID());

                        if (Creature* l_CommandeeredCannon = GetClosestCreatureWithEntry(me, eCreatures::NpcCommandeeredCannon, 50.0f))
                            if (l_CommandeeredCannon->GetNameTag() == "CommandeeredCannonEvent")
                                if (l_CommandeeredCannon->IsAIEnabled)
                                    l_CommandeeredCannon->AI()->DoAction(eActions::ActionCommandeeredCannon);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_godfrey_35906AI(p_Creature);
        }
};

/// @Creature : Commandeered Cannon - 35914
class npc_commandeered_cannon_35914 : public CreatureScript
{
    public:
        npc_commandeered_cannon_35914() : CreatureScript("npc_commandeered_cannon_35914") { }

        struct npc_commandeered_cannon_35914AI : public PassiveAI
        {
            npc_commandeered_cannon_35914AI(Creature* p_Creature) : PassiveAI(p_Creature) { }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCommandeeredCannon:
                    {
                        for (int l_I = 0; l_I < 12; l_I++)
                        {
                            if (Creature* l_BloodfangRipper = me->SummonCreature(eCreatures::NpcBloodfangRipper2, -1734.44f + frand(-10.0f, 10.0f), 1361.24f + frand(-10.0f, 10.0f), 20.00f, 2.399998f, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                            {
                                l_BloodfangRipper->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                l_BloodfangRipper->GetMotionMaster()->MovePoint(1, Positions::g_BloodfangLeft);
                            }
                        }

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            for (int l_I = 0; l_I < 12; l_I++)
                            {
                                if (Creature* l_BloodfangRipper = l_Me->SummonCreature(eCreatures::NpcBloodfangRipper2, -1734.44f + frand(-10.0f, 10.0f), 1361.24f + frand(-10.0f, 10.0f), 20.00f, 2.399998f, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                                {
                                    l_BloodfangRipper->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                    l_BloodfangRipper->GetMotionMaster()->MovePoint(1, Positions::g_BloodfangLeft);
                                }
                            }
                        }, 2000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->CastSpell(l_Me, eSpells::SpellCannonFire, true);
                        }, 4000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->CastSpell(l_Me, eSpells::SpellCannonFire, true);
                        }, 7000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commandeered_cannon_35914AI(p_Creature);
        }
};

/// @Creature : Afflicted Gilnean - 50471
class npc_afflicted_gilnean_50471 : public CreatureScript
{
    public:
        npc_afflicted_gilnean_50471() : CreatureScript("npc_afflicted_gilnean_50471") { }

        struct npc_afflicted_gilnean_50471AI : public ScriptedAI
        {
            npc_afflicted_gilnean_50471AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Enraged = false;
            }

            bool m_Enraged;

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_GilneasCityGuard = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcGilneasCityGuard2, 10.0f))
                        l_Me->CombatStart(l_GilneasCityGuard, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && p_Source->GetEntry() == eCreatures::NpcGilneasCityGuard2)
                    p_Damage = 0;

                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_afflicted_gilnean_50471AI(p_Creature);
        }
};

/// @Creature : Gilneas City Guard - 50474
class npc_gilneas_city_guard_50474 : public CreatureScript
{
    public:
        npc_gilneas_city_guard_50474() : CreatureScript("npc_gilneas_city_guard_50474") { }

        struct npc_gilneas_city_guard_50474AI : public ScriptedAI
        {
            npc_gilneas_city_guard_50474AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_AfflictedGilnean = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcAfflictedGilnean, 10.0f))
                        if (!l_AfflictedGilnean->isInCombat())
                            l_Me->CombatStart(l_AfflictedGilnean, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilneas_city_guard_50474AI(p_Creature);
        }
};

/// @Creature : King Genn Greymane - 35911
class npc_king_genn_greymane_35911 : public CreatureScript
{
    public:
        npc_king_genn_greymane_35911() : CreatureScript("npc_king_genn_greymane_35911") { }

        struct npc_king_genn_greymane_35911AI : public ScriptedAI
        {
            npc_king_genn_greymane_35911AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::TimetoRegroup:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = p_Player->GetGUID();

                        PersonalTalk(0, l_PlayerGUID);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Creature* l_LordDariusCrowley = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcLordDariusCrowley, 20.0f))
                                if (l_LordDariusCrowley->IsAIEnabled)
                                    l_LordDariusCrowley->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 9000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Creature* l_PrinceLiamGreymane = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcPrinceLiamGreymane2, 20.0f))
                                if (l_PrinceLiamGreymane->IsAIEnabled)
                                    l_PrinceLiamGreymane->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 17000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Creature* l_LordDariusCrowley = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcLordDariusCrowley, 20.0f))
                                if (l_LordDariusCrowley->IsAIEnabled)
                                    l_LordDariusCrowley->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 23000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Creature* l_LordDariusCrowley = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcLordDariusCrowley, 20.0f))
                                if (l_LordDariusCrowley->IsAIEnabled)
                                    l_LordDariusCrowley->AI()->PersonalTalk(2, l_PlayerGUID);
                        }, 33000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 39000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_king_genn_greymane_35911AI(p_Creature);
        }
};

/// @Creature : Crowley's Horse - 44427
class npc_crowleys_horse_44427 : public CreatureScript
{
    public:
        npc_crowleys_horse_44427() : CreatureScript("npc_crowleys_horse_44427") { }

        struct npc_crowleys_horse_44427AI : public ScriptedAI
        {
            npc_crowleys_horse_44427AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                uint64 l_GUID = me->GetGUID();
                uint64 l_CrowleysHordeGUID;

                if (l_Player->IsCreatureInInaccessibleList(l_GUID))
                    return;

                l_Player->AddCreatureInInaccessibleList(l_GUID);

                if (Creature* l_CrowleysHorse = l_Player->SummonCreature(eCreatures::NpcCrowleysHorse2, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    if (l_CrowleysHorse->IsAIEnabled)
                    {
                        l_CrowleysHorse->AI()->DoAction(eActions::ActionCrowleysHorse);
                        l_CrowleysHordeGUID = l_CrowleysHorse->GetGUID();
                    }

                if (Creature* l_LordDariusCrowley = l_Player->SummonCreature(eCreatures::NpcLordDariusCrowley2, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    if (Creature* l_CrowleysHorse = ObjectAccessor::GetCreature(*l_LordDariusCrowley, l_CrowleysHordeGUID))
                        l_LordDariusCrowley->EnterVehicle(l_CrowleysHorse, 1);

                l_Player->AddDelayedEvent([l_Player, l_GUID]() -> void
                {
                    l_Player->RemoveCreatureFromInaccessibleList(l_GUID);
                }, 500);
            }

            void OnBuildDynFlags(uint32& p_DynFlags, Player* p_Player) override
            {
                if (!p_Player->HasQuest(eQuests::Sacrifices))
                {
                    p_DynFlags = UnitDynFlags::UNIT_DYNFLAG_REFER_A_FRIEND;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_crowleys_horse_44427AI(p_Creature);
        }
};

/// @Creature : Crowley's Horse - 35231
class npc_crowleys_horse_35231 : public CreatureScript
{
    public:
        npc_crowleys_horse_35231() : CreatureScript("npc_crowleys_horse_35231") { }

        enum eMoves
        {
            MoveToJump1 = 1,
            MoveJump1,
            MoveToJump2,
            MoveJump2,
            MoveToEnd,
            MoveToDespawn
        };

        struct npc_crowleys_horse_35231AI : public VehicleAI
        {
            npc_crowleys_horse_35231AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCrowleysHorse:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->EnterVehicle(l_Me, 0);
                        }, 500);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToJump1, Waypoints::g_CrowleysHorseStart.data(), Waypoints::g_CrowleysHorseStart.size(), false);
                        }, 2000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToJump1:
                    {
                        me->GetMotionMaster()->MoveReverseJump(Positions::g_CrowleysHorse1, 1919, 20.0f, eMoves::MoveJump1);

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Vehicle* l_Vehicle = l_Me->GetVehicleKit())
                                if (Unit* l_LordDariusCrowleyUnit = l_Vehicle->GetPassenger(1))
                                    if (Creature* l_LordDariusCrowley = l_LordDariusCrowleyUnit->ToCreature())
                                        l_LordDariusCrowley->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 2000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Vehicle* l_Vehicle = l_Me->GetVehicleKit())
                                if (Unit* l_LordDariusCrowleyUnit = l_Vehicle->GetPassenger(1))
                                    if (Creature* l_LordDariusCrowley = l_LordDariusCrowleyUnit->ToCreature())
                                        l_LordDariusCrowley->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 22000);
                        break;
                    }
                    case eMoves::MoveJump1:
                    {
                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetSpeed(UnitMoveType::MOVE_RUN, 1.5f, true);
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToJump2, Waypoints::g_CrowleysHorseJump1.data(), Waypoints::g_CrowleysHorseJump1.size(), false);
                        }, 500);

                        break;
                    }
                    case eMoves::MoveToJump2:
                    {
                        me->GetMotionMaster()->MoveReverseJump(Positions::g_CrowleysHorse2, 1596, 20.0f, eMoves::MoveJump2);
                        break;
                    }
                    case eMoves::MoveJump2:
                    {
                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetSpeed(UnitMoveType::MOVE_RUN, 1.5f, true);
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToEnd, Waypoints::g_CrowleysHorseJump2.data(), Waypoints::g_CrowleysHorseJump2.size(), false);
                        }, 500);

                        break;
                    }
                    case eMoves::MoveToEnd:
                    {
                        me->getThreatManager().resetAllAggro();
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_CrowleysHorseEnd.data(), Waypoints::g_CrowleysHorseEnd.size(), false);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                        {
                            l_Player->getThreatManager().resetAllAggro();
                            l_Player->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        }

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        for (auto l_Itr : me->getThreatManager().getThreatList())
                            if (Unit* l_Unit = l_Itr->getTarget())
                                l_Unit->RemoveAurasDueToSpell(eSpells::SpellThrowTorch);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                        {
                            for (auto l_Itr : l_Player->getThreatManager().getThreatList())
                                if (Unit* l_Unit = l_Itr->getTarget())
                                    l_Unit->RemoveAurasDueToSpell(eSpells::SpellThrowTorch);

                            l_Player->ExitVehicle();
                        }

                        if (Vehicle* l_Vehicle = me->GetVehicleKit())
                        {
                            if (Unit* l_LordDariusCrowley = l_Vehicle->GetPassenger(1))
                                if (l_LordDariusCrowley->ToCreature())
                                    l_LordDariusCrowley->ToCreature()->DespawnOrUnsummon(2000);
                        }

                        me->DespawnOrUnsummon(2000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                Player* l_Player = p_Passenger->ToPlayer();
                if (!l_Player)
                    return;

                if (!p_Apply)
                {
                    l_Player->getThreatManager().resetAllAggro();
                    l_Player->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(80))
                    p_Damage = 0;
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        if (Vehicle* l_Vehicle = me->GetVehicleKit())
                        {
                            if (Unit* l_LordDariusCrowley = l_Vehicle->GetPassenger(1))
                                if (l_LordDariusCrowley->ToCreature())
                                    l_LordDariusCrowley->ToCreature()->DespawnOrUnsummon(0);
                        }

                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_crowleys_horse_35231AI(p_Creature);
        }
};

/// @Creature : Bloodfang Stalker - 35229
class npc_bloodfang_stalker_35229 : public CreatureScript
{
    public:
        npc_bloodfang_stalker_35229() : CreatureScript("npc_bloodfang_stalker_35229") { }

        struct npc_bloodfang_stalker_35229AI : public ScriptedAI
        {
            npc_bloodfang_stalker_35229AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Enraged = false;
            }

            bool m_Enraged;

            void Reset() override
            {
                if (me->GetDefaultMovementType() != MovementGeneratorType::RANDOM_MOTION_TYPE)
                {
                    Creature* l_Me = me;
                    me->AddDelayedEvent([l_Me]() -> void
                    {
                        if (Creature* l_GilneasCityGuard = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcNorthgateRebel, 5.0f))
                            if (!l_GilneasCityGuard->isInCombat())
                                l_Me->CombatStart(l_GilneasCityGuard, true);
                    }, 1000);
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && p_Source->GetEntry() == eCreatures::NpcNorthgateRebel)
                    p_Damage = 0;

                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode /*p_RemoveMode*/) override
            {
                if (p_SpellID != eSpells::SpellThrowTorch)
                    return;

                EnterEvadeMode();
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id != eSpells::SpellThrowTorch)
                    return;

                l_Player->KilledMonsterCredit(eCreatures::NpcBloodfangStalkerKC);
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcTobiasMistmantle)
                    return false;

                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcCrowleysHorse3)
                    return false;

                return true;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcTobiasMistmantle)
                    return false;

                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcCrowleysHorse3)
                    return false;

                return true;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_bloodfang_stalker_35229AI(p_Creature);
        }
};

/// @Creature : Northgate Rebel - 36057
class npc_northgate_rebel_36057 : public CreatureScript
{
    public:
        npc_northgate_rebel_36057() : CreatureScript("npc_northgate_rebel_36057") { }

        struct npc_northgate_rebel_36057AI : public ScriptedAI
        {
            npc_northgate_rebel_36057AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_BloodfangStalker = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcBloodfangStalker, 5.0f))
                        if (!l_BloodfangStalker->isInCombat())
                            l_Me->CombatStart(l_BloodfangStalker, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_northgate_rebel_36057AI(p_Creature);
        }
};

/// @Creature : Rebel Cannon - 35317
class npc_rebel_cannon_35317 : public CreatureScript
{
    public:
        npc_rebel_cannon_35317() : CreatureScript("npc_rebel_cannon_35317") { }

        struct npc_rebel_cannon_35317AI : public VehicleAI
        {
            npc_rebel_cannon_35317AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_PassengerGUID = 0;
            }

            uint64 m_PassengerGUID;

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(50))
                    p_Damage = 0;
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                Player* l_Player = p_Passenger->ToPlayer();
                if (!l_Player)
                    return;

                if (!p_Apply)
                {
                    m_PassengerGUID = 0;

                    for (auto l_Itr : l_Player->getThreatManager().getThreatList())
                    {
                        if (Unit* l_Unit = l_Itr->getTarget())
                            if (Creature* l_Creature = l_Unit->ToCreature())
                                if (l_Creature->IsAIEnabled)
                                    l_Creature->AI()->EnterEvadeMode();
                    }

                    for (auto l_Itr : me->getThreatManager().getThreatList())
                    {
                        if (Unit* l_Unit = l_Itr->getTarget())
                            if (Creature* l_Creature = l_Unit->ToCreature())
                                if (l_Creature->IsAIEnabled)
                                    l_Creature->AI()->EnterEvadeMode();
                    }

                    l_Player->getThreatManager().resetAllAggro();
                    me->getThreatManager().resetAllAggro();

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                    l_Player->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);

                    Creature* l_Me = me;
                    me->AddDelayedEvent([l_Me]() -> void
                    {
                        l_Me->SetFacingTo(l_Me->GetHomePosition().GetOrientation());
                        l_Me->SetOrientation(l_Me->GetHomePosition().GetOrientation());
                        l_Me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                    }, 1000);

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        l_Player->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    }, 500);
                }
                else
                    m_PassengerGUID = l_Player->GetGUID();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PassengerGUID);
                if (!l_Player || !l_Player->IsInWorld())
                    return;

                if (l_Player->GetQuestStatus(eQuests::ByBloodandAsh) == QUEST_STATUS_COMPLETE)
                    l_Player->ExitVehicle();
            }

            void OnBuildDynFlags(uint32& p_DynFlags, Player* p_Player) override
            {
                if (!p_Player->HasQuest(eQuests::ByBloodandAsh) || p_Player->GetQuestStatus(eQuests::ByBloodandAsh) == QUEST_STATUS_COMPLETE)
                {
                    p_DynFlags = UnitDynFlags::UNIT_DYNFLAG_REFER_A_FRIEND;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rebel_cannon_35317AI(p_Creature);
        }
};

/// @Creature : Tobias Mistmantle - 35618
/// @Creature : Crowley's Horse - 44429
class npc_gilneas_passive_ai : public CreatureScript
{
    public:
        npc_gilneas_passive_ai() : CreatureScript("npc_gilneas_passive_ai") { }

        struct npc_gilneas_passive_aiAI : public ScriptedAI
        {
            npc_gilneas_passive_aiAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                return false;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilneas_passive_aiAI(p_Creature);
        }
};

/// @Creature : Frenzied Stalker - 35627
class npc_frenzied_stalker_35627 : public CreatureScript
{
    public:
        npc_frenzied_stalker_35627() : CreatureScript("npc_frenzied_stalker_35627") { }

        struct npc_frenzied_stalker_35627AI : public ScriptedAI
        {
            npc_frenzied_stalker_35627AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Enraged = false;
            }

            bool m_Enraged;

            void Reset() override
            {
                CreatureAI::Reset();

                if (me->GetDefaultMovementType() != MovementGeneratorType::RANDOM_MOTION_TYPE)
                {
                    Creature* l_Me = me;
                    me->AddDelayedEvent([l_Me]() -> void
                    {
                        if (Creature* l_NorthgateRebel = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcNorthgateRebel2, 5.0f))
                            if (!l_NorthgateRebel->isInCombat())
                                l_Me->CombatStart(l_NorthgateRebel, true);
                    }, 1000);
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && p_Source->GetEntry() == eCreatures::NpcNorthgateRebel2)
                    p_Damage = 0;

                if (me->GetHealthPct() <= 30.0f && !m_Enraged)
                {
                    m_Enraged = true;

                    if (urand(0, 3))
                    {
                        Talk(0);
                        DoCast(eSpells::SpellEnrage);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_frenzied_stalker_35627AI(p_Creature);
        }
};

/// @Creature : Northgate Rebel - 41015
class npc_northgate_rebel_41015 : public CreatureScript
{
    public:
        npc_northgate_rebel_41015() : CreatureScript("npc_northgate_rebel_41015") { }

        struct npc_northgate_rebel_41015AI : public ScriptedAI
        {
            npc_northgate_rebel_41015AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_FrenziedStalker = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcFrenziedStalker, 5.0f))
                        if (!l_FrenziedStalker->isInCombat())
                            l_Me->CombatStart(l_FrenziedStalker, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_northgate_rebel_41015AI(p_Creature);
        }
};

/// @Creature : Lord Darius Crowley - 35566
class npc_lord_darius_crowley_35566 : public CreatureScript
{
    public:
        npc_lord_darius_crowley_35566() : CreatureScript("npc_lord_darius_crowley_35566") { }

        struct npc_lord_darius_crowley_35566AI : public ScriptedAI
        {
            npc_lord_darius_crowley_35566AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::LastStand:
                    {
                        p_Player->CastSpell(p_Player, eSpells::SpellLastStandComplete, true);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_darius_crowley_35566AI(p_Creature);
        }
};

/// @Creature : Krennan Aranas - 36331
class npc_krennan_aranas_36331 : public CreatureScript
{
    public:
        npc_krennan_aranas_36331() : CreatureScript("npc_krennan_aranas_36331") { }

        enum eMoves
        {
            MoveToTalk = 1
        };

        struct npc_krennan_aranas_36331AI : public ScriptedAI
        {
            npc_krennan_aranas_36331AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionKrennanAranas:
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(eMoves::MoveToTalk, Positions::g_KrennanAranas3);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());

                        }, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_krennan_aranas_36331AI(p_Creature);
        }
};

/// @Creature : Lord Godfrey - 36330
class npc_lord_godfrey_36330 : public CreatureScript
{
    public:
        npc_lord_godfrey_36330() : CreatureScript("npc_lord_godfrey_36330") { }

        enum eMoves
        {
            MoveToTalk = 1
        };

        struct npc_lord_godfrey_36330AI : public ScriptedAI
        {
            npc_lord_godfrey_36330AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionLordGodfrey:
                    {
                        me->SetDisplayId(15435);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetDisplayId(l_Me->GetNativeDisplayId());
                            l_Me->SetWalk(true);
                            l_Me->GetMotionMaster()->MovePoint(eMoves::MoveToTalk, Positions::g_LordGodfrey3);
                        }, 10000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());

                        }, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_godfrey_36330AI(p_Creature);
        }
};

/// @Creature : King Genn Greymane - 36332
class npc_king_genn_greymane_36332 : public CreatureScript
{
    public:
        npc_king_genn_greymane_36332() : CreatureScript("npc_king_genn_greymane_36332") { }

        enum eMoves
        {
            MoveToTalk = 1
        };

        struct npc_king_genn_greymane_36332AI : public ScriptedAI
        {
            npc_king_genn_greymane_36332AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();

                l_Player->SetTimingForPhase(1422201, 43000);
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::LastChanceatHumanity:
                    {
                        p_Player->RemoveAurasDueToSpell(eSpells::SpellCurseoftheWorgen);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionKingGennGreymane:
                    {
                        me->SetDisplayId(15435);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetDisplayId(l_Me->GetNativeDisplayId());
                            l_Me->SetWalk(true);
                            l_Me->GetMotionMaster()->MovePoint(eMoves::MoveToTalk, Positions::g_GennGreymane3);
                        }, 18000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            l_Me->SetOrientation(1.32645f);
                            l_Me->SetFacingTo(1.32645f);

                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());

                        }, 1000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(1, l_Player->GetGUID());

                        }, 11000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetOrientation(5.497787f);
                            l_Me->SetFacingTo(5.497787f);
                        }, 12000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_king_genn_greymane_36332AI(p_Creature);
        }
};

/// @Creature : Gilnean Crow - 50260
class npc_gilnean_crow_50260 : public CreatureScript
{
    public:
        npc_gilnean_crow_50260() : CreatureScript("npc_gilnean_crow_50260") { }

        struct npc_gilnean_crow_50260AI : public ScriptedAI
        {
            npc_gilnean_crow_50260AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_StartMove = false;
                m_TimerMove = 0;
            }

            bool m_StartMove;
            uint32 m_TimerMove;

            void Reset() override
            {
                if (CreatureData const* l_Data = me->GetCreatureData())
                    me->NearTeleportTo(l_Data->posX, l_Data->posY, l_Data->posZ, l_Data->orientation);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id != eSpells::SpellPingGilneanCrow)
                    return;

                if (!m_StartMove)
                {
                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_BYTES_1, 50331649);
                    me->SetDisableGravity(true, true);
                    m_StartMove = true;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_StartMove)
                    return;

                if (m_TimerMove <= p_Diff)
                {
                    me->GetMotionMaster()->MoveSmoothFlyPath(0, Position((me->GetPositionX() + frand(-15.0f, 15.0f)), (me->GetPositionY() + frand(-15.0f, 15.0f)), (me->GetPositionZ() + frand(5.0f, 15.0f))));
                    m_TimerMove = urand(500, 1000);
                }
                else m_TimerMove -= p_Diff;

                if ((me->GetPositionZ() - me->GetCreatureData()->posZ) >= 20.0f)
                {
                    m_StartMove = false;
                    me->DisappearAndDie();
                    me->RemoveCorpse(true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilnean_crow_50260AI(p_Creature);
        }
};

/// @Creature : Krennan Aranas - 36132
class npc_krennan_aranas_36132 : public CreatureScript
{
    public:
        npc_krennan_aranas_36132() : CreatureScript("npc_krennan_aranas_36132") { }

        struct npc_krennan_aranas_36132AI : public ScriptedAI
        {
            npc_krennan_aranas_36132AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetInPhase(1437503, true, true);
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::AmongHumansAgain:
                    {
                        if (Creature* l_GwenArmstead = GetClosestCreatureWithEntry(me, eCreatures::NpcGwenArmstead, 10.0f))
                            if (l_GwenArmstead->IsAIEnabled)
                                l_GwenArmstead->AI()->PersonalTalk(0, p_Player->GetGUID());

                        break;
                    }
                    default:
                        break;
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::InNeedofIngredients:
                    {
                        PersonalTalk(0, p_Player->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_krennan_aranas_36132AI(p_Creature);
        }
};

/// @Creature : Slain Watchman - 36205
class npc_slain_watchman_36205 : public CreatureScript
{
    public:
        npc_slain_watchman_36205() : CreatureScript("npc_slain_watchman_36205") { }

        struct npc_slain_watchman_36205AI : public ScriptedAI
        {
            npc_slain_watchman_36205AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::Invasion:
                    {
                        if (Creature* l_ForsakenAssassin = p_Player->SummonCreature(eCreatures::NpcForsakenAssassin, Positions::g_ForsakenAssassin, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                            if (l_ForsakenAssassin->IsAIEnabled)
                            {
                                l_ForsakenAssassin->AI()->AttackStart(p_Player, true);
                                l_ForsakenAssassin->AI()->PersonalTalk(0, p_Player->GetGUID());
                                l_ForsakenAssassin->SetInCombatWith(p_Player);
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
            return new npc_slain_watchman_36205AI(p_Creature);
        }
};

/// @Creature : Forsaken Invader - 34511
class npc_forsaken_invader_34511 : public CreatureScript
{
    public:
        npc_forsaken_invader_34511() : CreatureScript("npc_forsaken_invader_34511") { }

        struct npc_forsaken_invader_34511AI : public ScriptedAI
        {
            npc_forsaken_invader_34511AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_DuskhavenWatchman = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcDuskhavenWatchman, 10.0f))
                        l_Me->CombatStart(l_DuskhavenWatchman, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && p_Source->GetEntry() == eCreatures::NpcDuskhavenWatchman)
                    p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_invader_34511AI(p_Creature);
        }
};

/// @Creature : Forsaken Footsoldier - 36236
class npc_forsaken_footsoldier_36236 : public CreatureScript
{
    public:
        npc_forsaken_footsoldier_36236() : CreatureScript("npc_forsaken_footsoldier_36236") { }

        struct npc_forsaken_footsoldier_36236AI : public ScriptedAI
        {
            npc_forsaken_footsoldier_36236AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_DuskhavenWatchman = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcDuskhavenWatchman, 10.0f))
                        l_Me->CombatStart(l_DuskhavenWatchman, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && p_Source->GetEntry() == eCreatures::NpcDuskhavenWatchman)
                    p_Damage = 0;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcAttackMastiff)
                    return false;

                return true;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcAttackMastiff)
                    return false;

                return true;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_footsoldier_36236AI(p_Creature);
        }
};

/// @Creature : Duskhaven Watchman - 36211
class npc_duskhaven_watchman_36211 : public CreatureScript
{
    public:
        npc_duskhaven_watchman_36211() : CreatureScript("npc_duskhaven_watchman_36211") { }

        struct npc_duskhaven_watchman_36211AI : public ScriptedAI
        {
            npc_duskhaven_watchman_36211AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_ForsakenInvader = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcForsakenInvader, 10.0f))
                    {
                        if (!l_ForsakenInvader->isInCombat())
                            l_Me->CombatStart(l_ForsakenInvader, true);
                    }
                    else
                    {
                        if (Creature* l_ForsakenFootsoldier = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcForsakenFootsoldier, 10.0f))
                            if (!l_ForsakenFootsoldier->isInCombat())
                                l_Me->CombatStart(l_ForsakenFootsoldier, true);
                    }
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_duskhaven_watchman_36211AI(p_Creature);
        }
};

/// @Creature : Horrid Abomination - 36231
class npc_horrid_abomination_36231 : public CreatureScript
{
    public:
        npc_horrid_abomination_36231() : CreatureScript("npc_horrid_abomination_36231") { }

        struct npc_horrid_abomination_36231AI : public ScriptedAI
        {
            npc_horrid_abomination_36231AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                me->RemoveAurasDueToSpell(eSpells::SpellKegPlaced);
                me->RemoveAurasDueToSpell(eSpells::SpellAbominationExplosion);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id != eSpells::SpellKegPlaced)
                    return;

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->StopAttack();
                me->CombatStop();

                Talk(0);

                Creature* l_Me = me;
                uint64 l_PlayerGUID = l_Player->GetGUID();

                me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                {
                    l_Me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    l_Me->RemoveAurasDueToSpell(eSpells::SpellKegPlaced);

                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                        l_Player->KilledMonsterCredit(eCreatures::NpcHorridAbominationKC);

                    for (int l_I = 0; l_I < 5; l_I++)
                        l_Me->CastSpell(l_Me, eSpells::SpellPointPoison, true);

                    l_Me->CastSpell(l_Me, eSpells::SpellBloodyDeath, true);
                    l_Me->CastSpell(l_Me, eSpells::SpellAbominationExplosion, true);
                    l_Me->DespawnOrUnsummon(2000);
                }, 5000);
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_horrid_abomination_36231AI(p_Creature);
        }
};

/// @Creature : Cynthia - 36287
class npc_cynthia_36287 : public CreatureScript
{
    public:
        npc_cynthia_36287() : CreatureScript("npc_cynthia_36287") { }

        enum eMoves
        {
            MoveToDespawn = 1
        };

        struct npc_cynthia_36287AI : public ScriptedAI
        {
            npc_cynthia_36287AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                me->DestroyForPlayer(l_Player);
                l_Player->UpdateObjectVisibility(true);

                if (Creature* l_Cynthia = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, l_Player->GetGUID()))
                    if (l_Cynthia->IsAIEnabled)
                        l_Cynthia->AI()->DoAction(eActions::ActionCynthia);
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCynthia:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                        {
                            std::string l_Text;
                            if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(eBroadcasts::PlayerCynthia))
                                l_Text = GetBroadcastTextValue(l_BroadcastText, l_Player->GetSession()->GetSessionDb2Locale(), l_Player->getGender());

                            if (!l_Text.empty())
                            {
                                WorldPacket l_Packet;
                                l_Player->BuildPlayerChat(&l_Packet, l_Player->GetGUID(), ChatMsg::CHAT_MSG_SAY, l_Text, 0);
                                l_Player->GetSession()->SendPacket(&l_Packet);
                            }
                        }

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            {
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());

                                l_Player->KilledMonsterCredit(l_Me->GetEntry());
                            }
                        }, 4000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_CynthiaDespawn.data(), Waypoints::g_CynthiaDespawn.size(), false);
                        }, 6000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cynthia_36287AI(p_Creature);
        }
};

/// @Creature : James - 36289
class npc_james_36289 : public CreatureScript
{
    public:
        npc_james_36289() : CreatureScript("npc_james_36289") { }

        enum eMoves
        {
            MoveToDespawn = 1
        };

        struct npc_james_36289AI : public ScriptedAI
        {
            npc_james_36289AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                me->DestroyForPlayer(l_Player);
                l_Player->UpdateObjectVisibility(true);

                if (Creature* l_James = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, l_Player->GetGUID()))
                    if (l_James->IsAIEnabled)
                        l_James->AI()->DoAction(eActions::ActionJames);
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionJames:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                        {
                            std::string l_Text;
                            if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(eBroadcasts::PlayerJames))
                                l_Text = GetBroadcastTextValue(l_BroadcastText, l_Player->GetSession()->GetSessionDb2Locale(), l_Player->getGender());

                            if (!l_Text.empty())
                            {
                                WorldPacket l_Packet;
                                l_Player->BuildPlayerChat(&l_Packet, l_Player->GetGUID(), ChatMsg::CHAT_MSG_SAY, l_Text, 0);
                                l_Player->GetSession()->SendPacket(&l_Packet);
                            }
                        }

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            {
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());

                                l_Player->KilledMonsterCredit(l_Me->GetEntry());
                            }
                        }, 4000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_JamesDespawn.data(), Waypoints::g_JamesDespawn.size(), false);
                        }, 6000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_james_36289AI(p_Creature);
        }
};

/// @Creature : Ashley - 36288
class npc_ashley_36288 : public CreatureScript
{
    public:
        npc_ashley_36288() : CreatureScript("npc_ashley_36288") { }

        enum eMoves
        {
            MoveToDespawn = 1
        };

        struct npc_ashley_36288AI : public ScriptedAI
        {
            npc_ashley_36288AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                me->DestroyForPlayer(l_Player);
                l_Player->UpdateObjectVisibility(true);

                if (Creature* l_Ashley = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, l_Player->GetGUID()))
                    if (l_Ashley->IsAIEnabled)
                        l_Ashley->AI()->DoAction(eActions::ActionAshley);
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionAshley:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);

                        if (Player * l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                        {
                            std::string l_Text;
                            if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(eBroadcasts::PlayerAshley))
                                l_Text = GetBroadcastTextValue(l_BroadcastText, l_Player->GetSession()->GetSessionDb2Locale(), l_Player->getGender());

                            if (!l_Text.empty())
                            {
                                WorldPacket l_Packet;
                                l_Player->BuildPlayerChat(&l_Packet, l_Player->GetGUID(), ChatMsg::CHAT_MSG_SAY, l_Text, 0);
                                l_Player->GetSession()->SendPacket(&l_Packet);
                            }
                        }

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            {
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());

                                l_Player->KilledMonsterCredit(l_Me->GetEntry());
                            }
                        }, 4000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_AshleyDespawn.data(), Waypoints::g_AshleyDespawn.size(), false);
                        }, 6000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashley_36288AI(p_Creature);
        }
};

/// @Creature : Forsaken Catapult - 36283
class npc_forsaken_catapult_36283 : public CreatureScript
{
    public:
        npc_forsaken_catapult_36283() : CreatureScript("npc_forsaken_catapult_36283") { }

        enum eEvents
        {
            EventFieryBoulder = 1,
            EventDespawn
        };

        struct npc_forsaken_catapult_36283AI : public ScriptedAI
        {
            npc_forsaken_catapult_36283AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PassengerGUID = 0;
            }

            uint64 m_PassengerGUID;

            void Reset() override
            {
                CreatureAI::Reset();
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (Creature* l_Creature = p_Passenger->ToCreature())
                {
                    if (p_Apply)
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        l_Creature->ClearUnitState(UNIT_STATE_ONVEHICLE);
                        events.ScheduleEvent(eEvents::EventFieryBoulder, urand(5000, 10000));
                    }
                    else
                    {
                        events.CancelEvent(eEvents::EventFieryBoulder);
                        events.ScheduleEvent(eEvents::EventDespawn, 60000);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                if (Player* l_Player = p_Passenger->ToPlayer())
                {
                    if (p_Apply)
                        m_PassengerGUID = l_Player->GetGUID();
                    else
                        m_PassengerGUID = 0;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() != eCreatures::NpcForsakenMachinist)
                    return;

                p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFieryBoulder:
                    {
                        DoCast(eSpells::SpellFieryBoulder);
                        events.ScheduleEvent(eEvents::EventFieryBoulder, urand(5000, 10000));
                        break;
                    }
                    case eEvents::EventDespawn:
                    {
                        if (!m_PassengerGUID)
                        {
                            me->Respawn(true, true, 10000);
                            return;
                        }
                        else
                            events.ScheduleEvent(eEvents::EventDespawn, 60000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject *>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellFieryBoulder:
                    {
                        p_Targets.remove_if([](WorldObject* p_Target)->bool
                        {
                            if (!p_Target->ToCreature())
                                return true;

                            if (p_Target->GetEntry() != eCreatures::NpcGenericTriggerCanon)
                                return true;

                            return false;
                        });

                        if (p_Targets.empty())
                            return;

                        JadeCore::Containers::RandomResizeList(p_Targets, 1);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_catapult_36283AI(p_Creature);
        }
};

/// @Creature : Attack Mastiff - 36405
class npc_attack_mastiff_36405 : public CreatureScript
{
    public:
        npc_attack_mastiff_36405() : CreatureScript("npc_attack_mastiff_36405") { }

        struct npc_attack_mastiff_36405AI : public ScriptedAI
        {
            npc_attack_mastiff_36405AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_LeapDone = false;
                m_PlayerGUID = 0;
                m_RangerGUID = 0;
            }

            uint64 m_PlayerGUID;
            uint64 m_RangerGUID;
            bool m_LeapDone;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID*/) override
            {
                m_RangerGUID = p_Guid;
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionAttackMastiff:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_RangerGUID))
                        {
                            AttackStart(l_Creature, true);
                            me->SetInCombatWith(l_Creature);
                            l_Creature->SetInCombatWith(me);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_RangerGUID);
                    if (!l_Creature || l_Creature->isDead())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    if (!UpdateVictim())
                        return;

                    events.Update(p_Diff);

                    if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                        return;

                    if (me->GetDistance2d(l_Creature) < 20.0f && !m_LeapDone)
                    {
                        m_LeapDone = true;
                        DoCast(l_Creature, eSpells::SpellLeap, true);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->CastSpell(l_Me, eSpells::SpellDemoralizingRoar, true);
                        }, urand (2000, 5000));
                    }

                    DoMeleeAttackIfReady();
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->GetEntry() == eCreatures::NpcForsakenFootsoldier)
                    p_Damage = 0;
            }

            void EnterEvadeMode() override
            {
                // What's the problem?
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcDarkRangerThyala)
                    return true;

                return false;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target->ToCreature() && p_Target->ToCreature()->GetEntry() == eCreatures::NpcDarkRangerThyala)
                    return true;

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_attack_mastiff_36405AI(p_Creature);
        }
};

/// @Creature : Dark Ranger Thyala - 36312
class npc_dark_ranger_thyala_36312 : public CreatureScript
{
    public:
        npc_dark_ranger_thyala_36312() : CreatureScript("npc_dark_ranger_thyala_36312") { }

        struct npc_dark_ranger_thyala_36312AI : public ScriptedAI
        {
            npc_dark_ranger_thyala_36312AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Killed = false;
            }

            bool m_Killed;

            void Reset() override
            {
                CreatureAI::Reset();

                m_Killed = false;
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth(p_Source))
                {
                    p_Damage = 0;

                    if (!m_Killed)
                    {
                        uint64 l_PlayerGUID;

                        std::list<uint64> l_ThreatGuidList;
                        for (auto itr : me->getThreatManager().getThreatList())
                        {
                            Unit* l_Unit = itr->getTarget();
                            if (!l_Unit)
                                continue;

                            Player* l_Player = l_Unit->ToPlayer();
                            if (!l_Player)
                                continue;

                            l_Player->KilledMonsterCredit(me->GetEntry());
                            l_PlayerGUID = l_Player->GetGUID();
                        }

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGUID))
                            l_Player->Kill(me);

                        m_Killed = true;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dark_ranger_thyala_36312AI(p_Creature);
        }
};

/// @Creature : Lord Godfrey - 36290
class npc_lord_godfrey_36290 : public CreatureScript
{
    public:
        npc_lord_godfrey_36290() : CreatureScript("npc_lord_godfrey_36290") { }

        struct npc_lord_godfrey_36290AI : public ScriptedAI
        {
            npc_lord_godfrey_36290AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::LeaderofthePack:
                    {
                        DoCast(p_Player, eSpells::SpellForcecastCataclysm, true);

                        PersonalTalk(0, p_Player->GetGUID());

                        if (Creature* l_MelindaHammond = GetClosestCreatureWithEntry(me, eCreatures::NpcMelindaHammond, 10.0f))
                            if (l_MelindaHammond->IsAIEnabled)
                                l_MelindaHammond->AI()->PersonalTalk(0, p_Player->GetGUID());

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_godfrey_36290AI(p_Creature);
        }
};

/// @Creature : Drowning Watchman - 36440
class npc_drowning_watchman_36440 : public CreatureScript
{
    public:
        npc_drowning_watchman_36440() : CreatureScript("npc_drowning_watchman_36440") { }

        struct npc_drowning_watchman_36440AI : public ScriptedAI
        {
            npc_drowning_watchman_36440AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_KillCredit = false;
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            bool m_KillCredit;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                uint64 l_PlayerGUID = l_Player->GetGUID();

                if (Creature* l_DrowningWatchman = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_PlayerGUID))
                {
                    l_DrowningWatchman->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                    l_DrowningWatchman->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                    l_DrowningWatchman->SetReactState(ReactStates::REACT_PASSIVE);
                    l_DrowningWatchman->SetDisplayId(me->GetDisplayId());

                    l_DrowningWatchman->AddDelayedEvent([l_DrowningWatchman, l_PlayerGUID]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_DrowningWatchman, l_PlayerGUID))
                            l_DrowningWatchman->EnterVehicle(l_Player);
                    }, 1000);
                }

                me->AddToHideList(l_Player->GetGUID(), 30000);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    Creature* l_PrinceLiamGreymane = GetClosestCreatureWithEntry(l_Player, eCreatures::NpcPrinceLiamGreymane3, 1.0f);
                    if (!l_PrinceLiamGreymane || m_KillCredit)
                        return;

                    m_KillCredit = true;

                    l_Player->KilledMonsterCredit(eCreatures::NpcDrowningWatchmanKC);
                    l_Player->RemoveAurasDueToSpell(eSpells::SpellRescueDrowningWatchman);

                    me->ExitVehicle();
                    me->RemoveAllAuras();
                    me->DespawnOrUnsummon(5000);

                    Creature* l_Me = me;
                    uint64 l_PlayerGUID = m_PlayerGUID;

                    me->AddDelayedEvent([l_Me]() -> void
                    {
                        l_Me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
                    }, 1000);

                    me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());
                    }, 2000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_drowning_watchman_36440AI(p_Creature);
        }
};

/// @Creature : Mountain Horse - 36540
class npc_mountain_horse_36540 : public CreatureScript
{
    public:
        npc_mountain_horse_36540() : CreatureScript("npc_mountain_horse_36540") { }

        struct npc_mountain_horse_36540AI : public VehicleAI
        {
            npc_mountain_horse_36540AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void Reset() override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                Player* l_Player = p_Passenger->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Apply)
                {
                    m_PlayerGUID = l_Player->GetGUID();
                }
                else
                {
                    m_PlayerGUID = 0;
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->Respawn(true, true, 5000);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id != eSpells::SpellRoundUpHorse)
                    return;

                me->AddToHideList(l_Player->GetGUID(), 120000);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                if (!l_Player)
                    return;

                if (!l_Player->HasQuest(eQuests::TheHungryEttin) || l_Player->GetQuestStatus(eQuests::TheHungryEttin) == QUEST_STATUS_COMPLETE)
                    l_Player->ExitVehicle();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_mountain_horse_36540AI(p_Creature);
        }
};

/// @Creature : Mountain Horse - 36555
class npc_mountain_horse_36555 : public CreatureScript
{
    public:
        npc_mountain_horse_36555() : CreatureScript("npc_mountain_horse_36555") { }

        struct npc_mountain_horse_36555AI : public ScriptedAI
        {
            npc_mountain_horse_36555AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_KillCredit = false;
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            bool m_KillCredit;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->CastSpell(l_Player, eSpells::SpellRopeChannel, true);
                me->GetMotionMaster()->MoveFollow(l_Player, 3.0f, frand(0.0, M_PI * 2.0f));
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    Creature* l_LornaCrowley = GetClosestCreatureWithEntry(l_Player, eCreatures::NpcLornaCrowley2, 5.0f);
                    if (!l_LornaCrowley || m_KillCredit)
                        return;

                    m_KillCredit = true;

                    l_Player->KilledMonsterCredit(eCreatures::NpcMountainHorseKC);

                    me->GetMotionMaster()->MoveIdle();
                    me->DespawnOrUnsummon(2000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_mountain_horse_36555AI(p_Creature);
        }
};

/// @Creature : Forsaken Castaway - 36488
class npc_forsaken_castaway_36488 : public CreatureScript
{
    public:
        npc_forsaken_castaway_36488() : CreatureScript("npc_forsaken_castaway_36488") { }

        struct npc_forsaken_castaway_36488AI : public ScriptedAI
        {
            npc_forsaken_castaway_36488AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(90) && p_Source->ToCreature())
                    p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_castaway_36488AI(p_Creature);
        }
};

/// @Creature : Tim Hayward - 36454
/// @Creature : Walt Hayward - 36455
/// @Creature : Trent Hayward - 36491
/// @Creature : Ron Hayward - 36492
class npc_gilneas_brother_hayward : public CreatureScript
{
    public:
        npc_gilneas_brother_hayward() : CreatureScript("npc_gilneas_brother_hayward") { }

        struct npc_gilneas_brother_haywardAI : public ScriptedAI
        {
            npc_gilneas_brother_haywardAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_ForsakenCastaway = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcForsakenCastaway, 10.0f))
                        if (!l_ForsakenCastaway->isInCombat())
                            l_Me->CombatStart(l_ForsakenCastaway, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilneas_brother_haywardAI(p_Creature);
        }
};

/// @Creature : Chance - 36459
class npc_chance_36459 : public CreatureScript
{
    public:
        npc_chance_36459() : CreatureScript("npc_chance_36459") { }

        struct npc_chance_36459AI : public ScriptedAI
        {
            npc_chance_36459AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                Creature* l_Me = me;
                uint64 l_PlayerGUID = l_Player->GetGUID();

                l_Player->AddObjectInLockoutList(me->GetDBTableGUIDLow(), eQuests::GrandmasCat);
                l_Player->ForceUpdateForObjectWithEntry(me->GetEntry(), true);

                if (Creature* l_LuciustheCruel = l_Player->SummonCreature(eCreatures::NpcLuciustheCruel, Positions::g_LuciustheCruel, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_PlayerGUID))
                    if (l_LuciustheCruel->IsAIEnabled)
                        l_LuciustheCruel->AI()->DoAction(eActions::ActionLuciustheCruel);

                l_Me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                    {
                        l_Me->DestroyForPlayer(l_Player);
                        l_Player->RemoveObjectsFromLockoutListByQuest(eQuests::GrandmasCat);
                    }
                }, 4000);
            }

            void OnBuildDynFlags(uint32& p_DynFlags, Player* p_Player) override
            {
                if (p_Player->IsObjectInLockoutList(me->GetDBTableGUIDLow()))
                {
                    p_DynFlags = UnitDynFlags::UNIT_DYNFLAG_REFER_A_FRIEND;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_chance_36459AI(p_Creature);
        }
};

/// @Creature : Lucius the Cruel - 36461
class npc_lucius_the_cruel_36461 : public CreatureScript
{
    public:
        npc_lucius_the_cruel_36461() : CreatureScript("npc_lucius_the_cruel_36461") { }

        enum eMoves
        {
            MoveToChance = 1
        };

        struct npc_lucius_the_cruel_36461AI : public ScriptedAI
        {
            npc_lucius_the_cruel_36461AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(5) && p_Source->GetEntry() == eCreatures::NpcGrandmaWahl)
                    p_Damage = 0;
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionLuciustheCruel:
                    {
                        me->GetMotionMaster()->MovePoint(eMoves::MoveToChance, Positions::g_LuciustheCruel2);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToChance:
                    {
                        PersonalTalk(0, m_PlayerGUID);

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_KNEEL);
                        }, 500);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            {
                                l_Me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                l_Me->SetInCombatWith(l_Player);
                                l_Me->CombatStart(l_Player, true);

                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->AttackStart(l_Player);

                                if (Creature* l_GrandmaWahl = l_Player->SummonCreature(eCreatures::NpcGrandmaWahl, Positions::g_GrandmaWah, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_PlayerGUID))
                                    if (l_GrandmaWahl->IsAIEnabled)
                                    {
                                        l_GrandmaWahl->AI()->SetGUID(l_Me->GetGUID());
                                        l_GrandmaWahl->AI()->DoAction(eActions::ActionGrandmaWah);
                                    }
                            }
                        }, 4000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                DoMeleeAttackIfReady();
            }

            void EnterEvadeMode() override
            {
                // What's the problem?
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lucius_the_cruel_36461AI(p_Creature);
        }
};

/// @Creature : Grandma Wahl - 36458
class npc_grandma_wahl_36458 : public CreatureScript
{
    public:
        npc_grandma_wahl_36458() : CreatureScript("npc_grandma_wahl_36458") { }

        enum eMoves
        {
            MoveToLucius = 1,
            MoveToDespawn
        };

        struct npc_grandma_wahl_36458AI : public ScriptedAI
        {
            npc_grandma_wahl_36458AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_LuciusKilled = false;
                m_PlayerGUID = 0;
                m_LuciusGUID = 0;
            }

            uint64 m_PlayerGUID;
            uint64 m_LuciusGUID;
            bool m_LuciusKilled;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID*/) override
            {
                m_LuciusGUID = p_Guid;
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionGrandmaWah:
                    {
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK | NPCFlags::UNIT_NPC_FLAG_GOSSIP);

                        if (Creature* l_LuciustheCruel = ObjectAccessor::GetCreature(*me, m_LuciusGUID))
                        {
                            float l_X, l_Y, l_Z;
                            l_LuciustheCruel->GetContactPoint(me, l_X, l_Y, l_Z, 0.0f);

                            me->GetMotionMaster()->MovePoint(eMoves::MoveToLucius, l_X, l_Y, l_Z);
                        }
                        break;
                    }
                    case eActions::ActionGrandmaWah2:
                    {
                        m_LuciusKilled = true;

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->CombatStop();
                        me->StopAttack();

                        me->GetMotionMaster()->MovePoint(eMoves::MoveToDespawn, Positions::g_GrandmaWah);
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToLucius:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        Creature* l_Me = me;
                        uint64 l_LuciusGUID = m_LuciusGUID;

                        me->AddDelayedEvent([l_Me, l_LuciusGUID]() -> void
                        {
                            if (Creature* l_LuciustheCruel = ObjectAccessor::GetCreature(*l_Me, l_LuciusGUID))
                            {
                                l_Me->SetInCombatWith(l_LuciustheCruel);
                                l_Me->CombatStart(l_LuciustheCruel, true);

                                if (l_Me->IsAIEnabled)
                                {
                                    l_Me->AI()->AttackStart(l_LuciustheCruel);
                                    l_Me->AI()->Talk(0);
                                }

                                l_Me->SetDisplayId(36852);
                            }
                        }, 500);

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon(0);
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    if (Creature* l_LuciustheCruel = ObjectAccessor::GetCreature(*me, m_LuciusGUID))
                        if (l_LuciustheCruel->isDead() && !m_LuciusKilled)
                            DoAction(eActions::ActionGrandmaWah2);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                DoMeleeAttackIfReady();
            }

            void EnterEvadeMode() override
            {
                // What's the problem?
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_grandma_wahl_36458AI(p_Creature);
        }
};

/// @Creature : Gwen Armstead - 36452
class npc_gwen_armstead_36452 : public CreatureScript
{
    public:
        npc_gwen_armstead_36452() : CreatureScript("npc_gwen_armstead_36452") { }

        struct npc_gwen_armstead_36452AI : public ScriptedAI
        {
            npc_gwen_armstead_36452AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::ToGreymaneManor:
                    {
                        if (Creature* l_SwiftMountainHorse = p_Player->SummonCreature(eCreatures::NpcSwiftMountainHorse, Positions::g_MountainHorse, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            if (l_SwiftMountainHorse->IsAIEnabled)
                                l_SwiftMountainHorse->AI()->DoAction(eActions::ActionSwiftMountainHorse);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gwen_armstead_36452AI(p_Creature);
        }
};

/// @Creature : Swift Mountain Horse - 36741
class npc_swift_mountain_horse_36741 : public CreatureScript
{
    public:
        npc_swift_mountain_horse_36741() : CreatureScript("npc_swift_mountain_horse_36741") { }

        enum eMoves
        {
            MoveToDespawn = 1
        };

        struct npc_swift_mountain_horse_36741AI : public VehicleAI
        {
            npc_swift_mountain_horse_36741AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionSwiftMountainHorse:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->EnterVehicle(me, 0);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_SwiftMountainHorse.data(), Waypoints::g_SwiftMountainHorse.size(), false);
                        }, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToDespawn:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->ExitVehicle();
                        }, 1000);

                        me->DespawnOrUnsummon(3000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                Player* l_Player = p_Passenger->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Apply)
                {
                    l_Player->SetInPhase(1407804, true, true);
                    l_Player->SetInPhase(1407802, true, false);
                }
                else
                {
                    l_Player->SetInPhase(1407804, true, false);
                    l_Player->SetInPhase(1407802, true, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_swift_mountain_horse_36741AI(p_Creature);
        }
};

/// @Creature : Stagecoach Harness - 38755
class npc_stagecoach_harness_38755 : public CreatureScript
{
    public:
        npc_stagecoach_harness_38755() : CreatureScript("npc_stagecoach_harness_38755") { }

        struct npc_stagecoach_harness_38755AI : public ScriptedAI
        {
            npc_stagecoach_harness_38755AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                Creature* l_Me = me;

                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_StagecoachHorse = l_Me->SummonCreature(eCreatures::NpcStagecoachHorse, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_StagecoachHorse->EnterVehicle(l_Me, 0);

                    if (Creature* l_StagecoachHorse = l_Me->SummonCreature(eCreatures::NpcStagecoachHorse, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_StagecoachHorse->EnterVehicle(l_Me, 1);

                    if (Creature* l_StagecoachCarriage = l_Me->SummonCreature(eCreatures::NpcStagecoachCarriage, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_StagecoachCarriage->EnterVehicle(l_Me, 2);
                }, 100);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stagecoach_harness_38755AI(p_Creature);
        }
};

/// @Creature : Stagecoach Carriage - 44928
class npc_stagecoach_carriage_44928 : public CreatureScript
{
    public:
        npc_stagecoach_carriage_44928() : CreatureScript("npc_stagecoach_carriage_44928") { }

        struct npc_stagecoach_carriage_44928AI : public ScriptedAI
        {
            npc_stagecoach_carriage_44928AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                Creature* l_Me = me;

                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_MarieAllen = l_Me->SummonCreature(eCreatures::NpcMarieAllen, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_MarieAllen->EnterVehicle(l_Me, 1);

                    if (Creature* l_GwenArmstead = l_Me->SummonCreature(eCreatures::NpcGwenArmstead2, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_GwenArmstead->EnterVehicle(l_Me, 2);

                    if (Creature* l_KrennanAranas = l_Me->SummonCreature(eCreatures::NpcKrennanAranas3, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_KrennanAranas->EnterVehicle(l_Me, 3);

                    if (Creature* l_LornaCrowley = l_Me->SummonCreature(eCreatures::NpcLornaCrowley3, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_LornaCrowley->EnterVehicle(l_Me, 6);
                }, 100);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                if (Creature* l_StagecoachHarness = l_Player->SummonCreature(eCreatures::NpcStagecoachHarness, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    if (l_StagecoachHarness->IsAIEnabled)
                        l_StagecoachHarness->AI()->DoAction(eActions::ActionStagecoachHarness);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stagecoach_carriage_44928AI(p_Creature);
        }
};

/// @Creature : Stagecoach Harness - 43336
class npc_stagecoach_harness_43336 : public CreatureScript
{
    public:
        npc_stagecoach_harness_43336() : CreatureScript("npc_stagecoach_harness_43336") { }

        enum eMoves
        {
            MoveToBridge = 1,
            MoveToCrash,
            MoveToDespawn
        };

        struct npc_stagecoach_harness_43336AI : public ScriptedAI
        {
            npc_stagecoach_harness_43336AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void Reset() override
            {
                Creature* l_Me = me;

                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_StagecoachHorse = l_Me->SummonCreature(eCreatures::NpcStagecoachHorse, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_StagecoachHorse->EnterVehicle(l_Me, 0);

                    if (Creature* l_StagecoachHorse = l_Me->SummonCreature(eCreatures::NpcStagecoachHorse, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_StagecoachHorse->EnterVehicle(l_Me, 1);

                    if (Creature* l_StagecoachCarriage = l_Me->SummonCreature(eCreatures::NpcStagecoachCarriage2, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_StagecoachCarriage->EnterVehicle(l_Me, 2);
                }, 100);
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStagecoachHarness:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Vehicle* l_Vehicle = l_Me->GetVehicleKit())
                            {
                                if (Unit* l_StagecoachCarriageUnit = l_Vehicle->GetPassenger(2))
                                {
                                    if (Creature* l_StagecoachCarriage = l_StagecoachCarriageUnit->ToCreature())
                                    {
                                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_StagecoachCarriage, l_PlayerGUID))
                                        {
                                            l_Player->EnterVehicle(l_StagecoachCarriage, 0);
                                            l_Player->KilledMonsterCredit(eCreatures::NpcStagecoachCarriage);
                                        }
                                    }
                                }
                            }
                        }, 500);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetSpeed(UnitMoveType::MOVE_RUN, 2.5f, true);
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToBridge, Waypoints::g_StagecoachHarness.data(), Waypoints::g_StagecoachHarness.size(), false);
                        }, 1500);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToBridge:
                    {
                        me->SetSpeed(UnitMoveType::MOVE_RUN, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToCrash, Waypoints::g_StagecoachHarness2.data(), Waypoints::g_StagecoachHarness2.size(), false);
                        break;
                    }
                    case eMoves::MoveToCrash:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->ExitVehicle();

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_StagecoachHarnessDespawn.data(), Waypoints::g_StagecoachHarnessDespawn.size(), false);
                        }, 1000);

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }

            void JustDespawned() override
            {
                summons.DespawnAll();
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stagecoach_harness_43336AI(p_Creature);
        }
};

/// @Creature : Stagecoach Carriage - 43337
class npc_stagecoach_carriage_43337 : public CreatureScript
{
    public:
        npc_stagecoach_carriage_43337() : CreatureScript("npc_stagecoach_carriage_43337") { }

        struct npc_stagecoach_carriage_43337AI : public ScriptedAI
        {
            npc_stagecoach_carriage_43337AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_MarieAllen = l_Me->SummonCreature(eCreatures::NpcMarieAllen, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_MarieAllen->EnterVehicle(l_Me, 1);

                    if (Creature* l_GwenArmstead = l_Me->SummonCreature(eCreatures::NpcGwenArmstead2, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_GwenArmstead->EnterVehicle(l_Me, 2);

                    if (Creature* l_KrennanAranas = l_Me->SummonCreature(eCreatures::NpcKrennanAranas3, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_KrennanAranas->EnterVehicle(l_Me, 3);

                    if (Creature* l_DuskhavenWatchman = l_Me->SummonCreature(eCreatures::NpcDuskhavenWatchman2, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_DuskhavenWatchman->EnterVehicle(l_Me, 4);

                    if (Creature* l_DuskhavenWatchman = l_Me->SummonCreature(eCreatures::NpcDuskhavenWatchman3, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_DuskhavenWatchman->EnterVehicle(l_Me, 5);

                    if (Creature* l_LornaCrowley = l_Me->SummonCreature(eCreatures::NpcLornaCrowley3, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_LornaCrowley->EnterVehicle(l_Me, 6);
                }, 100);
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                Player* l_Player = p_Passenger->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Apply)
                    l_Player->SetInPhase(1446500, true, false);
                else
                    l_Player->SetInPhase(1446500, true, true);
            }

            void JustDespawned() override
            {
                summons.DespawnAll();
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stagecoach_carriage_43337AI(p_Creature);
        }
};

/// @Creature : Crash Survivor - 37067
class npc_crash_survivor_37067 : public CreatureScript
{
    public:
        npc_crash_survivor_37067() : CreatureScript("npc_crash_survivor_37067") { }

        struct npc_crash_survivor_37067AI : public ScriptedAI
        {
            npc_crash_survivor_37067AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    l_Me->CastSpell(l_Me, eSpells::SpellSummonSwampCrocolisk, true);
                }, 100);
            }

            void EnterEvadeMode() override
            {
                // What's the problem?
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() != eCreatures::NpcSwampCrocolisk)
                    return;

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->DespawnOrUnsummon(1000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_crash_survivor_37067AI(p_Creature);
        }
};

/// @Creature : Swamp Crocolisk - 37078
class npc_swamp_crocolisk_37078 : public CreatureScript
{
    public:
        npc_swamp_crocolisk_37078() : CreatureScript("npc_swamp_crocolisk_37078") { }

        struct npc_swamp_crocolisk_37078AI : public ScriptedAI
        {
            npc_swamp_crocolisk_37078AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Source->IsPlayer())
                {
                    me->getThreatManager().resetAllAggro();
                    p_Source->AddThreat(me, 1.0f);
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (p_Source->isPet())
                {
                    me->getThreatManager().resetAllAggro();
                    me->AddThreat(p_Source, 1.0f);
                    AttackStart(p_Source);
                }
                else if (me->HealthBelowPct(80) && p_Source->GetEntry() == eCreatures::NpcCrashSurvivor)
                    p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_swamp_crocolisk_37078AI(p_Creature);
        }
};

/// @Creature : Koroth the Hillbreaker - 36294
class npc_koroth_the_hillbreaker_36294 : public CreatureScript
{
    public:
        npc_koroth_the_hillbreaker_36294() : CreatureScript("npc_koroth_the_hillbreaker_36294") { }

        enum eMoves
        {
            MoveToBanner = 1,
            MoveToHome
        };

        struct npc_koroth_the_hillbreaker_36294AI : public ScriptedAI
        {
            npc_koroth_the_hillbreaker_36294AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayEvent = false;
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            bool m_PlayEvent;

            void Reset() override
            {
                CreatureAI::Reset();
                m_PlayEvent = false;
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID*/) override
            {
                m_PlayerGUID = p_Guid;
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionKoroththeHillbreaker:
                    {
                        if (m_PlayEvent || me->isInCombat())
                            return;

                        m_PlayEvent = true;

                        PersonalTalk(0, m_PlayerGUID);

                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(eMoves::MoveToBanner, Positions::g_Koroth);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToBanner:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);

                        }, 1000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetWalk(true);
                            l_Me->GetMotionMaster()->MovePoint(eMoves::MoveToHome, l_Me->GetHomePosition());
                        }, 4000);

                        break;
                    }
                    case eMoves::MoveToHome:
                    {
                        me->SetFacingTo(3.141590f);
                        me->SetOrientation(3.141590f);

                        m_PlayEvent = false;
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_koroth_the_hillbreaker_36294AI(p_Creature);
        }
};

/// @Creature : Prince Liam Greymane - 37065
class npc_prince_liam_greymane_37065 : public CreatureScript
{
    public:
        npc_prince_liam_greymane_37065() : CreatureScript("npc_prince_liam_greymane_37065") { }

        struct npc_prince_liam_greymane_37065AI : public ScriptedAI
        {
            npc_prince_liam_greymane_37065AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::IntroductionsAreinOrder:
                    {
                        uint64 l_PlayerGUID = p_Player->GetGUID();

                        PersonalTalk(0, l_PlayerGUID);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            l_Me->CastSpell(Positions::g_StolenBanner, eSpells::SpellPlaceBanner, true);

                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                            {
                                if (Creature* l_Koroth = l_Player->SummonCreature(eCreatures::NpcKorothHillbreaker2, Positions::g_Koroth2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000, 0, l_Player->GetGUID()))
                                {
                                    l_Koroth->DisableEvadeMode();
                                    l_Koroth->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                    l_Koroth->GetMotionMaster()->MoveSmoothPath(1, Waypoints::g_KorothHillbreaker.data(), Waypoints::g_KorothHillbreaker.size(), false);
                                    l_Koroth->DespawnOrUnsummon(60000);
                                }

                                if (Creature* l_CaptainAsther = l_Player->SummonCreature(eCreatures::NpcCaptainAsther, Positions::g_CaptainAsther, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000, 0, l_Player->GetGUID()))
                                    l_CaptainAsther->DespawnOrUnsummon(60000);

                                for (uint8 l_I = 0; l_I < 12; ++l_I)
                                    if (Creature* l_ForsakenSoldier = l_Player->SummonCreature(eCreatures::NpcForsakenSoldier, g_ForsakenSoldierPos[l_I], TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000, 0, l_Player->GetGUID()))
                                        l_ForsakenSoldier->DespawnOrUnsummon(60000);
                            }
                        }, 3000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->SummonGameObject(eGameobjects::GobStolenBanner, Positions::g_StolenBanner, 0.0f, 0.0f, 0.0f, 60000, l_Player->GetGUID());
                        }, 4000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 6000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_prince_liam_greymane_37065AI(p_Creature);
        }
};

/// @Creature : Generic Trigger LAB - 35374
class npc_trigger_dark_scout_35374 : public CreatureScript
{
    public:
        npc_trigger_dark_scout_35374() : CreatureScript("npc_trigger_dark_scout_35374") { }

        struct npc_trigger_dark_scout_35374AI : public ScriptedAI
        {
            npc_trigger_dark_scout_35374AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void _DoCheckPlayersInRange()
            {
                std::list<Player*> l_PlayerList;
                me->GetPlayerListInGrid(l_PlayerList, 10.0f);
                for (auto l_Player : l_PlayerList)
                {
                    if (l_Player->HasAura(eSpells::SpellDarkScoutTracker) || l_Player->isInCombat())
                        continue;

                    if (l_Player->isGameMaster())
                        continue;

                    if (l_Player->HasQuest(eQuests::LosingYourTail) && l_Player->GetQuestStatus(eQuests::LosingYourTail) != QUEST_STATUS_COMPLETE)
                        me->CastSpell(l_Player, eSpells::SpellFreezingTrapEffect, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->GetNameTag() == "trigger_dark_scout")
                    _DoCheckPlayersInRange();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trigger_dark_scout_35374AI(p_Creature);
        }
};

/// @Creature : Dark Scout - 37953
class npc_dark_scout_37953 : public CreatureScript
{
    public:
        npc_dark_scout_37953() : CreatureScript("npc_dark_scout_37953") { }

        struct npc_dark_scout_37953AI : public ScriptedAI
        {
            npc_dark_scout_37953AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();

                me->AddPlayerInPersonnalVisibilityList(m_PlayerGUID);
                me->SetFacingToObject(l_Player);
                me->SetReactState(ReactStates::REACT_PASSIVE);

                Creature* l_Me = me;
                uint64 l_PlayerGUID = m_PlayerGUID;

                me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                        if (l_Me->IsAIEnabled)
                            l_Me->AI()->PersonalTalk(0, l_Player->GetGUID());
                }, 5000);

                me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                    {
                        if (l_Player->HasAura(eSpells::SpellFreezingTrapEffect))
                        {
                            if (l_Me->IsAIEnabled)
                            {
                                l_Me->AI()->PersonalTalk(1, l_Player->GetGUID());
                                l_Me->CastSpell(l_Player, eSpells::SpellAimedShot, false);
                            }
                        }
                    }
                }, 10000);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id != eSpells::SpellBelysrasTalisman)
                    return;

                l_Player->RemoveAura(eSpells::SpellFreezingTrapEffect);

                me->InterruptNonMeleeSpells(true);
                me->CastStop();
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                AttackStart(l_Player, true);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo || p_SpellInfo->Id != eSpells::SpellAimedShot)
                    return;

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                    AttackStart(l_Player, true);
            }

            void EnterEvadeMode() override
            {
                // What's the problem?
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dark_scout_37953AI(p_Creature);
        }
};

/// @Creature : Veteran Dark Ranger - 38022
class npc_veteran_dark_ranger_38022 : public CreatureScript
{
    public:
        npc_veteran_dark_ranger_38022() : CreatureScript("npc_veteran_dark_ranger_38022") { }

        struct npc_veteran_dark_ranger_38022AI : public ScriptedAI
        {
            npc_veteran_dark_ranger_38022AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_SummonGUID = 0;
            }

            uint64 m_SummonGUID;

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() != eCreatures::NpcTaldorenTracker)
                    return;

                if (p_Summon->IsAIEnabled)
                {
                    m_SummonGUID = p_Summon->GetGUID();
                    p_Summon->AI()->DoAction(eActions::ActionTaldorenTracker);
                }
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(80) && p_Source->GetEntry() == eCreatures::NpcTaldorenTracker)
                    p_Damage = 0;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (p_Target->GetGUID() == m_SummonGUID || p_Target->IsPlayer())
                    return true;

                return false;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target->GetGUID() == m_SummonGUID || p_Target->IsPlayer())
                    return true;

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_veteran_dark_ranger_38022AI(p_Creature);
        }
};

/// @Creature : Tal'doren Tracker - 38027
class npc_taldoren_tracker_38027 : public CreatureScript
{
    public:
        npc_taldoren_tracker_38027() : CreatureScript("npc_taldoren_tracker_38027") { }

        enum eMoves
        {
            MoveToJump = 1,
            MoveToDespawn
        };

        struct npc_taldoren_tracker_38027AI : public ScriptedAI
        {
            npc_taldoren_tracker_38027AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_SummonGUID = 0;
            }

            uint64 m_SummonGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonGUID = p_Summoner->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionTaldorenTracker:
                    {
                        if (Creature* l_VeteranDarkRanger = ObjectAccessor::GetCreature(*me, m_SummonGUID))
                        {
                            AttackStart(l_VeteranDarkRanger, true);
                            me->SetInCombatWith(l_VeteranDarkRanger);
                            l_VeteranDarkRanger->SetInCombatWith(me);
                        }

                        break;
                    }
                    case eActions::ActionTaldorenTracker2:
                    {
                        DoCast(eSpells::SpellWarStomp);

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->CombatStop();
                        me->StopAttack();

                        Position l_Pos = Positions::g_TaldorenTracker1;
                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 20.0f, frand(0.0f, M_PI), true);
                        me->GetMotionMaster()->MovePoint(eMoves::MoveToJump, l_Pos);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToJump:
                    {
                        me->GetMotionMaster()->MoveReverseJump(Positions::g_TaldorenTracker2, 4000, 16.0f, eMoves::MoveToDespawn);
                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (p_Target->GetGUID() == m_SummonGUID)
                    return true;

                return false;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target->GetGUID() == m_SummonGUID)
                    return true;

                return false;
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_taldoren_tracker_38027AI(p_Creature);
        }
};

/// @Creature : Tobias Mistmantle - 38029
class npc_tobias_mistmantle_38029 : public CreatureScript
{
    public:
        npc_tobias_mistmantle_38029() : CreatureScript("npc_tobias_mistmantle_38029") { }

        struct npc_tobias_mistmantle_38029AI : public ScriptedAI
        {
            npc_tobias_mistmantle_38029AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionTobiasMistmantle:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->Talk(0, l_PlayerGUID);

                            l_Me->CastSpell(l_Me, eSpells::SpellSummonTaldorenTracker, true);
                        }, 5000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->Talk(1);

                            std::list<Creature*> l_TaldorenTrackersList;
                            l_Me->GetCreatureListWithEntryInGrid(l_TaldorenTrackersList, eCreatures::NpcTaldorenTracker, 200.0f);

                            for (Creature* l_Creature : l_TaldorenTrackersList)
                                if (l_Creature->IsAIEnabled)
                                    l_Creature->AI()->DoAction(eActions::ActionTaldorenTracker2);

                        }, 50000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tobias_mistmantle_38029AI(p_Creature);
        }
};

/// @Creature : Lord Darius Crowley - 37195
class npc_lord_darius_crowley_37195 : public CreatureScript
{
    public:
        npc_lord_darius_crowley_37195() : CreatureScript("npc_lord_darius_crowley_37195") { }

        struct npc_lord_darius_crowley_37195AI : public ScriptedAI
        {
            npc_lord_darius_crowley_37195AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::AtOurDoorstep:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = p_Player->GetGUID();

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 11000);

                        if (Creature* l_TobiasMistmantle = p_Player->SummonCreature(eCreatures::NpcTobiasMistmantle3, Positions::g_TobiasMistmantl2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 25000, 0, l_PlayerGUID))
                        {
                            l_TobiasMistmantle->GetMotionMaster()->MoveSmoothPath(1, Waypoints::g_TobiasMistmantle1.data(), Waypoints::g_TobiasMistmantle1.size(), false);

                            l_TobiasMistmantle->AddDelayedEvent([l_TobiasMistmantle]() -> void
                            {
                                l_TobiasMistmantle->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
                            }, 3000);

                            l_TobiasMistmantle->AddDelayedEvent([l_TobiasMistmantle, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_TobiasMistmantle, l_PlayerGUID))
                                    if (l_TobiasMistmantle->IsAIEnabled)
                                        l_TobiasMistmantle->AI()->PersonalTalk(0, l_PlayerGUID);
                            }, 4000);

                            l_TobiasMistmantle->AddDelayedEvent([l_TobiasMistmantle, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_TobiasMistmantle, l_PlayerGUID))
                                    if (l_TobiasMistmantle->IsAIEnabled)
                                        l_TobiasMistmantle->AI()->PersonalTalk(1, l_PlayerGUID);
                            }, 17000);

                            l_TobiasMistmantle->AddDelayedEvent([l_TobiasMistmantle]() -> void
                            {
                                l_TobiasMistmantle->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
                                l_TobiasMistmantle->GetMotionMaster()->MoveSmoothPath(2, Waypoints::g_TobiasMistmantle2.data(), Waypoints::g_TobiasMistmantle2.size(), false);
                            }, 19000);
                        }

                        break;
                    }
                    case eQuests::NeitherHumanNorBeast:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = p_Player->GetGUID();

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 9000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(2, l_PlayerGUID);
                        }, 33000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(3, l_PlayerGUID);
                        }, 58000);

                        if (Creature* l_LornaCrowley = p_Player->SummonCreature(eCreatures::NpcLornaCrowley4, Positions::g_LornaCrowley2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 90000, 0, l_PlayerGUID))
                        {
                            l_LornaCrowley->GetMotionMaster()->MoveSmoothPath(1, Waypoints::g_LornaCrowley1.data(), Waypoints::g_LornaCrowley1.size(), false);

                            l_LornaCrowley->AddDelayedEvent([l_LornaCrowley, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_LornaCrowley, l_PlayerGUID))
                                    if (l_LornaCrowley->IsAIEnabled)
                                        l_LornaCrowley->AI()->PersonalTalk(0, l_PlayerGUID);
                            }, 3000);

                            l_LornaCrowley->AddDelayedEvent([l_LornaCrowley]() -> void
                            {
                                l_LornaCrowley->GetMotionMaster()->MoveSmoothPath(2, Waypoints::g_LornaCrowley2.data(), Waypoints::g_LornaCrowley2.size(), false);
                            }, 18000);
                        }

                        if (Creature* l_KingGennGreymane = p_Player->SummonCreature(eCreatures::NpcKingGennGreymane3, Positions::g_GennGreymane1, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 90000, 0, l_PlayerGUID))
                        {
                            l_KingGennGreymane->GetMotionMaster()->MoveSmoothPath(1, Waypoints::g_KingGennGreymane1.data(), Waypoints::g_KingGennGreymane1.size(), true);

                            l_KingGennGreymane->AddDelayedEvent([l_KingGennGreymane, l_PlayerGUID]() -> void
                            {
                                l_KingGennGreymane->CastSpell(l_KingGennGreymane, eSpells::SpellWorgenCombatTransform, true);
                                l_KingGennGreymane->SetDisplayId(31177);

                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_KingGennGreymane, l_PlayerGUID))
                                    if (l_KingGennGreymane->IsAIEnabled)
                                        l_KingGennGreymane->AI()->PersonalTalk(0, l_PlayerGUID);
                            }, 47000);

                            l_KingGennGreymane->AddDelayedEvent([l_KingGennGreymane, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_KingGennGreymane, l_PlayerGUID))
                                    if (l_KingGennGreymane->IsAIEnabled)
                                        l_KingGennGreymane->AI()->PersonalTalk(1, l_PlayerGUID);
                            }, 69000);
                        }

                        if (Creature* l_LordGodfrey = p_Player->SummonCreature(eCreatures::NpcLordGodfrey2, Positions::g_LordGodfrey1, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 90000, 0, l_PlayerGUID))
                        {
                            l_LordGodfrey->GetMotionMaster()->MoveSmoothPath(1, Waypoints::g_LordGodfrey1.data(), Waypoints::g_LordGodfrey1.size(), true);

                            l_LordGodfrey->AddDelayedEvent([l_LordGodfrey, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_LordGodfrey, l_PlayerGUID))
                                    if (l_LordGodfrey->IsAIEnabled)
                                        l_LordGodfrey->AI()->PersonalTalk(0, l_PlayerGUID);
                            }, 17000);

                            l_LordGodfrey->AddDelayedEvent([l_LordGodfrey, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_LordGodfrey, l_PlayerGUID))
                                    if (l_LordGodfrey->IsAIEnabled)
                                        l_LordGodfrey->AI()->PersonalTalk(1, l_PlayerGUID);
                            }, 47000);
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
            return new npc_lord_darius_crowley_37195AI(p_Creature);
        }
};

/// @Creature : King Genn Greymane - 37876
class npc_king_genn_greymane_37876 : public CreatureScript
{
    public:
        npc_king_genn_greymane_37876() : CreatureScript("npc_king_genn_greymane_37876") { }

        struct npc_king_genn_greymane_37876AI : public ScriptedAI
        {
            npc_king_genn_greymane_37876AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::BetrayalatTempestsReach:
                    {
                        PersonalTalk(0, p_Player->GetGUID());

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = p_Player->GetGUID();

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Creature* l_LordGodfrey = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcLordGodfrey3, 10.0f))
                                if (l_LordGodfrey->IsAIEnabled)
                                    l_LordGodfrey->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 5000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_king_genn_greymane_37876AI(p_Creature);
        }
};

/// @Creature: Lord Hewell - 38764
class npc_lord_hewell_38764 : public CreatureScript
{
    public:
        npc_lord_hewell_38764() : CreatureScript("npc_lord_hewell_38764") { }

        struct npc_lord_hewell_38764AI : public ScriptedAI
        {
            npc_lord_hewell_38764AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 p_Action) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();

                switch (p_Action)
                {
                    case 0:
                    {
                        if (Creature* l_StoutMountainHorse = p_Player->SummonCreature(eCreatures::NpcStoutMountainHorse, p_Player->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            if (l_StoutMountainHorse->IsAIEnabled)
                                l_StoutMountainHorse->AI()->DoAction(eActions::ActionStoutMountainHorse);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lord_hewell_38764AI(p_Creature);
        }
};

/// @Creature : Stout Mountain Horse - 38765
class npc_stout_mountain_horse_38765 : public CreatureScript
{
    public:
        npc_stout_mountain_horse_38765() : CreatureScript("npc_stout_mountain_horse_38765") { }

        enum eMoves
        {
            MoveToDespawn1 = 1,
            MoveToDespawn2,
            MoveToDespawn3
        };

        struct npc_stout_mountain_horse_38765AI : public VehicleAI
        {
            npc_stout_mountain_horse_38765AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStoutMountainHorse:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->EnterVehicle(me, 0);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn1, Waypoints::g_StoutMountainHorse1.data(), Waypoints::g_StoutMountainHorse1.size(), false);
                        }, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToDespawn1:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn2, Waypoints::g_StoutMountainHorse2.data(), Waypoints::g_StoutMountainHorse2.size(), false);
                        break;
                    }
                    case eMoves::MoveToDespawn2:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn3, Waypoints::g_StoutMountainHorse3.data(), Waypoints::g_StoutMountainHorse3.size(), false);
                        break;
                    }
                    case eMoves::MoveToDespawn3:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->ExitVehicle();
                        }, 1000);

                        me->DespawnOrUnsummon(3000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stout_mountain_horse_38765AI(p_Creature);
        }
};

/// @Creature : Lorna Crowley - 38611
class npc_lorna_crowley_38611 : public CreatureScript
{
    public:
        npc_lorna_crowley_38611() : CreatureScript("npc_lorna_crowley_38611") { }

        struct npc_lorna_crowley_38611AI : public ScriptedAI
        {
            npc_lorna_crowley_38611AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::TheHuntForSylvanas:
                    {
                        if (Creature* l_TobiasMistmantle = p_Player->SummonCreature(eCreatures::NpcTobiasMistmantle4, Positions::g_Tobias1, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            if (l_TobiasMistmantle->IsAIEnabled)
                                l_TobiasMistmantle->AI()->DoAction(eActions::ActionTobiasMistmantle2);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lorna_crowley_38611AI(p_Creature);
        }
};

/// @Creature : Tobias Mistmantle - 38507
class npc_tobias_mistmantle_38507 : public CreatureScript
{
    public:
        npc_tobias_mistmantle_38507() : CreatureScript("npc_tobias_mistmantle_38507") { }

        enum eMoves
        {
            MoveToJump1 = 1,
            MoveToJump2,
            MoveToJump3,
            MoveToJump4,
            MoveToJump5,
            MoveToEvent,
            MoveToCathedral,
            MoveToWater
        };

        struct npc_tobias_mistmantle_38507AI : public ScriptedAI
        {
            npc_tobias_mistmantle_38507AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionTobiasMistmantle2:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        PersonalTalk(0, l_PlayerGUID);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 8000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToJump1, Waypoints::g_Tobias1.data(), Waypoints::g_Tobias1.size(), false);
                        }, 12000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToJump1:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        PersonalTalk(2, l_PlayerGUID);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveReverseJump(Positions::g_Tobias2, 2540, 3.720007f, eMoves::MoveToJump2);
                        }, 4000);

                        break;
                    }
                    case eMoves::MoveToJump2:
                    {
                        me->SetFacingTo(1.433004f);
                        me->SetOrientation(1.433004f);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveReverseJump(Positions::g_Tobias3, 997, 19.2911f, eMoves::MoveToJump3);
                        }, 10000);

                        break;
                    }
                    case eMoves::MoveToJump3:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToJump4, Waypoints::g_Tobias2.data(), Waypoints::g_Tobias2.size(), false);
                        break;
                    }
                    case eMoves::MoveToJump4:
                    {
                        me->GetMotionMaster()->MoveReverseJump(Positions::g_Tobias4, 1104, 19.2911f, eMoves::MoveToJump5);
                        break;
                    }
                    case eMoves::MoveToJump5:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToEvent, Waypoints::g_Tobias3.data(), Waypoints::g_Tobias3.size(), false);
                        break;
                    }
                    case eMoves::MoveToEvent:
                    {
                        me->SetFacingTo(1.063866f);
                        me->SetOrientation(1.063866f);

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Creature* l_ForsakenGeneral = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcForsakenGeneral, 20.0f))
                                if (l_ForsakenGeneral->IsAIEnabled)
                                    l_ForsakenGeneral->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 2000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(3, l_PlayerGUID);
                        }, 10000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToCathedral, Waypoints::g_Tobias4.data(), Waypoints::g_Tobias4.size(), false);
                        }, 16000);

                        break;
                    }
                    case eMoves::MoveToCathedral:
                    {
                        PersonalTalk(4, m_PlayerGUID);
                        me->GetMotionMaster()->MoveReverseJump(Positions::g_Tobias5, 392, 26.03082f, eMoves::MoveToWater);
                        break;
                    }
                    case eMoves::MoveToWater:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                        {
                            if (Creature* l_SylvanasWindrunner = l_Player->SummonCreature(eCreatures::NpcSylvanasWindrunner, Positions::g_Sylvanas1, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, m_PlayerGUID))
                                if (l_SylvanasWindrunner->IsAIEnabled)
                                    l_SylvanasWindrunner->AI()->DoAction(eActions::ActionSylvanasWindrunner);

                            if (Creature* l_HighExecutorCrenshaw = l_Player->SummonCreature(eCreatures::NpcHighExecutorCrenshaw, Positions::g_Crenshaw1, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, m_PlayerGUID))
                                if (l_HighExecutorCrenshaw->IsAIEnabled)
                                    l_HighExecutorCrenshaw->AI()->DoAction(eActions::ActionHighExecutorCrenshaw);

                            if (Creature* l_GeneralWarhowl = l_Player->SummonCreature(eCreatures::NpcGeneralWarhowl, Positions::g_Warhowl1, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, m_PlayerGUID))
                                if (l_GeneralWarhowl->IsAIEnabled)
                                    l_GeneralWarhowl->AI()->DoAction(eActions::ActionGeneralWarhowl);
                        }

                        me->DespawnOrUnsummon(20000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tobias_mistmantle_38507AI(p_Creature);
        }
};

/// @Creature : Lady Sylvanas Windrunner - 38530
class npc_lady_sylvanas_windrunner_38530 : public CreatureScript
{
    public:
        npc_lady_sylvanas_windrunner_38530() : CreatureScript("npc_lady_sylvanas_windrunner_38530") { }

        enum eMoves
        {
            MoveToTalk = 1,
            MoveToDespawn
        };

        struct npc_lady_sylvanas_windrunner_38530AI : public ScriptedAI
        {
            npc_lady_sylvanas_windrunner_38530AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionSylvanasWindrunner:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToTalk, Waypoints::g_Sylvanas1.data(), Waypoints::g_Sylvanas1.size(), true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        me->SetFacingTo(0.8028514f);
                        me->SetOrientation(0.8028514f);

                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 10000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 32000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(2, l_PlayerGUID);
                        }, 56000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Me, l_PlayerGUID))
                                l_Player->CompleteQuest(eQuests::TheHuntForSylvanas);

                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(3, l_PlayerGUID);

                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_Sylvanas2.data(), Waypoints::g_Sylvanas2.size(), true);
                        }, 74000);

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_sylvanas_windrunner_38530AI(p_Creature);
        }
};

/// @Creature : High Executor Crenshaw - 38537
class npc_high_executor_crenshaw_38537 : public CreatureScript
{
    public:
        npc_high_executor_crenshaw_38537() : CreatureScript("npc_high_executor_crenshaw_38537") { }

        enum eMoves
        {
            MoveToTalk = 1,
            MoveToDespawn
        };

        struct npc_high_executor_crenshaw_38537AI : public ScriptedAI
        {
            npc_high_executor_crenshaw_38537AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionHighExecutorCrenshaw:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToTalk, Waypoints::g_Crenshaw1.data(), Waypoints::g_Crenshaw1.size(), true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(0, l_PlayerGUID);
                        }, 60000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 86000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_Crenshaw2.data(), Waypoints::g_Crenshaw2.size(), true);
                        }, 88000);

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_executor_crenshaw_38537AI(p_Creature);
        }
};

/// @Creature : General Warhowl - 38533
class npc_general_warhowl_38533 : public CreatureScript
{
    public:
        npc_general_warhowl_38533() : CreatureScript("npc_general_warhowl_38533") { }

        enum eMoves
        {
            MoveToTalk = 1,
            MoveToDespawn
        };

        struct npc_general_warhowl_38533AI : public ScriptedAI
        {
            npc_general_warhowl_38533AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionGeneralWarhowl:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToTalk, Waypoints::g_Warhowl1.data(), Waypoints::g_Warhowl1.size(), true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToTalk:
                    {
                        Creature* l_Me = me;
                        uint64 l_PlayerGUID = m_PlayerGUID;

                        PersonalTalk(0, l_PlayerGUID);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(1, l_PlayerGUID);
                        }, 20000);

                        me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                        {
                            if (l_Me->IsAIEnabled)
                                l_Me->AI()->PersonalTalk(2, l_PlayerGUID);
                        }, 50000);

                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_Warhowl2.data(), Waypoints::g_Warhowl2.size(), true);
                        }, 52000);

                        break;
                    }
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_general_warhowl_38533AI(p_Creature);
        }
};

/// @Creature : Forsaken Catapult - 38287
class npc_forsaken_catapult_38287 : public CreatureScript
{
    public:
        npc_forsaken_catapult_38287() : CreatureScript("npc_forsaken_catapult_38287") { }

        enum eEvents
        {
            EventPlagueBarrel = 1
        };

        struct npc_forsaken_catapult_38287AI : public ScriptedAI
        {
            npc_forsaken_catapult_38287AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                if (me->GetNameTag() == "trigger_barrel")
                    events.ScheduleEvent(eEvents::EventPlagueBarrel, urand(1000, 6000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventPlagueBarrel:
                    {
                        std::list<Creature*> l_PlagueCloudBunnyList;
                        me->GetCreatureListWithEntryInGrid(l_PlagueCloudBunnyList, eCreatures::NpcPlagueCloudBunny, 150.0f);

                        if (!l_PlagueCloudBunnyList.empty())
                        {
                            JadeCore::Containers::RandomResizeList(l_PlagueCloudBunnyList, 1);
                        }

                        if (!l_PlagueCloudBunnyList.empty())
                        {
                            for (Creature* l_Creature : l_PlagueCloudBunnyList)
                                DoCast(l_Creature, eSpells::SpellPlagueBarrelLaunch);
                        }

                        events.ScheduleEvent(eEvents::EventPlagueBarrel, urand(1000, 6000));
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_catapult_38287AI(p_Creature);
        }
};

/// @Creature : Forsaken Invader - 38363
class npc_forsaken_invader_38363 : public CreatureScript
{
    public:
        npc_forsaken_invader_38363() : CreatureScript("npc_forsaken_invader_38363") { }

        enum eEvents
        {
            EventShootPlague = 1
        };

        struct npc_forsaken_invader_38363AI : public ScriptedAI
        {
            npc_forsaken_invader_38363AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (me->GetNameTag() == "trigger_shoot_plague")
                    events.ScheduleEvent(eEvents::EventShootPlague, urand(1000, 6000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShootPlague:
                    {
                        DoCast(eSpells::SpellShootPlague);
                        events.ScheduleEvent(eEvents::EventShootPlague, urand(1000, 6000));
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_invader_38363AI(p_Creature);
        }
};

/// @Creature : Slime Spigot Bunny - 38365
class npc_slime_spigot_bunny_38365 : public CreatureScript
{
    public:
        npc_slime_spigot_bunny_38365() : CreatureScript("npc_slime_spigot_bunny_38365") { }

        enum eEvents
        {
            EventSlimeSpigo = 1
        };

        struct npc_slime_spigot_bunny_38365AI : public ScriptedAI
        {
            npc_slime_spigot_bunny_38365AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();
                me->SetReactState(ReactStates::REACT_PASSIVE);

                events.ScheduleEvent(eEvents::EventSlimeSpigo, 1000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSlimeSpigo:
                    {
                        DoCast(eSpells::SpellSlimeSpigo);
                        events.ScheduleEvent(eEvents::EventSlimeSpigo, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_slime_spigot_bunny_38365AI(p_Creature);
        }
};

/// @Creature : Captured Riding Bat - 38615
class npc_captured_riding_bat_38615 : public CreatureScript
{
    public:
        npc_captured_riding_bat_38615() : CreatureScript("npc_captured_riding_bat_38615") { }

        struct npc_captured_riding_bat_38615AI : public ScriptedAI
        {
            npc_captured_riding_bat_38615AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                if (Creature* l_CapturedRidingBat = l_Player->SummonCreature(eCreatures::NpcCapturedRidingBat2, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    if (l_CapturedRidingBat->IsAIEnabled)
                        l_CapturedRidingBat->AI()->DoAction(eActions::ActionCapturedRidingBat);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_captured_riding_bat_38615AI(p_Creature);
        }
};

/// @Creature : Captured Riding Bat - 38540
class npc_captured_riding_bat_38540 : public CreatureScript
{
    public:
        npc_captured_riding_bat_38540() : CreatureScript("npc_captured_riding_bat_38540") { }

        enum eMoves
        {
            MoveToStart = 1,
            MoveToLoop1,
            MoveToLoop2,
            MoveToLoop3,
            MoveTakeoff1,
            MoveTakeoff2,
            MoveToEnd
        };

        struct npc_captured_riding_bat_38540AI : public VehicleAI
        {
            npc_captured_riding_bat_38540AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_Exit = false;
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            bool m_Exit;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCapturedRidingBat:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->EnterVehicle(me, 0);

                        Creature* l_Me = me;
                        me->AddDelayedEvent([l_Me]() -> void
                        {
                            l_Me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                            l_Me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveToStart, Waypoints::g_CapturedRidingBatStart.data(), Waypoints::g_CapturedRidingBatStart.size());
                        }, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToStart:
                    {
                        if (m_Exit)
                            return;

                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveToLoop1, Waypoints::g_CapturedRidingBatLoop1.data(), Waypoints::g_CapturedRidingBatLoop1.size());
                        break;
                    }
                    case eMoves::MoveToLoop1:
                    {
                        if (m_Exit)
                            return;

                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveToLoop2, Waypoints::g_CapturedRidingBatLoop2.data(), Waypoints::g_CapturedRidingBatLoop2.size());
                        break;
                    }
                    case eMoves::MoveToLoop2:
                    {
                        if (m_Exit)
                            return;

                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveToLoop3, Waypoints::g_CapturedRidingBatLoop3.data(), Waypoints::g_CapturedRidingBatLoop3.size());
                        break;
                    }
                    case eMoves::MoveToLoop3:
                    {
                        if (m_Exit)
                            return;

                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveToLoop1, Waypoints::g_CapturedRidingBatLoop1.data(), Waypoints::g_CapturedRidingBatLoop1.size());
                        break;
                    }
                    case eMoves::MoveTakeoff1:
                    {
                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveTakeoff2, Positions::g_CapturedBatEnd);
                        break;
                    }
                    case eMoves::MoveTakeoff2:
                    {
                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 2.5f, true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveToEnd, Waypoints::g_CapturedRidingBatEnd.data(), Waypoints::g_CapturedRidingBatEnd.size());
                        break;
                    }
                    case eMoves::MoveToEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            l_Player->ExitVehicle();

                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id != eSpells::SpellFlyBack)
                    return;

                if (m_Exit)
                    return;

                m_Exit = true;

                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
                me->SetDisableGravity(true);

                Unit* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    Position l_Pos = l_Me->GetPosition();
                    l_Pos.m_positionZ = 133.0f;

                    l_Me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.5f, true);
                    l_Me->GetMotionMaster()->MoveTakeoff(eMoves::MoveTakeoff1, l_Pos, 3);
                }, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_captured_riding_bat_38540AI(p_Creature);
        }
};

/// @Creature : Gilneas Funeral Camera - 51083
class npc_gilneas_funeral_camera_51083 : public CreatureScript
{
    public:
        npc_gilneas_funeral_camera_51083() : CreatureScript("npc_gilneas_funeral_camera_51083") { }

        struct npc_gilneas_funeral_camera_51083AI : public VehicleAI
        {
            npc_gilneas_funeral_camera_51083AI(Creature* p_Creature) : VehicleAI(p_Creature)
            {
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                Player* l_Player = p_Passenger->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Apply)
                {
                    l_Player->SetInPhase(2467901, true, true);

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        if (Creature* l_KingGennGreymane = GetClosestCreatureWithEntry(l_Player, eCreatures::NpcKingGennGreymane4, 50.0f))
                            if (l_KingGennGreymane->IsAIEnabled)
                                l_KingGennGreymane->AI()->PersonalTalk(0, l_Player->GetGUID());
                    }, 6000);

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        if (l_Player->IsOnVehicle())
                            l_Player->ExitVehicle();
                    }, 12000);
                }
                else
                {
                    l_Player->SetInPhase(2467901, true, false);
                    l_Player->CastSpell(l_Player, eSpells::SpellFuneralExit, true);
                    me->DespawnOrUnsummon(2000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilneas_funeral_camera_51083AI(p_Creature);
        }
};

/// @Creature : Riding War Wolf - 37939
class npc_riding_war_wolf_37939 : public CreatureScript
{
    public:
        npc_riding_war_wolf_37939() : CreatureScript("npc_riding_war_wolf_37939") { }

        struct npc_riding_war_wolf_37939AI : public VehicleAI
        {
            npc_riding_war_wolf_37939AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            void Reset() override
            {
                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_WolfmawOutrider = l_Me->SummonCreature(eCreatures::NpcWolfmawOutrider, l_Me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        l_WolfmawOutrider->EnterVehicle(l_Me, 0);
                }, 100);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_riding_war_wolf_37939AI(p_Creature);
        }
};

/// @Creature : Orcish War Machine - 37921
class npc_orcish_war_machine_37921 : public CreatureScript
{
    public:
        npc_orcish_war_machine_37921() : CreatureScript("npc_orcish_war_machine_37921") { }

        enum eEvents
        {
            EventFlamingBoulder = 1
        };

        struct npc_orcish_war_machine_37921AI : public ScriptedAI
        {
            npc_orcish_war_machine_37921AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, true);

                events.ScheduleEvent(eEvents::EventFlamingBoulder, urand(5000, 15000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFlamingBoulder:
                    {
                        std::list<Creature*> l_ToweringAncientList;
                        me->GetCreatureListWithEntryInGrid(l_ToweringAncientList, eCreatures::NpcToweringAncient, 80.0f);

                        if (!l_ToweringAncientList.empty())
                        {
                            l_ToweringAncientList.sort(JadeCore::DistanceCompareOrderPred(me));
                            l_ToweringAncientList.resize(1);
                        }

                        if (!l_ToweringAncientList.empty())
                        {
                            for (Creature* l_Creature : l_ToweringAncientList)
                                DoCast(l_Creature->GetPosition(), eSpells::SpellFlamingBoulder);
                        }

                        events.ScheduleEvent(eEvents::EventFlamingBoulder, urand(5000, 15000));
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_orcish_war_machine_37921AI(p_Creature);
        }
};

/// @Creature : Orc Raider - 37916
class npc_orc_raider_37916 : public CreatureScript
{
    public:
        npc_orc_raider_37916() : CreatureScript("npc_orc_raider_37916") { }

        struct npc_orc_raider_37916AI : public ScriptedAI
        {
            npc_orc_raider_37916AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_ToweringAncient = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcToweringAncient, 20.0f))
                        l_Me->CombatStart(l_ToweringAncient, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(50) && p_Source->GetEntry() == eCreatures::NpcToweringAncient)
                    p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_orc_raider_37916AI(p_Creature);
        }
};

/// @Creature : Towering Ancient - 37914
class npc_towering_ancient_37914 : public CreatureScript
{
    public:
        npc_towering_ancient_37914() : CreatureScript("npc_towering_ancient_37914") { }

        struct npc_towering_ancient_37914AI : public ScriptedAI
        {
            npc_towering_ancient_37914AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                CreatureAI::Reset();

                Creature* l_Me = me;
                me->AddDelayedEvent([l_Me]() -> void
                {
                    if (Creature* l_OrcRaider = GetClosestCreatureWithEntry(l_Me, eCreatures::NpcOrcRaider, 20.0f))
                        if (!l_OrcRaider->isInCombat())
                            l_Me->CombatStart(l_OrcRaider, true);
                }, 1000);
            }

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_towering_ancient_37914AI(p_Creature);
        }
};

/// @Creature : Lorna Crowley - 43727
class npc_lorna_crowley_43727 : public CreatureScript
{
    public:
        npc_lorna_crowley_43727() : CreatureScript("npc_lorna_crowley_43727") { }

        struct npc_lorna_crowley_43727AI : public ScriptedAI
        {
            npc_lorna_crowley_43727AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::Endgame:
                    {
                        PersonalTalk(0, p_Player->GetGUID());

                        bool l_First = false;

                        for (uint8 l_I = 0; l_I < 6; ++l_I)
                        {
                            if (Creature* l_GilneanSurvivor = p_Player->SummonCreature(eCreatures::NpcGilneanSurvivor, g_GilneanSurvivorPos[l_I], TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                            {
                                if (l_GilneanSurvivor->IsAIEnabled)
                                {
                                    if (!l_First)
                                    {
                                        l_First = true;
                                        l_GilneanSurvivor->AI()->SetGUID(me->GetGUID());
                                    }

                                    l_GilneanSurvivor->AI()->DoAction(eActions::ActionGilneanSurvivor);
                                }
                            }
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
            return new npc_lorna_crowley_43727AI(p_Creature);
        }
};

/// @Creature : Gilnean Survivor - 38781
class npc_gilnean_survivor_38781 : public CreatureScript
{
    public:
        npc_gilnean_survivor_38781() : CreatureScript("npc_gilnean_survivor_38781") { }

        enum eMoves
        {
            MoveToDespawn = 1
        };

        struct npc_gilnean_survivor_38781AI : public ScriptedAI
        {
            npc_gilnean_survivor_38781AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_TalkDone = false;
                m_PlayerGUID = 0;
            }

            uint64 m_PlayerGUID;
            bool m_TalkDone;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                m_PlayerGUID = l_Player->GetGUID();
            }

            void SetGUID(uint64 /*p_Guid*/, int32 /*p_ID*/) override
            {
                m_TalkDone = true;
            }

            void DoAction(int32 p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionGilneanSurvivor:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveToDespawn, Waypoints::g_GilneanSurvivor.data(), Waypoints::g_GilneanSurvivor.size(), false);

                        if (m_TalkDone)
                        {
                            Creature* l_Me = me;
                            uint64 l_PlayerGUID = m_PlayerGUID;

                            me->AddDelayedEvent([l_Me, l_PlayerGUID]() -> void
                            {
                                if (l_Me->IsAIEnabled)
                                    l_Me->AI()->PersonalTalk(0, l_PlayerGUID);
                            }, 3000);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eMoves::MoveToDespawn:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gilnean_survivor_38781AI(p_Creature);
        }
};

/// @Gameobject: Merchant_Square_Door - 195327
class gob_merchant_square_door_195327 : public GameObjectScript
{
    public:
        gob_merchant_square_door_195327() : GameObjectScript("gob_merchant_square_door_195327") { }

        struct gob_merchant_square_door_195327AI : public GameObjectAI
        {
            gob_merchant_square_door_195327AI(GameObject* p_Go) : GameObjectAI(p_Go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (!p_Unit)
                    return;

                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetQuestStatus(eQuests::EvacuatetheMerchantSquare) == QUEST_STATUS_INCOMPLETE)
                {
                    go->SetGoState(GOState::GO_STATE_ACTIVE);

                    Position l_Pos;
                    go->GetNearPosition(l_Pos, -2.0f, 0.0f);

                    if (urand(0, 100) > 30)
                    {
                        if (Creature* l_FrightenedCitizenFriendly = l_Player->SummonCreature(eCreatures::NpcFrightenedCitizen1, l_Pos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            if (l_FrightenedCitizenFriendly->IsAIEnabled)
                                l_FrightenedCitizenFriendly->AI()->DoAction(eActions::ActionFrightenedCitizen1);
                    }
                    else
                    {
                        if (Creature* l_FrightenedCitizenHostile = l_Player->SummonCreature(eCreatures::NpcFrightenedCitizen2, l_Pos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            if (l_FrightenedCitizenHostile->IsAIEnabled)
                                l_FrightenedCitizenHostile->AI()->DoAction(eActions::ActionFrightenedCitizen2);
                    }

                    GameObject* l_Go = go;
                    go->AddDelayedEvent([l_Go]() -> void
                    {
                        l_Go->SetGoState(GOState::GO_STATE_READY);
                    }, 30000);
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_Go) const override
        {
            return new gob_merchant_square_door_195327AI(p_Go);
        }
};

/// @Gameobject: Dustfall - 196465
class go_dustfall_196465 : public GameObjectScript
{
    public:
        go_dustfall_196465() : GameObjectScript("go_dustfall_196465") { }

        struct go_dustfall_196465AI : public GameObjectAI
        {
            go_dustfall_196465AI(GameObject* p_GameObject) : GameObjectAI(p_GameObject)
            {
                m_AnimTimer = 120000;
            }

            uint32 m_AnimTimer;

            void UpdateAI(uint32 p_Diff) override
            {
                if (m_AnimTimer)
                {
                    if (m_AnimTimer <= p_Diff)
                    {
                        m_AnimTimer = 120000;
                        go->SendCustomAnim(1);
                    }
                    else
                        m_AnimTimer -= p_Diff;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_dustfall_196465AI(p_GameObject);
        }
};

/// @Gameobject: Well of Fury - 201950
class gob_well_of_fury_201950 : public GameObjectScript
{
    public:
        gob_well_of_fury_201950() : GameObjectScript("gob_well_of_fury_201950") { }

        struct gob_well_of_fury_201950AI : public GameObjectAI
        {
            gob_well_of_fury_201950AI(GameObject* p_Go) : GameObjectAI(p_Go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (!p_Unit)
                    return;

                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->IsObjectInLockoutList(go->GetDBTableGUIDLow()))
                    return;

                if (p_State == 2)
                {
                    l_Player->AddObjectInLockoutList(go->GetDBTableGUIDLow(), eQuests::NeitherHumanNorBeast);

                    if (Creature* l_LyrosSwiftwind = GetClosestCreatureWithEntry(l_Player, eCreatures::NpcLyrosSwiftwind, 20.0f))
                        if (l_LyrosSwiftwind->IsAIEnabled)
                            l_LyrosSwiftwind->AI()->PersonalTalk(0, l_Player->GetGUID());

                    if (Creature* l_GenericTrigger = l_Player->SummonCreature(eCreatures::NpcGenericTrigger, go->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 15000, 0, l_Player->GetGUID()))
                        l_GenericTrigger->CastSpell(l_GenericTrigger, eSpells::SpellTaldorenWell, true);
                }
            }

            void OnBuildDynFlags(uint16& p_DynFlags, Player* p_Player) override
            {
                if (p_Player->IsObjectInLockoutList(go->GetDBTableGUIDLow()))
                {
                    p_DynFlags = GO_DYNFLAG_LO_NO_INTERACT;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_Go) const override
        {
            return new gob_well_of_fury_201950AI(p_Go);
        }
};

/// @Gameobject: Well of Tranquility - 201951
class gob_well_of_tranquility_201951 : public GameObjectScript
{
    public:
        gob_well_of_tranquility_201951() : GameObjectScript("gob_well_of_tranquility_201951") { }

        struct gob_well_of_tranquility_201951AI : public GameObjectAI
        {
            gob_well_of_tranquility_201951AI(GameObject* p_Go) : GameObjectAI(p_Go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (!p_Unit)
                    return;

                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->IsObjectInLockoutList(go->GetDBTableGUIDLow()))
                    return;

                if (p_State == 2)
                {
                    l_Player->AddObjectInLockoutList(go->GetDBTableGUIDLow(), eQuests::NeitherHumanNorBeast);

                    if (Creature* l_VassandraStormclaw = GetClosestCreatureWithEntry(l_Player, eCreatures::NpcVassandraStormclaw, 20.0f))
                        if (l_VassandraStormclaw->IsAIEnabled)
                            l_VassandraStormclaw->AI()->PersonalTalk(0, l_Player->GetGUID());

                    if (Creature* l_GenericTrigger = l_Player->SummonCreature(eCreatures::NpcGenericTrigger, go->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 15000, 0, l_Player->GetGUID()))
                        l_GenericTrigger->CastSpell(l_GenericTrigger, eSpells::SpellTaldorenWell, true);
                }
            }

            void OnBuildDynFlags(uint16& p_DynFlags, Player* p_Player) override
            {
                if (p_Player->IsObjectInLockoutList(go->GetDBTableGUIDLow()))
                {
                    p_DynFlags = GO_DYNFLAG_LO_NO_INTERACT;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_Go) const override
        {
            return new gob_well_of_tranquility_201951AI(p_Go);
        }
};

/// @Gameobject: Well of Balance - 201952
class gob_well_of_balance_201952 : public GameObjectScript
{
    public:
        gob_well_of_balance_201952() : GameObjectScript("gob_well_of_balance_201952") { }

        struct gob_well_of_balance_201952AI : public GameObjectAI
        {
            gob_well_of_balance_201952AI(GameObject* p_Go) : GameObjectAI(p_Go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (!p_Unit)
                    return;

                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->IsObjectInLockoutList(go->GetDBTableGUIDLow()))
                    return;

                if (p_State == 2)
                {
                    l_Player->AddObjectInLockoutList(go->GetDBTableGUIDLow(), eQuests::NeitherHumanNorBeast);

                    if (Creature* l_TalranoftheWild = GetClosestCreatureWithEntry(l_Player, eCreatures::NpcTalranoftheWild, 20.0f))
                        if (l_TalranoftheWild->IsAIEnabled)
                            l_TalranoftheWild->AI()->PersonalTalk(0, l_Player->GetGUID());

                    if (Creature* l_GenericTrigger = l_Player->SummonCreature(eCreatures::NpcGenericTrigger, go->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 15000, 0, l_Player->GetGUID()))
                        l_GenericTrigger->CastSpell(l_GenericTrigger, eSpells::SpellTaldorenWell, true);
                }
            }

            void OnBuildDynFlags(uint16& p_DynFlags, Player* p_Player) override
            {
                if (p_Player->IsObjectInLockoutList(go->GetDBTableGUIDLow()))
                {
                    p_DynFlags = GO_DYNFLAG_LO_NO_INTERACT;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_Go) const override
        {
            return new gob_well_of_balance_201952AI(p_Go);
        }
};

/// @Gameobject: Ball and Chain - 201775
class gob_ball_and_chain_201775 : public GameObjectScript
{
    public:
        gob_ball_and_chain_201775() : GameObjectScript("gob_ball_and_chain_201775") { }

        struct gob_ball_and_chain_201775AI : public GameObjectAI
        {
            gob_ball_and_chain_201775AI(GameObject* p_Go) : GameObjectAI(p_Go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (!p_Unit)
                    return;

                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;

                if (p_State == 2)
                {
                    uint64 l_PlayerGUID = l_Player->GetGUID();

                    if (Creature* l_EnslavedVillager = GetClosestCreatureWithEntry(go, eCreatures::NpcEnslavedVillager, 5.0f))
                        if (l_EnslavedVillager->IsAIEnabled)
                        {
                            l_EnslavedVillager->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_ONESHOT_NONE);
                            l_EnslavedVillager->AI()->PersonalTalk(0, l_PlayerGUID);

                            l_EnslavedVillager->AddDelayedEvent([l_EnslavedVillager, l_PlayerGUID]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_EnslavedVillager, l_PlayerGUID))
                                    l_EnslavedVillager->DestroyForPlayer(l_Player);
                            }, 4000);

                            l_EnslavedVillager->AddDelayedEvent([l_EnslavedVillager]() -> void
                            {
                                l_EnslavedVillager->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_WORK_MINING);
                            }, 10000);
                        }

                    go->DestroyForPlayer(l_Player);
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_Go) const override
        {
            return new gob_ball_and_chain_201775AI(p_Go);
        }
};

/// @Spell : Attack Lurker - 67805
class spell_attack_lurker_67805 : public SpellScriptLoader
{
    public:
        spell_attack_lurker_67805() : SpellScriptLoader("spell_attack_lurker_67805") { }

        class spell_attack_lurker_67805_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_attack_lurker_67805_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Creature* l_BloodfangLurker = GetClosestCreatureWithEntry(l_Caster, eCreatures::NpcBloodfangLurker, 30.0f);
                if (l_BloodfangLurker)
                {
                    if (!l_BloodfangLurker->IsWithinLOS(l_Caster->m_positionX, l_Caster->m_positionY, l_Caster->m_positionZ))
                    {
                        l_Caster->SendPetCastFail(eSpells::SpellAttackLurker, SpellCastResult::SPELL_FAILED_LINE_OF_SIGHT, 0, GetSpell()->GetCastGuid());
                        return SPELL_FAILED_DONT_REPORT;
                    }
                    else
                        return SPELL_CAST_OK;
                }
                else
                {
                    l_Caster->SendPetCastFail(eSpells::SpellAttackLurker, SpellCastResult::SPELL_FAILED_OUT_OF_RANGE, 0, GetSpell()->GetCastGuid());
                    return SPELL_FAILED_DONT_REPORT;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_attack_lurker_67805_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_attack_lurker_67805_SpellScript();
        }
};

/// @Spell : Rescue Krennan - 68219
class spell_rescue_krennan_68219 : public SpellScriptLoader
{
    public:
        spell_rescue_krennan_68219() : SpellScriptLoader("spell_rescue_krennan_68219") { }

        class spell_rescue_krennan_68219_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rescue_krennan_68219_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Creature* l_NpcKrennanAranasTree = GetClosestCreatureWithEntry(l_Caster, eCreatures::NpcKrennanAranasTree, 5.0f);
                if (l_NpcKrennanAranasTree)
                    return SPELL_CAST_OK;

                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rescue_krennan_68219_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rescue_krennan_68219_SpellScript();
        }
};

/// @Spell: Throw Torch - 67063
class spell_throw_torch_67063 : public SpellScriptLoader
{
    public:
        spell_throw_torch_67063() : SpellScriptLoader("spell_throw_torch_67063") { }

        class spell_throw_torch_67063_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_throw_torch_67063_AuraScript);

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (AuraEffect* l_AuraEffect = const_cast<AuraEffect*>(p_AuraEffect))
                    l_AuraEffect->SetAmount(int32(l_Target->CountPctFromMaxHealth(50)));
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_throw_torch_67063_AuraScript::HandleOnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_throw_torch_67063_AuraScript();
        }
};

/// @Spell : Toss Keg - 69094
class spell_toss_keg_69094 : public SpellScriptLoader
{
    public:
        spell_toss_keg_69094() : SpellScriptLoader("spell_toss_keg_69094") { }

        class spell_toss_keg_69094_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toss_keg_69094_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Target->GetEntry() != eCreatures::NpcHorridAbomination || l_Target->isDead())
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Target->HasAura(eSpells::SpellKegPlaced))
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_toss_keg_69094_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_toss_keg_69094_SpellScript();
        }
};

/// @Spell: Launch - 68659
class spell_launch_68659 : public SpellScriptLoader
{
    public:
        spell_launch_68659() : SpellScriptLoader("spell_launch_68659") { }

        class spell_launch_68659_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_launch_68659_SpellScript);

            void HandleHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                WorldLocation const* l_WorldLoc = GetHitDest();
                if (!l_WorldLoc)
                    return;

                Position l_Pos = Position(l_WorldLoc->GetPositionX(), l_WorldLoc->GetPositionY(), l_WorldLoc->GetPositionZ(), l_Caster->GetAngle(l_WorldLoc->GetPositionX(), l_WorldLoc->GetPositionY()));

                if (Vehicle* l_Vehicle = l_Caster->GetVehicleKit())
                {
                    if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                    {
                        if (Player* l_Player = l_Unit->ToPlayer())
                        {
                            if (!l_Pos.IsNearPosition(&Positions::g_BoatRight, 50.0f) && !l_Pos.IsNearPosition(&Positions::g_BoatLeft, 50.0f))
                            {
                                std::string l_Text;
                                if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(eBroadcasts::PlayerCatapult))
                                    l_Text = GetBroadcastTextValue(l_BroadcastText, l_Player->GetSession()->GetSessionDb2Locale(), l_Player->getGender());

                                if (!l_Text.empty())
                                {
                                    WorldPacket l_Packet;
                                    l_Player->BuildPlayerChat(&l_Packet, l_Player->GetGUID(), ChatMsg::CHAT_MSG_SAY, l_Text, 0);
                                    l_Player->GetSession()->SendPacket(&l_Packet);
                                }
                            }
                            else
                            {
                                l_Player->ExitVehicle();
                                l_Player->SetOrientation(l_Caster->GetAngle(l_WorldLoc->GetPositionX(), l_WorldLoc->GetPositionY()));
                                l_Player->CastSpell(l_Player, eSpells::SpellLaunchFall, true);
                                l_Player->GetMotionMaster()->MoveReverseJump(l_Pos, 2000, 20.0f);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_launch_68659_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_launch_68659_SpellScript();
        }
};

/// @Spell : Call Attack Mastiffs - 68682
class spell_call_attack_mastiffs_68682 : public SpellScriptLoader
{
    public:
        spell_call_attack_mastiffs_68682() : SpellScriptLoader("spell_call_attack_mastiffs_68682") { }

        class spell_call_attack_mastiffs_68682_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_call_attack_mastiffs_68682_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_BAD_TARGETS;

                if (Creature* l_DarkRangerThyala = GetClosestCreatureWithEntry(l_Caster, eCreatures::NpcDarkRangerThyala, 40.0f))
                    if (l_DarkRangerThyala->isAlive())
                        return SPELL_CAST_OK;

                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }

            void HandleSummon(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Creature* l_DarkRangerThyala = GetClosestCreatureWithEntry(l_Caster, eCreatures::NpcDarkRangerThyala, 40.0f))
                {
                    for (uint8 l_I = 0; l_I < 9; ++l_I)
                    {
                        if (Creature* l_AttackMastiff = l_Caster->SummonCreature(eCreatures::NpcAttackMastiff, g_AttackMastiffPos[l_I]))
                        {
                            if (l_AttackMastiff->IsAIEnabled)
                            {
                                l_AttackMastiff->AI()->SetGUID(l_DarkRangerThyala->GetGUID());
                                l_AttackMastiff->AI()->DoAction(eActions::ActionAttackMastiff);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_call_attack_mastiffs_68682_SpellScript::CheckCast);
                OnEffectHit += SpellEffectFn(spell_call_attack_mastiffs_68682_SpellScript::HandleSummon, EFFECT_2, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_call_attack_mastiffs_68682_SpellScript();
        }
};

/// @Spell : Rescue Drowning Watchman - 68735
class spell_rescue_drowning_watchman_68735 : public SpellScriptLoader
{
    public:
        spell_rescue_drowning_watchman_68735() : SpellScriptLoader("spell_rescue_drowning_watchman_68735") { }

        class spell_rescue_drowning_watchman_68735_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rescue_drowning_watchman_68735_AuraScript);

            uint64 m_AncientMindbinderGUID = 0;

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcDrowningWatchman, true);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcDrowningWatchman, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rescue_drowning_watchman_68735_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_rescue_drowning_watchman_68735_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rescue_drowning_watchman_68735_AuraScript();
        }
};

/// @Spell : Round Up Horse - 68903
class spell_round_up_horse_68903 : public SpellScriptLoader
{
    public:
        spell_round_up_horse_68903() : SpellScriptLoader("spell_round_up_horse_68903") { }

        class spell_round_up_horse_68903_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_round_up_horse_68903_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Target->GetEntry() != eCreatures::NpcMountainHorse || l_Target->isDead())
                    return SPELL_FAILED_BAD_TARGETS;

                if (Vehicle* l_Vehicle = l_Target->GetVehicleKit())
                    if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                        return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_round_up_horse_68903_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_round_up_horse_68903_SpellScript();
        }
};

/// @Spell: Horn of Tal'doren - 71061
class spell_horn_of_taldoren_71061 : public SpellScriptLoader
{
    public:
        spell_horn_of_taldoren_71061() : SpellScriptLoader("spell_horn_of_taldoren_71061") { }

        class spell_horn_of_taldoren_71061_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_horn_of_taldoren_71061_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Caster->IsNearPosition(&Positions::g_TobiasMistmantle, 100.0f))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Creature* l_TobiasMistmantle = GetClosestCreatureWithEntry(l_Caster, eCreatures::NpcTobiasMistmantle2, 100.0f);
                if (l_TobiasMistmantle)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                return SPELL_CAST_OK;
            }

            void HandleSendEvent(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Creature* l_TobiasMistmantle = l_Caster->SummonCreature(eCreatures::NpcTobiasMistmantle2, Positions::g_TobiasMistmantle, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000))
                    if (l_TobiasMistmantle->IsAIEnabled)
                        l_TobiasMistmantle->AI()->DoAction(eActions::ActionTobiasMistmantle);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_horn_of_taldoren_71061_SpellScript::CheckCast);
                OnEffectHit += SpellEffectFn(spell_horn_of_taldoren_71061_SpellScript::HandleSendEvent, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_horn_of_taldoren_71061_SpellScript();
        }
};

/// @Spell: Iron Bomb - 72247
class spell_iron_bomb_72247 : public SpellScriptLoader
{
    public:
        spell_iron_bomb_72247() : SpellScriptLoader("spell_iron_bomb_72247") { }

        class spell_iron_bomb_72247_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_iron_bomb_72247_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (Creature* l_Target = p_Target->ToCreature())
                    {
                        switch (l_Target->GetEntry())
                        {
                            case eCreatures::NpcForsakenCatapult:
                            case eCreatures::NpcForsakenInvader2:
                            case eCreatures::NpcForsakenPlaguesmith:
                                return false;
                            default:
                                break;
                        }
                    }

                    return true;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_iron_bomb_72247_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_iron_bomb_72247_SpellScript();
        }
};

class playerscript_gilneas : public PlayerScript
{
    public:
        playerscript_gilneas() : PlayerScript("playerscript_gilneas") { }

        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasAura(eSpells::SpellCurseoftheWorgen))
                p_Player->RemoveAurasDueToSpell(eSpells::SpellCurseoftheWorgen);
        }

        void OnPhaseUpdate(Player* p_Player, uint32 p_QuestId, bool p_Accept) override
        {
            switch (p_QuestId)
            {
                case eQuests::TheRebelLordsArsenal:
                {
                    if (!p_Accept)
                    {
                        p_Player->SetTimingForPhase(1409901, 5000);
                    }
                    break;
                }
                case eQuests::LastStand:
                {
                    if (!p_Accept)
                    {
                        p_Player->SetTimingForPhase(1422201, 120000);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestAccept(Player* p_Player, const Quest* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::Sacrifices:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcCrowleysHorse, true);
                    }, 100);
                    break;
                }
                case eQuests::ByBloodandAsh:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcRebelCannon, true);
                    }, 100);
                    break;
                }
                case eQuests::Exodus:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcStagecoachCarriage, true);
                    }, 100);
                    break;
                }
                case eQuests::TheBattleforGilneasCity:
                {
                    p_Player->CastSpell(p_Player, eSpells::SpellCuttoBlack, true);
                    p_Player->KilledMonsterCredit(eCreatures::NpcBattleforGilneasKC);
                    p_Player->TeleportTo(654, Positions::g_BattleGilneas);
                    break;
                }
                case eQuests::SlowingtheInevitable:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcCapturedRidingBat, true);
                    }, 100);
                    break;
                }
                case eQuests::TheyHaveAlliesButSoDoWe:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcGlaiveThrower, true);
                    }, 100);
                    break;
                }
                case eQuests::Endgame:
                {
                    p_Player->KilledMonsterCredit(eCreatures::NpcFinaleCredit);
                    p_Player->PlayerTalkClass->SendCloseGossip();
                    break;
                }
                case eQuests::GrandmasCat:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcChance, true);
                    }, 100);
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestAbandon(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::Sacrifices:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcCrowleysHorse, true);
                    }, 100);
                    break;
                }
                case eQuests::ByBloodandAsh:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcRebelCannon, true);
                    }, 100);
                    break;
                }
                case eQuests::Exodus:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcStagecoachCarriage, true);
                    }, 100);
                    break;
                }
                case eQuests::SlowingtheInevitable:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcCapturedRidingBat, true);
                    }, 100);
                    break;
                }
                case eQuests::TheyHaveAlliesButSoDoWe:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcGlaiveThrower, true);
                    }, 100);
                    break;
                }
                case eQuests::GrandmasCat:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcChance, true);
                    }, 100);
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestComplete(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::ByBloodandAsh:
                {
                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        p_Player->ForceUpdateForObjectWithEntry(eCreatures::NpcRebelCannon, true);
                    }, 100);
                    break;
                }
                case eQuests::PatriarchsBlessing:
                {
                    p_Player->CastSpell(p_Player, eSpells::SpellFuneralEventCamera, true);
                    break;
                }
                default:
                    break;
            }
        }

        void OnMovieComplete(Player* p_Player, uint32 p_MovieId) override
        {
            if (p_MovieId == eMovies::GilneasCathedral)
            {
                p_Player->CastSpell(p_Player, eSpells::SpellTwoForms, true);
                p_Player->CastSpell(p_Player, eSpells::SpellCurseoftheWorgen, true);
                p_Player->SwitchToWorgenForm();

                p_Player->AddDelayedEvent([p_Player]() -> void
                {
                    if (Creature* l_KrennanAranas = p_Player->SummonCreature(eCreatures::NpcKrennanAranas2, Positions::g_KrennanAranas2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 43000, 0, p_Player->GetGUID()))
                        if (l_KrennanAranas->IsAIEnabled)
                            l_KrennanAranas->AI()->DoAction(eActions::ActionKrennanAranas);

                    if (Creature* l_LordGodfrey = p_Player->SummonCreature(eCreatures::NpcLordGodfrey, Positions::g_LordGodfrey2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 43000, 0, p_Player->GetGUID()))
                        if (l_LordGodfrey->IsAIEnabled)
                            l_LordGodfrey->AI()->DoAction(eActions::ActionLordGodfrey);

                    if (Creature* l_KingGennGreymane = p_Player->SummonCreature(eCreatures::NpcKingGennGreymane, Positions::g_GennGreymane2, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 43000, 0, p_Player->GetGUID()))
                        if (l_KingGennGreymane->IsAIEnabled)
                            l_KingGennGreymane->AI()->DoAction(eActions::ActionKingGennGreymane);
                }, 4000);
            }
        }

        void OnItemLooted(Player* p_Player, Item* p_Item) override
        {
            switch (p_Item->GetEntry())
            {
                case eItems::KorothsBanner:
                {
                    if (Creature* l_KoroththeHillbreaker = GetClosestCreatureWithEntry(p_Player, eCreatures::NpcKorothHillbreaker, 100.0f))
                    {
                        if (l_KoroththeHillbreaker->IsAIEnabled)
                        {
                            l_KoroththeHillbreaker->AI()->SetGUID(p_Player->GetGUID());
                            l_KoroththeHillbreaker->AI()->DoAction(eActions::ActionKoroththeHillbreaker);
                        }
                    }

                    break;
                }
                default:
                    break;
            }
        }

        void OnTeleport(Player* p_Player, SpellInfo const* /*p_Spell*/, bool) override
        {
            if (!(p_Player->getRace() == Races::RACE_WORGEN))
                return;

            if (p_Player->GetMapId() != eMaps::MapGilneas)
            {
                if (p_Player->m_homebindMapId == eMaps::MapGilneas)
                {
                    /// Set Hearthstone to Stormwind
                    WorldLocation l_NewHomeBind = WorldLocation(0, -8831.893f, 624.321f, 94.082f, 0.675444f);
                    p_Player->SetHomebind(l_NewHomeBind, 5148);
                }
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_gilneas()
{
    /// Creatures
    new npc_prince_liam_greymane_34850();
    new npc_prince_liam_greymane_34913();
    new npc_gilneas_city_guard_34916();
    new npc_rampaging_worgen_34884();
    new npc_frightened_citizen_35836();
    new npc_lord_godfrey_35115();
    new npc_bloodfang_worgen_35118();
    new npc_gilnean_royal_guard_35232();
    new npc_sergeant_cleese_35839();
    new npc_myriam_spellwaker_35872();
    new npc_sean_dempsey_35081();
    new npc_lord_darius_crowley_35077();
    new npc_worgen_runt_35188();
    new npc_worgen_alpha_35170();
    new npc_josiah_avery_35369();
    new npc_lorna_crowley_35378();
    new npc_gilnean_mastiff_35631();
    new npc_king_genn_greymane_35550();
    new npc_king_greymanes_horse_35905();
    new npc_krennan_aranas_35907();
    new npc_lord_godfrey_35906();
    new npc_commandeered_cannon_35914();
    new npc_afflicted_gilnean_50471();
    new npc_gilneas_city_guard_50474();
    new npc_king_genn_greymane_35911();
    new npc_crowleys_horse_44427();
    new npc_crowleys_horse_35231();
    new npc_bloodfang_stalker_35229();
    new npc_northgate_rebel_36057();
    new npc_rebel_cannon_35317();
    new npc_gilneas_passive_ai();
    new npc_frenzied_stalker_35627();
    new npc_northgate_rebel_41015();
    new npc_lord_darius_crowley_35566();
    new npc_krennan_aranas_36331();
    new npc_lord_godfrey_36330();
    new npc_king_genn_greymane_36332();
    new npc_gilnean_crow_50260();
    new npc_krennan_aranas_36132();
    new npc_slain_watchman_36205();
    new npc_forsaken_invader_34511();
    new npc_forsaken_footsoldier_36236();
    new npc_duskhaven_watchman_36211();
    new npc_horrid_abomination_36231();
    new npc_cynthia_36287();
    new npc_james_36289();
    new npc_ashley_36288();
    new npc_forsaken_catapult_36283();
    new npc_attack_mastiff_36405();
    new npc_dark_ranger_thyala_36312();
    new npc_lord_godfrey_36290();
    new npc_drowning_watchman_36440();
    new npc_mountain_horse_36540();
    new npc_mountain_horse_36555();
    new npc_forsaken_castaway_36488();
    new npc_gilneas_brother_hayward();
    new npc_chance_36459();
    new npc_lucius_the_cruel_36461();
    new npc_grandma_wahl_36458();
    new npc_gwen_armstead_36452();
    new npc_swift_mountain_horse_36741();
    new npc_stagecoach_harness_38755();
    new npc_stagecoach_carriage_44928();
    new npc_stagecoach_harness_43336();
    new npc_stagecoach_carriage_43337();
    new npc_crash_survivor_37067();
    new npc_swamp_crocolisk_37078();
    new npc_koroth_the_hillbreaker_36294();
    new npc_prince_liam_greymane_37065();
    new npc_trigger_dark_scout_35374();
    new npc_dark_scout_37953();
    new npc_veteran_dark_ranger_38022();
    new npc_taldoren_tracker_38027();
    new npc_tobias_mistmantle_38029();
    new npc_lord_darius_crowley_37195();
    new npc_king_genn_greymane_37876();
    new npc_lord_hewell_38764();
    new npc_stout_mountain_horse_38765();
    new npc_lorna_crowley_38611();
    new npc_tobias_mistmantle_38507();
    new npc_lady_sylvanas_windrunner_38530();
    new npc_high_executor_crenshaw_38537();
    new npc_general_warhowl_38533();
    new npc_forsaken_catapult_38287();
    new npc_forsaken_invader_38363();
    new npc_slime_spigot_bunny_38365();
    new npc_captured_riding_bat_38615();
    new npc_captured_riding_bat_38540();
    new npc_gilneas_funeral_camera_51083();
    new npc_riding_war_wolf_37939();
    new npc_orcish_war_machine_37921();
    new npc_orc_raider_37916();
    new npc_towering_ancient_37914();
    new npc_lorna_crowley_43727();
    new npc_gilnean_survivor_38781();

    /// Gameobjects
    new gob_merchant_square_door_195327();
    new go_dustfall_196465();
    new gob_well_of_fury_201950();
    new gob_well_of_tranquility_201951();
    new gob_well_of_balance_201952();
    new gob_ball_and_chain_201775();

    /// Spells
    new spell_attack_lurker_67805();
    new spell_rescue_krennan_68219();
    new spell_throw_torch_67063();
    new spell_toss_keg_69094();
    new spell_launch_68659();
    new spell_call_attack_mastiffs_68682();
    new spell_rescue_drowning_watchman_68735();
    new spell_round_up_horse_68903();
    new spell_horn_of_taldoren_71061();
    new spell_iron_bomb_72247();

    /// Playerscript
    new playerscript_gilneas();
}
#endif
