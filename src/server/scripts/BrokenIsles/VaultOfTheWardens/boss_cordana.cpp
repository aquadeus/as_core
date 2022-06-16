////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "vault_of_the_wardens.hpp"
#include "G3D/Rect2D.h"

enum Says
{
    SAY_AGGRO,
    SAY_DEEPENING_SHADOWS,
    SAY_FEL_GLAIVE,
    SAY_TURN_KICK,
    SAY_SHADOWSTEP,
    SAY_SHADOWSTEP_WARN,
    SAY_CORDANA_FOUND,
    SAY_CREEPING_DOOM,
    SAY_CREEPING_DOOM_WARN,
    SAY_CONTINUE_CREEPING_DOOM,
    SAY_SLAY,
    SAY_WIPE,
    SAY_DEATH
};

enum Spells
{
    /// Elune's Light
    SPELL_ELUNES_LIGHT                  = 201359,
    SPELL_ELUNES_LIGHT_ORB_VISUAL       = 197897,
    SPELL_ELUNES_LIGHT_HALO_VISUAL      = 204294,
    SPELL_ELUNES_LIGHT_AT_1             = 197941,
    SPELL_ELUNES_LIGHT_AT_2             = 197910,
    SPELL_ELUNES_LIGHT_CLICK            = 197890,
    SPELL_ELUNES_LIGHT_OVERRIDE         = 204485,
    SPELL_ELUNES_LIGHT_ORB_VISUAL_2     = 192656,
    SPELL_ELUNES_LIGHT_DUMMY            = 197894,

    /// Others
    SPELL_CORDANA_INTRO                 = 198309,
    SPELL_VEILED_IN_SHADOW_2            = 204461,
    SPELL_HELP_IM_FALLING               = 197225,

    /// Cordana Felsong
    /// Kick
    SPELL_KNOCKDOWN_KICK                = 197251,
    SPELL_TURN_KICK                     = 197250,
    /// Deepening Shadows
    SPELL_DEEPENING_SHADOWS_CAST        = 213576,
    SPELL_DEEPENING_SHADOWS             = 213411,
    SPELL_DEEPENING_SHADOWS_SPAWN       = 213397,
    /// Detonating Moonglaive
    SPELL_DETONATING_MOONGLAIVE         = 197513,
    /// Shadowstep
    SPELL_SHADOWSTEP                    = 203416,
    SPELL_SHADOWSTEP_2                  = 197823,
    SPELL_FEL_GLAIVE_PERIODIC           = 197333,
    SPELL_FEL_GLAIVE_5_AT               = 197258,
    SPELL_FEL_GLAIVE_4_AT               = 197328,
    /// Avatar of Shadow
    SPELL_STEAL_LIGHT                   = 206387,
    SPELL_STOLEN_LIGHT                  = 206567,
    SPELL_SHADOW_STRIKE_SEARCHER        = 206460,
    SPELL_SHADOW_STRIKE                 = 206459,
    /// Avatar of Vengeance
    SPELL_VENGEANCE_AURA                = 205004,
    SPELL_AVATAR_OF_VENGEANCE_MISSILE   = 204984,
    SPELL_AVATAR_OF_VENGEANCE           = 197438,
    SPELL_SPIRIT_OF_VENGEANCE           = 197442,
    SPELL_VENGEANCE_DAMAGES             = 197454,
    /// Creeping Doom
    SPELL_CREEPING_DOOM_CHANNEL         = 197422,
    SPELL_CREEPING_DOOM_VISUAL          = 213628,
    SPELL_CREEPING_DOOM_AURA            = 197415,
    SPELL_CREEPING_DOOM_OTHERS          = 213685,

    /// Grimhorn
    SPELL_FEL_DISSOLVE_IN               = 211762,
    SPELL_IMPRISON_MISSILE              = 202625,
    SPELL_TORMENT_CHANNEL               = 202634,
    SPELL_JAILERS_CAGE                  = 202621,
    SPELL_ANGUISHED_SOULS_FILTER        = 202609,
    SPELL_ANGUISHED_SOULS_MISSILE       = 202606
};

enum eEvents
{
    EVENT_KNOCKDOWN_KICK        = 1,
    EVENT_DEEPENING_SHADOWS     = 2,
    EVENT_DETONATING_MOONGLAIVE = 3,
    EVENT_SHADOWSTEP            = 4,
    EVENT_VENGEANCE             = 5,
    EVENT_CREEPING_DOOM         = 6
};

enum eCordanaData
{
    DATA_CREEPING_DOOM_POS          = 4,
    DATA_CREEPING_DOOM_ARROW_COUNT  = 12,
    DATA_CREEPING_DOOM_SUM_COUNT    = 23,
    DATA_CREEPING_DOOM_GAP_COUNT    = 2,
    DATA_CREEPING_DOOM_VISUAL       = 56509,
    DATA_MAX_SENTRY                 = 4,

    DATA_WALKWAY_ENTRY              = 10057
};

static std::array<Position const, eCordanaData::DATA_CREEPING_DOOM_POS> g_CreepingDoomStartPos =
{
    {
        { 4015.0f, -333.0f, -280.5f, 2.0f * float(M_PI) },
        { 4015.0f, -263.0f, -280.5f, 3.0f * float(M_PI) / 2.0f },
        { 4085.0f, -333.0f, -280.5f,        float(M_PI) },
        { 4015.0f, -333.0f, -280.5f,        float(M_PI) / 2.0f }
    }
};

Position const g_CenterPos = { 4049.907f, -297.4254f, -281.4167f, 0.0f };

Position const g_WalkwayPos = { 4133.0f, -298.0f, -286.8862f, 0.0f };

static std::array<Position const, eCordanaData::DATA_MAX_SENTRY> g_VengeanceSpawnPos =
{
    {
        { 4026.155f, -321.99f, -281.113f, 0.8f },
        { 4025.848f, -272.77f, -281.113f, 5.5f },
        { 4075.237f, -272.83f, -281.113f, 3.9f },
        { 4075.283f, -321.85f, -281.113f, 2.4f }
    }
};

G3D::Rect2D g_FightArea(G3D::Rect2D::xyxy(g_CenterPos.m_positionX - 30.f, g_CenterPos.m_positionY - 30.f, g_CenterPos.m_positionX + 30.f, g_CenterPos.m_positionY + 30.f));

/// Cordana Felsong - 95888
class boss_cordana : public CreatureScript
{
    public:
        boss_cordana() : CreatureScript("boss_cordana") { }

        struct boss_cordanaAI : public BossAI
        {
            boss_cordanaAI(Creature* p_Creature) : BossAI(p_Creature, DATA_CORDANA)
            {
                m_Intro             = true;
                m_CheckIntroTimer   = 0;
                m_MainSentryGuid    = 0;

                p_Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                p_Creature->SetReactState(REACT_PASSIVE);
            }

            bool m_Intro;
            uint32 m_CheckIntroTimer;

            uint64 m_MainSentryGuid;

            uint8 m_FelGlaiveCount;
            float m_FelGlaiveAngle;

            bool m_AvatarOfVengeance;
            int32 m_VengeancePct;
            int32 m_VengeanceCount;

            bool m_CreepingDoom;
            bool m_FirstCreepingDoom;
            int32 m_CreepingDoomPct;
            int8 m_LastCreepingDoomIDx;

            bool m_CanCheckLight;
            uint32 m_ShadowstepTime;

            int32 m_SafetyCheckTimer = 0;

            void Reset() override
            {
                _Reset();

                if (!m_Intro)
                    m_CheckIntroTimer = 0;
                else
                {
                    DoCast(me, eVotWSpells::SpellVeiledInShadow, true);

                    m_CheckIntroTimer = 2000;
                }

                me->SetReactState(REACT_AGGRESSIVE);

                SetFlyMode(false);

                me->SetAnimTier(0);

                m_FelGlaiveCount = 0;
                m_FelGlaiveAngle = frand(0.0f, 2.0f * M_PI);

                m_AvatarOfVengeance = false;
                m_VengeancePct = IsHeroicOrMythic() ? 25 : 50;
                m_VengeanceCount = 0;

                m_CreepingDoom = false;
                m_FirstCreepingDoom = true;
                m_CreepingDoomPct = 40;
                m_LastCreepingDoomIDx = -1;

                events.StopEvents(false);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();

                events.ScheduleEvent(EVENT_KNOCKDOWN_KICK, 9000);
                events.ScheduleEvent(EVENT_DEEPENING_SHADOWS, 11000);
                events.ScheduleEvent(EVENT_DETONATING_MOONGLAIVE, 16000);
                events.ScheduleEvent(EVENT_SHADOWSTEP, 34000);

                HandleWalkway();

                m_CanCheckLight = false;
                m_ShadowstepTime = 0;

                m_SafetyCheckTimer = 500;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();

                ResetEncounter();

                if (Creature* l_Drelanim = me->SummonCreature(NPC_DRELANIM_WHISPERWIND_2, 4064.80f, -281.94f, -281.42f, 3.9f))
                {
                    if (l_Drelanim->IsAIEnabled)
                        l_Drelanim->AI()->DoAction(0);
                }

                /// Prevent stuck in combat at the end of the dungeon
                if (instance)
                    instance->DoCombatStopOnPlayers();
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(SAY_SLAY);
            }

            void JustReachedHome() override
            {
                Talk(SAY_WIPE);
            }

            void EnterEvadeMode() override
            {
                ClearDelayedOperations();

                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAreasTrigger();

                CreatureAI::EnterEvadeMode();

                ResetEncounter();
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);

                    AddTimedDelayedOperation(10000, [this]() -> void
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_AGGRESSIVE);
                    });
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == NPC_AVATAR_OF_SHADOW)
                {
                    DoCast(me, SPELL_SHADOWSTEP_2, true);
                    DoCast(me, SPELL_FEL_GLAIVE_PERIODIC, true);

                    Talk(SAY_SHADOWSTEP_WARN);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        m_CanCheckLight = true;
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_KNOCKDOWN_KICK:
                    {
                        me->DelayedCastSpell(p_Target, SPELL_TURN_KICK, false, 10);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_DEEPENING_SHADOWS:
                    {
                        me->SummonCreature(NPC_DEEPENING_SHADOW, p_Dest);
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
                    case SPELL_DEEPENING_SHADOWS_CAST:
                    {
                        if (Player* l_Target = SelectRangedTarget())
                            DoCast(l_Target, SPELL_DEEPENING_SHADOWS);

                        break;
                    }
                    case SPELL_CREEPING_DOOM_CHANNEL:
                    {
                        Talk(SAY_CREEPING_DOOM);

                        m_FirstCreepingDoom = false;

                        SetFlyMode(true);

                        me->SetAnimTier(3);

                        Position l_Pos = me->GetPosition();
                        l_Pos.m_positionZ += 5.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveTakeoff(0, l_Pos);
                        break;
                    }
                    case SPELL_CREEPING_DOOM_OTHERS:
                    {
                        Talk(SAY_CONTINUE_CREEPING_DOOM);

                        /// Others Creeping Doom aren't channeled, there is a 2s cast time
                        /// After what Cordana will spawn two rows of Ghosts travelling the room and continues fighting
                        DoAction(2);
                        DoAction(2);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case SPELL_FEL_GLAIVE_4_AT:
                    case SPELL_FEL_GLAIVE_5_AT:
                    {
                        Position l_Pos = me->GetPosition();

                        if (m_FelGlaiveCount < 2)
                            l_Pos.m_orientation = Position::NormalizeOrientation(m_FelGlaiveAngle + frand(-(M_PI / 4.0f), (M_PI / 4.0f)));
                        else if (m_FelGlaiveCount < 4)
                            l_Pos.m_orientation = Position::NormalizeOrientation(m_FelGlaiveAngle + (M_PI / 2.0f) + frand(-(M_PI / 4.0f), (M_PI / 4.0f)));
                        else if (m_FelGlaiveCount < 6)
                            l_Pos.m_orientation = Position::NormalizeOrientation(m_FelGlaiveAngle + M_PI + frand(-(M_PI / 4.0f), (M_PI / 4.0f)));
                        else if (m_FelGlaiveCount < 8)
                            l_Pos.m_orientation = Position::NormalizeOrientation(m_FelGlaiveAngle + (3.0f * M_PI / 2.0f) + frand(-(M_PI / 4.0f), (M_PI / 4.0f)));
                        else
                            l_Pos.m_orientation = frand(0.0f, 2.0f * M_PI);

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 100.0f, l_Pos.m_orientation);
                        p_AreaTrigger->MoveAreaTrigger(l_Pos, 10000);

                        ++m_FelGlaiveCount;
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_AvatarOfVengeance && me->HealthBelowPctDamaged(m_VengeancePct, p_Damage))
                {
                    m_AvatarOfVengeance = true;
                    events.ScheduleEvent(EVENT_VENGEANCE, 1);
                }
                else if (IsHeroicOrMythic() && !m_CreepingDoom && me->HealthBelowPctDamaged(m_CreepingDoomPct, p_Damage))
                {
                    m_CreepingDoom = true;
                    events.ScheduleEvent(EVENT_CREEPING_DOOM, 1);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                    {
                        EndVeiledInShadow();
                        break;
                    }
                    case 1:
                    {
                        m_FelGlaiveCount = 0;
                        m_FelGlaiveAngle = frand(0.0f, 2.0f * M_PI);
                        break;
                    }
                    case 2:
                    {
                        if (m_LastCreepingDoomIDx <= -1 || m_LastCreepingDoomIDx >= eCordanaData::DATA_CREEPING_DOOM_POS)
                            m_LastCreepingDoomIDx = urand(0, 3);
                        else
                        {
                            std::set<uint8> l_IDs = { 0, 1, 2, 3 };

                            l_IDs.erase(m_LastCreepingDoomIDx);

                            JadeCore::RandomResizeList(l_IDs, 1);

                            m_LastCreepingDoomIDx = (*l_IDs.begin());
                        }

                        Position l_StartPos = g_CreepingDoomStartPos[m_LastCreepingDoomIDx];

                        uint8 l_DisabledID1 = urand(1, 9);
                        uint8 l_DisabledID2 = l_DisabledID1 + 1;

                        for (uint8 l_I = 0; l_I < eCordanaData::DATA_CREEPING_DOOM_ARROW_COUNT; ++l_I)
                        {
                            Position l_Pos = l_StartPos;

                            if (!(m_LastCreepingDoomIDx % 2))
                                l_StartPos.m_positionY += 6.0f;
                            else
                                l_StartPos.m_positionX += 6.0f;

                            if (l_I == l_DisabledID1 || l_I == l_DisabledID2)
                                continue;

                            me->PlayOrphanSpellVisual(me->GetPosition().AsVector3(), G3D::Vector3(), l_Pos.AsVector3(), eCordanaData::DATA_CREEPING_DOOM_VISUAL, 0.5f);
                        }

                        l_StartPos = g_CreepingDoomStartPos[m_LastCreepingDoomIDx];

                        float l_DisabledMinX = l_StartPos.m_positionX;
                        float l_DisabledMinY = l_StartPos.m_positionY;

                        if (!(m_LastCreepingDoomIDx % 2))
                            l_DisabledMinY += (6.0f * l_DisabledID1);
                        else
                            l_DisabledMinX += (6.0f * l_DisabledID1);

                        float l_DisabledMaxX = l_DisabledMinX + (6.0f * (l_DisabledID2 - l_DisabledID1));
                        float l_DisabledMaxY = l_DisabledMinY + (6.0f * (l_DisabledID2 - l_DisabledID1));

                        for (uint8 l_I = 0; l_I < eCordanaData::DATA_CREEPING_DOOM_SUM_COUNT; ++l_I)
                        {
                            Position l_Pos = l_StartPos;

                            if (!(m_LastCreepingDoomIDx % 2))
                            {
                                l_StartPos.m_positionY += 3.0f;

                                if (l_Pos.m_positionY >= l_DisabledMinY && l_Pos.m_positionY <= l_DisabledMaxY)
                                    continue;
                            }
                            else
                            {
                                l_StartPos.m_positionX += 3.0f;

                                if (l_Pos.m_positionX >= l_DisabledMinX && l_Pos.m_positionX <= l_DisabledMaxX)
                                    continue;
                            }

                            AddTimedDelayedOperation(10, [this, l_Pos]() -> void
                            {
                                me->SummonCreature(NPC_CREEPING_DOOM, l_Pos);
                            });
                        }

                        break;
                    }
                    case 3:
                    {
                        SetFlyMode(false);

                        me->SetAnimTier(0);

                        Position l_Pos = me->GetPosition();
                        l_Pos.m_positionZ -= 5.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveTakeoff(0, l_Pos);

                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            AttackStart(l_Target);

                        me->SetReactState(REACT_AGGRESSIVE);

                        events.ScheduleEvent(EVENT_VENGEANCE, 10000);
                        events.DelayEvents(3 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    default:
                        break;
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case EVENT_KNOCKDOWN_KICK:
                    {
                        Talk(SAY_TURN_KICK);
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_KNOCKDOWN_KICK);
                        events.ScheduleEvent(EVENT_KNOCKDOWN_KICK, m_ShadowstepTime > 0 ? 15000 : 21000);
                        break;
                    }
                    case EVENT_DEEPENING_SHADOWS:
                    {
                        Talk(SAY_DEEPENING_SHADOWS);
                        DoCast(SPELL_DEEPENING_SHADOWS_CAST);
                        events.ScheduleEvent(EVENT_DEEPENING_SHADOWS, m_ShadowstepTime > 0 ? 32000 : 22000);
                        break;
                    }
                    case EVENT_DETONATING_MOONGLAIVE:
                    {
                        Talk(SAY_FEL_GLAIVE);
                        DoCast(SPELL_DETONATING_MOONGLAIVE);
                        events.ScheduleEvent(EVENT_DETONATING_MOONGLAIVE, m_ShadowstepTime > 0 ? 47000 : 7000);
                        break;
                    }
                    case EVENT_SHADOWSTEP:
                    {
                        if (me->HasAura(SPELL_VENGEANCE_AURA))
                        {
                            events.ScheduleEvent(EVENT_SHADOWSTEP, 2000);
                            break;
                        }

                        Talk(SAY_SHADOWSTEP);
                        me->SetReactState(REACT_PASSIVE);
                        me->StopAttack();
                        me->GetMotionMaster()->Clear();
                        m_CanCheckLight = false;
                        m_ShadowstepTime = uint32(time(nullptr));
                        DoCast(SPELL_SHADOWSTEP);
                        events.StopEvents(true);
                        events.ScheduleEvent(EVENT_SHADOWSTEP, 60000);
                        break;
                    }
                    case EVENT_VENGEANCE:
                    {
                        if (m_VengeanceCount >= eCordanaData::DATA_MAX_SENTRY)
                            m_VengeanceCount = 0;

                        DoCast(g_VengeanceSpawnPos[m_VengeanceCount++], SPELL_AVATAR_OF_VENGEANCE_MISSILE, true);

                        events.ScheduleEvent(EVENT_VENGEANCE, 45000);
                        break;
                    }
                    case EVENT_CREEPING_DOOM:
                    {
                        Talk(SAY_CREEPING_DOOM_WARN);

                        if (m_FirstCreepingDoom)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->GetMotionMaster()->Clear();
                            me->SetTarget(0);
                            me->AttackStop();
                            me->DelayedCastSpell(me, SPELL_CREEPING_DOOM_CHANNEL, false, IN_MILLISECONDS);
                            events.DelayEvents(3 * IN_MILLISECONDS);
                        }
                        else
                            me->DelayedCastSpell(me, SPELL_CREEPING_DOOM_OTHERS, false, IN_MILLISECONDS);

                        events.ScheduleEvent(EVENT_CREEPING_DOOM, 70000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void CheckPlayersFall()
            {
                if (instance == nullptr)
                    return;

                Map::PlayerList const& l_Players = instance->instance->GetPlayers();

                for (auto l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                {
                    Unit* l_Target = l_Itr->getSource();

                    if (l_Target == nullptr)
                        continue;

                    if (l_Target->GetPositionZ() > -300.f)
                        continue;

                    if (l_Target->IsOnVehicle())
                        continue;

                    me->getThreatManager().modifyThreatPercent(l_Target, -99.f);

                    l_Target->CastSpell(l_Target, Spells::SPELL_HELP_IM_FALLING, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Intro && m_CheckIntroTimer)
                {
                    if (m_CheckIntroTimer <= p_Diff)
                    {
                        if (Player* l_Player = me->FindNearestPlayer(80.0f))
                        {
                            m_Intro = false;

                            DoCast(me, SPELL_CORDANA_INTRO, true);

                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(1, g_CenterPos);
                            me->SetHomePosition(g_CenterPos);

                            me->RemoveAura(eVotWSpells::SpellVeiledInShadow);

                            /// Used to remove Elune's Light from players
                            ResetEncounter();

                            m_CheckIntroTimer = 0;
                        }
                        else
                            m_CheckIntroTimer = 1000;
                    }
                    else
                        m_CheckIntroTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                if (!g_FightArea.contains(G3D::Point2(me->GetPositionX(), me->GetPositionY())))
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (m_SafetyCheckTimer <= 0)
                {
                    m_SafetyCheckTimer = IN_MILLISECONDS;
                    CheckPlayersFall();
                }
                else
                    m_SafetyCheckTimer -= p_Diff;

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_CREEPING_DOOM_CHANNEL))
                    return;

                if (events.IsStopped() && m_CanCheckLight)
                {
                    std::list<Player*> l_PlayerList;
                    me->GetPlayerListInGrid(l_PlayerList, 15.0f);

                    /// Cordana will disappear into stealth until her position is revealed by Elune's Light...
                    if (IsNearElunesLight(me))
                        EndVeiledInShadow();
                    /// ... or until 1 minute has passed
                    else if (uint32(time(nullptr)) >= (m_ShadowstepTime + 60))
                        EndVeiledInShadow();

                    return;
                }

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_MainSentryGuid = p_Guid;
            }

            uint64 GetGUID(int32 p_ID /*= 0*/) override
            {
                return m_MainSentryGuid;
            }

            void ResetEncounter()
            {
                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELUNES_LIGHT_OVERRIDE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELUNES_LIGHT_ORB_VISUAL_2);
                }

                me->DespawnCreaturesInArea(eCreatures::NPC_ELUNES_LIGHT_FIGHT, 200.f);
            }

            void EndVeiledInShadow()
            {
                Talk(SAY_CORDANA_FOUND);

                me->RemoveAura(eVotWSpells::SpellVeiledInShadow);
                me->RemoveAura(SPELL_VEILED_IN_SHADOW_2);
                me->RemoveAura(SPELL_FEL_GLAIVE_PERIODIC);

                events.StopEvents(false);

                events.RescheduleEvent(EVENT_KNOCKDOWN_KICK, 12000);
                events.RescheduleEvent(EVENT_DEEPENING_SHADOWS, 15000);
                events.RescheduleEvent(EVENT_DETONATING_MOONGLAIVE, 34000);

                me->SetReactState(REACT_AGGRESSIVE);

                if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                {
                    AttackStart(l_Target);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(l_Target);
                }
            }

            void HandleWalkway()
            {
                AddTimedDelayedOperation(5000, [this]() -> void
                {
                    AreaTrigger* l_AreaTrigger = new AreaTrigger;
                    if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 0, me, me, nullptr, g_WalkwayPos, g_WalkwayPos, nullptr, 0, eCordanaData::DATA_WALKWAY_ENTRY))
                        delete l_AreaTrigger;
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_cordanaAI(p_Creature);
        }
};

/// Glowing Sentry - 100525
class npc_cordana_glowing_sentry : public CreatureScript
{
    public:
        npc_cordana_glowing_sentry() : CreatureScript("npc_cordana_glowing_sentry") { }

        struct npc_cordana_glowing_sentryAI : public ScriptedAI
        {
            npc_cordana_glowing_sentryAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                if (me->GetPositionZ() > 270.0f) ///< Not boss
                {
                    DoCast(me, SPELL_ELUNES_LIGHT_ORB_VISUAL, true);
                    DoCast(me, SPELL_ELUNES_LIGHT_HALO_VISUAL, true);
                }
            }

            void Reset() override
            {
                std::vector<Position> l_CheckPositions =
                {
                    { 4404.76f, -229.340f, -258.500f, 3.84000f },
                    { 4078.45f, -325.234f, -281.274f, 2.35448f },
                    { 4482.64f, -334.073f, -240.317f, 2.43962f }
                };

                for (Position l_Pos : l_CheckPositions)
                {
                    if (me->IsNearPosition(&l_Pos, 0.1f))
                    {
                        DoCast(me, SPELL_ELUNES_LIGHT_ORB_VISUAL, true);
                        DoCast(me, SPELL_ELUNES_LIGHT_HALO_VISUAL, true);
                    }
                }

                /// Boss sentry
                if (me->IsNearPosition(&l_CheckPositions[1], 0.1f))
                {
                    AddTimedDelayedOperation(1000, [this]() -> void
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Cordana = Creature::GetCreature(*me, l_Instance->GetData64(NPC_CORDANA_FELSONG)))
                            {
                                if (l_Cordana->IsAIEnabled)
                                    l_Cordana->AI()->SetGUID(me->GetGUID());
                            }
                        }
                    });
                }
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (me->HasAura(SPELL_ELUNES_LIGHT_ORB_VISUAL) && me->HasAura(SPELL_ELUNES_LIGHT_HALO_VISUAL))
                {
                    DoCast(p_Clicker, SPELL_ELUNES_LIGHT_CLICK);
                    DoCast(p_Clicker, SPELL_ELUNES_LIGHT_ORB_VISUAL_2);

                    me->RemoveAurasDueToSpell(SPELL_ELUNES_LIGHT_ORB_VISUAL);
                    me->RemoveAurasDueToSpell(SPELL_ELUNES_LIGHT_HALO_VISUAL);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_STEAL_LIGHT:
                    {
                        me->RemoveAurasDueToSpell(SPELL_ELUNES_LIGHT_ORB_VISUAL);
                        me->RemoveAurasDueToSpell(SPELL_ELUNES_LIGHT_HALO_VISUAL);
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
            return new npc_cordana_glowing_sentryAI(p_Creature);
        }
};

/// Elune's Light - 103342
class npc_cordana_elunes_light : public CreatureScript
{
    public:
        npc_cordana_elunes_light() : CreatureScript("npc_cordana_elunes_light") { }

        struct npc_cordana_elunes_lightAI : public ScriptedAI
        {
            npc_cordana_elunes_lightAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Clicked(false), m_SentryGuid(0) { }

            bool m_Clicked;

            uint64 m_SentryGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->IsPlayer())
                    p_Summoner->CastSpell(me, SPELL_ELUNES_LIGHT_AT_2, true);
                else
                    me->CastSpell(me, SPELL_ELUNES_LIGHT_AT_2, true);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (!m_Clicked && me->HasAura(SPELL_ELUNES_LIGHT_AT_2))
                {
                    m_Clicked = true;

                    DoCast(p_Clicker, SPELL_ELUNES_LIGHT_CLICK);
                    DoCast(p_Clicker, SPELL_ELUNES_LIGHT_ORB_VISUAL_2);

                    if (AreaTrigger* l_AT = me->FindNearestAreaTrigger(SPELL_ELUNES_LIGHT_AT_2, 2.0f))
                        l_AT->Despawn();

                    me->DespawnOrUnsummon();
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_STEAL_LIGHT:
                    {
                        if (AreaTrigger* l_AT = me->FindNearestAreaTrigger(SPELL_ELUNES_LIGHT_AT_2, 2.0f))
                            l_AT->Despawn();

                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDespawned() override
            {
                if (m_Clicked)
                    return;

                if (Creature* l_Sentry = Creature::GetCreature(*me, m_SentryGuid))
                {
                    l_Sentry->CastSpell(l_Sentry, SPELL_ELUNES_LIGHT_ORB_VISUAL, true);
                    l_Sentry->CastSpell(l_Sentry, SPELL_ELUNES_LIGHT_HALO_VISUAL, true);
                }
                else if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Cordana = Creature::GetCreature(*me, l_Instance->GetData64(NPC_CORDANA_FELSONG)))
                    {
                        Creature* l_Sentry = nullptr;
                        if (l_Cordana->IsAIEnabled && (l_Sentry = Creature::GetCreature(*me, l_Cordana->AI()->GetGUID())))
                        {
                            l_Sentry->CastSpell(l_Sentry, SPELL_ELUNES_LIGHT_ORB_VISUAL, true);
                            l_Sentry->CastSpell(l_Sentry, SPELL_ELUNES_LIGHT_HALO_VISUAL, true);
                        }
                    }
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_SentryGuid = p_Guid;
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cordana_elunes_lightAI(p_Creature);
        }
};

/// Deepening Shadows - 107533
class npc_cordana_deepening_shadows : public CreatureScript
{
    public:
        npc_cordana_deepening_shadows() : CreatureScript("npc_cordana_deepening_shadows") { }

        struct npc_cordana_deepening_shadowsAI : public ScriptedAI
        {
            npc_cordana_deepening_shadowsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_VEILED_IN_SHADOW_2, true);
                DoCast(me, SPELL_DEEPENING_SHADOWS_SPAWN, true);
                DoCast(me, eVotWSpells::SpellRevealed, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cordana_deepening_shadowsAI(p_Creature);
        }
};

/// Avatar of Shadow - 104293
class npc_cordana_avatar_of_shadow : public CreatureScript
{
    public:
        npc_cordana_avatar_of_shadow() : CreatureScript("npc_cordana_avatar_of_shadow") { }

        struct npc_cordana_avatar_of_shadowAI : public Scripted_NoMovementAI
        {
            npc_cordana_avatar_of_shadowAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner == nullptr)
                    return;

                Creature* l_Cordana = p_Summoner->ToCreature();
                if (l_Cordana == nullptr || !l_Cordana->IsAIEnabled)
                    return;

                DoCast(me, SPELL_STEAL_LIGHT, true);
                DoCast(me, SPELL_AVATAR_OF_VENGEANCE, true);
                DoCast(me, eVotWSpells::SpellRevealed, true);

                me->SetInCombatWithZone();

                if (m_Instance)
                {
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELUNES_LIGHT_OVERRIDE);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELUNES_LIGHT_ORB_VISUAL_2);

                    if (Creature* l_Sentry = Creature::GetCreature(*me, l_Cordana->AI()->GetGUID()))
                    {
                        l_Sentry->RemoveAura(SPELL_ELUNES_LIGHT_ORB_VISUAL);
                        l_Sentry->RemoveAura(SPELL_ELUNES_LIGHT_HALO_VISUAL);
                    }

                    me->DespawnCreaturesInArea(NPC_ELUNES_LIGHT_FIGHT, 200.0f);

                    DoCast(me, SPELL_ELUNES_LIGHT_ORB_VISUAL_2, true);
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 1000);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 2);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Position l_Pos  = g_CenterPos;

                float l_Dist    = std::min(me->GetDistance(l_Pos), 25.0f);
                float l_Angle   = Position::NormalizeOrientation(g_CenterPos.GetAngle(me) + frand(-(M_PI / 6.0f), M_PI / 6.0f));

                g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, l_Dist, l_Angle);

                DoCast(l_Pos, eVotWSpells::SpellElunesLightSummon, true);
            }

            void OnAddThreat(Unit* /*p_Victim*/, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                p_Threat = 0.0f;
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_SHADOW_STRIKE_SEARCHER:
                    {
                        DoCast(p_Target, SPELL_SHADOW_STRIKE, false);
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

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_SHADOW_STRIKE_SEARCHER, true);
                        events.ScheduleEvent(EVENT_1, 1500);
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
            return new npc_cordana_avatar_of_shadowAI(p_Creature);
        }
};

/// Avatar of Vengeance - 100351
class npc_cordana_avatar_of_vengeance : public CreatureScript
{
    public:
        npc_cordana_avatar_of_vengeance() : CreatureScript("npc_cordana_avatar_of_vengeance") { }

        struct npc_cordana_avatar_of_vengeanceAI : public ScriptedAI
        {
            npc_cordana_avatar_of_vengeanceAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();

                m_Found = false;
            }

            InstanceScript* m_Instance;

            bool m_Found;

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(me, SPELL_SPIRIT_OF_VENGEANCE, true);
                DoCast(me, eVotWSpells::SpellVeiledInShadow, true);
                DoCast(me, SPELL_VEILED_IN_SHADOW_2, true);

                p_Summoner->AddAura(SPELL_VENGEANCE_AURA, p_Summoner);

                me->SetVisible(false);

                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);

                me->SetInCombatWithZone();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 3);
            }

            void AttackStart(Unit* p_Target, bool /*p_Melee = true*/) override
            {
                ScriptedAI::AttackStart(p_Target, m_Found);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Found && !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (!m_Found)
                {
                    if (IsNearElunesLight(me))
                        EndVeiledInShadow();
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskNone))
                            DoCast(l_Player, SPELL_VENGEANCE_DAMAGES);

                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void EndVeiledInShadow()
            {
                m_Found = true;

                me->RemoveAura(eVotWSpells::SpellVeiledInShadow);
                me->RemoveAura(SPELL_VEILED_IN_SHADOW_2);

                me->SetVisible(true);

                me->ClearUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    me->InterruptNonMeleeSpells(true);

                    l_Owner->RemoveAura(SPELL_VENGEANCE_AURA);

                    if (Unit* l_Target = l_Owner->getVictim())
                        AttackStart(l_Target, true);
                }

                events.ScheduleEvent(EVENT_1, 5000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cordana_avatar_of_vengeanceAI(p_Creature);
        }
};

/// Creeping Doom - 100336
class npc_cordana_creeping_doom : public CreatureScript
{
    public:
        npc_cordana_creeping_doom() : CreatureScript("npc_cordana_creeping_doom") { }

        struct npc_cordana_creeping_doomAI : public ScriptedAI
        {
            npc_cordana_creeping_doomAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                DoCast(me, SPELL_CREEPING_DOOM_VISUAL, true);
                DoCast(me, SPELL_CREEPING_DOOM_AURA, true);

                me->SetCollision(false);

                Position l_Pos = me->GetPosition();

                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 65.0f, me->m_orientation, true);

                me->SetWalk(true);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(1, l_Pos);
            }

            bool CanBeSeenForced() const override
            {
                return true;
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = me->GetSpeed(MOVE_WALK);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID != 1)
                    return;

                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                std::list<Player*> l_PlayerList;
                me->GetPlayerListInGrid(l_PlayerList, 20.0f);

                for (Player* l_Player : l_PlayerList)
                {
                    if (l_Player->HasAura(SPELL_ELUNES_LIGHT_OVERRIDE) && l_Player->HasAura(SPELL_ELUNES_LIGHT_ORB_VISUAL_2))
                    {
                        me->SetVisible(true);
                        return;
                    }
                }

                if (me->FindNearestCreature(NPC_ELUNES_LIGHT_FIGHT, 20.0f))
                {
                    me->SetVisible(true);
                    return;
                }

                me->SetVisible(false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cordana_creeping_doomAI(p_Creature);
        }
};

/// Grimhorn the Enslaver - 102566
class npc_cordana_grimhorn_the_enslaver : public CreatureScript
{
    public:
        npc_cordana_grimhorn_the_enslaver() : CreatureScript("npc_cordana_grimhorn_the_enslaver") { }

        struct npc_cordana_grimhorn_the_enslaverAI : public ScriptedAI
        {
            npc_cordana_grimhorn_the_enslaverAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                p_Creature->CastSpell(p_Creature, SPELL_FEL_DISSOLVE_IN, true);
            }

            void Reset() override
            {
                events.Reset();

                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 8500);
                events.ScheduleEvent(EVENT_2, 16000);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_ANGUISHED_SOULS_FILTER:
                    {
                        Position l_Pos = p_Target->GetPosition();
                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 3.0f, frand(0.0f, 2.0f * M_PI));

                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            DoCast(l_Pos, SPELL_ANGUISHED_SOULS_MISSILE, true);

                            l_Pos.m_orientation = Position::NormalizeOrientation(l_Pos.m_orientation + (2.0f * M_PI / 3.0f));
                            p_Target->SimplePosXYRelocationByAngle(l_Pos, 3.0f, l_Pos.m_orientation);
                        }

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

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetCurrentSpell(CURRENT_CHANNELED_SPELL) != nullptr)
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                            DoCast(l_Target, SPELL_IMPRISON_MISSILE);

                        events.ScheduleEvent(EVENT_1, 20000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_ANGUISHED_SOULS_FILTER);
                        events.ScheduleEvent(EVENT_2, 10000);
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
            return new npc_cordana_grimhorn_the_enslaverAI(p_Creature);
        }
};

/// Cage - 102572
class npc_cordana_cage : public CreatureScript
{
    public:
        npc_cordana_cage() : CreatureScript("npc_cordana_cage") { }

        struct npc_cordana_cageAI : public Scripted_NoMovementAI
        {
            npc_cordana_cageAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(REACT_PASSIVE);

                DoCast(me, SPELL_JAILERS_CAGE);

                p_Summoner->CastSpell(me, SPELL_TORMENT_CHANNEL, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cordana_cageAI(p_Creature);
        }
};

/// Bat - 100285
class npc_cordana_bat : public CreatureScript
{
    public:
        npc_cordana_bat() : CreatureScript("npc_cordana_bat") { }

        struct npc_cordana_batAI : public ScriptedAI
        {
            npc_cordana_batAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_PassengerGuid = 0;
            }

            uint64 m_PassengerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->IsPlayer())
                    return;

                SetFlyMode(true);

                m_PassengerGuid = p_Summoner->GetGUID();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Unit* l_Unit = Unit::GetUnit(*me, m_PassengerGuid))
                    {
                        //l_Unit->EnterVehicle(me);
                        //l_Unit->SetCanFly(false);
                    }

                    AddTimedDelayedOperation(1000, [this]() -> void
                    {
                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 25.0f, g_CenterPos.GetAngle(me));

                        l_Pos.m_positionZ = -280.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, l_Pos);
                    });
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    if (Unit* l_Unit = Unit::GetUnit(*me, m_PassengerGuid))
                    {
                        l_Unit->ExitVehicle();
                        l_Unit->SetCanFly(false);
                    }

                    me->DespawnOrUnsummon(500);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cordana_batAI(p_Creature);
        }
};

/// Drelanim Whisperwind <Warden> - 103860
class npc_cordana_drelanim : public CreatureScript
{
    public:
        npc_cordana_drelanim() : CreatureScript("npc_cordana_drelanim") { }

        struct npc_cordana_drelanimAI : public ScriptedAI
        {
            npc_cordana_drelanimAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                    {
                        uint32 l_Time = 2000;

                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(0);
                        });

                        l_Time += 8000;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(1);
                        });

                        l_Time += 8000;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(2);
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
          return new npc_cordana_drelanimAI(p_Creature);
        }
};

/// Elune's Light - 204481
class spell_cordana_elunes_light : public SpellScriptLoader
{
    public:
        spell_cordana_elunes_light() : SpellScriptLoader("spell_cordana_elunes_light") { }

        class spell_cordana_elunes_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cordana_elunes_light_SpellScript);

            uint64 m_SentryGuid = 0;

            void HandleLaunch(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(SPELL_ELUNES_LIGHT_OVERRIDE))
                        m_SentryGuid = l_Aura->GetCasterGUID();

                    l_Caster->RemoveAura(SPELL_ELUNES_LIGHT_OVERRIDE);
                    l_Caster->RemoveAura(SPELL_ELUNES_LIGHT_ORB_VISUAL_2);
                }
            }

            void HandleHit(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest()))
                    {
                        l_Caster->CastSpell(l_Dest, eVotWSpells::SpellElunesLightSummon, true);

                        Position l_Pos = Position(l_Dest->m_positionX, l_Dest->m_positionY, l_Dest->m_positionZ, l_Dest->m_orientation);

                        uint64 l_Guid = m_SentryGuid;
                        float l_Dist  = l_Caster->GetDistance(l_Pos) + 5.0f;
                        l_Caster->AddDelayedEvent([l_Caster, l_Guid, l_Dist]() -> void
                        {
                            if (Creature* l_Light = l_Caster->FindNearestCreature(NPC_ELUNES_LIGHT_FIGHT, l_Dist))
                            {
                                if (l_Light->IsAIEnabled && l_Light->GetAnyOwner() == l_Caster)
                                    l_Light->AI()->SetGUID(l_Guid);
                            }
                        }, 100);
                    }
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_cordana_elunes_light_SpellScript::HandleLaunch, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHit += SpellEffectFn(spell_cordana_elunes_light_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cordana_elunes_light_SpellScript();
        }
};

/// Elune's Light (overrider) - 204485
class spell_cordana_elunes_light_override : public SpellScriptLoader
{
    public:
        spell_cordana_elunes_light_override() : SpellScriptLoader("spell_cordana_elunes_light_override") { }

        class spell_cordana_elunes_light_override_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cordana_elunes_light_override_AuraScript);

            uint64 m_SentryGuid = 0;

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (l_Target == nullptr || !l_Target->IsPlayer() || l_Caster == nullptr)
                    return;

                m_SentryGuid = l_Caster->GetGUID();
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                    return;

                Unit* l_Target = GetTarget();
                if (l_Target == nullptr || !l_Target->IsPlayer())
                    return;

                Position l_Pos = l_Target->GetPosition();

                l_Pos.GetRandomNearPosition(l_Pos, 10.0f, l_Target);

                l_Target->CastSpell(l_Pos, eVotWSpells::SpellElunesLightSummon, true);

                uint64 l_Guid = m_SentryGuid;
                l_Target->AddDelayedEvent([l_Target, l_Guid]() -> void
                {
                    if (Creature* l_Light = l_Target->FindNearestCreature(NPC_ELUNES_LIGHT_FIGHT, 15.0f))
                    {
                        if (l_Light->IsAIEnabled && l_Light->GetAnyOwner() == l_Target)
                            l_Light->AI()->SetGUID(l_Guid);
                    }
                }, 100);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_cordana_elunes_light_override_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_cordana_elunes_light_override_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cordana_elunes_light_override_AuraScript();
        }
};

/// Fel Glaive - 197333
class spell_cordana_fel_glaive_periodic : public SpellScriptLoader
{
    public:
        spell_cordana_fel_glaive_periodic() : SpellScriptLoader("spell_cordana_fel_glaive_periodic") { }

        class spell_cordana_fel_glaive_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cordana_fel_glaive_periodic_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Creature* l_Cordana = l_Caster->ToCreature())
                    {
                        if (l_Cordana->IsAIEnabled)
                            l_Cordana->AI()->DoAction(1);
                    }

                    l_Caster->CastSpell(l_Caster, SPELL_FEL_GLAIVE_5_AT, true);
                    l_Caster->CastSpell(l_Caster, SPELL_FEL_GLAIVE_4_AT, true);

                    Position l_Pos = g_CenterPos;
                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 25.0f), frand(0.0f, 2.0f * M_PI));

                    l_Caster->NearTeleportTo(l_Pos);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Cordana = GetCaster()->ToCreature())
                {
                    if (l_Cordana->IsAIEnabled)
                        l_Cordana->AI()->DoAction(0);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_cordana_fel_glaive_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_cordana_fel_glaive_periodic_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cordana_fel_glaive_periodic_AuraScript();
        }
};

/// Creeping Doom - 197422
class spell_cordana_creeping_doom_periodic : public SpellScriptLoader
{
    public:
        spell_cordana_creeping_doom_periodic() : SpellScriptLoader("spell_cordana_creeping_doom_periodic") { }

        class spell_cordana_creeping_doom_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cordana_creeping_doom_periodic_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                /// Retail ticks
                if (GetCaster()->GetMap()->IsMythic())
                {
                    if (p_AurEff->GetTickNumber() != 1 && p_AurEff->GetTickNumber() != 3 &&
                        p_AurEff->GetTickNumber() != 6 && p_AurEff->GetTickNumber() != 8)
                        return;
                }
                else if (GetCaster()->GetMap()->IsHeroic())
                {
                    if (p_AurEff->GetTickNumber() != 1 &&  p_AurEff->GetTickNumber() != 6)
                        return;
                }

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(2);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes p_Mode)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(3);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_cordana_creeping_doom_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_cordana_creeping_doom_periodic_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cordana_creeping_doom_periodic_AuraScript();
        }
};

/// Boss 4 Walkway - 10057
class at_cordana_walkway : public AreaTriggerEntityScript
{
    public:
        at_cordana_walkway() : AreaTriggerEntityScript("at_cordana_walkway") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 230765);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 111397);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 53.85165f);

            p_AreaTrigger->SetWindTransPos(G3D::Vector3(0.0f, -20.0f, 0.0f));
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_cordana_walkway();
        }
};

/// Deepening Shadows - 213397 / AT id 7101
class areatrigger_cordana_deepening_shadows : public AreaTriggerEntityScript
{
    public:
        areatrigger_cordana_deepening_shadows() : AreaTriggerEntityScript("areatrigger_cordana_deepening_shadows") { }

        enum eNpc
        {
            GlowingSentry = 100525,
            ElunesLight   = 103342
        };

        bool OnAddTarget(AreaTrigger* /*p_AreaTrigger*/, Unit* p_Target) override
        {
            /// Elune's Light wielder is immune to Deepening Shadows
            if (p_Target->HasAura(SPELL_ELUNES_LIGHT_OVERRIDE))
                return true;

            return false;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            bool l_ShouldGrow = true;
            std::list<Unit*> l_Targets;
            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_Targets, *p_AreaTrigger, p_AreaTrigger->GetRadius() + 8, true); ///< Check every player closer than radius + 15 (range of Elune's Light)

            for (Unit* l_Target : l_Targets)
            {
                if ((l_Target->HasAura(SPELL_ELUNES_LIGHT_OVERRIDE) && l_Target->IsPlayer()) || l_Target->GetEntry() == eNpc::ElunesLight)
                    l_ShouldGrow = false;
            }

            if (l_ShouldGrow)
            {
                /// On retail, even if the value didn't changed, the fields are sent
                for (uint8 l_I = 0; l_I < 7; ++l_I)
                    p_AreaTrigger->ForceValuesUpdateAtIndex(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

                AreaTriggerInfo l_AtInfo = p_AreaTrigger->GetAreaTriggerInfo();

                /// No need to update target scaling if there is no targets
                if (p_AreaTrigger->GetLiveTime() <= l_AtInfo.scalingDelay || !l_AtInfo.Param)
                    return;

                float l_BaseScale = l_AtInfo.Radius;
                float l_Scale = p_AreaTrigger->GetRadius();
                float l_PctPerMSec = 5.0f / 1000.0f;

                float l_Pct = (l_Scale / l_BaseScale) + (l_AtInfo.Param * l_PctPerMSec / 100.0f);

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetLiveTime());
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);

                /// Current scale
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, l_Scale / l_BaseScale);
                /// Next scale
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Pct);

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, l_AtInfo.Param);

                p_AreaTrigger->SetRadius(l_BaseScale * l_Pct);

                p_AreaTrigger->SetCurveChanged(true);
            }
            else
            {
                /// On retail, even if the value didn't changed, the fields are sent
                for (uint8 l_I = 0; l_I < 7; ++l_I)
                    p_AreaTrigger->ForceValuesUpdateAtIndex(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

                AreaTriggerInfo l_AtInfo = p_AreaTrigger->GetAreaTriggerInfo();

                /// No need to update target scaling if there is no targets
                if (p_AreaTrigger->GetLiveTime() <= l_AtInfo.scalingDelay || !l_AtInfo.Param)
                    return;

                float l_BaseScale = l_AtInfo.Radius;
                float l_Scale = p_AreaTrigger->GetRadius();
                float l_PctPerMSec = -10.0f / 1000.0f;

                float l_Pct = (l_Scale / l_BaseScale) + (l_AtInfo.Param * l_PctPerMSec / 100.0f);

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetLiveTime());
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);

                /// Current scale
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, l_Scale / l_BaseScale);
                /// Next scale
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Pct);

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, l_AtInfo.Param);

                p_AreaTrigger->SetRadius(l_BaseScale * l_Pct);

                p_AreaTrigger->SetCurveChanged(true);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_cordana_deepening_shadows();
        }
};

/// Detonating Moonglaive - 197513 - Areatrigger Entry 5354
class areatrigger_cordana_detonating_moonglaive : public AreaTriggerEntityScript
{
    public:
        areatrigger_cordana_detonating_moonglaive() : AreaTriggerEntityScript("areatrigger_cordana_detonating_moonglaive") { }

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* /*p_Target*/) override
        {
            /// Damage should be done only when the glaive is returning
            return p_AreaTrigger->GetAreaTriggerInfo().moveType == AreaTriggerMoveType::AT_MOVE_TYPE_BOOMERANG;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_cordana_detonating_moonglaive();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_cordana()
{
    /// Boss
    new boss_cordana();

    /// Creatures
    new npc_cordana_glowing_sentry();
    new npc_cordana_elunes_light();
    new npc_cordana_deepening_shadows();
    new npc_cordana_avatar_of_shadow();
    new npc_cordana_avatar_of_vengeance();
    new npc_cordana_creeping_doom();
    new npc_cordana_grimhorn_the_enslaver();
    new npc_cordana_cage();
    new npc_cordana_bat();
    new npc_cordana_drelanim();

    /// Spells
    new spell_cordana_elunes_light();
    new spell_cordana_elunes_light_override();
    new spell_cordana_fel_glaive_periodic();
    new spell_cordana_creeping_doom_periodic();

    /// AreaTriggers
    new at_cordana_walkway();
    new areatrigger_cordana_deepening_shadows();
    new areatrigger_cordana_detonating_moonglaive();
}
#endif
