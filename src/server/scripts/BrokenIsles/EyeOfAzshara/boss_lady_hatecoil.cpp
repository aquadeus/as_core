////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "eye_of_azshara.hpp"

enum Says
{
    SAY_AGGRO,
    SAY_NOVA,
    SAY_NOVA_EMOTE,
    SAY_STORM_EMOTE,
    SAY_STORM,
    SAY_FOCUSED,
    SAY_DEATH
};

enum Spells
{
    SPELL_CRACKLING_THUNDER     = 197324,
    SPELL_CRACKLING_THUNDER_DMG = 197326,
    SPELL_STATIC_NOVA           = 193597,
    SPELL_CURSE_WITCH_1         = 193712, ///< MaxTargets 1
    SPELL_CURSE_WITCH_2         = 193716, ///< MaxTargets 3
    SPELL_CURSE_WITCH_3         = 193717, ///< MaxTargets 5
    SPELL_CURSE_WITCH_AURA      = 193698,
    SPELL_CURSE_WITCH_KNOCK     = 193700,
    SPELL_CURSE_WITCH_KILL_G    = 193720, ///< Hit npc: 98293
    SPELL_CURSE_WITCH_ROOT      = 194197,
    SPELL_BECKON_STORM          = 193682,
    SPELL_BECKON_STORM_SUMMON   = 193683,
    SPELL_FOCUSED_LIGHTNING     = 193611,
    SPELL_EXCESS_LIGHTNING      = 193625,
    SPELL_MONSOON_FILTER_1      = 196629, ///< MaxTargets 1
    SPELL_MONSOON_FILTER_2      = 196634, ///< MaxTargets 3
    SPELL_MONSOON_FILTER_3      = 196635, ///< MaxTargets 5
    SPELL_MONSOON_SUM           = 196630,
    SPELL_MONSOON_VISUAL        = 196609,
    SPELL_MONSOON_SEARCH_PLR    = 196624,
    SPELL_MONSOON_FIXATE        = 196622,
    SPELL_MONSOON_HIT           = 196614,
    SPELL_MONSOON_PLAYER_HIT    = 196610,
    SPELL_ARCANE_SHIELDING      = 197868,
    SPELL_SAND_DUNE_GO          = 193060,
    SPELL_SAND_DUNE_AT          = 193064,

    SPELL_BONUS_ROLL            = 226619
};

enum Creatures
{
    NPC_HATECOIL_ARCANIST = 97171
};

enum eEvents
{
    EVENT_CRACKLING_THUNDER     = 1,
    EVENT_STATIC_NOVA           = 2,
    EVENT_CURSE_WITCH           = 3,
    EVENT_BECKON_STORM          = 4,
    EVENT_FOCUSED_LIGHTNING     = 5,
    EVENT_MONSOON               = 6  ///< Heroic
};

uint32 g_CurseSpells[3] =
{
    SPELL_CURSE_WITCH_1,
    SPELL_CURSE_WITCH_2,
    SPELL_CURSE_WITCH_3
};

Position const g_HomePos = { -3443.38f, 4590.50f, 0.0f, 0.0f };

Position const g_NagaPos[3] =
{
    { -3358.20f, 4634.52f, 0.45f, 2.76f },
    { -3457.37f, 4760.65f, 4.28f, 3.50f },
    { -3555.38f, 4741.02f, 4.76f, 5.77f }
};

/// Lady Hatecoil - 91789
class boss_lady_hatecoil : public CreatureScript
{
    public:
        boss_lady_hatecoil() : CreatureScript("boss_lady_hatecoil") { }

        struct boss_lady_hatecoilAI : public BossAI
        {
            boss_lady_hatecoilAI(Creature* p_Creature) : BossAI(p_Creature, DATA_HATECOIL)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                SummonNagas();
                m_NagaDiedCount = 0;
            }

            uint8 m_NagaDiedCount;
            uint8 m_MonsoonCount;

            bool m_Achievement = false;

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
                _Reset();
                m_MonsoonCount = 0;

                m_Achievement = false;
            }

            void StartEvents()
            {
                events.ScheduleEvent(EVENT_CRACKLING_THUNDER, 4000);
                events.ScheduleEvent(EVENT_STATIC_NOVA, 11000);
                events.ScheduleEvent(EVENT_CURSE_WITCH, 17000);
                events.ScheduleEvent(EVENT_BECKON_STORM, 19000);
                events.ScheduleEvent(EVENT_FOCUSED_LIGHTNING, 25000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_MONSOON, 32000);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);

                _EnterCombat();
                StartEvents();
            }

            void JustReachedHome() override
            {
                Reset();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);

                if (instance && IsMythic() && m_Achievement)
                    instance->DoCompleteAchievement(eEoAAchievements::StaySalty);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_CURSE_WITCH_1:
                    case SPELL_CURSE_WITCH_2:
                    case SPELL_CURSE_WITCH_3:
                    {
                        DoCast(p_Target, SPELL_CURSE_WITCH_AURA, true);
                        break;
                    }
                    case SPELL_MONSOON_FILTER_1:
                    case SPELL_MONSOON_FILTER_2:
                    case SPELL_MONSOON_FILTER_3:
                    {
                        DoCast(p_Target, SPELL_MONSOON_SUM, true);
                        break;
                    }
                    case SPELL_BECKON_STORM:
                    {
                        DoCast(p_Target, SPELL_BECKON_STORM_SUMMON, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                m_Achievement = p_Value != 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 50.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if (events.Empty())
                    StartEvents();

                switch (events.ExecuteEvent())
                {
                    case EVENT_CRACKLING_THUNDER:
                    {
                        DoCast(me, SPELL_CRACKLING_THUNDER, true);
                        events.ScheduleEvent(EVENT_CRACKLING_THUNDER, 4000);
                        break;
                    }
                    case EVENT_STATIC_NOVA:
                    {
                        DoCast(SPELL_STATIC_NOVA);
                        Talk(SAY_NOVA);
                        Talk(SAY_NOVA_EMOTE);
                        events.ScheduleEvent(EVENT_STATIC_NOVA, 35000);
                        break;
                    }
                    case EVENT_CURSE_WITCH:
                    {
                        DoCast(g_CurseSpells[urand(0, 2)]);
                        events.ScheduleEvent(EVENT_CURSE_WITCH, 24000);
                        break;
                    }
                    case EVENT_BECKON_STORM:
                    {
                        DoCast(SPELL_BECKON_STORM);
                        Talk(SAY_STORM_EMOTE);
                        Talk(SAY_STORM);
                        events.ScheduleEvent(EVENT_BECKON_STORM, 47000);
                        break;
                    }
                    case EVENT_FOCUSED_LIGHTNING:
                    {
                        DoCast(SPELL_FOCUSED_LIGHTNING);
                        Talk(SAY_FOCUSED);
                        events.ScheduleEvent(EVENT_FOCUSED_LIGHTNING, 36000);
                        break;
                    }
                    case EVENT_MONSOON:
                    {
                        if (m_MonsoonCount < 2)
                            DoCast(SPELL_MONSOON_FILTER_1);
                        else if (m_MonsoonCount >= 2 && m_MonsoonCount <= 5)
                            DoCast(SPELL_MONSOON_FILTER_2);
                        else if (m_MonsoonCount > 5)
                            DoCast(SPELL_MONSOON_FILTER_3);
                        m_MonsoonCount++;
                        events.ScheduleEvent(EVENT_MONSOON, 21000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void SummonNagas()
            {
                for (uint8 l_I = 0; l_I < 3; l_I++)
                {
                    me->SummonCreature(NPC_HATECOIL_ARCANIST, g_NagaPos[l_I]);

                    if (l_I < 2)
                        DoCast(me, SPELL_ARCANE_SHIELDING, true);
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case NPC_HATECOIL_ARCANIST:
                    {
                        m_NagaDiedCount++;
                        break;
                    }
                    default:
                        return;
                }

                if (m_NagaDiedCount == 2)
                {
                    me->RemoveAurasDueToSpell(SPELL_ARCANE_SHIELDING);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetReactState(REACT_AGGRESSIVE);
                    _Reset();

                    if (p_Summon->IsAIEnabled)
                        p_Summon->AI()->ZoneTalk(1);
                }
                else if (m_NagaDiedCount == 1)
                {
                    me->SetAuraStack(SPELL_ARCANE_SHIELDING, me, 1);

                    if (p_Summon->IsAIEnabled)
                        p_Summon->AI()->ZoneTalk(0);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_lady_hatecoilAI(p_Creature);
        }
};

/// Sand Dune - 97853
class npc_hatecoil_sand_dune : public CreatureScript
{
    public:
        npc_hatecoil_sand_dune() : CreatureScript("npc_hatecoil_sand_dune") { }

        struct npc_hatecoil_sand_duneAI : public ScriptedAI
        {
            npc_hatecoil_sand_duneAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            uint64 m_DustGuid;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_SAND_DUNE_GO, true);
                DoCast(me, SPELL_SAND_DUNE_AT, true);
            }

            void JustSummonedGO(GameObject* p_GameObject) override
            {
                m_DustGuid = p_GameObject->GetGUID();
            }

            void JustDespawned() override
            {
                me->RemoveAllAreasTrigger();

                if (GameObject* l_GameObject = me->GetMap()->GetGameObject(m_DustGuid))
                    l_GameObject->Delete();
            }

            void SpellHit(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_EXCESS_LIGHTNING || p_SpellInfo->Id == SPELL_MONSOON_HIT)
                {
                    me->RemoveAurasDueToSpell(SPELL_SAND_DUNE_AT);

                    if (GameObject* l_GameObject = me->GetMap()->GetGameObject(m_DustGuid))
                        l_GameObject->Delete();

                    me->DespawnOrUnsummon(500);
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hatecoil_sand_duneAI(p_Creature);
        }
};

/// Moonsoon - 99852
class npc_hatecoil_monsoon : public CreatureScript
{
    public:
        npc_hatecoil_monsoon() : CreatureScript("npc_hatecoil_monsoon") { }

        struct npc_hatecoil_monsoonAI : public ScriptedAI
        {
            npc_hatecoil_monsoonAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            bool m_Spawned;
            uint16 m_SearchPlrTimer;
            uint64 m_PlayerTargetGuid = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 100.0f);
                m_SearchPlrTimer = 200;
                m_Spawned = false;
                me->SetSpeed(MOVE_SWIM, 0.20f);
                DoCast(me, SPELL_MONSOON_VISUAL, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_MONSOON_FIXATE:
                    {
                        me->AddThreat(p_Target, 10000.0f);
                        AttackStart(p_Target);
                        m_PlayerTargetGuid = p_Target->GetGUID();
                        break;
                    }
                    case SPELL_MONSOON_HIT:
                    {
                        me->DespawnOrUnsummon(50);
                        break;
                    }
                    case SPELL_MONSOON_PLAYER_HIT:
                    {
                        me->DespawnOrUnsummon(50);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_SearchPlrTimer <= p_Diff)
                {
                    if (!m_PlayerTargetGuid)
                        DoCast(me, SPELL_MONSOON_SEARCH_PLR, true);
                    else if (Player* l_Player = Player::GetPlayer(*me, m_PlayerTargetGuid))
                    {
                        if (l_Player->isAlive())
                        {
                            if (!m_Spawned)
                            {
                                m_Spawned = true;
                                DoCast(me, SPELL_MONSOON_VISUAL, true);
                            }
                        }
                    }

                    m_SearchPlrTimer = 2000;
                }
                else
                {
                    if (Player* l_Player = Player::GetPlayer(*me, m_PlayerTargetGuid))
                        me->GetMotionMaster()->MovePoint(1, *l_Player);

                    m_SearchPlrTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hatecoil_monsoonAI(p_Creature);
        }
};

/// Crackling Thunder - 197324
class spell_crackling_thunder_filter : public SpellScriptLoader
{
    public:
        spell_crackling_thunder_filter() : SpellScriptLoader("spell_crackling_thunder_filter") { }

        class spell_crackling_thunder_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_crackling_thunder_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                std::list<WorldObject*> l_TempList;
                for (std::list<WorldObject*>::const_iterator l_Iter = p_Targets.begin(); l_Iter != p_Targets.end(); ++l_Iter)
                {
                    if ((*l_Iter)->GetDistance(g_HomePos) > 45.0f)
                        l_TempList.push_back((*l_Iter));
                }

                p_Targets.clear();
                p_Targets = l_TempList;
            }

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_CRACKLING_THUNDER_DMG, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crackling_thunder_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_crackling_thunder_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_crackling_thunder_filter_SpellScript();
        }
};

/// Curse of the Witch - 193698
class spell_hatecoil_curse_of_the_witch : public SpellScriptLoader
{
    public:
        spell_hatecoil_curse_of_the_witch() : SpellScriptLoader("spell_hatecoil_curse_of_the_witch") { }

        class spell_hatecoil_curse_of_the_witch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hatecoil_curse_of_the_witch_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    /// Main target isn't affected by knock back
                    l_Target->CastSpell(l_Target, SPELL_CURSE_WITCH_KNOCK, true, nullptr, nullptr, GetCasterGUID());
                    l_Target->CastSpell(l_Target, SPELL_CURSE_WITCH_KILL_G, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hatecoil_curse_of_the_witch_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hatecoil_curse_of_the_witch_AuraScript();
        }
};

/// Curse of the Witch (Cone) - 193720
class spell_hatecoil_curse_of_the_witch_cone : public SpellScriptLoader
{
    public:
        spell_hatecoil_curse_of_the_witch_cone() : SpellScriptLoader("spell_hatecoil_curse_of_the_witch_cone") { }

        class spell_hatecoil_curse_of_the_witch_cone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hatecoil_curse_of_the_witch_cone_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                if (!l_Instance)
                    return;

                Creature* l_Lady = Creature::GetCreature(*l_Caster, l_Instance->GetData64(NPC_LADY_HATECOIL));
                if (!l_Lady || !l_Lady->IsAIEnabled)
                    return;

                uint32 l_Count = 0;
                for (WorldObject* l_Object : p_Targets)
                {
                    if (l_Object->GetEntry() == NPC_SALTSEA_GLOBULE)
                        ++l_Count;
                }

                /// Validate achievement criteria
                if (l_Count >= 11)
                    l_Lady->AI()->SetData(0, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hatecoil_curse_of_the_witch_cone_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hatecoil_curse_of_the_witch_cone_SpellScript();
        }
};

/// Curse of the Witch (Knock Back) - 193700
class spell_hatecoil_curse_of_the_witch_knock : public SpellScriptLoader
{
    public:
        spell_hatecoil_curse_of_the_witch_knock() : SpellScriptLoader("spell_hatecoil_curse_of_the_witch_knock") { }

        class spell_hatecoil_curse_of_the_witch_knock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hatecoil_curse_of_the_witch_knock_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Targets.empty())
                    return;

                p_Targets.remove(l_Caster);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hatecoil_curse_of_the_witch_knock_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hatecoil_curse_of_the_witch_knock_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_lady_hatecoil()
{
    /// Boss
    new boss_lady_hatecoil();

    /// Creatures
    new npc_hatecoil_sand_dune();
    new npc_hatecoil_monsoon();

    /// Spells
    new spell_crackling_thunder_filter();
    new spell_hatecoil_curse_of_the_witch();
    new spell_hatecoil_curse_of_the_witch_cone();
    new spell_hatecoil_curse_of_the_witch_knock();
}
#endif
