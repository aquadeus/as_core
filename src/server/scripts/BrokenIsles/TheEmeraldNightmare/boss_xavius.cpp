////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_emerald_nightmare.hpp"

enum Says
{
    SAY_AGGRO               = 0,
    SAY_NIGHTMARE_BLADES    = 1,
    SAY_MANIFEST_CORRUPTION = 2,
    SAY_DARKENING_SOUL      = 3,
    SAY_PEREPHASE           = 4,
    SAY_DEATH               = 5
};

enum Spells
{
    SPELL_XAVIUS_ENERGIZE               = 226192,
    SPELL_XAVIUS_ENERGIZE_PHASE_1       = 226184,
    SPELL_XAVIUS_ENERGIZE_PHASE_2       = 226193,
    SPELL_XAVIUS_ENERGIZE_PHASE_3       = 226185,
    SPELL_UNFATHOMABLE_REALITY          = 207160,
    SPELL_THE_INFINITE_DARK             = 211634,

    /// Ysera
    SPELL_DREAM_CONVERSATION            = 222713,
    SPELL_HYPNOPHOBIA_DEBUFF            = 206242,

    /// Phase 1
    SPELL_LURKING_ERUPTION              = 208322,
    SPELL_MANIFEST_CORRUPTION           = 210264,
    SPELL_DARKENING_SOUL                = 206651,
    SPELL_DARKENING_SOUL_AOE            = 207859,
    SPELL_DARKENING_SOUL_ENERGY         = 206652,
    SPELL_NIGHTMARE_BLADES_SUM          = 206653,
    SPELL_NIGHTMARE_BLADES_MARK         = 209001,
    SPELL_NIGHTMARE_BLADES_MARK_2       = 211802,
    SPELL_NIGHTMARE_BLADES_DMG          = 206656,

    /// Phase 2
    SPELL_BLACKENING_SOUL               = 209158,
    SPELL_BLACKENED                     = 205612,
    SPELL_BLACKENED_TAINTING_ENERGY     = 207853,
    SPELL_NIGHTMARE_INFUSION            = 209443,
    SPELL_CALL_OF_NIGHTMARES            = 205588, ///< Energy cost
    SPELL_TAINTED_DISCHARGE_TRIG_AT     = 208362,
    SPELL_TAINTED_DISCHARGE_AT          = 208363,
    SPELL_CORRUPTION_METEOR             = 206308,
    SPELL_CORRUPTION_METEOR_MISSILE     = 206341,
    SPELL_CORRUPTION_METEOR_AURA        = 224508,

    /// Phase 3
    SPELL_WRITHING_DEEP                 = 226194, ///< Energy cost

    /// Phase 2: Heroic
    SPELL_BONDS_OF_TERROR               = 209032,
    SPELL_BONDS_OF_TERROR_AURA          = 209034,
    SPELL_BONDS_OF_TERROR_AURA_2        = 210451,

    /// Player spells
    SPELL_NIGHTMARE_TORMENT_ALT_POWER   = 189960,
    SPELL_NIGHTMARE_TORMENT_TICK        = 226227,
    SPELL_THE_DREAMING_SUM_CLONE        = 206000,
    SPELL_THE_DREAMING_CLONE_IMAGE      = 206002,
    SPELL_DREAM_SIMULACRUM              = 206005,
    SPELL_AWAKENING_DREAM               = 207634,
    SPELL_INSANITY_LEVEL_1              = 210203,
    SPELL_INSANITY_LEVEL_2              = 210204,
    SPELL_DESCENT_INTO_MADNESS          = 208431,
    SPELL_CORRUPTION_MADNESS            = 207409,

    /// Player clone spells
    SPELL_DREAMING_CLONE_ABORB          = 189449,

    /// Dread Abomination
    SPELL_CORRUPTION_CRUSHING_SHADOWS   = 208748,

    /// Lurking Terror
    SPELL_TORMENTING_FIXATION_FILTER    = 205770,
    SPELL_TORMENTING_FIXATION           = 205771,
    SPELL_TORMENTING_INFECTION_AT       = 217989, ///< Normal
    SPELL_TORMENTING_INFECTION_DMG      = 217990,
    SPELL_TORMENTING_DETONATION_AT      = 205780, ///< Heroic
    SPELL_TORMENTING_DETONATION_DMG     = 205782,

    /// Corruption Horror
    SPELL_CORRUPTION_HORROR_BIRTH       = 213345,
    SPELL_TORMENTING_SWIPE              = 224649,
    SPELL_CORRUPTING_NOVA               = 207830,

    /// Inconceivable Horror
    SPELL_DARK_RUINATION                = 209288,
    SPELL_TAINTED_DISCHARGE_SUM         = 212124, ///< Summon Inconceivable Horror
    SPELL_OUT_OF_THE_SHADOWS            = 209240,

    /// Nightmare Tentacle
    SPELL_NIGHTMARE_BOLT                = 206920,

    /// Mythic
    SPELL_FONT_OF_DREAMING_MISSILE      = 210832,
    SPELL_DEFILED_FONT                  = 213515,
    SPELL_NIGHTMARE_REVERBERATIONS      = 189448
};

enum eEvents
{
    EVENT_ABOMINATION_CRUSHING          = 1,
    EVENT_DARKENING_SOUL                = 2,
    EVENT_NIGHTMARE_BLADES              = 3,
    EVENT_BLACKENING_SOUL               = 4,
    EVENT_NIGHTMARE_INFUSION            = 5,
    EVENT_CORRUPTION_METEOR             = 6,
    EVENT_CHECK_ALT_POWER               = 7,
    EVENT_CHECK_MIST                    = 8,
    EVENT_CHECK_PLAYERS                 = 9,

    /// Heroic
    EVENT_BONDS_OF_TERROR
};

enum ePhase
{
    PHASE_1     = 0,
    PHASE_2,
    PHASE_3
};

Position const g_DreadPos[8] =
{
    { -3061.0f, -4959.0f, 147.78f, 5.4f },
    { -2919.0f, -5101.0f, 147.78f, 2.3f },
    { -3090.0f, -5030.0f, 147.78f, 0.0f },
    { -3061.0f, -5101.0f, 147.78f, 0.7f },
    { -2990.0f, -5130.0f, 147.78f, 1.5f },
    { -2919.0f, -4959.0f, 147.78f, 3.9f },
    { -2990.0f, -4930.0f, 147.78f, 4.7f },
    { -2890.0f, -5030.0f, 147.78f, 3.1f }
};

Position const g_CenterPos = { -2990.16f, -5029.85f, 147.78f, 0.0f };

Position const g_EventPos[10] =
{
    { -2953.67f, -5117.50f, 147.70f, 0.0f },
    { -2938.12f, -4980.82f, 147.70f, 0.0f },
    { -3069.40f, -4997.82f, 147.70f, 0.0f },
    { -2960.12f, -5043.86f, 147.70f, 0.0f },
    { -2995.12f, -4992.84f, 147.70f, 0.0f },
    { -3015.60f, -5056.74f, 147.70f, 0.0f },
    { -3030.53f, -5054.32f, 147.70f, 0.0f },
    { -2971.15f, -5066.08f, 147.70f, 0.0f },
    { -2956.51f, -4994.40f, 147.70f, 0.0f },
    { -3030.22f, -4992.07f, 147.70f, 0.0f }
};

/// Xavius <Nightmare Lord> - 103769
class boss_xavius : public CreatureScript
{
    public:
        boss_xavius() : CreatureScript("boss_xavius") { }

        struct boss_xaviusAI : public BossAI
        {
            boss_xaviusAI(Creature* p_Creature) : BossAI(p_Creature, DATA_XAVIUS)
            {
                m_Intro = true;
            }

            bool m_Intro;
            uint8 m_HealthPct;
            uint8 m_HealthPhasePct;
            uint8 m_EventPhase;

            uint64 m_YseraGuid;

            void Reset() override
            {
                _Reset();
                m_HealthPct = 95;
                m_HealthPhasePct = 65;
                m_EventPhase = 0;
                me->SetPower(POWER_ENERGY, 30);

                me->SetFullHealth();

                if (instance && instance->GetData(DATA_PRE_EVENT_XAVIUS) == DONE) ///< Only after pre-event boss can start to do any things
                {
                    if (m_Intro)
                    {
                        m_Intro = false;
                        me->SetVisible(true);
                        me->GetMotionMaster()->MovePoint(0, { -3019.56f, -5060.41f, 147.70f, 0.0f });
                        me->SetHomePosition({ -3019.56f, -5060.41f, 147.70f, me->m_orientation });
                    }

                    me->SetReactState(REACT_AGGRESSIVE);

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        for (uint8 l_I = 0; l_I < 8; l_I++)
                            me->SummonCreature(NPC_DREAD_ABOMINATION, g_DreadPos[l_I]);
                    });
                }
                else
                {
                    me->SetVisible(false);
                    me->SetReactState(REACT_PASSIVE);
                }

                m_YseraGuid = 0;

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                ClearPlayerAuras();
                DoCast(me, SPELL_XAVIUS_ENERGIZE_PHASE_1, true);
                DoCast(me, SPELL_UNFATHOMABLE_REALITY, true);
                DoCast(me, SPELL_INSANITY_LEVEL_2, true);

                if (instance)
                    instance->DoCastSpellOnPlayers(SPELL_NIGHTMARE_TORMENT_ALT_POWER);

                StartEvents(PHASE_1);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
                ClearPlayerAuras();
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                ClearPlayerAuras();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                me->InterruptNonMeleeSpells(true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(DATA_XAVIUS, EncounterState::FAIL);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 3.6670056f;
            }

            void StartEvents(uint8 p_Phase)
            {
                events.Reset();
                m_EventPhase = p_Phase;

                switch (p_Phase)
                {
                    case PHASE_1:
                    {
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLADES, 18000);
                        events.ScheduleEvent(EVENT_DARKENING_SOUL, 7000);
                        break;
                    }
                    case PHASE_2:
                    {
                        events.ScheduleEvent(EVENT_BLACKENING_SOUL, 7000);
                        events.ScheduleEvent(EVENT_CORRUPTION_METEOR, 21000);
                        events.ScheduleEvent(EVENT_NIGHTMARE_INFUSION, 32000);

                        if (IsHeroicOrMythic())
                            events.ScheduleEvent(EVENT_BONDS_OF_TERROR, 12000);

                        break;
                    }
                    case PHASE_3:
                    {
                        events.ScheduleEvent(EVENT_BLACKENING_SOUL, 7000);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLADES, 18000);
                        events.ScheduleEvent(EVENT_CORRUPTION_METEOR, 21000);
                        events.ScheduleEvent(EVENT_NIGHTMARE_INFUSION, 32000);
                        break;
                    }
                    default:
                        break;
                }

                events.ScheduleEvent(EVENT_ABOMINATION_CRUSHING, 12000);
                events.ScheduleEvent(EVENT_CHECK_ALT_POWER, 2000);
                events.ScheduleEvent(EVENT_CHECK_MIST, 1000);
                events.ScheduleEvent(EVENT_CHECK_PLAYERS, 1000);
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Attacker->IsPlayer() && !p_Attacker->HasAura(SPELL_NIGHTMARE_TORMENT_ALT_POWER))
                {
                    p_Damage = 0;
                    return;
                }

                if (m_HealthPct && me->HealthBelowPct(m_HealthPct))
                {
                    if (IsMythic())
                    {
                        if (m_HealthPct > 80)
                            m_HealthPct = 80;
                        else if (m_HealthPct > 60)
                            m_HealthPct = 60;
                        else if (m_HealthPct > 45)
                            m_HealthPct = 45;
                        else
                            m_HealthPct = 0;

                        float l_Angle   = 0.0f;
                        float l_Dist    = 30.0f;

                        for (uint8 l_I = 0; l_I < 5; ++l_I)
                        {
                            Position l_Pos = g_CenterPos;

                            l_Pos.m_positionX += l_Dist * std::cos(l_Angle);
                            l_Pos.m_positionY += l_Dist * std::sin(l_Angle);

                            DoCast(l_Pos, SPELL_FONT_OF_DREAMING_MISSILE, true);

                            l_Angle += (M_PI * 2.0f) / 5.0f;
                        }
                    }
                    else
                    {
                        bool l_FirstDream = m_HealthPct > 60;

                        if (m_HealthPct > 60)
                            m_HealthPct = 60;
                        else
                            m_HealthPct = 0;

                        if (Creature* l_Ysera = me->SummonCreature(NPC_YSERA, { -3060.455f, -5107.713f, 193.9954f, 1.857913f }))
                        {
                            m_YseraGuid = l_Ysera->GetGUID();

                            std::list<Player*> l_PlayerList;
                            GetPlayerListInGrid(l_PlayerList, me, 100.0f);

                            if (!l_PlayerList.empty())
                            {
                                auto l_TriggerDream = [&]() -> void
                                {
                                    for (Player* const& l_Player : l_PlayerList)
                                    {
                                        if (!l_Player || !l_Player->isAlive() || l_Player->isGameMaster())
                                            continue;

                                        l_Player->CastSpell(l_Player, SPELL_THE_DREAMING_SUM_CLONE, true);
                                    }
                                };

                                if (!l_FirstDream)
                                {
                                    l_PlayerList.remove_if([this](Player* p_Player) -> bool
                                    {
                                        if (p_Player == nullptr || p_Player->HasAura(SPELL_HYPNOPHOBIA_DEBUFF))
                                            return true;

                                        if (p_Player->HasAura(SPELL_DREAM_SIMULACRUM))
                                            return true;

                                        return false;
                                    });

                                    l_TriggerDream();
                                }

                                int8 l_Count = l_PlayerList.size() > 1 ? l_PlayerList.size() / 2 : 1;

                                std::list<Player*> l_Tanks;
                                std::list<Player*> l_Healers;
                                std::list<Player*> l_Others;

                                /// Half of the raid enters the Dream each time, so every player will see the Dream mechanic
                                /// Ysera will take an even split of roles so there are always tanks and healers in the Dream
                                /// The players taken for the first Dream are chosen at random - the raid cannot influence which players are selected
                                for (Player* l_Player : l_PlayerList)
                                {
                                    switch (l_Player->GetRoleForGroup())
                                    {
                                        case Roles::ROLE_TANK:
                                        {
                                            l_Tanks.push_back(l_Player);
                                            break;
                                        }
                                        case Roles::ROLE_HEALER:
                                        {
                                            l_Healers.push_back(l_Player);
                                            break;
                                        }
                                        case Roles::ROLE_DAMAGE:
                                        default:
                                        {
                                            l_Others.push_back(l_Player);
                                            break;
                                        }
                                    }
                                }

                                l_PlayerList.clear();

                                if (l_Tanks.size() > 1 && l_Count > 0)
                                {
                                    JadeCore::Containers::RandomResizeList(l_Tanks, 1);

                                    l_PlayerList.push_back(l_Tanks.front());

                                    --l_Count;
                                }

                                if (l_Healers.size() > 1 && l_Count > 0)
                                {
                                    JadeCore::RandomResizeList(l_Healers, l_Healers.size() / 2);

                                    for (Player* l_Healer : l_Healers)
                                    {
                                        if (l_Count <= 0)
                                            break;

                                        l_PlayerList.push_back(l_Healer);

                                        --l_Count;
                                    }
                                }

                                if (l_Others.size() > 1 && l_Count > 0)
                                {
                                    JadeCore::RandomResizeList(l_Others, l_Others.size() / 2);

                                    for (Player* l_Other : l_Others)
                                    {
                                        if (l_Count <= 0)
                                            break;

                                        l_PlayerList.push_back(l_Other);

                                        --l_Count;
                                    }
                                }

                                /// If still there is no found player, just select random ones
                                if (l_PlayerList.empty() && l_Count > 0)
                                {
                                    for (Player* l_Player : l_Tanks)
                                        l_PlayerList.push_back(l_Player);

                                    for (Player* l_Player : l_Healers)
                                        l_PlayerList.push_back(l_Player);

                                    for (Player* l_Player : l_Others)
                                        l_PlayerList.push_back(l_Player);

                                    JadeCore::RandomResizeList(l_PlayerList, l_Count);

                                    l_Count = 0;
                                }

                                l_TriggerDream();

                                l_Ysera->m_Functions.AddFunction([l_Ysera]() -> void
                                {
                                    l_Ysera->CastSpell(l_Ysera, SPELL_DREAM_CONVERSATION);
                                }, l_Ysera->m_Functions.CalculateTime(5000));
                            }
                        }
                    }
                }

                /// Regulates events phases
                if (me->HealthBelowPct(m_HealthPhasePct))
                {
                    Talk(SAY_PEREPHASE);
                    if (m_HealthPhasePct > 30) ///< 65%
                    {
                        StartEvents(PHASE_2);
                        m_HealthPhasePct = 30;
                    }
                    else
                    {
                        StartEvents(PHASE_3);
                        m_HealthPhasePct = 0;
                    }
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_CALL_OF_NIGHTMARES)
                {
                    uint8 l_Count = std::max(uint8(1), uint8(me->GetSizeSaveThreat() / 3));
                    Position l_Pos;
                    float l_Angle = 0.0f;
                    for (uint8 l_I = 0; l_I < l_Count; ++l_I)
                    {
                        l_Angle = frand(0, 2.0f * M_PI);
                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 60.0f, l_Angle);
                        me->CastSpell(l_Pos, SPELL_TAINTED_DISCHARGE_SUM, true);
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_NIGHTMARE_BLADES_MARK:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 2, 150.0f, true, -SPELL_NIGHTMARE_BLADES_MARK))
                            DoCast(l_Target, SPELL_NIGHTMARE_BLADES_MARK_2, true);

                        Talk(SAY_NIGHTMARE_BLADES);
                        break;
                    }
                    case SPELL_BONDS_OF_TERROR:
                    {
                        DoCast(p_Target, SPELL_BONDS_OF_TERROR_AURA, true);
                        break;
                    }
                    case SPELL_CORRUPTION_METEOR:
                    {
                        DoCast(p_Target, SPELL_CORRUPTION_METEOR_MISSILE, true);
                        DoCast(p_Target, SPELL_CORRUPTION_METEOR_AURA, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool p_Removed) override
            {
                /// Only re-summon if removed by expire
                if (!p_Removed && p_AreaTrigger->GetSpellId() == SPELL_TAINTED_DISCHARGE_AT)
                    me->CastSpell(*p_AreaTrigger, SPELL_TAINTED_DISCHARGE_SUM, true);
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == DATA_XAVIUS)
                    return m_EventPhase;

                return 0;
            }

            void ClearPlayerAuras()
            {
                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE_TORMENT_ALT_POWER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE_TORMENT_TICK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INSANITY_LEVEL_1);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INSANITY_LEVEL_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DREAM_SIMULACRUM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DESCENT_INTO_MADNESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTION_MADNESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLACKENING_SOUL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONDS_OF_TERROR_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONDS_OF_TERROR_AURA_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HYPNOPHOBIA_DEBUFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_THE_INFINITE_DARK);
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

                switch (events.ExecuteEvent())
                {
                    case EVENT_ABOMINATION_CRUSHING:
                    {
                        EntryCheckPredicate l_Pred(NPC_DREAD_ABOMINATION);
                        summons.DoAction(ACTION_1, l_Pred, 1);
                        events.ScheduleEvent(EVENT_ABOMINATION_CRUSHING, 10000);
                        break;
                    }
                    case EVENT_DARKENING_SOUL:
                    {
                        DoCastVictim(SPELL_DARKENING_SOUL);
                        events.ScheduleEvent(EVENT_DARKENING_SOUL, 8000);
                        if (urand(0, 3) == 3)
                            Talk(SAY_DARKENING_SOUL);
                        break;
                    }
                    case EVENT_NIGHTMARE_BLADES:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, { -SPELL_NIGHTMARE_BLADES_MARK_2, -SPELL_CORRUPTION_METEOR_AURA }))
                        {
                            DoCast(l_Target, SPELL_NIGHTMARE_BLADES_MARK, true);    ///< Invisible marker
                            DoCast(l_Target, SPELL_NIGHTMARE_BLADES_MARK_2, true);  ///< Red arrow
                        }

                        events.ScheduleEvent(EVENT_NIGHTMARE_BLADES, 15000);
                        break;
                    }
                    case EVENT_BLACKENING_SOUL:
                    {
                        DoCastVictim(SPELL_BLACKENING_SOUL);
                        events.ScheduleEvent(EVENT_BLACKENING_SOUL, 7000);
                        break;
                    }
                    case EVENT_NIGHTMARE_INFUSION:
                    {
                        DoCast(me, SPELL_NIGHTMARE_INFUSION, true);
                        events.ScheduleEvent(EVENT_NIGHTMARE_INFUSION, 62000);
                        break;
                    }
                    case EVENT_CORRUPTION_METEOR:
                    {
                        DoCast(me, SPELL_CORRUPTION_METEOR, true);
                        events.ScheduleEvent(EVENT_CORRUPTION_METEOR, 29000);
                        break;
                    }
                    case EVENT_CHECK_ALT_POWER:
                    {
                        Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (l_Player->isAlive() && !l_Player->isGameMaster() && !l_Player->HasAura(SPELL_NIGHTMARE_TORMENT_ALT_POWER))
                                    l_Player->CastSpell(l_Player, SPELL_NIGHTMARE_TORMENT_ALT_POWER, true);
                            }
                        }

                        events.ScheduleEvent(EVENT_CHECK_ALT_POWER, 2000);
                        break;
                    }
                    case EVENT_BONDS_OF_TERROR:
                    {
                        uint8 l_FreePlrCount = 0;
                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();

                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Player* l_Player = me->GetPlayer(*me, l_Ref->getUnitGuid()))
                            {
                                /// Tanks cannot be targeted by this, but all other players are valid targets
                                if (!l_Player->HasAura(SPELL_BONDS_OF_TERROR_AURA) && !l_Player->HasAura(SPELL_BONDS_OF_TERROR_AURA_2) &&
                                    l_Player->GetRoleForGroup() != Roles::ROLE_TANK)
                                    l_FreePlrCount++;
                            }
                        }

                        if (l_FreePlrCount > 1)
                            DoCast(me, SPELL_BONDS_OF_TERROR, true);

                        events.ScheduleEvent(EVENT_BONDS_OF_TERROR, 12000);
                        break;
                    }
                    case EVENT_CHECK_MIST:
                    {
                        if (instance)
                        {
                            if (Creature* l_Rift = Creature::GetCreature(*me, instance->GetData64(NPC_RIFT_OF_ALN)))
                            {
                                Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                                for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                                {
                                    if (Player* l_Player = l_Iter->getSource())
                                    {
                                        if (l_Player->isAlive() && !l_Player->isGameMaster() && l_Player->GetDistance(l_Rift) > 45.0f)
                                            l_Player->CastSpell(l_Player, SPELL_THE_INFINITE_DARK, true);
                                        else
                                            l_Player->RemoveAura(SPELL_THE_INFINITE_DARK);
                                    }
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_CHECK_MIST, 1000);
                        break;
                    }
                    case EVENT_CHECK_PLAYERS:
                    {
                        bool l_PlayerFound = false;
                        std::list<HostileReference*> l_RefList = me->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_RefList)
                        {
                            if (Unit* l_Unit = Unit::GetUnit(*me, l_Ref->getUnitGuid()))
                            {
                                if (l_Unit->IsPlayer())
                                {
                                    l_PlayerFound = true;
                                    break;
                                }
                            }
                        }

                        if (!l_PlayerFound)
                        {
                            EnterEvadeMode();
                            return;
                        }

                        events.ScheduleEvent(EVENT_CHECK_PLAYERS, 1000);
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
            return new boss_xaviusAI (p_Creature);
        }
};

/// Sleeping Version - 104096
class npc_xavius_sleeping_version : public CreatureScript
{
    public:
        npc_xavius_sleeping_version() : CreatureScript("npc_xavius_sleeping_version") { }

        struct npc_xavius_sleeping_versionAI : public ScriptedAI
        {
            npc_xavius_sleeping_versionAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                m_Instance = me->GetInstanceScript();

                m_Init = false;
            }

            InstanceScript* m_Instance;
            uint16 m_CheckStateBoss;

            /// Simulacrum Data
            float                           m_HealthPct;
            std::array<int32, MAX_POWERS>   m_PowerValues;
            bool                            m_Init;

            void Reset() override
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                events.ScheduleEvent(EVENT_1, 1000);
                p_Summoner->CastSpell(me, SPELL_THE_DREAMING_CLONE_IMAGE, true);
                DoCast(me, SPELL_DREAMING_CLONE_ABORB, true);

                if (p_Summoner->IsPlayer())
                {
                    m_HealthPct = p_Summoner->GetHealthPct();

                    for (uint8 l_I = 0; l_I < MAX_POWERS; ++l_I)
                        m_PowerValues[l_I] = p_Summoner->GetPower(Powers(l_I));

                    m_Init = true;

                    /// When players enter the Dream, they are set to zero Corruption.
                    p_Summoner->SetPower(Powers::POWER_ALTERNATE_POWER, 0);
                }

                if (IsMythic())
                    events.ScheduleEvent(EVENT_2, 2000);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_Caster->IsPlayer() && p_SpellInfo->Id == SPELL_AWAKENING_DREAM && m_Init)
                {
                    uint64 l_Guid = p_Caster->GetGUID();
                    AddTimedDelayedOperation(10, [this, l_Guid]() -> void
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                        {
                            /// However, upon leaving the Dream, players are set back to the Corruption levels they had before the Dream began.
                            for (uint8 l_I = 0; l_I < MAX_POWERS; ++l_I)
                                l_Player->SetPower(Powers(l_I), m_PowerValues[l_I]);

                            /// Additionally, any cooldowns used during the Dream are restored when the player awakens
                            l_Player->RemoveSpellCooldownsWithTime(5 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);

                            l_Player->SetHealth(l_Player->CountPctFromMaxHealth(m_HealthPct));
                        }

                        me->DespawnOrUnsummon(10);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (m_Instance && m_Instance->GetBossState(DATA_XAVIUS) != IN_PROGRESS)
                            me->DespawnOrUnsummon();

                        events.ScheduleEvent(EVENT_1, 1000);
                        break;
                    }
                    case EVENT_2:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 2.0f);

                        if (!l_PlayerList.empty())
                            me->CastSpell(*me, SPELL_NIGHTMARE_REVERBERATIONS, true);

                        events.ScheduleEvent(EVENT_2, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_sleeping_versionAI(p_Creature);
        }
};

/// Dread Abomination - 105343
class npc_xavius_dread_abomination : public CreatureScript
{
    public:
        npc_xavius_dread_abomination() : CreatureScript("npc_xavius_dread_abomination") { }

        struct npc_xavius_dread_abominationAI : public ScriptedAI
        {
            npc_xavius_dread_abominationAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetVisible(false);
                me->CastSpell(me, SPELL_INSANITY_LEVEL_1, true);
            }

            void Reset() override { }

            void DoAction(int32 const /*p_Action*/) override
            {
                events.ScheduleEvent(EVENT_1, 100);
            }

            bool RegulatePersonnalVisibility()
            {
                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    std::list<HostileReference*> l_ThreatList = l_Owner->getThreatManager().getThreatList();

                    if (!l_ThreatList.empty())
                    {
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Player* l_Player = me->GetPlayer(*me, l_Ref->getUnitGuid()))
                            {
                                if (l_Player->HasAura(SPELL_INSANITY_LEVEL_1))
                                {
                                    if (!me->IsPlayerInPersonnalVisibilityList(l_Player->GetGUID()))
                                        me->AddPlayerInPersonnalVisibilityList(l_Player->GetGUID());
                                }
                            }
                        }

                        return true;
                    }
                }

                events.Reset();
                me->SetVisible(false);
                return false;
            }

            void OnSendDisplayID(uint32& p_DisplayID, Player* p_Target) override
            {
                if (me->IsPlayerInPersonnalVisibilityList(p_Target->GetGUID()) || p_Target->isGameMaster())
                    p_DisplayID = me->GetCreatureTemplate()->Modelid[0];
                else
                    p_DisplayID = me->GetCreatureTemplate()->Modelid[1];
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (RegulatePersonnalVisibility())
                            events.ScheduleEvent(EVENT_2, 500);
                        break;
                    }
                    case EVENT_2:
                    {
                        me->SetVisible(true);
                        events.ScheduleEvent(EVENT_3, 1000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_CORRUPTION_CRUSHING_SHADOWS);
                        events.ScheduleEvent(EVENT_4, 5000);
                        break;
                    }
                    case EVENT_4:
                    {
                        me->SetVisible(false);
                        me->ClearVisibleOnlyForSomePlayers();
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
            return new npc_xavius_dread_abominationAI(p_Creature);
        }
};

/// Lurking Terror - 103694
class npc_xavius_lurking_terror : public CreatureScript
{
    public:
        npc_xavius_lurking_terror() : CreatureScript("npc_xavius_lurking_terror") { }

        struct npc_xavius_lurking_terrorAI : public ScriptedAI
        {
            npc_xavius_lurking_terrorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.6f);
                me->SetSpeed(MOVE_WALK, 0.6f);
            }

            uint64 m_TargetGUID = 0;
            uint64 m_ExcludeGUID = 0;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (Player* l_Player = me->FindNearestPlayer(1.0f))
                    m_ExcludeGUID = l_Player->GetGUID();

                events.ScheduleEvent(EVENT_1, 2000);
                events.ScheduleEvent(EVENT_3, 3000);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.665f;
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_TORMENTING_FIXATION_FILTER)
                {
                    DoCast(p_Target, SPELL_TORMENTING_FIXATION, true);
                    m_TargetGUID = p_Target->GetGUID();
                    events.ScheduleEvent(EVENT_2, 1000);
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Spell->GetSpellInfo()->Id != SPELL_TORMENTING_FIXATION_FILTER || p_Targets.empty() || !m_ExcludeGUID)
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetGUID() == m_ExcludeGUID)
                        return true;

                    return false;
                });
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_TORMENTING_INFECTION_DMG || p_SpellInfo->Id == SPELL_TORMENTING_DETONATION_DMG)
                {
                    events.Reset();
                    me->GetMotionMaster()->Clear();
                    me->DespawnOrUnsummon(1000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_TORMENTING_FIXATION_FILTER, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGUID))
                        {
                            me->GetMotionMaster()->MovePoint(1, *l_Target);
                            events.ScheduleEvent(EVENT_2, 500);
                        }
                        else
                            events.ScheduleEvent(EVENT_1, 1000);

                        break;
                    }
                    case EVENT_3:
                    {
                        if (!IsMythic())
                            DoCast(me, SPELL_TORMENTING_INFECTION_AT, true);
                        else
                            DoCast(me, SPELL_TORMENTING_DETONATION_AT, true);

                        events.ScheduleEvent(EVENT_4, 500);
                        break;
                    }
                    case EVENT_4:
                    {
                        if (Player* l_Target = me->FindNearestPlayer(0.5f))
                        {
                            if (!IsMythic())
                                me->CastSpell(l_Target, SPELL_TORMENTING_INFECTION_DMG, true);
                            else
                                me->CastSpell(l_Target, SPELL_TORMENTING_DETONATION_DMG, true);

                            break;
                        }

                        events.ScheduleEvent(EVENT_4, 200);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_lurking_terrorAI(p_Creature);
        }
};

/// Corruption Horror - 103695
class npc_xavius_corruption_horror : public CreatureScript
{
    public:
        npc_xavius_corruption_horror() : CreatureScript("npc_xavius_corruption_horror") { }

        struct npc_xavius_corruption_horrorAI : public ScriptedAI
        {
            npc_xavius_corruption_horrorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_CORRUPTION_HORROR_BIRTH, true);
                DoZoneInCombat(me, 100.0f);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 4.6f;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 9000);
                events.ScheduleEvent(EVENT_2, 14000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCastVictim(SPELL_TORMENTING_SWIPE);
                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_CORRUPTING_NOVA);
                        events.ScheduleEvent(EVENT_2, 20000);
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
            return new npc_xavius_corruption_horrorAI(p_Creature);
        }
};

/// Nightmare Blades - 104422
class npc_xavius_nightmare_blades : public CreatureScript
{
    public:
        npc_xavius_nightmare_blades() : CreatureScript("npc_xavius_nightmare_blades") { }

        struct npc_xavius_nightmare_bladesAI : public ScriptedAI
        {
            npc_xavius_nightmare_bladesAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            std::list<uint64> m_GuidList;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_GuidList.clear();
                events.ScheduleEvent(EVENT_1, 200);
            }

            bool GetObjectData(uint64 const& p_Guid) override
            {
                bool l_Find = false;

                for (uint64 const& l_Guid : m_GuidList)
                {
                    if (l_Guid == p_Guid)
                        l_Find = true;
                }

                if (!l_Find)
                    m_GuidList.push_back(p_Guid);

                return l_Find;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (!l_Owner->IsAIEnabled)
                                break;

                            if (Unit* l_Target = l_Owner->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true, SPELL_NIGHTMARE_BLADES_MARK))
                            {
                                Position l_Pos;
                                float l_Angle = l_Target->GetRelativeAngle(me);
                                float l_Dist = me->GetDistance(l_Target) + 20.0f;
                                l_Target->GetNearPosition(l_Pos, l_Dist, l_Angle);

                                float l_Angle2 = l_Pos.GetRelativeAngle(l_Target);
                                float l_Dist2 = 8.0f;
                                for (uint8 l_I = 0; l_I < 18; ++l_I)
                                {
                                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_Dist2, l_Angle2);
                                    me->m_Events.AddEvent(new DelayDestCastEvent(*me, l_Pos, SPELL_NIGHTMARE_BLADES_DMG, true), me->m_Events.CalculateTime(l_I * 150));
                                }
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_nightmare_bladesAI(p_Creature);
        }
};

/// Inconceivable Horror - 105611
class npc_xavius_inconceivable_horror : public CreatureScript
{
    public:
        npc_xavius_inconceivable_horror() : CreatureScript("npc_xavius_inconceivable_horror") { }

        struct npc_xavius_inconceivable_horrorAI : public ScriptedAI
        {
            npc_xavius_inconceivable_horrorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.3f);
                me->SetSpeed(MOVE_WALK, 0.3f);
            }

            bool m_MovementStopped;

            void Reset() override
            {
                m_MovementStopped = true;

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetDisplayId(11686);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);

                events.ScheduleEvent(EVENT_1, 500);
                events.ScheduleEvent(EVENT_2, 500);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 5.66666f;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (l_Owner->isInCombat())
                        l_Owner->CastSpell(*me, SPELL_TAINTED_DISCHARGE_TRIG_AT, true);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 100)
                {
                    if (Unit* l_Owner = me->GetAnyOwner())
                    {
                        if (me->GetDistance(l_Owner) > 0.0f)
                            m_MovementStopped = true;
                        else
                        {
                            me->GetMotionMaster()->Clear();
                            me->SetFacingToObject(l_Owner);

                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                DoCast(SPELL_DARK_RUINATION);
                            });
                        }
                    }
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_DARK_RUINATION)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        DoCast(SPELL_DARK_RUINATION);
                    });
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = baseMoveSpeed[MOVE_RUN] * 0.3f;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (me->GetDistance(l_Owner) > 0.0f)
                            {
                                if (m_MovementStopped && !me->HasBreakableByDamageCrowdControlAura())
                                {
                                    m_MovementStopped = false;

                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MovePoint(100, *l_Owner);
                                }
                            }
                            else
                                me->StopMoving();
                        }

                        events.ScheduleEvent(EVENT_1, 500);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (me->GetDistance(g_CenterPos) <= 40.0f)
                        {
                            me->RestoreDisplayId();
                            DoCast(me, SPELL_OUT_OF_THE_SHADOWS, true);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        else
                            events.ScheduleEvent(EVENT_2, 500);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_inconceivable_horrorAI(p_Creature);
        }
};

/// Nightmare Tentacle - 104592
class npc_xavius_nightmare_tentacle : public CreatureScript
{
    public:
        npc_xavius_nightmare_tentacle() : CreatureScript("npc_xavius_nightmare_tentacle") { }

        struct npc_xavius_nightmare_tentacleAI : public ScriptedAI
        {
            npc_xavius_nightmare_tentacleAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 500);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 3.316667f;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            DoCast(l_Target, SPELL_NIGHTMARE_BOLT);
                        events.ScheduleEvent(EVENT_1, urand(3, 5) * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_nightmare_tentacleAI(p_Creature);
        }
};

/// Rift of Aln - 109847
class npc_xavius_event_conroller : public CreatureScript
{
    public:
        npc_xavius_event_conroller() : CreatureScript("npc_xavius_event_conroller") { }

        struct npc_xavius_event_conrollerAI : public ScriptedAI
        {
            npc_xavius_event_conrollerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                m_CountAdds = 0;
                m_CheckEvade = 1000;
                SetCanSeeEvenInPassiveMode(true);

                if (m_Instance && m_Instance->GetData(DATA_PRE_EVENT_XAVIUS) == NOT_STARTED)
                {
                    for (int l_I = 0; l_I < 9; ++l_I)
                        me->SummonCreature(NPC_SMALL_TRASH, g_EventPos[6].GetPositionX() + irand(-15, 15), g_EventPos[6].GetPositionY() + irand(-15, 15), g_EventPos[6].GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                    for (int l_I = 0; l_I < 9; ++l_I)
                        me->SummonCreature(NPC_SMALL_TRASH, g_EventPos[7].GetPositionX() + irand(-15, 15), g_EventPos[7].GetPositionY() + irand(-15, 15), g_EventPos[7].GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                    for (int l_I = 0; l_I < 9; ++l_I)
                        me->SummonCreature(NPC_SMALL_TRASH, g_EventPos[8].GetPositionX() + irand(-15, 15), g_EventPos[8].GetPositionY() + irand(-15, 15), g_EventPos[8].GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                    for (int l_I = 0; l_I < 9; ++l_I)
                        me->SummonCreature(NPC_SMALL_TRASH, g_EventPos[9].GetPositionX() + irand(-15, 15), g_EventPos[9].GetPositionY() + irand(-15, 15), g_EventPos[9].GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                }
            }

            InstanceScript* m_Instance; ///< DATA_PRE_EVENT_XAVIUS

            uint8 m_CountAdds;
            uint32 m_CheckEvade;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (m_Instance && m_Instance->GetData(DATA_PRE_EVENT_XAVIUS) != NOT_STARTED)
                    return;

                if (me->GetExactDist2d(p_Who) <= 17.0f)
                    return;

                events.Reset();

                if (m_Instance)
                    m_Instance->SetData(DATA_PRE_EVENT_XAVIUS, IN_PROGRESS);

                std::list<Creature*> l_Adds;
                GetCreatureListWithEntryInGrid(l_Adds, me, NPC_SMALL_TRASH, 150.0f);
                for (Creature* l_Cre : l_Adds)
                {
                    l_Cre->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    l_Cre->GetMotionMaster()->MovePoint(0, (2.0f * l_Cre->GetPositionX() - me->GetPositionX()), (2.0 * l_Cre->GetPositionY() - me->GetPositionY()), l_Cre->GetPositionZ()); ///< Way for nice despawn
                    l_Cre->DespawnOrUnsummon(3000);
                }

                events.ScheduleEvent(EVENT_1, 5000);
                m_CountAdds = 0;
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
               ///DoZoneInCombat(summon, 100.0f);
               if (p_Summon->GetEntry() == NPC_SMALL_TRASH)
                   p_Summon->GetMotionMaster()->MoveRandom(5.0f);
               else if (Player* l_Target = me->FindNearestPlayer(100.0f))
               {
                   if (p_Summon->IsAIEnabled)
                       p_Summon->AI()->AttackStart(l_Target);
               }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() != NPC_IN_THE_SHADOW && p_Summon->GetEntry() != NPC_NIGHTMARE_AMALGAMATION && p_Summon->GetEntry() != NPC_SHADOW_POUNDER)
                    return;

                m_CountAdds++;

                switch (m_CountAdds)
                {
                    case 3:
                        events.ScheduleEvent(EVENT_2, 3000);
                        break;
                    case 6:
                        events.ScheduleEvent(EVENT_3, 3000);
                        break;
                    case 12:
                        events.ScheduleEvent(EVENT_5, 5000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CheckEvade <= p_Diff)
                {
                    if (m_Instance && m_Instance->GetData(DATA_PRE_EVENT_XAVIUS) == IN_PROGRESS)
                    {
                        if (!me->SelectNearestPlayerNotGM(70.0f))
                        {
                            events.Reset();
                            m_Instance->SetData(DATA_PRE_EVENT_XAVIUS, NOT_STARTED);
                            summons.DespawnAll();
                        }
                    }

                    m_CheckEvade = 1000;
                }
                else
                    m_CheckEvade -= p_Diff;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        for (uint8 l_I = 0; l_I < 3; l_I++)
                            me->SummonCreature(NPC_IN_THE_SHADOW, g_EventPos[l_I], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        events.ScheduleEvent(EVENT_6, 5000);
                        break;
                    }
                    case EVENT_2:
                    {
                        for (uint8 l_I = 0; l_I < 3; l_I++)
                            me->SummonCreature(NPC_NIGHTMARE_AMALGAMATION, g_EventPos[l_I], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        events.ScheduleEvent(EVENT_6, 5000);
                        break;
                    }
                    case EVENT_3:
                    {
                        for (uint8 l_I = 0; l_I < 2; l_I++)
                            me->SummonCreature(NPC_NIGHTMARE_AMALGAMATION, g_EventPos[l_I], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        me->SummonCreature(NPC_IN_THE_SHADOW, g_EventPos[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        events.ScheduleEvent(EVENT_4, 7000);
                        events.ScheduleEvent(EVENT_6, 5000);
                        break;
                    }
                    case EVENT_4:
                    {
                        for (uint8 l_I = 3; l_I < 6; l_I++)
                            me->SummonCreature(NPC_SHADOW_POUNDER, g_EventPos[l_I], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        break;
                    }
                    case EVENT_5:
                    {
                        if (m_Instance)
                            m_Instance->SetData(DATA_PRE_EVENT_XAVIUS, DONE);

                        if (Creature* l_Boss = me->FindNearestCreature(NPC_XAVIUS, 100.0f, true))
                        {
                            if (l_Boss->IsAIEnabled)
                                l_Boss->AI()->Reset();
                        }

                        events.Reset();
                        summons.DespawnAll();
                        break;
                    }
                    case EVENT_6:
                    {
                        for (uint8 l_I = 0; l_I < 6; l_I++)
                            me->SummonCreature(NPC_DARK_DEVOURERS, me->GetPositionX() + irand(-25, 25), me->GetPositionY() + irand(-25, 25), me->GetPositionZ());

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_event_conrollerAI(p_Creature);
        }
};

/// Ysera - 105245
class npc_xavius_ysera : public CreatureScript
{
    public:
        npc_xavius_ysera() : CreatureScript("npc_xavius_ysera") { }

        struct npc_xavius_yseraAI : public ScriptedAI
        {
            npc_xavius_yseraAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetCanFly(true);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    std::vector<G3D::Vector3> l_Path =
                    {
                        { -3055.767f, -5123.588f, 196.8564f },
                        { -3060.455f, -5107.713f, 193.9954f },
                        { -3065.142f, -5091.838f, 191.1344f },
                        { -3054.170f, -5060.583f, 185.6389f },
                        { -3058.050f, -5016.332f, 179.7637f },
                        { -3040.203f, -4988.585f, 179.7637f },
                        { -3001.075f, -4967.594f, 179.7637f },
                        { -2962.755f, -4973.608f, 179.7637f },
                        { -2937.092f, -4996.413f, 179.7637f },
                        { -2927.945f, -5022.122f, 179.7637f },
                        { -2933.070f, -5061.300f, 179.7637f },
                        { -2951.382f, -5081.163f, 179.7637f },
                        { -2982.594f, -5093.592f, 179.7637f },
                        { -3014.064f, -5089.389f, 179.7637f },
                        { -3034.717f, -5078.068f, 179.7637f },
                        { -3050.969f, -5058.018f, 179.7637f },
                        { -3058.766f, -5031.172f, 179.7637f },
                        { -3053.604f, -5012.647f, 179.7637f },
                        { -3029.878f, -4978.170f, 179.7637f },
                        { -2992.825f, -4967.333f, 179.7637f },
                        { -2971.592f, -4957.952f, 179.7637f },
                        { -2937.377f, -4934.000f, 191.1044f },
                        { -2903.087f, -4910.788f, 200.5305f },
                        { -2877.590f, -4884.870f, 205.3849f },
                        { -2847.945f, -4849.059f, 213.3566f }
                    };

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothFlyPath(0, l_Path.data(), l_Path.size());
                });

                AddTimedDelayedOperation(5000, [this]() -> void
                {
                    me->CastSpell(me, SPELL_DREAM_CONVERSATION);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE && p_ID == 1)
                    me->DespawnOrUnsummon(100);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xavius_yseraAI(p_Creature);
        }
};

/// Xavius Energize Phase 1 - 226184
class spell_xavius_periodic_energize : public SpellScriptLoader
{
    public:
        spell_xavius_periodic_energize() : SpellScriptLoader("spell_xavius_periodic_energize") { }

        class spell_xavius_periodic_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_periodic_energize_AuraScript);

            int32 m_PowerCount = 0;
            uint8 m_CastCount = 0;
            bool m_HighTick = false;

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat() || !l_Caster->IsAIEnabled)
                    return;

                m_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (m_PowerCount < 100)
                {
                    if (l_Caster->AI()->GetData(DATA_XAVIUS) == PHASE_2)
                    {
                        if (m_HighTick)
                        {
                            l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 3);
                            m_HighTick = false;
                        }
                        else
                        {
                            l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 2);
                            m_HighTick = true;
                        }
                    }
                    else
                        l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 5);
                }
                else if (!l_Caster->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (l_Caster->AI()->GetData(DATA_XAVIUS) == PHASE_1)
                    {
                        if (m_CastCount < 3)
                        {
                            m_CastCount++;
                            l_Caster->CastSpell(l_Caster, SPELL_LURKING_ERUPTION, true);
                        }
                        else
                        {
                            m_CastCount = 0;
                            l_Caster->CastSpell(l_Caster, SPELL_MANIFEST_CORRUPTION, true);
                            l_Caster->AI()->Talk(SAY_MANIFEST_CORRUPTION);
                        }

                        l_Caster->SetPower(POWER_ENERGY, 0);
                    }
                    else if (l_Caster->AI()->GetData(DATA_XAVIUS) == PHASE_2)
                        l_Caster->CastSpell(l_Caster, SPELL_CALL_OF_NIGHTMARES);
                    else if (l_Caster->AI()->GetData(DATA_XAVIUS) == PHASE_3)
                        l_Caster->CastSpell(l_Caster, SPELL_WRITHING_DEEP);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xavius_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_periodic_energize_AuraScript();
        }
};

/// Dream Simulacrum - 206005
class spell_xavius_dream_simulacrum : public SpellScriptLoader
{
    public:
        spell_xavius_dream_simulacrum() : SpellScriptLoader("spell_xavius_dream_simulacrum") { }

        class spell_xavius_dream_simulacrum_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_dream_simulacrum_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
                    {
                        if (l_Instance->GetBossState(DATA_XAVIUS) != IN_PROGRESS)
                            return;
                    }

                    GuidList* l_SummonList = l_Player->GetSummonList(NPC_SLEEPING_VERSION);
                    for (GuidList::const_iterator l_Iter = l_SummonList->begin(); l_Iter != l_SummonList->end(); ++l_Iter)
                    {
                        if (Creature* l_Summon = ObjectAccessor::GetCreature(*l_Player, (*l_Iter)))
                        {
                            l_Player->RemoveAllAurasOnDeath();
                            l_Player->CastSpell(l_Summon, SPELL_AWAKENING_DREAM, true);
                            break;
                        }
                    }
                }
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (!GetTarget())
                    return;

                if (p_DmgInfo.GetAmount() < GetTarget()->GetHealth())
                    p_AbsorbAmount = 0;
                else
                    GetAura()->Remove();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xavius_dream_simulacrum_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_xavius_dream_simulacrum_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_dream_simulacrum_AuraScript();
        }
};

/// Nightmare Torment - 189960
class spell_xavius_nightmare_torment_alt_power : public SpellScriptLoader
{
    public:
        spell_xavius_nightmare_torment_alt_power() : SpellScriptLoader("spell_xavius_nightmare_torment_alt_power") { }

        class spell_xavius_nightmare_torment_alt_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_nightmare_torment_alt_power_AuraScript);

            bool m_Madness = false;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    int32 l_AltPower = l_Player->GetPower(POWER_ALTERNATE_POWER);
                    if (l_AltPower >= 33 && l_AltPower < 66 && !l_Player->HasAura(SPELL_INSANITY_LEVEL_1))
                        l_Player->CastSpell(l_Player, SPELL_INSANITY_LEVEL_1, true);
                    else if (l_AltPower < 33 && l_Player->HasAura(SPELL_INSANITY_LEVEL_1))
                        l_Player->RemoveAurasDueToSpell(SPELL_INSANITY_LEVEL_1);

                    if (l_AltPower >= 66 && l_AltPower < 100 && !l_Player->HasAura(SPELL_INSANITY_LEVEL_2))
                        l_Player->CastSpell(l_Player, SPELL_INSANITY_LEVEL_2, true);
                    else if (l_AltPower < 66 && l_Player->HasAura(SPELL_INSANITY_LEVEL_2))
                        l_Player->RemoveAurasDueToSpell(SPELL_INSANITY_LEVEL_2);

                    if (l_AltPower >= 100 && !m_Madness)
                    {
                        m_Madness = true;
                        l_Player->CastSpell(l_Player, SPELL_DESCENT_INTO_MADNESS, true);
                    }
                }
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = 1;
            }

            void Absorb(AuraEffect* /*p_AurEff**/, DamageInfo& /*p_DmgInfo*/, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xavius_nightmare_torment_alt_power_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_xavius_nightmare_torment_alt_power_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_xavius_nightmare_torment_alt_power_AuraScript::Absorb, EFFECT_2, SPELL_AURA_SCHOOL_HEAL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_nightmare_torment_alt_power_AuraScript();
        }
};

/// Corruption: Crushing Shadows - 208860
class spell_xavius_crushing_shadows : public SpellScriptLoader
{
    public:
        spell_xavius_crushing_shadows() : SpellScriptLoader("spell_xavius_crushing_shadows") { }

        class spell_xavius_crushing_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_crushing_shadows_SpellScript);

            void DealDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                Position const* l_Pos = GetExplTargetDest();
                float l_Distancce = l_Pos->GetExactDist2d(GetHitUnit());
                float l_Radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster());
                if (l_Radius == 0.0f)
                    return;

                uint32 l_Damage = GetHitDamage() * (1 - ((l_Distancce * 2) / l_Radius));

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_xavius_crushing_shadows_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_crushing_shadows_SpellScript();
        }
};

/// Corruption: Unfathomable Reality - 207160
class spell_xavius_unfathomable_reality : public SpellScriptLoader
{
    public:
        spell_xavius_unfathomable_reality() : SpellScriptLoader("spell_xavius_unfathomable_reality") { }

        class spell_xavius_unfathomable_reality_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_unfathomable_reality_AuraScript);

            void OnPereodic(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                Position l_Pos;
                GetCaster()->GetRandomNearPosition(l_Pos, 60.0f);
                GetCaster()->CastSpell(l_Pos, 206878, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xavius_unfathomable_reality_AuraScript::OnPereodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_unfathomable_reality_AuraScript();
        }
};

/// Corruption: Descent into Madness - 208431
class spell_xavius_descent_into_madness : public SpellScriptLoader
{
    public:
        spell_xavius_descent_into_madness() : SpellScriptLoader("spell_xavius_descent_into_madness") { }

        class spell_xavius_descent_into_madness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_descent_into_madness_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    {
                        if (l_Instance->GetBossState(DATA_XAVIUS) == IN_PROGRESS)
                        {
                            if (Creature* l_Xavius = l_Instance->instance->GetCreature(l_Instance->GetData64(NPC_XAVIUS)))
                            {
                                if (!l_Target->HasAura(SPELL_DREAM_SIMULACRUM))
                                    l_Xavius->CastSpell(l_Target, SPELL_CORRUPTION_MADNESS, true);
                                else
                                    l_Target->RemoveAura(SPELL_DREAM_SIMULACRUM);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xavius_descent_into_madness_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_descent_into_madness_AuraScript();
        }
};

/// Darkening Soul - 206651
/// Blackening Soul - 209158
class spell_xavius_darkening_soul : public SpellScriptLoader
{
    public:
        spell_xavius_darkening_soul() : SpellScriptLoader("spell_xavius_darkening_soul") { }

        class spell_xavius_darkening_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_darkening_soul_AuraScript);

            bool m_Remove = false;
            bool m_Dispel = false;

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget() || GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL || m_Remove)
                    return;

                if (GetTarget())
                {
                    m_Remove = true;

                    if (GetId() == SPELL_DARKENING_SOUL)
                        GetTarget()->CastSpell(GetTarget(), SPELL_DARKENING_SOUL_AOE, true);
                    else if (GetId() == SPELL_BLACKENING_SOUL)
                        GetTarget()->CastSpell(GetTarget(), SPELL_BLACKENED, true);
                }
            }

            void HandleDispel(DispelInfo* p_DispelInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !GetUnitOwner() || m_Dispel)
                    return;

                if (Unit* l_Dispeller = p_DispelInfo->GetDispeller())
                {
                    m_Dispel = true;

                    if (GetId() == SPELL_DARKENING_SOUL)
                        l_Dispeller->CastSpell(l_Dispeller, SPELL_DARKENING_SOUL_ENERGY, true);
                    else if (GetId() == SPELL_BLACKENING_SOUL)
                    {
                        l_Caster->CastSpell(*GetUnitOwner(), SPELL_TAINTED_DISCHARGE_TRIG_AT, true);
                        l_Dispeller->CastSpell(l_Dispeller, SPELL_BLACKENED_TAINTING_ENERGY, true);
                    }
                }
            }

            void Register() override
            {
                OnDispel += AuraDispelFn(spell_xavius_darkening_soul_AuraScript::HandleDispel);
                AfterEffectRemove += AuraEffectRemoveFn(spell_xavius_darkening_soul_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_darkening_soul_AuraScript();
        }
};

/// Nightmare Blades - 211802
class spell_xavius_nightmare_blades : public SpellScriptLoader
{
    public:
        spell_xavius_nightmare_blades() : SpellScriptLoader("spell_xavius_nightmare_blades") { }

        class spell_xavius_nightmare_blades_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_nightmare_blades_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (!GetTarget()->HasAura(SPELL_NIGHTMARE_BLADES_MARK))
                    GetCaster()->CastSpell(GetTarget(), SPELL_NIGHTMARE_BLADES_SUM, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xavius_nightmare_blades_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_nightmare_blades_AuraScript();
        }
};

/// Nightmare Blades - 206656
class spell_xavius_nightmare_blades_dmg_filter : public SpellScriptLoader
{
    public:
        spell_xavius_nightmare_blades_dmg_filter() : SpellScriptLoader("spell_xavius_nightmare_blades_dmg_filter") { }

        class spell_xavius_nightmare_blades_dmg_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_nightmare_blades_dmg_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->IsAIEnabled && !p_Targets.empty())
                    {
                        p_Targets.remove_if([=](WorldObject* p_Object) -> bool
                        {
                            if (l_Caster->GetAI()->GetObjectData(p_Object->GetGUID()))
                                return true;

                            return false;
                        });
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xavius_nightmare_blades_dmg_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xavius_nightmare_blades_dmg_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_nightmare_blades_dmg_filter_SpellScript();
        }
};

/// Bonds of Terror - 210451
class spell_xavius_bonds_of_terror : public SpellScriptLoader
{
    public:
        spell_xavius_bonds_of_terror() : SpellScriptLoader("spell_xavius_bonds_of_terror") { }

        class spell_xavius_bonds_of_terror_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xavius_bonds_of_terror_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetCaster() && GetCaster()->HasAura(SPELL_BONDS_OF_TERROR_AURA))
                    GetCaster()->RemoveAurasDueToSpell(SPELL_BONDS_OF_TERROR_AURA);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xavius_bonds_of_terror_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xavius_bonds_of_terror_AuraScript();
        }
};

/// Bonds of Terror (Ally) - 210449
class spell_xavius_bonds_of_terror_ally : public SpellScriptLoader
{
    public:
        spell_xavius_bonds_of_terror_ally() : SpellScriptLoader("spell_xavius_bonds_of_terror_ally") { }

        class spell_xavius_bonds_of_terror_ally_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_bonds_of_terror_ally_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (l_Target == nullptr)
                    return;

                GetCaster()->CastSpell(l_Target, SPELL_BONDS_OF_TERROR_AURA_2, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_xavius_bonds_of_terror_ally_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_bonds_of_terror_ally_SpellScript();
        }
};

/// Writhing Deep - 226194
class spell_xavius_writhing_deep : public SpellScriptLoader
{
    public:
        spell_xavius_writhing_deep() : SpellScriptLoader("spell_xavius_writhing_deep") { }

        class spell_xavius_writhing_deep_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_writhing_deep_SpellScript);

            void HandleScript(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                if (Unit* l_Caster = GetCaster())
                {
                    Position l_Pos;
                    l_Caster->GetNearPosition(l_Pos, frand(15.0f, 30.0f), frand(0.0f, 2.0f * M_PI));
                    uint32 l_SpellID = GetSpellInfo()->Effects[p_EffIndex].TriggerSpell;
                    uint32 l_Delay = GetSpellInfo()->Effects[p_EffIndex].MiscValue;
                    l_Caster->m_Events.AddEvent(new DelayDestCastEvent(*l_Caster, l_Pos, l_SpellID, true), l_Caster->m_Events.CalculateTime(l_Delay));
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_xavius_writhing_deep_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_xavius_writhing_deep_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_xavius_writhing_deep_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectLaunch += SpellEffectFn(spell_xavius_writhing_deep_SpellScript::HandleScript, EFFECT_3, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_writhing_deep_SpellScript();
        }
};

/// Corrupting Nova - 207830
class spell_xavius_corrupting_nova : public SpellScriptLoader
{
    public:
        spell_xavius_corrupting_nova() : SpellScriptLoader("spell_xavius_corrupting_nova") { }

        class spell_xavius_corrupting_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_corrupting_nova_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(EFFECT_1);

                if (Unit* l_Target = GetHitUnit())
                {
                    if (l_Target->HasAura(SPELL_NIGHTMARE_TORMENT_ALT_POWER))
                        l_Target->ModifyPower(POWER_ALTERNATE_POWER, 5);
                }
            }

            void Register() override
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_xavius_corrupting_nova_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_corrupting_nova_SpellScript();
        }
};

/// Font of Dreaming (triggered) - 210743
class spell_xavius_font_of_dreaming : public SpellScriptLoader
{
    public:
        spell_xavius_font_of_dreaming() : SpellScriptLoader("spell_xavius_font_of_dreaming") { }

        class spell_xavius_font_of_dreaming_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xavius_font_of_dreaming_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                /// A player must stand in each pillar and receive Dream Simulacrum, or a Corruption Horror will spawn.
                if (p_Targets.empty())
                    DefileFont(l_Caster);
                else
                {
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == nullptr || !p_Object->IsPlayer())
                            return true;

                        Player* l_Player = p_Object->ToPlayer();
                        if (!l_Player->isAlive() || l_Player->isGameMaster())
                            return true;

                        /// Each player may only receive Dream Simulacrum once per fight
                        if (l_Player->HasAura(SPELL_HYPNOPHOBIA_DEBUFF) ||
                            l_Player->HasAura(SPELL_DREAM_SIMULACRUM))
                            return true;

                        return false;
                    });

                    if (p_Targets.empty())
                        DefileFont(l_Caster);
                }
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (l_Target == nullptr)
                    return;

                l_Target->CastSpell(l_Target, SPELL_THE_DREAMING_SUM_CLONE, true);
            }

            void DefileFont(Unit* p_Caster)
            {
                if (InstanceScript* l_Instance = p_Caster->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(DATA_XAVIUS) != IN_PROGRESS)
                        return;
                }

                WorldLocation const* l_Dest = GetExplTargetDest();
                if (l_Dest == nullptr)
                    return;

                p_Caster->CastSpell(l_Dest, SPELL_DEFILED_FONT, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xavius_font_of_dreaming_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_xavius_font_of_dreaming_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xavius_font_of_dreaming_SpellScript();
        }
};

/// Portal to Xavius
class eventobject_teleport_xavius : public EventObjectScript
{
    public:
        eventobject_teleport_xavius() : EventObjectScript("eventobject_teleport_xavius") { }

        bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/)
        {
            if (InstanceScript* l_Instance = p_Player->GetInstanceScript())
            {
                Position l_Pos = { 11370.38f, -12674.65f, 486.99f, 4.656f };

                if (l_Instance->GetBossState(DATA_CENARIUS) == DONE)
                    l_Pos = { -2990.38f, -5026.19f, 147.706f, 3.94f };

                p_Player->NearTeleportTo(l_Pos, false, TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT);
            }

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_xavius()
{
    /// Boss
    new boss_xavius();

    /// Creatures
    new npc_xavius_sleeping_version();
    new npc_xavius_dread_abomination();
    new npc_xavius_lurking_terror();
    new npc_xavius_corruption_horror();
    new npc_xavius_nightmare_blades();
    new npc_xavius_inconceivable_horror();
    new npc_xavius_nightmare_tentacle();
    new npc_xavius_event_conroller();
    new npc_xavius_ysera();

    /// Spells
    new spell_xavius_periodic_energize();
    new spell_xavius_dream_simulacrum();
    new spell_xavius_nightmare_torment_alt_power();
    new spell_xavius_crushing_shadows();
    new spell_xavius_unfathomable_reality();
    new spell_xavius_descent_into_madness();
    new spell_xavius_darkening_soul();
    new spell_xavius_nightmare_blades();
    new spell_xavius_nightmare_blades_dmg_filter();
    new spell_xavius_bonds_of_terror();
    new spell_xavius_bonds_of_terror_ally();
    new spell_xavius_writhing_deep();
    new spell_xavius_corrupting_nova();
    new spell_xavius_font_of_dreaming();

    /// EventObject
    new eventobject_teleport_xavius();
}
#endif
