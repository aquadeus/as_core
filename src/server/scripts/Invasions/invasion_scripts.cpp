////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "CreatureGroups.h"

/// Commander Jarod Shadowsong - 118183
class npc_commander_jarod_shadowsong_118183 : public CreatureScript
{
public:
    npc_commander_jarod_shadowsong_118183() : CreatureScript("npc_commander_jarod_shadowsong_118183") { }

    struct npc_commander_jarod_shadowsong_118183AI : public ScriptedAI
    {
        npc_commander_jarod_shadowsong_118183AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        G3D::Vector3 const Path_CenarionDefenderFirst[9] =
        {
            { 3060.574f, 7440.575f, 61.15076f },
            { 3027.643f, 7428.453f, 84.64363f },
            { 3019.113f, 7458.991f, 102.7124f },
            { 3043.831f, 7492.986f, 121.1733f },
            { 3070.966f, 7514.611f, 154.5914f },
            { 3102.756f, 7526.091f, 200.5525f },
            { 3140.075f, 7488.096f, 236.5943f },
            { 3149.76f, 7437.845f, 240.9736f },
            { 3160.81f, 7410.596f, 250.8833f }
        };

        G3D::Vector3 const Path_CenarionDefenderSecond[9] =
        {
            { 3055.075f, 7448.143f, 66.17311f },
            { 3028.224f, 7440.213f, 94.25181f },
            { 3026.59f, 7469.43f, 117.2555f },
            { 3039.232f, 7498.262f, 140.0079f },
            { 3057.971f, 7520.844f, 167.9952f },
            { 3102.478f, 7533.488f, 209.072f },
            { 3137.32f, 7503.7f, 236.5943f },
            { 3136.711f, 7436.183f, 242.3114f },
            { 3155.281f, 7389.57f, 252.2648f }
        };

        enum eQuests
        {
            Quest_AssaultOnValsharah = 45812
        };

        enum eObjectives
        {
            Obj_LegionPointsRepelled = 288650
        };

        enum eKillcredits
        {
            Killcredit_MeetJarodShadowsong = 118237,
        };

        enum eSpells
        {
            Spell_FlyToBlackRockHold = 229829,
            Spell_SwiftFlightForm = 100202
        };

        enum eNpcs
        {
            Npc_CenarionDefender = 91432
        };

        enum ePoints
        {
            Point_CenarionDefenderPathEnd = 1
        };

        Position const CenarionDefenderFirstSpawnPos = { 3069.761f, 7446.523f, 52.38126f, 1.156871f };
        Position const CenarionDefenderSecondSpawnPos = { 3065.869f, 7452.583f, 52.16493f, 0.6373477f };

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player || me->GetExactDist2d(p_Who) > 10.0f ||
                l_Player->GetQuestObjectiveCounter(eObjectives::Obj_LegionPointsRepelled) != 4 ||
                l_Player->GetQuestStatus(eQuests::Quest_AssaultOnValsharah) == QUEST_STATUS_COMPLETE)
                return;

            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_MeetJarodShadowsong);
            Talk(0, l_Player->GetGUID());
        }

        void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_CenarionDefender, CenarionDefenderFirstSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                l_Creature->SetSpeed(MOVE_RUN, 4.0f);
                l_Creature->CastSpell(l_Creature, eSpells::Spell_SwiftFlightForm, true);
                l_Creature->SendPlaySpellVisualKit(29736, 0, 0);
                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CenarionDefenderPathEnd, Path_CenarionDefenderFirst, 9);
                l_Creature->DespawnOrUnsummon(15000);
            }

            if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_CenarionDefender, CenarionDefenderSecondSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                l_Creature->SetSpeed(MOVE_RUN, 4.0f);
                l_Creature->CastSpell(l_Creature, eSpells::Spell_SwiftFlightForm, true);
                l_Creature->SendPlaySpellVisualKit(29736, 0, 0);
                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CenarionDefenderPathEnd, Path_CenarionDefenderSecond, 9);
                l_Creature->DespawnOrUnsummon(15000);
            }

            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->CastSpell(p_Player, eSpells::Spell_FlyToBlackRockHold, true);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_commander_jarod_shadowsong_118183AI(p_Creature);
    }
};

/// Portal Bunny - 115585
class npc_portal_bunny : public CreatureScript
{
public:
    npc_portal_bunny() : CreatureScript("npc_portal_bunny") { }

    struct npc_portal_bunnyAI : public ScriptedAI
    {
        npc_portal_bunnyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eNpcs
        {
            Npc_GeneralZeonar = 115603
        };

        enum eSpells
        {
            Spell_FelfireGate     = 229742,
            Spell_FelInfusion     = 229875
        };

        enum eEvents
        {
            Event_SummonEventNpcs = 1
        };

        EventMap m_Events;
        uint8 m_HarvestersDied = 0;

        void Reset() override
        {
            me->CastSpell(me, eSpells::Spell_FelfireGate, true);
            me->SummonCreatureGroup(0);
        }

        void JustSummoned(Creature* p_Summon) override
        {
            p_Summon->SetPhaseMask(0 ,true);
            p_Summon->SetInPhase(8006, true, true);

            if (p_Summon->GetEntry() == eNpcs::Npc_GeneralZeonar)
                p_Summon->SetInPhase(8003, true, true);

            if (p_Summon->GetEntry() != eNpcs::Npc_GeneralZeonar)
                p_Summon->CastSpell(me, eSpells::Spell_FelInfusion, true);
        }

        void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
        {
            ++m_HarvestersDied;

            if (m_HarvestersDied == 3)
            {
                me->RemoveAllAuras();
                me->SummonCreature(eNpcs::Npc_GeneralZeonar, me->GetPosition(), TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
            }

            if (p_Summon->GetEntry() == eNpcs::Npc_GeneralZeonar)
            {
                m_Events.ScheduleEvent(eEvents::Event_SummonEventNpcs, 30000);
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_SummonEventNpcs:
            {
                me->CastSpell(me, eSpells::Spell_FelfireGate, true);
                me->SummonCreatureGroup(0);
                m_HarvestersDied = 0;
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_portal_bunnyAI(p_Creature);
    }
};

/// Vethir - 116868
class npc_vethir_116868 : public CreatureScript
{
public:
    npc_vethir_116868() : CreatureScript("npc_vethir_116868") { }

    struct npc_vethir_116868AI : public ScriptedAI
    {
        npc_vethir_116868AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eValues
        {
            Quest_AssaultOnStormheim = 45839,
            Obj_LegionPointsRepelled = 288805,
            Killcredit_MeetWithVethir = 116868,
            Spell_RideVethir = 232855,
            Killcredit_BeginTheAssault = 116868
        };

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player || me->GetExactDist2d(p_Who) > 10.0f ||
                l_Player->GetQuestObjectiveCounter(eValues::Obj_LegionPointsRepelled) != 4 ||
                l_Player->GetQuestStatus(eValues::Quest_AssaultOnStormheim) == QUEST_STATUS_COMPLETE)
                return;

            l_Player->KilledMonsterCredit(eValues::Killcredit_MeetWithVethir);
            Talk(0, l_Player->GetGUID());
        }

        void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
        {
            Talk(1, p_Player->GetGUID());
        }

        void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            p_Player->CastSpell(p_Player, eValues::Spell_RideVethir, true);
            p_Player->KilledMonsterCredit(eValues::Killcredit_BeginTheAssault);
            me->DestroyForPlayer(p_Player);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_vethir_116868AI(p_Creature);
    }
};

/// Vethir - 116867
class npc_vethir_116867 : public CreatureScript
{
public:
    npc_vethir_116867() : CreatureScript("npc_vethir_116867") { }

    struct npc_vethir_116867AI : public VehicleAI
    {
        npc_vethir_116867AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        G3D::Vector3 const Path_Vethir_Cycle[32] =
        {
            { 4752.497f, -195.8594f, 33.4059f },
            { 4773.624f, -226.941f, 60.89109f },
            { 4802.207f, -254.0365f, 78.26902f },
            { 4849.207f, -246.0295f, 116.5547f },
            { 4871.058f, -223.6406f, 139.7099f },
            { 4920.887f, -175.6806f, 137.0948f },
            { 4961.844f, -139.7778f, 137.0948f },
            { 4978.81f, -77.40625f, 134.2396f },
            { 4945.995f, -16.22917f, 121.1587f },
            { 4914.168f, 53.13368f, 121.1587f },
            { 4917.328f, 125.8663f, 121.1587f },
            { 4937.827f, 195.8646f, 121.1587f },
            { 4954.713f, 262.2813f, 121.1587f },
            { 4959.983f, 334.4358f, 136.055f },
            { 4943.181f, 399.8924f, 138.4741f },
            { 4916.554f, 465.4931f, 142.9061f },
            { 4870.898f, 508.967f, 142.9061f },
            { 4799.28f, 492.7118f, 142.9061f },
            { 4750.654f, 422.6111f, 142.9061f },
            { 4733.188f, 363.2951f, 150.1407f },
            { 4746.791f, 286.0816f, 150.1407f },
            { 4803.004f, 221.1337f, 150.1407f },
            { 4825.268f, 136.2882f, 119.6428f },
            { 4825.731f, 57.71701f, 114.5062f },
            { 4804.153f, -5.048611f, 116.8073f },
            { 4758.354f, -40.97049f, 116.8073f },
            { 4704.492f, -50.36285f, 139.1206f },
            { 4663.394f, -90.59375f, 141.7855f },
            { 4702.023f, -134.3976f, 139.928f },
            { 4768.483f, -141.8837f, 131.4215f },
            { 4820.598f, -158.3229f, 132.8685f },
            { 4868.757f, -173.9323f, 135.987f }
        };

        enum eValues
        {
            Point_Vethir_Path = 1
        };

        ObjectGuid m_SummonerGuid;

        void PassengerBoarded(Unit* /*who*/, int8 /*seatId*/, bool apply) override
        {
            if (!apply)
            {
                me->DespawnOrUnsummon();
            }
        }

        void IsSummonedBy(Unit* p_Summoner) override
        {
            me->SetPhaseMask(0, true);
            me->SetInPhase(8001, true, true);
            m_SummonerGuid = p_Summoner->GetGUID();
            p_Summoner->CastSpell(me, VehicleSpells::VEHICLE_SPELL_RIDE_HARDCODED, true);
            me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_Vethir_Path, Path_Vethir_Cycle, 32);
            Talk(0, p_Summoner->GetGUID());
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            if (p_PointId == eValues::Point_Vethir_Path)
            {
                me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_Vethir_Path, Path_Vethir_Cycle, 32);
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_vethir_116867AI(p_Creature);
    }
};

class npc_blightwing_119695 : public CreatureScript
{
public:
    npc_blightwing_119695() : CreatureScript("npc_blightwing_119695") { }

    struct npc_blightwing_119695AI : public VehicleAI
    {
        npc_blightwing_119695AI(Creature* p_Creature) : VehicleAI(p_Creature), summons(me) { }

        enum eValues
        {
            Point_Vythe_Eject = 1,
            Npc_FelCommanderErixtol = 119715,
            Npc_FelboundRitualist = 119716,
            Spell_FelExplosion = 237742
        };

        SummonList summons;
        uint8 m_RitualistsDied = 0;
        ObjectGuid m_FelCommanderErixtolGuid;
        const Position m_DespawnPos = { 3832.786f, 4138.208f, 934.7393f };
        const Position m_EjectPos = { 3875.124f, 4140.947f, 887.193f };

        void Reset() override
        {
            summons.DespawnAll();
            m_RitualistsDied = 0;
            me->SummonCreatureGroup(0);
        }

        void JustSummoned(Creature* p_Summon) override
        {
            summons.Summon(p_Summon);

            switch (p_Summon->GetEntry())
            {
                case eValues::Npc_FelCommanderErixtol:
                {
                    m_FelCommanderErixtolGuid = p_Summon->GetGUID();
                    break;
                }
                default:
                    break;
            }
        }

        void SummonedCreatureDies(Creature* /*p_Summon*/, Unit* /*p_Killer*/) override
        {
            ++m_RitualistsDied;

            if (m_RitualistsDied == 4)
            {
                if (Vehicle* l_Vehicle = me->GetVehicleKit())
                {
                    if (Unit* l_Passenger = l_Vehicle->GetPassenger(0))
                        l_Passenger->ToCreature()->AI()->Talk(0);
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_FelCommanderErixtolGuid))
                {
                    l_Creature->CastSpell(l_Creature, eValues::Spell_FelExplosion, true);
                    l_Creature->DespawnOrUnsummon(1000);
                }

                me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_Vythe_Eject, m_EjectPos);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == eValues::Point_Vythe_Eject)
            {
                if (Vehicle* l_Vehicle = me->GetVehicleKit())
                {
                    if (Unit* l_Passenger = l_Vehicle->GetPassenger(0))
                    {
                        l_Passenger->ExitVehicle();
                        l_Passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_Vythe_Eject, m_DespawnPos);
                        me->DespawnOrUnsummon(5000);
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_blightwing_119695AI(p_Creature);
    }
};

/// Ghostly Stagwing - 117779
class npc_ghostly_stagwing_117779 : public CreatureScript
{
public:
    npc_ghostly_stagwing_117779() : CreatureScript("npc_ghostly_stagwing_117779") { }

    struct npc_ghostly_stagwing_117779AI : public VehicleAI
    {
        npc_ghostly_stagwing_117779AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        G3D::Vector3 const Path_GhostlyStagwing_Cycle[18] =
        {
            { -1149.858f, 6238.115f, 87.68879f },
            { -1157.661f, 6262.238f, 102.2176f },
            { -1188.983f, 6279.978f, 121.5737f },
            { -1245.84f, 6273.672f, 154.345f },
            { -1254.719f, 6275.278f, 165.9998f },
            { -1289.238f, 6267.238f, 179.8718f },
            { -1323.757f, 6259.198f, 193.7439f },
            { -1364.017f, 6259.722f, 195.7627f },
            { -1391.385f, 6239.645f, 197.2999f },
            { -1404.29f, 6202.728f, 204.1108f },
            { -1401.986f, 6160.508f, 204.1108f },
            { -1361.004f, 6130.643f, 204.1108f },
            { -1316.462f, 6131.724f, 204.1108f },
            { -1280.859f, 6159.361f, 204.1108f },
            { -1268.17f, 6204.804f, 204.1108f },
            { -1283.016f, 6241.177f, 204.1108f },
            { -1323.757f, 6259.198f, 193.7439f },
            { -1364.017f, 6259.722f, 195.7627f }
        };

        G3D::Vector3 const Path_GhostlyStagwing_Return[16] =
        {
            { -1655.452f, 6285.239f, 232.5366f },
            { -1404.392f, 6201.972f, 204.173f },
            { -1153.332f, 6118.705f, 175.8095f },
            { -926.9479f, 6035.851f, 175.8095f },
            { -586.1875f, 5934.884f, 175.8095f },
            { -313.6285f, 5835.533f, 220.4864f },
            { -121.6563f, 5827.238f, 211.3078f },
            { 65.51389f, 5828.191f, 156.7284f },
            { 280.1076f, 5855.513f, 151.2979f },
            { 508.4722f, 5865.947f, 157.8281f },
            { 662.2274f, 5874.304f, 196.6673f },
            { 766.1094f, 5883.088f, 193.0371f },
            { 848.9323f, 5908.163f, 172.8089f },
            { 886.2691f, 5944.036f, 156.5567f },
            { 887.3768f, 5982.927f, 148.0821f },
            { 873.8386f, 5991.096f, 142.2509f }
        };

        enum eValues
        {
            Point_GhostlyStagwing_Cycle_Path = 1,
            Point_GhostlyStagwing_Return_Path = 2,
            Quest_AConduitNoMore = 46205,
            Spell_ReturnHome = 234229
        };

        ObjectGuid m_SummonerGuid;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
            p_Summoner->CastSpell(me, VehicleSpells::VEHICLE_SPELL_RIDE_HARDCODED, true);
            me->SetPhaseMask(0, true);
            me->SetInPhase(8001, true, true);
            me->SetInPhase(8005, true, true);
            me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_GhostlyStagwing_Cycle_Path, Path_GhostlyStagwing_Cycle, 18);
        }

        void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eValues::Spell_ReturnHome)
            {
                me->GetMotionMaster()->Clear();
                me->SetSpeed(MOVE_FLIGHT, 5.0f);
                me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_GhostlyStagwing_Return_Path, Path_GhostlyStagwing_Return, 16);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            switch (p_PointId)
            {
            case eValues::Point_GhostlyStagwing_Cycle_Path:
            {
                if (l_Player->GetQuestStatus(eValues::Quest_AConduitNoMore) != QUEST_STATUS_COMPLETE)
                    me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_GhostlyStagwing_Cycle_Path, Path_GhostlyStagwing_Cycle, 18);
                break;
            }
            case eValues::Point_GhostlyStagwing_Return_Path:
            {
                l_Player->ExitVehicle();
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
        return new npc_ghostly_stagwing_117779AI(p_Creature);
    }
};

/// Power Source - 117797
class npc_power_source_117797 : public CreatureScript
{
public:
    npc_power_source_117797() : CreatureScript("npc_power_source_117797") { }

    struct npc_power_source_117797AI : public ScriptedAI
    {
        npc_power_source_117797AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            OverloadEnergy = 234224,
            KillCreditPowerSource = 236536,
            ReturnHome = 234229
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (Vehicle* l_Vehicle = p_Caster->GetVehicleKit())
            {
                if (Unit* l_Passenger = l_Vehicle->GetPassenger(0))
                {
                    if (p_Spell->Id == eSpells::OverloadEnergy && l_Passenger->ToPlayer())
                    {
                        l_Passenger->CastSpell(l_Passenger, eSpells::KillCreditPowerSource, true);
                        l_Passenger->CastSpell(l_Passenger, eSpells::ReturnHome, true);
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_power_source_117797AI(p_Creature);
    }
};

/// Hound of the Legion - 118280
class npc_hound_of_the_legion_118280 : public CreatureScript
{
public:
    npc_hound_of_the_legion_118280() : CreatureScript("npc_hound_of_the_legion_118280") { }

    struct npc_hound_of_the_legion_118280AI : public ScriptedAI
    {
        npc_hound_of_the_legion_118280AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eValues
        {
            Npc_TormentedAncestor = 118300,
            Killcredit_RescueSpirits = 118305,
            Spell_Souldrain = 234987,
            Spell_DrainLife = 204896,
            Event_DrainLife = 1
        };

        EventMap m_Events;
        std::vector<ObjectGuid> m_TormentedAncestorsGuids;

        void SpellHitTarget(Unit* target, SpellInfo const* /*spell*/) override
        {
            if (target->GetEntry() == eValues::Npc_TormentedAncestor)
            {
                m_TormentedAncestorsGuids.push_back(target->GetGUID());
            }
        }

        void JustDied(Unit* killer) override
        {
            if (killer->ToPlayer())
            {
                for (auto itr : m_TormentedAncestorsGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->AI()->SetData(1, 1);
                        killer->ToPlayer()->KilledMonsterCredit(eValues::Killcredit_RescueSpirits);
                    }
                }
            }
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            m_Events.ScheduleEvent(eValues::Event_DrainLife, urand(2000, 5000));
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            if (Creature* l_Creature = me->FindNearestCreature(eValues::Npc_TormentedAncestor, 5.0f, true))
            {
                if (!me->isInCombat() && !l_Creature->HasAura(eValues::Spell_Souldrain))
                {
                    DoCast(eValues::Spell_Souldrain, true);
                }
            }

            switch (m_Events.ExecuteEvent())
            {
            case eValues::Event_DrainLife:
            {
                DoCastVictim(eValues::Spell_DrainLife, false);
                m_Events.ScheduleEvent(eValues::Event_DrainLife, urand(10000, 20000));
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
        return new npc_hound_of_the_legion_118280AI(p_Creature);
    }
};

/// Fel Mortar - 115747
class npc_fel_mortar_115747 : public CreatureScript
{
public:
    npc_fel_mortar_115747() : CreatureScript("npc_fel_mortar_115747") { }

    struct npc_fel_mortar_115747AI : public VehicleAI
    {
        npc_fel_mortar_115747AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        void Reset() override
        {
            me->SetRooted(true);
            me->SetReactState(ReactStates::REACT_PASSIVE);
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
        {
            if (!apply)
            {
                Position l_Pos = me->GetHomePosition();
                who->ToPlayer()->SendTeleportPacket(l_Pos); // Prevent fall under textures
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_fel_mortar_115747AI(p_Creature);
    }
};

/// Lasan Skyhorn - 119676
class npc_lasan_skyhorn_119676 : public CreatureScript
{
public:
    npc_lasan_skyhorn_119676() : CreatureScript("npc_lasan_skyhorn_119676") { }

    struct npc_lasan_skyhorn_119676AI : public ScriptedAI
    {
        npc_lasan_skyhorn_119676AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        G3D::Vector3 const Path_Lasan[3] =
        {
            { 3949.31f, 4163.412f, 898.5798f },
            { 3937.191f, 4150.812f, 890.8067f },
            { 3930.496f, 4144.286f, 887.1487f }
        };

        G3D::Vector3 const Path_SurvivalistFirst[4] =
        {
            { 3968.337f, 4147.84f, 901.8954f },
            { 3955.385f, 4135.643f, 897.9358f },
            { 3942.005f, 4129.747f, 894.3922f },
            { 3928.67f, 4127.943f, 890.8824f }
        };

        G3D::Vector3 const Path_SurvivalistSecond[4] =
        {
            { 3948.976f, 4164.508f, 901.8954f },
            { 3936.795f, 4151.139f, 896.6511f },
            { 3928.531f, 4146.07f, 893.4012f },
            { 3920.05f, 4146.143f, 889.6049f }
        };

        enum eKillcredits
        {
            Killcredit_LasanAndSurvivalistsArrived = 289242,
            Killcredit_SpeakWithLasan = 119676
        };

        enum eNpcs
        {
            Npc_SkyhornSurvivalist = 119193
        };

        enum ePoints
        {
            Point_LasanPathEnd = 1,
            Point_SurvivalistPathEnd = 2
        };

        enum eEvents
        {
            Event_Killcredit = 1,
            Event_DismountSurvivalists = 2
        };

        ObjectGuid m_SummonerGuid;
        std::vector<ObjectGuid> m_SurvivalistsGuids;
        EventMap m_Events;
        Position const m_SurvivalistFirstSpawnPos = { 3994.69f, 4167.44f, 904.067f,  2.976602f };
        Position const m_SurvivalistSecondSpawnPos = { 3986.64f, 4188.29f, 908.435f, 3.528398f };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            if (Creature* l_Creature = p_Summoner->SummonCreature(eNpcs::Npc_SkyhornSurvivalist, m_SurvivalistFirstSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Summoner->GetGUID()))
            {
                m_SurvivalistsGuids.push_back(l_Creature->GetGUID());
                l_Creature->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 70260);
                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_SurvivalistPathEnd, Path_SurvivalistFirst, 4);
            }

            if (Creature* l_Creature = p_Summoner->SummonCreature(eNpcs::Npc_SkyhornSurvivalist, m_SurvivalistSecondSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Summoner->GetGUID()))
            {
                m_SurvivalistsGuids.push_back(l_Creature->GetGUID());
                l_Creature->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 70260);
                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_SurvivalistPathEnd, Path_SurvivalistSecond, 4);
            }

            m_SummonerGuid = p_Summoner->GetGUID();
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, 3);
            me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 75559);
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LasanPathEnd, Path_Lasan, 3);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            switch (p_PointId)
            {
            case ePoints::Point_LasanPathEnd:
            {
                Talk(0, l_Player->GetGUID());
                me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                m_Events.ScheduleEvent(eEvents::Event_DismountSurvivalists, 1500);
                m_Events.ScheduleEvent(eEvents::Event_Killcredit, 5000);
                break;
            }
            default:
                break;
            }
        }

        void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            std::set<uint32> l_DungeonSet;
            l_DungeonSet.insert(1490);
            sLFGMgr->Join(p_Player, 8, l_DungeonSet, "");
            p_Player->KilledMonsterCredit(eKillcredits::Killcredit_SpeakWithLasan);
            p_Player->PlayerTalkClass->SendCloseGossip();
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!me->isSummon())
                return;

            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_DismountSurvivalists:
            {
                for (auto itr : m_SurvivalistsGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                        l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 214);
                    }
                }

                break;
            }
            case eEvents::Event_Killcredit:
            {
                for (auto itr : m_SurvivalistsGuids)
                {
                    if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                    {
                        l_Creature->DespawnOrUnsummon();
                    }
                }

                me->DespawnOrUnsummon();
                l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_LasanAndSurvivalistsArrived);
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_lasan_skyhorn_119676AI(p_Creature);
    }
};

/// Shadowdirge Soul Reaper - 116823
class npc_shadowdirge_soul_reaper_116823 : public CreatureScript
{
public:
    npc_shadowdirge_soul_reaper_116823() : CreatureScript("npc_shadowdirge_soul_reaper_116823") { }

    struct npc_shadowdirge_soul_reaper_116823AI : public ScriptedAI
    {
        npc_shadowdirge_soul_reaper_116823AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eNpcs
        {
            Npc_CaptiveVrykul = 116824
        };

        enum eSpells
        {
            Spell_ShadowManacles = 188818,
            Spell_FelChains = 232717
        };

        enum eEvents
        {
            Event_CastShadowManacles = 1
        };

        EventMap m_Events;
        std::vector<ObjectGuid> m_CaptiveVrykulsGuids;

        void JustReachedHome() override
        {
            if (m_CaptiveVrykulsGuids.size() == 2)
            {
                if (Creature* l_FirstVrykul = ObjectAccessor::GetCreature(*me, m_CaptiveVrykulsGuids[0]))
                {
                    if (Creature* l_SecondVrykul = ObjectAccessor::GetCreature(*me, m_CaptiveVrykulsGuids[1]))
                    {
                        if (me->GetExactDist2d(l_FirstVrykul) < me->GetExactDist2d(l_SecondVrykul))
                        {
                            DoCast(l_FirstVrykul, eSpells::Spell_FelChains, true);
                            l_FirstVrykul->CastSpell(l_SecondVrykul, eSpells::Spell_FelChains, true);
                        }
                        else
                        {
                            DoCast(l_SecondVrykul, eSpells::Spell_FelChains, true);
                            l_SecondVrykul->CastSpell(l_FirstVrykul, eSpells::Spell_FelChains, true);
                        }
                    }
                }
            }
        }

        void Reset() override
        {
            if (!m_CaptiveVrykulsGuids.empty())
                return;

            if (me->GetDefaultMovementType() == MovementGeneratorType::WAYPOINT_MOTION_TYPE)
            {
                if (auto l_Formation = me->GetFormation())
                {
                    for (auto itr : l_Formation->GetMembers())
                    {
                        if (itr.first != me->GetGUID())
                        {
                            m_CaptiveVrykulsGuids.push_back(itr.first);
                            return;
                        }
                    }
                }
            }

            if (int32(me->GetPositionX()) == 3949)
            {
                me->SummonCreatureGroup(0);
            }
            else if (int32(me->GetPositionX()) == 4037)
            {
                me->SummonCreatureGroup(1);
            }
            else if (int32(me->GetPositionX()) == 4227)
            {
                me->SummonCreatureGroup(2);
            }
            else if (int32(me->GetPositionX()) == 4174)
            {
                me->SummonCreatureGroup(3);
            }
            else if (int32(me->GetPositionX()) == 3966 && int32(me->GetPositionY()) == 1544)
            {
                me->SummonCreatureGroup(4);
            }
        }

        void JustSummoned(Creature* p_Summon) override
        {
            p_Summon->SetAIAnimKitId(10816);
            p_Summon->SetInPhase(45390, true, true);
            m_CaptiveVrykulsGuids.push_back(p_Summon->GetGUID());

            if (m_CaptiveVrykulsGuids.size() == 2)
            {
                if (Creature* l_FirstVrykul = ObjectAccessor::GetCreature(*me, m_CaptiveVrykulsGuids[0]))
                {
                    if (Creature* l_SecondVrykul = ObjectAccessor::GetCreature(*me, m_CaptiveVrykulsGuids[1]))
                    {
                        if (me->GetExactDist2d(l_FirstVrykul) < me->GetExactDist2d(l_SecondVrykul))
                        {
                            DoCast(l_FirstVrykul, eSpells::Spell_FelChains, true);
                            l_FirstVrykul->CastSpell(l_SecondVrykul, eSpells::Spell_FelChains, true);
                            me->SetFacingToObject(l_FirstVrykul);
                            l_FirstVrykul->SetFacingToObject(me);
                        }
                        else
                        {
                            DoCast(l_SecondVrykul, eSpells::Spell_FelChains, true);
                            l_SecondVrykul->CastSpell(l_FirstVrykul, eSpells::Spell_FelChains, true);
                            me->SetFacingToObject(l_SecondVrykul);
                            l_SecondVrykul->SetFacingToObject(me);
                        }
                    }
                }
            }
        }

        void JustDied(Unit* killer) override
        {
            for (auto itr : m_CaptiveVrykulsGuids)
            {
                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                {
                    if (m_CaptiveVrykulsGuids.size() == 2)
                    {
                        if (roll_chance_i(50))
                        {
                            l_Creature->AI()->Talk(0, killer->GetGUID());
                        }
                    }
                    else
                    {
                        l_Creature->AI()->Talk(0, killer->GetGUID());
                    }

                    l_Creature->RemoveAllAuras();
                    l_Creature->SetAIAnimKitId(0);
                    l_Creature->GetMotionMaster()->MoveRandom(20.0f);
                    l_Creature->DespawnOrUnsummon(5000);
                }
            }
        }

        void EnterCombat(Unit* victim) override
        {
            if (roll_chance_i(30))
            {
                Talk(0, victim->GetGUID());
            }

            for (auto itr : m_CaptiveVrykulsGuids)
            {
                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, itr))
                {
                    l_Creature->RemoveAllAuras();
                }
            }

            m_Events.ScheduleEvent(eEvents::Event_CastShadowManacles, urand(2000, 5000));
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_CastShadowManacles:
            {
                DoCastVictim(eSpells::Spell_ShadowManacles, false);
                m_Events.ScheduleEvent(eEvents::Event_CastShadowManacles, urand(10000, 20000));
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
        return new npc_shadowdirge_soul_reaper_116823AI(p_Creature);
    }
};

/// Soul Harvester - 116809, 116810, 116811, 116812, 116813
class npc_soul_harvesters : public CreatureScript
{
public:
    npc_soul_harvesters() : CreatureScript("npc_soul_harvesters") { }

    struct npc_soul_harvestersAI : public VehicleAI
    {
        npc_soul_harvestersAI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        enum eSpells
        {
            Spell_DestroyingFirst = 232680,
            Spell_DestroyingSecond = 232677,
            Spell_DestroyingThird = 232678,
            Spell_DestroyingFourth = 232682,
            Spell_DestroyingFifth = 232683,
            Spell_Destruction = 232671
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (Player* l_Player = p_Caster->ToPlayer())
            {
                switch (p_Spell->Id)
                {
                case Spell_DestroyingFirst:
                case Spell_DestroyingSecond:
                case Spell_DestroyingThird:
                case Spell_DestroyingFourth:
                case Spell_DestroyingFifth:
                {
                    me->DestroyForPlayer(l_Player);
                    l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID());
                    break;
                }
                default:
                    break;
                }
            }
        }

        void IsSummonedBy(Unit* /*p_Summoner*/) override
        {
            me->SetInPhase(45390, true, true);
            DoCast(eSpells::Spell_Destruction, false);
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->DespawnOrUnsummon(2000);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_soul_harvestersAI(p_Creature);
    }
};

/// Lord Kormag - 117673
class npc_lord_kormag_117673 : public CreatureScript
{
public:
    npc_lord_kormag_117673() : CreatureScript("npc_lord_kormag_117673") { }

    struct npc_lord_kormag_117673AI : public ScriptedAI
    {
        npc_lord_kormag_117673AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_FelEmpowerment = 234069,
            Spell_Fireball = 239513,
            Spell_BlazingStomp = 205828,
            Spell_RainOfFire = 239514
        };

        enum eEvents
        {
            Event_CastFireball = 1,
            Event_CastBlazingStomp = 2,
            Event_CastRainOfFire = 3
        };

        enum ePoints
        {
            Point_FlyPoint = 1
        };

        EventMap m_Events;
        uint8 m_SoulsDestroyed = 0;
        Position const m_FlyPos = { 4213.314f, 1346.365f, 166.2962f };

        void SetData(uint32 p_ID, uint32 p_Value) override
        {
            m_SoulsDestroyed++;

            if (m_SoulsDestroyed == 2)
            {
                Talk(0);
                me->RemoveAura(eSpells::Spell_FelEmpowerment);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPoint, m_FlyPos);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_FlyPoint)
            {
                me->SetAnimTier(0);
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                me->RemoveFlag(UNIT_FIELD_FLAGS, 2147517248);
                me->SetFlag(UNIT_FIELD_FLAGS, 32832);
                me->SetDisableGravity(false);
            }
        }

        void Reset() override
        {
            m_SoulsDestroyed = 0;
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
            me->SetSwim(true);
            me->SetDisableGravity(true);
            me->SetFlag(UNIT_FIELD_FLAGS, 2147517248);
            me->SummonCreatureGroup(0);
        }

        void JustSummoned(Creature* p_Summon) override
        {
            p_Summon->SetInPhase(45390, true, true);
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            m_Events.ScheduleEvent(eEvents::Event_CastFireball, 5000);
            m_Events.ScheduleEvent(eEvents::Event_CastBlazingStomp, 11000);
            m_Events.ScheduleEvent(eEvents::Event_CastRainOfFire, 13000);
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
            case eEvents::Event_CastFireball:
            {
                DoCastVictim(eSpells::Spell_Fireball, false);
                m_Events.ScheduleEvent(eEvents::Event_CastFireball, urand(10000, 15000));
                break;
            }
            case eEvents::Event_CastBlazingStomp:
            {
                DoCastAOE(eSpells::Spell_BlazingStomp, false);
                m_Events.ScheduleEvent(eEvents::Event_CastBlazingStomp, urand(20000, 30000));
                break;
            }
            case eEvents::Event_CastRainOfFire:
            {
                DoCastVictim(eSpells::Spell_RainOfFire, false);
                m_Events.ScheduleEvent(eEvents::Event_CastRainOfFire, urand(15000, 20000));
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
        return new npc_lord_kormag_117673AI(p_Creature);
    }
};

/// Fel Cannon - 116892
class npc_fel_cannon_116892 : public CreatureScript
{
public:
    npc_fel_cannon_116892() : CreatureScript("npc_fel_cannon_116892") { }

    struct npc_fel_cannon_116892AI : public ScriptedAI
    {
        npc_fel_cannon_116892AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_FelfireShot = 232897
        };

        enum eEvents
        {
            Event_CastFelfireShot = 1
        };

        enum eNpcs
        {
            Npc_HelarjarBerserker = 116888
        };

        EventMap m_Events;
        std::vector<Position> m_BerserkersPosVector;

        void Reset() override
        {
            std::list<Creature*> l_HelarjarBerserkersList;
            me->GetCreatureListWithEntryInGrid(l_HelarjarBerserkersList, eNpcs::Npc_HelarjarBerserker, 200.0f);
            if (l_HelarjarBerserkersList.empty())
                return;

            for (auto itr : l_HelarjarBerserkersList)
            {
                if (itr->isInCombat())
                    m_BerserkersPosVector.push_back(itr->GetPosition());
            }

            m_Events.ScheduleEvent(eEvents::Event_CastFelfireShot, urand(3000, 6000));
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
                case eEvents::Event_CastFelfireShot:
                {
                    if (m_BerserkersPosVector.empty())
                        break;

                    me->CastSpell(m_BerserkersPosVector[urand(0, m_BerserkersPosVector.size() - 1)], eSpells::Spell_FelfireShot, true);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelfireShot, urand(3000, 5000));
                    break;
                }
                default:
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_fel_cannon_116892AI(p_Creature);
    }
};

/// Vethir - 118691
class npc_vethir_118691 : public CreatureScript
{
public:
    npc_vethir_118691() : CreatureScript("npc_vethir_118691") { }

    struct npc_vethir_118691AI : public ScriptedAI
    {
        npc_vethir_118691AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eObjectives
        {
            Obj_VethirLineHeard = 288846
        };

        enum eQuests
        {
            Quest_TheStormsFury = 45406
        };

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player)
                return;

            if (me->GetExactDist2d(p_Who) < 25.0f && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_VethirLineHeard) == 0 &&
                l_Player->HasQuest(eQuests::Quest_TheStormsFury))
            {
                Talk(0, l_Player->GetGUID());
                l_Player->SetQuestObjectiveCounter(eObjectives::Obj_VethirLineHeard, 1);
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_vethir_118691AI(p_Creature);
    }
};

/// Lord Commander Alexius - 118566
class npc_lord_commander_alexius_118566 : public CreatureScript
{
public:
    npc_lord_commander_alexius_118566() : CreatureScript("npc_lord_commander_alexius_118566") { }

    struct npc_lord_commander_alexius_118566AI : public ScriptedAI
    {
        npc_lord_commander_alexius_118566AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_FelDissolveIn = 211762,
            Spell_SummonCommandCenter = 235763,
            Spell_FieryVisual = 218802,
            Spell_FelLightning = 185127,
            Spell_FelBombardment = 242729,
            Spell_GraspingFel = 235784,
            Spell_FelBarrier = 235639,
            Spell_InvisibleManTransform = 152821
        };

        enum eEvents
        {
            Event_CastFelLightning = 1,
            Event_SayThirdLine = 2,
            Event_PlayScene = 3,
            Event_PlayVisual = 4
        };

        EventMap m_Events;
        bool FelBombardmentComplete = false;
        bool RetireEventComplete = false;

        void InitializeAI() override
        {
            DoCast(eSpells::Spell_SummonCommandCenter);
            DoCast(eSpells::Spell_FelDissolveIn, true);
        }

        void Reset() override
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, 2181595200);
            m_Events.Reset();
            FelBombardmentComplete = false;
            RetireEventComplete = false;
        }

        void JustReachedHome() override
        {
            DoCast(eSpells::Spell_SummonCommandCenter);
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            Talk(0);
            m_Events.ScheduleEvent(eEvents::Event_CastFelLightning, urand(3000, 5000));
        }

        void DamageTaken(Unit* attacker, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
        {
            if (me->GetHealthPct() <= 90 && !FelBombardmentComplete)
            {
                Talk(1);
                DoCast(eSpells::Spell_FelBombardment, true);
                FelBombardmentComplete = true;
            }

            if (me->GetHealthPct() <= 85 && !RetireEventComplete)
            {
                me->CastStop();
                m_Events.Reset();
                DoCastAOE(eSpells::Spell_GraspingFel, true);
                DoCast(eSpells::Spell_FelBarrier, true);
                me->SetFlag(UNIT_FIELD_FLAGS, 2148042816);
                me->StopAttack();
                me->SetFacingToObject(attacker);
                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 378);
                Talk(2);
                m_Events.ScheduleEvent(eEvents::Event_SayThirdLine, 4000);
                RetireEventComplete = true;
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
            case eEvents::Event_CastFelLightning:
            {
                DoCastVictim(eSpells::Spell_FelLightning, false);
                m_Events.ScheduleEvent(eEvents::Event_CastFelLightning, urand(5000, 10000));
                break;
            }
            case eEvents::Event_SayThirdLine:
            {
                Talk(3);
                m_Events.ScheduleEvent(eEvents::Event_PlayScene, 2000);
                break;
            }
            case eEvents::Event_PlayScene:
            {
                for (auto itr : me->getThreatManager().getThreatList())
                {
                    if (Unit* l_Unit = itr->getTarget())
                    {
                        if (Player* l_Player = l_Unit->ToPlayer())
                        {
                            l_Player->PlayScene(1793, l_Player);
                        }
                    }
                }

                m_Events.ScheduleEvent(eEvents::Event_PlayVisual, 3000);
                break;
            }
            case eEvents::Event_PlayVisual:
            {
                me->SendPlaySpellVisualKit(61036, 0, 0);
                DoCast(eSpells::Spell_InvisibleManTransform, true);
                me->SetFlag(UNIT_FIELD_FLAGS, 2181595200);
                me->DespawnOrUnsummon(1000);
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
        return new npc_lord_commander_alexius_118566AI(p_Creature);
    }
};

/// Odyn - 118781
class npc_odyn_118781 : public CreatureScript
{
public:
    npc_odyn_118781() : CreatureScript("npc_odyn_118781") { }

    struct npc_odyn_118781AI : public ScriptedAI
    {
        npc_odyn_118781AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eEvents
        {
            Event_CastFlashOfHeaven = 1,
            Event_SetFlags = 2
        };

        enum eSpells
        {
            Spell_FlashOfHeaven = 222109
        };

        enum eConversations
        {
            Conv_QuestBattleForStormheimReward = 4661
        };

        EventMap m_Events;

        void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
        {
            p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
        }

        void IsSummonedBy(Unit* p_Summoner) override
        {
            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_QuestBattleForStormheimReward, p_Summoner, nullptr, p_Summoner->GetPosition()))
                    delete l_Conversation;
            }

            m_Events.ScheduleEvent(eEvents::Event_CastFlashOfHeaven, 6000);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_CastFlashOfHeaven:
            {
                DoCast(eSpells::Spell_FlashOfHeaven, false);
                m_Events.ScheduleEvent(eEvents::Event_SetFlags, 2200);
                break;
            }
            case eEvents::Event_SetFlags:
            {
                me->SetFlag(UNIT_FIELD_FLAGS, 33587968);
                me->DespawnOrUnsummon(9000);
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_odyn_118781AI(p_Creature);
    }
};

/// Captured Icefang Pup - 119211
class npc_captured_icefang_pup_119211 : public CreatureScript
{
public:
    npc_captured_icefang_pup_119211() : CreatureScript("npc_captured_icefang_pup_119211") { }

    struct npc_captured_icefang_pup_119211AI : public ScriptedAI
    {
        npc_captured_icefang_pup_119211AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_Release = 236723
        };

        enum eKillcredits
        {
            Killcredit_IcefangPupsRescued = 119211
        };

        enum ePoints
        {
            Point_MoveEnd = 1
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_Release)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    me->DestroyForPlayer(l_Player);

                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        l_Creature->RemoveAllAuras();
                        l_Creature->SetAIAnimKitId(0);
                        l_Creature->PlayOneShotAnimKitId(10058);
                        l_Creature->DespawnOrUnsummon(4000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                        }, 1000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                        }, 2000);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            Position l_MovePos = l_Creature->GetPosition();
                            l_MovePos.m_positionX = l_MovePos.GetPositionX() + (std::cos(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::cos(l_MovePos.m_orientation) * 10);
                            l_MovePos.m_positionY = l_MovePos.GetPositionY() + (std::sin(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::sin(l_MovePos.m_orientation) * 10);
                            l_MovePos.m_positionZ = l_MovePos.GetPositionZ() + 0;

                            l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_MoveEnd, l_MovePos, true);
                        }, 3000);
                    }

                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_IcefangPupsRescued);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_captured_icefang_pup_119211AI(p_Creature);
    }
};

/// Legion Portal - 119192
class npc_legion_portal_119192 : public CreatureScript
{
public:
    npc_legion_portal_119192() : CreatureScript("npc_legion_portal_119192") { }

    struct npc_legion_portal_119192AI : public ScriptedAI
    {
        npc_legion_portal_119192AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_Attacking = 233794
        };

        void InitializeAI() override
        {
            me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_KIT_ID, 8602);
        }

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_Attacking)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    me->DestroyForPlayer(l_Player);

                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->SetUInt32Value(UNIT_FIELD_STATE_ANIM_KIT_ID, 976);
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        l_Creature->DespawnOrUnsummon(1000);
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_legion_portal_119192AI(p_Creature);
    }
};

/// Observation Notes - 119913
class npc_observation_notes_119913 : public CreatureScript
{
public:
    npc_observation_notes_119913() : CreatureScript("npc_observation_notes_119913") { }

    struct npc_observation_notes_119913AI : public ScriptedAI
    {
        npc_observation_notes_119913AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_Burn = 238517
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_Burn)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    me->DestroyForPlayer(l_Player);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_observation_notes_119913AI(p_Creature);
    }
};

/// Confinement Crystal - 120277
class npc_confinement_crystal_120277 : public CreatureScript
{
public:
    npc_confinement_crystal_120277() : CreatureScript("npc_confinement_crystal_120277") { }

    struct npc_confinement_crystal_120277AI : public ScriptedAI
    {
        npc_confinement_crystal_120277AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_Dispelling = 239806,
            Spell_Dispelled = 239809
        };

        enum eKillcredits
        {
            Killcredit_ConfinementCrystalsDestroyed = 120277
        };

        enum eNpcs
        {
            Npc_ValebearCub = 120285,
            Npc_ValedewFawn = 120279
        };

        enum ePoints
        {
            Point_MoveEnd = 1
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_Dispelling)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    me->DestroyForPlayer(l_Player);
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_ConfinementCrystalsDestroyed);

                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Spell_Dispelled, true);
                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        l_Creature->DespawnOrUnsummon();
                    }

                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_ValebearCub, 10.0f);
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_ValedewFawn, 10.0f);

                    if (!l_CreatureList.empty())
                    {
                        for (auto itr : l_CreatureList)
                        {
                            if (itr->isSummon())
                                continue;

                            itr->DestroyForPlayer(l_Player);

                            if (Creature* l_Creature = l_Player->SummonCreature(itr->GetEntry(), itr->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Creature->RemoveAllAuras();
                                l_Creature->PlayOneShotAnimKitId(4486);
                                l_Creature->DespawnOrUnsummon(4000);

                                l_Creature->AddDelayedEvent([l_Creature]() -> void
                                {
                                    Position l_MovePos = l_Creature->GetPosition();
                                    l_MovePos.m_positionX = l_MovePos.GetPositionX() + (std::cos(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::cos(l_MovePos.m_orientation) * 10);
                                    l_MovePos.m_positionY = l_MovePos.GetPositionY() + (std::sin(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::sin(l_MovePos.m_orientation) * 10);
                                    l_MovePos.m_positionZ = l_MovePos.GetPositionZ() + 0;

                                    l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_MoveEnd, l_MovePos, true);
                                }, 3000);
                            }
                        }
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_confinement_crystal_120277AI(p_Creature);
    }
};

/// Fel Corruptor - 120151
class npc_fel_corruptor_120151 : public CreatureScript
{
public:
    npc_fel_corruptor_120151() : CreatureScript("npc_fel_corruptor_120151") { }

    struct npc_fel_corruptor_120151AI : public ScriptedAI
    {
        npc_fel_corruptor_120151AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_Destroying = 239431
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_Destroying)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    me->DestroyForPlayer(l_Player);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_fel_corruptor_120151AI(p_Creature);
    }
};

/// Legion Cage - 267603, 267634, 267635, 267638
class gob_legion_cage_q45786 : public GameObjectScript
{
public:
    gob_legion_cage_q45786() : GameObjectScript("gob_legion_cage_q45786") { }

    enum eValues
    {
        Killcredit_RescueSpirits = 118305,
        Npc_TormentedAncestor = 118291,
    };

    struct gob_legion_cage_q45786AI : public GameObjectAI
    {
        gob_legion_cage_q45786AI(GameObject* go) : GameObjectAI(go) { }

        void OnLootStateChanged(uint32 state, Unit* unit) override
        {
            if (state == 2 && unit->ToPlayer())
            {
                if (Creature* l_Creature = go->FindNearestCreature(eValues::Npc_TormentedAncestor, 5.0f, true))
                {
                    l_Creature->AI()->SetData(1, 1);
                    unit->ToPlayer()->KilledMonsterCredit(eValues::Killcredit_RescueSpirits);
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new gob_legion_cage_q45786AI(go);
    }
};

/// Fel Bombardment - 242728
class spell_gen_alexius_fel_bombardment : public SpellScriptLoader
{
public:
    spell_gen_alexius_fel_bombardment() : SpellScriptLoader("spell_gen_alexius_fel_bombardment") { }

    class spell_gen_alexius_fel_bombardment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_alexius_fel_bombardment_SpellScript);

        enum eSpells
        {
            Spell_FelBombardmentProjectile = 235986
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            WorldLocation const* l_Wl = GetExplTargetDest();
            if (!l_Caster || !l_Wl)
                return;

            l_Caster->PlayOrphanSpellVisual(G3D::Vector3(4813.21f, 125.808f, 131.948f), G3D::Vector3(0, 0, 0), G3D::Vector3(l_Wl->GetPositionX(), l_Wl->GetPositionY(), l_Wl->GetPositionZ()), 64309, 3.0f, 0, true, 0.0f);
            l_Caster->CastSpell(l_Wl, eSpells::Spell_FelBombardmentProjectile, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_gen_alexius_fel_bombardment_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_alexius_fel_bombardment_SpellScript();
    }
};

class playerscript_invasion_scripts : public PlayerScript
{
public:
    playerscript_invasion_scripts() : PlayerScript("playerscript_invasion_scripts") {}

    G3D::Vector3 const Path_CenarionDefender[5] =
    {
        { 3285.365f, 7254.462f, 308.5165f },
        { 3282.4f, 7296.81f, 302.653f },
        { 3279.435f, 7339.158f, 296.7895f },
        { 3278.567f, 7350.523f, 294.0218f },
        { 3277.823f, 7354.652f, 292.0648f }
    };

    G3D::Vector3 const Path_VioletwingHippogryph[3] =
    {
        { 3285.486f, 7344.984f, 299.7477f },
        { 3282.527f, 7349.643f, 297.4062f },
        { 3280.776f, 7353.708f, 292.3394f }
    };

    enum eQuests
    {
        // Valsharah
        Quest_AssaultOnValsharah = 45812,
        Quest_HoldingTheRamparts = 44789,
        Quest_BattleForValsharah = 45856,
        // Highmountain
        Quest_AssaultOnHighmountain = 45840,
        Quest_HoldingOurGround = 45572,
        Quest_BattleForHighmountain = 46182,
        // Azsuna
        Quest_AssaultOnAzsuna = 45838,
        Quest_PresenceOfPower = 45795,
        Quest_AConduitNoMore = 46205,
        Quest_BattleForAzsuna = 46199,
        // Stormheim
        Quest_AssaultOnStormheim = 45839,
        Quest_TheStormsFury = 45406,
        Quest_BattleForStormheim = 46110
    };

    enum eObjectives
    {
        // Valsharah
        Obj_ValsharahLegionPointsRepelled = 288650,
        Obj_PassageAcquired = 288174,
        Obj_GeneralZeonarSlain = 287027,
        Obj_LegionForcesOnBlackRookHoldRepelled = 288175,
        Obj_ReachTheTopOfBlackRookHold = 288177,
        // Highmountain
        Obj_ThunderTotemNativeRallied = 288287,
        Obj_DemonForces = 289223,
        Obj_IncineratorVytheSlain = 289239,
        Obj_InjuredProtectorRescued = 288153,
        Obj_DemonsSlain = 288192,
        Obj_EnergyCrystalDestroyed = 289282,
        // Azsuna
        Obj_AzsunaLegionPointsRepelled = 289018,
        Obj_PortalKeeperVthaalSlain = 288543,
        Obj_ZorthaduunSlain = 288544,
        // Stormheim
        Obj_StormheimLegionPointsRepelled = 288805,
        Obj_BurningLegionInvadersDefeated = 287945,
        Obj_CommandCenterSlain = 289072
    };

    enum eKillcredits
    {
        Killcredit_ReachTheTopOfBlackRookHold = 117311
    };

    enum eSpells
    {
        // Valsharah
        Spell_ToBlackRookConversation = 234697,
        Spell_PassageConversation = 229814,
        Spell_LegionForcesConversation = 229935,
        Spell_SwiftFlightForm = 100202,
        // Highmountain
        Spell_HighmountainFourQuestsCompleteConversation = 239115,
        Spell_NeedToDefenceThunderTotemConversation = 238557,
        Spell_ThunderTotemDefencedConversation = 233528,
        Spell_SkyhornProtection = 237606,
        // Azsuna
        Spell_FourQuestsCompleteConversation = 236125,
        // Stormheim
        Spell_AlexiusDefeated = 235848,
        Spell_FourthQuestCompletedConversation = 235690,
        Spell_SafeLanding = 232856,
        Spell_FlightEscape = 235541,
        Spell_KnockedOut = 235596
    };

    enum eConversations
    {
        Conv_PortalKeeperVthaalSlain = 4303,
        Conv_ZorthaduunSlain = 4304,
        Conv_LegionConduitDestroyed = 4518
    };

    enum eNpcs
    {
        Npc_CenarionDefender = 118405,
        Npc_VioletwingHippogryph = 115721,
        Npc_Lasan = 119676
    };

    enum ePoints
    {
        Point_CenarionDefenderPathEnd = 1,
        Point_VioletwingHippogryphPathEnd = 2
    };

    Position const m_CenarionDefenderSpawnPos = { 3282.123f, 7300.771f, 302.1092f, 1.640574f };
    Position const m_VioletwingHippogryphSpawnPos = { 3297.832275f, 7300.684082f, 302.109192f, 1.852526f };
    Position const m_TopOfBlackRookHoldPos = { 3247.877930f, 7370.711426f, 292.173523f };
    Position const m_LasanSpawnPos = { 3979.4f, 4193.14f, 910.122f, 3.575699f };

    void OnQuestComplete(Player* p_Player, const Quest* p_Quest) override
    {
        switch (p_Quest->GetQuestId())
        {
        case eQuests::Quest_AssaultOnHighmountain:
        {
            p_Player->DelayedCastSpell(p_Player, eSpells::Spell_HighmountainFourQuestsCompleteConversation, true, 10000);
            break;
        }
        case eQuests::Quest_HoldingOurGround:
        {
            p_Player->RemoveAura(eSpells::Spell_SkyhornProtection);
            break;
        }
        case eQuests::Quest_AConduitNoMore:
        {
            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_LegionConduitDestroyed, p_Player, nullptr, p_Player->GetPosition()))
                    delete l_Conversation;
            }

            break;
        }
        default:
            break;
        }
    }

    void OnObjectiveValidate(Player* p_Player, uint32 /*p_QuestID*/, uint32 p_ObjectiveID) override
    {
        switch (p_ObjectiveID)
        {
        case eObjectives::Obj_ValsharahLegionPointsRepelled:
        {
            p_Player->CastSpell(p_Player, eSpells::Spell_ToBlackRookConversation, true);
            break;
        }
        case eObjectives::Obj_PassageAcquired:
        {
            p_Player->DelayedCastSpell(p_Player, eSpells::Spell_PassageConversation, true, 12000);
            break;
        }
        case eObjectives::Obj_LegionForcesOnBlackRookHoldRepelled:
        {
            p_Player->CastSpell(p_Player, eSpells::Spell_LegionForcesConversation, true);
            break;
        }
        case eObjectives::Obj_ThunderTotemNativeRallied:
        {
            if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_DemonForces) == 10)
                p_Player->CastSpell(p_Player, eSpells::Spell_ThunderTotemDefencedConversation, true);
            break;
        }
        case eObjectives::Obj_DemonForces:
        {
            if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_ThunderTotemNativeRallied) == 3)
                p_Player->CastSpell(p_Player, eSpells::Spell_ThunderTotemDefencedConversation, true);
            break;
        }
        case eObjectives::Obj_AzsunaLegionPointsRepelled:
        {
            p_Player->CastSpell(p_Player, eSpells::Spell_FourQuestsCompleteConversation, true);
            break;
        }
        case eObjectives::Obj_PortalKeeperVthaalSlain:
        {
            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_PortalKeeperVthaalSlain, p_Player, nullptr, p_Player->GetPosition()))
                    delete l_Conversation;
            }

            break;
        }
        case eObjectives::Obj_ZorthaduunSlain:
        {
            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_ZorthaduunSlain, p_Player, nullptr, p_Player->GetPosition()))
                    delete l_Conversation;
            }

            break;
        }
        case eObjectives::Obj_StormheimLegionPointsRepelled:
        {
            p_Player->DelayedCastSpell(p_Player, eSpells::Spell_FourthQuestCompletedConversation, true, 15000);
            break;
        }
        case eObjectives::Obj_BurningLegionInvadersDefeated:
        {
            if (Vehicle* l_Vehicle = p_Player->GetVehicle())
            {
                p_Player->ExitVehicle();
                p_Player->SetRooted(true);
                p_Player->CastSpell(p_Player, eSpells::Spell_SafeLanding, true);
                p_Player->PlayScene(1794, p_Player);
            }

            break;
        }
        case eObjectives::Obj_GeneralZeonarSlain:
        {
            if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_CenarionDefender, m_CenarionDefenderSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                l_Creature->CastSpell(l_Creature, eSpells::Spell_SwiftFlightForm, true);
                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CenarionDefenderPathEnd, Path_CenarionDefender, 5);
            }

            if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_VioletwingHippogryph, m_VioletwingHippogryphSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_VioletwingHippogryphPathEnd, Path_VioletwingHippogryph, 3);
            }

            break;;
        }
        case eObjectives::Obj_IncineratorVytheSlain:
        {
            p_Player->SummonCreature(eNpcs::Npc_Lasan, m_LasanSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID());
            break;
        }
        case eObjectives::Obj_DemonsSlain:
        {
            if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_EnergyCrystalDestroyed) == 3 &&
                p_Player->GetQuestObjectiveCounter(eObjectives::Obj_InjuredProtectorRescued) == 2)
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_NeedToDefenceThunderTotemConversation, true);
            }

            break;
        }
        case eObjectives::Obj_EnergyCrystalDestroyed:
        {
            if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_DemonsSlain) == 10 &&
                p_Player->GetQuestObjectiveCounter(eObjectives::Obj_InjuredProtectorRescued) == 2)
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_NeedToDefenceThunderTotemConversation, true);
            }

            break;
        }
        case eObjectives::Obj_InjuredProtectorRescued:
        {
            if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_DemonsSlain) == 10 &&
                p_Player->GetQuestObjectiveCounter(eObjectives::Obj_EnergyCrystalDestroyed) == 3)
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_NeedToDefenceThunderTotemConversation, true);
            }

            break;
        }
        default:
            break;
        }
    }

    void OnUpdateMovement(Player* p_Player, uint16 /*p_OpcodeID*/) override
    {
        if (p_Player && p_Player->GetAreaId() == 7780)
        {
            if (p_Player->GetDistance(m_TopOfBlackRookHoldPos) <= 2.0f &&
                p_Player->GetQuestObjectiveCounter(eObjectives::Obj_ReachTheTopOfBlackRookHold) == 0)
            {
                p_Player->KilledMonsterCredit(eKillcredits::Killcredit_ReachTheTopOfBlackRookHold);
            }
        }
    }

    void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
    {
        if (p_Player->GetSceneInstanceIDByPackage(1794) == p_SceneInstanceId)
        {
            p_Player->SetRooted(false);
            p_Player->CastSpell(p_Player, eSpells::Spell_FlightEscape, true);
            p_Player->CastSpell(p_Player, eSpells::Spell_KnockedOut, true);
        }
        else if (p_Player->GetSceneInstanceIDByPackage(1793) == p_SceneInstanceId)
        {
            p_Player->CastSpell(p_Player, eSpells::Spell_AlexiusDefeated, true);
        }
    }

    void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceID) override
    {
        if (p_Player->GetSceneInstanceIDByPackage(1794) == p_SceneInstanceID)
        {
            p_Player->SetRooted(false);
            p_Player->CastSpell(p_Player, eSpells::Spell_FlightEscape, true);
            p_Player->CastSpell(p_Player, eSpells::Spell_KnockedOut, true);
        }
        else if (p_Player->GetSceneInstanceIDByPackage(1793) == p_SceneInstanceID)
        {
            p_Player->CastSpell(p_Player, eSpells::Spell_AlexiusDefeated, true);
        }
    }
};

#ifndef __clang_analyzer__
void AddSC_invasion_scripts()
{
    new npc_commander_jarod_shadowsong_118183();
    new npc_portal_bunny();
    new npc_vethir_116868();
    new npc_vethir_116867();
    new npc_blightwing_119695();
    new npc_ghostly_stagwing_117779();
    new npc_power_source_117797();
    new npc_hound_of_the_legion_118280();
    new npc_fel_mortar_115747();
    new npc_lasan_skyhorn_119676();
    new npc_shadowdirge_soul_reaper_116823();
    new npc_soul_harvesters();
    new npc_lord_kormag_117673();
    new npc_fel_cannon_116892();
    new npc_vethir_118691();
    new npc_lord_commander_alexius_118566();
    new npc_odyn_118781();
    new npc_captured_icefang_pup_119211();
    new npc_legion_portal_119192();
    new npc_observation_notes_119913();
    new npc_confinement_crystal_120277();
    new npc_fel_corruptor_120151();
    new spell_gen_alexius_fel_bombardment();
    new gob_legion_cage_q45786();
    new playerscript_invasion_scripts();
}
#endif