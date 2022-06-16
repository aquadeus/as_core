////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

/// Vindicator Boros - 121756
class npc_vindicator_boros_121756 : public CreatureScript
{
    public:
        npc_vindicator_boros_121756() : CreatureScript("npc_vindicator_boros_121756") { }

        struct npc_vindicator_boros_121756AI : public ScriptedAI
        {
            npc_vindicator_boros_121756AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_SummonAllianceCopies = 255356,
                Spell_SummonHordeCopies = 255348,
                Spell_LightforgedBeaconDeparture = 247987
            };

            enum eConversations
            {
                Conv_AllianceCopiesSummoned = 5708,
                Conv_HordeCopiesSummoned = 47223
            };

            enum eNpcs
            {
                Npc_Vereesa = 121754,
                Npc_Redeemer = 121755,
                Npc_Liadrin = 122065,
                Npc_Aethas = 122066
            };

            enum eEvents
            {
                Event_MoveToBeacon = 1
            };

            enum ePoints
            {
                Point_Beacon = 1
            };

            EventMap m_Events;
            Position const m_LightforgedBeaconPos = { -4077.358f, -11421.9f, -141.3749f, 5.256058f };

            void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
            {
                if (p_Player->GetTeamId() == TeamId::TEAM_ALLIANCE)
                {
                    if (Creature* l_Vereesa = me->FindNearestCreature(eNpcs::Npc_Vereesa, 20.0f, true))
                    {
                        l_Vereesa->DestroyForPlayer(p_Player);
                    }

                    if (Creature* l_Redeemer = me->FindNearestCreature(eNpcs::Npc_Redeemer, 20.0f, true))
                    {
                        l_Redeemer->DestroyForPlayer(p_Player);
                    }

                    me->DestroyForPlayer(p_Player);
                    p_Player->CastSpell(p_Player, eSpells::Spell_SummonAllianceCopies, true);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AllianceCopiesSummoned, p_Player, nullptr, p_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
                else if (p_Player->GetTeamId() == TeamId::TEAM_HORDE)
                {
                    if (Creature* l_Liadrin = me->FindNearestCreature(eNpcs::Npc_Liadrin, 20.0f, true))
                    {
                        l_Liadrin->DestroyForPlayer(p_Player);
                    }

                    if (Creature* l_Aethas = me->FindNearestCreature(eNpcs::Npc_Aethas, 20.0f, true))
                    {
                        l_Aethas->DestroyForPlayer(p_Player);
                    }

                    me->DestroyForPlayer(p_Player);
                    p_Player->CastSpell(p_Player, eSpells::Spell_SummonHordeCopies, true);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_HordeCopiesSummoned, p_Player, nullptr, p_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_MoveToBeacon, urand(1000, 6000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                if (me->GetExactDist2d(&m_LightforgedBeaconPos) <= 5.0f)
                {
                    me->GetMotionMaster()->Clear();
                    DoCast(eSpells::Spell_LightforgedBeaconDeparture, true);
                    me->DespawnOrUnsummon();
                }

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_MoveToBeacon:
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_Beacon, m_LightforgedBeaconPos);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vindicator_boros_121756AI(p_Creature);
        }
};

/// Vindicator Boros - 128785
class npc_vindicator_boros_128785 : public CreatureScript
{
    public:
        npc_vindicator_boros_128785() : CreatureScript("npc_vindicator_boros_128785") { }

        struct npc_vindicator_boros_128785AI : public ScriptedAI
        {
            npc_vindicator_boros_128785AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_VindicatorBoros_Mount[8] =
            {
                { -4244.901f, -11346.53f, 8.994392f },
                { -4246.401f, -11347.78f, 9.744392f },
                { -4247.901f, -11348.28f, 9.994392f },
                { -4255.651f, -11349.53f, 5.744393f },
                { -4258.151f, -11349.78f, 5.244393f },
                { -4259.651f, -11350.78f, 5.244393f },
                { -4260.651f, -11352.28f, 5.244393f },
                { -4261.151f, -11354.03f, 5.244393f }
            };

            G3D::Vector3 const Path_VindicatorBoros_Main[188] =
            {
                { -4259.829f, -11367.57f, 6.488183f },
                { -4259.329f, -11372.07f, 6.738183f },
                { -4259.079f, -11376.32f, 6.738183f },
                { -4258.579f, -11379.82f, 6.488183f },
                { -4258.329f, -11383.32f, 6.238183f },
                { -4257.829f, -11387.07f, 5.738183f },
                { -4257.329f, -11391.32f, 5.238183f },
                { -4257.079f, -11395.07f, 5.238183f },
                { -4256.579f, -11398.82f, 5.488183f },
                { -4256.079f, -11402.32f, 5.488183f },
                { -4255.829f, -11405.82f, 6.238183f },
                { -4255.829f, -11409.57f, 6.738183f },
                { -4255.329f, -11413.82f, 7.488183f },
                { -4254.829f, -11418.57f, 7.988183f },
                { -4254.579f, -11422.57f, 8.488184f },
                { -4254.079f, -11426.57f, 8.738184f },
                { -4253.829f, -11430.32f, 8.988184f },
                { -4253.579f, -11433.82f, 9.238184f },
                { -4253.079f, -11437.57f, 9.988184f },
                { -4252.829f, -11441.07f, 10.48818f },
                { -4252.579f, -11444.57f, 10.98818f },
                { -4252.079f, -11447.57f, 10.98818f },
                { -4251.579f, -11450.82f, 11.23818f },
                { -4250.829f, -11453.57f, 10.98818f },
                { -4249.579f, -11456.32f, 10.98818f },
                { -4248.079f, -11459.07f, 10.73818f },
                { -4246.329f, -11461.82f, 10.48818f },
                { -4244.579f, -11464.07f, 10.23818f },
                { -4242.579f, -11466.57f, 10.23818f },
                { -4240.079f, -11468.82f, 9.988184f },
                { -4237.829f, -11470.32f, 9.988184f },
                { -4235.579f, -11472.07f, 9.738184f },
                { -4232.579f, -11474.32f, 9.488184f },
                { -4229.579f, -11476.32f, 9.238184f },
                { -4226.579f, -11478.07f, 9.238184f },
                { -4223.579f, -11479.57f, 9.238184f },
                { -4220.079f, -11481.32f, 9.488184f },
                { -4216.329f, -11483.07f, 9.238184f },
                { -4213.079f, -11484.82f, 9.488184f },
                { -4208.829f, -11486.82f, 9.488184f },
                { -4205.079f, -11488.57f, 9.488184f },
                { -4201.329f, -11490.07f, 9.238184f },
                { -4197.829f, -11491.57f, 9.488184f },
                { -4194.079f, -11493.07f, 9.488184f },
                { -4190.829f, -11494.07f, 9.488184f },
                { -4187.079f, -11495.07f, 9.488184f },
                { -4183.079f, -11495.82f, 9.488184f },
                { -4178.579f, -11496.57f, 9.488184f },
                { -4174.579f, -11497.07f, 9.488184f },
                { -4171.079f, -11497.32f, 9.488184f },
                { -4166.829f, -11497.32f, 9.738184f },
                { -4162.829f, -11497.32f, 9.738184f },
                { -4159.329f, -11497.07f, 9.738184f },
                { -4155.579f, -11497.07f, 9.738184f },
                { -4152.329f, -11497.07f, 9.988184f },
                { -4148.829f, -11497.32f, 9.738184f },
                { -4145.329f, -11497.82f, 9.738184f },
                { -4142.329f, -11499.07f, 9.738184f },
                { -4139.329f, -11500.07f, 9.738184f },
                { -4136.079f, -11501.82f, 9.738184f },
                { -4133.329f, -11503.32f, 10.23818f },
                { -4130.079f, -11505.07f, 10.73818f },
                { -4127.329f, -11506.57f, 10.73818f },
                { -4124.329f, -11508.07f, 10.73818f },
                { -4121.079f, -11510.07f, 10.73818f },
                { -4118.829f, -11511.32f, 10.23818f },
                { -4116.579f, -11512.32f, 9.488184f },
                { -4106.375f, -11516.12f, 4.127544f },
                { -4102.625f, -11517.37f, 2.627544f },
                { -4099.625f, -11518.12f, 1.127544f },
                { -4096.375f, -11518.87f, -0.3724556f },
                { -4093.375f, -11519.12f, -1.872456f },
                { -4090.625f, -11518.87f, -3.622456f },
                { -4087.125f, -11518.87f, -5.622456f },
                { -4083.875f, -11518.62f, -7.122456f },
                { -4080.375f, -11518.37f, -8.872456f },
                { -4076.875f, -11517.62f, -10.62246f },
                { -4073.875f, -11517.12f, -12.12246f },
                { -4071.125f, -11516.62f, -13.87246f },
                { -4068.625f, -11515.62f, -15.37246f },
                { -4065.625f, -11514.37f, -16.87246f },
                { -4063.125f, -11513.37f, -18.12246f },
                { -4060.625f, -11511.87f, -19.62246f },
                { -4057.875f, -11510.37f, -21.12246f },
                { -4055.625f, -11508.62f, -22.37246f },
                { -4053.875f, -11507.12f, -23.87246f },
                { -4051.625f, -11505.37f, -25.37246f },
                { -4049.375f, -11503.12f, -26.87246f },
                { -4046.625f, -11500.87f, -28.62246f },
                { -4044.625f, -11498.62f, -30.12246f },
                { -4042.625f, -11496.12f, -31.62246f },
                { -4041.125f, -11494.12f, -33.12246f },
                { -4039.625f, -11491.62f, -34.87246f },
                { -4038.125f, -11489.37f, -36.12246f },
                { -4037.125f, -11486.87f, -37.62246f },
                { -4035.875f, -11484.12f, -38.87246f },
                { -4034.375f, -11481.12f, -40.62246f },
                { -4033.625f, -11478.87f, -41.87246f },
                { -4033.125f, -11476.12f, -43.62246f },
                { -4032.625f, -11473.62f, -45.12246f },
                { -4032.375f, -11470.37f, -46.62246f },
                { -4032.125f, -11467.12f, -48.12246f },
                { -4031.875f, -11464.12f, -49.62246f },
                { -4034.191f, -11449.09f, -56.97744f },
                { -4034.941f, -11445.59f, -59.22744f },
                { -4035.941f, -11442.09f, -61.47744f },
                { -4036.941f, -11438.84f, -63.22744f },
                { -4038.941f, -11435.09f, -65.22744f },
                { -4040.941f, -11431.59f, -67.22744f },
                { -4042.941f, -11428.34f, -69.22744f },
                { -4044.691f, -11425.59f, -71.47744f },
                { -4046.941f, -11423.09f, -73.22744f },
                { -4049.691f, -11420.59f, -74.97744f },
                { -4052.441f, -11418.59f, -76.72744f },
                { -4055.441f, -11416.34f, -78.22744f },
                { -4058.191f, -11414.59f, -80.47744f },
                { -4060.691f, -11413.09f, -82.47744f },
                { -4063.941f, -11411.59f, -84.22744f },
                { -4067.191f, -11410.34f, -85.97744f },
                { -4070.691f, -11409.34f, -87.72744f },
                { -4074.191f, -11408.09f, -89.72744f },
                { -4077.191f, -11407.09f, -91.47744f },
                { -4080.191f, -11406.59f, -93.22744f },
                { -4082.691f, -11406.34f, -94.72744f },
                { -4085.441f, -11406.09f, -96.22744f },
                { -4088.691f, -11406.09f, -97.72744f },
                { -4092.191f, -11406.34f, -99.22744f },
                { -4095.191f, -11406.59f, -101.2274f },
                { -4097.941f, -11407.09f, -102.9774f },
                { -4100.441f, -11407.59f, -104.4774f },
                { -4103.691f, -11408.59f, -106.2274f },
                { -4106.941f, -11409.84f, -107.7274f },
                { -4109.941f, -11411.09f, -109.4774f },
                { -4112.691f, -11412.34f, -111.2274f },
                { -4115.191f, -11413.84f, -113.2274f },
                { -4117.691f, -11415.34f, -114.9774f },
                { -4119.941f, -11417.34f, -116.4774f },
                { -4122.191f, -11419.34f, -117.7274f },
                { -4124.691f, -11421.34f, -119.4774f },
                { -4126.941f, -11423.59f, -121.2274f },
                { -4128.941f, -11425.84f, -122.9774f },
                { -4130.691f, -11427.84f, -124.7274f },
                { -4132.191f, -11430.09f, -126.2274f },
                { -4133.941f, -11432.59f, -127.7274f },
                { -4135.441f, -11434.84f, -129.4774f },
                { -4137.795f, -11445.08f, -130.6318f },
                { -4137.795f, -11448.83f, -130.6318f },
                { -4137.295f, -11453.08f, -130.6318f },
                { -4136.295f, -11457.58f, -130.6318f },
                { -4134.795f, -11461.58f, -130.6318f },
                { -4132.795f, -11465.33f, -130.3818f },
                { -4130.295f, -11468.83f, -130.3818f },
                { -4127.295f, -11472.08f, -130.3818f },
                { -4124.045f, -11475.33f, -130.1318f },
                { -4121.295f, -11477.33f, -130.1318f },
                { -4118.545f, -11479.08f, -130.1318f },
                { -4115.295f, -11480.83f, -131.1318f },
                { -4112.545f, -11482.08f, -132.3818f },
                { -4109.295f, -11483.58f, -133.8818f },
                { -4107.295f, -11484.33f, -134.8818f },
                { -4103.795f, -11484.33f, -134.1318f },
                { -4100.795f, -11483.83f, -134.1318f },
                { -4098.045f, -11482.58f, -133.8818f },
                { -4096.045f, -11481.08f, -134.3818f },
                { -4094.295f, -11479.08f, -135.1318f },
                { -4092.295f, -11476.58f, -135.6318f },
                { -4090.545f, -11474.33f, -136.3818f },
                { -4088.795f, -11471.83f, -138.1318f },
                { -4087.295f, -11469.33f, -139.8818f },
                { -4086.045f, -11467.08f, -141.3818f },
                { -4084.045f, -11464.33f, -141.6318f },
                { -4081.795f, -11461.58f, -141.6318f },
                { -4079.795f, -11459.08f, -141.6318f },
                { -4078.045f, -11456.83f, -141.6318f },
                { -4076.295f, -11454.58f, -141.6318f },
                { -4074.795f, -11452.58f, -141.6318f },
                { -4073.295f, -11449.83f, -141.6318f },
                { -4072.545f, -11447.58f, -141.6318f },
                { -4072.795f, -11445.33f, -141.3818f },
                { -4074.045f, -11442.83f, -141.3818f },
                { -4075.545f, -11440.58f, -141.3818f },
                { -4077.045f, -11438.58f, -141.3818f },
                { -4078.795f, -11437.33f, -141.1318f },
                { -4080.795f, -11436.33f, -141.1318f },
                { -4082.795f, -11435.33f, -141.1318f },
                { -4084.295f, -11434.83f, -141.1318f },
                { -4085.795f, -11434.33f, -141.1318f },
                { -4086.979f, -11434.03f, -141.6179f }
            };

            enum eValues
            {
                Point_VindicatorBoros_Mount = 1,
                Point_VindicatorBoros_Main = 2,
                Event_Path_Mount = 1,
                Event_Mount = 2,
                Event_StartMainPath = 3,
                Spell_SummonGreatExarchsElekk = 250373
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                m_Events.ScheduleEvent(eValues::Event_Path_Mount, 5000);
                me->SetFacingToObject(p_Summoner);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case eValues::Point_VindicatorBoros_Mount:
                {
                    DoCast(eValues::Spell_SummonGreatExarchsElekk, false);
                    m_Events.ScheduleEvent(eValues::Event_Mount, 1500);
                    break;
                }
                case eValues::Point_VindicatorBoros_Main:
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
                case eValues::Event_Path_Mount:
                {
                    me->GetMotionMaster()->MoveSmoothPath(eValues::Point_VindicatorBoros_Mount, Path_VindicatorBoros_Mount, 8, false);
                    break;
                }
                case eValues::Event_Mount:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 31368);
                    m_Events.ScheduleEvent(eValues::Event_StartMainPath, 2500);
                    break;
                }
                case eValues::Event_StartMainPath:
                {
                    me->GetMotionMaster()->MoveSmoothPath(eValues::Point_VindicatorBoros_Main, Path_VindicatorBoros_Main, 188, false);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vindicator_boros_128785AI(p_Creature);
        }
};

/// Vereesa Windrunner - 121754
class npc_vereesa_windrunner_121754 : public CreatureScript
{
    public:
        npc_vereesa_windrunner_121754() : CreatureScript("npc_vereesa_windrunner_121754") { }

        struct npc_vereesa_windrunner_121754AI : public ScriptedAI
        {
            npc_vereesa_windrunner_121754AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eValues
            {
                Quest_TheHandOfFate = 47221,
                Quest_TwoIfBySea = 47222,
                Obj_BoatArrived = 290251,
                Killcredit_MeetStormwindHarbor = 121751,
                Conv_StormwindHarbor = 5046,
                Conv_VereesaLos = 5062,
                Event_MoveToBeacon = 1,
                Point_Beacon = 1,
                Spell_LightforgedBeaconDeparture = 247987,
                Spell_TeleportAzuremystIsle = 243270
            };

            EventMap m_Events;
            Position const m_LightforgedBeaconPos = { -4077.358f, -11421.9f, -141.3749f, 5.256058f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(p_Who) <= 20.0f &&
                    l_Player->GetQuestObjectiveCounter(eValues::Obj_BoatArrived) == 0 &&
                    !l_Player->IsQuestRewarded(eValues::Quest_TwoIfBySea) &&
                    l_Player->GetQuestStatus(eValues::Quest_TwoIfBySea) != QUEST_STATUS_NONE && me->GetMapId() == 1750)
                {
                    l_Player->SetQuestObjectiveCounter(eValues::Obj_BoatArrived, 1);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eValues::Conv_VereesaLos, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }

                if (me->GetExactDist2d(p_Who) <= 50.0f &&
                    l_Player->GetQuestStatus(eValues::Quest_TheHandOfFate) != QUEST_STATUS_COMPLETE &&
                    !l_Player->IsQuestRewarded(eValues::Quest_TheHandOfFate) &&
                    l_Player->GetQuestStatus(eValues::Quest_TheHandOfFate) != QUEST_STATUS_NONE)
                {
                    l_Player->KilledMonsterCredit(eValues::Killcredit_MeetStormwindHarbor);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eValues::Conv_StormwindHarbor, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->CastSpell(p_Player, eValues::Spell_TeleportAzuremystIsle, true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_Events.ScheduleEvent(eValues::Event_MoveToBeacon, urand(1000, 6000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                if (me->GetExactDist2d(&m_LightforgedBeaconPos) <= 5.0f)
                {
                    me->GetMotionMaster()->Clear();
                    DoCast(eValues::Spell_LightforgedBeaconDeparture, true);
                    me->DespawnOrUnsummon();
                }

                switch (m_Events.ExecuteEvent())
                {
                case eValues::Event_MoveToBeacon:
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(eValues::Point_Beacon, m_LightforgedBeaconPos);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vereesa_windrunner_121754AI(p_Creature);
        }
};

/// Vereesa Windrunner - 128784
class npc_vereesa_windrunner_128784 : public CreatureScript
{
    public:
        npc_vereesa_windrunner_128784() : CreatureScript("npc_vereesa_windrunner_128784") { }

        struct npc_vereesa_windrunner_128784AI : public ScriptedAI
        {
            npc_vereesa_windrunner_128784AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Vereesa_Mount[7] =
            {
                { -4245.613f, -11345.28f, 8.941891f },
                { -4246.363f, -11347.78f, 9.691891f },
                { -4247.863f, -11348.28f, 10.19189f },
                { -4255.613f, -11349.53f, 5.691891f },
                { -4257.863f, -11350.53f, 5.191891f },
                { -4258.863f, -11351.53f, 5.191891f },
                { -4259.113f, -11353.28f, 5.191891f }
            };

            G3D::Vector3 const Path_Vereesa_Main[190] =
            {
                { -4257.823f, -11367.67f, 6.433657f },
                { -4257.573f, -11371.92f, 6.683657f },
                { -4257.073f, -11376.42f, 6.683657f },
                { -4256.573f, -11379.92f, 6.433657f },
                { -4256.073f, -11383.67f, 5.933657f },
                { -4255.573f, -11387.17f, 5.683657f },
                { -4255.073f, -11391.17f, 5.183657f },
                { -4254.823f, -11395.17f, 5.433657f },
                { -4254.573f, -11398.67f, 5.433657f },
                { -4254.323f, -11402.17f, 5.683657f },
                { -4254.073f, -11405.92f, 6.183657f },
                { -4253.823f, -11409.42f, 6.683657f },
                { -4253.323f, -11413.67f, 7.433657f },
                { -4253.073f, -11418.42f, 8.183657f },
                { -4252.823f, -11422.67f, 8.683657f },
                { -4252.323f, -11426.67f, 8.683657f },
                { -4251.823f, -11430.17f, 8.933657f },
                { -4251.573f, -11433.92f, 9.183657f },
                { -4251.573f, -11437.67f, 9.933657f },
                { -4251.323f, -11441.17f, 10.68366f },
                { -4251.323f, -11444.42f, 10.93366f },
                { -4250.823f, -11447.67f, 11.18366f },
                { -4250.323f, -11450.42f, 11.18366f },
                { -4249.573f, -11453.17f, 11.18366f },
                { -4248.323f, -11455.92f, 10.93366f },
                { -4246.823f, -11458.67f, 10.68366f },
                { -4245.073f, -11461.17f, 10.43366f },
                { -4243.573f, -11463.42f, 10.43366f },
                { -4241.323f, -11465.67f, 10.18366f },
                { -4239.073f, -11467.67f, 9.933657f },
                { -4236.823f, -11469.42f, 9.933657f },
                { -4234.573f, -11471.17f, 9.683657f },
                { -4231.823f, -11473.17f, 9.433657f },
                { -4228.823f, -11475.17f, 9.183657f },
                { -4225.823f, -11476.92f, 9.183657f },
                { -4222.823f, -11478.42f, 9.183657f },
                { -4219.573f, -11480.17f, 9.183657f },
                { -4215.573f, -11482.17f, 9.183657f },
                { -4212.073f, -11483.42f, 9.183657f },
                { -4208.323f, -11485.42f, 9.433657f },
                { -4204.323f, -11487.17f, 9.433657f },
                { -4200.573f, -11488.67f, 9.433657f },
                { -4197.323f, -11489.92f, 9.433657f },
                { -4193.323f, -11491.42f, 9.683657f },
                { -4190.323f, -11492.67f, 9.683657f },
                { -4186.573f, -11493.67f, 9.683657f },
                { -4182.823f, -11494.42f, 9.683657f },
                { -4178.323f, -11495.17f, 9.433657f },
                { -4174.323f, -11495.42f, 9.433657f },
                { -4170.573f, -11495.67f, 9.683657f },
                { -4166.573f, -11495.67f, 9.683657f },
                { -4162.823f, -11495.67f, 9.683657f },
                { -4159.073f, -11495.67f, 9.683657f },
                { -4155.573f, -11495.67f, 9.683657f },
                { -4152.073f, -11495.67f, 9.683657f },
                { -4148.323f, -11495.92f, 9.683657f },
                { -4144.823f, -11496.42f, 9.683657f },
                { -4141.573f, -11497.67f, 9.683657f },
                { -4138.573f, -11499.17f, 9.683657f },
                { -4135.323f, -11500.67f, 9.933657f },
                { -4132.573f, -11502.17f, 10.18366f },
                { -4129.323f, -11504.17f, 10.68366f },
                { -4126.573f, -11505.67f, 10.68366f },
                { -4123.573f, -11507.17f, 10.93366f },
                { -4120.573f, -11508.92f, 10.93366f },
                { -4118.323f, -11510.17f, 10.18366f },
                { -4115.823f, -11511.42f, 9.433657f },
                { -4105.889f, -11514.96f, 4.356998f },
                { -4102.389f, -11515.96f, 2.356998f },
                { -4099.139f, -11516.96f, 0.8569984f },
                { -4096.139f, -11517.46f, -0.3930016f },
                { -4093.139f, -11517.71f, -1.893002f },
                { -4090.389f, -11517.71f, -3.643002f },
                { -4086.889f, -11517.71f, -5.643002f },
                { -4083.639f, -11517.21f, -7.393002f },
                { -4080.389f, -11516.96f, -8.893002f },
                { -4077.139f, -11516.46f, -10.393f },
                { -4074.139f, -11515.71f, -12.143f },
                { -4071.389f, -11514.96f, -13.893f },
                { -4069.139f, -11514.46f, -15.393f },
                { -4066.139f, -11513.21f, -17.143f },
                { -4063.639f, -11512.21f, -18.393f },
                { -4061.139f, -11510.96f, -19.643f },
                { -4058.389f, -11509.21f, -21.143f },
                { -4056.389f, -11507.71f, -22.393f },
                { -4054.389f, -11506.21f, -23.893f },
                { -4052.139f, -11504.21f, -25.643f },
                { -4049.889f, -11502.21f, -27.143f },
                { -4047.389f, -11499.96f, -28.893f },
                { -4045.639f, -11497.71f, -30.143f },
                { -4043.389f, -11495.21f, -31.643f },
                { -4041.889f, -11493.21f, -33.143f },
                { -4040.389f, -11490.96f, -34.893f },
                { -4039.139f, -11488.71f, -36.393f },
                { -4037.889f, -11486.46f, -37.643f },
                { -4036.889f, -11483.96f, -38.893f },
                { -4035.639f, -11480.71f, -40.643f },
                { -4034.889f, -11478.21f, -41.893f },
                { -4034.139f, -11475.71f, -43.643f },
                { -4033.889f, -11473.21f, -45.143f },
                { -4033.389f, -11470.46f, -46.643f },
                { -4033.389f, -11467.21f, -48.393f },
                { -4033.139f, -11463.96f, -49.893f },
                { -4035.341f, -11449.45f, -56.9417f },
                { -4036.341f, -11445.95f, -59.4417f },
                { -4037.091f, -11442.45f, -61.6917f },
                { -4038.341f, -11439.2f, -63.4417f },
                { -4040.091f, -11435.45f, -65.1917f },
                { -4042.091f, -11431.95f, -67.1917f },
                { -4044.091f, -11428.95f, -69.4417f },
                { -4045.841f, -11426.2f, -71.4417f },
                { -4047.841f, -11423.95f, -73.4417f },
                { -4050.591f, -11421.45f, -74.9417f },
                { -4053.341f, -11419.45f, -76.6917f },
                { -4056.091f, -11417.45f, -78.4417f },
                { -4058.841f, -11415.7f, -80.6917f },
                { -4061.591f, -11414.45f, -82.4417f },
                { -4064.591f, -11412.95f, -84.4417f },
                { -4068.091f, -11411.45f, -86.1917f },
                { -4071.091f, -11410.45f, -87.6917f },
                { -4074.591f, -11409.2f, -89.9417f },
                { -4077.341f, -11408.45f, -91.4417f },
                { -4080.341f, -11407.7f, -93.4417f },
                { -4083.091f, -11407.45f, -94.6917f },
                { -4085.591f, -11407.45f, -96.1917f },
                { -4088.841f, -11407.45f, -97.6917f },
                { -4092.091f, -11407.7f, -99.4417f },
                { -4095.091f, -11407.95f, -101.4417f },
                { -4097.591f, -11408.2f, -102.9417f },
                { -4100.091f, -11408.7f, -104.6917f },
                { -4103.341f, -11409.7f, -106.1917f },
                { -4106.591f, -11410.95f, -107.9417f },
                { -4109.591f, -11412.2f, -109.4417f },
                { -4112.341f, -11413.45f, -111.4417f },
                { -4114.591f, -11414.7f, -113.1917f },
                { -4117.091f, -11416.45f, -114.9417f },
                { -4119.341f, -11418.2f, -116.4417f },
                { -4121.591f, -11419.95f, -117.6917f },
                { -4124.091f, -11422.45f, -119.4417f },
                { -4125.841f, -11424.45f, -121.1917f },
                { -4128.091f, -11426.7f, -123.1917f },
                { -4129.591f, -11428.7f, -124.6917f },
                { -4131.341f, -11430.95f, -126.4417f },
                { -4132.841f, -11433.2f, -127.6917f },
                { -4133.841f, -11435.45f, -129.1917f },
                { -4136.098f, -11445.38f, -130.6342f },
                { -4136.348f, -11448.88f, -130.6342f },
                { -4135.848f, -11452.88f, -130.6342f },
                { -4134.848f, -11457.13f, -130.6342f },
                { -4133.098f, -11461.13f, -130.6342f },
                { -4131.348f, -11464.63f, -130.3842f },
                { -4129.098f, -11467.88f, -130.3842f },
                { -4126.348f, -11471.13f, -130.3842f },
                { -4123.098f, -11473.88f, -130.3842f },
                { -4120.348f, -11476.38f, -130.1342f },
                { -4117.848f, -11478.13f, -130.1342f },
                { -4114.598f, -11479.88f, -131.3842f },
                { -4112.348f, -11481.13f, -132.3842f },
                { -4108.848f, -11482.63f, -133.8842f },
                { -4106.598f, -11483.13f, -134.8842f },
                { -4103.848f, -11483.13f, -134.3842f },
                { -4101.348f, -11482.63f, -134.1342f },
                { -4098.598f, -11481.63f, -134.1342f },
                { -4096.848f, -11480.13f, -134.3842f },
                { -4095.098f, -11478.13f, -135.1342f },
                { -4093.098f, -11475.88f, -135.3842f },
                { -4091.598f, -11473.63f, -136.3842f },
                { -4089.848f, -11470.88f, -138.3842f },
                { -4088.348f, -11468.38f, -140.1342f },
                { -4087.348f, -11466.38f, -141.3842f },
                { -4085.098f, -11463.63f, -141.6342f },
                { -4082.848f, -11460.63f, -141.6342f },
                { -4080.848f, -11458.13f, -141.6342f },
                { -4079.098f, -11455.63f, -141.6342f },
                { -4077.348f, -11453.38f, -141.6342f },
                { -4076.098f, -11451.63f, -141.6342f },
                { -4074.848f, -11449.63f, -141.8842f },
                { -4074.598f, -11447.88f, -141.6342f },
                { -4074.598f, -11445.88f, -141.6342f },
                { -4075.598f, -11443.63f, -141.6342f },
                { -4076.848f, -11441.38f, -141.6342f },
                { -4078.098f, -11439.63f, -141.3842f },
                { -4079.598f, -11438.38f, -141.1342f },
                { -4081.348f, -11437.38f, -141.1342f },
                { -4083.348f, -11436.63f, -141.1342f },
                { -4085.348f, -11435.88f, -141.1342f },
                { -4087.598f, -11435.38f, -141.1342f },
                { -4089.098f, -11434.63f, -141.1342f },
                { -4089.848f, -11433.63f, -141.1342f },
                { -4090.035f, -11432.1f, -141.6464f }
            };

            enum eValues
            {
                Point_Vereesa_Mount = 1,
                Point_Vereesa_Main = 2,
                Event_Path_Mount = 1,
                Event_Mount = 2
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                m_Events.ScheduleEvent(eValues::Event_Path_Mount, 10000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case eValues::Point_Vereesa_Mount:
                {
                    me->HandleEmoteCommand(69);
                    m_Events.ScheduleEvent(eValues::Event_Mount, 1500);
                    break;
                }
                case eValues::Point_Vereesa_Main:
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
                case eValues::Event_Path_Mount:
                {
                    me->GetMotionMaster()->MoveSmoothPath(eValues::Point_Vereesa_Mount, Path_Vereesa_Mount, 7, false);
                    break;
                }
                case eValues::Event_Mount:
                {
                    me->HandleEmoteCommand(Emote::EMOTE_STAND_STATE_NONE);
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 22474);
                    me->GetMotionMaster()->MoveSmoothPath(eValues::Point_Vereesa_Main, Path_Vereesa_Main, 190, false);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vereesa_windrunner_128784AI(p_Creature);
        }
};

/// Arator the Redeemer - 128783
class npc_arator_the_redeemer_128783 : public CreatureScript
{
    public:
        npc_arator_the_redeemer_128783() : CreatureScript("npc_arator_the_redeemer_128783") { }

        struct npc_arator_the_redeemer_128783AI : public ScriptedAI
        {
            npc_arator_the_redeemer_128783AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Redeemer_Mount[4] =
            {
                { -4258.458f, -11349.31f, 5.281162f },
                { -4260.708f, -11350.56f, 5.031162f },
                { -4261.958f, -11352.31f, 5.281162f },
                { -4262.458f, -11354.06f, 5.281162f }
            };

            G3D::Vector3 const Path_Redeemer_Main[189] =
            {
                { -4261.091f, -11368.13f, 6.48553f },
                { -4260.591f, -11372.13f, 6.73553f },
                { -4260.341f, -11376.38f, 6.73553f },
                { -4259.841f, -11379.88f, 6.48553f },
                { -4259.591f, -11383.38f, 6.23553f },
                { -4259.091f, -11387.38f, 5.73553f },
                { -4258.591f, -11391.63f, 5.23553f },
                { -4258.091f, -11395.38f, 5.48553f },
                { -4257.841f, -11398.88f, 5.48553f },
                { -4257.341f, -11402.63f, 5.48553f },
                { -4257.091f, -11406.13f, 6.23553f },
                { -4256.841f, -11409.63f, 6.73553f },
                { -4256.591f, -11413.88f, 7.48553f },
                { -4256.091f, -11418.63f, 7.98553f },
                { -4255.841f, -11422.88f, 8.48553f },
                { -4255.341f, -11426.63f, 8.73553f },
                { -4255.091f, -11430.38f, 8.98553f },
                { -4254.591f, -11434.13f, 9.23553f },
                { -4254.341f, -11437.88f, 9.98553f },
                { -4254.091f, -11441.13f, 10.48553f },
                { -4253.591f, -11444.63f, 10.73553f },
                { -4253.341f, -11447.88f, 10.98553f },
                { -4252.841f, -11451.13f, 10.98553f },
                { -4252.091f, -11453.88f, 10.98553f },
                { -4250.841f, -11456.88f, 10.98553f },
                { -4249.341f, -11459.88f, 10.73553f },
                { -4247.341f, -11462.63f, 10.23553f },
                { -4245.591f, -11465.13f, 9.98553f },
                { -4243.341f, -11467.63f, 9.98553f },
                { -4241.091f, -11469.63f, 9.98553f },
                { -4238.841f, -11471.88f, 9.73553f },
                { -4236.341f, -11473.63f, 9.73553f },
                { -4233.341f, -11475.63f, 9.48553f },
                { -4230.091f, -11477.63f, 9.23553f },
                { -4227.341f, -11479.38f, 9.23553f },
                { -4224.091f, -11481.13f, 9.48553f },
                { -4220.841f, -11482.88f, 9.48553f },
                { -4217.091f, -11484.88f, 9.48553f },
                { -4213.591f, -11486.63f, 9.73553f },
                { -4209.341f, -11488.38f, 9.73553f },
                { -4205.591f, -11490.13f, 9.73553f },
                { -4201.841f, -11491.63f, 9.48553f },
                { -4198.341f, -11493.13f, 9.48553f },
                { -4194.591f, -11494.63f, 9.48553f },
                { -4191.091f, -11495.88f, 9.73553f },
                { -4187.591f, -11496.88f, 9.73553f },
                { -4183.091f, -11497.63f, 9.73553f },
                { -4178.841f, -11498.38f, 9.73553f },
                { -4174.841f, -11498.63f, 9.48553f },
                { -4171.091f, -11498.63f, 9.48553f },
                { -4166.591f, -11498.63f, 9.73553f },
                { -4162.841f, -11498.63f, 9.73553f },
                { -4159.091f, -11498.63f, 9.98553f },
                { -4155.841f, -11498.63f, 9.98553f },
                { -4152.341f, -11498.63f, 9.98553f },
                { -4148.841f, -11498.63f, 9.98553f },
                { -4145.591f, -11499.13f, 9.73553f },
                { -4142.591f, -11500.13f, 9.73553f },
                { -4139.841f, -11501.38f, 9.73553f },
                { -4136.591f, -11502.88f, 9.73553f },
                { -4133.841f, -11504.38f, 10.23553f },
                { -4130.591f, -11506.13f, 10.73553f },
                { -4127.841f, -11507.63f, 10.73553f },
                { -4124.841f, -11509.38f, 10.73553f },
                { -4121.591f, -11511.13f, 10.73553f },
                { -4119.341f, -11512.38f, 10.23553f },
                { -4117.091f, -11513.63f, 9.48553f },
                { -4106.893f, -11518.11f, 4.226784f },
                { -4103.143f, -11519.11f, 2.726784f },
                { -4100.143f, -11519.86f, 1.226784f },
                { -4096.893f, -11520.61f, -0.2732162f },
                { -4093.643f, -11520.61f, -2.023216f },
                { -4090.893f, -11520.61f, -3.523216f },
                { -4088.143f, -11520.61f, -5.023216f },
                { -4084.893f, -11520.36f, -6.523216f },
                { -4080.393f, -11520.11f, -8.523216f },
                { -4076.643f, -11519.61f, -10.52322f },
                { -4073.643f, -11518.86f, -12.02322f },
                { -4070.893f, -11518.11f, -13.77322f },
                { -4067.893f, -11517.11f, -15.27322f },
                { -4064.893f, -11516.11f, -16.77322f },
                { -4062.643f, -11514.86f, -18.02322f },
                { -4059.893f, -11513.36f, -19.52322f },
                { -4057.393f, -11511.86f, -21.02322f },
                { -4054.893f, -11510.11f, -22.27322f },
                { -4052.893f, -11508.36f, -23.77322f },
                { -4050.643f, -11506.61f, -25.27322f },
                { -4048.393f, -11504.61f, -26.77322f },
                { -4045.393f, -11501.86f, -28.52322f },
                { -4043.393f, -11499.86f, -29.77322f },
                { -4041.393f, -11497.36f, -31.52322f },
                { -4039.893f, -11495.11f, -33.02322f },
                { -4038.393f, -11492.61f, -34.52322f },
                { -4036.893f, -11489.86f, -36.02322f },
                { -4035.643f, -11487.36f, -37.52322f },
                { -4034.393f, -11484.86f, -38.77322f },
                { -4033.143f, -11481.86f, -40.52322f },
                { -4032.143f, -11479.36f, -41.77322f },
                { -4031.643f, -11476.86f, -43.52322f },
                { -4031.143f, -11473.86f, -45.02322f },
                { -4030.643f, -11470.61f, -46.52322f },
                { -4030.393f, -11467.11f, -48.02322f },
                { -4030.143f, -11464.11f, -49.52322f },
                { -4032.532f, -11448.82f, -56.89236f },
                { -4033.532f, -11445.32f, -58.89236f },
                { -4034.532f, -11441.82f, -61.14236f },
                { -4035.532f, -11438.32f, -63.14236f },
                { -4036.782f, -11435.32f, -64.64236f },
                { -4038.282f, -11432.57f, -66.14236f },
                { -4040.032f, -11429.32f, -67.89236f },
                { -4042.282f, -11426.07f, -69.89236f },
                { -4044.532f, -11423.32f, -72.14236f },
                { -4047.032f, -11420.57f, -74.14236f },
                { -4049.782f, -11418.32f, -75.89236f },
                { -4052.532f, -11416.32f, -77.39236f },
                { -4056.282f, -11413.82f, -79.64236f },
                { -4059.782f, -11411.82f, -82.14236f },
                { -4063.282f, -11410.07f, -84.39236f },
                { -4066.782f, -11408.82f, -86.14236f },
                { -4070.032f, -11407.57f, -87.64236f },
                { -4073.782f, -11406.57f, -89.64236f },
                { -4076.532f, -11405.57f, -91.14236f },
                { -4079.532f, -11405.07f, -93.14236f },
                { -4082.532f, -11404.57f, -94.64236f },
                { -4085.282f, -11404.57f, -96.14236f },
                { -4088.532f, -11404.57f, -97.64236f },
                { -4092.282f, -11404.82f, -99.39236f },
                { -4095.532f, -11405.07f, -101.1424f },
                { -4098.282f, -11405.32f, -102.8924f },
                { -4100.782f, -11406.07f, -104.6424f },
                { -4104.282f, -11407.07f, -106.3924f },
                { -4107.282f, -11408.32f, -107.8924f },
                { -4110.282f, -11409.57f, -109.3924f },
                { -4113.282f, -11411.07f, -111.1424f },
                { -4116.032f, -11412.57f, -113.1424f },
                { -4118.532f, -11414.07f, -114.8924f },
                { -4121.032f, -11415.82f, -116.3924f },
                { -4123.282f, -11417.57f, -117.8924f },
                { -4125.782f, -11420.07f, -119.3924f },
                { -4128.032f, -11422.32f, -120.8924f },
                { -4130.032f, -11424.82f, -122.8924f },
                { -4131.782f, -11426.82f, -124.6424f },
                { -4133.532f, -11429.07f, -126.3924f },
                { -4135.282f, -11431.57f, -127.8924f },
                { -4136.532f, -11433.82f, -129.1424f },
                { -4139.405f, -11445.49f, -130.6486f },
                { -4139.155f, -11449.49f, -130.6486f },
                { -4138.655f, -11453.74f, -130.6486f },
                { -4137.405f, -11457.99f, -130.6486f },
                { -4135.905f, -11462.24f, -130.3986f },
                { -4133.905f, -11465.99f, -130.3986f },
                { -4131.155f, -11469.74f, -130.3986f },
                { -4128.155f, -11473.24f, -130.1486f },
                { -4124.655f, -11476.24f, -130.1486f },
                { -4122.155f, -11478.49f, -129.8986f },
                { -4119.655f, -11480.49f, -129.8986f },
                { -4116.155f, -11482.24f, -131.1486f },
                { -4113.405f, -11483.49f, -132.3986f },
                { -4109.905f, -11484.74f, -133.8986f },
                { -4107.405f, -11485.74f, -134.3986f },
                { -4103.905f, -11485.74f, -134.1486f },
                { -4100.405f, -11485.24f, -133.8986f },
                { -4097.655f, -11483.99f, -133.8986f },
                { -4095.155f, -11481.99f, -134.3986f },
                { -4093.155f, -11479.49f, -135.1486f },
                { -4091.155f, -11476.99f, -135.6486f },
                { -4089.405f, -11474.74f, -136.6486f },
                { -4087.655f, -11472.24f, -138.3986f },
                { -4086.155f, -11469.99f, -140.1486f },
                { -4084.405f, -11467.74f, -141.3986f },
                { -4082.405f, -11465.24f, -141.3986f },
                { -4080.655f, -11462.99f, -141.3986f },
                { -4078.655f, -11460.49f, -141.3986f },
                { -4076.405f, -11457.99f, -141.3986f },
                { -4074.655f, -11455.49f, -141.3986f },
                { -4072.905f, -11452.99f, -141.3986f },
                { -4071.905f, -11450.24f, -141.6486f },
                { -4071.405f, -11447.24f, -141.3986f },
                { -4071.905f, -11443.99f, -141.1486f },
                { -4072.905f, -11441.74f, -141.1486f },
                { -4074.155f, -11439.74f, -141.1486f },
                { -4075.905f, -11437.99f, -141.3986f },
                { -4078.405f, -11435.99f, -141.1486f },
                { -4080.405f, -11434.74f, -141.1486f },
                { -4082.405f, -11433.49f, -141.1486f },
                { -4084.405f, -11432.74f, -141.1486f },
                { -4086.405f, -11431.99f, -141.1486f },
                { -4087.905f, -11430.99f, -141.1486f },
                { -4089.389f, -11430.16f, -141.6313f }
            };

            enum eValues
            {
                Point_Redeemer_Mount = 1,
                Point_Redeemer_Main = 2,
                Event_Path_Mount = 1,
                Event_Mount = 2,
                Spell_SummonCharger = 250385
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                m_Events.ScheduleEvent(eValues::Event_Path_Mount, 10000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case eValues::Point_Redeemer_Mount:
                {
                    me->CastSpell(me, eValues::Spell_SummonCharger, false);
                    m_Events.ScheduleEvent(eValues::Event_Mount, 1500);
                    break;
                }
                case eValues::Point_Redeemer_Main:
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
                case eValues::Event_Path_Mount:
                {
                    me->GetMotionMaster()->MoveSmoothPath(eValues::Point_Redeemer_Mount, Path_Redeemer_Mount, 4, false);
                    break;
                }
                case eValues::Event_Mount:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 14584);
                    me->GetMotionMaster()->MoveSmoothPath(eValues::Point_Redeemer_Main, Path_Redeemer_Main, 189, false);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arator_the_redeemer_128783AI(p_Creature);
        }
};

/// Prophet Velen - 120977
class npc_prophet_velen_120977 : public CreatureScript
{
    public:
        npc_prophet_velen_120977() : CreatureScript("npc_prophet_velen_120977") { }

        struct npc_prophet_velen_120977AI : public ScriptedAI
        {
            npc_prophet_velen_120977AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_LightsExodus = 47223,
                Quest_TheVindicaar = 47224,
                Quest_IntoTheNight = 48440
            };

            enum eObjectives
            {
                Obj_VelenIntro = 290258,
                Obj_EnterMainChamber = 292146
            };

            enum eConversations
            {
                Conv_IllidanAndVelen = 5066,
                Conv_AcceptQuestTheVindicaar = 5220,
                Conv_BeaconActivated = 5070,
                Conv_Boarded = 5710,
                Conv_AcceptQuestIntoTheNight = 5711
            };

            enum eKillcredits
            {
                Killcredit_VelentIntro = 122091,
                Killcredit_BeaconPlaced = 290762
            };

            enum eEvents
            {
                Event_BeaconActivated_Conversation = 1,
                Event_VisualCicle = 2,
                Event_SummonLightforgedBeacon = 3,
                Event_RemoveAuras = 4,
                Event_Teleports = 5,
                Event_Teleport = 6
            };

            enum eNpcs
            {
                Npc_Bunny = 54638,
                Npc_Crystal = 122052,
                Npc_LightforgedBeacon = 122045,
                Npc_LightsHeart = 122046
            };

            enum eSpells
            {
                Spell_TeleporterArrival = 243634,
                Spell_TeleporterArrivalCircle = 243787,
                Spell_AegisOfLight = 244085,
                Spell_LightforgedBeaconDeparture = 247987,
                Spell_AegisOfLightOrig = 243598,
                Spell_ToArgus = 243785
            };

            enum ePoints
            {
                Point_Crystal = 1
            };

            ObjectGuid m_SummonerGuid;
            EventMap m_Events;
            Position const m_BunnyPos = { -4077.385f, -11421.86f, -141.368f, 5.25663f };
            Position const m_LightforgedBeaconPos = { -4077.358f, -11421.9f, -141.3749f, 5.256058f };

            std::array<uint32, 13> m_CreaturesForTeleportArray =
            { {
                121756,
                121754,
                121755,
                122065,
                122067,
                122066,
                121423,
                121263,
                121262,
                120979,
                120885,
                120978,
                122046
            } };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(p_Who) < 40.0f && l_Player->GetPositionZ() < -130.0f &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_VelenIntro) == 0 &&
                    l_Player->GetQuestStatus(eQuests::Quest_LightsExodus) == QUEST_STATUS_COMPLETE)
                {
                    l_Player->SetQuestObjectiveCounter(eObjectives::Obj_VelenIntro, 1);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_IllidanAndVelen, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }

                if (me->GetExactDist2d(p_Who) < 40.0f && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_EnterMainChamber) == 0 &&
                    l_Player->GetZoneId() == 8840 && l_Player->GetQuestStatus(eQuests::Quest_TheVindicaar) == QUEST_STATUS_COMPLETE)
                {
                    l_Player->SetQuestObjectiveCounter(eObjectives::Obj_EnterMainChamber, 1);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_Boarded, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
            }

            void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                case eQuests::Quest_TheVindicaar:
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AcceptQuestTheVindicaar, p_Player, nullptr, p_Player->GetPosition()))
                            delete l_Conversation;
                    }

                    break;
                }
                case eQuests::Quest_IntoTheNight:
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AcceptQuestIntoTheNight, p_Player, nullptr, p_Player->GetPosition()))
                            delete l_Conversation;
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_ToArgus, true);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->DelayedCastSpell(me, eSpells::Spell_AegisOfLight, true, 12000);
                m_Events.ScheduleEvent(eEvents::Event_BeaconActivated_Conversation, 2000);
                DoCast(eSpells::Spell_AegisOfLightOrig, true);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, 3);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_BeaconActivated_Conversation:
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        if (Conversation* l_Conversation = new Conversation)
                        {
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_BeaconActivated, l_Player, nullptr, l_Player->GetPosition()))
                                delete l_Conversation;
                        }
                    }

                    m_Events.ScheduleEvent(eEvents::Event_VisualCicle, 17000);
                    break;
                }
                case eEvents::Event_VisualCicle:
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_Crystal, 50.0f, true))
                        {
                            l_Creature->DestroyForPlayer(l_Player);
                        }

                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_Bunny, 50.0f, true))
                        {
                            l_Creature->DestroyForPlayer(l_Player);
                        }

                        if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_Bunny, m_BunnyPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 4000, 0, l_Player->GetGUID()))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_TeleporterArrival, true);
                            l_Creature->DelayedCastSpell(l_Creature, eSpells::Spell_TeleporterArrivalCircle, true, 1000);
                        }
                    }

                    m_Events.ScheduleEvent(eEvents::Event_SummonLightforgedBeacon, 4000);
                    break;
                }
                case eEvents::Event_SummonLightforgedBeacon:
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        if (Creature* l_Creature = l_Player->SummonCreature(eNpcs::Npc_LightforgedBeacon, m_LightforgedBeaconPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 3000, 0, l_Player->GetGUID()))
                        {
                            l_Creature->PlayOneShotAnimKitId(14161);
                        }
                    }

                    m_Events.ScheduleEvent(eEvents::Event_RemoveAuras, 3000);
                    break;
                }
                case eEvents::Event_RemoveAuras:
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        me->RemoveAllAuras();
                        me->SetFacingToObject(l_Player);
                        Talk(0, l_Player->GetGUID());
                    }

                    m_Events.ScheduleEvent(eEvents::Event_Teleports, 2000);
                    break;
                }
                case eEvents::Event_Teleports:
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        for (auto l_Itr : m_CreaturesForTeleportArray)
                        {
                            if (Creature* l_Creature = l_Player->FindNearestCreature(l_Itr, 50.0f, true))
                            {
                                if (l_Creature->GetEntry() == eNpcs::Npc_LightsHeart)
                                {
                                    l_Creature->DestroyForPlayer(l_Player);
                                }
                                else if (Creature* l_NewCreature = l_Player->SummonCreature(l_Itr, l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                                {
                                    l_Creature->DestroyForPlayer(l_Player);
                                }
                            }
                        }

                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_BeaconPlaced);
                        m_Events.ScheduleEvent(eEvents::Event_Teleport, 7000);
                    }
                    break;
                }
                case eEvents::Event_Teleport:
                {
                    me->SetFacingTo(2.92989f);
                    me->DelayedCastSpell(me, eSpells::Spell_LightforgedBeaconDeparture, true, 500);
                    me->DespawnOrUnsummon(1000);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_prophet_velen_120977AI(p_Creature);
        }
};

/// 121755, 122065, 122067, 122066, 121423, 121262, 120979, 120885, 120978
class npc_moving_to_beacon_npcs : public CreatureScript
{
    public:
        npc_moving_to_beacon_npcs() : CreatureScript("npc_moving_to_beacon_npcs") { }

        struct npc_moving_to_beacon_npcsAI : public ScriptedAI
        {
            npc_moving_to_beacon_npcsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eValues
            {
                Event_MoveToBeacon = 1,
                Point_Beacon = 1,
                Spell_LightforgedBeaconDeparture = 247987,
                Npc_Kal = 121423
            };

            EventMap m_Events;
            Position const m_LightforgedBeaconPos = { -4077.358f, -11421.9f, -141.3749f, 5.256058f };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetMapId() == 1750)
                {
                    m_Events.ScheduleEvent(eValues::Event_MoveToBeacon, urand(1000, 6000));
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                if (me->GetExactDist2d(&m_LightforgedBeaconPos) <= 5.0f)
                {
                    me->GetMotionMaster()->Clear();
                    DoCast(eValues::Spell_LightforgedBeaconDeparture, true);
                    me->DespawnOrUnsummon();
                }

                switch (m_Events.ExecuteEvent())
                {
                case eValues::Event_MoveToBeacon:
                {
                    if (me->GetEntry() == eValues::Npc_Kal)
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(eValues::Point_Beacon, m_LightforgedBeaconPos);
                    }
                    else
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(eValues::Point_Beacon, m_LightforgedBeaconPos);
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
            return new npc_moving_to_beacon_npcsAI(p_Creature);
        }
};

/// Lady Liadrin - 122065
class npc_lady_liadrin_122065 : public CreatureScript
{
    public:
        npc_lady_liadrin_122065() : CreatureScript("npc_lady_liadrin_122065") { }

        struct npc_lady_liadrin_122065AI : public ScriptedAI
        {
            npc_lady_liadrin_122065AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_TheHandOfFate = 47835,
                Quest_TwoIfBySea = 47867
            };

            enum eObjectives
            {
                Obj_BoatArrived = 291025
            };

            enum eConversations
            {
                Conv_BladefistBayMeet = 5335,
                Conv_LiadrinBoatArrived = 47867
            };

            enum eKillcredits
            {
                Killcredit_MeetEscortAtBladefistBay = 121751
            };

            enum eSpells
            {
                Spell_TeleportAzuremystIsle = 247215,
                Spell_LightforgedBeaconDeparture = 247987
            };

            enum eEvents
            {
                Event_MoveToBeacon = 1
            };

            enum ePoints
            {
                Point_Beacon = 1
            };

            EventMap m_Events;
            Position const m_LightforgedBeaconPos = { -4077.358f, -11421.9f, -141.3749f, 5.256058f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(p_Who) <= 50.0f &&
                    l_Player->GetQuestStatus(eQuests::Quest_TheHandOfFate) != QUEST_STATUS_COMPLETE &&
                    !l_Player->IsQuestRewarded(eQuests::Quest_TheHandOfFate) &&
                    l_Player->GetQuestStatus(eQuests::Quest_TheHandOfFate) != QUEST_STATUS_NONE)
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_MeetEscortAtBladefistBay);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_BladefistBayMeet, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }

                if (me->GetExactDist2d(p_Who) <= 20.0f &&
                    l_Player->GetQuestObjectiveCounter(eObjectives::Obj_BoatArrived) == 0 &&
                    !l_Player->IsQuestRewarded(eQuests::Quest_TwoIfBySea) &&
                    l_Player->GetQuestStatus(eQuests::Quest_TwoIfBySea) != QUEST_STATUS_NONE && me->GetMapId() == 1750)
                {
                    l_Player->SetQuestObjectiveCounter(eObjectives::Obj_BoatArrived, 1);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_LiadrinBoatArrived, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_TeleportAzuremystIsle, true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_MoveToBeacon, urand(1000, 6000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                if (me->GetExactDist2d(&m_LightforgedBeaconPos) <= 5.0f)
                {
                    me->GetMotionMaster()->Clear();
                    DoCast(eSpells::Spell_LightforgedBeaconDeparture, true);
                    me->DespawnOrUnsummon();
                }

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_MoveToBeacon:
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_Beacon, m_LightforgedBeaconPos);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_liadrin_122065AI(p_Creature);
        }
};

/// Vindicator Boros - 128782
class npc_vindicator_boros_128782 : public CreatureScript
{
    public:
        npc_vindicator_boros_128782() : CreatureScript("npc_vindicator_boros_128782") { }

        struct npc_vindicator_boros_128782AI : public ScriptedAI
        {
            npc_vindicator_boros_128782AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_VindicatorBorosMount[14] =
            {
                { -4287.786f, -11359.15f, 10.64665f },
                { -4287.536f, -11354.9f, 10.64665f },
                { -4287.036f, -11350.65f, 10.64665f },
                { -4286.286f, -11347.4f, 10.39665f },
                { -4285.536f, -11344.65f, 10.64665f },
                { -4284.036f, -11343.4f, 10.64665f },
                { -4282.286f, -11342.4f, 10.64665f },
                { -4270.536f, -11341.65f, 6.146649f },
                { -4268.536f, -11341.65f, 5.646649f },
                { -4266.536f, -11342.4f, 5.646649f },
                { -4265.536f, -11344.15f, 5.396649f },
                { -4265.036f, -11346.15f, 5.396649f },
                { -4264.536f, -11349.15f, 5.146649f },
                { -4264.286f, -11352.15f, 5.146649f }
            };

            G3D::Vector3 const Path_VindicatorBorosMain[187] =
            {
                { -4262.591f, -11368.08f, 6.27099f },
                { -4262.341f, -11372.33f, 6.77099f },
                { -4261.841f, -11376.33f, 6.77099f },
                { -4261.341f, -11380.08f, 6.52099f },
                { -4261.091f, -11383.58f, 6.02099f },
                { -4260.591f, -11387.33f, 5.77099f },
                { -4260.091f, -11391.58f, 5.27099f },
                { -4259.591f, -11395.08f, 5.27099f },
                { -4258.841f, -11398.83f, 5.52099f },
                { -4258.341f, -11403.08f, 5.52099f },
                { -4257.841f, -11406.58f, 6.27099f },
                { -4257.341f, -11409.83f, 6.77099f },
                { -4256.591f, -11414.33f, 7.52099f },
                { -4255.841f, -11418.58f, 8.02099f },
                { -4255.341f, -11422.83f, 8.52099f },
                { -4254.841f, -11427.08f, 8.77099f },
                { -4254.341f, -11430.58f, 9.02099f },
                { -4253.841f, -11434.33f, 9.27099f },
                { -4253.591f, -11438.08f, 9.77099f },
                { -4253.341f, -11441.33f, 10.52099f },
                { -4253.091f, -11444.58f, 10.77099f },
                { -4252.591f, -11447.83f, 11.02099f },
                { -4251.591f, -11451.08f, 11.02099f },
                { -4250.841f, -11453.83f, 11.02099f },
                { -4249.591f, -11456.83f, 11.02099f },
                { -4248.091f, -11459.08f, 10.77099f },
                { -4246.341f, -11462.08f, 10.27099f },
                { -4244.591f, -11464.58f, 10.27099f },
                { -4242.341f, -11466.58f, 10.02099f },
                { -4240.091f, -11468.58f, 10.02099f },
                { -4237.591f, -11470.58f, 9.77099f },
                { -4235.341f, -11472.33f, 9.77099f },
                { -4232.591f, -11474.33f, 9.27099f },
                { -4229.591f, -11476.33f, 9.27099f },
                { -4226.591f, -11478.08f, 9.27099f },
                { -4223.341f, -11479.83f, 9.27099f },
                { -4220.091f, -11481.33f, 9.27099f },
                { -4216.341f, -11483.33f, 9.27099f },
                { -4213.091f, -11484.83f, 9.52099f },
                { -4208.841f, -11486.83f, 9.27099f },
                { -4204.841f, -11488.58f, 9.52099f },
                { -4201.341f, -11490.08f, 9.27099f },
                { -4197.591f, -11491.58f, 9.52099f },
                { -4193.841f, -11492.83f, 9.52099f },
                { -4190.591f, -11494.08f, 9.52099f },
                { -4186.841f, -11495.08f, 9.52099f },
                { -4182.841f, -11495.83f, 9.52099f },
                { -4178.341f, -11496.58f, 9.52099f },
                { -4174.341f, -11497.08f, 9.52099f },
                { -4170.841f, -11497.33f, 9.52099f },
                { -4166.841f, -11497.33f, 9.52099f },
                { -4164.591f, -11497.33f, 9.77099f },
                { -4160.091f, -11497.08f, 9.77099f },
                { -4157.091f, -11497.08f, 9.77099f },
                { -4153.341f, -11497.08f, 9.77099f },
                { -4150.591f, -11497.33f, 9.77099f },
                { -4147.091f, -11497.58f, 9.77099f },
                { -4143.841f, -11498.33f, 9.77099f },
                { -4140.591f, -11499.83f, 9.77099f },
                { -4137.341f, -11501.08f, 9.77099f },
                { -4134.841f, -11502.58f, 9.77099f },
                { -4129.841f, -11505.33f, 10.77099f },
                { -4126.091f, -11507.33f, 10.77099f },
                { -4122.091f, -11509.58f, 10.77099f },
                { -4118.841f, -11511.08f, 10.27099f },
                { -4116.841f, -11512.33f, 9.52099f },
                { -4106.443f, -11516.1f, 4.165247f },
                { -4102.693f, -11517.35f, 2.415247f },
                { -4099.693f, -11518.1f, 1.165247f },
                { -4096.443f, -11518.85f, -0.334753f },
                { -4093.443f, -11519.1f, -2.084753f },
                { -4090.443f, -11518.85f, -3.584753f },
                { -4087.193f, -11518.85f, -5.584753f },
                { -4083.693f, -11518.6f, -7.334753f },
                { -4080.193f, -11518.35f, -8.834753f },
                { -4076.693f, -11517.85f, -10.58475f },
                { -4073.943f, -11517.1f, -12.08475f },
                { -4071.193f, -11516.6f, -13.83475f },
                { -4068.693f, -11515.6f, -15.33475f },
                { -4065.693f, -11514.35f, -17.08475f },
                { -4063.193f, -11513.35f, -18.33475f },
                { -4060.693f, -11511.85f, -19.58475f },
                { -4057.943f, -11510.35f, -21.08475f },
                { -4055.693f, -11508.6f, -22.33475f },
                { -4053.943f, -11507.1f, -23.83475f },
                { -4051.693f, -11505.35f, -25.58475f },
                { -4049.193f, -11503.1f, -27.08475f },
                { -4046.693f, -11500.85f, -28.58475f },
                { -4044.693f, -11498.6f, -30.08475f },
                { -4042.693f, -11496.1f, -31.58475f },
                { -4041.193f, -11494.1f, -33.08475f },
                { -4039.693f, -11491.6f, -34.83475f },
                { -4038.193f, -11489.35f, -36.33475f },
                { -4036.943f, -11487.1f, -37.58475f },
                { -4035.693f, -11484.35f, -39.08475f },
                { -4034.443f, -11481.35f, -40.58475f },
                { -4033.693f, -11478.85f, -41.83475f },
                { -4033.193f, -11476.1f, -43.58475f },
                { -4032.693f, -11473.6f, -45.08475f },
                { -4032.193f, -11470.35f, -46.58475f },
                { -4031.943f, -11467.35f, -48.08475f },
                { -4031.943f, -11464.1f, -49.58475f },
                { -4032.596f, -11448.93f, -56.98756f },
                { -4033.346f, -11445.43f, -58.98756f },
                { -4034.346f, -11441.68f, -61.23756f },
                { -4035.596f, -11438.43f, -63.23756f },
                { -4036.846f, -11435.43f, -64.73756f },
                { -4038.346f, -11432.68f, -66.23756f },
                { -4040.096f, -11429.43f, -67.98756f },
                { -4042.346f, -11426.18f, -69.98756f },
                { -4044.596f, -11423.18f, -72.23756f },
                { -4047.096f, -11420.68f, -74.23756f },
                { -4049.846f, -11418.18f, -75.98756f },
                { -4052.346f, -11416.43f, -77.48756f },
                { -4056.346f, -11413.93f, -79.48756f },
                { -4059.596f, -11411.68f, -82.23756f },
                { -4063.346f, -11410.18f, -84.23756f },
                { -4066.596f, -11408.93f, -85.98756f },
                { -4069.846f, -11407.68f, -87.73756f },
                { -4073.596f, -11406.43f, -89.48756f },
                { -4076.596f, -11405.68f, -91.23756f },
                { -4079.596f, -11404.93f, -92.98756f },
                { -4082.596f, -11404.68f, -94.73756f },
                { -4085.096f, -11404.43f, -95.98756f },
                { -4088.346f, -11404.68f, -97.73756f },
                { -4092.346f, -11404.93f, -99.48756f },
                { -4095.596f, -11405.18f, -101.2376f },
                { -4098.096f, -11405.43f, -102.9876f },
                { -4100.596f, -11405.93f, -104.4876f },
                { -4104.346f, -11407.18f, -106.2376f },
                { -4107.346f, -11408.43f, -107.9876f },
                { -4110.346f, -11409.68f, -109.4876f },
                { -4113.346f, -11410.93f, -111.2376f },
                { -4116.096f, -11412.43f, -112.9876f },
                { -4118.346f, -11413.93f, -114.9876f },
                { -4120.846f, -11415.93f, -116.4876f },
                { -4123.096f, -11417.68f, -117.7376f },
                { -4125.596f, -11420.18f, -119.4876f },
                { -4128.096f, -11422.43f, -120.9876f },
                { -4130.096f, -11424.68f, -122.9876f },
                { -4131.846f, -11426.68f, -124.7376f },
                { -4133.596f, -11428.93f, -126.2376f },
                { -4135.346f, -11431.68f, -127.7376f },
                { -4136.596f, -11433.68f, -129.2375f },
                { -4137.951f, -11445.18f, -130.6419f },
                { -4137.951f, -11448.93f, -130.6419f },
                { -4137.451f, -11453.18f, -130.6419f },
                { -4136.201f, -11457.43f, -130.6419f },
                { -4134.701f, -11461.68f, -130.3919f },
                { -4132.701f, -11465.43f, -130.3919f },
                { -4130.201f, -11468.93f, -130.3919f },
                { -4127.451f, -11472.18f, -130.3919f },
                { -4124.201f, -11475.18f, -130.1419f },
                { -4121.451f, -11477.43f, -130.1419f },
                { -4118.451f, -11479.18f, -130.1419f },
                { -4115.451f, -11480.93f, -131.1419f },
                { -4112.451f, -11482.18f, -132.3919f },
                { -4109.201f, -11483.68f, -133.8919f },
                { -4107.201f, -11484.43f, -134.8919f },
                { -4103.951f, -11484.43f, -134.1419f },
                { -4100.701f, -11483.93f, -133.8919f },
                { -4097.951f, -11482.68f, -133.8919f },
                { -4095.951f, -11481.18f, -134.3919f },
                { -4094.201f, -11478.93f, -135.1419f },
                { -4092.201f, -11476.43f, -135.6419f },
                { -4090.451f, -11474.18f, -136.3919f },
                { -4088.951f, -11471.93f, -138.1419f },
                { -4087.201f, -11469.43f, -139.8919f },
                { -4086.201f, -11467.18f, -141.3919f },
                { -4083.951f, -11464.43f, -141.6419f },
                { -4081.951f, -11461.68f, -141.6419f },
                { -4079.701f, -11458.93f, -141.6419f },
                { -4077.951f, -11456.68f, -141.6419f },
                { -4076.201f, -11454.68f, -141.6419f },
                { -4074.951f, -11452.68f, -141.6419f },
                { -4073.201f, -11449.93f, -141.6419f },
                { -4072.701f, -11447.68f, -141.6419f },
                { -4072.951f, -11445.43f, -141.3919f },
                { -4074.201f, -11442.93f, -141.3919f },
                { -4075.451f, -11440.68f, -141.3919f },
                { -4076.951f, -11438.68f, -141.3919f },
                { -4078.701f, -11437.43f, -141.1419f },
                { -4080.701f, -11436.43f, -141.1419f },
                { -4082.951f, -11435.18f, -141.1419f },
                { -4084.451f, -11434.93f, -141.1419f },
                { -4085.701f, -11434.43f, -141.1419f },
                { -4086.979f, -11434.03f, -141.6179f }
            };

            enum ePoints
            {
                Point_VindicatorBorosMount = 1,
                Point_VindicatorBorosMain = 2
            };

            enum eEvents
            {
                Event_PathMount = 1,
                Event_Mount = 2,
                Event_StartMainPath = 3
            };

            enum eSpells
            {
                Spell_SummonGreatExarchsElekk = 250373
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                m_Events.ScheduleEvent(eEvents::Event_PathMount, 6000);
                me->SetFacingTo(4.414003f);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_VindicatorBorosMount:
                {
                    DoCast(eSpells::Spell_SummonGreatExarchsElekk, false);
                    m_Events.ScheduleEvent(eEvents::Event_Mount, 1500);
                    break;
                }
                case ePoints::Point_VindicatorBorosMain:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                    me->SetFacingTo(0.5934119f);
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
                case eEvents::Event_PathMount:
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_VindicatorBorosMount, Path_VindicatorBorosMount, 14, false);
                    break;
                }
                case eEvents::Event_Mount:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 31368);
                    m_Events.ScheduleEvent(eEvents::Event_StartMainPath, 1000);
                    break;
                }
                case eEvents::Event_StartMainPath:
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_VindicatorBorosMain, Path_VindicatorBorosMain, 187, false);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vindicator_boros_128782AI(p_Creature);
        }
};

/// Lady Liadrin - 128781
class npc_lady_liadrin_128781 : public CreatureScript
{
    public:
        npc_lady_liadrin_128781() : CreatureScript("npc_lady_liadrin_128781") { }

        struct npc_lady_liadrin_128781AI : public ScriptedAI
        {
            npc_lady_liadrin_128781AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_LiadrinMount[20] =
            {
                { -4289.527f, -11370.62f, 10.91477f },
                { -4289.277f, -11367.62f, 10.91477f },
                { -4289.027f, -11364.87f, 10.66477f },
                { -4288.777f, -11361.62f, 10.66477f },
                { -4288.527f, -11358.87f, 10.66477f },
                { -4288.277f, -11354.37f, 10.66477f },
                { -4287.777f, -11350.12f, 10.41477f },
                { -4287.277f, -11346.62f, 10.41477f },
                { -4286.277f, -11343.87f, 10.66477f },
                { -4284.777f, -11342.12f, 10.66477f },
                { -4282.777f, -11341.37f, 10.66477f },
                { -4270.527f, -11340.62f, 6.164768f },
                { -4268.527f, -11340.62f, 5.664768f },
                { -4266.277f, -11340.87f, 5.664768f },
                { -4264.277f, -11341.87f, 5.664768f },
                { -4262.777f, -11343.62f, 5.414768f },
                { -4262.027f, -11345.87f, 5.414768f },
                { -4261.777f, -11348.12f, 5.164768f },
                { -4261.527f, -11350.62f, 5.164768f },
                { -4261.277f, -11352.87f, 5.164768f }
            };

            G3D::Vector3 const Path_LiadrinMain[190] =
            {
                { -4259.605f, -11367.86f, 6.422843f },
                { -4259.355f, -11372.11f, 6.672843f },
                { -4258.855f, -11376.36f, 6.672843f },
                { -4258.355f, -11379.61f, 6.672843f },
                { -4257.855f, -11383.61f, 6.172843f },
                { -4257.605f, -11387.11f, 5.672843f },
                { -4257.105f, -11390.86f, 5.422843f },
                { -4256.605f, -11395.11f, 5.422843f },
                { -4256.105f, -11398.36f, 5.422843f },
                { -4255.605f, -11402.36f, 5.422843f },
                { -4255.105f, -11405.86f, 6.172843f },
                { -4254.605f, -11409.61f, 6.922843f },
                { -4253.855f, -11414.11f, 7.422843f },
                { -4253.355f, -11418.61f, 8.172843f },
                { -4252.855f, -11422.61f, 8.672843f },
                { -4252.605f, -11426.61f, 8.672843f },
                { -4252.105f, -11430.36f, 8.922843f },
                { -4251.855f, -11434.11f, 9.172843f },
                { -4251.855f, -11437.61f, 9.922843f },
                { -4251.605f, -11441.11f, 10.67284f },
                { -4251.355f, -11444.61f, 10.92284f },
                { -4250.855f, -11447.86f, 11.17284f },
                { -4250.355f, -11450.36f, 11.17284f },
                { -4249.605f, -11453.11f, 11.17284f },
                { -4248.355f, -11455.86f, 10.92284f },
                { -4246.605f, -11458.61f, 10.67284f },
                { -4245.105f, -11461.11f, 10.42284f },
                { -4243.355f, -11463.36f, 10.42284f },
                { -4241.355f, -11465.61f, 10.17284f },
                { -4239.105f, -11467.61f, 9.922843f },
                { -4236.855f, -11469.36f, 9.922843f },
                { -4234.605f, -11471.11f, 9.672843f },
                { -4231.855f, -11473.11f, 9.422843f },
                { -4228.855f, -11475.11f, 9.172843f },
                { -4225.855f, -11476.86f, 9.172843f },
                { -4222.605f, -11478.61f, 9.172843f },
                { -4219.355f, -11480.11f, 9.172843f },
                { -4215.355f, -11482.11f, 9.172843f },
                { -4212.105f, -11483.61f, 9.172843f },
                { -4208.355f, -11485.36f, 9.422843f },
                { -4204.355f, -11487.11f, 9.422843f },
                { -4200.605f, -11488.61f, 9.422843f },
                { -4197.105f, -11490.11f, 9.422843f },
                { -4193.355f, -11491.61f, 9.672843f },
                { -4190.355f, -11492.61f, 9.672843f },
                { -4186.355f, -11493.61f, 9.672843f },
                { -4182.855f, -11494.36f, 9.672843f },
                { -4178.105f, -11495.11f, 9.422843f },
                { -4174.105f, -11495.36f, 9.422843f },
                { -4170.605f, -11495.61f, 9.672843f },
                { -4166.605f, -11495.86f, 9.672843f },
                { -4162.855f, -11495.61f, 9.672843f },
                { -4159.105f, -11495.61f, 9.672843f },
                { -4155.605f, -11495.61f, 9.672843f },
                { -4152.105f, -11495.86f, 9.672843f },
                { -4148.355f, -11495.86f, 9.672843f },
                { -4144.855f, -11496.61f, 9.672843f },
                { -4141.605f, -11497.61f, 9.672843f },
                { -4138.605f, -11499.11f, 9.672843f },
                { -4135.355f, -11500.61f, 9.922843f },
                { -4132.605f, -11502.36f, 10.17284f },
                { -4129.355f, -11504.11f, 10.67284f },
                { -4126.605f, -11505.61f, 10.67284f },
                { -4123.605f, -11507.11f, 10.92284f },
                { -4120.605f, -11508.86f, 10.92284f },
                { -4118.105f, -11510.11f, 10.17284f },
                { -4115.855f, -11511.11f, 9.422843f },
                { -4106.028f, -11514.82f, 4.338924f },
                { -4102.278f, -11516.07f, 2.338924f },
                { -4099.278f, -11516.82f, 0.8389244f },
                { -4096.028f, -11517.57f, -0.4110756f },
                { -4093.278f, -11517.82f, -1.911076f },
                { -4090.278f, -11517.82f, -3.661076f },
                { -4087.028f, -11517.57f, -5.661076f },
                { -4083.528f, -11517.32f, -7.411076f },
                { -4080.278f, -11516.82f, -8.911076f },
                { -4077.028f, -11516.57f, -10.41108f },
                { -4074.278f, -11515.82f, -12.16108f },
                { -4071.528f, -11515.07f, -13.91108f },
                { -4069.028f, -11514.57f, -15.41108f },
                { -4066.028f, -11513.32f, -17.16108f },
                { -4063.528f, -11512.07f, -18.41108f },
                { -4061.028f, -11510.82f, -19.66108f },
                { -4058.528f, -11509.32f, -21.16108f },
                { -4056.278f, -11507.82f, -22.41108f },
                { -4054.278f, -11506.07f, -23.91108f },
                { -4052.278f, -11504.32f, -25.41108f },
                { -4049.778f, -11502.32f, -27.16108f },
                { -4047.278f, -11499.82f, -28.66108f },
                { -4045.528f, -11497.82f, -30.16108f },
                { -4043.528f, -11495.32f, -31.66108f },
                { -4042.028f, -11493.32f, -33.16108f },
                { -4040.528f, -11491.07f, -34.66108f },
                { -4039.278f, -11488.57f, -36.41108f },
                { -4038.028f, -11486.32f, -37.66108f },
                { -4036.778f, -11483.82f, -38.91108f },
                { -4035.528f, -11480.82f, -40.66108f },
                { -4034.778f, -11478.32f, -41.91108f },
                { -4034.278f, -11475.57f, -43.66108f },
                { -4033.778f, -11473.32f, -45.16108f },
                { -4033.528f, -11470.32f, -46.66108f },
                { -4033.278f, -11467.07f, -48.41108f },
                { -4033.028f, -11464.07f, -49.91108f },
                { -4035.341f, -11449.45f, -56.9417f },
                { -4036.341f, -11445.95f, -59.4417f },
                { -4037.091f, -11442.45f, -61.6917f },
                { -4038.341f, -11439.2f, -63.4417f },
                { -4040.091f, -11435.45f, -65.1917f },
                { -4042.091f, -11431.95f, -67.1917f },
                { -4044.091f, -11428.95f, -69.4417f },
                { -4045.841f, -11426.2f, -71.4417f },
                { -4047.841f, -11423.95f, -73.4417f },
                { -4050.591f, -11421.45f, -74.9417f },
                { -4053.341f, -11419.45f, -76.6917f },
                { -4056.091f, -11417.45f, -78.4417f },
                { -4058.841f, -11415.7f, -80.6917f },
                { -4061.591f, -11414.45f, -82.4417f },
                { -4064.591f, -11412.95f, -84.4417f },
                { -4068.091f, -11411.45f, -86.1917f },
                { -4071.091f, -11410.45f, -87.6917f },
                { -4074.591f, -11409.2f, -89.9417f },
                { -4077.341f, -11408.45f, -91.4417f },
                { -4080.341f, -11407.7f, -93.4417f },
                { -4083.091f, -11407.45f, -94.6917f },
                { -4085.591f, -11407.45f, -96.1917f },
                { -4088.841f, -11407.45f, -97.6917f },
                { -4092.091f, -11407.7f, -99.4417f },
                { -4095.091f, -11407.95f, -101.4417f },
                { -4097.591f, -11408.2f, -102.9417f },
                { -4100.091f, -11408.7f, -104.6917f },
                { -4103.341f, -11409.7f, -106.1917f },
                { -4106.591f, -11410.95f, -107.9417f },
                { -4109.591f, -11412.2f, -109.4417f },
                { -4112.341f, -11413.45f, -111.4417f },
                { -4114.591f, -11414.7f, -113.1917f },
                { -4117.091f, -11416.45f, -114.9417f },
                { -4119.341f, -11418.2f, -116.4417f },
                { -4121.591f, -11419.95f, -117.6917f },
                { -4124.091f, -11422.45f, -119.4417f },
                { -4125.841f, -11424.45f, -121.1917f },
                { -4128.091f, -11426.7f, -123.1917f },
                { -4129.591f, -11428.7f, -124.6917f },
                { -4131.341f, -11430.95f, -126.4417f },
                { -4132.841f, -11433.2f, -127.6917f },
                { -4133.841f, -11435.45f, -129.1917f },
                { -4136.098f, -11445.38f, -130.6342f },
                { -4136.348f, -11448.88f, -130.6342f },
                { -4135.848f, -11452.88f, -130.6342f },
                { -4134.848f, -11457.13f, -130.6342f },
                { -4133.098f, -11461.13f, -130.6342f },
                { -4131.348f, -11464.63f, -130.3842f },
                { -4129.098f, -11467.88f, -130.3842f },
                { -4126.348f, -11471.13f, -130.3842f },
                { -4123.098f, -11473.88f, -130.3842f },
                { -4120.348f, -11476.38f, -130.1342f },
                { -4117.848f, -11478.13f, -130.1342f },
                { -4114.598f, -11479.88f, -131.3842f },
                { -4112.348f, -11481.13f, -132.3842f },
                { -4108.848f, -11482.63f, -133.8842f },
                { -4106.598f, -11483.13f, -134.8842f },
                { -4103.848f, -11483.13f, -134.3842f },
                { -4101.348f, -11482.63f, -134.1342f },
                { -4098.598f, -11481.63f, -134.1342f },
                { -4096.848f, -11480.13f, -134.3842f },
                { -4095.098f, -11478.13f, -135.1342f },
                { -4093.098f, -11475.88f, -135.3842f },
                { -4091.598f, -11473.63f, -136.3842f },
                { -4089.848f, -11470.88f, -138.3842f },
                { -4088.348f, -11468.38f, -140.1342f },
                { -4087.348f, -11466.38f, -141.3842f },
                { -4085.098f, -11463.63f, -141.6342f },
                { -4082.848f, -11460.63f, -141.6342f },
                { -4080.848f, -11458.13f, -141.6342f },
                { -4079.098f, -11455.63f, -141.6342f },
                { -4077.348f, -11453.38f, -141.6342f },
                { -4076.098f, -11451.63f, -141.6342f },
                { -4074.848f, -11449.63f, -141.8842f },
                { -4074.598f, -11447.88f, -141.6342f },
                { -4074.598f, -11445.88f, -141.6342f },
                { -4075.598f, -11443.63f, -141.6342f },
                { -4076.848f, -11441.38f, -141.6342f },
                { -4078.098f, -11439.63f, -141.3842f },
                { -4079.598f, -11438.38f, -141.1342f },
                { -4081.348f, -11437.38f, -141.1342f },
                { -4083.348f, -11436.63f, -141.1342f },
                { -4085.348f, -11435.88f, -141.1342f },
                { -4087.598f, -11435.38f, -141.1342f },
                { -4089.098f, -11434.63f, -141.1342f },
                { -4089.848f, -11433.63f, -141.1342f },
                { -4090.035f, -11432.1f, -141.6464f }
            };

            enum ePoints
            {
                Point_LiadrinMount = 1,
                Point_LiadrinMain = 2
            };

            enum eEvents
            {
                Event_PathMount = 1,
                Event_Mount = 2,
                Event_StartMainPath = 3
            };

            enum eSpells
            {
                Spell_SummonRedemption = 250859
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                m_Events.ScheduleEvent(eEvents::Event_PathMount, 5500);
                me->SetFacingTo(1.466077f);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_LiadrinMount:
                {
                    DoCast(eSpells::Spell_SummonRedemption, false);
                    m_Events.ScheduleEvent(eEvents::Event_Mount, 1500);
                    break;
                }
                case ePoints::Point_LiadrinMain:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                    me->SetFacingTo(0.5934119f);
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
                case eEvents::Event_PathMount:
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_LiadrinMount, Path_LiadrinMount, 20, false);
                    break;
                }
                case eEvents::Event_Mount:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 19085);
                    m_Events.ScheduleEvent(eEvents::Event_StartMainPath, 1000);
                    break;
                }
                case eEvents::Event_StartMainPath:
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_LiadrinMain, Path_LiadrinMain, 190, false);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_liadrin_128781AI(p_Creature);
        }
};

/// Aethas Sunreaver - 128779
class npc_aethas_sunreaver_128779 : public CreatureScript
{
    public:
        npc_aethas_sunreaver_128779() : CreatureScript("npc_aethas_sunreaver_128779") { }

        struct npc_aethas_sunreaver_128779AI : public ScriptedAI
        {
            npc_aethas_sunreaver_128779AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_AethasMount[21] =
            {
                { -4291.622f, -11380.57f, 11.56491f },
                { -4289.291f, -11372.7f, 10.98151f },
                { -4289.041f, -11370.2f, 10.98151f },
                { -4288.791f, -11367.7f, 10.73151f },
                { -4288.291f, -11363.2f, 10.73151f },
                { -4288.041f, -11359.2f, 10.48151f },
                { -4287.291f, -11354.7f, 10.48151f },
                { -4286.791f, -11350.45f, 10.48151f },
                { -4286.291f, -11347.2f, 10.48151f },
                { -4285.541f, -11344.7f, 10.48151f },
                { -4284.041f, -11343.2f, 10.48151f },
                { -4282.291f, -11342.45f, 10.48151f },
                { -4270.541f, -11341.7f, 5.981511f },
                { -4268.541f, -11341.45f, 5.731511f },
                { -4266.541f, -11341.95f, 5.731511f },
                { -4264.791f, -11342.95f, 5.481511f },
                { -4263.791f, -11344.7f, 5.481511f },
                { -4263.291f, -11346.7f, 5.231511f },
                { -4263.041f, -11349.2f, 5.231511f },
                { -4262.791f, -11351.7f, 5.231511f },
                { -4262.541f, -11353.7f, 5.231511f },
            };

            G3D::Vector3 const Path_AethasMain[189] =
            {
                { -4261.052f, -11368.12f, 6.485294f },
                { -4260.552f, -11372.12f, 6.735294f },
                { -4260.302f, -11376.37f, 6.735294f },
                { -4259.802f, -11379.87f, 6.485294f },
                { -4259.552f, -11383.37f, 6.235294f },
                { -4259.052f, -11387.37f, 5.735294f },
                { -4258.552f, -11391.62f, 5.235294f },
                { -4258.302f, -11395.37f, 5.485294f },
                { -4257.802f, -11398.87f, 5.485294f },
                { -4257.302f, -11402.62f, 5.485294f },
                { -4257.052f, -11406.12f, 6.235294f },
                { -4256.802f, -11409.62f, 6.735294f },
                { -4256.552f, -11413.87f, 7.485294f },
                { -4256.052f, -11418.62f, 7.985294f },
                { -4255.802f, -11422.87f, 8.485294f },
                { -4255.302f, -11426.62f, 8.735294f },
                { -4255.052f, -11430.37f, 8.985294f },
                { -4254.552f, -11434.12f, 9.235294f },
                { -4254.302f, -11437.87f, 9.985294f },
                { -4254.052f, -11441.12f, 10.48529f },
                { -4253.552f, -11444.62f, 10.73529f },
                { -4253.302f, -11448.12f, 10.98529f },
                { -4252.802f, -11451.12f, 10.98529f },
                { -4252.052f, -11453.87f, 10.98529f },
                { -4250.802f, -11456.87f, 10.98529f },
                { -4249.302f, -11459.87f, 10.73529f },
                { -4247.302f, -11462.62f, 10.23529f },
                { -4245.552f, -11465.12f, 9.985294f },
                { -4243.302f, -11467.87f, 9.985294f },
                { -4241.052f, -11469.62f, 9.985294f },
                { -4238.802f, -11471.87f, 9.735294f },
                { -4236.302f, -11473.62f, 9.735294f },
                { -4233.552f, -11475.62f, 9.485294f },
                { -4230.052f, -11477.62f, 9.235294f },
                { -4227.302f, -11479.37f, 9.235294f },
                { -4224.052f, -11481.12f, 9.485294f },
                { -4220.802f, -11482.87f, 9.485294f },
                { -4217.052f, -11484.87f, 9.485294f },
                { -4213.552f, -11486.62f, 9.735294f },
                { -4209.302f, -11488.37f, 9.735294f },
                { -4205.552f, -11490.12f, 9.735294f },
                { -4201.802f, -11491.62f, 9.485294f },
                { -4198.302f, -11493.12f, 9.485294f },
                { -4194.552f, -11494.62f, 9.485294f },
                { -4191.052f, -11495.87f, 9.735294f },
                { -4187.552f, -11496.87f, 9.735294f },
                { -4183.052f, -11497.62f, 9.735294f },
                { -4178.802f, -11498.37f, 9.735294f },
                { -4174.802f, -11498.62f, 9.485294f },
                { -4171.052f, -11498.87f, 9.485294f },
                { -4166.802f, -11498.62f, 9.735294f },
                { -4162.802f, -11498.62f, 9.735294f },
                { -4159.052f, -11498.62f, 9.985294f },
                { -4155.802f, -11498.62f, 9.985294f },
                { -4152.302f, -11498.62f, 9.985294f },
                { -4148.802f, -11498.62f, 9.985294f },
                { -4145.552f, -11499.12f, 9.735294f },
                { -4142.802f, -11500.12f, 9.735294f },
                { -4139.802f, -11501.37f, 9.735294f },
                { -4136.552f, -11502.87f, 9.735294f },
                { -4133.802f, -11504.37f, 10.23529f },
                { -4130.802f, -11506.12f, 10.73529f },
                { -4127.802f, -11507.62f, 10.73529f },
                { -4125.052f, -11509.37f, 10.73529f },
                { -4121.552f, -11511.12f, 10.73529f },
                { -4119.302f, -11512.37f, 10.23529f },
                { -4117.052f, -11513.62f, 9.485294f },
                { -4106.893f, -11518.11f, 4.226784f },
                { -4103.143f, -11519.11f, 2.726784f },
                { -4100.143f, -11519.86f, 1.226784f },
                { -4096.893f, -11520.61f, -0.2732162f },
                { -4093.643f, -11520.61f, -2.023216f },
                { -4090.893f, -11520.61f, -3.523216f },
                { -4088.143f, -11520.61f, -5.023216f },
                { -4084.893f, -11520.36f, -6.523216f },
                { -4080.393f, -11520.11f, -8.523216f },
                { -4076.643f, -11519.61f, -10.52322f },
                { -4073.643f, -11518.86f, -12.02322f },
                { -4070.893f, -11518.11f, -13.77322f },
                { -4067.893f, -11517.11f, -15.27322f },
                { -4064.893f, -11516.11f, -16.77322f },
                { -4062.643f, -11514.86f, -18.02322f },
                { -4059.893f, -11513.36f, -19.52322f },
                { -4057.393f, -11511.86f, -21.02322f },
                { -4054.893f, -11510.11f, -22.27322f },
                { -4052.893f, -11508.36f, -23.77322f },
                { -4050.643f, -11506.61f, -25.27322f },
                { -4048.393f, -11504.61f, -26.77322f },
                { -4045.393f, -11501.86f, -28.52322f },
                { -4043.393f, -11499.86f, -29.77322f },
                { -4041.393f, -11497.36f, -31.52322f },
                { -4039.893f, -11495.11f, -33.02322f },
                { -4038.393f, -11492.61f, -34.52322f },
                { -4036.893f, -11489.86f, -36.02322f },
                { -4035.643f, -11487.36f, -37.52322f },
                { -4034.393f, -11484.86f, -38.77322f },
                { -4033.143f, -11481.86f, -40.52322f },
                { -4032.143f, -11479.36f, -41.77322f },
                { -4031.643f, -11476.86f, -43.52322f },
                { -4031.143f, -11473.86f, -45.02322f },
                { -4030.643f, -11470.61f, -46.52322f },
                { -4030.393f, -11467.11f, -48.02322f },
                { -4030.143f, -11464.11f, -49.52322f },
                { -4032.532f, -11448.82f, -56.89236f },
                { -4033.532f, -11445.32f, -58.89236f },
                { -4034.532f, -11441.82f, -61.14236f },
                { -4035.532f, -11438.32f, -63.14236f },
                { -4036.782f, -11435.32f, -64.64236f },
                { -4038.282f, -11432.57f, -66.14236f },
                { -4040.032f, -11429.32f, -67.89236f },
                { -4042.282f, -11426.07f, -69.89236f },
                { -4044.532f, -11423.32f, -72.14236f },
                { -4047.032f, -11420.57f, -74.14236f },
                { -4049.782f, -11418.32f, -75.89236f },
                { -4052.532f, -11416.32f, -77.39236f },
                { -4056.282f, -11413.82f, -79.64236f },
                { -4059.782f, -11411.82f, -82.14236f },
                { -4063.282f, -11410.07f, -84.39236f },
                { -4066.782f, -11408.82f, -86.14236f },
                { -4070.032f, -11407.57f, -87.64236f },
                { -4073.782f, -11406.57f, -89.64236f },
                { -4076.532f, -11405.57f, -91.14236f },
                { -4079.532f, -11405.07f, -93.14236f },
                { -4082.532f, -11404.57f, -94.64236f },
                { -4085.282f, -11404.57f, -96.14236f },
                { -4088.532f, -11404.57f, -97.64236f },
                { -4092.282f, -11404.82f, -99.39236f },
                { -4095.532f, -11405.07f, -101.1424f },
                { -4098.282f, -11405.32f, -102.8924f },
                { -4100.782f, -11406.07f, -104.6424f },
                { -4104.282f, -11407.07f, -106.3924f },
                { -4107.282f, -11408.32f, -107.8924f },
                { -4110.282f, -11409.57f, -109.3924f },
                { -4113.282f, -11411.07f, -111.1424f },
                { -4116.032f, -11412.57f, -113.1424f },
                { -4118.532f, -11414.07f, -114.8924f },
                { -4121.032f, -11415.82f, -116.3924f },
                { -4123.282f, -11417.57f, -117.8924f },
                { -4125.782f, -11420.07f, -119.3924f },
                { -4128.032f, -11422.32f, -120.8924f },
                { -4130.032f, -11424.82f, -122.8924f },
                { -4131.782f, -11426.82f, -124.6424f },
                { -4133.532f, -11429.07f, -126.3924f },
                { -4135.282f, -11431.57f, -127.8924f },
                { -4136.532f, -11433.82f, -129.1424f },
                { -4139.405f, -11445.49f, -130.6486f },
                { -4139.155f, -11449.49f, -130.6486f },
                { -4138.655f, -11453.74f, -130.6486f },
                { -4137.405f, -11457.99f, -130.6486f },
                { -4135.905f, -11462.24f, -130.3986f },
                { -4133.905f, -11465.99f, -130.3986f },
                { -4131.155f, -11469.74f, -130.3986f },
                { -4128.155f, -11473.24f, -130.1486f },
                { -4124.655f, -11476.24f, -130.1486f },
                { -4122.155f, -11478.49f, -129.8986f },
                { -4119.655f, -11480.49f, -129.8986f },
                { -4116.155f, -11482.24f, -131.1486f },
                { -4113.405f, -11483.49f, -132.3986f },
                { -4109.905f, -11484.74f, -133.8986f },
                { -4107.405f, -11485.74f, -134.3986f },
                { -4103.905f, -11485.74f, -134.1486f },
                { -4100.405f, -11485.24f, -133.8986f },
                { -4097.655f, -11483.99f, -133.8986f },
                { -4095.155f, -11481.99f, -134.3986f },
                { -4093.155f, -11479.49f, -135.1486f },
                { -4091.155f, -11476.99f, -135.6486f },
                { -4089.405f, -11474.74f, -136.6486f },
                { -4087.655f, -11472.24f, -138.3986f },
                { -4086.155f, -11469.99f, -140.1486f },
                { -4084.405f, -11467.74f, -141.3986f },
                { -4082.405f, -11465.24f, -141.3986f },
                { -4080.655f, -11462.99f, -141.3986f },
                { -4078.655f, -11460.49f, -141.3986f },
                { -4076.405f, -11457.99f, -141.3986f },
                { -4074.655f, -11455.49f, -141.3986f },
                { -4072.905f, -11452.99f, -141.3986f },
                { -4071.905f, -11450.24f, -141.6486f },
                { -4071.405f, -11447.24f, -141.3986f },
                { -4071.905f, -11443.99f, -141.1486f },
                { -4072.905f, -11441.74f, -141.1486f },
                { -4074.155f, -11439.74f, -141.1486f },
                { -4075.905f, -11437.99f, -141.3986f },
                { -4078.405f, -11435.99f, -141.1486f },
                { -4080.405f, -11434.74f, -141.1486f },
                { -4082.405f, -11433.49f, -141.1486f },
                { -4084.405f, -11432.74f, -141.1486f },
                { -4086.405f, -11431.99f, -141.1486f },
                { -4087.905f, -11430.99f, -141.1486f },
                { -4089.389f, -11430.16f, -141.6313f }
            };

            enum ePoints
            {
                Point_AethasMount = 1,
                Point_AethasMain = 2
            };

            enum eEvents
            {
                Event_SetFacing = 1,
                Event_PathMount = 2,
                Event_Mount = 3,
                Event_StartMainPath = 4
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                m_Events.ScheduleEvent(eEvents::Event_SetFacing, 2000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_AethasMount:
                {
                    me->HandleEmoteCommand(69);
                    m_Events.ScheduleEvent(eEvents::Event_Mount, 2000);
                    break;
                }
                case ePoints::Point_AethasMain:
                {
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                    me->SetFacingTo(0.5934119f);
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
                case eEvents::Event_SetFacing:
                {
                    me->SetFacingTo(1.32645f);
                    m_Events.ScheduleEvent(eEvents::Event_PathMount, 4000);
                    break;
                }
                case eEvents::Event_PathMount:
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_AethasMount, Path_AethasMount, 21, false);
                    break;
                }
                case eEvents::Event_Mount:
                {
                    me->HandleEmoteCommand(Emote::EMOTE_STAND_STATE_NONE);
                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 28889);
                    m_Events.ScheduleEvent(eEvents::Event_StartMainPath, 1000);
                    break;
                }
                case eEvents::Event_StartMainPath:
                {
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_AethasMain, Path_AethasMain, 189, false);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_aethas_sunreaver_128779AI(p_Creature);
        }
};

// Custom Areatriggers - 47223, 47224, 47225
class at_azuremyst_isle : public AreaTriggerEntityScript
{
    public:
        at_azuremyst_isle() : AreaTriggerEntityScript("at_azuremyst_isle") { }

        enum eAreatriggers
        {
            At_HighVindicatorEmoteTrigger = 47223,
            At_HighVindicatorConversationTrigger = 47224,
            At_VelenConversationTrigger = 47225
        };

        enum eNpcs
        {
            Npc_HighVindicator = 122130
        };

        enum eObjectives
        {
            Obj_HighVindicatorEmoteTriggered = 4722300,
            Obj_HighVindicatorConversationheard = 290258,
            Obj_VelenConversationHeard = 290126
        };

        enum eConversations
        {
            Conversation_HighVindicator = 5073,
            Conv_Velen = 5065
        };

        enum eQuests
        {
            Quest_LightsExodus = 47223
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Player* l_Player = p_Target->ToPlayer();
            if (!l_Player || !l_Player->HasQuest(eQuests::Quest_LightsExodus))
                return false;

            switch (p_Areatrigger->GetCustomEntry())
            {
            case eAreatriggers::At_HighVindicatorEmoteTrigger:
            {
                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_HighVindicatorEmoteTriggered) == 1)
                    break;

                std::list<Creature*> l_CreatureList;
                p_Areatrigger->GetCreatureListWithEntryInGrid(l_CreatureList, eNpcs::Npc_HighVindicator, 10.0f);

                if (l_CreatureList.empty())
                    break;

                for (auto l_Itr : l_CreatureList)
                {
                    {
                        l_Itr->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    }
                }

                l_Player->SetQuestObjectiveCounter(eObjectives::Obj_HighVindicatorEmoteTriggered, 1);
                break;
            }
            case eAreatriggers::At_HighVindicatorConversationTrigger:
            {
                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_HighVindicatorConversationheard) == 1)
                    break;

                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conversation_HighVindicator, l_Player, nullptr, l_Player->GetPosition()))
                        delete l_Conversation;
                }

                l_Player->SetQuestObjectiveCounter(eObjectives::Obj_HighVindicatorConversationheard, 1);
                break;
            }
            case eAreatriggers::At_VelenConversationTrigger:
            {
                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_VelenConversationHeard) == 1)
                    break;

                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_Velen, l_Player, nullptr, l_Player->GetPosition()))
                        delete l_Conversation;
                }

                l_Player->SetQuestObjectiveCounter(eObjectives::Obj_VelenConversationHeard, 1);
                break;
            }
            default:
                break;
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_azuremyst_isle();
        }
};

/// 245757 - Activating Beacon
class spell_gen_activating_beacon : public SpellScriptLoader
{
    public:
        spell_gen_activating_beacon() : SpellScriptLoader("spell_gen_activating_beacon") { }

        class spell_gen_activating_beacon_SpellScript : public SpellScript
        {
            enum eNpcs
            {
                Npc_ProphetVelen = 120977
            };

            Position m_VelenSpawnPos = { -4071.439f, -11423.2f, -141.3096f, 2.92989f };

            PrepareSpellScript(spell_gen_activating_beacon_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                l_Caster->SummonCreature(eNpcs::Npc_ProphetVelen, m_VelenSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_activating_beacon_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_activating_beacon_SpellScript();
        }
};

/// 250455 - Boarding
class spell_gen_boarding : public SpellScriptLoader
{
    public:
        spell_gen_boarding() : SpellScriptLoader("spell_gen_boarding") { }

        class spell_gen_boarding_SpellScript : public SpellScript
        {
            enum eKillcredits
            {
                Killcredit_BoardTheExodar = 120981
            };

            PrepareSpellScript(spell_gen_boarding_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_BoardTheExodar);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_boarding_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_boarding_SpellScript();
        }
};

/// Holy Missiles - 243555
class spell_gen_holy_missiles : public SpellScriptLoader
{
    public:
        spell_gen_holy_missiles() : SpellScriptLoader("spell_gen_holy_missiles") { }

        class  spell_gen_holy_missiles_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_holy_missiles_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->PlayOrphanSpellVisual(G3D::Vector3(-4077.366f, -11421.99f, -153.6791f), G3D::Vector3(0, 0, 0), G3D::Vector3(-4077.366f, -11421.99f, -138.6791f), 66785, 5.0f, 0, true, 0.0f);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_holy_missiles_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_holy_missiles_AuraScript();
        }
};

class playerscript_azuremyst_isle_scripts : public PlayerScript
{
    public:
        playerscript_azuremyst_isle_scripts() : PlayerScript("playerscript_azuremyst_isle_scripts") {}

        enum eQuests
        {
            Quest_TheHandOfFate_Alliance = 47221,
            Quest_TheHandOfFate_Horde = 47835,
            Quest_LightsExodus = 47223
        };

        enum eConversations
        {
            Conv_Enter_Exodar = 5063,
            Conv_TheHandOfFateHordeAccept = 5337,
            Conv_TheHandOfFateAllianceAccept = 4904
        };

        enum eObjectives
        {
            Obj_FindProphetVelen = 290126,
            Obj_VelenIntro = 290252
        };

        enum eSpells
        {
            Spell_ToArgus = 241454,
            Spell_ArgusQuestLaunch = 246048
        };

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 p_NewAreaID) override
        {
            if (p_NewAreaID == 8842 && p_Player->GetQuestStatus(eQuests::Quest_LightsExodus) == QUEST_STATUS_COMPLETE &&
                p_Player->GetQuestObjectiveCounter(eObjectives::Obj_VelenIntro) == 0)
            {
                p_Player->SetQuestObjectiveCounter(eObjectives::Obj_VelenIntro, 1);

                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_Enter_Exodar, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }
            }

            if (p_NewZoneID == 7502)
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_ArgusQuestLaunch, true);
            }
        }

        void OnQuestAccept(Player * p_Player, const Quest * p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
            case eQuests::Quest_TheHandOfFate_Alliance:
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_TheHandOfFateAllianceAccept, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            case eQuests::Quest_TheHandOfFate_Horde:
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_TheHandOfFateHordeAccept, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            default:
                break;
            }
        }

        void OnMovieComplete(Player* p_Player, uint32 p_MovieId) override
        {
            if (p_MovieId == 678)
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_ToArgus, true);
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_azuremyst_isle_scripts()
{
    new npc_vindicator_boros_121756();
    new npc_vindicator_boros_128785();
    new npc_vereesa_windrunner_121754();
    new npc_vereesa_windrunner_128784();
    new npc_arator_the_redeemer_128783();
    new npc_prophet_velen_120977();
    new npc_moving_to_beacon_npcs();
    new npc_lady_liadrin_122065();
    new npc_vindicator_boros_128782();
    new npc_lady_liadrin_128781();
    new npc_aethas_sunreaver_128779();
    new at_azuremyst_isle();
    new spell_gen_activating_beacon();
    new spell_gen_boarding();
    new spell_gen_holy_missiles();
    new playerscript_azuremyst_isle_scripts();
}
#endif
