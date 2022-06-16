////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"
#include "HelperDefines.h"

/// Last Update 7.3.5 - Build 26365
/// Ligthforged Warframe - 125476
class npc_antorus_lightforged_warframe : public CreatureScript
{
    public:
        npc_antorus_lightforged_warframe() : CreatureScript("npc_antorus_lightforged_warframe") { }

        struct npc_antorus_lightforged_warframeAI : public ScriptedAI
        {
            npc_antorus_lightforged_warframeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                AreatriggerCosmetic = 249099,
                LightLaser          = 249100,
                SummonBarrier       = 248515,
                CosmeticBombardment = 252787
            };

            void Reset() override
            {
                me->DelayedCastSpell(me, eSpells::AreatriggerCosmetic, true, 500);
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::LightLaser:
                    {
                        if (!me->GetInstanceScript())
                            return;

                        me->GetInstanceScript()->SetData(eData::DataFelBatDown, 0);
                        break;
                    }
                    default:
                        break;
                }

            }

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                switch (p_EventEntry.EventId)
                {
                    case eCosmeticEvents::AntorusFelhoundsOutro:
                    {
                        AddTimedDelayedOperation(p_Duration, [this]() -> void
                        {
                            me->RemoveAura(eSpells::AreatriggerCosmetic);
                            me->CastSpell(me, eSpells::SummonBarrier, true);
                            me->CastSpell(me, eSpells::CosmeticBombardment, true);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_warframeAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Searing Rend - 245857
class spell_antorus_searing_rend : public SpellScriptLoader
{
    public:
        spell_antorus_searing_rend() : SpellScriptLoader("spell_antorus_searing_rend") { }

        class spell_antorus_searing_rend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_searing_rend_SpellScript);

            Position l_Dest;

            enum eSpells
            {
                SearingRendMissile = 245858
            };

            void HandleOnPrepare()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                l_Dest = *l_Target;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::SearingRendMissile, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_antorus_searing_rend_SpellScript::HandleOnPrepare);
                OnEffectHit += SpellEffectFn(spell_antorus_searing_rend_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_searing_rend_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Shadow Storm - 245889
class spell_antorus_shadow_storm : public SpellScriptLoader
{
    public:
        spell_antorus_shadow_storm() : SpellScriptLoader("spell_antorus_shadow_storm") { }

        class spell_antorus_shadow_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_shadow_storm_SpellScript);

            enum eSpells
            {
                ShadowStormMissle = 245890
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ShadowStormMissle, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_shadow_storm_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_shadow_storm_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Shadow Storm - 245868
class spell_antorus_shadow_burst : public SpellScriptLoader
{
    public:
        spell_antorus_shadow_burst() : SpellScriptLoader("spell_antorus_shadow_burst") { }

        class spell_antorus_shadow_burst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_shadow_burst_SpellScript);

            enum eSpells
            {
                ShadowBurstMissile = 245869
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ShadowBurstMissile, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_shadow_burst_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_shadow_burst_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Decimation - 245768
class spell_antorus_decimation : public SpellScriptLoader
{
    public:
        spell_antorus_decimation() : SpellScriptLoader("spell_antorus_decimation") { }

        class spell_antorus_decimation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_decimation_SpellScript);

            enum eSpells
            {
                DecimationAura = 245770
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DecimationAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_decimation_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_decimation_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Decimation - 245770
class spell_antorus_decimation_aura : public SpellScriptLoader
{
    public:
        spell_antorus_decimation_aura() : SpellScriptLoader("spell_antorus_decimation_aura") { }

        class spell_antorus_decimation_aura_SpellScript : public AuraScript
        {
            PrepareAuraScript(spell_antorus_decimation_aura_SpellScript);

            enum eSpells
            {
                DecimationMissile       = 245771,
                DecimationCastVisual    = 245790
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    l_Caster->CastSpell(l_Caster, eSpells::DecimationCastVisual, false);

                l_Caster->CastSpell(l_Target, eSpells::DecimationMissile, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_antorus_decimation_aura_SpellScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antorus_decimation_aura_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Garothi Annihilator- 123398
class npc_antorus_garothi_annihilator : public CreatureScript
{
    public:
        npc_antorus_garothi_annihilator() : CreatureScript("npc_antorus_garothi_annihilator") { }

        struct npc_antorus_garothi_annihilatorAI : public LegionCombatAI
        {
            npc_antorus_garothi_annihilatorAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetAnimTier(3);
                me->SendPlayHoverAnim(true);
                me->SetDisableGravity(true);
                me->CastSpell(me, eSpells::FelReaverHover, true);
                me->GetMotionMaster()->MoveSmoothFlyPath(10, me->GetEntry() * 1000);
                m_Landed = false;
            }

            enum eSpells
            {
                FelReaverHover      = 257955,
                SummonAnnihilation  = 245808
            };

            enum eEvents
            {
                EventAnnihilation = 1
            };

            bool m_Landed;

            Position const m_Center = { -3299.15f, 9772.60f, -63.36f };

            bool CanFly() override
            {
                return !m_Landed;
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        Position const l_NewHomePos = { -3297.708f, 9794.954f, -63.41246f, 4.687736f };
                        me->RemoveAura(eSpells::FelReaverHover);
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        me->SetHomePosition(l_NewHomePos);
                        me->SetAnimTier(0);
                        me->SetDisableGravity(false);
                        me->SendPlayHoverAnim(false);
                        me->SetCanFly(false);
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_BYTES_1, 0);
                        m_Landed = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!me->GetInstanceScript())
                    return;

                me->GetInstanceScript()->SetData(eData::DataAnnihilatorDeath, 0);
            }

            bool AdditionalChecks(uint32 /*p_EventID*/) override
            {
                return m_Landed;
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventAnnihilation:
                    {
                        for (uint32 l_I = 0; l_I < 2; ++l_I)
                        {
                            Position l_Dest;
                            m_Center.SimplePosXYRelocationByAngle(l_Dest, frand(0.0f, 20.0f), frand(0.0f, 2 * M_PI));
                            me->CastSpell(l_Dest, eSpells::SummonAnnihilation, true);
                        }

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_garothi_annihilatorAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Lightforged Centurion   - 129617
/// Lightforged Centurion   - 125478
/// Lightforged Cleric      - 124778
class npc_antorus_lightforged_soldiers : public CreatureScript
{
    public:
        npc_antorus_lightforged_soldiers() : CreatureScript("npc_antorus_lightforged_soldiers") { }

        struct npc_antorus_lightforged_soldiersAI : public ScriptedAI
        {
            npc_antorus_lightforged_soldiersAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_EventId = 0;
            }

            enum eSpells
            {
                CosmeticHealChannel = 249402
            };

            uint64 m_EventId;

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        switch (m_EventId)
                        {
                            case MAKE_PAIR64(eCosmeticEvents::AntorusAnnihilatorDeath, 0):
                            case MAKE_PAIR64(eCosmeticEvents::AntorusAnnihilatorDeath, 2):
                            case MAKE_PAIR64(eCosmeticEvents::AntorusAnnihilatorDeath, 5):
                                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);
                                break;
                            default:
                                break;
                        }

                        AddTimedDelayedOperation(1500, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->SetWalk(true);
                            me->SetSpeed(MOVE_WALK, 1.4f, true);

                            switch (m_EventId)
                            {
                                case MAKE_PAIR64(eCosmeticEvents::AntorusAnnihilatorDeath, 0):
                                    me->GetMotionMaster()->MovePoint(5, G3D::Vector3({ -3306.106f, 9735.639f, -63.037f }));
                                    me->DespawnOrUnsummon(13 * IN_MILLISECONDS);
                                    break;
                                case MAKE_PAIR64(eCosmeticEvents::AntorusAnnihilatorDeath, 2):
                                    me->GetMotionMaster()->MovePoint(5, G3D::Vector3({ -3305.989f, 9735.593f, -63.037f }));
                                    me->DespawnOrUnsummon(11 * IN_MILLISECONDS);
                                    break;
                                case MAKE_PAIR64(eCosmeticEvents::AntorusAnnihilatorDeath, 5):
                                    me->GetMotionMaster()->MovePoint(5, G3D::Vector3({ -3306.007f, 9735.785f, -63.037f }));
                                    me->DespawnOrUnsummon(12 * IN_MILLISECONDS);
                                    break;
                                default:
                                    break;
                            }
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                m_EventId = MAKE_PAIR64(p_EventEntry.EventId, p_EventEntry.Index);

                switch (p_EventEntry.EventId)
                {
                    case eCosmeticEvents::AntorusAnnihilatorDeath:
                    {
                        AddTimedDelayedOperation(p_Duration, [this]()-> void
                        {
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 567);
                        });
                        break;
                    }
                    case eCosmeticEvents::AntorusFelhoundsOutro:
                    {
                        AddTimedDelayedOperation(p_Duration, [this]()-> void
                        {
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                        });
                    }
                    default:
                        break;
                }

                switch (m_EventId)
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 0):
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 1):
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 2):
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        break;
                    case MAKE_PAIR64(eCosmeticEvents::AntorusWorldbreakerDeath, 50):
                        me->CastSpell(me, eSpells::CosmeticHealChannel, true);
                        break;
                    case MAKE_PAIR64(eCosmeticEvents::AntorusWorldbreakerDeath, 51):
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);
                        break;
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                switch (m_EventId)
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 0):
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 1):
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 2):
                        me->GetMotionMaster()->Clear(false);
                        me->m_Events.KillAllEvents(true);
                            break;
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                switch (m_EventId)
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 0):
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 1):
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 2):
                        me->DespawnOrUnsummon(1);
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

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_soldiersAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Ligthforged Warframe (Garothi Worldbreaker intro) - 127256
class npc_antorus_lightforged_warframe_worldbreaker_intro : public CreatureScript
{
    public:
        npc_antorus_lightforged_warframe_worldbreaker_intro() : CreatureScript("npc_antorus_lightforged_warframe_worldbreaker_intro") { }

        struct npc_antorus_lightforged_warframe_worldbreaker_introAI : public ScriptedAI
        {
            npc_antorus_lightforged_warframe_worldbreaker_introAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Cosmetic = 253060
            };

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                if (p_EventEntry.EventId == eCosmeticEvents::AntorusAnnihilatorDeath)
                {
                    AddTimedDelayedOperation(p_Duration + urand(500, 1500), [this]()-> void
                    {
                        me->CastSpell(me, eSpells::Cosmetic, false);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_warframe_worldbreaker_introAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Battle Suit Cosmetic - 253062
class spell_antorus_battle_suit_cosmetic : public SpellScriptLoader
{
    public:
        spell_antorus_battle_suit_cosmetic() : SpellScriptLoader("spell_antorus_battle_suit_cosmetic") { }

        class spell_antorus_battle_suit_cosmetic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_battle_suit_cosmetic_SpellScript);

            enum eSpells
            {
                CosmeticMissile = 253066
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CosmeticMissile, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_battle_suit_cosmetic_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_battle_suit_cosmetic_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// High Exarch Turalyon - 125512
class npc_antorus_high_exarch_turalyon : public CreatureScript
{
    public:
        npc_antorus_high_exarch_turalyon() : CreatureScript("npc_antorus_high_exarch_turalyon") { }

        struct npc_antorus_high_exarch_turalyonAI : public ScriptedAI
        {
            npc_antorus_high_exarch_turalyonAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eConversations
            {
                Fallback = 5515
            };

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                if (p_EventEntry.EventId == eCosmeticEvents::AntorusAnnihilatorDeath)
                {
                    AddTimedDelayedOperation(p_Duration, [this]()-> void
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);

                        AddTimedDelayedOperation(12000, [this]() -> void
                        {
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                            me->SetSpeed(MOVE_RUN, 0.9f);
                            me->GetMotionMaster()->MovePoint(5, G3D::Vector3({ -3305.953f, 9735.559f, -62.91042f }));

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Fallback, me, nullptr, *me))
                                delete l_Conversation;

                            if (Creature* l_Garothi = me->FindNearestCreature(eCreatures::BossGarothiWorldbreaker, 100.0f))
                            {
                                if (l_Garothi->IsAIEnabled)
                                    l_Garothi->AI()->DoAction(ACTION_5);
                            }

                            AddTimedDelayedOperation(5000, [this]() -> void
                            {
                                me->DespawnOrUnsummon(1);
                            });
                        });
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_high_exarch_turalyonAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation - 123459
class npc_antorus_annihilation : public CreatureScript
{
    public:
        npc_antorus_annihilation() : CreatureScript("npc_antorus_annihilation") { }

        struct npc_antorus_annihilationAI : public ScriptedAI
        {
            npc_antorus_annihilationAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Started = false;
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            enum eSpells
            {
                Areatrigger  = 244795
            };

            bool m_Started;

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Started)
                {
                    m_Started = true;

                    me->DelayedCastSpell(me, eSpells::Areatrigger, true, 100);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_annihilationAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation - 10662 (244795)
class at_antorus_annihilation : public AreaTriggerEntityScript
{
    public:
        at_antorus_annihilation() : AreaTriggerEntityScript("at_antorus_annihilation") { }

        enum eSpells
        {
            VisualAura  = 244799,
            Missile     = 245809,

            BossMissile         = 244758,
            HaywireAnnihilator  = 246965,
            HaywireBossMissile  = 246969
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster || !p_Target->IsPlayer())
                return false;

            l_Caster->RemoveAura(eSpells::VisualAura);
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            GuidList* l_AffectedPlayers = p_Areatrigger->GetAffectedPlayers();
            if (!l_Caster || !l_AffectedPlayers)
                return false;

            if (l_Caster->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                return false;

            bool l_PlayerFound = false;
            for (uint64 l_GUID : *l_AffectedPlayers)
            {
                if (GUID_HIPART(l_GUID) != HighGuid::HIGHGUID_PLAYER)
                    continue;

                l_PlayerFound = true;
                break;
            }

            if (!l_PlayerFound)
                l_Caster->CastSpell(l_Caster, eSpells::VisualAura, true);

            return false;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Creature* l_Annihilator = nullptr;
            Creature* l_GarothiWorldbreaker = nullptr;
            if (Creature* l_GarothiAnnihilator = l_Caster->FindNearestCreature(eCreatures::NpcGarothiAnnihilator, 250.0f))
                l_GarothiAnnihilator->CastSpell(l_Caster, eSpells::Missile, true);
            else if ((l_Annihilator = l_Caster->FindNearestCreature(eCreatures::NpcAnnihilator, 250.0f))
                && (l_GarothiWorldbreaker = l_Caster->FindNearestCreature(eCreatures::BossGarothiWorldbreaker, 250.0f)))
            {
                if (!l_Annihilator->HasAura(eSpells::HaywireAnnihilator))
                    l_Annihilator->CastSpell(l_Caster, eSpells::BossMissile, true);
                else
                {
                    if (l_Caster->GetEntry() == eCreatures::NpcAnnihilation)
                        l_Annihilator->CastSpell(l_Caster, eSpells::HaywireBossMissile, true);
                }
            }

            l_Caster->RemoveAura(eSpells::VisualAura);

            if (l_Caster->ToCreature())
            {
                l_Caster->SetVisible(false);
                l_Caster->ToCreature()->DespawnOrUnsummon(1);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_antorus_annihilation();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Annihilation - 245810
class spell_antorus_annhiliation : public SpellScriptLoader
{
    public:
        spell_antorus_annhiliation() : SpellScriptLoader("spell_antorus_annhiliation") { }

        class spell_antorus_annhiliation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_annhiliation_SpellScript);

            enum eSpells
            {
                Annihilation = 245811
            };

            bool m_RaidWide;

            void CheckTargetCount(std::list<WorldObject*>& p_Targets)
            {
                m_RaidWide = p_Targets.empty();
            }

            void HandleAfterCast()
            {
                if (!m_RaidWide)
                    return;

                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::Annihilation, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_antorus_annhiliation_SpellScript::CheckTargetCount, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_antorus_annhiliation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_annhiliation_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Ligthforged Warframe (Garothi Worldbreaker outro) - 124785
class npc_antorus_lightforged_warframe_worldbreaker_outro : public CreatureScript
{
    public:
        npc_antorus_lightforged_warframe_worldbreaker_outro() : CreatureScript("npc_antorus_lightforged_warframe_worldbreaker_outro") { }

        struct npc_antorus_lightforged_warframe_worldbreaker_outroAI : public ScriptedAI
        {
            npc_antorus_lightforged_warframe_worldbreaker_outroAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                AppearingFromPod = 247963
            };

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                if (p_EventEntry.EventId == eCosmeticEvents::AntorusWorldbreakerDeath)
                {
                    me->CastSpell(me, eSpells::AppearingFromPod, true);

                    AddTimedDelayedOperation(p_Duration, [this]()-> void
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);

                        Position l_Dest;
                        me->SimplePosXYRelocationByAngle(l_Dest, 5.0f, 0.0f);
                        me->SummonCreature(eCreatures::NpcLightforgedBarricade, l_Dest);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_warframe_worldbreaker_outroAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Lightforged Centurion - 124777
/// Lightforged Comamnder - 124782
class npc_antorus_lightforged_worlbreaker_pod : public CreatureScript
{
    public:
        npc_antorus_lightforged_worlbreaker_pod() : CreatureScript("npc_antorus_lightforged_worlbreaker_pod") { }

        struct npc_antorus_lightforged_worlbreaker_podAI : public ScriptedAI
        {
            npc_antorus_lightforged_worlbreaker_podAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                AppearingFromPod = 247963
            };

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                if (p_EventEntry.EventId == eCosmeticEvents::AntorusWorldbreakerDeath)
                    me->CastSpell(me, eSpells::AppearingFromPod, true);

                AddTimedDelayedOperation(p_Duration, [this]() -> void
                {
                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_worlbreaker_podAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Legion Talon - 125771
class npc_antorus_legion_talon : public CreatureScript
{
    public:
        npc_antorus_legion_talon() : CreatureScript("npc_antorus_legion_talon") { }

        struct npc_antorus_legion_talonAI : public ScriptedAI
        {
            npc_antorus_legion_talonAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                TalonLaser  = 252756,
                TalonStrike = 249933
            };

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                switch (MAKE_PAIR64(p_EventEntry.EventId, p_EventEntry.Index))
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusWorldbreakerDeath, 11):
                    {
                        ///me->CastSpell(me, eSpells::TalonLaser, true);
                        break;
                    }
                    case MAKE_PAIR64(eCosmeticEvents::AntorusSpawnLegionShip, 0):
                    {
                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            uint32 l_Missiles = urand(10, 15);
                            for (uint32 l_I = 0; l_I < l_Missiles; ++l_I)
                            {
                                Position l_Dest;
                                me->SimplePosXYRelocationByAngle(l_Dest, frand(-12.0f, 12.0f), M_PI / 2.0f);
                                l_Dest.SimplePosXYRelocationByAngle(l_Dest, frand(15.0f, 90.0f), me->GetOrientation(), true);
                                l_Dest.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ);
                                me->CastSpell(l_Dest, eSpells::TalonStrike, true);
                            }
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_legion_talonAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Legion Talon - 125480
class npc_antorus_lightforged_commander : public CreatureScript
{
    public:
        npc_antorus_lightforged_commander() : CreatureScript("npc_antorus_lightforged_commander") { }

        struct npc_antorus_lightforged_commanderAI : public ScriptedAI
        {
            npc_antorus_lightforged_commanderAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_EventId = 0;
            }

            enum eTalks
            {
                IncommingLegionShip
            };

            uint64 m_EventId;

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                m_EventId = MAKE_PAIR64(p_EventEntry.EventId, p_EventEntry.Index);

                switch (m_EventId)
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusWorldbreakerDeath, 21):
                    {
                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::IncommingLegionShip);
                            if (me->GetInstanceScript())
                                me->GetInstanceScript()->SetData(eData::DataActivateLegionShips, 0);
                        });
                        break;
                    }
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 0):
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        break;
                    default:
                        break;
                }

                switch (p_EventEntry.EventId)
                {
                    case eCosmeticEvents::AntorusFelhoundsOutro:
                    {
                        AddTimedDelayedOperation(p_Duration, [this]() -> void
                        {
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                switch (m_EventId)
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 0):
                        me->GetMotionMaster()->Clear(false);
                        me->m_Events.KillAllEvents(true);
                            break;
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                switch (m_EventId)
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusLightforgedPatrol, 0):
                        me->DespawnOrUnsummon(1);
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

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_commanderAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Lightforged Centurion (Dead) - 128589
class npc_antorus_lightforged_centurion_dead : public CreatureScript
{
    public:
        npc_antorus_lightforged_centurion_dead() : CreatureScript("npc_antorus_lightforged_centurion_dead") { }

        struct npc_antorus_lightforged_centurion_deadAI : public ScriptedAI
        {
            npc_antorus_lightforged_centurion_deadAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                LightforgedClericConversation = 249424
            };

            enum eConversations
            {
                IllidanTalkFelhoundsIntro = 5522
            };

            void CheckConversation()
            {
                if (me->FindNearestPlayer(30.0f))
                {
                    me->CastSpell(me, eSpells::LightforgedClericConversation, true);
                    if (Creature* l_LightforgedCleric = me->FindNearestCreature(eCreatures::NpcLightforgedCleric, 35.0f))
                        l_LightforgedCleric->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
                }
                else
                {
                    AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                    {
                        CheckConversation();
                    });
                }
            }

            void CheckIllidanTalk()
            {
                if (me->FindNearestPlayer(30.0f))
                {
                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::IllidanTalkFelhoundsIntro, me, nullptr, *me))
                        delete l_Conversation;
                }
                else
                {
                    AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                    {
                        CheckIllidanTalk();
                    });
                }
            }

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                switch (MAKE_PAIR64(p_EventEntry.EventId, p_EventEntry.Index))
                {
                    case MAKE_PAIR64(eCosmeticEvents::AntorusWorldbreakerDeath, 52):
                        CheckConversation();
                        break;
                    case MAKE_PAIR64(eCosmeticEvents::AntorusWorldbreakerDeath, 53):
                        CheckIllidanTalk();
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_centurion_deadAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Hanging fiend Trigger - 11662 (253642)
class at_antorus_hanging_fiend_trigger : public AreaTriggerEntityScript
{
    public:
        at_antorus_hanging_fiend_trigger() : AreaTriggerEntityScript("at_antorus_hanging_fiend_trigger") { }

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return false;

            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster || !l_Caster->IsAIEnabled)
                return false;

            l_Caster->ToCreature()->AI()->DoAction(ACTION_1);
            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_antorus_hanging_fiend_trigger();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Slobbering Fiend - 127810
class npc_antorus_slobbering_fiend : public CreatureScript
{
    public:
        npc_antorus_slobbering_fiend() : CreatureScript("npc_antorus_slobbering_fiend") { }

        struct npc_antorus_slobbering_fiendAI : public LegionCombatAI
        {
            npc_antorus_slobbering_fiendAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->CastSpell(me, eSpells::HangingFiendTrigger, true);
                me->SetAIAnimKitId(eMiscs::AnimHanging);
            }

            enum eMiscs
            {
                AnimHanging = 14018
            };

            enum eSpells
            {
                HangingFiendTrigger     = 253642,
                HangingFiendCosmetic    = 253741
            };

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            Player* l_Player = me->FindNearestPlayer(100.0f);
                            if (!l_Player)
                                return;

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            me->SetAIAnimKitId(0);
                            me->SetAnimTier(0);
                            me->SendPlayHoverAnim(false);
                            me->SetCanFly(false, true);
                            me->SetDisableGravity(false);
                            me->RemoveAura(eSpells::HangingFiendTrigger);
                            me->RemoveAura(eSpells::HangingFiendCosmetic);
                            AttackStart(l_Player);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_slobbering_fiendAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Legion Devastator - 126575
class npc_antorus_legion_devastator : public CreatureScript
{
    public:
        npc_antorus_legion_devastator() : CreatureScript("npc_antorus_legion_devastator") { }

        struct npc_antorus_legion_devastatorAI : public Scripted_NoMovementAI
        {
            npc_antorus_legion_devastatorAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            uint64 m_TargetGUID;

            enum eEvent
            {
                EventBombardment = 1
            };

            enum eSpell
            {
                SpellInfernalBombardment    = 253542
            };

            void Reset() override
            {
                events.ScheduleEvent(eEvent::EventBombardment, urand(1 * TimeConstants::IN_MILLISECONDS, 5 * TimeConstants::IN_MILLISECONDS));
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventBombardment:
                    {
                        if (!m_Instance)
                            break;

                        /// 2 Devastators for Portal Keeper Hasabel trashes platform
                        if (me->m_orientation > 2.0f && me->m_orientation < 2.10f)
                        {
                            if (Creature* l_Stalker = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcDevastatorStalker)))
                                DoCast(l_Stalker, eSpell::SpellInfernalBombardment, true);
                        }
                        /// 2 Devastators for light Barricades
                        else
                        {
                            if (Creature* l_Barricade = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcLightforgedBarricade2)))
                            {
                                m_TargetGUID = l_Barricade->GetGUID();
                                DoCast(l_Barricade, eSpell::SpellInfernalBombardment, true);
                            }
                        }

                        events.ScheduleEvent(eEvent::EventBombardment, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* /*p_SpellInfo*/, SpellEffectHandleMode /*p_Mode*/) override
            {
                if (me->m_orientation > 2.0f && me->m_orientation < 2.10f)
                    return;

                if (Creature* l_Barricade = Creature::GetCreature(*me, m_TargetGUID))
                    l_Barricade->PlayOneShotAnimKitId(4849);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_legion_devastatorAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Fel Fireball - 252614
class spell_antorus_fel_fireball : public SpellScriptLoader
{
    public:
        spell_antorus_fel_fireball() : SpellScriptLoader("spell_antorus_fel_fireball") { }

        class spell_antorus_fel_fireball_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_fel_fireball_SpellScript);

            enum eSpells
            {
                FelFireball = 252606
            };

            void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FelFireball, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_fel_fireball_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_fel_fireball_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Battle Suit Cosmetic Bombardment - 252789
class spell_antorus_battle_suit_cosmetic_bombardment : public SpellScriptLoader
{
    public:
        spell_antorus_battle_suit_cosmetic_bombardment() : SpellScriptLoader("spell_antorus_battle_suit_cosmetic_bombardment") { }

        class spell_antorus_battle_suit_cosmetic_bombardment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_battle_suit_cosmetic_bombardment_SpellScript);

            enum eSpells
            {
                BombardmentMissile = 252791
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::BombardmentMissile, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_battle_suit_cosmetic_bombardment_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_battle_suit_cosmetic_bombardment_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Lightforged Teleport Pod - 128289
class npc_antorus_lightforged_teleport_pod : public CreatureScript
{
    public:
        npc_antorus_lightforged_teleport_pod() : CreatureScript("npc_antorus_lightforged_teleport_pod") { }

        struct npc_antorus_lightforged_teleport_podAI : public ScriptedAI
        {
            npc_antorus_lightforged_teleport_podAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void StartCosmeticEvent(CosmeticEventEntry const& p_EventEntry, int32 p_Duration) override
            {
                me->SetHomePosition(me->GetPosition());

                switch (MAKE_PAIR64(p_EventEntry.EventId, p_EventEntry.Index))
                {
                    case MAKE_PAIR64(eCosmeticEvents::ClubfistBeastlordDefeated, 1):
                        me->m_SpellHelper.GetUint32(eSpellHelpers::AntorusTeleportPodID) = eTeleportPods::FelhoundsToHighCommand;
                        break;
                    case MAKE_PAIR64(eCosmeticEvents::ClubfistBeastlordDefeated, 2):
                        me->m_SpellHelper.GetUint32(eSpellHelpers::AntorusTeleportPodID) = eTeleportPods::HighCommandToFelhounds;
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->GetDistance(me->GetHomePosition()) > 0.5f)
                    me->NearTeleportTo(me->GetHomePosition());
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lightforged_teleport_podAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Gateway - 253774
class spell_antorus_gateway_to_high_command : public SpellScriptLoader
{
    public:
        spell_antorus_gateway_to_high_command() : SpellScriptLoader("spell_antorus_gateway_to_high_command") { }

        class spell_antorus_gateway_to_high_command_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_gateway_to_high_command_SpellScript);

            enum eSpells
            {
                Gateway = 253773
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_TeleportPod = l_Caster->FindNearestCreature(eCreatures::NpcLigforgedTeleportPod, 20.0f);
                if (!l_TeleportPod)
                    return;

                uint32 l_PodID = l_TeleportPod->m_SpellHelper.GetUint32(eSpellHelpers::AntorusTeleportPodID);
                std::vector<G3D::Vector3> const& l_Path = k_GatewayPaths[l_PodID][urand(0, k_GatewayPaths[l_PodID].size() - 1)];

                l_Caster->CastSpell(l_Caster, eSpells::Gateway, true);
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Gateway))
                {
                    switch (l_PodID)
                    {
                        case eTeleportPods::FelhoundsToHighCommand:
                        case eTeleportPods::HighCommandToFelhounds:
                            l_Aura->SetDuration(4 * TimeConstants::IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                l_Caster->SetSpeed(UnitMoveType::MOVE_RUN, 7.0f);
                l_Caster->GetMotionMaster()->MoveSmoothFlyPath(m_scriptSpellId, l_Path.data(), l_Path.size());
                l_Caster->UpdateSpeed(UnitMoveType::MOVE_RUN, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_antorus_gateway_to_high_command_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_gateway_to_high_command_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Fel Portal - 127050
/// Fel Portal - 126257
class npc_antorus_fel_portal : public CreatureScript
{
    public:
        npc_antorus_fel_portal() : CreatureScript("npc_antorus_fel_portal") { }

        struct npc_antorus_fel_portalAI : public ScriptedAI
        {
            npc_antorus_fel_portalAI(Creature* p_Creature) : ScriptedAI(p_Creature){ }

            int32 m_SummonTimer;

            enum eMiscs
            {
                AnimPortalOpened = 13861
            };

            void Reset() override
            {
                ScriptedAI::Reset();
                ApplyAllImmunities(true);
            }

            void EnterEvadeMode() override
            {
                ScriptedAI::EnterEvadeMode();
                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcPortalAntoranFelguard, 50.0f);
                me->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcPortalAntoranChampion, 50.0f);
                for (Creature* l_Creature : l_Creatures)
                    l_Creature->DespawnOrUnsummon();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->SetAIAnimKitId(eMiscs::AnimPortalOpened);
                m_SummonTimer = 2 * TimeConstants::IN_MILLISECONDS;

                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->AddUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_SummonTimer -= p_Diff;

                if (m_SummonTimer < 0)
                {
                    m_SummonTimer = 60 * TimeConstants::IN_MILLISECONDS;
                    Creature* l_Summon = nullptr;
                    switch (me->GetEntry())
                    {
                        case eCreatures::NpcFelPortalLeft:
                        {
                            l_Summon = me->SummonCreature(eCreatures::NpcPortalAntoranFelguard, *me);
                            break;
                        }
                        case eCreatures::NpcFelPortalRight:
                        {
                            l_Summon = me->SummonCreature(eCreatures::NpcPortalAntoranChampion, *me);
                            break;
                        }
                        default:
                            break;
                    }

                    if (l_Summon)
                        l_Summon->SetInCombatWithZone();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_fel_portalAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Hulking Demolisher - 130192
class npc_antorus_call_assistance_legion_combat : public CreatureScript
{
    public:
        npc_antorus_call_assistance_legion_combat() : CreatureScript("npc_antorus_call_assistance_legion_combat") { }

        struct npc_antorus_call_assistance_legion_combatAI : public LegionCombatAI
        {
            npc_antorus_call_assistance_legion_combatAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->CallAssistance(50.0f, false);
                LegionCombatAI::EnterCombat(p_Attacker);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_call_assistance_legion_combatAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Soulburn - 253599
class spell_antorus_soulburn_searcher : public SpellScriptLoader
{
    public:
        spell_antorus_soulburn_searcher() : SpellScriptLoader("spell_antorus_soulburn_searcher") { }

        class spell_antorus_soulburn_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_soulburn_searcher_SpellScript);

            enum eSpells
            {
                SoulburnAura = 253601
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SoulburnAura, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_soulburn_searcher_SpellScript::HandleOnHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_soulburn_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Soulburn - 253600
class spell_antorus_soulburn_aura : public SpellScriptLoader
{
    public:
        spell_antorus_soulburn_aura() : SpellScriptLoader("spell_antorus_soulburn_aura") { }

        class spell_antorus_soulburn_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antorus_soulburn_aura_AuraScript);

            enum eSpells
            {
                SoulburnDamage = 253601
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SoulburnDamage, true);

                std::list<Player*> l_PlayerList;
                l_Target->GetPlayerListInGrid(l_PlayerList, GetSpellInfo()->Effects[EFFECT_0].BasePoints);
                if (l_PlayerList.empty())
                    return;

                JadeCore::RandomResizeList(l_PlayerList, std::min<int32>(l_PlayerList.size(), GetSpellInfo()->Effects[EFFECT_1].BasePoints));
                for (Player* l_Player : l_PlayerList)
                    l_Player->CastSpell(l_Player, m_scriptSpellId, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_antorus_soulburn_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antorus_soulburn_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Clobex - 127732
class npc_antorus_clobex : public CreatureScript
{
    public:
        npc_antorus_clobex() : CreatureScript("npc_antorus_clobex") { }

        struct npc_antorus_clobexAI : public LegionCombatAI
        {
            npc_antorus_clobexAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            void JustDied(Unit* p_Attacker) override
            {
                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(eData::DataClobex, DONE);

                LegionCombatAI::JustDied(p_Attacker);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_clobexAI(p_Creature);
        }
};

/// Daughter of Eonar - 126982
class npc_antorus_daughter_of_eonar : public CreatureScript
{
    public:
        npc_antorus_daughter_of_eonar() : CreatureScript("npc_antorus_daughter_of_eonar") { }

        enum eSpells
        {
            SpellCosmeticLifeBeam           = 253438,

            SpellGrowth                     = 169568,
            SpellGrowthAT                   = 169520,
            SpellGrowthAT2                  = 169518,
            SpellGrowthAT3                  = 169521,
            SpellBeckonLife                 = 253522,
            SpellEonarCosmeticFlowerGrowth  = 253964
        };

        struct npc_antorus_daughter_of_eonarAI : public Scripted_NoMovementAI
        {
            npc_antorus_daughter_of_eonarAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override
            {
                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (Creature* l_Eonar = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossEssenceOfEonar)))
                        {
                            if (l_Eonar->IsAIEnabled)
                                l_Eonar->AI()->DoAction(0);
                        }
                    }
                });

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if ((me->m_positionZ >= 770.0f && me->m_positionZ <= 800.0f) || (me->m_positionZ >= 720.0f && me->m_orientation >= 0.4f && me->m_orientation <= 0.5f))
                        DoCast(me, eSpells::SpellCosmeticLifeBeam, true);
                    else if (me->m_orientation >= 4.12f && me->m_orientation <= 4.13f)
                    {
                        std::vector<G3D::Vector3> l_Path =
                        {
                            { -4184.678f, -11157.99f, 720.9686f },
                            { -4189.100f, -11164.65f, 721.8436f },
                            { -4190.205f, -11166.31f, 722.4686f },
                            { -4191.311f, -11167.97f, 723.5491f },
                            { -4192.969f, -11170.47f, 725.7991f },
                            { -4195.180f, -11173.80f, 726.4241f },
                            { -4197.391f, -11177.13f, 727.0491f },
                            { -4199.049f, -11179.62f, 728.7991f },
                            { -4199.906f, -11180.91f, 728.8067f }
                        };

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothPath(eSpells::SpellBeckonLife, l_Path.data(), l_Path.size(), true);

                        std::vector<Position> l_Dests =
                        {
                            { -4168.288f, -11137.44f, 720.6558f, 0.4941051f },
                            { -4168.225f, -11136.79f, 720.7415f, 0.8581988f },
                            { -4171.529f, -11140.32f, 720.7498f, 0.9303721f },
                            { -4169.249f, -11141.32f, 720.9686f, 0.8734046f }
                        };

                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellGrowthAT);

                        for (Position const& l_Pos : l_Dests)
                        {
                            AreaTrigger* l_AreaTrigger = new AreaTrigger;
                            if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_AREATRIGGER), 5352, me, nullptr, l_SpellInfo, l_Pos, l_Pos))
                            {
                                delete l_AreaTrigger;
                                return;
                            }

                            DoCast(l_Pos, eSpells::SpellGrowth, true);
                        }
                    }
                });
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == eSpells::SpellBeckonLife)
                    DoCast(me, eSpells::SpellBeckonLife, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBeckonLife)
                    DoCast(me, eSpells::SpellEonarCosmeticFlowerGrowth, true);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBeckonLife)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellBeckonLife, true);
                    });
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode != SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellGrowthAT:
                    case eSpells::SpellGrowthAT2:
                    case eSpells::SpellGrowthAT3:
                    {
                        DoCast(p_Dest->_position, eSpells::SpellGrowth, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_daughter_of_eonarAI(p_Creature);
        }
};

/// Eonar Cosmetic Flower Growth - 253964
class spell_antorus_eonar_cosmetic_flower_growth : public SpellScriptLoader
{
    public:
        spell_antorus_eonar_cosmetic_flower_growth() : SpellScriptLoader("spell_antorus_eonar_cosmetic_flower_growth") { }

        enum eSpells
        {
            SpellGrowthAT   = 169520,
            SpellGrowthAT2  = 169518,
            SpellGrowthAT3  = 169521
        };

        class spell_antorus_eonar_cosmetic_flower_growth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antorus_eonar_cosmetic_flower_growth_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    std::vector<uint32> l_Spells = { eSpells::SpellGrowthAT, eSpells::SpellGrowthAT2, eSpells::SpellGrowthAT3 };

                    l_Target->CastSpell(l_Target, l_Spells[urand(0, l_Spells.size() - 1)], true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_antorus_eonar_cosmetic_flower_growth_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antorus_eonar_cosmetic_flower_growth_AuraScript();
        }
};

/// Lifebinder Construct - 127681
class npc_antorus_lifebinder_construct : public CreatureScript
{
    public:
        npc_antorus_lifebinder_construct() : CreatureScript("npc_antorus_lifebinder_construct") { }

        enum eSpells
        {
            SpellBlessingOfTheLifebinder    = 254097,
            SpellEssenceOfTheLifebinder     = 253875
        };

        struct npc_antorus_lifebinder_constructAI : public Scripted_NoMovementAI
        {
            npc_antorus_lifebinder_constructAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || p_Who->GetExactDist(me) >= 30.0f)
                    return;

                if (p_Who->HasAura(eSpells::SpellEssenceOfTheLifebinder))
                    return;

                if (m_Instance && m_Instance->GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE)
                    return;

                DoCast(p_Who, eSpells::SpellBlessingOfTheLifebinder, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBlessingOfTheLifebinder)
                    p_Target->CastSpell(p_Target, eSpells::SpellEssenceOfTheLifebinder, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_lifebinder_constructAI(p_Creature);
        }
};

/// Lightforged Warframe - 127963
class npc_antorus_warframe_to_imonar : public CreatureScript
{
    public:
        npc_antorus_warframe_to_imonar() : CreatureScript("npc_antorus_warframe_to_imonar") { }

        enum eSpells
        {
            SpellCallingBattlesuit = 253936
        };

        bool OnGossipSelect(Player* p_Player, Creature* /*p_Creature*/, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            if (!p_Player || !p_Player->GetInstanceScript())
                return false;

            InstanceScript* l_Instance = p_Player->GetInstanceScript();
            if (!l_Instance || l_Instance->IsEncounterInProgress())
                return false;

            /// @TODO: Also check skip quest
            /*if (l_Instance->GetBossState(eData::DataFelhoundsOfSargeras) != EncounterState::DONE ||
                l_Instance->GetBossState(eData::DataAntoranHighCommand) != EncounterState::DONE ||
                l_Instance->GetBossState(eData::DataPortalKeeperHasabel) != EncounterState::DONE ||
                l_Instance->GetBossState(eData::DataEonarTheLifeBinder) != EncounterState::DONE)
                return false;*/

            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->CastSpell(p_Player, eSpells::SpellCallingBattlesuit);
            return true;
        }
};

enum eGossipData
{
    GossipMenuID = 21377,
    GossipTextID = 32550,

    GossipBrokenCliffs  = 1,
    GossipTheExhaust    = 2,
    GossipBurningThrone = 3,
    GossipVindicaar     = 4,
    GossipAggramar      = 5
};

static std::map<uint32, uint32> k_GossipXEntry =
{
    { eGossipData::GossipBrokenCliffs,  eCreatures::NpcTeleportPodBrokenCliffs  },
    { eGossipData::GossipTheExhaust,    eCreatures::NpcTeleportPodTheExhaust    },
    { eGossipData::GossipBurningThrone, eCreatures::NpcTeleportPodBurningThrone }
};

/// Lightforged Beacon - 125720 / 128303 / 128304 / 130137
class npc_antorus_lightforged_beacon : public CreatureScript
{
    public:
        npc_antorus_lightforged_beacon() : CreatureScript("npc_antorus_lightforged_beacon") { }

        enum eSpell
        {
            SpellRaidTransition01 = 254498,
            SpellRaidTransition02 = 254499,
            SpellArrivalatWing03  = 250695
        };

        void AddTeleportOption(uint32 p_Id, uint32 p_Entry, Player* p_Player, uint32 p_OptionIndex)
        {
            auto l_Itr = k_GossipXEntry.find(p_Id);
            if (l_Itr != k_GossipXEntry.end() && l_Itr->second == p_Entry)
                return;

            p_Player->ADD_GOSSIP_ITEM_DB(eGossipData::GossipMenuID, p_OptionIndex, eTradeskill::GOSSIP_SENDER_MAIN, p_Id);
        }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player || !p_Player->GetInstanceScript())
                return true;

            InstanceScript* l_Instance = p_Player->GetInstanceScript();
            if (!l_Instance)
                return true;

            p_Player->PlayerTalkClass->ClearGossipMenu();

            if (l_Instance->GetData(eData::DataBrokenCliffsEnabled))
                AddTeleportOption(eGossipData::GossipBrokenCliffs, p_Creature->GetEntry(), p_Player, 0);

            if ((l_Instance->GetBossState(eData::DataGarothiWorldbreaker) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataFelhoundsOfSargeras) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataAntoranHighCommand) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataPortalKeeperHasabel) == EncounterState::DONE &&
                l_Instance->GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE) ||
                l_Instance->GetData(eData::DataImonarSkipQuest) > 0 ||
                l_Instance->GetData(eData::DataDungeonID) == eDungeonIDs::DungeonIDForbiddenDescent)
                AddTeleportOption(eGossipData::GossipTheExhaust, p_Creature->GetEntry(), p_Player, 1);

            if (l_Instance->GetBossState(eData::DataKinGaroth) == EncounterState::DONE)
                AddTeleportOption(eGossipData::GossipBurningThrone, p_Creature->GetEntry(), p_Player, 2);

            if (l_Instance->GetData(eData::DataAggramarSkipQuest) > 0)
                AddTeleportOption(eGossipData::GossipAggramar, p_Creature->GetEntry(), p_Player, 4);

            if (l_Instance->GetBossState(eData::DataArgusTheUnmaker) == EncounterState::DONE)
                AddTeleportOption(eGossipData::GossipVindicaar, p_Creature->GetEntry(), p_Player, 3);

            p_Player->SEND_GOSSIP_MENU(eGossipData::GossipTextID, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* /*p_Creature*/, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (!p_Player || !p_Player->GetInstanceScript())
                return false;

            InstanceScript* l_Instance = p_Player->GetInstanceScript();
            if (!l_Instance || l_Instance->IsEncounterInProgress())
                return false;

            switch (p_Action)
            {
                case eGossipData::GossipBrokenCliffs:
                {
                    p_Player->NearTeleportTo({ -3420.798f, 10153.00f, -150.11f, 5.65f });
                    break;
                }
                case eGossipData::GossipTheExhaust:
                {
                    p_Player->CastSpell(p_Player, eSpell::SpellRaidTransition01, true);
                    p_Player->NearTeleportTo({ -10574.8f, 8772.41f, 1871.47f, 4.732111f });
                    break;
                }
                case eGossipData::GossipBurningThrone:
                {
                    if (l_Instance->GetData(eData::DataTarneth) != DONE)
                    {
                        p_Player->CastSpell(p_Player, eSpell::SpellRaidTransition02, true);
                        p_Player->AddDelayedEvent([p_Player]() -> void
                        {
                            p_Player->CastSpell(p_Player, eSpell::SpellArrivalatWing03, true);
                        }, 3 * TimeConstants::IN_MILLISECONDS);
                    }

                    p_Player->NearTeleportTo({ -12634.4287f, -3355.1890f, 2515.f, 1.84f });
                    break;
                }
                case eGossipData::GossipAggramar:
                {
                    if (!l_Instance->GetData(eData::DataAggramarSkipQuest))
                        break;

                    p_Player->NearTeleportTo(eLocations::LocationAggramar);
                    break;
                }
                case eGossipData::GossipVindicaar:
                {
                    p_Player->CastSpell(p_Player, eSharedSpells::SpellArgusOutroTeleport, true);
                    break;
                }
                default:
                    return false;
            }

            p_Player->PlayerTalkClass->SendCloseGossip();

            return true;
        }
};

/// Calling Battlesuit - 253936
class spell_antorus_calling_battlesuit : public SpellScriptLoader
{
    public:
        spell_antorus_calling_battlesuit() : SpellScriptLoader("spell_antorus_calling_battlesuit") { }

        enum eSpells
        {
            SpellBattlesuitLanding  = 253832,
            SpellTravelForm         = 783
        };

        class spell_antorus_calling_battlesuit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_calling_battlesuit_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                l_Caster->RemoveAurasByType(AuraType::SPELL_AURA_MOD_SHAPESHIFT);

                /// Druid's Travel Form isn't a shapeshift
                l_Caster->RemoveAura(eSpells::SpellTravelForm);

                l_Caster->ToPlayer()->SetClientControl(l_Caster, 0);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    Movement::MoveSplineInit l_Init(l_Caster);

                    Optional<Movement::MonsterSplineFilter> l_Filter;

                    l_Init.SetFly();
                    l_Init.SetSmooth();
                    l_Init.SetUncompressed();

                    l_Filter.emplace();

                    l_Filter->FilterFlags   = 3;
                    l_Filter->BaseSpeed     = 50.0f;
                    l_Filter->AddedToStart  = 1;

                    l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(0, 5000));
                    l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(2, 3000));

                    l_Init.SetMonsterSplineFilters(*l_Filter.get_ptr());

                    std::vector<G3D::Vector3> l_Path =
                    {
                        { -3195.155f, 10088.18f, -64.80678f },
                        { -3297.182f, 10028.27f, -72.57681f },
                        { -3359.228f, 10063.88f, -72.57681f },
                        { -3380.960f, 10132.01f, -152.8872f }
                    };

                    for (G3D::Vector3 const& l_Point : l_Path)
                        l_Init.Path().push_back(l_Point);

                    l_Init.Launch();

                    l_Caster->DelayedCastSpell(l_Caster, eSpells::SpellBattlesuitLanding, true, 6 * TimeConstants::IN_MILLISECONDS + 500);

                }, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_calling_battlesuit_SpellScript::HandleOnHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_calling_battlesuit_SpellScript();
        }
};

/// Battle Suit Landing - 253832
class spell_antorus_battle_suit_landing : public SpellScriptLoader
{
    public:
        spell_antorus_battle_suit_landing() : SpellScriptLoader("spell_antorus_battle_suit_landing") { }

        enum eSpell
        {
            SpellCallingBattlesuit = 253936
        };

        class spell_antorus_battle_suit_landing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antorus_battle_suit_landing_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->IsPlayer())
                        return;

                    l_Target->RemoveAura(eSpell::SpellCallingBattlesuit);
                    l_Target->StopMoving();
                    l_Target->GetMotionMaster()->Clear();

                    l_Target->ToPlayer()->SetClientControl(l_Target, 1);

                    l_Target->NearTeleportTo({ -3380.960f, 10132.01f, -152.8872f, 1.974f });
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_antorus_battle_suit_landing_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antorus_battle_suit_landing_AuraScript();
        }
};

/// NPC 127231 - Garothi Decimator (Pre Kin'garoth)
class npc_antorus_trash_garothi_decimator : public CreatureScript
{
    public:
        npc_antorus_trash_garothi_decimator() : CreatureScript("npc_antorus_trash_garothi_decimator") { }

        struct npc_antorus_trash_garothi_decimatorAI : public ScriptedAI
        {
            npc_antorus_trash_garothi_decimatorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventDecimator = 1
            };

            enum eSpells
            {
                SpellDecimator       = 252793,
                SpellDecimatorTarget = 252798
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventDecimator, urand(5, 10) * IN_MILLISECONDS);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDecimator:
                    {
                        DoCast(eSpells::SpellDecimatorTarget);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventDecimator:
                            DoCast(eSpells::SpellDecimator);
                            events.ScheduleEvent(eEvents::EventDecimator, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_garothi_decimatorAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Decimation - 252798
class spell_antorus_trash_decimator : public SpellScriptLoader
{
public:
    spell_antorus_trash_decimator() : SpellScriptLoader("spell_antorus_trash_decimator") { }

    class spell_antorus_trash_decimator_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_antorus_trash_decimator_SpellScript);

        enum eSpells
        {
            SpellDecimatorDmg = 252796
        };

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_Target || !l_SpellInfo)
                return;

            l_Caster->CastSpell(l_Target, l_SpellInfo->Effects[EFFECT_0].BasePoints, true);
            l_Caster->DelayedCastSpell(l_Target, eSpells::SpellDecimatorDmg, true, 2500);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_antorus_trash_decimator_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_antorus_trash_decimator_SpellScript();
    }
};

/// NPC 127235 - Garothi Demolisher (Pre Kin'garoth)
class npc_antorus_trash_garothi_demolisher : public CreatureScript
{
    public:
        npc_antorus_trash_garothi_demolisher() : CreatureScript("npc_antorus_trash_garothi_demolisher") { }

        struct npc_antorus_trash_garothi_demolisherAI : public ScriptedAI
        {
            npc_antorus_trash_garothi_demolisherAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventDemolish = 1
            };

            enum eSpells
            {
                SpellDemolish        = 252758
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventDemolish, urand(5, 10) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventDemolish:
                            DoCast(eSpells::SpellDemolish);
                            events.ScheduleEvent(eEvents::EventDemolish, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_garothi_demolisherAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Demolish - 252758
class spell_antorus_trash_demolish : public SpellScriptLoader
{
public:
    spell_antorus_trash_demolish() : SpellScriptLoader("spell_antorus_trash_demolish") { }

    class spell_antorus_trash_demolish_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_antorus_trash_demolish_SpellScript);

        enum eSpells
        {
            SpellDemolishAura = 252760
        };

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_Target || !l_SpellInfo)
                return;

            l_Caster->CastSpell(l_Target, eSpells::SpellDemolishAura, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_antorus_trash_demolish_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_antorus_trash_demolish_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Demolish - 252760
class spell_antorus_trash_demolish_aura : public SpellScriptLoader
{
    public:
        spell_antorus_trash_demolish_aura() : SpellScriptLoader("spell_antorus_trash_demolish_aura") { }

        class spell_antorus_trash_demolish_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antorus_trash_demolish_aura_AuraScript);

            enum eSpells
            {
                SpellDemolishDmg = 252762
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellDemolishDmg, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_antorus_trash_demolish_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antorus_trash_demolish_aura_AuraScript();
        }
};

/// NPC 127230 - Garothi Annihilator (Pre Kin'garoth)
class npc_antorus_trash_garothi_annihilator : public CreatureScript
{
    public:
        npc_antorus_trash_garothi_annihilator() : CreatureScript("npc_antorus_trash_garothi_annihilator") { }

        struct npc_antorus_trash_garothi_annihilatorAI : public ScriptedAI
        {
            npc_antorus_trash_garothi_annihilatorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventAnnihilation  = 1
            };

            enum eSpells
            {
                SpellAnnihilation       = 252740,
                SpellAnnihilationSummon = 252741,
                SpellAnnihilationDmg    = 252742
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventAnnihilation, urand(5, 10) * IN_MILLISECONDS);
            }

            void SearchTarget()
            {
                DoCast(eSpells::SpellAnnihilation);
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 60.f);

                if (playerList.size() > 5)
                    JadeCore::RandomResizeList(playerList, 5);

                for (auto player : playerList)
                {
                    Position l_Pos = *player;
                    Position l_Dest;
                    l_Pos.SimplePosXYRelocationByAngle(l_Dest, frand(6.f, 15.f), frand(0.0f, 2 * M_PI));
                    me->CastSpell(l_Dest, eSpells::SpellAnnihilationSummon, true);
                    me->CastSpell(l_Dest, eSpells::SpellAnnihilationDmg, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventAnnihilation:
                            SearchTarget();
                            events.ScheduleEvent(eEvents::EventAnnihilation, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_garothi_annihilatorAI(p_Creature);
        }
};

/// NPC 124160 - Annihilator (Pre Kin'garoth)
class npc_antorus_trash_annihilator : public CreatureScript
{
    public:
        npc_antorus_trash_annihilator() : CreatureScript("npc_antorus_trash_annihilator") { }

        struct npc_antorus_trash_annihilatorAI : public ScriptedAI
        {
            npc_antorus_trash_annihilatorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventAnnihilationAT  = 1,
                EventAnnihilationCheck
            };

            enum eSpells
            {
                SpellAnnihilationAT     = 252745,
                SpellAnnihilationVisual = 252746
            };

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                events.Reset();
                events.ScheduleEvent(eEvents::EventAnnihilationAT, 100);
                events.ScheduleEvent(eEvents::EventAnnihilationCheck, 200);
            }

            void SearchTarget()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 5.f);
                Position l_Pos = *me;
                playerList.remove_if([l_Pos](Player* p_Target) -> bool
                {
                    if (p_Target->GetExactDist(&l_Pos) > 4.0f)
                        return true;

                    return false;
                });
                if (playerList.empty())
                {
                    if (!me->HasAura(eSpells::SpellAnnihilationVisual))
                        DoCast(eSpells::SpellAnnihilationVisual);
                }
                else
                {
                    if (me->HasAura(eSpells::SpellAnnihilationVisual))
                        me->RemoveAura(eSpells::SpellAnnihilationVisual);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventAnnihilationAT:
                            DoCast(eSpells::SpellAnnihilationAT, true);
                            DoCast(eSpells::SpellAnnihilationVisual, true);
                            break;
                        case eEvents::EventAnnihilationCheck:
                            SearchTarget();
                            events.ScheduleEvent(eEvents::EventAnnihilationCheck, 200);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_annihilatorAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Annihilation - 252743
class spell_antorus_trash_annihilation : public SpellScriptLoader
{
    public:
        spell_antorus_trash_annihilation() : SpellScriptLoader("spell_antorus_trash_annihilation") { }

        class spell_antorus_trash_annihilation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_trash_annihilation_SpellScript);

            enum eSpells
            {
                SpellDemolishAOEDmg = 252744
            };

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                l_CountTargets = int32(p_Targets.size());
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (l_CountTargets || !l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::SpellDemolishAOEDmg, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_antorus_trash_annihilation_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_antorus_trash_annihilation_SpellScript::HandleAfterCast);
            }

        private:
            int32 l_CountTargets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_trash_annihilation_SpellScript();
        }
};

/// Magni Bronzebeard - 125584
class npc_antorus_magni_bronzebeard : public CreatureScript
{
    public:
        npc_antorus_magni_bronzebeard() : CreatureScript("npc_antorus_magni_bronzebeard") { }

        enum eSpell
        {
            SpellEonarTransform = 249296
        };

        enum eConversation
        {
            ConversationEonar = 249794
        };

        struct npc_antorus_magni_bronzebeardAI : public Scripted_NoMovementAI
        {
            npc_antorus_magni_bronzebeardAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            bool m_FirstWingClearedOutro = false;

            void DoAction(int32 const /*p_Action*/) override
            {
                me->SetAIAnimKitId(0);

                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversation::ConversationEonar, me, nullptr, me->GetPosition()))
                        delete l_Conversation;
                }

                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!m_Instance)
                        return;

                    if (Creature* l_Eonar = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcImageOfEonar)))
                        l_Eonar->CastSpell(l_Eonar, eSpell::SpellEonarTransform, true);
                });

                AddTimedDelayedOperation(30 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!m_Instance)
                        return;

                    if (Creature* l_Eonar = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcImageOfEonar)))
                        l_Eonar->RemoveAura(eSpell::SpellEonarTransform);
                });
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || p_Who->GetExactDist(me) > 20.0f)
                    return;

                if (m_FirstWingClearedOutro || !m_Instance || !m_Instance->GetData(eData::DataFirstWingClearedIntro))
                    return;

                m_FirstWingClearedOutro = true;

                p_Who->DelayedCastSpell(p_Who, eSharedSpells::SpellWing1ClearedOutro, true, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_magni_bronzebeardAI(p_Creature);
        }
};

/// Antoran Felguard - 125590
class npc_antorus_antoran_felguard_rp : public CreatureScript
{
    public:
        npc_antorus_antoran_felguard_rp() : CreatureScript("npc_antorus_antoran_felguard_rp") { }

        struct npc_antorus_antoran_felguard_rpAI : public Scripted_NoMovementAI
        {
            npc_antorus_antoran_felguard_rpAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                me->SetReactState(ReactStates::REACT_DEFENSIVE);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_Instance)
                    return;

                if (Creature* l_Magni = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcMagniBronzebeard)))
                {
                    if (l_Magni->IsAIEnabled)
                        l_Magni->AI()->DoAction(0);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_antoran_felguard_rpAI(p_Creature);
        }
};

/// Portal to Hasabel/Eonar's area - 277335
class go_antorus_portal_to_hasabel_eonar : public GameObjectScript
{
    public:
        go_antorus_portal_to_hasabel_eonar() : GameObjectScript("go_antorus_portal_to_hasabel_eonar") { }

        enum eSpell
        {
            SpellPortalKeeperApproach = 249124
        };

        struct go_antorus_portal_to_hasabel_eonarAI : public GameObjectAI
        {
            go_antorus_portal_to_hasabel_eonarAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            bool m_IntroDone = false;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_IntroDone || !p_Who->IsPlayer() || p_Who->GetDistance(go) > 40.0f)
                    return;

                InstanceScript* l_Instance = go->GetInstanceScript();
                if (!l_Instance)
                    return;

                if (l_Instance->GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE &&
                    l_Instance->GetBossState(eData::DataPortalKeeperHasabel) == EncounterState::DONE)
                    return;

                m_IntroDone = true;

                p_Who->CastSpell(p_Who, eSpell::SpellPortalKeeperApproach, true);
            }

            void UpdateAI(uint32 p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_antorus_portal_to_hasabel_eonarAI(p_GameObject);
        }
};

/// NPC 123532 - Dark Keeper (Pre Varimathras)
class npc_antorus_trash_dark_keeper : public CreatureScript
{
    public:
        npc_antorus_trash_dark_keeper() : CreatureScript("npc_antorus_trash_dark_keeper") { }

        struct npc_antorus_trash_dark_keeperAI : public ScriptedAI
        {
            npc_antorus_trash_dark_keeperAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                l_Reconstitute = false;
                l_TimerRegen = 1 * IN_MILLISECONDS;
            }

            enum eEvents
            {
                EventDrawFlames  = 1,
                EventDestruction
            };

            enum eSpells
            {
                SpellDrawFlames    = 246903,
                SpellDestruction   = 249436,
                SpellReconstitute  = 246589
            };

            bool l_Reconstitute;
            int32 l_TimerRegen;

            void Reset() override
            {
                events.Reset();
                l_Reconstitute = false;
                me->SetPower(POWER_ENERGY, 0);
                l_TimerRegen = 1 * IN_MILLISECONDS;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventDrawFlames, urand(9, 10) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDestruction, urand(15, 16) * IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                ScriptedAI::EnterEvadeMode();
                l_Reconstitute = false;
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!l_Reconstitute && me->HealthBelowPctDamaged(11, p_Damage))
                {
                    l_Reconstitute = true;
                    DoCast(eSpells::SpellReconstitute);
                }
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDrawFlames:
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellInfo->Id, me->GetMap()->GetDifficultyID()))
                            me->ModifyPower(POWER_ENERGY, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                l_TimerRegen -= p_Diff;

                if (l_TimerRegen < 0)
                {
                    l_TimerRegen = 1 * IN_MILLISECONDS;
                    me->ModifyPower(POWER_ENERGY, 1);
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventDrawFlames:
                            DoCast(eSpells::SpellDrawFlames);
                            events.ScheduleEvent(eEvents::EventDrawFlames, urand(9, 10) * IN_MILLISECONDS);
                            break;
                        case eEvents::EventDestruction:
                            DoCast(eSpells::SpellDestruction);
                            events.ScheduleEvent(eEvents::EventDestruction, urand(15, 16) * IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_dark_keeperAI(p_Creature);
        }
};

/// NPC 123533 - Tarneth (Pre Varimathras)
class npc_antorus_trash_tarneth : public CreatureScript
{
    public:
        npc_antorus_trash_tarneth() : CreatureScript("npc_antorus_trash_tarneth") { }

        struct npc_antorus_trash_tarnethAI : public ScriptedAI
        {
            npc_antorus_trash_tarnethAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
               l_TimerRegen = 1 * IN_MILLISECONDS;
            }

            int32 l_TimerRegen;

            enum eEvents
            {
                EventDrawFlames  = 1,
                EventFlamesofReorigination
            };

            enum eSpells
            {
                SpellDrawFlames             = 246903,
                SpellFlamesofReorigination  = 249418
            };

            void Reset() override
            {
                events.Reset();
                me->SetPower(POWER_ENERGY, 0);
                l_TimerRegen = 1 * IN_MILLISECONDS;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventDrawFlames, urand(9, 10) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFlamesofReorigination, urand(15, 16) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!me->GetInstanceScript())
                    return;

                me->GetInstanceScript()->SetData(eData::DataTarneth, DONE);

                if (Creature* l_MagniBronzebeard = me->FindNearestCreature(eCreatures::NpcMagniBronzebeard2, 135.0f))
                    l_MagniBronzebeard->GetMotionMaster()->MovePoint(100, -12631.5400f, -2990.3799f, 2498.8601f);

                if (Creature* l_IllidanStormrage = me->FindNearestCreature(eCreatures::NpcIllidanStormrage, 135.0f))
                    l_IllidanStormrage->GetMotionMaster()->MovePoint(100, -12638.7002f, -2989.0205f, 2500.8601f);

                if (Creature* l_ProphetVelen = me->FindNearestCreature(eCreatures::NpcProphetVelen, 135.0f))
                    l_ProphetVelen->GetMotionMaster()->MovePoint(100, -12626.7598f, -2992.1624f, 2498.8601f);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDrawFlames:
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellInfo->Id, me->GetMap()->GetDifficultyID()))
                            me->ModifyPower(POWER_ENERGY, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                l_TimerRegen -= p_Diff;

                if (l_TimerRegen < 0)
                {
                    l_TimerRegen = 1 * IN_MILLISECONDS;
                    me->ModifyPower(POWER_ENERGY, 1);
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventDrawFlames:
                            DoCast(eSpells::SpellDrawFlames);
                            events.ScheduleEvent(eEvents::EventDrawFlames, urand(9, 10) * IN_MILLISECONDS);
                            break;
                        case eEvents::EventFlamesofReorigination:
                            DoCast(eSpells::SpellFlamesofReorigination);
                            events.ScheduleEvent(eEvents::EventFlamesofReorigination, urand(15, 16) * IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_tarnethAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Flames of Reorigination - 249418
class spell_antorus_flames_reorigination : public SpellScriptLoader
{
    public:
        spell_antorus_flames_reorigination() : SpellScriptLoader("spell_antorus_flames_reorigination") { }

        class spell_antorus_flames_reorigination_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_flames_reorigination_SpellScript);

            enum eSpells
            {
                SpellFlamesofReoriginationAura = 249297
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellFlamesofReoriginationAura);
                if (!l_Target || !p_SpellInfo || l_Target->IsImmunedToSpell(p_SpellInfo))
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellFlamesofReoriginationAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_flames_reorigination_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_flames_reorigination_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Flames of Reorigination - 249303
class spell_antorus_flames_reorigination_trigger : public SpellScriptLoader
{
    public:
        spell_antorus_flames_reorigination_trigger() : SpellScriptLoader("spell_antorus_flames_reorigination_trigger") { }

        class spell_antorus_flames_reorigination_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_flames_reorigination_trigger_SpellScript);

            enum eSpells
            {
                SpellFlamesofReoriginationAura = 249297
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellFlamesofReoriginationAura);
                if (!l_Caster || !l_Target || !p_SpellInfo || l_Target->IsImmunedToSpell(p_SpellInfo) || l_Caster == l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellFlamesofReoriginationAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_flames_reorigination_trigger_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_flames_reorigination_trigger_SpellScript();
        }
};

/// NPC 128095 - Sister of the Lash (Pre Varimathras)
class npc_antorus_trash_sister_of_the_lash : public CreatureScript
{
    public:
        npc_antorus_trash_sister_of_the_lash() : CreatureScript("npc_antorus_trash_sister_of_the_lash") { }

        struct npc_antorus_trash_sister_of_the_lashAI : public ScriptedAI
        {
            npc_antorus_trash_sister_of_the_lashAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventLashofPunishment = 1
            };

            enum eSpells
            {
                SpellLashofPunishment  = 254178
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventLashofPunishment, urand(9, 10) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventLashofPunishment:
                            DoCast(eSpells::SpellLashofPunishment);
                            events.ScheduleEvent(eEvents::EventLashofPunishment, urand(9, 10) * IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_sister_of_the_lashAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Lash of Punishment - 254178
class spell_antorus_lash_of_punishment : public SpellScriptLoader
{
    public:
        spell_antorus_lash_of_punishment() : SpellScriptLoader("spell_antorus_lash_of_punishment") { }

        class spell_antorus_lash_of_punishment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antorus_lash_of_punishment_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* p_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !p_SpellInfo)
                    return;

                l_Caster->CastSpell(l_Target, p_SpellInfo->Effects[EFFECT_0].BasePoints, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_antorus_lash_of_punishment_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antorus_lash_of_punishment_SpellScript();
        }
};

/// NPC 128060 - Priestess of Delirium (Pre Varimathras)
class npc_antorus_trash_priestess_of_delirium : public CreatureScript
{
    public:
        npc_antorus_trash_priestess_of_delirium() : CreatureScript("npc_antorus_trash_priestess_of_delirium") { }

        struct npc_antorus_trash_priestess_of_deliriumAI : public ScriptedAI
        {
            npc_antorus_trash_priestess_of_deliriumAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventCloudofConfusion = 1
            };

            enum eSpells
            {
                SpellCloudofConfusion  = 254122,
                SpellApproachVarimathras1 = 250797,
                SpellApproachVarimathras2 = 254650
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventCloudofConfusion, 6 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* l_MagniBronzebeard = me->FindNearestCreature(eCreatures::NpcMagniBronzebeard2, 135.0f))
                    l_MagniBronzebeard->GetMotionMaster()->MovePointWithRot(101, -12700.8887f, -2819.9160f, 2483.5579f, 6.27f);

                if (Creature* l_IllidanStormrage = me->FindNearestCreature(eCreatures::NpcIllidanStormrage, 135.0f))
                    l_IllidanStormrage->GetMotionMaster()->MovePointWithRot(101, -12701.6592f, -2813.8125f, 2483.5698f, 6.27f);

                if (Creature* l_ProphetVelen = me->FindNearestCreature(eCreatures::NpcProphetVelen, 135.0f))
                    l_ProphetVelen->GetMotionMaster()->MovePointWithRot(101, -12702.5654f, -2824.4678f, 2483.5579f, 6.27f);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetData(eData::DataVarimathrasIntro) != DONE)
                    {
                        if (Player* l_Player = me->SelectNearestPlayerNotGM(90.0f))
                        {
                            l_Instance->SetData(eData::DataVarimathrasIntro, DONE);
                            me->DelayedCastSpell(l_Player, eSpells::SpellApproachVarimathras2, true, 30 * IN_MILLISECONDS);
                            Map::PlayerList const& l_Players = l_Instance->instance->GetPlayers();
                            for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                            {
                                Player* l_Player = l_Itr->getSource();
                                if (!l_Player)
                                    continue;

                                me->CastSpell(l_Player, eSpells::SpellApproachVarimathras1, true);
                            }
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventCloudofConfusion:
                        {
                            if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.f, true))
                                DoCast(l_Target, eSpells::SpellCloudofConfusion);
                            events.ScheduleEvent(eEvents::EventCloudofConfusion, urand(10, 15) * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_antorus_trash_priestess_of_deliriumAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Lash of Punishment - 254122
class spell_antorus_cloud_of_confusion: public SpellScriptLoader
{
    public:
        spell_antorus_cloud_of_confusion() : SpellScriptLoader("spell_antorus_cloud_of_confusion") { }

        class spell_antorus_cloud_of_confusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antorus_cloud_of_confusion_AuraScript);

            enum eSpells
            {
                SpellCloudofConfusion  = 254125
            };

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellCloudofConfusion, true);
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellCloudofConfusion, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_antorus_cloud_of_confusion_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_antorus_cloud_of_confusion_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antorus_cloud_of_confusion_AuraScript();
        }
};

/// Antorus Meta Achievement
class PlayerScript_antorus_meta_achievement : public PlayerScript
{
    public:
        PlayerScript_antorus_meta_achievement() : PlayerScript("PlayerScript_antorus_meta_achievement") {}

        enum eAntorusAchievements
        {
            GloryToTheArgusRaider       = 11987,

            WormMonger                  = 11930, ///< Garothi Worldbrekaer
        /// HoundsGoodToMe              = 12065, ///< Felhounds of Sargeras
            ThisIsTheWarRoom            = 12129, ///< Antoran High command
            PortalCombat                = 11928, ///< Portal Keeper Hasabel
            SpheresOfInfluence          = 12067, ///< Eonar
            HardToKill                  = 11949, ///< Imonar
            TheWorldRevolvesAroundMe    = 12030, ///< KinGaroth
            TogetherWeStand             = 11948, ///< Varimathras
            RememberTheTitans           = 12046, ///< Coven of Shivarra
            DontSweatTheTechnique       = 11915, ///< Aggramar
            StardustCrusaders           = 12257  ///< Argus
        };

        std::array<uint32, 10> m_Achievements =
        {{
            eAntorusAchievements::WormMonger,
            eAntorusAchievements::ThisIsTheWarRoom,
            eAntorusAchievements::PortalCombat,
            eAntorusAchievements::SpheresOfInfluence,
            eAntorusAchievements::HardToKill,
            eAntorusAchievements::TheWorldRevolvesAroundMe,
            eAntorusAchievements::TogetherWeStand,
            eAntorusAchievements::RememberTheTitans,
            eAntorusAchievements::DontSweatTheTechnique,
            eAntorusAchievements::StardustCrusaders
        }};

        void OnAchievementComplete(Player* p_Player, AchievementEntry const* p_Achievement) override
        {
            if (std::find(m_Achievements.begin(), m_Achievements.end(), p_Achievement->ID) == m_Achievements.end())
                return;

            for (uint32 l_AchievementID : m_Achievements)
            {
                if (!p_Player->GetAchievementMgr()->HasAccountAchieved(l_AchievementID) && (l_AchievementID != p_Achievement->ID))
                    return;
            }

            AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAntorusAchievements::GloryToTheArgusRaider);
            if (!l_AchievementEntry)
                return;

            p_Player->CompletedAchievement(l_AchievementEntry);
        }

        void OnLogin(Player* p_Player) override
        {
            for (uint32 l_AchievementID : m_Achievements)
            {
                if (!p_Player->GetAchievementMgr()->HasAccountAchieved(l_AchievementID))
                    return;
            }

            AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAntorusAchievements::GloryToTheArgusRaider);
            if (!l_AchievementEntry)
                return;

            p_Player->CompletedAchievement(l_AchievementEntry);
        }
};

#ifndef __clang_analyzer__
void AddSC_antorus_the_burning_throne()
{
    /// First platform
    new npc_antorus_lightforged_warframe();

    /// Road to Garothi Worldbreaker platform
    new spell_antorus_searing_rend();
    new spell_antorus_shadow_storm();
    new spell_antorus_shadow_burst();
    new spell_antorus_decimation();
    new spell_antorus_decimation_aura();

    /// Garothi Worldbreaker platform
    new npc_antorus_garothi_annihilator();
    new npc_antorus_lightforged_soldiers();
    new npc_antorus_lightforged_warframe_worldbreaker_intro();
    new spell_antorus_battle_suit_cosmetic();
    new npc_antorus_high_exarch_turalyon();
    new npc_antorus_annihilation();
    new at_antorus_annihilation();
    new spell_antorus_annhiliation();
    new npc_antorus_lightforged_warframe_worldbreaker_outro();
    new npc_antorus_legion_talon();
    new npc_antorus_lightforged_worlbreaker_pod();
    new npc_antorus_lightforged_commander();
    new npc_antorus_lightforged_centurion_dead();
    new at_antorus_hanging_fiend_trigger();
    new npc_antorus_slobbering_fiend();
    new spell_antorus_fel_fireball();

    /// Felhounds platform
    new spell_antorus_battle_suit_cosmetic_bombardment();

    /// Antoran High Command Trashes
    new spell_antorus_gateway_to_high_command();
    new npc_antorus_lightforged_teleport_pod();
    new npc_antorus_fel_portal();
    new npc_antorus_call_assistance_legion_combat();
    new spell_antorus_soulburn_searcher();
    new spell_antorus_soulburn_aura();
    new npc_antorus_clobex();

    /// Portal Keeper Hasabel Trashes
    new npc_antorus_legion_devastator();

    /// Eonar the Life-Binder Trashes
    new npc_antorus_daughter_of_eonar();
    new spell_antorus_eonar_cosmetic_flower_growth();
    new npc_antorus_lifebinder_construct();

    /// Imonar the Soulhunter
    new npc_antorus_warframe_to_imonar();
    new npc_antorus_lightforged_beacon();
    new spell_antorus_calling_battlesuit();
    new spell_antorus_battle_suit_landing();

    ///< Kin'garoth Trashes
    new npc_antorus_trash_garothi_decimator();
    new spell_antorus_trash_decimator();
    new npc_antorus_trash_garothi_demolisher();
    new spell_antorus_trash_demolish();
    new spell_antorus_trash_demolish_aura();
    new npc_antorus_trash_garothi_annihilator();
    new npc_antorus_trash_annihilator();
    new spell_antorus_trash_annihilation();

    /// RP Events
    new npc_antorus_magni_bronzebeard();
    new npc_antorus_antoran_felguard_rp();
    new go_antorus_portal_to_hasabel_eonar();

    ///< Varimathras Trashes
    new npc_antorus_trash_dark_keeper();
    new npc_antorus_trash_tarneth();
    new spell_antorus_flames_reorigination();
    new spell_antorus_flames_reorigination_trigger();
    new npc_antorus_trash_sister_of_the_lash();
    new spell_antorus_lash_of_punishment();
    new npc_antorus_trash_priestess_of_delirium();
    new spell_antorus_cloud_of_confusion();

    ///< Misc
    new PlayerScript_antorus_meta_achievement();
}
#endif
