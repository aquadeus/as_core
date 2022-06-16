////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

enum Says
{
    SAY_START           = 0,
    SAY_AGGRO           = 1,
    SAY_EVADE           = 2,
    SAY_EXECUTION       = 3,
    SAY_EXECUTION_EMOTE = 4,
    SAY_DEATH           = 5,
};

enum Spells
{
    SPELL_ARMED                 = 179670,
    SPELL_CHAOTIC_ENGERY        = 203622,
    SPELL_FEL_SLASH             = 203637,
    SPELL_MIGHTY_SMASH          = 202328,
    SPELL_MIGHTY_SMASH_JUMP_1   = 202339,
    SPELL_MIGHTY_SMASH_JUMP_2   = 202340,
    SPELL_MIGHTY_SMASH_JUMP_3   = 202343,
    SPELL_EXECUTION             = 202361,
    SPELL_EXECUTION_ROOT_PLR    = 205233,
    SPELL_STASIS_CRYSTAL_SUM    = 205230,
    SPELL_STASIS_CRYSTAL_VIS    = 205225,

    SPELL_BONUS_LOOT            = 226658
};

enum eEvents
{
    EVENT_PRE_FIGHT             = 1,
    EVENT_CHAOTIC_ENGERY        = 2,
    EVENT_FEL_SLASH             = 3,
    EVENT_MIGHTY_SMASH          = 4,
    EVENT_EXECUTION             = 5,

    ACTION_CRYSTAL_DESTROYED
};

Position const g_JumpPos = { 4679.24f, 4014.53f, 91.75f, 3.14f };
Position const g_PlayerJumpPos = { 4675.05f, 4015.61f, 91.74f, 6.2113f };

Position const g_CrystalSumPos[2] =
{
    { 4675.41f, 4020.20f, 91.74f, 0.0f },
    { 4674.90f, 4010.71f, 91.74f, 0.0f }
};

/// Fel Lord Betrug - 102446
class boss_fel_lord_betrug : public CreatureScript
{
public:
    boss_fel_lord_betrug() : CreatureScript("boss_fel_lord_betrug") { }

    struct boss_fel_lord_betrugAI : public BossAI
    {
        boss_fel_lord_betrugAI(Creature* creature) : BossAI(creature, DATA_BETRUG)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            me->SetVisible(false);
        }

        bool readyFight;
        bool crystalDestroyed;

        uint64 m_ExecuteTarget;

        void Reset() override
        {
            m_ExecuteTarget = 0;

            _Reset();
            DespawnTrashAndRemoveBuff();

            DoCast(me, SPELL_ARMED, true);

            if (instance && instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                me->SetReactState(REACT_DEFENSIVE);
            else
                readyFight = false;
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _EnterCombat();

            events.ScheduleEvent(EVENT_CHAOTIC_ENGERY, 6000); //18:43, 18:56, 19:35, 19:46
            events.ScheduleEvent(EVENT_FEL_SLASH, 12000); //18:49, 19:05, 20:01
            events.ScheduleEvent(EVENT_MIGHTY_SMASH, 32000); //19:09, 20:04, 20:58
        }

        void EnterEvadeMode() override
        {
            CreatureAI::EnterEvadeMode();

            Talk(SAY_EVADE);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();
            DespawnTrashAndRemoveBuff();

            CastSpellToPlayers(me, SPELL_BONUS_LOOT, true);
        }

        void DoAction(int32 const action) override
        {
            if (instance->GetData(DATA_MAIN_EVENT_PHASE) != IN_PROGRESS)
                return;

            if (action == 1)
            {
                me->SetVisible(true);
                me->GetMotionMaster()->MoveJump(saboFightPos.GetPositionX(), saboFightPos.GetPositionY(), saboFightPos.GetPositionZ(), 20.0f, 20.0f, 10);
                me->SetHomePosition(saboFightPos);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            }

            if (action == ACTION_CRYSTAL_DESTROYED)
            {
                if (crystalDestroyed)
                {
                    me->RemoveAurasDueToSpell(SPELL_EXECUTION);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXECUTION_ROOT_PLR);
                }
                crystalDestroyed = true;
            }
        }

        void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
        {
            if (p_Target == nullptr)
                return;

            if (p_SpellInfo->Id == SPELL_MIGHTY_SMASH)
                m_ExecuteTarget = p_Target->GetGUID();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != EFFECT_MOTION_TYPE)
                return;

            if (id == 10)
            {
                Talk(SAY_START);
                events.ScheduleEvent(EVENT_PRE_FIGHT, 1000);
            }

            if (id == SPELL_MIGHTY_SMASH_JUMP_1)
            {
                if (Unit* l_Target = Unit::GetUnit(*me, m_ExecuteTarget))
                    me->SetFacingToObject(l_Target);

                me->CastSpell(g_CrystalSumPos[0], SPELL_STASIS_CRYSTAL_SUM, true);
                me->CastSpell(g_CrystalSumPos[1], SPELL_STASIS_CRYSTAL_SUM, true);

                events.ScheduleEvent(EVENT_EXECUTION, 100);
            }
        }

        void DespawnTrashAndRemoveBuff()
        {
            if (instance)
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXECUTION_ROOT_PLR);

            std::list<Creature*> creList;
            GetCreatureListWithEntryInGrid(creList, me, NPC_STASIS_CRYSTAL, 120.0f);
            for (auto const& crystal : creList)
                crystal->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!UpdateVictim() && !readyFight)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
                case EVENT_PRE_FIGHT:
                {
                    readyFight = true;
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                    break;
                }
                case EVENT_CHAOTIC_ENGERY:
                {
                    DoCast(me, SPELL_CHAOTIC_ENGERY, true);
                    events.ScheduleEvent(EVENT_CHAOTIC_ENGERY, 12000);
                    break;
                }
                case EVENT_FEL_SLASH:
                {
                    DoCast(SPELL_FEL_SLASH);
                    events.ScheduleEvent(EVENT_FEL_SLASH, 16000);
                    break;
                }
                case EVENT_MIGHTY_SMASH:
                {
                    crystalDestroyed = false;
                    DoCast(SPELL_MIGHTY_SMASH);
                    events.ScheduleEvent(EVENT_MIGHTY_SMASH, 54000);
                    break;
                }
                case EVENT_EXECUTION:
                {
                    if (Unit* l_Target = Unit::GetUnit(*me, m_ExecuteTarget))
                    {
                        me->CastSpell(l_Target, SPELL_EXECUTION, false);

                        Talk(SAY_EXECUTION);
                        Talk(SAY_EXECUTION_EMOTE, m_ExecuteTarget);
                    }

                    break;
                }
                default:
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fel_lord_betrugAI (creature);
    }
};

//103672
class npc_betrug_stasis_crystal : public CreatureScript
{
public:
    npc_betrug_stasis_crystal() : CreatureScript("npc_betrug_stasis_crystal") { }

    struct npc_betrug_stasis_crystalAI : public ScriptedAI
    {
        npc_betrug_stasis_crystalAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            destroyed = false;
        }

        InstanceScript* instance;
        bool destroyed;

        void Reset() override { }

        void IsSummonedBy(Unit* summoner) override
        {
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            DoCast(me, SPELL_STASIS_CRYSTAL_VIS, true);
            DoCast(summoner, 205226, true);
        }

        void OnSpellClick(Unit* clicker) override
        {
            if (clicker->HasAura(SPELL_EXECUTION_ROOT_PLR))
                return;

            clicker->CastSpell(me, 205227, true);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (destroyed)
                return;

            if (spell->Id == 205227)
            {
                if (Creature* betrug = instance->instance->GetCreature(instance->GetData64(DATA_BETRUG)))
                    betrug->AI()->DoAction(ACTION_CRYSTAL_DESTROYED);

                destroyed = true;
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->RemoveAllAuras();
                me->DespawnOrUnsummon(2000);
            }
        }

        void UpdateAI(uint32 const diff) override { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_betrug_stasis_crystalAI(creature);
    }
};

//202328
class spell_betrug_mighty_smash : public SpellScriptLoader
{
public:
    spell_betrug_mighty_smash() : SpellScriptLoader("spell_betrug_mighty_smash") { }

    class spell_betrug_mighty_smash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_betrug_mighty_smash_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            if (p_Targets.empty())
                return;

            p_Targets.remove_if([this](WorldObject* p_Object) -> bool
            {
                if (!p_Object->IsPlayer())
                    return true;

                return false;
            });

            if (!p_Targets.empty())
                JadeCore::RandomResizeList(p_Targets, 1);
        }

        void HandleOnCast()
        {
            if (!GetCaster())
                return;

            GetCaster()->CastSpell(g_JumpPos, SPELL_MIGHTY_SMASH_JUMP_1, true);
        }

        void HandleOnHit()
        {
            Unit* l_Target = GetHitUnit();
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Target)
                return;

            l_Target->GetMotionMaster()->MoveJump(g_PlayerJumpPos, 30.0f, 15.0f);

            l_Target->CastSpell(l_Target, SPELL_EXECUTION_ROOT_PLR, true);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_betrug_mighty_smash_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnCast += SpellCastFn(spell_betrug_mighty_smash_SpellScript::HandleOnCast);
            OnHit += SpellHitFn(spell_betrug_mighty_smash_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_betrug_mighty_smash_SpellScript();
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_fel_lord_betrug()
{
    new boss_fel_lord_betrug();
    new npc_betrug_stasis_crystal();
    new spell_betrug_mighty_smash();
}
#endif
