/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "vault_of_the_wardens.hpp"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_LINGERING       = 1,
    SAY_FOCUS           = 2,
    SAY_HITS_BY_PULSE   = 3,
    SAY_PULSE           = 4,
    SAY_WARN_FOCUS      = 5,
    SAY_DEATH           = 6,
    SAY_SLAY            = 7,
    SAY_WIPE            = 8
};

enum Spells
{
    SPELL_PULSE                         = 194849,
    SPELL_PULSE_AT                      = 194853,
    SPELL_PULSE_SMALL_AT                = 213286,
    SPELL_PULSE_DAMAGE                  = 194880,
    SPELL_LINGERING_GAZE                = 194942,
    SPELL_LINGERING_GAZE_DUMMY_1        = 201395,
    SPELL_LINGERING_GAZE_DUMMY_2        = 201396,
    SPELL_LINGERING_GAZE_DOT            = 194945,
    SPELL_TELEPORT                      = 194149,
    SPELL_ENERGIZE                      = 193443,
    SPELL_FOCUSED                       = 194289,
    SPELL_FOCUSED_AURA                  = 194323,
    SPELL_BEAM_VIS_1                    = 194261,
    SPELL_BEAM_VIS_2                    = 201042,

    /// Lens
    SPELL_ACTIVATE_GO                   = 197151,
    SPELL_DEACTIVATE_GO                 = 198165,
    SPELL_LENS_SPAWN_VIS                = 203926, ///< Visual spawn
    SPELL_BEAM_VIS_3                    = 193373,
    SPELL_BEAM_VIS_4                    = 194333, ///< Target boss
    SPELL_BEAM_DMG                      = 202046,
    SPELL_MIRROR_SPAWN                  = 225816,
    SPELL_OVERLOAD                      = 225817,

    /// Illianna
    SPELL_PIERCE                        = 191524,
    SPELL_DEAFENING_SHOUT               = 191527,
    SPELL_GIFT_OF_THE_DOOMSAYER         = 193164,
    SPELL_GIFT_OF_THE_DOOMSAYER_HEAL    = 193176,
    SPELL_REKINDLE                      = 225814,
    SPELL_ENKINDLE                      = 225815,

    /// Safety Net
    SPELL_SAFETY_NET_SEARCH             = 203801,
    SPELL_SAFETY_NET_STRANGULATE        = 203806,
    SPELL_SAFETY_NET_TELEPORT           = 203805
};

enum eEvents
{
    EVENT_PULSE             = 1,
    EVENT_LINGERING_GAZE    = 2,
    EVENT_ENERGIZE          = 3,
    EVENT_FOCUSED           = 4
};

enum eBossData
{
    DATA_WEST,
    DATA_EAST,
    DATA_MAX_POS,

    DATA_MAX_LENS = 5
};

static std::array<Position const, eBossData::DATA_MAX_POS> g_StartBeamPos
{
    {
        { 4451.21f, -623.264f, 117.2333f, 0.0f },
        { 4510.05f, -676.140f, 117.2333f, 0.0f }
    }
};

static std::array<Position const, eBossData::DATA_MAX_POS> g_EndBeamPos
{
    {
        { 4489.994f, -648.8089f, 117.2333f, 0.0f },
        { 4475.297f, -630.6860f, 117.2333f, 0.0f }
    }
};

static std::array<Position const, eBossData::DATA_MAX_POS> g_TeleportPos
{
    {
        { 4442.56f, -664.622f, 117.15f, 2.342442f },
        { 4459.94f, -681.876f, 117.15f, 5.497787f }
    }
};

static std::array<Position const, eBossData::DATA_MAX_POS> g_OverloadedLensPos
{
    {
        { 4451.21f, -690.556f, 117.2333f, 4.712389f },
        { 4433.94f, -673.131f, 117.2333f, 3.926990f }
    }
};

static std::array<std::array<Position, eBossData::DATA_MAX_LENS>, eBossData::DATA_MAX_POS> g_LensPos =
{
    {
        {
            {
                { 4416.63f, -673.264f, 117.2333f, 5.497787f },
                { 4425.27f, -664.622f, 117.2333f, 0.000000f },
                { 4433.92f, -690.556f, 117.2333f, 2.356194f },
                { 4468.50f, -655.976f, 117.2333f, 3.141593f },
                { 4468.50f, -690.556f, 117.2333f, 3.141593f }
            }
        },
        {
            {
                { 4468.50f, -655.976f, 117.2333f, 3.141593f },
                { 4459.90f, -699.205f, 117.2333f, 1.570796f },
                { 4451.12f, -707.897f, 117.2333f, 3.926990f },
                { 4434.12f, -690.574f, 117.2333f, 0.785397f },
                { 4433.97f, -655.907f, 117.2333f, 4.712389f }
            }
        }
    }
};

Position const g_CenterPos = { 4450.98f, -673.278f, 117.235f, 0.0f };

/// Glazer - 95887
class boss_glazer : public CreatureScript
{
    public:
        boss_glazer() : CreatureScript("boss_glazer") { }

        struct boss_glazerAI : public BossAI
        {
            boss_glazerAI(Creature* p_Creature) : BossAI(p_Creature, DATA_GLAZER) { }

            uint32 m_LastTalkTimer;

            std::set<uint64> m_HitsByPulseGuids;

            int8 m_TeleportIDx;

            void Reset() override
            {
                me->SetHover(true);
                _Reset();
                me->SetReactState(REACT_DEFENSIVE);
                me->RemoveAllAreasTrigger();
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                m_LastTalkTimer = 0;

                m_HitsByPulseGuids.clear();

                m_TeleportIDx = -1;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoTalk(SAY_AGGRO, false);
                _EnterCombat();

                events.ScheduleEvent(EVENT_PULSE, 6000);
                events.ScheduleEvent(EVENT_LINGERING_GAZE, 12000);
                events.ScheduleEvent(EVENT_ENERGIZE, 30000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoTalk(SAY_DEATH, false);
                _JustDied();
                me->RemoveAllAreasTrigger();

                RemoveEncounterAuras();
            }

            void EnterEvadeMode() override
            {
                me->InterruptNonMeleeSpells(true);

                DoTalk(SAY_WIPE);

                CreatureAI::EnterEvadeMode();

                RemoveEncounterAuras();
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    DoTalk(SAY_SLAY, false);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_TELEPORT:
                    {
                        me->GetMotionMaster()->Clear();
                        DoCast(me, SPELL_FOCUSED_AURA, true);
                        events.ScheduleEvent(EVENT_FOCUSED, 100);
                        break;
                    }
                    case SPELL_ENERGIZE:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            m_TeleportIDx = urand(0, 1);
                            DoCast(g_TeleportPos[m_TeleportIDx], SPELL_TELEPORT, true);
                        });

                        break;
                    }
                    case SPELL_BEAM_VIS_4:
                    {
                        EndBeamPhase();
                        break;
                    }
                    case SPELL_PULSE_DAMAGE:
                    {
                        DoTalk(SAY_HITS_BY_PULSE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_PULSE:
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            Position l_Pos      = me->GetPosition();
                            l_Pos.m_orientation = frand(0.0f, 2.0f * M_PI);

                            uint8 l_Count = IsMythic() ? 4 : (IsHeroic() ? 3 : 2);
                            float l_OStep = M_PI / 2.0f;

                            for (uint8 l_I = 0; l_I < l_Count; ++l_I)
                            {
                                DoCast(l_Pos, SPELL_PULSE_AT, true);

                                l_Pos.m_orientation = Position::NormalizeOrientation(l_Pos.m_orientation + l_OStep);
                            }
                        }

                        break;
                    }
                    case SPELL_FOCUSED:
                    {
                        DoCast(p_Target, SPELL_BEAM_VIS_2, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Attacker->GetGUID() != me->GetGUID())
                    return;

                /// Prevent Glazer to kills itself in case of very low hp
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;

                    /// Force victim to kill the boss - should fix encounter completion issues
                    if (Player* l_Tank = SelectMainTank())
                        l_Tank->Kill(me);
                }
            }

            void SummonedCreatureDespawn(Creature* p_Summon) override
            {
                summons.Despawn(p_Summon);

                p_Summon->CastSpell(p_Summon, SPELL_DEACTIVATE_GO, true);

                if (p_Summon->GetEntry() == NPC_LENS || p_Summon->GetEntry() == NPC_OVERLOADED_LENS)
                {
                    if (GameObject* l_Tile = p_Summon->FindNearestGameObject(10.0f))
                        l_Tile->SetGoState(GO_STATE_READY);
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool p_Removed) override
            {
                /// p_Removed = true if despawned by action
                if (!p_Removed)
                    return;

                switch (p_AreaTrigger->GetSpellId())
                {
                    case SPELL_PULSE_AT:
                    {
                        if (!IsHeroicOrMythic())
                            break;

                        if (m_HitsByPulseGuids.find(p_AreaTrigger->GetGUID()) != m_HitsByPulseGuids.end())
                        {
                            m_HitsByPulseGuids.erase(p_AreaTrigger->GetGUID());
                            break;
                        }

                        Position l_Pos      = p_AreaTrigger->GetPosition();
                        l_Pos.m_orientation = frand(0.0f, 2.0f * M_PI);

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            DoCast(l_Pos, SPELL_PULSE_SMALL_AT, true);

                            l_Pos.m_orientation = Position::NormalizeOrientation(l_Pos.m_orientation + M_PI / 2.0f);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == SPELL_PULSE_AT)
                    p_AreaTrigger->SetRadius(3.5f);
            }

            bool CheckAreaTriggerCastAction(AreaTrigger* p_AreaTrigger, Unit* p_Unit, SpellInfo const* p_SpellInfo) override
            {
                if (p_Unit == nullptr || p_SpellInfo == nullptr)
                    return false;

                if (p_SpellInfo->Id == SPELL_PULSE_DAMAGE && p_Unit->GetEntry() == me->GetEntry())
                    m_HitsByPulseGuids.insert(p_AreaTrigger->GetGUID());

                return true;
            }

            uint32 GetData(uint32 p_ID) override
            {
                return m_TeleportIDx;
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
                    case EVENT_PULSE:
                    {
                        Talk(SAY_PULSE);
                        DoCast(SPELL_PULSE);
                        events.ScheduleEvent(EVENT_PULSE, 12000);
                        break;
                    }
                    case EVENT_LINGERING_GAZE:
                    {
                        Talk(SAY_LINGERING, false);

                        if (Player* l_Target = SelectRangedTarget())
                        {
                            Position l_Pos = l_Target->GetPosition();
                            DoCast(l_Pos, SPELL_LINGERING_GAZE);

                            AddTimedDelayedOperation(1200, [this, l_Pos]() -> void
                            {
                                DoCast(l_Pos, SPELL_LINGERING_GAZE_DUMMY_1, true);

                                AddTimedDelayedOperation(300, [this, l_Pos]() -> void
                                {
                                    DoCast(l_Pos, SPELL_LINGERING_GAZE_DUMMY_2, true);
                                });
                            });
                        }

                        events.ScheduleEvent(EVENT_LINGERING_GAZE, 18000);
                        break;
                    }
                    case EVENT_ENERGIZE:
                    {
                        DoTalk(SAY_FOCUS, false);
                        DoTalk(SAY_WARN_FOCUS, false);
                        me->GetMotionMaster()->Clear();
                        me->SetReactState(REACT_PASSIVE);
                        me->SetTarget(0);
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        DoCast(SPELL_ENERGIZE);
                        break;
                    }
                    case EVENT_FOCUSED:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            for (Position const l_Pos : g_LensPos[m_TeleportIDx])
                                me->SummonCreature(NPC_LENS, l_Pos);

                            if (IsMythic())
                                me->SummonCreature(NPC_OVERLOADED_LENS, g_OverloadedLensPos[m_TeleportIDx]);

                            if (Creature* l_Beam = me->SummonCreature(NPC_GLAZER_BEAM, g_StartBeamPos[m_TeleportIDx]))
                                me->SetFacingToObject(l_Beam);

                            DoCast(SPELL_FOCUSED);
                        });

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void DoTalk(uint8 p_TextID, bool p_CheckTimer = true)
            {
                if (m_LastTalkTimer && p_CheckTimer && time(nullptr) < (m_LastTalkTimer + 10))
                    return;

                m_LastTalkTimer = time(nullptr);
                Talk(p_TextID);
            }

            void RemoveEncounterAuras()
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LINGERING_GAZE_DOT);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BEAM_DMG);
                }
            }

            void EndBeamPhase(bool p_Finished = true)
            {
                if (p_Finished)
                    Talk(SAY_HITS_BY_PULSE);

                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->InterruptNonMeleeSpells(false, SPELL_BEAM_VIS_2);
                me->RemoveAurasDueToSpell(SPELL_FOCUSED_AURA);
                me->SetReactState(REACT_AGGRESSIVE);

                EntryCheckPredicate l_Pred(NPC_LENS);
                summons.DoAction(true, l_Pred);

                EntryCheckPredicate l_Pred1(NPC_GLAZER_BEAM);
                summons.DoAction(true, l_Pred1);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetData(DATA_GLAZER_SYSTEM, 0);

                events.ScheduleEvent(EVENT_ENERGIZE, 55000);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Unit* l_Target = me->getVictim())
                    {
                        AttackStart(l_Target);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(l_Target);
                    }
                });

            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_glazerAI(p_Creature);
        }
};

/// Beam - 102310
class npc_glazer_beam : public CreatureScript
{
    public:
        npc_glazer_beam() : CreatureScript("npc_glazer_beam") { }

        struct npc_glazer_beamAI : public ScriptedAI
        {
            npc_glazer_beamAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(11686);
                m_MoveComplete = false;
            }

            bool m_MoveComplete;

            void Reset() override
            {
                SetFlyMode(true);

                me->SetDisableGravity(true);
                me->SetCollision(true);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (m_MoveComplete)
                    return;

                if (p_SpellInfo->Id == SPELL_FOCUSED)
                {
                    m_MoveComplete = true;

                    int32 l_IDx = -1;
                    if (Unit* l_Owner = me->GetAnyOwner())
                    {
                        if (l_Owner->ToCreature() && l_Owner->IsAIEnabled)
                            l_IDx = l_Owner->ToCreature()->AI()->GetData(0);
                    }

                    if (l_IDx < 0)
                        return;

                    me->GetMotionMaster()->MovePoint(1, g_EndBeamPos[l_IDx]);
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                me->DespawnOrUnsummon();
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (me->GetAnyOwner())
                {
                    if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                    {
                        l_Summoner->InterruptNonMeleeSpells(false, SPELL_FOCUSED);
                        l_Summoner->InterruptNonMeleeSpells(false, SPELL_BEAM_VIS_2);

                        if (Creature* l_Lens = me->FindNearestCreature(NPC_LENS, 30.0f))
                        {
                            l_Summoner->CastSpell(l_Lens, SPELL_BEAM_VIS_2, true);
                            l_Summoner->SetFacingToObject(l_Lens);
                        }
                    }
                }

                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (Unit* l_Summoner = me->GetAnyOwner())
                    l_Summoner->SetFacingToObject(me);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_glazer_beamAI(p_Creature);
        }
};

/// Lens - 98082
class npc_glazer_lens : public CreatureScript
{
    public:
        npc_glazer_lens() : CreatureScript("npc_glazer_lens") { }

        struct npc_glazer_lensAI : public ScriptedAI
        {
            npc_glazer_lensAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(11686);

                m_LastClickTimer = 0;
            }

            uint64 m_TargetGuid;

            uint32 m_LastClickTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                events.ScheduleEvent(EVENT_1, 500);
                events.ScheduleEvent(EVENT_2, 1000);
                events.ScheduleEvent(EVENT_3, 3000);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                me->InterruptNonMeleeSpells(false, SPELL_BEAM_VIS_3);
                me->InterruptNonMeleeSpells(false, SPELL_BEAM_VIS_4);
                me->RemoveAurasDueToSpell(SPELL_LENS_SPAWN_VIS);
                me->DespawnOrUnsummon(2000);
            }

            void OnSpellClick(Unit* /*p_Clicker*/) override
            {
                if (m_LastClickTimer && time(nullptr) < (m_LastClickTimer + 1))
                    return;

                m_LastClickTimer = time(nullptr);

                me->SetFacingTo(Position::NormalizeOrientation(me->m_orientation + (M_PI / 2.0f)));
            }

            void SearchBeamTarget()
            {
                std::list<Creature*> l_CreatureList;
                std::list<Creature*> l_ValidTargets;

                me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_BEAM_TARGET_TRIGGER, 100.0f);
                me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_LENS, 100.0f);
                me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_OVERLOADED_LENS, 100.0f);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Glazer = Creature::GetCreature(*me, l_Instance->GetData64(NPC_GLAZER)))
                        l_CreatureList.push_back(l_Glazer);
                }

                for (Creature* l_Creature : l_CreatureList)
                {
                    float l_Angle = me->GetAngle(l_Creature);

                    if (me->GetOrientation() >= (l_Angle - 0.05f) && me->GetOrientation() <= (l_Angle + 0.05f))
                    {
                        if (l_Creature->HasAura(SPELL_BEAM_VIS_2) || l_Creature->HasAura(SPELL_BEAM_VIS_3) || l_Creature->HasAura(SPELL_BEAM_VIS_4))
                        {
                            if (l_Creature->GetEntry() == NPC_LENS && me->HasAura(SPELL_BEAM_VIS_3, l_Creature->GetGUID()))
                            {
                                me->InterruptNonMeleeSpells(false, SPELL_BEAM_VIS_3);
                                return;
                            }
                        }

                        if (!l_Creature->isAlive() || l_Creature == me)
                            continue;

                        l_ValidTargets.push_back(l_Creature);
                    }
                }

                if (!l_ValidTargets.empty())
                {
                    l_ValidTargets.sort(JadeCore::ObjectDistanceOrderPred(me));

                    m_TargetGuid = l_ValidTargets.front()->GetGUID();
                }

                if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                {
                    if (l_Target->GetEntry() != NPC_GLAZER)
                    {
                        if (!l_Target->HasAura(SPELL_BEAM_VIS_3, me->GetGUID()))
                            DoCast(l_Target, SPELL_BEAM_VIS_3, true);
                    }
                    else
                    {
                        if (!l_Target->HasAura(SPELL_BEAM_VIS_4, me->GetGUID()))
                            DoCast(l_Target, SPELL_BEAM_VIS_4, true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_ACTIVATE_GO, true);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_LENS_SPAWN_VIS, true);
                        break;
                    }
                    case EVENT_3:
                    {
                        if (me->HasAura(SPELL_BEAM_VIS_2) || me->HasAura(SPELL_BEAM_VIS_3) || me->HasAura(SPELL_BEAM_VIS_4))
                            SearchBeamTarget();
                        events.ScheduleEvent(EVENT_3, 300);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_glazer_lensAI(p_Creature);
        }
};

/// Overloaded Lens - 113552
class npc_glazer_overloaded_lens : public CreatureScript
{
    public:
        npc_glazer_overloaded_lens() : CreatureScript("npc_glazer_overloaded_lens") { }

        struct npc_glazer_overloaded_lensAI : public Scripted_NoMovementAI
        {
            npc_glazer_overloaded_lensAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_MIRROR_SPAWN);

                AddTimedDelayedOperation(500, [this]() -> void
                {
                    DoCast(me, SPELL_LENS_SPAWN_VIS, true);
                });
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_MIRROR_SPAWN)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                else if (p_SpellInfo->Id == SPELL_BEAM_VIS_3)
                    events.ScheduleEvent(EVENT_1, 1);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnOrUnsummon(2000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == EVENT_1)
                {
                    DoCast(me, SPELL_OVERLOAD);
                    events.ScheduleEvent(EVENT_1, 1000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_glazer_overloaded_lensAI(p_Creature);
        }
};

/// Blade Dancer Illianna - 96657
class npc_glazer_blade_dancer_illianna : public CreatureScript
{
    public:
        npc_glazer_blade_dancer_illianna() : CreatureScript("npc_glazer_blade_dancer_illianna") { }

        struct npc_glazer_blade_dancer_illiannaAI : public ScriptedAI
        {
            npc_glazer_blade_dancer_illiannaAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::set<uint64> m_GuardianGuids;

            void Reset() override
            {
                AddTimedDelayedOperation(2000, [this]() -> void
                {
                    std::list<Creature*> l_Trashes;

                    me->GetCreatureListWithEntryInGrid(l_Trashes, NPC_BLADE_DANCER_PET_1, 15.0f);
                    me->GetCreatureListWithEntryInGrid(l_Trashes, NPC_BLADE_DANCER_PET_2, 15.0f);

                    for (Creature* l_Iter : l_Trashes)
                        m_GuardianGuids.insert(l_Iter->GetGUID());
                });

                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->SetWalk(false);

                events.ScheduleEvent(EVENT_1, 9000);
                events.ScheduleEvent(EVENT_2, 15000);
                events.ScheduleEvent(EVENT_3, 20000);
                events.ScheduleEvent(EVENT_4, 20000);
                events.ScheduleEvent(EVENT_5, 37000);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                AddTimedDelayedOperation(1000, [this]() -> void
                {
                    Position l_Pos = me->GetPosition();

                    l_Pos.m_positionX += 30.0f * std::cos(l_Pos.m_orientation);
                    l_Pos.m_positionY += 30.0f * std::sin(l_Pos.m_orientation);

                    me->SetWalk(true);

                    me->SetHomePosition(l_Pos);
                    me->NearTeleportTo(l_Pos);

                    for (uint64 l_Guid : m_GuardianGuids)
                    {
                        if (Creature* l_Guardian = Creature::GetCreature(*me, l_Guid))
                        {
                            l_Pos = l_Guardian->GetPosition();

                            l_Pos.m_positionX += 30.0f * std::cos(l_Pos.m_orientation);
                            l_Pos.m_positionY += 30.0f * std::sin(l_Pos.m_orientation);

                            l_Guardian->SetHomePosition(l_Pos);
                            l_Guardian->NearTeleportTo(l_Pos);
                        }
                    }
                });
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
                    case EVENT_1:
                    {
                        DoCastVictim(SPELL_PIERCE);
                        events.ScheduleEvent(EVENT_1, 15000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_DEAFENING_SHOUT);
                        events.ScheduleEvent(EVENT_2, 19000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_GIFT_OF_THE_DOOMSAYER);
                        events.ScheduleEvent(EVENT_3, 20000);
                        break;
                    }
                    case EVENT_4:
                    {
                        Talk(0, me->GetGUID());
                        DoCast(SPELL_REKINDLE);
                        events.ScheduleEvent(EVENT_4, 20000);
                        break;
                    }
                    case EVENT_5:
                    {
                        Talk(1, me->GetGUID());
                        DoCast(SPELL_ENKINDLE);
                        events.ScheduleEvent(EVENT_5, 37000);
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
            return new npc_glazer_blade_dancer_illiannaAI(p_Creature);
        }
};

/// Safety Net - 103093
class npc_glazer_safety_net : public CreatureScript
{
    public:
        npc_glazer_safety_net() : CreatureScript("npc_glazer_safety_net") { }

        struct npc_glazer_safety_netAI : public ScriptedAI
        {
            npc_glazer_safety_netAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_1, 500);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SAFETY_NET_SEARCH)
                    DoCast(p_Caster, SPELL_SAFETY_NET_STRANGULATE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (events.ExecuteEvent() == EVENT_1)
                {
                    std::list<Player*> l_PlayerList;
                    me->GetPlayerListInGrid(l_PlayerList, 50.0f);

                    for (Player* l_Player : l_PlayerList)
                    {
                        if (l_Player->m_positionZ >= 110.0f)
                            continue;

                        /// Don't handle the same player twice
                        if (l_Player->HasAura(SPELL_SAFETY_NET_STRANGULATE))
                            continue;

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Glazer = Creature::GetCreature(*me, l_Instance->GetData64(NPC_GLAZER)))
                                l_Glazer->getThreatManager().modifyThreatPercent(l_Player, -99);
                        }

                        l_Player->SetDisableGravity(true);
                        l_Player->CastSpell(l_Player, SPELL_SAFETY_NET_SEARCH, true);
                    }

                    events.ScheduleEvent(EVENT_1, 500);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_glazer_safety_netAI(p_Creature);
        }
};

/// Radiation - 195032
class spell_glazer_radiation : public SpellScriptLoader
{
    public:
        spell_glazer_radiation() :  SpellScriptLoader("spell_glazer_radiation") { }

        class spell_glazer_radiation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_glazer_radiation_SpellScript);

            uint8 m_Stack = 0;
            int32 m_Damage = 0;

            void RecalculateDamage()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(195034))
                    m_Stack = l_Aura->GetStackAmount();

                m_Damage = CalculatePct(GetHitDamage(), m_Stack);
                SetHitDamage(GetHitDamage() + m_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_glazer_radiation_SpellScript::RecalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_glazer_radiation_SpellScript();
        }
};

/// Gift of the Doomsayer - 193164
class spell_glazer_gift_of_the_doomsayer : public SpellScriptLoader
{
    public:
        spell_glazer_gift_of_the_doomsayer() : SpellScriptLoader("spell_glazer_gift_of_the_doomsayer") { }

        class spell_glazer_gift_of_the_doomsayer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_glazer_gift_of_the_doomsayer_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Actor = p_ProcInfo.GetActor();

                if (l_Caster == nullptr || l_Actor == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (l_DamageInfo == nullptr || l_DamageInfo->GetSpellInfo() == nullptr)
                    return;

                if (!l_DamageInfo->GetSpellInfo()->IsPositive())
                    return;

                int32 l_Bp = l_DamageInfo->GetAmount();

                l_Caster->CastCustomSpell(l_Caster, SPELL_GIFT_OF_THE_DOOMSAYER_HEAL, &l_Bp, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_glazer_gift_of_the_doomsayer_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_glazer_gift_of_the_doomsayer_AuraScript();
        }
};

/// Rekindle - 225814
class spell_glazer_rekindle : public SpellScriptLoader
{
    public:
        spell_glazer_rekindle() : SpellScriptLoader("spell_glazer_rekindle") { }

        class spell_glazer_rekindle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_glazer_rekindle_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPred());

                WorldObject* l_Object = p_Targets.front();

                p_Targets.clear();

                p_Targets.push_back(l_Object);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_glazer_rekindle_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_glazer_rekindle_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_glazer_rekindle_SpellScript();
        }
};

/// Safety Net - 203806
class spell_glazer_safety_net : public SpellScriptLoader
{
    public:
        spell_glazer_safety_net() : SpellScriptLoader("spell_glazer_safety_net") { }

        class spell_glazer_safety_net_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_glazer_safety_net_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->SetDisableGravity(false);

                    Position l_Pos = g_CenterPos;
                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 30.0f, l_Pos.GetAngle(l_Target));

                    l_Target->CastSpell(l_Pos, SPELL_SAFETY_NET_TELEPORT, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_glazer_safety_net_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_STRANGULATE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_glazer_safety_net_AuraScript();
        }
};

/// Beam - 193373
/// Beam - 201042
class spell_glazer_beams : public SpellScriptLoader
{
    public:
        spell_glazer_beams() : SpellScriptLoader("spell_glazer_beams") { }

        class spell_glazer_beams_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_glazer_beams_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                /// Casting Lens
                if (Unit* l_Caster = GetCaster())
                {
                    InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                    if (l_Instance == nullptr)
                        return;

                    if (Creature* l_Glazer = Creature::GetCreature(*l_Caster, l_Instance->GetData64(NPC_GLAZER)))
                    {
                        /// Targeted Lens
                        if (Unit* l_Target = GetTarget())
                        {
                            std::list<Player*> l_PlayerList;
                            l_Caster->GetPlayerListInGrid(l_PlayerList, 100.0f);

                            if (l_PlayerList.empty())
                                return;

                            l_PlayerList.remove_if([this, l_Caster, l_Target, l_Glazer](Player* p_Player) -> bool
                            {
                                /// Not attackable
                                if (!l_Glazer->IsValidAttackTarget(p_Player))
                                {
                                    p_Player->RemoveAura(SPELL_BEAM_DMG, l_Caster->GetGUID());
                                    return true;
                                }

                                /// Not in the beam line
                                float l_Width = GetId() == SPELL_BEAM_VIS_3 ? 1.15f : 0.5f;

                                if (!p_Player->IsInAxe(l_Caster, l_Width, 100.f))
                                {
                                    p_Player->RemoveAura(SPELL_BEAM_DMG, l_Caster->GetGUID());
                                    return true;
                                }

                                /// Not between the two Lens
                                if (l_Caster->GetDistance(p_Player) > l_Caster->GetDistance(l_Target))
                                {
                                    p_Player->RemoveAura(SPELL_BEAM_DMG, l_Caster->GetGUID());
                                    return true;
                                }

                                return false;
                            });

                            for (Player* l_Player : l_PlayerList)
                                l_Caster->CastSpell(l_Player, SPELL_BEAM_DMG, true, nullptr, nullptr, l_Glazer->GetGUID());
                        }
                    }
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasAura(SPELL_BEAM_VIS_2) && !l_Target->HasAura(SPELL_BEAM_VIS_3))
                        l_Target->InterruptNonMeleeSpells(false, SPELL_BEAM_VIS_3);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_glazer_beams_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_glazer_beams_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_glazer_beams_AuraScript();
        }
};

/// Pulse - 194853
/// Pulse (small) - 213286
class areatrigger_glazer_pulse : public AreaTriggerEntityScript
{
    public:
        areatrigger_glazer_pulse() : AreaTriggerEntityScript("areatrigger_glazer_pulse") { }

        void OnSetCreatePosition(AreaTrigger* p_AreaTrigger, Unit* /*p_Caster*/, Position& /*p_SourcePosition*/, Position& /*p_DestinationPosition*/, std::vector<G3D::Vector3>& /*p_PathToLinearDestination*/) override
        {
            p_AreaTrigger->SetBounceRef(g_CenterPos);
        }

        bool OnAreaTriggerBounce(AreaTrigger* p_AreaTrigger) override
        {
            if (Player* l_Target = p_AreaTrigger->FindNearestPlayer(100.0f))
                p_AreaTrigger->BounceAreaTrigger(p_AreaTrigger->GetAngle(l_Target), p_AreaTrigger->GetDistance(l_Target));

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_glazer_pulse();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_glazer()
{
    /// Boss
    new boss_glazer();

    /// Creatures
    new npc_glazer_beam();
    new npc_glazer_lens();
    new npc_glazer_overloaded_lens();
    new npc_glazer_blade_dancer_illianna();
    new npc_glazer_safety_net();

    /// Spells
    new spell_glazer_radiation();
    new spell_glazer_gift_of_the_doomsayer();
    new spell_glazer_rekindle();
    new spell_glazer_safety_net();
    new spell_glazer_beams();

    /// AreaTrigger
    new areatrigger_glazer_pulse();
}
#endif
