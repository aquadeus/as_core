////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "darkheart_thicket.hpp"

enum Says
{
    SAY_AGGRO   = 0,
    SAY_DEATH   = 1,
    SAY_END     = 0,
};

enum Spells
{
    SPELL_SHADE_VISUAL                  = 195106,
    SPELL_FESTERING_RIP                 = 200182,
    SPELL_NIGHTMARE_BOLT                = 204808,
    SPELL_FEED_ON_THE_WEAK              = 200238,
    SPELL_INDUCED_PARANOIA              = 200359,
    SPELL_COWARDICE                     = 200273,
    SPELL_WAKING_NIGHTMARE              = 200243,
    SPELL_GROWING_PARANOIA              = 200289,

    /// Final Phase 50%HP
    SPELL_PHASE_CHANGE_CONVERSATION     = 199857,
    SPELL_APOCALYPTIC_NIGHTMARE         = 200050,
    SPELL_APOCALYPTIC_NIGHTMARE_BOLT    = 200067,
    SPELL_DRAIN_ESSENCE                 = 199837,

    /// Cage
    SPELL_NIGHTMARE_SHIELD              = 204511,
    SPELL_NIGHTMARE_BINDINGS            = 199752,

    /// Malfurion
    SPELL_NATURE_RECOVERY               = 204680,
    SPELL_ESCAPES_CONVERSATION          = 199912,
    SPELL_NATURE_DOMINANCE              = 199922,

    SPELL_BONUS_ROLL                    = 226615
};

enum eEvents
{
    EVENT_FESTERING_RIP     = 1,
    EVENT_NIGHTMARE_BOLT    = 2,
    EVENT_FEED_ON_THE_WEAK  = 3,
    EVENT_INDUCED_PARANOIA  = 4,
    EVENT_FINAL_PHASE       = 5
};

/// Shade of Xavius - 99192
class boss_shade_of_xavius : public CreatureScript
{
    public:
        boss_shade_of_xavius() : CreatureScript("boss_shade_of_xavius") { }

        struct boss_shade_of_xaviusAI : public BossAI
        {
            boss_shade_of_xaviusAI(Creature* p_Creature) : BossAI(p_Creature, DATA_XAVIUS) { }

            bool m_FinalPhase;

            void Reset() override
            {
                _Reset();

                summons.DespawnAll();

                m_FinalPhase = false;

                DoCast(me, SPELL_SHADE_VISUAL, true);

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    if (instance != nullptr)
                    {
                        if (Creature* l_Malfurion = instance->instance->GetCreature(instance->GetData64(NPC_MALFURION_STORMRAGE)))
                            l_Malfurion->DespawnOrUnsummon();
                    }

                    if (Creature* l_Bindings = me->SummonCreature(NPC_NIGHTMARE_BINDINGS, 2686.353f, 1290.243f, 131.8871f, 0.7438113f))
                        l_Bindings->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                CHECK_REQUIRED_BOSSES_AND_TELEPORT(DATA_XAVIUS, g_StartPos);

                Talk(SAY_AGGRO);
                _EnterCombat();
                events.ScheduleEvent(EVENT_FESTERING_RIP,    3000);
                events.ScheduleEvent(EVENT_NIGHTMARE_BOLT,   9000);
                events.ScheduleEvent(EVENT_FEED_ON_THE_WEAK, 14000);
                events.ScheduleEvent(EVENT_INDUCED_PARANOIA, 19000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (!p_Target || !p_Spell)
                    return;

                if (p_Spell->Id == Spells::SPELL_INDUCED_PARANOIA)
                    me->CastSpell(p_Target, Spells::SPELL_GROWING_PARANOIA, false);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
                summons.DespawnAll();

                if (instance != nullptr)
                {
                    if (Creature* l_Malfurion = instance->instance->GetCreature(instance->GetData64(NPC_MALFURION_STORMRAGE)))
                    {
                        if (l_Malfurion->IsAIEnabled)
                            l_Malfurion->AI()->Talk(SAY_END);

                        l_Malfurion->InterruptNonMeleeSpells(false, SPELL_NATURE_DOMINANCE);
                    }
                }

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == NPC_NIGHTMARE_BINDINGS)
                {
                    p_Summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                    p_Summon->SetReactState(REACT_PASSIVE);
                    p_Summon->CastSpell(p_Summon, SPELL_NIGHTMARE_SHIELD, true);
                    p_Summon->CastSpell(p_Summon, SPELL_NIGHTMARE_BINDINGS, true);
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == NPC_NIGHTMARE_BINDINGS && instance)
                {
                    if (Creature* l_Malfurion = instance->instance->GetCreature(instance->GetData64(NPC_MALFURION_STORMRAGE)))
                    {
                        if (l_Malfurion->IsAIEnabled)
                            l_Malfurion->AI()->DoAction(ACTION_1);
                    }

                    me->InterruptNonMeleeSpells(false, SPELL_DRAIN_ESSENCE);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(50, p_Damage) && !m_FinalPhase)
                {
                    m_FinalPhase = true;
                    events.ScheduleEvent(EVENT_FINAL_PHASE, 1000);
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (roll_chance_f(33.33f))
                {
                    Position l_RoomCenter   = { 2712.756f, 1329.71f, 128.368f, 4.434f };
                    Position l_Pos          = Position();

                    l_RoomCenter.GetRandomNearPosition(l_Pos, 40.0f, me);

                    if (l_Pos.IsNearPosition(&l_RoomCenter, 1.0f))
                        l_RoomCenter.GetRandomNearPosition(l_Pos, 30.0f, me);

                    me->CastSpell(l_Pos, SPELL_APOCALYPTIC_NIGHTMARE_BOLT, true);
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

                if (me->GetDistance(me->GetHomePosition()) >= 80.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_FESTERING_RIP:
                    {
                        if (Player* l_Target = SelectMeleeTarget(true))
                            DoCast(l_Target, SPELL_FESTERING_RIP);
                        events.ScheduleEvent(EVENT_FESTERING_RIP, 17000);
                        break;
                    }
                    case EVENT_NIGHTMARE_BOLT:
                    {
                        DoCast(SPELL_NIGHTMARE_BOLT);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BOLT, 17000);
                        break;
                    }
                    case EVENT_FEED_ON_THE_WEAK:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                            DoCast(l_Target, SPELL_FEED_ON_THE_WEAK);
                        events.ScheduleEvent(EVENT_FEED_ON_THE_WEAK, 30000);
                        break;
                    }
                    case EVENT_INDUCED_PARANOIA:
                    {
                        DoCast(SPELL_INDUCED_PARANOIA);
                        events.ScheduleEvent(EVENT_INDUCED_PARANOIA, 21000);
                        break;
                    }
                    case EVENT_FINAL_PHASE:
                    {
                        DoCast(me, SPELL_PHASE_CHANGE_CONVERSATION, true);
                        DoCast(me, SPELL_APOCALYPTIC_NIGHTMARE, true);

                        if (instance != nullptr)
                        {
                            if (Creature* l_Cage = instance->instance->GetCreature(instance->GetData64(NPC_NIGHTMARE_BINDINGS)))
                            {
                                l_Cage->RemoveAurasDueToSpell(SPELL_NIGHTMARE_SHIELD);
                                l_Cage->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                            }
                        }

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
            return new boss_shade_of_xaviusAI(p_Creature);
        }
};

/// Malfurion Stormrage - 100652
class npc_xavius_malfurion_stormrage : public CreatureScript
{
    public:
        npc_xavius_malfurion_stormrage() : CreatureScript("npc_xavius_malfurion_stormrage") { }

        struct npc_xavius_malfurion_stormrageAI : public ScriptedAI
        {
            npc_xavius_malfurion_stormrageAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
            }

            InstanceScript* m_Instance;

            void Reset() override { }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == ACTION_1)
                {
                    if (m_Instance && m_Instance->GetBossState(DATA_XAVIUS) == IN_PROGRESS)
                        me->GetMotionMaster()->MoveJump(2713.19f, 1329.85f, 128.36f, 10.0f, 10.0f);
                }
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;

                DoCast(me, SPELL_NATURE_RECOVERY, true);
                events.ScheduleEvent(EVENT_1, 20000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_ESCAPES_CONVERSATION, true);
                        DoCast(SPELL_NATURE_DOMINANCE);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport me to Dalaran.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            p_Player->SEND_GOSSIP_MENU(p_Player->GetGossipTextId(p_Creature), p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();

            if (p_Action == GOSSIP_ACTION_INFO_DEF)
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                p_Player->TeleportTo(1220, -835.19f, 4283.43f, 746.258f, 4.3735f);
            }

            return true;
        }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_malfurion_stormrageAI(p_Creature);
        }
};

/// Nightmare Bolt - 200185
class spell_xavius_nightmare_bolt : public SpellScriptLoader
{
    public:
        spell_xavius_nightmare_bolt() : SpellScriptLoader("spell_xavius_nightmare_bolt") { }

        class spell_xavius_nightmare_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_nightmare_bolt_SpellScript);

            void HandleDummy(SpellEffIndex p_EffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->IsHeroicOrMythic())
                        return;
                    else
                        PreventHitDefaultEffect(p_EffIndex);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_xavius_nightmare_bolt_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_nightmare_bolt_SpellScript();
        }
};

/// Curse of Isolation - 201839
class spell_xavius_curse_of_isolation : public SpellScriptLoader
{
    public:
        spell_xavius_curse_of_isolation() : SpellScriptLoader("spell_xavius_curse_of_isolation") { }

        enum eSpell
        {
            CurseOfIsolationAoE = 201842
        };

        class spell_xavius_curse_of_isolation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_curse_of_isolation_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, eSpell::CurseOfIsolationAoE, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xavius_curse_of_isolation_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_curse_of_isolation_AuraScript();
        }
};

/// Apocalyptic Empowerment - 221315
class spell_xavius_apocalyptic_empowerment : public SpellScriptLoader
{
    public:
        spell_xavius_apocalyptic_empowerment() : SpellScriptLoader("spell_xavius_apocalyptic_empowerment")
        {}

        class spell_xavius_apocalyptic_empowerment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_apocalyptic_empowerment_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr || GetTarget() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                {
                    uint8 l_Stacks = GetStackAmount();

                    if (l_Stacks >= 10)
                    {
                        InstanceScript* l_Instance = l_Owner->GetInstanceScript();

                        if (l_Instance && l_Instance->instance->IsMythic())
                            l_Instance->DoCompleteAchievement(eAchievements::BurningDownTheHouse);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xavius_apocalyptic_empowerment_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_apocalyptic_empowerment_AuraScript();
        }
};

// Growing Paranoia - 200289
class spell_xavius_growing_paranoia : public SpellScript
{
    PrepareSpellScript(spell_xavius_growing_paranoia);

    void HandleAfterHit()
    {
        Unit* l_Target = GetHitUnit();

        if (!l_Target)
            return;

        l_Target->DelayedRemoveAura(Spells::SPELL_WAKING_NIGHTMARE, 100);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_xavius_growing_paranoia::HandleAfterHit);
    }
};

// Waking Nightmare - 200243
class spell_xavius_waking_nightmare : public SpellScript
{
    PrepareSpellScript(spell_xavius_waking_nightmare);

    void HandleAfterHit()
    {
        Unit* l_Target = GetHitUnit();

        if (!l_Target)
            return;

        l_Target->DelayedRemoveAura(Spells::SPELL_GROWING_PARANOIA, 100);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_xavius_waking_nightmare::HandleAfterHit);
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_shade_of_xavius()
{
    /// Boss
    new boss_shade_of_xavius();

    /// Creature
    new npc_xavius_malfurion_stormrage();

    /// Spells
    new spell_xavius_nightmare_bolt();
    new spell_xavius_curse_of_isolation();
    new spell_xavius_apocalyptic_empowerment();
    
    RegisterSpellScript(spell_xavius_growing_paranoia);
    RegisterSpellScript(spell_xavius_waking_nightmare);
}
#endif
