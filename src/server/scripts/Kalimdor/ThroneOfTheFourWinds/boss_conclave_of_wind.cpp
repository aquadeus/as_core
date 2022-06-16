#include "throne_of_the_four_winds.h"
#include "Vehicle.h"
#include "ScriptMgr.h"

enum AnshalTalk
{
    SAY_ANSHAL_AGGRO = 1, //It shall be I that earns the favor of our lord by casting out the intruders. My calmest wind shall still prove too much for them!
    SAY_ANSHAL_DEATH,     //Begone, outsider!
    SAY_ANSHAL_KILL,      //Your presence shall no longer defile our home!
    SAY_ANSHAL_LOW,       //My power grows feeble, brothers. I shamefully must rely on you for a time.
    SAY_ANSHAL_ENRAGE,    //You think to outrun the wind? A fatal mistake.
    SAY_CONCLAVE_ULTIMATE,//The power of our winds, UNLEASHED!
};

enum NezirTalk
{
    SAY_NEZIR_AGGRO = 1, //The honor of slaying the interlopers shall be mine, brothers! Their feeble bodies will freeze solid from my wind's icy chill!
    SAY_NEZIR_DEATH,     //Frozen solid.
    SAY_NEZIR_KILL,      //Another mortal has taken their last breath!
    SAY_NEZIR_LOW,       //Brothers, beware! These mortals are dangerous. I must pause and gather my strength.
    SAY_NEZIR_ENRAGE,    //You throw away your honor and flee as cowards? Then die!
};

enum RohashTalk
{
    SAY_ROHASH_AGGRO = 1,  //As I am the strongest wind, it shall be I that tears the invaders apart!
    SAY_ROHASH_DEATH,      //Mere dust...
    SAY_ROHASH_KILL,       //Blown away!
    SAY_ROHASH_LOW,        //The intruders stand fast, brothers, I cannot break them. Allow me a brief respite to strengthen my winds.
    SAY_ROHASH_ENRAGE,     //Why do you flee, mortals? There is nowhere you can run or hide here!
};

enum Spells
{

    SPELL_NO_REGEN                  = 78725,

    // Anshal
    SPELL_SOOTHING_BREEZE           = 86206, // 86205 - Naios must have spell scripted them. - there is no problem doing it this way, without db.
    SPELL_SOOTHING_BREEZE_SILENCE   = 86207,
    SPELL_SOOTHING_BREEZE_SUMMON    = 86204,
    SPELL_SOOTHING_BREEZE_VISUAL    = 86208,

    SPELL_NURTURE                   = 85422, // Base spell, trigger summon every sec.
    SPELL_NURTURE_SUMMON_TRIGGER    = 85425,
    SPELL_NURTURE_DUMMY_AURA        = 85428, // Trigger on self.
    SPELL_NURTURE_CREEPER_SUMMON    = 85429, // Trigger on self.

    SPELL_TOXIC_SPORES              = 86281,
    SPELL_DUMMY_FOR_NURTURE         = 89813, //Needed on boss before nurture on channel stops.

    // Nezir
    SPELL_ICE_PATCH                 = 86122,
    SPELL_ICE_PATCH_VISUAL          = 86107, //it is not a visual you idiot! it's the damn spell.
    SPELL_ICE_PATCH_AURA            = 86111,

    SPELL_PERMAFROST                = 86082, // 86081 to cast on targets. damn 86082 casts the 86081 on self boss!!
    SPELL_WIND_CHILL                = 84645,

    // Rohash
    SPELL_SLICING_GALE              = 86182,

    SPELL_WIND_BLAST                = 86193, //casts the other spell... 
    SPELL_WIND_BLAST_EFFECT         = 85483, //casted by this 85480

    SPELL_TORNADO_DUMMY             = 109442, //Dummy to warn target!
    SPELL_DAMAGE_TORNADO            = 86134,  //damage - adds an aura on the targets
    SPELL_TRI_TORNADO_SUMMON        = 86192,
    SPELL_TORNADO_VISUAL            = 87621,  //Deals the damage, makes the burn. - Needs to be scripted properly - 3yds range (Naios says 86134 is Hurricane Visual)
    SPELL_SLOW_SPIN                 = 99380,  //For Spinning and Wind Blast

    SPELL_STORM_SHIELD              = 93059,

    //Play Dead
    SPELL_DEAD                      = 81238, // Dead look.
    SPELL_SELF_ROOT                 = 42716,
    //    SPELL_FLIGHT                    = 97249,

    // All
    SPELL_GATHER_STRENGHT           = 86307,

    // Pre Combat Effects - see .h

    // Other
    SPELL_TELEPORT_VISUAL           = 83369,

    // Debuffs
    SPELL_WIND_DISTANCE_CHECKER     = 85763,

    SPELL_WITHERING_WINDS           = 89137,
    SPELL_WITHERING_WINDS_EFFECT    = 85576,
    SPELL_WITHERING_WINDS_DAMAGE    = 93168,

    SPELL_CHILLING_WINDS            = 89135,
    SPELL_CHILLING_WINDS_EFFECT     = 85578,
    SPELL_CHILLING_WINDS_DAMAGE     = 93163,

    SPELL_DEAFENING_WINDS           = 89136,
    SPELL_DEAFENING_WINDS_EFFECT    = 85573,
    SPELL_DEAFENING_WINDS_DAMAGE    = 93166,

    // Ultimates
    SPELL_ZEPHYR_ULTIMATE           = 84638,
    SPELL_ZEPHYR_SPEED_AURA         = 85734, // 85740 needs radius 10
    SPELL_SLEET_STORM_ULTIMATE      = 84644,
    SPELL_HURRICANE_ULTIMATE        = 84643,  // 84643 - cannot be cast needs to added as aura.
    SPELL_HURRICANE_VISUAL          = 86498,  // 74429    - 86492 (on player)
    SPELL_HURRICANE_CAST            = 86492,

    // Instance
    SPELL_WINDDRAFT                 = 84576,
    SPELL_JETSTREAM_PREVENT         = 89771,
    SPELL_BERSERK                   = 82395,
    SPELL_SLIPSTREAM                = 87740,
    SPELL_SLIPSTREAM_VISUAL         = 85063,

    SPELL_WIND_PRE_EFFECT_WARNING   = 96508,

    // Achievements
    SPELL_ACHIEVEMENT_CONCLAVE      = 88835,
};

enum ExistingMobs // more - see.h
{
    NPC_TORNADO                     = 46207,
    NPC_HURRICANE                   = 46419,
    NPC_RAVENOUS_TRIGGER            = 45813,
};

enum Events
{
    // Anshal
    EVENT_SOOTHING_BREEZE           = 1,
    EVENT_NURTURE,
    EVENT_ULTIMATE_ANSHAL,

    // Nezir
    EVENT_ICE_PATCH,
    EVENT_PERMAFROST,
    EVENT_WIND_CHILL,
    EVENT_SLEET_STORM_ULTIMATE,
    EVENT_ULTIMATE_NEZIR,

    // Rohash
    EVENT_SLICING_GALE,
    EVENT_WIND_BLAST,
    EVENT_TORNADO,
    EVENT_STORM_SHIELD,
    EVENT_ULTIMATE_ROHASH,

    //All
    EVENT_REVIVE,
    EVENT_GATHER_STRENGHT,

    //Hurricane
    EVENT_GRAB_PLAYER,
    EVENT_DESPAWN,
};

Position const HomePos[3] =
{
    {-48.556f,  1054.369f,  199.455f,   4.701710f},     // Anshal 
    {118.113f,  813.822f,   199.455f,   3.125420f},     // Nezir
    {-52.820f,  577.172f,   199.455f,   1.565960f},     // Rohash
};

class CouncilGameObject
{
public:
    CouncilGameObject() { }

    bool operator()(GameObject* go)
    {
        switch (go->GetEntry())
        {
        case GOB_WIND_DRAFTEFFECT_CENTER:
            go->SetGoState(GO_STATE_READY);
            break;

        case GOB_DEIJING_HEALING:
        case GOB_DEIJING_FROST:
        case GOB_DEIJING_TORNADO:
            go->SetGoState(GO_STATE_ACTIVE);
            break;

        case GO_FLOOR_ALAKIR:
            go->SetGoState(GO_STATE_READY);
            break;

        case GOB_WIND_DRAFTEFFECT_1:
        case GOB_WIND_DRAFTEFFECT_2:
        case GOB_WIND_DRAFTEFFECT_3:
        case GOB_WIND_DRAFTEFFECT_4:
        case GOB_WIND_DRAFTEFFECT_5:
        case GOB_WIND_DRAFTEFFECT_6:
        case GOB_WIND_DRAFTEFFECT_7:
        case GOB_WIND_DRAFTEFFECT_8:
            go->SetGoState(GO_STATE_READY);
            break;
        default:
            break;
        }

        return false;
    }
};

struct conclave_checkerAI : public ScriptedAI
{
    explicit conclave_checkerAI(Creature* creature) : ScriptedAI(creature) {}

    void SetData(uint32 data, uint32 value) override
    {
        data = value;
    }

    uint32 GetData(uint32 i) override
    {
        return data;
    }

private:
    uint32 data{};
};

struct boss_anshal : public conclave_checkerAI
{
    explicit boss_anshal(Creature* creature) : conclave_checkerAI(creature)
    {
        instance = creature->GetInstanceScript();
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
        me->SetMaxPower(POWER_MANA, 90);
    }

    InstanceScript* instance;
    EventMap events;

    bool gatherStormCast;
    uint16 victimChekTimer;

    void Reset() override
    {
        events.Reset();
        summons.DespawnAll();
        gatherStormCast = false;
        victimChekTimer = 500;
        me->SetPower(POWER_MANA, 0);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveAllAuras();
        if (instance)
        {
            instance->SetData(DATA_CONCLAVE_OF_WIND_EVENT, NOT_STARTED);
            instance->SetBossState(DATA_CONCLAVE_OF_WIND_EVENT, NOT_STARTED);
        }
        DoCast(me, SPELL_PRE_COMBAT_EFFECT_ANSHAL, true);
        DoCast(me, SPELL_NO_REGEN, true);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        me->NearTeleportTo(me->GetHomePosition());
        if (instance)
        {
            if (instance->GetData(DATA_CONCLAVE_OF_WIND_EVENT) == IN_PROGRESS)
                instance->SetData(DATA_CONCLAVE_OF_WIND_EVENT, FAIL);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
        }
    }

    void EnterCombat(Unit* who) override
    {
        if (instance)
        {
            instance->SetData(DATA_CONCLAVE_OF_WIND_EVENT, IN_PROGRESS);
            instance->SetBossState(DATA_CONCLAVE_OF_WIND_EVENT, IN_PROGRESS);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        DoStartNoMovement(me);
        events.ScheduleEvent(EVENT_SOOTHING_BREEZE, urand(16500, 19000));
        events.ScheduleEvent(EVENT_NURTURE, urand(7000, 11000));
        events.ScheduleEvent(EVENT_ULTIMATE_ANSHAL, 1000);

        if (Creature* nezir = instance->instance->GetCreature(instance->GetData64(DATA_NEZIR)))
            if (nezir && !nezir->isInCombat())
                nezir->AI()->DoZoneInCombat(nezir, 500.0f);

        if (Creature* rohash = instance->instance->GetCreature(instance->GetData64(DATA_ROHASH)))
            if (rohash && !rohash->isInCombat())
                rohash->AI()->DoZoneInCombat(rohash, 500.0f);
    }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_ANSHAL_DEATH);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        if (instance)
        {
            instance->SetData(DATA_CONCLAVE_OF_WIND_EVENT, DONE);
            instance->SetBossState(DATA_CONCLAVE_OF_WIND_EVENT, DONE);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
        summons.DespawnAll();
    }

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
    {
        if (attacker->GetGUID() == me->GetGUID())
            return;

        if (me->GetHealth(attacker) <= damage)
        {
            damage = 0;
            me->SetHealth(me->CountPctFromMaxHealth(1));

            Creature* nezir = instance->instance->GetCreature(instance->GetData64(DATA_NEZIR));
            Creature* rohash = instance->instance->GetCreature(instance->GetData64(DATA_ROHASH));

            if (nezir && rohash)
            {
                if (nezir->HealthBelowPct(3) && rohash->HealthBelowPct(3) && me->HealthBelowPct(3))
                {
                    // Bosses are done.
                    nezir->Kill(nezir);
                    me->Kill(me);
                    rohash->Kill(rohash);
                }
                else if (me->HealthBelowPct(3))
                {
                    if (!gatherStormCast)
                    {
                        gatherStormCast = true;
                        me->AddAura(SPELL_DEAD, me);
                        me->AddAura(SPELL_SELF_ROOT, me);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        DoCast(me, SPELL_GATHER_STRENGHT);
                        events.ScheduleEvent(EVENT_REVIVE, 60000);
                    }
                }
            }
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
        case NPC_SOOTHING_BREEZE:
            summon->SetInCombatWithZone();
            summon->SetReactState(REACT_PASSIVE);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            summon->AddAura(SPELL_SOOTHING_BREEZE_VISUAL, summon);
            summon->AddAura(SPELL_SOOTHING_BREEZE, summon);
            summon->AddAura(SPELL_SOOTHING_BREEZE_SILENCE, summon);
            DoStartNoMovement(summon);
            break;
        case NPC_RAVENOUS_CREEPER:
            DoZoneInCombat(summon);
            summon->SetReactState(REACT_AGGRESSIVE);
            summon->AddAura(SPELL_TOXIC_SPORES, summon);
            break;
        default:
            summon->AI()->DoZoneInCombat();
            break;
        }
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Talk(SAY_ANSHAL_KILL);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (victimChekTimer)
        {
            if (victimChekTimer <= diff)
            {
                victimChekTimer = 500;
                if (Unit* victim = me->getVictim())
                {
                    if (victim->GetDistance(me->GetHomePosition()) >= 50.0f)
                    {
                        DoResetThreat();
                        me->AddUnitState(UNIT_STATE_ROOT);
                    }
                    else
                    {
                        if (!me->HasAura(SPELL_SELF_ROOT))
                            me->ClearUnitState(UNIT_STATE_ROOT);
                    }
                }
                if (me->GetDistance(me->GetHomePosition()) >= 45.0f)
                    me->NearTeleportTo(me->GetHomePosition());
            }
            else
                victimChekTimer -= diff;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_SOOTHING_BREEZE:
                if (Creature* creeper = me->FindNearestCreature(NPC_RAVENOUS_CREEPER, 90.0f, true))
                    if (creeper)
                        DoCast(creeper, SPELL_SOOTHING_BREEZE_SUMMON);
                events.ScheduleEvent(EVENT_SOOTHING_BREEZE, 25000);
                break;

            case EVENT_NURTURE:
                DoCast(me, SPELL_NURTURE, true);
                me->AddAura(SPELL_DUMMY_FOR_NURTURE, me);
                events.ScheduleEvent(EVENT_NURTURE, urand(111000, 113000));
                break;

            case EVENT_ULTIMATE_ANSHAL:
                if (me->GetPower(POWER_MANA) == 90)
                {
                    me->NearTeleportTo(HomePos[0].GetPositionX(), HomePos[0].GetPositionY(), HomePos[0].GetPositionZ(), HomePos[0].GetOrientation());
                    me->AttackStop();
                    me->CastStop();
                    me->AddAura(SPELL_ZEPHYR_ULTIMATE, me);//drains power and does it nicely
                }
                else
                    me->SetPower(POWER_MANA, me->GetPower(POWER_MANA) + 1);

                if (me->GetPower(POWER_MANA) == 0)
                    Talk(SAY_ANSHAL_LOW);

                if (!SelectTarget(SELECT_TARGET_NEAREST, 0, 90.0f, true))
                {
                    if (!me->HasAura(SPELL_WITHERING_WINDS))
                    {
                        Talk(SAY_ANSHAL_ENRAGE);
                        DoCast(me, SPELL_WITHERING_WINDS, true);
                        me->AddAura(SPELL_WITHERING_WINDS_DAMAGE, me);
                        me->AddAura(SPELL_WITHERING_WINDS_EFFECT, me);
                    }
                } // else
                events.ScheduleEvent(EVENT_ULTIMATE_ANSHAL, 1000);
                break;

            case EVENT_REVIVE:
                me->RemoveAurasDueToSpell(SPELL_DEAD); // Dead Look
                me->RemoveAura(SPELL_SELF_ROOT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                break;
            default:
                break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

//
struct boss_nezir : public conclave_checkerAI
{
    explicit boss_nezir(Creature* creature) : conclave_checkerAI(creature), summons(me)
    {
        instance = creature->GetInstanceScript();
        me->SetMaxPower(POWER_MANA, 90);
    }

    InstanceScript* instance;
    EventMap events;
    SummonList summons;

    bool gatherStormCast;
    uint16 victimChekTimer;

    void Reset() override
    {
        events.Reset();
        summons.DespawnAll();
        gatherStormCast = false;
        victimChekTimer = 500;
        me->SetPower(POWER_MANA, 0);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveAllAuras();
        DoCast(me, SPELL_PRE_COMBAT_EFFECT_NEZIR, true);
        DoCast(me, SPELL_NO_REGEN, true);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        me->NearTeleportTo(me->GetHomePosition());
        if (instance)
        {
            if (instance->GetData(DATA_CONCLAVE_OF_WIND_EVENT) == IN_PROGRESS)
                instance->SetData(DATA_CONCLAVE_OF_WIND_EVENT, FAIL);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
        }
    }

    void EnterCombat(Unit* who) override
    {
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

        Talk(SAY_NEZIR_AGGRO);
        me->AddUnitState(UNIT_STATE_ROOT);

        events.ScheduleEvent(EVENT_ICE_PATCH, urand(10000, 12000));
        events.ScheduleEvent(EVENT_PERMAFROST, urand(20000, 23000));
        events.ScheduleEvent(EVENT_WIND_CHILL, 15000);
        events.ScheduleEvent(EVENT_SLEET_STORM_ULTIMATE, 30000);
        events.ScheduleEvent(EVENT_ULTIMATE_NEZIR, 1000);

        if (Creature* anshal = instance->instance->GetCreature(instance->GetData64(DATA_ANSHAL)))
            if (anshal && !anshal->isInCombat())
                anshal->AI()->DoZoneInCombat(anshal, 500.0f);

        if (Creature* rohash = instance->instance->GetCreature(instance->GetData64(DATA_ROHASH)))
            if (rohash && !rohash->isInCombat())
                rohash->AI()->DoZoneInCombat(rohash, 500.0f);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
        case NPC_ICE_PATCH:
            summon->SetInCombatWithZone();
            summon->SetReactState(REACT_PASSIVE);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            summon->AddAura(SPELL_ICE_PATCH_VISUAL, summon);
            summon->AddAura(SPELL_ICE_PATCH_AURA, summon);
            DoStartNoMovement(summon);
            break;
        default:
            return;
        }
    }

    bool AchieveAuraCheck()
    {
        auto const& players = me->GetMap()->GetPlayers();
        if (!players.isEmpty())
            for (auto const& itr : players)
                if (Player* player = itr.getSource())
                {
                    if (player->GetAuraCount(SPELL_WIND_CHILL) < 7)
                        return false;
                }

        return true;
    }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_NEZIR_DEATH);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

        summons.DespawnAll();

        if (AchieveAuraCheck())
            instance->DoCastSpellOnPlayers(94119);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Talk(SAY_NEZIR_KILL);
    }

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
    {
        if (attacker->GetGUID() == me->GetGUID())
            return;

        if (me->GetHealth(attacker) <= damage)
        {
            damage = 0;
            me->SetHealth(me->CountPctFromMaxHealth(1));

            Creature* anshal = instance->instance->GetCreature(instance->GetData64(DATA_ANSHAL));
            Creature* rohash = instance->instance->GetCreature(instance->GetData64(DATA_ROHASH));

            if (anshal && rohash)
            {
                if (me->HealthBelowPct(3) && rohash->HealthBelowPct(3) && anshal->HealthBelowPct(3))
                {
                    // Bosses are done.
                    anshal->Kill(anshal);
                    me->Kill(me);
                    rohash->Kill(rohash);
                }
                else if (me->HealthBelowPct(3))
                {
                    if (!gatherStormCast)
                    {
                        gatherStormCast = true;
                        me->AddAura(SPELL_DEAD, me);
                        me->AddAura(SPELL_SELF_ROOT, me);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        DoCast(me, SPELL_GATHER_STRENGHT);
                        events.ScheduleEvent(EVENT_REVIVE, 60000);
                    }
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ICE_PATCH:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    DoCast(target, SPELL_ICE_PATCH);
                events.ScheduleEvent(EVENT_ICE_PATCH, urand(10000, 12000));
                break;

            case EVENT_PERMAFROST:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_PERMAFROST);
                events.ScheduleEvent(EVENT_PERMAFROST, 20000);
                break;

            case EVENT_WIND_CHILL:
                DoCast(SPELL_WIND_CHILL);
                events.ScheduleEvent(EVENT_WIND_CHILL, 15000);
                break;

            case EVENT_ULTIMATE_NEZIR:
                if (me->GetPower(POWER_MANA) == 90)
                {
                    //me->NearTeleportTo(HomePos[1].GetPositionX(), HomePos[1].GetPositionY(), HomePos[1].GetPositionZ(), HomePos[1].GetOrientation());
                    me->AttackStop();
                    me->CastStop();
                    DoCast(me, SPELL_SLEET_STORM_ULTIMATE);//drains power and does it nicely
                }
                else
                    me->SetPower(POWER_MANA, me->GetPower(POWER_MANA) + 1);

                if (me->GetPower(POWER_MANA) == 0)
                    Talk(SAY_NEZIR_LOW);

                if (!SelectTarget(SELECT_TARGET_NEAREST, 0, 90.0f, true))
                {
                    if (!me->HasAura(SPELL_CHILLING_WINDS))
                    {
                        Talk(SAY_NEZIR_ENRAGE);
                        DoCast(me, SPELL_CHILLING_WINDS, true);
                        me->AddAura(SPELL_CHILLING_WINDS_DAMAGE, me);
                        me->AddAura(SPELL_CHILLING_WINDS_EFFECT, me);
                    }
                } //else
                events.ScheduleEvent(EVENT_ULTIMATE_NEZIR, 1000);
                break;

            case EVENT_REVIVE:
                me->RemoveAurasDueToSpell(SPELL_DEAD); // Dead Look
                me->RemoveAura(SPELL_SELF_ROOT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                break;
            default:
                break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

//
struct boss_rohash : public conclave_checkerAI
{
    explicit boss_rohash(Creature* creature) : conclave_checkerAI(creature), summons(me)
    {
        instance = creature->GetInstanceScript();
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
        me->SetMaxPower(POWER_MANA, 90);
    }

    InstanceScript* instance;
    EventMap events;
    SummonList summons;
    bool gatherStormCast;
    uint16 victimChekTimer;

    void Reset() override
    {
        events.Reset();
        gatherStormCast = false;
        victimChekTimer = 500;
        me->SetPower(POWER_MANA, 0);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveAllAuras();
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        DoCast(me, SPELL_NO_REGEN, true);
        DoCast(me, SPELL_PRE_COMBAT_EFFECT_ROHASH, true);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        me->NearTeleportTo(me->GetHomePosition());
        if (instance)
        {
            if (instance->GetData(DATA_CONCLAVE_OF_WIND_EVENT) == IN_PROGRESS)
                instance->SetData(DATA_CONCLAVE_OF_WIND_EVENT, FAIL);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
        }
    }

    void EnterCombat(Unit* who) override
    {
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

        DoStartNoMovement(me);
        Talk(SAY_ROHASH_AGGRO);
        events.ScheduleEvent(EVENT_SLICING_GALE, 10000);
        events.ScheduleEvent(EVENT_WIND_BLAST, 30000);
        events.ScheduleEvent(EVENT_TORNADO, 20000);
        events.ScheduleEvent(EVENT_STORM_SHIELD, 30000);
        events.ScheduleEvent(EVENT_ULTIMATE_ROHASH, 1000);

        if (Creature* nezir = instance->instance->GetCreature(instance->GetData64(DATA_NEZIR)))
            if (nezir && !nezir->isInCombat())
                nezir->AI()->DoZoneInCombat(nezir, 500.0f);

        if (Creature* anshal = instance->instance->GetCreature(instance->GetData64(DATA_ANSHAL)))
            if (anshal && !anshal->isInCombat())
                anshal->AI()->DoZoneInCombat(anshal, 500.0f);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
        case NPC_TORNADO:
            summon->SetSpeed(MOVE_RUN, 0.3f);
            summon->SetReactState(REACT_PASSIVE);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            summon->SetInCombatWithZone();
            summon->CastSpell(summon, SPELL_TORNADO_VISUAL, TRIGGERED_FULL_MASK);
            summon->AddAura(SPELL_DAMAGE_TORNADO, summon);
            summon->DespawnOrUnsummon(20000);
            break;
        }
    }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_ROHASH_DEATH);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

        summons.DespawnAll();
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Talk(SAY_ROHASH_KILL);
    }

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
    {
        if (attacker->GetGUID() == me->GetGUID())
            return;

        if (me->GetHealth(attacker) <= damage)
        {
            damage = 0;
            me->SetHealth(me->CountPctFromMaxHealth(1));

            Creature* anshal = instance->instance->GetCreature(instance->GetData64(DATA_ANSHAL));
            Creature* nezir = instance->instance->GetCreature(instance->GetData64(DATA_NEZIR));

            if (anshal && nezir)
            {
                if (me->HealthBelowPct(3) && nezir->HealthBelowPct(3) && anshal->HealthBelowPct(3))
                {
                    // Bosses are done.
                    anshal->Kill(anshal);
                    me->Kill(me);
                    nezir->Kill(nezir);
                }
                else if (me->HealthBelowPct(3))
                {
                    if (gatherStormCast == false)
                    {
                        me->AddAura(SPELL_DEAD, me);
                        me->AddAura(SPELL_SELF_ROOT, me);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        DoCast(me, SPELL_GATHER_STRENGHT);
                        events.ScheduleEvent(EVENT_REVIVE, 60000);
                        gatherStormCast = true;
                    }
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (victimChekTimer)
        {
            if (victimChekTimer <= diff)
            {
                victimChekTimer = 500;
                if (Unit* victim = me->getVictim())
                {
                    if (victim->GetDistance(me->GetHomePosition()) >= 50.0f)
                    {
                        DoResetThreat();
                        me->AddUnitState(UNIT_STATE_ROOT);
                    }
                    else
                    {
                        if (!me->HasAura(SPELL_SELF_ROOT))
                            me->ClearUnitState(UNIT_STATE_ROOT);
                    }
                }
                if (me->GetDistance(me->GetHomePosition()) >= 45.0f)
                    me->NearTeleportTo(me->GetHomePosition());
            }
            else
                victimChekTimer -= diff;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_WIND_BLAST:
                DoCast(me, SPELL_WIND_BLAST);
                me->GetMotionMaster()->MoveRotate(10000, urand(0, 1) ? ROTATE_DIRECTION_LEFT : ROTATE_DIRECTION_RIGHT);
                events.ScheduleEvent(EVENT_WIND_BLAST, 30000); // a rather long cooldown
                events.ScheduleEvent(EVENT_SLICING_GALE, 12000);
                break;

            case EVENT_SLICING_GALE:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_SLICING_GALE);
                if (me->HasAura(SPELL_HURRICANE_VISUAL))
                    me->RemoveAura(SPELL_HURRICANE_VISUAL);
                events.ScheduleEvent(EVENT_SLICING_GALE, 1500);
                break;

            case EVENT_TORNADO: //This is hawkward
                DoCast(me, SPELL_TRI_TORNADO_SUMMON);
                events.ScheduleEvent(EVENT_TORNADO, 45000);
                break;

            case EVENT_STORM_SHIELD:
                if (IsHeroic())
                    DoCast(me, SPELL_STORM_SHIELD);
                events.ScheduleEvent(EVENT_STORM_SHIELD, 105000); // 60s cd
                break;

            case EVENT_ULTIMATE_ROHASH:
                if (me->GetPower(POWER_MANA) == 90)
                {
                    me->CastStop();
                    me->NearTeleportTo(HomePos[2].GetPositionX(), HomePos[2].GetPositionY(), HomePos[2].GetPositionZ(), HomePos[2].GetOrientation());
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    //me->DespawnCreaturesInArea(NPC_TORNADO, 90.f);
                    DoCast(me, SPELL_HURRICANE_ULTIMATE);
                    me->AddAura(SPELL_HURRICANE_VISUAL, me);
                }
                else me->SetPower(POWER_MANA, me->GetPower(POWER_MANA) + 1);

                if (me->GetPower(POWER_MANA) == 0)
                    Talk(SAY_ROHASH_LOW);

                // if nobody is on platform
                if (!SelectTarget(SELECT_TARGET_NEAREST, 0, 90.0f, true))
                {
                    if (!me->HasAura(SPELL_DEAFENING_WINDS))
                    {
                        Talk(SAY_ROHASH_ENRAGE);
                        DoCast(me, SPELL_DEAFENING_WINDS, true);
                        me->AddAura(SPELL_DEAFENING_WINDS_DAMAGE, me);
                        me->AddAura(SPELL_DEAFENING_WINDS_EFFECT, me);
                    }
                }
                events.ScheduleEvent(EVENT_ULTIMATE_ROHASH, 1000);
                break;

            case EVENT_REVIVE:
                me->RemoveAurasDueToSpell(SPELL_DEAD); // Dead Look
                me->RemoveAura(SPELL_SELF_ROOT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                break;
            default:
                break;
            }
        }
    }
};

struct npc_tornado : public ScriptedAI
{
    explicit npc_tornado(Creature* creature) : ScriptedAI(creature)
    {
        TargetSelectTimer = 2000;
        creature->SetReactState(REACT_PASSIVE);
    }

    uint32 TargetSelectTimer;

    void UpdateAI(uint32 diff) override
    {
        if (TargetSelectTimer <= diff)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 90.0f, true))
            {
                me->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                me->AddAura(SPELL_TORNADO_DUMMY, target);
                TargetSelectTimer = 6000;
            }
        }
        else TargetSelectTimer -= diff;
    }
};

struct npc_creeper_trigger : public ScriptedAI
{
    explicit npc_creeper_trigger(Creature* creature) : ScriptedAI(creature)
    {
        CreeperSummonTimer = 8000;
        DoCast(creature, SPELL_NURTURE_DUMMY_AURA);
    }

    uint32 CreeperSummonTimer;

    void UpdateAI(uint32 diff) override
    {
        if (CreeperSummonTimer <= diff)
        {
            //DoCast(me, SPELL_NURTURE_CREEPER_SUMMON);
            me->SummonCreature(NPC_RAVENOUS_CREEPER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN);
            me->DespawnOrUnsummon(100);
            CreeperSummonTimer = 500;
        }
        else CreeperSummonTimer -= diff;
    }
};


struct npc_hurricane : public ScriptedAI
{
    explicit npc_hurricane(Creature* creature) : ScriptedAI(creature), grabbedPlayer() { }

    void Reset() override
    {
        events.Reset();
        me->SetReactState(REACT_PASSIVE);
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_GRAB_PLAYER, 500);
        events.ScheduleEvent(EVENT_DESPAWN, 15000);
    }

    void SetGUID(uint64 guid, int32 /* = 0*/) override
    {
        grabbedPlayer = guid;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_GRAB_PLAYER:
                if (!grabbedPlayer)
                {
                    if (Unit* target = GetPlayerAtMinimumRange(0.9f))
                    {
                        if (me->GetDistance(target) < 6.5f)
                            DoCast(target, SPELL_HURRICANE_CAST, true);
                        me->AddAura(SPELL_HURRICANE_CAST, target);
                        target->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 30.0f, target->GetOrientation());
                    }
                }
            case EVENT_DESPAWN:
                if (Unit* target = GetPlayerAtMinimumRange(45.0f))
                    if (target && target->HasAura(SPELL_HURRICANE_CAST))
                        me->RemoveAura(SPELL_HURRICANE_CAST);
                CouncilGameObject reset;
                JadeCore::GameObjectWorker<CouncilGameObject> worker(me, reset);
                me->VisitNearbyGridObject(400.0f, worker);
                me->DespawnOrUnsummon(500);
                break;
            }
        }

        // no melee attacks
    }

private:
    EventMap events;
    ObjectGuid grabbedPlayer;
};

class spell_hurricane_conclave : public AuraScript
{
    PrepareAuraScript(spell_hurricane_conclave);

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Unit* caster = GetCaster())
                target->EnterVehicle(caster, 0);
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (Vehicle* vehicle = caster->GetVehicleKit())
                vehicle->RemoveAllPassengers();
    }

    void Register()
    {
        OnEffectApply += AuraEffectApplyFn(spell_hurricane_conclave::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hurricane_conclave::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_conclave_of_wind()
{
    RegisterCreatureAI(boss_anshal);
    RegisterCreatureAI(boss_nezir);
    RegisterCreatureAI(boss_rohash);
    RegisterCreatureAI(npc_hurricane);
    RegisterCreatureAI(npc_tornado);
    RegisterCreatureAI(npc_creeper_trigger);
    RegisterAuraScript(spell_hurricane_conclave); // spell_hurricane
}