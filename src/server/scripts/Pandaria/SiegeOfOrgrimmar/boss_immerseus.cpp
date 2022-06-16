#include "siege_of_orgrimmar.h"


enum eSpells
{
    //Immerseus
    SPELL_CORROSIVE_BLAST       = 143436,
    SPELL_SHA_BOLT              = 143293,
    SPELL_SWIRL                 = 143309,
    SPELL_SWIRL_AURA_DUMMY      = 113762,
    SPELL_SWIRL_DMG             = 143412,
    SPELL_MINI_SWIRL_AT         = 143410,
    SPELL_MINI_SWIRL_DMG        = 143413,
    SPELL_SEEPING_SHA           = 143286,
    SPELL_SEEPING_SHA_AT        = 143281,
    SPELL_SUBMERGE              = 139832,
    SPELL_SUBMERGE_2            = 143281,
    SPELL_BERSERK               = 64238,
    SPELL_SHA_SPLASH_DUMMY      = 130063,
    SPELL_SHA_SPLASH_AT         = 143298,
    //HM
    SPELL_SWELLING_CORRUPTION   = 143574,
    SPELL_SWELLING_CORRUPTION_S = 143581,
    //npc sha pool   
    SPELL_SHA_POOL              = 143462,
    //npc sha puddle
    SPELL_ERUPTING_SHA          = 143498,
    SPELL_SHA_RESIDUE           = 143459,//buff
    //npc contaminated puddle
    SPELL_CONGEALING            = 143540,//slow, self buff
    SPELL_ERUPTING_WATER        = 145377,
    SPELL_PURIFIED_RESIDUE      = 143524,//buff

    SPELL_ACHIEV_CREDIT         = 145889,
    SPELL_SPLIT_C_PUDDLE_SUM_V  = 143024, //Visual dest pos contaminated puddle
    SPELL_SPLUT_S_PUDDLE_SUM_V  = 143022, //Visual dest pos sha puddle
    SPELL_SPLIT_VISUAL          = 143020,
};

enum Events
{
    //Immerseus
    EVENT_CORROSIVE_BLAST       = 1,
    EVENT_SHA_BOLT              = 2,
    EVENT_SWIRL                 = 3,
    EVENT_INTRO_PHASE_TWO       = 4,
    //HM
    EVENT_SWELLING_CORRUPTION   = 5,
    EVENT_SHA_POOL              = 6,
    //Summons
    EVENT_START_MOVING          = 7,
    EVENT_CHECK_DIST            = 8,
    EVENT_RE_ATTACK             = 11,
    EVENT_START_ROTATE          = 12,
    EVENT_UPDATE_ROTATE         = 13,
    EVENT_UPDATE_POINT          = 14,
    EVENT_SPAWN                 = 15,
};

enum Actions
{
    //Immerseus
    ACTION_INTRO_PHASE_ONE      = 1,
    //Summons
    ACTION_SPAWN                = 2,
    ACTION_MOVE                 = 3,
    ACTION_RE_ATTACK_WITH_DELAY = 4,
    ACTION_LAUNCH_ROTATE        = 5,
    ACTION_RE_ATTACK_SWIRL      = 6,
    ACTION_SPAWN_WAVE           = 7,
};

enum SData
{
    //Immerseus
    DATA_SP_DONE                = 1,
    DATA_CP_DONE                = 2,
    DATA_P_FINISH_MOVE          = 3,
    DATA_SEND_F_P_COUNT         = 4,

    //Summons
    DATA_SEND_INDEX             = 5,
};

//puddle spawn pos
Position const psp[25] =
{
    {1417.32f, 658.89f, 246.8535f},
    {1404.85f, 665.43f, 246.8601f},
    {1390.78f, 677.95f, 246.8363f},
    {1365.75f, 703.08f, 246.8346f},
    {1358.45f, 718.70f, 246.8346f},
    {1354.80f, 733.24f, 246.8355f},
    {1353.71f, 771.01f, 246.8345f},
    {1357.54f, 784.59f, 246.8345f},
    {1365.33f, 801.40f, 246.8345f},
    {1396.49f, 830.86f, 246.8346f},
    {1409.99f, 836.37f, 246.8346f},
    {1428.83f, 840.66f, 246.8346f},
    {1440.99f, 844.04f, 246.8345f},
    {1454.44f, 842.49f, 246.8346f},
    {1474.15f, 835.67f, 246.8346f},
    {1492.23f, 825.11f, 246.8332f},
    {1520.68f, 798.13f, 246.8348f},
    {1526.22f, 783.98f, 246.8348f},
    {1529.01f, 771.96f, 246.8348f},
    {1529.04f, 730.04f, 246.7348f},
    {1525.13f, 718.31f, 246.8348f},
    {1517.43f, 701.80f, 246.8348f},
    {1487.90f, 673.78f, 246.8346f},
    {1478.02f, 665.65f, 246.8619f},
    {1462.59f, 657.93f, 246.8514f},
};

//100 - 75
uint32 const wave1[25] =
{
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
};

//75 - 50
uint32 const wave2[25] =
{
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
};

//50 - 30
uint32 const wave3[25] =
{
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
};

//30 - 15
uint32 const wave4[25] =
{
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
};

//15
uint32 const wave5[25] =
{
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_SHA_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
    NPC_CONTAMINATED_PUDDLE,
};

Position swirltriggerspawnpos[48] =
{
    {1503.11f, 790.54f, 246.83f,   0.0f},
    {1484.00f, 778.59f, 246.83f,   0.0f},
    {1517.04f, 784.59f, 246.83f,   0.0f},
    {1499.16f, 772.37f, 246.83f,   0.0f},
    {1477.98f, 760.18f, 246.83f,   0.0f},
    {1515.58f, 764.80f, 246.83f,   0.0f},
    {1477.57f, 743.81f, 246.83f,   0.0f},
    {1517.99f, 736.69f, 246.83f,   0.0f},
    {1490.39f, 731.63f, 246.83f,   0.0f},
    {1504.58f, 731.19f, 246.83f,   0.0f},
    {1517.66f, 718.20f, 246.83f,   0.0f},
    {1492.11f, 716.08f, 246.83f,   0.0f},
    {1463.28f, 715.59f, 246.8380f, 0.0f},
    {1487.96f, 680.39f, 246.8357f, 0.0f},
    {1467.84f, 695.48f, 246.8405f, 0.0f},
    {1470.54f, 679.32f, 246.8402f, 0.0f},
    {1453.25f, 691.09f, 246.8467f, 0.0f},
    {1449.36f, 701.75f, 246.2595f, 0.0f},
    {1439.94f, 713.27f, 246.2595f, 0.0f},
    {1441.28f, 678.05f, 246.2595f, 0.0f},
    {1430.18f, 698.05f, 246.8376f, 0.0f},
    {1416.14f, 674.73f, 246.8449f, 0.0f},
    {1417.18f, 711.23f, 246.8368f, 0.0f},
    {1404.28f, 694.41f, 246.8383f, 0.0f},
    {1398.79f, 724.17f, 246.8354f, 0.0f},
    {1369.23f, 706.64f, 246.8354f, 0.0f},
    {1378.66f, 725.02f, 246.8354f, 0.0f},
    {1401.93f, 744.52f, 246.8354f, 0.0f},
    {1362.98f, 733.04f, 246.8354f, 0.0f},
    {1373.13f, 745.15f, 246.8354f, 0.0f},
    {1381.15f, 762.37f, 246.8355f, 0.0f},
    {1359.80f, 770.43f, 246.8355f, 0.0f},
    {1402.84f, 766.08f, 246.8355f, 0.0f},
    {1381.12f, 778.54f, 246.8355f, 0.0f},
    {1373.18f, 792.70f, 246.8355f, 0.0f},
    {1396.71f, 781.58f, 246.8355f, 0.0f},
    {1424.31f, 788.55f, 246.8352f, 0.0f},
    {1439.69f, 796.66f, 246.8352f, 0.0f},
    {1453.77f, 790.09f, 246.8352f, 0.0f},
    {1401.69f, 800.04f, 246.8349f, 0.0f},
    {1421.91f, 804.12f, 246.8352f, 0.0f},
    {1437.24f, 815.48f, 246.8356f, 0.0f},
    {1450.44f, 808.34f, 246.8339f, 0.0f},
    {1470.79f, 798.65f, 246.8339f, 0.0f},
    {1409.69f, 821.80f, 246.8352f, 0.0f},
    {1431.02f, 826.63f, 246.8352f, 0.0f},
    {1460.02f, 829.52f, 246.8352f, 0.0f},
    {1488.93f, 826.60f, 246.8336f, 0.0f},
};

// 71543
struct boss_immerseus : public BossAI
{
    boss_immerseus(Creature* creature) : BossAI(creature, DATA_IMMERSEUS)
    {
        instance = creature->GetInstanceScript();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
        if (Creature* cho = instance->instance->GetCreature(instance->GetData64(NPC_LOREWALKER_CHO)))
            cho->AI()->SetData(DATA_IMMERSEUS, IN_PROGRESS);
    }

    InstanceScript* instance;
    uint32 lasthp, berserk;
    uint8 donecp, donesp, maxpcount;
    std::vector<uint64> shapoollist;
    float lasthppct;
    bool phase_two;

    void Reset() override
    {
        _Reset();
        if (auto pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
            pp->RemoveAurasDueToSpell(SPELL_SEEPING_SHA_AT);
        me->SetFloatValue(OBJECT_FIELD_SCALE, 1.0f);
        me->SetReactState(REACT_DEFENSIVE);
        //me->RemoveAllNegativeAurasFromBoss();
        me->RemoveAurasDueToSpell(SPELL_SWIRL_AURA_DUMMY);
        me->RemoveAurasDueToSpell(SPELL_SHA_POOL);
        me->RemoveAurasDueToSpell(SPELL_BERSERK);
        me->RemoveAurasDueToSpell(SPELL_SPLIT_VISUAL);
        me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
        me->RemoveAurasDueToSpell(SPELL_SUBMERGE_2);
        me->RemoveAurasDueToSpell(SPELL_SWELLING_CORRUPTION);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->setPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 100);
        lasthp = me->GetMaxHealth();
        lasthppct = me->GetHealthPct();
        shapoollist.clear();
        phase_two = false;
        berserk = 0;
        donecp = 0;
        donesp = 0;
        maxpcount = 0;
    }

    void JustSummoned(Creature* sum) override
    {
        switch (sum->GetEntry())
        {
        case NPC_SHA_POOL:
            shapoollist.push_back(sum->GetGUID());
            break;
        case NPC_CONGEALED_SHA:
            DoZoneInCombat(sum, 100.0f);
            break;
        }
        summons.Summon(sum);
    }

    void EnterCombat(Unit* who) override
    {
        _EnterCombat();
        berserk = 600000;
        if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
            pp->CastSpell(pp, SPELL_SEEPING_SHA_AT, true);
        events.ScheduleEvent(EVENT_SHA_BOLT, 6000);
        events.ScheduleEvent(EVENT_CORROSIVE_BLAST, 9000);
        events.ScheduleEvent(EVENT_SWIRL, 14000);
        if (me->GetMap()->IsHeroic())
            events.ScheduleEvent(EVENT_SWELLING_CORRUPTION, 12000);
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
        case DATA_SP_DONE:
            maxpcount++;
            donesp++;
            UpdatePower();
            break;
        case DATA_CP_DONE:
            maxpcount++;
            donecp++;
            UpdatePower();
            break;
        case DATA_P_FINISH_MOVE:
            maxpcount++;
            break;
        }
    }

    void UpdatePower()
    {
        auto newValue = [=]() -> int32
        {
            if (sWorld->getBoolConfig(CONFIG_IS_TEST_SERVER) && me->GetPower(POWER_ENERGY) >= 2)
                int32(me->GetPower(POWER_ENERGY) - 2);
            if (me->GetPower(POWER_ENERGY) >= 1)
                return int32(me->GetPower(POWER_ENERGY) - 1);
            return int32(0);
        }();

        if (me->GetPower(POWER_ENERGY) >= 1)
            me->SetPower(POWER_ENERGY, newValue);
        if (me->GetMap()->IsHeroic())
            me->SetFloatValue(OBJECT_FIELD_SCALE, me->GetFloatValue(OBJECT_FIELD_SCALE) + 0.5f);
    }

    uint32 GetData(uint32 type) override
    {
        if (type == DATA_SEND_F_P_COUNT)
            return maxpcount;
        else
            return 0;
    }

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* spell) override
    {
        if (damage >= me->GetHealth() && !phase_two)
        {
            me->InterruptNonMeleeSpells(true);
            if (!summons.empty())
                summons.DespawnEntry(NPC_SWIRL_TRIGGER);
            damage = 0;
            phase_two = true;
            events.Reset();
            me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
            me->StopAttack();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveAurasDueToSpell(SPELL_SWELLING_CORRUPTION);
            DoCast(me, SPELL_SPLIT_VISUAL);
            me->SetFullHealth();
            if (!shapoollist.empty())
            {
                for (std::vector<uint64>::const_iterator guid = shapoollist.begin(); guid != shapoollist.end(); guid++)
                    if (Creature* sp = me->GetCreature(*me, *guid))
                        sp->AI()->DoAction(ACTION_MOVE);
                shapoollist.clear();
            }
        }
        else if (phase_two)
            damage = 0;
    }

    void DoAction(int32 const action) override
    {
        switch (action)
        {
        case ACTION_SPAWN_WAVE:
            SpawnWave();
            break;
        case ACTION_RE_ATTACK_SWIRL:
            if (!summons.empty())
                summons.DespawnEntry(NPC_SWIRL_TARGET);
            events.ScheduleEvent(EVENT_RE_ATTACK, 1000);
            break;
        case ACTION_RE_ATTACK_WITH_DELAY:
            events.ScheduleEvent(EVENT_RE_ATTACK, 1500);
            break;
        case ACTION_INTRO_PHASE_ONE:
            if (me->GetPower(POWER_ENERGY) == 0)
            {
                //Done
                me->Kill(me, true);
                return;
            }

            uint8 doneval = donesp + donecp;
            if (doneval)
            {
                uint32 modh = me->CountPctFromMaxHealth(float(doneval));
                if (lasthp - modh > 0)
                    me->SetHealth(lasthp - modh);
                else
                    me->SetHealth(me->CountPctFromMaxHealth(1));
            }
            else
                me->SetHealth(lasthp);

            lasthp = me->GetHealth();
            lasthppct = me->GetHealthPct();
            me->RemoveAurasDueToSpell(SPELL_SHA_POOL);
            me->RemoveAurasDueToSpell(SPELL_SPLIT_VISUAL);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetReactState(REACT_AGGRESSIVE);
            me->SetInCombatWithZone();
            phase_two = false;
            if (me->GetMap()->IsHeroic())
                events.ScheduleEvent(EVENT_SWELLING_CORRUPTION, 12000);
            events.ScheduleEvent(EVENT_CORROSIVE_BLAST, 10000);
            events.ScheduleEvent(EVENT_SWIRL, 20000);
            events.ScheduleEvent(EVENT_SHA_BOLT, 6000);
            break;
        }
    }

    void JustDied(Unit* killer) override
    {
        me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
            pp->RemoveAurasDueToSpell(SPELL_SEEPING_SHA_AT);
        instance->DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEV_CREDIT, 0, me);
        if (killer == me || sWorld->getBoolConfig(CONFIG_IS_TEST_SERVER))
        {
            _JustDied();
            me->setFaction(35);
            if (!me->GetMap()->IsLFR())
                me->SummonGameObject(221776, 1441.22f, 821.749f, 246.836f, 4.727f, 0.0f, 0.0f, 0.701922f, -0.712254f, 604800);

            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            if (!players.isEmpty())
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (Player* pPlayer = itr->getSource())
                        me->GetMap()->ToInstanceMap()->PermBindAllPlayers(pPlayer);
        }
        BossAI::JustDied(killer);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!phase_two && !UpdateVictim())
            return;

        if (berserk)
        {
            if (berserk <= diff)
            {
                berserk = 0;
                DoCast(me, SPELL_BERSERK, true);
            }
            else
                berserk -= diff;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_CORROSIVE_BLAST:
                if (Unit* target = me->getVictim())
                {
                    me->SetFacingTo(me->GetAngle(target));
                    me->CastSpell(me, SPELL_CORROSIVE_BLAST);
                }
                events.ScheduleEvent(EVENT_CORROSIVE_BLAST, 17000);
                break;
            case EVENT_SHA_BOLT:
            {
                std::list<HostileReference*> threatlist = me->getThreatManager().getThreatList();
                if (!threatlist.empty())
                    for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); itr++)
                        if (Player* pl = me->GetPlayer(*me, (*itr)->getUnitGuid()))
                            DoCast(pl, SPELL_SHA_BOLT);
                events.ScheduleEvent(EVENT_SHA_BOLT, 10000);
                break;
            }
            case EVENT_SWIRL:
                me->StopAttack();
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                {
                    float ang = me->GetAngle(target);
                    me->SetFacingTo(ang);
                    if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
                    {
                        pp->SetFacingTo(ang);
                        float x, y;
                        GetPositionWithDistInOrientation(pp, 78.0f, ang, x, y);
                        if (Creature* st = me->SummonCreature(NPC_SWIRL_TARGET, x, y, me->GetPositionZ()))
                        {
                            DoCast(st, SPELL_SWIRL);
                            pp->AI()->SetGUID(st->GetGUID(), 1);
                            st->AI()->SetGUID(pp->GetGUID(), 2);
                        }
                        events.DelayEvents(16000);
                        for (uint8 n = 0; n < 48; n++)
                            me->SummonCreature(NPC_SWIRL_TRIGGER, swirltriggerspawnpos[n], TEMPSUMMON_TIMED_DESPAWN, 14000);
                    }
                }
                events.ScheduleEvent(EVENT_SWIRL, 48000);
                break;
            case EVENT_RE_ATTACK:
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetInCombatWithZone();
                break;
                //HM
            case EVENT_SWELLING_CORRUPTION:
            {
                int32 mod = me->GetPower(POWER_ENERGY) / 2;
                if (mod)
                    me->CastCustomSpell(SPELL_SWELLING_CORRUPTION, SPELLVALUE_AURA_STACK, mod, me, true);
                events.ScheduleEvent(EVENT_SWELLING_CORRUPTION, 75000);
                break;
            }
            case EVENT_SHA_POOL:
                DoCast(me, SPELL_SHA_POOL, true);
                break;
            }
        }
        if (!phase_two)
            DoMeleeAttackIfReady();
    }

    void SpawnWave()
    {
        if (lasthppct >= 75)
        {
            for (uint8 n = 0; n < 25; n++)
                me->CastSpell(psp[n].GetPositionX(), psp[n].GetPositionY(), psp[n].GetPositionZ(), wave1[n] == NPC_CONTAMINATED_PUDDLE ? SPELL_SPLIT_C_PUDDLE_SUM_V : SPELL_SPLUT_S_PUDDLE_SUM_V, true);
        }
        else if (lasthppct < 75 && lasthppct > 50)
        {
            for (uint8 n = 0; n < 25; n++)
                me->CastSpell(psp[n].GetPositionX(), psp[n].GetPositionY(), psp[n].GetPositionZ(), wave2[n] == NPC_CONTAMINATED_PUDDLE ? SPELL_SPLIT_C_PUDDLE_SUM_V : SPELL_SPLUT_S_PUDDLE_SUM_V, true);
        }
        else if (lasthppct <= 50 && lasthppct > 30)
        {
            for (uint8 n = 0; n < 25; n++)
                me->CastSpell(psp[n].GetPositionX(), psp[n].GetPositionY(), psp[n].GetPositionZ(), wave3[n] == NPC_CONTAMINATED_PUDDLE ? SPELL_SPLIT_C_PUDDLE_SUM_V : SPELL_SPLUT_S_PUDDLE_SUM_V, true);
        }
        else if (lasthppct <= 30 && lasthppct > 15)
        {
            for (uint8 n = 0; n < 25; n++)
                me->CastSpell(psp[n].GetPositionX(), psp[n].GetPositionY(), psp[n].GetPositionZ(), wave4[n] == NPC_CONTAMINATED_PUDDLE ? SPELL_SPLIT_C_PUDDLE_SUM_V : SPELL_SPLUT_S_PUDDLE_SUM_V, true);
        }
        else if (lasthppct <= 15)
        {
            for (uint8 n = 0; n < 25; n++)
                me->CastSpell(psp[n].GetPositionX(), psp[n].GetPositionY(), psp[n].GetPositionZ(), wave5[n] == NPC_CONTAMINATED_PUDDLE ? SPELL_SPLIT_C_PUDDLE_SUM_V : SPELL_SPLUT_S_PUDDLE_SUM_V, true);
        }
        if (me->GetMap()->IsHeroic())
            events.ScheduleEvent(EVENT_SHA_POOL, 6000);
        maxpcount = 0;
        donecp = 0;
        donesp = 0;
    }
};

//71548
struct npc_swirl_trigger : public ScriptedAI
{
    npc_swirl_trigger(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (InstanceScript*)pCreature->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        me->SetDisplayId(11686);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
    InstanceScript* pInstance;
    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_SPAWN, 500);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == 4)
            events.ScheduleEvent(EVENT_UPDATE_POINT, 500);
    }

    void EnterEvadeMode() override {}

    void EnterCombat(Unit* who) override {}

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* spell) override
    {
        if (damage >= me->GetHealth())
            damage = 0;
    }

    void GetPosInRadiusWithRandomOrientation(Unit* unit, float dist, float& x, float& y)
    {
        float mod = urand(0, 6);
        float orientation = mod <= 5 ? mod + float(urand(1, 9)) / 10 : mod;
        x = unit->GetPositionX() + (dist * cos(orientation));
        y = unit->GetPositionY() + (dist * sin(orientation));
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_SPAWN:
            {
                float x, y;
                GetPosInRadiusWithRandomOrientation(me, urand(3, 8), x, y);
                me->GetMotionMaster()->MoveCharge(x, y, me->GetPositionZ(), 5.0f, 4);
                me->CastSpell(me, SPELL_MINI_SWIRL_AT);
                break;
            }
            case EVENT_UPDATE_POINT:
                float x, y;
                GetPosInRadiusWithRandomOrientation(me, urand(3, 8), x, y);
                me->GetMotionMaster()->MoveCharge(x, y, me->GetPositionZ(), 5.0f, 4);
                break;
            }
        }
    }
};

//71550
struct npc_swirl_target : public ScriptedAI
{
    npc_swirl_target(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (InstanceScript*)pCreature->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        me->SetDisplayId(11686);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
    }

    InstanceScript* pInstance;
    EventMap events;
    uint64 ppGuid;

    void Reset()
    {
        events.Reset();
    }

    void SetGUID(uint64 guid, int32 id) override
    {
        if (id == 2)
            ppGuid = guid;
    }

    void EnterEvadeMode() override {}

    void EnterCombat(Unit* who) override {}

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* spell) override
    {
        if (damage >= me->GetHealth())
            damage = 0;
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == POINT_MOTION_TYPE)
            if (pointId == 1)
                events.ScheduleEvent(EVENT_UPDATE_POINT, 100);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_UPDATE_POINT)
            {
                if (Creature* pp = me->GetCreature(*me, ppGuid))
                {
                    float x, y;
                    GetPositionWithDistInOrientation(pp, 78.0f, pp->GetOrientation(), x, y);
                    me->GetMotionMaster()->MoveCharge(x, y, pp->GetPositionZ() + 2.0f, 15.0f, 1);
                }
            }
        }
    }
};

//71544
struct npc_sha_pool : public ScriptedAI
{
    npc_sha_pool(Creature* creature) : ScriptedAI(creature)
    {
        me->SetDisplayId(11686);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
    }

    EventMap events;

    void Reset() override
    {
        me->SetFloatValue(OBJECT_FIELD_SCALE, 2.0f);
        events.ScheduleEvent(EVENT_SHA_POOL, 1500);
    }

    void DoAction(int32 const action) override
    {
        if (action == ACTION_MOVE)
        {
            if (me->ToTempSummon())
                if (Unit* i = me->ToTempSummon()->GetSummoner())
                    me->GetMotionMaster()->MoveCharge(i->GetPositionX(), i->GetPositionY(), i->GetPositionZ(), 4.0f, 0);
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == POINT_MOTION_TYPE)
            if (pointId == 0)
                me->DespawnOrUnsummon();
    }

    void EnterEvadeMode() override {}

    void EnterCombat(Unit* who) override {}

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SHA_POOL)
                DoCast(me, SPELL_SHA_SPLASH_AT, true);
        }
    }
};

struct immerseus_puddleAI : public ScriptedAI
{
    immerseus_puddleAI(Creature* creature) : ScriptedAI(creature) {}

    void CalcPuddle(InstanceScript* instance, Creature* caller, uint32 callerEntry, bool done)
    {
        if (caller && instance)
        {
            if (Creature* i = caller->GetCreature(*caller, instance->GetData64(NPC_IMMERSEUS)))
            {
                if (done)
                {
                    switch (callerEntry)
                    {
                    case NPC_SHA_PUDDLE:
                        i->AI()->SetData(DATA_SP_DONE, 0);
                        break;
                    case NPC_CONTAMINATED_PUDDLE:
                        i->AI()->SetData(DATA_CP_DONE, 0);
                        break;
                    }
                }
                else
                    i->AI()->SetData(DATA_P_FINISH_MOVE, 0);

                caller->DespawnOrUnsummon();

                if (i->AI()->GetData(DATA_SEND_F_P_COUNT) >= 25)
                {
                    i->SetFloatValue(OBJECT_FIELD_SCALE, 1.0f);
                    i->AI()->DoAction(ACTION_INTRO_PHASE_ONE);
                }
            }
        }
    }
};

//71603
struct npc_sha_puddle : public immerseus_puddleAI
{
    npc_sha_puddle(Creature* creature) : immerseus_puddleAI(creature)
    {
        instance = creature->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    InstanceScript* instance;
    EventMap events;
    uint8 index;
    bool finish;

    void Reset() override
    {
        events.Reset();
        finish = false;
        index = 0;
        me->ModifyAuraState(AURA_STATE_UNKNOWN22, true);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->ToCreature())
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            events.ScheduleEvent(EVENT_START_MOVING, 1500);
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* spell) override
    {
        if (damage >= me->GetHealth())
            DoCast(SPELL_SHA_RESIDUE);
    }

    void JustDied(Unit* killer) override
    {
        CalcPuddle(instance, me, me->GetEntry(), true);
    }

    void EnterEvadeMode() override {}

    void EnterCombat(Unit* who) override {}

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_MOVING:
                if (instance)
                {
                    if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
                    {
                        me->GetMotionMaster()->MoveFollow(pp, 10.0f, 0.0f);
                        events.ScheduleEvent(EVENT_CHECK_DIST, 1000);
                    }
                }
                break;
            case EVENT_CHECK_DIST:
                if (instance)
                {
                    if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
                    {
                        if (me->GetDistance(pp) <= 18.0f && !finish)
                        {
                            finish = true;
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            DoCast(me, SPELL_ERUPTING_SHA);
                            CalcPuddle(instance, me, me->GetEntry(), false);
                        }
                        else if (me->GetDistance(pp) > 18.0f && !finish)
                            events.ScheduleEvent(EVENT_CHECK_DIST, 1000);
                    }
                }
                break;
            }
        }
    }
};

//71604
struct npc_contaminated_puddle : public immerseus_puddleAI
{
    npc_contaminated_puddle(Creature* creature) : immerseus_puddleAI(creature)
    {
        instance = creature->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    InstanceScript* instance;
    EventMap events;
    uint8 index;
    bool done, finish;

    void Reset() override
    {
        events.Reset();
        me->SetHealth(1);
        finish = false;
        done = false;
        index = 0;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->ToCreature())
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            events.ScheduleEvent(EVENT_START_MOVING, 1500);
        }
    }

    void EnterEvadeMode() override {}

    void EnterCombat(Unit* who) override {}

    void SendSlow(uint8 val)
    {
        switch (val)
        {
        case 1:
        case 2:
            me->SetSpeed(MOVE_RUN, 0.9f);
            break;
        case 3:
        case 4:
            me->SetSpeed(MOVE_RUN, 0.8f);
            break;
        case 5:
        case 6:
            me->SetSpeed(MOVE_RUN, 0.7f);
            break;
        case 7:
            me->SetSpeed(MOVE_RUN, 0.6f);
            break;
        default:
            break;
        }
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        for (uint8 n = 0; n < MAX_EFFECTS; n++)
        {
            if (spell->Effects[n].Effect == SPELL_EFFECT_HEAL || spell->Effects[n].Effect == SPELL_EFFECT_HEAL_PCT)
            {
                uint8 mod = (uint8)floor(me->GetHealthPct() / 10);
                {
                    if (!mod)
                        return;

                    if (mod > 7)
                        mod = 7;

                    if (!me->HasAura(SPELL_CONGEALING))
                    {
                        me->CastCustomSpell(SPELL_CONGEALING, SPELLVALUE_AURA_STACK, mod, me);
                        SendSlow(mod);
                    }
                    else
                    {
                        if (Aura* aura = me->GetAura(SPELL_CONGEALING))
                        {
                            if (aura->GetStackAmount() < mod)
                            {
                                aura->SetStackAmount(mod);
                                SendSlow(mod);
                            }
                        }
                    }
                }
            }
        }

        if (me->GetHealth() == me->GetMaxHealth() && !done)
        {
            done = true;
            DoCast(me, SPELL_PURIFIED_RESIDUE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_MOVING:
                if (instance)
                {
                    if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
                    {
                        me->GetMotionMaster()->MoveFollow(pp, 10.0f, 0.0f);
                        events.ScheduleEvent(EVENT_CHECK_DIST, 1000);
                    }
                }
                break;
            case EVENT_CHECK_DIST:
                if (instance)
                {
                    if (Creature* pp = me->GetCreature(*me, instance->GetData64(NPC_PUDDLE_POINT)))
                    {
                        if (me->GetDistance(pp) <= 18.0f && !finish)
                        {
                            finish = true;
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            DoCast(me, SPELL_ERUPTING_WATER);
                            CalcPuddle(instance, me, me->GetEntry(), done ? true : false);
                        }
                        else if (me->GetDistance(pp) > 18.0f && !finish)
                            events.ScheduleEvent(EVENT_CHECK_DIST, 1000);
                    }
                }
                break;
            }
        }
    }
};

//90000 new trigger, nedded for work SPELL_SEEPING_SHA_AT = 143281
struct npc_puddle_point : public ScriptedAI
{
    npc_puddle_point(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
        me->SetDisplayId(11686);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_REMOVE_CLIENT_CONTROL);
    }
    InstanceScript* instance;
    EventMap events;
    uint64 swirltargetGuid;

    void Reset() override
    {
        events.Reset();
        swirltargetGuid = 0;
    }

    void SetGUID(uint64 guid, int32 id) override
    {
        if (id == 1)
            swirltargetGuid = guid;
    }

    void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* spell) override
    {
        if (damage >= me->GetHealth())
            damage = 0;
    }

    void DoAction(int32 const action) override
    {
        if (action == ACTION_LAUNCH_ROTATE)
        {
            me->GetMotionMaster()->MoveRotate(15000, ROTATE_DIRECTION_RIGHT);
            events.ScheduleEvent(EVENT_START_ROTATE, 1000);
        }
    }

    void EnterCombat(Unit* who) override {}

    void EnterEvadeMode() override {}

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_START_ROTATE)
            {
                if (Creature* st = me->GetCreature(*me, swirltargetGuid))
                {
                    float x, y;
                    GetPositionWithDistInOrientation(me, 78.0f, me->GetOrientation(), x, y);
                    st->GetMotionMaster()->MoveCharge(x, y, me->GetPositionZ() + 2.0f, 15.0f, 1);
                }
            }
        }
    }
};

//143309
class spell_immerseus_swirl : public AuraScript
{
    PrepareAuraScript(spell_immerseus_swirl);

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            {
                if (Creature* pp = GetCaster()->GetCreature(*GetCaster(), instance->GetData64(NPC_PUDDLE_POINT)))
                {
                    GetCaster()->CastSpell(GetCaster(), SPELL_SWIRL_AURA_DUMMY, true);
                    pp->AI()->DoAction(ACTION_LAUNCH_ROTATE);
                }
            }
        }
    }

    void HandleEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (GetCaster()->isAlive() && GetCaster()->isInCombat())
            {
                GetCaster()->RemoveAurasDueToSpell(SPELL_SWIRL_AURA_DUMMY);
                GetCaster()->ToCreature()->AI()->DoAction(ACTION_RE_ATTACK_SWIRL);
            }
        }
    }

    void Register()
    {
        OnEffectApply += AuraEffectApplyFn(spell_immerseus_swirl::OnApply, EFFECT_1, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_immerseus_swirl::HandleEffectRemove, EFFECT_1, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
    }
};

class SwirlFilter
{
public:
    SwirlFilter(Unit* caster, Creature* target) : _caster(caster), _target(target) {}

    bool operator()(WorldObject* unit)
    {
        if (unit->IsInBetween(_caster, _target, 8.0f))
            return false;
        return true;
    }
private:
    Unit* _caster;
    Creature* _target;
};

//125925
class spell_immerseus_swirl_searcher : public SpellScript
{
    PrepareSpellScript(spell_immerseus_swirl_searcher);

    void FilterTarget(std::list<WorldObject*>& targets)
    {
        if (GetCaster())
        {
            if (Creature* st = GetCaster()->FindNearestCreature(NPC_SWIRL_TARGET, 100.0f, true))
                targets.remove_if(SwirlFilter(GetCaster(), st));
            else
                targets.clear();
        }
    }

    void HitHandler()
    {
        if (GetHitUnit())
            GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SWIRL_DMG, true);
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_swirl_searcher::FilterTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_immerseus_swirl_searcher::HitHandler);
    }
};

class ExactDistanceCheck
{
public:
    ExactDistanceCheck(WorldObject* source, float dist) : _source(source), _dist(dist) {}

    bool operator()(WorldObject* unit)
    {
        return _source->GetExactDist2d(unit) > _dist;
    }
private:
    WorldObject* _source;
    float _dist;
};

//143460
class spell_immerseus_sha_pool : public SpellScript
{
    PrepareSpellScript(spell_immerseus_sha_pool);

    void ScaleRange(std::list<WorldObject*>& targets)
    {
        targets.remove_if(ExactDistanceCheck(GetCaster(), 20.0f * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE)));
    }

    void HitHandler()
    {
        if (GetCaster() && GetHitUnit())
        {
            if ((GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE) - 0.8f) < 1.0f)
                GetCaster()->SetFloatValue(OBJECT_FIELD_SCALE, 1.0f);
            else
                GetCaster()->SetFloatValue(OBJECT_FIELD_SCALE, GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE) - 0.8f);
        }
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_sha_pool::ScaleRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_immerseus_sha_pool::HitHandler);
    }
};

//143461
class spell_immerseus_sha_pool_filter : public SpellScript
{
    PrepareSpellScript(spell_immerseus_sha_pool_filter);

    void ScaleRange(std::list<WorldObject*>& targets)
    {
        targets.remove_if(ExactDistanceCheck(GetCaster(), 20.0f * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE)));
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_sha_pool_filter::ScaleRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class CorrosiveBlastFilter
{
public:
    CorrosiveBlastFilter(Unit* caster) : _caster(caster) {}

    bool operator()(WorldObject* unit)
    {
        if (_caster->isInFront(unit, M_PI / 4))
            return false;
        return true;
    }
private:
    Unit* _caster;
};

//143436
class spell_immerseus_corrosive_blast : public SpellScript
{
    PrepareSpellScript(spell_immerseus_corrosive_blast);

    void FilterTarget(std::list<WorldObject*>& targets)
    {
        if (GetCaster())
            targets.remove_if(CorrosiveBlastFilter(GetCaster()));
    }

    void HandleAfterCast()
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_RE_ATTACK_WITH_DELAY);
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_corrosive_blast::FilterTarget, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_corrosive_blast::FilterTarget, EFFECT_1, TARGET_UNIT_CONE_ENEMY_104);
        AfterCast += SpellCastFn(spell_immerseus_corrosive_blast::HandleAfterCast);
    }
};

//143020
class spell_immerseus_split_visual : public SpellScript
{
    PrepareSpellScript(spell_immerseus_split_visual);

    void HandleScript(SpellEffIndex effIndex)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            if (GetCaster()->isAlive() && GetCaster()->isInCombat())
                GetCaster()->ToCreature()->AI()->DoAction(ACTION_SPAWN_WAVE);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_split_visual::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

#ifndef __clang_analyzer__
void AddSC_immerseus()
{
    RegisterCreatureAI(boss_immerseus);
    RegisterCreatureAI(npc_swirl_trigger);
    RegisterCreatureAI(npc_swirl_target);
    RegisterCreatureAI(npc_sha_pool);
    RegisterCreatureAI(npc_sha_puddle);
    RegisterCreatureAI(npc_contaminated_puddle);
    RegisterCreatureAI(npc_puddle_point);

    RegisterSpellScript(spell_immerseus_swirl_searcher);
    RegisterSpellScript(spell_immerseus_sha_pool);
    RegisterSpellScript(spell_immerseus_sha_pool_filter);
    RegisterSpellScript(spell_immerseus_corrosive_blast);
    RegisterSpellScript(spell_immerseus_split_visual);
    RegisterAuraScript(spell_immerseus_swirl);
}
#endif
