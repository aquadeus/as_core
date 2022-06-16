////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "darkheart_thicket.hpp"

enum Spells
{
    SPELL_BREATH_OF_CORRUPTION      = 199329,
    SPELL_BREATH_OF_CORRUPTION_DMG  = 191325,
    SPELL_EARTHSHAKING_ROAR         = 199389,
    SPELL_FALLING_ROCKS_AT          = 199458,
    SPELL_DOWN_DRAFT                = 199345,

    /// Egg
    SPELL_SUM_BIRTH_WHELPLING_1     = 199304,
    SPELL_SUM_BIRTH_WHELPLING_2     = 199313,

    /// Whelpling
    SPELL_HATESPAWN_DETONATION      = 212797,

    SPELL_BONUS_ROLL                = 226611
};

enum eEvents
{
    EVENT_BREATH_OF_CORRUPTION  = 1,
    EVENT_EARTHSHAKING_ROAR     = 2,
    EVENT_DOWN_DRAFT            = 3
};

/// Dresaron - 99200
class boss_dresaron : public CreatureScript
{
    public:
        boss_dresaron() : CreatureScript("boss_dresaron") { }

        struct boss_dresaronAI : public BossAI
        {
            boss_dresaronAI(Creature* p_Creature) : BossAI(p_Creature, DATA_DRESARON) { }

            void Reset() override
            {
                _Reset();
                TreshRestore(false);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                TreshRestore(false);

                _EnterCombat();

                events.Reset();

                events.ScheduleEvent(EVENT_BREATH_OF_CORRUPTION, 7000);
                events.ScheduleEvent(EVENT_EARTHSHAKING_ROAR, 14000);
                events.ScheduleEvent(EVENT_DOWN_DRAFT, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                TreshRestore(true);
                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);

                me->GetMotionMaster()->Clear();

                Position l_Pos = *me;
                l_Pos.m_positionZ = me->GetMap()->GetHeight(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                me->GetMotionMaster()->MoveLand(0, l_Pos);

                instance->DoUpdateCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, 1838, 0, me);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_BREATH_OF_CORRUPTION)
                    DoCast(p_Target, SPELL_BREATH_OF_CORRUPTION_DMG);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (!p_Summon)
                    return;

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NPC_BREATH_OF_CORRUPTION:
                    {
                        me->SetFacingTo(me->GetAngle(p_Summon));
                        me->DelayedCastSpell(p_Summon, Spells::SPELL_BREATH_OF_CORRUPTION_DMG, false, 100);
                        break;
                    }
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case Spells::SPELL_BREATH_OF_CORRUPTION:
                    {
                        p_Targets.clear();

                        if (me->getVictim())
                            p_Targets.push_back(me->getVictim());

                        break;
                    }
                }
            }

            void TreshRestore(bool p_EncounterEnd)
            {
                std::list<Creature*> l_CreatureList;
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_CORRUPTED_DRAGON_EGG, 100.0f);
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_HATESPAWN_WHELPLING, 100.0f);

                for (auto const& l_Trash : l_CreatureList)
                {
                    if (l_Trash->GetEntry() == NPC_CORRUPTED_DRAGON_EGG)
                    {
                        if (p_EncounterEnd)
                            l_Trash->DespawnOrUnsummon();
                        else
                        {
                            if (!l_Trash->isAlive())
                            {
                                l_Trash->RemoveCorpse();
                                l_Trash->Respawn();
                            }
                        }
                    }
                    else
                        l_Trash->DespawnOrUnsummon();
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

                if (me->GetDistance(me->GetHomePosition()) >= 45.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_BREATH_OF_CORRUPTION:
                    {
                        DoCastVictim(SPELL_BREATH_OF_CORRUPTION);
                        events.ScheduleEvent(EVENT_BREATH_OF_CORRUPTION, 27000);
                        break;
                    }
                    case EVENT_EARTHSHAKING_ROAR:
                    {
                        DoCast(SPELL_EARTHSHAKING_ROAR);
                        events.ScheduleEvent(EVENT_EARTHSHAKING_ROAR, 19000);
                        break;
                    }
                    case EVENT_DOWN_DRAFT:
                    {
                        DoCast(SPELL_DOWN_DRAFT);
                        events.ScheduleEvent(EVENT_DOWN_DRAFT, 30000);
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
            return new boss_dresaronAI(p_Creature);
        }
};

/// Corrupted Egg - 100533
/// Corrupted Dragon Egg - 101072
class npc_dresaron_corrupted_dragon_egg : public CreatureScript
{
    public:
        npc_dresaron_corrupted_dragon_egg() : CreatureScript("npc_dresaron_corrupted_dragon_egg") { }

        struct npc_dresaron_corrupted_dragon_eggAI : public ScriptedAI
        {
            npc_dresaron_corrupted_dragon_eggAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            }

            InstanceScript* m_Instance;
            bool m_Destroyed;

            void Reset() override
            {
                m_Destroyed = false;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || p_Who->ToPlayer()->isGameMaster())
                    return;

                if ((p_Who->GetDistance(*me) < 2.0f) && !m_Destroyed)
                {
                    m_Destroyed = true;
                    DoCast(me, SPELL_SUM_BIRTH_WHELPLING_2, true);
                    me->Kill(me);
                }
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dresaron_corrupted_dragon_eggAI(p_Creature);
        }
};

/// Hatespawn Whelping - 101074
class npc_dresaron_hatespawn_whelpling : public CreatureScript
{
    public:
        npc_dresaron_hatespawn_whelpling() : CreatureScript("npc_dresaron_hatespawn_whelpling") { }

        struct npc_dresaron_hatespawn_whelplingAI : public ScriptedAI
        {
            npc_dresaron_hatespawn_whelplingAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 60.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (IsHeroicOrMythic())
                    DoCast(me, SPELL_HATESPAWN_DETONATION, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dresaron_hatespawn_whelplingAI(p_Creature);
        }
};

/// Bloodtainted Fury - 100531
class npc_dresaron_bloodtainted_fury : public CreatureScript
{
    public:
        npc_dresaron_bloodtainted_fury() : CreatureScript("npc_dresaron_bloodtainted_fury") { }

        enum eSpells
        {
            SpellBloodAssaultCast   = 201226,
            SpellBloodAssaultRemove = 201231,

            SpellBloodBomb          = 201272,

            SpellDispersion         = 201250
        };

        struct npc_dresaron_bloodtainted_furyAI : public ScriptedAI
        {
            npc_dresaron_bloodtainted_furyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, urand(4000, 9000));
                events.ScheduleEvent(EVENT_2, urand(10000, 30000));
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBloodAssaultCast)
                {
                    me->SetReactState(REACT_PASSIVE);

                    Position l_Pos = *me;

                    l_Pos.m_positionX += 30.0f * std::cos(me->m_orientation);
                    l_Pos.m_positionY += 30.0f * std::sin(me->m_orientation);
                    l_Pos.m_positionZ = me->GetMap()->GetHeight(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                    float l_Angle   = me->GetRelativeAngle(l_Pos.GetPositionX(), l_Pos.GetPositionY());
                    float l_Dist    = me->GetDistance(l_Pos);

                    me->GetFirstCollisionPosition(l_Pos, l_Dist, l_Angle);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveCharge(&l_Pos);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == EVENT_CHARGE)
                {
                    DoCast(me, eSpells::SpellBloodAssaultRemove);
                    me->SetReactState(REACT_AGGRESSIVE);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();

                        if (Unit* l_Target = me->getVictim())
                            AttackStart(l_Target);
                    });
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellDispersion, true);
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
                        DoCast(me, eSpells::SpellBloodAssaultCast);
                        events.ScheduleEvent(EVENT_1, urand(10000, 18000));
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellBloodBomb);

                        events.ScheduleEvent(EVENT_2, urand(20000, 35000));
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
            return new npc_dresaron_bloodtainted_furyAI(p_Creature);
        }
};

/// Down draft DOT - 220855
/// Called by Down draft - 199345
class spell_dresaron_down_draft : public SpellScriptLoader
{
    public:
        spell_dresaron_down_draft() : SpellScriptLoader("spell_dresaron_down_draft") { }

        class spell_dresaron_down_draft_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dresaron_down_draft_AuraScript);

            enum eSpells
            {
                DownDraftDOT = 220855
            };

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || l_Target->ToPlayer())
                    return;

                l_Target->CastSpell(l_Target, eSpells::DownDraftDOT);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Target->ToPlayer())
                    return;

                if (l_Target->HasAura(eSpells::DownDraftDOT))
                    l_Target->RemoveAura(eSpells::DownDraftDOT);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dresaron_down_draft_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dresaron_down_draft_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dresaron_down_draft_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_dresaron()
{
    /// Boss
    new boss_dresaron();

    /// Creatures
    new npc_dresaron_corrupted_dragon_egg();
    new npc_dresaron_hatespawn_whelpling();
    new npc_dresaron_bloodtainted_fury();

    /// Spell
    new spell_dresaron_down_draft();
}
#endif
