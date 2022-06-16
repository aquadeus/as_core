////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"
#include "HelperDefines.h"

enum eScriptData
{
    HasAppliedTouch         = 1,
    ActionApplySecondTouch,

    ShatugJumpToBridge = 2000,
    ShatugJumpToPlatform,

    ShatugLittlePatrol1 = 122135000,
    ShatugLittlePatrol2,
    ShatugLongPatrol1,
    ShatugLongPatrol2,
    ShatugLongPatrol3,

    PatrolCount = 9,
};

std::array<uint32, eScriptData::PatrolCount> g_Patrols =
{{
    eScriptData::ShatugLittlePatrol1,
    eScriptData::ShatugLittlePatrol2,
    eScriptData::ShatugLittlePatrol1,
    eScriptData::ShatugLittlePatrol2,
    eScriptData::ShatugLittlePatrol1,
    eScriptData::ShatugLittlePatrol2,
    eScriptData::ShatugLongPatrol1,
    eScriptData::ShatugLongPatrol2,
    eScriptData::ShatugLongPatrol3
}};

/// Last Update 7.3.5 - Build 26365
/// Shatug  (Felhound of Sargeras) - 122135
class boss_felhound_shatug : public CreatureScript
{
    public:
        boss_felhound_shatug() : CreatureScript("boss_felhound_shatug") { }

        struct boss_felhound_shatugAI : BossAI
        {
            boss_felhound_shatugAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataFelhoundsOfSargeras)
            {
                m_PatrolCount = Occurence(4, 0, eScriptData::PatrolCount - 1);
                m_FhargGUID = 0;
                m_NeverEngaged = true;

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Fharg = me->FindNearestCreature(eCreatures::BossFharg, 100.0f, true);
                    if (!l_Fharg)
                        l_Fharg = me->FindNearestCreature(eCreatures::BossFharg, 100.0f, false);

                    if (!l_Fharg)
                        return;

                    m_FhargGUID = l_Fharg->GetGUID();
                });

                switch (me->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidLFR:
                    case Difficulty::DifficultyRaidNormal:
                        m_EnergyRegenTimer = (85 * TimeConstants::IN_MILLISECONDS) / 100;
                        break;
                    case Difficulty::DifficultyRaidHeroic:
                        m_EnergyRegenTimer = (80 * TimeConstants::IN_MILLISECONDS) / 100;
                        break;
                    case Difficulty::DifficultyRaidMythic:
                        m_EnergyRegenTimer = (73 * TimeConstants::IN_MILLISECONDS) / 100;
                        break;
                    default:
                        m_EnergyRegenTimer = static_cast<uint32>(-1);
                }
            }

            enum eSpells
            {
                /// Always
                CorruptingMaw               = 254760,
                CorruptingMawAvailable      = 251447,
                SoulLink                    = 244051,
                DestroyersBoon              = 244050,
                SargerasBlessing            = 246057,
                FocusingPower               = 251356,

                /// Triggered by energy level
                ConsumingSphereAreatrigger  = 244107,
                SiphonCorruptionSearcher    = 244578,
                SiphonCorruption            = 244056,

                /// LFR
                EruptingShadows             = 257238,

                /// Heroic
                WeightOfDarkness            = 244069,

                /// Mythic
                DarkReconstitution          = 249113,
                Flametouched                = 244054,
                Shadowtouched               = 244055,
                ConsumingDetonation         = 251366,

                /// Cosmetic
                Uninterractible             = 253280,
            };

            enum eEvents
            {
                EventCorruptingMaw = 1,
                EventRegenEnergy,
                EventCheckDistance,
                EventFocusingPower,

                EventConsumingSphere,
                EventWeightOfDarkness,
                EventSiphonCorruption
            };

            Occurence m_PatrolCount;
            bool m_NeverEngaged;

            uint64 m_FhargGUID;
            uint32 m_EnergySpellCasted;
            uint32 m_EnergyRegenTimer;

            EventMap m_TriggeredEvents;

            std::set<uint64> m_Shadowtouched;
            bool m_SecondTouchApplied;

            Creature* GetFharg()
            {
                if (!m_FhargGUID)
                {
                    Creature* l_Fharg = me->FindNearestCreature(eCreatures::BossFharg, 300.0f, true);
                    if (!l_Fharg)
                        l_Fharg = me->FindNearestCreature(eCreatures::BossFharg, 300.0f, false);

                    if (!l_Fharg)
                        return nullptr;

                    m_FhargGUID = l_Fharg->GetGUID();
                }

                Creature* l_Fharg = sObjectAccessor->FindCreature(m_FhargGUID);
                return (l_Fharg && l_Fharg->IsAIEnabled) ? l_Fharg : nullptr;
            }

            void Patrol()
            {
                if (me->isInCombat())
                    return;

                if (m_PatrolCount >= 6 && !m_NeverEngaged)
                    m_PatrolCount = 0;

                me->GetMotionMaster()->MovePathWithPointId(g_Patrols[m_PatrolCount], g_Patrols[m_PatrolCount], g_Patrols[m_PatrolCount] < eScriptData::ShatugLongPatrol1);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case eScriptData::ShatugLittlePatrol1:
                    case eScriptData::ShatugLittlePatrol2:
                    case eScriptData::ShatugLongPatrol1:
                    case eScriptData::ShatugLongPatrol2:
                    {
                        /// Switch on next paths
                        switch (g_Patrols[++m_PatrolCount])
                        {
                            case eScriptData::ShatugLittlePatrol1:
                            case eScriptData::ShatugLittlePatrol2:
                            {
                                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                                {
                                    Patrol();
                                });
                                break;
                            }
                            case eScriptData::ShatugLongPatrol1:
                            {
                                if (!m_NeverEngaged)
                                {
                                    AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                                    {
                                        Patrol();
                                    });
                                    break;
                                }

                                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                                {
                                    me->SetWalk(false);
                                    me->SetSpeed(MOVE_RUN, 5.0f);
                                    me->SetReactState(ReactStates::REACT_PASSIVE);
                                    me->CastSpell(me, eSpells::Uninterractible, true);
                                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);

                                    if (Creature* l_Fharg = GetFharg())
                                    {
                                        me->SetReactState(ReactStates::REACT_PASSIVE);
                                        l_Fharg->CastSpell(l_Fharg, eSpells::Uninterractible, true);
                                        l_Fharg->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                                        l_Fharg->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);
                                    }

                                    me->GetMotionMaster()->MoveReverseJump({-3130.84f, 10428.2f, -194.775f}, 2049, 19.2911f, eScriptData::ShatugJumpToBridge);
                                });
                                break;
                            }
                            case eScriptData::ShatugLongPatrol2:
                            {
                                me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_ROAR);
                                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                                {
                                    Patrol();
                                });
                                break;
                            }
                            case eScriptData::ShatugLongPatrol3:
                            {
                                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 438);
                                AddTimedDelayedOperation(23 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);
                                    me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_ROAR);
                                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        Patrol();
                                    });
                                });

                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    case eScriptData::ShatugLongPatrol3:
                    {
                        ++m_PatrolCount;
                        me->GetMotionMaster()->MoveReverseJump({ -3215.8f, 10368.8f, -155.472f }, 2386, 19.2911f, eScriptData::ShatugJumpToPlatform);
                        break;
                    }
                    case eScriptData::ShatugJumpToBridge:
                    {
                        Patrol();
                        break;
                    }
                    case eScriptData::ShatugJumpToPlatform:
                    {
                        me->SetWalk(true);
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        me->RemoveAura(eSpells::Uninterractible);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);

                        if (Creature* l_Fharg = GetFharg())
                        {
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            l_Fharg->RemoveAura(eSpells::Uninterractible);
                            l_Fharg->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Fharg->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);
                        }

                        Patrol();
                        break;
                    }
                    default:
                        break;
                }
            }

            void Reset() override
            {
                BossAI::_Reset();

                ClearDelayedOperations();
                m_TriggeredEvents.Reset();
                events.Reset();

                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    DoCast(eSpells::SoulLink, true);

                DoCast(eSpells::DestroyersBoon);
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_OVERRIDE_DISPLAY_POWER_ID, 241);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(Powers::POWER_ENERGY, 86);
                m_EnergySpellCasted = 2;

                me->SetSpeed(UnitMoveType::MOVE_WALK, 2.0f);
                me->SetWalk(true);

                m_PatrolCount = 0;
                Patrol();

                m_Shadowtouched.clear();
                m_SecondTouchApplied = false;

                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        l_Player->RemoveAura(eSpells::Shadowtouched);
                        l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                    }
                }
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                if (Creature* l_Fharg = GetFharg())
                    reinterpret_cast<BossAI*>(l_Fharg->AI())->BossAI::EnterEvadeMode();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                me->SetWalk(false);
                ClearDelayedOperations();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveCharge(p_Attacker);

                if (Creature* l_Fharg = GetFharg())
                    l_Fharg->AI()->AttackStart(p_Attacker);

                DefaultEvents();

                if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                    ApplyShadowtouched();

                m_NeverEngaged = false;
            }

            void FilterTargets(std::list<WorldObject *>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SiphonCorruptionSearcher:
                    {
                        p_Targets.remove_if([](WorldObject const* p_Target) -> bool
                        {
                            if (!p_Target->IsPlayer())
                                return true;

                            if (p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            return p_Target->ToPlayer()->HasAura(eSpells::SiphonCorruption);
                        });

                        JadeCore::RandomResizeList(p_Targets, 1);

                        break;
                    }
                    case eSpells::WeightOfDarkness:
                    {
                        JadeCore::RandomResizeList(p_Targets, std::max<uint32>(me->GetMap()->GetPlayersCountExceptGMs() / 5 + 2, 4));
                        break;
                    }
                    default:
                        break;
                }
            }

            void ApplyShadowtouched()
            {
                if (!instance || !instance->instance)
                    return;

                /// Second application, shadow touch everyone that wasn't the first time
                if (!m_Shadowtouched.empty())
                {
                    for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->isGameMaster())
                                continue;

                            if (m_Shadowtouched.find(l_Player->GetGUID()) != m_Shadowtouched.end())
                                continue;

                            me->CastSpell(l_Player, eSpells::Shadowtouched, true);
                        }
                    }

                    return;
                }

                if (Creature* l_Fharg = GetFharg())
                {
                    if (l_Fharg->AI()->GetData(eScriptData::HasAppliedTouch))
                    {
                        for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (l_Player->isGameMaster())
                                    continue;

                                if (l_Player->HasAura(eSpells::Flametouched))
                                    continue;

                                me->CastSpell(l_Player, eSpells::Shadowtouched, true);
                                m_Shadowtouched.insert(l_Player->GetGUID());
                            }
                        }

                        return;
                    }
                }

                /// This mechanic should affect exactly half of the DPS, half of the healers, and half of the tanks
                uint32 l_MeleeDPSIndex = 0;
                uint32 l_RangedDPSIndex = 0;
                uint32 l_TankIndex = 0;
                uint32 l_HealerIndex = 0;
                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        if (l_Player->IsActiveSpecTankSpec())
                        {
                            if ((l_TankIndex++) % 2)
                                continue;
                        }

                        if (l_Player->IsHealer())
                        {
                            if ((l_HealerIndex++) % 2)
                                continue;
                        }

                        if (l_Player->IsRangedDamageDealer())
                        {
                            if ((l_RangedDPSIndex++) % 2)
                                continue;
                        }

                        if (l_Player->IsMeleeDamageDealer())
                        {
                            if ((l_MeleeDPSIndex++) % 2)
                                continue;
                        }

                        me->CastSpell(l_Player, eSpells::Shadowtouched, true);
                        m_Shadowtouched.insert(l_Player->GetGUID());
                    }
                }
            }

            void ApplySecondTouch()
            {
                if (m_SecondTouchApplied)
                    return;

                if (!instance || !instance->instance)
                    return;

                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->RemoveAura(eSpells::Shadowtouched);
                }

                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                {
                    ApplyShadowtouched();
                });

                m_SecondTouchApplied = true;

                if (Creature* l_Fharg = GetFharg())
                    l_Fharg->AI()->DoAction(eScriptData::ActionApplySecondTouch);
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                switch (p_ID)
                {
                    case eScriptData::HasAppliedTouch:
                        return !m_Shadowtouched.empty();
                    default:
                        break;
                }

                return 0;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eScriptData::ActionApplySecondTouch:
                        ApplySecondTouch();
                        break;
                    default:
                        break;
                }
            }

            void DefaultEvents()
            {
                m_TriggeredEvents.ScheduleEvent(eEvents::EventCorruptingMaw, 10 * TimeConstants::IN_MILLISECONDS);
                m_TriggeredEvents.ScheduleEvent(eEvents::EventRegenEnergy, 1 * TimeConstants::IN_MILLISECONDS);
                m_TriggeredEvents.ScheduleEvent(eEvents::EventCheckDistance, 1 * TimeConstants::IN_MILLISECONDS);
                m_TriggeredEvents.ScheduleEvent(eEvents::EventFocusingPower, 0 * TimeConstants::IN_MILLISECONDS);
            }

            void CheckEnergy()
            {
                if (me->GetPower(Powers::POWER_ENERGY) >= 33 && !m_EnergySpellCasted)
                {
                    events.ScheduleEvent(eEvents::EventConsumingSphere, 0);
                    m_EnergySpellCasted = 1;
                }

                if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                    || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                {
                    if (me->GetPower(Powers::POWER_ENERGY) >= 66 && m_EnergySpellCasted < 2)
                    {
                        events.ScheduleEvent(eEvents::EventWeightOfDarkness, 0);
                        m_EnergySpellCasted = 2;
                    }
                }

                if (me->GetPower(Powers::POWER_ENERGY) >= 100)
                {
                    events.ScheduleEvent(eEvents::EventSiphonCorruption, 0);
                    me->SetPower(Powers::POWER_ENERGY, 0);
                    m_EnergySpellCasted = 0;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                if (me->GetHealthPct() < 50.0f)
                {
                    ApplySecondTouch();
                }

                if (!me->GetHealth() || p_Damage < me->GetHealth())
                    return;

                p_Damage = me->GetHealth() - 1;

                if (Creature* l_Fharg = GetFharg())
                {
                    if (l_Fharg->GetHealth() == 1)
                    {
                        p_Attacker->Kill(l_Fharg);
                        p_Attacker->Kill(me);
                    }
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    me->CastStop(eSpells::DarkReconstitution);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                me->CastSpell(me, eSpells::DarkReconstitution, false);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                events.Update(p_Diff);
                m_TriggeredEvents.Update(p_Diff);

                if (!UpdateVictim())
                    return;

                Position l_HomePos = me->GetHomePosition();
                if (me->GetExactDist2d(&l_HomePos) > 115.0f)
                    EnterEvadeMode();

                CheckEnergy();

                switch (m_TriggeredEvents.ExecuteEvent())
                {
                    case eEvents::EventCorruptingMaw:
                    {
                        me->CastSpell(me, eSpells::CorruptingMawAvailable, true);
                        m_TriggeredEvents.ScheduleEvent(eEvents::EventCorruptingMaw, 10 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRegenEnergy:
                    {
                        if (!me->HasAura(eSpells::FocusingPower))
                            me->ModifyPower(Powers::POWER_ENERGY, 1);

                        m_TriggeredEvents.ScheduleEvent(eEvents::EventRegenEnergy, m_EnergyRegenTimer);
                        break;
                    }
                    case eEvents::EventCheckDistance:
                    {
                        Creature* l_Fharg = GetFharg();
                        if (l_Fharg && me->GetExactDist2d(l_Fharg) < 40.0f)
                        {
                            if (AuraEffect* l_AuraEffect = me->GetAuraEffect(eSpells::DestroyersBoon, EFFECT_3))
                                l_AuraEffect->SetAmount(100);

                            me->CastSpell(me, eSpells::SargerasBlessing, true);
                        }
                        else
                        {
                            if (AuraEffect* l_AuraEffect = me->GetAuraEffect(eSpells::DestroyersBoon, EFFECT_3))
                                l_AuraEffect->SetAmount(0);

                            me->RemoveAura(eSpells::SargerasBlessing);
                        }

                        m_TriggeredEvents.ScheduleEvent(eEvents::EventCheckDistance, 1 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFocusingPower:
                    {
                        DoCast(eSpells::FocusingPower, true);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            m_TriggeredEvents.ScheduleEvent(eEvents::EventFocusingPower, 105 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventConsumingSphere:
                    {
                        std::list<Player*> l_Players;
                        float l_Radius = 200.0f;
                        JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck l_Check(me, l_Radius);
                        JadeCore::PlayerListSearcher<decltype(l_Check)> l_Searcher(me, l_Players, l_Check);
                        me->VisitNearbyObject(l_Radius, l_Searcher);
                        if (l_Players.empty())
                            break;

                        l_Players.sort(JadeCore::UnitSortDistance(false, me));
                        Unit* l_Target = l_Players.front();
                        if (!l_Target)
                            break;

                        me->CastSpell(l_Target, eSpells::ConsumingSphereAreatrigger, false);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            me->CastSpell(l_Target, eSpells::ConsumingDetonation, true);

                        AddTimedDelayedOperation(20900, [this]() -> void
                        {
                            AreaTrigger* l_AreaTrigger = me->GetAreaTrigger(eSpells::ConsumingSphereAreatrigger);
                            if (!l_AreaTrigger)
                                return;

                            l_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                            l_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                            l_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                        });

                        break;
                    }
                    case eEvents::EventWeightOfDarkness:
                    {
                        DoCast(eSpells::WeightOfDarkness, true);
                        break;
                    }
                    case eEvents::EventSiphonCorruption:
                    {
                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidLFR)
                            DoCast(eSpells::EruptingShadows, false);
                        else
                            DoCast(eSpells::SiphonCorruption, false);

                        break;
                    }
                    default:
                        break;
                }

                if (me->HasAura(eSpells::CorruptingMawAvailable))
                {
                    if (me->isAttackReady())
                    {
                        me->CastSpell(me->getVictim(), eSpells::CorruptingMaw, false);
                        me->resetAttackTimer();
                    }
                }
                else
                    DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (!instance || !instance->instance)
                    return;

                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->RemoveAura(eSpells::Shadowtouched);
                }

                instance->PlayCosmeticEvent(eCosmeticEvents::AntorusFelhoundsOutro);

                me->SetFlag(EObjectFields::OBJECT_FIELD_DYNAMIC_FLAGS, UnitDynFlags::UNIT_DYNFLAG_LOOTABLE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_felhound_shatugAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// F'harg (Felhound of Sargeras) - 122477
class boss_felhound_fharg : public CreatureScript
{
    public:
        boss_felhound_fharg() : CreatureScript("boss_felhound_fharg") { }

        struct boss_felhound_fhargAI : BossAI
        {
            boss_felhound_fhargAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataFelhoundsOfSargeras)
            {
                m_ShatugGUID = 0;

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Shatug = me->FindNearestCreature(eCreatures::BossShatug, 100.0f, true);
                    if (!l_Shatug)
                        l_Shatug = me->FindNearestCreature(eCreatures::BossShatug, 100.0f, false);

                    if (!l_Shatug)
                        return;

                    m_ShatugGUID = l_Shatug->GetGUID();
                });

                switch (me->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidLFR:
                    case Difficulty::DifficultyRaidNormal:
                        m_EnergyRegenTimer = (103 * TimeConstants::IN_MILLISECONDS) / 100;
                        break;
                    case Difficulty::DifficultyRaidHeroic:
                        m_EnergyRegenTimer = (100 * TimeConstants::IN_MILLISECONDS) / 100;
                        break;
                    case Difficulty::DifficultyRaidMythic:
                        m_EnergyRegenTimer = (88 * TimeConstants::IN_MILLISECONDS) / 100;
                        break;
                    default:
                        m_EnergyRegenTimer = static_cast<uint32>(-1);
                }
            }

            enum eSpells
            {
                /// Always
                BurningMaw                  = 254747,
                BurningMawAvailable         = 251448,
                SoulLink                    = 244051,
                DestroyersBoon              = 244050,
                SargerasBlessing            = 246057,
                FocusingPower               = 251356,

                /// Triggered by energy level
                DesolateGazeCastBar         = 244825,
                DesolateGazeSearcher        = 244441,
                EnflameCorruptionSearcher   = 244471,
                EnflameCorruption           = 244057,

                /// LFR
                EruptingFlames              = 257236,

                /// Heroic
                MoltenTouch                 = 244072,

                /// Mythic
                DarkReconstitution          = 249113,
                Flametouched                = 244054,
                Shadowtouched               = 244055,
                BurningRemnant              = 245023
            };

            enum eEvents
            {
                EventBurningMaw = 1,
                EventRegenEnergy,
                EventCheckDistance,
                EventFocusingPower,

                EventDesolateGaze,
                EventMoltenTouch,
                EventEnflameCorruption
            };

            uint64 m_ShatugGUID;
            bool m_Evading;
            uint32 m_EnergySpellCasted;
            uint32 m_EnergyRegenTimer;

            EventMap m_TriggeredEvents;

            std::set<uint64> m_Flametouched;
            bool m_SecondTouchApplied;

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            Creature* GetShatug()
            {
                if (!m_ShatugGUID)
                {
                    Creature* l_Shatug = me->FindNearestCreature(eCreatures::BossShatug, 300.0f, true);
                    if (!l_Shatug)
                        l_Shatug = me->FindNearestCreature(eCreatures::BossShatug, 300.0f, false);

                    if (!l_Shatug)
                        return nullptr;

                    m_ShatugGUID = l_Shatug->GetGUID();
                }

                Creature* l_Shatug = sObjectAccessor->FindCreature(m_ShatugGUID);
                return (l_Shatug && l_Shatug->IsAIEnabled) ? l_Shatug : nullptr;
            }

            void Reset() override
            {
                BossAI::_Reset();

                m_TriggeredEvents.Reset();
                events.Reset();

                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    DoCast(eSpells::SoulLink, true);

                DoCast(eSpells::DestroyersBoon);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(Powers::POWER_ENERGY, 62);
                m_EnergySpellCasted = 1;

                m_Flametouched.clear();
                m_SecondTouchApplied = false;

                if (!instance || !instance->instance)
                    return;

                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->RemoveAura(eSpells::Flametouched);
                }
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                if (Creature* l_Shatug = GetShatug())
                    reinterpret_cast<BossAI*>(l_Shatug->AI())->BossAI::EnterEvadeMode();

                me->DespawnAreaTriggersInArea(eSpells::BurningRemnant, 250.0f);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                Position l_JumpPosition = { -3239.97f, 10423.67f, -155.08f };
                me->GetMotionMaster()->MoveJump(l_JumpPosition, 50.0f, 10.0f);

                if (Creature* l_Shatug = GetShatug())
                    l_Shatug->AI()->AttackStart(p_Attacker);

                DefaultEvents();

                if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                    ApplyFlametouched();
            }

            void FilterTargets(std::list<WorldObject *>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::EnflameCorruptionSearcher:
                    {
                        uint32 l_TankCount = 0;
                        p_Targets.remove_if([&l_TankCount](WorldObject const* p_Target) -> bool
                        {
                            if (!p_Target->IsPlayer())
                                return true;

                            if (p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                            {
                                l_TankCount++;
                                return true;
                            }

                            return p_Target->ToPlayer()->HasAura(eSpells::EnflameCorruption);
                        });

                        uint32 l_TargetCount = 0;
                        if (me->GetMap() && me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            l_TargetCount = 3;
                        else
                            l_TargetCount = std::min<uint32>((me->GetMap()->GetPlayersCountExceptGMs() - l_TankCount) / 3 + 1, p_Targets.size());

                        JadeCore::RandomResizeList(p_Targets, l_TargetCount);

                        break;
                    }
                    case eSpells::MoltenTouch:
                    {
                        p_Targets.remove_if([](WorldObject const* p_Target) -> bool
                        {
                            if (!p_Target->IsPlayer())
                                return true;

                            if (p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            return false;
                        });

                        JadeCore::RandomResizeList(p_Targets, 3);

                        break;
                    }
                    case eSpells::DesolateGazeSearcher:
                    {
                        JadeCore::RandomResizeList(p_Targets, std::max<uint32>(2, me->GetMap()->GetPlayersCountExceptGMs() / 10 + 1));
                        break;
                    }
                    default:
                        break;
                }
            }

            void ApplyFlametouched()
            {
                if (!instance || !instance->instance)
                    return;

                /// Second application, shadow touch everyone that wasn't the first time
                if (!m_Flametouched.empty())
                {
                    for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->isGameMaster())
                                continue;

                            if (m_Flametouched.find(l_Player->GetGUID()) != m_Flametouched.end())
                                continue;

                            me->CastSpell(l_Player, eSpells::Flametouched, true);
                        }
                    }

                    return;
                }

                if (Creature* l_Shatug = GetShatug())
                {
                    if (l_Shatug->AI()->GetData(eScriptData::HasAppliedTouch))
                    {
                        for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (l_Player->isGameMaster())
                                    continue;

                                if (l_Player->HasAura(eSpells::Shadowtouched))
                                    continue;

                                me->CastSpell(l_Player, eSpells::Flametouched, true);
                                m_Flametouched.insert(l_Player->GetGUID());
                            }
                        }

                        return;
                    }
                }

                /// This mechanic should affect exactly half of the DPS, half of the healers, and half of the tanks
                uint32 l_MeleeDPSIndex = 0;
                uint32 l_RangedDPSIndex = 0;
                uint32 l_TankIndex = 0;
                uint32 l_HealerIndex = 0;
                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        if (l_Player->IsActiveSpecTankSpec())
                        {
                            if ((l_TankIndex++) % 2)
                                continue;
                        }

                        if (l_Player->IsHealer())
                        {
                            if ((l_HealerIndex++) % 2)
                                continue;
                        }

                        if (l_Player->IsRangedDamageDealer())
                        {
                            if ((l_RangedDPSIndex++) % 2)
                                continue;
                        }

                        if (l_Player->IsMeleeDamageDealer())
                        {
                            if ((l_MeleeDPSIndex++) % 2)
                                continue;
                        }

                        me->CastSpell(l_Player, eSpells::Flametouched, true);
                        m_Flametouched.insert(l_Player->GetGUID());
                    }
                }
            }

            void ApplySecondTouch()
            {
                if (m_SecondTouchApplied)
                    return;

                if (!instance || !instance->instance)
                    return;

                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->RemoveAura(eSpells::Flametouched);
                }

                AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                {
                    ApplyFlametouched();
                });

                m_SecondTouchApplied = true;

                if (Creature* l_Shatug = GetShatug())
                    l_Shatug->AI()->DoAction(eScriptData::ActionApplySecondTouch);
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                switch (p_ID)
                {
                    case eScriptData::HasAppliedTouch:
                        return !m_Flametouched.empty();
                    default:
                        break;
                }

                return 0;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eScriptData::ActionApplySecondTouch:
                        ApplySecondTouch();
                        break;
                    default:
                        break;
                }
            }

            void DefaultEvents()
            {
                m_TriggeredEvents.ScheduleEvent(eEvents::EventBurningMaw, 10 * TimeConstants::IN_MILLISECONDS);
                m_TriggeredEvents.ScheduleEvent(eEvents::EventRegenEnergy, 1 * TimeConstants::IN_MILLISECONDS);
                m_TriggeredEvents.ScheduleEvent(eEvents::EventCheckDistance, 1 * TimeConstants::IN_MILLISECONDS);
                m_TriggeredEvents.ScheduleEvent(eEvents::EventFocusingPower, 0 * TimeConstants::IN_MILLISECONDS);
            }

            void CheckEnergy()
            {
                if (me->GetPower(Powers::POWER_ENERGY) >= 33 && !m_EnergySpellCasted)
                {
                    events.ScheduleEvent(eEvents::EventDesolateGaze, 0);
                    m_EnergySpellCasted = 1;
                }

                if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                    || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                {
                    if (me->GetPower(Powers::POWER_ENERGY) >= 66 && m_EnergySpellCasted < 2)
                    {
                        events.ScheduleEvent(eEvents::EventMoltenTouch, 0);
                        m_EnergySpellCasted = 2;
                    }
                }

                if (me->GetPower(Powers::POWER_ENERGY) >= 100)
                {
                    events.ScheduleEvent(eEvents::EventEnflameCorruption, 0);
                    me->SetPower(Powers::POWER_ENERGY, 0);
                    m_EnergySpellCasted = 0;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                if (me->GetHealthPct() < 50.0f)
                {
                    ApplySecondTouch();
                }

                if (!me->GetHealth() || p_Damage < me->GetHealth())
                    return;

                p_Damage = me->GetHealth() - 1;

                if (Creature* l_Shatug = GetShatug())
                {
                    if (l_Shatug->GetHealth() == 1)
                    {
                        p_Attacker->Kill(l_Shatug);
                        p_Attacker->Kill(me);
                    }
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    me->CastStop(eSpells::DarkReconstitution);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                me->CastSpell(me, eSpells::DarkReconstitution, false);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                events.Update(p_Diff);
                m_TriggeredEvents.Update(p_Diff);

                if (!UpdateVictim())
                    return;

                Position l_HomePos = me->GetHomePosition();
                if (me->GetExactDist2d(&l_HomePos) > 140.0f)
                    EnterEvadeMode();

                CheckEnergy();

                switch (m_TriggeredEvents.ExecuteEvent())
                {
                    case eEvents::EventBurningMaw:
                    {
                        me->CastSpell(me, eSpells::BurningMawAvailable, true);
                        m_TriggeredEvents.ScheduleEvent(eEvents::EventBurningMaw, 10 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRegenEnergy:
                    {
                        if (!me->HasAura(eSpells::FocusingPower))
                            me->ModifyPower(Powers::POWER_ENERGY, 1);

                        m_TriggeredEvents.ScheduleEvent(eEvents::EventRegenEnergy, m_EnergyRegenTimer);
                        break;
                    }
                    case eEvents::EventCheckDistance:
                    {
                        Creature* l_Shatug = GetShatug();
                        if (l_Shatug && me->GetExactDist2d(l_Shatug) < 40.0f)
                        {
                            if (AuraEffect* l_AuraEffect = me->GetAuraEffect(eSpells::DestroyersBoon, EFFECT_3))
                                l_AuraEffect->SetAmount(100);

                            me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
                            me->CastSpell(me, eSpells::SargerasBlessing, true);
                        }
                        else
                        {
                            if (AuraEffect* l_AuraEffect = me->GetAuraEffect(eSpells::DestroyersBoon, EFFECT_3))
                                l_AuraEffect->SetAmount(0);

                            me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
                            me->RemoveAura(eSpells::SargerasBlessing);
                        }

                        m_TriggeredEvents.ScheduleEvent(eEvents::EventCheckDistance, 1 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFocusingPower:
                    {
                        DoCast(eSpells::FocusingPower, true);

                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            m_TriggeredEvents.ScheduleEvent(eEvents::EventFocusingPower, 105 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDesolateGaze:
                    {
                        DoCast(eSpells::DesolateGazeSearcher, true);
                        AddTimedDelayedOperation(6 * IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(eSpells::DesolateGazeCastBar, false);
                        });
                        break;
                    }
                    case eEvents::EventMoltenTouch:
                    {
                        DoCast(eSpells::MoltenTouch, true);

                        break;
                    }
                    case eEvents::EventEnflameCorruption:
                    {
                        if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidLFR)
                            DoCast(eSpells::EruptingFlames, false);
                        else
                            DoCast(eSpells::EnflameCorruption, false);

                        break;
                    }
                    default:
                        break;
                }

                if (me->HasAura(eSpells::BurningMawAvailable))
                {
                    if (me->isAttackReady())
                    {
                        me->CastSpell(me->getVictim(), eSpells::BurningMaw, false);
                        me->resetAttackTimer();
                    }
                }
                else
                    DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                me->DespawnAreaTriggersInArea(eSpells::BurningRemnant, 250.0f);

                if (!instance || !instance->instance)
                    return;

                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                        l_Player->RemoveAura(eSpells::Flametouched);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_felhound_fhargAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Burning Maw - 254747
class spell_fharg_burning_maw : public SpellScriptLoader
{
    public:
        spell_fharg_burning_maw() : SpellScriptLoader("spell_fharg_burning_maw") { }

        class spell_fharg_burning_maw_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fharg_burning_maw_SpellScript);

            enum eSpells
            {
                Smouldering = 251445
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Smouldering, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_fharg_burning_maw_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fharg_burning_maw_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Smouldering - 251445
class spell_fharg_smouldering : public SpellScriptLoader
{
    public:
        spell_fharg_smouldering() : SpellScriptLoader("spell_fharg_smouldering") { }

        class spell_fharg_smouldering_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fharg_smouldering_AuraScript);

            enum eSpells
            {
                SmoulderingDamage = 254384
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                int32 l_Damage = p_AuraEffect->GetAmount();

                l_Caster->CastCustomSpell(l_Target, eSpells::SmoulderingDamage, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fharg_smouldering_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fharg_smouldering_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Corrupting Maw - 254760
class spell_shatug_corrupting_maw : public SpellScriptLoader
{
    public:
        spell_shatug_corrupting_maw() : SpellScriptLoader("spell_shatug_corrupting_maw") { }

        class spell_shatug_corrupting_maw_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shatug_corrupting_maw_SpellScript);

            enum eSpells
            {
                Decay = 245098
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Decay, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_shatug_corrupting_maw_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shatug_corrupting_maw_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Smouldering - 245098
class spell_shatug_decay : public SpellScriptLoader
{
    public:
        spell_shatug_decay() : SpellScriptLoader("spell_shatug_decay") { }

        class spell_shatug_decay_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shatug_decay_AuraScript);

            enum eSpells
            {
                DecayDamage = 245383
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                int32 l_Damage = p_AuraEffect->GetAmount();

                l_Caster->CastCustomSpell(l_Target, eSpells::DecayDamage, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_shatug_decay_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shatug_decay_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Consuming Sphere - 10661 - 10586 - (244107)
class at_shatug_consuming_sphere : public AreaTriggerEntityScript
{
    public:
        at_shatug_consuming_sphere() : AreaTriggerEntityScript("at_shatug_consuming_sphere") { }

        int32 p_MovementForceUpdateTime = 500;

        struct MovementForceData
        {
            MovementForceData(float p_Range, float p_Amplitude) :
                Range(p_Range), Magnitude(p_Amplitude)
            {
            }

            float const Range;
            float const Magnitude;

            Position Dest;
        };

        std::vector<MovementForceData> m_MovementForces;
        std::map<uint64, float> m_AffectedPlayers;
        uint64 m_SourceGUID;

        void OnCreate(AreaTrigger* p_Areatrigger) override
        {
            p_Areatrigger->MoveAreaTrigger(p_Areatrigger->GetDestination(), p_Areatrigger->getMoveSpeed(), true, true);

            m_SourceGUID = MAKE_NEW_GUID(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 10661, HIGHGUID_AREATRIGGER);

            if (p_Areatrigger->GetMap()->GetDifficultyID() == Difficulty::DifficultyMythic)
                m_MovementForces.push_back(MovementForceData(8.0f, 3.0f));

            m_MovementForces.push_back(MovementForceData(20.0f, 1.75f));
            m_MovementForces.push_back(MovementForceData(40.0f, 1.75f));
            m_MovementForces.push_back(MovementForceData(200.0f, 1.5f));
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (p_AreaTrigger->GetDestination().GetExactDist2d(p_AreaTrigger) < 1.0f)
            {
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                p_AreaTrigger->SetDuration(0);
            }

            p_MovementForceUpdateTime -= p_Time;
            if (p_MovementForceUpdateTime >= 0)
                return;

            p_MovementForceUpdateTime = 500;

            float l_Range = 200.0f;
            std::list<Player*> l_Players;
            JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Range, false);
            JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_Players, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Range, l_Searcher);

            std::map<uint64, float> l_NewAffectedPlayers;
            for (Player* l_Player : l_Players)
            {
                if (l_Player->isGameMaster())
                    continue;

                l_NewAffectedPlayers.insert({ l_Player->GetGUID(), 0 });
                for (MovementForceData& l_MovementForce : m_MovementForces)
                {
                    if (l_Player->GetDistance(p_AreaTrigger) < l_MovementForce.Range)
                        l_NewAffectedPlayers[l_Player->GetGUID()] += l_MovementForce.Magnitude;
                }

                if (m_AffectedPlayers.find(l_Player->GetGUID()) == m_AffectedPlayers.end())
                    l_Player->SendApplyMovementForce(m_SourceGUID, true, *p_AreaTrigger, l_NewAffectedPlayers[l_Player->GetGUID()], 1);
            }

            for (std::pair<uint64, float> l_AffectedPlayer : m_AffectedPlayers)
            {
                auto l_Itr = l_NewAffectedPlayers.find(l_AffectedPlayer.first);
                if (l_Itr == l_NewAffectedPlayers.end())
                {
                    Player* l_Player = sObjectAccessor->GetPlayer(*p_AreaTrigger, l_AffectedPlayer.first);
                    if (!l_Player)
                        continue;

                    l_Player->SendApplyMovementForce(m_SourceGUID, false);
                    continue;
                }

                Player* l_Player = sObjectAccessor->GetPlayer(*p_AreaTrigger, l_Itr->first);
                if (!l_Player)
                    continue;

                l_Player->SendApplyMovementForce(m_SourceGUID, false);
                l_Player->SendApplyMovementForce(m_SourceGUID, true, *p_AreaTrigger, l_Itr->second, 1);
            }

            m_AffectedPlayers = l_NewAffectedPlayers;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            for (std::pair<uint64, float> l_AffectedPlayer : m_AffectedPlayers)
            {
                Player* l_Player = sObjectAccessor->GetPlayer(*p_AreaTrigger, l_AffectedPlayer.first);
                if (!l_Player)
                    continue;

                l_Player->SendApplyMovementForce(m_SourceGUID, false);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_shatug_consuming_sphere();
        }
};

/// Last Update 7.3.5 Build 26365
/// Siphoned - 248819
class spell_shatug_siphoned : public SpellScriptLoader
{
    public:
        spell_shatug_siphoned() : SpellScriptLoader("spell_shatug_siphoned") { }

        class spell_shatug_siphoned_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shatug_siphoned_AuraScript);

            enum eSpells
            {
                Siphoned = 244583
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Siphoned, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_shatug_siphoned_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shatug_siphoned_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Desolate Gaze - 244441
class spell_fharg_desolate_gaze : public SpellScriptLoader
{
    public:
        spell_fharg_desolate_gaze() : SpellScriptLoader("spell_fharg_desolate_gaze") { }

        class spell_fharg_desolate_gaze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fharg_desolate_gaze_SpellScript);

            enum eSpells
            {
                DesolateGazeVisual = 244768
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DesolateGazeVisual, true);
                Aura* l_Aura = l_Target->GetAura(eSpells::DesolateGazeVisual);
                if (!l_Aura)
                    return;

                l_Aura->SetDuration(8 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fharg_desolate_gaze_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fharg_desolate_gaze_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Desolate Gaze Damage - 244768
class spell_fharg_desolate_gaze_aura : public SpellScriptLoader
{
    public:
        spell_fharg_desolate_gaze_aura() : SpellScriptLoader("spell_fharg_desolate_gaze_aura") { }

        class spell_fharg_desolate_gaze_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fharg_desolate_gaze_aura_AuraScript);

            enum eSpells
            {
                DesolatePath            = 244833,
                DesolatePathAreatrigger = 244767
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                uint64 l_TargetGuid = l_Target->GetGUID();
                Position l_Dest;
                l_Caster->SimplePosXYRelocationByAngle(l_Dest, 200.0f, l_Caster->GetAngle(l_Target), true);
                l_Caster->CastSpell(l_Dest, eSpells::DesolatePath, true);
                l_Caster->CastSpell(l_Target, eSpells::DesolatePathAreatrigger, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_fharg_desolate_gaze_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fharg_desolate_gaze_aura_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Enflamed - 248815
class spell_fharg_enflamed : public SpellScriptLoader
{
    public:
        spell_fharg_enflamed() : SpellScriptLoader("spell_fharg_enflamed") { }

        class spell_fharg_enflamed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fharg_enflamed_AuraScript);

            enum eSpells
            {
                Enflamed = 244473
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Enflamed, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_fharg_enflamed_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fharg_enflamed_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Weight Of Darkness - 244069
class spell_shatug_weight_of_darkness : public SpellScriptLoader
{
    public:
        spell_shatug_weight_of_darkness() : SpellScriptLoader("spell_shatug_weight_of_darkness") { }

        class spell_shatug_weight_of_darkness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shatug_weight_of_darkness_SpellScript);

            enum eSpells
            {
                WeightOfDarknessAreatrigger = 254429
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::WeightOfDarknessAreatrigger, true);
                Aura* l_Aura = l_Target->GetAura(eSpells::WeightOfDarknessAreatrigger);
                if (!l_Aura)
                    return;

                l_Aura->SetDuration(6 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_shatug_weight_of_darkness_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shatug_weight_of_darkness_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Weight Of Darkness (Aura) - 254429
class spell_shatug_weight_of_darkness_aura : public SpellScriptLoader
{
    public:
        spell_shatug_weight_of_darkness_aura() : SpellScriptLoader("spell_shatug_weight_of_darkness_aura") { }

        class spell_shatug_weight_of_darkness_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shatug_weight_of_darkness_aura_AuraScript);

            enum eSpells
            {
                WeightOfDarknessFear = 244071
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::WeightOfDarknessFear, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_shatug_weight_of_darkness_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shatug_weight_of_darkness_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Weight Of Darkness (Fear) - 244071
class spell_shatug_weight_of_darkness_fear : public SpellScriptLoader
{
    public:
        spell_shatug_weight_of_darkness_fear() : SpellScriptLoader("spell_shatug_weight_of_darkness_fear") { }

        class spell_shatug_weight_of_darkness_fear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shatug_weight_of_darkness_fear_SpellScript);

            void OnSelectTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_TargetCount = p_Targets.size();
                if (l_TargetCount >= 3)
                {
                    p_Targets.clear();
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightOfDarknessTargetCount) = l_TargetCount;
                    return;
                }

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightOfDarknessTargetCount) = l_TargetCount;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shatug_weight_of_darkness_fear_SpellScript::OnSelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shatug_weight_of_darkness_fear_SpellScript();
        }

        class spell_shatug_weight_of_darkness_fear_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shatug_weight_of_darkness_fear_AuraScript);

            void CalculateMaxDuration(int32& p_MaxDuration)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_SpellInfo || !l_Caster)
                    return;

                if (!l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightOfDarknessTargetCount))
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightOfDarknessTargetCount) = 1;

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightOfDarknessTargetCount) >= 3)
                    p_MaxDuration = 0;
                else
                    p_MaxDuration = l_SpellInfo->GetMaxDuration() / l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightOfDarknessTargetCount);
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_shatug_weight_of_darkness_fear_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shatug_weight_of_darkness_fear_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Molten Touch - 244072
class spell_fharg_molten_touch : public SpellScriptLoader
{
    public:
        spell_fharg_molten_touch() : SpellScriptLoader("spell_fharg_molten_touch") { }

        class spell_fharg_molten_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fharg_molten_touch_AuraScript);

            enum eSpells
            {
                MoltenTouchStrangulate  = 249241,
                MoltenTouchPeriodic     = 244086
            };

            void SendMonsterMove(Unit* p_Target)
            {
                Movement::MoveSplineInit l_Init(p_Target);
                Optional<Movement::MonsterSplineFilter> l_Filter;

                l_Init.SetFly();
                l_Init.SetSmooth();
                l_Init.SetUncompressed();

                l_Filter.emplace();

                l_Filter->FilterFlags = 2;
                l_Filter->BaseSpeed = 10.0f;
                l_Filter->AddedToStart = 1;

                uint32 l_Speed = 1100;
                for (uint32 l_Idx = 0; l_Idx < 8; l_Idx++)
                    l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(0, (l_Speed -= 100)));

                l_Init.SetMonsterSplineFilters(*l_Filter.get_ptr());

                Position l_Pos = *p_Target;
                for (uint8 l_IDx = 0; l_IDx < 9; ++l_IDx)
                    l_Init.Path().push_back(Position(l_Pos.m_positionX, l_Pos.m_positionY, (l_Pos.m_positionZ += 1.0f)).AsVector3());

                l_Init.Launch();
            }

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MoltenTouchStrangulate, true);
                uint64 l_CasterGUID = l_Caster->GetGUID();
                l_Target->AddDelayedEvent([l_Target, l_CasterGUID]() -> void
                {
                    Unit* l_Caster = sObjectAccessor->GetUnit(*l_Target, l_CasterGUID);
                    if (!l_Caster)
                        return;

                    Position l_Dest = *l_Caster;
                    l_Dest.m_positionZ += 10.0f;
                    l_Target->NearTeleportTo(l_Dest);
                }, 2 * TimeConstants::IN_MILLISECONDS);

                l_Caster->DelayedCastSpell(l_Target, eSpells::MoltenTouchPeriodic, true, 1500);

                l_Target->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);

                l_Target->AddDelayedEvent([l_Target]() -> void
                {
                    l_Target->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                }, 6 * TimeConstants::IN_MILLISECONDS);

                SendMonsterMove(l_Target);

                Aura* l_Aura = l_Target->GetAura(eSpells::MoltenTouchStrangulate);
                if (!l_Aura)
                    return;

                l_Aura->SetMaxDuration(GetAura()->GetMaxDuration());
                l_Aura->SetDuration(GetAura()->GetDuration());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_fharg_molten_touch_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_PACIFY_SILENCE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fharg_molten_touch_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Singed - 244086
class spell_fharg_molten_touch_periodic : public SpellScriptLoader
{
    public:
        spell_fharg_molten_touch_periodic() : SpellScriptLoader("spell_fharg_molten_touch_periodic") { }

        class spell_fharg_molten_touch_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fharg_molten_touch_periodic_AuraScript);

            enum eSpells
            {
                Singed    = 244091
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Singed, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fharg_molten_touch_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fharg_molten_touch_periodic_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Singed - 244091
class spell_fharg_singed : public SpellScriptLoader
{
    public:
        spell_fharg_singed() : SpellScriptLoader("spell_fharg_singed") { }

        class spell_fharg_singed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fharg_singed_AuraScript);

            enum eSpells
            {
                SingedDamage    = 245097,
                MoltenFlare     = 244160
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Target || !l_Aura)
                    return;

                int32 l_Damage = p_AuraEffect->GetAmount();
                l_Caster->CastCustomSpell(l_Target, eSpells::SingedDamage, &l_Damage, nullptr, nullptr, true);

                l_Caster->CastSpell(l_Caster, eSpells::MoltenFlare, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fharg_singed_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fharg_singed_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Molten Flare - 244160
class spell_fharg_molten_flare : public SpellScriptLoader
{
    public:
        spell_fharg_molten_flare() : SpellScriptLoader("spell_fharg_molten_flare") { }

        class spell_fharg_molten_flare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fharg_molten_flare_SpellScript);

            enum eSpells
            {
                MoltenFlareMissile  = 244162,
                MoltenFlareVisual   = 244169,
                Singed              = 244091
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer())
                        return true;

                    if (p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return p_Target->ToPlayer()->HasAura(eSpells::Singed);
                });

                if (!p_Targets.empty())
                    JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleAfterHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Fharg = l_Target->FindNearestCreature(eCreatures::BossFharg, 250.0f);
                if (!l_Fharg)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MoltenFlareMissile, true, nullptr, nullptr, l_Fharg->GetGUID());
                l_Caster->CastSpell(l_Target, eSpells::MoltenFlareVisual, true, nullptr, nullptr, l_Fharg->GetGUID());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fharg_molten_flare_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_fharg_molten_flare_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fharg_molten_flare_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Shadowtouched - 244055
class spell_shatug_shadowtouched : public SpellScriptLoader
{
    public:
        spell_shatug_shadowtouched() : SpellScriptLoader("spell_shatug_shadowtouched") { }

        class spell_shatug_shadowtouched_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shatug_shadowtouched_AuraScript);

            enum eSpells
            {
                ShadowscarVisual    = 245077,
                ShadowscarDamage    = 245100,
                ShadowscarJump      = 245151
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                return (l_DamageInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE);
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = l_Target->FindNearestPlayer(200.0f);
                if (!l_Player)
                    return;

                l_Target->CastSpell(l_Player, eSpells::ShadowscarVisual, true);
                l_Target->CastSpell(l_Player, eSpells::ShadowscarDamage, true);
                l_Player->CastSpell(l_Target, eSpells::ShadowscarJump, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_shatug_shadowtouched_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_shatug_shadowtouched_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shatug_shadowtouched_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Flametouched - 244054
class spell_shatug_flametouched : public SpellScriptLoader
{
    public:
        spell_shatug_flametouched() : SpellScriptLoader("spell_shatug_flametouched") { }

        class spell_shatug_flametouched_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shatug_flametouched_AuraScript);

            enum eSpells
            {
                ShadowscarDamage    = 245100,
                BurningFlash        = 245021
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                {
                    if (l_SpellInfo->Id == eSpells::ShadowscarDamage)
                        return false;
                }

                return (l_DamageInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_SHADOW);
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                /// It's Shatug that casts the fire spell, because Flametouched players should be around F'harg, this allow a longer travel time (Blizzlike)
                Creature* l_Shatug = l_Target->FindNearestCreature(eCreatures::BossShatug, 200.0f);
                if (!l_Shatug)
                    return;

                l_Shatug->CastSpell(l_Target, eSpells::BurningFlash, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_shatug_flametouched_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_shatug_flametouched_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shatug_flametouched_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Consuming Detonation - 244130
class spell_shatug_consuming_detonation : public SpellScriptLoader
{
    public:
        spell_shatug_consuming_detonation() : SpellScriptLoader("spell_shatug_consuming_detonation") { }

        class spell_shatug_consuming_detonation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shatug_consuming_detonation_SpellScript);

            void HandleAfterHit(SpellEffIndex /*p_EffectIndex*/)
            {
                WorldLocation const* l_Center = GetExplTargetDest();
                Unit* l_Target = GetHitUnit();
                if (!l_Center || !l_Target)
                    return;

                uint32 l_Damage = GetHitDamage();
                /// Scales a Pct between 100 (< 15y) and 10 (== 60y)
                l_Damage = CalculatePct(static_cast<float>(l_Damage), std::min((65.0f - l_Center->GetExactDist2d(l_Target)) * 2.0f, 100.0f));
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_shatug_consuming_detonation_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shatug_consuming_detonation_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_felhound_of_sargeras()
{
    new boss_felhound_shatug();
    new boss_felhound_fharg();

    new spell_shatug_corrupting_maw();
    new spell_shatug_decay();
    new at_shatug_consuming_sphere();
    new spell_shatug_siphoned();
    new spell_shatug_weight_of_darkness();
    new spell_shatug_weight_of_darkness_aura();
    new spell_shatug_weight_of_darkness_fear();
    new spell_shatug_shadowtouched();
    new spell_shatug_consuming_detonation();

    new spell_fharg_burning_maw();
    new spell_fharg_smouldering();
    new spell_fharg_desolate_gaze();
    new spell_fharg_desolate_gaze_aura();
    new spell_fharg_enflamed();
    new spell_fharg_molten_touch();
    new spell_fharg_molten_touch_periodic();
    new spell_fharg_singed();
    new spell_fharg_molten_flare();
    new spell_shatug_flametouched();
}
#endif
