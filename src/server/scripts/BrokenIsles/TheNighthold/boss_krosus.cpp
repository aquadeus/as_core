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
    Side,
    DataSlam,
    DataAchievement,
    Trash
};

/// Last Update 7.1.5 Build 23420
/// Krosus - 101002
class boss_krosus : public CreatureScript
{
    public:
        boss_krosus() : CreatureScript("boss_krosus") { }

        struct boss_krosusAI : BossAI
        {
            boss_krosusAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataKrosus)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                m_CheckPlayersTimer = 500;

                me->SetReactState(ReactStates::REACT_PASSIVE);
                m_TrashSetInit = false;

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_Trashes;

                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcFelweaverPharamere, 40.0f);
                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcSummonerXiv, 40.0f);
                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcChaosMageBeleron, 40.0f);

                    for (Creature*& l_Itr : l_Trashes)
                    {
                        if (l_Itr == nullptr || l_Itr->isDead() || l_Itr->IsDuringRemoveFromWorld())
                            continue;

                        m_TrashSet.insert(l_Itr->GetGUID());
                    }

                    if (!m_TrashSet.empty())
                    {
                        m_TrashSetInit = true;
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        me->SetVisible(false);
                    }
                    else
                    {
                        m_TrashSetInit = false;
                        me->SetReactState(ReactStates::REACT_DEFENSIVE);
                        me->SetVisible(true);
                    }
                });
            }

            bool m_TrashSetInit;
            std::set<uint64> m_TrashSet;

            enum eTalks
            {
                TalkAggro,
                TalkOrbOfDestruction,
                TalkSlam,
                TalkKillPlayer,
                TalkWipe,
                TalkDeath,
                WarningSlam,
            };

            enum eSpells
            {
                Slam                = 205862,
                BurningPitch        = 205420,
                OrbOfDestruction    = 205343,
                FelBeam             = 205383,
                SearingBrand        = 206677,
                IsolatedRage        = 208201,
                Accelerate          = 209006,
                Emerge              = 208495,
                Submerge            = 224051,
            };

            enum eEvents
            {
                EventSlam = 1,
                EventBurningPitch,
                EventOrbOfDestruction,
                EventFelBeam,
                EventSearingBrand,
                EventAccelerate
            };

            enum eDisplayIDs
            {
                Bridge1Damaged  = 32142, //< find real id
                Bridge1Broken   = 32142, //< find real id
                Bridge2Damaged  = 32500,
                Bridge2Broken   = 35270,
                Bridge3Damaged  = 32499,
                Bridge3Broken   = 35269,
                Bridge4Damaged  = 32498,
                Bridge4Broken   = 32471
            };

            enum eIndexes
            {
                GobID,
                DisplayDamaged,
                DisplayBroken
            };

            enum eAchievements
            {
                BurningBridges = 10575
            };

            Position const m_BridgePosition = { -34.563541f, 2830.011963f, -80.367294f, 3.653727f };

            std::array<std::vector<uint32>, 4> m_BridgeIDs =
            {{
                { eGameObjects::GoKrosusBridge1, eDisplayIDs::Bridge1Damaged, eDisplayIDs::Bridge1Broken},
                { eGameObjects::GoKrosusBridge2, eDisplayIDs::Bridge2Damaged, eDisplayIDs::Bridge2Broken},
                { eGameObjects::GoKrosusBridge3, eDisplayIDs::Bridge3Damaged, eDisplayIDs::Bridge3Broken},
                { eGameObjects::GoKrosusBridge4, eDisplayIDs::Bridge4Damaged, eDisplayIDs::Bridge4Broken},
            }};

            std::array<Position, 3> m_KrosusPositions =
            {{
                { -47.2274f, 2822.84f, 3.65958f, 0.5137f },
                { -71.5191f, 2809.25f, 3.65958f, 0.5137f },
                { -96.5191f, 2794.32f, 3.65958f, 0.5137f }
            }};

            std::map<Difficulty, std::map<uint32, std::vector<uint32>>> m_TimerData =
            {
                {
                    Difficulty::DifficultyRaidLFR,
                    {
                        { eEvents::EventFelBeam,{ 7000, 15000, 30000, 30000, 23000, 27000, 30000, 44000, 14000, 16000, 14000, 16000, 22000, 60000 } },
                        { eEvents::EventOrbOfDestruction,{ 70000, 40000, 60000, 25000, 60000, 37000, 15000, 15000, 30000 } },
                        { eEvents::EventBurningPitch,{ 38000, 102000, 85000, 90000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidNormal,
                    {
                        {eEvents::EventFelBeam, { 7000, 15000, 30000, 30000, 23000, 27000, 30000, 44000, 14000, 16000, 14000, 16000, 22000, 60000 } },
                        {eEvents::EventOrbOfDestruction, { 70000, 40000, 60000, 25000, 60000, 37000, 15000, 15000, 30000 } },
                        {eEvents::EventBurningPitch, { 38000, 102000, 85000, 90000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidHeroic,
                    {
                        { eEvents::EventFelBeam, { 9000, 29000, 30000, 42000, 16000, 16000, 14000, 16000, 27000, 54000, 26000, 5000, 5000, 16000, 5000, 12000, 12000, 5000, 13000 } },
                        { eEvents::EventOrbOfDestruction, { 19900, 60000, 23000, 62000, 27000, 25000, 15000, 15400, 14600, 30000, 55000 } },
                        { eEvents::EventBurningPitch, { 49800, 85000, 90000, 94000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidMythic,
                    {
                        { eEvents::EventFelBeam, { 8000, 16000, 16000, 16000, 14000, 16000, 27000, 55000, 26000, 4700, 21300, 4700, 12200, 12000, 4700, 13200, 19000, 4700, 25200, 4700, 25200, 4700 } },
                        { eEvents::EventOrbOfDestruction, { 13000, 62000, 27000, 25000, 14900, 15000, 15000, 30000, 55100, 38000, 30000, 12000, 18000 } },
                        { eEvents::EventBurningPitch, { 45000, 90000, 93900, 78000 } }
                    }
                }
            };

            std::map<Difficulty, std::map<uint32, std::queue<uint32>>> m_Timers;

            std::stack<uint64> m_BridgeGUIDs;

            uint8 m_SlamCounter;
            uint32 m_Side;

            uint32 m_BurningEmbersQuenched;
            int32 m_CheckPlayersTimer;

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

            bool CanBeHitInTheBack() override
            {
                return false;
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::Trash:
                    {
                        m_TrashSet.erase(p_Guid);

                        if (m_TrashSet.empty())
                        {
                            me->SetVisible(true);
                            me->CastSpell(me, eSpells::Emerge, true);
                            AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                            {
                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                                m_TrashSetInit = false;
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void Reset() override
            {
                _Reset();

                if (m_TrashSet.empty())
                {
                    m_TrashSetInit = false;
                    me->SetVisible(true);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(ReactStates::REACT_DEFENSIVE);
                }

                while (!m_BridgeGUIDs.empty())
                {
                    uint64 l_BridgeGUID = m_BridgeGUIDs.top();
                    m_BridgeGUIDs.pop();

                    GameObject* l_Bridge = GameObject::GetGameObject(*me, l_BridgeGUID);
                    if (!l_Bridge)
                        continue;

                    l_Bridge->Delete();
                }

                for (std::vector<uint32> const& l_IDs : m_BridgeIDs)
                {
                    GameObject* l_Bridge = me->SummonGameObject(l_IDs[eIndexes::GobID], m_BridgePosition);
                    if (!l_Bridge)
                        continue;

                    m_BridgeGUIDs.push(l_Bridge->GetGUID());
                }

                me->NearTeleportTo(me->GetHomePosition());

                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetHover(false);

                m_SlamCounter = 0;
                m_Side = 0;
                m_BurningEmbersQuenched = 0;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eScriptData::Side:
                        m_Side = p_Value;
                        break;
                    case eScriptData::DataAchievement:
                        m_BurningEmbersQuenched++;
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::Side:
                        return m_Side;
                    case eScriptData::DataSlam:
                        return m_SlamCounter == 2;
                    default:
                        break;
                }

                return 0;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::TalkAggro);
                _EnterCombat();
                DefaultEvents();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->ClearLootContainers();
                Talk(eTalks::TalkDeath);
                _JustDied();

                if (m_BurningEmbersQuenched >= 15 && instance && !IsLFR())
                    instance->DoCompleteAchievement(eAchievements::BurningBridges);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (!p_Victim->IsPlayer())
                    return;

                if (instance && instance->IsWipe())
                    Talk(eTalks::TalkWipe);
                else
                    Talk(eTalks::TalkKillPlayer);

            }

            void DefaultEvents()
            {
                events.Reset();

                events.ScheduleEvent(eEvents::EventSlam, 30 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSearingBrand, 2500);

                InitTimers();
                AutoSchedule(eEvents::EventFelBeam);
                AutoSchedule(eEvents::EventOrbOfDestruction);
                AutoSchedule(eEvents::EventBurningPitch);

                if (!me->GetMap())
                    return;

                switch (me->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidMythic:
                        events.ScheduleEvent(eEvents::EventAccelerate, 13 * IN_MILLISECONDS);
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::Slam:
                    {
                        GameObject* l_Gob = GameObject::GetGameObject(*me, m_BridgeGUIDs.top());
                        if (!l_Gob)
                            break;

                        m_SlamCounter++;
                        switch (m_SlamCounter)
                        {
                            case 2:
                            {
                                l_Gob->SetDisplayId(m_BridgeIDs[m_BridgeGUIDs.size() - 1][eIndexes::DisplayDamaged]);
                                l_Gob->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DAMAGED);
                                break;
                            }
                            case 3:
                            {
                                m_SlamCounter = 0;

                                l_Gob->SetDisplayId(m_BridgeIDs[m_BridgeGUIDs.size() - 1][eIndexes::DisplayBroken]);
                                l_Gob->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DAMAGED);
                                l_Gob->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DESTROYED);

                                uint64 l_BridgeGUID = m_BridgeGUIDs.top();
                                m_BridgeGUIDs.pop();

                                GameObject* l_Bridge = GameObject::GetGameObject(*me, l_BridgeGUID);
                                if (l_Bridge)
                                    l_Bridge->Delete();

                                if (m_BridgeGUIDs.empty())
                                {
                                    EnterEvadeMode();
                                    return;
                                }

                                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                                me->SetHover(true);

                                me->GetMotionMaster()->Clear();
                                SetCombatMovement(true);
                                me->GetMotionMaster()->MovePoint(10, m_KrosusPositions[m_BridgeGUIDs.size() - 1]);

                                Position l_TargetPos = m_KrosusPositions[m_BridgeGUIDs.size() - 1];

                                AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this, l_TargetPos]() -> void
                                {
                                    if (me->isInCombat())
                                    {
                                        me->NearTeleportTo(l_TargetPos);
                                        SetCombatMovement(false);
                                        me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                                    }
                                });

                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 10:
                    {
                        me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                        SetCombatMovement(false);
                        me->SetHover(false);
                        break;
                    }
                    default:
                        break;
                }
            }

            void CheckPlayers()
            {
                if (instance && instance->instance)
                {
                    for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->isGameMaster())
                                continue;

                            if (l_Player->GetPositionZ() < 1.0f && me->GetExactDist(l_Player) < 200.0f)
                                l_Player->Kill(l_Player);
                        }
                    }
                }

                if (me->getVictim() && !me->IsWithinMeleeRange(me->getVictim()) && me->HasUnitState(UNIT_STATE_ROOT))
                    DoCastAOE(eSpells::IsolatedRage, false);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if ((m_CheckPlayersTimer -= p_Diff) < 0)
                {
                    m_CheckPlayersTimer = 500;
                    CheckPlayers();
                }

                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                {
                    do
                    {
                        if (Spell const* l_CurrentChanneledSpell = me->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL))
                        {
                            if (SpellInfo const* l_SpellInfo = l_CurrentChanneledSpell->GetSpellInfo())
                            {
                                if (l_SpellInfo->Id == eSpells::IsolatedRage)
                                    break;
                            }
                        }
                        return;
                    } while (false);
                }

                if (me->IsWithinMeleeRange(me->getVictim()))
                    me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);

                uint32 l_Event = events.ExecuteEvent();
                if (!l_Event)
                    return;

                me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);

                switch (l_Event)
                {
                    case eEvents::EventSlam:
                    {
                        Talk(eTalks::TalkSlam);
                        Talk(eTalks::WarningSlam);
                        DoCastAOE(eSpells::Slam, false);
                        events.ScheduleEvent(eEvents::EventSlam, 30 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBurningPitch:
                    {
                        DoCastAOE(eSpells::BurningPitch, false);
                        AutoSchedule(eEvents::EventBurningPitch);
                        break;
                    }
                    case eEvents::EventOrbOfDestruction:
                    {
                        Talk(eTalks::TalkOrbOfDestruction);
                        DoCastAOE(eSpells::OrbOfDestruction, false);
                        AutoSchedule(eEvents::EventOrbOfDestruction);
                        break;
                    }
                    case eEvents::EventFelBeam:
                    {
                        DoCastAOE(eSpells::FelBeam, false);
                        AutoSchedule(eEvents::EventFelBeam);
                        break;
                    }
                    case eEvents::EventSearingBrand:
                    {
                        DoCastVictim(eSpells::SearingBrand, false);
                        events.ScheduleEvent(eEvents::EventSearingBrand, 2500);
                        break;
                    }
                    case eEvents::EventAccelerate:
                    {
                        events.ScheduleEvent(eEvents::EventAccelerate, 10 * IN_MILLISECONDS);
                        std::list<Creature*> l_Embers;
                        me->GetCreatureListWithEntryInGrid(l_Embers, eCreatures::BurningEmber, 200.0f);
                        if (l_Embers.empty())
                            break;

                        JadeCore::RandomResizeList(l_Embers, 1);
                        l_Embers.front()->CastSpell(l_Embers.front(), eSpells::Accelerate, true);
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
            return new boss_krosusAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Slam - 205862
class spell_krosus_slam : public SpellScriptLoader
{
    public:
        spell_krosus_slam() : SpellScriptLoader("spell_krosus_slam") { }

        class spell_krosus_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_slam_SpellScript);

            enum eSpells
            {
            /// SlamDamageTank          = 205863, Used in areatrigger_actions
                SlamDamageRaid          = 205875,
                SlamDamageRaidReduced   = 225362,
                SlamRightHandAura       = 205879,
                SlamLeftHandAura        = 205882,
                SlamRightHandAT         = 205881,
                SlamLeftHandAT          = 205880
            };

            void HandleOnPrepare()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Caster = GetCaster();
                if (!l_Spell || !l_Caster)
                    return;

                if (!l_Caster->GetMap())
                    return;

                switch (l_Caster->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidHeroic:
                        l_Spell->SetCastTime(3 * IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }

                Position l_Pos;
                l_Caster->SimplePosXYRelocationByAngle(l_Pos, 21.0f, -0.36f);
                l_Caster->CastSpell(l_Pos, eSpells::SlamRightHandAT, true);

                l_Caster->SimplePosXYRelocationByAngle(l_Pos, 23.0f, 0.31f);
                l_Caster->CastSpell(l_Pos, eSpells::SlamLeftHandAT, true);
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster ||!l_Caster->IsAIEnabled)
                    return;

                if ((!l_Caster->HasAura(eSpells::SlamLeftHandAura) || !l_Caster->HasAura(eSpells::SlamRightHandAura)) && !(l_Caster->GetAI()->GetData(eScriptData::DataSlam)))
                    l_Caster->CastSpell(l_Caster, eSpells::SlamDamageRaid, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::SlamDamageRaidReduced, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_krosus_slam_SpellScript::HandleOnPrepare);
                OnCast += SpellCastFn(spell_krosus_slam_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_slam_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Slam (Right Hand) - 6146 (205881)
class at_krosus_slam_right_hand : public AreaTriggerEntityScript
{
    public:
        at_krosus_slam_right_hand() : AreaTriggerEntityScript("at_krosus_slam_right_hand") { }

        enum eSpells
        {
            SlamRightHandAura       = 205879
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return false;

            l_Caster->CastSpell(p_Target, eSpells::SlamRightHandAura);

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
                l_Caster->RemoveAura(eSpells::SlamRightHandAura);

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_krosus_slam_right_hand();
        }
};

/// Last Update 7.1.5 Build 23420
/// Slam (Left Hand) - 6145 (205880)
class at_krosus_slam_left_hand : public AreaTriggerEntityScript
{
    public:
        at_krosus_slam_left_hand() : AreaTriggerEntityScript("at_krosus_slam_left_hand") { }

        enum eSpells
        {
            SlamLeftHandAura        = 205882
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return false;

            l_Caster->CastSpell(p_Target, eSpells::SlamLeftHandAura);

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
                l_Caster->RemoveAura(eSpells::SlamLeftHandAura);

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_krosus_slam_left_hand();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Pitch - 205420
class spell_krosus_burning_pitch : public SpellScriptLoader
{
    public:
        spell_krosus_burning_pitch() : SpellScriptLoader("spell_krosus_burning_pitch") { }

        class spell_krosus_burning_pitch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_burning_pitch_SpellScript);

            enum eSpells
            {
            /// BurningPitchDamageRadius    = 206280, triggered by missiles
                BurningPitchMissileLeft     = 206281,
                BurningPitchMissileRight    = 206307
            };

            Position const m_BridgeEnd = { -17.06f, 2840.40f, 3.16f };
            std::vector<Position> m_MissilesDest;
            uint8 m_MissileCount;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_MissilesSpellInfo = sSpellMgr->GetSpellInfo(eSpells::BurningPitchMissileLeft);
                if (!l_Caster || !l_SpellInfo || !l_MissilesSpellInfo || !l_MissilesSpellInfo->Speed)
                    return;

                SpellCastTimesEntry const* l_CastTimeEntry = l_SpellInfo->CastTimeEntry;
                InstanceScript* l_InstanceScript = l_Caster->GetInstanceScript();
                if (!l_InstanceScript || !l_CastTimeEntry)
                    return;

                if (!l_InstanceScript->instance)
                    return;

                m_MissileCount = l_InstanceScript->instance->GetPlayerCount();
                switch (l_InstanceScript->instance->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidNormal:
                    case Difficulty::DifficultyRaidLFR:
                        m_MissileCount = m_MissileCount * 3 / 4;
                        break;
                    case Difficulty::DifficultyRaidMythic:
                        m_MissileCount = 30;
                        break;
                    case Difficulty::DifficultyRaidHeroic:
                    default:
                        break;
                }

                if (!m_MissileCount)
                    return;

                uint32 l_Interval = 2 * IN_MILLISECONDS / m_MissileCount;

                for (uint8 l_I = 0; l_I < m_MissileCount; ++l_I)
                {
                    float l_MinRange = 15.0f;
                    float l_Left = 14.0f;
                    float l_Right = 15.0f;

                    float l_FrontDist = frand(l_MinRange, l_Caster->GetExactDist(&m_BridgeEnd));
                    float l_SideDist = frand(-l_Left, l_Right);

                    Position l_Target;
                    l_Caster->SimplePosXYRelocationByAngle(l_Target, l_FrontDist, 0.0f);
                    l_Target.SetOrientation(l_Caster->GetOrientation());
                    l_Target.SimplePosXYRelocationByAngle(l_Target, l_SideDist, M_PI / 2.0f);
                    l_Target.SetOrientation(0.0f);
                    l_Target.m_positionZ = 3.16f;

                    m_MissilesDest.push_back(l_Target);

                    float l_Dist = l_Caster->GetExactDist(&l_Target);

                    float l_VisualDuration = l_Interval * l_I + l_CastTimeEntry->CastTime;
                    l_VisualDuration /= IN_MILLISECONDS;
                    l_VisualDuration += l_Dist / l_MissilesSpellInfo->Speed;

                    l_Caster->SendPlaySpellVisual(54102, nullptr, l_VisualDuration, l_Target, true);
                }
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_MissilesDest.size() < m_MissileCount)
                    return;

                if (!m_MissileCount)
                    return;

                uint32 l_Interval = 2 * IN_MILLISECONDS / m_MissileCount;
                for (uint8 l_I = 0; l_I < m_MissileCount; ++l_I)
                {
                    Position l_Dest = m_MissilesDest[l_I];
                    l_Caster->AddDelayedEvent([l_Caster, l_Dest]() -> void
                    {
                        l_Caster->CastSpell(l_Dest, urand(0, 1) ? eSpells::BurningPitchMissileLeft : eSpells::BurningPitchMissileRight, true);
                    }, l_I * l_Interval);
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_krosus_burning_pitch_SpellScript::HandleOnPrepare);
                OnCast += SpellCastFn(spell_krosus_burning_pitch_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_burning_pitch_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Pitch - 206280
class spell_krosus_burning_pitch_missiles : public SpellScriptLoader
{
    public:
        spell_krosus_burning_pitch_missiles() : SpellScriptLoader("spell_krosus_burning_pitch_missiles") { }

        class spell_krosus_burning_pitch_missiles_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_burning_pitch_missiles_SpellScript);

            enum eSpells
            {
                BurningPitchSummonEmber     = 206350,
                BurningPitchDamage          = 206376
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size())
                    return;

                WorldLocation const* l_Dest = GetExplTargetDest();
                Unit* l_Caster = GetCaster();
                if (!l_Dest || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::BurningPitchSummonEmber, true);
            }

            void HandleOnEffectHitTarget(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::BurningPitchDamage, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_krosus_burning_pitch_missiles_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_krosus_burning_pitch_missiles_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_burning_pitch_missiles_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Orb Of Destruction (Filter) - 205343
class spell_krosus_orb_of_destruction_filter : public SpellScriptLoader
{
    public:
        spell_krosus_orb_of_destruction_filter() : SpellScriptLoader("spell_krosus_orb_of_destruction_filter") { }

        class spell_krosus_orb_of_destruction_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_orb_of_destruction_filter_SpellScript);

            enum eSpells
            {
                OrbOfDestruction = 205361
            };

            enum eTalks
            {
                WarningOrbOfDestruction = 7
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                std::list<WorldObject*> l_TempTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Player* l_Player = l_Target->ToPlayer();
                    if (!l_Player)
                        continue;

                    if (l_Player->IsRangedDamageDealer(true))
                        l_TempTargets.push_back(l_Target);
                }

                if (!l_TempTargets.empty())
                {
                    JadeCore::RandomResizeList(l_TempTargets, 1);
                    p_Targets = l_TempTargets;
                    return;
                }

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    Player* l_Player = (*l_Itr)->ToPlayer();
                    if (!l_Player || l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                    {
                        l_Itr = p_Targets.erase(l_Itr);
                        continue;
                    }

                    l_Itr++;
                }

                if (!p_Targets.empty())
                    JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Caster->IsAIEnabled || !l_Target)
                    return;

                reinterpret_cast<CreatureAI*>(l_Caster->GetAI())->Talk(eTalks::WarningOrbOfDestruction, l_Target->GetGUID());
                l_Caster->CastSpell(l_Target, eSpells::OrbOfDestruction, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_krosus_orb_of_destruction_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_krosus_orb_of_destruction_filter_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_orb_of_destruction_filter_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Orb Of Destruction - 205361
class spell_krosus_orb_of_destruction : public SpellScriptLoader
{
    public:
        spell_krosus_orb_of_destruction() : SpellScriptLoader("spell_krosus_orb_of_destruction") { }

        class spell_krosus_orb_of_destruction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_orb_of_destruction_SpellScript);

            enum eSpells
            {
                OrbOfDestructionChargingVisual  = 205347,
                OrbOfDestructionAura            = 205344
            /// OrbOfDestructionDamage          = 205348  triggered by aura
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster ||!l_Target)
                    return;

                l_Caster->DelayedCastSpell(l_Target, eSpells::OrbOfDestructionChargingVisual, true, 1 * IN_MILLISECONDS);
                l_Caster->CastSpell(l_Target, eSpells::OrbOfDestructionAura, true);

                InstanceScript* l_InstanceScript = l_Caster->GetInstanceScript();
                if (!l_InstanceScript || !l_InstanceScript->instance)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::OrbOfDestructionAura);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::OrbOfDestructionAura, l_InstanceScript->instance->GetDifficultyID());
                if (!l_Aura || !l_SpellInfo)
                    return;

                l_Aura->SetMaxDuration(l_SpellInfo->Effects[EFFECT_0].Amplitude);
                l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_0].Amplitude);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_krosus_orb_of_destruction_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_orb_of_destruction_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
///  Orb Of Destruction - 205344
class spell_krosus_orb_of_destruction_aura : public SpellScriptLoader
{
    public:
        spell_krosus_orb_of_destruction_aura() : SpellScriptLoader("spell_krosus_orb_of_destruction_aura") { }

        class spell_krosus_orb_of_destruction_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_krosus_orb_of_destruction_aura_AuraScript);

            enum  eSpells
            {
                IsolatedRage = 208194
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Caster = GetCaster();
                if (!l_Aura || !l_Caster)
                    return;

                if (!l_Caster->GetMap() || l_Caster->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidLFR)
                    return;

                l_Aura->SetDuration(6 * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_krosus_orb_of_destruction_aura_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_krosus_orb_of_destruction_aura_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Orb Of Destruction (Damage) - 205348
class spell_krosus_orb_of_destruction_damage : public SpellScriptLoader
{
    public:
        spell_krosus_orb_of_destruction_damage() : SpellScriptLoader("spell_krosus_orb_of_destruction_damage") { }

        class spell_krosus_orb_of_destruction_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_orb_of_destruction_damage_SpellScript);

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
                OnEffectHitTarget += SpellEffectFn(spell_krosus_orb_of_destruction_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_orb_of_destruction_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Beam - 205383
class spell_krosus_fel_beam : public SpellScriptLoader
{
    public:
        spell_krosus_fel_beam() : SpellScriptLoader("spell_krosus_fel_beam") { }

        class spell_krosus_fel_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_fel_beam_SpellScript);

            enum eSpells
            {
                FelBeamLeft     = 205368,
                FelBeamRight    = 205370,
            /// FelBeamDamage   = 205391  triggered by FelBeamLeft and FelBeamRight
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                uint32 l_Side = l_Caster->GetAI()->GetData(eScriptData::Side) ? 0 : 1;
                l_Caster->GetAI()->SetData(eScriptData::Side, l_Side);
                l_Caster->CastSpell(l_Caster, l_Side ? eSpells::FelBeamLeft : eSpells::FelBeamRight, false);
            }

            void HandleOnPrepare()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Caster = GetCaster();
                if (!l_Spell || !l_Caster)
                    return;

                if (!l_Caster->GetMap())
                    return;

                switch (l_Caster->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidLFR:
                        l_Spell->SetCastTime(4500);
                        break;
                    case Difficulty::DifficultyRaidNormal:
                        l_Spell->SetCastTime(4 * IN_MILLISECONDS);
                        break;
                    case Difficulty::DifficultyRaidHeroic:
                        l_Spell->SetCastTime(3 * IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_krosus_fel_beam_SpellScript::HandleOnPrepare);
                OnCast += SpellCastFn(spell_krosus_fel_beam_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_fel_beam_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Beam - 205391
class spell_krosus_fel_beam_damage : public SpellScriptLoader
{
    public:
        spell_krosus_fel_beam_damage() : SpellScriptLoader("spell_krosus_fel_beam_damage") { }

        class spell_krosus_fel_beam_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krosus_fel_beam_damage_SpellScript);

            std::array<float, 2> m_Sides =
            {
                {
                    15.0f,
                    17.0f
                }
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                uint32 l_Side = l_Caster->GetAI()->GetData(eScriptData::Side);
                if (l_Side > 1)
                    return;

                Position l_Origin;
                float l_Dist = m_Sides[l_Side] / 2.0f;
                float l_Angle = l_Side ? M_PI / 2.0f : -M_PI / 2.0f;
                l_Caster->SimplePosXYRelocationByAngle(l_Origin, l_Dist, l_Angle);
                l_Origin.SetOrientation(l_Caster->GetOrientation());

                Position l_Dest;
                l_Origin.SimplePosXYRelocationByAngle(l_Dest, 150.0f, 0.0f);

                WorldLocation l_WorldOrigin(l_Caster->GetMapId(), l_Origin.m_positionX, l_Origin.m_positionY, l_Origin.m_positionZ);
                WorldLocation l_WorldDest(l_Caster->GetMapId(), l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ);

                std::list<WorldObject*> l_FinalTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    if (!l_Target->IsInAxe(&l_WorldOrigin, &l_WorldDest, l_Dist + 2.0f))
                        continue;

                    l_FinalTargets.push_back(l_Target);
                }

                p_Targets = l_FinalTargets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_krosus_fel_beam_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST_2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_krosus_fel_beam_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Isolated Rage - 208201
class spell_krosus_isolated_rage : public SpellScriptLoader
{
    public:
        spell_krosus_isolated_rage() : SpellScriptLoader("spell_krosus_isolated_rage") { }

        class spell_krosus_isolated_rage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_krosus_isolated_rage_AuraScript);

            enum  eSpells
            {
                IsolatedRage = 208194
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::IsolatedRage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_krosus_isolated_rage_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_krosus_isolated_rage_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Ember - 104262
class npc_krosus_burning_ember : public CreatureScript
{
    public:
        npc_krosus_burning_ember() : CreatureScript("npc_krosus_burning_ember") { }

        struct npc_krosus_burning_emberAI : public LegionCombatAI
        {
            npc_krosus_burning_emberAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eSpells
            {
                Burnout = 214141
            };

            void Reset() override
            {
                LegionCombatAI::Reset();

                me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                LegionCombatAI::UpdateAI(p_Diff);

                if (me->GetMap())
                {
                    if (me->m_positionZ < 0.0f)
                    {
                        me->Kill(me);
                        Creature* l_Krosus = me->FindNearestCreature(eCreatures::Krosus, 200.0f);
                        if (l_Krosus && l_Krosus->IsAIEnabled)
                            l_Krosus->GetAI()->SetData(eScriptData::DataAchievement, 0);

                        return;
                    }
                    if (me->GetMap()->GetHeight(me->GetPhaseMask(), me->m_positionX, me->m_positionY, me->m_positionZ, false) < 3.0f && !me->IsMoving())
                    {
                        Position l_FallingTarget = *me;
                        l_FallingTarget.m_positionZ -= 10.0f;

                        me->GetMotionMaster()->MovePoint(0, l_FallingTarget, false);
                    }
                }

                if (me->GetPower(Powers::POWER_ENERGY) != me->GetMaxPower(Powers::POWER_ENERGY))
                    return;

                me->CastSpell(me, eSpells::Burnout, true);
                me->Kill(me);
            }

            bool CanChase() override
            {
                return false;
            }

            bool CanAutoAttack() override
            {
                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_krosus_burning_emberAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Image of Khadgar - 110677
class npc_krosus_khadgar : public CreatureScript
{
    public:
        npc_krosus_khadgar() : CreatureScript("npc_krosus_khadgar") { }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->NearTeleportTo(98.08f, 2951.02f, 108.97f, 0.78f);
            return true;
        }
};

/// Last Update 7.1.5 Build 23420
/// Felweaver Pharamere - 111227
/// Summoner Xiv - 111226
/// Chaos Mage Beleron - 111225
class npc_krosus_intro : public CreatureScript
{
    public:
        npc_krosus_intro() : CreatureScript("npc_krosus_intro") { }

        enum eSpells
        {
            SpellFelChanneling      = 187960,
            SpellFelOrbVisual       = 209344,
            SpellFelBeamKrosus      = 209346,

            /// Beleron
            SpellChaoticEnergies    = 221464,

            /// Pharamere
            SpellSeedOfFlame        = 222101,

            /// Xiv
            SpellNetherRift         = 222131,
        };

        enum eEvents
        {
            EventChaoticEnergies    = 1,
            EventSeedOfFlame,
            EventNetherRift
        };

        struct npc_krosus_introAI : public ScriptedAI
        {
            npc_krosus_introAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcWorldTrigger, 100.f, true);

                    if (l_Stalker != nullptr)
                    {
                        me->CastSpell(l_Stalker, eSpells::SpellFelChanneling, true);
                        l_Stalker->CastSpell(l_Stalker, eSpells::SpellFelOrbVisual, true);
                    }
                });
            }

            void JustReachedHome() override
            {
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcWorldTrigger, 100.f, true);

                    if (l_Stalker != nullptr)
                    {
                        me->CastSpell(l_Stalker, eSpells::SpellFelChanneling, true);
                        l_Stalker->CastSpell(l_Stalker, eSpells::SpellFelOrbVisual, true);
                    }
                });
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcWorldTrigger, 50.f, true);

                if (l_Stalker != nullptr)
                {
                    l_Stalker->CastStop();
                    l_Stalker->RemoveAllAuras();
                }

                if (me->GetEntry() == eCreatures::NpcSummonerXiv)
                    events.ScheduleEvent(eEvents::EventNetherRift, urand(4, 10) * IN_MILLISECONDS);
                else if (me->GetEntry() == eCreatures::NpcChaosMageBeleron)
                    events.ScheduleEvent(eEvents::EventChaoticEnergies, urand(10, 20) * IN_MILLISECONDS);
                else
                    events.ScheduleEvent(eEvents::EventSeedOfFlame, urand(8, 16) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                events.Reset();

                Creature* l_Krosus = me->FindNearestCreature(eCreatures::Krosus, 200.0f);
                if (!l_Krosus || !l_Krosus->IsAIEnabled)
                    return;

                l_Krosus->GetAI()->SetGUID(me->GetGUID(), eScriptData::Trash);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventChaoticEnergies:
                    {
                        DoCast(me, eSpells::SpellChaoticEnergies);
                        events.ScheduleEvent(eEvents::EventChaoticEnergies, urand(25, 30) * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSeedOfFlame:
                    {
                        DoCastVictim(eSpells::SpellSeedOfFlame);
                        events.ScheduleEvent(eEvents::EventSeedOfFlame, urand(14, 20) * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventNetherRift:
                    {
                        DoCast(me, eSpells::SpellNetherRift);
                        events.ScheduleEvent(eEvents::EventNetherRift, urand(14, 20) * IN_MILLISECONDS);
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

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_krosus_introAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_krosus()
{
    /// Boss
    new boss_krosus();

    /// Spells
    new spell_krosus_slam();
    new spell_krosus_burning_pitch();
    new spell_krosus_burning_pitch_missiles();
    new spell_krosus_orb_of_destruction_filter();
    new spell_krosus_orb_of_destruction();
    new spell_krosus_orb_of_destruction_aura();
    new spell_krosus_orb_of_destruction_damage();
    new spell_krosus_fel_beam();
    new spell_krosus_fel_beam_damage();
    new spell_krosus_isolated_rage();

    /// Areatrigger
    new at_krosus_slam_right_hand();
    new at_krosus_slam_left_hand();

    /// NPC
    new npc_krosus_burning_ember();

    /// Event
    new npc_krosus_khadgar();
    new npc_krosus_intro();
}
#endif
