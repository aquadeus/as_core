////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2018 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "zone_kezan.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Vehicle.h"

/// Hot Rod - 34840
class npc_hot_rod_34840 : public CreatureScript
{
    public:
        npc_hot_rod_34840() : CreatureScript("npc_hot_rod_34840") { }

        struct npc_hot_rod_34840AI : public VehicleAI
        {
            npc_hot_rod_34840AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();

                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetQuestStatus(eQuests::RollingWithMyHomies) == QUEST_STATUS_INCOMPLETE)
                    p_Summoner->AddAura(eSpells::SeeQuestInv, p_Summoner);
                else
                    p_Summoner->RemoveAura(eSpells::SeeQuestInv);

                m_SummonerGuid = p_Summoner->GetObjectGuid();
                p_Summoner->AddAura(eSpells::SeeQuestInv, p_Summoner);
            }

            void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (!p_Apply && !p_Who->IsPlayer())
                {
                    switch (p_Who->GetEntry())
                    {
                        case eCreatures::Ace:
                            p_Who->GetMotionMaster()->MoveFollow(l_Player, 2.0f, M_PI);
                            break;
                        case eCreatures::Izzy:
                            p_Who->GetMotionMaster()->MoveFollow(l_Player, 2.0f, 5 * M_PI_4);
                            break;
                        case eCreatures::Gobber:
                            p_Who->GetMotionMaster()->MoveFollow(l_Player, 2.0f, 3 * M_PI_4);
                            break;
                        default:
                            break;
                    }
                }

                if (p_Apply && p_Who->IsPlayer() && l_Player->IsQuestRewarded(eQuests::RollingWithMyHomies))
                {
                    GuidList* l_SummonListAce = l_Player->GetSummonList(eCreatures::Ace);
                    GuidList* l_SummonListIzzy = l_Player->GetSummonList(eCreatures::Izzy);
                    GuidList* l_SummonListGobber = l_Player->GetSummonList(eCreatures::Gobber);

                    if (l_SummonListAce)
                    {
                        for (auto const& l_Guid : *l_SummonListAce)
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, l_Guid))
                                l_Creature->DespawnOrUnsummon(500);
                    }

                    if (l_SummonListIzzy)
                    {
                        for (auto const& l_Guid : *l_SummonListIzzy)
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, l_Guid))
                                l_Creature->DespawnOrUnsummon(500);
                    }

                    if (l_SummonListGobber)
                    {
                        for (auto const& l_Guid : *l_SummonListGobber)
                            if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, l_Guid))
                                l_Creature->DespawnOrUnsummon(500);
                    }

                    l_Player->CastSpell(l_Player, eSpells::ResummonAce, true);
                    l_Player->CastSpell(l_Player, eSpells::ResummonGobber, true);
                    l_Player->CastSpell(l_Player, eSpells::ResummonIzzy, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hot_rod_34840AI(p_Creature);
        }
};

/// Izzy 34959
class npc_izzy_34959 : public CreatureScript
{
    public:
        npc_izzy_34959() : CreatureScript("npc_izzy_34959") { }

        struct npc_izzy_34959AI : public ScriptedAI
        {
            npc_izzy_34959AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                if (Vehicle* l_Vehicle = p_Summoner->GetVehicle())
                    if (Unit* l_Base = l_Vehicle->GetBase())
                        me->EnterVehicle(l_Base);

                if (me->isSummon())
                    me->RemoveAura(eSpells::QuestInv);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player || me->GetNameTag() != "Rolling")
                    return;

                if (me->GetExactDist2d(p_Who) <= 5.0f && me->GetEntry() == eCreatures::Izzy && !l_Player->GetQuestObjectiveCounter(eObjectives::PickIzzy))
                {
                    l_Player->CastSpell(l_Player, eSpells::SummonIzzy, true);
                    l_Player->KilledMonsterCredit(eCreatures::Izzy);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_izzy_34959AI(p_Creature);
        }
};

/// Gobber 34958
class npc_gobber_34958 : public CreatureScript
{
    public:
        npc_gobber_34958() : CreatureScript("npc_gobber_34958") { }

        struct npc_gobber_34958AI : public ScriptedAI
        {
            npc_gobber_34958AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                if (Vehicle* l_Vehicle = p_Summoner->GetVehicle())
                    if (Unit* l_Base = l_Vehicle->GetBase())
                    {
                        me->EnterVehicle(l_Base);
                        me->RemoveAura(eSpells::QuestInv);
                    }

                if (me->isSummon())
                    me->RemoveAura(eSpells::QuestInv);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player || me->GetNameTag() != "Rolling")
                    return;

                if (me->GetExactDist2d(p_Who) <= 5.0f && me->GetEntry() == eCreatures::Gobber && !l_Player->GetQuestObjectiveCounter(eObjectives::PickGobber))
                {
                    l_Player->CastSpell(l_Player, eSpells::SummonGobber, true);
                    l_Player->KilledMonsterCredit(eCreatures::Gobber);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gobber_34958AI(p_Creature);
        }
};

/// Ace 34957
class npc_ace_34957 : public CreatureScript
{
    public:
        npc_ace_34957() : CreatureScript("npc_ace_34957") { }

        struct npc_ace_34957AI : public ScriptedAI
        {
            npc_ace_34957AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                if (Vehicle* l_Vehicle = p_Summoner->GetVehicle())
                    if (Unit* l_Base = l_Vehicle->GetBase())
                    {
                        me->EnterVehicle(l_Base);
                        me->RemoveAura(eSpells::QuestInv);
                    }

                if (me->isSummon())
                    me->RemoveAura(eSpells::QuestInv);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player || me->GetNameTag() != "Rolling")
                    return;

                if (me->GetExactDist2d(p_Who) <= 5.0f && me->GetEntry() == eCreatures::Ace && !l_Player->GetQuestObjectiveCounter(eObjectives::PickAce))
                {
                    l_Player->CastSpell(l_Player, eSpells::SummonAce, true);
                    l_Player->KilledMonsterCredit(eCreatures::Ace);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ace_34957AI(p_Creature);
        }
};

/// Bilgewater Buccaneer - 37179
class npc_bilgewater_buccaneer_37179 : public CreatureScript
{
    public:
        npc_bilgewater_buccaneer_37179() : CreatureScript("npc_bilgewater_buccaneer_37179") { }

        struct npc_bilgewater_buccaneer_37179AI : public VehicleAI
        {
            npc_bilgewater_buccaneer_37179AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            ObjectGuid m_SummonerGuid;

            std::vector < std::pair < uint32, Position>> const m_Sharks =
            {
                {eSpells::SummonSteamwheedleShark000, Positions::g_SteamwheedleShark000},
                {eSpells::SummonSteamwheedleShark001, Positions::g_SteamwheedleShark001},
                {eSpells::SummonSteamwheedleShark002, Positions::g_SteamwheedleShark002},
                {eSpells::SummonSteamwheedleShark003, Positions::g_SteamwheedleShark003},
                {eSpells::SummonSteamwheedleShark004, Positions::g_SteamwheedleShark004},
                {eSpells::SummonSteamwheedleShark005, Positions::g_SteamwheedleShark005},
                {eSpells::SummonSteamwheedleShark006, Positions::g_SteamwheedleShark006},
                {eSpells::SummonSteamwheedleShark007, Positions::g_SteamwheedleShark007}
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
            }

            void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (p_Apply && p_Who->IsPlayer())
                {
                    PersonalTalk(0, m_SummonerGuid);
                    for (auto l_Pair : m_Sharks)
                        l_Player->CastSpell(l_Pair.second, l_Pair.first, true);
                }

                if (!p_Apply)
                    l_Player->CastSpell(l_Player, eSpells::DespawnSummonedSteamwheedleSharks, true);

                if (l_Player->GetQuestStatus(eQuests::NecessaryRoughness) == QUEST_STATUS_COMPLETE)
                    l_Player->ExitVehicle();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_bilgewater_buccaneer_37179AI(p_Creature);
        }
};

/// Steamwheedle Shark  37114
class npc_steamwheedle_shark_37114 : public CreatureScript
{
    public:
        npc_steamwheedle_shark_37114() : CreatureScript("npc_steamwheedle_shark_37114") { }

        struct npc_steamwheedle_shark_37114AI : public ScriptedAI
        {
            npc_steamwheedle_shark_37114AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                BilgewaterBuccaneer = 1
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();

                Position l_SummonerPos = p_Summoner->GetPosition();
                Position l_MovePos = me->GetPosition();

                l_MovePos.SimplePosXYRelocationByAngle(l_MovePos, me->GetDistance2d(l_SummonerPos.m_positionX, l_SummonerPos.m_positionY) - 0.5f, me->GetAngle(&l_SummonerPos), true);

                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(ePoints::BilgewaterBuccaneer, l_MovePos, false);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || p_Spell->Id != eSpells::ThrowFootbomb)
                    return;

                me->DespawnOrUnsummon();
                l_Player->KilledMonsterCredit(me->GetEntry());
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_steamwheedle_shark_37114AI(p_Creature);
        }
};

/// Bilgewater Buccaneer - 37213
class npc_bilgewater_buccaneer_37213 : public CreatureScript
{
    public:
        npc_bilgewater_buccaneer_37213() : CreatureScript("npc_bilgewater_buccaneer_37213") { }

        struct npc_bilgewater_buccaneer_37213AI : public VehicleAI
        {
            npc_bilgewater_buccaneer_37213AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            void PassengerBoarded(Unit* p_Who, int8 p_SeatId, bool p_Apply) override
            {
                if (p_Apply && p_Who->IsPlayer())
                    if (Creature* l_Creature = p_Who->SummonCreature(eCreatures::FourthAndGoalTarget, Positions::g_FourthAndGoalTarget, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Who->GetGUID()))
                        l_Creature->CastSpell(l_Creature, eSpells::FourthAndGoalTargetAndInvi, true);

                me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                if (p_Apply && p_Who->IsPlayer() && p_SeatId == 0)
                {
                    if (Creature * l_Creature = p_Who->SummonCreature(eCreatures::FourthAndGoalTarget, Positions::g_FourthAndGoalTarget, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Who->GetObjectGuid()))
                    {
                        l_Creature->CastSpell(l_Creature, 115431, true);
                        l_Creature->SetPhaseMask(2, true);
                    }

                    p_Who->AddAura(eSpells::SeeQuestInv15, p_Who);
                }

                p_Who->AddDelayedEvent([p_Who]() -> void
                {
                    p_Who->ChangeSeat(1);
                }, 500);

                if (!p_Apply && p_SeatId == 1)
                    me->DespawnOrUnsummon();

                if (p_Apply)
                    p_Who->SetPhaseMask(2, true);
                else
                    p_Who->SetPhaseMask(1, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_bilgewater_buccaneer_37213AI(p_Creature);
        }
};

/// Deathwing 48572
class npc_deathwing_48572 : public CreatureScript
{
    public:
        npc_deathwing_48572() : CreatureScript("npc_deathwing_48572") { }

        struct npc_deathwing_48572AI : public ScriptedAI
        {
            npc_deathwing_48572AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Path = 1
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Path, Waypoints::Path_Deathwing, 14);
                AddTimedDelayedOperation(3000, [this]()->void
                {
                    Talk(0);
                });
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                    case ePoints::Path:
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
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_deathwing_48572AI(p_Creature);
        }
};

/// Kezan Partygoer 35186
class npc_kezan_partygoer_35186 : public CreatureScript
{
    public:
        npc_kezan_partygoer_35186() : CreatureScript("npc_kezan_partygoer_35186") { }

        struct npc_kezan_partygoer_35186AI : public ScriptedAI
        {
            npc_kezan_partygoer_35186AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eKillcredits
            {
                PartygoearEntertained = 35175
            };

            enum eEvents
            {
                SayText = 1,
                ResetEvent = 2
            };

            ObjectGuid m_SummonerGuid;
            EventMap m_Events;
            std::vector<uint32> l_NeedSpells =
            {
                eSpells::NeedBubbly,
                eSpells::NeedBucket,
                eSpells::NeedDance,
                eSpells::NeedFireworks,
                eSpells::NeedHorsDoeuvres
            };

            void Reset() override
            {
                me->CastSpell(me, l_NeedSpells[urand(0, 4)], true);
                m_Events.Reset();
                m_Events.ScheduleEvent(eEvents::SayText, urand(10, 30) * IN_MILLISECONDS);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player || me->HasAura(eSpells::HappyPartyGoer))
                    return;

                switch (p_Spell->Id)
                {
                    case eSpells::Bubbly:
                    {
                        if (!me->HasAura(eSpells::NeedBubbly))
                            return;

                        Talk(1);
                        break;
                    }
                    case eSpells::Bucket:
                    {
                        if (!me->HasAura(eSpells::NeedBucket))
                            return;

                        Talk(10);
                        break;
                    }
                    case eSpells::Dance:
                    {
                        if (!me->HasAura(eSpells::NeedDance))
                            return;

                        Talk(0);
                        break;
                    }
                    case eSpells::Fireworks:
                    {
                        if (!me->HasAura(eSpells::NeedFireworks))
                            return;

                        Talk(9);
                        break;
                    }
                    case eSpells::HorsDoeuvres:
                    {
                        if (!me->HasAura(eSpells::NeedHorsDoeuvres))
                            return;

                        Talk(11);
                        break;
                    }
                    default:
                        return;
                }

                me->RemoveAllAuras();
                l_Player->KilledMonsterCredit(eKillcredits::PartygoearEntertained);
                me->CastSpell(me, eSpells::HappyPartyGoer, true);
                m_Events.ScheduleEvent(eEvents::ResetEvent, 60 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);
                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::SayText:
                    {
                        if (me->HasAura(eSpells::NeedFireworks))
                            Talk(6);
                        else if (me->HasAura(eSpells::NeedHorsDoeuvres))
                            Talk(3);
                        else if (me->HasAura(eSpells::NeedBubbly))
                            Talk(5);
                        else if (me->HasAura(eSpells::NeedDance))
                            Talk(7);
                        else if (me->HasAura(eSpells::NeedBucket))
                            Talk(8);

                        m_Events.ScheduleEvent(eEvents::SayText, urand(10, 30) * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::ResetEvent:
                    {
                        me->RemoveAura(eSpells::HappyPartyGoer);
                        me->CastSpell(me, l_NeedSpells[urand(0, 4)], true);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kezan_partygoer_35186AI(p_Creature);
        }
};

/// Hired Looter 35234
struct npc_hired_looter_35234 : public ScriptedAI
{
    npc_hired_looter_35234(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum eItems
    {
        StolenLoot = 47530
    };

    void MoveInLineOfSight(Unit* p_Who) override
    {
        Player* l_Player = p_Who->ToPlayer();
        if (!l_Player)
            return;

        if (me->GetExactDist2d(p_Who) <= 5.0f && !me->isDead() && l_Player->GetVehicle())
            l_Player->Kill(me);
    }

    void JustDied(Unit* p_Killer) override
    {
        if (Player* l_Player = p_Killer->ToPlayer())
            l_Player->AddItem(eItems::StolenLoot, 1);
    }
};

/// First Bank of Kezan Vault 35486
struct npc_first_bank_of_kezan_vault_35486 : public VehicleAI
{
    npc_first_bank_of_kezan_vault_35486(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum eItems
    {
        PersonalRiches = 46858
    };

    enum eTexts
    {
        Intro = 35566,
        Elipse = 38461,
        ShowAll = 35567,
        Cracked = 35568,
        GoodLuck = 35569,
        KajamiteDrillText = 35576,
        InfinifoldLockpickText = 35579,
        AmazingGRayText = 35700,
        EarOScopeText = 35578,
        BlastcrackersText = 35577,
        Correct = 35702,
        TimedOut = 35773,
        Incorrect = 35704,
        Success = 35852
    };

    enum eSpells
    {
        AmazingGRay = 67526,
        Blastcrackers = 67508,
        EarOScope = 67524,
        InfinifoldLockpick = 67525,
        KajamiteDrill = 67522,
        PowerCorrect = 67493,
        PowerIncorrect = 67494,
        Timer = 67502
    };

    std::vector < std::pair < uint32, uint32>> const m_SpellsTexts =
    {
        {eSpells::AmazingGRay, eTexts::AmazingGRayText},
        {eSpells::Blastcrackers, eTexts::BlastcrackersText},
        {eSpells::EarOScope, eTexts::EarOScopeText},
        {eSpells::InfinifoldLockpick, eTexts::InfinifoldLockpickText},
        {eSpells::KajamiteDrill, eTexts::KajamiteDrillText},
    };

    ObjectGuid m_SummonerGuid;
    uint32 m_Expected;
    bool m_Started = false;

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        me->SetControlled(true, UnitState::UNIT_STATE_ROOT);

        if (!p_Apply)
            m_Started = false;
    }

    void IsSummonedBy(Unit* p_Summoner) override
    {
        Player* l_Player = p_Summoner->ToPlayer();
        if (!l_Player)
            return;

        m_SummonerGuid = p_Summoner->GetGUID();
        l_Player->KilledMonsterCredit(me->GetEntry());

        AddTimedDelayedOperation(500, [this]()->void
        {
            me->SetPower(Powers::POWER_MANA, 0);
        });

        AddTimedDelayedOperation(2 * IN_MILLISECONDS , [this]()->void
        {
            SendBroadcastText(eTexts::Intro);
            SendBroadcastText(eTexts::Elipse);
        });
        AddTimedDelayedOperation(7 * IN_MILLISECONDS, [this]()->void
        {
            SendBroadcastText(eTexts::ShowAll);
            SendBroadcastText(eTexts::Elipse);
        });
        AddTimedDelayedOperation(17 * IN_MILLISECONDS, [this]()->void
        {
            SendBroadcastText(eTexts::Cracked);
            SendBroadcastText(eTexts::Elipse);
        });
        AddTimedDelayedOperation(25 * IN_MILLISECONDS, [this]()->void
        {
            SendBroadcastText(eTexts::GoodLuck);
            SendBroadcastText(eTexts::Elipse);
            NextGadget();
            m_Started = true;
        });
    }

    void NextGadget()
    {
        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]()->void
        {
            if (me->GetPower(Powers::POWER_MANA) >= 121)
                return;

            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            uint8 l_Event = urand(0, 4);
            m_Expected = m_SpellsTexts[l_Event].first;

            SendBroadcastText(m_SpellsTexts[l_Event].second);
            DoCast(eSpells::Timer);
            l_Player->AddAura(eSpells::Timer, l_Player);
        });
    }

    void FailEvent()
    {
        Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
        if (!l_Player)
            return;

        me->RemoveAura(eSpells::Timer);
        l_Player->RemoveAura(eSpells::Timer);

        me->CastSpell(me, eSpells::PowerIncorrect, true);

        NextGadget();
    }

    void SuccedEvent()
    {
        Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
        if (!l_Player)
            return;

        me->RemoveAura(eSpells::Timer);
        l_Player->RemoveAura(eSpells::Timer);

        SendBroadcastText(eTexts::Correct);
        SendBroadcastText(eTexts::Elipse);
        me->CastSpell(me, eSpells::PowerCorrect, true);

        NextGadget();
    }

    void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
    {
        uint32 l_SpellId = p_Spell->Id;
        if (l_SpellId != eSpells::AmazingGRay &&
            l_SpellId != eSpells::Blastcrackers &&
            l_SpellId != eSpells::EarOScope &&
            l_SpellId != eSpells::InfinifoldLockpick &&
            l_SpellId != eSpells::KajamiteDrill)
            return;

        if (p_Spell->Id != m_Expected)
        {
            SendBroadcastText(eTexts::Incorrect);
            FailEvent();
        }
        else
            SuccedEvent();
    }

    void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
    {
        if (p_SpellID != eSpells::Timer)
            return;

        if (p_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
        {
            SendBroadcastText(eTexts::TimedOut);
            FailEvent();
        }
    }

    void SetPower(Powers /*p_Power*/, int32 p_Value) override
    {
        if (p_Value >= 121 && m_Started)
        {
            AddTimedDelayedOperation(500, [this]()->void
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                SendBroadcastText(eTexts::Success);
                l_Player->AddItem(eItems::PersonalRiches, 1);
            });
        }
    }

    void SendBroadcastText(uint32 p_ID)
    {
        Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
        if (!l_Player)
            return;

        std::string l_Text;
        if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(p_ID))
            l_Text = GetBroadcastTextValue(l_BroadcastText, l_Player->GetSession()->GetSessionDb2Locale(), l_Player->getGender());

        if (l_Text.empty())
            return;

        WorldPacket l_Packet;
        l_Player->BuildPlayerChat(&l_Packet, l_Player->GetGUID(), ChatMsg::CHAT_MSG_RAID_BOSS_WHISPER, l_Text, 0);
        l_Player->GetSession()->SendPacket(&l_Packet);
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        UpdateOperations(p_Diff);
    }
};

/// Mook Disguise 48925
struct npc_mook_disguise_48925 : public ScriptedAI
{
    npc_mook_disguise_48925(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    void OnExitVehicle(Unit* /*p_Vehicle*/, Position& p_ExitPos) override
    {
        me->DespawnOrUnsummon();
    }
};

/// Gasbot 37598
struct npc_gasbot_37598 : public ScriptedAI
{
    npc_gasbot_37598(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        Headquarters = 1
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
        me->GetMotionMaster()->MoveSmoothPath(ePoints::Headquarters, Waypoints::Path_Gasbot, 5, true);
        me->AddAura(eSpells::GasbotSteam, me);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::Headquarters:
            {
                me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
                DoCast(eSpells::GasbotExplosion);

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    l_Player->KilledMonsterCredit(me->GetEntry());

                break;
            }
            default:
                break;
        }
    }
};

/// Bilgewater Buccaneer 48526
struct npc_bilgewater_buccaneer_48526 : public ScriptedAI
{
    npc_bilgewater_buccaneer_48526(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    bool CanUseSpellClick(Unit* p_Clicker, uint32 p_SpellClicked) override
    {
        Player* l_Player = p_Clicker->ToPlayer();
        if (!l_Player)
            return false;

        if (l_Player->GetQuestStatus(eQuests::NecessaryRoughness) == QUEST_STATUS_INCOMPLETE)
            return true;

        l_Player->SetPhaseMask(2, true);

        if (l_Player->GetQuestStatus(eQuests::FourthAndGoal) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature * l_Creature = l_Player->SummonCreature(eCreatures::BilgewaterBuccaneer, Positions::g_FourthAndGoalBuccaneer, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetObjectGuid()))
                l_Player->EnterVehicle(l_Creature);
        }

        return false;
    }
};

/// Radio - 66299
class spell_kezan_radio : public SpellScript
{
    PrepareSpellScript(spell_kezan_radio);

    enum eSounds
    {
        Sound1 = 23406
    };

    void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        Unit* l_Caster = GetCaster();
        if (!l_Target || !l_Caster)
            return;

        if (Vehicle* l_Vehicle = l_Caster->GetVehicleKit())
            if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                if (Player* l_Player = l_Unit->ToPlayer())
                    l_Caster->PlayDistanceSound(l_Caster, eSounds::Sound1, l_Player);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_kezan_radio::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/// Necessary Roughness: Despawn Summoned Steamwheedle Sharks - 69987
class spell_despawn_summoned_steamwheedle_sharks : public SpellScript
{
    PrepareSpellScript(spell_despawn_summoned_steamwheedle_sharks);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        Creature* l_Creature = l_Target->ToCreature();
        if (!l_Creature)
            return;

        if (l_Creature->GetEntry() == eCreatures::SteamwheedleShark)
            l_Creature->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_despawn_summoned_steamwheedle_sharks::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/// Kablooey Bombs - 67682
class spell_kablooey_bombs : public SpellScript
{
    PrepareSpellScript(spell_kablooey_bombs);

    enum eGameObjects
    {
        KajamiteDeposit = 195488
    };

    void FilterTargets(std::list<WorldObject*>& p_Targets)
    {
        p_Targets.remove_if([&](WorldObject* p_Object) -> bool
        {
            if (!p_Object)
                return true;

            GameObject* l_Gob = p_Object->ToGameObject();
            if (!l_Gob)
                return true;

            if (l_Gob->GetEntry() != eGameObjects::KajamiteDeposit)
                return true;

            return false;
        });
    }

    void HandleActivateObject(SpellEffIndex /*p_EffIndex*/)
    {
        GameObject* l_GameObject = GetHitGObj();
        Unit* l_Caster = GetCaster();
        if (!l_GameObject || !l_Caster)
            return;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return;

        l_Player->AddObjectInLockoutList(l_GameObject->GetDBTableGUIDLow(), eQuests::LiberateTheKajamite, true);

        for (uint8 l_I = 0; l_I < 3; ++l_I)
        {
            Position l_Pos;
            l_GameObject->GetRandomNearPosition(l_Pos, 4.0f);
            l_Player->SummonGameObject(eGameobjects::KajamiteChunk, l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, l_Player->GetGUID());
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kablooey_bombs::FilterTargets, EFFECT_0, TARGET_GAMEOBJECT_DEST_AREA);
        OnEffectHitTarget += SpellEffectFn(spell_kablooey_bombs::HandleActivateObject, EFFECT_0, SPELL_EFFECT_ACTIVATE_OBJECT);
    }
};

/// The Great Bank Heist: Vault Interact - 67555
class spell_kezan_vault_interact : public SpellScript
{
    PrepareSpellScript(spell_kezan_vault_interact);

    enum eSpells
    {
        SummonBunnyVehicle = 67488
    };

    void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        l_Target->CastSpell(l_Target, eSpells::SummonBunnyVehicle, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_kezan_vault_interact::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/// Hobart's Ingenious Cap of Mook Foolery - 67435
class spell_kezan_hobarts_ingenious_cap_of_mook_foolery : public AuraScript
{
    PrepareAuraScript(spell_kezan_hobarts_ingenious_cap_of_mook_foolery);

    enum eCreatures
    {
        MookDisguise = 48925
    };

    void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
    {
        Unit* l_Caster = GetCaster();
        if (l_Caster)
        {
            l_Caster->AddDelayedEvent([l_Caster]()->void
            {
                if (Creature* l_Creature = l_Caster->SummonCreature(eCreatures::MookDisguise, l_Caster->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID()))
                    l_Creature->EnterVehicle(l_Caster);
            }, 500);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_kezan_hobarts_ingenious_cap_of_mook_foolery::HandleAfterApply, EFFECT_1, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
    }
};

/// 447: AICast Gasbot Master - 70253
class spell_kezan_aicast_gasbot_master : public SpellScript
{
    PrepareSpellScript(spell_kezan_aicast_gasbot_master);

    enum eObjectives
    {
        OverloadTheGenerator = 266731,
        ActivateTheLeakyStove = 266732,
        DropACigar = 266733,
        KTCHeadquartersSetAblaze = 266734
    };

    SpellCastResult HandleCheckCast()
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return SPELL_FAILED_DONT_REPORT;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return SPELL_FAILED_DONT_REPORT;

        if (l_Player->GetQuestStatus(eQuests::q447) != QUEST_STATUS_INCOMPLETE)
            return SPELL_FAILED_DONT_REPORT;

        if (l_Player->GetQuestObjectiveCounter(eObjectives::OverloadTheGenerator) == 0 ||
            l_Player->GetQuestObjectiveCounter(eObjectives::ActivateTheLeakyStove) == 0 ||
            l_Player->GetQuestObjectiveCounter(eObjectives::DropACigar) == 0)
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_GOBLIN_STARTING_MISSION);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        return SPELL_CAST_OK;
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        l_Caster->SummonCreature(eCreatures::Gasbot, l_Caster->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID());
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_kezan_aicast_gasbot_master::HandleCheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_kezan_aicast_gasbot_master::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/// Life Savings: Rope Ladder - Cover - 70389
class spell_kezan_life_savings_rope_ladder : public SpellScript
{
    PrepareSpellScript(spell_kezan_life_savings_rope_ladder);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        l_Target->GetMotionMaster()->MoveReverseJump(Positions::g_RopeLadderJumpPos, 384, 271.2674f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_kezan_life_savings_rope_ladder::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class playerscript_kezan_scripts : public PlayerScript
{
    public:
        playerscript_kezan_scripts() : PlayerScript("playerscript_kezan_scripts") {}

        void OnQuestAccept(Player * p_Player, const Quest * p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::RollingWithMyHomies:
                {
                    std::string l_Text;
                    if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(48504))
                        l_Text = GetBroadcastTextValue(l_BroadcastText, p_Player->GetSession()->GetSessionDb2Locale(), p_Player->getGender());

                    if (l_Text.empty())
                        return;

                    WorldPacket l_Packet;
                    p_Player->BuildPlayerChat(&l_Packet, p_Player->GetGUID(), ChatMsg::CHAT_MSG_RAID_BOSS_WHISPER, l_Text, 0);
                    p_Player->GetSession()->SendPacket(&l_Packet);
                    break;
                }
                case eQuests::FourthAndGoal:
                {
                    p_Player->SummonCreature(eCreatures::BilgewaterBuccaneer, Positions::g_FourthAndGoalBuccaneer, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
                    break;
                }
                case eQuests::LifeOfTheParty:
                {
                    std::string l_Text;
                    if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(35444))
                        l_Text = GetBroadcastTextValue(l_BroadcastText, p_Player->GetSession()->GetSessionDb2Locale(), p_Player->getGender());

                    if (l_Text.empty())
                        return;

                    WorldPacket l_Packet;
                    p_Player->BuildPlayerChat(&l_Packet, p_Player->GetGUID(), ChatMsg::CHAT_MSG_RAID_BOSS_WHISPER, l_Text, 0);
                    p_Player->GetSession()->SendPacket(&l_Packet);
                    break;
                }
                case eQuests::RobbingHoods:
                {
                    std::string l_Text;
                    if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(48844))
                        l_Text = GetBroadcastTextValue(l_BroadcastText, p_Player->GetSession()->GetSessionDb2Locale(), p_Player->getGender());

                    if (l_Text.empty())
                        return;

                    WorldPacket l_Packet;
                    p_Player->BuildPlayerChat(&l_Packet, p_Player->GetGUID(), ChatMsg::CHAT_MSG_RAID_BOSS_WHISPER, l_Text, 0);
                    p_Player->GetSession()->SendPacket(&l_Packet);
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
                case eQuests::LifeSavings:
                {
                    p_Player->AddMovieDelayedTeleport(22, 648, 534.8350f, 3272.9199f, 0.1719f, 5.147953f);
                    p_Player->SendMovieStart(22);

                    WorldLocation l_NewHomeBind = WorldLocation(648, 604.8064f, 3126.3450f, 2.2438f);
                    p_Player->SetHomebind(l_NewHomeBind, 4871);
                    break;
                }
                default:
                    break;
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_zone_kezan()
{
    /// Creatures
    new npc_hot_rod_34840();
    new npc_izzy_34959();
    new npc_gobber_34958();
    new npc_ace_34957();
    new npc_bilgewater_buccaneer_37179();
    new npc_steamwheedle_shark_37114();
    new npc_bilgewater_buccaneer_37213();
    new npc_deathwing_48572();
    new npc_kezan_partygoer_35186();
    RegisterCreatureAI(npc_hired_looter_35234);
    RegisterCreatureAI(npc_first_bank_of_kezan_vault_35486);
    RegisterCreatureAI(npc_mook_disguise_48925);
    RegisterCreatureAI(npc_gasbot_37598);
    RegisterCreatureAI(npc_bilgewater_buccaneer_48526);

    /// Spells
    RegisterSpellScript(spell_kezan_radio);
    RegisterSpellScript(spell_despawn_summoned_steamwheedle_sharks);
    RegisterSpellScript(spell_kablooey_bombs);
    RegisterSpellScript(spell_kezan_vault_interact);
    RegisterSpellScript(spell_kezan_aicast_gasbot_master);
    RegisterSpellScript(spell_kezan_life_savings_rope_ladder);
    RegisterAuraScript(spell_kezan_hobarts_ingenious_cap_of_mook_foolery);

    /// PlayerScripts
    new playerscript_kezan_scripts();
}
#endif