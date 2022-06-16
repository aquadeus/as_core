////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

enum Says
{
    SAY_AGGRO           = 2,
    SAY_DEATH           = 7,
};

enum Spells
{
    SPELL_FINGER_LASER              = 201159, //Delta Finger Laser X-treme!!!
    SPELL_ELEMENTIUM_BOMB           = 201240,
    SPELL_OVERLOADED_E_BOMB         = 201432,
    SPELL_ROCKET_CHICKEN            = 201392,
    SPELL_REINFORCED_ROCKET_CHICKEN = 201438,
    SPELL_MILLIFICENTS_RAGE         = 201572,

    //Summons
    SPELL_EXPLOSION                 = 201291,
    SPELL_DISARMING                 = 201265,
    SPELL_EJECT_ALL_PASSENGERS      = 158747,
    SPELL_SQUIRREL_SEARCH           = 201305,
    SPELL_SQUIRREL_CHARGE           = 201302,
    SPELL_SQUIRREL_EXPLOSION        = 201303,
    SPELL_OVERLOADED                = 201434,
    SPELL_OVERLOADED_SUM            = 201436,

    SPELL_CHICKEN_SWARM_ROCKETS     = 201356,
    SPELL_SWARM_ROCKETS_SEARCH      = 201386,
    SPELL_SWARM_ROCKETS_DMG         = 201387,
    SPELL_ROCKET_CHICKEN_ROCKET     = 201369,
    SPELL_THORIUM_PLATING           = 201441,

    //?
    SPELL_MILLIFICENTS_IRE          = 208114,
    SPELL_HOLOGRAMMIFICATION        = 201239,

    SPELL_BONUS_LOOT                = 226660
};

enum eEvents
{
    EVENT_FINGER_LASER              = 1,
    EVENT_ELEMENTIUM_BOMB           = 2,
    EVENT_ROCKET_CHICKEN            = 3,
};

Position const g_ImageSpawnPos = { 4627.0f, 4060.36f, 82.63f, 5.46f };

/// Millificent Manastorm <Engineering Genius> - 102616
class boss_millificent_manastorm : public CreatureScript
{
public:
    boss_millificent_manastorm() : CreatureScript("boss_millificent_manastorm") {}

    struct boss_millificent_manastormAI : public BossAI
    {
        boss_millificent_manastormAI(Creature* creature) : BossAI(creature, DATA_MILLIFICENT) 
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            removeloot = false;
        }

        bool enrage;
        bool removeloot;

        uint32 timer;
        uint32 phase;
        bool event;
        bool end;
        bool check;
        uint64 imageGUID;

        void Reset() override
        {
            _Reset();
            enrage = false;

            if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                me->SetReactState(REACT_DEFENSIVE);

            timer = 0;
            phase = 0;
            event = false;
            end = false;
            check = false;
            imageGUID = 0;
        }

        void EnterCombat(Unit* /*who*/) override
        {
            ZoneTalk(SAY_AGGRO);
            _EnterCombat();

            events.ScheduleEvent(EVENT_FINGER_LASER, 6000); //47:53, 48:00, 48:09, 48:19, 48:27
            events.ScheduleEvent(EVENT_ELEMENTIUM_BOMB, 7000); //47:54, 48:15, 48:33, 48:51
            events.ScheduleEvent(EVENT_ROCKET_CHICKEN, 24000); //48:11, 48:30, 48:48
            phase = 4;
            timer = 5000;
            event = true;

            if (!imageGUID)
            {
                if (Creature* l_Image = me->SummonCreature(NPC_IMAGE_OF_MILLHOUSE_MANASTORM, g_ImageSpawnPos))
                {
                    l_Image->CastSpell(l_Image, SPELL_HOLOGRAMMIFICATION);
                    imageGUID = l_Image->GetGUID();
                }
            }
        }

        void EnterEvadeMode() override
        {
            CreatureAI::EnterEvadeMode();

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
        }

        void JustDied(Unit* /*killer*/) override
        {
            //ZoneTalk(SAY_DEATH);
            _JustDied();

            if (removeloot)
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, SpellInfo const* p_SpellInfo) override
        {
            /// Prevent unwanted death
            if (end && damage >= me->GetHealth())
            {
                me->SetHealth(1);
                damage = 0;
                return;
            }

            if (me->HealthBelowPct(45) && !enrage)
            {
                enrage = true;
                if (!imageGUID)
                    DoCast(me, SPELL_MILLIFICENTS_RAGE, true);
                phase = 8;
                timer = 1000;
                event = true;
            }

            if (Creature* image = Unit::GetCreature(*me, imageGUID))
                if (me->HealthBelowPct(10) && !end)
                {
                    end = true;
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                    image->AI()->ZoneTalk(7);
                    DoCast(202310); // stun
                    phase = 13;
                    timer = 4000;
                    event = true;
                }
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_REMOVE_LOOT)
            {
                removeloot = true;
                me->SetVisible(true);
            }

            if (action == 1)
            {
                timer = 6000;
                phase = 1;

                if (Creature* l_Image = me->SummonCreature(NPC_IMAGE_OF_MILLHOUSE_MANASTORM, g_ImageSpawnPos))
                {
                    l_Image->CastSpell(l_Image, SPELL_HOLOGRAMMIFICATION);
                    imageGUID = l_Image->GetGUID();
                }

                ZoneTalk(0);
                event = true;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 2 && !check)
            {
                check = true;
                DoCast(201581);
                if (Creature* image = Unit::GetCreature(*me, imageGUID))
                    image->AI()->ZoneTalk(8);
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == 201581 && instance)
            {
                me->GetMotionMaster()->MovePoint(2, 4555.65f, 4014.96f, 83.67f);
                phase = 15;
                timer = 8000;
                event = true;

                if (!removeloot)
                    me->SummonGameObject(GO_MILLIFICENT_DISCARDED_LOCKBOX, *me, Position());

                CastSpellToPlayers(me, SPELL_BONUS_LOOT, true);

                instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
                instance->SetBossState(DATA_MILLIFICENT, DONE);
                instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);
                instance->DoCombatStopOnPlayers();
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (event)
            {
                Creature* image = Unit::GetCreature(*me, imageGUID);
                if (!image)
                    return;

                    switch(phase)
                    {
                        case 1:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(0);
                            phase++;
                            timer = 8000;
                            }
                            else timer -= diff;
                            break;
                        case 2:
                        if (timer < diff) {
                            ZoneTalk(1);
                            phase++;
                            timer = 7000;
                            }
                            else timer -= diff;
                            break;
                        case 3:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(1);
                            phase = 0;
                            timer = 8000;
                            event = false;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC|UNIT_FLAG_NON_ATTACKABLE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            }
                            else timer -= diff;
                            break;
                        case 4:
                        if (timer < diff) {
                            ZoneTalk(3);
                            phase++;
                            timer = 5000;
                            }
                            else timer -= diff;
                            break;
                        case 5:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(2);
                            phase++;
                            timer = 13000;
                            }
                            else timer -= diff;
                            break;
                        case 6:
                        if (timer < diff) {
                            ZoneTalk(4);  // Here comes the heat!
                            phase++;
                            timer = 5000;
                            }
                            else timer -= diff;
                            break;
                        case 7:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(3);
                            phase = 0;
                            timer = 6000;
                            event = false;
                            }
                            else timer -= diff;
                            break;
                        case 8:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(4);
                            phase++;
                            timer = 6000;
                            }
                            else timer -= diff;
                            break;   
                        case 9:
                        if (timer < diff) {
                            ZoneTalk(5);
                            DoCast(me, SPELL_MILLIFICENTS_RAGE, true);
                            phase++;
                            timer = 5000;
                            }
                            else timer -= diff;
                            break;
                        case 10:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(5);
                            phase++;
                            timer = 6000;
                            }
                            else timer -= diff;
                            break;
                        case 11:
                        if (timer < diff) {
                            ZoneTalk(6);
                            phase++;
                            timer = 4000;
                            }
                            else timer -= diff;
                            break;
                        case 12:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(6);
                            phase = 0;
                            timer = 6000;
                            event = false;
                            }
                            else timer -= diff;
                            break;
                        case 13:
                        if (timer < diff) {
                            summons.DespawnAllExceptEntries({ NPC_IMAGE_OF_MILLHOUSE_MANASTORM });
                            ZoneTalk(7);
                            me->RemoveAurasDueToSpell(SPELL_MILLIFICENTS_RAGE);
                            phase++;
                            timer = 2000;
                            }
                            else timer -= diff;
                            break;
                        case 14:
                        if (timer < diff) {
                            me->GetMotionMaster()->MovePoint(2, 4596.99f, 4014.73f, 83.31f);
                            phase = 0;
                            timer = 5000;
                            event = false;
                            }
                            else timer -= diff;
                            break;
                        case 15:
                        if (timer < diff) {
                            image->AI()->ZoneTalk(9);
                            image->GetMotionMaster()->MovePoint(0, 4596.92f, 4015.52f, 83.31f);
                            phase++;
                            timer = 8000;
                            }
                            else timer -= diff;
                            break;
                        case 16:
                        if (timer < diff) {
                            image->SetOrientation(0.07f);
                            image->AI()->ZoneTalk(10);
                            image->DespawnOrUnsummon(6000);
                            me->SetVisible(false);
                            me->DespawnOrUnsummon();
                            phase = 0;
                            timer = 5000;
                            event = false;
                            }
                            else timer -= diff;
                            break;                            
                    }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (end)
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_FINGER_LASER:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(pTarget, SPELL_FINGER_LASER);
                        events.ScheduleEvent(EVENT_FINGER_LASER, 7000);
                        break;
                    case EVENT_ELEMENTIUM_BOMB:
                        if (!me->HasAura(SPELL_MILLIFICENTS_RAGE))
                            DoCast(SPELL_ELEMENTIUM_BOMB);
                        else
                            DoCast(SPELL_OVERLOADED_E_BOMB);
                        events.ScheduleEvent(EVENT_ELEMENTIUM_BOMB, 18000);
                        break;
                    case EVENT_ROCKET_CHICKEN:
                        if (!me->HasAura(SPELL_MILLIFICENTS_RAGE))
                            DoCast(SPELL_ROCKET_CHICKEN);
                        else
                            DoCast(SPELL_REINFORCED_ROCKET_CHICKEN);
                        events.ScheduleEvent(EVENT_ROCKET_CHICKEN, 18000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_millificent_manastormAI (creature);
    }
};

//102043, 102136
class npc_millificent_elementium_squirrel_bomb : public CreatureScript
{
public:
    npc_millificent_elementium_squirrel_bomb() : CreatureScript("npc_millificent_elementium_squirrel_bomb") {}

    struct npc_millificent_elementium_squirrel_bombAI : public ScriptedAI
    {
        npc_millificent_elementium_squirrel_bombAI(Creature* creature) : ScriptedAI(creature), m_Vehicle(creature->GetVehicleKit())
        {
            me->SetReactState(REACT_PASSIVE);

            m_OriginalSquirrel = false;
        }

        uint16 diedTimer;

        Vehicle* m_Vehicle;

        std::array<uint64, 3> m_SquirrelsGuids;

        bool m_OriginalSquirrel;

        void Reset() override
        {
            if (m_Vehicle != nullptr)
            {
                AddTimedDelayedOperation(10, [this]() -> void
                {
                    std::array<uint32, 3> l_Entries = { {102044, 102085, 102084} };

                    uint8 l_I = 0;
                    for (uint32 l_Entry : l_Entries)
                    {
                        if (Creature* l_Summon = me->SummonCreature(l_Entry, *me))
                        {
                            m_SquirrelsGuids[l_I++] = l_Summon->GetGUID();
                            l_Summon->EnterVehicle(me);
                            l_Summon->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        }
                    }

                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                    m_OriginalSquirrel = true;
                });
            }
        }

        void IsSummonedBy(Unit* summoner) override
        {
            DoCast(SPELL_EXPLOSION);
            diedTimer = 10500;

            if (me->GetEntry() == 102136)
                DoCast(me, SPELL_OVERLOADED, true);
        }

        void JustSummoned(Creature* p_Summon) override
        {
            if (me->GetEntry() == 102136 && m_OriginalSquirrel)
                p_Summon->CastSpell(p_Summon, SPELL_SQUIRREL_SEARCH, true);
        }

        void OnSpellClick(Unit* clicker) override
        {
            if (clicker->IsPlayer())
                clicker->CastSpell(me, SPELL_DISARMING);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_DISARMING)
            {
                me->RemoveAurasDueToSpell(SPELL_OVERLOADED);
                me->InterruptNonMeleeSpells(false);
                me->SendPlaySpellVisualKit(0, 63152);
                diedTimer = 2000;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            UpdateOperations(diff);

            if (diedTimer)
            {
                if (diedTimer <= diff)
                {
                    for (uint64 l_Guid : m_SquirrelsGuids)
                    {
                        if (Creature* l_Squirrel = Creature::GetCreature(*me, l_Guid))
                        {
                            l_Squirrel->ExitVehicle();
                            l_Squirrel->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            l_Squirrel->CastSpell(l_Squirrel, SPELL_SQUIRREL_SEARCH, true);
                        }
                    }

                    diedTimer = 0;
                    me->Kill(me);
                }
                else
                    diedTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_millificent_elementium_squirrel_bombAI(creature);
    }
};

//102103, 102139
class npc_millificent_thorium_rocket_chicken : public CreatureScript
{
public:
    npc_millificent_thorium_rocket_chicken() : CreatureScript("npc_millificent_thorium_rocket_chicken") {}

    struct npc_millificent_thorium_rocket_chickenAI : public ScriptedAI
    {
        npc_millificent_thorium_rocket_chickenAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint16 rocketCastTimer;

        void Reset() override
        {
            rocketCastTimer = 2000;
        }

        void IsSummonedBy(Unit* summoner) override
        {
            DoZoneInCombat(me, 120.0f);
            DoCast(me, SPELL_CHICKEN_SWARM_ROCKETS, true);

            if (me->GetEntry() == 102139)
                DoCast(me, SPELL_THORIUM_PLATING, true);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SWARM_ROCKETS_SEARCH)
                DoCast(me, SPELL_SWARM_ROCKETS_DMG, true);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (rocketCastTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                {
                    me->SetFacingToObject(pTarget);
                    DoCast(pTarget, SPELL_ROCKET_CHICKEN_ROCKET);
                }
                rocketCastTimer = 4000;
            }
            else
                rocketCastTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_millificent_thorium_rocket_chickenAI(creature);
    }
};

//102137 
class npc_millificent_mechanical_bomb_squirrel : public CreatureScript
{
public:
    npc_millificent_mechanical_bomb_squirrel() : CreatureScript("npc_millificent_mechanical_bomb_squirrel") {}

    struct npc_millificent_mechanical_bomb_squirrelAI : public ScriptedAI
    {
        npc_millificent_mechanical_bomb_squirrelAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);

            m_TargetGuid = 0;
        }

        uint64 m_TargetGuid;
        EventMap events;

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_SQUIRREL_SEARCH && target->IsPlayer())
            {
                m_TargetGuid = target->GetGUID();
                DoCast(target, SPELL_SQUIRREL_CHARGE, false);
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == EVENT_CHARGE_PREPATH)
            {
                if (Player* l_Target = Player::GetPlayer(*me, m_TargetGuid))
                    DoCast(l_Target, SPELL_SQUIRREL_EXPLOSION);

                events.ScheduleEvent(EVENT_1, 100);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->Kill(me);
                        break;
                    default:
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_millificent_mechanical_bomb_squirrelAI(creature);
    }
};

#ifndef __clang_analyzer__
void AddSC_boss_millificent_manastorm()
{
    new boss_millificent_manastorm();
    new npc_millificent_elementium_squirrel_bomb();
    new npc_millificent_thorium_rocket_chicken();
    new npc_millificent_mechanical_bomb_squirrel();
}
#endif
