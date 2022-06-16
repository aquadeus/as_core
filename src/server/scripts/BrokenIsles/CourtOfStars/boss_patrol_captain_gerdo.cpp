////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "court_of_stars.hpp"
#include "Vehicle.h"
#include "MoveSplineInit.h"
#include "ScriptedCreature.h"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_ARCANE_LOCKDOWN = 1,
    SAY_SIGNAL          = 2,
    SAY_FLASK           = 3,
    SAY_DEATH           = 4,
    SAY_POISONED_FLASK  = 5
};

enum Spells
{
    SPELL_RESONANT_SLASH            = 207261,
    SPELL_RESONANT_SLASH_FRONT      = 206574,
    SPELL_RESONANT_SLASH_BACK       = 206580,
    SPELL_ARCANE_LOCKDOWN           = 207278,
    SPELL_SIGNAL_BEACON             = 207806,
    SPELL_SUM_VIGILANT_NIGHTWATCH   = 210435,
    SPELL_FLASK_SOLEMN_NIGHT        = 207815,

    /// Guards
    SPELL_HINDER                    = 215204,

    /// Streetsweeper
    SPELL_STREETSWEEPER             = 219488,
    SPELL_STREETSWEEPER_VISUAL      = 219475,
    SPELL_STREETSWEEPER_VISUAL_BEAM = 219483,
    SPELL_STREETSWEEPER_AT          = 219477,

    /// Beacon
    SPELL_WATCHPOST_VICINITY        = 212479,
    SPELL_WATCHPOST_VICINITY_AT     = 212480,
    SPELL_CAST_DISABLE_BEACON       = 210253,

    SPELL_SOUND_ALARM               = 210261,
    SPELL_THROW_TORCH               = 209036,

    SPELL_INTERACT                  = 210295,
    SPELL_SIGNALING                 = 209524,
    SPELL_RIDE_BOAT                 = 213507,
    SPELL_EXIT_BOAT                 = 208703,
    SPELL_AREA_SECURED              = 227147,

    SPELL_FLASK_POISONED            = 210385,
    SPELL_FLASK_DEFAULT             = 208182
};

enum eEvents
{
    EVENT_RESONANT_SLASH = 1,
    EVENT_ARCANE_LOCKDOWN,
    EVENT_FLASK_SOLEMN_NIGHT,
    EVENT_FLASK_SOLEMN_NIGHT_END,
    EVENT_STREETSWEEPER,
    EVENT_FLASK_POISONED_KILL
};

Position const g_FlaskPos = { 1308.29f, 3537.42f, 20.76f, 0.0f };

/// Patrol Captain Gerdo - 104215
class boss_patrol_captain_gerdo : public CreatureScript
{
    public:
        boss_patrol_captain_gerdo() : CreatureScript("boss_patrol_captain_gerdo") { }

        struct boss_patrol_captain_gerdoAI : public BossAI
        {
            boss_patrol_captain_gerdoAI(Creature* p_Creature) : BossAI(p_Creature, DATA_CAPTAIN_GERDO)
            {
                m_InstanceScript = p_Creature->GetInstanceScript();
            }

            bool m_FlaskEvent;
            bool m_BeaconEvent;
            uint64 m_StreetsweeperTarget = 0;
            InstanceScript* m_InstanceScript;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void Reset() override
            {
                _Reset();
                ClearDelayedOperations();
                m_FlaskEvent = false;
                m_BeaconEvent = false;
                me->SetReactState(REACT_AGGRESSIVE);
                RenewSpellSecuredAura();

                InitalizeWaypoints();
            }

            void InitalizeWaypoints()
            {
                /// Creating the circle path from the center
                Movement::MoveSplineInit l_Init(me);

                float l_ZR = frand(-0.3f, +0.4f);
                float l_ZX = frand(-0.015f, +0.038f);

                std::vector<G3D::Vector3> l_Path =
                {
                    { 1312.9972f, 3572.4250f, 20.0262f },
                    { 1334.3140f, 3564.5015f, 20.0262f },
                    { 1343.7964f, 3546.9956f, 20.0259f },
                    { 1334.8950f, 3535.0635f, 20.3708f },
                    { 1333.6654f, 3510.6375f, 20.0267f },
                    { 1291.7965f, 3519.0142f, 20.0267f },
                    { 1285.7400f, 3542.3850f, 20.0267f },
                    { 1286.2914f, 3550.1042f, 20.0263f },
                    { 1297.4331f, 3556.4919f, 20.0263f }
                };

                for (G3D::Vector3 l_Point : l_Path)
                    l_Init.Path().push_back(l_Point);

                if (l_Init.Path().empty())
                    return;

                l_Init.SetSmooth();
                l_Init.SetVelocity(frand(1.3f, 1.5f));
                l_Init.SetSmooth();
                l_Init.SetCyclic();
                l_Init.Launch();

                SetFlyMode(false);
            }

            void RenewSpellSecuredAura()
            {
                me->RemoveAura(SPELL_AREA_SECURED);
                if (m_InstanceScript && m_InstanceScript->GetData(DATA_AREA_SECURED) < 5)
                    if (Aura* l_Aura = me->AddAura(SPELL_AREA_SECURED, me))
                        l_Aura->SetStackAmount(5 - m_InstanceScript->GetData(DATA_AREA_SECURED));
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();

                events.ScheduleEvent(EVENT_RESONANT_SLASH, 7000);
                events.ScheduleEvent(EVENT_ARCANE_LOCKDOWN, 15000);
                events.ScheduleEvent(EVENT_STREETSWEEPER, 18000);
            }

            void EnterEvadeMode() override
            {
                ClearDelayedOperations();
                BossAI::EnterEvadeMode();
                _EnterEvadeMode();

                std::list<Creature*> l_GuardsList;
                GetCreatureListWithEntryInGrid(l_GuardsList, me, 104918, 250.0f);

                for (Creature* l_Guard : l_GuardsList)
                    l_Guard->DespawnOrUnsummon();

                me->NearTeleportTo(me->GetHomePosition(), false);
                me->Respawn(true, true, 20 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();

                if (Creature* l_Dusk = me->SummonCreature(113617, 967.265f, 3468.67f, 1.90085f, 4.46257f, TEMPSUMMON_MANUAL_DESPAWN))
                    l_Dusk->SetPhaseMask(me->GetPhaseMask(), true);

                if (Creature* l_SuramarCitizen = me->SummonCreature(107471, 1290.2429f, 3456.3633f, 30.3251f, 1.21394f, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    l_SuramarCitizen->GetMotionMaster()->Clear(false);
                    l_SuramarCitizen->GetMotionMaster()->MovePoint(0, 1299.2692f, 3479.3894f, 30.1346f);
                }

                if (m_InstanceScript != nullptr)
                {
                    if (GameObject* l_Door = m_InstanceScript->instance->GetGameObject(m_InstanceScript->GetData64(eGameObjects::GO_CAPTAIN_DOOR)))
                    {
                        l_Door->ResetLootMode();
                        l_Door->ResetDoorOrButton();
                        l_Door->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NOT_SELECTABLE);
                    }

                    if (IsMythic() && m_InstanceScript->GetData(DATA_AREA_SECURED) >= 5)
                        m_InstanceScript->DoCompleteAchievement(eAchievements::ACHIEVEMENT_WAITING_FOR_GERDO);
                }
            }

            void SpellHit(Unit* /*p_Target*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SIGNAL_BEACON && instance)
                    instance->SetData(DATA_BEACON_ACTIVATE, IN_PROGRESS);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* p_SpellInfo) override
            {
                if (me->HealthBelowPct(26) && !m_FlaskEvent)
                {
                    m_FlaskEvent = true;
                    Talk(SAY_FLASK);
                    events.ScheduleEvent(EVENT_FLASK_SOLEMN_NIGHT, 500);
                }
                else if (me->HealthBelowPct(76) && !m_BeaconEvent)
                {
                    me->CastStop();
                    me->StopMoving();

                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    m_BeaconEvent = true;
                    DoCast(SPELL_SIGNAL_BEACON);

                    AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                    });

                    events.DelayEvents(6 * TimeConstants::IN_MILLISECONDS);

                    Talk(SAY_SIGNAL);
                }
            }

            uint64 GetGUID(int32 p_ID /* = 0 */) override
            {
                return p_ID == 1 ? m_StreetsweeperTarget : 0;
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                    events.ScheduleEvent(EVENT_FLASK_SOLEMN_NIGHT_END, 500);
            }

            Player* SelectRandomPlayer()
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (i->getSource()->isAlive() && !i->getSource()->isGameMaster())
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
                    return (*j);
                }
                return nullptr;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    return;
                }
                else
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                switch (events.ExecuteEvent())
                {
                    case EVENT_RESONANT_SLASH:
                    {
                        DoCast(SPELL_RESONANT_SLASH);
                        Position l_Pos = *me;

                        if (Creature* l_FrontTrigger = me->SummonCreature(NPC_GERDO_TRIG_FRONT, l_Pos))
                        {
                            l_FrontTrigger->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            l_FrontTrigger->SetOrientation(l_Pos.GetOrientation());
                            l_FrontTrigger->SetFacingTo(l_Pos.GetOrientation());
                            l_FrontTrigger->CastSpell(l_FrontTrigger, SPELL_RESONANT_SLASH_FRONT);
                        }

                        me->GetNearPosition(l_Pos, 4.0f, me->GetOrientation() + M_PI);

                        if (Creature* l_BackTrigger = me->SummonCreature(NPC_GERDO_TRIG_BACK, l_Pos))
                        {
                            l_BackTrigger->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            l_BackTrigger->SetOrientation(l_Pos.GetOrientation());
                            l_BackTrigger->SetFacingTo(l_Pos.GetOrientation());
                            l_BackTrigger->CastSpell(l_BackTrigger, SPELL_RESONANT_SLASH_BACK);
                        }

                        events.ScheduleEvent(EVENT_RESONANT_SLASH, 12000);
                        break;
                    }
                    case EVENT_ARCANE_LOCKDOWN:
                    {
                        DoCast(SPELL_ARCANE_LOCKDOWN);
                        Talk(SAY_ARCANE_LOCKDOWN);
                        events.ScheduleEvent(EVENT_ARCANE_LOCKDOWN, 28000);
                        break;
                    }
                    case EVENT_FLASK_SOLEMN_NIGHT:
                    {
                        DoStopAttack();

                        me->CastStop();
                        me->StopMoving();

                        events.DelayEvent(EVENT_ARCANE_LOCKDOWN, 15 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(EVENT_RESONANT_SLASH, 15 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(EVENT_STREETSWEEPER, 15 * TimeConstants::IN_MILLISECONDS);

                        me->SetReactState(REACT_PASSIVE);

                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MovePoint(1, g_FlaskPos);
                        break;
                    }
                    case EVENT_FLASK_SOLEMN_NIGHT_END:
                    {
                        if (Creature* l_Flask = me->FindNearestCreature(NPC_FLASK_OF_SOLEMN_NIGHT, 50.0f))
                        {
                            if (l_Flask->HasAura(SPELL_FLASK_POISONED))
                            {
                                Talk(SAY_POISONED_FLASK);
                                events.ScheduleEvent(EVENT_FLASK_POISONED_KILL, 1500);
                            }
                            else
                            {
                                DoCast(SPELL_FLASK_SOLEMN_NIGHT);
                                me->SetReactState(REACT_AGGRESSIVE);
                            }
                        }

                        break;
                    }
                    case EVENT_STREETSWEEPER:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            m_StreetsweeperTarget = l_Target->GetGUID();
                            me->CastSpell(l_Target, SPELL_STREETSWEEPER, false);
                        }

                        events.ScheduleEvent(EVENT_STREETSWEEPER, 18000);
                        break;
                    }
                    case EVENT_FLASK_POISONED_KILL:
                    {
                        if (me->isAlive())
                            if (Player* player = SelectRandomPlayer())
                                player->Kill(me);

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
            return new boss_patrol_captain_gerdoAI(p_Creature);
        }
};

/// Vigilant Duskwatch - 104918
class npc_captain_gerdo_vigilant_duskwatch : public CreatureScript
{
    public:
        npc_captain_gerdo_vigilant_duskwatch() : CreatureScript("npc_captain_gerdo_vigilant_duskwatch") { }

        struct npc_captain_gerdo_vigilant_duskwatchAI : public ScriptedAI
        {
            npc_captain_gerdo_vigilant_duskwatchAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                /// Might be better..?
                if (UnitAI* l_AI = p_Summoner->GetAI())
                {
                    if (Unit* l_Target = l_AI->SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        AttackStart(l_Target);
                }

               /// DoZoneInCombat(me, 100.0f);
                events.ScheduleEvent(EVENT_1, 5000);
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
                        DoCastVictim(SPELL_HINDER);
                        events.ScheduleEvent(EVENT_1, 10000);
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
            return new npc_captain_gerdo_vigilant_duskwatchAI(p_Creature);
        }
};

/// Quest: "The Deceitful Student" 42784
/// Arcanist Marold - 108796
class npc_cos_arcanist_marlod : public CreatureScript
{
    public:
        npc_cos_arcanist_marlod() : CreatureScript("npc_cos_arcanist_marlod") { }

        struct npc_cos_arcanist_marlodAI : public ScriptedAI
        {
            npc_cos_arcanist_marlodAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                UncontrollableBlast      = 216110,
                UncontrolableBlastDmg    = 216111,
                WildMagic                = 216096,
                EventWildMagic           = 1,
                EventUncontrollableBlast = 2,
                QuestId                  = 42784
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Who) override
            {
                events.ScheduleEvent(EventUncontrollableBlast, 10000);
                events.ScheduleEvent(EventWildMagic, 20000);
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
                    case EventUncontrollableBlast:
                    {
                        DoCastVictim(UncontrollableBlast);
                        events.ScheduleEvent(EventUncontrollableBlast, 10000);
                        break;
                    }
                    case EventWildMagic:
                    {
                        DoCast(WildMagic);
                        events.ScheduleEvent(EventWildMagic, 20000);
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
            return new npc_cos_arcanist_marlodAI(p_Creature);
        }
};

/// Signal Lantern - 105729
class npc_signal_lantern : public CreatureScript
{
    public:
        npc_signal_lantern() : CreatureScript("npc_signal_lantern") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player->HasAura(SPELL_INTERACT))
            {
                p_Player->ADD_GOSSIP_ITEM_DB(19515, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                p_Player->SEND_GOSSIP_MENU(28836, p_Creature->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action != (GOSSIP_ACTION_INFO_DEF + 1) || p_Player->HasAura(SPELL_INTERACT))
                return false;

            if (p_Creature->IsAIEnabled)
            {
                if (auto l_AI = CAST_AI(npc_signal_lantern::npc_signal_lanternAI, p_Creature->GetAI()))
                    l_AI->m_PlayerGuid = p_Player->GetGUID();
            }

            p_Player->CastSpell(p_Player, SPELL_SIGNALING);
            p_Player->PlayerTalkClass->SendCloseGossip();
            return true;
        }

        struct npc_signal_lanternAI : public ScriptedAI
        {
            npc_signal_lanternAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_MessageDelievered = false;
            }

            int32 m_EndCount = 0;

            uint64 m_PlayerGuid = 0;

            uint32 m_NearestPlayerDiff = 1000;

            bool m_MessageDelievered;
            uint8 m_SummonCount = 0; ///< summon on first boat

            void Reset() override
            {
                m_NearestPlayerDiff = 1000;

                me->AddAura(209519, me);
                m_SummonCount = 0;
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 0)
                {
                    if (m_EndCount > 4)
                        m_EndCount++;
                    else
                        m_EndCount = 0;
                }

                if (p_Action == 1)
                {
                    if (m_PlayerGuid)
                        if (Player* l_Plr = Player::GetPlayer(*me, m_PlayerGuid))
                            l_Plr->AddAura(SPELL_INTERACT, l_Plr);

                    Creature* l_Boat = me->SummonCreature(83816, 984.176f, 3786.657f, 1.0f, 0.3f, TEMPSUMMON_TIMED_DESPAWN, 6000);
                    if (l_Boat != nullptr)
                    {
                        l_Boat->SetDisplayId(69057);

                        l_Boat->SetSpeed(UnitMoveType::MOVE_RUN, 2.0f, true);

                        l_Boat->SetReactState(ReactStates::REACT_PASSIVE);

                        l_Boat->setFaction(FACTION_FRIENDLY);

                        l_Boat->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                        l_Boat->GetMotionMaster()->Clear(false);
                        l_Boat->GetMotionMaster()->MovePoint(1, 1007.048f, 3777.371f, -2.032380f, false);
                    }

                    uint64 l_Guid = l_Boat->GetGUID();
                    uint8 SummCount = m_SummonCount;
                    AddTimedDelayedOperation(3000, [this, l_Guid, SummCount]() -> void
                    {
                        if (!m_PlayerGuid || !l_Guid)
                            return;

                        if (Creature* l_Boat = Creature::GetCreature(*me, l_Guid))
                        {
                            if (Player* l_Player = Player::GetPlayer(*me, m_PlayerGuid))
                            {
                                if (Creature* l_ActualBoat = l_Player->SummonCreature(105264, 1006.705f, 3776.961f, 0.000243f, 6.020809f, TEMPSUMMON_MANUAL_DESPAWN)) ///< Spawns actual boat.
                                    if (SummCount == 0)
                                        if (CreatureAI* l_AI = l_ActualBoat->AI())
                                            l_AI->DoAction(0);

                                l_Boat->SetDisplayId(11686);
                                l_Boat->DespawnOrUnsummon(200);
                            }
                        }
                    });

                    m_SummonCount++;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon != nullptr)
                {
                    if (p_Summon->GetEntry() == 83816)
                    {
                        p_Summon->SetWalk(false);
                        p_Summon->SetCanFly(true, true);
                        p_Summon->SetDisableGravity(true);
                    }
                }
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                return m_EndCount;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_NearestPlayerDiff <= p_Diff)
                {
                    if (!m_MessageDelievered)
                    {
                        m_MessageDelievered = true;

                        Talk(0);
                    }

                    m_NearestPlayerDiff = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_NearestPlayerDiff -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_signal_lanternAI(p_Creature);
        }
};

/// Duskwatch Sentry - 104251
class npc_duskwatch_sentry : public CreatureScript
{
    public:
        npc_duskwatch_sentry() : CreatureScript("npc_duskwatch_sentry") { }

        struct npc_duskwatch_sentryAI : public ScriptedAI
        {
            npc_duskwatch_sentryAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Alarm = false;
            bool m_Disabled = false;
            bool m_Torch;
            bool m_CastAlarm = false;
            bool m_CombatCheck = false;

            void Reset() override
            {
                m_Alarm = false;
                m_Torch = true;
                m_CastAlarm = false;
                m_CombatCheck = false;

                SetCombatMovement(false);

                ClearDelayedOperations();
            }

            void AttackVictim()
            {
                AddTimedDelayedOperation(2000, [this]() -> void
                {
                    me->GetMotionMaster()->Clear(false);
                    SetCombatMovement(true);
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    m_CombatCheck = true;

                    if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.0f, true))
                        AttackStart(l_Target);
                });
            }

            void EnterCombat(Unit* p_Who) override
            {
                if (!m_Alarm)
                {
                    if (m_Torch)
                    {
                        m_Torch = false;

                        me->CastSpell(p_Who, SPELL_THROW_TORCH, true);
                        me->SetReactState(REACT_PASSIVE);
                    }

                    m_Alarm = true;
                    Creature* l_Target = me->FindNearestCreature(NPC_GERDO_ARCANE_BEACON, 200.0f);
                    if (l_Target)
                        m_Disabled = l_Target->HasAura(SPELL_DISABLED);

                    if (m_Disabled)
                    {
                        AttackVictim();
                        return;
                    }

                    me->CombatStop(true);
                    Talk(0);

                    if (l_Target)
                    {
                        me->GetMotionMaster()->Clear();
                        me->SetWalk(false);

                        float _x, _y, _z;
                        float _angle = float(rand_norm())*static_cast<float>(2 * M_PI);
                        l_Target->GetClosePoint(_x, _y, _z, l_Target->GetObjectSize(), frand(2.f, 3.f), _angle);
                        me->GetMotionMaster()->MovePoint(100, _x, _y, l_Target->GetPositionZ());
                    }
                }
            }

            void EnterEvadeMode() override
            {
                m_CombatCheck = false;
                ScriptedAI::EnterEvadeMode();
                me->SetReactState(REACT_AGGRESSIVE);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo != nullptr)
                {
                    if (p_SpellInfo->Id == SPELL_SOUND_ALARM)
                    {
                        m_CastAlarm = false;
                        if (Creature* l_Target = me->FindNearestCreature(NPC_GERDO_ARCANE_BEACON, 20.f))
                        {
                            l_Target->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Target->CastSpell(l_Target, SPELL_SECURED_BUFF, true);
                        }

                        AttackVictim();
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 100 && !m_Disabled)
                {
                    me->SetReactState(REACT_AGGRESSIVE);

                    if (Player* l_Player = me->FindNearestPlayer(20.0f))
                        me->SendMeleeAttackStart(l_Player);

                    m_CastAlarm = true;

                    m_Disabled = true;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                {
                    if (m_CombatCheck)
                        EnterEvadeMode();
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_CastAlarm)
                    DoCast(me, SPELL_SOUND_ALARM);

                if(m_Disabled)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_duskwatch_sentryAI(p_Creature);
        }
};

/// Arcane Beacon - 104245
class npc_gerdo_arcane_beacon : public CreatureScript
{
    public:
        npc_gerdo_arcane_beacon() : CreatureScript("npc_gerdo_arcane_beacon") { }

        struct npc_gerdo_arcane_beaconAI : public ScriptedAI
        {
            npc_gerdo_arcane_beaconAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, 206309, true); ///< Spotlight
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (!me->HasAura(SPELL_DISABLED) && !me->HasAura(SPELL_SECURED_BUFF))
                    p_Clicker->CastSpell(me, SPELL_CAST_DISABLE_BEACON);
            }

            void SpellHit(Unit* /*p_Target*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_CAST_DISABLE_BEACON)
                {
                    me->RemoveAura(206309); ///< Spotlight

                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    DoCast(me, SPELL_DISABLED, true);
                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(DATA_AREA_SECURED, 0);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gerdo_arcane_beaconAI(p_Creature);
        }
};

/// Streetsweeper - 110403
class npc_gerdo_streetsweeper : public CreatureScript
{
    public:
        npc_gerdo_streetsweeper() : CreatureScript("npc_gerdo_streetsweeper") { }

        struct npc_gerdo_streetsweeperAI : public ScriptedAI
        {
            npc_gerdo_streetsweeperAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_STREETSWEEPER_VISUAL, true);
                events.ScheduleEvent(EVENT_1, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_owner = me->GetAnyOwner())
                        {
                            if (Creature* l_Summoner = l_owner->ToCreature())
                            {
                                if (l_Summoner->IsAIEnabled)
                                {
                                    if (Unit* l_Target = l_Summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                                        me->SetFacingToObject(l_Target);
                                }
                            }
                        }
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_STREETSWEEPER_AT, true);
                        events.ScheduleEvent(EVENT_3, 2000);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(me, SPELL_STREETSWEEPER_VISUAL_BEAM, true);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gerdo_streetsweeperAI(p_Creature);
        }
};

/// Arcane Lockdown - 207278
class spell_patrol_arcane_lockdown : public SpellScriptLoader
{
    public:
        spell_patrol_arcane_lockdown() : SpellScriptLoader("spell_patrol_arcane_lockdown") { }

        class spell_patrol_arcane_lockdown_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_patrol_arcane_lockdown_AuraScript);

            uint8 m_Stacks = 0;

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetTarget())
                    return;

                if (Aura* l_Aura = p_AurEff->GetBase())
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() + 2);
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                if (!(p_EventInfo.GetTypeMask() & PROC_FLAG_JUMP))
                    return;

                if (Aura* l_Aura = p_AurEff->GetBase())
                {
                    uint8 m_Stacks = l_Aura->GetStackAmount();

                    if (m_Stacks > 1)
                        l_Aura->SetStackAmount(l_Aura->GetStackAmount() - 1);
                    else
                        l_Aura->Remove();
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_patrol_arcane_lockdown_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                OnEffectProc += AuraEffectProcFn(spell_patrol_arcane_lockdown_AuraScript::OnProc, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_patrol_arcane_lockdown_AuraScript();
        }
};

/// Nightborne Boat
class npc_cos_nightborne_boat : public VehicleScript
{
    public:
        npc_cos_nightborne_boat() : VehicleScript("npc_cos_nightborne_boat") { }

        struct npc_cos_nightborne_boatAI : public npc_escortAI
        {
            npc_cos_nightborne_boatAI(Creature* creature) : npc_escortAI(creature)
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Reset() override
            {
                SetMaxPlayerDistance(300.0f);
                SetDespawnAtFar(false);
                SetDespawnAtEnd(false);
                SetEscortPaused(true);
            }

            uint64 m_SummonerGUID = 0;

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 0)
                {
                    uint64 l_SummonerGUID = m_SummonerGUID;
                    AddTimedDelayedOperation(1400, [this, l_SummonerGUID]() -> void
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, l_SummonerGUID))
                            if (Creature* l_Iyleth = l_Player->SummonCreature(105296, *me, TEMPSUMMON_MANUAL_DESPAWN))
                                l_Iyleth->CastCustomSpell(213507, SPELLVALUE_BASE_POINT0, 2, me, true);
                    });
                }
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->IsPlayer())
                    return;

                m_SummonerGUID = p_Summoner->GetGUID();

                uint64 l_SummonerGUID = m_SummonerGUID;

                if (Player* l_Player = Player::GetPlayer(*me, l_SummonerGUID))
                {
                    if (l_Player->IsMounted())
                    {
                        l_Player->Dismount();
                        l_Player->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    }
                }

                AddTimedDelayedOperation(1000, [this, l_SummonerGUID]() -> void
                {
                    if (Player* l_Player = Player::GetPlayer(*me, l_SummonerGUID))
                    {
                        if (l_Player->IsMounted())
                        {
                            l_Player->Dismount();
                            l_Player->RemoveAurasByType(SPELL_AURA_MOUNTED);
                        }

                        l_Player->RemoveAurasByType(SPELL_AURA_WATER_WALK);
                        l_Player->CastStop();
                        l_Player->NearTeleportTo(*me);
                        l_Player->CastSpell(l_Player, 115790, true);
                        l_Player->CastSpell(me, 213507, false);
                    }
                });
            }

            void WaypointReached(uint32 p_WpId) override
            {
                switch (p_WpId)
                {
                    case 14: ///< 1st step last point
                    {
                        Position const l_Pos = { 966.614f, 3462.21f, 2.113f, 5.53f };

                        if (Unit* l_Passenger = me->GetVehicleKit()->GetPassenger(0))
                        {
                            l_Passenger->RemoveAura(115790);
                            l_Passenger->ExitVehicle(&l_Pos);

                            uint64 l_GUID = l_Passenger->GetGUID();

                            AddTimedDelayedOperation(500, [this, l_GUID, l_Pos]() -> void
                            {
                                if (Player* l_Player = Player::GetPlayer(*me, l_GUID))
                                {
                                    l_Player->RemoveAurasByType(SPELL_AURA_WATER_WALK);
                                    l_Player->NearTeleportTo(l_Pos);
                                }
                            });
                        }

                        if (Unit* l_Passenger = me->GetVehicleKit()->GetPassenger(1))
                        {
                            if (Creature* l_Creature = l_Passenger->ToCreature())
                                l_Creature->DespawnOrUnsummon();
                        }

                        AddTimedDelayedOperation(2000, [this]() -> void { me->DespawnOrUnsummon(); });
                        break;
                    }
                    default:
                        break;
                }
            }

            void PassengerBoarded(Unit* p_Passenger, int8 p_SeatID, bool p_Apply) override
            {
                if (p_Apply)
                {
                    uint64 l_SummonerGUID = m_SummonerGUID;

                    AddTimedDelayedOperation(1400, [this, l_SummonerGUID]() -> void
                    {
                        me->SetSpeed(MOVE_FLIGHT, 1.5f, true);
                        Start(false, true, l_SummonerGUID);
                        SetNextWaypoint(1);
                        SetRun(true);
                        me->SetWalk(false);
                        SetEscortPaused(false);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                npc_escortAI::UpdateAI(p_Diff);
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cos_nightborne_boatAI(creature);
        }
};

/// Ly'leth Lunastre - 105296
class npc_lilet_lunarh : public VehicleScript
{
    public:
        npc_lilet_lunarh() : VehicleScript("npc_lilet_lunarh") { }

        struct npc_lilet_lunarhAI : public ScriptedAI
        {
            npc_lilet_lunarhAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!p_Summoner->IsPlayer())
                    return;

                uint64 l_SummonerGUID = p_Summoner->GetGUID();

                AddTimedDelayedOperation(1000,  [this]()  -> void { Talk(0); });
                AddTimedDelayedOperation(6000,  [this]()  -> void { Talk(1); });
                AddTimedDelayedOperation(12000, [this]()  -> void { Talk(2); });
                AddTimedDelayedOperation(17000, [this]()  -> void { Talk(3); });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_lilet_lunarhAI(creature);
        }
};

/// Guardian Construct - 104270
class npc_cos_guardian_construct : public VehicleScript
{
    public:
        npc_cos_guardian_construct() : VehicleScript("npc_cos_guardian_construct") { }

        struct npc_cos_guardian_constructAI : public ScriptedAI
        {
            npc_cos_guardian_constructAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventChargedSmash = 1,
                EventChargingStation,
                EventSuppress,

                SpellChargedSmash     = 209495,
                SpellChargingStation  = 225100,
                SpellSuppress         = 209413
            };

            void Reset() override
            {
                if (me->GetInstanceScript())
                {
                    if (me->GetInstanceScript()->GetData(me->GetEntry()))
                    {
                        me->setFaction(7); ///< Neutral
                        me->SetReactState(REACT_DEFENSIVE); ///< Can be always attacked
                    }
                }
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                Reset();
                events.Reset();

                events.ScheduleEvent(eData::EventChargedSmash, 3 * IN_MILLISECONDS);
                events.ScheduleEvent(eData::EventSuppress, 19 * IN_MILLISECONDS);
                if (IsMythic())
                    events.ScheduleEvent(eData::EventChargingStation, 7 * IN_MILLISECONDS);
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
                    case EventChargedSmash:
                    {
                        DoCastVictim(eData::SpellChargedSmash);
                        events.ScheduleEvent(EventChargedSmash, 28000);
                        break;
                    }
                    case EventChargingStation:
                    {
                        me->CastSpell(me, SpellChargingStation, false);
                        events.ScheduleEvent(EventChargingStation, 28000);
                        break;
                    }
                    case EventSuppress:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, SpellSuppress, false);
                        events.ScheduleEvent(EventSuppress, 28000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_cos_guardian_constructAI(creature);
        }
};

class npc_cos_resonant_stalker : public VehicleScript
{
    public:
        npc_cos_resonant_stalker() : VehicleScript("npc_cos_resonant_stalker") { }

        struct npc_cos_resonant_stalkerAI : public ScriptedAI
        {
            npc_cos_resonant_stalkerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_cos_resonant_stalkerAI(creature);
        }
};

/// 105117
class npc_cos_flask_of_solemn_night : public CreatureScript
{
    public:
        npc_cos_flask_of_solemn_night() : CreatureScript("npc_cos_flask_of_solemn_night") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->getClass() == CLASS_ROGUE || p_Player->HasSkill(SKILL_ALCHEMY))
            {
                p_Creature->RemoveAura(SPELL_INTERACT);
                p_Creature->RemoveAura(SPELL_FLASK_DEFAULT);

                p_Creature->AddAura(SPELL_FLASK_POISONED, p_Creature);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return true;
        }

        struct npc_cos_flask_of_solemn_nightAI : public ScriptedAI
        {
            npc_cos_flask_of_solemn_nightAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                me->AddAura(SPELL_FLASK_DEFAULT, me);
                me->RemoveAura(SPELL_FLASK_POISONED);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cos_flask_of_solemn_nightAI(creature);
        }
};

/// Dusk Lilly Agent - 113617
class npc_cos_dusk_lilly_agent : public CreatureScript
{
    public:
        npc_cos_dusk_lilly_agent() : CreatureScript("npc_cos_dusk_lilly_agent") { }

        enum eData
        {
            Stealth = 229112
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->isInCombat())
                return false;

            ///< Teleport outside.

            p_Player->ADD_GOSSIP_ITEM_DB(20738, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            p_Player->SEND_GOSSIP_MENU(31130, p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* /*p_Creature*/, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action != (GOSSIP_ACTION_INFO_DEF + 1))
                return false;

            p_Player->StopMoving();
            p_Player->NearTeleportTo(1011.2268f, 3795.1375f, 2.8492f, p_Player->GetOrientation());

            return true;
        }

        struct npc_cos_dusk_lilly_agentAI : public ScriptedAI
        {
            npc_cos_dusk_lilly_agentAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                me->CastSpell(me, eData::Stealth, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cos_dusk_lilly_agentAI(creature);
        }
};

class spell_cos_power_charge : public SpellScriptLoader
{
    public:
        spell_cos_power_charge() : SpellScriptLoader("spell_cos_power_charge") { }

        class spell_cos_power_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_power_charge_SpellScript);

            enum eCreatures
            {
                HeavySpear  = 76796,
                PackBeast   = 77128
            };

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetEntry() == NPC_GERDO_ARCANE_BEACON) ///< Arcane Beacon
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cos_power_charge_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cos_power_charge_SpellScript::CorrectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_power_charge_SpellScript();
        }
};

class npc_cos_bound_energy : public CreatureScript
{
    public:
        npc_cos_bound_energy() : CreatureScript("npc_cos_bound_energy") { }

        struct npc_cos_bound_energyAI : public ScriptedAI
        {
            npc_cos_bound_energyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                /// Events
                EventChargedBlast = 1,

                /// Spells
                SpellChargedBlast = 212031
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eData::EventChargedBlast, 2 * IN_MILLISECONDS);
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
                    case eData::EventChargedBlast:
                    {
                        DoCastVictim(eData::SpellChargedBlast);
                        events.ScheduleEvent(eData::EventChargedBlast, 8000);
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
            return new npc_cos_bound_energyAI(p_Creature);
        }
};

class nps_cos_arcane_manifestation : public CreatureScript
{
    public:
        nps_cos_arcane_manifestation() : CreatureScript("nps_cos_arcane_manifestation") { }

        struct nps_cos_arcane_manifestationAI : public ScriptedAI
        {
            nps_cos_arcane_manifestationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                EventDrainMagic     = 1,
                SpellDrainMagic     = 209485,
                SpellDrainMagicAura = 209490,
            };

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eData::EventDrainMagic, 5 * IN_MILLISECONDS);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eData::SpellDrainMagic:
                    {
                        DoCast(eData::SpellDrainMagicAura, true);
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
                    case eData::EventDrainMagic:
                    {
                        DoCast(eData::SpellDrainMagic);
                        events.ScheduleEvent(eData::EventDrainMagic, urand(5, 8) * IN_MILLISECONDS);
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
            return new nps_cos_arcane_manifestationAI(p_Creature);
        }
};

class npc_cos_watchful_inquisitor : public CreatureScript
{
    public:
        npc_cos_watchful_inquisitor() : CreatureScript("npc_cos_watchful_inquisitor") { }

        struct npc_cos_watchful_inquisitorAI : public ScriptedAI
        {
            npc_cos_watchful_inquisitorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                EventSearingGlare       = 1,
                EventEyeStorm           = 2,
                EventEyeDmg             = 3,
                SpellSearingGlare       = 211299,
                SpellEyeStorm           = 212784,
                SpellInquistorEye       = 213122,
                SpellEyeStormDmg        = 212791,
                CreatureWatchFulOcculus = 106584,
                CreatureInquistorsEye   = 109396
            };

            uint32 m_EyeStormDmgDiff;

            void Reset() override
            {
                events.Reset();

                m_EyeStormDmgDiff = 1000;

                SetCombatMovement(true);
                ClearDelayedOperations();

                summons.DespawnAll();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eData::EventSearingGlare, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(eData::EventEyeStorm, 20 * IN_MILLISECONDS);
            }

            void InitCircularMovement(uint64 p_WatcherGuid)
            {
                if (p_WatcherGuid)
                {
                    if (Creature* l_Watcher = Creature::GetCreature(*me, p_WatcherGuid))
                    {
                        l_Watcher->SetObjectScale(0.5f);

                        /// Creating the circle path from the center
                        Movement::MoveSplineInit l_Init(l_Watcher);

                        float l_ZR = frand(-0.3f, +0.4f);
                        float l_ZX = frand(-0.015f, +0.038f);

                        std::vector<G3D::Vector3> l_Path =
                        {
                            { me->m_positionX + frand(0.0f, 0.2f), me->m_positionY, me->m_positionZ + frand(-0.3f, 2.0f) },
                            { me->m_positionX + 0.800f, me->m_positionY - 2.0f, me->m_positionZ },
                            { me->m_positionX + 2.8f + frand(0.0f, 0.6f), me->m_positionY - 1.0f, me->m_positionZ + frand(-0.3f, 2.0f) },
                            { me->m_positionX - 1.5f,  me->m_positionY + 1.0f,  me->m_positionZ },
                            { me->m_positionX - 3.0f + frand(0.0f, 0.6f), me->m_positionY,  me->m_positionZ + frand(-0.3f, 2.0f) }
                        };

                        for (G3D::Vector3 l_Point : l_Path)
                            l_Init.Path().push_back(l_Point);

                        if (l_Init.Path().empty())
                            return;

                        l_Init.SetVelocity(frand(6.0f, 8.20f));
                        l_Init.SetBackward();
                        l_Init.SetCyclic();
                        l_Init.SetSmooth();
                        l_Init.SetCyclic();
                        l_Init.Launch();

                        SetFlyMode(true);
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eData::SpellEyeStorm)
                {
                    m_EyeStormDmgDiff = 1000;

                    for (int8 l_I = 0; l_I < 5; l_I++)
                    {
                        if (me->GetVehicleKit()->GetPassenger(l_I) == nullptr)
                        {
                            Creature* l_Eye = nullptr;

                            l_Eye = me->SummonCreature(eData::CreatureInquistorsEye, *me, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 7 * TimeConstants::IN_MILLISECONDS);

                            if (l_Eye == nullptr)
                                return;

                            InitCircularMovement(l_Eye->GetGUID());
                        }
                    }
                }
            }

            void OnSpellInterrupted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id != eData::SpellSearingGlare)
                    return;

                std::list<Creature*> l_Oculi;
                me->GetCreatureListWithEntryInGrid(l_Oculi, eData::CreatureWatchFulOcculus, 40);
                for (Unit* l_Itr : l_Oculi)
                {
                    if (l_Itr->ToCreature())
                        l_Itr->ToCreature()->DespawnOrUnsummon();
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon != nullptr)
                {
                    if (p_Summon->GetEntry() == eData::CreatureWatchFulOcculus || p_Summon->GetEntry() == eData::CreatureInquistorsEye)
                    {
                        p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Summon->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_EyeStormDmgDiff <= p_Diff)
                {
                    if (me->HasAura(eData::SpellEyeStorm))
                    {
                        std::list<Player*> l_NerbyPlayersList;
                        me->GetPlayerListInGrid(l_NerbyPlayersList, 3.0f);

                        for (Player* l_Iter : l_NerbyPlayersList)
                        {
                            me->CastSpell(l_Iter, eData::SpellEyeStormDmg, true);
                        }
                    }

                    m_EyeStormDmgDiff = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_EyeStormDmgDiff -= p_Diff;

                UpdateOperations(p_Diff);
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eData::EventSearingGlare:
                    {
                        for (uint32 l_I = 0; l_I < 2; l_I++)
                        {
                            Position l_Pos = *me;
                            Position l_Dest;

                            l_Pos.SimplePosXYRelocationByAngle(l_Dest, frand(6.0f, 9.0f), frand(0.0f, 2 * M_PI));

                            me->SummonCreature(eData::CreatureWatchFulOcculus, l_Dest, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 6 * TimeConstants::IN_MILLISECONDS);
                        }

                        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                        {
                            if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.f, true))
                                 me->CastSpell(l_Target, eData::SpellSearingGlare, false);
                        });

                        events.ScheduleEvent(eData::EventSearingGlare, frand(8 * IN_MILLISECONDS, 15 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eData::EventEyeStorm:
                    {
                        events.DelayEvent(EventSearingGlare, 4 * TimeConstants::IN_MILLISECONDS);

                        if (Unit* l_target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(l_target, eData::SpellEyeStorm, false);

                        events.ScheduleEvent(eData::EventEyeStorm, 25 * IN_MILLISECONDS);
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
            return new npc_cos_watchful_inquisitorAI(p_Creature);
        }
};

class npc_cos_watchful_oculus : public CreatureScript
{
    public:
        npc_cos_watchful_oculus() : CreatureScript("npc_cos_watchful_oculus") { }

        struct npc_cos_watchful_oculusAI : public ScriptedAI
        {
            npc_cos_watchful_oculusAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                EventSearchTarget    = 1,
                EventSearingGlareDmg = 2,
                SpellSearingGlareDmg = 211300
            };

            void TacticRandomOrientation()
            {
                std::list<Player*> l_PlayerList;
                me->GetPlayerListInGrid(l_PlayerList, 50.0f);
                if (l_PlayerList.empty())
                    return;

                JadeCore::Containers::RandomResizeList(l_PlayerList, uint32(1));
                if (Player* l_Player = l_PlayerList.front())
                {
                    me->SetFacingToObject(l_Player);
                    me->SetOrientation(me->GetAngle(l_Player));
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                }
            }

            void Reset() override
            {
                events.Reset();
                me->setFaction(16); ///< Aggressive
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                TacticRandomOrientation();
                events.ScheduleEvent(eData::EventSearchTarget, 2* IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eData::EventSearchTarget:
                    {
                        TacticRandomOrientation();
                        events.ScheduleEvent(eData::EventSearingGlareDmg, 2 * IN_MILLISECONDS);
                        break;
                    }
                    case eData::EventSearingGlareDmg:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 50.0f);
                        if (l_PlayerList.empty())
                        {
                            events.ScheduleEvent(eData::EventSearchTarget, 1 * IN_MILLISECONDS);
                            break;
                        }

                        l_PlayerList.remove_if([this](Player* p_Player) -> bool
                        {
                            if (!p_Player || !me->isInFront(p_Player))
                                return true;

                            return false;
                        });

                        if (l_PlayerList.empty())
                        {
                            events.ScheduleEvent(eData::EventSearchTarget, 1 * IN_MILLISECONDS);
                            break;
                        }

                        JadeCore::Containers::RandomResizeList(l_PlayerList, uint32(1));

                        if (Player* l_Player = l_PlayerList.front())
                            DoCast(l_Player, eData::SpellSearingGlareDmg);

                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        events.ScheduleEvent(eData::EventSearchTarget, 1 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_watchful_oculusAI(p_Creature);
        }
};

class npc_cos_duskwatch_guard : public CreatureScript
{
    public:
        npc_cos_duskwatch_guard() : CreatureScript("npc_cos_duskwatch_guard") { }

        struct npc_cos_duskwatch_guardAI : public ScriptedAI
        {
            npc_cos_duskwatch_guardAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                /// Events
                EventQuellingStrike = 1,
                EventFortification = 2,

                /// Spells
                SpellQuellingStrike = 209027,
                SpellFortification  = 209033,
            };

            void EnterCombat(Unit* p_Who) override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventQuellingStrike, urand(6, 7) * IN_MILLISECONDS);
                events.ScheduleEvent(eData::EventFortification, urand(10, 11) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    return;
                }
                else
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                switch (events.ExecuteEvent())
                {
                    case eData::EventQuellingStrike:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eData::SpellQuellingStrike, false);

                        events.ScheduleEvent(eData::EventQuellingStrike, 15000);
                        break;
                    }
                    case eData::EventFortification:
                    {
                        me->CastSpell(me, eData::SpellFortification, false);
                        events.ScheduleEvent(eData::EventFortification, 15000);
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
            return new npc_cos_duskwatch_guardAI(p_Creature);
        }
};

class npc_cos_suramar_citizen : public CreatureScript
{
    public:
        npc_cos_suramar_citizen() : CreatureScript("npc_cos_suramar_citizen") { }

        struct npc_cos_suramar_citizenAI : public ScriptedAI
        {
            npc_cos_suramar_citizenAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                l_InstanceScript = p_Creature->GetInstanceScript();
            }

            uint8 l_Count = 0;

            uint32 l_Diff;

            uint32 m_TrickyWaypointCount = 0;

            bool m_StairwayEvent = false;

            bool m_Startled;

            InstanceScript* l_InstanceScript = nullptr;

            void Reset() override
            {
                ClearDelayedOperations();

                m_Startled = false;

                l_Diff = 2.5f * TimeConstants::IN_MILLISECONDS;

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->GetEntry() == eCreatures::NPC_PATROL_CAPTAIN_GERDO)
                {
                    AddTimedDelayedOperation(1.5f * IN_MILLISECONDS, [this]() -> void
                    {
                        m_StairwayEvent = true;

                        Talk(0);
                    });
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (m_StairwayEvent)
                    return;

                me->DeMorph();
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID != 1)
                    return;

                if (m_TrickyWaypointCount >= 14)
                    return;

                me->GetMotionMaster()->MovePoint(1, g_StarledPackRoom[2][m_TrickyWaypointCount++]);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (l_Diff <= p_Diff)
                {
                    if (m_Startled)
                        return;

                    if (InstanceScript* l_InsScript = me->GetInstanceScript())
                    {
                        if (Creature* l_Talixae = l_InsScript->instance->GetCreature(l_InsScript->GetData64(DATA_TALIXAE)))
                        {
                            if (Creature* l_Gerdo = l_InsScript->instance->GetCreature(l_InsScript->GetData64(DATA_CAPTAIN_GERDO)))
                            {
                                if (l_Talixae->isDead() || l_Gerdo->isAlive())
                                    return;

                                if (Player* l_Nearest = me->FindNearestPlayer(15.f))
                                {
                                    if (m_StairwayEvent && !m_Startled)
                                    {
                                        m_Startled = true;

                                        Talk(1);

                                        me->GetMotionMaster()->Clear(false);
                                        me->GetMotionMaster()->MovePoint(0, 1288.8662f, 3453.9099f, 30.3246f);
                                    }
                                    else if (!m_Startled)
                                    {
                                        m_Startled = true;

                                        me->SetSpeed(UnitMoveType::MOVE_RUN, 1.35f, true);

                                        me->GetMotionMaster()->MoveRandom(30.f);

                                        AddTimedDelayedOperation(1.0f * IN_MILLISECONDS, [this]() -> void
                                        {
                                            me->GetMotionMaster()->Clear(false);

                                            me->SetWalk(false);
                                            me->SetSpeed(UnitMoveType::MOVE_RUN, 1.5f, true);

                                            if (me->GetDistance(g_StarledPackRoom[0][0].x, g_StarledPackRoom[0][0].y, g_StarledPackRoom[0][0].z) <= 70.0f && me->GetPositionZ() <= 52.0f && me->GetPositionZ() >= 25.0f)
                                            {
                                                if (me->IsOutdoors())
                                                {
                                                    /// Two outside building.
                                                    if (me->GetDistance(1312.322f, 3464.821f, 32.615f) <= 80.0f || me->GetDistance(1239.508f, 3580.863f, 34.366f) <= 80.0f)
                                                    {
                                                        Talk(2);

                                                        me->GetMotionMaster()->MovePoint(0, 1297.300f, 3476.809f, 30.190f);

                                                        AddTimedDelayedOperation(2.5f * IN_MILLISECONDS, [this]() -> void
                                                        {
                                                            me->GetMotionMaster()->Clear(false);
                                                            me->GetMotionMaster()->MoveSmoothPath(0, g_StarledPackRoom[0].data(), g_StarledPackRoom[0].size(), false);

                                                            me->DespawnOrUnsummon(12 * TimeConstants::IN_MILLISECONDS);
                                                        });
                                                    }
                                                }
                                                else
                                                {
                                                    Talk(2);

                                                    me->GetMotionMaster()->Clear(false);
                                                    me->GetMotionMaster()->MoveSmoothPath(0, g_StarledPackRoom[1].data(), g_StarledPackRoom[1].size(), false);

                                                    me->DespawnOrUnsummon(12 * TimeConstants::IN_MILLISECONDS);
                                                }
                                            }
                                            else if (me->GetDistance(g_StarledPackRoom[1][0].x, g_StarledPackRoom[1][0].y, g_StarledPackRoom[1][0].z) <= 70.0f)
                                            {
                                                Talk(2);

                                                me->GetMotionMaster()->Clear(false);
                                                me->GetMotionMaster()->MovePoint(1, g_StarledPackRoom[2][0], true);

                                                me->DespawnOrUnsummon(15 * TimeConstants::IN_MILLISECONDS);
                                            }
                                            else if (me->GetDistance(g_StarledPackRoom[2][0].x, g_StarledPackRoom[2][0].y, g_StarledPackRoom[2][0].z) <= 75.0f)
                                            {
                                                Talk(2);

                                                me->GetMotionMaster()->Clear(false);
                                                me->GetMotionMaster()->MoveSmoothPath(0, g_StarledPackRoom[2].data(), g_StarledPackRoom[2].size(), false);

                                                me->DespawnOrUnsummon(15 * TimeConstants::IN_MILLISECONDS);
                                            }
                                            else if (me->GetDistance(g_StarledPackRoom[3][0].x, g_StarledPackRoom[3][0].y, g_StarledPackRoom[3][0].z) <= 45.0f)
                                            {
                                                Talk(2);

                                                me->GetMotionMaster()->Clear(false);
                                                me->GetMotionMaster()->MoveSmoothPath(0, g_StarledPackRoom[3].data(), g_StarledPackRoom[3].size(), false);

                                                me->DespawnOrUnsummon(15 * TimeConstants::IN_MILLISECONDS);
                                            }
                                            else if (me->GetDistance(g_StarledPackRoom[4][0].x, g_StarledPackRoom[4][0].y, g_StarledPackRoom[4][0].z) <= 55.0f)
                                            {
                                                Talk(2);

                                                me->GetMotionMaster()->Clear(false);
                                                me->GetMotionMaster()->MoveSmoothPath(0, g_StarledPackRoom[4].data(), g_StarledPackRoom[4].size(), false);

                                                me->DespawnOrUnsummon(15 * TimeConstants::IN_MILLISECONDS);
                                            }
                                            else if (me->GetDistance(g_StarledPackRoom[5][0].x, g_StarledPackRoom[5][0].y, g_StarledPackRoom[5][0].z) <= 80.0f)
                                            {
                                                Talk(2);

                                                me->GetMotionMaster()->Clear(false);
                                                me->GetMotionMaster()->MoveSmoothPath(0, g_StarledPackRoom[5].data(), g_StarledPackRoom[5].size(), false);

                                                me->DespawnOrUnsummon(15 * TimeConstants::IN_MILLISECONDS);
                                            }
                                        });
                                    }
                                }
                            }
                        }
                    }

                    l_Diff = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    l_Diff -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_suramar_citizenAI(p_Creature);
        }
};

class spell_cos_quelling_strike : public SpellScriptLoader
{
    public:
        spell_cos_quelling_strike() : SpellScriptLoader("spell_cos_quelling_strike") { }

        class spell_cos_quelling_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_quelling_strike_SpellScript);

            Position l_Pos = Position();

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();

                if (GetCaster() == nullptr)
                    return;

                l_Caster->AttackStop();

                l_Caster->StopMoving();

                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                l_Caster->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                l_Caster->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
            }

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                    {
                        uint64 l_Guid = l_Caster->GetGUID();

                        l_Instance->AddTimedDelayedOperation(1.5f * IN_MILLISECONDS, [this, l_Guid]() -> void
                        {
                            if (Creature* l_Caster = sObjectAccessor->FindCreature(l_Guid))
                            {
                                if (Player* l_Nearest = l_Caster->FindNearestPlayer(30.f))
                                {
                                    if (UnitAI* l_AI = l_Caster->GetAI())
                                        l_AI->AttackStart(l_Nearest);
                                }

                                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                l_Caster->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                                l_Caster->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                            }
                        });
                    }
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !l_Caster->isInFront(p_Object, 1.81514f)) ///< Arcane Beacon
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_cos_quelling_strike_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_cos_quelling_strike_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cos_quelling_strike_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cos_quelling_strike_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_quelling_strike_SpellScript();
        }
};

/// Signaling.. 209524
class spell_cos_signaling : public SpellScriptLoader
{
    public:
        spell_cos_signaling() : SpellScriptLoader("spell_cos_signaling") { }

        class spell_cos_signaling_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_signaling_SpellScript);

            void HandleTriggerEffect(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster())
                    return;

                Unit* l_Caster = GetCaster();

                if (Creature* l_SignalLantern = l_Caster->FindNearestCreature(eCreatures::NPC_SIGNAL_LANTERN, 100.0f, true))
                {
                    if (CreatureAI* l_AI = l_SignalLantern->AI())
                        l_AI->DoAction(1);
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_cos_signaling_SpellScript::HandleTriggerEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_signaling_SpellScript();
        }
};

/// Wild Magic - 216105
/// Wild Magic - 216760
class spell_cos_wild_magic : public SpellScriptLoader
{
    public:
        spell_cos_wild_magic() : SpellScriptLoader("spell_cos_wild_magic") { }

        class spell_cos_wild_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_wild_magic_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                if (!GetCaster())
                    return;

                Position l_Pos;
                GetCaster()->GetRandomNearPosition(l_Pos, 15.0f);
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                l_Dest->Relocate(l_Pos);

                SetExplTargetDest(*l_Dest);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_cos_wild_magic_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_wild_magic_SpellScript();
        }
};

/// Searing Gaze - 211299
class spell_cos_searing_gaze : public SpellScriptLoader
{
    public:
        spell_cos_searing_gaze() : SpellScriptLoader("spell_cos_searing_gaze") { }

        class spell_cos_searing_gaze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_searing_gaze_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->ToCreature())
                        return;

                    if (CreatureAI* l_AI = l_Caster->ToCreature()->AI())
                        l_AI->DoAction(0);
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_cos_searing_gaze_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_searing_gaze_SpellScript();
        }
};

class spell_cos_streetsweeper_damage : public SpellScriptLoader
{
    public:
        spell_cos_streetsweeper_damage() : SpellScriptLoader("spell_cos_streetsweeper_damage") { }

        class spell_cos_streetsweeper_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_streetsweeper_damage_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_NpcStreetsweeper = l_Caster->FindNearestCreature(110403, 200.0f);
                if (!l_NpcStreetsweeper)
                    return;

                l_NpcStreetsweeper->DespawnOrUnsummon(1000);
            }

            void SelectTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_NpcStreetsweeper = l_Caster->FindNearestCreature(110403, 200.0f);
                if (!l_NpcStreetsweeper)
                    return;

                p_Targets.remove_if([this, l_NpcStreetsweeper](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !l_NpcStreetsweeper->isInFront(p_Object, M_PI / 7.5f))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cos_streetsweeper_damage_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_cos_streetsweeper_damage_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_streetsweeper_damage_SpellScript();
        }
};

///< Duskwatch Reinforcement 107073
class npc_cos_duskwatch_reinforcement : public CreatureScript
{
    public:
        npc_cos_duskwatch_reinforcement() : CreatureScript("npc_cos_duskwatch_reinforcement") { }

        struct npc_cos_duskwatch_reinforcementAI : public ScriptedAI
        {
            npc_cos_duskwatch_reinforcementAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                EventSubdue  = 1,
                SPELL_SUBDUE = 212773
            };

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoZoneInCombat(me, 100.0f);
                events.ScheduleEvent(eData::EventSubdue, urand(5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
            }

            void EnterEvadeMode() override
            {
                Reset();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->LoadCreaturesAddon();
                me->ClearLootContainers();
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
                    case eData::EventSubdue:
                    {
                        DoCastVictim(SPELL_SUBDUE);
                        events.ScheduleEvent(eData::EventSubdue, urand(13 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_duskwatch_reinforcementAI(p_Creature);
        }
};
///< 104295 Blazing Imp
class npc_cos_blazing_imp : public VehicleScript
{
    public:
        npc_cos_blazing_imp() : VehicleScript("npc_cos_blazing_imp") { }

        struct npc_cos_blazing_impAI : public ScriptedAI
        {
            npc_cos_blazing_impAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            enum eData
            {
                EventDriftingEmbers    = 1,
                EventFirebolt,

                SpellDriftingEmbers       = 211401,
                SpellDriftingEmbersTarget = 211408,
                SpellDriftingEmbersDmg    = 211412,
                SpellFirebolt             = 211406
            };

            void EnterCombat(Unit* p_Attacker) override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventDriftingEmbers, 3 * IN_MILLISECONDS);
                events.ScheduleEvent(eData::EventFirebolt, 8 * IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eData::SpellDriftingEmbersTarget)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                        me->CastSpell(l_Target, eData::SpellDriftingEmbersDmg, true);
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
                    case eData::EventDriftingEmbers:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(me, eData::SpellDriftingEmbers, false);
                        events.ScheduleEvent(eData::EventDriftingEmbers, urand(9, 13) * IN_MILLISECONDS);
                        break;
                    }
                    case eData::EventFirebolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eData::SpellFirebolt, false);
                        events.ScheduleEvent(eData::EventFirebolt, urand(9, 13) * IN_MILLISECONDS);
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
            return new npc_cos_blazing_impAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_patrol_captain_gerdo()
{
    /// Boss
    new boss_patrol_captain_gerdo();

    /// Creatures
    new npc_captain_gerdo_vigilant_duskwatch();
    new npc_signal_lantern();
    new npc_lilet_lunarh();
    new npc_duskwatch_sentry();
    new npc_gerdo_arcane_beacon();
    new npc_gerdo_streetsweeper();
    new npc_cos_nightborne_boat();
    new npc_cos_resonant_stalker();
    new npc_cos_flask_of_solemn_night();
    new npc_cos_bound_energy();
    new npc_cos_suramar_citizen();
    new nps_cos_arcane_manifestation();
    new npc_cos_guardian_construct();
    new npc_cos_duskwatch_guard();
    new npc_cos_watchful_inquisitor();
    new npc_cos_watchful_oculus();
    new npc_cos_dusk_lilly_agent();
    new npc_cos_arcanist_marlod();
    new npc_cos_duskwatch_reinforcement();
    new npc_cos_blazing_imp();

    /// Spell
    new spell_cos_signaling();
    new spell_patrol_arcane_lockdown();
    new spell_cos_wild_magic();
    new spell_cos_searing_gaze();
    new spell_cos_power_charge();
    new spell_cos_quelling_strike();
    new spell_cos_streetsweeper_damage();
}
#endif
