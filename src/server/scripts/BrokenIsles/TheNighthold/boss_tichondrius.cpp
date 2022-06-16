////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eScriptData
{
    VampiricAuraBonus,
    Phase1Summon,
    Phase2Summon,
    PhantasmaticBloodfang,
    TargetNetherZone,
    CarrionPlagueTargets,
    AchievementFailed
};

/// Tichondrius - 103685
class boss_tichondrius : public CreatureScript
{
    public:
        boss_tichondrius() : CreatureScript("boss_tichondrius") { }

        struct boss_tichondriusAI : BossAI
        {
            boss_tichondriusAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataTichondrius) { }

            enum eSpells
            {
                VampiricAura        = 206892,
                CarrionPlague       = 212997,
                SeekerSwarm         = 213238,
                FeastOfBlood        = 208230,
                EchoesOfTheVoid     = 213531,
                BrandOfArgus        = 212794,

                IllusionaryNight    = 206365,
                EssenceOfNight      = 206462,
                EssenceOfNightBuff  = 206466,

                NetherZone          = 216026
            };

            enum eEvents
            {
                EventCarrionPlague = 1,
                EventSeekerSwarm,
                EventFeastOfBlood,
                EventEchoesOfTheVoid,
                EventIllusionaryNight,
                EventCarrionNightmare,
                EventPhantasmalBloodfang,
                EventFelswornSpellguard,
                EventSightlessWatcher,
                EventBrandOfArgus,
                EventBerserk
            };

            enum eTalks
            {
                TalkIntro1,
                TalkIntro2,
                TalkAggro,
                TalkCarrionPlague,
                TalkFeastOfBlood,
                TalkCallUnderlings,
                TalkEchoesOfTheVoid,
                TalkIllusionaryNight,
                TalkKilledPlayer,
                TalkWipe,
                TalkDeath,
                WarningCarrionPlague
            };

            enum eTimers
            {
                BerserkNormal = 465 * IN_MILLISECONDS
            };

            enum eDisplayIDs
            {
                Invisible = 11686
            };

            enum eAchievements
            {
                NotForYou = 10704
            };

            Position const l_PositionCheck1 = { 256.65f, 3384.48f, 171.72f }; //32
            Position const l_PositionCheck2 = { 234.63f, 3405.72f, 171.72f }; //40

             std::map<Difficulty, std::map<uint32, std::vector<uint32>>> m_TimerData =
            {
                {
                    Difficulty::DifficultyRaidLFR,
                    {
                        { eEvents::EventCarrionPlague, { 5000, 25000, 35600, 24400, 75000, 25500, 35600, 26900, 75000, 25600, 40600, 20500, 53600, 25600} },
                        { eEvents::EventSeekerSwarm, { 25000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75000, 25600, 36100, 2250, 56100 } },
                        { eEvents::EventFeastOfBlood, { 20000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75100, 25600, 36200, 22500, 56100, 25600 } },
                        { eEvents::EventIllusionaryNight, { 130000, 153000, 153000 } },
                        { eEvents::EventEchoesOfTheVoid, { 55000, 59700, 85000, 65000, 80000, 67500 } },

                        { eEvents::EventCarrionNightmare, { 132000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000 } },
                        { eEvents::EventFelswornSpellguard, { 187000, 25000, 25000, 101000, 25000, 25000 } },
                        { eEvents::EventSightlessWatcher, { 338000, 25000, 25000 } },
                        { eEvents::EventPhantasmalBloodfang, { 132000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidNormal,
                    {
                        { eEvents::EventCarrionPlague, { 5000, 25000, 35600, 24400, 75000, 25500, 35600, 26900, 75000, 25600, 40600, 20500, 53600, 25600} },
                        { eEvents::EventSeekerSwarm, { 25000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75000, 25600, 36100, 2250, 56100 } },
                        { eEvents::EventFeastOfBlood, { 20000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75100, 25600, 36200, 22500, 56100, 25600 } },
                        { eEvents::EventIllusionaryNight, { 130000, 153000, 153000 } },
                        { eEvents::EventEchoesOfTheVoid, { 55000, 59700, 85000, 65000, 80000, 67500 } },

                        { eEvents::EventCarrionNightmare, { 132000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000 } },
                        { eEvents::EventFelswornSpellguard, { 187000, 25000, 25000, 101000, 25000, 25000 } },
                        { eEvents::EventSightlessWatcher, { 338000, 25000, 25000 } },
                        { eEvents::EventPhantasmalBloodfang, { 132000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidHeroic,
                    {
                        { eEvents::EventCarrionPlague, { 5000, 25000, 35600, 24400, 75000, 25500, 35600, 26900, 75000, 25600, 40600, 20500, 53600, 25600} },
                        { eEvents::EventSeekerSwarm, { 25000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75000, 25600, 36100, 2250, 56100 } },
                        { eEvents::EventFeastOfBlood, { 20000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75100, 25600, 36200, 22500, 56100, 25600 } },
                        { eEvents::EventBrandOfArgus, { 15000, 25000, 35000, 25000, 75000, 25500, 32500, 30000, 75000, 25600, 36100, 22500, 56100, 25600 } },
                        { eEvents::EventIllusionaryNight, { 130000, 153000, 153000 } },
                        { eEvents::EventEchoesOfTheVoid, { 55000, 59700, 85000, 65000, 80000, 67500 } },

                        { eEvents::EventCarrionNightmare, { 132000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000 } },
                        { eEvents::EventFelswornSpellguard, { 187000, 25000, 25000, 101000, 25000, 25000 } },
                        { eEvents::EventSightlessWatcher, { 338000, 25000, 25000 } },
                        { eEvents::EventPhantasmalBloodfang, { 132000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidMythic,
                    {
                        { eEvents::EventCarrionPlague, { 5000, 25000, 35600, 24400, 75000, 25500, 35600, 26900, 75000, 25600, 40600, 20500, 53600, 25600} },
                        { eEvents::EventSeekerSwarm, { 25000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75000, 25600, 36100, 2250, 56100 } },
                        { eEvents::EventFeastOfBlood, { 20000, 25000, 35000, 25000, 75000, 25500, 37500, 25000, 75100, 25600, 36200, 22500, 56100, 25600 } },
                        { eEvents::EventBrandOfArgus,{ 15000, 25000, 35000, 25000, 75000, 25500, 32500, 30000, 75000, 25600, 36100, 22500, 56100, 25600 } },
                        { eEvents::EventIllusionaryNight, { 130000, 153000, 153000 } },
                        { eEvents::EventEchoesOfTheVoid, { 55000, 59700, 85000, 65000, 80000, 67500 } },

                        { eEvents::EventCarrionNightmare, { 132000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000, 128000, 5000, 5000, 5000, 5000, 5000 } },
                        { eEvents::EventFelswornSpellguard, { 187000, 25000, 25000, 101000, 25000, 25000 } },
                        { eEvents::EventSightlessWatcher, { 338000, 25000, 25000 } },
                        { eEvents::EventPhantasmalBloodfang, { 132000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                                                                125000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1250, 1250, 1250, 1250, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 } }
                    }
                }
            };

            std::map<Difficulty, std::map<uint32, std::queue<uint32>>> m_Timers;

            std::array<Position, 5> const m_CarrionNightmares =
            {{
                { 284.4688f, 3399.675f, 171.8008f, 3.47469f },
                { 241.7361f, 3354.396f, 171.8008f, 1.35744f },
                { 240.7969f, 3430.175f, 171.8008f, 5.06633f },
                { 252.3177f, 3430.454f, 171.8008f, 4.53727f },
                { 209.3993f, 3386.834f, 171.8008f, 0.23903f }
            }};

            std::array<Position, 2> const m_FelswornSpellguards =
            {{
                { 285.3375f, 3398.4651f, 171.72f, 3.25f },
                { 242.9075f, 3354.4392f, 171.72f, 1.49f }
            }};

            Position const m_SightlessWatcher = { 269.0985f, 3371.5474f, 172.64f, 2.38f };

            Position const m_RoomCenter = { 241.8694f, 3398.8193f, 171.72f };

            uint32 m_VampiricAuraBonus;
            uint32 m_CarrionPlagueTargets;
            std::set<uint64> m_Players;

            EventMap m_AddsEvent;

            std::list<uint64> m_Phase1Summons;

            void Reset() override
            {
                _Reset();
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                me->RestoreDisplayId();
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->SetSpeed(MOVE_RUN, 1.5f);
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

                switch (p_EventID)
                {
                    case eEvents::EventCarrionPlague:
                    case eEvents::EventSeekerSwarm:
                    case eEvents::EventFeastOfBlood:
                    case eEvents::EventEchoesOfTheVoid:
                    case eEvents::EventIllusionaryNight:
                    case eEvents::EventBrandOfArgus:
                        events.ScheduleEvent(p_EventID, l_SecondItr->second.front());
                        break;
                    case eEvents::EventCarrionNightmare:
                    case eEvents::EventPhantasmalBloodfang:
                    case eEvents::EventFelswornSpellguard:
                    case eEvents::EventSightlessWatcher:
                        m_AddsEvent.ScheduleEvent(p_EventID, l_SecondItr->second.front());
                        break;
                    default:
                        break;
                }

                l_SecondItr->second.pop();
            }

            void DefaultEvents()
            {
                events.Reset();
                m_AddsEvent.Reset();

                InitTimers();
                AutoSchedule(eEvents::EventCarrionPlague);
                AutoSchedule(eEvents::EventSeekerSwarm);
                AutoSchedule(eEvents::EventFeastOfBlood);
                AutoSchedule(eEvents::EventEchoesOfTheVoid);
                AutoSchedule(eEvents::EventIllusionaryNight);
                AutoSchedule(eEvents::EventCarrionNightmare);
                AutoSchedule(eEvents::EventSightlessWatcher);
                AutoSchedule(eEvents::EventFelswornSpellguard);
                AutoSchedule(eEvents::EventPhantasmalBloodfang);
                AutoSchedule(eEvents::EventBrandOfArgus);
                m_AddsEvent.ScheduleEvent(eEvents::EventBerserk, eTimers::BerserkNormal);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                me->CastSpell(me, eSpells::VampiricAura, true);
                m_VampiricAuraBonus = 0;
                me->SetPhaseMaskWithSummons(3, true);
                m_Phase1Summons.clear();
                m_CarrionPlagueTargets = 2;
                for (Map::PlayerList::const_iterator itr = me->GetMap()->GetPlayers().begin(); itr != me->GetMap()->GetPlayers().end(); ++itr)
                {
                    Player* l_Player = itr->getSource();
                    if (!l_Player)
                        continue;

                    m_Players.insert(l_Player->GetGUID());
                }

                Talk(eTalks::TalkAggro);

                DefaultEvents();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);
                _JustDied();

                if (!me->GetMap() || me->GetMap()->IsLFR())
                    return;

                AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::NotForYou);
                if (!l_AchievementEntry)
                    return;

                for (uint64 l_GUID : m_Players)
                {
                    Player* l_Player = Player::GetPlayer(*me, l_GUID);
                    if (!l_Player)
                        continue;

                    l_Player->CompletedAchievement(l_AchievementEntry);
                }
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (!p_Victim->IsPlayer())
                    return;

                if (instance && instance->IsWipe())
                    Talk(eTalks::TalkWipe);
                else
                    Talk(eTalks::TalkKilledPlayer);
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eScriptData::VampiricAuraBonus:
                        m_VampiricAuraBonus = p_Value;
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::VampiricAuraBonus:
                        return m_VampiricAuraBonus;
                    case eScriptData::CarrionPlagueTargets:
                        return m_CarrionPlagueTargets ^= 1; ///< alternate betweeen 2 and 3
                    default:
                        break;
                }

                return 0;
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::Phase1Summon:
                    {
                        m_Phase1Summons.push_back(p_Guid);
                        break;
                    }
                    case eScriptData::PhantasmaticBloodfang:
                    {
                        Creature* l_PhantasmaticBloodfang = Creature::GetCreature(*me, p_Guid);
                        if (!l_PhantasmaticBloodfang)
                            break;

                        me->CastSpell(l_PhantasmaticBloodfang, eSpells::EssenceOfNight, true);
                        break;
                    }
                    case eScriptData::TargetNetherZone:
                    {
                        Unit* l_Target = Unit::GetUnit(*me, p_Guid);
                        if (!l_Target)
                            break;

                        me->CastSpell(l_Target, eSpells::NetherZone, true);
                        break;
                    }
                    case eScriptData::AchievementFailed:
                    {
                        m_Players.erase(p_Guid);
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
                    case eActions::TichondriusStartIllusionaryNight:
                    {
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->SetDisplayId(eDisplayIDs::Invisible);
                        me->SendClearTarget();
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAura(eSpells::VampiricAura);

                        for (std::list<uint64>::const_iterator l_Itr = m_Phase1Summons.begin(); l_Itr != m_Phase1Summons.end();)
                        {
                            Creature* l_Creature = Creature::GetCreature(*me, *l_Itr);
                            if (!l_Creature)
                            {
                                l_Itr = m_Phase1Summons.erase(l_Itr);
                                continue;
                            }

                            l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                            l_Creature->SetPhaseMaskWithSummons(3, true);
                            l_Creature->SendClearTarget();
                            l_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Creature->SetVisible(false);

                            ++l_Itr;
                        }

                        AddTimedDelayedOperation(30 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            me->RestoreDisplayId();
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            me->CastSpell(me, eSpells::VampiricAura, true);

                            for (std::list<uint64>::const_iterator l_Itr = m_Phase1Summons.begin(); l_Itr != m_Phase1Summons.end();)
                            {
                                Creature* l_Creature = Creature::GetCreature(*me, *l_Itr);
                                if (!l_Creature)
                                {
                                    l_Itr = m_Phase1Summons.erase(l_Itr);
                                    continue;
                                }

                                l_Creature->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                l_Creature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                                l_Creature->SetVisible(true);

                                ++l_Itr;
                            }

                            std::list<Creature*> l_PhantasmaticBloodfangs;
                            me->GetCreatureListWithEntryInGrid(l_PhantasmaticBloodfangs, eCreatures::NpcPhantasmaticBloodfang, 150.0f);
                            for (Creature* l_PhantasmaticBloodfang : l_PhantasmaticBloodfangs)
                                l_PhantasmaticBloodfang->DespawnOrUnsummon();

                            std::list<AreaTrigger*> l_EssencesOfNight;
                            me->GetAreaTriggerList(l_EssencesOfNight, eSpells::EssenceOfNight);
                            for (AreaTrigger* l_EssenceOfNight : l_EssencesOfNight)
                                l_EssenceOfNight->Despawn();

                            if (GetDifficulty() != Difficulty::DifficultyRaidMythic)
                                return;

                            for (Map::PlayerList::const_iterator itr = me->GetMap()->GetPlayers().begin(); itr != me->GetMap()->GetPlayers().end(); ++itr)
                            {
                                Player* l_Player = itr->getSource();
                                if (!l_Player)
                                    continue;

                                if (!l_Player->HasAura(eSpells::EssenceOfNightBuff))
                                    me->Kill(l_Player);
                            }
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

                if (me->GetExactDist(&l_PositionCheck1) > 32.0f && me->GetExactDist(&l_PositionCheck2) > 40.0f)
                    EnterEvadeMode();

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);
                m_AddsEvent.Update(p_Diff);

                switch (m_AddsEvent.ExecuteEvent())
                {
                    case eEvents::EventCarrionNightmare:
                    {
                        me->SummonCreature(eCreatures::NpcCarrionNightmare, m_CarrionNightmares[urand(0, m_CarrionNightmares.size() - 1)]);
                        AutoSchedule(eEvents::EventCarrionNightmare);
                        break;
                    }
                    case eEvents::EventPhantasmalBloodfang:
                    {
                        Position l_Pos;
                        m_RoomCenter.SimplePosXYRelocationByAngle(l_Pos, frand(0.0f, 25.0f), frand(0.0f, 2.0f * M_PI));
                        me->SummonCreature(eCreatures::NpcPhantasmaticBloodfang, l_Pos);
                        AutoSchedule(eEvents::EventPhantasmalBloodfang);
                        break;
                    }
                    case eEvents::EventFelswornSpellguard:
                    {
                        Talk(eTalks::TalkCallUnderlings);
                        for (Position const& l_Pos : m_FelswornSpellguards)
                        {
                            Creature* l_FelswornSpellguard = me->SummonCreature(eCreatures::FelswornSpellguard, l_Pos);
                            SetGUID(l_FelswornSpellguard->GetGUID(), eScriptData::Phase1Summon);
                        }
                        AutoSchedule(eEvents::EventFelswornSpellguard);
                        break;
                    }
                    case eEvents::EventSightlessWatcher:
                    {
                        Creature* l_SightlessWatcher = me->SummonCreature(eCreatures::SightlessWatcher, m_SightlessWatcher);
                        SetGUID(l_SightlessWatcher->GetGUID(), eScriptData::Phase1Summon);
                        AutoSchedule(eEvents::EventSightlessWatcher);
                        break;
                    }
                    case eEvents::EventBerserk:
                    {
                        DoCastAOE(eSharedSpells::SpellBerserk, true);
                        me->SetSpeed(MOVE_RUN, 4.0f);
                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCarrionPlague:
                    {
                        Talk(eTalks::WarningCarrionPlague);
                        Talk(eTalks::TalkCarrionPlague);
                        DoCastAOE(eSpells::CarrionPlague, false);
                        AutoSchedule(eEvents::EventCarrionPlague);
                        break;
                    }
                    case eEvents::EventSeekerSwarm:
                    {
                        DoCastAOE(eSpells::SeekerSwarm, false);
                        AutoSchedule(eEvents::EventSeekerSwarm);
                        break;
                    }
                    case eEvents::EventFeastOfBlood:
                    {
                        Talk(eTalks::TalkFeastOfBlood);
                        DoCastVictim(eSpells::FeastOfBlood, false);
                        AutoSchedule(eEvents::EventFeastOfBlood);
                        break;
                    }
                    case eEvents::EventEchoesOfTheVoid:
                    {
                        Talk(eTalks::TalkEchoesOfTheVoid);
                        DoCastAOE(eSpells::EchoesOfTheVoid, false);
                        AutoSchedule(eEvents::EventEchoesOfTheVoid);
                        break;
                    }
                    case eEvents::EventIllusionaryNight:
                    {
                        Talk(eTalks::TalkIllusionaryNight);
                        DoCastAOE(eSpells::IllusionaryNight, false);
                        AutoSchedule(eEvents::EventIllusionaryNight);
                        break;
                    }
                    case eEvents::EventBrandOfArgus:
                    {
                        DoCastAOE(eSpells::BrandOfArgus, false);
                        AutoSchedule(eEvents::EventBrandOfArgus);
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
            return new boss_tichondriusAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Vampiric Aura - 206893
class spell_tichondrius_vampiric_aura : public SpellScriptLoader
{
    public:
        spell_tichondrius_vampiric_aura() : SpellScriptLoader("spell_tichondrius_vampiric_aura") { }

        class spell_tichondrius_vampiric_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_vampiric_aura_AuraScript);

            enum eSpells
            {
                VampiricAura = 206894
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VampiricAura);
                Unit* l_Target = GetTarget();
                if (!l_DamageInfo || !l_Target)
                    return;

                Creature* l_Tichondrius = l_Target->FindNearestCreature(eCreatures::Tichondrius, 150.0f);
                if (!l_Tichondrius || !l_Tichondrius->IsAIEnabled)
                    return;

                uint32 l_Amount = l_DamageInfo->GetAmount();
                AddPct(l_Amount, l_Tichondrius->GetAI()->GetData(eScriptData::VampiricAuraBonus) + 600);

                l_Target->HealBySpell(l_Target, l_SpellInfo, l_Amount);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_tichondrius_vampiric_aura_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_vampiric_aura_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Vampiric Aura - 6297 - 9132
class at_tichondrius_vampiric_aura : public AreaTriggerEntityScript
{
    public:
        at_tichondrius_vampiric_aura() : AreaTriggerEntityScript("at_tichondrius_vampiric_aura") { }

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return true;

            switch (p_Target->GetEntry())
            {
                case eCreatures::FelSpire:
                case eCreatures::FelSpire2:
                    return true;
                default:
                    break;
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_tichondrius_vampiric_aura();
        }
};

/// Last Update 7.1.5 Build 23420
/// Carrion Plague (Filter) - 212997
class spell_tichondrius_carrion_plague : public SpellScriptLoader
{
    public:
        spell_tichondrius_carrion_plague() : SpellScriptLoader("spell_tichondrius_carrion_plague") { }

        class spell_tichondrius_carrion_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_carrion_plague_SpellScript);

            enum eSpells
            {
                CarrionPlague = 206480
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetMap() || !l_Caster->IsAIEnabled)
                    return;

                std::list<WorldObject*> l_EligibleTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    if (l_Target->IsPlayer() && ((l_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK) || l_Target->ToPlayer()->HasAura(eSpells::CarrionPlague)))
                        continue;

                    l_EligibleTargets.push_back(l_Target);
                }

                if (l_EligibleTargets.empty())
                    return;

                uint32 l_TargetAmount = 2;
                if (l_Caster->GetMap()->IsMythic())
                    l_TargetAmount = l_Caster->GetAI()->GetData(eScriptData::CarrionPlagueTargets);

                JadeCore::RandomResizeList(l_EligibleTargets, std::min<uint32>(l_EligibleTargets.size(), l_TargetAmount));
                p_Targets = l_EligibleTargets;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CarrionPlague, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_carrion_plague_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_carrion_plague_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_carrion_plague_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Seeker Swarm (Filter) - 213238
class spell_tichondrius_seeker_swarm : public SpellScriptLoader
{
    public:
        spell_tichondrius_seeker_swarm() : SpellScriptLoader("spell_tichondrius_seeker_swarm") { }

        class spell_tichondrius_seeker_swarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_seeker_swarm_SpellScript);

            enum eSpells
            {
                SeekerSwarm     = 206474,
                CarrionPlague   = 206480
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                std::list<WorldObject*> l_EligibleTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    if (!l_Target->IsPlayer() || !l_Target->ToPlayer()->HasAura(eSpells::CarrionPlague))
                        continue;

                    l_EligibleTargets.push_back(l_Target);
                }

                p_Targets = l_EligibleTargets;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SeekerSwarm, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_seeker_swarm_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_seeker_swarm_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_seeker_swarm_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Seeker Swarm (Damage) - 206474
class spell_tichondrius_seeker_swarm_damage : public SpellScriptLoader
{
    public:
        spell_tichondrius_seeker_swarm_damage() : SpellScriptLoader("spell_tichondrius_seeker_swarm_damage") { }

        class spell_tichondrius_seeker_swarm_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_seeker_swarm_damage_SpellScript);

            enum eSpells
            {
                SeekerSwarm     = 206474,
                CarrionPlague   = 206480
            };

            enum eOrphanVisual
            {
                SeekerSwarmVisual = 56358
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                WorldLocation l_AxeEnd;
                l_Caster->SimplePosXYRelocationByAngle(l_AxeEnd, 150.0f, l_Caster->GetAngle(l_Target), true);
                l_AxeEnd.m_mapId = l_Caster->GetMapId();

                std::list<Unit*> l_FinalTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    if (!l_Target->IsInAxe(l_Caster, &l_AxeEnd, 3.0f))
                        continue;

                    l_FinalTargets.push_back(l_UnitTarget);
                }

                if (l_Caster->GetMap() && l_Caster->GetMap()->IsMythic())
                {
                    l_FinalTargets.sort(JadeCore::DistanceOrderPred(l_Caster));
                    l_FinalTargets.resize(1);
                }

                p_Targets.clear();
                for (WorldObject* l_Target : l_FinalTargets)
                    p_Targets.push_back(l_Target);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                bool l_IsMythic = l_Caster->GetMap() && l_Caster->GetMap()->IsMythic();
                bool l_PlayOrphan = l_IsMythic;

                if (!l_Target->HasAura(eSpells::CarrionPlague))
                    l_Caster->CastSpell(l_Target, eSpells::CarrionPlague, true);
                else if (l_IsMythic)
                    SetHitDamage(GetHitDamage() * 11);
                else
                    l_PlayOrphan = true;

                if (!l_PlayOrphan)
                    return;

                l_Caster->PlayOrphanSpellVisual(l_Target->AsVector3(), { 0.0f, 0.0f,  l_Caster->GetAngle(l_Target) }, l_Caster->AsVector3(), eOrphanVisual::SeekerSwarmVisual);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_seeker_swarm_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST_2);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_seeker_swarm_damage_SpellScript::FilterTargets, EFFECT_1, TARGET_ENNEMIES_IN_CYLINDER);
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_seeker_swarm_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_seeker_swarm_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Feast Of Blood - 208230
class spell_tichondrius_feast_of_blood : public SpellScriptLoader
{
    public:
        spell_tichondrius_feast_of_blood() : SpellScriptLoader("spell_tichondrius_feast_of_blood") { }

        class spell_tichondrius_feast_of_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_feast_of_blood_AuraScript);

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Caster->IsAIEnabled || !l_SpellInfo)
                    return;

                l_Caster->GetAI()->SetData(eScriptData::VampiricAuraBonus, l_SpellInfo->Effects[EFFECT_1].BasePoints);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->GetAI()->SetData(eScriptData::VampiricAuraBonus, 0);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_tichondrius_feast_of_blood_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_tichondrius_feast_of_blood_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_feast_of_blood_AuraScript();
        }

        class spell_tichondrius_feast_of_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_feast_of_blood_SpellScript);

            enum eSpells
            {
                FeastOfBlood = 216421
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->IsAIEnabled)
                    return;

                float l_Angle = -M_PI / 2.0f;
                uint32 l_TaintedBloodCount = (l_Caster->GetMap() && l_Caster->GetMap()->IsMythic()) ? 4 : 3;

                for (uint32 l_I = 0; l_I < l_TaintedBloodCount; ++l_I)
                {
                    Creature* l_TaintedBlood = l_Caster->SummonCreature(eCreatures::TaintedBlood, *l_Caster);
                    if (!l_TaintedBlood)
                        continue;

                    l_Caster->GetAI()->SetGUID(l_TaintedBlood->GetGUID(), eScriptData::Phase1Summon);

                    Position l_JumpDest;
                    l_Caster->SimplePosXYRelocationByAngle(l_JumpDest, 10.0f, l_Angle);
                    l_TaintedBlood->CastSpell(l_JumpDest, eSpells::FeastOfBlood, true);
                    l_Angle += M_PI / static_cast<float>(l_TaintedBloodCount - 1);

                    if (!l_TaintedBlood->IsAIEnabled)
                        continue;

                    l_TaintedBlood->GetAI()->SetGUID(l_Target->GetGUID());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_feast_of_blood_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_feast_of_blood_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Tainted Blood - 108934
class npc_tichondrius_tainted_blood : public CreatureScript
{
    public:
        npc_tichondrius_tainted_blood() : CreatureScript("npc_tichondrius_tainted_blood") { }

        struct npc_tichondrius_tainted_bloodAI : public LegionCombatAI
        {
            npc_tichondrius_tainted_bloodAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            enum eSpells
            {
                FeastOfBlood = 216421
            };

            uint64 m_Target;
            bool m_JumpDone;

            void Reset() override
            {
                LegionCombatAI::Reset();
                m_JumpDone = false;
                m_Target = 0;
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                m_Target = p_Guid;
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case eSpells::FeastOfBlood:
                    {
                        m_JumpDone = true;
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_JumpDone || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                Unit* l_Target = Unit::GetUnit(*me, m_Target);
                if (!l_Target)
                {
                    l_Target = me->SelectVictim();
                    if (l_Target)
                        m_Target = l_Target->GetGUID();
                }

                if (!l_Target)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                AttackStart(l_Target, true);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tichondrius_tainted_bloodAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Echoes Of The Void (Periodic) - 213531
class spell_tichondrius_echoes_of_the_void : public SpellScriptLoader
{
    public:
        spell_tichondrius_echoes_of_the_void() : SpellScriptLoader("spell_tichondrius_echoes_of_the_void") { }

        class spell_tichondrius_echoes_of_the_void_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_echoes_of_the_void_SpellScript);

            enum eSpells
            {
                SummonFelSpire                  = 208369,
                KnockBack                       = 216295,
                SummonSpireProtector            = 215668,
            /// SummonSpireProtectorNPC         = 208413, triggered by SummonSpireProtector
            /// SummonSpireProtectorCollision   = 215669, triggered by SummonSpireProtector

            /// EchoesOfTheVoidPeriodicUNKAreatrigger       = 215875, UNUSED
            /// EchoesOfTheVoidUNKAreatrigger               = 213650, UNUSED
            /// EchoesOfTheVoidDummyAura30sec               = 215879, UNUSED
            };

            std::vector<Position> m_SpirePositions =
            {
                { 251.9511f, 3374.994f, 171.9067f, 5.9038110f },
                { 223.8920f, 3398.970f, 171.9067f, 1.4230640f },
                { 243.4036f, 3416.163f, 171.8952f, 0.9414529f },
                { 263.0068f, 3394.960f, 171.9067f, 0.1945787f }
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (Position const& l_Position : m_SpirePositions)
                {
                    l_Caster->CastSpell(l_Position, eSpells::SummonFelSpire, true);
                    l_Caster->CastSpell(l_Position, eSpells::KnockBack, true);
                }

                std::list<Creature*> l_FelSpires;
                l_Caster->GetCreatureListWithEntryInGrid(l_FelSpires, eCreatures::FelSpire, 150.0f);
                for (Creature* l_Creature : l_FelSpires)
                    l_Creature->CastSpell(l_Creature, eSpells::SummonSpireProtector, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_tichondrius_echoes_of_the_void_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_echoes_of_the_void_SpellScript();
        }

        class spell_tichondrius_echoes_of_the_void_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_echoes_of_the_void_AuraScript);

            enum eSpells
            {
                EchoesOfTheVoidDamage       = 213534,
                EchoesOfTheVoidCustomDamage = 215906
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetMap())
                    return;

                std::list<Creature*> l_FelSpires;
                l_Caster->GetCreatureListWithEntryInGrid(l_FelSpires, eCreatures::FelSpire2, 150.0f);

                for (Map::PlayerList::const_iterator itr = l_Caster->GetMap()->GetPlayers().begin(); itr != l_Caster->GetMap()->GetPlayers().end(); ++itr)
                {
                    if (Player* l_Player = itr->getSource())
                    {
                        Creature* l_Obstacle = nullptr;
                        for (Creature* l_Creature : l_FelSpires)
                        {
                            if (!l_Creature->IsInAxe(l_Caster, l_Player, 3.0f) || (l_Caster->GetExactDist(l_Player) < l_Caster->GetExactDist(l_Creature)))
                                continue;

                            l_Obstacle = l_Creature;
                            break;
                        }
                        if (l_Obstacle)
                        {
                            l_Caster->CastSpell(l_Obstacle, eSpells::EchoesOfTheVoidCustomDamage, true);
                            return;
                        }

                        l_Caster->CastSpell(l_Player, eSpells::EchoesOfTheVoidDamage, true);

                        if (!l_Caster->IsAIEnabled)
                            return;

                        l_Caster->GetAI()->SetGUID(l_Player->GetGUID(), eScriptData::AchievementFailed);
                    }
                }
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Creature*> l_FelSpires;
                l_Caster->GetCreatureListWithEntryInGrid(l_FelSpires, eCreatures::FelSpire, 150.0f);
                for (Creature* l_Creature : l_FelSpires)
                    l_Creature->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tichondrius_echoes_of_the_void_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tichondrius_echoes_of_the_void_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_echoes_of_the_void_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Echoes Of The Void (Periodic) - 215668
class spell_tichondrius_summon_fel_spire : public SpellScriptLoader
{
    public:
        spell_tichondrius_summon_fel_spire() : SpellScriptLoader("spell_tichondrius_summon_fel_spire") { }

        class spell_tichondrius_summon_fel_spire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_summon_fel_spire_AuraScript);

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Creature* l_FelSpire = l_Target->FindNearestCreature(eCreatures::FelSpire2, 5.0f))
                    l_FelSpire->DespawnOrUnsummon();

                if (GameObject* l_FelSpire = l_Target->FindNearestGameObject(eGameObjects::FelSpireGob, 5.0f))
                    l_FelSpire->Delete();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tichondrius_summon_fel_spire_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_LINKED_2, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_summon_fel_spire_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Spire - 108677
class npc_tichondrius_fel_spire_obstacle : public CreatureScript
{
    public:
        npc_tichondrius_fel_spire_obstacle() : CreatureScript("npc_tichondrius_fel_spire_obstacle") { }

        struct npc_tichondrius_fel_spire_obstacleAI : public LegionCombatAI
        {
            npc_tichondrius_fel_spire_obstacleAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                ApplyAllImmunities(true);
            }

            enum eSpells
            {
                EchoesOfTheVoidCustomDamage = 215906
            };

            void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                p_Damage = 0;

                if (!p_SpellInfo || p_SpellInfo->Id != eSpells::EchoesOfTheVoidCustomDamage)
                    return;

                Creature* m_Parent = me->FindNearestCreature(eCreatures::FelSpire, 5.0f);
                if (!m_Parent || !m_Parent->IsAIEnabled)
                    return;

                m_Parent->GetAI()->DoAction(ACTION_1);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tichondrius_fel_spire_obstacleAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Spire - 108625
class npc_tichondrius_fel_spire : public CreatureScript
{
    public:
        npc_tichondrius_fel_spire() : CreatureScript("npc_tichondrius_fel_spire") { }

        struct npc_tichondrius_fel_spireAI : public LegionCombatAI
        {
            npc_tichondrius_fel_spireAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                ApplyAllImmunities(true);
            }

            enum eSpells
            {
                SummonSpireProtector    = 215668,
                BreakingVisual          = 215918
            };

            int32 m_Health;

            void JustSummonedGO(GameObject* p_GameObject) override
            {
                if (p_GameObject->GetEntry() == eGameObjects::FelSpireGob)
                    p_GameObject->EnableCollision(false);
            }

            void DamageTaken(Unit* /*p_Source*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void Reset() override
            {
                LegionCombatAI::Reset();

                if (!me->GetMap())
                    return;

                uint32 l_HealthMulti = 0;
                switch (me->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidLFR:
                        l_HealthMulti = 10;
                        break;
                    case Difficulty::DifficultyRaidNormal:
                        l_HealthMulti = 8;
                        break;
                    case Difficulty::DifficultyRaidHeroic:
                        l_HealthMulti = 6;
                        break;
                    case Difficulty::DifficultyRaidMythic:
                        l_HealthMulti = 4;
                        break;
                    default:
                        break;
                }

                m_Health = me->GetMap()->GetPlayersCountExceptGMs() * l_HealthMulti;
            }

            void JustDespawned() override
            {
                me->RemoveAurasDueToSpell(eSpells::SummonSpireProtector);
            }

            void DoAction(int32 const p_Action) override
            {
                m_Health--;
                if (m_Health > 0 || !me->HasAura(eSpells::SummonSpireProtector))
                    return;

                me->RemoveAurasDueToSpell(eSpells::SummonSpireProtector);
                me->CastSpell(me, eSpells::BreakingVisual, true);

                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tichondrius_fel_spireAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Illusionary Night - 206365
class spell_tichondrius_illusionary_night : public SpellScriptLoader
{
    public:
        spell_tichondrius_illusionary_night() : SpellScriptLoader("spell_tichondrius_illusionary_night") { }

        class spell_tichondrius_illusionary_night_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_illusionary_night_SpellScript);

            enum eSpells
            {
                IllusionaryNightPhasing = 206311,
            /// IllusionaryNightRemove          = 206391, UNUSED
            /// IllusionaryNightUNKAreatrigger  = 206385, UNUSED
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->GetAI()->DoAction(eActions::TichondriusStartIllusionaryNight);

                l_Caster->CastSpell(l_Caster, eSpells::IllusionaryNightPhasing, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_tichondrius_illusionary_night_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_illusionary_night_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Illusionary Night (Phasing) - 206311
class spell_tichondrius_illusionary_night_phasing : public SpellScriptLoader
{
    public:
        spell_tichondrius_illusionary_night_phasing() : SpellScriptLoader("spell_tichondrius_illusionary_night_phasing") { }

        class spell_tichondrius_illusionary_night_phasing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_illusionary_night_phasing_AuraScript);

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Caster)
                    return;

                std::map<uint64, uint64>& l_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::TichondriusPhaseMask];
                l_Helper[l_Target->GetGUID()] = l_Target->GetPhaseMask();

                l_Target->SetPhaseMaskWithSummons(3, true);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Caster)
                    return;

                std::map<uint64, uint64>& l_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::TichondriusPhaseMask];
                if ((l_Helper.find(l_Target->GetGUID()) == l_Helper.end()) || (l_Helper[l_Target->GetGUID()] == 0))
                    l_Target->SetPhaseMaskWithSummons(1, true);
                else
                    l_Target->SetPhaseMaskWithSummons(l_Helper[l_Target->GetGUID()], true);

                l_Helper[l_Target->GetGUID()] = 0;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tichondrius_illusionary_night_phasing_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tichondrius_illusionary_night_phasing_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_illusionary_night_phasing_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Carrion Nightmare - 108739
class npc_tichondrius_carrion_nightmare : public CreatureScript
{
    public:
        npc_tichondrius_carrion_nightmare() : CreatureScript("npc_tichondrius_carrion_nightmare") { }

        struct npc_tichondrius_carrion_nightmareAI : public LegionCombatAI
        {
            npc_tichondrius_carrion_nightmareAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->SetPhaseMaskWithSummons(2, true);
            }

            enum eSpells
            {
                CarrionNightmareVisual  = 215973,
                CarrionNightmare        = 215988
            };

            void Reset() override
            {
                LegionCombatAI::Reset();

                AddTimedDelayedOperation(200, [this]() -> void
                {
                    me->CastSpell(me, eSpells::CarrionNightmareVisual, true);
                    Position l_Target;
                    me->SimplePosXYRelocationByAngle(l_Target, 200.0f, 0.0f);
                    me->CastSpell(l_Target, eSpells::CarrionNightmare, false);
                    me->DespawnOrUnsummon(2500);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tichondrius_carrion_nightmareAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Carrion Nightmare - 215988
class spell_tichondrius_carrion_nightmare : public SpellScriptLoader
{
    public:
        spell_tichondrius_carrion_nightmare() : SpellScriptLoader("spell_tichondrius_carrion_nightmare") { }

        class spell_tichondrius_carrion_nightmare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_carrion_nightmare_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                WorldLocation l_AxeEnd;
                l_Caster->SimplePosXYRelocationByAngle(l_AxeEnd, 150.0f, 0.0f);
                l_AxeEnd.m_mapId = l_Caster->GetMapId();

                std::list<WorldObject*> l_FinalTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    if (!l_Target->IsInAxe(l_Caster, &l_AxeEnd, 5.0f))
                        continue;

                    l_FinalTargets.push_back(l_Target);
                }

                p_Targets = l_FinalTargets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_carrion_nightmare_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_carrion_nightmare_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Essence of Night - 206466
class spell_tichondrius_essence_of_night : public SpellScriptLoader
{
    public:
        spell_tichondrius_essence_of_night() : SpellScriptLoader("spell_tichondrius_essence_of_night") { }

        class spell_tichondrius_essence_of_night_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_essence_of_night_AuraScript);

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Aura || l_Aura->GetDuration() > 0 || (l_Target->GetPhaseMask() & 2))
                    return;

                l_Aura->SetMaxDuration(30 * IN_MILLISECONDS);
                l_Aura->SetDuration(30 * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tichondrius_essence_of_night_AuraScript::HandleOnPeriodic, EFFECT_2, SPELL_AURA_OBS_MOD_POWER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_essence_of_night_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Phantasmatic Bloodfang - 104326
class npc_tichondrius_phantasmatic_bloodfang : public CreatureScript
{
    public:
        npc_tichondrius_phantasmatic_bloodfang() : CreatureScript("npc_tichondrius_phantasmatic_bloodfang") { }

        struct npc_tichondrius_phantasmatic_bloodfangAI : public LegionCombatAI
        {
            npc_tichondrius_phantasmatic_bloodfangAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetPhaseMaskWithSummons(2, true);
            }

            enum eEvents
            {
                EssenceOfNight = 1
            };

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EssenceOfNight:
                    {
                        Creature* l_Tichondrius = me->FindNearestCreature(eCreatures::Tichondrius, 150.0f);
                        if (!l_Tichondrius || !l_Tichondrius->IsAIEnabled)
                            return false;

                        l_Tichondrius->GetAI()->SetGUID(me->GetGUID(), eScriptData::PhantasmaticBloodfang);

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tichondrius_phantasmatic_bloodfangAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Felsworn Spellguard - 108591
class npc_tichondrius_felsworn_spellguard : public CreatureScript
{
    public:
        npc_tichondrius_felsworn_spellguard() : CreatureScript("npc_tichondrius_felsworn_spellguard") { }

        struct npc_tichondrius_felsworn_spellguardAI : public LegionCombatAI
        {
            npc_tichondrius_felsworn_spellguardAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            enum eEvents
            {
                EventVolatileWound = 2
            };

            void Reset() override
            {
                LegionCombatAI::Reset();

                Unit* l_Tichondrius = me->FindNearestCreature(eCreatures::Tichondrius, 150.0f);
                if (!l_Tichondrius)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (me->GetDistance(l_Tichondrius) < 10.0f)
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    return;
                }

                me->SetWalk(false);
                me->GetMotionMaster()->MoveCharge(l_Tichondrius, 10.0f);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case EventId::EVENT_CHARGE:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        break;
                    }
                    default:
                        break;
                }
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventVolatileWound:
                    {
                        if (urand(0, 3))
                            return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);

                        float l_Range = 10.0f;
                        std::list<Player*> l_Players;
                        JadeCore::AnyPlayerInObjectRangeCheck check(me, l_Range);
                        JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> searcher(me, l_Players, check);
                        me->VisitNearbyObject(l_Range, searcher);

                        if (l_Players.empty())
                            return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);

                        JadeCore::RandomResizeList(l_Players, 1);
                        return LegionCombatAI::ExecuteEvent(p_EventID, l_Players.front());
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tichondrius_felsworn_spellguardAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Volatile Wound - 216024
class spell_tichondrius_volatile_wound : public SpellScriptLoader
{
    public:
        spell_tichondrius_volatile_wound() : SpellScriptLoader("spell_tichondrius_volatile_wound") { }

        class spell_tichondrius_volatile_wound_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_volatile_wound_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Creature* l_Tichondrius = l_Target->FindNearestCreature(eCreatures::Tichondrius, 150.0f);
                if (!l_Tichondrius || !l_Tichondrius->IsAIEnabled)
                    return;

                l_Tichondrius->GetAI()->SetGUID(l_Target->GetGUID(), eScriptData::TargetNetherZone);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tichondrius_volatile_wound_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_volatile_wound_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Soul - 216040
class spell_tichondrius_burning_soul : public SpellScriptLoader
{
    public:
        spell_tichondrius_burning_soul() : SpellScriptLoader("spell_tichondrius_burning_soul") { }

        class spell_tichondrius_burning_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_burning_soul_SpellScript);

            uint64 l_SelectedTarget = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (l_SelectedTarget)
                {
                    p_Targets.clear();
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Unit* l_Target = Unit::GetUnit(*l_Caster, l_SelectedTarget);
                    if (!l_Target)
                        return;

                    p_Targets.push_back(l_Target);
                    return;
                }

                std::list<WorldObject*> l_EligibleTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Player* l_Player = l_Target->ToPlayer();
                    if (!l_Player || l_Player->GetRoleForGroup() != Roles::ROLE_HEALER)
                        continue;

                    l_EligibleTargets.push_back(l_Target);
                }

                if (l_EligibleTargets.empty())
                {
                    p_Targets.clear();
                    return;
                }

                JadeCore::RandomResizeList(l_EligibleTargets, 1);
                p_Targets = l_EligibleTargets;

                l_SelectedTarget = p_Targets.front()->GetGUID();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_burning_soul_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_burning_soul_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_burning_soul_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_burning_soul_SpellScript();
        }

        class spell_tichondrius_volatile_wound_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tichondrius_volatile_wound_AuraScript);

            enum eSpells
            {
                BurningSoul = 216041
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::BurningSoul, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tichondrius_volatile_wound_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tichondrius_volatile_wound_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ring of Shadows - 216723
class spell_tichondrius_ring_of_shadows : public SpellScriptLoader
{
    public:
        spell_tichondrius_ring_of_shadows() : SpellScriptLoader("spell_tichondrius_ring_of_shadows") { }

        class spell_tichondrius_ring_of_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_ring_of_shadows_SpellScript);

            enum eSpells
            {
                RingOfShadowVisual = 216727
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RingOfShadowVisual, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_tichondrius_ring_of_shadows_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_ring_of_shadows_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ring Of Shadows - 7491
class at_tichondrius_ring_of_shadows : public AreaTriggerEntityScript
{
    public:
        at_tichondrius_ring_of_shadows() : AreaTriggerEntityScript("at_tichondrius_ring_of_shadows") { }

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return true;

            if (p_Target->GetExactDist(l_Caster) < 10.0f)
                return true;

            return false;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            std::set<uint64>* l_Targets = p_AreaTrigger->GetAffectedPlayers();
            for (std::set<uint64>::const_iterator l_Itr = (*l_Targets).begin(); l_Itr != (*l_Targets).end();)
            {
                Unit* l_Target = Unit::GetUnit(*p_AreaTrigger, *l_Itr);
                if (l_Target && l_Target->GetExactDist(p_AreaTrigger) > 10.0f)
                {
                    ++l_Itr;
                    continue;
                }

                l_Itr = l_Targets->erase(l_Itr);
                p_AreaTrigger->AffectUnit(l_Target, AT_ACTION_MOMENT_LEAVE);
                if (l_Targets->empty())
                    p_AreaTrigger->AffectUnit(l_Target, AT_ACTION_MOMENT_LEAVE_ALL);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_tichondrius_ring_of_shadows();
        }
};

/// Last Update 7.1.5 Build 23420
/// Brand of Argus - 212794
class spell_tichondrius_brand_of_argus : public SpellScriptLoader
{
    public:
        spell_tichondrius_brand_of_argus() : SpellScriptLoader("spell_tichondrius_brand_of_argus") { }

        class spell_tichondrius_brand_of_argus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_brand_of_argus_SpellScript);

            enum eSpells
            {
                CarrionPlague       = 206480,
                BrandOfArgusVisual  = 215413
            /// BrandOfArgusCheck   = 213740 triggered by Periodic of 212794
            };

            std::pair<uint64, uint64> l_SelectedTarget = { 0, 0 };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (l_SelectedTarget.first)
                {
                    p_Targets.clear();
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Unit* l_FirstTarget = Unit::GetUnit(*l_Caster, l_SelectedTarget.first);
                    Unit* l_SecondTarget = Unit::GetUnit(*l_Caster, l_SelectedTarget.second);

                    if (l_FirstTarget)
                        p_Targets.push_back(l_FirstTarget);
                    if (l_SecondTarget)
                        p_Targets.push_back(l_SecondTarget);

                    return;
                }

                if (p_Targets.empty())
                    return;

                std::list<WorldObject*> l_EligibleTargets;
                for (std::list<WorldObject*>::const_iterator l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    Player* l_Player = (*l_Itr)->ToPlayer();
                    if (l_Player && l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                    {
                        l_Itr = p_Targets.erase(l_Itr);
                        continue;
                    }

                    if (!l_Player || l_Player->HasAura(eSpells::CarrionPlague))
                    {
                        ++l_Itr;
                        continue;
                    }

                    l_EligibleTargets.push_back(*l_Itr);
                    l_Itr = p_Targets.erase(l_Itr);
                }

                std::list<WorldObject*> l_RangedEligibleTargets;
                for (std::list<WorldObject*>::const_iterator l_Itr = l_EligibleTargets.begin(); l_Itr != l_EligibleTargets.end();)
                {
                    if ((*l_Itr)->ToPlayer() && (*l_Itr)->ToPlayer()->IsRangedDamageDealer(true))
                    {
                        l_RangedEligibleTargets.push_back(*l_Itr);
                        l_Itr = l_EligibleTargets.erase(l_Itr);
                        continue;
                    }

                    ++l_Itr;
                }

                while (l_RangedEligibleTargets.size() < 2 && l_EligibleTargets.size())
                {
                    auto l_Itr = l_EligibleTargets.begin();
                    std::advance(l_Itr, urand(0, l_EligibleTargets.size() - 1));
                    l_RangedEligibleTargets.push_back(*l_Itr);
                    l_EligibleTargets.erase(l_Itr);
                }

                l_EligibleTargets = l_RangedEligibleTargets;

                if (l_EligibleTargets.size() < 2 && !p_Targets.empty())
                {
                    JadeCore::RandomResizeList(p_Targets, std::min<uint32>(p_Targets.size(), 2 - l_EligibleTargets.size()));

                    auto l_Itr = p_Targets.begin();
                    uint32 l_MissingTargets = 2 - l_EligibleTargets.size();
                    for (int32 l_I = 0; l_I < l_MissingTargets, l_Itr != p_Targets.end(); ++l_I, ++l_Itr)
                        l_EligibleTargets.push_back(*l_Itr);
                }

                if (l_EligibleTargets.empty())
                {
                    p_Targets.clear();
                    return;
                }

                if (l_EligibleTargets.size() > 2)
                    JadeCore::RandomResizeList(l_EligibleTargets, 2);

                p_Targets = l_EligibleTargets;

                l_SelectedTarget.first = p_Targets.front()->GetGUID();

                if (l_EligibleTargets.size() > 1)
                    l_SelectedTarget.second = p_Targets.front()->GetGUID();
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::BrandOfArgusVisual, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_brand_of_argus_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_brand_of_argus_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_brand_of_argus_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_brand_of_argus_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Brand of Argus (Check) - 213740
class spell_tichondrius_brand_of_argus_check : public SpellScriptLoader
{
    public:
        spell_tichondrius_brand_of_argus_check() : SpellScriptLoader("spell_tichondrius_brand_of_argus_check") { }

        class spell_tichondrius_brand_of_argus_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_brand_of_argus_check_SpellScript);

            enum eSpells
            {
                BrandOfArgus            = 212794,
                BrandOfArgusDamages     = 212795,
                BrandOfArgusExplosion   = 220109,
                BrandOfArgusVisual      = 215413,
                FlamesOfArgus           = 216465
            };

            bool m_Explode = false;

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() < 4)
                    return;

                m_Explode = true;
            }

            void HandleAfterCast()
            {
                if (!m_Explode)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_Tichondrius = l_Caster->FindNearestCreature(eCreatures::Tichondrius, 150.0f);
                if (!l_Tichondrius)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::BrandOfArgusExplosion, true);
                l_Tichondrius->CastSpell(l_Caster, eSpells::BrandOfArgusDamages, true);
                l_Caster->RemoveAurasDueToSpell(eSpells::BrandOfArgus);
                l_Caster->RemoveAurasDueToSpell(eSpells::BrandOfArgusVisual);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Target->GetMap() || !l_Target->GetMap()->IsMythic())
                    return;

                Creature* l_Tichondrius = l_Target->FindNearestCreature(eCreatures::Tichondrius, 150.0f);
                if (!l_Tichondrius)
                    return;

                l_Tichondrius->CastSpell(l_Target, eSpells::FlamesOfArgus, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tichondrius_brand_of_argus_check_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                AfterCast += SpellCastFn(spell_tichondrius_brand_of_argus_check_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_brand_of_argus_check_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_brand_of_argus_check_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Brand of Argus (Damage) - 212795
class spell_tichondrius_brand_of_argus_damage : public SpellScriptLoader
{
    public:
        spell_tichondrius_brand_of_argus_damage() : SpellScriptLoader("spell_tichondrius_brand_of_argus_damage") { }

        class spell_tichondrius_brand_of_argus_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tichondrius_brand_of_argus_damage_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                WorldLocation const* l_Dest = GetExplTargetDest();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Dest || !l_Target || !l_SpellInfo)
                    return;

                SpellRadiusEntry const* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                float l_Pct = std::max(0.25f * l_RadiusEntry->RadiusMax, l_RadiusEntry->RadiusMax - l_Dest->GetExactDist(l_Target)) / l_RadiusEntry->RadiusMax;
                SetHitDamage(static_cast<float>(GetHitDamage()) * l_Pct);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tichondrius_brand_of_argus_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tichondrius_brand_of_argus_damage_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_tichondrius()
{
    /// Boss
    new boss_tichondrius();

    /// Spells
    new spell_tichondrius_vampiric_aura();
    new spell_tichondrius_carrion_plague();
    new spell_tichondrius_seeker_swarm();
    new spell_tichondrius_seeker_swarm_damage();
    new spell_tichondrius_feast_of_blood();
    new spell_tichondrius_echoes_of_the_void();
    new spell_tichondrius_summon_fel_spire();
    new spell_tichondrius_illusionary_night();
    new spell_tichondrius_illusionary_night_phasing();
    new spell_tichondrius_carrion_nightmare();
    new spell_tichondrius_essence_of_night();
    new spell_tichondrius_volatile_wound();
    new spell_tichondrius_burning_soul();
    new spell_tichondrius_ring_of_shadows();
    new spell_tichondrius_brand_of_argus();
    new spell_tichondrius_brand_of_argus_check();
    new spell_tichondrius_brand_of_argus_damage();

    /// NPC
    new npc_tichondrius_tainted_blood();
    new npc_tichondrius_fel_spire_obstacle();
    new npc_tichondrius_fel_spire();
    new npc_tichondrius_carrion_nightmare();
    new npc_tichondrius_phantasmatic_bloodfang();
    new npc_tichondrius_felsworn_spellguard();

    /// Areatrigger
    new at_tichondrius_vampiric_aura();
    new at_tichondrius_ring_of_shadows();
}
#endif
