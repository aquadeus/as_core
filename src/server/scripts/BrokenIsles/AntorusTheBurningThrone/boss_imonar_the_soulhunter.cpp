////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"

/// Imonar the Soulhunter - 124158
class boss_imonar_the_soulhunter : public CreatureScript
{
    public:
        boss_imonar_the_soulhunter() : CreatureScript("boss_imonar_the_soulhunter") { }

        enum eSpells
        {
            /// Misc
            SpellFelJetpacks                    = 248995,
            SpellFireJetpacks                   = 248194,
            SpellDarkWinds                      = 254253,
            SpellFelInfernalLanding             = 255472,
            SpellFireInfernalLanding            = 248250,
            SpellStasisTraps                    = 248224,
            SpellSearedSkinDebuff               = 254183,
            SpellSpawnVisual                    = 246951,
            /// Shock Lance
            SpellShockLanceTarget               = 247367,
            SpellEmpoweredShockLance            = 250255,
            /// Sleep Canister
            SpellSleepCanisterCast              = 254244,
            SpellSleepCanisterMissile           = 247552,
            SpellSleepCanisterDebuff            = 255029,
            /// Pulse Grenade
            SpellPulseGrenade                   = 247376,
            SpellPulseGrenadeSearcher1          = 247373,
            SpellPulseGrenadeMissile            = 247381,
            SpellPulseGrenadeTraps              = 248223,
            SpellEmpoweredPulseGrenadeCast      = 248068,
            SpellEmpoweredPulseGrenadeSearcher  = 248154,
            SpellEmpoweredPulseGrenadeAura      = 250006,
            /// Infernal Rockets
            SpellInfernalRockets                = 248252,
            /// Conflagration
            SpellConflagrationChannel           = 248233,
            SpellConflagrationSearcher          = 250136,
            SpellConflagrationDoT               = 250191,
            SpellGatheringPower                 = 248424,
            /// Sever
            SpellSeverCast                      = 247687,
            /// Charged Blasts
            SpellChargedBlastsCast              = 248254,
            SpellChargedBlastsDoT               = 247716,
            SpellChargedBlastsScale             = 257197,
            /// Shrapnel Blast
            SpellShrapnelBlastSearcher          = 247923,
            SpellShrapnelBlastMissile           = 247927,
            SpellShrapnelSearcher               = 248238,
            SpellEmpoweredShrapnelBlastCast     = 248070,
            SpellEmpoweredShrapnelBlastDoT      = 248094,
            SpellShrapnelBlastDoT               = 247932,
            /// Blastwire
            SpellBlastwireSearcher              = 248237
        };

        enum eEvents
        {
            EventShockLance = 1,
            EventSleepCanister,
            EventPulseGrenade,
            EventEndIntermissionMove,
            EventSever,
            EventChargedBlasts,
            EventShrapnelBlast,
            EventEmpoweredShockLance,
            EventEmpoweredPulseGrenade,
            EventEmpoweredShrapnelBlast
        };

        enum eStages
        {
            StageOneAttackForce,
            StageTwoContractToKill,
            StageThreeMarkedForDeath,
            StageFourKillSwitch,
            StageFiveThePerfectWeapon,
            IntermissionOnDeadlyGround,

            StageSidesCount = 2
        };

        enum eActions
        {
            ActionIntro         = 0,
            ActionPulseGrenade  = 0,
            ActionBlastwire,
            ActionStasisTrap,
            ActionShrapnelBlast
        };

        enum eTalks
        {
            TalkIntro,
            TalkAggro,
            TalkSleepCanister,
            TalkTransition,
            TalkSever,
            TalkConflagration,
            TalkShrapnelBlast,
            TalkInfernalRockets,
            TalkSlay,
            TalkWipe,
            TalkDeath,
            TalkSleepCanisterWarn
        };

        struct boss_imonar_the_soulhunterAI : public BossAI
        {
            boss_imonar_the_soulhunterAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataImonarTheSoulhunter)
            {
                /// Open way to Kin'garoth if Imonar is dead
                InstanceScript* l_Instance = p_Creature->GetInstanceScript();
                if (!l_Instance)
                    return;

                uint64 l_Guid = p_Creature->GetGUID();
                l_Instance->AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [l_Instance, l_Guid]() -> void
                {
                    if (Creature* l_Imonar = l_Instance->instance->GetCreature(l_Guid))
                    {
                        if (l_Imonar->isDead() && !l_Imonar->IsNeedRespawn())
                        {
                            l_Instance->SetData(eData::DataDestroyKingarothDoor, 1);
                            if (l_Instance->GetBossState(eData::DataImonarTheSoulhunter) != EncounterState::DONE)
                                l_Instance->SetBossState(eData::DataImonarTheSoulhunter, EncounterState::DONE);
                        }
                    }
                });
            }

            bool m_IntroDone = false;
            bool m_Intermission = false;
            bool m_FireTraps = false;
            bool m_Achievement = false;

            uint64 m_ShipGuid = 0;

            uint32 m_BerserkerTimer = 0;
            uint32 m_RocketsTimer = 0;

            uint8 m_StageID = eStages::StageOneAttackForce;

            uint8 m_SwitchStagePctIDx = 0;

            std::vector<uint32> m_SwitchHealthPcts;

            std::vector<uint8> m_ConflagrationIDx;

            uint32 m_EmpoweredShrapnels = 0;
            uint32 m_ConflagrationCount = 0;
            uint32 m_TrapsToTrigger = 0;

            void OnCalculateAttackDistance(float& p_AttackDistance) override
            {
                p_AttackDistance = 6.0f;
            }

            bool BypassXPDistanceIfNeeded(WorldObject* /*p_Object*/) override
            {
                return true;
            }

            void Reset() override
            {
                _Reset();

                summons.DespawnAll();

                me->RemoveAllAreasTrigger();

                me->RemoveAura(eSharedSpells::SpellBerserk);

                m_Intermission = false;
                m_FireTraps = false;
                m_Achievement = false;

                m_BerserkerTimer = 0;
                m_RocketsTimer = 0;

                m_SwitchStagePctIDx = 0;

                if (IsMythic())
                    m_SwitchHealthPcts = { 80, 60, 40, 20 };
                else
                    m_SwitchHealthPcts = { 66, 33 };

                m_ConflagrationIDx.clear();

                m_EmpoweredShrapnels = 0;
                m_ConflagrationCount = 0;
                m_TrapsToTrigger = 0;
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                if (m_Intermission)
                    p_Velocity = 25.0f;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                m_StageID = eStages::StageOneAttackForce;

                Talk(eTalks::TalkAggro);

                DefaultEvents();
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(eTalks::TalkSlay);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                Talk(eTalks::TalkDeath);

                RemoveEncounterAuras();

                std::list<Creature*> l_Traps;

                me->GetCreatureListWithEntryInGrid(l_Traps, eCreatures::NpcTraps, 200.0f);

                for (Creature* l_Trap : l_Traps)
                    l_Trap->DespawnOrUnsummon();

                if (m_ShipGuid)
                {
                    if (Creature* l_Ship = Creature::GetCreature(*me, m_ShipGuid))
                        l_Ship->DespawnOrUnsummon();
                }

                if (!IsLFR() && m_Achievement && instance)
                {
                    bool l_AchievDisabled = sObjectMgr->IsDisabledMap(instance->instance->GetId(), instance->instance->GetDifficultyID());

                    if (!l_AchievDisabled)
                        l_AchievDisabled = sObjectMgr->IsDisabledEncounter(eData::DataImonarEncounterID, instance->instance->GetDifficultyID());

                    if (!l_AchievDisabled)
                        instance->DoCompleteAchievement(eAchievements::HardToKill);
                }

                if (instance && instance->GetData(eData::DataDungeonID) != eDungeonIDs::DungeonIDForbiddenDescent)
                    instance->SetData(eData::DataDestroyKingarothDoor, 0);
            }

            void EnterEvadeMode() override
            {
                me->InterruptNonMeleeSpells(true);

                std::list<Creature*> l_Traps;

                me->GetCreatureListWithEntryInGrid(l_Traps, eCreatures::NpcTraps, 200.0f);

                for (Creature* l_Trap : l_Traps)
                {
                    l_Trap->RemoveAllAuras();
                    l_Trap->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);
                }

                Talk(eTalks::TalkWipe);

                ClearDelayedOperations();

                BossAI::EnterEvadeMode();

                RemoveEncounterAuras();
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_SwitchStagePctIDx >= m_SwitchHealthPcts.size())
                    return;

                if (me->HealthBelowPctDamaged(m_SwitchHealthPcts[m_SwitchStagePctIDx], p_Damage))
                {
                    m_Intermission = true;

                    m_ConflagrationCount = 0;

                    m_ConflagrationIDx.clear();

                    events.Reset();

                    me->StopAttack();
                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    Talk(eTalks::TalkTransition);

                    Position l_Dest;

                    if (me->GetDistance(g_WestImonarPos) > me->GetDistance(g_EastImonarPos))
                        l_Dest = g_WestImonarPos;
                    else
                        l_Dest = g_EastImonarPos;

                    me->SetFacingTo(me->GetAngle(&l_Dest));

                    SpawnTraps(m_FireTraps, &l_Dest);

                    ++m_SwitchStagePctIDx;
                    ++m_StageID;

                    if (m_FireTraps)
                        DoCast(me, eSpells::SpellFireJetpacks, true);
                    else
                        DoCast(me, eSpells::SpellFelJetpacks, true);

                    if (IsMythic())
                        DoCast(me, eSpells::SpellDarkWinds, true);

                    Movement::MoveSplineInit l_Init(me);

                    Optional<Movement::MonsterSplineFilter> l_Filter;

                    l_Init.SetFly();
                    l_Init.SetSmooth();
                    l_Init.SetUncompressed();

                    l_Filter.emplace();

                    l_Filter->FilterFlags   = 2;
                    l_Filter->BaseSpeed     = 20.0f;
                    l_Filter->AddedToStart  = 1;

                    l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(0, 2000));
                    l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(1, 3000));

                    l_Init.SetMonsterSplineFilters(*l_Filter.get_ptr());

                    Position l_Pos = me->GetPosition();

                    l_Pos.m_positionZ += 8.3f;

                    l_Init.Path().push_back(l_Pos.AsVector3());

                    float l_Step = me->GetDistance(l_Dest) / 10.0f;
                    float l_Dist = l_Step;

                    for (uint8 l_I = 0; l_I < 9; ++l_I)
                    {
                        me->SimplePosXYRelocationByAngle(l_Pos, l_Dist, me->m_orientation, true);

                        l_Pos.m_positionZ += 8.3f;

                        l_Dist += l_Step;

                        l_Init.Path().push_back(l_Pos.AsVector3());
                    }

                    l_Init.Path().push_back(l_Dest.AsVector3());

                    l_Init.Launch();

                    events.ScheduleEvent(eEvents::EventEndIntermissionMove, 8 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSleepCanisterCast:
                    {
                        sCreatureTextMgr->SendChat(me, eTalks::TalkSleepCanisterWarn, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);
                        DoCast(p_Target, eSpells::SpellSleepCanisterMissile, true);
                        break;
                    }
                    case eSpells::SpellPulseGrenadeSearcher1:
                    {
                        DoCast(p_Target, eSpells::SpellPulseGrenadeMissile, true);
                        break;
                    }
                    case eSpells::SpellPulseGrenadeTraps:
                    {
                        Creature* l_Trap = p_Target->ToCreature();
                        if (!l_Trap || !l_Trap->IsAIEnabled)
                            break;

                        ++m_TrapsToTrigger;
                        m_Achievement = false;

                        l_Trap->AI()->DoAction(eActions::ActionPulseGrenade);
                        break;
                    }
                    case eSpells::SpellStasisTraps:
                    {
                        Creature* l_Trap = p_Target->ToCreature();
                        if (!l_Trap || !l_Trap->IsAIEnabled)
                            break;

                        ++m_TrapsToTrigger;
                        m_Achievement = false;

                        l_Trap->AI()->DoAction(eActions::ActionStasisTrap);
                        break;
                    }
                    case eSpells::SpellShrapnelBlastSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellShrapnelBlastMissile, true);
                        break;
                    }
                    case eSpells::SpellBlastwireSearcher:
                    {
                        Creature* l_Trap = p_Target->ToCreature();
                        if (!l_Trap || !l_Trap->IsAIEnabled)
                            break;

                        ++m_TrapsToTrigger;
                        m_Achievement = false;

                        l_Trap->AI()->DoAction(eActions::ActionBlastwire);
                        break;
                    }
                    case eSpells::SpellShrapnelSearcher:
                    {
                        Creature* l_Trap = p_Target->ToCreature();
                        if (!l_Trap || !l_Trap->IsAIEnabled)
                            break;

                        ++m_TrapsToTrigger;
                        m_Achievement = false;

                        l_Trap->AI()->DoAction(eActions::ActionShrapnelBlast);
                        break;
                    }
                    case eSpells::SpellEmpoweredPulseGrenadeSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellEmpoweredPulseGrenadeAura, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellSleepCanisterCast:
                    {
                        uint64 targets[2] = { 0,0 };

                        auto fillHMTargets = [&targets](std::list<WorldObject*>& _targets)
                        {
                            JadeCore::Containers::RandomShuffleList(_targets);
                            for (auto targ : _targets)
                            {
                                if (!targ->IsPlayer())
                                    continue;

                                if (targ->ToPlayer()->IsMeleeDamageDealer(false, false))
                                    targets[0] = targ->GetGUID();
                                if (targ->ToPlayer()->IsHealer() || targ->ToPlayer()->IsRangedDamageDealer(false))
                                    targets[1] = targ->GetGUID();
                            }
                        };

                        if (IsHeroicOrMythic())
                            fillHMTargets(p_Targets);

                        p_Targets.remove_if([this, targets](WorldObject* obj) -> bool
                        {
                            if (!obj || !obj->IsPlayer() || obj->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            if (IsHeroicOrMythic())
                            {
                                if (targets[0] && obj->ToPlayer()->IsMeleeDamageDealer(false, false) && obj->GetGUID() != targets[0])
                                    return true;
                                if ((obj->ToPlayer()->IsHealer() || obj->ToPlayer()->IsRangedDamageDealer(false)) && targets[1] && obj->GetGUID() != targets[1])
                                    return true;
                            }

                            return false;
                        });

                        break;
                    }
                    case eSpells::SpellShrapnelBlastSearcher:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            return false;
                        });

                        break;
                    }
                    case eSpells::SpellEmpoweredPulseGrenadeSearcher:
                    {
                        p_Targets.clear();

                        std::vector<uint64> l_Targets;

                        for (uint8 l_I = 0; l_I < 2; ++l_I)
                        {
                            if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllRanged, { -int32(eSpells::SpellEmpoweredPulseGrenadeAura) }, 0.0f, l_Targets))
                            {
                                l_Targets.push_back(l_Target->GetGUID());

                                p_Targets.push_back(l_Target);
                            }
                        }

                        break;
                    }
                    case eSpells::SpellPulseGrenadeSearcher1:
                    {
                        if (!instance)
                        {
                            p_Targets.clear();
                            break;
                        }

                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            return false;
                        });

                        uint32 l_PlayerCount = instance->instance->GetPlayersCountExceptGMs();
                        if (!l_PlayerCount)
                        {
                            p_Targets.clear();
                            break;
                        }

                        float l_Count = 2.0f + (1.0f * ((float(l_PlayerCount) - 10.0f) / 5.0f));

                        /// Can't target enough players
                        if (l_Count < 1.0f)
                        {
                            p_Targets.clear();
                            break;
                        }

                        /// No need to resize
                        if (p_Targets.size() <= l_Count)
                            break;

                        JadeCore::Containers::RandomResizeList(p_Targets, l_Count);
                        break;
                    }
                    case eSpells::SpellInfernalRockets:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->IsFalling())
                                return true;

                            switch (m_StageID)
                            {
                                case eStages::StageOneAttackForce:
                                case eStages::StageThreeMarkedForDeath:
                                case eStages::StageFiveThePerfectWeapon:
                                {
                                    if (p_Object->GetDistance(g_WestImonarPos) < 28.0f)
                                        return true;

                                    break;
                                }
                                case eStages::StageTwoContractToKill:
                                case eStages::StageFourKillSwitch:
                                {
                                    if (p_Object->GetDistance(g_EastImonarPos) < 28.0f)
                                        return true;

                                    break;
                                }
                                default:
                                    break;
                            }

                            return false;
                        });

                        break;
                    }
                    case eSpells::SpellPulseGrenadeTraps:
                    case eSpells::SpellStasisTraps:
                    case eSpells::SpellBlastwireSearcher:
                    case eSpells::SpellShrapnelSearcher:
                    case eSpells::SpellConflagrationSearcher:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object)
                                return true;

                            Creature* l_Trap = p_Object->ToCreature();
                            if (!l_Trap || !l_Trap->IsAIEnabled)
                                return true;

                            if (!l_Trap->AI()->GetData())
                                return true;

                            return false;
                        });

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
                    case eSpells::SpellChargedBlastsCast:
                    {
                        std::array<std::vector<Position>, eStages::StageSidesCount> l_ChargedBlastsPos =
                        {
                            {
                                {
                                    {
                                        { -10531.30f, 8555.837f, 1878.132f, 3.569957f },
                                        { -10538.64f, 8499.767f, 1878.132f, 2.354932f },
                                        { -10523.78f, 8526.526f, 1878.146f, 2.958116f }
                                    }
                                },
                                {
                                    {
                                        { -10625.28f, 8535.341f, 1878.146f, 0.012794f },
                                        { -10621.72f, 8571.310f, 1878.132f, 5.638225f },
                                        { -10627.93f, 8495.828f, 1878.132f, 0.646940f }
                                    }
                                }
                            }
                        };

                        std::vector<Position> l_Positions = l_ChargedBlastsPos[urand(0, 1)];

                        JadeCore::RandomResizeList(l_Positions, 2);

                        for (Position const& l_Pos : l_Positions)
                            me->SummonCreature(eCreatures::NpcBombingRun, l_Pos);

                        break;
                    }
                    case eSpells::SpellEmpoweredShrapnelBlastCast:
                    {
                        std::array<Position, eStages::StageSidesCount> l_ShipPos =
                        {
                            {
                                { -10616.62f, 8708.638f, 1904.495f, 0.575878f },
                                { -10533.54f, 8757.922f, 1892.452f, 4.417715f }
                            }
                        };

                        me->SummonCreature(eCreatures::NpcArtilleryStrike, l_ShipPos[urand(0, 1)]);
                        break;
                    }
                    case eSpells::SpellGatheringPower:
                    {
                        if (!IsHeroicOrMythic())
                            break;

                        std::array<std::array<Position, 3>, eStages::StageSidesCount> l_ConflagrationPos =
                        {
                            {
                                {
                                    {
                                        { -10565.92f, 8709.794f, 1872.075f, 4.712122f },
                                        { -10574.82f, 8710.021f, 1871.838f, 4.711349f },
                                        { -10583.89f, 8710.041f, 1872.075f, 4.712497f }
                                    }
                                },
                                {
                                    {
                                        { -10565.63f, 8555.972f, 1871.938f, 1.568416f },
                                        { -10575.08f, 8555.803f, 1871.736f, 1.567259f },
                                        { -10584.54f, 8556.196f, 1871.957f, 1.569359f }
                                    }
                                }
                            }
                        };

                        if (m_ConflagrationIDx.empty())
                        {
                            m_ConflagrationIDx = { 0, 1, 2 };

                            std::random_device l_RandomDevice;
                            std::mt19937 l_RandomGenerator(l_RandomDevice());
                            std::shuffle(m_ConflagrationIDx.begin(), m_ConflagrationIDx.end(), l_RandomGenerator);
                        }

                        ++m_ConflagrationCount;

                        /// Pause for three more ticks
                        if (m_ConflagrationCount > 3 && m_ConflagrationCount <= 6)
                            break;

                        if (m_ConflagrationCount >= 6)
                        {
                            m_ConflagrationCount = 1;

                            m_ConflagrationIDx = { 0, 1, 2 };

                            std::random_device l_RandomDevice;
                            std::mt19937 l_RandomGenerator(l_RandomDevice());
                            std::shuffle(m_ConflagrationIDx.begin(), m_ConflagrationIDx.end(), l_RandomGenerator);
                        }

                        if (Creature* l_Orb = me->SummonCreature(eCreatures::NpcConflagration, l_ConflagrationPos[m_FireTraps][m_ConflagrationIDx[m_ConflagrationCount % 3]]))
                        {
                            if (l_Orb->IsAIEnabled)
                                l_Orb->AI()->SetData(0, !m_FireTraps);
                        }

                        DoCast(me, eSpells::SpellConflagrationSearcher, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                switch (p_SpellID)
                {
                    case eSpells::SpellConflagrationChannel:
                    {
                        if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_CANCEL && p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                            break;

                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_Intermission = false;
                        });

                        summons.DespawnEntry(eCreatures::NpcConflagration);
                        summons.DespawnEntry(eCreatures::NpcImonarsGarothi);

                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        for (std::list<HostileReference*>::const_iterator l_Iter = l_ThreatList.begin(); l_Iter != l_ThreatList.end(); ++l_Iter)
                        {
                            if (Unit* l_Unit = (*l_Iter)->getTarget())
                            {
                                if (!l_Unit->IsPlayer() || l_Unit->isDead())
                                    continue;

                                l_Unit->ToPlayer()->SendApplyMovementForce(me->GetGUID(), false);
                            }
                        }

                        DefaultEvents();

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Unit* l_Target = me->getVictim())
                        {
                            AttackStart(l_Target);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Target);
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

                if (!m_IntroDone)
                {
                    if (Player* l_Player = me->FindNearestPlayer(60.0f))
                    {
                        m_IntroDone = true;

                        if (instance)
                        {
                            if (Creature* l_Ship = instance->instance->SummonCreature(eCreatures::NpcImonarShip, { -10574.94f, 8479.502f, 1914.706f, 1.5591f }))
                                m_ShipGuid = l_Ship->GetGUID();

                            if (GameObject* l_Door = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoKingarothAccessDoor)))
                                l_Door->SetGoState(GOState::GO_STATE_READY);
                        }

                        AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetVisible(true);

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            DoCast(me, eSpells::SpellSpawnVisual, true);
                        });

                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro);
                        });
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_BerserkerTimer)
                {
                    if (m_BerserkerTimer <= p_Diff)
                    {
                        m_BerserkerTimer = 0;

                        DoCast(me, eSharedSpells::SpellBerserk, true);
                    }
                    else
                        m_BerserkerTimer -= p_Diff;
                }

                HandleInfernalRockets(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShockLance:
                    {
                        DoCastVictim(eSpells::SpellShockLanceTarget);

                        events.ScheduleEvent(eEvents::EventShockLance, 5 * TimeConstants::IN_MILLISECONDS + 900);
                        break;
                    }
                    case eEvents::EventSleepCanister:
                    {
                        DoCast(me, eSpells::SpellSleepCanisterCast);

                        Talk(eTalks::TalkSleepCanister);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventSleepCanister, 13 * TimeConstants::IN_MILLISECONDS + 100);
                        else
                            events.ScheduleEvent(eEvents::EventSleepCanister, 11 * TimeConstants::IN_MILLISECONDS + 900);

                        break;
                    }
                    case eEvents::EventPulseGrenade:
                    {
                        DoCast(me, eSpells::SpellPulseGrenade);

                        events.ScheduleEvent(eEvents::EventPulseGrenade, 17 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventEndIntermissionMove:
                    {
                        Position l_Src;
                        Position l_Dest;

                        if (me->GetDistance(g_WestImonarPos) > me->GetDistance(g_EastImonarPos))
                        {
                            l_Src = g_EastImonarPos;
                            l_Dest = g_WestImonarPos;
                        }
                        else
                        {
                            l_Src = g_WestImonarPos;
                            l_Dest = g_EastImonarPos;
                        }

                        l_Src.SetOrientation(l_Src.GetAngle(&l_Dest));

                        AddTimedDelayedOperation(1, [this, l_Dest]() -> void
                        {
                            me->SetFacingTo(me->GetAngle(&l_Dest));
                        });

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Src]() -> void
                        {
                            me->NearTeleportTo(l_Src);

                            Talk(eTalks::TalkConflagration);

                            DoCast(me, eSpells::SpellConflagrationChannel);
                        });

                        me->RemoveAura(eSpells::SpellFelJetpacks);
                        me->RemoveAura(eSpells::SpellFireJetpacks);

                        if (m_FireTraps)
                            DoCast(me, eSpells::SpellFireInfernalLanding, true);
                        else
                            DoCast(me, eSpells::SpellFelInfernalLanding, true);

                        m_FireTraps = !m_FireTraps;
                        break;
                    }
                    case eEvents::EventSever:
                    {
                        Talk(eTalks::TalkSever);

                        DoCastVictim(eSpells::SpellSeverCast);

                        events.ScheduleEvent(eEvents::EventSever, 8 * TimeConstants::IN_MILLISECONDS + 300);
                        break;
                    }
                    case eEvents::EventChargedBlasts:
                    {
                        DoCast(me, eSpells::SpellChargedBlastsCast);

                        if (IsMythic())
                        {
                            if (m_StageID == eStages::StageTwoContractToKill)
                                events.ScheduleEvent(eEvents::EventChargedBlasts, 14 * TimeConstants::IN_MILLISECONDS + 500);
                            else
                                events.ScheduleEvent(eEvents::EventChargedBlasts, 18 * TimeConstants::IN_MILLISECONDS + 200);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventChargedBlasts, 18 * TimeConstants::IN_MILLISECONDS + 200);

                        break;
                    }
                    case eEvents::EventShrapnelBlast:
                    {
                        Talk(eTalks::TalkShrapnelBlast);

                        DoCast(me, eSpells::SpellShrapnelBlastSearcher);

                        if (IsMythic())
                        {
                            if (m_StageID == eStages::StageTwoContractToKill)
                                events.ScheduleEvent(eEvents::EventShrapnelBlast, 17 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventShrapnelBlast, 14 * TimeConstants::IN_MILLISECONDS + 600);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventShrapnelBlast, 13 * TimeConstants::IN_MILLISECONDS + 400);

                        break;
                    }
                    case eEvents::EventEmpoweredShockLance:
                    {
                        DoCastVictim(eSpells::SpellEmpoweredShockLance);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventEmpoweredShockLance, 7 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventEmpoweredShockLance, 10 * TimeConstants::IN_MILLISECONDS + 700);

                        break;
                    }
                    case eEvents::EventEmpoweredPulseGrenade:
                    {
                        DoCast(me, eSpells::SpellEmpoweredPulseGrenadeCast);

                        /// Mythic only
                        if (m_StageID == eStages::StageFiveThePerfectWeapon)
                            events.ScheduleEvent(eEvents::EventEmpoweredPulseGrenade, 13 * TimeConstants::IN_MILLISECONDS + 300);
                        else
                            events.ScheduleEvent(eEvents::EventEmpoweredPulseGrenade, 26 * TimeConstants::IN_MILLISECONDS + 700);

                        break;
                    }
                    case eEvents::EventEmpoweredShrapnelBlast:
                    {
                        Talk(eTalks::TalkShrapnelBlast);

                        ++m_EmpoweredShrapnels;

                        uint32 l_Time = 0;

                        if (IsMythic())
                        {
                            std::vector<uint32> l_Timers =
                            {
                                { 0, 15700, 15700, 14500, 14500, 12200, 12200 }
                            };

                            if (m_EmpoweredShrapnels < l_Timers.size())
                                l_Time = l_Timers[m_EmpoweredShrapnels];
                        }
                        else
                        {
                            std::vector<uint32> l_Timers =
                            {
                                { 0, 21800, 20600, 18000, 15800, 16000, 13500, 10000 }
                            };

                            if (m_EmpoweredShrapnels < l_Timers.size())
                                l_Time = l_Timers[m_EmpoweredShrapnels];
                        }

                        DoCast(me, eSpells::SpellEmpoweredShrapnelBlastCast);

                        if (!l_Time)
                            break;

                        events.ScheduleEvent(eEvents::EventEmpoweredShrapnelBlast, l_Time);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void SetData(uint32 /*p_ID*/, uint32 /*p_Value*/) override
            {
                --m_TrapsToTrigger;

                if (!m_Achievement && !m_TrapsToTrigger)
                    m_Achievement = true;
            }

            void HandleInfernalRockets(uint32 const p_Diff)
            {
                if (!m_RocketsTimer || m_Intermission)
                    return;

                if (m_RocketsTimer <= p_Diff)
                {
                    m_RocketsTimer = 1 * TimeConstants::IN_MILLISECONDS;

                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator l_Iter = l_ThreatList.begin(); l_Iter != l_ThreatList.end(); ++l_Iter)
                    {
                        if (Unit* l_Unit = (*l_Iter)->getTarget())
                        {
                            if (!l_Unit->IsPlayer() || l_Unit->IsFalling())
                                continue;

                            switch (m_StageID)
                            {
                                case eStages::StageOneAttackForce:
                                case eStages::StageThreeMarkedForDeath:
                                case eStages::StageFiveThePerfectWeapon:
                                {
                                    if (l_Unit->GetExactDist(&g_WestImonarPos) < 40.0f)
                                        continue;

                                    break;
                                }
                                case eStages::StageTwoContractToKill:
                                case eStages::StageFourKillSwitch:
                                {
                                    if (l_Unit->GetExactDist(&g_EastImonarPos) < 40.0f)
                                        continue;

                                    break;
                                }
                                default:
                                    break;
                            }

                            if (!urand(0, 3))
                                Talk(eTalks::TalkInfernalRockets);

                            DoCast(me, eSpells::SpellInfernalRockets, true);
                            break;
                        }
                    }
                }
                else
                    m_RocketsTimer -= p_Diff;
            }

            void SpawnTraps(bool p_FireTraps, Position const* p_Dest)
            {
                /// Fire on the edges of the bridge
                std::array<Position, 9> l_FirePos =
                {
                    {
                        { -10561.67f, 8664.349f, 1873.310f, 1.360466f },
                        { -10589.34f, 8652.250f, 1873.938f, 1.586943f },
                        { -10588.45f, 8664.407f, 1873.025f, 1.586943f },
                        { -10554.88f, 8641.350f, 1872.523f, 2.330433f },
                        { -10560.06f, 8630.497f, 1872.855f, 1.530985f },
                        { -10560.54f, 8600.841f, 1872.991f, 1.576859f },
                        { -10560.57f, 8595.661f, 1872.870f, 1.576859f },
                        { -10588.57f, 8595.094f, 1872.778f, 1.575356f },
                        { -10588.86f, 8624.691f, 1873.217f, 1.586943f }
                    }
                };

                for (Position const& l_Pos : l_FirePos)
                {
                    AreaTrigger* l_AreaTrigger = new AreaTrigger;
                    if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_AREATRIGGER), 0, me, nullptr, nullptr, l_Pos, l_Pos, nullptr, 0, 16404))
                    {
                        delete l_AreaTrigger;
                        return;
                    }

                    l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 15.20691f);
                }

                /// Vents that apply a movement force
                if (IsMythic())
                {
                    std::array<Position, eStages::StageSidesCount> l_VentsPos =
                    {
                        {
                            { -10575.05f, 8624.416f, 1871.608f, 3.1440130f },
                            { -10575.08f, 8624.702f, 1871.608f, 0.1144211f }
                        }
                    };

                    me->SummonCreature(eCreatures::NpcImonarsGarothi, l_VentsPos[!p_FireTraps]);

                    /// Apply movement forces
                    std::array<Position, eStages::StageSidesCount> l_VentsDir =
                    {
                        {
                            { -10575.05f, 8624.416f, 1871.608f, 3.1440130f },
                            { -10575.08f, 8624.702f, 1871.608f, 0.1144211f }
                        }
                    };

                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator l_Iter = l_ThreatList.begin(); l_Iter != l_ThreatList.end(); ++l_Iter)
                    {
                        if (Unit* l_Unit = (*l_Iter)->getTarget())
                        {
                            if (!l_Unit->IsPlayer() || l_Unit->isDead())
                                continue;

                            l_Unit->ToPlayer()->SendApplyMovementForce(me->GetGUID(), true, l_VentsDir[!p_FireTraps], p_FireTraps ? -2.5f : 2.5f, 0, p_FireTraps ? G3D::Vector3(-2.5f, 0.0f, 0.0f) : G3D::Vector3(2.5f, 0.0f, 0.0f));
                        }
                    }
                }

                if (p_FireTraps)
                {
                    bool l_Blastwire = true;

                    std::array<uint32, eStages::StageSidesCount> l_FelTraps =
                    {
                        {
                            eSpells::SpellBlastwireSearcher,
                            eSpells::SpellShrapnelSearcher
                        }
                    };

                    uint32 l_Time = (me->GetExactDist(p_Dest) - 160.0f) * 20.0f;

                    for (uint8 l_I = 0; l_I < 6; ++l_I)
                    {
                        if (!l_Time)
                            DoCast(me, l_FelTraps[uint8(l_Blastwire)], true);
                        else
                        {
                            AddTimedDelayedOperation(l_Time, [this, l_FelTraps, l_Blastwire]() -> void
                            {
                                DoCast(me, l_FelTraps[uint8(l_Blastwire)], true);
                            });
                        }

                        l_Time += 1 * TimeConstants::IN_MILLISECONDS;

                        l_Blastwire = !l_Blastwire;
                    }
                }
                /// Spawn Stasis Traps and Pulse Grenades
                else
                {
                    bool l_Grenade = true;

                    std::array<uint32, eStages::StageSidesCount> l_FelTraps =
                    {
                        {
                            eSpells::SpellPulseGrenadeTraps,
                            eSpells::SpellStasisTraps
                        }
                    };

                    uint32 l_Time = (me->GetExactDist(p_Dest) - 160.0f) * 20.0f;

                    for (uint8 l_I = 0; l_I < 6; ++l_I)
                    {
                        if (!l_Time)
                            DoCast(me, l_FelTraps[uint8(l_Grenade)], true);
                        else
                        {
                            AddTimedDelayedOperation(l_Time, [this, l_FelTraps, l_Grenade]() -> void
                            {
                                DoCast(me, l_FelTraps[uint8(l_Grenade)], true);
                            });
                        }

                        l_Time += 1 * TimeConstants::IN_MILLISECONDS;

                        l_Grenade = !l_Grenade;
                    }
                }
            }

            void RemoveEncounterAuras()
            {
                if (!instance)
                    return;

                instance->DoRemoveForcedMovementsOnPlayers(me->GetGUID());

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSeverCast);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellChargedBlastsDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellChargedBlastsScale);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSearedSkinDebuff);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredShockLance);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredPulseGrenadeAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredShrapnelBlastDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellConflagrationDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSleepCanisterDebuff);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellShrapnelBlastDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellInfernalRockets);
            }

            void DefaultEvents()
            {
                switch (m_StageID)
                {
                    case eStages::StageOneAttackForce:
                    {
                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventShockLance, 4 * TimeConstants::IN_MILLISECONDS + 800);
                            events.ScheduleEvent(eEvents::EventSleepCanister, 7 * TimeConstants::IN_MILLISECONDS + 200);
                            events.ScheduleEvent(eEvents::EventEmpoweredPulseGrenade, 13 * TimeConstants::IN_MILLISECONDS + 400);

                            m_BerserkerTimer = 480 * TimeConstants::IN_MILLISECONDS;
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventShockLance, 4 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventSleepCanister, 7 * TimeConstants::IN_MILLISECONDS + 300);
                            events.ScheduleEvent(eEvents::EventPulseGrenade, 12 * TimeConstants::IN_MILLISECONDS + 200);
                        }

                        break;
                    }
                    case eStages::StageTwoContractToKill:
                    {
                        events.ScheduleEvent(eEvents::EventSever, 7 * TimeConstants::IN_MILLISECONDS + 700);
                        events.ScheduleEvent(eEvents::EventChargedBlasts, 10 * TimeConstants::IN_MILLISECONDS + 600);
                        events.ScheduleEvent(eEvents::EventShrapnelBlast, 12 * TimeConstants::IN_MILLISECONDS + 800);
                        break;
                    }
                    case eStages::StageThreeMarkedForDeath:
                    {
                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventShockLance, 4 * TimeConstants::IN_MILLISECONDS + 800);
                            events.ScheduleEvent(eEvents::EventSleepCanister, 7 * TimeConstants::IN_MILLISECONDS + 300);
                            events.ScheduleEvent(eEvents::EventEmpoweredPulseGrenade, 6 * TimeConstants::IN_MILLISECONDS + 800);
                            events.ScheduleEvent(eEvents::EventShrapnelBlast, 12 * TimeConstants::IN_MILLISECONDS + 800);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventEmpoweredShockLance, 4 * TimeConstants::IN_MILLISECONDS + 300);
                            events.ScheduleEvent(eEvents::EventEmpoweredPulseGrenade, 6 * TimeConstants::IN_MILLISECONDS + 800);
                            events.ScheduleEvent(eEvents::EventEmpoweredShrapnelBlast, 15 * TimeConstants::IN_MILLISECONDS + 300);
                        }

                        break;
                    }
                    case eStages::StageFourKillSwitch:
                    {
                        m_EmpoweredShrapnels = 0;

                        events.ScheduleEvent(eEvents::EventSever, 7 * TimeConstants::IN_MILLISECONDS + 700);
                        events.ScheduleEvent(eEvents::EventSleepCanister, 7 * TimeConstants::IN_MILLISECONDS + 300);
                        events.ScheduleEvent(eEvents::EventEmpoweredShrapnelBlast, 15 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventChargedBlasts, 10 * TimeConstants::IN_MILLISECONDS + 600);
                        break;
                    }
                    case eStages::StageFiveThePerfectWeapon:
                    {
                        m_EmpoweredShrapnels = 0;

                        events.ScheduleEvent(eEvents::EventSleepCanister, 7 * TimeConstants::IN_MILLISECONDS + 300);
                        events.ScheduleEvent(eEvents::EventEmpoweredShockLance, 4 * TimeConstants::IN_MILLISECONDS + 300);
                        events.ScheduleEvent(eEvents::EventEmpoweredPulseGrenade, 6 * TimeConstants::IN_MILLISECONDS + 800);
                        events.ScheduleEvent(eEvents::EventEmpoweredShrapnelBlast, 15 * TimeConstants::IN_MILLISECONDS + 700);
                        break;
                    }
                    default:
                        break;
                }

                m_RocketsTimer = 2 * TimeConstants::IN_MILLISECONDS;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_imonar_the_soulhunterAI(p_Creature);
        }
};

/// Imonar the Soulhunter (Ship) - 125692
class npc_imonar_ship : public CreatureScript
{
    public:
        npc_imonar_ship() : CreatureScript("npc_imonar_ship") { }

        enum eSpells
        {
            SpellBlastingBeam   = 250186
        };

        struct npc_imonar_shipAI : public ScriptedAI
        {
            npc_imonar_shipAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Talk(0);

                    Position l_Pos = me->GetPosition();

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 360.0f, l_Pos.m_orientation, true);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, l_Pos, false);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 1:
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Position l_Pos = { -10303.65f, 8869.139f, 1914.706f, 0.02098464f };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(2, l_Pos, false);
                        });

                        break;
                    }
                    case 2:
                    {
                        AddTimedDelayedOperation(20 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();

                            DoCast(me, eSpells::SpellBlastingBeam, true);

                            Movement::MoveSplineInit l_Init(me);

                            l_Init.SetFly();
                            l_Init.SetSmooth();
                            l_Init.SetUncompressed();
                            l_Init.SetCyclic();

                            std::vector<G3D::Vector3> l_Path =
                            {
                                { -10308.51f, 8857.303f, 1958.076f },
                                { -10844.39f, 8850.083f, 1856.335f },
                                { -10794.27f, 8858.842f, 1925.930f },
                                { -10303.65f, 8869.139f, 1914.706f }
                            };

                            for (G3D::Vector3 const& l_Point : l_Path)
                                l_Init.Path().push_back(l_Point);

                            l_Init.Launch();
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_SpellID == eSpells::SpellBlastingBeam && p_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    me->DelayedCastSpell(me, eSpells::SpellBlastingBeam, true, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_imonar_shipAI(p_Creature);
        }
};

/// Traps! - 124686
class npc_imonar_traps : public CreatureScript
{
    public:
        npc_imonar_traps() : CreatureScript("npc_imonar_traps") { }

        enum eSpells
        {
            /// Pulse Grenade
            SpellPulseGrenadeAT5mn  = 247397,
            SpellPulseGrenadeAT2mn  = 247668,
            SpellPulseGrenadeAoE    = 247388,
            /// Blastwire
            SpellBlastwireAT        = 247955,
            /// Stasis Trap
            SpellStasisTrapAT       = 247637,
            /// Shrapnel Blast
            SpellShrapnelBlastAT    = 247944
        };

        enum eActions
        {
            ActionPulseGrenade,
            ActionBlastwire,
            ActionStasisTrap,
            ActionShrapnelBlast
        };

        struct npc_imonar_trapsAI : public Scripted_NoMovementAI
        {
            npc_imonar_trapsAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_IsAvailable = true;

            uint32 m_SpellID = 0;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DoAction(int32 const p_Action) override
            {
                if (!m_IsAvailable)
                    return;

                m_IsAvailable = false;

                switch (p_Action)
                {
                    case eActions::ActionPulseGrenade:
                    {
                        if (me->GetDBTableGUIDLow() > 0)
                            m_SpellID = eSpells::SpellPulseGrenadeAT2mn;
                        else
                            m_SpellID = eSpells::SpellPulseGrenadeAT5mn;

                        break;
                    }
                    case eActions::ActionBlastwire:
                    {
                        m_SpellID = eSpells::SpellBlastwireAT;
                        break;
                    }
                    case eActions::ActionStasisTrap:
                    {
                        m_SpellID = eSpells::SpellStasisTrapAT;
                        break;
                    }
                    case eActions::ActionShrapnelBlast:
                    {
                        m_SpellID = eSpells::SpellShrapnelBlastAT;
                        break;
                    }
                    default:
                        break;
                }

                DoCast(me, m_SpellID, true);
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                p_AreaTrigger->SetDuration(-1);
            }

            void AreaTriggerDespawned(AreaTrigger* /*p_AreaTrigger*/, bool /*p_Removed*/) override
            {
                /// Don't despawn static traps
                if (me->GetDBTableGUIDLow() > 0)
                {
                    m_IsAvailable = true;

                    /// Count triggered traps for Achievement
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (Creature* l_Imonar = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossImonarTheSoulhunter)))
                        {
                            if (l_Imonar->IsAIEnabled)
                                l_Imonar->AI()->SetData(0, 0);
                        }
                    }

                    me->DelayedRemoveAura(m_SpellID, 1);
                    return;
                }

                me->DespawnOrUnsummon(1);
            }

            uint32 GetData(uint32 /*p_ID = 0*/) override
            {
                return m_IsAvailable;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_imonar_trapsAI(p_Creature);
        }
};

/// Garothi Worldbreaker - 128154
class npc_imonar_garothi_worldbreaker : public CreatureScript
{
    public:
        npc_imonar_garothi_worldbreaker() : CreatureScript("npc_imonar_garothi_worldbreaker") { }

        enum eSpells
        {
            SpellVentsVisualAura = 254277
        };

        struct npc_imonar_garothi_worldbreakerAI : public Scripted_NoMovementAI
        {
            npc_imonar_garothi_worldbreakerAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                DoCast(me, eSpells::SpellVentsVisualAura, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_imonar_garothi_worldbreakerAI(p_Creature);
        }
};

/// Shrapnel Blast - 124776
class npc_imonar_shrapnel_blast : public CreatureScript
{
    public:
        npc_imonar_shrapnel_blast() : CreatureScript("npc_imonar_shrapnel_blast") { }

        enum eSpells
        {
            SpellShrapnelBlastAT = 247939
        };

        struct npc_imonar_shrapnel_blastAI : public Scripted_NoMovementAI
        {
            npc_imonar_shrapnel_blastAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellShrapnelBlastAT, true);
            }

            void AreaTriggerDespawned(AreaTrigger* /*p_AreaTrigger*/, bool /*p_Removed*/) override
            {
                me->RemoveAura(eSpells::SpellShrapnelBlastAT);

                me->DespawnOrUnsummon(31 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_imonar_shrapnel_blastAI(p_Creature);
        }
};

/// Bombing Run - 124704
class npc_imonar_bombing_run : public CreatureScript
{
    public:
        npc_imonar_bombing_run() : CreatureScript("npc_imonar_bombing_run") { }

        enum eSpells
        {
            SpellLegionShipSpawn        = 251446,

            SpellChargedBlastsSearcher  = 247711,
            SpellChargedBlastsCast      = 247704,
            SpellChargedBlastsMarker    = 257197,
            SpellChargedBlastsAura      = 248968,
            SpellChargedBlastsAT        = 247707,
            SpellChargedBlastDummy      = 247727,
            SpellChargedBlastsDamage    = 247716
        };

        struct npc_imonar_bombing_runAI : public Scripted_NoMovementAI
        {
            npc_imonar_bombing_runAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint64 m_TargetGuid = 0;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellLegionShipSpawn, true);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                switch (p_SpellID)
                {
                    case eSpells::SpellLegionShipSpawn:
                    {
                        if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                            break;

                        DoCast(me, eSpells::SpellChargedBlastsSearcher, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellChargedBlastsSearcher:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer())
                                return true;

                            Player* l_Player = p_Object->ToPlayer();
                            if (l_Player->IsMeleeDamageDealer() || l_Player->IsRangedDamageDealer())
                                return false;

                            return true;
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
                    case eSpells::SpellChargedBlastsSearcher:
                    {
                        m_TargetGuid = p_Target->GetGUID();

                        DoCast(p_Target, eSpells::SpellChargedBlastsCast);
                        DoCast(p_Target, eSpells::SpellChargedBlastsMarker, true);
                        DoCast(p_Target, eSpells::SpellChargedBlastsAura, true);

                        DoCast(me, eSpells::SpellChargedBlastsAT, true);
                        break;
                    }
                    case eSpells::SpellChargedBlastsCast:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            me->CastSpell(p_Target, eSpells::SpellChargedBlastDummy, true, nullptr, nullptr, l_Owner->GetGUID());
                            me->CastSpell(p_Target, eSpells::SpellChargedBlastsDamage, true, nullptr, nullptr, l_Owner->GetGUID());
                        }

                        me->DespawnOrUnsummon(1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (!m_TargetGuid)
                    return;

                if (Player* l_Player = Player::GetPlayer(*me, m_TargetGuid))
                    me->SetFacingTo(me->GetAngle(l_Player));
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_imonar_bombing_runAI(p_Creature);
        }
};

/// Artillery Strike - 124889
class npc_imonar_artillery_strike : public CreatureScript
{
    public:
        npc_imonar_artillery_strike() : CreatureScript("npc_imonar_artillery_strike") { }

        enum eSpells
        {
            SpellLegionShipVisual               = 251446,
            SpellEmpoweredShrapnelBlastCast     = 248076,
            SpellEmpoweredShrapnelBlastMissile  = 248077
        };

        struct npc_imonar_artillery_strikeAI : public Scripted_NoMovementAI
        {
            npc_imonar_artillery_strikeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellLegionShipVisual, true);
                DoCast(me, eSpells::SpellEmpoweredShrapnelBlastCast);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellEmpoweredShrapnelBlastCast:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            std::list<HostileReference*> l_ThreatList = l_Owner->getThreatManager().getThreatList();
                            for (HostileReference* l_Ref : l_ThreatList)
                            {
                                if (Unit* l_Target = l_Ref->getTarget())
                                {
                                    Player* l_Player = l_Target->ToPlayer();

                                    /// Can't be affected
                                    if (!l_Player)
                                        continue;

                                    DoCast(l_Player, eSpells::SpellEmpoweredShrapnelBlastMissile, true);
                                }
                            }
                        }

                        me->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_imonar_artillery_strikeAI(p_Creature);
        }
};

/// Conflagration - 124550
class npc_imonar_conflagration : public CreatureScript
{
    public:
        npc_imonar_conflagration() : CreatureScript("npc_imonar_conflagration") { }

        enum eSpells
        {
            SpellConflagrationSearcher  = 250136,
            SpellConflagrationVisual    = 248315,
            SpellConflagrationFireAT    = 250138,
            SpellConflagrationFelAT     = 248208
        };

        struct npc_imonar_conflagrationAI : public Scripted_NoMovementAI
        {
            npc_imonar_conflagrationAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_IsAvailable = true;
            bool m_Fire = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellConflagrationSearcher:
                    {
                        m_IsAvailable = false;

                        DoCast(me, eSpells::SpellConflagrationVisual, true);

                        if (m_Fire)
                            DoCast(me, eSpells::SpellConflagrationFireAT);
                        else
                            DoCast(me, eSpells::SpellConflagrationFelAT);

                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerDespawned(AreaTrigger* /*p_AreaTrigger*/, bool /*p_Removed*/) override
            {
                me->DespawnOrUnsummon(1);
            }

            uint32 GetData(uint32 /*p_ID = 0*/) override
            {
                return m_IsAvailable;
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                m_Fire = p_Value;
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_imonar_conflagrationAI(p_Creature);
        }
};

/// Blasting Beam - 253000
class spell_imonar_blasting_beam : public SpellScriptLoader
{
    public:
        spell_imonar_blasting_beam() : SpellScriptLoader("spell_imonar_blasting_beam") { }

        class spell_imonar_blasting_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_imonar_blasting_beam_SpellScript);

            std::array<Position, 10> m_PossibleDests =
            {
                {
                    { -10588.88f, 8987.613f, 1992.892f },
                    { -10598.26f, 8948.391f, 1964.978f },
                    { -10667.12f, 8979.469f, 1949.875f },
                    { -10569.50f, 8972.951f, 1954.421f },
                    { -10617.55f, 8981.068f, 1983.084f },
                    { -10626.31f, 8962.479f, 1952.006f },
                    { -10614.00f, 8977.157f, 1974.005f },
                    { -10576.10f, 8973.403f, 1967.867f },
                    { -10579.29f, 8992.635f, 1986.459f },
                    { -10572.63f, 8980.122f, 1979.200f }
                }
            };

            void HandleScript(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                if (Unit* l_Caster = GetCaster())
                {
                    uint32 l_Delay      = GetSpellInfo()->Effects[p_EffIndex].MiscValue;
                    uint32 l_SpellID    = GetSpellInfo()->Effects[p_EffIndex].TriggerSpell;

                    float l_Dist        = 1000000.0f;

                    Position* l_Pos;

                    /// Find nearest position
                    for (uint8 l_I = 0; l_I < 10; ++l_I)
                    {
                        float l_CurrDist = l_Caster->GetDistance(m_PossibleDests[l_I]);
                        if (l_CurrDist < l_Dist)
                        {
                            l_Dist  = l_CurrDist;
                            l_Pos   = &m_PossibleDests[l_I];
                        }
                    }

                    if (l_Delay)
                        l_Caster->DelayedCastSpell(l_Pos, l_SpellID, true, l_Delay);
                    else
                        l_Caster->CastSpell(*l_Pos, l_SpellID, true);
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_imonar_blasting_beam_SpellScript::HandleScript, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_imonar_blasting_beam_SpellScript::HandleScript, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_imonar_blasting_beam_SpellScript::HandleScript, SpellEffIndex::EFFECT_2, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_imonar_blasting_beam_SpellScript::HandleScript, SpellEffIndex::EFFECT_3, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_imonar_blasting_beam_SpellScript::HandleScript, SpellEffIndex::EFFECT_4, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_imonar_blasting_beam_SpellScript();
        }
};

/// Electrify - 250078
class spell_imonar_electrify : public SpellScriptLoader
{
    public:
        spell_imonar_electrify() : SpellScriptLoader("spell_imonar_electrify") { }

        enum eSpells
        {
            SpellShockLance     = 247367,
            SpellElectrifyDmg   = 247542
        };

        class spell_imonar_electrify_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_imonar_electrify_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::SpellShockLance))
                    {
                        uint8 l_Stacks = l_Aura->GetStackAmount();

                        if (l_Stacks && p_Targets.size() > l_Stacks)
                            JadeCore::Containers::RandomResizeList(p_Targets, l_Stacks);
                    }
                }
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (Aura* l_Aura = l_Caster->GetAura(eSpells::SpellShockLance))
                            l_Caster->CastSpell(l_Target, eSpells::SpellElectrifyDmg, true, nullptr, nullptr, l_Aura->GetCasterGUID());
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_imonar_electrify_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_imonar_electrify_SpellScript::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_imonar_electrify_SpellScript();
        }
};

/// Sleep Canister - 255029
/// Sleep Canister - 257196
class spell_imonar_sleep_canister : public SpellScriptLoader
{
    public:
        spell_imonar_sleep_canister() : SpellScriptLoader("spell_imonar_sleep_canister") { }

        enum eSpell
        {
            SpellSlumberGas = 247565
        };

        class spell_imonar_sleep_canister_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_imonar_sleep_canister_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetMap())
                    return;

                if (l_Caster->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidLFR)
                    p_Duration = 12 * TimeConstants::IN_MILLISECONDS;
                else
                    p_Duration = 20 * TimeConstants::IN_MILLISECONDS;
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE && l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->GetMap()->IsLFR())
                        l_Target->CastSpell(l_Target, eSpell::SpellSlumberGas, true);
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_imonar_sleep_canister_AuraScript::CalculateMaxDuration);
                AfterEffectRemove += AuraEffectRemoveFn(spell_imonar_sleep_canister_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_STUN, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_imonar_sleep_canister_AuraScript();
        }
};

/// Pulse Grenade - 247386
class spell_imonar_pulse_grenade : public SpellScriptLoader
{
    public:
        spell_imonar_pulse_grenade() : SpellScriptLoader("spell_imonar_pulse_grenade") { }

        enum eAction
        {
            ActionPulseGrenade
        };

        class spell_imonar_pulse_grenade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_imonar_pulse_grenade_SpellScript);

            void HandleSummon(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                if (Unit* l_Caster = GetCaster())
                {
                    if (Creature* l_Trap = l_Caster->SummonCreature(eCreatures::NpcTraps, *GetExplTargetDest()))
                    {
                        if (!l_Trap->IsAIEnabled)
                            return;

                        l_Trap->AI()->DoAction(eAction::ActionPulseGrenade);
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_imonar_pulse_grenade_SpellScript::HandleSummon, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_imonar_pulse_grenade_SpellScript();
        }
};

/// Charged Blasts - 247716
class spell_imonar_charged_blasts : public SpellScriptLoader
{
    public:
        spell_imonar_charged_blasts() : SpellScriptLoader("spell_imonar_charged_blasts") { }

        class spell_imonar_charged_blasts_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_imonar_charged_blasts_SpellScript);

            enum eSpell
            {
                TargetRestrict = 37424
            };

            std::set<uint64> m_Targets;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_Targets.empty())
                {
                    std::list<Player*> l_PlayerList;

                    l_Caster->GetPlayerListInGrid(l_PlayerList, 100.0f);

                    if (l_PlayerList.empty())
                        return;

                    SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                    if (l_Restriction == nullptr)
                        return;

                    l_PlayerList.remove_if([this, l_Caster, l_Restriction](Player* p_Player) -> bool
                    {
                        if (p_Player == nullptr)
                            return true;

                        if (!p_Player->IsInAxe(l_Caster, l_Restriction->Width / 2.0f, 100.0f))
                            return true;

                        return false;
                    });

                    p_Targets.clear();

                    for (Player* l_Player : l_PlayerList)
                    {
                        m_Targets.insert(l_Player->GetGUID());

                        p_Targets.push_back(l_Player);
                    }
                }
                else
                {
                    p_Targets.clear();

                    for (uint64 const& l_Guid : m_Targets)
                    {
                        if (Player* l_Player = Player::GetPlayer(*l_Caster, l_Guid))
                            p_Targets.push_back(l_Player);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_imonar_charged_blasts_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_ENEMY_BETWEEN_DEST);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_imonar_charged_blasts_SpellScript::FilterTargets, SpellEffIndex::EFFECT_1, Targets::TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_imonar_charged_blasts_SpellScript();
        }
};

/// Empowered Electrify - 250256
class spell_imonar_empowered_electrify : public SpellScriptLoader
{
    public:
        spell_imonar_empowered_electrify() : SpellScriptLoader("spell_imonar_empowered_electrify") { }

        enum eSpells
        {
            SpellEmpoweredShockLance    = 250255,
            SpellEmpoweredElectrifyDmg  = 250257
        };

        class spell_imonar_empowered_electrify_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_imonar_empowered_electrify_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::SpellEmpoweredShockLance))
                    {
                        uint8 l_Stacks = l_Aura->GetStackAmount();

                        if (l_Stacks && p_Targets.size() > l_Stacks)
                            JadeCore::Containers::RandomResizeList(p_Targets, l_Stacks);
                    }
                }
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                        l_Caster->CastSpell(l_Target, eSpells::SpellEmpoweredElectrifyDmg, true);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_imonar_empowered_electrify_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_imonar_empowered_electrify_SpellScript::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_imonar_empowered_electrify_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_imonar_the_soulhunter()
{
    /// Boss
    new boss_imonar_the_soulhunter();

    /// Creatures
    new npc_imonar_ship();
    new npc_imonar_traps();
    new npc_imonar_garothi_worldbreaker();
    new npc_imonar_shrapnel_blast();
    new npc_imonar_bombing_run();
    new npc_imonar_artillery_strike();
    new npc_imonar_conflagration();

    /// Spells
    new spell_imonar_blasting_beam();
    new spell_imonar_electrify();
    new spell_imonar_sleep_canister();
    new spell_imonar_pulse_grenade();
    new spell_imonar_charged_blasts();
    new spell_imonar_empowered_electrify();
}
#endif
