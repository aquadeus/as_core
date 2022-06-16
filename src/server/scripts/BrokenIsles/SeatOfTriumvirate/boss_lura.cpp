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
    /// L'ura
    SpellCallToTheVoid              = 247795,
    SpellDarkenedShroud             = 254020,
    SpellVoidBlast                  = 245289,
    SpellUmbralCadence              = 247930,
    SpellBacklash                   = 247816,
    SpellGrandShift                 = 249009,
    SpellSorrowShock                = 247948,
    SpellNaruuLamentChannel         = 245393,
    SpellNaruuLament                = 248535,

    /// Greater Rift Warden
    SpellVoidBound                  = 247835,
    SpellFragmentOfDespairAOE       = 245164,
    SpellFragmentOfDespairMissile   = 245165,
    SpellEruptingDespair            = 245178,
    SpellFragmentCrash              = 245187,

    /// Waning Void
    SpellDarkLashing                = 254727,
    SpellRemnantOfAnguishTrigger    = 249057,
    SpellRemnantOfAnguish           = 245244,
    SpellRemnantOfAnguishAT         = 245241,

    /// Void Portal
    SpellVoidPortalVisualLura       = 247840,
    SpellWaningVoidSummon           = 247909,
    SpellSummonGreaterRiftWarden    = 247831,
    SpellFromTheVoidSummon          = 245236,
};

enum eEvents
{
    /// Lura
    EventCallToTheVoid = 1,
    EventVoidBlast,
    EventUmbralCadence,
    EventGrandShift,

    /// Greater Rift Warden
    EventFragmentOfDespair,
    EventSorrowShock,
};

/// L'ura - 124870
class boss_lura : public CreatureScript
{
    public:
        boss_lura() : CreatureScript("boss_lura")
        {}

        struct boss_lura_AI : public BossAI
        {
            explicit boss_lura_AI(Creature* p_Me) : BossAI(p_Me, eData::DataLura)
            {
                CheckIntro();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.68f;
            }

            void Reset() override
            {
                _Reset();
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                SetCombatMovement(false);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                CheckIntro();
            }

            void CheckIntro()
            {
                if (instance == nullptr)
                    return;

                if (instance->GetData(me->GetEntry()) == EncounterState::DONE)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                else
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            }

            void EnterCombat(Unit* /**/) override
            {
                _EnterCombat();

                m_BacklashCount = 0;
                events.ScheduleEvent(eEvents::EventCallToTheVoid, 500);
            }

            void OnRemoveAura(uint32 p_SpellId, AuraRemoveMode p_RemoveMode) override
            {
                if (p_SpellId == eSpells::SpellBacklash &&
                    p_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                {
                    if (m_BacklashCount < 2)
                        events.ScheduleEvent(eEvents::EventCallToTheVoid, 500);
                    else
                        StartPhaseTwo();
                }
            }

            void SpellHit(Unit* /**/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSharedSpells::SpellDarkTorrent)
                {
                    m_BacklashCount++;
                    me->CastSpell(me, eSpells::SpellBacklash, true);
                    me->RemoveAura(eSpells::SpellCallToTheVoid);

                    if (AreaTrigger* l_At = me->GetAreaTrigger(eSpells::SpellDarkenedShroud))
                    {
                        l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                        l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                        l_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                        l_At->SetDuration(500);
                    }

                    me->RemoveAura(eSpells::SpellDarkenedShroud);
                }
            }

            void StartPhaseTwo()
            {
                SetCombatMovement(true);

                me->CastSpell(me, eSpells::SpellNaruuLament, true);

                events.ScheduleEvent(eEvents::EventUmbralCadence, 2 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventVoidBlast, urand(10, 15) * IN_MILLISECONDS);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventGrandShift, 8 * IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellNaruuLamentChannel)
                    me->CastSpell(me, eSpells::SpellDarkenedShroud, true);
                else if (p_Spell->Id == eSpells::SpellCallToTheVoid)
                {
                    if (instance == nullptr)
                        return;

                    Unit* l_Alleria = sObjectAccessor->GetUnit(*me, instance->GetData64(eCreatures::NpcAlleriaLura));

                    if (l_Alleria && l_Alleria->IsAIEnabled)
                        l_Alleria->GetAI()->DoAction(eSharedActions::ActionCallOfTheVoid);
                }
            }

            void CleanUp()
            {
                std::list<Creature*> l_Portals;

                me->GetCreatureListWithEntryInGrid(l_Portals, eCreatures::NpcVoidPortalDummy, 150.f);

                for (Creature* l_Itr : l_Portals)
                {
                    if (l_Itr == nullptr)
                        continue;

                    l_Itr->RemoveAllAuras();
                    l_Itr->SetVisible(false);
                }

                me->DespawnAreaTriggersInArea(eSpells::SpellDarkenedShroud, 150.f);
                me->DespawnCreaturesInArea(eCreatures::NpcRemnantOfAnguish, 250.f);

                if (Unit* l_Alleria = sObjectAccessor->GetUnit(*me, instance->GetData64(eCreatures::NpcAlleriaLura)))
                {
                    if (l_Alleria->IsAIEnabled)
                        l_Alleria->GetAI()->Reset();
                }
            }

            void EnterEvadeMode() override
            {
                CleanUp();
                BossAI::EnterEvadeMode();
            }

            uint32 GetData(uint32 p_Id) override
            {
                if (p_Id == eCreatures::NpcVoidPortalDummy)
                    return m_BacklashCount;

                return 0;
            }

            void JustDied(Unit* /**/) override
            {
                DoCastAOE(eSharedSpells::SpellAlleriaMovie, true);

                if (IsMythic() && instance)
                {
                    auto check = false;

                    instance->instance->ApplyOnEveryPlayer([&check](Player* player) {
                        
                        if (!player->HasAura(SpellCrushingDarkness))
                            check = false;

                    });

                    if (check)
                        instance->DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, 250822);
                }

                if (!IsChallengeMode() && instance)
                    instance->SendScenarioEventToAllPlayers(eScenarioEvents::LuraDeath);

                _JustDied();

                CleanUp();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    if (instance)
                        instance->SetData(me->GetEntry(), EncounterState::DONE);

                    AddTimedDelayedOperation(38 * IN_MILLISECONDS, [this] () -> void
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    });

                    std::list<Creature*> m_VoidPortalsTrash;

                    me->GetCreatureListWithEntryInGrid(m_VoidPortalsTrash, eCreatures::NpcVoidPortalTrash, 150.f);

                    for (Creature* l_Itr : m_VoidPortalsTrash)
                    {
                        if (l_Itr == nullptr)
                            continue;

                        l_Itr->RemoveAllAuras();
                        l_Itr->SetVisible(false);
                    }
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventCallToTheVoid:
                    {
                        DoCast(me, eSpells::SpellCallToTheVoid);
                        break;
                    }

                    case eEvents::EventVoidBlast:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellVoidBlast);

                        events.ScheduleEvent(eEvents::EventVoidBlast, urand(10, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventUmbralCadence:
                    {
                        DoCastAOE(eSpells::SpellUmbralCadence);
                        events.ScheduleEvent(eEvents::EventUmbralCadence, 11 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGrandShift:
                    {
                        DoCastAOE(eSpells::SpellGrandShift);
                        events.ScheduleEvent(eEvents::EventGrandShift, 15 * IN_MILLISECONDS);
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

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }

            uint8 m_BacklashCount;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_lura_AI(p_Me);
        }
};

/// Greater Rift Warden - 124745
class npc_lura_greater_rift_warden : public CreatureScript
{
    public:
        npc_lura_greater_rift_warden() : CreatureScript("npc_lura_greater_rift_warden")
        {}

        enum eTalks
        {
            WarnSpellFragmentOfDespair = 0
        };

        struct npc_lura_greater_rift_warden_AI : public ScriptedAI
        {
            explicit npc_lura_greater_rift_warden_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_PortalGuid = 0;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner == nullptr)
                    return;

                m_PortalGuid = p_Summoner->GetGUID();

                p_Summoner->CastSpell(me, eSpells::SpellVoidBound, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Boss = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::BossLura)))
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->AI()->JustSummoned(me);
                    }

                    if (Unit* l_Alleria = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::NpcAlleriaLura)))
                    {
                        if (l_Alleria->IsAIEnabled)
                            l_Alleria->GetAI()->SetGUID(me->GetGUID(), eCreatures::NpcGreatRiftWarden);
                    }
                }

            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventSorrowShock, urand(10, 20) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFragmentOfDespair, 10 * IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (p_Target == nullptr ||
                    p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellFragmentOfDespairAOE)
                {
                    p_Target->CastSpell(p_Target, eSpells::SpellFragmentCrash, true);
                    me->CastSpell(p_Target, eSpells::SpellFragmentOfDespairMissile, true);
                }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFragmentOfDespair:
                    {
                        Talk(eTalks::WarnSpellFragmentOfDespair);
                        DoCastAOE(eSpells::SpellFragmentOfDespairAOE);
                        events.ScheduleEvent(eEvents::EventFragmentOfDespair, urand(15, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSorrowShock:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellSorrowShock);

                        events.ScheduleEvent(eEvents::EventSorrowShock, urand(8, 12) * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            uint64 GetGUID(int32 p_Id) override
            {
                if (p_Id == eCreatures::NpcVoidPortalDummy)
                    return m_PortalGuid;

                return 0;
            }

            void JustDied(Unit* /**/) override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (Creature* l_Walker = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::NpcLocusWalkerLura)))
                {
                    if (l_Walker->IsAIEnabled)
                        l_Walker->GetAI()->DoAction(eSharedActions::ActionVoidPortal);
                }

                if (Creature* l_Alleria = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::NpcAlleriaLura)))
                {
                }
            }

            uint64 m_PortalGuid;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_lura_greater_rift_warden_AI(p_Me);
        }
};

///< Remnant of Anguish - 123054
class npc_lura_remnant_of_anguish : public CreatureScript
{
    public:
        npc_lura_remnant_of_anguish() : CreatureScript("npc_lura_remnant_of_anguish")
        {}

        struct npc_lura_remnant_of_anguish_AI : public ScriptedAI
        {
            explicit npc_lura_remnant_of_anguish_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon == nullptr)
                    return;

                if (p_Summon->GetEntry() == me->GetEntry())
                {
                    me->SetVisible(false);
                    me->DespawnOrUnsummon(IN_MILLISECONDS * 2);
                }
            }

            void Reset() override
            {
                me->CastSpell(me, eSpells::SpellRemnantOfAnguishAT, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_lura_remnant_of_anguish_AI(p_Me);
        }
};

/// Waning Void - 123050
class npc_lura_waning_void : public CreatureScript
{
    public:
        npc_lura_waning_void() : CreatureScript("npc_lura_waning_void")
        {}

        struct npc_lura_waning_void_AI : public ScriptedAI
        {
            explicit npc_lura_waning_void_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                me->CastSpell(me, eSpells::SpellDarkLashing, true);
            }

            void IsSummonedBy(Unit* /**/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Boss = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::BossLura)))
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->AI()->JustSummoned(me);
                    }
                }
            }

            void JustDied(Unit* /**/) override
            {
                me->CastSpell(me, eSpells::SpellRemnantOfAnguish, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_lura_waning_void_AI(p_Me);
        }
};

/// Call to the Void - 247795
class spell_lura_call_to_void : public SpellScriptLoader
{
    public:
        spell_lura_call_to_void() : SpellScriptLoader("spell_lura_call_to_void")
        {}

        class spell_lura_call_to_void_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lura_call_to_void_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Map* l_Map = l_Caster->GetMap();

                if (l_Map == nullptr)
                    return;

                std::list<Creature*> l_Portals;

                l_Caster->GetCreatureListWithEntryInGrid(l_Portals, eCreatures::NpcVoidPortalDummy, 100.0f);

                if (l_Portals.empty())
                    return;

                if (l_Caster->IsAIEnabled)
                {
                    uint32 l_BothSides = l_Caster->GetAI()->GetData(eCreatures::NpcVoidPortalDummy);

                    if (l_BothSides > 0)
                    {
                        for (Creature* l_Portal : l_Portals)
                        {
                            l_Portal->SetVisible(true);
                            l_Portal->CastSpell(l_Portal, eSpells::SpellVoidPortalVisualLura, true);
                            l_Portal->DelayedCastSpell(l_Portal, eSpells::SpellSummonGreaterRiftWarden, true, 500);
                            l_Portal->DelayedCastSpell(l_Portal, eSpells::SpellWaningVoidSummon, true, 1500);
                        }
                    }
                    else
                    {
                        Creature* l_Portal = JadeCore::Containers::SelectRandomContainerElement(l_Portals);

                        if (l_Portal != nullptr)
                        {
                            l_Portal->SetVisible(true);
                            l_Portal->CastSpell(l_Portal, eSpells::SpellVoidPortalVisualLura, true);
                            l_Portal->DelayedCastSpell(l_Portal, eSpells::SpellSummonGreaterRiftWarden, true, 500);
                            l_Portal->DelayedCastSpell(l_Portal, eSpells::SpellWaningVoidSummon, true, 1500);
                        }
                    }

                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_lura_call_to_void_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lura_call_to_void_SpellScript();
        }
};

/// Summon Waning Voids - 247909
class spell_lura_summon_waning_voids : public SpellScriptLoader
{
    public:
        spell_lura_summon_waning_voids() : SpellScriptLoader("spell_lura_summon_waning_voids")
        {}

        class spell_lura_summon_waning_voids_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_lura_summon_waning_voids_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                uint8 l_Adds = urand(2, 3);

                Position l_Pos;

                for (uint8 l_Itr = 0; l_Itr < l_Adds; ++l_Itr)
                    l_Owner->CastSpell(l_Owner, eSpells::SpellFromTheVoidSummon, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_lura_summon_waning_voids_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_lura_summon_waning_voids_AuraScript();
        }
};

/// Void Sever - 247878
class spell_lura_void_sever : public SpellScriptLoader
{
    public:
        spell_lura_void_sever() : SpellScriptLoader("spell_lura_void_sever")
        {}

        class spell_lura_void_sever_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_lura_void_sever_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr ||
                    GetCaster() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();

                if (AuraApplication const* l_App = GetTargetApplication())
                {
                    if (l_App->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    {
                        l_Owner->RemoveAurasDueToSpell(eSpells::SpellVoidPortalVisualLura);
                        l_Owner->RemoveAurasDueToSpell(eSpells::SpellWaningVoidSummon);
                        l_Owner->SetVisible(false);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_lura_void_sever_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_lura_void_sever_AuraScript();
        }
};

/// Grand Shift - 249009
class spell_lura_grand_shift : public SpellScriptLoader
{
    public:
        spell_lura_grand_shift() : SpellScriptLoader("spell_lura_grand_shift")
        {}

        class spell_lura_grand_shift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lura_grand_shift_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || GetCaster() == nullptr)
                    return;

                p_Targets.remove_if([](WorldObject* p_Itr) -> bool
                {
                    return !p_Itr->IsPlayer();
                });

                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();

                std::list<Creature*> l_Remnants;

                l_Caster->GetCreatureListWithEntryInGrid(l_Remnants, eCreatures::NpcRemnantOfAnguish, 250.f);

                for (Creature* l_Itr : l_Remnants)
                {
                    if (l_Itr == nullptr)
                        continue;

                    WorldObject* l_Target = JadeCore::Containers::SelectRandomContainerElement(p_Targets);

                    l_Itr->RemoveAllAuras();
                    l_Itr->CastSpell(l_Target->ToUnit(), eSpells::SpellRemnantOfAnguishTrigger, true);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lura_grand_shift_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            size_t m_TargetsSize;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lura_grand_shift_SpellScript();
        }
};

/// Remnant of Anguish - 249058
class spell_lura_remnant_of_anguish : public SpellScriptLoader
{
    public:
        spell_lura_remnant_of_anguish() : SpellScriptLoader("spell_lura_remnant_of_anguish")
        {}

        class spell_lura_remnant_of_anguish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lura_remnant_of_anguish_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([](WorldObject* p_Itr)
                {
                    return !p_Itr->IsPlayer();
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lura_remnant_of_anguish_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lura_remnant_of_anguish_SpellScript::HandleTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lura_remnant_of_anguish_SpellScript();
        }
};

/// Fragment of Despair - 245177
class spell_lura_fragment_of_despair : public SpellScriptLoader
{
    public:
        spell_lura_fragment_of_despair() : SpellScriptLoader("spell_lura_fragment_of_despair")
        {}

        class spell_lura_fragment_of_despair_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lura_fragment_of_despair_SpellScript);

            bool Load() override
            {
                m_HasTargets = false;
                return true;
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([] (WorldObject* p_Itr) -> bool
                {
                    return !p_Itr->IsPlayer();
                });

                m_HasTargets = !p_Targets.empty();
            }

            void HandleAfterCast()
            {
                if (!m_HasTargets)
                {
                    Unit* l_Caster = GetCaster();

                    if (l_Caster != nullptr)
                        l_Caster->CastSpell(l_Caster, eSpells::SpellEruptingDespair, true);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lura_fragment_of_despair_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                AfterCast += SpellCastFn(spell_lura_fragment_of_despair_SpellScript::HandleAfterCast);
            }

            bool m_HasTargets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lura_fragment_of_despair_SpellScript();
        }
};

/// Check for Unbound Rifts/Cast Dark Torrent - 247870
class spell_lura_check_for_unbound_rifts_cast_dark_torrent : public AuraScript
{
    PrepareAuraScript(spell_lura_check_for_unbound_rifts_cast_dark_torrent);

    void HandleAfterEffPeriodic(AuraEffect const* p_AurEff)
    {
        Unit* l_Owner = GetUnitOwner();

        if (!l_Owner)
            return;

        std::list<Creature*> l_Portals;

        l_Owner->GetCreatureListWithEntryInGrid(l_Portals, eCreatures::NpcVoidPortalDummy, 50.0f);

        bool l_AllPortalsClosed = true;

        for (Creature* l_Itr : l_Portals)
        {
            if (!l_Itr)
                continue;

            if (!l_Itr->HasAura(eSpells::SpellVoidPortalVisualLura))
                continue;

            if (l_Itr->HasAura(eSpells::SpellVoidPortalVisualLura))
                l_AllPortalsClosed = false;

            if (!l_Itr->HasUnitState(UNIT_STATE_CASTING) && !l_Owner->HasUnitState(UNIT_STATE_CASTING))
                l_Owner->DelayedCastSpell(l_Itr, eSharedSpells::SpellVoidSever, false, IN_MILLISECONDS);
        }

        if (l_AllPortalsClosed && l_Owner->IsAIEnabled)
        {
            l_Owner->GetAI()->DoAction(eSharedActions::ActionVoidPortal);
            Remove();
        }
    }

    void Register() override
    {
        AfterEffectPeriodic += AuraEffectPeriodicFn(spell_lura_check_for_unbound_rifts_cast_dark_torrent::HandleAfterEffPeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_lura()
{
    /// Creatures
    new boss_lura();
    new npc_lura_greater_rift_warden();
    new npc_lura_waning_void();
    new npc_lura_remnant_of_anguish();

    /// Spells
    new spell_lura_call_to_void();
    new spell_lura_void_sever();
    new spell_lura_fragment_of_despair();
    new spell_lura_summon_waning_voids();
    new spell_lura_grand_shift();
    new spell_lura_remnant_of_anguish();
    RegisterAuraScript(spell_lura_check_for_unbound_rifts_cast_dark_torrent);}
#endif
