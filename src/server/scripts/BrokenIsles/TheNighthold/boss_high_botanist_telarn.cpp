////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eSpells
{
    SpellControlledChaos        = 218438,
    SpellControlledChaosSummon  = 218447,
    SpellControlledChaos10Yd    = 218463,
    SpellControlledChaos20Yd    = 218466,
    SpellControlledChaos30Yd    = 218470,

    SpellChaoticNature10Yd      = 223386,
    SpellChaoticNature20Yd      = 223387,
    SpellChaoticNature30Yd      = 223389,

    SpellSolarCollapse          = 218148,
    SpellSolarCollapseMissile   = 223734,
    SpellSolarCollapseMissileFast   = 218152,

    SpellRecursiveStrikes       = 218502,
    SpellRecursiveStrikesPlayer = 218503,
    SpellParasiticFetterAoe     = 218424,
    SpellParasiticFetterDot     = 218304,
    SpellParasiticFetterRoot    = 219340,

    SpellNightosisSolarist      = 216830,
    SpellNightosisNaturalist    = 216877,
    SpellNightosisModelArcanist = 216878,
    SpellNightosisModelBotanist = 216832,

    SpellCallOfNightAoe         = 218807,
    SpellCallOfNightAura        = 218809,
    SpellArcaneEclipse          = 218838,
    SpellChaosSpheres           = 223034,

    /// Parasitic Slasher
    SpellParasiticFixate        = 218342,
    SpellParasiticFetterAt      = 218357,
    SpellRampantGrowth          = 219248,
    SpellOverGrowth             = 219270,
    SpellBurrowVisual           = 202496,
    SpellSubmerge               = 53421,

    /// Solarist Tel'arn
    SpellPlasmaSpheres          = 218774,
    SpellPlasmaSpheresDot       = 219235,
    SpellPlasmaExplosion        = 218780,
    SpellFlare                  = 218806,
    SpellCollapseOfNight        = 223437,

    /// Naturalist Tel'arn
    SpellGraceOfNature          = 218927,
    SpellGraceOfNatureAura      = 219009,
    SpellGraceOfNatureHeal      = 219023,
    SpellToxicSpores            = 219049,
    SpellToxicSporesArea        = 219233,
    SpellToxicSporesDot         = 219235,
    SpellChaoticSpheresNature   = 223219,
};

enum eTalks
{
    TalkIntro   = 0,
    TalkAggro,
    TalkSolarCollapse,
    TalkParasiticFetter,
    TalkControlledChaos,
    TalkPlasmaSpheres,
    TalkGraceOfNature,
    TalkCallOfNight,
    TalkPhase2,
    TalkPhase3,
    TalkKill,
    TalkWipe,
    TalkDead,
};

enum eTelarnActions
{
    ActionArcanistDead = 1,
    ActionNaturalistDead,
    ActionSolaristDead,
    ActionEmergeLasher,
    ActionChaoticNatureSphere,
    ActionNaturalistSpawnmed,
};

enum ePhases
{
    InitialPhase,
    SolaristPhase,
    NaturalistPhase,
};

Position const g_PlasmaSpherePos[] =
{
    { 655.056f, 2777.77f, 125.3953f, 0.f },
    { 610.007f, 2787.85f, 125.4043f, 0.f },
    { 640.286f, 2817.88f, 125.3983f, 0.f },
};

static std::array<uint32, 3> g_BotanistEntries =
{
    {
        eCreatures::NpcNaturalistTelarn,
        eCreatures::NpcArcanistTelarn,
        eCreatures::NpcSolaristTelarn
    }
};

static bool IsNaturalistLastAlive()
{
    return g_FormsDead.at(1).m_Dead && g_FormsDead.back().m_Dead;
}

static bool IsArcanistLastAlive()
{
    return g_FormsDead.front().m_Dead && g_FormsDead.back().m_Dead;
}

static bool IsSolaristLastAlive()
{
    return g_FormsDead.at(1).m_Dead && g_FormsDead.front().m_Dead;
}

static bool IsFirstDeadActivate()
{
    bool l_Ret = std::any_of(g_FormsDead.begin(), g_FormsDead.end(), [] (FormDead& p_Itr)
    {
        return p_Itr.m_FirstDead;
    });

    return l_Ret;
}

/// High Botanist Tel'arn - 104528
class boss_high_botanist_telarn : public CreatureScript
{
    public:
        boss_high_botanist_telarn() : CreatureScript("boss_high_botanist_telarn") { }

        enum eEvents
        {
            EventSolarCollapse  = 1,
            EventParasiticFetter,
            EventControlledChaos,
            EventNightosis,
            EventCallOfNight,
            EventSummonChaosSpheres,
            EventBerserk,
        };

        struct boss_high_botanist_telarnAI : public BossAI
        {
            boss_high_botanist_telarnAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataHighBotanistTelarn)
			{
			}

            void Reset() override
            {
                m_Botanist = false;
                m_Solarist = false;
                m_Phase = ePhases::InitialPhase;
                _Reset();
                CleanUp();
            }

            void CleanUp()
            {
                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAura);
                }

                me->DespawnCreaturesInArea(eCreatures::NpcToxicSpore, 500.f);
                me->DespawnCreaturesInArea(eCreatures::NpcPlasmaSphere, 500.f);
                me->DespawnCreaturesInArea(eCreatures::NpcSolarCollapseStalker, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcPlasmaSphere, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcChaosSphere, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcControlledChaosStalker, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcParasiticLasher, 250.f);
                me->DespawnAreaTriggersInArea(eSpells::SpellGraceOfNature, 250.f);
            }

            void EnterEvadeMode() override
            {
                Talk(eTalks::TalkWipe);

                me->RemoveAllAreasTrigger();
                BossAI::EnterEvadeMode();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * IN_MILLISECONDS);
            }

            void HealReceived(Unit* p_Owner, uint32 & p_Heal) override
            {
                if (IsMythic())
                    return;

                uint32 l_Entry = p_Owner->GetEntry();

                if (l_Entry != me->GetEntry())
                    return;

                Creature* l_Botanist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 500.f);
                Creature* l_Boss = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 500.f);

                if (l_Botanist)
                    me->DealHeal(l_Botanist, p_Heal);

                if (l_Boss)
                    me->DealHeal(l_Boss, p_Heal);
            }

            void DamageTaken(Unit* p_Owner, uint32 & p_Damage, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(75) && !m_Solarist)
                {
                    m_Solarist = true;

                    events.CancelEvent(eEvents::EventSolarCollapse);
                    events.CancelEvent(eEvents::EventControlledChaos);
                    events.CancelEvent(eEvents::EventParasiticFetter);

                    events.ScheduleEvent(eEvents::EventNightosis, 500);
                }
                else if (HealthBelowPct(50) && !m_Botanist)
                {
                    m_Botanist = true;

                    events.CancelEvent(eEvents::EventControlledChaos);
                    events.CancelEvent(eEvents::EventParasiticFetter);

                    events.ScheduleEvent(eEvents::EventNightosis, 500);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellNightosisSolarist)
                {
                    m_Phase = ePhases::SolaristPhase;
                    me->CastSpell(me, eSpells::SpellNightosisModelBotanist, true);
                    PrepareTimers(m_Phase);
                }
                else if (p_SpellInfo->Id == eSpells::SpellNightosisNaturalist)
                {
                    m_Phase = ePhases::NaturalistPhase;
                    me->RemoveAurasDueToSpell(eSpells::SpellNightosisModelBotanist);
                    me->CastSpell(me, eSpells::SpellNightosisModelArcanist, true);
                    PrepareTimers(m_Phase);

                    if (Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f))
                    {
                        if (l_Solarist->IsAIEnabled)
                            l_Solarist->GetAI()->DoAction(eTelarnActions::ActionNaturalistSpawnmed);
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eTelarnActions::ActionSolaristDead)
                    events.ScheduleEvent(eEvents::EventSummonChaosSpheres, 30 * IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::TalkAggro);
                _EnterCombat();
                ScheduleTasks();
                DoCast(me, eSpells::SpellRecursiveStrikes, true);
            }

            void ScheduleTasks()
            {
                m_Phase = ePhases::InitialPhase;
                PrepareTimers(m_Phase);
                events.ScheduleEvent(eEvents::EventBerserk, 480 * IN_MILLISECONDS);
            }

            void PrepareTimers(ePhases const p_Phase)
            {
                if (p_Phase == ePhases::InitialPhase)
                {
                    events.ScheduleEvent(eEvents::EventControlledChaos, 35 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventSolarCollapse, 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventParasiticFetter, 22 * IN_MILLISECONDS);
                }
                else if (p_Phase == ePhases::SolaristPhase)
                {
                    events.ScheduleEvent(eEvents::EventControlledChaos, 42 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventParasiticFetter, 22 * IN_MILLISECONDS);
                }
                else
                {
                    events.ScheduleEvent(eEvents::EventCallOfNight, 20 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventControlledChaos, 52 * IN_MILLISECONDS);
                }
            }

            uint32 GetTimerForEvent() const
            {
                if (m_Phase == ePhases::InitialPhase)
                    return 35 * IN_MILLISECONDS;
                else if (m_Phase == ePhases::SolaristPhase)
                    return 40 * IN_MILLISECONDS;
                else if (m_Phase == ePhases::NaturalistPhase)
                    return 50 * IN_MILLISECONDS;

                return 0;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDead);
                me->RemoveAllAuras();
                me->RemoveAllAreasTrigger();
                me->DespawnCreaturesInArea(eCreatures::NpcToxicSpore, 250.f);
                _JustDied();
                CleanUp();
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventSolarCollapse:
                    {
                        Talk(eTalks::TalkSolarCollapse);
                        DoCastVictim(eSpells::SpellSolarCollapse);
                        events.ScheduleEvent(eEvents::EventSolarCollapse, GetTimerForEvent());
                        break;
                    }

                    case eEvents::EventParasiticFetter:
                    {
                        Talk(eTalks::TalkParasiticFetter);
                        DoCast(me, eSpells::SpellParasiticFetterAoe);
                        events.ScheduleEvent(eEvents::EventParasiticFetter, GetTimerForEvent());
                        break;
                    }

                    case eEvents::EventControlledChaos:
                    {
                        Talk(eTalks::TalkControlledChaos);

                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellControlledChaos);

                        events.ScheduleEvent(eEvents::EventControlledChaos, GetTimerForEvent());
                        break;
                    }

                    case eEvents::EventNightosis:
                    {
                        if (m_Solarist && !m_Botanist)
                        {
                            Talk(eTalks::TalkPhase2);
                            me->CastSpell(me, eSpells::SpellNightosisSolarist, false);
                        }
                        else
                        {
                            Talk(eTalks::TalkPhase3);
                            me->CastSpell(me, eSpells::SpellNightosisNaturalist, false);
                        }

                        break;
                    }

                    case eEvents::EventCallOfNight:
                    {
                        Talk(eTalks::TalkCallOfNight);
                        DoCastVictim(eSpells::SpellCallOfNightAoe);
                        events.ScheduleEvent(eEvents::EventCallOfNight, GetTimerForEvent());
                        break;
                    }

                    case eEvents::EventBerserk:
                    {
                        DoCast(me, eSharedSpells::SpellBerserk, true);

                        Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f);
                        Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 250.f);

                        if (l_Solarist)
                            l_Solarist->CastSpell(l_Solarist, eSharedSpells::SpellBerserk, true);

                        if (l_Naturalist)
                            l_Naturalist->CastSpell(l_Naturalist, eSharedSpells::SpellBerserk, true);

                        break;
                    }

                    default : break;
                }
            }

            private:
                ePhases m_Phase;
                bool m_Solarist, m_Botanist;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_high_botanist_telarnAI(p_Creature);
        }
};

/// Arcanist Tel'arn - 109040
class npc_tnh_telarn_arcanist_telarn : public CreatureScript
{
    public:
        npc_tnh_telarn_arcanist_telarn() : CreatureScript("npc_tnh_telarn_arcanist_telarn")
        {}

        enum eEvents
        {
            EventCallOfNight    = 1,
            EventControlledChaos,
            EventSummonChaosSpheres,
            EventBerserk
        };

        struct npc_tnh_telarn_arcanist_telarn_AI : public ScriptedAI
        {
            explicit npc_tnh_telarn_arcanist_telarn_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void CleanUp()
            {
                me->RemoveAllAreasTrigger();
                summons.DespawnAll();
                me->DespawnCreaturesInArea(eCreatures::NpcControlledChaosStalker, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcChaosSphere, 250.f);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAura);
            }

            void SendEncounterId()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                me->SetNativeDisplayId(68963);
                l_Instance->UpdateEncounterState(EncounterCreditType::ENCOUNTER_CREDIT_KILL_CREATURE, 104528, me);
            }

            void JustDied(Unit* /**/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (IsMythic())
                {
                    g_FormsDead.at(1).m_Dead = true;

                    if (!IsFirstDeadActivate())
                        g_FormsDead.at(1).m_FirstDead = true;

                    InstanceScript* l_Instance = me->GetInstanceScript();

                    Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f, true);
                    Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 250.f, true);

                    if (IsArcanistLastAlive())
                    {
                        if (l_Instance)
                        {
                            l_Instance->SetBossState(eData::DataHighBotanistTelarn, DONE);
                            SendEncounterId();
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAoe);
                            l_Instance->SaveToDB();
                        }
                    }
                    else
                    {
                        me->ClearLootContainers();

                        if (l_Solarist)
                            l_Solarist->SetFullHealth();

                        if (l_Naturalist && l_Naturalist->IsAIEnabled)
                        {
                            l_Naturalist->SetFullHealth();

                            if (IsNaturalistLastAlive())
                                l_Naturalist->GetAI()->DoAction(eTelarnActions::ActionChaoticNatureSphere);
                        }
                    }
                }
                else
                {
                    me->ClearLootContainers();
                    CleanUp();
                }
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 1.9358f;
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventBerserk:
                    {
                        DoCast(me, eSharedSpells::SpellBerserk, true);
                        break;
                    }

                    case eEvents::EventCallOfNight:
                    {
                        Talk(eTalks::TalkCallOfNight);
                        DoCastAOE(eSpells::SpellCallOfNightAoe);
                        events.ScheduleEvent(eEvents::EventCallOfNight, 65 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventControlledChaos:
                    {
                        Talk(eTalks::TalkControlledChaos);

                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellControlledChaos);

                        events.ScheduleEvent(eEvents::EventControlledChaos, 65 * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon != nullptr)
                    summons.Summon(p_Summon);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /**/) override
            {
                if (p_Summon != nullptr)
                    summons.Despawn(p_Summon);
            }

            void EnterCombat(Unit* /**/) override
            {
                if (IsMythic())
                {
                    Talk(eTalks::TalkAggro);

                    std::for_each(g_FormsDead.begin(), g_FormsDead.end(), [](FormDead& p_Itr)
                    {
                        p_Itr.m_Dead = false;
                        p_Itr.m_FirstDead = false;
                    });

                    DoCast(me, eSpells::SpellRecursiveStrikes, true);
                    Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 100.f);
                    Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 100.f);

                    if (l_Solarist != nullptr && l_Naturalist != nullptr)
                    {
                        DoZoneInCombat(l_Solarist, 100.f);
                        DoZoneInCombat(l_Naturalist, 100.f);
                    }
                }

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventBerserk, 540 * IN_MILLISECONDS);

                events.RescheduleEvent(eEvents::EventCallOfNight, (IsMythic() ? 55 : 65) * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventControlledChaos, 30 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
                CleanUp();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void EnterEvadeMode() override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                ScriptedAI::EnterEvadeMode();

                if (IsMythic())
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAura);
                        l_Instance->SetBossState(eData::DataHighBotanistTelarn, EncounterState::FAIL);
                    }

                    me->InterruptNonMeleeSpells(true);
                    me->NearTeleportTo(me->GetHomePosition());
                    me->Respawn(true, true, 30 * IN_MILLISECONDS);
                    summons.DespawnAll();

                    Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f, false);
                    Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 250.f, false);

                    if (l_Solarist != nullptr)
                    {
                        if (l_Solarist->isDead())
                        {
                            l_Solarist->NearTeleportTo(l_Solarist->GetHomePosition());
                            l_Solarist->Respawn(true, true, 30 * IN_MILLISECONDS);
                        }
                    }

                    if (l_Naturalist != nullptr)
                    {
                        if (l_Naturalist->isDead())
                        {
                            l_Naturalist->NearTeleportTo(l_Naturalist->GetHomePosition());
                            l_Naturalist->Respawn(true, true, 30 * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
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
            return new npc_tnh_telarn_arcanist_telarn_AI(p_Me);
        }
};

/// Solarist Tel'arn - 109038
class npc_nh_telarn_solarist_telarn : public CreatureScript
{
    public:
        npc_nh_telarn_solarist_telarn() : CreatureScript("npc_nh_telarn_solarist_telarn")
        {}

        enum eEvents
        {
            EventFlare  = 1,
            EventSolarCollapse,
            EventSummonPlasmaSpheres,
            EventCollapseOfNight,
            EventBerserk
        };

        struct npc_nh_telarn_solarist_telarn_AI : public ScriptedAI
        {
            explicit npc_nh_telarn_solarist_telarn_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 3.0006f;
                p_Modifier /= 1.55f;
            }

            void SendEncounterId()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                me->SetNativeDisplayId(68963);
                l_Instance->UpdateEncounterState(EncounterCreditType::ENCOUNTER_CREDIT_KILL_CREATURE, 104528, me);
            }

            void CleanUp()
            {
                me->RemoveAllAreasTrigger();
                summons.DespawnAll();
                me->DespawnCreaturesInArea(eCreatures::NpcSolarCollapseStalker, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcPlasmaSphere, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcChaosSphere, 250.f);
            }

            void Reset() override
            {
                m_NaturalistDead = false;
                events.Reset();

                CleanUp();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eTelarnActions::ActionNaturalistSpawnmed)
                {
                    events.CancelEvent(eEvents::EventSolarCollapse);
                    events.CancelEvent(eEvents::EventSummonPlasmaSpheres);

                    m_Phase = ePhases::NaturalistPhase;
                    events.ScheduleEvent(eEvents::EventSolarCollapse, 42 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventSummonPlasmaSpheres, 25 * IN_MILLISECONDS);
                }
            }

            void EnterEvadeMode() override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (IsMythic())
                {
                    ScriptedAI::EnterEvadeMode();

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAura);

                        l_Instance->SetBossState(eData::DataHighBotanistTelarn, EncounterState::FAIL);
                    }

                    me->InterruptNonMeleeSpells(true);
                    me->NearTeleportTo(me->GetHomePosition());
                    me->Respawn(true, true, 30 * IN_MILLISECONDS);
                    summons.DespawnAll();

                    Creature* l_Arcanist = me->FindNearestCreature(eCreatures::NpcArcanistTelarn, 250.f, false);
                    Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 250.f, false);

                    if (l_Arcanist != nullptr)
                    {
                        if (l_Arcanist->isDead())
                        {
                            l_Arcanist->NearTeleportTo(l_Arcanist->GetHomePosition());
                            l_Arcanist->Respawn(true, true, 30 * IN_MILLISECONDS);
                        }
                    }

                    if (l_Naturalist != nullptr)
                    {
                        if (l_Naturalist->isDead())
                        {
                            l_Naturalist->NearTeleportTo(l_Naturalist->GetHomePosition());
                            l_Naturalist->Respawn(true, true, 30 * IN_MILLISECONDS);
                        }
                    }
                }
                else
                    CleanUp();
            }

            void HealReceived(Unit* p_Owner, uint32 & p_Heal) override
            {
                if (IsMythic())
                    return;

                uint32 l_Entry = p_Owner->GetEntry();

                if (l_Entry != me->GetEntry())
                    return;

                Creature* l_Botanist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 500.f);
                Creature* l_Boss = me->FindNearestCreature(eCreatures::BossHighBotanistTelarn, 500.f);

                if (l_Botanist)
                    me->DealHeal(l_Botanist, p_Heal);

                if (l_Boss)
                    me->DealHeal(l_Boss, p_Heal);
            }

            void ScheduleTasks()
            {
                if (IsMythic())
                {
                    events.ScheduleEvent(eEvents::EventBerserk, 540 * IN_MILLISECONDS);
                    events.RescheduleEvent(eEvents::EventSolarCollapse, 5 * IN_MILLISECONDS);
                    events.RescheduleEvent(eEvents::EventSummonPlasmaSpheres, 30 * IN_MILLISECONDS);
                    events.RescheduleEvent(eEvents::EventFlare, 10 * IN_MILLISECONDS);
                }
                else
                {
                    m_Phase = ePhases::SolaristPhase;

                    if (Creature* l_Boss = me->FindNearestCreature(eCreatures::BossHighBotanistTelarn, 150.f))
                        me->SetHealth(l_Boss->GetHealth());

                    events.ScheduleEvent(eEvents::EventSolarCollapse, 32 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventSummonPlasmaSpheres, 12 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventFlare, 3 * IN_MILLISECONDS);
                }
            }

            void JustDied(Unit* /**/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

				events.Reset();

                if (IsMythic())
                {
                    g_FormsDead.back().m_Dead = true;

                    if (!IsFirstDeadActivate())
                        g_FormsDead.back().m_FirstDead = true;

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);

                    Creature* l_Arcanist = me->FindNearestCreature(eCreatures::NpcArcanistTelarn, 250.f, true);
                    Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 250.f, true);

                    if (IsSolaristLastAlive())
                    {
                        InstanceScript* l_Instance = me->GetInstanceScript();

                        me->RemoveAllAreasTrigger();
                        summons.DespawnAll();

                        if (l_Instance != nullptr)
                        {
                            SendEncounterId();
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAoe);
                            l_Instance->SetBossState(eData::DataHighBotanistTelarn, DONE);
                            l_Instance->SaveToDB();
                        }
                    }
                    else
                    {
                        if (l_Arcanist && l_Arcanist->IsAIEnabled)
                        {
                            l_Arcanist->SetFullHealth();
                            l_Arcanist->GetAI()->DoAction(eTelarnActions::ActionSolaristDead);
                        }
                        if (l_Naturalist)
                        {
                            if (IsNaturalistLastAlive() && l_Naturalist->IsAIEnabled)
                                l_Naturalist->GetAI()->DoAction(eTelarnActions::ActionChaoticNatureSphere);

                            l_Naturalist->SetFullHealth();
                        }

                        me->ClearLootContainers();
                    }
                }
                else
                {
                    me->ClearLootContainers();
                    CleanUp();
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon != nullptr)
                    summons.Summon(p_Summon);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /**/) override
            {
                if (p_Summon != nullptr)
                    summons.Despawn(p_Summon);
            }

            void EnterCombat(Unit* /**/) override
            {
                if (IsMythic())
                {
                    Creature* l_Naturalist = me->FindNearestCreature(eCreatures::NpcNaturalistTelarn, 100.f);
                    Creature* l_Arcanist = me->FindNearestCreature(eCreatures::NpcArcanistTelarn, 100.f);

                    if (l_Arcanist != nullptr && l_Naturalist != nullptr)
                    {
                        DoZoneInCombat(l_Arcanist, 100.f);
                        DoZoneInCombat(l_Naturalist, 100.f);
                    }
                }

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                ScheduleTasks();
            }

            uint32 GetTimerForEvent() const
            {
                if (m_Phase == ePhases::SolaristPhase)
                    return 40;
                else if (m_Phase == ePhases::NaturalistPhase)
                    return 50;

                return 0;
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFlare:
                    {
                        DoCastVictim(eSpells::SpellFlare);
                        events.ScheduleEvent(eEvents::EventFlare, 8 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSolarCollapse:
                    {
                        Talk(eTalks::TalkSolarCollapse);
                        DoCast(me, g_FormsDead.at(1).m_FirstDead ? eSpells::SpellCollapseOfNight : eSpells::SpellSolarCollapse);
                        events.ScheduleEvent(eEvents::EventSolarCollapse, (IsMythic() ? 65 : GetTimerForEvent()) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSummonPlasmaSpheres:
                    {
                        Talk(eTalks::TalkPlasmaSpheres);
                        DoCast(me, eSpells::SpellPlasmaSpheres);
                        events.ScheduleEvent(eEvents::EventSummonPlasmaSpheres, (IsMythic() ? 65 : GetTimerForEvent()) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventBerserk:
                    {
                        DoCast(me, eSharedSpells::SpellBerserk, true);
                        break;
                    }

                    default : break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            private:
                ePhases m_Phase;
                bool m_NaturalistDead;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_telarn_solarist_telarn_AI(p_Me);
        }

};

/// Naturalist Tel'arn - 109041
class npc_nh_telarn_naturalist_telarn : public CreatureScript
{
    public:
        npc_nh_telarn_naturalist_telarn() : CreatureScript("npc_nh_telarn_naturalist_telarn")
        {}

        enum eEvents
        {
            EventParasiticFetter    = 1,
            EventGraceOfNature,
            EventToxicSpores,
            EventBerserk,
            EventChaoticSpheresofNature
        };

        struct npc_nh_telarn_naturalist_telarn_AI : public ScriptedAI
        {
            explicit npc_nh_telarn_naturalist_telarn_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 3.0006f;
                p_Modifier /= 1.55f;
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eTelarnActions::ActionChaoticNatureSphere)
                    events.ScheduleEvent(eEvents::EventChaoticSpheresofNature, 30 * IN_MILLISECONDS);
            }

            void SendEncounterId()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                me->SetNativeDisplayId(68963);
                l_Instance->UpdateEncounterState(EncounterCreditType::ENCOUNTER_CREDIT_KILL_CREATURE, 104528, me);
            }

            void HealReceived(Unit* p_Owner, uint32 & p_Heal) override
            {
                if (IsMythic())
                    return;

                uint32 l_Entry = p_Owner->GetEntry();

                if (l_Entry != me->GetEntry())
                    return;

                Creature* l_Botanist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 500.f);
                Creature* l_Boss = me->FindNearestCreature(eCreatures::BossHighBotanistTelarn, 500.f);

                if (l_Botanist)
                    me->DealHeal(l_Botanist, p_Heal);

                if (l_Boss)
                    me->DealHeal(l_Boss, p_Heal);
            }

            void CleanUp()
            {
                me->RemoveAllAreasTrigger();
                summons.DespawnAll();
                me->DespawnCreaturesInArea(eCreatures::NpcToxicSpore, 500.f);
                me->DespawnAreaTriggersInArea(eSpells::SpellGraceOfNature);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                }
            }

            void Reset() override
            {
                events.Reset();
                CleanUp();

				if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SetBossState(eData::DataHighBotanistTelarn, NOT_STARTED);
					l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                }

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void EnterEvadeMode() override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (IsMythic())
                {
                    CreatureAI::EnterEvadeMode();

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                        l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAura);
                        l_Instance->SetBossState(eData::DataHighBotanistTelarn, EncounterState::FAIL);
                        me->DespawnAreaTriggersInArea(eSpells::SpellGraceOfNature);
                    }

                    me->RemoveAllAreasTrigger();
                    me->InterruptNonMeleeSpells(true);
                    me->NearTeleportTo(me->GetHomePosition());
                    me->Respawn(true, true, 30 * IN_MILLISECONDS);
                    summons.DespawnAll();

                    Creature* l_Arcanist = me->FindNearestCreature(eCreatures::NpcArcanistTelarn, 250.f, false);
                    Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f, false);

                    if (l_Arcanist != nullptr)
                    {
                        if (l_Arcanist->isDead())
                        {
                            l_Arcanist->NearTeleportTo(l_Arcanist->GetHomePosition());
                            l_Arcanist->Respawn(true, true, 30 * IN_MILLISECONDS);
                        }
                    }

                    if (l_Solarist != nullptr)
                    {
                        if (l_Solarist->isDead())
                        {
                            l_Solarist->NearTeleportTo(l_Solarist->GetHomePosition());
                            l_Solarist->Respawn(true, true, 30 * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                if (IsMythic())
                {
                    Creature* l_Arcanist = me->FindNearestCreature(eCreatures::NpcArcanistTelarn, 100.f);
                    Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 100.f);

                    if (l_Arcanist != nullptr && l_Solarist != nullptr)
                    {
                        DoZoneInCombat(l_Arcanist, 100.f);
                        DoZoneInCombat(l_Solarist, 100.f);
                    }

                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                        l_Instance->SetBossState(eData::DataHighBotanistTelarn, IN_PROGRESS);
                }

                DoZoneInCombat();
                ScheduleTasks();
            }

            void TalkBotanist(eTalks p_TalkId)
            {
                Creature* l_Botanist = me->FindNearestCreature(eCreatures::BossHighBotanistTelarn, 150.f, true);

                if (!l_Botanist)
                    return;

                if (l_Botanist->IsAIEnabled)
                    l_Botanist->AI()->Talk(p_TalkId);
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (IsMythic())
                {
                    g_FormsDead.front().m_Dead = true;


                    if (!IsFirstDeadActivate())
                        g_FormsDead.front().m_FirstDead = true;

                    Creature* l_Arcanist = me->FindNearestCreature(eCreatures::NpcArcanistTelarn, 250.f, true);
                    Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f, true);

                    if (IsNaturalistLastAlive())
                    {
                        InstanceScript* l_Instance = me->GetInstanceScript();

                        if (l_Instance != nullptr)
                        {
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterDot);
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticFetterRoot);
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCallOfNightAoe);
                            SendEncounterId();
                            l_Instance->SetBossState(eData::DataHighBotanistTelarn, DONE);
                            l_Instance->SaveToDB();
                        }
                    }
                    else
                    {
                        if (l_Solarist)
                            l_Solarist->SetFullHealth();

                        if (l_Arcanist)
                            l_Arcanist->SetFullHealth();

                        me->ClearLootContainers();
                    }
                }
                else
                {
                    me->ClearLootContainers();
                    CleanUp();
                }
            }

            void ScheduleTasks()
            {
                if (IsMythic())
                {
                    events.RescheduleEvent(eEvents::EventBerserk, 540 * IN_MILLISECONDS);
                    events.RescheduleEvent(eEvents::EventParasiticFetter, 18 * IN_MILLISECONDS);
                    events.RescheduleEvent(eEvents::EventToxicSpores, 15 * IN_MILLISECONDS);
                    events.RescheduleEvent(eEvents::EventGraceOfNature, 65 * IN_MILLISECONDS);
                }
                else
                {
                    if (Creature* l_Boss = me->FindNearestCreature(eCreatures::BossHighBotanistTelarn, 150.f))
                        me->SetHealth(l_Boss->GetHealth());

                    events.ScheduleEvent(eEvents::EventParasiticFetter, 35 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventToxicSpores, 5 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventGraceOfNature, 10 * IN_MILLISECONDS);
                }
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    TalkBotanist(eTalks::TalkKill);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon == nullptr)
                    return;

                summons.Summon(p_Summon);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /**/) override
            {
                if (p_Summon == nullptr)
                    return;

                summons.Despawn(p_Summon);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventBerserk:
                    {
                        DoCast(me, eSharedSpells::SpellBerserk, true);
                        break;
                    }
                    case eEvents::EventParasiticFetter:
                    {
                        Talk(eTalks::TalkParasiticFetter);
                        DoCast(me, eSpells::SpellParasiticFetterAoe);
                        events.ScheduleEvent(eEvents::EventParasiticFetter, (IsMythic() ? 65 : 50) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventToxicSpores:
                    {
                        DoCastVictim(eSpells::SpellToxicSpores);
                        events.ScheduleEvent(eEvents::EventToxicSpores, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGraceOfNature:
                    {
                        Talk(eTalks::TalkGraceOfNature);
                        DoCast(me, eSpells::SpellGraceOfNature);
                        events.ScheduleEvent(eEvents::EventGraceOfNature, (IsMythic() ? 65 : 70) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventChaoticSpheresofNature:
                    {
                        DoCastAOE(eSpells::SpellChaoticSpheresNature);
                        events.ScheduleEvent(eEvents::EventChaoticSpheresofNature, 65 * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
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
            return new npc_nh_telarn_naturalist_telarn_AI(p_Me);
        }
};

/// Controlled Chaos Stalker - 109792
class npc_nh_telarn_controlled_chaos_stalker : public CreatureScript
{
    public:
        npc_nh_telarn_controlled_chaos_stalker() : CreatureScript("npc_nh_telarn_controlled_chaos_stalker")
        {}

        enum eEvents
        {
            EventControlledChaos = 1,
        };

        struct npc_nh_telarn_controlled_chaos_stalker_AI : public ScriptedAI
        {
            explicit npc_nh_telarn_controlled_chaos_stalker_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner == nullptr)
                    return;

                if (IsArcanistLastAlive())
                    p_Summoner->SummonCreature(eCreatures::NpcSolarCollapseStalker, *me, TEMPSUMMON_TIMED_DESPAWN, 7500);

                m_SpellId = eSpells::SpellControlledChaos10Yd;
                events.ScheduleEvent(eEvents::EventControlledChaos, 100);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellControlledChaos10Yd)
                {
                    if (IsNaturalistLastAlive())
                        DoCast(me, eSpells::SpellChaoticNature10Yd, true);

                    m_SpellId = eSpells::SpellControlledChaos20Yd;
                    events.ScheduleEvent(eEvents::EventControlledChaos, 100);
                }
                else if (p_SpellInfo->Id == eSpells::SpellControlledChaos20Yd)
                {
                    if (IsNaturalistLastAlive())
                        DoCast(me, eSpells::SpellChaoticNature20Yd, true);

                    events.ScheduleEvent(eEvents::EventControlledChaos, 100);
                    m_SpellId = eSpells::SpellControlledChaos30Yd;
                }
                else if (p_SpellInfo->Id == eSpells::SpellControlledChaos30Yd)
                {
                    if (IsNaturalistLastAlive())
                        DoCast(me, eSpells::SpellChaoticNature30Yd, true);
                }
            }

            void EnterEvadeMode() override
            {
                /**/
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvents::EventControlledChaos)
                    DoCastAOE(m_SpellId);
            }

            private:
                uint32 m_SpellId;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_telarn_controlled_chaos_stalker_AI(p_Me);
        }
};

/// Solar Collapse Stalker - 109583
class npc_nh_telarn_solar_collapse_stalker : public CreatureScript
{
    public:
        npc_nh_telarn_solar_collapse_stalker() : CreatureScript("npc_nh_telarn_solar_collapse_stalker")
        {}

        enum eEvents
        {
            EventSolarCollapse = 1,
        };

        struct npc_nh_telarn_solar_collapse_stalker_AI : public ScriptedAI
        {
            explicit npc_nh_telarn_solar_collapse_stalker_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_Dist = 20.f;
                m_CastingCount = 0;
            }

            void IsSummonedBy(Unit* /**/) override
            {
                CreateSolarRing(m_Dist, eSpells::SpellSolarCollapseMissile);
                m_Dist -= 6.f;
                events.ScheduleEvent(eEvents::EventSolarCollapse, 3250);
            }

            void CreateSolarRing(const float & p_Dist, uint32 p_Spell = eSpells::SpellSolarCollapseMissileFast)
            {
                Position l_Pos;
                float l_Angle = 0.f;
                m_CastingCount++;

                const uint8 l_Missiles = m_CastingCount < 3 ? 12 : 6;

                for (uint8 i = 0; i < l_Missiles; ++i)
                {
                    if (m_CastingCount >= 5)
                        l_Pos = me->GetPosition();
                    else
                        me->GetNearPosition(l_Pos, p_Dist, l_Angle);

                    me->CastSpell(l_Pos, p_Spell, true);
                    l_Angle += (2.f * float(M_PI))/l_Missiles;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CastingCount > 4)
                    return;

                events.Update(p_Diff);

                if (events.ExecuteEvent() == eEvents::EventSolarCollapse)
                {
                    CreateSolarRing(m_Dist);
					m_Dist -= 4.f;
                    events.ScheduleEvent(eEvents::EventSolarCollapse, IN_MILLISECONDS + 500);
				}
            }

            private:
                float m_Dist;
                uint8 m_CastingCount;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_telarn_solar_collapse_stalker_AI(p_Me);
        }
};

/// Plasma Sphere - 109804
class npc_tnh_telarn_plasma_sphere : public CreatureScript
{
    public:
        npc_tnh_telarn_plasma_sphere() : CreatureScript("npc_tnh_telarn_plasma_sphere")
        {}

        struct npc_tnh_telarn_plasma_sphere_AI : public ScriptedAI
        {
            explicit npc_tnh_telarn_plasma_sphere_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->DisableHealthRegen();
                m_Timer = 0;

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void EnterEvadeMode() override
            {
                /*Empty*/
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 2.6650f;
                p_Modifier /= 1.55f;
            }

            void Reset() override
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                if (me->GetEntry() == eCreatures::NpcChaosSphere || me->GetEntry() == eCreatures::NpcChaoticSphere)
                    DoCast(me, eSpells::SpellControlledChaosSummon, true);

                DoCast(me, eSpells::SpellPlasmaExplosion, true);

                if (IsMythic())
                {
                    /// Naturalist and Arcanist Dead
                    if (g_FormsDead.front().m_FirstDead && !IsSolaristLastAlive())
                    {
                        Position l_Pos;

                        for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                        {
                            me->GetNearPosition(l_Pos, frand(3.f, 5.f), frand(0.f, 2.f * float(M_PI)));
                            me->SummonCreature(eCreatures::NpcParasiticLasher, l_Pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        }
                    }
                    else if (IsSolaristLastAlive())
                    {
                        Creature* l_Solarist = me->FindNearestCreature(eCreatures::NpcSolaristTelarn, 250.f, true);

                        if (l_Solarist == nullptr)
                            return;

                        Position l_Pos;

                        l_Solarist->CastSpell(*me, eSpells::SpellToxicSporesArea, true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
              m_Timer += p_Diff;

                if (me->GetEntry() == eCreatures::NpcChaosSphere && m_Timer >= 500)
                {
                    m_Timer = 0;
                    me->DealDamage(me, me->GetMaxHealth() * 0.02f);
                }
            }

            uint32 m_Timer;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_telarn_plasma_sphere_AI(p_Me);
        }
};

/// Toxic Spore - 110125
class npc_tnh_telarn_toxic_spore : public CreatureScript
{
    public:
        npc_tnh_telarn_toxic_spore() : CreatureScript("npc_tnh_telarn_toxic_spore")
        {}

        struct npc_tnh_telarn_toxic_spore_AI : public ScriptedAI
        {
            explicit npc_tnh_telarn_toxic_spore_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_TimerCheck    = 0;
                m_SummonerGUID  = 0;
                m_Died          = false;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner == nullptr)
                    return;

                m_SummonerGUID = p_Summoner->GetGUID();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Died)
                    return;

                m_TimerCheck += p_Diff;

                if (m_TimerCheck >= 1500)
                {
                    m_TimerCheck = 0;

                    Unit* l_Summoner = sObjectAccessor->GetUnit(*me, m_SummonerGUID);

                    Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();

                    for (auto l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                    {
                        Unit* l_Target = l_Itr->getSource();

                        if (l_Target == nullptr)
                            continue;

                        if (l_Target->GetDistance2d(me) <= 4.f)
                        {
                            m_Died = true;

                            if (l_Summoner != nullptr)
                                l_Summoner->CastSpell(l_Target, eSpells::SpellToxicSporesDot, true);

                            break;
                        }
                    }

                    if (m_Died)
                        me->DespawnOrUnsummon(IN_MILLISECONDS);
                }
            }

            private:
				uint64 m_SummonerGUID;
                bool m_Died;
                uint32 m_TimerCheck;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_tnh_telarn_toxic_spore_AI(p_Me);
        }
};

/// Parasitic Lasher - 109075
class npc_nh_telarn_parasitic_lasher : public CreatureScript
{
    public:
        npc_nh_telarn_parasitic_lasher() : CreatureScript("npc_nh_telarn_parasitic_lasher")
        {}

        enum eEvents
        {
            EventParasiticFixate = 1,
            EventEmerge
        };

        struct npc_nh_telarn_parasitic_lasher_AI : public ScriptedAI
        {
                explicit npc_nh_telarn_parasitic_lasher_AI(Creature* p_Me) : ScriptedAI(p_Me)
                {
                    m_TargetGUID = 0;
                    me->SetReactState(REACT_PASSIVE);
                }

                void IsSummonedBy(Unit* p_Summoner) override
                {
                    if (IsNaturalistLastAlive())
                        DoCast(me, eSpells::SpellCallOfNightAura, true);

                    events.ScheduleEvent(eEvents::EventParasiticFixate, 2 * IN_MILLISECONDS);
                }

                void Reset() override
                {
                    events.Reset();
                }

                void JustDied(Unit* /**/) override
                {
                    if (Unit* l_MarkedTarget = sObjectAccessor->GetUnit(*me, m_TargetGUID))
                        l_MarkedTarget->RemoveAurasDueToSpell(eSpells::SpellParasiticFixate, me->GetGUID());

                    events.Reset();
                    me->RemoveAllAuras();
                }

                void DoAction(int32 const p_Action) override
                {
                    if (p_Action == eTelarnActions::ActionEmergeLasher)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAurasDueToSpell(eSpells::SpellSubmerge);
                        me->RemoveAurasDueToSpell(eSpells::SpellBurrowVisual);
                        m_TargetGUID = 0;
                        events.ScheduleEvent(eEvents::EventParasiticFixate, urand(2, 4) * IN_MILLISECONDS);
                    }
                }

                Unit* GetNewTargetForFixate()
                {
                    uint64 l_Guid = 0;
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance)
                    {
                        Creature* l_Boss = nullptr;

                        if (IsMythic())
                        {
                            for (uint32 const & l_Itr : g_BotanistEntries)
                            {
                                l_Guid = l_Instance->GetData64(l_Itr);

                                if (Creature* l_Botanist = sObjectAccessor->GetCreature(*me, l_Guid))
                                {
                                    if (l_Botanist->isAlive())
                                        break;
                                }
                            }
                        }
                        else
                            l_Guid = l_Instance->GetData64(eCreatures::BossHighBotanistTelarn);

                        l_Boss = sObjectAccessor->GetCreature(*me, l_Guid);

                        if (l_Boss && l_Boss->IsAIEnabled)
                            return l_Boss->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true, 0);
                    }

                    return nullptr;
                }

                void OnSpellCasted(SpellInfo const* p_Spell) override
                {
                    if (p_Spell == nullptr)
                        return;

                    if (p_Spell->Id == eSpells::SpellSubmerge)
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        DoCast(me, eSpells::SpellBurrowVisual, true);
                    }
                    else if (p_Spell->Id == eSpells::SpellParasiticFetterDot)
                        me->RemoveAura(eSpells::SpellRampantGrowth);
                }

                void JustDespawned() override
                {
                    if (Unit* l_MarkedTarget = sObjectAccessor->GetUnit(*me, m_TargetGUID))
                        l_MarkedTarget->RemoveAurasDueToSpell(eSpells::SpellParasiticFixate, me->GetGUID());
                }

                void ExecuteEvent(uint32 const p_EventId)
                {
                    if (p_EventId == eEvents::EventParasiticFixate)
                    {
                        Unit* l_Target = GetNewTargetForFixate();

                        if (l_Target == nullptr)
                            events.ScheduleEvent(eEvents::EventParasiticFixate, 500);
                        else
                        {
                            m_TargetGUID = l_Target->GetGUID();
                            DoCast(me, eSpells::SpellParasiticFetterAt, true);
                            DoCast(l_Target, eSpells::SpellParasiticFixate, true);
                        }
                    }
                }

                void UpdateAI(uint32 const p_Diff) override
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (l_Instance->GetBossState(eData::DataHighBotanistTelarn) != IN_PROGRESS)
                            me->DespawnOrUnsummon(IN_MILLISECONDS);
                    }

                    events.Update(p_Diff);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    while (uint32 const l_EventId = events.ExecuteEvent())
                        ExecuteEvent(l_EventId);
                }

            private:
                uint64 m_TargetGUID;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_telarn_parasitic_lasher_AI(p_Me);
        }
};

/// Controlled Chaos Aoe - 218438
class spell_nh_telarn_controlled_chaos_aoe : public SpellScriptLoader
{
    public:
        spell_nh_telarn_controlled_chaos_aoe() : SpellScriptLoader("spell_nh_telarn_controlled_chaos_aoe")
        {}

        class spell_controlled_chaos_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_controlled_chaos_aoe_SpellScript);

            void HandleOnDummy(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellControlledChaosSummon, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_controlled_chaos_aoe_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_controlled_chaos_aoe_SpellScript();
        }
};

/// Controlled Chaos Heal - 223386
class spell_nh_telarn_chaotic_nature : public SpellScriptLoader
{
    public:
        spell_nh_telarn_chaotic_nature() : SpellScriptLoader("spell_nh_telarn_chaotic_nature")
        {}

        class spell_nh_telarn_chaotic_nature_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nh_telarn_chaotic_nature_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    return p_Itr->IsPlayer();
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nh_telarn_chaotic_nature_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nh_telarn_chaotic_nature_SpellScript();
        }
};

/// Flare - 218806
class spell_nh_telarn_flare : public SpellScriptLoader
{
    public:
        spell_nh_telarn_flare() : SpellScriptLoader("spell_nh_telarn_flare")
        {}

        class spell_nh_telarn_flare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nh_telarn_flare_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                if (IsSolaristLastAlive())
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellParasiticFetterDot, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_nh_telarn_flare_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nh_telarn_flare_SpellScript();
        }
};

/// Solar Collapse Aoe - 218418
class spell_nh_telarn_solar_collapse_aoe : public SpellScriptLoader
{
    public:
        spell_nh_telarn_solar_collapse_aoe() : SpellScriptLoader("spell_nh_telarn_solar_collapse_aoe")
        {}

        class spell_solar_collapse_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_solar_collapse_aoe_SpellScript);

            void HandleOnDummy(SpellEffIndex /**/)
            {
                if (!GetHitUnit() || !GetCaster())
                    return;

                GetCaster()->SummonCreature(eCreatures::NpcSolarCollapseStalker, *GetHitUnit(), TEMPSUMMON_TIMED_DESPAWN, 9500);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_solar_collapse_aoe_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_solar_collapse_aoe_SpellScript();
        }
};

/// Parasitic Fetter Aoe - 218424
class spell_nh_telarn_parasitic_fetter_aoe : public SpellScriptLoader
{
    public:
        spell_nh_telarn_parasitic_fetter_aoe() : SpellScriptLoader("spell_nh_telarn_parasitic_fetter_aoe")
        {}

        class spell_parasitic_fetter_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_parasitic_fetter_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                TankFilter l_Cmp;

                p_Targets.sort([&l_Cmp](auto & p_Itr, auto& p_Itr2)
                {
                    return !l_Cmp(p_Itr->ToPlayer()) && l_Cmp(p_Itr2->ToPlayer());
                });

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleOnDummy(SpellEffIndex /**/)
            {
                if (!GetHitUnit() || !GetCaster())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellParasiticFetterDot, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_parasitic_fetter_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_parasitic_fetter_aoe_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_parasitic_fetter_aoe_SpellScript();
        }
};

/// Parasitic Fetter Dot - 218304
class spell_nh_telarn_parasitic_fetter_dot : public SpellScriptLoader
{
    public:
        spell_nh_telarn_parasitic_fetter_dot() : SpellScriptLoader("spell_nh_telarn_parasitic_fetter_dot")
        {}

        class spell_parasitic_fetter_dot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_parasitic_fetter_dot_AuraScript);

            void HandleAfterDispel(DispelInfo* /**/)
            {
                if (!GetUnitOwner() || !GetCaster())
                    return;

                if (InstanceScript* l_Ins = GetUnitOwner()->GetInstanceScript())
                {
                    if (l_Ins->GetBossState(eData::DataHighBotanistTelarn) != EncounterState::IN_PROGRESS)
                        return;
                }

                Position l_Pos;

                if (GetCaster()->HasAura(eSpells::SpellBurrowVisual) && GetCaster()->IsAIEnabled)
                    GetCaster()->GetAI()->DoAction(eTelarnActions::ActionEmergeLasher);

                for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                {
                    GetUnitOwner()->GetNearPosition(l_Pos, frand(3.f, 7.f), frand(0.f, 2.f * float(M_PI) ) );
                    Creature* l_Lasher = GetCaster()->SummonCreature(eCreatures::NpcParasiticLasher, l_Pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                }

                if (g_FormsDead.at(1).m_FirstDead)
                    GetCaster()->CastSpell(GetUnitOwner(), eSpells::SpellControlledChaosSummon, true);
            }

            void HandleOnPeriodic(AuraEffect const* p_AurEff)
            {
                if (AuraEffect* l_AurEff = const_cast<AuraEffect*>(p_AurEff))
                    l_AurEff->SetAmount(l_AurEff->GetAmount() * 1.10f);
            }

            void Register() override
            {
                AfterDispel         += AuraDispelFn(spell_parasitic_fetter_dot_AuraScript::HandleAfterDispel);
                OnEffectPeriodic    += AuraEffectPeriodicFn(spell_parasitic_fetter_dot_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_parasitic_fetter_dot_AuraScript();
        }
};

/// Parasitic Fixate - 218342
class spell_nh_telarn_parasitic_fixate : public SpellScriptLoader
{
    public:
        spell_nh_telarn_parasitic_fixate() : SpellScriptLoader("spell_nh_telarn_parasitic_fixate")
        {}

        class spell_parasitic_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_parasitic_fixate_AuraScript);

            bool Load() override
            {
                m_TimerRegrowth = 0;
                return true;
            }

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetCaster() || !GetUnitOwner())
                    return;

                m_TimerRegrowth += 250;

                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();

                if (l_Caster->isDead())
                    Remove();

                if (m_TimerRegrowth >= 3 * IN_MILLISECONDS)
                {
                    m_TimerRegrowth = 0;

                    l_Caster->CastSpell(l_Caster, eSpells::SpellRampantGrowth, true);

                    if (Aura* l_Growth = l_Caster->GetAura(eSpells::SpellRampantGrowth))
                    {
                        if (l_Growth->GetStackAmount() == 5)
                            l_Caster->CastSpell(l_Caster, eSpells::SpellOverGrowth, true);
                    }
                }

                l_Caster->GetMotionMaster()->MoveFollow(l_Owner, 0, 0);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_parasitic_fixate_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }

            uint32 m_TimerRegrowth;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_parasitic_fixate_AuraScript();
        }
};

/// Summon Plasma Spheres - 218774
class spell_nh_telarn_summon_plasma_spheres : public SpellScriptLoader
{
    public:
        spell_nh_telarn_summon_plasma_spheres() : SpellScriptLoader("spell_nh_telarn_summon_plasma_spheres")
        {}

        class spell_summon_plasma_spheres_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_plasma_spheres_SpellScript);

            void HandleOnDummy(SpellEffIndex /**/)
            {
                Creature* l_SolarSphere = nullptr;

                for (uint8 i = 0; i < 3; ++i)
                {
                    l_SolarSphere = GetCaster()->SummonCreature(eCreatures::NpcPlasmaSphere, g_PlasmaSpherePos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                    if (l_SolarSphere != nullptr)
                        l_SolarSphere->DisableHealthRegen();
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_summon_plasma_spheres_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_plasma_spheres_SpellScript();
        }
};

/// Grace Of Nature Aura - 219009
class spell_nh_telarn_grace_of_nature_aura : public SpellScriptLoader
{
    public:
        spell_nh_telarn_grace_of_nature_aura() : SpellScriptLoader("spell_nh_telarn_grace_of_nature_aura")
        {}

        class spell_grace_of_nature_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_grace_of_nature_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetUnitOwner())
                    return;

                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellGraceOfNatureHeal, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_grace_of_nature_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_grace_of_nature_AuraScript();
        }
};

/// Call of Night Aoe - 218007
class spell_nh_telarn_call_of_night_aoe : public SpellScriptLoader
{
    public:
        spell_nh_telarn_call_of_night_aoe() : SpellScriptLoader("spell_nh_telarn_call_of_night_aoe")
        {}

        class spell_call_of_night_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_call_of_night_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || !GetCaster())
                    return;

                Map* l_Map = GetCaster()->GetMap();

                if (l_Map == nullptr)
                    return;

                TankFilter Cmp;

                p_Targets.sort([&Cmp] (WorldObject*& p_Itr, WorldObject*& p_Itr2)
                {
                    return !Cmp(p_Itr->ToPlayer()) && Cmp(p_Itr2->ToPlayer());
                });

                if (p_Targets.size() < 2)
                    return;

                const size_t& l_Size = p_Targets.size();

                if (l_Map->IsMythic())
                    p_Targets.resize(2);
                else if (l_Size >= 1 && l_Size <= 19)
                    p_Targets.resize(1);
                else if (l_Size >= 20 && l_Size <= 25)
                    p_Targets.resize(2);
                else if (l_Size >= 26 && l_Size <= 30)
                    p_Targets.resize(3);
            }

            void HandleOnDummy(SpellEffIndex /**/)
            {
                if (!GetHitUnit() || !GetCaster())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellCallOfNightAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_call_of_night_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_call_of_night_aoe_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_call_of_night_aoe_SpellScript();
        }
};

/// Call of Night Periodic Aura - 218809
class spell_tnh_call_of_night_periodic_aura : public SpellScriptLoader
{
    public:
        spell_tnh_call_of_night_periodic_aura() : SpellScriptLoader("spell_tnh_call_of_night_periodic_aura")
        {}

        class spell_call_of_night_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_call_of_night_periodic_AuraScript);

            bool Load() override
            {
                m_TimerCheck = 0;
                return true;
            }

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetUnitOwner() || !GetCaster())
                    return;

                Unit* l_Owner = GetUnitOwner();

                m_TimerCheck += 500;

                if (m_TimerCheck >= 10 * IN_MILLISECONDS)
                {
                    m_TimerCheck = 0;

                    /// Naturalist First Dead
                    if ((g_FormsDead.front().m_FirstDead || IsArcanistLastAlive()) && !IsSolaristLastAlive())
                        GetCaster()->CastSpell(l_Owner, eSpells::SpellToxicSporesArea, true);
                    else if (g_FormsDead.at(1).m_FirstDead || IsSolaristLastAlive()) /// Arcanist First Dead
                        GetCaster()->SummonCreature(eCreatures::NpcSolarCollapseStalker, *l_Owner, TEMPSUMMON_TIMED_DESPAWN, 7500);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_call_of_night_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            uint32 m_TimerCheck;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_call_of_night_periodic_AuraScript();
        }
};

/// Call of Night Area Cheker- 218820
class spell_nh_telarn_call_of_night_area_checker : public SpellScriptLoader
{
    public:
        spell_nh_telarn_call_of_night_area_checker() : SpellScriptLoader("spell_nh_telarn_call_of_night_area_checker")
        {}

        class spell_call_of_night_area_checker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_call_of_night_area_checker_SpellScript);

            void CheckForPlayers(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([this](WorldObject*& p_Itr)
                {
                    return !p_Itr->IsPlayer() || (p_Itr->GetGUID() == GetCaster()->GetGUID());
                });

                bool l_Exist;

                InstanceScript* l_Instance = GetCaster()->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                l_Exist = std::any_of(p_Targets.begin(), p_Targets.end(), JadeCore::UnitAuraCheck(true, eSpells::SpellCallOfNightAura));

                Unit* l_Boss = sObjectAccessor->GetUnit(*GetCaster(), l_Instance->GetData64(eCreatures::BossHighBotanistTelarn));

                for (auto & p_Itr : g_BotanistEntries)
                {
                    if (l_Boss != nullptr)
                    {
                        if (p_Targets.empty() || l_Exist)
                        {
                            l_Boss->CastSpell(GetCaster(), eSpells::SpellArcaneEclipse, true);
                            return;
                        }
                    }

                    l_Boss = sObjectAccessor->GetUnit(*GetCaster(), l_Instance->GetData64(p_Itr));
                }
            }

            void CheckForLashers(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([this](WorldObject*& p_Itr)
                {
                    return p_Itr->GetGUID() == GetCaster()->GetGUID();
                });

                InstanceScript* l_Instance = GetCaster()->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                bool l_Exist = std::any_of(p_Targets.begin(), p_Targets.end(), JadeCore::UnitEntryCheck(-eCreatures::NpcParasiticLasher, 0, 0));
                bool l_PlayerNear = std::any_of(p_Targets.begin(), p_Targets.end(), JadeCore::UnitTypeCheck(true, TYPEID_PLAYER));

                Unit* l_Boss = sObjectAccessor->GetUnit(*GetCaster(), l_Instance->GetData64(eCreatures::BossHighBotanistTelarn));

                for (auto & p_Itr : g_BotanistEntries)
                {
                    if (l_Boss != nullptr)
                    {
                        if (!l_PlayerNear || l_Exist)
                        {
                            l_Boss->CastSpell(GetCaster(), eSpells::SpellArcaneEclipse, true);
                            return;
                        }
                    }

                    l_Boss = sObjectAccessor->GetUnit(*GetCaster(), l_Instance->GetData64(p_Itr));
                }
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->IsPlayer())
                    CheckForPlayers(p_Targets);
                else if (IsNaturalistLastAlive())
                    CheckForLashers(p_Targets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_call_of_night_area_checker_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_call_of_night_area_checker_SpellScript();
        }
};

/// Toxic Spores Aoe - 219049
class spell_nh_telarn_toxic_spores_aoe : public SpellScriptLoader
{
    public:
        spell_nh_telarn_toxic_spores_aoe() : SpellScriptLoader("spell_nh_telarn_toxic_spores_aoe")
        {}

        class spell_toxic_spores_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toxic_spores_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                RangedFilter l_Cmp;

                p_Targets.sort([&l_Cmp](WorldObject*& p_Itr, WorldObject*& p_Itr2)
                {
                    return l_Cmp(p_Itr->ToPlayer()) && !l_Cmp(p_Itr2->ToPlayer());
                });

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleOnDummy(SpellEffIndex /**/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                /// Solarist First Dead
                if (g_FormsDead.back().m_FirstDead || IsNaturalistLastAlive())
                    GetCaster()->SummonCreature(eCreatures::NpcSolarCollapseStalker, *GetHitUnit(), TEMPSUMMON_TIMED_DESPAWN, 7500);

                GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellToxicSporesArea, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toxic_spores_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_toxic_spores_aoe_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_toxic_spores_aoe_SpellScript();
        }
};

/// Summon Chaos Spheres - 223034
class spell_tnh_telarn_summon_chaos_spheres : public SpellScriptLoader
{
    public:
        spell_tnh_telarn_summon_chaos_spheres() : SpellScriptLoader("spell_tnh_telarn_summon_chaos_spheres")
        {}

        class spell_summon_chaos_spheres_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_chaos_spheres_SpellScript);

            void HandleOnDummy(SpellEffIndex /**/)
            {
                Creature* l_ChaosSphere = nullptr;
                uint32 l_Id = GetSpellInfo()->Id;

                for (uint8 l_Itr = 0; l_Itr < 3; ++l_Itr)
                {
                    if (l_Id == eSpells::SpellChaosSpheres)
                        l_ChaosSphere = GetCaster()->SummonCreature(eCreatures::NpcChaosSphere, g_PlasmaSpherePos[l_Itr], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    else if (l_Id == eSpells::SpellChaoticSpheresNature)
                        l_ChaosSphere = GetCaster()->SummonCreature(eCreatures::NpcChaoticSphere, g_PlasmaSpherePos[l_Itr], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                    if (l_ChaosSphere != nullptr)
                    {
                        l_ChaosSphere->DisableHealthRegen();
                        l_ChaosSphere->CastSpell(l_ChaosSphere, eSpells::SpellPlasmaSpheresDot, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_summon_chaos_spheres_SpellScript::HandleOnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_chaos_spheres_SpellScript();
        }
};

/// Recursive Strikes Debuff - 218503
class spell_tnh_telarn_recursive_strikes_aura : public SpellScriptLoader
{
    public:
        spell_tnh_telarn_recursive_strikes_aura() : SpellScriptLoader("spell_tnh_telarn_recursive_strikes_aura")
        {}

        class spell_tnh_telarn_recursive_strikes_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tnh_telarn_recursive_strikes_aura_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (IsArcanistLastAlive())
                {
                    Creature* l_Ptr = GetCaster()->SummonCreature(eCreatures::NpcChaosSphere, *GetUnitOwner(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                    if (l_Ptr != nullptr && l_Ptr->IsAIEnabled)
                    {
                        l_Ptr->DisableHealthRegen();
                        l_Ptr->GetAI()->DoAction(eTelarnActions::ActionEmergeLasher);
                    }

                    GetCaster()->CastSpell(GetCaster(), eSpells::SpellChaosSpheres, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tnh_telarn_recursive_strikes_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tnh_telarn_recursive_strikes_aura_AuraScript();
        }
};

/// Recursive Strikes - 218508
class spell_tnh_telarn_recursirve_strikes : public SpellScriptLoader
{
    public:
        spell_tnh_telarn_recursirve_strikes() : SpellScriptLoader("spell_tnh_telarn_recursirve_strikes")
        {}

        class spell_recursive_strikes_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_recursive_strikes_weapon_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitDamage() > 0)
                {
                    if (AuraEffect* l_AurEff = GetHitUnit()->GetAuraEffect(eSpells::SpellRecursiveStrikesPlayer, EFFECT_0))
                    {
                        float l_Mod = l_AurEff->GetAmount() / 100.f + 1.0f;
                        SetHitDamage(GetHitDamage() * l_Mod);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_recursive_strikes_weapon_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_recursive_strikes_weapon_SpellScript();
        }
};

/// Plasma Explosion - 218780
class spell_nh_telarn_plasma_explosion : public SpellScriptLoader
{
    public:
        spell_nh_telarn_plasma_explosion() : SpellScriptLoader("spell_nh_telarn_plasma_explosion")
        {}

        class spell_nh_telarn_plasma_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nh_telarn_plasma_explosion_SpellScript);

            void HandleDamage(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                if (Aura* l_Aura = GetHitUnit()->GetAura(eSpells::SpellPlasmaExplosion))
                {
                    uint8 l_Stacks = l_Aura->GetStackAmount();
                    
                    if (l_Stacks > 0)
                        SetHitDamage(GetHitDamage() * (1.0 * l_Stacks));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_nh_telarn_plasma_explosion_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nh_telarn_plasma_explosion_SpellScript();
        }
};

/// Parasitic Fetter Area - 218357
class at_nh_telarn_parasitic_fetter : public AreaTriggerEntityScript
{
    public:
        at_nh_telarn_parasitic_fetter() : AreaTriggerEntityScript("at_nh_telarn_parasitic_fetter")
        {}

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_At->GetCaster()->HasAura(eSpells::SpellSubmerge))
                return false;

            if (p_Target->IsPlayer() && p_Target->HasAura(eSpells::SpellParasiticFixate, p_At->GetTargetGuid()))
            {
                p_At->GetCaster()->GetMotionMaster()->Clear();

                p_At->GetCaster()->CastSpell(p_Target, eSpells::SpellParasiticFetterDot, true);

                Unit* l_Owner = sObjectAccessor->GetUnit(*p_At, p_At->GetTargetGuid());

                p_Target->RemoveAurasDueToSpell(eSpells::SpellParasiticFixate, p_At->GetTargetGuid());

                if (l_Owner != nullptr)
                    l_Owner->CastSpell(l_Owner, eSpells::SpellSubmerge, true);
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nh_telarn_parasitic_fetter();
        }
};

/// Grace of Nature - 218927
class at_nh_telarn_grace_of_nature : public AreaTriggerEntityScript
{
    public:
        at_nh_telarn_grace_of_nature() : AreaTriggerEntityScript("at_nh_telarn_grace_of_nature")
        {}

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            switch (p_Target->GetEntry())
            {
                case eCreatures::NpcChaosSphere:
                case eCreatures::NpcPlasmaSphere:
                case eCreatures::NpcChaoticSphere:
                    return true;

                default: return false;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nh_telarn_grace_of_nature();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_high_botanist_telarn()
{
    /// Boss
    new boss_high_botanist_telarn();

    /// Npcs
    new npc_nh_telarn_parasitic_lasher();
    new npc_nh_telarn_solarist_telarn();
    new npc_nh_telarn_naturalist_telarn();
    new npc_tnh_telarn_arcanist_telarn();
    new npc_nh_telarn_controlled_chaos_stalker();
    new npc_nh_telarn_solar_collapse_stalker();
    new npc_tnh_telarn_plasma_sphere();
    new npc_tnh_telarn_toxic_spore();

    /// Spells
    new spell_tnh_telarn_recursirve_strikes();
    new spell_nh_telarn_controlled_chaos_aoe();
    new spell_nh_telarn_chaotic_nature();
    new spell_nh_telarn_solar_collapse_aoe();
    new spell_nh_telarn_parasitic_fixate();
    new spell_nh_telarn_parasitic_fetter_aoe();
    new spell_nh_telarn_parasitic_fetter_dot();
    new spell_nh_telarn_summon_plasma_spheres();
    new spell_nh_telarn_plasma_explosion();
    new spell_nh_telarn_grace_of_nature_aura();
    new spell_nh_telarn_call_of_night_aoe();
    new spell_tnh_call_of_night_periodic_aura();
    new spell_nh_telarn_call_of_night_area_checker();
    new spell_nh_telarn_toxic_spores_aoe();
    new spell_tnh_telarn_summon_chaos_spheres();
    new spell_tnh_telarn_recursive_strikes_aura();
    new spell_nh_telarn_flare();

    /// AreaTrigger
    new at_nh_telarn_parasitic_fetter();
    new at_nh_telarn_grace_of_nature();
}
#endif
