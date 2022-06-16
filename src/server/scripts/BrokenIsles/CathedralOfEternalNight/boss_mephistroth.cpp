#include "cathedral_of_eternal_night.hpp"

enum eSpells
{
    SpellMephistrothFadeIn      = 241817,
    SpellMephistrothFadeOut     = 241909,
    SpellCarrionSwarm           = 233155,
    SpellCarrionSwarmMissile    = 233175,
    SpellDemonicUpheaval        = 233196,
    SpellDemonicUpheavalSummon  = 236370,
    SpellDemonicUpheavalDmg     = 243152,
    SpellDarkSolitude           = 234817,
    SpellShadowFade             = 233206,
    SpellShadowBlastSummon      = 234011,
    SpellShadowBlastAt          = 236233,
    SpellShadowBlastDmg         = 241788,
    SpellCreepingShadows        = 234370,

    /// Illidan
    SpellExpelShadowsAoe        = 215466,
    SpellExpelShadowsPowerBar   = 234875,

    SpellAegisOfAggramarOverride            = 234114,
    SpellAegisOfAggramarMissile             = 234125,
    SpellAegisOfAggramarBuffCounterShadows  = 234083,
};

enum eEvents
{
    EventCarrionSwarm   = 1,
    EventDemonicUpheaval,
    EventDarkSolitude,
    EventShadowFade,
};

enum eTalks
{
    TalkIntro,
    TalkAggro,
    TalkShadowFadeIn,
    TalkShadowBlast,
    TalkShadowFadeOut,
    TalkWipe,
    TalkDied,
};

enum eMephistrothData
{
    ShadowOrbsRepeled = 40,
};

using ShadowOfMephistrothInfo = std::tuple<Position, bool, uint64>;

const std::array<Position, 4> g_DreadBatsPos = 
{
    {
        { -506.850f, 2487.500f, 552.193f },
        { -506.545f, 2565.040f, 552.193f },
        { -574.338f, 2487.679f, 554.192f },
    }
};

/// Mephistroth - 116944
class boss_mephistroth : public CreatureScript
{
    public:
        boss_mephistroth() : CreatureScript("boss_mephistroth")
        {}

        struct boss_mephistroth_AI : public BossAI
        {
            explicit boss_mephistroth_AI(Creature* p_Me) : BossAI(p_Me, eData::DataMephistroth)
            {
                m_Intro = false;
                me->SetVisible(false);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                m_ShadowsOrbRepeled = 0;

                m_OddTick = false;

                _Reset();
                CleanUp();
                CheckIntro();
                FillShadowsPositionsList();

                if (IsMythic())
                    me->UpdateStatsForLevel();

                me->SetPower(Powers::POWER_ENERGY, 50);
            }

            void GetMythicHealthMod(float& p_HealthMod) override
            {
                p_HealthMod = 2.103f;
            }

            void CleanUp()
            {
                std::list<AreaTrigger*> l_Areas;

                me->GetAreaTriggerListWithSpellIDInRange(l_Areas, eSpells::SpellShadowBlastAt, 250.f);

                for (AreaTrigger* l_Itr : l_Areas)
                {
                    l_Itr->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                    l_Itr->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                    l_Itr->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                    l_Itr->SetDuration(500);
                }

                me->DespawnCreaturesInArea(eCreatures::NpcShadowOfMephistroth, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcMephistrothDreadwing, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcFelSpike, 250.f);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCreepingShadows);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAegisOfAggramarOverride);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer) override
            {
                if (p_Summon->GetEntry() == eCreatures::NpcShadowOfMephistroth)
                {
                    auto l_Itr = std::find_if(m_ShadowsPositions.begin(), m_ShadowsPositions.end(), [&p_Summon] (ShadowOfMephistrothInfo& p_Itr)
                    {
                        return p_Summon->GetGUID() == std::get<2>(p_Itr);
                    });

                    if (l_Itr != m_ShadowsPositions.end())
                    {
                        std::get<1>(*l_Itr) = false;
                        std::get<2>(*l_Itr) = 0;
                    }
                }
            }

            void EnterEvadeMode() override
            {
                Creature* l_Illidan = me->FindNearestCreature(eCreatures::NpcIllidanStormrage, 250.f);

                if (l_Illidan != nullptr && l_Illidan->IsAIEnabled)
                    l_Illidan->GetAI()->SetData(eCreatures::BossMephistroth, EncounterState::FAIL);

                CleanUp();
                BossAI::EnterEvadeMode();
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                if (p_Power != Powers::POWER_ENERGY)
                {
                    p_Value = 0;
                    return;
                }

                if (!me->isInCombat() || m_ShadowFade)
                {
                    p_Value = 0;
                    return;
                }

                p_Value = m_OddTick ? 3 : 2;

                m_OddTick = !m_OddTick;

                if ((me->GetPower(p_Power) + p_Value) >= me->GetMaxPower(p_Power) && !events.HasEvent(eEvents::EventShadowFade))
                    events.ScheduleEvent(eEvents::EventShadowFade, 1);
            }

            void EnterCombat(Unit* /**/) override
            {
                m_ShadowFade = false;

                Talk(eTalks::TalkAggro);
                _EnterCombat();
                FillShadowsPositionsList();
                events.ScheduleEvent(eEvents::EventCarrionSwarm, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDarkSolitude, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDemonicUpheaval, 3 * IN_MILLISECONDS);
            }

            void CheckIntro()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (l_Instance->GetData(me->GetEntry()) == EncounterState::DONE)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetVisible(true);
                    m_Intro = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                }
                else
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    m_Intro = false;
                    me->SetVisible(false);
                }
            }
            
            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellShadowFade)
                {
                    me->SetPower(Powers::POWER_ENERGY, 0, false);
                    Talk(eTalks::TalkShadowFadeIn);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoCastSpellOnPlayers(eSpells::SpellCreepingShadows);
                }
            }

            void SpellHit(Unit* /**/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellExpelShadowsAoe)
                {
                    Talk(eTalks::TalkShadowFadeOut);
                    me->RemoveAurasDueToSpell(eSpells::SpellShadowFade);
                    me->DespawnCreaturesInArea(eCreatures::NpcShadowOfMephistroth);
                    FillShadowsPositionsList();
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCreepingShadows);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    m_ShadowFade = false;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionBossIntro && !m_Intro)
                {
                    AddTimedDelayedOperation(12 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::TalkIntro);
                        me->SetReactState(REACT_DEFENSIVE);
                        me->SetVisible(true);
                        me->CastSpell(me, eSpells::SpellMephistrothFadeIn, true);
                        m_Intro = true;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    });
                }
            }

            void FillShadowsPositionsList()
            {
                m_ShadowsPositions.clear();

                std::list<Creature*> l_ShadowsDummies;

                me->GetCreatureListWithEntryInGridAppend(l_ShadowsDummies, eCreatures::NpcFelPortalDummy, 300.f);
                me->GetCreatureListWithEntryInGridAppend(l_ShadowsDummies, eCreatures::NpcFelPortalDummy2, 300.f);

                for (Creature* l_Itr : l_ShadowsDummies)
                {
                    if (l_Itr == nullptr)
                        continue;

                    m_ShadowsPositions.push_back(ShadowOfMephistrothInfo(l_Itr->GetPosition(), false, 0));
                }
            }

            void SetData(uint32 p_Id, uint32 p_Data) override
            {
                if (p_Id == eCreatures::NpcShadowOfMephistroth && p_Data == EncounterState::IN_PROGRESS)
                {
                    auto l_Itr = std::find_if(m_ShadowsPositions.begin(), m_ShadowsPositions.end(), [] (ShadowOfMephistrothInfo & p_Itr)
                    {
                        return std::get<1>(p_Itr) == false;
                    });

                    if (l_Itr != m_ShadowsPositions.end())
                    {
                        std::get<1>(*l_Itr) = true;
                        auto* l_Ptr = me->SummonCreature(eCreatures::NpcShadowOfMephistroth, std::get<0>(*l_Itr), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        
                        if (l_Ptr != nullptr)
                            std::get<2>(*l_Itr) = l_Ptr->GetGUID();
                    }
                }
                else if (p_Id == eMephistrothData::ShadowOrbsRepeled)
                    m_ShadowsOrbRepeled += p_Data;
            }

            void JustReachedHome() override
            {
                Talk(eTalks::TalkWipe);
                _JustReachedHome();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                CleanUp();
            }

            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDied);
                me->RemoveAllAreasTrigger();
                CleanUp();
                _JustDied();

                if (IsMythic() && m_ShadowsOrbRepeled > 20)
                    instance->DoCompleteAchievement(eAchievements::MasterOfShadows);

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                BossAI::UpdateAI(p_Diff);
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventCarrionSwarm:
                    {
                        DoCastVictim(eSpells::SpellCarrionSwarm);
                        events.ScheduleEvent(eEvents::EventCarrionSwarm, 18 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDarkSolitude:
                    {
                        DoCastAOE(eSpells::SpellDarkSolitude);
                        events.ScheduleEvent(eEvents::EventDarkSolitude, 8 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDemonicUpheaval:
                    {
                        DoCastAOE(eSpells::SpellDemonicUpheaval);
                        events.ScheduleEvent(eEvents::EventDemonicUpheaval, 32 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventShadowFade:
                    {
                        m_ShadowFade = true;

                        me->CastSpell(me, eSpells::SpellShadowFade, false);
                        Creature* l_Illidan = me->FindNearestCreature(eCreatures::NpcIllidanStormrage, 250.f, true);

                        if (l_Illidan != nullptr && l_Illidan->IsAIEnabled)
                            l_Illidan->GetAI()->DoAction(eSharedActions::ActionMephistrothAppear);

                        break;
                    }
                }
            }

            private:
                bool m_ShadowFade = false;
                bool m_Intro;
                uint8 m_ShadowsOrbRepeled;
                std::vector<ShadowOfMephistrothInfo> m_ShadowsPositions;
                bool m_OddTick = false;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_mephistroth_AI(p_Me);
        }
};

/// Fel Spike - 117124
class npc_mephistroth_fel_spike : public CreatureScript
{
    public:
        npc_mephistroth_fel_spike() : CreatureScript("npc_mephistroth_fel_spike")
        {}

        struct npc_mephistroth_fel_spike_AI : public ScriptedAI
        {
            explicit npc_mephistroth_fel_spike_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                DoCastAOE(eSpells::SpellDemonicUpheavalDmg, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {}
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_mephistroth_fel_spike_AI(p_Me);
        }
};

/// Dreadwing - 118642
class npc_mephistroth_dreadwing : public CreatureScript
{
    public:
        npc_mephistroth_dreadwing() : CreatureScript("npc_mephistroth_dreadwing")
        {}

        enum eSpells
        {
            SpellFelBlaze   = 239522,
            SpellFelBlazeMissile    = 239524
        };

        struct npc_mephistroth_dreadwing_AI : public ScriptedAI
        {
            explicit npc_mephistroth_dreadwing_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetCanFly(true);
                me->SetDisableGravity(true, true);
                
                me->SetSpeed(MOVE_WALK, 4.0f);
                me->SetSpeed(MOVE_RUN, 4.0f);
                me->SetSpeed(MOVE_SWIM, 4.0f);
                me->SetSpeed(MOVE_FLIGHT, 4.0f);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::SpellFelBlaze)
                {
                    Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcDreadlordbatStalker, 100.f);

                    if (l_Stalker != nullptr)
                    {
                        me->SetFacingToObject(l_Stalker);

                        Position l_Tgt = l_Stalker->GetPosition();

                        me->GetNearestPosition(l_Tgt, 30.f, 0);

                        const uint8 l_MaxMissiles = urand(6, 8);

                        for (uint8 l_Itr = 0; l_Itr < l_MaxMissiles; ++l_Itr)
                        {
                            l_Stalker->GetRandomNearPosition(l_Tgt, frand(5, 30));
                            m_MissilesPos.push_back(l_Tgt);
                        }

                        AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                        {
                            for (auto & l_Itr : m_MissilesPos)
                                me->CastSpell(l_Itr, eSpells::SpellFelBlazeMissile, true);
                        });
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionMephistrothAppear)
                {
                    DoCast(me, eSpells::SpellFelBlaze, true);

                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        Position l_Pos;
                        float l_Ori = me->GetHomePosition().m_orientation;

                        if (l_Ori == 3.8238f)
                            l_Pos = g_DreadBastSummonPos.at(2);
                        else if (l_Ori == 5.4000f)
                            l_Pos = g_DreadBastSummonPos.at(3);
                        else if (l_Ori == 0.8006f)
                            l_Pos = g_DreadBastSummonPos.at(0);
                        else
                            l_Pos = g_DreadBastSummonPos.at(1);

                        me->GetMotionMaster()->MovePoint(eSharedPoints::PointMephistrothWindow, l_Pos);
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type == POINT_MOTION_TYPE && p_Id == eSharedPoints::PointMephistrothWindow)
                    me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                ScriptedAI::UpdateAI(p_Diff);
            }

        private:
            std::vector<Position> m_MissilesPos;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_mephistroth_dreadwing_AI(p_Me);
        }
};

/// Aegis of Aggramar - 118418
class npc_mephistroth_aegis_of_aggramar : public CreatureScript
{
    public:
        npc_mephistroth_aegis_of_aggramar() : CreatureScript("npc_mephistroth_aegis_of_aggramar")
        {}

        struct npc_mephistroth_aegis_of_aggramar_AI : public ScriptedAI
        {
            explicit npc_mephistroth_aegis_of_aggramar_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
            }

            void Reset() override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (l_Instance->GetBossState(eData::DataDomatrax) == EncounterState::DONE)
                    me->SetVisible(true);
                else
                    me->SetVisible(false);
            }

            void OnSpellClick(Unit* p_Player) override
            {
                p_Player->CastSpell(p_Player, eSpells::SpellAegisOfAggramarOverride, true);
                me->SetVisible(false);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_mephistroth_aegis_of_aggramar_AI(p_Me);
        }
};

/// Shadow of Mephistroth - 117590
class npc_mephistroth_shadow_of_mephistroth : public CreatureScript
{
    public:
        npc_mephistroth_shadow_of_mephistroth() : CreatureScript("npc_mephistroth_shadow_of_mephistroth")
        {}

        enum eEvents
        {
            EventShadowBlast     = 1,
        };

        struct npc_mephistroth_shadow_of_mephistroth_AI : public ScriptedAI
        {
            explicit npc_mephistroth_shadow_of_mephistroth_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 1.95001f;
            }

            void Reset() override
            {
                events.ScheduleEvent(eEvents::EventShadowBlast, IN_MILLISECONDS);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventShadowBlast)
                {
                    Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcAegisRPFinaleStalker, 250.f, true);

                    if (l_Stalker != nullptr)
                    {
                        me->SetFacingToObject(l_Stalker);
                        DoCast(me, eSpells::SpellShadowBlastAt);

                        Creature* l_Mephistroth = me->FindNearestCreature(eCreatures::BossMephistroth, 250.f);

                        if (l_Mephistroth != nullptr && l_Mephistroth->IsAIEnabled)
                        {
                            if (roll_chance_i(5))
                                l_Mephistroth->AI()->Talk(eTalks::TalkShadowBlast);
                        }
                    }

                    events.ScheduleEvent(eEvents::EventShadowBlast, 7 * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_mephistroth_shadow_of_mephistroth_AI(p_Me);
        }
};

/// Carrion Swarm - 233155
class spell_mephistroth_carrion_swarm : public SpellScriptLoader
{
    public:
        spell_mephistroth_carrion_swarm() : SpellScriptLoader("spell_mephistroth_carrion_swarm")
        {}

        class spell_mephistroth_carrion_swarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mephistroth_carrion_swarm_SpellScript);

            void HandleOnCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Position l_Src = GetCaster()->GetPosition();
                Position l_Tgt;

                l_Caster->GetFirstCollisionPosition(l_Tgt, 50.f, 0);

                G3D::Vector3 l_Dir;
                G3D::Vector3 l_Pos = l_Src.AsVector3();

                l_Dir = (l_Tgt.AsVector3() - l_Src.AsVector3());

                for (uint8 l_Itr = 2; l_Itr <= 20; l_Itr += 2)
                {
                    l_Pos = l_Src.AsVector3() + (l_Dir * (l_Itr / 10.f));
                    l_Caster->CastSpell(l_Pos, eSpells::SpellCarrionSwarmMissile, true);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mephistroth_carrion_swarm_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mephistroth_carrion_swarm_SpellScript();
        }
};

/// Demonic Upheaval AOE - 233196
class spell_mephistroth_demonic_upheaval_aoe : public SpellScriptLoader
{
    public:
        spell_mephistroth_demonic_upheaval_aoe() : SpellScriptLoader("spell_mephistroth_demonic_upheaval_aoe")
        {}

        class spell_mephistroth_demonic_upheaval_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mephistroth_demonic_upheaval_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort([] (WorldObject*& p_Lhs, WorldObject*& p_Rhs) -> bool
                {
                    if (p_Lhs == nullptr || p_Rhs == nullptr)
                        return false;

                    Player* l_LhsPlayer = p_Lhs->ToPlayer();
                    Player* l_RhsPlayer = p_Rhs->ToPlayer();

                    if (l_LhsPlayer == nullptr || l_RhsPlayer == nullptr)
                        return false;

                    uint32 l_LhsRole = l_LhsPlayer->GetRoleForGroup();
                    uint32 l_RhsRole = l_RhsPlayer->GetRoleForGroup();

                    if (l_LhsRole == ROLE_DAMAGE && l_RhsRole == ROLE_DAMAGE)
                        return l_LhsPlayer->IsRangedDamageDealer(false) && !l_RhsPlayer->IsRangedDamageDealer(false);

                    return l_LhsRole > l_RhsRole;
                });

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mephistroth_demonic_upheaval_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mephistroth_demonic_upheaval_aoe_SpellScript();
        }
};

/// Demonic Upheaval - 233963
class spell_mephistroth_demonic_upheaval : public SpellScriptLoader
{
    public:
        spell_mephistroth_demonic_upheaval() : SpellScriptLoader("spell_mephistroth_demonic_upheaval")
        {}

        class spell_mephistroth_demonic_upheaval_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mephistroth_demonic_upheaval_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                GetCaster()->CastSpell(GetUnitOwner(), eSpells::SpellDemonicUpheavalSummon, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mephistroth_demonic_upheaval_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mephistroth_demonic_upheaval_AuraScript();
        }
};

/// Shadow Fade - 233206
class spell_mephistroth_shadow_fade : public SpellScriptLoader
{
    public:
        spell_mephistroth_shadow_fade() : SpellScriptLoader("spell_mephistroth_shadow_fade")
        {}

        class spell_mephistroth_shadow_fade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mephistroth_shadow_fade_AuraScript);

            void HandleOnApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                m_Counter = 0;
                l_Caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                GetCaster()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void HandlePeriodic(AuraEffect const* /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                m_Counter++;
                Unit* l_Caster = GetCaster();
                
                if (l_Caster->IsAIEnabled)
                {
                    l_Caster->GetAI()->SetData(eCreatures::NpcShadowOfMephistroth, EncounterState::IN_PROGRESS);

                    if (m_Counter == 3 && l_Caster->GetMap() && l_Caster->GetMap()->IsMythic())
                    {
                        m_Counter = 0;
                        auto* l_Ptr = l_Caster->SummonCreature(eCreatures::NpcMephistrothDreadwing, g_DreadBastSummonPos.at(urand(0, 3)), TEMPSUMMON_TIMED_DESPAWN, 20000);

                        if (l_Ptr != nullptr && l_Ptr->IsAIEnabled)
                            l_Ptr->GetAI()->DoAction(eSharedActions::ActionMephistrothAppear);
                    }
                }

            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mephistroth_shadow_fade_AuraScript::HandleOnApply, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mephistroth_shadow_fade_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mephistroth_shadow_fade_AuraScript::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }

            private:
                uint8 m_Counter;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mephistroth_shadow_fade_AuraScript();
        }
};

/// Expel Shadow - 234875
class spell_mephistroth_expel_shadow : public SpellScriptLoader
{
    public:
        spell_mephistroth_expel_shadow() : SpellScriptLoader("spell_mephistroth_expel_shadow")
        {}

        class spell_mephistroth_expel_shadow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mephistroth_expel_shadow_AuraScript);

            void HandlePeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (l_Owner->GetPower(Powers::POWER_ALTERNATE_POWER) + 10 >= 100)
                {
                    Creature* l_Boss = l_Owner->FindNearestCreature(eCreatures::BossMephistroth, 200.f);

                    if (l_Boss != nullptr)
                    {
                        l_Owner->CastSpell(l_Boss, eSpells::SpellExpelShadowsAoe, true);
                        Remove();
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mephistroth_expel_shadow_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mephistroth_expel_shadow_AuraScript();
        }
};

/// Aegis of Aggramar Override Spell Bar - 234114
class spell_mephistroth_aegis_of_aggramar_override_spell_bar : public SpellScriptLoader
{
    public:
        spell_mephistroth_aegis_of_aggramar_override_spell_bar() : SpellScriptLoader("spell_mephistroth_aegis_of_aggramar_override_spell_bar")
        {}

        class spell_mephistroth_aegis_of_aggramar_override_spell_bar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mephistroth_aegis_of_aggramar_override_spell_bar_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Creature* l_Aegis = GetUnitOwner()->FindNearestCreature(eCreatures::NpcAegisOfAggramarMephistroth, 100.f, true);

                if (l_Aegis != nullptr)
                {
                    l_Aegis->NearTeleportTo(GetUnitOwner()->GetPosition());
                    l_Aegis->SetVisible(true);
                }

                GetUnitOwner()->RemoveAurasDueToSpell(eSpells::SpellAegisOfAggramarOverride);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mephistroth_aegis_of_aggramar_override_spell_bar_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mephistroth_aegis_of_aggramar_override_spell_bar_AuraScript();
        }
};

/// Shadow Blast - 236233
class at_mephistroth_shadow_blast : public AreaTriggerEntityScript
{
    public:
        at_mephistroth_shadow_blast() : AreaTriggerEntityScript("at_mephistroth_shadow_blast")
        {
            m_Despawnmed = false;
        }

        void OnSetCreatePosition(AreaTrigger* p_At, Unit* /**/, Position& /**/, Position& p_DestinationPosition, std::vector<G3D::Vector3>& /**/) override
        {
            p_At->SetDuration(7500);
            p_At->SetTimeToTarget(7500);
            p_At->SetMoveType(AreaTriggerMoveType::AT_MOVE_TYPE_THROUGH_WALLS);
            p_DestinationPosition = g_AegisOfAggramarPos;
        }

        void DespawnAreatrigger(AreaTrigger* p_At)
        {
            p_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
            p_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
            p_At->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
            p_At->SetDuration(500);
        }

        void OnRemove(AreaTrigger* p_At, uint32 /**/) override
        {
            if (!m_Despawnmed)
                DespawnAreatrigger(p_At);
        }

        void CountShadowForAchievement(AreaTrigger* p_At)
        {
            InstanceScript* l_Instance = p_At->GetInstanceScript();
            uint64 l_Guid = (l_Instance != nullptr ? l_Instance->GetData64(eCreatures::BossMephistroth) : 0);

            if (Creature* l_Boss = sObjectAccessor->GetCreature(*p_At, l_Guid))
            {
                if (l_Boss->IsAIEnabled)
                    l_Boss->GetAI()->SetData(eMephistrothData::ShadowOrbsRepeled, 1);
            }
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr || m_Despawnmed)
                return true;

            if (p_Target->GetEntry() == eCreatures::NpcIllidanStormrage)
            {
                m_Despawnmed = true;

                p_Target->CastStop();
                p_Target->RemoveAllAuras();
                p_Target->SetPower(Powers::POWER_ALTERNATE_POWER, 0);
                p_Target->DelayedCastSpell(p_Target, eSpells::SpellExpelShadowsPowerBar, false, IN_MILLISECONDS);

                p_At->GetCaster()->CastSpell(p_Target, eSpells::SpellShadowBlastDmg, true);
                DespawnAreatrigger(p_At);
            }
            else if (p_Target->IsPlayer())
            {
                m_Despawnmed = true;
                if (p_Target->isInFront(p_At, float(M_PI)) && p_Target->HasAura(eSpells::SpellAegisOfAggramarBuffCounterShadows))
                {
                    if (p_At->GetMap() && p_At->GetMap()->IsMythic())
                        CountShadowForAchievement(p_At);

                    DespawnAreatrigger(p_At);
                    return true;
                }
                else
                    p_At->GetCaster()->CastSpell(p_Target, eSpells::SpellShadowBlastDmg, true);
                
                DespawnAreatrigger(p_At);
            }

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_mephistroth_shadow_blast();
        }

    private:
        bool m_Despawnmed;
};

#ifndef __clang_analyzer__
void AddSC_boss_mephistroth()
{
    new boss_mephistroth();
    new npc_mephistroth_fel_spike();
    new npc_mephistroth_aegis_of_aggramar();
    new npc_mephistroth_shadow_of_mephistroth();
    new npc_mephistroth_dreadwing();

    new spell_mephistroth_carrion_swarm();
    new spell_mephistroth_demonic_upheaval_aoe();
    new spell_mephistroth_demonic_upheaval();
    new spell_mephistroth_shadow_fade();
    new spell_mephistroth_expel_shadow();
    new spell_mephistroth_aegis_of_aggramar_override_spell_bar();

    new at_mephistroth_shadow_blast();
}
#endif
