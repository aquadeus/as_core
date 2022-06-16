////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "maw_of_souls.hpp"

enum Says
{
    SAY_INTRO,
    SAY_AGGRO,
    SAY_SUMMON,
    SAY_FRAGMENT,
    SAY_EMOTE_FRAGMENT,
    SAY_DEATH,
    SAY_WIPE,
    SAY_SLAY_PLAYER
};

enum Spells
{
    SPELL_COSMIC_SCYTHE             = 205330,
    SPELL_COSMIC_SCYTHE_VIS         = 198580,
    SPELL_COSMIC_SCYTHE_VIS_2       = 194667,
    SPELL_COSMIC_SCYTHE_DMG         = 194218,
    SPELL_SUM_SHACKLED_SERVITOR     = 194231,
    SPELL_FRAGMENT                  = 194325,

    /// Heroic
    SPELL_NETHER_RIP_AURA           = 194668,

    /// Summons
    SPELL_SHACKLED_SERVITOR         = 194259,
    SPELL_VOID_SNAP                 = 194266,
    SPELL_FRAGMENT_CLONE_PLAYER     = 194345,
    SPELL_FRAGMENT_PERIODIC         = 194344,
    SPELL_FRAGMENT_SCALE            = 194381,
    SPELL_FRAGMENT_DUMMY            = 194327,

    SPELL_NETHER_RIP_MISSILE        = 199457,

    SPELL_BONUS_ROLL                = 226638,

    SPELL_POOR_UNFORTUNATE_SOUL     = 213413
};

enum eEvents
{
    EVENT_COSMIC_SCYTHE             = 1,
    EVENT_SUM_SHACKLED_SERVITOR     = 2,
    EVENT_FRAGMENT                  = 3,
    EVENT_NETHER_RIP                = 4
};

/// Harbaron <The Ferryman> - 96754
class boss_harbaron : public CreatureScript
{
    public:
        boss_harbaron() : CreatureScript("boss_harbaron") { }

        struct boss_harbaronAI : public BossAI
        {
            boss_harbaronAI(Creature* p_Creature) : BossAI(p_Creature, DATA_HARBARON) { }

            bool m_IntroDone;
            std::vector<Position> mNetherRipTargetPos;

            void Reset() override
            {
                _Reset();
                me->DespawnCreaturesInArea(NPC_SOUL_FRAGMENT, 100.f);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);

                _EnterCombat();

                events.ScheduleEvent(EVENT_COSMIC_SCYTHE, 4000);
                events.ScheduleEvent(EVENT_SUM_SHACKLED_SERVITOR, 7000);
                events.ScheduleEvent(EVENT_FRAGMENT, 18000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_NETHER_RIP, 13000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                me->DespawnCreaturesInArea(eCreatures::NPC_SOUL_FRAGMENT);
                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);

                if (instance)
                    instance->DoCastSpellOnPlayers(SPELL_POOR_UNFORTUNATE_SOUL);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_COSMIC_SCYTHE:
                    {
                        Position l_Position = me->GetPosition();

                        float l_ScytheRange = -3.0f;

                        uint32 l_Time = 0;
                        for (uint8 l_I = 0; l_I < 6; ++l_I)
                        {
                            auto l_Summon = [this](Position p_Pos, float p_Range) -> void
                            {
                                Position l_Pos = p_Pos;

                                me->SimplePosXYRelocationByAngle(l_Pos, p_Range, p_Range < 0.0f ? me->m_orientation - M_PI : me->m_orientation, true);

                                l_Pos.m_positionZ += 1.0f;
                                l_Pos.m_orientation = Position::NormalizeOrientation(p_Pos.m_orientation + M_PI / 2.0f);

                                me->SummonCreature(NPC_COSMIC_SCYTHE_2, l_Pos);
                            };

                            if (l_Time)
                            {
                                AddTimedDelayedOperation(l_Time, [this, l_Summon, l_Position, l_ScytheRange]() -> void
                                {
                                    l_Summon(l_Position, l_ScytheRange);
                                });
                            }
                            else
                                l_Summon(l_Position, l_ScytheRange);

                            l_ScytheRange += 5.0f;
                            l_Time += 100;
                        }

                        break;
                    }
                    case SPELL_FRAGMENT:
                    {
                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                            p_Target->SummonCreature(NPC_SOUL_FRAGMENT, p_Target->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 120 * IN_MILLISECONDS);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SetNetherRip()
            {
                Position l_Pos = Position();
                if (mNetherRipTargetPos.empty())
                {
                    Position l_Dest = *me;
                    l_Dest.SimplePosXYRelocationByAngle(l_Pos, frand(3.f, 10.f), frand(0.0f, 2 * M_PI));
                }
                else
                {
                    uint8 ltr = urand(0, mNetherRipTargetPos.size()-1);
                    l_Pos = mNetherRipTargetPos[ltr];
                    mNetherRipTargetPos.erase(mNetherRipTargetPos.begin() + ltr);
                }

                DoCast(l_Pos, SPELL_NETHER_RIP_MISSILE, true);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 45.0f, true))
                {
                    m_IntroDone = true;
                    Talk(SAY_INTRO);
                }
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                Talk(SAY_WIPE);
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(SAY_SLAY_PLAYER);
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
                    case EVENT_COSMIC_SCYTHE:
                    {
                        DoCast(SPELL_COSMIC_SCYTHE);
                        events.ScheduleEvent(EVENT_COSMIC_SCYTHE, 8000);
                        break;
                    }
                    case EVENT_SUM_SHACKLED_SERVITOR:
                    {
                        Talk(SAY_SUMMON);

                        Unit* l_Target = SelectRangedTarget(true);

                        if (l_Target == nullptr)
                            l_Target = SelectMeleeTarget(false);

                        if (l_Target == nullptr)
                            l_Target = me->getVictim();

                        if (l_Target != nullptr)
                            DoCast(l_Target, SPELL_SUM_SHACKLED_SERVITOR);

                        events.ScheduleEvent(EVENT_SUM_SHACKLED_SERVITOR, 23000);
                        break;
                    }
                    case EVENT_FRAGMENT:
                    {
                        Talk(SAY_FRAGMENT);

                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me, true));

                        if (l_Target)
                            DoCast(l_Target, SPELL_FRAGMENT);
                        else
                        {
                            l_Target = me->getVictim();
                            DoCastVictim(SPELL_FRAGMENT);
                        }

                        if (l_Target != nullptr)
                            Talk(SAY_EMOTE_FRAGMENT, l_Target->GetGUID());


                        events.ScheduleEvent(EVENT_FRAGMENT, 30000);
                        break;
                    }
                    case EVENT_NETHER_RIP:
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM);

                        if (l_Target)
                            DoCast(l_Target, SPELL_NETHER_RIP_AURA);
                        else
                        {
                            l_Target = me->getVictim();
                            DoCastVictim(SPELL_NETHER_RIP_AURA);
                        }

                        events.ScheduleEvent(EVENT_NETHER_RIP, 13000);
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
            return new boss_harbaronAI(p_Creature);
        }
};

/// Cosmic Scythe - 100839
class npc_harbaron_scythe : public CreatureScript
{
    public:
        npc_harbaron_scythe() : CreatureScript("npc_harbaron_scythe") { }

        struct npc_harbaron_scytheAI : public ScriptedAI
        {
            npc_harbaron_scytheAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            Position m_FinalPos;

            uint64 m_Passenger = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->m_orientation = Position::NormalizeOrientation(me->m_orientation - M_PI / 2.0f);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                if (Creature* l_Passenger = p_Summoner->SummonCreature(NPC_COSMIC_SCYTHE, me->GetPosition()))
                {
                    l_Passenger->CastSpell(l_Passenger, SPELL_COSMIC_SCYTHE_VIS, true);

                    m_Passenger = l_Passenger->GetGUID();

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                            l_Passenger->EnterVehicle(me);
                    });
                }

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    if (Unit* l_Owner = me->GetAnyOwner())
                        l_Owner->CastSpell(Position(m_FinalPos.GetPositionX(), m_FinalPos.GetPositionY(), l_Owner->GetPositionZ(), 0.0f), SPELL_COSMIC_SCYTHE_DMG, false);

                    if (Creature* l_Passenger = Creature::GetCreature(*me, m_Passenger))
                    {
                        l_Passenger->DespawnOrUnsummon(500);

                        l_Passenger->RemoveAurasDueToSpell(SPELL_COSMIC_SCYTHE_VIS);
                        l_Passenger->CastSpell(l_Passenger, SPELL_COSMIC_SCYTHE_VIS_2, true);
                    }

                    me->DespawnOrUnsummon(500);
                });

                Movement::MoveSplineInit l_Init(me);

                Optional<Movement::MonsterSplineFilter> l_Filter;

                l_Init.SetFly();
                l_Init.SetSmooth();
                l_Init.SetUncompressed();

                l_Filter.emplace();

                l_Filter->FilterFlags = 2;
                l_Filter->BaseSpeed = 50.0f;
                l_Filter->AddedToStart = 1;

                l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(0, 5000));
                l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(1, 125));
                l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(2, 5000));

                l_Init.SetMonsterSplineFilters(*l_Filter.get_ptr());

                std::vector<float> l_ZOffsets   = { 5.0f, 6.0f, 7.0f, 6.0f, -3.0f };
                std::vector<float> l_Dists      = { 0.0f, 1.0f, 1.0f, 4.0f, 4.0f };

                uint8 l_IDx = 0;
                float l_CurrDist = 0.0f;

                for (float const& l_Dist : l_Dists)
                {
                    l_CurrDist += l_Dist;

                    Position l_Pos = me->GetPosition();

                    if (!G3D::fuzzyEq(l_CurrDist, 0.0f))
                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_CurrDist, 0.0f);

                    l_Pos.m_positionZ += l_ZOffsets[l_IDx++];

                    l_Init.Path().push_back(l_Pos.AsVector3());

                    m_FinalPos = l_Pos;
                }

                l_Init.Launch();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harbaron_scytheAI(p_Creature);
        }
};

/// Shackled Servitor - 98693
class npc_harbaron_shackled_servitor : public CreatureScript
{
    public:
        npc_harbaron_shackled_servitor() : CreatureScript("npc_harbaron_shackled_servitor") { }

        struct npc_harbaron_shackled_servitorAI : public ScriptedAI
        {
            npc_harbaron_shackled_servitorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                ApplyAllImmunities(true);

                /// Remove interrupting immunity
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_INTERRUPT_CAST, false);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SILENCE, false);

                DoZoneInCombat(me, 100.0f);
                DoCast(me, SPELL_SHACKLED_SERVITOR, true);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                events.ScheduleEvent(EVENT_1, 2000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_VOID_SNAP);
                        events.ScheduleEvent(EVENT_1, 6000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harbaron_shackled_servitorAI(p_Creature);
        }
};

/// Soul Fragment - 98761
class npc_harbaron_soul_fragment : public CreatureScript
{
    public:
        npc_harbaron_soul_fragment() : CreatureScript("npc_harbaron_soul_fragment") { }

        struct npc_harbaron_soul_fragmentAI : public ScriptedAI
        {
            public:
                npc_harbaron_soul_fragmentAI(Creature* p_Creature) : ScriptedAI(p_Creature)
                {
                    me->SetReactState(REACT_PASSIVE);
                    m_SummonerGuid = 0;
                    m_nextMoveTimer = 0;
                }

                void JustDespawned() override
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*me, m_SummonerGuid);

                    if (l_Target == nullptr)
                        return;

                    l_Target->RemoveAurasDueToSpell(SPELL_FRAGMENT_PERIODIC, me->GetGUID());

                    if (!l_Target->HasAura(SPELL_FRAGMENT_PERIODIC))
                        l_Target->RemoveAurasDueToSpell(SPELL_FRAGMENT_DUMMY);
                }

                void MoveEvent()
                {
                    if (IsInControl() || IsInDisable())
                    {
                        me->GetMotionMaster()->MoveIdle();
                        me->GetMotionMaster()->Clear();
                        return;
                    }

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void {
                        if (Unit* l_Summoner = Unit::GetUnit(*me, m_SummonerGuid))
                        {
                            float l_Angle = frand(0.0f, 2.0f * M_PI);

                            Position l_Pos = me->GetPosition();

                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, 10.0f, l_Angle, true);

                            me->SetWalk(true);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(0, l_Pos);
                        }
                    });
                }

                void MovementInform(uint32 type, uint32 id) override
                {
                    if (type != POINT_MOTION_TYPE)
                        return;

                    if (id == 0)
                        m_nextMoveTimer = 1;
                }

                void IsSummonedBy(Unit* p_Summoner) override
                {
                    if (p_Summoner == nullptr)
                        return;

                    m_SummonerGuid = p_Summoner->GetGUID();

                    p_Summoner->CastSpell(me, SPELL_FRAGMENT_CLONE_PLAYER, true); ///< Clone Caster
                    DoCast(p_Summoner, SPELL_FRAGMENT_PERIODIC, true); ///< Dmg tick summoner
                    DoCast(me, SPELL_FRAGMENT_SCALE, true); ///< Scale

                    MoveEvent();
                }

                void JustDied(Unit* /*p_Killer*/) override
                {
                    me->DespawnOrUnsummon();
                }

                void UpdateAI(uint32 const p_Diff) override
                {
                    UpdateOperations(p_Diff);

                    if (m_nextMoveTimer)
                    {
                        m_nextMoveTimer = 0;
                        MoveEvent();
                    }
                }

            private:
                uint64 m_SummonerGuid;
                uint32 m_nextMoveTimer;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harbaron_soul_fragmentAI(p_Creature);
        }
};

/// Nether Rip - 194668
class spell_harbaron_nether_rip : public SpellScriptLoader
{
    public:
        spell_harbaron_nether_rip() : SpellScriptLoader("spell_harbaron_nether_rip") { }

        class spell_harbaron_nether_rip_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harbaron_nether_rip_AuraScript);

            void PeriodicTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Creature* l_Harbaron = l_Caster->ToCreature())
                    if (boss_harbaron::boss_harbaronAI* l_LinkAI = CAST_AI(boss_harbaron::boss_harbaronAI, l_Harbaron->GetAI()))
                        l_LinkAI->SetNetherRip();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_harbaron_nether_rip_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harbaron_nether_rip_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_harbaron()
{
    /// Boss
    new boss_harbaron();

    /// Creatures
    new npc_harbaron_scythe();
    new npc_harbaron_shackled_servitor();
    new npc_harbaron_soul_fragment();

    /// Spell
    new spell_harbaron_nether_rip();
}
#endif
