////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "legion_worldbosses.hpp"

enum eSpells
{
    /// Flotsam
    SPELL_STALKER                   = 220277,
    SPELL_BREAKSAM                  = 223317,
    SPELL_GETSAM                    = 220340,
    SPELL_GETSAM_DAMAGE             = 220367,
    SPELL_YAKSAM                    = 223373,
    SPELL_JETSAM                    = 220295,
    SPELL_JETSAM_DAMAGE             = 220354,
    SPELL_REGEN                     = 223304,
    SPELL_YAKSAM_MISSILE_01         = 223391,
    SPELL_YAKSAM_MISSILE_02         = 223403,
    SPELL_YAKSAM_MISSILE_03         = 223404,
    SPELL_FLOTSAM_SLEEP             = 204253,
    SPELL_OOZING_BILE_PERIODIC      = 223355,
    SPELL_OOZING_BILE_DUMMY         = 223357,
    SPELL_OOZING_BILE_MISSILE       = 223361,

    /// Levantus
    SPELL_GUST_OF_WIND_AOE          = 217211,
    SPELL_GUST_OF_WIND_SUMM         = 217212,
    SPELL_GUST_OF_WIND_VIS          = 217198,
    SPELL_RENDING_WHIRL             = 217235,
    SPELL_MASSIVE_SPOUT             = 217249,
    SPELL_ELECTRIFY                 = 217344,
    SPELL_ELECTRIFY_DMG             = 217352,
    SPELL_TURBULENT_VORTEX          = 217360,
    SPELL_RAMPAGING_TORRENT         = 217229,

    /// Nithogg
    SPELL_TAIL_LASH                 = 212836,
    SPELL_STORM_BREATH              = 212852,
    SPELL_ELECTRICAL_STORM          = 212867,
    SPELL_ELECTRICAL_STORM_MISSILE  = 212872,
    SPELL_ELECTRICAL_STORM_AT       = 212885,
    SPELL_CRACKLING_STORM_VISUAL_1  = 198900,
    SPELL_CRACKLING_STORM_VISUAL_2  = 198901,
    SPELL_STATIC_CHARGE             = 212887,
    SPELL_STATIC_CHARGE_SEARCHER    = 212889,
    SPELL_CRACKLING_JOLT            = 212837,
    SPELL_CRACKLING_JOLT_MISSILE    = 212838,
    SPELL_STATIC_ORB_COSM           = 212915,
    SPELL_UNSTABLE_EXPLOSION        = 212948,
    SPELL_LIGHTNING_ROD             = 212943,

    /// Humongris
    SPELL_FIRE_BOOM                 = 216427,
    SPELL_FIRE_BOOM_TRIGGERED       = 216428,
    SPELL_EARTHSHAKE_STOMP          = 216430,
    SPELL_ICE_FIST                  = 216432,
    SPELL_YOU_GO_BANG               = 216817,
    SPELL_YOU_GO_BANG_TRIGGERED     = 216822,
    SPELL_YOU_GO_BANG_DEBUFF        = 227177,
    SPELL_MAKE_THE_SNOW             = 216467,
    SPELL_PADAWSEN_TELEPORT         = 40163,

    /// Calamir
    SPELL_PHASE_FIRE                = 217563,
    SPELL_PHASE_FROST               = 217831,
    SPELL_PHASE_ARCANE              = 217834,
    SPELL_BURNING_BOMB              = 217874,
    SPELL_HOWLING_GALE              = 217966,
    SPELL_ARCANOPULSE               = 218005,
    SPELL_WRATHFUL_FLAMES           = 217893,
    SPELL_ICY_COMET                 = 217919,
    SPELL_ICY_COMET_BIG_SELECT      = 217920,
    SPELL_ICY_COMET_MISSILE         = 217921,
    SPELL_ARCANE_DESOLATION         = 217986,

    /// Jim
    SPELL_WITHERED_PRESENCE_AT      = 223632,
    SPELL_MORE_MORE_MORE            = 223715,
    SPELL_MORE_MORE_MORE_TRIGG      = 223723,
    SPELL_NIGHTSHIFTED_BOLTS        = 223623,
    SPELL_NIGHTSHIFTED_BOLTS_AOE    = 223634,
    SPELL_NIGHTSHIFTED_BOLTS_DMG    = 223708,
    SPELL_RESONANCE                 = 223614,
    SPELL_RESONANCE_MISSILE         = 223616,
    SPELL_RESONANCE_DAMAGE          = 223652,
    SPELL_RESONANCE_AURA            = 223653,
    SPELL_NIGHTSTABLE_ENERGY        = 223689,
    SPELL_NIGHTSTABLE_ENERGY_AURA   = 223686,
    SPELL_NIGHTSTABLE_ENERGY_DMG    = 223685,

    /// Shar'thos
    SPELL_NIGHTMARE_BREATH          = 215821,
    SPELL_SHARTHOS_TAIL_LASH        = 215806,
    SPELL_DREAD_FLAME               = 216043,
    SPELL_CRY_OF_THE_TORMENTED      = 216044,
    SPELL_BURNING_EARTH_FILTER      = 215856,
    SPELL_BURNING_EARTH_MISSILE     = 215860,

    /// Drugon the Frostblood
    SPELL_SNOW_CRASH_DUMMY          = 219493,
    SPELL_SNOW_CRASH_AREATRIGGER    = 219520,
    SPELL_ICE_HURL                  = 219803,
    SPELL_AVALANCHE_MISSILE         = 219542,
    SPELL_AVALANCHE_MISSILE_01      = 219687,
    SPELL_AVALANCHE_MISSILE_02      = 219689,
    SPELL_SNOW_PLOW_DUMMY           = 219601,
    SPELL_SNOW_PLOW_FIXATE          = 219602,
    SPELL_SNOW_PLOW_TARGET_CAUGHT   = 219610,

    /// Ana-Mouz
    SPELL_GASEOUS_BREATH_SEARCHER   = 219255,
    SPELL_GASEOUS_BREATH_CAST       = 219254,
    SPELL_FEL_GEYSER_SEARCHER       = 218823,
    SPELL_FEL_GEYSER_AURA           = 197374,
    SPELL_FEL_GEYSER_DUMMY          = 197375,
    SPELL_FEL_GEYSER_MISSILE        = 197376,
    SPELL_FIRE_BOLT                 = 203368,
    SPELL_IMPISH_FLAMES             = 218888,
    SPELL_MOTHERS_EMBRACE_SEARCHER  = 219045,
    SPELL_MOTHERS_EMBRACE_CHARM     = 219068,
    SPELL_MOTHERS_EMBRACE_CLEAR     = 227351,

    /// Na'zak the Fiend
    SPELL_CORRODING_SPRAY           = 219349,
    SPELL_WEB_WRAP_CAST             = 219861,
    SPELL_WEB_WRAP_AURA             = 219865,
    SPELL_FOUNDATIONAL_COLLAPSE     = 219591,
    SPELL_ABSORB_LEYSTONES          = 219813,
    SPELL_LEY_INFUSION              = 219836,
    SPELL_LEY_INFUSION_DAMAGER      = 226002,
    SPELL_LEY_ENERGY_DISCHARGE      = 220027,
    SPELL_SELF_STUN                 = 141480,

    /// Common
    SPELL_WORLD_BOSS_BONUS_ROLL     = 227128
};

///////////////////////////////////////////////////////////////////////////////////
/// Azsuna WorldBosses
///////////////////////////////////////////////////////////////////////////////////

/// Levantus - 108829
class boss_levantus : public CreatureScript
{
    public:
        boss_levantus() : CreatureScript("boss_levantus") { }

        struct boss_levantusAI : public BossAI
        {
            boss_levantusAI(Creature* p_Creature) : BossAI(p_Creature, 0)
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void Reset() override
            {
                SetFlyMode(true);
                me->SetHover(true);

                events.Reset();
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(EVENT_1, urand(7000, 8000));
                events.ScheduleEvent(EVENT_2, urand(25000, 27000));
                events.ScheduleEvent(EVENT_3, urand(50000, 51000));
                events.ScheduleEvent(EVENT_4, 11000);
                events.ScheduleEvent(EVENT_5, 1000);
                events.ScheduleEvent(EVENT_6, 3000);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                Reset();
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == eLegionWBCreatures::NpcGustOfWind)
                    p_Summon->CastSpell(p_Summon, SPELL_GUST_OF_WIND_VIS, true);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_ELECTRIFY)
                {
                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    if (!l_ThreatList.empty())
                    {
                        for (std::list<HostileReference*>::const_iterator l_Iter = l_ThreatList.begin(); l_Iter != l_ThreatList.end(); l_Iter++)
                        {
                            if (Player* l_Player = me->GetPlayer(*me, (*l_Iter)->getUnitGuid()))
                            {
                                if (l_Player && l_Player->GetPositionZ() < 0.0f)
                                    me->CastSpell(l_Player, SPELL_ELECTRIFY_DMG, true);
                            }
                        }
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_GUST_OF_WIND_AOE)
                    DoCast(p_Target, SPELL_GUST_OF_WIND_SUMM);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_RENDING_WHIRL);
                        events.ScheduleEvent(EVENT_1, urand(39000, 41000));
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_MASSIVE_SPOUT);
                        events.ScheduleEvent(EVENT_2, urand(39000, 41000));
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_ELECTRIFY);
                        events.ScheduleEvent(EVENT_3, urand(39000, 41000));
                        break;
                    }
                    case EVENT_4:
                    {
                        DoCast(SPELL_TURBULENT_VORTEX);
                        events.ScheduleEvent(EVENT_4, 10000);
                        break;
                    }
                    case EVENT_5:
                    {
                        if (Unit* l_Target = me->FindNearestPlayer(100.0f))
                        {
                            if (!me->IsWithinMeleeRange(l_Target))
                                DoCast(SPELL_RAMPAGING_TORRENT);
                        }

                        events.ScheduleEvent(EVENT_5, 1000);
                        break;
                    }
                    case EVENT_6:
                    {
                        DoCast(me, SPELL_GUST_OF_WIND_AOE);
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
            return new boss_levantusAI(p_Creature);
        }
};

/// Massive Spout - 217249
class spell_levantus_massive_spout : public SpellScriptLoader
{
    public:
        spell_levantus_massive_spout() : SpellScriptLoader("spell_levantus_massive_spout") { }

        class spell_levantus_massive_spout_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_levantus_massive_spout_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetCaster() == nullptr)
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    l_Caster->GetMotionMaster()->MoveRotate(20000, urand(0, 1) ? ROTATE_DIRECTION_LEFT : ROTATE_DIRECTION_RIGHT);
                    l_Caster->ToCreature()->AttackStop();
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                }
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetCaster() == nullptr)
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    l_Caster->StopMoving();
                    l_Caster->GetMotionMaster()->Clear(false);
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    l_Caster->ToCreature()->AI()->DoZoneInCombat(l_Caster, 150.0f);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_levantus_massive_spout_AuraScript::OnApply, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_levantus_massive_spout_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_levantus_massive_spout_AuraScript();
        }
};

/// Gust of Wind - 217212
class spell_levantus_gust_of_wind : public SpellScriptLoader
{
    public:
        spell_levantus_gust_of_wind() : SpellScriptLoader("spell_levantus_gust_of_wind") { }

        class spell_levantus_gust_of_wind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_levantus_gust_of_wind_SpellScript);

            void ModDestHeight(SpellEffIndex p_EffIndex)
            {
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                if (l_Dest == nullptr)
                    return;

                Position const l_Offset = { 0.0f, 0.0f, -(l_Dest->m_positionZ), 0.0f };

                l_Dest->RelocateOffset(l_Offset);
                GetHitDest()->RelocateOffset(l_Offset);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_levantus_gust_of_wind_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_levantus_gust_of_wind_SpellScript();
        }
};

/// Calamir - 109331
class boss_calamir : public CreatureScript
{
    public:
        boss_calamir() : CreatureScript("boss_calamir") { }

        enum ePhases
        {
            PhaseFire,
            PhaseFrost,
            PhaseArcane,
            PhaseMax
        };

        enum eActions
        {
            ActionApplyHowlingGale,
            ActionRemoveHowlingGale
        };

        enum eEvents
        {
            EventIcyComet = 1,
            EventHowlingGale,
            EventBurningBomb,
            EventWrathfulFlames,
            EventArcanopulse,
            EventArcaneDesolation
        };

        struct boss_calamirAI : public BossAI
        {
            boss_calamirAI(Creature* p_Creature) : BossAI(p_Creature, 0)
            {
                m_TimerPhase    = 100;
                m_NextPhase     = 0;
                m_CurrentPhase  = 0;
            }

            uint32 const m_PhaseSpells[ePhases::PhaseMax]
            {
                SPELL_PHASE_FIRE,
                SPELL_PHASE_FROST,
                SPELL_PHASE_ARCANE
            };

            uint32 m_TimerPhase;
            uint8 m_NextPhase;
            uint8 m_CurrentPhase;

            std::set<uint64> m_AffectedPlayers;

            EventMap m_CosmeticEvents;

            void Reset() override
            {
                m_CosmeticEvents.Reset();

                m_CosmeticEvents.ScheduleEvent(EVENT_1, 100);

                events.Reset();

                me->RemoveAllAreasTrigger();

                summons.DespawnAll();

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == eLegionWBCreatures::NpcWrathfulFlames)
                {
                    p_Summon->SetReactState(ReactStates::REACT_PASSIVE);

                    p_Summon->GetMotionMaster()->Clear();
                    p_Summon->GetMotionMaster()->MoveRandom(30.0f);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionApplyHowlingGale:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 30.0f);

                        if (l_PlayerList.empty())
                            break;

                        l_PlayerList.remove_if([this](Player* p_Player) -> bool
                        {
                            if (p_Player == nullptr || p_Player->isGameMaster())
                                return true;

                            return false;
                        });

                        if (l_PlayerList.empty())
                            break;

                        for (Player* l_Player : l_PlayerList)
                        {
                            m_AffectedPlayers.insert(l_Player->GetGUID());

                            l_Player->SendApplyMovementForce(me->GetGUID(), true, *me, 0.7f, 1);
                        }

                        break;
                    }
                    case eActions::ActionRemoveHowlingGale:
                    {
                        for (uint64 l_Guid : m_AffectedPlayers)
                        {
                            if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                                l_Player->SendApplyMovementForce(me->GetGUID(), false, Position());
                        }

                        m_AffectedPlayers.clear();
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_ICY_COMET_BIG_SELECT)
                    me->CastSpell(*p_Target, SPELL_ICY_COMET_MISSILE, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_CosmeticEvents.Update(p_Diff);

                switch (m_CosmeticEvents.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        m_CurrentPhase = m_NextPhase;

                        DoCast(me, m_PhaseSpells[m_NextPhase], true);

                        if (m_NextPhase >= 2)
                            m_NextPhase = 0;
                        else
                            m_NextPhase++;

                        events.Reset();

                        switch (m_CurrentPhase)
                        {
                            case ePhases::PhaseFire:
                            {
                                /// Once fire buff appears, the Boss should start with Burning Bomb purring bomb on 2 random players
                                events.ScheduleEvent(eEvents::EventBurningBomb, 1);
                                break;
                            }
                            case ePhases::PhaseFrost:
                            {
                                /// Should instantly cast Howling Gale right after buff appeared
                                events.ScheduleEvent(eEvents::EventHowlingGale, 1);
                                break;
                            }
                            case ePhases::PhaseArcane:
                            {
                                /// Should start with Arcane Desolation
                                events.ScheduleEvent(eEvents::EventArcaneDesolation, 1);
                                break;
                            }
                            default:
                                break;
                        }

                        m_CosmeticEvents.ScheduleEvent(EVENT_1, 25000);
                        break;
                    }
                    default:
                        break;
                }

                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (uint32 l_EventID = events.ExecuteEvent())
                {
                    if (m_CurrentPhase < ePhases::PhaseMax && !me->HasAura(m_PhaseSpells[m_CurrentPhase]))
                        return;

                    switch (l_EventID)
                    {
                        case eEvents::EventIcyComet:
                        {
                            DoCast(SPELL_ICY_COMET);

                            /// Should cast Howling Gale again after Icy Comet finished casting
                            events.ScheduleEvent(eEvents::EventHowlingGale, 6000);
                            break;
                        }
                        case eEvents::EventHowlingGale:
                        {
                            m_AffectedPlayers.clear();
                            DoCast(SPELL_HOWLING_GALE);

                            /// Should cast Icy Comet after Howling Gale finished casting
                            events.ScheduleEvent(eEvents::EventIcyComet, 8100);
                            break;
                        }
                        case eEvents::EventBurningBomb:
                        {
                            DoCast(SPELL_BURNING_BOMB);

                            /// After a small delay, casts Wrathful Flames
                            events.ScheduleEvent(eEvents::EventWrathfulFlames, 4000);
                            break;
                        }
                        case eEvents::EventWrathfulFlames:
                        {
                            DoCast(SPELL_WRATHFUL_FLAMES);

                            /// After a small delay, repeat cycle
                            events.ScheduleEvent(eEvents::EventBurningBomb, 5000);
                            break;
                        }
                        case eEvents::EventArcanopulse:
                        {
                            DoCast(SPELL_ARCANOPULSE);

                            /// Repeat cycle after 12s
                            events.ScheduleEvent(eEvents::EventArcaneDesolation, 12000);
                            break;
                        }
                        case eEvents::EventArcaneDesolation:
                        {
                            DoCast(SPELL_ARCANE_DESOLATION);

                            /// Followed by Arcanopulse shortly after
                            events.ScheduleEvent(eEvents::EventArcanopulse, 1000);
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
            return new boss_calamirAI(p_Creature);
        }
};

/// Arcanopulse - 218003
class areatrigger_calamir_arcanopusle : public AreaTriggerEntityScript
{
    public:
        areatrigger_calamir_arcanopusle() : AreaTriggerEntityScript("areatrigger_calamir_arcanopusle"), m_ScaleTime(0) { }

        enum eSpell
        {
            ArcanopuseEffect = 218012
        };

        uint32 m_ScaleTime;
        uint32 m_MaxScaleTime = 60 * TimeConstants::IN_MILLISECONDS;

        float m_MaxRadius     = 4.0f;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                m_ScaleTime += p_Time;

                float l_ActualRadius = (m_ScaleTime * m_MaxRadius / m_MaxScaleTime) + 2.f;

                std::list<Unit*> l_TargetList;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_ActualRadius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                l_Caster->VisitNearbyObject(l_ActualRadius, l_Searcher);

                if (!l_TargetList.empty())
                {
                    l_TargetList.remove_if([&](Unit* p_Unit) -> bool
                    {
                        if (p_Unit == nullptr)
                            return true;

                        if (p_Unit->GetDistance(*p_AreaTrigger) > l_ActualRadius)
                            return true;

                        return false;
                    });
                }

                for (Unit* l_Iter : l_TargetList)
                    l_Caster->CastSpell(l_Iter, eSpell::ArcanopuseEffect, true);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_calamir_arcanopusle();
        }
};

/// Howling Gale - 217966
class spell_calamir_howling_gale : public SpellScriptLoader
{
    public:
        spell_calamir_howling_gale() : SpellScriptLoader("spell_calamir_howling_gale") { }

        class spell_calamir_howling_gale_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_calamir_howling_gale_AuraScript);

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Creature* l_Boss = l_Target->ToCreature())
                {
                    if (l_Boss->IsAIEnabled)
                        l_Boss->AI()->DoAction(0);
                }
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Creature* l_Boss = l_Target->ToCreature())
                {
                    if (l_Boss->IsAIEnabled)
                        l_Boss->AI()->DoAction(1);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_calamir_howling_gale_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_calamir_howling_gale_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_calamir_howling_gale_AuraScript();
        }
};

/// Burning Bomb - 217874
class spell_calamir_burning_bomb_searcher : public SpellScriptLoader
{
    public:
        spell_calamir_burning_bomb_searcher() : SpellScriptLoader("spell_calamir_burning_bomb_searcher") { }

        class spell_calamir_burning_bomb_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_calamir_burning_bomb_searcher_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_calamir_burning_bomb_searcher_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_calamir_burning_bomb_searcher_SpellScript();
        }
};

/// Withered J'im - 112350
/// Withered J'im - 102075
class boss_withered_jim : public CreatureScript
{
    public:
        boss_withered_jim() : CreatureScript("boss_withered_jim") { }

        struct boss_withered_jimAI : public BossAI
        {
            boss_withered_jimAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            void Reset() override
            {
                events.Reset();
                me->RemoveAllAreasTrigger();
                summons.DespawnAll();

                me->SetFullHealth();

                me->CastSpell(me, SPELL_WITHERED_PRESENCE_AT, true);

                if (me->GetEntry() == eLegionWBCreatures::BossWitheredJim)
                {
                    if (Creature* l_Shard = me->SummonCreature(eLegionWBCreatures::NpcUnstableManashard, { -1762.01f, 6360.96f, 57.1644f, 0.0f }))
                    {
                        l_Shard->SetReactState(ReactStates::REACT_PASSIVE);
                        l_Shard->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    }
                }

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (me->GetEntry() != eLegionWBCreatures::BossWitheredJim)
                    return;

                events.ScheduleEvent(EVENT_1, 18000);
                events.ScheduleEvent(EVENT_2, 24000);
                events.ScheduleEvent(EVENT_3, 22000);
                events.ScheduleEvent(EVENT_4, 30000);
            }

            void JustDespawned() override
            {
                summons.DespawnAll();
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
                DoZoneInCombat(p_Summon, 150.0f);

                switch (p_Summon->GetEntry())
                {
                    case eLegionWBCreatures::NpcUnstableResonance:
                    {
                        p_Summon->CastSpell(p_Summon, SPELL_RESONANCE_AURA, true);
                        break;
                    }
                    case eLegionWBCreatures::NpcNightstableEnergy:
                    {
                        p_Summon->CastSpell(p_Summon, SPELL_NIGHTSTABLE_ENERGY_AURA, true);
                        p_Summon->GetMotionMaster()->Clear();
                        p_Summon->GetMotionMaster()->MoveRandom(15.0f);
                        p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                if (me->GetEntry() != eLegionWBCreatures::NpcWitheredJim)
                {
                    me->RemoveAllAreasTrigger();
                    RewardAchievement(p_Killer);
                }

                summons.DespawnAll();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_NIGHTSHIFTED_BOLTS);
                        events.ScheduleEvent(EVENT_1, urand(18000, 30000));
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_RESONANCE);
                        events.ScheduleEvent(EVENT_2, urand(24000, 30000));
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_NIGHTSTABLE_ENERGY);
                        events.ScheduleEvent(EVENT_3, urand(22000, 34000));
                        break;
                    }
                    case EVENT_4:
                    {
                        DoCast(SPELL_MORE_MORE_MORE);
                        events.ScheduleEvent(EVENT_4, 30000);
                        events.ScheduleEvent(EVENT_5, 1000);
                        break;
                    }
                    case EVENT_5:
                    {
                        if (me->HasAura(SPELL_MORE_MORE_MORE))
                            events.ScheduleEvent(EVENT_5, 1000);
                        else
                        {
                            if (Creature* l_Add = me->SummonCreature(eLegionWBCreatures::NpcWitheredJim, *me))
                            {
                                l_Add->CastSpell(l_Add, SPELL_MORE_MORE_MORE_TRIGG, true);
                                l_Add->SetInCombatWithZone();
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:

            void RewardAchievement(Unit* p_Killer)
            {
                Player* l_Player = p_Killer->GetCharmerOrOwnerPlayerOrPlayerItself();
                if (l_Player == nullptr)
                    return;

                // Players kill eLegionWBCreatures::BossWitheredJim but it requires eLegionWBCreatures::NpcWitheredJim

                if (GroupPtr l_Group = l_Player->GetGroup())
                {
                    l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr) -> void
                    {
                        if (Player* l_Member = l_Itr->player)
                            if (l_Member->IsAtGroupRewardDistance(me))
                                l_Member->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_KILL_CREATURE, eLegionWBCreatures::NpcWitheredJim, 1);
                    });
                }
                else
                {
                    if (l_Player->IsAtGroupRewardDistance(me))
                        l_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_KILL_CREATURE, eLegionWBCreatures::NpcWitheredJim, 1);
                }
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_withered_jimAI(p_Creature);
        }
};

/// Unstable Manashard - 112355
class npc_jim_unstable_manashard : public CreatureScript
{
    public:
        npc_jim_unstable_manashard() : CreatureScript("npc_jim_unstable_manashard") { }

        struct npc_jim_unstable_manashardAI : public ScriptedAI
        {
            npc_jim_unstable_manashardAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
            {
                if (p_SpellInfo->Id == SPELL_NIGHTSHIFTED_BOLTS_AOE)
                    me->CastSpell(p_Dest, SPELL_NIGHTSHIFTED_BOLTS_DMG, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_jim_unstable_manashardAI(p_Creature);
        }
};

/// Resonance - 223614
class spell_jim_resonance : public SpellScriptLoader
{
    public:
        spell_jim_resonance() : SpellScriptLoader("spell_jim_resonance") { }

        class spell_jim_resonance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_jim_resonance_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, SPELL_RESONANCE_MISSILE, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_jim_resonance_AuraScript::OnProc, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_jim_resonance_AuraScript();
        }
};

/// Resonance - 223653
class spell_jim_resonance_aura : public SpellScriptLoader
{
    public:
        spell_jim_resonance_aura() : SpellScriptLoader("spell_jim_resonance_aura") { }

        class spell_jim_resonance_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_jim_resonance_aura_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();
                if (l_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, SPELL_RESONANCE_DAMAGE, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_jim_resonance_aura_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_jim_resonance_aura_AuraScript();
        }
};

/// Nightstable Energy - 223686
class spell_jim_nightstable_energy_periodic : public SpellScriptLoader
{
    public:
        spell_jim_nightstable_energy_periodic() : SpellScriptLoader("spell_jim_nightstable_energy_periodic") { }

        class spell_jim_nightstable_energy_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_jim_nightstable_energy_periodic_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, SPELL_NIGHTSTABLE_ENERGY_DMG, false);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_jim_nightstable_energy_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_jim_nightstable_energy_periodic_AuraScript();
        }
};

///////////////////////////////////////////////////////////////////////////////////
/// Val'Sharah WorldBosses
///////////////////////////////////////////////////////////////////////////////////

/// Humongris <The Wizard> - 108879
class boss_humongris : public CreatureScript
{
    public:
        boss_humongris() : CreatureScript("boss_humongris") { }

        enum eEvents
        {
            EventEarthshakeStomp = 1,
            EventIceFist,
            EventYouGoBang,
            EventMakeTheSnow,
            EventFireBoom
        };

        enum eTalks
        {
            TalkFireBoom,
            TalkMakeTheSnow,
            TalkIceFist,
            TalkDeath
        };

        struct boss_humongrisAI : public BossAI
        {
            boss_humongrisAI(Creature* p_Creature) : BossAI(p_Creature, 0)
            {
                m_PadawsenGuid = 0;
            }

            uint64 m_PadawsenGuid;
            uint32 m_LastTalkTimer;

            void Reset() override
            {
                events.Reset();
                me->RemoveAllAreasTrigger();

                m_LastTalkTimer = 0;

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventFireBoom, 16 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventEarthshakeStomp, 13 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventIceFist, 18 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventYouGoBang, 20 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventMakeTheSnow, 26 * TimeConstants::IN_MILLISECONDS);
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Passenger->GetEntry() == eLegionWBCreatures::NpcPadawsen && p_Apply)
                    m_PadawsenGuid = p_Passenger->GetGUID();
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_FIRE_BOOM)
                    me->CastSpell(p_Target, SPELL_FIRE_BOOM_TRIGGERED, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* l_Padawsen = Creature::GetCreature(*me, m_PadawsenGuid))
                {
                    Position l_ExitPos = *me;

                    me->GetRandomNearPosition(l_ExitPos, 15.0f);

                    l_Padawsen->GetMotionMaster()->Clear();
                    l_Padawsen->GetMotionMaster()->MoveJump(l_ExitPos, 30.0f, 10.0f);

                    l_Padawsen->AddDelayedEvent([=]() -> void
                    {
                        l_Padawsen->CastSpell(l_Padawsen, SPELL_PADAWSEN_TELEPORT, false);
                    }, 5 * TimeConstants::IN_MILLISECONDS);

                    l_Padawsen->AddDelayedEvent([=]() -> void
                    {
                        l_Padawsen->DespawnOrUnsummon();
                    }, 6010);
                }

                SayHelper(eTalks::TalkDeath);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFireBoom:
                    {
                        DoCast(SPELL_FIRE_BOOM);
                        SayHelper(eTalks::TalkFireBoom);
                        events.ScheduleEvent(eEvents::EventFireBoom, 27 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventEarthshakeStomp:
                    {
                        DoCast(SPELL_EARTHSHAKE_STOMP);
                        events.ScheduleEvent(eEvents::EventEarthshakeStomp, 34 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventIceFist:
                    {
                        DoCast(SPELL_ICE_FIST);
                        SayHelper(eTalks::TalkIceFist);
                        events.ScheduleEvent(eEvents::EventIceFist, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventYouGoBang:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 40, true))
                            me->CastSpell(l_Target, SPELL_YOU_GO_BANG);
                        events.ScheduleEvent(eEvents::EventYouGoBang, 24 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventMakeTheSnow:
                    {
                        DoCast(SPELL_MAKE_THE_SNOW);
                        SayHelper(eTalks::TalkMakeTheSnow);
                        events.ScheduleEvent(eEvents::EventMakeTheSnow, 34 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void SayHelper(uint32 p_Text)
            {
                if (!m_PadawsenGuid || ((m_LastTalkTimer + 10) > time(nullptr)))
                    return;

                Talk(p_Text);

                if (Creature* l_Padawsen = Creature::GetCreature(*me, m_PadawsenGuid))
                    sCreatureTextMgr->SendChat(l_Padawsen, p_Text, 0, ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                m_LastTalkTimer = time(nullptr);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_humongrisAI(p_Creature);
        }
};

/// You Go Bang! - 216817
class spell_humongris_you_go_bang : public SpellScriptLoader
{
    public:
        spell_humongris_you_go_bang() : SpellScriptLoader("spell_humongris_you_go_bang") { }

        class spell_humongris_you_go_bang_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_humongris_you_go_bang_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();
                if (l_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, SPELL_YOU_GO_BANG_TRIGGERED, true);
                l_Target->CastSpell(l_Target, SPELL_YOU_GO_BANG_DEBUFF, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_humongris_you_go_bang_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_humongris_you_go_bang_AuraScript();
        }
};

/// Shar'thos - 108678
class boss_sharthos : public CreatureScript
{
    public:
        boss_sharthos() : CreatureScript("boss_sharthos") { }

        enum eEvents
        {
            EventNightmareBreath = 1,
            EventTailLash,
            EventDreadFlame,
            EventCryOfTheTormented,
            EventBurningEarth
        };

        struct boss_sharthosAI : public BossAI
        {
            boss_sharthosAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_FirstTailLash;

            void Reset() override
            {
                events.Reset();

                me->RemoveAllAreasTrigger();

                m_FirstTailLash = true;

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void JustReachedHome() override
            {
                me->SetByteFlag(EUnitFields::UNIT_FIELD_ANIM_TIER, UnitBytes1Offsets::UNIT_BYTES_1_OFFSET_STAND_STATE, UnitStandFlags::UNIT_STAND_FLAGS_CREEP | UnitStandFlags::UNIT_STAND_FLAGS_UNK1);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventNightmareBreath, 16 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTailLash, 44 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDreadFlame, 11 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCryOfTheTormented, 60 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBurningEarth, 21 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_BURNING_EARTH_FILTER)
                    me->CastSpell(p_Target, SPELL_BURNING_EARTH_MISSILE, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventNightmareBreath:
                    {
                        me->CastSpell(me, SPELL_NIGHTMARE_BREATH, true);
                        events.ScheduleEvent(eEvents::EventNightmareBreath, 19 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTailLash:
                    {
                        me->CastSpell(me, SPELL_SHARTHOS_TAIL_LASH, false);

                        if (m_FirstTailLash)
                            events.ScheduleEvent(eEvents::EventTailLash, 15 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventTailLash, 50 * TimeConstants::IN_MILLISECONDS);

                        m_FirstTailLash = !m_FirstTailLash;
                        break;
                    }
                    case eEvents::EventDreadFlame:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SPELL_DREAD_FLAME, true);
                        events.ScheduleEvent(eEvents::EventDreadFlame, 16 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCryOfTheTormented:
                    {
                        me->CastSpell(me, SPELL_CRY_OF_THE_TORMENTED, false);
                        events.ScheduleEvent(eEvents::EventCryOfTheTormented, 75 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBurningEarth:
                    {
                        me->CastSpell(me, SPELL_BURNING_EARTH_FILTER, true);
                        events.ScheduleEvent(eEvents::EventBurningEarth, 26 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_sharthosAI(p_Creature);
        }
};

///////////////////////////////////////////////////////////////////////////////////
/// Highmountains WorldBosses
///////////////////////////////////////////////////////////////////////////////////

/// Flotsam - 99929
class boss_flotsam : public CreatureScript
{
    public:
        boss_flotsam() : CreatureScript("boss_flotsam") { }

        enum eTalks
        {
            TalkAggro,
            TalkYaksam,
            TalkGetsam,
            TalkDeath
        };

        struct boss_flotsamAI : public BossAI
        {
            boss_flotsamAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            void Reset() override
            {
                events.Reset();

                me->SetMaxPower(Powers::POWER_MANA, 100);
                me->SetPower(Powers::POWER_MANA, 0);

                summons.DespawnAll();

                me->RemoveAura(SPELL_REGEN);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void JustReachedHome() override
            {
                me->SetByteFlag(EUnitFields::UNIT_FIELD_ANIM_TIER, UnitBytes1Offsets::UNIT_BYTES_1_OFFSET_STAND_STATE, UnitStandFlags::UNIT_STAND_FLAGS_CREEP | UnitStandFlags::UNIT_STAND_FLAGS_UNK1);

                me->CastSpell(me, SPELL_FLOTSAM_SLEEP, false);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->RemoveAura(SPELL_FLOTSAM_SLEEP);

                Talk(eTalks::TalkAggro);

                events.ScheduleEvent(EVENT_1, urand(8000, 9000));
                events.ScheduleEvent(EVENT_2, urand (16000, 19000));
                events.ScheduleEvent(EVENT_3, 45000);
                events.ScheduleEvent(EVENT_4, 1000);

                me->SetPower(Powers::POWER_MANA, 0);

                DoCast(SPELL_REGEN);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(eTalks::TalkDeath);
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                p_Value = 0;
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
                DoZoneInCombat(p_Summon, 150.0f);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_BREAKSAM)
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            me->CastSpell(*l_Target, SPELL_STALKER, true);
                        events.ScheduleEvent(EVENT_1, urand(8000, 9000));
                        break;
                    }
                    case EVENT_2:
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        DoCast(SPELL_BREAKSAM);
                        events.ScheduleEvent(EVENT_2, urand(16000, 19000));
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_YAKSAM);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, SPELL_YAKSAM_MISSILE_01, true);
                        });

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS + 300, [this]() -> void
                        {
                            me->CastSpell(me, SPELL_YAKSAM_MISSILE_02, true);
                        });

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS + 600, [this]() -> void
                        {
                            me->CastSpell(me, SPELL_YAKSAM_MISSILE_03, true);
                        });

                        Talk(eTalks::TalkYaksam);
                        events.ScheduleEvent(EVENT_3, 50000);
                        break;
                    }
                    case EVENT_4:
                    {
                        if (me->GetPower(Powers::POWER_MANA) == 100)
                        {
                            DoCast(SPELL_GETSAM);
                            Talk(eTalks::TalkGetsam);
                        }

                        events.ScheduleEvent(EVENT_4, 1000);
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
            return new boss_flotsamAI(p_Creature);
        }
};

/// Invisible Jetsam Stalker - 110772
class npc_jetsam_stalker : public CreatureScript
{
    public:
        npc_jetsam_stalker() : CreatureScript("npc_jetsam_stalker") { }

        struct npc_jetsam_stalkerAI : public ScriptedAI
        {
            npc_jetsam_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                p_Summoner->CastSpell(me, SPELL_JETSAM);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_GETSAM:
                    {
                        DoCast(SPELL_GETSAM_DAMAGE);
                        me->DespawnOrUnsummon(100);
                        break;
                    }
                    case SPELL_JETSAM:
                    {
                        DoCast(SPELL_JETSAM_DAMAGE);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_jetsam_stalkerAI(p_Creature);
        }
};

/// Regurgitated Marshstomper - 112128
class npc_regurgitated_marshstomper : public CreatureScript
{
    public:
        npc_regurgitated_marshstomper() : CreatureScript("npc_regurgitated_marshstomper") { }

        struct npc_regurgitated_marshstomperAI : public ScriptedAI
        {
            npc_regurgitated_marshstomperAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->CastSpell(me, SPELL_OOZING_BILE_PERIODIC, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_OOZING_BILE_DUMMY)
                    me->CastSpell(me, SPELL_OOZING_BILE_MISSILE, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_regurgitated_marshstomperAI(p_Creature);
        }
};

/// Drugon the Frostblood - 110378
class boss_drugon_the_frostblood : public CreatureScript
{
    public:
        boss_drugon_the_frostblood() : CreatureScript("boss_drugon_the_frostblood") { }

        enum eTalks
        {
            TalkSnowPlowBefore,
            TalkSnowPlowAfter
        };

        enum eEvents
        {
            EventSnowCrash = 1,
            EventIceHurl,
            EventAvalanche,
            EventSnowPlow
        };

        struct boss_drugon_the_frostbloodAI : public BossAI
        {
            boss_drugon_the_frostbloodAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            uint64 m_AvalancheTarget;
            uint64 m_SnowPlowTarget;

            void Reset() override
            {
                events.Reset();

                me->RemoveAllAreasTrigger();

                summons.DespawnAll();

                m_AvalancheTarget = 0;
                m_SnowPlowTarget = 0;

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventSnowCrash, 17 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventIceHurl, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventAvalanche, 35 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSnowPlow, 45 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
            {
                if (p_Dest == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_SNOW_CRASH_DUMMY)
                {
                    me->CastSpell(p_Dest, SPELL_SNOW_CRASH_AREATRIGGER, true);
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->IsAIEnabled)
                    p_Summon->AI()->SetGUID(m_AvalancheTarget);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_SNOW_PLOW_DUMMY)
                {
                    m_SnowPlowTarget = p_Target->GetGUID();
                    me->CastSpell(p_Target, SPELL_SNOW_PLOW_FIXATE, true);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                Talk(eTalks::TalkSnowPlowAfter);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (m_SnowPlowTarget)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, m_SnowPlowTarget))
                    {
                        if (l_Player->GetDistance(me) < 1.0f)
                        {
                            me->InterruptNonMeleeSpells(false, SPELL_SNOW_PLOW_FIXATE);

                            me->SetReactState(ReactStates::REACT_PASSIVE);
                            me->CastSpell(l_Player, SPELL_SNOW_PLOW_TARGET_CAUGHT, false);

                            m_SnowPlowTarget = 0;
                            return;
                        }
                    }
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSnowCrash:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->SetFacingTo(me->GetAngle(l_Target));

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        me->CastSpell(me, SPELL_SNOW_CRASH_DUMMY, false);
                        events.ScheduleEvent(eEvents::EventSnowCrash, 22 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventIceHurl:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            me->CastSpell(l_Target, SPELL_ICE_HURL, false);

                        events.ScheduleEvent(eEvents::EventIceHurl, 7 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAvalanche:
                    {
                        m_AvalancheTarget = 0;

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 50.0f, true))
                        {
                            m_AvalancheTarget = l_Target->GetGUID();

                            me->CastSpell(l_Target, SPELL_AVALANCHE_MISSILE, false);
                        }

                        events.ScheduleEvent(eEvents::EventAvalanche, 38 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSnowPlow:
                    {
                        m_SnowPlowTarget = 0;
                        Talk(eTalks::TalkSnowPlowBefore);
                        me->CastSpell(me, SPELL_SNOW_PLOW_DUMMY, false);
                        events.ScheduleEvent(eEvents::EventSnowPlow, 48 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_drugon_the_frostbloodAI(p_Creature);
        }
};

/// Avalanche Stalker - 110452
class npc_avalanche_stalker : public CreatureScript
{
    public:
        npc_avalanche_stalker() : CreatureScript("npc_avalanche_stalker") { }

        struct npc_avalanche_stalkerAI : public ScriptedAI
        {
            npc_avalanche_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_SummonerGuid(0) { }

            uint64 m_SummonerGuid;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    for (uint8 l_I = 0; l_I < 8; ++l_I)
                        me->CastSpell(me, SPELL_AVALANCHE_MISSILE_01, true, nullptr, nullptr, m_SummonerGuid);
                });
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                AddTimedDelayedOperation(10, [=]() -> void
                {
                    if (Player* l_Player = Player::GetPlayer(*me, p_Guid))
                        me->CastSpell(l_Player, SPELL_AVALANCHE_MISSILE_02, true, nullptr, nullptr, m_SummonerGuid);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_avalanche_stalkerAI(p_Creature);
        }
};

/// Snow Plow - 219610
class spell_drugon_snow_plow : public SpellScriptLoader
{
    public:
        spell_drugon_snow_plow() : SpellScriptLoader("spell_drugon_snow_plow") { }

        class spell_drugon_snow_plow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_drugon_snow_plow_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetCaster() == nullptr || GetTarget() == nullptr)
                    return;

                Creature* l_Drugon = GetCaster()->ToCreature();
                if (l_Drugon == nullptr || !l_Drugon->IsAIEnabled)
                    return;

                l_Drugon->AI()->DoAction(0);

                Position l_ExitPos = l_Drugon->GetPosition();

                l_ExitPos.m_positionX = l_ExitPos.m_positionX + 10.0f * std::cos(l_ExitPos.m_orientation);
                l_ExitPos.m_positionY = l_ExitPos.m_positionY + 10.0f * std::sin(l_ExitPos.m_orientation);
                l_ExitPos.m_positionZ = l_Drugon->GetMap()->GetHeight(l_Drugon->GetPhaseMask(), l_ExitPos.m_positionX, l_ExitPos.m_positionY, l_ExitPos.m_positionZ);

                GetTarget()->_ExitVehicle(&l_ExitPos);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_drugon_snow_plow_AuraScript::AfterRemove, EFFECT_2, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_drugon_snow_plow_AuraScript();
        }
};

///////////////////////////////////////////////////////////////////////////////////
/// Stormheim WorldBosses
///////////////////////////////////////////////////////////////////////////////////

/// Nithogg - 107023
class boss_nithogg : public CreatureScript
{
    public:
        boss_nithogg() : CreatureScript("boss_nithogg") { }

        enum eEvents
        {
            EventStormBreath = 1,
            EventElectricalStorm,
            EventStaticCharge,
            EventTailLash,
            EventCracklingJolt
        };

        struct boss_nithoggAI : public BossAI
        {
            boss_nithoggAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventStormBreath, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventElectricalStorm, 16 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventStaticCharge, 19 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTailLash, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCracklingJolt, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SPELL_TAIL_LASH)
                    me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SPELL_ELECTRICAL_STORM:
                    {
                        DoCast(p_Target, eSpells::SPELL_ELECTRICAL_STORM_MISSILE, true);
                        break;
                    }
                    case eSpells::SPELL_CRACKLING_JOLT:
                    {
                        DoCast(p_Target, eSpells::SPELL_CRACKLING_JOLT_MISSILE, true);
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

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventStormBreath:
                    {
                        DoCast(eSpells::SPELL_STORM_BREATH);
                        events.ScheduleEvent(eEvents::EventStormBreath, 23 * TimeConstants::IN_MILLISECONDS + 700);
                        break;
                    }
                    case eEvents::EventElectricalStorm:
                    {
                        DoCast(eSpells::SPELL_ELECTRICAL_STORM);
                        events.ScheduleEvent(eEvents::EventElectricalStorm, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventStaticCharge:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            me->CastSpell(l_Target, eSpells::SPELL_STATIC_CHARGE, true);

                        events.ScheduleEvent(eEvents::EventStaticCharge, 39 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTailLash:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 10.0f);

                        if (!l_PlayerList.empty())
                        {
                            l_PlayerList.remove_if([this](Player* p_Player) -> bool
                            {
                                if (!me->isInBack(p_Player, (50.0f * M_PI / 180.0f)))
                                    return true;

                                return false;
                            });

                            if (!l_PlayerList.empty())
                            {
                                me->AddUnitState(UnitState::UNIT_STATE_CANNOT_TURN);

                                DoCast(me, eSpells::SPELL_TAIL_LASH);
                            }
                        }

                        events.ScheduleEvent(eEvents::EventTailLash, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCracklingJolt:
                    {
                        DoCast(eSpells::SPELL_CRACKLING_JOLT);

                        events.ScheduleEvent(eEvents::EventCracklingJolt, 11 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_nithoggAI(p_Creature);
        }
};

/// Electrical Storm - 107338
class npc_nithogg_electrical_storm : public CreatureScript
{
    public:
        npc_nithogg_electrical_storm() : CreatureScript("npc_nithogg_electrical_storm") { }

        struct npc_nithogg_electrical_stormAI : public ScriptedAI
        {
            npc_nithogg_electrical_stormAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SPELL_ELECTRICAL_STORM_AT, true);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRandom(15.0f);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nithogg_electrical_stormAI(p_Creature);
        }
};

/// Static Orb - 107353
class npc_nithogg_static_orb : public CreatureScript
{
    public:
        npc_nithogg_static_orb() : CreatureScript("npc_nithogg_static_orb") { }

        struct npc_nithogg_static_orbAI : public ScriptedAI
        {
            npc_nithogg_static_orbAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_FixateTarget = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                uint64 l_Guid = p_Summoner->GetGUID();

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                {
                    Creature* l_Nithogg = Creature::GetCreature(*me, l_Guid);
                    if (!l_Nithogg || !l_Nithogg->IsAIEnabled)
                        return;

                    me->SetInCombatWithZone();

                    DoCast(me, eSpells::SPELL_STATIC_ORB_COSM, true);

                    if (Player* l_Player = l_Nithogg->AI()->SelectPlayerTarget(eTargetTypeMask::TypeMaskAll, { -int32(eSpells::SPELL_LIGHTNING_ROD) }))
                    {
                        m_FixateTarget = l_Player->GetGUID();

                        DoCast(l_Player, eSpells::SPELL_LIGHTNING_ROD, true);
                    }
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_FixateTarget)
                    return;

                if (Player* l_Player = Player::GetPlayer(*me, m_FixateTarget))
                {
                    if (l_Player->isDead())
                        me->DespawnOrUnsummon();
                    else
                    {
                        if (me->GetExactDist(l_Player) <= 1.0f)
                        {
                            m_FixateTarget = 0;

                            DoCast(me, eSpells::SPELL_UNSTABLE_EXPLOSION, true);

                            me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
                        }
                    }
                }
                else
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nithogg_static_orbAI(p_Creature);
        }
};

/// Static Charge - 212887
class spell_nithogg_static_charge : public SpellScriptLoader
{
    public:
        spell_nithogg_static_charge() : SpellScriptLoader("spell_nithogg_static_charge") { }

        class spell_nithogg_static_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nithogg_static_charge_AuraScript);

            void HandleEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Caster = GetCaster()->ToCreature();
                if (l_Caster && l_Caster->GetEntry() == eLegionWBCreatures::BossNithogg)
                {
                    Position l_Pos = GetTarget()->GetPosition();

                    for (uint8 l_I = 0; l_I < p_AurEff->GetAmount(); ++l_I)
                    {
                        if (l_I > 0)
                        {
                            l_Caster->AddDelayedEvent([l_Caster, l_Pos]() -> void
                            {
                                l_Caster->SummonCreature(eLegionWBCreatures::NpcStaticOrb, l_Pos);
                            }, (l_I * 100) + 1);
                        }
                        else
                            l_Caster->SummonCreature(eLegionWBCreatures::NpcStaticOrb, l_Pos);
                    }
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_nithogg_static_charge_AuraScript::HandleEffectRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nithogg_static_charge_AuraScript();
        }
};

/// Electrical Storm - 212885
class spell_nithogg_electrical_storm : public SpellScriptLoader
{
    public:
        spell_nithogg_electrical_storm() : SpellScriptLoader("spell_nithogg_electrical_storm") { }

        class spell_nithogg_electrical_storm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nithogg_electrical_storm_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                TriggerVisual(GetCaster());
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                TriggerVisual(GetCaster());
            }

            void TriggerVisual(Unit* p_Caster)
            {
                if (!p_Caster)
                    return;

                p_Caster->CastSpell(p_Caster, urand(0, 1) ? eSpells::SPELL_CRACKLING_STORM_VISUAL_1 : eSpells::SPELL_CRACKLING_STORM_VISUAL_2, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_nithogg_electrical_storm_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_nithogg_electrical_storm_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nithogg_electrical_storm_AuraScript();
        }
};

enum eSoultakersSpells
{
    SpellSharedHealth       = 212403,
    SpellSharedEnergizer    = 213674,

    /// Captain Hring
    /// Tentacle Bash
    SpellTentacleBash       = 213420,
    /// Cursed Crew
    SpellCursedCrewSearcher = 213522,
    SpellCursedCrewSummon   = 213523,
    SpellCursedCrewVisual   = 213530,
    SpellShatterCrewmenCast = 213532,
    SpellShatterCrewmenAoE  = 213533,
    /// Reaver Jdorn
    /// Marauding Mists
    SpellMaraudingMistsCast = 213665,
    /// Seadog's Scuttle
    SpellSeadogsScuttle     = 213581,
    SpellSeadogsScuttleAura = 213584,
    SpellSeadogsScuttleCast = 213588,
    /// Soultrapper Mevra
    SpellTeleport           = 215681,
    /// Expel Soul
    SpellExpelSoulCast      = 213625,
    SpellExpelSoulDummy     = 213638,
    SpellExpelSoulDamage    = 213648,
    /// Soul Rend
    SpellSoulRendCast       = 213606,
    SpellSoulRendSearcher   = 213605,
    SpellSoulRendMissile    = 213609
};

enum eSoultakersEvents
{
    /// Captain Hring
    EventTentacleBash = 1,
    EventCursedCrew,
    EventShatterCrewmen,
    /// Reaver Jdorn
    EventMaraudingMists,
    EventSeadogsScuttle,
    /// Soultrapper Mevra
    EventExpelSoul,
    EventSoulRend
};

struct boss_the_soultakers : public BossAI
{
    boss_the_soultakers(Creature* p_Creature) : BossAI(p_Creature, 0) { }

    eSoultakersEvents m_PowerEvent;

    void Reset() override
    {
        _Reset();

        events.Reset();

        SetCombatMovement(true);

        me->SetPower(Powers::POWER_ENERGY, 0);

        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
    }

    void EnterCombat(Unit* /*p_Who*/) override
    {
        _EnterCombat();

        events.Reset();

        DoCast(me, eSoultakersSpells::SpellSharedEnergizer, true);

        DoCast(me, eSoultakersSpells::SpellSharedHealth, true);
    }

    void EnterEvadeMode() override
    {
        BossAI::EnterEvadeMode();

        me->SetFullHealth();

        me->RemoveAllAreasTrigger();

        me->InterruptNonMeleeSpells(true);
    }

    void JustReachedHome() override
    {
        Reset();
    }

    void PowerModified(Powers p_Power, int32 p_Value) override
    {
        if (p_Power == Powers::POWER_ENERGY && p_Value >= 100)
            events.ScheduleEvent(m_PowerEvent, 1);
    }

    void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
    {
        if (p_Damage >= me->GetHealth() && me->GetEntry() == eLegionWBCreatures::BossCaptainHring)
        {
            std::list<HostileReference*> const& l_ThreatList = me->getThreatManager().getThreatList();
            if (l_ThreatList.empty())
                return;

            for (HostileReference* l_Iter : l_ThreatList)
            {
                if (Player* l_Player = l_Iter->getTarget()->ToPlayer())
                {
                    l_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_KILL_CREATURE, eLegionWBCreatures::BossReaverJdorn);
                    l_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_KILL_CREATURE, eLegionWBCreatures::BossSoultrapperMevra);
                }
            }
        }
    }

    bool CanBypassResetRange() const override
    {
        return true;
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        UpdateOperations(p_Diff);

        if (!UpdateVictim())
            return;

        if (!CanBeSeenByPlayers() || me->GetDistance(me->GetHomePosition()) >= 85.0f)
        {
            EnterEvadeMode();
            return;
        }

        events.Update(p_Diff);
    }

    void SetPowerEvent(eSoultakersEvents p_Event)
    {
        m_PowerEvent = p_Event;
    }
};

/// Captain Hring - 106981
class boss_captain_hring : public CreatureScript
{
    public:
        boss_captain_hring() : CreatureScript("boss_captain_hring") { }

        struct boss_captain_hringAI : public boss_the_soultakers
        {
            boss_captain_hringAI(Creature* p_Creature) : boss_the_soultakers(p_Creature) { }

            bool m_CrewHandled = false;

            std::array<uint64, 2> m_OtherSoultakers;

            void Reset() override
            {
                boss_the_soultakers::Reset();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    uint8 l_IDx = 0;

                    if (Creature* l_Jdorn = me->SummonCreature(eLegionWBCreatures::BossReaverJdorn, { 4851.49f, 527.1436f, -48.82396f, 1.578411f }))
                        m_OtherSoultakers[l_IDx++] = l_Jdorn->GetGUID();

                    if (Creature* l_Mevra = me->SummonCreature(eLegionWBCreatures::BossSoultrapperMevra, { 4877.94f, 523.0764f, -49.01387f, 1.337622f }))
                        m_OtherSoultakers[l_IDx++] = l_Mevra->GetGUID();
                });

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                boss_the_soultakers::EnterCombat(p_Attacker);

                for (uint64 const& l_Guid : m_OtherSoultakers)
                {
                    if (Creature* l_Other = Creature::GetCreature(*me, l_Guid))
                    {
                        if (l_Other->IsAIEnabled)
                            l_Other->AI()->AttackStart(p_Attacker);
                    }
                }

                me->SetPower(Powers::POWER_ENERGY, 66);

                events.ScheduleEvent(eSoultakersEvents::EventTentacleBash, 4 * TimeConstants::IN_MILLISECONDS);

                SetPowerEvent(eSoultakersEvents::EventCursedCrew);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSoultakersSpells::SpellCursedCrewSearcher)
                    me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSoultakersSpells::SpellCursedCrewSearcher:
                    {
                        DoCast(p_Target, eSoultakersSpells::SpellCursedCrewSummon, true);

                        if (m_CrewHandled)
                            break;

                        m_CrewHandled = true;

                        events.DelayEvent(eSoultakersEvents::EventTentacleBash, 3 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eSoultakersEvents::EventShatterCrewmen, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eSoultakersSpells::SpellShatterCrewmenCast:
                    {
                        p_Target->CastSpell(p_Target, eSoultakersSpells::SpellShatterCrewmenAoE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_the_soultakers::UpdateAI(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eSoultakersEvents::EventTentacleBash:
                    {
                        DoCast(me, eSoultakersSpells::SpellTentacleBash);
                        events.ScheduleEvent(eSoultakersEvents::EventTentacleBash, 16 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eSoultakersEvents::EventCursedCrew:
                    {
                        m_CrewHandled = false;

                        DoCast(me, eSoultakersSpells::SpellCursedCrewSearcher);
                        break;
                    }
                    case eSoultakersEvents::EventShatterCrewmen:
                    {
                        DoCast(me, eSoultakersSpells::SpellShatterCrewmenCast);
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
            return new boss_captain_hringAI(p_Creature);
        }
};

/// Soultrapper Mevra - 106984
class boss_soultrapper_mevra : public CreatureScript
{
    public:
        boss_soultrapper_mevra() : CreatureScript("boss_soultrapper_mevra") { }

        struct boss_soultrapper_mevraAI : public boss_the_soultakers
        {
            boss_soultrapper_mevraAI(Creature* p_Creature) : boss_the_soultakers(p_Creature) { }

            int8 m_ExpelSoulIDx = 0;

            std::vector<Position> m_RandomTeleportPos =
            {
                { 4877.94f, 523.076f, -48.903f, 1.3376f },
                { 4821.58f, 484.583f, -53.006f, 0.5803f },
                { 4863.44f, 445.973f, -53.341f, 1.3618f },
                { 4912.75f, 463.944f, -53.006f, 2.1472f }
            };

            void EnterCombat(Unit* p_Attacker) override
            {
                boss_the_soultakers::EnterCombat(p_Attacker);

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (l_Owner->IsAIEnabled)
                        l_Owner->GetAI()->AttackStart(p_Attacker);
                }

                SetCombatMovement(false);

                me->SetPower(Powers::POWER_ENERGY, 33);

                events.ScheduleEvent(eSoultakersEvents::EventExpelSoul, 1);

                SetPowerEvent(eSoultakersEvents::EventSoulRend);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSoultakersSpells::SpellSoulRendCast)
                    me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSoultakersSpells::SpellExpelSoulDummy:
                    {
                        if ((++m_ExpelSoulIDx) >= 4)
                        {
                            m_ExpelSoulIDx = 0;

                            DoCast(p_Target, eSoultakersSpells::SpellExpelSoulDamage, true);
                        }

                        break;
                    }
                    case eSoultakersSpells::SpellSoulRendSearcher:
                    {
                        DoCast(p_Target, eSoultakersSpells::SpellSoulRendMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_the_soultakers::UpdateAI(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eSoultakersEvents::EventExpelSoul:
                    {
                        m_ExpelSoulIDx = -1;

                        DoCast(m_RandomTeleportPos[urand(0, m_RandomTeleportPos.size() - 1)], eSoultakersSpells::SpellTeleport, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                                DoCast(l_Player, eSoultakersSpells::SpellExpelSoulCast);
                        });

                        events.ScheduleEvent(eSoultakersEvents::EventExpelSoul, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eSoultakersEvents::EventSoulRend:
                    {
                        DoCast(me, eSoultakersSpells::SpellSoulRendCast);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_soultrapper_mevraAI(p_Creature);
        }
};

/// Reaver Jdorn - 106982
class boss_reaver_jdorn : public CreatureScript
{
    public:
        boss_reaver_jdorn() : CreatureScript("boss_reaver_jdorn") { }

        struct boss_reaver_jdornAI : public boss_the_soultakers
        {
            boss_reaver_jdornAI(Creature* p_Creature) : boss_the_soultakers(p_Creature) { }

            bool m_SeadogsScuttle       = false;
            uint8 m_SeadogsScuttleIDx   = 0;

            void EnterCombat(Unit* p_Attacker) override
            {
                boss_the_soultakers::EnterCombat(p_Attacker);

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (l_Owner->IsAIEnabled)
                        l_Owner->GetAI()->AttackStart(p_Attacker);
                }

                events.ScheduleEvent(eSoultakersEvents::EventMaraudingMists, 3 * TimeConstants::IN_MILLISECONDS);

                SetPowerEvent(eSoultakersEvents::EventSeadogsScuttle);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSoultakersSpells::SpellSeadogsScuttle:
                    {
                        ++m_SeadogsScuttleIDx;

                        DoCast(p_Target, eSoultakersSpells::SpellSeadogsScuttleAura, true);
                        DoCast(p_Target, eSoultakersSpells::SpellSeadogsScuttleCast);
                        break;
                    }
                    case eSoultakersSpells::SpellSeadogsScuttleCast:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveCharge(p_Target, 25.0f);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE && p_ID != EventId::EVENT_CHARGE)
                    return;

                me->RemoveAura(eSoultakersSpells::SpellSeadogsScuttleCast);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    if (m_SeadogsScuttleIDx >= 3)
                    {
                        m_SeadogsScuttle = false;

                        if (Unit* l_Victim = me->getVictim())
                        {
                            AttackStart(l_Victim);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Victim);
                        }
                    }
                    else
                        DoCast(me, eSoultakersSpells::SpellSeadogsScuttle);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_the_soultakers::UpdateAI(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_SeadogsScuttle)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eSoultakersEvents::EventMaraudingMists:
                    {
                        DoCast(me, eSoultakersSpells::SpellMaraudingMistsCast);
                        events.ScheduleEvent(eSoultakersEvents::EventMaraudingMists, 11 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eSoultakersEvents::EventSeadogsScuttle:
                    {
                        /// Just for safety
                        AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_SeadogsScuttle = false;

                            if (Unit* l_Victim = me->getVictim())
                            {
                                AttackStart(l_Victim);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Victim);
                            }
                        });

                        me->SetPower(Powers::POWER_ENERGY, 0);

                        m_SeadogsScuttle    = true;
                        m_SeadogsScuttleIDx = 0;

                        DoCast(me, eSoultakersSpells::SpellSeadogsScuttle);
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
            return new boss_reaver_jdornAI(p_Creature);
        }
};

/// Cursed Crewman - 107569
class npc_soultakers_cursed_crewman : public CreatureScript
{
    public:
        npc_soultakers_cursed_crewman() : CreatureScript("npc_soultakers_cursed_crewman") { }

        struct npc_soultakers_cursed_crewmanAI : public ScriptedAI
        {
            npc_soultakers_cursed_crewmanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(me, eSoultakersSpells::SpellCursedCrewVisual, true);

                me->SetInCombatWithZone();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSoultakersSpells::SpellShatterCrewmenAoE)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->Kill(me);
                    });
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode l_RemoveMode) override
            {
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSoultakersSpells::SpellShatterCrewmenAoE)
                    me->DespawnOrUnsummon(1);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                ScriptedAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_soultakers_cursed_crewmanAI(p_Creature);
        }
};

/// Soul Rend - 213606
class spell_soultakers_soul_rend : public SpellScriptLoader
{
    public:
        spell_soultakers_soul_rend() : SpellScriptLoader("spell_soultakers_soul_rend") { }

        class spell_soultakers_soul_rend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_soultakers_soul_rend_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSoultakersSpells::SpellSoulRendSearcher, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_soultakers_soul_rend_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_soultakers_soul_rend_AuraScript();
        }
};

///////////////////////////////////////////////////////////////////////////////////
/// Suramar WorldBosses
///////////////////////////////////////////////////////////////////////////////////

/// Ana-Mouz - 109943
class boss_ana_mouz : public CreatureScript
{
    public:
        boss_ana_mouz() : CreatureScript("boss_ana_mouz") { }

        enum eEvents
        {
            EventGaseousBreath = 1,
            EventFelGeyser,
            EventImpishFlames,
            EventMothersEmbrace
        };

        struct boss_ana_mouzAI : public BossAI
        {
            boss_ana_mouzAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            std::array<Position const, 3> m_FelGeyserPos =
            {
                {
                    { 1012.760f, 4927.861f, 33.31094f, 1.2408640f },
                    { 991.3976f, 4928.713f, 33.47534f, 6.1316240f },
                    { 1002.755f, 4943.825f, 33.42272f, 0.2113495f }
                }
            };

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);

                SetCombatMovement(false);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventGaseousBreath, 6 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventFelGeyser, 10 * TimeConstants::IN_MILLISECONDS + 200);
                events.ScheduleEvent(eEvents::EventImpishFlames, 14 * TimeConstants::IN_MILLISECONDS + 600);
                events.ScheduleEvent(eEvents::EventMothersEmbrace, 20 * TimeConstants::IN_MILLISECONDS + 800);

                for (Position const& l_Pos : m_FelGeyserPos)
                    me->SummonCreature(eLegionWBCreatures::NpcFelGeyser, l_Pos);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                DoCast(me, eSpells::SPELL_MOTHERS_EMBRACE_CLEAR, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SPELL_GASEOUS_BREATH_SEARCHER:
                    {
                        DoCast(p_Target, eSpells::SPELL_GASEOUS_BREATH_CAST);
                        break;
                    }
                    case eSpells::SPELL_FEL_GEYSER_DUMMY:
                    {
                        DoCast(p_Target, eSpells::SPELL_FEL_GEYSER_MISSILE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SPELL_FEL_GEYSER_SEARCHER:
                    {
                        DoCast(me, eSpells::SPELL_FEL_GEYSER_AURA, true);
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

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventGaseousBreath:
                    {
                        DoCast(me, eSpells::SPELL_GASEOUS_BREATH_SEARCHER, true);

                        events.ScheduleEvent(eEvents::EventGaseousBreath, 30 * TimeConstants::IN_MILLISECONDS + 200);
                        break;
                    }
                    case eEvents::EventFelGeyser:
                    {
                        DoCast(me, eSpells::SPELL_FEL_GEYSER_SEARCHER);

                        events.ScheduleEvent(eEvents::EventFelGeyser, 22 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventImpishFlames:
                    {
                        DoCast(me, eSpells::SPELL_IMPISH_FLAMES);

                        events.ScheduleEvent(eEvents::EventImpishFlames, 24 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventMothersEmbrace:
                    {
                        DoCast(me, eSpells::SPELL_MOTHERS_EMBRACE_SEARCHER);

                        events.ScheduleEvent(eEvents::EventMothersEmbrace, 62 * TimeConstants::IN_MILLISECONDS + 400);
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
            return new boss_ana_mouzAI(p_Creature);
        }
};

/// Lesser Minion - 109948
class npc_ana_mouz_lesser_minion : public CreatureScript
{
    public:
        npc_ana_mouz_lesser_minion() : CreatureScript("npc_ana_mouz_lesser_minion") { }

        enum eEvent
        {
            EventFireBolt = 1
        };

        struct npc_ana_mouz_lesser_minionAI : public ScriptedAI
        {
            npc_ana_mouz_lesser_minionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (Unit* l_Victim = p_Summoner->getVictim())
                    AttackStart(l_Victim);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvent::EventFireBolt, 2 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventFireBolt:
                    {
                        DoCastVictim(eSpells::SPELL_FIRE_BOLT);

                        events.ScheduleEvent(eEvent::EventFireBolt, 4 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_ana_mouz_lesser_minionAI(p_Creature);
        }
};

/// Mother's Embrace - 219045
class spell_ana_mouz_mothers_embrace : public SpellScriptLoader
{
    public:
        spell_ana_mouz_mothers_embrace() : SpellScriptLoader("spell_ana_mouz_mothers_embrace") { }

        class spell_ana_mouz_mothers_embrace_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ana_mouz_mothers_embrace_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SPELL_MOTHERS_EMBRACE_CHARM, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_ana_mouz_mothers_embrace_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ana_mouz_mothers_embrace_AuraScript();
        }
};

/// Na'zak the Fiend - 110321
class boss_nazak_the_fiend : public CreatureScript
{
    public:
        boss_nazak_the_fiend() : CreatureScript("boss_nazak_the_fiend") { }

        enum eEvents
        {
            EventCorrodingSpray = 1,
            EventWebWrap,
            EventFoundationalCollapse
        };

        struct boss_nazak_the_fiendAI : public BossAI
        {
            boss_nazak_the_fiendAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_Leystones = false;

            void Reset() override
            {
                events.Reset();

                me->SetBonusLootSpell(SPELL_WORLD_BOSS_BONUS_ROLL);

                m_Leystones = false;
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventCorrodingSpray, 5 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventWebWrap, 9 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFoundationalCollapse, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SPELL_FOUNDATIONAL_COLLAPSE:
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, eSpells::SPELL_ABSORB_LEYSTONES);
                        });

                        break;
                    }
                    case eSpells::SPELL_ABSORB_LEYSTONES:
                    {
                        m_Leystones = false;

                        std::list<Creature*> l_Leystones;

                        me->GetCreatureListWithEntryInGrid(l_Leystones, eLegionWBCreatures::NpcFelInfusedRubble, 80.0f);

                        for (Creature* l_Iter : l_Leystones)
                        {
                            l_Iter->DespawnOrUnsummon();

                            DoCast(me, eSpells::SPELL_LEY_INFUSION, true);
                        }

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

                if (!CanBeSeenByPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_Leystones)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCorrodingSpray:
                    {
                        DoCastVictim(eSpells::SPELL_CORRODING_SPRAY);

                        events.ScheduleEvent(eEvents::EventCorrodingSpray, 23 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventWebWrap:
                    {
                        DoCast(me, eSpells::SPELL_WEB_WRAP_CAST);

                        events.ScheduleEvent(eEvents::EventWebWrap, 34 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFoundationalCollapse:
                    {
                        DoCast(me, eSpells::SPELL_FOUNDATIONAL_COLLAPSE);

                        m_Leystones = true;

                        events.ScheduleEvent(eEvents::EventFoundationalCollapse, 57 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_nazak_the_fiendAI(p_Creature);
        }
};

/// Web Wrap - 110578
class npc_nazak_web_wrap : public CreatureScript
{
    public:
        npc_nazak_web_wrap() : CreatureScript("npc_nazak_web_wrap") { }

        struct npc_nazak_web_wrapAI : public Scripted_NoMovementAI
        {
            npc_nazak_web_wrapAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint64 m_TargetGuid = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                m_TargetGuid = p_Summoner->GetGUID();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_TargetGuid)
                    return;

                if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                    l_Target->RemoveAura(eSpells::SPELL_WEB_WRAP_AURA);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nazak_web_wrapAI(p_Creature);
        }
};

/// Ley-Infused Rubble - 110417
class npc_nazak_ley_infused_rubble : public CreatureScript
{
    public:
        npc_nazak_ley_infused_rubble() : CreatureScript("npc_nazak_ley_infused_rubble") { }

        struct npc_nazak_ley_infused_rubbleAI : public Scripted_NoMovementAI
        {
            npc_nazak_ley_infused_rubbleAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    DoCast(me, eSpells::SPELL_SELF_STUN, true);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nazak_ley_infused_rubbleAI(p_Creature);
        }
};

/// Ley Infusion - 219836
class spell_nazak_ley_infusion : public SpellScriptLoader
{
    public:
        spell_nazak_ley_infusion() : SpellScriptLoader("spell_nazak_ley_infusion") { }

        class spell_nazak_ley_infusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nazak_ley_infusion_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpells::SPELL_LEY_ENERGY_DISCHARGE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_nazak_ley_infusion_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nazak_ley_infusion_AuraScript();
        }
};

/// Ley-Energy Discharge - 220027
class spell_nazak_ley_energy_discharge : public SpellScriptLoader
{
    public:
        spell_nazak_ley_energy_discharge() : SpellScriptLoader("spell_nazak_ley_energy_discharge") { }

        class spell_nazak_ley_energy_discharge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nazak_ley_energy_discharge_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SPELL_LEY_INFUSION_DAMAGER);
                if (!l_SpellInfo)
                    return;

                int32 l_Damage = l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].CalcValue(l_Caster);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_nazak_ley_energy_discharge_SpellScript::HandleDamage, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nazak_ley_energy_discharge_SpellScript();
        }
};

/// Siphoning - 219647
class spell_nazak_siphoning : public SpellScriptLoader
{
    public:
        spell_nazak_siphoning() : SpellScriptLoader("spell_nazak_siphoning") { }

        class spell_nazak_siphoning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nazak_siphoning_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Creature* l_Leystone = l_Caster->FindNearestCreature(eLegionWBCreatures::NpcFelInfusedRubble, 10.0f))
                        l_Leystone->DespawnOrUnsummon();
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_nazak_siphoning_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nazak_siphoning_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_legion_worldbosses()
{
    ///////////////////////////////////////////////////////////////////////////////////
    /// Azsuna WorldBosses
    ///////////////////////////////////////////////////////////////////////////////////

    new boss_levantus();

    new spell_levantus_massive_spout();
    new spell_levantus_gust_of_wind();

    new boss_calamir();

    new spell_calamir_howling_gale();
    new spell_calamir_burning_bomb_searcher();

    new areatrigger_calamir_arcanopusle();

    new boss_withered_jim();

    new npc_jim_unstable_manashard();

    new spell_jim_resonance();
    new spell_jim_resonance_aura();
    new spell_jim_nightstable_energy_periodic();

    ///////////////////////////////////////////////////////////////////////////////////
    /// Val'Sharah WorldBosses
    ///////////////////////////////////////////////////////////////////////////////////

    new boss_humongris();

    new spell_humongris_you_go_bang();

    new boss_sharthos();

    ///////////////////////////////////////////////////////////////////////////////////
    /// Highmountains WorldBosses
    ///////////////////////////////////////////////////////////////////////////////////

    new boss_flotsam();

    new npc_jetsam_stalker();
    new npc_regurgitated_marshstomper();

    new boss_drugon_the_frostblood();

    new npc_avalanche_stalker();

    new spell_drugon_snow_plow();

    ///////////////////////////////////////////////////////////////////////////////////
    /// Stormheim WorldBosses
    ///////////////////////////////////////////////////////////////////////////////////

    new boss_nithogg();

    new npc_nithogg_electrical_storm();
    new npc_nithogg_static_orb();

    new spell_nithogg_static_charge();
    new spell_nithogg_electrical_storm();

    new boss_captain_hring();
    new boss_soultrapper_mevra();
    new boss_reaver_jdorn();

    new npc_soultakers_cursed_crewman();

    new spell_soultakers_soul_rend();

    ///////////////////////////////////////////////////////////////////////////////////
    /// Suramar WorldBosses
    ///////////////////////////////////////////////////////////////////////////////////

    new boss_ana_mouz();

    new npc_ana_mouz_lesser_minion();

    new spell_ana_mouz_mothers_embrace();

    new boss_nazak_the_fiend();

    new npc_nazak_web_wrap();
    new npc_nazak_ley_infused_rubble();

    new spell_nazak_ley_infusion();
    new spell_nazak_ley_energy_discharge();
    new spell_nazak_siphoning();
}
#endif
