////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "neltharions_lair.hpp"

enum Says
{
    SAY_SUM     = 0,
    SAY_EMOTE   = 1
};

enum Spells
{
    SPELL_INTRO_MYSTIC          = 209625, ///< Boss 03 Intro Mystic Cast
    SPELL_INTRO_EMERGE          = 209641, ///< Boss 03 Intro Emerge - hit npc 105766
    SPELL_INTRO_CONVERSATION    = 209629,
    SPELL_GAIN_ENERGY           = 200086,
    SPELL_PUTRID_SKIES          = 198963,
    SPELL_FRENZY                = 199775,
    SPELL_RANCID_MAW            = 205549,
    SPELL_RANCID_MAW_DOT        = 224136,
    SPELL_TOXIC_WRETCH          = 210150,
    SPELL_TOXIC_WRETCH_AT       = 210159,
    SPELL_TOXIC_WRETCH_AOE      = 217828,
    SPELL_TOXIC_WRETCH_DOT      = 217851,
    SPELL_SPIKED_TONGUE_CHANNEL = 199178,
    SPELL_SPIKED_TONGUE_RIDE    = 205417,
    SPELL_SPIKED_TONGUE         = 199176,
    SPELL_SPIKED_TONGUE_PULL    = 199178,
    SPELL_SPIKED_TONGUE_DMG     = 199705,
    SPELL_SPIKED_TONGUE_JUMP    = 204136,
    SPELL_RAVENOUS              = 199246,
    SPELL_FANATIC_SACRIFICE     = 209902,
    SPELL_DEVOUR_FANATIC        = 216797,

    /// Heroic
    SPELL_CALL_ANGRY_CROWD      = 217028,

    /// Tresh
    SPELL_JUMP_VISUAL           = 184483,
    SPELL_FANATICS_SACRIFICE    = 209906,

    SPELL_HURLING_ROCKS         = 199245,

    SPELL_BONUS_ROLL            = 226642
};

enum eEvents
{
    EVENT_RANCID_MAW        = 1,
    EVENT_TOXIC_WRETCH      = 2,
    EVENT_SUM_WORMSPEAKER   = 3,
    EVENT_PLR_OUT           = 4,
    EVENT_MELEE_CHECKER,

    /// Heroic
    EVENT_CALL_ANGRY_CROWD
};

enum eAchievements
{
    CantEatJustOne      = 10875
};

Position const g_SpeakerSpawnPos[2] =
{
    { 3045.138f, 1807.521f, -44.24927f, 3.545784f },
    { 3048.908f, 1799.710f, -45.51458f, 3.352974f }
};

Position const g_SpeakerJumpPos[2] =
{
    { 3033.458f, 1791.019f, -61.25781f, 3.545784f },
    { 3024.776f, 1802.295f, -60.10603f, 3.352974f }
};

Position const g_SpeakerSacrificePos[2] =
{
    { 3009.1760f, 1825.3792f, -60.41782f, 3.068341f },
    { 2996.1443f, 1808.9387f, -61.33771f, 1.902030f }
};

std::array<Position const, 6> g_AngryCrowdPos =
{
    {
        { 3040.564f, 1817.040f, -41.99748f, 3.767707f },
        { 3047.784f, 1800.060f, -34.60545f, 3.451150f },
        { 3034.438f, 1814.540f, -32.08216f, 3.767707f },
        { 3051.378f, 1787.255f, -45.60298f, 3.45115f },
        { 3047.895f, 1804.575f, -46.44583f, 3.45115f },
        { 3048.608f, 1794.196f, -44.99854f, 3.45115f }
    }
};

/// Naraxas - 91005
class boss_naraxas : public CreatureScript
{
    public:
        boss_naraxas() : CreatureScript("boss_naraxas") { }

        struct boss_naraxasAI : public BossAI
        {
            boss_naraxasAI(Creature* p_Creature) : BossAI(p_Creature, DATA_NARAXAS)
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                SetCombatMovement(false);
                me->SetMaxPower(POWER_MANA, 100);
                m_IntroDone = false;
                m_RavenousStack = 0;
                DoCast(me, SPELL_INTRO_MYSTIC, true);
            }

            bool m_BerserkActive;
            bool m_IntroDone;
            uint16 m_CheckMeleeTimer;
            uint8 m_RavenousStack;
            uint64 m_TargetGuid;
            std::set<uint64> m_Players;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CheckAchievement()
            {
                if (!IsMythic())
                    return false;

                if (m_RavenousStack >= 6)
                    return true;

                return false;
            }

            void Reset() override
            {
                SetFlyMode(true);

                if (!m_IntroDone)
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->RemoveAllAreasTrigger();

                _Reset();
                summons.DespawnAll();
                me->RemoveAurasDueToSpell(SPELL_GAIN_ENERGY);
                me->RemoveAurasDueToSpell(SPELL_FRENZY);
                me->RemoveAurasDueToSpell(SPELL_RAVENOUS);
                me->SetPower(POWER_MANA, 0);
                m_CheckMeleeTimer = 2000;
                m_BerserkActive = false;
                m_RavenousStack = 0;

                for (Position const l_Pos : g_AngryCrowdPos)
                {
                    if (Creature* l_Crowd = me->SummonCreature(NPC_ANGRY_CROWD, l_Pos))
                        l_Crowd->SetReactState(REACT_PASSIVE);
                }

                m_TargetGuid = 0;

                me->SetReactState(REACT_AGGRESSIVE);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                DoCast(me, SPELL_GAIN_ENERGY, true);

                for (Map::PlayerList::const_iterator l_Itr = me->GetMap()->GetPlayers().begin(); l_Itr != me->GetMap()->GetPlayers().end(); ++l_Itr)
                {
                    Player* l_Player = l_Itr->getSource();
                    if (!l_Player)
                        continue;

                    m_Players.insert(l_Player->GetGUID());
                }

                events.ScheduleEvent(EVENT_RANCID_MAW, 8000);
                events.ScheduleEvent(EVENT_TOXIC_WRETCH, 12000);
                events.ScheduleEvent(EVENT_SUM_WORMSPEAKER, 7000);
                events.ScheduleEvent(eEvents::EVENT_MELEE_CHECKER, 2 * IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_CALL_ANGRY_CROWD, 5500);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->SetHover(true);

                if (CheckAchievement())
                {
                    AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::CantEatJustOne);
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

                _JustDied();

                if (instance)
                {
                    if (Unit* l_Navarogg = sObjectAccessor->GetUnit(*me, instance->GetData64(NPC_NAVAROGG)))
                    {
                        if (l_Navarogg->IsAIEnabled)
                            l_Navarogg->GetAI()->DoAction(5);
                    }

                    if (Unit* l_Ebonhorn = sObjectAccessor->GetUnit(*me, instance->GetData64(NPC_SPIRITWALKER_EBONHORN)))
                    {
                        if (l_Ebonhorn->IsAIEnabled)
                            l_Ebonhorn->GetAI()->DoAction(3);
                    }
                }

                me->ClearLootContainers();

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void RegeneratePower(Powers /*p_Power*/, int32& p_Value) override
            {
                p_Value = 0;
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_SpellID == Spells::SPELL_SPIKED_TONGUE_CHANNEL)
                    me->SetReactState(REACT_AGGRESSIVE);
            }

            void PowerModified(Powers p_Power, int32 p_Value) override
            {
                if (p_Value >= 100 && p_Power == Powers::POWER_MANA)
                {
                    if (me->getVictim() == nullptr)
                        return;

                    Unit* l_Victim = me->getVictim();

                    me->SetReactState(REACT_PASSIVE);
                    me->StopAttack();

                    events.DelayEvents(12 * IN_MILLISECONDS);

                    me->CastSpell(l_Victim, Spells::SPELL_SPIKED_TONGUE, false);
                    Talk(Says::SAY_EMOTE);
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || m_IntroDone || p_Who->GetDistance(me) > 75.0f)
                    return;

                if (Creature* l_Target = me->FindNearestCreature(NPC_BLIGHTSHARD_SHAPER, 30.0f))
                {
                    m_IntroDone = true;

                    me->CastSpell(l_Target, SPELL_INTRO_EMERGE, false);
                    me->RemoveAurasDueToSpell(SPELL_INTRO_MYSTIC);
                    me->CastSpell(me, SPELL_INTRO_CONVERSATION, true);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(20, p_Damage) && !m_BerserkActive)
                {
                    m_BerserkActive = true;
                    DoCast(me, SPELL_FRENZY, true);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_TOXIC_WRETCH:
                    {
                        DoCast(me, SPELL_TOXIC_WRETCH_AOE, true);

                        Position l_Pos;
                        float l_Dist;

                        for (uint8 l_I = 0; l_I < 12; l_I++)
                        {
                            l_Dist = frand(10.0f, 30.0f);
                            me->GetNearPosition(l_Pos, l_Dist, frand(-(3.0f * M_PI / 8.0f), (3.0f * M_PI / 8.0f)));
                            me->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), SPELL_TOXIC_WRETCH_AT, true);
                        }
                        break;
                    }
                    case SPELL_FANATIC_SACRIFICE:
                    {
                        DoCast(p_Caster, SPELL_DEVOUR_FANATIC);
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
                    case SPELL_TOXIC_WRETCH_AOE:
                    {
                        DoCast(p_Target, SPELL_TOXIC_WRETCH_DOT, true);
                        break;
                    }
                    case SPELL_SPIKED_TONGUE:
                    {
                        me->ClearUnitState(UnitState::UNIT_STATE_CASTING);
                        DoCast(p_Target, SPELL_SPIKED_TONGUE_PULL);
                        me->SetPower(POWER_MANA, 0);
                        break;
                    }
                    case SPELL_CALL_ANGRY_CROWD:
                    {
                        p_Target->CastSpell(p_Target, SPELL_HURLING_ROCKS, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void PassengerBoarded(Unit* p_Who, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply)
                    DoCast(p_Who, SPELL_SPIKED_TONGUE_DMG, true);
                else
                {
                    if (!p_Who->IsPlayer())
                        p_Who->Kill(p_Who);
                    else
                    {
                        m_TargetGuid = p_Who->GetGUID();
                        events.ScheduleEvent(EVENT_PLR_OUT, 200);
                    }

                    DoCast(me, SPELL_RAVENOUS, true);
                    m_RavenousStack++;
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EVENT_RANCID_MAW:
                    {
                        DoCast(SPELL_RANCID_MAW);
                        events.ScheduleEvent(EVENT_RANCID_MAW, 18000);
                        break;
                    }

                    case eEvents::EVENT_TOXIC_WRETCH:
                    {
                        DoCast(SPELL_TOXIC_WRETCH);
                        events.ScheduleEvent(EVENT_TOXIC_WRETCH, 14000);
                        break;
                    }

                    case eEvents::EVENT_SUM_WORMSPEAKER:
                    {
                        Talk(SAY_SUM);

                        for (uint8 l_I = 0; l_I < 2; l_I++)
                        {
                            if (Creature* l_Speaker = me->SummonCreature(NPC_WORMSPEAKER_DEVOUT, g_SpeakerSpawnPos[l_I]))
                            {
                                if (l_Speaker->IsAIEnabled)
                                    l_Speaker->AI()->SetData(0, l_I);
                            }
                        }

                        events.ScheduleEvent(EVENT_SUM_WORMSPEAKER, 64000);
                        break;
                    }

                    case eEvents::EVENT_PLR_OUT:
                    {
                        Position l_Pos;
                        me->GetNearPosition(l_Pos, 20.0f, 0.0f);
                        l_Pos.m_positionZ += 10.0f;
                        if (Player* l_Player = Player::GetPlayer(*me, m_TargetGuid))
                            l_Player->CastSpell(l_Pos, SPELL_SPIKED_TONGUE_JUMP, true);
                        break;
                    }

                    case eEvents::EVENT_CALL_ANGRY_CROWD:
                    {
                        events.ScheduleEvent(EVENT_CALL_ANGRY_CROWD, 75000);
                        DoCast(SPELL_CALL_ANGRY_CROWD);
                        break;
                    }

                    case eEvents::EVENT_MELEE_CHECKER:
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            if (!me->IsWithinMeleeRange(l_Victim))
                                DoCast(SPELL_PUTRID_SKIES);
                        }
                        events.ScheduleEvent(eEvents::EVENT_MELEE_CHECKER, 2 * IN_MILLISECONDS);
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

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_naraxasAI(p_Creature);
        }
};

/// Wormspeaker Devout - 101075
class npc_naraxas_wormspeaker_devout : public CreatureScript
{
    public:
        npc_naraxas_wormspeaker_devout() : CreatureScript("npc_naraxas_wormspeaker_devout") {}

        struct npc_naraxas_wormspeaker_devoutAI : public ScriptedAI
        {
            npc_naraxas_wormspeaker_devoutAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Index = 2;

                p_Creature->SetReactState(REACT_PASSIVE);

                switch (p_Creature->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyNormal:
                        p_Creature->SetMaxHealth(727487);
                        break;
                    case Difficulty::DifficultyHeroic:
                        p_Creature->SetMaxHealth(1454974);
                        break;
                    case Difficulty::DifficultyMythic:
                        p_Creature->SetMaxHealth(1818717);
                        break;
                    default:
                        break;
                }

                p_Creature->SetFullHealth();

                m_MovementWasStopped = true;
            }

            uint32 m_Index;

            bool m_MovementWasStopped;

            void Reset() override
            {
                events.Reset();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_BATTLEROAR);
                });
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 1000);
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                m_Index = p_Value;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0: ///< Slow applied
                    {
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != EFFECT_MOTION_TYPE && p_Type != POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 0:
                    {
                        me->RemoveAurasDueToSpell(SPELL_JUMP_VISUAL);
                        events.ScheduleEvent(EVENT_2, 1000);
                        break;
                    }
                    case 1:
                    {
                        if (!me->IsNearPosition(&g_SpeakerSacrificePos[m_Index], 0.5f))
                        {
                            m_MovementWasStopped = true;
                            break;
                        }

                        Talk(0);
                        DoCast(me, SPELL_FANATICS_SACRIFICE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() && me->isInCombat())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (m_Index >= 2)
                            break;

                        DoCast(me, SPELL_JUMP_VISUAL, true);
                        me->GetMotionMaster()->MoveJump(g_SpeakerJumpPos[m_Index], 30.0f, 15.0f, 0);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (m_Index >= 2)
                            break;

                        if (!me->HasBreakableByDamageCrowdControlAura() && m_MovementWasStopped)
                        {
                            m_MovementWasStopped = false;
                            me->GetMotionMaster()->MovePoint(1, g_SpeakerSacrificePos[m_Index]);
                        }

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
            return new npc_naraxas_wormspeaker_devoutAI(p_Creature);
        }
};

/// Spiked Tongue (channeled) - 199178
class spell_naraxas_spiked_tongue_periodic : public SpellScriptLoader
{
    public:
        spell_naraxas_spiked_tongue_periodic() : SpellScriptLoader("spell_naraxas_spiked_tongue_periodic") { }

        class spell_naraxas_spiked_tongue_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_naraxas_spiked_tongue_periodic_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Target = GetTarget()->ToPlayer())
                        l_Target->SendApplyMovementForce(l_Caster->GetGUID(), true, *l_Caster, 14.0f, 1, G3D::Vector3(-8.03966f, 11.46141f, 0.003943613f));
                }
            }

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (l_Target->GetDistance2d(l_Caster) <= 2.f)
                {
                    l_Caster->InterruptNonMeleeSpells(true);
                    l_Caster->CastSpell(l_Target, Spells::SPELL_SPIKED_TONGUE_RIDE, true);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Target = GetTarget()->ToPlayer())
                        l_Target->SendApplyMovementForce(l_Caster->GetGUID(), false, *l_Caster);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_naraxas_spiked_tongue_periodic_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_naraxas_spiked_tongue_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_naraxas_spiked_tongue_periodic_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_naraxas_spiked_tongue_periodic_AuraScript();
        }
};

/// Rancid Maw (Devout debuff) - 205609
class spell_naraxas_rancid_maw_debuff : public SpellScriptLoader
{
    public:
        spell_naraxas_rancid_maw_debuff() : SpellScriptLoader("spell_naraxas_rancid_maw_debuff") { }

        class spell_naraxas_rancid_maw_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_naraxas_rancid_maw_debuff_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Creature* l_Devout = GetTarget()->ToCreature())
                {
                    if (l_Devout->IsAIEnabled)
                        l_Devout->AI()->DoAction(0);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Creature* l_Devout = GetTarget()->ToCreature())
                {
                    if (l_Devout->IsAIEnabled)
                        l_Devout->AI()->DoAction(0);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_naraxas_rancid_maw_debuff_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_naraxas_rancid_maw_debuff_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_naraxas_rancid_maw_debuff_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_naraxas()
{
    /// Boss
    new boss_naraxas();

    /// Creature
    new npc_naraxas_wormspeaker_devout();

    /// Spell
    new spell_naraxas_spiked_tongue_periodic();
    new spell_naraxas_rancid_maw_debuff();
}
#endif
