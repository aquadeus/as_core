////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"

enum eTalks
{
    Aggro           = 2,
    Bomb            = 3,
    Time            = 4,
    TimeEmote       = 5,
    TimeRem         = 6,
    Evade           = 8,
    Death           = 9
};

enum eSpells
{
    /// Advisor Vandros Spells
    AcceleratingBlast           = 203176,
    ChronoShards                = 203254,
    ForceBomb                   = 202974,
    ForceBombAreatrigger        = 202975,
    ForceBombAreatrigger2       = 203090,
    ForceNovaDOT                = 203139,
    ForceDetonation             = 203087,
    BanishInTimeStun            = 203922,
    BanishInTimeAreatrigger     = 203882,
    BanishInTimeTeleport        = 203883,
    BanishInTimeTimer           = 203914,
    BanishInTimeRemove          = 203941,
    UnstableMana                = 220871,
    UnstableManaMissile         = 220872,
    IntroCosmetic               = 214709,

    /// Trash Mobs Spells
    TimeSplit                   = 203833,
    PulseVisual                 = 203835,
    OozePuddle                  = 193942,
    OozeExplosion               = 193938,

    ChallengersMight            = 206150
};

enum eEvents
{
    EventBlast              = 1,
    EventChronoShards       = 2,
    EventForceBomb          = 3,
    EventBanishInTime       = 4,
    EventUnstableMana       = 5,
    EventBISChroneShard     = 6,
    EventSecondTalk         = 7,
    EventAttackConfirm      = 8
};

enum eAchievements
{
    NoTimeToWaste           = 10776
};

Position const g_TeleportPos[4] =
{
    { 3134.70f, 4892.10f, 617.64f, 2.86f },
    { 3151.23f, 5109.96f, 623.22f, 2.06f },
    { 3154.38f, 4661.42f, 574.20f, 2.95f },
    { 3327.04f, 4523.76f, 570.80f, 3.25f }
};

/// Advisor Vandros - 98208
class boss_advisor_vandros : public CreatureScript
{
    public:
        boss_advisor_vandros() : CreatureScript("boss_advisor_vandros") { }

        struct boss_advisor_vandrosAI : public BossAI
        {
            boss_advisor_vandrosAI(Creature* p_Creature) : BossAI(p_Creature, DATA_VANDROS)
            {
                m_TwoSay = false;
                m_Activated = false;
                m_EncounterStarted = false;
                me->SetVisible(false);
                
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_INTERRUPT_CAST, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SILENCE, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SLEEP, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FREEZE, true);
            }

            std::set<uint64> m_GUIDs;
            std::set<uint32> m_AchievementGUIDs;
            uint16 m_ChronoShardTimer;
            bool m_Activated;
            bool m_SpecialPhaseComplete;
            bool m_SpecialPhase;
            bool m_TPDone;
            bool m_FirstPlayer;
            bool m_TwoSay;
            bool m_EncounterStarted;

            void Reset() override
            {
                if (m_SpecialPhase && !m_SpecialPhaseComplete)
                    return;

                if (m_Activated)
                    return;

                _Reset();

                for (uint64 l_GUID : m_GUIDs)
                {
                    Player* l_Player = sObjectAccessor->FindPlayer(l_GUID);
                    if (!l_Player)
                        continue;

                    if (l_Player->getClass() == CLASS_HUNTER && l_Player->GetPet() && l_Player->GetPet()->isHunterPet())
                        l_Player->GetPet()->DespawnOrUnsummon();

                    l_Player->SetPhaseMaskWithSummons(1, true);
                    m_GUIDs.erase(l_GUID);
                }

                DoCast(eSpells::IntroCosmetic);
                m_FirstPlayer = true;
                m_SpecialPhase = false;
                m_SpecialPhaseComplete = false;
                m_TPDone = false;
                m_EncounterStarted = false;
                m_AchievementGUIDs.clear();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BanishInTimeTimer);
                    instance->DoRemoveForcedMovementsOnPlayers();
                }
            }

            void OnTaunt(Unit* p_Taunter) override
            {
                if (!p_Taunter->IsPlayer())
                    return;
                
                EnterCombat(p_Taunter);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (m_EncounterStarted)
                    return;

                m_EncounterStarted = true;
                Talk(eTalks::Aggro);
                me->RemoveAura(eSpells::IntroCosmetic);
                _EnterCombat();
                DefaultEvents();
            }

            void DefaultEvents()
            {
                events.ScheduleEvent(eEvents::EventAttackConfirm, 1000);
                events.ScheduleEvent(eEvents::EventBlast, 3000);
                events.ScheduleEvent(eEvents::EventChronoShards, 12000);
                events.ScheduleEvent(eEvents::EventForceBomb, 16000);
            }

            void EnterEvadeMode() override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                Talk(eTalks::Evade);
                BossAI::EnterEvadeMode();
            }

            /// Returning true => Prevent Spellsteal
            /// Accelerating Blast should be able to be spellstolen
            bool OnSpellSteal(Unit* p_Caster, uint32 p_SpellID) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return true;

                if (p_SpellID != eSpells::AcceleratingBlast)
                    return true;

                return false;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                if (p_ID == 0)
                    return;

                switch (p_ID)
                {
                    case DATA_ACH_NO_TIME_TO_WASTE:
                    {
                        if (m_AchievementGUIDs.find(p_Value) != m_AchievementGUIDs.end())
                            break;

                        m_AchievementGUIDs.insert(p_Value);
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

                for (uint64 l_GUID : m_GUIDs)
                {
                    Player* l_Player = sObjectAccessor->FindPlayer(l_GUID);
                    if (!l_Player)
                        continue;

                    if (l_Player->getClass() == CLASS_HUNTER && l_Player->GetPet() && l_Player->GetPet()->isHunterPet())
                        l_Player->GetPet()->DespawnOrUnsummon();

                    l_Player->SetPhaseMaskWithSummons(1, true);

                    if (m_AchievementGUIDs.empty())
                    {
                        AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::NoTimeToWaste);
                        if (!l_AchievementEntry)
                            return;

                        l_Player->CompletedAchievement(l_AchievementEntry);
                    }
                }

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BanishInTimeTimer);
                    instance->DoRemoveForcedMovementsOnPlayers();
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::BanishInTimeAreatrigger)
                {
                    me->NearTeleportTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
                    me->GetMotionMaster()->Clear(false);

                    m_ChronoShardTimer = 8000;

                    me->GetMap()->ForEachPlayer([](Player* p_Itr)
                    {
                        if (p_Itr->isAlive())
                        {
                            p_Itr->CastSpell(p_Itr, eSpells::BanishInTimeTeleport, true);
                            p_Itr->DelayedCastSpell(p_Itr, eSpells::BanishInTimeTimer, true, 100);
                        }
                    });

                    Talk(eTalks::TimeEmote);

                    AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        DoAction(1);
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::ForceBomb)
                    DoCast(p_Target, eSpells::ForceBombAreatrigger, true);
            }

            void DamageTaken(Unit* p_Attacker, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(50) && !m_SpecialPhase)
                {
                    m_SpecialPhase = true;
                    events.Reset();
                    events.ScheduleEvent(eEvents::EventBanishInTime, 500);
                }
                if (m_SpecialPhase && m_TPDone && !m_SpecialPhaseComplete && instance && !instance->IsWipe())
                {
                    if (me->GetDistance(p_Attacker) <= 40.0f)
                    {
                        Player* l_Player = p_Attacker->ToPlayer();
                        if (l_Player == nullptr)
                            return;

                        me->RemoveAurasDueToSpell(eSpells::BanishInTimeAreatrigger);

                        me->AddThreat(p_Attacker, 100.0f);
                        DoAction(0);
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                    {
                        if (!m_SpecialPhaseComplete)
                        {
                            m_SpecialPhaseComplete = true;

                            if (instance)
                                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BanishInTimeTimer);

                            me->SetReactState(REACT_AGGRESSIVE);
                            me->InterruptNonMeleeSpells(false);
                            Talk(eTalks::TimeRem);
                            DefaultEvents();
                            events.ScheduleEvent(eEvents::EventUnstableMana, 3000);

                            for (uint64 l_Itr : m_GUIDs)
                            {
                                Player* l_Player = sObjectAccessor->FindPlayer(l_Itr);
                                if (!l_Player)
                                    continue;

                                l_Player->SetPhaseMaskWithSummons(1, true);
                            }
                        }
                        break;
                    }
                    case 1:
                    {
                        if (m_SpecialPhase && m_TPDone && !m_SpecialPhaseComplete && instance && !instance->IsWipe())
                        {
                            m_FirstPlayer = true;
                            uint64 l_GUID = me->GetGUID();
                            for (Map::PlayerList::const_iterator l_Itr = instance->instance->GetPlayers().begin(); l_Itr != instance->instance->GetPlayers().end(); ++l_Itr)
                            {
                                if (Player* l_Player = l_Itr->getSource())
                                {
                                    /// To only trigger that on the position of one player, only one chrono shard spawns.
                                    Position l_Pos;
                                    l_Player->GetRandomNearPosition(l_Pos, 10.0f);
                                    if (Creature* l_Creature = me->SummonCreature(102849, l_Pos, TEMPSUMMON_CORPSE_DESPAWN))
                                    {
                                        if (!l_Creature->IsAIEnabled)
                                            return;

                                        l_Creature->AI()->DoAction(2);
                                    }
                                }
                            }
                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                            {
                                Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                                if (!l_Creature || !l_Creature->IsAIEnabled)
                                    return;

                                l_Creature->AI()->DoAction(1);
                            });
                        }
                        break;
                    }
                    case 2:
                    {
                        m_Activated = true;

                        std::list<Creature*> l_CreatureList;
                        me->GetCreatureListInGrid(l_CreatureList, 150.0f);

                        me->SetVisible(true);

                        if (l_CreatureList.empty())
                            return;

                        for (auto l_Itr : l_CreatureList)
                        {
                            switch (l_Itr->GetEntry())
                            {
                                case eCreatures::NPC_ACIDIC_BILE:
                                case eCreatures::NPC_PLAGUED_RATS:
                                case eCreatures::NPC_FORGOTTEN_SPIRIT:
                                case eCreatures::NPC_UNSTABLE_AMALGATION:
                                {
                                    l_Itr->DespawnOrUnsummon();
                                    break;
                                }
                                default:
                                    break;
                                    
                            }
                        }

                        AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            ZoneTalk(1);
                        });
                        break;
                    }
                }
            }

            void ExecuteEvent(uint32 const p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventAttackConfirm:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    case eEvents::EventBlast:
                    {
                        DoCast(eSpells::AcceleratingBlast);
                        events.ScheduleEvent(eEvents::EventBlast, 6000);
                        break;
                    }
                    case eEvents::EventChronoShards:
                    {
                        for (uint8 l_I = 0; l_I < 3; l_I++)
                            DoCast(eSpells::ChronoShards);

                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            switch (l_I)
                            {
                                case 0:
                                {
                                    AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        Creature* l_Creature = me->FindNearestCreature(102849, 30.0f, true);
                                        if (!l_Creature || !l_Creature->IsAIEnabled)
                                            return;

                                        l_Creature->AI()->DoAction(1);
                                    });
                                    break;
                                }
                                case 1:
                                {
                                    AddTimedDelayedOperation(9 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        Creature* l_Creature = me->FindNearestCreature(102849, 30.0f, true);
                                        if (!l_Creature || !l_Creature->IsAIEnabled)
                                            return;

                                        l_Creature->AI()->DoAction(1);
                                    });
                                    break;
                                }
                                case 2:
                                {
                                    AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        Creature* l_Creature = me->FindNearestCreature(102849, 30.0f, true);
                                        if (!l_Creature || !l_Creature->IsAIEnabled)
                                            return;

                                        l_Creature->AI()->DoAction(1);
                                    });
                                    break;
                                }
                                default:
                                    break;
                            }
                        }

                        events.ScheduleEvent(eEvents::EventChronoShards, 15000);
                        break;
                    }
                    case eEvents::EventForceBomb:
                    {
                        Talk(eTalks::Bomb);
                        DoCast(eSpells::ForceBomb);
                        events.ScheduleEvent(eEvents::EventForceBomb, 30000);
                        break;
                    }
                    case eEvents::EventBanishInTime:
                    {
                        if (instance)
                        {
                            instance->instance->ForEachPlayer([this](Player* p_Itr) -> void
                            {
                                if (p_Itr->getClass() == CLASS_HUNTER && p_Itr->GetPet() && p_Itr->GetPet()->isHunterPet())
                                    p_Itr->GetPet()->DespawnOrUnsummon();

                                m_GUIDs.insert(p_Itr->GetGUID());
                            });
                        }

                        Talk(eTalks::Time);
                        DoStopAttack();

                        if (instance)
                            instance->SetData(DATA_RAND_TELEPORT, urand(0, 3));

                        DoCast(eSpells::BanishInTimeAreatrigger);
                        me->SetReactState(REACT_PASSIVE);
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_TPDone = true;
                            for (uint64 l_Itr : m_GUIDs)
                            {
                                Player* l_Player = sObjectAccessor->FindPlayer(l_Itr);
                                if (!l_Player)
                                    continue;

                                l_Player->SetPhaseMaskWithSummons(2, true);
                            }
                        });
                        break;
                    }
                    case eEvents::EventUnstableMana:
                    {
                        DoCast(eSpells::UnstableMana);
                        events.ScheduleEvent(eEvents::EventUnstableMana, 30000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_SpecialPhase && !m_SpecialPhaseComplete && instance && me->HasAura(eSpells::BanishInTimeAreatrigger))
                {
                    if (instance->IsWipe())
                    {
                        m_SpecialPhase = false;
                        m_SpecialPhaseComplete = false;
                        return;
                    }

                    Player* l_Target = me->FindNearestPlayer(10.0f);

                    if (m_TPDone && l_Target)
                    {
                        me->RemoveAurasDueToSpell(eSpells::BanishInTimeAreatrigger);
                        me->AddThreat(l_Target, 100.0f);
                        DoAction(0);
                    }

                    return;
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);
                UpdateNovaTargets(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventID = events.ExecuteEvent())
                    ExecuteEvent(l_EventID);

                DoMeleeAttackIfReady();
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool p_Removed) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::ForceBombAreatrigger)
                {
                    Position l_Pos = *p_AreaTrigger;
                    DoCast(l_Pos, eSpells::ForceDetonation, true);
                }
            }

            void UpdateNovaTargets(uint32 const p_Diff)
            {
                std::list<AreaTrigger*> l_AreaTriggerList;
                me->GetAreaTriggerList(l_AreaTriggerList, eSpells::ForceBombAreatrigger2);
                if (l_AreaTriggerList.empty())
                    return;

                for (auto l_Nova : l_AreaTriggerList)
                {
                    uint32 l_Time = 12500 - l_Nova->GetDuration(); ///< Gets the 'timer' of the AT (basically max duration - duration)
                    Position l_ATPos = *l_Nova;
                    uint64 l_ATGUID = l_Nova->GetGUID();

                    float l_YardsPerMs = 7.0f / (float)TimeConstants::IN_MILLISECONDS;
                    /// Base min radius is 10 yards, which is increased depending on time passed
                    float l_MinRadius = 8.0f;
                    float l_InnerRange = 6.0f;

                    l_MinRadius += (l_YardsPerMs * l_Time);

                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    for (HostileReference* l_Ref : l_ThreatList)
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, l_Ref->getUnitGuid()))
                        {
                            if (l_Player->GetDistance(l_ATPos) >= (l_MinRadius - l_InnerRange) && l_Player->GetDistance(l_ATPos) <= l_MinRadius && l_Player->GetPositionZ() - l_Nova->GetPositionZ() < 0.5)
                            {
                                if (!l_Player->HasAura(eSpells::ForceNovaDOT))
                                    me->CastSpell(l_Player, eSpells::ForceNovaDOT, true);

                                /// In addition to Force Nova's normal effects, it now also pushes players away as the nova moves outwards.
                                if (!l_Player->HasMovementForce(l_ATGUID))
                                    l_Player->SendApplyMovementForce(l_ATGUID, true, *l_Nova, -5.5f, 1);
                            }
                            else
                            {
                                l_Player->RemoveAura(eSpells::ForceNovaDOT);

                                /// Removing movement force
                                if (l_Player->HasMovementForce(l_ATGUID))
                                    l_Player->SendApplyMovementForce(l_ATGUID, false, Position());
                            }
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_advisor_vandrosAI(p_Creature);
        }
};

/// Chrono Shard - 102849
class npc_vandros_chrono_shard : public CreatureScript
{
    public:
        npc_vandros_chrono_shard() : CreatureScript("npc_vandros_chrono_shard") { }

        struct npc_vandros_chrono_shardAI : public ScriptedAI
        {
            npc_vandros_chrono_shardAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 1:
                    {
                        DoCast(eSpells::TimeSplit);
                        me->ForcedDespawn(0.2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case 2:
                        events.ScheduleEvent(EVENT_1, 8000);
                        break;
                    default:
                        break;
                }
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(eSpells::PulseVisual);
                Position l_Pos;
                me->GetRandomNearPosition(l_Pos, 10.0f);
                me->GetMotionMaster()->MovePoint(1, l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(eSpells::TimeSplit);
                        me->ForcedDespawn(0.2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vandros_chrono_shardAI(p_Creature);
        }
};

/// Last Update 7.1.5
/// Banished in time areatrigger - 203882
class spell_at_vandros_banish_in_time : public AreaTriggerEntityScript
{
    public:
        spell_at_vandros_banish_in_time() : AreaTriggerEntityScript("spell_at_vandros_banish_in_time") { }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Creature* l_Creature = l_Caster->ToCreature();
            if (!l_Creature || !l_Creature->IsAIEnabled)
                return;

            l_Creature->AI()->DoAction(0);

            if (p_AreaTrigger->GetDuration() < 1 * IN_MILLISECONDS)
            {
                Map::PlayerList const& l_Players = l_Creature->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                {
                    Player* l_Player = l_Iter->getSource();
                    if (l_Player == nullptr)
                        return;

                    l_Player->Kill(l_Player);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_vandros_banish_in_time();
        }
};

/// Unstable Ooze - 98426
/// Unstable Amalgamation - 98425
class npc_unstable_amalgamation : public CreatureScript
{
    public:
        npc_unstable_amalgamation() : CreatureScript("npc_unstable_amalgamation") { }

        struct npc_unstable_amalgamationAI : public ScriptedAI
        {
            npc_unstable_amalgamationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint32 m_AddSum;

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Position l_Pos = { 3434.770f, 4806.058f, 590.928f, 6.087f };
                if (me->GetEntry() == NPC_UNSTABLE_AMALGATION)
                    events.ScheduleEvent(EVENT_2, 15000);

                if (me->IsNearPosition(&l_Pos, 15.0f))
                {
                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListInGrid(l_CreatureList, 15.0f);
                    for (Creature* l_Creature : l_CreatureList)
                    {
                        if (l_Creature->GetEntry() == 98733) ///< 98733 = Withered Fiend
                            l_Creature->Kill(l_Creature);
                    }
                }
            }

            bool CheckTrash()
            {
                Creature* l_WitheredManaFiend = GetClosestCreatureWithEntry(me, 98733, 20.0f, true);

                if (l_WitheredManaFiend)
                    return false;

                return true;
            }

            void Reset() override
            {
                events.Reset();
                Position l_Pos = { 3434.770f, 4806.058f, 590.928f, 6.087f };
                if (me->IsNearPosition(&l_Pos, 20.0f) && CheckTrash())
                    me->PlayOneShotAnimKitId(2758);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(eSpells::OozePuddle);
                Position l_Pos;
                float l_X, l_Y, l_Z;
                me->GetPosition(l_X, l_Y, l_Z);
                if (me->GetEntry() == NPC_UNSTABLE_AMALGATION)
                    m_AddSum = NPC_UNSTABLE_OOZE;
                if (me->GetEntry() == NPC_UNSTABLE_OOZE)
                    m_AddSum = NPC_UNSTABLE_OOZELING;
                for (uint8 l_I = 0; l_I < 4; l_I++)
                {
                    if (Unit* l_Summon = me->SummonCreature(m_AddSum, l_X, l_Y, l_Z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                    {
                        float l_Angle = l_I * static_cast<float>(M_PI);
                        me->GetNearPosition(l_Pos, 5.0f, l_Angle);
                        l_Summon->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), 193852, true);
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

                switch (events.ExecuteEvent())
                {
                    case EVENT_2:
                    {
                        DoCast(eSpells::OozeExplosion);
                        events.ScheduleEvent(EVENT_2, 16000);
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
            return new npc_unstable_amalgamationAI(p_Creature);
        }
};

/// Timeless Wrath - 103130
class npc_arcway_timeless_wrath : public CreatureScript
{
    public:
        npc_arcway_timeless_wrath() : CreatureScript("npc_arcway_timeless_wrath") { }

        struct npc_arcway_timeless_wrathAI : public ScriptedAI
        {
            npc_arcway_timeless_wrathAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                TimeLock = 203957
            };

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoZoneInCombat();

                Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO);
                if (!l_Target)
                    return;

                DoCast(l_Target, eSpells::TimeLock, true);
            }

            void JustDied(Unit* p_Killer) override
            {
                if (!p_Killer->IsPlayer())
                    return;

                Player* l_Player = p_Killer->ToPlayer();
                if (!l_Player)
                    return;

                auto l_Instance = me->GetInstanceScript();
                if (!l_Instance)
                    return;

                uint64 l_VandrosGUID = l_Instance->GetData64(DATA_VANDROS);
                if (l_VandrosGUID == 0)
                    return;

                Creature* l_Vandros = ObjectAccessor::FindCreature(l_VandrosGUID);
                if (!l_Vandros)
                    return;

                if (!l_Vandros->IsAIEnabled)
                    return;

                l_Vandros->AI()->SetData(DATA_ACH_NO_TIME_TO_WASTE, l_Player->GetGUIDLow());
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcway_timeless_wrathAI(p_Creature);
        }
};

/// Banish In Time - 203883
class spell_vandros_teleport_plr : public SpellScriptLoader
{
    public:
        spell_vandros_teleport_plr() : SpellScriptLoader("spell_vandros_teleport_plr") { }

        class spell_vandros_teleport_plr_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vandros_teleport_plr_SpellScript);

            uint8 m_Random;

            void HandleScriptEffect(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
                    m_Random = l_Instance->GetData(DATA_RAND_TELEPORT);

                Position l_Dest = g_TeleportPos[m_Random];

                l_Player->TeleportTo(l_Player->GetMapId(), l_Dest, TELE_TO_NOT_LEAVE_COMBAT);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_vandros_teleport_plr_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vandros_teleport_plr_SpellScript();
        }
};

/// Banished In Time - 203914
class spell_vandros_banish_in_time_timer : public SpellScriptLoader
{
    public:
        spell_vandros_banish_in_time_timer() : SpellScriptLoader("spell_vandros_banish_in_time_timer") { }

        class spell_vandros_banish_in_time_timer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_vandros_banish_in_time_timer_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (GetTargetApplication()-> GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    l_Target->Kill(l_Target);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_vandros_banish_in_time_timer_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_vandros_banish_in_time_timer_AuraScript();
        }
};

/// Unstable Mana - 220871
class spell_vandros_unstable_mana : public SpellScriptLoader
{
    public:
        spell_vandros_unstable_mana() : SpellScriptLoader("spell_vandros_unstable_mana") { }

        class spell_vandros_unstable_mana_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_vandros_unstable_mana_AuraScript);

            void OnPereodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::UnstableManaMissile, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_vandros_unstable_mana_AuraScript::OnPereodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_vandros_unstable_mana_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_advisor_vandros()
{
    /// Boss
    new boss_advisor_vandros();

    /// Creatures
    new npc_vandros_chrono_shard();
    new npc_unstable_amalgamation();
    new npc_arcway_timeless_wrath();

    /// Spells
    new spell_vandros_teleport_plr();
    new spell_vandros_banish_in_time_timer();
    new spell_vandros_unstable_mana();
    new spell_at_vandros_banish_in_time();
}
#endif
