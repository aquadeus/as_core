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
    SAY_AGGRO           = 0,
    SAY_ROT             = 1,
    SAY_BREATH_EMOTE    = 2,
    SAY_BREATH          = 3,
    SAY_PHASE           = 4,
    SAY_DEATH           = 5,
    SAY_BERSERK         = 6,
    SAY_WIPE            = 7
};

enum Spells
{
    SPELL_ALLOW_ROT_CAST            = 204755,
    SPELL_HEART_SWARM               = 203552,
    SPELL_ROT_FILTER                = 203095,
    SPELL_ROT_DOT                   = 203096,
    SPELL_ROT_DUMMY                 = 203536,
    SPELL_VOLATILE_ROT              = 204463,
    SPELL_INFESTED_GROUND_AT        = 204377,
    SPELL_INFESTED_GROUND_AT_2      = 203044,
    SPELL_INFESTED_GROUND_VIS       = 204476,
    SPELL_INFESTED_GROUND_MISSILE   = 203537,
    SPELL_INFESTED_GROUND_DOT       = 203045,
    SPELL_INFESTED_BREATH_FILTER    = 202968,
    SPELL_INFESTED_BREATH_CHANNEL   = 202977,
    SPELL_INFESTED_BREATH_DMG       = 202978,
    SPELL_TRANSFORM_BUGS            = 204894,

    /// Heroic
    SPELL_TARGETING_AURA            = 220189,
    SPELL_INFESTED                  = 204504,

    /// Mythic
    SPELL_INFESTED_MIND_FILTER      = 225943,
    SPELL_INFESTED_MIND             = 205043,
    SPELL_SPREAD_INFESTATION        = 205070,

    /// Trash
    /// Gelatinized Decay - 111004
    SPELL_TRANSFORM_INSECT          = 220976,
    SPELL_WAVE_OF_DECAY             = 221059,
    SPELL_UNSTABLE_DECAY            = 221030,
    SPELL_PUTRID_SWARM              = 221033, ///< Aura summons Vermin

    /// Corrupted Vermin - 111005
    SPELL_HEART_SWARM_SCALE         = 203647,
    SPELL_PUTRID_SWARM_DMG          = 221036,

    /// Corrupted Vermin - 102998
    SPELL_SCALE                     = 196285,
    SPELL_BURST_OF_CORRUPTION       = 203646,

    /// Cosmetic
    SPELL_TAINTED_GROUND_01         = 221980,
    SPELL_TAINTED_GROUND_02         = 221967,
    SPELL_TAINTED_GROUND_03         = 220911
};

enum eEvents
{
    EVENT_CHECK_POWER           = 1,
    EVENT_ALLOW_ROT_CAST        = 2,
    EVENT_ROT                   = 3,
    EVENT_VOLATILE_ROT          = 4,
    EVENT_INFESTED_BREATH       = 5,
    EVENT_HEART_SWARM_END       = 6,
    EVENT_INFESTED_MIND         = 7  ///< Mythic
};

enum ePhases
{
    PHASE_BATTLE                = 1,
    PHASE_SWARM                 = 2
};

enum eOther
{
    DATA_SWARM_COUNTER          = 1,
    DATA_DECAY_COUNTER          = 5,
    DATA_TAINTED_GROUND         = 51
};

std::array<Position const, DATA_DECAY_COUNTER> g_DecayPos =
{
    {
        { 1910.29f, 1332.19f, 369.50f, 0.0f },
        { 1866.0f,  1332.56f, 369.50f, 0.0f },
        { 1851.89f, 1295.29f, 369.50f, 0.0f },
        { 1874.62f, 1275.05f, 369.50f, 0.0f },
        { 1919.84f, 1302.85f, 369.50f, 0.0f }
    }
};

struct TaintedGround
{
    uint32 SpellID;
    Position Pos;
};

std::array<TaintedGround, DATA_TAINTED_GROUND> g_TaintedGround =
{
    {
        { SPELL_TAINTED_GROUND_01, { 1855.811f, 1320.957f, 369.5015f, 2.742845f } },
        { SPELL_TAINTED_GROUND_01, { 1858.773f, 1320.335f, 369.5015f, 2.718412f } },
        { SPELL_TAINTED_GROUND_02, { 1858.773f, 1320.335f, 369.5015f, 2.700314f } },
        { SPELL_TAINTED_GROUND_02, { 1865.792f, 1319.760f, 369.5015f, 2.580464f } },
        { SPELL_TAINTED_GROUND_02, { 1868.915f, 1317.372f, 369.5015f, 2.607691f } },
        { SPELL_TAINTED_GROUND_03, { 1873.549f, 1315.063f, 369.5015f, 2.561869f } },
        { SPELL_TAINTED_GROUND_03, { 1877.990f, 1311.477f, 369.5015f, 2.793176f } },
        { SPELL_TAINTED_GROUND_02, { 1903.474f, 1329.307f, 369.5015f, 1.248556f } },
        { SPELL_TAINTED_GROUND_02, { 1900.193f, 1325.325f, 369.5015f, 1.360443f } },
        { SPELL_TAINTED_GROUND_01, { 1906.823f, 1330.887f, 369.5015f, 1.136822f } },
        { SPELL_TAINTED_GROUND_01, { 1909.373f, 1332.191f, 369.5015f, 1.069673f } },
        { SPELL_TAINTED_GROUND_02, { 1897.632f, 1320.498f, 369.5015f, 1.491837f } },
        { SPELL_TAINTED_GROUND_03, { 1884.903f, 1309.300f, 369.5015f, 6.000515f } },
        { SPELL_TAINTED_GROUND_03, { 1879.656f, 1305.035f, 369.5015f, 4.595711f } },
        { SPELL_TAINTED_GROUND_03, { 1895.429f, 1315.608f, 369.5015f, 1.768607f } },
        { SPELL_TAINTED_GROUND_03, { 1890.250f, 1311.276f, 369.5015f, 2.819204f } },
        { SPELL_TAINTED_GROUND_01, { 1847.564f, 1289.379f, 369.5015f, 3.075492f } },
        { SPELL_TAINTED_GROUND_03, { 1878.488f, 1301.616f, 369.5015f, 0.404819f } },
        { SPELL_TAINTED_GROUND_01, { 1850.333f, 1289.214f, 369.5015f, 6.242309f } },
        { SPELL_TAINTED_GROUND_03, { 1886.240f, 1304.760f, 369.5015f, 3.534745f } },
        { SPELL_TAINTED_GROUND_02, { 1854.217f, 1289.118f, 369.5015f, 6.256053f } },
        { SPELL_TAINTED_GROUND_03, { 1896.365f, 1309.771f, 369.5015f, 2.077643f } },
        { SPELL_TAINTED_GROUND_02, { 1859.542f, 1289.629f, 369.5015f, 0.038780f } },
        { SPELL_TAINTED_GROUND_03, { 1892.580f, 1306.290f, 369.5015f, 3.738506f } },
        { SPELL_TAINTED_GROUND_03, { 1877.557f, 1296.858f, 369.5015f, 0.269369f } },
        { SPELL_TAINTED_GROUND_02, { 1865.033f, 1291.007f, 369.5015f, 0.113883f } },
        { SPELL_TAINTED_GROUND_03, { 1882.806f, 1299.720f, 369.5015f, 4.939978f } },
        { SPELL_TAINTED_GROUND_03, { 1870.531f, 1293.113f, 369.5015f, 0.182915f } },
        { SPELL_TAINTED_GROUND_03, { 1891.085f, 1301.528f, 369.5015f, 3.227296f } },
        { SPELL_TAINTED_GROUND_03, { 1877.646f, 1292.755f, 369.5015f, 0.124631f } },
        { SPELL_TAINTED_GROUND_03, { 1882.290f, 1294.403f, 369.5015f, 2.337311f } },
        { SPELL_TAINTED_GROUND_03, { 1887.939f, 1297.611f, 369.5015f, 0.214953f } },
        { SPELL_TAINTED_GROUND_03, { 1897.349f, 1302.755f, 369.5015f, 3.200196f } },
        { SPELL_TAINTED_GROUND_03, { 1887.345f, 1292.083f, 369.5015f, 2.106977f } },
        { SPELL_TAINTED_GROUND_03, { 1896.809f, 1298.073f, 369.5015f, 3.375860f } },
        { SPELL_TAINTED_GROUND_03, { 1893.469f, 1294.477f, 369.5015f, 1.415363f } },
        { SPELL_TAINTED_GROUND_03, { 1902.556f, 1297.323f, 369.5015f, 3.473113f } },
        { SPELL_TAINTED_GROUND_03, { 1898.608f, 1291.748f, 369.5015f, 0.828552f } },
        { SPELL_TAINTED_GROUND_03, { 1891.788f, 1286.675f, 369.5015f, 1.617925f } },
        { SPELL_TAINTED_GROUND_03, { 1896.363f, 1287.872f, 369.5015f, 0.656433f } },
        { SPELL_TAINTED_GROUND_02, { 1908.210f, 1295.335f, 369.5015f, 3.694195f } },
        { SPELL_TAINTED_GROUND_02, { 1889.161f, 1281.859f, 369.5015f, 3.896219f } },
        { SPELL_TAINTED_GROUND_03, { 1904.813f, 1291.434f, 369.5015f, 3.764250f } },
        { SPELL_TAINTED_GROUND_02, { 1912.220f, 1296.573f, 369.5015f, 3.775522f } },
        { SPELL_TAINTED_GROUND_02, { 1916.075f, 1298.366f, 369.5015f, 3.870712f } },
        { SPELL_TAINTED_GROUND_01, { 1920.983f, 1302.379f, 369.5015f, 3.982058f } },
        { SPELL_TAINTED_GROUND_02, { 1885.915f, 1277.521f, 369.5015f, 3.998931f } },
        { SPELL_TAINTED_GROUND_01, { 1919.444f, 1300.347f, 369.5015f, 4.023560f } },
        { SPELL_TAINTED_GROUND_02, { 1882.009f, 1273.703f, 369.5015f, 3.967733f } },
        { SPELL_TAINTED_GROUND_01, { 1879.597f, 1271.698f, 369.5015f, 3.944333f } }
    }
};

/// Nythendra <Guardian of Shaladrassil> - 102672
class boss_nythendra : public CreatureScript
{
    public:
        boss_nythendra() : CreatureScript("boss_nythendra") { }

        struct boss_nythendraAI : public BossAI
        {
            boss_nythendraAI(Creature* p_Creature) : BossAI(p_Creature, DATA_NYTHENDRA)
            {
                m_Intro = true;
                m_BerserkerTimer = 0;
            }

            bool m_Intro;
            uint8 m_SwarmCount;
            uint8 m_RotCastCount;
            uint8 m_DecayDiedCount;

            int32 m_BerserkerTimer;

            void Intro()
            {
                m_Intro = false;
                m_DecayDiedCount = 0;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetStandState(UNIT_STAND_STATE_SLEEP);

                for (Position const l_Pos : g_DecayPos)
                    me->SummonCreature(NPC_GELATINIZED_DECAY, l_Pos);

                for (TaintedGround l_Data : g_TaintedGround)
                    me->CastSpell(l_Data.Pos, l_Data.SpellID, true);
            }

            void Reset() override
            {
                _Reset();
                RemoveBugs();

                me->RemoveAura(SPELL_BOSS_ENRAGE);

                m_SwarmCount = 0;

                if (m_Intro)
                    Intro();
                else
                {
                    me->SetAIAnimKitId(0);
                    me->SetReactState(REACT_DEFENSIVE);
                    me->SetPower(POWER_ENERGY, me->GetMaxPower(POWER_ENERGY));
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();

                events.ScheduleEvent(EVENT_CHECK_POWER, 5000);
                events.ScheduleEvent(EVENT_ALLOW_ROT_CAST, 1000);

                if (!IsLFR())
                    m_BerserkerTimer = IsHeroicOrMythic() ? 480 * IN_MILLISECONDS : 600 * IN_MILLISECONDS;

                DefaultEvents(true);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(DATA_NYTHENDRA, EncounterState::FAIL);

                Talk(SAY_WIPE);

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void DefaultEvents(bool p_EnterCombat)
            {
                events.ScheduleEvent(EVENT_ROT, p_EnterCombat ? 6000 : 14000);
                events.ScheduleEvent(EVENT_VOLATILE_ROT, p_EnterCombat ? 24000 : 35000);
                events.ScheduleEvent(EVENT_INFESTED_BREATH, (p_EnterCombat ? urand(35, 40) : 47) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                RemoveBugs();

                Talk(SAY_DEATH);

                Conversation* l_Conversation = new Conversation;
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3601, me, nullptr, *me))
                    delete l_Conversation;
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == NPC_GELATINIZED_DECAY)
                {
                    m_DecayDiedCount++;

                    if (m_DecayDiedCount >= DATA_DECAY_COUNTER)
                    {
                        me->SetReactState(REACT_DEFENSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        me->SetAIAnimKitId(0);
                        me->RemoveAllAreasTrigger();
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_INFESTED_BREATH_FILTER:
                    {
                        if (Creature* l_Stalker = me->SummonCreature(NPC_BREATH_STALKER, p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000))
                        {
                            for (uint8 l_I = 0; l_I < 5; l_I++)
                                me->m_Events.AddEvent(new DelayCastEvent(*me, me->GetGUID(), SPELL_INFESTED_BREATH_DMG, true), me->m_Events.CalculateTime((l_I + 3) * 1000));

                            me->SetFacingTo(me->GetAngle(l_Stalker));

                            me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                            DoCast(l_Stalker, SPELL_INFESTED_BREATH_CHANNEL);
                        }

                        break;
                    }
                    case SPELL_TRANSFORM_BUGS:
                    {
                        p_Target->SetVisible(true);
                        break;
                    }
                    case SPELL_INFESTED_MIND_FILTER:
                    {
                        if (Aura* l_Aura = p_Target->GetAura(SPELL_INFESTED))
                        {
                            if (l_Aura->GetStackAmount() >= 10)
                            {
                                DoCast(p_Target, SPELL_INFESTED_MIND, true);

                                p_Target->RemoveAurasDueToSpell(SPELL_ROT_DOT);
                                p_Target->RemoveAurasDueToSpell(SPELL_VOLATILE_ROT);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void RemoveBugs()
            {
                auto& l_BugList = me->GetMap()->GetObjectsOnMap();
                for (auto l_Object = l_BugList.begin(); l_Object != l_BugList.end(); ++l_Object)
                {
                    if ((*l_Object)->GetEntry() != NPC_CORRUPTED_VERMIN || (*l_Object)->ToCreature() == nullptr)
                        continue;

                    Creature* l_Bug = (*l_Object)->ToCreature();

                    l_Bug->RemoveAura(SPELL_TRANSFORM_BUGS);
                    l_Bug->RemoveAura(SPELL_HEART_SWARM_SCALE);

                    l_Bug->SetVisible(false);

                    if (l_Bug->IsAIEnabled)
                        l_Bug->AI()->Reset();
                }

                if (IsHeroic())
                {
                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();

                    for (HostileReference* l_Ref : l_ThreatList)
                    {
                        if (Player* l_Player = me->GetPlayer(*me, l_Ref->getUnitGuid()))
                        {
                            if (l_Player->HasAura(SPELL_INFESTED))
                                l_Player->RemoveAurasDueToSpell(SPELL_INFESTED);
                        }
                    }
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_SWARM_COUNTER:
                    {
                        uint32 l_Count = me->GetMap()->GetPlayersCountExceptGMs();
                        if (l_Count <= 10)
                            return m_SwarmCount + 1;
                        else if (l_Count <= 20)
                            return m_SwarmCount + 2;
                        else
                            return m_SwarmCount + 3;
                    }
                }

                return 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (IsWipe())
                {
                    EnterEvadeMode();
                    return;
                }

                if (m_BerserkerTimer)
                {
                    if (m_BerserkerTimer <= p_Diff)
                    {
                        Talk(SAY_BERSERK);
                        me->CastSpell(me, SPELL_BOSS_ENRAGE, true);

                        m_BerserkerTimer = 0;
                    }
                    else
                        m_BerserkerTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_CHECK_POWER:
                    {
                        if (me->GetPower(POWER_ENERGY) == 0)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            m_SwarmCount++;
                            DoStopAttack();
                            events.Reset();
                            Talk(SAY_PHASE);
                            DoCast(me, SPELL_TRANSFORM_BUGS, true);
                            DoCast(SPELL_HEART_SWARM);
                            events.ScheduleEvent(EVENT_HEART_SWARM_END, 20000);
                        }
                        events.ScheduleEvent(EVENT_CHECK_POWER, 10000);
                        break;
                    }
                    case EVENT_ALLOW_ROT_CAST:
                    {
                        DoCast(me, SPELL_ALLOW_ROT_CAST, true);
                        events.ScheduleEvent(EVENT_ALLOW_ROT_CAST, 120000);
                        break;
                    }
                    case EVENT_ROT:
                    {
                        if (me->GetPower(POWER_ENERGY) > 0)
                        {
                            DoCast(SPELL_ROT_FILTER);
                            Talk(SAY_ROT);
                        }

                        events.ScheduleEvent(EVENT_ROT, 16000);
                        break;
                    }
                    case EVENT_VOLATILE_ROT:
                    {
                        if (me->GetPower(POWER_ENERGY) > 0)
                            DoCastVictim(SPELL_VOLATILE_ROT);

                        events.ScheduleEvent(EVENT_VOLATILE_ROT, 25000);
                        break;
                    }
                    case EVENT_INFESTED_BREATH:
                    {
                        DoCast(SPELL_INFESTED_BREATH_FILTER);
                        Talk(SAY_BREATH_EMOTE);
                        Talk(SAY_BREATH);

                        if (IsMythic())
                            events.ScheduleEvent(EVENT_INFESTED_MIND, 1000);

                        events.ScheduleEvent(EVENT_INFESTED_BREATH, 36000);
                        break;
                    }
                    case EVENT_HEART_SWARM_END:
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        DefaultEvents(false);
                        RemoveBugs();
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    case EVENT_INFESTED_MIND:
                    {
                        DoCast(SPELL_INFESTED_MIND_FILTER);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            bool IsWipe() const
            {
                std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                for (HostileReference* l_Ref : l_ThreatList)
                {
                    if (Unit* l_Target = l_Ref->getTarget())
                    {
                        /// If all players aren't controlled, don't reset
                        if (l_Target->IsPlayer() && !l_Target->HasAura(SPELL_INFESTED_MIND))
                            return false;
                    }
                }

                return true;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_nythendraAI (p_Creature);
        }
};

/// Gelatinized Decay - 111004
class npc_nythendra_gelatinized_decay : public CreatureScript
{
    public:
        npc_nythendra_gelatinized_decay() : CreatureScript("npc_nythendra_gelatinized_decay") {}

        struct npc_nythendra_gelatinized_decayAI : public ScriptedAI
        {
            npc_nythendra_gelatinized_decayAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_MoveActive;
            uint16 m_RandomMoveTimer;

            void Reset() override
            {
                events.Reset();
                m_MoveActive = true;
                m_RandomMoveTimer = 2000;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 2000);
                events.ScheduleEvent(EVENT_2, 4000);
                events.ScheduleEvent(EVENT_3, 22000);
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                m_MoveActive = true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_MoveActive && !me->isInCombat())
                {
                    if (m_RandomMoveTimer <= p_Diff)
                    {
                        m_MoveActive = false;
                        m_RandomMoveTimer = urand(2, 6) * IN_MILLISECONDS;
                        float l_Angle = (float)rand_norm() * (2.0f * M_PI);
                        float l_Distance = 15 * (float)rand_norm();
                        float l_X = me->GetHomePosition().GetPositionX() + l_Distance * std::cos(l_Angle);
                        float l_Y = me->GetHomePosition().GetPositionY() + l_Distance * std::sin(l_Angle);
                        float l_Z = me->GetHomePosition().GetPositionZ();
                        JadeCore::NormalizeMapCoord(l_X);
                        JadeCore::NormalizeMapCoord(l_Y);

                        me->GetMotionMaster()->MovePoint(1, l_X, l_Y, l_Z);
                    }
                    else
                        m_RandomMoveTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_WAVE_OF_DECAY);
                        events.ScheduleEvent(EVENT_1, 3000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_UNSTABLE_DECAY);
                        events.ScheduleEvent(EVENT_2, 10000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_PUTRID_SWARM);
                        events.ScheduleEvent(EVENT_3, 30000);
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
            return new npc_nythendra_gelatinized_decayAI(p_Creature);
        }
};

/// Corrupted Vermin - 102998, 111005
class npc_nythendra_corrupted_vermin : public CreatureScript
{
    public:
        npc_nythendra_corrupted_vermin() : CreatureScript("npc_nythendra_corrupted_vermin") { }

        struct npc_nythendra_corrupted_verminAI : public ScriptedAI
        {
            npc_nythendra_corrupted_verminAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetSpeed(MOVE_WALK, 0.5f);
                me->SetSpeed(MOVE_RUN, 0.5f);
                me->SetReactState(REACT_PASSIVE);
                me->SetVisible(false);
            }

            uint8 m_CastCount;
            bool m_Active;

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_1, 1000);

                me->GetMotionMaster()->MoveRandom(5.0f);
                DoCast(me, SPELL_SCALE, true);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                TriggerCorruption();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (me->HasAura(SPELL_TRANSFORM_BUGS) || me->HasAura(SPELL_TRANSFORM_INSECT) || me->HasAura(SPELL_HEART_SWARM_SCALE))
                        {
                            if (InstanceScript* l_Instance = me->GetInstanceScript())
                            {
                                if (l_Instance->GetBossState(DATA_NYTHENDRA) != IN_PROGRESS)
                                {
                                    me->RemoveAura(SPELL_TRANSFORM_BUGS);
                                    me->RemoveAura(SPELL_TRANSFORM_INSECT);
                                    me->RemoveAura(SPELL_HEART_SWARM_SCALE);

                                    events.ScheduleEvent(EVENT_1, 2000);
                                    break;
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    }
                    case EVENT_2:
                    {
                        m_CastCount++;
                        DoCast(SPELL_BURST_OF_CORRUPTION);

                        if (m_CastCount == 3)
                        {
                            m_CastCount = 0;
                            DoCast(me, SPELL_SCALE, true);
                            me->RemoveAurasDueToSpell(SPELL_HEART_SWARM_SCALE);
                            me->ClearUnitState(UNIT_STATE_ROOT);
                            break;
                        }

                        events.ScheduleEvent(EVENT_2, 2000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void TriggerCorruption()
            {
                m_CastCount = 0;
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->RemoveAurasDueToSpell(SPELL_SCALE);
                DoCast(me, SPELL_HEART_SWARM_SCALE, true);
                events.ScheduleEvent(EVENT_2, 2000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nythendra_corrupted_verminAI(p_Creature);
        }
};

/// Rot - 203095
class spell_nythendra_rot : public SpellScriptLoader
{
    public:
        spell_nythendra_rot() : SpellScriptLoader("spell_nythendra_rot") { }

        class spell_nythendra_rot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_rot_SpellScript);

            uint8 m_SwarmCount;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, SPELL_INFESTED_MIND));

                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->getVictim())
                        p_Targets.remove(l_Caster->getVictim());

                    if (l_Caster->IsAIEnabled)
                        m_SwarmCount = l_Caster->GetAI()->GetData(DATA_SWARM_COUNTER);
                }

                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });

                JadeCore::Containers::RandomResizeList(p_Targets, m_SwarmCount);
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_ROT_DOT, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nythendra_rot_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnHit += SpellHitFn(spell_nythendra_rot_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_rot_SpellScript();
        }
};

/// Volatile Rot - 204463
/// Rot - 203096
class spell_nythendra_volatile_rot : public SpellScriptLoader
{
    public:
        spell_nythendra_volatile_rot() : SpellScriptLoader("spell_nythendra_volatile_rot") { }

        class spell_nythendra_volatile_rot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_volatile_rot_AuraScript);

            void OnRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                if (!GetCaster() || !GetTarget())
                    return;

                if (p_AurEff->GetId() == SPELL_VOLATILE_ROT)
                {
                    for (uint8 l_I = 0; l_I < (GetTarget()->GetMap()->IsMythic() ? urand(5, 6) : 3); l_I++)
                        GetTarget()->CastSpell(GetTarget(), SPELL_INFESTED_GROUND_VIS, true);
                }
                else if (p_AurEff->GetId() == SPELL_ROT_DOT)
                    GetTarget()->CastSpell(GetTarget(), SPELL_INFESTED_GROUND_AT_2, true, nullptr, nullptr, GetCaster()->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_nythendra_volatile_rot_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);

                if (m_scriptSpellId == SPELL_VOLATILE_ROT)
                    AfterEffectRemove += AuraEffectRemoveFn(spell_nythendra_volatile_rot_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_volatile_rot_AuraScript();
        }
};

/// Volatile Rot - 204470
class spell_nythendra_volatile_rot_dmg : public SpellScriptLoader
{
    public:
        spell_nythendra_volatile_rot_dmg() : SpellScriptLoader("spell_nythendra_volatile_rot_dmg") { }

        class spell_nythendra_volatile_rot_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_volatile_rot_dmg_SpellScript);

            void DealDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float l_Distance = GetCaster()->GetExactDist2d(GetHitUnit());
                float l_Radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster());
                uint32 l_Percent;
                if (l_Distance < 15.0f)
                    l_Percent = 100 - (l_Distance / 10) * 30;
                else
                    l_Percent = 15 - (l_Distance / l_Radius) * 30;

                if (l_Percent <= 0)
                    l_Percent = 1;

                uint32 l_Damage = CalculatePct(GetHitDamage(), l_Percent);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_nythendra_volatile_rot_dmg_SpellScript::DealDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_volatile_rot_dmg_SpellScript();
        }
};

/// Rot - 203536
/// Infested Ground - 203537
class spell_nythendra_infested_ground_rot : public SpellScriptLoader
{
    public:
        spell_nythendra_infested_ground_rot() : SpellScriptLoader("spell_nythendra_infested_ground_rot") { }

        class spell_nythendra_infested_ground_rot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_infested_ground_rot_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster())
                    return;

                WorldLocation* l_Loc = GetHitDest();
                if (l_Loc == nullptr)
                    return;

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Nythendra = l_Instance->instance->GetCreature(l_Instance->GetData64(NPC_NYTHENDRA)))
                    {
                        if (l_Nythendra->isInCombat())
                            l_Nythendra->CastSpell(l_Loc->GetPositionX(), l_Loc->GetPositionY(), l_Loc->GetPositionZ(), SPELL_INFESTED_GROUND_AT, true);
                    }
                }
            }

            void HandleScript(SpellEffIndex p_EffIndex)
            {
                if (!GetCaster() || !GetCaster()->isInCombat())
                    return;

                WorldLocation* l_Loc = GetHitDest();
                if (l_Loc == nullptr)
                    return;

                GetCaster()->CastSpell(l_Loc->GetPositionX(), l_Loc->GetPositionY(), l_Loc->GetPositionZ(), SPELL_INFESTED_GROUND_AT_2, true);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case SPELL_ROT_DUMMY:
                        OnEffectHit += SpellEffectFn(spell_nythendra_infested_ground_rot_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                        break;
                    default:
                        OnEffectHit += SpellEffectFn(spell_nythendra_infested_ground_rot_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_infested_ground_rot_SpellScript();
        }
};

/// Heart of the Swarm - 203552
class spell_nythendra_heart_of_the_swarm : public SpellScriptLoader
{
    public:
        spell_nythendra_heart_of_the_swarm() : SpellScriptLoader("spell_nythendra_heart_of_the_swarm") { }

        class spell_nythendra_heart_of_the_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_heart_of_the_swarm_AuraScript);

            std::set<uint64> m_AreaTriggers;

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    std::list<AreaTrigger*> l_List;
                    l_Caster->GetAreaTriggerList(l_List, SPELL_INFESTED_GROUND_AT_2);

                    l_List.sort(JadeCore::ObjectDistanceOrderPred(l_Caster));

                    for (AreaTrigger* l_AT : l_List)
                        m_AreaTriggers.insert(l_AT->GetGUID());
                }
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    int8 l_Count = 6;
                    std::set<uint64> l_MovedATs;

                    for (uint64 l_Guid : m_AreaTriggers)
                    {
                        if (AreaTrigger* l_AT = sObjectAccessor->GetAreaTrigger(*l_Caster, l_Guid))
                        {
                            if (l_Caster->GetDistance(l_AT) <= 100.0f)
                            {
                                l_MovedATs.insert(l_Guid);
                                l_AT->CastAction();

                                --l_Count;
                            }
                        }

                        if (l_Count <= 0)
                            break;
                    }

                    for (uint64 l_Guid : l_MovedATs)
                        m_AreaTriggers.erase(l_Guid);

                    std::list<Creature*> l_Bugs;
                    auto& l_BugList = l_Caster->GetMap()->GetObjectsOnMap();
                    for (auto l_Object = l_BugList.begin(); l_Object != l_BugList.end(); ++l_Object)
                    {
                        if ((*l_Object)->GetEntry() != NPC_CORRUPTED_VERMIN || (*l_Object)->ToCreature() == nullptr)
                            continue;

                        Creature* l_Bug = (*l_Object)->ToCreature();
                        if (l_Bug->HasAura(SPELL_HEART_SWARM_SCALE))
                            continue;

                        l_Bugs.push_back(l_Bug);
                    }

                    if (!l_Bugs.empty())
                        JadeCore::RandomResizeList(l_Bugs, 2);
                    else
                        return;

                    for (Creature* l_Bug : l_Bugs)
                    {
                        if (l_Bug && l_Bug->IsAIEnabled)
                            l_Bug->AI()->DoAction(0);
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_nythendra_heart_of_the_swarm_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_nythendra_heart_of_the_swarm_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_heart_of_the_swarm_AuraScript();
        }
};

/// Infested Ground - 203045
/// Infested Breath - 202978
/// Rot - 203097
/// Volatile Rot - 204470
/// Burst of Corruption - 203646
/// Spread Infestation - 205070
class spell_nythendra_infested : public SpellScriptLoader
{
    public:
        spell_nythendra_infested() : SpellScriptLoader("spell_nythendra_infested") { }

        class spell_nythendra_infested_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nythendra_infested_SpellScript);

            void HandleOnHit()
            {
                if (!GetCaster() || !GetHitUnit() || GetHitUnit()->HasAura(SPELL_INFESTED_MIND))
                    return;

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Nythendra = l_Instance->instance->GetCreature(l_Instance->GetData64(NPC_NYTHENDRA)))
                    {
                        if (l_Nythendra->GetMap()->IsHeroicOrMythic())
                            l_Nythendra->CastSpell(GetHitUnit(), SPELL_INFESTED, true);
                    }
                }
            }

            void Register() override
            {
                if (m_scriptSpellId != SPELL_INFESTED_GROUND_DOT)
                    OnHit += SpellHitFn(spell_nythendra_infested_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nythendra_infested_SpellScript();
        }

        class spell_nythendra_infested_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_infested_AuraScript);

            void HandleOnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster() || !GetTarget() || GetTarget()->HasAura(SPELL_INFESTED_MIND))
                    return;

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Nythendra = l_Instance->instance->GetCreature(l_Instance->GetData64(NPC_NYTHENDRA)))
                    {
                        if (l_Nythendra->GetMap()->IsHeroicOrMythic())
                            l_Nythendra->CastSpell(GetTarget(), SPELL_INFESTED, true);
                    }
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == SPELL_INFESTED_GROUND_DOT)
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_nythendra_infested_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_infested_AuraScript();
        }
};

/// Infested Mind - 205043
class spell_nythendra_infested_mind : public SpellScriptLoader
{
    public:
        spell_nythendra_infested_mind() : SpellScriptLoader("spell_nythendra_infested_mind") { }

        class spell_nythendra_infested_mind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_infested_mind_AuraScript);

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& /*p_DmgInfo*/, uint32& p_Absorb)
            {
                p_Absorb = 1;

                if (GetTarget() && GetTarget()->HealthBelowPct(51))
                    GetAura()->Remove();
            }

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                if (GetTarget())
                {
                    GetTarget()->GetMotionMaster()->Clear();
                    GetTarget()->CastSpell(GetTarget(), SPELL_SPREAD_INFESTATION);
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_nythendra_infested_mind_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_nythendra_infested_mind_AuraScript::OnPeriodic, EFFECT_8, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_infested_mind_AuraScript();
        }
};

/// Infested Breath - 202977
class spell_nythendra_infested_breath : public SpellScriptLoader
{
    public:
        spell_nythendra_infested_breath() : SpellScriptLoader("spell_nythendra_infested_breath") { }

        class spell_nythendra_infested_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_nythendra_infested_breath_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    for (uint8 l_I = 0; l_I < (l_Caster->GetMap()->IsMythic() ? 2 : 1); ++l_I)
                    {
                        float l_X = l_Caster->m_positionX;
                        float l_Y = l_Caster->m_positionY;
                        float l_Z = l_Caster->m_positionZ;
                        float l_A = 70.0f * M_PI / 180.0f / 2.0f;
                        float l_O = l_Caster->m_orientation + frand(-l_A, l_A);
                        float l_D = frand(15.0f, 55.0f);

                        l_X = l_X + l_D * std::cos(l_O);
                        l_Y = l_Y + l_D * std::sin(l_O);

                        l_Caster->CastSpell(l_X, l_Y, l_Z, SPELL_INFESTED_GROUND_MISSILE, true);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_nythendra_infested_breath_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectRemove += AuraEffectRemoveFn(spell_nythendra_infested_breath_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_nythendra_infested_breath_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_nythendra()
{
    /// Boss
    new boss_nythendra();

    /// Creatures
    new npc_nythendra_gelatinized_decay();
    new npc_nythendra_corrupted_vermin();

    /// Spells
    new spell_nythendra_volatile_rot();
    new spell_nythendra_volatile_rot_dmg();
    new spell_nythendra_infested_ground_rot();
    new spell_nythendra_heart_of_the_swarm();
    new spell_nythendra_rot();
    new spell_nythendra_infested();
    new spell_nythendra_infested_mind();
    new spell_nythendra_infested_breath();
}
#endif
