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
    SAY_FEEDING_TIME        = 0,
    SAY_GATHERING_CLOUDS    = 1,
    SAY_RAZOR_WING          = 2,
    SAY_AGGRO               = 3,
    SAY_FEEDING_TIME_1      = 4,
    SAY_GATHERING_CLOUDS_1  = 5,
    SAY_RAZOR_WING_1        = 6,
};

enum Spells
{
    SPELL_SPIDER_START                  = 210284,
    SPELL_ENERGY_TRACKER_TRANSFORM      = 225364,
    SPELL_ENERGY_TRANSFORM_COST         = 210366,
    SPELL_TRANSFORM_DISSOLVE            = 210309,
    SPELL_WEB_WRAPPED                   = 215188,
    SPELL_BERSERK                       = 26662,
    SPELL_DRIPPING_FANGS_DEBUFF         = 210228,
    SPELL_NECROTIC_VENOM_DEBUFF_01      = 215464,
    SPELL_NECROTIC_VENOM_DEBUFF_02      = 215460,
    SPELL_NECROTIC_VENOM_DEBUFF_03      = 215449,

    /// Phase One - Spider form
    SPELL_SPIDER_TRANSFORM_DUMMY        = 210326,
    SPELL_SPIDER_TRANSFORM              = 210292,
    SPELL_WEB_OF_PAIN_FILTER            = 215288,
    SPELL_WEB_OF_PAIN_SHARE_AURA_01     = 215300,
    SPELL_WEB_OF_PAIN_SHARE_AURA_02     = 215307,
    SPELl_WEB_OF_PAIN_FAR_VISUAL        = 218656,
    SPELL_WEB_OF_PAIN_DAMAGE            = 233485,
    SPELL_FEEDING_TIME                  = 212364,
    SPELL_FEEDING_TIME_2                = 214305,
    SPELL_FEEDING_TIME_FILTER           = 214311,
    SPELL_FEEDING_TIME_SUM_STALKER      = 214275, ///< NPC_VILE_AMBUSH_STALKER
    SPELL_VILE_AMBUSH_FILTER            = 214328,
    SPELL_VILE_AMBUSH_JUMP              = 214339,
    SPELL_VILE_AMBUSH_JUMP_TRIGGERED    = 214346,
    SPELL_VILE_AMBUSH_VISUAL_JUMP       = 215253,
    SPELL_VILE_AMBUSH_DAMAGE            = 214348,
    SPELL_NECROTIC_VENOM                = 215443,
    SPELL_VENOMOUS_POOL_AT              = 213123,
    SPELL_VENOMOUS_POOL_AT_2            = 225519,
    SPELL_VENOMOUS_POOL_MISSILE         = 213789,

    /// Mythic
    SPELL_PAIN_LASH                     = 215306,
    SPELL_TANGLED_WEB_AREATRIGGER       = 217982,

    /// Phase Second - Darkwing form
    SPELL_DARKWING_FORM_DUMMY           = 210308,
    SPELL_DARKWING_FORM                 = 210293,
    SPELL_TWISTING_SHADOWS_FILTER       = 210864,
    SPELL_TWISTING_SHADOWS_AT           = 210792, ///< Casters: 106350, 111439
    SPELL_TWISTING_SHADOWS_AURA         = 210863,
    SPELL_GATHERING_CLOUDS              = 212707,
    SPELL_SHIMMERING_FEATHER_TRIG       = 213064,
    SPELL_RAZOR_WING_FILTER             = 210553,
    SPELL_RAZOR_WING                    = 210547,
    SPELL_RAZOR_WING_VISUAL             = 212842,
    SPELL_RAZOR_WING_DMG                = 212853,
    SPELL_DARK_STORM_FILTER             = 210948,
    SPELL_DARK_STORM_AURA               = 211122,
    SPELL_DARK_STORM_AT                 = 211124,
    SPELL_DARK_STORM_DAMAGE             = 213936,
    SPELL_DARK_STORM_COSMETIC_AURA      = 218536,
    SPELL_DARK_STORM_COSMETIC_DUMMY     = 221588,
    SPELL_EYE_OF_THE_STORM              = 211127,
    SPELL_FOUL_WINDS_STACK              = 211137,
    SPELL_RAKING_TALONS                 = 215582,
    SPELL_RAKING_TALONS_2               = 222037,
    SPELL_FOUL_WINDS                    = 211137,

    /// Mythic
    SPELL_VIOLENT_WINDS_ALLOWING        = 217947,
    SPELL_VIOLENT_WINDS                 = 218124,
    SPELL_VIOLENT_WINDS_JUMP            = 218537,
    SPELL_VIOLENT_WINDS_COSMETIC        = 225439,
    SPELL_WIND_BURN                     = 218519,
    SPELL_GROWING_EGG_SAC_COSMETIC      = 225204,
    SPELL_GROWING_EGG_SAC_SCALE         = 225259,
    SPELL_GROWING_EGG_SCALE_FULL        = 228192,
    SPELL_NIGHTMARE_SPAWN_PERIODIC      = 218629,
    SPELL_NIGHTMARE_SPAWN_DUMMY         = 218630,
    SPELL_NIGHTMARE_SPAWN_MISSILE       = 218632,

    /// Other
    SPELL_DRUID_TRANSFORM               = 226017,
    SPELL_SPIDER_TRANSFORM_2            = 228523,
    SPELL_SHIMMERING_FEATHER_OVERRIDE   = 221913,
    SPELL_SWARM                         = 214450  ///< Mythic buff boss
};

enum eEvents
{
    /// Phase One - Spider form
    EVENT_WEB_OF_PAIN = 1,
    EVENT_FEEDING_TIME,
    EVENT_NECROTIC_VENOM,
    EVENT_SPIDER_TRANSFORM,

    /// Phase Second - Darkwing form
    EVENT_DARKWING_FORM,
    EVENT_TWISTING_SHADOWS,
    EVENT_GATHERING_CLOUDS,
    EVENT_SUM_SHIMMERING_FEATHER,
    EVENT_RAZOR_WING,
    EVENT_DARK_STORM,
    EVENT_RAKING_TALONS,
    EVENT_RAKING_TALONS_2,
    EVENT_VIOLENT_WINDS,

    EVENT_REACT_AGGRESSIVE,
    EVENT_NEXT_PLATFORM,
    EVENT_CHECK_PLAYER
};

enum eTransform
{
    SPIDER_FORM                 = 1,
    DARKWING_FORM               = 2
};

enum eMisc
{
    /// SMSG_SET_MOVEMENT_ANIM_KIT
    ANIM_1                              = 753,
    ANIM_2                              = 2737,

    /// SMSG_PLAY_ONE_SHOT_ANIM_KIT
    ANIM_3                              = 9779,

    SPELL_EGG_DUMMY_1                   = 223438,
    SPELL_EGG_DUMMY_2                   = 215508,
    SPELL_SUM_SKITTERING_SPIDERLING     = 215505, ///< 4x

    MAX_PLATFORMS                       = 3
};

Position const g_PlatformPos[eMisc::MAX_PLATFORMS] =
{
    { 11405.59f, 11400.70f, -85.32f, 0.0f },
    { 11376.42f, 11224.97f, -102.0f, 0.0f },
    { 11538.02f, 11298.43f, -87.06f, 0.0f }
};

Position const g_SpiderPos[12] =
{
    { 11411.42f, 11402.46f, -51.12f, 0.0f }, ///< Platform 1
    { 11413.96f, 11369.06f, -50.60f, 0.0f },
    { 11434.47f, 11374.71f, -51.50f, 0.0f },
    { 11373.11f, 11388.3f,  -50.58f, 0.0f },
    { 11343.57f, 11240.51f, -44.54f, 0.0f }, ///< Platform 2
    { 11382.04f, 11262.68f, -53.15f, 0.0f },
    { 11403.85f, 11224.34f, -51.75f, 0.0f },
    { 11377.61f, 11199.40f, -45.50f, 0.0f },
    { 11537.53f, 11264.99f, -47.43f, 0.0f }, ///< Platform 3
    { 11503.03f, 11288.73f, -45.12f, 0.0f },
    { 11522.21f, 11323.06f, -43.80f, 0.0f },
    { 11552.95f, 11302.63f, -38.93f, 0.0f }
};

Position const g_TwistBridgePos[5] =
{
    { 11362.16f, 11372.33f, -87.34f, 0.0f }, ///< NPC_TWISTING_SHADOWS_BRIDGE
    { 11449.68f, 11389.96f, -85.37f, 0.0f },
    { 11365.16f, 11308.75f, -98.53f, 0.0f },
    { 11491.63f, 11268.83f, -89.58f, 0.0f },
    { 11456.38f, 11238.37f, -93.77f, 0.0f }
};

std::array<Position const, MAX_PLATFORMS> g_PlatformTriggerPos =
{
    {
        { 11383.9f, 11230.1f, -101.783f, 0.68815f },
        { 11530.3f, 11291.9f, -87.5011f, 2.36680f },
        { 11411.8f, 11408.3f, -85.4046f, 5.27214f }
    }
};

/// Elerethe Renferal - 106087
class boss_elerethe_renferal : public CreatureScript
{
    public:
        boss_elerethe_renferal() : CreatureScript("boss_elerethe_renferal") { }

        struct boss_elerethe_renferalAI : public BossAI
        {
            boss_elerethe_renferalAI(Creature* p_Creature) : BossAI(p_Creature, DATA_RENFERAL)
            {
                m_IntroDone = false;
            }

            uint8 m_PlatformID;
            uint16 m_CheckEvadeTimer;
            uint32 m_BerserkTimer;
            bool m_IntroDone;
            bool m_ViolentWinds;

            uint8 m_ViolentWindsCount;

            bool m_IsFlying;

            void Reset() override
            {
                _Reset();
                me->SetPower(POWER_ENERGY, 0);
                RemoveEventTrash();
                me->RemoveAllAuras();
                me->SetReactState(REACT_DEFENSIVE);
                me->GetMotionMaster()->MoveIdle();
                me->NearTeleportTo({ 11405.6f, 11400.7f, -85.32f, 4.62f }); ///< HomePos
                me->SetFullHealth();
                me->SetAnimTier(0);

                m_PlatformID = 0;
                m_CheckEvadeTimer = 2000;
                m_ViolentWinds = false;
                m_ViolentWindsCount = 0;
                m_IsFlying = false;

                me->SetDisableGravity(false);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                std::list<Creature*> l_Triggers;
                auto& l_TriggerList = me->GetMap()->GetObjectsOnMap();
                for (auto l_Object = l_TriggerList.begin(); l_Object != l_TriggerList.end(); ++l_Object)
                {
                    if ((*l_Object)->GetEntry() != NPC_FLIGHT_LOC_STALKER || (*l_Object)->ToCreature() == nullptr)
                        continue;

                    l_Triggers.push_back((*l_Object)->ToCreature());
                }

                for (Creature* l_Trigger : l_Triggers)
                    l_Trigger->RemoveAura(SPELL_DARK_STORM_COSMETIC_AURA);

                if (instance != nullptr)
                    instance->SendWeatherToPlayers(WeatherState::WEATHER_STATE_FOG, 0.0f);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 250.0f))
                {
                    m_IntroDone = true;
                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3626, p_Who, nullptr, *p_Who))
                        delete l_Conversation;
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();
                me->SetReactState(REACT_AGGRESSIVE);
                DoCast(me, SPELL_ENERGY_TRACKER_TRANSFORM, true);
                DoCast(me, SPELL_WEB_WRAPPED, true);
                m_BerserkTimer = 9 * MINUTE * IN_MILLISECONDS;

                EventsForm(SPIDER_FORM);
                Talk(SAY_AGGRO);
            }

            bool BypassXPDistanceIfNeeded(WorldObject* p_Object) override
            {
                if (p_Object->GetZoneId() == me->GetZoneId())
                    return true;

                return false;
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (instance != nullptr)
                {
                    if (Creature* l_Stalker = Creature::GetCreature(*me, instance->GetData64(NPC_NIGHTMARE_SPAWN_STALKER)))
                        l_Stalker->RemoveAura(SPELL_NIGHTMARE_SPAWN_PERIODIC);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWISTING_SHADOWS_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WIND_BURN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EYE_OF_THE_STORM);

                    instance->SetBossState(DATA_RENFERAL, EncounterState::FAIL);

                    instance->DoRemoveForcedMovementsOnPlayers();
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->SetAnimTier(0);
                RemoveEventTrash();
                _JustDied();

                if (instance != nullptr)
                {
                    if (Creature* l_Stalker = Creature::GetCreature(*me, instance->GetData64(NPC_NIGHTMARE_SPAWN_STALKER)))
                        l_Stalker->RemoveAura(SPELL_NIGHTMARE_SPAWN_PERIODIC);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWISTING_SHADOWS_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WIND_BURN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EYE_OF_THE_STORM);

                    instance->DoRemoveForcedMovementsOnPlayers();
                }
            }

            void RemoveEventTrash()
            {
                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NECROTIC_VENOM_DEBUFF_01);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NECROTIC_VENOM_DEBUFF_02);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NECROTIC_VENOM_DEBUFF_03);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DRIPPING_FANGS_DEBUFF);
                }

                std::list<AreaTrigger*> l_List;

                me->GetAreaTriggerListWithSpellIDInRange(l_List, SPELL_VENOMOUS_POOL_AT, 300.0f);

                for (AreaTrigger* l_AT : l_List)
                    l_AT->Despawn();

                std::list<Creature*> l_CreatureList;

                me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_TWISTING_SHADOWS, 300.0f);
                me->GetCreatureListWithEntryInGridAppend(l_CreatureList, NPC_SKITTERING_SPIDERLING, 300.0f);
                me->GetCreatureListWithEntryInGridAppend(l_CreatureList, NPC_MYTHIC_VENOMOUS_SPIDERLING, 300.0f);
                me->GetCreatureListWithEntryInGridAppend(l_CreatureList, NPC_MYTHIC_SURGING_EGG_SAC, 300.0f);

                for (Creature* l_Creature : l_CreatureList)
                    l_Creature->DespawnOrUnsummon();
            }

            void EventsForm(uint8 p_Form, bool p_DelayRegen = false)
            {
                switch (p_Form)
                {
                    case SPIDER_FORM:
                    {
                        events.ScheduleEvent(EVENT_WEB_OF_PAIN, 6500);
                        events.ScheduleEvent(EVENT_NECROTIC_VENOM, 12000);
                        events.ScheduleEvent(EVENT_FEEDING_TIME, 16000);
                        break;
                    }
                    case DARKWING_FORM:
                    {
                        events.ScheduleEvent(EVENT_TWISTING_SHADOWS, 6600);
                        events.ScheduleEvent(EVENT_GATHERING_CLOUDS, 16000);
                        events.ScheduleEvent(EVENT_DARK_STORM, 26000);

                        /// First roc transform, schedule Violent Winds
                        if (IsMythic() && !m_PlatformID)
                        {
                            events.ScheduleEvent(EVENT_VIOLENT_WINDS, 50000);
                            events.ScheduleEvent(EVENT_RAKING_TALONS, 66000);
                            events.ScheduleEvent(EVENT_RAZOR_WING, 73000);
                        }
                        else
                        {
                            events.ScheduleEvent(EVENT_RAKING_TALONS, 52000);
                            events.ScheduleEvent(EVENT_RAZOR_WING, 59000);
                        }

                        break;
                    }
                    default:
                        break;
                }

                if (p_DelayRegen)
                {
                    AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(me, SPELL_ENERGY_TRACKER_TRANSFORM, true);
                    });
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_FEEDING_TIME:
                    {
                        m_CheckEvadeTimer = 20 * TimeConstants::IN_MILLISECONDS;
                        DoStopAttack();
                        me->SetReactState(REACT_PASSIVE);
                        me->SetDisableGravity(true);
                        me->GetMotionMaster()->MoveTakeoff(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 30.0f);
                        break;
                    }
                    case SPELL_ENERGY_TRANSFORM_COST:
                    {
                        events.Reset();

                        DoStopAttack();
                        me->SetReactState(REACT_PASSIVE);

                        me->RemoveAura(SPELL_ENERGY_TRACKER_TRANSFORM);

                        if (me->HasAura(SPELL_DARKWING_FORM))
                            events.ScheduleEvent(EVENT_SPIDER_TRANSFORM, 0);
                        else
                            events.ScheduleEvent(EVENT_DARKWING_FORM, 0);

                        break;
                    }
                    case SPELL_SPIDER_TRANSFORM_DUMMY:
                    case SPELL_DARKWING_FORM_DUMMY:
                    {
                        events.ScheduleEvent(EVENT_REACT_AGGRESSIVE, 3000);
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
                    case SPELL_FEEDING_TIME_FILTER:
                    {
                        me->CastSpell(*p_Target, SPELL_FEEDING_TIME_SUM_STALKER, true);
                        me->GetMotionMaster()->MoveTakeoff(2, p_Target->GetPositionX(), p_Target->GetPositionY(), me->GetPositionZ());
                        break;
                    }
                    case SPELL_VILE_AMBUSH_FILTER:
                    {
                        DoCast(me, SPELL_VILE_AMBUSH_VISUAL_JUMP, true);
                        DoCast(p_Target, SPELL_VILE_AMBUSH_JUMP, true);
                        break;
                    }
                    case SPELL_RAZOR_WING_FILTER:
                    {
                        DoCast(p_Target, SPELL_RAZOR_WING);
                        break;
                    }
                    case SPELL_RAZOR_WING:
                    {
                        DoCast(p_Target, SPELL_RAZOR_WING_VISUAL, true);
                        DoCast(p_Target, SPELL_RAZOR_WING_DMG, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_VIOLENT_WINDS)
                {
                    m_ViolentWinds = false;

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();

                        if (Unit* l_Target = me->getVictim())
                            AttackStart(l_Target);
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    switch (p_ID)
                    {
                        case 1:
                        {
                            uint8 l_SpiderPosID = 0;
                            if (m_PlatformID == 1)
                                l_SpiderPosID = 4;
                            else if (m_PlatformID == 2)
                                l_SpiderPosID = 8;

                            for (uint8 l_I = l_SpiderPosID; l_I < 4 + l_SpiderPosID; l_I++)
                                me->SummonCreature(NPC_VENOMOUS_SPIDERLING, g_SpiderPos[l_I]);

                            DoCast(me, SPELL_FEEDING_TIME_FILTER, true);
                            break;
                        }
                        case 2:
                        {
                            DoCast(me, SPELL_VILE_AMBUSH_FILTER, true);
                            break;
                        }
                        case SPELL_VILE_AMBUSH_JUMP_TRIGGERED:
                        {
                            me->SetAnimTier(0);
                            me->SetDisableGravity(false);
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (IsMythic() && m_PlatformID == 0)
                                DoCast(me, SPELL_TANGLED_WEB_AREATRIGGER);

                            break;
                        }
                        case SPELL_VIOLENT_WINDS_JUMP:
                        {
                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                uint64 l_Guid = 0;

                                if (Unit* l_Target = me->getVictim())
                                {
                                    l_Guid = l_Target->GetGUID();

                                    me->SetFacingToObject(l_Target);
                                }

                                AddTimedDelayedOperation(10, [this, l_Guid]() -> void
                                {
                                    if (Unit* l_Target = Unit::GetUnit(*me, l_Guid))
                                        DoCast(l_Target, SPELL_VIOLENT_WINDS);
                                });
                            });

                            break;
                        }
                        default:
                            break;
                    }
                }

                if (p_Type == WAYPOINT_MOTION_TYPE)
                {
                    if (p_ID == 4)
                    {
                        m_IsFlying = false;

                        SetFlyMode(false);
                        DoCast(me, SPELL_DARK_STORM_AT, true);
                        m_CheckEvadeTimer = 2000;
                        events.ScheduleEvent(EVENT_CHECK_PLAYER, 1000);

                        if (IsMythic())
                        {
                            switch (m_PlatformID)
                            {
                                case 1:
                                {
                                    DoCast(SPELL_VIOLENT_WINDS_ALLOWING, true);
                                    break;
                                }
                                case 2:
                                {
                                    if (instance != nullptr)
                                    {
                                        if (Creature* l_Stalker = Creature::GetCreature(*me, instance->GetData64(NPC_NIGHTMARE_SPAWN_STALKER)))
                                            l_Stalker->CastSpell(l_Stalker, SPELL_NIGHTMARE_SPAWN_PERIODIC, true);
                                    }

                                    break;
                                }
                                default:
                                    break;
                            }
                        }

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();

                            if (Unit* l_Target = me->getVictim())
                                AttackStart(l_Target);
                        });
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_DARK_STORM_DAMAGE)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        DoCast(me, SPELL_FOUL_WINDS_STACK, true);
                    });
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                if (!m_IsFlying)
                    return;

                p_Velocity = baseMoveSpeed[MOVE_FLIGHT] * 5.0f;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                m_ViolentWindsCount = p_Value;
            }

            uint32 GetData(uint32 p_ID) override
            {
                return m_ViolentWindsCount;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_BerserkTimer)
                {
                    if (m_BerserkTimer <= p_Diff)
                    {
                        m_BerserkTimer = 0;
                        DoCast(me, SPELL_BERSERK, true);
                    }
                    else
                        m_BerserkTimer -= p_Diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || m_ViolentWinds)
                    return;

                if (m_CheckEvadeTimer)
                {
                    if (m_CheckEvadeTimer <= p_Diff)
                    {
                        m_CheckEvadeTimer = 2000;
                        if (me->GetDistance(g_PlatformPos[m_PlatformID]) > 45.0f)
                            EnterEvadeMode();
                    }
                    else
                        m_CheckEvadeTimer -= p_Diff;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_REACT_AGGRESSIVE:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    case EVENT_SPIDER_TRANSFORM:
                    {
                        me->RemoveAurasDueToSpell(SPELL_DARKWING_FORM);
                        me->RemoveAurasDueToSpell(SPELL_DARK_STORM_AURA);
                        me->RemoveAurasDueToSpell(SPELL_FOUL_WINDS_STACK);
                        me->RemoveAreaTrigger(SPELL_DARK_STORM_AT);
                        DoCast(me, SPELL_SPIDER_TRANSFORM, true);
                        DoCast(SPELL_SPIDER_TRANSFORM_DUMMY);
                        EventsForm(SPIDER_FORM, true);

                        std::list<Creature*> l_Triggers;
                        auto& l_TriggerList = me->GetMap()->GetObjectsOnMap();
                        for (auto l_Object = l_TriggerList.begin(); l_Object != l_TriggerList.end(); ++l_Object)
                        {
                            if ((*l_Object)->GetEntry() != NPC_FLIGHT_LOC_STALKER || (*l_Object)->ToCreature() == nullptr)
                                continue;

                            l_Triggers.push_back((*l_Object)->ToCreature());
                        }

                        for (Creature* l_Trigger : l_Triggers)
                            l_Trigger->RemoveAura(SPELL_DARK_STORM_COSMETIC_AURA);

                        if (instance != nullptr)
                        {
                            instance->SendWeatherToPlayers(WeatherState::WEATHER_STATE_FOG, 0.0f);

                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EYE_OF_THE_STORM);
                        }

                        break;
                    }
                    case EVENT_DARKWING_FORM:
                    {
                        me->RemoveAurasDueToSpell(SPELL_SPIDER_TRANSFORM);
                        DoCast(me, SPELL_DARKWING_FORM, true);
                        DoCast(SPELL_DARKWING_FORM_DUMMY);
                        EventsForm(DARKWING_FORM, true);
                        break;
                    }
                    case EVENT_WEB_OF_PAIN:
                    {
                        DoCast(SPELL_WEB_OF_PAIN_FILTER);
                        break;
                    }
                    case EVENT_FEEDING_TIME:
                    {
                        Talk(SAY_FEEDING_TIME);
                        Talk(SAY_FEEDING_TIME_1);
                        DoCast(SPELL_FEEDING_TIME);
                        events.ScheduleEvent(EVENT_FEEDING_TIME, 50000);
                        break;
                    }
                    case EVENT_NECROTIC_VENOM:
                    {
                        DoCast(me, SPELL_NECROTIC_VENOM, true);
                        events.ScheduleEvent(EVENT_NECROTIC_VENOM, 22000);
                        break;
                    }
                    case EVENT_TWISTING_SHADOWS:
                    {
                        DoCast(SPELL_TWISTING_SHADOWS_FILTER);
                        events.ScheduleEvent(EVENT_TWISTING_SHADOWS, 40000);
                        break;
                    }
                    case EVENT_GATHERING_CLOUDS:
                    {
                        Talk(SAY_GATHERING_CLOUDS);
                        Talk(SAY_GATHERING_CLOUDS_1);

                        me->SetReactState(REACT_PASSIVE);

                        DoCast(SPELL_GATHERING_CLOUDS);

                        AddTimedDelayedOperation(10500, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                        });

                        events.ScheduleEvent(EVENT_SUM_SHIMMERING_FEATHER, 8000);
                        break;
                    }
                    case EVENT_SUM_SHIMMERING_FEATHER:
                    {
                        Position l_Pos;
                        float l_Angle = -1.0f;
                        for (uint8 l_I = 0; l_I < 8; l_I++)
                        {
                            me->GetNearPosition(l_Pos, 15.0f, l_Angle);
                            me->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), SPELL_SHIMMERING_FEATHER_TRIG, true);
                            l_Angle += 0.25f;
                        }
                        break;
                    }
                    case EVENT_RAZOR_WING:
                    {
                        Talk(SAY_RAZOR_WING);
                        Talk(SAY_RAZOR_WING_1);
                        DoCast(me, SPELL_RAZOR_WING_FILTER, true);
                        events.ScheduleEvent(EVENT_RAZOR_WING, 32000);
                        break;
                    }
                    case EVENT_DARK_STORM:
                    {
                        for (uint8 l_I = 0; l_I < 5; l_I++)
                            me->SummonCreature(NPC_TWISTING_SHADOWS_BRIDGE, g_TwistBridgePos[l_I]);
                        DoCast(SPELL_DARK_STORM_FILTER);
                        events.ScheduleEvent(EVENT_NEXT_PLATFORM, 4100);
                        break;
                    }
                    case EVENT_RAKING_TALONS:
                    {
                        DoCastVictim(SPELL_RAKING_TALONS);
                        events.ScheduleEvent(EVENT_RAKING_TALONS, 30000);
                        events.ScheduleEvent(EVENT_RAKING_TALONS_2, 4000);
                        break;
                    }
                    case EVENT_RAKING_TALONS_2:
                    {
                        DoCastVictim(SPELL_RAKING_TALONS_2);
                        break;
                    }
                    case EVENT_NEXT_PLATFORM:
                    {
                        m_IsFlying = true;

                        me->RemoveAura(SPELL_VIOLENT_WINDS_ALLOWING);

                        me->SetReactState(REACT_PASSIVE);
                        DoStopAttack();
                        me->StopMoving();
                        m_CheckEvadeTimer = 0;
                        DoCast(me, SPELL_DARK_STORM_AURA, true);
                        SetFlyMode(true);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePath(me->GetEntry() * 100 + m_PlatformID, false);

                        std::list<Creature*> l_Triggers;
                        auto& l_TriggerList = me->GetMap()->GetObjectsOnMap();
                        for (auto l_Object = l_TriggerList.begin(); l_Object != l_TriggerList.end(); ++l_Object)
                        {
                            if ((*l_Object)->GetEntry() != NPC_FLIGHT_LOC_STALKER || (*l_Object)->ToCreature() == nullptr)
                                continue;

                            /// Don't deal damage on targeted platform, it's the Eye of the storm
                            if ((*l_Object)->IsNearPosition(&g_PlatformTriggerPos[m_PlatformID], 0.5f))
                                continue;

                            l_Triggers.push_back((*l_Object)->ToCreature());
                        }

                        for (Creature* l_Trigger : l_Triggers)
                            l_Trigger->CastSpell(l_Trigger, SPELL_DARK_STORM_COSMETIC_AURA, true);

                        if (instance != nullptr)
                            instance->SendWeatherToPlayers(WeatherState(21), 1.0f);

                        m_PlatformID++;

                        if (m_PlatformID >= MAX_PLATFORMS)
                            m_PlatformID = 0;

                        break;
                    }
                    case EVENT_CHECK_PLAYER:
                    {
                        if (me->FindNearestPlayer(30.0f) != nullptr)
                        {
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Unit* l_Target = me->getVictim())
                            {
                                AttackStart(l_Target);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Target);
                            }
                        }
                        else
                            events.ScheduleEvent(EVENT_CHECK_PLAYER, 1000);

                        break;
                    }
                    case EVENT_VIOLENT_WINDS:
                    {
                        if (m_PlatformID == 1)
                        {
                            m_ViolentWinds = true;

                            if (Creature* l_Tracker = me->FindNearestCreature(NPC_FLIGHT_LOC_STALKER, 70.0f))
                            {
                                Position l_Pos = l_Tracker->GetPosition();

                                me->GetRandomNearPosition(l_Pos, 20.0f);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 10.0f, SPELL_VIOLENT_WINDS_JUMP);
                            }
                        }

                        events.ScheduleEvent(EVENT_VIOLENT_WINDS, 40500);
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
            return new boss_elerethe_renferalAI(p_Creature);
        }
};

/// Venomous Spiderling - 107459
/// Venomous Spiderling - 106311
class npc_elerethe_venomous_spiderling : public CreatureScript
{
    public:
        npc_elerethe_venomous_spiderling() : CreatureScript("npc_elerethe_venomous_spiderling") { }

        struct npc_elerethe_venomous_spiderlingAI : public ScriptedAI
        {
            npc_elerethe_venomous_spiderlingAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                if (me->GetEntry() != NPC_MYTHIC_VENOMOUS_SPIDERLING)
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void Reset() override
            {
                events.Reset();

                /// Update health mod
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetEntry() == NPC_MYTHIC_VENOMOUS_SPIDERLING)
                    return;

                me->SetDisableGravity(true);
                events.ScheduleEvent(EVENT_1, 500);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (me->GetEntry() == NPC_MYTHIC_VENOMOUS_SPIDERLING)
                {
                    if (IsMythic())
                        DoCast(me, SPELL_SWARM, true); ///< Swarm

                    return;
                }

                if (p_Type != POINT_MOTION_TYPE)
                    return;

                me->SetDisableGravity(false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 120.0f);

                if (IsMythic())
                    DoCast(me, SPELL_SWARM, true); ///< Swarm
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 1.4f;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->ToTempSummon())
                {
                    if (Unit* l_Summoner = me->ToTempSummon()->GetSummoner())
                        me->CastSpell(me, SPELL_VENOMOUS_POOL_AT, true, nullptr, nullptr, l_Summoner->GetGUID());
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() && me->isInCombat())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 32.0f);
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
            return new npc_elerethe_venomous_spiderlingAI(p_Creature);
        }
};

/// Twisting Shadows - 106350, 107767, 111439
class npc_elerethe_twisting_shadows : public CreatureScript
{
    public:
        npc_elerethe_twisting_shadows() : CreatureScript("npc_elerethe_twisting_shadows") { }

        struct npc_elerethe_twisting_shadowsAI : public ScriptedAI
        {
            npc_elerethe_twisting_shadowsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            uint64 m_TargetGuid = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetEntry() == NPC_TWISTING_PASSENGER)
                    return;

                if (me->GetEntry() == NPC_TWISTING_SHADOWS_BRIDGE)
                {
                    for (uint8 l_I = 0; l_I < 5; l_I++)
                    {
                        if (me->GetDistance(g_TwistBridgePos[l_I]) < 5.0f)
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePath((me->GetEntry() * 100 + l_I), true);
                            break;
                        }
                    }
                }
                else
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveRandom(15.0f);
                }

                DoCast(me, SPELL_TWISTING_SHADOWS_AT, true);
                events.ScheduleEvent(EVENT_2, 100);
            }

            void PassengerBoarded(Unit* p_Who, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (me->GetEntry() != NPC_TWISTING_PASSENGER || p_Apply)
                    return;

                m_TargetGuid = p_Who->GetGUID();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, m_TargetGuid))
                            l_Player->KnockbackFrom(me->GetPositionX(), me->GetPositionY(), 30.0f, 10.0f);

                        break;
                    }
                    case EVENT_2:
                    {
                        std::vector<uint32> l_Spells = { SPELL_VENOMOUS_POOL_AT, SPELL_VENOMOUS_POOL_AT_2 };

                        for (uint32 l_Spell : l_Spells)
                        {
                            std::list<AreaTrigger*> l_ATList;
                            me->GetAreaTriggerListWithSpellIDInRange(l_ATList, l_Spell, 5.0f);

                            for (AreaTrigger* l_AT : l_ATList)
                            {
                                l_AT->SetDuration(0);

                                if (InstanceScript* l_Instance = me->GetInstanceScript())
                                {
                                    if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(NPC_ELERETHE_RENFERAL)))
                                    {
                                        boss_elerethe_renferal::boss_elerethe_renferalAI* l_AI = CAST_AI(boss_elerethe_renferal::boss_elerethe_renferalAI, l_Boss->GetAI());
                                        if (l_AI != nullptr)
                                        {
                                            for (uint8 l_I = 0; l_I < 2; ++l_I)
                                            {
                                                if (Unit* l_Target = l_AI->SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                                    me->CastSpell(l_Target, SPELL_VENOMOUS_POOL_MISSILE, true);
                                            }
                                        }
                                    }
                                }
                            }
                        }

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
            return new npc_elerethe_twisting_shadowsAI(p_Creature);
        }
};

/// Surging Egg Sac - 108540
/// Surging Egg Sac - 109855
class npc_elerethe_surging_egg_sac : public CreatureScript
{
    public:
        npc_elerethe_surging_egg_sac() : CreatureScript("npc_elerethe_surging_egg_sac") { }

        struct npc_elerethe_surging_egg_sacAI : public ScriptedAI
        {
            npc_elerethe_surging_egg_sacAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                m_AnimSwitch = false;
            }

            InstanceScript* m_Instance;
            bool m_AnimSwitch;
            uint8 m_ScaleCount;

            void Reset() override
            {
                if (me->GetEntry() == NPC_MYTHIC_SURGING_EGG_SAC)
                {
                    me->SetUInt32Value(UNIT_FIELD_SCALE_DURATION, 2000);

                    DoCast(SPELL_GROWING_EGG_SAC_COSMETIC, true);

                    m_ScaleCount = 0;
                }

                events.ScheduleEvent(EVENT_1, urand(1000, 5000));
                events.ScheduleEvent(EVENT_2, 500);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_EGG_DUMMY_1:
                    {
                        DoCast(me, SPELL_EGG_DUMMY_2, true);
                        me->PlayOneShotAnimKitId(ANIM_3);
                        break;
                    }
                    case SPELL_EGG_DUMMY_2:
                    {
                        for (uint8 l_I = 0; l_I < 4; l_I++)
                        {
                            Position l_Pos;
                            me->GetRandomNearPosition(l_Pos, 3.0f);
                            me->CastSpell(l_Pos, SPELL_SUM_SKITTERING_SPIDERLING, true);
                        }

                        me->DespawnOrUnsummon(3000);
                        break;
                    }
                    case SPELL_GROWING_EGG_SAC_SCALE:
                    {
                        ++m_ScaleCount;

                        if (m_ScaleCount >= 5)
                            DoCast(SPELL_GROWING_EGG_SCALE_FULL, true);

                        break;
                    }
                    case SPELL_VILE_AMBUSH_DAMAGE:
                    {
                        events.Reset();
                        DoCast(me, SPELL_EGG_DUMMY_1, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->SetMovementAnimKitId(m_AnimSwitch ? ANIM_1 : ANIM_2);
                        m_AnimSwitch = m_AnimSwitch ? false : true;
                        events.ScheduleEvent(EVENT_1, 2500);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (m_Instance && m_Instance->GetBossState(DATA_RENFERAL) == IN_PROGRESS)
                        {
                            if (Player* l_Player = me->FindNearestPlayer(5.0f))
                            {
                                if (l_Player->GetDistance(*me) < 2.5f)
                                {
                                    events.Reset();
                                    DoCast(me, SPELL_EGG_DUMMY_1, true);
                                    break;
                                }
                            }
                        }

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
            return new npc_elerethe_surging_egg_sacAI(p_Creature);
        }
};

/// Nightmare Spawn Stalker - 109849
class npc_elerethe_nightmare_spawn_stalker : public CreatureScript
{
    public:
        npc_elerethe_nightmare_spawn_stalker() : CreatureScript("npc_elerethe_nightmare_spawn_stalker") { }

        struct npc_elerethe_nightmare_spawn_stalkerAI : public ScriptedAI
        {
            npc_elerethe_nightmare_spawn_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_NIGHTMARE_SPAWN_DUMMY:
                    {
                        for (uint8 l_I = 0; l_I < 5; ++l_I)
                        {
                            Position l_Pos = me->GetPosition();

                            me->GetRandomNearPosition(l_Pos, 60.0f);

                            DoCast(l_Pos, SPELL_NIGHTMARE_SPAWN_MISSILE, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elerethe_nightmare_spawn_stalkerAI(p_Creature);
        }
};

/// Energy Tracker Initial Bird Transform - 225364
class spell_elerethe_energy_tracker_transform : public SpellScriptLoader
{
    public:
        spell_elerethe_energy_tracker_transform() : SpellScriptLoader("spell_elerethe_energy_tracker_transform") { }

        class spell_elerethe_energy_tracker_transform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_energy_tracker_transform_AuraScript);

            enum eDisplay
            {
                SpiderForm = 69474
            };

            uint8 m_PowerTick = 0;
            uint32 m_ApplyTime = 0;

            bool Load() override
            {
                m_ApplyTime = uint32(time(nullptr));
                return true;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                int32 l_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (l_PowerCount < 100)
                {
                    /// Phase 1 takes 90s
                    if (l_Caster->GetDisplayId() == eDisplay::SpiderForm)
                    {
                        if ((m_PowerTick < 9 && l_PowerCount < 9) || (m_PowerTick < 8 && l_PowerCount < 29) || (m_PowerTick < 7 && l_PowerCount < 38) || (m_PowerTick < 8 && l_PowerCount >= 40))
                        {
                            m_PowerTick++;
                            l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, 1, POWER_ENERGY);
                        }
                        else
                        {
                            m_PowerTick = 0;
                            l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, 2, POWER_ENERGY);
                        }
                    }
                    /// Phase 2 takes 120s
                    else
                    {
                        uint32 l_CurrTime   = uint32(time(nullptr));
                        int32 l_Power       = std::min<uint32>((l_CurrTime - m_ApplyTime) * 100 / 120, 100);

                        if (l_Power <= l_Caster->GetPower(POWER_ENERGY))
                            return;

                        l_Caster->SetPower(POWER_ENERGY, l_Power);
                    }
                }
                else if (!l_Caster->HasUnitState(UNIT_STATE_CASTING))
                    l_Caster->CastSpell(l_Caster, SPELL_ENERGY_TRANSFORM_COST);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_elerethe_energy_tracker_transform_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_energy_tracker_transform_AuraScript();
        }
};

/// Web of Pain - 215288
class spell_elerethe_web_of_pain_filter : public SpellScriptLoader
{
    public:
        spell_elerethe_web_of_pain_filter() : SpellScriptLoader("spell_elerethe_web_of_pain_filter") { }

        class spell_elerethe_web_of_pain_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_web_of_pain_filter_SpellScript);

            uint64 m_SecondTarget = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                std::list<WorldObject*> l_List;

                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([&](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                    {
                        l_List.push_back(p_Object);
                        return true;
                    }

                    return false;
                });

                if (p_Targets.size() > 2)
                    JadeCore::RandomResizeList(p_Targets, 2);

                uint64 l_SecondTarget = 0;

                if (l_List.size() > 2)
                    JadeCore::RandomResizeList(l_List, 2);

                if (l_List.size() > 1)
                {
                    /// Tanks
                    for (WorldObject* l_Object : l_List)
                    {
                        if (Player* l_Player = l_Object->ToPlayer())
                        {
                            if (!l_SecondTarget)
                                l_SecondTarget = l_Object->GetGUID();
                            else if (l_SecondTarget)
                            {
                                if (Unit* l_Target = Unit::GetUnit(*l_Object, l_SecondTarget))
                                {
                                    l_Target->CastSpell(l_Player, SPELL_WEB_OF_PAIN_SHARE_AURA_01, true);
                                    l_Player->CastSpell(l_Target, SPELL_WEB_OF_PAIN_SHARE_AURA_02, true);
                                }
                            }
                        }
                    }
                }

                l_SecondTarget = 0;

                if (p_Targets.size() > 1)
                {
                    /// Random players
                    for (WorldObject* l_Object : p_Targets)
                    {
                        if (Player* l_Player = l_Object->ToPlayer())
                        {
                            if (!l_SecondTarget)
                                l_SecondTarget = l_Object->GetGUID();
                            else if (l_SecondTarget)
                            {
                                if (Unit* l_Target = Unit::GetUnit(*l_Object, l_SecondTarget))
                                {
                                    l_Target->CastSpell(l_Player, SPELL_WEB_OF_PAIN_SHARE_AURA_01, true);
                                    l_Player->CastSpell(l_Target, SPELL_WEB_OF_PAIN_SHARE_AURA_02, true);
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elerethe_web_of_pain_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_web_of_pain_filter_SpellScript();
        }
};

/// Web of Pain - 215300, 215307
class spell_elerethe_web_of_pain : public SpellScriptLoader
{
    public:
        spell_elerethe_web_of_pain() : SpellScriptLoader("spell_elerethe_web_of_pain") { }

        class spell_elerethe_web_of_pain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_web_of_pain_AuraScript);

            void OnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                int32 l_ReflectionPct   = p_AuraEffect->GetAmount();
                int32 l_Damage          = CalculatePct(p_DmgInfo.GetAmount(), l_ReflectionPct);

                p_AbsorbAmount = 0;

                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                if (p_DmgInfo.GetSpellInfo() && p_DmgInfo.GetSpellInfo()->Id == SPELL_WEB_OF_PAIN_DAMAGE)
                    return;

                Creature* l_Boss = nullptr;

                if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    l_Boss = Creature::GetCreature(*l_Target, l_Instance->GetData64(NPC_ELERETHE_RENFERAL));

                if (!l_Boss)
                    return;

                l_Target->CastCustomSpell(l_Caster, SPELL_WEB_OF_PAIN_DAMAGE, &l_Damage, nullptr, nullptr, true, nullptr, nullptr, l_Boss->GetGUID());
            }

            uint16 m_CheckTimer = 500;

            void OnUpdate(uint32 p_Diff, AuraEffect* p_AurEff)
            {
                if (m_CheckTimer <= p_Diff)
                {
                    Unit* l_Caster = GetCaster();

                    m_CheckTimer = 500;

                    if (!l_Caster || !l_Caster->isAlive())
                        p_AurEff->GetBase()->Remove();

                    float l_BaseDist    = 20.0f;
                    Unit* l_Target      = GetUnitOwner();

                    if (l_Target == nullptr || l_Caster == l_Target || !l_Caster)
                        return;

                    float l_Dist = l_Caster->GetDistance(l_Target);
                    if (l_Dist <= l_BaseDist)
                    {
                        /// Reset amount to 100% if 20yards or less
                        p_AurEff->ChangeAmount(100.0f);
                        return;
                    }

                    int32 l_Pct     = 100.0f;
                    float l_AddPct  = (l_Dist - l_BaseDist) / l_BaseDist * 100.0f;

                    AddPct(l_Pct, int32(l_AddPct));

                    p_AurEff->ChangeAmount(l_Pct);
                }
                else
                    m_CheckTimer -= p_Diff;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->GetDistance(l_Target) > 20.0f)
                {
                    l_Caster->CastSpell(l_Target, SPELl_WEB_OF_PAIN_FAR_VISUAL, true);
                    l_Target->CastSpell(l_Caster, SPELl_WEB_OF_PAIN_FAR_VISUAL, true);
                }

                if (!l_Caster->GetMap()->IsMythic())
                    return;

                Map::PlayerList const& l_Players = l_Caster->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->GetGUID() == l_Caster->GetGUID() || l_Player->GetGUID() == l_Target->GetGUID())
                            continue;

                        if (l_Player->IsInBetween(l_Caster, l_Target, 1.0f))
                            l_Player->CastSpell(l_Player, SPELL_PAIN_LASH, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_elerethe_web_of_pain_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectUpdate += AuraEffectUpdateFn(spell_elerethe_web_of_pain_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);

                switch (m_scriptSpellId)
                {
                    case SPELL_WEB_OF_PAIN_SHARE_AURA_01:
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elerethe_web_of_pain_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                        break;
                    default:
                        OnEffectUpdate += AuraEffectUpdateFn(spell_elerethe_web_of_pain_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_SHARE_DAMAGE_PCT);
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_web_of_pain_AuraScript();
        }
};

/// Vile Ambush - 214348
class spell_elerethe_vile_ambush : public SpellScriptLoader
{
    public:
        spell_elerethe_vile_ambush() : SpellScriptLoader("spell_elerethe_vile_ambush") { }

        class spell_elerethe_vile_ambush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_vile_ambush_SpellScript);

            void DealDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float l_Distance = GetCaster()->GetExactDist2d(GetHitUnit());
                float l_Radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster());
                uint32 l_Percent;
                if (l_Distance < 15.0f)
                    l_Percent = 100 - (l_Distance / 10) * 30;
                else
                    l_Percent = 20 - (l_Distance / l_Radius) * 30;

                uint32 l_Damage = CalculatePct(GetHitDamage(), l_Percent);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_elerethe_vile_ambush_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_vile_ambush_SpellScript();
        }
};

/// Feeding Time - 214311
class spell_elerethe_feeding_time_filter : public SpellScriptLoader
{
    public:
        spell_elerethe_feeding_time_filter() : SpellScriptLoader("spell_elerethe_feeding_time_filter") { }

        class spell_elerethe_feeding_time_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_feeding_time_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                {
                    if (Creature* l_Caster = GetCaster()->ToCreature())
                    {
                        if (l_Caster->IsAIEnabled)
                            l_Caster->AI()->EnterEvadeMode();
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elerethe_feeding_time_filter_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_feeding_time_filter_SpellScript();
        }
};

/// Necrotic Venom - 215449
class spell_elerethe_necrotic_venom : public SpellScriptLoader
{
    public:
        spell_elerethe_necrotic_venom() : SpellScriptLoader("spell_elerethe_necrotic_venom") { }

        class spell_elerethe_necrotic_venom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_necrotic_venom_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastSpell(GetTarget(), GetSpellInfo()->Effects[EFFECT_0].BasePoints, true, nullptr, nullptr, GetCaster()->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_elerethe_necrotic_venom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_necrotic_venom_AuraScript();
        }
};

/// Shimmering Feather - 212993
class spell_elerethe_shimmering_feather_proc : public SpellScriptLoader
{
    public:
        spell_elerethe_shimmering_feather_proc() : SpellScriptLoader("spell_elerethe_shimmering_feather_proc") { }

        class spell_elerethe_shimmering_feather_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_shimmering_feather_proc_AuraScript);

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                if (GetCaster())
                    GetCaster()->CastSpell(GetCaster(), SPELL_SHIMMERING_FEATHER_OVERRIDE, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_elerethe_shimmering_feather_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_shimmering_feather_proc_AuraScript();
        }
};

/// Dark Storm Cosmetic - 218536
class spell_elerethe_dark_storm_cosmetic : public SpellScriptLoader
{
    public:
        spell_elerethe_dark_storm_cosmetic() : SpellScriptLoader("spell_elerethe_dark_storm_cosmetic") { }

        class spell_elerethe_dark_storm_cosmetic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_dark_storm_cosmetic_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    for (uint8 l_I = 0; l_I < 3; ++l_I)
                    {
                        Position l_Pos;

                        l_Caster->GetRandomNearPosition(l_Pos, 45.0f);

                        l_Caster->CastSpell(l_Pos, SPELL_DARK_STORM_COSMETIC_DUMMY, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_elerethe_dark_storm_cosmetic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_dark_storm_cosmetic_AuraScript();
        }
};

/// Gathering Clouds (periodic damage) - 212709
class spell_elerethe_gathering_clouds_dmg : public SpellScriptLoader
{
    public:
        spell_elerethe_gathering_clouds_dmg() : SpellScriptLoader("spell_elerethe_gathering_clouds_dmg") { }

        class spell_elerethe_gathering_clouds_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_gathering_clouds_dmg_SpellScript);

            void DealDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float l_Distance    = GetHitUnit()->GetDistance(GetCaster());
                float l_Percent     = 0.0f;

                /// 100% damage at 40 yards or more
                if (l_Distance >= 20.0f)
                    return;

                l_Percent = l_Distance * 100.0f / 20.0f;

                if (l_Percent < 20.0f)
                    l_Percent = 20.0f;

                uint32 l_Damage = CalculatePct(GetHitDamage(), l_Percent);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elerethe_gathering_clouds_dmg_SpellScript::DealDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_gathering_clouds_dmg_SpellScript();
        }
};

/// Violent Winds - 225149
class spell_elerethe_violent_winds_filter : public SpellScriptLoader
{
    public:
        spell_elerethe_violent_winds_filter() : SpellScriptLoader("spell_elerethe_violent_winds_filter") { }

        class spell_elerethe_violent_winds_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_violent_winds_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || p_Targets.empty())
                    return;

                Creature* l_Boss = nullptr;

                if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                    l_Boss = Creature::GetCreature(*l_Caster, l_Instance->GetData64(NPC_ELERETHE_RENFERAL));

                Unit* l_MainTarget = nullptr;
                if (l_Boss != nullptr && l_Boss->IsAIEnabled && (l_MainTarget = l_Boss->AI()->SelectTarget(SELECT_TARGET_TOPAGGRO)))
                {
                    uint8  l_Stacks     = 20;
                    bool   l_TankFound  = false;

                    l_Boss->AI()->SetData(0, p_Targets.size());

                    for (WorldObject* l_Obj : p_Targets)
                    {
                        if (Unit* l_Target = l_Obj->ToUnit())
                        {
                            if (l_Target->GetGUID() == l_MainTarget->GetGUID())
                            {
                                l_TankFound = true;
                                continue;
                            }

                            --l_Stacks;
                            l_Boss->CastSpell(l_Target, SPELL_WIND_BURN, true);
                        }
                    }

                    if (l_TankFound)
                    {
                        for (uint8 l_I = 0; l_I < l_Stacks; ++l_I)
                            l_Boss->CastSpell(l_MainTarget, SPELL_WIND_BURN, true);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elerethe_violent_winds_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_violent_winds_filter_SpellScript();
        }
};

/// Violent Winds Cosmetic - 225437
class spell_elerethe_violent_winds_cosmetic : public SpellScriptLoader
{
    public:
        spell_elerethe_violent_winds_cosmetic() : SpellScriptLoader("spell_elerethe_violent_winds_cosmetic") { }

        class spell_elerethe_violent_winds_cosmetic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_violent_winds_cosmetic_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, SPELL_VIOLENT_WINDS_COSMETIC, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_elerethe_violent_winds_cosmetic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_violent_winds_cosmetic_AuraScript();
        }
};

/// Violent Winds - 218141
class spell_elerethe_violent_winds_speed : public SpellScriptLoader
{
    public:
        spell_elerethe_violent_winds_speed() : SpellScriptLoader("spell_elerethe_violent_winds_speed") { }

        class spell_elerethe_violent_winds_speed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elerethe_violent_winds_speed_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Creature* l_Boss = l_Caster->ToCreature())
                {
                    if (l_Boss->IsAIEnabled && l_Boss->AI()->GetData(0) > 1)
                        p_Amount = p_Amount / 10 * l_Boss->AI()->GetData(0);
                    else
                        p_Amount = 0.0f;

                    if (p_Amount < -100)
                        p_Amount = -100;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_elerethe_violent_winds_speed_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_MOVEMENT_FORCE_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elerethe_violent_winds_speed_AuraScript();
        }
};

/// Dark Storm - 213936
class spell_elerethe_dark_storm_damage : public SpellScriptLoader
{
    public:
        spell_elerethe_dark_storm_damage() : SpellScriptLoader("spell_elerethe_dark_storm_damage") { }

        class spell_elerethe_dark_storm_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elerethe_dark_storm_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || p_Targets.empty())
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || p_Object->GetDistance(l_Caster) <= 40.0f)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elerethe_dark_storm_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elerethe_dark_storm_damage_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_elerethe_renferal()
{
    /// Boss
    new boss_elerethe_renferal();

    /// Creatures
    new npc_elerethe_venomous_spiderling();
    new npc_elerethe_twisting_shadows();
    new npc_elerethe_surging_egg_sac();
    new npc_elerethe_nightmare_spawn_stalker();

    /// Spells
    new spell_elerethe_energy_tracker_transform();
    new spell_elerethe_web_of_pain_filter();
    new spell_elerethe_web_of_pain();
    new spell_elerethe_vile_ambush();
    new spell_elerethe_feeding_time_filter();
    new spell_elerethe_necrotic_venom();
    new spell_elerethe_shimmering_feather_proc();
    new spell_elerethe_dark_storm_cosmetic();
    new spell_elerethe_gathering_clouds_dmg();
    new spell_elerethe_violent_winds_filter();
    new spell_elerethe_violent_winds_cosmetic();
    new spell_elerethe_violent_winds_speed();
    new spell_elerethe_dark_storm_damage();
}
#endif
