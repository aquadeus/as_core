////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

const std::array<Position, 2> g_ThalysrraHomePositions
{
    {
        { 408.010010f,  3345.179932f,   59.972599f  }, /// Second Wing Arcing Depths Thalyssra
        { 564.328f,     3352.240f,      109.366f    }
    }
};

const std::array<G3D::Vector3, 4> g_ThalysrraPathDepths =
{
    {
        { 419.718f, 3246.610f, 59.8043f }, /// Mid Point For have a better path
        { 457.840f, 3210.469f, 60.9013f }, /// After the First Trash of the Second Wing On the Depths
        { 504.120f, 3209.062f, 85.0989f }, /// Between the Rows with Flowers on the Floor
        { 543.604f, 3208.005f, 109.366f }, /// Before the Depth Door
    }
};

const std::array<Position, 7> g_ThalysrraEscortLastPos =
{
    {
        { 562.213f, 3355.362f, 109.366f, 1.679f },
        { 559.380f, 3355.065f, 109.366f, 1.640f },
        { 561.285f, 3351.186f, 109.366f, 1.640f },
        { 566.373f, 3354.565f, 109.366f, 1.615f },
        { 569.486f, 3355.904f, 109.366f, 1.615f },
        { 569.622f, 3352.882f, 109.366f, 1.615f },
        { 567.063f, 3352.766f, 109.366f, 1.615f }
    }
};

/// First Arcanist Thalyssra - 110791
class npc_nighthold_thalyssra_helper : public CreatureScript
{
    public:
        npc_nighthold_thalyssra_helper() : CreatureScript("npc_nighthold_thalyssra_helper") { }

        enum eSecondWingPoints
        {
            PointMidPathReached     = 20,
            PointAfterTrashKilled   = 21,
            PointRowsWithFlowers    = 22,
            PointBeforeTheDepthDoor = 23,
            PointNearFirstPortal    = 24,
        };

        struct npc_nighthold_thalyssra_helperAI : public ScriptedAI
        {
            npc_nighthold_thalyssra_helperAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);

                m_FirstAcidmaw = false;
                m_FirstVolatile = false;
            }

            bool m_FirstAcidmaw;
            bool m_FirstVolatile;

            std::set<uint64> m_Allies;

            void Reset() override
            {
                AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_Guards;

                    GetCreatureListWithEntryInGrid(l_Guards, me, eCreatures::NpcKirinTorGuardian, 60.0f);
                    GetCreatureListWithEntryInGrid(l_Guards, me, eCreatures::NpcKirinTorGuardian2, 60.0f);
                    GetCreatureListWithEntryInGrid(l_Guards, me, eCreatures::NpcKhadgar, 60.0f);

                    for (Creature* l_Creature : l_Guards)
                        m_Allies.insert(l_Creature->GetGUID());
                });
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionThalyssraRaidIntro:
                    {
                        Talk(6);
                        break;
                    }
                    case eActions::ActionMoveFirstBoss:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, { 58.64583f, 3482.142f, -250.6933f, 0.0f });
                        me->SetHomePosition({ 58.64583f, 3482.142f, -250.6933f, 0.0f });

                        std::multimap<uint32, Position> l_PosMoveByEntry =
                        {
                            { eCreatures::NpcKhadgar,           { 52.7691f, 3477.066f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian2, { 50.9271f, 3479.031f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian2, { 49.4566f, 3477.019f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian2, { 56.6962f, 3484.644f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian2, { 52.5729f, 3482.490f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian,  { 52.2461f, 3474.518f, -250.3848f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian,  { 56.8872f, 3481.563f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian,  { 59.2969f, 3484.340f, -250.6933f, 0.0f } },
                            { eCreatures::NpcKirinTorGuardian,  { 54.7691f, 3477.762f, -250.6933f, 0.0f } }
                        };

                        for (uint64 l_Guid : m_Allies)
                        {
                            if (Creature* l_Creature = Creature::GetCreature(*me, l_Guid))
                            {
                                if (l_PosMoveByEntry.find(l_Creature->GetEntry()) == l_PosMoveByEntry.end())
                                    continue;

                                auto l_Iter = l_PosMoveByEntry.find(l_Creature->GetEntry());

                                Position l_Pos = l_Iter->second;

                                l_PosMoveByEntry.erase(l_Iter);

                                l_Creature->GetMotionMaster()->Clear();
                                l_Creature->GetMotionMaster()->MovePoint(1, l_Pos);
                                l_Creature->SetHomePosition(l_Pos);
                            }
                        }

                        break;
                    }
                    case eActions::ActionMoveAfterFirstBoss:
                    {
                        AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(0);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(2, { 66.15972f, 3414.013f, -249.8891f, 3.804818f });
                            me->SetHomePosition({ 66.15972f, 3414.013f, -249.8891f, 3.804818f });

                            std::multimap<uint32, Position> l_PosMoveByEntry =
                            {
                                { eCreatures::NpcKhadgar,           { 63.27604f, 3420.543f, -249.9609f, 4.171337f } },
                                { eCreatures::NpcKirinTorGuardian2, { 69.30035f, 3414.392f, -249.8829f, 3.403392f } },
                                { eCreatures::NpcKirinTorGuardian2, { 62.47222f, 3424.158f, -249.9946f, 3.403392f } },
                                { eCreatures::NpcKirinTorGuardian2, { 67.04688f, 3418.108f, -249.9134f, 3.926991f } },
                                { eCreatures::NpcKirinTorGuardian2, { 65.72743f, 3420.534f, -249.9420f, 3.769911f } },
                                { eCreatures::NpcKirinTorGuardian,  { 61.46875f, 3419.079f, -249.9637f, 4.607669f } },
                                { eCreatures::NpcKirinTorGuardian,  { 63.77778f, 3414.405f, -250.0087f, 3.909538f } },
                                { eCreatures::NpcKirinTorGuardian,  { 65.35416f, 3411.781f, -249.9224f, 3.839724f } },
                                { eCreatures::NpcKirinTorGuardian,  { 63.46701f, 3417.144f, -249.9336f, 4.101524f } }
                            };

                            for (uint64 l_Guid : m_Allies)
                            {
                                if (Creature* l_Creature = Creature::GetCreature(*me, l_Guid))
                                {
                                    if (l_PosMoveByEntry.find(l_Creature->GetEntry()) == l_PosMoveByEntry.end())
                                        continue;

                                    auto l_Iter = l_PosMoveByEntry.find(l_Creature->GetEntry());

                                    Position l_Pos = l_Iter->second;

                                    l_PosMoveByEntry.erase(l_Iter);

                                    l_Creature->GetMotionMaster()->Clear();
                                    l_Creature->GetMotionMaster()->MovePoint(1, l_Pos);
                                    l_Creature->SetHomePosition(l_Pos);
                                }
                            }
                        });

                        break;
                    }
                    case eActions::ActionFirstMoveToAnomaly:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(3, { 81.99f, 3305.88f, -222.80f, 5.512f });
                        me->SetHomePosition({ 81.99f, 3305.88f, -222.80f, 5.512f });

                        std::multimap<uint32, Position> l_PosMoveByEntry =
                        {
                            { eCreatures::NpcKhadgar,           { 77.01f, 3300.50f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian2, { 73.67f, 3301.19f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian2, { 74.38f, 3304.53f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian2, { 78.32f, 3308.53f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian2, { 81.70f, 3309.91f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian,  { 78.18f, 3296.36f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian,  { 79.65f, 3300.41f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian,  { 82.77f, 3303.64f, -222.80f, 5.512f } },
                            { eCreatures::NpcKirinTorGuardian,  { 86.80f, 3304.90f, -222.80f, 5.512f } }
                        };

                        for (uint64 l_Guid : m_Allies)
                        {
                            if (Creature* l_Creature = Creature::GetCreature(*me, l_Guid))
                            {
                                if (l_PosMoveByEntry.find(l_Creature->GetEntry()) == l_PosMoveByEntry.end())
                                    continue;

                                auto l_Iter = l_PosMoveByEntry.find(l_Creature->GetEntry());

                                Position l_Pos = l_Iter->second;

                                l_PosMoveByEntry.erase(l_Iter);

                                l_Creature->GetMotionMaster()->Clear();
                                l_Creature->GetMotionMaster()->MovePoint(3, l_Pos);
                                l_Creature->SetHomePosition(l_Pos);
                            }
                        }

                        break;
                    }
                    case eActions::ActionSecondMoveToAnomaly:
                    {
                        Talk(1);

                        for (uint64 l_Guid : m_Allies)
                        {
                            if (Creature* l_Ally = Creature::GetCreature(*me, l_Guid))
                            {
                                Position l_Pos = l_Ally->GetPosition();

                                l_Pos.m_positionX += 55.0f * std::cos(5.512f);
                                l_Pos.m_positionY += 55.0f * std::sin(5.512f);
                                l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                                l_Ally->GetMotionMaster()->Clear();
                                l_Ally->GetMotionMaster()->MovePoint(4, l_Pos);

                                l_Ally->SetHomePosition(l_Pos);
                            }

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(4, { 108.634f, 3274.12f, -225.737f, 5.512f });

                            me->SetHomePosition({ 108.634f, 3274.12f, -225.737f, 5.512f });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Point) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_Point)
                {
                    case 1:
                    {
                        Talk(7);
                        break;
                    }
                    case 2:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->SetFacingTo(3.804818f);
                        });

                        break;
                    }
                    case 4:
                    {
                        DoCast(me, eSharedSpells::SpellOpenDoor);
                        break;
                    }
                    case 5:
                    {
                        std::multimap<uint32, Position> l_PosMoveByEntry =
                        {
                            { eCreatures::NpcKhadgar,           { 167.2708f, 3212.835f, -224.8360f, 5.488285f } },
                            { eCreatures::NpcKirinTorGuardian2, { 161.1267f, 3215.479f, -224.5661f, 5.523761f } },
                            { eCreatures::NpcKirinTorGuardian2, { 167.4809f, 3220.704f, -224.5278f, 5.626689f } },
                            { eCreatures::NpcKirinTorGuardian2, { 163.5486f, 3216.747f, -224.5290f, 5.544666f } },
                            { eCreatures::NpcKirinTorGuardian2, { 165.7396f, 3218.762f, -224.4821f, 5.506598f } },
                            { eCreatures::NpcKirinTorGuardian,  { 163.2483f, 3211.765f, -224.6818f, 5.469501f } },
                            { eCreatures::NpcKirinTorGuardian,  { 169.2413f, 3217.798f, -224.6218f, 5.646496f } },
                            { eCreatures::NpcKirinTorGuardian,  { 164.3819f, 3214.399f, -224.6080f, 5.556710f } },
                            { eCreatures::NpcKirinTorGuardian,  { 171.6181f, 3217.943f, -224.6917f, 5.559052f } }
                        };

                        for (uint64 l_Guid : m_Allies)
                        {
                            if (Creature* l_Creature = Creature::GetCreature(*me, l_Guid))
                            {
                                if (l_PosMoveByEntry.find(l_Creature->GetEntry()) == l_PosMoveByEntry.end())
                                    continue;

                                auto l_Iter = l_PosMoveByEntry.find(l_Creature->GetEntry());

                                Position l_Pos = l_Iter->second;

                                l_PosMoveByEntry.erase(l_Iter);

                                l_Creature->GetMotionMaster()->Clear();
                                l_Creature->GetMotionMaster()->MovePoint(6, l_Pos, false);
                                l_Creature->SetHomePosition(l_Pos);
                            }
                        }

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(6, { 168.9444f, 3214.447f, -224.6829f, 5.521536f }, false);

                        me->SetHomePosition({ 168.9444f, 3214.447f, -224.6829f, 5.521536f });
                        break;
                    }
                    case 6:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->SetFacingTo(5.521536f);

                            AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(2);

                                AddTimedDelayedOperation(11 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    Talk(3);

                                    AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        Talk(4);
                                    });
                                });
                            });
                        });

                        break;
                    }

                    case eSharedPoints::PointAlurielDead:
                    {
                        Creature* l_Aluriel = me->FindNearestCreature(eCreatures::BossSpellbladeAluriel, 50.f, false);

                        if (l_Aluriel != nullptr)
                            me->SetFacingToObject(l_Aluriel);

                        break;
                    }

                    case eSecondWingPoints::PointBeforeTheDepthDoor:
                    {
                        me->SetHomePosition(me->GetPosition());
                        break;
                    }

                    case eSecondWingPoints::PointNearFirstPortal:
                    {
                        me->SetHomePosition(me->GetPosition());

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Instance->SetData(eData::DataPreAlurielTrash, EncounterState::DONE);

                        break;
                    }

                    default:
                        break;
                }
            }

            void SetData(uint32 p_Type, uint32 p_Value) override
            {
                switch (p_Type)
                {
                    case eData::DataFirstTrashesKilled:
                    {
                        if (p_Value == eCreatures::NpcTrashAcidmawScorpid)
                            m_FirstAcidmaw = true;
                        else if (p_Value == eCreatures::NpcTrashVolatileScorpid)
                            m_FirstVolatile = true;

                        if (m_FirstAcidmaw && m_FirstVolatile)
                        {
                            for (uint64 l_Guid : m_Allies)
                            {
                                if (Creature* l_Ally = Creature::GetCreature(*me, l_Guid))
                                {
                                    Position l_Pos = l_Ally->GetPosition();

                                    l_Pos.m_positionX += 115.0f * std::cos(5.5f);
                                    l_Pos.m_positionY += 115.0f * std::sin(5.5f);
                                    l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                                    l_Ally->GetMotionMaster()->Clear();
                                    l_Ally->GetMotionMaster()->MovePoint(0, l_Pos);

                                    l_Ally->SetHomePosition(l_Pos);
                                }

                                Position l_Pos = me->GetPosition();

                                l_Pos.m_positionX += 115.0f * std::cos(5.5f);
                                l_Pos.m_positionY += 115.0f * std::sin(5.5f);
                                l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MovePoint(0, l_Pos);

                                me->SetHomePosition(l_Pos);
                            }
                        }

                        break;
                    }

                    case eData::DataSecondTrashesKilled:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eSecondWingPoints::PointBeforeTheDepthDoor, g_ThalysrraPathDepths.data(), g_ThalysrraPathDepths.size(), false);

                        for (const uint64 & l_Itr : m_Allies)
                        {
                            if (Unit* l_Allie = sObjectAccessor->GetUnit(*me, l_Itr))
                                l_Allie->GetMotionMaster()->MovePoint(eSecondWingPoints::PointBeforeTheDepthDoor, g_ThalysrraPathDepths.back());
                        }

                        break;
                    }

                    case eData::DataPreAlurielTrash:
                    {
                        uint32 l_It = 0;
                        me->GetMotionMaster()->MovePoint(eSecondWingPoints::PointNearFirstPortal, g_ThalysrraHomePositions.back());

                        for (const uint64 & l_Itr : m_Allies)
                        {
                            if (Unit* l_Allie = sObjectAccessor->GetUnit(*me, l_Itr))
                            {
                                l_Allie->ToCreature()->SetHomePosition(g_ThalysrraEscortLastPos.at(l_It));
                                l_Allie->GetMotionMaster()->MovePoint(eSecondWingPoints::PointNearFirstPortal, g_ThalysrraEscortLastPos.at(l_It));
                            }

                            l_It++;

                            if (l_It > 7)
                                break;
                        }

                        break;
                    }

                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSharedSpells::SpellOpenDoor)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (GameObject* l_Gob = GameObject::GetGameObject(*me, l_Instance->GetData64(eGameObjects::GoAnomalyPre)))
                                l_Gob->SetGoState(GOState::GO_STATE_ACTIVE);
                        }

                        AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(5, { 122.633f, 3267.984f, -225.737f, 5.512f });

                            me->SetHomePosition({ 122.633f, 3267.984f, -225.737f, 5.512f });
                        });
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                /// Disable interaction while moving
                if (me->IsMoving())
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
                else
                    me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nighthold_thalyssra_helperAI(p_Creature);
        }
};

/// General Script For Handle All Npcs Escort
class npc_nighthold_npc_escort : public CreatureScript
{
    public:
        npc_nighthold_npc_escort() : CreatureScript("npc_nighthold_npc_escort")
        {}

        struct npc_nighthold_npc_escort_AI : public ScriptedAI
        {
            explicit npc_nighthold_npc_escort_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActions::ActionAlurielOutro)
                {
                    DelayTalk(6, eSharedTalks::TalkAlurielOutroStart);

                    AddTimedDelayedOperation(12 * IN_MILLISECONDS, [this] () -> void
                    {
                        me->GetMotionMaster()->MovePoint(eSharedPoints::PointAlurielDead, me->GetHomePosition());
                    });
                }
            }

            void RescheduleOutroTalk()
            {
                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Thalysrra = me->FindNearestCreature(eCreatures::NpcThalyssra, 30.f);

                    if (l_Thalysrra == nullptr)
                    {
                        RescheduleOutroTalk();
                        return;
                    }

                    if (l_Thalysrra != nullptr && l_Thalysrra->IsAIEnabled)
                    {
                        l_Thalysrra->AI()->Talk(eSharedTalks::TalkAlurielOutroStart);
                        DelayTalk(12, eSharedTalks::TalkAlurielOutroEnd);
                        l_Thalysrra->AI()->DelayTalk(22, eSharedTalks::TalkAlurielOutroEnd);

                    }

                    DelayTalk(35, eSharedTalks::TalkKhadgarAlurielEnd);
                });
            }

            void StartAlurielOutro()
            {
                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this] () -> void
                {
                    Creature* l_Thalysrra = me->FindNearestCreature(eCreatures::NpcThalyssra, 30.f);

                    if (l_Thalysrra == nullptr)
                    {
                        RescheduleOutroTalk();
                        return;
                    }

                    if (l_Thalysrra != nullptr && l_Thalysrra->IsAIEnabled)
                    {
                        l_Thalysrra->AI()->Talk(eSharedTalks::TalkAlurielOutroStart);
                        DelayTalk(12, eSharedTalks::TalkAlurielOutroEnd);
                        l_Thalysrra->AI()->DelayTalk(22, eSharedTalks::TalkAlurielOutroEnd);

                    }

                    DelayTalk(35, eSharedTalks::TalkKhadgarAlurielEnd);
                });
            }

            bool IsKirinTorGuard()
            {
                return (me->GetEntry() == eCreatures::NpcKirinTorGuardian || me->GetEntry() == eCreatures::NpcKirinTorGuardian2);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                switch (p_Id)
                {
                    case eSharedPoints::PointAlurielDead:
                    {
                        Creature* l_Aluriel = me->FindNearestCreature(eCreatures::BossSpellbladeAluriel, 20.f, false);

                        if (l_Aluriel != nullptr && !IsKirinTorGuard())
                            me->SetFacingToObject(l_Aluriel);

                        if (me->GetEntry() == eCreatures::NpcKhadgar)
                            StartAlurielOutro();
                        else if (IsKirinTorGuard())
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, EMOTE_STATE_READY1H);

                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                /// Disable interaction while moving
                if (me->IsMoving())
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
                else
                    me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_npc_escort_AI(p_Me);
        }
};

/// Withered Skulker - 113128
class npc_nighthold_withered_skulker : public CreatureScript
{
    public:
        npc_nighthold_withered_skulker() : CreatureScript("npc_nighthold_withered_skulker") { }

        enum eSpells
        {
            SpellWitheringVolley    = 225410,
            SpellMournfulHowl       = 225407
        };

        enum eEvents
        {
            EventWitheringVolley = 1,
            EventMournfulHowl
        };

        struct npc_nighthold_withered_skulkerAI : public ScriptedAI
        {
            npc_nighthold_withered_skulkerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventWitheringVolley, urand(5000, 8000));
                events.ScheduleEvent(eEvents::EventMournfulHowl, urand(5000, 8000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventWitheringVolley:
                    {
                        DoCastAOE(eSpells::SpellWitheringVolley);
                        events.ScheduleEvent(eEvents::EventWitheringVolley, urand(10000, 15000));
                        break;
                    }
                    case eEvents::EventMournfulHowl:
                    {
                        DoCastAOE(eSpells::SpellMournfulHowl);
                        events.ScheduleEvent(eEvents::EventWitheringVolley, urand(10000, 15000));
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
            return new npc_nighthold_withered_skulkerAI(p_Creature);
        }
};

/// Shambling Hungerer - 113321
class npc_nighthold_shambling_hungerer : public CreatureScript
{
    public:
        npc_nighthold_shambling_hungerer() : CreatureScript("npc_nighthold_shambling_hungerer") { }

        enum eSpells
        {
            SpellHungeringFrenzyJump    = 225425,
            SpellHungeringFrenzyChannel = 225509
        };

        enum eEvent
        {
            EventHungeringFrenzy = 1
        };

        struct npc_nighthold_shambling_hungererAI : public ScriptedAI
        {
            npc_nighthold_shambling_hungererAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_HungeringFrenzy;

            void Reset() override
            {
                m_HungeringFrenzy = false;

                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvent::EventHungeringFrenzy, urand(6000, 10000));
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellHungeringFrenzyChannel)
                    m_HungeringFrenzy = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_HungeringFrenzy || me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventHungeringFrenzy:
                    {
                        m_HungeringFrenzy = true;

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellHungeringFrenzyJump, true);

                        events.ScheduleEvent(eEvent::EventHungeringFrenzy, urand(10000, 15000));
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
            return new npc_nighthold_shambling_hungererAI(p_Creature);
        }
};

/// Crystalline Scorpid - 108361
class npc_nighthold_crystalline_scorpid : public CreatureScript
{
    public:
        npc_nighthold_crystalline_scorpid() : CreatureScript("npc_nighthold_crystalline_scorpid") { }

        enum eSpells
        {
            SpellBoonOfTheScorpid   = 205289,
            SpellEnergySurge        = 204766
        };

        enum eEvents
        {
            EventBoonOfTheScorpid = 1,
            EventEnergySurge
        };

        struct npc_nighthold_crystalline_scorpidAI : public ScriptedAI
        {
            npc_nighthold_crystalline_scorpidAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventBoonOfTheScorpid, 10000);
                events.ScheduleEvent(eEvents::EventEnergySurge, urand(3000, 8000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBoonOfTheScorpid:
                    {
                        DoCast(me, eSpells::SpellBoonOfTheScorpid, true);
                        events.ScheduleEvent(eEvents::EventBoonOfTheScorpid, urand(7000, 8000));
                        break;
                    }
                    case eEvents::EventEnergySurge:
                    {
                        DoCastVictim(eSpells::SpellEnergySurge);
                        events.ScheduleEvent(eEvents::EventEnergySurge, urand(10000, 15000));
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
            return new npc_nighthold_crystalline_scorpidAI(p_Creature);
        }
};

/// Acidmaw Scorpid - 108360
class npc_nighthold_acidmaw_scorpid : public CreatureScript
{
    public:
        npc_nighthold_acidmaw_scorpid() : CreatureScript("npc_nighthold_acidmaw_scorpid") { }

        enum eSpells
        {
            SpellBoonOfTheScorpid   = 205289,
            SpellToxicChitinDummy   = 204748,
            SpellToxicChitinMissile = 204751
        };

        enum eEvents
        {
            EventBoonOfTheScorpid = 1,
            EventToxicChitin
        };

        struct npc_nighthold_acidmaw_scorpidAI : public ScriptedAI
        {
            npc_nighthold_acidmaw_scorpidAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventBoonOfTheScorpid, 10000);
                events.ScheduleEvent(eEvents::EventToxicChitin, 5000);
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
            {
                if (p_SpellInfo->Id == eSpells::SpellToxicChitinDummy)
                    me->CastSpell(p_Dest, eSpells::SpellToxicChitinMissile, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellToxicChitinMissile, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBoonOfTheScorpid:
                    {
                        DoCast(me, eSpells::SpellBoonOfTheScorpid, true);
                        events.ScheduleEvent(eEvents::EventBoonOfTheScorpid, urand(7000, 8000));
                        break;
                    }
                    case eEvents::EventToxicChitin:
                    {
                        DoCastVictim(eSpells::SpellToxicChitinDummy, true);
                        events.ScheduleEvent(eEvents::EventToxicChitin, urand(5000, 10000));
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
            return new npc_nighthold_acidmaw_scorpidAI(p_Creature);
        }
};

/// Volatile Scorpid - 108359
class npc_nighthold_volatile_scorpid : public CreatureScript
{
    public:
        npc_nighthold_volatile_scorpid() : CreatureScript("npc_nighthold_volatile_scorpid") { }

        enum eSpells
        {
            SpellScorpidSwarmAT     = 204697,
            SpellBoonOfTheScorpid   = 205289,
            SpellVolatileChitin     = 204733
        };

        enum eEvents
        {
            EventScorpidSwarm = 1,
            EventBoonOfTheScorpid
        };

        struct npc_nighthold_volatile_scorpidAI : public ScriptedAI
        {
            npc_nighthold_volatile_scorpidAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventScorpidSwarm, urand(15000, 20000));
                events.ScheduleEvent(eEvents::EventBoonOfTheScorpid, 10000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellVolatileChitin, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventScorpidSwarm:
                    {
                        DoCast(me, eSpells::SpellScorpidSwarmAT, true);
                        break;
                    }
                    case eEvents::EventBoonOfTheScorpid:
                    {
                        DoCast(me, eSpells::SpellBoonOfTheScorpid, true);
                        events.ScheduleEvent(eEvents::EventBoonOfTheScorpid, urand(7000, 8000));
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
            return new npc_nighthold_volatile_scorpidAI(p_Creature);
        }
};

/// Chaotid - 111075
class npc_nighthold_chaotid : public CreatureScript
{
    public:
        npc_nighthold_chaotid() : CreatureScript("npc_nighthold_chaotid") { }

        enum eSpells
        {
            SpellCompressTheVoidAoE     = 221160,
            SpellCompressTheVoidJump    = 221162,
            SpellReleaseTheVoid         = 221158
        };

        enum eEvents
        {
            EventCompressTheVoid = 1,
            EventReleaseTheVoid
        };

        struct npc_nighthold_chaotidAI : public ScriptedAI
        {
            npc_nighthold_chaotidAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventCompressTheVoid, 7 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellCompressTheVoidAoE:
                    {
                        p_Target->CastSpell(*me, eSpells::SpellCompressTheVoidJump, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCompressTheVoid:
                    {
                        DoCast(me, eSpells::SpellCompressTheVoidAoE);
                        events.ScheduleEvent(eEvents::EventReleaseTheVoid, 100);
                        events.ScheduleEvent(eEvents::EventCompressTheVoid, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventReleaseTheVoid:
                    {
                        DoCast(me, eSpells::SpellReleaseTheVoid, false);
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
            return new npc_nighthold_chaotidAI(p_Creature);
        }
};

/// Pulsauron - 111073
class npc_nighthold_pulsauron_vehicle : public CreatureScript
{
    public:
        npc_nighthold_pulsauron_vehicle() : CreatureScript("npc_nighthold_pulsauron_vehicle") { }

        struct npc_nighthold_pulsauron_vehicleAI : public ScriptedAI
        {
            npc_nighthold_pulsauron_vehicleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Init = false;

            void Reset() override
            {
                if (!m_Init)
                {
                    m_Init = true;

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Creature* l_Pulsauron = me->SummonCreature(eCreatures::NpcPulsauronFighter, *me))
                        {
                            uint64 l_Guid = l_Pulsauron->GetGUID();
                            AddTimedDelayedOperation(100, [this, l_Guid]() -> void
                            {
                                if (Creature* l_Pulsauron = Creature::GetCreature(*me, l_Guid))
                                    l_Pulsauron->EnterVehicle(me);
                            });
                        }
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nighthold_pulsauron_vehicleAI(p_Creature);
        }
};

/// Pulsauron - 111071
class npc_nighthold_pulsauron : public CreatureScript
{
    public:
        npc_nighthold_pulsauron() : CreatureScript("npc_nighthold_pulsauron") { }

        enum eSpells
        {
            SpellShield = 205647,
            SpellBeam   = 221153
        };

        enum eEvents
        {
            EventBeam = 1,
            EventRotate,
            EventPassengerRotate
        };

        struct npc_nighthold_pulsauronAI : public ScriptedAI
        {
            npc_nighthold_pulsauronAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Vehicle(p_Creature->GetVehicleKit()) { }

            Vehicle* m_Vehicle = nullptr;
            uint64 m_Passenger = 0;
            uint64 m_VehicleGuid = 0;

            EventMap m_RotateEvent;

            bool m_Init = false;

            void Reset() override
            {
                if (!m_Init)
                {
                    m_Init = true;

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Unit* l_Vehicle = me->GetVehicleBase())
                            m_VehicleGuid = l_Vehicle->GetGUID();

                        if (Creature* l_Pulsauron = me->SummonCreature(eCreatures::NpcPulsauronShielder, *me))
                        {
                            l_Pulsauron->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            l_Pulsauron->SetReactState(ReactStates::REACT_PASSIVE);

                            m_Passenger = l_Pulsauron->GetGUID();

                            AddTimedDelayedOperation(100, [this]() -> void
                            {
                                if (Creature* l_Pulsauron = Creature::GetCreature(*me, m_Passenger))
                                    l_Pulsauron->EnterVehicle(me);
                            });
                        }
                    });
                }

                events.Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventBeam, 7 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventRotate, 10);
            }

            void OnAddThreat(Unit* /*p_Victim*/, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                /// Pulsaurons cannot be tanked, and have no threat table; the Pulsaurons simply patrol in a defined path.
                p_Threat = 0.0f;
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply && p_Passenger->GetEntry() == eCreatures::NpcPulsauronShielder)
                {
                    p_Passenger->CastSpell(p_Passenger, eSpells::SpellShield, true);

                    p_Passenger->ToCreature()->DisableEvadeMode();
                    p_Passenger->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                    m_RotateEvent.Reset();

                    m_RotateEvent.ScheduleEvent(eEvents::EventPassengerRotate, 10);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBeam)
                    events.RescheduleEvent(eEvents::EventRotate, 10);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                {
                    if (l_Passenger->ToCreature())
                    {
                        l_Passenger->RemoveAllAuras();
                        l_Passenger->DespawnOrUnsummon();
                    }
                }

                if (Unit* l_Unit = Unit::GetUnit(*me, m_VehicleGuid))
                    l_Unit->Kill(l_Unit);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                m_RotateEvent.Update(p_Diff);

                if (m_RotateEvent.ExecuteEvent() == eEvents::EventPassengerRotate && m_Vehicle)
                {
                    if (Unit* l_Passenger = m_Vehicle->GetPassenger(0))
                    {
                        l_Passenger->ClearUnitState(UNIT_STATE_ROOT);

                        l_Passenger->GetMotionMaster()->Clear();
                        l_Passenger->GetMotionMaster()->MoveRotate(30 * TimeConstants::IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_LEFT);
                    }

                    m_RotateEvent.ScheduleEvent(eEvents::EventPassengerRotate, 30 * TimeConstants::IN_MILLISECONDS);
                }

                if (!me->isInCombat())
                {
                    if (Player* l_Player = me->FindNearestPlayer(15.0f))
                        AttackStart(l_Player);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBeam:
                    {
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();

                        DoCast(me, eSpells::SpellBeam);

                        Position l_Pos = me->GetPosition();

                        me->SimplePosXYRelocationByAngle(l_Pos, 5.0f, me->m_orientation, true);

                        if (me->IsWithinLOS(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ))
                        {
                            std::list<Player*> l_PlayerList;
                            me->GetPlayerListInGrid(l_PlayerList, 10.0f);

                            for (Player*& l_Player : l_PlayerList)
                                l_Player->NearTeleportTo(l_Pos);
                        }

                        events.ScheduleEvent(eEvents::EventBeam, 12 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRotate:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveRotate(30 * TimeConstants::IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_RIGHT);

                        events.ScheduleEvent(eEvents::EventRotate, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nighthold_pulsauronAI(p_Creature);
        }
};

/// Fulminant - 111081
class npc_nighthold_fulminant : public CreatureScript
{
    public:
        npc_nighthold_fulminant() : CreatureScript("npc_nighthold_fulminant") { }

        enum eSpells
        {
            SpellScatterMonoTarget  = 221191,
            SpellScatterDamage      = 221172,
            SpellCharge             = 221175,
            SpellScatterAoE         = 221189,
            SpellFulminate          = 221164
        };

        enum eEvents
        {
            EventScatter = 1,
            EventCheckPower,
            EventFulminate
        };

        struct npc_nighthold_fulminantAI : public ScriptedAI
        {
            npc_nighthold_fulminantAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();

                me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventScatter, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCheckPower, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellScatterMonoTarget:
                    {
                        p_Target->CastSpell(me, eSpells::SpellCharge, true);
                        DoCast(p_Target, eSpells::SpellScatterDamage, true);
                        DoCast(p_Target, eSpells::SpellScatterAoE, true);
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

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventScatter:
                    {
                        DoCastVictim(eSpells::SpellScatterMonoTarget);
                        events.ScheduleEvent(eEvents::EventScatter, 6 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCheckPower:
                    {
                        if (me->GetPower(Powers::POWER_ENERGY) >= 100)
                            events.ScheduleEvent(eEvents::EventFulminate, 10);

                        events.ScheduleEvent(eEvents::EventCheckPower, 1 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFulminate:
                    {
                        DoCast(me, eSpells::SpellFulminate);
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
            return new npc_nighthold_fulminantAI(p_Creature);
        }
};

/// SLG Generic Stalker (Gigantic AOI) - 92879
class npc_nighthold_slg_generic_stalker : public CreatureScript
{
    public:
        npc_nighthold_slg_generic_stalker() : CreatureScript("npc_nighthold_slg_generic_stalker") { }

        struct npc_nighthold_slg_generic_stalkerAI : public ScriptedAI
        {
            npc_nighthold_slg_generic_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();

                m_IntroMove = true;
                m_DoorOpening = true;
            }

            InstanceScript* m_Instance;

            std::set<uint64> m_Allies;

            bool m_IntroMove;
            bool m_DoorOpening;

            void Reset() override
            {
                events.Reset();

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_AlliesList;

                    me->GetCreatureListWithEntryInGrid(l_AlliesList, eCreatures::NpcFulminant, 45.0f);
                    me->GetCreatureListWithEntryInGrid(l_AlliesList, eCreatures::NpcChaotoid, 45.0f);

                    for (Creature* l_Creature : l_AlliesList)
                    {
                        if (l_Creature->isAlive())
                            m_Allies.insert(l_Creature->GetGUID());
                    }

                    if (m_Allies.empty())
                    {
                        m_DoorOpening = false;

                        m_Instance->SetData(eData::DataAnomalyIntro, 1);

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (GameObject* l_Gob = GameObject::GetGameObject(*me, l_Instance->GetData64(eGameObjects::GoAnomalyPre)))
                                l_Gob->SetGoState(GOState::GO_STATE_ACTIVE);
                        }
                    }
                });

                if (m_IntroMove)
                    events.ScheduleEvent(EVENT_1, 1000);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_Allies.erase(p_Guid);

                if (m_Allies.empty() && m_DoorOpening)
                {
                    m_DoorOpening = false;

                    m_Instance->SetData(eData::DataAnomalyIntro, 1);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (!m_IntroMove)
                            break;

                        if (Player* l_Player = me->FindNearestPlayer(50.0f))
                        {
                            m_IntroMove = false;

                            m_Instance->SetData(eData::DataAnomalyIntro, 0);
                        }
                        else
                            events.ScheduleEvent(EVENT_1, 1000);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nighthold_slg_generic_stalkerAI(p_Creature);
        }
};

/// Putrid Sludge - 113512
/// Putrid Sludge - 112251
class npc_nighthold_putrid_sludge : public CreatureScript
{
    public:
        npc_nighthold_putrid_sludge() : CreatureScript("npc_nighthold_putrid_sludge") { }

        enum eSpells
        {
            /// Pre-Trilliax trash
            SpellSlopBurst          = 214181,
            SpellSludgeCrawl        = 223511,
            SpellVileSludgePeriodic = 223528,
            SpellVileSludgeDummy    = 223535,
            SpellVileSludgeMissile  = 223537,
            /// Trilliax trash
            SpellSlimePoolSummon    = 223601
        };

        enum eEvents
        {
            EventSlopBurst = 1,
            EventSludgeCrawl
        };

        struct npc_nighthold_putrid_sludgeAI : public ScriptedAI
        {
            npc_nighthold_putrid_sludgeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventSlopBurst, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSludgeCrawl, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSludgeCrawl:
                    {
                        DoCast(me, eSpells::SpellVileSludgePeriodic, true);
                        break;
                    }
                    case eSpells::SpellVileSludgeDummy:
                    {
                        DoCast(me, eSpells::SpellVileSludgeMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->GetEntry() == eCreatures::NpcPutridSludge)
                    DoCast(me, eSpells::SpellSlimePoolSummon, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSlopBurst:
                    {
                        DoCast(me, eSpells::SpellSlopBurst);
                        events.ScheduleEvent(eEvents::EventSlopBurst, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSludgeCrawl:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellSludgeCrawl, true);

                        events.ScheduleEvent(eEvents::EventSludgeCrawl, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nighthold_putrid_sludgeAI(p_Creature);
        }
};

/// Slime Pool - 112293
class npc_nighthold_slime_pool : public CreatureScript
{
    public:
        npc_nighthold_slime_pool() : CreatureScript("npc_nighthold_slime_pool") { }

        enum eSpells
        {
            SpellSlimePoolAura  = 223574
        };

        struct npc_nighthold_slime_poolAI : public ScriptedAI
        {
            npc_nighthold_slime_poolAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellSlimePoolAura, true);

                me->SetWalk(false);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Creature* l_Sludgerax = me->FindNearestCreature(eCreatures::NpcSludgerax, 50.0f))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, l_Sludgerax->GetPosition());
                    }
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->RemoveAura(eSpells::SpellSlimePoolAura);

                    if (Creature* l_Sludgerax = me->FindNearestCreature(eCreatures::NpcSludgerax, 5.0f))
                    {
                        if (l_Sludgerax->IsAIEnabled)
                            l_Sludgerax->AI()->DoAction(c_actions::ACTION_1);
                    }

                    me->DespawnOrUnsummon(2 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nighthold_slime_poolAI(p_Creature);
        }
};

/// Second Wing

/// Nighthold Citizen - 112660
class npc_nighthold_nighthold_citizen : public CreatureScript
{
    public:
        npc_nighthold_nighthold_citizen() : CreatureScript("npc_nighthold_nighthold_citizen")
        {}

        enum eTalks
        {
            TalkPlayerNear,
            TalkAlertGuards,
        };

        enum ePoints
        {
            PointBuildingReached = 10
        };

        struct npc_nighthold_nighthold_citizen_AI : public ScriptedAI
        {
            explicit npc_nighthold_nighthold_citizen_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_Moved = false;
                m_Talk = false;
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 1)
                {
                    Talk(eTalks::TalkAlertGuards);
                    m_Talk = true;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who == nullptr)
                    return;

                if (p_Who->IsPlayer() && me->GetDistance2d(p_Who) < 7.0f && !m_Moved)
                {
                    m_Moved = true;
                    Talk(eTalks::TalkPlayerNear);

                    if (Creature* l_Citizen = me->FindNearestCreature(me->GetEntry(), 10.f, true))
                    {
                        if (!m_Talk && l_Citizen->IsAIEnabled)
                            l_Citizen->GetAI()->DoAction(1);

                    }

                    me->GetMotionMaster()->MovePoint(ePoints::PointBuildingReached, g_CitizenBuildings.at(urand(0, 1)));
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == ePoints::PointBuildingReached)
                    me->SetVisible(false);
            }

            bool m_Talk;
            bool m_Moved;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_nighthold_citizen_AI(p_Me);
        }
};

/// Chronowraith - 113307
class npc_nighthold_chronowraith : public CreatureScript
{
    public:
        npc_nighthold_chronowraith() : CreatureScript("npc_nighthold_chronowraith")
        {}

        enum eEvents
        {
            EventMassSiphon = 1,
        };

        enum eSpells
        {
            SpellMassSiphon = 225412
        };

        struct npc_nighthold_chronowraith_AI : public ScriptedAI
        {
            explicit npc_nighthold_chronowraith_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventMassSiphon, urand(15, 25) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventMassSiphon)
                {
                    DoCastAOE(eSpells::SpellMassSiphon);
                    events.ScheduleEvent(eEvents::EventMassSiphon, urand(20, 30) * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_chronowraith_AI(p_Me);
        }
};

/// Trained Shadescale - 112804
class npc_nighthold_trained_shadescale : public CreatureScript
{
    public:
        npc_nighthold_trained_shadescale() : CreatureScript("npc_nighthold_trained_shadescale")
        {}

        enum eEvents
        {
            EventBlink = 1,
            EventManaburst,
        };

        enum eSpells
        {
            SpellBlink  = 226005,
            SpellManaburst  = 225404
        };

        struct npc_nighthold_trained_shadescale_AI : public ScriptedAI
        {
            explicit npc_nighthold_trained_shadescale_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventBlink, urand(5, 12) * IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::SpellBlink)
                    me->DelayedCastSpell(me, eSpells::SpellManaburst, false, 500);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventBlink)
                {
                    DoCast(me, eSpells::SpellBlink);
                    events.ScheduleEvent(eEvents::EventBlink, urand(10, 20) * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_trained_shadescale_AI(p_Me);
        }
};

/// Duskwatch Battlemagus - 112671
class npc_nighthold_duskwatch_battle_magus : public CreatureScript
{
    public:
        npc_nighthold_duskwatch_battle_magus() : CreatureScript("npc_nighthold_duskwatch_battle_magus")
        {}

        enum eSpells
        {
            SpellCracklingSlice = 224510,
            SpellArcBlade       = 224511,
        };

        enum eEvents
        {
            EventArcBlade   = 1,
            EventCracklingSlice,
        };

        struct npc_nighthold_duskwatch_battle_magus_AI : public ScriptedAI
        {
            explicit npc_nighthold_duskwatch_battle_magus_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Me) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventArcBlade, urand(5, 8) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCracklingSlice, urand(10, 15) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventArcBlade:
                    {
                        DoCastVictim(eSpells::SpellArcBlade);
                        events.ScheduleEvent(eEvents::EventArcBlade, urand(10, 15) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventCracklingSlice:
                    {
                        DoCastVictim(eSpells::SpellCracklingSlice);
                        events.ScheduleEvent(eEvents::EventCracklingSlice, urand(8, 20) * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_duskwatch_battle_magus_AI(p_Me);
        }
};

/// Duskwatch Sentinel - 112675
class npc_nighthold_duskwatch_sentinel : public CreatureScript
{
    public:
        npc_nighthold_duskwatch_sentinel() : CreatureScript("npc_nighthold_duskwatch_sentinel")
        {}

        enum eEvents
        {
            EventSlam   = 1,
            EventProtectiveShield,
        };

        enum eSpells
        {
            SpellSlam               = 224483,
            SpellProtectiveShield   = 225389,
        };

        struct npc_nighthold_duskwatch_sentinel_AI : public ScriptedAI
        {
            explicit npc_nighthold_duskwatch_sentinel_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventSlam:
                    {
                        DoCastVictim(eSpells::SpellSlam);
                        events.ScheduleEvent(eEvents::EventSlam, urand(8, 15) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventProtectiveShield:
                    {
                        DoCast(me, eSpells::SpellProtectiveShield);
                        events.ScheduleEvent(eEvents::EventProtectiveShield, 35 * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventSlam, urand(10, 15) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventProtectiveShield, urand(5, 10) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_duskwatch_sentinel_AI(p_Me);
        }
};

/// Nighthold Protector - 112665
class npc_nighthold_nighthold_protector : public CreatureScript
{
    public:
        npc_nighthold_nighthold_protector() : CreatureScript("npc_nighthold_nighthold_protector")
        {}

        enum eEvents
        {
            EventChargedSmash   = 1,
            EventMassSuppress,
        };

        enum eSpells
        {
            SpellChargedSmash   = 224570,
            SpellMassSuppress   = 224568,
        };

        struct npc_nighthold_nighthold_protector_AI : public ScriptedAI
        {
            explicit npc_nighthold_nighthold_protector_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventChargedSmash, urand(6, 10) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventMassSuppress, urand(15, 25) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventChargedSmash:
                    {
                        DoCastVictim(eSpells::SpellChargedSmash);
                        events.ScheduleEvent(eEvents::EventChargedSmash, urand(15, 25) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventMassSuppress:
                    {
                        DoCastAOE(eSpells::SpellMassSuppress);
                        events.ScheduleEvent(eEvents::EventMassSuppress, urand(12, 18) * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_nighthold_protector_AI(p_Me);
        }
};

/// Nobleborn Warpcaster - 112676
class npc_nighthold_nobleborn_warpcaster : public CreatureScript
{
    public:
        npc_nighthold_nobleborn_warpcaster() : CreatureScript("npc_nighthold_nobleborn_warpcaster")
        {}

        enum eSpells
        {
            SpellWarpedBlast    = 224515,
            SpellReverseWounds  = 224488,
        };

        enum eEvents
        {
            EventWarpedBlast    = 1,
            EventReverseWounds,
        };

        struct npc_nighthold_nobleborn_warpcaster_AI : public ScriptedAI
        {
            explicit npc_nighthold_nobleborn_warpcaster_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* p_Me) override
            {
                events.ScheduleEvent(eEvents::EventWarpedBlast, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventReverseWounds, urand(15, 20) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventWarpedBlast:
                    {
                        DoCastVictim(eSpells::SpellWarpedBlast);
                        events.ScheduleEvent(eEvents::EventWarpedBlast, urand(6, 10) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventReverseWounds:
                    {
                        if (Unit* l_Target = DoSelectLowestHpFriendly(50.f))
                            DoCast(l_Target, eSpells::SpellReverseWounds);
                        else
                            DoCast(me, eSpells::SpellReverseWounds);

                        events.ScheduleEvent(eEvents::EventReverseWounds, urand(15, 20) * IN_MILLISECONDS);
                        break;
                    }

                     default : break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_nobleborn_warpcaster_AI(p_Me);
        }
};

/// Gilded Guardian - 112712
class npc_tnh_gilded_guardian : public CreatureScript
{
    public:
        npc_tnh_gilded_guardian() : CreatureScript("npc_tnh_gilded_guardian")
        {}

        enum eEvents
        {
            EventGravityWell    = 1,
            EventCrushingStomp,
            EventArcBurst,
        };

        enum eSpells
        {
            SpellArcBurst       = 224436,
            SpellGravityWell    = 225927,
            SpellCrushingStomp  = 224440,
        };

        struct npc_tnh_gilded_guardian_AI : public ScriptedAI
        {
            explicit npc_tnh_gilded_guardian_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventGravityWell, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCrushingStomp, 22 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventArcBurst, urand(5, 8) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventArcBurst:
                    {
                        DoCastAOE(eSpells::SpellArcBurst);
                        events.ScheduleEvent(eEvents::EventArcBurst, urand(13, 18) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGravityWell:
                    {
                        DoCast(me, eSpells::SpellGravityWell);
                        events.ScheduleEvent(eEvents::EventGravityWell, 15 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventCrushingStomp:
                    {
                        DoCast(me, eSpells::SpellCrushingStomp);
                        events.ScheduleEvent(eEvents::EventCrushingStomp, 22 * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_gilded_guardian_AI(p_Me);
        }
};

/// Terrace Grover-Tender - 112603
class npc_tnh_terrace_grover_tender : public CreatureScript
{
    public:
        npc_tnh_terrace_grover_tender() : CreatureScript("npc_tnh_terrace_grover_tender")
        {}

        enum eEvents
        {
            EventGrow   = 1,
            EventShrink,
        };

        enum eSpells
        {
            SpellGrow   = 225043,
            SpellShrink = 225047,
        };

        struct npc_tnh_terrace_grover_tender_AI : public ScriptedAI
        {
            explicit npc_tnh_terrace_grover_tender_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventGrow, urand(10, 15) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventShrink, urand(5, 8) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventShrink:
                    {
                        DoCastAOE(eSpells::SpellShrink);
                        events.ScheduleEvent(eEvents::EventShrink, urand(12, 18) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGrow:
                    {
                        auto&& l_AlliesList = DoFindFriendlyCC(30.f);

                        if (!l_AlliesList.empty())
                        {
                            if (auto* l_Target = JadeCore::Containers::SelectRandomContainerElement(l_AlliesList))
                                DoCast(l_Target, eSpells::SpellGrow);
                        }

                        events.ScheduleEvent(eEvents::EventGrow, urand(15, 25) * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_terrace_grover_tender_AI(p_Me);
        }
};

/// Duskwatch Warden - 112596
class npc_nighthold_duskwatch_warden : public CreatureScript
{
    public:
        npc_nighthold_duskwatch_warden() : CreatureScript("npc_nighthold_duskwatch_warden")
        {}

        enum eSpells
        {
            SpellSealedMagic    = 225803,
            SpellForceRepel     = 225581,
        };

        enum eEvents
        {
            EventSealedMagic    = 1,
            EventForceRepel,
        };

        struct npc_nighthold_duskwatch_warden_AI : public ScriptedAI
        {
            explicit npc_nighthold_duskwatch_warden_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventForceRepel, urand(10, 15) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSealedMagic, urand(5, 9) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
                me->RemoveAllAreasTrigger();
            }

            void JustDied(Unit* /**/) override
            {
                me->RemoveAllAreasTrigger();
                events.Reset();
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventForceRepel:
                    {
                        DoCastAOE(eSpells::SpellForceRepel);
                        events.ScheduleEvent(eEvents::EventForceRepel, urand(15, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSealedMagic:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellSealedMagic);

                        events.ScheduleEvent(eEvents::EventSealedMagic, urand(15, 30) * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_duskwatch_warden_AI(p_Me);
        }
};

/// Shal'dorei Naturalist - 112678
class npc_nighthold_shaldorei_naturalist : public CreatureScript
{
    public:
        npc_nighthold_shaldorei_naturalist() : CreatureScript("npc_nighthold_shaldorei_naturalist")
        {}

        enum eEvents
        {
            EventWrath  = 1,
            EventPoisonBrambles,
            EventTorrentOfWater
        };

        enum eSpells
        {
            SpellWrath          = 225855,
            SpellPoisonBrambles = 225856,
            SpellTorrentOfWater = 225859,
        };

        struct npc_nighthold_shaldorei_naturalist_AI : public ScriptedAI
        {
            explicit npc_nighthold_shaldorei_naturalist_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                me->RemoveAllAreasTrigger();
                events.Reset();
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventWrath, urand(4, 6) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventPoisonBrambles, urand(10, 15) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTorrentOfWater, urand(12, 18) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /**/) override
            {
                me->RemoveAllAreasTrigger();
                events.Reset();
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventWrath:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellWrath);

                        events.ScheduleEvent(eEvents::EventWrath, urand(15, 30) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventPoisonBrambles:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellPoisonBrambles);

                        events.ScheduleEvent(eEvents::EventPoisonBrambles, urand(10, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventTorrentOfWater:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellTorrentOfWater);

                        events.ScheduleEvent(eEvents::EventTorrentOfWater, urand(20, 30) * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (const uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_shaldorei_naturalist_AI(p_Me);
        }
};

/// Star Shooting Stalker - 112767
class npc_nighthold_star_shooting_stalker : public CreatureScript
{
  public:
    npc_nighthold_star_shooting_stalker() : CreatureScript("npc_nighthold_star_shooting_stalker")
    {}

    enum eSpells
    {
        SpellFallingStar    = 224484
    };

    struct npc_nighthold_star_shooting_stalker_AI : public ScriptedAI
    {
        explicit npc_nighthold_star_shooting_stalker_AI(Creature* p_Me) : ScriptedAI(p_Me)
        {
        }

        void Reset() override
        {
            m_TimerShoot = irand(2, 4) * IN_MILLISECONDS;
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_TimerShoot -= int32(p_Diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (m_TimerShoot <= 0)
            {
                m_TimerShoot = irand(2,4) * IN_MILLISECONDS;
                DoCast(me, eSpells::SpellFallingStar, false);
            }
        }

        private:
            int32 m_TimerShoot;
    };

    CreatureAI* GetAI(Creature* p_Me) const override
    {
        return new npc_nighthold_star_shooting_stalker_AI(p_Me);
    }
};

/// Astral Defender - 112638
class npc_nighthold_astral_defender : public CreatureScript
{
    public:
        npc_nighthold_astral_defender() : CreatureScript("npc_nighthold_astral_defender")
        {}

        enum eEvents
        {
            EventArcingShatter  = 1,
            EventStellarCrush
        };

        enum eSpells
        {
            SpellArcingShatter  = 224205,
            SpellStellarCrush   = 224181,
        };

        struct npc_nighthold_astral_defender_AI : public ScriptedAI
        {
            explicit npc_nighthold_astral_defender_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void JustDied(Unit* /**/) override
            {
                events.Reset();
                me->RemoveAllAreasTrigger();
            }

            void Reset() override
            {
                me->RemoveAllAreasTrigger();
                events.Reset();
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventArcingShatter, urand(5, 9) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventStellarCrush, urand(15, 25) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                {
                    if (l_EventId == eEvents::EventArcingShatter)
                    {
                        DoCastVictim(eSpells::SpellArcingShatter);
                        events.ScheduleEvent(eEvents::EventArcingShatter, urand(15, 30) * IN_MILLISECONDS);
                    }
                    else if (l_EventId == eEvents::EventStellarCrush)
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true);

                        if (l_Target)
                            DoCast(l_Target, eSpells::SpellStellarCrush);

                        events.ScheduleEvent(eEvents::EventStellarCrush, urand(10, 20) * IN_MILLISECONDS);
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_astral_defender_AI(p_Me);
        }
};

/// Celestial Acolyte - 112655
class npc_nighthold_celestial_acolyte : public CreatureScript
{
  public:
    npc_nighthold_celestial_acolyte() : CreatureScript("npc_nighthold_celestial_acolyte")
    {}

    enum eEvents
    {
        EventLuminousBolt   = 1,
        EventDecideFate,
        EventArcWell,
    };

    enum eSpells
    {
        SpellArcWell        = 224246,
        SpellDecideFate     = 224229,
        SpellLuminousBolt   = 224378,
    };

    struct npc_nighthold_celestial_acolyte_AI : public ScriptedAI
    {
        explicit npc_nighthold_celestial_acolyte_AI(Creature* p_Me) : ScriptedAI(p_Me)
        {}

        void Reset() override
        {
            events.Reset();
            summons.DespawnAll();
        }

        void JustSummoned(Creature* p_Summon) override
        {
            if (!p_Summon)
                return;

            summons.Summon(p_Summon);
            DoZoneInCombat(p_Summon, 50.f);
        }

        void SummonedCreatureDespawn(Creature* p_Summon) override
        {
            if (!p_Summon)
                return;

            summons.Despawn(p_Summon);
        }

        void EnterCombat(Unit* /**/) override
        {
            DoZoneInCombat();
            events.ScheduleEvent(eEvents::EventDecideFate, 20 * IN_MILLISECONDS);
            events.ScheduleEvent(eEvents::EventLuminousBolt, 5 * IN_MILLISECONDS);
            events.ScheduleEvent(eEvents::EventArcWell, urand(8, 16) * IN_MILLISECONDS);
        }

        void JustDied(Unit* /**/) override
        {
            events.Reset();
        }

        void ExecuteEvent(uint32 const p_EventId)
        {
            switch (p_EventId)
            {
                case eEvents::EventLuminousBolt:
                {
                    DoCastVictim(eSpells::SpellLuminousBolt);
                    events.ScheduleEvent(eEvents::EventLuminousBolt, urand(10, 15) * IN_MILLISECONDS);
                    break;
                }

                case eEvents::EventDecideFate:
                {
                    DoCast(me, eSpells::SpellDecideFate);
                    events.ScheduleEvent(eEvents::EventDecideFate, 20 * IN_MILLISECONDS);
                    break;
                }

                case eEvents::EventArcWell:
                {
                    DoCast(me, eSpells::SpellArcWell);
                    events.ScheduleEvent(eEvents::EventArcWell, urand(10, 25) * IN_MILLISECONDS);
                    break;
                }

                default : break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 l_EventId = events.ExecuteEvent())
                ExecuteEvent(l_EventId);

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Me) const override
    {
        return new npc_nighthold_celestial_acolyte_AI(p_Me);
    }
};

/// Arc Well - 112664
class npc_nighthold_arc_well : public CreatureScript
{
    public:
        npc_nighthold_arc_well() : CreatureScript("npc_nighthold_arc_well")
        {
        }

        enum eSpells
        {
            SpellOverflow   = 224321,
            SpellCosmeticArcWell = 224325,
        };

        struct npc_nighthold_arc_well_AI : public ScriptedAI
        {
            explicit npc_nighthold_arc_well_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                m_TimerOverFlow = 0;
                me->CastSpell(me, eSpells::SpellCosmeticArcWell, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_TimerOverFlow += p_Diff;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (m_TimerOverFlow >= 10 * IN_MILLISECONDS)
                {
                    m_TimerOverFlow = 0;
                    DoCast(me, eSpells::SpellOverflow);
                }
            }

            private:
                uint32 m_TimerOverFlow;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_arc_well_AI(p_Me);
        }
};

/// Astrologer Jarin - 112803
class npc_nighthold_astrologer_jarin : public CreatureScript
{
    public:
        npc_nighthold_astrologer_jarin() : CreatureScript("npc_nighthold_astrologer_jarin")
        {}

        enum eEvents
        {
            EventCelestialBrand = 1,
            EventHeavenlyCrash,
        };

        enum eSpells
        {
            SpellCelestialBrand = 224560,
            SpellHeavenlyCrash  = 224632,
        };

        struct npc_nighthold_astrologer_jarin_AI : public ScriptedAI
        {
            explicit npc_nighthold_astrologer_jarin_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                events.Reset();

                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance != nullptr)
                    l_Instance->SetData(eData::DataAstrologerJarin, NOT_STARTED);
            }

            void JustDied(Unit* /**/) override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance)
                    l_Instance->SetData(eData::DataAstrologerJarin, DONE);
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventCelestialBrand, urand(14, 28) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventHeavenlyCrash, urand(5, 10) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                {
                    if (l_EventId == eEvents::EventCelestialBrand)
                    {
                        DoCastAOE(eSpells::SpellCelestialBrand);
                        events.ScheduleEvent(eEvents::EventCelestialBrand, urand(10, 20) * IN_MILLISECONDS);
                    }
                    else if (l_EventId == eEvents::EventHeavenlyCrash)
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellHeavenlyCrash);

                        events.ScheduleEvent(eEvents::EventHeavenlyCrash, urand(12, 18) * IN_MILLISECONDS);
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_astrologer_jarin_AI(p_Me);
        }
};

/// Searing Infernal - 111210
class npc_nighthold_searing_infernal : public CreatureScript
{
    public:
        npc_nighthold_searing_infernal() : CreatureScript("npc_nighthold_searing_infernal")
        {}

        enum eEvents
        {
            EventAnnihilatingOrb    = 1,
            EventConsumingFlame,
        };

        enum eSpells
        {
            SpellAnnihilatingOrb    = 221333,
            SpellConsumingFlame     = 221269,
        };

        struct npc_nighthold_searing_infernal_AI : public ScriptedAI
        {
            explicit npc_nighthold_searing_infernal_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventAnnihilatingOrb:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellAnnihilatingOrb);

                        events.ScheduleEvent(eEvents::EventAnnihilatingOrb, 35 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventConsumingFlame:
                    {
                        DoCastAOE(eSpells::SpellConsumingFlame);
                        events.ScheduleEvent(eEvents::EventConsumingFlame, urand(25, 30) * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(p_Diff);

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nighthold_searing_infernal_AI(p_Me);
        }
};
/// Ill-Fated - 224234
class spell_nighthold_ill_fated : public SpellScriptLoader
{
    public:
        spell_nighthold_ill_fated() : SpellScriptLoader("spell_nighthold_ill_fated")
        {}

        class spell_nighthold_ill_fated_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nighthold_ill_fated_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                TankFilter Cmp;

                p_Targets.sort([&Cmp] (WorldObject*& p_It, WorldObject*& p_It2)
                {
                    return Cmp(p_It->ToPlayer()) && !Cmp(p_It2->ToPlayer());
                });

                p_Targets.remove_if([] (WorldObject*& p_It)
                {
                    return p_It == nullptr || !p_It->IsPlayer() || p_It->ToUnit()->isDead();
                });

                if (p_Targets.size() > 3)
                {
                    auto* l_TargetOne = p_Targets.front();
                    auto* l_TargetTwo = p_Targets.back();

                    p_Targets.pop_back();

                    auto* l_TargetThree = p_Targets.back();

                    p_Targets.clear();

                    p_Targets.push_back(l_TargetOne);
                    p_Targets.push_back(l_TargetTwo);
                    p_Targets.push_back(l_TargetThree);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nighthold_ill_fated_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nighthold_ill_fated_SpellScript();
        }
};

/// Suramar Portal 05 - 225665 - down
/// Suramar Portal 06 - 225673 - down
/// Suramar Portal 05 - 225667 - up
/// Suramar Portal 06 - 225675 - up
class spell_suramar_portal_plr_path : public SpellScriptLoader
{
    public:
        spell_suramar_portal_plr_path() : SpellScriptLoader("spell_suramar_portal_plr_path") { }

        class spell_suramar_portal_plr_path_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_suramar_portal_plr_path_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    ///l_Player->GetMotionMaster()->MoveIdle();

                    switch (GetId())
                    {
                        case eSharedSpells::SpellSuramarPortal05:
                        {
                            l_Player->NearTeleportTo(1135.44f, 4188.93f, 23.64f, 2.59f); ///< Hack
                            ///l_Player->GetMotionMaster()->MovePath(GetId() * 100, false);
                            break;
                        }
                        case eSharedSpells::SpellSuramarPortal05_2:
                        {
                            ///l_Player->GetMotionMaster()->MovePath(GetId() * 100, false);
                            l_Player->NearTeleportTo(1138.89f, 4186.87f, 95.60f, 2.60f); ///< Hack
                            break;
                        }
                        case eSharedSpells::SpellSuramarPortal06:
                        {
                            ///l_Player->GetMotionMaster()->MovePath(GetId() * 100, false);
                            l_Player->NearTeleportTo(1082.38f, 4219.53f, 23.63f, 5.72f); ///< Hack
                            break;
                        }
                        case eSharedSpells::SpellSuramarPortal06_2:
                        {
                            ///l_Player->GetMotionMaster()->MovePath(GetId() * 100, false);
                            l_Player->NearTeleportTo(1084.13f, 4218.51f, 95.60f, 5.80f); ///< Hack
                            break;
                        }
                        default:
                            break;
                    }

                    l_Player->RemoveAurasDueToSpell(GetId()); ///< Hack
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_suramar_portal_plr_path_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_TRANSFORM, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_suramar_portal_plr_path_AuraScript();
        }
};

/// Scatter - 221189
class spell_nighthold_scatter : public SpellScriptLoader
{
    public:
        spell_nighthold_scatter() : SpellScriptLoader("spell_nighthold_scatter") { }

        enum eSpells
        {
            SpellScatterDamage  = 221173,
            SpellCharge         = 221175
        };

        class spell_nighthold_scatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nighthold_scatter_SpellScript);

            uint32 m_TargetCount = 0;

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                m_TargetCount = uint32(p_Targets.size());
            }

            void DealDamage(SpellEffIndex p_EffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (!m_TargetCount)
                            return;

                        int32 l_Damage = GetSpellInfo()->Effects[p_EffIndex].BasePoints / m_TargetCount;

                        l_Caster->CastCustomSpell(l_Target, eSpells::SpellScatterDamage, &l_Damage, nullptr, nullptr, true);
                        l_Target->CastSpell(l_Caster, eSpells::SpellCharge, true);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nighthold_scatter_SpellScript::CountTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_nighthold_scatter_SpellScript::DealDamage, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nighthold_scatter_SpellScript();
        }
};

/// Fulminate - 221164
class spell_nighthold_fulminate : public SpellScriptLoader
{
    public:
        spell_nighthold_fulminate() : SpellScriptLoader("spell_nighthold_fulminate") { }

        class spell_nighthold_fulminate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nighthold_fulminate_SpellScript);

            void DealDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        float l_Distance = l_Target->GetDistance(l_Caster);
                        if (l_Distance >= 15.0f)
                            return;

                        float l_DistPct = l_Distance / 15.0f * 100.0f;

                        int32 l_Damage = CalculatePct(float(GetHitDamage()), l_DistPct);

                        if (l_Damage <= 0)
                        {
                            SetHitDamage(0);
                            return;
                        }

                        SetHitDamage(l_Damage);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_nighthold_fulminate_SpellScript::DealDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nighthold_fulminate_SpellScript();
        }
};

/// Poison Brambles Area - 225856
class at_nh_poison_brambles : public AreaTriggerEntityScript
{
    public:
        at_nh_poison_brambles() : AreaTriggerEntityScript("at_nh_poison_brambles")
        {}

        enum eSpells
        {
            SpellPoisonBramblesDot  = 225856,
        };

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return false;

            if (p_Target->IsPlayer())
                p_Target->CastSpell(p_Target, eSpells::SpellPoisonBramblesDot, true);

            return true;
        }

        bool OnRemoveTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            if (p_Target->IsPlayer())
                p_Target->RemoveAurasDueToSpell(eSpells::SpellPoisonBramblesDot);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nh_poison_brambles();
        }
};

/// Last Update 7.1.5 Build 23420
/// Annihilating Orb (Filter) - 221329
class spell_tnh_annihilating_orb_filter : public SpellScriptLoader
{
    public:
        spell_tnh_annihilating_orb_filter() : SpellScriptLoader("spell_tnh_annihilating_orb_filter") { }

        class spell_tnh_annihilating_orb_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_annihilating_orb_filter_SpellScript);

            enum eSpells
            {
                AnnihilatingOrb = 221333
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AnnihilatingOrb, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tnh_annihilating_orb_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tnh_annihilating_orb_filter_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_annihilating_orb_filter_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Annihilating Orb - 221333
class spell_tnh_annihilating_orb : public SpellScriptLoader
{
    public:
        spell_tnh_annihilating_orb() : SpellScriptLoader("spell_tnh_annihilating_orb") { }

        class spell_tnh_annihilating_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_annihilating_orb_SpellScript);

            enum eSpells
            {
                AnnihilatingOrbAura            = 205344
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster ||!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AnnihilatingOrbAura, true);

                InstanceScript* l_InstanceScript = l_Caster->GetInstanceScript();
                if (!l_InstanceScript || !l_InstanceScript->instance)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::AnnihilatingOrbAura);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AnnihilatingOrbAura, l_InstanceScript->instance->GetDifficultyID());
                if (!l_Aura || !l_SpellInfo)
                    return;

                l_Aura->SetMaxDuration(l_SpellInfo->Effects[EFFECT_0].Amplitude);
                l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_0].Amplitude);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_tnh_annihilating_orb_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_annihilating_orb_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Annihilating Orb (Damage) - 221345
class spell_tnh_annihilating_orb_damage : public SpellScriptLoader
{
    public:
        spell_tnh_annihilating_orb_damage() : SpellScriptLoader("spell_tnh_annihilating_orb_damage") { }

        class spell_tnh_annihilating_orb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_annihilating_orb_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.push_back(l_Caster);
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                SpellRadiusEntry const* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                float l_Pct = std::max(0.25f * l_RadiusEntry->RadiusMax, l_RadiusEntry->RadiusMax - l_Caster->GetExactDist(l_Target)) / l_RadiusEntry->RadiusMax;
                SetHitDamage(static_cast<float>(GetHitDamage()) * l_Pct);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tnh_annihilating_orb_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_tnh_annihilating_orb_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_annihilating_orb_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Consuming Flame (Filter) - 221269
class spell_tnh_consuming_flame : public SpellScriptLoader
{
    public:
        spell_tnh_consuming_flame() : SpellScriptLoader("spell_tnh_consuming_flame") { }

        class spell_tnh_consuming_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_consuming_flame_SpellScript);

            enum eSpells
            {
                ConsumingFlame = 221270
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, std::min<uint32>(p_Targets.size(), 3));
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ConsumingFlame, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tnh_consuming_flame_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tnh_consuming_flame_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_consuming_flame_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Consuming Flame (Filter) - 221465
class spell_tnh_chaotic_energies : public SpellScriptLoader
{
    public:
        spell_tnh_chaotic_energies() : SpellScriptLoader("spell_tnh_chaotic_energies") { }

        class spell_tnh_chaotic_energies_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_chaotic_energies_SpellScript);

            enum eSpells
            {
                ChaoticEnergies = 221460
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, std::min<uint32>(p_Targets.size(), 5));
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ChaoticEnergies, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tnh_chaotic_energies_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tnh_chaotic_energies_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_chaotic_energies_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Seed of Flame - 222101
class spell_tnh_seed_of_flame : public SpellScriptLoader
{
    public:
        spell_tnh_seed_of_flame() : SpellScriptLoader("spell_tnh_seed_of_flame") { }

        class spell_tnh_seed_of_flame_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tnh_seed_of_flame_AuraScript);

            enum eSpells
            {
                RoilingFlame = 222108
            };

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::RoilingFlame, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_tnh_seed_of_flame_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tnh_seed_of_flame_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Midnight Siphoner - 111151
class npc_tnh_midnight_siphoner : public CreatureScript
{
    public:
        npc_tnh_midnight_siphoner() : CreatureScript("npc_tnh_midnight_siphoner") { }

        struct npc_tnh_midnight_siphonerAI : public LegionCombatAI
        {
            npc_tnh_midnight_siphonerAI(Creature* p_Me) : LegionCombatAI(p_Me)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            enum eScriptData
            {
                Trash = 7
            };

            void Reset() override
            {
                LegionCombatAI::Reset();

                if (!me->IsInEvadeMode())
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                else
                    me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 100.0f);
                if (!l_Elisande || !l_Elisande->IsAIEnabled)
                    return;

                l_Elisande->GetAI()->SetGUID(me->GetGUID(), eScriptData::Trash);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_midnight_siphonerAI(p_Me);
        }
};

/// Last Update 7.1.5 Build 23420
/// Twilight Stardancer - 111164
class npc_tnh_twilight_stardancer : public CreatureScript
{
    public:
        npc_tnh_twilight_stardancer() : CreatureScript("npc_tnh_twilight_stardancer") { }

        struct npc_tnh_twilight_stardancerAI : public LegionCombatAI
        {
            npc_tnh_twilight_stardancerAI(Creature* p_Me) : LegionCombatAI(p_Me)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eScriptData
            {
                Trash = 7
            };

            enum eEvents
            {
                Focused = 1
            };

            bool m_IsFocused;

            void Reset() override
            {
                LegionCombatAI::Reset();
                m_IsFocused = true;
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->NearTeleportTo(me->GetHomePosition());

                if (!me->IsInEvadeMode())
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                LegionCombatAI::UpdateAI(p_Diff);

                if (me->IsInEvadeMode() && me->GetDistance(me->GetHomePosition()) < 2.0f)
                    me->ClearUnitState(UNIT_STATE_EVADE);

                if (!m_IsFocused && me->GetReactState() == ReactStates::REACT_PASSIVE)
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                }
            }

            bool AdditionalChecks(uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::Focused:
                        return true;
                    default:
                        return !m_IsFocused;
                }

                return false;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 100.0f);
                if (!l_Elisande || !l_Elisande->IsAIEnabled)
                    return;

                l_Elisande->GetAI()->SetGUID(me->GetGUID(), eScriptData::Trash);
            }

            void OnTaunt(Unit* /*p_Taunter*/) override
            {
                m_IsFocused = false;
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_IsFocused && me->GetHealthPct() < 90.0f)
                    m_IsFocused = false;
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_twilight_stardancerAI(p_Me);
        }
};

/// Last Update 7.1.5 Build 23420
/// Resolute Courtesan - 111166
class npc_tnh_resolute_courtesan : public CreatureScript
{
    public:
        npc_tnh_resolute_courtesan() : CreatureScript("npc_tnh_resolute_courtesan") { }

        struct npc_tnh_resolute_courtesanAI : public LegionCombatAI
        {
            npc_tnh_resolute_courtesanAI(Creature* p_Me) : LegionCombatAI(p_Me) { }

            enum eScriptData
            {
                Trash = 7
            };

            enum eEvents
            {
                EventProtect = 1
            };

            enum eSpells
            {
                Protect = 221524
            };

            std::set<uint64> m_ProtectedTargets;

            Unit* GetTarget(std::list<Unit*>& p_TargetList, uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventProtect:
                    {
                        p_TargetList.remove_if([](Unit* p_Target) -> bool
                        {
                            if (p_Target->GetEntry() == eCreatures::NpcResoluteCourtesan)
                                return true;

                            return false;
                        });

                        break;
                    }
                    default:
                        break;
                }

                Unit* l_Target = LegionCombatAI::GetTarget(p_TargetList, p_EventID);
                if (!l_Target)
                    return nullptr;

                m_ProtectedTargets.insert(l_Target->GetGUID());
                return l_Target;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                for (uint64 l_TargetGUID : m_ProtectedTargets)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    l_Target->RemoveAurasDueToSpell(eSpells::Protect, me->GetGUID());
                }

                Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 100.0f);
                if (!l_Elisande || !l_Elisande->IsAIEnabled)
                    return;

                l_Elisande->GetAI()->SetGUID(me->GetGUID(), eScriptData::Trash);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_resolute_courtesanAI(p_Me);
        }
};

/// Last Update 7.1.5 Build 23420
/// Astral Farseer - 111170
class npc_tnh_astral_farseer : public CreatureScript
{
    public:
        npc_tnh_astral_farseer() : CreatureScript("npc_tnh_astral_farseer") { }

        struct npc_tnh_astral_farseerAI : public LegionCombatAI
        {
            npc_tnh_astral_farseerAI(Creature* p_Me) : LegionCombatAI(p_Me)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eScriptData
            {
                Trash = 7
            };

            enum eEvents
            {
                SpellCasting = 1,
                EventCallToTheStars = 2
            };

            enum eSpells
            {
                CallToTheStars = 221487
            };

            bool m_IsSpellCasting;

            void Reset() override
            {
                LegionCombatAI::Reset();
                m_IsSpellCasting = true;
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->NearTeleportTo(me->GetHomePosition());

                if (!me->IsInEvadeMode())
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                LegionCombatAI::UpdateAI(p_Diff);

                if (me->IsInEvadeMode() && me->GetDistance(me->GetHomePosition()) < 2.0f)
                    me->ClearUnitState(UNIT_STATE_EVADE);

                if (!m_IsSpellCasting && me->GetReactState() == ReactStates::REACT_PASSIVE)
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                    me->InterruptNonMeleeSpells(false, eSpells::CallToTheStars, true);
                }
            }

            bool AdditionalChecks(uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::SpellCasting:
                        return true;
                    case eEvents::EventCallToTheStars:
                        return m_IsSpellCasting;
                    default:
                        return !m_IsSpellCasting;
                }

                return false;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 100.0f);
                if (!l_Elisande || !l_Elisande->IsAIEnabled)
                    return;

                l_Elisande->GetAI()->SetGUID(me->GetGUID(), eScriptData::Trash);
            }

            void OnTaunt(Unit* /*p_Taunter*/) override
            {
                m_IsSpellCasting = false;
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_IsSpellCasting && me->GetHealthPct() < 90.0f)
                    m_IsSpellCasting = false;
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_astral_farseerAI(p_Me);
        }
};

/// Last Update 7.1.5 Build 23420
/// Protect - 221528
class spell_tnh_protect : public SpellScriptLoader
{
    public:
        spell_tnh_protect() : SpellScriptLoader("spell_tnh_protect") { }

        class spell_tnh_protect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_protect_SpellScript);

            enum eSpells
            {
                Protect     = 221251,
                ProtectAura = 221254
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    Unit* l_Unit = (*l_Itr)->ToUnit();
                    if (!l_Unit || l_Caster == l_Unit || l_Unit->HasAura(eSpells::ProtectAura, l_Caster->GetGUID()))
                        l_Itr = p_Targets.erase(l_Itr);
                    else
                        ++l_Itr;
                }
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Protect, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tnh_protect_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_tnh_protect_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_protect_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Siphon Nightwell - 221340
class spell_tnh_siphon_nightwell : public SpellScriptLoader
{
    public:
        spell_tnh_siphon_nightwell() : SpellScriptLoader("spell_tnh_siphon_nightwell") { }

        class spell_tnh_siphon_nightwell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tnh_siphon_nightwell_AuraScript);

            enum eSpells
            {
                ReleaseNightwellEnergy = 221347
            };

            void AfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ReleaseNightwellEnergy, false);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tnh_siphon_nightwell_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tnh_siphon_nightwell_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Call to the Stars - 221487
class spell_tnh_call_to_the_stars : public SpellScriptLoader
{
    public:
        spell_tnh_call_to_the_stars() : SpellScriptLoader("spell_tnh_call_to_the_stars") { }

        class spell_tnh_call_to_the_stars_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tnh_call_to_the_stars_AuraScript);

            enum eSpells
            {
                Starfall = 221488
            };

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_Dest = {};
                uint32 l_I = 0;
                float l_MaxDist = 40.0f;

                do
                    l_Caster->SimplePosXYRelocationByAngle(l_Dest, frand(0.0f, l_MaxDist -= 5.0f), frand(0.0f, 2.0f * M_PI));
                while (!l_Caster->IsWithinLOS(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ) && l_MaxDist > 0.0f);

                if (l_MaxDist <= 0.0f)
                    l_Dest = *l_Caster;

                l_Caster->CastSpell(l_Dest, eSpells::Starfall, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tnh_call_to_the_stars_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tnh_call_to_the_stars_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Sweeping Strike - 221356
class spell_tnh_sweeping_strike : public SpellScriptLoader
{
    public:
        spell_tnh_sweeping_strike() : SpellScriptLoader("spell_tnh_sweeping_strike") { }

        class spell_tnh_sweeping_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_sweeping_strike_SpellScript);

            enum eSpells
            {
                SweepingStrike = 221354
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SweepingStrike, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_tnh_sweeping_strike_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_sweeping_strike_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Swing Weapon - 221614
class spell_tnh_swing_weapon : public SpellScriptLoader
{
    public:
        spell_tnh_swing_weapon() : SpellScriptLoader("spell_tnh_swing_weapon") { }

        class spell_tnh_swing_weapon_Spellscript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_swing_weapon_Spellscript);

            enum eSpells
            {
                SwingWeaponLeft     = 221591,
                SwingWeaponRight    = 221604
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, urand(0, 1) ? eSpells::SwingWeaponLeft : eSpells::SwingWeaponRight, false);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_tnh_swing_weapon_Spellscript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_swing_weapon_Spellscript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Swing Weapon (Left) - 221591 - Swing Weapon (Right) - 221604
class spell_tnh_swing_weapon_damage : public SpellScriptLoader
{
    public:
        spell_tnh_swing_weapon_damage() : SpellScriptLoader("spell_tnh_swing_weapon_damage") { }

        class spell_tnh_swing_weapon_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tnh_swing_weapon_damage_SpellScript);

            enum eSpells
            {
                SwingWeaponLeft     = 221591,
                SwingWeaponRight    = 221604
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_Pos = *l_Caster;
                if (m_scriptSpellId == eSpells::SwingWeaponLeft)
                    l_Pos.m_orientation += M_PI / 2.0f;
                if (m_scriptSpellId == eSpells::SwingWeaponRight)
                    l_Pos.m_orientation -= M_PI / 2.0f;

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    if (!l_Pos.HasInArc(M_PI, *l_Itr))
                        l_Itr = p_Targets.erase(l_Itr);
                    else
                        ++l_Itr;
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tnh_swing_weapon_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tnh_swing_weapon_damage_SpellScript();
        }
};

/// Chosen Fate - 225845
class spell_nh_chosen_fate : public SpellScriptLoader
{
public:
    spell_nh_chosen_fate() : SpellScriptLoader("spell_nh_chosen_fate")
    {}

    enum eSpells
    {
        SpellChosenFateDmg  = 225846,
    };

    class spell_nh_chosen_fate_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_nh_chosen_fate_AuraScript);

        void HandleProc(ProcEventInfo& p_Proc)
        {
            if (GetUnitOwner() == nullptr)
                return;

            DamageInfo* l_Dmg = p_Proc.GetDamageInfo();

            if (l_Dmg == nullptr)
                return;

            GetUnitOwner()->CastCustomSpell(eSpells::SpellChosenFateDmg, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Dmg->GetAmount(), GetUnitOwner(), true);
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_nh_chosen_fate_AuraScript::HandleProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_nh_chosen_fate_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Seed of Flame - 222101

/// Portcullis - 254241
class go_tnh_portcullis : public GameObjectScript
{
    public:
        go_tnh_portcullis() : GameObjectScript("go_tnh_portcullis")
        {}

        void OnGameObjectStateChanged(const GameObject* p_Go, uint32 p_State) override
        {
            if (p_State == GOState::GO_STATE_ACTIVE)
            {
                GameObject* l_Door = const_cast<GameObject*>(p_Go);

                if (l_Door == nullptr)
                    return;

                InstanceScript* l_Instance = l_Door->GetInstanceScript();

                if (l_Instance)
                    l_Instance->SetData(eData::DataPreAlurielTrash, EncounterState::IN_PROGRESS);
            }
        }
};

/// Last Update 7.1.5 Build 23420
/// Suramar Portal - 116662
/// Suramar Portal - 116667
/// Suramar Portal - 116819
/// Suramar Portal - 116820
/// Suramar Portal - 116670
class npc_tnh_teleporter : public CreatureScript
{
    public:
        npc_tnh_teleporter() : CreatureScript("npc_tnh_teleporter") { }

        enum eActions
        {
            TeleportToEntrance          = GOSSIP_ACTION_INFO_DEF + 1,
            TeleportToArcingDepths      = GOSSIP_ACTION_INFO_DEF + 2,
            TeleportToCourtyard         = GOSSIP_ACTION_INFO_DEF + 3,
            TeleportToSecretQuarters    = GOSSIP_ACTION_INFO_DEF + 4,
            TeleportToElisande          = GOSSIP_ACTION_INFO_DEF + 5,
            TeleportToGuldan            = GOSSIP_ACTION_INFO_DEF + 6
        };

        enum eMisc
        {
            MenuID = 11666200,
            TextID = 31058
        };

        enum eQuests
        {
            LoveTap = 44938
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player || !p_Player->GetInstanceScript())
                return false;

            InstanceScript* l_Instance = p_Player->GetInstanceScript();

            p_Player->ADD_GOSSIP_ITEM_DB(eMisc::MenuID, 0, GOSSIP_SENDER_MAIN, eActions::TeleportToEntrance);

            if (l_Instance->GetBossState(eData::DataTrilliax) == EncounterState::DONE)
                p_Player->ADD_GOSSIP_ITEM_DB(eMisc::MenuID, 1, GOSSIP_SENDER_MAIN, eActions::TeleportToArcingDepths);

            if (l_Instance->GetBossState(eData::DataSpellbladeAluriel) == EncounterState::DONE)
            {
                p_Player->ADD_GOSSIP_ITEM_DB(eMisc::MenuID, 2, GOSSIP_SENDER_MAIN, eActions::TeleportToCourtyard);
                if (p_Player->HasQuest(eQuests::LoveTap))
                    p_Player->ADD_GOSSIP_ITEM_DB(eMisc::MenuID, 3, GOSSIP_SENDER_MAIN, eActions::TeleportToSecretQuarters);
            }

            if (l_Instance->GetData(eData::DataHasQuickAccess) || (l_Instance->GetBossState(eData::DataKrosus) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataStarAugurEtraeus) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataHighBotanistTelarn) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataTichondrius) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataSpellbladeAluriel) == EncounterState::DONE) ||
                (l_Instance->instance->IsLFR() && l_Instance->GetBossState(eData::DataKrosus) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataTichondrius) == EncounterState::DONE))
                p_Player->ADD_GOSSIP_ITEM_DB(eMisc::MenuID, 4, GOSSIP_SENDER_MAIN, eActions::TeleportToElisande);

            if (l_Instance->GetBossState(eData::DataElisande) == EncounterState::DONE && !l_Instance->instance->IsLFR())
                p_Player->ADD_GOSSIP_ITEM_DB(eMisc::MenuID, 5, GOSSIP_SENDER_MAIN, eActions::TeleportToGuldan);

            p_Player->SEND_GOSSIP_MENU(eMisc::TextID, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            switch (p_Action)
            {
                case eActions::TeleportToEntrance:
                    p_Player->NearTeleportTo(-112.49f, 3376.22f, -250.82f, 0.80f);
                    break;
                case eActions::TeleportToArcingDepths:
                    p_Player->NearTeleportTo(414.34f, 3362.42f, 61.06f, 3.78f);
                    break;
                case eActions::TeleportToCourtyard:
                    p_Player->NearTeleportTo(428.06f, 3281.03f, 142.02f, 3.89f);
                    break;
                case eActions::TeleportToElisande:
                    p_Player->NearTeleportTo(362.64f, 3055.35f, 216.17f, 2.36f);
                    break;
                case eActions::TeleportToGuldan:
                    p_Player->NearTeleportTo(316.35f, 3135.35f, 465.85f, 3.20f);
                    break;
                case eActions::TeleportToSecretQuarters:
                    p_Player->NearTeleportTo(335.39f, 3083.21f, 289.41f, 2.37f);
                    break;
                default:
                    break;
            }

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_the_nighthold()
{
    /// Teleporters
    new npc_tnh_teleporter();

    /// Creatures
    new npc_nighthold_thalyssra_helper();
    new npc_nighthold_npc_escort();
    new npc_nighthold_withered_skulker();
    new npc_nighthold_shambling_hungerer();
    new npc_nighthold_crystalline_scorpid();
    new npc_nighthold_acidmaw_scorpid();
    new npc_nighthold_volatile_scorpid();
    new npc_nighthold_chaotid();
    new npc_nighthold_pulsauron_vehicle();
    new npc_nighthold_pulsauron();
    new npc_nighthold_fulminant();
    new npc_nighthold_slg_generic_stalker();
    new npc_nighthold_putrid_sludge();
    new npc_nighthold_slime_pool();


    /// Spellblade Aluriel
    new npc_nighthold_nighthold_citizen();
    new npc_nighthold_chronowraith();
    new npc_nighthold_trained_shadescale();
    new npc_nighthold_duskwatch_battle_magus();
    new npc_nighthold_duskwatch_sentinel();
    new npc_nighthold_nighthold_protector();
    new npc_nighthold_nobleborn_warpcaster();
    new npc_tnh_gilded_guardian();
    new npc_tnh_terrace_grover_tender();

    /// High Botanist Tel'arn
    new npc_nighthold_duskwatch_warden();
    new npc_nighthold_shaldorei_naturalist();
    new spell_nh_chosen_fate();

    /// Star Augur Etraeus
    new npc_nighthold_star_shooting_stalker();
    new npc_nighthold_astral_defender();
    new npc_nighthold_celestial_acolyte();
    new npc_nighthold_astrologer_jarin();
    new npc_nighthold_arc_well();

    /// Krosus
    new npc_nighthold_searing_infernal();

    /// Gameobjects
    new go_tnh_portcullis();

    /// Spell
    new spell_suramar_portal_plr_path();
    new spell_nighthold_scatter();
    new spell_nighthold_fulminate();
    new spell_nighthold_ill_fated();

    /// Krosus
    new spell_tnh_annihilating_orb_filter();
    new spell_tnh_annihilating_orb();
    new spell_tnh_annihilating_orb_damage();
    new spell_tnh_consuming_flame();
    new spell_tnh_chaotic_energies();
    new spell_tnh_seed_of_flame();

    /// Elisande
    new npc_tnh_midnight_siphoner();
    new npc_tnh_twilight_stardancer();
    new npc_tnh_resolute_courtesan();
    new npc_tnh_astral_farseer();
    new spell_tnh_protect();
    new spell_tnh_siphon_nightwell();
    new spell_tnh_call_to_the_stars();
    new spell_tnh_sweeping_strike();
    new spell_tnh_swing_weapon();
    new spell_tnh_swing_weapon_damage();

    /// AreaTriggers
    new at_nh_poison_brambles();
}
#endif
