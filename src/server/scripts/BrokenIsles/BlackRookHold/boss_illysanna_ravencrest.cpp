////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "black_rook_hold_dungeon.hpp"

enum eEvents
{
    EVENT_BRUTAL_GLAIVE     = 1,
    EVENT_VENGEFUL_SHEAR    = 2,
    EVENT_DARK_RUSH_1       = 3,
    EVENT_DARK_RUSH_2       = 4,
    EVENT_SUMMON_ADDS       = 5,
    EVENT_EYE_BEAMS         = 6,
    EVENT_PHASE_1           = 7
};

enum eTalks
{
    Aggro,
    VengefulShear,
    DarkRush,
    SecondPhase,
    EyeBeams,
    PlayerKilled,
    Death
};

Position const g_SummonPos[2] =
{
    { 3105.11f, 7296.94f, 103.28f, 3.14f }, ///< NPC_RISEN_ARCANIST
    { 3083.07f, 7313.35f, 103.28f, 4.92f }  ///< NPC_SOUL_TORN_VANGUARD
};

Position const g_FlyingPosition = { 3100.95f, 7315.51f, 105.0016f, 4.1f };
Position const g_LandingPosition = { 3089.76f, 7299.66f, 103.53f, 4.1f };

/// Illysanna Ravencrest - 98696
class boss_illysanna_ravencrest : public CreatureScript
{
    public:
        boss_illysanna_ravencrest() : CreatureScript("boss_illysanna_ravencrest") { }

        struct boss_illysanna_ravencrestAI : public BossAI
        {
            boss_illysanna_ravencrestAI(Creature* p_Creature) : BossAI(p_Creature, eData::Illysanna)
            {
                m_Instance = me->GetInstanceScript();
            }

            enum eSpells
            {
                FuryPowerOverride   = 197367,
                RegenPower          = 197394,
                BrutalGlaive        = 197546,
                VengefulShear       = 197418,
                DarkRushFilter      = 197478,
                DarkRushCharge      = 197484,
                EyeBeams            = 197674,
                EyeBeamsAura        = 197696,
                BoneBreakingStrike  = 200261
            };

            bool m_IntroDone;
            bool m_PhaseTwo;

            bool m_IsCharging;
            std::deque<uint64> m_ChargeTargets;
            InstanceScript* m_Instance;

            bool m_HasTurned;

            void Reset() override
            {
                _Reset();
                me->SetPower(POWER_ENERGY, 100);
                DoCast(me, eSpells::FuryPowerOverride, true);
                DoCast(me, eSpells::RegenPower, true);
                me->SetReactState(REACT_AGGRESSIVE);
                m_PhaseTwo = false;
                me->SetAnimTier(0);
                me->SetAIAnimKitId(0);
                SetFlyMode(false);
                m_HasTurned = false;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                DefaultEvents();
                m_IsCharging = false;
                Talk(eTalks::Aggro);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                Talk(eTalks::Death);
            }

            void DefaultEvents()
            {
                events.ScheduleEvent(EVENT_BRUTAL_GLAIVE, 6000);
                events.ScheduleEvent(EVENT_VENGEFUL_SHEAR, 9000);
                events.ScheduleEvent(EVENT_DARK_RUSH_1, 12000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::VengefulShear:
                    {
                        if (!p_Target->IsActiveSaveAbility())
                            DoCast(p_Target, 197429, true);
                        break;
                    }
                    case eSpells::DarkRushCharge:
                    {
                        uint64 l_TargetGUID = p_Target->GetGUID();

                        if (me->IsMoving())
                            m_ChargeTargets.push_back(l_TargetGUID);
                        else
                            AreaTriggerAndCharge(l_TargetGUID);

                        m_IsCharging = true;

                        break;
                    }
                    case eSpells::EyeBeams:
                    {
                        Position l_Pos = p_Target->GetPosition();
                        if (Creature* l_Stalker = me->SummonCreature(eCreatures::EyeBeamStalker, l_Pos, TEMPSUMMON_TIMED_DESPAWN, 12000))
                            DoCast(l_Stalker, eSpells::EyeBeamsAura, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerAndCharge(uint64 l_TargetGUID)
            {
                Unit* p_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DarkRushCharge);
                if (!p_Target)
                    return;

                uint32 l_TotalDist = me->GetExactDist2d(p_Target);
                if (l_TotalDist < 4)
                    return;

                float l_StepX = 4.0f * std::cos(me->GetAngle(p_Target));
                float l_StepY = 4.0f * std::sin(me->GetAngle(p_Target));
                Position l_PositionTarget = *me;
                l_PositionTarget.SetOrientation(me->GetAngle(p_Target));
                for (uint8 l_Itr = 0; l_Itr * 4 < l_TotalDist; ++l_Itr)
                {
                    l_PositionTarget.m_positionX += l_StepX;
                    l_PositionTarget.m_positionY += l_StepY;
                    AreaTrigger* l_AreaTrigger = new AreaTrigger;
                    if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 5352, me, nullptr, l_SpellInfo, l_PositionTarget, l_PositionTarget))
                    {
                        delete l_AreaTrigger;
                        return;
                    }
                }

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveCharge(p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ(), 50.0f, eSpells::DarkRushCharge);
            }

            void OnContactPosition(Unit* p_Source, Position& p_Dest)
            {
                float l_X = 3096.7980f, l_Y = 7309.4711f, l_Z = 103.30f;

                float l_Dist = me->GetCombatReach() + 5.0f;

                me->GetContactPoint(p_Source, l_X, l_Y, l_Z, l_Dist);
                p_Dest.Relocate(l_X, l_Y, l_Z);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    if (p_ID == 2)
                    {
                        me->GetMotionMaster()->Clear(false);
                        me->SetFacingTo(4.1f);
                        me->SetAnimTier(3);
                        me->SetAIAnimKitId(9260);
                        me->SetReactState(REACT_PASSIVE);
                        events.ScheduleEvent(EVENT_SUMMON_ADDS, 1000);
                        events.ScheduleEvent(EVENT_EYE_BEAMS, 2000);
                    }
                    else if (p_ID == 1)
                    {
                        SetFlyMode(false);
                        me->SetAnimTier(0);
                        me->SetAIAnimKitId(0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(me, eSpells::RegenPower, true);
                        m_PhaseTwo = false;
                    }
                }
                else if (p_Type == POINT_MOTION_TYPE)
                {
                    if (p_ID == eSpells::DarkRushCharge)
                        events.ScheduleEvent(EVENT_DARK_RUSH_2, 500);
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (!m_PhaseTwo)
                {
                    Talk(eTalks::SecondPhase);
                    m_PhaseTwo = true;
                    events.Reset();
                    DoStopAttack();
                    me->RemoveAurasDueToSpell(eSpells::RegenPower);
                    SetFlyMode(true);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJump(g_FlyingPosition.m_positionX, g_FlyingPosition.m_positionY, g_FlyingPosition.m_positionZ, me->GetExactDist2d(g_FlyingPosition.m_positionX, g_FlyingPosition.m_positionY), 10.0f, me->GetOrientation(), 2);
                    me->SetReactState(REACT_PASSIVE);
                }
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return m_PhaseTwo;
            }

            void KilledUnit(Unit* p_Victim) override
            {
                ScriptedAI::KilledUnit(p_Victim);

                if (p_Victim->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!me->isInCombat() && !m_HasTurned)
                {
                    if (m_Instance && m_Instance->instance)
                    for (Map::PlayerList::const_iterator itr = m_Instance->instance->GetPlayers().begin(); itr != m_Instance->instance->GetPlayers().end(); ++itr)
                    {
                        if (Player* l_Player = itr->getSource())
                        {
                            if (me->IsWithinLOSInMap(l_Player))
                            {
                                me->SetFacingToObject(l_Player);
                                m_HasTurned = true;
                                break;
                            }
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 35.0f && !m_IsCharging)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (m_IsCharging)
                {
                    if (!me->IsMoving())
                    {
                        if (!m_ChargeTargets.empty())
                        {
                            AreaTriggerAndCharge(m_ChargeTargets.front());
                            m_ChargeTargets.pop_front();
                        }
                        else
                        {
                            m_IsCharging = false;
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            AddTimedDelayedOperation(100, [this]() -> void
                            {
                                if (Unit* l_Victim = me->getVictim())
                                    AttackStart(l_Victim, true);
                            });
                        }
                    }
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_BRUTAL_GLAIVE:
                    {
                        Unit* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank);
                            DoCast(l_Target, eSpells::BrutalGlaive);

                        events.ScheduleEvent(EVENT_BRUTAL_GLAIVE, 14000);
                        break;
                    }
                    case EVENT_VENGEFUL_SHEAR:
                    {
                        Talk(eTalks::VengefulShear);
                        DoCastVictim(eSpells::VengefulShear);
                        events.ScheduleEvent(EVENT_VENGEFUL_SHEAR, 14000);
                        break;
                    }
                    case EVENT_DARK_RUSH_1:
                    {
                        Talk(eTalks::DarkRush);
                        DoCast(eSpells::DarkRushFilter);
                        events.ScheduleEvent(EVENT_DARK_RUSH_1, 30000);
                        events.ScheduleEvent(EVENT_DARK_RUSH_2, 3000);
                        break;
                    }
                    case EVENT_DARK_RUSH_2:
                    {
                        DoCast(me, eSpells::DarkRushCharge, true);
                        break;
                    }
                    case EVENT_SUMMON_ADDS:
                    {
                        if (IsHeroicOrMythic())
                            me->SummonCreature(eCreatures::RisenArcanist, g_SummonPos[0]);
                        me->SummonCreature(eCreatures::SoulTornVanguard, g_SummonPos[1]);
                        break;
                    }
                    case EVENT_EYE_BEAMS:
                    {
                        Talk(eTalks::EyeBeams);
                        me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) - 33);
                        DoCast(eSpells::EyeBeams);
                        if (me->GetPower(POWER_ENERGY) <= 0)
                            events.ScheduleEvent(EVENT_PHASE_1, 14000);
                        else
                            events.ScheduleEvent(EVENT_EYE_BEAMS, 12000);
                        break;
                    }
                    case EVENT_PHASE_1:
                    {
                        DefaultEvents();
                        DoCast(me, eSpells::FuryPowerOverride, true);
                        me->GetMotionMaster()->MoveJump(g_LandingPosition.m_positionX, g_LandingPosition.m_positionY, g_LandingPosition.m_positionZ, me->GetExactDist2d(g_LandingPosition.m_positionX, g_LandingPosition.m_positionY), 10.0f, me->GetOrientation(), 1);
                        me->SetReactState(REACT_AGGRESSIVE);
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
            return new boss_illysanna_ravencrestAI(p_Creature);
        }
};

/// Illysanna Ravencrest - 100436
class npc_illysanna_eye_beam_stalker : public CreatureScript
{
    public:
        npc_illysanna_eye_beam_stalker() : CreatureScript("npc_illysanna_eye_beam_stalker") { }

        struct npc_illysanna_eye_beam_stalkerAI : public ScriptedAI
        {
            npc_illysanna_eye_beam_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                FixateBeam = 197687
            };

            uint64 m_TargetGuid;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 500);
                events.ScheduleEvent(EVENT_2, 1000);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (Creature* l_Summoner = l_Owner->ToCreature())
                            {
                                if (l_Summoner->IsAIEnabled)
                                {
                                    if (Unit* l_Target = l_Summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                                    {
                                        m_TargetGuid = l_Target->GetGUID();
                                        me->CastSpell(l_Target, eSpells::FixateBeam, true);
                                    }
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = ObjectAccessor::GetUnit(*me, m_TargetGuid))
                            me->GetMotionMaster()->MovePoint(1, l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ());
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_illysanna_eye_beam_stalkerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dark Rush - 197484
class spell_illysanna_dark_rush : public SpellScriptLoader
{
    public:
        spell_illysanna_dark_rush() : SpellScriptLoader("spell_illysanna_dark_rush") { }

        class spell_illysanna_dark_rush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_illysanna_dark_rush_SpellScript);

            enum eSpells
            {
                DarkRushMarker = 197478
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                std::list<Unit*> l_TargetList;
                float l_Radius = 50.0f;
                JadeCore::AnyUnitHavingBuffInObjectRangeCheck l_Ucheck(l_Caster, l_Caster, 50.0f, eSpells::DarkRushMarker, false);
                JadeCore::UnitListSearcher<JadeCore::AnyUnitHavingBuffInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Ucheck);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                if (l_TargetList.empty())
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void HandleAreatriggers(SpellEffIndex p_SpellEffectIndex)
            {

            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_illysanna_dark_rush_SpellScript::HandleCheckCast);
                OnEffectLaunch += SpellEffectFn(spell_illysanna_dark_rush_SpellScript::HandleAreatriggers, EFFECT_1, SPELL_EFFECT_CHARGE_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_illysanna_dark_rush_SpellScript();
        }
};

/// Periodic Energize - 197394
class spell_illysanna_periodic_energize : public SpellScriptLoader
{
    public:
        spell_illysanna_periodic_energize() : SpellScriptLoader("spell_illysanna_periodic_energize") { }

        class spell_illysanna_periodic_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_illysanna_periodic_energize_AuraScript);

            uint8 m_PowerCount = 0;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Creature* l_Caster = GetCaster() ? GetCaster()->ToCreature() : nullptr;
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                m_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (m_PowerCount < 100)
                    l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 1);
                else
                {
                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_illysanna_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_illysanna_periodic_energize_AuraScript();
        }
};

/// Eye Beams - 197696
class spell_illysanna_eye_beams : public SpellScriptLoader
{
    public:
        spell_illysanna_eye_beams() : SpellScriptLoader("spell_illysanna_eye_beams") { }

        class spell_illysanna_eye_beams_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_illysanna_eye_beams_AuraScript);

            enum eSpells
            {
                EyeBeamsAreatrigger = 197703,
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::EyeBeamsAreatrigger, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_illysanna_eye_beams_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_illysanna_eye_beams_AuraScript();
        }
};

/// Vengeful Shear - 197418
class spell_illysanna_vengeful_shear : public SpellScriptLoader
{
    public:
        spell_illysanna_vengeful_shear() : SpellScriptLoader("spell_illysana_vengeful_shear")
        {}

        enum eSpells
        {
            DemonSpikes             = 203819,
            Ironfur                 = 192081,
            IronskinBrew            = 215479,
            ShieldOfTheRighteous    = 132403,
            ShieldBlock             = 132404,
            BloodShield             = 77535,
            DeathStrike             = 180612,
            VengefulShearDebuff     = 197429,
        };

        class spell_illysanna_vengeful_shear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_illysanna_vengeful_shear_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();

                std::vector<uint32> l_Auras =
                {
                    eSpells::DemonSpikes,
                    eSpells::Ironfur,
                    eSpells::IronskinBrew,
                    eSpells::ShieldOfTheRighteous,
                    eSpells::ShieldBlock,
                    eSpells::DeathStrike,
                    eSpells::BloodShield,
                };

                for (const uint32 & l_Itr : l_Auras)
                {
                    if (l_Target->HasAura(l_Itr))
                        return;
                }

                l_Caster->CastSpell(l_Target, eSpells::VengefulShearDebuff, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_illysanna_vengeful_shear_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_illysanna_vengeful_shear_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Rook Spiderling - 98677
class npc_rook_spiderling : public CreatureScript
{
    public:
        npc_rook_spiderling() : CreatureScript("npc_rook_spiderling") { }

        struct npc_rook_spiderlingAI : public ScriptedAI
        {
            npc_rook_spiderlingAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                InternalRupture = 225917
            };

            void Reset() override { }

            void JustDied(Unit* p_Killer) override
            {
                if (!IsMythic())
                    return;

                me->CastSpell(me, eSpells::InternalRupture, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rook_spiderlingAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Internal rupture - 225917
class spell_brh_internal_rupture: public SpellScriptLoader
{
    public:
        spell_brh_internal_rupture() : SpellScriptLoader("spell_brh_internal_rupture") { }

        class spell_brh_internal_rupture_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brh_internal_rupture_SpellScript);

            enum eSpells
            {
                SoulVenomDot = 225909
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SoulVenomDot, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_brh_internal_rupture_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brh_internal_rupture_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Risen Arcanists - 98280
class npc_risen_arcanist : public CreatureScript
{
    public:
        npc_risen_arcanist() : CreatureScript("npc_risen_arcanist") { }

        struct npc_risen_arcanistAI : public ScriptedAI
        {
            npc_risen_arcanistAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            enum eSpells
            {
                ArcaneBlitz = 200248
            };

            void Reset() override { }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_Instance)
                    return;

                uint64 l_Commander = m_Instance->GetData64(eCreatures::CommanderShemdahSohn);
                Creature* l_Creature = Creature::GetCreature(*me, l_Commander);
                if (!l_Creature)
                    return;

                UnitAI* l_CreatureAI = l_Creature->GetAI();
                if (!l_CreatureAI)
                    return;

                l_CreatureAI->SetGUID(me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                {
                    EnterEvadeMode();
                    return;
                }

                if (me->GetDistance2d(l_Victim) > 15)
                {
                    me->GetMotionMaster()->MoveChase(l_Victim);
                    return;
                }

                AttackStart(l_Victim, false);

                me->CastSpell(l_Victim, eSpells::ArcaneBlitz, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_risen_arcanistAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Arcane Minion - 101549
class npc_arcane_minion : public CreatureScript
{
    public:
        npc_arcane_minion() : CreatureScript("npc_arcane_minion") { }

        struct npc_arcane_minionAI : public ScriptedAI
        {
            npc_arcane_minionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                PhasedExplosion = 200256
            };

            void Reset() override { }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                {
                    EnterEvadeMode();
                    return;
                }

                if (me->GetDistance2d(l_Victim) > 15)
                {
                    me->GetMotionMaster()->MoveChase(l_Victim);
                    return;
                }

                AttackStart(l_Victim, false);

                me->CastSpell(l_Victim, eSpells::PhasedExplosion, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcane_minionAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Risen Archers - 98275
class npc_risen_archer : public CreatureScript
{
    public:
        npc_risen_archer() : CreatureScript("npc_risen_archer") { }

        struct npc_risen_archerAI : public ScriptedAI
        {
            npc_risen_archerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            enum eSpells
            {
                Shoot = 193633,
                ArrowBarrage = 200345
            };

            int32 m_ArrowBarrageTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_ArrowBarrageTimer = 20 * IN_MILLISECONDS;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_Instance)
                    return;

                uint64 l_Commander = m_Instance->GetData64(eCreatures::CommanderShemdahSohn);
                Creature* l_Creature = Creature::GetCreature(*me, l_Commander);
                if (!l_Creature)
                    return;

                UnitAI* l_CreatureAI = l_Creature->GetAI();
                if (!l_CreatureAI)
                    return;

                l_CreatureAI->SetGUID(me->GetGUID());
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::ArrowBarrage:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
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

                if (!me->isInCombat())
                    return;

                m_ArrowBarrageTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                {
                    EnterEvadeMode();
                    return;
                }

                if (me->GetDistance2d(l_Victim) > 15)
                {
                    me->GetMotionMaster()->MoveChase(l_Victim);
                    return;
                }

                if (m_ArrowBarrageTimer < 0)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    me->CastSpell(me, eSpells::ArrowBarrage, false);
                    m_ArrowBarrageTimer = 22 * IN_MILLISECONDS;
                    return;
                }

                AttackStart(l_Victim, false);

                me->CastSpell(l_Victim, eSpells::Shoot, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_risen_archerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Risen Companion - 101839
class npc_risen_companion : public CreatureScript
{
    public:
        npc_risen_companion() : CreatureScript("npc_risen_companion") { }

        struct npc_risen_companionAI : public ScriptedAI
        {
            npc_risen_companionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                BloodthirstyLeap = 225962
            };

            int32 m_BloothirstyLeapTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_BloothirstyLeapTimer = 8 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_BloothirstyLeapTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_BloothirstyLeapTimer < 0)
                {
                    Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0,100.0f, true);
                    if (!l_Victim)
                        return;

                    me->CastSpell(l_Victim, eSpells::BloodthirstyLeap, false);
                    m_BloothirstyLeapTimer = 8 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_risen_companionAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Risen Scout - 98691
class npc_risen_scout : public CreatureScript
{
    public:
        npc_risen_scout() : CreatureScript("npc_risen_scout") { }

        struct npc_risen_scoutAI : public ScriptedAI
        {
            npc_risen_scoutAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            enum eSpells
            {
                KnifeDance = 200291
            };

            int32 m_KnifeDanceTimer;

            void Reset() override { }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_Instance)
                    return;

                uint64 l_Commander = m_Instance->GetData64(eCreatures::CommanderShemdahSohn);
                Creature* l_Creature = Creature::GetCreature(*me, l_Commander);
                if (!l_Creature)
                    return;

                UnitAI* l_CreatureAI = l_Creature->GetAI();
                if (!l_CreatureAI)
                    return;

                l_CreatureAI->SetGUID(me->GetGUID());
            }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_KnifeDanceTimer = 18 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_KnifeDanceTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_KnifeDanceTimer < 0)
                {
                    me->CastSpell(me, eSpells::KnifeDance, false);
                    m_KnifeDanceTimer = 20 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_risen_scoutAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Soul-Torn Champion - 98243
class npc_soul_torn_champion : public CreatureScript
{
    public:
        npc_soul_torn_champion() : CreatureScript("npc_soul_torn_champion") { }

        struct npc_soul_torn_championAI : public ScriptedAI
        {
            npc_soul_torn_championAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                BonebreakingStrike = 200261
            };

            int32 m_BonebreakingStrike;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_BonebreakingStrike = 15 * IN_MILLISECONDS;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::BonebreakingStrike:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
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

                if (!me->isInCombat())
                    return;

                m_BonebreakingStrike -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_BonebreakingStrike < 0)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    me->CastSpell(me, eSpells::BonebreakingStrike, false);
                    m_BonebreakingStrike = 20 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_soul_torn_championAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Commander Shemdah'Sohn - 98706
class npc_commander_shemdah_sohn : public CreatureScript
{
    public:
        npc_commander_shemdah_sohn() : CreatureScript("npc_commander_shemdah_sohn") { }

        struct npc_commander_shemdah_sohnAI : public ScriptedAI
        {
            npc_commander_shemdah_sohnAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eSpells
            {
                BonebreakingStrike = 200261
            };

            enum eTalks
            {
                Ambush
            };

            int32 m_BonebreakingStrike;
            InstanceScript* m_Instance;
            std::set<uint64> m_DownStairAlive;
            std::set<uint64> m_UpStairAlive;
            bool m_AmbushTriggered = false;

            std::vector<Position> l_JumpTargets;
            Position l_CommanderTarget;

            void Reset() override
            {
                ScriptedAI::Reset();

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::RisenArcanist, 30.0f);
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::RisenScout, 30.0f);
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::RisenArcher, 30.0f);

                    for (Creature* l_Creature : l_CreatureList)
                    {
                        if (l_Creature->isAlive() && l_Creature->GetPositionZ() < me->GetPositionZ())
                        {
                            l_JumpTargets.push_back(l_Creature->GetPosition());
                            m_DownStairAlive.insert(l_Creature->GetGUID());
                        }
                    }

                    l_CreatureList.clear();
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::RisenArcanist, 45.0f);
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::RisenScout, 45.0f);
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::RisenArcher, 45.0f);

                    for (Creature* l_Creature : l_CreatureList)
                    {
                        if (l_Creature->isAlive() && l_Creature->GetPositionZ() > me->GetPositionZ())
                            m_UpStairAlive.insert(l_Creature->GetGUID());
                    }

                    m_UpStairAlive.insert(me->GetGUID());

                });

                l_CommanderTarget = Position(3115.5f, 7335.2f, 86.3f, 0.94f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                m_UpStairAlive.erase(me->GetGUID());
                HandleDoors();
            }

            void HandleDoors()
            {
                if (!m_Instance || !m_UpStairAlive.empty())
                    return;

                GameObject* l_GobLeft = GameObject::GetGameObject(*me, m_Instance->GetData64(eGameObjects::AmbushEventDoorLeft));
                GameObject* l_GobRight = GameObject::GetGameObject(*me, m_Instance->GetData64(eGameObjects::AmbushEventDoorRight));
                if (!l_GobLeft || !l_GobRight)
                    return;

                l_GobLeft->SetGoState(GO_STATE_ACTIVE);
                l_GobRight->SetGoState(GO_STATE_ACTIVE);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                m_DownStairAlive.erase(p_Guid);
                m_UpStairAlive.erase(p_Guid);
                HandleDoors();
            }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_BonebreakingStrike = 15 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_AmbushTriggered)
                {
                    if (!m_DownStairAlive.empty() || !m_Instance || !l_JumpTargets.size())
                        return;

                    m_AmbushTriggered = true;

                    Talk(eTalks::Ambush);
                    AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void
                    {
                        uint32 l_I = 0;
                        for (uint64 l_GUID : m_UpStairAlive)
                        {
                            Creature* l_Creature = Creature::GetCreature(*me, l_GUID);
                            if (!l_Creature)
                                continue;

                            Position l_Dest;
                            if (l_I >= l_JumpTargets.size())
                                l_I = 0;

                            l_Dest = l_JumpTargets[l_I];

                            l_Creature->GetMotionMaster()->Clear();
                            l_Creature->GetMotionMaster()->MoveJump(l_Dest, 30.0f, 10.0f);
                            l_Creature->SetHomePosition(l_Dest);
                            l_I++;

                            AddTimedDelayedOperation(1500, [l_Creature]() -> void
                            {
                                if (!l_Creature->IsAIEnabled)
                                    return;

                                if (Player* l_Player = l_Creature->FindNearestPlayer(30.0f))
                                    l_Creature->AI()->AttackStart(l_Player, l_Creature->GetEntry() != eCreatures::RisenArcher);
                            });
                        }

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(l_CommanderTarget, 30.0f, 10.0f);
                        me->SetHomePosition(l_CommanderTarget);

                        AddTimedDelayedOperation(1500, [this]() -> void
                        {
                            if (Player* l_Player = me->FindNearestPlayer(30.0f))
                                AttackStart(l_Player, true);
                        });

                        return;
                    });
                }

                if (!UpdateVictim())
                    return;

                m_BonebreakingStrike -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_BonebreakingStrike < 0)
                {
                    me->CastSpell(me, eSpells::BonebreakingStrike, false);
                    m_BonebreakingStrike = 20 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_commander_shemdah_sohnAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_illysanna_ravencrest()
{
    /// Boss
    new boss_illysanna_ravencrest();

    /// Creatures
    new npc_illysanna_eye_beam_stalker();

    /// Spells
    new spell_illysanna_periodic_energize();
    new spell_illysanna_eye_beams();
    new spell_illysanna_dark_rush();

    /// Miniboss
    new npc_commander_shemdah_sohn();

    /// Trashes
    new npc_rook_spiderling();
    new npc_risen_arcanist();
    new npc_arcane_minion();
    new npc_risen_archer();
    new npc_risen_companion();
    new npc_risen_scout();
    new npc_soul_torn_champion();

    new spell_brh_internal_rupture();
}
#endif
