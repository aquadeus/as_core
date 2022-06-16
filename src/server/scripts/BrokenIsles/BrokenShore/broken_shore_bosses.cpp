////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "BrokenIslesPCH.h"
#include "ScriptMgr.h"

enum eConvSpells
{
    SpellIntroConversation = 218047,
    SpellOutroConversation = 218041
};

/// Apocron - 121124
class boss_apocron : public CreatureScript
{
    public:
        boss_apocron() : CreatureScript("boss_apocron") { }

        enum eSpells
        {
            /// Misc
            SpellBonusRollPrompt            = 242969,

            /// Felfire Missiles
            SpellFelfireMissilesSearcher    = 241497,
            SpellFelfireMissilesSummon      = 241498,
            SpellFelfireMissilesAura        = 241503,
            SpellFelfireMissilesDummy       = 241506,
            SpellFelfireMissilesMissile     = 241507,
            /// Sear
            SpellSearCast                   = 241518,
            /// Quake
            SpellQuakeCast                  = 241458
        };

        enum eEvents
        {
            EventFelfireMissiles = 1,
            EventSear,
            EventQuake
        };

        struct boss_apocronAI : public BossAI
        {
            boss_apocronAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(eSpells::SpellBonusRollPrompt);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventFelfireMissiles, 7 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSear, 14 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventQuake, 16 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFelfireMissilesSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellFelfireMissilesSummon);
                        break;
                    }
                    case eSpells::SpellFelfireMissilesAura:
                    {
                        uint32 l_Time = 0;

                        for (uint8 l_I = 0; l_I < 5; ++l_I)
                        {
                            if (!l_Time)
                                DoCast(me, eSpells::SpellFelfireMissilesDummy, true);
                            else
                            {
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    DoCast(me, eSpells::SpellFelfireMissilesDummy, true);
                                });
                            }

                            l_Time += 700;
                        }

                        break;
                    }
                    case eSpells::SpellFelfireMissilesDummy:
                    {
                        for (uint8 l_I = 0; l_I < 7; ++l_I)
                            DoCast(me, eSpells::SpellFelfireMissilesMissile, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFelfireMissilesSummon)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellFelfireMissilesAura);
                    });
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                p_Who->CastSpell(p_Who, eConvSpells::SpellIntroConversation, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eConvSpells::SpellOutroConversation, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFelfireMissiles:
                    {
                        DoCast(me, eSpells::SpellFelfireMissilesSearcher, true);

                        events.ScheduleEvent(eEvents::EventFelfireMissiles, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSear:
                    {
                        DoCast(me, eSpells::SpellSearCast);

                        events.ScheduleEvent(eEvents::EventSear, 22 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventQuake:
                    {
                        DoCast(me, eSpells::SpellQuakeCast);

                        events.ScheduleEvent(eEvents::EventQuake, 23 * TimeConstants::IN_MILLISECONDS + 300);
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
            return new boss_apocronAI(p_Creature);
        }
};

/// Malificus - 117303
class boss_malificus : public CreatureScript
{
    public:
        boss_malificus() : CreatureScript("boss_malificus") { }

        enum eSpells
        {
            /// Misc
            SpellBonusRollPrompt    = 242969,

            /// Shadow Barrage
            SpellShadowBarrage      = 234452,
            /// Pestilence
            SpellPestilenceSearcher = 233614,
            SpellPestilenceAT       = 233631,
            /// Incite Panic
            SpellIncitePanicSearch  = 233569,
            SpellIncitePanicCast    = 233570
        };

        enum eEvents
        {
            EventShadowBarrage = 1,
            EventPestilence,
            EventIncitePanic
        };

        struct boss_malificusAI : public BossAI
        {
            boss_malificusAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(eSpells::SpellBonusRollPrompt);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventShadowBarrage, 8 * TimeConstants::IN_MILLISECONDS + 300);
                events.ScheduleEvent(eEvents::EventPestilence, 10 * TimeConstants::IN_MILLISECONDS + 700);
                events.ScheduleEvent(eEvents::EventIncitePanic, 13 * TimeConstants::IN_MILLISECONDS + 200);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellPestilenceSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellPestilenceAT, true);
                        break;
                    }
                    case eSpells::SpellIncitePanicSearch:
                    {
                        DoCast(p_Target, eSpells::SpellIncitePanicCast, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                p_Who->CastSpell(p_Who, eConvSpells::SpellIntroConversation, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eConvSpells::SpellOutroConversation, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShadowBarrage:
                    {
                        DoCast(me, eSpells::SpellShadowBarrage);

                        events.ScheduleEvent(eEvents::EventShadowBarrage, 17 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventPestilence:
                    {
                        DoCast(me, eSpells::SpellPestilenceSearcher);

                        events.ScheduleEvent(eEvents::EventPestilence, 18 * TimeConstants::IN_MILLISECONDS + 300);
                        break;
                    }
                    case eEvents::EventIncitePanic:
                    {
                        DoCast(me, eSpells::SpellIncitePanicSearch, true);

                        events.ScheduleEvent(eEvents::EventIncitePanic, 14 * TimeConstants::IN_MILLISECONDS + 500);
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
            return new boss_malificusAI(p_Creature);
        }
};

/// Si'vash - 117470
class boss_sivash : public CreatureScript
{
    public:
        boss_sivash() : CreatureScript("boss_sivash") { }

        enum eSpells
        {
            /// Misc
            SpellBonusRollPrompt    = 242969,

            /// Tidal Wave
            SpellTidalWaveSearcher  = 233976,
            SpellTidalWaveTriggered = 233977,
            /// Submerge
            SpellSubmergeSearcher   = 241433,
            SpellSubmergeMissile    = 241434,
            /// Call Honor Guards
            SpellHonorGuardsEvent   = 233968    ///< For AddOns (Eg. Big Wigs=
        };

        enum eEvents
        {
            EventTidalWave = 1,
            EventSubmerge,
            EventHonorGuard
        };

        enum eCreatures
        {
            SivashiHonorGuard   = 117487,
            WaveStalker         = 117564
        };

        struct boss_sivashAI : public BossAI
        {
            boss_sivashAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            Position m_CenterPos = { -838.80f, 1499.374f, 5.6063f, 0.0f };

            std::array<Position const, 4> m_WaveStalkerPos =
            {
                {
                    { -832.0504f, 1323.639f, 0.1848762f, 1.470690f },
                    { -888.0799f, 1339.165f, 0.1848762f, 1.290048f },
                    { -932.3611f, 1363.158f, 0.1848762f, 0.801530f },
                    { -982.6406f, 1394.186f, 0.1848762f, 0.750479f }
                }
            };

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(eSpells::SpellBonusRollPrompt);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventTidalWave, 4 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSubmerge, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventHonorGuard, 10 * TimeConstants::IN_MILLISECONDS);

                for (Position const& l_Pos : m_WaveStalkerPos)
                    me->SummonCreature(eCreatures::WaveStalker, l_Pos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, 0, nullptr, nullptr, true);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSubmergeSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellSubmergeMissile, true);
                        break;
                    }
                    case eSpells::SpellTidalWaveSearcher:
                    {
                        p_Target->CastSpell(p_Target, eSpells::SpellTidalWaveTriggered, true, nullptr, nullptr, me->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                p_Who->CastSpell(p_Who, eConvSpells::SpellIntroConversation, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eConvSpells::SpellOutroConversation, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventTidalWave:
                    {
                        DoCast(me, eSpells::SpellTidalWaveSearcher, true);

                        events.ScheduleEvent(eEvents::EventTidalWave, 22 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSubmerge:
                    {
                        DoCast(me, eSpells::SpellSubmergeSearcher, true);

                        events.ScheduleEvent(eEvents::EventSubmerge, 13 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHonorGuard:
                    {
                        DoCast(me, eSpells::SpellHonorGuardsEvent, true);

                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            Position l_Pos;

                            m_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 40.0f, frand(0.0f, 2.0f * M_PI), true);

                            if (Creature* l_Guard = me->SummonCreature(eCreatures::SivashiHonorGuard, l_Pos))
                            {
                                if (l_Guard->IsAIEnabled && me->getVictim())
                                    l_Guard->AI()->AttackStart(me->getVictim());
                            }
                        }

                        events.ScheduleEvent(eEvents::EventHonorGuard, 24 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_sivashAI(p_Creature);
        }
};

/// Brutallus - 117239
class boss_legion_brutallus : public CreatureScript
{
    public:
        boss_legion_brutallus() : CreatureScript("boss_legion_brutallus") { }

        enum eSpells
        {
            /// Misc
            SpellBonusRollPrompt        = 242969,

            /// Meteor Slash
            SpellMeteorSlashCast        = 233484,
            /// Crashing Embers
            SpellCrashingEmbersSearcher = 233515,
            SpellCrashingEmbersMissile  = 233516,
            /// Rupture
            SpellRuptureCast            = 233566,
            SpellRuptureMissile         = 234389
        };

        enum eEvents
        {
            EventMeteorSlash = 1,
            EventCrashingEmbers,
            EventRupture
        };

        struct boss_legion_brutallusAI : public BossAI
        {
            boss_legion_brutallusAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(eSpells::SpellBonusRollPrompt);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventMeteorSlash, 9 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCrashingEmbers, 14 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventRupture, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellCrashingEmbersSearcher:
                    {
                        if (p_Targets.empty())
                            break;

                        p_Targets.remove_if([this](WorldObject* p_Obj) -> bool
                        {
                            if (!p_Obj->IsPlayer())
                                return true;

                            return false;
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
                    case eSpells::SpellCrashingEmbersSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellCrashingEmbersMissile, true);
                        break;
                    }
                    case eSpells::SpellRuptureCast:
                    {
                        DoCast(p_Target, eSpells::SpellRuptureMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                p_Who->CastSpell(p_Who, eConvSpells::SpellIntroConversation, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eConvSpells::SpellOutroConversation, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventMeteorSlash:
                    {
                        DoCast(me, eSpells::SpellMeteorSlashCast);

                        events.ScheduleEvent(eEvents::EventMeteorSlash, 16 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCrashingEmbers:
                    {
                        DoCast(me, eSpells::SpellCrashingEmbersSearcher, true);

                        events.ScheduleEvent(eEvents::EventCrashingEmbers, 17 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRupture:
                    {
                        DoCast(me, eSpells::SpellRuptureCast);

                        events.ScheduleEvent(eEvents::EventRupture, 19 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_legion_brutallusAI(p_Creature);
        }
};

/// Si'vashi Honor Guard - 117487
class npc_sivash_honor_guard : public CreatureScript
{
    public:
        npc_sivash_honor_guard() : CreatureScript("npc_sivash_honor_guard") { }

        enum eSpells
        {
            SpellFlurry     = 236646,
            SpellHookedNet  = 241535
        };

        enum eEvents
        {
            EventFlurry = 1,
            EventHookedNet
        };

        struct npc_sivash_honor_guardAI : public ScriptedAI
        {
            npc_sivash_honor_guardAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventFlurry, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventHookedNet, 12 * TimeConstants::IN_MILLISECONDS + 200);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFlurry:
                    {
                        DoCast(me, eSpells::SpellFlurry);

                        events.ScheduleEvent(eEvents::EventFlurry, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHookedNet:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Target, eSpells::SpellHookedNet);

                        events.ScheduleEvent(eEvents::EventHookedNet, 17 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_sivash_honor_guardAI(p_Creature);
        }
};

/// Wave Stalker - 117564
class npc_sivash_wave_stalker : public CreatureScript
{
    public:
        npc_sivash_wave_stalker() : CreatureScript("npc_sivash_wave_stalker") { }

        enum eSpell
        {
            SpellTidalWaveAT = 233970
        };

        struct npc_sivash_wave_stalkerAI : public Scripted_NoMovementAI
        {
            npc_sivash_wave_stalkerAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpell::SpellTidalWaveAT)
                {
                    p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_EXTRA_SCALE, 27);

                    uint64 l_Guid = p_AreaTrigger->GetGUID();
                    AddTimedDelayedOperation(22 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                    {
                        if (AreaTrigger* l_AT = sObjectAccessor->GetAreaTrigger(*me, l_Guid))
                            l_AT->SetDuration(0);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_sivash_wave_stalkerAI(p_Creature);
        }
};

/// Syaith - 118403
class npc_syaith_brokenshore : public CreatureScript
{
    public:
        npc_syaith_brokenshore() : CreatureScript("npc_syaith_brokenshore") { }

        enum eItem
        {
            GlyphOfTheShadowSuccubus = 147120
        };

        enum eSpell
        {
            SpellGlyphOfTheShadowSuccubus = 240270
        };

        enum eText
        {
            GlyphOfTheShadowSuccubusText = 0
        };

        struct npc_syaith_brokenshoreAI : public ScriptedAI
        {
            npc_syaith_brokenshoreAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void ReceiveEmote(Player* p_Player, uint32 p_Emote) override
            {
                if (p_Emote == TEXT_EMOTE_KISS)
                {
                    if (!p_Player->HasAura(eSpell::SpellGlyphOfTheShadowSuccubus) || !p_Player->HasItemCount(eItem::GlyphOfTheShadowSuccubus, 1))
                    {
                        p_Player->AddItem(eItem::GlyphOfTheShadowSuccubus, 1);
                        me->AI()->Talk(eText::GlyphOfTheShadowSuccubusText);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_syaith_brokenshoreAI(p_Creature);
        }
};

/// Quake - 241458
class spell_apocron_quake_damage : public SpellScriptLoader
{
    public:
        spell_apocron_quake_damage() : SpellScriptLoader("spell_apocron_quake_damage") { }

        class spell_apocron_quake_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_apocron_quake_damage_SpellScript);

            void DealDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster() || !GetHitUnit() || !GetExplTargetDest())
                    return;

                float l_Distance    = GetExplTargetDest()->GetExactDist2d(GetHitUnit());
                float l_Radius      = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].CalcRadius(GetCaster());

                if (!l_Radius)
                    return;

                float l_Pct = std::max(0.25f * l_Radius, l_Radius - l_Distance) / l_Radius * 100.0f;

                int32 l_Damage = CalculatePct(GetHitDamage(), l_Pct);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_apocron_quake_damage_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_apocron_quake_damage_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_broken_shore_bosses()
{
    new boss_apocron();
    new boss_malificus();
    new boss_sivash();
    new boss_legion_brutallus();

    new npc_sivash_honor_guard();
    new npc_sivash_wave_stalker();
    new npc_syaith_brokenshore();

    new spell_apocron_quake_damage();
}
#endif
