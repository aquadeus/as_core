////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "tomb_of_sargeras.h"

/// Maiden of Vigilance - 118289
class boss_maiden_of_vigilance : public CreatureScript
{
    public:
        boss_maiden_of_vigilance() : CreatureScript("boss_maiden_of_vigilance") { }

        enum eSpells
        {
            /// Infusion
            SpellInfusionCast                       = 235271,
            SpellFelInfusionAura                    = 235240,
            SpellLightInfusionAura                  = 235213,
            SpellLightInfusionAT                    = 241315,
            SpellFelInfusionAT                      = 241316,
            SpellUnstableSoulAura                   = 235117,
            SpellUnstableSoulDummy                  = 243275,
            SpellUnstableSoulVisualAura             = 249912,
            SpellFragmentBurstDoT                   = 248801,
            SpellRavagedSoul                        = 241729,
            /// Aegwynn's Ward
            SpellAegwynnsWardAura                   = 236420,
            SpellAegwynnsWardHeroicAura             = 241593,
            SpellAegwynnsWardSafe                   = 241565,
            /// Hammer of Creation
            SpellHammerOfCreation                   = 241635,
            SpellHammerOfCreationDamage             = 241624,
            SpellHammerOfCreationAoE                = 235569,
            SpellLightRemanence                     = 237994,
            /// Hammer of Obliteration
            SpellHammerOfObliteration               = 241636,
            SpellHammerOfObliterationDamage         = 241634,
            SpellHammerOfObliterationAoE            = 235573,
            SpellFelRemanence                       = 238316,
            /// Mass Instability
            SpellMassInstabilitySearcher            = 235267,
            SpellMassInstabilityLight               = 243266,
            SpellMassInstabilityFel                 = 243243,
            /// Phase change
            SpellTranslocateTeleport                = 235630,
            SpellBlowback                           = 248812,
            SpellTitanicBulwarkTrigger              = 236069,
            SpellTitanicBulwarkAura                 = 235028,
            /// Wrath of the Creators
            SpellWrathOfTheCreatorsChannel          = 234891,
            SpellWrathOfTheCreatorsDummy            = 235014,
            SpellWrathOfTheCreatorsLight            = 234917,
            SpellWrathOfTheCreatorsFel              = 236433,
            SpellWrathOfTheCreatorsStack            = 237339,
            SpellExtinguishLifeAoE                  = 238472,
            SpellEssenceFragmentsAura               = 236061,
            SpellFragmentOfCreationMissile          = 236690,
            SpellFragmentOfObliterationMissile      = 236708,
            /// Spontaneous Fragmentation
            SpellSpontaneousFragmentationSearch     = 239153,
            SpellSpontaneousFragmentationLight      = 241697,
            SpellSpontaneousFragmentationFel        = 241728,
            SpellSpontaneousFragmentationLightDummy = 239158,
            SpellSpontaneousFragmentationFelDummy   = 239159
        };

        enum eEvents
        {
            EventInfusion = 1,
            EventHammerOfCreation,
            EventMassInstability,
            EventHammerOfObliteration,
            EventBlowback,
            EventWrathOfTheCreators,
            EventSpontaneousFragmentation
        };

        enum eTalks
        {
            TalkAggro,
            TalkSlay,
            TalkWipe,
            TalkDeath,
            TalkInfusion,
            TalkInfusionWarn,
            TalkBlowback,
            TalkWrathOfTheCreators,
            Talk10Percent
        };

        enum eVisual
        {
            UnstableSoulVisual = 86449
        };

        struct boss_maiden_of_vigilanceAI : public BossAI
        {
            boss_maiden_of_vigilanceAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataMaidenOfVigilance) { }

            uint32 m_BerserkTimer = 0;

            uint32 m_InfusionCounter = 0;
            uint32 m_CreationHammerCount = 0;
            uint32 m_ObliterationHammerCount = 0;
            uint32 m_InstabilityCount = 0;
            uint32 m_OrbCount = 0;

            bool m_LightWave = true;
            bool m_InitialOrb = true;
            bool m_10PercentTalk = false;
            bool m_Achievement = false;

            int8 m_FirstTank = -1;

            ///       Light  Fel
            std::pair<uint8, uint8> m_InfusedCount;

            std::set<uint64> m_FallingPlayers;

            uint64 m_OrbTargetGuid = 0;

            uint32 m_AnomalyCount = 0;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool BypassXPDistanceIfNeeded(WorldObject* /*p_Object*/) override
            {
                return true;
            }

            void Reset() override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->SetCanFly(false);
                me->SetDisableGravity(false);

                m_BerserkTimer = 0;

                m_InfusionCounter = 0;
                m_CreationHammerCount = 0;
                m_ObliterationHammerCount = 0;
                m_InstabilityCount = 0;
                m_OrbCount = 0;

                m_LightWave = true;
                m_InitialOrb = true;
                m_10PercentTalk = false;
                m_Achievement = false;

                m_FirstTank = -1;

                m_InfusedCount = { 0, 0 };

                m_FallingPlayers.clear();

                m_OrbTargetGuid = 0;

                m_AnomalyCount = 0;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                Talk(eTalks::TalkAggro);

                events.Reset();

                if (!IsLFR())
                {
                    m_BerserkTimer = 480 * TimeConstants::IN_MILLISECONDS;

                    events.ScheduleEvent(eEvents::EventInfusion, 2 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventHammerOfCreation, 12 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventMassInstability, 22 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventHammerOfObliteration, 32 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventBlowback, 42 * TimeConstants::IN_MILLISECONDS + 500);

                    /// First orb for achievement
                    me->SummonCreature(eCreatures::NpcTitanicAnomaly, { 6348.2f, -795.9496f, 2855.169f, 0.0f });
                }
                else
                {
                    events.ScheduleEvent(eEvents::EventMassInstability, 6 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventInfusion, 41 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventBlowback, 46 * TimeConstants::IN_MILLISECONDS);
                }

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventSpontaneousFragmentation, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                Talk(eTalks::TalkWipe);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellUnstableSoulAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegwynnsWardAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegwynnsWardHeroicAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegwynnsWardSafe);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellRavagedSoul);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFragmentBurstDoT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellUnstableSoulVisualAura);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                me->RemoveAllAreasTrigger();

                Talk(eTalks::TalkDeath);

                if (instance)
                {
                    Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->isDead() && l_Player->m_positionZ < 2886.0f)
                                l_Player->NearTeleportTo(me->GetHomePosition());
                        }
                    }

                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellUnstableSoulAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegwynnsWardAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegwynnsWardHeroicAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegwynnsWardSafe);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellRavagedSoul);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFragmentBurstDoT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellUnstableSoulVisualAura);

                    if (!IsLFR() && m_Achievement)
                        instance->DoCompleteAchievement(eAchievements::SkyWalker);
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(eTalks::TalkSlay);

                p_Killed->RemoveAura(eSpells::SpellFelInfusionAT);
                p_Killed->RemoveAura(eSpells::SpellLightInfusionAT);
                p_Killed->RemoveAura(eSpells::SpellFelInfusionAura);
                p_Killed->RemoveAura(eSpells::SpellLightInfusionAura);
                p_Killed->RemoveAura(eSpells::SpellUnstableSoulVisualAura);

                p_Killed->CancelSpellVisualKit(eVisual::UnstableSoulVisual);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_10PercentTalk && me->HealthBelowPctDamaged(10, p_Damage))
                {
                    m_10PercentTalk = true;

                    Talk(eTalks::Talk10Percent);
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellSpontaneousFragmentationSearch:
                    {
                        p_Targets.clear();

                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                            p_Targets.push_back(l_Target);

                        break;
                    }
                    case eSpells::SpellBlowback:
                    {
                        if (p_Targets.empty() || p_EffIndex != SpellEffIndex::EFFECT_0)
                            break;

                        /// Don't stun players who are falling in/from the pit because of Unstable Soul
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->IsFalling())
                                return true;

                            return false;
                        });

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
                    case eSpells::SpellInfusionCast:
                    {
                        bool l_Light = true;

                        /// Which debuff players gain seems to be entirely random, apart from the tanks which will always receive opposite Infusions from one another.
                        if (p_Target->IsPlayer() && p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        {
                            if (m_FirstTank == -1)
                            {
                                m_FirstTank = urand(0, 1);

                                if (m_FirstTank)
                                    ++m_InfusedCount.first;
                                else
                                {
                                    l_Light = false;
                                    ++m_InfusedCount.second;
                                }
                            }
                            else
                            {
                                if (m_FirstTank)
                                {
                                    l_Light = false;
                                    ++m_InfusedCount.second;
                                }
                                else
                                    ++m_InfusedCount.first;
                            }
                        }
                        else
                        {
                            /// First Infusion
                            if (!m_InfusedCount.first && !m_InfusedCount.second)
                            {
                                if (urand(0, 1))
                                    ++m_InfusedCount.first;
                                else
                                {
                                    l_Light = false;
                                    ++m_InfusedCount.second;
                                }
                            }
                            else if (m_InfusedCount.first < m_InfusedCount.second)
                                ++m_InfusedCount.first;
                            else
                            {
                                l_Light = false;
                                ++m_InfusedCount.second;
                            }
                        }

                        if (l_Light)
                            DoCast(p_Target, eSpells::SpellLightInfusionAura, true);
                        else
                            DoCast(p_Target, eSpells::SpellFelInfusionAura, true);

                        break;
                    }
                    case eSpells::SpellHammerOfCreation:
                    {
                        DoCast(p_Target, eSpells::SpellHammerOfCreationDamage, true);
                        DoCast(p_Target, eSpells::SpellHammerOfCreationAoE, true);

                        if (IsHeroicOrMythic())
                            DoCast(me, eSpells::SpellLightRemanence, true);

                        break;
                    }
                    case eSpells::SpellHammerOfObliteration:
                    {
                        DoCast(p_Target, eSpells::SpellHammerOfObliterationDamage, true);
                        DoCast(p_Target, eSpells::SpellHammerOfObliterationAoE, true);

                        if (IsHeroicOrMythic())
                            DoCast(me, eSpells::SpellFelRemanence, true);

                        break;
                    }
                    case eSpells::SpellMassInstabilitySearcher:
                    {
                        if (p_Target->HasAura(eSpells::SpellLightInfusionAura))
                            DoCast(p_Target, eSpells::SpellMassInstabilityLight, true);
                        else
                            DoCast(p_Target, eSpells::SpellMassInstabilityFel, true);

                        break;
                    }
                    case eSpells::SpellUnstableSoulDummy:
                    {
                        p_Target->RemoveAura(eSpells::SpellFelInfusionAT);
                        p_Target->RemoveAura(eSpells::SpellLightInfusionAT);

                        p_Target->CastSpell(p_Target, eSpells::SpellUnstableSoulAura, true);
                        break;
                    }
                    case eSpells::SpellFragmentBurstDoT:
                    {
                        DoCast(p_Target, eSpells::SpellRavagedSoul, true);
                        break;
                    }
                    case eSpells::SpellSpontaneousFragmentationSearch:
                    {
                        m_OrbTargetGuid = p_Target->GetGUID();

                        if (p_Target->HasAura(eSpells::SpellLightInfusionAura))
                            DoCast(p_Target, eSpells::SpellSpontaneousFragmentationFel, true);
                        else
                            DoCast(p_Target, eSpells::SpellSpontaneousFragmentationLight, true);

                        break;
                    }
                    ///////////////////////////////////////////////////////////////////////
                    /// Holy damages that triggers Unstable Soul on Fel Infused targets
                    ///////////////////////////////////////////////////////////////////////
                    case eSpells::SpellHammerOfCreationDamage:
                    case eSpells::SpellHammerOfCreationAoE:
                    {
                        if (p_Target->HasAura(eSpells::SpellFelInfusionAT))
                        {
                            p_Target->RemoveAura(eSpells::SpellFelInfusionAT);

                            p_Target->CastSpell(p_Target, eSpells::SpellUnstableSoulAura, true);
                        }

                        break;
                    }
                    ///////////////////////////////////////////////////////////////////////
                    /// Fire damages that triggers Unstable Soul on Fel Infused targets
                    ///////////////////////////////////////////////////////////////////////
                    case eSpells::SpellHammerOfObliterationDamage:
                    case eSpells::SpellHammerOfObliterationAoE:
                    {
                        if (p_Target->HasAura(eSpells::SpellLightInfusionAT))
                        {
                            p_Target->RemoveAura(eSpells::SpellLightInfusionAT);

                            p_Target->CastSpell(p_Target, eSpells::SpellUnstableSoulAura, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode != SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFragmentOfCreationMissile:
                    {
                        const_cast<SpellDestination*>(p_Dest)->_position.m_orientation = p_Dest->_position.GetAngle(&g_MaidenCenterPos);

                        me->SummonCreature(eCreatures::NpcFragmentOfCreation, p_Dest->_position);
                        break;
                    }
                    case eSpells::SpellFragmentOfObliterationMissile:
                    {
                        const_cast<SpellDestination*>(p_Dest)->_position.m_orientation = p_Dest->_position.GetAngle(&g_MaidenCenterPos);

                        me->SummonCreature(eCreatures::NpcFragmentOfObliteration, p_Dest->_position);
                        break;
                    }
                    case eSpells::SpellSpontaneousFragmentationLightDummy:
                    {
                        if (Creature* l_Orb = me->SummonCreature(eCreatures::NpcFragmentOfCreation, p_Dest->_position))
                        {
                            if (l_Orb->IsAIEnabled)
                                l_Orb->AI()->SetGUID(m_OrbTargetGuid);
                        }

                        m_OrbTargetGuid = 0;
                        break;
                    }
                    case eSpells::SpellSpontaneousFragmentationFelDummy:
                    {
                        if (Creature* l_Orb = me->SummonCreature(eCreatures::NpcFragmentOfObliteration, p_Dest->_position))
                        {
                            if (l_Orb->IsAIEnabled)
                                l_Orb->AI()->SetGUID(m_OrbTargetGuid);
                        }

                        m_OrbTargetGuid = 0;
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
                    case eSpells::SpellInfusionCast:
                    {
                        Talk(eTalks::TalkInfusion);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_FirstTank = -1;

                            m_InfusedCount = { 0, 0 };
                        });

                        break;
                    }
                    case eSpells::SpellTranslocateTeleport:
                    {
                        AddTimedDelayedOperation(200, [this]() -> void
                        {
                            me->SetFacingTo(me->GetAngle(&g_MaidenCenterPos));

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                                events.ScheduleEvent(eEvents::EventWrathOfTheCreators, 1);
                            });
                        });

                        break;
                    }
                    case eSpells::SpellTitanicBulwarkAura:
                    {
                        AddTimedDelayedOperation(500, [this]() -> void
                        {
                            Talk(eTalks::TalkWrathOfTheCreators);

                            DoCast(me, eSpells::SpellWrathOfTheCreatorsChannel, true);
                        });

                        break;
                    }
                    case eSpells::SpellWrathOfTheCreatorsChannel:
                    {
                        DoCast(me, eSpells::SpellEssenceFragmentsAura, true);
                        break;
                    }
                    case eSpells::SpellWrathOfTheCreatorsDummy:
                    {
                        if (m_LightWave)
                            DoCast(me, eSpells::SpellWrathOfTheCreatorsLight, true);
                        else
                            DoCast(me, eSpells::SpellWrathOfTheCreatorsFel, true);

                        if (Aura* l_Stacks = me->GetAura(eSpells::SpellWrathOfTheCreatorsStack))
                        {
                            if (l_Stacks->GetStackAmount() >= l_Stacks->GetSpellInfo()->StackAmount)
                                DoCast(me, eSpells::SpellExtinguishLifeAoE, true);
                        }

                        m_LightWave = !m_LightWave;
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* /*p_SpellInfo*/, SpellInfo const* p_CurrSpellInfo, uint32 /*p_SchoolMask*/) override
            {
                if (p_CurrSpellInfo->Id == eSpells::SpellWrathOfTheCreatorsChannel)
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                    me->RemoveAura(eSpells::SpellWrathOfTheCreatorsStack);

                    if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                    {
                        AttackStart(l_Target);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(l_Target);
                    }

                    m_InfusionCounter = 1;
                    m_CreationHammerCount = 1;
                    m_ObliterationHammerCount = 1;
                    m_InstabilityCount = 1;
                    m_OrbCount = 1;

                    m_LightWave = true;
                    m_InitialOrb = false;

                    events.Reset();

                    if (instance)
                    {
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAT);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAT);
                    }

                    if (!IsLFR())
                    {
                        events.ScheduleEvent(eEvents::EventInfusion, 2 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventHammerOfCreation, 14 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventMassInstability, 22 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventHammerOfObliteration, 32 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventBlowback, 82 * TimeConstants::IN_MILLISECONDS + 500);
                    }
                    else
                    {
                        events.ScheduleEvent(eEvents::EventMassInstability,8 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventBlowback, 66 * TimeConstants::IN_MILLISECONDS);
                    }

                    if (IsMythic())
                        events.ScheduleEvent(eEvents::EventSpontaneousFragmentation, 8 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                /// Titanic Anomaly soaked
                ++m_AnomalyCount;

                switch (m_AnomalyCount)
                {
                    case 1:
                    {
                        Position l_Pos  = g_MaidenCenterPos;
                        float l_Angle   = 0.0f;

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            g_MaidenCenterPos.SimplePosXYRelocationByAngle(l_Pos, 18.0f, l_Angle, true);

                            l_Pos.m_positionZ = 2855.169f;

                            if (Creature* l_Anomaly = me->SummonCreature(eCreatures::NpcTitanicAnomaly, l_Pos))
                            {
                                if (l_Anomaly->IsAIEnabled)
                                    l_Anomaly->AI()->DoAction(0);
                            }

                            l_Angle += M_PI / 2.0f;
                        }

                        break;
                    }
                    case 5:
                    {
                        Position l_Pos = g_MaidenCenterPos;
                        float l_Angle = 0.0f;

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            g_MaidenCenterPos.SimplePosXYRelocationByAngle(l_Pos, 44.0f, l_Angle, true);

                            l_Pos.m_positionZ = 2925.0f;

                            if (Creature* l_Anomaly = me->SummonCreature(eCreatures::NpcTitanicAnomaly, l_Pos))
                            {
                                if (l_Anomaly->IsAIEnabled)
                                    l_Anomaly->AI()->DoAction(1);
                            }

                            l_Angle += M_PI / 2.0f;
                        }

                        break;
                    }
                    case 9:
                    {
                        m_Achievement = IsLFR() ? false : true;
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

                Position l_Home = me->GetHomePosition();
                if (me->GetExactDist2d(&l_Home) >= 90.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                CheckAegwynnWard();

                if (m_BerserkTimer)
                {
                    if (m_BerserkTimer <= p_Diff)
                    {
                        DoCast(me, eSharedSpells::SpellBerserkOther, true);

                        m_BerserkTimer = 0;
                    }
                    else
                        m_BerserkTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventInfusion:
                    {
                        Talk(eTalks::TalkInfusionWarn);

                        ++m_InfusionCounter;

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAura);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAura);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelInfusionAT);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellLightInfusionAT);
                        }

                        DoCast(me, eSpells::SpellInfusionCast);

                        if (m_InfusionCounter == 2)
                            events.ScheduleEvent(eEvents::EventInfusion, 38 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventHammerOfCreation:
                    {
                        ++m_CreationHammerCount;

                        DoCastVictim(eSpells::SpellHammerOfCreation);

                        if (m_CreationHammerCount == 2)
                            events.ScheduleEvent(eEvents::EventHammerOfCreation, 36 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventMassInstability:
                    {
                        ++m_InstabilityCount;

                        DoCast(me, eSpells::SpellMassInstabilitySearcher);

                        if (IsLFR() && m_InstabilityCount < 5)
                            events.ScheduleEvent(eEvents::EventMassInstability, 12 * TimeConstants::IN_MILLISECONDS);
                        else if (!IsLFR() && m_InstabilityCount == 2)
                            events.ScheduleEvent(eEvents::EventMassInstability, 36 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventHammerOfObliteration:
                    {
                        ++m_ObliterationHammerCount;

                        DoCastVictim(eSpells::SpellHammerOfObliteration);

                        if (m_ObliterationHammerCount == 2)
                            events.ScheduleEvent(eEvents::EventHammerOfObliteration, 36 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventBlowback:
                    {
                        Talk(eTalks::TalkBlowback);

                        me->StopAttack();

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        events.Reset();

                        Position l_Pos = me->GetPosition();

                        g_MaidenCenterPos.SimplePosXYRelocationByAngle(l_Pos, 35.0f, l_Pos.GetAngle(&g_MaidenCenterPos), true);

                        l_Pos.SetOrientation(l_Pos.GetAngle(&g_MaidenCenterPos));

                        DoCast(l_Pos, eSpells::SpellTranslocateTeleport, true);
                        break;
                    }
                    case eEvents::EventWrathOfTheCreators:
                    {
                        DoCast(me, eSpells::SpellBlowback, true);
                        DoCast(me, eSpells::SpellTitanicBulwarkTrigger);
                        break;
                    }
                    case eEvents::EventSpontaneousFragmentation:
                    {
                        ++m_OrbCount;

                        DoCast(me, eSpells::SpellSpontaneousFragmentationSearch, true);

                        if (m_OrbCount <= 3 && m_InitialOrb)
                            events.ScheduleEvent(eEvents::EventSpontaneousFragmentation, 8 * TimeConstants::IN_MILLISECONDS);
                        else if (!m_InitialOrb)
                        {
                            std::vector<uint32> l_OrbTimers = { 8000, 8500, 7500, 10500, 11500, 8000, 8000, 10000 };

                            if (m_OrbCount <= (l_OrbTimers.size() - 1))
                                events.ScheduleEvent(eEvents::EventSpontaneousFragmentation, l_OrbTimers[m_OrbCount]);
                        }

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void CheckAegwynnWard()
            {
                std::set<uint64> l_Targets;

                std::list<HostileReference*>& l_ThreatList = me->getThreatManager().getThreatList();
                for (std::list<HostileReference*>::const_iterator l_Iter = l_ThreatList.begin(); l_Iter != l_ThreatList.end(); ++l_Iter)
                {
                    if (Unit* l_Unit = (*l_Iter)->getTarget())
                    {
                        if (l_Unit->IsFalling() && l_Unit->m_positionZ < 2886.0f)
                        {
                            l_Targets.insert(l_Unit->GetGUID());

                            if (!l_Unit->HasAura(eSpells::SpellAegwynnsWardAura) && !l_Unit->HasAura(eSpells::SpellAegwynnsWardHeroicAura))
                            {
                                m_FallingPlayers.insert(l_Unit->GetGUID());

                                /// Reset threat against falling players
                                DoModifyThreatPercent(l_Unit, -100);

                                /// Heroic difficulty: Players will not fall slowly whilst within the well, and instead will fall at regular speed.
                                if (IsHeroicOrMythic())
                                    l_Unit->CastSpell(l_Unit, eSpells::SpellAegwynnsWardHeroicAura, true);
                                else
                                {
                                    l_Unit->CastSpell(l_Unit, eSpells::SpellAegwynnsWardAura, true);
                                    l_Unit->CastSpell(l_Unit, eSpells::SpellAegwynnsWardSafe, true);
                                }
                            }
                        }
                    }
                }

                for (std::set<uint64>::iterator l_Iter = m_FallingPlayers.begin(); l_Iter != m_FallingPlayers.end();)
                {
                    if (l_Targets.find((*l_Iter)) != l_Targets.end())
                    {
                        ++l_Iter;
                        continue;
                    }

                    if (Unit* l_Unit = Unit::GetUnit(*me, (*l_Iter)))
                    {
                        l_Iter = m_FallingPlayers.erase(l_Iter);

                        l_Unit->RemoveAura(eSpells::SpellAegwynnsWardAura);
                        l_Unit->RemoveAura(eSpells::SpellAegwynnsWardSafe);
                        continue;
                    }

                    ++l_Iter;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_maiden_of_vigilanceAI(p_Creature);
        }
};

/// Fragment of Creation - 118640
class npc_maiden_fragment_of_creation : public CreatureScript
{
    public:
        npc_maiden_fragment_of_creation() : CreatureScript("npc_maiden_fragment_of_creation") { }

        enum eSpells
        {
            SpellEssenceFragmentsAura           = 250500,
            SpellFragmentOfCreationAura         = 235477,

            SpellSpontaneousFragmentationFixate = 241713,

            SpellFragmentBurstAoE               = 239069,
            SpellFragmentBurstDoT               = 248801
        };

        struct npc_maiden_fragment_of_creationAI : public ScriptedAI
        {
            npc_maiden_fragment_of_creationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Clockwise = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                SetCombatMovement(false);

                DoCast(me, eSpells::SpellEssenceFragmentsAura, true);
                DoCast(me, eSpells::SpellFragmentOfCreationAura, true);

                Position l_Src = p_Summoner->GetPosition();

                m_Clockwise = Position::NormalizeOrientation(me->GetRelativeAngle(&l_Src)) > M_PI;

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->StopMoving();

                    me->GetMotionMaster()->Clear();

                    std::vector<G3D::Vector3> l_Path;

                    FillCirclePath(g_MaidenCenterPos, me->GetExactDist(&g_MaidenCenterPos), me->m_positionZ, l_Path, m_Clockwise);

                    me->GetMotionMaster()->MoveSmoothPath(0, l_Path.data(), l_Path.size(), false);
                });

                me->DespawnOrUnsummon(14 * TimeConstants::IN_MILLISECONDS);
            }

            void FillCirclePath(Position const& p_Center, float p_Radius, float p_Z, std::vector<G3D::Vector3>& p_Path, bool p_Clockwise)
            {
                float l_Step = p_Clockwise ? -M_PI / 8.0f : M_PI / 8.0f;
                float l_Angle = p_Center.GetAngle(me->GetPositionX(), me->GetPositionY());

                for (uint8 l_Iter = 0; l_Iter < 16; l_Angle += l_Step, ++l_Iter)
                {
                    G3D::Vector3 l_Point;
                    l_Point.x = p_Center.GetPositionX() + p_Radius * cosf(l_Angle);
                    l_Point.y = p_Center.GetPositionY() + p_Radius * sinf(l_Angle);
                    l_Point.z = p_Z;
                    p_Path.push_back(l_Point);
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                float l_Distance = me->GetDistance(g_MaidenCenterPos);

                if (l_Distance < 30.0f)
                    p_Velocity = 3.146f;
                else if (l_Distance < 40.0f)
                    p_Velocity = 4.19f;
                else
                    p_Velocity = 5.2f;

                p_Velocity *= 2.0f;
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellSpontaneousFragmentationFixate)
                {
                    if (Unit* l_Boss = me->GetAnyOwner())
                    {
                        l_Boss->CastSpell(l_Boss, eSpells::SpellFragmentBurstAoE, true);
                        l_Boss->CastSpell(l_Boss, eSpells::SpellFragmentBurstDoT, true);
                    }

                    me->DespawnOrUnsummon(1);
                }
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                ClearDelayedOperations();

                if (Player* l_Player = Player::GetPlayer(*me, p_Guid))
                    DoCast(l_Player, eSpells::SpellSpontaneousFragmentationFixate, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_maiden_fragment_of_creationAI(p_Creature);
        }
};

/// Fragment of Obliteration - 118643
class npc_maiden_fragment_of_obliteration : public CreatureScript
{
    public:
        npc_maiden_fragment_of_obliteration() : CreatureScript("npc_maiden_fragment_of_obliteration") { }

        enum eSpells
        {
            SpellEssenceFragmentsAura           = 250500,
            SpellFragmentOfObliterationAura     = 235479,

            SpellSpontaneousFragmentationFixate = 241727,

            SpellFragmentBurstAoE               = 239069,
            SpellFragmentBurstDoT               = 248801
        };

        struct npc_maiden_fragment_of_obliterationAI : public ScriptedAI
        {
            npc_maiden_fragment_of_obliterationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Clockwise = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                SetCombatMovement(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellEssenceFragmentsAura, true);
                DoCast(me, eSpells::SpellFragmentOfObliterationAura, true);

                Position l_Src = p_Summoner->GetPosition();

                m_Clockwise = Position::NormalizeOrientation(me->GetRelativeAngle(&l_Src)) > M_PI;

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->StopMoving();

                    me->GetMotionMaster()->Clear();

                    std::vector<G3D::Vector3> l_Path;

                    FillCirclePath(g_MaidenCenterPos, me->GetExactDist2d(&g_MaidenCenterPos), me->m_positionZ, l_Path, m_Clockwise);

                    me->GetMotionMaster()->MoveSmoothPath(0, l_Path.data(), l_Path.size(), false);
                });

                me->DespawnOrUnsummon(14 * TimeConstants::IN_MILLISECONDS);
            }

            void FillCirclePath(Position const& p_Center, float p_Radius, float p_Z, std::vector<G3D::Vector3>& p_Path, bool p_Clockwise)
            {
                float l_Step = p_Clockwise ? -M_PI / 8.0f : M_PI / 8.0f;
                float l_Angle = p_Center.GetAngle(me->GetPositionX(), me->GetPositionY());

                for (uint8 l_Iter = 0; l_Iter < 16; l_Angle += l_Step, ++l_Iter)
                {
                    G3D::Vector3 l_Point;
                    l_Point.x = p_Center.GetPositionX() + p_Radius * cosf(l_Angle);
                    l_Point.y = p_Center.GetPositionY() + p_Radius * sinf(l_Angle);
                    l_Point.z = p_Z;
                    p_Path.push_back(l_Point);
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                float l_Distance = me->GetDistance(g_MaidenCenterPos);

                if (l_Distance < 30.0f)
                    p_Velocity = 3.146f;
                else if (l_Distance < 40.0f)
                    p_Velocity = 4.19f;
                else
                    p_Velocity = 5.2f;

                p_Velocity *= 2.0f;
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellSpontaneousFragmentationFixate)
                {
                    if (Unit* l_Boss = me->GetAnyOwner())
                    {
                        l_Boss->CastSpell(l_Boss, eSpells::SpellFragmentBurstAoE, true);
                        l_Boss->CastSpell(l_Boss, eSpells::SpellFragmentBurstDoT, true);
                    }

                    me->DespawnOrUnsummon(1);
                }
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                ClearDelayedOperations();

                if (Player* l_Player = Player::GetPlayer(*me, p_Guid))
                    DoCast(l_Player, eSpells::SpellSpontaneousFragmentationFixate, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_maiden_fragment_of_obliterationAI(p_Creature);
        }
};

/// Light Remanence Stalker - 119825
class npc_maiden_light_remanence_stalker : public CreatureScript
{
    public:
        npc_maiden_light_remanence_stalker() : CreatureScript("npc_maiden_light_remanence_stalker") { }

        enum eSpells
        {
            SpellLightEchoes            = 238311,
            SpellLightEchoesSearcher    = 238041,
            SpellLightEchoesMissile     = 238035,
            SpellLightEchoesDamages     = 238037,

            SpellFelInfusionAT          = 241316,
            SpellUnstableSoulAura       = 235117
        };

        struct npc_maiden_light_remanence_stalkerAI : public Scripted_NoMovementAI
        {
            npc_maiden_light_remanence_stalkerAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellLightEchoes, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::SpellLightEchoesSearcher && !p_Targets.empty())
                {
                    /// Don't targets players who are falling in/from the pit because of Unstable Soul
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->IsFalling())
                            return true;

                        return false;
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellLightEchoesSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellLightEchoesMissile, true);
                        break;
                    }
                    case eSpells::SpellLightEchoesDamages:
                    {
                        if (p_Target->HasAura(eSpells::SpellFelInfusionAT))
                        {
                            p_Target->RemoveAura(eSpells::SpellFelInfusionAT);

                            p_Target->CastSpell(p_Target, eSpells::SpellUnstableSoulAura, true);
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
            return new npc_maiden_light_remanence_stalkerAI(p_Creature);
        }
};

/// Fel Remanence Stalker - 119826
class npc_maiden_fel_remanence_stalker : public CreatureScript
{
    public:
        npc_maiden_fel_remanence_stalker() : CreatureScript("npc_maiden_fel_remanence_stalker") { }

        enum eSpells
        {
            SpellFelEchoes              = 238416,
            SpellFelEchoesSearcher      = 238417,
            SpellFelEchoesMissile       = 238419,
            SpellFelEchoesDamages       = 238420,

            SpellLightInfusionAT        = 241315,
            SpellUnstableSoulAura       = 235117
        };

        struct npc_maiden_fel_remanence_stalkerAI : public Scripted_NoMovementAI
        {
            npc_maiden_fel_remanence_stalkerAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellFelEchoes, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::SpellFelEchoesSearcher && !p_Targets.empty())
                {
                    /// Don't targets players who are falling in/from the pit because of Unstable Soul
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object || !p_Object->IsPlayer() || !p_Object->ToPlayer()->IsFalling())
                            return true;

                        return false;
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFelEchoesSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellFelEchoesMissile, true);
                        break;
                    }
                    case eSpells::SpellFelEchoesDamages:
                    {
                        if (p_Target->HasAura(eSpells::SpellLightInfusionAT))
                        {
                            p_Target->RemoveAura(eSpells::SpellLightInfusionAT);

                            p_Target->CastSpell(p_Target, eSpells::SpellUnstableSoulAura, true);
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
            return new npc_maiden_fel_remanence_stalkerAI(p_Creature);
        }
};

/// Titanic Anomaly - 120552
class npc_maiden_titanic_anomaly : public CreatureScript
{
    public:
        npc_maiden_titanic_anomaly() : CreatureScript("npc_maiden_titanic_anomaly") { }

        enum eSpells
        {
            SpellTitanicAnomalyVisual   = 240330,
            SpellStarterAreaTrigger     = 240341
        };

        struct npc_maiden_titanic_anomalyAI : public Scripted_NoMovementAI
        {
            npc_maiden_titanic_anomalyAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            std::vector<G3D::Vector3> m_Path;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                SetCombatMovement(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellTitanicAnomalyVisual, true);
                DoCast(me, eSpells::SpellStarterAreaTrigger, true);
            }

            void DoAction(int32 const p_Action) override
            {
                bool l_InThePit = p_Action == 0;

                AddTimedDelayedOperation(1, [this, l_InThePit]() -> void
                {
                    me->StopMoving();

                    me->GetMotionMaster()->Clear();

                    FillCirclePath(g_MaidenCenterPos, l_InThePit ? 18.0f : 44.0f, me->m_positionZ, m_Path, true);

                    me->GetMotionMaster()->MoveSmoothPath(0, m_Path.data(), m_Path.size(), false);
                });
            }

            void FillCirclePath(Position const& p_Center, float p_Radius, float p_Z, std::vector<G3D::Vector3>& p_Path, bool p_Clockwise)
            {
                float l_Step = p_Clockwise ? -M_PI / 8.0f : M_PI / 8.0f;
                float l_Angle = p_Center.GetAngle(me->GetPositionX(), me->GetPositionY());

                for (uint8 l_Iter = 0; l_Iter < 16; l_Angle += l_Step, ++l_Iter)
                {
                    G3D::Vector3 l_Point;
                    l_Point.x = p_Center.GetPositionX() + p_Radius * cosf(l_Angle);
                    l_Point.y = p_Center.GetPositionY() + p_Radius * sinf(l_Angle);
                    l_Point.z = p_Z;
                    p_Path.push_back(l_Point);
                }
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();

                    me->GetMotionMaster()->MoveSmoothPath(0, m_Path.data(), m_Path.size(), false);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_maiden_titanic_anomalyAI(p_Creature);
        }
};

/// Light Infusion - 235213
class spell_maiden_light_infusion : public SpellScriptLoader
{
    public:
        spell_maiden_light_infusion() : SpellScriptLoader("spell_maiden_light_infusion") { }

        enum eSpells
        {
            SpellLightInfusionDmg   = 235214,
            SpellLightInfusionAT    = 241315
        };

        class spell_maiden_light_infusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maiden_light_infusion_AuraScript);

            bool m_First = true;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        l_Caster->CastSpell(l_Target, eSpells::SpellLightInfusionDmg, true);

                        if (m_First)
                        {
                            m_First = false;

                            l_Caster->CastSpell(l_Target, eSpells::SpellLightInfusionAT, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_maiden_light_infusion_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maiden_light_infusion_AuraScript();
        }
};

/// Fel Infusion - 235240
class spell_maiden_fel_infusion : public SpellScriptLoader
{
    public:
        spell_maiden_fel_infusion() : SpellScriptLoader("spell_maiden_fel_infusion") { }

        enum eSpells
        {
            SpellFelInfusionDmg = 235253,
            SpellFelInfusionAT  = 241316
        };

        class spell_maiden_fel_infusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maiden_fel_infusion_AuraScript);

            bool m_First = true;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        l_Caster->CastSpell(l_Target, eSpells::SpellFelInfusionDmg, true);

                        if (m_First)
                        {
                            m_First = false;

                            l_Caster->CastSpell(l_Target, eSpells::SpellFelInfusionAT, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_maiden_fel_infusion_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maiden_fel_infusion_AuraScript();
        }
};

/// Unstable Soul - 235117
class spell_maiden_unstable_soul : public SpellScriptLoader
{
    public:
        spell_maiden_unstable_soul() : SpellScriptLoader("spell_maiden_unstable_soul") { }

        enum eSpells
        {
            SpellUnstableSoulAoEDamage      = 235138,
            SpellUnstableSoulAoEKnockBack   = 235133,
            SpellUnstableSoulSelfKnockBack  = 254062,
            SpellUnstableSoulVisualAura     = 249912,

            SpellAegwynnsWardHeroicAura     = 241593,
            SpellAegwynnsWardAura           = 236420,

            SpellFelInfusionAT              = 241316,
            SpellLightInfusionAT            = 241315,
            SpellFelInfusionAura            = 235240,
            SpellLightInfusionAura          = 235213
        };

        enum eVisual
        {
            UnstableSoulVisual = 86449
        };

        class spell_maiden_unstable_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maiden_unstable_soul_SpellScript);

            SpellCastResult CheckSoul()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetExplTargetUnit())
                    {
                        if (l_Target->HasAura(GetSpellInfo()->Id))
                            return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_maiden_unstable_soul_SpellScript::CheckSoul);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_maiden_unstable_soul_SpellScript();
        }

        class spell_maiden_unstable_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maiden_unstable_soul_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->CastSpell(l_Target, eSpells::SpellUnstableSoulVisualAura, true);

                    l_Target->SendPlaySpellVisualKit(eVisual::UnstableSoulVisual, 0);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAura(eSpells::SpellUnstableSoulVisualAura);

                l_Target->CancelSpellVisualKit(eVisual::UnstableSoulVisual);

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (l_Target->HasAura(eSpells::SpellAegwynnsWardHeroicAura) || l_Target->HasAura(eSpells::SpellAegwynnsWardAura))
                {
                    l_Target->CastSpell(l_Target, eSpells::SpellUnstableSoulSelfKnockBack, true);

                    l_Target->RemoveAura(eSpells::SpellAegwynnsWardAura);
                    l_Target->RemoveAura(eSpells::SpellAegwynnsWardHeroicAura);
                }
                else
                {
                    l_Target->CastSpell(l_Target, eSpells::SpellUnstableSoulAoEDamage, true);
                    l_Target->CastSpell(l_Target, eSpells::SpellUnstableSoulAoEKnockBack, true);
                }

                if (l_Target->HasAura(eSpells::SpellFelInfusionAura))
                    l_Target->CastSpell(l_Target, eSpells::SpellFelInfusionAT, true);
                else if (l_Target->HasAura(eSpells::SpellLightInfusionAura))
                    l_Target->CastSpell(l_Target, eSpells::SpellLightInfusionAT, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_maiden_unstable_soul_AuraScript::AfterApply, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_MOD_SCALE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_maiden_unstable_soul_AuraScript::AfterRemove, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_MOD_SCALE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maiden_unstable_soul_AuraScript();
        }
};

/// Wrath of the Creators (Holy damages) - 234896
/// Wrath of the Creators (Fire damages) - 236432
class spell_maiden_wrath_of_the_creators_dmg : public SpellScriptLoader
{
    public:
        spell_maiden_wrath_of_the_creators_dmg() : SpellScriptLoader("spell_maiden_wrath_of_the_creators_dmg") { }

        enum eSpell
        {
            SpellWrathOfTheCreatorsStack = 237339
        };

        class spell_maiden_wrath_of_the_creators_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maiden_wrath_of_the_creators_dmg_SpellScript);

            void DealDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    int32 l_Damage = GetHitDamage();

                    if (AuraEffect* l_Stacks = l_Caster->GetAuraEffect(eSpell::SpellWrathOfTheCreatorsStack, SpellEffIndex::EFFECT_0))
                        AddPct(l_Damage, l_Stacks->GetAmount());

                    SetHitDamage(l_Damage);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_maiden_wrath_of_the_creators_dmg_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_maiden_wrath_of_the_creators_dmg_SpellScript();
        }
};

/// Blowback - 248812
class spell_maiden_blowback : public SpellScriptLoader
{
    public:
        spell_maiden_blowback() : SpellScriptLoader("spell_maiden_blowback") { }

        class spell_maiden_blowback_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maiden_blowback_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Unit* l_Target = GetUnitOwner();
                    if (!l_Target)
                        return;

                    float l_DistPct = std::min(100.0f, std::max(60.0f, l_Target->GetDistance(l_Caster)) / 60.0f * 100.0f);

                    p_Duration = std::max(500.0f, (p_Duration - CalculatePct(float(p_Duration), l_DistPct)));
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_maiden_blowback_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maiden_blowback_AuraScript();
        }
};

/// Essence Fragments - 236061
class spell_maiden_essence_fragments : public SpellScriptLoader
{
    public:
        spell_maiden_essence_fragments() : SpellScriptLoader("spell_maiden_essence_fragments") { }

        enum eSpells
        {
            SpellFragmentOfCreationMissile      = 236690,
            SpellFragmentOfObliterationMissile  = 236708
        };

        class spell_maiden_essence_fragments_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maiden_essence_fragments_AuraScript);

            ///                  Dist,  Angle
            std::array<std::pair<float, float>, 6> m_RelativesDistAndAngle =
            {
                {
                    { 3.605603f, 2.164170f },
                    { 11.88517f, 2.891737f },
                    { 11.88486f, 3.402092f },
                    { 3.605501f, 4.129739f },
                    { 7.615149f, 0.437842f },
                    { 7.615149f, 5.883560f }
                }
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    for (auto const& l_Pair : m_RelativesDistAndAngle)
                    {
                        Position l_Pos = l_Caster->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Pair.first, l_Pair.second);

                        if (urand(0, 1))
                            l_Caster->CastSpell(l_Pos, eSpells::SpellFragmentOfCreationMissile, true);
                        else
                            l_Caster->CastSpell(l_Pos, eSpells::SpellFragmentOfObliterationMissile, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_maiden_essence_fragments_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maiden_essence_fragments_AuraScript();
        }
};

/// Unstable Soul (AoE Damages) - 238138
class spell_maiden_unstable_soul_aoe : public SpellScriptLoader
{
    public:
        spell_maiden_unstable_soul_aoe() : SpellScriptLoader("spell_maiden_unstable_soul_aoe") { }

        class spell_maiden_unstable_soul_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_maiden_unstable_soul_aoe_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// LFR Difficulty: The Unstable Soul now only deals damage to the afflicted player when it explodes, rather than the entire raid.
                    if (l_Caster->GetMap()->IsLFR())
                    {
                        p_Targets.clear();
                        p_Targets.push_back(l_Caster);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_maiden_unstable_soul_aoe_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_SRC_CASTER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_maiden_unstable_soul_aoe_SpellScript();
        }
};

/// Essence Fragments - 250500
class areatrigger_maiden_essence_fragments : public AreaTriggerEntityScript
{
    public:
        areatrigger_maiden_essence_fragments() : AreaTriggerEntityScript("areatrigger_maiden_essence_fragments") { }

        enum eSpells
        {
            SpellCreatorsGrace      = 235534,
            SpellDemonsVigor        = 235538,
            SpellFelInfusionAura    = 235240,
            SpellFelInfusionAT      = 241316,
            SpellLightInfusionAT    = 241315,
            SpellUnstableSoulAura   = 235117,

            SpellFragmentBurstAoE   = 239069,
            SpellFragmentBurstDoT   = 248801
        };

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster || !p_Target->IsPlayer())
                return true;

            bool l_HasLight = !p_Target->HasAura(eSpells::SpellFelInfusionAura);

            if (l_Caster->GetEntry() == eCreatures::NpcFragmentOfCreation)
            {
                if (l_HasLight)
                {
                    std::list<Player*> l_PlayerList;

                    p_Target->GetPlayerListInGrid(l_PlayerList, 5.0f);

                    for (Player* l_Player : l_PlayerList)
                    {
                        if (!l_Player->HasAura(eSpells::SpellFelInfusionAura))
                            l_Player->CastSpell(l_Player, eSpells::SpellCreatorsGrace, true);
                    }

                    p_Target->CastSpell(p_Target, eSpells::SpellCreatorsGrace, true);
                }
                else
                    TriggerUnstability(p_Target, l_Caster);
            }
            else
            {
                if (!l_HasLight)
                {
                    std::list<Player*> l_PlayerList;

                    p_Target->GetPlayerListInGrid(l_PlayerList, 5.0f);

                    for (Player* l_Player : l_PlayerList)
                    {
                        if (l_Player->HasAura(eSpells::SpellFelInfusionAura))
                            l_Player->CastSpell(l_Player, eSpells::SpellDemonsVigor, true);
                    }

                    p_Target->CastSpell(p_Target, eSpells::SpellDemonsVigor, true);
                }
                else
                    TriggerUnstability(p_Target, l_Caster);
            }

            p_AreaTrigger->SetDuration(0);
            p_AreaTrigger->SetOnDespawn(true);

            if (l_Caster->ToCreature())
                l_Caster->ToCreature()->DespawnOrUnsummon(1);

            return true;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (p_AreaTrigger->IsOnDespawn() || !p_AreaTrigger->GetCaster() || !p_AreaTrigger->GetMap()->IsMythic())
                return;

            if (Unit* l_Caster = p_AreaTrigger->GetCaster()->GetAnyOwner())
            {
                l_Caster->CastSpell(l_Caster, eSpells::SpellFragmentBurstAoE, true);
                l_Caster->CastSpell(l_Caster, eSpells::SpellFragmentBurstDoT, true);
            }
        }

        void TriggerUnstability(Unit* p_Target, Unit* p_Caster)
        {
            if (!p_Target->HasAura(eSpells::SpellUnstableSoulAura))
            {
                p_Target->RemoveAura(eSpells::SpellFelInfusionAT);
                p_Target->RemoveAura(eSpells::SpellLightInfusionAT);

                p_Caster->CastSpell(p_Target, eSpells::SpellUnstableSoulAura, true);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_maiden_essence_fragments();
        }
};

/// Starter Area Trigger - 240341
class areatrigger_maiden_starter_area_trigger : public AreaTriggerEntityScript
{
    public:
        areatrigger_maiden_starter_area_trigger() : AreaTriggerEntityScript("areatrigger_maiden_starter_area_trigger") { }

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster || !p_Target->IsPlayer())
                return true;

            if (!l_Caster->GetAnyOwner() || !l_Caster->GetAnyOwner()->ToCreature())
                return true;

            Creature* l_Boss = l_Caster->GetAnyOwner()->ToCreature();
            if (!l_Boss || !l_Boss->IsAIEnabled)
                return true;

            l_Boss->AI()->DoAction(0);

            p_AreaTrigger->SetDuration(0);
            p_AreaTrigger->SetOnDespawn(true);

            if (l_Caster->ToCreature())
                l_Caster->ToCreature()->DespawnOrUnsummon(1);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_maiden_starter_area_trigger();
        }
};

/// First door to Fallen Avatar - 269261
class go_maiden_first_door_to_avatar : public GameObjectScript
{
    public:
        go_maiden_first_door_to_avatar() : GameObjectScript("go_maiden_first_door_to_avatar") { }

        struct go_maiden_first_door_to_avatarAI : public GameObjectAI
        {
            go_maiden_first_door_to_avatarAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || go->GetGoState() == GOState::GO_STATE_ACTIVE)
                    return;

                if (p_Who->GetDistance(go) >= 45.0f)
                    return;

                go->SetGoState(GOState::GO_STATE_ACTIVE);

                std::list<Creature*> l_Creatures;
                go->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcDefensiveCountermeasure, 100.0f);
                for (Creature* l_Creature : l_Creatures)
                {
                    if (l_Creature->isAlive())
                        return;
                }

                if (GameObject* l_SecondDoor = go->FindNearestGameObject(eGameObjects::GoDoorPreFallenAvatar, 150.0f))
                    l_SecondDoor->SetGoState(GOState::GO_STATE_ACTIVE);
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_maiden_first_door_to_avatarAI(p_GameObject);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_maiden_of_vigilance()
{
    /// Boss
    new boss_maiden_of_vigilance();

    /// Creatures
    new npc_maiden_fragment_of_creation();
    new npc_maiden_fragment_of_obliteration();
    new npc_maiden_light_remanence_stalker();
    new npc_maiden_fel_remanence_stalker();
    new npc_maiden_titanic_anomaly();

    /// Spells
    new spell_maiden_light_infusion();
    new spell_maiden_fel_infusion();
    new spell_maiden_unstable_soul();
    new spell_maiden_wrath_of_the_creators_dmg();
    new spell_maiden_blowback();
    new spell_maiden_essence_fragments();
    new spell_maiden_unstable_soul_aoe();

    /// AreaTriggers
    new areatrigger_maiden_essence_fragments();
    new areatrigger_maiden_starter_area_trigger();

    /// GameObject
    new go_maiden_first_door_to_avatar();
}
#endif
