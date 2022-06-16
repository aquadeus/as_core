////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "neltharions_lair.hpp"

enum Says
{
    SAY_AGGRO,
    SAY_EMOTE,
    SAY_DEATH,
    SAY_CRYSTAL_SPIKE
};

enum Spells
{
    SPELL_GAIN_ENERGY               = 201444,
    SPELL_CONVERSATION_01           = 201665, ///< SPELL_MAGMA_SCULPTOR
    SPELL_CONVERSATION_02           = 201666, ///< SPELL_MAGMA_SCULPTOR
    SPELL_MAGMA_SCULPTOR            = 200637,
    SPELL_LANDSLIDE                 = 200700,
    SPELL_LANDSLIDE_MISSILE         = 200722,
    SPELL_LANDSLIDE_DAMAGE          = 200721,
    SPELL_CRYSTAL_WALL              = 200551,
    SPELL_MAGMA_WAVE                = 200404,
    SPELL_MAGMA_WAVE_AT             = 200418,
    SPELL_MAGMA_WAVE_DUMMY          = 209947,
    SPELL_MAGMA_WAVE_TICK           = 217090,
    SPELL_MOLTEN_CRASH_VISUAL       = 217353,
    SPELL_MOLTEN_CRASH              = 200732,

    /// Heroic
    SPELL_MAGMA_BREAKER_DARGRUL     = 216368,
    SPELL_LAVA_GEYSER_SUM           = 216373,
    SPELL_LAVA_GEYSER_AT            = 216385,
    SPELL_FLAME_GOUT                = 216374,

    SPELL_CONVERSATION_03           = 201663,
    SPELL_CONVERSATION_04           = 201659,
    SPELL_CONVERSATION_05           = 201661, ///< SPELL_MAGMA_WAVE

    SPELL_SUM_PILLAR_CREATION       = 213589,

    /// Trash
    SPELL_FIXATE_PLR                = 200154,
    SPELL_COLLISION_AT              = 188197,
    SPELL_MAGMA_BREAKER             = 209926,
    SPELL_MAGMA_BREAKER_AURA        = 209920,
    SPELL_RISING_INFERNO            = 216369,

    SPELL_CRYSTAL_BLOCKER_AT        = 200367,
    SPELL_CRYSTAL_WALL_SUM_GO       = 209964, ///< GO DOOR

    SPELL_CRYSTAL_CRACKED           = 200672,

    SPELL_BONUS_ROLL                = 226643
};

enum eEvents
{
    EVENT_MAGMA_SCULPTOR    = 1,
    EVENT_LANDSLIDE         = 2,
    EVENT_CRYSTAL_WALL      = 3,
    EVENT_MOLTEN_CRASH      = 4,
    /// Heroic
    EVENT_LAVA_GEYSER       = 5
};

/// Dargrul <The Underking> - 91007
class boss_dargrul_the_underking : public CreatureScript
{
    public:
        boss_dargrul_the_underking() : CreatureScript("boss_dargrul_the_underking") { }

        struct boss_dargrul_the_underkingAI : public BossAI
        {
            boss_dargrul_the_underkingAI(Creature* p_Creature) : BossAI(p_Creature, DATA_DARGRUL)
            {
                me->SetMaxPower(POWER_MANA, 60);

                m_EbonhornLeaved = false;
            }

            std::set<uint64> m_AlreadyHitTarget;

            bool m_EbonhornLeaved;

            void Reset() override
            {
                _Reset();
                me->RemoveAurasDueToSpell(SPELL_GAIN_ENERGY);
                me->SetPower(POWER_MANA, 0);
                me->SetMaxPower(POWER_MANA, 60);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            bool CanBeTargetedOutOfLOS() override
            {
                /// Needed for Crystal Spikes
                return true;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                CHECK_REQUIRED_BOSSES_AND_TELEPORT(DATA_DARGRUL, g_StartPos);

                Talk(SAY_AGGRO);
                _EnterCombat();
                DoCast(me, SPELL_GAIN_ENERGY, true);

                if (IsHeroicOrMythic())
                    DoCast(me, SPELL_MAGMA_BREAKER_DARGRUL, true);

                events.ScheduleEvent(EVENT_MAGMA_SCULPTOR, 7000);
                events.ScheduleEvent(EVENT_LANDSLIDE, 16000);
                events.ScheduleEvent(EVENT_CRYSTAL_WALL, 22000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Navarrogg = Creature::GetCreature(*me, l_Instance->GetData64(NPC_NAVAROGG)))
                    {
                        if (l_Navarrogg->IsAIEnabled)
                            l_Navarrogg->AI()->DoAction(6);
                    }

                    if (Creature* l_Ebonhorn = Creature::GetCreature(*me, l_Instance->GetData64(NPC_SPIRITWALKER_EBONHORN)))
                    {
                        if (l_Ebonhorn->IsAIEnabled)
                            l_Ebonhorn->AI()->DoAction(5);
                    }
                }

                Position l_Pos      = *me;
                l_Pos.m_positionZ   += 2.0f;

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 0, 0);
                me->CastSpell(l_Pos, SPELL_SUM_PILLAR_CREATION, true);

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
            }

            void JustSummonedGO(GameObject* p_GameObject) override
            {
                p_GameObject->SetUInt32Value(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, 0);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_EbonhornLeaved || !p_Who->IsPlayer() || p_Who->GetDistance(me) > 65.0f)
                    return;

                m_EbonhornLeaved = true;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Ebonhorn = Creature::GetCreature(*me, l_Instance->GetData64(NPC_SPIRITWALKER_EBONHORN)))
                    {
                        if (l_Ebonhorn->IsAIEnabled)
                            l_Ebonhorn->AI()->DoAction(4);
                    }
                }
            }

            void RegeneratePower(Powers /*p_Power*/, int32& p_Value) override
            {
                p_Value = 0;
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case SPELL_LANDSLIDE:
                    {
                        Position l_Pos;
                        float l_Angle;

                        for (uint8 l_I = 0; l_I < 100; l_I++)
                        {
                            l_Angle = frand(-0.17f, 0.17f);

                            float l_X = me->m_positionX + float(l_I) * std::cos(l_Angle + me->m_orientation);
                            float l_Y = me->m_positionY + float(l_I) * std::sin(l_Angle + me->m_orientation);
                            float l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, me->m_positionZ);

                            l_Pos = { l_X, l_Y, l_Z, l_Angle + me->m_orientation };

                            me->CastSpell(l_Pos, SPELL_LANDSLIDE_MISSILE, true);
                        }

                        break;
                    }
                    case SPELL_LANDSLIDE_DAMAGE:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        if (m_AlreadyHitTarget.find(p_Target->GetGUIDLow()) != m_AlreadyHitTarget.end())
                            break;

                        m_AlreadyHitTarget.insert(p_Target->GetGUIDLow());
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID /*= 0*/) override
            {
                return m_AlreadyHitTarget.find(p_ID) != m_AlreadyHitTarget.end();
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                me->SetPower(POWER_MANA, 0);

                Talk(SAY_EMOTE);

                AddTimedDelayedOperation(500, [this]() -> void
                {
                    DoCast(me, SPELL_MAGMA_WAVE_TICK, true);
                });

                AddTimedDelayedOperation(2000, [this]() -> void
                {
                    DoCast(me, SPELL_MAGMA_WAVE_TICK, true);
                });

                AddTimedDelayedOperation(3500, [this]() -> void
                {
                    DoCast(me, SPELL_MAGMA_WAVE_TICK, true);
                });

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_LAVA_GEYSER, 1000);
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
                    case EVENT_MAGMA_SCULPTOR:
                    {
                        DoCast(me, urand(0, 1) ? SPELL_CONVERSATION_01 : SPELL_CONVERSATION_02, true);
                        DoCast(SPELL_MAGMA_SCULPTOR);
                        events.ScheduleEvent(EVENT_MAGMA_SCULPTOR, 70000);
                        break;
                    }
                    case EVENT_LANDSLIDE:
                    {
                        m_AlreadyHitTarget.clear();
                        DoCast(SPELL_LANDSLIDE);
                        events.ScheduleEvent(EVENT_LANDSLIDE, 16000);
                        events.ScheduleEvent(EVENT_MOLTEN_CRASH, 1000);
                        break;
                    }
                    case EVENT_CRYSTAL_WALL:
                    {
                        Talk(SAY_CRYSTAL_SPIKE);
                        DoCast(SPELL_CRYSTAL_WALL);
                        events.ScheduleEvent(EVENT_CRYSTAL_WALL, 22000);
                        break;
                    }
                    case EVENT_MOLTEN_CRASH:
                    {
                        DoCast(me, SPELL_MOLTEN_CRASH_VISUAL, true);

                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_MOLTEN_CRASH);

                        break;
                    }
                    case EVENT_LAVA_GEYSER:
                    {
                        DoCast(SPELL_LAVA_GEYSER_SUM);
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
            return new boss_dargrul_the_underkingAI(p_Creature);
        }
};

/// Molten Charskin - 101476
class npc_dargrul_molten_charskin : public CreatureScript
{
    public:
        npc_dargrul_molten_charskin() : CreatureScript("npc_dargrul_molten_charskin") { }

        struct npc_dargrul_molten_charskinAI : public ScriptedAI
        {
            npc_dargrul_molten_charskinAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_PlayerGuid;

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoZoneInCombat(me, 100.0f);

                if (IsHeroicOrMythic())
                    DoCast(me, SPELL_RISING_INFERNO, true);
                else
                    DoCast(me, SPELL_MAGMA_BREAKER_AURA, true);

                if (Creature* l_Boss = p_Summoner->ToCreature())
                {
                    if (!l_Boss->IsAIEnabled)
                        return;

                    if (Unit* l_Target = l_Boss->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1, 80.0f, true, -SPELL_FIXATE_PLR))
                    {
                        me->CastSpell(l_Target, SPELL_FIXATE_PLR);
                        return;
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_MAGMA_BREAKER_AURA);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case SPELL_FIXATE_PLR:
                    {
                        m_PlayerGuid = p_Target->GetGUID();

                        me->getThreatManager().clearReferences();
                        me->getThreatManager().addThreat(p_Target, std::numeric_limits<float>::max());

                        me->TauntApply(p_Target);

                        me->ClearUnitState(UnitState::UNIT_STATE_CASTING);

                        me->GetMotionMaster()->Clear(true);
                        me->GetMotionMaster()->MoveChase(p_Target);
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

                if (Player* l_Target = Player::GetPlayer(*me, m_PlayerGuid))
                {
                    if (!l_Target->isAlive() || me->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL) == nullptr)
                    {
                        m_PlayerGuid = 0;

                        if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Summoner->IsAIEnabled)
                            {
                                if (Unit* pTarget = l_Summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1, 80.0f, true, -SPELL_FIXATE_PLR))
                                {
                                    me->CastSpell(pTarget, SPELL_FIXATE_PLR);
                                    return;
                                }
                            }
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dargrul_molten_charskinAI(p_Creature);
        }
};

/// Crystal Wall Stalker - 101593
class npc_dargrul_crystal_wall : public CreatureScript
{
    public:
        npc_dargrul_crystal_wall() : CreatureScript("npc_dargrul_crystal_wall") {}

        struct npc_dargrul_crystal_wallAI : public ScriptedAI
        {
            npc_dargrul_crystal_wallAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            uint64 m_WallGUID;

            void Reset() override { }

            void JustDespawned() override
            {
                if (GameObject* l_GameObject = me->GetMap()->GetGameObject(m_WallGUID))
                    l_GameObject->Delete();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_CRYSTAL_BLOCKER_AT, true);
                DoCast(me, SPELL_CRYSTAL_WALL_SUM_GO, true);

                events.ScheduleEvent(EVENT_1, 200);
            }

            void JustSummonedGO(GameObject* p_GameObject) override
            {
                m_WallGUID = p_GameObject->GetGUID();
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_LANDSLIDE_DAMAGE || p_SpellInfo->Id == SPELL_MAGMA_WAVE_DUMMY)
                    DespawnMe();
            }

            void DespawnMe()
            {
                if (GameObject* l_GameObject = me->GetMap()->GetGameObject(m_WallGUID))
                    l_GameObject->Delete();

                me->DespawnOrUnsummon();
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Creature* l_Charskin = me->FindNearestCreature(NPC_MOLTEN_CHARSKIN, 2.0f))
                        {
                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                DespawnMe();
                            });

                            l_Charskin->CastSpell(l_Charskin, SPELL_CRYSTAL_CRACKED, true);
                            break;
                        }

                        events.ScheduleEvent(EVENT_1, 200);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dargrul_crystal_wallAI(p_Creature);
        }
};

/// Lava Geyser - 108926
class npc_dargrul_lava_geyser : public CreatureScript
{
    public:
        npc_dargrul_lava_geyser() : CreatureScript("npc_dargrul_lava_geyser") {}

        struct npc_dargrul_lava_geyserAI : public ScriptedAI
        {
            npc_dargrul_lava_geyserAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_LAVA_GEYSER_AT, true);
                DoCast(me, SPELL_FLAME_GOUT, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dargrul_lava_geyserAI(p_Creature);
        }
};

/// Gain Energy - 201444
class spell_dargrul_gain_energy : public SpellScriptLoader
{
    public:
        spell_dargrul_gain_energy() : SpellScriptLoader("spell_dargrul_gain_energy") { }

        class spell_dargrul_gain_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dargrul_gain_energy_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster)
                    return;

                if (l_Caster->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (l_Caster->GetPower(POWER_MANA) >= 60)
                {
                    l_Caster->CastSpell(l_Caster, SPELL_CONVERSATION_05, true);
                    l_Caster->CastSpell(l_Caster, SPELL_MAGMA_WAVE_AT, true);
                    l_Caster->CastSpell(l_Caster, SPELL_MAGMA_WAVE);

                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dargrul_gain_energy_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dargrul_gain_energy_AuraScript();
        }
};

/// Magma Breaker - 209920, 216368
class spell_dargrul_magma_breaker : public SpellScriptLoader
{
    public:
        spell_dargrul_magma_breaker() : SpellScriptLoader("spell_dargrul_magma_breaker") { }

        class spell_dargrul_magma_breaker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dargrul_magma_breaker_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->IsMoving())
                    l_Caster->CastSpell(l_Caster, SPELL_MAGMA_BREAKER, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dargrul_magma_breaker_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dargrul_magma_breaker_AuraScript();
        }
};

/// Magma Wave (Final Wave) - 200404
/// Magma Wave (Periodic) - 217090
class spell_dargrul_magma_wave_filter : public SpellScriptLoader
{
    public:
        spell_dargrul_magma_wave_filter() : SpellScriptLoader("spell_dargrul_magma_wave_filter") { }

        class spell_dargrul_magma_wave_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dargrul_magma_wave_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (p_Targets.empty())
                    return;

                std::list<GameObject*> l_WallList;
                l_Caster->GetGameObjectListWithEntryInGrid(l_WallList, GO_CRYSTAL_WALL_COLLISION, 100.0f);

                for (GameObject* l_Wall : l_WallList)
                {
                    if (p_Targets.empty())
                        break;

                    p_Targets.remove_if([=](WorldObject* p_Object) -> bool
                    {
                        if (l_Wall->IsInBetween(l_Caster, p_Object, 4.0f))
                            return true;

                        return false;
                    });
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dargrul_magma_wave_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);

                if (m_scriptSpellId == SPELL_MAGMA_WAVE_TICK)
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dargrul_magma_wave_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dargrul_magma_wave_filter_SpellScript();
        }
};

/// Molten Crash - 200732
class spell_dargrul_molten_crash : public SpellScriptLoader
{
    public:
        spell_dargrul_molten_crash() : SpellScriptLoader("spell_dargrul_molten_crash") { }

        class spell_dargrul_molten_crash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dargrul_molten_crash_SpellScript);

            void HandleTriggerEffect(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetExplTargetUnit())
                    return;

                if (GetExplTargetUnit()->IsActiveSaveAbility())
                    PreventHitDefaultEffect(EFFECT_1);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_dargrul_molten_crash_SpellScript::HandleTriggerEffect, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dargrul_molten_crash_SpellScript();
        }
};

/// Crystal Spikes - 200551
class spell_dargrul_crystal_spikes_searcher : public SpellScriptLoader
{
    public:
        spell_dargrul_crystal_spikes_searcher() : SpellScriptLoader("spell_dargrul_crystal_spikes_searcher") { }

        class spell_dargrul_crystal_spikes_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dargrul_crystal_spikes_searcher_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                WorldObject* obj = nullptr;
                std::vector<WorldObject*> ranges, melees, healers, targets;
                for (auto& target : p_Targets)
                {
                    if (auto plr = target->ToPlayer())
                    {
                        if (plr->IsRangedDamageDealer(false))
                            ranges.emplace_back(plr);
                        else if (plr->IsMeleeDamageDealer(false, false))
                            melees.emplace_back(plr);
                        else if (plr->IsHealer())
                            healers.push_back(plr);
                    }
                }

                auto IsLesserThan3Ranges = (ranges.size() + healers.size()) < 3;

                targets.insert(targets.end(), ranges.begin(), ranges.end());
                targets.insert(targets.end(), healers.begin(), healers.end());
                if (IsLesserThan3Ranges)
                    targets.insert(targets.end(), melees.begin(), melees.end());

                p_Targets.clear();
                p_Targets.push_back(JadeCore::Containers::SelectRandomContainerElement(targets));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dargrul_crystal_spikes_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dargrul_crystal_spikes_searcher_SpellScript();
        }
};

/// Landslide (Damage filter) - 200721
class spell_dargrul_landslide_dmg_filter : public SpellScriptLoader
{
    public:
        spell_dargrul_landslide_dmg_filter() : SpellScriptLoader("spell_dargrul_landslide_dmg_filter") { }

        class spell_dargrul_landslide_dmg_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dargrul_landslide_dmg_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                uint64 l_Guid = GetCaster()->GetGUID();
                p_Targets.remove_if([this, l_Guid](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    Creature* l_Caster = Creature::GetCreature(*p_Object, l_Guid);
                    if (!l_Caster || !l_Caster->IsAIEnabled)
                        return true;

                    if (l_Caster->AI()->GetData(p_Object->GetGUIDLow()))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dargrul_landslide_dmg_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dargrul_landslide_dmg_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dargrul_landslide_dmg_filter_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_dargrul_the_underking()
{
    /// Boss
    new boss_dargrul_the_underking();

    /// Creatures
    new npc_dargrul_molten_charskin();
    new npc_dargrul_crystal_wall();
    new npc_dargrul_lava_geyser();

    /// Spells
    new spell_dargrul_gain_energy();
    new spell_dargrul_magma_breaker();
    new spell_dargrul_magma_wave_filter();
    new spell_dargrul_molten_crash();
    new spell_dargrul_crystal_spikes_searcher();
    new spell_dargrul_landslide_dmg_filter();
}
#endif
