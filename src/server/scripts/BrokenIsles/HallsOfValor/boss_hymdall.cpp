////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "halls_of_valor.hpp"

enum Says
{
    SAY_AGGRO,
    SAY_HORN,
    SAY_HORN_EMOTE,
    SAY_BLOODLETTING,
    SAY_DEATH,
    SAY_INTRO,
    SAY_WIPE,
    SAY_SLAY
};

enum Spells
{
    SPELL_HORN_OF_VALOR             = 191284,
    SPELL_DANCING_BLADE             = 193235,
    SPELL_DANCING_BLADE_AT          = 193214,
    SPELL_BLOODLETTING_SWEEP        = 193092,
    SPELL_CRACKLE_MISSILE           = 199807,

    /// Heroic
    SPELL_BALL_LIGHTNING_AT_FRONT   = 193240,
    SPELL_BALL_LIGHTNING_AT_BACK    = 188405,

    /// Drake
    SPELL_STORM_BREATH_VIS          = 188404,
    SPELL_STORM_BREATH_AT           = 192959,

    SPELL_HOV_SCENARIO_INTRO        = 202036
};

enum eEvents
{
    EVENT_HORN_OF_VALOR         = 1,
    EVENT_SUM_S_DRAKE           = 2,
    EVENT_DANCING_BLADE         = 3,
    EVENT_BLOODLETTING_SWEEP    = 4
};

Position const g_DrakePos[6] =
{
    { 3566.12f, 675.0f,  626.44f, 4.71f },
    { 3566.12f, 383.00f, 626.44f, 1.60f },
    { 3534.45f, 675.02f, 626.44f, 4.71f },
    { 3534.45f, 383.00f, 626.44f, 1.60f },
    { 3502.45f, 675.04f, 626.44f, 4.71f },
    { 3502.45f, 383.00f, 626.44f, 1.60f }
};

/// Hymdall <The Guardian> - 94960
class boss_hymdall : public CreatureScript
{
    public:
        boss_hymdall() : CreatureScript("boss_hymdall") { }

        struct boss_hymdallAI : public BossAI
        {
            boss_hymdallAI(Creature* p_Creature) : BossAI(p_Creature, DATA_HYMDALL)
            {
                m_IntroDone = false;
                m_EncounterDone = instance ? instance->GetBossState(DATA_HYMDALL) == DONE : false;
            }

            bool m_IntroDone;
            bool m_EncounterDone;

            uint32 m_IntroCheckTimer = 1 * TimeConstants::IN_MILLISECONDS;

            uint8 m_RandDrakePos;

            void Reset() override
            {
                if (m_EncounterDone)
                {
                    me->setFaction(35);
                    me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    return;
                }

                _Reset();

                m_RandDrakePos = 0;

                if (m_IntroDone)
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);

                _EnterCombat();

                events.ScheduleEvent(EVENT_HORN_OF_VALOR, 6000);
                events.ScheduleEvent(EVENT_DANCING_BLADE, 5000);
                events.ScheduleEvent(EVENT_BLOODLETTING_SWEEP, 20000);
            }

            void JustReachedHome() override
            {
                Talk(Says::SAY_WIPE);
                BossAI::JustReachedHome();
            }

            void EnterEvadeMode() override
            {
                if (m_EncounterDone)
                {
                    summons.DespawnAll();
                    return;
                }

                BossAI::EnterEvadeMode();
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_EncounterDone)
                    return;

                if (p_Damage > me->GetHealth())
                {
                    p_Damage = 0;
                    me->SetHealth(1);
                    EndOfEncounter();
                    return;
                }

                if (me->HealthBelowPctDamaged(10, p_Damage))
                {
                    p_Damage = 0;
                    EndOfEncounter();
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == NPC_DANCING_BLADE)
                {
                    p_Summon->SetReactState(REACT_PASSIVE);
                    p_Summon->CastSpell(p_Summon, SPELL_DANCING_BLADE_AT, true);
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed != nullptr && p_Killed->IsPlayer())
                    Talk(SAY_SLAY);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_IntroDone && m_IntroCheckTimer)
                {
                    if (m_IntroCheckTimer <= p_Diff)
                    {
                        if (Player* l_Player = me->FindNearestPlayer(250.0f))
                        {
                            l_Player->CastSpell(l_Player, SPELL_HOV_SCENARIO_INTRO, true);

                            m_IntroDone = true;
                            m_IntroCheckTimer = 0;

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        else
                            m_IntroCheckTimer = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_IntroCheckTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                Position l_Pos = me->GetHomePosition();
                if (me->GetExactDist(&l_Pos) > 74.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_HORN_OF_VALOR:
                    {
                        Talk(SAY_HORN);
                        Talk(SAY_HORN_EMOTE);

                        DoCast(SPELL_HORN_OF_VALOR);

                        m_RandDrakePos = urand(0, 5);

                        for (uint8 l_I = 0; l_I < (IsMythic() ? 3 : 1); l_I++)
                            events.ScheduleEvent(EVENT_SUM_S_DRAKE, 3 * (l_I + 1) * IN_MILLISECONDS);

                        events.ScheduleEvent(EVENT_HORN_OF_VALOR, 30000);
                        break;
                    }
                    case EVENT_SUM_S_DRAKE:
                    {
                        me->SummonCreature(NPC_STORM_DRAKE, g_DrakePos[m_RandDrakePos]);

                        m_RandDrakePos += 2;

                        if (m_RandDrakePos > 5)
                            m_RandDrakePos = 0;

                        break;
                    }
                    case EVENT_DANCING_BLADE:
                    {
                        Unit* l_Target = SelectRangedTarget();

                        if (l_Target == nullptr)
                        {
                            l_Target = SelectMeleeTarget(false);

                            if (l_Target == nullptr)
                                l_Target = me->getVictim();
                        }

                        if (l_Target != nullptr)
                            DoCast(l_Target, Spells::SPELL_DANCING_BLADE);

                        events.ScheduleEvent(EVENT_DANCING_BLADE, 32000);
                        break;
                    }
                    case EVENT_BLOODLETTING_SWEEP:
                    {
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_BLOODLETTING_SWEEP, false);

                        Talk(SAY_BLOODLETTING);
                        events.ScheduleEvent(EVENT_BLOODLETTING_SWEEP, 26000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->SetFacingTo(0.0f);

                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);

                        if (instance && !instance->instance->IsChallengeMode())
                        {
                            if (GameObject* l_Cache = GameObject::GetGameObject(*me, instance->GetData64(GO_HYMDALL_CACHE)))
                                l_Cache->SetRespawnTime(86400);
                        }
                    });

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 1185, me, nullptr, me->GetPosition()))
                            delete l_Conversation;
                    });
                }
            }

            void EndOfEncounter()
            {
                if (instance != nullptr)
                {
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    m_EncounterDone = true;

                    Talk(SAY_DEATH);

                    me->RemoveAllAuras();
                    me->RemoveAllAreasTrigger();

                    events.Reset();
                    summons.DespawnAll();

                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_HYMDALL, DONE);
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);
                    instance->DoCombatStopOnPlayers();

                    me->setFaction(35);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, { 3502.79f, 529.042f, 616.813f, 0.0f });
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_hymdallAI(p_Creature);
        }
};

/// Storm Drake - 97788
class npc_hymdall_storm_drake : public CreatureScript
{
    public:
        npc_hymdall_storm_drake() : CreatureScript("npc_hymdall_storm_drake") { }

        struct npc_hymdall_storm_drakeAI : public ScriptedAI
        {
            npc_hymdall_storm_drakeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            EventMap m_Events;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_Events.ScheduleEvent(EVENT_1, 1000);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_STORM_BREATH_VIS)
                {
                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_STATIC_FIELD, 150.0f);

                    if (!l_CreatureList.empty())
                    {
                        l_CreatureList.remove_if([this](Creature* p_Creature) -> bool
                        {
                            if (!me->isInFront(p_Creature, M_PI / 7.0f))
                                return true;

                            return false;
                        });
                    }

                    if (!l_CreatureList.empty())
                        l_CreatureList.sort(JadeCore::ObjectDistanceOrderPred(me));

                    uint32 l_Time = 10;
                    for (Creature* l_Field : l_CreatureList)
                    {
                        uint64 l_Guid = l_Field->GetGUID();
                        AddTimedDelayedOperation(l_Time, [this, l_Guid]() -> void
                        {
                            if (Creature* l_Field = Creature::GetCreature(*me, l_Guid))
                            {
                                if (!l_Field->HasAura(SPELL_STORM_BREATH_AT))
                                {
                                    l_Field->CastSpell(l_Field, SPELL_STORM_BREATH_AT, true);

                                    if (IsHeroicOrMythic())
                                    {
                                        if (urand(0, 1))
                                            l_Field->CastSpell(l_Field, SPELL_BALL_LIGHTNING_AT_FRONT, true);
                                        else
                                            l_Field->CastSpell(l_Field, SPELL_BALL_LIGHTNING_AT_BACK, true);
                                    }
                                }
                            }
                        });

                        l_Time += 200;
                    }

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        Position l_Pos = me->GetPosition();

                        l_Pos.m_positionX += 150.0f * std::cos(l_Pos.m_orientation);
                        l_Pos.m_positionY += 150.0f * std::sin(l_Pos.m_orientation);

                        SetFlyMode(true);
                        me->SetSpeed(MOVE_FLIGHT, 3.5f);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(2, l_Pos);
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 1:
                    {
                        DoCast(SPELL_STORM_BREATH_VIS);
                        break;
                    }
                    case 2:
                    {
                        m_Events.Reset();
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        Position l_Pos = me->GetPosition();

                        l_Pos.m_positionX += 75.0f * std::cos(l_Pos.m_orientation);
                        l_Pos.m_positionY += 75.0f * std::sin(l_Pos.m_orientation);
                        l_Pos.m_positionZ -= 5.0f;

                        SetFlyMode(true);
                        me->SetSpeed(MOVE_FLIGHT, 3.5f);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, l_Pos);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hymdall_storm_drakeAI(p_Creature);
        }
};

/// Mug of Mead - 102423
class npc_hymdall_mug_of_mead : public CreatureScript
{
    public:
        npc_hymdall_mug_of_mead() : CreatureScript("npc_hymdall_mug_of_mead") { }

        struct npc_hymdall_mug_of_meadAI : public ScriptedAI
        {
            npc_hymdall_mug_of_meadAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Clicked = false;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (m_Clicked)
                    return;

                m_Clicked = true;

                me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hymdall_mug_of_meadAI(p_Creature);
        }
};

/// Valarjar Thundercaller - 95842
class npc_hymdall_valarjar_thundercaller : public CreatureScript
{
    public:
        npc_hymdall_valarjar_thundercaller() : CreatureScript("npc_hymdall_valarjar_thundercaller") { }

        enum eSpells
        {
            SpellThunderousBolt     = 198595,
            SpellThunderstrikeAura  = 198599,
            SpellThunderstrikeCast  = 198605,
            SpellThunderstrikeAura2 = 215430,
            SpellThunderstrikeCast2 = 215429
        };

        struct npc_hymdall_valarjar_thundercallerAI : public ScriptedAI
        {
            npc_hymdall_valarjar_thundercallerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_1, 3000);
                events.ScheduleEvent(EVENT_2, 17000);
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
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellThunderousBolt);

                        events.ScheduleEvent(EVENT_1, 5000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                        {
                            if (IsMythic())
                                DoCast(l_Target, eSpells::SpellThunderstrikeAura2);
                            else
                                DoCast(l_Target, eSpells::SpellThunderstrikeAura);
                        }

                        events.ScheduleEvent(EVENT_2, 20000);
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
            return new npc_hymdall_valarjar_thundercallerAI(p_Creature);
        }
};

/// Thunderstrike - 198599
/// Thunderstrike - 215430
class spell_thunderstrike : public SpellScriptLoader
{
    public:
        spell_thunderstrike() : SpellScriptLoader("spell_thunderstrike") { }

        class spell_thunderstrike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_thunderstrike_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        if (l_Target->GetMap()->IsMythic())
                            l_Caster->CastSpell(l_Target, 215429, true);
                        else
                            l_Caster->CastSpell(l_Target, 198605, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_thunderstrike_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_thunderstrike_AuraScript();
        }
};

/// Mug of Mead - 202298
class spell_hymdall_mug_of_mead : public SpellScriptLoader
{
    public:
        spell_hymdall_mug_of_mead() : SpellScriptLoader("spell_hymdall_mug_of_mead") { }

        class spell_hymdall_mug_of_mead_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hymdall_mug_of_mead_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    std::set<uint32> l_AllowedEntries =
                    {
                        NPC_VALARJAR_CHAMPION,
                        NPC_VALARJAR_RUNECARVER,
                        NPC_VALARJAR_MYSTIC,
                        NPC_VALARJAR_THUNDERCALLER,
                        NPC_KING_HALDOR,
                        NPC_KING_BJORN,
                        NPC_KING_RANULF,
                        NPC_KING_TOR
                    };

                    if (l_AllowedEntries.find(p_Object->GetEntry()) == l_AllowedEntries.end())
                        return true;

                    if (p_Object->ToUnit()->isInCombat())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hymdall_mug_of_mead_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hymdall_mug_of_mead_SpellScript();
        }

        class spell_hymdall_mug_of_mead_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hymdall_mug_of_mead_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Creature* l_Target = GetTarget()->ToCreature())
                {
                    l_Target->SetReactState(REACT_PASSIVE);
                    l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    l_Target->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 35);

                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    {
                        std::set<uint32> l_Kings =
                        {
                            eCreatures::NPC_KING_HALDOR,
                            eCreatures::NPC_KING_BJORN,
                            eCreatures::NPC_KING_RANULF,
                            eCreatures::NPC_KING_TOR
                        };

                        if (l_Instance->instance->IsMythic() && l_Kings.find(l_Target->GetEntry()) != l_Kings.end())
                        {
                            for (uint32 const& l_Entry : l_Kings)
                            {
                                if (l_Target->GetEntry() == l_Entry)
                                    continue;

                                if (Creature* l_King = Creature::GetCreature(*l_Target, l_Instance->GetData64(l_Entry)))
                                {
                                    if (!l_King->HasAura(GetSpellInfo()->Id))
                                        return;
                                }
                            }

                            l_Instance->DoCompleteAchievement(eAchievements::I_GO_WHAT_YOU_MEAD);
                        }
                    }
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Creature* l_Target = GetTarget()->ToCreature())
                {
                    l_Target->SetReactState(REACT_AGGRESSIVE);
                    l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    l_Target->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_hymdall_mug_of_mead_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hymdall_mug_of_mead_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hymdall_mug_of_mead_AuraScript();
        }
};

/// Crackle - 199805
class spell_hymdall_crackle : public SpellScriptLoader
{
    public:
        spell_hymdall_crackle() : SpellScriptLoader("spell_hymdall_crackle") { }

        class spell_hymdall_crackle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hymdall_crackle_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        Position l_Pos      = *l_Caster;
                        float l_Angle       = l_Caster->GetAngle(l_Target);
                        float l_Step        = 1.0f;
                        float l_Distance    = l_Pos.GetExactDist(&*l_Target);

                        for (uint8 l_I = 0; l_I < uint32(l_Distance); ++l_I)
                        {
                            Position l_Pos =
                            {
                                l_Caster->m_positionX + (l_Step * l_I * std::cos(l_Angle)),
                                l_Caster->m_positionY + (l_Step * l_I * std::sin(l_Angle)),
                                l_Caster->m_positionZ,
                                l_Angle
                            };

                            l_Caster->CastSpell(l_Pos, SPELL_CRACKLE_MISSILE, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hymdall_crackle_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hymdall_crackle_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_hymdall()
{
    /// Boss
    new boss_hymdall();

    /// Creatures
    new npc_hymdall_storm_drake();
    new npc_hymdall_mug_of_mead();
    new npc_hymdall_valarjar_thundercaller();

    /// Spells
    new spell_thunderstrike();
    new spell_hymdall_mug_of_mead();
    new spell_hymdall_crackle();
}
#endif
