////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2020 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "seat_of_triumvirate.hpp"

enum eSpells
{
    /// Saprish
    SpellSaprishIntroConver     = 246880,
    SpellHuntersRush            = 247145,
    SpellVoidTrapAOE            = 247175,
    SpellVoidTrapAT             = 245903,
    SpellVoidTrapDmg            = 246026,
    SpellVoidTrapMissile        = 247176,
    SpellOverloadTrap           = 247206,
    SpellOverloadTrapDmg        = 247207,
    SpellUmbralFlankingMark     = 247245,
    SpellUmbralFlanking         = 247246,
    SpellSummonDarkfang         = 243890,
    SpellDarkBond               = 246943,

    /// Darkfang
    SpellRavagingDarknessAOE    = 245800,
    SpellRavagingDarknessTele   = 245801,
    SpellRavagingDarknessDmg    = 245802,
    SpellShadowPounce           = 245741,
    SpellShadowPounceDmg        = 245742,
    SpellShadowPounceOrbs       = 250747,

    /// Shadewing
    SpellSwoop                  = 248830,
    SpellSwoopDmg               = 248829,
    SpellDreadScreech           = 248831,

    /// Sealed Void Cache
    SpellSealedVoidCacheVisual  = 250745,
    SpellDecimate               = 244579,
};

enum eDataSaprish
{
    DataFlankedTargets = 20,
};

/// Saprish - 124872
class boss_saprish : public CreatureScript
{
    public:
        boss_saprish() : CreatureScript("boss_saprish")
        {}

        enum eEvents
        {
            EventVoidTrap   = 1,
            EventUmbralFlanking,
            EventOverloadTraps,
        };

        enum eTalks
        {
            TalkIntro,
            TalkAggro,
            TalkDeath,
            TalkUmbralFlanking,
            TalkKill,
        };

        struct boss_saprish_AI : public BossAI
        {
            explicit boss_saprish_AI(Creature* p_Me) : BossAI(p_Me, eData::DataSaprish)
            {
            }

            void InitializeAI() override
            {
                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    PrepareIntroState();
                });
            }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);

                m_Caches = 0;

                events.ScheduleEvent(eEvents::EventVoidTrap, 8.8 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventOverloadTraps, 12.5 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventUmbralFlanking, 20.4 * IN_MILLISECONDS);
                _EnterCombat();
            }

            void SetData(uint32 p_Id, uint32 p_State) override
            {
                if (p_Id == eCreatures::NpcSealedVoidCache)
                    m_Caches++;
            }

            void ResetCaches()
            {
                std::list<Creature*> l_Caches;

                me->GetCreatureListWithEntryInGrid(l_Caches, eCreatures::NpcSealedVoidCache, 500.f);

                for (Creature* l_Itr : l_Caches)
                    l_Itr->Respawn(true);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    PrepareIntroState();
                });
            }

            void PrepareIntroState()
            {
                if (instance && instance->GetData(eCreatures::BossSaprish) == EncounterState::DONE)
                {
                    me->SetVisible(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    SummonPets();
                }
                else
                {
                    me->SetVisible(false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void EnterEvadeMode() override
            {
                if (IsMythic())
                    ResetCaches();

                BossAI::EnterEvadeMode();
            }

            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDeath);
                CleanUp();
                _JustDied();

                me->CastSpell(me, eSharedSpells::SpellSummonSaprishStaff, true);
                me->DelayedCastSpell(me, eSharedSpells::SpellSummonLocusWalker, true, 1000);

                if (!IsChallengeMode())
                    instance->SendScenarioEventToAllPlayers(eScenarioEvents::SaprishDeath);

                if (IsMythic() && m_Caches >= 4)
                    instance->DoCompleteAchievement(eAchievements::LetItAllOut);
                   
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.15f;
            }

            void Reset() override
            {
                CleanUp();
                _Reset();

                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                m_Caches = 0;

                me->DespawnAreaTriggersInArea(eSpells::SpellVoidTrapAT);
                me->DespawnAreaTriggersInArea(eSpells::SpellDecimate);

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    PrepareIntroState();
                });

                me->CastCustomSpell(eSpells::SpellHuntersRush, SpellValueMod::SPELLVALUE_AURA_STACK, 3, me, true);
            }

            void SummonPets()
            {
                DoCast(me, eSpells::SpellSummonDarkfang, true);

                if (IsMythic())
                {
                    Position l_Pos;
                    me->GetNearPosition(l_Pos, 6.f, me->GetOrientation() - (float(M_PI) / 2.f));

                    me->SummonCreature(eCreatures::NpcShadewing, l_Pos, TEMPSUMMON_MANUAL_DESPAWN);
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_Id) override
            {
                if (p_Id == eDataSaprish::DataFlankedTargets)
                    m_FlankTargets.push(p_Guid);
            }

            uint64 GetGUID(int32 p_Id) override
            {
                uint64 l_Ret = 0;

                if (p_Id == eDataSaprish::DataFlankedTargets)
                {
                    if (!m_FlankTargets.empty())
                    {
                        l_Ret = m_FlankTargets.front();
                        m_FlankTargets.pop();
                        return l_Ret;
                    }
                }

                return l_Ret;
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionSaprishIntro)
                {
                    instance->SetData(eCreatures::BossSaprish, EncounterState::IN_PROGRESS);

                    uint64 l_FirstPortalGUID = instance->GetData64(eData::DataFirstPortalSaprish);

                    if (Creature* l_FirstPortal = sObjectAccessor->GetCreature(*me, l_FirstPortalGUID))
                    {
                        instance->DoCastSpellOnPlayers(eSharedSpells::SpellSaprishWaveOneConv);

                        if (l_FirstPortal->IsAIEnabled)
                            l_FirstPortal->GetAI()->DoAction(eSharedActions::ActionSaprishPortalOpen);
                    }
                }
                else if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    instance->SetData(eCreatures::BossSaprish, EncounterState::DONE);

                    if (Unit* l_BehindPortal = sObjectAccessor->GetUnit(*me, instance->GetData64(eData::DataPortalBehindSaprish)))
                    {
                        l_BehindPortal->CastSpell(l_BehindPortal, eSharedSpells::SpellVoidPortalVisual, true);
                        
                        l_BehindPortal->AddDelayedEvent([l_BehindPortal]() -> void
                        {
                            l_BehindPortal->RemoveAllAuras();
                        }, 6 * IN_MILLISECONDS);
                    }

                    AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this] () -> void
                    {
                        me->SetVisible(true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        SummonPets();
                        DoCast(me, eSpells::SpellSaprishIntroConver, true);
                    });
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon == nullptr)
                    return;

                uint32 l_Entry = p_Summon->GetEntry();

                if (l_Entry == eCreatures::NpcVoidTrap)
                {
                    if (IsChallengeMode())
                        p_Summon->CastSpell(p_Summon, ChallengeSpells::ChallengersMight, true);

                    p_Summon->SetReactState(REACT_PASSIVE);
                    p_Summon->CastSpell(p_Summon, eSpells::SpellVoidTrapAT, true);
                    return;
                }
                else if (l_Entry == eCreatures::NpcShadewing ||
                        l_Entry == eCreatures::NpcDarkfang)
                    p_Summon->CastSpell(p_Summon, eSpells::SpellDarkBond, true);

                BossAI::JustSummoned(p_Summon);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim && p_Victim->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void CleanUp()
            {
                std::list<AreaTrigger*> l_Ats;
                std::list<Creature*> l_Bombs;
                me->GetAreaTriggerListWithSpellIDInRange(l_Ats, eSpells::SpellVoidTrapAT, 500.f);

                for (AreaTrigger* l_At : l_Ats)
                {
                    l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                    l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                    l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                    l_At->SetDuration(500);
                }

                me->GetCreatureListWithEntryInGrid(l_Bombs, eCreatures::NpcVoidTrap, 500.f);

                for (Creature* l_Itr : l_Bombs)
                {
                    if (l_Itr == nullptr)
                        continue;

                    l_Itr->DespawnOrUnsummon(IN_MILLISECONDS);
                }
            }
            
            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventVoidTrap:
                    {
                        DoCastAOE(eSpells::SpellVoidTrapAOE);
                        events.ScheduleEvent(eEvents::EventVoidTrap, 15.8 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventOverloadTraps:
                    {
                        DoCastAOE(eSpells::SpellOverloadTrap, true);
                        events.ScheduleEvent(eEvents::EventOverloadTraps, 21 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventUmbralFlanking:
                    {
                        Talk(eTalks::TalkUmbralFlanking);
                        DoCastAOE(eSpells::SpellUmbralFlankingMark);

                        AddTimedDelayedOperation(5000, [this]() -> void
                        {
                            DoCastAOE(eSpells::SpellUmbralFlanking, true);
                        });

                        events.ScheduleEvent(eEvents::EventUmbralFlanking, 35.2 * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                BossAI::UpdateAI(p_Diff);
            }

            std::queue<uint64> m_FlankTargets;
            uint8 m_Caches;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_saprish_AI(p_Me);
        }
};

/// Darkfang - 122319
class npc_saprish_darkfang : public CreatureScript
{
    public:
        npc_saprish_darkfang() : CreatureScript("npc_saprish_darkfang")
        {}

        enum eEvents
        {
            EventRavagingDarkness = 1,
            EventShadowPounce,
        };

        struct npc_saprish_darkfang_AI : public ScriptedAI
        {
            explicit npc_saprish_darkfang_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
            }

            void JustReachedHome() override
            {
                ScriptedAI::JustReachedHome();
            }

            void ActivateCombatOnSaprish()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (Creature* l_Saprish = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::BossSaprish)))
                {
                    if (!l_Saprish->isInCombat())
                        DoZoneInCombat(l_Saprish);
                }
            }

            Unit* GetPounceTarget()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return nullptr;

                if (Creature* l_Saprish = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::BossSaprish)))
                {
                    if (l_Saprish->IsAIEnabled)
                        return l_Saprish->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true);
                }

                return nullptr;
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.15f;
            }

            void Reset() override
            {
                me->UpdateStatsForLevel();
                me->SetReactState(REACT_AGGRESSIVE);
                me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                events.Reset();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
                ClearDelayedOperations();
            }

            void OnSpellFinished(SpellInfo const* p_Spell) override
            {
                switch (p_Spell->Id)
                {
                    case eSpells::SpellRavagingDarknessDmg:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                        });

                        break;
                    }

                    case eSpells::SpellRavagingDarknessTele:
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                        me->AttackStop();
                        me->StopMoving();
                        break;
                    }
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                ActivateCombatOnSaprish();
                events.ScheduleEvent(eEvents::EventRavagingDarkness, 4 * IN_MILLISECONDS);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventShadowPounce, 10 * IN_MILLISECONDS);

                if (IsChallengeMode())
                    me->CastSpell(me, ChallengeSpells::ChallengersMight, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    if (p_Id == eSpells::SpellShadowPounce)
                        me->CastSpell(me, eSpells::SpellShadowPounceDmg, true);
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::SpellShadowPounceDmg)
                    me->CastSpell(me, eSpells::SpellShadowPounceOrbs, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (!p_Target || !p_Spell)
                    return;

                if (p_Target->GetEntry() == eCreatures::NpcSealedVoidCache && p_Spell->Id == eSpells::SpellShadowPounceOrbs)
                {
                    p_Target->RemoveAllAuras();
                    me->CastSpell(p_Target, eSpells::SpellDecimate, true);

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        Unit* l_Boss = sObjectAccessor->FindUnit(l_Instance->GetData64(eCreatures::BossSaprish));

                        if (l_Boss && l_Boss->IsAIEnabled)
                            l_Boss->GetAI()->SetData(eCreatures::NpcSealedVoidCache, 1);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) ||
                    me->GetReactState() == REACT_PASSIVE)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventRavagingDarkness:
                    {
                        DoCastAOE(eSpells::SpellRavagingDarknessAOE);
                        events.ScheduleEvent(eEvents::EventRavagingDarkness, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventShadowPounce:
                    {
                        if (Unit* l_Target = GetPounceTarget())
                            DoCast(l_Target, eSpells::SpellShadowPounce, true);

                        events.ScheduleEvent(eEvents::EventShadowPounce, 15 * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_saprish_darkfang_AI(p_Me);
        }
};

/// Shadewing - 125340
class npc_saprish_shadewing : public CreatureScript
{
    public:
        npc_saprish_shadewing() : CreatureScript("npc_saprish_shadewing")
        {}

        enum eEvents
        {
            EventSwoop = 1,
            EventDreadScreech,
        };

        struct npc_saprish_shadewing_AI : public ScriptedAI
        {
            explicit npc_saprish_shadewing_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.15f;
            }

            void Reset() override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->UpdateStatsForLevel();
                events.Reset();
            }

            void ActivateCombatOnSaprish()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (Creature* l_Saprish = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::BossSaprish)))
                {
                    if (!l_Saprish->isInCombat())
                        DoZoneInCombat(l_Saprish);
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                ActivateCombatOnSaprish();
                DoZoneInCombat();

                if (IsChallengeMode())
                    me->CastSpell(me, ChallengeSpells::ChallengersMight, true);

                events.ScheduleEvent(eEvents::EventSwoop, 5 * IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellSwoopDmg)
                    events.ScheduleEvent(eEvents::EventDreadScreech, 100);
                else if (p_Spell->Id == eSpells::SpellDreadScreech)
                {
                    AddTimedDelayedOperation(100, [this]() -> void
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->ClearUnitState(UNIT_STATE_ROOT);
                    });
                }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventSwoop:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellSwoop);

                        events.ScheduleEvent(eEvents::EventSwoop, 15 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDreadScreech:
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->AddUnitState(UNIT_STATE_ROOT);
                        me->AttackStop();
                        me->StopMoving();

                        AddTimedDelayedOperation(3200, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->ClearUnitState(UNIT_STATE_ROOT);
                        });

                        DoCastAOE(eSpells::SpellDreadScreech);
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
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_saprish_shadewing_AI(p_Me);
        }
};

/// Overload Trap - 247206
class spell_saprish_overload_trap : public SpellScriptLoader
{
    public:
        spell_saprish_overload_trap() : SpellScriptLoader("spell_saprish_overload_trap")
        {}

        class spell_saprish_overload_trap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_overload_trap_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject* const & p_Itr) -> bool
                {
                    return p_Itr->GetEntry() != eCreatures::NpcVoidTrap;
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                Unit* l_Target = GetHitUnit();

                l_Target->CastSpell(l_Target, eSpells::SpellOverloadTrapDmg, false);
                
                if (Creature* l_Creature = l_Target->ToCreature())
                {
                    std::list<AreaTrigger*> l_Bombs;
                    l_Target->GetAreaTriggerList(l_Bombs, eSpells::SpellVoidTrapAT);

                    for (AreaTrigger* l_At : l_Bombs)
                    {
                        l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                        l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                        l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                        l_At->SetDuration(500);
                    }

                    l_Creature->DespawnOrUnsummon(2200);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_saprish_overload_trap_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_saprish_overload_trap_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_overload_trap_SpellScript();
        }
};

/// Ravaging Darkness - 245803
class spell_saprish_ravaging_darkness : public SpellScriptLoader
{
    public:
        spell_saprish_ravaging_darkness() : SpellScriptLoader("spell_saprish_ravaging_darkness")
        {}

        class spell_saprish_ravaging_darkness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_ravaging_darkness_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                Unit* l_Boss = sObjectAccessor->GetUnit(*l_Caster, l_Instance->GetData64(eCreatures::BossSaprish));

                if (l_Boss != nullptr)
                    l_Boss->CastSpell(l_Boss, eSpells::SpellHuntersRush, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_saprish_ravaging_darkness_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_ravaging_darkness_SpellScript();
        }
};

/// Swoop - 248830
class spell_saprish_swoop : public SpellScriptLoader
{
    public:
        spell_saprish_swoop() :SpellScriptLoader("spell_saprish_swoop")
        {}

        enum eSpells
        {
            SpellSwoopDmg   = 248829,
        };

        class spell_saprish_swoop_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_swoop_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                GetCaster()->CastSpell(GetCaster(), eSpells::SpellSwoopDmg, true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_saprish_swoop_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_JUMP_DEST_2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_swoop_SpellScript();
        }
};

/// Umbral Flanking Mark - 247245
class spell_saprish_umbral_flaking_aoe : public SpellScriptLoader
{
    public:
        spell_saprish_umbral_flaking_aoe() : SpellScriptLoader("spell_saprish_umbral_flaking_aoe")
        {}

        class spell_saprish_umbral_flaking_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_umbral_flaking_aoe_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster->IsAIEnabled)
                    l_Caster->GetAI()->SetGUID(l_Target->GetGUID(), eDataSaprish::DataFlankedTargets);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_saprish_umbral_flaking_aoe_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_umbral_flaking_aoe_SpellScript();
        }
};

/// Umbral Flanking - 247246
class spell_saprish_umbral_flaking : public SpellScriptLoader
{
    public:
        spell_saprish_umbral_flaking() : SpellScriptLoader("spell_saprish_umbral_flaking")
        {}

        class spell_saprish_umbral_flaking_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_umbral_flaking_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster() == nullptr)
                    return;

                p_Targets.clear();

                Unit* l_Caster = GetCaster();

                std::list<HostileReference*> const & l_Targets = l_Caster->getThreatManager().getThreatList();
                std::list<Unit*> l_MarkedTargets;

                for (HostileReference* l_Itr : l_Targets)
                {
                    Unit* l_Target = l_Itr->getTarget();

                    if (l_Target == nullptr || !l_Target->IsPlayer())
                        continue;

                    if (l_Target->HasAura(eSpells::SpellUmbralFlankingMark))
                        l_MarkedTargets.push_back(l_Target);

                    p_Targets.push_back(l_Target);
                }


                if (l_MarkedTargets.empty())
                {
                    p_Targets.clear();
                    return;
                }

                Unit* l_MarkedTarget = JadeCore::Containers::SelectRandomContainerElement(l_MarkedTargets);
                m_DestTarget = l_MarkedTarget->GetGUID();

                p_Targets.remove_if([this, &l_Caster, &l_MarkedTarget] (WorldObject*& p_Itr)
                {
                    return !p_Itr->IsInAxe(l_Caster, l_MarkedTarget, 5.f);
                });

            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                    return;

                GetCaster()->CastSpell(GetCaster(), eSpells::SpellHuntersRush, true);
            }

            void HandleOnDest(WorldObject*& p_Target)
            {
                if (GetCaster() == nullptr)
                    return;

                p_Target = sObjectAccessor->GetUnit(*GetCaster(), m_DestTarget);

                if (p_Target != nullptr)
                    SetExplTargetDest(*p_Target);
            }

            void HandleAfterCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                if (Unit* l_Marked = sObjectAccessor->GetUnit(*GetCaster(), m_DestTarget))
                    l_Marked->RemoveAurasDueToSpell(eSpells::SpellUmbralFlankingMark);

                if (l_Caster->IsAIEnabled)
                {
                    uint64 l_NextGUID = l_Caster->GetAI()->GetGUID(eDataSaprish::DataFlankedTargets);

                    if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_NextGUID))
                        l_Caster->DelayedCastSpell(l_Target, eSpells::SpellUmbralFlanking, true, 500);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_saprish_umbral_flaking_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_saprish_umbral_flaking_SpellScript::HandleOnDest, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_saprish_umbral_flaking_SpellScript::HandleOnHit, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_saprish_umbral_flaking_SpellScript::HandleAfterCast);
            }

            uint64 m_DestTarget;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_umbral_flaking_SpellScript();
        }
};

/// Void Traps AOE - 247175
class spell_saprish_void_trap_aoe : public SpellScriptLoader
{
    public:
        spell_saprish_void_trap_aoe() : SpellScriptLoader("spell_saprish_void_trap_aoe")
        {}

        class spell_saprish_void_trap_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_void_trap_aoe_SpellScript);

            void HandleAfterCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Position l_Pos;

                for (uint8 l_Itr = 0; l_Itr < 5; ++l_Itr)
                {
                    l_Caster->GetRandomNearPosition(l_Pos, 15.f);
                    l_Caster->CastSpell(l_Pos, eSpells::SpellVoidTrapMissile, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_saprish_void_trap_aoe_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_void_trap_aoe_SpellScript();
        }
};

/// Hunter's Rush - 247195
class spell_saprish_hunter_rush : public SpellScriptLoader
{
    public:
        spell_saprish_hunter_rush() : SpellScriptLoader("spell_saprish_hunter_rush")
        {}

        class spell_saprish_hunter_rush_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_saprish_hunter_rush_AuraScript);

            void HandleAfterEffectProc(AuraEffect const* /**/, ProcEventInfo& /**/)
            {
                if (!GetAura())
                    return;

                ModStackAmount(-1);
            }

            void Register() override
            {
                AfterEffectProc += AuraEffectProcFn(spell_saprish_hunter_rush_AuraScript::HandleAfterEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript()  const override
        {
            return new spell_saprish_hunter_rush_AuraScript();
        }
};

/// Void Trap - 246026
class spell_saprish_void_trap_dmg : public SpellScriptLoader
{
    public:
        spell_saprish_void_trap_dmg() : SpellScriptLoader("spell_saprish_void_trap_dmg")
        {}

        class spell_saprish_void_trap_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saprish_void_trap_dmg_SpellScript);

            void HandleOnDmg(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr ||
                    GetCaster() == nullptr ||
                    GetHitDamage() <= 0)
                    return;

                int32 l_Dmg = GetHitDamage();

                InstanceScript* l_Instance = GetCaster()->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (!l_Instance->IsChallenge())
                    return;

                if (Unit* l_Saprish = sObjectAccessor->FindUnit(l_Instance->GetData64(eCreatures::BossSaprish)))
                {
                    if (AuraEffect* l_Might = l_Saprish->GetAuraEffect(ChallengeSpells::ChallengersMight, EFFECT_1))
                        AddPct(l_Dmg, l_Might->GetAmount());
                }

                SetHitDamage(l_Dmg);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_saprish_void_trap_dmg_SpellScript::HandleOnDmg, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_saprish_void_trap_dmg_SpellScript;
        }
};

/// Void Trap - 245903
class at_saprish_void_trap : public AreaTriggerEntityScript
{
    public:
        at_saprish_void_trap() : AreaTriggerEntityScript("at_saprish_void_trap")
        {
            m_IsArmed   = false;
            m_SpawnTime = 0;
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_At == nullptr || p_Target == nullptr)
                return true;

            if (!m_IsArmed)
                return true;

            DespawnAreatrigger(p_At);

            return false;
        }

        void DespawnAreatrigger(AreaTrigger* p_At)
        {
            if (Unit* l_Caster = p_At->GetCaster())
            {
                if (l_Caster->ToCreature())
                    l_Caster->ToCreature()->DespawnOrUnsummon(100);
            }

            p_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
            p_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
            p_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
            p_At->SetDuration(500);
        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            if (p_At == nullptr || p_At->GetCaster() == nullptr)
                return;

            if (m_IsArmed)
                return;

            m_SpawnTime += p_Diff;

            if (m_SpawnTime >= 2 * IN_MILLISECONDS)
            {
                m_IsArmed = true;

                GuidList* l_Targets = p_At->GetAffectedPlayers();

                if (l_Targets)
                {
                    for (uint64 l_Itr : *l_Targets)
                    {
                        Player* l_Target = sObjectAccessor->FindPlayer(l_Itr);

                        if (l_Target)
                        {
                            Unit* l_Caster = p_At->GetCaster();
                            l_Caster->CastSpell(l_Caster, eSpells::SpellVoidTrapDmg, true);
                            DespawnAreatrigger(p_At);
                            return;
                        }
                    }
                    
                }

            }
        }
        
        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_saprish_void_trap();
        }

    private:
        bool m_IsArmed;
        uint32 m_SpawnTime;
};

#ifndef __clang_analyzer__
void AddSC_boss_saprish()
{
    /// Creatures
    new boss_saprish();
    new npc_saprish_darkfang();
    new npc_saprish_shadewing();

    /// Spells
    new spell_saprish_void_trap_aoe();
    new spell_saprish_overload_trap();
    new spell_saprish_umbral_flaking_aoe();
    new spell_saprish_umbral_flaking();
    new spell_saprish_ravaging_darkness();
    new spell_saprish_swoop();
    new spell_saprish_hunter_rush();
    new spell_saprish_void_trap_dmg();

    /// Areatriggers
    new at_saprish_void_trap();
}
#endif
