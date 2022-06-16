////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"
#include "HelperDefines.h"

enum eScriptData
{
    BossEnteringPod,
    BossLeavingPod,
    OpenPod,
    ClosePod,
    KickPlayer,
    DisablePod,
    EnablePod,
    AchievementCounter,

    BossCount = 3
};

thread_local bool tl_InReset = false;
thread_local bool tl_Dying = false;

static std::array<uint32, eScriptData::BossCount> const k_Bosses =
{{
    eCreatures::BossAdmiralSvirax,
    eCreatures::BossChiefEngineerIshkar,
    eCreatures::BossGeneralErodus
}};

Position const k_ExitRoom = { -2858.63f, 10813.17f, 137.5 };

static std::vector<Position> k_FusilladeTargets =
{
    { -2787.939f, 10830.80f, 139.623f },
    { -2819.560f, 10822.47f, 139.623f },
    { -2808.046f, 10842.41f, 139.623f },
    { -2796.533f, 10862.35f, 139.623f },
    { -2839.667f, 10834.07f, 139.623f },
    { -2828.154f, 10854.02f, 139.623f },
    { -2816.640f, 10873.96f, 139.623f },
    { -2805.126f, 10893.90f, 139.623f },
    { -2859.774f, 10845.68f, 139.623f },
    { -2848.261f, 10865.63f, 139.623f },
    { -2836.747f, 10885.57f, 139.623f },
    { -2879.882f, 10857.29f, 139.623f },
    { -2868.368f, 10877.23f, 139.623f }
};

static Position k_LegionCruiserSpawn = { -2826.16f, 10859.24f, 247.3957f, 0.40f };

static std::vector<uint32> k_EntriesToDespawn =
{
    eCreatures::NpcFelshieldEmitter,
    eCreatures::NpcScreamingShrike,
    eCreatures::NpcFelbladeShocktrooper,
    eCreatures::NpcFanaticalPyromancer,
    eCreatures::NpcEntropicMine
};

static std::vector<Position> k_ShrikeSpawnPositions =
{
    { -2838.40f, 10882.73f, 150.25f },
    { -2799.78f, 10855.00f, 150.25f }
};

enum eTalks
{
    TalkAggro,
    TalkPassiveMechanic,
    TalkKilledPlayer
};

struct boss_antoran_high_commandAI : BossAI
{
    boss_antoran_high_commandAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataAntoranHighCommand)
    {
        BindSelfToHighCommand();

        m_ActiveBoss = Occurence(0, 0, 2);
    }

    /// Herited Functions
    public:
        enum eSpells
        {
            Invisible               = 245212,
            AssumeCommand           = 245227,
            HologramSpawn           = 253589,
            PsychicAssault          = 244172,
            PsychicScaring          = 244388,

            ExploitWeakness         = 244892,
            ChaosPulse              = 257973,

            SummonReinforcements    = 244923,
            SummonEntropicMines     = 245159,
            EntropicMinesCount      = 245304,
            ZeroingIn               = 244696,

            ShockGrenade            = 244722,
            ShockGrenadeMissile     = 244729
        };

        enum eEvents
        {
            EventSwitchBoss = 1,
            EventExploitWeakness,
            EventShockGrenade,
            EventSummonScreamingShrike,
            EventChaosPulse,
            EventEnergyRegen,

            EventSummonReinforcements,
            EventSummonEntropicMines,
            EventFusillade,
        };

        void KilledUnit(Unit* p_Killed) override
        {
            if (p_Killed->IsPlayer() && IsActiveBoss())
                Talk(eTalks::TalkKilledPlayer);
        }

        void EnterCombat(Unit* p_Attacker) override
        {
            if (me->GetEntry() == eCreatures::BossAdmiralSvirax)
            {
                for (uint32 l_Entry : k_Bosses)
                {
                    if (l_Entry == eCreatures::BossAdmiralSvirax)
                        continue;

                    Creature* l_Boss = me->FindNearestCreature(l_Entry, 100.0f);
                    if (!l_Boss)
                        continue;

                    l_Boss->SetInCombatWithZone();
                }
            }

            ActivateHologramStalker(false);

            DefaultEvents(IsActiveBoss());

            _EnterCombat();
        }

        void EnterEvadeMode() override
        {
            /// EnterEvadeMode must be called for each boss before tl_InReset returns to false
            /// because it calls Reset() which need to know that boss is reseting
            if (CanResetHighCommand())
            {
                tl_InReset = true;
                EvadeHighCommand(this);
                BossAI::EnterEvadeMode();
                tl_InReset = false;
                return;
            }

            BossAI::EnterEvadeMode();
        }

        void Reset() override
        {
            if (m_HighCommandBosses.size() < 3)
                return;

            if (CanResetHighCommand())
            {
                tl_InReset = true;
                ResetHighCommand(this);
                tl_InReset = false;
            }

            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
            me->SetPower(Powers::POWER_ENERGY, 0);

            m_ActiveBoss = 0;
            m_BatIndex = 0;

            if (Creature* l_Pod = GetPod())
                l_Pod->AI()->DoAction(eScriptData::KickPlayer);

            if (Creature* l_Caster = GetPassiveCaster())
                l_Caster->Respawn(true, true, 0);

            _Reset();
        }

        void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
        {
            if (p_Damage >= me->GetHealth())
                return;

            DamageHighCommand(this, p_Damage);
        }

        void JustDied(Unit* p_Killer) override
        {
            if (!IsDying())
            {
                tl_Dying = true;
                TerminateHighCommand(this, p_Killer);
                tl_Dying = false;
            }

            Disembark();

            _JustDied();
        }

        virtual void DefaultEvents(bool p_Active)
        {
            m_PassiveEvents.Reset();
            events.Reset();

            m_ActiveTimer = 89 * TimeConstants::IN_MILLISECONDS;

            if (p_Active)
            {
                events.ScheduleEvent(eEvents::EventExploitWeakness, 8500);
                events.ScheduleEvent(eEvents::EventEnergyRegen, 9 * TimeConstants::IN_MILLISECONDS);
                if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                {
                    events.ScheduleEvent(eEvents::EventShockGrenade, 11 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventSummonScreamingShrike, 8 * TimeConstants::IN_MILLISECONDS);
                }
            }
            else
                m_PassiveEvents.ScheduleEvent(eEvents::EventChaosPulse, 0);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            UpdateOperations(p_Diff);

            if (!me->isInCombat())
                return;

            if (IsActiveBoss())
            {
                if (me->GetExactDist2d(&k_ExitRoom) < 15.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);
                m_ActiveTimer -= p_Diff;
                if (m_ActiveTimer > 0)
                {
                    UpdateAIActive();
                    return;
                }

                /// Prevents infinite phase change
                m_ActiveTimer = 10 * TimeConstants::IN_MILLISECONDS;

                EmbarkPod(false);
                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    boss_antoran_high_commandAI* l_NextBossAI = SwitchActiveBoss();
                    l_NextBossAI->Disembark();
                });
            }
            else
            {
                m_PassiveEvents.Update(p_Diff);
                ExecutePassiveEvent(m_PassiveEvents.ExecuteEvent());
            }
        }

        void UpdateAIActive()
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
                case eEvents::EventEnergyRegen:
                {
                    me->ModifyPower(Powers::POWER_ENERGY, 11);
                    events.ScheduleEvent(eEvents::EventEnergyRegen, 10 * TimeConstants::IN_MILLISECONDS);
                    break;
                }
                case eEvents::EventExploitWeakness:
                {
                    me->CastSpell(me->getVictim(), eSpells::ExploitWeakness, false);

                    events.ScheduleEvent(eEvents::EventExploitWeakness, 8500);
                    break;
                }
                case eEvents::EventShockGrenade:
                {
                    me->CastSpell(me, eSpells::ShockGrenade, false);

                    events.ScheduleEvent(eEvents::EventShockGrenade, 16 * TimeConstants::IN_MILLISECONDS);
                    break;
                }
                case eEvents::EventSummonScreamingShrike:
                {
                    if (GetPod() && GetPod()->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE))
                    {
                        Position l_Dest = k_ShrikeSpawnPositions[urand(0, k_ShrikeSpawnPositions.size() - 1)];
                        me->SummonCreature(eCreatures::NpcScreamingShrike, l_Dest);

                        if (++m_BatIndex == 5)
                        {
                            me->SummonCreature(eCreatures::NpcScreamingShrike, l_Dest);
                            m_BatIndex = 0;
                        }
                    }

                    events.ScheduleEvent(eEvents::EventSummonScreamingShrike, 8 * TimeConstants::IN_MILLISECONDS);
                    break;
                }
                default:
                    break;
            }

            DoMeleeAttackIfReady();
        }

        void FilterTargets(std::list<WorldObject *>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
        {
            switch (p_Spell->m_spellInfo->Id)
            {
                case eSpells::ShockGrenade:
                {
                    p_Targets.remove_if([](WorldObject* p_Target) -> bool
                    {
                        if (!p_Target->ToPlayer())
                            return true;

                        if (p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                            return true;

                        return false;
                    });
                }
                default:
                    break;
            }
        }

        void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
        {
            switch (p_SpellInfo->Id)
            {
                case eSpells::ShockGrenade:
                {
                    me->CastSpell(p_Target, eSpells::ShockGrenadeMissile, true);

                    break;
                }
                default:
                    break;
            }
        }

        virtual bool ExecutePassiveEvent(uint32 const p_Event)
        {
            switch (p_Event)
            {
                case eEvents::EventChaosPulse:
                {
                    if (Unit* l_Target = SelectPlayerTarget(UnitAI::TypeMaskNonTank, { -eSpells::PsychicAssault }))
                    {
                        uint64 l_TargetGUID = l_Target->GetGUID();

                        /// Fires 3 bursts of 3 bolts at the target
                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            for (uint8 l_J = 0; l_J < 3; ++l_J)
                            {
                                AddTimedDelayedOperation((l_I  * TimeConstants::IN_MILLISECONDS) + (l_J * 100), [this, l_TargetGUID]() -> void
                                {
                                    Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                                    if (!l_Target)
                                        return;

                                    Creature* l_Caster = GetPassiveCaster();
                                    if (!l_Caster)
                                        return;

                                    l_Caster->CastSpell(l_Target, eSpells::ChaosPulse, true);
                                });
                            }
                        }
                    }

                    m_PassiveEvents.ScheduleEvent(eEvents::EventChaosPulse, 9 * TimeConstants::IN_MILLISECONDS);
                    return true;
                }
                default:
                    break;
            }

            return false;
        }

    /// Helpers
    public:
        bool CanResetHighCommand() const
        {
            return !tl_InReset;
        }

        bool IsDying() const
        {
            return tl_Dying;
        }

        virtual Creature* GetPod(uint32 p_Entry = 0)
        {
            Creature* l_Pod = me->FindNearestCreature(p_Entry, 100.0f);
            if (!l_Pod || !l_Pod->IsAIEnabled)
                return nullptr;

            return l_Pod;
        }

        virtual Creature* GetPassiveCaster(uint32 p_Entry = 0)
        {
            Creature* l_Caster = me->FindNearestCreature(p_Entry, 100.0f);
            if (!l_Caster)
            {
                if ((l_Caster = me->FindNearestCreature(p_Entry, 100.0f, false)))
                    l_Caster->Respawn(true, true, 0);

                return nullptr;
            }

            l_Caster->SetReactState(ReactStates::REACT_PASSIVE);
            l_Caster->AddUnitState(UnitState::UNIT_STATE_ROOT);

            return l_Caster;
        }

        Creature* GetActiveBoss()
        {
            if (IsActiveBoss())
                return me;

            for (uint64 l_BossGUID : m_HighCommandBosses)
            {
                if (l_BossGUID == me->GetGUID())
                    continue;

                Creature* l_Boss = sObjectAccessor->GetCreature(*me, l_BossGUID);
                if (!l_Boss || !l_Boss->IsAIEnabled)
                    continue;

                if (l_Boss->GetEntry() != k_Bosses[m_ActiveBoss])
                    continue;

                return l_Boss;
            }

            return nullptr;
        }

        void EmbarkPod(bool p_Triggered)
        {
            if (p_Triggered)
            {
                Creature* l_Pod = GetPod();
                if (!l_Pod)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        EmbarkPod(true);
                    });

                    return;
                }

                l_Pod->AI()->DoAction(eScriptData::BossEnteringPod);
                l_Pod->AI()->DoAction(eScriptData::ClosePod);

                me->CastSpell(me, eSpells::Invisible, true);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetPower(Powers::POWER_ENERGY, 0);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->StopAttack();
                me->RemoveAllDoTAuras();
                me->NearTeleportTo(me->GetHomePosition());

                return;
            }

            me->CastSpell(me, eSpells::AssumeCommand, false);
            AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
            {
                EmbarkPod(true);
            });

            return;
        }

        void Disembark()
        {
            me->RemoveAura(eSpells::Invisible);
            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

            Creature* l_Pod = GetPod();
            if (!l_Pod)
                return;

            m_BatIndex = 0;

            l_Pod->AI()->DoAction(eScriptData::BossLeavingPod);

            if (me->isDead())
            {
                l_Pod->AI()->DoAction(eScriptData::KickPlayer);
                l_Pod->AI()->DoAction(eScriptData::ClosePod);
                return;
            }

            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
            {
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            });
        }

        void ActivateHologramStalker(bool p_Apply)
        {
            Creature* l_HologramStalker = me->FindNearestCreature(eCreatures::NpcHologramStalker, 100.0f);
            if (!l_HologramStalker)
                return;

            if (p_Apply)
                l_HologramStalker->CastSpell(l_HologramStalker, eSpells::HologramSpawn, true);
            else
                l_HologramStalker->RemoveAura(eSpells::HologramSpawn);
        }

        EventMap& GetPassiveEvents()
        {
            return m_PassiveEvents;
        }

        bool IsActiveBoss() const
        {
            return me->GetEntry() == k_Bosses[m_ActiveBoss];
        }

    /// Fight Management
    private:
        void BindSelfToHighCommand()
        {
            for (uint32 l_Entry : k_Bosses)
            {
                if (l_Entry == me->GetEntry())
                    continue;

                Creature* l_Boss = me->FindNearestCreature(l_Entry, 100.0f);
                if (!l_Boss)
                    l_Boss = me->FindNearestCreature(l_Entry, 100.0f, false);

                if (!l_Boss || !l_Boss->IsAIEnabled)
                    continue;

                boss_antoran_high_commandAI* l_BossAI = reinterpret_cast<boss_antoran_high_commandAI*>(l_Boss->AI());

                l_BossAI->RegisterHighCommandBoss(me);
                RegisterHighCommandBoss(l_Boss);
            }

            RegisterHighCommandBoss(me);
        }

        void RegisterHighCommandBoss(Creature* l_Boss)
        {
            m_HighCommandBosses.insert(l_Boss->GetGUID());
        }

        static void ResetHighCommand(boss_antoran_high_commandAI* l_Reseter)
        {
            for (uint64 l_BossGUID : l_Reseter->m_HighCommandBosses)
            {
                if (l_BossGUID == l_Reseter->me->GetGUID())
                    continue;

                Creature* l_Boss = sObjectAccessor->GetCreature(*l_Reseter->me, l_BossGUID);
                if (!l_Boss || !l_Boss->IsAIEnabled)
                    continue;

                l_Boss->AI()->Reset();
            }

            l_Reseter->ActivateHologramStalker(true);

            Map::PlayerList const& l_Players = l_Reseter->me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
            {
                Player* l_Player = l_Itr->getSource();
                if (!l_Player)
                    continue;

                l_Player->RemoveAura(eSpells::PsychicScaring);
            }
        }

        static void EvadeHighCommand(boss_antoran_high_commandAI* l_Reseter)
        {
            for (uint64 l_BossGUID : l_Reseter->m_HighCommandBosses)
            {
                if (l_BossGUID == l_Reseter->me->GetGUID())
                    continue;

                Creature* l_Boss = sObjectAccessor->GetCreature(*l_Reseter->me, l_BossGUID);
                if (!l_Boss || !l_Boss->IsAIEnabled)
                    continue;

                l_Boss->AI()->EnterEvadeMode();
            }

            l_Reseter->me->DespawnCreaturesInArea(k_EntriesToDespawn, 250.0f);
        }

        static void DamageHighCommand(boss_antoran_high_commandAI* l_DamagedBoss, uint32 p_Damage)
        {
            for (uint64 l_BossGUID : l_DamagedBoss->m_HighCommandBosses)
            {
                if (l_BossGUID == l_DamagedBoss->me->GetGUID())
                    continue;

                Creature* l_Boss = sObjectAccessor->GetCreature(*l_DamagedBoss->me, l_BossGUID);
                if (!l_Boss)
                    continue;

                if (l_Boss->GetHealth() > p_Damage)
                    l_Boss->ModifyHealth(-static_cast<int32>(p_Damage));
            }
        }

        static void TerminateHighCommand(boss_antoran_high_commandAI* l_DyingBoss, Unit* p_Killer)
        {
            for (uint64 l_BossGUID : l_DyingBoss->m_HighCommandBosses)
            {
                if (l_BossGUID == l_DyingBoss->me->GetGUID())
                    continue;

                Creature* l_Boss = sObjectAccessor->GetCreature(*l_DyingBoss->me, l_BossGUID);
                if (!l_Boss)
                    continue;

                p_Killer->Kill(l_Boss);

                if (!l_Boss->IsAIEnabled)
                    continue;

                if (Creature* l_Caster = reinterpret_cast<boss_antoran_high_commandAI*>(l_Boss->AI())->GetPassiveCaster())
                    l_Caster->Respawn(true, true, 0);
            }

            Map::PlayerList const& l_Players = l_DyingBoss->me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
            {
                Player* l_Player = l_Itr->getSource();
                if (!l_Player)
                    continue;

                l_Player->RemoveAura(eSpells::PsychicScaring);
            }

            l_DyingBoss->me->DespawnCreaturesInArea(k_EntriesToDespawn, 250.0f);
        }

        boss_antoran_high_commandAI* SwitchActiveBoss()
        {
            boss_antoran_high_commandAI* l_NextBoss = nullptr;

            for (uint64 l_BossGUID : m_HighCommandBosses)
            {
                Creature* l_Boss = sObjectAccessor->GetCreature(*me, l_BossGUID);
                if (!l_Boss || !l_Boss->IsAIEnabled)
                    continue;

                boss_antoran_high_commandAI* l_BossAI = reinterpret_cast<boss_antoran_high_commandAI*>(l_Boss->AI());

                if (l_BossAI->me->GetEntry() == k_Bosses[++l_BossAI->m_ActiveBoss])
                    l_NextBoss = l_BossAI;

                l_BossAI->DefaultEvents(l_BossAI->IsActiveBoss());
            }

            return l_NextBoss;
        }

    private:
        std::set<uint64> m_HighCommandBosses;
        EventMap m_PassiveEvents;
        Occurence m_ActiveBoss;
        int32 m_ActiveTimer;
        uint32 m_BatIndex;
};

/// Last Update 7.3.5 - Build 26365
/// Admiral Svirax (Antoran High Command) - 122367
class boss_high_command_svirax : public CreatureScript
{
    public:
        boss_high_command_svirax() : CreatureScript("boss_high_command_svirax") { }

        struct boss_high_command_sviraxAI : boss_antoran_high_commandAI
        {
            boss_high_command_sviraxAI(Creature* p_Creature) : boss_antoran_high_commandAI(p_Creature) { }

            Creature* GetPod(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_commandAI::GetPod(eCreatures::NpcAdmiralsPod);
            }

            Creature* GetPassiveCaster(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_commandAI::GetPassiveCaster(eCreatures::NpcPassiveAdmiralSvirax);
            }

            void Reset() override
            {
                boss_antoran_high_commandAI::Reset();
                Disembark();
                if (Creature* l_Pod = GetPod())
                    l_Pod->AI()->DoAction(eScriptData::DisablePod);

                me->RemoveAura(eSpells::ZeroingIn);
                me->m_SpellHelper.GetUint64Set()[eSpellHelpers::Fusillade].clear();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                boss_antoran_high_commandAI::EnterCombat(p_Attacker);
                if (Creature* l_Pod = GetPod())
                    l_Pod->AI()->DoAction(eScriptData::EnablePod);

                Talk(eTalks::TalkAggro);
            }

            void DefaultEvents(bool p_Active) override
            {
                boss_antoran_high_commandAI::DefaultEvents(p_Active);

                GetPassiveEvents().ScheduleEvent(eEvents::EventFusillade, 11 * TimeConstants::IN_MILLISECONDS);
            }

            bool ExecutePassiveEvent(uint32 const p_Event) override
            {
                if (boss_antoran_high_commandAI::ExecutePassiveEvent(p_Event))
                    return true;

                switch (p_Event)
                {
                    case eEvents::EventFusillade:
                    {
                        Talk(eTalks::TalkPassiveMechanic);

                        if (Creature* l_PassiveCaster = GetPassiveCaster())
                            l_PassiveCaster->SummonCreature(eCreatures::NpcLegionCruiser, k_LegionCruiserSpawn);

                        GetPassiveEvents().ScheduleEvent(eEvents::EventFusillade, 30 * TimeConstants::IN_MILLISECONDS);
                        return true;
                    }
                    default:
                        break;
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_high_command_sviraxAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Chief Engineer Ishkar (Antoran High Command) - 122369
class boss_high_command_ishkar : public CreatureScript
{
    public:
        boss_high_command_ishkar() : CreatureScript("boss_high_command_ishkar") { }

        struct boss_high_command_ishkarAI : boss_antoran_high_commandAI
        {
            boss_high_command_ishkarAI(Creature* p_Creature) : boss_antoran_high_commandAI(p_Creature) { }

            enum eAchievements
            {
                ThisIsTheWarRoom = 12129
            };

            uint32 m_AchievementCounter;

            Creature* GetPod(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_commandAI::GetPod(eCreatures::NpcEngineersPod);
            }

            Creature* GetPassiveCaster(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_commandAI::GetPassiveCaster(eCreatures::NpcPassiveChiefEngineerIshkar);
            }

            void EnterEvadeMode() override
            {
                boss_antoran_high_commandAI::EnterEvadeMode();
                EmbarkPod(true);
            }

            void Reset() override
            {
                boss_antoran_high_commandAI::Reset();
                AddTimedDelayedOperation(1, [this]()-> void
                {
                    EmbarkPod(true);
                });

                m_AchievementCounter = 0;
            }

            void DefaultEvents(bool p_Active) override
            {
                boss_antoran_high_commandAI::DefaultEvents(p_Active);
                if (p_Active)
                    return;

                GetPassiveEvents().ScheduleEvent(eEvents::EventSummonEntropicMines, 15 * TimeConstants::IN_MILLISECONDS);
            }

            bool ExecutePassiveEvent(uint32 const p_Event) override
            {
                if (boss_antoran_high_commandAI::ExecutePassiveEvent(p_Event))
                    return true;

                switch (p_Event)
                {
                    case eEvents::EventSummonEntropicMines:
                    {
                        Talk(eTalks::TalkPassiveMechanic);

                        if (Unit* l_Target = SelectPlayerTarget(UnitAI::TypeMaskAllRanged, { -eSpells::PsychicAssault }))
                        {
                            SpellInfo const* l_CountSpellInfo = sSpellMgr->GetSpellInfo(eSpells::EntropicMinesCount);
                            if (!l_CountSpellInfo)
                                return true;

                            for (uint32 l_I = 0; l_I < l_CountSpellInfo->Effects[EFFECT_0].BasePoints; ++l_I)
                            {
                                Position l_Dest;
                                float l_MinDist = me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic ? 4.0f : 7.0f;
                                float l_MaxDist = me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic ? 20.0f : 15.0f;
                                float l_Dist = frand(l_MinDist, l_MaxDist);
                                float l_Angle = frand(0.0f, 2.0f * M_PI);
                                l_Target->SimplePosXYRelocationByAngle(l_Dest, l_Dist, l_Angle);
                                if (!l_Target->IsWithinLOS(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ))
                                {
                                    bool l_Found = false;
                                    for (float l_AngleOffset = M_PI / 4.0f; l_AngleOffset < 2.0f * M_PI; l_AngleOffset += M_PI / 4.0f)
                                    {
                                        l_Target->SimplePosXYRelocationByAngle(l_Dest, l_Dist, l_Angle + l_AngleOffset);
                                        if (l_Target->IsWithinLOS(l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ))
                                        {
                                            l_Found = true;
                                            l_Angle = l_Angle + l_AngleOffset;
                                            break;
                                        }
                                    }

                                    if (!l_Found)
                                        l_Dest = *l_Target;
                                }

                                me->CastSpell(l_Dest, eSpells::SummonEntropicMines, true);
                            }
                        }

                        GetPassiveEvents().ScheduleEvent(eEvents::EventSummonEntropicMines, 10 * TimeConstants::IN_MILLISECONDS);
                        return true;
                    }
                    default:
                        break;
                }

                return false;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eScriptData::AchievementCounter:
                        m_AchievementCounter++;
                        break;
                    default:
                        break;
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                boss_antoran_high_commandAI::JustDied(p_Killer);

                if (m_AchievementCounter > 3)
                    return;

                AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::ThisIsTheWarRoom);
                if (!l_AchievementEntry)
                    return;

                Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                {
                    Player* l_Player = l_Itr->getSource();
                    if (!l_Player)
                        continue;

                    l_Player->CompletedAchievement(l_AchievementEntry);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_high_command_ishkarAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// General Erodus (Antoran High Command) - 122333
class boss_high_command_erodus : public CreatureScript
{
    public:
        boss_high_command_erodus() : CreatureScript("boss_high_command_erodus") { }

        struct boss_high_command_erodusAI : boss_antoran_high_commandAI
        {
            boss_high_command_erodusAI(Creature* p_Creature) : boss_antoran_high_commandAI(p_Creature) { }

            Creature* GetPod(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_commandAI::GetPod(eCreatures::NpcGeneralsPod);
            }

            Creature* GetPassiveCaster(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_commandAI::GetPassiveCaster(eCreatures::NpcPassiveGeneralErodus);
            }

            void EnterEvadeMode() override
            {
                boss_antoran_high_commandAI::EnterEvadeMode();
                EmbarkPod(true);
            }

            void Reset() override
            {
                boss_antoran_high_commandAI::Reset();
                AddTimedDelayedOperation(1, [this]()-> void
                {
                    EmbarkPod(true);
                });
            }

            void DefaultEvents(bool p_Active) override
            {
                boss_antoran_high_commandAI::DefaultEvents(p_Active);
                if (p_Active)
                    return;

                GetPassiveEvents().ScheduleEvent(eEvents::EventSummonReinforcements, 8 * TimeConstants::IN_MILLISECONDS);
            }

            bool ExecutePassiveEvent(uint32 const p_Event) override
            {
                if (boss_antoran_high_commandAI::ExecutePassiveEvent(p_Event))
                    return true;

                switch (p_Event)
                {
                    case eEvents::EventSummonReinforcements:
                    {
                        Talk(eTalks::TalkPassiveMechanic);

                        Creature* l_ActiveBoss = GetActiveBoss();
                        if (!l_ActiveBoss)
                            return true;

                        me->CastSpell(l_ActiveBoss, eSpells::SummonReinforcements, true);

                        GetPassiveEvents().ScheduleEvent(eEvents::EventSummonReinforcements, 35 * TimeConstants::IN_MILLISECONDS);
                        return true;
                    }
                    default:
                        break;
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_high_command_erodusAI(p_Creature);
        }
};

struct boss_antoran_high_command_podAI : ScriptedAI
{
    public:
        boss_antoran_high_command_podAI(Creature* p_Creature) : ScriptedAI(p_Creature)
        {
            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
            me->SetPower(Powers::POWER_ENERGY, 0);

            me->AddUnitState(UnitState::UNIT_STATE_ROOT);

            if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                me->m_spells[2] = eSpells::DefensiveCountermeasures;

            m_BossInPod = false;
            Open();
        }

        enum eData
        {
            OpenedPodAnimKit = 8810
        };

        enum eSpells
        {
            PsychicAssault              = 244172,
            PsychicScaring              = 244388,

            ChaosPulse                  = 244418,
            ChaosPulseMissile           = 244419,
            WitheringFire               = 245027,
            WitheringFireDamage         = 245103,
            WitheringFireMissile        = 245085,

            DefensiveCountermeasures    = 254219
        };

        enum eEvents
        {
            EventPsychicAssault = 1,
        };

    /// Herited Function
    public:
        void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
        {
            if (p_Apply)
            {
                for (uint32 l_BossEntry : k_Bosses)
                {
                    Creature* l_Boss = me->FindNearestCreature(l_BossEntry, 150.0f);
                    if (!l_Boss || !l_Boss->IsAIEnabled)
                        return;

                    reinterpret_cast<ScriptedAI*>(l_Boss->AI())->DoModifyThreatPercent(p_Passenger, -100);
                }

                events.ScheduleEvent(eEvents::EventPsychicAssault, 1);
                Close();

                return;
            }

            Position l_Dest = *me;
            l_Dest.m_positionZ += 0.5f; ///< Hackfix because pods are slightly below ground level ...
            l_Dest.SimplePosXYRelocationByAngle(l_Dest, 15.0f, 0.0f); ///< ... and there is now ground immediatly under them
            p_Passenger->AddDelayedEvent([p_Passenger, l_Dest]() -> void
            {
                p_Passenger->NearTeleportTo(l_Dest);
            }, 1);

            p_Passenger->RemoveAura(eSpells::PsychicAssault);

            if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic ||
                me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
            {
                if (Aura* l_Aura = p_Passenger->AddAura(eSpells::PsychicScaring, p_Passenger))
                {
                    if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                        l_Aura->SetDuration(-1);
                }
            }

            events.Reset();

            Open();
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            UpdateOperations(p_Diff);
            events.Update(p_Diff);

            if (me->GetDistance(me->GetHomePosition()) > 1.0f)
                me->NearTeleportTo(me->GetHomePosition());

            switch (events.ExecuteEvent())
            {
                case eEvents::EventPsychicAssault:
                {
                    Unit* l_Passenger = me->GetVehicleKit()->GetPassenger(0);
                    if (!l_Passenger)
                        return;

                    l_Passenger->ClearUnitState(UnitState::UNIT_STATE_ONVEHICLE);

                    me->CastSpell(l_Passenger, eSpells::PsychicAssault, true);

                    if (me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic ||
                        me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                        events.ScheduleEvent(eEvents::EventPsychicAssault, 2 * TimeConstants::IN_MILLISECONDS);
                }
                default:
                    break;
            }
        }

        bool CanTargetOutOfLOS() override
        {
            return true;
        }

        void DoCast(Unit* p_Victim, uint32 p_SpellId, bool p_Triggered = false) override
        {
            Creature* l_PassiveCaster = GetPassiveCaster();
            if (!l_PassiveCaster)
                return;

            l_PassiveCaster->CastSpell(p_Victim, p_SpellId, p_Triggered, nullptr, nullptr, me->GetGUID());
        }

        void DoCast(Position p_Position, uint32 p_SpellId, bool p_Triggered = false) override
        {
            Creature* l_PassiveCaster = GetPassiveCaster();
            if (!l_PassiveCaster)
                return;

            l_PassiveCaster->CastSpell(p_Position, p_SpellId, p_Triggered, nullptr, nullptr, me->GetGUID());
        }

        void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
        {
            switch (p_SpellInfo->Id)
            {
                case eSpells::ChaosPulse:
                {
                    uint64 l_TargetGUID = p_Target->GetGUID();
                    for (uint8 l_I = 0; l_I < 3; ++l_I)
                    {
                        AddTimedDelayedOperation(l_I * 200, [this, l_TargetGUID]() -> void
                        {
                            Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                            if (!l_Target)
                                return;

                            DoCast(l_Target, eSpells::ChaosPulseMissile, true);
                        });
                    }

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
                case eScriptData::BossEnteringPod:
                {
                    if (m_BossInPod)
                        break;

                    m_BossInPod = true;

                    Open();
                    if (Unit* l_Passenger = me->GetVehicleKit()->GetPassenger(0))
                        l_Passenger->ExitVehicle();

                    break;
                }
                case eScriptData::ClosePod:
                {
                    Close();
                    break;
                }
                case eScriptData::BossLeavingPod:
                {
                    m_BossInPod = false;
                    /// No Break Intended
                }
                case eScriptData::OpenPod:
                {
                    Open();
                    break;
                }
                case eScriptData::KickPlayer:
                {
                    if (Unit* l_Passenger = me->GetVehicleKit()->GetPassenger(0))
                    {
                        Open();
                        l_Passenger->ExitVehicle();
                    }

                    break;
                }
                case eScriptData::DisablePod:
                {
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    break;
                }
                case eScriptData::EnablePod:
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    break;
                }
                default:
                    break;
            }
        }

    /// Helpers
    public:
        virtual Creature* GetPassiveCaster(uint32 p_Entry = 0)
        {
            Creature* l_Caster = me->FindNearestCreature(p_Entry, 100.0f);
            if (!l_Caster)
                return nullptr;

            l_Caster->SetReactState(ReactStates::REACT_PASSIVE);
            l_Caster->AddUnitState(UnitState::UNIT_STATE_ROOT);

            return l_Caster;
        }

        void Open()
        {
            me->SetAIAnimKitId(eData::OpenedPodAnimKit);
            me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
        }

        void Close()
        {
            me->SetAIAnimKitId(0);
            me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
        }

        bool IsBossInPod()
        {
            return m_BossInPod;
        }

    private:
        bool m_BossInPod;
};

/// Last Update 7.3.5 - Build 26365
/// Admiral's Pod - 122554
class npc_high_command_admirals_pod : public CreatureScript
{
    public:
        npc_high_command_admirals_pod() : CreatureScript("npc_high_command_admirals_pod") { }

        struct npc_high_command_admirals_podAI : public boss_antoran_high_command_podAI
        {
            npc_high_command_admirals_podAI(Creature* p_Creature) : boss_antoran_high_command_podAI(p_Creature) { }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::WitheringFire:
                    {
                        if (p_Mode != SPELL_EFFECT_HANDLE_HIT)
                            break;

                        Position l_Dest = p_Dest->_position;
                        for (uint8 l_I = 0; l_I < 8; ++l_I)
                        {
                            AddTimedDelayedOperation(l_I * 100, [this, l_Dest]() -> void
                            {
                                std::list<Creature*> l_ScoutShips;
                                me->GetCreatureListWithEntryInGrid(l_ScoutShips, eCreatures::NpcScoutShip, 150.0f);
                                for (Creature* l_ScoutShip : l_ScoutShips)
                                {
                                    Position l_MissileDest;
                                    l_Dest.SimplePosXYRelocationByAngle(l_MissileDest, frand(0.0f, 8.0f), frand(0.0f, M_PI));
                                    l_ScoutShip->CastSpell(l_MissileDest, eSpells::WitheringFireMissile, true);
                                    AddTimedDelayedOperation(TimeConstants::IN_MILLISECONDS, [this, l_MissileDest]() ->void
                                    {
                                        DoCast(l_MissileDest, eSpells::WitheringFireDamage, true);
                                    });
                                }
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            Creature* GetPassiveCaster(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_command_podAI::GetPassiveCaster(eCreatures::NpcPassiveAdmiralSvirax);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_admirals_podAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Engineer's Pod - 123020
class npc_high_command_engineers_pod : public CreatureScript
{
    public:
        npc_high_command_engineers_pod() : CreatureScript("npc_high_command_engineers_pod") { }

        struct npc_high_command_engineers_podAI : public boss_antoran_high_command_podAI
        {
            npc_high_command_engineers_podAI(Creature* p_Creature) : boss_antoran_high_command_podAI(p_Creature) { }

            enum eSpells
            {
                SummonFelshieldEmitter = 244902
            };

            Creature* GetPassiveCaster(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_command_podAI::GetPassiveCaster(eCreatures::NpcPassiveChiefEngineerIshkar);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SummonFelshieldEmitter:
                    {
                        Creature* l_Ishkar = me->FindNearestCreature(eCreatures::BossChiefEngineerIshkar, 150.0f);
                        if (!l_Ishkar || !l_Ishkar->IsAIEnabled)
                            return;

                        l_Ishkar->AI()->DoAction(eScriptData::AchievementCounter);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_engineers_podAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// General's Pod - 123013
class npc_high_command_generals_pod : public CreatureScript
{
    public:
        npc_high_command_generals_pod() : CreatureScript("npc_high_command_generals_pod") { }

        struct npc_high_command_generals_podAI : public boss_antoran_high_command_podAI
        {
            npc_high_command_generals_podAI(Creature* p_Creature) : boss_antoran_high_command_podAI(p_Creature) { }

            Creature* GetPassiveCaster(uint32 p_Entry = 0) override
            {
                return boss_antoran_high_command_podAI::GetPassiveCaster(eCreatures::NpcPassiveGeneralErodus);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_generals_podAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Entropic Mine - 122992
class npc_high_command_entropic_mine : public CreatureScript
{
    public:
        npc_high_command_entropic_mine() : CreatureScript("npc_high_command_entropic_mine") { }

        struct npc_high_command_entropic_mineAI : public ScriptedAI
        {
            npc_high_command_entropic_mineAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Delay = 0;
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            int32 m_Delay;
            int32 m_ForceExplosionDelay;
            bool m_Activated;

            enum eSpells
            {
                CosmeticFadeIn      = 257262,
                CosmeticRadius      = 246601,
                EntropicMinesData   = 245161,
                EntropicBlast       = 245121,
                EntropicBlastDot    = 253290
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::CosmeticRadius, true);
                AddTimedDelayedOperation(50, [this]()->void
                {
                    me->CastSpell(me, eSpells::CosmeticFadeIn, true);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                });

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EntropicMinesData);
                if (!l_SpellInfo)
                    return;

                m_Delay = l_SpellInfo->Effects[EFFECT_2].BasePoints * TimeConstants::IN_MILLISECONDS;
                m_ForceExplosionDelay = 3 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS;
                m_Activated = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Activated)
                    return;

                m_Delay -= p_Diff;
                m_ForceExplosionDelay -= p_Diff;
                if (m_ForceExplosionDelay < 0)
                {
                    Explode();
                    return;
                }

                if (m_Delay > 0)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EntropicMinesData);
                if (!l_SpellInfo)
                    return;

                Player* l_NearestPlayer = me->FindNearestPlayer(static_cast<float>(l_SpellInfo->Effects[EFFECT_1].BasePoints));
                if (!l_NearestPlayer || me->GetExactDist2d(l_NearestPlayer) > 3.0f)
                    return;

                Explode();
            }

            void Explode()
            {
                me->CastSpell(me, eSpells::EntropicBlast, true);
                me->CastSpell(me, eSpells::EntropicBlastDot, true);

                m_Activated = true;
                me->GetMotionMaster()->Clear();
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->SetVisible(false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_entropic_mineAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Legion Cruiser - 122739
class npc_high_command_legion_cruiser : public CreatureScript
{
    public:
        npc_high_command_legion_cruiser() : CreatureScript("npc_high_command_legion_cruiser") { }

        struct npc_high_command_legion_cruiserAI : public ScriptedAI
        {
            npc_high_command_legion_cruiserAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetReactState(ReactStates::REACT_PASSIVE);

                m_SummonerGUID = 0;
            }

            enum eTalks
            {
                TalkLegionCruiserWarning,
                TalkLegionCruiserCommand
            };

            enum eSpells
            {
                Fusillade           = 244625,
                FusilladeMissile    = 244626,
                ZeroingIn           = 244696,

                SummonShadowTrigger = 256436
            };

            uint64 m_SummonerGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Talk(eTalks::TalkLegionCruiserWarning);

                m_SummonerGUID = p_Summoner->GetGUID();

                if (Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcHologramStalker, 300.0f))
                    l_Stalker->CastSpell(l_Stalker, eSpells::SummonShadowTrigger, false);

                AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Summoner = sObjectAccessor->GetCreature(*me, m_SummonerGUID);
                    if (l_Summoner && l_Summoner->IsAIEnabled)
                        l_Summoner->AI()->Talk(eTalks::TalkLegionCruiserCommand);

                    me->CastSpell(me, eSpells::Fusillade, false);
                });
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::Fusillade:
                    {
                        for (Position const& l_Pos : k_FusilladeTargets)
                            me->CastSpell(l_Pos, eSpells::FusilladeMissile, true, nullptr, nullptr, m_SummonerGUID);

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::ZeroingIn, true);

                            if (Creature* l_AdmiralSvirax = me->FindNearestCreature(eCreatures::BossAdmiralSvirax, 250.0f))
                                l_AdmiralSvirax->m_SpellHelper.GetUint64Set()[eSpellHelpers::Fusillade].clear();

                            me->DespawnOrUnsummon(1);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_legion_cruiserAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Fusillade - 244627
class spell_svirax_fusillade : public SpellScriptLoader
{
    public:
        spell_svirax_fusillade() : SpellScriptLoader("spell_svirax_fusillade") { }

        class spell_svirax_fusillade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_svirax_fusillade_SpellScript);

            enum eSpells
            {
                ZeroingIn       = 244696,
                PsychicAssault  = 244172
            };

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::PsychicAssault))
                {
                    SetHitDamage(0);
                    return;
                }

                Creature* l_AdmiralSvirax = l_Caster->FindNearestCreature(eCreatures::BossAdmiralSvirax, 250.0f);
                if (!l_AdmiralSvirax)
                    return;

                if (l_AdmiralSvirax->m_SpellHelper.GetUint64Set()[eSpellHelpers::Fusillade].count(l_Target->GetGUID()))
                {
                    SetHitDamage(0);
                    return;
                }

                l_AdmiralSvirax->m_SpellHelper.GetUint64Set()[eSpellHelpers::Fusillade].insert(l_Target->GetGUID());

                AuraEffect const* l_AuraEffect = l_AdmiralSvirax->GetAuraEffect(eSpells::ZeroingIn, EFFECT_1);
                if (!l_AuraEffect)
                    return;

                int32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_AuraEffect->GetAmount());
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_svirax_fusillade_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_svirax_fusillade_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Activate Felshield - 244907
class spell_antoran_high_command_felshield : public SpellScriptLoader
{
    public:
        spell_antoran_high_command_felshield() : SpellScriptLoader("spell_antoran_high_command_felshield") { }

        class spell_antoran_high_command_felshield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antoran_high_command_felshield_SpellScript);

            enum eSpells
            {
                Felshield = 244909
            };

            void HandleAfterCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target || !l_Target->ToCreature() || !l_Target->HasFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK))
                    return;

                l_Target->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                l_Target->CastSpell(l_Target, eSpells::Felshield, true);

                l_Target->ToCreature()->DespawnOrUnsummon(10 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_antoran_high_command_felshield_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antoran_high_command_felshield_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Fanatical Pyromancer - 122890
class npc_high_command_fanatical_pyromancer : public CreatureScript
{
    public:
        npc_high_command_fanatical_pyromancer() : CreatureScript("npc_high_command_fanatical_pyromancer") { }

        struct npc_high_command_fanatical_pyromancerAI : public LegionCombatAI
        {
            npc_high_command_fanatical_pyromancerAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                EventPyroblast = 1
            };

            enum eSpells
            {
                PsychicAssault  = 244172
            };

            Unit* GetTarget(std::list<Unit*>& p_TargetList, uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventPyroblast:
                        p_TargetList.remove_if([](Unit* p_Target) -> bool
                        {
                            if (p_Target->HasAura(eSpells::PsychicAssault))
                                return true;

                            return false;
                        });

                        if (!p_TargetList.empty())
                        {
                            JadeCore::RandomResizeList(p_TargetList, 1);
                            return p_TargetList.front();
                        }

                        return nullptr;
                    default:
                        break;
                }

                return LegionCombatAI::GetTarget(p_TargetList, p_EventID);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_fanatical_pyromancerAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Felblade Shocktrooper - 122718
class npc_high_command_felblade_shocktrooper : public CreatureScript
{
    public:
        npc_high_command_felblade_shocktrooper() : CreatureScript("npc_high_command_felblade_shocktrooper") { }

        struct npc_high_command_felblade_shocktrooperAI : public LegionCombatAI
        {
            npc_high_command_felblade_shocktrooperAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                EventBladestorm = 1
            };

            enum eSpells
            {
                BladeStorm      = 253038,
                PsychicAssault  = 244172
            };

            Unit* GetTarget(std::list<Unit*>& p_TargetList, uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventBladestorm:
                        p_TargetList.remove_if([](Unit* p_Target) -> bool
                        {
                            if (!p_Target->ToPlayer())
                                return true;

                            if (p_Target->ToPlayer()->IsActiveSpecTankSpec() || p_Target->ToPlayer()->IsMeleeDamageDealer())
                                return true;

                            if (p_Target->HasAura(eSpells::PsychicAssault))
                                return true;

                            return false;
                        });

                        if (!p_TargetList.empty())
                        {
                            JadeCore::RandomResizeList(p_TargetList, 1);
                            return p_TargetList.front();
                        }

                        return me;
                    default:
                        break;
                }

                return LegionCombatAI::GetTarget(p_TargetList, p_EventID);
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventBladestorm:
                        if (p_Target != me)
                            me->GetMotionMaster()->MoveCharge(p_Target);
                        else
                            me->CastSpell(me, eSpells::BladeStorm, false);

                        return true;
                    default:
                        break;
                }

                return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_Id != EventId::EVENT_CHARGE)
                    return;

                me->CastSpell(me, eSpells::BladeStorm, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_felblade_shocktrooperAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Disruptor Beacon - 122884
class npc_high_command_disruptor_beacon : public CreatureScript
{
    public:
        npc_high_command_disruptor_beacon() : CreatureScript("npc_high_command_disruptor_beacon") { }

        struct npc_high_command_disruptor_beaconAI : public ScriptedAI
        {
            npc_high_command_disruptor_beaconAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            enum eSpells
            {
                DisruptionField = 254771
            };

            uint64 m_SummonerGUID;

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::DisruptionField:
                    {
                        if (p_Target->GetEntry() != eCreatures::NpcEntropicMine)
                            break;

                        p_Target->ToCreature()->DespawnOrUnsummon();
                    }
                    default:
                        break;
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGUID = p_Summoner->GetGUID();

                for (uint32 l_I = 0; l_I < 3; ++l_I)
                {
                    AddTimedDelayedOperation(l_I * 3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->CastSpell(me, eSpells::DisruptionField, true, nullptr, nullptr, m_SummonerGUID);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_disruptor_beaconAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Enter Pod - 244141
class spell_antoran_high_command_enter_pod : public SpellScriptLoader
{
    public:
        spell_antoran_high_command_enter_pod() : SpellScriptLoader("spell_antoran_high_command_enter_pod") { }

        class spell_antoran_high_command_enter_pod_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antoran_high_command_enter_pod_SpellScript);

            enum eSpells
            {
                PsychicScaring = 244388
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->HasAura(eSpells::PsychicScaring))
                    return SPELL_FAILED_CASTER_AURASTATE;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_antoran_high_command_enter_pod_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antoran_high_command_enter_pod_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Chaos Pulse - 244418
class spell_antoran_high_command_chaos_pulse_pod : public SpellScriptLoader
{
    public:
        spell_antoran_high_command_chaos_pulse_pod() : SpellScriptLoader("spell_antoran_high_command_chaos_pulse_pod") { }

        class spell_antoran_high_command_chaos_pulse_pod_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antoran_high_command_chaos_pulse_pod_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (l_Target->IsPlayer())
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_antoran_high_command_chaos_pulse_pod_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antoran_high_command_chaos_pulse_pod_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Shock Grenade - 244737
class spell_antoran_high_command_shock_grenade : public SpellScriptLoader
{
    public:
        spell_antoran_high_command_shock_grenade() : SpellScriptLoader("spell_antoran_high_command_shock_grenade") { }

        class spell_antoran_high_command_shock_grenade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antoran_high_command_shock_grenade_AuraScript);

            enum eSpells
            {
                Shocked = 244748
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::Shocked, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_antoran_high_command_shock_grenade_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antoran_high_command_shock_grenade_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Defensive Countermeasures - 254219
class spell_antoran_high_command_defensive_countermeasures : public SpellScriptLoader
{
    public:
        spell_antoran_high_command_defensive_countermeasures() : SpellScriptLoader("spell_antoran_high_command_defensive_countermeasures") { }

        class spell_antoran_high_command_defensive_countermeasures_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_antoran_high_command_defensive_countermeasures_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target || l_Target->GetEntry() != eCreatures::NpcScreamingShrike)
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_antoran_high_command_defensive_countermeasures_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_antoran_high_command_defensive_countermeasures_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Screaming Shrike - 128069
class npc_high_command_screaming_shrike : public CreatureScript
{
    public:
        npc_high_command_screaming_shrike() : CreatureScript("npc_high_command_screaming_shrike") { }

        struct npc_high_command_screaming_shrikeAI : public ScriptedAI
        {
            npc_high_command_screaming_shrikeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_SummonerGUID = 0;
                m_TargetGUID = 0;
            }

            enum eSpells
            {
                ExtractInterloper = 254130
            };

            uint64 m_SummonerGUID;
            uint64 m_TargetGUID;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGUID = p_Summoner->GetGUID();
                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    Creature* l_ActiveBoss = sObjectAccessor->GetCreature(*me, m_SummonerGUID);
                    if (!l_ActiveBoss || !l_ActiveBoss->IsAIEnabled)
                        return;

                    boss_antoran_high_commandAI* l_ActiveBossAI = reinterpret_cast<boss_antoran_high_commandAI*>(l_ActiveBoss->AI());
                    if (!l_ActiveBossAI)
                        return;

                    Creature* l_Pod = l_ActiveBossAI->GetPod();
                    if (!l_Pod)
                        return;

                    Position l_Dest;
                    me->SimplePosXYRelocationByAngle(l_Dest, me->GetExactDist2d(l_Pod) - 10.0f, me->GetAngle(l_Pod), true);
                    l_Dest.m_positionZ = 145.0f;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, l_Dest);
                    m_TargetGUID = l_Pod->GetGUID();
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                Creature* l_ActiveBoss = sObjectAccessor->GetCreature(*me, m_SummonerGUID);
                if (!l_ActiveBoss || !l_ActiveBoss->IsAIEnabled)
                    return;

                boss_antoran_high_commandAI* l_ActiveBossAI = reinterpret_cast<boss_antoran_high_commandAI*>(l_ActiveBoss->AI());
                if (!l_ActiveBossAI)
                    return;

                Creature* l_Pod = l_ActiveBossAI->GetPod();
                if (!l_Pod)
                    return;

                if (l_Pod != sObjectAccessor->GetCreature(*me, m_TargetGUID))
                {
                    me->InterruptNonMeleeSpells(true);

                    Position l_Dest;
                    me->SimplePosXYRelocationByAngle(l_Dest, me->GetExactDist2d(l_Pod) - 10.0f, me->GetAngle(l_Pod), true);
                    l_Dest.m_positionZ = 145.0f;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, l_Dest);
                    m_TargetGUID = l_Pod->GetGUID();
                }

                if (me->GetExactDist2d(l_Pod) > 16.0f)
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                me->GetMotionMaster()->Clear(false);
                me->StopMoving();

                me->CastSpell(l_Pod, eSpells::ExtractInterloper, false);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (me->isDying() || me->isDead())
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::ExtractInterloper:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            Creature* l_Pod = sObjectAccessor->GetCreature(*me, m_TargetGUID);
                            if (!l_Pod || !l_Pod->IsAIEnabled)
                                return;

                            l_Pod->AI()->DoAction(eScriptData::KickPlayer);

                            me->CastSpell(l_Pod, eSpells::ExtractInterloper, false);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_command_screaming_shrikeAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Psychic Assault - 244172
class spell_antoran_high_command_psychic_assault : public SpellScriptLoader
{
    public:
        spell_antoran_high_command_psychic_assault() : SpellScriptLoader("spell_antoran_high_command_psychic_assault") { }

        class spell_antoran_high_command_psychic_assault_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_antoran_high_command_psychic_assault_AuraScript);

            void HandlePeriodicDamage(AuraEffect const* p_AuraEffect)
            {
                uint32 l_StackAmount = GetStackAmount();

                /// Formula from reverse calculations
                float l_Damage = p_AuraEffect->GetBaseAmount();
                for (uint32 l_I = 1; l_I < l_StackAmount; ++l_I)
                    l_Damage += l_Damage * std::exp(-0.003f * std::pow(std::abs(int(l_I) - 15), 2)) * 6.5f / 100.0f;

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(uint32(l_Damage));
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_antoran_high_command_psychic_assault_AuraScript::HandlePeriodicDamage, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_antoran_high_command_psychic_assault_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_antoran_high_command()
{
    /// Boss
    new boss_high_command_svirax();
    new boss_high_command_ishkar();
    new boss_high_command_erodus();

    /// Svirax
    new npc_high_command_admirals_pod();
    new npc_high_command_legion_cruiser();
    new spell_svirax_fusillade();

    /// Ishkar
    new npc_high_command_engineers_pod();
    new npc_high_command_entropic_mine();
    new spell_antoran_high_command_felshield();

    /// Erodus
    new npc_high_command_generals_pod();
    new npc_high_command_fanatical_pyromancer();
    new npc_high_command_felblade_shocktrooper();
    new npc_high_command_disruptor_beacon();

    /// Common
    new spell_antoran_high_command_enter_pod();
    new spell_antoran_high_command_chaos_pulse_pod();
    new spell_antoran_high_command_shock_grenade();
    new spell_antoran_high_command_defensive_countermeasures();
    new npc_high_command_screaming_shrike();
    new spell_antoran_high_command_psychic_assault();
}
#endif
