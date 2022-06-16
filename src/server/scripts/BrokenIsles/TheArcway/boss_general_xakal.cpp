////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"

 enum eTalks
{
    Aggro           = 1,
    Fel             = 2,
    Summon          = 3,
    Wicked          = 4,
    WickedText      = 5,
    Death           = 6
};

enum eSpells
{
    FelFissure              = 197776,
    FelFissureSummon        = 197765,
    FelFissureAreatrigger   = 197542,
    FelFissureGameobject    = 197573,
    ShadowSlash             = 212028,
    ShadowSlashAreatrigger  = 212030,
    WickedSlam              = 197810,
    WakeOfShadow            = 220441,

    /// Felbat
    BombardmentAura         = 197786,
    BombardmentDamage       = 197787,
    ThirstForBlood          = 220533
};

enum eEvents
{
    EventFelFissure         = 1,
    EventShadowSlash        = 2,
    EventWickedSlam         = 3,
    EventSummonFelbat       = 4
};

enum eActions
{
    ActionStartPath         = 1
};

/// General Xakal - 98206
class boss_general_xakal : public CreatureScript
{
    public:
        boss_general_xakal() : CreatureScript("boss_general_xakal") { }

        struct boss_general_xakalAI : public BossAI
        {
            boss_general_xakalAI(Creature* p_Creature) : BossAI(p_Creature, DATA_XAKAL) { }

            uint8 m_ShadowDist = 0;
            uint16 m_ShadowTimer = 0;
            Position m_ShadowPos;
            float m_Angle;
            float m_Direction;

            void Reset() override
            {
                _Reset();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventFelFissure, 5000);
                events.ScheduleEvent(eEvents::EventShadowSlash, 13000);
                events.ScheduleEvent(eEvents::EventWickedSlam, 35000);
                events.ScheduleEvent(eEvents::EventSummonFelbat, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);
                _JustDied();
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::FelFissure:
                    {
                        DoCast(p_Target, eSpells::FelFissureSummon, true);
                        break;
                    }
                    case eSpells::ShadowSlash:
                    {
                        Position l_Pos;
                        Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM);
                        m_Angle = me->GetRelativeAngle(l_Target);
                        me->GetNearPosition(l_Pos, 1.0f, m_Angle);
                        l_Pos.SimplePosXYRelocationByAngle(m_ShadowPos, 1.0f, m_Angle);
                        DoCast(l_Target, eSpells::ShadowSlashAreatrigger);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_ShadowTimer)
                {
                    if (m_ShadowTimer <= p_Diff)
                    {
                        m_ShadowDist += 5;
                        m_ShadowTimer = 500;
                        me->SetFacingTo(m_Direction);
                        me->SetOrientation(m_Direction);
                        Position l_Pos;
                        m_ShadowPos.SimplePosXYRelocationByAngle(l_Pos, m_ShadowDist, m_Angle);
                        me->CastSpell(l_Pos, eSpells::WakeOfShadow, true);
                        if (m_ShadowDist > 60)
                            m_ShadowTimer = 0;
                        me->SetReactState(REACT_AGGRESSIVE);
                    }
                    else
                        m_ShadowTimer -= p_Diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFelFissure:
                    {
                        DoCast(eSpells::FelFissure);
                        Talk(eTalks::Fel);
                        events.ScheduleEvent(eEvents::EventFelFissure, 23000);
                        break;
                    }
                    case eEvents::EventShadowSlash:
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM);
                        if (!l_Target)
                            return;

                        me->SetReactState(REACT_PASSIVE);
                        m_ShadowDist = 0;
                        m_ShadowTimer = 2000;
                        m_Direction = me->GetOrientation();
                        me->CastSpell(l_Target, eSpells::ShadowSlash);
                        events.ScheduleEvent(eEvents::EventShadowSlash, 25000);
                        break;
                    }
                    case eEvents::EventWickedSlam:
                    {
                        Talk(eTalks::Wicked);
                        Talk(eTalks::WickedText);
                        DoCast(eSpells::WickedSlam);
                        events.ScheduleEvent(eEvents::EventWickedSlam, 47000);
                        break;
                    }
                    case eEvents::EventSummonFelbat:
                    {
                        Talk(eTalks::Summon);
                        Position l_Pos_First;
                        Position l_Pos_Second;

                        /// First Felbat
                        me->GetRandomNearPosition(l_Pos_First, 30.0f);
                        me->SummonCreature(NPC_DREAD_FELBAT, l_Pos_First.GetPositionX(), l_Pos_First.GetPositionY(), l_Pos_First.GetPositionZ() + 20.0f);

                        /// Second Felbat
                        me->GetRandomNearPosition(l_Pos_Second, 30.0f);
                        me->SummonCreature(NPC_DREAD_FELBAT, l_Pos_Second.GetPositionX(), l_Pos_Second.GetPositionY(), l_Pos_Second.GetPositionZ() + 20.0f);

                        events.ScheduleEvent(eEvents::EventSummonFelbat, 31000);
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
            return new boss_general_xakalAI(p_Creature);
        }
};

/// Fissure - 100342
class npc_xakal_fissure : public CreatureScript
{
    public:
        npc_xakal_fissure() : CreatureScript("npc_xakal_fissure") { }

        struct npc_xakal_fissureAI : public ScriptedAI
        {
            npc_xakal_fissureAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            }

            bool m_Active;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::FelFissureAreatrigger, true);
            }

            void OnAreaTriggerCast(Unit* /*p_Caster*/, Unit* /*p_Target*/, uint32 /*p_SpellID*/) override
            {
                if (!m_Active)
                {
                    m_Active = true;
                    DoCast(me, eSpells::FelFissureGameobject, true);
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xakal_fissureAI(p_Creature);
        }
};

/// Dread Felbat - 100393
class npc_xakal_dread_felbat : public CreatureScript
{
    public:
        npc_xakal_dread_felbat() : CreatureScript("npc_xakal_dread_felbat") { }

        struct npc_xakal_dread_felbatAI : public ScriptedAI
        {
            npc_xakal_dread_felbatAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            std::vector<G3D::Vector3> g_Path_Fel_Bats_Xakal =
            {
                { 3305.682f, 4502.035f, 615.783f },
                { 3287.946f, 4539.245f, 615.783f },
                { 3311.332f, 4528.422f, 615.783f },
                { 3347.396f, 4543.653f, 611.773f }
            };

            void Reset() override { }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case TheArcwayMoves::eMoves::FelBatPath:
                        me->GetMotionMaster()->Clear();
                        events.ScheduleEvent(EVENT_1, 500);
                        break;
                    default:
                        break;
                }
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 120.0f);
                DoCast(me, eSpells::BombardmentAura, true);
                events.ScheduleEvent(EVENT_3, 1000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        events.ScheduleEvent(EVENT_2, 5000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCastVictim(eSpells::ThirstForBlood);
                        events.ScheduleEvent(EVENT_2, 5000);
                        break;
                    }
                    case EVENT_3:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(TheArcwayMoves::eMoves::FelBatPath, g_Path_Fel_Bats_Xakal.data(), g_Path_Fel_Bats_Xakal.size());
                        /// I know, shitty hardcoded but MovementInform didnt get triggered, dont ask me why
                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            events.ScheduleEvent(EVENT_1, 500);
                        });
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
            return new npc_xakal_dread_felbatAI(p_Creature);
        }
};

/// Bombardment - 197786
class spell_xakal_bombardment : public SpellScriptLoader
{
    public:
        spell_xakal_bombardment() : SpellScriptLoader("spell_xakal_bombardment") { }

        class spell_xakal_bombardment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xakal_bombardment_AuraScript);

            void OnPeriodic(AuraEffect const* p_AurEff)
            {
                Creature* l_Caster = GetCaster() ? GetCaster()->ToCreature() : nullptr;
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                if (Unit* l_Target = l_Caster->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    l_Caster->CastSpell(l_Target, eSpells::BombardmentDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xakal_bombardment_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xakal_bombardment_AuraScript();
        }
};

/// Trash 211950
uint32 g_RandomSpells[7] =
{
    211995,
    211959,
    211999,
    211956,
    211958,
    211994,
    212008
};

/// Throw Stuff Primer - 211950
class spell_throw_staff : public SpellScriptLoader
{
    public:
        spell_throw_staff() : SpellScriptLoader("spell_throw_staff") { }

        class spell_throw_staff_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_throw_staff_SpellScript);

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* l_Target = l_Caster->FindNearestPlayer(30.0f))
                    l_Caster->CastSpell(l_Target, g_RandomSpells[urand(0, 7)]);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_throw_staff_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_throw_staff_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_general_xakal()
{
    /// Boss
    new boss_general_xakal();

    /// Creatures
    new npc_xakal_fissure();
    new npc_xakal_dread_felbat();

    /// Spells
    new spell_xakal_bombardment();
    new spell_throw_staff();
}
#endif
