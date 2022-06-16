////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

# include "PandariaPCH.h"

enum Texts
{
    SAY_AGGRO         = 0, ///< You will take my place on the eternal brazier.
    SAY_DEATH         = 1, ///< The eternal fire will never be extinguished.
    SAY_INTRO         = 2,
    SAY_SLAY          = 3, ///< Up in smoke.
    SAY_ANCIENT_FLAME = 4, ///< Your flesh will melt.
    SAY_ETERNAL_AGONY = 5, ///< Your pain will be endless.
    SAY_POOL_OF_FIRE  = 6, ///< You will feel but a fraction of my agony.
    SAY_BURNING_SOUL  = 7  ///< Burn!
};

enum Spells
{
    SPELL_MAGMA_CRUSH        = 144688, ///< 10s, every 15 s.

    SPELL_ANCIENT_FLAME      = 144695, ///< 40s
    SPELL_ANCIENT_FLAME_AURA = 144691, ///< Periodic dmg. aura.
    SPELL_ANCIENT_FLAME_DMG  = 144699, ///< Dmg spell.

    SPELL_BURNING_SOUL       = 144689, ///< 20s, every 30 s.
    SPELL_BURNING_SOUL_EX    = 144690, ///< Removal Explode spell.

    SPELL_POOL_OF_FIRE       = 144692, ///< 30s - Summon spell.
    SPELL_POOL_OF_FIRE_A     = 144693, ///< Periodic dmg. aura.

    SPELL_ETERNAL_AGONY      = 144696  ///< Enrage spell, 5 min.
};

enum Events
{
    EVENT_ANCIENT_FLAME = 1,
    EVENT_BURNING_SOUL  = 2,
    EVENT_POOL_OF_FIRE  = 3,
    EVENT_MAGMA_CRUSH   = 4,

    EVENT_ETERNAL_AGONY = 5 ///< Berserk.
};

enum Adds
{
    NPC_ANCIENT_FLAME = 72059
};

enum Quests
{
    QUEST_SHORT_SUPPLY_REWARD = 33118, ///< weekly quest, to flag completed encounter
};

enum Items
{
    ITEM_FLAME_SCARRED_CACHE_OF_OFFERINGS = 104273,
};

enum Timers
{
    TIMER_ANCIENT_FLAME_FIRST = 40 * IN_MILLISECONDS,
    TIMER_ANCIENT_FLAME       = 40 * IN_MILLISECONDS,
    TIMER_BURNING_SOUL_FIRST  = 25 * IN_MILLISECONDS,
    TIMER_BURNING_SOUL        = 25 * IN_MILLISECONDS,
    TIMER_POOL_OF_FIRE_FIRST  = 35 * IN_MILLISECONDS,
    TIMER_POOL_OF_FIRE        = 45 * IN_MILLISECONDS,
    TIMER_MAGMA_CRUSH_FIRST   = 10 * IN_MILLISECONDS,
    TIMER_MAGMA_CRUSH         = 23 * IN_MILLISECONDS,
    TIMER_ETERNAL_AGONY       = 5 * MINUTE * IN_MILLISECONDS,
};

enum AreaIds
{
    AREA_ORDOS_TEMPLE = 6823,
};

const Position celestialChallengePos = { -621.23f, -4921.44f, 2.06f, 6.08f };

#define ARENA_MIN_POSITION_X -99.33f
#define ARENA_MAX_POSITION_X -36.60f
#define ARENA_MIN_POSITION_Y -5435.89f
#define ARENA_MAX_POSITION_Y -5360.81f

bool IsTargetOutOfOrdosArena(WorldObject const* target)
{
    if (target->GetPositionX() < ARENA_MIN_POSITION_X ||
        target->GetPositionX() > ARENA_MAX_POSITION_X ||
        target->GetPositionY() < ARENA_MIN_POSITION_Y ||
        target->GetPositionY() > ARENA_MAX_POSITION_Y)
    {
        return true;
    }

    return false;
}

class LootCheck
{
    public:
        LootCheck() {}

        bool operator()(Player* target)
        {
            if (IsTargetOutOfOrdosArena(target))
                return false;

            Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_SHORT_SUPPLY_REWARD);
            if (!quest)
                return false;

            if (!target->ToPlayer()->SatisfyQuestWeek(quest, false))
                return false;

            return true;
        }
};

class boss_ordos : public CreatureScript
{
    public:
        boss_ordos() : CreatureScript("boss_ordos") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ordosAI(creature);
        }

        struct boss_ordosAI : public ScriptedAI
        {
            boss_ordosAI(Creature* creature) : ScriptedAI(creature)
            {
                ApplyAllImmunities(true);
            }

            void InitializeAI() override
            {
                if (!me->isDead())
                    Reset();
            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();

                me->SetReactState(REACT_DEFENSIVE);
            }

            void JustRespawned() override
            {
                Talk(SAY_INTRO);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_MAGMA_CRUSH, TIMER_ANCIENT_FLAME_FIRST);
                events.ScheduleEvent(EVENT_ANCIENT_FLAME, TIMER_ANCIENT_FLAME_FIRST);
                events.ScheduleEvent(EVENT_BURNING_SOUL, TIMER_BURNING_SOUL_FIRST);
                events.ScheduleEvent(EVENT_POOL_OF_FIRE, TIMER_POOL_OF_FIRE_FIRST);
                events.ScheduleEvent(EVENT_ETERNAL_AGONY, TIMER_ETERNAL_AGONY);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                events.Reset();
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (IsOutOfArena())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    ProcessEvents(eventId);
                }

                DoMeleeAttackIfReady();
            }

        private:

            void ProcessEvents(const uint32 eventId)
            {
                switch (eventId)
                {
                case EVENT_MAGMA_CRUSH:
                    DoMagmaCrush();
                    events.ScheduleEvent(EVENT_MAGMA_CRUSH, TIMER_MAGMA_CRUSH);
                    break;
                case EVENT_ANCIENT_FLAME:
                    DoAncientFlame();
                    events.ScheduleEvent(EVENT_ANCIENT_FLAME, TIMER_ANCIENT_FLAME);
                    break;
                case EVENT_POOL_OF_FIRE:
                    DoPoolOfFire();
                    events.ScheduleEvent(EVENT_POOL_OF_FIRE, TIMER_POOL_OF_FIRE);
                    break;
                case EVENT_BURNING_SOUL:
                    DoBurningSoul();
                    events.ScheduleEvent(EVENT_BURNING_SOUL, TIMER_BURNING_SOUL);
                    break;
                case EVENT_ETERNAL_AGONY:
                    DoEternalAgony();
                    events.ScheduleEvent(EVENT_ETERNAL_AGONY, 15000);
                    break;
                }
            }

            bool IsOutOfArena() const
            {
                if (IsTargetOutOfOrdosArena(me))
                    return true;

                return false;
            }

            void DoMagmaCrush()
            {
                DoCastVictim(SPELL_MAGMA_CRUSH);
            }

            void DoAncientFlame()
            {
                Talk(SAY_ANCIENT_FLAME);
                DoCast(me, SPELL_ANCIENT_FLAME);
            }

            void DoPoolOfFire()
            {
                Talk(SAY_POOL_OF_FIRE);

                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true);
                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);

                if (target)
                    DoCast(target, SPELL_POOL_OF_FIRE);
            }

            void DoBurningSoul()
            {
                Talk(SAY_BURNING_SOUL);

                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true);
                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);

                if (target)
                    DoCast(target, SPELL_BURNING_SOUL);
            }

            void DoEternalAgony()
            {
                Talk(SAY_ETERNAL_AGONY);
                DoCast(me, SPELL_ETERNAL_AGONY);
            }

            void MarkAsBossIsDone(Player* player, Quest const* questMark)
            {
                if (questMark)
                    player->RewardQuest(questMark, 0, nullptr, false);
            }
        };
};

class npc_ordos_ancient_flame : public CreatureScript
{
    public:
        npc_ordos_ancient_flame() : CreatureScript("npc_ordos_ancient_flame")
        {
        }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ordos_ancient_flameAI(creature);
        }

        struct npc_ordos_ancient_flameAI : public Scripted_NoMovementAI
        {
            npc_ordos_ancient_flameAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetDisableGravity(true);
                me->SetCanFly(true);
            }

            void Reset()
            {
                me->AddAura(SPELL_ANCIENT_FLAME_AURA, me);
            }
        };
};

class spell_ordos_burning_soul_aura : public SpellScriptLoader
{
    public:
        spell_ordos_burning_soul_aura() : SpellScriptLoader("spell_ordos_burning_soul_aura") { }

        class spell_ordos_burning_soul_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ordos_burning_soul_aura_SpellScript);

            void FillTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (Creature* pCreature = GetCaster()->ToCreature())
                {
                    const uint8 targetsCount = 3;

                    std::list<Unit*> newTargets;
                    pCreature->AI()->SelectTargetList(newTargets, 25, SELECT_TARGET_RANDOM, 0.0f, true);
                    if (newTargets.size() > targetsCount)
                    {
                        newTargets.remove(pCreature->getVictim());
                    }

                    JadeCore::RandomResizeList(newTargets, targetsCount);

                    targets.clear();

                    for (auto target : newTargets)
                        targets.push_back(target);
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ordos_burning_soul_aura_SpellScript::FillTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_ordos_burning_soul_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ordos_burning_soul_aura_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(GetTarget(), SPELL_BURNING_SOUL_EX, true);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_ordos_burning_soul_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ordos_burning_soul_aura_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_ordos_burning_soul_aura_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ordos()
{
    new boss_ordos();                           ///< 72057
    new npc_ordos_ancient_flame();              ///< 72059
    new spell_ordos_burning_soul_aura();        ///< 144689
}
#endif
