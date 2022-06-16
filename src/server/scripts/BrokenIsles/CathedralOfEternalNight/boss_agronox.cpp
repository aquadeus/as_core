#include "cathedral_of_eternal_night.hpp"

const Position g_LashersPositions[] =
{
    { -548.406f, 2539.18f, 388.452f },
    { -545.320f, 2548.58f, 388.452f },
    { -536.108f, 2550.91f, 388.215f },
    { -494.371f, 2540.62f, 388.215f },
    { -497.040f, 2535.12f, 388.215f },
    { -496.510f, 2547.46f, 388.215f },
    { -500.413f, 2541.02f, 388.215f },
    { -501.170f, 2549.28f, 388.215f },
    { -536.833f, 2568.32f, 388.452f },
    { -537.724f, 2562.05f, 388.452f },
    { -548.406f, 2539.18f, 388.452f },
    { -545.320f, 2548.58f, 388.452f },
    { -536.108f, 2550.91f, 388.215f },
    { -497.040f, 2535.12f, 388.215f },
    { -494.371f, 2540.62f, 388.215f },
    { -500.413f, 2541.02f, 388.215f },
    { -496.510f, 2547.46f, 388.215f },
    { -501.170f, 2549.28f, 388.215f },
    { -536.833f, 2568.32f, 388.452f },
    { -537.724f, 2562.05f, 388.452f }
};

const Position g_VinesPositions[] = 
{
    {-551.943f, 2500.690f, 388.452f },
    {-547.531f, 2545.000f, 388.452f },
    {-501.962f, 2530.680f, 388.452f },
    {-495.561f, 2493.160f, 388.452f }
};

enum eAgronoxData
{
    DataFixateTarget    = 1,
    DataBoomBlom,
};

/// Agronox - 117193
class boss_agronox : public CreatureScript
{
    public:
        boss_agronox() : CreatureScript("boss_agronox") { }

        enum eEvents
        {
            EventTimberSmash = 1,
            EventChokingViners,
            EventFulminatingLashers,
            EventSucculentLashers,
            EventPoisonousSpores,
        };

        enum eSpells
        {
            SpellPoisonousSporesDummy = 244338,
            SpellPoisonousSpores = 236524,
            SpellTimberSmash = 235751,
            SpellSucculentLasher = 236639,
            SpellFulminatingLashers = 236527,
            SpellFulminatinLasherSummon = 236592,
            SpellChokingVines = 238593,
        };

        enum eTalks
        {
            TalkAggro,
            TalkChokingVines,
            TalkSummoningLashers,
            TalkKill,
            TalkWipe,
            TalkDead,
        };

        struct boss_agronox_AI : public BossAI
        {
            explicit boss_agronox_AI(Creature* p_Me) : BossAI(p_Me, eData::DataAgronox) { }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);

                events.ScheduleEvent(eEvents::EventPoisonousSpores, 12 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFulminatingLashers, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTimberSmash, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventChokingViners, 23 * IN_MILLISECONDS);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventSucculentLashers, 18 * IN_MILLISECONDS);

                _EnterCombat();
            }

            void SetData(uint32 p_Id, uint32 p_Data) override
            {
                if (p_Id == eAgronoxData::DataBoomBlom)
                    m_FloralCount += p_Data;
            }

            void CleanUp()
            {
                me->RemoveAllAreasTrigger();
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellPoisonousSpores);

                std::list<Creature*> m_Flowers;

                me->GetCreatureListWithEntryInGrid(m_Flowers, eCreatures::NpcBuddingFlower, 250.f);

                for (Creature*& l_Itr : m_Flowers)
                    l_Itr->RemoveAllAuras();
            }

            void Reset() override
            {
                m_FloralCount = 0;
                _FulminatingLasherCastCount = 0;

                CleanUp();
                _Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                Talk(eTalks::TalkWipe);
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who == nullptr)
                    return;

                if (p_Who->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDead);
                _JustDied();
                CleanUp();

                if (IsMythic() && m_FloralCount >= 4)
                    instance->DoCompleteAchievement(eAchievements::BoomBloom);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (p_Summon == nullptr)
                    return;

                DoZoneInCombat(p_Summon, 250.f);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                switch (p_Spell->Id)
                {
                    case eSpells::SpellFulminatingLashers:
                    {
                        _FulminatingLasherCastCount++;

                        uint32 l_Lashers = urand(3, 6);

                        for (uint8 l_Itr = 0; l_Itr < l_Lashers; ++l_Itr)
                        {
                            Position l_Pos = g_LashersPositions[urand(0, 19)];
                            me->CastSpell(l_Pos, eSpells::SpellFulminatinLasherSummon, true);
                        }

                        break;
                    }

                    case eSpells::SpellChokingVines:
                    {
                        uint32 l_Vines = urand(1, 2);

                        for (uint8 l_Itr = 0; l_Itr < l_Vines; ++l_Itr)
                        {
                            Position l_Pos = g_VinesPositions[urand(0, 3)];
                            DoSummon(eCreatures::NpcChokingVines, l_Pos, 5000, TEMPSUMMON_CORPSE_TIMED_DESPAWN);
                        }

                        break;
                    }

                    default: break;
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventPoisonousSpores:
                    {
                        DoCastAOE(eSpells::SpellPoisonousSpores);
                        events.ScheduleEvent(eEvents::EventPoisonousSpores, 21 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFulminatingLashers:
                    {
                        if (_FulminatingLasherCastCount >= 2)
                            break;

                        Talk(eTalks::TalkSummoningLashers);
                        DoCastAOE(eSpells::SpellFulminatingLashers);
                        events.ScheduleEvent(eEvents::EventFulminatingLashers, 42 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventChokingViners:
                    {
                        Talk(eTalks::TalkChokingVines);
                        DoCast(me, eSpells::SpellChokingVines, true);
                        events.ScheduleEvent(eEvents::EventChokingViners, 40 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventTimberSmash:
                    {
                        DoCastVictim(eSpells::SpellTimberSmash);
                        events.ScheduleEvent(eEvents::EventTimberSmash, (IsMythic() ? 20 : 30) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSucculentLashers:
                    {
                        DoCastAOE(eSpells::SpellSucculentLasher);
                        events.ScheduleEvent(eEvents::EventSucculentLashers, 40 * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            bool _IsInFightArea()
            {
                Position l_ExitSrc = { -489.2169f, 2553.8604f, 389.8460f };
                Position l_ExitTgt = { -494.4563f, 2567.9570f, 390.1183f };

                return DistanceFromLine(l_ExitSrc, l_ExitTgt, *me) > 3.0f;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
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

                while (uint32 const l_EventId = events.ExecuteEvent())
                {
                    ExecuteEvent(l_EventId);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }

        private:
            uint8 _FulminatingLasherCastCount;
            uint8 m_FloralCount;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_agronox_AI(p_Me);
        }
};
/// Succulent Lasher - 119144
class npc_succulent_lasher : public CreatureScript
{
    public:
        npc_succulent_lasher() : CreatureScript("npc_succulent_lasher")
        {}

        enum eEvents
        {
            EventSucculentSecretion = 1
        };

        enum eSpells
        {
            SpellSucculentSecretion = 240061
        };

        struct npc_succulent_lasher_AI : public ScriptedAI
        {
            explicit npc_succulent_lasher_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void IsSummonedBy(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventSucculentSecretion, urand(5, 8) * IN_MILLISECONDS);
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 1.401f;
            }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvents::EventSucculentSecretion)
                {
                    DoCast(me, eSpells::SpellSucculentSecretion);
                    events.ScheduleEvent(eEvents::EventSucculentSecretion, 15 * IN_MILLISECONDS);
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_succulent_lasher_AI(p_Me);
        }
};

/// Choking Vines - 120014
class npc_choking_vines : public CreatureScript
{
    public:
        npc_choking_vines() : CreatureScript("npc_choking_vines")
        {}

        enum eEvents
        {
            EventChokingVines   = 1
        };

        enum eSpells
        {
            SpellChokingVines   = 238598,
            SpellPullTarget     = 170404,
        };

        struct npc_choking_vines_AI : public ScriptedAI
        {
            explicit npc_choking_vines_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
            }

            void EnterEvadeMode() override
            {
                /*Empty*/
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventChokingVines, 2 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventChokingVines, 2 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvents::EventChokingVines)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                    {
                        l_Target->DelayedCastSpell(me, eSpells::SpellPullTarget, true, 100);
                        me->DelayedCastSpell(l_Target, eSpells::SpellChokingVines, false, 1000);
                        return;
                    }
                    else
                        events.ScheduleEvent(eEvents::EventChokingVines, 500);
                }
            }

        private:
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_choking_vines_AI(p_Me);
        }
};

/// Fulminating Lashers - 121318
class npc_coen_fulminating_lasher : public CreatureScript
{
    public:
        npc_coen_fulminating_lasher() : CreatureScript("npc_coen_fulminating_lasher")
        {}

        enum eEvents
        {
            EventFixate = 1
        };

        enum eSpells
        {
            SpellFixate = 238674,
            SpellFloralFulmination = 238673
        };

        enum eActions
        {
            ActionTargetReached = 1
        };

        struct npc_coen_fulminating_lasher_AI : public ScriptedAI
        {
            explicit npc_coen_fulminating_lasher_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;

                if (me->GetEntry() == eCreatures::NpcFulminatingLasher)
                    p_ModHealth = 2.031576f;
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActions::ActionTargetReached)
                {
                    me->RemoveAllAuras();
                    me->AddUnitState(UNIT_STATE_ROOT);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    DoCast(me, eSpells::SpellFloralFulmination, false);
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellFloralFulmination)
                    events.ScheduleEvent(eEvents::EventFixate, urand(5, 10) * IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /**/) override
            {
                me->RemoveAllAuras();

                if (IsChallengeMode())
                    me->AddAura(ChallengeSpells::ChallengersMight, me);

                if (me->GetEntry() == eCreatures::NpcFulminatingLasher)
                    events.ScheduleEvent(eEvents::EventFixate, IN_MILLISECONDS);
                else
                    events.ScheduleEvent(eEvents::EventFixate, urand(6, 16) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                case eEvents::EventFixate:
                {
                    Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true, -eSpells::SpellFixate);

                    if (l_Target)
                        DoCast(l_Target, eSpells::SpellFixate);
                    else
                    {
                        l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true);

                        if (l_Target)
                            DoCast(l_Target, eSpells::SpellFixate);
                        else
                            events.ScheduleEvent(eEvents::EventFixate, 1 * IN_MILLISECONDS);
                    }

                    break;
                }

                default: break;
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

        private:
            uint64 m_FixateGUID;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_fulminating_lasher_AI(p_Me);
        }
};

/// Choking Vines - 238598
class spell_agronox_choking_vines : public SpellScriptLoader
{
    public:
        spell_agronox_choking_vines() : SpellScriptLoader("spell_agronox_choking_vines")
        {}

        class spell_agronox_choking_vines_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_agronox_choking_vines_AuraScript);

            void HandlePeriodic(AuraEffect const* p_AurEff)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                if (AuraEffect* p_Eff = const_cast<AuraEffect*>(p_AurEff))
                    p_Eff->SetAmount(p_Eff->GetAmount() * 1.1f);

                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();

                if (l_Caster->GetDistance2d(l_Owner) > 20.f)
                    l_Owner->Kill(l_Caster);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_agronox_choking_vines_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_agronox_choking_vines_AuraScript();
        }
};

/// Fixate - 238674
class spell_agronox_fixate : public SpellScriptLoader
{
    public:
        spell_agronox_fixate() : SpellScriptLoader("spell_agronox_fixate")
        {}

        class spell_agronox_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_agronox_fixate_AuraScript);

            void HandleOnUpdate(const uint32 p_Diff)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();

                l_Caster->GetMotionMaster()->MoveFollow(l_Owner, 0, 0);
            }

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                GetCaster()->GetMotionMaster()->Clear();
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_agronox_fixate_AuraScript::HandleOnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_agronox_fixate_AuraScript();
        }
};

/// Floral Fulmination - 236627
class spell_agronox_floral_fulmination : public SpellScriptLoader
{
    public:
        spell_agronox_floral_fulmination() : SpellScriptLoader("spell_agronox_floral_fulmination")
        {}
        
        class spell_agronox_floral_fulmination_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_agronox_floral_fulmination_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster() == nullptr || p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject* p_Itr)
                {
                    return p_Itr->GetEntry() != eCreatures::NpcBuddingFlower;
                });

                Unit* l_Caster = GetCaster();
                Creature* l_Boss = l_Caster->FindNearestCreature(eCreatures::BossAgronox, 250.f);

                if (l_Boss && l_Boss->IsAIEnabled)
                    l_Boss->GetAI()->SetData(eAgronoxData::DataBoomBlom, p_Targets.size());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_agronox_floral_fulmination_SpellScript::HandleTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_agronox_floral_fulmination_SpellScript();
        }
};

/// Fixate - 238674
class at_agronox_fixate : public AreaTriggerEntityScript
{
    public:
        at_agronox_fixate() : AreaTriggerEntityScript("at_agronox_fixate")
        {
            m_TargetGUID = 0;
        }

        enum eSpells
        {
            SpellFixate = 238674,
            SpellFloralFulmination  = 236627
        };


        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return false;

            if (!p_Target->IsPlayer())
                return true;

            if (p_Target->HasAura(eSpells::SpellFixate, p_At->GetCasterGUID()))
                p_At->GetCaster()->CastSpell(p_At->GetCaster(), eSpells::SpellFloralFulmination, false);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_agronox_fixate();
        }

    private:
        uint64 m_TargetGUID;
};

/// Succulent Secretion - 240063
class at_agronox_succulent_secretion : public AreaTriggerEntityScript
{
    public:
        at_agronox_succulent_secretion() : AreaTriggerEntityScript("at_agronox_succulent_secretion")
        {}

        void OnCreate(AreaTrigger* p_At) override
        {
            p_At->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 950);

            p_At->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 1);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.f);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.f);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 1.f); ///< (+200% / TimeToTargetScale / 1000) per second
            p_At->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1.40f);
        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            float l_VisualRadius = p_At->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);
            float l_CurrRadius = p_At->GetRadius();

            if (l_VisualRadius > 2.5f)
                return;

            l_VisualRadius += CalculatePct(1.f, 1.8f);
            l_CurrRadius += CalculatePct(1.f, 1.2f);

            p_At->SetRadius(l_CurrRadius);
            p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_VisualRadius); ///< (+200% / TimeToTargetScale / 1000) per second

        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_agronox_succulent_secretion();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_agronox()
{
    new boss_agronox();
    new npc_succulent_lasher();
    new npc_choking_vines();
    new npc_coen_fulminating_lasher();

    new spell_agronox_choking_vines();
    new spell_agronox_fixate();
    new spell_agronox_floral_fulmination();

    new at_agronox_fixate();
    new at_agronox_succulent_secretion();
}
#endif
