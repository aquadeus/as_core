////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "tomb_of_sargeras.h"

const Position k_MaidenSpawnPosition = { 6595.0f, -766.0f, 1663.6f, 3.72f };
const Position k_PortalPosition = { 6752.71f, -796.30f, 1527.46f };
std::array<Position, 5> k_ContainementPylons =
{{
    { 6572.559f, -735.3646f, 1662.983f, 5.235988f },
    { 6547.430f, -760.6458f, 1662.983f, 5.759586f },
    { 6547.384f, -829.5295f, 1662.983f, 0.523599f },
    { 6538.186f, -795.1736f, 1662.983f, 0.000000f },
    { 6572.728f, -854.6563f, 1662.983f, 1.047198f }
}};

std::array<Position, 5> k_ContainementPylonStalkers =
{{
    { 6595.392f, -801.7465f, 1662.754f, 3.662716f },
    { 6593.492f, -795.3698f, 1662.754f, 3.134099f },
    { 6600.672f, -806.7847f, 1662.754f, 4.186530f },
    { 6600.382f, -783.0712f, 1662.752f, 2.093218f },
    { 6595.412f, -788.4479f, 1662.754f, 2.614965f }
}};

enum eMoveInform
{
    HitGround = 10
};

/// Last Update 7.3.5 - Build 26365
/// Fallen Avatar - 116939
class boss_fallen_avatar : public CreatureScript
{
    public:
        boss_fallen_avatar() : CreatureScript("boss_fallen_avatar") { }

        struct boss_fallen_avatarAI : BossAI
        {
            boss_fallen_avatarAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataFallenAvatar)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidLFR)
                    {
                        Creature* l_Maiden = me->SummonCreature(eCreatures::NpcMaidenOfValor, k_MaidenSpawnPosition);
                        if (l_Maiden)
                            m_MaidenGUID = l_Maiden->GetGUID();
                        else
                            m_MaidenGUID = 0;
                    }

                    for (Position const& l_Position : k_ContainementPylons)
                    {
                        if (Creature* l_Pylon = me->SummonCreature(eCreatures::NpcContainementPylon, l_Position))
                            m_Pylons.push_back(l_Pylon->GetGUID());
                    }

                    for (Position const& l_Position : k_ContainementPylonStalkers)
                        me->SummonCreature(eCreatures::NpcPylonBeamStalker, l_Position);

                    m_AddsSpawnScheduled = false;

                    if (!me->isAlive())
                        return;

                    GameObject* l_Door = me->FindNearestGameObject(eGameObjects::GoDoorPreFallenAvatar, 200.0f);
                    if (!l_Door)
                        return;

                    l_Door->SetGoState(GOState::GO_STATE_READY);
                });
                m_AddsSpawnScheduled = true;

                m_IntroDone = false;

                me->SetAIAnimKitId(12757);
                me->CastSpell(me, eSpells::Chained, true);

                ApplyAllImmunities(true);
            }

            std::list<uint64> m_Pylons;
            uint64 m_MaidenGUID;
            int32 m_RippleTimer;
            uint8 m_Phase;
            uint32 m_EnergyTick;
            int32 m_CheckLavaTimer;
            uint32 m_LaserCount;
            uint32 m_DesolateCounter;
            uint32 m_DarkMarkCounter;

            bool m_IntroDone;
            bool m_HasFallen;
            bool m_AddsSpawnScheduled;

            std::array<std::pair<uint32, Position>, 25> m_LowerPlatforms =
            {{
                { eGameObjects::GoLowerPlatform1,  { } },
                { eGameObjects::GoLowerPlatform2,  { } },
                { eGameObjects::GoLowerPlatform3,  { } },
                { eGameObjects::GoLowerPlatform4,  { } },
                { eGameObjects::GoLowerPlatform5,  { } },
                { eGameObjects::GoLowerPlatform6,  { } },
                { eGameObjects::GoLowerPlatform7,  { } },
                { eGameObjects::GoLowerPlatform8,  { } },
                { eGameObjects::GoLowerPlatform9,  { } },
                { eGameObjects::GoLowerPlatform10, { } },
                { eGameObjects::GoLowerPlatform11, { } },
                { eGameObjects::GoLowerPlatform12, { } },
                { eGameObjects::GoLowerPlatform13, { } },
                { eGameObjects::GoLowerPlatform14, { } },
                { eGameObjects::GoLowerPlatform15, { } },
                { eGameObjects::GoLowerPlatform16, { } },
                { eGameObjects::GoLowerPlatform17, { } },
                { eGameObjects::GoLowerPlatform18, { } },
                { eGameObjects::GoLowerPlatform19, { } },
                { eGameObjects::GoLowerPlatform20, { } },
                { eGameObjects::GoLowerPlatform21, { } },
                { eGameObjects::GoLowerPlatform22, { } },
                { eGameObjects::GoLowerPlatform23, { } },
                { eGameObjects::GoLowerPlatform24, { } },
                { eGameObjects::GoLowerPlatform25, { } }
            }};

            enum eSpells
            {
                /// Intro
                EquipWeapon                 = 235582,
                Chained                     = 239847,
                EncounterFrame              = 252157,

                /// Phase 1
                Desolate                    = 236494,
                RuptureRealities            = 239132,
                RippleOfDarkness            = 236528,
                UnboundChaos                = 234057,
                ShadowyBladeCountSpell      = 236604,
                CorruptedMatrix             = 233556,
                SparkOfTheDarkTitan         = 234873,
                BindingBeam                 = 233878,

                /// Transition
                Consume                     = 240594,
                Annihilation                = 235597,

                /// Phase 2
                RuptureRealitiesPhase2      = 235572,
                Sear                        = 234179,
                DarkMark                    = 239825,

                /// Heroic
                TouchOfSargeras             = 239057,
                TouchOfSargerasCountSpell   = 239058,
                BlackWinds                  = 239417,
                FelInfusion                 = 236682,
                FelInfusionBuff             = 236684,
                MoltenFel                   = 240249,

                /// Mythic
                TaintedMatrix               = 240623,
                TaintedMatrixMaiden         = 240758,
                TaintedMatrixPlayer         = 240631,

                /// Achievement
                BingoRow                    = 240535,
                BingoColumn                 = 240536
            };

            enum eEvents
            {
                EventDesolate = 1,
                EventRuptureRealities,
                EventUnboundChaos,
                EventShadowyBlades,
                EventActivatePylon,
                EventActivateTaintedPylon,
                EventEnergize,
                EventDarkMark,
                EventTouchOfSargeras,
                EventRainOfTheDestroyer
            };

            enum eTalks
            {
                TalkAggro,
                TalkShadowyBlades,
                TalkTouchOfSargeras,
                TalkRuptureRealities,
                TalkUnboundChaos,
                TalkTransition,
                TalkKillingPlayer,
                TalkWipe,
                TalkDeath
            };

            std::map<Difficulty, std::map<uint32, std::vector<uint32>>> m_TimerData =
            {
                {
                    Difficulty::DifficultyRaidLFR,
                    {
                        { eEvents::EventUnboundChaos, { 7000, 42000, 35000, 41000, 36000, 35000, 43000 } },
                        { eEvents::EventRuptureRealities, { 30000, 60000, 60000, 60000, 60000 } },
                        { eEvents::EventShadowyBlades, { 37000, 60800, 60000, 62000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidNormal,
                    {
                        { eEvents::EventUnboundChaos, { 7000, 42000, 35000, 41000, 36000, 35000, 43000 } },
                        { eEvents::EventRuptureRealities, { 30000, 60000, 60000, 60000, 60000 } },
                        { eEvents::EventShadowyBlades, { 37000, 60800, 60000, 62000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidHeroic,
                    {
                        { eEvents::EventUnboundChaos, { 7000, 42000, 35000, 41000, 36000, 35000, 43000 } },
                        { eEvents::EventTouchOfSargeras, { 15000, 42500, 55000, 43000, 42500, 42500 } },
                        { eEvents::EventRuptureRealities, { 30000, 60000, 60000, 60000, 60000 } },
                        { eEvents::EventShadowyBlades, { 37000, 60800, 60000, 62000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidMythic,
                    {
                        { eEvents::EventUnboundChaos, { 7000, 43800, 41400, 35700, 36100, 35300, 35100 } },
                        { eEvents::EventTouchOfSargeras, { 15500, 63500, 63000, 63000 } },
                        { eEvents::EventRuptureRealities, { 34500, 70500, 63000, 61000, 65000 } },
                        { eEvents::EventShadowyBlades, { 34500, 64500, 63000, 63000 } }
                    }
                }
            };

            std::map<Difficulty, std::map<uint32, std::queue<uint32>>> m_Timers;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return !m_HasFallen;
            }

            bool CanBeHitInTheBack() override
            {
                return m_HasFallen;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetAIAnimKitId(12758);
                        });

                        AddTimedDelayedOperation(14500, [this]() -> void
                        {
                            GameObject* l_LeftChain = me->FindNearestGameObject(eGameObjects::GoLeftChain, 150.0f);
                            if (!l_LeftChain)
                                return;

                            l_LeftChain->SetGoState(GOState::GO_STATE_ACTIVE);
                        });

                        AddTimedDelayedOperation(16500, [this]() -> void
                        {
                            GameObject* l_RightChain = me->FindNearestGameObject(eGameObjects::GoRightChain, 150.0f);
                            if (!l_RightChain)
                                return;

                            l_RightChain->SetGoState(GOState::GO_STATE_ACTIVE);
                        });

                        AddTimedDelayedOperation(20 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->RemoveAura(eSpells::Chained);
                            me->CastSpell(me, eSpells::EquipWeapon, true);
                        });

                        AddTimedDelayedOperation(22 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetAIAnimKitId(0);
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            Creature* l_Maiden = me->FindNearestCreature(eCreatures::NpcMaidenOfValor, 50.0f);
                            if (!l_Maiden)
                                return;

                            l_Maiden->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        });

                        break;
                    }
                    case ACTION_2:
                    {
                        if (m_Phase == 1)
                            ChangePhase();
                        break;
                    }
                    case ACTION_3:
                    {
                        if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidLFR)
                        {
                            if (!me->HasAura(eSpells::SparkOfTheDarkTitan))
                            {
                                me->CastSpell(me, eSpells::BindingBeam, true);
                                me->CastSpell(me, eSpells::SparkOfTheDarkTitan, true);
                            }

                            m_LaserCount++;
                        }
                        break;
                    }
                    case ACTION_4:
                    {
                        m_LaserCount--;

                        if (!m_LaserCount)
                        {
                            me->RemoveAura(eSpells::SparkOfTheDarkTitan);
                            me->RemoveAura(eSpells::BindingBeam);
                        }
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoveInform::HitGround:
                    {
                        Position l_Dest = { 6606.24f, -792.5764f, 1509.918f, 10.0f };
                        me->NearTeleportTo(l_Dest);

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->getThreatManager().clearReferences();
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            me->CastSpell(me, eSpells::Sear, true);

                            if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                                || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                                me->CastSpell(me, eSpells::BlackWinds, true);

                            m_HasFallen = true;

                            Unit* l_Target = SelectMainTank();
                            if (!l_Target)
                            {
                                l_Target = me->FindNearestPlayer(150.0f);
                                if (!l_Target)
                                    return;
                            }

                            AttackStart(l_Target);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::RuptureRealitiesPhase2:
                    {
                        if (m_Phase == 1)
                            break;

                        for (auto const& l_Pair : m_LowerPlatforms)
                        {
                            if (l_Pair.second.GetExactDist(me) <= 50.0f)
                            {
                                GameObject* l_Platform = me->FindNearestGameObject(l_Pair.first, 200.0f);
                                if (!l_Platform)
                                    continue;

                                l_Platform->SetDestructibleState(GameObjectDestructibleState::GO_DESTRUCTIBLE_DESTROYED);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(eTalks::TalkDeath);

                _JustDied();

                me->RemoveAura(eSpells::EncounterFrame);

                Creature* l_KilJeaden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 250.0f, true);
                if (l_KilJeaden && l_KilJeaden->IsAIEnabled)
                    l_KilJeaden->AI()->DoAction(ACTION_4);

                if (GameObject* l_FelStorm = me->FindNearestGameObject(eGameObjects::GoFelStorm, 250.0f))
                    l_FelStorm->SetGoState(GOState::GO_STATE_ACTIVE);
                if (GameObject* l_FelStormCollision1 = me->FindNearestGameObject(eGameObjects::GoFelStormCollision1, 250.0f))
                    l_FelStormCollision1->SetGoState(GOState::GO_STATE_ACTIVE);
                if (GameObject* l_FelStormCollision2= me->FindNearestGameObject(eGameObjects::GoFelStormCollision2, 250.0f))
                    l_FelStormCollision2->SetGoState(GOState::GO_STATE_ACTIVE);
                if (GameObject* l_Door = me->FindNearestGameObject(eGameObjects::GoDoorPreFallenAvatar, 200.0f))
                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->RemoveAura(eSpells::MoltenFel);
                }

                for (uint32 l_I = 0; l_I < 5; ++l_I)
                {
                    bool l_Row   = true;
                    bool l_Column = true;

                    for (int32 l_J = 0; l_J < 5; ++l_J)
                    {
                        GameObject* l_Platform = me->FindNearestGameObject(m_LowerPlatforms[l_I + l_J * 5].first, 200.0f);
                        if (!l_Platform || (l_Platform->GetDestructibleState() != GameObjectDestructibleState::GO_DESTRUCTIBLE_INTACT))
                            l_Row = false;

                        l_Platform = me->FindNearestGameObject(m_LowerPlatforms[l_I * 5 + l_J].first, 200.0f);
                        if (!l_Platform || (l_Platform->GetDestructibleState() != GameObjectDestructibleState::GO_DESTRUCTIBLE_INTACT))
                            l_Column = false;
                    }

                    if (l_Row)
                        me->CastSpell(me, eSpells::BingoRow, true);
                    if (l_Column)
                        me->CastSpell(me, eSpells::BingoColumn, true);
                }
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (!p_Victim->IsPlayer())
                    return;

                if (me->GetInstanceScript() && me->GetInstanceScript()->IsWipe())
                {
                    Talk(eTalks::TalkWipe);
                    return;
                }

                Talk(eTalks::TalkKillingPlayer);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /* = 0 */) override
            {
                Creature* l_Target = sObjectAccessor->GetCreature(*me, p_Guid);
                if (!l_Target)
                    return;

                std::pair<uint32, float> l_NearestPlatform = { 0, -10.0f };
                for (auto const& l_PlatformData : m_LowerPlatforms)
                {
                    if (l_NearestPlatform.second < 0.0f)
                        l_NearestPlatform = { l_PlatformData.first, l_Target->GetExactDist2d(&l_PlatformData.second) };

                    float l_CurrentDist = 0.0f;
                    if (l_NearestPlatform.second > (l_CurrentDist = l_Target->GetExactDist2d(&l_PlatformData.second)))
                        l_NearestPlatform = { l_PlatformData.first, l_CurrentDist };
                }

                AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this, l_NearestPlatform]() -> void
                {
                    GameObject* l_Platform = me->FindNearestGameObject(l_NearestPlatform.first, 200.0f);
                    if (!l_Platform)
                        return;

                    l_Platform->SetDestructibleState(GameObjectDestructibleState::GO_DESTRUCTIBLE_DESTROYED);
                });
            }

            void InitLowerPlatform()
            {
                Position l_SpawnPos;

                int32 l_CenterXPos = 2;
                int32 l_CenterYPos = 2;

                for (uint32 l_I = 0; l_I < m_LowerPlatforms.size(); ++l_I)
                {
                    auto& l_Pair = m_LowerPlatforms[l_I];

                    if (GameObject* l_Platform = me->FindNearestGameObject(l_Pair.first, 200.0f))
                    {
                        l_Platform->SetDestructibleState(GameObjectDestructibleState::GO_DESTRUCTIBLE_INTACT);
                        if (!l_SpawnPos)
                            l_SpawnPos = l_Platform->GetPosition();
                    }

                    if (!l_SpawnPos)
                        continue;

                    if (!l_Pair.second)
                    {
                        int32 l_XPos = l_I % 5;
                        int32 l_YPos = l_I / 5;

                        int32 l_XDiff = l_XPos - l_CenterXPos;
                        int32 l_YDiff = l_YPos - l_CenterYPos;

                        Position l_PlatformPosition = l_SpawnPos;
                        l_PlatformPosition.m_positionX += 20.0f * l_XDiff;
                        l_PlatformPosition.m_positionY -= 20.0f * l_YDiff;

                        l_Pair.second = l_PlatformPosition;
                    }
                }
            }

            void InitTimers()
            {
                for (auto& l_Itr : m_TimerData)
                {
                    for (auto& l_SecondItr : l_Itr.second)
                    {
                        while (!m_Timers[l_Itr.first][l_SecondItr.first].empty())
                            m_Timers[l_Itr.first][l_SecondItr.first].pop();

                        for (uint32 l_Timer : l_SecondItr.second)
                            m_Timers[l_Itr.first][l_SecondItr.first].push(l_Timer);
                    }
                }
            }

            void AutoSchedule(uint32 p_EventID)
            {
                if (!me->GetMap())
                    return;

                auto l_Itr = m_Timers.find(me->GetMap()->GetDifficultyID());
                if (l_Itr == m_Timers.end())
                    return;

                auto l_SecondItr = l_Itr->second.find(p_EventID);
                if (l_SecondItr == l_Itr->second.end())
                    return;

                if (l_SecondItr->second.empty())
                    return;

                events.ScheduleEvent(p_EventID, l_SecondItr->second.front());
                l_SecondItr->second.pop();
            }

            void DefaultEvents()
            {
                events.Reset();

                InitTimers();

                switch (m_Phase)
                {
                    case 1:
                    {
                        events.ScheduleEvent(eEvents::EventDesolate, 12 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventEnergize, 3500);
                        events.ScheduleEvent(eEvents::EventActivatePylon, 0);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            events.ScheduleEvent(eEvents::EventActivateTaintedPylon, 0);

                        AutoSchedule(eEvents::EventUnboundChaos);
                        AutoSchedule(eEvents::EventShadowyBlades);
                        AutoSchedule(eEvents::EventRuptureRealities);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic
                            || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic)
                            AutoSchedule(eEvents::EventTouchOfSargeras);

                        break;
                    }
                    case 3:
                        events.ScheduleEvent(eEvents::EventDesolate, 20 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventEnergize, 0);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                        {
                            events.ScheduleEvent(eEvents::EventDarkMark, 31100);
                            events.ScheduleEvent(eEvents::EventRainOfTheDestroyer, 14300);
                        }
                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic)
                            events.ScheduleEvent(eEvents::EventDarkMark, 21500);


                        break;
                    default:
                        break;
                }
            }

            void SummonShadowyBlades()
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowyBladeCountSpell, me->GetMap()->GetDifficultyID());
                if (!l_SpellInfo)
                    return;

                Position l_AvatarPosition = *me;
                l_AvatarPosition.m_orientation = M_PI;
                for (uint32 l_I = 0; l_I < l_SpellInfo->Effects[EFFECT_2].BasePoints; ++l_I)
                {
                    Position l_TargetPos;
                    if (l_I < 3)
                        l_AvatarPosition.SimplePosXYRelocationByAngle(l_TargetPos, 20.0f, frand(M_PI / 2.0f, (3.0f * M_PI) / 2.0f), true);
                    else
                        l_AvatarPosition.SimplePosXYRelocationByAngle(l_TargetPos, 85.0f, frand((2 * M_PI) / 3.0f, (4.0f * M_PI) / 3.0f), true);

                    l_TargetPos.m_positionZ = 1667.702f;
                    me->SummonCreature(eCreatures::NpcShadowyBlade, l_TargetPos);
                }
            }

            void SummonTouchOfSargeras()
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TouchOfSargerasCountSpell, me->GetMap()->GetDifficultyID());
                if (!l_SpellInfo)
                    return;

                Position l_AvatarPosition = *me;
                l_AvatarPosition.m_orientation = M_PI;
                for (uint32 l_I = 0; l_I < l_SpellInfo->Effects[EFFECT_1].BasePoints; ++l_I)
                {
                    Position l_TargetPos;
                    l_AvatarPosition.SimplePosXYRelocationByAngle(l_TargetPos, frand(20.0f, 85.0f), frand((2 * M_PI) / 3.0f, (4.0f * M_PI) / 3.0f), true);
                    l_TargetPos.m_positionZ = 1663.583f;
                    me->CastSpell(l_TargetPos, eSpells::TouchOfSargeras, true);
                }
            }

            void SummonRainOfTheDestroyer()
            {
                GameObject* l_Platform = me->FindNearestGameObject(eGameObjects::GoLowerPlatform1, 200.0f);
                if (!l_Platform)
                    return;

                std::list<Position> l_AvailablePlatforms;
                for (auto const& l_PlatformData : m_LowerPlatforms)
                {
                    GameObject* l_Platform = me->FindNearestGameObject(l_PlatformData.first, 150.0f);
                    if (!l_Platform || l_Platform->GetDestructibleState() == GameObjectDestructibleState::GO_DESTRUCTIBLE_DESTROYED)
                        continue;

                    l_AvailablePlatforms.push_back(l_PlatformData.second);
                }

                if (l_AvailablePlatforms.empty())
                    return;

                for (uint32 l_I = 0; l_I < 6; ++l_I)
                {
                    Position l_PlatformPos = JadeCore::Containers::SelectRandomContainerElement(l_AvailablePlatforms);
                    Position l_TargetPos;
                    bool l_Ok = false;
                    for (uint32 l_J = 0; l_J < 5; ++l_J)
                    {
                        l_TargetPos.m_positionX = l_PlatformPos.m_positionX + frand(-7.0f, 7.0f);
                        l_TargetPos.m_positionY = l_PlatformPos.m_positionY + frand(-7.0f, 7.0f);
                        l_TargetPos.m_positionZ = 1510.86f;

                        if (me->GetMap()->GetHeight(me->GetPhaseMask(), l_TargetPos.m_positionX, l_TargetPos.m_positionY, l_TargetPos.m_positionZ) < 1509.0f)
                            continue;

                        l_Ok = true;
                        break;
                    }

                    if (!l_Ok)
                        l_TargetPos = l_PlatformPos;

                    AddTimedDelayedOperation(l_I * IN_MILLISECONDS, [this, l_TargetPos]() -> void
                    {
                        me->SummonCreature(eCreatures::NpcFallenAvatarInfernalImpact, l_TargetPos);
                    });
                }
            }

            void ChangePhase()
            {
                Talk(eTalks::TalkTransition);

                m_Phase++;

                events.Reset();
                me->InterruptNonMeleeSpells(true);

                AddTimedDelayedOperation(1500, [this]() -> void
                {
                    m_Phase++;

                    me->CastSpell(me, eSpells::Consume, true);
                    me->CastSpell(me, eSpells::Annihilation, false);

                    std::list<Creature*> l_Summons;
                    me->GetCreatureListWithEntryInGrid(l_Summons, eCreatures::NpcShadowyBlade, 100.0f);
                    me->GetCreatureListWithEntryInGrid(l_Summons, eCreatures::NpcTouchOfSargeras, 100.0f);
                    me->GetCreatureListWithEntryInGrid(l_Summons, eCreatures::NpcContainementPylon, 250.0f);
                    me->GetCreatureListWithEntryInGrid(l_Summons, eCreatures::NpcPylonBeamStalker, 250.0f);
                    for (Creature* l_Creature : l_Summons)
                        l_Creature->DespawnOrUnsummon(0);

                    me->SetPower(Powers::POWER_ENERGY, 0);

                    DefaultEvents();
                    m_CheckLavaTimer = 1 * IN_MILLISECONDS;
                });
            }

            void CheckLava()
            {
                for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        if (l_Player->GetPositionZ() < 1509.5f)
                        {
                            if (!l_Player->HasAura(eSpells::MoltenFel))
                                l_Player->CastSpell(l_Player, eSpells::MoltenFel, true, nullptr, nullptr, me->GetGUID());
                        }
                        else
                            l_Player->RemoveAura(eSpells::MoltenFel);
                    }
                }

                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidHeroic
                    && me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                if (m_HasFallen && me->GetMap()->GetHeight(me->GetPhaseMask(), me->m_positionX, me->m_positionY, me->m_positionZ) < 1509.5f)
                {
                    if (!me->HasAura(eSpells::FelInfusion))
                        me->CastSpell(me, eSpells::FelInfusion, true);
                }
                else
                    me->RemoveAura(eSpells::FelInfusion);
            }

            void Reset() override
            {
                _Reset();

                if (!m_AddsSpawnScheduled)
                {
                    if (Creature* l_Maiden = sObjectAccessor->GetCreature(*me, m_MaidenGUID))
                        l_Maiden->DespawnOrUnsummon();

                    if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidLFR)
                    {
                        Creature* l_Maiden = me->SummonCreature(eCreatures::NpcMaidenOfValor, k_MaidenSpawnPosition);
                        if (l_Maiden)
                            m_MaidenGUID = l_Maiden->GetGUID();
                        else
                            m_MaidenGUID = 0;
                    }

                    std::list<Creature*> l_Summons;
                    me->GetCreatureListWithEntryInGrid(l_Summons, eCreatures::NpcContainementPylon, 250.0f);
                    me->GetCreatureListWithEntryInGrid(l_Summons, eCreatures::NpcPylonBeamStalker, 250.0f);
                    for (Creature* l_Summon : l_Summons)
                        l_Summon->DespawnOrUnsummon(0);

                    AddTimedDelayedOperation(200, [this]() -> void
                    {
                        m_Pylons.clear();

                        for (Position const& l_Position : k_ContainementPylons)
                        {
                            if (Creature* l_Pylon = me->SummonCreature(eCreatures::NpcContainementPylon, l_Position))
                                m_Pylons.push_back(l_Pylon->GetGUID());
                        }

                        for (Position const& l_Position : k_ContainementPylonStalkers)
                            me->SummonCreature(eCreatures::NpcPylonBeamStalker, l_Position);
                    });
                }

                if (GameObject* l_Door = me->FindNearestGameObject(eGameObjects::GoDoorPreFallenAvatar, 400.0f))
                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                m_RippleTimer = 0;
                m_EnergyTick = 0;
                m_LaserCount = 0;
                m_DesolateCounter = 1;
                m_DarkMarkCounter = 1;

                m_Phase = 1;
                m_HasFallen = false;

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(Powers::POWER_ENERGY, 0);

                GameObject* l_Platform = me->FindNearestGameObject(eGameObjects::GoUpperPlatform, 200.0f);
                if (l_Platform)
                    l_Platform->SetDestructibleState(GameObjectDestructibleState::GO_DESTRUCTIBLE_INTACT);

                if (Creature* l_Velen = me->FindNearestCreature(eCreatures::NpcFallenAvatarVelen, 300.0f))
                    l_Velen->DespawnOrUnsummon(0);
                if (Creature* l_Khadgar = me->FindNearestCreature(eCreatures::NpcFallenAvatarKhadgar, 300.0f))
                    l_Khadgar->DespawnOrUnsummon(0);
                if (Creature* l_Illidan = me->FindNearestCreature(eCreatures::NpcFallenAvatarIllidan, 300.0f))
                    l_Illidan->DespawnOrUnsummon(0);
                if (Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 300.0f))
                {
                    l_KilJaeden->ClearInCombat();
                    l_KilJaeden->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                }

                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);

                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetDisableGravity(true);
                me->SetCanFly(true, true);
                me->RemoveAura(eSpells::Annihilation);
                me->RemoveAura(eSpells::Sear);
                me->RemoveAura(eSpells::BlackWinds);
                me->RemoveAura(eSpells::FelInfusion);
                me->RemoveAura(eSpells::FelInfusionBuff);
                me->NearTeleportTo(me->GetHomePosition());

                InitLowerPlatform();

                if (m_IntroDone)
                {
                    me->CastSpell(me, eSpells::EquipWeapon, true);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    Creature* l_Maiden = me->FindNearestCreature(eCreatures::NpcMaidenOfValor, 50.0f);
                    if (!l_Maiden)
                        return;

                    l_Maiden->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }

                me->RemoveAura(eSpells::EncounterFrame);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                {
                    if (!me->isInCombat())
                        return;

                    if (GameObject* l_Door = me->FindNearestGameObject(eGameObjects::GoDoorPreFallenAvatar, 400.0f))
                        l_Door->SetGoState(GOState::GO_STATE_READY);
                });

                me->CastSpell(me, eSpells::EncounterFrame, true);
                if (Creature* l_Maiden = me->FindNearestCreature(eCreatures::NpcMaidenOfValor, 50.0f))
                    l_Maiden->CastSpell(l_Maiden, eSpells::EncounterFrame, true);

                DefaultEvents();
                Talk(eTalks::TalkAggro);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                events.Update(p_Diff);

                if (m_Phase == 3 && (m_CheckLavaTimer -= p_Diff) < 0)
                {
                    m_CheckLavaTimer = 500;
                    CheckLava();
                }

                if (!m_IntroDone)
                {
                    if (me->FindNearestPlayer(75.0f))
                    {
                        Creature* l_KilJeaden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f, true);
                        if (l_KilJeaden && l_KilJeaden->IsAIEnabled)
                            l_KilJeaden->AI()->DoAction(ACTION_1);
                        else
                        {
                            if (GameObject* l_LeftChain = me->FindNearestGameObject(eGameObjects::GoLeftChain, 150.0f))
                                l_LeftChain->SetGoState(GOState::GO_STATE_ACTIVE);
                            if (GameObject* l_RightChain = me->FindNearestGameObject(eGameObjects::GoRightChain, 150.0f))
                                l_RightChain->SetGoState(GOState::GO_STATE_ACTIVE);

                            me->RemoveAura(eSpells::Chained);
                            me->CastSpell(me, eSpells::EquipWeapon, true);
                            me->SetAIAnimKitId(0);
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            if (Creature* l_Maiden = me->FindNearestCreature(eCreatures::NpcMaidenOfValor, 50.0f))
                                l_Maiden->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        }

                        m_IntroDone = true;
                    }
                }

                Creature* l_Maiden = me->FindNearestCreature(eCreatures::NpcMaidenOfValor, 50.0f, true);
                if (m_Phase == 1 && (me->GetPower(Powers::POWER_ENERGY) >= 100 || me->GetHealthPct() <= 20.0f || (!l_Maiden && !m_AddsSpawnScheduled && (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidLFR))))
                {
                    ChangePhase();
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (m_Phase == 1 && m_RippleTimer <= 0)
                {
                    std::list<Player*> l_NearPlayers;
                    for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                    {
                        Player* l_Player = l_Iter->getSource();
                        if (!l_Player || !l_Player->isAlive() || !l_Player->IsWithinMeleeRange(me))
                            continue;

                        l_NearPlayers.push_back(l_Player);
                    }

                    if (l_NearPlayers.empty())
                    {
                        me->CastSpell(me, eSpells::RippleOfDarkness, false);
                        m_RippleTimer = 2 * IN_MILLISECONDS;
                    }
                    else
                    {
                        auto l_Itr = std::find(l_NearPlayers.begin(), l_NearPlayers.end(), me->getVictim());
                        if (l_Itr == l_NearPlayers.end())
                        {
                            l_Itr = l_NearPlayers.begin();
                            std::advance(l_Itr, urand(0, l_NearPlayers.size() - 1));
                            AttackStart(*l_Itr);
                        }
                    }
                }
                else
                    m_RippleTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_Phase == 3 && me->GetPower(Powers::POWER_ENERGY) >= 100.0f)
                {
                    Talk(eTalks::TalkRuptureRealities);
                    me->CastSpell(me, eSpells::RuptureRealitiesPhase2, false);
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDesolate:
                    {
                        Unit* l_Target = me->getVictim();
                        if (l_Target)
                            me->CastSpell(l_Target, eSpells::Desolate, false);

                        m_DesolateCounter++;

                        switch (m_Phase)
                        {
                            case 1:
                                events.ScheduleEvent(eEvents::EventDesolate, m_DesolateCounter % 4 == 3 ? 24300 : 11500);
                                break;
                            case 3:
                                events.ScheduleEvent(eEvents::EventDesolate, m_DesolateCounter % 2 ? 23 * IN_MILLISECONDS : 12 * IN_MILLISECONDS);
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case eEvents::EventRuptureRealities:
                    {
                        Talk(eTalks::TalkRuptureRealities);
                        me->CastSpell(me, eSpells::RuptureRealities, false);

                        AutoSchedule(eEvents::EventRuptureRealities);
                        break;
                    }
                    case eEvents::EventUnboundChaos:
                    {
                        Talk(eTalks::TalkUnboundChaos);
                        me->CastSpell(me, eSpells::UnboundChaos, false);

                        AutoSchedule(eEvents::EventUnboundChaos);
                        break;
                    }
                    case eEvents::EventShadowyBlades:
                    {
                        Talk(eTalks::TalkShadowyBlades);
                        SummonShadowyBlades();

                        AutoSchedule(eEvents::EventShadowyBlades);
                        break;
                    }
                    case eEvents::EventActivatePylon:
                    {
                        std::list<uint64> l_Pylons = m_Pylons;

                        l_Pylons.remove_if([this](uint64 p_PylonGuid) -> bool
                        {
                            if (Creature* l_Pylon = Creature::GetCreature(*me, p_PylonGuid))
                                return l_Pylon->HasAura(eSpells::TaintedMatrix) || l_Pylon->HasUnitState(UnitState::UNIT_STATE_CASTING);
                            else
                                return true;
                        });

                        Creature* l_Pylon = Creature::GetCreature(*me, JadeCore::Containers::SelectRandomContainerElement(l_Pylons));
                        if (!l_Pylon)
                            break;

                        l_Pylon->CastSpell(l_Pylon, eSpells::CorruptedMatrix, false);

                        events.ScheduleEvent(eEvents::EventActivatePylon, me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic ? 20 * IN_MILLISECONDS : 50 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventActivateTaintedPylon:
                    {
                        std::list<uint64> l_Pylons = m_Pylons;

                        /// Cleanup
                        {
                            for (uint64 l_PylonGuid : l_Pylons)
                            {
                                if (Creature* l_Pylon = Creature::GetCreature(*me, l_PylonGuid))
                                    l_Pylon->RemoveAura(eSpells::TaintedMatrix);
                            }

                            if (Creature* l_Maiden = me->FindNearestCreature(eCreatures::NpcMaidenOfValor, 50.0f, true))
                                l_Maiden->RemoveAura(eSpells::TaintedMatrixMaiden);

                            for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    l_Player->RemoveAura(eSpells::TaintedMatrixPlayer);
                            }
                        }

                        l_Pylons.remove_if([this](uint64 p_PylonGuid) -> bool
                        {
                            if (Creature* l_Pylon = Creature::GetCreature(*me, p_PylonGuid))
                                return l_Pylon->HasAura(eSpells::CorruptedMatrix) || l_Pylon->HasUnitState(UnitState::UNIT_STATE_CASTING);
                            else
                                return true;
                        });

                        JadeCore::RandomResizeList(l_Pylons, 2);

                        for (uint64 l_PylonGuid : l_Pylons)
                        {
                            if (Creature* l_Pylon = Creature::GetCreature(*me, l_PylonGuid))
                                l_Pylon->CastSpell(l_Pylon, eSpells::TaintedMatrix, false);
                        }

                        events.ScheduleEvent(eEvents::EventActivateTaintedPylon, 57 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventEnergize:
                    {
                        if (m_Phase == 1)
                        {
                            me->ModifyPower(Powers::POWER_ENERGY, 1);
                            events.ScheduleEvent(eEvents::EventEnergize, 3500);
                            break;
                        }

                        if (m_Phase == 3)
                        {
                            me->ModifyPower(Powers::POWER_ENERGY, ++m_EnergyTick % 3 ? 3 : 4);
                            events.ScheduleEvent(eEvents::EventEnergize, 1 * IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eEvents::EventDarkMark:
                    {
                        me->CastSpell(me, eSpells::DarkMark, false);

                        m_DarkMarkCounter++;

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic)
                            events.ScheduleEvent(eEvents::EventDarkMark, 34 * IN_MILLISECONDS);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            events.ScheduleEvent(eEvents::EventDarkMark, m_DarkMarkCounter == 2 ? 25500 : 30500);

                        break;
                    }
                    case eEvents::EventTouchOfSargeras:
                    {
                        Talk(eTalks::TalkTouchOfSargeras);
                        SummonTouchOfSargeras();

                        AutoSchedule(eEvents::EventTouchOfSargeras);
                        break;
                    }
                    case eEvents::EventRainOfTheDestroyer:
                    {
                        SummonRainOfTheDestroyer();

                        events.ScheduleEvent(eEvents::EventRainOfTheDestroyer, 35 * IN_MILLISECONDS);
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
            return new boss_fallen_avatarAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Maiden Of Valor - 117264
class npc_fallen_avatar_maiden_of_valor : public CreatureScript
{
    public:
        npc_fallen_avatar_maiden_of_valor() : CreatureScript("npc_fallen_avatar_maiden_of_valor") { }

        struct npc_fallen_avatar_maiden_of_valorAI : public ScriptedAI
        {
            npc_fallen_avatar_maiden_of_valorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
            }

            enum eSpells
            {
                CleansingProtocol = 233856
            };

            enum eTalks
            {
                TalkCleansingProtocol
            };

            void Reset() override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::CleansingProtocol:
                        me->SetPower(Powers::POWER_ENERGY, 0);
                        break;
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_FallenAvatar = me->FindNearestCreature(eCreatures::BossFallenAvatar, 150.0f);
                if (!l_FallenAvatar || !l_FallenAvatar->IsAIEnabled)
                    return;

                l_FallenAvatar->AI()->DoAction(ACTION_2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (me->GetPower(Powers::POWER_ENERGY) >= 100)
                {
                    Talk(eTalks::TalkCleansingProtocol);
                    me->CastSpell(me, eSpells::CleansingProtocol, false);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_maiden_of_valorAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Cleansing Protocol - 233856
class spell_maiden_of_valor_cleansing_protocol : public SpellScriptLoader
{
    public:
        spell_maiden_of_valor_cleansing_protocol() : SpellScriptLoader("spell_maiden_of_valor_cleansing_protocol") { }

        class spell_maiden_of_valor_cleansing_protocol_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maiden_of_valor_cleansing_protocol_SpellScript);

            enum eSpells
            {
                CleansingProtocolShield = 241008
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint64 l_MaxHealth = l_Caster->GetMaxHealth();
                int32 l_ShieldAmount = CalculatePct(l_MaxHealth, 5);

                l_Caster->CastCustomSpell(l_Caster, eSpells::CleansingProtocolShield, &l_ShieldAmount, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_maiden_of_valor_cleansing_protocol_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_maiden_of_valor_cleansing_protocol_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Cleansing Protocol - 241008
class spell_maiden_of_valor_cleansing_protocol_shield : public SpellScriptLoader
{
    public:
        spell_maiden_of_valor_cleansing_protocol_shield() : SpellScriptLoader("spell_maiden_of_valor_cleansing_protocol_shield") { }

        class spell_maiden_of_valor_cleansing_protocol_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maiden_of_valor_cleansing_protocol_shield_AuraScript);

            enum eSpells
            {
                Malfunction = 233739
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Malfunction, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_maiden_of_valor_cleansing_protocol_shield_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maiden_of_valor_cleansing_protocol_shield_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Malfuntion - 233739
class spell_maiden_of_valor_malfunction : public SpellScriptLoader
{
    public:
        spell_maiden_of_valor_malfunction() : SpellScriptLoader("spell_maiden_of_valor_malfunction") { }

        class spell_maiden_of_valor_malfunction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maiden_of_valor_malfunction_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                uint32 l_HealthPct = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                l_Caster->ModifyHealth(-static_cast<int64>(CalculatePct(l_Caster->GetMaxHealth(), l_HealthPct)));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_maiden_of_valor_malfunction_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_maiden_of_valor_malfunction_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Rupture Realities - 239132
/// Rupture Realities - 235572
class spell_fallen_avatar_rupture_realities : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_rupture_realities() : SpellScriptLoader("spell_fallen_avatar_rupture_realities") { }

        class spell_fallen_avatar_rupture_realities_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_rupture_realities_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_FallenAvatar = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_FallenAvatar || !l_Target)
                    return;

                uint32 l_Damage = GetHitDamage();
                float l_Distance = l_FallenAvatar->GetExactDist2d(l_Target);

                l_Damage = l_Damage * (70 - std::min<uint32>(l_Distance, 50));
                l_Damage /= 70;

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_rupture_realities_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_rupture_realities_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Unbound Chaos - 234057
class spell_fallen_avatar_unbound_chaos_filter : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_unbound_chaos_filter() : SpellScriptLoader("spell_fallen_avatar_unbound_chaos_filter") { }

        class spell_fallen_avatar_unbound_chaos_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_unbound_chaos_filter_SpellScript);

            enum eSpells
            {
                UnboundChaosAura = 234059
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::UnboundChaosAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_unbound_chaos_filter_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_unbound_chaos_filter_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Unbound Chaos - 234059
class spell_fallen_avatar_unbound_chaos_aura : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_unbound_chaos_aura() : SpellScriptLoader("spell_fallen_avatar_unbound_chaos_aura") { }

        class spell_fallen_avatar_unbound_chaos_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_unbound_chaos_aura_AuraScript);

            enum eSpells
            {
                UnboundChaosMissile = 239482
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::UnboundChaosMissile, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_unbound_chaos_aura_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_unbound_chaos_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Shadowy Blades - 119158
class npc_fallen_avatar_shadowy_blades : public CreatureScript
{
    public:
        npc_fallen_avatar_shadowy_blades() : CreatureScript("npc_fallen_avatar_shadowy_blades") { }

        struct npc_fallen_avatar_shadowy_bladesAI : public ScriptedAI
        {
            npc_fallen_avatar_shadowy_bladesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                VisualBeam          = 237666,
                CastVisual          = 236571,
                ShadowyBladesFilter = 238460
            };

            uint64 m_TargetGUID;
            bool m_Casted;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                m_TargetGUID = 0;

                std::list<Unit*> l_NearPlayers;
                float l_Radius = 100.0f;

                JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck l_Ucheck(me, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> l_Searcher(me, l_NearPlayers, l_Ucheck);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                for (auto l_Itr = l_NearPlayers.begin(); l_Itr != l_NearPlayers.end();)
                {
                    if (!(*l_Itr)->IsPlayer() || (*l_Itr)->HasAura(eSpells::VisualBeam) || (*l_Itr)->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        l_Itr = l_NearPlayers.erase(l_Itr);
                    else
                        ++l_Itr;
                }

                if (l_NearPlayers.empty())
                    return;

                JadeCore::RandomResizeList(l_NearPlayers, 1);

                me->CastSpell(l_NearPlayers.front(), eSpells::VisualBeam, true);
                me->CastSpell(l_NearPlayers.front(), eSpells::CastVisual, false);
                m_TargetGUID = l_NearPlayers.front()->GetGUID();

                m_Casted = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_Casted)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_Target)
                {
                    me->NearTeleportTo(6753.295, -799.6613, 1400.0f, 0.0f);
                    me->DespawnOrUnsummon(1 * IN_MILLISECONDS);
                    return;
                }

                me->CastSpell(l_Target, eSpells::ShadowyBladesFilter, true);
                m_Casted = true;

                Position l_Dest = *l_Target;
                l_Dest.m_positionZ = 1667.702f;
                me->GetMotionMaster()->MoveCharge(&l_Dest, 100.0f);

                AddTimedDelayedOperation(1200, [this]() -> void
                {
                    me->NearTeleportTo(6753.295, -799.6613, 1400.0f, 0.0f);
                    me->DespawnOrUnsummon(1 * IN_MILLISECONDS);
                });
            }

            uint64 GetGUID(int32 p_ID /* = 0 */) override
            {
                return m_TargetGUID;
;            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_shadowy_bladesAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Shadowy Blades - 238460
class spell_fallen_avatar_shadowy_blades : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_shadowy_blades() : SpellScriptLoader("spell_fallen_avatar_shadowy_blades") { }

        class spell_fallen_avatar_shadowy_blades_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_shadowy_blades_SpellScript);

            enum eSpells
            {
                ShadowyBladesAreatrigger = 236604
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->ToCreature()->AI()->GetGUID());
                if (!l_Target)
                    return;

                p_Targets.remove_if([l_Caster, l_Target](WorldObject* p_Target) -> bool
                {
                    return !p_Target->IsInAxe(l_Caster, l_Target, 3.0f);
                });
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_FallenAvatar = l_Caster->FindNearestCreature(eCreatures::BossFallenAvatar, 150.0f);
                if (!l_FallenAvatar)
                    return;

                if (l_FallenAvatar->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidHeroic
                    && l_FallenAvatar->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                l_FallenAvatar->CastSpell(l_Target, eSpells::ShadowyBladesAreatrigger, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fallen_avatar_shadowy_blades_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST_2);
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_shadowy_blades_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_shadowy_blades_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Corrupted Matrix - 233556
class spell_fallen_avatar_corrupted_matrix : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_corrupted_matrix() : SpellScriptLoader("spell_fallen_avatar_corrupted_matrix") { }

        class spell_fallen_avatar_corrupted_matrix_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_corrupted_matrix_SpellScript);

            enum eSpells
            {
                PylonBeam = 238244
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::PylonBeam, true);

                if (l_Caster->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    Creature* l_Stalker = l_Caster->FindNearestCreature(eCreatures::NpcPylonBeamStalker, 60.0f);
                    if (!l_Stalker)
                        return;

                    Aura* l_Aura = l_Stalker->GetAura(eSpells::PylonBeam, l_Caster->GetGUID());
                    if (!l_Aura)
                    {
                        sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 1635). l_Duration = 12000");
                        return;
                    }

                    l_Aura->SetDuration(12 * TimeConstants::IN_MILLISECONDS);
                    l_Aura->SetMaxDuration(12 * TimeConstants::IN_MILLISECONDS);
                }, 1);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_fallen_avatar_corrupted_matrix_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_corrupted_matrix_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Pylon Beam - 238244
class spell_fallen_avatar_pylon_beam : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_pylon_beam() : SpellScriptLoader("spell_fallen_avatar_pylon_beam") { }

        class spell_fallen_avatar_pylon_beam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_pylon_beam_AuraScript);

            enum eSpells
            {
                PylonBeam           = 238244,
                PylonBeamMaiden     = 238335
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Maiden = l_Caster->FindNearestCreature(eCreatures::NpcMaidenOfValor, 60.0f);
                if (!l_Maiden || !l_Maiden->IsInAxe(l_Caster, l_Target, 2.0f))
                    return;

                l_Target->AddDelayedEvent([l_Target]() -> void
                {
                    l_Target->RemoveAura(eSpells::PylonBeam);
                }, 1);

                int32 l_Duration = p_AuraEffect->GetBase()->GetDuration();

                l_Caster->CastSpell(l_Maiden, eSpells::PylonBeamMaiden, true);
                l_Maiden->AddDelayedEvent([l_Maiden, l_Duration]() -> void
                {
                    Aura* l_Aura = l_Maiden->GetAura(eSpells::PylonBeamMaiden);
                    if (!l_Aura)
                    {
                        sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 1697). l_Duration = %u", l_Duration);
                        return;
                    }

                    l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                }, 1 * IN_MILLISECONDS);
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Creature* l_FallenAvatar = l_Target->FindNearestCreature(eCreatures::BossFallenAvatar, 20.0f);
                if (!l_FallenAvatar || !l_FallenAvatar->IsAIEnabled)
                    return;

                l_FallenAvatar->AI()->DoAction(ACTION_3);
            }

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Creature* l_FallenAvatar = l_Target->FindNearestCreature(eCreatures::BossFallenAvatar, 20.0f);
                if (!l_FallenAvatar || !l_FallenAvatar->IsAIEnabled)
                    return;

                l_FallenAvatar->AI()->DoAction(ACTION_4);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_pylon_beam_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_fallen_avatar_pylon_beam_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_fallen_avatar_pylon_beam_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_pylon_beam_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Pylon Beam - 238335
class spell_fallen_avatar_pylon_beam_maiden : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_pylon_beam_maiden() : SpellScriptLoader("spell_fallen_avatar_pylon_beam_maiden") { }

        class spell_fallen_avatar_pylon_beam_maiden_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_pylon_beam_maiden_AuraScript);

            enum eSpells
            {
                PylonBeam           = 238244,
                PylonBeamMaiden     = 238335,
                CleansingProtocol   = 241008,
                MatrixEmpowerment   = 233961
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->ModifyPower(Powers::POWER_ENERGY, p_AuraEffect->GetTickNumber() % 5 ? 2 : 3);
                if (l_Target->HasAura(eSpells::CleansingProtocol) && !l_Target->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    l_Target->CastSpell(l_Target, eSpells::MatrixEmpowerment, true);

                Creature* l_Stalker = l_Caster->FindNearestCreature(eCreatures::NpcPylonBeamStalker, 60.0f);
                if (!l_Stalker || l_Target->IsInAxe(l_Caster, l_Stalker, 2.0f))
                    return;

                l_Target->AddDelayedEvent([l_Target]() -> void
                {
                    l_Target->RemoveAura(eSpells::PylonBeamMaiden);
                }, 1);

                int32 l_Duration = p_AuraEffect->GetBase()->GetDuration();

                l_Caster->CastSpell(l_Stalker, eSpells::PylonBeam, true);
                l_Stalker->AddDelayedEvent([l_Stalker, l_Duration]() -> void
                {
                    Aura* l_Aura = l_Stalker->GetAura(eSpells::PylonBeam);
                    if (!l_Aura)
                    {
                        sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 1792). l_Duration = %u", l_Duration);
                        return;
                    }

                    l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                }, 1 * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_pylon_beam_maiden_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_pylon_beam_maiden_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Tainted Matrix - 240623
class spell_fallen_avatar_tainted_matrix : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_tainted_matrix() : SpellScriptLoader("spell_fallen_avatar_tainted_matrix") { }

        class spell_fallen_avatar_tainted_matrix_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_tainted_matrix_SpellScript);

            enum eSpells
            {
                PylonBeam = 240630
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::PylonBeam, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_fallen_avatar_tainted_matrix_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_tainted_matrix_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Pylon Beam - 240630
class spell_fallen_avatar_tainted_pylon_beam : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_tainted_pylon_beam() : SpellScriptLoader("spell_fallen_avatar_tainted_pylon_beam") { }

        class spell_fallen_avatar_tainted_pylon_beam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_tainted_pylon_beam_AuraScript);

            enum eSpells
            {
                TaintedPylonBeam            = 240630,
                TaintedMatrixMaiden         = 240758,
                TaintedMatrixPlayer         = 240631,
                SparkOfTheDarkTitan         = 234873, // TODO
                TaintedBindingBeam          = 240628,
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_NearestPlayerInAxe = nullptr;
                std::list<Player*> l_Players;
                l_Caster->GetPlayerListInGrid(l_Players, 60.0f);
                for (Player* l_Player : l_Players)
                {
                    if (l_NearestPlayerInAxe && l_NearestPlayerInAxe->GetDistance(l_Caster) < l_Player->GetDistance(l_Caster))
                        continue;

                    if (!l_Player->IsInAxe(l_Caster, l_Target, 2.0f))
                        continue;

                    l_NearestPlayerInAxe = l_Player;
                }

                Creature* l_Maiden = l_Caster->FindNearestCreature(eCreatures::NpcMaidenOfValor, 60.0f);
                bool l_MaidenCloser = (l_Maiden && l_Maiden->IsInAxe(l_Caster, l_Target, 2.0f) && (!l_NearestPlayerInAxe || l_NearestPlayerInAxe->GetDistance(l_Caster) > l_Maiden->GetDistance(l_Caster)));
                if (l_MaidenCloser || l_NearestPlayerInAxe)
                {
                    l_Target->AddDelayedEvent([l_Target]() -> void
                    {
                        l_Target->RemoveAura(eSpells::TaintedPylonBeam);
                    }, 1);

                    int32 l_Duration = p_AuraEffect->GetBase()->GetDuration();

                    if (l_MaidenCloser)
                    {
                        l_Caster->CastSpell(l_Maiden, eSpells::TaintedMatrixMaiden, true);
                        l_Maiden->AddDelayedEvent([l_Maiden, l_Duration]() -> void
                        {
                            Aura* l_Aura = l_Maiden->GetAura(eSpells::TaintedMatrixMaiden);
                            if (!l_Aura)
                            {
                                sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 1909). l_Duration = %u", l_Duration);
                                return;
                            }

                            l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                        }, 1 * IN_MILLISECONDS);

                        return;
                    }

                    l_Caster->CastSpell(l_NearestPlayerInAxe, eSpells::TaintedMatrixPlayer, true);
                    l_NearestPlayerInAxe->AddDelayedEvent([l_NearestPlayerInAxe, l_Duration]() -> void
                    {
                        Aura* l_Aura = l_NearestPlayerInAxe->GetAura(eSpells::TaintedMatrixPlayer);
                        if (!l_Aura)
                        {
                            sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 1925). l_Duration = %u", l_Duration);
                            return;
                        }

                        l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                    }, 1 * IN_MILLISECONDS);
                }
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Creature* l_FallenAvatar = l_Target->FindNearestCreature(eCreatures::BossFallenAvatar, 20.0f);
                if (!l_FallenAvatar || !l_FallenAvatar->IsAIEnabled)
                    return;

                l_FallenAvatar->AI()->DoAction(ACTION_3);
            }

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Creature* l_FallenAvatar = l_Target->FindNearestCreature(eCreatures::BossFallenAvatar, 20.0f);
                if (!l_FallenAvatar || !l_FallenAvatar->IsAIEnabled)
                    return;

                l_FallenAvatar->AI()->DoAction(ACTION_4);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_tainted_pylon_beam_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_fallen_avatar_tainted_pylon_beam_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_fallen_avatar_tainted_pylon_beam_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_tainted_pylon_beam_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Tainted Matrix - 240758
class spell_fallen_avatar_tainted_matrix_maiden : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_tainted_matrix_maiden() : SpellScriptLoader("spell_fallen_avatar_tainted_matrix_maiden") { }

        class spell_fallen_avatar_tainted_matrix_maiden_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_tainted_matrix_maiden_AuraScript);

            enum eSpells
            {
                TaintedPylonBeam        = 240630,
                TaintedMatrixMaiden     = 240758,
                TaintedMatrixPlayer     = 240631
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Maiden = GetTarget();
                if (!l_Caster || !l_Maiden)
                    return;

                Unit* l_Stalker = l_Caster->FindNearestCreature(eCreatures::NpcPylonBeamStalker, 70.0f);
                if (!l_Stalker)
                    return;

                Player* l_NearestPlayerInAxe = nullptr;
                std::list<Player*> l_Players;
                l_Caster->GetPlayerListInGrid(l_Players, 60.0f);
                for (Player* l_Player : l_Players)
                {
                    if (l_NearestPlayerInAxe && l_NearestPlayerInAxe->GetDistance(l_Caster) < l_Player->GetDistance(l_Caster))
                        continue;

                    if (!l_Player->IsInAxe(l_Caster, l_Stalker, 2.0f))
                        continue;

                    l_NearestPlayerInAxe = l_Player;
                }

                if (l_Maiden && l_Maiden->IsInAxe(l_Caster, l_Stalker, 2.0f) && (!l_NearestPlayerInAxe || l_NearestPlayerInAxe->GetDistance(l_Caster) > l_Maiden->GetDistance(l_Caster)))
                    return;

                PreventDefaultAction();

                l_Maiden->AddDelayedEvent([l_Maiden]() -> void
                {
                    l_Maiden->RemoveAura(eSpells::TaintedMatrixMaiden);
                }, 1);

                int32 l_Duration = p_AuraEffect->GetBase()->GetDuration();

                if (l_NearestPlayerInAxe)
                {
                    l_Caster->CastSpell(l_NearestPlayerInAxe, eSpells::TaintedMatrixPlayer, true);
                    l_NearestPlayerInAxe->AddDelayedEvent([l_NearestPlayerInAxe, l_Duration]() -> void
                    {
                        Aura* l_Aura = l_NearestPlayerInAxe->GetAura(eSpells::TaintedMatrixPlayer);
                        if (!l_Aura)
                        {
                            sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 2037). l_Duration = %u", l_Duration);
                            return;
                        }

                        l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                    }, 1 * IN_MILLISECONDS);

                    return;
                }

                l_Caster->CastSpell(l_Caster, eSpells::TaintedPylonBeam, true);
                l_Caster->AddDelayedEvent([l_Stalker, l_Duration]() -> void
                {
                    Aura* l_Aura = l_Stalker->GetAura(eSpells::TaintedPylonBeam);
                    if (!l_Aura)
                    {
                        sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 2053). l_Duration = %u", l_Duration);
                        return;
                    }

                    l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                }, 1 * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_tainted_matrix_maiden_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_tainted_matrix_maiden_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Tainted Matrix - 240631
class spell_fallen_avatar_tainted_matrix_player : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_tainted_matrix_player() : SpellScriptLoader("spell_fallen_avatar_tainted_matrix_player") { }

        class spell_fallen_avatar_tainted_matrix_player_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_tainted_matrix_player_AuraScript);

            enum eSpells
            {
                TaintedPylonBeam            = 240630,
                TaintedMatrixMaiden         = 240758,
                TaintedMatrixPlayer         = 240631,

                TaintedMatrixPlayerDebuff   = 240728
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Unit* l_Stalker = l_Caster->FindNearestCreature(eCreatures::NpcPylonBeamStalker, 70.0f);
                Unit* l_Maiden = l_Caster->FindNearestCreature(eCreatures::NpcMaidenOfValor, 70.0f);
                if (!l_Stalker || !l_Maiden)
                    return;

                Unit* l_NearestPlayerInAxe = nullptr;
                std::list<Player*> l_Players;
                l_Caster->GetPlayerListInGrid(l_Players, 60.0f);
                for (Player* l_Player : l_Players)
                {
                    if (l_NearestPlayerInAxe && l_NearestPlayerInAxe->GetDistance(l_Caster) < l_Player->GetDistance(l_Caster))
                        continue;

                    if (!l_Player->IsInAxe(l_Caster, l_Stalker, 2.0f))
                        continue;

                    l_NearestPlayerInAxe = l_Player;
                }

                std::list<Unit*> l_PotentialTargets;
                if (l_NearestPlayerInAxe)
                    l_PotentialTargets.push_back(l_NearestPlayerInAxe);

                if (l_Maiden->IsInAxe(l_Caster, l_Stalker, 2.0f))
                    l_PotentialTargets.push_back(l_Maiden);

                l_PotentialTargets.push_back(l_Stalker);

                l_PotentialTargets.sort(JadeCore::DistanceOrderPred(l_Caster));

                if (l_PotentialTargets.front() == l_Target)
                {
                    Creature* l_FallenAvatar = l_Target->FindNearestCreature(eCreatures::BossFallenAvatar, 150.0f);
                    if (!l_FallenAvatar)
                        return;

                    l_Target->CastSpell(l_Target, eSpells::TaintedMatrixPlayerDebuff, true, nullptr, nullptr, l_FallenAvatar->GetGUID());
                    return;
                }

                l_Target->AddDelayedEvent([l_Target]() -> void
                {
                    l_Target->RemoveAura(eSpells::TaintedMatrixPlayer);
                }, 1);

                int32 l_Duration = p_AuraEffect->GetBase()->GetDuration();

                if (l_PotentialTargets.front()->IsPlayer())
                {
                    l_Caster->CastSpell(l_NearestPlayerInAxe, eSpells::TaintedMatrixPlayer, true);
                    l_NearestPlayerInAxe->AddDelayedEvent([l_NearestPlayerInAxe, l_Duration]() -> void
                    {
                        Aura* l_Aura = l_NearestPlayerInAxe->GetAura(eSpells::TaintedMatrixPlayer);
                        if (!l_Aura)
                        {
                            sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 2155). l_Duration = %u", l_Duration);
                            return;
                        }

                        l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                    }, 1 * IN_MILLISECONDS);

                    return;
                }

                if (l_PotentialTargets.front()->GetEntry() == eCreatures::NpcPylonBeamStalker)
                {
                    Creature* l_Target = l_PotentialTargets.front()->ToCreature();
                    l_Caster->CastSpell(l_Caster, eSpells::TaintedPylonBeam, true);
                    l_Caster->AddDelayedEvent([l_Target, l_Duration]() -> void
                    {
                        Aura* l_Aura = l_Target ? l_Target->GetAura(eSpells::TaintedPylonBeam) : nullptr;
                        if (!l_Aura)
                        {
                            sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 2174). l_Duration = %u", l_Duration);
                            return;
                        }

                        l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                    }, 1 * IN_MILLISECONDS);
                    return;
                }

                l_Caster->CastSpell(l_Maiden, eSpells::TaintedMatrixMaiden, true);
                l_Maiden->AddDelayedEvent([l_Maiden, l_Duration]() -> void
                {
                    Aura* l_Aura = l_Maiden->GetAura(eSpells::TaintedMatrixMaiden);
                    if (!l_Aura)
                    {
                        sLog->outAshran("FallenAvatarLog: Couldn't find new aura to set duration (l. 2189). l_Duration = %u", l_Duration);
                        return;
                    }

                    l_Aura->SetDuration(std::max(l_Duration - 1 * IN_MILLISECONDS, 1));
                }, 1 * IN_MILLISECONDS);

            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_tainted_matrix_player_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_tainted_matrix_player_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Tainted Essence - 240728
class spell_fallen_avatar_tainted_essence : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_tainted_essence() : SpellScriptLoader("spell_fallen_avatar_tainted_essence") { }

        class spell_fallen_avatar_tainted_essence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_tainted_essence_AuraScript);

            enum eSpells
            {
                TaintedEssenceInstakill = 242081
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Aura || l_Aura->GetStackAmount() < 10)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Unit* l_FallenAvatar = l_Target->FindNearestCreature(eCreatures::BossFallenAvatar, 150.0f);
                if (!l_FallenAvatar)
                    return;

                l_Target->CastSpell(l_Target, eSpells::TaintedEssenceInstakill, true, nullptr, nullptr, l_FallenAvatar->GetGUID());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_tainted_essence_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_tainted_essence_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Consume - 240594
class spell_fallen_avatar_consume : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_consume() : SpellScriptLoader("spell_fallen_avatar_consume") { }

        class spell_fallen_avatar_consume_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_consume_SpellScript);

            void HandleAfterHit(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_Caster || !l_Target->isAlive())
                    return;

                float l_HealthPct = l_Target->GetHealthPct();
                l_Caster->ModifyHealth(CalculatePct(l_Caster->GetMaxHealth(), l_HealthPct));
                l_Caster->Kill(l_Target);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_consume_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_consume_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation - 235597
class spell_fallen_avatar_annihilation : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_annihilation() : SpellScriptLoader("spell_fallen_avatar_annihilation") { }

        class spell_fallen_avatar_annihilation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_annihilation_SpellScript);

            enum eSpells
            {
                SafeFall    = 238642,
                Jump        = 235613
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                GameObject* l_Platform = l_Caster->FindNearestGameObject(eGameObjects::GoUpperPlatform, 50.0f);
                if (!l_Platform)
                    return;

                std::list<Creature*> l_Pylons;
                l_Caster->GetCreatureListWithEntryInGrid(l_Pylons, eCreatures::NpcContainementPylon, 100.0f);
                for (Creature* l_Pylon : l_Pylons)
                    l_Pylon->InterruptNonMeleeSpells(true);

                l_Platform->SetDestructibleState(GameObjectDestructibleState::GO_DESTRUCTIBLE_DESTROYED);

                l_Caster->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                l_Caster->SetDisableGravity(false);
                l_Caster->SetCanFly(false, true);

                Position l_Dest = { 6606.24f, -792.5764f, 1509.918f, 10.0f };
                l_Caster->GetMotionMaster()->MoveJump(l_Dest, 50.0f, 19.642354899f, eMoveInform::HitGround);

                l_Caster->CastSpell(l_Caster, eSpells::SafeFall, true);

                l_Caster->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

                l_Caster->ToCreature()->StopAttack();
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                Creature* l_KilJaeden = l_Caster->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 250.0f);
                if (!l_KilJaeden || !l_KilJaeden->IsAIEnabled)
                    return;

                l_KilJaeden->AI()->DoAction(ACTION_2);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_fallen_avatar_annihilation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_annihilation_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Dark Mark - 239825
class spell_fallen_avatar_dark_mark : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_dark_mark() : SpellScriptLoader("spell_fallen_avatar_dark_mark") { }

        class spell_fallen_avatar_dark_mark_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_dark_mark_SpellScript);

            enum eSpells
            {
                DarkMarkAura = 239739
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DarkMarkAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_dark_mark_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_dark_mark_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Dark Mark (Aura) - 239739
class spell_fallen_avatar_dark_mark_aura : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_dark_mark_aura() : SpellScriptLoader("spell_fallen_avatar_dark_mark_aura") { }

        class spell_fallen_avatar_dark_mark_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_dark_mark_aura_AuraScript);

            enum eSpells
            {
               DarkMarkDamage       = 239742,
               DarkMarkKnockback    = 239819,
               DarkMarkKnockup      = 240398
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::DarkMarkDamage, true, nullptr, nullptr, l_Caster->GetGUID());
                l_Target->CastSpell(l_Target, eSpells::DarkMarkKnockback, true);
                l_Target->CastSpell(l_Target, eSpells::DarkMarkKnockup, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_fallen_avatar_dark_mark_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_dark_mark_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Touch Of Sargeras - 10236 (240827)
class at_fallen_avatar_touch_of_sargeras : public AreaTriggerEntityScript
{
    public:
        at_fallen_avatar_touch_of_sargeras() : AreaTriggerEntityScript("at_fallen_avatar_touch_of_sargeras") { }

        enum eSpells
        {
            VisualGeyser        = 240834,
            TouchOfSargerasAura = 240840,
            DamageSoaked        = 239058,
            DamageRaid          = 239287
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster || !p_Target->IsPlayer())
                return false;

            l_Caster->CastSpell(p_Target, eSpells::TouchOfSargerasAura);
            l_Caster->RemoveAura(eSpells::VisualGeyser);

            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            GuidList* l_AffectedPlayers = p_Areatrigger->GetAffectedPlayers();
            if (!l_Caster || !l_AffectedPlayers)
                return false;

            bool l_PlayerFound = false;
            for (uint64 l_GUID : *l_AffectedPlayers)
            {
                if (GUID_HIPART(l_GUID) != HighGuid::HIGHGUID_PLAYER)
                    continue;

                l_PlayerFound = true;
                break;
            }

            if (!l_PlayerFound)
            {
                l_Caster->RemoveAura(eSpells::TouchOfSargerasAura);
                l_Caster->CastSpell(l_Caster, eSpells::VisualGeyser, true);
            }

            return false;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Unit* l_FallenAvatar = l_Caster->FindNearestCreature(eCreatures::BossFallenAvatar, 250.0f);
            if (l_FallenAvatar)
            {
                if (l_Caster->HasAura(eSpells::TouchOfSargerasAura))
                    l_FallenAvatar->CastSpell(l_Caster, eSpells::DamageSoaked, true);
                else
                    l_FallenAvatar->CastSpell(l_Caster, eSpells::DamageRaid, true);
            }

            l_Caster->RemoveAura(eSpells::TouchOfSargerasAura);
            l_Caster->RemoveAura(eSpells::VisualGeyser);

            if (l_Caster->ToCreature())
            {
                l_Caster->SetVisible(false);
                l_Caster->ToCreature()->DespawnOrUnsummon(1);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_fallen_avatar_touch_of_sargeras();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Touch Of Sargeras - 120838
class npc_fallen_avatar_touch_of_sargeras : public CreatureScript
{
    public:
        npc_fallen_avatar_touch_of_sargeras() : CreatureScript("npc_fallen_avatar_touch_of_sargeras") { }

        struct npc_fallen_avatar_touch_of_sargerasAI : public ScriptedAI
        {
            npc_fallen_avatar_touch_of_sargerasAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eSpells
            {
                Areatrigger  = 240827,
                VisualGeyser = 240834
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    m_Started = true;

                    me->DelayedCastSpell(me, eSpells::Areatrigger, true, 100);
                    AddTimedDelayedOperation(9900, [this]() -> void
                    {
                        AreaTrigger* l_AreaTrigger = me->GetAreaTrigger(eSpells::Areatrigger);
                        if (!l_AreaTrigger)
                            return;

                        l_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                        l_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                        l_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                    });

                    me->CastSpell(me, eSpells::VisualGeyser, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_touch_of_sargerasAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Rain Of The Destroyer - 10264 (241039)
class at_fallen_avatar_rain_of_the_destroyer : public AreaTriggerEntityScript
{
    public:
        at_fallen_avatar_rain_of_the_destroyer() : AreaTriggerEntityScript("at_fallen_avatar_rain_of_the_destroyer") { }

        enum eSpells
        {
            VisualAura  = 244799,
            Missile     = 234417
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster || !p_Target->IsPlayer())
                return false;

            l_Caster->RemoveAura(eSpells::VisualAura);
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            GuidList* l_AffectedPlayers = p_Areatrigger->GetAffectedPlayers();
            if (!l_Caster || !l_AffectedPlayers)
                return false;

            bool l_PlayerFound = false;
            for (uint64 l_GUID : *l_AffectedPlayers)
            {
                if (GUID_HIPART(l_GUID) != HighGuid::HIGHGUID_PLAYER)
                    continue;

                l_PlayerFound = true;
                break;
            }

            if (!l_PlayerFound)
                l_Caster->CastSpell(l_Caster, eSpells::VisualAura, true);

            return false;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Creature* l_FallenAvatar = l_Caster->FindNearestCreature(eCreatures::BossFallenAvatar, 250.0f);
            if (l_FallenAvatar && l_FallenAvatar->IsAIEnabled)
            {
                l_FallenAvatar->CastSpell(l_Caster, eSpells::Missile, true);

                if (l_Caster->HasAura(eSpells::VisualAura))
                    l_FallenAvatar->AI()->SetGUID(l_Caster->GetGUID());
            }

            l_Caster->RemoveAura(eSpells::VisualAura);

            if (l_Caster->ToCreature())
            {
                l_Caster->SetVisible(false);
                l_Caster->ToCreature()->DespawnOrUnsummon(1);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_fallen_avatar_rain_of_the_destroyer();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Rain Of The Destroyer - 234418
class spell_fallen_avatar_rain_of_the_destroyer : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_rain_of_the_destroyer() : SpellScriptLoader("spell_fallen_avatar_rain_of_the_destroyer") { }

        class spell_fallen_avatar_rain_of_the_destroyer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_rain_of_the_destroyer_SpellScript);

            enum eSpells
            {
                RainOfTheDestroyer = 240396
            };

            bool m_RaidWide;

            void CheckTargetCount(std::list<WorldObject*>& p_Targets)
            {
                m_RaidWide = p_Targets.empty();
            }

            void HandleAfterCast()
            {
                if (!m_RaidWide)
                    return;

                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::RainOfTheDestroyer, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fallen_avatar_rain_of_the_destroyer_SpellScript::CheckTargetCount, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_fallen_avatar_rain_of_the_destroyer_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_rain_of_the_destroyer_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Rain Of The Destroyer - 240396
class spell_fallen_avatar_rain_of_the_destroyer_damage : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_rain_of_the_destroyer_damage() : SpellScriptLoader("spell_fallen_avatar_rain_of_the_destroyer_damage") { }

        class spell_fallen_avatar_rain_of_the_destroyer_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_rain_of_the_destroyer_damage_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {

                WorldLocation const* l_Center = GetExplTargetDest();
                Unit* l_Target = GetHitUnit();
                if (!l_Center || !l_Target)
                    return;

                l_Target->KnockbackFrom(l_Center->m_positionX, l_Center->m_positionY, 30, 10);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_rain_of_the_destroyer_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_rain_of_the_destroyer_damage_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Infernal Impact - 120961
class npc_fallen_avatar_infernal_impact : public CreatureScript
{
    public:
        npc_fallen_avatar_infernal_impact() : CreatureScript("npc_fallen_avatar_infernal_impact") { }

        struct npc_fallen_avatar_infernal_impactAI : public ScriptedAI
        {
            npc_fallen_avatar_infernal_impactAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eSpells
            {
                Areatrigger  = 241039
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    m_Started = true;

                    me->DelayedCastSpell(me, eSpells::Areatrigger, true, 100);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_infernal_impactAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Black Winds - 239417
class spell_fallen_avatar_black_winds : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_black_winds() : SpellScriptLoader("spell_fallen_avatar_black_winds") { }

        class spell_fallen_avatar_black_winds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_black_winds_AuraScript);

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                GameObject* l_Platform = l_Caster->FindNearestGameObject(eGameObjects::GoLowerPlatform1, 200.0f);
                if (!l_Platform)
                    return;

                Position l_Center = *l_Platform;

                int32 PosX = 0;
                int32 PosY = 0;
                if (urand(0, 1))
                {
                    PosX = irand(-2, 2);
                    PosY = urand(0, 1) ? -3 : 3;
                }
                else
                {
                    PosX = urand(0, 1) ? -3 : 3;
                    PosY = irand(-2, 2);
                }

                Position l_SpawnPosition = l_Center;
                l_SpawnPosition.m_positionX += 20.0f * PosX;
                l_SpawnPosition.m_positionY -= 20.0f * PosY;
                l_SpawnPosition.m_positionZ = 1509.125f;

                l_Caster->SummonCreature(eCreatures::NpcBlackWinds, l_SpawnPosition);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_black_winds_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_black_winds_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Black Winds - 121294
class npc_fallen_avatar_black_winds : public CreatureScript
{
    public:
        npc_fallen_avatar_black_winds() : CreatureScript("npc_fallen_avatar_black_winds") { }

        struct npc_fallen_avatar_black_windsAI : public ScriptedAI
        {
            npc_fallen_avatar_black_windsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidHeroic)
                    return;

                me->SetSpeed(UnitMoveType::MOVE_WALK, me->GetSpeedRate(UnitMoveType::MOVE_WALK) * 0.7f);
                me->SetSpeed(UnitMoveType::MOVE_RUN, me->GetSpeedRate(UnitMoveType::MOVE_RUN) * 0.7f);
            }

            enum eSpells
            {
                BlackWindsAreaTrigger = 239418
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                GameObject* l_Platform = me->FindNearestGameObject(eGameObjects::GoLowerPlatform1, 200.0f);
                if (!l_Platform)
                    return;

                me->GetMotionMaster()->Clear();
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                Position l_Dest = *me;

                if (me->m_positionX < (*l_Platform).m_positionX - 55.0f)
                {
                    l_Dest.m_positionX = (*l_Platform).m_positionX + 60.0f;
                    me->GetMotionMaster()->MovePoint(10, l_Dest);
                }
                else if (me->m_positionX > (*l_Platform).m_positionX + 55.0f)
                {
                    l_Dest.m_positionX = (*l_Platform).m_positionX - 60.0f;
                    me->GetMotionMaster()->MovePoint(10, l_Dest);
                }
                else if (me->m_positionY < (*l_Platform).m_positionY - 55.0f)
                {
                    l_Dest.m_positionY = (*l_Platform).m_positionY + 60.0f;
                    me->GetMotionMaster()->MovePoint(10, l_Dest);
                }
                else if (me->m_positionY > (*l_Platform).m_positionY + 55.0f)
                {
                    l_Dest.m_positionY = (*l_Platform).m_positionY - 60.0f;
                    me->GetMotionMaster()->MovePoint(10, l_Dest);
                }

                me->CastSpell(me, eSpells::BlackWindsAreaTrigger, true);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                        me->DespawnOrUnsummon(1);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_black_windsAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Fel Infusion - 236682
class spell_fallen_avatar_fel_infusion : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_fel_infusion() : SpellScriptLoader("spell_fallen_avatar_fel_infusion") { }

        class spell_fallen_avatar_fel_infusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fallen_avatar_fel_infusion_AuraScript);

            enum eSpells
            {
                FelInfusion = 236684
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FelInfusion, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fallen_avatar_fel_infusion_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fallen_avatar_fel_infusion_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Sear - 234180
class spell_fallen_avatar_sear : public SpellScriptLoader
{
    public:
        spell_fallen_avatar_sear() : SpellScriptLoader("spell_fallen_avatar_sear") { }

        class spell_fallen_avatar_sear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fallen_avatar_sear_SpellScript);

            enum eSpells
            {
                FelInfusion = 236684
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::FelInfusion, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                uint32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_AuraEffect->GetAmount());
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fallen_avatar_sear_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fallen_avatar_sear_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Kil'Jaeden (Events from Fallen Avatar intro and outro) - 120867
class npc_fallen_avatar_kil_jaeden : public CreatureScript
{
    public:
        npc_fallen_avatar_kil_jaeden() : CreatureScript("npc_fallen_avatar_kil_jaeden") { }

        struct npc_fallen_avatar_kil_jaedenAI : public ScriptedAI
        {
            npc_fallen_avatar_kil_jaedenAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Creature = me->FindNearestCreature(eCreatures::BossFallenAvatar, 250.0f);
                    if (l_Creature && l_Creature->isAlive())
                        return;

                    GameObject* l_UpperPlatform = me->FindNearestGameObject(eGameObjects::GoUpperPlatform, 250.0f);
                    if (!l_UpperPlatform)
                        return;

                    l_UpperPlatform->SetDestructibleState(GameObjectDestructibleState::GO_DESTRUCTIBLE_DESTROYED);

                    if (GameObject* l_FelStorm = me->FindNearestGameObject(eGameObjects::GoFelStorm, 250.0f))
                        l_FelStorm->SetGoState(GOState::GO_STATE_ACTIVE);
                    if (GameObject* l_FelStormCollision1 = me->FindNearestGameObject(eGameObjects::GoFelStormCollision1, 250.0f))
                        l_FelStormCollision1->SetGoState(GOState::GO_STATE_ACTIVE);
                    if (GameObject* l_FelStormCollision2 = me->FindNearestGameObject(eGameObjects::GoFelStormCollision2, 250.0f))
                        l_FelStormCollision2->SetGoState(GOState::GO_STATE_ACTIVE);
                    if (GameObject* l_LeftChain = me->FindNearestGameObject(eGameObjects::GoLeftChain, 150.0f))
                        l_LeftChain->SetGoState(GOState::GO_STATE_ACTIVE);
                    if (GameObject* l_RightChain = me->FindNearestGameObject(eGameObjects::GoRightChain, 150.0f))
                        l_RightChain->SetGoState(GOState::GO_STATE_ACTIVE);

                    me->DespawnOrUnsummon(1);
                });
            }

            enum eTalks
            {
                Intro1,
                Intro2,
                Intro3
            };

            enum eSpells
            {
                Awaken      = 240897,
                Teleport    = 240923,
                NetherShift = 245332
            };

            enum eConversation
            {
                Intro = 4899,
                Outro = 4955
            };

            Position const m_KhadgarSpawn   = { 6515.735f, -809.6979f, 1663.67f };
            Position const m_IllidanSpawn   = { 6514.167f, -785.1441f, 1663.67f };
            Position const m_VelenSpawn     = { 6515.987f, -797.2882f, 1663.67f };

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversation::Intro, me, nullptr, *me))
                            delete l_Conversation;

                        AddTimedDelayedOperation(35 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_FallenAvatar = me->FindNearestCreature(eCreatures::BossFallenAvatar, 50.0f);
                            if (!l_FallenAvatar || !l_FallenAvatar->IsAIEnabled)
                                return;

                            me->SetFacingToObject(l_FallenAvatar);
                            me->CastSpell(l_FallenAvatar, eSpells::Awaken, false);
                            l_FallenAvatar->AI()->DoAction(ACTION_1);
                        });

                        AddTimedDelayedOperation(40 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::Teleport, false);
                        });

                        AddTimedDelayedOperation(41750, [this]() -> void
                        {
                            Position l_TargetPos = { 6716.2300f, -782.2210f, 1521.1000f, 4.0746f };
                            me->NearTeleportTo(l_TargetPos);
                        });

                        break;
                    }
                    case ACTION_2:
                    {
                        me->RemoveAura(eSpells::NetherShift);

                        AddTimedDelayedOperation(9 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SummonCreature(eCreatures::NpcFallenAvatarIllidan, m_IllidanSpawn);
                            me->SummonCreature(eCreatures::NpcFallenAvatarKhadgar, m_KhadgarSpawn);
                        });

                        AddTimedDelayedOperation(11500, [this]() -> void
                        {
                            me->SummonCreature(eCreatures::NpcFallenAvatarVelen, m_VelenSpawn);
                        });

                        break;
                    }
                    case ACTION_3:
                    {
                        Creature* l_Illidan = me->FindNearestCreature(eCreatures::NpcFallenAvatarIllidan, 50.0f);
                        if (!l_Illidan)
                            break;

                        AttackStart(l_Illidan);
                        break;
                    }
                    case ACTION_4:
                    {
                        me->AttackStop();
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversation::Outro, me, nullptr, *me))
                            delete l_Conversation;

                        if (Creature* l_Velen = me->FindNearestCreature(eCreatures::NpcFallenAvatarVelen, 50.0f))
                        {
                            if (l_Velen->IsAIEnabled)
                                l_Velen->AI()->DoAction(ACTION_1);
                        }
                        if (Creature* l_Khadgar = me->FindNearestCreature(eCreatures::NpcFallenAvatarKhadgar, 50.0f))
                        {
                            if (l_Khadgar->IsAIEnabled)
                                l_Khadgar->AI()->DoAction(ACTION_1);
                        }
                        if (Creature* l_Illidan = me->FindNearestCreature(eCreatures::NpcFallenAvatarIllidan, 50.0f))
                        {
                            if (l_Illidan->IsAIEnabled)
                                l_Illidan->AI()->DoAction(ACTION_1);
                        }

                        AddTimedDelayedOperation(44 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetWalk(false);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(10, k_PortalPosition);
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

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_kil_jaedenAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Prophet Velen (Events from Fallen Avatar intro and outro) - 121170
class npc_fallen_avatar_prophet_velen : public CreatureScript
{
    public:
        npc_fallen_avatar_prophet_velen() : CreatureScript("npc_fallen_avatar_prophet_velen") { }

        struct npc_fallen_avatar_prophet_velenAI : public ScriptedAI
        {
            npc_fallen_avatar_prophet_velenAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
            }

            enum eSpells
            {
                VelenFlyOrb         = 241078,
                HolyBlast           = 241470,
                Penance             = 241463,
                ConversationMiddle  = 241103
            };

            enum eEvents
            {
                EventHolyBlast = 1,
                EventPenance
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    Creature* l_FallenAvatar = me->FindNearestCreature(eCreatures::BossFallenAvatar, 350.0f);
                    if (!l_FallenAvatar)
                        me->DespawnOrUnsummon(1);

                    m_Started = true;

                    me->CastSpell(me, eSpells::VelenFlyOrb, true);

                    me->GetMotionMaster()->Clear();
                    me->SetDisableGravity(true);
                    me->SetCanFly(true);
                    me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                    me->GetMotionMaster()->MoveSmoothFlyPath(10, me->GetEntry() * 100);
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventHolyBlast:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::HolyBlast, false);
                        events.ScheduleEvent(eEvents::EventHolyBlast, 3 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventPenance:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::Penance, false);
                        events.ScheduleEvent(eEvents::EventPenance, 12 * IN_MILLISECONDS);
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
                    case ACTION_1:
                        me->AttackStop();
                        events.Reset();

                        AddTimedDelayedOperation(60 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetWalk(false);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(20, k_PortalPosition);
                        });

                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        me->RemoveAura(eSpells::VelenFlyOrb);
                        me->CastSpell(me, eSpells::ConversationMiddle, true);
                        me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                        AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                            if (!l_KilJaeden || !l_KilJaeden->IsAIEnabled)
                                return;

                            l_KilJaeden->AI()->DoAction(ACTION_3);
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            events.ScheduleEvent(eEvents::EventHolyBlast, 0);
                            events.ScheduleEvent(eEvents::EventPenance, 12 * IN_MILLISECONDS);
                            AttackStart(l_KilJaeden);
                        });

                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->SetFacingToObject(l_KilJaeden);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_prophet_velenAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Illidan Stormrage (Events from Fallen Avatar intro and outro) - 121169
class npc_fallen_avatar_illidan_stormrage : public CreatureScript
{
    public:
        npc_fallen_avatar_illidan_stormrage() : CreatureScript("npc_fallen_avatar_illidan_stormrage") { }

        struct npc_fallen_avatar_illidan_stormrageAI : public ScriptedAI
        {
            npc_fallen_avatar_illidan_stormrageAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
            }

            enum eSpells
            {
                FlameCrash  = 241268,
                Shear       = 241484
            };

            enum eEvents
            {
                EventFlameCrash = 1,
                EventShear
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    Creature* l_FallenAvatar = me->FindNearestCreature(eCreatures::BossFallenAvatar, 350.0f);
                    if (!l_FallenAvatar)
                        me->DespawnOrUnsummon(1);

                    m_Started = true;

                    AddTimedDelayedOperation(2500, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->SetDisableGravity(true);
                        me->SetCanFly(true);
                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        me->GetMotionMaster()->MoveSmoothFlyPath(10, me->GetEntry() * 100);
                    });

                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFlameCrash:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::FlameCrash, false);
                        events.ScheduleEvent(eEvents::EventFlameCrash, 10 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShear:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::Shear, false);
                        events.ScheduleEvent(eEvents::EventShear, 10 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                        me->AttackStop();
                        events.Reset();

                        AddTimedDelayedOperation(115 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetWalk(false);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(20, k_PortalPosition);
                        });

                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                            if (!l_KilJaeden)
                                return;

                            events.ScheduleEvent(eEvents::EventFlameCrash, 5 * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventShear, 15 * IN_MILLISECONDS);
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            AttackStart(l_KilJaeden);
                        });

                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->SetFacingToObject(l_KilJaeden);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_illidan_stormrageAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Archmage Khadgar (Events from Fallen Avatar intro and outro) - 121171
class npc_fallen_avatar_archmage_khadgar: public CreatureScript
{
    public:
        npc_fallen_avatar_archmage_khadgar() : CreatureScript("npc_fallen_avatar_archmage_khadgar") { }

        struct npc_fallen_avatar_archmage_khadgarAI : public ScriptedAI
        {
            npc_fallen_avatar_archmage_khadgarAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
            }

            enum eSpells
            {
                KhadgarFlightDisc   = 241084,
                ArcaneBlast         = 241122,
                ArcaneBarrage       = 241128,
                ArcaneMissile       = 241123
            };

            enum eEvents
            {
                EventArcaneBlast = 1,
                EventArcaneBarrage,
                EventArcaneMissile
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    Creature* l_FallenAvatar = me->FindNearestCreature(eCreatures::BossFallenAvatar, 350.0f);
                    if (!l_FallenAvatar)
                        me->DespawnOrUnsummon(1);

                    m_Started = true;

                    me->CastSpell(me, eSpells::KhadgarFlightDisc, true);

                    AddTimedDelayedOperation(2500, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->SetDisableGravity(true);
                        me->SetCanFly(true);
                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        me->GetMotionMaster()->MoveSmoothFlyPath(10, me->GetEntry() * 100);
                    });
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcaneBlast:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::ArcaneBlast, false);
                        events.ScheduleEvent(eEvents::EventArcaneBlast, 2 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventArcaneBarrage:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::ArcaneBarrage, false);
                        events.ScheduleEvent(eEvents::EventArcaneBarrage, 15 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventArcaneMissile:
                    {
                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->CastSpell(l_KilJaeden, eSpells::ArcaneMissile, false);
                        events.ScheduleEvent(eEvents::EventArcaneMissile, 15 * IN_MILLISECONDS);
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
                    case ACTION_1:
                        me->AttackStop();
                        events.Reset();

                        AddTimedDelayedOperation(115 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetWalk(false);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(20, k_PortalPosition);
                        });

                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        me->RemoveAura(eSpells::KhadgarFlightDisc);

                        AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                            if (!l_KilJaeden)
                                return;

                            events.ScheduleEvent(eEvents::EventArcaneBlast, 0);
                            events.ScheduleEvent(eEvents::EventArcaneBarrage, 8 * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventArcaneMissile, 12 * IN_MILLISECONDS);
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                            AttackStart(l_KilJaeden);
                        });

                        Creature* l_KilJaeden = me->FindNearestCreature(eCreatures::NpcFallenAvatarKilJaeden, 50.0f);
                        if (!l_KilJaeden)
                            break;

                        me->SetFacingToObject(l_KilJaeden);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fallen_avatar_archmage_khadgarAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Fel Storm - 269783
class gob_fallen_avatar_fel_storm : public GameObjectScript
{
    public:
        gob_fallen_avatar_fel_storm() : GameObjectScript("gob_fallen_avatar_fel_storm") { }

        enum eScenes
        {
            FallenAvatarOutro = 1876
        };

        struct gob_fallen_avatar_fel_stormAI : public GameObjectAI
        {
            gob_fallen_avatar_fel_stormAI(GameObject* go) : GameObjectAI(go)
            {
                m_UpdateTimer = 0;
            }

            int32 m_UpdateTimer;
            std::set<uint64> m_ProcessedPlayers;
            std::set<uint64> m_ProcessedCreatures;

            void UpdateAI(uint32 p_Diff) override
            {
                if (go->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidLFR)
                    return;

                m_UpdateTimer -= p_Diff;
                if (m_UpdateTimer > 0)
                    return;

                m_UpdateTimer = 1 * IN_MILLISECONDS;

                std::list<Player*> l_Players;
                go->GetPlayerListInGrid(l_Players, 15.0f);

                for (Player* l_Player : l_Players)
                {
                    if (m_ProcessedPlayers.find(l_Player->GetGUID()) != m_ProcessedPlayers.end())
                        continue;

                    m_ProcessedPlayers.insert(l_Player->GetGUID());

                    l_Player->PlayStandaloneScene(eScenes::FallenAvatarOutro, 16, *l_Player);
                }

                std::list<Creature*> l_Creatures;
                go->GetCreatureListInGrid(l_Creatures, 15.0f);

                for (Creature* l_Creature : l_Creatures)
                {
                    if (m_ProcessedCreatures.find(l_Creature->GetGUID()) != m_ProcessedCreatures.end())
                        continue;

                    switch (l_Creature->GetEntry())
                    {
                        case eCreatures::NpcFallenAvatarIllidan:
                        case eCreatures::NpcFallenAvatarKhadgar:
                        case eCreatures::NpcFallenAvatarVelen:
                        case eCreatures::NpcFallenAvatarKilJaeden:
                        {
                            l_Creature->SetVisible(false);
                            m_ProcessedCreatures.insert(l_Creature->GetGUID());
                            break;
                        }
                        default:
                            m_ProcessedCreatures.insert(l_Creature->GetGUID());
                            break;
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_fallen_avatar_fel_stormAI(go);
        }
};

class Fallen_Avatar_PlayerScript : public PlayerScript
{
    public:
        Fallen_Avatar_PlayerScript() : PlayerScript("Fallen_Avatar_PlayerScript") {}

        enum eScenes
        {
            FallenAvatarOutro = 1876
        };

        Position const m_TeleportTarget = { 4500.30f, -1098.60f, 5336.00f, 4.7313f };

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(eScenes::FallenAvatarOutro) != p_SceneInstanceId)
                return;

            p_Player->TeleportTo(1676, m_TeleportTarget);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Defensive Countermesure - 120449
class npc_tos_defensive_countermesure : public CreatureScript
{
    public:
        npc_tos_defensive_countermesure() : CreatureScript("npc_tos_defensive_countermesure") { }

        struct npc_tos_defensive_countermesureAI : public LegionCombatAI
        {
            npc_tos_defensive_countermesureAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            void JustDied(Unit* p_Killer) override
            {
                LegionCombatAI::JustDied(p_Killer);

                if (me->FindNearestCreature(me->GetEntry(), 200.0f))
                    return;

                GameObject* l_Door = me->FindNearestGameObject(eGameObjects::GoDoorPreFallenAvatar, 200.0f);
                if (!l_Door)
                    return;

                l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
            }

            bool CanAutoAttack() override
            {
                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tos_defensive_countermesureAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_fallen_avatar()
{
    new boss_fallen_avatar();

    new spell_fallen_avatar_rupture_realities();
    new spell_fallen_avatar_unbound_chaos_filter();
    new spell_fallen_avatar_unbound_chaos_aura();
    new spell_fallen_avatar_corrupted_matrix();
    new spell_fallen_avatar_pylon_beam();
    new spell_fallen_avatar_pylon_beam_maiden();
    new spell_maiden_of_valor_cleansing_protocol();
    new spell_maiden_of_valor_cleansing_protocol_shield();
    new spell_maiden_of_valor_malfunction();
    new spell_fallen_avatar_consume();
    new spell_fallen_avatar_shadowy_blades();
    new spell_fallen_avatar_annihilation();
    new spell_fallen_avatar_dark_mark();
    new spell_fallen_avatar_dark_mark_aura();
    new spell_fallen_avatar_black_winds();
    new spell_fallen_avatar_tainted_matrix();
    new spell_fallen_avatar_tainted_pylon_beam();
    new spell_fallen_avatar_tainted_matrix_maiden();
    new spell_fallen_avatar_tainted_matrix_player();
    new spell_fallen_avatar_tainted_essence();
    new spell_fallen_avatar_rain_of_the_destroyer();
    new spell_fallen_avatar_rain_of_the_destroyer_damage();
    new spell_fallen_avatar_fel_infusion();
    new spell_fallen_avatar_sear();

    new npc_fallen_avatar_shadowy_blades();
    new npc_fallen_avatar_maiden_of_valor();
    new npc_fallen_avatar_touch_of_sargeras();
    new npc_fallen_avatar_black_winds();
    new npc_fallen_avatar_infernal_impact();

    new at_fallen_avatar_touch_of_sargeras();
    new at_fallen_avatar_rain_of_the_destroyer();

    new npc_fallen_avatar_kil_jaeden();
    new npc_fallen_avatar_illidan_stormrage();
    new npc_fallen_avatar_archmage_khadgar();
    new npc_fallen_avatar_prophet_velen();

    new gob_fallen_avatar_fel_storm();
    new Fallen_Avatar_PlayerScript();

    new npc_tos_defensive_countermesure();
}
#endif
