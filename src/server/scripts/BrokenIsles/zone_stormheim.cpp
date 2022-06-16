////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"

/// Eyir - 93628
class npc_stormheim_eyir : public CreatureScript
{
    public:
        npc_stormheim_eyir() : CreatureScript("npc_stormheim_eyir") { }

        enum eQuest
        {
            EyirsForgiveness = 45527
        };

        enum eConv
        {
            ConversationQuest = 4276
        };

        enum eCreatures
        {
            CreatureSigryn  = 116568,
            CreatureCredit  = 117494
        };

        struct npc_stormheim_eyirAI : public ScriptedAI
        {
            npc_stormheim_eyirAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::set<uint64> m_ProcessingPlayers;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                Player* l_Player = p_Who->ToPlayer();

                /// Don't have quest, or already completed
                if (!l_Player->HasQuest(eQuest::EyirsForgiveness) || l_Player->GetQuestStatus(eQuest::EyirsForgiveness) == QuestStatus::QUEST_STATUS_COMPLETE || l_Player->GetQuestStatus(eQuest::EyirsForgiveness) == QuestStatus::QUEST_STATUS_REWARDED)
                    return;

                if (m_ProcessingPlayers.find(l_Player->GetGUID()) != m_ProcessingPlayers.end())
                    return;

                m_ProcessingPlayers.insert(l_Player->GetGUID());

                me->SummonCreature(eCreatures::CreatureSigryn, { 2542.423f, 1276.939f, 47.58974f, 4.738419f }, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, p_Who->GetGUID());

                Conversation* l_Conversation = new Conversation;
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConv::ConversationQuest, p_Who, nullptr, *p_Who))
                    delete l_Conversation;

                uint64 l_Guid = l_Player->GetGUID();
                AddTimedDelayedOperation(92 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                {
                    if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                        l_Player->KilledMonsterCredit(eCreatures::CreatureCredit);

                    m_ProcessingPlayers.erase(l_Guid);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_stormheim_eyirAI(p_Creature);
        }
};

/// Sigryn - 116568
class npc_stormheim_sigryn : public CreatureScript
{
    public:
        npc_stormheim_sigryn() : CreatureScript("npc_stormheim_sigryn") { }

        enum eQuest
        {
            ACommonEnnemy = 45534
        };

        bool OnQuestAccept(Player* /*p_Player*/, Creature* p_Creature, Quest const* p_Quest) override
        {
            if (p_Quest->GetQuestId() != eQuest::ACommonEnnemy)
                return false;

            p_Creature->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);
            return true;
        }
};


void ChangeHidingForPlayer(WorldObject* p_Object, uint64 p_Guid, bool p_Apply)
{
    if (p_Apply)
        p_Object->AddToHideList(p_Guid);
    else
        p_Object->RemoveFromHideList(p_Guid);

    if (Player* l_Player = ObjectAccessor::GetPlayer(*p_Object, p_Guid))
        l_Player->UpdateObjectVisibility(true);
}

/// Nathanos Blightcaller - 96683
class npc_nathanos_blightcaller_96683 : public CreatureScript
{
    public:
        npc_nathanos_blightcaller_96683() : CreatureScript("npc_nathanos_blightcaller_96683") { }

        struct npc_nathanos_blightcaller_96683AI : public ScriptedAI
        {
            npc_nathanos_blightcaller_96683AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveToPortalEnd = 1
            };

            enum eSpells
            {
                Spell_TeleportVisual = 164233
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                uint64 l_PlayerGuid = p_Summoner->GetGUID();

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                    {
                        me->SetFacingToObject(l_Player);
                        Talk(0, l_PlayerGuid);
                    }
                }, 27000);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToPortalEnd, -831.259f, 4654.42f, 749.621f);
                }, 34000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveToPortalEnd)
                {
                    DoCast(eSpells::Spell_TeleportVisual, true);
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nathanos_blightcaller_96683AI(p_Creature);
        }
};

/// Lady Sylvanas Windrunner - 96686
class npc_lady_sylvanas_windrunner_96686 : public CreatureScript
{
    public:
        npc_lady_sylvanas_windrunner_96686() : CreatureScript("npc_lady_sylvanas_windrunner_96686") { }

        struct npc_lady_sylvanas_windrunner_96686AI : public ScriptedAI
        {
            npc_lady_sylvanas_windrunner_96686AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_SylvanasToCatapult[18] =
            {
                { 1494.66f,  -5210.655f, 11.06805f },
                { 1493.793f, -5212.724f, 10.93926f },
                { 1491.662f, -5213.501f, 10.78744f },
                { 1490.387f, -5213.289f, 10.2681f  },
                { 1487.387f, -5213.539f, 7.018101f },
                { 1486.563f, -5213.814f, 6.097775f },
                { 1485.275f, -5212.83f,  5.880461f },
                { 1484.275f, -5212.08f,  5.880461f },
                { 1480.669f, -5210.396f, 5.319456f },
                { 1478.522f, -5205.182f, 5.410386f },
                { 1476.705f, -5202.289f, 5.485039f },
                { 1475.493f, -5201.924f, 5.976235f },
                { 1474.993f, -5201.924f, 6.476235f },
                { 1471.993f, -5202.174f, 9.976235f },
                { 1470.741f, -5202.454f, 10.52945f },
                { 1469.891f, -5202.857f, 10.58721f },
                { 1468.891f, -5203.107f, 10.83721f },
                { 1467.073f, -5204.042f, 10.82795f }
            };

            G3D::Vector3 const Path_SylvanasToRangers[9] =
            {
                { 1476.451f, -5201.978f, 5.527115f },
                { 1468.601f, -5203.509f, 10.92883f },
                { 1469.851f, -5203.259f, 10.92883f },
                { 1471.351f, -5203.009f, 10.67883f },
                { 1475.351f, -5202.259f, 6.178831f },
                { 1476.451f, -5201.978f, 5.527115f },
                { 1479.617f, -5204.836f, 5.742015f },
                { 1481.675f, -5207.27f,  5.319456f },
                { 1485.43f,  -5211.277f, 5.319456f }
            };

            G3D::Vector3 const Path_SylvanasToTattersail[11] =
            {
                { 1486.112f, -5212.917f, 6.147043f },
                { 1486.362f, -5213.667f, 5.897043f },
                { 1487.362f, -5213.667f, 7.147043f },
                { 1490.612f, -5213.167f, 10.14704f },
                { 1491.23f,  -5213.457f, 10.79592f },
                { 1493.094f, -5212.93f,  11.0104f  },
                { 1495.844f, -5212.68f,  11.2604f  },
                { 1496.958f, -5212.403f, 11.22488f },
                { 1497.331f, -5212.518f, 11.34519f },
                { 1498.331f, -5213.268f, 11.59519f },
                { 1499.199f, -5214.042f, 11.54164f }
            };

            enum ePoints
            {
                Point_PathToCatapultEnd   = 1,
                Point_PathToRangersEnd    = 2,
                Point_PathToTattersailEnd = 3
            };

            enum eObjectives
            {
                Obj_InspectCatapult       = 279849,
                Obj_SaluteForsakenRangers = 279850
            };

            enum eKillcredits
            {
                Killcredit_SpeakWithDreadCaptainTattersail = 96689
            };

            enum eEvents
            {
                Event_StartPathToCatapult                  = 1,
                Event_StartPathToRangers                   = 2,
                Event_SayLineAfterRangersPath              = 3,
                Event_SayLineAfterSalute                   = 4,
                Event_StartPathToTattersail                = 5,
                Event_SayLineMiddlePath                    = 6,
                Event_StartTattersailScript                = 7,
                Event_SetFacingToTattersail                = 8,
                Event_SayLastLine                          = 9
            };

            enum eSpells
            {
                Spell_ConversationSylvanasWalkToCatapult        = 220271,
                Spell_ConversationSylvanasWalkToForsakenRangers = 220290,
                Spell_Spyglass                                  = 220252,
                Spell_DummyAura                                 = 94562,
                Spell_QueueForMawOfNashalHorde                  = 228560
            };

            enum eNpcs
            {
                Npc_DreadCaptainTattersail = 96689
            };

            enum eQuests
            {
                Quest_MakingTheRounds    = 39698,
                Quest_TheSplinteredFleet = 39801
            };

            ObjectGuid m_SummonerGuid;
            ObjectGuid m_TattersailGuid;
            EventMap m_Events;
            Position const m_DreadCaptainTattersailSpawnPos = { 1502.566f, -5212.933f, 12.00079f, 4.676727f };
            bool m_PathToRangersStarted    = false;
            bool m_PathToTattersailStarted = false;
            bool m_PathToCatapultEnded     = false;
            bool m_PathToRangersEnded      = false;

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::Quest_MakingTheRounds:
                    {
                        p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
                        break;
                    }
                    case eQuests::Quest_TheSplinteredFleet:
                    {
                        p_Player->CastSpell(p_Player, eSpells::Spell_QueueForMawOfNashalHorde, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (Creature* l_Creature = p_Summoner->SummonCreature(eNpcs::Npc_DreadCaptainTattersail, m_DreadCaptainTattersailSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Summoner->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_TattersailGuid = l_Creature->GetGUID();
                }

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_SummonerGuid = p_Summoner->GetGUID();
                m_Events.ScheduleEvent(eEvents::Event_StartPathToCatapult, 2000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                switch (p_PointId)
                {
                    case ePoints::Point_PathToCatapultEnd:
                    {
                        Talk(0, m_SummonerGuid);
                        m_PathToCatapultEnded = true;
                        break;
                    }
                    case ePoints::Point_PathToRangersEnd:
                    {
                        me->SetFacingTo(2.346901f);
                        m_Events.ScheduleEvent(eEvents::Event_SayLineAfterRangersPath, 2000);
                        m_PathToRangersEnded = true;
                        break;
                    }
                    case ePoints::Point_PathToTattersailEnd:
                    {
                        me->SetFacingTo(4.685642f);
                        Talk(4, m_SummonerGuid);
                        m_Events.ScheduleEvent(eEvents::Event_StartTattersailScript, 6000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isSummon())
                    return;

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_MakingTheRounds))
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_TattersailGuid))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }

                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_InspectCatapult) == 1 && !m_PathToRangersStarted && m_PathToCatapultEnded)
                {
                    m_Events.ScheduleEvent(eEvents::Event_StartPathToRangers, 3000);
                    m_PathToRangersStarted = true;
                }

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SaluteForsakenRangers) == 1 && !m_PathToTattersailStarted && m_PathToRangersEnded)
                {
                    m_Events.ScheduleEvent(eEvents::Event_SayLineAfterSalute, 2000);
                    m_PathToTattersailStarted = true;
                }

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_StartPathToCatapult:
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_ConversationSylvanasWalkToCatapult, true);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToCatapultEnd, Path_SylvanasToCatapult, 18, true);
                        break;
                    }
                    case eEvents::Event_StartPathToRangers:
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_ConversationSylvanasWalkToForsakenRangers, true);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToRangersEnd, Path_SylvanasToRangers, 9, true);
                        break;
                    }
                    case eEvents::Event_SayLineAfterRangersPath:
                    {
                        Talk(1, m_SummonerGuid);
                        l_Player->CastSpell(l_Player, eSpells::Spell_DummyAura, true);
                        break;
                    }
                    case eEvents::Event_SayLineAfterSalute:
                    {
                        Talk(2, m_SummonerGuid);
                        m_Events.ScheduleEvent(eEvents::Event_StartPathToTattersail, 7000);
                        break;
                    }
                    case eEvents::Event_StartPathToTattersail:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToTattersailEnd, Path_SylvanasToTattersail, 11, true);
                        m_Events.ScheduleEvent(eEvents::Event_SayLineMiddlePath, 6000);
                        break;
                    }
                    case eEvents::Event_SayLineMiddlePath:
                    {
                        Talk(3, m_SummonerGuid);
                        break;
                    }
                    case eEvents::Event_StartTattersailScript:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_TattersailGuid))
                        {
                            uint64 l_PlayerGuid = m_SummonerGuid;

                            l_Creature->RemoveAura(eSpells::Spell_Spyglass);

                            l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                            {
                                if (Player* l_Pl = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    l_Creature->AI()->Talk(0, l_PlayerGuid);

                                l_Creature->SetFacingTo(3.463912f);
                            }, 1000);

                            l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                            {
                                if (Player* l_Pl = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    l_Creature->AI()->Talk(1, l_PlayerGuid);
                            }, 7000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Spyglass, true);
                                l_Creature->SetFacingTo(l_Creature->GetHomePosition().m_orientation);
                            }, 13000);
                        }

                        m_Events.ScheduleEvent(eEvents::Event_SetFacingToTattersail, 2000);
                        break;
                    }
                    case eEvents::Event_SetFacingToTattersail:
                    {
                        me->SetFacingTo(0.4619455f);
                        m_Events.ScheduleEvent(eEvents::Event_SayLastLine, 12000);
                        break;
                    }
                    case eEvents::Event_SayLastLine:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_TattersailGuid))
                        {
                            l_Creature->DespawnOrUnsummon();
                        }

                        me->SetFacingTo(4.703095f);
                        Talk(5, m_SummonerGuid);
                        me->DespawnOrUnsummon();
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SpeakWithDreadCaptainTattersail);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_sylvanas_windrunner_96686AI(p_Creature);
        }
};

/// Dread-Rider Cullen - 91473
class npc_dread_rider_cullen_91473 : public CreatureScript
{
    public:
        npc_dread_rider_cullen_91473() : CreatureScript("npc_dread_rider_cullen_91473") { }

        struct npc_dread_rider_cullen_91473AI : public ScriptedAI
        {
            npc_dread_rider_cullen_91473AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_CullenFromCamp[3] =
            {
                { 4100.3f,   2925.78f,  38.21133f },
                { 4099.716f, 2906.252f, 58.83791f },
                { 4058.707f, 2867.462f, 83.87518f }
            };

            enum ePoints
            {
                Point_MoveFromCampEnd = 1
            };

            enum eQuests
            {
                Quest_TheSplinteredFleet = 39801,
                Quest_TheRangerLord      = 38332
            };

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Player->IsQuestRewarded(eQuests::Quest_TheSplinteredFleet))
                {
                    p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
                }
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                if (p_Player->HasQuest(eQuests::Quest_TheRangerLord))
                {
                    p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);

                me->AddDelayedEvent([this]() -> void
                {
                    me->Mount(60307);
                }, 2000);

                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_CullenFromCamp[3] =
                    {
                        { 4100.3f,   2925.78f,  38.21133f },
                        { 4099.716f, 2906.252f, 58.83791f },
                        { 4058.707f, 2867.462f, 83.87518f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_MoveFromCampEnd, Path_CullenFromCamp, 3);
                }, 4000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveFromCampEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dread_rider_cullen_91473AI(p_Creature);
        }
};

/// Nathanos Blightcaller - 91158
class npc_nathanos_blightcaller_91158 : public CreatureScript
{
    public:
        npc_nathanos_blightcaller_91158() : CreatureScript("npc_nathanos_blightcaller_91158") { }

        struct npc_nathanos_blightcaller_91158AI : public ScriptedAI
        {
            npc_nathanos_blightcaller_91158AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_NathanosAndRangersMoveEnd            = 1,
                Point_RunToPlagueTippedArrowEnd            = 2,
                Point_WalkToPlagueTippedArrowEnd           = 3,
                Point_NathanosMoveFromPlagueTippedArrowEnd = 4,
                Point_NathanosMoveToThanEnd                = 5,
                Point_NathanosMoveAfterFightWithThanEnd    = 6
            };

            enum eQuests
            {
                Quest_WrathOfTheBlightcaller = 38361,
                Quest_TheWindrunnersFate     = 38360,
                Quest_AGrimTrophy            = 38362
            };

            enum eActions
            {
                Action_NathanosMoveToPlagueTippedArrow = 1,
                Action_LeaveWithRangers                = 2,
                Action_MoveFromPlagueTippedArrow       = 3,
                Action_NathanosFightWithThane          = 4,
                Action_NathanosFightWithThanEnd        = 5
            };

            enum eNpcs
            {
                Npc_ForsakenRanger = 91950,
                Npc_Blighthound    = 91536,
                Npc_ThaneWildsky   = 91571,
                Npc_Huginn         = 97305
            };

            enum eKillcredits
            {
                Killcredit_NathanosAppeared = 3836000
            };

            enum eSpells
            {
                Spell_Shoot = 16100
            };

            enum eObjectives
            {
                Obj_NathanosDispleasedLineHeard = 3836201
            };

            ObjectGuid m_SummonerGuid;
            Position const m_FirstBlighthoundSpawnPos           = { 4337.47f,  2861.464f, 1.71281f,   2.240887f };
            Position const m_SecondBlighthoundSpawnPos          = { 4325.864f, 2839.835f, 1.054809f,  1.352896f };
            Position const m_FirstRangerSpawnPos                = { 4354.252f, 2841.337f, 1.298271f,  1.72435f  };
            Position const m_SecondRangerSpawnPos               = { 4343.755f, 2821.549f, 1.803666f,  6.0014f   };
            Position const m_ThirdRangerSpawnPos                = { 4329.898f, 2864.551f, 1.036707f,  2.918916f };
            Position const m_FourthRangerSpawnPos               = { 4318.61f,  2844.363f, 0.7468792f, 2.734517f };
            Position const m_NathanosRunToPlagueTippedArrowPos  = { 4473.04f,  2475.503f, 1.854343f };
            Position const m_NathanosWalkToPlagueTippedArrowPos = { 4473.658f, 2473.425f, 1.851119f };
            Position const m_NathanosMoveToThanPos              = { 4353.29f,  2600.323f, 39.22023f };
            bool m_PathToThanEnd = false;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player || me->isSummon())
                    return;

                if (me->GetExactDist2d(l_Player) < 40.0f && l_Player->HasQuest(eQuests::Quest_AGrimTrophy) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_NathanosDispleasedLineHeard) == 0)
                {
                    Talk(4, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_NathanosDispleasedLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eQuests::Quest_WrathOfTheBlightcaller:
                    case eQuests::Quest_TheWindrunnersFate:
                    {
                        if (p_Player->HasQuest(eQuests::Quest_WrathOfTheBlightcaller) && p_Player->HasQuest(eQuests::Quest_TheWindrunnersFate))
                        {
                            if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                            {
                                l_Creature->AI()->DoAction(eActions::Action_LeaveWithRangers);
                            }
                        }

                        break;
                    }
                    case eQuests::Quest_AGrimTrophy:
                    {
                        if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->AI()->DoAction(eActions::Action_MoveFromPlagueTippedArrow);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
            {
                if (p_Quest->GetQuestId() == eQuests::Quest_AGrimTrophy)
                {
                    if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_Huginn, 20.0f, true))
                    {
                        l_Creature->AI()->Talk(0, p_Player->GetGUID());
                    }
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_LeaveWithRangers:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                            me->AddDelayedEvent([this]() -> void
                            {
                                me->SetFacingTo(3.333579f);
                                me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                            }, 2000);

                            me->AddDelayedEvent([this]() -> void
                            {
                                G3D::Vector3 const Path_Nathanos[12] =
                                {
                                    { 4337.731f, 2833.445f, 1.907475f    },
                                    { 4324.481f, 2827.945f, 0.9074755f   },
                                    { 4316.231f, 2822.695f, -0.09252453f },
                                    { 4305.731f, 2811.695f, -0.09252453f },
                                    { 4292.231f, 2795.695f, 0.6574755f   },
                                    { 4285.981f, 2788.695f, 1.407475f    },
                                    { 4285.481f, 2781.195f, 0.9074755f   },
                                    { 4286.481f, 2772.695f, 0.9074755f   },
                                    { 4282.731f, 2764.195f, 1.657475f    },
                                    { 4275.481f, 2757.445f, 4.157475f    },
                                    { 4269.231f, 2750.445f, 7.657475f    },
                                    { 4268.101f, 2740.8f,   13.39977f    }
                                };

                                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Nathanos, 12, false);
                            }, 4000);

                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_Blighthound, m_FirstBlighthoundSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Blighthound[17] =
                                    {
                                        { 4334.024f, 2849.293f, 1.42337f     },
                                        { 4333.524f, 2839.043f, 1.67337f     },
                                        { 4328.024f, 2828.543f, 1.17337f     },
                                        { 4323.274f, 2824.293f, 0.9233704f   },
                                        { 4318.024f, 2820.293f, -0.07662964f },
                                        { 4311.274f, 2815.293f, -0.3266296f  },
                                        { 4301.774f, 2804.543f, -0.07662964f },
                                        { 4293.024f, 2793.293f, 0.9233704f   },
                                        { 4290.524f, 2788.793f, 1.67337f     },
                                        { 4289.024f, 2782.043f, 1.17337f     },
                                        { 4288.524f, 2773.793f, 0.9233704f   },
                                        { 4286.274f, 2765.043f, 1.17337f     },
                                        { 4282.524f, 2760.043f, 2.17337f     },
                                        { 4278.524f, 2755.793f, 4.42337f     },
                                        { 4274.274f, 2752.043f, 6.67337f     },
                                        { 4272.274f, 2747.293f, 9.42337f     },
                                        { 4272.078f, 2744.122f, 11.13393f    }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Blighthound, 17, false);
                                }, 2000);
                            }

                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_Blighthound, m_SecondBlighthoundSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Blighthound[18] =
                                    {
                                        { 4322.281f, 2832.028f, 0.9379611f  },
                                        { 4318.781f, 2828.278f, 0.4379611f  },
                                        { 4316.781f, 2825.528f, -0.0620389f },
                                        { 4311.781f, 2820.778f, -0.3120389f },
                                        { 4306.531f, 2815.528f, -0.3120389f },
                                        { 4299.031f, 2807.528f, -0.0620389f },
                                        { 4293.781f, 2800.528f, 0.1879611f  },
                                        { 4288.031f, 2794.528f, 0.9379611f  },
                                        { 4284.031f, 2790.278f, 1.187961f   },
                                        { 4282.531f, 2785.278f, 1.187961f   },
                                        { 4283.281f, 2777.778f, 0.9379611f  },
                                        { 4283.031f, 2771.528f, 1.187961f   },
                                        { 4281.781f, 2767.778f, 1.437961f   },
                                        { 4278.031f, 2762.528f, 2.437961f   },
                                        { 4273.031f, 2757.278f, 4.687961f   },
                                        { 4268.281f, 2752.778f, 6.687961f   },
                                        { 4266.031f, 2748.528f, 8.437962f   },
                                        { 4266.197f, 2743.721f, 11.82111f   }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Blighthound, 18, false);
                                }, 4000);
                            }

                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ForsakenRanger, m_FirstRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Ranger[14] =
                                    {
                                        { 4350.848f, 2840.76f,  1.876581f   },
                                        { 4347.598f, 2839.51f,  2.126581f   },
                                        { 4341.598f, 2836.51f,  2.126581f   },
                                        { 4332.848f, 2832.01f,  1.626581f   },
                                        { 4327.098f, 2826.26f,  1.126581f   },
                                        { 4323.348f, 2820.51f,  0.3765812f  },
                                        { 4313.848f, 2810.51f,  -0.1234188f },
                                        { 4305.598f, 2802.01f,  -0.1234188f },
                                        { 4300.098f, 2794.26f,  0.1265812f  },
                                        { 4295.348f, 2787.51f,  1.126581f   },
                                        { 4291.098f, 2778.26f,  0.8765812f  },
                                        { 4289.098f, 2767.26f,  0.8765812f  },
                                        { 4291.848f, 2756.51f,  1.626581f   },
                                        { 4297.444f, 2743.684f, 3.454892f   }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Ranger, 14, false);
                                }, 4000);
                            }

                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ForsakenRanger, m_SecondRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Ranger[10] =
                                    {
                                        { 4339.417f, 2822.822f, 1.890947f   },
                                        { 4333.167f, 2822.322f, 1.390947f   },
                                        { 4324.667f, 2818.072f, 0.6409469f  },
                                        { 4316.417f, 2810.572f, -0.1090531f },
                                        { 4307.917f, 2802.322f, -0.1090531f },
                                        { 4298.917f, 2789.822f, 0.3909469f  },
                                        { 4294.167f, 2780.322f, 0.6409469f  },
                                        { 4290.167f, 2766.572f, 0.8909469f  },
                                        { 4295.917f, 2753.072f, 1.640947f   },
                                        { 4300.579f, 2743.595f, 2.478227f   }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Ranger, 10, false);
                                }, 5000);
                            }

                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ForsakenRanger, m_ThirdRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Ranger[14] =
                                    {
                                        { 4329.182f, 2855.541f, 1.354586f   },
                                        { 4325.432f, 2846.291f, 1.354586f   },
                                        { 4318.182f, 2836.541f, 0.8545862f  },
                                        { 4311.432f, 2826.541f, -0.1454138f },
                                        { 4303.682f, 2818.291f, -0.1454138f },
                                        { 4293.182f, 2805.541f, 0.1045862f  },
                                        { 4286.682f, 2798.041f, 0.3545862f  },
                                        { 4282.182f, 2791.291f, 1.354586f   },
                                        { 4281.682f, 2786.291f, 1.354586f   },
                                        { 4281.432f, 2780.791f, 0.8545862f  },
                                        { 4278.932f, 2775.041f, 1.104586f   },
                                        { 4273.682f, 2770.541f, 1.604586f   },
                                        { 4265.682f, 2767.041f, 1.854586f   },
                                        { 4258.465f, 2763.531f, 2.172465f   }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Ranger, 14, false);
                                }, 2000);
                            }

                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ForsakenRanger, m_FourthRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Ranger[11] =
                                    {
                                        { 4313.367f, 2839.325f, 0.158918f    },
                                        { 4309.367f, 2834.325f, -0.09108198f },
                                        { 4302.367f, 2826.075f, -0.09108198f },
                                        { 4289.617f, 2809.075f, 0.158918f    },
                                        { 4281.617f, 2797.825f, 0.158918f    },
                                        { 4279.617f, 2792.575f, 1.158918f    },
                                        { 4279.867f, 2788.075f, 1.408918f    },
                                        { 4278.867f, 2778.575f, 0.908918f    },
                                        { 4271.117f, 2771.825f, 1.158918f    },
                                        { 4265.867f, 2769.825f, 1.408918f    },
                                        { 4256.124f, 2764.786f, 1.570957f    }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosAndRangersMoveEnd, Path_Ranger, 11, false);
                                }, 7000);
                            }
                        }

                        break;
                    }
                    case eActions::Action_NathanosMoveToPlagueTippedArrow:
                    {
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        Talk(0, m_SummonerGuid);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetWalk(false);
                            me->GetMotionMaster()->MovePoint(ePoints::Point_RunToPlagueTippedArrowEnd, m_NathanosRunToPlagueTippedArrowPos);
                        }, 1000);

                        break;
                    }
                    case eActions::Action_MoveFromPlagueTippedArrow:
                    {
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                        me->AddDelayedEvent([this]() -> void
                        {
                            G3D::Vector3 const Path_Nathanos[6] =
                            {
                                { 4473.121f, 2479.924f, 2.393524f },
                                { 4473.871f, 2496.174f, 2.393524f },
                                { 4469.871f, 2516.424f, 2.143524f },
                                { 4461.621f, 2529.174f, 3.643524f },
                                { 4446.621f, 2545.924f, 11.64352f },
                                { 4425.584f, 2554.924f, 24.84784f }
                            };

                            me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosMoveFromPlagueTippedArrowEnd, Path_Nathanos, 6, false);
                        }, 2000);

                        break;
                    }
                    case eActions::Action_NathanosFightWithThane:
                    {
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_NathanosMoveToThanEnd, m_NathanosMoveToThanPos);
                        Talk(2);
                        break;
                    }
                    case eActions::Action_NathanosFightWithThanEnd:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            Talk(3);
                        }, 2000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            G3D::Vector3 const Path_Nathanos[26] =
                            {
                                { 4326.733f, 2619.13f,  40.22533f },
                                { 4323.983f, 2620.38f,  40.97533f },
                                { 4321.233f, 2621.88f,  41.72533f },
                                { 4319.483f, 2622.63f,  42.47533f },
                                { 4317.604f, 2623.512f, 43.23042f },
                                { 4315.027f, 2624.274f, 44.10533f },
                                { 4313.027f, 2625.024f, 45.10533f },
                                { 4310.277f, 2625.774f, 45.85533f },
                                { 4307.277f, 2626.774f, 46.60533f },
                                { 4305.527f, 2627.274f, 47.35533f },
                                { 4303.064f, 2627.947f, 48.00533f },
                                { 4300.854f, 2630.08f,  49.07851f },
                                { 4297.604f, 2632.08f,  49.57851f },
                                { 4292.866f, 2635.625f, 49.82532f },
                                { 4288.114f, 2635.196f, 50.74677f },
                                { 4282.114f, 2634.696f, 51.24677f },
                                { 4280.583f, 2634.492f, 51.39612f },
                                { 4278.41f,  2638.026f, 50.53166f },
                                { 4276.16f,  2641.276f, 49.78166f },
                                { 4273.41f,  2645.526f, 49.03166f },
                                { 4267.91f,  2653.526f, 48.53166f },
                                { 4264.91f,  2657.526f, 47.78166f },
                                { 4262.16f,  2661.776f, 47.28166f },
                                { 4261.66f,  2662.526f, 46.53166f },
                                { 4260.41f,  2664.276f, 45.78166f },
                                { 4259.823f, 2664.783f, 45.18911f }
                            };

                            me->CombatStop(true);
                            me->SetReactState(ReactStates::REACT_PASSIVE);
                            me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_NathanosMoveAfterFightWithThanEnd, Path_Nathanos, 26, false);
                        }, 5000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_NathanosAndRangersMoveEnd:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    case ePoints::Point_RunToPlagueTippedArrowEnd:
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_WalkToPlagueTippedArrowEnd, m_NathanosWalkToPlagueTippedArrowPos);
                        break;
                    }
                    case ePoints::Point_WalkToPlagueTippedArrowEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            me->SetFacingTo(0.3432975f);
                            Talk(1, m_SummonerGuid);
                            l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_NathanosAppeared);
                        }

                        me->DespawnOrUnsummon();
                        break;
                    }
                    case ePoints::Point_NathanosMoveFromPlagueTippedArrowEnd:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    case ePoints::Point_NathanosMoveToThanEnd:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_ThaneWildsky, 50.0f, true))
                        {
                            AttackStart(l_Creature, false);
                        }

                        SetCombatMovement(false);
                        m_PathToThanEnd = true;
                        break;
                    }
                    case ePoints::Point_NathanosMoveAfterFightWithThanEnd:
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
                if (!me->isSummon() || !m_PathToThanEnd)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (Unit* l_Unit = me->getVictim())
                {
                    if (me->IsWithinMeleeRange(l_Unit))
                        DoMeleeAttackIfReady();
                    else
                        DoSpellAttackIfReady(eSpells::Spell_Shoot);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nathanos_blightcaller_91158AI(p_Creature);
        }
};

/// Thane Wildsky - 91571
class npc_thane_wildsky_91571 : public CreatureScript
{
    public:
        npc_thane_wildsky_91571() : CreatureScript("npc_thane_wildsky_91571") { }

        struct npc_thane_wildsky_91571AI : public ScriptedAI
        {
            npc_thane_wildsky_91571AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_BloodyHack         = 185698,
                Spell_PlagueArrow        = 182466,
                Spell_PlagueArrowWarning = 186047,
                Spell_Camouflage         = 154410
            };

            enum eEvents
            {
                Event_CastBloodyHack          = 1,
                Event_RangersCastPlagueArrows = 2
            };

            enum eObjectives
            {
                Obj_ThaneWildskyLineHeard = 3836200
            };

            enum eQuests
            {
                Quest_AGrimTrophy = 38362
            };

            enum eNpcs
            {
                Npc_NathanosBlightcaller = 91158,
                Npc_ForsakenRanger       = 91950
            };

            enum eActions
            {
                Action_NathanosFightWithThane   = 4,
                Action_NathanosFightWithThanEnd = 5
            };

            enum ePoints
            {
                Point_RangerCamouflageMoveEnd = 2
            };

            std::vector<ObjectGuid> m_FightEventCreaturesGuids;
            ObjectGuid m_FirstRangerGuid;
            ObjectGuid m_SecondRangerGuid;
            ObjectGuid m_ThirdRangerGuid;
            ObjectGuid m_FourthRangerGuid;
            ObjectGuid m_NathanosGuid;
            EventMap m_Events;
            Position const m_FirstRangerSpawnPos  = { 4343.068f, 2564.008f, 62.4986f,  1.752949f  };
            Position const m_SecondRangerSpawnPos = { 4305.588f, 2605.437f, 60.03453f, 6.05935f   };
            Position const m_ThirdRangerSpawnPos  = { 4318.443f, 2576.236f, 68.8053f,  0.8793465f };
            Position const m_FourthRangerSpawnPos = { 4311.024f, 2583.206f, 68.35406f, 0.5320016f };
            Position const m_NathanosSpawnPos     = { 4363.026f, 2595.179f, 39.21639f, 2.655514f  };
            bool m_NathanosSummoned = false;
            bool m_PlagueArrowLineComplete = false;

            void ResetVariables()
            {
                m_FightEventCreaturesGuids.clear();
                m_FirstRangerGuid.Clear();
                m_SecondRangerGuid.Clear();
                m_ThirdRangerGuid.Clear();
                m_FourthRangerGuid.Clear();
                m_NathanosGuid.Clear();
                m_NathanosSummoned = false;
                m_PlagueArrowLineComplete = false;
                m_Events.Reset();
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 40.0f && l_Player->HasQuest(eQuests::Quest_AGrimTrophy) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_ThaneWildskyLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_ThaneWildskyLineHeard);
                }
            }

            void JustReachedHome() override
            {
                for (auto itr : m_FightEventCreaturesGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }

                ResetVariables();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_NathanosGuid))
                {
                    l_Creature->AI()->DoAction(eActions::Action_NathanosFightWithThanEnd);
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_FirstRangerGuid))
                {
                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_Camouflage, true);
                    }, 3000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->SetWalk(true);
                        l_Creature->GetMotionMaster()->MoveBackward(ePoints::Point_RangerCamouflageMoveEnd, 4343.974f, 2559.091f, 64.30225f);
                    }, 4000);
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_SecondRangerGuid))
                {
                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_Camouflage, true);
                    }, 3000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->SetWalk(true);
                        l_Creature->GetMotionMaster()->MoveBackward(ePoints::Point_RangerCamouflageMoveEnd, 4300.712f, 2606.547f, 61.43257f);
                    }, 4000);
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_ThirdRangerGuid))
                {
                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_Camouflage, true);
                    }, 3000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->SetWalk(true);
                        l_Creature->GetMotionMaster()->MoveBackward(ePoints::Point_RangerCamouflageMoveEnd, 4315.255f, 2572.384f, 71.35875f);
                    }, 4000);
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_FourthRangerGuid))
                {
                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_Camouflage, true);
                    }, 3000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->SetWalk(true);
                        l_Creature->GetMotionMaster()->MoveBackward(ePoints::Point_RangerCamouflageMoveEnd, 4306.715f, 2580.669f, 70.7755f);
                    }, 4000);
                }

                ResetVariables();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                me->CallForHelp(20.0f);
                Talk(1);
                m_Events.ScheduleEvent(eEvents::Event_CastBloodyHack, 5000);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (attacker->ToPlayer() && !attacker->ToPlayer()->HasQuest(eQuests::Quest_AGrimTrophy))
                    return;

                if (me->GetHealthPct() <= 90 && !m_NathanosSummoned)
                {
                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_NathanosBlightcaller, m_NathanosSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_NathanosFightWithThane);
                        m_NathanosGuid = l_Creature->GetGUID();
                        m_FightEventCreaturesGuids.push_back(m_NathanosGuid);
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ForsakenRanger, m_FirstRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0))
                    {
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
                        l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 385);
                        m_FirstRangerGuid = l_Creature->GetGUID();
                        m_FightEventCreaturesGuids.push_back(m_FirstRangerGuid);
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ForsakenRanger, m_SecondRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0))
                    {
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
                        l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 385);
                        m_SecondRangerGuid = l_Creature->GetGUID();
                        m_FightEventCreaturesGuids.push_back(m_SecondRangerGuid);
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ForsakenRanger, m_ThirdRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0))
                    {
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
                        l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 385);
                        m_ThirdRangerGuid = l_Creature->GetGUID();
                        m_FightEventCreaturesGuids.push_back(m_ThirdRangerGuid);
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ForsakenRanger, m_FourthRangerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0))
                    {
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
                        l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 385);
                        m_FourthRangerGuid = l_Creature->GetGUID();
                        m_FightEventCreaturesGuids.push_back(m_FourthRangerGuid);
                    }

                    m_Events.ScheduleEvent(eEvents::Event_RangersCastPlagueArrows, 1000);
                    m_NathanosSummoned = true;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastBloodyHack:
                {
                    DoCastVictim(eSpells::Spell_BloodyHack);
                    m_Events.ScheduleEvent(eEvents::Event_CastBloodyHack, urand(13000, 15000));
                    break;
                }
                case eEvents::Event_RangersCastPlagueArrows:
                {
                    if (!m_PlagueArrowLineComplete)
                    {
                        DoCast(eSpells::Spell_PlagueArrowWarning);
                        m_PlagueArrowLineComplete = true;
                    }

                    for (auto itr : m_FightEventCreaturesGuids)
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                        {
                            if (l_Creature->GetEntry() == eNpcs::Npc_ForsakenRanger)
                            {
                                l_Creature->CastSpell(me, eSpells::Spell_PlagueArrow, false);
                            }
                        }
                    }

                    m_Events.ScheduleEvent(eEvents::Event_RangersCastPlagueArrows, 11000);
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
            return new npc_thane_wildsky_91571AI(p_Creature);
        }
};

/// Huginn - 97305
class npc_huginn_97305 : public CreatureScript
{
    public:
        npc_huginn_97305() : CreatureScript("npc_huginn_97305") { }

        struct npc_huginn_97305AI : public ScriptedAI
        {
            npc_huginn_97305AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Huginn[3] =
            {
                { 3965.126f, 2892.837f, 252.8991f },
                { 3944.49f, 2871.637f, 252.8991f  },
                { 3902.323f, 2819.003f, 252.8991f }
            };

            enum ePoints
            {
                Point_HuginnMoveUpEnd  = 1,
                Point_HuginnFlyPathEnd = 2
            };

            enum eSpells
            {
                Spell_RunedScrollConversation = 183614
            };

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        Position const l_FlyUpPos = { 3976.914f, 2898.343f, 251.7536f };

                        l_Creature->SetSpeed(MOVE_FLIGHT, 0.5f);
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_HuginnMoveUpEnd, l_FlyUpPos);
                    }, 2000);
                }

                p_Player->CastSpell(p_Player, eSpells::Spell_RunedScrollConversation, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_PointId) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;

                switch (p_PointId)
                {
                    case ePoints::Point_HuginnMoveUpEnd:
                    {
                        me->SetSpeed(MOVE_FLIGHT, 1.0f);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_HuginnFlyPathEnd, Path_Huginn, 3);
                        break;
                    }
                    case ePoints::Point_HuginnFlyPathEnd:
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
            return new npc_huginn_97305AI(p_Creature);
        }
};

/// Forsaken Bat-Rider - 97250
class npc_forsaken_bat_rider_97250 : public CreatureScript
{
    public:
        npc_forsaken_bat_rider_97250() : CreatureScript("npc_forsaken_bat_rider_97250") { }

        struct npc_forsaken_bat_rider_97250AI : public VehicleAI
        {
            npc_forsaken_bat_rider_97250AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_BatRiderAfterEject[3] =
            {
                { 3959.409f, 2884.411f, 257.5773f },
                { 3919.918f, 2857.729f, 301.0493f },
                { 3841.984f, 2835.719f, 338.607f  }
            };

            enum ePoints
            {
                Point_BatRiderPathToFootholdEnd = 1,
                Point_BatRiderPathAfterEjectEnd = 2
            };

            enum eSpells
            {
                Spell_RideVehicle = 52391
            };

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                me->DestroyForPlayer(p_Player);

                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_PlayerGuid = p_Player->GetGUID();

                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->Mount(62247);
                    }, 3000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Creature->AI()->Talk(0, l_PlayerGuid);
                    }, 4000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Player->CastSpell(l_Creature, eSpells::Spell_RideVehicle, true);
                    }, 5000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_BatRiderToFoothold[6] =
                        {
                            { 3765.331f, 2917.064f, 433.1172f },
                            { 3788.091f, 2919.985f, 437.4163f },
                            { 3866.476f, 2942.092f, 398.2769f },
                            { 3930.006f, 2950.385f, 350.7877f },
                            { 3963.462f, 2901.734f, 259.7968f },
                            { 3953.732f, 2886.535f, 242.3017f }
                        };

                        l_Creature->SetDisableGravity(true);
                        l_Creature->SetSpeed(MOVE_FLIGHT, 2.0f);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_BatRiderPathToFootholdEnd, Path_BatRiderToFoothold, 6);
                    }, 7000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_BatRiderPathToFootholdEnd:
                    {
                        if (me->GetVehicleKit())
                        {
                            me->GetVehicleKit()->RemoveAllPassengers();
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_BatRiderPathAfterEjectEnd, Path_BatRiderAfterEject, 3);
                        }

                        break;
                    }
                    case ePoints::Point_BatRiderPathAfterEjectEnd:
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
            return new npc_forsaken_bat_rider_97250AI(p_Creature);
        }
};

/// Havi - 96254
class npc_havi_96254 : public CreatureScript
{
    public:
        npc_havi_96254() : CreatureScript("npc_havi_96254") { }

        struct npc_havi_96254AI : public ScriptedAI
        {
            npc_havi_96254AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_HavisTest = 38331
            };

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Quest->GetQuestId() == eQuests::Quest_HavisTest)
                {
                    uint64 l_PlayerGuid = p_Player->GetGUID();

                    ChangeHidingForPlayer(me, l_PlayerGuid, true);

                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Creature->AI()->Talk(0, l_PlayerGuid);
                        }, 3000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Creature->AI()->Talk(1, l_PlayerGuid);
                        }, 9000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Creature->AI()->Talk(2, l_PlayerGuid);
                        }, 15000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Creature->AI()->Talk(3, l_PlayerGuid);
                        }, 24000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid, this]() -> void
                        {
                            if (me)
                                ChangeHidingForPlayer(me, l_PlayerGuid, false);
                            l_Creature->DespawnOrUnsummon();
                        }, 30000);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_havi_96254AI(p_Creature);
        }
};

/// Throwing Veggies, Throwing Crabs, Throwing Grain - 110605, 110608, 110609
class npc_stormheim_ingredients : public CreatureScript
{
    public:
        npc_stormheim_ingredients() : CreatureScript("npc_stormheim_ingredients") { }

        struct npc_stormheim_ingredientsAI : public VehicleAI
        {
            npc_stormheim_ingredientsAI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_RideVehicle = 52391
            };

            enum eNpcs
            {
                Npc_Veggies = 110605,
                Npc_Crabs   = 110608,
                Npc_Grain   = 110609
            };

            enum eObjectives
            {
                Obj_RootVegetablesAdded = 285317,
                Obj_CrabsAdded          = 285318,
                Obj_HeartyGrainsAdded   = 285319
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->NearTeleportTo(p_Summoner->GetPosition());
                m_SummonerGuid = p_Summoner->GetGUID();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                p_Summoner->DelayedCastSpell(me, eSpells::Spell_RideVehicle, true, 500);
            }

            void OnCharmed(bool apply) override
            {
                if (!apply)
                {
                    me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                Vehicle* l_Vehicle = me->GetVehicleKit();
                if (!l_Player || !l_Vehicle)
                    return;

                if (me->GetEntry() == eNpcs::Npc_Veggies &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RootVegetablesAdded) == 1)
                {
                    l_Vehicle->RemoveAllPassengers();
                    me->DespawnOrUnsummon();
                }
                else if (me->GetEntry() == eNpcs::Npc_Crabs &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CrabsAdded) == 1)
                {
                    l_Vehicle->RemoveAllPassengers();
                    me->DespawnOrUnsummon();
                }
                else if (me->GetEntry() == eNpcs::Npc_Grain &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_HeartyGrainsAdded) == 1)
                {
                    l_Vehicle->RemoveAllPassengers();
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stormheim_ingredientsAI(p_Creature);
        }
};

/// Skovald Event Seen Trigger - 3959000
class npc_skovald_event_trigger_3959000 : public CreatureScript
{
    public:
        npc_skovald_event_trigger_3959000() : CreatureScript("npc_skovald_event_trigger_3959000") { }

        struct npc_skovald_event_trigger_3959000AI : public ScriptedAI
        {
            npc_skovald_event_trigger_3959000AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_SummonGodKing = 190947
            };

            enum eObjectives
            {
                Obj_SkovaldEventSeenTrigger = 3959000
            };

            enum eQuests
            {
                Quest_AheadOfTheGame = 39590
            };

            Position const m_SkovaldSpellDestPos = { 3442.498f, 2285.231f, 239.4956f, 1.62559f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 20.0f && l_Player->GetQuestStatus(eQuests::Quest_AheadOfTheGame) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SkovaldEventSeenTrigger) == 0)
                {
                    l_Player->CastSpell(m_SkovaldSpellDestPos, eSpells::Spell_SummonGodKing, true);
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_SkovaldEventSeenTrigger);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skovald_event_trigger_3959000AI(p_Creature);
        }
};

/// God-King Skovald - 96314
class npc_god_king_skovald_96314 : public CreatureScript
{
    public:
        npc_god_king_skovald_96314() : CreatureScript("npc_god_king_skovald_96314") { }

        struct npc_god_king_skovald_96314AI : public ScriptedAI
        {
            npc_god_king_skovald_96314AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_HoldYotnarsHead  = 190939,
                Spell_ThrowYotnarsHead = 190941,
                Spell_GodKingPortal    = 190948
            };

            enum eObjectives
            {
                Obj_YotnarsHeadThrown = 3959001
            };

            enum ePoints
            {
                Point_SkovaldMoveFromVaultEnd = 1,
                Point_SkovaldMoveToPortalEnd  = 2
            };

            enum eEvents
            {
                Event_ThrowHead             = 1,
                Event_HeadVisibleKillcredit = 2,
                Event_HeadSayLine           = 3,
                Event_MoveToPortal          = 4
            };

            enum eNpcs
            {
                Npc_Yotnar = 96283
            };

            ObjectGuid m_SummonerGuid;
            EventMap m_Events;
            Position const m_SkovaldMoveFromVaultPos = { 3441.42f,  2305.04f,  239.4295f };
            Position const m_SkovaldMoveToPortalPos  = { 3442.316f, 2293.024f, 239.3023f };
            Position const m_HeadMissileDestPos      = { 3458.023f, 2321.406f, 239.28f };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
                DoCast(eSpells::Spell_HoldYotnarsHead, true);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(ePoints::Point_SkovaldMoveFromVaultEnd, m_SkovaldMoveFromVaultPos);
                Talk(0, m_SummonerGuid, 1);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_SkovaldMoveFromVaultEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            Talk(1, m_SummonerGuid);
                        }

                        m_Events.ScheduleEvent(eEvents::Event_ThrowHead, 9000);
                        break;
                    }
                    case ePoints::Point_SkovaldMoveToPortalEnd:
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
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_ThrowHead:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            Talk(2, m_SummonerGuid);
                        }

                        me->RemoveAura(eSpells::Spell_HoldYotnarsHead);
                        me->CastSpell(m_HeadMissileDestPos, eSpells::Spell_ThrowYotnarsHead, true);
                        m_Events.ScheduleEvent(eEvents::Event_HeadVisibleKillcredit, 2300);
                        break;
                    }
                    case eEvents::Event_HeadVisibleKillcredit:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_YotnarsHeadThrown);
                        }

                        m_Events.ScheduleEvent(eEvents::Event_HeadSayLine, 3000);
                        break;
                    }
                    case eEvents::Event_HeadSayLine:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_Yotnar, 50.0f, true))
                            {
                                l_Creature->AI()->Talk(0, m_SummonerGuid);
                            }
                        }

                        m_Events.ScheduleEvent(eEvents::Event_MoveToPortal, 5000);
                        break;
                    }
                    case eEvents::Event_MoveToPortal:
                    {
                        me->SetFacingTo(4.677482f);
                        DoCast(eSpells::Spell_GodKingPortal);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_SkovaldMoveToPortalEnd, m_SkovaldMoveToPortalPos);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_god_king_skovald_96314AI(p_Creature);
        }
};

/// Felskorn Torturer - 96121
class npc_felskorn_torturer_96121 : public CreatureScript
{
    public:
        npc_felskorn_torturer_96121() : CreatureScript("npc_felskorn_torturer_96121") { }

        struct npc_felskorn_torturer_96121AI : public ScriptedAI
        {
            npc_felskorn_torturer_96121AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveAfterSummonEnd = 1
            };

            enum eNpcs
            {
                Npc_BlazesightOculus = 99450
            };

            enum eSpells
            {
                Spell_BlazesightOcculusSummonCosmetics = 195678,
                Spell_MindRend = 187706,
                Spell_FieryBlink = 195590,
                Spell_BurningGlare = 195682
            };

            enum eEvents
            {
                Event_CastMindRend = 1,
                Event_CastFieryBlink = 2,
                Event_BurningGlare = 3
            };

            enum eActions
            {
                Action_ConsoleActivated = 1,
                Action_FelskornTorturerEvaded = 2
            };

            std::vector<ObjectGuid> m_BlazesightOcculusGuids;
            ObjectGuid m_ConsoleGuid;
            EventMap m_Events;
            Position const m_MoveAfterSummonPos = { 3442.3f, 2086.05f, 277.3493f };
            Position const m_FirstBlazesightOculusSpawnPos = { 3435.262f, 2087.788f, 277.3831f, 1.333907f };
            Position const m_SecondBlazesightOculusSpawnPos = { 3449.137f, 2087.514f, 277.3831f, 1.837686f };

            void JustReachedHome() override
            {
                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_FelskornTorturerEvaded);
                }

                for (auto itr : m_BlazesightOcculusGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }

                me->DespawnOrUnsummon();
            }

            void InitializeAI() override
            {
                if (!me->isSummon())
                    return;

                Talk(0, 0, 1);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_MoveAfterSummonEnd, m_MoveAfterSummonPos);
                }, 1000);

                me->AddDelayedEvent([this]() -> void
                {
                    Talk(1);
                }, 3000);

                me->AddDelayedEvent([this]() -> void
                {
                    Talk(2);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_BlazesightOculus, m_FirstBlazesightOculusSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
                    {
                        m_BlazesightOcculusGuids.push_back(l_Creature->GetGUID());
                        l_Creature->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_BlazesightOcculusSummonCosmetics, true);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                        }, 12000);
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_BlazesightOculus, m_SecondBlazesightOculusSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
                    {
                        m_BlazesightOcculusGuids.push_back(l_Creature->GetGUID());
                        l_Creature->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_BlazesightOcculusSummonCosmetics, true);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                        }, 12000);
                    }
                }, 14000);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID = 0) override
            {
                m_ConsoleGuid = p_Guid;
            }

            void EnterCombat(Unit* victim) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastMindRend, 5000);
                m_Events.ScheduleEvent(eEvents::Event_CastFieryBlink, 8000);
                m_Events.ScheduleEvent(eEvents::Event_BurningGlare, 14000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_ConsoleActivated);
                }

                for (auto itr : m_BlazesightOcculusGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastMindRend:
                    {
                        DoCastVictim(eSpells::Spell_MindRend);
                        m_Events.ScheduleEvent(eEvents::Event_CastMindRend, urand(9000, 13000));
                        break;
                    }
                    case eEvents::Event_CastFieryBlink:
                    {
                        DoCastVictim(eSpells::Spell_FieryBlink);
                        m_Events.ScheduleEvent(eEvents::Event_CastFieryBlink, urand(7000, 10000));
                        break;
                    }
                    case eEvents::Event_BurningGlare:
                    {
                        DoCastVictim(eSpells::Spell_BurningGlare);
                        m_Events.ScheduleEvent(eEvents::Event_BurningGlare, urand(14000, 17000));
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
            return new npc_felskorn_torturer_96121AI(p_Creature);
        }
};

/// A Trial of Valor Quest Creatures - 96078, 96071, 96074, 96075, 96076
class npc_trial_of_valor_creatures : public CreatureScript
{
    public:
        npc_trial_of_valor_creatures() : CreatureScript("npc_trial_of_valor_creatures") { }

        struct npc_trial_of_valor_creaturesAI : public ScriptedAI
        {
            npc_trial_of_valor_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_KingsPoweredVisual   = 190745,
                Spell_PlayerHologram       = 190556,
                Spell_FirstConv            = 197661,
                Spell_SecondConv           = 197662,
                Spell_ThirdConv            = 197663,
                Spell_RulersHologramVisual = 190549,
                Spell_MysticKingChannel    = 190685
            };

            enum ePoints
            {
                Point_MoveToPlayerCopyEnd = 1
            };

            enum eNpcs
            {
                Npc_PlayerCopyController = 96078,
                Npc_WarriorKing          = 96071,
                Npc_MysticKing           = 96074,
                Npc_DragonRiderKing      = 96075,
                Npc_ShieldmaidenQueen    = 96076
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());

                if (me->GetEntry() == eNpcs::Npc_PlayerCopyController)
                {
                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 8);
                    DoCast(eSpells::Spell_KingsPoweredVisual, true);
                    p_Summoner->CastSpell(p_Summoner, eSpells::Spell_PlayerHologram, true);
                    p_Summoner->DelayedCastSpell(p_Summoner, eSpells::Spell_FirstConv, true, 4000);
                    p_Summoner->DelayedCastSpell(p_Summoner, eSpells::Spell_SecondConv, true, 14000);
                    p_Summoner->DelayedCastSpell(p_Summoner, eSpells::Spell_ThirdConv, true, 25000);
                    me->DespawnOrUnsummon(32000);
                }
                else
                {
                    me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
                    me->SetWalk(true);
                    DoCast(eSpells::Spell_RulersHologramVisual, true);
                    me->DespawnOrUnsummon(28000);
                }

                if (me->GetEntry() == eNpcs::Npc_WarriorKing)
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToPlayerCopyEnd, 3446.655f, 2094.542f, 277.3f);
                    }, 4000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 375);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 1);
                    }, 14000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
                        me->PlayOneShotAnimKitId(7866);
                    }, 25000);
                }

                if (me->GetEntry() == eNpcs::Npc_MysticKing)
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToPlayerCopyEnd, 3438.405f, 2094.891f, 277.3f);
                    }, 4000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        DoCast(eSpells::Spell_MysticKingChannel, true);
                    }, 14000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->RemoveAura(eSpells::Spell_MysticKingChannel);
                        me->PlayOneShotAnimKitId(7866);
                    }, 25000);
                }

                if (me->GetEntry() == eNpcs::Npc_DragonRiderKing)
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToPlayerCopyEnd, 3446.563f, 2102.382f, 277.445f);
                    }, 4000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 1);
                    }, 14000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
                        me->PlayOneShotAnimKitId(7866);
                    }, 25000);
                }

                if (me->GetEntry() == eNpcs::Npc_ShieldmaidenQueen)
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToPlayerCopyEnd, 3438.919f, 2102.396f, 277.311f);
                    }, 4000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 1);
                    }, 14000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
                        me->PlayOneShotAnimKitId(2206);
                    }, 25000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trial_of_valor_creaturesAI(p_Creature);
        }
};

/// Felskorn Raider - 96129
class npc_felskorn_raider_96129 : public CreatureScript
{
    public:
        npc_felskorn_raider_96129() : CreatureScript("npc_felskorn_raider_96129") { }

        struct npc_felskorn_raider_96129AI : public ScriptedAI
        {
            npc_felskorn_raider_96129AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Skewer        = 195512,
                Spell_RazorsharpAxe = 195506
            };

            enum eEvents
            {
                Event_CastSkewer        = 1,
                Event_CastRazorsharpAxe = 2
            };

            enum eActions
            {
                Action_RaiderDied    = 1,
                Action_SummonsEvaded = 3
            };

            enum ePoints
            {
                Point_FelskornRaiderMoveToConsoleEnd = 1
            };

            ObjectGuid m_ConsoleGuid;
            EventMap m_Events;

            void JustReachedHome() override
            {
                if (!me->isSummon())
                    return;

                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_SummonsEvaded);
                }

                me->DespawnOrUnsummon();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID = 0) override
            {
                m_ConsoleGuid = p_Guid;
            }

            void EnterCombat(Unit* victim) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastSkewer, 5000);
                m_Events.ScheduleEvent(eEvents::Event_CastRazorsharpAxe, urand(3000, 10000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!me->isSummon())
                    return;

                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_RaiderDied);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FelskornRaiderMoveToConsoleEnd)
                {
                    me->SetHomePosition(me->GetPosition());
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastSkewer:
                    {
                        DoCastVictim(eSpells::Spell_Skewer);
                        m_Events.ScheduleEvent(eEvents::Event_CastSkewer, urand(7000, 8000));
                        break;
                    }
                    case eEvents::Event_CastRazorsharpAxe:
                    {
                        DoCastVictim(eSpells::Spell_RazorsharpAxe);
                        m_Events.ScheduleEvent(eEvents::Event_CastRazorsharpAxe, urand(15000, 30000));
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
            return new npc_felskorn_raider_96129AI(p_Creature);
        }
};

/// Felskorn Warmonger - 108263
class npc_felskorn_warmonger_108263 : public CreatureScript
{
    public:
        npc_felskorn_warmonger_108263() : CreatureScript("npc_felskorn_warmonger_108263") { }

        struct npc_felskorn_warmonger_108263AI : public ScriptedAI
        {
            npc_felskorn_warmonger_108263AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_FelblazeCleave = 187653,
                Spell_IgniteFelblade = 187656
            };

            enum eEvents
            {
                Event_CastFelblazeCleave = 1,
                Event_CastIgniteFelblade = 2
            };

            enum eActions
            {
                Action_WarmongerDied = 2,
                Action_SummonsEvaded = 3
            };

            enum ePoints
            {
                Point_FelskornWarmongerMoveToConsoleEnd = 2
            };

            ObjectGuid m_ConsoleGuid;
            EventMap m_Events;

            void JustReachedHome() override
            {
                if (!me->isSummon())
                    return;

                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_SummonsEvaded);
                }

                me->DespawnOrUnsummon();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID = 0) override
            {
                m_ConsoleGuid = p_Guid;
            }

            void EnterCombat(Unit* victim) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastFelblazeCleave, 5000);
                m_Events.ScheduleEvent(eEvents::Event_CastIgniteFelblade, 9000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!me->isSummon())
                    return;

                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_WarmongerDied);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FelskornWarmongerMoveToConsoleEnd)
                {
                    me->SetHomePosition(me->GetPosition());
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastFelblazeCleave:
                    {
                        DoCastVictim(eSpells::Spell_FelblazeCleave);
                        m_Events.ScheduleEvent(eEvents::Event_CastFelblazeCleave, urand(13000, 19000));
                        break;
                    }
                    case eEvents::Event_CastIgniteFelblade:
                    {
                        DoCast(eSpells::Spell_IgniteFelblade);
                        m_Events.ScheduleEvent(eEvents::Event_CastIgniteFelblade, 18000);
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
            return new npc_felskorn_warmonger_108263AI(p_Creature);
        }
};

/// A Trial of Will Quest Creatures - 96144, 96067
class npc_trial_of_will_creatures : public CreatureScript
{
    public:
        npc_trial_of_will_creatures() : CreatureScript("npc_trial_of_will_creatures") { }

        struct npc_trial_of_will_creaturesAI : public ScriptedAI
        {
            npc_trial_of_will_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_DragonsPoweredVisual = 190715,
                Spell_PlayerHologram       = 190556,
                Spell_FirstConversation    = 197658,
                Spell_SecondConversation   = 197659,
                Spell_ThirdConversation    = 197660,
                Spell_RulersHologramVisual = 190549,
                Spell_StormVisual          = 190758,
                Spell_DragonBreath         = 190752
            };

            enum eNpcs
            {
                Npc_PlayerCopyController = 96144,
                Npc_StormDragonHologram  = 96067
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());

                if (me->GetEntry() == eNpcs::Npc_PlayerCopyController)
                {
                    me->SetAIAnimKitId(7752);
                    DoCast(eSpells::Spell_DragonsPoweredVisual, true);
                    p_Summoner->CastSpell(p_Summoner, eSpells::Spell_PlayerHologram, true);
                    p_Summoner->CastSpell(p_Summoner, eSpells::Spell_FirstConversation, true);
                    p_Summoner->DelayedCastSpell(p_Summoner, eSpells::Spell_SecondConversation, true, 8000);
                    p_Summoner->DelayedCastSpell(p_Summoner, eSpells::Spell_ThirdConversation, true, 17000);
                    me->DespawnOrUnsummon(25000);
                }
                else if (me->GetEntry() == eNpcs::Npc_StormDragonHologram)
                {
                    me->NearTeleportTo(3333.486f, 2230.95f, 324.7559f, 4.712389f);
                    me->SetAIAnimKitId(947);
                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                    DoCast(eSpells::Spell_RulersHologramVisual, true);
                    DoCast(eSpells::Spell_StormVisual, true);
                    me->DespawnOrUnsummon(23000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetAIAnimKitId(0);
                        me->PlayOneShotAnimKitId(574);
                    }, 8000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetAIAnimKitId(947);
                    }, 12000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetAIAnimKitId(0);
                        DoCast(eSpells::Spell_DragonBreath);
                    }, 17000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trial_of_will_creaturesAI(p_Creature);
        }
};

/// Yotnar - 96175
class npc_yotnar_96175 : public CreatureScript
{
    public:
        npc_yotnar_96175() : CreatureScript("npc_yotnar_96175") { }

        struct npc_yotnar_96175AI : public ScriptedAI
        {
            npc_yotnar_96175AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_HologramActivate         = 190773,
                Spell_TrialOfMightCredit       = 190908,
                Spell_ActivateGuardianOrb      = 195765,
                Spell_LightforgedDefenseMatrix = 195799
            };

            enum eEvents
            {
                Event_CastActivateGuardianOrb      = 1,
                Event_CastLightforgedDefenseMatrix = 2
            };

            enum eActions
            {
                Action_ConsoleReady = 1
            };

            enum eNpcs
            {
                Npc_BunnyVisual         = 35845,
                Npc_LightforgedSentinel = 96174
            };

            enum eKillcredits
            {
                Killcredit_YotnarDefeated = 96175
            };

            enum ePoints
            {
                Point_YotnarReturnToHomePos = 1
            };

            std::vector<ObjectGuid> m_SentinelGuids;
            ObjectGuid m_ConsoleGuid;
            EventMap m_Events;
            Position const m_FirstSentinelSpawnPos  = { 3529.068f, 2129.038f, 234.4087f, 2.304771f  };
            Position const m_SecondSentinelSpawnPos = { 3479.516f, 2129.606f, 234.7332f, 0.7552171f };
            Position const m_ThirdSentinelSpawnPos  = { 3481.21f,  2200.563f, 234.7332f, 5.581653f  };
            Position const m_FourthSentinelSpawnPos = { 3529.406f, 2197.368f, 235.5099f, 3.795767f  };
            bool m_YotnarDefeated = false;

            void InitializeAI() override
            {
                if (!me->isSummon())
                    return;

                if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_BunnyVisual, 5.0f, true))
                {
                    l_Creature->CastSpell(l_Creature, eSpells::Spell_HologramActivate, true);
                }

                Talk(0, 0, 1);

                me->AddDelayedEvent([this]() -> void
                {
                    Talk(1);

                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 50.0f);

                    if (l_CreatureList.empty())
                        return;

                    for (auto itr : l_CreatureList)
                    {
                        if (int32(itr->GetPosition().m_positionZ) == 234)
                        {
                            itr->CastSpell(itr, eSpells::Spell_HologramActivate, true);
                        }
                    }
                }, 9000);

                me->AddDelayedEvent([this]() -> void
                {
                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_LightforgedSentinel, m_FirstSentinelSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        m_SentinelGuids.push_back(l_Creature->GetGUID());
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_LightforgedSentinel, m_SecondSentinelSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        m_SentinelGuids.push_back(l_Creature->GetGUID());
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_LightforgedSentinel, m_ThirdSentinelSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        m_SentinelGuids.push_back(l_Creature->GetGUID());
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_LightforgedSentinel, m_FourthSentinelSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        m_SentinelGuids.push_back(l_Creature->GetGUID());
                    }

                }, 10000);

                me->AddDelayedEvent([this]() -> void
                {
                    Talk(2);
                    DoCastAOE(eSpells::Spell_TrialOfMightCredit);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, 256);

                    if (Player* l_Player = me->FindNearestPlayer(100.0f))
                    {
                        AttackStart(l_Player, true);
                    }
                }, 17000);
            }

            void JustReachedHome() override
            {
                if (!me->isSummon())
                    return;

                if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                {
                    l_Gob->AI()->DoAction(eActions::Action_ConsoleReady);
                }

                for (auto itr : m_SentinelGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }

                me->DespawnOrUnsummon();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID = 0) override
            {
                m_ConsoleGuid = p_Guid;
            }

            void EnterCombat(Unit* victim) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastActivateGuardianOrb, 5000);
                m_Events.ScheduleEvent(eEvents::Event_CastLightforgedDefenseMatrix, 10000);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetHealth(1);
                }

                if (me->GetHealthPct() <= 20 && !m_YotnarDefeated)
                {
                    for (auto itr : me->getThreatManager().getThreatList())
                    {
                        if (Unit* l_Unit = itr->getTarget())
                        {
                            if (Player* l_Player = l_Unit->ToPlayer())
                            {
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_YotnarDefeated);
                            }
                        }
                    }

                    me->SetFlag(UNIT_FIELD_FLAGS, 526592);
                    me->CastStop();
                    me->StopAttack();
                    Talk(3);
                    me->SetFacingToObject(attacker);
                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 8);
                    m_Events.Reset();
                    m_YotnarDefeated = true;

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->RemoveAllAuras();
                        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                        me->SetWalk(true);
                        Talk(4);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_YotnarReturnToHomePos, me->GetHomePosition());
                    }, 5000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetFacingTo(me->GetHomePosition().m_orientation);
                        Talk(5);
                    }, 15000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_ConsoleGuid))
                        {
                            l_Gob->AI()->DoAction(eActions::Action_ConsoleReady);
                        }

                        for (auto itr : m_SentinelGuids)
                        {
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                            {
                                l_Creature->DespawnOrUnsummon();
                            }
                        }

                        me->DespawnOrUnsummon();
                    }, 25000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastActivateGuardianOrb:
                {
                    DoCastVictim(eSpells::Spell_ActivateGuardianOrb);
                    m_Events.ScheduleEvent(eEvents::Event_CastActivateGuardianOrb, 14000);
                    break;
                }
                case eEvents::Event_CastLightforgedDefenseMatrix:
                {
                    DoCast(eSpells::Spell_LightforgedDefenseMatrix);
                    m_Events.ScheduleEvent(eEvents::Event_CastLightforgedDefenseMatrix, 14000);
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
            return new npc_yotnar_96175AI(p_Creature);
        }
};

/// Yotnar - 96258
class npc_yotnar_96258 : public CreatureScript
{
    public:
        npc_yotnar_96258() : CreatureScript("npc_yotnar_96258") { }

        struct npc_yotnar_96258AI : public VehicleAI
        {
            npc_yotnar_96258AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_YotnarsBlessingFirst  = 190904,
                Spell_YotnarsBlessingSecond = 190905
            };

            enum eKillcredits
            {
                Killcredit_YotnarsBlessingReceived = 96285
            };

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();

                uint64 l_PlayerGuid = p_Player->GetGUID();

                ChangeHidingForPlayer(me, l_PlayerGuid, true);

                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                        {
                            l_Creature->SetFacingToObject(l_Player);
                            l_Creature->AI()->Talk(0, l_PlayerGuid);
                        }
                    }, 1000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_YotnarsBlessingFirst, true);
                            l_Creature->AI()->Talk(1, l_PlayerGuid);
                        }
                    }, 8000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                        {
                            l_Creature->RemoveAura(eSpells::Spell_YotnarsBlessingFirst);
                            l_Player->RemoveAura(eSpells::Spell_YotnarsBlessingFirst);
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_YotnarsBlessingReceived);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_YotnarsBlessingSecond, true);
                            l_Creature->AI()->Talk(2, l_Player->GetGUID());
                        }
                    }, 17000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Creature->AI()->Talk(3, l_Player->GetGUID());
                    }, 23000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid, this]() -> void
                    {
                        if (me)
                            ChangeHidingForPlayer(me, l_PlayerGuid, false);
                        l_Creature->DespawnOrUnsummon();
                    }, 27000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_yotnar_96258AI(p_Creature);
        }
};

/// Dread-Rider Cullen - 92561
class npc_dread_rider_cullen_92561 : public CreatureScript
{
    public:
        npc_dread_rider_cullen_92561() : CreatureScript("npc_dread_rider_cullen_92561") { }

        struct npc_dread_rider_cullen_92561AI : public ScriptedAI
        {
            npc_dread_rider_cullen_92561AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            enum eQuests
            {
                Quest_WillOfTheThorignir = 38611
            };

            enum eObjectives
            {
                Obj_DreadRiderCullenLineHeard = 3861100
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && l_Player->HasQuest(eQuests::Quest_WillOfTheThorignir) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_DreadRiderCullenLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_DreadRiderCullenLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Cullen[47] =
                        {
                            { 2880.219f, 2432.039f, 242.2244f },
                            { 2879.219f, 2432.039f, 242.2244f },
                            { 2876.969f, 2433.539f, 242.2244f },
                            { 2874.969f, 2435.789f, 242.2244f },
                            { 2870.469f, 2436.789f, 241.4744f },
                            { 2866.969f, 2437.539f, 240.9744f },
                            { 2864.219f, 2440.539f, 240.4744f },
                            { 2863.014f, 2441.608f, 239.8015f },
                            { 2859.678f, 2447.978f, 239.5209f },
                            { 2858.428f, 2450.228f, 239.2709f },
                            { 2857.178f, 2453.978f, 238.5209f },
                            { 2855.928f, 2456.478f, 238.0209f },
                            { 2855.178f, 2458.228f, 237.5209f },
                            { 2853.678f, 2461.978f, 237.0209f },
                            { 2852.928f, 2463.978f, 236.2709f },
                            { 2852.178f, 2465.728f, 235.5209f },
                            { 2851.928f, 2466.728f, 235.5209f },
                            { 2851.928f, 2467.978f, 235.7709f },
                            { 2851.678f, 2468.728f, 236.0209f },
                            { 2848.131f, 2471.095f, 234.7566f },
                            { 2844.692f, 2470.521f, 234.2343f },
                            { 2844.192f, 2470.271f, 233.9843f },
                            { 2841.692f, 2469.771f, 233.2343f },
                            { 2839.192f, 2469.021f, 232.9843f },
                            { 2837.692f, 2469.271f, 232.9843f },
                            { 2828.938f, 2469.993f, 232.4154f },
                            { 2826.406f, 2469.639f, 232.6122f },
                            { 2822.156f, 2468.889f, 232.6122f },
                            { 2817.406f, 2467.889f, 232.3622f },
                            { 2814.156f, 2467.389f, 232.1122f },
                            { 2808.656f, 2466.889f, 232.3622f },
                            { 2804.656f, 2466.139f, 231.8622f },
                            { 2800.156f, 2465.389f, 231.6122f },
                            { 2794.45f,  2464.198f, 230.5708f },
                            { 2786.625f, 2462.973f, 229.8823f },
                            { 2783.125f, 2462.473f, 229.1323f },
                            { 2780.625f, 2462.473f, 228.6323f },
                            { 2776.125f, 2461.723f, 227.6323f },
                            { 2773.875f, 2461.473f, 227.3823f },
                            { 2771.125f, 2460.973f, 226.6323f },
                            { 2767.375f, 2460.473f, 226.1323f },
                            { 2765.625f, 2460.223f, 225.6323f },
                            { 2762.625f, 2459.973f, 224.8823f },
                            { 2760.806f, 2459.405f, 223.7592f },
                            { 2757.606f, 2459.274f, 222.8821f },
                            { 2756.356f, 2459.274f, 222.3821f },
                            { 2739.965f, 2457.229f, 217.2049f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_Cullen, 47, false);
                    }, 2000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dread_rider_cullen_92561AI(p_Creature);
        }
};

/// Dread-Rider Cullen - 92566
class npc_dread_rider_cullen_92566 : public CreatureScript
{
    public:
        npc_dread_rider_cullen_92566() : CreatureScript("npc_dread_rider_cullen_92566") { }

        struct npc_dread_rider_cullen_92566AI : public ScriptedAI
        {
            npc_dread_rider_cullen_92566AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_JumpOnWallEnd   = 1,
                Point_JumpFromWallEnd = 2,
                Point_StealthMoveEnd  = 3
            };

            enum eQuests
            {
                Quest_AGrappleADay      = 38612,
                Quest_NoWingsRequired   = 38613,
                Quest_ToWeatherTheStorm = 38614
            };

            enum eObjectives
            {
                Obj_DreadRiderCullenLineHeard = 3861200
            };

            enum eSpells
            {
                Spell_GrapplingHookVisual = 215221,
                Spell_GrapplingGun        = 215227,
                Spell_Stealth             = 132653
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && l_Player->HasQuest(eQuests::Quest_AGrappleADay) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_DreadRiderCullenLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_DreadRiderCullenLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Player->HasQuest(eQuests::Quest_NoWingsRequired) && p_Player->HasQuest(eQuests::Quest_ToWeatherTheStorm))
                {
                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        uint64 l_PlayerGuid = p_Player->GetGUID();

                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            {
                                l_Creature->AI()->Talk(1, l_PlayerGuid);
                            }
                        }, 2000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetVirtualItem(4, 112340);
                            l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 2);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_GrapplingHookVisual, true);
                        }, 6000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetFacingTo(1.625093f);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_GrapplingGun, true);
                            l_Creature->GetMotionMaster()->MoveJump(2552.642f, 2372.63f, 248.3506f, 20.0f, 15.0f, 0.0f, ePoints::Point_JumpOnWallEnd);
                        }, 9000);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_JumpOnWallEnd:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetFacingTo(4.572762f);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        }, 2000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetFacingTo(4.066617f);
                            me->GetMotionMaster()->MoveJump(2559.58f, 2398.67f, 235.88f, 10.0f, 10.0f, 0.0f, ePoints::Point_JumpFromWallEnd);
                        }, 5000);

                        break;
                    }
                    case ePoints::Point_JumpFromWallEnd:
                    {
                        G3D::Vector3 const Path_Cullen[9] =
                        {
                            { 2563.748f, 2412.885f, 235.8928f },
                            { 2569.774f, 2418.151f, 235.5178f },
                            { 2575.435f, 2416.228f, 235.1113f },
                            { 2580.629f, 2414.583f, 234.5782f },
                            { 2593.427f, 2409.095f, 234.2132f },
                            { 2597.717f, 2406.953f, 233.7032f },
                            { 2604.729f, 2407.482f, 233.1783f },
                            { 2608.729f, 2407.482f, 232.9283f },
                            { 2611.641f, 2407.436f, 232.0141f }
                        };

                        DoCast(eSpells::Spell_Stealth);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_StealthMoveEnd, Path_Cullen, 9, true);
                        break;
                    }
                    case ePoints::Point_StealthMoveEnd:
                    {
                        me->DespawnOrUnsummon();
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dread_rider_cullen_92566AI(p_Creature);
        }
};

/// Grappling Hook Bunny - 108402
class npc_grappling_hook_bunny_108402 : public CreatureScript
{
    public:
        npc_grappling_hook_bunny_108402() : CreatureScript("npc_grappling_hook_bunny_108402") { }

        struct npc_grappling_hook_bunny_108402AI : public ScriptedAI
        {
            npc_grappling_hook_bunny_108402AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_JumpEnd = 1
            };

            enum eSpells
            {
                Spell_GrapplingHookVisual = 181178
            };

            enum eNpcs
            {
                Npc_Cullen = 92566
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (Unit* l_Unit = p_Summoner->GetAnyOwner())
                {
                    me->AddPlayerInPersonnalVisibilityList(l_Unit->GetGUID());
                    me->SetGuidValue(UNIT_FIELD_DEMON_CREATOR, l_Unit->GetGUID());
                    l_Unit->UpdateObjectVisibility(false);
                }

                DoCast(eSpells::Spell_GrapplingHookVisual, true);

                if (p_Summoner->GetEntry() == eNpcs::Npc_Cullen)
                {
                    me->GetMotionMaster()->MoveJump(2552.642f, 2372.63f, 248.3506f, 20.0f, 15.0f, 0.0f, ePoints::Point_JumpEnd);
                }
                else
                {
                    me->GetMotionMaster()->MoveJump(2668.163f, 2545.227f, 238.7137f, 20.0f, 15.0f, 0.0f, ePoints::Point_JumpEnd);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_grappling_hook_bunny_108402AI(p_Creature);
        }
};

/// Grappling Hook Bunny - 91971
class npc_grappling_hook_bunny_91971 : public CreatureScript
{
    public:
        npc_grappling_hook_bunny_91971() : CreatureScript("npc_grappling_hook_bunny_91971") { }

        struct npc_grappling_hook_bunny_91971AI : public ScriptedAI
        {
            npc_grappling_hook_bunny_91971AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_JumpEnd = 1
            };

            enum eSpells
            {
                Spell_GrapplingHook = 182468,
                Spell_GrapplingGun  = 182470
            };

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, me->GetOwnerGUID());
                if (!l_Player)
                    return;

                switch (p_PointId)
                {
                    case ePoints::Point_JumpEnd:
                    {
                        me->RemoveAura(eSpells::Spell_GrapplingHook);
                        l_Player->CastSpell(me, eSpells::Spell_GrapplingGun, true);
                        l_Player->GetMotionMaster()->MoveJump(me->GetPosition(), 20.0f, 15.0f, ePoints::Point_JumpEnd);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_grappling_hook_bunny_91971AI(p_Creature);
        }
};

/// Dread-Rider Cullen - 92567
class npc_dread_rider_cullen_92567 : public CreatureScript
{
    public:
        npc_dread_rider_cullen_92567() : CreatureScript("npc_dread_rider_cullen_92567") { }

        struct npc_dread_rider_cullen_92567AI : public ScriptedAI
        {
            npc_dread_rider_cullen_92567AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ToWeatherTheStorm = 38614
            };

            enum eObjectives
            {
                Obj_DreadRiderCullenLineHeard = 3861400
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && l_Player->GetQuestStatus(eQuests::Quest_ToWeatherTheStorm) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_DreadRiderCullenLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_DreadRiderCullenLineHeard);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dread_rider_cullen_92567AI(p_Creature);
        }
};

/// Dread-Rider Cullen - 92573
class npc_dread_rider_cullen_92573 : public CreatureScript
{
    public:
        npc_dread_rider_cullen_92573() : CreatureScript("npc_dread_rider_cullen_92573") { }

        struct npc_dread_rider_cullen_92573AI : public ScriptedAI
        {
            npc_dread_rider_cullen_92573AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ImpalementInsurance = 38615
            };

            enum eObjectives
            {
                Obj_CullenLineHeard  = 3861501,
                Obj_SkovaldSceneSeen = 3861500
            };

            enum eSpells
            {
                Spell_SummonScout = 183448
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 10.0f && l_Player->GetQuestStatus(eQuests::Quest_ImpalementInsurance) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CullenLineHeard) == 0 && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SkovaldSceneSeen) == 1)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_CullenLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_SummonScout, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dread_rider_cullen_92573AI(p_Creature);
        }
};

/// Vaengir - 91555
class npc_vaengir_91555 : public CreatureScript
{
    public:
        npc_vaengir_91555() : CreatureScript("npc_vaengir_91555") { }

        struct npc_vaengir_91555AI : public VehicleAI
        {
            npc_vaengir_91555AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_ExplodingBridge = 191819
            };

            enum ePoints
            {
                Point_FlyFromUnderBridgeEnd = 1,
                Point_FlyAwayEnd            = 2
            };

            enum eNpcs
            {
                Npc_Cullen  = 92573,
                Npc_Crowley = 91553
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->ToPlayer())
                    return;

                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                me->setFaction(16);

                if (Vehicle* l_Vehicle = me->GetVehicleKit())
                {
                    if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                    {
                        l_Unit->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                        l_Unit->SetGuidValue(UNIT_FIELD_DEMON_CREATOR, p_Summoner->GetGUID());
                        p_Summoner->UpdateObjectVisibility(false);
                    }
                }

                me->AddDelayedEvent([this]() -> void
                {
                    me->CastSpell(me, eSpells::Spell_ExplodingBridge, true);
                }, 300);

                me->AddDelayedEvent([this]() -> void
                {
                    me->CastSpell(me, eSpells::Spell_ExplodingBridge, true);
                }, 500);

                me->AddDelayedEvent([this]() -> void
                {
                    me->CastSpell(me, eSpells::Spell_ExplodingBridge, true);
                }, 800);

                me->AddDelayedEvent([this]() -> void
                {
                    Position const l_FlyPos = { 2346.63f, 2254.67f, 283.6079f };
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromUnderBridgeEnd, l_FlyPos);
                }, 900);

                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_Vaengir[4] =
                    {
                        { 2342.347f, 2221.24f, 291.0988f },
                        { 2359.243f, 2164.642f, 291.0988f },
                        { 2310.46f, 2081.58f, 291.0988f },
                        { 2225.215f, 2039.144f, 190.4943f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyAwayEnd, Path_Vaengir, 4);
                }, 20000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FlyAwayEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vaengir_91555AI(p_Creature);
        }
};

/// Dread-Rider Cullen - 92569
class npc_dread_rider_cullen_92569 : public CreatureScript
{
public:
    npc_dread_rider_cullen_92569() : CreatureScript("npc_dread_rider_cullen_92569") { }

    struct npc_dread_rider_cullen_92569AI : public ScriptedAI
    {
        npc_dread_rider_cullen_92569AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eQuests
        {
            Quest_AnotherWay = 38617,
            Quest_AboveTheWinterMoonlight = 38618
        };

        enum eObjectives
        {
            Obj_GunnlaugScaleheartSlain = 278227,
            Obj_ClimbTheTower = 278228
        };

        enum ePoints
        {
            Point_MoveEnd = 1
        };

        ObjectGuid m_SummonerGuid;
        bool m_GunnlaugScaleheartSlain = false;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
        }

        void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            me->SetWalk(false);
            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveEnd, 2341.517f, 2368.25f, 316.9446f);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            if (p_PointId == ePoints::Point_MoveEnd)
            {
                me->SetFacingTo(3.560472f);
                Talk(1, m_SummonerGuid);
                me->DespawnOrUnsummon(120000);
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player || !l_Player->IsInWorld())
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (l_Player->GetQuestStatus(eQuests::Quest_AnotherWay) == QUEST_STATUS_INCOMPLETE &&
                l_Player->GetQuestObjectiveCounter(eObjectives::Obj_GunnlaugScaleheartSlain) == 1 && !m_GunnlaugScaleheartSlain)
            {
                Talk(0, m_SummonerGuid);
                m_GunnlaugScaleheartSlain = true;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_dread_rider_cullen_92569AI(p_Creature);
    }
};

/// Thrymjaris - 91570
class npc_thrymjaris_91570 : public CreatureScript
{
    public:
        npc_thrymjaris_91570() : CreatureScript("npc_thrymjaris_91570") { }

        struct npc_thrymjaris_91570AI : public ScriptedAI
        {
            npc_thrymjaris_91570AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Thrymjaris[3] =
            {
                { 2020.757f, 2615.29f,  487.4648f },
                { 2007.141f, 2591.214f, 491.4424f },
                { 2008.701f, 2558.05f,  492.4952f }
            };

            enum ePoints
            {
                Point_GroundMoveEnd = 1,
                Point_FlyUpEnd      = 2,
                Point_FlyPathEnd    = 3
            };

            ObjectGuid m_SummonedGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonedGuid = p_Summoner->GetGUID();
                uint64 l_SumGuid = m_SummonedGuid;

                me->AddDelayedEvent([this, l_SumGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SumGuid))
                    {
                        Talk(0, l_SumGuid);
                    }

                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_GroundMoveEnd, 2025.85f, 2619.62f, 467.632f);
                }, 2000);

                me->AddDelayedEvent([this]() -> void
                {
                    Position const l_FlyPos = { 2025.85f, 2619.62f, 477.632f };
                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_FlyPos);
                }, 9000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonedGuid);
                if (!l_Player)
                    return;

                switch (p_PointId)
                {
                    case ePoints::Point_FlyUpEnd:
                    {
                        Talk(1, m_SummonedGuid);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Thrymjaris, 3);
                        break;
                    }
                    case ePoints::Point_FlyPathEnd:
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
            return new npc_thrymjaris_91570AI(p_Creature);
        }
};

/// Vethir - 96465
class npc_vethir_96465 : public CreatureScript
{
    public:
        npc_vethir_96465() : CreatureScript("npc_vethir_96465") { }

        struct npc_vethir_96465AI : public VehicleAI
        {
            npc_vethir_96465AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum ePoints
            {
                Point_FlyPathEnd = 1
            };

            enum eSpells
            {
                Spell_RideVehicle              = 52391,
                Spell_ConversationVethirFlavor = 215914
            };

            enum eKillcredits
            {
                Killcredit_FlyToTheTopOfThorimsPeak = 96466
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();

                uint64 l_PlayerGuid = m_SummonerGuid;

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                p_Summoner->AddDelayedEvent([p_Summoner]() -> void
                {
                    p_Summoner->CastSpell(p_Summoner, eSpells::Spell_ConversationVethirFlavor, true);
                }, 4000);

                me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                        l_Player->CastSpell(me, eSpells::Spell_RideVehicle, true);
                }, 2000);

                me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                {
                    G3D::Vector3 const Path_Vethir[8] =
                    {
                        { 2007.181f, 2538.161f, 505.5903f },
                        { 2034.651f, 2530.503f, 551.3307f },
                        { 2066.264f, 2530.153f, 569.8964f },
                        { 2098.396f, 2543.869f, 605.7377f },
                        { 2117.002f, 2557.601f, 635.6623f },
                        { 2121.924f, 2597.49f,  657.6256f },
                        { 2093.825f, 2632.592f, 679.9068f },
                        { 2080.953f, 2639.094f, 675.9411f }
                    };

                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Vethir, 8);
                }, 3500);
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                me->DestroyForPlayer(p_Player);
                p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FlyPathEnd)
                {
                    me->SetFacingTo(3.612832f);
                    me->SetAIAnimKitId(7763);

                    if (me->GetVehicleKit())
                    {
                        me->GetVehicleKit()->RemoveAllPassengers();
                        me->DespawnOrUnsummon();
                    }

                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FlyToTheTopOfThorimsPeak);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vethir_96465AI(p_Creature);
        }
};

/// Thrymjaris - 97061
class npc_thrymjaris_97061 : public CreatureScript
{
    public:
        npc_thrymjaris_97061() : CreatureScript("npc_thrymjaris_97061") { }

        struct npc_thrymjaris_97061AI : public ScriptedAI
        {
            npc_thrymjaris_97061AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_DragonBreath = 191773
            };

            enum eKillcredits
            {
                Killcredit_FlyToTheTopOfThorimsPeak = 96467
            };

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                me->DestroyForPlayer(p_Player);

                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_PlayerGuid = p_Player->GetGUID();

                    l_Creature->AI()->Talk(0, l_PlayerGuid);

                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Creature->AI()->Talk(1, l_PlayerGuid);
                    }, 9000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                        {
                            l_Creature->AI()->Talk(2, l_PlayerGuid);
                            l_Creature->SetFacingToObject(l_Player);
                        }

                        l_Creature->CastSpell(l_Creature, eSpells::Spell_DragonBreath, true);
                    }, 16000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FlyToTheTopOfThorimsPeak);
                        l_Creature->DespawnOrUnsummon();
                    }, 26000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_thrymjaris_97061AI(p_Creature);
        }
};

/// Vethir - 92302
class npc_vethir_92302 : public CreatureScript
{
    public:
        npc_vethir_92302() : CreatureScript("npc_vethir_92302") { }

        struct npc_vethir_92302AI : public VehicleAI
        {
            npc_vethir_92302AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_VethirToHrydshal[20] =
            {
                { 2078.307f, 2636.052f, 687.1846f },
                { 2062.227f, 2600.412f, 678.063f  },
                { 2052.997f, 2583.832f, 641.521f  },
                { 2003.519f, 2546.639f, 555.2606f },
                { 2027.012f, 2459.842f, 548.5745f },
                { 2040.259f, 2411.24f,  550.9505f },
                { 2072.405f, 2344.983f, 550.9505f },
                { 2131.889f, 2334.517f, 550.9505f },
                { 2192.372f, 2373.42f,  543.0538f },
                { 2169.538f, 2463.353f, 519.4471f },
                { 2224.958f, 2533.388f, 528.8968f },
                { 2286.609f, 2530.392f, 526.0931f },
                { 2251.092f, 2457.405f, 480.4201f },
                { 2240.877f, 2396.472f, 454.7444f },
                { 2222.651f, 2315.009f, 428.4316f },
                { 2208.137f, 2205.116f, 387.1461f },
                { 2293.712f, 2162.01f,  368.7104f },
                { 2370.762f, 2167.575f, 330.5749f },
                { 2441.507f, 2264.42f,  323.3042f },
                { 2455.257f, 2349.924f, 323.8041f }
            };

            G3D::Vector3 const Path_VethirCycleOnHrydshal[10] =
            {
                { 2447.208f, 2420.543f, 330.0511f },
                { 2424.219f, 2521.253f, 330.0511f },
                { 2467.566f, 2611.861f, 330.0511f },
                { 2509.169f, 2634.241f, 330.0511f },
                { 2600.878f, 2627.521f, 292.8448f },
                { 2633.832f, 2509.883f, 271.4635f },
                { 2645.7f,   2441.297f, 276.1383f },
                { 2599.433f, 2398.005f, 298.0257f },
                { 2519.639f, 2378.281f, 318.0031f },
                { 2479.516f, 2394.118f, 330.0511f }
            };

            G3D::Vector3 const Path_VethirLeaveHrydshal[6] =
            {
                { 2740.932f, 2588.307f, 293.2005f },
                { 2857.542f, 2544.175f, 278.2764f },
                { 2958.264f, 2447.842f, 274.2056f },
                { 3052.188f, 2347.637f, 255.7774f },
                { 3075.675f, 2304.932f, 240.893f  },
                { 3077.25f,  2280.743f, 227.1357f }
            };

            enum eSpells
            {
                Spell_RideVehicle  = 52391,
                Spell_ThundersBite = 183058,
                Spell_RideDragon   = 182998
            };

            enum eKillcredits
            {
                Killcredit_BeginTheAssaultOnHrydshal = 91767,
                Killcredit_LeaveHrydshal             = 91768
            };

            enum ePoints
            {
                Point_PathToHrydshalEnd   = 1,
                Point_PathOnHrydshalEnd   = 2,
                Point_PathFromHrydshalEnd = 3
            };

            enum eObjectives
            {
                Obj_GodKingsMinions = 278236
            };

            enum eQuests
            {
                Quest_CryThunder = 38624
            };

            enum eNpcs
            {
                Npc_DrekirjarGaleborn = 92312
            };

            ObjectGuid m_SummonerGuid;
            bool m_CanLeaveHrydshal = false;
            bool m_PlayerEntered;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->ToPlayer())
                    return;

                m_SummonerGuid = p_Summoner->GetGUID();
                p_Summoner->CastSpell(me, eSpells::Spell_RideVehicle, true);
                p_Summoner->ToPlayer()->KilledMonsterCredit(Killcredit_BeginTheAssaultOnHrydshal);
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathToHrydshalEnd, Path_VethirToHrydshal, 20);
                m_PlayerEntered = true;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (p_Caster->GetEntry() == eNpcs::Npc_DrekirjarGaleborn && p_Spell->Id == eSpells::Spell_RideDragon)
                {
                    Talk(1, m_SummonerGuid);
                }
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (spell->Id == eSpells::Spell_ThundersBite)
                {
                    if (roll_chance_i(5))
                        Talk(0, m_SummonerGuid);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_PathToHrydshalEnd:
                    {
                        if (!m_CanLeaveHrydshal)
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathOnHrydshalEnd, Path_VethirCycleOnHrydshal, 10);
                        break;
                    }
                    case ePoints::Point_PathOnHrydshalEnd:
                    {
                        if (!m_CanLeaveHrydshal)
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathOnHrydshalEnd, Path_VethirCycleOnHrydshal, 10);
                        break;
                    }
                    case ePoints::Point_PathFromHrydshalEnd:
                    {
                        me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            Talk(2, m_SummonerGuid);
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_LeaveHrydshal);
                        }

                        me->AddDelayedEvent([this]() -> void
                        {
                            if (me->GetVehicleKit())
                            {
                                me->GetVehicleKit()->RemoveAllPassengers();
                                me->DespawnOrUnsummon();
                            }
                        }, 3000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_CryThunder) ||
                    (m_PlayerEntered && !l_Player->IsOnVehicle()))
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_GodKingsMinions) == 150 && !m_CanLeaveHrydshal)
                {
                    m_CanLeaveHrydshal = true;
                    me->SetSpeed(MOVE_FLIGHT, 2.0f);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathFromHrydshalEnd, Path_VethirLeaveHrydshal, 6);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vethir_92302AI(p_Creature);
        }
};

/// Havi - 92539
class npc_havi_92539 : public CreatureScript
{
    public:
        npc_havi_92539() : CreatureScript("npc_havi_92539") { }

        struct npc_havi_92539AI : public ScriptedAI
        {
            npc_havi_92539AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_PathToCampfireEnd         = 1,
                Point_PathFromCampfirEnd        = 2,
                Point_SecondPathToCampfireEnd   = 3,
                Point_SecondPathFromCampfireEnd = 4
            };

            enum eQuests
            {
                Quest_SpeakingOfBones = 39804,
                Quest_AHeavyBurden    = 40078
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Quest->GetQuestId() == eQuests::Quest_SpeakingOfBones)
                {
                    uint64 l_PlayerGuid = p_Player->GetGUID();

                    ChangeHidingForPlayer(me, l_PlayerGuid, true);

                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            G3D::Vector3 const Path_HaviToCampfire[3] =
                            {
                                { 3209.066f, 1539.285f, 180.4697f },
                                { 3203.256f, 1534.458f, 180.6413f },
                                { 3198.236f, 1526.384f, 180.7871f }
                            };

                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Creature->AI()->Talk(0, l_PlayerGuid);

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToCampfireEnd, Path_HaviToCampfire, 3, true);
                        }, 1000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid, this]() -> void
                        {
                            if (me)
                                ChangeHidingForPlayer(me, l_PlayerGuid, false);
                            l_Creature->DespawnOrUnsummon();
                        }, 40000);
                    }
                }
                else if (p_Quest->GetQuestId() == eQuests::Quest_AHeavyBurden)
                {
                    uint64 l_PlayerGuid = p_Player->GetGUID();

                    ChangeHidingForPlayer(me, l_PlayerGuid, true);

                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            G3D::Vector3 const Path_HaviToCampfire[9] =
                            {
                                { 3211.513f, 1541.556f, 180.7802f },
                                { 3211.013f, 1539.556f, 180.7802f },
                                { 3211.013f, 1538.556f, 180.7802f },
                                { 3211.013f, 1537.556f, 180.7802f },
                                { 3210.513f, 1533.306f, 180.7802f },
                                { 3208.263f, 1531.306f, 180.7802f },
                                { 3204.513f, 1525.306f, 180.7802f },
                                { 3204.513f, 1516.806f, 181.0302f },
                                { 3201.65f, 1516.5f,    180.7622f }
                            };

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_SecondPathToCampfireEnd, Path_HaviToCampfire, 9, true);
                        }, 1000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Creature->AI()->Talk(4, l_PlayerGuid);
                        }, 2000);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid, this]() -> void
                        {
                            if (me)
                                ChangeHidingForPlayer(me, l_PlayerGuid, false);
                            l_Creature->DespawnOrUnsummon();
                        }, 48000);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_PathToCampfireEnd:
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        me->AI()->Talk(1, m_SummonerGuid);

                    uint64 l_PlayerGuid = m_SummonerGuid;

                    me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            me->AI()->Talk(2, l_PlayerGuid);
                    }, 6000);

                    me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                    {
                        G3D::Vector3 const Path_HaviFromCampfire[2] =
                        {
                            { 3206.388f, 1534.967f, 180.5058f },
                            { 3214.54f,  1543.55f,  180.7244f }
                        };

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            me->AI()->Talk(3, l_PlayerGuid);

                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathFromCampfirEnd, Path_HaviFromCampfire, 2, true);
                    }, 18000);

                    break;
                }
                case ePoints::Point_PathFromCampfirEnd:
                {
                    me->SetFacingTo(2.670354f);
                    break;
                }
                case ePoints::Point_SecondPathToCampfireEnd:
                {
                    uint64 l_PlayerGuid = m_SummonerGuid;

                    me->SetFacingTo(2.513274f);

                    me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            me->AI()->Talk(5, l_PlayerGuid);
                    }, 1000);

                    me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            me->AI()->Talk(6, l_PlayerGuid);
                    }, 9000);

                    me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                    {
                        G3D::Vector3 const Path_HaviFromCampfire[9] =
                        {
                            { 3204.5f,  1516.945f, 180.9519f },
                            { 3208.25f, 1523.195f, 180.7019f },
                            { 3210.5f,  1525.195f, 180.7019f },
                            { 3210.5f,  1529.445f, 180.7019f },
                            { 3210.5f,  1533.445f, 180.7019f },
                            { 3211.0f,  1537.445f, 180.7019f },
                            { 3211.0f,  1538.445f, 180.7019f },
                            { 3211.0f,  1539.695f, 180.7019f },
                            { 3213.85f, 1543.89f,  180.6416f }
                        };

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            me->AI()->Talk(7, l_PlayerGuid);

                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_SecondPathFromCampfireEnd, Path_HaviFromCampfire, 9, true);
                    }, 18000);

                    break;
                }
                case ePoints::Point_SecondPathFromCampfireEnd:
                {
                    me->SetFacingTo(3.071779f);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_havi_92539AI(p_Creature);
        }
};

/// Vydhar - 93231
class npc_vydhar_93231 : public CreatureScript
{
    public:
        npc_vydhar_93231() : CreatureScript("npc_vydhar_93231") { }

        struct npc_vydhar_93231AI : public ScriptedAI
        {
            npc_vydhar_93231AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ToHaustvald         = 39796,
                Quest_TheRunewoodsRevenge = 39788,
                Quest_TurnTheKeys         = 38778
            };

            enum eObjectives
            {
                Obj_VydharLineHeard  = 3979600,
                Obj_VydharLinesHeard = 3978800
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                uint64 l_PlayerGuid = l_Player->GetGUID();

                if (me->GetExactDist2d(l_Player) < 20.0f && l_Player->HasQuest(eQuests::Quest_ToHaustvald) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_VydharLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_VydharLineHeard);
                }

                if (me->GetExactDist2d(l_Player) < 20.0f && l_Player->GetQuestStatus(eQuests::Quest_TheRunewoodsRevenge) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestStatus(eQuests::Quest_TurnTheKeys) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_VydharLinesHeard) == 0)
                {
                    Talk(1, l_PlayerGuid);

                    me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                        {
                            Talk(2, l_Player->GetGUID());
                        }
                    }, 4000);

                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_VydharLinesHeard);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vydhar_93231AI(p_Creature);
        }
};

/// Runestone - 93182, 93343, 93342
class npc_stormheim_runestones : public CreatureScript
{
    public:
        npc_stormheim_runestones() : CreatureScript("npc_stormheim_runestones") { }

        struct npc_stormheim_runestonesAI : public ScriptedAI
        {
            npc_stormheim_runestonesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_TurnRunestone                = 184579,
                Spell_ActivateRunestoneVisual      = 216204,
                Spell_RunestoneActivatedKillcredit = 184585
            };

            enum eKillcredits
            {
                Killcredit_FirstRunestoneActivated  = 93185,
                Killcredit_SecondRunestoneActivated = 93341,
                Killcredit_ThirdRunestoneActivated  = 93340,
            };

            enum eNpcs
            {
                Npc_FirstRunestone  = 93182,
                Npc_SecondRunestone = 93343,
                Npc_ThirdRunestone  = 93342
            };

            enum eConversations
            {
                Conv_FirstRunestoneActivated  = 2586,
                Conv_SecondRunestoneActivated = 258600,
                Conv_ThirdRunestoneActivated  = 258601
            };

            enum eObjectives
            {
                Obj_RunestonesActivated = 278431
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id == eSpells::Spell_TurnRunestone)
                {
                    switch (me->GetEntry())
                    {
                        case eNpcs::Npc_FirstRunestone:
                        {
                            me->DestroyForPlayer(l_Player);

                            if (Creature* l_Creature = p_Caster->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                uint64 l_SummonedGuid = p_Caster->GetGUID();

                                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                l_Creature->GetMotionMaster()->MoveRotate(1700, RotateDirection::ROTATE_DIRECTION_RIGHT);
                                l_Creature->SendPlaySound(7754, true);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 67110912);

                                l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                                    {
                                        l_Player->CastSpell(l_Player, eSpells::Spell_RunestoneActivatedKillcredit);

                                        if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 1)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_FirstRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                        else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 2)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_SecondRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                        else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 3)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_ThirdRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                    }

                                    l_Creature->CastSpell(l_Creature, eSpells::Spell_ActivateRunestoneVisual, true);
                                }, 1700);

                                l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                                    {
                                        l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_FirstRunestoneActivated);
                                    }

                                    l_Creature->DespawnOrUnsummon();
                                }, 6700);
                            }

                            break;
                        }
                        case eNpcs::Npc_SecondRunestone:
                        {
                            me->DestroyForPlayer(l_Player);

                            if (Creature* l_Creature = p_Caster->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                uint64 l_SummonedGuid = p_Caster->GetGUID();

                                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                l_Creature->GetMotionMaster()->MoveRotate(1700, RotateDirection::ROTATE_DIRECTION_RIGHT);
                                l_Creature->SendPlaySound(7754, true);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 67110912);

                                l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                                    {
                                        l_Player->CastSpell(l_Player, eSpells::Spell_RunestoneActivatedKillcredit);

                                        if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 1)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_FirstRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                        else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 2)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_SecondRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                        else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 3)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_ThirdRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                    }

                                    l_Creature->CastSpell(l_Creature, eSpells::Spell_ActivateRunestoneVisual, true);
                                }, 1700);

                                l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                                    {
                                        l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_SecondRunestoneActivated);
                                    }

                                    l_Creature->DespawnOrUnsummon();
                                }, 6700);
                            }

                            break;
                        }
                        case eNpcs::Npc_ThirdRunestone:
                        {
                            me->DestroyForPlayer(l_Player);

                            if (Creature* l_Creature = p_Caster->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                uint64 l_SummonedGuid = p_Caster->GetGUID();

                                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                l_Creature->GetMotionMaster()->MoveRotate(1700, RotateDirection::ROTATE_DIRECTION_RIGHT);
                                l_Creature->SendPlaySound(7754, true);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 67110912);

                                l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                                    {
                                        l_Player->CastSpell(l_Player, eSpells::Spell_RunestoneActivatedKillcredit);

                                        if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 1)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_FirstRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                        else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 2)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_SecondRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                        else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RunestonesActivated) == 3)
                                        {
                                            if (Conversation* l_Conversation = new Conversation)
                                            {
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_ThirdRunestoneActivated, l_Player, nullptr, l_Player->GetPosition()))
                                                    delete l_Conversation;
                                            }
                                        }
                                    }

                                    l_Creature->CastSpell(l_Creature, eSpells::Spell_ActivateRunestoneVisual, true);
                                }, 1700);

                                l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                                    {
                                        l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_ThirdRunestoneActivated);
                                    }

                                    l_Creature->DespawnOrUnsummon();
                                }, 6700);
                            }

                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stormheim_runestonesAI(p_Creature);
        }
};

/// Skovald Event Seen Trigger - 3881600
class npc_skovald_event_trigger_3881600 : public CreatureScript
{
    public:
        npc_skovald_event_trigger_3881600() : CreatureScript("npc_skovald_event_trigger_3881600") { }

        struct npc_skovald_event_trigger_3881600AI : public ScriptedAI
        {
            npc_skovald_event_trigger_3881600AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_ConversationGodKingScene = 216972
            };

            enum eObjectives
            {
                Obj_SkovaldEventSeenTrigger = 3881600
            };

            enum eQuests
            {
                Quest_BreakingTheBonespeakers = 38816
            };

            enum eNpcs
            {
                Npc_Vaengir = 109123
            };

            enum ePoints
            {
                Point_VaengirMoveAfterSpawnEnd = 1,
                Point_VaengirLeavePathEnd = 2
            };

            Position const m_VaengirSpawnPos = { 3477.07f, 955.6371f, 162.9627f, 2.510875f };
            Position const m_VaengirFlyPos   = { 3440.91f, 982.042f,  138.1375f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 30.0f && l_Player->HasQuest(eQuests::Quest_BreakingTheBonespeakers) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SkovaldEventSeenTrigger) == 0)
                {
                    if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_Vaengir, m_VaengirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        if (Vehicle* l_Vehicle = l_Creature->GetVehicleKit())
                        {
                            if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                            {
                                l_Unit->AddPlayerInPersonnalVisibilityList(l_Player->GetGUID());
                                l_Unit->SetGuidValue(UNIT_FIELD_DEMON_CREATOR, l_Player->GetGUID());
                                l_Player->UpdateObjectVisibility(false);
                            }
                        }

                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_VaengirMoveAfterSpawnEnd, m_VaengirFlyPos);

                        l_Player->DelayedCastSpell(l_Player, eSpells::Spell_ConversationGodKingScene, true, 5000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            G3D::Vector3 const Path_VaengirLeave[5] =
                            {
                                { 3429.998f, 992.6945f, 159.3074f },
                                { 3403.878f, 1014.714f, 197.9391f },
                                { 3356.386f, 1063.755f, 253.8531f },
                                { 3296.121f, 1136.682f, 288.8789f },
                                { 3266.894f, 1215.797f, 288.8789f }
                            };

                            l_Creature->SetSpeed(MOVE_FLIGHT, 2.0f);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_VaengirLeavePathEnd, Path_VaengirLeave, 5);
                        }, 13000);
                    }

                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_SkovaldEventSeenTrigger);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skovald_event_trigger_3881600AI(p_Creature);
        }
};

/// Bone Effect Bunny - 97281
class npc_bone_effect_bunny_97281 : public CreatureScript
{
    public:
        npc_bone_effect_bunny_97281() : CreatureScript("npc_bone_effect_bunny_97281") { }

        struct npc_bone_effect_bunny_97281AI : public ScriptedAI
        {
            npc_bone_effect_bunny_97281AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_BoneEffect = 192062
            };

            enum eKillcredits
            {
                Killcredit_AshildirReturned = 3881500
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(eSpells::Spell_BoneEffect, false);
                me->DespawnOrUnsummon(5000);

                if (!p_Summoner->ToPlayer())
                    return;

                p_Summoner->AddDelayedEvent([p_Summoner]() -> void
                {
                    p_Summoner->ToPlayer()->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_AshildirReturned);
                }, 5000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_bone_effect_bunny_97281AI(p_Creature);
        }
};

/// Runeseer Faljar - 93093
class npc_runeseer_faljar_93093 : public CreatureScript
{
    public:
        npc_runeseer_faljar_93093() : CreatureScript("npc_runeseer_faljar_93093") { }

        struct npc_runeseer_faljar_93093AI : public ScriptedAI
        {
            npc_runeseer_faljar_93093AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_RuneChannelVisual           = 209164,
                Spell_RunicBulwarkVisual          = 209307,
                Spell_LosConversation             = 209361,
                Spell_ShatteredRune               = 195493,
                Spell_ForbiddenRune               = 209182,
                Spell_RuneChannel                 = 209164,
                Spell_RunicBulwark                = 208544,
                Spell_ActivateRunestone           = 208815,
                Spell_ActivateRunestoneAfterDeath = 208816,
                Spell_RunicBulwarkText            = 209177,
                Spell_SpawnCosmetics              = 209367,
                Spell_ShadowyGhostVisual          = 209381,
                Spell_SigilOfBinding              = 209393,
                Spell_EndConversation             = 209310,
                Spell_RuneseerConfronted          = 185032,
                Spell_Soul                        = 209399
            };

            enum eQuests
            {
                Quest_TheFinalJudgment = 38818
            };

            enum eNpcs
            {
                Npc_EmpoweredRunestone  = 97221,
                Npc_Ashildir            = 93428,
                Npc_BonespeakerDrudge   = 105680,
                Npc_ValkyraShieldmaiden = 105687
            };

            enum eEvents
            {
                Event_CastShatteredRune = 1,
                Event_CastForbiddenRune = 2
            };

            enum ePoints
            {
                Point_FinalMoveEnd  = 1,
                Point_ValkyraFlyEnd = 2
            };

            EventMap m_Events;
            std::vector<ObjectGuid> m_SummonGuids;
            ObjectGuid m_LeftRunestoneGuid;
            ObjectGuid m_RightRunestoneGuid;
            ObjectGuid m_FirstBonespeakerGuid;
            ObjectGuid m_SecondBonespeakerGuid;
            ObjectGuid m_ThirdBonespeakerGuid;
            ObjectGuid m_FourthBonespeakerGuid;
            ObjectGuid m_FirstValkyraGuid;
            ObjectGuid m_SecondValkyraGuid;
            ObjectGuid m_ThirdValkyraGuid;
            ObjectGuid m_FourthValkyraGuid;
            Position m_LeftRunestoneSpawnPos     = { 3655.526f, 784.2031f, 39.12351f, 3.809607f };
            Position m_RightRunestoneSpawnPos    = { 3630.622f, 759.9427f, 38.78061f, 5.054792f };
            Position m_FirstBonespeakerSpawnPos  = { 3655.113f, 794.0608f, 39.10766f, 1.510229f };
            Position m_SecondBonespeakerSpawnPos = { 3637.947f, 791.4202f, 39.10844f, 2.014529f };
            Position m_ThirdBonespeakerSpawnPos  = { 3623.872f, 776.9514f, 39.10868f, 2.928881f };
            Position m_FourthBonespeakerSpawnPos = { 3621.083f, 763.0434f, 39.10814f, 3.146997f };
            Position m_FirstValkyraSpawnPos      = { 3656.21f,  805.8941f, 39.02677f, 4.572102f };
            Position m_SecondValkyraSpawnPos     = { 3634.627f, 810.5504f, 39.76195f, 4.985665f };
            Position m_ThirdValkyraSpawnPos      = { 3599.355f, 773.3386f, 40.82829f, 4.985665f };
            Position m_FourthValkyraSpawnPos     = { 3602.904f, 752.3073f, 41.61448f, 0.3900764f };
            Position m_FinalMovePos              = { 3626.21f,  791.125f,  38.6973f };
            bool m_EventStarted                  = false;
            bool m_FirstRunicBulwarkPhase        = false;
            bool m_SecondRunicBulwarkPhase       = false;
            bool m_ArmiesCalled                  = false;
            bool m_ToHelheim                     = false;

            void ResetVariables()
            {
                m_Events.Reset();
                m_SummonGuids.clear();
                m_LeftRunestoneGuid.Clear();
                m_RightRunestoneGuid.Clear();
                m_FirstBonespeakerGuid.Clear();
                m_SecondBonespeakerGuid.Clear();
                m_ThirdBonespeakerGuid.Clear();
                m_FourthBonespeakerGuid.Clear();
                m_FirstValkyraGuid.Clear();
                m_SecondValkyraGuid.Clear();
                m_ThirdValkyraGuid.Clear();
                m_FourthValkyraGuid.Clear();
                m_EventStarted            = false;
                m_FirstRunicBulwarkPhase  = false;
                m_SecondRunicBulwarkPhase = false;
                m_ArmiesCalled            = false;
                m_ToHelheim               = false;
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_RuneChannelVisual, true);
                DoCast(eSpells::Spell_RunicBulwarkVisual, true);

                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_EmpoweredRunestone, m_LeftRunestoneSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    m_LeftRunestoneGuid = l_Creature->GetGUID();
                }

                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_EmpoweredRunestone, m_RightRunestoneSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    m_RightRunestoneGuid = l_Creature->GetGUID();
                }
            }

            void JustReachedHome() override
            {
                DoCast(eSpells::Spell_RuneChannelVisual, true);
                DoCast(eSpells::Spell_RunicBulwarkVisual, true);

                for (auto itr : m_SummonGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_LeftRunestoneGuid))
                {
                    l_Creature->DespawnOrUnsummon();
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_RightRunestoneGuid))
                {
                    l_Creature->DespawnOrUnsummon();
                }

                ResetVariables();

                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_EmpoweredRunestone, m_LeftRunestoneSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    m_LeftRunestoneGuid = l_Creature->GetGUID();
                }

                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_EmpoweredRunestone, m_RightRunestoneSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    m_RightRunestoneGuid = l_Creature->GetGUID();
                }
            }

            void JustRespawned() override
            {
                DoCast(eSpells::Spell_RuneChannelVisual, true);
                DoCast(eSpells::Spell_RunicBulwarkVisual, true);

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_LeftRunestoneGuid))
                {
                    l_Creature->DespawnOrUnsummon();
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_RightRunestoneGuid))
                {
                    l_Creature->DespawnOrUnsummon();
                }

                ResetVariables();

                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_EmpoweredRunestone, m_LeftRunestoneSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    m_LeftRunestoneGuid = l_Creature->GetGUID();
                }

                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_EmpoweredRunestone, m_RightRunestoneSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    m_RightRunestoneGuid = l_Creature->GetGUID();
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 30.0f && l_Player->HasQuest(eQuests::Quest_TheFinalJudgment) &&
                    !m_EventStarted)
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                    }, 11000);

                    DoCast(eSpells::Spell_LosConversation, true);
                    m_EventStarted = true;
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                me->CastStop();
                me->RemoveAllAuras();
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastShatteredRune, 0);
                m_Events.ScheduleEvent(eEvents::Event_CastForbiddenRune, 15000);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetHealth(1);
                }

                if (me->GetHealthPct() <= 80 && !m_FirstRunicBulwarkPhase)
                {
                    DoCast(eSpells::Spell_RuneChannelVisual, true);

                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_LeftRunestoneGuid))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(l_Creature, eSpells::Spell_RunicBulwark, true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ActivateRunestone, true);
                    }

                    if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_Ashildir, 100.0f, true))
                    {
                        l_Creature->AI()->Talk(0);
                    }

                    m_FirstRunicBulwarkPhase = true;
                }

                if (me->GetHealthPct() <= 30 && !m_SecondRunicBulwarkPhase)
                {
                    DoCast(eSpells::Spell_RuneChannelVisual, true);

                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_RightRunestoneGuid))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(l_Creature, eSpells::Spell_RunicBulwark, true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ActivateRunestone, true);
                    }

                    m_SecondRunicBulwarkPhase = true;
                }

                if (me->GetHealthPct() <= 65 && !m_ArmiesCalled)
                {
                    Talk(2);

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_BonespeakerDrudge, m_FirstBonespeakerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        m_FirstBonespeakerGuid = l_Creature->GetGUID();
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_BonespeakerDrudge, m_SecondBonespeakerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        m_SecondBonespeakerGuid = l_Creature->GetGUID();
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_BonespeakerDrudge, m_ThirdBonespeakerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        m_ThirdBonespeakerGuid = l_Creature->GetGUID();
                    }

                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_BonespeakerDrudge, m_FourthBonespeakerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        m_FourthBonespeakerGuid = l_Creature->GetGUID();
                    }

                    me->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_Ashildir, 100.0f, true))
                        {
                            l_Creature->AI()->Talk(1);
                        }

                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ValkyraShieldmaiden, m_FirstValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (Creature* l_Bonespeaker = ObjectAccessor::GetCreature(*me, m_FirstBonespeakerGuid))
                            {
                                l_Creature->Attack(l_Bonespeaker, false);
                            }

                            m_FirstValkyraGuid = l_Creature->GetGUID();
                        }

                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ValkyraShieldmaiden, m_SecondValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (Creature* l_Bonespeaker = ObjectAccessor::GetCreature(*me, m_SecondBonespeakerGuid))
                            {
                                l_Creature->Attack(l_Bonespeaker, false);
                            }

                            m_SecondValkyraGuid = l_Creature->GetGUID();
                        }

                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ValkyraShieldmaiden, m_ThirdValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (Creature* l_Bonespeaker = ObjectAccessor::GetCreature(*me, m_ThirdBonespeakerGuid))
                            {
                                l_Creature->Attack(l_Bonespeaker, false);
                            }

                            m_ThirdValkyraGuid = l_Creature->GetGUID();
                        }

                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_ValkyraShieldmaiden, m_FourthValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (Creature* l_Bonespeaker = ObjectAccessor::GetCreature(*me, m_FourthBonespeakerGuid))
                            {
                                l_Creature->Attack(l_Bonespeaker, false);
                            }

                            m_FourthValkyraGuid = l_Creature->GetGUID();
                        }
                    }, 6000);

                    m_ArmiesCalled = true;
                }

                if (me->GetHealthPct() <= 10 && !m_ToHelheim)
                {
                    Talk(3);
                    me->RemoveAllAuras();
                    m_Events.Reset();
                    DoCastAOE(eSpells::Spell_SigilOfBinding, true);
                    me->StopAttack();
                    me->DeleteThreatList();
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 559360);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_FinalMoveEnd, m_FinalMovePos, false);
                    m_ToHelheim = true;

                    for (auto itr : m_SummonGuids)
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                        {
                            l_Creature->AttackStop();
                            l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        }
                    }
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == eNpcs::Npc_EmpoweredRunestone)
                {
                    p_Summon->CastSpell(p_Summon, eSpells::Spell_ActivateRunestoneAfterDeath, true);
                    p_Summon->SetFlag(UNIT_FIELD_FLAGS, 34113792);
                    me->RemoveAura(eSpells::Spell_RunicBulwark);
                    me->RemoveAura(eSpells::Spell_RuneChannel);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon->GetEntry() == eNpcs::Npc_EmpoweredRunestone)
                {
                    p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                    p_Summon->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                }
                else if (p_Summon->GetEntry() == eNpcs::Npc_BonespeakerDrudge)
                {
                    p_Summon->CastSpell(p_Summon, eSpells::Spell_SpawnCosmetics, true);
                    m_SummonGuids.push_back(p_Summon->GetGUID());
                }
                else if (p_Summon->GetEntry() == eNpcs::Npc_ValkyraShieldmaiden)
                {
                    p_Summon->CastSpell(p_Summon, eSpells::Spell_ShadowyGhostVisual, true);
                    m_SummonGuids.push_back(p_Summon->GetGUID());
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode /*p_RemoveMode*/) override
            {
                if (p_SpellID == eSpells::Spell_RunicBulwark)
                {
                    me->RemoveAura(eSpells::Spell_RuneChannel);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FinalMoveEnd && p_Type == POINT_MOTION_TYPE)
                {
                    me->SetFacingTo(5.550147f);
                    DoCast(eSpells::Spell_RuneChannelVisual, true);
                    DoCast(eSpells::Spell_RunicBulwarkVisual, true);
                    me->DelayedCastSpell(me, eSpells::Spell_EndConversation, true, 3000);
                    me->DelayedCastSpell(me, eSpells::Spell_RuneseerConfronted, true, 32000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        for (auto itr : m_SummonGuids)
                        {
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                            {
                                l_Creature->DespawnOrUnsummon();
                            }
                        }

                        ResetVariables();
                        me->DespawnOrUnsummon();
                    }, 40000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_FirstValkyraGuid))
                        {
                            G3D::Vector3 const Path_Valkyra[4] =
                            {
                                { 3622.869f, 810.4583f,  52.52684f },
                                { 3606.445f, 822.6077f,  47.79438f },
                                { 3588.304f, 822.0174f,  37.52422f },
                                { 3671.616f, 750.2899f, -12.84349f }
                            };

                            l_Creature->SetSpeed(MOVE_RUN, 3.0f);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_Soul, true);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyraFlyEnd, Path_Valkyra, 4);
                        }

                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_SecondValkyraGuid))
                        {
                            G3D::Vector3 const Path_Valkyra[4] =
                            {
                                { 3622.869f, 810.4583f,  52.52684f },
                                { 3606.445f, 822.6077f,  47.79438f },
                                { 3588.304f, 822.0174f,  37.52422f },
                                { 3671.616f, 750.2899f, -12.84349f }
                            };

                            l_Creature->SetSpeed(MOVE_RUN, 3.0f);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_Soul, true);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyraFlyEnd, Path_Valkyra, 4);
                        }

                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_ThirdValkyraGuid))
                        {
                            G3D::Vector3 const Path_Valkyra[4] =
                            {
                                { 3602.072f, 791.2535f,  46.91028f },
                                { 3576.494f, 804.9583f,  51.01867f },
                                { 3589.067f, 822.7327f,  29.01908f },
                                { 3671.616f, 750.2899f, -12.84349f }
                            };

                            l_Creature->SetSpeed(MOVE_RUN, 3.0f);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_Soul, true);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyraFlyEnd, Path_Valkyra, 4);
                        }

                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_FourthValkyraGuid))
                        {
                            G3D::Vector3 const Path_Valkyra[4] =
                            {
                                { 3622.869f, 810.4583f,  52.52684f },
                                { 3606.445f, 822.6077f,  47.79438f },
                                { 3588.304f, 822.0174f,  37.52422f },
                                { 3671.616f, 750.2899f, -12.84349f }
                            };

                            l_Creature->SetSpeed(MOVE_RUN, 3.0f);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_Soul, true);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyraFlyEnd, Path_Valkyra, 4);
                        }
                    }, 29000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastShatteredRune:
                    {
                        DoCastVictim(eSpells::Spell_ShatteredRune);
                        m_Events.ScheduleEvent(eEvents::Event_CastShatteredRune, urand(6000, 10000));
                        break;
                    }
                    case eEvents::Event_CastForbiddenRune:
                    {
                        DoCastVictim(eSpells::Spell_ForbiddenRune);
                        m_Events.ScheduleEvent(eEvents::Event_CastForbiddenRune, urand(21000, 27000));
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
            return new npc_runeseer_faljar_93093AI(p_Creature);
        }
};

/// Ashildir - 97319
class npc_ashildir_97319 : public CreatureScript
{
    public:
        npc_ashildir_97319() : CreatureScript("npc_ashildir_97319") { }

        struct npc_ashildir_97319AI : public ScriptedAI
        {
            npc_ashildir_97319AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eObjectives
            {
                Obj_AshildirLineHeard = 3881800
            };

            enum eQuests
            {
                Quest_TheFinalJudgment = 38818
            };

            enum ePoints
            {
                Point_PathEnd       = 1,
                Point_FirstJumpEnd  = 2,
                Point_SecondJumpEnd = 3,
                Point_ThirdJumpEnd  = 4
            };

            Position const m_FirstJumpPos  = { 381.625f, 354.679f, 33.333f  };
            Position const m_SecondJumpPos = { 396.733f, 356.366f, 17.0678f };
            Position const m_ThirdJumpPos  = { 407.953f, 330.622f, 4.36825f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 30.0f && l_Player->HasQuest(eQuests::Quest_TheFinalJudgment) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_AshildirLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_AshildirLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Ashildir[11] =
                        {
                            { 345.9887f, 362.9392f, 29.94434f },
                            { 348.4887f, 366.9392f, 29.44434f },
                            { 350.2118f, 369.4566f, 28.92906f },
                            { 357.9567f, 368.3807f, 28.66289f },
                            { 359.4567f, 368.1307f, 28.91289f },
                            { 360.4567f, 368.1307f, 28.66289f },
                            { 362.4567f, 367.6307f, 28.41289f },
                            { 365.2067f, 366.6307f, 28.41289f },
                            { 366.7067f, 366.1307f, 28.41289f },
                            { 367.7847f, 362.8698f, 28.41587f },
                            { 372.6337f, 359.1198f, 28.43772f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathEnd, Path_Ashildir, 11, false);
                    }, 1500);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_PathEnd:
                    {
                        me->GetMotionMaster()->MoveJump(m_FirstJumpPos, 15.0f, 10.0f, ePoints::Point_FirstJumpEnd);
                        break;
                    }
                    case ePoints::Point_FirstJumpEnd:
                    {
                        me->GetMotionMaster()->MoveJump(m_SecondJumpPos, 15.0f, 10.0f, ePoints::Point_SecondJumpEnd);
                        break;
                    }
                    case ePoints::Point_SecondJumpEnd:
                    {
                        me->GetMotionMaster()->MoveJump(m_ThirdJumpPos, 15.0f, 10.0f, ePoints::Point_ThirdJumpEnd);
                        break;
                    }
                    case ePoints::Point_ThirdJumpEnd:
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
            return new npc_ashildir_97319AI(p_Creature);
        }
};

/// Colborn the Unworthy - 91531
class npc_colborn_the_unworthy_91531 : public CreatureScript
{
    public:
        npc_colborn_the_unworthy_91531() : CreatureScript("npc_colborn_the_unworthy_91531") { }

        struct npc_colborn_the_unworthy_91531AI : public ScriptedAI
        {
            npc_colborn_the_unworthy_91531AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eKillcredits
            {
                Killcredit_ColbornMovedFromCage = 3983700
            };

            enum ePoints
            {
                Point_PathEnd          = 1,
                Point_PathToSeaweedEnd = 2,
                Point_PathToGuarmEnd   = 3
            };

            enum eSpells
            {
                Spell_ShadowyGhostVisual     = 211179,
                Spell_MaintainSummonAshildir = 192299,
                Spell_KvaldirDisguise        = 181705
            };

            enum eQuests
            {
                Quest_AnUnworthyTask   = 39837,
                Quest_StealthBySeaweed = 38347
            };

            ObjectGuid m_SummonerGuid;

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Quest->GetQuestId() == eQuests::Quest_StealthBySeaweed)
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_MaintainSummonAshildir, true);
                    p_Player->CastSpell(p_Player, eSpells::Spell_KvaldirDisguise, true);

                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            G3D::Vector3 const Path_Colborn[2] =
                            {
                                { 368.3004f, 365.6339f, 28.32065f },
                                { 367.606f,  365.056f,  28.10019f }
                            };

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToSeaweedEnd, Path_Colborn, 2, true);
                        }, 1000);
                    }
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->ToPlayer())
                    return;

                DoCast(eSpells::Spell_ShadowyGhostVisual, true);
                m_SummonerGuid = p_Summoner->GetGUID();

                if (p_Summoner->ToPlayer()->HasQuest(eQuests::Quest_AnUnworthyTask))
                {
                    uint64 l_SummonerGuid = m_SummonerGuid;
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                        {
                            Talk(0, l_SummonerGuid);
                        }

                        G3D::Vector3 const Path_Colborn[22] =
                        {
                            { 374.5938f, 410.0651f, 19.54136f },
                            { 375.3438f, 407.3151f, 20.04136f },
                            { 375.8629f, 404.7726f, 20.11307f },
                            { 375.04f,   402.1028f, 21.23714f },
                            { 373.79f,   398.3528f, 21.98714f },
                            { 372.8945f, 395.2227f, 22.37006f },
                            { 369.298f,  393.6701f, 23.35698f },
                            { 366.798f,  392.4201f, 23.85698f },
                            { 363.298f,  390.9201f, 24.60698f },
                            { 361.625f,  389.9375f, 24.70432f },
                            { 360.5975f, 387.46f,   25.47046f },
                            { 358.3475f, 382.96f,   26.47046f },
                            { 357.0365f, 380.7431f, 26.60826f },
                            { 360.0174f, 374.7309f, 27.22202f },
                            { 362.767f,  372.2488f, 27.90119f },
                            { 363.767f,  370.9988f, 27.65119f },
                            { 366.767f,  369.4988f, 27.90119f },
                            { 367.767f,  369.4988f, 27.65119f },
                            { 368.767f,  369.4988f, 27.65119f },
                            { 369.767f,  369.4988f, 27.65119f },
                            { 370.767f,  369.4988f, 27.40119f },
                            { 370.9462f, 368.1424f, 27.38044f }
                        };

                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathEnd, Path_Colborn, 22, false);
                    }, 4000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_PathEnd:
                {
                    me->SetFacingTo(2.879793f);
                    me->SetAIAnimKitId(1093);
                    me->DespawnOrUnsummon(4000);

                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        l_Player->AddDelayedEvent([l_Player]() -> void
                        {
                            l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_ColbornMovedFromCage);
                        }, 2000);
                    }

                    break;
                }
                case ePoints::Point_PathToSeaweedEnd:
                {
                    me->SetFacingTo(0.8377581f);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LOOT);

                    me->AddDelayedEvent([this]() -> void
                    {
                        G3D::Vector3 const Path_Colborn[36] =
                        {
                            { 360.6242f, 372.1461f, 27.77787f },
                            { 360.1242f, 372.6461f, 27.77787f },
                            { 356.8742f, 378.8961f, 27.52787f },
                            { 356.1424f, 379.7361f, 26.95555f },
                            { 358.5044f, 382.0158f, 26.65861f },
                            { 359.7544f, 383.5158f, 25.90861f },
                            { 363.2544f, 387.0158f, 25.40861f },
                            { 365.7544f, 390.0158f, 24.40861f },
                            { 366.6076f, 391.0052f, 23.79783f },
                            { 369.6638f, 392.6984f, 23.62192f },
                            { 371.9138f, 393.9484f, 22.87192f },
                            { 372.4138f, 394.1984f, 22.62192f },
                            { 373.9138f, 397.6984f, 22.12192f },
                            { 375.1638f, 400.4484f, 21.62192f },
                            { 376.1638f, 403.1984f, 20.87192f },
                            { 377.2969f, 405.7379f, 19.88138f },
                            { 378.6052f, 408.8943f, 20.00038f },
                            { 379.6052f, 409.8943f, 19.50038f },
                            { 380.1052f, 412.3943f, 19.25038f },
                            { 382.4896f, 414.1858f, 18.42227f },
                            { 387.1262f, 415.838f, 18.03689f },
                            { 389.8455f, 416.8802f, 17.10733f },
                            { 390.1562f, 417.0072f, 17.29533f },
                            { 393.4062f, 419.5072f, 16.54533f },
                            { 395.6562f, 421.2572f, 16.04533f },
                            { 398.6562f, 423.5072f, 15.54533f },
                            { 401.1562f, 425.2572f, 14.79533f },
                            { 404.4062f, 427.5072f, 14.04533f },
                            { 407.4549f, 429.7795f, 13.14984f },
                            { 412.3967f, 429.563f, 12.62284f },
                            { 416.3967f, 429.563f, 12.12284f },
                            { 419.3967f, 429.313f, 11.62284f },
                            { 421.9844f, 429.3351f, 10.7923f },
                            { 426.8819f, 429.5646f, 10.60425f },
                            { 431.8819f, 429.5646f, 9.854248f },
                            { 432.8924f, 429.4097f, 9.358336f }
                        };

                        me->CastSpell(me, eSpells::Spell_KvaldirDisguise, true);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToGuarmEnd, Path_Colborn, 36, false);
                    }, 3000);

                    break;
                }
                case ePoints::Point_PathToGuarmEnd:
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
            return new npc_colborn_the_unworthy_91531AI(p_Creature);
        }
};

/// Ashildir - 97419
class npc_ashildir_97419 : public CreatureScript
{
    public:
        npc_ashildir_97419() : CreatureScript("npc_ashildir_97419") { }

        struct npc_ashildir_97419AI : public ScriptedAI
        {
            npc_ashildir_97419AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Ashildir[6] =
            {
                { 441.1747f, 649.4187f, 5.669166f  },
                { 425.6747f, 654.9187f, 5.669166f  },
                { 410.9247f, 656.1687f, 4.919166f  },
                { 397.6747f, 654.9187f, 1.919166f  },
                { 382.9247f, 654.6687f, 0.6691661f },
                { 377.1649f, 652.9323f, 0.1079441f }
            };

            enum eSpells
            {
                Spell_KvaldirDisguise              = 181705,
                Spell_RemoveKvaldirDisguise        = 182109,
                Spell_CancelMaintainSummonAshildir = 192518,
                Spell_HelirasGazeFaction           = 181709
            };

            enum eQuests
            {
                Quest_StealthBySeaweed  = 38347,
                Quest_ADesperateBargain = 39848
            };

            enum eKillcredits
            {
                Killcredit_SneakPastGuarm = 91386
            };

            enum ePoints
            {
                Point_MoveAwayEnd = 1
            };

            ObjectGuid m_SummonerGuid;
            Position const m_SneakedPastTriggerPos = { 455.185791f, 579.037842f, 0.268828f };

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveAwayEnd, Path_Ashildir, 6, false);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->ToPlayer())
                    return;

                me->DelayedCastSpell(me, eSpells::Spell_KvaldirDisguise, true, 500);
                Talk(0, p_Summoner->GetGUID());
                m_SummonerGuid = p_Summoner->GetGUID();
                me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveAwayEnd)
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_CancelMaintainSummonAshildir, true);
                    }

                    me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (me->GetExactDist2d(&m_SneakedPastTriggerPos) <= 20.0f && l_Player->GetQuestStatus(eQuests::Quest_StealthBySeaweed) == QUEST_STATUS_INCOMPLETE)
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SneakPastGuarm);
                    l_Player->CastSpell(l_Player, eSpells::Spell_RemoveKvaldirDisguise, true);
                    l_Player->RemoveAura(eSpells::Spell_HelirasGazeFaction);
                    DoCast(eSpells::Spell_RemoveKvaldirDisguise, true);
                    Talk(1, m_SummonerGuid);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashildir_97419AI(p_Creature);
        }
};

/// Lady Sylvanas Windrunner - 97695
class npc_lady_sylvanas_windrunner_97695 : public CreatureScript
{
    public:
        npc_lady_sylvanas_windrunner_97695() : CreatureScript("npc_lady_sylvanas_windrunner_97695") { }

        struct npc_lady_sylvanas_windrunner_97695AI : public ScriptedAI
        {
            npc_lady_sylvanas_windrunner_97695AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eObjectives
            {
                Obj_SylvanasSceneSeen = 3984800
            };

            enum eKillcredits
            {
                Killcredit_ApproachHelyasThrone = 97696,
                Killcredit_SylvanasSceneSeen    = 3984800
            };

            enum eQuests
            {
                Quest_ADesperateBargain = 39848
            };

            enum ePoints
            {
                Point_FirstJumpEnd = 1,
                Point_SecondJumpEnd = 2
            };

            enum eNpcs
            {
                Npc_Helya = 91387
            };

            enum eSpells
            {
                Spell_ConversationHelya             = 217241,
                Spell_EmpowerLanternHelya           = 217231,
                Spell_EmpowerLanternSylvanas        = 217223,
                Spell_EmpowerLanternSylvanasPowered = 217275
            };

            Position const m_HelyaSpawnPos = { 549.7691f, 763.1024f, 1.253749f, 3.93789f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 100.0f && l_Player->HasQuest(eQuests::Quest_ADesperateBargain) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SylvanasSceneSeen) == 0)
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SylvanasSceneSeen);

                    if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_Helya, m_HelyaSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_EmpowerLanternHelya, true);
                        l_Creature->DespawnOrUnsummon(15000);
                    }

                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        uint64 l_SummonerGuid = l_Player->GetGUID();
                        l_Player->DelayedCastSpell(l_Player, eSpells::Spell_ConversationHelya, true, 3000);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_EmpowerLanternSylvanas, true);
                        l_Creature->DelayedCastSpell(l_Creature, eSpells::Spell_EmpowerLanternSylvanasPowered, true, 10000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->RemoveAllAuras();
                        }, 12000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetFacingTo(3.665191f);
                        }, 14000);

                        l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                            {
                                l_Creature->AI()->Talk(l_Player->GetTeam() == HORDE ? 0 : 3, l_SummonerGuid);
                            }
                        }, 16000);

                        l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                            {
                                l_Creature->AI()->Talk(l_Player->GetTeam() == HORDE ? 1 : 4, l_SummonerGuid);
                            }
                        }, l_Player->GetTeam() == HORDE ? 19000 : 22000);

                        l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                            {
                                l_Creature->AI()->Talk(l_Player->GetTeam() == HORDE ? 2 : 5, l_SummonerGuid);
                            }
                        }, l_Player->GetTeam() == HORDE ? 25000 : 28000);

                        l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                            {
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_ApproachHelyasThrone);
                            }

                            Position const l_FirstJumpPos = { 460.264f, 718.047f, 31.8195f };

                            l_Creature->GetMotionMaster()->MoveJump(l_FirstJumpPos, 15.0f, 10.0f, ePoints::Point_FirstJumpEnd);
                        }, 31000);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_FirstJumpEnd:
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        Position const l_SecondJumpPos = { 439.448f, 743.182f, -2.68336f };

                        me->GetMotionMaster()->MoveJump(l_SecondJumpPos, 15.0f, 10.0f, ePoints::Point_SecondJumpEnd);
                    }, 1000);

                    break;
                }
                case ePoints::Point_SecondJumpEnd:
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
            return new npc_lady_sylvanas_windrunner_97695AI(p_Creature);
        }
};

/// Ashildir - 97480
class npc_ashildir_97480 : public CreatureScript
{
    public:
        npc_ashildir_97480() : CreatureScript("npc_ashildir_97480") { }

        struct npc_ashildir_97480AI : public ScriptedAI
        {
            npc_ashildir_97480AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eObjectives
            {
                Obj_AshildirFirstLineHeard  = 3985700,
                Obj_AshildirSecondLineHeard = 3984900
            };

            enum eQuests
            {
                Quest_TheEternalNemesis = 39857,
                Quest_ToLightTheWay     = 39849,
                Quest_Sundered          = 39850,
                Quest_VictoryIsEternal  = 39853
            };

            enum eSpells
            {
                Spell_MaintainSummonFragmentWill  = 192216,
                Spell_MaintainSummonFragmentMight = 192488,
                Spell_MaintainSummonFragmentValor = 192491,
                Spell_MaintainSummonAshildir      = 192471,
                Spell_ShadowyGhostVisual          = 211179,
                Spell_ValkyraFlyVisual            = 192699
            };

            enum eNpcs
            {
                Npc_ValkyraGuardian = 97665
            };

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            std::array<Position const, 8> m_ValkyrasSpawnPos =
            { {
                { 301.0226f, 602.4167f, 10.12746f, 2.469414f  },
                { 282.6823f, 582.7621f, 13.21317f, 3.293188f  },
                { 294.6215f, 566.1511f, 8.48146f,  3.82732f   },
                { 322.8646f, 570.934f,  6.207253f, 2.469414f  },
                { 336.7813f, 562.1007f, 11.63603f, 5.232825f  },
                { 327.0538f, 581.2465f, 6.054722f, 2.469414f  },
                { 343.6962f, 582.5486f, 9.403357f, 0.4730051f },
                { 334.6042f, 601.7552f, 3.879115f, 1.162095f  }
            } };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 20.0f && l_Player->HasQuest(eQuests::Quest_TheEternalNemesis) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_AshildirFirstLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_AshildirFirstLineHeard);
                }

                if (me->GetExactDist2d(l_Player) < 20.0f && l_Player->GetQuestStatus(eQuests::Quest_ToLightTheWay) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_AshildirSecondLineHeard) == 0)
                {
                    Talk(1, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_AshildirSecondLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Quest->GetQuestId() == eQuests::Quest_Sundered)
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_MaintainSummonFragmentWill, true);
                    p_Player->CastSpell(p_Player, eSpells::Spell_MaintainSummonFragmentMight, true);
                    p_Player->CastSpell(p_Player, eSpells::Spell_MaintainSummonFragmentValor, true);
                }
                else if (p_Quest->GetQuestId() == eQuests::Quest_VictoryIsEternal)
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_MaintainSummonAshildir, true);

                    for (auto itr : m_ValkyrasSpawnPos)
                    {
                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, itr, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowyGhostVisual, true);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_ValkyraFlyVisual, true);

                                Position l_FlyPos = l_Creature->GetPosition();
                                l_FlyPos.m_positionZ += 30.0f;

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, l_FlyPos);
                                l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 33554432);
                            }, 2000);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashildir_97480AI(p_Creature);
        }
};

/// Drowning Valkyra - 97469
class npc_drowning_valkyra_97469 : public CreatureScript
{
    public:
        npc_drowning_valkyra_97469() : CreatureScript("npc_drowning_valkyra_97469") { }

        struct npc_drowning_valkyra_97469AI : public ScriptedAI
        {
            npc_drowning_valkyra_97469AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Releasing = 192387,
                Spell_ValkyraFlyVisual = 192699,
                Spell_CurseOfTheDepths = 192388
            };

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            enum eKillcredits
            {
                Killcredit_BoundValkyraReleased = 97472
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id == eSpells::Spell_Releasing)
                {
                    me->DestroyForPlayer(l_Player);
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_BoundValkyraReleased);

                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        uint64 l_SummonedGuid = l_Player->GetGUID();
                        l_Creature->SetInt32Value(UNIT_FIELD_EMOTE_STATE, 383);
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        l_Creature->RemoveAura(eSpells::Spell_CurseOfTheDepths);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetFlag(UNIT_FIELD_BYTES_1, 33554432);
                            l_Creature->SetInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                        }, 2000);

                        l_Creature->AddDelayedEvent([l_Creature, l_SummonedGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonedGuid))
                            {
                                l_Creature->SetFacingToObject(l_Player);
                                l_Creature->AI()->Talk(0, l_SummonedGuid);
                            }
                        }, 4000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_ValkyraFlyVisual, true);

                            Position l_FlyPos = l_Creature->GetPosition();
                            l_FlyPos.m_positionZ += 30.0f;

                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, l_FlyPos);
                        }, 8000);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FlyEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_drowning_valkyra_97469AI(p_Creature);
        }
};

/// Fragments - 97571, 97356, 97570
class npc_stormheim_fragments : public CreatureScript
{
    public:
        npc_stormheim_fragments() : CreatureScript("npc_stormheim_fragments") { }

        struct npc_stormheim_fragmentsAI : public ScriptedAI
        {
            npc_stormheim_fragmentsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_FragmentOfWill         = 192207,
                Spell_ChainedValor           = 192486,
                Spell_ChainedWill            = 192214,
                Spell_ChainedMight           = 192485,
                Spell_PingChainBunnies       = 192500,
                Spell_FragmentOfWillClone    = 192197,
                Spell_Releasing              = 192493,
                Spell_BreakChains            = 192495,
                Spell_HelyaConversationValor = 212904,
                Spell_HelyaConversationWill  = 212904,
                Spell_HelyaConversationMight = 212903,
                Spell_CancelFragmentOfWill   = 192509,
                Spell_CancelFragmentOfMight  = 192510,
                Spell_CancelFragmentOfValor  = 192512
            };

            enum eNpcs
            {
                Npc_FragmentOfValor = 97571,
                Npc_FragmentOfWill  = 97356,
                Npc_FragmentOfMight = 97570
            };

            enum ePoints
            {
                Point_FallEnd = 1
            };

            enum eKillcredits
            {
                Killcredit_FragmentOfWill  = 97482,
                Killcredit_FragmentOfMight = 97483,
                Killcredit_FragmentOfValor = 97484
            };

            enum eQuests
            {
                Quest_Sundered = 39850
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
                DoCast(eSpells::Spell_FragmentOfWill, true);
                DoCast(eSpells::Spell_PingChainBunnies, true);
                p_Summoner->CastSpell(me, eSpells::Spell_FragmentOfWillClone, true);

                switch (me->GetEntry())
                {
                    case eNpcs::Npc_FragmentOfValor:
                    {
                        DoCast(eSpells::Spell_ChainedValor, true);
                        break;
                    }
                    case eNpcs::Npc_FragmentOfWill:
                    {
                        DoCast(eSpells::Spell_ChainedWill, true);
                        break;
                    }
                    case eNpcs::Npc_FragmentOfMight:
                    {
                        DoCast(eSpells::Spell_ChainedMight, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id == eSpells::Spell_Releasing)
                {
                    DoCast(eSpells::Spell_BreakChains, true);
                    me->SetDisableGravity(false);
                    me->GetMotionMaster()->MoveFall(ePoints::Point_FallEnd);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 33587968);

                    switch (me->GetEntry())
                    {
                        case eNpcs::Npc_FragmentOfValor:
                        {
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FragmentOfValor);
                            break;
                        }
                        case eNpcs::Npc_FragmentOfWill:
                        {
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FragmentOfWill);
                            break;
                        }
                        case eNpcs::Npc_FragmentOfMight:
                        {
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FragmentOfMight);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (p_PointId == ePoints::Point_FallEnd)
                {
                    me->SetAIAnimKitId(1615);
                    me->DespawnOrUnsummon(2000);

                    switch (me->GetEntry())
                    {
                        case eNpcs::Npc_FragmentOfValor:
                        {
                            l_Player->CastSpell(l_Player, eSpells::Spell_HelyaConversationValor, true);
                            l_Player->DelayedCastSpell(l_Player, eSpells::Spell_CancelFragmentOfValor, true, 2000);
                            break;
                        }
                        case eNpcs::Npc_FragmentOfWill:
                        {
                            l_Player->CastSpell(l_Player, eSpells::Spell_HelyaConversationWill, true);
                            l_Player->DelayedCastSpell(l_Player, eSpells::Spell_CancelFragmentOfWill, true, 2000);
                            break;
                        }
                        case eNpcs::Npc_FragmentOfMight:
                        {
                            l_Player->CastSpell(l_Player, eSpells::Spell_HelyaConversationMight, true);
                            l_Player->DelayedCastSpell(l_Player, eSpells::Spell_CancelFragmentOfMight, true, 2000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_Sundered))
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stormheim_fragmentsAI(p_Creature);
        }
};

/// Ashildir - 97558
class npc_ashildir_97558 : public CreatureScript
{
    public:
        npc_ashildir_97558() : CreatureScript("npc_ashildir_97558") { }

        struct npc_ashildir_97558AI : public ScriptedAI
        {
            npc_ashildir_97558AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eNpcs
            {
                Npc_Geir            = 91948,
                Npc_ValkyraGuardian = 97665
            };

            enum eSpells
            {
                Spell_ShadowyGhostVisual = 211179,
                Spell_ValkyraFlyVisual   = 192699,
                Spell_HelyaConversation  = 212907
            };

            enum ePoints
            {
                Point_ValkyrasFlyEnd = 2,
                Point_ValkyraMoveToHomePosEnd = 4
            };

            enum eQuests
            {
                Quest_VictoryIsEternal = 39853,
                Quest_PaidInLifeblood  = 39855
            };

            enum eActions
            {
                Action_SummonedForFight = 1,
                Action_Leave = 2
            };

            std::vector<ObjectGuid> m_ValkyrasGuid;
            ObjectGuid m_SummonerGuid;
            Position const m_FirstValkyraSpawnPos  = { 176.184f,  574.257f,  23.85049f, 0.3840921f };
            Position const m_SecondValkyraSpawnPos = { 202.033f,  629.8698f, 19.83694f, 4.781522f  };
            Position const m_ThirdValkyraSpawnPos  = { 251.7379f, 606.7049f, 40.81426f, 3.477265f  };
            Position const m_FourthValkyraSpawnPos = { 235.2934f, 549.184f,  36.46869f, 1.907143f  };
            Position const m_FirstValkyraFlyPos    = { 197.955f,  583.056f, -3.28752f };
            Position const m_SecondValkyraFlyPos   = { 203.734f,  605.304f, -3.9063f  };
            Position const m_ThirdValkyraFlyPos    = { 224.955f,  597.361f, -4.47148f };
            Position const m_FourthValkyraFlyPos   = { 226.26f,   575.021f, -2.67507f };
            bool m_ValkyrasSummoned = false;
            bool m_GeirDefeated = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->ToPlayer())
                    return;

                m_SummonerGuid = p_Summoner->GetGUID();
                Talk(0, m_SummonerGuid);
                me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                for (auto itr : m_ValkyrasGuid)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    for (auto itr : m_ValkyrasGuid)
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                        {
                            l_Creature->DespawnOrUnsummon();
                        }
                    }

                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Player->isInCombat() && l_Player->GetSelectedUnit() && l_Player->GetSelectedUnit()->GetEntry() == eNpcs::Npc_Geir && !m_ValkyrasSummoned && !m_GeirDefeated)
                {
                    me->Attack(l_Player->GetSelectedUnit(), false);

                    if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_FirstValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_SummonedForFight);
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->SetDisableGravity(true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowyGhostVisual, true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ValkyraFlyVisual, true);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyrasFlyEnd, m_FirstValkyraFlyPos);
                        l_Creature->SetHomePosition(m_FirstValkyraFlyPos);
                        m_ValkyrasGuid.push_back(l_Creature->GetGUID());
                    }

                    if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_SecondValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_SummonedForFight);
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->SetDisableGravity(true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowyGhostVisual, true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ValkyraFlyVisual, true);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyrasFlyEnd, m_SecondValkyraFlyPos);
                        l_Creature->SetHomePosition(m_SecondValkyraFlyPos);
                        m_ValkyrasGuid.push_back(l_Creature->GetGUID());
                    }

                    if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_ThirdValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_SummonedForFight);
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->SetDisableGravity(true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowyGhostVisual, true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ValkyraFlyVisual, true);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyrasFlyEnd, m_ThirdValkyraFlyPos);
                        l_Creature->SetHomePosition(m_ThirdValkyraFlyPos);
                        m_ValkyrasGuid.push_back(l_Creature->GetGUID());
                    }

                    if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_FourthValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_SummonedForFight);
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->SetDisableGravity(true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowyGhostVisual, true);
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_ValkyraFlyVisual, true);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyrasFlyEnd, m_FourthValkyraFlyPos);
                        l_Creature->SetHomePosition(m_FourthValkyraFlyPos);
                        m_ValkyrasGuid.push_back(l_Creature->GetGUID());
                    }

                    m_ValkyrasSummoned = true;
                }

                if (l_Player->GetQuestStatus(eQuests::Quest_VictoryIsEternal) == QUEST_STATUS_COMPLETE && !m_GeirDefeated)
                {
                    for (auto itr : m_ValkyrasGuid)
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                        {
                            l_Creature->AI()->DoAction(eActions::Action_Leave);
                        }
                    }

                    l_Player->CastSpell(l_Player, eSpells::Spell_HelyaConversation, true);
                    m_GeirDefeated = true;
                }

                if ((m_GeirDefeated || l_Player->HasQuest(eQuests::Quest_PaidInLifeblood)) && me->GetAreaId() == 7629)
                {
                    me->DespawnOrUnsummon();
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashildir_97558AI(p_Creature);
        }
};

/// Valkyra Guardian - 97665
class npc_valkyra_guardian_97665 : public CreatureScript
{
    public:
        npc_valkyra_guardian_97665() : CreatureScript("npc_valkyra_guardian_97665") { }

        struct npc_valkyra_guardian_97665AI : public ScriptedAI
        {
            npc_valkyra_guardian_97665AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eNpcs
            {
                Npc_Geir = 91948
            };

            enum ePoints
            {
                Point_FlyEnd                  = 1,
                Point_ValkyrasFlyEnd          = 2,
                Point_ValkyrasFinallyFlyEnd   = 3,
                Point_ValkyraMoveToHomePosEnd = 4
            };

            enum eSpells
            {
                Spell_ValkyraFlyVisual = 192699
            };

            enum eActions
            {
                Action_SummonedForFight = 1,
                Action_Leave            = 2
            };

            enum eQuests
            {
                Quest_VictoryIsEternal = 39853
            };

            ObjectGuid m_SummonerGuid;
            bool m_CanLeave = false;
            bool m_CanDespawn = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActions::Action_Leave)
                {
                    m_CanLeave = true;
                }
                else
                {
                    m_CanDespawn = true;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_FlyEnd:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    case ePoints::Point_ValkyrasFlyEnd:
                    {
                        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                        me->SetDisableGravity(false);
                        me->RemoveAura(eSpells::Spell_ValkyraFlyVisual);
                        me->SetUInt32Value(UNIT_FIELD_FLAGS, 559360);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            me->setFaction(l_Player->GetTeam() == HORDE ? 1610 : 4);
                        }

                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_Geir, 100.0f, true))
                        {
                            me->Attack(l_Creature, false);
                            me->SetInCombatWith(l_Creature);
                        }

                        break;
                    }
                    case ePoints::Point_ValkyraMoveToHomePosEnd:
                    {
                        m_CanLeave = false;
                        me->CastSpell(me, eSpells::Spell_ValkyraFlyVisual, true);

                        Position l_FlyPos = me->GetPosition();
                        l_FlyPos.m_positionZ += 30.0f;

                        me->SetDisableGravity(true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ValkyrasFinallyFlyEnd, l_FlyPos);
                        break;
                    }
                    case ePoints::Point_ValkyrasFinallyFlyEnd:
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
                if (!me->isSummon() || !m_CanDespawn)
                    return;

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (l_Owner->ToPlayer())
                    {
                        if (!l_Owner->ToPlayer()->HasQuest(eQuests::Quest_VictoryIsEternal))
                        {
                            me->DespawnOrUnsummon();
                            return;
                        }
                    }
                }

                if (m_CanLeave && !me->isInCombat() && !me->IsMoving())
                {
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_ValkyraMoveToHomePosEnd, me->GetHomePosition(), false);
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_valkyra_guardian_97665AI(p_Creature);
        }
};

/// Ashildir Levitate Bunny - 97670
class npc_ashildir_levitate_bunny_97670 : public CreatureScript
{
    public:
        npc_ashildir_levitate_bunny_97670() : CreatureScript("npc_ashildir_levitate_bunny_97670") { }

        struct npc_ashildir_levitate_bunny_97670AI : public VehicleAI
        {
            npc_ashildir_levitate_bunny_97670AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_RideVehicle = 52391
            };

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Position l_FlyPos = me->GetPosition();
                l_FlyPos.m_positionZ += 7.0f;

                p_Summoner->CastSpell(me, eSpells::Spell_RideVehicle, true);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, l_FlyPos);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashildir_levitate_bunny_97670AI(p_Creature);
        }
};

/// Player Levitate Bunny - 97669
class npc_player_levitate_bunny_97669 : public CreatureScript
{
    public:
        npc_player_levitate_bunny_97669() : CreatureScript("npc_player_levitate_bunny_97669") { }

        struct npc_player_levitate_bunny_97669AI : public VehicleAI
        {
            npc_player_levitate_bunny_97669AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_RideVehicle        = 52391,
                Spell_LevitateBunnyEject = 192676
            };

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Position l_FlyPos = me->GetPosition();
                l_FlyPos.m_positionZ += 7.0f;

                p_Summoner->CastSpell(me, eSpells::Spell_RideVehicle, true);
                m_SummonerGuid = p_Summoner->GetGUID();
                me->SetSpeed(MOVE_FLIGHT, 0.1f);
                me->SetDisableGravity(true);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, l_FlyPos);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_LevitateBunnyEject && p_Caster->GetGUID() == m_SummonerGuid)
                {
                    if (me->GetVehicleKit())
                    {
                        me->GetVehicleKit()->RemoveAllPassengers();
                        me->DespawnOrUnsummon();
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_player_levitate_bunny_97669AI(p_Creature);
        }
};

/// Forsaken Catapult - 95212
class npc_forsaken_catapult_95212 : public CreatureScript
{
    public:
        npc_forsaken_catapult_95212() : CreatureScript("npc_forsaken_catapult_95212") { }

        struct npc_forsaken_catapult_95212AI : public ScriptedAI
        {
            npc_forsaken_catapult_95212AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Firing = 189100,
                Spell_FirePlagueBarrel = 189176
            };

            void InitializeAI() override
            {
                me->SetUInt32Value(UNIT_FIELD_STATE_SPELL_VISUAL_ID, 48822);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_Spell->Id == eSpells::Spell_Firing)
                {
                    me->DestroyForPlayer(l_Player);

                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 360000, 0, l_Player->GetGUID()))
                    {
                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            std::array<Position, 3> l_SpellDest =
                            { {
                                { 2383.993f, 1639.132f, 17.15132f },
                                { 2407.521f, 1658.127f, 3.523904f },
                                { 2399.729f, 1649.578f, 9.062463f }
                            } };

                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Creature->SetUInt32Value(UNIT_FIELD_STATE_SPELL_VISUAL_ID, 0);
                            l_Creature->CastSpell(l_SpellDest[urand(0, 2)], eSpells::Spell_FirePlagueBarrel, true);
                        }, 2000);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_forsaken_catapult_95212AI(p_Creature);
        }
};

/// Rowboat to Skold Ashil - 109454
class npc_rowboat_to_skold_ashil_109454 : public CreatureScript
{
    public:
        npc_rowboat_to_skold_ashil_109454() : CreatureScript("npc_rowboat_to_skold_ashil_109454") { }

        struct npc_rowboat_to_skold_ashil_109454AI : public VehicleAI
        {
            npc_rowboat_to_skold_ashil_109454AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_PingRowboat = 217918,
                Spell_StartRowing = 217888,
                Spell_HoldOars    = 217910
            };

            enum ePoints
            {
                Point_PathEnd = 1
            };

            enum eNpcs
            {
                Npc_DreadwakeDeathguard = 109452
            };

            ObjectGuid m_SummonerGuid;
            bool m_CastDone = false;

            void OnSpellClick(Unit* p_Clicker) override
            {
                Player* l_Player = p_Clicker->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ExitVehicle();
                me->DestroyForPlayer(l_Player);

                if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                {
                    l_Player->EnterVehicle(l_Creature, 0, true);

                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Boat[8] =
                        {
                            { 2519.563f, 1821.177f, -0.2500886f },
                            { 2522.953f, 1812.623f, -0.3898493f },
                            { 2527.049f, 1799.286f, -0.2275997f },
                            { 2523.351f, 1777.477f, -0.3747796f },
                            { 2507.8f,   1753.724f, -0.2544793f },
                            { 2514.765f, 1733.609f, -0.3179016f },
                            { 2532.759f, 1735.134f, -0.1934764f },
                            { 2553.971f, 1742.431f, -0.1589042f }
                        };

                        if (Vehicle* l_Vehicle = l_Creature->GetVehicleKit())
                        {
                            if (Unit* l_Unit = l_Vehicle->GetPassenger(1))
                            {
                                l_Unit->RemoveAura(eSpells::Spell_HoldOars);
                                l_Unit->CastSpell(l_Unit, eSpells::Spell_StartRowing, true);
                            }
                        }

                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathEnd, Path_Boat, 8);
                    }, 4000);
                }
            }

            void InitializeAI() override
            {
                me->SetUInt32Value(UNIT_FIELD_INTERACT_SPELL_ID, 217918);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();

                if (Vehicle* l_Vehicle = me->GetVehicleKit())
                {
                    if (Unit* l_Unit = l_Vehicle->GetPassenger(1))
                    {
                        l_Unit->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
                        l_Unit->SetGuidValue(UNIT_FIELD_DEMON_CREATOR, m_SummonerGuid);
                        p_Summoner->UpdateObjectVisibility(false);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_PathEnd)
                {
                    if (Vehicle* l_Vehicle = me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = l_Vehicle->GetPassenger(1))
                        {
                            l_Unit->RemoveAura(eSpells::Spell_StartRowing);
                            l_Unit->CastSpell(l_Unit, eSpells::Spell_HoldOars, true);
                        }
                    }

                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        l_Player->ExitVehicle();
                    }

                    me->DespawnOrUnsummon(6000);
                }
            }

            void UpdateAI(const uint32 /*diff*/) override
            {
                if (m_CastDone)
                    return;

                if (Vehicle* l_Vehicle = me->GetVehicleKit())
                {
                    if (Unit* l_Unit = l_Vehicle->GetPassenger(1))
                    {
                        l_Unit->CastSpell(l_Unit, eSpells::Spell_HoldOars, true);
                        m_CastDone = true;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rowboat_to_skold_ashil_109454AI(p_Creature);
        }
};

/// Lady Sylvanas Windrunner - 94227
class npc_lady_sylvanas_windrunner_94227 : public CreatureScript
{
    public:
        npc_lady_sylvanas_windrunner_94227() : CreatureScript("npc_lady_sylvanas_windrunner_94227") { }

        struct npc_lady_sylvanas_windrunner_94227AI : public ScriptedAI
        {
            npc_lady_sylvanas_windrunner_94227AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eConversations
            {
                Conv_SylvanasLosConv = 3593
            };

            enum eQuests
            {
                Quest_ToSkoldAshil = 39154
            };

            enum eObjectives
            {
                Obj_SylvanasLosConversationHeard = 3915400
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 25.0f && l_Player->HasQuest(eQuests::Quest_ToSkoldAshil) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SylvanasLosConversationHeard) == 0)
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_SylvanasLosConv, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }

                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_SylvanasLosConversationHeard);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_sylvanas_windrunner_94227AI(p_Creature);
        }
};

/// Player Levitate Bunny - 94769
class npc_player_levitate_bunny_94769 : public CreatureScript
{
    public:
        npc_player_levitate_bunny_94769() : CreatureScript("npc_player_levitate_bunny_94769") { }

        struct npc_player_levitate_bunny_94769AI : public VehicleAI
        {
            npc_player_levitate_bunny_94769AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_RideVehicle    = 52391,
                Spell_SummonAshildir = 187832
            };

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                uint64 l_SummonerGuid = p_Summoner->GetGUID();
                Position l_FlyPos = me->GetPosition();
                l_FlyPos.m_positionZ += 7.0f;

                p_Summoner->CastSpell(me, eSpells::Spell_RideVehicle, true);
                me->SetSpeed(MOVE_FLIGHT, 0.1f);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, l_FlyPos);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_SummonAshildir, true);
                    }
                }, 25000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        l_Player->ExitVehicle();
                    }

                    me->DespawnOrUnsummon();
                }, 27000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_player_levitate_bunny_94769AI(p_Creature);
        }
};

/// Ashildir - 94764
class npc_ashildir_94764 : public CreatureScript
{
    public:
        npc_ashildir_94764() : CreatureScript("npc_ashildir_94764") { }

        struct npc_ashildir_94764AI : public VehicleAI
        {
            npc_ashildir_94764AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_AshildirsBlessing = 187826
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                uint64 l_SummonerGuid = p_Summoner->GetGUID();
                Talk(0, l_SummonerGuid);
                me->DespawnOrUnsummon(24000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(1, l_SummonerGuid);
                    }
                }, 6000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(2, l_SummonerGuid);
                    }

                    DoCast(eSpells::Spell_AshildirsBlessing);
                }, 12000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(3, l_SummonerGuid);
                    }
                }, 18000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ashildir_94764AI(p_Creature);
        }
};

/// Lady Sylvanas Windrunner - 94228
class npc_lady_sylvanas_windrunner_94228 : public CreatureScript
{
    public:
        npc_lady_sylvanas_windrunner_94228() : CreatureScript("npc_lady_sylvanas_windrunner_94228") { }

        struct npc_lady_sylvanas_windrunner_94228AI : public ScriptedAI
        {
            npc_lady_sylvanas_windrunner_94228AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            enum eSpells
            {
                Spell_NarrowEscape = 229076
            };

            enum eQuests
            {
                Quest_ANewLifeForUndeath = 38882
            };

            ObjectGuid m_SummonerGuid;

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Sylvanas[22] =
                        {
                            { 2543.494f, 1401.1f,   32.89903f },
                            { 2543.494f, 1399.6f,   33.14903f },
                            { 2543.244f, 1396.1f,   32.89903f },
                            { 2543.244f, 1388.6f,   32.89903f },
                            { 2542.994f, 1380.35f,  32.89903f },
                            { 2542.994f, 1375.1f,   32.89903f },
                            { 2542.994f, 1373.6f,   32.89903f },
                            { 2542.994f, 1366.35f,  32.89903f },
                            { 2542.994f, 1363.35f,  32.89903f },
                            { 2542.744f, 1354.6f,   32.89903f },
                            { 2542.464f, 1353.08f,  33.12475f },
                            { 2542.606f, 1346.431f, 35.51183f },
                            { 2542.606f, 1342.431f, 37.01183f },
                            { 2542.606f, 1333.681f, 39.76183f },
                            { 2542.856f, 1327.431f, 42.01183f },
                            { 2542.748f, 1326.781f, 41.8989f  },
                            { 2542.717f, 1325.039f, 42.04565f },
                            { 2542.467f, 1319.539f, 42.04565f },
                            { 2542.467f, 1316.039f, 42.04565f },
                            { 2542.35f, 1314.51f,   41.8989f  },
                            { 2542.566f, 1310.504f, 42.15414f },
                            { 2542.396f, 1301.095f, 41.40938f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_Sylvanas, 22, false);
                    }, 2000);
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveEnd)
                {
                    me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 376);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_NarrowEscape && p_Caster->GetGUID() == m_SummonerGuid)
                {
                    me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isSummon())
                    return;

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_ANewLifeForUndeath))
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_sylvanas_windrunner_94228AI(p_Creature);
        }
};

/// Credit Bunny - 94915
class npc_stormheim_racial_final_quest : public CreatureScript
{
    public:
        npc_stormheim_racial_final_quest() : CreatureScript("npc_stormheim_racial_final_quest") { }

        struct npc_stormheim_racial_final_questAI : public ScriptedAI
        {
            npc_stormheim_racial_final_questAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ANewLifeForUndeath    = 38882,
                Quest_EndingTheNewBeginning = 39122
            };

            enum eSpells
            {
                Spell_NarrowEscape = 205552
            };

            enum eKillcredits
            {
                Killcredit_SylvanasFollowed = 94915,
                Killcredit_GreymaneFollowed = 95269
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 10.0f && (l_Player->GetQuestStatus(eQuests::Quest_ANewLifeForUndeath) == QUEST_STATUS_INCOMPLETE ||
                    l_Player->GetQuestStatus(eQuests::Quest_EndingTheNewBeginning) == QUEST_STATUS_INCOMPLETE))
                {
                    l_Player->CastSpell(l_Player, eSpells::Spell_NarrowEscape, true);
                    if (l_Player->GetTeam() == HORDE)
                    {
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SylvanasFollowed);
                    }
                    else
                    {
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_GreymaneFollowed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stormheim_racial_final_questAI(p_Creature);
        }
};

/// Vethir - 97979
class npc_vethir_97979 : public CreatureScript
{
    public:
        npc_vethir_97979() : CreatureScript("npc_vethir_97979") { }

        struct npc_vethir_97979AI : public ScriptedAI
        {
            npc_vethir_97979AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_KnockingOnValorsDoor = 40001,
                Quest_StemTheTide          = 40003,
                Quest_BreakTheSpine        = 40004
            };

            enum eObjectives
            {
                Obj_VethirLineHeard = 4000100,
                Obj_VethirFreed     = 4000200
            };

            enum eKillcredits
            {
                Killcredit_VethirFreed = 97981
            };

            enum eSpells
            {
                Spell_DestroyBindings = 182046
            };

            enum eNpcs
            {
                Npc_CircleOfBinding = 91743
            };

            enum ePoints
            {
                Point_PreFlyEnd  = 1,
                Point_MainFlyEnd = 2
            };

            Position const m_CircleSpawnPos = { 2873.212f, 1207.078f, 188.8467f, 5.55298f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 20.0f && l_Player->HasQuest(eQuests::Quest_KnockingOnValorsDoor) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_VethirLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_VethirLineHeard);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (!p_Caster->ToPlayer())
                    return;

                if (p_Spell->Id == eSpells::Spell_DestroyBindings)
                {
                    if (Creature* l_Creature = p_Caster->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                        l_Creature->AI()->Talk(1, p_Caster->GetGUID());
                        l_Creature->PlayOneShotAnimKitId(1439);
                        l_Creature->DespawnOrUnsummon(5000);
                    }

                    if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_CircleOfBinding, m_CircleSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetAIAnimKitId(3934);
                        }, 2000);

                        l_Creature->DespawnOrUnsummon(8000);
                    }

                    p_Caster->AddDelayedEvent([p_Caster]() -> void
                    {
                        if (p_Caster->ToPlayer())
                            p_Caster->ToPlayer()->QuestObjectiveOptionalSatisfy(eObjectives::Obj_VethirFreed);
                    }, 5000);

                    p_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_VethirFreed);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if ((p_Quest->GetQuestId() == eQuests::Quest_StemTheTide || p_Quest->GetQuestId() == eQuests::Quest_BreakTheSpine) &&
                    (p_Player->HasQuest(eQuests::Quest_StemTheTide) && p_Player->HasQuest(eQuests::Quest_BreakTheSpine)))
                {
                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            Position const l_FlyPos = { 2875.261f, 1208.984f, 198.8467f };

                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PreFlyEnd, l_FlyPos);
                        }, 1500);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_PreFlyEnd)
                {
                    me->SetFlag(UNIT_FIELD_BYTES_1, 50331648);

                    me->AddDelayedEvent([this]() -> void
                    {
                        G3D::Vector3 const Path_Vethir[4] =
                        {
                            { 2855.129f, 1224.578f, 207.8734f },
                            { 2746.481f, 1200.408f, 226.405f  },
                            { 2677.826f, 1138.332f, 240.9755f },
                            { 2584.483f, 1043.339f, 287.9793f }
                        };

                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_MainFlyEnd, Path_Vethir, 4);
                    }, 2000);
                }
                else if (p_PointId == ePoints::Point_MainFlyEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vethir_97979AI(p_Creature);
        }
};

/// Portals Bunny - 97983, 97984, 97985
class npc_stormheim_portals_bunny : public CreatureScript
{
    public:
        npc_stormheim_portals_bunny() : CreatureScript("npc_stormheim_portals_bunny") { }

        struct npc_stormheim_portals_bunnyAI : public ScriptedAI
        {
            npc_stormheim_portals_bunnyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_CallVethir = 193327
            };

            enum eConversations
            {
                Conv_OnePortalDestroyed  = 2844,
                Conv_TwoPortalsDestroyed = 284400
            };

            enum eObjectives
            {
                Obj_PortalsDestroyed = 280263
            };

            enum eKillcredits
            {
                Killcredit_PortalsDestroyed = 98186
            };

            enum eNpcs
            {
                Npc_NorthBunny = 97983,
                Npc_EastBunny  = 97984,
                Npc_SouthBunny = 97985,
                Npc_Vethir     = 98174,
                Npc_Hrafsir    = 98175,
                Npc_Erilar     = 98176
            };

            enum ePoints
            {
                Point_FlyToNorthPortalEnd = 1,
                Point_FlyToEastPortalEnd = 3,
                Point_FlyToSouthPortalEnd = 5
            };

            Position const m_NorthVethirSpawnPos  = { 2557.36f,  1011.649f, 278.4036f, 5.651166f };
            Position const m_NorthHrafsirSpawnPos = { 2543.036f, 1003.531f, 282.9974f, 4.910566f };
            Position const m_NorthErilarSpawnPos  = { 2548.105f, 1029.094f, 277.2491f, 5.904737f };
            Position const m_EastVethirSpawnPos   = { 2590.445f, 1131.631f, 275.7822f, 5.133287f };
            Position const m_EastHrafsirSpawnPos  = { 2596.909f, 1144.61f,  274.5002f, 4.441939f };
            Position const m_EastErilarSpawnPos   = { 2574.919f, 1136.032f, 277.0474f, 5.199128f };
            Position const m_SouthVethirSpawnPos  = { 2633.848f, 1105.009f, 274.0415f, 3.278288f };
            Position const m_SouthHrafsirSpawnPos = { 2643.083f, 1118.445f, 277.5313f, 3.398571f };
            Position const m_SouthErilarSpawnPos  = { 2648.842f, 1103.14f,  275.5584f, 3.593979f };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_PortalsDestroyed);

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_PortalsDestroyed) == 1)
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_OnePortalDestroyed, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
                else if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_PortalsDestroyed) == 2)
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_TwoPortalsDestroyed, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }

                if (p_Spell->Id == eSpells::Spell_CallVethir)
                {

                    switch (me->GetEntry())
                    {
                        case eNpcs::Npc_NorthBunny:
                        {
                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Vethir, m_NorthVethirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Vethir[4] =
                                {
                                    { 2563.313f, 1008.753f, 276.419f  },
                                    { 2612.785f, 988.2118f, 251.2283f },
                                    { 2661.322f, 977.5018f, 233.1153f },
                                    { 2697.712f, 970.8924f, 229.5292f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToNorthPortalEnd, Path_Vethir, 4);
                            }

                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Hrafsir, m_NorthHrafsirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Hrafsir[4] =
                                {
                                    { 2549.839f, 997.8629f, 276.419f  },
                                    { 2597.488f, 983.7413f, 251.2283f },
                                    { 2652.601f, 970.6702f, 233.1153f },
                                    { 2690.181f, 964.2899f, 229.5292f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToNorthPortalEnd, Path_Hrafsir, 4);
                            }

                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Erilar, m_NorthErilarSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Erilar[4] =
                                {
                                    { 2558.255f, 1024.441f, 276.419f  },
                                    { 2605.132f, 1000.92f,  251.2283f },
                                    { 2654.578f, 985.6163f, 233.1153f },
                                    { 2692.187f, 979.2778f, 229.5292f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToNorthPortalEnd, Path_Erilar, 4);
                            }

                            break;
                        }
                        case eNpcs::Npc_EastBunny:
                        {
                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Vethir, m_EastVethirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Vethir[4] =
                                {
                                    { 2594.957f, 1124.101f, 273.2536f },
                                    { 2629.932f, 1071.733f, 250.289f  },
                                    { 2628.747f, 1028.056f, 242.5045f },
                                    { 2619.979f, 1001.273f, 238.6427f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToEastPortalEnd, Path_Vethir, 4);
                            }

                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Hrafsir, m_EastHrafsirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Hrafsir[4] =
                                {
                                    { 2600.3f,   1135.734f, 273.2536f },
                                    { 2637.592f, 1075.51f, 250.289f   },
                                    { 2637.749f, 1030.858f, 242.5045f },
                                    { 2629.78f,  1000.88f, 238.6427f  }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToEastPortalEnd, Path_Hrafsir, 4);
                            }

                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Erilar, m_EastErilarSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Erilar[4] =
                                {
                                    { 2580.025f, 1128.858f, 273.2536f },
                                    { 2621.34f,  1075.238f, 250.289f  },
                                    { 2622.896f, 1038.177f, 242.5045f },
                                    { 2617.136f, 1014.788f, 238.6427f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToEastPortalEnd, Path_Erilar, 4);
                            }

                            break;
                        }
                        case eNpcs::Npc_SouthBunny:
                        {
                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Vethir, m_SouthVethirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Vethir[4] =
                                {
                                    { 2616.707f, 1103.024f, 273.7189f },
                                    { 2553.688f, 1098.469f, 250.1073f },
                                    { 2518.227f, 1090.509f, 241.9547f },
                                    { 2492.753f, 1067.556f, 234.0448f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToSouthPortalEnd, Path_Vethir, 4);
                            }

                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Hrafsir, m_SouthHrafsirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Hrafsir[4] =
                                {
                                    { 2626.163f, 1114.809f, 273.7189f },
                                    { 2562.97f, 1107.205f, 250.1073f  },
                                    { 2520.433f, 1099.613f, 241.9547f },
                                    { 2492.861f, 1077.328f, 234.0448f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToSouthPortalEnd, Path_Hrafsir, 4);
                            }

                            if (Creature* l_Creature = p_Caster->SummonCreature(eNpcs::Npc_Erilar, m_SouthErilarSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Caster->GetGUID()))
                            {
                                G3D::Vector3 const Path_Erilar[4] =
                                {
                                    { 2626.953f, 1094.613f, 273.7189f },
                                    { 2563.334f, 1092.009f, 250.1073f },
                                    { 2527.575f, 1085.22f,  241.9547f },
                                    { 2502.208f, 1066.651f, 234.0448f }
                                };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToSouthPortalEnd, Path_Erilar, 4);
                            }

                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_stormheim_portals_bunnyAI(p_Creature);
        }
};

/// Vethir - 97979
class npc_stormheim_destroy_dragons : public CreatureScript
{
    public:
        npc_stormheim_destroy_dragons() : CreatureScript("npc_stormheim_destroy_dragons") { }

        struct npc_stormheim_destroy_dragonsAI : public ScriptedAI
        {
            npc_stormheim_destroy_dragonsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_FlyToNorthPortalEnd   = 1,
                Point_FlyFromNorthPortalEnd = 2,
                Point_FlyToEastPortalEnd    = 3,
                Point_FlyFromEastPortalEnd  = 4,
                Point_FlyToSouthPortalEnd   = 5,
                Point_FlyFromSouthPortalEnd = 6,
            };

            enum eSpells
            {
                Spell_ThundersBite = 193338
            };

            enum eNpcs
            {
                Npc_Vethir  = 98174,
                Npc_Hrafsir = 98175,
                Npc_Erilar  = 98176
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (spell->Id == eSpells::Spell_ThundersBite)
                {
                    l_Player->KilledMonsterCredit(target->GetEntry());
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_FlyToNorthPortalEnd:
                    {
                        DoCastAOE(eSpells::Spell_ThundersBite);

                        switch (me->GetEntry())
                        {
                            case eNpcs::Npc_Vethir:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Vethir[5] =
                                    {
                                        { 2727.493f, 963.7518f, 235.5064f },
                                        { 2733.49f, 926.0555f, 241.6753f },
                                        { 2671.419f, 829.3212f, 277.7679f },
                                        { 2589.149f, 788.7691f, 361.8595f },
                                        { 2557.42f, 813.0833f, 428.5614f }
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromNorthPortalEnd, Path_Vethir, 5);
                                }, 2000);

                                break;
                            }
                            case eNpcs::Npc_Hrafsir:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Hrafsir[5] =
                                    {
                                        { 2710.426f, 960.5851f, 235.5064f },
                                        { 2724.299f, 931.7292f, 241.6753f },
                                        { 2672.345f, 845.0938f, 277.7679f },
                                        { 2603.441f, 801.3073f, 361.8595f },
                                        { 2586.052f, 801.6528f, 428.5614f },
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromNorthPortalEnd, Path_Hrafsir, 5);
                                }, 2000);

                                break;
                            }
                            case eNpcs::Npc_Erilar:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Erilar[5] =
                                    {
                                        { 2724.495f, 983.7864f, 235.5064f },
                                        { 2747.818f, 933.6268f, 241.6753f },
                                        { 2689.241f, 835.5208f, 277.7679f },
                                        { 2611.901f, 779.5833f, 361.8595f },
                                        { 2573.309f, 786.9462f, 428.5614f },
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromNorthPortalEnd, Path_Erilar, 5);
                                }, 2000);

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case ePoints::Point_FlyToEastPortalEnd:
                    {
                        DoCastAOE(eSpells::Spell_ThundersBite);

                        switch (me->GetEntry())
                        {
                            case eNpcs::Npc_Vethir:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Vethir[4] =
                                    {
                                        { 2630.344f, 967.2604f, 250.7028f },
                                        { 2648.661f, 890.1129f, 287.123f },
                                        { 2596.475f, 786.8403f, 349.4916f },
                                        { 2553.722f, 716.4844f, 414.2062f }
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromEastPortalEnd, Path_Vethir, 4);
                                }, 2000);

                                break;
                            }
                            case eNpcs::Npc_Hrafsir:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Hrafsir[4] =
                                    {
                                        { 2638.379f, 978.4688f, 250.7028f },
                                        { 2659.246f, 888.8281f, 287.123f },
                                        { 2614.146f, 789.7274f, 349.4916f },
                                        { 2574.053f, 723.9028f, 414.2062f }
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromEastPortalEnd, Path_Hrafsir, 4);
                                }, 2000);

                                break;
                            }
                            case eNpcs::Npc_Erilar:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Erilar[4] =
                                    {
                                        { 2619.279f, 970.7396f, 250.7028f },
                                        { 2642.186f, 895.9636f, 287.123f },
                                        { 2596.188f, 801.0868f, 349.4916f },
                                        { 2556.347f, 738.1702f, 414.2062f }
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromEastPortalEnd, Path_Erilar, 4);
                                }, 2000);

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case ePoints::Point_FlyToSouthPortalEnd:
                    {
                        DoCastAOE(eSpells::Spell_ThundersBite);

                        switch (me->GetEntry())
                        {
                            case eNpcs::Npc_Vethir:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Vethir[4] =
                                    {
                                        { 2456.425f, 1057.156f, 264.2624f },
                                        { 2352.158f, 1130.648f, 337.8951f },
                                        { 2307.667f, 1118.043f, 413.069f },
                                        { 2253.46f, 1071.385f, 499.2413f }
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromSouthPortalEnd, Path_Vethir, 4);
                                }, 2000);

                                break;
                            }
                            case eNpcs::Npc_Hrafsir:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Hrafsir[4] =
                                    {
                                        { 2468.99f, 1059.679f, 264.2624f },
                                        { 2365.344f, 1135.141f, 337.8951f },
                                        { 2315.641f, 1132.641f, 413.069f },
                                        { 2262.271f, 1088.043f, 499.2413f }
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromSouthPortalEnd, Path_Hrafsir, 4);
                                }, 2000);

                                break;
                            }
                            case eNpcs::Npc_Erilar:
                            {
                                me->AddDelayedEvent([this]() -> void
                                {
                                    G3D::Vector3 const Path_Erilar[4] =
                                    {
                                        { 2459.288f, 1046.186f, 264.2624f },
                                        { 2358.061f, 1116.481f, 337.8951f },
                                        { 2322.064f, 1114.889f, 413.069f  },
                                        { 2267.684f, 1074.924f, 499.2413f },
                                    };

                                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyFromSouthPortalEnd, Path_Erilar, 4);
                                }, 2000);

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case ePoints::Point_FlyFromNorthPortalEnd:
                    case ePoints::Point_FlyFromEastPortalEnd:
                    case ePoints::Point_FlyFromSouthPortalEnd:
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
            return new npc_stormheim_destroy_dragonsAI(p_Creature);
        }
};

/// Vethir - 97986
class npc_vethir_97986 : public CreatureScript
{
    public:
        npc_vethir_97986() : CreatureScript("npc_vethir_97986") { }

        struct npc_vethir_97986AI : public VehicleAI
        {
            npc_vethir_97986AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_VethirCircle[9] =
            {
                { 2502.292f, 854.3715f, 670.5589f },
                { 2477.597f, 856.4549f, 668.7033f },
                { 2406.931f, 918.2518f, 668.7127f },
                { 2395.729f, 958.9132f, 667.2391f },
                { 2427.644f, 989.1788f, 666.5181f },
                { 2467.33f,  976.5313f, 665.5366f },
                { 2529.943f, 911.9167f, 670.0196f },
                { 2530.667f, 877.7917f, 670.9f    },
                { 2508.977f, 858.2952f, 671.3801f }
            };

            enum ePoints
            {
                Point_FlyOnTopEnd       = 1,
                Point_CircleFlyEnd      = 2,
                Point_FlyAfterCircleEnd = 3
            };

            enum eSpells
            {
                Spell_RideVehicle       = 52391,
                Spell_VethirLandTrigger = 193504
            };

            enum eKillcredits
            {
                Killcredit_VethirLanded         = 103766,
                Killcredit_VethirGossipSelected = 4000500
            };

            enum eQuests
            {
                Quest_StormheimsSalvation = 40005
            };

            ObjectGuid m_SummonerGuid;
            Position const m_FlyAfterCirclePos = { 2489.71f, 946.526f, 596.2235f };
            bool m_CanReturnToLand = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                p_Player->KilledMonsterCredit(eKillcredits::Killcredit_VethirGossipSelected);

                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_SummonerGuid = p_Player->GetGUID();

                    p_Player->CastSpell(l_Creature, eSpells::Spell_RideVehicle, true);
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Vethir[11] =
                        {
                            { 2488.99f,  959.9774f, 251.612f  },
                            { 2502.353f, 970.2066f, 259.7113f },
                            { 2525.488f, 981.6371f, 274.458f  },
                            { 2590.089f, 970.3507f, 310.3419f },
                            { 2605.459f, 861.6927f, 405.3873f },
                            { 2563.982f, 801.2205f, 451.652f  },
                            { 2510.543f, 757.8143f, 514.5131f },
                            { 2425.472f, 762.6337f, 587.8784f },
                            { 2391.108f, 898.8264f, 637.5467f },
                            { 2425.408f, 957.6354f, 620.3625f },
                            { 2487.24f,  945.2153f, 603.3312f }
                        };

                        l_Creature->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyOnTopEnd, Path_Vethir, 11);
                    }, 1000);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(0, l_SummonerGuid);
                        }
                    }, 2000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_FlyOnTopEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            l_Player->ExitVehicle();
                        }

                        me->AddDelayedEvent([this]() -> void
                        {
                            G3D::Vector3 const Path_Vethir[13] =
                            {
                                { 2501.122f, 937.9757f, 619.1315f },
                                { 2518.854f, 916.7361f, 645.6746f },
                                { 2523.48f, 890.5261f, 660.7466f },
                                { 2510.491f, 866.7518f, 670.3904f },
                                { 2502.292f, 854.3715f, 670.5589f },
                                { 2477.597f, 856.4549f, 668.7033f },
                                { 2406.931f, 918.2518f, 668.7127f },
                                { 2395.729f, 958.9132f, 667.2391f },
                                { 2427.644f, 989.1788f, 666.5181f },
                                { 2467.33f, 976.5313f, 665.5366f },
                                { 2529.943f, 911.9167f, 670.0196f },
                                { 2530.667f, 877.7917f, 670.9f },
                                { 2508.977f, 858.2952f, 671.3801f }
                            };

                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CircleFlyEnd, Path_Vethir, 13);
                        }, 1000);

                        break;
                    }
                    case ePoints::Point_CircleFlyEnd:
                    {
                        if (!m_CanReturnToLand)
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CircleFlyEnd, Path_VethirCircle, 9);
                        break;
                    }
                    case ePoints::Point_FlyAfterCircleEnd:
                    {
                        uint64 l_SummonerGuid = m_SummonerGuid;

                        me->SetDisableGravity(false);
                        me->RemoveFlag(UNIT_FIELD_BYTES_1, 50331648);
                        me->SetFacingTo(3.874631f);

                        me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                            {
                                Talk(1, l_SummonerGuid, 1);
                                l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_VethirLanded);
                            }

                            me->DespawnOrUnsummon();
                        }, 10000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player || p_Spell->Id != eSpells::Spell_VethirLandTrigger || p_Caster->GetGUID() != m_SummonerGuid)
                    return;

                m_CanReturnToLand = true;
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyAfterCircleEnd, m_FlyAfterCirclePos);
            }

            void UpdateAI(const uint32 /*diff*/) override
            {
                if (!me->isSummon())
                    return;

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_StormheimsSalvation))
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vethir_97986AI(p_Creature);
        }
};

/// God-King Skovald - 92307
class npc_god_king_skovald_92307 : public CreatureScript
{
    public:
        npc_god_king_skovald_92307() : CreatureScript("npc_god_king_skovald_92307") { }

        struct npc_god_king_skovald_92307AI : public ScriptedAI
        {
            npc_god_king_skovald_92307AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_FelStrike         = 191132,
                Spell_SummonFelOrb      = 191161,
                Spell_OpenGatewayAura   = 190586,
                Spell_DemonicBindings   = 214298,
                Spell_VethirLandTrigger = 193504
            };

            enum eEvents
            {
                Event_CastFelStrike    = 1,
                Event_CastSummonFelOrb = 2,
                Event_OpenFirstGates   = 3,
                Event_OpenSecondGates  = 4,
                Event_OpenThirdGates   = 5,
                Event_OpenFourthGates  = 6
            };

            enum eNpcs
            {
                Npc_DemonicGateway     = 96080,
                Npc_FelskornChanneler  = 96489,
                Npc_FelskornReaver     = 96491,
                Npc_EngulfedFelstalker = 96090,
                Npc_FelOrb             = 96091
            };

            enum eGobs
            {
                Gob_Gateway = 243799
            };

            enum ePoints
            {
                Point_PreJumpMoveEnd = 1,
                Point_JumpEnd        = 2,
                Point_FlyUpEnd       = 3,
                Point_LeavePathEnd   = 4
            };

            enum eKillcredits
            {
                Killcredit_GodKingSkovaldDefeated = 92307
            };

            EventMap m_Events;
            Position const m_PreJumpPos = { 2442.446f, 903.1895f, 596.2234f };
            Position const m_JumpPos    = { 2399.941f, 858.7778f, 535.8759f };
            Position const m_FlyUpPos   = { 2414.731f, 873.7188f, 625.5952f };
            Position const m_RespawnPos = { 2449.323f, 907.8177f, 596.2245f, 0.7244619f };
            bool m_CanStartLeaveScene   = false;

            void DespawnSummons()
            {
                m_Events.Reset();

                std::list<Creature*> l_CreatureList;
                me->GetCreatureListInGrid(l_CreatureList, 100.0f);

                if (l_CreatureList.empty())
                    return;

                for (auto itr : l_CreatureList)
                {
                    switch (itr->GetEntry())
                    {
                    case eNpcs::Npc_FelskornChanneler:
                    case eNpcs::Npc_FelskornReaver:
                    case eNpcs::Npc_EngulfedFelstalker:
                    case eNpcs::Npc_FelOrb:
                    {
                        itr->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                    }
                }

                std::list<GameObject*> l_GameObjectList;
                me->GetGameObjectListWithEntryInGrid(l_GameObjectList, eGobs::Gob_Gateway, 100.0f);

                if (l_GameObjectList.empty())
                    return;

                for (auto itr : l_GameObjectList)
                {
                    itr->SendGameObjectActivateAnimKit(0);
                }
            }

            void Reset() override
            {
                DespawnSummons();
                m_CanStartLeaveScene = false;
            }

            void JustRespawned() override
            {
                m_CanStartLeaveScene = false;
                me->NearTeleportTo(m_RespawnPos);
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastFelStrike,    6000);
                m_Events.ScheduleEvent(eEvents::Event_CastSummonFelOrb, 10000);
                m_Events.ScheduleEvent(eEvents::Event_OpenFirstGates,   15000);
                m_Events.ScheduleEvent(eEvents::Event_OpenSecondGates,  42000);
                m_Events.ScheduleEvent(eEvents::Event_OpenThirdGates,   62000);
                m_Events.ScheduleEvent(eEvents::Event_OpenFourthGates,  93000);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetHealth(1);
                }

                if (me->GetHealthPct() <= 20 && !m_CanStartLeaveScene)
                {
                    m_CanStartLeaveScene = true;
                    Talk(4);
                    me->CastStop();
                    DoCastAOE(eSpells::Spell_DemonicBindings, true);
                    DespawnSummons();
                    me->RemoveAllAuras();
                    me->StopAttack();
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_PreJumpMoveEnd, m_PreJumpPos);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_PreJumpMoveEnd:
                    {
                        for (auto itr : me->getThreatManager().getThreatList())
                        {
                            if (Unit* l_Unit = itr->getTarget())
                            {
                                if (Player* l_Player = l_Unit->ToPlayer())
                                {
                                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_GodKingSkovaldDefeated);
                                    l_Player->CastSpell(l_Player, eSpells::Spell_VethirLandTrigger, true);
                                }
                            }
                        }

                        me->SetDisableGravity(true);
                        me->GetMotionMaster()->MoveJump(m_JumpPos, 30.0f, 15.0f, ePoints::Point_JumpEnd);
                        break;
                    }
                    case ePoints::Point_JumpEnd:
                    {
                        me->Mount(70361);
                        me->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                        me->SetSpeed(MOVE_RUN, 3.0f);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, m_FlyUpPos);
                        break;
                    }
                    case ePoints::Point_FlyUpEnd:
                    {
                        Talk(5);

                        me->AddDelayedEvent([this]() -> void
                        {
                            Position const l_LeavePos = { 2568.863f, 1032.641f, 675.62f };

                            DespawnSummons();
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LeavePathEnd, l_LeavePos);
                        }, 7000);

                        break;
                    }
                    case ePoints::Point_LeavePathEnd:
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
                if (!m_CanStartLeaveScene && !UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastFelStrike:
                    {
                        DoCastVictim(eSpells::Spell_FelStrike);
                        m_Events.ScheduleEvent(eEvents::Event_CastFelStrike, urand(8000, 13000));
                        break;
                    }
                    case eEvents::Event_CastSummonFelOrb:
                    {
                        DoCastVictim(eSpells::Spell_SummonFelOrb);
                        m_Events.ScheduleEvent(eEvents::Event_CastSummonFelOrb, urand(19000, 23000));
                        break;
                    }
                    case eEvents::Event_OpenFirstGates:
                    {
                        Talk(1);

                        std::list<Creature*> l_CreatureList;
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_DemonicGateway, 100.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            if (int32(itr->GetPositionX()) == 2444)
                            {
                                DoCast(itr, eSpells::Spell_OpenGatewayAura, true);

                                if (GameObject* l_Gob = itr->FindNearestGameObject(eGobs::Gob_Gateway, 10.0f))
                                {
                                    l_Gob->SendGameObjectActivateAnimKit(8602);
                                }
                            }
                        }

                        break;
                    }
                    case eEvents::Event_OpenSecondGates:
                    {
                        Talk(2);

                        std::list<Creature*> l_CreatureList;
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_DemonicGateway, 100.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            if (int32(itr->GetPositionX()) == 2415)
                            {
                                DoCast(itr, eSpells::Spell_OpenGatewayAura, true);

                                if (GameObject* l_Gob = itr->FindNearestGameObject(eGobs::Gob_Gateway, 10.0f))
                                {
                                    l_Gob->SendGameObjectActivateAnimKit(8602);
                                }
                            }
                        }

                        break;
                    }
                    case eEvents::Event_OpenThirdGates:
                    {
                        Talk(1);

                        std::list<Creature*> l_CreatureList;
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_DemonicGateway, 100.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            if (int32(itr->GetPositionX()) == 2477)
                            {
                                DoCast(itr, eSpells::Spell_OpenGatewayAura, true);

                                if (GameObject* l_Gob = itr->FindNearestGameObject(eGobs::Gob_Gateway, 10.0f))
                                {
                                    l_Gob->SendGameObjectActivateAnimKit(8602);
                                }
                            }
                        }

                        break;
                    }
                    case eEvents::Event_OpenFourthGates:
                    {
                        Talk(3);

                        std::list<Creature*> l_CreatureList;
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_DemonicGateway, 100.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            if (int32(itr->GetPositionX()) == 2509)
                            {
                                DoCast(itr, eSpells::Spell_OpenGatewayAura, true);

                                if (GameObject* l_Gob = itr->FindNearestGameObject(eGobs::Gob_Gateway, 10.0f))
                                {
                                    l_Gob->SendGameObjectActivateAnimKit(8602);
                                }
                            }
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
            return new npc_god_king_skovald_92307AI(p_Creature);
        }
};

/// Vethir - 98190
class npc_vethir_98190 : public CreatureScript
{
    public:
        npc_vethir_98190() : CreatureScript("npc_vethir_98190") { }

        struct npc_vethir_98190AI : public VehicleAI
        {
            npc_vethir_98190AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            enum eSpells
            {
                Spell_RideVehicle                 = 52391,
                Spell_ConversationSkovaldDefeated = 218272
            };

            enum eKillcredits
            {
                Killcredit_VethirTopVisible = 4000502,
                Killcredit_VethirLanded     = 4000501
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                p_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_VethirTopVisible);

                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_SummonerGuid = p_Player->GetGUID();

                    p_Player->CastSpell(l_Creature, eSpells::Spell_RideVehicle, true);
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        G3D::Vector3 const Path_Vethir[14] =
                        {
                            { 2488.314f, 945.1337f, 606.3078f },
                            { 2501.672f, 938.6389f, 624.0499f },
                            { 2528.109f, 898.6702f, 651.5321f },
                            { 2511.556f, 859.882f,  665.2982f },
                            { 2475.401f, 843.1285f, 668.2026f },
                            { 2388.54f,  890.8368f, 672.5418f },
                            { 2389.413f, 988.7066f, 662.0587f },
                            { 2403.113f, 1021.99f,  591.8265f },
                            { 2441.509f, 1058.45f,  435.517f  },
                            { 2484.752f, 1073.186f, 336.3525f },
                            { 2549.932f, 1061.439f, 292.0263f },
                            { 2529.335f, 992.691f,  267.692f  },
                            { 2489.115f, 959.9479f, 247.0776f },
                            { 2489.115f, 959.9479f, 241.6235f }
                        };

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Player->CastSpell(l_Player, eSpells::Spell_ConversationSkovaldDefeated, true);
                        }

                        l_Creature->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, Path_Vethir, 14);
                    }, 1000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_FlyEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            l_Player->ExitVehicle();
                        }

                        me->RemoveFlag(UNIT_FIELD_BYTES_1, 50331648);
                        me->SetDisableGravity(false);
                        me->SetFacingTo(0.8028514f);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_VethirLanded);
                        }

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
            return new npc_vethir_98190AI(p_Creature);
        }
};

/// Havi - 97988
class npc_havi_97988 : public CreatureScript
{
    public:
        npc_havi_97988() : CreatureScript("npc_havi_97988") { }

        struct npc_havi_97988AI : public ScriptedAI
        {
            npc_havi_97988AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            enum eSpells
            {
                Spell_HaviTransform             = 193518,
                Spell_ConversationHaviTransform = 218288
            };

            enum eKillcredits
            {
                Killcredit_OdynVisible = 4007200
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_SummonerGuid = p_Player->GetGUID();

                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        G3D::Vector3 const Path_Havi[4] =
                        {
                            { 2489.816f, 944.5427f, 241.8677f },
                            { 2487.066f, 940.2927f, 241.8677f },
                            { 2482.816f, 933.5427f, 241.8677f },
                            { 2480.24f,  929.648f,  241.6235f }
                        };

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(0, l_SummonerGuid);
                        }

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_Havi, 4, true);
                    }, 2000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_MoveEnd:
                    {
                        uint64 l_SummonerGuid = m_SummonerGuid;

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            me->SetFacingToObject(l_Player);
                        }

                        me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                        {
                            me->CastSpell(me, eSpells::Spell_HaviTransform, true);

                            if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                            {
                                l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_OdynVisible);
                                l_Player->CastSpell(l_Player, eSpells::Spell_ConversationHaviTransform, true);
                            }

                            me->DespawnOrUnsummon();
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
            return new npc_havi_97988AI(p_Creature);
        }
};

/// Grappling Hook Bunny - 92315
class npc_grappling_hook_bunny_92315 : public CreatureScript
{
    public:
        npc_grappling_hook_bunny_92315() : CreatureScript("npc_grappling_hook_bunny_92315") { }

        struct npc_grappling_hook_bunny_92315AI : public ScriptedAI
        {
            npc_grappling_hook_bunny_92315AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_GrapplingHookVisual = 182959,
                Spell_RideDragon          = 182990
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(eSpells::Spell_GrapplingHookVisual, true);
                DoCast(eSpells::Spell_RideDragon, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->ExitVehicle();
                    me->DespawnOrUnsummon();
                }, 6000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_grappling_hook_bunny_92315AI(p_Creature);
        }
};

/// Scovald Scene Trigger - 3861500
class npc_skovald_scene_trigger_3861500 : public CreatureScript
{
    public:
        npc_skovald_scene_trigger_3861500() : CreatureScript("npc_skovald_scene_trigger_3861500") { }

        struct npc_skovald_scene_trigger_3861500AI : public ScriptedAI
        {
            npc_skovald_scene_trigger_3861500AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ImpalementInsuranceHorde    = 38615,
                Quest_ImpalementInsuranceAlliance = 38410
            };

            enum eObjectives
            {
                Obj_SkovaldEventSeenHorde         = 3861500,
                Obj_SkovaldEventSeenAlliance      = 3861502,
            };

            enum eSpells
            {
                Spell_GodKingScene             = 183296,
                Spell_ConversationGodKingScene = 215685
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && (l_Player->GetQuestStatus(eQuests::Quest_ImpalementInsuranceHorde) == QUEST_STATUS_COMPLETE ||
                    l_Player->GetQuestStatus(eQuests::Quest_ImpalementInsuranceAlliance) == QUEST_STATUS_COMPLETE) &&
                    (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SkovaldEventSeenHorde) == 0 && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SkovaldEventSeenAlliance) == 0))
                {
                    l_Player->CastSpell(l_Player, eSpells::Spell_GodKingScene, true);
                    l_Player->CastSpell(l_Player, eSpells::Spell_ConversationGodKingScene, true);
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_SkovaldEventSeenHorde);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skovald_scene_trigger_3861500AI(p_Creature);
        }
};

/// Sky Admiral Rogers - 96644
class npc_sky_admiral_rogers_96644 : public CreatureScript
{
    public:
        npc_sky_admiral_rogers_96644() : CreatureScript("npc_sky_admiral_rogers_96644") { }

        struct npc_sky_admiral_rogers_96644AI : public ScriptedAI
        {
            npc_sky_admiral_rogers_96644AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_RogersToCannon[11] =
            {
                { -8489.277f, 1395.926f, 196.37387f },
                { -8489.234f, 1398.122f, 196.5913f  },
                { -8488.734f, 1400.372f, 196.8413f  },
                { -8490.85f,  1401.769f, 196.5979f  },
                { -8498.67f,  1401.434f, 193.9571f  },
                { -8501.42f,  1401.434f, 193.9571f  },
                { -8504.67f,  1401.434f, 194.2071f  },
                { -8505.756f, 1401.198f, 193.6589f  },
                { -8509.866f, 1401.064f, 193.6422f  },
                { -8514.906f, 1400.9f,   193.8808f  },
                { -8516.243f, 1400.856f, 194.0575f  }
            };

            G3D::Vector3 const Path_RogersToFacing[8] =
            {
                { -8516.154f, 1398.03f,  194.2441f },
                { -8516.154f, 1396.03f,  194.2441f },
                { -8516.154f, 1390.53f,  194.2441f },
                { -8516.185f, 1388.776f, 193.9456f },
                { -8522.66f,  1388.333f, 194.0439f },
                { -8525.66f,  1388.333f, 194.0439f },
                { -8527.66f,  1388.333f, 193.7939f },
                { -8530.135f, 1387.891f, 193.6422f }
            };

            G3D::Vector3 const Path_RogersToDragoons[8] =
            {
                { -8536.488f, 1392.011f, 193.8922f },
                { -8537.488f, 1392.511f, 194.1422f },
                { -8539.238f, 1393.261f, 194.1422f },
                { -8539.738f, 1393.511f, 193.8922f },
                { -8542.342f, 1394.632f, 193.6422f },
                { -8544.138f, 1397.296f, 193.9648f },
                { -8545.638f, 1399.296f, 193.9648f },
                { -8546.32f,  1399.887f, 193.7919f }
            };

            G3D::Vector3 const Path_RogersToGenn[19] =
            {
                { -8550.459f, 1411.355f, 193.6885f },
                { -8551.178f, 1411.525f, 193.9402f },
                { -8553.178f, 1411.775f, 193.6902f },
                { -8553.678f, 1412.025f, 194.1902f },
                { -8557.178f, 1412.775f, 193.9402f },
                { -8558.887f, 1412.978f, 193.6422f },
                { -8560.46f,  1412.797f, 193.9092f },
                { -8562.71f,  1412.547f, 193.9092f },
                { -8564.21f,  1412.297f, 193.9092f },
                { -8565.158f, 1412.122f, 194.0422f },
                { -8566.256f, 1411.635f, 194.8922f },
                { -8569.006f, 1409.885f, 195.8922f },
                { -8570.756f, 1408.885f, 196.8922f },
                { -8571.799f, 1408.246f, 196.6422f },
                { -8573.08f,  1407.005f, 196.7249f },
                { -8577.356f, 1402.391f, 196.6422f },
                { -8580.763f, 1398.817f, 196.6657f },
                { -8581.193f, 1398.884f, 196.8649f },
                { -8583.452f, 1398.534f, 196.5767f }
            };

            enum ePoints
            {
                Point_MoveToPortalEnd   = 1,
                Point_PathToCannonEnd   = 2,
                Point_PathToFacingEnd   = 3,
                Point_PathToDragoonsEnd = 4,
                Point_PathToGennEnd     = 5
            };

            enum eSpells
            {
                Spell_TeleportVisual                 = 164233,
                Spell_ConversationRogersWalkToCannon = 220090,
                Spell_DummyAura                      = 94562,
                Spell_ConversationRogersWalkToGenn   = 220091
            };

            enum eActions
            {
                Action_RogersDalaranScene        = 1,
                Action_RogersSkyfyrePrepareScene = 2
            };

            enum eEvents
            {
                Event_StartPathToCannon   = 1,
                Event_StartPathToFacing   = 2,
                Event_SayThirdLine        = 3,
                Event_StartPathToDragoons = 4,
                Event_StartPathToGenn     = 5,
                Event_PlayEmote           = 6,
                Event_SayLastLine         = 7
            };

            enum eQuests
            {
                Quest_ARoyalSummons   = 38035,
                Quest_MakingTheRounds = 38206,
            };

            enum eNpcs
            {
                Npc_GennGreymane = 96663
            };

            enum eObjectives
            {
                Obj_InspectDeckGuns      = 279839,
                Obj_SaluteLegionDragoons = 279840
            };

            enum eKillcredits
            {
                Killcredit_SpeakWithGennGreymane = 96663
            };

            EventMap m_Events;
            ObjectGuid m_SummonerGuid;
            ObjectGuid m_GennGuid;
            Position m_GennSpawnPos      = { -8587.887f, 1398.61f, 196.6643f, 3.182192f };
            bool m_PathToCannonEnded     = false;
            bool m_PathToDragoonsStarted = false;
            bool m_PathToDragoonsEnded   = false;
            bool m_PathToGennStarted     = false;

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_RogersDalaranScene:
                    {
                        uint64 l_PlayerGuid = m_SummonerGuid;

                        me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            {
                                me->SetFacingToObject(l_Player);
                                Talk(0, l_PlayerGuid);
                            }
                        }, 20000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToPortalEnd, -831.6406f, 4653.471f, 749.7227f);
                        }, 26000);

                        break;
                    }
                    case eActions::Action_RogersSkyfyrePrepareScene:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_GennGreymane, m_GennSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, 2);
                                m_GennGuid = l_Creature->GetGUID();
                            }
                        }

                        m_Events.ScheduleEvent(eEvents::Event_StartPathToCannon, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, 3);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_MoveToPortalEnd:
                    {
                        DoCast(eSpells::Spell_TeleportVisual, true);
                        me->DespawnOrUnsummon();
                        break;
                    }
                    case ePoints::Point_PathToCannonEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            Talk(1, m_SummonerGuid);
                        }

                        m_PathToCannonEnded = true;
                        break;
                    }
                    case ePoints::Point_PathToFacingEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            Talk(3, m_SummonerGuid);
                            me->SetFacingToObject(l_Player);
                        }

                        m_Events.ScheduleEvent(eEvents::Event_StartPathToDragoons, 3000);
                        break;
                    }
                    case ePoints::Point_PathToDragoonsEnd:
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        {
                            Talk(4, m_SummonerGuid);
                            l_Player->CastSpell(l_Player, eSpells::Spell_DummyAura, true);
                        }

                        me->SetFacingTo(3.156537f);
                        m_PathToDragoonsEnded = true;
                        break;
                    }
                    case ePoints::Point_PathToGennEnd:
                    {
                        me->SetFacingTo(3.12163f);
                        m_Events.ScheduleEvent(eEvents::Event_PlayEmote, 1000);
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
                    case eQuests::Quest_MakingTheRounds:
                    {
                        if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->AI()->DoAction(eActions::Action_RogersSkyfyrePrepareScene);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isSummon())
                    return;

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || (!l_Player->HasQuest(eQuests::Quest_MakingTheRounds) &&
                    !l_Player->HasQuest(eQuests::Quest_ARoyalSummons)))
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_GennGuid))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }

                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_InspectDeckGuns) == 1 && !m_PathToDragoonsStarted && m_PathToCannonEnded)
                {
                    m_Events.ScheduleEvent(eEvents::Event_StartPathToFacing, 1000);
                    m_PathToDragoonsStarted = true;
                }

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SaluteLegionDragoons) == 1 && !m_PathToGennStarted && m_PathToDragoonsEnded)
                {
                    m_Events.ScheduleEvent(eEvents::Event_StartPathToGenn, 1000);
                    m_PathToGennStarted = true;
                }

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_StartPathToCannon:
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_ConversationRogersWalkToCannon, true);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToCannonEnd, Path_RogersToCannon, 11, true);
                        break;
                    }
                    case eEvents::Event_StartPathToFacing:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToFacingEnd, Path_RogersToFacing, 8, true);
                        m_Events.ScheduleEvent(eEvents::Event_SayThirdLine, 5000);
                        break;
                    }
                    case eEvents::Event_SayThirdLine:
                    {
                        Talk(2, m_SummonerGuid);
                        break;
                    }
                    case eEvents::Event_StartPathToDragoons:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToDragoonsEnd, Path_RogersToDragoons, 8, true);
                        break;
                    }
                    case eEvents::Event_StartPathToGenn:
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_ConversationRogersWalkToGenn, true);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathToGennEnd, Path_RogersToGenn, 19, true);
                        break;
                    }
                    case eEvents::Event_PlayEmote:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_GennGuid))
                        {
                            uint64 l_PlayerGuid = m_SummonerGuid;

                            l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                            {
                                l_Creature->SetFacingTo(0.01494408f);

                                if (Player* l_Pl = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    l_Creature->AI()->Talk(0, l_PlayerGuid);
                            }, 1000);

                            l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                            {
                                if (Player* l_Pl = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    l_Creature->AI()->Talk(1, l_PlayerGuid);
                            }, 3500);
                        }

                        me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                        m_Events.ScheduleEvent(eEvents::Event_SayLastLine, 11000);
                        break;
                    }
                    case eEvents::Event_SayLastLine:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_GennGuid))
                        {
                            l_Creature->DespawnOrUnsummon();
                        }

                        Talk(5, m_SummonerGuid);
                        me->DespawnOrUnsummon();
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SpeakWithGennGreymane);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_sky_admiral_rogers_96644AI(p_Creature);
        }
};

/// Dread-Rider Malwick - 90902
class npc_dread_rider_malwick_90902 : public CreatureScript
{
    public:
        npc_dread_rider_malwick_90902() : CreatureScript("npc_dread_rider_malwick_90902") { }

        struct npc_dread_rider_malwick_90902AI : public ScriptedAI
        {
            npc_dread_rider_malwick_90902AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eActions
            {
                Action_RidingBatStartFly = 1,
                Action_RidingBatEndFly   = 2
            };

            enum eSpells
            {
                Spell_OverhandSlash = 183202,
                Spell_PlaguedSpit   = 183146,
                Spell_RideVehicle   = 52391
            };

            enum eEvents
            {
                Event_CastOverhandSlash = 1,
                Event_CastPlaguedSpit   = 2
            };

            EventMap m_Events;
            bool m_CombatStarted = false;

            void Reset() override
            {
                m_Events.Reset();

                if (m_CombatStarted)
                {
                    if (Unit* l_Unit = me->GetAnyOwner())
                    {
                        if (l_Unit->ToCreature())
                        {
                            l_Unit->ToCreature()->AI()->DoAction(eActions::Action_RidingBatEndFly);
                        }
                    }

                    m_CombatStarted = false;
                    me->DespawnOrUnsummon();
                }
            }

            void EnterCombat(Unit* victim) override
            {
                me->ExitVehicle();

                if (Unit* l_Unit = me->GetAnyOwner())
                {
                    if (l_Unit->ToCreature())
                    {
                        l_Unit->ToCreature()->AI()->DoAction(eActions::Action_RidingBatStartFly);
                    }
                }

                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastOverhandSlash, 4000);
                m_Events.ScheduleEvent(eEvents::Event_CastPlaguedSpit, 6000);
                m_CombatStarted = true;
            }

            void JustDied(Unit* p_Killer) override
            {
                if (Unit* l_Unit = me->GetAnyOwner())
                {
                    if (l_Unit->ToCreature())
                    {
                        l_Unit->ToCreature()->AI()->DoAction(eActions::Action_RidingBatEndFly);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastOverhandSlash:
                    {
                        DoCast(eSpells::Spell_OverhandSlash);
                        m_Events.ScheduleEvent(eEvents::Event_CastOverhandSlash, urand(10000, 13000));
                        break;
                    }
                    case eEvents::Event_CastPlaguedSpit:
                    {
                        DoCast(eSpells::Spell_PlaguedSpit);
                        m_Events.ScheduleEvent(eEvents::Event_CastPlaguedSpit, urand(10000, 11000));
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
            return new npc_dread_rider_malwick_90902AI(p_Creature);
        }
};

/// Riding Bat - 92407
class npc_riding_bat_92407 : public CreatureScript
{
    public:
        npc_riding_bat_92407() : CreatureScript("npc_riding_bat_92407") { }

        struct npc_riding_bat_92407AI : public VehicleAI
        {
            npc_riding_bat_92407AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_RidingBatFromLand[4] =
            {
                { 3080.976f, 2978.908f, 436.8046f },
                { 3089.107f, 2983.911f, 446.068f  },
                { 3098.531f, 2983.804f, 455.1244f },
                { 3084.806f, 2998.129f, 457.7018f }
            };

            G3D::Vector3 const Path_RidingBatCircle[7] =
            {
                { 3082.325f, 2996.306f, 451.7862f },
                { 3063.456f, 2975.92f,  451.9156f },
                { 3067.494f, 2963.513f, 453.6686f },
                { 3082.62f,  2959.635f, 457.7527f },
                { 3101.072f, 2982.522f, 459.0074f },
                { 3094.597f, 2995.193f, 454.1658f },
                { 3083.509f, 2997.338f, 451.6741f }
            };

            enum eSpells
            {
                Spell_PlaguedSpit        = 183151,
                Spell_PlaguedSpitMissile = 183145
            };

            enum eActions
            {
                Action_RidingBatStartFly = 1,
                Action_RidingBatEndFly   = 2
            };

            enum ePoints
            {
                Point_MoveFromLandEnd = 1,
                Point_CirclePathEnd   = 2
            };

            bool m_CanRepeat = true;

            void Reset() override
            {
                me->NearTeleportTo(me->GetHomePosition());
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_MoveFromLandEnd:
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_RidingBatCircle, 7);
                        break;
                    }
                    case ePoints::Point_CirclePathEnd:
                    {
                        if (m_CanRepeat)
                        {
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_RidingBatCircle, 7);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player || p_Spell->Id != eSpells::Spell_PlaguedSpit)
                    return;

                DoCast(l_Player, eSpells::Spell_PlaguedSpitMissile);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_RidingBatStartFly:
                    {
                        m_CanRepeat = true;
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_MoveFromLandEnd, Path_RidingBatFromLand, 4);
                        break;
                    }
                    case eActions::Action_RidingBatEndFly:
                    {
                        m_CanRepeat = false;
                        me->GetMotionMaster()->MoveIdle();
                        me->GetMotionMaster()->Clear();
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
            return new npc_riding_bat_92407AI(p_Creature);
        }
};

/// 7th Legion Dragoon - 90948
class npc_legion_dragoon_90948 : public CreatureScript
{
    public:
        npc_legion_dragoon_90948() : CreatureScript("npc_legion_dragoon_90948") { }

        struct npc_legion_dragoon_90948AI : public ScriptedAI
        {
            npc_legion_dragoon_90948AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_ReleasingStaticField = 180737
            };

            enum eKillcredits
            {
                Killcredit_DragoonsReleased = 90948
            };

            void Reset() override
            {
                if (!me->isSummon())
                {
                    me->SetUInt32Value(UNIT_FIELD_STATE_SPELL_VISUAL_ID, 45944);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player || p_Spell->Id != eSpells::Spell_ReleasingStaticField)
                    return;

                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_DragoonsReleased);
                me->DestroyForPlayer(l_Player);

                if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                {
                    l_Creature->SetUInt32Value(UNIT_FIELD_DISPLAY_ID, me->GetUInt32Value(UNIT_FIELD_DISPLAY_ID));
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    l_Creature->SetUInt32Value(UNIT_FIELD_STATE_SPELL_VISUAL_ID, 0);
                    l_Creature->SetUInt32Value(UNIT_FIELD_STATE_ANIM_KIT_ID, 6050);
                    l_Creature->AI()->Talk(0, l_Player->GetGUID());
                    l_Creature->DespawnOrUnsummon(5000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_legion_dragoon_90948AI(p_Creature);
        }
};

/// Forsaken Dreadwing - 92516
class npc_forsaken_dreadwing_92516 : public CreatureScript
{
    public:
        npc_forsaken_dreadwing_92516() : CreatureScript("npc_forsaken_dreadwing_92516") { }

        struct npc_forsaken_dreadwing_92516AI : public VehicleAI
        {
            npc_forsaken_dreadwing_92516AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_ForsakenDreadwing[2] =
            {
                { 3272.164f, 3167.58f,  541.606f },
                { 3243.825f, 3174.507f, 541.606f }
            };

            enum eSpells
            {
                Spell_RideVehicle = 183363
            };

            enum ePoints
            {
                Point_FlyUpEnd    = 1,
                Point_FlyPathEnd  = 2,
                Point_LandPathEnd = 3
            };

            enum eActions
            {
                Action_FirstDreadwing  = 1,
                Action_SecondDreadwing = 2
            };

            ObjectGuid m_SummonerGuid;
            Position const m_FlyUpPos = { 3267.019f, 3135.761f, 541.606f };
            bool m_MeIsFirst  = false;
            bool m_MeIsSecond = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetSpeed(MOVE_FLIGHT, 1.0f);
                m_SummonerGuid = p_Summoner->GetGUID();
                p_Summoner->CastSpell(me, eSpells::Spell_RideVehicle, true);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, m_FlyUpPos);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_FirstDreadwing:
                    {
                        m_MeIsFirst = true;
                        break;
                    }
                    case eActions::Action_SecondDreadwing:
                    {
                        m_MeIsSecond = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_FlyUpEnd:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_SummonerGuid))
                        {

                            l_Creature->ExitVehicle();
                            l_Creature->GetMotionMaster()->MoveFall();

                            if (m_MeIsFirst)
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_FirstDeathstalker[7] =
                                    {
                                        { 3263.92f, 3135.823f,  526.9578f },
                                        { 3263.17f, 3137.823f,  526.4578f },
                                        { 3261.92f, 3140.573f,  526.7078f },
                                        { 3260.17f, 3145.073f,  526.9578f },
                                        { 3258.67f, 3148.573f,  526.9578f },
                                        { 3257.17f, 3152.323f,  527.4578f },
                                        { 3252.591f, 3162.813f, 526.9484f }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_LandPathEnd, Path_FirstDeathstalker, 7, false);
                                }, 2000);
                            }
                            else if (m_MeIsSecond)
                            {
                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_SecondDeathstalker[9] =
                                    {
                                        { 3265.504f, 3133.75f,  527.3416f },
                                        { 3264.254f, 3136.5f,   527.0916f },
                                        { 3263.754f, 3137.5f,   526.5916f },
                                        { 3262.754f, 3140.25f,  526.5916f },
                                        { 3260.254f, 3145.75f,  526.8416f },
                                        { 3259.254f, 3148.25f,  527.0916f },
                                        { 3258.504f, 3150.0f,   527.0916f },
                                        { 3257.254f, 3152.75f,  527.3416f },
                                        { 3252.677f, 3162.849f, 526.9545f }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_LandPathEnd, Path_SecondDeathstalker, 9, false);
                                }, 2000);
                            }
                        }

                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_ForsakenDreadwing, 2);
                        break;
                    }
                    case ePoints::Point_FlyPathEnd:
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
            return new npc_forsaken_dreadwing_92516AI(p_Creature);
        }
};

/// Mind-Controlled Dreadwing - 92547
class npc_mind_controlled_dreadwing_92547 : public CreatureScript
{
    public:
        npc_mind_controlled_dreadwing_92547() : CreatureScript("npc_mind_controlled_dreadwing_92547") { }

        struct npc_mind_controlled_dreadwing_92547AI : public VehicleAI
        {
            npc_mind_controlled_dreadwing_92547AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_DreadwingCircle[7] =
            {
                { 3255.517f, 3164.878f, 532.5855f },
                { 3283.716f, 3147.13f,  538.4818f },
                { 3274.4f,   3132.647f, 538.9511f },
                { 3252.925f, 3178.733f, 534.8672f },
                { 3220.17f,  3187.375f, 541.2244f },
                { 3228.753f, 3197.353f, 539.9489f },
                { 3240.858f, 3172.839f, 535.222f  }
            };

            enum eSpells
            {
                Spell_MindControlHelmet = 183481
            };

            enum ePoints
            {
                Point_InitPathEnd     = 1,
                Point_CirclePathEnd   = 2,
                Point_PrePathEnd      = 3,
                Point_EventEndPathEnd = 4
            };

            enum eActions
            {
                Action_StartEventEndPath = 3
            };

            Position const m_PreLeaveFlyPos = { 3263.91f, 3157.495f, 536.2158f };
            bool m_CanRepeat = true;

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_MindControlHelmet);
                me->SetSpeed(MOVE_FLIGHT, 2.0f);

                me->AddDelayedEvent([this]() -> void
                {
                    if (me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = me->GetVehicleKit()->GetPassenger(1))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ToCreature()->AI()->Talk(0, 0, 1);
                            }
                        }
                    }

                    G3D::Vector3 const Path_DreadwingInit[3] =
                    {
                        { 3203.468f, 3123.149f, 512.3557f },
                        { 3206.988f, 3145.468f, 532.8476f },
                        { 3232.116f, 3167.839f, 533.2913f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_InitPathEnd, Path_DreadwingInit, 3);
                }, 1000);

                me->AddDelayedEvent([this]() -> void
                {
                    if (me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = me->GetVehicleKit()->GetPassenger(0))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ToCreature()->AI()->Talk(0, 0, 1);
                            }
                        }
                    }
                }, 6000);

                me->AddDelayedEvent([this]() -> void
                {
                    me->HandleEmoteCommand(EMOTE_STATE_ONESHOT_MOUNT_SPECIAL);
                }, 8000);

                me->AddDelayedEvent([this]() -> void
                {
                    if (me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = me->GetVehicleKit()->GetPassenger(1))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ToCreature()->AI()->Talk(1, 0, 1);
                            }
                        }
                    }

                    G3D::Vector3 const Path_DreadwingCircle[7] =
                    {
                        { 3255.517f, 3164.878f, 532.5855f },
                        { 3283.716f, 3147.13f, 538.4818f },
                        { 3274.4f, 3132.647f, 538.9511f },
                        { 3252.925f, 3178.733f, 534.8672f },
                        { 3220.17f, 3187.375f, 541.2244f },
                        { 3228.753f, 3197.353f, 539.9489f },
                        { 3240.858f, 3172.839f, 535.222f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_DreadwingCircle, 7);
                }, 12000);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_StartEventEndPath:
                    {
                        m_CanRepeat = false;
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PrePathEnd, m_PreLeaveFlyPos);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_CirclePathEnd:
                    {
                        if (m_CanRepeat)
                        {
                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_DreadwingCircle, 7);
                        }

                        break;
                    }
                    case ePoints::Point_PrePathEnd:
                    {
                        if (me->GetVehicleKit())
                        {
                            if (Unit* l_Unit = me->GetVehicleKit()->GetPassenger(0))
                            {
                                if (l_Unit->ToCreature())
                                {
                                    l_Unit->ToCreature()->AI()->Talk(1, 1);
                                }
                            }
                        }

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->HandleEmoteCommand(EMOTE_STATE_ONESHOT_MOUNT_SPECIAL);
                        }, 2000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            G3D::Vector3 const Path_DreadwingEventEnd[3] =
                            {
                                { 3275.217f, 3144.387f, 534.6689f },
                                { 3319.468f, 3108.144f, 534.6689f },
                                { 3380.169f, 3078.576f, 534.6689f }
                            };

                            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_EventEndPathEnd, Path_DreadwingEventEnd, 3);
                        }, 5000);

                        break;
                    }
                    case ePoints::Point_EventEndPathEnd:
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
            return new npc_mind_controlled_dreadwing_92547AI(p_Creature);
        }
};

/// Vethir - 90907
class npc_vethir_90907 : public CreatureScript
{
    public:
        npc_vethir_90907() : CreatureScript("npc_vethir_90907") { }

        struct npc_vethir_90907AI : public ScriptedAI
        {
            npc_vethir_90907AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_VethirInit[2] =
            {
                { 3253.717f, 3205.005f, 568.2557f },
                { 3250.293f, 3192.031f, 568.2557f },
            };

            G3D::Vector3 const Path_VethirLand[4] =
            {
                { 3250.587f, 3187.966f, 526.5084f },
                { 3250.587f, 3183.966f, 526.5084f },
                { 3250.587f, 3179.216f, 526.5084f },
                { 3250.88f,  3168.4f,   526.4459f }
            };

            G3D::Vector3 const Path_VethirLeave[3] =
            {
                { 3259.324f, 3177.424f, 556.5145f },
                { 3262.238f, 3175.321f, 570.3306f },
                { 3180.915f, 3139.72f,  615.8902f }
            };

            enum eSpells
            {
                Spell_StaticMaelstrom  = 183423,
                Spell_LightningBreath  = 183454,
                Spell_CracklingTempest = 220129,
                Spell_Missiles         = 183426
            };

            enum eEvents
            {
                Event_CastLightningBreath  = 1,
                Event_CastCracklingTempest = 2,
                Event_CastStaticMaelstrom  = 3
            };

            enum ePoints
            {
                Point_InitPathEnd  = 1,
                Point_FlyDownEnd   = 2,
                Point_LandPathEnd  = 3,
                Point_LeavePathEnd = 4
            };

            enum eDatas
            {
                Data_GyrocoptersArrived = 3,
                Data_VethirDefeated     = 4
            };

            enum eKillcredits
            {
                Killcredit_TransponderDefended = 90907
            };

            ObjectGuid m_GobGuid;
            EventMap m_Events;
            Position const m_FlyDownPos = { 3250.294f, 3192.031f, 526.5709f };
            Position const m_LeavePos   = { 3259.324f, 3177.424f, 556.5145f };
            bool m_GyrocoptersCalled = false;
            bool m_VethirDefeated    = false;
            bool m_CombatStarted     = false;

            void InitializeAI() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 2.0f);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_InitPathEnd, Path_VethirInit, 2);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_InitPathEnd:
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyDownEnd, m_FlyDownPos);
                        break;
                    }
                    case ePoints::Point_FlyDownEnd:
                    {
                        me->RemoveFlag(UNIT_FIELD_BYTES_1, 50331648);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_LandPathEnd, Path_VethirLand, 4, false);
                        break;
                    }
                    case ePoints::Point_LandPathEnd:
                    {
                        me->SetHomePosition(me->GetPosition());
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        break;
                    }
                    case ePoints::Point_LeavePathEnd:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                m_GobGuid = p_Guid;
            }

            void EnterCombat(Unit* victim) override
            {
                m_CombatStarted = true;
                m_Events.ScheduleEvent(eEvents::Event_CastLightningBreath, 5000);
                m_Events.ScheduleEvent(eEvents::Event_CastCracklingTempest, 8000);
                m_Events.ScheduleEvent(eEvents::Event_CastStaticMaelstrom, 14000);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_Missiles && !m_VethirDefeated)
                {
                    if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_GobGuid))
                    {
                        l_Gob->AI()->SetData(0, eDatas::Data_VethirDefeated);
                    }

                    for (auto itr : me->getThreatManager().getThreatList())
                    {
                        if (Unit* l_Unit = itr->getTarget())
                        {
                            if (Player* l_Player = l_Unit->ToPlayer())
                            {
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_TransponderDefended);
                            }
                        }
                    }

                    m_VethirDefeated = true;
                    me->CastStop();
                    me->StopAttack();
                    m_Events.Reset();
                    me->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LeavePathEnd, Path_VethirLeave, 3);
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetHealth(1);
                }

                if (me->GetHealthPct() <= 60 && !m_GyrocoptersCalled)
                {
                    if (GameObject* l_Gob = ObjectAccessor::GetGameObject(*me, m_GobGuid))
                    {
                        l_Gob->AI()->SetData(0, eDatas::Data_GyrocoptersArrived);
                    }

                    m_GyrocoptersCalled = true;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CombatStarted && !UpdateVictim())
                {
                    m_Events.Reset();
                    m_CombatStarted = false;
                    return;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastLightningBreath:
                    {
                        DoCast(eSpells::Spell_LightningBreath);
                        m_Events.ScheduleEvent(eEvents::Event_CastLightningBreath, urand(8000, 13000));
                        break;
                    }
                    case eEvents::Event_CastCracklingTempest:
                    {
                        DoCastVictim(eSpells::Spell_CracklingTempest);
                        m_Events.ScheduleEvent(eEvents::Event_CastCracklingTempest, urand(10000, 16000));
                        break;
                    }
                    case eEvents::Event_CastStaticMaelstrom:
                    {
                        DoCast(eSpells::Spell_StaticMaelstrom);
                        m_Events.ScheduleEvent(eEvents::Event_CastStaticMaelstrom, 20000);
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
            return new npc_vethir_90907AI(p_Creature);
        }
};

/// Muninn - 97306
class npc_muninn_97306 : public CreatureScript
{
    public:
        npc_muninn_97306() : CreatureScript("npc_muninn_97306") { }

        struct npc_muninn_97306AI : public ScriptedAI
        {
            npc_muninn_97306AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Muninn[3] =
            {
                { 3402.268f, 3075.313f, 538.1175f },
                { 3362.619f, 3059.96f,  538.1175f },
                { 3288.12f,  2899.279f, 538.1175f }
            };

            enum ePoints
            {
                Point_MuninnMoveUpEnd  = 1,
                Point_MuninnFlyPathEnd = 2
            };

            enum eSpells
            {
                Spell_RunedScroll = 183614
            };

            enum eQuests
            {
                Quest_SignalBoost = 38060
            };

            enum eObjectives
            {
                Obj_RogersAndTinkmasterLinesHeard = 3806000
            };

            enum eNpcs
            {
                Npc_TinkmasterOverspark = 90866,
                Npc_SkyAdmiralRogers = 90749
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 25.0f && l_Player->GetQuestStatus(eQuests::Quest_SignalBoost) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_RogersAndTinkmasterLinesHeard) == 0)
                {
                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_TinkmasterOverspark, 50.0f, true))
                    {
                        l_Creature->AI()->Talk(0, l_Player->GetGUID());
                    }

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_SkyAdmiralRogers, 50.0f, true))
                        {
                            l_Creature->AI()->Talk(0, l_Player->GetGUID());
                        }
                    }, 6000);

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_TinkmasterOverspark, 50.0f, true))
                        {
                            l_Creature->AI()->Talk(1, l_Player->GetGUID());
                        }
                    }, 13000);

                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_RogersAndTinkmasterLinesHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        Position const l_FlyUpPos = { 3403.811f, 3075.332f, 536.686f, 0.0f };

                        l_Creature->SetSpeed(MOVE_RUN, 0.3f);
                        l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                        l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_MuninnMoveUpEnd, l_FlyUpPos);
                    }, 1000);
                }

                p_Player->CastSpell(p_Player, eSpells::Spell_RunedScroll, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_PointId) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;

                switch (p_PointId)
                {
                case ePoints::Point_MuninnMoveUpEnd:
                {
                    me->SetSpeed(MOVE_RUN, 1.0f);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_MuninnFlyPathEnd, Path_Muninn, 3);
                    break;
                }
                case ePoints::Point_MuninnFlyPathEnd:
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
            return new npc_muninn_97306AI(p_Creature);
        }
};

/// Commander Lorna Crowley - 91222
class npc_commander_lorna_crowley_91222 : public CreatureScript
{
    public:
        npc_commander_lorna_crowley_91222() : CreatureScript("npc_commander_lorna_crowley_91222") { }

        struct npc_commander_lorna_crowley_91222AI : public ScriptedAI
        {
            npc_commander_lorna_crowley_91222AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            enum eQuests
            {
                Quest_WillOfTheThorignir = 38473
            };

            enum eObjectives
            {
                Obj_CommanderLornaCrowleyLineHeard = 3847300
            };

            enum eSpells
            {
                Spell_Stealth = 132653
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && l_Player->HasQuest(eQuests::Quest_WillOfTheThorignir) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CommanderLornaCrowleyLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_CommanderLornaCrowleyLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_SummonerGuid = p_Player->GetGUID();

                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(1, l_SummonerGuid);
                        }

                        l_Creature->CastSpell(l_Creature, eSpells::Spell_Stealth, true);

                        G3D::Vector3 const Path_Lorna[10] =
                        {
                            { 2709.249f, 2864.299f, 309.5029f },
                            { 2712.999f, 2864.049f, 309.2529f },
                            { 2715.999f, 2863.549f, 308.7529f },
                            { 2716.749f, 2863.549f, 308.5029f },
                            { 2718.749f, 2863.549f, 308.0029f },
                            { 2719.749f, 2863.299f, 308.0029f },
                            { 2721.749f, 2863.049f, 307.7529f },
                            { 2723.749f, 2862.799f, 307.2529f },
                            { 2724.749f, 2862.799f, 307.0029f },
                            { 2725.585f, 2862.438f, 306.3574f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_Lorna, 10, true);
                    }, 2000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commander_lorna_crowley_91222AI(p_Creature);
        }
};

/// Commander Lorna Crowley - 91481
class npc_commander_lorna_crowley_91481 : public CreatureScript
{
    public:
        npc_commander_lorna_crowley_91481() : CreatureScript("npc_commander_lorna_crowley_91481") { }

        struct npc_commander_lorna_crowley_91481AI : public ScriptedAI
        {
            npc_commander_lorna_crowley_91481AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_JumpOnWallEnd   = 1,
                Point_JumpFromWallEnd = 2,
                Point_StealthMoveEnd  = 3
            };

            enum eQuests
            {
                Quest_AGrappleADay      = 38312,
                Quest_NoWingsRequired   = 38318,
                Quest_ToWeatherTheStorm = 38405
            };

            enum eObjectives
            {
                Obj_CommanderLornaCrowleyLineHeard = 3831200
            };

            enum eSpells
            {
                Spell_GrapplingHookVisual = 215221,
                Spell_GrapplingGun        = 215227,
                Spell_Stealth             = 132653
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && l_Player->HasQuest(eQuests::Quest_AGrappleADay) &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CommanderLornaCrowleyLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_CommanderLornaCrowleyLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                if (p_Player->HasQuest(eQuests::Quest_NoWingsRequired) && p_Player->HasQuest(eQuests::Quest_ToWeatherTheStorm))
                {
                    if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        uint64 l_PlayerGuid = p_Player->GetGUID();

                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        l_Creature->RemoveFlag(UNIT_FIELD_BYTES_1, 8);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->PlayOneShotAnimKitId(5179);
                        }, 500);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            {
                                l_Creature->AI()->Talk(1, l_PlayerGuid);
                            }
                        }, 3000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetFacingTo(4.032633f);
                            l_Creature->SetVirtualItem(4, 112340);
                            l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_GrapplingHookVisual, true);
                        }, 6000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SetFacingTo(1.625093f);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_GrapplingGun, true);
                            l_Creature->GetMotionMaster()->MoveJump(2668.163f, 2545.227f, 238.7137f, 20.0f, 15.0f, 0.0f, ePoints::Point_JumpOnWallEnd);
                        }, 9000);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_JumpOnWallEnd:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetFacingTo(1.343904f);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        }, 2000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetFacingTo(4.066617f);
                            me->GetMotionMaster()->MoveJump(2653.03f, 2525.05f, 221.965f, 10.0f, 10.0f, 0.0f, ePoints::Point_JumpFromWallEnd);
                        }, 5000);

                        break;
                    }
                    case ePoints::Point_JumpFromWallEnd:
                    {
                        G3D::Vector3 const Path_Lorna[10] =
                        {
                            { 2652.007f, 2525.466f, 222.2757f },
                            { 2650.007f, 2525.216f, 222.2757f },
                            { 2647.007f, 2525.216f, 222.5257f },
                            { 2646.007f, 2525.216f, 222.7757f },
                            { 2644.007f, 2525.216f, 222.5257f },
                            { 2643.007f, 2525.216f, 222.5257f },
                            { 2642.007f, 2525.216f, 222.2757f },
                            { 2638.984f, 2525.382f, 222.0864f },
                            { 2624.542f, 2531.536f, 222.0864f },
                            { 2612.946f, 2539.121f, 222.0864f }
                        };

                        DoCast(eSpells::Spell_Stealth);
                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_StealthMoveEnd, Path_Lorna, 10, true);
                        break;
                    }
                    case ePoints::Point_StealthMoveEnd:
                    {
                        me->DespawnOrUnsummon();
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commander_lorna_crowley_91481AI(p_Creature);
        }
};

/// Commander Lorna Crowley - 91519
class npc_commander_lorna_crowley_91519 : public CreatureScript
{
    public:
        npc_commander_lorna_crowley_91519() : CreatureScript("npc_commander_lorna_crowley_91519") { }

        struct npc_commander_lorna_crowley_91519AI : public ScriptedAI
        {
            npc_commander_lorna_crowley_91519AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ToWeatherTheStorm = 38405
            };

            enum eObjectives
            {
                Obj_CommanderLornaCrowleyLineHeard = 3840500
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 15.0f && l_Player->GetQuestStatus(eQuests::Quest_ToWeatherTheStorm) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CommanderLornaCrowleyLineHeard) == 0)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_CommanderLornaCrowleyLineHeard);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commander_lorna_crowley_91519AI(p_Creature);
        }
};

/// Commander Lorna Crowley - 91553
class npc_commander_lorna_crowley_91553 : public CreatureScript
{
    public:
        npc_commander_lorna_crowley_91553() : CreatureScript("npc_commander_lorna_crowley_91553") { }

        struct npc_commander_lorna_crowley_91553AI : public ScriptedAI
        {
            npc_commander_lorna_crowley_91553AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_ImpalementInsurance = 38410
            };

            enum eObjectives
            {
                Obj_LornaLineHeard   = 3861503,
                Obj_SkovaldSceneSeen = 3861502
            };

            enum eKillcredits
            {
                Killcredit_LornaLineHeard = 3861501
            };

            enum eSpells
            {
                Spell_SummonScout = 181686
            };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) < 10.0f && l_Player->GetQuestStatus(eQuests::Quest_ImpalementInsurance) == QUEST_STATUS_COMPLETE &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_LornaLineHeard) == 0 && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SkovaldSceneSeen) == 1)
                {
                    Talk(0, l_Player->GetGUID());
                    l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_LornaLineHeard);
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_SummonScout, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commander_lorna_crowley_91553AI(p_Creature);
        }
};

/// Commander Lorna Crowley - 91553
class npc_commander_lorna_crowley_91558 : public CreatureScript
{
    public:
        npc_commander_lorna_crowley_91558() : CreatureScript("npc_commander_lorna_crowley_91558") { }

        struct npc_commander_lorna_crowley_91558AI : public ScriptedAI
        {
            npc_commander_lorna_crowley_91558AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_AnotherWay              = 38342,
                Quest_AboveTheWinterMoonlight = 38412
            };

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            enum eSpells
            {
                Spell_Shoot = 178317
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(ePoints::Point_MoveEnd, 2341.517f, 2368.25f, 316.9446f);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (p_PointId == ePoints::Point_MoveEnd)
                {
                    me->SetFacingTo(3.560472f);
                    Talk(0, m_SummonerGuid);
                    me->DespawnOrUnsummon(120000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!UpdateVictim())
                    return;

                DoSpellAttackIfReady(eSpells::Spell_Shoot);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commander_lorna_crowley_91558AI(p_Creature);
        }
};

/// Credit Bunny - 94340, 94341, 94342
class npc_combustible_contagion_bunnys : public CreatureScript
{
    public:
        npc_combustible_contagion_bunnys() : CreatureScript("npc_combustible_contagion_bunnys") { }

        struct npc_combustible_contagion_bunnysAI : public ScriptedAI
        {
            npc_combustible_contagion_bunnysAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_BurningPlagueCacheDest   = 186745,
                Spell_BurningPlagueCacheNearby = 186746,
                Spell_BurningPlagueCacheAura   = 186747
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player || me->isSummon())
                    return;

                if (p_Spell->Id == eSpells::Spell_BurningPlagueCacheDest)
                {
                    l_Player->CastSpell(l_Player, eSpells::Spell_BurningPlagueCacheNearby);
                }
                else if (p_Spell->Id == eSpells::Spell_BurningPlagueCacheNearby)
                {
                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000, 0, l_Player->GetGUID()))
                    {
                        l_Player->KilledMonsterCredit(me->GetEntry());
                        l_Player->CastSpell(l_Player, eSpells::Spell_BurningPlagueCacheAura, true);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_combustible_contagion_bunnysAI(p_Creature);
        }
};

/// Genn Greymane - 95804
class npc_genn_greymane_95804 : public CreatureScript
{
    public:
        npc_genn_greymane_95804() : CreatureScript("npc_genn_greymane_95804") { }

        struct npc_genn_greymane_95804AI : public ScriptedAI
        {
            npc_genn_greymane_95804AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_GennPathEnd        = 1,
                Point_FirstGuardPathEnd  = 2,
                Point_SecondGuardPathEnd = 3,
                Point_ThirdGuardPathEnd  = 4,
                Point_FourthGuardPathEnd = 5
            };

            enum eSpells
            {
                Spell_NarrowEscape = 229076
            };

            enum eQuests
            {
                Quest_EndingTheNewBeginning = 39122
            };

            enum eNpcs
            {
                Npc_GreywatchGuard = 95859
            };

            ObjectGuid m_SummonerGuid;
            Position const m_FirstGuardSpawnPos  = { 2539.239f, 1420.646f, 32.67332f, 2.359324f  };
            Position const m_SecondGuardSpawnPos = { 2538.59f,  1411.938f, 32.67334f, 4.012599f  };
            Position const m_ThirdGuardSpawnPos  = { 2548.767f, 1414.106f, 32.67334f, 5.565257f  };
            Position const m_FourthGuardSpawnPos = { 2546.031f, 1422.429f, 32.67332f, 0.6451124f };

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    uint64 l_SummonerGuid = p_Player->GetGUID();

                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, 2);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(0, l_SummonerGuid);
                        }

                        G3D::Vector3 const Path_Genn[9] =
                        {
                            { 2543.357f, 1402.351f, 32.59066f },
                            { 2542.911f, 1353.533f, 32.50801f },
                            { 2543.056f, 1345.984f, 35.73095f },
                            { 2543.2f,   1333.934f, 39.45388f },
                            { 2542.852f, 1326.911f, 41.91837f },
                            { 2542.934f, 1320.883f, 42.10944f },
                            { 2542.695f, 1314.024f, 41.89891f },
                            { 2542.376f, 1307.543f, 41.49371f },
                            { 2542.346f, 1300.91f,  41.41578f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_GennPathEnd, Path_Genn, 9, false);
                    }, 2000);
                }

                if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_GreywatchGuard, m_FirstGuardSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Guard[12] =
                        {
                            { 2539.765f, 1413.203f, 32.93611f },
                            { 2539.765f, 1401.703f, 32.93611f },
                            { 2540.265f, 1389.953f, 32.93611f },
                            { 2540.254f, 1380.247f, 32.94891f },
                            { 2540.254f, 1374.247f, 32.94891f },
                            { 2540.004f, 1355.997f, 32.94891f },
                            { 2540.106f, 1345.18f,  35.83028f },
                            { 2540.208f, 1338.13f,  37.96166f },
                            { 2540.25f,  1327.296f, 42.07598f },
                            { 2540.426f, 1321.156f, 41.8989f  },
                            { 2540.241f, 1315.951f, 41.94872f },
                            { 2538.797f, 1304.354f, 41.46073f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FirstGuardPathEnd, Path_Guard, 12, false);
                    }, 2000);

                    if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_GreywatchGuard, m_SecondGuardSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            G3D::Vector3 const Path_Guard[12] =
                            {
                                { 2537.828f, 1401.97f,  32.69891f },
                                { 2537.528f, 1398.05f,  32.69891f },
                                { 2538.082f, 1380.189f, 32.94891f },
                                { 2538.082f, 1374.439f, 32.94891f },
                                { 2537.332f, 1366.439f, 32.94891f },
                                { 2536.832f, 1359.439f, 32.94891f },
                                { 2537.188f, 1344.146f, 36.08785f },
                                { 2537.664f, 1333.21f,  39.90997f },
                                { 2537.914f, 1327.21f,  42.15997f },
                                { 2538.364f, 1316.044f, 42.18852f },
                                { 2538.364f, 1311.294f, 42.18852f },
                                { 2533.962f, 1301.691f, 41.43552f }
                            };

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_SecondGuardPathEnd, Path_Guard, 12, false);
                        }, 2000);
                    }

                    if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_GreywatchGuard, m_ThirdGuardSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            G3D::Vector3 const Path_Guard[13] =
                            {
                                { 2548.654f, 1400.625f, 32.93612f },
                                { 2548.654f, 1393.375f, 32.93612f },
                                { 2548.654f, 1391.875f, 32.93612f },
                                { 2546.83f,  1380.213f, 32.95135f },
                                { 2546.83f,  1374.463f, 32.95135f },
                                { 2547.33f,  1366.213f, 32.95135f },
                                { 2548.048f, 1353.312f, 32.69891f },
                                { 2548.038f, 1347.766f, 35.14227f },
                                { 2548.044f, 1337.913f, 38.08564f },
                                { 2547.391f, 1327.17f,  42.17942f },
                                { 2546.794f, 1315.96f,  42.05495f },
                                { 2546.98f,  1310.059f, 41.65545f },
                                { 2551.226f, 1301.54f,  41.41201f }
                            };

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_ThirdGuardPathEnd, Path_Guard, 13, false);
                        }, 2000);
                    }

                    if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_GreywatchGuard, m_FourthGuardSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            G3D::Vector3 const Path_Guard[13] =
                            {
                                { 2546.022f, 1417.36f,  32.93611f },
                                { 2546.022f, 1400.11f,  33.18611f },
                                { 2546.022f, 1392.86f,  32.93611f },
                                { 2546.013f, 1386.792f, 32.69891f },
                                { 2545.986f, 1380.31f,  32.74588f },
                                { 2545.736f, 1374.06f,  32.74588f },
                                { 2545.736f, 1366.31f,  32.74588f },
                                { 2545.457f, 1353.134f, 32.79286f },
                                { 2545.665f, 1346.901f, 35.37718f },
                                { 2545.707f, 1333.342f, 39.92776f },
                                { 2545.457f, 1327.092f, 42.17776f },
                                { 2544.997f, 1315.858f, 42.16446f },
                                { 2545.508f, 1304.366f, 41.45242f }
                            };

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FourthGuardPathEnd, Path_Guard, 13, false);
                        }, 2000);
                    }
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_GennPathEnd)
                {
                    me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_NarrowEscape && p_Caster->GetGUID() == m_SummonerGuid)
                {
                    me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isSummon())
                    return;

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_EndingTheNewBeginning))
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_genn_greymane_95804AI(p_Creature);
        }
};

/// Greywatch Guard - 95859
class npc_greywatch_guard_95859 : public CreatureScript
{
    public:
        npc_greywatch_guard_95859() : CreatureScript("npc_greywatch_guard_95859") { }

        struct npc_greywatch_guard_95859AI : public ScriptedAI
        {
            npc_greywatch_guard_95859AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_FirstGuardPathEnd  = 2,
                Point_SecondGuardPathEnd = 3,
                Point_ThirdGuardPathEnd  = 4,
                Point_FourthGuardPathEnd = 5
            };

            enum eSpells
            {
                Spell_NarrowEscape = 229076
            };

            enum eQuests
            {
                Quest_EndingTheNewBeginning = 39122
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_NarrowEscape && p_Caster->GetGUID() == m_SummonerGuid)
                {
                    me->DespawnOrUnsummon();
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_FirstGuardPathEnd:
                    {
                        me->SetFacingTo(4.677482f);
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        break;
                    }
                    case ePoints::Point_SecondGuardPathEnd:
                    {
                        me->SetFacingTo(4.590216f);
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        break;
                    }
                    case ePoints::Point_ThirdGuardPathEnd:
                    {
                        me->SetFacingTo(4.572762f);
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        break;
                    }
                    case ePoints::Point_FourthGuardPathEnd:
                    {
                        me->SetFacingTo(4.607669f);
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isSummon())
                    return;

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld() || !l_Player->HasQuest(eQuests::Quest_EndingTheNewBeginning))
                {
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_greywatch_guard_95859AI(p_Creature);
        }
};

/// Blood-Thane Lucard - 107588
class npc_blood_thane_lucard_107588 : public CreatureScript
{
    public:
        npc_blood_thane_lucard_107588() : CreatureScript("npc_blood_thane_lucard_107588") { }

        struct npc_blood_thane_lucard_107588AI : public ScriptedAI
        {
            npc_blood_thane_lucard_107588AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Staking          = 214669,
                Spell_Knockback        = 214671,
                Spell_RedFog           = 214638,
                Spell_LingeringHunger  = 215041,
                Spell_BloodRitual      = 215075,
                Spell_ConsumeTheLiving = 214999
            };

            enum ePoints
            {
                Point_EngageJumpEnd = 1,
                Point_RightJumpEnd  = 2,
                Point_LeftJumpEnd   = 3
            };

            enum eEvents
            {
                Event_CastLingeringHunger  = 1,
                Event_CastBloodRitual      = 2,
                Event_CastConsumeTheLiving = 3
            };

            enum eNpcs
            {
                Npc_Bunny          = 34527,
                Npc_DrainedCorsair = 108150
            };

            std::vector<ObjectGuid> m_BunnyGuids;
            EventMap m_Events;
            Position const m_EngageJumpPos = { 3180.25f, 838.158f, 12.176f,   3.992574f };
            Position const m_RightJumpPos  = { 3195.48f, 820.899f, 13.31563f, 0.0f      };
            Position const m_LeftJumpPos   = { 3165.08f, 854.597f, 13.28176f, 0.0f      };
            bool m_RightPosUsed            = false;
            bool m_LeftPosUsed             = false;
            bool m_CanCastConsumeTheLiving = false;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                for (auto itr : m_BunnyGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        if (!l_Creature->HasAura(eSpells::Spell_RedFog))
                        {
                            l_Creature->AddAura(eSpells::Spell_RedFog, l_Creature);
                        }
                    }
                }

                std::list<Creature*> l_CreatureList;
                me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_DrainedCorsair, 50.0f);

                if (l_CreatureList.empty())
                    return;

                for (auto itr : l_CreatureList)
                {
                    if (itr->isDead())
                    {
                        itr->Respawn();
                    }
                }

                m_BunnyGuids.clear();
                m_RightPosUsed = false;
                m_LeftPosUsed = false;
                m_CanCastConsumeTheLiving = false;
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastLingeringHunger, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastBloodRitual, 15000);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_Staking)
                {
                    me->SetAIAnimKitId(0);
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    DoCastAOE(eSpells::Spell_Knockback, false);

                    me->AddDelayedEvent([this]() -> void
                    {
                        std::list<Creature*> l_CreatureList;
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_Bunny, 50.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            if (itr->HasAura(eSpells::Spell_RedFog))
                            {
                                m_BunnyGuids.push_back(itr->GetGUID());
                                itr->RemoveAura(eSpells::Spell_RedFog);
                            }
                        }

                    }, 3000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        Talk(0);
                    }, 4000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetFlag(UNIT_FIELD_BYTES_2, 1);
                        me->GetMotionMaster()->MoveJump(m_EngageJumpPos, 5.0f, 15.0f, ePoints::Point_EngageJumpEnd);
                    }, 8000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Point_EngageJumpEnd:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            Talk(1);
                        }, 1000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                            if (Player* l_Player = me->FindNearestPlayer(50.0f))
                            {
                                AttackStart(l_Player, false);
                            }

                        }, 2000);

                        break;
                    }
                    case ePoints::Point_RightJumpEnd:
                    {
                        me->SetFacingTo(2.365297f);
                        DoCast(eSpells::Spell_BloodRitual);
                        break;
                    }
                    case ePoints::Point_LeftJumpEnd:
                    {
                        me->SetFacingTo(5.423188f);
                        DoCast(eSpells::Spell_BloodRitual);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(4);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!m_CanCastConsumeTheLiving && me->GetHealthPct() <= 80)
                {
                    m_CanCastConsumeTheLiving = true;
                    m_Events.ScheduleEvent(eEvents::Event_CastConsumeTheLiving, 1000);
                }

                if (m_CanCastConsumeTheLiving && me->IsFullHealth())
                {
                    m_CanCastConsumeTheLiving = false;
                    m_Events.CancelEvent(Event_CastConsumeTheLiving);
                }

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::Event_CastLingeringHunger:
                    {
                        DoCastVictim(eSpells::Spell_LingeringHunger);
                        m_Events.ScheduleEvent(eEvents::Event_CastLingeringHunger, urand(28000, 39000));
                        break;
                    }
                    case eEvents::Event_CastBloodRitual:
                    {
                        if (!m_LeftPosUsed)
                        {
                            me->GetMotionMaster()->MoveJump(m_LeftJumpPos, 20.0f, 10.0f, ePoints::Point_LeftJumpEnd);
                            m_LeftPosUsed = true;
                            m_RightPosUsed = false;
                        }
                        else if (!m_RightPosUsed)
                        {
                            me->GetMotionMaster()->MoveJump(m_RightJumpPos, 20.0f, 10.0f, ePoints::Point_RightJumpEnd);
                            m_RightPosUsed = true;
                            m_LeftPosUsed = false;
                        }

                        m_Events.ScheduleEvent(eEvents::Event_CastBloodRitual, urand(30000, 40000));
                        break;
                    }
                    case eEvents::Event_CastConsumeTheLiving:
                    {
                        DoCastAOE(eSpells::Spell_ConsumeTheLiving);
                        m_Events.ScheduleEvent(eEvents::Event_CastConsumeTheLiving, urand(25000, 30000));
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
            return new npc_blood_thane_lucard_107588AI(p_Creature);
        }
};

/// Restless Ancestor - 93094
class npc_restless_ancestor_93094 : public CreatureScript
{
    public:
        npc_restless_ancestor_93094() : CreatureScript("npc_restless_ancestor_93094") { }

        struct npc_restless_ancestor_93094AI : public ScriptedAI
        {
            npc_restless_ancestor_93094AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Wail = 198442,
                Spell_FeastOnTheLiving = 198427,
                Spell_WeakenedSpirit = 191995,
                Spell_ReturningSpirit = 191996,
                Spell_ReturningSpiritSpellHit = 191993
            };

            enum eKillcredits
            {
                Killcredit_RestlessAncestorReturnedToTheEarth = 97254
            };

            enum eEvents
            {
                Event_CastWail = 1,
                Event_CastFeastOnTheLiving = 2
            };

            EventMap m_Events;
            bool m_Weakened = false;
            bool m_Returned = false;

            void Reset() override
            {
                m_Events.Reset();
                me->SetDisableGravity(true);
                me->SetPlayerHoverAnim(true);
                me->SendPlayHoverAnim(true);
                me->SetFall(false);
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                m_Weakened = false;
                m_Returned = false;
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                me->SetDisableGravity(false);
                me->SetPlayerHoverAnim(false);
                me->SendPlayHoverAnim(false);
                me->SetFall(true);
                m_Events.ScheduleEvent(eEvents::Event_CastWail, urand(4000, 10000));
                m_Events.ScheduleEvent(eEvents::Event_CastFeastOnTheLiving, urand(3000, 13000));
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_ReturningSpiritSpellHit)
                    {
                        m_Events.Reset();
                        me->AttackStop();
                        me->SetUInt32Value(UNIT_FIELD_FLAGS, 34113536);
                        me->SetFacingTo(me->GetOrientation());
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_RestlessAncestorReturnedToTheEarth);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->CastSpell(me, eSpells::Spell_ReturningSpirit, true);
                            me->DespawnOrUnsummon(2000);
                        }, 3000);

                        m_Returned = true;
                    }
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (damage >= me->GetHealth() && m_Returned)
                {
                    damage = 0;
                    me->SetHealth(1);
                }

                if (me->GetHealthPct() < 40 && !m_Weakened)
                {
                    DoCast(eSpells::Spell_WeakenedSpirit, true);
                    m_Weakened = true;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastWail:
                {
                    DoCast(eSpells::Spell_Wail);
                    m_Events.ScheduleEvent(eEvents::Event_CastWail, urand(7000, 14000));
                    break;
                }
                case eEvents::Event_CastFeastOnTheLiving:
                {
                    DoCastVictim(eSpells::Spell_FeastOnTheLiving);
                    m_Events.ScheduleEvent(eEvents::Event_CastFeastOnTheLiving, 22000);
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
            return new npc_restless_ancestor_93094AI(p_Creature);
        }
};

/// Unpowered Console - 243808
class gob_unpowered_console_243808 : public GameObjectScript
{
    public:
        gob_unpowered_console_243808() : GameObjectScript("gob_unpowered_console_243808") { }

        struct gob_unpowered_console_243808AI : public GameObjectAI
        {
            gob_unpowered_console_243808AI(GameObject* go) : GameObjectAI(go) { }

            enum eNpcs
            {
                Npc_BunnyVisual      = 34527,
                Npc_TitanConsole     = 96122,
                Npc_PortalBunny      = 35845,
                Npc_FelskornTorturer = 96121
            };

            enum eSpells
            {
                Spell_Powering     = 190746,
                Spell_ShadowPortal = 190664,
                Spell_Powered      = 190678
            };

            enum eActions
            {
                Action_ConsoleActivated       = 1,
                Action_FelskornTorturerEvaded = 2
            };

            enum eGobs
            {
                Gob_PoweredConsole = 243801
            };

            Position const m_FelskornTorturerSpawnPos = { 3442.369f, 2082.66f, 279.4027f, 1.514689f };

            void OnLootStateChanged(uint32 p_State, Unit* /*p_Unit*/) override
            {
                if (p_State == 2)
                {
                    go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);

                    if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                    {
                        l_Creature->AI()->Talk(0);
                    }

                    std::list<Creature*> l_CreatureList;
                    go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                    if (l_CreatureList.empty())
                        return;

                    for (auto itr : l_CreatureList)
                    {
                        itr->CastSpell(itr, eSpells::Spell_Powering, true);
                    }

                    go->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_PortalBunny, 25.0f, true))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowPortal, true);
                        }
                    }, 7000);

                    go->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornTorturer, m_FelskornTorturerSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                        {
                            l_Creature->AI()->SetGUID(go->GetGUID());
                        }
                    }, 9000);

                    go->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_PortalBunny, 25.0f, true))
                        {
                            l_Creature->RemoveAura(eSpells::Spell_ShadowPortal);
                        }
                    }, 18000);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_ConsoleActivated:
                    {
                        go->SetGoState(GOState::GO_STATE_READY);

                        if (GameObject* l_Gob = go->FindNearestGameObject(eGobs::Gob_PoweredConsole, 5.0f))
                        {
                            l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);
                        }

                        if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                        {
                            l_Creature->AI()->Talk(1);
                        }

                        std::list<Creature*> l_CreatureList;
                        go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            itr->RemoveAura(eSpells::Spell_Powering);
                            itr->CastSpell(itr, eSpells::Spell_Powered, true);
                        }

                        go->AddDelayedEvent([this]() -> void
                        {
                            go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);

                            if (GameObject* l_Gob = go->FindNearestGameObject(eGobs::Gob_PoweredConsole, 5.0f))
                            {
                                l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                            }

                            std::list<Creature*> l_CreatureList;
                            go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                            if (l_CreatureList.empty())
                                return;

                            for (auto itr : l_CreatureList)
                            {
                                itr->RemoveAura(eSpells::Spell_Powered);
                            }
                        }, 60000);

                        break;
                    }
                    case eActions::Action_FelskornTorturerEvaded:
                    {
                        go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);
                        go->SetGoState(GOState::GO_STATE_READY);

                        if (GameObject* l_Gob = go->FindNearestGameObject(eGobs::Gob_PoweredConsole, 5.0f))
                        {
                            l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                        }

                        std::list<Creature*> l_CreatureList;
                        go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            itr->RemoveAllAuras();
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_unpowered_console_243808AI(go);
        }
};

/// Unpowered Console - 243814
class gob_unpowered_console_243814 : public GameObjectScript
{
    public:
        gob_unpowered_console_243814() : GameObjectScript("gob_unpowered_console_243814") { }

        struct gob_unpowered_console_243814AI : public GameObjectAI
        {
            gob_unpowered_console_243814AI(GameObject* go) : GameObjectAI(go) { }

            enum eNpcs
            {
                Npc_BunnyVisual       = 34527,
                Npc_TitanConsole      = 96139,
                Npc_FelskornRaider    = 96129,
                Npc_FelskornWarmonger = 108263
            };

            enum eSpells
            {
                Spell_Powering = 190746,
                Spell_Powered  = 190678
            };

            enum eActions
            {
                Action_RaiderDied    = 1,
                Action_WarmongerDied = 2,
                Action_SummonsEvaded = 3
            };

            enum eGobs
            {
                Gob_PoweredConsole = 243802
            };

            enum ePoints
            {
                Point_FelskornRaiderMoveToConsoleEnd    = 1,
                Point_FelskornWarmongerMoveToConsoleEnd = 2
            };

            std::vector<ObjectGuid> m_SummonsGuids;
            Position const m_FirstWaveFirstFelskornRaiderSpawnPos   = { 3337.821f, 2161.978f, 299.3525f, 2.444728f };
            Position const m_FirstWaveSecondFelskornRaiderSpawnPos  = { 3333.63f,  2158.97f,  299.6485f, 1.684438f };
            Position const m_SecondWaveFirstFelskornRaiderSpawnPos  = { 3339.048f, 2163.434f, 299.7288f, 2.446229f };
            Position const m_SecondWaveSecondFelskornRaiderSpawnPos = { 3335.4f,   2160.611f, 299.4125f, 2.384349f };
            Position const m_SecondWaveThirdFelskornRaiderSpawnPos  = { 3332.411f, 2157.981f, 299.6475f, 1.706203f };
            Position const m_FelskornWarmongerSpawnPos              = { 3336.681f, 2160.384f, 299.0743f, 1.989021f };
            uint8 m_RaidersDies      = 0;
            bool m_FirstWaveComplete = false;

            void OnLootStateChanged(uint32 p_State, Unit* /*p_Unit*/) override
            {
                if (p_State == 2)
                {
                    go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);

                    if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                    {
                        l_Creature->AI()->Talk(0);
                    }

                    go->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornRaider, m_FirstWaveFirstFelskornRaiderSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                        {
                            G3D::Vector3 const Path_FelskornRaider[14] =
                            {
                                { 3335.521f, 2163.902f, 300.6739f },
                                { 3333.222f, 2165.827f, 301.6193f },
                                { 3331.688f, 2167.11f,  302.2812f },
                                { 3330.155f, 2168.394f, 303.1562f },
                                { 3328.622f, 2169.677f, 303.7812f },
                                { 3327.245f, 2170.829f, 304.8896f },
                                { 3329.805f, 2175.339f, 305.5795f },
                                { 3329.805f, 2179.339f, 307.3295f },
                                { 3330.104f, 2183.756f, 308.8369f },
                                { 3330.354f, 2185.756f, 309.3369f },
                                { 3330.715f, 2192.764f, 311.7721f },
                                { 3330.965f, 2194.764f, 312.5221f },
                                { 3331.531f, 2203.438f, 314.7182f },
                                { 3331.779f, 2209.169f, 314.7598f }
                            };

                            m_SummonsGuids.push_back(l_Creature->GetGUID());
                            l_Creature->AI()->SetGUID(go->GetGUID());
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FelskornRaiderMoveToConsoleEnd, Path_FelskornRaider, 14, false);
                        }

                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornRaider, m_FirstWaveSecondFelskornRaiderSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                        {
                            G3D::Vector3 const Path_FelskornRaider[18] =
                            {
                                { 3333.29f,  2161.95f,  300.7443f },
                                { 3332.95f,  2164.93f,  301.3693f },
                                { 3332.724f, 2166.916f, 301.9062f },
                                { 3332.497f, 2168.902f, 302.5312f },
                                { 3332.271f, 2170.889f, 303.5312f },
                                { 3331.974f, 2173.5f,   304.2867f },
                                { 3332.222f, 2173.691f, 304.6119f },
                                { 3332.972f, 2175.691f, 305.6119f },
                                { 3333.472f, 2177.191f, 306.3619f },
                                { 3334.472f, 2181.191f, 307.8619f },
                                { 3335.222f, 2182.941f, 308.8619f },
                                { 3335.392f, 2183.735f, 309.1373f },
                                { 3335.642f, 2187.485f, 309.8873f },
                                { 3335.642f, 2190.485f, 311.1373f },
                                { 3335.969f, 2190.58f,  311.3935f },
                                { 3335.969f, 2194.33f,  312.8935f },
                                { 3336.219f, 2200.33f,  313.6435f },
                                { 3336.565f, 2203.679f, 314.6564f }
                            };

                            m_SummonsGuids.push_back(l_Creature->GetGUID());
                            l_Creature->AI()->SetGUID(go->GetGUID());
                            l_Creature->AI()->Talk(0);
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FelskornRaiderMoveToConsoleEnd, Path_FelskornRaider, 18, false);
                        }
                    }, 1500);

                    go->AddDelayedEvent([this]() -> void
                    {
                        std::list<Creature*> l_CreatureList;
                        go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            itr->CastSpell(itr, eSpells::Spell_Powering, true);
                        }
                    }, 2000);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_RaiderDied:
                    {
                        ++m_RaidersDies;

                        if (m_RaidersDies == 2 && !m_FirstWaveComplete)
                        {
                            if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                            {
                                l_Creature->AI()->Talk(1);
                            }

                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornRaider, m_SecondWaveFirstFelskornRaiderSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    G3D::Vector3 const Path_FelskornRaider[12] =
                                    {
                                        { 3336.745f, 2165.355f, 301.0489f },
                                        { 3334.442f, 2167.277f, 301.7277f },
                                        { 3334.567f, 2170.885f, 303.2716f },
                                        { 3334.567f, 2174.885f, 304.7716f },
                                        { 3335.067f, 2175.885f, 305.5216f },
                                        { 3335.817f, 2177.885f, 306.2716f },
                                        { 3335.559f, 2181.493f, 308.4469f },
                                        { 3336.059f, 2183.493f, 309.1969f },
                                        { 3336.559f, 2187.243f, 309.9469f },
                                        { 3336.946f, 2192.127f, 312.5126f },
                                        { 3337.727f, 2203.223f, 314.6772f },
                                        { 3338.233f, 2208.247f, 314.7559f }
                                    };

                                    m_SummonsGuids.push_back(l_Creature->GetGUID());
                                    l_Creature->AI()->SetGUID(go->GetGUID());
                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FelskornRaiderMoveToConsoleEnd, Path_FelskornRaider, 12, false);
                                }

                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornRaider, m_SecondWaveSecondFelskornRaiderSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    G3D::Vector3 const Path_FelskornRaider[12] =
                                    {
                                        { 3333.947f, 2161.984f, 300.2989f },
                                        { 3331.041f, 2164.73f,  301.8693f },
                                        { 3329.588f, 2166.104f, 302.6193f },
                                        { 3327.408f, 2168.163f, 303.9062f },
                                        { 3325.769f, 2169.71f,  305.1f    },
                                        { 3326.851f, 2176.066f, 306.606f  },
                                        { 3326.677f, 2184.194f, 308.9748f },
                                        { 3326.677f, 2186.194f, 309.7248f },
                                        { 3327.029f, 2191.545f, 311.8628f },
                                        { 3327.279f, 2195.295f, 312.8628f },
                                        { 3327.859f, 2203.915f, 314.5652f },
                                        { 3328.295f, 2209.357f, 314.7598f }
                                    };

                                    m_SummonsGuids.push_back(l_Creature->GetGUID());
                                    l_Creature->AI()->SetGUID(go->GetGUID());
                                    l_Creature->AI()->Talk(1);
                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FelskornRaiderMoveToConsoleEnd, Path_FelskornRaider, 12, false);
                                }

                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornRaider, m_SecondWaveThirdFelskornRaiderSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    G3D::Vector3 const Path_FelskornRaider[16] =
                                    {
                                        { 3332.141f, 2159.963f, 300.4943f },
                                        { 3331.736f, 2162.937f, 301.1193f },
                                        { 3331.332f, 2165.91f,  301.8693f },
                                        { 3331.197f, 2166.901f, 302.5312f },
                                        { 3330.793f, 2169.875f, 303.4062f },
                                        { 3330.254f, 2173.84f,  304.2812f },
                                        { 3330.84f,  2175.845f, 305.7099f },
                                        { 3331.09f,  2179.845f, 307.2099f },
                                        { 3331.34f,  2181.845f, 307.9599f },
                                        { 3331.542f, 2184.355f, 308.9511f },
                                        { 3331.792f, 2189.105f, 310.2011f },
                                        { 3332.042f, 2191.105f, 311.2011f },
                                        { 3332.133f, 2191.468f, 311.5611f },
                                        { 3332.133f, 2195.218f, 312.8111f },
                                        { 3332.383f, 2200.218f, 313.5611f },
                                        { 3332.689f, 2204.108f, 314.8763f }
                                    };

                                    m_SummonsGuids.push_back(l_Creature->GetGUID());
                                    l_Creature->AI()->SetGUID(go->GetGUID());
                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FelskornRaiderMoveToConsoleEnd, Path_FelskornRaider, 16, false);
                                }
                            }, 5000);

                            m_FirstWaveComplete = true;
                            m_RaidersDies = 0;
                        }
                        else if (m_RaidersDies == 3)
                        {
                            if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                            {
                                l_Creature->AI()->Talk(2);
                            }

                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_FelskornWarmonger, m_FelskornWarmongerSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    G3D::Vector3 const Path_FelskornWarmonger[20] =
                                    {
                                        { 3336.274f, 2161.298f, 299.4239f },
                                        { 3335.462f, 2163.126f, 300.1739f },
                                        { 3335.056f, 2164.04f, 300.9239f  },
                                        { 3333.837f, 2166.782f, 301.7277f },
                                        { 3333.024f, 2168.61f, 302.2812f  },
                                        { 3332.212f, 2170.438f, 303.1562f },
                                        { 3331.399f, 2172.267f, 303.7812f },
                                        { 3330.587f, 2174.095f, 304.7812f },
                                        { 3330.435f, 2174.968f, 305.3372f },
                                        { 3330.685f, 2175.968f, 305.5872f },
                                        { 3330.935f, 2177.718f, 306.5872f },
                                        { 3331.435f, 2179.718f, 307.3372f },
                                        { 3331.685f, 2181.718f, 308.0872f },
                                        { 3331.935f, 2183.718f, 308.8372f },
                                        { 3332.025f, 2187.358f, 309.7003f },
                                        { 3332.275f, 2191.358f, 311.2003f },
                                        { 3332.533f, 2193.493f, 312.1722f },
                                        { 3332.533f, 2195.243f, 312.9222f },
                                        { 3332.533f, 2202.243f, 314.4222f },
                                        { 3332.699f, 2204.029f, 314.9158f }
                                    };

                                    m_SummonsGuids.push_back(l_Creature->GetGUID());
                                    l_Creature->AI()->SetGUID(go->GetGUID());
                                    l_Creature->AI()->Talk(0);
                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FelskornWarmongerMoveToConsoleEnd, Path_FelskornWarmonger, 20, false);
                                }
                            }, 4000);
                        }

                        break;
                    }
                    case eActions::Action_WarmongerDied:
                    {
                        if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                        {
                            l_Creature->AI()->Talk(3);
                        }

                        go->SetGoState(GOState::GO_STATE_READY);

                        if (GameObject* l_Gob = go->FindNearestGameObject(eGobs::Gob_PoweredConsole, 5.0f))
                        {
                            l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);
                        }

                        std::list<Creature*> l_CreatureList;
                        go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            itr->RemoveAura(eSpells::Spell_Powering);
                            itr->CastSpell(itr, eSpells::Spell_Powered, true);
                        }

                        go->AddDelayedEvent([this]() -> void
                        {
                            go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);

                            if (GameObject* l_Gob = go->FindNearestGameObject(eGobs::Gob_PoweredConsole, 5.0f))
                            {
                                l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                            }

                            std::list<Creature*> l_CreatureList;
                            go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                            if (l_CreatureList.empty())
                                return;

                            for (auto itr : l_CreatureList)
                            {
                                itr->RemoveAura(eSpells::Spell_Powered);
                            }

                            m_RaidersDies = 0;
                            m_FirstWaveComplete = false;
                            m_SummonsGuids.clear();
                        }, 60000);

                        break;
                    }
                    case eActions::Action_SummonsEvaded:
                    {
                        go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);
                        go->SetGoState(GOState::GO_STATE_READY);

                        if (GameObject* l_Gob = go->FindNearestGameObject(eGobs::Gob_PoweredConsole, 5.0f))
                        {
                            l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                        }

                        std::list<Creature*> l_CreatureList;
                        go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 20.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            itr->RemoveAllAuras();
                        }

                        for (auto itr : m_SummonsGuids)
                        {
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*go, itr))
                            {
                                l_Creature->DespawnOrUnsummon();
                            }
                        }

                        m_RaidersDies = 0;
                        m_FirstWaveComplete = false;
                        m_SummonsGuids.clear();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_unpowered_console_243814AI(go);
        }
};

/// Powered Console - 243817
class gob_powered_console_243817 : public GameObjectScript
{
    public:
        gob_powered_console_243817() : GameObjectScript("gob_powered_console_243817") { }

        struct gob_powered_console_243817AI : public GameObjectAI
        {
            gob_powered_console_243817AI(GameObject* go) : GameObjectAI(go) { }

            enum eNpcs
            {
                Npc_BunnyVisual  = 35845,
                Npc_TitanConsole = 96176,
                Npc_Yotnar       = 96175
            };

            enum eActions
            {
                Action_ConsoleReady = 1
            };

            enum eSpells
            {
                Spell_WatchersPoweredVisual = 190763,
                Spell_Powered               = 190765
            };

            Position const m_YotnarSpawnPos = { 3502.967f, 2164.512f, 231.977f, 6.230515f };

            void OnLootStateChanged(uint32 p_State, Unit* /*p_Unit*/) override
            {
                if (p_State == 2)
                {
                    go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);

                    if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_TitanConsole, 5.0f, true))
                    {
                        l_Creature->AI()->Talk(0);
                    }

                    std::list<Creature*> l_CreatureList;
                    go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 80.0f);

                    if (l_CreatureList.empty())
                        return;

                    for (auto itr : l_CreatureList)
                    {
                        if (int32(itr->GetPosition().m_positionZ) == 234 || int32(itr->GetPosition().m_positionZ) == 231)
                        {
                            itr->CastSpell(itr, eSpells::Spell_Powered, true);
                        }
                        else
                        {
                            itr->CastSpell(itr, eSpells::Spell_WatchersPoweredVisual, true);
                        }
                    }

                    go->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_Yotnar, m_YotnarSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            l_Creature->AI()->SetGUID(go->GetGUID());
                        }
                    }, 7000);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::Action_ConsoleReady:
                    {
                        go->SetGoState(GOState::GO_STATE_READY);
                        go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);

                        std::list<Creature*> l_CreatureList;
                        go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_BunnyVisual, 80.0f);

                        if (l_CreatureList.empty())
                            return;

                        for (auto itr : l_CreatureList)
                        {
                            itr->RemoveAllAuras();
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_powered_console_243817AI(go);
        }
};

/// Ritual Circle - 240650, 244337, 244336, 244335
class gob_stormheim_ritual_circles : public GameObjectScript
{
    public:
        gob_stormheim_ritual_circles() : GameObjectScript("gob_stormheim_ritual_circles") { }

        struct gob_stormheim_ritual_circlesAI : public GameObjectAI
        {
            gob_stormheim_ritual_circlesAI(GameObject* go) : GameObjectAI(go) { }

            enum eNpcs
            {
                Npc_Hrafsir   = 97031,
                Npc_Erilar    = 97032,
                Npc_Hridmogir = 97033,
                Npc_Aleifir   = 97028
            };

            enum eGobs
            {
                Gob_HrafsirRitualCircle   = 244335,
                Gob_ErilarRitualCircle    = 244336,
                Gob_HridmogirRitualCircle = 244337,
                Gob_AleifirRitualCircle   = 240650
            };

            enum ePoints
            {
                Point_FlyUpEnd      = 1,
                Point_FlyPathEnd    = 2,
                Point_GroundPathEnd = 3
            };

            Position const m_HrafsirSpawnPos   = { 2132.361f, 2372.491f, 490.7057f, 3.76372f   };
            Position const m_ErilarSpawnPos    = { 2057.099f, 2473.283f, 483.9565f, 4.235073f  };
            Position const m_HridmogirSpawnPos = { 1998.809f, 2385.101f, 482.0347f, 0.9617527f };
            Position const m_AleifirSpawnPos   = { 2033.698f, 2186.07f,  465.2264f, 0.1326201f };

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2)
                {
                    switch (go->GetEntry())
                    {
                        case eGobs::Gob_HrafsirRitualCircle:
                        {
                            if (Creature* l_Creature = p_Unit->SummonCreature(eNpcs::Npc_Hrafsir, m_HrafsirSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000, 0, p_Unit->GetGUID()))
                            {
                                uint64 l_PlayerGuid = p_Unit->GetGUID();

                                if (GameObject* l_Gameobject = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 3, l_PlayerGuid))
                                {
                                    l_Gameobject->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 11049);
                                }

                                l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    {
                                        l_Creature->AI()->Talk(0, l_PlayerGuid);
                                    }

                                    Position const l_FlyUpPos = { 2132.361f, 2372.491f, 500.789f };
                                    l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_FlyUpPos);
                                }, 2000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Hrafsir[4] =
                                    {
                                        { 2119.339f, 2381.438f, 513.1914f },
                                        { 2050.934f, 2435.809f, 534.4803f },
                                        { 2015.967f, 2514.359f, 560.6859f },
                                        { 2047.042f, 2553.039f, 602.535f  }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Hrafsir, 4);
                                }, 9000);
                            }

                            break;
                        }
                        case eGobs::Gob_HridmogirRitualCircle:
                        {
                            if (Creature* l_Creature = p_Unit->SummonCreature(eNpcs::Npc_Hridmogir, m_HridmogirSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000, 0, p_Unit->GetGUID()))
                            {
                                uint64 l_PlayerGuid = p_Unit->GetGUID();

                                if (GameObject* l_Gameobject = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 3, l_PlayerGuid))
                                {
                                    l_Gameobject->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 11049);
                                }

                                l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    {
                                        l_Creature->AI()->Talk(0, l_PlayerGuid);
                                    }
                                }, 1000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    Position const l_FlyUpPos = { 1998.809f, 2385.101f, 492.1181f };
                                    l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_FlyUpPos);
                                }, 7000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Hridmogir[6] =
                                    {
                                        { 2011.274f, 2404.344f, 503.7035f },
                                        { 2008.856f, 2438.155f, 508.6901f },
                                        { 1977.847f, 2481.766f, 523.9328f },
                                        { 1991.439f, 2516.594f, 533.0993f },
                                        { 2016.259f, 2537.281f, 550.1844f },
                                        { 2046.087f, 2554.989f, 587.6499f }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Hridmogir, 6);
                                }, 8000);
                            }

                            break;
                        }
                        case eGobs::Gob_ErilarRitualCircle:
                        {
                            if (Creature* l_Creature = p_Unit->SummonCreature(eNpcs::Npc_Erilar, m_ErilarSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000, 0, p_Unit->GetGUID()))
                            {
                                uint64 l_PlayerGuid = p_Unit->GetGUID();

                                if (GameObject* l_Gameobject = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 3, l_PlayerGuid))
                                {
                                    l_Gameobject->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 11049);
                                }

                                l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                                {
                                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    {
                                        l_Creature->AI()->Talk(0, l_PlayerGuid);
                                    }
                                }, 2000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    Position const l_FlyUpPos = { 2057.099f, 2473.283f, 494.0398f };
                                    l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_FlyUpPos);
                                }, 8000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Erilar[5] =
                                    {
                                        { 2042.922f, 2472.997f, 501.0861f },
                                        { 2025.068f, 2492.417f, 509.4812f },
                                        { 1981.653f, 2581.808f, 547.0822f },
                                        { 1978.962f, 2631.127f, 579.7579f },
                                        { 1982.613f, 2676.106f, 614.285f  }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Erilar, 5);
                                }, 9000);
                            }

                            break;
                        }
                        case eGobs::Gob_AleifirRitualCircle:
                        {
                            if (Creature* l_Creature = p_Unit->SummonCreature(eNpcs::Npc_Aleifir, m_AleifirSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000, 0, p_Unit->GetGUID()))
                            {
                                l_Creature->AI()->Talk(0, p_Unit->GetGUID());

                                if (GameObject* l_Gameobject = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 3, p_Unit->GetGUID()))
                                {
                                    l_Gameobject->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 11049);
                                }

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Aleifir[27] =
                                    {
                                        { 2036.265f, 2186.702f, 465.6555f },
                                        { 2039.765f, 2187.202f, 465.6555f },
                                        { 2042.015f, 2187.452f, 465.6555f },
                                        { 2043.015f, 2187.452f, 465.6555f },
                                        { 2045.015f, 2187.952f, 465.6555f },
                                        { 2048.015f, 2188.202f, 465.6555f },
                                        { 2049.265f, 2188.452f, 465.6555f },
                                        { 2053.265f, 2188.952f, 465.6555f },
                                        { 2060.265f, 2189.952f, 465.9055f },
                                        { 2061.515f, 2190.202f, 465.9055f },
                                        { 2063.265f, 2190.452f, 465.9055f },
                                        { 2066.765f, 2190.952f, 465.9055f },
                                        { 2068.015f, 2190.952f, 465.9055f },
                                        { 2072.015f, 2191.452f, 466.1555f },
                                        { 2077.765f, 2192.202f, 466.1555f },
                                        { 2079.765f, 2192.452f, 466.1555f },
                                        { 2080.765f, 2192.702f, 466.1555f },
                                        { 2083.265f, 2192.952f, 466.1555f },
                                        { 2084.765f, 2193.202f, 466.1555f },
                                        { 2088.265f, 2193.702f, 466.9055f },
                                        { 2089.765f, 2193.952f, 466.9055f },
                                        { 2092.765f, 2194.202f, 467.4055f },
                                        { 2095.765f, 2194.702f, 467.9055f },
                                        { 2097.265f, 2194.952f, 467.9055f },
                                        { 2100.015f, 2195.202f, 467.9055f },
                                        { 2104.265f, 2195.952f, 467.6555f },
                                        { 2108.333f, 2195.834f, 467.5011f }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_GroundPathEnd, Path_Aleifir, 27, false);
                                }, 2000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    Position const l_FlyUpPos = { 2108.333f, 2195.834f, 477.5011f };
                                    l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_FlyUpPos);
                                }, 7000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    G3D::Vector3 const Path_Aleifir[4] =
                                    {
                                        { 2133.002f, 2189.667f, 489.2584f },
                                        { 2177.512f, 2241.927f, 514.7372f },
                                        { 2128.748f, 2323.031f, 527.0399f },
                                        { 2047.585f, 2453.273f, 578.1786f }
                                    };

                                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Aleifir, 4);
                                }, 8000);
                            }

                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_stormheim_ritual_circlesAI(go);
        }
};

/// Offering Bowl - 241868, 241864, 241869
class gob_stormheim_offering_bowls : public GameObjectScript
{
    public:
        gob_stormheim_offering_bowls() : GameObjectScript("gob_stormheim_offering_bowls") { }

        struct gob_stormheim_offering_bowlsAI : public GameObjectAI
        {
            gob_stormheim_offering_bowlsAI(GameObject* go) : GameObjectAI(go) { }

            enum eNpcs
            {
                Npc_Bunny                = 35845,
                Npc_RythasTheOracle      = 92918,
                Npc_OktelDragonblood     = 92920,
                Npc_HeimirOfTheBlackFist = 92889
            };

            enum eSpells
            {
                Spell_TorchFlameVisual = 184830,
                Spell_IncenseVisual    = 184831
            };

            enum eGobs
            {
                Gob_OfferingBowlRythas = 241868,
                Gob_OfferingBowlOktel  = 241864,
                Gob_OfferingBowlHeimir = 241869
            };

            Position const m_RythasTheOracleSpawnPos      = { 3378.224f, 1279.495f, 128.6259f, 4.462586f };
            Position const m_OktelDragonbloodSpawnPos     = { 3415.488f, 1162.41f,  138.7502f, 3.968045f };
            Position const m_HeimirOfTheBlackFistSpawnPos = { 3514.32f,  1102.326f, 158.0024f, 2.091035f };

            void OnLootStateChanged(uint32 p_State, Unit* /*p_Unit*/) override
            {
                if (p_State == 2)
                {
                    switch (go->GetEntry())
                    {
                        case eGobs::Gob_OfferingBowlRythas:
                        {
                            if (go->FindNearestCreature(eNpcs::Npc_RythasTheOracle, 100.0f, true))
                                return;

                            std::list<Creature*> l_CreatureList;
                            go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_Bunny, 10.0f);

                            if (l_CreatureList.empty())
                                return;

                            for (auto itr : l_CreatureList)
                            {
                                if (int32(itr->GetPosition().m_positionZ) == 128)
                                {
                                    itr->CastSpell(itr, eSpells::Spell_TorchFlameVisual, true);
                                }
                                else
                                {
                                    itr->CastSpell(itr, eSpells::Spell_IncenseVisual, true);
                                }
                            }

                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_RythasTheOracle, m_RythasTheOracleSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
                                {
                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->AI()->Talk(0);
                                    }, 2000);

                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->AI()->Talk(1);
                                    }, 9000);

                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                    }, 10000);
                                }
                            }, 2000);

                            break;
                        }
                        case eGobs::Gob_OfferingBowlOktel:
                        {
                            if (go->FindNearestCreature(eNpcs::Npc_OktelDragonblood, 100.0f, true))
                                return;

                            std::list<Creature*> l_CreatureList;
                            go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_Bunny, 10.0f);

                            if (l_CreatureList.empty())
                                return;

                            for (auto itr : l_CreatureList)
                            {
                                if (int32(itr->GetPosition().m_positionY) == 1154 || int32(itr->GetPosition().m_positionY) == 1151)
                                {
                                    itr->CastSpell(itr, eSpells::Spell_TorchFlameVisual, true);
                                }
                                else
                                {
                                    itr->CastSpell(itr, eSpells::Spell_IncenseVisual, true);
                                }
                            }

                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_OktelDragonblood, m_OktelDragonbloodSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
                                {
                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->AI()->Talk(0);
                                    }, 3000);

                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->AI()->Talk(1);
                                    }, 9000);

                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                    }, 10000);
                                }
                            }, 1000);

                            break;
                        }
                        case eGobs::Gob_OfferingBowlHeimir:
                        {
                            if (go->FindNearestCreature(eNpcs::Npc_HeimirOfTheBlackFist, 100.0f, true))
                                return;

                            std::list<Creature*> l_CreatureList;
                            go->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_Bunny, 10.0f);

                            if (l_CreatureList.empty())
                                return;

                            for (auto itr : l_CreatureList)
                            {
                                if (int32(itr->GetPosition().m_positionX) == 3515 || int32(itr->GetPosition().m_positionX) == 3512)
                                {
                                    itr->CastSpell(itr, eSpells::Spell_TorchFlameVisual, true);
                                }
                                else
                                {
                                    itr->CastSpell(itr, eSpells::Spell_IncenseVisual, true);
                                }
                            }

                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_HeimirOfTheBlackFist, m_HeimirOfTheBlackFistSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
                                {
                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->AI()->Talk(0);
                                    }, 3000);

                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->AI()->Talk(1);
                                    }, 9000);

                                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                                    {
                                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                    }, 10000);
                                }
                            }, 1000);

                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_stormheim_offering_bowlsAI(go);
        }
};

/// Ritual Stone - 251413, 241849, 251412
class gob_stormheim_ritual_stones : public GameObjectScript
{
    public:
        gob_stormheim_ritual_stones() : GameObjectScript("gob_stormheim_ritual_stones") { }

        struct gob_stormheim_ritual_stonesAI : public GameObjectAI
        {
            gob_stormheim_ritual_stonesAI(GameObject* go) : GameObjectAI(go) { }

            enum eKillcredits
            {
                Killcredit_RitualStoneDestroyed = 109061
            };

            void InitializeAI() override
            {
                if (go->GetGoState() == GOState::GO_STATE_ACTIVE)
                {
                    go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                }
            }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2 && p_Unit && p_Unit->ToPlayer())
                {
                    if (GameObject* l_Gob = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 0, p_Unit->GetGUID()))
                    {
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 3761);

                        l_Gob->AddDelayedEvent([l_Gob]() -> void
                        {
                            l_Gob->Delete();
                        }, 3000);
                    }

                    p_Unit->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_RitualStoneDestroyed);
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_stormheim_ritual_stonesAI(go);
        }
};

/// Helya's Altar - 244559
class gob_helyas_altar_244559 : public GameObjectScript
{
    public:
        gob_helyas_altar_244559() : GameObjectScript("gob_helyas_altar_244559") { }

        struct gob_helyas_altar_244559AI : public GameObjectAI
        {
            gob_helyas_altar_244559AI(GameObject* go) : GameObjectAI(go) { }

            enum eSpells
            {
                Spell_HelyaConversationAfterFirstGossip = 212898,
                Spell_HelyasDominance                   = 192395,
                Spell_HelyaConversationAfterAcceptQuest = 212900,
                Spell_HelyaFirstLeaveConversation       = 212916,
                Spell_HelyaSecondLeaveConversation      = 212917,
                Spell_HelyaThirdLeaveConversation       = 212918,
                Spell_HelyaFourthLeaveConversation      = 212919,
                Spell_SummonLevitateBunny               = 228702,
                Spell_LeavingHelheimEffect              = 192668,
                Spell_SummonLevitateBunnyPlayer         = 192666,
                Spell_LevitateBunnyEject                = 192676,
                Spell_HelheimExitTeleportFly            = 192669,
                Spell_HelheimExitTeleport               = 192670
            };

            enum eNpcs
            {
                Npc_Helya           = 91387,
                Npc_Ashildir        = 97664,
                Npc_ValkyraGuardian = 97665
            };

            enum eKillcredits
            {
                Killcredit_AltarGossipCheck = 3985500,
                Killcredit_HelheimEscaped   = 97487
            };

            Position const m_HelyaSpawnPos         = { 549.7691f, 763.1024f, 1.253749f, 3.93789f   };
            Position const m_AshildirSpawnPos      = { 489.3559f, 716.118f,  23.44108f, 0.4214985f };
            Position const m_FirstValkyraSpawnPos  = { 468.1493f, 712.6268f, 19.83929f, 0.6997211f };
            Position const m_SecondValkyraSpawnPos = { 473.8177f, 705.0521f, 19.80802f, 0.5015526f };
            Position const m_ThirdValkyraSpawnPos  = { 482.3403f, 721.2882f, 23.44108f, 0.6540835f };

            bool GossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 p_Action) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();

                switch (p_Action)
                {
                    case 0:
                    {
                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_Helya, m_HelyaSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            l_Creature->DespawnOrUnsummon(6000);
                        }

                        p_Player->CastSpell(p_Player, eSpells::Spell_HelyaConversationAfterFirstGossip, true);
                        p_Player->CastSpell(p_Player, eSpells::Spell_HelyasDominance, true);
                        break;
                    }
                    case 1:
                    {
                        p_Player->KilledMonsterCredit(eKillcredits::Killcredit_AltarGossipCheck);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HelyaFirstLeaveConversation, true, 9000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HelyaSecondLeaveConversation, true, 19000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_LeavingHelheimEffect, true, 14000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_SummonLevitateBunnyPlayer, true, 14000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HelyaThirdLeaveConversation, true, 27000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HelyaFourthLeaveConversation, true, 40000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_LevitateBunnyEject, true, 40000);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HelheimExitTeleportFly, true, 40100);
                        p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HelheimExitTeleport, true, 44200);

                        p_Player->AddDelayedEvent([p_Player]() -> void
                        {
                            p_Player->KilledMonsterCredit(eKillcredits::Killcredit_HelheimEscaped);
                        }, 44000);


                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_Helya, m_HelyaSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->SetDisplayId(27824);
                            l_Creature->DespawnOrUnsummon(50000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetDisplayId(65043);
                                l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            }, 9000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetDisplayId(65043);
                                l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            }, 14000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetDisplayId(65043);
                                l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            }, 27000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetDisplayId(65043);
                                l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            }, 41000);
                        }

                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_Ashildir, m_AshildirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            uint64 l_SummonerGuid = p_Player->GetGUID();

                            l_Creature->DespawnOrUnsummon(60000);

                            l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                                {
                                    l_Creature->AI()->Talk(0, l_SummonerGuid);
                                }
                            }, 2000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 33588032);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_LeavingHelheimEffect, true);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_SummonLevitateBunny, true);
                            }, 13000);

                            l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                                {
                                    l_Creature->AI()->Talk(1, l_SummonerGuid);
                                }
                            }, 23000);

                            l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                            {
                                if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                                {
                                    l_Creature->AI()->Talk(2, l_SummonerGuid);
                                }
                            }, 33000);
                        }

                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_FirstValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                            l_Creature->DespawnOrUnsummon(60000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                                l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 33587456);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_LeavingHelheimEffect, true);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_SummonLevitateBunny, true);
                            }, 18000);
                        }

                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_SecondValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                            l_Creature->DespawnOrUnsummon(60000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                                l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 33587456);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_LeavingHelheimEffect, true);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_SummonLevitateBunny, true);
                            }, 18000);
                        }

                        if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_ValkyraGuardian, m_ThirdValkyraSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                        {
                            l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                            l_Creature->DespawnOrUnsummon(60000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                                l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 33587456);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_LeavingHelheimEffect, true);
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_SummonLevitateBunny, true);
                            }, 18000);
                        }

                        break;
                    }
                    default:
                        break;
                }

                return true;
            }

            bool QuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_Helya, m_HelyaSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                {
                    l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    l_Creature->DespawnOrUnsummon(6000);
                }

                p_Player->CastSpell(p_Player, eSpells::Spell_HelyaConversationAfterAcceptQuest, true);
                return true;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_helyas_altar_244559AI(go);
        }
};

/// Plant Explosives - 243455
class gob_plant_explosives_243455 : public GameObjectScript
{
    public:
        gob_plant_explosives_243455() : GameObjectScript("gob_plant_explosives_243455") { }

        struct gob_plant_explosives_243455AI : public GameObjectAI
        {
            gob_plant_explosives_243455AI(GameObject* go) : GameObjectAI(go) { }

            enum eSpells
            {
                Spell_CreateDetonator = 189152
            };

            enum eObjectives
            {
                Obj_ExplosivesPlanted = 279315
            };

            enum eItems
            {
                Item_Detonator = 128287
            };

            void InitializeAI() override
            {
                go->SetUInt32Value(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 37794);
            }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2 && p_Unit && p_Unit->ToPlayer())
                {
                    go->DestroyForPlayer(p_Unit->ToPlayer());

                    if (GameObject* l_Gob = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), go->GetRotation(), 0, p_Unit->GetGUID(), nullptr, 0, 0, 0, 1, 1))
                    {
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 8743);

                        l_Gob->AddDelayedEvent([l_Gob]() -> void
                        {
                            l_Gob->Delete();
                        }, 360000);
                    }

                    p_Unit->ToPlayer()->UpdateObjectVisibility();

                    if (p_Unit->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_ExplosivesPlanted) == 3 &&
                        !p_Unit->ToPlayer()->HasItemCount(eItems::Item_Detonator, 1))
                    {
                        p_Unit->CastSpell(p_Unit, eSpells::Spell_CreateDetonator, true);
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_plant_explosives_243455AI(go);
        }
};

/// Engraved Shield - 251557, 251558, 251559, 251560, 251561, 242673
class gob_stormheim_engraved_shield : public GameObjectScript
{
    public:
        gob_stormheim_engraved_shield() : GameObjectScript("gob_stormheim_engraved_shield") { }

        struct gob_stormheim_engraved_shieldAI : public GameObjectAI
        {
            gob_stormheim_engraved_shieldAI(GameObject* go) : GameObjectAI(go) { }

            enum eQuests
            {
                Quest_ShieldedSecretsHorde    = 38878,
                Quest_ShieldedSecretsAlliance = 39063
            };

            enum eKillcredits
            {
                Killcredit_ExamineEngravedShields = 109346
            };

            bool GossipHello(Player* p_Player) override
            {
                Quest const* l_QuestInfo = sObjectMgr->GetQuestTemplate(p_Player->GetTeam() == HORDE ? eQuests::Quest_ShieldedSecretsHorde : eQuests::Quest_ShieldedSecretsAlliance);
                if (!l_QuestInfo)
                    return false;

                for (QuestObjective const& obj : l_QuestInfo->QuestObjectives)
                {
                    if (obj.ObjectID == go->GetEntry())
                    {
                        if (!p_Player->IsQuestObjectiveComplete(l_QuestInfo, &obj))
                            p_Player->KilledMonsterCredit(eKillcredits::Killcredit_ExamineEngravedShields);
                    }
                }

                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_stormheim_engraved_shieldAI(go);
        }
};

/// Skyfire Propeller - 240235
class gob_skyfire_propeller_240235 : public GameObjectScript
{
    public:
        gob_skyfire_propeller_240235() : GameObjectScript("gob_skyfire_propeller_240235") { }

        struct gob_skyfire_propeller_240235AI : public GameObjectAI
        {
            gob_skyfire_propeller_240235AI(GameObject* go) : GameObjectAI(go) { }

            G3D::Vector3 const Path_FirstForsakenRider[5] =
            {
                { 3220.259f, 3149.763f, 526.0599f },
                { 3225.634f, 3155.685f, 526.3099f },
                { 3233.024f, 3163.827f, 526.5599f },
                { 3235.04f,  3166.048f, 526.8f    },
                { 3237.056f, 3168.269f, 527.0709f }
            };

            G3D::Vector3 const Path_SecondForsakenRider[6] =
            {
                { 3222.185f, 3149.775f, 526.3173f },
                { 3224.685f, 3159.025f, 526.5673f },
                { 3225.937f, 3163.458f, 526.8099f },
                { 3230.327f, 3166.439f, 526.7493f },
                { 3231.327f, 3168.189f, 526.9993f },
                { 3233.788f, 3172.093f, 527.188f  }
            };

            enum eNpcs
            {
                Npc_SignalBoostController   = 92475,
                Npc_VfxBunny                = 72587,
                Npc_ForsakenBatRider        = 90899,
                Npc_ForsakenDeathstalker    = 92224,
                Npc_ForsakenDreadwing       = 92516,
                Npc_MindControlledDreadwing = 92547,
                Npc_Vethir                  = 90907,
                Npc_DaveMurphy              = 91082,
                Npc_AllianceGyrocopter      = 91084
            };

            enum eSpells
            {
                Spell_DistressSignalAreatrigger        = 183267,
                Spell_DistressSignalVisual             = 183439,
                Spell_DistressSignalPlayer             = 183266,
                Spell_OversparkTalkingHeadConversation = 242484,
                Spell_CallForsakenDreadwing            = 180797,
                Spell_Missiles                         = 183426
            };

            enum ePoints
            {
                Point_FlyEnd       = 1,
                Point_LeavePathEnd = 2
            };

            enum eDatas
            {
                Data_RiderDied          = 1,
                Data_StalkerDied        = 2,
                Data_GyrocoptersArrived = 3,
                Data_VethirDefeated     = 4,
                Data_ResetEvent         = 5
            };

            enum eActions
            {
                Action_FirstDreadwing    = 1,
                Action_SecondDreadwing   = 2,
                Action_StartEventEndPath = 3
            };

            enum eQuests
            {
                Quest_SignalBoost = 38060
            };

            std::vector<ObjectGuid> m_SummonGuids;
            ObjectGuid m_MindControlledDreadwingGuid;
            Position const m_SignalBoostControllerSpawnPos      = { 3248.893f, 3170.679f, 526.2991f, 1.34892f   };
            Position const m_VfxBunnySpawnPos                   = { 3245.816f, 3147.566f, 527.3777f, 1.34892f   };
            Position const m_FirstForsakenRiderSpawnPos         = { 3218.915f, 3148.282f, 526.3576f, 0.8337698f };
            Position const m_SecondForsakenRiderSpawnPos        = { 3220.932f, 3145.593f, 525.8246f, 1.018374f  };
            Position const m_FirstForsakenDeathstalkerSpawnPos  = { 3261.634f, 3136.629f, 525.7294f, 1.138425f  };
            Position const m_SecondForsakenDeathstalkerSpawnPos = { 3264.234f, 3133.124f, 526.9566f, 1.187939f  };
            Position const m_MindControlledDreadwingSpawnPos    = { 3202.607f, 3120.728f, 510.1589f, 1.229006f  };
            Position const m_VethirSpawnPos                     = { 3205.991f, 3250.76f,  614.7765f, 2.960875f  };
            Position const m_DaveMurphySpawnPos                 = { 3277.277f, 3149.584f, 557.2189f, 2.364597f  };
            Position const m_FirstGyrocopterSpawnPos            = { 3282.003f, 3166.26f,  566.736f,  2.418694f  };
            Position const m_SecondGyrocopterSpawnPos           = { 3270.241f, 3143.815f, 560.7479f, 2.150572f  };
            Position const m_ThirdGyrocopterSpawnPos            = { 3226.026f, 3152.445f, 553.815f,  1.092906f  };
            uint8 m_RidersDied   = 0;
            uint8 m_StalkersDied = 0;

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2)
                {
                    if (p_Unit->ToPlayer() && p_Unit->ToPlayer()->GetQuestStatus(eQuests::Quest_SignalBoost) == QUEST_STATUS_INCOMPLETE)
                    {
                        go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 20);

                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_SignalBoostController, m_SignalBoostControllerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_DistressSignalAreatrigger, true);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_OversparkTalkingHeadConversation, true);
                            }, 3000);

                            m_SummonGuids.push_back(l_Creature->GetGUID());
                        }

                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_VfxBunny, m_VfxBunnySpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_DistressSignalVisual, true);
                            m_SummonGuids.push_back(l_Creature->GetGUID());
                        }

                        go->AddDelayedEvent([this]() -> void
                        {
                            if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_ForsakenBatRider, m_FirstForsakenRiderSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_CallForsakenDreadwing, true);
                                l_Creature->Mount(62247);
                                l_Creature->SetSpeed(MOVE_FLIGHT, 2.0f);
                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, Path_FirstForsakenRider, 5);
                                m_SummonGuids.push_back(l_Creature->GetGUID());
                            }

                            if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_ForsakenBatRider, m_SecondForsakenRiderSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_CallForsakenDreadwing, true);
                                l_Creature->Mount(62247);
                                l_Creature->SetSpeed(MOVE_FLIGHT, 2.0f);
                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, Path_SecondForsakenRider, 6);
                                m_SummonGuids.push_back(l_Creature->GetGUID());
                            }
                        }, 6000);
                    }
                }
            }

            void SetData(uint32 /*p_Id*/, uint32 p_Value) override
            {
                switch (p_Value)
                {
                    case eDatas::Data_RiderDied:
                    {
                        m_RidersDied++;

                        if (m_RidersDied == 2)
                        {
                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_ForsakenDeathstalker, m_FirstForsakenDeathstalkerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    if (Creature* l_Dreadwing = l_Creature->SummonCreature(eNpcs::Npc_ForsakenDreadwing, l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        l_Dreadwing->AI()->DoAction(eActions::Action_FirstDreadwing);
                                    }

                                    m_SummonGuids.push_back(l_Creature->GetGUID());
                                }

                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_ForsakenDeathstalker, m_SecondForsakenDeathstalkerSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    if (Creature* l_Dreadwing = l_Creature->SummonCreature(eNpcs::Npc_ForsakenDreadwing, l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        l_Dreadwing->AI()->DoAction(eActions::Action_SecondDreadwing);
                                    }

                                    m_SummonGuids.push_back(l_Creature->GetGUID());
                                }
                            }, 3000);
                        }

                        break;
                    }
                    case eDatas::Data_StalkerDied:
                    {
                        m_StalkersDied++;

                        if (m_StalkersDied == 2)
                        {
                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_MindControlledDreadwing, m_MindControlledDreadwingSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    m_MindControlledDreadwingGuid = l_Creature->GetGUID();
                                    m_SummonGuids.push_back(l_Creature->GetGUID());
                                }
                            }, 6000);

                            go->AddDelayedEvent([this]() -> void
                            {
                                if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_Vethir, m_VethirSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    l_Creature->AI()->SetGUID(go->GetGUID());
                                    m_SummonGuids.push_back(l_Creature->GetGUID());
                                }
                            }, 14000);
                        }
                        break;
                    }
                    case eDatas::Data_GyrocoptersArrived:
                    {
                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_DaveMurphy, m_DaveMurphySpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                            l_Creature->Mount(44634);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3277.277f, 3149.584f, 532.2189f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, m_FlyPos);
                            }, 1000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                G3D::Vector3 const Path_DaveMurphy[4] =
                                {
                                    { 3292.883f, 3131.767f, 533.2443f },
                                    { 3301.383f, 3121.767f, 533.7443f },
                                    { 3303.383f, 3119.517f, 533.9943f },
                                    { 3307.99f,  3113.95f,  533.7696f }
                                };
                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LeavePathEnd, Path_DaveMurphy, 4);
                            }, 17000);

                            m_SummonGuids.push_back(l_Creature->GetGUID());
                        }

                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_AllianceGyrocopter, m_FirstGyrocopterSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3282.003f, 3166.26f, 541.736f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, m_FlyPos);
                            }, 2000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Missiles);
                            }, 6000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Missiles);
                            }, 8000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3322.088f, 3099.121f, 562.0291f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LeavePathEnd, m_FlyPos);
                            }, 21000);

                            m_SummonGuids.push_back(l_Creature->GetGUID());
                        }

                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_AllianceGyrocopter, m_SecondGyrocopterSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3270.241f, 3143.815f, 535.7479f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, m_FlyPos);
                            }, 1000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Missiles);
                            }, 5000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Missiles);
                            }, 7000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3320.194f, 3097.207f, 561.0661f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LeavePathEnd, m_FlyPos);
                            }, 20000);

                            m_SummonGuids.push_back(l_Creature->GetGUID());
                        }

                        if (Creature* l_Creature = go->SummonCreature(eNpcs::Npc_AllianceGyrocopter, m_ThirdGyrocopterSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3226.026f, 3152.445f, 528.815f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, m_FlyPos);
                            }, 1000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Missiles);
                            }, 5000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->CastSpell(l_Creature, eSpells::Spell_Missiles);
                            }, 7000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position const m_FlyPos = { 3318.878f, 3095.796f, 561.6509f };

                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LeavePathEnd, m_FlyPos);
                            }, 19000);

                            m_SummonGuids.push_back(l_Creature->GetGUID());
                        }

                        break;
                    }
                    case eDatas::Data_VethirDefeated:
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*go, m_MindControlledDreadwingGuid))
                        {
                            l_Creature->AI()->DoAction(eActions::Action_StartEventEndPath);
                        }

                        go->AddDelayedEvent([this]() -> void
                        {
                            go->AI()->SetData(0, eDatas::Data_ResetEvent);
                        }, 24000);

                        break;
                    }
                    case eDatas::Data_ResetEvent:
                    {
                        for (auto itr : m_SummonGuids)
                        {
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*go, itr))
                            {
                                l_Creature->DespawnOrUnsummon();
                            }
                        }

                        go->SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, 0);
                        m_RidersDied = 0;
                        m_StalkersDied = 0;
                        m_SummonGuids.clear();
                        m_MindControlledDreadwingGuid.Clear();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_skyfire_propeller_240235AI(go);
        }
};

// Custom Salute Areatrigger - 39698
class at_stormheim_salute_horde : public AreaTriggerEntityScript
{
    public:
        at_stormheim_salute_horde() : AreaTriggerEntityScript("at_stormheim_salute_horde") { }

        enum eSpells
        {
            Spell_SaluteOverride = 220293,
            Spell_DummyAura      = 94562
        };

        bool OnAddTarget(AreaTrigger* /*p_Areatrigger*/, Unit* p_Target) override
        {
            if (p_Target->HasAura(eSpells::Spell_DummyAura))
            {
                p_Target->CastSpell(p_Target, eSpells::Spell_SaluteOverride, true);
            }

            return false;
        }

        bool OnRemoveTarget(AreaTrigger* /*p_Areatrigger*/, Unit* p_Target) override
        {
            p_Target->RemoveAura(eSpells::Spell_SaluteOverride);
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_stormheim_salute_horde();
        }
};

// Attacker Grapple Area Trigger - 4039
class at_stormheim_attacker_grapple : public AreaTriggerEntityScript
{
    public:
        at_stormheim_attacker_grapple() : AreaTriggerEntityScript("at_stormheim_attacker_grapple") { }

        enum eSpells
        {
            Spell_GrapplingGunSummon = 182941,
            Spell_RideDragon         = 182998
        };

        enum eNpcs
        {
            Npc_Vethir = 92302
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Owner = p_Areatrigger->GetCaster();
            if (!l_Owner || !l_Owner->ToCreature() || p_Target->GetEntry() != eNpcs::Npc_Vethir ||
                !p_Target->GetVehicleKit() || p_Target->GetVehicleKit()->GetAvailableSeatCount() < 2)
                return false;

            l_Owner->CastSpell(l_Owner, eSpells::Spell_GrapplingGunSummon, true);

            l_Owner->AddDelayedEvent([l_Owner]() -> void
            {
                l_Owner->CastSpell(l_Owner, eSpells::Spell_RideDragon, true);

                if (Unit* l_Unit = l_Owner->GetVehicleBase())
                {
                    l_Owner->Attack(l_Unit->ToUnit(), true);
                }
            }, 3000);

            l_Owner->AddDelayedEvent([l_Owner]() -> void
            {
                l_Owner->SetUInt32Value(UNIT_FIELD_BYTES_2, 1);
            }, 5000);

            p_Areatrigger->SetDuration(0);
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_stormheim_attacker_grapple();
        }
};

// Custom Salute Areatrigger - 38206
class at_stormheim_salute_alliance : public AreaTriggerEntityScript
{
    public:
        at_stormheim_salute_alliance() : AreaTriggerEntityScript("at_stormheim_salute_alliance") { }

        enum eSpells
        {
            Spell_SaluteOverride = 220526,
            Spell_DummyAura      = 94562
        };

        bool OnAddTarget(AreaTrigger* /*p_Areatrigger*/, Unit* p_Target) override
        {
            if (p_Target->HasAura(eSpells::Spell_DummyAura))
            {
                p_Target->CastSpell(p_Target, eSpells::Spell_SaluteOverride, true);
            }

            return false;
        }

        bool OnRemoveTarget(AreaTrigger* /*p_Areatrigger*/, Unit* p_Target) override
        {
            p_Target->RemoveAura(eSpells::Spell_SaluteOverride);
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_stormheim_salute_alliance();
        }
};

// Static Traps - 3830
class at_stormheim_static_traps : public AreaTriggerEntityScript
{
    public:
        at_stormheim_static_traps() : AreaTriggerEntityScript("at_stormheim_static_traps") { }

        enum eSpells
        {
            Spell_StaticField = 183706
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (p_Target->ToPlayer())
            {
                p_Target->CastSpell(p_Target, eSpells::Spell_StaticField, true);
                p_Areatrigger->SetDuration(0);
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_stormheim_static_traps();
        }
};

// Distress Signal - 4069
class at_stormheim_distress_signal : public AreaTriggerEntityScript
{
    public:
        at_stormheim_distress_signal() : AreaTriggerEntityScript("at_stormheim_distress_signal") { }

        enum eQuests
        {
            Quest_SignalBoost = 38060
        };

        enum eKillcredits
        {
            Killcredit_DistressSignalStarted = 92475
        };

        bool OnAddTarget(AreaTrigger* /*p_Areatrigger*/, Unit* p_Target) override
        {
            Player* l_Player = p_Target->ToPlayer();
            if (!l_Player)
                return false;

            if (l_Player->HasQuest(eQuests::Quest_SignalBoost))
            {
                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_DistressSignalStarted);
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_stormheim_distress_signal();
        }
};

// Blood Ritual - 7333 - 215083
class at_stormheim_blood_ritual : public AreaTriggerEntityScript
{
    public:
        at_stormheim_blood_ritual() : AreaTriggerEntityScript("at_stormheim_blood_ritual") { }

        enum eSpells
        {
            Spell_BloodRitualDamage = 215119
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            Player* l_Player = p_Target->ToPlayer();
            if (!l_Caster || !l_Player)
                return false;

            l_Caster->CastSpell(l_Player, eSpells::Spell_BloodRitualDamage, true);
            p_Areatrigger->Despawn();
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_stormheim_blood_ritual();
        }
};

/// Royal Summons - 191467
class spell_stormheim_royal_summons_horde : public SpellScriptLoader
{
    public:
        spell_stormheim_royal_summons_horde() : SpellScriptLoader("spell_stormheim_royal_summons_horde") { }

        class spell_stormheim_royal_summons_horde_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_royal_summons_horde_SpellScript);

            enum eNpcs
            {
                Npc_Nathanos = 96683
            };

            enum eObjectives
            {
                Obj_RoyalSummonsRead = 279847
            };

            Position const m_NathanosSpawnPos = { -840.5087f, 4633.81f, 749.58f, 4.168828f };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer() ||
                    l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_RoyalSummonsRead) != 0)
                    return;

                l_Caster->SummonCreature(eNpcs::Npc_Nathanos, m_NathanosSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID());
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_stormheim_royal_summons_horde_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_royal_summons_horde_SpellScript();
        }
};

/// Salute - 220291
class spell_stormheim_salute_horde : public SpellScriptLoader
{
    public:
        spell_stormheim_salute_horde() : SpellScriptLoader("spell_stormheim_salute_horde") { }

        class spell_stormheim_salute_horde_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_salute_horde_SpellScript);

            enum eNpcs
            {
                Npc_ForsakenRanger = 91950
            };

            enum eSpells
            {
                Spell_DummyAura = 94562
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                std::list<Creature*> l_CreatureList;
                l_Caster->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_ForsakenRanger, 50.0f);

                if (l_CreatureList.empty())
                    return;

                for (auto itr : l_CreatureList)
                {
                    if (itr->GetUInt32Value(UNIT_FIELD_EMOTE_STATE) == 214)
                    {
                        itr->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    }
                }

                l_Caster->RemoveAura(eSpells::Spell_DummyAura);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_salute_horde_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_salute_horde_SpellScript();
        }
};

/// Pump - 181981
class spell_stormheim_pump : public SpellScriptLoader
{
    public:
        spell_stormheim_pump() : SpellScriptLoader("spell_stormheim_pump") { }

        class spell_stormheim_pump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_pump_SpellScript);

            enum eKillcredits
            {
                Killcredit_ContainmentUnitFilled = 91590
            };

            void HandleHitTarget(SpellEffIndex p_EffectIndex)
            {
                PreventHitEffect(p_EffectIndex);

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_SpellInfo)
                    return;

                uint8 l_BasePoints = urand(15, 20);

                l_Caster->ModifyPower(Powers::POWER_ALTERNATE_POWER, l_BasePoints);

                for (uint8 i = 0; i < l_BasePoints; i++)
                {
                    l_Caster->ToPlayer()->KilledMonsterCredit(l_SpellInfo->Effects[EFFECT_0].MiscValue);
                }

                if (l_Caster->GetPower(Powers::POWER_ALTERNATE_POWER) == 100)
                {
                    l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_ContainmentUnitFilled);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_pump_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_KILL_CREDIT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_pump_SpellScript();
        }
};

/// Containment Unit - 181970
class spell_stormheim_containment_unit : public SpellScriptLoader
{
    public:
        spell_stormheim_containment_unit() : SpellScriptLoader("spell_stormheim_containment_unit") { }

        class spell_stormheim_containment_unit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_containment_unit_AuraScript);

            enum eObjectives
            {
                Obj_VolatileWaste = 277818
            };

            void HandleOnEffectPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                l_Caster->SetMaxPower(Powers::POWER_ALTERNATE_POWER, 100);
                l_Caster->SetPower(Powers::POWER_ALTERNATE_POWER, l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_VolatileWaste));
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_stormheim_containment_unit_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_containment_unit_AuraScript();
        }
};

/// Untie - 182313
class spell_stormheim_untie : public SpellScriptLoader
{
    public:
        spell_stormheim_untie() : SpellScriptLoader("spell_stormheim_untie") { }

        class spell_stormheim_untie_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_untie_SpellScript);

            enum eNpcs
            {
                Npc_DreadCaptainTattersail = 91904
            };

            enum ePoints
            {
                Point_DreadCaptainTattersailMoveEnd = 1
            };

            Position const m_TattersailSpawnPos = { 4413.555f, 2612.518f, 1.818938f, 1.92788f };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (Creature* l_Creature = l_Caster->SummonCreature(eNpcs::Npc_DreadCaptainTattersail, m_TattersailSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID()))
                {
                    uint64 l_PlayerGuid = l_Caster->GetGUID();

                    l_Creature->RemoveAllAuras();
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->PlayOneShotAnimKitId(6292);
                    }, 500);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Creature->AI()->Talk(0, l_PlayerGuid);
                    }, 1000);

                    l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                            l_Creature->AI()->Talk(1, l_PlayerGuid);
                    }, 8000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    }, 14000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_Tattersail[8] =
                        {
                            { 4412.881f, 2616.173f, 1.808357f  },
                            { 4408.381f, 2620.923f, 1.808357f  },
                            { 4405.131f, 2627.423f, 2.058357f  },
                            { 4398.131f, 2638.923f, 1.308357f  },
                            { 4391.881f, 2649.173f, 1.558357f  },
                            { 4385.881f, 2654.423f, 2.058357f  },
                            { 4378.631f, 2657.923f, 2.058357f  },
                            { 4368.708f, 2657.828f, 0.7144436f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_DreadCaptainTattersailMoveEnd, Path_Tattersail, 8, false);
                    }, 15000);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_untie_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_untie_SpellScript();
        }
};

/// Plague Arrow Warning = 186047
class spell_stormheim_plague_arrow_warning : public SpellScriptLoader
{
    public:
        spell_stormheim_plague_arrow_warning() : SpellScriptLoader("spell_stormheim_plague_arrow_warning") { }

        class spell_stormheim_plague_arrow_warning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_plague_arrow_warning_SpellScript);

            void HandleHitTarget(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = GetHitPlayer();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Player)
                    return;

                l_Caster->ToCreature()->AI()->Talk(2, l_Player->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_plague_arrow_warning_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_plague_arrow_warning_SpellScript();
        }
};

/// Investigating - 181532
/// Hack for quest Eyes in the Overlook (38308), because player can't reward this quest for some reasons
class spell_stormheim_investigating : public SpellScriptLoader
{
    public:
        spell_stormheim_investigating() : SpellScriptLoader("spell_stormheim_investigating") { }

        class spell_stormheim_investigating_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_investigating_SpellScript);

            enum eQuests
            {
                Quest_EyesInTheOverlook = 38308,
                Quest_MastersOfDisguise = 38317
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Quest const* l_QuestEyesInTheOverlook = sObjectMgr->GetQuestTemplate(eQuests::Quest_EyesInTheOverlook);
                Quest const* l_MastersOfDisguise = sObjectMgr->GetQuestTemplate(eQuests::Quest_MastersOfDisguise);
                if (!l_Caster || !l_Caster->ToPlayer() || !l_QuestEyesInTheOverlook || !l_MastersOfDisguise)
                    return;

                l_Caster->ToPlayer()->RewardQuest(l_QuestEyesInTheOverlook, 0, nullptr, true);
                l_Caster->ToPlayer()->AddQuest(l_MastersOfDisguise, nullptr);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_investigating_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_investigating_SpellScript();
        }
};

/// Fiery Blink - 195590
class spell_stormheim_fiery_blink : public SpellScriptLoader
{
    public:
        spell_stormheim_fiery_blink() : SpellScriptLoader("spell_stormheim_fiery_blink") { }

        class spell_stormheim_fiery_blink_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_fiery_blink_SpellScript);

            enum eSpells
            {
                Spell_FieryBlinkMissile  = 195631,
                Spell_FieryBlinkTeleport = 195669
            };

            void HandleHitTarget(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Unit* l_Victim = l_Caster->getVictim())
                {
                    l_Target->CastSpell(l_Victim, eSpells::Spell_FieryBlinkMissile);
                    l_Target->CastSpell(l_Target, eSpells::Spell_FieryBlinkTeleport);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_fiery_blink_SpellScript::HandleHitTarget, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_fiery_blink_SpellScript();
        }
};

/// Burning Glare - 195682, 195703
class spell_stormheim_burning_glare : public SpellScriptLoader
{
    public:
        spell_stormheim_burning_glare() : SpellScriptLoader("spell_stormheim_burning_glare") { }

        class spell_stormheim_burning_glare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_burning_glare_SpellScript);

            enum eSpells
            {
                Spell_BurningGlareTargetDummy = 195682,
                Spell_BurningGlareArea        = 195703,
                Spell_BurningGlareDamage      = 195688
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Unit* l_Victim = l_Caster->getVictim();
                if (!l_Victim)
                    return;

                if (m_scriptSpellId == eSpells::Spell_BurningGlareTargetDummy)
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_BurningGlareArea, true);
                else
                    l_Target->CastSpell(l_Victim, eSpells::Spell_BurningGlareDamage, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_burning_glare_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_burning_glare_SpellScript();
        }
};

/// Purifier Blast - 195764
class spell_stormheim_purifier_blast : public SpellScriptLoader
{
    public:
        spell_stormheim_purifier_blast() : SpellScriptLoader("spell_stormheim_purifier_blast") { }

        class spell_stormheim_purifier_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_purifier_blast_SpellScript);

            std::array<Position, 12> m_MissilesPos =
            { {
                {  -2.61f, 3.665f   },
                { -5.457f, 2.492f   },
                { -7.466f, -0.713f  },
                { -7.331f, -5.221f  },
                { -4.361f, -9.551f  },
                {  1.141f, -11.946f },
                {  7.831f, -10.997f },
                { 13.645f, -6.232f  },
                { 16.426f, 1.568f   },
                { 14.663f, 10.44f   },
                {  8.101f, 17.737f  },
                { -1.996f, 20.905f  }
            } };

            enum eSpells
            {
                Spell_PurifierBlastMissiles = 218842
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                uint16 l_Delay = 0;

                for (auto itr : m_MissilesPos)
                {
                    Position l_CasterPos = l_Caster->GetPosition();
                    l_CasterPos.m_positionX += itr.m_positionX;
                    l_CasterPos.m_positionY += itr.m_positionY;

                    l_Caster->AddDelayedEvent([l_Caster, l_CasterPos]() -> void
                    {
                        l_Caster->CastSpell(l_CasterPos, eSpells::Spell_PurifierBlastMissiles, false);
                    }, l_Delay);

                    l_Delay += 400;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_purifier_blast_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_purifier_blast_SpellScript();
        }
};

/// Lightforged Defense Matrix - 195799
class spell_stormheim_lightforged_defense_matrix : public SpellScriptLoader
{
    public:
        spell_stormheim_lightforged_defense_matrix() : SpellScriptLoader("spell_stormheim_lightforged_defense_matrix") { }

        class spell_stormheim_lightforged_defense_matrix_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_lightforged_defense_matrix_SpellScript);

            G3D::Vector3 const Path_MatrixFirst[7] =
            {
                { 3498.243f, 2146.537f, 230.1639f },
                { 3487.405f, 2158.237f, 230.1639f },
                { 3486.905f, 2171.487f, 230.1639f },
                { 3494.905f, 2179.737f, 230.1639f },
                { 3504.905f, 2183.737f, 230.1639f },
                { 3516.655f, 2177.987f, 230.1639f },
                { 3522.405f, 2163.987f, 230.1639f }
            };

            G3D::Vector3 const Path_MatrixSecond[7] =
            {
                { 3486.076f, 2157.123f, 230.1626f },
                { 3486.729f, 2171.51f,  230.1626f },
                { 3494.729f, 2179.76f,  230.1626f },
                { 3504.729f, 2183.76f,  230.1626f },
                { 3516.729f, 2178.01f,  230.1626f },
                { 3522.479f, 2164.01f,  230.1626f },
                { 3513.979f, 2148.51f,  230.1626f }
            };

            G3D::Vector3 const Path_MatrixThird[8] =
            {
                { 3507.834f, 2183.572f, 229.227f  },
                { 3517.254f, 2178.341f, 229.227f  },
                { 3522.377f, 2164.253f, 230.1617f },
                { 3513.877f, 2148.753f, 230.1617f },
                { 3496.877f, 2146.503f, 230.1617f },
                { 3495.127f, 2148.003f, 230.1617f },
                { 3487.377f, 2158.253f, 230.1617f },
                { 3486.877f, 2171.753f, 230.1617f }
            };

            G3D::Vector3 const Path_MatrixFourth[10] =
            {
                { 3522.160f, 2165.345f, 230.163f  },
                { 3513.848f, 2148.661f, 230.163f  },
                { 3497.098f, 2146.661f, 230.163f  },
                { 3495.098f, 2147.911f, 230.163f  },
                { 3487.348f, 2158.411f, 230.163f  },
                { 3486.848f, 2171.661f, 230.163f  },
                { 3494.848f, 2180.161f, 230.163f  },
                { 3504.69f,  2183.783f, 230.0225f },
                { 3516.669f, 2178.104f, 230.0225f },
                { 3522.386f, 2164.075f, 230.0225f }
            };

            enum eSpells
            {
                Spell_LightforgedDefenseMatrixPeriodic = 195811
            };

            enum eNpcs
            {
                Npc_LightforgedDefenseMatrix = 99573
            };

            enum ePoints
            {
                Point_MatrixPathEnd = 1
            };

            Position const m_FirstMatrixSpawnPos  = { 3512.857f, 2146.467f, 229.3054f };
            Position const m_SecondMatrixSpawnPos = { 3495.641f, 2146.682f, 229.3028f };
            Position const m_ThirdMatrixSpawnPos  = { 3496.652f, 2182.205f, 229.301f  };
            Position const m_FourthMatrixSpawnPos = { 3513.005f, 2182.038f, 229.3036f };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (int32(l_Target->GetPositionX()) == 3529 && int32(l_Target->GetPositionY()) == 2129)
                {
                    if (Creature* l_Creature = l_Target->SummonCreature(eNpcs::Npc_LightforgedDefenseMatrix, m_FirstMatrixSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        l_Target->CastSpell(l_Creature, eSpells::Spell_LightforgedDefenseMatrixPeriodic, true);
                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MatrixPathEnd, Path_MatrixFirst, 7, false);
                    }
                }
                else if (int32(l_Target->GetPositionX()) == 3479 && int32(l_Target->GetPositionY()) == 2129)
                {
                    if (Creature* l_Creature = l_Target->SummonCreature(eNpcs::Npc_LightforgedDefenseMatrix, m_SecondMatrixSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        l_Target->CastSpell(l_Creature, eSpells::Spell_LightforgedDefenseMatrixPeriodic, true);
                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MatrixPathEnd, Path_MatrixSecond, 7, false);
                    }
                }
                else if (int32(l_Target->GetPositionX()) == 3481 && int32(l_Target->GetPositionY()) == 2200)
                {
                    if (Creature* l_Creature = l_Target->SummonCreature(eNpcs::Npc_LightforgedDefenseMatrix, m_ThirdMatrixSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        l_Target->CastSpell(l_Creature, eSpells::Spell_LightforgedDefenseMatrixPeriodic, true);
                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MatrixPathEnd, Path_MatrixThird, 8, false);
                    }
                }
                else if (int32(l_Target->GetPositionX()) == 3529 && int32(l_Target->GetPositionY()) == 2197)
                {
                    if (Creature* l_Creature = l_Target->SummonCreature(eNpcs::Npc_LightforgedDefenseMatrix, m_FourthMatrixSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        l_Target->CastSpell(l_Creature, eSpells::Spell_LightforgedDefenseMatrixPeriodic, true);
                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MatrixPathEnd, Path_MatrixFourth, 10, false);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_lightforged_defense_matrix_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_lightforged_defense_matrix_SpellScript();
        }
};

/// Grappling Gun - 195682, 182467
class spell_stormheim_grappling_gun : public SpellScriptLoader
{
    public:
        spell_stormheim_grappling_gun() : SpellScriptLoader("spell_stormheim_grappling_gun") { }

        class spell_stormheim_grappling_gun_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_grappling_gun_SpellScript);

            enum eSpells
            {
                Spell_GrapplingHookVisual = 181178
            };

            enum ePoints
            {
                Point_JumpEnd = 1
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                Position l_DestPos = GetExplTargetDest()->GetPosition();
                if (!l_DestPos)
                    return;

                if (TempSummon* l_Summon = l_Caster->SummonCreature(GetSpellInfo()->Effects[EFFECT_0].MiscValue, l_Caster->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, GetSpellInfo()->GetDuration()))
                {
                    l_Caster->ToPlayer()->AddSpellCooldown(m_scriptSpellId, 0, 3000, true);
                    l_Summon->SetGuidValue(UNIT_FIELD_SUMMONED_BY, l_Caster->GetGUID());
                    l_Summon->CastSpell(l_Summon, eSpells::Spell_GrapplingHookVisual, true);
                    l_Caster->CastSpell(l_Summon, GetSpellInfo()->Effects[EFFECT_1].TriggerSpell, true);
                    l_Summon->GetMotionMaster()->MoveJump(l_DestPos, 20.0f, 15.0f, ePoints::Point_JumpEnd);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_grappling_gun_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_grappling_gun_SpellScript();
        }
};

/// Flames of the Fallen - 188161
class spell_stormheim_flames_of_the_fallen : public SpellScriptLoader
{
    public:
        spell_stormheim_flames_of_the_fallen() : SpellScriptLoader("spell_stormheim_flames_of_the_fallen") { }

        class spell_stormheim_flames_of_the_fallen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_flames_of_the_fallen_AuraScript);

            enum eSpells
            {
                Spell_FlamesOfTheFallenMissileFirst  = 188167,
                Spell_FlamesOfTheFallenMissileSecond = 188170
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                uint16 l_Delay = 300;

                for (uint8 i = 0; i < 5; i++)
                {
                    Position l_DestPos;
                    l_Caster->GetRandomNearPosition(l_DestPos, 15.0f);

                    if (i % 2)
                    {
                        l_Caster->AddDelayedEvent([l_Caster, l_DestPos]() -> void
                        {
                            l_Caster->CastSpell(l_DestPos, eSpells::Spell_FlamesOfTheFallenMissileFirst, true);
                        }, l_Delay);
                    }
                    else
                    {
                        l_Caster->AddDelayedEvent([l_Caster, l_DestPos]() -> void
                        {
                            l_Caster->CastSpell(l_DestPos, eSpells::Spell_FlamesOfTheFallenMissileSecond, true);
                        }, l_Delay);
                    }

                    l_Delay += 300;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_stormheim_flames_of_the_fallen_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_flames_of_the_fallen_AuraScript();
        }
};

/// Runic Bulwark - 209177
class spell_stormheim_runic_bulwark : public SpellScriptLoader
{
    public:
        spell_stormheim_runic_bulwark() : SpellScriptLoader("spell_stormheim_runic_bulwark") { }

        class spell_stormheim_runic_bulwark_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_runic_bulwark_SpellScript);

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target || !l_Target->isInCombat())
                    return;

                l_Caster->ToCreature()->AI()->Talk(1, l_Target->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_runic_bulwark_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_runic_bulwark_SpellScript();
        }
};

/// Sigil of Binding - 209393
class spell_stormheim_sigil_of_binding : public SpellScriptLoader
{
    public:
        spell_stormheim_sigil_of_binding() : SpellScriptLoader("spell_stormheim_sigil_of_binding") { }

        class spell_stormheim_sigil_of_binding_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_sigil_of_binding_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature() || p_Targets.empty())
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (p_Target->ToPlayer())
                    {
                        if (l_Caster->getThreatManager().HaveInThreatList(p_Target->GetGUID()))
                            return false;
                    }

                    return true;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_stormheim_sigil_of_binding_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_stormheim_sigil_of_binding_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_sigil_of_binding_SpellScript();
        }
};

/// Runeseer Confronted - 185032
class spell_stormheim_runeseer_confronted : public SpellScriptLoader
{
    public:
        spell_stormheim_runeseer_confronted() : SpellScriptLoader("spell_stormheim_runeseer_confronted") { }

        class spell_stormheim_runeseer_confronted_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_runeseer_confronted_SpellScript);

            G3D::Vector3 const Path_ToHelheim[3] =
            {
                { 3604.213f, 812.4097f, 46.68877f   },
                { 3596.75f,  820.1371f, 30.99474f   },
                { 3651.616f, 761.8577f, -0.2883554f }
            };

            enum eSpells
            {
                Spell_SigilOfBinding  = 209393,
                Spell_BanishToHelheim = 209241,
                Spell_HelheimTeleport = 185037
            };

            enum ePoints
            {
                Point_FlyToHelheimEnd = 1
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Target->ToPlayer() || !l_Target->HasAura(eSpells::Spell_SigilOfBinding))
                    return;

                l_Target->RemoveAura(eSpells::Spell_SigilOfBinding);
                l_Target->CastSpell(l_Target, eSpells::Spell_BanishToHelheim, true);
                l_Target->SetSpeed(MOVE_RUN, 3.0f);
                l_Target->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToHelheimEnd, Path_ToHelheim, 3);
                l_Target->DelayedCastSpell(l_Target, eSpells::Spell_HelheimTeleport, true, 5000);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_runeseer_confronted_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_KILL_CREDIT2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_runeseer_confronted_SpellScript();
        }
};

/// Detonating - 189134
class spell_stormheim_detonating : public SpellScriptLoader
{
    public:
        spell_stormheim_detonating() : SpellScriptLoader("spell_stormheim_detonating") { }

        class spell_stormheim_detonating_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_detonating_SpellScript);

            std::array<Position, 3> m_FirstVisualPos =
            { {
                { 2428.599f, 1651.379f, -0.0114686f },
                { 2397.684f, 1635.458f, -0.8594689f },
                { 2402.349f, 1661.774f, -0.3142873f }
            } };

            enum eSpells
            {
                Spell_DetonatingVisualFirst  = 189132,
                Spell_DetonatingVisualSecond = 189135
            };

            enum eKillcredits
            {
                Killcredit_ExplosivesDetonated = 95412
            };

            enum eItems
            {
                Item_Detonator = 128287
            };

            enum eNpcs
            {
                Npc_GennGreymane = 95444
            };

            enum ePoints
            {
                Point_GennPathEnd = 1,
                Point_GennJumpEnd = 2
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToPlayer()->DestroyItemCount(eItems::Item_Detonator, 1, false);
                }, 1000);

                for (auto itr : m_FirstVisualPos)
                    l_Caster->CastSpell(itr, eSpells::Spell_DetonatingVisualFirst, true);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    std::array<Position, 5> l_SecondVisualPosFirst =
                    { {
                        { 2418.04f,  1660.72f,  3.56187f  },
                        { 2410.757f, 1651.828f, 3.210817f },
                        { 2418.04f,  1660.72f,  3.56187f  },
                        { 2388.918f, 1641.962f, 15.68268f },
                        { 2418.04f,  1660.72f,  3.56187f  }
                    } };

                    for (auto itr : l_SecondVisualPosFirst)
                        l_Caster->CastSpell(itr, eSpells::Spell_DetonatingVisualSecond, true);
                }, 1000);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    std::array<Position, 5> l_SecondVisualPosSecond =
                    { {
                        { 2388.918f, 1641.962f, 15.68268f },
                        { 2428.387f, 1653.111f, 9.027951f },
                        { 2410.757f, 1651.828f, 3.210817f },
                        { 2425.366f, 1656.45f,  8.685101f },
                        { 2411.224f, 1644.472f, 3.534132f }
                    } };

                    Position const l_GennSpawnPos = { 2381.75f, 1649.818f, 16.77415f, 1.753269f };

                    for (auto itr : l_SecondVisualPosSecond)
                        l_Caster->CastSpell(itr, eSpells::Spell_DetonatingVisualSecond, true);

                    l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_ExplosivesDetonated);

                    if (Creature* l_Creature = l_Caster->SummonCreature(eNpcs::Npc_GennGreymane, l_GennSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID()))
                    {
                        l_Creature->DespawnOrUnsummon(4000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            G3D::Vector3 const Path_Genn[6] =
                            {
                                { 2381.29f, 1648.734f, 17.46899f },
                                { 2380.79f, 1646.234f, 17.71899f },
                                { 2380.04f, 1643.984f, 18.21899f },
                                { 2380.29f, 1642.734f, 17.96899f },
                                { 2380.54f, 1639.484f, 18.21899f },
                                { 2380.33f, 1637.149f, 18.16384f }
                            };

                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_GennPathEnd, Path_Genn, 6, false);
                        }, 1000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            Position const l_GennJumpPos = { 2388.74f, 1614.45f, 0.0489187f };

                            l_Creature->GetMotionMaster()->MoveJump(l_GennJumpPos, 15.0f, 10.0f, ePoints::Point_GennJumpEnd);
                        }, 3000);
                    }

                }, 2000);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_detonating_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_detonating_SpellScript();
        }
};

/// Summon Ashildir - 187829
class spell_stormheim_summon_ashildir : public SpellScriptLoader
{
    public:
        spell_stormheim_summon_ashildir() : SpellScriptLoader("spell_stormheim_summon_ashildir") { }

        class spell_stormheim_summon_ashildir_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_summon_ashildir_AuraScript);

            enum eSpells
            {
                Spell_SummonAshildir = 187830
            };

            Position const m_SpellDestPos = { 2541.743f, 1448.545f, 25.52189f, 1.58825f };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                l_Caster->CastSpell(m_SpellDestPos, eSpells::Spell_SummonAshildir, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_stormheim_summon_ashildir_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_summon_ashildir_AuraScript();
        }
};

/// Narrow Escape - 205552
class spell_stormheim_narrow_escape : public SpellScriptLoader
{
    public:
        spell_stormheim_narrow_escape() : SpellScriptLoader("spell_stormheim_narrow_escape") { }

        class spell_stormheim_narrow_escape_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_narrow_escape_AuraScript);

            enum eSpells
            {
                Spell_NarrowEscapeHorde    = 205551,
                Spell_NarrowEscapeAlliance = 190073
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (l_Caster->ToPlayer()->GetTeam() == HORDE)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_NarrowEscapeHorde, true);
                }
                else
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_NarrowEscapeAlliance, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_stormheim_narrow_escape_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_narrow_escape_AuraScript();
        }
};

/// Open Gateway - 190586
class spell_stormheim_open_gateway : public SpellScriptLoader
{
    public:
        spell_stormheim_open_gateway() : SpellScriptLoader("spell_stormheim_open_gateway") { }

        class spell_stormheim_open_gateway_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_open_gateway_AuraScript);

            enum eSpells
            {
                Spell_OpenGateway    = 190587,
                Spell_GatewayVrykul  = 191223,
                Spell_GatewayReaver  = 191224,
                Spell_GatewayStalker = 190849
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Target->ToCreature())
                    return;

                l_Target->CastSpell(l_Target, eSpells::Spell_OpenGateway, true);

                uint16 l_Delay = 0;

                switch (int32(l_Target->GetPositionX()))
                {
                    case 2444:
                    {
                        for (uint8 i = 0; i < 2; i++)
                        {
                            l_Target->DelayedCastSpell(l_Target, eSpells::Spell_GatewayReaver, true, l_Delay);
                            l_Delay += 100;
                        }

                        l_Delay += 100;
                        l_Target->DelayedCastSpell(l_Target, eSpells::Spell_GatewayVrykul, true, l_Delay);
                        break;
                    }
                    case 2415:
                    {
                        for (uint8 i = 0; i < 5; i++)
                        {
                            l_Target->DelayedCastSpell(l_Target, eSpells::Spell_GatewayStalker, true, l_Delay);
                        }

                        break;
                    }
                    case 2477:
                    {
                        for (uint8 i = 0; i < 2; i++)
                        {
                            l_Target->DelayedCastSpell(l_Target, eSpells::Spell_GatewayReaver, true, l_Delay);
                            l_Delay += 100;
                        }

                        l_Delay += 100;
                        l_Target->DelayedCastSpell(l_Target, eSpells::Spell_GatewayVrykul, true, l_Delay);
                        break;
                    }
                    case 2509:
                    {
                        for (uint8 i = 0; i < 5; i++)
                        {
                            l_Target->DelayedCastSpell(l_Target, eSpells::Spell_GatewayStalker, true, l_Delay);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_stormheim_open_gateway_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_open_gateway_AuraScript();
        }
};

/// Open Gateway - 190587
class spell_stormheim_open_gateway_text : public SpellScriptLoader
{
    public:
        spell_stormheim_open_gateway_text() : SpellScriptLoader("spell_stormheim_open_gateway_text") { }

        class spell_stormheim_open_gateway_text_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_open_gateway_text_SpellScript);

            enum eQuests
            {
                Quest_StormheimsSalvation = 40005
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = GetHitPlayer();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Player ||
                    !l_Player->HasQuest(eQuests::Quest_StormheimsSalvation))
                    return;

                switch (int32(l_Caster->GetPositionX()))
                {
                    case 2444:
                    {
                        l_Caster->ToCreature()->AI()->Talk(0, l_Player->GetGUID(), 1);
                        break;
                    }
                    case 2415:
                    {
                        l_Caster->ToCreature()->AI()->Talk(1, l_Player->GetGUID(), 1);
                        break;
                    }
                    case 2477:
                    {
                        l_Caster->ToCreature()->AI()->Talk(0, l_Player->GetGUID(), 1);
                        break;
                    }
                    case 2509:
                    {
                        l_Caster->ToCreature()->AI()->Talk(1, l_Player->GetGUID(), 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_open_gateway_text_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_open_gateway_text_SpellScript();
        }
};

/// Vethir Land Trigger - 193504
class spell_stormheim_vethir_land_trigger : public SpellScriptLoader
{
    public:
        spell_stormheim_vethir_land_trigger() : SpellScriptLoader("spell_stormheim_vethir_land_trigger") { }

        class spell_stormheim_vethir_land_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_vethir_land_trigger_SpellScript);

            enum eKillcredits
            {
                Killcredit_VethirLanded = 103766
            };

            enum eNpcs
            {
                Npc_Vethir = 97986
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (p_Target->ToCreature())
                    {
                        if (p_Target->GetEntry() == eNpcs::Npc_Vethir)
                            return false;
                    }

                    return true;
                });

                if (p_Targets.empty())
                    l_Caster->ToPlayer()->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_VethirLanded);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_stormheim_vethir_land_trigger_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_vethir_land_trigger_SpellScript();
        }
};

/// Royal Summons - 191405
class spell_stormheim_royal_summons_alliance : public SpellScriptLoader
{
    public:
        spell_stormheim_royal_summons_alliance() : SpellScriptLoader("spell_stormheim_royal_summons_alliance") { }

        class spell_stormheim_royal_summons_alliance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_royal_summons_alliance_SpellScript);

            enum eNpcs
            {
                Npc_SkyAdmiralRogers = 96644
            };

            enum eObjectives
            {
                Obj_RoyalSummonsRead = 279833
            };

            enum eActions
            {
                Action_RogersDalaranScene = 1
            };

            Position const m_SkyAdmiralRogersSpawnPos = { -840.5087f, 4633.81f, 749.4966f, 4.168828f };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer() ||
                    l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_RoyalSummonsRead) != 0)
                    return;

                if (Creature* l_Creature = l_Caster->SummonCreature(eNpcs::Npc_SkyAdmiralRogers, m_SkyAdmiralRogersSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID()))
                {
                    l_Creature->AI()->DoAction(eActions::Action_RogersDalaranScene);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_stormheim_royal_summons_alliance_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_royal_summons_alliance_SpellScript();
        }
};

/// Salute - 220525
class spell_stormheim_salute_alliance : public SpellScriptLoader
{
    public:
        spell_stormheim_salute_alliance() : SpellScriptLoader("spell_stormheim_salute_alliance") { }

        class spell_stormheim_salute_alliance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_salute_alliance_SpellScript);

            enum eNpcs
            {
                Npc_LegionDragoon = 92670
            };

            enum eSpells
            {
                Spell_DummyAura = 94562
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                std::list<Creature*> l_CreatureList;
                l_Caster->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_LegionDragoon, 50.0f);

                if (l_CreatureList.empty())
                    return;

                for (auto itr : l_CreatureList)
                {
                    itr->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                }

                l_Caster->RemoveAura(eSpells::Spell_DummyAura);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_stormheim_salute_alliance_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_salute_alliance_SpellScript();
        }
};

/// Inspecting - 220521
class spell_stormheim_inspecting : public SpellScriptLoader
{
    public:
        spell_stormheim_inspecting() : SpellScriptLoader("spell_stormheim_inspecting") { }

        class spell_stormheim_inspecting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_inspecting_SpellScript);

            enum eKillcredits
            {
                Killcredit_InspectDeckGuns = 96662
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_Target)
                    return;

                l_Target->PlayOneShotAnimKitId(2206);
                l_Target->PlayDistanceSound(l_Target, 12243, l_Target);
                l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_InspectDeckGuns);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_inspecting_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_inspecting_SpellScript();
        }
};

/// Plagued Spit - 183146
class spell_stormheim_plagued_spit : public SpellScriptLoader
{
    public:
        spell_stormheim_plagued_spit() : SpellScriptLoader("spell_stormheim_plagued_spit") { }

        class spell_stormheim_plagued_spit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_plagued_spit_SpellScript);

            enum eSpells
            {
                Spell_PlaguedSpit = 183151
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::Spell_PlaguedSpit, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_plagued_spit_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_plagued_spit_SpellScript();
        }
};

/// Transponder Pack - 180645
class spell_stormheim_transponder_pack : public SpellScriptLoader
{
    public:
        spell_stormheim_transponder_pack() : SpellScriptLoader("spell_stormheim_transponder_pack") { }

        class spell_stormheim_transponder_pack_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_transponder_pack_AuraScript);

            enum eObjectives
            {
                Obj_ChargeLevel = 277457
            };

            enum eSpells
            {
                Spell_ChargedTransponderPack = 180860
            };

            enum eKillcredits
            {
                Killcredit_TransponderCharged = 90866
            };

            void HandleOnEffectPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_ChargeLevel) < 100)
                {
                    l_Caster->SetMaxPower(Powers::POWER_ALTERNATE_POWER, 100);
                    l_Caster->SetPower(Powers::POWER_ALTERNATE_POWER, l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_ChargeLevel));
                }
                else
                {
                    l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_TransponderCharged);
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_ChargedTransponderPack, true);
                    Remove();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_stormheim_transponder_pack_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_transponder_pack_AuraScript();
        }
};

/// Lingering Hunger - 215041
class spell_stormheim_lingering_hunger : public SpellScriptLoader
{
    public:
        spell_stormheim_lingering_hunger() : SpellScriptLoader("spell_stormheim_lingering_hunger") { }

        class  spell_stormheim_lingering_hunger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_lingering_hunger_AuraScript);

            enum eSpells
            {
                Spell_LingeringHungerDamage = 215063
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_LingeringHungerDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_stormheim_lingering_hunger_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_lingering_hunger_AuraScript();
        }
};

/// Consume the Living - 214999
class spell_stormheim_consume_the_living : public SpellScriptLoader
{
    public:
        spell_stormheim_consume_the_living() : SpellScriptLoader("spell_stormheim_consume_the_living") { }

        class spell_stormheim_consume_the_living_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormheim_consume_the_living_SpellScript);

            enum eSpells
            {
                Spell_ConsumeTheLivingChannel = 214993,
                Spell_ConsumeTheLivingHeal    = 215000
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target || !l_Target->ToCreature())
                    return;

                std::list<Player*> l_PlayerList;
                l_Caster->GetPlayerListInGrid(l_PlayerList, 50.0f);

                if (l_PlayerList.empty())
                    return;

                for (auto itr : l_PlayerList)
                {
                    l_Caster->ToCreature()->AI()->Talk(3, itr->GetGUID());
                }

                l_Caster->ToCreature()->AI()->Talk(2);
                l_Target->SetFacingToObject(l_Caster);
                l_Target->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                l_Target->DisableHealthRegen();
                l_Target->SetHealth(200000);
                l_Target->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->CastSpell(l_Target, eSpells::Spell_ConsumeTheLivingHeal, false);
                l_Target->CastSpell(l_Caster, eSpells::Spell_ConsumeTheLivingChannel, true);

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stormheim_consume_the_living_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stormheim_consume_the_living_SpellScript();
        }
};

/// Consume the Living - 214993
class spell_stormheim_consume_the_living_channel : public SpellScriptLoader
{
    public:
        spell_stormheim_consume_the_living_channel() : SpellScriptLoader("spell_stormheim_consume_the_living_channel") { }

        class spell_stormheim_consume_the_living_channel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_stormheim_consume_the_living_channel_AuraScript);

            enum eSpells
            {
                Spell_ConsumeTheLivingVisual = 215018
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                {
                    l_Caster->CastSpell(l_Target, eSpells::Spell_ConsumeTheLivingVisual, true);
                    l_Target->Kill(l_Caster);
                }
                else if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_CANCEL)
                {
                    l_Target->CastStop();
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_stormheim_consume_the_living_channel_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_stormheim_consume_the_living_channel_AuraScript();
        }
};

class playerscript_stormheim_scripts : public PlayerScript
{
    public:
        playerscript_stormheim_scripts() : PlayerScript("playerscript_stormheim_scripts") {}

        enum eSpells
        {
            Spell_ContainmentUnit                   = 181970,
            Spell_ContainmentUnitVisual             = 182151,
            Spell_SummonScout                       = 183448,
            Spell_SummonMysticSpirit                = 185063,
            Spell_SummonDragonRiderSpirit           = 185064,
            Spell_SummonWarriorSpirit               = 185060,
            Spell_CancelMaintainSummonAshildir      = 192518,
            Spell_CancelMaintainSummonAshildirFinal = 192684,
            Spell_NarrowEscapeHordeTeleport         = 205553,
            Spell_ConversationLastPortalDestroyed   = 218267,
            Spell_TeleportForsakenFleetWreck        = 185615,
            Spell_HaviConversation                  = 192170,
            Spell_QueueForMawOfNashal               = 228712,
            Spell_TeleportWreckOfTheSkyfire         = 185613,
            Spell_TransponderPack                   = 180645,
            Spell_ChargedTransponderPack            = 180860,
            Spell_SummonScoutLorna                  = 181686,
            Spell_CancelFragmentOfWill              = 192509,
            Spell_CancelFragmentOfMight             = 192510,
            Spell_CancelFragmentOfValor             = 192512,
            Spell_NarrowEscapeAllianceTeleport      = 190067
        };

        enum eQuests
        {
            Quest_PumpItUp                        = 38358,
            Quest_TheWindrunnersFate              = 38360,
            Quest_AboveTheWinterMoonlightHorde    = 38618,
            Quest_HeartOfADragon                  = 38414,
            Quest_StealthBySeaweed                = 38347,
            Quest_ADesperateBargain               = 39848,
            Quest_VictoryIsEternal                = 39853,
            Quest_PaidInLifeblood                 = 39855,
            Quest_AGiftForGreymane                = 39385,
            Quest_StoriesOfBattle                 = 39405,
            Quest_StemTheTide                     = 40003,
            Quest_TheTrialsContinue               = 39803,
            Quest_GreymanesGambit                 = 39800,
            Quest_LightningRod                    = 38058,
            Quest_SignalBoost                     = 38060,
            Quest_AboveTheWinterMoonlightAlliance = 38412,
            Quest_Sundered                        = 39850,
            Quest_BloodAndGold                    = 39595,
            Quest_StormheimHorde                  = 39864,
            Quest_StormheimAlliance               = 39735
        };

        enum eActions
        {
            Action_NathanosMoveToPlagueTippedArrow = 1
        };

        enum eNpcs
        {
            Npc_NathanosBlightcaller = 91158,
            Npc_Thrymjaris           = 91570,
            Npc_ColbornTheUnworthy   = 91531,
            Npc_Muninn               = 97306
        };

        enum eObjectives
        {
            Obj_JudgedByRythasTheOracle      = 278465,
            Obj_JudgedByHeimirOfTheBlackFist = 278464,
            Obj_JudgedByOktelDragonblood     = 278466,
            Obj_ChargeLevel                  = 277457
        };

        enum eKillcredits
        {
            Killcredit_SylvanasMissionComplete = 113320,
            Killcredit_TravelToDalaran         = 97377
        };

        enum eItems
        {
            Item_Detonator = 128287,
            Item_ChallengersTribute = 128511
        };

        enum eMovies
        {
            Movie_SylvanasAndGennFightHorde    = 495,
            Movie_StormheimScenarioStart       = 483,
            Movie_SylvanasAndGennFightAlliance = 494
        };

        Position const m_NathanosNearPlagueTippedArrowSpawnPos = { 4464.76f, 2503.34f, 1.77254f, 0.3432975f };
        Position const m_ThrymjarisSpawnPos = { 2026.872f, 2629.76f, 467.3713f, 5.764378f };

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/) override
        {
            if (p_NewZoneID == 7502)
            {
                if (p_Player->HasQuest(eQuests::Quest_StormheimHorde) || p_Player->HasQuest(eQuests::Quest_StormheimAlliance))
                {
                    p_Player->KilledMonsterCredit(eKillcredits::Killcredit_TravelToDalaran);
                }
            }
        }

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 /*p_OldValue*/, int32& p_NewValue, bool /*p_Regen*/, bool /*p_After*/) override
        {
            if (p_Power == Powers::POWER_ALTERNATE_POWER && p_Player->HasQuest(eQuests::Quest_LightningRod))
            {
                if (p_NewValue)
                {
                    p_Player->SetQuestObjectiveCounter(eObjectives::Obj_ChargeLevel, p_NewValue);
                }
            }
        }

        void OnObjectiveValidate(Player* p_Player, uint32 /*p_QuestID*/, uint32 p_ObjectiveID) override
        {
            switch (p_ObjectiveID)
            {
                case eObjectives::Obj_JudgedByRythasTheOracle:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_SummonMysticSpirit, true);
                    break;
                }
                case eObjectives::Obj_JudgedByOktelDragonblood:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_SummonDragonRiderSpirit, true);
                    break;
                }
                case eObjectives::Obj_JudgedByHeimirOfTheBlackFist:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_SummonWarriorSpirit, true);
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestAccept(Player * p_Player, const Quest * p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::Quest_PumpItUp:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_ContainmentUnit, true);
                    break;
                }
                case eQuests::Quest_TheTrialsContinue:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_HaviConversation, true);
                    break;
                }
                case eQuests::Quest_GreymanesGambit:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_QueueForMawOfNashal, true);
                    break;
                }
                case eQuests::Quest_LightningRod:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_TransponderPack, true);
                    break;
                }
                case eQuests::Quest_BloodAndGold:
                {
                    p_Player->AddItem(eItems::Item_ChallengersTribute, 1);
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
                case eQuests::Quest_PumpItUp:
                {
                    p_Player->RemoveAura(eSpells::Spell_ContainmentUnit);
                    break;
                }
                case eQuests::Quest_StealthBySeaweed:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelMaintainSummonAshildir, true);
                    break;
                }
                case eQuests::Quest_VictoryIsEternal:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelMaintainSummonAshildirFinal, true);
                    break;
                }
                case eQuests::Quest_AGiftForGreymane:
                {
                    p_Player->DestroyItemCount(eItems::Item_Detonator, 1, false);
                    break;
                }
                case eQuests::Quest_LightningRod:
                {
                    p_Player->RemoveAura(eSpells::Spell_TransponderPack);
                    break;
                }
                case eQuests::Quest_Sundered:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelFragmentOfWill, true);
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelFragmentOfMight, true);
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelFragmentOfValor, true);
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestComplete(Player* p_Player, const Quest* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::Quest_PumpItUp:
                {
                    p_Player->RemoveAura(eSpells::Spell_ContainmentUnit);
                    p_Player->CastSpell(p_Player, eSpells::Spell_ContainmentUnitVisual, true);
                    break;
                }
                case eQuests::Quest_TheWindrunnersFate:
                {
                    if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_NathanosBlightcaller, m_NathanosNearPlagueTippedArrowSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_NathanosMoveToPlagueTippedArrow);
                    }

                    break;
                }
                case eQuests::Quest_HeartOfADragon:
                {
                    p_Player->SummonCreature(eNpcs::Npc_Thrymjaris, m_ThrymjarisSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
                    break;
                }
                case eQuests::Quest_StoriesOfBattle:
                {
                    p_Player->RewardQuest(p_Quest, 0, nullptr, true);
                    break;
                }
                case eQuests::Quest_StemTheTide:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_ConversationLastPortalDestroyed, true);
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestReward(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::Quest_PumpItUp:
                {
                    p_Player->RemoveAura(eSpells::Spell_ContainmentUnitVisual);
                    break;
                }
                case eQuests::Quest_AboveTheWinterMoonlightHorde:
                {
                    p_Player->RemoveAura(eSpells::Spell_SummonScout);
                    break;
                }
                case eQuests::Quest_ADesperateBargain:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelMaintainSummonAshildir, true);
                    break;
                }
                case eQuests::Quest_PaidInLifeblood:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_CancelMaintainSummonAshildirFinal, true);
                    break;
                }
                case eQuests::Quest_LightningRod:
                {
                    if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(eQuests::Quest_SignalBoost))
                    {
                        p_Player->AddQuest(l_Quest, nullptr);
                    }

                    break;
                }
                case eQuests::Quest_SignalBoost:
                {
                    if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_Muninn, 50.0f, true))
                    {
                        l_Creature->AI()->Talk(0, p_Player->GetGUID());
                    }

                    p_Player->RemoveAura(eSpells::Spell_ChargedTransponderPack);
                    break;
                }
                case eQuests::Quest_AboveTheWinterMoonlightAlliance:
                {
                    p_Player->RemoveAura(eSpells::Spell_SummonScoutLorna);
                    break;
                }
                default:
                    break;
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasAura(eSpells::Spell_SummonScout) && p_Player->GetAreaId() == 7608 &&
                !p_Player->HasQuest(eQuests::Quest_AboveTheWinterMoonlightHorde))
            {
                p_Player->RemoveAura(eSpells::Spell_SummonScout);
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_SummonScout, true, 5000);
            }

            if (p_Player->HasAura(eSpells::Spell_SummonScoutLorna) && p_Player->GetAreaId() == 7608 &&
                !p_Player->HasQuest(eQuests::Quest_AboveTheWinterMoonlightAlliance))
            {
                p_Player->RemoveAura(eSpells::Spell_SummonScoutLorna);
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_SummonScoutLorna, true, 5000);
            }
        }

        void OnMovieComplete(Player* p_Player, uint32 p_MovieId) override
        {
            switch (p_MovieId)
            {
                case eMovies::Movie_SylvanasAndGennFightHorde:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_NarrowEscapeHordeTeleport, true);
                    break;
                }
                case eMovies::Movie_StormheimScenarioStart:
                {
                    if (p_Player->GetTeam() == HORDE)
                    {
                        p_Player->CastSpell(p_Player, eSpells::Spell_TeleportForsakenFleetWreck, true);
                        p_Player->KilledMonsterCredit(eKillcredits::Killcredit_SylvanasMissionComplete);
                    }
                    else
                    {
                        p_Player->CastSpell(p_Player, eSpells::Spell_TeleportWreckOfTheSkyfire, true);
                        p_Player->KilledMonsterCredit(eKillcredits::Killcredit_SylvanasMissionComplete);
                    }

                    break;
                }
                case eMovies::Movie_SylvanasAndGennFightAlliance:
                {
                    p_Player->CastSpell(p_Player, eSpells::Spell_NarrowEscapeAllianceTeleport, true);
                    break;
                }
                default:
                    break;
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_zone_stormheim()
{
    new npc_stormheim_eyir();
    new npc_stormheim_sigryn();
    new npc_nathanos_blightcaller_96683();
    new npc_lady_sylvanas_windrunner_96686();
    new npc_dread_rider_cullen_91473();
    new npc_nathanos_blightcaller_91158();
    new npc_thane_wildsky_91571();
    new npc_huginn_97305();
    new npc_forsaken_bat_rider_97250();
    new npc_havi_96254();
    new npc_stormheim_ingredients();
    new npc_skovald_event_trigger_3959000();
    new npc_god_king_skovald_96314();
    new npc_felskorn_torturer_96121();
    new npc_trial_of_valor_creatures();
    new npc_felskorn_raider_96129();
    new npc_felskorn_warmonger_108263();
    new npc_trial_of_will_creatures();
    new npc_yotnar_96175();
    new npc_yotnar_96258();
    new npc_dread_rider_cullen_92561();
    new npc_dread_rider_cullen_92566();
    new npc_grappling_hook_bunny_108402();
    new npc_grappling_hook_bunny_91971();
    new npc_dread_rider_cullen_92567();
    new npc_dread_rider_cullen_92573();
    new npc_dread_rider_cullen_92569();
    new npc_vaengir_91555();
    new npc_thrymjaris_91570();
    new npc_vethir_96465();
    new npc_thrymjaris_97061();
    new npc_vethir_92302();
    new npc_havi_92539();
    new npc_vydhar_93231();
    new npc_stormheim_runestones();
    new npc_skovald_event_trigger_3881600();
    new npc_bone_effect_bunny_97281();
    new npc_runeseer_faljar_93093();
    new npc_ashildir_97319();
    new npc_colborn_the_unworthy_91531();
    new npc_ashildir_97419();
    new npc_lady_sylvanas_windrunner_97695();
    new npc_ashildir_97480();
    new npc_drowning_valkyra_97469();
    new npc_stormheim_fragments();
    new npc_ashildir_97558();
    new npc_valkyra_guardian_97665();
    new npc_ashildir_levitate_bunny_97670();
    new npc_player_levitate_bunny_97669();
    new npc_forsaken_catapult_95212();
    new npc_rowboat_to_skold_ashil_109454();
    new npc_lady_sylvanas_windrunner_94227();
    new npc_player_levitate_bunny_94769();
    new npc_ashildir_94764();
    new npc_lady_sylvanas_windrunner_94228();
    new npc_stormheim_racial_final_quest();
    new npc_vethir_97979();
    new npc_stormheim_portals_bunny();
    new npc_stormheim_destroy_dragons();
    new npc_vethir_97986();
    new npc_god_king_skovald_92307();
    new npc_vethir_98190();
    new npc_havi_97988();
    new npc_grappling_hook_bunny_92315();
    new npc_skovald_scene_trigger_3861500();
    new npc_sky_admiral_rogers_96644();
    new npc_dread_rider_malwick_90902();
    new npc_riding_bat_92407();
    new npc_legion_dragoon_90948();
    new npc_forsaken_dreadwing_92516();
    new npc_mind_controlled_dreadwing_92547();
    new npc_vethir_90907();
    new npc_muninn_97306();
    new npc_commander_lorna_crowley_91222();
    new npc_commander_lorna_crowley_91481();
    new npc_commander_lorna_crowley_91519();
    new npc_commander_lorna_crowley_91553();
    new npc_commander_lorna_crowley_91558();
    new npc_combustible_contagion_bunnys();
    new npc_genn_greymane_95804();
    new npc_greywatch_guard_95859();
    new npc_blood_thane_lucard_107588();
    new npc_restless_ancestor_93094();
    new gob_unpowered_console_243808();
    new gob_unpowered_console_243814();
    new gob_powered_console_243817();
    new gob_stormheim_ritual_circles();
    new gob_stormheim_offering_bowls();
    new gob_stormheim_ritual_stones();
    new gob_helyas_altar_244559();
    new gob_plant_explosives_243455();
    new gob_stormheim_engraved_shield();
    new gob_skyfire_propeller_240235();
    new at_stormheim_salute_horde();
    new at_stormheim_attacker_grapple();
    new at_stormheim_salute_alliance();
    new at_stormheim_static_traps();
    new at_stormheim_distress_signal();
    new at_stormheim_blood_ritual();
    new spell_stormheim_royal_summons_horde();
    new spell_stormheim_salute_horde();
    new spell_stormheim_pump();
    new spell_stormheim_containment_unit();
    new spell_stormheim_untie();
    new spell_stormheim_plague_arrow_warning();
    new spell_stormheim_investigating();
    new spell_stormheim_fiery_blink();
    new spell_stormheim_burning_glare();
    new spell_stormheim_purifier_blast();
    new spell_stormheim_lightforged_defense_matrix();
    new spell_stormheim_grappling_gun();
    new spell_stormheim_flames_of_the_fallen();
    new spell_stormheim_runic_bulwark();
    new spell_stormheim_sigil_of_binding();
    new spell_stormheim_runeseer_confronted();
    new spell_stormheim_detonating();
    new spell_stormheim_summon_ashildir();
    new spell_stormheim_narrow_escape();
    new spell_stormheim_open_gateway();
    new spell_stormheim_open_gateway_text();
    new spell_stormheim_vethir_land_trigger();
    new spell_stormheim_royal_summons_alliance();
    new spell_stormheim_salute_alliance();
    new spell_stormheim_inspecting();
    new spell_stormheim_plagued_spit();
    new spell_stormheim_transponder_pack();
    new spell_stormheim_lingering_hunger();
    new spell_stormheim_consume_the_living();
    new spell_stormheim_consume_the_living_channel();
    new playerscript_stormheim_scripts();
}
#endif
