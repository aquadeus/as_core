////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "neltharions_lair.hpp"

/// Rockbound Pelter - 91008
class npc_nl_rockbound_pelter : public CreatureScript
{
    public:
        npc_nl_rockbound_pelter() : CreatureScript("npc_nl_rockbound_pelter") { }

        struct npc_nl_rockbound_pelter_AI : public LegionCombatAI
        {
            explicit npc_nl_rockbound_pelter_AI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                EventJaggedDisc = 1
            };

            bool AdditionalChecks(uint32 p_EventID) override
            {
                if (p_EventID == EventJaggedDisc)
                {
                    Unit* l_Target = me->getVictim();

                    if (l_Target && me->GetDistance(l_Target) > 35.0f)
                        return false;
                }

                return true;
            }
        };

        LegionCombatAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nl_rockbound_pelter_AI(p_Creature);
        }
};

// Blighshard Shaper - 90998
class npc_nl_blightshard_shaper : public CreatureScript
{
    public:
        npc_nl_blightshard_shaper() : CreatureScript("npc_nl_blightshard_shaper") { }

        struct npc_nl_blightshard_shaper_AI : public LegionCombatAI
        {
            explicit npc_nl_blightshard_shaper_AI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                EventStoneBolt = 1
            };

            bool AdditionalChecks(uint32 p_EventID) override
            {
                if (p_EventID == EventStoneBolt)
                {
                    Unit* l_Target = me->getVictim();

                    if (l_Target && me->GetDistance(l_Target) > 40.f)
                        return false;
                }

                return true;
            }
        };

        LegionCombatAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nl_blightshard_shaper_AI(p_Creature);
        }
};

/// Navarrogg <Stonedark Leader> - 100700
class npc_neltharion_navarogg : public CreatureScript
{
    public:
        npc_neltharion_navarogg() : CreatureScript("npc_neltharion_navarogg") { }

        enum eActions
        {
            ActionFirstIntro,
            ActionSecondIntro,
            ActionEndSecondIntro,
            ActionRokmoraDead,
            ActionBarrelUsed,
            ActionNaraxasDead,
            ActionDargrulDead
        };

        enum eMoves
        {
            MoveFirstIntro = 1,
            MoveSecondIntro,
            MoveJumpFirst,
            MovePointFirst,
            MovePointSecond,
            MovePointThird,
            MovePointFourth,
            MovePointFifth,
            MovePointSixth,
            MovePointSeventh,
            MoveAfterRokmora,
            MoveIntroDargrul,
            MoveIntroDargrulSecond,
            MoveIntroDargrulThird
        };

        enum eSpells
        {
            SpellSurfIntro          = 210061,
            SpellIntroConversation  = 208687,
            SpellJumpCosmetic       = 184483,
            SpellBlockCaveIn        = 209517,
            SpellBarrelRide         = 183213,
            SpellNaraxasDeathConv   = 208691,
            SpellDargrulIntroConv   = 209698,
            SpellAllyAttack         = 225785,
            SpellDargrulDeadConv    = 209702
        };

        enum eVisuals
        {
            AnimSecondIntro = 3785
        };

        struct npc_neltharion_navaroggAI : public ScriptedAI
        {
            npc_neltharion_navaroggAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_IntroTriggered        = false;
                m_SecondIntroTriggered  = false;
                m_ThirdIntroNeeded      = false;
                m_FourthIntroNeeded     = false;
                m_Instance              = p_Creature->GetInstanceScript();
                m_BarrelCount           = -1;
            }

            bool m_IntroTriggered;
            bool m_SecondIntroTriggered;
            bool m_ThirdIntroNeeded;
            bool m_FourthIntroNeeded;

            InstanceScript* m_Instance;

            int8 m_BarrelCount;

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFirstIntro:
                    {
                        if (m_IntroTriggered)
                            break;

                        m_IntroTriggered = true;

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::SpellIntroConversation, true);

                            Position l_Pos = { 2950.095f, 1002.082f, 367.0588f, 0.0f };

                            l_Pos.m_orientation = me->GetAngle(&l_Pos);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveFirstIntro, l_Pos);
                        });

                        break;
                    }
                    case eActions::ActionSecondIntro:
                    {
                        if (m_SecondIntroTriggered)
                            break;

                        m_SecondIntroTriggered = true;

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::SpellJumpCosmetic, true);

                            me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);

                            me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 4.5f);

                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 2955.541f, 1392.188f, 57.0529f },
                                { 2938.144f, 1396.052f, 63.0311f },
                                { 2933.304f, 1395.229f, 53.5029f },
                                { 2930.990f, 1395.785f, -2.6415f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveJumpFirst, l_Path.data(), l_Path.size());
                        });

                        break;
                    }
                    case eActions::ActionEndSecondIntro:
                    {
                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetAIAnimKitId(0);

                            me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MovePointSecond, { 2884.442f, 1397.453f, -2.393522f, 0.0f });
                        });

                        break;
                    }
                    case eActions::ActionRokmoraDead:
                    {
                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (Creature* l_Ebonhorn = Creature::GetCreature(*me, m_Instance->GetData64(NPC_SPIRITWALKER_EBONHORN)))
                                l_Ebonhorn->SetVisible(true);

                            me->SetVisible(true);

                            Position l_Pos = { 2822.574f, 1353.080f, 2.7711730f, 0.0f };
                            if (!me->IsNearPosition(&l_Pos, 0.5f))
                                me->NearTeleportTo(l_Pos);

                            std::list<Creature*> l_Barrels;

                            me->GetCreatureListWithEntryInGrid(l_Barrels, NPC_EMPTY_BARREL, 100.0f);

                            for (Creature* l_Iter : l_Barrels)
                            {
                                l_Iter->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                                l_Iter->SetUInt32Value(EUnitFields::UNIT_FIELD_INTERACT_SPELL_ID, eSpells::SpellBarrelRide);
                                l_Iter->SetDynamicValue(EUnitDynamicFields::UNIT_DYNAMIC_FIELD_WORLD_EFFECTS, 0, 2100);
                            }

                            if (Creature* l_Drudge = me->SummonCreature(NPC_NAVARROGG_DRUDGE, { 2789.087f, 1357.24f, 6.74474f, 5.870215f }))
                            {
                                l_Drudge->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                                l_Drudge->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                l_Drudge->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                                std::vector<G3D::Vector3> l_Path
                                {
                                    { 2792.807f, 1357.023f, 6.213045f },
                                    { 2794.057f, 1356.773f, 6.463045f },
                                    { 2795.807f, 1356.773f, 6.213045f },
                                    { 2798.057f, 1356.523f, 6.463045f },
                                    { 2800.057f, 1356.273f, 6.713045f },
                                    { 2801.807f, 1356.023f, 6.963045f },
                                    { 2802.807f, 1356.023f, 7.213045f },
                                    { 2804.057f, 1356.023f, 7.463045f },
                                    { 2805.057f, 1355.773f, 7.463045f },
                                    { 2806.307f, 1355.523f, 7.463045f },
                                    { 2807.807f, 1355.273f, 7.213045f },
                                    { 2809.026f, 1354.806f, 6.681350f }
                                };

                                l_Drudge->GetMotionMaster()->Clear();
                                l_Drudge->GetMotionMaster()->MoveSmoothPath(1, l_Path.data(), l_Path.size(), false);
                            }

                            if (Creature* l_Drudge = me->SummonCreature(NPC_NAVARROGG_DRUDGE, { 2790.699f, 1362.781f, 6.729089f, 5.870215f }))
                            {
                                l_Drudge->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                                l_Drudge->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                l_Drudge->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                                std::vector<G3D::Vector3> l_Path
                                {
                                    { 2793.952f, 1361.378f, 6.267318f },
                                    { 2797.202f, 1360.628f, 6.017318f },
                                    { 2799.952f, 1359.878f, 6.267318f },
                                    { 2801.452f, 1359.628f, 6.267318f },
                                    { 2803.452f, 1359.128f, 7.017318f },
                                    { 2804.952f, 1358.878f, 7.517318f },
                                    { 2808.452f, 1357.878f, 7.267318f },
                                    { 2809.952f, 1357.378f, 6.767318f },
                                    { 2811.452f, 1357.128f, 6.267318f },
                                    { 2812.706f, 1356.475f, 5.805547f }
                                };

                                l_Drudge->GetMotionMaster()->Clear();
                                l_Drudge->GetMotionMaster()->MoveSmoothPath(1, l_Path.data(), l_Path.size(), false);
                            }

                            if (Creature* l_Drudge = me->SummonCreature(NPC_NAVARROGG_DRUDGE, { 2791.377f, 1368.946f, 8.008836f, 5.870215f }))
                            {
                                l_Drudge->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                                l_Drudge->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                l_Drudge->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                                std::vector<G3D::Vector3> l_Path
                                {
                                    { 2792.862f, 1368.277f, 7.815346f },
                                    { 2795.112f, 1367.277f, 7.315346f },
                                    { 2797.112f, 1366.027f, 6.815346f },
                                    { 2798.112f, 1365.527f, 6.565346f },
                                    { 2800.362f, 1364.527f, 6.815346f },
                                    { 2802.862f, 1363.527f, 7.315346f },
                                    { 2804.362f, 1362.777f, 7.565346f },
                                    { 2806.112f, 1362.027f, 8.315346f },
                                    { 2808.112f, 1361.277f, 8.065346f },
                                    { 2809.862f, 1360.777f, 7.565346f },
                                    { 2811.346f, 1360.108f, 7.121857f }
                                };

                                l_Drudge->GetMotionMaster()->Clear();
                                l_Drudge->GetMotionMaster()->MoveSmoothPath(1, l_Path.data(), l_Path.size(), false);
                            }

                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 2816.340f, 1356.812f, 4.999396f },
                                { 2818.090f, 1355.062f, 4.499396f },
                                { 2818.590f, 1354.562f, 4.249396f },
                                { 2819.590f, 1354.312f, 3.999396f },
                                { 2822.340f, 1353.562f, 3.249396f },
                                { 2823.664f, 1353.507f, 2.480890f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveAfterRokmora, l_Path.data(), l_Path.size(), false);

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 1805, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        break;
                    }
                    case eActions::ActionBarrelUsed:
                    {
                        if (m_BarrelCount == -1)
                            m_BarrelCount = int8(me->GetMap()->GetPlayersCountExceptGMs());

                        --m_BarrelCount;

                        if (m_BarrelCount <= 0)
                        {
                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->SetVisible(false);
                                me->NearTeleportTo({ 3034.906f, 1809.625f, -61.17451f, 3.597514f });
                            });
                        }

                        break;
                    }
                    case eActions::ActionNaraxasDead:
                    {
                        me->SetVisible(true);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellNaraxasDeathConv, true);

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->NearTeleportTo({ 2921.605f, 1891.372f, -189.2398f, 5.657896f });

                                m_ThirdIntroNeeded = true;

                                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                                });
                            });
                        });

                        break;
                    }
                    case eActions::ActionDargrulDead:
                    {
                        me->NearTeleportTo({ 2853.846f, 2037.129f, -209.33f, 3.057744f });

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellDargrulDeadConv);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE &&
                    p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveFirstIntro:
                    {
                        me->CastSpell(me, eSpells::SpellSurfIntro, true);
                        break;
                    }
                    case eMoves::MoveSecondIntro:
                    {
                        me->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                        me->RemoveAura(eSpells::SpellSurfIntro);
                        me->NearTeleportTo({ 2959.872f, 1390.531f, 53.52835f, 2.721163f });
                        break;
                    }
                    case eMoves::MoveJumpFirst:
                    {
                        me->RemoveAura(eSpells::SpellJumpCosmetic);

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);

                            me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);

                            Position l_Pos = { 2917.32f, 1402.288f, -2.281739f, 0.0f };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MovePointFirst, l_Pos);
                        });

                        break;
                    }
                    case eMoves::MovePointFirst:
                    {
                        if (m_Instance == nullptr)
                            break;

                        if (Creature* l_Ularogg = Creature::GetCreature(*me, m_Instance->GetData64(NPC_COSMETIC_ULAROGG)))
                        {
                            if (l_Ularogg->IsAIEnabled)
                                l_Ularogg->AI()->DoAction(0);
                        }

                        me->SetAIAnimKitId(eVisuals::AnimSecondIntro);
                        break;
                    }
                    case eMoves::MovePointSecond:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MovePointThird, { 2886.757f, 1399.142f, -2.393522f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MovePointThird:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MovePointFourth, { 2876.358f, 1391.255f, -2.393522f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MovePointFourth:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MovePointFifth, { 2872.275f, 1383.991f, -2.393522f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MovePointFifth:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MovePointSixth, { 2865.902f, 1373.759f, -2.171148f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MovePointSixth:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 2859.331f, 1367.610f, -1.1458840f },
                                { 2858.581f, 1366.860f, -1.1458840f },
                                { 2854.831f, 1364.360f, -0.6458837f },
                                { 2850.581f, 1361.360f, -0.1458837f },
                                { 2848.760f, 1359.962f, -0.1206193f },
                                { 2840.839f, 1355.285f, -0.2376847f },
                                { 2833.364f, 1353.740f, 0.53049100f },
                                { 2830.136f, 1352.953f, 0.70891740f },
                                { 2826.333f, 1353.188f, 1.60055500f },
                                { 2823.333f, 1353.438f, 2.60055500f },
                                { 2822.574f, 1353.080f, 2.77117300f },
                                { 2801.777f, 1360.160f, 6.40802700f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(eMoves::MovePointSeventh, l_Path.data(), l_Path.size(), false);
                        });

                        break;
                    }
                    case eMoves::MovePointSeventh:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->SetFacingTo(6.051625f);
                            me->SetVisible(false);
                        });

                        break;
                    }
                    case eMoves::MoveAfterRokmora:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                        me->CastSpell(me, eSpells::SpellBlockCaveIn, false);
                        break;
                    }
                    case eMoves::MoveIntroDargrul:
                    {
                        DoCast(me, eSpells::SpellAllyAttack);
                        break;
                    }
                    case eMoves::MoveIntroDargrulSecond:
                    {
                        m_FourthIntroNeeded = true;
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                        break;
                    }
                    case eMoves::MoveIntroDargrulThird:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSurfIntro:
                    {
                        std::vector<G3D::Vector3> l_Path =
                        {
                            { 2950.880f, 1014.229f, 325.4731f },
                            { 2958.365f, 1018.120f, 318.2576f },
                            { 2982.388f, 1037.797f, 305.1869f },
                            { 2988.570f, 1066.708f, 289.7835f },
                            { 2976.042f, 1089.498f, 273.5788f },
                            { 2944.292f, 1097.300f, 256.6184f },
                            { 2923.748f, 1085.398f, 245.9293f },
                            { 2910.531f, 1070.689f, 235.4604f },
                            { 2887.073f, 1081.453f, 222.4978f },
                            { 2870.568f, 1098.023f, 184.3552f },
                            { 2864.102f, 1101.613f, 100.5632f }
                        };

                        me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 5.0f);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveSecondIntro, l_Path.data(), l_Path.size());
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellAllyAttack:
                    {
                        std::vector<G3D::Vector3> l_Path =
                        {
                            { 2905.964f, 1933.845f, -193.2781f },
                            { 2903.714f, 1939.095f, -193.7781f },
                            { 2900.964f, 1945.595f, -194.2781f },
                            { 2900.448f, 1946.169f, -194.6447f },
                            { 2898.448f, 1951.669f, -195.1447f },
                            { 2895.698f, 1958.919f, -195.6447f },
                            { 2894.684f, 1961.106f, -196.1990f }
                        };

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveIntroDargrulSecond, l_Path.data(), l_Path.size(), false);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_Instance == nullptr || !p_Who->IsPlayer())
                    return;

                if (m_ThirdIntroNeeded && p_Who->GetDistance(*me) <= 40.0f)
                {
                    m_ThirdIntroNeeded = false;

                    if (Creature* l_Dargrul = Creature::GetCreature(*me, m_Instance->GetData64(NPC_DARGRUL)))
                        l_Dargrul->CastSpell(l_Dargrul, eSpells::SpellDargrulIntroConv, true);

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                        std::vector<G3D::Vector3> l_Path =
                        {
                            { 2910.074f, 1906.721f, -190.9473f },
                            { 2909.324f, 1907.721f, -190.9473f },
                            { 2907.324f, 1911.721f, -191.4473f },
                            { 2904.324f, 1916.971f, -191.9473f },
                            { 2903.122f, 1918.703f, -192.4526f },
                            { 2908.905f, 1927.332f, -192.8631f }
                        };

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveIntroDargrul, l_Path.data(), l_Path.size(), false);
                    });
                }
                else if (m_FourthIntroNeeded && p_Who->GetDistance(*me) <= 10.0f)
                {
                    m_FourthIntroNeeded = false;

                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMoves::MoveIntroDargrulThird, { 2873.823f, 1986.971f, -197.153f, 2.710304f });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_navaroggAI(p_Creature);
        }
};

/// Spiritwalker Ebonhorn - 100701
/// Spiritwalker Ebonhorn - 113526
class npc_neltharion_spiritwalker_ebonhorn : public CreatureScript
{
    public:
        npc_neltharion_spiritwalker_ebonhorn() : CreatureScript("npc_neltharion_spiritwalker_ebonhorn") { }

        enum eActions
        {
            ActionFirstIntro,
            ActionSecondIntro,
            ActionRokmoraDead,
            ActionNaraxasDead,
            ActionLeaves,
            ActionDargrulDead
        };

        enum eMoves
        {
            MoveFirstIntro = 1,
            MoveSecondIntro
        };

        enum eSpells
        {
            SpellSurfIntro          = 210061,
            SpellSafetyExitTeleport = 225759,
            SpellSecondIntroConv    = 225779,
            SpellEbonhornLeaves     = 225794
        };

        enum eEvents
        {
            EventCheckPlayer = 1
        };

        bool OnGossipSelect(Player* p_Player, Creature*, uint32, uint32) override
        {
            if (p_Player == nullptr || !p_Player->IsInWorld())
                return true;

            p_Player->CastSpell(p_Player, eSpells::SpellSafetyExitTeleport, true);
            return false;
        }

        struct npc_neltharion_spiritwalker_ebonhornAI : public ScriptedAI
        {
            npc_neltharion_spiritwalker_ebonhornAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_IntroTriggered       = false;
                m_SecondIntroTriggered = false;
                m_Instance             = me->GetInstanceScript();
            }

            bool m_IntroTriggered;
            bool m_SecondIntroTriggered;

            InstanceScript* m_Instance;

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFirstIntro:
                    {
                        if (m_IntroTriggered)
                            break;

                        m_IntroTriggered = true;

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Position l_Pos = { 2953.357f, 1007.993f, 367.7356f, 0.0f };

                            l_Pos.m_orientation = me->GetAngle(&l_Pos);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveFirstIntro, l_Pos);
                        });

                        break;
                    }
                    case eActions::ActionSecondIntro:
                    {
                        if (m_SecondIntroTriggered)
                            break;

                        m_SecondIntroTriggered = true;

                        me->CastSpell(me, eSpells::SpellSecondIntroConv, true);

                        if (m_Instance)
                        {
                            if (Creature* l_Navarogg = Creature::GetCreature(*me, m_Instance->GetData64(NPC_NAVAROGG)))
                            {
                                if (l_Navarogg->IsAIEnabled)
                                    l_Navarogg->AI()->DoAction(p_Action);
                            }
                        }

                        me->NearTeleportTo({ 2873.241f, 1363.675f, -0.9571614f, 3.083671f });
                        break;
                    }
                    case eActions::ActionRokmoraDead:
                    {
                        AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetVisible(false);
                            me->NearTeleportTo({ 3003.534f, 1790.826f, -61.17309f, 2.07292f });
                        });

                        break;
                    }
                    case eActions::ActionNaraxasDead:
                    {
                        me->SetVisible(true);

                        AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetVisible(true);
                            me->NearTeleportTo({ 2866.835f, 1936.163f, -196.3248f, 2.390640f });

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                            });
                        });

                        break;
                    }
                    case eActions::ActionLeaves:
                    {
                        DoCast(me, eSpells::SpellEbonhornLeaves);

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetVisible(false);
                        });

                        break;
                    }
                    case eActions::ActionDargrulDead:
                    {
                        me->SetVisible(true);
                        me->NearTeleportTo({ 2849.891f, 2028.601f, -209.5919f, 2.564017f });
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE &&
                    p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveFirstIntro:
                    {
                        me->CastSpell(me, eSpells::SpellSurfIntro, true);
                        break;
                    }
                    case eMoves::MoveSecondIntro:
                    {
                        me->SetVisible(false);
                        me->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                        me->RemoveAura(eSpells::SpellSurfIntro);
                        me->NearTeleportTo({ 2925.21f, 1329.115f, 89.18594f, 1.552881f });
                        events.ScheduleEvent(eEvents::EventCheckPlayer, 200);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSurfIntro:
                    {
                        std::vector<G3D::Vector3> l_Path =
                        {
                            { 2950.880f, 1014.229f, 325.4731f },
                            { 2958.365f, 1018.120f, 318.2576f },
                            { 2982.388f, 1037.797f, 305.1869f },
                            { 2988.570f, 1066.708f, 289.7835f },
                            { 2976.042f, 1089.498f, 273.5788f },
                            { 2944.292f, 1097.300f, 256.6184f },
                            { 2923.748f, 1085.398f, 245.9293f },
                            { 2910.531f, 1070.689f, 235.4604f },
                            { 2887.073f, 1081.453f, 222.4978f },
                            { 2870.568f, 1098.023f, 184.3552f },
                            { 2864.102f, 1101.613f, 100.5632f }
                        };

                        me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 5.0f);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMoves::MoveSecondIntro, l_Path.data(), l_Path.size());
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCheckPlayer:
                    {
                        if (me->FindNearestPlayer(10.0f))
                            DoAction(eActions::ActionSecondIntro);
                        else
                            events.ScheduleEvent(eEvents::EventCheckPlayer, 200);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_spiritwalker_ebonhornAI(p_Creature);
        }
};

/// Understone Drudge - 92350
/// Understone Drudge - 105720
/// Understone Drudge - 105636
/// Understone Drudge (Cosmetic) - 105633
/// Understone Drudge (Navarrogg Cosmetic) - 113197
class npc_neltharion_understone_drudge : public CreatureScript
{
    public:
        npc_neltharion_understone_drudge() : CreatureScript("npc_neltharion_understone_drudge") { }

        enum eSpells
        {
            SpellCosmeticRope = 209286,
            SpellCosmeticJump = 209313
        };

        struct npc_neltharion_understone_drudgeAI : public ScriptedAI
        {
            npc_neltharion_understone_drudgeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_IsInCombat;

            bool CanBeTargetedOutOfLOS() override
            {
                return me->GetEntry() == NPC_NAVARROGG_DRUDGE;
            }

            void Reset() override
            {
                m_IsInCombat = false;

                if (me->GetEntry() != NPC_COSMETIC_DRUDGE &&
                    me->GetEntry() != NPC_NAVARROGG_DRUDGE)
                    return;

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                if (me->GetEntry() != NPC_COSMETIC_DRUDGE)
                    return;

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->CastSpell(me, eSpells::SpellCosmeticRope, false);
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (m_IsInCombat)
                    return;

                m_IsInCombat = true;

                me->RemoveAllAuras();

                switch (me->GetEntry())
                {
                    case NPC_COSMETIC_DRUDGE:
                    {
                        me->CastSpell({ 2921.719f, 1343.635f, -5.526f, 4.8273f }, eSpells::SpellCosmeticJump, true);
                        me->DespawnOrUnsummon(5000);
                        Talk(0);
                        return;
                    }
                    case NPC_TALKING_DRUDGE:
                    {
                        Talk(0);
                        break;
                    }
                    default:
                        break;
                }

                std::list<Creature*> l_Allies;

                me->GetCreatureListWithEntryInGridAppend(l_Allies, NPC_COSMETIC_DRUDGE, 40.0f);
                me->GetCreatureListWithEntryInGridAppend(l_Allies, NPC_TALKING_DRUDGE, 40.0f);
                me->GetCreatureListWithEntryInGridAppend(l_Allies, NPC_DRUDGE_1, 40.0f);
                me->GetCreatureListWithEntryInGridAppend(l_Allies, NPC_DRUDGE_2, 40.0f);

                for (Creature* l_Iter : l_Allies)
                {
                    if (l_Iter->GetEntry() == me->GetEntry() || !l_Iter->IsAIEnabled)
                        continue;

                    if (l_Iter->GetEntry() == NPC_COSMETIC_DRUDGE)
                    {
                        l_Iter->AI()->EnterCombat(p_Attacker);
                        continue;
                    }

                    l_Iter->AI()->AttackStart(p_Attacker);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE &&
                    p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    /// After Rokmora's death
                    case 1:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
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

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_understone_drudgeAI(p_Creature);
        }
};

/// Ularogg Cragshaper (Cosmetic) - 105300
class npc_neltharion_cosmetic_ularogg_cragshaper : public CreatureScript
{
    public:
        npc_neltharion_cosmetic_ularogg_cragshaper() : CreatureScript("npc_neltharion_cosmetic_ularogg_cragshaper") { }

        enum eActions
        {
            ActionFirstIntro
        };

        enum eSpells
        {
            SpellIntroEmerge        = 209394,
            SpellIntroConversation  = 209374
        };

        enum eVisuals
        {
            IntroVisual = 1093
        };

        enum eMoves
        {
            MoveFirst = 1,
            MoveSecond,
            MoveThird,
            MoveFourth,
            MoveFifth,
            MoveSixth
        };

        struct npc_neltharion_cosmetic_ularogg_cragshaperAI : public ScriptedAI
        {
            npc_neltharion_cosmetic_ularogg_cragshaperAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFirstIntro:
                    {
                        if (m_Instance == nullptr)
                            return;

                        me->InterruptNonMeleeSpells(true);

                        if (Creature* l_Rokmora = Creature::GetCreature(*me, m_Instance->GetData64(NPC_ROKMORA)))
                            l_Rokmora->CastSpell(l_Rokmora, eSpells::SpellIntroEmerge, false);

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->SetFacingTo(5.480334f);
                        });

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS + 200, [this]() -> void
                        {
                            me->SetAIAnimKitId(eVisuals::IntroVisual);
                            me->CastSpell(me, eSpells::SpellIntroConversation, true);

                            AddTimedDelayedOperation(14 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->SetAIAnimKitId(0);

                                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                                if (Creature* l_Navarrogg = Creature::GetCreature(*me, m_Instance->GetData64(NPC_NAVAROGG)))
                                {
                                    if (l_Navarrogg->IsAIEnabled)
                                        l_Navarrogg->AI()->DoAction(2);
                                }

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MovePoint(eMoves::MoveFirst, { 2884.442f, 1397.453f, -2.393522f, 0.0f });
                            });
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE &&
                    p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveFirst:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveSecond, { 2871.429f, 1382.59f, -2.277433f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MoveSecond:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveThird, { 2864.82f, 1372.012f, -1.961431f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MoveThird:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveFourth, { 2847.353f, 1358.851f, -0.1174455f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MoveFourth:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveFifth, { 2828.669f, 1352.583f, 0.9628236f, 0.0f });
                        });

                        break;
                    }
                    case eMoves::MoveFifth:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 2827.912f, 1353.367f, 1.707013f },
                                { 2825.912f, 1353.367f, 2.457013f },
                                { 2822.912f, 1353.617f, 2.957013f },
                                { 2820.912f, 1353.617f, 3.457013f },
                                { 2818.162f, 1353.867f, 4.207013f },
                                { 2815.162f, 1353.867f, 3.707013f },
                                { 2813.412f, 1354.617f, 5.457013f },
                                { 2810.412f, 1355.867f, 6.707013f },
                                { 2809.162f, 1356.367f, 6.957013f },
                                { 2805.662f, 1357.617f, 7.457013f },
                                { 2804.912f, 1357.867f, 7.457013f },
                                { 2802.412f, 1358.867f, 6.707013f },
                                { 2801.412f, 1359.117f, 6.207013f },
                                { 2800.162f, 1359.367f, 6.207013f },
                                { 2799.297f, 1359.827f, 5.851357f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(eMoves::MoveSixth, l_Path.data(), l_Path.size(), false);
                        });

                        break;
                    }
                    case eMoves::MoveSixth:
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
            return new npc_neltharion_cosmetic_ularogg_cragshaperAI(p_Creature);
        }
};

/// Empty Barrel - 92473
class npc_neltharion_empty_barrel : public CreatureScript
{
    public:
        npc_neltharion_empty_barrel() : CreatureScript("npc_neltharion_empty_barrel") { }

        enum eSpells
        {
            SpellBarrelRide         = 183213,
            SpellLookoutCalloutConv = 209531
        };

        struct npc_neltharion_empty_barrelAI : public ScriptedAI
        {
            npc_neltharion_empty_barrelAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Vehicle(p_Creature->GetVehicleKit())
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                m_Clicked = false;
            }

            Vehicle* m_Vehicle;

            bool m_Clicked;

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (m_Clicked)
                    return;

                m_Clicked = true;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Navarrogg = Creature::GetCreature(*me, l_Instance->GetData64(NPC_NAVAROGG)))
                    {
                        if (l_Navarrogg->IsAIEnabled)
                            l_Navarrogg->AI()->DoAction(4);
                    }
                }

                p_Clicker->CastSpell(p_Clicker, eSpells::SpellBarrelRide, true);
                p_Clicker->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                p_Clicker->SetSpeed(UnitMoveType::MOVE_FLIGHT, 4.0f);

                std::vector<G3D::Vector3> l_Path =
                {
                    { 2816.349f, 1326.556f, -4.435783f },
                    { 2793.588f, 1312.149f, -4.435783f },
                    { 2776.077f, 1281.120f, -4.435783f },
                    { 2766.570f, 1267.863f, -4.435783f },
                    { 2747.937f, 1261.615f, -4.435783f },
                    { 2732.379f, 1259.710f, -4.435783f },
                    { 2717.877f, 1254.504f, -4.435783f },
                    { 2696.255f, 1252.934f, -4.435783f },
                    { 2674.564f, 1263.347f, -4.435783f },
                    { 2644.133f, 1289.913f, -4.435783f },
                    { 2634.784f, 1308.066f, -4.435783f },
                    { 2626.245f, 1332.637f, -4.435783f },
                    { 2622.346f, 1351.427f, -4.435783f },
                    { 2613.220f, 1362.825f, -4.435783f },
                    { 2607.183f, 1378.359f, -4.435783f },
                    { 2593.728f, 1392.705f, -4.435783f },
                    { 2587.470f, 1403.057f, -4.435783f },
                    { 2581.382f, 1415.825f, -4.435783f },
                    { 2571.175f, 1432.349f, -7.388824f },
                    { 2561.087f, 1446.839f, -23.25275f },
                    { 2555.858f, 1454.370f, -56.21880f }
                };

                p_Clicker->GetMotionMaster()->Clear();
                p_Clicker->GetMotionMaster()->MoveSmoothFlyPath(0, l_Path.data(), l_Path.size());

                uint64 l_Guid = p_Clicker->GetGUID();
                AddTimedDelayedOperation(16 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                            l_Player->CastSpell(l_Player, eSpells::SpellLookoutCalloutConv, true);
                    });

                    if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                    {
                        l_Player->RemoveAura(eSpells::SpellBarrelRide);
                        l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                        l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);
                    }
                });

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    m_Clicked = false;
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_empty_barrelAI(p_Creature);
        }
};

/// Blightshard Shaper - 105766
class npc_neltharion_blightshard_shaper : public CreatureScript
{
    public:
        npc_neltharion_blightshard_shaper() : CreatureScript("npc_neltharion_blightshard_shaper") { }

        enum eEvent
        {
            EventCheckPlayer = 1
        };

        enum eSpells
        {
            SpellWormApproachConv   = 209582,
            SpellWormChannel        = 209625,
            SpellWormEmerge         = 209641
        };

        struct npc_neltharion_blightshard_shaperAI : public ScriptedAI
        {
            npc_neltharion_blightshard_shaperAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset() override
            {
                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->SetDisplayId(11686);
                });

                events.ScheduleEvent(eEvent::EventCheckPlayer, 500);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellWormEmerge:
                    {
                        me->SetVisible(false);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventCheckPlayer:
                    {
                        if (Player* l_Player = me->FindNearestPlayer(5.0f))
                        {
                            me->CastSpell(me, eSpells::SpellWormApproachConv, true);
                            me->NearTeleportTo({ 3011.103f, 1810.031f, -61.34912f, 2.453355f });
                            me->SetHomePosition({ 3011.103f, 1810.031f, -61.34912f, 2.453355f });
                            me->RestoreDisplayId();
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            me->CastSpell(me, eSpells::SpellWormChannel, false);
                            break;
                        }

                        events.ScheduleEvent(eEvent::EventCheckPlayer, 500);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_blightshard_shaperAI(p_Creature);
        }
};

/// Vileshard Hulk - 91000
class npc_neltharion_vileshard_hulk : public CreatureScript
{
    public:
        npc_neltharion_vileshard_hulk() : CreatureScript("npc_neltharion_vileshard_hulk") { }

        enum eEvents
        {
            EventFracture = 1,
            EventPiercingShards
        };

        enum eSpells
        {
            SpellFracture               = 193505,
            SpellPiercingShards         = 226296,
            SpellPiercingShardsVisual   = 226305
        };

        struct npc_neltharion_vileshard_hulkAI : public ScriptedAI
        {
            npc_neltharion_vileshard_hulkAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            Position m_PiercingShardsPos;

            bool m_FirstPiercingShards;

            void Reset() override
            {
                m_PiercingShardsPos = Position();
                m_FirstPiercingShards = true;

                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventFracture, 8000);
                events.ScheduleEvent(eEvents::EventPiercingShards, 10000);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellPiercingShards)
                {
                    Position l_Pos  = *me;
                    float l_Angle   = 60.0f * M_PI / 180.0f;

                    for (uint8 l_I = 0; l_I < 93; ++l_I)
                    {
                        float l_Distance = l_Pos.GetExactDist(&m_PiercingShardsPos) + frand(-2.5f, 20.0f);
                        float l_Orientation = l_Pos.GetAngle(&m_PiercingShardsPos) + frand(-l_Angle, l_Angle);

                        float l_X = me->m_positionX + l_Distance * std::cos(l_Orientation);
                        float l_Y = me->m_positionY + l_Distance * std::sin(l_Orientation);
                        float l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, m_PiercingShardsPos.m_positionZ);

                        me->CastSpell({ l_X, l_Y, l_Z, l_Orientation }, eSpells::SpellPiercingShardsVisual, true);
                    }

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    });

                    if (IsMythic() && m_FirstPiercingShards)
                    {
                        events.RescheduleEvent(eEvents::EventPiercingShards, 2500);
                    }
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
                    case eEvents::EventFracture:
                    {
                        DoCast(me, eSpells::SpellFracture);
                        events.ScheduleEvent(eEvents::EventFracture, 18000);
                        break;
                    }
                    case eEvents::EventPiercingShards:
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                        if (Unit* l_Target = me->getVictim())
                        {
                            m_PiercingShardsPos = *l_Target;
                            DoCast(l_Target, eSpells::SpellPiercingShards);
                        }

                        uint32 l_Timer = IsMythic() ? urand(13, 15) * IN_MILLISECONDS : urand(11, 13) * IN_MILLISECONDS;

                        m_FirstPiercingShards = !m_FirstPiercingShards;

                        events.ScheduleEvent(eEvents::EventPiercingShards, l_Timer);
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
            return new npc_neltharion_vileshard_hulkAI(p_Creature);
        }
};

/// Stoneclaw Hunter - 91332
class npc_neltharion_stoneclaw_hunter : public CreatureScript
{
    public:
        npc_neltharion_stoneclaw_hunter() : CreatureScript("npc_neltharion_stoneclaw_hunter") { }

        enum eEvents
        {
            EventKillCommand = 1,
            EventStoneShatter
        };

        enum eSpells
        {
            SpellStoneShatter   = 226347,
            SpellKillCommand    = 202198,
            SpellStoneGaze      = 202181
        };

        struct npc_neltharion_stoneclaw_hunterAI : public ScriptedAI
        {
            npc_neltharion_stoneclaw_hunterAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_BasilikGuid(0) { }

            uint64 m_BasilikGuid;

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (Creature* l_Basilik = Creature::GetCreature(*me, m_BasilikGuid))
                    l_Basilik->SetInCombatWith(p_Attacker);

                events.ScheduleEvent(eEvents::EventKillCommand, 8000);
                events.ScheduleEvent(eEvents::EventStoneShatter, 5000);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                m_BasilikGuid = p_Summon->GetGUID();

                if (me->isInCombat())
                {
                    if (Unit* l_Target = me->getVictim())
                        p_Summon->SetInCombatWith(l_Target);
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer) override
            {
                if (p_Summon->GetGUID() == m_BasilikGuid && me->isAlive())
                {
                    p_Summon->ForcedDespawn(2000);

                    AddTimedDelayedOperation(5000, [this]() -> void
                    {
                        DoCast(me, eNeltharionSpells::SpellHunterBasilik, true);
                    });
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
                    case eEvents::EventKillCommand:
                    {
                        if (Creature* l_Basilik = Creature::GetCreature(*me, m_BasilikGuid))
                        {
                            DoCast(l_Basilik, eSpells::SpellKillCommand, true);

                            if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                                l_Basilik->CastSpell(l_Target, eSpells::SpellStoneGaze, false);
                        }

                        events.ScheduleEvent(eEvents::EventKillCommand, 18000);
                        break;
                    }
                    case eEvents::EventStoneShatter:
                    {
                        if (Unit* l_Target = me->getVictim())
                            DoCast(l_Target, eSpells::SpellStoneShatter);

                        events.ScheduleEvent(eEvents::EventStoneShatter, 15000);
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
            return new npc_neltharion_stoneclaw_hunterAI(p_Creature);
        }
};

/// Burning Geode - 101437
class npc_neltharion_burning_geode : public CreatureScript
{
    public:
        npc_neltharion_burning_geode() : CreatureScript("npc_neltharion_burning_geode") { }

        enum eEvent
        {
            EventScorch = 1
        };

        enum eSpell
        {
            SpellScorch = 202086
        };

        struct npc_neltharion_burning_geodeAI : public ScriptedAI
        {
            npc_neltharion_burning_geodeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED);
                me->AddUnitState(UNIT_STATE_ROOT);

                SetCombatMovement(false);

                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvent::EventScorch, 5000);
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
                    case eEvent::EventScorch:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpell::SpellScorch, true);

                        events.ScheduleEvent(eEvent::EventScorch, 5000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_burning_geodeAI(p_Creature);
        }
};

/// Emberhusk Dominator - 102287
class npc_neltharion_emberhusk_dominator : public CreatureScript
{
    public:
        npc_neltharion_emberhusk_dominator() : CreatureScript("npc_neltharion_emberhusk_dominator") { }

        enum eEvents
        {
            EventFrenzy = 1,
            EventEmberSwipe
        };

        enum eSpells
        {
            SpellFrenzy     = 201983,
            SpellEmberSwipe = 226406
        };

        struct npc_neltharion_emberhusk_dominatorAI : public ScriptedAI
        {
            npc_neltharion_emberhusk_dominatorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_RiderGuid = 0;

            void Reset() override
            {
                if (Creature* l_Rider = Creature::GetCreature(*me, m_RiderGuid))
                    l_Rider->DespawnOrUnsummon();

                AddTimedDelayedOperation(1000, [this]() -> void
                {
                    if (Creature* l_Rider = me->SummonCreature(NPC_EMBERHUSK_RIDER, *me))
                    {
                        m_RiderGuid = l_Rider->GetGUID();

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            if (Creature* l_Rider = Creature::GetCreature(*me, m_RiderGuid))
                                l_Rider->EnterVehicle(me);
                        });
                    }
                });

                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventFrenzy, urand(5000, 10000));
                events.ScheduleEvent(eEvents::EventEmberSwipe, urand(6000, 12000));

                if (Creature* l_Rider = Creature::GetCreature(*me, m_RiderGuid))
                {
                    if (l_Rider->IsAIEnabled)
                        l_Rider->AI()->AttackStart(p_Attacker);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* l_Rider = Creature::GetCreature(*me, m_RiderGuid))
                    me->Kill(l_Rider);
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
                    case eEvents::EventFrenzy:
                    {
                        DoCast(me, eSpells::SpellFrenzy);
                        events.ScheduleEvent(eEvents::EventFrenzy, urand(15000, 20000));
                        break;
                    }
                    case eEvents::EventEmberSwipe:
                    {
                        DoCast(me, eSpells::SpellEmberSwipe);
                        events.ScheduleEvent(eEvents::EventEmberSwipe, urand(6000, 12000));
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
            return new npc_neltharion_emberhusk_dominatorAI(p_Creature);
        }
};

/// Emberhusk Dominator (Rider) - 102295
class npc_neltharion_emberhusk_dominator_rider : public CreatureScript
{
    public:
        npc_neltharion_emberhusk_dominator_rider() : CreatureScript("npc_neltharion_emberhusk_dominator_rider") { }

        enum eEvent
        {
            EventCrystalSpike = 1
        };

        enum eSpells
        {
            SpellCrystalSpikeAoE    = 201959,
            SpellCrystalSpikeDummy  = 201953
        };

        struct npc_neltharion_emberhusk_dominator_riderAI : public ScriptedAI
        {
            npc_neltharion_emberhusk_dominator_riderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvent::EventCrystalSpike, urand(3000, 4500));
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellCrystalSpikeAoE:
                    {
                        DoCast(p_Target, eSpells::SpellCrystalSpikeDummy, true);
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

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventCrystalSpike:
                    {
                        DoCast(me, eSpells::SpellCrystalSpikeAoE);
                        events.ScheduleEvent(eEvent::EventCrystalSpike, urand(3000, 4500));
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_neltharion_emberhusk_dominator_riderAI(p_Creature);
        }
};

/// Permanent Feign Death (NO Stun, Untrackable, Immune) - 159474
class spell_neltharion_perm_feign_death : public SpellScriptLoader
{
    public:
        spell_neltharion_perm_feign_death() : SpellScriptLoader("spell_neltharion_perm_feign_death") { }

        class spell_neltharion_perm_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_neltharion_perm_feign_death_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_SERVER_CONTROLLED);
                    l_Target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH | UNIT_FLAG2_DISABLE_TURN);
                    l_Target->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

                    l_Target->AddUnitState(UNIT_STATE_DIED);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_SERVER_CONTROLLED);
                    l_Target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH | UNIT_FLAG2_DISABLE_TURN);
                    l_Target->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

                    l_Target->ClearUnitState(UNIT_STATE_DIED);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_neltharion_perm_feign_death_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_neltharion_perm_feign_death_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_neltharion_perm_feign_death_AuraScript();
        }
};

/// Petrifying Cloud - 186576
class spell_neltharion_petrifying_cloud : public SpellScriptLoader
{
    public:
        spell_neltharion_petrifying_cloud() : SpellScriptLoader("spell_neltharion_petrifying_cloud") { }

        class spell_neltharion_petrifying_cloud_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_neltharion_petrifying_cloud_AuraScript);

            enum eSpell
            {
                SpellPetrified = 186616
            };

            void HandleUpdate(uint32 p_Diff)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (Aura* l_Aura = GetAura())
                    {
                        if (l_Aura->GetStackAmount() >= 10)
                        {
                            l_Aura->Remove();

                            l_Target->CastSpell(l_Target, eSpell::SpellPetrified, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_neltharion_petrifying_cloud_AuraScript::HandleUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_neltharion_petrifying_cloud_AuraScript();
        }
};

/// Crystal Spike - 201959
class spell_neltharion_crystal_spike_searcher : public SpellScriptLoader
{
    public:
        spell_neltharion_crystal_spike_searcher() : SpellScriptLoader("spell_neltharion_crystal_spike_searcher") { }

        class spell_neltharion_crystal_spike_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_neltharion_crystal_spike_searcher_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                uint32 l_MaxTargets = GetSpellInfo()->MaxAffectedTargets;

                std::list<WorldObject*> l_TempList;

                for (WorldObject* l_Obj : p_Targets)
                {
                    if (Player* l_Player = l_Obj->ToPlayer())
                    {
                        if (l_Player->IsRangedDamageDealer())
                            l_TempList.push_back(l_Player);
                    }
                }

                if (uint32(l_TempList.size()) > l_MaxTargets)
                    JadeCore::RandomResizeList(l_TempList, l_MaxTargets);
                else if (uint32(l_TempList.size()) < l_MaxTargets)
                {
                    for (WorldObject* l_Obj : p_Targets)
                    {
                        if (uint32(l_TempList.size()) >= l_MaxTargets)
                            break;

                        if (Player* l_Player = l_Obj->ToPlayer())
                        {
                            if (l_Player->IsMeleeDamageDealer())
                                l_TempList.push_back(l_Player);
                        }
                    }
                }

                p_Targets = l_TempList;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_neltharion_crystal_spike_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_neltharion_crystal_spike_searcher_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_neltharions_lair()
{
    /// Creatures
    new npc_nl_blightshard_shaper();
    new npc_nl_rockbound_pelter();
    new npc_neltharion_navarogg();
    new npc_neltharion_spiritwalker_ebonhorn();
    new npc_neltharion_understone_drudge();
    new npc_neltharion_cosmetic_ularogg_cragshaper();
    new npc_neltharion_empty_barrel();
    new npc_neltharion_blightshard_shaper();
    new npc_neltharion_vileshard_hulk();
    new npc_neltharion_stoneclaw_hunter();
    new npc_neltharion_burning_geode();
    new npc_neltharion_emberhusk_dominator();

    /// Spells
    new spell_neltharion_perm_feign_death();
    new spell_neltharion_petrifying_cloud();
    new spell_neltharion_crystal_spike_searcher();
}
#endif
