////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "black_rook_hold_dungeon.hpp"

enum Says
{
    SAY_AGGRO   = 0,
    SAY_SCYTHE  = 1,
    SAY_SOUL    = 2,
    SAY_REAP    = 3,
    SAY_DEATH   = 4,
    SAY_CALL    = 5,
    SAY_ENERGY  = 6,
};

enum eEvents
{
    EventSwirlingScythe   = 1,
    EventSoulEchoes       = 2,
    EventReapSouls        = 3,
    EventMoveCenterPos    = 4,
    EventCallSouls        = 5,
    EventCheckSouls       = 6,
    EventRemoveSouls      = 7
};

enum eAnim ///< SMSG_PLAY_ONE_SHOT_ANIM_KIT
{
    STALKER_ANIM_1 = 9038,
    STALKER_ANIM_2 = 9039,
    STALKER_ANIM_3 = 9176
};

uint32 g_AnimRand[3] =
{
    STALKER_ANIM_1,
    STALKER_ANIM_2,
    STALKER_ANIM_3
};

Position const g_CenterPos = { 3251.35f, 7582.79f, 12.75f, 0.0f };

Position const g_SoulsPos[7] =
{
    { 3288.37f, 7593.30f, 14.10f, 3.45f },
    { 3259.17f, 7620.32f, 14.10f, 4.53f },
    { 3239.09f, 7618.44f, 14.10f, 5.05f },
    { 3213.97f, 7587.99f, 14.10f, 6.12f },
    { 3215.85f, 7567.98f, 14.10f, 0.36f },
    { 3245.75f, 7541.42f, 14.10f, 1.41f },
    { 3265.50f, 7543.86f, 14.10f, 1.90f }
};

/// Amalgam of Souls - 98542
class boss_the_amalgam_of_souls : public CreatureScript
{
    public:
        boss_the_amalgam_of_souls() : CreatureScript("boss_the_amalgam_of_souls")  { }

        struct boss_the_amalgam_of_soulsAI : public BossAI
        {
            boss_the_amalgam_of_soulsAI(Creature* p_Creature) : BossAI(p_Creature, eData::Amalgam)
            {
                if (me->isAlive())
                    instance->SetData(eData::AmalgamOutro, IN_PROGRESS);
            }

            enum eAchievements
            {
                BlackRookMoan = 10710
            };

            enum eSpells
            {
                SwirlingScythe          = 195254,
                SoulEchoes              = 194966,
                ReapSoul                = 194956,

                /// Heroic
                CallSouls               = 196078,
                CallSoulsAreatrigger    = 196925,
                SoulBurst               = 196587,
                SoulGorge               = 196930,

                /// Other
                SummonEtheldrinRavencrest = 196619,
                SummonVelandrasRavencrest = 196620,
                SummonStaellisRivermoor   = 196646,
            };

            bool m_HeroicEvent;
            bool m_IsChannelingSouls;
            bool m_AchievementReady;

            void Reset() override
            {
                _Reset();
                me->RemoveAurasDueToSpell(CallSoulsAreatrigger);
                me->SetReactState(REACT_AGGRESSIVE);
                m_HeroicEvent = false;
                m_IsChannelingSouls = false;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                DefaultEvents();
                m_AchievementReady = false;
            }

            void DefaultEvents()
            {
                events.ScheduleEvent(eEvents::EventSwirlingScythe, 8000);
                events.ScheduleEvent(eEvents::EventSoulEchoes, 14000);
                events.ScheduleEvent(eEvents::EventReapSouls, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                DoCast(me, eSpells::SummonStaellisRivermoor, true);
                DoCast(me, eSpells::SummonVelandrasRavencrest, true);
                DoCast(me, eSpells::SummonEtheldrinRavencrest, true);

                if (m_AchievementReady && instance)
                    instance->DoCompleteAchievement(eAchievements::BlackRookMoan);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(50) && !m_HeroicEvent)
                {
                    m_HeroicEvent = true;
                    m_IsChannelingSouls = true;
                    me->SetReactState(REACT_PASSIVE);

                    if (IsHeroicOrMythic())
                    {
                        DoStopAttack();

                        events.Reset();
                        events.ScheduleEvent(eEvents::EventMoveCenterPos, 500);
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE || p_ID != 1)
                    return;

                events.ScheduleEvent(EventCallSouls, 1000);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == CallSouls)
                {
                    DoCast(me, CallSoulsAreatrigger, true);

                    for (uint8 l_I = 0; l_I < 7; l_I++)
                        me->SummonCreature(eCreatures::RestlessSoul, g_SoulsPos[l_I]);

                    events.ScheduleEvent(eEvents::EventCheckSouls, 2000);
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SwirlingScythe:
                    case eSpells::ReapSoul:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });
                    }
                        break;
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                m_AchievementReady = true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 19.0f)
                    EnterEvadeMode();

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSwirlingScythe:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                        {
                            me->SetFacingToObject(l_Target);
                            Talk(SAY_SCYTHE);
                            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            DoCast(l_Target, eSpells::SwirlingScythe);
                        }
                        events.ScheduleEvent(eEvents::EventSwirlingScythe, 20000);
                        break;
                    }
                    case eEvents::EventSoulEchoes:
                    {
                        Talk(SAY_SOUL);
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(l_Target, eSpells::SoulEchoes);
                        events.ScheduleEvent(eEvents::EventSoulEchoes, 28000);
                        break;
                    }
                    case eEvents::EventReapSouls:
                    {
                        Talk(SAY_REAP);
                        me->SetFacingToObject(me->getVictim());
                        me->NearTeleportTo(*me);

                        DoCastVictim(eSpells::ReapSoul);
                        AddTimedDelayedOperation(50, [this]() -> void
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });

                        events.ScheduleEvent(eEvents::EventReapSouls, 13000);
                        break;
                    }
                    case eEvents::EventMoveCenterPos:
                    {
                        me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                        break;
                    }
                    case eEvents::EventCallSouls:
                    {
                        Talk(SAY_CALL);
                        DoCast(CallSouls);
                        break;
                    }
                    case eEvents::EventCheckSouls:
                    {
                        if (Creature* l_Soul = me->FindNearestCreature(eCreatures::RestlessSoul, 100.0f))
                            events.ScheduleEvent(eEvents::EventCheckSouls, 2000);
                        else
                        {
                            Talk(Says::SAY_ENERGY);
                            me->RemoveAurasDueToSpell(CallSoulsAreatrigger);
                            DoCast(SoulBurst);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DefaultEvents();
                            events.ScheduleEvent(eEvents::EventRemoveSouls, 6000);
                            m_IsChannelingSouls = false;
                        }
                        break;
                    }
                    case eEvents::EventRemoveSouls:
                    {
                        me->RemoveAurasDueToSpell(SoulGorge);
                        break;
                    }
                    default:
                        break;
                }

                if (!m_IsChannelingSouls)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_the_amalgam_of_soulsAI(p_Creature);
        }
};

/// Staellis Rivermoor - 99426
/// Lord Etheldrin Ravencrest - 99857
/// Lady Velandras Ravencrest - 99585
class npc_soul_echoes_outro : public CreatureScript
{
    public:
        npc_soul_echoes_outro() : CreatureScript("npc_soul_echoes_outro") { }

        struct npc_soul_echoes_outroAI : public ScriptedAI
        {
            npc_soul_echoes_outroAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                if ((m_Instance = me->GetInstanceScript()) != nullptr)
                    m_Instance->SetData(eData::AmalgamOutro, IN_PROGRESS);

                m_Outro = false;
            }

            enum eSpells
            {
                ConversationOpenDoor = 197075,

                SecretDoorChannelRight = 205212,
                SecretDoorChannelLeft = 205210,
                SecretDoorChannelMiddle = 205211
            };

            InstanceScript* m_Instance;
            bool m_Outro;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_3, 10);
                me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE || !m_Instance)
                    return;

                switch (p_ID)
                {
                    case 1:
                    {
                        m_Instance->SetData(eData::AmalgamOutro, 0);
                        Talk(0);
                        break;
                    }
                    case 2:
                    {
                        m_Instance->SetData(eData::AmalgamOutro, 0);
                        break;
                    }
                    case 3:
                    {
                        m_Instance->SetData(eData::AmalgamOutro, 0);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (m_Instance && m_Instance->GetData(eData::AmalgamOutro) >= 3 && !m_Outro)
                {
                    m_Outro = true;

                    if (me->GetEntry() == eCreatures::LordEtheldrinRavencrest)
                        me->CastSpell(me, eSpells::SecretDoorChannelRight);
                    else if (me->GetEntry() == eCreatures::StaellisRivermoor)
                        me->CastSpell(me, eSpells::SecretDoorChannelLeft);
                    else if (me->GetEntry() == eCreatures::LadyVelandrasRavencrest)
                    {
                        std::list<Creature*> l_List;
                        me->GetCreatureListWithEntryInGrid(l_List, eCreatures::SecretDoorStalker, 30.0f);

                        Creature* l_FoundCreature = nullptr;
                        for (Creature* l_Iter : l_List)
                        {
                            if (!l_FoundCreature)
                            {
                                l_FoundCreature = l_Iter;
                                continue;
                            }

                            if (l_FoundCreature->m_positionZ < l_Iter->m_positionZ)
                                l_FoundCreature = l_Iter;
                        }

                        me->CastSpell(l_FoundCreature, eSpells::SecretDoorChannelMiddle, true);
                    }

                    events.ScheduleEvent(EVENT_2, 5000);
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, 0);
                        if (me->GetEntry() == 99857)
                            me->GetMotionMaster()->MovePoint(1, 3226.46f, 7553.97f, 15.26f);
                        if (me->GetEntry() == 99858)
                            me->GetMotionMaster()->MovePoint(2, 3228.56f, 7548.91f, 14.85f);
                        if (me->GetEntry() == 99426)
                            me->GetMotionMaster()->MovePoint(3, 3233.58f, 7548.09f, 15.04f);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (!m_Instance)
                            return;

                        me->CastStop();
                        m_Instance->SetData(eData::AmalgamOutro, DONE);

                        uint64 l_DoorGuid = m_Instance->GetData64(eGameObjects::AmalgamExitDoor);
                        GameObject* l_Gob = GameObject::GetGameObject(*me, l_DoorGuid);
                        if (l_Gob)
                            l_Gob->SetGoState(GO_STATE_ACTIVE);

                        me->DespawnOrUnsummon(2000);
                        break;
                    }
                    case EVENT_3:
                    {
                        if (me->GetEntry() == 99857)
                            DoCast(eSpells::ConversationOpenDoor);
                        events.ScheduleEvent(EVENT_1, 7000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_soul_echoes_outroAI(p_Creature);
        }
};

/// Restless Soul - 99664
class npc_amalgam_restless_soul : public CreatureScript
{
    public:
        npc_amalgam_restless_soul() : CreatureScript("npc_amalgam_restless_soul") { }

        struct npc_amalgam_restless_soulAI : public ScriptedAI
        {
            npc_amalgam_restless_soulAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_FLIGHT, 0.3f);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            enum eSpells
            {
                SoulGorge = 196930
            };

            uint16 m_MoveTimer;
            int32 m_TransformTimer;
            uint64 m_Amalgam;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_Amalgam = p_Summoner->GetGUID();
                m_MoveTimer = 1000;
                m_TransformTimer = 60000;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->GetEntry() == eCreatures::FrustratedSoul)
                {
                    Creature* l_Amalgam = Creature::GetCreature(*me, m_Amalgam);
                    if (l_Amalgam && l_Amalgam->IsAIEnabled)
                        l_Amalgam->GetAI()->DoAction(0);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_TransformTimer -= p_Diff;
                if (m_TransformTimer < 0)
                    me->UpdateEntry(eCreatures::FrustratedSoul);

                if (m_MoveTimer)
                {
                    if (m_MoveTimer <= p_Diff)
                    {
                        m_MoveTimer = 1000;

                        if (me->GetDistance(g_CenterPos) < 4.0f)
                        {
                            m_MoveTimer = 0;
                            me->DespawnOrUnsummon(4000);
                            DoCast(me, eSpells::SoulGorge, true);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                            me->SetVisible(false);
                        }
                        else
                        {
                            if (!me->HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT))
                                me->GetMotionMaster()->MovePoint(1, g_CenterPos);
                        }
                    }
                    else
                        m_MoveTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_amalgam_restless_soulAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Ghostly Councilor - 98370
class npc_ghostly_councilor : public CreatureScript
{
    public:
        npc_ghostly_councilor() : CreatureScript("npc_ghostly_councilor") { }

        struct npc_ghostly_councilorAI : public LegionCombatAI
        {
            npc_ghostly_councilorAI(Creature* p_Creature) : LegionCombatAI(p_Creature) {}

            enum eEvents
            {
                EventDarkMending = 2
            };

            bool AdditionalChecks(uint32 p_EventID) override
            {
                if (p_EventID == eEvents::EventDarkMending)
                    return me->GetMap()->IsMythic();

                return true;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ghostly_councilorAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Sacrifice Soul - 200105
class spell_brh_sacrifice_soul: public SpellScriptLoader
{
    public:
        spell_brh_sacrifice_soul() : SpellScriptLoader("spell_brh_sacrifice_soul") { }

        class spell_brh_sacrifice_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brh_sacrifice_soul_AuraScript);

            enum eSpells
            {
                SacrificeSoulBuff = 200099
            };

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SacrificeSoulBuff, true);
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Creature*> l_Creatures;
                l_Caster->GetCreatureListInGrid(l_Creatures, 20.0f);
                for (Creature* l_Creature : l_Creatures)
                    l_Creature->RemoveAurasDueToSpell(eSpells::SacrificeSoulBuff, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_brh_sacrifice_soul_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_brh_sacrifice_soul_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brh_sacrifice_soul_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Lady Velandras Ravencrest - 98538
class npc_lady_velandras_ravencrest : public CreatureScript
{
    public:
        npc_lady_velandras_ravencrest() : CreatureScript("npc_lady_velandras_ravencrest") { }

        struct npc_lady_velandras_ravencrestAI : public LegionCombatAI
        {
            npc_lady_velandras_ravencrestAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            void Reset() override
            {
                LegionCombatAI::Reset();

                AddTimedDelayedOperation(1000, [this]() -> void
                {
                    std::list<Creature*> l_CreatureList;
                    GetCreatureListWithEntryInGrid(l_CreatureList, me, eCreatures::GhostlyProtector, 15.0f);
                    GetCreatureListWithEntryInGrid(l_CreatureList, me, eCreatures::GhostlyRetainer, 15.0f);
                    GetCreatureListWithEntryInGrid(l_CreatureList, me, eCreatures::GhostlyCouncilor, 15.0f);

                    for (Creature* l_Creature : l_CreatureList)
                        l_Creature->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lady_velandras_ravencrestAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Soul Echoes Stalker -99090
class npc_soul_echoes_stalker : public CreatureScript
{
    public:
        npc_soul_echoes_stalker() : CreatureScript("npc_soul_echoes_stalker") { }

        struct npc_soul_echoes_stalkerAI : public ScriptedAI
        {
            npc_soul_echoes_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                SoulEchoesCloneCaster = 194981,
                SoulEchoesDetonate = 194960
            };

            int32 m_DetonateTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                p_Summoner->CastSpell(me, eSpells::SoulEchoesCloneCaster, true);
                me->PlayOneShotAnimKitId(g_AnimRand[urand(0, 2)]);
                m_DetonateTimer = 4 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_DetonateTimer -= p_Diff;
                if (m_DetonateTimer < 0)
                {
                    me->CastSpell(me, eSpells::SoulEchoesDetonate, true);
                    me->DespawnOrUnsummon(1);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_soul_echoes_stalkerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Soul Echoes Periodic - 194966
class spell_brh_soul_echoes_periodic: public SpellScriptLoader
{
    public:
        spell_brh_soul_echoes_periodic() : SpellScriptLoader("spell_brh_soul_echoes_periodic") { }

        class spell_brh_soul_echoes_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brh_soul_echoes_periodic_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_SpellInfo)
                    return;

                l_Target->CastSpell(l_Target, l_SpellInfo->Effects[EFFECT_0].TriggerSpell, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_brh_soul_echoes_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brh_soul_echoes_periodic_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_the_amalgam_of_souls()
{
    /// Boss
    new boss_the_amalgam_of_souls();

    /// MiniBosses
    new npc_lady_velandras_ravencrest();

    new npc_soul_echoes_stalker();
    new spell_brh_soul_echoes_periodic();

    /// Creatures
    new npc_soul_echoes_outro();
    new npc_amalgam_restless_soul();

    /// Trashes
    new npc_ghostly_councilor();

    new spell_brh_sacrifice_soul();
}
#endif
