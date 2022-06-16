////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "maw_of_souls.hpp"

enum Says
{
    SAY_INTRO    = 0,
    SAY_INTRO_2  = 1,
    SAY_AGGRO    = 2,
    SAY_SCREAMS  = 3,
    SAY_WINDS    = 4,
    SAY_DARK     = 5,
    SAY_DEATH    = 6,
    SAY_WIPE     = 7,
    SAY_KILL     = 8,
    SAY_ARISE    = 9,
};

enum Spells
{
    SPELL_ZERO_REGEN            = 118357,
    SPELL_POWER                 = 167922,
    SPELL_KNEELING_VISUAL       = 197227, ///< State
    SPELL_SOUL_SIPHON           = 194665, ///< HitTarget npc 102375
    SPELL_SOUL_SIPHON_TRIGGERED = 243029,
    SPELL_DARK_SLASH            = 193211,
    SPELL_SCREAMS_OF_DEAD       = 193364,
    SPELL_WINDS_OF_NORTHREND    = 193977,
    SPELL_BANE                  = 193460,
    SPELL_BANE_MISSILE_AT       = 193463,
    SPELL_BANE_NOVA             = 200194,
    SPELL_BANE_AT               = 193465,
    SPELL_ARISE_FALLEN          = 193566,
    SPELL_ARISE_FALLEN_SUMMON   = 193594,
    SPELL_VIGOR_AURA            = 203816,

    /// Trash
    SPELL_STATE                 = 196025,
    SPELL_INSTAKILL             = 118443,
    SPELL_FRACTURE              = 198752,
    SPELL_BARBED_SPEAR          = 194674,
    SPELL_SWIRLING_MUCK         = 201567,

    SPELL_BONUS_ROLL            = 226637,

    SPELL_DAMNED_SOUL           = 213441
};

enum eEvents
{
    EVENT_DARK_SLASH            = 1,
    EVENT_SCREAMS_OF_DEAD       = 2,
    EVENT_WINDS_OF_NORTHREND    = 3,
    EVENT_BANE                  = 4,
    EVENT_INTRO                 = 5,
    ///EVENT_INTRO_2               = 6,

    /// Trash
    EVENT_TRASH_INTRO           = 7,
    EVENT_TRASH_INTRO_2         = 8,
    EVENT_FRACTURE              = 9,
    EVENT_BARBED                = 10,
    EVENT_SWIRLING              = 11,
    EVENT_ARISE_FALLEN          = 12,
    EVENT_VIGOR                 = 13
};

enum ePoints
{
    POINT_KNEEL  = 100
};

/// Ymiron, the Fallen King - 96756
class boss_ymiron_the_fallen_king : public CreatureScript
{
    public:
        boss_ymiron_the_fallen_king() : CreatureScript("boss_ymiron_the_fallen_king") { }

        enum eVisual
        {
            VisualAriseFallen = 50259
        };

        struct boss_ymiron_the_fallen_kingAI : public BossAI
        {
            boss_ymiron_the_fallen_kingAI(Creature* p_Creature) : BossAI(p_Creature, DATA_YMIRON) { }

            bool m_FirstSlash = true;
            bool m_FirstScream = true;

            bool m_Achievement = false;
            uint32 m_SoldierCount = 0;

            void InitializeAI() override
            {
                BossAI::InitializeAI();

                m_Phase = 7;
                m_Timer = 0;
                m_StartIntro = false;
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == ePoints::POINT_KNEEL)
                {
                    if (GameObject* l_GameObject = me->FindNearestGameObject(GO_YMIRON_GORN, 30.0f))
                        me->SetFacingToObject(l_GameObject);

                    m_StartIntro = false;

                    DoCast(SPELL_KNEELING_VISUAL);
                    m_Phase = 6;
                    m_Timer = 1000;

                    AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->SetWalk(false);
                    });
                }
            }

            void Reset() override
            {
                _Reset();
                me->RemoveAurasDueToSpell(SPELL_POWER);
                DoCast(me, SPELL_ZERO_REGEN, true);
                me->SetMaxPower(POWER_MANA, 100);
                me->SetPower(POWER_MANA, 80);

                if (instance != nullptr)
                {
                    if (GameObject* l_GameObject = instance->instance->GetGameObject(instance->GetData64(DATA_YMIRON_GORN)))
                        l_GameObject->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                    if (instance->GetData(eData::DATA_YMIRON_RP_EVENT) == EncounterState::DONE)
                    {
                        m_Phase = 7;
                        m_Timer = 0;
                        m_StartIntro = false;
                    }
                }
            }

            void EnterCombat(Unit* p_Who) override
            {
                if (m_StartIntro)
                {
                    float l_Angle = 0.f;

                    if (GameObject* l_Horn = me->FindNearestGameObject(GO_YMIRON_GORN, 30.0f))
                        l_Angle = me->GetAngle(l_Horn);

                    me->SetHomePosition(Position{ 7396.58f, 7276.02f, 43.79f, l_Angle });
                    me->SetWalk(false);
                    me->CastStop();
                    m_Phase = 7;
                    m_Timer = 0;
                    m_StartIntro = false;
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();

                    if (Unit* l_Victim = me->getVictim())
                        me->GetMotionMaster()->MoveChase(l_Victim);
                }

                m_FirstSlash = true;
                m_FirstScream = true;

                m_Achievement = false;
                m_SoldierCount = 0;

                Talk(SAY_AGGRO);

                _EnterCombat();

                DoCast(me, SPELL_POWER, true);

                events.ScheduleEvent(EVENT_SCREAMS_OF_DEAD, 6000);
                events.ScheduleEvent(EVENT_WINDS_OF_NORTHREND, 16000);
                events.ScheduleEvent(EVENT_BANE, 22000);

                me->SetWalk(false);
                me->RemoveAurasDueToSpell(SPELL_KNEELING_VISUAL);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    Talk(Says::SAY_KILL);
            }

            void JustReachedHome() override
            {
                Talk(Says::SAY_WIPE);
                BossAI::JustReachedHome();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                if (!instance)
                    return;

                if (GameObject* l_GameObject = instance->instance->GetGameObject(instance->GetData64(DATA_YMIRON_GORN)))
                    l_GameObject->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);

                instance->DoCastSpellOnPlayers(SPELL_DAMNED_SOUL);

                if (m_Achievement && IsMythic())
                    instance->DoCompleteAchievement(eAchievements::InstantKarma);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_ARISE_FALLEN:
                    {
                        std::list<AreaTrigger*> l_ATList;
                        me->GetAreaTriggerList(l_ATList, SPELL_BANE_AT);

                        std::vector<uint64> plrGuids;
                        auto threatList = me->getThreatManager().getThreatList();
                        for (auto& ref : threatList)
                            if (auto plr = ObjectAccessor::GetPlayer(*me, ref->getUnitGuid()))
                                plrGuids.push_back(plr->GetGUID());

                        uint32 atSize = l_ATList.size();
                        for (uint32 i = 0; i < atSize; ++i)
                        {
                            for (auto& pGuid : plrGuids)
                            {
                                if (auto plr = ObjectAccessor::GetPlayer(*me, pGuid))
                                {
                                    if (plr->isAlive())
                                    {
                                        if (l_ATList.empty())
                                            return;

                                        auto at = l_ATList.front();
                                        if (at)
                                        {
                                            Position l_Pos;
                                            plr->GetRandomNearPosition(l_Pos, 5.0f);

                                            float dist = me->GetExactDist2d(plr->GetPositionX(), plr->GetPositionY());
                                            float speed = dist / 2;
                                            float l_Time = dist / speed;

                                            me->PlayOrphanSpellVisual(at->GetPosition().AsVector3(), G3D::Vector3(), l_Pos.AsVector3(), eVisual::VisualAriseFallen, l_Time);

                                            AddTimedDelayedOperation(uint32(l_Time * TimeConstants::IN_MILLISECONDS), [this, l_Pos]() -> void
                                                {
                                                    me->CastSpell(l_Pos, SPELL_ARISE_FALLEN_SUMMON, true);
                                                });

                                            l_ATList.remove(at);
                                            at->Despawn();
                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == NPC_RISEN_SOLDIER)
                {
                    if ((++m_SoldierCount) >= 6)
                        m_Achievement = true;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == ACTION_1)
                    events.ScheduleEvent(EVENT_DARK_SLASH, 500);
                else if (p_Action == ACTION_2)
                {
                    me->SetWalk(true);
                    m_Timer = 1000;
                    m_Phase = 0;
                    m_StartIntro = true;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Spell->GetSpellInfo()->Id == Spells::SPELL_SOUL_SIPHON_TRIGGERED && !p_Targets.empty())
                {
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object->ToUnit() || !p_Object->ToUnit()->HasAura(VEHICLE_SPELL_RIDE_HARDCODED) || p_Object->ToUnit()->isInCombat())
                            return true;

                        return false;
                    });

                    for (WorldObject* p_Itr : p_Targets)
                    {
                        if (!p_Itr || !p_Itr->ToUnit())
                            continue;

                        if (p_Itr->GetEntry() == NPC_RUNECARVER)
                            p_Itr->ToUnit()->DelayedCastSpell(p_Itr, Spells::SPELL_INSTAKILL, true, 10200);
                    }
                }
            }

            void ExecuteEvent(uint32 const p_EventID) override
            {
                switch (p_EventID)
                {
                    case EVENT_DARK_SLASH:
                    {
                        DoCastVictim(SPELL_DARK_SLASH);

                        if (!m_FirstSlash)
                            Talk(SAY_DARK);

                        m_FirstSlash = false;
                        break;
                    }
                    case EVENT_SCREAMS_OF_DEAD:
                    {
                        DoCast(SPELL_SCREAMS_OF_DEAD);
                        events.ScheduleEvent(EVENT_SCREAMS_OF_DEAD, 23000);

                        if (!m_FirstScream)
                            Talk(SAY_SCREAMS);

                        m_FirstScream = false;
                        break;
                    }
                    case EVENT_WINDS_OF_NORTHREND:
                    {
                        DoCast(SPELL_WINDS_OF_NORTHREND);
                        events.ScheduleEvent(EVENT_WINDS_OF_NORTHREND, 24000);
                        Talk(SAY_WINDS);
                        break;
                    }
                    case EVENT_BANE:
                    {
                        DoCast(SPELL_BANE);
                        events.ScheduleEvent(EVENT_BANE, 50000);

                        if (IsHeroicOrMythic())
                            events.ScheduleEvent(EVENT_ARISE_FALLEN, 18 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case EVENT_ARISE_FALLEN:
                    {
                        Talk(Says::SAY_ARISE);
                        DoCast(SPELL_ARISE_FALLEN);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() && !m_StartIntro)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventID = events.ExecuteEvent())
                    ExecuteEvent(l_EventID);

                DoMeleeAttackIfReady();

                if (m_StartIntro)
                {
                    switch (m_Phase)
                    {
                        case 0:
                        {
                            if (m_Timer < p_Diff)
                            {
                                me->GetMotionMaster()->MoveJump(7406.41f, 7280.58f, 44.26f, 30.0f, 10.0f);
                                m_Phase++;
                                m_Timer = 5000;
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        case 1:
                        {
                            if (m_Timer < p_Diff)
                            {
                                me->GetMotionMaster()->MovePoint(0, 7408.16f, 7293.06f, 43.78f);
                                m_Phase++;
                                m_Timer = 5000;
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        case 2:
                        {
                            if (m_Timer < p_Diff)
                            {
                                me->GetMotionMaster()->MovePoint(0, 7397.77f, 7297.18f, 43.78f);
                                m_Phase++;
                                m_Timer = 5000;
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        case 3:
                        {
                            if (m_Timer < p_Diff)
                            {
                                std::list<Creature*> l_CreatureList;
                                me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_RUNECARVER, 18.0f);

                                DoCast(SPELL_SOUL_SIPHON);
                                Talk(SAY_INTRO);
                                m_Phase++;
                                m_Timer = 6000;
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        case 4:
                        {
                            if (m_Timer < p_Diff)
                            {
                                me->GetMotionMaster()->MovePoint(ePoints::POINT_KNEEL, 7396.58f, 7276.02f, 43.79f);
                                Talk(SAY_INTRO_2);
                                m_Phase++;
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            private:
                bool m_StartIntro;
                uint32 m_Timer;
                int m_Phase;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_ymiron_the_fallen_kingAI(p_Creature);
        }
};

/// Start-trigger - 950001
class npc_trigger_mos_start : public CreatureScript
{
    public:
        npc_trigger_mos_start() : CreatureScript("npc_trigger_mos_start") { }

        struct npc_trigger_mos_startAI : public ScriptedAI
        {
            npc_trigger_mos_startAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
                m_IntroDone = false;
                m_Pull = false;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 45.0f))
                {
                    m_IntroDone = true;

                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListWithEntryInGrid(l_CreatureList, NPC_BIRD, 50.0f);

                    for (Creature*l_Iter : l_CreatureList)
                    {
                        l_Iter->RemoveAurasDueToSpell(SPELL_STATE);
                        l_Iter->GetMotionMaster()->MovePoint(0, 7257.08f, 7081.54f, 26.41f);
                    }
                }

                if (!m_Pull && me->IsWithinDistInMap(p_Who, 30.0f))
                {
                    m_Pull = true;

                    if (Creature* l_Trigger = me->FindNearestCreature(NPC_RUNECARVER, 20.0f, true))
                    {
                        l_Trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                        if (Creature* l_Target = me->FindNearestCreature(NPC_SEACURSED_SLAVER, 30.0f, true))
                        {
                            if (l_Target->IsAIEnabled)
                                l_Target->AI()->DoAction(ACTION_2);
                        }

                        if (Creature* l_Ymiron = me->FindNearestCreature(NPC_YMIRON, 130.0f, true))
                        {
                            if (l_Ymiron->IsAIEnabled)
                                l_Ymiron->AI()->DoAction(ACTION_2);
                        }
                    }

                    if (Creature* l_Trigger = me->FindNearestCreature(NPC_SHIELDMAIDEN, 20.0f, true))
                    {
                        if (l_Trigger->IsAIEnabled)
                            l_Trigger->AI()->Talk(0);

                        l_Trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                        if (Creature* l_Target = me->FindNearestCreature(NPC_SEACURSED_SLAVER, 30.0f, true))
                        {
                            if (l_Target->IsAIEnabled)
                                l_Target->AI()->DoAction(ACTION_1);
                        }
                    }
                }

                ScriptedAI::MoveInLineOfSight(p_Who);
            }

            private:
                bool m_IntroDone;
                bool m_Pull;
                InstanceScript* m_Instance;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trigger_mos_startAI(p_Creature);
        }
};

/// Risen Warrior - 98246
class npc_ymiron_risen_warrior : public CreatureScript
{
    public:
        npc_ymiron_risen_warrior() : CreatureScript("npc_ymiron_risen_warrior") { }

        struct npc_ymiron_risen_warriorAI : public ScriptedAI
        {
            npc_ymiron_risen_warriorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                HandleVigor();
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
                    case EVENT_VIGOR:
                    {
                        HandleVigor();
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void HandleVigor()
            {
                DoCast(SPELL_VIGOR_AURA);

                events.ScheduleEvent(EVENT_VIGOR, 6000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ymiron_risen_warriorAI(p_Creature);
        }
};

/// Seacursed Slaver - 97043
class npc_referee : public CreatureScript
{
    public:
        npc_referee() : CreatureScript("npc_referee") { }

        struct npc_refereeAI : public ScriptedAI
        {
            npc_refereeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->RemoveAllAuras();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                std::list<Creature*> l_WeaponsVehicle;

                me->GetCreatureListWithEntryInGrid(l_WeaponsVehicle, NPC_WEAPON_VEHICLE, 50.f);

                l_WeaponsVehicle.sort(JadeCore::DistanceOrderPred(me));

                for (auto & l_Itr : l_WeaponsVehicle)
                {
                    Vehicle* l_Vec = l_Itr->GetVehicleKit();

                    if (l_Vec && !l_Vec->IsVehicleInUse())
                    {
                        l_Itr->EnterVehicle(me);
                        break;
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == ACTION_1)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    m_Events.ScheduleEvent(EVENT_TRASH_INTRO, 3000);
                    m_IntroEvent = false;
                }
                else if (p_Action == ACTION_2)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    m_Events.ScheduleEvent(EVENT_TRASH_INTRO_2, 1000);
                    m_IntroEvent = false;
                }
            }

            void HandleIntro()
            {
                if (m_IntroEvent)
                    return;

                Position l_FirstCage = { 7267.00f, 7289.00f, 27.160f };

                if (me->IsNearPosition(&l_FirstCage, 25.f))
                    DoAction(ACTION_1);
                else
                    DoAction(ACTION_2);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                HandleIntro();
                m_Events.ScheduleEvent(EVENT_FRACTURE, 15000);
                m_Events.ScheduleEvent(EVENT_BARBED, 9000);
                m_Events.ScheduleEvent(EVENT_SWIRLING, 17000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (urand(0, 2))
                    Talk(0);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                if (p_Spell->GetSpellInfo()->Id == SPELL_BARBED_SPEAR)
                {
                    p_Targets.clear();

                    if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllRanged, { }, 8.0f))
                        p_Targets.push_back(l_Target);
                    else if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                        p_Targets.push_back(l_Target);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() && m_IntroEvent)
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case EVENT_TRASH_INTRO:
                    {
                        if (m_IntroEvent)
                            break;

                        /*me->GetMotionMaster()->MovePoint(0, 7267.00f, 7289.00f, 27.16f);

                        Talk(0);

                        if (GameObject* l_GameObject = me->FindNearestGameObject(GO_KVALDIR_CAGE, 20.0f))
                            l_GameObject->SetGoState(GO_STATE_ACTIVE);

                        Talk(1);

                        if (Unit* l_Target = me->FindNearestPlayer(100.0f))
                        {
                            if (Creature* l_Trigger = me->FindNearestCreature(NPC_SHIELDMAIDEN, 30.0f, true))
                            {
                                l_Trigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                                if (l_Trigger->IsAIEnabled)
                                    l_Trigger->AI()->AttackStart(l_Target);
                            }

                            AttackStart(l_Target);
                        }*/

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        m_IntroEvent = true;
                        break;
                    }
                    case EVENT_TRASH_INTRO_2:
                    {
                        if (m_IntroEvent)
                            break;

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                        if (GameObject* l_GameObject = me->FindNearestGameObject(GO_KVALDIR_CAGE_2, 20.0f))
                            l_GameObject->SetGoState(GO_STATE_ACTIVE);

                        if (Unit* l_Target = me->FindNearestPlayer(100.0f))
                        {
                            if (Creature* l_Trigger = me->FindNearestCreature(NPC_RUNECARVER, 20.0f, true))
                            {
                                l_Trigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                                if (l_Trigger->IsAIEnabled)
                                    l_Trigger->AI()->AttackStart(l_Target);
                            }

                            AttackStart(l_Target);
                        }

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        Talk(3);
                        m_IntroEvent = true;
                        break;
                    }
                    case EVENT_FRACTURE:
                    {
                        DoCastVictim(SPELL_FRACTURE);
                        m_Events.ScheduleEvent(EVENT_FRACTURE, 15000);
                        break;
                    }
                    case EVENT_BARBED:
                    {
                        DoCast(SPELL_BARBED_SPEAR);
                        m_Events.ScheduleEvent(EVENT_BARBED, 9000);
                        break;
                    }
                    case EVENT_SWIRLING:
                    {
                        DoCast(SPELL_SWIRLING_MUCK);
                        m_Events.ScheduleEvent(EVENT_SWIRLING, 17000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap m_Events;
                InstanceScript* m_Instance;
                bool m_IntroEvent;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_refereeAI(p_Creature);
        }
};

/// Power - 167922
class spell_ymiron_power_regen : public SpellScriptLoader
{
    public:
        spell_ymiron_power_regen() : SpellScriptLoader("spell_ymiron_power_regen") { }

        class spell_ymiron_power_regen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ymiron_power_regen_AuraScript);

            uint8 m_Power;
            uint8 m_PowerCounter;

            bool Load() override
            {
                m_PowerCounter = 0;
                return true;
            }

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Ymiron = GetCaster()->ToCreature())
                {
                    m_Power = l_Ymiron->GetPower(POWER_MANA);

                    if (m_Power < 100)
                    {
                        if (m_PowerCounter < 2)
                        {
                            m_PowerCounter++;
                            l_Ymiron->SetPower(POWER_MANA, m_Power + 7);
                        }
                        else
                        {
                            m_PowerCounter = 0;
                            l_Ymiron->SetPower(POWER_MANA, m_Power + 6);
                        }
                    }
                    else
                    {
                        l_Ymiron->SetPower(POWER_MANA, 0);

                        if (l_Ymiron->IsAIEnabled)
                            l_Ymiron->AI()->DoAction(ACTION_1); ///< EVENT_DARK_SLASH
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ymiron_power_regen_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ymiron_power_regen_AuraScript();
        }
};

/// Bane - 193460
class spell_ymiron_bane : public SpellScriptLoader
{
    public:
        spell_ymiron_bane() : SpellScriptLoader("spell_ymiron_bane") { }

        class spell_ymiron_bane_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ymiron_bane_AuraScript);

            void OnPeriodic(AuraEffect const* p_AurEff)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                auto l_SpawnBane = [this]() -> void
                {
                    Position l_Position;
                    GetCaster()->GetPosition(&l_Position);

                    float l_Angle = (float)rand_norm() * static_cast<float>(2 * M_PI);
                    float l_Dist = frand(11.0f, 30.0f);

                    l_Position.m_positionX = l_Position.m_positionX + l_Dist * std::cos(l_Angle);
                    l_Position.m_positionY = l_Position.m_positionY + l_Dist * std::sin(l_Angle);

                    GetCaster()->CastSpell(l_Position, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
                };

                int8 l_SpawnNumber = p_AurEff->GetTickNumber() == 1 ? 4 : 1;
                for (int8 l_I = 0; l_I < l_SpawnNumber; l_I++)
                    l_SpawnBane();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ymiron_bane_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ymiron_bane_AuraScript();
        }
};

/// Helheim - 191560
class spell_ymiron_helheim_teleport : public SpellScriptLoader
{
    public:
        spell_ymiron_helheim_teleport() : SpellScriptLoader("spell_ymiron_helheim_teleport") { }

        class spell_ymiron_helheim_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ymiron_helheim_teleport_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Player* l_Player = GetHitUnit()->ToPlayer();
                if (!l_Player)
                    return;

                if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
                {
                    l_Instance->SendWeatherToPlayers(WeatherState::WEATHER_STATE_RAIN_DROP_01, 0.4f, nullptr, true);

                    if (GameObject* l_Ship = GameObject::GetGameObject(*l_Player, l_Instance->GetData64(DATA_SHIP)))
                        l_Ship->SendUpdateToPlayer(l_Player);

                    l_Player->PlayStandaloneScene(1686, 17, { 7382.421f, 7278.579f, 43.78862f, 5.27222f });

                    l_Player->TeleportTo(1492, g_PlayerTeleportPos);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ymiron_helheim_teleport_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ymiron_helheim_teleport_SpellScript();
        }
};

/// Defiant Strike - 195031
class spell_ymiron_defiant_strike : public SpellScriptLoader
{
    public:
        spell_ymiron_defiant_strike() : SpellScriptLoader("spell_ymiron_defiant_strike") { }

        class spell_ymiron_defiant_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ymiron_defiant_strike_AuraScript);

            enum eSpells
            {
                SpellDefiantStrikeSmall = 195033,
                SpellDefiantStrikeBig   = 195036
            };

            uint8 m_Tick = 0;

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_Tick == 1 || m_Tick == 3 || m_Tick == 5)
                    l_Caster->CastSpell(l_Caster, eSpells::SpellDefiantStrikeSmall, true);

                ++m_Tick;
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SpellDefiantStrikeBig, false);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ymiron_defiant_strike_AuraScript::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ymiron_defiant_strike_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ymiron_defiant_strike_AuraScript();
        }
};

class player_script_helheim_scene : public PlayerScript
{
    public:
        player_script_helheim_scene() : PlayerScript("player_script_helheim_scene") { }

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(1686) == p_SceneInstanceID)
                p_Player->TeleportTo(1492, g_PlayerTeleportPos);
        }

        void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(1686) == p_SceneInstanceID)
                p_Player->TeleportTo(1492, g_PlayerTeleportPos);
        }
};

/// Ymiron RP Eventobject
class eventobject_ymiron_mos_intro : public EventObjectScript
{
public:
    eventobject_ymiron_mos_intro() : EventObjectScript("eventobject_ymiron_mos_intro")
    {
    }

    bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/) override
    {
        if (InstanceScript* l_Instance = p_Player->GetInstanceScript())
        {
            if (l_Instance->GetData(eData::DATA_YMIRON_RP_EVENT) == EncounterState::DONE)
                return false;

            l_Instance->SetData(eData::DATA_YMIRON_RP_EVENT, EncounterState::DONE);

            Unit* l_Ymiron = Unit::GetUnit(*p_Player, l_Instance->GetData64(eData::DATA_YMIRON));

            if (l_Ymiron && l_Ymiron->IsAIEnabled)
                l_Ymiron->GetAI()->DoAction(ACTION_2);
        }

        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_ymiron_the_fallen_king()
{
    /// Boss
    new boss_ymiron_the_fallen_king();

    /// Creatures
    new npc_trigger_mos_start();
    new npc_ymiron_risen_warrior();
    new npc_referee();

    /// Spells
    new spell_ymiron_power_regen();
    new spell_ymiron_bane();
    new spell_ymiron_helheim_teleport();
    new spell_ymiron_defiant_strike();

    /// PlayerScript
    new player_script_helheim_scene();
    new eventobject_ymiron_mos_intro();
}
#endif
