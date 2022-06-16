////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "end_time.h"

enum ScriptTexts
{
    SAY_AGGRO       = 0,
    SAY_HIGHBORNE   = 1,
    SAY_DEATH       = 2,
    SAY_KILL        = 3,
    SAY_WIPE        = 4,
};

enum Spells
{
    SPELL_BLIGHTED_ARROW_AOE        = 101567,
    SPELL_BLIGHTED_ARROW_SUMMON     = 101547,
    SPELL_BLIGHTED_ARROW_EXPLOSION  = 101401,
    SPELL_BLIGHTED_ARROW_KNOCKBACK  = 100763,
    SPELL_SUMMON_JUMP_STALKER       = 101517,
    SPELL_ENTER_JUMP_VEHICLE        = 101528,
    SPELL_TRIGGER_JUMP_SYLVANAS     = 101527,


    SPELL_SHRIEK_OF_THE_HIGHBORNE   = 101412,
    SPELL_UNHOLY_SHOT               = 101411,
    SPELL_BLACK_ARROW               = 101404,
    SPELL_WRACKING_PAIN_ANY         = 100865,
    SPELL_WRACKING_PAIN_AURA        = 101258,
    SPELL_WRACKING_PAIN_DMG         = 101257,
    SPELL_DEATH_GRIP_AOE            = 101397,
    SPELL_DEATH_GRIP                = 101987,
    SPELL_SUMMON_GHOULS             = 102603, // before combat
    SPELL_TELEPORT                  = 101398,
    SPELL_CALL_OF_THE_HIGHBORNE     = 100686, // immune
    SPELL_CALL_OF_THE_HIGHBORNE_VIS_SUMMON_CIRCLE   = 100867, // visual spawn ghouls
    SPELL_CALL_OF_THE_HIGHBORNE_VISUAL_PURPLE_AREA  = 105766, // visual back ghouls
    SPELL_CALL_OF_THE_HIGHBORNE_3   = 102581, // visual ?
    SPELL_SPAWN_GHOUL               = 101200,
    SPELL_SEEPING_SHADOWS_DUMMY     = 103175,
    SPELL_SEEPING_SHADOWS_AURA      = 103182,
    SPELL_SACRIFICE                 = 101348,
    SPELL_DWINDLE                   = 101259,
    SPELL_JUMP                      = 101517,
    SPELL_JUMP_SCRIPT               = 101527,
    SPELL_JUMP_VEHICLE              = 101528,
    SPELL_PERMANENT_FEIGH_DEATH     = 96733,
    SPELL_SHRINK                    = 101318,

    /// Ghoul Circle Dummie -> Not real ghoul
    SPELL_SUMMON_GHOUL_N            = 100894,
    SPELL_SUMMON_GHOUL_NE           = 100917,
    SPELL_SUMMON_GHOUL_NW           = 100918,
    SPELL_SUMMON_GHOUL_E            = 100919,
    SPELL_SUMMON_GHOUL_W            = 100920,
    SPELL_SUMMON_GHOUL_S            = 100921,
    SPELL_SUMMON_GHOUL_SE           = 100923,
    SPELL_SUMMON_GHOUL_SW           = 100924,
    /// Ghoul Spells
    SPELL_CALLING_OF_HIGHBORNE_BEAM = 100862

};

enum Events
{
    EVENT_SHRIEK_OF_THE_HIGHBORNE   = 1,
    EVENT_UNHOLY_SHOT               = 2,
    EVENT_BLACK_ARROW               = 3,
    EVENT_DEATH_GRIP                = 4,
    EVENT_TELEPORT                  = 5,
    EVENT_TELEPORT_1                = 6,
    EVENT_SPAWN_GHOUL               = 7,
    EVENT_MOVE_GHOUL                = 8,
    EVENT_START                     = 9,
    EVENT_BLIGHTED_ARROW            = 10,

};

enum Adds
{
    NPC_GHOUL_SUMMON_CIRCLE     = 54197,
    NPC_BRITTLE_GHOUL           = 54952,
    NPC_RISEN_GHOUL             = 54191,
    NPC_JUMP                    = 54385
};

enum Others
{
    DATA_GUID           = 1,
    POINT_GHOUL         = 2,
    ACTION_GHOUL        = 3,
    ACTION_KILL_GHOUL   = 4,
    WS_ACHIEVEMENT      = 6885
};

const Position centerPos = { 3845.51f, 909.318f, 56.1463f, 1.309f };

class boss_echo_of_sylvanas : public CreatureScript
{
    public:
        boss_echo_of_sylvanas() : CreatureScript("boss_echo_of_sylvanas") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_echo_of_sylvanasAI(creature);
        }

        struct boss_echo_of_sylvanasAI : public BossAI
        {
            boss_echo_of_sylvanasAI(Creature* creature) : BossAI(creature, DATA_ECHO_OF_SYLVANAS)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                me->CastSpell(me, SPELL_SUMMON_GHOULS, true);
            }

            uint8 ghouls;
            uint8 m_Deadghouls;
            uint64 m_JumpStalker;

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(ETScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset() override
            {
                m_JumpStalker = 0;

                _Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                ghouls = 0;

                DoCast(me, Spells::SPELL_SUMMON_JUMP_STALKER, true);

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    Unit* l_Summon = GetJumpStalker();

                    if (l_Summon)
                        me->CastSpell(l_Summon, Spells::SPELL_ENTER_JUMP_VEHICLE, true);
                });
            }

            Unit* GetJumpStalker()
            {
                return ObjectAccessor::GetUnit(*me, m_JumpStalker);
            }


            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);

                // Quest
                Map::PlayerList const& PlayerList = instance->instance->GetPlayers();
                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (Player* pPlayer = i->getSource())
                            if (me->GetDistance2d(pPlayer) <= 50.0f && pPlayer->GetQuestStatus(30097) == QUEST_STATUS_INCOMPLETE)
                                DoCast(pPlayer, SPELL_ARCHIVED_SYLVANAS, true);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (p_Summon->GetEntry() == NPC_JUMP)
                {
                    m_JumpStalker = p_Summon->GetGUID();
                    p_Summon->SetDisableGravity(true);
                    p_Summon->SetCanFly(true);
                    Position l_Pos = p_Summon->GetHomePosition();
                    l_Pos.m_positionZ = 63.146297f;
                    p_Summon->NearTeleportTo(l_Pos, false);
                }
            }

            void SummonedCreatureDespawn(Creature* p_Summon) override
            {
                BossAI::SummonedCreatureDespawn(p_Summon);

                if (p_Summon && p_Summon->GetEntry() == NPC_JUMP)
                    m_JumpStalker = 0;
            }

            void OnRemoveAura(uint32 p_AuraId, AuraRemoveMode /**/) override
            {
                if (p_AuraId == Spells::SPELL_ENTER_JUMP_VEHICLE)
                {
                    if (Unit* l_Veh = GetJumpStalker())
                    {
                        if (l_Veh->GetVehicleKit())
                            l_Veh->GetVehicleKit()->RemoveAllPassengers();
                    }
                }
            }

            void EnterCombat(Unit* /*who*/) override
            {
                instance->DoUpdateWorldState(WS_ACHIEVEMENT, 0);

                _EnterCombat();
                me->RemoveAllAuras();

                me->SetDisableGravity(false);
                me->SetCanFly(false);

                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_UNHOLY_SHOT, urand(5 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHRIEK_OF_THE_HIGHBORNE, urand(5 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_TELEPORT, 40 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BLIGHTED_ARROW, 30 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BLACK_ARROW, 7 * TimeConstants::IN_MILLISECONDS);

                summons.DespawnEntry(NPC_BRITTLE_GHOUL);
                m_Deadghouls = 0;
            }

            void KilledUnit(Unit* who) override
            {
                if (who && who->IsPlayer())
                    Talk(SAY_KILL);
            }

            void DoAction(const int32 action) override
            {
                if (action == ACTION_GHOUL)
                {
                    ghouls++;
                    if (ghouls >= 1)
                    {
                        DoCastAOE(SPELL_SACRIFICE);
                        me->DespawnCreaturesInArea({ NPC_RISEN_GHOUL, NPC_GHOUL_SUMMON_CIRCLE });
                        events.ScheduleEvent(EVENT_START, 2 * TimeConstants::IN_MILLISECONDS);
                        ghouls = 0;
                    }
                }
                else if (action == ACTION_KILL_GHOUL)
                {
                    m_Deadghouls++;

                    if (m_Deadghouls >= 2)
                        instance->DoUpdateWorldState(WS_ACHIEVEMENT, 1);
                }
            }

            void JustReachedHome() override
            {
                Talk(SAY_WIPE);
                DoCast(me, SPELL_SUMMON_GHOULS, true);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                switch (p_Spell->Id)
                {
                    case Spells::SPELL_TRIGGER_JUMP_SYLVANAS:
                    {
                        me->DelayedCastSpell(p_Caster, Spells::SPELL_ENTER_JUMP_VEHICLE, true, 1);
                        me->DelayedCastSpell(me, Spells::SPELL_BLIGHTED_ARROW_EXPLOSION, false, 250);
                        break;
                    }

                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_Spell) override
            {
                if (p_Spell && p_Spell->Id == Spells::SPELL_BLIGHTED_ARROW_EXPLOSION)
                    me->DelayedRemoveAura(Spells::SPELL_ENTER_JUMP_VEHICLE, 250);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_Spell) override
            {
                switch (p_Spell->Id)
                {
                    case Spells::SPELL_BLIGHTED_ARROW_AOE:
                    {
                        me->CastSpell(me, Spells::SPELL_SUMMON_JUMP_STALKER, true);
                        AddTimedDelayedOperation(150, [this]() -> void
                        {
                            Unit* l_Stalker = GetJumpStalker();

                            if (!l_Stalker)
                                return;

                            Position l_Pos = me->GetPosition();
                            l_Pos.m_positionZ = l_Stalker->GetPositionZ();

                            l_Stalker->NearTeleportTo(l_Pos, false);
                            l_Stalker->DelayedCastSpell(l_Stalker, SPELL_TRIGGER_JUMP_SYLVANAS, true, 100);
                        });
                        break;
                    }

                    default:
                        break;
                }
            }

            void ExecuteEvent(uint32 const p_EventID) override
            {
                switch (p_EventID)
                {
                    case EVENT_BLACK_ARROW:
                    {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(pTarget, SPELL_BLACK_ARROW);
                        events.ScheduleEvent(EVENT_BLACK_ARROW, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_BLIGHTED_ARROW:
                    {
                        DoCastAOE(Spells::SPELL_BLIGHTED_ARROW_AOE);
                        events.ScheduleEvent(EVENT_BLACK_ARROW, urand(10 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case EVENT_UNHOLY_SHOT:
                    {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(pTarget, SPELL_UNHOLY_SHOT);
                        events.ScheduleEvent(EVENT_UNHOLY_SHOT, urand(10 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }

                    case EVENT_SHRIEK_OF_THE_HIGHBORNE:
                    {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(pTarget, SPELL_SHRIEK_OF_THE_HIGHBORNE);
                        events.ScheduleEvent(EVENT_SHRIEK_OF_THE_HIGHBORNE, urand(15 * TimeConstants::IN_MILLISECONDS, 21 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }

                    case EVENT_TELEPORT:
                    {
                        events.Reset();
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_TELEPORT, true);
                        events.ScheduleEvent(EVENT_TELEPORT_1, 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }

                    case EVENT_TELEPORT_1:
                    {
                        Talk(SAY_HIGHBORNE);
                        DoCast(me, SPELL_CALL_OF_THE_HIGHBORNE, true);
                        DoCast(me, Spells::SPELL_SUMMON_GHOUL_N, true);

                        for (uint32 l_Idx = Spells::SPELL_SUMMON_GHOUL_NE; l_Idx <= Spells::SPELL_SUMMON_GHOUL_S; ++l_Idx)
                            DoCast(me, l_Idx, true);

                        DoCast(me, Spells::SPELL_SUMMON_GHOUL_SE, true);
                        DoCast(me, Spells::SPELL_SUMMON_GHOUL_SW, true);

                        events.ScheduleEvent(EVENT_DEATH_GRIP, 3 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SPAWN_GHOUL, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_DEATH_GRIP:
                    {
                        DoCastAOE(SPELL_DEATH_GRIP_AOE);
                        break;
                    }
                    case EVENT_SPAWN_GHOUL:
                    {
                        for (uint64 l_Guid : summons)
                        {
                            Unit* l_Summon = ObjectAccessor::GetUnit(*me, l_Guid);

                            if (l_Summon && l_Summon->GetEntry() == NPC_GHOUL_SUMMON_CIRCLE)
                                l_Summon->CastSpell(l_Summon, Spells::SPELL_SPAWN_GHOUL, true);
                        }
                        break;
                    }
                    case EVENT_START:
                    {
                        me->RemoveAura(SPELL_CALL_OF_THE_HIGHBORNE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->GetMotionMaster()->MoveChase(me->getVictim());

                        events.ScheduleEvent(EVENT_BLACK_ARROW, 15 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BLIGHTED_ARROW, 5 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_UNHOLY_SHOT, urand(5 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        events.ScheduleEvent(EVENT_SHRIEK_OF_THE_HIGHBORNE, urand(5 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        events.ScheduleEvent(EVENT_TELEPORT, 40 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }

                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                BossAI::UpdateAI(p_Diff);
            }
        };
};

class npc_echo_of_sylvanas_ghoul : public CreatureScript
{
    public:

        npc_echo_of_sylvanas_ghoul() : CreatureScript("npc_echo_of_sylvanas_ghoul") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_echo_of_sylvanas_ghoulAI(pCreature);
        }

        struct npc_echo_of_sylvanas_ghoulAI : public ScriptedAI
        {
            npc_echo_of_sylvanas_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void IsSummonedBy(Unit* /*owner*/) override
            {
                DoCast(me, SPELL_CALL_OF_THE_HIGHBORNE_VIS_SUMMON_CIRCLE, true);
                DoCast(me, SPELL_CALL_OF_THE_HIGHBORNE_VISUAL_PURPLE_AREA, true);
            }
        };
};

class npc_echo_of_sylvanas_risen_ghoul : public CreatureScript
{
    public:
        npc_echo_of_sylvanas_risen_ghoul() : CreatureScript("npc_echo_of_sylvanas_risen_ghoul") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_echo_of_sylvanas_risen_ghoulAI(pCreature);
        }

        struct npc_echo_of_sylvanas_risen_ghoulAI : public Scripted_NoMovementAI
        {
            npc_echo_of_sylvanas_risen_ghoulAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            uint64 _guid;

            void Reset() override
            {
                events.Reset();
                me->CastSpell(me, Spells::SPELL_CALLING_OF_HIGHBORNE_BEAM, true);
                events.ScheduleEvent(EVENT_MOVE_GHOUL, 2 * IN_MILLISECONDS);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner)
                    return;

                p_Summoner->RemoveAura(Spells::SPELL_CALL_OF_THE_HIGHBORNE_VIS_SUMMON_CIRCLE);
            }


            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAura(SPELL_WRACKING_PAIN_ANY);

                if (Unit* l_Summoner = me->GetAnyOwner())
                {
                    if (l_Summoner->ToCreature())
                        l_Summoner->ToCreature()->DespawnOrUnsummon(IN_MILLISECONDS);
                }

                if (Creature* pSylvanas = me->FindNearestCreature(NPC_ECHO_OF_SYLVANAS, 300.0f))
                    pSylvanas->GetAI()->DoAction(ACTION_KILL_GHOUL);
                me->DespawnOrUnsummon(500);
            }

            void MovementInform(uint32 type, uint32 data) override
            {
                if (type == POINT_MOTION_TYPE && data == POINT_GHOUL)
                {
                    if (Unit* pSylvanas = me->FindNearestCreature(NPC_ECHO_OF_SYLVANAS, 50.0f))
                        pSylvanas->GetAI()->DoAction(ACTION_GHOUL);
                }
            }

            void UpdateAI(const uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_GHOUL:
                        {
                            if (Unit* l_Summoner = me->GetAnyOwner())
                            {
                                l_Summoner->CastSpell(l_Summoner, Spells::SPELL_SHRINK, true);
                                l_Summoner->GetMotionMaster()->MovePoint(POINT_GHOUL, centerPos);
                                l_Summoner->SetWalk(true);
                                l_Summoner->SetSpeed(MOVE_RUN, 0.5f, true);
                                l_Summoner->SetSpeed(MOVE_WALK, 0.5f, true);
                            }

                            me->SetWalk(true);
                            me->SetSpeed(MOVE_RUN, 0.5f, true);
                            me->SetSpeed(MOVE_WALK, 0.5f, true);
                            me->GetMotionMaster()->MovePoint(POINT_GHOUL, centerPos);
                            break;
                        }
                    }
                }
            }
        };
};

/// Blighted Arrows - 101549
class spell_et_sylvanas_blighted_arrows : public SpellScriptLoader
{
    public:
        spell_et_sylvanas_blighted_arrows() : SpellScriptLoader("spell_et_sylvanas_blighted_arrows") { }


        class spell_et_sylvanas_blighted_arrows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_et_sylvanas_blighted_arrows_SpellScript);


            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_Pos;
                float l_Dist = 3.0f;

                for (uint8 l_Itr = 0; l_Itr < 5; ++l_Itr)
                {
                    l_Caster->GetNearestPosition(l_Pos, l_Dist, float(M_PI));
                    l_Dist += 3.0f;
                    l_Caster->CastSpell(l_Pos, Spells::SPELL_BLIGHTED_ARROW_SUMMON, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_et_sylvanas_blighted_arrows_SpellScript::HandleEffectHitTarget, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_et_sylvanas_blighted_arrows_SpellScript();
        }
};

/// Blighted Arrows - 101401
class spell_et_sylvanas_blighted_arrows_dmg : public SpellScriptLoader
{
    public:
        spell_et_sylvanas_blighted_arrows_dmg() : SpellScriptLoader("spell_et_sylvanas_blighted_arrows_dmg") { }

        class spell_blighted_arrows_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blighted_arrows_dmg_SpellScript);

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, Spells::SPELL_BLIGHTED_ARROW_KNOCKBACK, true);

                if (l_Target->ToCreature())
                    l_Target->ToCreature()->DespawnOrUnsummon(IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_blighted_arrows_dmg_SpellScript::HandleEffectHitTarget, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blighted_arrows_dmg_SpellScript();
        }
};

class spell_echo_of_sylvanas_wracking_pain_dmg : public SpellScriptLoader
{
    public:
        spell_echo_of_sylvanas_wracking_pain_dmg() : SpellScriptLoader("spell_echo_of_sylvanas_wracking_pain_dmg") { }

        class spell_echo_of_sylvanas_wracking_pain_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_echo_of_sylvanas_wracking_pain_dmg_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster() || !GetCaster()->isAlive())
                {
                    targets.clear();
                    return;
                }

                if (!GetCaster()->GetAI())
                {
                    targets.clear();
                    return;
                }

                uint64 _guid = GetCaster()->GetAI()->GetGUID(DATA_GUID);
                if (Creature* pTarget = ObjectAccessor::GetCreature(*GetCaster(), _guid))
                {
                    if (pTarget->isAlive())
                        targets.remove_if(WrackingPainTargetSelector(GetCaster(), pTarget));
                    else
                        targets.clear();
                }
                else
                    targets.clear();
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_echo_of_sylvanas_wracking_pain_dmg_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

        private:

            class WrackingPainTargetSelector
            {
            public:
                WrackingPainTargetSelector(WorldObject* caster, WorldObject* target) : i_caster(caster), i_target(target) { }

                bool operator()(WorldObject* unit)
                {
                    if (unit->IsInBetween(i_caster, i_target))
                        return false;
                    return true;
                }

                WorldObject* i_caster;
                WorldObject* i_target;
            };
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_echo_of_sylvanas_wracking_pain_dmg_SpellScript();
        }
};

class spell_echo_of_sylvanas_death_grip_aoe : public SpellScriptLoader
{
    public:
        spell_echo_of_sylvanas_death_grip_aoe() : SpellScriptLoader("spell_echo_of_sylvanas_death_grip_aoe") { }

        class spell_echo_of_sylvanas_death_grip_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_echo_of_sylvanas_death_grip_aoe_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetCaster(), SPELL_DEATH_GRIP, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_echo_of_sylvanas_death_grip_aoe_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_echo_of_sylvanas_death_grip_aoe_SpellScript();
        }
};

class spell_echo_of_sylvanas_seeping_shadows : public SpellScriptLoader
{
    public:
        spell_echo_of_sylvanas_seeping_shadows() : SpellScriptLoader("spell_echo_of_sylvanas_seeping_shadows") { }

        class spell_echo_of_sylvanas_seeping_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_echo_of_sylvanas_seeping_shadows_AuraScript);

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                int32 amount = int32(0.2f * (100.0f - GetCaster()->GetHealthPct()));

                if (Aura* aur = GetCaster()->GetAura(103182))
                    aur->ModStackAmount(amount - aur->GetStackAmount());
                else
                    GetCaster()->CastCustomSpell(103182, SPELLVALUE_AURA_STACK, amount, GetCaster(), true);

            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_echo_of_sylvanas_seeping_shadows_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_echo_of_sylvanas_seeping_shadows_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_echo_of_sylvanas()
{
    new boss_echo_of_sylvanas();
    new npc_echo_of_sylvanas_ghoul();
    new npc_echo_of_sylvanas_risen_ghoul();
    ///new spell_echo_of_sylvanas_wracking_pain_dmg();
    new spell_echo_of_sylvanas_death_grip_aoe();
    new spell_echo_of_sylvanas_seeping_shadows();
    new spell_et_sylvanas_blighted_arrows();
    new spell_et_sylvanas_blighted_arrows_dmg();
}
#endif