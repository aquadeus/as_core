////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_emerald_nightmare.hpp"

enum Says
{
    SAY_AGGRO               = 0,
    SAY_FOCUSED_GAZE_EMOTE  = 1,
    SAY_FOCUSED_GAZE        = 2,
    SAY_CACOPHONY           = 3,
    SAY_DEATH               = 4,
    SAY_BLOOD_FRENZY        = 5
};

enum Spells
{
    SPELL_EMPTY_ENERGY              = 205270,
    SPELL_FEROCIOUS_YELL            = 204990,
    SPELL_ENERGIZE                  = 205272, ///< 2.941176470588235
    SPELL_BERSERK                   = 26662,
    SPELL_OVERWHELM_AURA            = 197946,
    SPELL_OVERWHELM_DUMMY           = 197947,
    SPELL_OVERWHELM_DMG             = 197943,
    SPELL_REND_FLESH_AURA           = 197948,
    SPELL_REND_FLESH_DUMMY          = 197949,
    SPELL_REND_FLESH_CAST           = 197942,
    SPELL_REND_FLESH_DAMAGE         = 204859,
    SPELL_FOCUSED_GAZE              = 198006,
    SPELL_BARRELING_MOMENTUM        = 198002,
    SPELL_B_MOMENTUM_FILTER         = 198004,
    SPELL_B_MOMENTUM_PING           = 198007,
    SPELL_B_MOMENTUM_CHARGE         = 198009,
    SPELL_B_MOMENTUM_AT             = 198093,
    SPELL_B_MOMENTUM_AOE            = 198109,
    SPELL_B_MOMENTUM_DUMMY          = 198102,
    SPELL_B_MOMENTUM_SPEED          = 204881,
    SPELL_B_MOMENTUM_DAMAGE         = 198099,
    SPELL_MOMENTUM_DEBUFF           = 198108,
    SPELL_TRAMPLING_SLAM            = 199237,
    SPELL_BLOOD_FRENZY              = 198388,
    SPELL_ROARING_CACOPHONY_AURA    = 197986,
    SPELL_ROARING_CACOPHONY_DUMMY_1 = 197988,
    SPELL_ROARING_CACOPHONY_DUMMY_2 = 197989,
    SPELL_ROARING_CACOPHONY_DMG     = 197969,
    SPELL_ECHOING_DISCHORD          = 198392,

    /// Heroic+
    SPELL_ROARING_CACOPHONY_SUM     = 197982, ///< Sum npc 100576
    SPELL_NIGHTMARISH_CACOPHONY     = 197980,
    SPELL_NIGHTMARE_IMAGE_AT        = 197960, ///< Dmg 205611
    SPELL_NIGHTMARE_IMAGE_MORPH     = 204969,
    SPELL_NIGHTMARE_IMAGE_SEARCHER  = 224490,
    SPELL_NIGHTMARE_IMAGE_CHARGE    = 224491
};

enum eEvents
{
    EVENT_FOCUSED_GAZE              = 1,
    EVENT_REND_FLESH                = 2,
    EVENT_OVERWHELM                 = 3,
    EVENT_ROARING_CACOPHONY         = 4,
    EVENT_ROARING_CACOPHONY_2       = 5,

    EVENT_ACTIVATE_IMAGE, ///< Mythic
    ACTION_BARRELING_MOMENTUM
};

enum eOther
{
    DATA_PLAYER_HITCOUNT            = 1,
    DATA_MOMENTUM_TARGET            = 2,
    DATA_MIASMA_TRIGGERED           = 3
};

/// Ursoc <Cursed Bear God> - 100497
class boss_ursoc : public CreatureScript
{
    public:
        boss_ursoc() : CreatureScript("boss_ursoc") { }

        struct boss_ursocAI : public BossAI
        {
            boss_ursocAI(Creature* p_Creature) : BossAI(p_Creature, DATA_URSOC)
            {
                m_IntroDone = false;
            }

            bool m_IntroDone;
            bool m_Focused;
            bool m_LowHP;
            bool m_MustTriggerMiasma;
            uint8 m_PlayerHitCount;
            uint32 m_BerserkTimer;
            uint64 m_TargetGUID;

            Position m_ChargeSource;

            void Reset() override
            {
                _Reset();
                summons.DespawnAll();
                me->SetPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_EMPTY_ENERGY, true);
                m_Focused = false;
                m_LowHP = false;
                m_MustTriggerMiasma = false;

                m_ChargeSource = Position();

                me->RemoveAura(SPELL_BERSERK);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                DoCast(me, SPELL_FEROCIOUS_YELL, true);
                DoCast(me, SPELL_ENERGIZE, true);

                m_BerserkTimer = 5 * MINUTE * IN_MILLISECONDS;
                events.ScheduleEvent(EVENT_OVERWHELM, 10000);
                events.ScheduleEvent(EVENT_REND_FLESH, 13000);
                events.ScheduleEvent(EVENT_ROARING_CACOPHONY, 18000);
                Talk(SAY_AGGRO);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SetBossState(DATA_URSOC, EncounterState::FAIL);

                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MOMENTUM_DEBUFF);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REND_FLESH_DAMAGE);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FOCUSED_GAZE);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERWHELM_DMG);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                Talk(SAY_DEATH);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MOMENTUM_DEBUFF);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REND_FLESH_DAMAGE);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FOCUSED_GAZE);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERWHELM_DMG);
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 100.0f))
                {
                    m_IntroDone = true;
                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3605, p_Who, nullptr, *p_Who))
                        delete l_Conversation;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_OVERWHELM_DUMMY:
                    {
                        DoCastVictim(SPELL_OVERWHELM_DMG, true);
                        break;
                    }
                    case SPELL_REND_FLESH_DUMMY:
                    {
                        DoCast(SPELL_REND_FLESH_CAST);
                        break;
                    }
                    case SPELL_ROARING_CACOPHONY_DUMMY_2:
                    {
                        DoCast(SPELL_ROARING_CACOPHONY_DMG);

                        if (IsHeroicOrMythic())
                        {
                            EntryCheckPredicate l_Pred(NPC_NIGHTMARE_IMAGE);
                            summons.DoAction(ACTION_1, l_Pred);
                            DoCast(me, SPELL_ROARING_CACOPHONY_SUM, true);
                        }

                        Talk(SAY_CACOPHONY);
                        break;
                    }
                    case SPELL_B_MOMENTUM_PING:
                    {
                        m_ChargeSource = me->GetPosition();
                        m_PlayerHitCount = 0;

                        DoCast(me, SPELL_B_MOMENTUM_AT, true);
                        DoCast(p_Caster, SPELL_TRAMPLING_SLAM, true);

                        me->SetFacingTo(me->GetAngle(p_Caster));
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                        /// Hack
                        uint64 l_Guid = p_Caster->GetGUID();
                        AddTimedDelayedOperation(10, [=]() -> void
                        {
                            if (Unit* l_Caster = Unit::GetUnit(*me, l_Guid))
                            {
                                me->SetPower(POWER_ENERGY, 0);
                                me->GetMotionMaster()->MoveCharge(l_Caster->GetPositionX(), l_Caster->GetPositionY(), l_Caster->GetPositionZ(), 60.0f, SPELL_B_MOMENTUM_CHARGE);

                                m_Focused = false;
                            }
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_B_MOMENTUM_FILTER:
                    {
                        m_TargetGUID = p_Target->GetGUID();

                        DoCast(p_Target, SPELL_FOCUSED_GAZE, true);
                        DoCast(p_Target, SPELL_B_MOMENTUM_DUMMY, true);

                        sCreatureTextMgr->SendChat(me, SAY_FOCUSED_GAZE_EMOTE, p_Target->GetGUID(), CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL);
                        Talk(SAY_FOCUSED_GAZE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_REND_FLESH_CAST)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, SPELL_REND_FLESH_DAMAGE, true);
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == NPC_NIGHTMARE_IMAGE)
                {
                    Position l_Pos = p_Summon->GetPosition();
                    AddTimedDelayedOperation(1, [this, l_Pos]() -> void
                    {
                        m_MustTriggerMiasma = true;
                        me->SummonCreature(NPC_NIGHTMARE_IMAGE, l_Pos);
                        m_MustTriggerMiasma = false;
                    });

                    p_Summon->DespawnOrUnsummon(100);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == POINT_MOTION_TYPE && p_ID == SPELL_B_MOMENTUM_CHARGE)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    DoCast(me, SPELL_B_MOMENTUM_SPEED, true);

                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    std::list<Unit*> l_Targets;

                    for (HostileReference* l_Ref : l_ThreatList)
                    {
                        if (Unit* l_Unit = l_Ref->getTarget())
                        {
                            /// Focused Gaze target isn't affected by Momentum
                            if (l_Unit->GetGUID() == m_TargetGUID)
                                continue;

                            l_Targets.push_back(l_Unit);
                        }
                    }

                    for (Unit* l_Target : l_Targets)
                        DoCast(l_Target, SPELL_B_MOMENTUM_DAMAGE, true);

                    DoCast(me, SPELL_B_MOMENTUM_AOE, true);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(31) && !m_LowHP)
                {
                    m_LowHP = true;
                    DoCast(me, SPELL_BLOOD_FRENZY, true);
                    Talk(SAY_BLOOD_FRENZY);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == ACTION_BARRELING_MOMENTUM && !m_Focused)
                {
                    m_Focused = true;
                    events.ScheduleEvent(EVENT_FOCUSED_GAZE, 500);
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_PLAYER_HITCOUNT:
                        return m_PlayerHitCount;
                    case DATA_MIASMA_TRIGGERED:
                        return m_MustTriggerMiasma;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 p_Type, uint32 /*p_Data*/) override
            {
                switch (p_Type)
                {
                    case DATA_PLAYER_HITCOUNT:
                        m_PlayerHitCount++;
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (m_BerserkTimer)
                {
                    if (m_BerserkTimer <= p_Diff)
                    {
                        m_BerserkTimer = 0;
                        DoCast(me, SPELL_BERSERK, true);
                    }
                    else
                        m_BerserkTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_REND_FLESH:
                    {
                        DoCast(me, SPELL_REND_FLESH_AURA, true);
                        break;
                    }
                    case EVENT_FOCUSED_GAZE:
                    {
                        m_TargetGUID = 0;
                        DoCast(me, SPELL_B_MOMENTUM_FILTER, true);
                        break;
                    }
                    case EVENT_OVERWHELM:
                    {
                        DoCast(me, SPELL_OVERWHELM_AURA, true);
                        break;
                    }
                    case EVENT_ROARING_CACOPHONY:
                    {
                        DoCast(me, SPELL_ROARING_CACOPHONY_DUMMY_2, true);
                        events.ScheduleEvent(EVENT_ROARING_CACOPHONY_2, 20000);
                        if (IsMythic())
                            events.ScheduleEvent(EVENT_ACTIVATE_IMAGE, 40000);
                        break;
                    }
                    case EVENT_ROARING_CACOPHONY_2:
                    {
                        DoCast(me, SPELL_ROARING_CACOPHONY_AURA, true);
                        break;
                    }
                    case EVENT_ACTIVATE_IMAGE:
                    {
                        for (uint64 l_Guid : summons)
                        {
                            if (Creature* l_Summon = Creature::GetCreature(*me, l_Guid))
                            {
                                /// Not a Nightmare Image, or Image already activated
                                if (l_Summon->GetEntry() != NPC_NIGHTMARE_IMAGE || l_Summon->GetReactState() == REACT_AGGRESSIVE)
                                    continue;

                                if (l_Summon->IsAIEnabled)
                                {
                                    l_Summon->AI()->DoAction(ACTION_2);
                                    break;
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_ACTIVATE_IMAGE, 40000);
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
            return new boss_ursocAI (p_Creature);
        }
};

/// Nightmare Image - 100576
class npc_ursoc_nightmare_image : public CreatureScript
{
    public:
        npc_ursoc_nightmare_image() : CreatureScript("npc_ursoc_nightmare_image") { }

        struct npc_ursoc_nightmare_imageAI : public ScriptedAI
        {
            npc_ursoc_nightmare_imageAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            void Reset() override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        me->RemoveAurasDueToSpell(SPELL_NIGHTMARE_IMAGE_MORPH);
                        DoCast(SPELL_NIGHTMARISH_CACOPHONY);

                        /// Don't do that if activated
                        if (me->GetReactState() != REACT_AGGRESSIVE)
                            events.ScheduleEvent(EVENT_1, 4000);

                        break;
                    }
                    case ACTION_2:
                    {
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                    }
                    default:
                        break;
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Creature* l_Boss = p_Summoner->ToCreature();
                if (!l_Boss || !l_Boss->IsAIEnabled)
                    return;

                DoCast(me, SPELL_NIGHTMARE_IMAGE_MORPH, true);

                if (l_Boss->AI()->GetData(DATA_MIASMA_TRIGGERED))
                    DoCast(me, SPELL_NIGHTMARE_IMAGE_AT, true);
                else
                    events.ScheduleEvent(EVENT_3, 3000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_NIGHTMARE_IMAGE_SEARCHER)
                    DoCast(p_Target, SPELL_NIGHTMARE_IMAGE_CHARGE, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() && me->isInCombat())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_NIGHTMARE_IMAGE_MORPH, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        me->RemoveAreaTrigger(SPELL_NIGHTMARE_IMAGE_AT);
                        me->RemoveAurasDueToSpell(SPELL_NIGHTMARE_IMAGE_MORPH);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 100.0f);
                        events.ScheduleEvent(EVENT_4, 100);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(me, SPELL_NIGHTMARE_IMAGE_AT, true);
                        break;
                    }
                    case EVENT_4:
                    {
                        DoCast(me, SPELL_NIGHTMARE_IMAGE_SEARCHER, true);
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
            return new npc_ursoc_nightmare_imageAI(p_Creature);
        }
};

/// Energize [DNT] - 205272
class spell_ursoc_periodic_energize : public SpellScriptLoader
{
    public:
        spell_ursoc_periodic_energize() : SpellScriptLoader("spell_ursoc_periodic_energize") { }

        class spell_ursoc_periodic_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ursoc_periodic_energize_AuraScript);

            int32 m_PowerCount;

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                m_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (m_PowerCount < 100)
                    l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 3);
                else
                    l_Caster->AI()->DoAction(ACTION_BARRELING_MOMENTUM);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ursoc_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ursoc_periodic_energize_AuraScript();
        }
};

/// Trampling Slam - 199237
class spell_ursoc_trampling_slam : public SpellScriptLoader
{
    public:
        spell_ursoc_trampling_slam() : SpellScriptLoader("spell_ursoc_trampling_slam") { }

        class spell_ursoc_trampling_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ursoc_trampling_slam_SpellScript);

            void DealDamage()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                float l_Distance = l_Caster->GetDistance(l_Target);

                if (l_Distance >= 0 && l_Distance <= 50.0f)
                    SetHitDamage(GetHitDamage() * (1.0f - (l_Distance / 50.0f)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ursoc_trampling_slam_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ursoc_trampling_slam_SpellScript();
        }
};

/// Barreling Impact - 198109
class spell_ursoc_barreling_impact : public SpellScriptLoader
{
    public:
        spell_ursoc_barreling_impact() : SpellScriptLoader("spell_ursoc_barreling_impact") { }

        class spell_ursoc_barreling_impact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ursoc_barreling_impact_SpellScript);

            void DealDamage()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->IsAIEnabled)
                    {
                        uint8 l_PlayerCount = l_Caster->GetMap()->GetPlayerCount();
                        uint8 l_HitCount = l_Caster->GetAI()->GetData(DATA_PLAYER_HITCOUNT);
                        if (l_HitCount && l_PlayerCount)
                        {
                            uint32 l_Perc = float(float(l_HitCount) / float(l_PlayerCount) * 100.0f);
                            if (l_Perc >= 100)
                                l_Perc = 90;
                            int32 l_Damage = CalculatePct(GetHitDamage(), 100 - l_Perc);
                            SetHitDamage(l_Damage);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ursoc_barreling_impact_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ursoc_barreling_impact_SpellScript();
        }
};

/// Momentum - 198099
class spell_ursoc_momentum_filter : public SpellScriptLoader
{
    public:
        spell_ursoc_momentum_filter() : SpellScriptLoader("spell_ursoc_momentum_filter") { }

        class spell_ursoc_momentum_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ursoc_momentum_filter_SpellScript);

            void FilterTargets(WorldObject*& p_Target)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (boss_ursoc::boss_ursocAI* l_AI = CAST_AI(boss_ursoc::boss_ursocAI, l_Caster->GetAI()))
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Caster, l_AI->m_TargetGUID))
                        {
                            if (l_Caster->getVictim() && !p_Target->IsInBetween(l_Player, l_AI->m_ChargeSource.m_positionX, l_AI->m_ChargeSource.m_positionY, 5.0f))
                                p_Target = nullptr;
                        }
                    }
                }
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                {
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_MOMENTUM_DEBUFF, true);
                    GetCaster()->GetAI()->SetData(DATA_PLAYER_HITCOUNT, 1);
                }
            }

            void Register() override
            {
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_ursoc_momentum_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_ursoc_momentum_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
                OnHit += SpellHitFn(spell_ursoc_momentum_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ursoc_momentum_filter_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ursoc()
{
    /// Boss
    new boss_ursoc();

    /// Creature
    new npc_ursoc_nightmare_image();

    /// Spells
    new spell_ursoc_periodic_energize();
    new spell_ursoc_trampling_slam();
    new spell_ursoc_barreling_impact();
    new spell_ursoc_momentum_filter();
}
#endif
