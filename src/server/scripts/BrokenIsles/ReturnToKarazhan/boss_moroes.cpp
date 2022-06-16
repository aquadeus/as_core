////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

Position const g_HelperPos[4] =
{
    { -10988.90f, -1878.57f, 81.81f, 0.04f },
    { -10986.40f, -1876.76f, 81.81f, 4.26f },
    { -10978.29f, -1877.71f, 81.81f, 4.87f },
    { -10976.29f, -1879.96f, 81.81f, 3.19f }
};

/// Moroes <Tower Steward> - 114312
class boss_rtk_moroes : public CreatureScript
{
    public:
        boss_rtk_moroes() : CreatureScript("boss_rtk_moroes") { }

        struct boss_rtk_moroesAI : public BossAI
        {
            boss_rtk_moroesAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataMoroes)
            {
                m_GroupMember = sFormationMgr->CreateCustomFormation(me);
                std::vector<uint32> l_Helpers =
                {
                    eCreatures::BaronessDorothea,
                    eCreatures::LadyCatriona,
                    eCreatures::BaronRafe,
                    eCreatures::LadyKeira,
                    eCreatures::LordRobin,
                    eCreatures::LordCrispin
                };

                uint32 l_WS = sWorld->getWorldState(WorldStates::WS_RETURN_TO_KARAZHAN);
                for (uint32 l_I = 3; (1 << l_I) <= l_WS; ++l_I)
                {
                    if (l_WS & (1 << l_I))
                        m_HelperList.push_back(l_Helpers[l_I - 3]);
                }
            }

            enum eTalks
            {
                Aggro               = 0,
                TalkGhastlyPurge    = 1,
                Death               = 2,
                TalkCoatCheck       = 3,
                PlayerKilled        = 4,
                Intro               = 5
            };

            enum eSpells
            {
                VanishFilter        = 227737,
                Vanish              = 227736,
                Garrote             = 227742,
                ConversationCoat    = 229848,
                CoatCheck           = 227851,
                CoatCheckResist     = 227832,
                GhastlyPurge        = 227872,
                SummonKeys          = 233989
            };

            enum eEvents
            {
                EventVanish = 1,
                EventCoatCheck = 2
            };

            enum eAchievements
            {
                DineAndBash = 11338
            };

            std::vector<uint32> m_HelperList;
            FormationInfo* m_GroupMember;
            uint8 m_HealthPct;

            int32 m_Timer;
            bool m_TimerStarted;
            uint32 m_DeadGuests;
            bool m_AchievementComplete;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.3f;
            }

            void Reset() override
            {
                _Reset();
                m_HealthPct = 51;
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                m_Timer = 0;
                m_TimerStarted = false;
                m_DeadGuests = 0;
                m_AchievementComplete = false;

                if (!m_HelperList.empty())
                {
                    for (uint8 l_I = 0; l_I < 4; l_I++)
                    {
                        if (Creature* l_Helper = me->SummonCreature(m_HelperList[l_I], g_HelperPos[l_I]))
                        {
                            if (CreatureGroup* l_Formation = me->GetFormation())
                                l_Formation->AddMember(l_Helper, m_GroupMember);
                        }
                    }
                }
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventVanish, 7000);
                events.ScheduleEvent(eEvents::EventCoatCheck, 30000);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::DataMoroes, EncounterState::FAIL);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);
                _JustDied();

                if (IsHeroicOrMythic())
                    DoCast(me, eSpells::SummonKeys, true);

                if (m_AchievementComplete && IsMythic())
                    instance->DoCompleteAchievement(eAchievements::DineAndBash);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::Vanish)
                    DoCast(p_Target, eSpells::Garrote, true);
                else if (p_SpellInfo->Id == eSpells::CoatCheck)
                {
                    Talk(eTalks::TalkCoatCheck);
                    DoCast(p_Target, eSpells::CoatCheckResist, true);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(m_HealthPct)) ///< 51%
                {
                    m_HealthPct = 0;
                    Talk(eTalks::TalkGhastlyPurge);
                    DoCast(me, eSpells::GhastlyPurge, true);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        Talk(eTalks::Intro);
                        break;
                    }
                    case ACTION_2:
                    {
                        if (!m_TimerStarted || m_Timer > 0)
                        {
                            m_TimerStarted = true;
                            m_Timer = 10 * IN_MILLISECONDS;
                            m_DeadGuests++;
                            if (m_DeadGuests == 4)
                                m_AchievementComplete = true;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_TimerStarted)
                    m_Timer -= p_Diff;

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 70.0f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventVanish:
                    {
                        DoCast(me, eSpells::VanishFilter, true);
                        events.ScheduleEvent(eEvents::EventVanish, 19000);
                        break;
                    }
                    case eEvents::EventCoatCheck:
                    {
                        Talk(eTalks::TalkCoatCheck);
                        DoCastVictim(eSpells::CoatCheck);
                        events.ScheduleEvent(eEvents::EventCoatCheck, 30000);
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
            return new boss_rtk_moroesAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Vanish - 227737
class spell_rtk_vanish : public SpellScriptLoader
{
    public:
        spell_rtk_vanish() : SpellScriptLoader("spell_rtk_vanish") { }

        class spell_rtk_vanish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_vanish_SpellScript);

            enum eSpells
            {
                Vanish = 227736,
                Garrote = 227742
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                auto l_GetStacks = [](WorldObject* p_Target) -> uint32
                {
                    uint32 l_Stacks = 0;

                    Unit* l_Target = p_Target->ToUnit();

                    if (!l_Target)
                        return l_Stacks;

                    if (Aura* l_Garrote = l_Target->GetAura(eSpells::Garrote))
                        l_Stacks = l_Garrote->GetStackAmount();

                    return l_Stacks;
                };

                auto l_Cmp = [&l_GetStacks](WorldObject* p_Lhs, WorldObject* p_Rhs)
                {
                    uint32 l_LhsPriority = l_GetStacks(p_Lhs);
                    uint32 l_RhsPriority = l_GetStacks(p_Rhs);

                    return l_LhsPriority < l_RhsPriority;
                };

                bool l_AnyHasGarrote = std::any_of(p_Targets.begin(), p_Targets.end(), JadeCore::UnitAuraCheck(true, eSpells::Garrote));
                WorldObject* l_Healer = nullptr;

                if (!l_AnyHasGarrote)
                {
                    std::list<WorldObject*> l_Targets = p_Targets;

                    for (WorldObject* p_Itr : p_Targets)
                    {
                        Player* l_Target = p_Itr->ToPlayer();

                        if (!l_Target)
                            continue;

                        if (l_Target->IsHealer() && l_Target->isAlive())
                        {
                            l_Healer = p_Itr;
                            break;
                        }
                    }

                    if (l_Healer)
                    {
                        p_Targets.clear();
                        p_Targets.push_back(l_Healer);
                    }
                }
                else
                {
                    p_Targets.sort(l_Cmp);
                    p_Targets.resize(1);
                }
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Vanish, false);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rtk_vanish_SpellScript::HandleOnHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_vanish_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_vanish_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Baroness Dorothea Millstipe - 114316
class npc_moroes_baroness_dorothea_millstipe : public CreatureScript
{
    public:
        npc_moroes_baroness_dorothea_millstipe() : CreatureScript("npc_moroes_baroness_dorothea_millstipe") { }

        struct npc_moroes_baroness_dorothea_millstipeAI : public ScriptedAI
        {
            npc_moroes_baroness_dorothea_millstipeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DinnerParty     = 233669,
                DinnerPartyAT   = 233671,
                ArcaneBlast     = 227575,
                ManaDrain       = 227545
            };

            enum eEvents
            {
                EventArcaneBlast = 1,
                EventManaDrain
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.2f;
            }

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::DinnerParty, true);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoCast(me, eSpells::DinnerPartyAT, true);

                events.ScheduleEvent(eEvents::EventArcaneBlast, 1000);
                events.ScheduleEvent(eEvents::EventManaDrain, 9000);

                DoZoneInCombat(me, 100.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Moroes = me->FindNearestCreature(eCreatures::Moroes, 100.0f);
                if (!l_Moroes || !l_Moroes->IsAIEnabled)
                    return;

                l_Moroes->GetAI()->DoAction(ACTION_2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcaneBlast:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(l_Target, eSpells::ArcaneBlast);
                        events.ScheduleEvent(eEvents::EventArcaneBlast, 3000);
                        break;
                    }
                    case eEvents::EventManaDrain:
                    {
                        std::list<Player*> l_PlayerList;
                        GetPlayerListInGrid(l_PlayerList, me, 80.0f);

                        if (!l_PlayerList.empty())
                        {
                            JadeCore::RandomResizeList(l_PlayerList, l_PlayerList.size());

                            bool l_Casted = false;
                            for (Player* l_Player : l_PlayerList)
                            {
                                if (l_Player->GetPower(POWER_MANA) > 0)
                                {
                                    DoCast(l_Player, eSpells::ManaDrain);
                                    l_Casted = true;
                                }
                            }

                            if (!l_Casted && !l_PlayerList.empty())
                                me->CastSpell(l_PlayerList.front(), eSpells::ManaDrain, false);
                        }

                        events.ScheduleEvent(eEvents::EventManaDrain, 15000);
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
            return new npc_moroes_baroness_dorothea_millstipeAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Lady Catriona Von'Indi - 114317
class npc_moroes_lady_catriona_von_indi : public CreatureScript
{
    public:
        npc_moroes_lady_catriona_von_indi() : CreatureScript("npc_moroes_lady_catriona_von_indi") { }

        struct npc_moroes_lady_catriona_von_indiAI : public ScriptedAI
        {
            npc_moroes_lady_catriona_von_indiAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DinnerParty     = 233669,
                DinnerPartyAT   = 233671,
                Smite           = 227542,
                HealingStream   = 227578
            };

            enum eEvents
            {
                EventSmite = 1,
                EventHealingStream
            };

            uint64 m_MoroesGUID;

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.4f;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_MoroesGUID = p_Summoner->GetGUID();
            }

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::DinnerParty, true);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoCast(me, eSpells::DinnerPartyAT, true);

                events.ScheduleEvent(eEvents::EventSmite, 1000);
                events.ScheduleEvent(eEvents::EventHealingStream, 20000);

                DoZoneInCombat(me, 100.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Moroes = me->FindNearestCreature(eCreatures::Moroes, 100.0f);
                if (!l_Moroes || !l_Moroes->IsAIEnabled)
                    return;

                l_Moroes->GetAI()->DoAction(ACTION_2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSmite:
                    {
                        DoCastVictim(eSpells::Smite);
                        events.ScheduleEvent(eEvents::EventSmite, 3000);
                        break;
                    }
                    case eEvents::EventHealingStream:
                    {
                        Creature* l_Creature = Creature::GetCreature(*me, m_MoroesGUID);
                        if (l_Creature && l_Creature->GetHealthPct() < 90.0f)
                        {
                            DoCast(eSpells::HealingStream);
                            events.ScheduleEvent(eEvents::EventHealingStream, 30000);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventHealingStream, 3000);

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
            return new npc_moroes_lady_catriona_von_indiAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Baron Rafe Dreuger - 114318
class npc_moroes_baron_rafe_dreuger : public CreatureScript
{
    public:
        npc_moroes_baron_rafe_dreuger() : CreatureScript("npc_moroes_baron_rafe_dreuger") { }

        struct npc_moroes_baron_rafe_dreugerAI : public ScriptedAI
        {
            npc_moroes_baron_rafe_dreugerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DinnerParty = 233669,
                DinnerPartyAT = 233671,
                IronWhirlwind = 227637
            };

            enum eEvents
            {
                EventIronWhirlwind = 1
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.4f;
            }

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::DinnerParty, true);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoCast(me, eSpells::DinnerPartyAT, true);

                events.ScheduleEvent(eEvents::EventIronWhirlwind, 5000);

                DoZoneInCombat(me, 100.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Moroes = me->FindNearestCreature(eCreatures::Moroes, 100.0f);
                if (!l_Moroes || !l_Moroes->IsAIEnabled)
                    return;

                l_Moroes->GetAI()->DoAction(ACTION_2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventIronWhirlwind:
                    {
                        DoCast(me, eSpells::IronWhirlwind);
                        events.ScheduleEvent(eEvents::EventIronWhirlwind, 10000);
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
            return new npc_moroes_baron_rafe_dreugerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Iron Whirlwind - 227637
class spell_rtk_iron_whirlwind : public SpellScriptLoader
{
    public:
        spell_rtk_iron_whirlwind() : SpellScriptLoader("spell_rtk_iron_whirlwind") { }

        class spell_rtk_iron_whirlwind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_iron_whirlwind_SpellScript);

            enum eSpells
            {
                IronWhirlwind = 227643
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::IronWhirlwind, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rtk_iron_whirlwind_SpellScript::HandleOnHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_iron_whirlwind_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_iron_whirlwind_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Lady Keira Berrybuck - 114319
class npc_moroes_lady_keira_berrybuck : public CreatureScript
{
    public:
        npc_moroes_lady_keira_berrybuck() : CreatureScript("npc_moroes_lady_keira_berrybuck") { }

        struct npc_moroes_lady_keira_berrybuckAI : public ScriptedAI
        {
            npc_moroes_lady_keira_berrybuckAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DinnerParty     = 233669,
                DinnerPartyAT   = 233671,
                EmpoweredArms   = 227616
            };

            enum eEvents
            {
                EventEmpoweredArms = 1
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.4f;
            }

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::DinnerParty, true);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoCast(me, eSpells::DinnerPartyAT, true);

                events.ScheduleEvent(eEvents::EventEmpoweredArms, 10000);

                DoZoneInCombat(me, 100.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Moroes = me->FindNearestCreature(eCreatures::Moroes, 100.0f);
                if (!l_Moroes || !l_Moroes->IsAIEnabled)
                    return;

                l_Moroes->GetAI()->DoAction(ACTION_2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventEmpoweredArms:
                    {
                        DoCast(eSpells::EmpoweredArms);
                        events.ScheduleEvent(eEvents::EventEmpoweredArms, 12000);
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
            return new npc_moroes_lady_keira_berrybuckAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Lord Robin Daris - 114320
class npc_moroes_lord_robin_daris : public CreatureScript
{
    public:
        npc_moroes_lord_robin_daris() : CreatureScript("npc_moroes_lord_robin_daris") { }

        struct npc_moroes_lord_robin_darisAI : public ScriptedAI
        {
            npc_moroes_lord_robin_darisAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DinnerParty     = 233669,
                DinnerPartyAT   = 233671,
                WhirlingEdge    = 227463
            };

            enum eEvents
            {
                EventWhirlingEdge = 1
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.2f;
            }

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::DinnerParty, true);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoCast(me, eSpells::DinnerPartyAT, true);

                events.ScheduleEvent(eEvents::EventWhirlingEdge, 1000);

                DoZoneInCombat(me, 100.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Moroes = me->FindNearestCreature(eCreatures::Moroes, 100.0f);
                if (!l_Moroes || !l_Moroes->IsAIEnabled)
                    return;

                l_Moroes->GetAI()->DoAction(ACTION_2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventWhirlingEdge:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                        {
                            if (Unit* l_Owner = me->GetAnyOwner())
                                me->CastSpell(l_Target, eSpells::WhirlingEdge, false, nullptr, nullptr, l_Owner->GetGUID());
                        }
                        events.ScheduleEvent(eEvents::EventWhirlingEdge, 18000);
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
            return new npc_moroes_lord_robin_darisAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Lord Crispin Ference - 114321
class npc_moroes_lord_crispin_ference : public CreatureScript
{
    public:
        npc_moroes_lord_crispin_ference() : CreatureScript("npc_moroes_lord_crispin_ference") { }

        struct npc_moroes_lord_crispin_ferenceAI : public ScriptedAI
        {
            npc_moroes_lord_crispin_ferenceAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DinnerParty     = 233669,
                DinnerPartyAT   = 233671,
                WillBreaker     = 227672
            };

            enum eEvents
            {
                EventWillBreaker = 1
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.4f;
            }

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::DinnerParty, true);
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoCast(me, eSpells::DinnerPartyAT, true);

                events.ScheduleEvent(eEvents::EventWillBreaker, 11000);

                DoZoneInCombat(me, 100.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Moroes = me->FindNearestCreature(eCreatures::Moroes, 100.0f);
                if (!l_Moroes || !l_Moroes->IsAIEnabled)
                    return;

                l_Moroes->GetAI()->DoAction(ACTION_2);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::WillBreaker:
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        }, 100);
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

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventWillBreaker:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                        {
                            me->SetFacingToObject(l_Target);
                            me->NearTeleportTo(*me);
                            me->CastSpell(l_Target, eSpells::WillBreaker, false);
                            me->AddDelayedEvent([this]() -> void
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            }, 50);
                        }
                        events.ScheduleEvent(eEvents::EventWillBreaker, 18000);
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
            return new npc_moroes_lord_crispin_ferenceAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Whirling Edge - 114327
class npc_moroes_whirling_edge : public CreatureScript
{
    public:
        npc_moroes_whirling_edge() : CreatureScript("npc_moroes_whirling_edge") { }

        struct npc_moroes_whirling_edgeAI : public Scripted_NoMovementAI
        {
            npc_moroes_whirling_edgeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                WhirlingEdgeAT = 227472
            };

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::WhirlingEdgeAT, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_moroes_whirling_edgeAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Dinner Party - 233676
class spell_rtk_dinner_party : public SpellScriptLoader
{
    public:
        spell_rtk_dinner_party() : SpellScriptLoader("spell_rtk_dinner_party") { }

        class spell_rtk_dinner_party_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_dinner_party_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                switch (l_Target->GetEntry())
                {
                    case eCreatures::LadyCatriona:
                    case eCreatures::LadyKeira:
                    case eCreatures::LordCrispin:
                    case eCreatures::LordRobin:
                    case eCreatures::BaronessDorothea:
                    case eCreatures::BaronRafe:
                        return SPELL_CAST_OK;
                    default:
                        return SPELL_FAILED_BAD_TARGETS;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rtk_dinner_party_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_dinner_party_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Medivh - 115426
class npc_moroes_medivh : public CreatureScript
{
    public:
        npc_moroes_medivh() : CreatureScript("npc_moroes_medivh") { }

        struct npc_moroes_medivhAI : public ScriptedAI
        {
            npc_moroes_medivhAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetWalk(true);
            }

            enum eAnimKit
            {
                Medivh1       = 5874,
                Medivh2       = 12168,
                Medivh3       = 9569
            };

            enum eSpells
            {
                Vision                  = 229485,
                UncontrollableEnergy    = 229571,
                UnleashedPower          = 232697,
                KnockbackSelf           = 66033,
                FeignDeath              = 145363
            };

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::Vision, true);

                events.ScheduleEvent(EVENT_2, 500);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::UnleashedPower && p_Target->GetEntry() == eCreatures::NielasAran)
                {
                    me->RemoveAurasDueToSpell(eSpells::UncontrollableEnergy);
                    me->RemoveAurasDueToSpell(eSpells::UnleashedPower);
                    p_Target->SetAIAnimKitId(0);
                    p_Target->CastSpell(p_Target, eSpells::FeignDeath, true);
                    p_Target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                    p_Target->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    DoCast(me, eSpells::KnockbackSelf, true);
                    me->SetAIAnimKitId(eAnimKit::Medivh2);
                    events.ScheduleEvent(EVENT_4, 3000);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == POINT_MOTION_TYPE && p_ID == 1)
                    events.ScheduleEvent(EVENT_6, 1000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_2:
                    {
                        me->SetAIAnimKitId(eAnimKit::Medivh1);
                        DoCast(me, eSpells::UncontrollableEnergy, true);
                        DelayTalk(13, 0);
                        events.ScheduleEvent(EVENT_3, 17000);
                        break;
                    }
                    case EVENT_3:
                    {
                        me->SetAIAnimKitId(0);
                        DoCast(eSpells::UnleashedPower);
                        break;
                    }
                    case EVENT_4:
                    {
                        DelayTalk(1, 1);
                        me->SetAIAnimKitId(0);
                        me->PlayOneShotAnimKitId(12312);
                        events.ScheduleEvent(EVENT_5, 3500);
                        break;
                    }
                    case EVENT_5:
                    {
                        me->GetMotionMaster()->MovePoint(1, -10983.81f, -1882.08f, 81.72f);
                        break;
                    }
                    case EVENT_6:
                    {
                        me->SetAIAnimKitId(eAnimKit::Medivh3);
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_moroes_medivhAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Nielas Aran - 115427
class npc_moroes_nielas_aran : public CreatureScript
{
    public:
        npc_moroes_nielas_aran() : CreatureScript("npc_moroes_nielas_aran") { }

        struct npc_moroes_nielas_aranAI : public ScriptedAI
        {
            npc_moroes_nielas_aranAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetWalk(true);
            }

            enum eSpells
            {
                Vision      = 229485,
                PlayScene   = 232486,
                FeignDeath  = 145363
            };

            enum eAnimKit
            {
                NielasAran    = 12310
            };

            void Reset() override
            {
                events.Reset();
                DoCast(me, eSpells::Vision, true);

                events.ScheduleEvent(EVENT_1, 500);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::PlayScene)
                    p_Target->CastSpell(p_Target, 232480, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->SetAIAnimKitId(eAnimKit::NielasAran);
                        DoCast(me, eSpells::PlayScene, true);
                        DelayTalk(9, 0);
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_moroes_nielas_aranAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_moroes_legion()
{
    /// Boss
    new boss_rtk_moroes();

    /// Creatures
    new npc_moroes_baroness_dorothea_millstipe();
    new npc_moroes_lady_catriona_von_indi();
    new npc_moroes_baron_rafe_dreuger();
    new npc_moroes_lady_keira_berrybuck();
    new npc_moroes_lord_robin_daris();
    new npc_moroes_lord_crispin_ference();

    new npc_moroes_medivh();
    new npc_moroes_nielas_aran();

    /// Spells
    new npc_moroes_whirling_edge();
    new spell_rtk_dinner_party();
    new spell_rtk_iron_whirlwind();
    new spell_rtk_vanish();
}
#endif
