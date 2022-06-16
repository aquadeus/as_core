////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "boss_tov_guarm.hpp"

enum eScriptData
{
    BreathHit = 1,
    AreatriggerTargetsRed,
    AreatriggerTargetsGreen,
    AreatriggerTargetsPurple
};

/// Guarm - 114323
class tov_boss_guarm : public CreatureScript
{
    public:
        tov_boss_guarm() : CreatureScript("tov_boss_guarm") { }

        struct tov_boss_guarmAI : public BossAI
        {
            tov_boss_guarmAI(Creature* p_Creature) : BossAI(p_Creature, eTovData::DataBossGuarm)
            {
                m_InstanceScript = p_Creature->GetInstanceScript();
                m_Intro = false;
            }

            enum eTimersOffSet
            {
                TimerOffsetRoaringLeap = 0,
                TimerOffsetFlashingFangs,
                TimerOffsetHeadlongCharge,
                TimerOffsetLicks,
                TimerOffsetGuardiansBreath,
                TimerOffsetVolatileFoam
            };

            enum eConversationSpell
            {
                ConversationSpellNearBoss = 0,
                ConversationSpellDeathOfBoss = 1
            };

            InstanceScript* m_InstanceScript;

            bool m_HeadlingCharge;

            bool m_BerserkCharge;

            bool m_Intro;

            bool m_StartRegen;
            bool m_CastingGuardiansBreath;
            bool m_CanCastGuardianBreath;

            uint8 m_BoneGuardiansHitCount;

            uint32 m_BerserkersRageCount;
            uint32 m_LicksChain;
            uint32 m_BreathRand;
            int32 m_HeadlongChargeDiff;
            int32 m_BerserkChargeDiff;
            int32 m_canCastBreathDiff;
            uint32 m_PowerTimer;
            uint32 m_PowerCount = 0;
            /// Roaring Leap, Flashing Fangs, Headlong Charge, Licks, Guardian's Breath, Volatile Foam
            uint32 m_TimersOffset[6] = { 0, 0, 0, 0, 0, 0 };

            uint64 m_BoneTargetGuid;

            uint32 m_RedTargetAmount;
            uint32 m_GreenTargetAmount;
            uint32 m_PurpleTargetAmount;

            std::list<uint64> m_PlayerGuidList;
            std::list<uint64> m_BreathAffected;

            std::list<uint32> m_BoneHittedColours;

            uint32 m_PlayersHitByBreath;

            void Reset() override
            {
                _Reset();

                events.Reset();

                ClearDelayedOperations();

                m_LicksChain = 0;
                m_BreathRand = 0;
                m_BoneTargetGuid = 0;
                m_BoneGuardiansHitCount = 0;

                m_HeadlongChargeDiff =  500;
                m_BerserkChargeDiff  = 500;
                m_canCastBreathDiff = 500;

                m_RedTargetAmount = 0;
                m_GreenTargetAmount = 0;
                m_PurpleTargetAmount = 0;

                m_StartRegen = false;
                m_HeadlingCharge = false;
                m_BerserkCharge  = false;
                m_CanCastGuardianBreath = true;

                me->SetCanFly(false);
                me->SetDisableGravity(false);

                m_CastingGuardiansBreath = 0;
                m_BerserkersRageCount = 0;

                m_PlayersHitByBreath = 0;

                m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                m_PowerCount = 0;

                m_PlayerGuidList.clear();
                m_BoneHittedColours.clear();

                m_BreathAffected.clear();

                DespawnCreaturesInArea(eTovCreatures::CreatureWorldTrigger, me);

                me->SetPower(me->getPowerType(), int32(0), false);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->RemoveAllAuras();

                me->SetSpeed(UnitMoveType::MOVE_RUN, 2.1f, true);

                SetCombatMovement(true);

                for (uint8 l_Itr = 0; l_Itr < 6; l_Itr++)
                {
                    m_TimersOffset[l_Itr] = 0;
                }

                if (m_InstanceScript != nullptr)
                {
                    for (uint32 l_Iter : g_SpellsToRemove)
                        m_InstanceScript->DoRemoveAurasDueToSpellOnPlayers(l_Iter);
                }

                me->ReenableEvadeMode();
            }

            void EnterCombat(Unit *p_Attacker) override
            {
                _EnterCombat();

                if (m_InstanceScript != nullptr)
                {
                    m_InstanceScript->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                me->CastSpell(me, eSpells::MultiHeadedAuraDummy);

                events.Reset();

                uint32 l_EnrageTimer = eTimers::TimerEnragedNM;

                if (IsMythic())
                {
                    l_EnrageTimer = eTimers::TimerEnragedMM;
                }

                if (IsHeroic())
                    l_EnrageTimer = eTimers::TimerEnragedHC;

                for (uint32 l_Itr = 0; l_Itr < 6; l_Itr++)
                {
                    m_TimersOffset[l_Itr] = 0;
                }

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                    m_PowerCount = 0;
                    m_StartRegen = true;
                });

                me->SetPower(me->getPowerType(), int32(33), false);

                events.ScheduleEvent(eEvents::EventOffTheLeash, eTimers::TimerOffTheLeash);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventVolatileFoam, eTimersOffsetVolatileFoam[m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam]++]);

                events.ScheduleEvent(eEvents::EventHeadlongCharge, eTimersOffsetHeadlingCharge[m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge]++]);
                events.ScheduleEvent(eEvents::EventFlashingFang, eTimersOffsetFlashingFangs[m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs]++]);
                events.ScheduleEvent(eEvents::EventRoaringLeap, eTimersOffsetRoaringLeap[m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap]++]);
                events.ScheduleEvent(eEvents::EventLick, eTimersOffsetLick[m_TimersOffset[eTimersOffSet::TimerOffsetLicks]++]);
                events.ScheduleEvent(eEvents::EventEnrage, l_EnrageTimer);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                ClearDelayedOperations();

                if (m_InstanceScript != nullptr)
                {
                    m_InstanceScript->SetBossState(eTovData::DataBossGuarm, EncounterState::FAIL);
                    m_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
                }

                SetCombatMovement(true);

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_InstanceScript != nullptr)
                {
                    m_InstanceScript->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    if (!IsLFR())
                    {
                        if (m_BoneHittedColours.size() >= 3) ///< Achiv completed.
                            m_InstanceScript->DoCompleteAchievement(eTovAchievements::BonefiedTriTip);
                    }
                }

                _JustDied();
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::GuardiansBreathVisualBreath1:
                    case eSpells::GuardiansBreathVisualBreath2:
                    case eSpells::GuardiansBreathVisualBreath3:
                    case eSpells::GuardiansBreathVisualBreath4:
                    case eSpells::GuardiansBreathVisualBreath5:
                    case eSpells::GuardiansBreathVisualBreath6:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            if (!instance || !instance->instance)
                                return;

                            int32 l_PlayerCount = instance->instance->GetPlayersCountExceptGMs();
                            int32 l_NotHitPlayers = l_PlayerCount - m_PlayersHitByBreath;
                            
                            if (l_NotHitPlayers > 0)
                            {
                                for (int32 l_I = 0; l_I < l_NotHitPlayers; ++l_I)
                                    me->CastSpell(me, eSpells::FrothingRage, true);
                            }

                            m_PlayersHitByBreath = 0;
                            me->SetPower(Powers::POWER_ENERGY, 0);
                        });

                        DoAction(ACTION_1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eScriptData::BreathHit:
                    {
                        m_PlayersHitByBreath++;
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetAffectedPlayerCount(GuidList* l_UnitList)
            {
                uint32 l_Count = 0;
                for (uint64 l_GUID : *l_UnitList)
                {
                    if (IS_PLAYER_GUID(l_GUID))
                        l_Count++;
                }

                return l_Count;
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::AreatriggerTargetsRed:
                    {
                        if (m_RedTargetAmount)
                            return m_RedTargetAmount;

                        AreaTrigger* l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtRedFront);
                        if (!l_Areatrigger)
                            l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtRedLeft);
                        if (!l_Areatrigger)
                            l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtRedRight);

                        m_RedTargetAmount = GetAffectedPlayerCount(l_Areatrigger->GetAffectedPlayers());
                        return m_RedTargetAmount;
                    }
                    case eScriptData::AreatriggerTargetsGreen:
                    {
                        if (m_GreenTargetAmount)
                            return m_GreenTargetAmount;

                        AreaTrigger* l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtGreenFront);
                        if (!l_Areatrigger)
                            l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtGreenLeft);
                        if (!l_Areatrigger)
                            l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtGreenRight);

                        m_GreenTargetAmount = GetAffectedPlayerCount(l_Areatrigger->GetAffectedPlayers());
                        return m_GreenTargetAmount;
                    }
                    case eScriptData::AreatriggerTargetsPurple:
                    {
                        if (m_PurpleTargetAmount)
                            return m_PurpleTargetAmount;

                        AreaTrigger* l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtPurpleFront);
                        if (!l_Areatrigger)
                            l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtPurpleLeft);
                        if (!l_Areatrigger)
                            l_Areatrigger = me->GetAreaTrigger(eSpells::GuardiansBreathCreateAtPurpleRight);

                        m_PurpleTargetAmount = GetAffectedPlayerCount(l_Areatrigger->GetAffectedPlayers());
                        return m_PurpleTargetAmount;
                    }
                    default:
                        break;
                }

                return 0;
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMovementInformed::GuremRoaringLeapHit:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        break;
                    }
                    case eMovementInformed::HeadlongChargeStart:
                    {
                        m_HeadlingCharge = true;
                        m_HeadlongChargeDiff = 500;

                        me->DisableEvadeMode();

                        if (m_InstanceScript != nullptr)
                        {
                            if (Creature* l_GuarmChewToy = Creature::GetCreature(*me, m_InstanceScript->GetData64(eTovCreatures::CreatureGuarmChewToy)))
                            {
                                l_GuarmChewToy->SetDisplayId(60327);

                                l_GuarmChewToy->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                                const Position l_Behind = {
                                    me->m_positionX - 8.f * std::cos(me->m_orientation),  me->m_positionY - 8.f * std::sin(me->m_orientation),
                                    me->m_positionZ,  me->m_orientation
                                };

                                l_GuarmChewToy->NearTeleportTo(l_Behind, false);
                                l_GuarmChewToy->Respawn(true, false, 1 * TimeConstants::IN_MILLISECONDS);
                            }
                        }

                        me->SetSpeed(UnitMoveType::MOVE_RUN, 5.7f, true);

                        uint8 l_MoveId = 0;

                        if (me->m_positionZ >= 5.0f)
                            l_MoveId = 0;
                        else
                            l_MoveId = 1;

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_MoveId]() -> void
                        {
                            me->CastSpell(me, eSpells::HeadlongChargeTriggerSpel, true);

                            if (l_MoveId == 0)
                                me->SetFacingTo(1.573f);
                            else
                                me->SetFacingTo(4.573f);

                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveSmoothPath(eMovementInformed::HeadlingChargeEnd, g_HeadlongCharge[l_MoveId].data(), g_HeadlongCharge[l_MoveId].size(), false);
                        });;

                        break;
                    }
                    case eMovementInformed::HeadlingChargeEnd:
                    {
                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                            m_StartRegen = true;
                        });

                        m_CanCastGuardianBreath = true;
                        m_canCastBreathDiff = 500;

                        me->GetMotionMaster()->Clear(false);

                        me->ReenableEvadeMode();

                        SetCombatMovement(true);

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        m_HeadlingCharge = false;
                        m_HeadlongChargeDiff = 500;

                        me->RemoveAura(eSpells::HeadlongChargeTriggerSpel);

                        me->SetSpeed(UnitMoveType::MOVE_RUN, 2.1f, true);

                        events.CancelEvent(eEvents::EventVolatileFoam);
                        events.CancelEvent(eEvents::EventHeadlongCharge);
                        events.CancelEvent(eEvents::EventFlashingFang);
                        events.CancelEvent(eEvents::EventGuardianBreath);
                        events.CancelEvent(eEvents::EventRoaringLeap);
                        events.CancelEvent(eEvents::EventLick);

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs] >= 2)
                            m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs] = 0;

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam] >= 3)
                            m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam] = 0;

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetLicks] >= 3)
                            m_TimersOffset[eTimersOffSet::TimerOffsetLicks] = 0;

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap] >= 2)
                            m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap] = 0;

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge] > 2)
                            m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge] = 0;

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetGuardiansBreath] >= 2)
                            m_TimersOffset[eTimersOffSet::TimerOffsetGuardiansBreath] = 0;

                        if (m_LicksChain > 2)
                            m_LicksChain = 0;

                        events.CancelEvent(eEvents::EventHeadlongCharge);
                        events.CancelEvent(eEvents::EventFlashingFang);
                        events.CancelEvent(eEvents::EventRoaringLeap);
                        events.CancelEvent(eEvents::EventLick);
                        events.CancelEvent(eEvents::EventVolatileFoam);

                        events.ScheduleEvent(eEvents::EventHeadlongCharge, eTimersOffsetHeadlingCharge[m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge]++]);
                        events.ScheduleEvent(eEvents::EventFlashingFang, eTimersOffsetFlashingFangs[m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs]++]);
                        events.ScheduleEvent(eEvents::EventRoaringLeap, eTimersOffsetRoaringLeap[m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap]++]);
                        events.ScheduleEvent(eEvents::EventLick, eTimersOffsetLick[m_TimersOffset[eTimersOffSet::TimerOffsetLicks]++]);
                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventVolatileFoam, eTimersOffsetLick[m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam]++]);

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.0f, true))
                            AttackStart(l_Target, true);

                        break;
                    }
                    case eMovementInformed::BerserkChargeStart:
                    {
                        if (me->GetReactState() != ReactStates::REACT_PASSIVE)
                            me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->CastSpell(me, eSpells::BerserkCharge, true);

                        me->SetSpeed(UnitMoveType::MOVE_RUN, 6.0f, true);

                        m_BerserkCharge = true;
                        m_BerserkChargeDiff = 1000;

                        me->CastSpell(me, eSpells::BerserkCharge, true);

                        me->SetSpeed(UnitMoveType::MOVE_RUN, 6.0f, true);

                        AddTimedDelayedOperation(200, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveCharge(&g_BereserkChargeDest, 64.0f, eMovementInformed::BerserkChargeEnd);
                        });

                        AddTimedDelayedOperation(3000, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveCharge(&g_BereserkChargeSrc, 64.0f, eMovementInformed::BerserkChargeEnd);
                        });

                        AddTimedDelayedOperation(6000, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveCharge(&g_BereserkChargeDest, 64.0f, eMovementInformed::BerserkChargeEnd);
                        });

                        AddTimedDelayedOperation(9000, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveCharge(&g_BereserkChargeSrc, 64.0f, eMovementInformed::BerserkChargeEnd);
                        });

                        AddTimedDelayedOperation(12000, [this]() -> void
                        {
                            me->RemoveAura(eSpells::BerserkCharge);

                            m_BerserkCharge = false;
                            m_BerserkChargeDiff = 1000;

                            me->CombatStop();

                            me->ReenableEvadeMode();

                            EnterEvadeMode();
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                p_Value = 0;
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    me->CastSpell(p_Victim, eSpells::MultiHeadedDmg, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::RoaringLeapKnockBack:
                    {
                        DoCast(eSpells::OffTheLeash, true);

                        break;
                    }
                    case eSpells::OffTheLeash:
                    {
                        if (m_InstanceScript != nullptr)
                        {
                            if (Creature* l_GuarmChewToy = Creature::GetCreature(*me, m_InstanceScript->GetData64(eTovCreatures::CreatureGuarmChewToy)))
                            {
                                if (m_InstanceScript != nullptr)
                                {
                                    if (Creature* l_GuarmChewToy = Creature::GetCreature(*me, m_InstanceScript->GetData64(eTovCreatures::CreatureGuarmChewToy)))
                                    {
                                        l_GuarmChewToy->SetDisplayId(60327);

                                        l_GuarmChewToy->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                                        Position const l_Behind = {
                                            me->m_positionX - 8.f * std::cos(me->m_orientation),  me->m_positionY - 8.f * std::sin(me->m_orientation),
                                            me->m_positionZ,  me->m_orientation
                                        };

                                        l_GuarmChewToy->NearTeleportTo(l_Behind, false);
                                        l_GuarmChewToy->Respawn(true, false, 1 * TimeConstants::IN_MILLISECONDS);
                                    }
                                }

                                l_GuarmChewToy->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 1) ///< Done casting Breath
                {
                    m_CanCastGuardianBreath = true;
                    m_CastingGuardiansBreath = false;
                    m_canCastBreathDiff = 500;
                }
            }

            void CheckForBone(uint32 const p_Diff)
            {
                if (m_BoneTargetGuid)
                {
                    if (Player* l_Plr = Player::GetPlayer(*me, m_BoneTargetGuid))
                    {
                        if (!l_Plr->HasAura(eSpells::GuarmChewToy))
                        {
                            m_BoneTargetGuid = 0;

                            return;
                        }

                        if (l_Plr->GetDistance2d(me) <= 2.0f)
                        {
                            m_BoneTargetGuid = 0;
                            l_Plr->RemoveAura(eSpells::GuarmChewToy);

                            return;
                        }
                    }
                }
            }

            void PowerRegenTimer(uint32 const p_Diff)
            {
                if (m_StartRegen)
                {
                    if (m_PowerTimer <= p_Diff)
                    {
                        m_PowerTimer = 1000;
                        m_PowerCount = me->GetPower(POWER_ENERGY);

                        int32 l_PowerToAdd = 5;

                        if (m_PowerCount < 100)
                            me->SetPower(POWER_ENERGY, m_PowerCount + l_PowerToAdd);
                    }
                    else
                        m_PowerTimer -= p_Diff;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                CheckHomeDistToEvade(p_Diff, 58.0f);

                PowerRegenTimer(p_Diff);

                if (m_CanCastGuardianBreath)
                {
                    if (m_canCastBreathDiff <= p_Diff)
                    {
                        if (me->GetPower(me->getPowerType()) >= 99)
                        {
                            m_CanCastGuardianBreath = false;

                            events.ScheduleEvent(eEvents::EventGuardianBreath, 500);
                        }

                        m_canCastBreathDiff = 500;
                    }
                    else
                        m_canCastBreathDiff -= p_Diff;
                }

                if (m_HeadlingCharge)
                {
                    if (m_HeadlongChargeDiff <= p_Diff)
                    {
                        if (!me->HasAura(eSpells::HeadlongChargeTriggerSpel))
                            me->AddAura(eSpells::HeadlongChargeTriggerSpel, me);

                        std::list<Player*> l_ListPlayersNerby;
                        me->GetPlayerListInGrid(l_ListPlayersNerby, 3.0f);

                        for (Player* l_Plr : l_ListPlayersNerby)
                        {
                            if (!m_HeadlingCharge)
                                continue;

                            if (me->isInFront(l_Plr))
                                l_Plr->CastSpell(l_Plr, eSpells::Trample, true);
                        }

                        m_HeadlongChargeDiff = 400;
                    }
                    else
                        m_HeadlongChargeDiff -= p_Diff;
                }

                if (m_BerserkCharge)
                {
                    if (m_BerserkChargeDiff <= p_Diff)
                    {
                        if (me->HasAura(eSpells::BerserkCharge))
                        {
                            std::list<Player*> l_ListPlayersNerby;
                            me->GetPlayerListInGrid(l_ListPlayersNerby, 10.f);

                            for (Player* l_Plr : l_ListPlayersNerby)
                            {
                                l_Plr->CastSpell(l_Plr, 232197, true);
                            }
                        }

                        m_BerserkChargeDiff = 500;
                    }
                    else
                        m_BerserkChargeDiff -= p_Diff;
                }

                CheckForBone(p_Diff);

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventOffTheLeash:
                    {
                        me->CastSpell(me, eSpells::OffTheLeash);

                        if (m_InstanceScript != nullptr)
                        {
                            if (Creature* l_GuarmChewToy = Creature::GetCreature(*me, m_InstanceScript->GetData64(eTovCreatures::CreatureGuarmChewToy)))
                            {
                                l_GuarmChewToy->SetDisplayId(60327);

                                l_GuarmChewToy->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                                const Position l_Behind = {
                                    me->m_positionX - 8.f * std::cos(me->m_orientation),  me->m_positionY - 8.f * std::sin(me->m_orientation),
                                    me->m_positionZ,  me->m_orientation
                                };

                                l_GuarmChewToy->NearTeleportTo(l_Behind, false);
                                l_GuarmChewToy->Respawn(true, false, 1 * TimeConstants::IN_MILLISECONDS);
                            }
                        }

                        events.ScheduleEvent(eEvents::EventOffTheLeash, eTimers::TimerOffTheLeash);
                        break;
                    }
                    case eEvents::EventGuardianBreath:
                    {
                        m_RedTargetAmount = 0;
                        m_GreenTargetAmount = 0;
                        m_PurpleTargetAmount = 0;

                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        me->SetFacingToObject(me->getVictim());
                        me->NearTeleportTo(*me);
                        DoCastAOE(eSpells::GuardiansBreath, false);

                        events.DelayEvent(eEvents::EventFlashingFang, 2 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHeadlongCharge:
                    {
                        m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                        m_StartRegen = false;

                        m_CanCastGuardianBreath = false;
                        m_canCastBreathDiff = 500;

                        events.DelayEvent(eEvents::EventFlashingFang, 10 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventOffTheLeash, 10 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventLick, 10 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventRoaringLeap, 10 * TimeConstants::IN_MILLISECONDS);

                        if (IsMythic())
                            events.DelayEvent(eEvents::EventVolatileFoam, 10 * TimeConstants::IN_MILLISECONDS);

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            events.CancelEvent(eEvents::EventHeadlongCharge);
                            events.CancelEvent(eEvents::EventFlashingFang);
                            events.CancelEvent(eEvents::EventGuardianBreath);
                            events.CancelEvent(eEvents::EventRoaringLeap);
                            events.CancelEvent(eEvents::EventLick);

                            me->StopAttack();

                            me->StopMoving();

                            SetCombatMovement(false);

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            me->CastSpell(me, eSpells::HeadlongChargeEffDummy);

                            Position l_SpawnPos;

                            if (roll_chance_i(50))
                                l_SpawnPos = { 481.186f, 443.651f, 5.057f, 1.661911f };
                            else
                                l_SpawnPos = { 452.9309f, 559.1723f, 0.9460f, 4.67785f };

                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveJump(l_SpawnPos.m_positionX, l_SpawnPos.m_positionY, l_SpawnPos.m_positionZ, 35.0f, 24.0f, l_SpawnPos.m_orientation, eMovementInformed::HeadlongChargeStart);

                            if (m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge] > 2)
                                m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge] = 0;

                            events.ScheduleEvent(eEvents::EventHeadlongCharge, eTimersOffsetHeadlingCharge[m_TimersOffset[eTimersOffSet::TimerOffsetHeadlongCharge]++]);
                        });

                        break;
                    }
                    case eEvents::EventRoaringLeap:
                    {
                        m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                        m_StartRegen = false;

                        DoCast(eSpells::RoaringLeapKnockBack, true);

                        events.DelayEvent(eEvents::EventFlashingFang, 5 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventOffTheLeash, 5 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventHeadlongCharge, 5 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventLick, 5 * TimeConstants::IN_MILLISECONDS);

                        if (IsMythic())
                            events.DelayEvent(eEvents::EventVolatileFoam, 10 * TimeConstants::IN_MILLISECONDS);

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            /// 2nd part - choosing most crowded group of players;
                            uint64 l_TargetGuid = 0;

                            uint32 l_Numb = 0;

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            std::list<Player*> l_ListPlayerTempOv;

                            std::list<Player*> l_ListPlayer;
                            me->GetPlayerListInGrid(l_ListPlayer, 100.0f, false);

                            for (Player* l_Itr : l_ListPlayer)
                            {
                                std::list<Player*> l_ListPlayerTemp;

                                l_Itr->GetPlayerListInGrid(l_ListPlayerTemp, 10.0f, false);

                                if (l_ListPlayerTemp.empty() || (l_Numb && l_ListPlayerTemp.size() < l_Numb))
                                    continue;

                                l_Numb = l_ListPlayerTemp.size();

                                l_TargetGuid = l_Itr->GetGUID();
                            }

                            if (l_TargetGuid)
                            {
                                if (Player* l_Target = Player::GetPlayer(*me, l_TargetGuid))
                                    me->CastSpell(l_Target, eSpells::RoaringLeapJumpDest, true);
                            }
                        });

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap] >= 2)
                            m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap] = 0;

                        events.ScheduleEvent(eEvents::EventRoaringLeap, eTimersOffsetRoaringLeap[m_TimersOffset[eTimersOffSet::TimerOffsetRoaringLeap]++]);

                        AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                            m_StartRegen = true;
                        });

                        break;
                    }
                    case eEvents::EventLick:
                    {
                        std::list<Player*> l_ListPlayersToGetAffected;
                        me->GetPlayerListInGrid(l_ListPlayersToGetAffected, 150.0f, false);

                        l_ListPlayersToGetAffected.remove_if([this](Player* p_Player) -> bool
                        {
                            if (p_Player == nullptr || p_Player->isDead()) ///< Arcane Beacon
                                return true;

                            for (uint8 l_I = 0; l_I < 3; l_I++)
                            {
                                if (p_Player->HasAura(g_LickAuras[l_I]))
                                    return true;
                            }

                            return false;
                        });

                        if (!l_ListPlayersToGetAffected.empty())
                        {
                            std::list<Player*> l_RangedPlayers = l_ListPlayersToGetAffected;
                            l_RangedPlayers.remove_if([this](Player* p_Player) -> bool
                            {
                                if (!p_Player || (!p_Player->IsRangedDamageDealer() && p_Player->GetRoleForGroup() != Roles::ROLE_HEALER))
                                    return true;

                                return false;
                            });

                            if (!l_RangedPlayers.empty())
                                l_ListPlayersToGetAffected = l_RangedPlayers;

                            JadeCore::RandomResizeList(l_ListPlayersToGetAffected, 1);

                            std::list<Player*>::const_iterator l_Iter = l_ListPlayersToGetAffected.begin();

                            uint32 l_Entry = g_LickSpellsEntriesVector[m_LicksChain++];

                            if ((*l_Iter) != nullptr)
                                me->CastSpell(*(*l_Iter), l_Entry, true);

                            uint64 l_Guid = (*l_Iter)->GetGUID();

                            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this, l_Guid, l_Entry]() -> void
                            {
                                if (!l_Guid)
                                    return;

                                if (const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Entry))
                                {
                                    if (uint32 l_TriggerSpell = l_SpellInfo->Effects[EFFECT_0].TriggerSpell)
                                    {
                                        if (Player* l_Plr = me->GetPlayer(*me, l_Guid))
                                            me->CastSpell(l_Plr, l_TriggerSpell, true);
                                    }
                                }
                            });
                        }

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetLicks] >= 3)
                            m_TimersOffset[eTimersOffSet::TimerOffsetLicks] = 0;

                        if (m_LicksChain > 2)
                            m_LicksChain = 0;

                        events.ScheduleEvent(eEvents::EventLick, eTimersOffsetLick[m_TimersOffset[eTimersOffSet::TimerOffsetLicks]++]);

                        break;
                    }
                    case eEvents::EventVolatileFoam:
                    {
                        std::list<Player*> l_ListPlayersToGetAffected;
                        me->GetPlayerListInGrid(l_ListPlayersToGetAffected, 150.0f, false);

                        l_ListPlayersToGetAffected.remove_if([this](Player* p_Player) -> bool
                        {
                            if (p_Player == nullptr || p_Player->isDead()) ///< Arcane Beacon
                                return true;

                            for (uint8 l_I = 0; l_I < 3; l_I++)
                            {
                                if (p_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                                    return true;

                                if (p_Player->HasAura(g_VolatileFoamSpellsFullDurationVector[l_I]))
                                    return true;
                            }

                            return false;
                        });

                        if (!l_ListPlayersToGetAffected.empty())
                        {
                            uint32 l_PlayersToAffect = std::min<uint32>(l_ListPlayersToGetAffected.size(), 3);
                            JadeCore::RandomResizeList(l_ListPlayersToGetAffected, l_PlayersToAffect);

                            uint32 l_MaxCount = urand(1, l_PlayersToAffect);

                            std::list<Player*>::const_iterator l_Iter = l_ListPlayersToGetAffected.begin();

                            for (uint8 l_I = 0; l_I < l_MaxCount; l_I++)
                            {
                                uint32 l_SpellId = g_VolatileFoamSpellsFullDurationVector[l_I];

                                if (l_Iter == l_ListPlayersToGetAffected.end())
                                    break;

                                if ((*l_Iter) != nullptr)
                                    me->CastSpell((*l_Iter), l_SpellId);

                                ++l_Iter;
                            }
                        }

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam] > 3)
                            m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam] = 0;

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventVolatileFoam, eTimersOffsetVolatileFoam[m_TimersOffset[eTimersOffSet::TimerOffsetVolatileFoam]++]);

                        break;
                    }
                    case eEvents::EventFlashingFang:
                    {
                        if(Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.f, true))
                          me->CastSpell(l_Target, eSpells::FlashingFangsDmg, false);

                        if (m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs] >= 3)
                            m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs] = 1;

                        events.ScheduleEvent(eEvents::EventFlashingFang, eTimersOffsetFlashingFangs[m_TimersOffset[eTimersOffSet::TimerOffsetFlashingFangs]++]);
                        events.DelayEvent(eEvents::EventGuardianBreath, 2 * IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventEnrage:
                    {
                        events.CancelEvent(eEvents::EventVolatileFoam);
                        events.CancelEvent(eEvents::EventHeadlongCharge);
                        events.CancelEvent(eEvents::EventFlashingFang);
                        events.CancelEvent(eEvents::EventGuardianBreath);
                        events.CancelEvent(eEvents::EventRoaringLeap);
                        events.CancelEvent(eEvents::EventLick);

                        if (!m_CanCastGuardianBreath)
                        {
                            events.ScheduleEvent(eEvents::EventEnrage, 1 * IN_MILLISECONDS);
                            break;
                        }

                        m_StartRegen = false;
                        m_BerserkCharge = false;
                        m_BerserkChargeDiff = 1 * TimeConstants::IN_MILLISECONDS;

                        me->RemoveAllAuras();

                        me->SetPower(me->getPowerType(), int32(0), false);

                        ClearDelayedOperations();

                        SetCombatMovement(false);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->StopAttack();

                        me->StopMoving();

                        me->CombatStop();

                        me->DisableEvadeMode();

                        me->CastStop();

                        me->CastSpell(me, eSpells::Enrage);

                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveJump(g_BereserkChargeSrc.m_positionX, g_BereserkChargeSrc.m_positionY, g_BereserkChargeSrc.m_positionZ, 35.0f, 14.0f, g_BereserkChargeSrc.m_orientation, eMovementInformed::BerserkChargeStart);

                        break;
                    }
                    default:
                        break;
                }

                if (!m_HeadlingCharge || !m_CastingGuardiansBreath)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new tov_boss_guarmAI(p_Creature);
        }
};

/// Flaming Volatile Foam (long cycle)  - 228744
/// Briney Volatile Foam  (long cycle)  - 228810
/// Shadowy Volatile Foam (long cycle)  - 228818
/// Flaming Volatile Foam (short cycle) - 228794
/// Briney Volatile Foam  (short cycle) - 228811
/// Shadowy Volatile Foam (short cycle) - 228819
class spell_tov_odyn_volatile_foam : public SpellScriptLoader
{
    public:
        spell_tov_odyn_volatile_foam() : SpellScriptLoader("spell_tov_odyn_volatile_foam") { }

        class spell_tov_odyn_volatile_foam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_odyn_volatile_foam_AuraScript);

            enum eSpells
            {
                SaltySpittleAuraDummy         = 228768,
                DarkDischargeAuraDummy        = 228769,
                FieryPhlegmAuraDummy          = 228758,
                FlamingVolatileFoamMultiEff01 = 228744,
                FlamingVolatileFoamMultiEff02 = 228794,
                BrineyVolatileFoamMultiEff01  = 228810,
                BrineyVolatileFoamMultiEff02  = 228811,
                ShadowyVolatileFoamMultiEff01 = 228818,
                ShadowyVolatileFoamMultiEff02 = 228819
            };

            uint64 m_TargetGUID = 0;

            void HandleOnDispel(DispelInfo* /*p_DispelInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(GetAura()->GetId(), l_Caster->GetMap()->GetDifficultyID());
                if (!l_Caster || !l_SpellInfo)
                    return;

                std::list<AuraApplication*> l_Applications;
                l_Aura->GetApplicationList(l_Applications);
                if (l_Applications.size() != 1)
                    return;

                AuraApplication* l_AuraApp = l_Applications.front();
                if (!l_AuraApp)
                    return;

                Unit* l_Target = l_AuraApp->GetTarget();
                if (!l_Target)
                    return;

                std::list<Player*> l_ListPlayersToGetAffected;
                l_Caster->GetPlayerListInGrid(l_ListPlayersToGetAffected, 150.0f, false);

                l_ListPlayersToGetAffected.remove_if([this, l_Target, l_Caster](Player* p_Player) -> bool
                {
                    if (p_Player == nullptr || p_Player->isDead() || !l_Caster->IsValidAttackTarget(p_Player)) ///< Arcane Beacon
                        return true;

                    for (uint8 l_I = 0; l_I < 3; l_I++)
                    {
                        if (p_Player == l_Target)
                            return true;

                        if (p_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                            return true;

                        if (p_Player->HasAura(g_VolatileFoamSpellsFullDurationVector[l_I]))
                            return true;

                        if (p_Player->HasAura(g_VolatileFoamSpellsShortDurationVector[l_I]))
                            return true;
                    }

                    return false;
                });

                if (!l_ListPlayersToGetAffected.empty())
                {
                    l_ListPlayersToGetAffected.sort(JadeCore::UnitSortDistance(true, l_Caster));

                    if (Player* l_NearestPlayer = l_ListPlayersToGetAffected.front())
                        m_TargetGUID = l_NearestPlayer->GetGUID();
                }
                else
                    PreventDefaultAction();
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes p_Modes)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        if (GetAura() == nullptr)
                            return;

                        /// Gob  vis aura
                        const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(GetAura()->GetId(), l_Caster->GetMap()->GetDifficultyID());

                        if (l_SpellInfo == nullptr)
                            return;

                        uint32 l_SpellId = 0;
                        uint32 l_BreathDotId = 0;

                        switch (GetSpellInfo()->Id)
                        {
                            case eSpells::FlamingVolatileFoamMultiEff01:
                            {
                                l_SpellId = eSpells::FlamingVolatileFoamMultiEff02;
                                l_BreathDotId = eSpells::FieryPhlegmAuraDummy;
                                break;
                            }
                            case eSpells::BrineyVolatileFoamMultiEff01:
                            {
                                l_SpellId = eSpells::BrineyVolatileFoamMultiEff02;
                                l_BreathDotId = eSpells::SaltySpittleAuraDummy;
                                break;
                            }
                            case eSpells::ShadowyVolatileFoamMultiEff01:
                            {
                                l_SpellId = eSpells::ShadowyVolatileFoamMultiEff02;
                                l_BreathDotId = eSpells::DarkDischargeAuraDummy;
                                break;
                            }
                            default:
                                break;
                        }

                        if (!l_BreathDotId || !l_SpellId)
                            return;

                        AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();

                        if (l_Mode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                        {
                            switch (l_SpellInfo->Id)
                            {
                                case eSpells::FlamingVolatileFoamMultiEff01:
                                {
                                    /// Deadly Mixture
                                    if (l_Target->HasAura(eSpells::SaltySpittleAuraDummy) || l_Target->HasAura(eSpells::DarkDischargeAuraDummy))
                                        l_Caster->Kill(l_Target);

                                    break;
                                }
                                case eSpells::BrineyVolatileFoamMultiEff01:
                                {
                                    /// Deadly mixture
                                    if (l_Target->HasAura(eSpells::FieryPhlegmAuraDummy) || l_Target->HasAura(eSpells::DarkDischargeAuraDummy))
                                        l_Caster->Kill(l_Target);

                                    break;
                                }
                                case eSpells::ShadowyVolatileFoamMultiEff01:
                                {
                                    /// Deadly Mixture
                                    if (l_Target->HasAura(eSpells::SaltySpittleAuraDummy) || l_Target->HasAura(eSpells::FieryPhlegmAuraDummy))
                                        l_Caster->Kill(l_Target);

                                    break;
                                }
                                default:
                                    break;
                            }

                            if (!l_Target->HasAura(l_BreathDotId))
                                l_Caster->CastSpell(l_Target, l_SpellId, true);
                        }
                        else if (l_Mode == AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                        {
                            if (Unit* l_NearestPlayer = sObjectAccessor->GetUnit(*l_Caster, m_TargetGUID))
                            {
                                if (l_SpellInfo->GetDuration() <= 4000)
                                {
                                    if (!l_NearestPlayer->HasAura(l_SpellId))
                                        l_Caster->CastSpell(l_NearestPlayer, l_SpellId, true);

                                    l_Target->RemoveAura(GetAura(), AuraRemoveMode::AURA_REMOVE_BY_CANCEL);
                                }
                                else
                                {
                                    if (!l_NearestPlayer->HasAura(l_SpellId))
                                        l_Caster->CastSpell(l_NearestPlayer, l_SpellInfo->Id, true);

                                    l_Target->RemoveAura(GetAura(), AuraRemoveMode::AURA_REMOVE_BY_CANCEL);
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnDispel += AuraDispelFn(spell_tov_odyn_volatile_foam_AuraScript::HandleOnDispel);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_odyn_volatile_foam_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_odyn_volatile_foam_AuraScript();
        }
};

/// Guarm Chew Toy  - 235900
class spell_tov_guarm_chew_toy : public SpellScriptLoader
{
    public:
        spell_tov_guarm_chew_toy() : SpellScriptLoader("spell_tov_guarm_chew_toy") { }

        class spell_tov_guarm_chew_toy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_guarm_chew_toy_AuraScript);

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Target->IsAIEnabled)
                    return;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return;

                if (InstanceScript* l_InstanceScript = l_Creature->GetInstanceScript())
                {
                    if (Creature* l_GuarmChewToy = Creature::GetCreature(*l_Creature, l_InstanceScript->GetData64(eTovCreatures::CreatureGuarmChewToy)))
                    {
                        l_GuarmChewToy->SetDisplayId(11686);

                        l_GuarmChewToy->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tov_guarm_chew_toy_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_guarm_chew_toy_AuraScript();
        }
};

/// Flame Lick - 228228
class spell_tov_guarm_flame_lick : public SpellScriptLoader
{
    public:
        spell_tov_guarm_flame_lick() : SpellScriptLoader("spell_tov_guarm_flame_lick") { }

        class spell_tov_guarm_flame_lick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_guarm_flame_lick_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if(l_Caster->IsPlayer())
                        p_Targets.clear();
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_guarm_flame_lick_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_guarm_flame_lick_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_guarm_flame_lick_SpellScript();
        }

        class spell_tov_guarm_flame_lick_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_guarm_flame_lick_AuraScript);

            enum eSpell
            {
                FlameLick = 228228
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetAura())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::FlameLick, l_Caster->GetMap()->GetDifficultyID());

                        if (l_SpellInfo == nullptr)
                            return;

                        const Position* l_Src = l_Target;

                        l_Target->CastCustomSpell(l_Target, l_SpellInfo->Id, 0, 0, 0,  true, nullptr, nullptr);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_guarm_flame_lick_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_guarm_flame_lick_AuraScript();
        }
};

/// Frost Lick - 228248
class spell_tov_guarm_frost_lick : public SpellScriptLoader
{
    public:
        spell_tov_guarm_frost_lick() : SpellScriptLoader("spell_tov_guarm_frost_lick") { }

        class spell_tov_guarm_frost_lick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_guarm_frost_lick_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetExplTargetUnit())
                    {
                        p_Targets.remove_if([this, l_Target](WorldObject* p_WorldObject) -> bool
                        {
                            if (p_WorldObject == nullptr) ///< Arcane Beacon
                                return true;

                            if (l_Target->GetDistance2d(p_WorldObject->m_positionX, p_WorldObject->m_positionY) > 5.0f)
                                return true;

                            return false;
                        });
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_guarm_frost_lick_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_guarm_frost_lick_SpellScript();
        }
};

/// Licks missile - 228247 , 228227, 228251
class spell_tov_guarm_licks_missiles : public SpellScriptLoader
{
    public:
        spell_tov_guarm_licks_missiles() : SpellScriptLoader("spell_tov_guarm_licks_missiles") { }

        class spell_tov_guarm_licks_missiles_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_guarm_licks_missiles_SpellScript);

            void HandleScript(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                PreventHitEffect(p_EffIndex);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_tov_guarm_licks_missiles_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_guarm_licks_missiles_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Guardian's Breath - 227573
class spell_tov_guarm_guardians_breath : public SpellScriptLoader
{
    public:
        spell_tov_guarm_guardians_breath() : SpellScriptLoader("spell_tov_guarm_guardians_breath") { }

        class spell_tov_guarm_guardians_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_guarm_guardians_breath_SpellScript);

            std::array<std::pair<uint32, uint32>, 6> m_Breathes =
            {{
                { eSpells::GuardiansBreathCombination1, eSpells::GuardiansBreathVisualBreath1 },
                { eSpells::GuardiansBreathCombination2, eSpells::GuardiansBreathVisualBreath2 },
                { eSpells::GuardiansBreathCombination3, eSpells::GuardiansBreathVisualBreath3 },
                { eSpells::GuardiansBreathCombination4, eSpells::GuardiansBreathVisualBreath4 },
                { eSpells::GuardiansBreathCombination5, eSpells::GuardiansBreathVisualBreath5 },
                { eSpells::GuardiansBreathCombination6, eSpells::GuardiansBreathVisualBreath6 }
            }};

            void HandleAfterCast()
            {
                Unit* l_Body = GetCaster();
                if (m_Breathes.empty() || !l_Body)
                    return;

                uint32 l_SpellIndex = urand(0, m_Breathes.size() - 1);

                l_Body->CastSpell(l_Body, m_Breathes[l_SpellIndex].first, false);
                l_Body->CastSpell(l_Body, m_Breathes[l_SpellIndex].second, false);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_tov_guarm_guardians_breath_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_guarm_guardians_breath_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// FieryPhlegm - 227539 - Salty Spittle - 227566 - Dark Discharge - 227570
class spell_tov_guardians_breath_effects : public SpellScriptLoader
{
    public:
        spell_tov_guardians_breath_effects() : SpellScriptLoader("spell_tov_guardians_breath_effects") { }

        class spell_tov_guardians_breath_effects_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_guardians_breath_effects_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster || !l_Caster->IsAIEnabled)
                    return;

                uint32 l_TargetAmount = 0;

                switch (m_scriptSpellId)
                {
                    case eSpells::FieryPhlegm:
                    {
                        if (l_Target->HasAura(eSpells::SaltySpittleDebuff) || l_Target->HasAura(eSpells::DarkDischargeDebuff))
                        {
                            l_Caster->Kill(l_Target);
                            return;
                        }
                        l_Target->CastSpell(l_Target, eSpells::FieryPhlegmDebuff, true);

                        l_TargetAmount = l_Caster->GetAI()->GetData(eScriptData::AreatriggerTargetsRed);

                        break;
                    }
                    case eSpells::SaltySpittle:
                    {
                        if (l_Target->HasAura(eSpells::FieryPhlegmDebuff) || l_Target->HasAura(eSpells::DarkDischargeDebuff))
                        {
                            l_Caster->Kill(l_Target);
                            return;
                        }
                        l_Target->CastSpell(l_Target, eSpells::SaltySpittleDebuff, true);

                        l_TargetAmount = l_Caster->GetAI()->GetData(eScriptData::AreatriggerTargetsGreen);

                        break;
                    }
                    case eSpells::DarkDischarge:
                    {
                        if (l_Target->HasAura(eSpells::SaltySpittleDebuff) || l_Target->HasAura(eSpells::FieryPhlegmDebuff))
                        {
                            l_Caster->Kill(l_Target);
                            return;
                        }
                        l_Target->CastSpell(l_Target, eSpells::DarkDischargeDebuff, true);

                        l_TargetAmount = l_Caster->GetAI()->GetData(eScriptData::AreatriggerTargetsPurple);

                        break;
                    }
                    default:
                        break;
                }

                l_Caster->GetAI()->SetData(eScriptData::BreathHit, 0);

                SetHitDamage(GetHitDamage() / (l_TargetAmount ? l_TargetAmount : 1));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tov_guardians_breath_effects_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_guardians_breath_effects_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Multi-Headed- 227642
class spell_tov_guarm_multi_headed : public SpellScriptLoader
{
    public:
        spell_tov_guarm_multi_headed() : SpellScriptLoader("spell_tov_guarm_multi_headed") { }

        class spell_tov_guarm_multi_headed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_guarm_multi_headed_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Target = GetExplTargetUnit();
                if (p_Targets.size() < 2 || !l_Target)
                    return;

                p_Targets.sort(JadeCore::ObjectDistanceOrderPredPlayer(l_Target));

                p_Targets.pop_front();
                p_Targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_guarm_multi_headed_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_guarm_multi_headed_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_tov_boss_guarm()
{
    /// Boss
    new tov_boss_guarm();

    /// Spells
    new spell_tov_guarm_guardians_breath();
    new spell_tov_guardians_breath_effects();
    new spell_tov_guarm_frost_lick();
    new spell_tov_odyn_volatile_foam();
    new spell_tov_guarm_chew_toy();
    new spell_tov_guarm_flame_lick();
    new spell_tov_guarm_licks_missiles();
    new spell_tov_guarm_multi_headed();
}
#endif
