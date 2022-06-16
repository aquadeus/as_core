////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"

enum Spells
{
    SPELL_ARCANE_WEB_BEAM       = 209833,
    SPELL_BLINK_STRIKES_VISUAL  = 199864,
    SPELL_BLINK_STRIKES_SEARCH  = 199809,
    SPELL_BLINK_STRIKES_TP      = 199810,
    SPELL_BLINK_STRIKES_DMG     = 199811,
    SPELL_NETHER_VENOM          = 200024,
    SPELL_TANGLED_WEB           = 200227,
    SPELL_TANGLED_WEB_JUMP      = 200230,
    SPELL_TANGLED_WEB_DMG       = 200284,
    SPELL_TANGLED_BEAM          = 200237,

    /// Trash
    SPELL_DEVOUR                = 211543
};

enum eEvents
{
    EVENT_BLINK_STRIKES_1   = 1,
    EVENT_BLINK_STRIKES_2   = 2,
    EVENT_BLINK_STRIKE_BACK = 3,
    EVENT_NETHER_VENOM      = 4,
    EVENT_TANGLED_WEB       = 5,
    EVENT_SUM_MANAFANG      = 6
};

enum eActions
{
    ACTION_MOVE_UP          = 1,
    ACTION_MOVE_DOWN        = 2,
    ACTION_FALL_DOWN        = 3
};

/// Nal'tira - 98207
class boss_naltira : public CreatureScript
{
    public:
        boss_naltira() : CreatureScript("boss_naltira") { }

        struct boss_naltiraAI : public BossAI
        {
            boss_naltiraAI(Creature* p_Creature) : BossAI(p_Creature, DATA_NALTIRA)
            {
                me->SetUnitMovementFlags(MovementFlags::MOVEMENTFLAG_DISABLE_GRAVITY);
                me->AddUnitState(UNIT_STATE_UNATTACKABLE);
                m_CameDown = false;
                m_IsOnCeiling = false;
                m_SecondWaveStarted = false;
            }

            bool m_SecondWaveStarted;
            bool m_IsOnCeiling;
            bool m_CameDown;
            bool m_CheckWebPlayer;
            uint64 m_PlayerWebGuid;

            void Reset() override
            {
                BossAI::Reset();

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TANGLED_BEAM);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEVOUR);

                if (m_CameDown)
                {
                    me->SetReactState(REACT_AGGRESSIVE);

                    if (me->HasUnitState(UNIT_STATE_UNATTACKABLE))
                        me->ClearUnitState(UNIT_STATE_UNATTACKABLE);
                }
                else
                {
                    Creature* l_Trigger = GetClosestCreatureWithEntry(me, eCreatures::NPC_NALTIRA_TRIGGER, 100.0f, true);
                    me->CastSpell(l_Trigger, 209833, false);
                }

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
            }

            bool CheckTrash()
            {
                Creature* l_ViciousManafang = GetClosestCreatureWithEntry(me, eCreatures::NPC_VICIOUS_MANAFANG_2, 150.0f, true);
                Creature* l_EnchantedBrood = GetClosestCreatureWithEntry(me, eCreatures::NPC_ENCHANTED_BROODLING, 150.0f, true);

                if (l_ViciousManafang || l_EnchantedBrood)
                    return false;

                return true;
            }

            bool CheckSecondWave()
            {
                Creature* l_EnchantedBrood = GetClosestCreatureWithEntry(me, eCreatures::NPC_ENCHANTED_BROODLING, 150.0f, true);
                if (l_EnchantedBrood)
                    return false;

                return true;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (CheckTrash() && me->GetDistance(p_Who) < 100.0f && !m_CameDown && m_IsOnCeiling)
                {
                    m_CameDown = true;
                    uint64 l_GUID = me->GetGUID();
                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                    {
                        Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->DoAction(ACTION_MOVE_DOWN);
                    });
                }

                if (!CheckTrash() && me->GetDistance(p_Who) < 50.0f && !m_IsOnCeiling)
                {
                    m_IsOnCeiling = true;
                    uint64 l_GUID = me->GetGUID();
                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                    {
                        Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->DoAction(ACTION_MOVE_UP);
                    });
                }

                if (me->GetDistance(p_Who) < 100.0f && CheckSecondWave() && !m_SecondWaveStarted)
                {
                    m_SecondWaveStarted = true;
                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NPC_VICIOUS_MANAFANG_2, 150.0f);

                    for (Creature* l_Creature : l_CreatureList)
                    {
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->DoAction(ACTION_FALL_DOWN);
                    }
                }
                return;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_MOVE_UP:
                    {
                        if (Creature* l_Trigger = GetClosestCreatureWithEntry(me, eCreatures::NPC_NALTIRA_TRIGGER, 100.0f, true))
                        {
                            me->CastSpell(l_Trigger, 209833, false);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(0, l_Trigger->GetPositionX(), l_Trigger->GetPositionY(), l_Trigger->GetPositionZ(), false);
                        }
                        break;
                    }
                    case ACTION_MOVE_DOWN:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(0, me->GetHomePosition(), false);
                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastStop();
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (me->HasUnitState(UNIT_STATE_UNATTACKABLE))
                                me->ClearUnitState(UNIT_STATE_UNATTACKABLE);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                me->CastStop();

                events.ScheduleEvent(EVENT_BLINK_STRIKES_1, 15 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_NETHER_VENOM, 25 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_TANGLED_WEB, 35 * TimeConstants::IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_SUM_MANAFANG, 25 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                if (instance)
                    instance->SetBossState(eData::DATA_NALTIRA, EncounterState::FAIL);

                Position l_Pos = { 3143.9500, 4659.2300, 574.1888 };
                me->Respawn(true, true, 5 * TimeConstants::IN_MILLISECONDS);
                me->NearTeleportTo(l_Pos);

                BossAI::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                me->RemoveAllAreasTrigger();
                instance->DoRemoveAurasDueToSpellOnPlayers(200237);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (!p_Spell)
                    return;

                if (p_Spell->Id == Spells::SPELL_BLINK_STRIKES_DMG)
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
            }

            void OnRemoveAura(uint32 p_AuraId, AuraRemoveMode p_Mode) override
            {
                if (p_AuraId == Spells::SPELL_BLINK_STRIKES_DMG)
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                    AddTimedDelayedOperation(100, [this]() -> void
                    {
                        if (Unit* l_Victim = me->getVictim())
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_TANGLED_WEB_DMG:
                    {
                        if (m_CheckWebPlayer)
                        {
                            m_CheckWebPlayer = false;
                            m_PlayerWebGuid = p_Target->GetGUID();
                        }
                        else if (Unit* l_Target = Unit::GetUnit(*me, m_PlayerWebGuid))
                            p_Target->CastSpell(l_Target, SPELL_TANGLED_BEAM, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            bool _IsInFightArea()
            {
                Position l_ExitSrc = { 3194.7803f, 4655.6914f, 575.0903f, };
                Position l_ExitTgt = { 3188.3752f, 4637.1704f, 575.0903f, };

                Position l_EntranceSrc = { 3140.1484f, 4705.7822f, 574.7450f, };
                Position l_EntranceTgt = { 3145.0171f, 4705.9111f, 574.7450f, };

                return DistanceFromLine(l_ExitSrc, l_ExitTgt, *me) > 3.0f &&
                    DistanceFromLine(l_EntranceSrc, l_EntranceTgt, *me) > 3.0f;
            }

            void ExecuteEvent(uint32 const p_EventID) override
            {
                switch (p_EventID)
                {
                    case EVENT_BLINK_STRIKES_1:
                    {
                        DoCastAOE(Spells::SPELL_BLINK_STRIKES_SEARCH);
                        events.ScheduleEvent(EVENT_BLINK_STRIKES_1, 30 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EVENT_BLINK_STRIKES_2, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_BLINK_STRIKES_2:
                    {
                        Unit* l_Target = me->getVictim();

                        if (!l_Target)
                            return;

                        me->CastSpell(l_Target, SPELL_BLINK_STRIKES_TP, true);

                        AddTimedDelayedOperation(500, [this, l_TargetGUID = l_Target->GetGUID()]
                        {
                            Unit * l_Target = Unit::GetUnit(*me, l_TargetGUID);

                            me->GetMotionMaster()->Clear();
                            me->StopMoving();

                            if (l_Target)
                                me->SetFacingTo(me->GetAngle(l_Target));

                            me->CastSpell(me, SPELL_BLINK_STRIKES_DMG, false);
                        });

                        break;
                    }
                    case EVENT_NETHER_VENOM:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, Spells::SPELL_NETHER_VENOM);

                        events.ScheduleEvent(EVENT_NETHER_VENOM, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_TANGLED_WEB:
                    {
                        Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank);
                        if (!l_Target)
                            return;

                        m_PlayerWebGuid = 0;
                        m_CheckWebPlayer = true;

                        DoCast(l_Target, SPELL_TANGLED_WEB, false);

                        Talk(0);
                        events.ScheduleEvent(EVENT_TANGLED_WEB, 22 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_SUM_MANAFANG:
                    {
                        Position l_Pos;
                        me->GetRandomNearPosition(l_Pos, 30.0f);
                        me->SummonCreature(NPC_VICIOUS_MANAFANG, l_Pos);
                        events.ScheduleEvent(EVENT_SUM_MANAFANG, 22 * TimeConstants::IN_MILLISECONDS);
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

                if (!_IsInFightArea())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const p_EventId = events.ExecuteEvent())
                    ExecuteEvent(p_EventId);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_naltiraAI(p_Creature);
        }
};

/// Vicious Manafang - 110966
class npc_naltira_vicious_manafang : public CreatureScript
{
    public:
        npc_naltira_vicious_manafang() : CreatureScript("npc_naltira_vicious_manafang") { }

        struct npc_naltira_vicious_manafangAI : public ScriptedAI
        {
            npc_naltira_vicious_manafangAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_CameDown = false;
            }

            bool m_Summoned;
            bool m_CameDown;
            uint64 m_TargetGuid;
            int32 m_Health;

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* p_Killer) override
            {
                if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                    l_Target->RemoveAurasDueToSpell(SPELL_DEVOUR);
            }

            void EnterCombat(Unit* p_Target) override
            {
                m_Health = 0;
                if (me->GetDBTableGUIDLow() != 0)
                    events.ScheduleEvent(EVENT_1, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_Health = 0;
                DoZoneInCombat(me, 100.0f);
                events.ScheduleEvent(EVENT_1, urand(10, 15) * IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_FALL_DOWN:
                    {
                        if (me->GetDBTableGUIDLow() != 0 && !m_CameDown)
                        {
                            m_CameDown = true;
                            Creature* l_Trigger = me->SummonCreature(68553, me->GetHomePosition(), TEMPSUMMON_TIMED_DESPAWN, 10 * TimeConstants::IN_MILLISECONDS);
                            me->CastSpell(l_Trigger, 209833, false);
                            Position l_NewHome = { me->GetPositionX(), me->GetPositionY(), 574.185f, me->GetOrientation() };
                            me->SetHomePosition(l_NewHome);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(0, l_NewHome, false);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }


            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_DEVOUR)
                {
                    me->EnterVehicle(p_Target);
                    me->ClearUnitState(UNIT_STATE_ONVEHICLE);
                    m_Health = me->CountPctFromMaxHealth(30.0f);
                    m_TargetGuid = p_Target->GetGUID();
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Health)
                {
                    m_Health -= p_Damage;

                    if (m_Health <= 0)
                    {
                        m_Health = 0;

                        if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                            l_Target->RemoveAurasDueToSpell(SPELL_DEVOUR);
                    }
                }
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
                        DoCast(SPELL_DEVOUR);
                        events.ScheduleEvent(EVENT_2, 30 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_naltira_vicious_manafangAI(p_Creature);
        }
};

/// Tangled Web - 200227
class spell_naltira_tangled_web : public SpellScriptLoader
{
    public:
        spell_naltira_tangled_web() : SpellScriptLoader("spell_naltira_tangled_web") { }

        class spell_naltira_tangled_web_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_naltira_tangled_web_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->ToPlayer() && (p_Object->ToPlayer()->IsHealer() || p_Object->ToPlayer()->IsActiveSpecTankSpec()))
                        return true;

                    return false;
                });

                if (p_Targets.size() < 2)
                    p_Targets.clear();
            }

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetCaster(), SPELL_TANGLED_WEB_JUMP, true);
                GetCaster()->DelayedCastSpell(GetHitUnit(), SPELL_TANGLED_WEB_DMG, true, 500);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_naltira_tangled_web_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_naltira_tangled_web_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_naltira_tangled_web_SpellScript();
        }
};

/// Blink Strikes - 199809
class spell_naltira_blink_strikes_aoe : public SpellScript
{
    PrepareSpellScript(spell_naltira_blink_strikes_aoe);

    void HandleTargets(std::list<WorldObject*>& p_Targets)
    {
        if (p_Targets.empty())
            return;

        if (p_Targets.size() > 1)
        {
            p_Targets.remove_if([](WorldObject* p_Itr) -> bool
            {
                return p_Itr->IsPlayer() && p_Itr->ToPlayer()->IsActiveSpecTankSpec();
            });
        }
    }

    void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
    {
        Unit* l_Caster = GetCaster();

        if (!l_Caster)
            return;

        Unit* l_Target = GetHitUnit();

        if (!l_Target)
            return;

        l_Caster->CastSpell(l_Target, SPELL_BLINK_STRIKES_TP, true);

        l_Caster->AddDelayedEvent([l_Caster, l_TargetGUID = l_Target->GetGUID()]() -> void
        {
            Unit* l_Target = Unit::GetUnit(*l_Caster, l_TargetGUID);

            l_Caster->GetMotionMaster()->Clear();
            l_Caster->StopMoving();

            if (l_Target)
                l_Caster->SetFacingTo(l_Caster->GetAngle(l_Target));

            l_Caster->CastSpell(l_Caster, SPELL_BLINK_STRIKES_DMG, false);
        }, 500);

    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_naltira_blink_strikes_aoe::HandleTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_naltira_blink_strikes_aoe::HandleEffectHitTarget, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
    }
};

/// Tangled Web - 200237
class spell_tangled_web_dist_checker : public AuraScript
{
    PrepareAuraScript(spell_tangled_web_dist_checker);

    uint32 m_Timer = 0;

    void HandleAfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /**/)
    {
        Unit* l_Caster = GetCaster();
        Unit* l_Owner = GetUnitOwner();

        if (l_Owner)
        {
            l_Owner->DelayedRemoveAura(Spells::SPELL_TANGLED_BEAM, IN_MILLISECONDS);
            l_Owner->DelayedRemoveAura(Spells::SPELL_TANGLED_WEB_DMG, IN_MILLISECONDS);
        }

        if (l_Caster)
        {
            l_Owner->DelayedRemoveAura(Spells::SPELL_TANGLED_BEAM, IN_MILLISECONDS);
            l_Owner->DelayedRemoveAura(Spells::SPELL_TANGLED_WEB_DMG, IN_MILLISECONDS);
        }
    }

    void HandleOnUpdate(uint32 const p_Diff)
    {
        m_Timer += p_Diff;

        if (m_Timer < 500)
            return;

        m_Timer = 0;
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        Unit* l_Target = GetUnitOwner();
        if (!l_Target)
            return;

        if (l_Target->GetExactDist2d(l_Caster) >= 30.0f)
        {
            l_Target->DelayedRemoveAura(Spells::SPELL_TANGLED_BEAM, 1);
            l_Target->DelayedRemoveAura(Spells::SPELL_TANGLED_WEB_DMG, 1);

            l_Caster->DelayedRemoveAura(Spells::SPELL_TANGLED_BEAM, 1);
            l_Caster->DelayedRemoveAura(Spells::SPELL_TANGLED_WEB_DMG, 1);
            Remove();
        }
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_tangled_web_dist_checker::HandleOnUpdate);
        AfterEffectRemove += AuraEffectRemoveFn(spell_tangled_web_dist_checker::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_naltira()
{
    /// Boss
    new boss_naltira();

    /// Creature
    new npc_naltira_vicious_manafang();

    /// Spells
    new spell_naltira_tangled_web();
    RegisterAuraScript(spell_tangled_web_dist_checker);
    RegisterSpellScript(spell_naltira_blink_strikes_aoe);
}
#endif
