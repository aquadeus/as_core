////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

Position g_CenterPos = { -11124.77f, -1925.23f, 0.0f, 5.63f };

/// Last Update 7.1.5 Build 23420
/// Nightbane - 114895
class boss_rtk_nightbane : public CreatureScript
{
    public:
        boss_rtk_nightbane() : CreatureScript("boss_rtk_nightbane") { }

        struct boss_rtk_nightbaneAI : public BossAI
        {
            boss_rtk_nightbaneAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataNightbane) { }

            enum eSpells
            {
                CharredEarth                = 228862,
                CinderBreath                = 228785,
                TailSwipe                   = 228787,
                IgniteSoul                  = 228796,
                BurningBones                = 228800,
                ReverberatingShadow         = 229307,
                InfernalPower               = 228792,
                ConcentratedPower           = 228790,

                RainOfBones                 = 228839,
                JaggedShard                 = 228834,
                SummonRestlessBones         = 228830,
                BellowingRoar               = 228837,

                SmolderingEmberWyrmSpell    = 231428
            };

            enum eEvents
            {
                EventCharredEarth = 1,
                EventTailSwipe,
                EventCinderBreath,
                EventIgniteSoul,
                EventBurningBones,
                EventReverberatingShadow,
                EventPower,

                IntroSecondPhase,
                EventJaggedShard,
                EventSummonRestlessBones,

                IntroThirdPhase,
                EventBellowingRoar
            };

            enum ePhases
            {
                PhaseNone,
                Phase1,
                Phase2,
                Phase3
            };

            enum eTalks
            {
                Aggro,
                TalkIgniteSoul,
                TakeOff,
                Landing,
                PlayerKilled,
                TalkBellowingRoar,
                Death
            };

            enum eItems
            {
                SmolderingEmberWyrm = 142552
            };

            Position m_LandPosition;
            bool m_IsTransitioningToThirdPhase = false;
            ePhases m_Phase = ePhases::PhaseNone;
            std::set<uint64> m_NotLootedPlayers;

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->ForcedDespawn();

                m_IsTransitioningToThirdPhase = false;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::DataNightbane, EncounterState::FAIL);
            }

            void Reset() override
            {
                _Reset();

                m_LandPosition = me->GetHomePosition();
                m_IsTransitioningToThirdPhase = false;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();

                m_Phase = ePhases::Phase1;
                DefaultEvents();
                Talk(eTalks::Aggro);

                m_NotLootedPlayers.clear();

                InstanceScript* l_Instance = me->GetInstanceScript();
                if (!l_Instance || !l_Instance->instance)
                    return;

                for (Map::PlayerList::const_iterator itr = l_Instance->instance->GetPlayers().begin(); itr != l_Instance->instance->GetPlayers().end(); ++itr)
                {
                    if (Player* l_Player = itr->getSource())
                    {
                        if (!l_Player->BossAlreadyLooted(me))
                            m_NotLootedPlayers.insert(l_Player->GetGUID());
                    }
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetHealthPct() < 50.0f && m_Phase == ePhases::Phase1)
                {
                    m_Phase = ePhases::Phase2;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Talk(eTalks::TakeOff);
                    DefaultEvents();
                }
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void DefaultEvents()
            {
                events.Reset();

                switch (m_Phase)
                {
                    case ePhases::Phase3:
                    {
                        events.ScheduleEvent(eEvents::EventBellowingRoar, 18 * IN_MILLISECONDS);
                        /// No Break intended
                    }
                    case ePhases::Phase1:
                    {
                        events.ScheduleEvent(eEvents::EventCharredEarth, 16 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventTailSwipe, 12 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventCinderBreath, 9 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventIgniteSoul, 21 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventBurningBones, 25 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventReverberatingShadow, 17 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventPower, 20 * IN_MILLISECONDS);
                        break;
                    }
                    case ePhases::Phase2:
                    {
                        events.ScheduleEvent(eEvents::EventCharredEarth, 12 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::IntroSecondPhase, 0);
                        events.ScheduleEvent(eEvents::EventJaggedShard, 9 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventSummonRestlessBones, 15 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);

                _JustDied();

                std::vector<Player*> l_LootingPlayers;
                for (uint64 l_PlayerGUID : m_NotLootedPlayers)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, l_PlayerGUID))
                        l_LootingPlayers.push_back(l_Player);
                }

                if (!roll_chance_i(20 * l_LootingPlayers.size()))
                    return;

                std::vector<Player*> l_EligiblePlayers;
                for (Player* l_Player : l_LootingPlayers)
                {
                    if (l_Player->HasSpell(eSpells::SmolderingEmberWyrmSpell) || l_Player->HasItemCount(eItems::SmolderingEmberWyrm))
                        continue;

                    l_EligiblePlayers.push_back(l_Player);
                }

                if (l_EligiblePlayers.empty())
                    return;

                Player* l_Looter = l_EligiblePlayers[urand(0, l_EligiblePlayers.size() - 1)];

                l_Looter->AddItem(eItems::SmolderingEmberWyrm, 1, {}, false, 0, false, true);
                l_Looter->SendDisplayToast(eItems::SmolderingEmberWyrm, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 11:
                    {
                        m_IsTransitioningToThirdPhase = false;
                        SetFlyMode(false);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetInCombatWithZone();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                        Talk(eTalks::Landing);
                        DefaultEvents();
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        m_Phase = ePhases::Phase3;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        events.ScheduleEvent(eEvents::IntroThirdPhase, 0);
                        break;
                    }
                    case ACTION_2:
                    {
                        EnterEvadeMode();
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Position l_TargetPos = *me;
                        l_TargetPos.m_positionZ -= 18.0f;
                        me->NearTeleportTo(l_TargetPos);
                        SetFlyMode(false);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->GetMotionMaster()->Clear();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if ((m_Phase != ePhases::Phase2 && !m_IsTransitioningToThirdPhase) && !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!m_IsTransitioningToThirdPhase && (me->GetPositionZ() < 91.0f || me->GetExactDist2d(&g_CenterPos) < 26.5f || me->GetExactDist2d(&g_CenterPos) > 48.0f || g_CenterPos.HasInArc(0.65f, me)))
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCharredEarth:
                    {
                        DoCast(eSpells::CharredEarth);
                        events.ScheduleEvent(eEvents::EventCharredEarth, m_Phase == ePhases::Phase2 ? 12 * IN_MILLISECONDS : 20 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCinderBreath:
                    {
                        DoCastVictim(eSpells::CinderBreath);
                        events.ScheduleEvent(eEvents::EventCinderBreath, 23 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTailSwipe:
                    {
                        DoCast(eSpells::TailSwipe);
                        events.ScheduleEvent(eEvents::EventTailSwipe, 40 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventIgniteSoul:
                    {
                        if (Unit* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                        {
                            Talk(eTalks::TalkIgniteSoul);
                            me->CastSpell(l_Target, eSpells::IgniteSoul, false);
                        }

                        events.ScheduleEvent(eEvents::EventIgniteSoul, 25 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBurningBones:
                    {
                        if (Unit* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                        {
                            for (uint32 l_I = 0; l_I < 5; ++l_I)
                                me->CastSpell(l_Target, eSpells::BurningBones, static_cast<bool>(l_I));
                        }

                        events.ScheduleEvent(eEvents::EventBurningBones, 20 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventReverberatingShadow:
                    {
                        DoCast(eSpells::ReverberatingShadow);
                        events.ScheduleEvent(eEvents::EventReverberatingShadow, 12 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventPower:
                    {
                        if (urand(0, 1))
                            DoCast(eSpells::InfernalPower);
                        else
                            DoCast(eSpells::ConcentratedPower);

                        events.ScheduleEvent(eEvents::EventTailSwipe, 40 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::IntroSecondPhase:
                    {
                        Position l_TargetPos = *me;
                        l_TargetPos.m_positionZ += 18.0f;
                        SetFlyMode(true);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(10, l_TargetPos);
                        DoCast(eSpells::RainOfBones);
                        break;
                    }
                    case eEvents::EventJaggedShard:
                    {
                        DoCast(eSpells::JaggedShard);
                        events.ScheduleEvent(eEvents::EventJaggedShard, 8 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSummonRestlessBones:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f))
                            l_Target->CastSpell(l_Target, eSpells::SummonRestlessBones, true, nullptr, nullptr, me->GetGUID());

                        events.ScheduleEvent(eEvents::EventSummonRestlessBones, 9 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::IntroThirdPhase:
                    {
                        m_IsTransitioningToThirdPhase = true;
                        me->SetReactState(REACT_PASSIVE);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(11, m_LandPosition);
                        break;
                    }
                    case eEvents::EventBellowingRoar:
                    {
                        DoCast(eSpells::BellowingRoar);
                        Talk(eTalks::TalkBellowingRoar);
                        events.ScheduleEvent(eEvents::EventBellowingRoar, 45 * IN_MILLISECONDS);
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
            return new boss_rtk_nightbaneAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Charred Earth - 228862
class spell_rtk_charred_earth : public SpellScriptLoader
{
    public:
        spell_rtk_charred_earth() : SpellScriptLoader("spell_rtk_charred_earth") { }

        class spell_rtk_charred_earth_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_charred_earth_SpellScript);

            enum eSpells
            {
                CharredEarth = 228806
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<WorldObject*> l_Targets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Player* l_Player = l_Target->ToPlayer();
                    if (!l_Player || (l_Player->GetRoleForGroup() == Roles::ROLE_TANK))
                        continue;

                    l_Targets.push_back(l_Player);
                }

                if (l_Targets.empty())
                {
                    p_Targets = l_Targets;
                    return;
                }

                l_Targets.sort(JadeCore::ObjectDistanceOrderPredPlayer(l_Caster, false));

                p_Targets = l_Targets;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CharredEarth, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rtk_charred_earth_SpellScript::HandleOnHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_charred_earth_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_charred_earth_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ignite Soul - 228796
class spell_rtk_ignite_soul : public SpellScriptLoader
{
    public:
        spell_rtk_ignite_soul() : SpellScriptLoader("spell_rtk_ignite_soul") { }

        class spell_rtk_ignite_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_ignite_soul_AuraScript);

            enum eSpells
            {
                IgniteSoul = 228800
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                int32 l_Damage = l_Target->GetHealth();
                l_Target->CastCustomSpell(l_Target, eSpells::IgniteSoul, &l_Damage, nullptr, nullptr, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_ignite_soul_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_ignite_soul_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ignite Soul - 228800
class spell_rtk_ignite_soul_damage : public SpellScriptLoader
{
    public:
        spell_rtk_ignite_soul_damage() : SpellScriptLoader("spell_rtk_ignite_soul_damage") { }

        class spell_rtk_ignite_soul_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_ignite_soul_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end(); ++l_Itr)
                {
                    Player* l_Player = (*l_Itr)->ToPlayer();
                    if (!l_Player || (l_Player->GetGUID() != l_Caster->GetGUID()))
                        continue;

                    p_Targets.erase(l_Itr);
                    break;
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_ignite_soul_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_ignite_soul_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ignite Soul - 228829
class spell_rtk_burning_bones : public SpellScriptLoader
{
    public:
        spell_rtk_burning_bones() : SpellScriptLoader("spell_rtk_burning_bones") { }

        class spell_rtk_burning_bones_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_burning_bones_AuraScript);

            enum eSpells
            {
                BurningBones = 228830
            };

            bool l_DropStack = false;

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_BaseAura = p_AuraEffect->GetBase();
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_BaseAura || !l_Caster || !l_Target)
                    return;

                for (uint32 l_I = 0; l_I < l_BaseAura->GetStackAmount(); ++l_I)
                {
                    l_Target->CastSpell(l_Target, eSpells::BurningBones, true, nullptr, nullptr, l_Caster->GetGUID());
                    return;
                }

                l_Target->CastSpell(l_Target, eSpells::BurningBones, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void HandleAfterPeriodic(AuraEffect const* p_AuraEffect)
            {
                if (!l_DropStack)
                {
                    l_DropStack = true;
                    return;
                }

                Aura* l_BaseAura = p_AuraEffect->GetBase();
                if (!l_BaseAura)
                    return;

                l_BaseAura->DropStack();
                l_DropStack = false;
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_burning_bones_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_rtk_burning_bones_AuraScript::HandleAfterPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_burning_bones_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Bonecurse - 114906
class npc_rtk_bonecurse : public CreatureScript
{
    public:
        npc_rtk_bonecurse() : CreatureScript("npc_rtk_bonecurse") { }

        struct npc_rtk_bonecurseAI : public LegionCombatAI
        {
            npc_rtk_bonecurseAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            uint64 m_NightBane;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_NightBane = p_Summoner->GetGUID();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Nightbane = Creature::GetCreature(*me, m_NightBane);
                if (!l_Nightbane || !l_Nightbane->IsAIEnabled)
                    return;

                l_Nightbane->GetAI()->DoAction(ACTION_1);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                LegionCombatAI::UpdateAI(p_Diff);

                Creature* l_Nightbane = Creature::GetCreature(*me, m_NightBane);
                if (!l_Nightbane || !l_Nightbane->IsAIEnabled)
                    return;

                if (me->GetPositionZ() < 91.0f || me->GetExactDist2d(&g_CenterPos) < 26.5f || me->GetExactDist2d(&g_CenterPos) > 48.0f || g_CenterPos.HasInArc(0.65f, me))
                {
                    l_Nightbane->GetAI()->DoAction(ACTION_2);
                    return;
                }

                if (UpdateVictim())
                    return;

                l_Nightbane->GetAI()->DoAction(ACTION_2);
                me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_bonecurseAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Medivh - 115038
class npc_rtk_nightbane_medivh : public CreatureScript
{
    public:
        npc_rtk_nightbane_medivh() : CreatureScript("npc_rtk_nightbane_medivh") { }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            if (!p_Creature || !p_Creature->IsAIEnabled)
                return false;

            p_Creature->GetAI()->DoAction(ACTION_1);

            return true;
        }

        struct npc_rtk_nightbane_medivhAI : public ScriptedAI
        {
            npc_rtk_nightbane_medivhAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Vision = 229485
            };

            enum eAchievements
            {
                OneNightInKarazhan = 11430
            };

            bool m_EventStarted;

            void Reset() override
            {
                me->AddAura(eSpells::Vision, me);
                m_EventStarted = false;
            }

            void DoAction(int32 const p_Action) override
            {
                if (m_EventStarted)
                    return;

                m_EventStarted = true;

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->DoCompleteAchievement(eAchievements::OneNightInKarazhan);

                Position const l_ArcanagosSpawn = { -11196.16f, -1836.97f, 97.18f, 5.38f };
                Creature* l_Creature = me->SummonCreature(eCreatures::ImageOfArcanagos, l_ArcanagosSpawn);
                if (!l_Creature)
                    return;

                DelayTalk(23, 0);
                DelayTalk(45, 1);
                DelayTalk(71, 2);
                DelayTalk(93, 3);
                DelayTalk(105, 4);

                AddTimedDelayedOperation(108 * IN_MILLISECONDS, [this]() -> void
                {
                    Position const l_Dest = { -11116.49f, -1886.35f, 91.48f };
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, l_Dest);
                });

                AddTimedDelayedOperation(123 * IN_MILLISECONDS, [this]() -> void
                {
                    me->DespawnOrUnsummon();
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_nightbane_medivhAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Image Of Arcanagos - 115213
class npc_rtk_image_of_arcanagos : public CreatureScript
{
    public:
        npc_rtk_image_of_arcanagos() : CreatureScript("npc_rtk_image_of_arcanagos") { }

        struct npc_rtk_image_of_arcanagosAI : public ScriptedAI
        {
            npc_rtk_image_of_arcanagosAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                SetFlyMode(true);
                me->SetDisableGravity(true);
            }

            void Reset() override
            {
                Position const l_Dest = { -11167.96f, -1872.41f, 97.18f };
                me->GetMotionMaster()->MovePoint(0, l_Dest);
                Talk(0);
                DelayTalk(33, 1);
                DelayTalk(57, 2);
                DelayTalk(83, 3);
                DelayTalk(112, 4);

                AddTimedDelayedOperation(119 * IN_MILLISECONDS, [this]() -> void
                {
                    Position const l_Dest = { -11153.50f, -1891.30f, 91.56f };
                    me->GetMotionMaster()->MovePoint(0, l_Dest);
                });

                AddTimedDelayedOperation(120500, [this]() -> void
                {
                    SetFlyMode(false);
                    me->SetDisableGravity(false);
                });

                AddTimedDelayedOperation(123 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SummonCreature(eCreatures::Nightbane, *me);
                    me->DespawnOrUnsummon(0);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_image_of_arcanagosAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_rtk_nightbane()
{
    /// Boss
    new boss_rtk_nightbane();

    /// Spells
    new spell_rtk_charred_earth();
    new spell_rtk_ignite_soul();
    new spell_rtk_ignite_soul_damage();
    new spell_rtk_burning_bones();

    /// Add
    new npc_rtk_bonecurse();

    /// Event
    new npc_rtk_nightbane_medivh();
    new npc_rtk_image_of_arcanagos();
}
#endif
