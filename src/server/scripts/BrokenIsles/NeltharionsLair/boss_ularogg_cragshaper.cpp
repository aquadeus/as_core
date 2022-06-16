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
    SAY_AGGRO,
    SAY_STANCE_EMOTE,
    SAY_DEATH,
    SAY_STRIKE,
    SAY_STANCE
};

enum Spells
{
    SPELL_SUNDER                    = 198496,
    SPELL_SUNDER_CALL               = 198823, ///< Conversation
    SPELL_STRIKE_MOUNTAIN           = 198428,
    SPELL_STRIKE_MOUNTAIN_2         = 216290,
    SPELL_STRIKE_MOUNTAIN_AT        = 216292,
    SPELL_MOUNTAIN_CALL             = 198825, ///< Conversation
    SPELL_BELLOW_DEEPS_1            = 193375,
    SPELL_BELLOW_DEEPS_2            = 193376,
    SPELL_BELLOW_DEEPS_CALL         = 198824,
    SPELL_STANCE_MOUNTAIN_JUMP      = 198509,
    SPELL_STANCE_MOUNTAIN_SUM       = 198565,
    SPELL_STANCE_MOUNTAIN_SUM_02    = 198564,
    SPELL_STANCE_MOUNTAIN_SUM_03    = 216250,
    SPELL_STANCE_MOUNTAIN_SUM_04    = 216249,
    SPELL_STANCE_MOUNTAIN_MORPH     = 198510,
    SPELL_STANCE_MOUNTAIN_TICK      = 198617,
    SPELL_FALLING_DEBRIS_ULAROGG    = 198719, ///< Boss
    SPELL_STANCE_MOUNTAIN_END       = 198631,

    SPELL_FALLING_DEBRIS            = 193267, ///< Npc 98081
    SPELL_FALLING_DEBRIS_2          = 198717, ///< Npc 100818
    SPELL_STANCE_MOUNTAIN_MOVE      = 198616,

    SPELL_INTRO_JUMP                = 217233,
    SPELL_INTRO_JUMP_KNOCK_BACK     = 217234,

    SPELL_BONUS_ROLL                = 226641
};

enum eEvents
{
    EVENT_SUNDER            = 1,
    EVENT_STRIKE_MOUNTAIN   = 2,
    EVENT_BELLOW_DEEPS_1    = 3,
    EVENT_BELLOW_DEEPS_2    = 4,
    EVENT_STANCE_MOUNTAIN_1 = 5
};

enum eIdolData
{
    IdolRight,
    IdolLeft,
    IdolBack,   ///< Only in heroic/mythic
    IdolFront,  ///< Only in heroic/mythic
    IdolCenter,

    IdolMax
};

std::array<Position const, 5> g_IdolSummonPos =
{
    {
        { 2842.439f, 1660.349f, -40.74621f, 3.6477380f },
        { 2834.184f, 1677.193f, -40.74133f, 3.6128320f },
        { 2831.690f, 1665.480f, -40.70417f, 3.6128320f },
        { 2844.770f, 1672.620f, -40.75297f, 0.5934119f },
        { 2838.145f, 1667.873f, -40.74132f, 3.0717790f }
    }
};

/// Ularogg Cragshaper - 91004
class boss_ularogg_cragshaper : public CreatureScript
{
    public:
        boss_ularogg_cragshaper() : CreatureScript("boss_ularogg_cragshaper") { }

        struct boss_ularogg_cragshaperAI : public BossAI
        {
            boss_ularogg_cragshaperAI(Creature* p_Creature) : BossAI(p_Creature, DATA_ULAROGG), m_CheckPlayerTimer(500) { }

            uint64 m_StanceGuid;
            bool m_LastStance;
            bool m_PhaseMountain;

            uint32 m_CheckPlayerTimer;

            uint64 m_TargetGuid;

            bool m_CanTargetOutOfLOS;

            void Reset() override
            {
                m_CanTargetOutOfLOS = false;
                m_TargetGuid = 0;
                _Reset();
                m_PhaseMountain = false;
                me->SetReactState(REACT_AGGRESSIVE);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);

                if (IsChallengeMode())
                    me->AddAura(ChallengeSpells::ChallengersMight, me);
            }

            bool CanTargetOutOfLOS() override
            {
                return m_CanTargetOutOfLOS;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return m_CanTargetOutOfLOS;
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_CheckPlayerTimer = 0;

                Talk(SAY_AGGRO);
                _EnterCombat();
                DefaultEvent(true);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                /// MoveChase is called right after EnterCombat, this mist be delayed
                uint64 l_Guid = p_Attacker->GetGUID();
                AddTimedDelayedOperation(10, [this, l_Guid]() -> void
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                    Position l_Pos = { 2838.145f, 1667.873f, -40.82465f, 3.324304f };
                    if (!me->IsNearPosition(&l_Pos, 2.0f))
                    {
                        m_CanTargetOutOfLOS = true;

                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(l_Pos, 40.0f, 20.0f, SPELL_INTRO_JUMP);
                    }
                    else
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                            AttackStart(l_Player);
                    }
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case SPELL_STANCE_MOUNTAIN_JUMP:
                    {
                        me->GetMotionMaster()->Clear();
                        me->AttackStop();
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->StopMoving();

                        me->CastSpell(g_IdolSummonPos[eIdolData::IdolRight], SPELL_STANCE_MOUNTAIN_SUM_02, false);

                        AddTimedDelayedOperation(500, [this]() -> void
                        {
                            if (InstanceScript* l_Instance = me->GetInstanceScript())
                                l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1 | eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });

                        break;
                    }
                    case SPELL_INTRO_JUMP:
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                        m_CanTargetOutOfLOS = false;

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_ROAR);
                        });

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->ClearUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                            me->SetHomePosition({ 2838.145f, 1667.873f, -40.82465f, 3.324304f });

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            if (Player* l_Player = Player::GetPlayer(*me, m_TargetGuid))
                                AttackStart(l_Player);
                        });

                        me->CastSpell(me, SPELL_INTRO_JUMP_KNOCK_BACK, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DefaultEvent(bool p_EnterCombat)
            {
                events.ScheduleEvent(EVENT_SUNDER, 8000);
                events.ScheduleEvent(EVENT_STRIKE_MOUNTAIN, 16000);
                events.ScheduleEvent(EVENT_BELLOW_DEEPS_1, 20000);
                events.ScheduleEvent(EVENT_STANCE_MOUNTAIN_1, p_EnterCombat ? 50000 : 120000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case 198619: ///< Stance of the Mountain
                    {
                        Position l_Pos;
                        me->GetRandomNearPosition(l_Pos, 30.0f);
                        p_Target->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), SPELL_STANCE_MOUNTAIN_MOVE, true);
                        break;
                    }
                    case SPELL_STRIKE_MOUNTAIN_2:
                    {
                        Position l_Pos  = p_Target->GetPosition();
                        float l_Radius  = 5.0f;
                        float l_Angle   = p_Target->m_orientation;

                        Position l_Dest = l_Pos;
                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            l_Pos.SimplePosXYRelocationByAngle(l_Dest, l_Radius, l_Angle, true);

                            l_Dest.m_orientation = l_Dest.GetAngle(p_Target) + (3.0f * M_PI / 2.0f);

                            p_Target->CastSpell(l_Dest, SPELL_STRIKE_MOUNTAIN_AT, true);

                            l_Angle += M_PI / 2.0f;

                            l_Angle = Position::NormalizeOrientation(l_Angle);
                        }

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
                    /// First spawn on right spot
                    case SPELL_STANCE_MOUNTAIN_SUM_02:
                    {
                        /// Second spawn on left spot (not in normal)
                        if (IsHeroicOrMythic())
                            me->CastSpell(g_IdolSummonPos[eIdolData::IdolBack], SPELL_STANCE_MOUNTAIN_SUM_04, true);

                        AddTimedDelayedOperation(1500, [this]() -> void
                        {
                            me->CastSpell(g_IdolSummonPos[eIdolData::IdolLeft], SPELL_STANCE_MOUNTAIN_SUM, false);

                            if (IsHeroicOrMythic())
                                me->CastSpell(g_IdolSummonPos[eIdolData::IdolFront], SPELL_STANCE_MOUNTAIN_SUM_03, true);
                        });

                        break;
                    }
                    /// Last Idol summoned
                    case SPELL_STANCE_MOUNTAIN_SUM:
                    {
                        AddTimedDelayedOperation(700, [this]() -> void
                        {
                            me->RemoveNegativeAuras();

                            me->CastSpell(me, SPELL_STANCE_MOUNTAIN_MORPH, false);
                        });

                        break;
                    }
                    case SPELL_STANCE_MOUNTAIN_MORPH:
                    {
                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_LastStance = true;
                            me->CastSpell(me, SPELL_STANCE_MOUNTAIN_TICK, true);
                            me->SummonCreature(NPC_BELLOWING_IDOL_2, g_IdolSummonPos[eIdolData::IdolCenter]);
                        });

                        break;
                    }
                    case SPELL_STANCE_MOUNTAIN_TICK:
                    {
                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            for (uint64 l_Guid : summons)
                            {
                                if (Creature* l_Idol = Creature::GetCreature(*me, l_Guid))
                                {
                                    if (l_Idol->GetEntry() != NPC_BELLOWING_IDOL_2)
                                        continue;

                                    if (l_Idol->IsAIEnabled)
                                        l_Idol->AI()->DoAction(1);
                                }
                            }

                            me->AddAura(SPELL_FALLING_DEBRIS_ULAROGG, me);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetGUID() == m_StanceGuid)
                {
                    me->RemoveAllAuras();
                    if (IsChallengeMode())
                        me->AddAura(ChallengeSpells::ChallengersMight, me);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1 | eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoCast(me, SPELL_STANCE_MOUNTAIN_END, true);
                    DefaultEvent(false);

                    if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                    {
                        AttackStart(l_Target);

                        /// Necessary, AttackStart can fail if target is already the victim
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(l_Target);
                    }

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == NPC_BELLOWING_IDOL_2 && m_LastStance)
                {
                    m_LastStance = false;
                    m_StanceGuid = p_Summon->GetGUID();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_CheckPlayerTimer)
                {
                    if (m_CheckPlayerTimer <= p_Diff)
                    {
                        if (Player* l_Target = me->FindNearestPlayer(25.0f))
                        {
                            m_TargetGuid = l_Target->GetGUID();

                            AttackStart(l_Target);

                            m_CheckPlayerTimer = 0;
                            return;
                        }

                        m_CheckPlayerTimer = 500;
                    }
                    else
                        m_CheckPlayerTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_SUNDER:
                    {
                        if (me->getVictim())
                        {
                            DoCast(me, SPELL_SUNDER_CALL, true);
                            DoCastVictim(SPELL_SUNDER);
                            events.ScheduleEvent(EVENT_SUNDER, 10000);
                        }
                        else
                            events.ScheduleEvent(EVENT_SUNDER, 1);

                        break;
                    }
                    case EVENT_STRIKE_MOUNTAIN:
                    {
                        Talk(SAY_STRIKE);
                        DoCast(SPELL_STRIKE_MOUNTAIN);
                        events.ScheduleEvent(EVENT_STRIKE_MOUNTAIN, 16000);
                        break;
                    }
                    case EVENT_BELLOW_DEEPS_1:
                    {
                        DoCast(SPELL_BELLOW_DEEPS_1);
                        events.ScheduleEvent(EVENT_BELLOW_DEEPS_1, 32000);
                        events.ScheduleEvent(EVENT_BELLOW_DEEPS_2, 3000);
                        break;
                    }
                    case EVENT_BELLOW_DEEPS_2:
                    {
                        DoCast(me, SPELL_BELLOW_DEEPS_CALL, true);
                        DoCast(SPELL_BELLOW_DEEPS_2);
                        break;
                    }
                    case EVENT_STANCE_MOUNTAIN_1:
                    {
                        events.Reset();
                        DoStopAttack();
                        me->CastSpell(g_IdolSummonPos[eIdolData::IdolCenter], SPELL_STANCE_MOUNTAIN_JUMP, true);
                        Talk(SAY_STANCE_EMOTE);
                        Talk(SAY_STANCE);
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
            return new boss_ularogg_cragshaperAI(p_Creature);
        }
};

/// Bellowing Idol - 98081, 100818
class npc_ularogg_bellowing_idols : public CreatureScript
{
    public:
        npc_ularogg_bellowing_idols() : CreatureScript("npc_ularogg_bellowing_idols") { }

        struct npc_ularogg_bellowing_idolsAI : public ScriptedAI
        {
            npc_ularogg_bellowing_idolsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetEntry() == NPC_BELLOWING_IDOL)
                    DoCast(me, SPELL_FALLING_DEBRIS, true);
                else
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    DoCast(me, 198569, true); ///< Visual Spawn
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                DoCast(me, SPELL_FALLING_DEBRIS_2, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ularogg_bellowing_idolsAI(p_Creature);
        }
};

/// Avalanche - 183088
class spell_avalanche : public SpellScriptLoader
{
    public:
        spell_avalanche() : SpellScriptLoader("spell_avalanche") { }

        class spell_avalanche_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_avalanche_SpellScript);

            void HandleScript(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Player*> l_Targets;
                GetPlayerListInGrid(l_Targets, l_Caster, 40);

                JadeCore::Containers::RandomResizeList(l_Targets, 2);

                for (Player* l_Iter : l_Targets)
                    l_Caster->CastSpell((*l_Iter), 183095, true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_avalanche_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_avalanche_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ularogg_cragshaper()
{
    /// Boss
    new boss_ularogg_cragshaper();

    /// Creature
    new npc_ularogg_bellowing_idols();

    /// Spell
    new spell_avalanche();
}
#endif
