////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2018 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "zone_lost_isles.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "ScriptedEscortAI.h"

/// Geargrinder Gizmo - 36600
struct npc_geargrinder_gizmo_36600 : public ScriptedAI
{
    npc_geargrinder_gizmo_36600(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    void MoveInLineOfSight(Unit* p_Who) override
    {
        Player* l_Player = p_Who->ToPlayer();
        if (!l_Player || me->GetNameTag() != "ArrivalEvent")
            return;

        if (me->GetExactDist2d(p_Who) <= 5.0f && !l_Player->IsCreatureInInaccessibleList(me->GetGUID()) && !l_Player->IsQuestRewarded(eQuests::DontGoIntoTheLight))
        {
            l_Player->AddCreatureInInaccessibleList(me->GetGUID());
            l_Player->CastSpell(l_Player, eSpells::NearDeath, true);

            p_Who->SummonCreature(eCreatures::DocZapnozzle, Positions::DocZapnozzleSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Who->GetGUID());
        }
    }

    void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
    {
        switch (p_Quest->GetQuestId())
        {
            case eQuests::GoblinEscapePods:
            {
                if (Creature* l_Creature = p_Player->FindDetectableNearestCreature(eCreatures::DocZapnozzle, 5.0f))
                    if (l_Creature->IsAIEnabled)
                        l_Creature->AI()->DoAction(1);

                uint64 l_PlayerGuid = p_Player->GetGUID();
                AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this, l_PlayerGuid]()->void
                {
                    Talk(1, l_PlayerGuid);
                });
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

/// Doc Zapnozzle  36608
struct doc_zapnozzle_36608 : public ScriptedAI
{
    doc_zapnozzle_36608(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        Path = 1,
        FinishEvent
    };

    enum eActions
    {
        GoAway = 1
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Path, Positions::DocZapnozzleStartEventPos);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::Path:
            {
                Talk(0, m_SummonerGuid);

                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]()->void
                {
                    Talk(1, m_SummonerGuid);
                });
                AddTimedDelayedOperation(8 * IN_MILLISECONDS, [this]()->void
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eCreatures::GeargrinderGizmo, 5.0f))
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->PersonalTalk(0, m_SummonerGuid);
                });
                AddTimedDelayedOperation(14 * IN_MILLISECONDS, [this]()->void
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                    if (!l_Player)
                        return;

                    Talk(2, m_SummonerGuid);
                    me->DelayedCastSpell(l_Player, eSpells::JumperCables, false, 1000);
                });
                AddTimedDelayedOperation(19 * IN_MILLISECONDS, [this]()->void
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                    if (!l_Player)
                        return;

                    Talk(3, m_SummonerGuid);
                    me->DelayedCastSpell(l_Player, eSpells::JumperCables, false, 1000);
                });
                AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]()->void
                {
                    Talk(4, m_SummonerGuid);
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                });
                break;
            }
            default:
                break;
        }
    }

    void DoAction(int32 const p_Action) override
    {
        Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
        if (!l_Player)
            return;

        switch (p_Action)
        {
            case eActions::GoAway:
            {
                Talk(6, m_SummonerGuid);
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]()->void
                {
                    Talk(4, m_SummonerGuid);
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::FinishEvent, Waypoints::Path_DozZapnozzleEndEvent, 3, false);
                });
                break;
            }
            default:
                break;
        }
    }

    void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
    {
        switch (p_Quest->GetQuestId())
        {
            case eQuests::DontGoIntoTheLight:
            {
                Talk(5, m_SummonerGuid);
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

/// Frightened Miner 35813
class npc_frightened_miner_35813 : public CreatureScript
{
    public:
        npc_frightened_miner_35813() : CreatureScript("npc_frightened_miner_35813") { }

        struct npc_frightened_miner_35813AI : public npc_escortAI
        {
            npc_frightened_miner_35813AI(Creature* p_Creature) : npc_escortAI(p_Creature) { }

            enum eEvents
            {
                CheckPlayer = 1
            };

            enum eKillcredits
            {
                MinerTroublesKC = 35816
            };

            ObjectGuid m_SummonerGuid;
            ObjectGuid m_CartGuid;
            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->GetMotionMaster()->Clear();
                me->StopMoving();
                Talk(0, m_SummonerGuid);

                if (Creature* l_Creature = me->SummonCreature(eCreatures::MinerTroublesOreCart, me->GetPosition()))
                {
                    m_CartGuid = l_Creature->GetGUID();
                    l_Creature->CastSpell(me, eSpells::MinerTroublesChains, true);
                    l_Creature->GetMotionMaster()->MoveFollow(me, 1.0f, float(M_PI));
                }

                AddTimedDelayedOperation(1000, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                });

                AddTimedDelayedOperation(3000, [this]() -> void
                {
                    Start(true);
                    m_Events.ScheduleEvent(eEvents::CheckPlayer, 3 * IN_MILLISECONDS);
                });
            }

            void WaypointReached(uint32 p_WaypointId) override
            {
                switch (p_WaypointId)
                {
                    case 4:
                        Talk(1, m_SummonerGuid);
                        break;
                    case 30:
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_CartGuid))
                            l_Creature->DespawnOrUnsummon();

                        me->DespawnOrUnsummon(500);
                        break;
                    case 10:
                    case 15:
                    case 19:
                    case 25:
                        me->HandleEmoteCommand(233);
                        break;
                    default:
                        break;
                }
            }

            void WaypointStart(uint32 p_WaypointId) override
            {
                switch (p_WaypointId)
                {
                    case 11:
                        Talk(2, m_SummonerGuid);
                        break;
                    case 16:
                        Talk(2, m_SummonerGuid);
                        break;
                    case 20:
                        Talk(3, m_SummonerGuid);
                        break;
                    case 26:
                        Talk(4, m_SummonerGuid);

                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                            l_Player->KilledMonsterCredit(eKillcredits::MinerTroublesKC);

                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                npc_escortAI::UpdateAI(p_Diff);
                UpdateOperations(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::CheckPlayer:
                    {
                        Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                        if (!l_Player)
                            return;

                        if (me->isInCombat())
                            return;

                        if (l_Player->isInCombat())
                        {
                            Unit* l_Target = l_Player->GetSelectedUnit();
                            if (!l_Target)
                                return;

                            me->CombatStart(l_Target);
                            AttackStart(l_Target, true);
                        }

                        m_Events.ScheduleEvent(eEvents::CheckPlayer, 1 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_frightened_miner_35813AI(p_Creature);
        }
};

/// Weed Whacker Bunny 35903
struct npc_weed_whacker_bunny_35903 : public ScriptedAI
{
    npc_weed_whacker_bunny_35903(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();

        me->CastSpell(me, eSpells::WeedWhackerControlVehicle, true);
        me->CastSpell(me, eSpells::WeedWhackerChannelBeam, true);
        me->CastSpell(me, eSpells::WeedWhackerPlantBurst, true);
    }
};

/// Orc Scout 35918
struct orc_scout_35918 : public ScriptedAI
{
    orc_scout_35918(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        Path = 1,
    };

    enum eActions
    {
        PathLeft = 1,
        PathRight = 2,
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
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

    void DoAction(int32 const p_Action) override
    {
        switch (p_Action)
        {
            case eActions::PathRight:
            {
                Talk(0, m_SummonerGuid);
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]()->void
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Path, Waypoints::Path_Scout2, 12, false);
                });
                break;
            }
            case eActions::PathLeft:
            {
                Talk(1, m_SummonerGuid);
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]()->void
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Path, Waypoints::Path_Scout1, 14, false);
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

/// Bastia 36585
struct npc_bastia_36585 : public VehicleAI
{
    npc_bastia_36585(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        PathToTheCliffs = 1
    };

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
            me->GetMotionMaster()->MoveSmoothPath(ePoints::PathToTheCliffs, Waypoints::Path_ToTheCliffs, 34, false);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::PathToTheCliffs:
            {
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
        }
    }
};

/// Gyrochoppa 36143
struct npc_gyrochoppa_36143 : public VehicleAI
{
    npc_gyrochoppa_36143(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        PathPreciousCargo = 1
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
    }

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::PathPreciousCargo, Waypoints::Path_PreciousCargo, 6);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::PathPreciousCargo:
            {
                Talk(0, m_SummonerGuid);
                me->DespawnOrUnsummon(500);
                break;
            }
            default:
                break;
        }
    }
};

/// Alliance Sailor 36176
struct npc_alliance_sailor_36176 : public ScriptedAI
{
    npc_alliance_sailor_36176(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    void InitializeAI() override
    {
        if (me->GetNameTag() == "PreciousCargo Fear")
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            me->SetControlled(true, UNIT_STATE_FLEEING);
            me->GetMotionMaster()->MoveFleeing(me);
        }
    }

    void Reset() override
    {
        InitializeAI();
    }
};

/// Cyclone of the Elements 36178
struct npc_cyclone_of_the_elements_36178 : public VehicleAI
{
    npc_cyclone_of_the_elements_36178(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        CyclicPath = 1,
        QuestCompletedP
    };

    enum eActions
    {
        QuestCompleted = 1
    };

    bool m_QuestCompleted = false;

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
    }

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
        {
            me->SetCanFly(true);
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::CyclicPath, Waypoints::Path_WarchiefProgress, 26);
        }
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::CyclicPath:
            {
                if (!m_QuestCompleted)
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::CyclicPath, Waypoints::Path_WarchiefProgress, 26);

                break;
            }
            case ePoints::QuestCompletedP:
            {
                me->DespawnOrUnsummon();
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
            case eActions::QuestCompleted:
            {
                m_QuestCompleted = true;

                me->GetMotionMaster()->MoveIdle();
                me->StopMoving();
                me->GetMotionMaster()->Clear();

                me->GetMotionMaster()->MoveSmoothPath(ePoints::QuestCompletedP, Waypoints::Path_WarchiefCompleted, 9, false);
                break;
            }
            default:
                break;
        }
    }
};

/// Sling Rocket 36505
struct npc_sling_rocket_36505 : public VehicleAI
{
    npc_sling_rocket_36505(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        UpAndDown = 1
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
    }

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::UpAndDown, Waypoints::Path_UpUpAway, 8);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::UpAndDown:
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                me->CastSpell(l_Player, eSpells::UpUpAwayKC, true);
                me->DespawnOrUnsummon(1000);
                break;
            }
            default:
                break;
        }
    }
};

/// Wild Clucker Egg 38195
struct npc_wild_clucker_egg_38195 : public ScriptedAI
{
    npc_wild_clucker_egg_38195(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum eActions
    {
        RaptorArrived = 1
    };

    ObjectGuid m_SummonerGuid;
    ObjectGuid m_RaptorGuid;
    bool m_Done;

    void Reset() override
    {
        m_Done = false;
    }

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
        me->GetMotionMaster()->Clear();
        me->StopMoving();
        me->SetControlled(true, UnitState::UNIT_STATE_ROOT);

        if (Creature* l_Creature = me->FindNearestCreature(eCreatures::SpinyRaptor, 40.0f, true))
        {
            l_Creature->GetMotionMaster()->MovePoint(1, me->GetPosition());
            l_Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            m_RaptorGuid = l_Creature->GetGUID();
        }
    }

    void DoAction(int32 const p_Action) override
    {
        switch (p_Action)
        {
            case eActions::RaptorArrived:
            {
                GameObject* l_Gob = me->FindNearestGameObject(eGameobjects::RaptorTrap, 5.0f);
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_RaptorGuid);
                if (!l_Gob || !l_Player || !l_Creature)
                    return;

                m_Done = true;
                l_Gob->SendCustomAnim(0);
                l_Gob->SendObjectDeSpawnAnim(m_SummonerGuid);
                l_Player->AddObjectInLockoutList(l_Gob->GetDBTableGUIDLow(), 1, true);

                me->Kill(l_Creature);
                l_Creature->DespawnOrUnsummon(1000);

                l_Player->SummonGameObject(eGameobjects::RaptorEgg, me->GetPosition(), 0, 0, 0, 0, 0, l_Player->GetGUID());
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

/// Spiny Raptor 38187
struct npc_spiny_raptor_38187 : public ScriptedAI
{
    npc_spiny_raptor_38187(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        ToTheTrap = 1
    };

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::ToTheTrap:
            {
                if (Creature* l_Creature = me->FindNearestCreature(eCreatures::WildCluckerEgg, 5.0f, true))
                    l_Creature->AI()->DoAction(1);

                break;
            }
            default:
                break;
        }
    }
};

/// Mechachicken 38224
struct npc_mechachicken_38224 : public ScriptedAI
{
    npc_mechachicken_38224(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    void JustDied(Unit* p_Killer) override
    {
        p_Killer->SummonGameObject(eGameobjects::TheBiggestEggEver, me->GetPosition(), 0, 0, 0, 0, 0);
        p_Killer->SummonCreature(eCreatures::TheBiggestEggEverVisual, me->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 120000);
    }
};

/// Mechashark X-Steam 38318
struct npc_mechashark_x_steam_38318 : public VehicleAI
{
    npc_mechashark_x_steam_38318(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum eEvents
    {
        CheckWater = 1
    };

    EventMap m_Events;
    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
        m_Events.ScheduleEvent(eEvents::CheckWater, 1 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        m_Events.Update(p_Diff);

        switch (m_Events.ExecuteEvent())
        {
            case eEvents::CheckWater:
            {
                if (!me->IsInWater())
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                        l_Player->RemoveAura(eSpells::MechasharkXSteam);

                    me->DespawnOrUnsummon();
                }

                m_Events.ScheduleEvent(eEvents::CheckWater, 1 * IN_MILLISECONDS);
                break;
            }
            default:
                break;
        }
    }
};

/// Naga Hatchling 44579 38412 44580
struct npc_naga_hatchling : public ScriptedAI
{
    npc_naga_hatchling(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum eKillcredits
    {
        NagaLured = 38413
    };

    enum eCreatures
    {
        Naga1 = 44579,
        Naga2 = 38412,
        Naga3 = 44580
    };

    enum eSpells
    {
        NagaHatching = 71918
    };

    EventMap m_Events;
    ObjectGuid m_SummonerGuid;

    void sGossipHello(Player* p_Player) override
    {
        if (p_Player->IsCreatureInInaccessibleList(me->GetGUID()))
            return;

        std::list<Creature*> l_Nagas;
        me->GetCreatureListWithEntryInGrid(l_Nagas, eCreatures::Naga1, 10.0f);
        me->GetCreatureListWithEntryInGridAppend(l_Nagas, eCreatures::Naga2, 10.0f);
        me->GetCreatureListWithEntryInGridAppend(l_Nagas, eCreatures::Naga3, 10.0f);

        for (auto l_Creature : l_Nagas)
        {
            if (p_Player->IsCreatureInInaccessibleList(l_Creature->GetGUID()))
                continue;

            p_Player->AddCreatureInInaccessibleList(l_Creature->GetGUID());
            p_Player->KilledMonsterCredit(eKillcredits::NagaLured);
            p_Player->CastSpell(l_Creature, eSpells::NagaHatching, true);

            l_Creature->DespawnOrUnsummon(500);
        }
    }
};

/// Naga Hatchling 44589
struct npc_naga_hatchling_44589 : public ScriptedAI
{
    npc_naga_hatchling_44589(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        me->GetMotionMaster()->MoveFollow(p_Summoner, 3.0f, PET_FOLLOW_ANGLE);
    }
};

/// Ace 38455
struct npc_ace_38455 : public ScriptedAI
{
    npc_ace_38455(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        Path = 1,
        RunAway
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
        me->CastSpell(me, eSpells::SummonNagaHatchlings, true);
        Talk(1, m_SummonerGuid);

        AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this] () -> void {
            Talk(2, m_SummonerGuid);
        });

        AddTimedDelayedOperation(8 * IN_MILLISECONDS, [this]() -> void {
            me->GetMotionMaster()->MoveSmoothPath(ePoints::Path, Waypoints::Path_AceSurrenderOrElse, 13, false);
        });
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::Path:
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                Talk(4, m_SummonerGuid);

                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this, l_Player]() -> void {
                    l_Player->SummonCreature(eCreatures::FacelessOfTheDeep, Positions::FacelessPos, TempSummonType::TEMPSUMMON_CORPSE_DESPAWN, 0, 0, l_Player->GetGUID());
                });

                AddTimedDelayedOperation(35 * IN_MILLISECONDS, [this]() -> void {
                    Talk(5, m_SummonerGuid);
                });

                AddTimedDelayedOperation(38 * IN_MILLISECONDS, [this]() -> void {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::RunAway, Waypoints::Path_AceRunAway, 7, false);
                });
                break;
            }
            case ePoints::RunAway:
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

/// Faceless Of The Deep 38448
struct npc_faceless_of_the_deep_38448 : public ScriptedAI
{
    npc_faceless_of_the_deep_38448(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        TakeOff = 1,
        Jump
    };

    ObjectGuid m_SummonerGuid;
    ObjectGuid m_VoidGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();

        if (Creature* l_Creature = me->SummonCreature(eCreatures::FacelessVoidZone, Positions::FacelessVoidPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 360000, 0, p_Summoner->GetGUID()))
            m_VoidGuid = l_Creature->GetGUID();

        me->CastSpell(me, eSpells::FacelessFreeze, true);
        me->CastSpell(me, eSpells::AbsorptionShield, true);
        me->GetMotionMaster()->MovePoint(ePoints::TakeOff, Positions::FacelessVoidAirPos);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::TakeOff:
            {
                Talk(0, m_SummonerGuid);

                AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void {
                    Talk(1, m_SummonerGuid);
                });
                AddTimedDelayedOperation(9 * IN_MILLISECONDS, [this]() -> void {
                    Talk(2, m_SummonerGuid);
                });
                AddTimedDelayedOperation(14 * IN_MILLISECONDS, [this]() -> void {
                    me->GetMotionMaster()->MoveReverseJump(Positions::FacelessVoidJumpPos, 1213, 27.18556, ePoints::Jump);
                });

                break;
            }
            case ePoints::Jump:
            {
                me->RemoveFlag(/*Hi Maxtor*/UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                me->RemoveAura(eSpells::FacelessFreeze);
                me->RemoveAura(eSpells::AbsorptionShield);

                if (Unit* l_Unit = ObjectAccessor::GetUnit(*me, m_SummonerGuid))
                    AttackStart(l_Unit);

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

/// B.C. Eliminator 38526
struct npc_bc_eliminator_38526 : public VehicleAI
{
    npc_bc_eliminator_38526(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum eEvents
    {
        SpawnNpcs = 1
    };

    EventMap m_Events;
    ObjectGuid m_Current;
    std::list<ObjectGuid> m_SummonList;

    std::vector <Position> const m_SpawnsPos =
    {
        { 913.3362f, 2204.3394f, 41.7559f, 1.46f },
        { 900.5726f, 2196.3430f, 48.8439f, 1.46f },
        { 910.3450f, 2197.0791f, 46.6422f, 1.46f },
        { 920.4683f, 2207.4043f, 38.7080f, 1.46f },
        { 902.8279f, 2208.8101f, 40.4603f, 1.46f },
    };

    void Reset() override
    {
        m_Events.Reset();
        m_Current.Clear();
    }

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
        {
            m_Events.ScheduleEvent(eEvents::SpawnNpcs, 1 * IN_MILLISECONDS);
            m_Current = p_Who->GetGUID();
        }
        else {
            for (auto l_Entry : m_SummonList)
                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, l_Entry))
                    l_Creature->DespawnOrUnsummon();

            m_Events.Reset();
            m_Current.Clear();
            m_SummonList.clear();
        }
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        m_Events.Update(p_Diff);
        UpdateOperations(p_Diff);

        switch (m_Events.ExecuteEvent())
        {
            case eEvents::SpawnNpcs:
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_Current);
                if (!l_Player)
                    return;

                for (Position const l_Pos : m_SpawnsPos)
                    if (Creature* l_Creature = l_Player->SummonCreature(eCreatures::OomlotWarrior, l_Pos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 60000, 0, l_Player->GetGUID()))
                        m_SummonList.push_back(l_Creature->GetGUID());

                m_Events.ScheduleEvent(eEvents::SpawnNpcs, 1 * IN_MILLISECONDS);
                break;
        }
        default:
            break;
        }
    }
};

/// Oomlot Warrior 38531
struct npc_oomlot_warrior_38531 : public ScriptedAI
{
    npc_oomlot_warrior_38531(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum eKillcredits
    {
        OomlotWarriorsDefeated = 38536
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        me->GetMotionMaster()->MovePoint(1, Positions::OomlotPos);
    }

    void JustDied(Unit* p_Killer) override
    {
        Vehicle* l_Vehicle = p_Killer->GetVehicleKit();
        if (!l_Vehicle)
            return;

        Unit* l_Unit = l_Vehicle->GetPassenger(0);
        if (!l_Unit)
            return;

        if (Player* l_Player = l_Unit->ToPlayer())
            l_Player->KilledMonsterCredit(eKillcredits::OomlotWarriorsDefeated);
    }
};

/// Oomlot Shaman 38644
struct npc_oomlot_shaman_38644 : public ScriptedAI
{
    npc_oomlot_shaman_38644(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    ObjectGuid m_CaptiveGuid;

    void InitializeAI() override
    {
        Position l_FrontPos;
        me->GetNearPosition(l_FrontPos, 5.0f, 0.0f);
        if (Creature* l_Creature = me->SummonCreature(eCreatures::GoblinCaptive, l_FrontPos, TempSummonType::TEMPSUMMON_CORPSE_DESPAWN))
        {
            l_Creature->CastSpell(me, eSpells::EnvelopingWindsChannel, false);
            m_CaptiveGuid = l_Creature->GetGUID();
        }
    }

    void Reset() override
    {
        InitializeAI();
    }

    void JustDied(Unit* p_Killer) override
    {
        Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_CaptiveGuid);
        if (!l_Creature)
            return;

        l_Creature->RemoveAura(eSpells::EnvelopingWinds);
        l_Creature->DespawnOrUnsummon(8 * IN_MILLISECONDS);
        l_Creature->AI()->Talk(0);

        l_Creature->AddDelayedEvent([l_Creature]() -> void
        {
            float l_X, l_Y, l_Z;
            l_Creature->GetRandomContactPoint(l_Creature, l_X, l_Y, l_Z, 1.0f, 20.0f);
            l_Creature->GetMotionMaster()->MovePoint(1, l_X, l_Y, l_Z);
        }, 2 * IN_MILLISECONDS);
    }
};

/// Volcanoth 38855
struct npc_volcanoth_38855 : public ScriptedAI
{
    npc_volcanoth_38855(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum eEvents
    {
        CastVolcanicBreath = 1
    };

    EventMap m_Events;
    Position const m_BreathPos = { 1217.2100f, 1148.5900f, 117.8395f, 0.314f };

    void Reset() override
    {
        m_Events.Reset();
    }

    void EnterCombat(Unit* victim) override
    {
        m_Events.ScheduleEvent(eEvents::CastVolcanicBreath, 3 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        m_Events.Update(p_Diff);
        UpdateOperations(p_Diff);

        switch (m_Events.ExecuteEvent())
        {
            case eEvents::CastVolcanicBreath:
            {
                me->CastSpell(m_BreathPos, eSpells::VolcanicBreath, false);

                me->SetOrientation(0.314f);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->SetOrientation(0.314f);

                AddTimedDelayedOperation(5500, [this]()->void
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                });

                m_Events.ScheduleEvent(eEvents::CastVolcanicBreath, 9 * IN_MILLISECONDS);
                break;
        }
            default:
                break;
        }
    }

    void JustDied(Unit* p_Killer) override
    {
        me->CastSpell(me, eSpells::VolcanothKC, true);
        me->CastSpell(me, eSpells::VolcanothBootzookaExtraFX, true);
    }
};

/// Flying Bomber 38929
struct npc_flying_bomber_38929 : public VehicleAI
{
    npc_flying_bomber_38929(Creature* p_Creature) : VehicleAI(p_Creature) { }

    void InitializeAI() override
    {
        if (Creature* l_Sassy = me->SummonCreature(eCreatures::SassyHardwrench, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
        {
            uint64 l_Guid = me->GetGUID();
            l_Sassy->AddDelayedEvent([l_Guid, l_Sassy]() -> void
            {
                if(Creature* l_Me = ObjectAccessor::GetCreature(*l_Sassy, l_Guid))
                    l_Sassy->EnterVehicle(l_Me, -1, true);

            }, 1000);
        }
    }
};

/// Flying Bomber 38918
struct npc_flying_bomber_38918 : public VehicleAI
{
    npc_flying_bomber_38918(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        Path = 1
    };

    ObjectGuid m_SummonerGuid;
    ObjectGuid m_SassyGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();

        if (Creature* l_Sassy = me->SummonCreature(eCreatures::SassyHardwrench2, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
        {
            uint64 l_Guid = me->GetGUID();
            l_Sassy->AddDelayedEvent([l_Sassy, l_Guid]()->void
            {
                Creature* l_Me = ObjectAccessor::GetCreature(*l_Sassy, l_Guid);
                if (!l_Me)
                    return;

                l_Sassy->EnterVehicle(l_Me, 0);
            }, 500);

            m_SassyGuid = l_Sassy->GetGUID();
            l_Sassy->AI()->Talk(0, m_SummonerGuid);
        }

        p_Summoner->EnterVehicle(me, 1);
    }

    void PassengerBoarded(Unit* p_Who, int8 p_SeatId, bool p_Apply) override
    {
        if (p_Apply && p_Who->IsPlayer())
        {
            me->AddDelayedEvent([this]() -> void
            {
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Path, Waypoints::Path_OldFriends, 25);
            }, 1000);
        }
    }

     void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::Path:
            {
                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_SassyGuid))
                    l_Creature->DespawnOrUnsummon();

                me->DespawnOrUnsummon(500);
                break;
            }
            default:
                break;
        }
    }
};

/// Bastia 39152
struct npc_bastia_39152 : public VehicleAI
{
    npc_bastia_39152(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        PathToTheCliffs = 1
    };

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
            me->GetMotionMaster()->MoveSmoothPath(ePoints::PathToTheCliffs, Waypoints::Path_LetsRide, 61, false);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::PathToTheCliffs:
            {
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
        }
    }
};

/// Weed Whacker Bunny 35903
struct npc_morale_boost : public ScriptedAI
{
    npc_morale_boost(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    ObjectGuid m_EventGuid;
    bool m_Available;

    void Reset() override
    {
        m_Available = false;
    }

    void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
    {
        if (p_Spell->Id != eSpells::KajaColaZeroOneItem)
            return;

        m_EventGuid = p_Caster->GetGUID();
        m_Available = false;
    }

    void OnSpellCasted(SpellInfo const* p_SpellInfo) override
    {
        if (p_SpellInfo->Id != eSpells::KajaColaZeroOne)
            return;

        uint32 l_SpellId = -1;

        switch (me->GetEntry())
        {
            case eCreatures::GobberMine:
            {
                l_SpellId = eSpells::FreeSummonGobber;
                break;
            }
            case eCreatures::IzzyMine:
            {
                l_SpellId = eSpells::FreeSummonIzzy;
                break;
            }
            case eCreatures::AceMinee:
            {
                l_SpellId = eSpells::FreeSummonAce;
                break;
            }
            default:
                break;
        }

        if (l_SpellId == -1)
            return;

        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_EventGuid))
            l_Player->CastSpell(l_Player, l_SpellId, true);
    }
};

/// Mine Cart 39329
struct npc_mine_cart_39329 : public VehicleAI
{
    npc_mine_cart_39329(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        FirstPath = 1,
        SecondPath,
    };

    enum eSpells
    {
        PathEnd = 73766
    };

    enum eKillcredits
    {
        CartRidden = 39335
    };

    ObjectGuid m_SummonerGuid;

    void IsSummonedBy(Unit* p_Summoner) override
    {
        m_SummonerGuid = p_Summoner->GetGUID();
    }

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
            me->GetMotionMaster()->MoveSmoothPath(ePoints::FirstPath, Waypoints::Path_MineCartFirst, 6, false);
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::FirstPath:
            {
                me->GetMotionMaster()->MoveJumpWithPath(10.0f, 7.0f, Waypoints::Path_MineCartSecond, 13, ePoints::SecondPath);
                break;
            }
            case ePoints::SecondPath:
            {
                me->DespawnOrUnsummon(1000);

                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                l_Player->CastSpell(l_Player, eSpells::PathEnd, false);
                l_Player->KilledMonsterCredit(eKillcredits::CartRidden);
                break;
            }
            default:
                break;
        }
    }
};

/// Footbomb Uniform 47956
struct npc_footbomb_uniform_47956 : public VehicleAI
{
    npc_footbomb_uniform_47956(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        Path = 1
    };

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
            me->GetMotionMaster()->MoveSmoothPath(ePoints::Path, Waypoints::Path_SlavePits, 30, false);
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
};

/// Captured Goblin 39456
struct npc_captured_goblin_39456 : public ScriptedAI
{
    npc_captured_goblin_39456(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    enum ePoints
    {
        Jump = 1
    };

    void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
    {
        if (p_Spell->Id != eSpells::EscapeVelocityOnInteract)
            return;

        Player* l_Player = p_Caster->ToPlayer();
        if (!l_Player)
            return;

        if (l_Player->IsCreatureInInaccessibleList(me->GetGUID()))
            return;

        me->DestroyForPlayer(l_Player);
        me->AddToHideList(l_Player->GetGUID());
        l_Player->AddCreatureInInaccessibleList(me->GetGUID());
        l_Player->KilledMonsterCredit(me->GetEntry());

        me->CastSpell(me, eSpells::EscapeVelocityRockets, true);
        me->SetCanFly(true, true);
        Talk(0, p_Caster->GetGUID());

        AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]()->void
        {
            Position const l_Pos = { me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 30.0f };
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Jump, l_Pos);
        });
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::Jump:
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

/// Sassy Hardwrench 38387
struct npc_sassy_ship_38387 : public ScriptedAI
{
    npc_sassy_ship_38387(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    void sGossipHello(Player* p_Player) override
    {
        p_Player->PrepareGossipMenu(me, 11244);
        p_Player->SendPreparedGossip(me);
    }

    void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 p_Action) override
    {
        switch (p_Action)
        {
            case 0:
            {
                me->CastSpell(p_Player, eSpells::WarchiefsEmissaryCharacterForceCastFromGossip, true);
                break;
            }
            default:
                break;
        }
    }
};

/// Raptor 86883
struct npc_raptor_86883 : public VehicleAI
{
    npc_raptor_86883(Creature* p_Creature) : VehicleAI(p_Creature) { }

    enum ePoints
    {
        Path1 = 1,
        Path2,
        Path3,
        Path4,
        Path5,
        Path6,
        PathFinal
    };

    void PassengerBoarded(Unit* p_Who, int8 /*p_SeatId*/, bool p_Apply) override
    {
        if (p_Apply)
        {
            AddTimedDelayedOperation(1000, [this]() -> void
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Path1, Waypoints::Path_Raptor, 5, false);
            });
        }
    }

    void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
    {
        switch (p_PointId)
        {
            case ePoints::Path1:
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Path2, Waypoints::Path_Raptor2, 5, false);
                break;
            }
            case ePoints::Path2:
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::PathFinal, Waypoints::Path_Raptor3, 6, false);
                break;
            }
            case ePoints::PathFinal:
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

/// Deadly Jungle Plant 35896, 35897, 35895, 35995
struct npc_deadly_jungle_plant : public ScriptedAI
{
    npc_deadly_jungle_plant(Creature* p_Creature) : ScriptedAI(p_Creature) { }

    void InitializeAI() override
    {
        me->SetControlled(true, UnitState::UNIT_STATE_NOT_MOVE);
    }

    void Reset() override
    {
        InitializeAI();
    }
};

/// Goblin Escape Pod 195188
struct gob_goblin_escape_pod_195188 : public GameObjectAI
{
    gob_goblin_escape_pod_195188(GameObject* go) : GameObjectAI(go) { }

    enum eKillcredit
    {
        GoblinRescued = 34748
    };

    void OnBuildDynFlags(uint16& p_DynFlags, Player* p_Player) override
    {
        if (p_Player->GetQuestStatus(eQuests::GoblinEscapePods) != QUEST_STATUS_INCOMPLETE || p_Player->IsCreatureInInaccessibleList(go->GetGUID()))
            p_DynFlags = GO_DYNFLAG_LO_NO_INTERACT;
    }

    void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
    {
        if (!p_Unit)
            return;

        Player *l_Player = p_Unit->ToPlayer();
        if (!l_Player)
            return;

        if (p_State == 2)
        {
            uint32 l_Entry = l_Player->GetQuestObjectiveCounter(eObjectives::GoblinsRescued) == 0 ? eCreatures::TradePrincePod : eCreatures::GoblinSurvivor;
            l_Player->SummonCreature(l_Entry, l_Player->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 20000, 0, l_Player->GetGUID());
            l_Player->KilledMonsterCredit(eKillcredit::GoblinRescued);

            uint32 l_Guid = go->GetDBTableGUIDLow();
            l_Player->AddDelayedEvent([l_Player, l_Guid]()-> void {
                l_Player->AddObjectInLockoutList(l_Guid, eQuests::GoblinEscapePods, true);
            }, 2000);

            l_Player->AddCreatureInInaccessibleList(go->GetGUID());
        }
    }
};

/// Naga Banner 202133
struct gob_naga_banner_202133 : public GameObjectAI
{
    gob_naga_banner_202133(GameObject* go) : GameObjectAI(go) { }

    void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
    {
        if (!p_Unit)
            return;

        Player *l_Player = p_Unit->ToPlayer();
        if (!l_Player)
            return;

        if (p_State == 2)
        {
            uint32 l_Guid = go->GetDBTableGUIDLow();
            l_Player->AddDelayedEvent([l_Player, l_Guid]()-> void {
                l_Player->AddObjectInLockoutList(l_Guid, eQuests::BilgewaterCartelRepresent, true);
            }, 1000);
        }
    }
};

/// Valve #1 202609
/// Valve #2 202610
/// Valve #3 202611
/// Valve #4 202612
struct gob_lost_isles_valve : public GameObjectAI
{
    gob_lost_isles_valve(GameObject* go) : GameObjectAI(go) { }

    void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
    {
        if (!p_Unit)
            return;

        Player *l_Player = p_Unit->ToPlayer();
        if (!l_Player)
            return;

        if (p_State == 2)
        {
            uint32 l_Guid = go->GetDBTableGUIDLow();
            l_Player->AddDelayedEvent([l_Player, l_Guid]()-> void {
                l_Player->AddObjectInLockoutList(l_Guid, eQuests::ReleaseTheValves, true);
            }, 1000);
        }
    }
};

/// Land Mine 202472
struct gob_land_mine_202472 : public GameObjectAI
{
    gob_land_mine_202472(GameObject* go) : GameObjectAI(go) { }

    void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
    {
        if (!p_Unit)
            return;

        Player *l_Player = p_Unit->ToPlayer();
        if (!l_Player)
            return;

        if (p_State == 2)
            l_Player->AddObjectInLockoutList(go->GetDBTableGUIDLow(), eQuests::MineDisposalTheGoblinWay, true);
    }
};

/// Don't Go Into The Light!: Quest Complete - 69013
class spell_dont_go_into_the_light_complete : public SpellScript
{
    PrepareSpellScript(spell_dont_go_into_the_light_complete);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        l_Target->RemoveAura(eSpells::NearDeath);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dont_go_into_the_light_complete::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/// Nitro-Potassium Bananas - 67917
class spell_nitro_potassium_bananas : public SpellScript
{
    PrepareSpellScript(spell_nitro_potassium_bananas);

    enum eKillcredits
    {
        MonkeysFed = 35760
    };

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        Unit* l_Caster = GetCaster();
        if (!l_Target || !l_Caster)
            return;

        if (Player* l_Player = l_Caster->ToPlayer())
            l_Player->KilledMonsterCredit(eKillcredits::MonkeysFed);

        l_Target->CastSpell(l_Target, eSpells::ExplodingBananas, false);

        l_Target->AddDelayedEvent([l_Target]() -> void
        {
            l_Target->Kill(l_Target);
        }, 5000);
    }

    SpellCastResult CheckTarget()
    {
        Unit* l_Target = GetExplTargetUnit();

        if (l_Target->GetEntry() != eCreatures::BombThrowingMonkey)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_nitro_potassium_bananas::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnCheckCast += SpellCheckCastFn(spell_nitro_potassium_bananas::CheckTarget);
    }
};

/// KTC Snapflash - 68280
class spell_ktc_snapflash : public SpellScript
{
    PrepareSpellScript(spell_ktc_snapflash);

    enum eCreatures
    {
        Bunny1 = 37872,
        Bunny2 = 37895,
        Bunny3 = 37896,
        Bunny4 = 37897
    };

    enum eObjectives
    {
        Bunny1Completed = 264961,
        Bunny2Completed = 264962,
        Bunny3Completed = 264963,
        Bunny4Completed = 264964
    };

    void FilterTargets(std::list<WorldObject*>& p_Targets)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return;

        p_Targets.remove_if([l_Player](WorldObject* p_Target) -> bool
        {
            if (!p_Target)
                return true;

            uint32 l_Entry = p_Target->GetEntry();
            if (l_Entry == eCreatures::Bunny1 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny1Completed) == 0)
                return false;
            if (l_Entry == eCreatures::Bunny2 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny2Completed) == 0)
                return false;
            if (l_Entry == eCreatures::Bunny3 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny3Completed) == 0)
                return false;
            if (l_Entry == eCreatures::Bunny4 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny4Completed) == 0)
                return false;

            return true;
        });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        Unit* l_Caster = GetCaster();
        if (!l_Target || !l_Caster)
            return;

        l_Caster->CastSpell(l_Target, eSpells::KTCSnapflash, true);
        l_Caster->CastSpell(l_Target, eSpells::BindSight, true);
        l_Caster->CastSpell(l_Caster, eSpells::ScreenEffect, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ktc_snapflash::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_ktc_snapflash::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/// Capturing The Unknown: Player's Bind Sight - 70641
class spell_lost_isles_bind_sight : public SpellScriptLoader
{
    public:
        spell_lost_isles_bind_sight() : SpellScriptLoader("spell_lost_isles_bind_sight") { }

        enum eCreatures
        {
            Bunny1 = 37872,
            Bunny2 = 37895,
            Bunny3 = 37896,
            Bunny4 = 37897
        };

        enum eObjectives
        {
            Bunny1Completed = 264961,
            Bunny2Completed = 264962,
            Bunny3Completed = 264963,
            Bunny4Completed = 264964
        };

        class spell_lost_isles_bind_sight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lost_isles_bind_sight_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                p_Targets.remove_if([l_Player](WorldObject* p_Target) -> bool
                {
                    if (!p_Target)
                        return true;

                    uint32 l_Entry = p_Target->GetEntry();
                    if (l_Entry == eCreatures::Bunny1 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny1Completed) == 0)
                        return false;
                    if (l_Entry == eCreatures::Bunny2 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny2Completed) == 0)
                        return false;
                    if (l_Entry == eCreatures::Bunny3 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny3Completed) == 0)
                        return false;
                    if (l_Entry == eCreatures::Bunny4 && l_Player->GetQuestObjectiveCounter(eObjectives::Bunny4Completed) == 0)
                        return false;

                    return true;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lost_isles_bind_sight_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_NEARBY_ENEMY);
            }
        };

        class spell_lost_isles_bind_sight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_lost_isles_bind_sight_AuraScript);

            void HandleOnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Caster->RemoveAura(eSpells::ScreenEffect);
                l_Player->KilledMonsterCredit(l_Target->GetEntry());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_lost_isles_bind_sight_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_BIND_SIGHT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lost_isles_bind_sight_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_lost_isles_bind_sight_AuraScript();
        }
};

/// Weed Whacker - 68211
class spell_lost_isles_weed_whacker : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_weed_whacker);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        l_Target->CastSpell(l_Target, eSpells::WeedWhackerMain, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_weed_whacker::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/// Weed Whacker - 68212
class spell_lost_isles_weed_whacker_main : public AuraScript
{
    PrepareAuraScript(spell_lost_isles_weed_whacker_main);

    void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        l_Caster->CastSpell(l_Caster, eSpells::WeedWhackerSummonBunny, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_lost_isles_weed_whacker_main::HandleAfterApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
    }
};

/// Cluster Cluck: Remote Control Fireworks Visual - 74177
class spell_lost_isles_remote_control_fireworks : public AuraScript
{
    PrepareAuraScript(spell_lost_isles_remote_control_fireworks);

    void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        l_Caster->AddDelayedEvent([l_Caster]()->void
        {
            Creature* l_Creature = l_Caster->ToCreature();
            if (!l_Creature)
                return;

            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(0, Positions::ClusterCluck);
            l_Creature->DespawnOrUnsummon(5000);
        }, 1000);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_lost_isles_remote_control_fireworks::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

/// Weed Whacker - 68211
class spell_lost_isles_remote_control_fireworks_dummy : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_remote_control_fireworks_dummy);

    enum eKillCredits
    {
        WildCluckerCaptured = 38117
    };

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        Unit* l_Caster = GetCaster();
        if (!l_Target || !l_Caster)
            return;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return;

        l_Target->CastSpell(l_Target, eSpells::RemoteControlVisual, true);
        l_Player->KilledMonsterCredit(eKillCredits::WildCluckerCaptured);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_remote_control_fireworks_dummy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/// Super Booster Rocket Boots - 72891
class spell_super_booster_rocket_boots : public SpellScript
{
    PrepareSpellScript(spell_super_booster_rocket_boots);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        l_Target->CastSpell(l_Target, eSpells::SuperBoosterSummon, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_super_booster_rocket_boots::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/// Bootzooka - 73066
class spell_lost_isles_bootzooka : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_bootzooka);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        SetHitDamage(GetHitDamage() * 8);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_bootzooka::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

/// Shark spells 38318
class spell_lost_isles_shark : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_shark);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        SetHitDamage(GetHitDamage() * 15);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_shark::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

/// Cannon Blast - 72207
class spell_lost_isles_cannon_blast : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_cannon_blast);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        SetHitDamage(GetHitDamage() * 100);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_cannon_blast::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

/// Volcanic Breath - 73097
class spell_lost_isles_volcanic_breath : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_volcanic_breath);

    void FilterTargets(std::list<WorldObject*>& p_Targets)
    {
        Unit* l_Caster = GetCaster();

        if (l_Caster == nullptr)
            return;

        p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
        {
            Position const l_Pos = { p_Object->GetPositionX(), p_Object->GetPositionY(), p_Object->GetPositionZ(), 0.314f };
            if (p_Object == nullptr || !l_Caster->HasInArc(M_PI / 6, &l_Pos ))
                return true;

            return false;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lost_isles_volcanic_breath::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
    }
};

/// Kaja'Cola Zero-One - 73583
class spell_lost_isles_kaja_cola_zero_one : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_kaja_cola_zero_one);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        Unit* l_Caster = GetCaster();
        if (!l_Target || !l_Caster)
            return;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return;

        l_Target->CastSpell(l_Target, eSpells::KajaColaZeroOne, false);

        uint32 l_Entry = l_Target->GetEntry();
        l_Player->AddDelayedEvent([l_Player, l_Entry]() -> void
        {
            l_Player->KilledMonsterCredit(l_Entry);
        }, 1500);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_kaja_cola_zero_one::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/// Kaja'Cola Zero-One - 73599
class spell_lost_isles_kaja_cola_zero_one_dummy : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_kaja_cola_zero_one_dummy);

    void HandleAfterCast()
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        switch (l_Caster->GetEntry())
        {
            case eCreatures::GoblinSurvivorMine:
            {
                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Creature)
                    return;


                float l_X, l_Y, l_Z;
                l_Creature->GetRandomContactPoint(l_Creature, l_X, l_Y, l_Z, 1.0f, 10.0f);

                if (l_Creature->IsAIEnabled)
                    l_Creature->AI()->Talk(0);

                l_Creature->DespawnOrUnsummon(3000);
                l_Creature->GetMotionMaster()->MovePoint(1, l_X, l_Y, l_Z);
                break;
            }
            default:
                break;
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_lost_isles_kaja_cola_zero_one_dummy::HandleAfterCast);
    }
};

/// Blastshadow's Soulstone - 73702
class spell_lost_isles_blastshadow_soulstone : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_blastshadow_soulstone);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return;

        l_Player->KilledMonsterCredit(39276);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_blastshadow_soulstone::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/// Illegal Contact - 73998
class spell_lost_isles_illegal_contact : public SpellScript
{
    PrepareSpellScript(spell_lost_isles_illegal_contact);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        SetHitDamage(GetHitDamage() * 15);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lost_isles_illegal_contact::HandleDamage, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
    }
};

class playerscript_lost_isles_scripts : public PlayerScript
{
    public:
        playerscript_lost_isles_scripts() : PlayerScript("playerscript_lost_isles_scripts") {}

        void OnQuestAccept(Player * p_Player, const Quest * p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::BackToAggra:
                {
                    if (Creature* l_Creature = p_Player->SummonCreature(eCreatures::OrcScout, Positions::ScoutPos1))
                        l_Creature->AI()->DoAction(1);
                    if (Creature* l_Creature = p_Player->SummonCreature(eCreatures::OrcScout, Positions::ScoutPos2))
                        l_Creature->AI()->DoAction(2);

                    break;
                }
                case eQuests::SurrenderOrElse:
                {
                    p_Player->SummonCreature(eCreatures::Ace, Positions::AceSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
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
                case eQuests::WarchiefsRevenge:
                {
                    if (Vehicle* l_Vehicle = p_Player->GetVehicle())
                        if (Unit* l_Base = l_Vehicle->GetBase())
                            if (l_Base->ToCreature() && l_Base->IsAIEnabled)
                                l_Base->ToCreature()->AI()->DoAction(1);

                    std::string l_Text;
                    if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(40144))
                        l_Text = GetBroadcastTextValue(l_BroadcastText, p_Player->GetSession()->GetSessionDb2Locale(), p_Player->getGender());

                    if (l_Text.empty())
                        return;

                    WorldPacket l_Packet;
                    p_Player->BuildPlayerChat(&l_Packet, p_Player->GetGUID(), ChatMsg::CHAT_MSG_RAID_BOSS_WHISPER, l_Text, 0);
                    p_Player->GetSession()->SendPacket(&l_Packet);

                    break;
                }
                case eQuests::Volcanoth:
                {
                    if (Creature* l_Sassy = p_Player->FindNearestCreature(eCreatures::SassyHardwrench, 100.0f))
                        if (l_Sassy->IsAIEnabled)
                            l_Sassy->AI()->Talk(0, p_Player->GetGUID());

                    break;
                }
                case eQuests::WarchiefEmissary:
                {
                    WorldLocation l_NewHomeBind = WorldLocation(1, 1572.8059f, -4437.7651f, 1.8782f);
                    p_Player->SetHomebind(l_NewHomeBind, 8618);
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
                case eQuests::ClusterCluck:
                {
                    if (Creature* l_Creature = p_Player->FindNearestCreature(eCreatures::BammMegabomb, 10.0f))
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->Talk(0, p_Player->GetGUID());

                    break;
                }
                case eQuests::TradingUp:
                {
                    if (Creature* l_Creature = p_Player->FindNearestCreature(eCreatures::HobartGrapplehammer, 10.0f))
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->Talk(2, p_Player->GetGUID());

                    break;
                }
                case eQuests::IrresistiblePoolPony:
                {
                    auto const& l_SummonList = p_Player->GetSummonList(eCreatures::NagaHatchlingSummon);
                    for (auto const& l_Entry : *l_SummonList)
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*p_Player, l_Entry))
                            l_Creature->DespawnOrUnsummon(1000);

                    break;
                }
                case eQuests::ZombiesvsSuperBoosterRocketBoots:
                {
                    p_Player->ExitVehicle();
                    break;
                }
                case eQuests::KajaColaGivesYouIDEASTM:
                {
                    p_Player->CastSpell(p_Player, eSpells::SummonAssistantGreely, true);

                    p_Player->AddDelayedEvent([p_Player]() -> void
                    {
                        if (Creature* l_Creature = p_Player->FindDetectableNearestCreature(eCreatures::AssistantGreely, 10.0f))
                            l_Creature->AI()->Talk(0, p_Player->GetGUID());
                    }, 1000);
                    break;
                }
                default:
                    break;
            }
        }

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 /*p_OldZoneID*/, uint32 p_NewAreaID) override
        {
            if (p_NewAreaID == 4924 && p_Player->GetQuestStatus(eQuests::KajaColaGivesYouIDEASTM) == QUEST_STATUS_REWARDED && p_Player->GetQuestStatus(eQuests::WildMineCartRide) == QUEST_STATUS_NONE)
            {
                GuidList* l_SummonList = p_Player->GetSummonList(eCreatures::AssistantGreely);
                if (!l_SummonList->empty())
                    return;

                p_Player->CastSpell(p_Player, eSpells::SummonAssistantGreely, true);
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_zone_lost_isles()
{
    /// Creatures
    RegisterCreatureAI(npc_geargrinder_gizmo_36600);
    RegisterCreatureAI(doc_zapnozzle_36608);
    new npc_frightened_miner_35813();
    RegisterCreatureAI(npc_weed_whacker_bunny_35903);
    RegisterCreatureAI(orc_scout_35918);
    RegisterCreatureAI(npc_bastia_36585);
    RegisterCreatureAI(npc_gyrochoppa_36143);
    RegisterCreatureAI(npc_alliance_sailor_36176);
    RegisterCreatureAI(npc_cyclone_of_the_elements_36178);
    RegisterCreatureAI(npc_sling_rocket_36505);
    RegisterCreatureAI(npc_wild_clucker_egg_38195);
    RegisterCreatureAI(npc_spiny_raptor_38187);
    RegisterCreatureAI(npc_mechachicken_38224);
    RegisterCreatureAI(npc_mechashark_x_steam_38318);
    RegisterCreatureAI(npc_naga_hatchling);
    RegisterCreatureAI(npc_naga_hatchling_44589);
    RegisterCreatureAI(npc_ace_38455);
    RegisterCreatureAI(npc_faceless_of_the_deep_38448);
    RegisterCreatureAI(npc_bc_eliminator_38526);
    RegisterCreatureAI(npc_oomlot_warrior_38531);
    RegisterCreatureAI(npc_oomlot_shaman_38644);
    RegisterCreatureAI(npc_volcanoth_38855);
    RegisterCreatureAI(npc_flying_bomber_38929);
    RegisterCreatureAI(npc_flying_bomber_38918);
    RegisterCreatureAI(npc_bastia_39152);
    RegisterCreatureAI(npc_morale_boost);
    RegisterCreatureAI(npc_mine_cart_39329);
    RegisterCreatureAI(npc_footbomb_uniform_47956);
    RegisterCreatureAI(npc_captured_goblin_39456);
    RegisterCreatureAI(npc_sassy_ship_38387);
    RegisterCreatureAI(npc_raptor_86883);
    RegisterCreatureAI(npc_deadly_jungle_plant);

    /// GameObjects
    RegisterGameObjectAI(gob_goblin_escape_pod_195188);
    RegisterGameObjectAI(gob_naga_banner_202133);
    RegisterGameObjectAI(gob_land_mine_202472);
    RegisterGameObjectAI(gob_lost_isles_valve);

    /// Spells
    RegisterSpellScript(spell_dont_go_into_the_light_complete);
    RegisterSpellScript(spell_nitro_potassium_bananas);
    RegisterSpellScript(spell_ktc_snapflash);
    new spell_lost_isles_bind_sight();
    RegisterSpellScript(spell_lost_isles_weed_whacker);
    RegisterAuraScript(spell_lost_isles_weed_whacker_main);
    RegisterAuraScript(spell_lost_isles_remote_control_fireworks);
    RegisterSpellScript(spell_lost_isles_remote_control_fireworks_dummy);
    RegisterSpellScript(spell_super_booster_rocket_boots);
    RegisterSpellScript(spell_lost_isles_bootzooka);
    RegisterSpellScript(spell_lost_isles_volcanic_breath);
    RegisterSpellScript(spell_lost_isles_kaja_cola_zero_one);
    RegisterSpellScript(spell_lost_isles_kaja_cola_zero_one_dummy);
    RegisterSpellScript(spell_lost_isles_blastshadow_soulstone);
    RegisterSpellScript(spell_lost_isles_shark);
    RegisterSpellScript(spell_lost_isles_cannon_blast);
    RegisterSpellScript(spell_lost_isles_illegal_contact);

    /// Playerscripts
    new playerscript_lost_isles_scripts();
}
#endif
