////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "boss_tov_helya.hpp"

/// Helya - 114537
class boss_tov_helya : public CreatureScript
{
    public:
        boss_tov_helya() : CreatureScript("boss_tov_helya") { }

        struct boss_tov_helyaAI : public BossAI
        {
            boss_tov_helyaAI(Creature* p_Creature) : BossAI(p_Creature, eTovData::DataBossHelya)
            {
                m_IntroDone = false;
                m_TrashesDone = false;
            }

            enum eTalks
            {
                TalkIntro1,
                TalkIntro2,
                TalkIntro3,
                TalkAggro,
                TalkBilewaterBreath,
                TalkOrbOfCorruption,
                TalkTentacleStrike,
                TalkPhase2,
                TalkSlay,
                TalkWipe,
                TalkOrbOfCorruptionWarn,
                TalkDeath,
                TalkFuryOfTheMaw,
                TalkPhase3,
                TalkOrbOfCorrosion,
                TalkOrbOfCorrosionWarn,
                TalkCorruptedBreath
            };

            enum eEvents
            {
                /// Phase 1
                EventBilewaterBreath = 1,
                EventCorrosiveNova,
                EventOrbOfCorruption,
                EventTentacleStrike,
                EventTaintOfTheSea,
                /// Phase 2
                EventTorrent,
                EventFuryOfTheMaw,
                EventDecayingMinions,
                EventSpawnMinion1,
                EventSpawnMinion2,
                /// Phase 3
                EventOrbOfCorrosion,
                EventCorruptedBreath
            };

            enum eSpells
            {
                /// Misc
                SpellBerserker                          = 64238,
                SpellHelyaPower                         = 232229,
                SpellHelyaPowerP2                       = 231269,
                SpellHelyaPowerP3                       = 228546,
                SpellHelyaCosmeticPowerP3               = 232230,
                SpellReanimationCosmetic1               = 234199,
                SpellReanimationCosmetic2               = 228104,
                SpellREUSE                              = 200239,
                SpellHelyaDamageImmunity                = 228847,
                SpellMistsOfHelheim                     = 228372,
                SpellDecay                              = 230195,
                SpellDecayAT                            = 228121,
                SpellFetidRot                           = 193367,
                SpellMistcasterSummon                   = 228125,
                SpellMistInfusionCast                   = 228854,
                SpellBleakEruption                      = 231862,
                SpellDarkWaters                         = 230197,
                /// Phase 1
                /// Corrosive Nova
                SpellCorrosiveNova                      = 228872,
                /// Bilewater Breath
                SpellBilewaterBreathCast                = 227967,
                SpellBilewaterDamage                    = 230229,
                SpellBilewaterRedox                     = 227982,
                SpellBilewaterBreathSummon              = 230216,
                SpellBilewaterLiquefactionDummy         = 227993,
                SpellBilewaterLiquefactionAoE           = 227992,
                /// Taint of the Sea
                SpellTaintOfTheSeaAura                  = 228054,
                /// Orb of Corruption
                SpellOrbOfCorruptionCast                = 227903,
                SpellOrbOfCorruptionDummyVisual         = 227906,
                SpellOrbOfCorruptionDummyCast           = 227920,
                SpellOrbOfCorruptionPlayerAura          = 232497,
                SpellOrbOfCorruptionRayVisual           = 227939,
                SpellOrbOfCorruptionTrigger             = 227938,
                SpellOrbOfCorruptionAoE                 = 227930,
                SpellOrbOfCorruptionAura                = 229119,
                SpellCorruptedSlicer                    = 232162,
                SpellCorruptedSlicerChannel             = 157932,
                /// Phase 2
                /// Torrent
                SpellTorrent                            = 228514,
                /// Fury of the Maw
                SpellFuryOfTheMawAT                     = 228027,
                SpellFuryOfTheMawChanneled              = 228300,
                /// Phase 3
                SpellFuryOfTheMawCast                   = 228032,
                SpellFuryOfTheMawDummy                  = 230356,
                /// Orb of Corrosion
                SpellOrbOfCorrosionCast                 = 228056,
                SpellOrbOfCorrosionDummyVisual          = 228057,
                SpellOrbOfCorrosionDummyCast            = 228058,
                SpellOrbOfCorrosionPlayerAura           = 232496,
                SpellOrbOfCorrosionRayVisual            = 228060,
                SpellOrbOfCorrosionTrigger              = 228062,
                SpellOrbOfCorrosionAoE                  = 228063,
                SpellOrbOfCorrosionAura                 = 230267,
                SpellCorrosiveSlicerChannel             = 231420,
                /// Corrupted Breath
                SpellCorruptedBreathCast                = 228565,
                SpellCorruptedBreathMissile             = 232418,
                SpellCorruptedBreathDamage              = 228566,
                SpellCorruptedAxionDmg                  = 232450,
                SpellCorruptedAxionAoE                  = 232452,
                SpellCorrupted                          = 232350,
                SpellDarkHatred                         = 232488,
                SpellCorruptedTransformFemale           = 232358,
                SpellCorruptedTransformMale             = 232364
            };

            enum ePhases
            {
                PhaseNone,
                Phase01,
                Phase02,
                Phase03
            };

            enum eActions
            {
                ActionFullEnergy,
                ActionFullAlternateEnergy
            };

            enum eVisuals
            {
                VisualOrbOfCorruption   = 55631,
                VisualRitualStone       = 12151,
                VisualOrbOfCorrosion    = 54100
            };

            uint64 m_BilewaterTankGuid;
            std::queue<float> m_BilewaterPcts;

            bool m_OrbAtRange = false;
            uint64 m_OrbOfCorruptionGuid;

            uint8 m_PhaseId;

            bool m_SecondPhaseTriggered = false;

            bool m_IntroDone;
            bool m_TrashesDone;

            std::set<uint64> m_AlliesGuids;

            uint32 m_CheckPlayerTimer = 0;
            uint32 m_CheckVictimTimer = 0;
            uint32 m_CosmeticMistTimer = 0;

            uint8 m_TaintedCount;

            std::array<uint64, eHelyaData::MaxGrippingTentacle> m_GrippingTentacles;

            uint32 m_LastSlayTalkTimer = 0;
            uint32 m_BerserkerTimer = 0;

            std::array<uint8, eHelyaData::MaxSides> m_GrippingTentacleIDx;

            uint8 m_TentacleCount = 0;
            uint8 m_TentaclesKilled = 0;

            int32 m_InfusionEnergized = 0;

            uint32 m_CheckWaterTimer = 0;

            std::map<uint64, uint8> m_HealersAffectedByOrb;

            uint8 m_MistwatcherIDx = 0;

            EventMap m_SummonEvents;

            void Reset() override
            {
                _Reset();

                events.Reset();
                ClearDelayedOperations();

                m_SummonEvents.Reset();

                m_PhaseId = ePhases::PhaseNone;

                m_SecondPhaseTriggered = false;

                m_BilewaterTankGuid = 0;

                m_OrbAtRange = true;
                m_OrbOfCorruptionGuid = 0;

                m_TaintedCount = 0;

                me->RemoveAura(eSpells::SpellBerserker);

                me->SetPower(Powers::POWER_ENERGY, 0);

                SetCombatMovement(false);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_31);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN | eUnitFlags2::UNIT_FLAG2_UNK14);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                if (!m_IntroDone)
                    m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                if (!m_TrashesDone)
                {
                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        std::list<Creature*> l_Allies;

                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureDarkSeraph, 100.0f);
                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureKvaldirTideWitch, 100.0f);
                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureDeepbrineMonstruosity, 100.0f);
                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureKvaldirReefcaller, 100.0f);
                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureAncientBonethrall, 100.0f);
                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureRotsoulGiant, 100.0f);
                        me->GetCreatureListWithEntryInGrid(l_Allies, eTovCreatures::CreatureKvaldirCoralMaiden, 100.0f);

                        if (!l_Allies.empty())
                        {
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            me->SetReactState(ReactStates::REACT_PASSIVE);
                        }

                        for (Creature* l_Ally : l_Allies)
                        {
                            if (l_Ally->isAlive())
                                m_AlliesGuids.insert(l_Ally->GetGUID());
                        }
                    });
                }

                m_CosmeticMistTimer = 20 * TimeConstants::IN_MILLISECONDS;

                m_LastSlayTalkTimer = 0;
                m_BerserkerTimer = 0;

                for (uint8& l_IDx : m_GrippingTentacleIDx)
                    l_IDx = 0;

                m_TentaclesKilled = 0;

                m_InfusionEnergized = 0;

                m_CheckWaterTimer = 0;

                m_HealersAffectedByOrb.clear();

                m_MistwatcherIDx = 0;

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->SummonCreature(eTovCreatures::CreatureGraspingTentacleLeft, g_LeftTentaclePos);
                    me->SummonCreature(eTovCreatures::CreatureGraspingTentacleRight, g_RightTentaclePos);
                });

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(eTalks::TalkAggro);

                _EnterCombat();

                me->RemoveAllAreasTrigger();

                m_CheckVictimTimer = 1 * TimeConstants::IN_MILLISECONDS;

                DefaultEvents(ePhases::Phase01);

                m_BerserkerTimer = 11 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS;

                m_CosmeticMistTimer = 0;

                m_CheckWaterTimer = 500;

                DoCast(me, eSpells::SpellHelyaPower);

                me->SetPower(Powers::POWER_ENERGY, 79);

                uint8 l_I = 0;

                for (Position const& l_Pos : g_GrippingTentaclePos)
                {
                    if (Creature* l_Gripping = me->SummonCreature(eTovCreatures::CreatureGrippingTentacle, l_Pos))
                        m_GrippingTentacles[l_I++] = l_Gripping->GetGUID();
                }
            }

            void DefaultEvents(uint8 p_PhaseID)
            {
                m_PhaseId = p_PhaseID;

                m_OrbAtRange = true;

                switch (m_PhaseId)
                {
                    case ePhases::Phase01:
                    {
                        m_TentacleCount = 0;

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventOrbOfCorruption, 14 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTentacleStrike, 35 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTaintOfTheSea, 15 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (IsHeroic())
                        {
                            events.ScheduleEvent(eEvents::EventOrbOfCorruption, 29 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTentacleStrike, 36 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTaintOfTheSea, 19 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventOrbOfCorruption, 18 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTentacleStrike, 53 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTaintOfTheSea, 12 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case ePhases::Phase02:
                    {
                        events.ScheduleEvent(eEvents::EventTorrent, 13 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventDecayingMinions, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case ePhases::Phase03:
                    {
                        m_CheckVictimTimer = 1 * TimeConstants::IN_MILLISECONDS;

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 6 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTaintOfTheSea, 14 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (IsHeroic())
                        {
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 15 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTaintOfTheSea, 25 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 12 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventTaintOfTheSea, 27 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                Talk(eTalks::TalkWipe);

                if (instance)
                    instance->SetBossState(eTovData::DataBossHelya, EncounterState::FAIL);

                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->InterruptNonMeleeSpells(true);

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (instance)
                {
                    instance->SetBossState(eTovData::DataBossHelya, EncounterState::FAIL);

                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellBilewaterRedox);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellOrbOfCorrosionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellOrbOfCorruptionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorruptedAxionDmg);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFetidRot);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorrupted);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorruptedTransformFemale);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorruptedTransformMale);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellBleakEruption);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDarkWaters);
                }
            }

            bool CanBeHitInTheBack() override
            {
                return false;
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 5.0f;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_31);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN | eUnitFlags2::UNIT_FLAG2_UNK14);

                Talk(eTalks::TalkDeath);

                _JustDied();

                if (instance)
                {
                    bool l_AchievementCheck = true;

                    Map::PlayerList const& l_Players = instance->instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (!l_Player->HasAura(eSpells::SpellFetidRot))
                            {
                                l_AchievementCheck = false;
                                break;
                            }
                        }
                    }

                    if (!IsLFR() && l_AchievementCheck)
                        instance->DoCompleteAchievement(eTovAchievements::PatientZero);

                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellBilewaterRedox);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellOrbOfCorrosionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellOrbOfCorruptionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorruptedAxionDmg);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFetidRot);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorrupted);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorruptedTransformFemale);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCorruptedTransformMale);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellBleakEruption);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDarkWaters);
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer() && (m_LastSlayTalkTimer + 10) <= uint32(time(nullptr)))
                {
                    m_LastSlayTalkTimer = uint32(time(nullptr));

                    Talk(eTalks::TalkSlay);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBilewaterBreathCast:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        if (Unit* l_Target = Unit::GetUnit(*me, m_BilewaterTankGuid))
                        {
                            float l_Angle   = me->GetAngle(l_Target) + frand(-0.175f, 0.175f);
                            float l_Dist    = frand(2.0f, 5.0f);

                            for (uint8 l_I = 0; l_I < 5; ++l_I)
                            {
                                Position l_Pos = l_Target->GetPosition();

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Dist, l_Angle, true);

                                l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, 30.0f);

                                DoCast(l_Pos, eSpells::SpellBilewaterBreathSummon, true);

                                l_Dist += frand(7.5f, 12.5f);
                            }
                        }

                        break;
                    }
                    case eSpells::SpellOrbOfCorruptionCast:
                    {
                        std::list<Player*> l_Targets;

                        std::vector<uint64> l_FilterGuids;

                        for (auto const& l_Iter : m_HealersAffectedByOrb)
                            l_FilterGuids.push_back(l_Iter.first);

                        uint64 l_HealerGuid = 0;

                        /// The Orb of Corruption will always pick the off-tank, a healer, then a ranged dps, in mythic: ranged, then melee, then ranged, then melee, etc...
                        if (m_OrbAtRange)
                        {
                            m_OrbAtRange = !m_OrbAtRange;

                            if (Player* l_Healer = SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer, { }, 0.0f, l_FilterGuids))
                            {
                                l_HealerGuid = l_Healer->GetGUID();
                                l_Targets.push_back(l_Healer);
                            }

                            if (Player* l_Ranged = SelectPlayerTarget(eTargetTypeMask::TypeMaskRanged))
                                l_Targets.push_back(l_Ranged);

                            if (Player* l_OffTank = SelectOffTank())
                                l_Targets.push_back(l_OffTank);
                        }
                        else
                        {
                            m_OrbAtRange = !m_OrbAtRange;

                            if (Player* l_Healer = SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer, { }, 0.0f, l_FilterGuids))
                            {
                                l_HealerGuid = l_Healer->GetGUID();
                                l_Targets.push_back(l_Healer);
                            }

                            if (Player* l_Melee = SelectPlayerTarget(eTargetTypeMask::TypeMaskMelee))
                                l_Targets.push_back(l_Melee);

                            if (Player* l_OffTank = SelectOffTank())
                                l_Targets.push_back(l_OffTank);
                        }

                        /// A healer can't be targeted by the 2 next sets of orbs after he got targeted once
                        if (l_HealerGuid != 0)
                        {
                            if (m_HealersAffectedByOrb.find(l_HealerGuid) == m_HealersAffectedByOrb.end())
                                m_HealersAffectedByOrb[l_HealerGuid] = 0;
                        }

                        /// Increment and remove healers from map if needed
                        for (std::map<uint64, uint8>::iterator l_Iter = m_HealersAffectedByOrb.begin(); l_Iter != m_HealersAffectedByOrb.end();)
                        {
                            /// Don't increment if first set for this healer
                            if (l_HealerGuid && l_Iter->first == l_HealerGuid)
                            {
                                ++l_Iter;
                                continue;
                            }

                            ++l_Iter->second;

                            if (l_Iter->second >= 2)
                                l_Iter = m_HealersAffectedByOrb.erase(l_Iter);
                            else
                                ++l_Iter;
                        }

                        Creature* l_FirstOrb = nullptr;
                        Creature* l_PrevOrb = nullptr;

                        for (Player*& l_Target : l_Targets)
                        {
                            sCreatureTextMgr->SendChat(me, eTalks::TalkOrbOfCorruptionWarn, l_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                            if (Creature* l_Vehicle = l_Target->SummonCreature(eTovCreatures::CreatureOrbOfCorruptionVehicle, l_Target->GetPosition()))
                            {
                                if (!l_FirstOrb)
                                    l_FirstOrb = l_Vehicle;

                                if (!l_PrevOrb)
                                    l_PrevOrb = l_Vehicle;
                                else if (IsMythic())
                                {
                                    l_PrevOrb->CastSpell(l_Vehicle, eSpells::SpellCorruptedSlicerChannel, true);
                                    l_PrevOrb = l_Vehicle;
                                }

                                if (Creature* l_Orb = Creature::GetCreature(*me, m_OrbOfCorruptionGuid))
                                {
                                    l_Orb->SendPlaySpellVisual(eVisuals::VisualOrbOfCorruption, l_Orb, 0.75f, Position(), true);

                                    l_Orb->CastSpell(l_Target, eSpells::SpellOrbOfCorruptionPlayerAura, true);
                                    l_Orb->CastSpell(l_Vehicle, eSpells::SpellOrbOfCorruptionRayVisual, true);
                                }

                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Orb = l_Vehicle->SummonCreature(eTovCreatures::CreatureOrbOfCorruption, l_Target->GetPosition()))
                                    {
                                        uint64 l_VehicleGuid = l_Vehicle->GetGUID();
                                        uint64 l_Guid        = l_Orb->GetGUID();
                                        AddTimedDelayedOperation(10, [this, l_VehicleGuid, l_Guid]() -> void
                                        {
                                            if (Creature* l_Vehicle = Creature::GetCreature(*me, l_VehicleGuid))
                                            {
                                                if (Creature* l_Orb = Creature::GetCreature(*me, l_Guid))
                                                    l_Orb->EnterVehicle(l_Vehicle);
                                            }
                                        });
                                    }
                                }
                            }
                        }

                        if (IsMythic() && l_PrevOrb && l_FirstOrb)
                            l_PrevOrb->CastSpell(l_FirstOrb, eSpells::SpellCorruptedSlicerChannel, true);

                        break;
                    }
                    case eSpells::SpellFuryOfTheMawDummy:
                    {
                        me->CastSpell(g_FuryOfTheMawSpawnPos, eSpells::SpellFuryOfTheMawAT, true);

                        if (m_PhaseId == ePhases::Phase03)
                        {
                            if (Creature* l_Boat = me->SummonCreature(eTovCreatures::CreatureKvaldirLongboat, g_LongboatSpawnPos[0]))
                            {
                                if (l_Boat->IsAIEnabled)
                                {
                                    if (Creature* l_Passenger = me->SummonCreature(eTovCreatures::CreatureNightWatchMariner, l_Boat->GetPosition()))
                                        l_Boat->AI()->SetGUID(l_Passenger->GetGUID());
                                }
                            }

                            me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);
                        }

                        break;
                    }
                    case eSpells::SpellOrbOfCorrosionCast:
                    {
                        std::list<Player*> l_Targets;

                        /// The Orb of Corruption will always pick the off-tank, a healer, then a ranged dps, in mythic: ranged, then melee, then ranged, then melee, etc...
                        if (m_OrbAtRange)
                        {
                            m_OrbAtRange = !m_OrbAtRange;

                            if (Player* l_Healer = SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer))
                                l_Targets.push_back(l_Healer);

                            uint64 l_RangedGuid = 0;
                            if (Player* l_Ranged = SelectPlayerTarget(eTargetTypeMask::TypeMaskRanged))
                            {
                                l_RangedGuid = l_Ranged->GetGUID();
                                l_Targets.push_back(l_Ranged);
                            }

                            if (Player* l_Ranged = SelectPlayerTarget(eTargetTypeMask::TypeMaskRanged, { }, 0.0f, { l_RangedGuid }))
                                l_Targets.push_back(l_Ranged);
                        }
                        else
                        {
                            m_OrbAtRange = !m_OrbAtRange;

                            if (Player* l_Healer = SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer))
                                l_Targets.push_back(l_Healer);

                            uint64 l_MeleeGuid = 0;
                            if (Player* l_Melee = SelectPlayerTarget(eTargetTypeMask::TypeMaskMelee))
                            {
                                l_MeleeGuid = l_Melee->GetGUID();
                                l_Targets.push_back(l_Melee);
                            }

                            if (Player* l_Melee = SelectPlayerTarget(eTargetTypeMask::TypeMaskMelee, { }, 0.0f, { l_MeleeGuid }))
                                l_Targets.push_back(l_Melee);
                        }

                        Creature* l_FirstOrb = nullptr;
                        Creature* l_PrevOrb = nullptr;

                        for (Player*& l_Target : l_Targets)
                        {
                            sCreatureTextMgr->SendChat(me, eTalks::TalkOrbOfCorrosionWarn, l_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                            if (Creature* l_Vehicle = l_Target->SummonCreature(eTovCreatures::CreatureOrbOfCorrosionVehicle, l_Target->GetPosition()))
                            {
                                if (!l_FirstOrb)
                                    l_FirstOrb = l_Vehicle;

                                if (!l_PrevOrb)
                                    l_PrevOrb = l_Vehicle;
                                else if (IsMythic())
                                {
                                    l_PrevOrb->CastSpell(l_Vehicle, eSpells::SpellCorrosiveSlicerChannel, true);
                                    l_PrevOrb = l_Vehicle;
                                }

                                if (Creature* l_Orb = Creature::GetCreature(*me, m_OrbOfCorruptionGuid))
                                {
                                    l_Orb->SendPlaySpellVisual(eVisuals::VisualOrbOfCorrosion, l_Orb, 1.5f, Position(), true);

                                    l_Vehicle->CastSpell(l_Target, eSpells::SpellOrbOfCorrosionPlayerAura, true);
                                    l_Orb->CastSpell(l_Vehicle, eSpells::SpellOrbOfCorrosionRayVisual, true);
                                }

                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Orb = l_Vehicle->SummonCreature(eTovCreatures::CreatureOrbOfCorruption, l_Target->GetPosition()))
                                    {
                                        uint64 l_VehicleGuid = l_Vehicle->GetGUID();
                                        uint64 l_Guid = l_Orb->GetGUID();
                                        AddTimedDelayedOperation(10, [this, l_VehicleGuid, l_Guid]() -> void
                                        {
                                            if (Creature* l_Vehicle = Creature::GetCreature(*me, l_VehicleGuid))
                                            {
                                                if (Creature* l_Orb = Creature::GetCreature(*me, l_Guid))
                                                    l_Orb->EnterVehicle(l_Vehicle);
                                            }
                                        });
                                    }
                                }
                            }
                        }

                        if (IsMythic() && l_PrevOrb && l_FirstOrb)
                            l_PrevOrb->CastSpell(l_FirstOrb, eSpells::SpellCorrosiveSlicerChannel, true);

                        break;
                    }
                    case eSpells::SpellCorruptedBreathCast:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        if (!IsHeroicOrMythic())
                            break;

                        /// Corrupted Breath will send out several bolts within the cone of the breath called  Corrupted Axions
                        if (Unit* l_Target = Unit::GetUnit(*me, m_BilewaterTankGuid))
                        {
                            float l_Angle = me->GetAngle(l_Target);
                            float l_Dist = frand(2.0f, 10.0f);

                            for (uint8 l_I = 0; l_I < 5; ++l_I)
                            {
                                Position l_Pos = l_Target->GetPosition();

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Dist, l_Angle, true);

                                l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, 30.0f);

                                DoCast(l_Pos, eSpells::SpellCorruptedBreathMissile, true);

                                l_Dist += frand(10.0f, 15.0f);
                            }
                        }

                        break;
                    }
                    case eSpells::SpellFuryOfTheMawCast:
                    {
                        m_InfusionEnergized = 0;
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
                    case eSpells::SpellBilewaterDamage:
                    {
                        DoCast(p_Target, eSpells::SpellBilewaterRedox, true);
                        break;
                    }
                    case eSpells::SpellOrbOfCorruptionAoE:
                    case eSpells::SpellOrbOfCorrosionAoE:
                    {
                        DoCast(p_Target, eSpells::SpellCorruptedSlicer, true);
                        break;
                    }
                    case eSpells::SpellCorruptedAxionAoE:
                    {
                        DoCast(p_Target, eSpells::SpellCorruptedAxionDmg, true);
                        break;
                    }
                    case eSpells::SpellCorruptedBreathDamage:
                    {
                        DoCast(p_Target, eSpells::SpellDarkHatred, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBilewaterLiquefactionDummy:
                    {
                        if (!p_Caster->GetAI())
                            break;

                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellBilewaterLiquefactionAoE, me->GetMap()->GetDifficultyID()))
                        {
                            float l_HealthPct   = std::min(p_Caster->GetAI()->GetFData(), float(l_SpellInfo->Effects[SpellEffIndex::EFFECT_2].BasePoints));
                            int32 l_Damage      = CalculatePct(float(l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].CalcValue(me)), std::max(5.0f, l_HealthPct));

                            me->CastCustomSpell(p_Caster->GetPosition(), eSpells::SpellBilewaterLiquefactionAoE, &l_Damage, nullptr, nullptr, true);
                        }

                        break;
                    }
                    case eSpells::SpellOrbOfCorruptionTrigger:
                    {
                        DoCast(p_Caster, eSpells::SpellOrbOfCorruptionAoE, true);
                        break;
                    }
                    case eSpells::SpellDecay:
                    {
                        DoCast(p_Caster, eSpells::SpellDecayAT, true);
                        break;
                    }
                    case eSpells::SpellOrbOfCorrosionTrigger:
                    {
                        DoCast(p_Caster, eSpells::SpellOrbOfCorrosionAoE, true);
                        break;
                    }
                    case eSpells::SpellMistInfusionCast:
                    {
                        m_InfusionEnergized += p_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints;
                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case eTovCreatures::CreatureBilewaterSlime:
                    {
                        if (p_Summon->IsAIEnabled)
                            m_BilewaterPcts.push(std::max(5.0f, p_Summon->AI()->GetFData()));

                        break;
                    }
                    case eTovCreatures::CreatureGrippingTentacle:
                    {
                        ++m_TentaclesKilled;

                        if (IsMythic())
                        {
                            uint8 l_Side = 0;

                            for (uint8 l_I = 0; l_I < eHelyaData::MaxSides; ++l_I)
                            {
                                for (auto const& l_Pair : g_GrippingTentacleRealMythicPos[l_I])
                                {
                                    if (p_Summon->IsNearPosition(&l_Pair.second, 0.5f))
                                    {
                                        l_Side = l_I;
                                        break;
                                    }
                                }
                            }

                            if (++m_GrippingTentacleIDx[l_Side] < (eHelyaData::MaxGrippingTentacleMythic / 2))
                            {
                                if (Creature* l_Tentacle = me->SummonCreature(g_GrippingTentacleRealMythicPos[l_Side][m_GrippingTentacleIDx[l_Side]].first, g_GrippingTentacleRealMythicPos[l_Side][m_GrippingTentacleIDx[l_Side]].second))
                                {
                                    /// Ritual stone activated
                                    if (GameObject* l_Stone = l_Tentacle->FindNearestGameObject(eTovGameObjects::GameObjectRitualStone, 5.0f))
                                        l_Stone->SetGoState(GOState::GO_STATE_READY);
                                }
                            }
                        }

                        /// Ritual stone destroyed
                        if (GameObject* l_Stone = p_Summon->FindNearestGameObject(eTovGameObjects::GameObjectRitualStone, 5.0f))
                            l_Stone->SendGameObjectActivateAnimKit(eVisuals::VisualRitualStone);

                        /// Each time a Gripping Tentacle is defeated, Helya will take a burst of damage equal to 2% of her maximum health.
                        me->DealDamage(me, uint32(me->CountPctFromMaxHealth(IsMythic() ? 2.25f : 2.0f)));

                        if (m_TentaclesKilled >= (IsMythic() ? eHelyaData::MaxGrippingTentacleMythic : eHelyaData::MaxGrippingTentacle))
                        {
                            events.Reset();

                            m_SummonEvents.Reset();

                            me->RemoveAreaTrigger(eSpells::SpellReanimationCosmetic1);
                            me->RemoveAreaTrigger(eSpells::SpellReanimationCosmetic2);

                            me->InterruptNonMeleeSpells(false);

                            me->RemoveAura(eSpells::SpellHelyaDamageImmunity);

                            DoCast(me, eSpells::SpellREUSE, true);

                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_31);

                            AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->NearTeleportTo(me->GetHomePosition());

                                Talk(eTalks::TalkPhase3);

                                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    DoCast(me, eSpells::SpellREUSE, true);

                                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        me->RemoveAura(eSpells::SpellHelyaPowerP2);

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN | eUnitFlags2::UNIT_FLAG2_UNK14);

                                        me->SetPower(Powers::POWER_ENERGY, 61);
                                        me->SetPower(Powers::POWER_ALTERNATE_POWER, 56);

                                        /// Refresh calculations
                                        DoCast(me, eSpells::SpellHelyaPowerP2, true);
                                        DoCast(me, eSpells::SpellHelyaPowerP3, true);
                                        DoCast(me, eSpells::SpellHelyaCosmeticPowerP3, true);

                                        DefaultEvents(ePhases::Phase03);

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_31);

                                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                        if (Unit* l_Target = me->getVictim())
                                            AttackStart(l_Target);
                                    });
                                });
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellBilewaterLiquefactionAoE && !m_BilewaterPcts.empty())
                {
                    float l_Pct = std::min(m_BilewaterPcts.front(), 95.0f) * 10.0f;

                    m_BilewaterPcts.pop();

                    uint64 l_Guid = p_AreaTrigger->GetGUID();
                    AddTimedDelayedOperation(10, [this, l_Guid, l_Pct]() -> void
                    {
                        if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*me, l_Guid))
                        {
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 17);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
                            l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
                            l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                            l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Pct / 100.0f);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 500);
                        }

                        AddTimedDelayedOperation(500, [this, l_Guid, l_Pct]() -> void
                        {
                            if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*me, l_Guid))
                                l_AreaTrigger->SetRadius(l_AreaTrigger->GetRadius() * l_Pct / 100.0f);
                        });
                    });
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_SecondPhaseTriggered && m_PhaseId == ePhases::Phase01 && me->GetReactState() == ReactStates::REACT_AGGRESSIVE && me->HealthBelowPctDamaged(65, p_Damage))
                {
                    m_SecondPhaseTriggered = true;

                    me->RemoveAllAuras();

                    m_CheckVictimTimer = 0;

                    DoCast(me, eSpells::SpellHelyaDamageImmunity, true);
                    DoCast(me, eSpells::SpellMistsOfHelheim, true);
                    DoCast(me, eSpells::SpellREUSE, true);

                    me->StopAttack();
                    me->ClearAllUnitState();
                    me->InterruptNonMeleeSpells(false);
                    me->SetGuidValue(EUnitFields::UNIT_FIELD_TARGET, 0);
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    me->SetPower(Powers::POWER_ENERGY, 0);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_31);

                    if (IsMythic())
                    {
                        for (uint8 l_I = 0; l_I < eHelyaData::MaxSides; ++l_I)
                        {
                            if (Creature* l_Tentacle = me->SummonCreature(g_GrippingTentacleRealMythicPos[l_I][m_GrippingTentacleIDx[l_I]].first, g_GrippingTentacleRealMythicPos[l_I][m_GrippingTentacleIDx[l_I]].second))
                            {
                                /// Ritual stone activated
                                if (GameObject* l_Stone = l_Tentacle->FindNearestGameObject(eTovGameObjects::GameObjectRitualStone, 5.0f))
                                    l_Stone->SetGoState(GOState::GO_STATE_READY);
                            }
                        }
                    }
                    else
                    {
                        for (auto const& l_Iter : g_GrippingTentacleRealPos)
                            me->SummonCreature(l_Iter.first, l_Iter.second);

                        if (instance)
                            instance->SetData(eTovData::DataHelyaRitualStones, 1);
                    }

                    events.Reset();

                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::TalkPhase2);

                        DefaultEvents(ePhases::Phase02);

                        me->SetFacingTo(4.18879f);

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->NearTeleportTo(g_SecondPhaseTeleportPos);

                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellREUSE, true);

                                if (!IsMythic())
                                    DoCast(me, eSpells::SpellHelyaPowerP2, true);

                                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN | eUnitFlags2::UNIT_FLAG2_UNK14);
                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_6 | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_31);
                            });
                        });

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (IsMythic())
                                DoCast(me, eSpells::SpellFuryOfTheMawChanneled);
                            else
                                DoCast(me, eSpells::SpellFuryOfTheMawAT, true);

                            uint8 l_I = 0;
                            for (Position const& l_Pos : g_LongboatSpawnPos)
                            {
                                if (Creature* l_Boat = me->SummonCreature(eTovCreatures::CreatureKvaldirLongboat, l_Pos))
                                {
                                    if (l_Boat->IsAIEnabled)
                                    {
                                        if (Creature* l_Passenger = me->SummonCreature(l_I == 0 ? eTovCreatures::CreatureNightWatchMariner : eTovCreatures::CreatureGrimelord, l_Boat->GetPosition()))
                                            l_Boat->AI()->SetGUID(l_Passenger->GetGUID());
                                    }
                                }

                                ++l_I;
                            }

                            if (IsMythic())
                            {
                                AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    for (Position const& l_Pos : g_MistwachersPos[m_MistwatcherIDx])
                                        DoCast(l_Pos, eSpells::SpellMistcasterSummon, true);

                                    ++m_MistwatcherIDx;
                                });
                            }
                        });
                    });
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                if (m_TrashesDone)
                    return;

                m_AlliesGuids.erase(p_Guid);

                if (m_AlliesGuids.empty())
                {
                    m_TrashesDone = true;

                    Talk(eTalks::TalkIntro3);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }
            }

            uint32 GetData(uint32 p_ID /*= 0*/) override
            {
                return m_PhaseId;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFullEnergy:
                    {
                        if (m_PhaseId == ePhases::Phase01)
                        {
                            if (events.HasEvent(eEvents::EventBilewaterBreath))
                                break;

                            events.ScheduleEvent(eEvents::EventBilewaterBreath, 1);
                        }
                        else
                        {
                            if (events.HasEvent(eEvents::EventCorruptedBreath))
                                break;

                            events.ScheduleEvent(eEvents::EventCorruptedBreath, 1);
                        }

                        break;
                    }
                    case eActions::ActionFullAlternateEnergy:
                    {
                        if (events.HasEvent(eEvents::EventFuryOfTheMaw))
                            break;

                        events.ScheduleEvent(eEvents::EventFuryOfTheMaw, 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_CheckPlayerTimer && !m_IntroDone)
                {
                    if (m_CheckPlayerTimer <= p_Diff)
                    {
                        if (me->FindNearestPlayer(110.0f) != nullptr)
                        {
                            Talk(eTalks::TalkIntro1);

                            AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::TalkIntro2);

                                AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    if (m_AlliesGuids.empty())
                                    {
                                        m_TrashesDone = true;

                                        Talk(eTalks::TalkIntro3);

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                    }
                                });
                            });

                            m_IntroDone = true;
                        }
                        else
                            m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_CheckPlayerTimer -= p_Diff;
                }

                if (m_CosmeticMistTimer)
                {
                    if (m_CosmeticMistTimer <= p_Diff)
                    {
                        DoCast(me, eSpells::SpellReanimationCosmetic1, true);
                        DoCast(me, eSpells::SpellReanimationCosmetic2, true);

                        m_CosmeticMistTimer = 20 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_CosmeticMistTimer -= p_Diff;
                }

                if (m_BerserkerTimer)
                {
                    if (m_BerserkerTimer <= p_Diff)
                        DoCast(me, eSpells::SpellBerserker);
                    else
                        m_BerserkerTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                m_SummonEvents.Update(p_Diff);

                switch (m_SummonEvents.ExecuteEvent())
                {
                    case eEvents::EventSpawnMinion1:
                    {
                        for (Position const& l_Pos : g_DecayingMinionsPos)
                            me->SummonCreature(eTovCreatures::CreatureDecayingMinion, l_Pos);

                        if (IsMythic() && m_PhaseId == ePhases::Phase02)
                        {
                            if (m_MistwatcherIDx == 1)
                                DoCast(urand(0, 1) ? g_MistwachersPos[m_MistwatcherIDx][0] : g_MistwachersPos[m_MistwatcherIDx][1], eSpells::SpellMistcasterSummon, true);
                            else
                            {
                                for (Position const& l_Pos : g_MistwachersPos[m_MistwatcherIDx])
                                    DoCast(l_Pos, eSpells::SpellMistcasterSummon, true);
                            }

                            ++m_MistwatcherIDx;

                            if (m_MistwatcherIDx >= eHelyaData::MaxMistwatchersIDx)
                                m_MistwatcherIDx = 0;
                        }

                        break;
                    }
                    case eEvents::EventSpawnMinion2:
                    {
                        for (Position const& l_Pos : g_DecayingMinionsPos)
                            me->SummonCreature(eTovCreatures::CreatureDecayingMinion, l_Pos);

                        break;
                    }
                    default:
                        break;
                }

                if (m_CheckWaterTimer)
                {
                    if (m_CheckWaterTimer <= p_Diff)
                    {
                        Map const* l_Map = me->GetBaseMap();

                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                if (!l_Target->IsPlayer())
                                    continue;

                                /// Under water, or on/in water around Helya, or behind Helya
                                Position l_HomePos = me->GetHomePosition();
                                if (l_Target->GetPositionZ() < l_Map->GetWaterLevel(l_Target->GetPositionX(), l_Target->GetPositionY()) || l_Target->GetDistance(l_HomePos) <= 20.0f || l_HomePos.IsInBack(l_Target))
                                {
                                    if (!l_Target->HasAura(eSpells::SpellDarkWaters))
                                        l_Target->CastSpell(l_Target, eSpells::SpellDarkWaters, true);
                                }
                                else
                                    l_Target->RemoveAura(eSpells::SpellDarkWaters);
                            }
                        }

                        m_CheckWaterTimer = 500;
                    }
                    else
                        m_CheckWaterTimer -= p_Diff;
                }

                if (m_PhaseId == ePhases::Phase03 && IsWipe())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_CheckVictimTimer)
                {
                    if (m_CheckVictimTimer <= p_Diff)
                    {
                        if (me->getVictim())
                        {
                            if (!me->IsWithinMeleeRange(me->getVictim()))
                                events.ScheduleEvent(eEvents::EventCorrosiveNova, 100);
                        }

                        m_CheckVictimTimer = 2000;
                    }
                    else
                        m_CheckVictimTimer -= p_Diff;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBilewaterBreath:
                    {
                        me->SetPower(Powers::POWER_ENERGY, 0);

                        Talk(eTalks::TalkBilewaterBreath);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        DoCastVictim(eSpells::SpellBilewaterBreathCast);

                        m_BilewaterTankGuid = me->getVictim() ? me->getVictim()->GetGUID() : 0;
                        break;
                    }
                    case eEvents::EventCorrosiveNova:
                    {
                        DoCast(me, eSpells::SpellCorrosiveNova);
                        break;
                    }
                    case eEvents::EventOrbOfCorruption:
                    {
                        Talk(eTalks::TalkOrbOfCorruption);

                        if (Creature* l_Orb = me->SummonCreature(eTovCreatures::CreatureOrbOfCorruptionDummy, g_OrbOfCorruptionPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 11 * TimeConstants::IN_MILLISECONDS))
                        {
                            l_Orb->CastSpell(l_Orb, eSpells::SpellOrbOfCorruptionDummyVisual, true);

                            m_OrbOfCorruptionGuid = l_Orb->GetGUID();

                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                if (Creature* l_Orb = Creature::GetCreature(*me, m_OrbOfCorruptionGuid))
                                {
                                    l_Orb->EnterVehicle(me);
                                    l_Orb->CastSpell(l_Orb, eSpells::SpellOrbOfCorruptionDummyCast);
                                }
                            });
                        }

                        DoCast(me, eSpells::SpellOrbOfCorruptionCast);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventOrbOfCorruption, 24 * TimeConstants::IN_MILLISECONDS);
                        else if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventOrbOfCorruption, 28 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventOrbOfCorruption, 31 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventTentacleStrike:
                    {
                        Talk(eTalks::TalkTentacleStrike);

                        bool l_Back = urand(0, 1);
                        std::vector<Position> l_Positions;

                        if (!IsMythic())
                        {
                            if (l_Back)
                                l_Positions.push_back(JadeCore::Containers::SelectRandomContainerElement(g_StrikingTentaclePos[1]));
                            else
                                l_Positions.push_back(JadeCore::Containers::SelectRandomContainerElement(g_StrikingTentaclePos[0]));
                        }
                        else
                        {
                            switch (m_TentacleCount)
                            {
                                case 0:
                                {
                                    l_Positions.push_back(g_StrikingTentaclePos[0][0]);
                                    l_Positions.push_back(g_StrikingTentaclePos[0][1]);
                                    break;
                                }
                                case 1:
                                {
                                    l_Positions.push_back(g_StrikingTentaclePos[1][1]);
                                    l_Positions.push_back(g_StrikingTentaclePos[0][0]);
                                    break;
                                }
                                case 2:
                                {
                                    l_Positions.push_back(g_StrikingTentaclePos[1][1]);
                                    l_Positions.push_back(g_StrikingTentaclePos[1][2]);
                                    break;
                                }
                                case 3:
                                {
                                    l_Positions.push_back(g_StrikingTentaclePos[0][1]);
                                    l_Positions.push_back(g_StrikingTentaclePos[0][0]);
                                    l_Positions.push_back(g_StrikingTentaclePos[0][1]);
                                    break;
                                }
                                case 4:
                                {
                                    l_Positions.push_back(g_StrikingTentaclePos[0][0]);
                                    l_Positions.push_back(g_StrikingTentaclePos[0][1]);
                                    break;
                                }
                                default:
                                {
                                    l_Positions.push_back(JadeCore::Containers::SelectRandomContainerElement(g_StrikingTentaclePos[0]));
                                    l_Positions.push_back(JadeCore::Containers::SelectRandomContainerElement(g_StrikingTentaclePos[1]));
                                    break;
                                }
                            }

                            ++m_TentacleCount;
                        }

                        uint8 l_I = 0;
                        for (Position& l_Pos : l_Positions)
                        {
                            if (Creature* l_Tentacle = me->SummonCreature(eTovCreatures::CreatureStrikingTentacle, l_Pos))
                            {
                                if (l_Tentacle->IsAIEnabled)
                                    l_Tentacle->AI()->SetData(eHelyaData::TentaclePosition, !IsMythic() ? (l_Back != 0) : (l_I++ != 0));
                            }
                        }

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventTentacleStrike, 35 * TimeConstants::IN_MILLISECONDS);
                        else if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventTentacleStrike, 42 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventTentacleStrike, 40 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventTaintOfTheSea:
                    {
                        ++m_TaintedCount;

                        DoCastVictim(eSpells::SpellTaintOfTheSeaAura, true);

                        if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer))
                            DoCast(l_Player, eSpells::SpellTaintOfTheSeaAura, true);

                        if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask(eTargetTypeMask::TypeMaskRanged | eTargetTypeMask::TypeMaskMelee)))
                            DoCast(l_Player, eSpells::SpellTaintOfTheSeaAura, true);

                        if (IsMythic())
                        {
                            if (m_PhaseId == ePhases::Phase03)
                            {
                                if (m_TaintedCount <= 11)
                                    events.ScheduleEvent(eEvents::EventTaintOfTheSea, 20 * TimeConstants::IN_MILLISECONDS);
                                else if (m_TaintedCount == 12)
                                    events.ScheduleEvent(eEvents::EventTaintOfTheSea, 17 * TimeConstants::IN_MILLISECONDS);
                                else if (m_TaintedCount == 13)
                                    events.ScheduleEvent(eEvents::EventTaintOfTheSea, 14 * TimeConstants::IN_MILLISECONDS);
                                else
                                    events.ScheduleEvent(eEvents::EventTaintOfTheSea, 11 * TimeConstants::IN_MILLISECONDS);
                            }
                            else
                                events.ScheduleEvent(eEvents::EventTaintOfTheSea, 12 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (IsHeroic())
                        {
                            if (m_PhaseId == ePhases::Phase03)
                                events.ScheduleEvent(eEvents::EventTaintOfTheSea, 25 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventTaintOfTheSea, 14 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            if (m_PhaseId == ePhases::Phase03)
                                events.ScheduleEvent(eEvents::EventTaintOfTheSea, 27 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventTaintOfTheSea, 12 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eEvents::EventTorrent:
                    {
                        DoCast(me, eSpells::SpellTorrent);
                        events.ScheduleEvent(eEvents::EventTorrent, 7 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFuryOfTheMaw:
                    {
                        Talk(eTalks::TalkFuryOfTheMaw);

                        if (m_PhaseId == ePhases::Phase02)
                        {
                            DoCast(me, eSpells::SpellFuryOfTheMawChanneled);

                            if (IsMythic())
                            {
                                uint8 l_I = 0;
                                for (Position const& l_Pos : g_LongboatSpawnPos)
                                {
                                    if (Creature* l_Boat = me->SummonCreature(eTovCreatures::CreatureKvaldirLongboat, l_Pos))
                                    {
                                        if (l_Boat->IsAIEnabled)
                                        {
                                            if (Creature* l_Passenger = me->SummonCreature(l_I == 0 ? eTovCreatures::CreatureNightWatchMariner : eTovCreatures::CreatureGrimelord, l_Boat->GetPosition()))
                                                l_Boat->AI()->SetGUID(l_Passenger->GetGUID());
                                        }
                                    }

                                    ++l_I;
                                }
                            }

                            me->RemoveAura(eSpells::SpellHelyaPowerP2);

                            me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);
                        }
                        else
                            DoCast(me, eSpells::SpellFuryOfTheMawCast);

                        break;
                    }
                    case eEvents::EventDecayingMinions:
                    {
                        DoCast(me, eSpells::SpellReanimationCosmetic1, true);
                        DoCast(me, eSpells::SpellReanimationCosmetic2, true);

                        m_SummonEvents.ScheduleEvent(eEvents::EventSpawnMinion1, 5 * TimeConstants::IN_MILLISECONDS);
                        m_SummonEvents.ScheduleEvent(eEvents::EventSpawnMinion2, 10 * TimeConstants::IN_MILLISECONDS);

                        events.ScheduleEvent(eEvents::EventDecayingMinions, 75 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventOrbOfCorrosion:
                    {
                        Talk(eTalks::TalkOrbOfCorrosion);

                        if (Creature* l_Orb = me->SummonCreature(eTovCreatures::CreatureOrbOfCorrosionDummy, g_OrbOfCorruptionPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 11 * TimeConstants::IN_MILLISECONDS))
                        {
                            l_Orb->CastSpell(l_Orb, eSpells::SpellOrbOfCorrosionDummyVisual, true);

                            m_OrbOfCorruptionGuid = l_Orb->GetGUID();

                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                if (Creature* l_Orb = Creature::GetCreature(*me, m_OrbOfCorruptionGuid))
                                {
                                    l_Orb->EnterVehicle(me);
                                    l_Orb->CastSpell(l_Orb, eSpells::SpellOrbOfCorrosionDummyCast);
                                }
                            });
                        }

                        DoCast(me, eSpells::SpellOrbOfCorrosionCast);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 12 * TimeConstants::IN_MILLISECONDS);
                        else if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 17 * TimeConstants::IN_MILLISECONDS);
                        else if (IsLFR())
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 33 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventOrbOfCorrosion, 17 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventCorruptedBreath:
                    {
                        me->SetPower(Powers::POWER_ENERGY, 0);

                        Talk(eTalks::TalkCorruptedBreath);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        DoCastVictim(eSpells::SpellCorruptedBreathCast);

                        m_BilewaterTankGuid = me->getVictim() ? me->getVictim()->GetGUID() : 0;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            bool IsWipe() const
            {
                std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                for (HostileReference* l_Ref : l_ThreatList)
                {
                    if (Unit* l_Target = l_Ref->getTarget())
                    {
                        /// If all players aren't controlled, don't reset
                        if (l_Target->IsPlayer() && !l_Target->HasAura(eSpells::SpellCorrupted))
                            return false;
                    }
                }

                return true;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_tov_helyaAI(p_Creature);
        }
};

/// Bilewater Slime - 114553
class npc_tov_helya_bilewater_slime : public CreatureScript
{
    public:
        npc_tov_helya_bilewater_slime() : CreatureScript("npc_tov_helya_bilewater_slime") { }

        struct npc_tov_helya_bilewater_slimeAI : public Scripted_NoMovementAI
        {
            npc_tov_helya_bilewater_slimeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpells
            {
                SpellBilewaterLiquefactionCast  = 227990,
                SpellBilewaterLiquefactionDummy = 227993,
                SpellBilewaterLiquefactionScale = 228984,

                SpellSlimey                     = 154241
            };

            bool m_Casted = false;
            float m_HealthPct = 95.0f;

            void Reset() override
            {
                ApplyAllImmunities(true);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 4.712f;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellSlimey, true);
                DoCast(me, eSpells::SpellBilewaterLiquefactionScale, true);
                DoCast(me, eSpells::SpellBilewaterLiquefactionCast);

                if (!p_Summoner->isInCombat())
                    me->DespawnOrUnsummon();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBilewaterLiquefactionCast:
                    {
                        m_HealthPct = me->GetHealthPct();

                        m_Casted = true;

                        DoCast(me, eSpells::SpellBilewaterLiquefactionDummy, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                if (m_Casted)
                    return;

                m_HealthPct = 0.0f;

                DoCast(me, eSpells::SpellBilewaterLiquefactionDummy, true);
            }

            float GetFData(uint32 p_ID /*= 0*/) const override
            {
                return m_HealthPct;
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_bilewater_slimeAI(p_Creature);
        }
};

/// Orb of Corruption - 115166
class npc_tov_helya_orb_of_corruption : public CreatureScript
{
    public:
        npc_tov_helya_orb_of_corruption() : CreatureScript("npc_tov_helya_orb_of_corruption") { }

        struct npc_tov_helya_orb_of_corruptionAI : public ScriptedAI
        {
            npc_tov_helya_orb_of_corruptionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellOrbOfCorruptionAura        = 229119,
                SpellOrbOfCorruptionPeriodic    = 227926,
                SpellCorruptedSlicerDummy       = 231304,
                SpellCorruptedSlicerDamages     = 228068
            };

            uint64 m_TargetGUID = 0;

            std::set<uint64> m_Passengers;

            bool m_IsMoving = false;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->IsPlayer())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                m_TargetGUID = p_Summoner->GetGUID();

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    if (Player* l_Player = Player::GetPlayer(*me, m_TargetGUID))
                    {
                        DoCast(l_Player, eSpells::SpellOrbOfCorruptionAura, true);
                        DoCast(me, eSpells::SpellOrbOfCorruptionPeriodic, true);

                        m_IsMoving = true;
                    }
                });
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply)
                    m_Passengers.insert(p_Passenger->GetGUID());
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target->GetGUID() == me->GetGUID() || p_Target->GetEntry() != me->GetEntry() || !m_IsMoving)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellCorruptedSlicerDummy)
                {
                    Creature* l_Helya = nullptr;
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Helya = Creature::GetCreature(*me, l_Instance->GetData64(eTovCreatures::CreatureBossHelya));

                    if (!l_Helya)
                        return;

                    float l_Range = me->GetDistance(p_Target);

                    std::list<Player*> l_PlayerList;

                    me->GetPlayerListInGrid(l_PlayerList, l_Range);

                    if (!l_PlayerList.empty())
                    {
                        l_PlayerList.remove_if([this, l_Helya, p_Target](Player* p_Player) -> bool
                        {
                            /// Not attackable
                            if (!l_Helya->IsValidAttackTarget(p_Player))
                                return true;

                            /// Not in the beam line
                            if (!p_Player->IsInBetween(me, p_Target, 1.0f))
                                return true;

                            /// Not between the two orbs
                            if (me->GetDistance(p_Player) > me->GetDistance(p_Target))
                                return true;

                            return false;
                        });
                    }

                    for (Player*& l_Player : l_PlayerList)
                        l_Helya->CastSpell(l_Player, eSpells::SpellCorruptedSlicerDamages, true);
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = 6.0f;
            }

            void JustDespawned() override
            {
                for (uint64 l_Guid : m_Passengers)
                {
                    if (Creature* l_Passenger = Creature::GetCreature(*me, l_Guid))
                    {
                        if (l_Passenger->ToCreature())
                        {
                            l_Passenger->RemoveAllAuras();
                            l_Passenger->DespawnOrUnsummon();
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                /// If target is dead or not targetable anymore, just move to its position no matter what
                if (Player* l_Player = Player::GetPlayer(*me, m_TargetGUID))
                {
                    if (l_Player->isDead() || !me->IsValidAttackTarget(l_Player))
                    {
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(0, l_Player->GetPosition());
                    }
                }

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eTovData::DataBossHelya) != EncounterState::IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_orb_of_corruptionAI(p_Creature);
        }
};

/// Striking Tentacle - 114881
class npc_tov_helya_striking_tentacle : public CreatureScript
{
    public:
        npc_tov_helya_striking_tentacle() : CreatureScript("npc_tov_helya_striking_tentacle") { }

        struct npc_tov_helya_striking_tentacleAI : public Scripted_NoMovementAI
        {
            npc_tov_helya_striking_tentacleAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpell
            {
                SpellTentacleStrike = 228730
            };

            enum eTalks
            {
                TalkTentacleFront,
                TalkTentacleBack
            };

            enum eVisuals
            {
                VisualStrike1 = 67509,
                VisualStrike2 = 54168
            };

            bool m_Back = false;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                Position l_Pos = me->GetPosition();

                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 45.0f, me->GetOrientation(), true);

                DoCast(l_Pos, eSpell::SpellTentacleStrike);

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SendPlaySpellVisualKit(eVisuals::VisualStrike1, 0);
                });
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpell::SpellTentacleStrike)
                {
                    Position l_Pos = me->GetPosition();

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, me->GetOrientation(), true);

                    float l_Step = 2.0f;

                    for (uint8 l_I = 0; l_I < eHelyaData::MaxTentacleVisuals; ++l_I)
                    {
                        me->SendPlaySpellVisual(eVisuals::VisualStrike2, nullptr, 0.5f, l_Pos, true);

                        l_Pos.m_positionZ = 2.79403f;

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Step, me->GetOrientation(), true);
                    }

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    me->DespawnOrUnsummon(4 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void SetData(uint32 p_Data, uint32 p_Value) override
            {
                switch (p_Data)
                {
                    case eHelyaData::TentaclePosition:
                    {
                        m_Back = p_Value != 0;

                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (IsMythic() && l_Instance)
                        {
                            if (Creature* l_Helya = Creature::GetCreature(*me, l_Instance->GetData64(eTovCreatures::CreatureBossHelya)))
                            {
                                if (me->GetDistance(*l_Helya) > 50.0f)
                                    m_Back = true;
                                else
                                    m_Back = false;
                            }
                        }

                        if (m_Back)
                            Talk(eTalks::TalkTentacleBack, me->GetGUID());
                        else
                            Talk(eTalks::TalkTentacleFront, me->GetGUID());

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eTovData::DataBossHelya) != EncounterState::IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_striking_tentacleAI(p_Creature);
        }
};

/// Gripping Tentacle - 114900
/// Gripping Tentacle - 114901
/// Gripping Tentacle - 114905
class npc_tov_helya_gripping_tentacle : public CreatureScript
{
    public:
        npc_tov_helya_gripping_tentacle() : CreatureScript("npc_tov_helya_gripping_tentacle") { }

        struct npc_tov_helya_gripping_tentacleAI : public ScriptedAI
        {
            npc_tov_helya_gripping_tentacleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellHelyaStun1 = 228772,
                SpellHelyaStun2 = 228778
            };

            void Reset() override
            {
                ApplyAllImmunities(true);

                if (me->GetEntry() == eTovCreatures::CreatureGrippingTentacle && IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 5.63115f;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return me->GetEntry() == eTovCreatures::CreatureGrippingTentacle;
            }

            bool CanTargetOutOfLOS() override
            {
                return me->GetEntry() == eTovCreatures::CreatureGrippingTentacle;
            }

            bool CanBeHitInTheBack() override
            {
                return false;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (me->GetEntry() == eTovCreatures::CreatureGrippingTentacle)
                {
                    SetCombatMovement(false);
                    me->AddUnitState(UnitState::UNIT_STATE_STUNNED);
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    return;
                }

                if (me->GetEntry() == eTovCreatures::CreatureGrippingTentacle1)
                    DoCast(me, eSpells::SpellHelyaStun1, true);
                else
                    DoCast(me, eSpells::SpellHelyaStun2, true);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Creature* l_Vehicle = me->FindNearestCreature(eTovCreatures::CreatureGrippingTentacle, 2.0f))
                    {
                        if (me->GetEntry() == eTovCreatures::CreatureGrippingTentacle2)
                        {
                            Position l_CheckPos = { 579.9722f, 667.9254f, 21.24489f, 3.211406f };

                            me->EnterVehicle(l_Vehicle, me->IsNearPosition(&l_CheckPos, 0.1f) ? 3 : 1);
                        }
                        else
                            me->EnterVehicle(l_Vehicle);

                        l_Vehicle->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    }
                });
            }

            void AttackStart(Unit* p_Target, bool p_Melee) override
            {
                if (me->GetEntry() == eTovCreatures::CreatureGrippingTentacle)
                    return;

                UnitAI::AttackStart(p_Target, p_Melee);
            }

            void OnVehicleExited(Unit* /*p_Vehicle*/) override
            {
                if (me->GetEntry() == eTovCreatures::CreatureGrippingTentacle)
                    return;

                me->RemoveAllAuras();
                me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eTovData::DataBossHelya) != EncounterState::IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_gripping_tentacleAI(p_Creature);
        }
};

/// Kvaldir Longboat - 115941
class npc_tov_helya_kvaldir_longboat : public CreatureScript
{
    public:
        npc_tov_helya_kvaldir_longboat() : CreatureScript("npc_tov_helya_kvaldir_longboat") { }

        struct npc_tov_helya_kvaldir_longboatAI : public ScriptedAI
        {
            npc_tov_helya_kvaldir_longboatAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellKvaldirLongboat1   = 230464,
                SpellKvaldirLongboat2   = 230491
            };

            uint64 m_Passenger = 0;
            bool m_Exited = false;

            uint8 m_MoveIDx = 0;
            uint8 m_ExitIDx = 0;

            std::vector<G3D::Vector3> m_Path = { };

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Position l_FirstBoat = { 638.672f, 817.6201f, -14.82369f, 3.943812f };

                if (me->IsNearPosition(&l_FirstBoat, 0.1f))
                {
                    DoCast(me, eSpells::SpellKvaldirLongboat1, true);

                    m_Path =
                    {
                        { 618.6702f, 782.9792f, -15.0107f },
                        { 528.4965f, 685.1771f, 2.806649f },
                        { 516.4566f, 626.7899f, 5.759933f },
                        { 544.5886f, 570.3906f, 7.612964f },
                        { 613.9167f, 532.8021f, 5.011508f }
                    };

                    m_ExitIDx = 3;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothFlyPath(1, m_Path.data(), m_Path.size());
                }
                else
                {
                    DoCast(me, eSpells::SpellKvaldirLongboat2, true);

                    m_Path =
                    {
                        { 549.3900f, 822.9790f, -14.9070f },
                        { 515.1337f, 691.9045f, 2.810000f },
                        { 450.8455f, 640.2570f, 5.759933f },
                        { 384.5156f, 636.4271f, 5.011508f }
                    };

                    m_ExitIDx = 3;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothFlyPath(1, m_Path.data(), m_Path.size());
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                    me->DespawnOrUnsummon();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_Passenger = p_Guid;

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                    {
                        l_Passenger->SetReactState(ReactStates::REACT_PASSIVE);
                        l_Passenger->EnterVehicle(me, l_Passenger->GetEntry() == eTovCreatures::CreatureGrimelord ? 1 : 0);
                    }
                });
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = 42.0f;
            }

            void JustDespawned() override
            {
                if (!m_Exited)
                    ExitPassenger();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eTovData::DataBossHelya) != EncounterState::IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }

                if (m_ExitIDx < m_Path.size() && !m_Exited)
                    ExitPassenger();
            }

            void ExitPassenger()
            {
                G3D::Vector3 l_Point = m_Path[m_ExitIDx];

                Position l_ExitPoint = Position(l_Point.x, l_Point.y, l_Point.z);

                if (me->IsNearPosition(&l_ExitPoint, m_Path.size() == 5 ? 10.0f : 15.0f))
                {
                    if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                    {
                        m_Exited = true;

                        l_Passenger->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        Position l_ExitPos;

                        if (l_Passenger->GetEntry() == eTovCreatures::CreatureGrimelord)
                            l_ExitPos = { 463.507f, 656.587f, 5.02879f, 0.014f };
                        else
                            l_ExitPos = { 535.932f, 622.037f, 6.75224f, 2.8337f };

                        me->RemoveAura(eSpells::SpellKvaldirLongboat1);
                        me->RemoveAura(eSpells::SpellKvaldirLongboat2);

                        l_Passenger->ExitVehicle();

                        AddTimedDelayedOperation(10, [this, l_ExitPos]() -> void
                        {
                            if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                                l_Passenger->NearTeleportTo(l_ExitPos);

                            AddTimedDelayedOperation(10, [this, l_ExitPos]() -> void
                            {
                                if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                                {
                                    l_Passenger->GetMotionMaster()->Clear();
                                    l_Passenger->GetMotionMaster()->MoveJump(l_ExitPos, 30.0f, 10.0f);

                                    l_Passenger->NearTeleportTo(l_ExitPos);

                                    l_Passenger->SetHomePosition(l_ExitPos);
                                    l_Passenger->ForceSendUpdate();

                                    l_Passenger->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                                    l_Passenger->ClearUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                                    l_Passenger->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                    l_Passenger->SetInCombatWithZone();

                                    if (l_Passenger->IsAIEnabled)
                                        l_Passenger->AI()->EnterCombat(me);

                                    m_Passenger = 0;

                                    me->DespawnOrUnsummon(10);
                                }
                            });
                        });

                        l_Passenger->SetInCombatWithZone();
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_kvaldir_longboatAI(p_Creature);
        }
};

/// Grimelord - 114709
class npc_tov_helya_grimelord : public CreatureScript
{
    public:
        npc_tov_helya_grimelord() : CreatureScript("npc_tov_helya_grimelord") { }

        struct npc_tov_helya_grimelordAI : public ScriptedAI
        {
            npc_tov_helya_grimelordAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                /// Misc
                SpellAnchor             = 228376,
                SpellKvaldirLongboat    = 167910,
                /// Fight
                SpellFetidRotSearcher   = 228838,
                SpellFetidRot           = 193367,
                SpellSludgeNova         = 228390,
                SpellAnchorSlam         = 228519,
                SpellAnchorSlamVisual   = 230997,
                SpellFetidFission       = 158534
            };

            enum eEvents
            {
                EventFetidRot = 1,
                EventSludgeNova,
                EventAnchorSlam,
                EventFetidFission
            };

            void Reset() override
            {
                DoCast(me, eSpells::SpellAnchor, true);
                DoCast(me, eSpells::SpellKvaldirLongboat, true);

                me->SendPlayHoverAnim(true);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 3.12604f;
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (p_Attacker->GetEntry() != eTovCreatures::CreatureKvaldirLongboat)
                    return;

                DefaultEvents();
            }

            void DefaultEvents()
            {
                me->RemoveAura(eSpells::SpellKvaldirLongboat);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->SendPlayHoverAnim(false);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 4);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                events.Reset();

                events.ScheduleEvent(eEvents::EventFetidRot, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSludgeNova, 15 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventAnchorSlam, 17 * TimeConstants::IN_MILLISECONDS);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventFetidFission, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFetidRotSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellFetidRot, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->GetReactState() == ReactStates::REACT_PASSIVE || !UpdateVictim())
                {
                    if (!me->IsOnVehicle())
                        DefaultEvents();

                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFetidRot:
                    {
                        DoCast(me, eSpells::SpellFetidRotSearcher, true);
                        events.ScheduleEvent(eEvents::EventFetidRot, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSludgeNova:
                    {
                        DoCast(me, eSpells::SpellSludgeNova);
                        events.ScheduleEvent(eEvents::EventSludgeNova, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAnchorSlam:
                    {
                        DoCastVictim(eSpells::SpellAnchorSlam);

                        uint64 l_Guid = me->getVictim() ? me->getVictim()->GetGUID() : 0;
                        AddTimedDelayedOperation(1500, [this, l_Guid]() -> void
                        {
                            if (Unit* l_Target = Unit::GetUnit(*me, l_Guid))
                            {
                                Position l_Pos = l_Target->GetPosition();
                                float l_Angle = 0.0f;

                                for (uint8 l_I = 0; l_I < 5; ++l_I)
                                {
                                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, l_Angle);

                                    if (Creature* l_Anchor = me->SummonCreature(eTovCreatures::CreatureAnchor, l_Pos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 3 * TimeConstants::IN_MILLISECONDS))
                                    {
                                        l_Anchor->SetFacingTo(l_Anchor->GetAngle(l_Target));
                                        l_Anchor->CastSpell(l_Anchor, eSpells::SpellAnchorSlamVisual, true);

                                        l_Anchor->GetMotionMaster()->Clear();
                                        l_Anchor->GetMotionMaster()->MoveCharge(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ + 5.0f, 10.0f);
                                    }

                                    l_Angle += 2.0f * M_PI / 5.0f;
                                }
                            }
                        });

                        events.ScheduleEvent(eEvents::EventAnchorSlam, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFetidFission:
                    {
                        DoCast(me, eSpells::SpellFetidFission, true);
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
            return new npc_tov_helya_grimelordAI(p_Creature);
        }
};

/// Night Watch Mariner - 114809
class npc_tov_helya_night_watch_mariner : public CreatureScript
{
    public:
        npc_tov_helya_night_watch_mariner() : CreatureScript("npc_tov_helya_night_watch_mariner") { }

        struct npc_tov_helya_night_watch_marinerAI : public ScriptedAI
        {
            npc_tov_helya_night_watch_marinerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                /// Misc
                SpellKvaldirLongboat        = 167910,
                SpellDarkness               = 228383,
                SpellPirateCharge           = 228638,
                SpellPirateCharge2          = 198336,
                /// Fight
                SpellGiveNoQuarterCast      = 228633,
                SpellGiveNoQuarterMissile   = 228632,
                SpellGhostlyRage            = 228611,
                SpellLanternOfDarkness      = 228619,
                SpellRallyOfTheKvaldir      = 232346
            };

            enum eEvents
            {
                EventGiveNoQuarter = 1,
                EventGhostlyRage,
                EventLanternOfDarkness,
                EventRallyOfTheKvaldir
            };

            enum eAction
            {
                ActionFullEnergy
            };

            enum ePhase
            {
                Phase03 = 3
            };

            void Reset() override
            {
                DoCast(me, eSpells::SpellKvaldirLongboat, true);

                me->SendPlayHoverAnim(true);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 4.11879f;
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (p_Attacker->GetEntry() != eTovCreatures::CreatureKvaldirLongboat)
                    return;

                me->RemoveAura(eSpells::SpellKvaldirLongboat);

                me->SendPlayHoverAnim(false);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->SetPower(Powers::POWER_ENERGY, 0);

                DoCast(me, eSpells::SpellDarkness, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 4);

                events.Reset();

                events.ScheduleEvent(eEvents::EventGiveNoQuarter, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventGhostlyRage, 12 * TimeConstants::IN_MILLISECONDS);

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (Creature* l_Helya = l_Owner->ToCreature())
                    {
                        if (l_Helya->IsAIEnabled && l_Helya->AI()->GetData() == ePhase::Phase03)
                            events.ScheduleEvent(eEvents::EventRallyOfTheKvaldir, 1 * TimeConstants::IN_MILLISECONDS);
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionFullEnergy)
                {
                    if (events.HasEvent(eEvents::EventLanternOfDarkness))
                        return;

                    events.ScheduleEvent(eEvents::EventLanternOfDarkness, 1);
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellGiveNoQuarterCast:
                    {
                        Position l_Pos = p_Target->GetPosition();

                        if (!me->GetAnyOwner())
                            return;

                        if (Creature* l_Helya = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Helya->IsAIEnabled)
                            {
                                l_Helya->AI()->AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [l_Helya, l_Pos]() -> void
                                {
                                    l_Helya->SummonCreature(eTovCreatures::CreatureNightWatchMarinerImage, l_Pos);
                                });
                            }
                        }

                        DoCast(p_Target, eSpells::SpellGiveNoQuarterMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellLanternOfDarkness)
                {
                    SetCombatMovement(true);

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Target);
                        }
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->GetReactState() == ReactStates::REACT_PASSIVE || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventGiveNoQuarter:
                    {
                        DoCast(me, eSpells::SpellGiveNoQuarterCast);

                        if (IsHeroicOrMythic())
                            events.ScheduleEvent(eEvents::EventGiveNoQuarter, 6 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventGiveNoQuarter, 10* TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventGhostlyRage:
                    {
                        DoCast(me, eSpells::SpellGhostlyRage, true);
                        events.ScheduleEvent(eEvents::EventGhostlyRage, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventLanternOfDarkness:
                    {
                        me->AttackStop();
                        me->StopMoving();

                        SetCombatMovement(false);

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        DoCast(me, eSpells::SpellLanternOfDarkness);
                        break;
                    }
                    case eEvents::EventRallyOfTheKvaldir:
                    {
                        DoCast(me, eSpells::SpellRallyOfTheKvaldir, true);
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
            return new npc_tov_helya_night_watch_marinerAI(p_Creature);
        }
};

/// Night Watch Mariner - 114812
class npc_tov_helya_night_watch_mariner_image : public CreatureScript
{
    public:
        npc_tov_helya_night_watch_mariner_image() : CreatureScript("npc_tov_helya_night_watch_mariner_image") { }

        struct npc_tov_helya_night_watch_mariner_imageAI : public Scripted_NoMovementAI
        {
            npc_tov_helya_night_watch_mariner_imageAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpells
            {
                SpellPirateCharge   = 228638,
                SpellPirateCharge2  = 198336
            };

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                DoCast(me, eSpells::SpellPirateCharge, true);

                AddTimedDelayedOperation(500, [this]() -> void
                {
                    DoCast(me, eSpells::SpellPirateCharge2, true);
                });

                me->DespawnOrUnsummon(2 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_night_watch_mariner_imageAI(p_Creature);
        }
};

/// Decaying Minion - 114568
class npc_tov_helya_decaying_minion : public CreatureScript
{
    public:
        npc_tov_helya_decaying_minion() : CreatureScript("npc_tov_helya_decaying_minion") { }

        struct npc_tov_helya_decaying_minionAI : public ScriptedAI
        {
            npc_tov_helya_decaying_minionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellRabid  = 202476,
                SpellFixate = 234189,
                SpellVigor  = 203816,
                SpellDecay  = 230195
            };

            enum eEvent
            {
                EventVigor = 1
            };

            bool m_Fixated = false;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 1.9375f;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    Position l_Pos  = me->GetPosition();
                    float l_Angle   = me->GetAngle(&g_CenterPos);

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 20.0f, l_Angle, true);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 10.0f, 1);
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                    DoCast(l_Target, eSpells::SpellFixate, true);

                events.ScheduleEvent(eEvent::EventVigor, 12 * TimeConstants::IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellRabid)
                {
                    me->DeleteThreatList();

                    if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    {
                        me->AddThreat(l_Target, std::numeric_limits<float>::max());
                        me->TauntApply(l_Target);
                        me->SetTarget(l_Target->GetGUID());

                        m_Fixated = true;
                    }
                }
            }

            void OnAddThreat(Unit* /*p_Attacker*/, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Fixated)
                    p_Threat = 0.0f;
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);

                    me->HandleEmoteCommand(Anim::ANIM_SPELL_CAST_DIRECTED);

                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        DoCast(me, eSpells::SpellRabid, true);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_3, 0);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        me->SetInCombatWithZone();

                        me->ClearUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                    });
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellDecay, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->GetReactState() == ReactStates::REACT_PASSIVE || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvent::EventVigor)
                {
                    DoCast(me, eSpells::SpellVigor, true);
                    events.ScheduleEvent(eEvent::EventVigor, 5 * TimeConstants::IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_decaying_minionAI(p_Creature);
        }
};

/// Orb of Corrosion - 114535
class npc_tov_helya_orb_of_corrosion : public CreatureScript
{
    public:
        npc_tov_helya_orb_of_corrosion() : CreatureScript("npc_tov_helya_orb_of_corrosion") { }

        struct npc_tov_helya_orb_of_corrosionAI : public ScriptedAI
        {
            npc_tov_helya_orb_of_corrosionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellOrbOfCorrosionAura     = 230267,
                SpellOrbOfCorrosionPeriodic = 228061,
                SpellCorrosiveSlicerDummy   = 231417,
                SpellCorrosiveSlicerDamage  = 231416
            };

            uint64 m_TargetGUID = 0;

            std::set<uint64> m_Passengers;

            bool m_IsMoving = false;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->IsPlayer())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                m_TargetGUID = p_Summoner->GetGUID();

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    if (Player* l_Player = Player::GetPlayer(*me, m_TargetGUID))
                    {
                        DoCast(l_Player, eSpells::SpellOrbOfCorrosionAura, true);
                        DoCast(me, eSpells::SpellOrbOfCorrosionPeriodic, true);

                        m_IsMoving = true;
                    }
                });
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply)
                    m_Passengers.insert(p_Passenger->GetGUID());
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target->GetGUID() == me->GetGUID() || p_Target->GetEntry() != me->GetEntry() || !m_IsMoving)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellCorrosiveSlicerDummy)
                {
                    Creature* l_Helya = nullptr;
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Helya = Creature::GetCreature(*me, l_Instance->GetData64(eTovCreatures::CreatureBossHelya));

                    if (!l_Helya)
                        return;

                    float l_Range = me->GetDistance(p_Target);

                    std::list<Player*> l_PlayerList;

                    me->GetPlayerListInGrid(l_PlayerList, l_Range);

                    if (!l_PlayerList.empty())
                    {
                        l_PlayerList.remove_if([this, l_Helya, p_Target](Player* p_Player) -> bool
                        {
                            /// Not attackable
                            if (!l_Helya->IsValidAttackTarget(p_Player))
                                return true;

                            /// Not in the beam line
                            if (!p_Player->IsInBetween(me, p_Target, 1.0f))
                                return true;

                            /// Not between the two orbs
                            if (me->GetDistance(p_Player) > me->GetDistance(p_Target))
                                return true;

                            return false;
                        });
                    }

                    for (Player*& l_Player : l_PlayerList)
                        l_Helya->CastSpell(l_Player, eSpells::SpellCorrosiveSlicerDamage, true);
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = 6.0f;
            }

            void JustDespawned() override
            {
                for (uint64 l_Guid : m_Passengers)
                {
                    if (Creature* l_Passenger = Creature::GetCreature(*me, l_Guid))
                    {
                        if (l_Passenger->ToCreature())
                        {
                            l_Passenger->RemoveAllAuras();
                            l_Passenger->DespawnOrUnsummon();
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                /// If target is dead or not targetable anymore, just move to its position no matter what
                if (Player* l_Player = Player::GetPlayer(*me, m_TargetGUID))
                {
                    if (l_Player->isDead() || !me->IsValidAttackTarget(l_Player))
                    {
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(0, l_Player->GetPosition());
                    }
                }

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eTovData::DataBossHelya) != EncounterState::IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_helya_orb_of_corrosionAI(p_Creature);
        }
};

/// Helarjar Mistwatcher - 116335
class npc_tov_helya_helarjar_mistwatcher : public CreatureScript
{
    public:
        npc_tov_helya_helarjar_mistwatcher() : CreatureScript("npc_tov_helya_helarjar_mistwatcher") { }

        struct npc_tov_helya_helarjar_mistwatcherAI : public Scripted_NoMovementAI
        {
            npc_tov_helya_helarjar_mistwatcherAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eSpells
            {
                SpellMistInfusionCast   = 228854,
                SpellBleakEruption      = 231863
            };

            enum eEvent
            {
                EventMistInfusion = 1
            };

            InstanceScript* m_Instance;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->SetInCombatWithZone();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 3);

                events.Reset();

                events.ScheduleEvent(eEvent::EventMistInfusion, 2 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellMistInfusionCast)
                    DoCast(me, eSpells::SpellBleakEruption, true);
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
                    case eEvent::EventMistInfusion:
                    {
                        if (m_Instance != nullptr)
                        {
                            uint64 l_Guid = m_Instance->GetData64(eTovCreatures::CreatureBossHelya);

                            if (Creature* l_Helya = Creature::GetCreature(*me, l_Guid))
                                me->SetFacingTo(me->GetAngle(l_Helya));

                            AddTimedDelayedOperation(10, [this, l_Guid]() -> void
                            {
                                if (Creature* l_Helya = Creature::GetCreature(*me, l_Guid))
                                    DoCast(l_Helya, eSpells::SpellMistInfusionCast);
                            });
                        }

                        events.ScheduleEvent(eEvent::EventMistInfusion, 10 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tov_helya_helarjar_mistwatcherAI(p_Creature);
        }
};

/// Taint of the Sea - 228054
class spell_tov_helya_taint_of_the_sea : public SpellScriptLoader
{
    public:
        spell_tov_helya_taint_of_the_sea() : SpellScriptLoader("spell_tov_helya_taint_of_the_sea") { }

        enum eSpell
        {
            SpellTaintedEssence = 228052
        };

        class spell_tov_helya_taint_of_the_sea_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_taint_of_the_sea_AuraScript);

            bool m_Dispel = false;

            void HandleDispel(DispelInfo* p_DispelInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Caster || !GetUnitOwner() || m_Dispel)
                    return;

                m_Dispel = true;

                l_Caster->CastSpell(l_Target, eSpell::SpellTaintedEssence, true);
            }

            void Register() override
            {
                OnDispel += AuraDispelFn(spell_tov_helya_taint_of_the_sea_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_taint_of_the_sea_AuraScript();
        }
};

/// Helya - 232229
class spell_tov_helya_energizer : public SpellScriptLoader
{
    public:
        spell_tov_helya_energizer() : SpellScriptLoader("spell_tov_helya_energizer") { }

        enum eAction
        {
            ActionFullEnergy
        };

        class spell_tov_helya_energizer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_energizer_AuraScript);

            bool m_FullPower        = false;
            int32 m_PowerCount      = 0;
            uint32 m_ApplyTime      = 0;
            uint32 m_EnergizeTime   = 0;

            bool Load() override
            {
                m_ApplyTime = uint32(time(nullptr));

                if (Unit* l_Caster = GetCaster())
                {
                    /// Time needed to reach full power
                    switch (GetCaster()->GetMap()->GetDifficultyID())
                    {
                        case Difficulty::DifficultyRaidNormal:
                        {
                            m_EnergizeTime = 56;
                            break;
                        }
                        case Difficulty::DifficultyRaidHeroic:
                        {
                            m_EnergizeTime = 52;
                            break;
                        }
                        case Difficulty::DifficultyRaidMythic:
                        {
                            m_EnergizeTime = 42;
                            break;
                        }
                        default:
                        {
                            m_EnergizeTime = 61;
                            break;
                        }
                    }

                    /// P1 starts at 79
                    m_ApplyTime -= CalculatePct(m_EnergizeTime, 79);
                }

                return true;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat() || !m_EnergizeTime || !l_Caster->IsAIEnabled)
                    return;

                m_PowerCount = l_Caster->GetPower(Powers::POWER_ENERGY);

                if (!m_PowerCount && m_FullPower)
                {
                    m_ApplyTime = uint32(time(nullptr));
                    m_FullPower = false;
                }

                if (m_PowerCount < 100)
                {
                    uint32 l_CurrTime   = uint32(time(nullptr));
                    int32 l_CurPower    = l_Caster->GetPower(Powers::POWER_ENERGY);
                    int32 l_NewPower    = std::min<uint32>((l_CurrTime - m_ApplyTime) * 100 / m_EnergizeTime, 100);

                    if (l_NewPower <= l_CurPower)
                        return;

                    l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, l_NewPower - l_CurPower, Powers::POWER_ENERGY);
                }
                else
                {
                    if (!m_FullPower)
                    {
                        m_FullPower = true;

                        if (l_Caster->IsAIEnabled)
                            l_Caster->AI()->DoAction(eAction::ActionFullEnergy);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_energizer_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_energizer_AuraScript();
        }
};

/// Orb of Corruption - 227926
/// Orb of Corrosion - 228061
class spell_tov_helya_orb_of_corruption : public SpellScriptLoader
{
    public:
        spell_tov_helya_orb_of_corruption() : SpellScriptLoader("spell_tov_helya_orb_of_corruption") { }

        class spell_tov_helya_orb_of_corruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_orb_of_corruption_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Orb = GetCaster()->ToCreature())
                    l_Orb->DespawnOrUnsummon();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_helya_orb_of_corruption_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_orb_of_corruption_AuraScript();
        }
};

/// Tentacle Strike - 228730
class spell_tov_helya_tentacle_strike : public SpellScriptLoader
{
    public:
        spell_tov_helya_tentacle_strike() : SpellScriptLoader("spell_tov_helya_tentacle_strike") { }

        class spell_tov_helya_tentacle_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_helya_tentacle_strike_SpellScript);

            enum eSpell
            {
                SpellTentacleSlam = 228731
            };

            bool m_IsEmpty = false;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                m_IsEmpty = p_Targets.empty();
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_IsEmpty)
                    return;

                l_Caster->CastSpell(l_Caster, eSpell::SpellTentacleSlam, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_helya_tentacle_strike_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_ENNEMIES_IN_CYLINDER);
                AfterCast += SpellCastFn(spell_tov_helya_tentacle_strike_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_helya_tentacle_strike_SpellScript();
        }
};

/// Fetid Rot - 193367
class spell_tov_helya_fetid_rot : public SpellScriptLoader
{
    public:
        spell_tov_helya_fetid_rot() : SpellScriptLoader("spell_tov_helya_fetid_rot") { }

        enum eSpells
        {
            SpellFetidRotDoT    = 232408,
            SpellFetidRotAoE    = 228397,
            SpellFetidFission   = 158534
        };

        class spell_tov_helya_fetid_rot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_fetid_rot_AuraScript);

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_AurEff->GetBase()->SetStackAmount(l_Caster->HasAura(eSpells::SpellFetidFission) ? 4 : 3);
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster && l_Target)
                {
                    if (l_Target->GetMap() && l_Target->GetMap()->IsMythic())
                        l_Target->CastSpell(l_Target, eSpells::SpellFetidRotAoE, true, nullptr, nullptr, l_Caster->GetGUID());

                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellFetidRotDoT, l_Target->GetMap()->GetDifficultyID()))
                    {
                        int32 l_BP = l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints * p_AurEff->GetBase()->GetStackAmount();

                        l_Target->CastCustomSpell(l_Target, eSpells::SpellFetidRotDoT, &l_BP, nullptr, nullptr, true);
                    }
                }

                p_AurEff->GetBase()->DropStack();
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster && l_Target)
                {
                    /// Mythic: The Fetid Rot in phase 2 will now attempt to spread each time a stack is removed, rather than at the end of the debuff.
                    if (l_Target->GetMap() && !l_Target->GetMap()->IsMythic())
                        l_Target->CastSpell(l_Target, eSpells::SpellFetidRotAoE, true, nullptr, nullptr, l_Caster->GetGUID());
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tov_helya_fetid_rot_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_fetid_rot_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_helya_fetid_rot_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_fetid_rot_AuraScript();
        }
};

/// Fetid Rot (AoE) - 228397
class spell_tov_helya_fetid_rot_aoe : public SpellScriptLoader
{
    public:
        spell_tov_helya_fetid_rot_aoe() : SpellScriptLoader("spell_tov_helya_fetid_rot_aoe") { }

        class spell_tov_helya_fetid_rot_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_helya_fetid_rot_aoe_SpellScript);

            enum eSpell
            {
                SpellFetidRot = 193367
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || l_Caster == l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpell::SpellFetidRot, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_tov_helya_fetid_rot_aoe_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_helya_fetid_rot_aoe_SpellScript();
        }
};

/// Darkness - 228383
class spell_tov_helya_darkness_energizer : public SpellScriptLoader
{
    public:
        spell_tov_helya_darkness_energizer() : SpellScriptLoader("spell_tov_helya_darkness_energizer") { }

        enum eAction
        {
            ActionFullEnergy
        };

        class spell_tov_helya_darkness_energizer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_darkness_energizer_AuraScript);

            uint8 m_TickNumber = 0;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat() || !l_Caster->IsAIEnabled)
                    return;

                int32 l_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (l_PowerCount < 100)
                {
                    ++m_TickNumber;

                    if (m_TickNumber < 3)
                    {
                        l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, 3, Powers::POWER_ENERGY);

                        l_PowerCount += 3;
                    }
                    else
                    {
                        m_TickNumber = 0;

                        l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, 4, Powers::POWER_ENERGY);

                        l_PowerCount += 4;
                    }
                }

                if (l_PowerCount >= 100)
                    l_Caster->AI()->DoAction(eAction::ActionFullEnergy);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_darkness_energizer_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_darkness_energizer_AuraScript();
        }
};

/// Helya - 231269
class spell_tov_helya_energizer_p2 : public SpellScriptLoader
{
    public:
        spell_tov_helya_energizer_p2() : SpellScriptLoader("spell_tov_helya_energizer_p2") { }

        enum eAction
        {
            ActionFullAlternateEnergy = 1
        };

        enum eSpell
        {
            SpellHelyaDamageImmunity = 228847
        };

        class spell_tov_helya_energizer_p2_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_energizer_p2_AuraScript);

            bool m_FullPower = false;
            int32 m_PowerCount = 0;
            uint32 m_ApplyTime = 0;
            uint32 m_EnergizeTime = 0;

            bool Load() override
            {
                m_ApplyTime = uint32(time(nullptr));

                if (Unit* l_Caster = GetCaster())
                {
                    bool l_Phase3 = !l_Caster->HasAura(eSpell::SpellHelyaDamageImmunity);

                    /// Time needed to reach full power
                    switch (GetCaster()->GetMap()->GetDifficultyID())
                    {
                        case Difficulty::DifficultyRaidHeroic:
                        {
                            if (l_Phase3)
                                m_EnergizeTime = 75;
                            else
                                m_EnergizeTime = 43;

                            break;
                        }
                        case Difficulty::DifficultyRaidMythic:
                        {
                            if (l_Phase3)
                                m_EnergizeTime = 55;
                            else
                                m_EnergizeTime = 45;

                            break;
                        }
                        default:
                        {
                            if (l_Phase3)
                            {
                                if (GetCaster()->GetMap()->IsLFR())
                                    m_EnergizeTime = 92;
                                else
                                    m_EnergizeTime = 77;
                            }
                            else
                                m_EnergizeTime = 45;

                            break;
                        }
                    }

                    /// P3 starts at 56
                    if (l_Phase3)
                        m_ApplyTime -= CalculatePct(m_EnergizeTime, 56);
                }

                return true;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat() || !m_EnergizeTime || !l_Caster->IsAIEnabled)
                    return;

                boss_tov_helya::boss_tov_helyaAI* l_AI = CAST_AI(boss_tov_helya::boss_tov_helyaAI, l_Caster->GetAI());
                if (!l_AI)
                    return;

                m_PowerCount = l_Caster->GetPower(Powers::POWER_ALTERNATE_POWER);

                if (!m_PowerCount && m_FullPower)
                {
                    m_ApplyTime = uint32(time(nullptr));
                    m_FullPower = false;
                }

                if (m_PowerCount < 100)
                {
                    uint32 l_CurrTime   = uint32(time(nullptr));
                    int32 l_CurPower    = l_Caster->GetPower(Powers::POWER_ALTERNATE_POWER);
                    int32 l_NewPower    = std::min<uint32>((l_CurrTime - m_ApplyTime) * 100 / m_EnergizeTime, 100);

                    if (l_CurPower > l_NewPower && l_AI->m_InfusionEnergized > 0)
                        l_CurPower -= l_AI->m_InfusionEnergized;

                    if (l_NewPower <= l_CurPower)
                        return;

                    l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, l_NewPower - l_CurPower, Powers::POWER_ALTERNATE_POWER);
                }
                else
                {
                    if (!m_FullPower)
                    {
                        m_FullPower = true;

                        if (l_Caster->IsAIEnabled)
                            l_Caster->AI()->DoAction(eAction::ActionFullAlternateEnergy);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_energizer_p2_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_energizer_p2_AuraScript();
        }
};

/// Fury of the Maw - 228300
class spell_tov_helya_fury_of_the_maw : public SpellScriptLoader
{
    public:
        spell_tov_helya_fury_of_the_maw() : SpellScriptLoader("spell_tov_helya_fury_of_the_maw") { }

        enum eSpells
        {
            SpellFuryOfTheMawAT     = 228027,
            SpellFuryOfTheMawDummy  = 230356,
            SpellHelyaPowerP2       = 231269
        };

        class spell_tov_helya_fury_of_the_maw_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_fury_of_the_maw_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->GetMap()->IsMythic())
                    p_Duration = 24 * TimeConstants::IN_MILLISECONDS;
                else
                    p_Duration = 32 * TimeConstants::IN_MILLISECONDS;
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                l_Caster->CastSpell(g_FuryOfTheMawSpawnPos, eSpells::SpellFuryOfTheMawDummy, true);

                /// Should be last tick
                if (p_AurEff->GetTickNumber() == (p_AurEff->GetTotalTicks() + 1))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::SpellHelyaPowerP2, true);

                    if (l_Caster->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    {
                        uint8 l_I = 0;
                        for (Position const& l_Pos : g_LongboatSpawnPos)
                        {
                            if (Creature* l_Boat = l_Caster->SummonCreature(eTovCreatures::CreatureKvaldirLongboat, l_Pos))
                            {
                                if (l_Boat->IsAIEnabled)
                                {
                                    if (Creature* l_Passenger = l_Caster->SummonCreature(l_I == 0 ? eTovCreatures::CreatureNightWatchMariner : eTovCreatures::CreatureGrimelord, l_Boat->GetPosition()))
                                        l_Boat->AI()->SetGUID(l_Passenger->GetGUID());
                                }
                            }

                            ++l_I;
                        }
                    }
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_tov_helya_fury_of_the_maw_AuraScript::CalculateMaxDuration);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_fury_of_the_maw_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_fury_of_the_maw_AuraScript();
        }
};

/// Fury of the Maw (DoT) - 228055
class spell_tov_helya_fury_of_the_maw_dot : public SpellScriptLoader
{
    public:
        spell_tov_helya_fury_of_the_maw_dot() : SpellScriptLoader("spell_tov_helya_fury_of_the_maw_dot") { }

        class spell_tov_helya_fury_of_the_maw_dot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_fury_of_the_maw_dot_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Target = GetTarget()->ToPlayer())
                        l_Target->SendApplyMovementForce(l_Caster->GetGUID(), true, g_ForcePos, 6.0f, 0, G3D::Vector3(-3.0f, -5.196153f, 0.0f));
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Target = GetTarget()->ToPlayer())
                        l_Target->SendApplyMovementForce(l_Caster->GetGUID(), false, Position());
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_tov_helya_fury_of_the_maw_dot_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_helya_fury_of_the_maw_dot_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_fury_of_the_maw_dot_AuraScript();
        }
};

/// Helya - 228546
class spell_tov_helya_energizer_p3 : public SpellScriptLoader
{
    public:
        spell_tov_helya_energizer_p3() : SpellScriptLoader("spell_tov_helya_energizer_p3") { }

        enum eAction
        {
            ActionFullEnergy
        };

        class spell_tov_helya_energizer_p3_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_energizer_p3_AuraScript);

            bool m_FullPower = false;
            int32 m_PowerCount = 0;
            uint32 m_ApplyTime = 0;
            uint32 m_EnergizeTime = 0;

            bool Load() override
            {
                m_ApplyTime = uint32(time(nullptr));

                if (Unit* l_Caster = GetCaster())
                {
                    /// Time needed to reach full power
                    switch (GetCaster()->GetMap()->GetDifficultyID())
                    {
                        case Difficulty::DifficultyRaidHeroic:
                        {
                            m_EnergizeTime = 48;
                            break;
                        }
                        case Difficulty::DifficultyRaidMythic:
                        {
                            m_EnergizeTime = 43;
                            break;
                        }
                        default:
                        {
                            m_EnergizeTime = 51;
                            break;
                        }
                    }
                }

                /// Starts at 61 energy
                m_ApplyTime -= CalculatePct(m_EnergizeTime, 61);
                return true;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat() || !m_EnergizeTime)
                    return;

                m_PowerCount = l_Caster->GetPower(Powers::POWER_ENERGY);

                if (!m_PowerCount && m_FullPower)
                {
                    m_ApplyTime = uint32(time(nullptr));
                    m_FullPower = false;
                }

                if (m_PowerCount < 100)
                {
                    uint32 l_CurrTime   = uint32(time(nullptr));
                    int32 l_CurPower    = l_Caster->GetPower(Powers::POWER_ENERGY);
                    int32 l_NewPower    = std::min<uint32>((l_CurrTime - m_ApplyTime) * 100 / m_EnergizeTime, 100);

                    if (l_NewPower <= l_CurPower)
                        return;

                    l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, l_NewPower - l_CurPower, Powers::POWER_ENERGY);
                }
                else
                {
                    if (!m_FullPower)
                    {
                        m_FullPower = true;

                        if (l_Caster->IsAIEnabled)
                            l_Caster->AI()->DoAction(eAction::ActionFullEnergy);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_energizer_p3_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_energizer_p3_AuraScript();
        }
};

/// Corrupted Axion - 232449
class spell_tov_helya_corrupted_axion : public SpellScriptLoader
{
    public:
        spell_tov_helya_corrupted_axion() : SpellScriptLoader("spell_tov_helya_corrupted_axion") { }

        enum eSpells
        {
            SpellCorruptedAxionDmg  = 232450,
            SpellCorruptedAxionAoE  = 232452
        };

        class spell_tov_helya_corrupted_axion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_helya_corrupted_axion_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetExplTargetDest() && p_Targets.empty())
                    l_Caster->CastSpell(GetExplTargetDest(), eSpells::SpellCorruptedAxionAoE, true);
                else if (!p_Targets.empty())
                {
                    WorldObject* l_Object = JadeCore::Containers::SelectRandomContainerElement(p_Targets);
                    if (Unit* l_Target = l_Object->ToUnit())
                        l_Caster->CastSpell(l_Target, eSpells::SpellCorruptedAxionDmg, true);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_helya_corrupted_axion_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_helya_corrupted_axion_SpellScript();
        }
};

/// Corrupted Axion - 232450
class spell_tov_helya_corrupted_axion_absorb : public SpellScriptLoader
{
    public:
        spell_tov_helya_corrupted_axion_absorb() : SpellScriptLoader("spell_tov_helya_corrupted_axion_absorb") { }

        enum eSpell
        {
            SpellCorrupted = 232350
        };

        class spell_tov_helya_corrupted_axion_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_corrupted_axion_absorb_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetTarget();
                    if (!l_Caster || !l_Target)
                        return;

                    if (!l_Caster->GetMap()->IsMythic())
                        return;

                    l_Caster->CastSpell(l_Target, eSpell::SpellCorrupted, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_helya_corrupted_axion_absorb_AuraScript::AfterRemove, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_corrupted_axion_absorb_AuraScript();
        }
};

/// Corrupted - 232350
class spell_tov_helya_corrupted : public SpellScriptLoader
{
    public:
        spell_tov_helya_corrupted() : SpellScriptLoader("spell_tov_helya_corrupted") { }

        enum eSpells
        {
            SpellBleakEruption              = 231862,
            SpellCorruptedTransformFemale   = 232358,
            SpellCorruptedTransformMale     = 232364
        };

        class spell_tov_helya_corrupted_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_helya_corrupted_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->getGender() == Gender::GENDER_FEMALE)
                    l_Caster->CastSpell(l_Target, eSpells::SpellCorruptedTransformFemale, true);
                else
                    l_Caster->CastSpell(l_Target, eSpells::SpellCorruptedTransformMale, true);
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->GetHealthPct() > float(p_AurEff->GetAmount()))
                    return;

                p_AurEff->GetBase()->Remove();
                l_Target->RemoveAura(eSpells::SpellBleakEruption);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->InterruptNonMeleeSpells(true);

                if (l_Target->getGender() == Gender::GENDER_FEMALE)
                    l_Target->RemoveAura(eSpells::SpellCorruptedTransformFemale);
                else
                    l_Target->RemoveAura(eSpells::SpellCorruptedTransformMale);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tov_helya_corrupted_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_helya_corrupted_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_helya_corrupted_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_helya_corrupted_AuraScript();
        }
};

/// Mist Infusion - 228627
class spell_tov_helya_mist_infusion : public SpellScriptLoader
{
    public:
        spell_tov_helya_mist_infusion() : SpellScriptLoader("spell_tov_helya_mist_infusion") { }

        enum eSpell
        {
            SpellMistInfusionTriggered = 228854
        };

        class spell_tov_helya_mist_infusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_helya_mist_infusion_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpell::SpellMistInfusionTriggered);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_tov_helya_mist_infusion_SpellScript::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_tov_helya_mist_infusion_SpellScript();
        }
};

/// Mist Infusion (triggered) - 228854
class spell_tov_helya_mist_infusion_triggered : public SpellScriptLoader
{
    public:
        spell_tov_helya_mist_infusion_triggered() : SpellScriptLoader("spell_tov_helya_mist_infusion_triggered") { }

        enum eSpell
        {
            SpellBleakEruption = 231863
        };

        class spell_tov_helya_mist_infusion_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_helya_mist_infusion_triggered_SpellScript);

            void HandleOnInterrupt(uint32 /*p_Time*/)
            {
                Erupt(GetCaster());
            }

            void HandleAfterCast()
            {
                Erupt(GetCaster());
            }

            void Erupt(Unit* p_Caster)
            {
                if (!p_Caster || !p_Caster->IsPlayer())
                    return;

                p_Caster->CastSpell(p_Caster, eSpell::SpellBleakEruption, true);
            }

            void Register() override
            {
                OnInterrupt += SpellInterruptFn(spell_tov_helya_mist_infusion_triggered_SpellScript::HandleOnInterrupt);
                AfterCast += SpellCastFn(spell_tov_helya_mist_infusion_triggered_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_helya_mist_infusion_triggered_SpellScript();
        }
};

/// Fury of the Maw - 228027
class at_tov_helya_fury_of_the_maw : public AreaTriggerEntityScript
{
    public:
        at_tov_helya_fury_of_the_maw() : AreaTriggerEntityScript("at_tov_helya_fury_of_the_maw") { }

        enum eSpells
        {
            SpellBilewaterLiquefaction  = 227992,
            SpellDecay                  = 228121
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Diff*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<AreaTrigger*> l_ATList;

                l_Caster->GetAreaTriggerListWithSpellIDInRange(l_ATList, eSpells::SpellBilewaterLiquefaction, 200.0f);
                l_Caster->GetAreaTriggerListWithSpellIDInRange(l_ATList, eSpells::SpellDecay, 200.0f);

                for (AreaTrigger* l_AT : l_ATList)
                {
                    if (p_AreaTrigger->IsInPolygon(l_AT))
                    {
                        if (l_AT->m_positionZ <= 16.0f)
                            l_AT->Remove();
                    }
                }
            }
        }

        bool OnAddTarget(AreaTrigger* /*p_AreaTrigger*/, Unit* p_Target) override
        {
            if (p_Target->m_positionZ >= 16.5f)
                return true;

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_tov_helya_fury_of_the_maw();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_tov_helya()
{
    /// Boss
    new boss_tov_helya();

    /// Creatures
    new npc_tov_helya_bilewater_slime();
    new npc_tov_helya_orb_of_corruption();
    new npc_tov_helya_striking_tentacle();
    new npc_tov_helya_gripping_tentacle();
    new npc_tov_helya_kvaldir_longboat();
    new npc_tov_helya_grimelord();
    new npc_tov_helya_night_watch_mariner();
    new npc_tov_helya_night_watch_mariner_image();
    new npc_tov_helya_decaying_minion();
    new npc_tov_helya_orb_of_corrosion();
    new npc_tov_helya_helarjar_mistwatcher();

    /// Spells
    new spell_tov_helya_taint_of_the_sea();
    new spell_tov_helya_energizer();
    new spell_tov_helya_orb_of_corruption();
    new spell_tov_helya_tentacle_strike();
    new spell_tov_helya_fetid_rot();
    new spell_tov_helya_fetid_rot_aoe();
    new spell_tov_helya_darkness_energizer();
    new spell_tov_helya_energizer_p2();
    new spell_tov_helya_fury_of_the_maw();
    new spell_tov_helya_fury_of_the_maw_dot();
    new spell_tov_helya_energizer_p3();
    new spell_tov_helya_corrupted_axion();
    new spell_tov_helya_corrupted_axion_absorb();
    new spell_tov_helya_corrupted();
    new spell_tov_helya_mist_infusion();
    new spell_tov_helya_mist_infusion_triggered();

    /// AreaTrigger
    new at_tov_helya_fury_of_the_maw();
}
#endif
