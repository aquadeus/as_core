////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"

/// Last Update 7.3.5 - Build 26365
/// Garothi Worldbreaker - 122450
class boss_garothi_worldbreaker : public CreatureScript
{
    public:
        boss_garothi_worldbreaker() : CreatureScript("boss_garothi_worldbreaker") { }

        struct boss_garothi_worldbreakerAI : BossAI
        {
            boss_garothi_worldbreakerAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataGarothiWorldbreaker)
            {
                me->SetPhaseMask(3, true);
                if (Creature* l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f))
                    l_Decimator->SetPhaseMask(2, true);
                if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                    l_Annihilator->SetPhaseMask(2, true);
                me->SetPhaseMask(2, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                m_IntroDone = false;
            }

            enum eConversations
            {
                Intro   = 5514
            };

            enum eAnims
            {
                AnimCannonDestroyed = 13264
            };

            enum eTalks
            {
                TalkAggro,
                TalkAnnihilation,
                TalkDecimation,
                TalkApocalypseDrive,
                TalkApocalypseDriveEnd,
                TalkKilledPlayer,
                TalkWipe,
                TalkDeath,

                TalkApocalypseDriveWarning,
                TalkEradicationWarning,

                TalkExterminated
            };

            enum eSpells
            {
                /// Intro
                Appearing               = 246256,
                EradicationIntro        = 249114,

                /// Phase 1
                Melee                   = 248230,
                FelBombardment          = 246220,
                Decimation              = 244399,
                SummonAnnihilation      = 244790,
                Annihilation            = 244294,
                Carnage                 = 244106,
                CannonChooser            = 245124,

                ApocalypseDrive         = 244152,
                ApocalypseDriveDamage   = 240277,
                Eradication             = 244969,

                Empowered               = 245237,
                SearingBarrage          = 244395,
                SearingBarrage2         = 246368,

                /// Heroic
                SurgingFel              = 246655,

                /// Mythic
                HaywireDecimator        = 246897,
                HaywireAnnihilator      = 246965,
                HaywireDecimation       = 246919,

                LuringDestruction       = 247159
            };

            enum eEvents
            {
                EventFelBombardment = 1,
                EventDecimation,
                EventAnnihilation
            };

            enum eApocalypseDriveState
            {
                NeverCasted,
                Charging,
                Disarmed,
                EradicationToCast,
                EradicationCasted,
                ChargingSecond,
                DisarmedSecond,
                EradicationToCastSecond,
                Finished
            };

            bool m_IntroDone;

            Position const m_Center = { -3299.15f, 9772.60f, -63.36f };
            Position const m_SouthWestCorner = { -3334.18f, 9749.39f, -63.49f };
            eApocalypseDriveState m_ApocalypseDrive;
            bool m_CanAutoAttack;

            int32 m_SurgingFelTimer;
            int32 m_MeleeTimer;

            uint32 m_BlightscaleWormsDecimated;

            std::vector<Position> m_FelSurgePositions =
            {
                { -3292.0f, 9810.797f, -64.07504f, 4.577713f },
                { -3322.0f, 9810.800f, -64.84264f, 4.577713f },
                { -3307.0f, 9810.800f, -63.85229f, 4.577713f },
                { -3262.0f, 9810.797f, -63.93631f, 4.577713f },
                { -3277.0f, 9810.800f, -63.68398f, 4.577713f }
            };

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanBeHitInTheBack() override
            {
                return false;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);

                _JustDied();
            }

            void Reset() override
            {
                _Reset();

                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_BYTES_1, 50331648);
                me->NearTeleportTo(*me);

                m_ApocalypseDrive = eApocalypseDriveState::NeverCasted;

                if (m_IntroDone)
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->RemoveAura(eSpells::Empowered);
                me->RemoveAura(eSpells::SearingBarrage);
                me->RemoveAura(eSpells::SearingBarrage2);

                m_SurgingFelTimer = 0;
                m_MeleeTimer = 0;
                m_CanAutoAttack = true;

                m_BlightscaleWormsDecimated = 0;
                for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->SetWorldState(13765, 0);
                }

                std::list<Creature*> l_FelSurges;
                me->GetCreatureListWithEntryInGrid(l_FelSurges, eCreatures::NpcFelSurge, 150.0f);
                for (Creature* l_FelSurge : l_FelSurges)
                    l_FelSurge->DespawnOrUnsummon();

                events.Reset();
            }

            void EnterCombat(Unit*  /*p_Attacker*/) override
            {
                _EnterCombat();

                if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                    || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                {
                    for (Position const& l_Pos : m_FelSurgePositions)
                    {
                        if (Creature* l_FelSurge = me->SummonCreature(eCreatures::NpcFelSurge, l_Pos))
                        {
                            l_FelSurge->SetReactState(ReactStates::REACT_PASSIVE);
                            l_FelSurge->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        }
                    }
                }

                Talk(eTalks::TalkAggro);
                DefaultEvents();
            }

            void OnContactPosition(Unit* p_Source, Position& p_Dest)
            {
                float l_X, l_Y, l_Z;

                float l_Dist = me->GetFloatValue(UNIT_FIELD_COMBAT_REACH);

                me->GetContactPoint(p_Source, l_X, l_Y, l_Z, l_Dist);
                p_Dest.Relocate(l_X, l_Y, l_Z);
            }

            void DefaultEvents()
            {
                switch (m_ApocalypseDrive)
                {
                    case boss_garothi_worldbreaker::boss_garothi_worldbreakerAI::NeverCasted:
                    {
                        events.ScheduleEvent(eEvents::EventAnnihilation, 8 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventFelBombardment, 9400);
                        events.ScheduleEvent(eEvents::EventDecimation, 23800);
                        break;
                    }
                    case boss_garothi_worldbreaker::boss_garothi_worldbreakerAI::EradicationCasted:
                    case boss_garothi_worldbreaker::boss_garothi_worldbreakerAI::Finished:
                    {
                        Creature* l_Decimator = nullptr;
                        if ((l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f)))
                            events.ScheduleEvent(eEvents::EventDecimation, 15800);
                        if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                            events.ScheduleEvent(eEvents::EventAnnihilation, l_Decimator ? 31600 : 15800);

                        events.ScheduleEvent(eEvents::EventFelBombardment, me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic ? 23400 : 23100);

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
                    {
                        AddTimedDelayedOperation(2500, [this]()-> void
                        {
                            me->SetPhaseMask(3, true);
                            if (Creature* l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f))
                                l_Decimator->SetPhaseMask(1, true);
                            if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                                l_Annihilator->SetPhaseMask(1, true);
                            me->SetPhaseMask(1, true);

                            me->SetBig(true);
                            me->CastSpell(me, eSpells::Appearing, true);

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Intro, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        AddTimedDelayedOperation(8500, [this]()-> void
                        {
                            me->CastSpell(me, eSpells::EradicationIntro, false);
                        });
                        break;
                    }
                    case ACTION_2:
                    {
                        m_ApocalypseDrive = m_ApocalypseDrive == eApocalypseDriveState::Charging ? eApocalypseDriveState::Disarmed : eApocalypseDriveState::DisarmedSecond;
                        me->RemoveAura(eSpells::ApocalypseDrive);
                        break;
                    }
                    case ACTION_3:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        InstanceScript* l_InstanceScript = me->GetInstanceScript();
                        if (!l_InstanceScript)
                            return;

                        if (Creature* l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f))
                        {
                            l_Decimator->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Decimator->AddUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                            l_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, l_Decimator);
                            l_Decimator->SetHealth(l_Decimator->GetMaxHealth());
                        }
                        if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                        {
                            l_Annihilator->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Annihilator->AddUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                            l_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, l_Annihilator);
                            l_Annihilator->SetHealth(l_Annihilator->GetMaxHealth());
                        }

                        if (m_ApocalypseDrive == eApocalypseDriveState::Charging || m_ApocalypseDrive == eApocalypseDriveState::ChargingSecond)
                            me->CastSpell(me, eSpells::ApocalypseDriveDamage, true);

                        m_ApocalypseDrive = m_ApocalypseDrive > eApocalypseDriveState::EradicationToCast ? eApocalypseDriveState::EradicationToCastSecond : eApocalypseDriveState::EradicationToCast;

                        break;
                    }
                    case ACTION_4:
                    {
                        if ((++m_BlightscaleWormsDecimated) >= 16)
                        {
                            for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    l_Player->SetWorldState(13765, 1); ///< Criteria for achievement
                            }
                        }
                        break;
                    }
                    case ACTION_5:
                    {
                        Talk(eTalks::TalkExterminated);
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
                    case eSpells::EradicationIntro:
                    {
                        AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            m_IntroDone = true;
                        });

                        break;
                    }
                    case eSpells::Eradication:
                    {
                        me->CastSpell(me, eSpells::Empowered, true);
                        if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                        {
                            if (!me->HasAura(eSpells::SearingBarrage))
                                me->CastSpell(me, eSpells::SearingBarrage, true);
                            else
                                me->CastSpell(me, eSpells::SearingBarrage2, true);
                        }
                        break;
                    }
                    default:
                        break;
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

                if (!urand(0, 4))
                    Talk(eTalks::TalkKilledPlayer);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                events.Update(p_Diff);
                m_MeleeTimer -= p_Diff;

                if (!UpdateVictim())
                    return;

                if (m_ApocalypseDrive == eApocalypseDriveState::Charging
                    || m_ApocalypseDrive == eApocalypseDriveState::ChargingSecond)
                {
                    m_SurgingFelTimer -= p_Diff;
                    if (m_SurgingFelTimer < 0)
                    {
                        std::list<Creature*> l_FelSurges;
                        me->GetCreatureListWithEntryInGrid(l_FelSurges, eCreatures::NpcFelSurge, 100.0f);

                        if (!l_FelSurges.empty())
                        {
                            Creature* l_FelSurge = JadeCore::Containers::SelectRandomContainerElement(l_FelSurges);
                            if (l_FelSurge)
                                l_FelSurge->CastSpell(l_FelSurge, eSpells::SurgingFel, true);

                            m_SurgingFelTimer = 9 * IN_MILLISECONDS;
                        }
                    }
                }

                std::list<Player*> l_NearPlayers;
                for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                {
                    Player* l_Player = l_Iter->getSource();
                    if (!l_Player || !me->IsWithinDist2d(l_Player, 11.0f) || !l_Player->isAlive() || l_Player->isGameMaster())
                        continue;

                    l_NearPlayers.push_back(l_Player);
                }

                bool l_IsCastingCarnage = false;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                {
                    do
                    {
                        if (Spell const* l_CurrentChanneledSpell = me->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL))
                        {
                            if (SpellInfo const* l_SpellInfo = l_CurrentChanneledSpell->GetSpellInfo())
                            {
                                if (l_SpellInfo->Id == eSpells::Carnage)
                                {
                                    l_IsCastingCarnage = true;
                                    break;
                                }
                            }
                        }
                        return;
                    } while (false);
                }

                if (l_NearPlayers.empty())
                {
                    if (m_CanAutoAttack && !l_IsCastingCarnage)
                        me->CastSpell(me, eSpells::Carnage, false);
                }
                else
                {
                    me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);

                    auto l_Itr = std::find(l_NearPlayers.begin(), l_NearPlayers.end(), me->getVictim());
                    if (l_Itr == l_NearPlayers.end())
                    {
                        l_Itr = l_NearPlayers.begin();
                        std::advance(l_Itr, urand(0, l_NearPlayers.size() - 1));
                        AttackStart(*l_Itr);
                    }
                }

                if (m_ApocalypseDrive == eApocalypseDriveState::Charging
                    || m_ApocalypseDrive == eApocalypseDriveState::ChargingSecond)
                    return;

                if (m_ApocalypseDrive == eApocalypseDriveState::NeverCasted
                    && me->GetHealthPct() < ((me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic) ? 65.0f : 60.0f))
                {
                    if (l_IsCastingCarnage)
                        me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);

                    m_CanAutoAttack = false;

                    Talk(eTalks::TalkApocalypseDrive);
                    Talk(eTalks::TalkApocalypseDriveWarning);

                    events.Reset();

                    m_ApocalypseDrive = eApocalypseDriveState::Charging;
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    InstanceScript* l_InstanceScript = me->GetInstanceScript();
                    if (!l_InstanceScript)
                        return;

                    if (Creature* l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f))
                    {
                        l_Decimator->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        l_Decimator->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                        l_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, l_Decimator, 2);
                    }
                    if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                    {
                        l_Annihilator->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        l_Annihilator->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                        l_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, l_Annihilator, 2);
                    }

                    me->CastSpell(me, eSpells::ApocalypseDrive, true);

                    if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                        || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                        m_SurgingFelTimer = 5 * IN_MILLISECONDS;
                }

                if (m_ApocalypseDrive == eApocalypseDriveState::EradicationCasted
                    && me->GetHealthPct() < ((me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic) ? 35.0f : 20.0f))
                {
                    if (l_IsCastingCarnage)
                        me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);

                    m_CanAutoAttack = false;

                    Talk(eTalks::TalkApocalypseDrive);
                    Talk(eTalks::TalkApocalypseDriveWarning);

                    events.Reset();

                    m_ApocalypseDrive = eApocalypseDriveState::ChargingSecond;
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    InstanceScript* l_InstanceScript = me->GetInstanceScript();
                    if (!l_InstanceScript)
                        return;

                    if (Creature* l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f))
                    {
                        if (!l_Decimator->HasAura(eSpells::HaywireDecimator))
                        {
                            l_Decimator->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Decimator->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                            l_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, l_Decimator, 2);
                        }
                    }
                    if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                    {
                        if (!l_Annihilator->HasAura(eSpells::HaywireAnnihilator))
                        {
                            l_Annihilator->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Annihilator->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
                            l_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, l_Annihilator, 2);
                        }
                    }

                    me->CastSpell(me, eSpells::ApocalypseDrive, true);

                    if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                        || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                        m_SurgingFelTimer = 5 * IN_MILLISECONDS;
                }

                switch (m_ApocalypseDrive)
                {
                    case eApocalypseDriveState::EradicationToCast:
                    {
                        m_ApocalypseDrive = eApocalypseDriveState::EradicationCasted;
                        me->PlayOneShotAnimKitId(eAnims::AnimCannonDestroyed);
                        AddTimedDelayedOperation(2500, [this]() -> void
                        {
                            m_CanAutoAttack = true;
                            Talk(eTalks::TalkApocalypseDriveEnd);
                            Talk(eTalks::TalkEradicationWarning);
                            me->CastSpell(me, eSpells::Eradication, false);

                            if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            {
                                me->CastSpell(me, eSpells::LuringDestruction, true);

                                Position l_Dest = { -3285.261f, 9812.452f, -46.21415f };
                                for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                                {
                                    if (Player* l_Player = l_Iter->getSource())
                                    {
                                        if (!l_Player->isGameMaster())
                                            l_Player->SendApplyMovementForce(me->GetGUID(), true, l_Dest, 14.0f, 1);
                                    }
                                }

                                AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this, l_Dest]()-> void
                                {
                                    for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                                    {
                                        if (Player* l_Player = l_Iter->getSource())
                                        {
                                            if (!l_Player->isGameMaster())
                                                l_Player->SendApplyMovementForce(me->GetGUID(), false, l_Dest);
                                        }
                                    }
                                });
                            }

                            DefaultEvents();
                        });

                        break;
                    }
                    case eApocalypseDriveState::EradicationToCastSecond:
                    {
                        m_ApocalypseDrive = eApocalypseDriveState::Finished;
                        m_CanAutoAttack = false;
                        me->PlayOneShotAnimKitId(eAnims::AnimCannonDestroyed);
                        AddTimedDelayedOperation(2500, [this]() -> void
                        {
                            m_CanAutoAttack = true;
                            Talk(eTalks::TalkApocalypseDriveEnd);
                            Talk(eTalks::TalkEradicationWarning);
                            me->CastSpell(me, eSpells::Eradication, false);

                            if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            {
                                me->CastSpell(me, eSpells::LuringDestruction, true);

                                Position l_Dest = { -3285.261f, 9812.452f, -46.21415f };
                                for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                                {
                                    if (Player* l_Player = l_Iter->getSource())
                                        l_Player->SendApplyMovementForce(me->GetGUID(), true, l_Dest, 14.0f, 1);
                                }

                                AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this, l_Dest]()-> void
                                {
                                    for (Map::PlayerList::const_iterator l_Iter = me->GetMap()->GetPlayers().begin(); l_Iter != me->GetMap()->GetPlayers().end(); ++l_Iter)
                                    {
                                        if (Player* l_Player = l_Iter->getSource())
                                            l_Player->SendApplyMovementForce(me->GetGUID(), false, l_Dest);
                                    }
                                });
                            }

                            DefaultEvents();
                        });
                        break;
                    }
                    default:
                        break;
                }

                uint32 l_Event = events.ExecuteEvent();
                if (l_Event && l_IsCastingCarnage)
                    me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);

                switch (l_Event)
                {
                    case eEvents::EventFelBombardment:
                    {
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eSpells::FelBombardment, false);

                        events.ScheduleEvent(eEvents::EventFelBombardment, me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic ? 15800 : 20700);

                        break;
                    }
                    case eEvents::EventDecimation:
                    {
                        Talk(eTalks::TalkDecimation);

                        Unit* l_Decimator = me->FindNearestCreature(eCreatures::NpcDecimator, 50.0f);
                        if (!l_Decimator)
                            break;

                        for (uint32 l_I = 0; l_I < 5; ++l_I)
                        {
                            Position l_Dest;
                            m_Center.SimplePosXYRelocationByAngle(l_Dest, frand(0.0f, 30.0f), frand(0.0f, 2 * M_PI));
                            me->SummonCreature(eCreatures::NpcBlightscaleWorm, l_Dest, TEMPSUMMON_TIMED_DESPAWN, 15 * IN_MILLISECONDS);
                        }

                        if (!l_Decimator->HasAura(eSpells::HaywireDecimator))
                            me->CastSpell(me, eSpells::Decimation, false);
                        else
                            me->CastSpell(me, eSpells::HaywireDecimation, false);

                        me->CastSpell(me, eSpells::CannonChooser, true);

                        events.ScheduleEvent(eEvents::EventDecimation, 31600);

                        break;
                    }
                    case eEvents::EventAnnihilation:
                    {
                        Talk(eTalks::TalkAnnihilation);

                        bool l_Haywire = false;
                        uint32 l_AnnihilationCount = 0;
                        uint32 l_PlayerCount = me->GetMap()->GetPlayersCountExceptGMs();
                        switch (me->GetMap()->GetDifficultyID())
                        {
                            case Difficulty::DifficultyRaidLFR:
                                l_AnnihilationCount = 2;
                                break;
                            case Difficulty::DifficultyRaidNormal:
                            {
                                if (l_PlayerCount >= 27)
                                    l_AnnihilationCount = 4;
                                else if (l_PlayerCount >= 20)
                                    l_AnnihilationCount = 3;
                                else if (l_PlayerCount >= 14)
                                    l_AnnihilationCount = 2;
                                else
                                    l_AnnihilationCount = 1;

                                break;
                            }
                            case Difficulty::DifficultyRaidHeroic:
                            {
                                l_AnnihilationCount = std::max<uint32>(l_PlayerCount / 4, 2);
                                break;
                            }
                            case Difficulty::DifficultyRaidMythic:
                            {
                                Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f);

                                if (!l_Annihilator || !l_Annihilator->HasAura(eSpells::HaywireAnnihilator))
                                    l_AnnihilationCount = 5;
                                else
                                {
                                    l_AnnihilationCount = 6;
                                    l_Haywire = true;
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                        {
                            for (uint32 l_I = 0; l_I < l_AnnihilationCount; ++l_I)
                            {
                                Position l_Dest = m_SouthWestCorner;
                                l_Dest.m_positionX += frand(5.0f, 55.0f);
                                l_Dest.m_positionY += frand(5.0f, 55.0f);
                                l_Annihilator->CastSpell(l_Dest, eSpells::SummonAnnihilation, true);
                            }
                        }

                        me->CastSpell(me, eSpells::Annihilation, false);
                        me->CastSpell(me, eSpells::CannonChooser, false);

                        events.ScheduleEvent(eEvents::EventAnnihilation, 31600);

                        break;
                    }
                    default:
                        break;
                }

                if (m_CanAutoAttack && m_MeleeTimer < 0)
                {
                    me->CastSpell(me->getVictim(), eSpells::Melee, false);
                    m_MeleeTimer = 2 * TimeConstants::IN_MILLISECONDS;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_garothi_worldbreakerAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Eradication (Intro) - 249114
class spell_garothi_worldbreaker_eradication_intro : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_eradication_intro() : SpellScriptLoader("spell_garothi_worldbreaker_eradication_intro") { }

        class spell_garothi_worldbreaker_eradication_intro_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_eradication_intro_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                uint32 l_Damage = GetHitDamage();
                l_Damage = CalculatePct(l_Damage, 1.0f - (l_Caster->GetExactDist2d(l_Target) / 100.0f));

                if (l_Target->ToCreature())
                {
                    l_Damage = 0;

                    if (l_Target->IsAIEnabled)
                        l_Target->ToCreature()->AI()->DoAction(ACTION_1);
                }

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_garothi_worldbreaker_eradication_intro_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_eradication_intro_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fel Bombardment - 246220
class spell_garothi_worldbreaker_fel_bombardment : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_fel_bombardment() : SpellScriptLoader("spell_garothi_worldbreaker_fel_bombardment") { }

        class spell_garothi_worldbreaker_fel_bombardment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_garothi_worldbreaker_fel_bombardment_AuraScript);

            enum eSpells
            {
                FelBombardmentPeriodic = 244536
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FelBombardmentPeriodic, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_garothi_worldbreaker_fel_bombardment_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garothi_worldbreaker_fel_bombardment_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fel Bombardment (Periodic) - 244536
class spell_garothi_worldbreaker_fel_bombardment_periodic : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_fel_bombardment_periodic() : SpellScriptLoader("spell_garothi_worldbreaker_fel_bombardment_periodic") { }

        class spell_garothi_worldbreaker_fel_bombardment_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_garothi_worldbreaker_fel_bombardment_periodic_AuraScript);

            enum eSpells
            {
                FelBombardmentMissile = 244533
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Position l_Dest = *l_Target;
                l_Target->UpdateAllowedPositionZ(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ);

                l_Caster->CastSpell(l_Dest, eSpells::FelBombardmentMissile, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_garothi_worldbreaker_fel_bombardment_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garothi_worldbreaker_fel_bombardment_periodic_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Decimation (Searcher) - 244399
class spell_garothi_worldbreaker_decimation_searcher : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_decimation_searcher() : SpellScriptLoader("spell_garothi_worldbreaker_decimation_searcher") { }

        class spell_garothi_worldbreaker_decimation_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_decimation_searcher_SpellScript);

            enum eSpells
            {
                DecimationAura          = 244410,
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([](WorldObject const* p_Target) -> bool
                {
                    Player const* l_Player = p_Target->ToPlayer();
                    if (!l_Player || l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });

                uint32 l_TargetCount = 0;
                if (l_Caster->GetMap() && l_Caster->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                    l_TargetCount = 5;
                else
                    l_TargetCount = std::max<uint32>(2, l_Caster->GetMap()->GetPlayersCountExceptGMs() / 5);

                JadeCore::RandomResizeList(p_Targets, l_TargetCount);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Decimator = l_Caster->FindNearestCreature(eCreatures::NpcDecimator, 50.0f);
                if (!l_Decimator)
                    return;

                l_Decimator->CastSpell(l_Target, eSpells::DecimationAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_decimation_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_decimation_searcher_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_garothi_worldbreaker_decimation_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_decimation_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Decimation (Aura) - 244410
class spell_garothi_worldbreaker_decimation_aura : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_decimation_aura() : SpellScriptLoader("spell_garothi_worldbreaker_decimation_aura") { }

        class spell_garothi_worldbreaker_decimation_aura_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_garothi_worldbreaker_decimation_aura_Aurascript);

            enum eSpells
            {
                DecimationMissile = 244448
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Decimator = l_Caster->FindNearestCreature(eCreatures::NpcDecimator, 50.0f);
                if (!l_Decimator)
                    return;

                l_Decimator->CastSpell(l_Target, eSpells::DecimationMissile, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_garothi_worldbreaker_decimation_aura_Aurascript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garothi_worldbreaker_decimation_aura_Aurascript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Haywire Decimation (Searcher) - 246919
class spell_garothi_worldbreaker_haywire_decimation_searcher : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_haywire_decimation_searcher() : SpellScriptLoader("spell_garothi_worldbreaker_haywire_decimation_searcher") { }

        class spell_garothi_worldbreaker_haywire_decimation_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_haywire_decimation_searcher_SpellScript);

            enum eSpells
            {
                DecimationHaywireAura   = 246920
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([](WorldObject const* p_Target) -> bool
                {
                    Player const* l_Player = p_Target->ToPlayer();
                    if (!l_Player || l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Decimator = l_Caster->FindNearestCreature(eCreatures::NpcDecimator, 50.0f);
                if (!l_Decimator)
                    return;

                l_Decimator->CastSpell(l_Target, eSpells::DecimationHaywireAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_haywire_decimation_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_haywire_decimation_searcher_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_garothi_worldbreaker_haywire_decimation_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_haywire_decimation_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Haywire Decimation (Aura) - 246920
class spell_garothi_worldbreaker_haywire_decimation_aura : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_haywire_decimation_aura() : SpellScriptLoader("spell_garothi_worldbreaker_haywire_decimation_aura") { }

        class spell_garothi_worldbreaker_haywire_decimation_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_garothi_worldbreaker_haywire_decimation_aura_AuraScript);

            enum eSpells
            {
                HaywireDecimationMissile = 254946
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Decimator = l_Caster->FindNearestCreature(eCreatures::NpcDecimator, 50.0f);
                if (!l_Decimator)
                    return;

                l_Decimator->CastSpell(l_Target, eSpells::HaywireDecimationMissile, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_garothi_worldbreaker_haywire_decimation_aura_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garothi_worldbreaker_haywire_decimation_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation - 122818
class npc_garothi_worldbreaker_annihilation : public CreatureScript
{
    public:
        npc_garothi_worldbreaker_annihilation() : CreatureScript("npc_garothi_worldbreaker_annihilation") { }

        struct npc_garothi_worldbreaker_annihilationAI : public ScriptedAI
        {
            npc_garothi_worldbreaker_annihilationAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eSpells
            {
                Areatrigger  = 244795
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
            return new npc_garothi_worldbreaker_annihilationAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation (Haywire) - 124330
class npc_garothi_worldbreaker_annihilation_haywire : public CreatureScript
{
    public:
        npc_garothi_worldbreaker_annihilation_haywire() : CreatureScript("npc_garothi_worldbreaker_annihilation_haywire") { }

        struct npc_garothi_worldbreaker_annihilation_haywireAI : public ScriptedAI
        {
            npc_garothi_worldbreaker_annihilation_haywireAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eSpells
            {
                Areatrigger = 244795,
                Shrapnel    = 247044
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    m_Started = true;

                    me->DelayedCastSpell(me, eSpells::Areatrigger, true, 100);
                    if (Creature* l_Annihilator = me->FindNearestCreature(eCreatures::NpcAnnihilator, 50.0f))
                        me->CastSpell(me, eSpells::Shrapnel, false, nullptr, nullptr, l_Annihilator->GetGUID());
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::Shrapnel:
                        me->DespawnOrUnsummon(1);
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_garothi_worldbreaker_annihilation_haywireAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation - 244761 - Annihilation (Haywire) 246971 - Shrapnel - 247044
class spell_garothi_worldbreaker_annihilation : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_annihilation() : SpellScriptLoader("spell_garothi_worldbreaker_annihilation") { }

        class spell_garothi_worldbreaker_annihilation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_annihilation_SpellScript);

            enum eSpells
            {
                Annihilation = 244762
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

                l_Caster->CastSpell(l_Dest, eSpells::Annihilation, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_annihilation_SpellScript::CheckTargetCount, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_garothi_worldbreaker_annihilation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_annihilation_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilator - 122778
/// Decimator   - 122773
class npc_garothi_worldbreaker_cannons : public CreatureScript
{
    public:
        npc_garothi_worldbreaker_cannons() : CreatureScript("npc_garothi_worldbreaker_cannons") { }

        struct npc_garothi_worldbreaker_cannonsAI : public ScriptedAI
        {
            npc_garothi_worldbreaker_cannonsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            enum eSpells
            {
                HaywireDecimator    = 246897,
                HaywireAnnihilator  = 246965
            };

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_GarothiWorldbreaker = me->FindNearestCreature(eCreatures::BossGarothiWorldbreaker, 50.0f);
                if (!l_GarothiWorldbreaker || !l_GarothiWorldbreaker->IsAIEnabled)
                    return;

                l_GarothiWorldbreaker->AI()->DoAction(ACTION_2);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                if (me->GetHealth() > p_Damage)
                    return;

                p_Damage = 0;

                switch (me->GetEntry())
                {
                    case eCreatures::NpcDecimator:
                        me->CastSpell(me, eSpells::HaywireDecimator, true);
                        break;
                    case eCreatures::NpcAnnihilator:
                        me->CastSpell(me, eSpells::HaywireAnnihilator, true);
                        break;
                    default:
                        break;
                }

                Creature* l_GarothiWorldbreaker = me->FindNearestCreature(eCreatures::BossGarothiWorldbreaker, 50.0f);
                if (!l_GarothiWorldbreaker || !l_GarothiWorldbreaker->IsAIEnabled)
                    return;

                l_GarothiWorldbreaker->AI()->DoAction(ACTION_2);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_garothi_worldbreaker_cannonsAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Apocalypse Drive - 244152
class spell_garothi_worldbreaker_apocalypse_drive : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_apocalypse_drive() : SpellScriptLoader("spell_garothi_worldbreaker_apocalypse_drive") { }

        class spell_garothi_worldbreaker_apocalypse_drive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_garothi_worldbreaker_apocalypse_drive_AuraScript);

            enum eSpells
            {
                ApocalypseDriveDamage = 253300
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->ToCreature()->AI()->DoAction(ACTION_3);
            }

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ApocalypseDriveDamage, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_garothi_worldbreaker_apocalypse_drive_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_garothi_worldbreaker_apocalypse_drive_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garothi_worldbreaker_apocalypse_drive_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Eradication - 244969
class spell_garothi_worldbreaker_eradication : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_eradication() : SpellScriptLoader("spell_garothi_worldbreaker_eradication") { }

        class spell_garothi_worldbreaker_eradication_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_eradication_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                uint32 l_Damage = GetHitDamage();
                float l_Distance = l_Caster->GetExactDist2d(l_Target);

                l_Damage = CalculatePct(l_Damage, std::max(std::min((-8.0f / 7.0f) * l_Distance + 123.0f, 100.0f), 0.0f)); ///< std::clamp is C++17

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_garothi_worldbreaker_eradication_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_eradication_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Searing Barrage - 244395
/// Searing Barrage - 246368
class spell_garothi_worldbreaker_searing_barrage : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_searing_barrage() : SpellScriptLoader("spell_garothi_worldbreaker_searing_barrage") { }

        class spell_garothi_worldbreaker_searing_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_garothi_worldbreaker_searing_barrage_AuraScript);

            enum eSpells
            {
                SearingBarrageSearcher = 246360
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SearingBarrageSearcher, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_garothi_worldbreaker_searing_barrage_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garothi_worldbreaker_searing_barrage_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Searing Barrage Searcher - 246360
class spell_garothi_worldbreaker_searing_barrage_searcher : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_searing_barrage_searcher() : SpellScriptLoader("spell_garothi_worldbreaker_searing_barrage_searcher") { }

        class spell_garothi_worldbreaker_searing_barrage_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_searing_barrage_searcher_SpellScript);

            enum eSpells
            {
                SearingBarrageDamage = 244400
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                JadeCore::RandomResizeList(p_Targets, std::min<uint32>(std::max<uint32>(l_Caster->GetMap()->GetPlayersCountExceptGMs() / 5, 2), p_Targets.size()));
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SearingBarrageDamage, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_searing_barrage_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_garothi_worldbreaker_searing_barrage_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_searing_barrage_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Surging Fel - 10876 (246655)
class at_garothi_worldbreaker_surging_fel : public AreaTriggerEntityScript
{
    public:
        at_garothi_worldbreaker_surging_fel() : AreaTriggerEntityScript("at_garothi_worldbreaker_surging_fel") { }

        enum eSpells
        {
            SurgingFelDamages = 246663
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Position l_Dest;
            l_Caster->SimplePosXYRelocationByAngle(l_Dest, 90.0f, 0.0f);
            l_Caster->CastSpell(l_Dest, eSpells::SurgingFelDamages, true);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_garothi_worldbreaker_surging_fel();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Surging Fel - 246663
class spell_garothi_worldbreaker_surging_fel : public SpellScriptLoader
{
    public:
        spell_garothi_worldbreaker_surging_fel() : SpellScriptLoader("spell_garothi_worldbreaker_surging_fel") { }

        class spell_garothi_worldbreaker_surging_fel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garothi_worldbreaker_surging_fel_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                WorldLocation l_Dest = *l_Caster;
                l_Caster->SimplePosXYRelocationByAngle(l_Dest, 90.0f, 0.0f);

                p_Targets.remove_if([l_Caster, l_Dest](WorldObject* p_WorldObject) -> bool
                {
                    return !p_WorldObject->IsInAxe(l_Caster, &l_Dest, 15.0f);
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garothi_worldbreaker_surging_fel_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_garothi_worldbreaker_surging_fel_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Blightscale Worm - 124436
class npc_garothi_worldbreaker_blightscale_worm : public CreatureScript
{
    public:
        npc_garothi_worldbreaker_blightscale_worm() : CreatureScript("npc_garothi_worldbreaker_blightscale_worm") { }

        struct npc_garothi_worldbreaker_blightscale_wormAI : public ScriptedAI
        {
            npc_garothi_worldbreaker_blightscale_wormAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_GarohtiWordlbreaker = me->FindNearestCreature(eCreatures::BossGarothiWorldbreaker, 150.0f);
                if (!l_GarohtiWordlbreaker || !l_GarohtiWordlbreaker->IsAIEnabled)
                    return;

                l_GarohtiWordlbreaker->AI()->DoAction(ACTION_4);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_garothi_worldbreaker_blightscale_wormAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_garothi_worldbreaker()
{
    new boss_garothi_worldbreaker();

    new spell_garothi_worldbreaker_eradication_intro();
    new spell_garothi_worldbreaker_fel_bombardment();
    new spell_garothi_worldbreaker_fel_bombardment_periodic();
    new spell_garothi_worldbreaker_decimation_searcher();
    new spell_garothi_worldbreaker_decimation_aura();
    new spell_garothi_worldbreaker_haywire_decimation_searcher();
    new spell_garothi_worldbreaker_haywire_decimation_aura();
    new npc_garothi_worldbreaker_annihilation();
    new spell_garothi_worldbreaker_annihilation();
    new npc_garothi_worldbreaker_cannons();
    new npc_garothi_worldbreaker_annihilation_haywire();
    new spell_garothi_worldbreaker_apocalypse_drive();
    new spell_garothi_worldbreaker_eradication();
    new spell_garothi_worldbreaker_searing_barrage();
    new spell_garothi_worldbreaker_searing_barrage_searcher();
    new at_garothi_worldbreaker_surging_fel();
    new spell_garothi_worldbreaker_surging_fel();
    new npc_garothi_worldbreaker_blightscale_worm();
}
#endif
