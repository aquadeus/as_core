////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "boss_eonar_the_life_binder.hpp"

Position const g_InquisitorPos = { -4207.34f, -10700.3f, 728.356f, 1.5708f };

/// Essence of Eonar - 122500
class boss_essence_of_eonar : public CreatureScript
{
    public:
        boss_essence_of_eonar() : CreatureScript("boss_essence_of_eonar") { }

        enum eSpells
        {
            /// Misc
            SpellBossEonarApproach          = 249247,
            SpellBossEonarSaved             = 249373,
            SpellSpawn                      = 246951,
            SpellEonarBonusLoot             = 250602,
            SpellParaxisExplosion           = 250361,
            SpellTormentDebuff              = 249156,
            SpellFeedbackFoulSteps          = 249014,
            SpellFeedbackBurningEmbers      = 249015,
            SpellFeedbackTargeted           = 249016,
            SpellFeedbackArcaneSingularity  = 249017,
            SpellEssenceOfTheLifebinder     = 245764,
            SpellCosmEssenceOfTheLifebinder = 253875,
            SpellSurgeOfLifeDown            = 254506,
            SpellRainOfFelAura              = 248332,
            SpellCloakAT                    = 246753,
            SpellFelWakeDoT                 = 248795,
            SpellFinalDoom                  = 249121,
            SpellPersistance                = 252749,
            /// Life Force
            SpellLifeForceParaxis           = 250030,
            SpellLifeForceVisual            = 250043,
            SpellLifeForceCast              = 250048,
            SpellLifeForceTriggered         = 250467,
            SpellLifeForceDamage            = 254269
        };

        enum eEvents
        {
            EventRainOfFel = 1,
            EventSpearOfDoom,
            EventFinalDoom,
            EventLifeForce,

            EventMax
        };

        enum eTalks
        {
            TalkIntro,
            TalkAggro,
            TalkLifeForceWarn,
            TalkLifeForce,
            Talk75Percent,
            Talk25Percent,
            TalkFailure,
            TalkKillRemainingDemons,
            TalkFinalDoom
        };

        enum eActions
        {
            ActionIntro,
            ActionFight,
            ActionRainOfFel,
            ActionSpearOfDoom,
            ActionFinalDoom,
            ActionParaxisDeath,
            ActionOrbOfLifeSequence
        };

        enum eGossipData
        {
            MenuAfterEncounter  = 21203,

            OptionToAntorus     = 33006
        };

        enum eAchievementData
        {
            SpheresOfInfluence  = 12067,

            OrbOfLifeCount      = 5
        };

        enum eAreaIDs
        {
            AreaElunaria        = 8681,
            AreaElarianSanctum  = 9333
        };

        struct boss_essence_of_eonarAI : public BossAI
        {
            boss_essence_of_eonarAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataEonarTheLifeBinder) { }

            std::map<uint32, uint64> m_JumpPadToDest;

            bool m_IntroDone = false;
            bool m_EncounterStarted = false;
            bool m_Talk75PctDone = false;
            bool m_Talk25PctDone = false;

            ///      DifficultyID ->      PhaseID ->       Event -> CastCount -> Timer
            std::map<Difficulty, std::map<uint32, std::vector<uint32>>> m_TimerData =
            {
                {
                    Difficulty::DifficultyRaidNormal,
                    {
                        { eEvents::EventRainOfFel,      { 30000, 31000, 35000, 45000, 80000, 50000, 20000, 35000, 20000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidHeroic,
                    {
                        { eEvents::EventRainOfFel,      { 15000, 38500, 10000, 45000, 34500, 19000, 19000, 29000, 44500, 35000, 97000, 99500 } },
                        { eEvents::EventSpearOfDoom,    { 29700, 59600, 64500, 40300, 84600, 35200, 65700, 35100, 64300 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidMythic,
                    {
                        { eEvents::EventRainOfFel,      { 6000, 29000, 25000, 48500, 5000, 20000, 50500, 25000, 4500, 46000, 24000, 4000, 50000, 50000, 50000 } },
                        { eEvents::EventSpearOfDoom,    { 15000, 75000, 75000, 75000, 25000, 75000, 75000, 50000, 50000 } },
                        { eEvents::EventFinalDoom,      { 60500, 120000, 100500, 104500, 100500 } }
                    }
                }
            };

            uint32 m_RainOfFelCounter = 0;
            uint32 m_SpearOfDoomCounter = 0;
            uint32 m_FinalDoomCounter = 0;

            uint64 m_TheParaxisGuid = 0;

            bool m_FirstWaveSpawned = false;
            bool m_LifeForceScheduled = false;

            uint32 m_LifeForceCount = 0;

            uint8 m_OrbOfLifeIDx = 0;
            uint32 m_OrbOfLifeTimer = 0;

            std::array<Position, eAchievementData::OrbOfLifeCount> m_OrbsOfLifePos =
            {
                {
                    { -3906.432f, -10800.15f, 706.6458f, 3.96814f },
                    { -3939.895f, -10943.33f, 725.2295f, 3.96814f },
                    { -4019.374f, -10726.98f, 662.2704f, 3.96814f },
                    { -3873.695f, -10727.41f, 689.7440f, 3.96814f },
                    { -4165.663f, -10948.08f, 761.5823f, 3.96814f }
                }
            };

            uint32 m_SurgeOfLifeTImer = 0;

            void Reset() override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                _Reset();

                ClearDelayedOperations();

                summons.DespawnAll();

                SetCombatMovement(false);

                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                me->SetPower(Powers::POWER_ENERGY, 0);
                me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);

                if (m_IntroDone && instance)
                {
                    instance->PlayCosmeticEvent(eCosmeticEvents::AntorusEonarCosmeticMobs, me->GetGUID(), true);

                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTormentDebuff);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackFoulSteps);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackBurningEmbers);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackTargeted);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackArcaneSingularity);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEssenceOfTheLifebinder);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCosmEssenceOfTheLifebinder);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellRainOfFelAura);

                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        SpawnAchievementOrbs();
                    });
                }

                InitializeJumpPads();

                m_FirstWaveSpawned = false;
                m_LifeForceScheduled = false;

                m_SurgeOfLifeTImer = 0;
            }

            void EnterEvadeMode() override
            {
                m_EncounterStarted = false;

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTormentDebuff);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackFoulSteps);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackBurningEmbers);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackTargeted);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackArcaneSingularity);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEssenceOfTheLifebinder);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCosmEssenceOfTheLifebinder);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellRainOfFelAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelWakeDoT);

                    /// Teleport back players that are inside the Paraxis at the entrance of the encounter area
                    if (IsMythic())
                    {
                        Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (l_Player->IsNearPosition(&g_InquisitorPos, 130.0f))
                                    l_Player->NearTeleportTo(eLocations::LocationEonarLifebinder);
                            }
                        }
                    }
                }

                me->ReenableHealthRegen();

                ClearDelayedOperations();

                summons.DespawnAll();

                me->InterruptNonMeleeSpells(true);

                if (Player* l_Player = me->FindNearestPlayer(SIZE_OF_GRIDS))
                {
                    l_Player->CombatStop();
                }

                BossAI::EnterEvadeMode();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionIntro:
                    {
                        if (m_IntroDone)
                            break;

                        if (instance)
                            instance->PlayCosmeticEvent(eCosmeticEvents::AntorusEonarCosmeticMobs, me->GetGUID(), true);

                        m_IntroDone = true;

                        if (Player* l_Player = me->FindNearestPlayer(SIZE_OF_GRIDS))
                            l_Player->CastSpell(l_Player, eSpells::SpellBossEonarApproach, true);

                        AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro);
                        });

                        break;
                    }
                    case eActions::ActionFight:
                    {
                        if (m_EncounterStarted)
                            return;

                        StartEncounter();
                        break;
                    }
                    case eActions::ActionParaxisDeath:
                    {
                        me->DisableEvadeMode();

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            SpawnAchievementOrbs();
                        });

                        /// Complete encounter
                        _JustDied();

                        me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        me->SetPower(Powers::POWER_ENERGY, 0);
                        me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);

                        me->CombatStop();

                        m_EncounterStarted = false;

                        ClearDelayedOperations();

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            summons.DespawnAll();
                        });

                        me->InterruptNonMeleeSpells(true);

                        if (Player* l_Player = me->FindNearestPlayer(SIZE_OF_GRIDS))
                        {
                            DoCast(l_Player, eSpells::SpellBossEonarSaved, true);
                            l_Player->CombatStop();
                        }

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTormentDebuff);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackFoulSteps);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackBurningEmbers);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackTargeted);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFeedbackArcaneSingularity);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEssenceOfTheLifebinder);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCosmEssenceOfTheLifebinder);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSurgeOfLifeDown);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellRainOfFelAura);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelWakeDoT);

                            instance->UpdateEncounterState(EncounterCreditType::ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);

                            instance->DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellSurgeOfLifeSecond);

                            if (Creature* l_Paraxis = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcTheParaxisDummy)))
                                l_Paraxis->CastSpell(l_Paraxis, eSpells::SpellParaxisExplosion, true);

                            if (GameObject* l_Paraxis = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoParaxisShip)))
                                l_Paraxis->Delete();

                            instance->DoCastSpellOnPlayers(eSpells::SpellEonarBonusLoot, me);
                            instance->DoCombatStopOnPlayers();

                            /// Teleport back players that are inside the Paraxis at the entrance of the encounter area
                            if (IsMythic())
                            {
                                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                                {
                                    if (Player* l_Player = l_Iter->getSource())
                                    {
                                        if (l_Player->IsNearPosition(&g_InquisitorPos, 130.0f))
                                            l_Player->NearTeleportTo(eLocations::LocationEonarLifebinder);
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case eActions::ActionOrbOfLifeSequence:
                    {
                        if ((m_OrbOfLifeIDx + 1) >= eAchievementData::OrbOfLifeCount && instance)
                        {
                            m_OrbOfLifeTimer = 0;

                            instance->DoCompleteAchievement(eAchievementData::SpheresOfInfluence);
                            break;
                        }

                        m_OrbOfLifeTimer = 90 * TimeConstants::IN_MILLISECONDS;

                        me->SummonCreature(eCreatures::NpcOrbOfLife, m_OrbsOfLifePos[++m_OrbOfLifeIDx]);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustSummonedCosmetic(Creature* p_Summon, uint8 p_EventID) override
            {
                /// Spawn visual
                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcFelguard:
                    case eCreatures::NpcFelInfusedDestructor:
                    case eCreatures::NpcFelPoweredPurifier:
                    case eCreatures::NpcVolantKerapteron:
                    case eCreatures::NpcFelHound:
                    case eCreatures::NpcFelLord:
                    {
                        if (!m_EncounterStarted)
                        {
                            p_Summon->DespawnOrUnsummon(1);
                            break;
                        }

                        p_Summon->DisableHealthRegen();
                        p_Summon->DisableEvadeMode();

                        p_Summon->CastSpell(p_Summon, eSpells::SpellSpawn, true);

                        if (IsMythic() && m_FirstWaveSpawned)
                            p_Summon->CastSpell(p_Summon, eSpells::SpellPersistance, true);

                        break;
                    }
                    case eCreatures::NpcFelChargedObfuscator:
                    {
                        if (!m_EncounterStarted)
                        {
                            p_Summon->DespawnOrUnsummon(1);
                            break;
                        }

                        p_Summon->DisableHealthRegen();
                        p_Summon->DisableEvadeMode();

                        p_Summon->CastSpell(p_Summon, eSpells::SpellSpawn, true);
                        p_Summon->CastSpell(p_Summon, eSpells::SpellCloakAT, true);

                        if (IsMythic() && m_FirstWaveSpawned)
                            p_Summon->CastSpell(p_Summon, eSpells::SpellPersistance, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                p_Summon->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcFelguard:
                    {
                        me->ModifyPower(Powers::POWER_ALTERNATE_POWER, IsMythic() ? 1 : 4);
                        break;
                    }
                    case eCreatures::NpcFelHound:
                    {
                        int32 l_Amount = 7;

                        if (IsHeroic())
                            l_Amount = 5;
                        else if (IsMythic())
                            l_Amount = 2;

                        me->ModifyPower(Powers::POWER_ALTERNATE_POWER, l_Amount);
                        break;
                    }
                    case eCreatures::NpcVolantKerapteron:
                    {
                        int32 l_Amount = 8;

                        if (IsHeroic())
                            l_Amount = 7;
                        else if (IsMythic())
                            l_Amount = 4;

                        me->ModifyPower(Powers::POWER_ALTERNATE_POWER, l_Amount);
                        break;
                    }
                    case eCreatures::NpcFelLord:
                    {
                        me->ModifyPower(Powers::POWER_ALTERNATE_POWER, IsMythic() ? 5 : 9);
                        break;
                    }
                    case eCreatures::NpcFelInfusedDestructor:
                    case eCreatures::NpcFelChargedObfuscator:
                    case eCreatures::NpcFelPoweredPurifier:
                    {
                        me->ModifyPower(Powers::POWER_ALTERNATE_POWER, IsMythic() ? 6 : 10);
                        break;
                    }
                    /// Mythic only
                    case eCreatures::NpcParaxisInquisitor:
                    {
                        me->ModifyPower(Powers::POWER_ALTERNATE_POWER, 1);
                        break;
                    }
                    default:
                        break;
                }

                if (!m_LifeForceScheduled && me->GetPower(Powers::POWER_ALTERNATE_POWER) >= 100)
                {
                    m_LifeForceScheduled = true;

                    events.ScheduleEvent(eEvents::EventLifeForce, 1);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellLifeForceCast:
                    {
                        std::array<Position const, 5> l_LifeForcePos =
                        {
                            {
                                { -4123.000f, -10668.00f, 736.0000f, 2.665772f },
                                { -4111.941f, -10677.20f, 736.0000f, 2.688237f },
                                { -4105.158f, -10689.87f, 729.0275f, 2.754601f },
                                { -4105.338f, -10728.01f, 736.0000f, 3.058585f },
                                { -4132.490f, -10723.22f, 749.5862f, 3.041226f }
                            }
                        };

                        for (Position const& l_Pos : l_LifeForcePos)
                            DoCast(l_Pos, eSpells::SpellLifeForceVisual, true);

                        DoCast(me, eSpells::SpellLifeForceParaxis, true);

                        me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);

                        m_LifeForceScheduled = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellLifeForceTriggered:
                    {
                        DoCast(p_Target, eSpells::SpellLifeForceDamage, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFinalDoom)
                {
                    Talk(eTalks::TalkFailure);

                    EnterEvadeMode();
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                EnterEvadeMode();
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_EncounterStarted)
                {
                    p_Damage = 0;
                    return;
                }

                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;

                    Talk(eTalks::TalkFailure);

                    EnterEvadeMode();
                }

                if (!m_Talk25PctDone && me->HealthBelowPctDamaged(25, p_Damage))
                {
                    m_Talk25PctDone = true;

                    Talk(eTalks::Talk25Percent);
                }
                else if (!m_Talk75PctDone && me->HealthBelowPctDamaged(75, p_Damage))
                {
                    m_Talk75PctDone = true;

                    Talk(eTalks::Talk75Percent);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_OrbOfLifeTimer)
                {
                    if (m_OrbOfLifeTimer <= p_Diff)
                    {
                        m_OrbOfLifeTimer = 0;

                        /// Reset orb sequence
                        summons.DespawnEntry(eCreatures::NpcOrbOfLife);

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            SpawnAchievementOrbs();
                        });
                    }
                    else
                        m_OrbOfLifeTimer -= p_Diff;
                }

                if (!m_EncounterStarted)
                    return;

                if (instance && instance->IsWipe())
                {
                    EnterEvadeMode();
                    return;
                }

                CheckSurgeOfLifeOnPlayers(p_Diff);

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventRainOfFel:
                    {
                        ++m_RainOfFelCounter;

                        if (Creature* l_Paraxis = Creature::GetCreature(*me, m_TheParaxisGuid))
                        {
                            if (l_Paraxis->IsAIEnabled)
                                l_Paraxis->AI()->DoAction(eActions::ActionRainOfFel);
                        }

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventRainOfFel))
                            events.ScheduleEvent(eEvents::EventRainOfFel, l_Timer);

                        break;
                    }
                    case eEvents::EventSpearOfDoom:
                    {
                        ++m_SpearOfDoomCounter;

                        if (Creature* l_Paraxis = Creature::GetCreature(*me, m_TheParaxisGuid))
                        {
                            if (l_Paraxis->IsAIEnabled)
                                l_Paraxis->AI()->DoAction(eActions::ActionSpearOfDoom);
                        }

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventSpearOfDoom))
                            events.ScheduleEvent(eEvents::EventSpearOfDoom, l_Timer);

                        break;
                    }
                    case eEvents::EventFinalDoom:
                    {
                        ++m_FinalDoomCounter;

                        me->SummonCreature(eCreatures::NpcParaxisInquisitor, g_InquisitorPos);

                        if (Creature* l_Paraxis = Creature::GetCreature(*me, m_TheParaxisGuid))
                        {
                            if (l_Paraxis->IsAIEnabled)
                                l_Paraxis->AI()->DoAction(eActions::ActionFinalDoom);
                        }

                        Talk(eTalks::TalkFinalDoom);

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventFinalDoom))
                            events.ScheduleEvent(eEvents::EventFinalDoom, l_Timer);

                        break;
                    }
                    case eEvents::EventLifeForce:
                    {
                        ++m_LifeForceCount;

                        Talk(eTalks::TalkLifeForce);
                        Talk(eTalks::TalkLifeForceWarn);

                        DoCast(me, eSpells::SpellLifeForceCast);

                        /// Last Life Force needed to destroy the Paraxis
                        if (m_LifeForceCount >= (IsLFR() ? 3 : 4))
                            Talk(eTalks::TalkKillRemainingDemons);

                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_ID /*= 0*/) override
            {
                auto const& l_Itr = m_JumpPadToDest.find(p_ID);
                if (l_Itr != m_JumpPadToDest.end())
                    return l_Itr->second;

                return m_TheParaxisGuid;
            }

            void InitializeJumpPads()
            {
                /// Only initialize it once
                if (!m_JumpPadToDest.empty())
                    return;

                ///                   Jump Pad        Jump Dest
                std::vector<std::pair<Position const, Position const>> l_CoordsLinks =
                {
                    {
                        { { -3896.99f, -10861.50f, 708.435f, 4.56132f }, { -3907.979f, -10891.87f, 686.339f, 0.0f } },
                        { { -3898.79f, -10886.82f, 687.787f, 1.45748f }, { -3885.675f, -10853.19f, 711.578f, 0.0f } },
                        { { -3974.74f, -10810.30f, 686.570f, 1.62543f }, { -3978.941f, -10769.44f, 700.832f, 0.0f } },
                        { { -3893.94f, -10782.50f, 708.435f, 1.63172f }, { -3891.101f, -10753.82f, 745.753f, 0.0f } },
                        { { -3902.17f, -10763.43f, 740.505f, 4.78193f }, { -3899.035f, -10798.59f, 706.646f, 0.0f } },
                        { { -3949.94f, -10726.60f, 698.531f, 0.30069f }, { -3864.285f, -10700.90f, 725.913f, 0.0f } },
                        { { -3985.33f, -10777.95f, 698.529f, 4.76791f }, { -3980.483f, -10820.61f, 684.833f, 0.0f } },
                        { { -3930.96f, -10824.82f, 708.435f, 3.20343f }, { -3956.927f, -10829.21f, 684.833f, 0.0f } }
                    }
                };

                for (auto const& l_Iter : l_CoordsLinks)
                {
                    if (Creature* l_Pad = instance->instance->SummonCreature(eCreatures::NpcJumpPad, l_Iter.first))
                    {
                        if (Creature* l_Dest = instance->instance->SummonCreature(eCreatures::NpcJumpDest, l_Iter.second))
                            m_JumpPadToDest[l_Pad->GetGUIDLow()] = l_Dest->GetGUID();
                    }
                }
            }

            void StartEncounter()
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);

                m_EncounterStarted = true;

                m_Talk75PctDone = false;
                m_Talk25PctDone = false;

                m_SurgeOfLifeTImer = 1 * TimeConstants::IN_MILLISECONDS;

                Talk(eTalks::TalkAggro);

                /// Despawn cosmetics for encounter
                summons.DespawnAll();

                m_TheParaxisGuid = 0;

                if (Creature* l_Paraxis = me->SummonCreature(eCreatures::NpcTheParaxis, { -4208.91f, -10700.0f, 1061.24f, 1.4571f }))
                    m_TheParaxisGuid = l_Paraxis->GetGUID();

                _EnterCombat();

                StartEvents();

                me->DisableHealthRegen();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCosmEssenceOfTheLifebinder);

                    DoCastToAllPlayers(eSpells::SpellEssenceOfTheLifebinder);

                    auto const& l_DiffID = g_WavePattern.find(GetDifficulty());
                    if (l_DiffID == g_WavePattern.end())
                        return;

                    for (auto const& l_Event : l_DiffID->second)
                    {
                        AddTimedDelayedOperation(l_Event.Timer, [this, l_Event]() -> void
                        {
                            if (l_Event.EventID == eCosmeticEvents::EonarWaveMythic2)
                                m_FirstWaveSpawned = true;

                            if (instance)
                                instance->PlayCosmeticEvent(l_Event.EventID, me->GetGUID(), true);
                        });
                    }
                }
            }

            void StartEvents()
            {
                m_RainOfFelCounter = 0;
                m_SpearOfDoomCounter = 0;
                m_FinalDoomCounter = 0;

                auto const& l_DiffMap = m_TimerData.find(GetDifficulty());
                if (l_DiffMap == m_TimerData.end())
                    return;

                for (uint8 l_I = 0; l_I < eEvents::EventMax; ++l_I)
                {
                    auto const& l_EventIter = l_DiffMap->second.find(l_I);
                    if (l_EventIter == l_DiffMap->second.end())
                        continue;

                    uint8 l_Counter = 0;
                    switch (l_I)
                    {
                        case eEvents::EventRainOfFel:
                            l_Counter = m_RainOfFelCounter;
                            break;
                        case eEvents::EventSpearOfDoom:
                            l_Counter = m_SpearOfDoomCounter;
                            break;
                        case eEvents::EventFinalDoom:
                            l_Counter = m_FinalDoomCounter;
                            break;
                        default:
                            break;
                    }

                    events.ScheduleEvent(l_I, l_EventIter->second[l_Counter]);
                }
            }

            uint32 GetEventTimer(uint8 p_EventID) const
            {
                auto const& l_DiffMap = m_TimerData.find(GetDifficulty());
                if (l_DiffMap == m_TimerData.end())
                    return 0;

                auto const& l_EventIter = l_DiffMap->second.find(p_EventID);
                if (l_EventIter == l_DiffMap->second.end())
                    return 0;

                uint8 l_Counter = 0;
                switch (p_EventID)
                {
                    case eEvents::EventRainOfFel:
                        l_Counter = m_RainOfFelCounter;
                        break;
                    case eEvents::EventSpearOfDoom:
                        l_Counter = m_SpearOfDoomCounter;
                        break;
                    case eEvents::EventFinalDoom:
                        l_Counter = m_FinalDoomCounter;
                        break;
                    default:
                        break;
                }

                if (l_Counter >= l_EventIter->second.size())
                    return 0;

                return l_EventIter->second[l_Counter];
            }

            void SpawnAchievementOrbs()
            {
                if (IsLFR())
                    return;

                m_OrbOfLifeIDx = 0;
                m_OrbOfLifeTimer = 0;

                me->SummonCreature(eCreatures::NpcOrbOfLife, m_OrbsOfLifePos[m_OrbOfLifeIDx]);
            }

            void CheckSurgeOfLifeOnPlayers(uint32 const p_Diff)
            {
                if (!m_SurgeOfLifeTImer)
                    return;

                if (m_SurgeOfLifeTImer <= p_Diff)
                {
                    m_SurgeOfLifeTImer = 3 * TimeConstants::IN_MILLISECONDS;

                    Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->GetAreaId() != eAreaIDs::AreaElunaria &&
                                l_Player->GetAreaId() != eAreaIDs::AreaElarianSanctum)
                                continue;

                            if (!l_Player->IsFalling() && !l_Player->IsFlying())
                            {
                                /// Remove previous auras
                                l_Player->RemoveAura(eSpells::SpellEssenceOfTheLifebinder);
                                l_Player->RemoveAura(eSpells::SpellCosmEssenceOfTheLifebinder);
                                l_Player->RemoveAura(eSpells::SpellSurgeOfLifeDown);

                                /// Add new one at next update
                                l_Player->DelayedCastSpell(l_Player, eSpells::SpellEssenceOfTheLifebinder, true, 1);
                            }
                        }
                    }
                }
                else
                    m_SurgeOfLifeTImer -= p_Diff;
            }
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player || !p_Player->GetInstanceScript())
                return false;

            InstanceScript* l_Instance = p_Player->GetInstanceScript();

            if (l_Instance->GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE)
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->ADD_GOSSIP_ITEM_DB(eGossipData::MenuAfterEncounter, 0, eTradeskill::GOSSIP_SENDER_MAIN, 0);
                p_Player->SEND_GOSSIP_MENU(eGossipData::OptionToAntorus, p_Creature->GetGUID());
                return true;
            }

            return false;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            if (!p_Player || !p_Player->GetInstanceScript() || !p_Creature->IsAIEnabled)
                return false;

            InstanceScript* l_Instance = p_Player->GetInstanceScript();

            /// Teleport player if Eonar is already finished
            if (l_Instance->GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE)
            {
                p_Player->NearTeleportTo({ -3110.22f, 10156.5f, -87.95f, 3.344574f });
                return true;
            }
            /// Start encounter
            else
                p_Creature->AI()->DoAction(eActions::ActionFight);

            return true;
        }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_essence_of_eonarAI(p_Creature);
        }
};

/// The Paraxis - 124445
class npc_eonar_the_paraxis : public CreatureScript
{
    public:
        npc_eonar_the_paraxis() : CreatureScript("npc_eonar_the_paraxis") { }

        enum eSpells
        {
            /// Misc
            SpellWarpIn                 = 247214,
            SpellTeleporterAT           = 249094,
            SpellEmpowermentYellow      = 249053,
            SpellEmpowermentRed         = 249054,
            SpellEmpowermentGreen       = 249055,
            SpellEmpowermentBlue        = 249056,
            SpellYellowBeam             = 250525,
            SpellRedBeam                = 250523,
            SpellGreenBeam              = 250524,
            SpellBlueBeam               = 250475,
            SpellFeedbackTargeted       = 249016,
            /// Paraxis Artillery
            SpellParaxisArtillerySearch = 246313,
            SpellParaxisArtilleryDamage = 246315,
            /// Meteor Storm - LFR
            SpellMeteorStormPeriodic    = 248333,
            SpellMeteorStormSearcher    = 248334,
            SpellMeteorStormMissile     = 248335,
            /// Rain of Fel - Normal+
            SpellRainOfFelSearcher      = 248326,
            SpellRainOfFelAura          = 248332,
            SpellRainOfFelMissile       = 248328,
            /// Spear of Doom - Heroic+
            SpellSpearOfDoomSearcher    = 248861,
            SpellSpearOfDoomSummon      = 248791,
            /// Final Doom - Mythic
            SpellFinalDoom              = 249121
        };

        enum eActions
        {
            ActionRainOfFel = 2,
            ActionSpearOfDoom,
            ActionFinalDoom,
            ActionParaxisDeath
        };

        enum eTalks
        {
            TalkRainOfFel,
            TalkSpearOfDoom,
            TalkFinalDoom
        };

        enum eMythicData
        {
            MythicTeleporterCount   = 4,
            MythicParaxisDoors      = 3
        };

        enum eGuidData
        {
            GuidTeleportPlayer,
            GuidFocusingCrystal
        };

        struct npc_eonar_the_paraxisAI : public Scripted_NoMovementAI
        {
            npc_eonar_the_paraxisAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            uint64 m_SpearTargetGuid = 0;

            std::array<Position, eMythicData::MythicTeleporterCount> m_TeleporterPos =
            {
                {
                    { -4011.479f, -10717.76f, 696.8240f },
                    { -4016.535f, -10728.44f, 696.8407f },
                    { -4018.300f, -10741.89f, 696.7355f },
                    { -4016.580f, -10751.54f, 696.7356f }
                }
            };

            uint8 m_ParaxisTeleportIDx = 0;

            std::array<Position, eMythicData::MythicTeleporterCount> m_TeleporterDestPos =
            {
                {
                    { -4216.1514f, -10688.7012f, 728.2731f, 5.36f },
                    { -4195.9565f, -10690.7861f, 728.2731f, 3.83f },
                    { -4198.2998f, -10711.4766f, 728.2731f, 2.23f },
                    { -4218.6689f, -10709.2100f, 728.2731f, 0.69f }
                }
            };

            std::array<uint32, eMythicData::MythicParaxisDoors> m_ParaxisDoors =
            {
                {
                    eGameObjects::GoParaxisDoor1,
                    eGameObjects::GoParaxisDoor2,
                    eGameObjects::GoParaxisDoor3
                }
            };

            std::array<std::pair<uint32, Position const>, eMythicData::MythicTeleporterCount> m_ParaxisCrystals =
            {
                {
                    { eCreatures::NpcParaxisFocusingCrystal1, { -4240.8364f, -10656.3643f, 734.5264f, 0.0f } },
                    { eCreatures::NpcParaxisFocusingCrystal2, { -4173.4419f, -10742.0293f, 734.5243f, 0.0f } },
                    { eCreatures::NpcParaxisFocusingCrystal3, { -4165.5469f, -10667.0254f, 734.5253f, 0.0f } },
                    { eCreatures::NpcParaxisFocusingCrystal4, { -4249.3135f, -10733.1016f, 734.5248f, 0.0f } }
                }
            };

            std::array<uint32, eMythicData::MythicTeleporterCount> m_CrystalColors =
            {
                {
                    eSpells::SpellEmpowermentYellow,
                    eSpells::SpellEmpowermentRed,
                    eSpells::SpellEmpowermentGreen,
                    eSpells::SpellEmpowermentBlue
                }
            };

            std::array<uint32, eMythicData::MythicTeleporterCount> m_CrystalBeams =
            {
                {
                    eSpells::SpellYellowBeam,
                    eSpells::SpellRedBeam,
                    eSpells::SpellGreenBeam,
                    eSpells::SpellBlueBeam
                }
            };

            std::map<uint32, uint64> m_FocusingCrystalGuids;

            uint64 m_FocusGuid = 0;

            std::set<uint64> m_FeedbackTargets;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetInCombatWithZone();

                DoCast(me, eSpells::SpellWarpIn, true);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);
            }

            void JustDespawned() override
            {
                me->RemoveAllAreasTrigger();

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellParaxisArtillerySearch:
                    {
                        DoCast(p_Target, eSpells::SpellParaxisArtilleryDamage, true);
                        break;
                    }
                    case eSpells::SpellRainOfFelSearcher:
                    {
                        sCreatureTextMgr->SendChat(me, eTalks::TalkRainOfFel, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                        uint64 l_Guid = p_Target->GetGUID();
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                        {
                            if (Unit* l_Target = Unit::GetUnit(*me, l_Guid))
                                DoCast(l_Target, eSpells::SpellRainOfFelMissile, true);
                        });

                        DoCast(p_Target, eSpells::SpellRainOfFelAura, true);
                        break;
                    }
                    case eSpells::SpellSpearOfDoomSearcher:
                    {
                        m_SpearTargetGuid = p_Target->GetGUID();

                        DoCast(p_Target, eSpells::SpellSpearOfDoomSummon, true);
                        break;
                    }
                    case eSpells::SpellMeteorStormSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellMeteorStormMissile, true);
                        break;
                    }
                    case eSpells::SpellFeedbackTargeted:
                    {
                        m_FeedbackTargets.insert(p_Target->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellParaxisArtillerySearch:
                    {
                        /// Paraxis Artillery should target the two-thirds of the raid
                        if (uint32 l_Count = uint32(p_Targets.size()) / 3)
                            JadeCore::Containers::RandomResizeList(p_Targets, uint32(p_Targets.size()) - l_Count);

                        /// Don't break, we still need to add Feedback targets
                    }
                    case eSpells::SpellRainOfFelSearcher:
                    case eSpells::SpellSpearOfDoomSearcher:
                    {
                        if (!p_Targets.empty())
                        {
                            /// Don't affect flying peoples
                            p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                            {
                                if (p_Object && p_Object->ToUnit() && (p_Object->ToUnit()->IsFlying() || p_Object->ToUnit()->IsFalling()))
                                    return true;

                                return false;
                            });
                        }

                        for (uint64 const& l_Guid : m_FeedbackTargets)
                        {
                            if (WorldObject* l_Target = Player::GetPlayer(*me, l_Guid))
                            {
                                /// If feedback target is not already in target list, add it
                                if (std::find(p_Targets.begin(), p_Targets.end(), l_Target) == p_Targets.end())
                                    p_Targets.push_back(l_Target);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_Instance)
                    return;

                for (uint32 const& l_Entry : m_ParaxisDoors)
                {
                    if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(l_Entry)))
                        l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
                }

                m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                if (Creature* l_Eonar = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::BossEssenceOfEonar)))
                {
                    if (l_Eonar->IsAIEnabled)
                        l_Eonar->AI()->DoAction(eActions::ActionParaxisDeath);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionRainOfFel:
                    {
                        if (IsLFR())
                            DoCast(me, eSpells::SpellMeteorStormPeriodic, true);
                        else
                            DoCast(me, eSpells::SpellRainOfFelSearcher, true);

                        break;
                    }
                    case eActions::ActionSpearOfDoom:
                    {
                        m_SpearTargetGuid = 0;

                        Talk(eTalks::TalkSpearOfDoom);

                        DoCast(me, eSpells::SpellSpearOfDoomSearcher, true);
                        break;
                    }
                    case eActions::ActionFinalDoom:
                    {
                        m_FocusingCrystalGuids.clear();

                        m_FocusGuid = 0;

                        Talk(eTalks::TalkFinalDoom);

                        m_ParaxisTeleportIDx = 0;

                        me->RemoveAreaTrigger(eSpells::SpellTeleporterAT);

                        for (Position const& l_Pos : m_TeleporterPos)
                            DoCast(l_Pos, eSpells::SpellTeleporterAT, true);

                        if (m_Instance)
                        {
                            if (Creature* l_Eonar = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::BossEssenceOfEonar)))
                            {
                                if (Creature* l_Focus = l_Eonar->SummonCreature(eCreatures::NpcFocus, { -4207.2085f, -10699.9326f, 844.0000f, 0.0f }))
                                    m_FocusGuid = l_Focus->GetGUID();

                                std::vector<uint8> l_Indexes;

                                for (uint8 l_I = 0; l_I < eMythicData::MythicTeleporterCount; ++l_I)
                                    l_Indexes.push_back(l_I);

                                auto l_Seed = std::chrono::system_clock::now().time_since_epoch().count();
                                std::shuffle(l_Indexes.begin(), l_Indexes.end(), std::default_random_engine(l_Seed));

                                for (uint8 l_I = 0; l_I < eMythicData::MythicTeleporterCount; ++l_I)
                                {
                                    if (Creature* l_Crystal = l_Eonar->SummonCreature(m_ParaxisCrystals[l_I].first, m_ParaxisCrystals[l_I].second))
                                    {
                                        m_FocusingCrystalGuids[m_ParaxisCrystals[l_I].first] = l_Crystal->GetGUID();

                                        me->CastSpell(l_Crystal, m_CrystalColors[l_Indexes[l_I]], true);

                                        l_Crystal->DelayedCastSpell(l_Crystal, m_CrystalBeams[l_Indexes[l_I]], true, 1 * TimeConstants::IN_MILLISECONDS);
                                    }
                                }
                            }

                            for (uint32 const& l_Entry : m_ParaxisDoors)
                            {
                                if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(l_Entry)))
                                    l_Door->SetGoState(GOState::GO_STATE_READY);
                            }
                        }

                        DoCast(me, eSpells::SpellFinalDoom);
                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_ID /*= 0*/) override
            {
                auto const& l_Iter = m_FocusingCrystalGuids.find(p_ID);
                if (l_Iter == m_FocusingCrystalGuids.end())
                    return m_SpearTargetGuid;

                return l_Iter->second;
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                switch (p_ID)
                {
                    case eGuidData::GuidTeleportPlayer:
                    {
                        if (m_ParaxisTeleportIDx >= eMythicData::MythicTeleporterCount)
                            break;

                        Player* l_Player = Player::GetPlayer(*me, p_Guid);
                        if (!l_Player)
                            break;

                        l_Player->UnsummonPetTemporaryIfAny();
                        l_Player->UnsummonCurrentBattlePetIfAny(true);

                        l_Player->NearTeleportTo(m_TeleporterDestPos[m_ParaxisTeleportIDx++]);
                        break;
                    }
                    case eGuidData::GuidFocusingCrystal:
                    {
                        if (Creature* l_Crystal = Creature::GetCreature(*me, p_Guid))
                        {
                            auto const& l_Iter = m_FocusingCrystalGuids.find(l_Crystal->GetEntry());
                            if (l_Iter == m_FocusingCrystalGuids.end())
                                break;

                            m_FocusingCrystalGuids.erase(l_Iter);

                            l_Crystal->DespawnOrUnsummon(3 * TimeConstants::IN_MILLISECONDS);
                        }

                        /// Interrupt Final Doom cast after all crystals got shattered
                        if (m_FocusingCrystalGuids.empty())
                        {
                            me->InterruptNonMeleeSpells(true, eSpells::SpellFinalDoom);

                            for (uint32 const& l_Entry : m_ParaxisDoors)
                            {
                                if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(l_Entry)))
                                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
                            }

                            if (Creature* l_Focus = Creature::GetCreature(*me, m_FocusGuid))
                                l_Focus->DespawnOrUnsummon(3 * TimeConstants::IN_MILLISECONDS);

                            me->RemoveAreaTrigger(eSpells::SpellTeleporterAT);
                        }

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
            return new npc_eonar_the_paraxisAI(p_Creature);
        }
};

/// Spear of Doom - 125319
class npc_eonar_spear_of_doom : public CreatureScript
{
    public:
        npc_eonar_spear_of_doom() : CreatureScript("npc_eonar_spear_of_doom") { }

        enum eSpells
        {
            SpellSpearOfDoomAura    = 248789,
            SpellDummyFixate        = 259304
        };

        struct npc_eonar_spear_of_doomAI : public ScriptedAI
        {
            npc_eonar_spear_of_doomAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetInCombatWithZone();

                p_Summoner->CastSpell(me, eSpells::SpellSpearOfDoomAura, true);

                if (Creature* l_Paraxis = p_Summoner->ToCreature())
                {
                    if (l_Paraxis->IsAIEnabled)
                    {
                        if (Player* l_Target = Player::GetPlayer(*me, l_Paraxis->AI()->GetData64()))
                            DoCast(l_Target, eSpells::SpellDummyFixate, true);
                    }
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eonar_spear_of_doomAI(p_Creature);
        }
};

/// Fel-Infused Destructor - 123760
class npc_eonar_fel_infused_destructor : public CreatureScript
{
    public:
        npc_eonar_fel_infused_destructor() : CreatureScript("npc_eonar_fel_infused_destructor") { }

        enum eSpells
        {
            SpellArtilleryMode      = 246301,
            SpellArtilleryStrike    = 246305
        };

        enum eEvent
        {
            EventArtilleryStrike = 1
        };

        struct npc_eonar_fel_infused_destructorAI : public ScriptedAI
        {
            npc_eonar_fel_infused_destructorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool IsPassived() override
            {
                return me->GetReactState() == ReactStates::REACT_PASSIVE;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SLEEP, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_KNOCKOUT, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SNARE, true);

                me->DisableHealthRegen();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 3);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->InterruptNonMeleeSpells(true);

                me->RemoveAura(eSpells::SpellArtilleryMode);

                events.ScheduleEvent(eEvent::EventArtilleryStrike, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                me->CombatStop();

                me->NearTeleportTo(me->GetHomePosition());

                LastWPReached();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void LastWPReached() override
            {
                me->SetHomePosition(me->GetPosition());

                SetCombatMovement(false);

                me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                DoCast(me, eSpells::SpellArtilleryMode, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (IsPassived() || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (Unit* l_Target = me->getVictim())
                {
                    if (!l_Target->IsWithinMeleeRange(me))
                    {
                        EnterEvadeMode();
                        return;
                    }
                }

                if (events.ExecuteEvent() == eEvent::EventArtilleryStrike)
                {
                    DoCast(me, eSpells::SpellArtilleryStrike);

                    events.ScheduleEvent(eEvent::EventArtilleryStrike, 10 * TimeConstants::IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eonar_fel_infused_destructorAI(p_Creature);
        }
};

/// Felguard - 123451
/// Fel Hound - 123191
/// Fel Lord - 123452
/// Fel-Charged Obfuscator - 124207
/// Volant Kerapteron - 124227
class npc_eonar_legion_attacking_forces : public CreatureScript
{
    public:
        npc_eonar_legion_attacking_forces() : CreatureScript("npc_eonar_legion_attacking_forces") { }

        struct npc_eonar_legion_attacking_forcesAI : public ScriptedAI
        {
            npc_eonar_legion_attacking_forcesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_CanAttack = false;

            bool IsPassived() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void LastWPReached() override
            {
                m_CanAttack = true;

                if (Unit* l_Eonar = me->GetAnyOwner())
                    AttackStart(l_Eonar);
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* /*p_SpellInfo*/) override
            {
                /// Legion forces always deal 6M damage to Eonar
                if (p_Victim->GetEntry() == eCreatures::BossEssenceOfEonar && p_DamageType == DamageEffectType::DIRECT_DAMAGE)
                {
                    p_Damage = 6000000;
                    return;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_CanAttack)
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eonar_legion_attacking_forcesAI(p_Creature);
        }
};

/// Fel-Powered Purifier - 123726
class npc_eonar_fel_powered_purifier : public CreatureScript
{
    public:
        npc_eonar_fel_powered_purifier() : CreatureScript("npc_eonar_fel_powered_purifier") { }

        enum eSpells
        {
            SpellFelWake        = 248796,
            SpellPurifiier      = 246745,
            SpellPurification   = 250073,
            SpellHighAlert      = 246233,
            SpellFelShielding   = 250555,

            SpellFelSwipe       = 250701
        };

        enum eEvent
        {
            EventFelSwipe = 1
        };

        struct npc_eonar_fel_powered_purifierAI : public ScriptedAI
        {
            npc_eonar_fel_powered_purifierAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_OnAlert = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellFelWake, true);
                DoCast(me, eSpells::SpellPurifiier, true);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    m_OnAlert = true;

                    DoCast(me, eSpells::SpellPurification, true);

                    me->SetReactState(ReactStates::REACT_DEFENSIVE);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvent::EventFelSwipe, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void LastWPReached() override
            {
                if (Unit* l_Eonar = me->GetAnyOwner())
                    AttackStart(l_Eonar);
            }

            void DamageTaken(Unit* p_Attacker, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInf*/o) override
            {
                if (!m_OnAlert)
                    return;

                AttackStart(p_Attacker);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                m_OnAlert = false;

                me->RemoveAura(eSpells::SpellHighAlert);
                me->RemoveAura(eSpells::SpellFelShielding);
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* /*p_SpellInfo*/) override
            {
                /// Legion forces always deal 6M damage to Eonar
                if (p_Victim->GetEntry() == eCreatures::BossEssenceOfEonar && p_DamageType == DamageEffectType::DIRECT_DAMAGE)
                {
                    p_Damage = 6000000;
                    return;
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

                if (events.ExecuteEvent() == eEvent::EventFelSwipe)
                {
                    DoCast(me, eSpells::SpellFelSwipe);

                    events.ScheduleEvent(eEvent::EventFelSwipe, 10 * TimeConstants::IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eonar_fel_powered_purifierAI(p_Creature);
        }
};

/// Orb of Life - 128088
class npc_eonar_orb_of_life : public CreatureScript
{
    public:
        npc_eonar_orb_of_life() : CreatureScript("npc_eonar_orb_of_life") { }

        enum eSpell
        {
            SpellOrbOfLifeAT = 253754
        };

        struct npc_eonar_orb_of_lifeAI : public Scripted_NoMovementAI
        {
            npc_eonar_orb_of_lifeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpell::SpellOrbOfLifeAT, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eonar_orb_of_lifeAI(p_Creature);
        }
};

/// Focusing Crystal - 125917
/// Focusing Crystal - 125918
/// Focusing Crystal - 125919
/// Focusing Crystal - 125920
class npc_eonar_focusing_crystal : public CreatureScript
{
    public:
        npc_eonar_focusing_crystal() : CreatureScript("npc_eonar_focusing_crystal") { }

        enum eSpells
        {
            SpellShatter                    = 250083,
            SpellEmpowermentYellow          = 249053,
            SpellEmpowermentRed             = 249054,
            SpellEmpowermentGreen           = 249055,
            SpellEmpowermentBlue            = 249056,
            SpellFeedbackFoulSteps          = 249014,
            SpellFeedbackBurningEmbers      = 249015,
            SpellFeedbackTargeted           = 249016,
            SpellFeedbackArcaneSingularity  = 249017
        };

        enum eGuidData
        {
            GuidFocusingCrystal = 1
        };

        struct npc_eonar_focusing_crystalAI : public Scripted_NoMovementAI
        {
            npc_eonar_focusing_crystalAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_Shattered = false;

            std::map<uint32, uint32> m_CrystalFeedbacks =
            {
                { eSpells::SpellEmpowermentYellow,  eSpells::SpellFeedbackFoulSteps           },
                { eSpells::SpellEmpowermentRed,     eSpells::SpellFeedbackBurningEmbers       },
                { eSpells::SpellEmpowermentGreen,   eSpells::SpellFeedbackTargeted            },
                { eSpells::SpellEmpowermentBlue,    eSpells::SpellFeedbackArcaneSingularity   }
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                Talk(0);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellShatter:
                    {
                        if (m_Shattered)
                            break;

                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (!l_Instance)
                            break;

                        Creature* l_Eonar = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossEssenceOfEonar));
                        if (!l_Eonar || !l_Eonar->IsAIEnabled)
                            break;

                        Creature* l_Paraxis = Creature::GetCreature(*me, l_Eonar->AI()->GetData64());
                        if (!l_Paraxis || !l_Paraxis->IsAIEnabled)
                            break;

                        for (auto const& l_Iter : m_CrystalFeedbacks)
                        {
                            if (!me->HasAura(l_Iter.first))
                                continue;

                            me->RemoveAllAuras();

                            m_Shattered = true;

                            me->InterruptNonMeleeSpells(true);

                            l_Paraxis->AI()->SetGUID(me->GetGUID(), eGuidData::GuidFocusingCrystal);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_INTERACT_SPELL_ID, 0);

                            l_Paraxis->CastSpell(p_Caster, l_Iter.second, true);
                            break;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_eonar_focusing_crystalAI(p_Creature);
        }
};

/// Paraxis Inquisitor - 125429
class npc_eonar_paraxis_inquisitor : public CreatureScript
{
    public:
        npc_eonar_paraxis_inquisitor() : CreatureScript("npc_eonar_paraxis_inquisitor") { }

        enum eSpells
        {
            /// Misc
            SpellShatterCrystal     = 250081,
            /// Mind Blast
            SpellMindBlast          = 249196,
            /// Death Throes
            SpellDeathThroesAura    = 249148,
            /// Wrack
            SpellWrackAura          = 249126,
            /// Torment
            SpellTormentChannel     = 249155
        };

        enum eEvents
        {
            EventMindBlast = 1,
            EventDeathThroes,
            EventWrack,
            EventTorment
        };

        enum eMythicData
        {
            MythicTeleporterCount = 4
        };

        struct npc_eonar_paraxis_inquisitorAI : public Scripted_NoMovementAI
        {
            npc_eonar_paraxis_inquisitorAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            bool m_DeathThroes = false;
            bool m_Tormenting = false;

            std::array<uint32, eMythicData::MythicTeleporterCount> m_ParaxisCrystals =
            {
                {
                    eCreatures::NpcParaxisFocusingCrystal1,
                    eCreatures::NpcParaxisFocusingCrystal2,
                    eCreatures::NpcParaxisFocusingCrystal3,
                    eCreatures::NpcParaxisFocusingCrystal4
                }
            };

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.Reset();

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 1);

                events.ScheduleEvent(eEvents::EventMindBlast, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventWrack, 11 * TimeConstants::IN_MILLISECONDS);

                m_DeathThroes = false;
                m_Tormenting = false;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);

                if (m_Instance)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    Creature* l_Eonar = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::BossEssenceOfEonar));
                    if (!l_Eonar || !l_Eonar->IsAIEnabled)
                        return;

                    Creature* l_Paraxis = Creature::GetCreature(*me, l_Eonar->AI()->GetData64());
                    if (!l_Paraxis || !l_Paraxis->IsAIEnabled)
                        return;

                    for (uint32 const& l_Entry : m_ParaxisCrystals)
                    {
                        if (Creature* l_Crystal = Creature::GetCreature(*me, l_Paraxis->AI()->GetData64(l_Entry)))
                        {
                            l_Crystal->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                            l_Crystal->SetUInt32Value(EUnitFields::UNIT_FIELD_INTERACT_SPELL_ID, eSpells::SpellShatterCrystal);
                        }
                    }
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_DeathThroes && me->HealthBelowPctDamaged(50, p_Damage))
                {
                    m_DeathThroes = true;

                    events.ScheduleEvent(eEvents::EventDeathThroes, 1);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (Unit* l_Victim = me->getVictim())
                {
                    if (!m_Tormenting && !me->IsWithinMeleeRange(l_Victim))
                    {
                        m_Tormenting = true;

                        DoCast(me, eSpells::SpellTormentChannel);
                    }
                    else if (m_Tormenting && me->IsWithinMeleeRange(l_Victim))
                    {
                        m_Tormenting = false;

                        me->InterruptNonMeleeSpells(true, eSpells::SpellTormentChannel);
                    }
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventMindBlast:
                    {
                        DoCast(me, eSpells::SpellMindBlast);

                        events.ScheduleEvent(eEvents::EventMindBlast, 8 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventDeathThroes:
                    {
                        DoCast(me, eSpells::SpellDeathThroesAura, true);
                        break;
                    }
                    case eEvents::EventWrack:
                    {
                        DoCast(me, eSpells::SpellWrackAura);

                        events.ScheduleEvent(eEvents::EventWrack, 15 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_eonar_paraxis_inquisitorAI(p_Creature);
        }
};

/// Artillery Mode - 246301
class spell_eonar_artillery_mode : public SpellScriptLoader
{
    public:
        spell_eonar_artillery_mode() : SpellScriptLoader("spell_eonar_artillery_mode") { }

        enum eSpell
        {
            SpellArtilleryStrike = 246305
        };

        class spell_eonar_artillery_mode_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_artillery_mode_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, eSpell::SpellArtilleryStrike, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_eonar_artillery_mode_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_artillery_mode_AuraScript();
        }
};

/// Artillery Strike - 246310
class spell_eonar_artillery_strike : public SpellScriptLoader
{
    public:
        spell_eonar_artillery_strike() : SpellScriptLoader("spell_eonar_artillery_strike") { }

        class spell_eonar_artillery_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_eonar_artillery_strike_SpellScript);

            void DealDamage(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                        SetHitDamage(l_Caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[p_EffIndex].BasePoints));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_eonar_artillery_strike_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_eonar_artillery_strike_SpellScript();
        }
};

/// Shatter - 250081
class spell_eonar_shatter : public SpellScriptLoader
{
    public:
        spell_eonar_shatter() : SpellScriptLoader("spell_eonar_shatter") { }

        enum eSpells
        {
            SpellShatter                    = 250083,

            SpellFeedbackFoulSteps          = 249014,
            SpellFeedbackBurningEmbers      = 249015,
            SpellFeedbackTargeted           = 249016,
            SpellFeedbackArcaneSingularity  = 249017
        };

        class spell_eonar_shatter_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_shatter_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, eSpells::SpellShatter, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_eonar_shatter_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_shatter_AuraScript();
        }

        class spell_eonar_shatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_eonar_shatter_SpellScript);

            SpellCastResult CheckSoul()
            {
                std::vector<uint32> l_Auras =
                {
                    eSpells::SpellFeedbackFoulSteps,
                    eSpells::SpellFeedbackBurningEmbers,
                    eSpells::SpellFeedbackTargeted,
                    eSpells::SpellFeedbackArcaneSingularity
                };

                if (Unit* l_Caster = GetCaster())
                {
                    for (uint32 const& l_SpellID : l_Auras)
                    {
                        if (l_Caster->HasAura(l_SpellID))
                            return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_eonar_shatter_SpellScript::CheckSoul);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_eonar_shatter_SpellScript();
        }
};

/// Feedback - Foul Steps - 249014
class spell_eonar_feedback_foul_steps : public SpellScriptLoader
{
    public:
        spell_eonar_feedback_foul_steps() : SpellScriptLoader("spell_eonar_feedback_foul_steps") { }

        enum eSpells
        {
            SpellFoulStepsDamage    = 250139,
            SpellFoulStepsDebuff    = 250140
        };

        class spell_eonar_feedback_foul_steps_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_feedback_foul_steps_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->IsFlying() && !l_Target->IsFalling() && l_Target->IsMoving())
                    {
                        l_Target->CastSpell(l_Target, eSpells::SpellFoulStepsDamage, true);
                        l_Target->CastSpell(l_Target, eSpells::SpellFoulStepsDebuff, true);
                    }
                    else if (Aura* l_Aura = l_Target->GetAura(eSpells::SpellFoulStepsDebuff))
                    {
                        l_Aura->RefreshDuration();
                        l_Aura->DropStack();
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_eonar_feedback_foul_steps_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_feedback_foul_steps_AuraScript();
        }
};

/// Feedback - Burning Embers - 249015
class spell_eonar_feedback_burning_embers : public SpellScriptLoader
{
    public:
        spell_eonar_feedback_burning_embers() : SpellScriptLoader("spell_eonar_feedback_burning_embers") { }

        enum eSpell
        {
            SpellBurningEmbersAura = 250691
        };

        class spell_eonar_feedback_burning_embers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_feedback_burning_embers_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, eSpell::SpellBurningEmbersAura, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_eonar_feedback_burning_embers_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_feedback_burning_embers_AuraScript();
        }
};

/// Burning Embers - 250691
class spell_eonar_burning_embers : public SpellScriptLoader
{
    public:
        spell_eonar_burning_embers() : SpellScriptLoader("spell_eonar_burning_embers") { }

        enum eSpell
        {
            SpellBurningEmbersAoE = 250128
        };

        class spell_eonar_burning_embers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_burning_embers_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellBurningEmbersAoE, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_eonar_burning_embers_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_burning_embers_AuraScript();
        }
};

/// Feedback - Arcane Singularity - 249017
class spell_eonar_feedback_arcane_singularity : public SpellScriptLoader
{
    public:
        spell_eonar_feedback_arcane_singularity() : SpellScriptLoader("spell_eonar_feedback_arcane_singularity") { }

        enum eSpell
        {
            SpellArcaneBurst = 250171
        };

        class spell_eonar_feedback_arcane_singularity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_feedback_arcane_singularity_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellArcaneBurst, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_eonar_feedback_arcane_singularity_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_feedback_arcane_singularity_AuraScript();
        }
};

/// Surge of Life - 245781
/// Surge of Life - 253874
class spell_eonar_surge_of_life : public SpellScriptLoader
{
    public:
        spell_eonar_surge_of_life() : SpellScriptLoader("spell_eonar_surge_of_life") { }

        enum eSpells
        {
            SpellEssenceOfTheLifebinderFight    = 245764,
            SpellEssenceOfTheLifebinder         = 253875
        };

        class spell_eonar_surge_of_life_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_eonar_surge_of_life_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->isDead())
                        return SpellCastResult::SPELL_FAILED_CASTER_DEAD;

                    if (l_Caster->IsNearPosition(&g_InquisitorPos, 130.0f))
                    {
                        if (Creature* l_Inquisitor = l_Caster->FindNearestCreature(eCreatures::NpcParaxisInquisitor, 130.0f))
                            return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleLaunch(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::SpellEssenceOfTheLifebinderFight);
                l_Caster->RemoveAura(eSpells::SpellEssenceOfTheLifebinder);

                Position l_Pos = l_Caster->GetPosition();

                if (float l_MaxZ = l_Caster->GetMap()->GetHeight(l_Caster->GetPhaseMask(), l_Caster->m_positionX, l_Caster->m_positionY, l_Caster->m_positionZ + 5.0f, true, 50.0f, true))
                {
                    if ((l_Pos.m_positionZ + 1.0f) < l_MaxZ)
                    {
                        if ((l_Pos.m_positionZ + 50.0f) < l_MaxZ)
                            l_Pos.m_positionZ += 50.0f;
                        else
                            l_Pos.m_positionZ = l_MaxZ - 2.0f;
                    }
                    else
                        l_Pos.m_positionZ += 50.0f;
                }

                l_Caster->GetMotionMaster()->Clear();
                l_Caster->GetMotionMaster()->MoveJump(l_Pos, 62.5f, 19.64138141f, 0, eSharedSpells::SpellSurgeOfLifeSecond);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_eonar_surge_of_life_SpellScript::HandleCheckCast);
                OnEffectLaunch += SpellEffectFn(spell_eonar_surge_of_life_SpellScript::HandleLaunch, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_JUMP_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_eonar_surge_of_life_SpellScript();
        }
};

/// Surge of Life (down) - 245786
class spell_eonar_surge_of_life_fall : public SpellScriptLoader
{
    public:
        spell_eonar_surge_of_life_fall() : SpellScriptLoader("spell_eonar_surge_of_life_fall") { }

        enum eSpell
        {
            SpellForwardThrust = 259065
        };

        class spell_eonar_surge_of_life_fall_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_eonar_surge_of_life_fall_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::SurgeOfLifeMovement) = true;

                l_Caster->DelayedCastSpell(l_Caster, eSpell::SpellForwardThrust, true, 200);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_eonar_surge_of_life_fall_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_eonar_surge_of_life_fall_SpellScript();
        }
};

/// Cloak - 246748
class spell_eonar_cloak_aura : public SpellScriptLoader
{
    public:
        spell_eonar_cloak_aura() : SpellScriptLoader("spell_eonar_cloak_aura") { }

        class spell_eonar_cloak_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_cloak_aura_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_eonar_cloak_aura_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_eonar_cloak_aura_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_cloak_aura_AuraScript();
        }
};

/// Persistance - 252749
class spell_eonar_persistance : public SpellScriptLoader
{
    public:
        spell_eonar_persistance() : SpellScriptLoader("spell_eonar_persistance") { }

        class spell_eonar_persistance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eonar_persistance_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                /// Second wave (and more) start spawning with base health increased by 10%
                p_Amount = 10;

                if (Unit* l_Caster = GetCaster())
                {
                    if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                    {
                        uint32 l_Time = l_Instance->GetEncounterTime();
                        if (!l_Time)
                            return;

                        /// Amount increased by 10% more for each 40 seconds passed during the encounter
                        p_Amount += uint32(l_Time % 40);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_eonar_persistance_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eonar_persistance_AuraScript();
        }
};

/// Jump Pad - 248222
class areatrigger_eonar_jump_pad : public AreaTriggerEntityScript
{
    public:
        areatrigger_eonar_jump_pad() : AreaTriggerEntityScript("areatrigger_eonar_jump_pad") { }

        InstanceScript* m_Instance = nullptr;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return true;

            if (!m_Instance)
                m_Instance = p_Target->GetInstanceScript();

            if (!m_Instance)
                return true;

            Creature* l_Eonar = Creature::GetCreature(*p_Target, m_Instance->GetData64(eCreatures::BossEssenceOfEonar));
            if (!l_Eonar || !l_Eonar->IsAIEnabled)
                return true;

            if (Creature* l_Dest = Creature::GetCreature(*p_Target, l_Eonar->AI()->GetData64(p_AreaTrigger->GetCaster()->GetGUIDLow())))
                p_Target->CastSpell(l_Dest, eSharedSpells::SpellJumpOnPad, true);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_eonar_jump_pad();
        }
};

/// Orb of Life - 253754
class areatrigger_eonar_orb_of_life : public AreaTriggerEntityScript
{
    public:
        areatrigger_eonar_orb_of_life() : AreaTriggerEntityScript("areatrigger_eonar_orb_of_life") { }

        enum eAction
        {
            ActionOrbOfLifeSequence = 6
        };

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return true;

            p_AreaTrigger->Remove();

            InstanceScript* l_Instance = p_Target->GetInstanceScript();
            if (!l_Instance)
                return true;

            if (Creature* l_Eonar = Creature::GetCreature(*p_Target, l_Instance->GetData64(eCreatures::BossEssenceOfEonar)))
            {
                if (!l_Eonar->IsAIEnabled)
                    return true;

                l_Eonar->AI()->DoAction(eAction::ActionOrbOfLifeSequence);
            }

            if (Creature* l_Orb = p_AreaTrigger->GetCaster()->ToCreature())
                l_Orb->DespawnOrUnsummon();

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_eonar_orb_of_life();
        }
};

/// Teleporter - 249094
class areatrigger_eonar_teleporter : public AreaTriggerEntityScript
{
    public:
        areatrigger_eonar_teleporter() : AreaTriggerEntityScript("areatrigger_eonar_teleporter") { }

        bool m_Teleported = false;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (m_Teleported || !p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return true;

            m_Teleported = true;

            p_AreaTrigger->Remove();

            InstanceScript* l_Instance = p_Target->GetInstanceScript();
            if (!l_Instance)
                return true;

            if (Creature* l_Paraxis = Creature::GetCreature(*p_Target, l_Instance->GetData64(eCreatures::NpcTheParaxis)))
            {
                if (!l_Paraxis->IsAIEnabled)
                    return true;

                l_Paraxis->AI()->SetGUID(p_Target->GetGUID());
            }

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_eonar_teleporter();
        }
};

class playerscript_eonar_surge_of_life_fall : public PlayerScript
{
    public:
        playerscript_eonar_surge_of_life_fall() : PlayerScript("playerscript_eonar_surge_of_life_fall") { }

        enum eSpells
        {
            SpellSurgeOfLifeDown        = 254506,
            SpellEssenceOfTheLifebinder = 245764
        };

        void OnUpdateMovement(Player* p_Player, uint16 p_OpcodeID) override
        {
            if (!p_Player || p_Player->GetMapId() != 1712 || p_OpcodeID != Opcodes::CMSG_MOVE_FALL_LAND)
                return;

            p_Player->RemoveAura(eSharedSpells::SpellSurgeOfLifeSecond);
            p_Player->RemoveAura(eSpells::SpellSurgeOfLifeDown);

            if (p_Player->m_SpellHelper.GetBool(eSpellHelpers::SurgeOfLifeMovement))
            {
                p_Player->m_SpellHelper.GetBool(eSpellHelpers::SurgeOfLifeMovement) = false;

                p_Player->RemoveAura(eSpells::SpellEssenceOfTheLifebinder);

                p_Player->DelayedCastSpell(p_Player, eSpells::SpellEssenceOfTheLifebinder, true, 100);
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_eonar_the_life_binder()
{
    /// Boss
    new boss_essence_of_eonar();

    /// NPCs
    new npc_eonar_the_paraxis();
    new npc_eonar_spear_of_doom();
    new npc_eonar_fel_infused_destructor();
    new npc_eonar_legion_attacking_forces();
    new npc_eonar_fel_powered_purifier();
    new npc_eonar_orb_of_life();
    new npc_eonar_focusing_crystal();
    new npc_eonar_paraxis_inquisitor();

    /// Spells
    new spell_eonar_artillery_mode();
    new spell_eonar_artillery_strike();
    new spell_eonar_shatter();
    new spell_eonar_feedback_foul_steps();
    new spell_eonar_feedback_burning_embers();
    new spell_eonar_burning_embers();
    new spell_eonar_feedback_arcane_singularity();
    new spell_eonar_surge_of_life();
    new spell_eonar_surge_of_life_fall();
    new spell_eonar_cloak_aura();
    new spell_eonar_persistance();

    /// AreaTrigger
    new areatrigger_eonar_jump_pad();
    new areatrigger_eonar_orb_of_life();
    new areatrigger_eonar_teleporter();

    /// PlayerScript
    new playerscript_eonar_surge_of_life_fall();
}
#endif
