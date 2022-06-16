////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "maw_of_souls.hpp"

enum Says
{
    SAY_INTRO,
    SAY_AGGRO,
    SAY_TAINT_SEA,
    SAY_FIRST_TENTACLE,
    SAY_THIRD_TENTACLE,
    SAY_FOURTH_TENTACLE,
    SAY_FIFTH_TENTACLE,
    SAY_PHASE2,
    SAY_WARN_WING_BUFFET,
    SAY_DEATH,
    SAY_BARRAGE_WARN,
    SAY_CORRUPTED_BELLOW,
    SAY_WIPE,
    SAY_CORRUPTED_BELLOW_WARN
};

enum Spells
{
    /// Intro
    SPELL_SOULLESS_SCREAM           = 194603,
    SPELL_SOULLESS_SCREAM_PACIFY    = 194841,

    SPELL_INTERFERE_TARGETTING      = 197710,
    SPELL_TAINT_OF_THE_SEA          = 197517,
    SPELL_TORRENT_P1_NORMAL         = 197805,
    SPELL_TORRENT_P2                = 198495,
    SPELL_TAINTED_ESSENCE           = 202470,

    /// Phase 2
    SPELL_SUBMERGE                  = 197226,
    SPELL_SUBMERGED                 = 196947, ///< 32:34
    SPELL_TRANSFORM                 = 197734, ///< 32:44 - Self hit
    SPELL_SMASH_ALLOW               = 241950,
    SPELL_SMASH                     = 197689, ///< Force cast all players
    SPELL_WING_BUFFET               = 198520,
    SPELL_CORRUPTED_BELLOW          = 227233,
    SPELL_CORRUPTED_BELLOW_AOE      = 227234,

    /// Tentacle
    SPELL_GRASPING_STUN_LEFT        = 198180,
    SPELL_GRASPING_STUN_RIGHT       = 196450,
    SPELL_PIERCING_TENTACLE         = 197112,
    SPELL_PIERCING_TENTACLE_DMG     = 197117,
    SPELL_SWIRLING_POOL_AT          = 195167,
    SPELL_SWIRLING_POOL_DMG         = 195309,
    SPELL_SWIRLING_POOL_JUMP        = 194839,

    /// Swirling Pool
    SPELL_TURBULENT_WATERS          = 197753,

    /// Heroic
    SPELL_BRACKWATER_BARRAGE        = 202088,
    SPELL_BRACKWATER_BARRAGE_DMG    = 202098,

    /// Trash
    SPELL_GIVE_NO                   = 196885,
    SPELL_DEBILITATING              = 195293,
    SPELL_BIND                      = 195279,
    SPELL_RAPIDE_RUPTURE            = 185539,
    SPELL_SMASH_TRIGGERED           = 196534,
    SPELL_TORRENT_OF_SOULS          = 199519,
    SPELL_BRACKWATER_BLAST          = 201397,

    SPELL_BONUS_ROLL                = 226639,

    SPELL_POOR_UNFORTUNATE_SOUL     = 213413
};

enum eEvents
{
    EVENT_TAINT_SEA = 1,
    EVENT_TORRENT,
    EVENT_SUM_PIERCING_TENTACLE,
    EVENT_SUBMERGED_START,
    EVENT_SUBMERGED_END,
    EVENT_WING_BUFFET,
    EVENT_ACTION_SWIRLING_POOL,
    EVENT_BRACKWATER_BARRAGE,
    EVENT_CORRUPTED_BELLOW,

    /// Trash
    EVENT_SUMMON_1 = 1,
    EVENT_SUMMON_2,
    EVENT_GIVE_NO,
    EVENT_DEBILITATING,
    EVENT_BIND,

    /// Cosmetic
    EVENT_CHECK_PLAYERS = 1
};

enum eVisualKits
{
    VISUAL_KIT_1 = 60920, ///< 30:14 - visual spawn
    VISUAL_KIT_2 = 62877, ///< 30:22 - start intro
    VISUAL_KIT_3 = 60222, ///< 30:28
    VISUAL_KIT_4 = 61048
};

Position const g_TentaclePos[16] =
{
    /// 1 (100% -> 90%)
    { 2933.28f, 960.37f, 512.42f, 4.71f }, ///< NPC_DESTRUCTOR_TENTACLE
    { 2942.16f, 967.02f, 513.79f, 4.08f }, ///< NPC_GRASPING_TENTACLE_1
    { 2925.54f, 965.84f, 512.26f, 5.65f }, ///< NPC_GRASPING_TENTACLE_2

    /// 2 (90% -> 87%)
    { 2919.81f, 959.98f, 513.21f, 5.85f }, ///< NPC_GRASPING_TENTACLE_3

    /// 3 (87% -> 83%)
    { 2933.28f, 960.37f, 512.42f, 4.71f }, ///< NPC_DESTRUCTOR_TENTACLE
    { 2933.40f, 941.15f, 512.42f, 4.71f },
    { 2948.52f, 921.45f, 512.42f, 4.71f },

    /// 4 (83% -> 80%)
    { 2946.55f, 959.81f, 513.23f, 3.58f }, ///< NPC_GRASPING_TENTACLE_4

    /// Swirling Pool VEH - 97099
    { 2939.53f, 928.46f, 512.41f, 1.57f },
    { 2922.20f, 937.07f, 512.41f, 1.57f },
    { 2927.44f, 928.84f, 512.41f, 1.57f },
    { 2948.52f, 921.45f, 512.41f, 1.57f },
    { 2933.40f, 941.15f, 512.41f, 1.57f },
    { 2943.52f, 935.80f, 512.41f, 1.57f },
    { 2918.25f, 921.30f, 512.41f, 1.57f },
    { 2933.34f, 960.28f, 511.94f, 3.14f }
};

/// Skyal
Position const g_AddPos[2] =
{
    { 2942.57f, 886.99f, 537.76f, 4.48f },
    { 2924.55f, 887.20f, 537.65f, 5.26f }
};

uint32 g_SwirlingGo[10]
{
    GO_TRAP_DOOR_001,
    GO_TRAP_DOOR_002,
    GO_TRAP_DOOR_003,
    GO_TRAP_DOOR_004,
    GO_TRAP_DOOR_005,
    GO_TRAP_DOOR_006,
    GO_TRAP_DOOR_007,
    GO_TRAP_DOOR_009,
    GO_TRAP_DOOR_010,
    GO_TRAP_DOOR_011
};

std::vector<std::pair<uint32, Position>> const graspingTentaclePoints
{
    // Left
    { NPC_GRASPING_TENTACLE_2, { 2919.81f, 959.98f, 513.20f, 5.85f } },
    { NPC_GRASPING_TENTACLE_3, { 2917.73f, 952.71f, 513.32f, 6.23f } },
    { NPC_GRASPING_TENTACLE_3, { 2924.31f, 967.26f, 513.32f, 5.60f } },

    // Right
    { NPC_GRASPING_TENTACLE_4, { 2946.55f, 959.81f, 513.23f, 3.58f } },
    { NPC_GRASPING_TENTACLE_1, { 2942.16f, 967.02f, 513.78f, 4.08f } },
    { NPC_GRASPING_TENTACLE_1, { 2949.95f, 951.91f, 513.78f, 3.64f } }
};

/// Helya - 96759
class boss_helya : public CreatureScript
{
    public:
        boss_helya() : CreatureScript("boss_helya") { }

        enum eBarrageData
        {
            MaxBarrage      = 28,
            BarrageVisual   = 52865,
            RightHandVisual = 52869,
            LeftHandVisual  = 52870,
            MaxCorrupted    = 3,
            CorruptedVisual = 61273
        };

        struct boss_helyaAI : public BossAI
        {
            boss_helyaAI(Creature* p_Creature) : BossAI(p_Creature, DATA_HELYA)
            {
                SetCombatMovement(false);
                m_IntroDone = false;
            }

            bool m_IntroDone;
            bool m_EncounterCompleted;
            bool m_InSubmerge;
            bool m_LeftHand;
            uint8 m_TentacleDiedCount;
            uint8 m_HealthSwitch;
            uint32 m_lastTentacleEntry;
            uint16 m_BrackwaterDist;
            uint64 m_LastTentaclePercing;
            uint64 m_DisabledTentacle;

            EventMap m_CosmeticEvent;
            EventMap m_BypassCastEvents;

            bool m_FirstTaint = true;
            bool m_FirstSubmerge = true;

            std::array<std::pair<float, Position>, eBarrageData::MaxBarrage> m_BrackwaterVisuals =
            {
                {
                    { 1.273846f, { 2929.071f, 960.5070f, 512.3322f, 0.0f } },
                    { 1.311667f, { 2923.645f, 958.0625f, 512.3322f, 0.0f } },
                    { 1.340964f, { 2928.606f, 955.4896f, 512.3322f, 0.0f } },
                    { 1.374767f, { 2922.828f, 953.3889f, 512.3322f, 0.0f } },
                    { 1.419930f, { 2918.421f, 950.5208f, 512.3322f, 0.0f } },
                    { 1.339247f, { 2919.765f, 956.4427f, 512.3322f, 0.0f } },
                    { 1.422183f, { 2925.681f, 949.5660f, 512.3322f, 0.0f } },
                    { 1.403160f, { 2930.631f, 950.7448f, 512.3322f, 0.0f } },
                    { 1.473799f, { 2929.989f, 945.4636f, 512.3322f, 0.0f } },
                    { 1.589365f, { 2925.201f, 937.0295f, 512.3322f, 0.0f } },
                    { 1.472567f, { 2920.877f, 946.2239f, 512.3322f, 0.0f } },
                    { 1.554057f, { 2921.970f, 939.9601f, 512.3322f, 0.0f } },
                    { 1.598469f, { 2916.681f, 937.2430f, 512.3322f, 0.0f } },
                    { 1.631643f, { 2921.165f, 934.1927f, 512.3322f, 0.0f } },
                    { 1.545600f, { 2929.156f, 940.1042f, 512.3322f, 0.0f } },
                    { 1.685346f, { 2923.483f, 929.9393f, 512.3322f, 0.0f } },
                    { 1.642226f, { 2927.870f, 932.9045f, 512.3322f, 0.0f } },
                    { 1.675961f, { 2916.649f, 931.3802f, 512.3322f, 0.0f } },
                    { 1.748326f, { 2918.188f, 925.7153f, 512.3322f, 0.0f } },
                    { 1.799129f, { 2913.167f, 922.5486f, 512.3322f, 0.0f } },
                    { 1.836681f, { 2917.700f, 919.1007f, 512.3322f, 0.0f } },
                    { 1.791672f, { 2922.553f, 922.0139f, 512.3322f, 0.0f } },
                    { 1.839980f, { 2927.189f, 918.0886f, 512.3322f, 0.0f } },
                    { 1.738372f, { 2927.610f, 925.6979f, 512.3322f, 0.0f } },
                    { 1.799139f, { 2931.849f, 921.0174f, 512.3322f, 0.0f } },
                    { 1.493258f, { 2925.924f, 944.2049f, 512.3322f, 0.0f } },
                    { 1.505501f, { 2917.764f, 944.1320f, 512.3322f, 0.0f } },
                    { 1.237330f, { 2925.567f, 963.4827f, 512.3322f, 0.0f } }
                }
            };

            std::array<Position const, eBarrageData::MaxCorrupted> m_CorruptedBellowPos =
            {
                {
                    { 2918.5f, 935.8112f, 512.332f, 4.188790f },
                    { 2933.5f, 931.7920f, 512.332f, 4.712389f },
                    { 2948.5f, 935.8112f, 512.332f, 5.235988f }
                }
            };

            std::vector<std::vector<Position>> m_CorruptedBellowVisuals =
            {
                    {
                        { 2916.228f, 944.0665f, 512.332f, 0.0f },
                        { 2921.111f, 939.5660f, 512.332f, 0.0f },
                        { 2927.078f, 938.0621f, 512.332f, 0.0f },
                        { 2912.959f, 941.3798f, 512.332f, 0.0f },
                        { 2918.918f, 935.9735f, 512.332f, 0.0f },
                        { 2925.543f, 932.9215f, 512.332f, 0.0f },
                        { 2909.466f, 937.2690f, 512.332f, 0.0f },
                        { 2913.929f, 933.6513f, 512.332f, 0.0f },
                        { 2918.866f, 931.4355f, 512.332f, 0.0f },
                        { 2924.617f, 928.7494f, 512.332f, 0.0f },
                        { 2906.005f, 934.7672f, 512.332f, 0.0f },
                        { 2911.455f, 930.5133f, 512.332f, 0.0f },
                        { 2916.935f, 927.1055f, 512.332f, 0.0f },
                        { 2924.090f, 924.4685f, 512.332f, 0.0f },
                        { 2902.923f, 931.7244f, 512.332f, 0.0f },
                        { 2906.995f, 927.3593f, 512.332f, 0.0f },
                        { 2911.726f, 924.2538f, 512.332f, 0.0f },
                        { 2917.445f, 922.2554f, 512.332f, 0.0f },
                        { 2922.695f, 920.7256f, 512.332f, 0.0f },
                        { 2899.629f, 928.0626f, 512.332f, 0.0f },
                        { 2904.452f, 923.9947f, 512.332f, 0.0f },
                        { 2909.581f, 920.6927f, 512.332f, 0.0f },
                        { 2915.175f, 918.1143f, 512.332f, 0.0f },
                        { 2921.424f, 915.8536f, 512.332f, 0.0f }
                    },
                    {//2927 - 2939   938 914
                        { 2927.145f, 915.0031f, 512.332f, 0.0f },
                        { 2927.145f, 920.0031f, 512.332f, 0.0f },
                        { 2927.145f, 925.0031f, 512.332f, 0.0f },
                        { 2927.145f, 930.0031f, 512.332f, 0.0f },
                        { 2927.145f, 935.0031f, 512.332f, 0.0f },
                        { 2927.145f, 938.0031f, 512.332f, 0.0f },

                        { 2933.145f, 915.0031f, 512.332f, 0.0f },
                        { 2933.145f, 920.0031f, 512.332f, 0.0f },
                        { 2933.145f, 925.0031f, 512.332f, 0.0f },
                        { 2933.145f, 930.0031f, 512.332f, 0.0f },
                        { 2933.145f, 935.0031f, 512.332f, 0.0f },
                        { 2933.145f, 938.0031f, 512.332f, 0.0f },

                        { 2939.145f, 915.0031f, 512.332f, 0.0f },
                        { 2939.145f, 920.0031f, 512.332f, 0.0f },
                        { 2939.145f, 925.0031f, 512.332f, 0.0f },
                        { 2939.145f, 930.0031f, 512.332f, 0.0f },
                        { 2939.145f, 935.0031f, 512.332f, 0.0f },
                        { 2939.145f, 938.0031f, 512.332f, 0.0f }
                    },
                    {
                        { 2939.637f, 937.7626f, 512.332f, 0.0f },
                        { 2946.111f, 940.0244f, 512.332f, 0.0f },
                        { 2951.143f, 944.5811f, 512.332f, 0.0f },
                        { 2941.013f, 932.7467f, 512.332f, 0.0f },
                        { 2948.163f, 935.8702f, 512.332f, 0.0f },
                        { 2954.671f, 940.7018f, 512.332f, 0.0f },
                        { 2942.329f, 928.5428f, 512.332f, 0.0f },
                        { 2947.623f, 930.6499f, 512.332f, 0.0f },
                        { 2952.392f, 933.9941f, 512.332f, 0.0f },
                        { 2957.532f, 937.0857f, 512.332f, 0.0f },
                        { 2943.142f, 925.0607f, 512.332f, 0.0f },
                        { 2950.217f, 927.0221f, 512.332f, 0.0f },
                        { 2955.895f, 930.6758f, 512.332f, 0.0f },
                        { 2960.958f, 934.3885f, 512.332f, 0.0f },
                        { 2944.392f, 920.3409f, 512.332f, 0.0f },
                        { 2949.805f, 921.8865f, 512.332f, 0.0f },
                        { 2954.969f, 924.0530f, 512.332f, 0.0f },
                        { 2959.404f, 927.4186f, 512.332f, 0.0f },
                        { 2964.416f, 931.3131f, 512.332f, 0.0f },
                        { 2945.606f, 915.5869f, 512.332f, 0.0f },
                        { 2951.402f, 917.6945f, 512.332f, 0.0f },
                        { 2957.222f, 920.4847f, 512.332f, 0.0f },
                        { 2962.419f, 923.8337f, 512.332f, 0.0f },
                        { 2967.324f, 927.9767f, 512.332f, 0.0f }
                    }
            };

            uint8 m_CorruptedBellowIDx = 0;

            void Reset() override
            {
                me->DisableHealthRegen();
                me->SetFullHealth();

                _Reset();
                m_BypassCastEvents.Reset();
                m_TentacleDiedCount = 0;
                m_HealthSwitch = 0;
                m_LastTentaclePercing = 0;
                m_DisabledTentacle = 0;
                m_lastTentacleEntry = 0;
                m_EncounterCompleted = false;
                m_InSubmerge = false;
                m_LeftHand = false;
                UpdateShipState(STATE_REBUILDING);

                //me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
                //me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                me->SetReactState(REACT_AGGRESSIVE);

                if (m_IntroDone)
                {
                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);

                    AfterIntroSummons();
                }
                else
                    m_CosmeticEvent.ScheduleEvent(EVENT_CHECK_PLAYERS, 1 * TimeConstants::IN_MILLISECONDS);

                for (uint8 l_I = 0; l_I < 10; l_I++)
                {
                    /// Don't reset this one at reset after intro, tentacle is still there
                    if (m_IntroDone && g_SwirlingGo[l_I] == GO_TRAP_DOOR_009)
                        continue;

                    if (GameObject* l_GameObject = me->FindNearestGameObject(g_SwirlingGo[l_I], 200.0f))
                        l_GameObject->SetGoState(GO_STATE_READY);
                }

                std::list<Creature*> l_PoolList;
                GetCreatureListWithEntryInGrid(l_PoolList, me, NPC_SWIRLING_POOL_TRIG, 200.0f);

                for (auto const& l_Pool : l_PoolList)
                    l_Pool->DespawnOrUnsummon();

                me->SetFloatValue(EUnitFields::UNIT_FIELD_BOUNDING_RADIUS, 41.50491f);
                me->SetFloatValue(EUnitFields::UNIT_FIELD_COMBAT_REACH, 2.0f);

                if (IsChallengeMode())
                    me->AddAura(ChallengeSpells::ChallengersMight, me);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                m_FirstTaint = true;
                m_FirstSubmerge = true;

                m_CorruptedBellowIDx = 0;

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                Talk(SAY_AGGRO);
                _EnterCombat();
                events.ScheduleEvent(EVENT_TAINT_SEA, 0);
                m_BypassCastEvents.ScheduleEvent(EVENT_SUM_PIERCING_TENTACLE, 8000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_BRACKWATER_BARRAGE, 16000);
                else
                    events.ScheduleEvent(EVENT_TORRENT, 3000);

                std::list<Creature*> l_Tentacles;

                GetCreatureListWithEntryInGrid(l_Tentacles, me, NPC_DESTRUCTOR_TENTACLE, 200.0f);
                GetCreatureListWithEntryInGrid(l_Tentacles, me, NPC_GRASPING_TENTACLE_1, 200.0f);
                GetCreatureListWithEntryInGrid(l_Tentacles, me, NPC_GRASPING_TENTACLE_2, 200.0f);

                for (Creature* l_Iter : l_Tentacles)
                {
                    if (!l_Iter->isInCombat() && l_Iter->IsAIEnabled)
                        l_Iter->AI()->DoZoneInCombat(l_Iter, 150.0f);
                }
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();
                me->NearTeleportTo(2933.5f, 1055.94f, 512.463f, 4.79f); ///< HomePos
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);

                Talk(SAY_WIPE);
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void SummonTentacle(bool rightSide)
            {
                auto i = rightSide ? urand(3, 5) : urand(0, 2);
                me->AddDelayedEvent([_me = me, _i = i]()->void {
                    if (_me->isInCombat())
                        if (auto tent = _me->SummonCreature(graspingTentaclePoints[_i].first, graspingTentaclePoints[_i].second))
                            tent->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                }, 2000);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                auto IsRight = false;
                switch (p_Summon->GetEntry())
                {
                    case NPC_GRASPING_TENTACLE_1:
                    case NPC_GRASPING_TENTACLE_4:
                        IsRight = true; // no break here
                    case NPC_GRASPING_TENTACLE_2:
                    case NPC_GRASPING_TENTACLE_3:
                    case NPC_DESTRUCTOR_TENTACLE:
                    {
                        m_TentacleDiedCount++;
                        break;
                    }
                    default:
                        return;
                }

                if (p_Summon->GetEntry() == NPC_GRASPING_TENTACLE_1 || p_Summon->GetEntry() == NPC_GRASPING_TENTACLE_2)
                {
                    m_lastTentacleEntry = p_Summon->GetEntry();
                }

                if (m_TentacleDiedCount < 6)
                {
                    if (p_Summon->GetEntry() != NPC_DESTRUCTOR_TENTACLE)
                        SummonTentacle(IsRight);
                }

                switch (m_TentacleDiedCount)
                {
                    case 1:
                    {
                        Talk(SAY_FIRST_TENTACLE);
                        break;
                    }
                    case 3:
                    {
                        Talk(SAY_THIRD_TENTACLE);
                        break;
                    }
                    case 4:
                    {
                        Talk(SAY_FOURTH_TENTACLE);
                        break;
                    }
                    case 5:
                    {
                        Talk(SAY_FIFTH_TENTACLE);
                        break;
                    }
                    case 6:
                    {
                        std::unordered_set<uint32> ids = { NPC_GRASPING_TENTACLE_1, NPC_GRASPING_TENTACLE_2, NPC_GRASPING_TENTACLE_3, NPC_GRASPING_TENTACLE_4 };
                        for (auto& guid : summons)
                        {
                            if (auto sum = ObjectAccessor::GetCreature(*me, guid))
                            {
                                if (ids.find(sum->GetEntry()) == ids.end())
                                    continue;
                                sum->SetVisible(false);
                                sum->DespawnOrUnsummon(2000);
                            }
                        }

                        events.ScheduleEvent(EVENT_SUBMERGED_START, 1000);
                        break;
                    }
                    default:
                        break;
                }

                if (m_HealthSwitch < 2)
                {
                    me->SetHealth(me->GetHealth() - me->CountPctFromMaxHealth(3));
                    m_HealthSwitch++;
                }
                else
                {
                    me->SetHealth(me->GetHealth() - me->CountPctFromMaxHealth(4));
                    m_HealthSwitch = 0;
                }
            }

            void UpdateShipState(uint8 p_State)
            {
                if (instance == nullptr)
                    return;

                if (GameObject* l_Ship = instance->instance->GetGameObject(instance->GetData64(DATA_SHIP)))
                {
                    if (p_State == STATE_DESTROY)
                    {
                        l_Ship->SetDisplayId(31853);
                        l_Ship->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED, nullptr, true);
                    }
                    else if (p_State == STATE_REBUILDING)
                    {
                        l_Ship->SetDisplayId(31852);
                        l_Ship->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, nullptr, true);
                    }

                    Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = l_Players.begin(); itr != l_Players.end(); ++itr)
                    {
                        if (Player* l_Player = itr->getSource())
                            l_Ship->SendUpdateToPlayer(l_Player);
                    }
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(71) && !m_EncounterCompleted && instance)
                {
                    me->SendPlaySpellVisualKit(VISUAL_KIT_4, 4, 30 * TimeConstants::IN_MILLISECONDS);
                    m_EncounterCompleted = true;
                    events.Reset();
                    m_BypassCastEvents.Reset();
                    DoStopAttack();
                    Talk(SAY_DEATH);

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (instance)
                            instance->SetBossState(DATA_HELYA, DONE);

                        me->DespawnOrUnsummon();
                    });

                    CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);

                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);
                    instance->DoKilledMonsterKredit(eQuests::MawOfSoulsAHopeInHelheim, me->GetEntry());
                    instance->DoKilledMonsterKredit(eQuests::MawOfSoulsSpiritingAway, me->GetEntry());
                    instance->DoKilledMonsterKredit(eQuests::MawOfSoulsPiercingTheMists, me->GetEntry());

                    /// We have to trigger the lfg reward manually because Helya don't die
                    Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                    if (l_PlayerList.isEmpty())
                        return;

                    bool l_Achievement = true;

                    for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                    {
                        if (Player* l_Player = l_Itr->getSource())
                        {
                            uint32 l_DungeonID = l_Player->GetGroup() ? sLFGMgr->GetDungeon(l_Player->GetGroup()->GetGUID()) : 0;
                            sLFGMgr->RewardDungeonDoneFor(l_DungeonID, l_Player);

                            if (l_Achievement && !l_Player->HasAura(SPELL_POOR_UNFORTUNATE_SOUL))
                                l_Achievement = false;
                        }
                    }

                    if (l_Achievement && IsMythic())
                        instance->DoCompleteAchievement(eAchievements::PoorUnfortunateSouls);
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_BRACKWATER_BARRAGE)
                {
                    for (uint8 l_I = 0; l_I < eBarrageData::MaxBarrage; ++l_I)
                    {
                        Position l_Pos = m_BrackwaterVisuals[l_I].second;

                        /// Positions are defined for right hand, make them rotate if needed
                        if (m_LeftHand)
                        {
                            /// She's facing the opposite side she'll hit
                            float l_Angle   = Position::NormalizeOrientation(4.78662f - 4.5942f);
                            float l_Cosinus = std::cos(l_Angle);
                            float l_Sinus   = std::sin(l_Angle);

                            float l_X       = l_Pos.m_positionX - me->m_positionX;
                            float l_Y       = l_Pos.m_positionY - me->m_positionY;

                            float l_NewX    = l_X * l_Cosinus - l_Y * l_Sinus;
                            float l_NewY    = l_X * l_Sinus + l_Y * l_Cosinus;

                            l_Pos.m_positionX = l_NewX + me->m_positionX;
                            l_Pos.m_positionY = l_NewY + me->m_positionY;
                        }

                        me->SendPlaySpellVisual(eBarrageData::BarrageVisual, nullptr, m_BrackwaterVisuals[l_I].first, l_Pos, true);

                        AddTimedDelayedOperation(uint32(m_BrackwaterVisuals[l_I].first * float(TimeConstants::IN_MILLISECONDS)), [this, l_Pos]() -> void
                        {
                            me->CastSpell(l_Pos, SPELL_BRACKWATER_BARRAGE_DMG, true);
                        });
                    }

                    me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_SOULLESS_SCREAM:
                    {
                        me->SendPlaySpellVisualKit(VISUAL_KIT_3, 0);

                        if (instance)
                            instance->DoCastSpellOnPlayers(SPELL_SOULLESS_SCREAM_PACIFY);

                        break;
                    }
                    case SPELL_SUBMERGE:
                    {
                        DoCast(me, SPELL_SUBMERGED, false);
                        break;
                    }
                    case SPELL_CORRUPTED_BELLOW_AOE:
                    {
                        for (Position const& l_Pos : m_CorruptedBellowVisuals[m_CorruptedBellowIDx])
                            me->SendPlaySpellVisual(eBarrageData::CorruptedVisual, nullptr, 95.0f, l_Pos);

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case SPELL_SUBMERGED:
                    {
                        events.ScheduleEvent(EVENT_SUBMERGED_END, 1);
                        break;
                    }
                    case SPELL_CORRUPTED_BELLOW:
                    {
                        me->SetFacingTo(4.71f);
                        AddTimedDelayedOperation(1500, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
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

                m_CosmeticEvent.Update(p_Diff);

                if (m_CosmeticEvent.ExecuteEvent() == EVENT_CHECK_PLAYERS && !m_IntroDone)
                {
                    if (Player* l_Player = me->SelectNearestPlayerNotGM(135.0f))
                    {
                        m_IntroDone = true;

                        me->SetVisible(true);
                        me->SendPlaySpellVisualKit(VISUAL_KIT_1, 0);

                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                        {
                            me->SendPlaySpellVisualKit(VISUAL_KIT_2, 0);
                            Talk(SAY_INTRO);

                            AfterIntroSummons();

                            for (uint64 const& l_Itr : summons)
                            {
                                Unit* l_Summon = ObjectAccessor::GetUnit(*me, l_Itr);

                                if (l_Summon)
                                    l_Summon->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            }

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                DoCast(me, SPELL_SOULLESS_SCREAM, false);
                                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);

                                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                                {
                                    me->SendPlaySpellVisualKit(VISUAL_KIT_3, 0);

                                    AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                                    {
                                        for (uint64 const& l_Itr : summons)
                                        {
                                            auto l_Summon = ObjectAccessor::GetCreature(*me, l_Itr);

                                            if (l_Summon && !l_Summon->isTrigger())
                                                l_Summon->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                                        }

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15);
                                        DoCast(me, SPELL_INTERFERE_TARGETTING);
                                    });
                                });
                            });
                        });
                    }
                    else
                        m_CosmeticEvent.ScheduleEvent(EVENT_CHECK_PLAYERS, 1 * TimeConstants::IN_MILLISECONDS);
                }

                if (!UpdateVictim())
                    return;

                /// Prevent players to fall undermap in P2
                Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->GetPositionZ() < 500.0f)
                            l_Player->TeleportTo(1492, 2933.f, 923.f, 512.34f, 0.0f, TeleportToOptions::TELE_TO_NOT_LEAVE_COMBAT);
                    }
                }

                events.Update(p_Diff);
                m_BypassCastEvents.Update(p_Diff);

                switch (m_BypassCastEvents.ExecuteEvent())
                {
                    case EVENT_SUM_PIERCING_TENTACLE:
                    {
                        std::list<Creature*> l_Tentacles;
                        GetCreatureListWithEntryInGrid(l_Tentacles, me, NPC_SWIRLING_POOL_VEH, 200.0f);

                        /// Make sure we change of position every new piercing tentacle event
                        bool l_Found = false;
                        for (Creature* l_Iter : l_Tentacles)
                        {
                            if (l_Iter->GetGUID() == m_LastTentaclePercing)
                            {
                                l_Found = true;
                                break;
                            }
                        }

                        if (Creature* l_Remove = Creature::GetCreature(*me, m_LastTentaclePercing))
                            l_Tentacles.remove(l_Remove);

                        JadeCore::Containers::RandomResizeList(l_Tentacles, 1);

                        for (Creature* l_Iter : l_Tentacles)
                        {
                            m_LastTentaclePercing = l_Iter->GetGUID();

                            if (l_Iter->IsAIEnabled)
                                l_Iter->AI()->DoAction(true);
                        }

                        m_BypassCastEvents.ScheduleEvent(EVENT_SUM_PIERCING_TENTACLE, m_InSubmerge ? 2000 : 6000);
                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_TAINT_SEA:
                    {
                        DoCast(SPELL_TAINT_OF_THE_SEA);
                        events.ScheduleEvent(EVENT_TAINT_SEA, 12000);

                        if (!m_FirstTaint)
                            Talk(SAY_TAINT_SEA);

                        m_FirstTaint = false;
                        break;
                    }
                    case EVENT_TORRENT:
                    {
                        DoCast(m_TentacleDiedCount == 6 ? SPELL_TORRENT_P2 : SPELL_TORRENT_P1_NORMAL);
                        events.ScheduleEvent(EVENT_TORRENT, 15000);
                        break;
                    }
                    case EVENT_SUBMERGED_START:
                    {
                        if (instance)
                            instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_SUBMERGE, false);
                        Talk(SAY_PHASE2);
                        events.CancelEvent(EVENT_BRACKWATER_BARRAGE);
                        events.CancelEvent(EVENT_TORRENT);
                        events.CancelEvent(EVENT_WING_BUFFET);
                        events.CancelEvent(EVENT_CORRUPTED_BELLOW);
                        events.ScheduleEvent(EVENT_ACTION_SWIRLING_POOL, 1000);

                        m_InSubmerge = true;
                        break;
                    }
                    case EVENT_SUBMERGED_END:
                    {
                        if (instance)
                            instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);

                        me->RemoveAllAuras();

                        if (IsChallengeMode())
                            me->AddAura(ChallengeSpells::ChallengersMight, me);

                        me->NearTeleportTo(2933.5f, 961.79f, 512.38f, 4.71f);
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);

                        DoCast(me, SPELL_TRANSFORM, true);

                        if (m_FirstSubmerge)
                            DoCast(me, SPELL_SMASH, true);

                        m_FirstSubmerge = false;

                        UpdateShipState(STATE_DESTROY);

                        if (Creature* l_Pool = Creature::GetCreature(*me, m_DisabledTentacle))
                        {
                            m_DisabledTentacle = 0;

                            for (uint8 l_I = 0; l_I < 10; l_I++)
                            {
                                if (GameObject* l_GameObject = l_Pool->FindNearestGameObject(g_SwirlingGo[l_I], 1.0f))
                                    l_GameObject->SetGoState(GO_STATE_READY);
                            }

                            if (AreaTrigger* l_AT = l_Pool->GetAreaTrigger(SPELL_SWIRLING_POOL_AT))
                                l_AT->Remove();

                            if (Creature* l_AT = l_Pool->FindNearestCreature(NPC_PIERCING_TENTACLE, 2.0f))
                                l_AT->DespawnOrUnsummon();

                            if (Creature* l_AT = l_Pool->FindNearestCreature(NPC_SWIRLING_POOL_TRIG, 2.0f))
                                l_AT->DespawnOrUnsummon();

                            l_Pool->DespawnOrUnsummon();
                        }

                        if (IsHeroicOrMythic())
                            events.ScheduleEvent(EVENT_TORRENT, 3000);

                        events.ScheduleEvent(EVENT_WING_BUFFET, 4000);
                        events.ScheduleEvent(EVENT_SUBMERGED_START, 60 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_CORRUPTED_BELLOW, 15 * TimeConstants::IN_MILLISECONDS);

                        std::list<Creature*> l_PoolList;
                        GetCreatureListWithEntryInGrid(l_PoolList, me, NPC_SWIRLING_POOL_TRIG, 150.0f);
                        for (auto const& l_Pool : l_PoolList)
                            l_Pool->RemoveAurasDueToSpell(SPELL_TURBULENT_WATERS);

                        me->SetFloatValue(EUnitFields::UNIT_FIELD_BOUNDING_RADIUS, 33.0f);
                        me->SetFloatValue(EUnitFields::UNIT_FIELD_COMBAT_REACH, 35.2f);
                        m_InSubmerge = false;
                        break;
                    }
                    case EVENT_WING_BUFFET:
                    {
                        if (Unit* l_Target = me->FindNearestPlayer(50))
                        {
                            if (!me->IsWithinMeleeRange(l_Target))
                            {
                                Talk(SAY_WARN_WING_BUFFET);
                                me->CastSpell(me, SPELL_WING_BUFFET);
                            }
                        }
                        events.ScheduleEvent(EVENT_WING_BUFFET, 2000);
                        break;
                    }
                    case EVENT_ACTION_SWIRLING_POOL:
                    {
                        std::list<Creature*> l_PoolList;
                        GetCreatureListWithEntryInGrid(l_PoolList, me, NPC_SWIRLING_POOL_TRIG, 150.0f);
                        JadeCore::Containers::RandomResizeList(l_PoolList, 10);
                        for (auto const& l_Pool : l_PoolList)
                            l_Pool->CastSpell(l_Pool, SPELL_TURBULENT_WATERS, true);
                        break;
                    }
                    case EVENT_BRACKWATER_BARRAGE:
                    {
                        Talk(SAY_BARRAGE_WARN);
                        m_LeftHand = urand(0, 1) != 0;
                        me->SendPlaySpellVisual(m_LeftHand ? eBarrageData::LeftHandVisual : eBarrageData::RightHandVisual, nullptr, IsMythic() ? 4.0f : 9.0f, { 2934.499f, 1055.943f, 510.5183f, 0.0f }, true);
                        me->SetFacingTo(m_LeftHand ? 4.5942f : 4.78662f);
                        me->AddUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
                        DoCast(SPELL_BRACKWATER_BARRAGE);
                        events.ScheduleEvent(EVENT_BRACKWATER_BARRAGE, 22000);
                        break;
                    }
                    case EVENT_CORRUPTED_BELLOW:
                    {
                        Talk(SAY_CORRUPTED_BELLOW_WARN);
                        Talk(SAY_CORRUPTED_BELLOW);

                        m_CorruptedBellowIDx = urand(0, eBarrageData::MaxCorrupted - 1);

                        //me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        //me->AddUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
                        auto angle = m_CorruptedBellowPos[m_CorruptedBellowIDx].m_orientation;
                        me->StopAttack();
                        me->SetFacingTo(angle);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            DoCast(me, SPELL_CORRUPTED_BELLOW);
                        });

                        events.ScheduleEvent(EVENT_CORRUPTED_BELLOW, 22 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void AfterIntroSummons()
            {
                me->AddDelayedEvent([_me=me]() -> void {
                    if (GameObject* l_Trap = _me->FindNearestGameObject(GO_TRAP_DOOR_009, 100.0f))
                        l_Trap->SetGoState(GOState::GO_STATE_ACTIVE);

                    if (Creature* l_Tentacle = _me->SummonCreature(NPC_DESTRUCTOR_TENTACLE, g_TentaclePos[0]))
                        l_Tentacle->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);

                    if (Creature* l_Tentacle = _me->SummonCreature(NPC_GRASPING_TENTACLE_1, g_TentaclePos[1]))
                        l_Tentacle->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);

                    if (Creature* l_Tentacle = _me->SummonCreature(NPC_GRASPING_TENTACLE_2, g_TentaclePos[2]))
                        l_Tentacle->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                }, 100);

                for (uint8 l_I = 8; l_I < 16; l_I++)
                {
                    Creature* l_Pool = me->SummonCreature(NPC_SWIRLING_POOL_VEH, g_TentaclePos[l_I]);

                    if (l_I == 12 && l_Pool)
                        m_DisabledTentacle = l_Pool->GetGUID();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_helyaAI(p_Creature);
        }
};

/// Grasping Tentacle - 98363, 100354, 100360, 100362
/// Destructor Tentacle - 99801
/// Piercing Tentacle - 100188
class npc_helya_tentacle : public CreatureScript
{
    public:
        npc_helya_tentacle() : CreatureScript("npc_helya_tentacle") { }

        struct npc_helya_tentacleAI : public ScriptedAI
        {
            npc_helya_tentacleAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                SetCombatMovement(false);
                me->SetReactState(REACT_DEFENSIVE);
            }

            InstanceScript* m_Instance;
            bool m_Find;

            void Reset() override { }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                CHECK_REQUIRED_BOSSES_AND_TELEPORT(DATA_HELYA, (m_Instance && m_Instance->GetBossState(DATA_YMIRON) == DONE) ?  g_PlayerTeleportPos : g_StartPos);

                if (m_Instance && m_Instance->GetBossState(DATA_HELYA) != IN_PROGRESS)
                {
                    if (Creature* l_Helya = m_Instance->instance->GetCreature(m_Instance->GetData64(DATA_HELYA)))
                        l_Helya->SetInCombatWithZone();
                }

                switch (me->GetEntry())
                {
                    case NPC_DESTRUCTOR_TENTACLE:
                    case NPC_DESTRUCTOR_TENTACLE_2:
                    {
                        if (m_Instance)
                            m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                        DoCast(me, SPELL_SMASH_ALLOW, true);

                        Talk(0);
                        events.ScheduleEvent(EVENT_5, 2000);
                        events.ScheduleEvent(EVENT_6, 2000);
                        break;
                    }
                    case NPC_GRASPING_TENTACLE_1:
                    case NPC_GRASPING_TENTACLE_4:
                    case NPC_GRASPING_TENTACLE_3:
                    case NPC_GRASPING_TENTACLE_2:
                    {
                        if (m_Instance)
                            m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 3);

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->ToTempSummon())
                {
                    if (Unit* l_Summoner = me->ToTempSummon()->GetSummoner())
                        l_Summoner->Kill(l_Summoner);
                }
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_GRASPING_TENTACLE_3:
                    case NPC_GRASPING_TENTACLE_2:
                    {
                        events.ScheduleEvent(EVENT_1, 1000);
                        break;
                    }
                    case NPC_GRASPING_TENTACLE_1:
                    case NPC_GRASPING_TENTACLE_4:
                    {
                        events.ScheduleEvent(EVENT_2, 1000);
                        break;
                    }
                    case NPC_PIERCING_TENTACLE:
                    {
                        events.ScheduleEvent(EVENT_3, 1000);
                        events.ScheduleEvent(EVENT_7, 3000);
                        return;
                    }
                    default:
                        break;
                }

                if (m_Instance && m_Instance->GetBossState(DATA_HELYA) == IN_PROGRESS)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat(me, 100.0f);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                me->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);

                if (!UpdateVictim() && me->isInCombat())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_GRASPING_STUN_LEFT, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_GRASPING_STUN_RIGHT, true);
                        break;
                    }
                    case EVENT_3:
                    {
                        me->CastSpell(me, SPELL_PIERCING_TENTACLE, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                        events.ScheduleEvent(EVENT_4, 4000);
                        break;
                    }
                    case EVENT_4:
                    {
                        me->SetDisplayId(11686);
                        break;
                    }
                    case EVENT_5:
                    {
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, SPELL_RAPIDE_RUPTURE, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));

                        events.ScheduleEvent(EVENT_5, 6000);
                        break;
                    }
                    case EVENT_6:
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            if (!me->IsWithinMeleeRange(l_Victim))
                                me->CastSpell(me, SPELL_SMASH_TRIGGERED, TriggerCastFlags(TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE));
                        }

                        events.ScheduleEvent(EVENT_6, 2000);
                        break;
                    }
                    case EVENT_7:
                    {
                        me->CastSpell(me, SPELL_PIERCING_TENTACLE_DMG, true);
                        break;
                    }
                    default:
                        break;
                }

                if (me->GetEntry() == NPC_DESTRUCTOR_TENTACLE)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_helya_tentacleAI(p_Creature);
        }
};

/// Swirling Pool - 97099
class npc_helya_tentacle_veh : public CreatureScript
{
    public:
        npc_helya_tentacle_veh() : CreatureScript("npc_helya_tentacle_veh") { }

        struct npc_helya_tentacle_vehAI : public ScriptedAI
        {
            npc_helya_tentacle_vehAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Summons(me)
            {
                me->SetReactState(REACT_PASSIVE);
                m_CreateAT = false;
            }

            SummonList m_Summons;
            bool m_CreateAT;

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (!m_CreateAT)
                {
                    m_CreateAT = true;
                    DoCast(me, SPELL_SWIRLING_POOL_AT, true);

                    for (uint8 l_I = 0; l_I < 10; l_I++)
                    {
                        if (GameObject* l_GameObject = me->FindNearestGameObject(g_SwirlingGo[l_I], 1.0f))
                            l_GameObject->SetGoState(GO_STATE_ACTIVE);
                    }
                }

                float l_X = me->GetPositionX();
                float l_Y = me->GetPositionY();
                float l_Z = me->GetPositionZ();

                me->SummonCreature(NPC_PIERCING_TENTACLE, l_X, l_Y, l_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 6 * IN_MILLISECONDS);
                me->SummonCreature(NPC_SWIRLING_POOL_TRIG, l_X, l_Y, l_Z);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                m_Summons.Summon(p_Summon);
                p_Summon->SetReactState(REACT_PASSIVE);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_helya_tentacle_vehAI(p_Creature);
        }
};

/// Skjal <Captain of the Damned> - 99307
class npc_skyal : public CreatureScript
{
    public:
        npc_skyal() : CreatureScript("npc_skyal") { }

        struct npc_skyalAI : public ScriptedAI
        {
            npc_skyalAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
               m_IntroDone = false;

               m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || !m_Instance)
                    return;

                if (!m_IntroDone && m_Instance->GetBossState(DATA_HARBARON) == EncounterState::DONE && me->IsWithinDistInMap(p_Who, 148.0f))
                {
                    m_IntroDone = true;
                    Talk(0);

                }
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                events.ScheduleEvent(EVENT_SUMMON_1, 1000);
                events.ScheduleEvent(EVENT_SUMMON_2, 10000);
                events.ScheduleEvent(EVENT_GIVE_NO, 7000);
                events.ScheduleEvent(EVENT_DEBILITATING, 13000);
                events.ScheduleEvent(EVENT_BIND, 17000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(2);

                if (m_Instance)
                    m_Instance->SetData(DATA_SKJAL_WALL, DONE);
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
                    case EVENT_SUMMON_1:
                    {
                        for (uint8 l_I = 0; l_I < 2; l_I++)
                        {
                            if (Creature* l_Summon = me->SummonCreature(NPC_SKELETAL_WARRIOR, g_AddPos[l_I], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 4000))
                            {
                                if (l_Summon->IsAIEnabled)
                                    l_Summon->AI()->DoZoneInCombat(l_Summon, 90.0f);
                            }
                        }

                        Talk(1);
                        events.ScheduleEvent(EVENT_SUMMON_1, 18000);
                        break;
                    }
                    case EVENT_SUMMON_2:
                    {
                        for (uint8 l_I = 0; l_I < 2; l_I++)
                        {
                            if (Creature* l_Summon = me->SummonCreature(NPC_SKELETAL_SORCERER, g_AddPos[l_I], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 4000))
                            {
                                if (l_Summon->IsAIEnabled)
                                    l_Summon->AI()->DoZoneInCombat(l_Summon, 90.0f);
                            }
                        }

                        Talk(1);
                        events.ScheduleEvent(EVENT_SUMMON_2, 18000);
                        break;
                    }
                    case EVENT_GIVE_NO:
                    {
                        Unit* target = nullptr;
                        if (auto targ = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.f, true))
                            target = targ;

                        if (!target)
                            if (auto targ = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.f, true))
                                target = targ;

                        if (target)
                        {
                            me->StopAttack();
                            me->SetFacingToObject(target);
                            
                            me->AddDelayedEvent([_me = me, tarGuid = target->GetGUID()]() -> void {
                                if (auto target = ObjectAccessor::GetPlayer(*_me, tarGuid))
                                {
                                    auto distanceToTarget = _me->GetExactDist(target);
                                    if (distanceToTarget > 3.0f)
                                    {
                                        auto delay = 0;
                                        auto distance = 0.0f;
                                        auto angle = _me->GetRelativeAngle(target);
                                        auto chargeCount = (int)floor(distanceToTarget / 3.0f);
                                        auto delayCoef = 2500 / chargeCount;

                                        for (auto i = 0; i < chargeCount; ++i)
                                        {
                                            Position point;
                                            _me->GetNearPoint2D(point.m_positionX, point.m_positionY, distance, angle);
                                            _me->MovePosition(point, distance, angle);
                                            _me->UpdateAllowedPositionZ(point.GetPositionX(), point.GetPositionY(), point.m_positionZ);
                                            distance += 3.0f;
                                            _me->AddDelayedEvent([__me=_me, point]()->void {
                                                __me->SummonCreature(100733, point, TEMPSUMMON_TIMED_DESPAWN, 5000); }, delay);
                                            delay += delayCoef;
                                        }

                                    }
                                    _me->CastSpell(target, SPELL_GIVE_NO);
                                }
                            }, 100);
                            me->AddDelayedEvent([_me = me]() -> void { _me->SetReactState(REACT_AGGRESSIVE); }, 4000);
                        }
                        events.ScheduleEvent(EVENT_GIVE_NO, 13000);
                        break;
                    }
                    case EVENT_DEBILITATING:
                    {
                        DoCast(SPELL_DEBILITATING);
                        events.ScheduleEvent(EVENT_DEBILITATING, 13000);
                        break;
                    }
                    case EVENT_BIND:
                    {
                        DoCast(SPELL_BIND);
                        events.ScheduleEvent(EVENT_BIND, 13000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool m_IntroDone;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skyalAI(p_Creature);
        }
};

//100733
struct npc_skjal_chargeai : ScriptedAI
{
    explicit npc_skjal_chargeai(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
        DoCast(me, 198270, true);

        me->AddDelayedEvent([_me = me]()-> void
            {
                _me->CastSpell(_me, 198336, true);
                _me->AddDelayedEvent([_me = _me]()-> void { _me->SetDisplayId(27823); }, 1000);
            }, 300);
    }
};

/// Swirling Water - 195309
class spell_helya_swirling_water : public SpellScriptLoader
{
    public:
        spell_helya_swirling_water() : SpellScriptLoader("spell_helya_swirling_water") { }

        class spell_helya_swirling_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_helya_swirling_water_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                {
                    if (Creature* l_Helya = Creature::GetCreature(*l_Caster, l_Instance->GetData64(DATA_HELYA)))
                    {
                        Position l_Pos = *l_Caster;

                        l_Caster->MovePosition(l_Pos, frand(7.0f, 10.0f), l_Helya->GetAngle(l_Caster));
                        l_Caster->GetMotionMaster()->MoveJump(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ() + 5.0f, 30.0f, 10.0f);
                        l_Caster->CastSpell(l_Caster, SPELL_SWIRLING_POOL_JUMP, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_helya_swirling_water_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_helya_swirling_water_SpellScript();
        }
};

/// Whirlpool of Souls - 199589
class spell_whirpool_of_souls : public SpellScriptLoader
{
    public:
        spell_whirpool_of_souls() : SpellScriptLoader("spell_whirpool_of_souls") { }

        class spell_whirpool_of_souls_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_whirpool_of_souls_AuraScript);

            void OnPereodic(AuraEffect const* /*p_AurEff*/)
            {
                PreventDefaultAction();

                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->CastSpell(l_Caster, SPELL_TORRENT_OF_SOULS);
                    l_Caster->CastSpell(l_Caster->SelectNearbyTarget(), SPELL_TORRENT_OF_SOULS);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_whirpool_of_souls_AuraScript::OnPereodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_whirpool_of_souls_AuraScript();
        }
};

/// Brackwater Blast - 200208
class spell_brackwater : public SpellScriptLoader
{
    public:
        spell_brackwater() : SpellScriptLoader("spell_brackwater") { }

        class spell_brackwater_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brackwater_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (Creature* l_Creature = l_Target->FindNearestCreature(NPC_SEACURSED_SWIFTBLADE, 40.0f, true))
                    l_Creature->CastSpell(l_Target, SPELL_BRACKWATER_BLAST, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_brackwater_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brackwater_AuraScript();
        }
};

/// Turbulent Waters - 197752
class spell_helya_turbulent_waters : public SpellScriptLoader
{
    public:
        spell_helya_turbulent_waters() : SpellScriptLoader("spell_helya_turbulent_waters") { }

        class spell_helya_turbulent_waters_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_helya_turbulent_waters_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                if (!GetCaster())
                    return;

                Position l_Pos;
                GetCaster()->GetRandomNearPosition(l_Pos, 30.0f);
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                l_Dest->Relocate(l_Pos);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_helya_turbulent_waters_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_helya_turbulent_waters_SpellScript();
        }
};

/// Taint of the Sea - 197262
class spell_helya_taint_of_the_sea : public SpellScriptLoader
{
    public:
        spell_helya_taint_of_the_sea() : SpellScriptLoader("spell_helya_taint_of_the_sea") { }

        class spell_helya_taint_of_the_sea_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_helya_taint_of_the_sea_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (GetCaster()->GetMap()->IsHeroicOrMythic() && GetCaster()->isInCombat())
                    GetCaster()->CastSpell(GetTarget(), SPELL_TAINTED_ESSENCE, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_helya_taint_of_the_sea_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_helya_taint_of_the_sea_AuraScript();
        }
};

/// Portal to Stormheim - 267034
class go_helya_portal_to_stormheim : public GameObjectScript
{
    public:
        go_helya_portal_to_stormheim() : GameObjectScript("go_helya_portal_to_stormheim") { }

        struct go_helya_portal_to_stormheimAI : public GameObjectAI
        {
            go_helya_portal_to_stormheimAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            bool GossipHello(Player* p_Player) override
            {
                p_Player->NearTeleportTo({ 7184.29f, 7318.97f, 23.27f, 6.053849f });
                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_helya_portal_to_stormheimAI(p_GameObject);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_helya()
{
    /// Boss
    new boss_helya();

    /// Creatures
    new npc_helya_tentacle();
    new npc_helya_tentacle_veh();
    new npc_skyal();
    RegisterCreatureAI(npc_skjal_chargeai);
    /// Spells
    new spell_helya_swirling_water();
    new spell_whirpool_of_souls();
    new spell_brackwater();
    new spell_helya_turbulent_waters();
    new spell_helya_taint_of_the_sea();

    /// GameObject
    new go_helya_portal_to_stormheim();
}
#endif
