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
    SAY_FIRST,
    SAY_INTRO_1,
    SAY_INTRO_2,
    SAY_INTRO_3,
    SAY_AGGRO,
    SAY_RADIANT,
    SAY_SPEARS,
    SAY_RUNIC,
    SAY_RUNIC_1,
    SAY_DEATH,

    // Odyn's Blessing quest
    SAY_QUEST_OB_1,
    SAY_QUEST_OB_2,
    SAY_QUEST_OB_3,
    SAY_QUEST_OB_4,
    SAY_QUEST_OB_5,

    SAY_SUMMON,
    SAY_UNWORTHY,
    SAY_WIPE
};

enum Spells
{
    SPELL_HONOR_BOUND           = 198187,
    SPELL_UNWORTHY              = 198189,
    SPELL_UNWORTHY_DEBUFF       = 198190,
    SPELL_SPEAR_OF_LIGHT        = 198072,
    SPELL_SPEAR_OF_LIGHT_SEARCH = 198284,
    SPELL_SPEAR_OF_LIGHT_DAMAGE = 198060,
    SPELL_RADIANT_TEMPEST       = 201006,
    SPELL_SHATTER_SPEARS        = 198077,
    SPELL_GLOWING_FRAGMENT      = 198078,
    SPELL_RUNIC_BRAND           = 197961,

    /// Heroic
    SPELL_SUMMON_STORMFORGED    = 201209,
    SPELL_SURGE                 = 198750,
    SPELL_CONDUIT               = 199787,

    SPELL_RUNIC_BRAND_PURE      = 197963,
    SPELL_RUNIC_BRAND_RED       = 197964,
    SPELL_RUNIC_BRAND_YELLOW    = 197965,
    SPELL_RUNIC_BRAND_BLUE      = 197966,
    SPELL_RUNIC_BRAND_GREEN     = 197967,

    SPELL_RUNIC_BRAND_PURE_AT   = 197968,
    SPELL_RUNIC_BRAND_RED_AT    = 197971,
    SPELL_RUNIC_BRAND_YELLOW_AT = 197972,
    SPELL_RUNIC_BRAND_BLUE_AT   = 197975,
    SPELL_RUNIC_BRAND_GREEN_AT  = 197977,

    SPELL_ODYNS_BLESSING_CREDIT = 198747,
    SPELL_HEART_OF_ZIN_AZSHARI  = 221520
};

enum eEvents
{
    EVENT_SKOVALD_DONE_1        = 1,
    EVENT_SKOVALD_DONE_2        = 2,
    EVENT_SPEAR_OF_LIGHT        = 3,
    EVENT_RADIANT_TEMPEST       = 4,
    EVENT_SHATTER_SPEARS        = 5,
    EVENT_RUNIC_BRAND           = 6,
    EVENT_SUMMON_STORMFORGED    = 7
};

uint32 g_SpellsRunicColour[5] =
{
    SPELL_RUNIC_BRAND_PURE,
    SPELL_RUNIC_BRAND_RED,
    SPELL_RUNIC_BRAND_YELLOW,
    SPELL_RUNIC_BRAND_BLUE,
    SPELL_RUNIC_BRAND_GREEN
};

uint32 g_SpellsRunicAT[5] =
{
    SPELL_RUNIC_BRAND_PURE_AT,
    SPELL_RUNIC_BRAND_RED_AT,
    SPELL_RUNIC_BRAND_YELLOW_AT,
    SPELL_RUNIC_BRAND_BLUE_AT,
    SPELL_RUNIC_BRAND_GREEN_AT
};

Position const g_CenterPos = { 2429.13f, 528.55f, 748.99f, 0.0f };

std::array<Position const, 6> g_UnworthyPos =
{
    {
        { 2401.35f, 472.80f, 772.90f, 1.28f },
        { 2439.68f, 468.80f, 772.99f, 1.55f },
        { 2477.74f, 472.95f, 772.90f, 1.86f },
        { 2478.41f, 585.86f, 772.89f, 4.41f },
        { 2439.65f, 590.93f, 772.99f, 4.70f },
        { 2402.48f, 586.13f, 772.90f, 4.99f }
    }
};

/// Odyn - 95676
class boss_odyn : public CreatureScript
{
    public:
        boss_odyn() : CreatureScript("boss_odyn") { }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

            p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            if (p_Creature->IsAIEnabled)
                p_Creature->AI()->DoAction(0);

            return true;
        }

        struct boss_odynAI : public BossAI
        {
            boss_odynAI(Creature* p_Creature) : BossAI(p_Creature, DATA_ODYN)
            {
                m_Intro = true;
                m_EncounterCompleted = false;
                m_FirstSay = false;
                me->setFaction(FACTION_FRIENDLY);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            bool m_Intro;
            bool m_FirstSay;
            bool m_EncounterCompleted;
            uint8 m_RunicBrandCount;

            void Reset() override
            {
                me->RemoveAura(SPELL_HONOR_BOUND);

                if (instance && instance->GetBossState(DATA_ODYN) == DONE)
                    return;

                _Reset();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(200988);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNWORTHY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNWORTHY_DEBUFF);

                    for (uint8 i = 0; i < 5; i++)
                        instance->DoRemoveAurasDueToSpellOnPlayers(g_SpellsRunicColour[i]);

                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (instance->GetBossState(DATA_SKOVALD) == DONE && m_Intro)
                            DoAction(true);
                    });
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (instance && instance->GetData(DATA_SKOVALD_EVENT) != DONE)
                {
                    if (!m_FirstSay && me->IsWithinDistInMap(p_Who, 150.0f))
                    {
                        m_FirstSay = true;
                        Talk(SAY_FIRST);
                    }
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(SAY_AGGRO);

                _EnterCombat();

                events.ScheduleEvent(EVENT_SPEAR_OF_LIGHT, 8000);
                events.ScheduleEvent(EVENT_RADIANT_TEMPEST, 24000);
                events.ScheduleEvent(EVENT_SHATTER_SPEARS, 40000);
                events.ScheduleEvent(EVENT_RUNIC_BRAND, 44000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_SUMMON_STORMFORGED, 18000);

                DoCast(me, SPELL_HONOR_BOUND, true);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                Talk(SAY_WIPE);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (!m_Intro)
                {
                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->setFaction(FACTION_MONSTER_2);
                    });

                    return;
                }

                m_Intro = false;
                Talk(SAY_INTRO_1);
                events.ScheduleEvent(EVENT_SKOVALD_DONE_1, 5000);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == NPC_SPEAR_OF_LIGHT && IsHeroicOrMythic())
                {
                    Position l_Pos  = p_Summon->GetPosition();
                    float l_Angle   = frand(0.0f, 2.0f * M_PI);

                    for (uint8 l_I = 0; l_I < 3; l_I++)
                    {
                        l_Angle += (2.0f * M_PI) / float(3.0f);

                        p_Summon->GetNearPosition(l_Pos, 1.0f, l_Angle);

                        l_Pos.m_orientation = l_Angle;

                        me->CastSpell(l_Pos, SPELL_GLOWING_FRAGMENT, true);
                    }
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SPEAR_OF_LIGHT)
                    DoCast(me, SPELL_SPEAR_OF_LIGHT_SEARCH, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_SHATTER_SPEARS:
                    {
                        if (p_Target->GetEntry() == NPC_SPEAR_OF_LIGHT)
                        {
                            uint8 l_Count = IsHeroicOrMythic() ? 5 : 3;
                            Position l_Pos = p_Target->GetPosition();
                            float l_Angle = frand(0.0f, 2.0f * M_PI);

                            for (uint8 l_I = 0; l_I < l_Count; l_I++)
                            {
                                l_Angle += (2.0f * M_PI) / float(l_Count);

                                p_Target->GetNearPosition(l_Pos, 1.0f, l_Angle);

                                l_Pos.m_orientation = l_Angle;

                                me->CastSpell(l_Pos, SPELL_GLOWING_FRAGMENT, true);
                            }

                            p_Target->ToCreature()->DespawnOrUnsummon(1000);
                        }

                        break;
                    }
                    case SPELL_RUNIC_BRAND:
                    {
                        DoCast(p_Target, g_SpellsRunicColour[m_RunicBrandCount], true);
                        DoCast(me, g_SpellsRunicAT[m_RunicBrandCount], true);

                        Talk(SAY_RUNIC_1);

                        if (instance)
                            instance->SetData(DATA_RUNES_ACTIVATED, m_RunicBrandCount);

                        m_RunicBrandCount++;
                        break;
                    }
                    case SPELL_SPEAR_OF_LIGHT_SEARCH:
                    {
                        me->CastSpell(p_Target, SPELL_SPEAR_OF_LIGHT_DAMAGE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_RADIANT_TEMPEST)
                    DoCast(me, 198263);
            }

            void SetData(uint32 /*p_Type*/, uint32 p_Data) override
            {
                me->RemoveAreaTrigger(g_SpellsRunicAT[p_Data]);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if ((me->HealthBelowPctDamaged(80, p_Damage) || p_Damage > me->GetHealth()) && !m_EncounterCompleted)
                {
                    p_Damage = 0;
                    m_EncounterCompleted = true;

                    DoStopAttack();

                    Talk(SAY_DEATH);

                    _JustDied();

                    if (instance)
                    {
                        instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);

                        instance->DoRemoveAurasDueToSpellOnPlayers(200988);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNWORTHY);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNWORTHY_DEBUFF);

                        instance->DoCombatStopOnPlayers();

                        instance->instance->ForEachPlayer([&](Player* p_Player) -> void
                        {
                            p_Player->KilledMonsterCredit(me->GetEntry());
                        });
                    }

                    me->setFaction(FACTION_FRIENDLY);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->RemoveAllAuras();
                    me->CombatStop();
                    me->GetMotionMaster()->MoveTargetedHome();

                    Position l_Rotation = { 0.0f, 0.0f, -0.1545763f, 0.9879808f };
                    if (GameObject* l_Aegis = me->SummonGameObject(GO_AEGIS_OF_AGGRAMAR, { 2402.998f, 539.1649f, 752.4249f, 5.72788f }))
                        l_Aegis->SetRotationQuat(&l_Rotation);

                    ProcessOdynsBlessing();

                    AddTimedDelayedOperation(IN_MILLISECONDS * 3, [this]() -> void
                    {
                        instance->DoCombatStopOnPlayers();
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() && me->isInCombat())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_SKOVALD_DONE_1:
                    {
                        Talk(SAY_INTRO_2);
                        me->GetMotionMaster()->MoveJump(2399.30f, 528.85f, 749.0f, 25.0f, 15.0f, 0.0f, 1);
                        events.ScheduleEvent(EVENT_SKOVALD_DONE_2, 10000);
                        break;
                    }
                    case EVENT_SKOVALD_DONE_2:
                    {
                        me->SetHomePosition(me->GetPosition());
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                    }
                    case EVENT_SPEAR_OF_LIGHT:
                    {
                        DoCast(me, SPELL_SPEAR_OF_LIGHT, true);
                        events.ScheduleEvent(EVENT_SPEAR_OF_LIGHT, 10000);
                        break;
                    }
                    case EVENT_RADIANT_TEMPEST:
                    {
                        DoCast(SPELL_RADIANT_TEMPEST);
                        Talk(SAY_RADIANT);
                        events.ScheduleEvent(EVENT_RADIANT_TEMPEST, 48000);
                        break;
                    }
                    case EVENT_SHATTER_SPEARS:
                    {
                        DoCast(SPELL_SHATTER_SPEARS);
                        Talk(SAY_SPEARS);
                        events.ScheduleEvent(EVENT_SHATTER_SPEARS, 56000);
                        break;
                    }
                    case EVENT_RUNIC_BRAND:
                    {
                        m_RunicBrandCount = 0;
                        DoCast(SPELL_RUNIC_BRAND);
                        Talk(SAY_RUNIC);
                        events.ScheduleEvent(EVENT_RUNIC_BRAND, 56000);
                        break;
                    }
                    case EVENT_SUMMON_STORMFORGED:
                    {
                        Talk(SAY_SUMMON);
                        me->CastSpell(g_CenterPos, SPELL_SUMMON_STORMFORGED, true);
                        events.ScheduleEvent(EVENT_SUMMON_STORMFORGED, 52000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:

            void ProcessOdynsBlessing()
            {
                if (!IsHeroicOrMythic())
                    return;

                bool l_HasQuest = false;
                instance->instance->ForEachPlayer([&](Player* p_Player) -> void
                {
                    if (p_Player->GetQuestStatus(eQuests::QUEST_ODYNS_BLESSING) == QuestStatus::QUEST_STATUS_INCOMPLETE && me->IsWithinDistInMap(p_Player, 100.0f))
                    {
                        l_HasQuest = true;
                    }
                });

                if (!l_HasQuest)
                    return;

                AddTimedDelayedOperation(16 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                { Talk(Says::SAY_QUEST_OB_1); });

                AddTimedDelayedOperation(23 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                { Talk(Says::SAY_QUEST_OB_2); });

                AddTimedDelayedOperation(27 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Heart = me->SummonCreature(eCreatures::NPC_HEART_OF_ZIN_AZSHARI, g_OdynsSpoilPos.GetPositionX(), g_OdynsSpoilPos.GetPositionY(), g_OdynsSpoilPos.GetPositionZ() + 5.0f, g_OdynsSpoilPos.GetOrientation(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 50 * TimeConstants::IN_MILLISECONDS))
                    {
                        l_Heart->SetDisableGravity(true);
                        DoCast(l_Heart, Spells::SPELL_HEART_OF_ZIN_AZSHARI, true);
                    }
                });

                AddTimedDelayedOperation(30 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Talk(Says::SAY_QUEST_OB_3);
                    DoCastAOE(Spells::SPELL_HEART_OF_ZIN_AZSHARI, true);
                });

                AddTimedDelayedOperation(43 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                { Talk(Says::SAY_QUEST_OB_4); });

                AddTimedDelayedOperation(55 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                { DoCastAOE(Spells::SPELL_ODYNS_BLESSING_CREDIT, true); });

                AddTimedDelayedOperation(57 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                { Talk(Says::SAY_QUEST_OB_5); });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_odynAI(p_Creature);
        }
};

/// Stormforged Obliterator - 102019
class npc_odyn_stormforged_obliterator : public CreatureScript
{
    public:
        npc_odyn_stormforged_obliterator() : CreatureScript("npc_odyn_stormforged_obliterator") { }

        struct npc_odyn_stormforged_obliteratorAI : public ScriptedAI
        {
            npc_odyn_stormforged_obliteratorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetMovementAnimKitId(6973);

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoZoneInCombat(me, 100.0f);
                    me->SetReactState(REACT_AGGRESSIVE);
                    events.ScheduleEvent(EVENT_1, 1000);
                });
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SURGE)
                {
                    DoCast(me, SPELL_CONDUIT);

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Instance->SetData(DATA_SURGE_PROTECTOR, 0);
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

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_SURGE);
                        events.ScheduleEvent(EVENT_1, 4000);
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
            return new npc_odyn_stormforged_obliteratorAI(p_Creature);
        }
};

/// Chosen of Eyir - 101638
class npc_odyn_chosen_of_eyir : public CreatureScript
{
    public:
        npc_odyn_chosen_of_eyir() : CreatureScript("npc_odyn_chosen_of_eyir") { }

        struct npc_odyn_chosen_of_eyirAI : public ScriptedAI
        {
            npc_odyn_chosen_of_eyirAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_TargetGuid;

            Position m_MovePos;

            void Reset() override
            {
                m_TargetGuid = 0;

                m_MovePos = Position();

                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_TargetGuid = p_Guid;

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                        l_Target->EnterVehicle(me);
                });
            }

            void MovementInform(uint32 p_ID, uint32 p_Type) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == 10)
                {
                    if (Vehicle* l_Vehicle = me->GetVehicleKit())
                    {
                        l_Vehicle->RemoveAllPassengers();

                        me->DespawnOrUnsummon(500);

                        if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                            l_Target->RemoveAura(SPELL_UNWORTHY);
                    }
                }
            }

            void PassengerBoarded(Unit* p_Passenger, int8 p_SeatID, bool p_Apply) override
            {
                if (p_Apply)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_Pos = g_UnworthyPos[urand(0, g_UnworthyPos.size() - 1)];

                        l_Pos.m_positionZ += 10.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(10, l_Pos, false);

                        m_MovePos = l_Pos;
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->IsNearPosition(&m_MovePos, 0.1f))
                    MovementInform(10, POINT_MOTION_TYPE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_odyn_chosen_of_eyirAI(p_Creature);
        }
};

/// Radiant Tempest - 201006
class spell_odyn_radiant_tempest : public SpellScriptLoader
{
    public:
        spell_odyn_radiant_tempest() : SpellScriptLoader("spell_odyn_radiant_tempest") { }

        class spell_odyn_radiant_tempest_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_odyn_radiant_tempest_SpellScript);

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (GetHitUnit() != GetCaster())
                    GetHitUnit()->CastSpell(GetCaster(), GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_odyn_radiant_tempest_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_odyn_radiant_tempest_SpellScript();
        }
};

/// Runic Brand - 197963, 197964, 197965, 197966, 197967
class spell_odyn_runic_brand : public SpellScriptLoader
{
    public:
        spell_odyn_runic_brand() : SpellScriptLoader("spell_odyn_runic_brand") { }

        class spell_odyn_runic_brand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_odyn_runic_brand_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                InstanceScript* l_Instance = l_Target->GetInstanceScript();
                if (!l_Instance)
                    return;

                for (uint8 l_I = 0; l_I < 5; ++l_I)
                {
                    if (GetId() == g_SpellsRunicColour[l_I])
                        l_Instance->SetData(DATA_RUNES_DEACTIVATED, l_I);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_odyn_runic_brand_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_odyn_runic_brand_AuraScript();
        }
};

/// Honor Bound - 198188
class spell_odyn_honor_bound : public SpellScriptLoader
{
    public:
        spell_odyn_honor_bound() : SpellScriptLoader("spell_odyn_honor_bound") { }

        class spell_odyn_honor_bound_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_odyn_honor_bound_AuraScript);

            bool m_Unworthy;

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                m_Unworthy = false;

                Unit* l_Target = GetUnitOwner();
                if (l_Target == nullptr || !l_Target->IsPlayer())
                    return;

                /// Infinite amount
                p_Amount = -1;
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (l_Target->HasAura(SPELL_UNWORTHY_DEBUFF) || m_Unworthy)
                {
                    p_AbsorbAmount = p_DmgInfo.GetAmount();
                    return;
                }

                Player* l_Player = l_Target->ToPlayer();
                if (p_DmgInfo.GetAmount() >= l_Target->GetHealth() && (!l_Player || !l_Player->GetCommandStatus(CHEAT_GOD)))
                {
                    m_Unworthy = true;

                    p_AbsorbAmount = p_DmgInfo.GetAmount();

                    l_Target->RemoveAllAurasOnDeath();
                    l_Target->CombatStop(true);

                    l_Target->CastSpell(l_Target, SPELL_UNWORTHY_DEBUFF, true);
                    l_Target->CastSpell(l_Target, SPELL_UNWORTHY, true);

                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    {
                        if (Creature* l_Odyn = Creature::GetCreature(*l_Target, l_Instance->GetData64(DATA_ODYN)))
                        {
                            if (l_Odyn->IsAIEnabled)
                                l_Odyn->AI()->Talk(SAY_UNWORTHY);

                            if (Creature* l_Valkyr = l_Odyn->SummonCreature(NPC_CHOSEN_OF_EYIR, *l_Target))
                            {
                                if (l_Valkyr->IsAIEnabled)
                                    l_Valkyr->AI()->SetGUID(l_Target->GetGUID());
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_odyn_honor_bound_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_odyn_honor_bound_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_odyn_honor_bound_AuraScript();
        }
};

/// Unworthy (debuff) - 198190
class spell_odyn_unworthy_debuff : public SpellScriptLoader
{
    public:
        spell_odyn_unworthy_debuff() : SpellScriptLoader("spell_odyn_unworthy_debuff") { }

        class spell_odyn_unworthy_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_odyn_unworthy_debuff_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_odyn_unworthy_debuff_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_PACIFY_SILENCE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_odyn_unworthy_debuff_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_PACIFY_SILENCE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_odyn_unworthy_debuff_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_odyn()
{
    /// Odyn
    new boss_odyn();

    /// Creature
    new npc_odyn_stormforged_obliterator();
    new npc_odyn_chosen_of_eyir();

    /// Spells
    new spell_odyn_radiant_tempest();
    new spell_odyn_runic_brand();
    new spell_odyn_honor_bound();
    new spell_odyn_unworthy_debuff();
}
#endif
